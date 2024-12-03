#include "..\Include.h"

//void UART_INI(UART_MODULE id,  UINT32 dataRate, INT_PRIORITY priority)
//{
//    UARTEnable(id, UART_DISABLE_FLAGS(UART_RX | UART_TX));              	//关闭UART ，主要是清错误标志
//    INTClearFlag(INT_SOURCE_UART_TX(id));                               	//clear u1art err/rx/tx interrupt flag
//    INTClearFlag(INT_SOURCE_UART_RX(id));
//    INTClearFlag(INT_SOURCE_UART_ERROR(id));
//    uartReg[id]->sta.reg = 0;
//
//    INTSetVectorPriority(INT_VECTOR_UART(id), priority);				//中断优先级
//    INTSetVectorSubPriority(INT_VECTOR_UART(id), INT_SUB_PRIORITY_LEVEL_0);
//
//    UARTConfigure(id, UART_ENABLE_PINS_TX_RX_ONLY);
//
//	UARTSetFifoMode(id, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
//
//    UARTSetLineControl(id, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
//
//    UARTSetDataRate(id, GetPeripheralClock(), dataRate);
//
//   	UARTEnable(id, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));
//
//    INTEnable(INT_SOURCE_UART_RX(id), INT_ENABLED);                    //uart rx interrupt enable
//    INTEnable(INT_SOURCE_UART_TX(id), INT_ENABLED);                    //uart tx interrupt enable
//}


//void UART_IbpPslSig_INI(void)
//{
//  	IPS_TxCnt = 0;
//    IPS_TxDataLen  = 0;
//    IPS_RxCnt  = 0;
//    //LLY_NJ7
//    //RE_IPS_DISABLE();
//    DE_IPS_DISABLE();
//    //DE_DISABLE();                                               //DE port disable
//    UART_INI(IbpPslSig_UART, 56000, INT_PRIORITY_LEVEL_5);
//    //DE_ENABLE();                                                //DE port enable
//    //RE_IPS_ENABLE();
//    DE_IPS_ENABLE();
//}

//This example configures the UART module at a 57.6k data rate with 8 bits of data,
//no parity, and one stop bit. The module uses RX and TX pins on the microcontroller.
//void UART_MMS_INI(void)
//{
//	 	tx2_cou = 0;
//		U2TxCou = 0;
//		U2RxCou = 0;
//		uUART2_RX_OVERTIME = 0;
//        //LLY_NJ7
//		//RE_S_DISABLE();
//		//DE_S_DISABLE();                                               //DE port disable
//        DE_MMS_DISABLE();
//		UART_INI(MMS_UART, 115200, INT_PRIORITY_LEVEL_5);
//        DE_MMS_ENABLE();
//		//RE_S_ENABLE();
//		//DE_S_ENABLE();                                                //DE port enable
//                                          //DE port enable
//}

//
//void UART_BAS_INI(void)
//{
//	//LLY_NJ7
//	//	switchTime    = 0;
//	//	switchCnt     = 0;
//    basSysTxLen       = 0;
//    basSysRxCnt       = 0;
//    u8BasSys_UartRx_Overtime = 0;
//
//    RE_BASSYS_DISABLE();
//    DE_BASSYS_DISABLE();
//    UART_INI(BAS_UART, BAS_UART_BAUDRATE, INT_PRIORITY_LEVEL_5);
//    //RE port enable
//    RE_BASSYS_ENABLE();
//}

//void UartComm_Pic24_INI(void)
//{
//  	PIC24_TxCnt_B = 0;
//    PIC24_TxDataLen_B = 0;
//    PIC24_RxCnt_B = 0;
//
//    UART_INI(PIC18A_UART, 56000, INT_PRIORITY_LEVEL_5);
//}

//U2STA bit3(PERR)/bit2(PERR)/bit1(OERR) 其中一位置一时，初始化UART2所有的配置
//void UART_ERR_INI(void)
//{
//    static tSYSTICK tickPrev = 0;
//    tSYSTICK interval;
//    tSYSTICK tickNow;
//    
//    tickNow = getSysTick();
//    interval = (tSYSTICK)(tickNow - tickPrev);
//    if(interval < 200)
//    {
//        return;
//    }
//    tickPrev = tickNow;
    
//	if ( INTGetFlag(INT_SOURCE_UART_ERROR(MMS_UART)) )
//	{
//		INTClearFlag(INT_SOURCE_UART_ERROR(MMS_UART));
//		UART_MMS_INI();
//	}
//
//	if ( INTGetFlag(INT_SOURCE_UART_ERROR(BAS_UART)) )
//	{
//		INTClearFlag(INT_SOURCE_UART_ERROR(BAS_UART));
//		UART_BAS_INI();
//	}
//
//	if ( INTGetFlag(INT_SOURCE_UART_ERROR(PIC18A_UART)) )
//	{
//		INTClearFlag(INT_SOURCE_UART_ERROR(PIC18A_UART));
//		UartComm_Pic24_INI();
//	}
//
//	if ( INTGetFlag(INT_SOURCE_UART_ERROR(IbpPslSig_UART)) )
//	{
//		INTClearFlag(INT_SOURCE_UART_ERROR(IbpPslSig_UART));
//		UART_IbpPslSig_INI();
//	}
//
//	uUART_ERR_TEST_TIME = 0;
//}

/* =============================================================================
 * 
 */
void dbgPrintMsg(int8u * pMsg, int16u len, BOOL bWait)
{
    #if (DEBUG_ENABLE != 0)
    int i;
    
    if(dbgTxLen > 0)
        return;
    
    if(len == 0)
        return;
    
    if(len > sizeof(dbgTxBuf))
    {
        len = sizeof(dbgTxBuf);
    }
    
    for(i=0; i<len; i++)
    {
        dbgTxBuf[i] = pMsg[i];
    }
    //memcpy(dbgTxBuf, pMsg, len);
    
    dbgTxCnt = 0;
    dbgTxLen = len;
    
    INTClearFlag(INT_SOURCE_UART_TX(DEBUG_UART));
    INTEnable(INT_SOURCE_UART_TX(DEBUG_UART), INT_ENABLED);
    
    if(bWait)
    {
        while(dbgTxLen > 0){ ; }
    }
    #endif
}

#define NIBBLE_TO_ASCII(a) ((a) > 9 ? ((a)-10 + 'A') : ((a) + '0'))

void dbgPrintHexMsg(int8u * pMsg, int16u len, BOOL bWait)
{
    #if (DEBUG_ENABLE != 0)
    int16u i;
    
    if(dbgTxLen > 0)
        return;
    
    if(len == 0)
        return;
    
    if(len > sizeof(dbgTxBuf)/3 - 3)
    {
        len = sizeof(dbgTxBuf)/3 - 3;
    }
    
    for(i=0; i<len; i++)
    {
        //sprintf(&dbgTxBuf[i*3], "%02 ", *pMsg);
        dbgTxBuf[i*3 + 0] = NIBBLE_TO_ASCII((*pMsg >> 4) & 0x0f);
        dbgTxBuf[i*3 + 1] = NIBBLE_TO_ASCII((*pMsg     ) & 0x0f);
        dbgTxBuf[i*3 + 2] = ' ';
        pMsg++;
    }
    
    dbgTxCnt = 0;
    dbgTxLen = len;
    
    INTClearFlag(INT_SOURCE_UART_TX(DEBUG_UART));
    INTEnable(INT_SOURCE_UART_TX(DEBUG_UART), INT_ENABLED);
    
    if(bWait)
    {
        while(dbgTxLen > 0){ ; }
    }
    
    #endif
}
