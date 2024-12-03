#ifndef _MMS_UART_H
#define _MMS_UART_H

#include "uartDrv.h"

#define UART_MMS_ID         UART2                   /* ͨ�Ŵ��ں�       */
#define UART_MMS_BR         115200                  /* ������           */
#define UART_MMS_IPL        INT_PRIORITY_LEVEL_5    /* �ж����ȼ�       */

#define MMS_RXBUF_SIZE      1024
#define MMS_TXBUF_SIZE      2048

/* ���ڴ����������֡�Ļص����� */
typedef BOOL   (*cbMMS_AppProcRxPkg)(int8u * pPara, int len);

/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
PUBLIC BOOL ifMMSUartTxIsBusy(void);

/* �� CAN �����������õĺ��������� TRUE ��ʾ���ڷ��ͽ��� */
PUBLIC BOOL chkFinishTxToMMS(void);

/* �������ݵ�MMS */
PUBLIC tEnmComTxSts sendPackageToMMS(int8u* pTxData, int TxDataLen);

/* ������յ� MMS ����֡ */
PUBLIC BOOL MMS_ProcRxFunct(cbMMS_AppProcRxPkg appMMS_ProcRxPkg);

#endif