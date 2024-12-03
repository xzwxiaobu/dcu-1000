
#include "_cpu.h"
#include "uartDrv.h"
#include "sysTimer.h"

#define PERR_MASK           (1<<3)
#define FERR_MASK           (1<<2)
#define OERR_MASK           (1<<1)
#define UART_X_ERR_MASK     (PERR_MASK | FERR_MASK | OERR_MASK)

/* MAX COM numer = 0 ~ 5 */
//tUartComm * UsedUart[6];// = {NULL ,NULL ,NULL ,NULL ,NULL ,NULL};

/* �������ڶ�Ӧ�Ŀ����� */
#define PHY_UART_NUM    6
tUartComm * PhyUartArray[PHY_UART_NUM] = {NULL ,NULL ,NULL ,NULL ,NULL ,NULL};

#ifdef  UART_RX_SOFTBUFF
PRIVATE void processUartIF_Tx(int32u uart_id);
#endif

#ifdef  UART_TX_BY_DMA
PRIVATE void processUartDMA_FinishedTx(int32u uart_id);
#endif

/* =============================================================================
 * ���⴮�ںŲ��Ҷ�Ӧ�������ں�
 * ����
 *      virCom   : ���� COM��
 *  */
BOOL uartVirID2PhyID(tEnmComList virCom, UART_MODULE * phyCom)
{
    BOOL ret = FALSE;
    int i;
    for(i=0; i<PHY_UART_NUM; i++)
    {
        if(PhyUartArray[i] != NULL)
        {
            if(PhyUartArray[i]->usrComNo == virCom)
            {
                if(PhyUartArray[i]->uartID <= UART6)
                {
                    *phyCom = PhyUartArray[i]->uartID;
                    ret     = TRUE;
                }
                break;
            }
        }
    }
    
    return ret;
}


/* =============================================================================
 * �������ڷ�������
 * ����
 *      virCom   : ���� COM��
 *      sendData : ��Ҫ���͵�����
 *      sendLen  : ��Ҫ���͵����ݵĳ���
 * ����
 *     ����״̬ tEnmComTxSts
 * */
PUBLIC tEnmComTxSts uartStartSend(tEnmComList virCom, int8u *sendData, int16u sendLen)
{
    int i;
    int8u *pDst;
    tEnmComTxSts ret = COM_TX_OK;
    UART_MODULE phy_id;

    tUartComm * pUartCOM;
    if(uartVirID2PhyID(virCom, &phy_id) == FALSE)
    {
        return COM_TX_ERR_UNINIT;
    }
    pUartCOM = PhyUartArray[phy_id];
    
    if((pUartCOM->uartSts.isSending != 0))
    {
        ret = COM_TX_ERR_BUSY;
    }
    else if(sendLen > pUartCOM->RxTxBuf.txBufSize)
    {
        ret = COM_TX_ERR_OVERSIZE;
    }
        
    if(ret == COM_TX_OK)
    {
        /* ���� 2W-485������������Ҫ�л�������״̬ */
        if(pUartCOM->RS485_4Wire == FALSE)
        {
            if(pUartCOM->fpRE_Dis != NULL)
                pUartCOM->fpRE_Dis();
            
            if(pUartCOM->fpDE_En != NULL)
            {
                pUartCOM->fpDE_En();
                for(i=0; i<100; i++)
                {
                    asm volatile ("nop");
                }
                
            }
        }
        else
        {
            if(pUartCOM->fpDE_En != NULL)
            {
                pUartCOM->fpDE_En();
            }
            if(pUartCOM->fpRE_En != NULL)
                pUartCOM->fpRE_En();
        }
                
        phy_id = pUartCOM->uartID;
            
        /* copy data to send buffer 
         * ��������͵�����ָ��ָ�򴮿��Լ��ķ��ͻ����������������ƹ���
         * ��������Ӧ�ò����ֱ���򴮿ڱ���ķ��ͻ������������
         */
        pDst = (int8u *)pUartCOM->RxTxBuf.pTxBuf;
        if(pDst != sendData)
        {
            for(i=0; i<sendLen; i++)
            {
                *pDst++ = *sendData++;
            }
        }
        
        #ifdef UART_TX_BY_DMA
        {
            INTClearFlag(INT_SOURCE_UART_TX(phy_id));
            INTEnable(INT_SOURCE_UART_TX(phy_id), INT_DISABLED);
            
            tUartDMASetting uartDMA;
            uartDMA.uartID    = phy_id;
            uartDMA.pTxData   = pUartCOM->RxTxBuf.pTxBuf;
            uartDMA.TxDataLen = sendLen;
            uartDMA.cbDMAFinished = processUartDMA_FinishedTx;
            
            pUartCOM->uartSts.isSending = 1;
            pUartCOM->startSendTime = getSysTick();
            uartStartSend_DMA(&uartDMA);
        }
        #else
        {
            pUartCOM->RxTxBuf.txCnt     = 0;
            pUartCOM->RxTxBuf.txDataLen = sendLen;
            pUartCOM->uartSts.isSending = 1;
            pUartCOM->startSendTime = getSysTick();
        
            /* enable uart tx interrupt to start to send */
            INTClearFlag(INT_SOURCE_UART_TX(phy_id));
            INTEnable(INT_SOURCE_UART_TX(phy_id), INT_ENABLED);
        }
        #endif
    }
    
    return ret;
}

/* =============================================================================
 * ֹͣ���ڷ�������
 * ����
 *      virCom      : ���⴮�ں�
 * ����
 *     ����״̬ tEnmComTxSts
 * */
PUBLIC tEnmComTxSts uartStopSend(tEnmComList virCom)
{
    tEnmComTxSts ret = COM_TX_ERR_UNINIT;
    UART_MODULE id;
    
    tUartComm * pUartCOM;
    if(uartVirID2PhyID(virCom, &id) == TRUE)
    {
        pUartCOM = PhyUartArray[id];

        /* 0 if interrupt is disabled, else is enabled */
        if( INTGetEnable(INT_SOURCE_UART_TX(id)) != 0 )
        {
            /* COM��ָ��Ĵ��ڻ���δ��ɵķ������񣬱�ǿ��ֹͣ */
            INTEnable(INT_SOURCE_UART_TX(id), INT_DISABLED);
        }

        ret = COM_TX_STOPPED;
        pUartCOM->RxTxBuf.txCnt = 0;
        pUartCOM->RxTxBuf.txDataLen = 0;
        pUartCOM->uartSts.isSending = 0;
        pUartCOM->uartSts.TxOVTM    = 0;

        #ifdef UART_TX_BY_DMA
        uartStopSend_DMA(id);
        #endif
        
        /* Ĭ�Ͽ�������ʹ�ܣ�������2��485ʱ���رմ��ڽ��պ�û�д򿪽��� */
        if(pUartCOM->fpRE_En != NULL)
            pUartCOM->fpRE_En();
    }
    
	return ret;	
}


/* =============================================================================
 * ���ô��ڽ��յ�����
 * ����
 *      virCom      : ���⴮�ں�
 * ����
 *     TRUE ��ʾOK
 * */
PUBLIC BOOL uartResetRecv(tEnmComList virCom)
{
    BOOL ret = FALSE;
    UART_MODULE id;
    tUartComm * pUartCOM;
    if(uartVirID2PhyID(virCom, &id) == TRUE)
    {
        pUartCOM = PhyUartArray[id];
    
        if(pUartCOM != NULL)
        {
            pUartCOM->RxTxBuf.rxCnt = 0;
            ret = TRUE;
        }
    }
    
	return ret;	
}

/* =============================================================================
 * ���ͨ�����Ӽ�����״̬
 * ����
 *      virCom      : ���⴮�ں�
 * ����
 *      �������״̬�仯Ϊʧ�ܣ�����TRUE
 * */
PUBLIC BOOL uartChkLinkAndSendSts(tEnmComList virCom)
{
    BOOL linkToFail = FALSE;
    UART_MODULE id;
    tUartComm * pUartCOM;
    
    if(uartVirID2PhyID(virCom, &id) == TRUE)
    {
        pUartCOM = PhyUartArray[id];
        
        if(pUartCOM != NULL)
        {
            /* ���ͨ������ */
            if(pUartCOM->uartSts.linkState == 1)
            {
                if(getSysTimeDiff(pUartCOM->gotFrameTick) >= pUartCOM->linkFailTime)
                {
                    linkToFail = TRUE;
                    pUartCOM->uartSts.linkState = 0;
                    uartStopSend(virCom);
                }
            }
            
            
            if(pUartCOM->uartSts.isSending == 1)
            {
                /* ʹ���˷��ͳ�ʱ��� */
                pUartCOM->uartSts.TxOVTM = 0;
                if(pUartCOM->sendOverTime != 0)
                {
                    if(getSysTimeDiff(pUartCOM->startSendTime) >= pUartCOM->sendOverTime)
                    {
                        /* ���ͳ�ʱ */
                        uartStopSend(virCom);
                        pUartCOM->uartSts.TxOVTM = 1;
                    }
                }
            }
        }
    }
    
	return linkToFail;
}

/* =============================================================================
 * ��ȡ���ڷ�������״̬
 * ����
 *      virCom      : ���⴮�ں�
 * ����
 *     ����״̬ tEnmComTxSts
 * */
PUBLIC tEnmComTxSts uartGetSendState(tEnmComList virCom)
{
    tEnmComTxSts ret = COM_TX_ERR_UNINIT;
    UART_MODULE id;
    tUartComm * pUartCOM;
    
    if(uartVirID2PhyID(virCom, &id) == TRUE)
    {
        pUartCOM = PhyUartArray[id];
        
    	/* 0 if interrupt is disabled, else is enabled */
    	if(( INTGetEnable(INT_SOURCE_UART_TX(id)) != 0 ) && (pUartCOM->RxTxBuf.txDataLen > 0))
    	{
            ret = COM_TX_ERR_BUSY;
    	}
    	else
        {
    	    ret = COM_TX_STOPPED;
        }
    }
	
	return ret;	
}

/* =============================================================================
 * ��������
 * ����
 *      virCom      : ���⴮�ں�
 * ����
 *     ����״̬ tEnmComTxSts (COM_TX_OK ��ʾ���óɹ�)
 * */
PUBLIC tEnmComIniSts DeInitUartCom(tEnmComList virCom, tUartComm * pUartCOM)
{    
    tEnmComTxSts ret;
    
    int i;
    int8u * p;
    UART_MODULE id;
    
    if(uartVirID2PhyID(virCom, &id) == TRUE)
    {
        UARTEnable(id, UART_DISABLE_FLAGS(UART_RX | UART_TX));
    
        PhyUartArray[id] = NULL;
    }
    
    if(pUartCOM != NULL)
    {
        p = (int8u *)pUartCOM;
        for(i=0; i<sizeof(tUartComm); i++)
        {
            *p ++= 0;
        }
    }
  
    ret = COM_INI_OK;
    
    return ret;
}

/* =============================================================================
 * ���ڳ�ʼ������
 * ����
 *   virCom    : ���⴮�ں�
 *   pUartCOM  : ָ�򴮿ڽṹ��
 *   dataRate  : ���ڲ�����
 *   priority  : �ж����ȼ�
 * ����
 *     ��ʼ����� tEnmComIniSts
 */
PUBLIC tEnmComIniSts uartCOM_Init(tEnmComList virCom, UART_MODULE phy_id, tUartComm * pUartCOM,  int32u dataRate, INT_PRIORITY priority)
{
    if(phy_id > UART6)
    {
        return COM_INI_ERR_COMID;
    }
    
    /* �����ж� */
    if((pUartCOM == NULL) || (dataRate ==0) || (dataRate > 1000000))
    {
        return COM_INI_ERR_PARA;
    }
    
    /* �����Ƿ��ѱ�ռ�� */
    if(PhyUartArray[phy_id] != NULL)
    {
        return COM_INI_ERR_OCCUPY;
    }
        
    /* ��ʼ�� */
    pUartCOM->usrComNo = virCom;
    pUartCOM->uartID   = phy_id;
    PhyUartArray[phy_id] = pUartCOM;
    
    #ifndef UART_TX_BY_DMA
    #ifdef  UART_RX_SOFTBUFF
    /* ��ʼ���������ݵ��жϴ�����
     * id = �����ںŶ��� */
    initUartTxCallBack(phy_id, processUartIF_Tx);
    #endif
    #endif
        
    if(pUartCOM->fpRE_En != NULL)
        pUartCOM->fpRE_En();
    
    if(pUartCOM->RS485_4Wire == TRUE)
    {
        if(pUartCOM->fpDE_En != NULL)
            pUartCOM->fpDE_En();
    }
    
    /* �ر�UART ����Ҫ��������־ */
    UARTEnable(phy_id, UART_DISABLE_FLAGS(UART_RX | UART_TX));
    
    /* clear u1art err/rx/tx interrupt flag */
    INTClearFlag(INT_SOURCE_UART_TX(phy_id));
    INTClearFlag(INT_SOURCE_UART_RX(phy_id));
    INTClearFlag(INT_SOURCE_UART_ERROR(phy_id));
    uartReg[phy_id]->sta.reg = 0;

    /* �ж����ȼ� */
    INTSetVectorPriority(INT_VECTOR_UART(phy_id), priority);
    INTSetVectorSubPriority(INT_VECTOR_UART(phy_id), INT_SUB_PRIORITY_LEVEL_0);

    UARTConfigure(phy_id, UART_ENABLE_PINS_TX_RX_ONLY);

	UARTSetFifoMode(phy_id, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);

    UARTSetLineControl(phy_id, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);

    UARTSetDataRate(phy_id, GetPeripheralClock(), dataRate);

   	UARTEnable(phy_id, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
    
    INTEnable(INT_SOURCE_UART_ERROR(phy_id), INT_ENABLED);      /* uart err interrupt enable */
    INTEnable(INT_SOURCE_UART_RX(phy_id), INT_ENABLED);         /* uart rx interrupt enable */
    //INTEnable(INT_SOURCE_UART_TX(phy_id), INT_ENABLED);     /* uart tx interrupt enable */
    
    return COM_INI_OK;
}

#ifndef     UART_RX_SOFTBUFF
/* =============================================================================
 * ���ڵ��շ������жϴ���
 * �� ���е� UART �����շ��ж϶������ﴦ�� ��
 */
int oerrU6_cnt = 0;
PRIVATE void processUartComm(UART_MODULE uart_id)
{
    tSYSTICK    nowTick;
    tUartComm * pComm;
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(uart_id)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(uart_id));
    }
    
    /* �� uart_id ��Ӧ�� COM �� */
    pComm = PhyUartArray[uart_id];
    
    if(pComm == NULL)
    {
        /* this UART has not been initialized */
        INTEnable(INT_SOURCE_UART_RX(uart_id), INT_DISABLED);
        INTEnable(INT_SOURCE_UART_TX(uart_id), INT_DISABLED);
        INTEnable(INT_SOURCE_UART_ERROR(uart_id), INT_DISABLED);
        
	    INTClearFlag(INT_SOURCE_UART_RX(uart_id));
		INTClearFlag(INT_SOURCE_UART_TX(uart_id));
        return;
    }
    
    if(uartReg[uart_id]->sta.reg & OERR_MASK)
    {
        #if 0
        /* ���� FIFO? */
        while (UARTReceivedDataIsAvailable(uart_id))
        {
            rxData = UARTGetDataByte(uart_id);
        }
        #endif
        uartReg[uart_id]->sta.clr = _U1STA_OERR_MASK;       /* �������������� */
        pComm->RxTxBuf.rxCnt = 0;                           /* Ϊ������������Ϣͷ */
        
		/* Clear the RX interrupt Flag */
	    INTClearFlag(INT_SOURCE_UART_RX(uart_id));
    }
    
    /* �������� */
	if(INTGetFlag(INT_SOURCE_UART_RX(uart_id)))
	{
		/* Clear the RX interrupt Flag */
	    INTClearFlag(INT_SOURCE_UART_RX(uart_id));
        
		while (UARTReceivedDataIsAvailable(uart_id))
		{
    		rxData = UARTGetDataByte(uart_id);
            nowTick = getSysTick();
            
            #if 0
            /* ���Խ������� */
            UARTSendDataByte(uart_id, rxData);
            /* �� MMS ͨ�Ŵ��ڻ��Խ������� */
            //UARTSendDataByte(UART2, rxData);
            #endif
            
            /* ���յ����ݳ��ȳ������ջ���������������ս������¼��� */
            if(pComm->RxTxBuf.rxCnt >= pComm->RxTxBuf.rxBufSize)
            {
                pComm->RxTxBuf.rxCnt = 0;
            }
            
            /* ���ý��մ����� */
    		if(pComm->funRx != NULL)
    	    {
                if(pComm->frameOverTime > 0)
                {
                    /* ���ֽ�֮��Ľ���ʱ�����ƣ�����ʱ������Ϊ���µ�һ֡���ݣ����½��ռ��� */
                    if((tSYSTICK)(nowTick - pComm->lastByteTick) >= pComm->frameOverTime)
                    {
                        pComm->RxTxBuf.rxCnt = 0;
                    }
                }

                /* �ص���������������ֽ� */
                pComm->funRx(uart_id, rxData);
            }
            
            pComm->lastByteTick = nowTick;
        }
        
    }
    
    
    /* �������� */
	if ( INTGetFlag(INT_SOURCE_UART_TX(uart_id)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(uart_id));
        
		if(pComm->RxTxBuf.txDataLen > 0)
		{
            pComm->uartSts.isSending = 1;
            if(pComm->RS485_4Wire == TRUE)
            {
                pComm->delayForStopTx = 0;
            }
            else
            {
                pComm->delayForStopTx = 10;
            }
            
			if(INTGetEnable(INT_SOURCE_UART_TX(uart_id)))
			{
				while (UARTTransmitterIsReady(uart_id) && (pComm->RxTxBuf.txDataLen > 0))
				{
                    UARTSendDataByte(uart_id, pComm->RxTxBuf.pTxBuf[pComm->RxTxBuf.txCnt]);
                    pComm->RxTxBuf.txCnt ++;
					pComm->RxTxBuf.txDataLen--;
				}
			}
		}
		else
	    {
            /* if the UART module has finshed transmitting the most recent data.*/
            
            /* Ϊ�˱�֤�����ϵķ������ݳ���һ��ʱ��
             * ���� PIC32 �ķ����жϽ��м�ʱ
             * ��ʱ�����л� 
             * �� 4w-485 ����Ҫ�ӳ�
             */
            if(pComm->delayForStopTx == 0)
            {
                if(UARTTransmissionHasCompleted(uart_id))
                {
                    /* finished sending, disable tx interrupt */
                    INTEnable(INT_SOURCE_UART_TX(uart_id), INT_DISABLED);

                    if(pComm->funTx != NULL)
                    {
                        pComm->funTx();
                    }
                    pComm->uartSts.isSending = 0;

                    /* ���� 2W-485�����ͽ�������Ҫ�л������� */
                    if(pComm->RS485_4Wire == FALSE)
                    {
                        if(pComm->fpDE_Dis != NULL)
                            pComm->fpDE_Dis();

                        if(pComm->fpRE_En != NULL)
                            pComm->fpRE_En();
                    }
                }
            }
            else
            {
                pComm->delayForStopTx --;
            }
        }
    }
}


/* =========================================================
 * ���� UART ͨ���ж���� 
 */

/* UART 1 */
void  __attribute__((section (".SEG_U1ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_1_VECTOR))) Uart1RXHandler(void)
{
    processUartComm(UART1);
}

/* UART 2 */
void  __attribute__((section (".SEG_U2ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_2_VECTOR))) Uart2RXHandler(void)
{
    processUartComm(UART2);
}

/* UART 3 */
void  __attribute__((section (".SEG_U3ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_3_VECTOR))) Uart3RXHandler(void)
{
    processUartComm(UART3);
}

/* UART 4 */
void  __attribute__((section (".SEG_U4ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_4_VECTOR))) Uart4RXHandler(void)
{
    processUartComm(UART4);
}

/* UART 6 */
void  __attribute__((section (".SEG_U6ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_6_VECTOR))) Uart6RXHandler(void)
{
    processUartComm(UART6);
}

#endif  /* UART_RX_SOFTBUFF */


/* =============================================================================
 * ��鴮�ڴ���������󲢳�ʼ��
 */
PUBLIC int checkAllUartErrorSta(void)
{
    int err;
    int errorFlags = 0;
    
    /* Uart 1 
     */
    if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART1)) )
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART1));
    }

    err = U1STA & UART_X_ERR_MASK;  //UART1GetErrors()
    if(err)
    {
        errorFlags |= 1<<0;
        U1STACLR = UART_X_ERR_MASK; //UART1ClearAllErrors();
    }

    /* Uart 2 
     */
    if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART2)) )
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART2));
    }

    err = U2STA & UART_X_ERR_MASK;  //UART2GetErrors()
    if(err)
    {
        errorFlags |= 1<<1;
        U2STACLR = UART_X_ERR_MASK; //UART2ClearAllErrors();
    }

    /* Uart 3 
     */
    if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART3)) )
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART3));
    }

    err = U3STA & UART_X_ERR_MASK;
    if(err)
    {
        errorFlags |= 1<<2;
        U3STACLR = UART_X_ERR_MASK;
    }

    /* Uart 4 
     */
    if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART4)) )
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART4));
    }

    err = U4STA & UART_X_ERR_MASK;
    if(err)
    {
        errorFlags |= 1<<3;
        U4STACLR = UART_X_ERR_MASK;
    }

    /* Uart 6
     */
    if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART6)) )
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART6));
    }

    err = U6STA & UART_X_ERR_MASK;
    if(err)
    {
        errorFlags |= 1<<5;
        U6STACLR = UART_X_ERR_MASK;
    }

    if(errorFlags != 0)
    {
        errorFlags = 0;
    }
    
}



#ifdef  UART_RX_SOFTBUFF
#ifndef UART_TX_BY_DMA

/* =============================================================================
 * ���ڷ����жϴ��������ɴ����жϵ���
 * (UART_MODULE)
 */
PRIVATE void processUartIF_Tx(int32u uart_id)
{
    tUartComm * pComm;
    
    /* Clear the TX interrupt Flag */
    //INTClearFlag(INT_SOURCE_UART_TX(uart_id));
    
    /* �� uart_id ��Ӧ�� COM �� */
    pComm = PhyUartArray[uart_id];
    if(pComm == NULL)
    {
        return;
    }

    if(pComm->RxTxBuf.txDataLen > 0)
    {
        pComm->uartSts.isSending = 1;
        if(pComm->RS485_4Wire == TRUE)
        {
            pComm->delayForStopTx = 0;
        }
        else
        {
            pComm->delayForStopTx = 10;
        }

        if(INTGetEnable(INT_SOURCE_UART_TX(uart_id)))
        {
            while (UARTTransmitterIsReady(uart_id) && (pComm->RxTxBuf.txDataLen > 0))
            {
                UARTSendDataByte(uart_id, pComm->RxTxBuf.pTxBuf[pComm->RxTxBuf.txCnt]);
                pComm->RxTxBuf.txCnt ++;
                pComm->RxTxBuf.txDataLen--;
            }
        }
    }
    else
    {
        /* if the UART module has finshed transmitting the most recent data.*/

        /* Ϊ�˱�֤�����ϵķ������ݳ���һ��ʱ��
         * ���� PIC32 �ķ����жϽ��м�ʱ
         * ��ʱ�����л� 
         * �� 4w-485 ����Ҫ�ӳ�
         */
        if(pComm->delayForStopTx == 0)
        {
            if(UARTTransmissionHasCompleted(uart_id))
            {
                /* finished sending, disable tx interrupt */
                INTEnable(INT_SOURCE_UART_TX(uart_id), INT_DISABLED);

                if(pComm->funTx != NULL)
                {
                    pComm->funTx();
                }
                pComm->uartSts.isSending = 0;

                /* ���� 2W-485�����ͽ�������Ҫ�л������� */
                if(pComm->RS485_4Wire == FALSE)
                {
                    if(pComm->fpDE_Dis != NULL)
                        pComm->fpDE_Dis();

                    if(pComm->fpRE_En != NULL)
                        pComm->fpRE_En();
                }
            }
        }
        else
        {
            pComm->delayForStopTx --;
        }
    }
}
#endif /* UART_TX_BY_DMA */


/* =============================================================================
 * ���ڷ���ͨ��DMAʵ�֣�
 * DMA��������Ժ��ڴ浽UART TX FIFO������Ҫ����FIFOȫ�����ͽ����ٴ���(2��-4���л�)
 * 
 * uart_id (UART_MODULE)
 */
PRIVATE void processUartDMA_FinishedTx(int32u uart_id)
{
    tUartComm * pComm = NULL;
    
    if(uart_id >= PHY_UART_NUM)
        return;
    
    /* �� uart_id ��Ӧ�� COM �� */
    pComm = PhyUartArray[uart_id];
    if(pComm == NULL)
    {
        return;
    }
    
    if(UARTTransmissionHasCompleted(uart_id))
    {
        /* finished sending, disable tx interrupt */
        INTEnable(INT_SOURCE_UART_TX(uart_id), INT_DISABLED);

        if(pComm->funTx != NULL)
        {
            pComm->funTx();
        }
        pComm->uartSts.isSending = 0;

        /* ���� 2W-485�����ͽ�������Ҫ�л������� */
        if(pComm->RS485_4Wire == FALSE)
        {
            if(pComm->fpDE_Dis != NULL)
                pComm->fpDE_Dis();

            if(pComm->fpRE_En != NULL)
                pComm->fpRE_En();
        }
    }
}

/* =============================================================================
 * ���ڽ����жϽ����ݱ��浽���ջ�������
 * ���������м�鴮�ڽ��ջ����������ݣ�����������֡���������ж��н�����������ռ��CPUʱ��
 * 
 * !!!!! ��������ѭ���е��� !!!!!
 */
PUBLIC void processAllUartRxDataBuff(void)
{
    tSYSTICK    nowTick;
    UART_MODULE phyID;
    int8u rxData;
    int i;
    int dealLen;
    tUartComm * pComm = NULL;
    
    for(phyID=0; phyID<PHY_UART_MAX_NUM; phyID++)
    {
        /* ������ ����id ��Ӧ�� ����COM �� */
        pComm = PhyUartArray[phyID];
        
        if(pComm == NULL)
            continue;
        
        /* ���������Ƿ��н��յ����ݣ��ӽ��ջ�������ȡ
         * ����������������� 5 �����ݣ����⴦��������ռ��CPU���� */
        dealLen = 5;
        while(dealLen--)
        {
            if(readUartRxByte(phyID ,&rxData ) == FALSE)
                break;
            
            /* �����յ��ֽڣ�ʱ�� */
            nowTick = getLastRxByteTick(phyID);//getSysTick();

            /* ���յ����ݳ��ȳ������ջ���������������ս������¼��� */
            if(pComm->RxTxBuf.rxCnt >= pComm->RxTxBuf.rxBufSize)
            {
                pComm->RxTxBuf.rxCnt = 0;
            }

            /* ���ý��մ����� */
            if(pComm->funRx != NULL)
            {
                if(pComm->frameOverTime > 0)
                {
                    /* ���ֽ�֮��Ľ���ʱ�����ƣ�����ʱ������Ϊ���µ�һ֡���ݣ����½��ռ��� */
                    if((tSYSTICK)(nowTick - pComm->lastByteTick) >= pComm->frameOverTime)
                    {
                        pComm->RxTxBuf.rxCnt = 0;
                    }
                }

                /* �ص���������������ֽ� */
                pComm->funRx(phyID, rxData);
            }

            pComm->lastByteTick = nowTick;
        }
    }
}


#endif

