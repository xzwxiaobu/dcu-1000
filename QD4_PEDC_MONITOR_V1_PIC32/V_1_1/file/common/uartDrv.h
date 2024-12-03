#ifndef _UARTDRV_H
#define _UARTDRV_H

#include "_cpu.h"

#if (defined(PCB_PIC24))
#include "uart.h"

/* Fcy = 16M, UART initialized to 8bit,no parity and 1 stopbit
 * UARTx BAUD RATE WITH BRGH = 0
 *   UxBRG = ((FCY/Desired Baud Rate)/16) - 1
 * UARTx BAUD RATE WITH BRGH = 1
 *   UxBRG = ((FCY/Desired Baud Rate)/4) - 1  
 * 
 *   注意计算考虑四舍五入，否则可能波特率偏差过大
 * BRGH = 0
 *   设置波特率     设置值     实际波特率               偏差
 *   56000          17        16M/(16*18)=55555.56      -0.79%      OK
 *   115200         9         16M/(16*10)=100000        -13.19%     NG
 *   115200         8         16M/(16*9) =111111.11     -3.55%      NG
 *   115200         7         16M/(16*8) =125000         8.51%      NG
 * BRGH = 1
 *   设置波特率     设置值     实际波特率               偏差
 *   56000          70        16M/(4*71)=56338.03       0.6%
 *   115200         33        16M/(4*34)=117647.06      2.12%
 *   115200         34        16M/(4*35)=114285.71     -0.79%
 */
/* 计算波特率设置值     U2_BRGVAL = ((((FCY * 10) / U2_BAUDRATE )/16 + 5)/10 - 1) */
#define FCY             16000000        /* 16MHz */
#define U2_BRGVAL       34              /* 115200 @BRGH = 1 */

typedef enum
{
    UART_1   = 1,
    UART_2   = 2,
}UART_MODULE;

typedef enum
{
    PRI_   = 0,
}INT_PRIORITY;

#else
#include <plib.h>
#include "peripheral\uart.h"
#endif

#include "sysTimer.h"
#include "uartRxBuff.h"
#include "uartDMA.h"
#include "usrCRC.h"

#define UART_RX_SOFTBUFF            /* 使用软件缓存串口接收数据，降低接收中断占用CPU时间 */
#define UART_TX_BY_DMA              /* 串口通过 DMA 发送数据，避免发送中断占用 CPU时间   */


/* =========================================================
 * 定义 COM 通信端口号
 * 注意: COM 号与 UART 号含义不相同
 *       tEnmComList 中定义的 COM  号是虚拟的编号
 *       UART 号是物理的，是 CPU 实际的串口资源，各个模块自行定义 (tUartComm.uartID)
 */

typedef enum
{
    COM_MMS = 0,        /* 与 MMS 通信              */
    COM_PIC24,          /* 与 PIC24芯片通信         */
    COM_IPS,            /* 与 IBP PSL SIG 板通信    */
    COM_SIGSYS1,        /* 与 信号系统通信          */
    COM_SIGSYS2,        /* 与 信号系统通信          */
    COM_BASSYS ,        /* 与 BAS 系统通信          */
    
    COM_NUM,            /* 自动计算 COM 总数        */
}tEnmComList; 

typedef enum
{
    COM_TX_OK = 0,          /* 启动串口发送数据成功                 */
    COM_TX_STOPPED,         /* 串口已经停止发送                     */
    COM_TX_ERR_PORTNUM,     /* 不支持的COM号  （软件COM 号错误）    */
    COM_TX_ERR_UNINIT,      /* COM号指向的串口未初始化              */
    COM_TX_ERR_BUSY,        /* COM号指向的串口还有未完成的发送任务  */
    COM_TX_ERR_OVERSIZE,    /* 待发送数据的长度超过发送缓冲区大小   */
}tEnmComTxSts; 

typedef enum
{
    COM_INI_OK = 0,         /* 初始化或重置成功                     */
    COM_INI_ERR_PORTNUM,    /* 不支持的COM号    （软件COM 号错误）  */
    COM_INI_ERR_COMID,      /* UART端口号不存在 （硬件COM ID错误）  */
    COM_INI_ERR_OCCUPY,     /* UART端口号已经被占用                 */
    COM_INI_ERR_PARA,       /* 错误的参数                 */
}tEnmComIniSts; 


typedef enum
{
    PROT_UNDEF  = 0,
    PROT_MODBUS    ,        /* 底层协议=MODBUS */
}tEnmUartProtocol;


/* 定义串口收发缓冲区结构体
 */
#pragma pack(2)
typedef struct
{
    int8u * pRxBuf;                     /* 接收数据区指针               */
    volatile int16u rxBufSize;          /* 接收数据区大小               */
    volatile int16u rxCnt;              /* 已接收的数据长度             */
                                        
    int8u * pTxBuf;                     /* 发送数据区指针               */
    volatile int16u txBufSize;          /* 发送数据区大小               */
    volatile int16u txCnt;              /* 已发送数据的长度             */
    volatile int16u txDataLen;          /* 待发送数据的长度，0表示没有  */
}tUartBuf;
#pragma pack()


/* 定义串口功能管理结构体
 */
typedef struct
{
    void (* fpRE_En) (void);            /* 函数指针 RE enable   */
    void (* fpRE_Dis)(void);            /* 函数指针 RE disable  */
    void (* fpDE_En) (void);            /* 函数指针 DE enable   */
    void (* fpDE_Dis)(void);            /* 函数指针 DE disable  */
    
    void (* funRx)  (int8u, int8u);     /* 函数指针 接收数据处理，中断调用 */
    void (* funTx)  (void)  ;           /* 函数指针 发送数据结束后中断调用 */
    
    int32u      usrComNo;               /* 自定义的串口编号 */
    UART_MODULE uartID;                 /* 串口 ID : 4字节 */
    BOOL        RS485_4Wire;            /* true = RS485_4W, else RS485_2W : 4字节 */
    int32u      delayForStopTx;
    
    volatile tSYSTICK lastByteTick;     /* 上一次读到字节的时间点 (ms)      */
    volatile tSYSTICK gotFrameTick;     /* 上一次得到一帧数据(通信成功)的时间点 (ms)  */
    volatile tSYSTICK abortFrameTime;   /* 获得一帧数据若超过此时间则丢弃 (ms)，0 表示无效    */
    volatile tSYSTICK frameOverTime;    /* 设置通信帧间隔时间(ms)，若在此时间内无数据则重新开始搜索帧，0 表示无效  */
    volatile tSYSTICK startSendTime;    /* 记录启动发送的时间(ms)，                                                */
    volatile tSYSTICK sendOverTime;     /* 设置发送超时错时间(ms)，若在此时间内无数据则认为通信错误，  0 表示无效  */
    
    volatile tSYSTICK linkFailTime;     /* 通信连接错误时间设置，超时后认为与对方通信断开   */
    
    tUartBuf    RxTxBuf;
    struct 
    {
        volatile int16u linkState   : 1;    /* 0=与对方通信断开         */
        volatile int16u isSending   : 1;    /* 1=表示正在执行发送       */
        volatile int16u gotFrame    : 1;    /* 1=表示接收到完整的数据帧 */
        volatile int16u errFrame    : 1;    /* 1=表示接收数据帧错误     */
        volatile int16u TxOVTM      : 1;    /* 1=发送通信超时错误       */
        volatile int16u receiving   : 1;    /* 1=正在接收数据           */
        volatile int16u             : 10;
    }uartSts;
    
    /* 串口通信底层封装解析协议 
     * 保留未用
     */
    tEnmUartProtocol uartProtocol;
    
}tUartComm;


/* =========================================================
 * 函数
 */
 
/* 串口重置
 * 输入
 *      ComNo    : COM号
 * 返回
 *      重置结果
 * */
PUBLIC tEnmComIniSts DeInitUartCom(tEnmComList ComNo, tUartComm *pUartCOM);

/* 串口初始化函数
 * 输入
 *   ComNo     : COM 口编号（不是 UART 编号）
 *   pUartCOM  : 指向串口结构体
 *   dataRate  : 串口波特率
 *   priority  : 中断优先级
 * 返回
 *      初始化结果
 */
PUBLIC tEnmComIniSts uartCOM_Init(tEnmComList virCom, UART_MODULE phy_id, tUartComm * pUartCOM,  int32u dataRate, INT_PRIORITY priority);

/* 启动串口发送数据
 * 输入
 *      ComNo    : COM号
 *      sendData : 需要发送的数据
 *      sendLen  : 需要发送的数据的长度
 * 返回
 *      发送状态
 * */
PUBLIC tEnmComTxSts uartStartSend(tEnmComList ComNo, int8u *sendData, int16u sendLen);

/* 停止串口发送数据
 * 输入
 *      ComNo    : COM号
 * 返回
 *      发送状态
 * */
PUBLIC tEnmComTxSts uartStopSend(tEnmComList ComNo);


/* 获取串口发送数据状态
 * 输入
 *      ComNo    : COM号
 * 返回
 *      发送状态
 * */
PUBLIC tEnmComTxSts uartGetSendState(tEnmComList ComNo);

/* 重置串口接收的数据
 */
PUBLIC BOOL uartResetRecv(tEnmComList ComNo);

/* 判断链接状态及发送超时 
 */
PUBLIC BOOL uartChkLinkAndSendSts(tEnmComList ComNo);

/* 检查串口错误并无条件清除，主程序需要定时（如 50ms）调用
 */
PUBLIC int checkAllUartErrorSta(void);

//***********************************************************
#endif  //_UARTDRV_H

