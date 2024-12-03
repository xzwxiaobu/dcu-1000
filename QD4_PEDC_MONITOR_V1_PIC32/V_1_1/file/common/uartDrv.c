
#include "_cpu.h"
#include "uartDrv.h"
#include "sysTimer.h"

#define PERR_MASK           (1<<3)
#define FERR_MASK           (1<<2)
#define OERR_MASK           (1<<1)
#define UART_X_ERR_MASK     (PERR_MASK | FERR_MASK | OERR_MASK)

/* MAX COM numer = 0 ~ 5 */
//tUartComm * UsedUart[6];// = {NULL ,NULL ,NULL ,NULL ,NULL ,NULL};

/* 与物理串口对应的控制量 */
#define PHY_UART_NUM    6
tUartComm * PhyUartArray[PHY_UART_NUM] = {NULL ,NULL ,NULL ,NULL ,NULL ,NULL};

#ifdef  UART_RX_SOFTBUFF
PRIVATE void processUartIF_Tx(int32u uart_id);
#endif

#ifdef  UART_TX_BY_DMA
PRIVATE void processUartDMA_FinishedTx(int32u uart_id);
#endif

/* =============================================================================
 * 虚拟串口号查找对应的物理串口号
 * 输入
 *      virCom   : 虚拟 COM号
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
 * 启动串口发送数据
 * 输入
 *      virCom   : 虚拟 COM号
 *      sendData : 需要发送的数据
 *      sendLen  : 需要发送的数据的长度
 * 返回
 *     发送状态 tEnmComTxSts
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
        /* 对于 2W-485，启动发送需要切换到发送状态 */
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
         * 如果待发送的数据指针指向串口自己的发送缓冲区，则跳开复制过程
         * 这样处理，应用层可以直接向串口本身的发送缓冲区填充数据
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
 * 停止串口发送数据
 * 输入
 *      virCom      : 虚拟串口号
 * 返回
 *     发送状态 tEnmComTxSts
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
            /* COM号指向的串口还有未完成的发送任务，被强行停止 */
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
        
        /* 默认开启接收使能，避免在2线485时，关闭串口接收后没有打开接收 */
        if(pUartCOM->fpRE_En != NULL)
            pUartCOM->fpRE_En();
    }
    
	return ret;	
}


/* =============================================================================
 * 重置串口接收的数据
 * 输入
 *      virCom      : 虚拟串口号
 * 返回
 *     TRUE 表示OK
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
 * 检测通信链接及发送状态
 * 输入
 *      virCom      : 虚拟串口号
 * 返回
 *      如果链接状态变化为失败，返回TRUE
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
            /* 检测通信链接 */
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
                /* 使能了发送超时检测 */
                pUartCOM->uartSts.TxOVTM = 0;
                if(pUartCOM->sendOverTime != 0)
                {
                    if(getSysTimeDiff(pUartCOM->startSendTime) >= pUartCOM->sendOverTime)
                    {
                        /* 发送超时 */
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
 * 获取串口发送数据状态
 * 输入
 *      virCom      : 虚拟串口号
 * 返回
 *     发送状态 tEnmComTxSts
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
 * 串口重置
 * 输入
 *      virCom      : 虚拟串口号
 * 返回
 *     发送状态 tEnmComTxSts (COM_TX_OK 表示重置成功)
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
 * 串口初始化函数
 * 输入
 *   virCom    : 虚拟串口号
 *   pUartCOM  : 指向串口结构体
 *   dataRate  : 串口波特率
 *   priority  : 中断优先级
 * 返回
 *     初始化结果 tEnmComIniSts
 */
PUBLIC tEnmComIniSts uartCOM_Init(tEnmComList virCom, UART_MODULE phy_id, tUartComm * pUartCOM,  int32u dataRate, INT_PRIORITY priority)
{
    if(phy_id > UART6)
    {
        return COM_INI_ERR_COMID;
    }
    
    /* 参数判断 */
    if((pUartCOM == NULL) || (dataRate ==0) || (dataRate > 1000000))
    {
        return COM_INI_ERR_PARA;
    }
    
    /* 串口是否已被占用 */
    if(PhyUartArray[phy_id] != NULL)
    {
        return COM_INI_ERR_OCCUPY;
    }
        
    /* 初始化 */
    pUartCOM->usrComNo = virCom;
    pUartCOM->uartID   = phy_id;
    PhyUartArray[phy_id] = pUartCOM;
    
    #ifndef UART_TX_BY_DMA
    #ifdef  UART_RX_SOFTBUFF
    /* 初始化发送数据的中断处理函数
     * id = 物理串口号对象 */
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
    
    /* 关闭UART ，主要是清错误标志 */
    UARTEnable(phy_id, UART_DISABLE_FLAGS(UART_RX | UART_TX));
    
    /* clear u1art err/rx/tx interrupt flag */
    INTClearFlag(INT_SOURCE_UART_TX(phy_id));
    INTClearFlag(INT_SOURCE_UART_RX(phy_id));
    INTClearFlag(INT_SOURCE_UART_ERROR(phy_id));
    uartReg[phy_id]->sta.reg = 0;

    /* 中断优先级 */
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
 * 串口的收发数据中断处理
 * （ 所有的 UART 数据收发中断都在这里处理 ）
 */
int oerrU6_cnt = 0;
PRIVATE void processUartComm(UART_MODULE uart_id)
{
    tSYSTICK    nowTick;
    tUartComm * pComm;
    int8u rxData;
    
    /* 错误中断? */
    if(INTGetFlag(INT_SOURCE_UART_ERROR(uart_id)))
    {
        INTClearFlag(INT_SOURCE_UART_ERROR(uart_id));
    }
    
    /* 与 uart_id 对应的 COM 口 */
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
        /* 读出 FIFO? */
        while (UARTReceivedDataIsAvailable(uart_id))
        {
            rxData = UARTGetDataByte(uart_id);
        }
        #endif
        uartReg[uart_id]->sta.clr = _U1STA_OERR_MASK;       /* 清除接收溢出错误 */
        pComm->RxTxBuf.rxCnt = 0;                           /* 为了重新搜索信息头 */
        
		/* Clear the RX interrupt Flag */
	    INTClearFlag(INT_SOURCE_UART_RX(uart_id));
    }
    
    /* 接收数据 */
	if(INTGetFlag(INT_SOURCE_UART_RX(uart_id)))
	{
		/* Clear the RX interrupt Flag */
	    INTClearFlag(INT_SOURCE_UART_RX(uart_id));
        
		while (UARTReceivedDataIsAvailable(uart_id))
		{
    		rxData = UARTGetDataByte(uart_id);
            nowTick = getSysTick();
            
            #if 0
            /* 回显接收数据 */
            UARTSendDataByte(uart_id, rxData);
            /* 用 MMS 通信串口回显接收数据 */
            //UARTSendDataByte(UART2, rxData);
            #endif
            
            /* 接收的数据长度超过接收缓冲区容量，则清空接收重新计数 */
            if(pComm->RxTxBuf.rxCnt >= pComm->RxTxBuf.rxBufSize)
            {
                pComm->RxTxBuf.rxCnt = 0;
            }
            
            /* 调用接收处理函数 */
    		if(pComm->funRx != NULL)
    	    {
                if(pComm->frameOverTime > 0)
                {
                    /* 两字节之间的接收时间限制，超过时间则认为是新的一帧数据，重新接收计数 */
                    if((tSYSTICK)(nowTick - pComm->lastByteTick) >= pComm->frameOverTime)
                    {
                        pComm->RxTxBuf.rxCnt = 0;
                    }
                }

                /* 回调函数，处理接收字节 */
                pComm->funRx(uart_id, rxData);
            }
            
            pComm->lastByteTick = nowTick;
        }
        
    }
    
    
    /* 发送数据 */
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
            
            /* 为了保证总线上的发送数据持续一段时间
             * 利用 PIC32 的发送中断进行计时
             * 延时后再切换 
             * 对 4w-485 不需要延迟
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

                    /* 对于 2W-485，发送结束后需要切换到接收 */
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
 * 所有 UART 通信中断入口 
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
 * 检查串口错误，清除错误并初始化
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
 * 串口发送中断处理函数，由串口中断调用
 * (UART_MODULE)
 */
PRIVATE void processUartIF_Tx(int32u uart_id)
{
    tUartComm * pComm;
    
    /* Clear the TX interrupt Flag */
    //INTClearFlag(INT_SOURCE_UART_TX(uart_id));
    
    /* 与 uart_id 对应的 COM 口 */
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

        /* 为了保证总线上的发送数据持续一段时间
         * 利用 PIC32 的发送中断进行计时
         * 延时后再切换 
         * 对 4w-485 不需要延迟
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

                /* 对于 2W-485，发送结束后需要切换到接收 */
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
 * 串口发送通过DMA实现，
 * DMA传输完成以后（内存到UART TX FIFO），需要串口FIFO全部发送结束再处理(2线-4线切换)
 * 
 * uart_id (UART_MODULE)
 */
PRIVATE void processUartDMA_FinishedTx(int32u uart_id)
{
    tUartComm * pComm = NULL;
    
    if(uart_id >= PHY_UART_NUM)
        return;
    
    /* 与 uart_id 对应的 COM 口 */
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

        /* 对于 2W-485，发送结束后需要切换到接收 */
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
 * 串口接收中断将数据保存到接收缓冲区，
 * 在主程序中检查串口接收缓冲区的数据，并解析数据帧，避免在中断中解析接收数据占有CPU时间
 * 
 * !!!!! 必须在主循环中调用 !!!!!
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
        /* 查找与 物理id 对应的 虚拟COM 口 */
        pComm = PhyUartArray[phyID];
        
        if(pComm == NULL)
            continue;
        
        /* 该物理串口是否有接收到数据：从接收缓存区读取
         * 若有则最多连续处理 5 个数据，避免处理单个串口占用CPU过长 */
        dealLen = 5;
        while(dealLen--)
        {
            if(readUartRxByte(phyID ,&rxData ) == FALSE)
                break;
            
            /* 最后接收的字节，时间 */
            nowTick = getLastRxByteTick(phyID);//getSysTick();

            /* 接收的数据长度超过接收缓冲区容量，则清空接收重新计数 */
            if(pComm->RxTxBuf.rxCnt >= pComm->RxTxBuf.rxBufSize)
            {
                pComm->RxTxBuf.rxCnt = 0;
            }

            /* 调用接收处理函数 */
            if(pComm->funRx != NULL)
            {
                if(pComm->frameOverTime > 0)
                {
                    /* 两字节之间的接收时间限制，超过时间则认为是新的一帧数据，重新接收计数 */
                    if((tSYSTICK)(nowTick - pComm->lastByteTick) >= pComm->frameOverTime)
                    {
                        pComm->RxTxBuf.rxCnt = 0;
                    }
                }

                /* 回调函数，处理接收字节 */
                pComm->funRx(phyID, rxData);
            }

            pComm->lastByteTick = nowTick;
        }
    }
}


#endif

