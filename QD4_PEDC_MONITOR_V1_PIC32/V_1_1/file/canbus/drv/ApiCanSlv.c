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
* 文	 件	:	ApiCanSlav.C
*			:	Can bus 应用，SLAVE端
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
void CapiProcessTransTimer(int8u u8CanNo);                 //处理数据传送进程的时间
void CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg);     //处理接收数据

static void CapiThreadSendPacket(tThreadPara * pTh);
static int16u CapiThreadSendDataO(tThreadPara * pTh);
static int16u CapiThreadSaveDataO(tThreadPara * pTh, tCdrvMsg * pMsg);

extern void CapiThreadSlvSendData(tThreadPara * pTh);
extern void CapiThreadSlvSaveData(tThreadPara * pTh, tCdrvMsg * pMsg);

extern int8u UpdateCmdFromMaster(tThreadPara * pTh, tCdrvMsg * pMsg);

//**********************************************************************************************
//                                                                                             *
//                                      Slave 函数功能                                         *
//                                                                                             *
//**********************************************************************************************
//---------------------------------------------------------------------------
// Function:    CapiProcessRxMsg()
// Description: Slave 处理接收数据(COB-ID: FUNID | ID)
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

	u8State = (int8u)GET_NODEID(pMsg->m_CanId);        //节点 ID

	if((u8State != 0) && (u8State != g_NodeId))       //ID 过滤，slave 只处理广播消息，或与自己ID相同的信息
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
	if((u8ExtCmd & TRANS_DIR))       //接收命令错误(不是 Master 发送)
		return;

	u8ExtCmd &= ~TRANS_DIR;          //取Master 发送的段号
	if(u8ExtCmd >= RSV_CMD_S)        //是功能命令?
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
		return;                       //保留,未处理
	}
	else
	{
		if(u8ExtCmd == 0)                     //第一段, 启动传输进程初始化
		{
         #if (UPDATE_FUN_EN == TRUE)
         if(u8FunID == FUNID_UPDATE_FIRMWARE)
         {
				wLen = (int16u)pMsg->m_bData[5];
				wLen|= (int16u)pMsg->m_bData[6]<<8;
				wLen ++;                       //还有一个字节：Checksum
				pBuf = (int8u*)FileDataBuf;
			}
			else
			#endif
			{
				wLen = GET_TRANS_LEN(u8FunID);
				pBuf = (int8u*)GET_TRANS_BUF(u8FunID);
			}
			pTh->dwId       = dwId;
			pTh->pBufPtr    = pBuf;            //进程的缓冲Buffer指针
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
				*(pTh->pBufPtr + pTh->wBufLen - 1) = CalcChkSum(pTh->pBufPtr, pTh->wBufLen-1);       //最后保存CheckSum
			}
		}

		if((pTh->pBufPtr == NULL) || (pTh->wBufLen == 0))
			return;

		u8State = pTh->u8State;
		if(!(u8State & fgRunning) || (u8State & (fgAbort | fgTransOK)))
			return;                              //进程无效, 或异常终止, 或传输完成, 则退出

		if(pTh->u8PacketCnt != u8ExtCmd)
			return;                              //不是需要的数据包

		if(u8State & fgUpload)
		{//上传
			CapiThreadSlvSendData(pTh);         //发送数据包
		}
		else
		{//下载
			CapiThreadSlvSaveData(pTh, pMsg);   //保存
		}
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSlvSendData()
// Description: Slave 发送数据
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiThreadSlvSendData(tThreadPara * pTh)
{
	int16u  wBytes;

	if(!(pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSendDataO(pTh);              //剩余的字节数
	pTh->u8PacketCnt++;                             //记录发送数据包
	if(wBytes == 0)
	{
		//完成上传
		pTh->u8State |= (fgTransOK | fgResult);
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSlvSaveData()
// Description: Slave 保存数据
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiThreadSlvSaveData(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int16u  wBytes;
	int8u  u8CS,u8FeedBack=1;

	if((pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSaveDataO(pTh,pMsg);         //剩余的字节数

	if(wBytes == 0)
	{
		//Slave结束下载，校验 Checksum
		u8CS = CalcChkSum(pTh->pBufPtr, pTh->wBufLen-1);
		if(u8CS == *(pTh->pBufPtr + pTh->wBufLen-1))
		{//应答成功接收
			pTh->u8State |= (fgTransOK | fgResult);
		}
		else
		{//校验错误，不应答 ???
			u8FeedBack = 0;
			pTh->u8State |= (fgAbort | fgResult);
		}
	}

	if(u8FeedBack)
	{
		//Slave应答
		if(GET_NODEID(pTh->dwId) != 0)               //不应答广播命令
		{
			CapiThreadSendPacket(pTh);
		}
	}

	pTh->u8PacketCnt++;                             //记录接收数据包
}

//---------------------------------------------------------------------------
// Function:    CapiProcessTransTimer()
// Description: 主循环调用，处理数据传送进程的时间
// Parameters:  不需要，aTransThread 是进程的状态记录
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

		if(u8State & fgRunning)                      //是否运行中
		{
			if(u8State & (fgAbort | fgTransOK))       //异常终止, 或传输完成 ?
				continue;

			/*
			if((int16u)(TgtGetTickCount() - pTh->wTick) > pTh->wTransOV)
			{   //超时
					pTh->u8State = 0;                   //注销进程
			}
			*/
		}
	}
	#endif
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSendPacket()
// Description: 发送命令
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
// Description: 进程发送数据
//---------------------------------------------------------------------------
static int16u CapiThreadSendDataO(tThreadPara * pTh)
{
	int16u  wBytes,j;
	int8u *pSrc,*pDst;
	tCdrvMsg  MsgBuf;

	wBytes = (pTh->wBufLen - pTh->wTxBytes);        //剩余的字节数
	if(wBytes > 0)                                  //传输完毕
	{
		if(wBytes > BYTES_PER_PACK)                   //一次传输最大 7 字节
			wBytes = BYTES_PER_PACK;

		MsgBuf.m_CanId = pTh->dwId;
		MsgBuf.m_bData[0] = pTh->u8PacketCnt | TRANS_DIR;

		if(pTh->pBufPtr == NULL)
			wBytes = 0;

		pSrc = pTh->pBufPtr + pTh->wTxBytes;
		pDst = &MsgBuf.m_bData[1];
		for(j=0; j<wBytes; j++)                     //取数据
			*pDst++ = *pSrc++;

		MsgBuf.m_bSize = 1+(int8u)wBytes;
		CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);        //发送数据

		pTh->wTxBytes += (int8u)wBytes;              //记录传送总数
		GET_TICK(pTh->wTick);                       //重置超时错

		wBytes = (pTh->wBufLen - pTh->wTxBytes);    //剩余的字节数
	}

	return wBytes;
}

//---------------------------------------------------------------------------
// Function:    CapiThreadSaveDataO()
// Description: 进程保存数据 pMsg to pTh,同时反馈信息
//---------------------------------------------------------------------------
static int16u CapiThreadSaveDataO(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int8u j;
	int8u u8Bytes;
	int8u *pSrc,*pDst;

	u8Bytes = pMsg->m_bSize - 1;                //因为第一个数据始终是命令字
	pSrc = (int8u*)&pMsg->m_bData[1];            //数据从第二个字节开始
	pDst = pTh->pBufPtr + pTh->wRxBytes;

	if(pTh->pBufPtr == NULL)
		u8Bytes = 0;

	for(j=0; j<u8Bytes; j++)                    //保存
		*pDst++ = *pSrc++;

	pTh->wRxBytes += u8Bytes;                   //接收总数
	GET_TICK(pTh->wTick);                       //重置超时错

	return (pTh->wBufLen - pTh->wRxBytes);
}

//---------------------------------------------------------------------------
// Function:    CapiCanManage()
// Description: 某个Slave 通讯的CAN1/CAN2管理 : id 是奇数的 Slave 尽可能用 CAN1, 是偶数的尽可能用 CAN2
// Parameters:  none
// Return:
//---------------------------------------------------------------------------
void CapiCanManage(void)
{
}

#endif //(CDRV_TYPE != CDRV_PEDC)


