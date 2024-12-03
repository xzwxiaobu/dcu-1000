#ifndef _IPSUART_H
#define _IPSUART_H

#include "MonCommCfg.h"
#include "uartDrv.h"

#define UART_IPS_ID                 UART4                   /* 通信串口号       */
#define UART_IPS_BR                 115200                  /* 波特率           */
#define UART_IPS_IPL                INT_PRIORITY_LEVEL_5    /* 中断优先级       */

/* 
 * IBP PSL SIG 的接收/发送缓冲区大小
 */
#define UART_COM_RXBUFSIZE          300
#define UART_COM_TXBUFSIZE          1024



/* RS485 DE 引脚操作宏定义及接口函数
 * 应用层不需要使用此定义，由 tUartComm 中 fpDE_Dis & fpDE_En 进行管理
 */
/* IBP PSL SIG */
//RE-M  : RD10
//DE-M  : RD11
#define RE_M_DISABLE()      mPORTDSetBits(BIT_10)
#define RE_M_ENABLE()       mPORTDClearBits(BIT_10)
#define DE_M_DISABLE()      mPORTDClearBits(BIT_11)
#define DE_M_ENABLE()       mPORTDSetBits(BIT_11)
//IPS = IBP PSL SIG 3pcbs
#define RE_IPS_DISABLE()    RE_M_DISABLE()
#define RE_IPS_ENABLE()     RE_M_ENABLE()
#define DE_IPS_DISABLE()    DE_M_DISABLE()
#define DE_IPS_ENABLE()     DE_M_ENABLE()

/*
 * 发送任务控制
 */
typedef enum
{
    SENDSTEP_IDLE = 0,      /* 没有发送任务             */
    SENDSTEP_WAITTX,        /* 延时进入发送             */
    SENDSTEP_NEWTASK,       /* 有发送任务               */
    SENDSTEP_DELAY,         /* 等待延时时间后执行发送   */
    SENDSTEP_SENDING,       /* 等待发送完成             */
    SENDSTEP_WAITRX,        /* 等待接收                 */
    SENDSTEP_FINISH,        /* 发送完成                 */
}tEnmSendPkgStep;
  
typedef BOOL   (*cbProcRxPkg)(void);

////////////////////////////////////////////////////////////////////////////////
/* 通用串口轮询函数 */
PUBLIC tEnmCommObj Uni_PollingSlaves(void * pPollingPtr);

/* 通用串口接收数据帧处理函数 */
PUBLIC BOOL Uni_ProcRecvedFrame(tUartComm * pUartCom, cbProcRxPkg pCbProcFrame);

/* 通用串口接收字节处理函数 */
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data);

/* 通用串口创建发送数据包 */
PUBLIC tEnmSendPkgStep createUniTxPackage(void *pPollingPtr, int8u* data, int dataLen, BOOL sendNow);


PUBLIC void initIPSComm(tUartComm * pUart);

PUBLIC tUartComm * getIPSUartCommPtr(void);

PUBLIC BOOL ifIPSUartTxIsBusy(void);

#endif //_IPSUART_H

