
#include "_cpu.h"
#include "uartDrv.h"
#include "sysTimer.h"


#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

#ifdef UART_RX_SOFTBUFF

PRIVATE tUartObj UART1Obj, UART2Obj,UART3Obj,UART4Obj,UART5Obj,UART6Obj;
tUartObj * UartObjList[PHY_UART_MAX_NUM] = { &UART1Obj, &UART2Obj, &UART3Obj, &UART4Obj, &UART5Obj, &UART6Obj};

PUBLIC tUartObj * getUartObjPtr(int8u phy_id)
{
    return UartObjList[phy_id];
}

/* =============================================================================
 * ��ʼ�����еĽ��ջ���
 */
void initAllUartRxSetting(void)
{
    int id;
    tUartObj *pUartObj;
    
    for(id=0; id<PHY_UART_MAX_NUM; id++)
    {
        /* �����ںŶ��� */
        pUartObj = UartObjList[id];
        
        pUartObj->rxWritePos    = 0;    /* д�������ݵ�λ�� */
        pUartObj->rxReadPos     = 0;    /* ���������ݵ�λ�� */
        pUartObj->rxCounter     = 0;    /* �ѽ������ݵĳ��ȣ�0 ��ʾ������û�п������� */
        pUartObj->lastTick_1    = 0;
        pUartObj->lastTick_2    = 0;

        pUartObj->uartErrors    = 0;    /* ����״̬ */
        pUartObj->OErrCnt       = 0;    /* OERR �������������� */
    }
}


#ifndef UART_TX_BY_DMA
/* ��ʼ�������жϴ���ص����� */
BOOL initUartTxCallBack(int32u phyID, cbUartTxCallBack cbTxIRQ)
{
    if(phyID >= PHY_UART_MAX_NUM)
    {
        return FALSE;
    }
    
    UartObjList[phyID]->txCallBack = cbTxIRQ;
    return TRUE;
}
#endif

/* =============================================================================
 * �ӽ��ջ����ж�ȡ1�ֽ�����
 * ����
 *      phyCom : �����ں�
 *      data   : �����ȡ������
 * ����
 *      TRUE   : ��ȡ�ɹ�
 *      FALSE  : ��ȡʧ�ܻ�û������
 */
BOOL readUartRxByte(int8u phyCom, int8u *data)
{
    tUartObj *pUartObj;
    if(phyCom > 5)
        return FALSE;
    
    pUartObj = UartObjList[phyCom];
    if(pUartObj == NULL)
        return FALSE;
    
    if(pUartObj->rxCounter == 0)
        return FALSE;
    
    /* ������ */
    *data = pUartObj->pRxBuf[pUartObj->rxReadPos];
    
    /* ָ����һ����ȡλ�ã�ע���ֹԽ�� */
    pUartObj->rxReadPos ++;
    pUartObj->rxReadPos %= UART_RXBUF_SIZE;//pUartObj->rxBufSize;
    
    /* �洢�������ļ����� -1
     * �ر��жϣ���������ı� */
    asm volatile("di");
    pUartObj->rxCounter--;
    asm volatile("ei");
    
    return TRUE;
}

/* =============================================================================
 * �ӽ��ջ����ж�ȡ���ֽ�����
 * ����
 *      phyCom : �����ں�
 *      data   : �����ȡ������
 *      readLen: ��Ҫ�����ֽ���
 * ����
 *      ��ȡ���ݵĳ��ȣ�0 ��ʾû��
 */
int readUartRxBytes(int8u phyCom, int8u *dataBuf, int readLen)
{    
    int32u readCnt;
    tUartObj *pUartObj;
    if(phyCom > 5)
        return FALSE;
    
    pUartObj = UartObjList[phyCom];
    if(pUartObj == NULL)
        return 0;
    
    while(readLen--)
    {
        if(pUartObj->rxCounter == 0)
            break;
        
        /* ������ */
        *dataBuf = pUartObj->pRxBuf[pUartObj->rxReadPos];

        /* ָ����һ����ȡλ�ã�ע���ֹԽ�� */
        pUartObj->rxReadPos ++;
        pUartObj->rxReadPos %= UART_RXBUF_SIZE;//pUartObj->rxBufSize;
        
        readCnt ++;
    }
    
    /* �޸Ĵ洢�������ļ�����
     * �ر��жϣ����ⷢ������ */
    asm volatile("di");
    if(pUartObj->rxCounter > readCnt)
        pUartObj->rxCounter -= readCnt;
    else
        pUartObj->rxCounter  = 0;
    asm volatile("ei");
        
    return readCnt;
}

int32u getLastRxByteTick(int8u phyCom)
{
    tUartObj *pUartObj;
    if(phyCom > 5)
        return FALSE;
    
    pUartObj = UartObjList[phyCom];
    if(pUartObj == NULL)
        return 0;
    
    return pUartObj->lastTick_1;
}




/* =========================================================
 * ���� UART ͨ���ж���� 
 */

/* UART 1 */
void __ISR(_UART_1_VECTOR)  Uart1RXHandler(void)
//void  __attribute__((section (".SEG_U1ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_1_VECTOR))) Uart1RXHandler(void)
{
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART1)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART1));
        UART1Obj.uartErrors = U1STA & UART_X_ERR_MASK;
    }
    else
    {
        UART1Obj.uartErrors = 0;
    }
    
    /* �����ж� */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART1)))
	{
        /* ѭ����ȡ FIFO */
		//while (UARTReceivedDataIsAvailable(UART1))
		while (U1STA & _U1STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART1))
		{
    		rxData = (int8u)U1RXREG;//UARTGetDataByte(UART1);
            #if 0
            /* ���Խ������� */
            U1TXREG = rxData;//UARTSendDataByte(UART1, rxData);
            #endif
            
            if(UART1Obj.rxCounter >= UART_RXBUF_SIZE)//UART1Obj.rxBufSize)
            {
                /* ���ջ��������� */
                UART1Obj.rxBufFull  = TRUE;
                UART1Obj.rxCounter  = 0;
                UART1Obj.rxWritePos = 0;
                UART1Obj.rxReadPos  = 0;
            }
            else
            {
                /* �����ݴ��뻺�������洢������ +1 */
                UART1Obj.pRxBuf[UART1Obj.rxWritePos] = rxData;
                UART1Obj.rxCounter ++;
                
                /* �ƶ�д�����λ�ò���ֹԽ�� */
                UART1Obj.rxWritePos++;
                UART1Obj.rxWritePos %= UART_RXBUF_SIZE;//UART1Obj.rxBufSize;
            }
            
            UART1Obj.lastTick_2 = UART1Obj.lastTick_1;
            UART1Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART1));
    
    /* �ȴ����˽����жϣ������Ҳ������ */
    if(U1STA & OERR_MASK)
    {
        U1STACLR = _U1STA_OERR_MASK;                /* �������������� */
        UART1Obj.OErrCnt++;                         /* ��������� */
    }
    
    #ifndef UART_TX_BY_DMA
    /* �����жϣ�ֱ�Ӵ��������������� */
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART1)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(UART1));
        //IFS1CLR = _IFS1_U1TXIF_MASK;
        
        if( UART1Obj.txCallBack != NULL )
        {
            UART1Obj.txCallBack(UART1);
        }
    }
    #endif
}

/* UART 2 */
void __ISR(_UART_2_VECTOR)  Uart2RXHandler(void)
//void  __attribute__((section (".SEG_U2ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_2_VECTOR))) Uart2RXHandler(void)
{
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART2)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART2));
        UART2Obj.uartErrors = U2STA & UART_X_ERR_MASK;
    }
    else
    {
        UART2Obj.uartErrors = 0;
    }
    
    /* �����ж� */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART2)))
	{
        /* ѭ����ȡ FIFO */
		//while (UARTReceivedDataIsAvailable(UART2))
		while (U2STA & _U2STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART2))
		{
    		rxData = (int8u)U2RXREG;//UARTGetDataByte(UART2);
            #if 0
            /* ���Խ������� */
            U2TXREG = rxData;//UARTSendDataByte(UART2, rxData);
            #endif
            
            if(UART2Obj.rxCounter >= UART_RXBUF_SIZE)//UART2Obj.rxBufSize)
            {
                /* ���ջ��������� */
                UART2Obj.rxBufFull  = TRUE;
                UART2Obj.rxCounter  = 0;
                UART2Obj.rxWritePos = 0;
                UART2Obj.rxReadPos  = 0;
            }
            else
            {
                /* �����ݴ��뻺�������洢������ +1 */
                UART2Obj.pRxBuf[UART2Obj.rxWritePos] = rxData;
                UART2Obj.rxCounter ++;
                
                /* �ƶ�д�����λ�ò���ֹԽ�� */
                UART2Obj.rxWritePos++;
                UART2Obj.rxWritePos %= UART_RXBUF_SIZE;//UART2Obj.rxBufSize;
            }
            
            UART2Obj.lastTick_2 = UART2Obj.lastTick_1;
            UART2Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART2));
    
    /* �ȴ����˽����жϣ������Ҳ������ */
    if(U2STA & OERR_MASK)
    {
        U2STACLR = _U2STA_OERR_MASK;                /* �������������� */
        UART2Obj.OErrCnt++;                         /* ��������� */
    }
    
    #ifndef UART_TX_BY_DMA
    /* �����жϣ�ֱ�Ӵ��������������� */
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART2)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(UART2));
        
        if( UART2Obj.txCallBack != NULL )
        {
            UART2Obj.txCallBack(UART2);
        }
    }
    #endif
}

/* UART 3 */
void __ISR(_UART_3_VECTOR)  Uart3RXHandler(void)
//void  __attribute__((section (".SEG_U3ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_3_VECTOR))) Uart3RXHandler(void)
{
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART3)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART3));
        UART3Obj.uartErrors = U3STA & UART_X_ERR_MASK;
    }
    else
    {
        UART3Obj.uartErrors = 0;
    }
    
    /* �����ж� */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART3)))
	{
        /* ѭ����ȡ FIFO */
		//while (UARTReceivedDataIsAvailable(UART3))
		while (U3STA & _U3STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART3))
		{
    		rxData = (int8u)U3RXREG;//UARTGetDataByte(UART3);
            #if 0
            /* ���Խ������� */
            U3TXREG = rxData;//UARTSendDataByte(UART3, rxData);
            #endif
            
            if(UART3Obj.rxCounter >= UART_RXBUF_SIZE)//UART3Obj.rxBufSize)
            {
                /* ���ջ��������� */
                UART3Obj.rxBufFull  = TRUE;
                UART3Obj.rxCounter  = 0;
                UART3Obj.rxWritePos = 0;
                UART3Obj.rxReadPos  = 0;
            }
            else
            {
                /* �����ݴ��뻺�������洢������ +1 */
                UART3Obj.pRxBuf[UART3Obj.rxWritePos] = rxData;
                UART3Obj.rxCounter ++;
                
                /* �ƶ�д�����λ�ò���ֹԽ�� */
                UART3Obj.rxWritePos++;
                UART3Obj.rxWritePos %= UART_RXBUF_SIZE;//UART3Obj.rxBufSize;
            }
            
            UART3Obj.lastTick_2 = UART3Obj.lastTick_1;
            UART3Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART3));
    
    /* �ȴ����˽����жϣ������Ҳ������ */
    if(U3STA & OERR_MASK)
    {
        U3STACLR = _U3STA_OERR_MASK;                /* �������������� */
        UART3Obj.OErrCnt++;                         /* ��������� */
    }
    
    #ifndef UART_TX_BY_DMA
    /* �����жϣ�ֱ�Ӵ��������������� */
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART3)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(UART3));
        
        if( UART3Obj.txCallBack != NULL )
        {
            UART3Obj.txCallBack(UART3);
        }
    }
    #endif
}


/* UART 4 */
void __ISR(_UART_4_VECTOR)  Uart4RXHandler(void)
//void  __attribute__((section (".SEG_U4ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_4_VECTOR))) Uart4RXHandler(void)
{
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART4)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART4));
        UART4Obj.uartErrors = U4STA & UART_X_ERR_MASK;
    }
    else
    {
        UART4Obj.uartErrors = 0;
    }
    
    /* �����ж� */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART4)))
	{
        /* ѭ����ȡ FIFO */
		//while (UARTReceivedDataIsAvailable(UART4))
		while (U4STA & _U4STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART4))
		{
    		rxData = (int8u)U4RXREG;//UARTGetDataByte(UART4);
            #if 0
            /* ���Խ������� */
            U4TXREG = rxData;//UARTSendDataByte(UART4, rxData);
            #endif
            
            if(UART4Obj.rxCounter >= UART_RXBUF_SIZE)//UART4Obj.rxBufSize)
            {
                /* ���ջ��������� */
                UART4Obj.rxBufFull  = TRUE;
                UART4Obj.rxCounter  = 0;
                UART4Obj.rxWritePos = 0;
                UART4Obj.rxReadPos  = 0;
            }
            else
            {
                /* �����ݴ��뻺�������洢������ +1 */
                UART4Obj.pRxBuf[UART4Obj.rxWritePos] = rxData;
                UART4Obj.rxCounter ++;
                
                /* �ƶ�д�����λ�ò���ֹԽ�� */
                UART4Obj.rxWritePos++;
                UART4Obj.rxWritePos %= UART_RXBUF_SIZE;//UART4Obj.rxBufSize;
            }
            
            UART4Obj.lastTick_2 = UART4Obj.lastTick_1;
            UART4Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART4));
    
    /* �ȴ����˽����жϣ������Ҳ������ */
    if(U4STA & OERR_MASK)
    {
        U4STACLR = _U4STA_OERR_MASK;                /* �������������� */
        UART4Obj.OErrCnt++;                         /* ��������� */
    }
    
    #ifndef UART_TX_BY_DMA
    /* �����жϣ�ֱ�Ӵ��������������� */
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART4)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(UART4));
        
        if( UART4Obj.txCallBack != NULL )
        {
            UART4Obj.txCallBack(UART4);
        }
    }
    #endif
}

/* UART 6 */
void __ISR(_UART_6_VECTOR)  Uart6RXHandler(void)
//void  __attribute__((section (".SEG_U6ARTRXHANDLER"))) __attribute__ ((interrupt(ipl6))) __attribute__ ((vector(_UART_6_VECTOR))) Uart6RXHandler(void)
{
    int8u rxData;
    
    /* �����ж�? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART6)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART6));
        UART6Obj.uartErrors = U6STA & UART_X_ERR_MASK;
    }
    else
    {
        UART6Obj.uartErrors = 0;
    }
    
    /* �����ж� */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART6)))
	{
        /* ѭ����ȡ FIFO */
		//while (UARTReceivedDataIsAvailable(UART6))
		while (U6STA & _U6STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART6))
		{
    		rxData = (int8u)U6RXREG;//UARTGetDataByte(UART6);
            #if 0
            /* ���Խ������� */
            U6TXREG = rxData;//UARTSendDataByte(UART6, rxData);
            #endif
            
            if(UART6Obj.rxCounter >= UART_RXBUF_SIZE)//UART6Obj.rxBufSize)
            {
                /* ���ջ��������� */
                UART6Obj.rxBufFull  = TRUE;
                UART6Obj.rxCounter  = 0;
                UART6Obj.rxWritePos = 0;
                UART6Obj.rxReadPos  = 0;
            }
            else
            {
                /* �����ݴ��뻺�������洢������ +1 */
                UART6Obj.pRxBuf[UART6Obj.rxWritePos] = rxData;
                UART6Obj.rxCounter ++;
                
                /* �ƶ�д�����λ�ò���ֹԽ�� */
                UART6Obj.rxWritePos++;
                UART6Obj.rxWritePos %= UART_RXBUF_SIZE;//UART6Obj.rxBufSize;
            }
            
            UART6Obj.lastTick_2 = UART6Obj.lastTick_1;
            UART6Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART6));
    
    /* �ȴ����˽����жϣ������Ҳ������ */
    if(U6STA & OERR_MASK)
    {
        U6STACLR = _U6STA_OERR_MASK;                /* �������������� */
        UART6Obj.OErrCnt++;                         /* ��������� */
    }
    
    #ifndef UART_TX_BY_DMA
    /* �����жϣ�ֱ�Ӵ���������������
     * ��֤�� ��ʹ�� DMA ����ʱ�������������PEDC�� MMS ͨ��Ҳ������
     */
	if ( INTGetFlag(INT_SOURCE_UART_TX(UART6)) )
	{
		/* Clear the TX interrupt Flag */
		INTClearFlag(INT_SOURCE_UART_TX(UART6));
        
        if( UART6Obj.txCallBack != NULL )
        {
            UART6Obj.txCallBack(UART6);
        }
    }
    #endif
}

#endif  /* UART_RX_SOFTBUFF */

