#ifndef	_SIGSYS_UART_H
#define	_SIGSYS_UART_H

#include "_cpu.h"

/* PEDC 与 信号系统通信
 * 通信传输层协议根据每个项目进行确定
 *  NanJing7 ： MODBUS，RS485-4W, 57600bps, UART6
 *  JiNan-R2 ： MODBUS，RS485-2W, 9600bps , UART6
 */
#define RS485_SIGSYS_4W             TRUE    /* TRUE 软件按4W方式工作, FALSE=2w */
#define SIGSYS_OFFLIN_TIME          5000    /* ms */
#define MODBUS_FRAME_FINISH_TIME    20      /* ms */

/* 通信物理串口号（硬件） */
#define UART_SIGSYS_ID              UART6
#define UART_SIGSYS_BR              57600
#define UART_SIGSYS_IPL             INT_PRIORITY_LEVEL_5


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

/* RS485 协议转换芯片的控制引脚定义 */
#define RE_SIGSYS_DISABLE()         RE_U6_DISABLE()
#define RE_SIGSYS_ENABLE()          RE_U6_ENABLE()
#define DE_SIGSYS_DISABLE()         DE_U6_DISABLE()
#define DE_SIGSYS_ENABLE()          DE_U6_ENABLE()

/* 定义收发缓冲区大小
 * 
 */
#define UART_SIGSYS_RXBUFSIZE       (32)
#define UART_SIGSYS_TXBUFSIZE       (32)


typedef BOOL (* cbAppSigSysProcFrame)(int8u * data, int len);

PUBLIC BOOL getSigSysOnLineSts(void);

PUBLIC BOOL ifSigSysUartTxIsBusy(void);

PUBLIC void initSigSysModule(void);

PUBLIC BOOL procSigSysRxFrame(cbAppSigSysProcFrame sigSysApp);

PUBLIC BOOL chkSigSysFinishSend(void);


#endif  //_SIGSYS_UART_H



