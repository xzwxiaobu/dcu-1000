
#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
//**********************************************

//**********************************************


//#define U2_RX_NUM       (UPFW_CODE_BUF_MAX + UART2_TX_FILEMESSAGE_LEN + 200 + 30)       //+30 防边界用

//#define U1_RX_NUM       (100 + 20)         //+20 防边界用

//EXTN int8u aInterrupt_RX2_BUF[U2_RX_NUM];                          //中断UART2 接收
//
//#define aUART2_RX_BUF aInterrupt_RX2_BUF

//EXTN int8u Tx2_Ptr[UART2_TX_FILEMESSAGE_LEN + AVR_ONLINE_ALLDCUSTATE_LEN + 120];   //+120 防边界用
//EXTN volatile int16u tx2_cou;
//EXTN volatile int16u U2RxCou;
//EXTN volatile int16u U2TxCou;

//pic18 A B
////UART1 Tx 传输控制位 ,1 表示有传输任务
//EXTN  BIT8 UP_PEDC;
//#define SF_FAULT               UP_PEDC.B0        //安全继电器故障（K31_K32端电压 和 K33触点 来回跳动）


//LLY JiNan2 BAS系统通信
//#define BASSYS_DATA_LEN     50
//EXTN int8u basSysTxBuf[FRAME_MESSAGE_LEN + BASSYS_DATA_LEN + 5];                         //+5 防边界用
//EXTN int8u basSysRxBuf[BASSYS_DATA_LEN];
//EXTN volatile int8u  basSysTxCnt;                       //已发送数据计数器
//EXTN volatile int8u  basSysTxLen;                       //发送数据的长度
//EXTN volatile int8u  basSysRxCnt;                       //已接收数据计数器
//EXTN int8u u8BasSys_UartRx_Overtime;                    //超时错。如果相邻两字节之间的时间超过设定值，则会重新找发送头
//#define BASSYS_UART_RX_OVERTIME             20 
//EXTN int8u u8BasSys_UartTx_Overtime;
//#define BASSYS_UART_TX_OVERTIME             50          // ms




////LLY_NJ7 PIC24A 通信
//#define PIC24_RX_TX_BUF_SIZE        40
//EXTN int8u PIC24_TxBuf_A[FRAME_MESSAGE_LEN+OUTPUT_DATA_LEN+5];                         //+5 防边界用
//EXTN int8u PIC24_RxBuf_A[PIC24_RX_TX_BUF_SIZE];
//EXTN volatile int8u  PIC24_TxCnt_A;
//EXTN volatile int8u  PIC24_TxDataLen_A;
//EXTN volatile int8u  PIC24_RxCnt_A;
//EXTN int8u PIC24_RX_OVERTIME_A;                         //超时错。如果相邻两字节之间的时间超过设定值，则会重新找发送头
//#define defPIC24_RX_OVERTIME_A          20              //20 ms
//EXTN int8u uPIC24_TX_TIME_A;                            //主 PIC24 定时发送
//#define defPIC24_TX_TIME_A              120             //120 ms
//
//
////LLY_NJ7 PIC24B 通信
//EXTN int8u PIC24_TxBuf_B[FRAME_MESSAGE_LEN+OUTPUT_DATA_LEN+5];                         //+5 防边界用
//EXTN int8u PIC24_RxBuf_B[PIC24_RX_TX_BUF_SIZE];
//EXTN volatile int8u  PIC24_TxCnt_B;
//EXTN volatile int8u  PIC24_TxDataLen_B;
//EXTN volatile int8u  PIC24_RxCnt_B;
//EXTN int8u PIC24_RX_OVERTIME_B;                         //超时错。如果相邻两字节之间的时间超过设定值，则会重新找发送头
//#define defPIC24_RX_OVERTIME_B          20              //20 ms
//EXTN int8u uPIC24_TX_TIME_B;                            //副PIC24 定时发送,检测是否通讯正常
//#define defPIC24_TX_TIME_B              200             //200 ms

//IBP PSL SIG 子板通信用
//#define IPS_DATA_LEN    50
//EXTN int8u IPS_Tx_Ptr[FRAME_MESSAGE_LEN+IPS_DATA_LEN+5];                         //+5 防边界用
//EXTN int8u IPS_RX_BUFF[IPS_DATA_LEN];
//EXTN volatile int8u  IPS_TxCnt;                         //发送计数器
//EXTN volatile int8u  IPS_TxDataLen;                     //待发送数据的长度
//EXTN volatile int8u  IPS_RxCnt;                         //已接收数据的长度
//EXTN int8u uIPS_UART_RX_OVERTIME;                       //超时错。如果相邻两字节之间的时间超过设定值，则会重新找发送头
//#define defIPS_UART_RX_OVERTIME 20                      //20 ms
//EXTN int8u uIPS_UART_TX_TIME;                           //主 PIC24 定时发送
//#define defIPS_UART_TX_TIME    50                       //50 ms

//LLY NJ7  BIT8==>BIT16
EXTN volatile BIT16 uInterruptFLAG;
//#define bFun_Uart2_Receive_Handle   uInterruptFLAG.B0 //=1, UART2 接收到命令
//#define bCrc_Err_flag               uInterruptFLAG.B1
//#define bUart1RX_Finish             uInterruptFLAG.B2
//#define Cpu_Sel_flag                uInterruptFLAG.B3
//#define Sel_Cpu_flag                uInterruptFLAG.B4
//#define b1msF                       uInterruptFLAG.B5 //1ms 标志
//#define bPIC24B_RX_Finish           uInterruptFLAG.B6
//#define bUart_IPS_RX_Finish         uInterruptFLAG.B7
//#define bPIC24A_RX_Finish           uInterruptFLAG.B8
//#define bUart_SIG_RX_Finish         uInterruptFLAG.B9
//#define bUart_BAS_RX_Finish         uInterruptFLAG.B10

//EXTN volatile BIT16 wTimeFlag;
//#define b1ms       wTimeFlag.B0
//#define b2ms       wTimeFlag.B1
//#define b4ms       wTimeFlag.B2
//#define b8ms       wTimeFlag.B3
//#define b16ms      wTimeFlag.B4
//#define b32ms      wTimeFlag.B5
//#define b64ms      wTimeFlag.B6
//#define b128ms     wTimeFlag.B7
//#define b256ms     wTimeFlag.B8
//#define b512ms     wTimeFlag.B9
//#define b1024ms    wTimeFlag.B10
//#define b2048ms    wTimeFlag.B11
//#define bIO_512ms  wTimeFlag.B12
//#define bSysTime8ms wTimeFlag.B13

//EXTN volatile int16u wTimeCal;

//EXTN int8u aCrcErrBuff[2];

//EXTN int8u uUART2_RX_OVERTIME;                        //超时错。如果相邻两字节之间的时间超过设定值，则会重新找发送头
//#define defUART2_RX_uUART2_RX_OVERTIME 20             //20 ms

//LLY NJ7 调试输出
#if (DEBUG_ENABLE != 0)
EXTN int8u dbgTxBuf[256];
EXTN int16u dbgTxCnt;
EXTN int16u dbgTxLen;
#endif

//EXTN int8u checksum(pINT8U ptr, int16u len);
//EXTN int8u _checksum(pINT8U ptr, int16u len);         //加下划线(_) ,中断用

#endif

