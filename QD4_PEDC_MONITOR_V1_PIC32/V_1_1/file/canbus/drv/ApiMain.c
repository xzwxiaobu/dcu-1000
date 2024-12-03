/*=============================================================================
* Project:		
* PEDC MCU:		PIC32MX795F512H
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.1
* Author:		lly
* Date:			2020-6-1
* =============================================
* ��	 ��	:	ApiMain.C
*			:	��������ã�ʵ��CANͨѶ����������
*     V0.1
*        1. ApiGetDevieceInfo ��Ϊ�����������
* =============================================================================*/
#include  "..\_g_canbus.h"

#include  "glbCan.h"
#include  "glbUpdate.h"
#include  "_glbApi.h"

#include  "CanTgt.h"
//#include  "upEEPROM.h"
#include  "..\ObjDict.h"
#include  "Update.h"

#if(CDRV_TYPE == CDRV_PEDC)
extern void UpFwStateToMMS(void);                     //����״̬���� TO MMS
#endif

//**********************************************************************************************
//                                                                                             *
//                                     Api ����                                                *
//                                                                                             *
//**********************************************************************************************
void ApiMainProcess()
{
	#if (CDRV_TYPE  == CDRV_PEDC)
		static int8u u8ChkIdOn1=ID_DCU_MIN;
		static int16u wTstCanOn=0;
		int8u   u8Flg;
		int16u u16Tmr;
	#endif

	//CAN ͨѶ���ݴ���, ��÷����ʼ
	CapiProcess();

	//PEDC or DCU ?
	#if (CDRV_TYPE  == CDRV_PEDC)
	{
		#if (UPDATE_FUN_EN == TRUE)
			if(UpdateProcess() == TRUE)
			{
				//if(u8UpdateState > UPFW_STATE_COPY)
					UpFwStateToMMS();                              //send message to MMS
			}
		#endif

		//MMS_UPDATE_STATE() : MST_STATE_IDLE ��ʾû������
		u8Flg = 0;
		u16Tmr = (int16u)(TgtGetTickCount () - wTstCanOn);
		if((ChkTransState(FUNID_CANMAN1) != fgRunning))
		{
			if(u16Tmr > 20)
			{
				u8Flg = 1;
			}
		}
		else if(u16Tmr > (int16u)(MASTER_DFT_OV+30))
		{
			u8Flg = 1;
		}

		if(u8Flg == 1)
		{
			CapiStartTrans(FUNID_CANMAN1, u8ChkIdOn1, GET_TRANS_LEN(FUNID_CANMAN1), GET_TRANS_BUF(FUNID_CANMAN1));
			//CapiStartTrans(FUNID_CANMAN1, 1, GET_TRANS_LEN(FUNID_CANMAN1), GET_TRANS_BUF(FUNID_CANMAN1));
			GET_TICK(wTstCanOn);

			DcuInc(&u8ChkIdOn1,MAX_DCU);			//u8ChkIdOn1++;

			if(u8ChkIdOn1 > (MAX_DCU + ID_DCU_MIN - 1))
			{
				u8ChkIdOn1 = ID_DCU_MIN;
			}
		}
	}

    /* �������ߵ�DCU���ڱ� ID ��ͻ 
     * ע��  SlvCan0Err / SlvCan1Err ��Ӧλ��0��ʾ����
     *     ��ֻҪ��һ��λΪ0 ���Ӧ�� DCU ������
     */
    int i;
    for(i = 0; i < sizeof(CanManage.IDConflict); i ++)
    {
        CanManage.IDConflict[i] &= ~(CanManage.SlvCan0Err[i] & CanManage.SlvCan1Err[i]);
    }
    
	//==============
	#else  //#if (CDRV_TYPE  == CDRV_PEDC)
	{
		#if (UPDATE_FUN_EN == TRUE)
			UpdateProcess();
		#endif

		CanOnLine[0] = 0x5A;
	}
	#endif //#if (CDRV_TYPE  == CDRV_PEDC)

}

//---------------------------------------------------------------------------
//
// Function:    ApiMainInit()
// Description: initializes
// Parameters:
// Return:      TRUE -- OK, FALSE -- FAIL
//
//---------------------------------------------------------------------------
void ApiMainInit()
{
	UpFw_ChkCopyState();
	GetAllChkSum();
	//UpFwEraseArea();				//avoid to reduce flash life

	TgtInitCan();
	#if ((CDRV_TYPE == CDRV_PEDC) && (CAN_MANAGE_TOGGLE == TRUE))
	CapiInitCanManage();
	#endif

	ApiGetDevieceInfo();
	CanOnLine[0] = 0x5A;
}

//---------------------------------------------------------------------------
//
// Function:    ApiGetDevieceInfo()
// Description: ���豸������Ϣ
// Parameters:
// Return:      TRUE -- OK, FALSE -- FAIL
// ˵��:        128 �ֽڵ���Ϣ����
/*
��������ַ���	96 �ֽ� [0:95]		�����ַ���
���������Ϣ	12 �ֽ�[96:107]		ʱ��(6) + �汾(2,����) + У���(4), ʮ�����Ƹ�ʽ��
									ʱ������Ϊ 2010.8.12 16:15, �汾��Ϣ���� 1.0
									У���4�ֽڵ����: ��2���ֽ��� EEDATA CS, ��2�ֽ��� ����CS��
									У����ǳ����FLASH��EEDATA�ռ�������ֽڼӷ��õ��Ľ������оƬ��ʵ���ݵķ�ӳ
����������Ϣ	12 �ֽ� [108:119]	ʱ��(6) + �汾(2,����) + У���(4), ʮ�����Ƹ�ʽ��;
									����Ϣ�Ǿ������������´���д���, ���û��������, ��˲��ֵ�����ȫ���� 0��
����			8  �ֽ� [120:127]
*/

//---------------------------------------------------------------------------
void ApiGetDevieceInfo(void)
{
	tFileInfo	myInfo,myHexInfo;
	int8u *p1, *p2;
	int8u cnt;
	int32u option;

	bios_memcpy((void*)DeviceInfo,(void*)UPFW_STATE_ADDR,UPFW_STATE_SIZE);
	myInfo    = ((tUpFwState*)DeviceInfo)->Info;
	myHexInfo = ((tUpFwState*)DeviceInfo)->HexInfo;

/*
	strSwDescription �� SW_DISCRIPTION_ADDR ��ͬ���ĵ�ַ�����Ǳ����� strSwDescription
	��������strSwDescriptionû�б�ʹ�ã����������Ż�
*/
	bios_memcpy((void*)DeviceInfo,(void*)strSwDescription,SW_DISCRIPTION_SIZE);

	//UpFwState.Info,  UpFwState.HexInfo
	p1 = (int8u*) DeviceInfo + 96;
	p2 = (int8u*) &myInfo;
	for(cnt=0; cnt<sizeof(tFileInfo); cnt++)
	{
		*p1++ = *p2++;
	}

	p2 = (int8u*) &myHexInfo;
	for(cnt=0; cnt<sizeof(tFileInfo); cnt++)
	{
		*p1++ = *p2++;
	}

	p1 = (int8u*) DeviceInfo + 120;

	//PIC30F6010A�� 4 ��16λ���üĴ���
	//PIC32�� 4 ��32λ���üĴ���, ����Ԥ���Ķ�8�ֽڳ��Ȳ���
    //2020-5-28 : �����ȡ������
    static BOOL optionFlag = FALSE;
    
    if(optionFlag == FALSE)
    {
        option = *((int32u*)config_BFC02FF0);
        *p1 ++ = (int8u)(option >> 0);
        *p1 ++ = (int8u)(option >> 8);
        *p1 ++ = (int8u)(option >> 16);
        *p1 ++ = (int8u)(option >> 24);
        
        option = *((int32u*)config_BFC02FF4);
        *p1 ++ = (int8u)(option >> 0);
        *p1 ++ = (int8u)(option >> 8);
        *p1 ++ = (int8u)(option >> 16);
        *p1 ++ = (int8u)(option >> 24);
    }
    else
    {
        option = *((int32u*)config_BFC02FF8);
        *p1 ++ = (int8u)(option >> 0);
        *p1 ++ = (int8u)(option >> 8);
        *p1 ++ = (int8u)(option >> 16);
        *p1 ++ = (int8u)(option >> 24);
        
        option = *((int32u*)config_BFC02FFC);
        *p1 ++ = (int8u)(option >> 0);
        *p1 ++ = (int8u)(option >> 8);
        *p1 ++ = (int8u)(option >> 16);
        *p1 ++ = (int8u)(option >> 24);
    }
}

