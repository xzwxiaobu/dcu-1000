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
* ��	 ��	:	ApiCanSlav.C
*			:	Can bus Ӧ�ã�SLAVE��
* =============================================================================*/

#include  "..\_g_canbus.h"

#include  "_glbApi.h"
#include	"glbCan.h"

#include  "CanDrv.h"
#include  "CanTgt.h"
#include  "..\ObjDict.h"

#if (UPDATE_FUN_EN == TRUE)
#include "glbUpdate.h"
#endif

//#include "UpEEPROM.h"

#if (CDRV_TYPE != CDRV_PEDC)

//---------------------------------------------------------------------------
void CapiProcessTransTimer(int8u u8CanNo);                 //�������ݴ��ͽ��̵�ʱ��
void CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg);     //�����������

static void CapiThreadSendPacket(tThreadPara * pTh);
static int16u CapiThreadSendDataO(tThreadPara * pTh);
static int16u CapiThreadSaveDataO(tThreadPara * pTh, tCdrvMsg * pMsg);

extern void CapiThreadSlvSendData(tThreadPara * pTh);
extern void CapiThreadSlvSaveData(tThreadPara * pTh, tCdrvMsg * pMsg);

extern int8u UpdateCmdFromMaster(tThreadPara * pTh, tCdrvMsg * pMsg);

//**********************************************************************************************
//                                                                                             *
//                                      Slave ��������                                         *
//                                                                                             *
//**********************************************************************************************
//---------------------------------------------------------------------------
// Function:    CapiProcessRxMsg()
// Description: Slave �����������(COB-ID: FUNID | ID)
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
void  CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg)
{
	int32u   dwId;
	int8u   u8FunID,u8ExtCmd,u8State;
	tThreadPara* pTh;

	int16u  wLen;
	int8u* pBuf;

	u8State = (int8u)GET_NODEID(pMsg->m_CanId);        //�ڵ� ID

	if((u8State != 0) && (u8State != g_NodeId))       //ID ���ˣ�slave ֻ����㲥��Ϣ�������Լ�ID��ͬ����Ϣ
	{
		return;
	}

	dwId = pMsg->m_CanId;
	u8FunID = (int8u)GET_FUNID(dwId);

	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)&aTransThread1[u8FunID];
	else
		pTh = (tThreadPara*)&aTransThread2[u8FunID];

	u8ExtCmd = pMsg->m_bData[0];
	if((u8ExtCmd & TRANS_DIR))       //�����������(���� Master ����)
		return;

	u8ExtCmd &= ~TRANS_DIR;          //ȡMaster ���͵Ķκ�
	if(u8ExtCmd >= RSV_CMD_S)        //�ǹ�������?
	{
		#if (UPDATE_FUN_EN == TRUE)
		if(u8FunID == FUNID_UPDATE_FIRMWARE)
		{
			if(UpdateCmdFromMaster(pTh, pMsg) == TRUE)
				pTh->u8State |= (fgTransOK | fgResult);
			else
				pTh->u8State |= (fgAbort | fgResult);
		}
		#endif
		return;                       //����,δ����
	}
	else
	{
		if(u8ExtCmd == 0)                     //��һ��, ����������̳�ʼ��
		{
         #if (UPDATE_FUN_EN == TRUE)
         if(u8FunID == FUNID_UPDATE_FIRMWARE)
         {
				wLen = (int16u)pMsg->m_bData[5];
				wLen|= (int16u)pMsg->m_bData[6]<<8;
				wLen ++;                       //����һ���ֽڣ�Checksum
				pBuf = (int8u*)FileDataBuf;
			}
			else
			#endif
			{
				wLen = GET_TRANS_LEN(u8FunID);
				pBuf = (int8u*)GET_TRANS_BUF(u8FunID);
			}
			pTh->dwId       = dwId;
			pTh->pBufPtr    = pBuf;            //���̵Ļ���Bufferָ��
			pTh->wBufLen    = wLen;
			pTh->wTransOV   = SLAVE_DFT_OV;
			pTh->wRxBytes   = 0;
			pTh->wTxBytes   = 0;
			pTh->u8PacketCnt= 0;
			pTh->u8CanNo    = u8CanNo;
			pTh->u8State    = fgRunning | GET_TRANS_TYPE(u8FunID);
			GET_TICK(pTh->wTick);
			if((pTh->pBufPtr != NULL) && (pTh->wBufLen > 0))
			{
				*(pTh->pBufPtr + pTh->wBufLen - 1) = CalcChkSum(pTh->pBufPtr, pTh->wBufLen-1);       //��󱣴�CheckSum
			}
		}

		if((pTh->pBufPtr == NULL) || (pTh->wBufLen == 0))
			return;

		u8State = pTh->u8State;
		if(!(u8State & fgRunning) || (u8State & (fgAbort | fgTransOK)))
			return;                              //������Ч, ���쳣��ֹ, �������, ���˳�

		if(pTh->u8PacketCnt != u8ExtCmd)
			return;                              //������Ҫ�����ݰ�

		if(u8State & fgUpload)
		{//�ϴ�
			CapiThreadSlvSendData(pTh);         //�������ݰ�
		}
		else
		{//����
			CapiThreadSlvSaveData(pTh, pMsg);   //����
		}
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSlvSendData()
// Description: Slave ��������
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiThreadSlvSendData(tThreadPara * pTh)
{
	int16u  wBytes;

	if(!(pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSendDataO(pTh);              //ʣ����ֽ���
	pTh->u8PacketCnt++;                             //��¼�������ݰ�
	if(wBytes == 0)
	{
		//����ϴ�
		pTh->u8State |= (fgTransOK | fgResult);
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSlvSaveData()
// Description: Slave ��������
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiThreadSlvSaveData(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int16u  wBytes;
	int8u  u8CS,u8FeedBack=1;

	if((pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSaveDataO(pTh,pMsg);         //ʣ����ֽ���

	if(wBytes == 0)
	{
		//Slave�������أ�У�� Checksum
		u8CS = CalcChkSum(pTh->pBufPtr, pTh->wBufLen-1);
		if(u8CS == *(pTh->pBufPtr + pTh->wBufLen-1))
		{//Ӧ��ɹ�����
			pTh->u8State |= (fgTransOK | fgResult);
		}
		else
		{//У����󣬲�Ӧ�� ???
			u8FeedBack = 0;
			pTh->u8State |= (fgAbort | fgResult);
		}
	}

	if(u8FeedBack)
	{
		//SlaveӦ��
		if(GET_NODEID(pTh->dwId) != 0)               //��Ӧ��㲥����
		{
			CapiThreadSendPacket(pTh);
		}
	}

	pTh->u8PacketCnt++;                             //��¼�������ݰ�
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
	int8u i;
	int8u u8State;
	tThreadPara* pTh;

	#if (TRANS_TIMEROVER == TRUE)
	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)aTransThread1;
	else
		pTh = (tThreadPara*)aTransThread2;

	for(i=0; i<MAX_TRANS_THREAD; i++,pTh++)
	{
		u8State = pTh->u8State;

		if(u8State & fgRunning)                      //�Ƿ�������
		{
			if(u8State & (fgAbort | fgTransOK))       //�쳣��ֹ, ������� ?
				continue;

			/*
			if((int16u)(TgtGetTickCount() - pTh->wTick) > pTh->wTransOV)
			{   //��ʱ
					pTh->u8State = 0;                   //ע������
			}
			*/
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

	MsgBuf.m_CanId    = (pTh->dwId & (int32u)(~0x7F)) | (int32u)g_NodeId;
	MsgBuf.m_bData[0] = pTh->u8PacketCnt | TRANS_DIR;

	MsgBuf.m_bSize    = 1;
	CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSendDataO()
// Description: ���̷�������
//---------------------------------------------------------------------------
static int16u CapiThreadSendDataO(tThreadPara * pTh)
{
	int16u  wBytes,j;
	int8u *pSrc,*pDst;
	tCdrvMsg  MsgBuf;

	wBytes = (pTh->wBufLen - pTh->wTxBytes);        //ʣ����ֽ���
	if(wBytes > 0)                                  //�������
	{
		if(wBytes > BYTES_PER_PACK)                   //һ�δ������ 7 �ֽ�
			wBytes = BYTES_PER_PACK;

		MsgBuf.m_CanId = pTh->dwId;
		MsgBuf.m_bData[0] = pTh->u8PacketCnt | TRANS_DIR;

		if(pTh->pBufPtr == NULL)
			wBytes = 0;

		pSrc = pTh->pBufPtr + pTh->wTxBytes;
		pDst = &MsgBuf.m_bData[1];
		for(j=0; j<wBytes; j++)                     //ȡ����
			*pDst++ = *pSrc++;

		MsgBuf.m_bSize = 1+(int8u)wBytes;
		CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);        //��������

		pTh->wTxBytes += (int8u)wBytes;              //��¼��������
		GET_TICK(pTh->wTick);                       //���ó�ʱ��

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

	u8Bytes = pMsg->m_bSize - 1;                //��Ϊ��һ������ʼ����������
	pSrc = (int8u*)&pMsg->m_bData[1];            //���ݴӵڶ����ֽڿ�ʼ
	pDst = pTh->pBufPtr + pTh->wRxBytes;

	if(pTh->pBufPtr == NULL)
		u8Bytes = 0;

	for(j=0; j<u8Bytes; j++)                    //����
		*pDst++ = *pSrc++;

	pTh->wRxBytes += u8Bytes;                   //��������
	GET_TICK(pTh->wTick);                       //���ó�ʱ��

	return (pTh->wBufLen - pTh->wRxBytes);
}

//---------------------------------------------------------------------------
// Function:    CapiCanManage()
// Description: ĳ��Slave ͨѶ��CAN1/CAN2���� : id �������� Slave �������� CAN1, ��ż���ľ������� CAN2
// Parameters:  none
// Return:
//---------------------------------------------------------------------------
void CapiCanManage(void)
{
}

#endif //(CDRV_TYPE != CDRV_PEDC)


