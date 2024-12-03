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
* 文	 件	:	ApiMain.C
*			:	主程序调用，实现CAN通讯及升级功能
*     V0.1
*        1. ApiGetDevieceInfo 改为交替读配置字
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
extern void UpFwStateToMMS(void);                     //升级状态返回 TO MMS
#endif

//**********************************************************************************************
//                                                                                             *
//                                     Api 函数                                                *
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

	//CAN 通讯数据处理, 最好放在最开始
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

		//MMS_UPDATE_STATE() : MST_STATE_IDLE 表示没有升级
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

    /* 对于离线的DCU不在报 ID 冲突 
     * 注意  SlvCan0Err / SlvCan1Err 对应位是0表示在线
     *     即只要有一个位为0 则对应的 DCU 是在线
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
// Description: 读设备自身信息
// Parameters:
// Return:      TRUE -- OK, FALSE -- FAIL
// 说明:        128 字节的信息内容
/*
软件描述字符串	96 字节 [0:95]		任意字符串
程序代码信息	12 字节[96:107]		时间(6) + 版本(2,保留) + 校验和(4), 十六进制格式；
									时间设置为 2010.8.12 16:15, 版本信息设置 1.0
									校验和4字节的组成: 高2个字节是 EEDATA CS, 低2字节是 程序CS；
									校验和是程序从FLASH和EEDATA空间读出后按字节加法得到的结果，是芯片真实内容的反映
升级代码信息	12 字节 [108:119]	时间(6) + 版本(2,保留) + 校验和(4), 十六进制格式；;
									此信息是经过升级程序下传后写入的, 如果没有升级过, 则此部分的内容全部是 0；
保留			8  字节 [120:127]
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
	strSwDescription 和 SW_DISCRIPTION_ADDR 是同样的地址，但是必须用 strSwDescription
	否则由于strSwDescription没有被使用，编译器将优化
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

	//PIC30F6010A有 4 个16位配置寄存器
	//PIC32有 4 个32位配置寄存器, 所以预留的额8字节长度不足
    //2020-5-28 : 交替读取配置字
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

