
#ifndef _Ini_UART_H
#define _Ini_UART_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

//LLY note : 
//       U1             U2          U3              U4          U6
//JNR2   BAS system     MMS         PIC24A or B     I,P,G子板    
//***********************************************************
//LLY_JN R2
//#define MMS_UART            UART2           //MMS 通讯使用的 , DE-PC
//#define IbpPslSig_UART      UART4           //IBP PSL SIG 通讯, DE-M

//#define PIC18A_UART         UART3           //和 PIC24 A 通讯
//#define PIC18B_UART         UART3           //
//#define PIC24A_UART         PIC18A_UART     //
//#define PIC24B_UART         PIC18B_UART     //


//#define PIC18A_UART         UART3           //和 PIC24 A 通讯
//#define PIC18B_UART         UART3           //
//#define PIC24A_UART         PIC18A_UART     //
//#define PIC24B_UART         PIC18B_UART     //


//#define INI_PIC18A_UART()  {UartComm_Pic24_INI();}
//#define INI_PIC18B_UART()  {UartComm_Pic24_INI();}
//
//#define TX_TO_PIC24A        0x80
//#define RX_FM_PIC24A        0x81
//#define TX_TO_PIC24B        0x90
//#define RX_FM_PIC24B        0x91
//
///* MMS */
////DE-PC : RF3
//#define DE_PC_DISABLE()     mPORTFClearBits(BIT_3)
//#define DE_PC_ENABLE()      mPORTFSetBits(BIT_3)
//#define DE_MMS_DISABLE()    DE_PC_DISABLE()
//#define DE_MMS_ENABLE()     DE_PC_ENABLE()

///* IBP PSL SIG */
////RE-M  : RD10
////DE-M  : RD11
//#define RE_M_DISABLE()      mPORTDSetBits(BIT_10)
//#define RE_M_ENABLE()       mPORTDClearBits(BIT_10)
//#define DE_M_DISABLE()      mPORTDClearBits(BIT_11)
//#define DE_M_ENABLE()       mPORTDSetBits(BIT_11)
////IPS = IBP PSL SIG 3pcbs
//#define RE_IPS_DISABLE()    RE_M_DISABLE()
//#define RE_IPS_ENABLE()     RE_M_ENABLE()
//#define DE_IPS_DISABLE()    DE_M_DISABLE()
//#define DE_IPS_ENABLE()     DE_M_ENABLE()


////UART1
////RE-S2 : RD6
////DE-S2 : RD5
//#define RE_S2_DISABLE()     mPORTDSetBits(BIT_6)
//#define RE_S2_ENABLE()      mPORTDClearBits(BIT_6)
//#define DE_S2_DISABLE()     mPORTDClearBits(BIT_5)
//#define DE_S2_ENABLE()      mPORTDSetBits(BIT_5)
//
////UART6
////RE-S1 : RD4
////DE-S1 : RC14
//#define RE_S1_DISABLE()         mPORTDSetBits(BIT_4)
//#define RE_S1_ENABLE()          mPORTDClearBits(BIT_4)
//#define DE_S1_DISABLE()         mPORTCClearBits(BIT_14)
//#define DE_S1_ENABLE()          mPORTCSetBits(BIT_14)
//
//
///* BAS system */
////UART1
//#define BAS_UART                UART1
//#define BAS_UART_BAUDRATE       9600
//
//#define RE_BASSYS_DISABLE()    RE_S2_DISABLE()
//#define RE_BASSYS_ENABLE()     RE_S2_ENABLE()
//#define DE_BASSYS_DISABLE()    DE_S2_DISABLE()
//#define DE_BASSYS_ENABLE()     DE_S2_ENABLE()



#if 0
/* signal system 1 */
#define RE_SIGSYS1_DISABLE()    RE_S1_DISABLE()
#define RE_SIGSYS1_ENABLE()     RE_S1_ENABLE()
#define DE_SIGSYS1_DISABLE()    DE_S1_DISABLE()
#define DE_SIGSYS1_ENABLE()     DE_S1_ENABLE()

/* signal system 2 */

#define RE_SIGSYS2_DISABLE()    RE_S2_DISABLE()
#define RE_SIGSYS2_ENABLE()     RE_S2_ENABLE()
#define DE_SIGSYS2_DISABLE()    DE_S2_DISABLE()
#define DE_SIGSYS2_ENABLE()     DE_S2_ENABLE()

/////
#define SIG_UART_6                           //定义后用 UART6 与信号系统通信，否则用 UART1
#define SIG_UART_BAUDRATE   57600           //MOXA MB3180 不支持56000

#ifdef SIG_UART_6
#define SIG_UART            UART6           //与信号系统通讯
#define RE_SIG_DISABLE()        RE_SIGSYS1_DISABLE()
#define RE_SIG_ENABLE()         RE_SIGSYS1_ENABLE()
#define DE_SIG_DISABLE()        DE_SIGSYS1_DISABLE()
#define DE_SIG_ENABLE()         DE_SIGSYS1_ENABLE()

#else

#define SIG_UART            UART1
#define RE_SIG_DISABLE()        RE_SIGSYS2_DISABLE()
#define RE_SIG_ENABLE()         RE_SIGSYS2_ENABLE()
#define DE_SIG_DISABLE()        DE_SIGSYS2_DISABLE()
#define DE_SIG_ENABLE()         DE_SIGSYS2_ENABLE()
#endif
#endif 

//***********************************************************
//EXTN void UART_ERR_INI(void);
//
//EXTN void UART_MMS_INI();
//EXTN void UART_BAS_INI();                /* UART SIG */
//EXTN void UartComm_Pic24_INI();         /* UART PIC24 */
//EXTN void UART_IbpPslSig_INI();         /* UART IBP,PSL,SIG */
//
//EXTN void UART_DBG_INI();

//LLY NJ7
extern void dbgPrintMsg(int8u * pMsg, int16u len, BOOL bWait);
extern void dbgPrintHexMsg(int8u * pMsg, int16u len, BOOL bWait);

//***********************************************************
#endif

