#ifndef _MAINIO_H
#define _MAINIO_H

#include "_cpu.h"
#include "MonCommCfg.h"
#include "Update.h"


/* 输入/输出数据最大长度
 *  PIC32-GPIO + 3 * PCA9505-IO = 12 + 3 * 5 = 27 byte
 *  对输入来说，可能还包括安全继电器闪断、通信状态等信息
 *  这里取 32 字节
 */

/* 定义PCB拨码ID对应的 IO口 */
#define READ_ID_ADR0() PORTReadBits(IOPORT_B, BIT_7)
#define READ_ID_ADR1() PORTReadBits(IOPORT_B, BIT_10)
#define READ_ID_ADR2() PORTReadBits(IOPORT_B, BIT_11)
#define READ_ID_ADR3() PORTReadBits(IOPORT_B, BIT_9)


/* 与从机相关的信息 
 */
typedef struct
{
    int8u   slaveLinkOK;            /* 与从机通信状态               */
    int8u   sendSeq;                /* 与从机通信序列号             */
    int8u   ConfigFlag;             /* 从机配置状态有效标志         */
    int8u   VersionFlag;            /* 1 = 从机版本信息有效标志     */
    int8u   commErrCnt;             /* 通信错误计数器               */
    int8u   dataValidCnt;           /* 通信连接成功以后，需要连续接收N次后，接收的输入数据才有效 */
    
    int8u   gotIOData;              /* 1 = 读到从机 IO 数据         */
    
    int8u   slaveReplyUpdate;       /* 接收升级状态标志             */
    int8u   slaveUpdateSts;         /* 升级状态                     */
    int16u  slaveUpdateSec;         /* 升级段序号                   */
    
    /* 从机 版本信息字符串 */
    char    slaveDevInfo[BYTES_INFO];
    
    tSlaveStsFlags slaveSts;
    
    /* 接收从机输入数据
     */
    /* 从机 IO输入数据 */
    //int8u   RdDataBuf[MAX_IODATA_SIZE];
    int     RdDataLen;
    tCmdReadIO_s slaveInput;
    
    /* 发送从机输出数据 */
    //int8u   WrDataBuf[MAX_IODATA_SIZE];
    int     WrDataLen;
    tCmdWriteIO_m   slaveOutput;

    /* 写从机的 IO配置 
     * 暂时不支持配置从机
     * tSlaveIOScanCfg slaveIOCfg;
     */
}tSlaveInfo;


/* =============================================================================
 * 调用函数接口
 * =============================================================================
 */

/* 返回PCB拨码ID */
PUBLIC int8u getPedcID(void);

/* 读 IO 信息获取PEDC拨码 */
PUBLIC void scanPEDC_ID(void);

/* 供外部获取从机相关信息 */
PUBLIC tSlaveInfo* getSlaveInfoPtr(tEnmCommObj obj);

/* 获取通信连接状态 
 * FALSE 表示连接已断开，TRUE 表示连接正常
 */
PUBLIC BOOL getObjCommLinkSts(tEnmCommObj obj);

/* 主程序定时汇总读入的IO输入数据，包括 MONITOR IBP PSL SIG PIC24A PIC24B  */
PUBLIC void mainIOProcess(void);

/* 收集 DCU 状态 AlarmParaTxToAvr */
PUBLIC void AlarmParaTxToAvr(void);

/* 测试 MONTIOR 输出 IO 的数据缓存 */
PUBLIC int8u *getPic24TestOutPtr(int * len);

PUBLIC BOOL readSlaveVerInfo(tEnmCommObj obj, char * pInfo, int * infoLen);

PUBLIC void resetSlaveInfo(void);

#endif  //_MAINIO_H


