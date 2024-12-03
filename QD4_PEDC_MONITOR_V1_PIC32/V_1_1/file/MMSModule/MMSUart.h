#ifndef _MMS_UART_H
#define _MMS_UART_H

#include "uartDrv.h"

#define UART_MMS_ID         UART2                   /* 通信串口号       */
#define UART_MMS_BR         115200                  /* 波特率           */
#define UART_MMS_IPL        INT_PRIORITY_LEVEL_5    /* 中断优先级       */

#define MMS_RXBUF_SIZE      1024
#define MMS_TXBUF_SIZE      2048

/* 用于处理接收数据帧的回调函数 */
typedef BOOL   (*cbMMS_AppProcRxPkg)(int8u * pPara, int len);

/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
PUBLIC BOOL ifMMSUartTxIsBusy(void);

/* 供 CAN 总线升级调用的函数，返回 TRUE 表示串口发送结束 */
PUBLIC BOOL chkFinishTxToMMS(void);

/* 发送数据到MMS */
PUBLIC tEnmComTxSts sendPackageToMMS(int8u* pTxData, int TxDataLen);

/* 处理接收的 MMS 数据帧 */
PUBLIC BOOL MMS_ProcRxFunct(cbMMS_AppProcRxPkg appMMS_ProcRxPkg);

#endif