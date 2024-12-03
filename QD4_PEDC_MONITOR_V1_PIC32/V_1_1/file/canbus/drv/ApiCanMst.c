/*=============================================================================
* Project:		WH2-LINE PEDC
* PEDC MCU:		PIC32MX795F512H + PIC18F23K22
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.0
* Author:		lly
* Date:			2011-6-16
* =============================================
* ��	 ��	:	ApiCanMst.C
*			:	Can bus Ӧ�ã�MASTER��
*		 	:   ͨ���л� CAN1/CAN2 ʵ������, ͨ�� CanCfg.h �� CAN_MANAGE_TOGGLE �ĺ궨��ʵ��
*			:   ���ַ�ʽ: �����ĳ��Slave��ĳ��CAN����, ��һ�������һ��ͨѶʧ��
* =============================================================================*/
#include  "..\_g_canbus.h"
#include  "define.h"
#include  "saveIbpX.h"
#include  "can.h"

#include  "_glbApi.h"
#include  "glbCan.h"

#include  "CanDrv.h"
#include  "CanTgt.h"
#include  "..\ObjDict.h"

#if (UPDATE_FUN_EN == TRUE)
#include "glbUpdate.h"
#endif

//#include "UpEEPROM.h"

#if (CDRV_TYPE == CDRV_PEDC)

//---------------------------------------------------------------------------
	static const int16u MstOverTime[16] =
	{
		MASTER_DFT_OV + 0,            //00
		MASTER_DFT_OV + 2,            //01
		MASTER_DFT_OV + 4,            //02
		MASTER_DFT_OV + 6,            //03
		MASTER_DFT_OV + 8,            //04
		MASTER_DFT_OV + 10,           //05
		MASTER_DFT_OV + 10,           //06
		MASTER_DFT_OV + 10,           //07
		MASTER_DFT_OV + 10,           //08
		MASTER_DFT_OV + 10,           //09
		MASTER_DFT_OV + 10,           //10
		MASTER_DFT_OV + 10,           //11
		MASTER_DFT_OV + 10,           //12
		MASTER_DFT_OV + 10,           //13
		MASTER_DFT_OV + 10,           //14
		MASTER_DFT_OV + 10,           //15
	};

	tCanManage CanManage;

#if (PRINT_TIME == TRUE)
	static void CapiPrintTime(tThreadPara * pTh);
#endif

//---------------------------------------------------------------------------
static int8u CapiCreateThread(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf, int8u cmdFlag);
static void CapiStartTransCan(tThreadPara* pTrans, int8u cmdFlag);
static void CapiThreadSendPacket(tThreadPara * pTh);
static int16u CapiThreadSendDataO(tThreadPara * pTh, int8u cmdFlag);
static int16u CapiThreadSaveDataO(tThreadPara * pTh, tCdrvMsg * pMsg);

static void CapiThreadMstSendData(tThreadPara * pTh, int8u cmdFlag);
static void CapiThreadMstSaveData(tThreadPara * pTh, tCdrvMsg * pMsg);

static void CapiSetCanState(tThreadPara * pTh, int8u flag);

extern int8u UpdateCmdFromSlave(tCdrvMsg * pMsg);

//**********************************************************************************************
//                                                                                             *
//                                      Master ����                                            *
//                                                                                             *
//**********************************************************************************************
//---------------------------------------------------------------------------
// Function:    CapiStartTransCmd()
// Description: ������չ����, ���ݳ��� <= 7 Bytes
// Parameters:  ���̹��ܺ� u8FunId
// Return:      FALSE = ʧ��
// State:
//---------------------------------------------------------------------------
int8u CapiTransExtCmd(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf)
{
	int8u Ret;

	Ret = CapiCreateThread(u8Fun, u8NodeId, len, pBuf, TRUE);
	return Ret;
}

//---------------------------------------------------------------------------
// Function:    CapiStartTrans()
// Description: �������ݴ���, ���ܴ���������ȫ��ͬ�Ľ���
// Parameters:  ���̹��ܺ� u8FunId
// Return:      FALSE = ʧ��
// State:
//---------------------------------------------------------------------------
int8u CapiStartTrans(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf)
{
	int8u Ret;

	Ret = CapiCreateThread(u8Fun, u8NodeId, len, pBuf, FALSE);
	return Ret;
}

//---------------------------------------------------------------------------
// Function:    CapiStartTrans()
// Description: �������ݴ���, ���ܴ���������ȫ��ͬ�Ľ���
// Parameters:  ���̹��ܺ� u8FunId
// Return:      FALSE = ʧ��
// State:
//---------------------------------------------------------------------------
static int8u CapiCreateThread(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf, int8u cmdFlag)
{
	int8u CanTxChn;
	int32u dwId;

	tThreadPara *pTh1;
	#ifndef NO_CAN_BUS2
	tThreadPara *pTh2;
	#endif

	if((u8Fun == (int8u)NODO) || (u8Fun > 15))
		return FALSE;

	dwId  = (int32u)SET_FUNID(u8Fun) | u8NodeId;

	//���� CAN1 ��������
	pTh1 = (tThreadPara*)&aTransThread1[u8Fun];
	pTh1->u8CanNo   = CAN1;
	pTh1->dwId      = dwId;
	pTh1->wBufLen   = len;
	pTh1->pBufPtr   = pBuf;
	pTh1->wRxBytes  = 0;
	pTh1->wTxBytes  = 0;
	pTh1->u8PacketCnt = 0;
	pTh1->wPrevPacketRxOrTxLen = 0;
	pTh1->wRepeatPacket        = 0;
	pTh1->u8State   = GET_TRANS_TYPE(u8Fun);
	GET_TICK(pTh1->wTick);
	pTh1->wTickTot = pTh1->wTick;
	if((pTh1->pBufPtr != NULL) && (len > 0))
	{
		*(pTh1->pBufPtr + len - 1) = CalcChkSum(pTh1->pBufPtr, len-1);       //���һ���ֽ�����������CheckSum
	}
	if(u8NodeId == 0)	//�Ƿ�㲥����
		pTh1->wTransOV  = MASTER_BROAD_DLY;
	else
		pTh1->wTransOV  = MstOverTime[u8Fun];

	if(cmdFlag == TRUE)
	{
		pTh1->u8State |= fgCmdFlag;
	}
	else
	{
		pTh1->u8State &= ~fgCmdFlag;
	}

	//��CAN2����һ����ȫ��ͬ�Ľ���
	#ifndef NO_CAN_BUS2
	pTh2 = (tThreadPara*)&aTransThread2[u8Fun];
	*pTh2 = *pTh1;
	pTh2->u8CanNo   = CAN2;
	#endif

	CanTxChn = GetTxChnl(u8Fun, u8NodeId);
	if(CanTxChn & 1)
	{
		CapiStartTransCan(pTh1, cmdFlag);
	}

	#ifndef NO_CAN_BUS2
	if(CanTxChn & 2)
	{
		CapiStartTransCan(pTh2, cmdFlag);
	}
	#endif

	return TRUE;
}

//---------------------------------------------------------------------------
// Function:    GetTxChnl()
// Description: ȡ������Ϣ�� CAN ͨ��
// Parameters:
// Return:      1 -- can1, 2 -- can2, 3 -- can1 and can2
// State:
//---------------------------------------------------------------------------
int8u GetTxChnl(int8u fun_id,int8u node_id)
{
	int8u chnl;
	#if (CAN_MANAGE_TOGGLE == TRUE)
		if((node_id == 0) || (node_id > ID_DCU_MAX) || (fun_id == FUNID_CANMAN1))
		{
			chnl = 3;
		}
		else
		{
			chnl = GET_CURRENT_CHNL(GET_SLV_ID0(node_id));

			if(chnl == 0)
				chnl = 1;
			else
				chnl = 2;
		}
	#else
		chnl = 3;
	#endif

	#ifdef NO_CAN_BUS2
	chnl = 1;
	#endif

	return chnl;
}


//---------------------------------------------------------------------------
// Function:    CapiStartTransCan()
// Description: �������ݴ���, ���ܴ���������ȫ��ͬ�Ľ���
// Parameters:  ���� pTrans
// Return:
// State:
//---------------------------------------------------------------------------
static void CapiStartTransCan(tThreadPara* pTrans, int8u cmdFlag)
{
	//added @ 2012-3-30 ��ȡ��ǰϵͳʱ��
	//{
	GET_TICK(pTrans->wTick);
	pTrans->wTickTot = pTrans->wTick;
	//}

	pTrans->u8State |= fgRunning;
	if(pTrans->u8State & fgUpload)
	{
		CapiThreadSendPacket(pTrans);                     //�ϴ�, ������ʼ����
	}
	else
	{
		CapiThreadMstSendData(pTrans,cmdFlag);            //����, ���͵�һ�����ݰ�
	}
}

//---------------------------------------------------------------------------
// Function:    CapiProcessRxMsg()
// Description: Master �����������(COB-ID: FUNID | ID)
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
void  CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg)
{
	int8u   u8FunID,u8Node,u8ExtCmd,u8State;
	tThreadPara* pTh;

	u8FunID = (int8u)GET_FUNID(pMsg->m_CanId);       //�յ�����Ĺ�����
	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)&aTransThread1[u8FunID];
	else
		pTh = (tThreadPara*)&aTransThread2[u8FunID];

	u8State = pTh->u8State;                        //��Ӧ������Ľ���״̬
	if(!(u8State & fgRunning) || (u8State & (fgAbort | fgTransOK)))
		return;                                     //������Ч, ���쳣��ֹ, �������, ���˳�

	if(u8FunID != (int8u)GET_FUNID(pTh->dwId))
		return;                                     //�����벻ƥ��

	u8Node = (int8u)GET_NODEID(pTh->dwId);          //ȡ���Ͷ���� id
    
    #if 1 
    /* dcu-can V1.8 Ϊ��֧�ֹ㲥����( u8FunID = FUNID_UPDATE_FIRMWARE )
     * u8FunID = 0 ����u8Node = 0 ��ʾ�㲥����
     * 
     *  */
	if((u8FunID == 0) || (u8Node == 0))
	{                                               //Master���ͷ�ʽ : �㲥����
		return;                                     //�㲥�����ҪӦ��, Master ��ʱ���� : CapiProcessTransTimer
	}
    #else
	if(u8Node == 0)
	{                                              //Master���ͷ�ʽ : �㲥����
		return;                                     //�㲥�����ҪӦ��, Master ��ʱ���� : CapiProcessTransTimer
	}
    #endif
	else
	{                                              //Master���ͷ�ʽ : ָ������
		if(u8Node != (int8u)GET_NODEID(pMsg->m_CanId))
			return;                                  //���Ͷ�����ָ������ƥ��
	}

	u8ExtCmd = pMsg->m_bData[0];
	if(!(u8ExtCmd & TRANS_DIR))                    //�����������(���� Slave ����)
		return;

	u8ExtCmd &= ~TRANS_DIR;                        //ȡSlave���صĶκ�
	CapiSetCanState(pTh, TRUE);

	if(u8ExtCmd >= RSV_CMD_S)                      //�ǹ�������?
	{
		#if (UPDATE_FUN_EN == TRUE)
		if(u8FunID == FUNID_UPDATE_FIRMWARE)
		{
			if(UpdateCmdFromSlave((tCdrvMsg *)pMsg) == TRUE)
				pTh->u8State |= (fgTransOK | fgResult);
			else
				pTh->u8State |= (fgAbort | fgResult);
		}
		#endif
		return;                                     //����,δ����
	}
	else
	{
		//�����������
		//�ϴ�

		if(CmdTransType[u8FunID] == fgUpload)
		{
			if(u8ExtCmd != (pTh->u8PacketCnt))          //Ҫ��Slave���صĶκ���Master����Ķκ���ͬ
				return;
			CapiThreadMstSaveData(pTh, pMsg);           //�����������
            
            /* dcu-can V1.8 */
            if(u8FunID == FUNID_CANMAN1)
            {
                /* ���ݴӵڶ����ֽڿ�ʼ 
                 * Ӧ�� dcu-can �汾�ţ����߳�ͻ״̬
                 */
                if(pMsg->m_bData[1] == DCUID_CONFLICTED)
                {
                    SET_IDCOFLICT(GET_SLV_ID0(u8Node));
                }
                else
                {
                    CLR_IDCOFLICT(GET_SLV_ID0(u8Node));
                }
            }
		}
		//����
		else
		{
			if(u8ExtCmd != (pTh->u8PacketCnt-1))          //Ҫ��Slave���صĶκ���Master����Ķκ���ͬ
				return;
			if(pTh->wBufLen <= pTh->wTxBytes)
			{
				pTh->u8State |= (fgTransOK | fgResult);

				#if (PRINT_TIME == TRUE)
				CapiPrintTime((tThreadPara*)pTh);
				#endif
			}
			CapiThreadMstSendData(pTh, FALSE);                 //������������
		}
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadMstSendData()
// Description: Master ��������
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadMstSendData(tThreadPara * pTh, int8u cmdFlag)
{
	if(pTh->u8State & fgUpload)
		return;

	CapiThreadSendDataO(pTh, cmdFlag);            //ʣ����ֽ���
	pTh->u8PacketCnt++;                           //��¼�������ݰ�
}

//---------------------------------------------------------------------------
// Function:    CapiThreadMstSaveData()
// Description: Master ��������
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadMstSaveData(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int16u  wBytes;
	int8u  u8CS;

	if(!(pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSaveDataO(pTh,pMsg);         //ʣ����ֽ���
	pTh->u8PacketCnt++;                             //��¼�������ݰ�
	if(wBytes == 0)
	{
		//����ϴ�, У��������ݵ� Checksum
		u8CS = CalcChkSum(pTh->pBufPtr, pTh->wBufLen-1);
		if(u8CS == *(pTh->pBufPtr + pTh->wBufLen-1))
		{
			pTh->u8State |= (fgTransOK | fgResult);
			#if (PRINT_TIME == TRUE)
			CapiPrintTime((tThreadPara*)pTh);
			#endif
		}
		else
		{
			pTh->u8State |= (fgAbort | fgResult);
		}
	}
	else
	{
		//�����ϴ�, �����ϴ���һ�����ݰ�
		CapiThreadSendPacket(pTh);
	}
}

//---------------------------------------------------------------------------
// Function:    CapiPrintTime()
// Description:
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
#if (PRINT_TIME == TRUE)
static void CapiPrintTime(tThreadPara * pTh)
{
	/*
	tCdrvMsg  MsgBuf;
	int16u wTimer;

	wTimer = (int16u)(TgtGetTickCount() - pTh->wTickTot);

	MsgBuf.m_CanId    = SET_COBID(GET_FUNID(pTh->dwId), PRINT_TIME_ID);
	MsgBuf.m_bData[0] = GET_NODEID(pTh->dwId);
	MsgBuf.m_bData[1] = (pTh->u8State & fgTransOK);
	MsgBuf.m_bData[2] = (int8u)(wTimer>>8);
	MsgBuf.m_bData[3] = (int8u)(wTimer);
	MsgBuf.m_bSize    = 4;
	CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);
	*/
}
#endif

//---------------------------------------------------------------------------
// Function:    CapiInitCanManage()
// Description: ��ʼ��CAN1/CAN2ͨѶ����,
// Parameters:  none
// Return:      none
//              ��ʼ��:
//						id=1,3,5...�� CAN1, id=2,4,5...�� CAN2;
//						CAN1/CAN2 Ĭ��û������
//---------------------------------------------------------------------------
void CapiInitCanManage(void)
{
	int16u cnt;
	for(cnt=0; cnt<BYTES_SLV_FLAG; cnt++)
	{
		#ifndef NO_CAN_BUS2
		CanManage.SlvCanNo[cnt]   = 0xAA;
		#else
		CanManage.SlvCanNo[cnt]   = 0x00;
		#endif

		CanManage.SlvCan0Err[cnt] = 0xFF;
		CanManage.SlvCan1Err[cnt] = 0xFF;
        
        CanManage.IDConflict[cnt] = 0;
	}

	for(cnt=0; cnt<BYTES_SLV_NUM; cnt++)
	{
		CanManage.ErrCnt[cnt] = CAN_ERR_CNT_MAX | (CAN_ERR_CNT_MAX<<4);
	}
}

//---------------------------------------------------------------------------
// Function:    CapiSetCanState()
// Description: ĳ��Slave ͨѶ��CAN1/CAN2����
// Parameters:  slave id, flag = TRUE or FALSE, TRUE ��ʾͨѶ�ɹ�, FALSE ��ʾͨѶʧ��
// Return:
//---------------------------------------------------------------------------
static void CapiSetCanState(tThreadPara * pTh, int8u flag)
{
	int8u id;
	int8u CanNo_Now;
	int8u ErrCnt;

	id = GET_NODEID(pTh->dwId);

	if( (id < ID_DCU_MIN) || (id > (ID_DCU_MIN + MAX_DCU - 1)))  //	if((id == 0) || (id > g_DcuNum))
		return;

/*
	#if (CAN_MANAGE_TOGGLE == TRUE)
		if((GET_FUNID(pTh->dwId) != FUNID_CANMAN1))
			return;
	#endif //(CAN_MANAGE_TOGGLE == TRUE)
*/

	id = GET_SLV_ID0(id);
	CanNo_Now = pTh->u8CanNo;

	if(flag == FALSE)
	{
		ErrCnt = CanManage.ErrCnt[id];
		if(CanNo_Now == 0)
		{
			ErrCnt &= 0x0F;
			ErrCnt ++;

			if(ErrCnt >= CAN_ERR_CNT_MAX)
			{
				ErrCnt = CAN_ERR_CNT_MAX;
				SET_CAN1_ERR(id);
			}
			CanManage.ErrCnt[id] &= 0xF0;
			CanManage.ErrCnt[id] |= ErrCnt;
		}
		else
		{
			ErrCnt &= 0xF0;
			ErrCnt += 0x10;

			if(ErrCnt >= (CAN_ERR_CNT_MAX<<4))
			{
				ErrCnt = (CAN_ERR_CNT_MAX<<4);
				SET_CAN2_ERR(id);
			}
			CanManage.ErrCnt[id] &= 0x0F;
			CanManage.ErrCnt[id] |= ErrCnt;
		}
	}
	else
	{
		if(CanNo_Now == 0)
		{
			CLR_CAN1_ERR(id);
			CanManage.ErrCnt[id] &= 0xF0;
		}
		else
		{
			CLR_CAN2_ERR(id);
			CanManage.ErrCnt[id] &= 0x0F;
		}
	}
}

//---------------------------------------------------------------------------
// Function:    CapiCanManage()
// Description: ĳ��Slave ͨѶ��CAN1/CAN2���� : id �������� Slave �������� CAN1, ��ż���ľ������� CAN2
// Parameters:  none
// Return:
//---------------------------------------------------------------------------
void CapiCanManage(void)
{
	#if (CAN_MANAGE_TOGGLE == TRUE)
	int8u i;
	tThreadPara *pTh1, *pTh2;

	int8u id;
	int8u CanNo_Old,CanNo_New;
	int8u Err1,Err2;

	for(id = 0; id < ID_DCU_MAX; id ++)        //for(id=0; id<g_DcuNum; id++)
	{
		Err1  = CHK_CAN1_ERR(id);
		Err2  = CHK_CAN2_ERR(id);
		CanNo_Old = GET_CURRENT_CHNL(id);

		if(Err1==Err2)
		{
			if(id & 1)
			{
		//		SET_CUR_CAN2(id);
			}
			else
			{
		//		SET_CUR_CAN1(id);
			}
			
			//�˴������ã� (Err1 = Err2) == 0,��ʾ����CAN �������Ķ����������ܼ�⵽��
			//����������������Ҳ�ж�ȡDCU 22�ֽڶ��ǳ�����ֳ�����ԭ����������������ֽ���--ֻ�����ֽڣ�DCU 22�ֽڳ�����
			//��ʱ��Ҫ�л�CAN BUS����
			if(Err1 == 0)
			{
				if(ReadDcuStateFail(id) == true) //out: = true ��ʾ��������������÷�Χ�� = false ��ʾ������������÷�Χ֮��
				{
					
					#ifndef NO_CAN_BUS2
						ClsReadDcuStateFailBuf(id);
						if(CanNo_Old == 0)
						{
							SET_CUR_CAN2(id);
						}
						else
						{
							SET_CUR_CAN1(id);
						}		
					#endif	
				}	

			}	
			
		}
		else //�������� CAN ��Ȼһ�� OK, һ�� Error
		{
			if(CanNo_Old == 0)
			{//CAN1
				if(Err1 == 1)
				{
					SET_CUR_CAN2(id);
				}
			}
			else
			{//CAN2
				if(Err2 == 1)
				{
					SET_CUR_CAN1(id);
				}
			}
		}

		CanNo_New = GET_CURRENT_CHNL(id);

		//����� CAN �����л�����, ��Ӧ id �Ĺ��ܾ��ظ�����һ��
		if(CanNo_New != CanNo_Old)
		{

			//������Ҫ�ظ����͵� Slave
			if(CanNo_New == 0)  //CAN2 �л��� CAN1
			{
				pTh1 = (tThreadPara*)aTransThread2;
				pTh2 = (tThreadPara*)aTransThread1;
			}
			else
			{
				pTh1 = (tThreadPara*)aTransThread1;
				pTh2 = (tThreadPara*)aTransThread2;
			}

			for(i=0; i<MAX_TRANS_THREAD; i++,pTh1++,pTh2++)
			{
				if(GET_SLV_ID0(GET_NODEID(pTh1->dwId)) == id)
				{
					if((pTh1->u8State & (fgRunning | fgResult | fgAbort)) == (fgRunning | fgResult | fgAbort) &&
						!(pTh2->u8State & fgRunning))
					{
						if(!(pTh1->u8State & fgCmdFlag))
						{
							CapiStartTransCan(pTh2, FALSE);
						}
					}
				}
				pTh1->u8State &= ~fgResult;
			}
		}
	}

	#endif
}

//---------------------------------------------------------------------------
// Function:    ChkSlaveOnLine
// Description: ���Slave�Ƿ�����
// Parameters:  Slave id , ע�� ���� ���������ı߽� !!!
// Return:      TRUE = DCU is on line
// State:
//---------------------------------------------------------------------------
int8u ChkSlaveOnLine(int8u Slave)
{
	int8u ret;
	ret  = ChkRamBit(CanManage.SlvCan0Err, Slave-ID_DCU_MIN);
	ret &= ChkRamBit(CanManage.SlvCan1Err, Slave-ID_DCU_MIN);

	if(ret == 0)
		return TRUE;
	else
		return FALSE;
}

//---------------------------------------------------------------------------
// Function:    CapiProcessTransTimer()
// Description: ��ѭ�����ã��������ݴ��ͽ��̵�ʱ��
// Parameters:  ����Ҫ��aTransThread �ǽ��̵�״̬��¼
// Return:
//
//---------------------------------------------------------------------------
void CapiProcessTransTimer(int8u u8CanNo)
{
	#if (TRANS_TIMEROVER == TRUE)
	int8u i;
	int8u u8State;
	tThreadPara* pTh;

	int8u repeat;

	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)aTransThread1;
	else
		pTh = (tThreadPara*)aTransThread2;

	for(i=0; i<MAX_TRANS_THREAD; i++,pTh++)//MAX_TRANS_THREAD
	{
		u8State = pTh->u8State;

		if(u8State & fgRunning)                      //�Ƿ�������
		{
			if(u8State & (fgAbort | fgTransOK))
			{
				continue;
			}

			if((int16u)(TgtGetTickCount() - pTh->wTick) > pTh->wTransOV)
			{   //��ʱ
				if(GET_NODEID(pTh->dwId) == 0)      //�Ƿ�㲥����
				{
					CapiThreadMstSendData(pTh, FALSE);      //�㲥�������ݶ�ʱ��������, ֱ���������

                    //����ִ���У����㲥�����Ƿ����
                    if((pTh->wBufLen != 0) && (pTh->wTxBytes != 0))
                    {
                        if((pTh->wBufLen == pTh->wTxBytes))
                            pTh->u8State |= fgTransOK | fgResult;       //�н������ �������
                    }
                    else
                    {   //û������
                        pTh->u8State = 0;
                    }
				}
				else
				{
					repeat = pTh->wBufLen / 8;
					if(repeat < Min_RepeatPacket)
					{
						repeat = Min_RepeatPacket;
					}
					else if(repeat > MAX_RepeatPacket)
					{
						repeat = MAX_RepeatPacket;
					}

					if((pTh->wRepeatPacket > repeat) || (((int8u)GET_FUNID(pTh->dwId)) == FUNID_CANMAN1) || (((int8u)GET_FUNID(pTh->dwId)) == FUNID_CANMAN2))
					{
						pTh->u8State |= (fgAbort | fgResult);         //master : �����쳣��ֹ
	
						#if (PRINT_TIME == TRUE)
							CapiPrintTime((tThreadPara*)pTh);
						#endif
	
						CapiSetCanState(pTh, FALSE);
					}
					else
					{
						pTh->wRepeatPacket ++;
						
						if(pTh->u8State & fgUpload)
						{
							//�����ϴ�, �����ϴ���ǰ���ݰ�
							CapiThreadSendPacket(pTh);
						}
						else
						{
							if((pTh->wTxBytes >= pTh->wPrevPacketRxOrTxLen) && (pTh->u8PacketCnt > 0))
							{
                                #if 1
                                /* 2021.1.19 ���� (�����Զ��ط���ʱ����Լ�� 40ms)
                                 * BUG����MMS��������DCU���� DCU û�лظ���
                                 * �� PEDC �Զ��ط�ʱ������ѯ��������ݣ�
                                 * �ײ��Զ����һ���ֽڶκŷ��͸�DCU
                                 *      7D 55 AA 00 7C      ������ȷ�Ĳ�ѯ����
                                 *      00 7D 55 AA 00 7C   ���Ǵ����
                                 */
                                extern int8u CmdBuf[];
                                if(pTh->pBufPtr == CmdBuf)
                                {
                                    #if 0
                                    /* ���ﲻ��Ҫ�Զ��ط���������ļ� UpApiMst.c �л��ظ���ѯ
                                     * ChkTransState(FUNID_UPDATE_FIRMWARE); */
                                    if(pTh->wRepeatPacket < 3)
                                    {
                                        CapiThreadMstSendData(pTh, TRUE);                  //�ط�����
                                    }
                                    #endif
                                }
                                else
                                #endif
                                {
                                    pTh->wTxBytes -= pTh->wPrevPacketRxOrTxLen;
                                    pTh->u8PacketCnt --;
                                    CapiThreadMstSendData(pTh, FALSE);                 //������������
                                }
							}
							
						}
					}
			
				}
			}
		}
	}

	#endif
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSendPacket()
// Description: ��������
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadSendPacket(tThreadPara * pTh)
{
	tCdrvMsg MsgBuf;

	//added @ 2012-3-30 ��ȡ��ǰϵͳʱ��
	//{
	GET_TICK(pTh->wTick);
	//}

	MsgBuf.m_CanId    = pTh->dwId;
	MsgBuf.m_bData[0] = pTh->u8PacketCnt;
	MsgBuf.m_bSize    = 1;
	CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSendDataO()
// Description: ���̷�������
//---------------------------------------------------------------------------
static int16u CapiThreadSendDataO(tThreadPara * pTh, int8u cmdFlag)
{
	int16u  wBytes,j;
	int8u *pSrc,*pDst;
	tCdrvMsg  MsgBuf;

	wBytes = (pTh->wBufLen - pTh->wTxBytes);        //ʣ����ֽ���
	if(wBytes > 0)                                  //�������
	{

		if(cmdFlag == FALSE)
		{
			if(wBytes > BYTES_PER_PACK)               //һ�δ������ 7 �ֽ�
				wBytes = BYTES_PER_PACK;
			if(pTh->pBufPtr == NULL)
				wBytes = 0;

			MsgBuf.m_bData[0] = pTh->u8PacketCnt;
			MsgBuf.m_bSize = 1+(int8u)wBytes;
			pDst = &MsgBuf.m_bData[1];
		}
		else
		{
			if(wBytes > 8)                           //һ�δ������ 7 �ֽ�
				wBytes = 8;
			if(pTh->pBufPtr == NULL)
				wBytes = 0;

			MsgBuf.m_bSize = (int8u)wBytes;
			pDst = &MsgBuf.m_bData[0];
		}

		pSrc = pTh->pBufPtr + pTh->wTxBytes;
		for(j=0; j<wBytes; j++)                     //ȡ����
			*pDst++ = *pSrc++;

		MsgBuf.m_CanId    = pTh->dwId;
		CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);        //��������

		pTh->wTxBytes += (int8u)wBytes;              //��¼��������
		GET_TICK(pTh->wTick);                       //���ó�ʱ��

		pTh->wPrevPacketRxOrTxLen = (int8u)wBytes;  //����

		wBytes = (pTh->wBufLen - pTh->wTxBytes);    //ʣ����ֽ���
	}

	return wBytes;
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSaveDataO()
// Description: ���̱������� pMsg to pTh,ͬʱ������Ϣ
//---------------------------------------------------------------------------
static int16u CapiThreadSaveDataO(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int8u j;
	int8u u8Bytes;
	int8u *pSrc,*pDst;

	u8Bytes = pMsg->m_bSize - 1;                    //��Ϊ��һ������ʼ����������
	pSrc = (int8u*)&pMsg->m_bData[1];               //���ݴӵڶ����ֽڿ�ʼ
	pDst = pTh->pBufPtr + pTh->wRxBytes;

    /* ����յ������ݴ���ʣ��Ĵ洢�ռ䳤�ȣ��򰴿ռ䳤�ȱ��� */
    if((pTh ->wRxBytes + u8Bytes) > pTh ->wBufLen)
    {
        u8Bytes = pTh ->wBufLen - pTh ->wRxBytes;
    }
    
	if(pTh->pBufPtr == NULL)
		u8Bytes = 0;

	for(j=0; j<u8Bytes; j++)                    //����
		*pDst++ = *pSrc++;

	pTh->wRxBytes += u8Bytes;                   //��������
	GET_TICK(pTh->wTick);                       //���ó�ʱ��

	return (pTh->wBufLen - pTh->wRxBytes);
}

#endif //#if (CDRV_TYPE == CDRV_PEDC)



