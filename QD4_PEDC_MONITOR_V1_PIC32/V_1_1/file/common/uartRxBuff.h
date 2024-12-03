#ifndef _UARTDRV_RXBUF_H
#define _UARTDRV_RXBUF_H

#include "_cpu.h"

#define PHY_UART_MAX_NUM    6

#define PERR_MASK           (1<<3)
#define FERR_MASK           (1<<2)
#define OERR_MASK           (1<<1)
#define UART_X_ERR_MASK     (PERR_MASK | FERR_MASK | OERR_MASK)


/* ������ջ������Ĵ�С
 *  */
#define UART_RXBUF_SIZE     512

typedef void (*cbUartTxCallBack)(int32u phyID);

typedef struct
{
    int8u   pRxBuf[UART_RXBUF_SIZE];/* ���ջ����� */
    //int32u  rxBufSize;              /* ���ջ������ĳ��� */
    volatile int32u  rxWritePos;    /* д�������ݵ�λ�� */
    volatile int32u  rxReadPos;     /* ���������ݵ�λ�� */
    volatile int32u  rxCounter;     /* �ѽ������ݵĳ��ȣ�0 ��ʾ������û�п������� */
    BOOL             rxBufFull;
    int32u           lastTick_1;    /* ���յ�������һ���ֽڵ�ʱ�� */
    int32u           lastTick_2;    /* ���յ������ڶ����ֽڵ�ʱ�� */
    
    cbUartTxCallBack txCallBack;    /* ���ͽ�����Ļص����� */
    
    volatile int32u  uartErrors;    /* ����״̬ */
    volatile int32u  OErrCnt;       /* OERR �������������� */
}tUartObj;


void    processAllUartRxDataBuff(void);
BOOL    readUartRxByte(int8u phyCom, int8u *data);
int     readUartRxBytes(int8u phyCom, int8u *dataBuf, int readLen);
int32u  getLastRxByteTick(int8u phyCom);
BOOL    initUartTxCallBack(int32u phyID, cbUartTxCallBack cbTxIRQ);

//***********************************************************
#endif  //_UARTDRV_RXBUF_H

