#ifndef _GLB_UPDATE_H_
#define _GLB_UPDATE_H_

#include "_cpu.h"

//===================================================================
#if (UPDATE_FUN_EN == TRUE)
typedef struct
{
    int8u u8Cmd;                              //命令, 0 -- 什么也不做
    int8u u8Dst;                              //操作对象
}tMMSCmd;

#define MMS_CMD_UPDATE        0xE0          //MMS 发出的升级命令
#define FB_TO_MMS_UPDATE      0xE1          //反馈给 MMS 升级状态

#define MMS_CMD_QUERY         0xE5          //MMS 发出的查询命令
#define FB_TO_MMS_QUERY       0xE6          //查询成功: 反馈给 MMS 信息
#define FB_TO_MMS_QUERY_FAIL  0xE7          //查询失败: 反馈给 MMS 信息


//输出升级状态指示
#define ENABLE_LED_MSG      1           //1 = 允许 LED 指示,  0 = 不允许
#define ENABLE_UART_MSG     0           //1 = 允许 UART 指示, 0 = 不允许

#define LED_PORT            PORTB       //PB.6
#define LED_BIT             (1<<6)      //

#define UPGRADE_UART        UART4       //注意：1.不要随意修改 2.不能打开中断允许！
#define UPGRADE_UART_TX_REG U4TXREG
#define UPGRADE_UART_TX_STA U4STA
#define UPGRADE_UART_BR     56000

//master 发送升级数据给 Slave 以后, 为了保证 Slave 有足够时间写入 FLASH, 延时一定时间后查询 Slave 的情况
//每行32条指令编程或擦除操作由器件自动计时，持续时间的标称值为2 ms
//升级数据的最大长度是 512 bytes(4行), 持续时间为: 4*(2+2) = 16ms
//考虑必要的一些时间消耗, 设定 40ms
//Master 发送查询命令以后, 等待回复的超时错时间有 35ms(glbCan.h 中 MASTER_DFT_OV + ApiCanMst.c 中 MstOverTime)
//因此 Slave 在接收到升级数据后, 在 (40+35)ms 时间内必须完成写入 FLASH 同时应答 Master
#define MST_DLY_QUERY         (SYS_TICK_1MS * 65)

//指定时间内收不到上位机的数据, 停止升级
#define MMS_NODATA_TMR          (SYS_TICK_1MS * 1000 * 6)

#define UPFW_OVER_TIME        (5000)               //ms, 从一次段写入成功开始计时, 此时间目前没有用

//for u8ProFlag
#define fgSlvUpgrading    (1<<0)                   //1：升级中
#define fgSlvRxData       (1<<1)                   //1：收到数据
#define fgSlvProcess      (1<<2)                   //1：有数据需要处理
#define fgSlvUpgradeCmd   (1<<3)                   //1：收到升级命令
#define fgSlvWaitDoorCls  (1<<4)                   //1：等待门关闭



//------------------------------------------------
//功能码:   FUNID_UPDATE_FIRMWARE
//扩展命令
// bit7   表示数据方向, 0=PEDC->DCU, 1=DCU->PEDC
// bit6~0 正常传输下是数据包计数器, 用于检查接收是否丢失数据包, 取值 0~119, Slave接收到0后进行初始化,
//        此协议最大传输数据量 120*7 = 840
//        120~127 保留作为命令, 目前升级程序只使用了 UPFW_CMD_QUERY
#define UPFW_CMD_START    (RSV_CMD_S+0) //启动升级      : 0x55AA+升级文件CS(4)+CS(1), Slave 启动接收升级数据
#define UPFW_CMD_DATA     (RSV_CMD_S+1) //段传输数据    : 0x55AA+段号(2)+段数据长度(2)+CS(1)
#define UPFW_CMD_WRITE    (RSV_CMD_S+2) //写入 Flash    : 0x55AA+段号(2)+CS(1)
#define UPFW_CMD_FINISH   (RSV_CMD_S+3) //升级传输完成  : 0x55AA+CS(1)
#define UPFW_CMD_ABORT    (RSV_CMD_S+4) //升级传输终止  : 0x55AA+CS(1)
#define UPFW_CMD_QUERY    (RSV_CMD_S+5) //查询升级状态  : 0x55AA+CS(1)
                                        //    Slave应答 : 0x5A5A+状态(1)+CS(1)
//------------------------------------------------
//for master
//tUpdateMsg.u8MstState
#define MST_STATE_IDLE              0       //空闲
#define MST_STATE_START             1       //启动升级
#define MST_STATE_TRANS             2       //启动发送数据
#define MST_STATE_QUERY_D           3       //发送命令后延时查询Slave
#define MST_STATE_QUERY             4       //发送查询命令后Slave等待应答
#define MST_STATE_TRANS_FAIL        5       //发送失败
//新增
#define MST_STATE_INIT_FAIL         6       //未初始化回调函数
#define MST_STATE_REPLY_STATE       7       //回复升级状态
#define MST_STATE_REPLY_REQUEST     8       //回复请求升级
#define MST_STATE_WAIT_REQUEST      9       /* 等待从机回复 请求升级 */
#define MST_STATE_WAITDATA          10      /* 等待升级数据包 */
#define MST_STATE_SENDING           0x10    //正在发送数据包

typedef struct
{
    tMMSCmd  MMSCmd;                         //MMS 的命令

    int8u  u8MstState;                        //Master 状态
    int8u  u8SlvState;                        //Slave 升级状态
    int16u  wSlvSection;                       //Slave 应答的段号

    int16u  wTimerAuto;                        //自动延时进行到下一个步骤, 需要考虑Slave的处理时间, 0 表示不自动进行
    int16u  wTickAuto;                         //计时器初始值
    int16u  wTimerOver;                        //设定超时错, 0 表示没有超时错
    int16u  wTickOver;                         //计时器初始值

    int32u dwCS;                              //升级文件Checksum
    int16u  wTotSections;                      //升级文件切割的段总数

    int8u *pData;                             //数据包指针
}tUpdateMsg;

extern tUpdateMsg          UpdateMsg;

#define MMS_OP_COMAND      UpdateMsg.MMSCmd.u8Cmd  //操作命令
#define MMS_OP_DST_ID      UpdateMsg.MMSCmd.u8Dst  //操作对象

#define MMS_UPDATE_STATE() UpdateMsg.u8MstState    //== MST_STATE_IDLE 表示没有升级

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define CHK_TIMER(s,now,ov)   if((ov != 0) && ((int16u)(now - s) > ov))

#define IF_TIMER_AUTO()       CHK_TIMER(UpdateMsg.wTickAuto, wNow, UpdateMsg.wTimerAuto)
#define IF_TIMER_OVER()       CHK_TIMER(UpdateMsg.wTickOver, wNow, UpdateMsg.wTimerOver)

#define SET_TIMER_OVER(a)     UpdateMsg.wTimerOver = (a)
#define SET_TIMER_AUTO(a)     UpdateMsg.wTimerAuto = (a)

#define DCU_ON_LINE_REG       DcuData.MsgBuf.DcuOnLineState     //main.h

//**********************************************************************************************
//                                                                                             *
//                                      变量定义                                               *
//                                                                                             *
//**********************************************************************************************
    #if (CDRV_TYPE == CDRV_PEDC)
    extern tMMSCmd    RxMMScmd;            //如果上位机有升级命令, 将该命令的参数保存到此结构体中
    #endif

    //#define UPFW_CODE_BUF_MAX  (512+32)    //段数据长度最大 512B，信息最大 24B，另外所有数据的校验和 1B
 extern  int8u __attribute__ ((aligned(4))) FileDataBuf[UPFW_CODE_BUF_MAX]; //数据长度最大 512B，信息最大 24B，另外所有数据的校验和 1B
                                                            //首地址必须对齐到 int32u

    extern int8u u8UpdateState;

//**********************************************************************************************
//                                                                                             *
//                                      API 函数定义                                           *
//                                                                                             *
//**********************************************************************************************
//===== 升级处理
extern int8u UpdateProcess(void);
extern void GetAllChkSum(void);
extern int8u UpFwProcessData(int8u * pSrc);

#if (CDRV_TYPE == CDRV_PEDC)
//===== 处理由上位机(MMS)发出的命令
//  参数: pCmd = 数据指针
//  返回: FALSE -- fail
extern int8u MMS_Command(int8u * pCmd, int pkgSize);
#endif //#if (CDRV_TYPE == CDRV_PEDC)

#if (CDRV_TYPE == CDRV_DCU)
extern void SetDoorState(int8u state);
extern void SetDcuState (int8u state);
extern int8u GetUpgradeState(void);
#endif






/* 0 = 不支持在线升级 IBP PSL SIG 板，否则支持 
 */
#define UPDATE_IPS_EN       1       
#define UP_PEDC_MON         200
#define UP_PEDC_PSL         201
#define UP_PEDC_SIG         202
#define UP_PEDC_IBP         203
#define UP_PEDC_PIC24A      204
#define UP_PEDC_PIC24B      205
#define UP_PEDC_SWITCHER    206
#define UP_PEDC_LOG_A_UA1   207
#define UP_PEDC_LOG_A_UB1   208
#define UP_PEDC_LOG_B_UA1   209
#define UP_PEDC_LOG_B_UB1   210

/* 从机ID: 201 ~ 220 */
#define UP_PEDC_SLAVES_S    201
#define UP_PEDC_SLAVES_E    220
            
#define UP_PEDC_DCU_S       ID_DCU_MIN
#define UP_PEDC_DCU_E       ID_DCU_MAX

#define UP_PEDC_MORE_DCUS   0x80    /* 批量升级DCU */

/* 升级对象类型 */
#define UPTYPE_UNKNOWN          0       /* 未定义类型 */
#define UPTYPE_PEDC_HOST        1       /* PEDC主控板: 如有些方案中是 MONITOR 板，有些方案中是 IBP 板 */
#define UPTYPE_PEDC_SLAVE       2       /* PEDC扩展板 */
#define UPTYPE_PEDC_PIC24       3       /* PEDC主控板的PIC24芯片 */
#define UPTYPE_DCU              4       /* 指定DCU */
#define UPTYPE_MORE_DCU         5       /* 批量DCU */

#if (UPDATE_IPS_EN != 0)

/* 串口发送: 升级数据包 */
typedef BOOL (*uartUpdate_UartTxPkg)(int8u *pCmd, int pkgSize);

/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
typedef BOOL (*uartUpdate_UartTxBusy)(void);

/* 串口发送: 查询升级状态 */
typedef BOOL (*uartUpdate_QuerySlave)(int8u id, int16u secNo);

/* 查询指定对象的在线状态 */
typedef BOOL (*uartUpdate_ChkOnline)(int8u id);

/* 查询指定对象的返回的升级状态 */
typedef BOOL (*uartUpdate_GetSlaveReply)(int8u id, int8u *sts, int16u *secNo);


PUBLIC BOOL InitUartUpdateCallBack(uartUpdate_UartTxPkg cbTxPkg, uartUpdate_UartTxBusy cbTxBz, uartUpdate_QuerySlave cbQuerySts, uartUpdate_ChkOnline chkOnline, uartUpdate_GetSlaveReply cbGetReply);

#endif






#endif //#if (UPDATE_FUN_EN == TRUE)




/* ==============================================
 * 批量升级功能
 *  */
BOOL ifBatchUpdateDCUs(void);
BOOL setBatchcastUpdateDCUs(int8u * settings);
int8u* getBatchUpdateDCUsSetting(void);

#endif //_GLB_UPDATE_H_

