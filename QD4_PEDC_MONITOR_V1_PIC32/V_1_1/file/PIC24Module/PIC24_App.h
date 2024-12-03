#ifndef _PIC24COMM_H
#define _PIC24COMM_H


#define UART_PIC24_ID               UART3                   /* 通信串口号       */
#define UART_PIC24_BR               57600                   /* 波特率           */
#define UART_PIC24_IPL              INT_PRIORITY_LEVEL_5    /* 中断优先级       */

#define MAINWORK_PIC24A             FALSE
#define MAINWORK_PIC24B             TRUE

/* 轮询任务 
 * 对于 RS485-2W，uartDrv 驱动底层自动管理
 * 对于发送超时：
 * 在 57600(1+8+1 bits)，按发送100字节，100*(1+8+1)/57600 ~= 17.4ms
 * 取发送超时 30ms
 * 考虑对方回复需要的时间，取单次轮询的时间 60ms
 */

#define PIC24_INTURN_TIME           60      /* 轮询一个对象的时间限制，必须大于 PIC24_TX_OVERTIME */
#define PIC24_TX_OVERTIME           30      /* 发送时间限制,ms */
#define PIC24_OFFLINE_NUM           5       /* 通信错误达到设定值后， 设置离线标记 */
#if (PIC24_INTURN_TIME <= PIC24_TX_OVERTIME)
#error "PIC24_INTURN_TIME must be lager than PIC24_TX_OVERTIME"
#endif

#define PIC24_SWITCH_ERRNUM         30      /* 通信错误达到设定值后， 切换主 PIC24 */
#define SELECTORPIC24_DELAY_MS      5       /* 选择PIC24的延时 */

PUBLIC tUartComm * getPic24UartCommPtr(void);
PUBLIC BOOL ifPic24UartTxIsBusy(void);

/* 接收PEDC数据包，进行解析后执行相应的任务并回复
 * 需要在主程序中循环调用
 */
PUBLIC void processPic24Comm(void);

/* 通用串口相关控制量及MCU串口初始化 */
PUBLIC void initPic24Module(void);

/* 获取需要实际输出数据指针 */
PUBLIC int8u * getPic24OutputIODataPtr(int * len);

/* 更新 PIC24 的输出 IO 数据 (同时更新 A and B) */
PUBLIC int updatePic24Outdata(int8u *outData, int8u outLen);

PUBLIC int8u getPCA9505ErrCnt(int8u node);
PUBLIC BOOL ifPCA9505Err(int8u node);

#endif //_PIC24COMM_H

