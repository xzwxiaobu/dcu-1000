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
* 文	 件	:	ApiCanMst.C
*			:	Can bus 应用，MASTER端
*		 	:   通过切换 CAN1/CAN2 实现冗余, 通过 CanCfg.h 中 CAN_MANAGE_TOGGLE 的宏定义实现
*			:   这种方式: 如果有某个Slave的某个CAN错误, 则一定会出现一次通讯失败
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
//                                      Master 函数                                            *
//                                                                                             *
//**********************************************************************************************
//---------------------------------------------------------------------------
// Function:    CapiStartTransCmd()
// Description: 发送扩展命令, 数据长度 <= 7 Bytes
// Parameters:  进程功能号 u8FunId
// Return:      FALSE = 失败
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
// Description: 启动数据传输, 不能创建两个完全相同的进程
// Parameters:  进程功能号 u8FunId
// Return:      FALSE = 失败
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
// Description: 启动数据传输, 不能创建两个完全相同的进程
// Parameters:  进程功能号 u8FunId
// Return:      FALSE = 失败
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

	//先在 CAN1 建立进程
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
		*(pTh1->pBufPtr + len - 1) = CalcChkSum(pTh1->pBufPtr, len-1);       //最后一个字节留出来保存CheckSum
	}
	if(u8NodeId == 0)	//是否广播命令
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

	//在CAN2建立一个完全相同的进程
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
// Description: 取发送信息的 CAN 通道
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
// Description: 启动数据传输, 不能创建两个完全相同的进程
// Parameters:  进程 pTrans
// Return:
// State:
//---------------------------------------------------------------------------
static void CapiStartTransCan(tThreadPara* pTrans, int8u cmdFlag)
{
	//added @ 2012-3-30 获取当前系统时间
	//{
	GET_TICK(pTrans->wTick);
	pTrans->wTickTot = pTrans->wTick;
	//}

	pTrans->u8State |= fgRunning;
	if(pTrans->u8State & fgUpload)
	{
		CapiThreadSendPacket(pTrans);                     //上传, 发送起始命令
	}
	else
	{
		CapiThreadMstSendData(pTrans,cmdFlag);            //下载, 发送第一个数据包
	}
}

//---------------------------------------------------------------------------
// Function:    CapiProcessRxMsg()
// Description: Master 处理接收数据(COB-ID: FUNID | ID)
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
void  CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg)
{
	int8u   u8FunID,u8Node,u8ExtCmd,u8State;
	tThreadPara* pTh;

	u8FunID = (int8u)GET_FUNID(pMsg->m_CanId);       //收到对象的功能码
	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)&aTransThread1[u8FunID];
	else
		pTh = (tThreadPara*)&aTransThread2[u8FunID];

	u8State = pTh->u8State;                        //对应功能码的进程状态
	if(!(u8State & fgRunning) || (u8State & (fgAbort | fgTransOK)))
		return;                                     //进程无效, 或异常终止, 或传输完成, 则退出

	if(u8FunID != (int8u)GET_FUNID(pTh->dwId))
		return;                                     //功能码不匹配

	u8Node = (int8u)GET_NODEID(pTh->dwId);          //取发送对象的 id
    
    #if 1 
    /* dcu-can V1.8 为了支持广播升级( u8FunID = FUNID_UPDATE_FIRMWARE )
     * u8FunID = 0 或者u8Node = 0 表示广播命令
     * 
     *  */
	if((u8FunID == 0) || (u8Node == 0))
	{                                               //Master发送方式 : 广播命令
		return;                                     //广播命令不需要应答, Master 定时发送 : CapiProcessTransTimer
	}
    #else
	if(u8Node == 0)
	{                                              //Master发送方式 : 广播命令
		return;                                     //广播命令不需要应答, Master 定时发送 : CapiProcessTransTimer
	}
    #endif
	else
	{                                              //Master发送方式 : 指定对象
		if(u8Node != (int8u)GET_NODEID(pMsg->m_CanId))
			return;                                  //发送对象与指定对象不匹配
	}

	u8ExtCmd = pMsg->m_bData[0];
	if(!(u8ExtCmd & TRANS_DIR))                    //接收命令错误(不是 Slave 发送)
		return;

	u8ExtCmd &= ~TRANS_DIR;                        //取Slave返回的段号
	CapiSetCanState(pTh, TRUE);

	if(u8ExtCmd >= RSV_CMD_S)                      //是功能命令?
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
		return;                                     //保留,未处理
	}
	else
	{
		//处理接收数据
		//上传

		if(CmdTransType[u8FunID] == fgUpload)
		{
			if(u8ExtCmd != (pTh->u8PacketCnt))          //要求Slave返回的段号与Master请求的段号相同
				return;
			CapiThreadMstSaveData(pTh, pMsg);           //保存接收数据
            
            /* dcu-can V1.8 */
            if(u8FunID == FUNID_CANMAN1)
            {
                /* 数据从第二个字节开始 
                 * 应答 dcu-can 版本号，或者冲突状态
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
		//下载
		else
		{
			if(u8ExtCmd != (pTh->u8PacketCnt-1))          //要求Slave返回的段号与Master请求的段号相同
				return;
			if(pTh->wBufLen <= pTh->wTxBytes)
			{
				pTh->u8State |= (fgTransOK | fgResult);

				#if (PRINT_TIME == TRUE)
				CapiPrintTime((tThreadPara*)pTh);
				#endif
			}
			CapiThreadMstSendData(pTh, FALSE);                 //继续传送数据
		}
	}
}

//---------------------------------------------------------------------------
// Function:    CapiThreadMstSendData()
// Description: Master 发送数据
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadMstSendData(tThreadPara * pTh, int8u cmdFlag)
{
	if(pTh->u8State & fgUpload)
		return;

	CapiThreadSendDataO(pTh, cmdFlag);            //剩余的字节数
	pTh->u8PacketCnt++;                           //记录发送数据包
}

//---------------------------------------------------------------------------
// Function:    CapiThreadMstSaveData()
// Description: Master 保存数据
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadMstSaveData(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int16u  wBytes;
	int8u  u8CS;

	if(!(pTh->u8State & fgUpload))
		return;

	wBytes = CapiThreadSaveDataO(pTh,pMsg);         //剩余的字节数
	pTh->u8PacketCnt++;                             //记录接收数据包
	if(wBytes == 0)
	{
		//完成上传, 校验接收数据的 Checksum
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
		//继续上传, 申请上传下一个数据包
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
// Description: 初始化CAN1/CAN2通讯管理,
// Parameters:  none
// Return:      none
//              初始化:
//						id=1,3,5...用 CAN1, id=2,4,5...用 CAN2;
//						CAN1/CAN2 默认没有连接
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
// Description: 某个Slave 通讯的CAN1/CAN2管理
// Parameters:  slave id, flag = TRUE or FALSE, TRUE 表示通讯成功, FALSE 表示通讯失败
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
// Description: 某个Slave 通讯的CAN1/CAN2管理 : id 是奇数的 Slave 尽可能用 CAN1, 是偶数的尽可能用 CAN2
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
			
			//此处的作用： (Err1 = Err2) == 0,表示两条CAN 生命报文都是正常，能检测到；
			//但生命报文正常，也有读取DCU 22字节都是出错的现场现象（原因分析：生命报文字节少--只有两字节，DCU 22字节长；）
			//此时需要切换CAN BUS总线
			if(Err1 == 0)
			{
				if(ReadDcuStateFail(id) == true) //out: = true 表示错误次数超出设置范围。 = false 表示错误次数在设置范围之内
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
		else //这里两个 CAN 必然一个 OK, 一个 Error
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

		//如果有 CAN 发生切换动作, 对应 id 的功能就重复发送一次
		if(CanNo_New != CanNo_Old)
		{

			//查找需要重复发送的 Slave
			if(CanNo_New == 0)  //CAN2 切换到 CAN1
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
// Description: 检测Slave是否在线
// Parameters:  Slave id , 注意 －－ 不检查参数的边界 !!!
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
// Description: 主循环调用，处理数据传送进程的时间
// Parameters:  不需要，aTransThread 是进程的状态记录
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

		if(u8State & fgRunning)                      //是否运行中
		{
			if(u8State & (fgAbort | fgTransOK))
			{
				continue;
			}

			if((int16u)(TgtGetTickCount() - pTh->wTick) > pTh->wTransOV)
			{   //超时
				if(GET_NODEID(pTh->dwId) == 0)      //是否广播命令
				{
					CapiThreadMstSendData(pTh, FALSE);      //广播命令数据定时发送数据, 直到发送完成

                    //进程执行中，检查广播发送是否完成
                    if((pTh->wBufLen != 0) && (pTh->wTxBytes != 0))
                    {
                        if((pTh->wBufLen == pTh->wTxBytes))
                            pTh->u8State |= fgTransOK | fgResult;       //有结果处理： 发送完成
                    }
                    else
                    {   //没有任务
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
						pTh->u8State |= (fgAbort | fgResult);         //master : 发送异常终止
	
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
							//继续上传, 申请上传当前数据包
							CapiThreadSendPacket(pTh);
						}
						else
						{
							if((pTh->wTxBytes >= pTh->wPrevPacketRxOrTxLen) && (pTh->u8PacketCnt > 0))
							{
                                #if 1
                                /* 2021.1.19 补丁 (这里自动重发的时间间隔约是 40ms)
                                 * BUG现象：MMS启动升级DCU，若 DCU 没有回复，
                                 * 则 PEDC 自动重发时，将查询命令当成数据，
                                 * 底层自动添加一个字节段号发送给DCU
                                 *      7D 55 AA 00 7C      这是正确的查询命令
                                 *      00 7D 55 AA 00 7C   这是错误的
                                 */
                                extern int8u CmdBuf[];
                                if(pTh->pBufPtr == CmdBuf)
                                {
                                    #if 0
                                    /* 这里不需要自动重发命令，升级文件 UpApiMst.c 中会重复查询
                                     * ChkTransState(FUNID_UPDATE_FIRMWARE); */
                                    if(pTh->wRepeatPacket < 3)
                                    {
                                        CapiThreadMstSendData(pTh, TRUE);                  //重发命令
                                    }
                                    #endif
                                }
                                else
                                #endif
                                {
                                    pTh->wTxBytes -= pTh->wPrevPacketRxOrTxLen;
                                    pTh->u8PacketCnt --;
                                    CapiThreadMstSendData(pTh, FALSE);                 //继续传送数据
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
// Description: 发送命令
// Parameters:
// Return:
//---------------------------------------------------------------------------
static void CapiThreadSendPacket(tThreadPara * pTh)
{
	tCdrvMsg MsgBuf;

	//added @ 2012-3-30 获取当前系统时间
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
// Description: 进程发送数据
//---------------------------------------------------------------------------
static int16u CapiThreadSendDataO(tThreadPara * pTh, int8u cmdFlag)
{
	int16u  wBytes,j;
	int8u *pSrc,*pDst;
	tCdrvMsg  MsgBuf;

	wBytes = (pTh->wBufLen - pTh->wTxBytes);        //剩余的字节数
	if(wBytes > 0)                                  //传输完毕
	{

		if(cmdFlag == FALSE)
		{
			if(wBytes > BYTES_PER_PACK)               //一次传输最大 7 字节
				wBytes = BYTES_PER_PACK;
			if(pTh->pBufPtr == NULL)
				wBytes = 0;

			MsgBuf.m_bData[0] = pTh->u8PacketCnt;
			MsgBuf.m_bSize = 1+(int8u)wBytes;
			pDst = &MsgBuf.m_bData[1];
		}
		else
		{
			if(wBytes > 8)                           //一次传输最大 7 字节
				wBytes = 8;
			if(pTh->pBufPtr == NULL)
				wBytes = 0;

			MsgBuf.m_bSize = (int8u)wBytes;
			pDst = &MsgBuf.m_bData[0];
		}

		pSrc = pTh->pBufPtr + pTh->wTxBytes;
		for(j=0; j<wBytes; j++)                     //取数据
			*pDst++ = *pSrc++;

		MsgBuf.m_CanId    = pTh->dwId;
		CdrvWriteMsg(pTh->u8CanNo, &MsgBuf);        //发送数据

		pTh->wTxBytes += (int8u)wBytes;              //记录传送总数
		GET_TICK(pTh->wTick);                       //重置超时错

		pTh->wPrevPacketRxOrTxLen = (int8u)wBytes;  //备份

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

	u8Bytes = pMsg->m_bSize - 1;                    //因为第一个数据始终是命令字
	pSrc = (int8u*)&pMsg->m_bData[1];               //数据从第二个字节开始
	pDst = pTh->pBufPtr + pTh->wRxBytes;

    /* 如果收到的数据大于剩余的存储空间长度，则按空间长度保存 */
    if((pTh ->wRxBytes + u8Bytes) > pTh ->wBufLen)
    {
        u8Bytes = pTh ->wBufLen - pTh ->wRxBytes;
    }
    
	if(pTh->pBufPtr == NULL)
		u8Bytes = 0;

	for(j=0; j<u8Bytes; j++)                    //保存
		*pDst++ = *pSrc++;

	pTh->wRxBytes += u8Bytes;                   //接收总数
	GET_TICK(pTh->wTick);                       //重置超时错

	return (pTh->wBufLen - pTh->wRxBytes);
}

#endif //#if (CDRV_TYPE == CDRV_PEDC)



