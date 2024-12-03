#ifndef _MAINIO_H
#define _MAINIO_H

#include "_cpu.h"
#include "MonCommCfg.h"
#include "Update.h"


/* ����/���������󳤶�
 *  PIC32-GPIO + 3 * PCA9505-IO = 12 + 3 * 5 = 27 byte
 *  ��������˵�����ܻ�������ȫ�̵������ϡ�ͨ��״̬����Ϣ
 *  ����ȡ 32 �ֽ�
 */

/* ����PCB����ID��Ӧ�� IO�� */
#define READ_ID_ADR0() PORTReadBits(IOPORT_B, BIT_7)
#define READ_ID_ADR1() PORTReadBits(IOPORT_B, BIT_10)
#define READ_ID_ADR2() PORTReadBits(IOPORT_B, BIT_11)
#define READ_ID_ADR3() PORTReadBits(IOPORT_B, BIT_9)


/* ��ӻ���ص���Ϣ 
 */
typedef struct
{
    int8u   slaveLinkOK;            /* ��ӻ�ͨ��״̬               */
    int8u   sendSeq;                /* ��ӻ�ͨ�����к�             */
    int8u   ConfigFlag;             /* �ӻ�����״̬��Ч��־         */
    int8u   VersionFlag;            /* 1 = �ӻ��汾��Ϣ��Ч��־     */
    int8u   commErrCnt;             /* ͨ�Ŵ��������               */
    int8u   dataValidCnt;           /* ͨ�����ӳɹ��Ժ���Ҫ��������N�κ󣬽��յ��������ݲ���Ч */
    
    int8u   gotIOData;              /* 1 = �����ӻ� IO ����         */
    
    int8u   slaveReplyUpdate;       /* ��������״̬��־             */
    int8u   slaveUpdateSts;         /* ����״̬                     */
    int16u  slaveUpdateSec;         /* ���������                   */
    
    /* �ӻ� �汾��Ϣ�ַ��� */
    char    slaveDevInfo[BYTES_INFO];
    
    tSlaveStsFlags slaveSts;
    
    /* ���մӻ���������
     */
    /* �ӻ� IO�������� */
    //int8u   RdDataBuf[MAX_IODATA_SIZE];
    int     RdDataLen;
    tCmdReadIO_s slaveInput;
    
    /* ���ʹӻ�������� */
    //int8u   WrDataBuf[MAX_IODATA_SIZE];
    int     WrDataLen;
    tCmdWriteIO_m   slaveOutput;

    /* д�ӻ��� IO���� 
     * ��ʱ��֧�����ôӻ�
     * tSlaveIOScanCfg slaveIOCfg;
     */
}tSlaveInfo;


/* =============================================================================
 * ���ú����ӿ�
 * =============================================================================
 */

/* ����PCB����ID */
PUBLIC int8u getPedcID(void);

/* �� IO ��Ϣ��ȡPEDC���� */
PUBLIC void scanPEDC_ID(void);

/* ���ⲿ��ȡ�ӻ������Ϣ */
PUBLIC tSlaveInfo* getSlaveInfoPtr(tEnmCommObj obj);

/* ��ȡͨ������״̬ 
 * FALSE ��ʾ�����ѶϿ���TRUE ��ʾ��������
 */
PUBLIC BOOL getObjCommLinkSts(tEnmCommObj obj);

/* ������ʱ���ܶ����IO�������ݣ����� MONITOR IBP PSL SIG PIC24A PIC24B  */
PUBLIC void mainIOProcess(void);

/* �ռ� DCU ״̬ AlarmParaTxToAvr */
PUBLIC void AlarmParaTxToAvr(void);

/* ���� MONTIOR ��� IO �����ݻ��� */
PUBLIC int8u *getPic24TestOutPtr(int * len);

PUBLIC BOOL readSlaveVerInfo(tEnmCommObj obj, char * pInfo, int * infoLen);

PUBLIC void resetSlaveInfo(void);

#endif  //_MAINIO_H


