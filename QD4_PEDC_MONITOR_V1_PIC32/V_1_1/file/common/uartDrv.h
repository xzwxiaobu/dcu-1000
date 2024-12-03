#ifndef _UARTDRV_H
#define _UARTDRV_H

#include "_cpu.h"

#if (defined(PCB_PIC24))
#include "uart.h"

/* Fcy = 16M, UART initialized to 8bit,no parity and 1 stopbit
 * UARTx BAUD RATE WITH BRGH = 0
 *   UxBRG = ((FCY/Desired Baud Rate)/16) - 1
 * UARTx BAUD RATE WITH BRGH = 1
 *   UxBRG = ((FCY/Desired Baud Rate)/4) - 1  
 * 
 *   ע����㿼���������룬������ܲ�����ƫ�����
 * BRGH = 0
 *   ���ò�����     ����ֵ     ʵ�ʲ�����               ƫ��
 *   56000          17        16M/(16*18)=55555.56      -0.79%      OK
 *   115200         9         16M/(16*10)=100000        -13.19%     NG
 *   115200         8         16M/(16*9) =111111.11     -3.55%      NG
 *   115200         7         16M/(16*8) =125000         8.51%      NG
 * BRGH = 1
 *   ���ò�����     ����ֵ     ʵ�ʲ�����               ƫ��
 *   56000          70        16M/(4*71)=56338.03       0.6%
 *   115200         33        16M/(4*34)=117647.06      2.12%
 *   115200         34        16M/(4*35)=114285.71     -0.79%
 */
/* ���㲨��������ֵ     U2_BRGVAL = ((((FCY * 10) / U2_BAUDRATE )/16 + 5)/10 - 1) */
#define FCY             16000000        /* 16MHz */
#define U2_BRGVAL       34              /* 115200 @BRGH = 1 */

typedef enum
{
    UART_1   = 1,
    UART_2   = 2,
}UART_MODULE;

typedef enum
{
    PRI_   = 0,
}INT_PRIORITY;

#else
#include <plib.h>
#include "peripheral\uart.h"
#endif

#include "sysTimer.h"
#include "uartRxBuff.h"
#include "uartDMA.h"
#include "usrCRC.h"

#define UART_RX_SOFTBUFF            /* ʹ��������洮�ڽ������ݣ����ͽ����ж�ռ��CPUʱ�� */
#define UART_TX_BY_DMA              /* ����ͨ�� DMA �������ݣ����ⷢ���ж�ռ�� CPUʱ��   */


/* =========================================================
 * ���� COM ͨ�Ŷ˿ں�
 * ע��: COM ���� UART �ź��岻��ͬ
 *       tEnmComList �ж���� COM  ��������ı��
 *       UART ��������ģ��� CPU ʵ�ʵĴ�����Դ������ģ�����ж��� (tUartComm.uartID)
 */

typedef enum
{
    COM_MMS = 0,        /* �� MMS ͨ��              */
    COM_PIC24,          /* �� PIC24оƬͨ��         */
    COM_IPS,            /* �� IBP PSL SIG ��ͨ��    */
    COM_SIGSYS1,        /* �� �ź�ϵͳͨ��          */
    COM_SIGSYS2,        /* �� �ź�ϵͳͨ��          */
    COM_BASSYS ,        /* �� BAS ϵͳͨ��          */
    
    COM_NUM,            /* �Զ����� COM ����        */
}tEnmComList; 

typedef enum
{
    COM_TX_OK = 0,          /* �������ڷ������ݳɹ�                 */
    COM_TX_STOPPED,         /* �����Ѿ�ֹͣ����                     */
    COM_TX_ERR_PORTNUM,     /* ��֧�ֵ�COM��  �����COM �Ŵ���    */
    COM_TX_ERR_UNINIT,      /* COM��ָ��Ĵ���δ��ʼ��              */
    COM_TX_ERR_BUSY,        /* COM��ָ��Ĵ��ڻ���δ��ɵķ�������  */
    COM_TX_ERR_OVERSIZE,    /* ���������ݵĳ��ȳ������ͻ�������С   */
}tEnmComTxSts; 

typedef enum
{
    COM_INI_OK = 0,         /* ��ʼ�������óɹ�                     */
    COM_INI_ERR_PORTNUM,    /* ��֧�ֵ�COM��    �����COM �Ŵ���  */
    COM_INI_ERR_COMID,      /* UART�˿ںŲ����� ��Ӳ��COM ID����  */
    COM_INI_ERR_OCCUPY,     /* UART�˿ں��Ѿ���ռ��                 */
    COM_INI_ERR_PARA,       /* ����Ĳ���                 */
}tEnmComIniSts; 


typedef enum
{
    PROT_UNDEF  = 0,
    PROT_MODBUS    ,        /* �ײ�Э��=MODBUS */
}tEnmUartProtocol;


/* ���崮���շ��������ṹ��
 */
#pragma pack(2)
typedef struct
{
    int8u * pRxBuf;                     /* ����������ָ��               */
    volatile int16u rxBufSize;          /* ������������С               */
    volatile int16u rxCnt;              /* �ѽ��յ����ݳ���             */
                                        
    int8u * pTxBuf;                     /* ����������ָ��               */
    volatile int16u txBufSize;          /* ������������С               */
    volatile int16u txCnt;              /* �ѷ������ݵĳ���             */
    volatile int16u txDataLen;          /* ���������ݵĳ��ȣ�0��ʾû��  */
}tUartBuf;
#pragma pack()


/* ���崮�ڹ��ܹ���ṹ��
 */
typedef struct
{
    void (* fpRE_En) (void);            /* ����ָ�� RE enable   */
    void (* fpRE_Dis)(void);            /* ����ָ�� RE disable  */
    void (* fpDE_En) (void);            /* ����ָ�� DE enable   */
    void (* fpDE_Dis)(void);            /* ����ָ�� DE disable  */
    
    void (* funRx)  (int8u, int8u);     /* ����ָ�� �������ݴ����жϵ��� */
    void (* funTx)  (void)  ;           /* ����ָ�� �������ݽ������жϵ��� */
    
    int32u      usrComNo;               /* �Զ���Ĵ��ڱ�� */
    UART_MODULE uartID;                 /* ���� ID : 4�ֽ� */
    BOOL        RS485_4Wire;            /* true = RS485_4W, else RS485_2W : 4�ֽ� */
    int32u      delayForStopTx;
    
    volatile tSYSTICK lastByteTick;     /* ��һ�ζ����ֽڵ�ʱ��� (ms)      */
    volatile tSYSTICK gotFrameTick;     /* ��һ�εõ�һ֡����(ͨ�ųɹ�)��ʱ��� (ms)  */
    volatile tSYSTICK abortFrameTime;   /* ���һ֡������������ʱ������ (ms)��0 ��ʾ��Ч    */
    volatile tSYSTICK frameOverTime;    /* ����ͨ��֡���ʱ��(ms)�����ڴ�ʱ���������������¿�ʼ����֡��0 ��ʾ��Ч  */
    volatile tSYSTICK startSendTime;    /* ��¼�������͵�ʱ��(ms)��                                                */
    volatile tSYSTICK sendOverTime;     /* ���÷��ͳ�ʱ��ʱ��(ms)�����ڴ�ʱ��������������Ϊͨ�Ŵ���  0 ��ʾ��Ч  */
    
    volatile tSYSTICK linkFailTime;     /* ͨ�����Ӵ���ʱ�����ã���ʱ����Ϊ��Է�ͨ�ŶϿ�   */
    
    tUartBuf    RxTxBuf;
    struct 
    {
        volatile int16u linkState   : 1;    /* 0=��Է�ͨ�ŶϿ�         */
        volatile int16u isSending   : 1;    /* 1=��ʾ����ִ�з���       */
        volatile int16u gotFrame    : 1;    /* 1=��ʾ���յ�����������֡ */
        volatile int16u errFrame    : 1;    /* 1=��ʾ��������֡����     */
        volatile int16u TxOVTM      : 1;    /* 1=����ͨ�ų�ʱ����       */
        volatile int16u receiving   : 1;    /* 1=���ڽ�������           */
        volatile int16u             : 10;
    }uartSts;
    
    /* ����ͨ�ŵײ��װ����Э�� 
     * ����δ��
     */
    tEnmUartProtocol uartProtocol;
    
}tUartComm;


/* =========================================================
 * ����
 */
 
/* ��������
 * ����
 *      ComNo    : COM��
 * ����
 *      ���ý��
 * */
PUBLIC tEnmComIniSts DeInitUartCom(tEnmComList ComNo, tUartComm *pUartCOM);

/* ���ڳ�ʼ������
 * ����
 *   ComNo     : COM �ڱ�ţ����� UART ��ţ�
 *   pUartCOM  : ָ�򴮿ڽṹ��
 *   dataRate  : ���ڲ�����
 *   priority  : �ж����ȼ�
 * ����
 *      ��ʼ�����
 */
PUBLIC tEnmComIniSts uartCOM_Init(tEnmComList virCom, UART_MODULE phy_id, tUartComm * pUartCOM,  int32u dataRate, INT_PRIORITY priority);

/* �������ڷ�������
 * ����
 *      ComNo    : COM��
 *      sendData : ��Ҫ���͵�����
 *      sendLen  : ��Ҫ���͵����ݵĳ���
 * ����
 *      ����״̬
 * */
PUBLIC tEnmComTxSts uartStartSend(tEnmComList ComNo, int8u *sendData, int16u sendLen);

/* ֹͣ���ڷ�������
 * ����
 *      ComNo    : COM��
 * ����
 *      ����״̬
 * */
PUBLIC tEnmComTxSts uartStopSend(tEnmComList ComNo);


/* ��ȡ���ڷ�������״̬
 * ����
 *      ComNo    : COM��
 * ����
 *      ����״̬
 * */
PUBLIC tEnmComTxSts uartGetSendState(tEnmComList ComNo);

/* ���ô��ڽ��յ�����
 */
PUBLIC BOOL uartResetRecv(tEnmComList ComNo);

/* �ж�����״̬�����ͳ�ʱ 
 */
PUBLIC BOOL uartChkLinkAndSendSts(tEnmComList ComNo);

/* ��鴮�ڴ����������������������Ҫ��ʱ���� 50ms������
 */
PUBLIC int checkAllUartErrorSta(void);

//***********************************************************
#endif  //_UARTDRV_H

