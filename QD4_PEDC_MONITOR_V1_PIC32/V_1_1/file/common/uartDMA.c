
#include <plib.h>
#include "peripheral\uart.h"
#include "uartDrv.h"

/* ÿ�����ڷ��Ͷ�Ӧһ�� DMA ͨ��
 * Ĭ�� UART(n) : DMA(n)
 * 
 * ע�⣺���� UART ���� FIFO ��ԭ��DMA ������ɲ����� UART�������
 * ���Զ���2���� 2587�ķ���ʹ�ܿ������ţ��� DMA ��������ж��в��������ر�
 *  */
typedef struct
{
    BOOL DMAInt_Init;       /* DMA �жϳ�ʼ����־ */
    BOOL DMABusy;           /* DMA æ��־   */
    BOOL DMAFinishedIF;     /* DMA ��������жϱ�־ */
    tUartDMASetting uartDMA;
}tDMAWorkSts;

tDMAWorkSts DMAWorkSts[DMA_CHANNELS];


/* ��� UART �����ж������� */
BOOL getDMA_UartTxIRQNum(UART_MODULE uid, int32u * txIRQNo)
{    
    int txIRQ;
    
    switch(uid)
    {
        case UART1:
            txIRQ = _UART1_TX_IRQ;
            break;
        case UART2:
            txIRQ = _UART2_TX_IRQ;
            break;
        case UART3:
            txIRQ = _UART3_TX_IRQ;
            break;
        case UART4:
            txIRQ = _UART4_TX_IRQ;
            break;
        case UART5:
            txIRQ = _UART5_TX_IRQ;
            break;
        case UART6:
            txIRQ = _UART6_TX_IRQ;
            break;
            
        default:
            return FALSE;
    }

    *txIRQNo = txIRQ;
    
    return TRUE;
}


/* ���ݴ��� ID ��� ���͹��ܶ�Ӧ�� DMA ͨ�� */
BOOL getDMA_UartTxDMAChnl(UART_MODULE uid, DmaChannel *dmachnl)
{    
    DmaChannel chnl;
    
    switch(uid)
    {
        case UART1:
            chnl = DMA_CHANNEL1;
            break;
        case UART2:
            chnl = DMA_CHANNEL2;
            break;
        case UART3:
            chnl = DMA_CHANNEL3;
            break;
        case UART4:
            chnl = DMA_CHANNEL4;
            break;
        case UART5:
            chnl = DMA_CHANNEL5;
            break;
        case UART6:
            chnl = DMA_CHANNEL6;
            break;
            
        default:
            return FALSE;
    }
    
    *dmachnl = chnl;

    return TRUE;
}

/* ����DMA ͨ�� ��ö�Ӧ�� ���� ID   */
BOOL getDMA_UartModuleID( DmaChannel dmachnl, UART_MODULE *uart_id)
{
    UART_MODULE uid;
    
    switch(dmachnl)
    {
        case DMA_CHANNEL1:
            uid = UART1;
            break;
        case DMA_CHANNEL2:
            uid = UART2;
            break;
        case DMA_CHANNEL3:
            uid = UART3;
            break;
        case DMA_CHANNEL4:
            uid = UART4;
            break;
        case DMA_CHANNEL5:
            uid = UART5;
            break;
        case DMA_CHANNEL6:
            uid = UART6;
            break;
            
        default:
            return FALSE;
    }
    
    *uart_id = uid;

    return TRUE;
}

/* =============================================================================
 * ���� DMA ʵ�ִ��ڷ���
 *  */
PUBLIC BOOL uartStartSend_DMA(tUartDMASetting *ptrUartDMA)
{
    int32u      txIRQ;
    DmaChannel  dmachnl;
    
	/* set the events: now the start event is the UART tx being empty */
    if(getDMA_UartTxIRQNum(ptrUartDMA->uartID, &txIRQ) == FALSE)
        return FALSE;
    
    if(getDMA_UartTxDMAChnl(ptrUartDMA->uartID, &dmachnl) == FALSE)
        return FALSE;
    
//    if(DMAWorkSts[dmachnl].DMABusy == TRUE)
//        return FALSE;
    
    DMAWorkSts[dmachnl].uartDMA = *ptrUartDMA;
    
    DmaChnOpen(dmachnl, DMA_CHN_PRI2, DMA_OPEN_DEFAULT);
    
    /* open dma channel */
    //if(DMAWorkSts[dmachnl].DMAInt_Init == FALSE)
    {
        /* set INT priority */
        INTSetVectorPriority(INT_VECTOR_DMA(dmachnl), INT_PRIORITY_LEVEL_5);
        INTSetVectorSubPriority(INT_VECTOR_DMA(dmachnl), INT_SUB_PRIORITY_LEVEL_0);
    
        DMAWorkSts[dmachnl].DMAInt_Init = TRUE;
    }
    
	DmaChnSetEventControl(dmachnl, DMA_EV_START_IRQ_EN|DMA_EV_START_IRQ(txIRQ));

	/* set the transfer source and dest addresses, source and dest size and cell size */
	DmaChnSetTxfer(dmachnl, ptrUartDMA->pTxData, (void*)&(uartReg[ptrUartDMA->uartID]->tx.reg), ptrUartDMA->TxDataLen, 1, 1);

    /* enable the transfer done interrupt: pattern match or all the characters transferred */
	DmaChnSetEvEnableFlags(dmachnl, DMA_EV_BLOCK_DONE);		

    /* enable the chn interrupt in the INT controller */
	INTEnable(INT_SOURCE_DMA(dmachnl), INT_ENABLED);
    
    DMAWorkSts[dmachnl].DMABusy = TRUE;
    
    /* ʹ��ͨ���������������䣬����ֻ��Ҫִ������һ��
     * ͬʱִ�л���ָ����ã������Ԥ�ڳ��ȵ�����
     * 
     * DmaChnEnable(dmachnl);
     * 
     * DmaChnStartTxfer 
     *  */
    
    /* force the DMA transfer: the UART tx flag it's already been active */
	DmaChnStartTxfer(dmachnl, DMA_WAIT_NOT, 0);
    
    return TRUE;
}


/* =============================================================================
 * ֹͣ DMA �Ĵ��ڷ���
 *  */
PUBLIC BOOL uartStopSend_DMA(UART_MODULE uid)
{
    DmaChannel  dmachnl;
    
    if(getDMA_UartTxDMAChnl(uid, &dmachnl) == FALSE)
        return FALSE;
        
    /* disalbe dma channel */
    DmaChnDisable(dmachnl);
    DmaChnAbortTxfer(dmachnl);
    
    DMAWorkSts[dmachnl].DMAInt_Init = FALSE;
}

void uartDMA_IRQ_Handler(DmaChannel dmaChnl)
{
    int evFlags;
    if(INTGetFlag(INT_SOURCE_DMA(dmaChnl)))
    {
        /* clear the interrupt */
        INTClearFlag(INT_SOURCE_DMA(dmaChnl)); 

        /*  get the event flags */
        evFlags = DmaChnGetEvFlags(dmaChnl);

        /* enabled just the DMA_EV_BLOCK_DONE transfer done interrupt */
        if(evFlags & DMA_EV_BLOCK_DONE)
        {
            DmaChnClrEvFlags(dmaChnl, DMA_EV_BLOCK_DONE);
            INTEnable(INT_SOURCE_DMA(dmaChnl), INT_DISABLED);
            
            DMAWorkSts[dmaChnl].DMABusy = FALSE;
            DMAWorkSts[dmaChnl].DMAFinishedIF = TRUE;
        }
    }
}

/* =============================================================================
 *  */
/* handler for the DMA channel 1~6 interrupt */
void __ISR(_DMA_1_VECTOR, IPL5SOFT) DmaHandler1(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL1);
}
void __ISR(_DMA_2_VECTOR, IPL5SOFT) DmaHandler2(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL2);
}
void __ISR(_DMA_3_VECTOR, IPL5SOFT) DmaHandler3(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL3);
}
void __ISR(_DMA_4_VECTOR, IPL5SOFT) DmaHandler4(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL4);
}
void __ISR(_DMA_5_VECTOR, IPL5SOFT) DmaHandler5(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL5);
}
void __ISR(_DMA_6_VECTOR, IPL5SOFT) DmaHandler6(void)
{    
    uartDMA_IRQ_Handler(DMA_CHANNEL6);
}


/* =============================================================================
 * ��鴮�ڷ��ͽ����������� DMA ��������ж��д���
 * 
 * �ȴ����ͻ�����ȫ�������Ժ󣬹ر�2587�ķ���ʹ��
 *  */
PUBLIC void checkUartTxDMAFinished(void)
{
    UART_MODULE uid;
    DmaChannel chn = 0;
    for(chn=DMA_CHANNEL0; chn<DMA_CHANNELS; chn ++)
    {
        if(DMAWorkSts[chn].DMAFinishedIF == TRUE)
        {
            if(getDMA_UartModuleID(chn, &uid) == FALSE)
                continue;
            
            if(UARTTransmissionHasCompleted(uid) == FALSE)
                continue;
            
            DMAWorkSts[chn].DMAFinishedIF = FALSE;
        
//            /* disable the chn */
//            DmaChnDisable(chn);
//            DMAWorkSts[chn].DMAInt_Init = FALSE;

            /* ���ͽ����Ժ�Ļص����� */
            if(DMAWorkSts[chn].uartDMA.cbDMAFinished != NULL)
            {
                DMAWorkSts[chn].uartDMA.cbDMAFinished(uid);
            }
        }
    }
}
