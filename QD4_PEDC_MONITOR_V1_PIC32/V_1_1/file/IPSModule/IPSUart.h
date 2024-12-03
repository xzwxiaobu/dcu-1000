#ifndef _IPSUART_H
#define _IPSUART_H

#include "MonCommCfg.h"
#include "uartDrv.h"

#define UART_IPS_ID                 UART4                   /* ͨ�Ŵ��ں�       */
#define UART_IPS_BR                 115200                  /* ������           */
#define UART_IPS_IPL                INT_PRIORITY_LEVEL_5    /* �ж����ȼ�       */

/* 
 * IBP PSL SIG �Ľ���/���ͻ�������С
 */
#define UART_COM_RXBUFSIZE          300
#define UART_COM_TXBUFSIZE          1024



/* RS485 DE ���Ų����궨�弰�ӿں���
 * Ӧ�ò㲻��Ҫʹ�ô˶��壬�� tUartComm �� fpDE_Dis & fpDE_En ���й���
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
 * �����������
 */
typedef enum
{
    SENDSTEP_IDLE = 0,      /* û�з�������             */
    SENDSTEP_WAITTX,        /* ��ʱ���뷢��             */
    SENDSTEP_NEWTASK,       /* �з�������               */
    SENDSTEP_DELAY,         /* �ȴ���ʱʱ���ִ�з���   */
    SENDSTEP_SENDING,       /* �ȴ��������             */
    SENDSTEP_WAITRX,        /* �ȴ�����                 */
    SENDSTEP_FINISH,        /* �������                 */
}tEnmSendPkgStep;
  
typedef BOOL   (*cbProcRxPkg)(void);

////////////////////////////////////////////////////////////////////////////////
/* ͨ�ô�����ѯ���� */
PUBLIC tEnmCommObj Uni_PollingSlaves(void * pPollingPtr);

/* ͨ�ô��ڽ�������֡������ */
PUBLIC BOOL Uni_ProcRecvedFrame(tUartComm * pUartCom, cbProcRxPkg pCbProcFrame);

/* ͨ�ô��ڽ����ֽڴ����� */
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data);

/* ͨ�ô��ڴ����������ݰ� */
PUBLIC tEnmSendPkgStep createUniTxPackage(void *pPollingPtr, int8u* data, int dataLen, BOOL sendNow);


PUBLIC void initIPSComm(tUartComm * pUart);

PUBLIC tUartComm * getIPSUartCommPtr(void);

PUBLIC BOOL ifIPSUartTxIsBusy(void);

#endif //_IPSUART_H

