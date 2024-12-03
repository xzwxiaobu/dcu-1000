#include "..\Include.h"

#include "ModbusDef.h"
#include "BASSys_Uart.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif
PRIVATE tUartComm uartBASSys;                               /* ����ͨ�ſ��� */

PRIVATE int8u RxBASSysDataBuf[UART_BASSYS_RXBUFSIZE];       /* ����ͨ�ſ���-���ջ��� */
PRIVATE int8u TxBASSysDataBuf[UART_BASSYS_TXBUFSIZE];       /* ����ͨ�ſ���-���ͻ��� */

/* =============================================================================*/
/* ��ȡBASϵͳ����״̬ ��FALSE ��ʾ���ߣ�TRUE ��ʾ���� */
PUBLIC BOOL getBASSysOnlineSts(void)
{
    return (BOOL)(uartBASSys.uartSts.linkState);
}

/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
PUBLIC BOOL ifBASSysUartTxIsBusy(void)
{
    return (BOOL)(uartBASSys.uartSts.isSending);
}

/* =============================================================================*/
PRIVATE void uartBAS_DE_Dis(void)
{
    DE_BASSYS_DISABLE();
}

PRIVATE void uartBAS_DE_En(void)
{
    DE_BASSYS_ENABLE();
}

PRIVATE void uartBAS_RE_Dis(void)
{
    RE_BASSYS_DISABLE();
}

PRIVATE void uartBAS_RE_En(void)
{
    RE_BASSYS_ENABLE();
}

/* =============================================================================
 * ͨ���жϴ����� : ���ź�ϵͳͨ��
 * ���룺
 *   module_id  : ����ID
 *   data       : һ���ֽ�����
 */
PRIVATE void BASSys_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    tSYSTICK    nowTick;
    tUartComm * pCom = &uartBASSys;
    
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
PRIVATE void initBASSysComm(void)
{
    DeInitUartCom(COM_BASSYS, &uartBASSys);
    
    uartBASSys.RxTxBuf.pRxBuf       = RxBASSysDataBuf;          /* ����������ָ��               */
    uartBASSys.RxTxBuf.rxBufSize    = sizeof(RxBASSysDataBuf);  /* ������������С               */
    uartBASSys.RxTxBuf.rxCnt        = 0;                        /* �ѽ��յ����ݳ���             */
  
    uartBASSys.RxTxBuf.pTxBuf       = TxBASSysDataBuf;          /* ����������ָ��               */
    uartBASSys.RxTxBuf.txBufSize    = sizeof(TxBASSysDataBuf);  /* ������������С               */
    uartBASSys.RxTxBuf.txCnt        = 0;                        /* �ѷ������ݵĳ���             */
    uartBASSys.RxTxBuf.txDataLen    = 0;                        /* ���������ݵĳ��ȣ�0��ʾû��  */
      
    uartBASSys.funRx                = &BASSys_UartReceiveData;  /* �����жϽ���һ���ֽں����           */
    uartBASSys.funTx                = NULL;                     /* �������ݽ������жϵ���, null��ʾû�� */
          
    uartBASSys.RS485_4Wire          = RS485_BASSYS_4W;          /* TRUE = ȫ˫��RS485 */
    uartBASSys.fpDE_Dis             = uartBAS_DE_Dis;
    uartBASSys.fpDE_En              = uartBAS_DE_En;
    uartBASSys.fpRE_Dis             = uartBAS_RE_Dis;
    uartBASSys.fpRE_En              = uartBAS_RE_En;
      
    /* ֡���ʱ�䣺�� MODBUS Э�飬֮֡��ļ������С�� 3.5���ֽڴ���ʱ��
     * 
     * ���� 9600bps����ÿ���ֽڴ���ʱ��Լ 1.05ms����֡���ʱ�䲻�õ���Լ 3.7ms
     * �����ֹ ��ֹ�ײ�֡���ʱ�� ��ʱ�����½��ռ����������ж� MODBUS Э������֡����
     */
    uartBASSys.frameOverTime        = 0;            /* ms����ֹ�ײ�֡���ʱ�䣬����ʱ����û��������Ϊ֡���� */
    uartBASSys.sendOverTime         = 200;          /* ms�����÷������ݳ�ʱʱ�䳤��                 */
    uartBASSys.abortFrameTime       = 500;          /* ms, ������ָ����ʱ��û����������֡����   */
    uartBASSys.linkFailTime         = BASSYS_OFFLIN_TIME;         /* ����ʱ�� */

    uartCOM_Init(COM_BASSYS, UART_BASSYS_ID, &uartBASSys, UART_BASSYS_BR, UART_BASSYS_IPL);
}


/* =========================================================
 * �ж��Ƿ���У����ȷ�Ľ�������֡
 * ���У�����Ӧ�ò㴦��
 */
PUBLIC BOOL procBASSysRxFrame(cbAppBASSysProcFrame BASSysApp)
{
    BOOL sts = FALSE;
    tUartComm * pCom = &uartBASSys;
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
            BASSysApp(pCom->RxTxBuf.pRxBuf, pCom->RxTxBuf.rxCnt);
            
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
PUBLIC void initBASSysModule(void)
{
    /* ���ڳ�ʼ�� */
    uartBAS_DE_Dis();
    uartBAS_RE_Dis();
    
    initBASSysComm();
    
    uartBAS_RE_En();
}

/* =========================================================
 * ������ BASϵͳ ��������
 */
PUBLIC BOOL  sendDataFrameToBASSys(int8u *pTxData, int TxLen)
{
    BOOL ret = FALSE;
    
    if(uartStartSend(uartBASSys.usrComNo, pTxData, TxLen) == COM_TX_OK)
    {
        ret = TRUE;
    }
}

/* =========================================================
 * ͨ����ʱ���ж�BASϵͳ�ķ�������֡�Ƿ����
 */
PUBLIC BOOL chkBASSysFinishSend(void)
{
    tSYSTICK nowTick;
    
    nowTick = getSysTick();
    
    if(uartBASSys.uartSts.receiving != 0)
    {
        /* ���ڽ��� 
         * �����յ����һ���ֽڹ���ָ��ʱ�����Ϊ�������
         * ��ʱ�жϽ��յĳ����Ƿ���� MODBUS ��С֡�ĳ���
         * �У�����Ҫ����
         */
        if((tSYSTICK)(nowTick - uartBASSys.lastByteTick) >= MODBUS_FRAME_FINISH_TIME)
        {
            if(uartBASSys.uartSts.gotFrame == 0)
            {
                if(uartBASSys.RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
                {
                    /* �������ݳ���������СҪ����Ҫ���� */
                    uartBASSys.uartSts.gotFrame  = 1;
                }
                else
                {
                    /* �������ݳ��Ȳ�������СҪ�����½��� */
                    uartBASSys.RxTxBuf.rxCnt = 0;
                }
            }
            
            uartBASSys.uartSts.receiving = 0;
        }
    }
}


