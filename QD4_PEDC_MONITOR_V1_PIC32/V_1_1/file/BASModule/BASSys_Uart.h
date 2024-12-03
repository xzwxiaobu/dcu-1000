#ifndef	_BASSYS_UART_H
#define	_BASSYS_UART_H

#include "_cpu.h"

#include    "SigSys_Uart.h"

/* PCB ԭ��ͼ: PEDC_MONITOR_SZ16.pdf
 * PEDC �� BASϵͳͨ��
 * ͨ�Ŵ����Э�����ÿ����Ŀ����ȷ��
 *  MODBUS��RS485-2W, 9600bps , UART6 (�� CAN �ڽ�Զ��)
 */
#define RS485_BASSYS_4W             FALSE    /* TRUE �����4W��ʽ����, FALSE=2w */
#define BASSYS_OFFLIN_TIME          5000    /* ms */
#define MODBUS_FRAME_FINISH_TIME    20      /* ms */

/* ͨ�������ںţ�Ӳ���� */
#define UART_BASSYS_ID              UART6
#define UART_BASSYS_BR              9600
#define UART_BASSYS_IPL             INT_PRIORITY_LEVEL_5

#if 0
//�˲��ֶ������ݰ����� "SigSys_Uart.h" ��
//UART6 - SIG1 (TXD-S1 & RXD-S1)
//RE-S1 : RD4,  DE-S1 : RC14
#define RE_U6_DISABLE()             mPORTDSetBits(BIT_4)
#define RE_U6_ENABLE()              mPORTDClearBits(BIT_4)
#define DE_U6_DISABLE()             mPORTCClearBits(BIT_14)
#define DE_U6_ENABLE()              mPORTCSetBits(BIT_14)

//UART1 - SIG2 (TXD-S2 & RXD-S2)
//RE-S2 : RD6,  DE-S2 : RD5
#define RE_U1_DISABLE()             mPORTDSetBits(BIT_6)
#define RE_U1_ENABLE()              mPORTDClearBits(BIT_6)
#define DE_U1_DISABLE()             mPORTDClearBits(BIT_5)
#define DE_U1_ENABLE()              mPORTDSetBits(BIT_5)
#endif

/* RS485 Э��ת��оƬ�Ŀ������Ŷ��� */
#define RE_BASSYS_DISABLE()         RE_U6_DISABLE()
#define RE_BASSYS_ENABLE()          RE_U6_ENABLE()
#define DE_BASSYS_DISABLE()         DE_U6_DISABLE()
#define DE_BASSYS_ENABLE()          DE_U6_ENABLE()

/* �����շ���������С
 * 
 */
#define UART_BASSYS_RXBUFSIZE       (32)
#define UART_BASSYS_TXBUFSIZE       (32)


typedef BOOL (* cbAppBASSysProcFrame)(int8u * data, int len);

PUBLIC BOOL getBASSysOnLineSts(void);

PUBLIC BOOL ifBASSysUartTxIsBusy(void);

PUBLIC void initBASSysModule(void);

PUBLIC BOOL procBASSysRxFrame(cbAppBASSysProcFrame BASSysApp);

PUBLIC BOOL chkBASSysFinishSend(void);


#endif  //_BASSYS_UART_H



