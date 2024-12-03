#include "..\Include.h"

#include "ModbusDef.h"
#include "SigSys_Uart.h"

PRIVATE tUartComm uartSigSys;                               /* ����ͨ�ſ��� */

PRIVATE int8u RxSigSysDataBuf[UART_SIGSYS_RXBUFSIZE];       /* ����ͨ�ſ���-���ջ��� */
PRIVATE int8u TxSigSysDataBuf[UART_SIGSYS_TXBUFSIZE];       /* ����ͨ�ſ���-���ͻ��� */

/* =============================================================================*/
/* ��ȡ�ź�ϵͳ����״̬ ��FALSE ��ʾ���ߣ�TRUE ��ʾ���� */
PUBLIC BOOL getSigSysOnlineSts(void)
{
    return (BOOL)(uartSigSys.uartSts.linkState);
}

/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
PUBLIC BOOL ifSigSysUartTxIsBusy(void)
{
    return (BOOL)(uartSigSys.uartSts.isSending);
}

/* =============================================================================*/
PRIVATE void uartSig_DE_Dis(void)
{
    DE_SIGSYS_DISABLE();
}

PRIVATE void uartSig_DE_En(void)
{
    DE_SIGSYS_ENABLE();
}

PRIVATE void uartSig_RE_Dis(void)
{
    RE_SIGSYS_DISABLE();
}

PRIVATE void uartSig_RE_En(void)
{
    RE_SIGSYS_ENABLE();
}

/* =============================================================================
 * ͨ���жϴ����� : ���ź�ϵͳͨ��
 * ���룺
 *   module_id  : ����ID
 *   data       : һ���ֽ�����
 */
PRIVATE void SigSys_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    tSYSTICK    nowTick;
    tUartComm * pCom = &uartSigSys;
    
    tModbusFixHeader * mb_Header;
    tModbusWrHeader  * wrHeader;
    BOOL rxedFrame = FALSE;
    
    nowTick = getSysTick();

    /* ���յ����ϴ�����û�д���
     * ������ָ����ʱ��û����������֡����
     */
    if(pCom->uartSts.gotFrame != 0)
    {
        if(pCom->abortFrameTime > 0)
        {
            if((tSYSTICK)(nowTick - pCom->gotFrameTick) >= pCom->abortFrameTime)
            {
                pCom->uartSts.receiving = 0;
                pCom->RxTxBuf.rxCnt     = 0;
                pCom->uartSts.gotFrame  = 0;
                pCom->gotFrameTick      = nowTick;
            }
        }
        
        if(pCom->uartSts.gotFrame != 0)
        {
            return;
        }
    }
    
    /* 1. ���յ����ݳ��ȳ������ջ���������������ս������¼��� 
     * 2. ���ֽ�֮��Ľ���ʱ�����ƣ�����ʱ������Ϊ���µ�һ֡���ݣ����½��ռ��� 
     * 
     * �Ѿ��������㴦��uartDrv.c --> lastByteTick��
     */

    pCom->uartSts.receiving = 1;

    /* save the received data */
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = data;
    pCom->RxTxBuf.rxCnt ++;

    if(pCom->RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
    {
        mb_Header = (tModbusFixHeader *)(pCom->RxTxBuf.pRxBuf);
        
        switch(mb_Header->funID)
        {
            /* ���Ĵ��������Ȧ */
            case fun_03_ReadHoldingRegisters:
            case fun_04_ReadinputRegisters:
                //if(pCom->RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail))
                {
                    rxedFrame = TRUE;
                    pCom->uartSts.gotFrame = 1;
                }
                break;

            case fun_16_WriteMultipleRegisters:
                wrHeader = (tModbusWrHeader *)(pCom->RxTxBuf.pRxBuf);

                //len = pCom->RxTxBuf.pRxBuf[mb_ByteCount] + mb_fun0x10_request_fix_len;
                //if((pCom->RxTxBuf.rxCnt >= len) && (pCom->RxTxBuf.pRxBuf[mb_ByteCount] <= mb_fun0x10_request_dataBUF_LEN))
                if(pCom->RxTxBuf.rxCnt >= (wrHeader->dataLen + sizeof(tModbusWrHeader) + sizeof(tModbusTail)))
                {
                    rxedFrame = TRUE;
                }
                break;

            default:
                pCom->RxTxBuf.rxCnt = 0;
                break;		
        }
    }
    else
    {
        /* MODBUS RTU Э��
         * 1  : �豸��ַ
         * 1  : ������ 
         * 2  : ���ʼĴ�����ַ
         * 2  : ���ʼĴ�������
         * 1  : �ֽ���
         * N  : ����
         * 2  : CRC У����(���ֽڣ����ֽ�)
         */
        switch(pCom->RxTxBuf.rxCnt)
        {
            case 1:     /* �豸��ַ */
                if(data != LOCAL_DEV_ADDR)
                {
                    pCom->RxTxBuf.rxCnt = 0;
                }
                break;
                
            case 2:     /* ������ */
                break;
        }
    }
    
    if(rxedFrame)
    {
        /* �õ�����������֡
         * ��¼��ǰ��ʱ��� 
         */
        pCom->uartSts.gotFrame  = 1;
        pCom->gotFrameTick      = nowTick;

        /* ���ﲻ����Ϊͨ����������Ϊ����֡��û�н���У�� 
         * pCom->uartSts.linkState = 1;
         */
    }
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART 
 * 
 *******************************************************************************
 *******************************************************************************/
PRIVATE void initSigSysComm(void)
{
    DeInitUartCom(COM_SIGSYS1, &uartSigSys);
    
    uartSigSys.RxTxBuf.pRxBuf       = RxSigSysDataBuf;          /* ����������ָ��               */
    uartSigSys.RxTxBuf.rxBufSize    = sizeof(RxSigSysDataBuf);  /* ������������С               */
    uartSigSys.RxTxBuf.rxCnt        = 0;                        /* �ѽ��յ����ݳ���             */
  
    uartSigSys.RxTxBuf.pTxBuf       = TxSigSysDataBuf;          /* ����������ָ��               */
    uartSigSys.RxTxBuf.txBufSize    = sizeof(TxSigSysDataBuf);  /* ������������С               */
    uartSigSys.RxTxBuf.txCnt        = 0;                        /* �ѷ������ݵĳ���             */
    uartSigSys.RxTxBuf.txDataLen    = 0;                        /* ���������ݵĳ��ȣ�0��ʾû��  */
      
    uartSigSys.funRx                = &SigSys_UartReceiveData;  /* �����жϽ���һ���ֽں����           */
    uartSigSys.funTx                = NULL;                     /* �������ݽ������жϵ���, null��ʾû�� */
          
    uartSigSys.RS485_4Wire          = RS485_SIGSYS_4W;          /* TRUE = ȫ˫��RS485 */
    uartSigSys.fpDE_Dis             = uartSig_DE_Dis;
    uartSigSys.fpDE_En              = uartSig_DE_En;
    uartSigSys.fpRE_Dis             = uartSig_RE_Dis;
    uartSigSys.fpRE_En              = uartSig_RE_En;
      
    /* ֡���ʱ�䣺�� MODBUS Э�飬֮֡��ļ������С�� 3.5���ֽڴ���ʱ��
     * 
     * ���� 9600bps����ÿ���ֽڴ���ʱ��Լ 1.05ms����֡���ʱ�䲻�õ���Լ 3.7ms
     * �����ֹ ��ֹ�ײ�֡���ʱ�� ��ʱ�����½��ռ����������ж� MODBUS Э������֡����
     */
    uartSigSys.frameOverTime        = 0;            /* ms����ֹ�ײ�֡���ʱ�䣬����ʱ����û��������Ϊ֡���� */
    uartSigSys.sendOverTime         = 200;          /* ms�����÷������ݳ�ʱʱ�䳤��                 */
    uartSigSys.abortFrameTime       = 500;          /* ms, ������ָ����ʱ��û����������֡����   */
    uartSigSys.linkFailTime         = SIGSYS_OFFLIN_TIME;         /* ����ʱ�� */

    uartCOM_Init(COM_SIGSYS1, UART_SIGSYS_ID, &uartSigSys, UART_SIGSYS_BR, UART_SIGSYS_IPL);
}


/* =========================================================
 * �ж��Ƿ���У����ȷ�Ľ�������֡
 * ���У�����Ӧ�ò㴦��
 */
PUBLIC BOOL procSigSysRxFrame(cbAppSigSysProcFrame sigSysApp)
{
    BOOL sts = FALSE;
    tUartComm * pCom = &uartSigSys;
    unsigned short CRC;
    unsigned short rxCRC;
    tModbusTail * pTail;
    
    if(pCom->uartSts.gotFrame)
    {
        CRC = CRC16_Modbus((int8u *)(pCom->RxTxBuf.pRxBuf), pCom->RxTxBuf.rxCnt - 2);
        
        pTail = (tModbusTail * )(pCom->RxTxBuf.pRxBuf + pCom->RxTxBuf.rxCnt - 2);
        rxCRC = (unsigned short)(pTail->crcL) + ((unsigned short)(pTail->crcH) << 8);
        if(CRC == rxCRC)
        {
            /* ��Է�ͨ������ */
            pCom->uartSts.linkState = 1;
            
            /* �ص���Ӧ�ò㴦��������� */
            sigSysApp(pCom->RxTxBuf.pRxBuf, pCom->RxTxBuf.rxCnt);
            
            sts = TRUE;
        }	
        
        pCom->uartSts.receiving = 0;
        pCom->RxTxBuf.rxCnt     = 0;
        pCom->uartSts.gotFrame  = 0;
    }
    
    return sts;
}


/* =========================================================
 * ģ���ʼ��
 */
PUBLIC void initSigSysModule(void)
{
    /* ���ڳ�ʼ�� */
    uartSig_DE_Dis();
    uartSig_RE_Dis();
    
    initSigSysComm();
    
    uartSig_RE_En();
    uartSig_DE_En();
}

/* =========================================================
 * ������ �ź�ϵͳ ��������
 */
PUBLIC BOOL  sendDataFrameToSisSys(int8u *pTxData, int TxLen)
{
    BOOL ret = FALSE;
    
    if(uartStartSend(uartSigSys.usrComNo, pTxData, TxLen) == COM_TX_OK)
    {
        ret = TRUE;
    }
}

/* =========================================================
 * ͨ����ʱ���ж��ź�ϵͳ�ķ�������֡�Ƿ����
 */
PUBLIC BOOL chkSigSysFinishSend(void)
{
    tSYSTICK nowTick;
    
    nowTick = getSysTick();
    
    if(uartSigSys.uartSts.receiving != 0)
    {
        /* ���ڽ��� 
         * �����յ����һ���ֽڹ���ָ��ʱ�����Ϊ�������
         * ��ʱ�жϽ��յĳ����Ƿ���� MODBUS ��С֡�ĳ���
         * �У�����Ҫ����
         */
        if((tSYSTICK)(nowTick - uartSigSys.lastByteTick) >= MODBUS_FRAME_FINISH_TIME)
        {
            if(uartSigSys.uartSts.gotFrame == 0)
            {
                if(uartSigSys.RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
                {
                    /* �������ݳ���������СҪ����Ҫ���� */
                    uartSigSys.uartSts.gotFrame  = 1;
                }
                else
                {
                    /* �������ݳ��Ȳ�������СҪ�����½��� */
                    uartSigSys.RxTxBuf.rxCnt = 0;
                }
            }
            
            uartSigSys.uartSts.receiving = 0;
        }
    }
}


