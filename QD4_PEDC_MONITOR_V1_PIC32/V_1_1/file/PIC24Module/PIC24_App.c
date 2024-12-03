/*******************************************************************************
 * ����ͨ�� ģ��
 * -> ��������ô��ڳ�ʼ��      initPic24Comm 
 * -> ��ѭ�����ô���ͨ�Ŵ���    processPic24Comm 
 * 
 * 
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "waveflash.h"

#include "PIC24_App.h"
#include "IPSUart.h"
#include "IPS_App.h"
#include "mainIO.h"

#include "P24UpdateDrv.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* ������ 
 * 0 = �����л� A B
 * 1 = �̶�оƬ A
 * 2 = �̶�оƬ B
 *  */
#define DBG_FIX_CHIP    0       

/* =============================================================================
 * ���ر���
 */
PRIVATE tUartComm uartPic24;                              /* ����ͨ�ſ��� */
PRIVATE int8u RxPic24DataBuf[UART_COM_RXBUFSIZE];         /* ����ͨ�ſ���-���ջ��� */
PRIVATE int8u TxPic24DataBuf[UART_COM_TXBUFSIZE];         /* ����ͨ�ſ���-���ͻ��� */
    PUBLIC tUartComm * getPic24UartCommPtr(void)
    {
        return &uartPic24;
    }
    
    /* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
    PUBLIC BOOL ifPic24UartTxIsBusy(void)
    {
        return (BOOL)(uartPic24.uartSts.isSending);
    }
    
/* �������ݰ������� */

/* ��Ҫ��ѯ�Ķ����б� 
 * ������оƬ��ѯƵ�θ���
 */
PRIVATE const int8u pollingPic24List_A[] = 
{
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_B,
};
PRIVATE const int8u pollingPic24List_B[] = 
{
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_A,
};

PRIVATE tPollingTask PollingPic24;

/* =============================================================================*/
/* ������оƬ FALSE = A, TRUE = B */
PRIVATE BOOL MainWorkChipAB; 
PRIVATE BOOL prevWorkChipAB;

/* �ж�������оƬ
 * ���� TRUE ��ʾ������оƬ�� PIC24A
 *  */
PUBLIC BOOL ifMainWorkPic24A(void)      
{
    return (MainWorkChipAB == MAINWORK_PIC24A);   //(bPEDC_CNT_PIC18 == choicePIC18A) 
}

/* AB-CTR0 ����ѡ��������оƬ PIC24A �� B��������оƬ�����ȡ I2C ��չIO
 * A0_B1 
 *   = MAINWORK_PIC24A ѡ��A��
 *   = MAINWORK_PIC24B ѡ��B
 * PEDC_MONITOR_V1 : AB-CTR0( PE0 )
 * 
 * PIC24 ����������
 * 
 */
PRIVATE void selectWorkChipAB(BOOL A0_B1)
{
    tSlaveInfo * pSlave;
    
    mPORTESetPinsDigitalOut(BIT_0); 
    #if (DBG_FIX_CHIP == 1)
    {
        A0_B1 = MAINWORK_PIC24A;
    }
    #elif (DBG_FIX_CHIP == 2)
    {
        A0_B1 = MAINWORK_PIC24B;
    }
    #endif
    if(A0_B1 == MAINWORK_PIC24A)
    {
        mPORTEClearBits(BIT_0);

        MainWorkChipAB = MAINWORK_PIC24A;
    }
    else
    {
        mPORTESetBits(BIT_0);

        MainWorkChipAB = MAINWORK_PIC24B;
    }
    
    if(prevWorkChipAB != MainWorkChipAB)
    {
        prevWorkChipAB = MainWorkChipAB;
        
        if(MainWorkChipAB == MAINWORK_PIC24A)
        {
            pSlave = getSlaveInfoPtr(COMMOBJ_IORW_A);
        }
        else
        {
            pSlave = getSlaveInfoPtr(COMMOBJ_IORW_B);
        }
        
        /* �л���оƬ�Ժ���Ҫ����ǰ�漸�ε����� */
        pSlave->dataValidCnt   = 3;  
    }
}

/* COMM-CTR IO ����ѡ��ͨ�Ŷ��� PIC24A �� B
 *  tEnmCommObj = 
 *      COMMOBJ_IORW_A ѡ���� PIC24
 *      COMMOBJ_IORW_B ѡ��� PIC24
 * PEDC_MONITOR_V1 : COMM-CTR (PE7)
 */
PRIVATE tEnmCommObj communicationObj;
PUBLIC void setCommunObjAB(tEnmCommObj obj)
{
    #if (DBG_FIX_CHIP == 1)
    {
        obj = COMMOBJ_IORW_A;
    }
    #elif (DBG_FIX_CHIP == 2)
    {
        obj = COMMOBJ_IORW_B;
    }
    #else
    #endif
    
    communicationObj = obj;
    
    mPORTESetPinsDigitalOut(BIT_7); 
    
    if(obj == COMMOBJ_IORW_A)
    {
        mPORTEClearBits(BIT_7);
    }
    else if(obj == COMMOBJ_IORW_B)
    {
        mPORTESetBits(BIT_7);
    }
}

/* =========================================================
 * ͨ���жϴ����� : �� IBP PSL SIG ��ͨ��
 * ���룺
 *   module_id  : ����ID
 *   data       : һ���ֽ�����
 */
PRIVATE void Pic24_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    /* �������� �� ������ѯ����
     * ������ͬ��Э���������֡
     *  */
    Uni_UartReceiveData(PollingPic24.pollingCom, data);
}

/* =============================================================================
 * �����������Ƿ��� PCA9505 ����
 * 
 */
#define PCA9505ERR_MAX      10              /* ������������ﵽ����ֵ��ǿ���л� PIC24 */
PRIVATE int8u PCA9505ErrCnt[2] = {0, 0};    /* ������������� */

PRIVATE void checkPCA9505Err(int8u node)
{
    int8u inx;
    if(node == COMMOBJ_IORW_A)
        inx = 0;
    else if(node == COMMOBJ_IORW_B)
        inx = 1;
    else
        return;
    
    /* �ж� I2C ���� */
    tSlaveInfo * pSlvInfo = getSlaveInfoPtr(node);
    if(pSlvInfo != NULL)
    {
        if  ( (pSlvInfo->slaveLinkOK) && 
             ((pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err1) || 
              (pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err2) || 
              (pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err3))
            )
        {
            if(PCA9505ErrCnt[inx] < PCA9505ERR_MAX)
            {
                PCA9505ErrCnt[inx] += 1;
            }
        }
        else
        {
            PCA9505ErrCnt[inx] = 0;
        }
    }
    /* 10.13 �����ã�
     *  ����PIC24Aû����¼����ʱ��PIC32 ż�����յ�PIC24A������
     *  ���Խ����MONITOR-V1 �����⣬������ҵ�ڹ���ʹ�õģ�PIC24B����PCA9505ʧ��
    if(PCA9505ErrCnt[0] > 0)
    {
        PCA9505ErrCnt[0] = 0;
    }
     */
}

/* ��PIC24���� PCA9505 ���������
 * ���� 
 *  ��Ӧ�Ĵ��������ֵ 
 */
PUBLIC int8u getPCA9505ErrCnt(int8u node)
{
    if(node == COMMOBJ_IORW_A)
        return PCA9505ErrCnt[0];
    else if(node == COMMOBJ_IORW_B)
        return PCA9505ErrCnt[1];
    else
        return 0;
}

/* �ж�PIC24���� PCA9505 ����״̬
 * ���� 
 *  TRUE ��ʾ�д��� 
 */
PUBLIC BOOL ifPCA9505Err(int8u node)
{
    if( getPCA9505ErrCnt(node) >= PCA9505ERR_MAX )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* =============================================================================
 * ��ѯ�Ƿ�����Ҫ�������Ϣ
 * ����
 *  TRUE  ���մӻ���Ϣ��ȷ
 *  FALSE û�н��յ�����յ��������Ϣ
 */
PRIVATE BOOL Pic24_PollingProcRxMsg(void)
{
    BOOL ret;
    
    /* PIC24 Bootloader �����������̺�������ݣ��������ﴦ��
     *  */
    
    /* ������ѯ�����ݽ��� */
    ret = IPS_ProcRxFrame(&uartPic24, PollingPic24.sendPkgHeader.dstNode, TRUE);
    
    if(ret == TRUE)
    {
        checkPCA9505Err(PollingPic24.sendPkgHeader.dstNode);
    }
    
    return ret;
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART 
 * 
 *******************************************************************************
 *******************************************************************************/
PRIVATE void initPic24Comm(void)
{
    DeInitUartCom(COM_PIC24, &uartPic24);
    
    uartPic24.RxTxBuf.pRxBuf        = RxPic24DataBuf;           /* ����������ָ��               */
    uartPic24.RxTxBuf.rxBufSize     = sizeof(RxPic24DataBuf);   /* ������������С               */
    uartPic24.RxTxBuf.rxCnt         = 0;                        /* �ѽ��յ����ݳ���             */
  
    uartPic24.RxTxBuf.pTxBuf        = TxPic24DataBuf;           /* ����������ָ��               */
    uartPic24.RxTxBuf.txBufSize     = sizeof(TxPic24DataBuf);   /* ������������С               */
    uartPic24.RxTxBuf.txCnt         = 0;                        /* �ѷ������ݵĳ���             */
    uartPic24.RxTxBuf.txDataLen     = 0;                        /* ���������ݵĳ��ȣ�0��ʾû��  */
      
    uartPic24.funRx                 = &Pic24_UartReceiveData;   /* �����жϽ���һ���ֽں����           */
    uartPic24.funTx                 = NULL;                     /* �������ݽ������жϵ���, null��ʾû�� */
          
    uartPic24.RS485_4Wire           = TRUE;                     /* ����Ƕȣ�RS232���Կ���ȫ˫��RS485 */
    uartPic24.fpDE_Dis              = NULL;
    uartPic24.fpDE_En               = NULL;
    uartPic24.fpRE_Dis              = NULL;
    uartPic24.fpRE_En               = NULL;
      
    uartPic24.frameOverTime         = 5;            /* ms��֡���ʱ�䣬����ʱ����û��������Ϊ֡���� */
    uartPic24.sendOverTime          = 100;          /* ms�����÷������ݳ�ʱʱ�䳤��                 */
    uartPic24.abortFrameTime        = 500;          /* ms, ������ָ����ʱ��û����������֡����   */
    uartPic24.linkFailTime          = 3000;

    uartCOM_Init(COM_PIC24, UART_PIC24_ID, &uartPic24, UART_PIC24_BR, UART_PIC24_IPL);
}

/* =========================================================
 * ģ���ʼ��
 */
PUBLIC void initPic24Module(void)
{
    tSlaveInfo *pSlaveInfo;
    
    /* ���ڳ�ʼ�� */
    initPic24Comm();
    
    /* �洢����ʼ�� */
    memset((int8u*)&PollingPic24, 0, sizeof(PollingPic24));
    
    PollingPic24.pollingCom = &uartPic24;
    PollingPic24.pollingObjList = (int8u *)pollingPic24List_A;
    PollingPic24.pollingObjNum  = sizeof(pollingPic24List_A);
    PollingPic24.setWaitTXTime  = SELECTORPIC24_DELAY_MS;
    
    PollingPic24.fpCbProcRxPkg      = Pic24_PollingProcRxMsg;
    PollingPic24.PollingTurnTime    = PIC24_INTURN_TIME;
    PollingPic24.PollingTxOvTime    = PIC24_TX_OVERTIME;
    PollingPic24.ErrMaxNumOffLine   = PIC24_OFFLINE_NUM;
    
    /* ������PIC24 ѡ�� */
    MainWorkChipAB = MAINWORK_PIC24A;
    prevWorkChipAB = !MainWorkChipAB;
    selectWorkChipAB(MainWorkChipAB);
    
    /* ���� A �� B */
    memcpy((int8u*)(getSlaveInfoPtr(COMMOBJ_IORW_B)), getSlaveInfoPtr(COMMOBJ_IORW_A), sizeof(pSlaveInfo));
}


/* 
 * =============================================================================
 * �� PIC24 ͨ�Ŵ���
 * ��Ҫ����������ѭ������
 * =============================================================================
 */
#define UPDATE_INTERVAL_MS      10      /* ��ѯ����״̬���ʱ�� ms */
PUBLIC void processPic24Comm(void)
{
    static  tSYSTICK changePic24Time;
    static  tSYSTICK upPic24Tick;
    tSYSTICK tickNow;
    tSlaveInfo *pSlaveInfo;
    tSlaveInfo *pSlaveInfo_s;
    int8u pcaErrCnt;
    
    tickNow = getSysTick();
    
    
    #if (PIC24_BL_ENABLE == 1)
    if(chkIsUpdatingPic24() == TRUE)
    {
        /* ��ʱִ������ */
        if((tickNow - upPic24Tick) >= UPDATE_INTERVAL_MS)
        {
            upPic24Tick = tickNow;
            Pic24UpdateProcess(&uartPic24);
        }
        
        /* �Ƿ��д����������֡
         *  */
        upPIC24_ChkP24Frame();
    }
    else
    #endif
    {
        /* ��ѯ PIC24A + PIC24B
         */
        if(ifMainWorkPic24A())
        {
            pSlaveInfo  = getSlaveInfoPtr(COMMOBJ_IORW_A);
            pSlaveInfo_s= getSlaveInfoPtr(COMMOBJ_IORW_B);
            PollingPic24.pollingObjList = (int8u *)pollingPic24List_A;
            PollingPic24.pollingObjNum  = sizeof(pollingPic24List_A);
        }
        else
        {
            pSlaveInfo  = getSlaveInfoPtr(COMMOBJ_IORW_B);
            pSlaveInfo_s= getSlaveInfoPtr(COMMOBJ_IORW_A);
            PollingPic24.pollingObjList = (int8u *)pollingPic24List_B;
            PollingPic24.pollingObjNum  = sizeof(pollingPic24List_B);
        }

        /* �����յ�����֡�Ժ󣬴˺�����ص� Pic24_PollingProcRxMsg
         *  */
        Uni_PollingSlaves((void *)&PollingPic24);
    
        #if 1   //0=����  ����ʹ��
        /* ����ͨ��״̬���ж��Ƿ���Ҫ�л� PIC24���ӹ�ϵ 
         * ����PIC24��ͨ�Ŵ�������ﵽĳ��ֵ�󣬻����� PIC24 �� I2CоƬPCA9505ͨ�Ŵ���
         * �Զ��л�����һ��PIC24
         * ������ѯһ�ε�ʱ������ʱ��Ϊ 55ms (= IPS_INTURN_TIME + SELECTOR_DELAY_MS)
         * ÿ 4 ����ѯ��PIC24 3�Σ��ڳ��������Լ 220ms ��3�δ��󣬰� 30/3 * 220 = 2200ms
         * ���ͬһ��������ѯ IBP PSL SIG PIC24A PIC24B��ע�� PIC24_SWITCH_ERRNUM ������ֵ
         */
        if(ifMainWorkPic24A())
            pcaErrCnt = PCA9505ErrCnt[0];
        else
            pcaErrCnt = PCA9505ErrCnt[1];
    
        if((pSlaveInfo->commErrCnt >= PIC24_SWITCH_ERRNUM) || (pcaErrCnt >= PCA9505ERR_MAX))
        {
            /* Ϊ�˱���Ƶ���л���������С�л�ʱ���� */
            if((tSYSTICK)(tickNow - changePic24Time) >= 2000)
            {
                changePic24Time = tickNow;

                /* ֹͣ�շ� */
                uartResetRecv(uartPic24.usrComNo);
                uartStopSend(uartPic24.usrComNo);

                /* �л��� PIC24 */
                selectWorkChipAB(!MainWorkChipAB);
            }
        }
        #endif
    }
    
    /* ��ʱ������ߵ�PIC24�İ汾��Ϣ���ٴ����ߺ���Ҫ��ȡ�汾��Ϣ 
     * ��Ҫ�ǿ���������������Զ�������MON �Զ��ٴ��ض�
     */
    static tSYSTICK tickReadVer;
    tSlaveInfo* pSlvInfo;
    if((tSYSTICK)(tickNow - tickReadVer) > 100)
    {
        tickReadVer = tickNow;
        
        if(getObjCommLinkSts(COMMOBJ_IORW_A) == FALSE)
        {
            pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_IORW_A);
            pSlvInfo->VersionFlag = 0;
        }

        if(getObjCommLinkSts(COMMOBJ_IORW_B) == FALSE)
        {
            pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_IORW_B);
            pSlvInfo->VersionFlag = 0;
        }
        
        
        /* �жϴ�������״̬������״̬�Ƿ��쳣 
         * ������һ��PIC24ͨ�ųɹ����򱾻� UART OK
         */
        if(uartChkLinkAndSendSts(COM_PIC24) == TRUE)
        {
            /* ��⴮��ͨ�Ŵ����־
             * �����������³�ʼ������
             *  */
            if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART_PIC24_ID)) )
            {
                INTClearFlag(INT_SOURCE_UART_ERROR(UART_PIC24_ID));
                initPic24Comm();
            }
        }
    }
}

/* =============================================================================
 * ��ȡ�������ָ��
 *    PC ����������ʱ����ɺ�(testPic24OutputMs = 0)����Ҫ�л������������ֵ
 */
PUBLIC int8u * getPic24OutputIODataPtr(int * len)
{
    *len = sizeof(MON_output_data);
    
    int8u *pSrc;
    if(chkIfTestOutputIO() == FALSE)
    {
        pSrc = (int8u*)&MON_output_data[0].byte;
    }
    else
    {
        pSrc = getTestOutputIOData();
    }
    
    return pSrc;
}

#if 0
/* =============================================================================
 * ���� PIC24 ����� IO ���� (A and B)
 */
PUBLIC int updatePic24Outdata(int8u *outData, int8u outLen)
{
    int ret = 0;
    //TODO
    tSlaveInfo *pic24Info;
    
    //PIC24A
    pic24Info = (tSlaveInfo *)getSlaveInfoPtr(COMMOBJ_IORW_A);
    if(pic24Info != NULL)
    {
        if(outLen <= sizeof(pic24Info->slaveOutput.wrBuf))
        {
            pic24Info->WrDataLen = outLen;
            memcpy((int8u*)pic24Info->slaveOutput.wrBuf, outData, outLen);
            ret |= 1;
        }
    }
    
    //PIC24B
    pic24Info = (tSlaveInfo *)getSlaveInfoPtr(COMMOBJ_IORW_B);
    if(pic24Info != NULL)
    {
        if(outLen > sizeof(pic24Info->slaveOutput.wrBuf))
        {            
            pic24Info->WrDataLen = outLen;
            memcpy((int8u*)pic24Info->slaveOutput.wrBuf, outData, outLen);
            ret |= 2;
        }
    }
    
    return ret;
}
#endif

