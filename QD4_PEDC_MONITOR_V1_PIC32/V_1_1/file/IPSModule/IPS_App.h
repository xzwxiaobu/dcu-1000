#ifndef _IPSCOMMUNI_H
#define _IPSCOMMUNI_H

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "IPSUart.h"


/* 轮询任务 
 * 对于 RS485-2W，uartDrv 驱动底层自动管理
 * 对于发送超时：
 * 在 115200bps(1+8+1 bits)，按发送100字节，100*(1+8+1)/115200 ~= 8.7ms
 * 回复信息 140Bytes 约需要 13ms
 * 取发送超时 20ms，取单次轮询的时间 IPS_INTURN_TIME = 50ms
 * 
 * 执行在线升级时，不执行轮询
 */
    
#define IPS_INTURN_TIME             50      /* 轮询一个对象的时间限制，必须大于 IPS_TX_OVERTIME */
#define IPS_TX_OVERTIME             30      /* 轮询发送时间限制,ms */
#define IPS_OFFLINE_NUM             5       /* 通信错误达到设定值后， 设置离线标记 */
#define IPS_ERRCNT_MAX              200     /* 通信错误次数最大值 */
#define IPS_LINKFAIL_TIME           500     /* 通信连接正常超时错, ms */

#if (IPS_INTURN_TIME <= IPS_TX_OVERTIME)
#error "IPS_INTURN_TIME must be lager than IPS_TX_OVERTIME"
#endif


//typedef int16u (*createTxPkg)(tPktHeader *pkgHeader, int8u* data, int8u dataLen, BOOL sendNow);
/* 轮询任务结构 */
typedef struct
{
    tUartComm   *pollingCom;            /* 轮询对象用到的串口 */
    int8u       *pollingObjList;        /* 轮询对象列表 */
    
    int8u       pollingObjNum;          /* 轮询对象数量 */
    int8u       pollingTurn;            /* 轮询顺序 */
    
    int8u       procNode;               /* 接收数据信息需要处理的节点 */
    
    tSYSTICK    PollingTurnTime;        /* 轮询间隔时间             */
    tSYSTICK    PollingTxOvTime;        /* 轮询时，发送时间限制     */
    tSYSTICK    pollingStepTime;        /* 轮询时，记录步骤执行时间 */
    
    int8u       ErrMaxNumOffLine;       /* 通信错误达到设定值后， 设置离线标记 */
    int8u       setWaitTXTime;          /* 从空闲切换到发送状态等待时间后再发送 */
    
    tEnmSendPkgStep sendPkgStep;        /* 轮询步骤 */
    tPktHeader      sendPkgHeader;
    int             sendPkgLen;
    int8u           sendPkgBuf[UART_COM_TXBUFSIZE];
    
    cbProcRxPkg     fpCbProcRxPkg;      /* 函数指针，回调处理接收的数据包 */
}tPollingTask;


/* 主程序初始化
 * IPS模块相关控制量及MCU串口初始化 */
PUBLIC void initIPSModule(void);

/* 接收PEDC数据包，进行解析后执行相应的任务并回复
 * 需要在主程序中循环调用
 */
PUBLIC void processIPSComm(void);

////////////////////////////////////////////////////////////////////////////////
/* 通用串口接收字节处理函数 */
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data);

/* 通用串口创建发送数据包 */
PUBLIC tEnmSendPkgStep createUniTxPackage(void *pPollingPtr, int8u* data, int dataLen, BOOL sendNow);


/* Uart 查询 IBP PSL SIG 在线状态 , 返回  TRUE 表示连接正常 */
PUBLIC BOOL IPSObj_chkOnline(int8u id);

/* 串口接收到数据（一个字节）后的处理程序
 *  */
PUBLIC void IPS_UartReceiveData(int8u module_id, int8u data);

/* 开放给 PIC24调用 */
PUBLIC BOOL IPS_ProcRxFrame(tUartComm   *pUartCom, int8u dstNode, BOOL incSeq);

/* 用于定时读继电器寿命信息 */
PUBLIC BOOL ifReadRelayLife(int8u obj);
PUBLIC void clearReadRelayLife(int8u obj);
PUBLIC void * getRelayRecordPtr(int8u obj);

#endif //_IPSCOMMUNI_H

