#ifndef _UARTDRV_RXBUF_H
#define _UARTDRV_RXBUF_H

#include "_cpu.h"

#define PHY_UART_MAX_NUM    6

#define PERR_MASK           (1<<3)
#define FERR_MASK           (1<<2)
#define OERR_MASK           (1<<1)
#define UART_X_ERR_MASK     (PERR_MASK | FERR_MASK | OERR_MASK)


/* 软件接收缓冲区的大小
 *  */
#define UART_RXBUF_SIZE     512

typedef void (*cbUartTxCallBack)(int32u phyID);

typedef struct
{
    int8u   pRxBuf[UART_RXBUF_SIZE];/* 接收缓冲区 */
    //int32u  rxBufSize;              /* 接收缓冲区的长度 */
    volatile int32u  rxWritePos;    /* 写接收数据的位置 */
    volatile int32u  rxReadPos;     /* 读接收数据的位置 */
    volatile int32u  rxCounter;     /* 已接收数据的长度，0 表示缓冲区没有可用数据 */
    BOOL             rxBufFull;
    int32u           lastTick_1;    /* 接收到倒数第一个字节的时间 */
    int32u           lastTick_2;    /* 接收到倒数第二个字节的时间 */
    
    cbUartTxCallBack txCallBack;    /* 发送结束后的回调函数 */
    
    volatile int32u  uartErrors;    /* 错误状态 */
    volatile int32u  OErrCnt;       /* OERR 计数器，调试用 */
}tUartObj;


void    processAllUartRxDataBuff(void);
BOOL    readUartRxByte(int8u phyCom, int8u *data);
int     readUartRxBytes(int8u phyCom, int8u *dataBuf, int readLen);
int32u  getLastRxByteTick(int8u phyCom);
BOOL    initUartTxCallBack(int32u phyID, cbUartTxCallBack cbTxIRQ);

//***********************************************************
#endif  //_UARTDRV_RXBUF_H

