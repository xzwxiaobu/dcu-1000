/*******************************************************************************
 * PIC24在线升级程序模块
 * -> upPIC24_InitStart     : 启动PIC24 升级
 * -> chkIsUpdatingPic24    : 判断是否 PIC24 处于升级状态
 * 
 * -> Pic24UpdateProcess    : PIC24 处于升级状态时，主程序调用升级处理函数     
 * -> upPIC24_ProcP24Reply  : 处理接收的数据帧，主程序接收PIC24回复数据后调用  
 * 
 * -> upPIC24_SendMsgToMMS  : MMS   通信模块提供实现，用于 发送信息给MMS
 * -> upPIC24_SendToPic24   : PIC24 通信模块提供实现，用于 发送信息给PIC24
 * -> upPIC24_ResetChip     : PIC24 通信模块提供实现，通知应用层即将重启PIC24
 *  
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"

#include "IPSUart.h"
#include "sysTimer.h"

#include "P24UpdateDrv.h"
#include "P24UpdateIF.h"

#undef  PRIVATE
#define PRIVATE

/* =============================================================================
 * 常量定义 */
#if (UART_COM_RXBUFSIZE < MAX_PACKET_SIZE)
#error "UART_COM_RXBUFSIZE must be larger than MAX_PACKET_SIZEQ"
#endif

/* 跳开烧录空白的 FLASH
 * 测试发现升级不成功（PIC24 回复校验错误），暂不启用此定义 */
/* #define SKIP_BLANK_FLASH */

/* 升级步骤 - 细分步骤 for updateStep
 *  */
#define P24_STEP_INIT           0
#define P24_STEP_SETOBJ         10
#define P24_STEP_TX2P24         20
#define P24_STEP_WAITRX         30
#define P24_STEP_REPLY_OK       100
#define P24_STEP_WAIT_REPLYMMS  101

/* 升级控制
 *  */
#define UPCMD_REPEAT_INTERVAL   200     /* 重复某个命令的时间间隔 */
#define UPCMD_REPEAT_TIMES      3       /* 重复某个命令的最大次数 */
#define UPCMD_TRY_VERIFY_TIMES  3       /* 允许校验错误的最大次数 */

#define BL_ERR_UNDEFCMD         0x00    /* 未定义命令 */
#define BL_ERR_ENTRANCE         0x01    /* PIC24入口错误（芯片复位无法启动 Bootloader） */
#define BL_ERR_CODE             0x02    /* Bootloader 代码错误（不存在） */
#define BL_ERR_KEYWORDS         0x03    /* APP程序关键字不匹配错误 */
#define BL_VERIFY_OK            0xFF    /* 校验通过，启动Bootloader */



/* 升级状态持续时间  ( 秒 )
 * 实测：升级一个 芯片大约需要 20s(57600bps,  UPDATE_INTERVAL_MS = 10ms)
 *  */
#define UPDATE_MAX_TIME_SEC     40     

/* FLASH 地址（字地址）转换成 BIN 文件地址（字节地址）
 *  */
#define FLASH_TO_BIN_ADDR(adr)  ((int8u *)(P24_CodeBin + (adr << 1)))

/* =============================================================================
 * 
 * PIC24 的代码（二进制文件）
 * 
 * 由 Bootloader 和 应用层代码拼接
 * 先将 Bootloader 和 app 的 HEX 文件转换成 BIN 文件
 * 然后截取 
 *   Bootloader 的前 8 个字节 +
 *   app        的中断向量    +
 *   Bootloader 的代码        +
 *   app        的代码
 * 合并后转换成 "p24BL_App.c"
 *  */
#pragma pack(4)
PRIVATE const unsigned char P24_CodeBin[] = 
{
    #include "p24BL_App.c"
};

/* =============================================================================
 * 本地变量 */

/* 升级对象，ONLY COMMOBJ_IORW_A or COMMOBJ_IORW_B */
PRIVATE tEnmCommObj updatePic24Obj;   

/* 升级过程控制 */
PRIVATE int8u           updateInitiator;        /* 升级发起者               */
PRIVATE int8u           updateMode;             /* 升级模式  */
    
PRIVATE enmUpPic24Step  updatePic24Sts;         /* 升级状态                 */
PRIVATE int8u           updateStep;             /* 每个状态下的过程步骤     */
PRIVATE int8u           updateRepeat;           /* 升级过程重复次数         */
PRIVATE int8u           verifyErrCnt;           /* 升级过程校验错误计数器   */
PRIVATE tSYSTICK        updateStepTick;         /* 升级过程步骤计时用       */
    
PRIVATE tSYSTICK        updateP24_StartTick;    /* 进入升级状态的起始时间   */
    
PRIVATE tDWORD          flashAddr;              /* 读写 FLASH 的地址，程序存储空间按字寻址 */

PRIVATE tFlashInfo hexInfo;                     /* 待升级代码的HEX信息 */

/* 收到自 PIC24 的信息内容 
 * pic24ReplyBuf 中的内容不包含数据帧的头信息
 */
PRIVATE int16u      pic24ReplyLen;
PRIVATE int8u       pic24ReplyBuf[MAX_PACKET_SIZE];

/* 发送到 PIC24 的信息内容 */
PRIVATE int16u      sendPic24Len;
PRIVATE int8u   __attribute__((aligned(4))) sendPic24Buf[MAX_PACKET_SIZE];

PRIVATE int8u       pic24BLRunning[2];      /* 0=PIC24运行应用层，否则表示PIC24运行Bootloader */

/* 发送到 MMS 的信息内容 */
PRIVATE int16u      sendMMSLen;
PRIVATE tPktHeader  sendPkgHeader;
PRIVATE int8u   __attribute__((aligned(4))) sendMMSBuf[MAX_PACKET_SIZE];

/* 串口功能接口 */
PRIVATE tUartComm *pP24UpComm;


/* =============================================================================
 * 在线升级 PIC24 串口通信接口函数
 *  */
extern BOOL upPIC24_SendMsgToMMS(int8u msgType, int8u srcNode, int8u * data, int16u dataLen);
extern BOOL upPIC24_SendToPic24(int8u msgType, int8u txDstNode, int8u * txData, int16u txDataLen);
extern BOOL upPIC24_ResetChip(tEnmCommObj updatePic24Obj);

/* 用 weak 定义，可以避免外部没有提供函数实现时编译错误 */
//BOOL __attribute__((weak)) upPIC24_ResetChip(tEnmCommObj updatePic24Obj);

/* =============================================================================
 * 在线升级 PIC24 应用函数
 *  */

/* 
 * 是否处于升级 PIC24 的状态
 * 返回 TRUE 表示升级中
 */
PUBLIC BOOL chkIsUpdatingPic24(void)
{
    BOOL ret = FALSE;
    if(updatePic24Sts != UP_PIC24_NULL)
    {
        ret = TRUE;
    }
    
    return ret;
}


/* =============================================================================
 * 发送数据给 PIC24 
 * 输入
 *  msgType         : 消息类型
 *  updatePic24Obj  : 本地全局变量，对象，
 *  sendPic24Buf    : 本地全局变量，待发送数据 
 *  sendPic24Len    : 本地全局变量，待发送数据长度
 * 返回发送状态    
 *  TRUE 等于成功
 */
PRIVATE BOOL sendToPic24Package(int8u msgType)
{
    BOOL   ret = upPIC24_SendToPic24(msgType, updatePic24Obj, sendPic24Buf, sendPic24Len);
    return ret;
}

/* =============================================================================
 * 发送数据给 MMS 
 * 输入
 *  OBJ_CMD_BL_CTRL : 常量
 *  COMMOBJ_MONITOR : 常量
 *  sendMMSBuf      : 本地全局变量，待发送数据 
 *  sendMMSLen      : 本地全局变量，待发送数据长度
 * 返回发送状态    
 *  TRUE 等于成功
 */
PRIVATE BOOL sendToMMSPackage(void)
{
    BOOL   ret = FALSE;
    if(sendMMSLen > 0)
    {
        ret = upPIC24_SendMsgToMMS(OBJ_CMD_BL_CTRL, COMMOBJ_MONITOR, sendMMSBuf, sendMMSLen);
        sendMMSLen = 0;
    }
    
    return ret;
}

/* =============================================================================
 * 升级 PIC24，处理 PIC24 回复的数据帧
 * 参数 
 *  msgType         : 消息类型
 *  rxData          : 消息数据
 *  rxDataLen       : 消息数据长度
 * 返回
 *  TRUE 接收到指定对象的数据帧
 */
PUBLIC BOOL upPIC24_ProcP24Reply(int8u msgType, int8u *rxData, int16u rxDataLen)
{
    /* 返回结果 */
    BOOL rxMsgSts = FALSE;
    
    if(rxDataLen > 0)
    {
        /* 复制PIC24回复的信息内容 */
        pic24ReplyLen = rxDataLen;
        memcpy(pic24ReplyBuf, rxData, rxDataLen);

        /* 从机回复的命令字
         *  */
        switch((tEnmCommObjCmd)(msgType))
        {     
            /* ====================================
             * Monitor-PIC32 与 PIC24应用层的通信 
             */
            case OBJ_CMD_BL_CTRL:
                switch(pic24ReplyBuf[0])
                {
                    /* PIC24 回复‘读Checksum信息’ */
                    case PIC24APP_CMD_GET_CS:
                        if((updatePic24Sts == UP_PIC24_INIT) && (updateStep == P24_STEP_WAITRX))
                        {
                            updateStep = P24_STEP_REPLY_OK;
                            rxMsgSts   = TRUE;
                        }
                        break;

                    /* 此处收到，表示无法启动 Bootloader */
                    case PIC24APP_CMD_ENTER_BL:
                        if((updatePic24Sts == UP_PIC24_START_BL) && (updateStep == P24_STEP_WAITRX))
                        {
                            if((pic24ReplyBuf[1] == 0x55) && (pic24ReplyBuf[2] == 0xAA))
                            {
                                /* pic24ReplyBuf[3] 中是回复信息 */
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                        break;
                        
                        
                    default:
                        break;
                }
                break;

            /* ====================================
             * Monitor-PIC32 与 PIC24 Bootloader的通信 
             */
            case OBJ_CMD_BOOTLOADER:
                switch(pic24ReplyBuf[0])
                {
                    /* Bootloader 已经启动
                     * 
                     * PIC24APP_CMD_ENTER_BL 是PIC32发给PIC24应用层的命令，
                     * PIC24 复位启动进入Bootloader以后回复
                     */
                    case PIC24APP_CMD_ENTER_BL:
                        if((updatePic24Sts == UP_PIC24_START_BL) && (updateStep == P24_STEP_WAITRX))
                        {
                            if((pic24ReplyBuf[1] == 0x55) && (pic24ReplyBuf[2] == 0xAA) && (pic24ReplyBuf[3] == BL_VERIFY_OK))
                            {
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                    break;

                    /* PIC24 Bootloader 回复擦除 FLASH 的结果
                     * 与 发送的擦除命令及参数相同( Blocks, addr全部是0 )
                     */
                    case ER_FLASH:
                        if(updatePic24Sts == UP_PIC24_ERASE)
                        {
                            if(pic24ReplyBuf[1] == ER_FLASH_BLOCKS)
                            {
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                        break;

                    /* PIC24 Bootloader 回复写入 FLASH 的结果
                     */
                    case WT_FLASH:
                        if(updatePic24Sts == UP_PIC24_WRITE)
                        {
                            rxMsgSts   = TRUE;
                            updateStep = P24_STEP_REPLY_OK;
                        }
                        break;

                    /* PIC24 Bootloader 回复读取 FLASH 的结果
                     */
                    case RD_FLASH:
                        if(updatePic24Sts == UP_PIC24_VERIFY_WR)
                        {
                            rxMsgSts   = TRUE;
                            updateStep = P24_STEP_REPLY_OK;
                        }
                        break;


                    case RD_VER:                /* PIC24 Bootloader 回复读 版本信息 的结果 */
                    case VERIFY_OK:             /* PIC24 Bootloader 回复  校验OK */
                    case (VERIFY_OK + 0x80):    /* PIC24 Bootloader 回复  校验OK */
                        rxMsgSts   = TRUE;
                        updateStep = P24_STEP_REPLY_OK;
                        break;

                }
                break;
        }
    }
}

/* =============================================================================
 * PIC32 与 PIC24 应用层通信协议程序，
 *  Initiator : 是升级发起者，调用此函数的源目标 （如， MMS 启动 PIC24升级 ）
 *  upMode    : 升级模式
 *  info      : 待升级代码的HEX信息，暂时保留未用
 *   初始化启动查询过程
 * 返回 TRUE  表示成功启动
 *      FALSE 表示已经启动或者启动失败
 */
PUBLIC BOOL upPIC24_InitStart(int8u Initiator, int8u upMode, tFlashInfo *info)
{
    BOOL ret = FALSE;
    
    if((upMode != UPMODE_AUTO) && (upMode != UPMODE_FORCE) && (upMode != UPMODE_BYCMD))
    {
        /* 不支持的升级模式 */
        sendMMSBuf[0] = updatePic24Obj;                 /* 对象 */
        sendMMSBuf[1] = TOMMS_PIC24_ERRORMODE;          /* 升级模式错误 */
        sendMMSLen    = 2;
    }
    else
    {
        if(updatePic24Sts == UP_PIC24_NULL)
        {
            int32u *pBin = (int32u *)P24_CodeBin;
            
            if(upMode == UPMODE_BYCMD)
            {
                hexInfo = *info;
            }

            /* FLASH 地址0，是一条跳转指令，跳到 Bootloader 的执行代码
             * FLASH 地址1，指向芯片复位后的第一条指令地址
             * 检查 BIN 文件大小（完整性）
             *  */
            if((sizeof(P24_CodeBin) == (P24_FLASH_SIZE_WORD <<1)) && (pBin[0] == 0x00040400L) && (pBin[1] == 0x00000000L))
            {
                ret = TRUE;
                updateInitiator = Initiator;
                updateMode      = upMode;

                updateStep          = P24_STEP_INIT;
                updatePic24Sts      = UP_PIC24_INIT;
                updatePic24Obj      = COMMOBJ_IORW_A;
                updateP24_StartTick = getSysTick();
            }
            else
            {
                sendMMSBuf[0]   = updatePic24Obj;               /* 对象 */
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_BINFILE;
                sendMMSLen      = 2;
            }
        }
        else
        {
            if(updatePic24Sts >= UP_PIC24_UPDATE_INI)
            {
                sendMMSBuf[0] = updatePic24Obj;                 /* 对象 */
                sendMMSBuf[1] = TOMMS_PIC24_UPDATING;           /* 已经处于升级中 */
                sendMMSBuf[2] = flashAddr.v[0];
                sendMMSBuf[3] = flashAddr.v[1];
                sendMMSBuf[4] = flashAddr.v[2];
                sendMMSBuf[5] = (int8u)(P24_FLASH_SIZE_WORD >> 0);
                sendMMSBuf[6] = (int8u)(P24_FLASH_SIZE_WORD >> 8);
                sendMMSBuf[7] = (int8u)(P24_FLASH_SIZE_WORD >> 16);
                sendMMSLen    = 8;
            }
            else
            {
                /* 在 upPIC24_App_QueryInfo() 中回复 TOMMS_PIC24_INIT
                 */
            }
        }
    }
    
    /* 
     * 回复 MMS
     */
    sendToMMSPackage();
    
    return ret;
}

/* =============================================================================
 * PIC32 与 PIC24 应用层通信协议程序，
 *   查询过程
 *   PIC32 发送  : 查询 PIC24 APP信息‘读Checksum信息’
 *                 校验信息如需要升级则启动下一步，否则退出
 *   PIC32 发送  : 启动 Bootloader
 */
PRIVATE void upPIC24_App_QueryInfo(void)
{
    tSYSTICK    tickNow;
    tickNow = getSysTick();

    switch(updateStep)
    {
        /* 初始化 */
        case P24_STEP_INIT:
            /* 
             * 回复 MMS: 
             * 升级初始化过程中，无扩展信息（或无意义）
             */
            sendMMSBuf[0]   = updatePic24Obj;
            sendMMSBuf[1]   = TOMMS_PIC24_INIT;
            sendMMSLen      = 2;

            /* 记录开始的时间点 */
            updateP24_StartTick = getSysTick();
            updateRepeat        = 0;
            updateStep          = P24_STEP_SETOBJ;

        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;

        /* 读 PIC24 程序代码信息，发给 PIC24 命令
         * 10 02 00 C8 CC  F1  00 01  01  10 03
         */
        case P24_STEP_TX2P24:
            sendPic24Buf[0] = PIC24APP_CMD_GET_CS;
            sendPic24Len    = 1;
            
            if(sendToPic24Package(OBJ_CMD_BL_CTRL) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;

        /* 等待 PIC24 回复：读 PIC24 程序代码信息 */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* 超时未回复，重复发送 '读 PIC24 程序代码信息'
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * 这里不回复 MMS (初始化升级失败)
                     * 可能 PIC24 只有 Bootloader 在运行，不响应 PIC24APP_CMD_GET_CS
                     */
                    #if 0
                    sendMMSBuf[0]   = updatePic24Obj;           /* 对象 */
                    sendMMSBuf[1]   = TOMMS_PIC24_NOAPP;        /* 失败代码 */
                    sendMMSBuf[1]   = TOMMS_PIC24_FAIL_INIT;    /* 失败代码 */
                    sendMMSLen      = 2;
                    updatePic24Sts = UP_PIC24_FINISHED;         /* 结束 */
                    
                    #else
                    updateStep      = P24_STEP_INIT;
                    updatePic24Sts  = UP_PIC24_START_BL;        /* 尝试启动 Bootloader */
                    
                    #endif
                }
            }
            break;

        /* 收到PIC24APP回复版本信息 "PIC24APP_CMD_GET_CS" 以后，
         * upPIC24_ProcP24Reply() 处理并赋值 updateStep  = P24_STEP_REPLY_OK
         *  */
        case P24_STEP_REPLY_OK:
            {
                /* PIC24回复的校验信息 */
                tFlashInfo rxInfo;
                memcpy((int8u*)&rxInfo, (int8u*)(pic24ReplyBuf + 1), sizeof(rxInfo));

                
                BOOL runUpdate = FALSE;
                if(updateMode == UPMODE_AUTO)
                {
                    /* PIC32 内置PIC24 BIN 的校验信息 */
                    getBinCodeInfo(&hexInfo);
                    
                    if(memcmp(&rxInfo, &hexInfo, sizeof(tFlashInfo)) == 0)
                    {
                        /* 版本信息完全相同，是最新的版本 */
                        sendMMSBuf[1]   = TOMMS_PIC24_UPDATED;
                        sendMMSBuf[2]   = UP_PIC24_FINISHED;
                    }
                    else
                    {
                        /* 信息不相同
                         * 判断：若应用层校验和不一致，必须启动升级 */
                        if(rxInfo.onlyAppCS == hexInfo.onlyAppCS)
                        {
                            /* APP 校验和相同，回复 MMS Bootloader信息 
                             * 此时 Bootloader 不相同
                             */
                            sendMMSBuf[1]   = TOMMS_PIC24_UPDATED_APP;
                            sendMMSBuf[2]   = UP_PIC24_FINISHED;
                        }
                        else
                        {
                            runUpdate = TRUE;
                            
                            /* 进入下一个过程：启动 PIC24 Bootloader */
                            updateStep      = P24_STEP_INIT;
                            updatePic24Sts  = UP_PIC24_START_BL;

                            /* 回复MMS 升级初始化成功 */
                            sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;
                            sendMMSBuf[2]   = UP_PIC24_INIT;
                        }
                    }
                }
                else if(updateMode == UPMODE_FORCE)
                {
                    runUpdate = TRUE;
                }
                else if(updateMode == UPMODE_BYCMD)
                {
                    /* reserved for future */
                }
                
                /* 判断是否相同 
                 * 一旦 PIC24 信息与 PIC32内置的PIC24-HEX 信息不符，就需要启动升级
                 */
                if(runUpdate == FALSE)
                {
                    updatePic24Sts  = UP_PIC24_FINISHED;        /* 结束 */
                }
                else
                {
                    /* 进入下一个过程：启动 PIC24 Bootloader */
                    updateStep      = P24_STEP_INIT;
                    updatePic24Sts  = UP_PIC24_START_BL;

                    /* 回复MMS 升级初始化成功 */
                    sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;
                    sendMMSBuf[2]   = UP_PIC24_INIT;
                }
                
                sendMMSBuf[0]   = updatePic24Obj;           /* 对象 */
                memcpy(&sendMMSBuf[3], (int8u*)&rxInfo, sizeof(rxInfo));
                sendMMSLen      = 3 + sizeof(rxInfo);
            }
            break;
    }
    
    
    /* 
     * 回复 MMS
     */
    sendToMMSPackage();
}


/* =============================================================================
 * 升级 PIC24 -- 
 * PIC32 与 PIC24 应用层通信协议程序，
 *   PIC32 发送  : 启动 PIC24 Bootloader
 */
PRIVATE void upPIC24_App_StartBL(void)
{
    tSYSTICK    tickNow;

    tickNow = getSysTick();

    sendPic24Len = 0;
    sendMMSLen   = 0;
    switch(updateStep)
    {
        /* 初始化 */
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            break;

        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;
        
        /* 发送命令 PIC24-APP 启动Bootloader, eg.
         * 10 02 00 C8 CC F1 00 04 50 55 AA FF 10 03
         */
        case P24_STEP_TX2P24:
            sendPic24Buf[0] = PIC24APP_CMD_ENTER_BL;
            sendPic24Buf[1] = 0x55;
            sendPic24Buf[2] = 0xAA;
            sendPic24Buf[3] = 0xFF;
            sendPic24Len    = 4;
            
            #if (APP_KEYWORDS_LEN > 0)
            /* 要求做关键字匹配 
             * 此关键字是 PIC24 APP 的项目信息，由宏定义 PROJECT_INFOS 明确的
             * 如 "PEDC-PIC24: "
             * 注意 ： FLASH 存储的数据不是连续的，不能直接 copy，如
             * memcpy(&sendPic24Buf[4], FLASH_TO_BIN_ADDR(APP_PRJINFO_ADDR), APP_KEYWORDS_LEN);
             */
            int8u *pInfo;
            int8u *pChk;
            int8u chkLen = APP_KEYWORDS_LEN;
            
            pChk  = &sendPic24Buf[4];
            pInfo = FLASH_TO_BIN_ADDR(APP_PRJINFO_ADDR);
            for(chkLen=0; chkLen<APP_KEYWORDS_LEN; )
            {
                *pChk++ = *pInfo ++;    /*      */
                *pChk++ = *pInfo ++;
                pInfo  += 2;            /* 必须的 */
                chkLen += 2;
            }
            sendPic24Len += APP_KEYWORDS_LEN;
            #endif
            
            if(sendToPic24Package(OBJ_CMD_BL_CTRL) == TRUE)
            {
                updateStepTick = tickNow;
                updateStep = P24_STEP_WAITRX;
            }
            break;
        
        /* 等待 PIC24 回复：启动Bootloader 
         * PIC24-APP 收到“启动Bootloader”后，不回复，延时复位进入 Bootloader 模式
         * PIC Bootloader 模式下，对“启动Bootloader”进行响应，
         * 所以至少需要有2次“启动Bootloader”的命令，1次是应用层处理，1次是Bootloader处理
         * 考虑到芯片复位并初始化的时间，发送时间间隔要大于正常交互发送时间间隔
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= 1000)
            {
                /* 超时未回复，重复发送 '启动Bootloader'
                 */
                updateRepeat++;
                if(updateRepeat < 5)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * 需要回复 MMS，启动Bootloader失败
                     */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_BL_OVTM;       /* 失败代码 */
                    sendMMSLen = 2;

                    updatePic24Sts = UP_PIC24_FINISHED; /* 结束 */
                }
            }
            break;
            
        /* 收到 PIC24 回复
         * 在 upPIC24_ProcP24Reply() 处理"PIC24APP_CMD_ENTER_BL"，并赋值 updateStep  = P24_STEP_REPLY_OK
         *  */
        case P24_STEP_REPLY_OK:
            if((pic24ReplyBuf[3] != BL_VERIFY_OK))
            {
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_BL_CODE;     /* 出错了   */
                sendMMSBuf[2]   = pic24ReplyBuf[3];             /* 错误代码  */
                sendMMSLen      = 3;
                updatePic24Sts = UP_PIC24_FINISHED;             /* 结束 */
            }
            else// if(pic24ReplyBuf[3] == BL_VERIFY_OK)
            {
                /* 收到PIC24 Bootloader回复，表示已经启动 Bootloader */
                sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;          /* 已经启动 Bootloader */
                sendMMSBuf[2]   = UP_PIC24_START_BL;
                sendMMSLen      = 3;

                /* 进入下一个过程： 初始化升级 FLASH */
                updatePic24Sts = UP_PIC24_UPDATE_INI;
            }
            break;
    }
    
    /* 
     * 回复 MMS
     */
    sendMMSBuf[0]   = updatePic24Obj;               /* 对象 */
    sendToMMSPackage();
}

/* =============================================================================
 * 升级 PIC24 -- 
 * PIC32 与 PIC24 Bootloader 通信协议程序，
 *   PIC32 发送  : 擦除 Flash
 * 一次擦除 中断向量区 及 整个应用区 FLASH
 * RESET地址、RESET启动向量、Bootloader 程序被保护
 */
PRIVATE void upPIC24_BL_EraseFlash(void)
{
    tSYSTICK    tickNow;

    tickNow = getSysTick();

    sendPic24Len = 0;
    switch(updateStep)
    {
        /* 初始化 */
        case P24_STEP_INIT:
            flashAddr.Val   = 0;
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            break;

        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;
        
        /* 发送命令给 PIC24 Bootloader : 擦除 FLASH 
         * eg. 10 02 00 C8 CC F0 00 05 03 15 00 00 00 10 03 E6
         *  */
        case P24_STEP_TX2P24:
            /* cmd, Len, addrL, addrM, addH */
            sendPic24Buf[0] = ER_FLASH;         /* erase 命令 */
            sendPic24Buf[1] = ER_FLASH_BLOCKS;  /* 待擦除的 FLASH 页数量（Block块）*/
            sendPic24Buf[2] = 0x00;             /* 待擦除的起始页地址，3字节 */
            sendPic24Buf[3] = 0x00;
            sendPic24Buf[4] = 0x00;
            sendPic24Len    = BL_HEADER_LEN;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* 等待 PIC24 Bootloader 回复擦除结果
         * 根据官方资料：
         *  Chip Erase Time         : min = 400ms
         *  Page Erase Time         : min = 40ms
         *  Row Programming Time    : min = 2ms
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= 1000)    /* 1000ms */
            {
                /* 超时未回复，重复发送 '擦除 FLASH'
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * 需要回复 MMS，擦除 Bootloader 超时
                     */
                    sendMMSBuf[0] = updatePic24Obj;                 /* 对象 */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;      /* 失败代码 */
                    sendMMSBuf[2] = ER_FLASH;                       /* 失败信息 */
                    sendMMSBuf[3] = 0x00;
                    sendMMSBuf[4] = 0x00;
                    sendMMSBuf[5] = 0x00;
                    sendMMSLen = 6;
                    updatePic24Sts = UP_PIC24_FINISHED;             /* 结束 */
                }
            }
            break;
            
        case P24_STEP_REPLY_OK:
            updateStep      = P24_STEP_INIT;
            updatePic24Sts  = UP_PIC24_WRITE;
            break;
    }
    
    /* 
     * 回复 MMS
     */
    sendToMMSPackage();
}

/* =============================================================================
 * 升级 PIC24 -- 
 * PIC32 与 PIC24 Bootloader 通信协议程序，
 *   PIC32 发送  : 写FLASH
 */
PRIVATE void upPIC24_BL_WriteFlash(void)
{
    int i;
    int8u * pReadBin;
    tSYSTICK    tickNow;

    tickNow = getSysTick();
    
    switch(updateStep)
    {
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            
        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
        
        /* 将程序 bin 数据，发送给 PIC24 Bootloader */
        case P24_STEP_TX2P24:
            /* cmd, len, addrL, addrM, addrH */
            sendPic24Buf[0] = WT_FLASH;
            sendPic24Buf[1] = P24_WRITE_INST_ROWS;
            sendPic24Buf[2] = flashAddr.v[0];
            sendPic24Buf[3] = flashAddr.v[1];
            sendPic24Buf[4] = flashAddr.v[2];
            
            /* 读 FLASH 数据 
             * 注意 FLASH 地址是字地址
             */
            pReadBin = FLASH_TO_BIN_ADDR(flashAddr.Val);
            memcpy(sendPic24Buf + BL_HEADER_LEN, pReadBin, P24_BYTES_PER_ROW);
            sendPic24Len = BL_HEADER_LEN + P24_BYTES_PER_ROW;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* 等待 PIC24 Bootloader 回复写入结果
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* 超时未回复，重复发送写入
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * 需要回复 MMS: 写入超时
                     */
                    sendMMSBuf[0] = updatePic24Obj;                /* 对象 */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;     /* 失败代码 */
                    sendMMSBuf[2] = WT_FLASH;                      /* 失败信息 */
                    sendMMSBuf[3] = flashAddr.v[0];
                    sendMMSBuf[4] = flashAddr.v[1];
                    sendMMSBuf[5] = flashAddr.v[2];
                    sendMMSLen = 6;
                    updatePic24Sts = UP_PIC24_FINISHED; /* 结束 */
                }
            }
            break;
            
        case P24_STEP_REPLY_OK:
            updateStep      = P24_STEP_INIT;
            updatePic24Sts  = UP_PIC24_VERIFY_WR;
            break;
    }
    
    
    /* 
     * 回复 MMS
     */
    sendToMMSPackage();
}

/* =============================================================================
 * 升级 PIC24 -- 
 * PIC32 与 PIC24 Bootloader 通信协议程序，
 *   PIC32 发送  : 读 FLASH 校验 写结果是否正确
 */
PRIVATE  void upPIC24_BL_VerifyProg(void)
{
    BOOL verifyOK;
    int8u * pReadBin;
    int8u * ptr;
    int i;

    tSYSTICK    tickNow;
    tickNow = getSysTick();
    
    switch(updateStep)
    {
        case P24_STEP_INIT:
            verifyErrCnt    = 0;
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            
        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep      = P24_STEP_TX2P24;
        
        /* 发送命令给 PIC24 App or Bootloader
         *  */
        case P24_STEP_TX2P24:
            /* 读 FLASH 的地址，与写入地址相同 
             * cmd, len, addrL, addrM, addrH
             */
            sendPic24Buf[0] = RD_FLASH;
            sendPic24Buf[1] = P24_INST_PER_ROW;
            sendPic24Buf[2] = flashAddr.v[0];
            sendPic24Buf[3] = flashAddr.v[1];
            sendPic24Buf[4] = flashAddr.v[2];
            sendPic24Len    = BL_HEADER_LEN;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* 等待 PIC24 App or Bootloader 回复
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* PIC24 超时未回复，重复执行发送命令给 PIC24
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 校验失败：回读 PIC24 超时错
                     * 回复 MMS
                     */
                    sendMMSBuf[0] = updatePic24Obj;                     /* 对象 */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;          /* 失败代码 */
                    sendMMSBuf[2] = RD_FLASH;                           /* 失败信息 */
                    sendMMSBuf[3] = flashAddr.v[0];
                    sendMMSBuf[4] = flashAddr.v[1];
                    sendMMSBuf[5] = flashAddr.v[2];
                    sendMMSLen    = 6;
                    updatePic24Sts = UP_PIC24_FINISHED; /* 结束 */
                }
            }
            break;
            
        /* 收到回复的 FLASH 数据，与 写入的数据 对比校验 
         */
        case P24_STEP_REPLY_OK:
            {
                /* 默认校验失败，需要重新发送命令给 PIC24 */
                verifyOK    = FALSE;            
                updateStep  = P24_STEP_TX2P24;

                /* 校验接收信息 */
                if((pic24ReplyBuf[1] == P24_INST_PER_ROW) && 
                   (pic24ReplyBuf[2] == flashAddr.v[0]) && 
                   (pic24ReplyBuf[3] == flashAddr.v[1]) && 
                   (pic24ReplyBuf[4] == flashAddr.v[2]))
                {
                    /* 进一步校验 */
                    pReadBin = FLASH_TO_BIN_ADDR(flashAddr.Val);
                    ptr      = (int8u *)(pic24ReplyBuf + BL_HEADER_LEN);

                    /* 比较发送数据和回读的数据 */
                    if(memcmp(pReadBin, ptr, P24_BYTES_PER_ROW) == 0)
                    {
                        /* 校验OK */
                        verifyOK  = TRUE;
                    }
                }

                if(verifyOK)
                {
                    /* 校验OK */
                    verifyErrCnt    = 0;
                    
                    #ifdef SKIP_BLANK_FLASH
                    BOOL continueWr = FALSE;
                    
                    while(1)
                    {
                        ClearWDT();
                    #endif
                        
                        /* 准备写入下一行指令
                         * 判断是否已经完成所有数据写入 FLASH
                         * 需要跳开 Bootloader 的区域
                         */
                        flashAddr.Val   += P24_BYTES_PER_ROW/2;             /* 注意读写 FLASH 是字地址   */
                        if((flashAddr.Val >= BL_CODE_START) && (flashAddr.Val <= BL_CODE_END))
                        {
                            flashAddr.Val = BL_CODE_END + 1;
                        }

                        /* 判断是否已经完成所有数据写入 FLASH 
                         * 这里没有用 P24_FLASH_SIZE_WORD 判断
                         */
                        if(flashAddr.Val >= APP_LAST_FLASH)
                        {
                            sendMMSBuf[1]   = TOMMS_PIC24_FINISHED;         /* 升级完成   */
                            updatePic24Sts  = UP_PIC24_VERIFY_OK;           /* 全部发送完成且校验Ok，准备重启 PIC24 */
                            updateStep      = 0;
                            #ifdef SKIP_BLANK_FLASH
                            break;
                            #endif
                        }
                        else
                        {
                            sendMMSBuf[1]   = TOMMS_PIC24_UPDATING;         /* 处于升级中   */
                            updatePic24Sts  = UP_PIC24_WRITE;

                            #ifdef SKIP_BLANK_FLASH
                            /* 若待写入的全部是空指令，则跳开此区域
                             *  */
                            DWORD *pInst;
                            pInst = (DWORD*)(FLASH_TO_BIN_ADDR(flashAddr.Val));
                            for(i=0; i<P24_BYTES_PER_ROW/P24_BYTES_PER_INST; i++)
                            {
                                if(*pInst != P24_BLANK_DATA)
                                {
                                    continueWr = TRUE;
                                    break;
                                }
                            }
                            #endif
                        }
                        
                    #ifdef SKIP_BLANK_FLASH
                        if(continueWr)
                            break;
                    }
                    #endif
                    
                    /* 回复 MMS 的信息 */
                    sendMMSBuf[0] = updatePic24Obj;                     /* 对象 */
                    sendMMSBuf[2] = flashAddr.v[0];                     /* 升级进度     */
                    sendMMSBuf[3] = flashAddr.v[1];
                    sendMMSBuf[4] = flashAddr.v[2];
                    sendMMSBuf[5] = (int8u)(P24_FLASH_SIZE_WORD >> 0);
                    sendMMSBuf[6] = (int8u)(P24_FLASH_SIZE_WORD >> 8);
                    sendMMSBuf[7] = (int8u)(P24_FLASH_SIZE_WORD >> 16);
                    sendMMSLen    = 8;
                }
                else
                {
                    /* 判断校验次数，是否需要再次校验 */
                    verifyErrCnt++;
                    if(verifyErrCnt >= UPCMD_TRY_VERIFY_TIMES)
                    {
                        /* 回复 MMS，校验失败：退出升级
                         */
                        updatePic24Sts = UP_PIC24_FINISHED;             /* 结束     */
                        
                        sendMMSBuf[0] = updatePic24Obj;                 /* 对象     */
                        sendMMSBuf[1] = TOMMS_PIC24_FAIL_VERIFY;        /* 失败代码 */
                        sendMMSBuf[2] = RD_FLASH;                       /* 失败信息 */
                        sendMMSBuf[3] = flashAddr.v[0];
                        sendMMSBuf[4] = flashAddr.v[1];
                        sendMMSBuf[5] = flashAddr.v[2];
                        sendMMSLen    = 6;
                    }
                }
            }
            break;
    }
    
    /* 
     * 回复 MMS
     */
    sendToMMSPackage();
}


/* =============================================================================
 * 升级 PIC24 -- 
 * PIC32 与 PIC24 Bootloader 通信协议程序，
 *   PIC32 发送  : 命令PIC24 重启运行
 */
PRIVATE void upPIC24_BL_VerifyOK(void)
{
    tFlashInfo      P24CodeInfo;
    tSYSTICK        tickNow;
    
    tickNow = getSysTick();
    
    switch(updateStep)
    {
        /* 初始化 */
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;

        /* 选择通信对象 */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep      = P24_STEP_TX2P24;
        
        /* 发送命令给 PIC24 App or Bootloader
         *  */
        case P24_STEP_TX2P24:
            getBinCodeInfo(&P24CodeInfo);
            sendPic24Buf[0] = VERIFY_OK;
            memcpy(&sendPic24Buf[1], &P24CodeInfo, sizeof(tFlashInfo));
            sendPic24Len    = sizeof(tFlashInfo) + 1;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* 等待 PIC24 App or Bootloader 回复
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* PIC24 超时未回复，重复执行发送命令给 PIC24
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 校验失败：回读 PIC24 超时错
                     * 回复 MMS
                     */
                    sendMMSBuf[0] = updatePic24Obj;                     /* 对象 */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;          /* 失败代码 */
                    sendMMSBuf[2] = VERIFY_OK;                          /* 失败信息 */
                    sendMMSLen    = 3;
                    updatePic24Sts = UP_PIC24_FINISHED; /* 结束 */
                }
            }
            break;
            
        /* 收到回复的启动状态，退出升级状态
         */
        case P24_STEP_REPLY_OK:
            updateStep      = UP_PIC24_INIT;
            updatePic24Sts  = UP_PIC24_FINISHED;            /* 结束     */
            
            sendMMSBuf[0]   = updatePic24Obj;               /* 对象     */
            sendMMSBuf[1]   = TOMMS_PIC24_RUNAPP;           /* 升级完成，启动APP   */
            sendMMSLen      = 3;
            if(pic24ReplyBuf[0] == VERIFY_OK)
            {
                sendMMSBuf[2]  = UP_PIC24_BL_RESET;         /* BOOTLOADER 校验成功，启动重启 */
                upPIC24_ResetChip(updatePic24Obj);
            }
            else if(pic24ReplyBuf[0] == (VERIFY_OK + 0x80))
            {
                sendMMSBuf[2]  = UP_PIC24_BL_NOTRESET;      /* BOOTLOADER 校验失败，不启动重启 */
            }
            break;
    }
    
    /* 
     * 回复 MMS
     */    
    sendToMMSPackage();

}
            
/* =============================================================================
 * 计算 PIC24 FLASH-HEX 文件（转换成 BIN）的校验和 
 * 返回全区域的校验和，以及 不包含 Bootloader 区域的校验和
 */
PUBLIC void getBinCodeInfo(tFlashInfo * P24CodeInfo)
{
    tDWORD flashData;
    int32u tmpCS;
    int32u offset;
    
    int8u   *pCode;
    int8u   *pInfo;
    int     i;
    
    /* bootloader 版本信息 */
    pInfo = (int8u *)(&P24CodeInfo->blInfo);
    pCode = (int8u *)(P24_CodeBin + (BL_VERSION_ADDR<<1));
    for(i=0; i<sizeof(P24CodeInfo->blInfo); i += 2)
    {
        *pInfo ++= *pCode ++;
        *pInfo ++= *pCode ++;
        pCode   += 2;           /* 跳过2字节 */
    }
    
    offset  = 0;
    P24CodeInfo->allCodeCS  = 0;
    P24CodeInfo->onlyAppCS  = 0;
    
    /* 指向 BIN 文件 */
    pCode   = (int8u*)P24_CodeBin;
    
    while(offset < (P24_FLASH_SIZE_WORD<<1))
    {
        /* read flash data 
         * 下述情况，在 HEX 转换成 BIN文件时处理，BIN 再转成 C
         *  - 复位向量(BL_RESET_VICTOR)指向 BOOTLOADER ,
         *  - 读配置字(P24_CONFIG_WORD_ADDR)高2字节总是 0,
         *  - FLASH 4 字节，最高字节总是0，
         */
        flashData.v[0] = *pCode++;
        flashData.v[1] = *pCode++;
        flashData.v[2] = *pCode++;
        flashData.v[3] = *pCode++;
        
        
        /* 根据规格书资料，读配置字时，总是为 FF xx xx，即高字节总是为FF
         * 编译器输出的 HEX 文件中，高字节是 00
         * 为了匹配2者的校验和，这里做了特殊处理
         */
        if(offset >= (P24_CONFIG_WORD_ADDR<<1))
        {
            flashData.v[2] = 0;
        }
        
        tmpCS = flashData.v[0] + flashData.v[1] + flashData.v[2] + flashData.v[3];
        P24CodeInfo->allCodeCS += tmpCS;
        
        if((offset < (BL_CODE_START<<1)) || (offset > (BL_CODE_END<<1)))
        {
            P24CodeInfo->onlyAppCS += tmpCS;
        }
        
        /* 下一条指令地址偏移
         */
        offset += P24_BYTES_PER_INST;
    }
    
    
    #if 0
    uartStartSend(COM_MMS, (int8u*)P24CodeInfo, sizeof(tFlashInfo));
    #endif
    
    return;
}

/* =============================================================================
 * 升级 PIC24 主程序
 * 
 */
PUBLIC void Pic24UpdateProcess(tUartComm *pComm)
{
    tSYSTICK tickNow;
    
    tickNow = getSysTick();
    pP24UpComm = pComm;
    
    {
        /* 判断启动升级的时间 
         * 从启动开始计算，若指定时间内没有响应，
         *  - 当前是 A，则切换对象B
         *  - 当前是 B，则退出升级
         */
        if(updatePic24Sts != UP_PIC24_NULL)
        {
            if((tickNow - updateP24_StartTick) >= (UPDATE_MAX_TIME_SEC * 1000))
            {
                sendMMSBuf[0]   = updatePic24Obj;               /* 对象     */
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_OVERTIME;    /* 失败代码 */
                sendMMSBuf[2]   = 0xff;                         /* 失败信息 */
                sendMMSBuf[3]   = 0xff;                         /* 失败信息 */
                sendMMSLen      = 4;

                /* 
                 * 回复 MMS
                 */
                sendToMMSPackage();

                updatePic24Sts      = UP_PIC24_FINISHED;
                updateP24_StartTick = tickNow;              /* 为了可能切换到 B       */
            }
        }
        
        switch(updatePic24Sts)
        {
            /* 空闲中，没有启动升级 */
            case UP_PIC24_NULL :            
                break;

            /* 升级初始化，先查询 PIC24 的HEX信息，与待升级的HEX对比
             * 发送命令 OBJ_CMD_BL_CTRL -> PIC24APP_CMD_GET_CS
             * 
             * 由 PIC24 APP 回复
             *  */
            case UP_PIC24_INIT:             
                upPIC24_App_QueryInfo();    /*  */
                break;

            /* 发送命令启动 PIC24 进入 Bootloader
             * OBJ_CMD_BL_CTRL -> PIC24APP_CMD_ENTER_BL
             * 
             * 由 PIC24 Bootloader 回复
             *  */
            case UP_PIC24_START_BL:         
                upPIC24_App_StartBL();
                break;

            /* ===== PIC24 已经启动 Bootloader 进行升级 ===== 
             * 以下全部是 PIC32 与 PIC24 Bootloader 完成交互过程
             *  1 先擦除 FLASH
             *  2 发送升级数据（写 FLASH）
             *  3 回读FLASH进行校验（读FLASH）
             *  4 重复 2~3，直到全部写入
             */
            case UP_PIC24_UPDATE_INI:               /* 启动升级初始化 */
                updateStep      = P24_STEP_INIT;
                flashAddr.Val   = 0;
                updatePic24Sts  = UP_PIC24_ERASE;   /* 进入下一个过程： PIC24 Bootloader 擦除 FLASH */
                break;

            case UP_PIC24_ERASE:                    /* 发送命令 PIC24 Bootloader 擦除       */
                upPIC24_BL_EraseFlash();
                
                #if 0
                /* 测试，可以读取 Bootloader擦除以后的FLASH内容 */
                if(updatePic24Sts == UP_PIC24_WRITE)
                {
                    updatePic24Sts = UP_PIC24_FINISHED;             /* 结束 */
                }
                #endif
                break;

            case UP_PIC24_WRITE:                    /* 发送命令给 PIC24 Bootloader 写FLASH  */
                //updatePic24Sts = UP_PIC24_NULL;
                upPIC24_BL_WriteFlash();
                break;

            case UP_PIC24_VERIFY_WR:                /* 读FLASH 进行校验  */
                upPIC24_BL_VerifyProg();
                break;
                
            case UP_PIC24_VERIFY_OK:                /* 重启PIC24运行    */
                upPIC24_BL_VerifyOK();
                break;

            case UP_PIC24_FINISHED:                 /*  结束本次升级 */
                /* 判断 A 和 B 都已结束？ */
                #if 1   /* 0 = 调试时只需要用 PIC24A */
                if(updatePic24Obj == COMMOBJ_IORW_A)
                {
                    /* A 结束，启动 B */
                    updatePic24Obj  = COMMOBJ_IORW_B;
                    updatePic24Sts  = UP_PIC24_INIT;
                    updateStep      = P24_STEP_INIT; 
                }
                else
                #endif
                {
                    /* A 和 B 都已结束 */
                    updatePic24Sts = UP_PIC24_NULL;
                }
                break;
        }
    }
    
}
