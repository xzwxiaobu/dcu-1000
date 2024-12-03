/*=============================================================================
* Project:		WH6-LINE PEDC
* PEDC MCU:		PIC32MX795F512H + PIC18F23K22
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.0
* Author:		lly
* Date:			2011-6-16
* =============================================
* 文	 件	:	ApiCanCom.C
*			:	Can bus 应用，MASTER及SLAVE的共有函数
* =============================================================================*/
#include  "_g_canbus.h"
#include  "define.h"
#include  "saveIbpX.h"
#include  "can.h"

#include  "_glbApi.h"
#include  "glbCan.h"

#include  "CanDrv.h"
#include  "CanTgt.h"
#include  "ObjDict.h"

#if (UPDATE_FUN_EN == TRUE)
#include  "glbUpdate.h"
#endif

//#include "UpEEPROM.h"

extern void CapiProcessTransTimer(int8u u8CanNo);                 //处理数据传送进程的时间
extern void CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg);     //处理接收数据

extern void CapiCanManage(void);

//---------------------------------------------------------------------------
	int16u    wCanBusTime[2]={0,0};   //unit : 1ms, cleared by can bus interrupt (Rx or Tx)
																	//reset can-bus and can-open
																	//if can interrupt is not happened within (CAN_BUS_ERR_TIME*1ms)

	tThreadPara aTransThread1[MAX_TRANS_THREAD];    //CAN1 传输进程
	tThreadPara aTransThread2[MAX_TRANS_THREAD];    //CAN2 传输进程

	int8u  CanOnLine[2];

	int8u  CalcChkSumLEN = 0;                       //== 0 16位长； != 0 8位长
    
    int8u  sendDCUSysTime[BYTES_SYSTIME_TODCU + 2]; //至少需要留1字节用于校验和
//---------------------------------------------------------------------------
// Function:    CapiProcess()
// Description: goes here every loop
// Parameters:  void
// Return:
// State:
//---------------------------------------------------------------------------
int8u CapiProcess(void)
{
	tCdrvMsg  RxMsgBuf;
	static  int16u  wCapiTimer = 0;

	//处理接收缓冲 CAN1 / CAN2
	if(CdrvReadMsg(CAN1, (tCdrvMsg *)&RxMsgBuf) == TRUE)
	{
		CapiProcessRxMsg(CAN1,(tCdrvMsg *)&RxMsgBuf);
	}

	#ifndef NO_CAN_BUS2
	if(CdrvReadMsg(CAN2, (tCdrvMsg *)&RxMsgBuf) == TRUE)
	{
		CapiProcessRxMsg(CAN2,(tCdrvMsg *)&RxMsgBuf);
	}
	#endif

	//处理发送缓冲 CAN1 / CAN2
	CdrvCheckTransmit (CAN1);
	#ifndef NO_CAN_BUS2
	CdrvCheckTransmit (CAN2);
	#endif
/*
	//处理错误状态 CAN1 / CAN2
	CdrvProcessError(CAN1);
	#ifndef NO_CAN_BUS2
		CdrvProcessError(CAN2);
	#endif
*/
	//数据传送计时 CAN1 / CAN2
	CapiProcessTransTimer(CAN1);
	#ifndef NO_CAN_BUS2
	CapiProcessTransTimer(CAN2);
	#endif

	//定时处理
	if((int16u)(TgtGetTickCount () - wCapiTimer) > 10)     //10ms
	{
		GET_TICK(wCapiTimer);

		//here every 10ms

		#if (BUS_TIMEROVER == TRUE)
			wCanBusTime[0]++;                             //CanBus1 offline timer
			wCanBusTime[1]++;                             //CanBus2 offline timer

			if(wCanBusTime[0] > CAN_BUS_ERR_TIME)
			{
				wCanBusTime[0] = 0;
				CdrvReset(CAN1,1);
				CapiFreeAllThread(CAN1);
			}

			#ifndef NO_CAN_BUS2
			if(wCanBusTime[1] > CAN_BUS_ERR_TIME)
			{
				wCanBusTime[1] = 0;
				CdrvReset(CAN2,1);
				CapiFreeAllThread(CAN2);
			}
			#endif
		#endif
	}

	CapiCanManage();

	return 1;
}

//---------------------------------------------------------------------------
// Function:    ChkTransState()
// Description: 取得指定进程当前状态
// Parameters:  进程功能号 u8FunId, 此功能号与创建(CapiStartTrans)的参数是一致的
// Return:
//        fgRunning -- 正在处理, 如果等于 0，则 fgAbort 和 fgTransOK 无效
//        fgAbort   -- 异常错误
//        fgTransOK -- 成功
//Note:   必须两个CAN都有结果才返回结果, 则只要其中一个 OK 就会返回OK, 两个同时失败就返回失败
//---------------------------------------------------------------------------
int8u ChkTransState(int8u u8FunId)
{
	int8u State1,State2, R1,R2;

	State1 = aTransThread1[u8FunId].u8State;
	State2 = aTransThread2[u8FunId].u8State;

	if(State1 & fgRunning)
	{
		if(State1 & fgTransOK)
			R1 = fgTransOK;
		else if(State1 & fgAbort)
			R1 = fgAbort;
		else
			R1 = fgRunning;
	}
	else
		R1 = 0;

	#ifndef NO_CAN_BUS2
	if(State2 & fgRunning)
	{
		if(State2 & fgTransOK)
			R2 = fgTransOK;
		else if(State2 & fgAbort)
			R2 = fgAbort;
		else
			R2 = fgRunning;
	}
	else
	#endif
		R2 = 0;

	if(R1 && R2)  //两个同时运行
	{
		//其中一个还在运行, 返回 fgRunning
		if((R1 == fgRunning) || (R2 == fgRunning))
			return fgRunning;

		//两个都失败, 则返回 fgAbort
		if((R1 == fgAbort) && (R2 == fgAbort))
			return fgAbort;

		//其中一个已经成功通讯, 返回 fgTransOK, 同时清除另一个 ???
		if((R1 == fgTransOK) || (R2 == fgTransOK))
			return fgTransOK;

		/*
		if(R1 == fgTransOK)
		{
			aTransThread2[u8FunId].u8State = 0;         //need or not ?????
			return fgTransOK;
		}
		if(R2 == fgTransOK)
		{
			aTransThread1[u8FunId].u8State = 0;         //need or not ?????
			return fgTransOK;
		}
		*/

		//两个都没有结果
		return 0;
	}

	else   //只有一个运行
	{
		if(R1)
			return R1;
		#ifndef NO_CAN_BUS2
		if(R2)
			return R2;
		#endif
		return 0;
	}
}

//---------------------------------------------------------------------------
// Function:    ClrTransState()
// Description: 清除指定进程当前状态
// Parameters:  进程功能号 u8FunId, 此功能号与创建(CapiStartTrans)的参数是一致的
// Return:      State (按位与下面的标志位，非零则有效)
//---------------------------------------------------------------------------
void ClrTransState(int8u u8FunId)
{
	aTransThread1[u8FunId].u8State = 0;
	aTransThread2[u8FunId].u8State = 0;
    
    aTransThread1[u8FunId].wBufLen = 0;
    aTransThread2[u8FunId].wBufLen = 0;
}

//---------------------------------------------------------------------------
// Function:    CapiFreeAllThread()
// Description: 删除某个CAN的所有进程
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiFreeAllThread (int8u u8CanNo)
{
	int i;
	tThreadPara* pTh;

	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)aTransThread1;
	else
		pTh = (tThreadPara*)aTransThread2;

	for(i=0; i<MAX_TRANS_THREAD; i++,pTh++)
	{
		pTh->u8State = 0;
	}
}

//---------------------------------------------------------------------------
// Function:    CalcChkSum()
// Description: 计算指定Buffer的CheckSum
// Parameters:  Buffer 首地址，Buffer的长度
// Return:      返回一字节 CheckSum
// 算法:        累加和
//	int8u  CalcChkSumLEN;          //== 0 16位长； != 0 8位长
//---------------------------------------------------------------------------
/*
			修改原因：早版本的CalcChkSum,长度为 int8u ;
			新版本的长度为： int16u;为兼容老版本的，所以
			匹配DCU的检验和，做了该处理
*/
int8u CalcChkSum(int8u* Buf, int16u u8Size)
{
	int8u Chksum = 0;

	int16 Len;
	Len = u8Size;
	if(UpdateMsg.u8MstState != MST_STATE_IDLE)
	{	
		if(CalcChkSumLEN != 0)
		{
			Len = (int16u)((int8u)Len);	
		}	
	}	

	while(Len)
	{
		Chksum += *Buf++;
		Len--;
	}

	return Chksum;
}

//---------------------------------------------------------------------------
// Function:    CapiCanDebugPrint()
// Description: 通过Can发送调试信息, 在监控器上查看
// Parameters:  数据指针及长度, 长度 < 8
// Return:
//---------------------------------------------------------------------------
void CapiCanDebugPrint(int8u * pData, int8u Len, int8u id)
{
	int8u *p;
	tCdrvMsg  MsgBuf;

	MsgBuf.m_CanId = SET_COBID(GET_FUNID(0), id);
	MsgBuf.m_bSize = Len;

	p = &MsgBuf.m_bData[0];
	while(Len)
	{
		*p++ = *pData++;
		Len--;
	}

	CdrvWriteMsg(CAN1, &MsgBuf);
	#ifndef NO_CAN_BUS2
	CdrvWriteMsg(CAN2, &MsgBuf);
	#endif
}

//---------------------------------------------------------------------------
// Function:    SetRamBit / ClrRamBit / ChkRamBit / TogRamBit
// Description: 位操作
// Parameters:  注意 －－ 不检查参数的边界 !!!
// Return:
// State:
//---------------------------------------------------------------------------
//位设置
void SetRamBit(int8u *p,int8u bit)
{
	p  += bit>>3;
	*p |= 1<<(bit & 7);
}

//位清除
void ClrRamBit(int8u *p,int8u bit)
{
	p  += bit>>3;
	*p &= ~(1<<(bit & 7));
}

//位测试
int8u ChkRamBit(int8u *p,int8u bit)
{
	p += bit>>3;
	if(*p & (1<<(bit & 7)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//位切换
void TogRamBit(int8u *p,int8u bit)
{
	p += bit>>3;
	*p ^= (1<<(bit & 7));
}
















