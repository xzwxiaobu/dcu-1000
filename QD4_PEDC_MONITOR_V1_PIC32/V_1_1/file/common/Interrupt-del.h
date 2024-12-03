
#ifndef _INTERRUPT_H
#define _INTERRUPT_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
//**********************************************

//**********************************************


//#define U2_RX_NUM       (UPFW_CODE_BUF_MAX + UART2_TX_FILEMESSAGE_LEN + 200 + 30)       //+30 ���߽���

//#define U1_RX_NUM       (100 + 20)         //+20 ���߽���

//EXTN int8u aInterrupt_RX2_BUF[U2_RX_NUM];                          //�ж�UART2 ����
//
//#define aUART2_RX_BUF aInterrupt_RX2_BUF

//EXTN int8u Tx2_Ptr[UART2_TX_FILEMESSAGE_LEN + AVR_ONLINE_ALLDCUSTATE_LEN + 120];   //+120 ���߽���
//EXTN volatile int16u tx2_cou;
//EXTN volatile int16u U2RxCou;
//EXTN volatile int16u U2TxCou;

//pic18 A B
////UART1 Tx �������λ ,1 ��ʾ�д�������
//EXTN  BIT8 UP_PEDC;
//#define SF_FAULT               UP_PEDC.B0        //��ȫ�̵������ϣ�K31_K32�˵�ѹ �� K33���� ����������


//LLY JiNan2 BASϵͳͨ��
//#define BASSYS_DATA_LEN     50
//EXTN int8u basSysTxBuf[FRAME_MESSAGE_LEN + BASSYS_DATA_LEN + 5];                         //+5 ���߽���
//EXTN int8u basSysRxBuf[BASSYS_DATA_LEN];
//EXTN volatile int8u  basSysTxCnt;                       //�ѷ������ݼ�����
//EXTN volatile int8u  basSysTxLen;                       //�������ݵĳ���
//EXTN volatile int8u  basSysRxCnt;                       //�ѽ������ݼ�����
//EXTN int8u u8BasSys_UartRx_Overtime;                    //��ʱ������������ֽ�֮���ʱ�䳬���趨ֵ����������ҷ���ͷ
//#define BASSYS_UART_RX_OVERTIME             20 
//EXTN int8u u8BasSys_UartTx_Overtime;
//#define BASSYS_UART_TX_OVERTIME             50          // ms




////LLY_NJ7 PIC24A ͨ��
//#define PIC24_RX_TX_BUF_SIZE        40
//EXTN int8u PIC24_TxBuf_A[FRAME_MESSAGE_LEN+OUTPUT_DATA_LEN+5];                         //+5 ���߽���
//EXTN int8u PIC24_RxBuf_A[PIC24_RX_TX_BUF_SIZE];
//EXTN volatile int8u  PIC24_TxCnt_A;
//EXTN volatile int8u  PIC24_TxDataLen_A;
//EXTN volatile int8u  PIC24_RxCnt_A;
//EXTN int8u PIC24_RX_OVERTIME_A;                         //��ʱ������������ֽ�֮���ʱ�䳬���趨ֵ����������ҷ���ͷ
//#define defPIC24_RX_OVERTIME_A          20              //20 ms
//EXTN int8u uPIC24_TX_TIME_A;                            //�� PIC24 ��ʱ����
//#define defPIC24_TX_TIME_A              120             //120 ms
//
//
////LLY_NJ7 PIC24B ͨ��
//EXTN int8u PIC24_TxBuf_B[FRAME_MESSAGE_LEN+OUTPUT_DATA_LEN+5];                         //+5 ���߽���
//EXTN int8u PIC24_RxBuf_B[PIC24_RX_TX_BUF_SIZE];
//EXTN volatile int8u  PIC24_TxCnt_B;
//EXTN volatile int8u  PIC24_TxDataLen_B;
//EXTN volatile int8u  PIC24_RxCnt_B;
//EXTN int8u PIC24_RX_OVERTIME_B;                         //��ʱ������������ֽ�֮���ʱ�䳬���趨ֵ����������ҷ���ͷ
//#define defPIC24_RX_OVERTIME_B          20              //20 ms
//EXTN int8u uPIC24_TX_TIME_B;                            //��PIC24 ��ʱ����,����Ƿ�ͨѶ����
//#define defPIC24_TX_TIME_B              200             //200 ms

//IBP PSL SIG �Ӱ�ͨ����
//#define IPS_DATA_LEN    50
//EXTN int8u IPS_Tx_Ptr[FRAME_MESSAGE_LEN+IPS_DATA_LEN+5];                         //+5 ���߽���
//EXTN int8u IPS_RX_BUFF[IPS_DATA_LEN];
//EXTN volatile int8u  IPS_TxCnt;                         //���ͼ�����
//EXTN volatile int8u  IPS_TxDataLen;                     //���������ݵĳ���
//EXTN volatile int8u  IPS_RxCnt;                         //�ѽ������ݵĳ���
//EXTN int8u uIPS_UART_RX_OVERTIME;                       //��ʱ������������ֽ�֮���ʱ�䳬���趨ֵ����������ҷ���ͷ
//#define defIPS_UART_RX_OVERTIME 20                      //20 ms
//EXTN int8u uIPS_UART_TX_TIME;                           //�� PIC24 ��ʱ����
//#define defIPS_UART_TX_TIME    50                       //50 ms

//LLY NJ7  BIT8==>BIT16
EXTN volatile BIT16 uInterruptFLAG;
//#define bFun_Uart2_Receive_Handle   uInterruptFLAG.B0 //=1, UART2 ���յ�����
//#define bCrc_Err_flag               uInterruptFLAG.B1
//#define bUart1RX_Finish             uInterruptFLAG.B2
//#define Cpu_Sel_flag                uInterruptFLAG.B3
//#define Sel_Cpu_flag                uInterruptFLAG.B4
//#define b1msF                       uInterruptFLAG.B5 //1ms ��־
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

//EXTN int8u uUART2_RX_OVERTIME;                        //��ʱ������������ֽ�֮���ʱ�䳬���趨ֵ����������ҷ���ͷ
//#define defUART2_RX_uUART2_RX_OVERTIME 20             //20 ms

//LLY NJ7 �������
#if (DEBUG_ENABLE != 0)
EXTN int8u dbgTxBuf[256];
EXTN int16u dbgTxCnt;
EXTN int16u dbgTxLen;
#endif

//EXTN int8u checksum(pINT8U ptr, int16u len);
//EXTN int8u _checksum(pINT8U ptr, int16u len);         //���»���(_) ,�ж���

#endif

