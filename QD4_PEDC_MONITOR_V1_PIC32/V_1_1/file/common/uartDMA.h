#ifndef _UART_DMA_H
#define _UART_DMA_H

#include <plib.h>
#include "peripheral\uart.h"
#include "_cpu.h"

typedef void (* cbFunDMAFinished)(int32u uart_id);

typedef struct
{
    UART_MODULE uartID;                 /* 需要发送的串口模块 ID */
    int8u*      pTxData;                /* 待发送数据存储区指针 */
    int         TxDataLen;              /* 待发送数据的长度     */
    
    cbFunDMAFinished   cbDMAFinished;   /* 发送结束以后回调函数 */
}tUartDMASetting;


PUBLIC BOOL uartStartSend_DMA(tUartDMASetting *ptrUartDMA);
PUBLIC BOOL uartStopSend_DMA(UART_MODULE uid);
PUBLIC void checkUartTxDMAFinished(void);


#endif

