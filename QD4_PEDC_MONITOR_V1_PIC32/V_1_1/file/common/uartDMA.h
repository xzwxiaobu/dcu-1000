#ifndef _UART_DMA_H
#define _UART_DMA_H

#include <plib.h>
#include "peripheral\uart.h"
#include "_cpu.h"

typedef void (* cbFunDMAFinished)(int32u uart_id);

typedef struct
{
    UART_MODULE uartID;                 /* ��Ҫ���͵Ĵ���ģ�� ID */
    int8u*      pTxData;                /* ���������ݴ洢��ָ�� */
    int         TxDataLen;              /* ���������ݵĳ���     */
    
    cbFunDMAFinished   cbDMAFinished;   /* ���ͽ����Ժ�ص����� */
}tUartDMASetting;


PUBLIC BOOL uartStartSend_DMA(tUartDMASetting *ptrUartDMA);
PUBLIC BOOL uartStopSend_DMA(UART_MODULE uid);
PUBLIC void checkUartTxDMAFinished(void);


#endif

