#ifndef _IPSCOMMUNI_H
#define _IPSCOMMUNI_H

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "IPSUart.h"


/* ��ѯ���� 
 * ���� RS485-2W��uartDrv �����ײ��Զ�����
 * ���ڷ��ͳ�ʱ��
 * �� 115200bps(1+8+1 bits)��������100�ֽڣ�100*(1+8+1)/115200 ~= 8.7ms
 * �ظ���Ϣ 140Bytes Լ��Ҫ 13ms
 * ȡ���ͳ�ʱ 20ms��ȡ������ѯ��ʱ�� IPS_INTURN_TIME = 50ms
 * 
 * ִ����������ʱ����ִ����ѯ
 */
    
#define IPS_INTURN_TIME             50      /* ��ѯһ�������ʱ�����ƣ�������� IPS_TX_OVERTIME */
#define IPS_TX_OVERTIME             30      /* ��ѯ����ʱ������,ms */
#define IPS_OFFLINE_NUM             5       /* ͨ�Ŵ���ﵽ�趨ֵ�� �������߱�� */
#define IPS_ERRCNT_MAX              200     /* ͨ�Ŵ���������ֵ */
#define IPS_LINKFAIL_TIME           500     /* ͨ������������ʱ��, ms */

#if (IPS_INTURN_TIME <= IPS_TX_OVERTIME)
#error "IPS_INTURN_TIME must be lager than IPS_TX_OVERTIME"
#endif


//typedef int16u (*createTxPkg)(tPktHeader *pkgHeader, int8u* data, int8u dataLen, BOOL sendNow);
/* ��ѯ����ṹ */
typedef struct
{
    tUartComm   *pollingCom;            /* ��ѯ�����õ��Ĵ��� */
    int8u       *pollingObjList;        /* ��ѯ�����б� */
    
    int8u       pollingObjNum;          /* ��ѯ�������� */
    int8u       pollingTurn;            /* ��ѯ˳�� */
    
    int8u       procNode;               /* ����������Ϣ��Ҫ����Ľڵ� */
    
    tSYSTICK    PollingTurnTime;        /* ��ѯ���ʱ��             */
    tSYSTICK    PollingTxOvTime;        /* ��ѯʱ������ʱ������     */
    tSYSTICK    pollingStepTime;        /* ��ѯʱ����¼����ִ��ʱ�� */
    
    int8u       ErrMaxNumOffLine;       /* ͨ�Ŵ���ﵽ�趨ֵ�� �������߱�� */
    int8u       setWaitTXTime;          /* �ӿ����л�������״̬�ȴ�ʱ����ٷ��� */
    
    tEnmSendPkgStep sendPkgStep;        /* ��ѯ���� */
    tPktHeader      sendPkgHeader;
    int             sendPkgLen;
    int8u           sendPkgBuf[UART_COM_TXBUFSIZE];
    
    cbProcRxPkg     fpCbProcRxPkg;      /* ����ָ�룬�ص�������յ����ݰ� */
}tPollingTask;


/* �������ʼ��
 * IPSģ����ؿ�������MCU���ڳ�ʼ�� */
PUBLIC void initIPSModule(void);

/* ����PEDC���ݰ������н�����ִ����Ӧ�����񲢻ظ�
 * ��Ҫ����������ѭ������
 */
PUBLIC void processIPSComm(void);

////////////////////////////////////////////////////////////////////////////////
/* ͨ�ô��ڽ����ֽڴ����� */
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data);

/* ͨ�ô��ڴ����������ݰ� */
PUBLIC tEnmSendPkgStep createUniTxPackage(void *pPollingPtr, int8u* data, int dataLen, BOOL sendNow);


/* Uart ��ѯ IBP PSL SIG ����״̬ , ����  TRUE ��ʾ�������� */
PUBLIC BOOL IPSObj_chkOnline(int8u id);

/* ���ڽ��յ����ݣ�һ���ֽڣ���Ĵ������
 *  */
PUBLIC void IPS_UartReceiveData(int8u module_id, int8u data);

/* ���Ÿ� PIC24���� */
PUBLIC BOOL IPS_ProcRxFrame(tUartComm   *pUartCom, int8u dstNode, BOOL incSeq);

/* ���ڶ�ʱ���̵���������Ϣ */
PUBLIC BOOL ifReadRelayLife(int8u obj);
PUBLIC void clearReadRelayLife(int8u obj);
PUBLIC void * getRelayRecordPtr(int8u obj);

#endif //_IPSCOMMUNI_H

