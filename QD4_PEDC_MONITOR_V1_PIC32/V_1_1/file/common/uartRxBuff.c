
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
 * 初始化所有的接收缓存
 */
void initAllUartRxSetting(void)
{
    int id;
    tUartObj *pUartObj;
    
    for(id=0; id<PHY_UART_MAX_NUM; id++)
    {
        /* 物理串口号对象 */
        pUartObj = UartObjList[id];
        
        pUartObj->rxWritePos    = 0;    /* 写接收数据的位置 */
        pUartObj->rxReadPos     = 0;    /* 读接收数据的位置 */
        pUartObj->rxCounter     = 0;    /* 已接收数据的长度，0 表示缓冲区没有可用数据 */
        pUartObj->lastTick_1    = 0;
        pUartObj->lastTick_2    = 0;

        pUartObj->uartErrors    = 0;    /* 错误状态 */
        pUartObj->OErrCnt       = 0;    /* OERR 计数器，调试用 */
    }
}


#ifndef UART_TX_BY_DMA
/* 初始化发送中断处理回调函数 */
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
 * 从接收缓存中读取1字节数据
 * 参数
 *      phyCom : 物理串口号
 *      data   : 保存读取的数据
 * 返回
 *      TRUE   : 读取成功
 *      FALSE  : 读取失败或没有数据
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
    
    /* 读数据 */
    *data = pUartObj->pRxBuf[pUartObj->rxReadPos];
    
    /* 指向下一个读取位置，注意防止越界 */
    pUartObj->rxReadPos ++;
    pUartObj->rxReadPos %= UART_RXBUF_SIZE;//pUartObj->rxBufSize;
    
    /* 存储缓存区的计数器 -1
     * 关闭中断，避免意外改变 */
    asm volatile("di");
    pUartObj->rxCounter--;
    asm volatile("ei");
    
    return TRUE;
}

/* =============================================================================
 * 从接收缓存中读取多字节数据
 * 参数
 *      phyCom : 物理串口号
 *      data   : 保存读取的数据
 *      readLen: 需要读的字节数
 * 返回
 *      读取数据的长度，0 表示没有
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
        
        /* 读数据 */
        *dataBuf = pUartObj->pRxBuf[pUartObj->rxReadPos];

        /* 指向下一个读取位置，注意防止越界 */
        pUartObj->rxReadPos ++;
        pUartObj->rxReadPos %= UART_RXBUF_SIZE;//pUartObj->rxBufSize;
        
        readCnt ++;
    }
    
    /* 修改存储缓存区的计数器
     * 关闭中断，避免发送意外 */
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
 * 所有 UART 通信中断入口 
 */

/* UART 1 */
void __ISR(_UART_1_VECTOR)  Uart1RXHandler(void)
//void  __attribute__((section (".SEG_U1ARTRXHANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_UART_1_VECTOR))) Uart1RXHandler(void)
{
    int8u rxData;
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART1)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART1));
        UART1Obj.uartErrors = U1STA & UART_X_ERR_MASK;
    }
    else
    {
        UART1Obj.uartErrors = 0;
    }
    
    /* 接收中断 */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART1)))
	{
        /* 循环读取 FIFO */
		//while (UARTReceivedDataIsAvailable(UART1))
		while (U1STA & _U1STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART1))
		{
    		rxData = (int8u)U1RXREG;//UARTGetDataByte(UART1);
            #if 0
            /* 回显接收数据 */
            U1TXREG = rxData;//UARTSendDataByte(UART1, rxData);
            #endif
            
            if(UART1Obj.rxCounter >= UART_RXBUF_SIZE)//UART1Obj.rxBufSize)
            {
                /* 接收缓冲区已满 */
                UART1Obj.rxBufFull  = TRUE;
                UART1Obj.rxCounter  = 0;
                UART1Obj.rxWritePos = 0;
                UART1Obj.rxReadPos  = 0;
            }
            else
            {
                /* 将数据存入缓冲区，存储计数器 +1 */
                UART1Obj.pRxBuf[UART1Obj.rxWritePos] = rxData;
                UART1Obj.rxCounter ++;
                
                /* 移动写缓存的位置并防止越界 */
                UART1Obj.rxWritePos++;
                UART1Obj.rxWritePos %= UART_RXBUF_SIZE;//UART1Obj.rxBufSize;
            }
            
            UART1Obj.lastTick_2 = UART1Obj.lastTick_1;
            UART1Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART1));
    
    /* 先处理了接收中断，溢出了也有数据 */
    if(U1STA & OERR_MASK)
    {
        U1STACLR = _U1STA_OERR_MASK;                /* 清除接收溢出错误 */
        UART1Obj.OErrCnt++;                         /* 错误计数器 */
    }
    
    #ifndef UART_TX_BY_DMA
    /* 发送中断，直接处理，不经过缓冲区 */
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
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART2)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART2));
        UART2Obj.uartErrors = U2STA & UART_X_ERR_MASK;
    }
    else
    {
        UART2Obj.uartErrors = 0;
    }
    
    /* 接收中断 */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART2)))
	{
        /* 循环读取 FIFO */
		//while (UARTReceivedDataIsAvailable(UART2))
		while (U2STA & _U2STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART2))
		{
    		rxData = (int8u)U2RXREG;//UARTGetDataByte(UART2);
            #if 0
            /* 回显接收数据 */
            U2TXREG = rxData;//UARTSendDataByte(UART2, rxData);
            #endif
            
            if(UART2Obj.rxCounter >= UART_RXBUF_SIZE)//UART2Obj.rxBufSize)
            {
                /* 接收缓冲区已满 */
                UART2Obj.rxBufFull  = TRUE;
                UART2Obj.rxCounter  = 0;
                UART2Obj.rxWritePos = 0;
                UART2Obj.rxReadPos  = 0;
            }
            else
            {
                /* 将数据存入缓冲区，存储计数器 +1 */
                UART2Obj.pRxBuf[UART2Obj.rxWritePos] = rxData;
                UART2Obj.rxCounter ++;
                
                /* 移动写缓存的位置并防止越界 */
                UART2Obj.rxWritePos++;
                UART2Obj.rxWritePos %= UART_RXBUF_SIZE;//UART2Obj.rxBufSize;
            }
            
            UART2Obj.lastTick_2 = UART2Obj.lastTick_1;
            UART2Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART2));
    
    /* 先处理了接收中断，溢出了也有数据 */
    if(U2STA & OERR_MASK)
    {
        U2STACLR = _U2STA_OERR_MASK;                /* 清除接收溢出错误 */
        UART2Obj.OErrCnt++;                         /* 错误计数器 */
    }
    
    #ifndef UART_TX_BY_DMA
    /* 发送中断，直接处理，不经过缓冲区 */
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
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART3)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART3));
        UART3Obj.uartErrors = U3STA & UART_X_ERR_MASK;
    }
    else
    {
        UART3Obj.uartErrors = 0;
    }
    
    /* 接收中断 */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART3)))
	{
        /* 循环读取 FIFO */
		//while (UARTReceivedDataIsAvailable(UART3))
		while (U3STA & _U3STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART3))
		{
    		rxData = (int8u)U3RXREG;//UARTGetDataByte(UART3);
            #if 0
            /* 回显接收数据 */
            U3TXREG = rxData;//UARTSendDataByte(UART3, rxData);
            #endif
            
            if(UART3Obj.rxCounter >= UART_RXBUF_SIZE)//UART3Obj.rxBufSize)
            {
                /* 接收缓冲区已满 */
                UART3Obj.rxBufFull  = TRUE;
                UART3Obj.rxCounter  = 0;
                UART3Obj.rxWritePos = 0;
                UART3Obj.rxReadPos  = 0;
            }
            else
            {
                /* 将数据存入缓冲区，存储计数器 +1 */
                UART3Obj.pRxBuf[UART3Obj.rxWritePos] = rxData;
                UART3Obj.rxCounter ++;
                
                /* 移动写缓存的位置并防止越界 */
                UART3Obj.rxWritePos++;
                UART3Obj.rxWritePos %= UART_RXBUF_SIZE;//UART3Obj.rxBufSize;
            }
            
            UART3Obj.lastTick_2 = UART3Obj.lastTick_1;
            UART3Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART3));
    
    /* 先处理了接收中断，溢出了也有数据 */
    if(U3STA & OERR_MASK)
    {
        U3STACLR = _U3STA_OERR_MASK;                /* 清除接收溢出错误 */
        UART3Obj.OErrCnt++;                         /* 错误计数器 */
    }
    
    #ifndef UART_TX_BY_DMA
    /* 发送中断，直接处理，不经过缓冲区 */
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
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART4)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART4));
        UART4Obj.uartErrors = U4STA & UART_X_ERR_MASK;
    }
    else
    {
        UART4Obj.uartErrors = 0;
    }
    
    /* 接收中断 */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART4)))
	{
        /* 循环读取 FIFO */
		//while (UARTReceivedDataIsAvailable(UART4))
		while (U4STA & _U4STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART4))
		{
    		rxData = (int8u)U4RXREG;//UARTGetDataByte(UART4);
            #if 0
            /* 回显接收数据 */
            U4TXREG = rxData;//UARTSendDataByte(UART4, rxData);
            #endif
            
            if(UART4Obj.rxCounter >= UART_RXBUF_SIZE)//UART4Obj.rxBufSize)
            {
                /* 接收缓冲区已满 */
                UART4Obj.rxBufFull  = TRUE;
                UART4Obj.rxCounter  = 0;
                UART4Obj.rxWritePos = 0;
                UART4Obj.rxReadPos  = 0;
            }
            else
            {
                /* 将数据存入缓冲区，存储计数器 +1 */
                UART4Obj.pRxBuf[UART4Obj.rxWritePos] = rxData;
                UART4Obj.rxCounter ++;
                
                /* 移动写缓存的位置并防止越界 */
                UART4Obj.rxWritePos++;
                UART4Obj.rxWritePos %= UART_RXBUF_SIZE;//UART4Obj.rxBufSize;
            }
            
            UART4Obj.lastTick_2 = UART4Obj.lastTick_1;
            UART4Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART4));
    
    /* 先处理了接收中断，溢出了也有数据 */
    if(U4STA & OERR_MASK)
    {
        U4STACLR = _U4STA_OERR_MASK;                /* 清除接收溢出错误 */
        UART4Obj.OErrCnt++;                         /* 错误计数器 */
    }
    
    #ifndef UART_TX_BY_DMA
    /* 发送中断，直接处理，不经过缓冲区 */
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
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(UART6)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(UART6));
        UART6Obj.uartErrors = U6STA & UART_X_ERR_MASK;
    }
    else
    {
        UART6Obj.uartErrors = 0;
    }
    
    /* 接收中断 */
	if(INTGetFlag(INT_SOURCE_UART_RX(UART6)))
	{
        /* 循环读取 FIFO */
		//while (UARTReceivedDataIsAvailable(UART6))
		while (U6STA & _U6STA_URXDA_MASK) //UARTReceivedDataIsAvailable(UART6))
		{
    		rxData = (int8u)U6RXREG;//UARTGetDataByte(UART6);
            #if 0
            /* 回显接收数据 */
            U6TXREG = rxData;//UARTSendDataByte(UART6, rxData);
            #endif
            
            if(UART6Obj.rxCounter >= UART_RXBUF_SIZE)//UART6Obj.rxBufSize)
            {
                /* 接收缓冲区已满 */
                UART6Obj.rxBufFull  = TRUE;
                UART6Obj.rxCounter  = 0;
                UART6Obj.rxWritePos = 0;
                UART6Obj.rxReadPos  = 0;
            }
            else
            {
                /* 将数据存入缓冲区，存储计数器 +1 */
                UART6Obj.pRxBuf[UART6Obj.rxWritePos] = rxData;
                UART6Obj.rxCounter ++;
                
                /* 移动写缓存的位置并防止越界 */
                UART6Obj.rxWritePos++;
                UART6Obj.rxWritePos %= UART_RXBUF_SIZE;//UART6Obj.rxBufSize;
            }
            
            UART6Obj.lastTick_2 = UART6Obj.lastTick_1;
            UART6Obj.lastTick_1 = getSysTick();
        }
    }
    /* Clear the RX interrupt Flag */
    INTClearFlag(INT_SOURCE_UART_RX(UART6));
    
    /* 先处理了接收中断，溢出了也有数据 */
    if(U6STA & OERR_MASK)
    {
        U6STACLR = _U6STA_OERR_MASK;                /* 清除接收溢出错误 */
        UART6Obj.OErrCnt++;                         /* 错误计数器 */
    }
    
    #ifndef UART_TX_BY_DMA
    /* 发送中断，直接处理，不经过缓冲区
     * 验证： 当使用 DMA 发送时，屏蔽下面代码PEDC与 MMS 通信也是正常
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

