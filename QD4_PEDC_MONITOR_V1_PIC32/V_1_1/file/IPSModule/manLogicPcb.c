/* =============================================================================*/
/* SZ16 管理冗余的逻辑板
 * LOG板采用主备冗余方式，LOG板包含了 IBP PSL SIG 的功能
 *  每个LOG板的ID通过拨码确定
 *  Monitor 通过发送命令给安全回路切换板，该板通过输出口输出选择工作的 LOG板
 *  */

#include "include.h"
#include "manLogicPcb.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。 */
#define LOG2_SWITCHTO_LOG1      1   /* 0--不允许 逻辑板2 切换到 逻辑板1，否则允许 */
#define SWITCH_RESET_SELECT     1   /* 0--不允许切换板重置 K0 继电器，否则允许    */

/* 定义 LOG 板工作状态 */
#define LOGERR_NONE             0   /* 无错误           */
#define LOGERR_OFFLINE          1   /* 离线             */
#define LOGERR_RELAY            2   /* 继电器错误       */
#define LOGERR_LINK             3   /* 板间连接线错误   */
#define LOGERR_I2CFUNCT         4   /* I2C访问错误      */
#define LOGERR_CMDOUTPUT        5   /* 输出命令错误     */
#define LOG_OUTCMD_ERR_TIME     (100)   /* 100 x 8ms = 800ms */

#define LOG_ERROR_LAST_TIME     (50)    /* 延时确认 LOG 板的错误，单位是8ms, 0 表示不需要延时 */

/* LOG PCB的状态信息 
 * 提供接口函数读取 LOG1 和 LOG2 的状态（合成为一字节，高4位是 LOG2，低4位是LOG1）
 */
PRIVATE int8u LOG1Error = LOGERR_NONE;    
PRIVATE int8u LOG2Error = LOGERR_NONE;
    PUBLIC int8u getLOGPcbState(void)
    {
        return ((LOG2Error & 0x0f) << 4) | (LOG1Error & 0x0f);
    }

/* 当前选择的工作 LOG 板ID */
PRIVATE tEnmCommObj workLOGID = COMMOBJ_UNDEF;  
    /* 读当前的工作LOG板ID */
    PUBLIC tEnmCommObj getWorkLOGID(void)
    {
        return workLOGID;
    }

extern tChkOutputCmd MON_CmdMon;       /* 监视板的命令监视点   */
extern tChkOutputCmd LOGOut_Relay;     /* 逻辑板继电器输出状态 */

/* =============================================================================*/
/* 设置工作LOG板ID
 * 输入
 *  id
 * 返回
 *  TRUE  表示设置成功
 *  FALSE 表示设置失败，将维持上一次的选择
 *  */
PUBLIC BOOL setWorkLOGID(tEnmCommObj LOGID)
{
    BOOL result;
    
    if((LOGID == COMMOBJ_LOG1) || (LOGID == COMMOBJ_LOG2))
    {
        result = TRUE;        
        
        if((workLOGID != LOGID))
        {
            /* 当前选择的工作 LOG 板变化以后，丢弃开始通信的数据，计数器 */
            ((tSlaveInfo*)getSlaveInfoPtr(LOGID))->dataValidCnt = 5;
        }
        workLOGID = LOGID;
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

/* =============================================================================\
 * 判断 LOG 板的错误情况
 * 注意LOG 板1和2经过了继电器进行通断处理
 * */
//BIT8 mon_LOGPcbCmdOut;
//#define MON_LOG_OUTCMD_OPN1     mon_LOGPcbCmdOut.B0     /* 监视输出开门命令1 */
//#define MON_LOG_OUTCMD_CLS1     mon_LOGPcbCmdOut.B1     /* 监视输出关门命令1 */
//#define MON_LOG_OUTCMD_OPN2     mon_LOGPcbCmdOut.B2     /* 监视输出开门命令2 */
//#define MON_LOG_OUTCMD_CLS2     mon_LOGPcbCmdOut.B3     /* 监视输出关门命令2 */


///* 根据输入判断逻辑板理论输出 */
//BIT8 LOG_CmdOutByIO;
//
//
//BIT8 LOG_CmdOutByRelay;
//#define LOG_LOG_OUTOPN1         LOG_CmdOutByRelay.B0
//#define LOG_LOG_OUTCLS1         LOG_CmdOutByRelay.B1
////#define LOG_LOG_OUTOPN2         LOG_CmdOutByRelay.B2
////#define LOG_LOG_OUTCLS2         LOG_CmdOutByRelay.B3
//#define LOG_LOG_OUT_IBPPSLOPN   LOG_CmdOutByRelay.B4
//#define LOG_LOG_OUT_OPNSIDE     LOG_CmdOutByRelay.B5

#ifdef LOG_OUT_Qn
BIT8 LOG_LOGPcbCmdOut;
#define LOG_LOG_OUTCMD_OPN1     LOG_LOGPcbCmdOut.B0     /* LOG板输出开门命令1 */
#define LOG_LOG_OUTCMD_CLS1     LOG_LOGPcbCmdOut.B1     /* LOG板输出关门命令1 */
#define LOG_LOG_OUTCMD_OPN2     LOG_LOGPcbCmdOut.B2     /* LOG板输出开门命令2 */
#define LOG_LOG_OUTCMD_CLS2     LOG_LOGPcbCmdOut.B3     /* LOG板输出关门命令2 */
#endif

/* LOGSelector 是外部选择 */
PRIVATE void checkLOGPcbErrors(tEnmCommObj LOGPcb)
{
    #if 1
    tSlaveInfo * pLOGInfo;
    int8u LOG1Err_tmp, LOG2Err_tmp;
    
    /* ========== 冗余的 LOG1 ========== */
    do
    {
        LOG1Err_tmp             = LOGERR_NONE;
        
        #ifdef LOG_OUT_Qn
        LOG_LOGPcbCmdOut.byte   = 0;
        #endif
        
        
        /* 离线? */
        if(IPSObj_chkOnline(COMMOBJ_LOG1) == FALSE)
        {
            LOG1Err_tmp = LOGERR_OFFLINE;
            break;
        }
        else
        {
            /* 判断丢弃数据次数 */
            if(((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1))->dataValidCnt > 0)
            {
                break;
            }
            
            /* 继电器是否有错误 */
            LOG1Err_tmp = PEDC_RELAY4.byte | PEDC_RELAY5.byte | PEDC_RELAY6.byte | PEDC_RELAY7.byte | PEDC_RELAY8.byte | PEDC_RELAY9.byte;
            LOG1Err_tmp &= 0b10101010;
            if(LOG1Err_tmp != LOGERR_NONE)
            {
                LOG1Err_tmp = LOGERR_RELAY;
                break;
            }
            
            /* 连线是否有错误，切换板B路接的是LOG1 */
            if((SWCH_LinkErr_CMD_LOG1 != 0) || (PEDC_LOG1MONLinkMsg.byte & LOGLINKERR_MASK_BITS != 0))
            {
                LOG1Err_tmp = LOGERR_LINK;
                break;
            }
            
            /* I2C是否有错误 */
            pLOGInfo = (tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1);
            if(pLOGInfo->slaveSts.bits.pca9505Err1 || pLOGInfo->slaveSts.bits.pca9505Err2 || pLOGInfo->slaveSts.bits.pca9505Err3)
            {
                LOG1Err_tmp = LOGERR_I2CFUNCT;
                break;
            }
        }
        
        /* 没有上述错误
         * 检查 LOG板 输入输出命令逻辑是否有错误
         * 注意：
         *   1. 逻辑板是根据输入信号用软件运算后输出控制命令，
         *      因此这里不能直接用输入得到输出的判断
         *   2. 监视板没有将两个逻辑板的输出都监视，只能监视选定的逻辑板
         */
        if(LOG1Err_tmp == LOGERR_NONE)
        {
            if(bLOG_WorkLOG2 == 0)
            {
                /* 当前工作逻辑板1，判断是否持续输出命令错误状态 */        
                if((PEDC_LinkErrMsg1.byte & MONLINKERR_MASK_BITS) != 0)
                {
                    if(checkingTim[TIM_LOG1_OUT_ERR].posTim >= LOG_OUTCMD_ERR_TIME)
                    {
                        LOG1Err_tmp = LOGERR_CMDOUTPUT;
                    }
                }
                else
                {
                    checkingTim[TIM_LOG1_OUT_ERR].posTim = 0;
                }
            }
        }
    }while(FALSE);
    
    /* 判断错误及其持续时间
     * 板1的错误确认时间 + 10 大于 板2的错误确认时间，
     * 目的是避免上电后板1板2都有错误时，板1先确认错误而切换到板2 */
    if(LOG1Err_tmp != LOGERR_NONE)
    {
        if(checkingTim[TIM_LOG1_ERROR].posTim >= (LOG_ERROR_LAST_TIME + 10))
        {
            /* 非离线错误以先到的错误为准，若有离线则优先报离线 */
            if((LOG1Error == LOGERR_NONE) || (LOG1Err_tmp == LOGERR_OFFLINE))
            {
                LOG1Error = LOG1Err_tmp;
            }
        }
    }
    else
    {
        checkingTim[TIM_LOG1_ERROR].posTim      = 0;
        if(LOG1Error != LOGERR_NONE)
        {
            /* 错误恢复？暂时没处理 */
            LOG1Error = LOGERR_NONE;
        }
    }
    
    /* ========== 冗余的 LOG2 ========== */
    do
    {
        LOG2Err_tmp             = LOGERR_NONE;
        
        #ifdef LOG_OUT_Qn
        LOG_LOGPcbCmdOut.byte   = 0;
        #endif
        
        /* 离线? */
        if(IPSObj_chkOnline(COMMOBJ_LOG2) == FALSE)
        {
            LOG2Err_tmp = LOGERR_OFFLINE;
            break;
        }
        else
        {
            /* 判断丢弃数据次数 */
            if(((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2))->dataValidCnt > 0)
            {
                break;
            }
            
            /* 继电器是否有错误 */
            LOG2Err_tmp = PEDC_RELAY10.byte | PEDC_RELAY11.byte | PEDC_RELAY12.byte | PEDC_RELAY13.byte | PEDC_RELAY14.byte | PEDC_RELAY15.byte;
            LOG2Err_tmp &= 0b10101010;
            if(LOG2Err_tmp != LOGERR_NONE)
            {
                LOG2Err_tmp = LOGERR_RELAY;
                break;
            }
            
            /* 连线是否有错误，切换板A路接的是LOG2 */
            if((SWCH_LinkErr_CMD_LOG2 != 0) || (PEDC_LOG2MONLinkMsg.byte & LOGLINKERR_MASK_BITS != 0))
            {
                LOG2Err_tmp = LOGERR_LINK;
                break;
            }
            
            /* I2C是否有错误 */
            pLOGInfo = (tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2);
            if(pLOGInfo->slaveSts.bits.pca9505Err1 || pLOGInfo->slaveSts.bits.pca9505Err2 || pLOGInfo->slaveSts.bits.pca9505Err3)
            {
                LOG2Err_tmp = LOGERR_I2CFUNCT;
                break;
            }
        }
        
        /* 没有上述错误
         * 检查 LOG板 输入输出命令逻辑是否有错误
         * 注意：
         *   1. 逻辑板是根据输入信号用软件运算后输出控制命令，
         *      因此这里不能直接用输入得到输出的判断
         *   2. 监视板没有将两个逻辑板的输出都监视，只能监视选定的逻辑板
         */
        if(LOG2Err_tmp == LOGERR_NONE)
        {
            if(bLOG_WorkLOG2 == 1)
            {
                /* 当前工作逻辑板2，判断是否持续输出命令错误状态 */        
                if((PEDC_LinkErrMsg1.byte & MONLINKERR_MASK_BITS) != 0)
                {
                    if(checkingTim[TIM_LOG2_OUT_ERR].posTim >= LOG_OUTCMD_ERR_TIME)
                    {
                        LOG2Err_tmp = LOGERR_CMDOUTPUT;
                    }
                }
                else
                {
                    checkingTim[TIM_LOG2_OUT_ERR].posTim = 0;
                }
            }
        }
    }while(FALSE);
    
    /* 判断错误及其持续时间
     * 板1的错误确认时间 + 10 大于 板2的错误确认时间，
     * 目的是避免上电后板1板2都有错误时，板1先确认错误而切换到板2 */
    if(LOG2Err_tmp != LOGERR_NONE)
    {
        if(checkingTim[TIM_LOG2_ERROR].posTim >= (LOG_ERROR_LAST_TIME + 0))
        {
            /* 非离线错误以先到的错误为准，若有离线则优先报离线 */
            if((LOG2Error == LOGERR_NONE) || (LOG2Err_tmp == LOGERR_OFFLINE))
            {
                LOG2Error = LOG2Err_tmp;
            }
        }
    }
    else
    {
        checkingTim[TIM_LOG2_ERROR].posTim      = 0;
        if(LOG2Error != LOGERR_NONE)
        {
            /* 错误恢复？暂时没处理 */
            LOG2Error = LOGERR_NONE;
        }
    }
    #endif
}


/* =============================================================================
 * 管理 LOG1 和 LOG2
 * 电气接线有3个输入点，分别表示手动选择 LOG1，自动选择，手动选择LOG2
 * 当手动选择时，MONITOR的选择被硬件切断即无效
 * 当自动选择时，MONITOR判断LOG1是否正常，是则默认选择LOG1，否则选择LOG2
 * 当自动切换到LOG2后，不再自动切换回S1
 * 
 *  */
PRIVATE int8u LOGMode = 0xff;       /* 0=  自动选择，1=手动选择LOG1，2=手动选择LOG2, 0xff=MCU复位 */
PRIVATE int8u autoLOG = 0xff;       /* 0=待自动选择，1=自动选择LOG1，2=自动选择LOG2, 0xff=MCU复位 */
PUBLIC void manageLOG1LOG2(void)
{
    #if 1
    /* MONITOR 上电或复位后等待一段时间后再判断
     * 避免一启动运行时出现的异常（如 LOG 板还没有通信成功）
     *  */
    static   tSYSTICK powerOnTick;
    tSYSTICK tickNow;
    tickNow = getSysTick();
    if(autoLOG == 0xff)
    {
        powerOnTick = tickNow;
        autoLOG = 0;
    }
    
    /* 判断延时时间 */
    if((tSYSTICK)(tickNow - powerOnTick < 3000))
    {
        /* 丢弃开始通信的数据，计数器 */
        ((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1))->dataValidCnt = 5;
        ((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2))->dataValidCnt = 5;
        return;
    }
    
    /* 
     * LOGMode = 通过IO判断当前选择的模式
     * 根据开关状态确定
     *  */
    if(bLOG_Sw_LOG1)
    {
        /* 1 = 手动选择LOG1 */
        if(LOGMode != 1)
        {
            LOG1Error   = LOGERR_NONE;
            LOGMode     = 1;
        }
        autoLOG = 0;
        
        setWorkLOGID(COMMOBJ_LOG1);
    }
    else if(bLOG_Sw_LOG2)
    {
        /* 2 = 手动选择LOG2 */
        if(LOGMode != 2)
        {
            LOG2Error   = LOGERR_NONE;
            LOGMode     = 2;
        }
        autoLOG = 0;
        setWorkLOGID(COMMOBJ_LOG2);
    }
    else /* if(bLOG_Sw_Auto) */
    {
        if(LOGMode != 0)
        {
            LOGMode     = 0;
            LOG1Error   = LOGERR_NONE;
            LOG2Error   = LOGERR_NONE;
        }
        
        /* 0 = 自动选择 */
        if(autoLOG == 0)
        {   
            /* 手动切换到自动，或者上电后第一次选择
             * 只要 LOG1 正常就选 LOG1
             * 当   LOG1 异常就切换到 LOG2
             */
            if(LOG1Error == LOGERR_NONE)
            {
                autoLOG = 1;
            }
            else
            {
                autoLOG = 2;
            }
        }
        else if(autoLOG == 1)
        {
            /* 自动选择 LOG1，
             * 若 LOG1 异常且 LOG2正常则选择 LOG2
             *  */
            if((IPSObj_chkOnline(COMMOBJ_LOG1) == TRUE) && (IPSObj_chkOnline(COMMOBJ_LOG2) == TRUE))
            {
                /* LOG1和2都在线 */
                if((LOG1Error != LOGERR_NONE) &&  (LOG2Error == LOGERR_NONE))
                {
                    autoLOG = 2;
                }
            }
            else
            {
                /* 如果只有LOG2在线，无论LOG2是否有其它故障，都切换到LOG2 */
                if((LOG1Error == LOGERR_OFFLINE) && (LOG2Error != LOGERR_OFFLINE))
                {
                    autoLOG = 2;
                }
            }
        }
        /* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。 */
        else if(autoLOG == 2)
        {
            #if (LOG2_SWITCHTO_LOG1 != 0)
            /* 自动选择 LOG2，
             * 若 LOG2 异常且 LOG1正常则选择 LOG1
             *  */
            if((IPSObj_chkOnline(COMMOBJ_LOG1) == TRUE) && (IPSObj_chkOnline(COMMOBJ_LOG2) == TRUE))
            {
                /* LOG1和2都在线 */
                if((LOG1Error == LOGERR_NONE) &&  (LOG2Error != LOGERR_NONE))
                {
                    autoLOG = 1;
                }
            }
            else
            {
                /* 如果只有LOG1在线，无论LOG1是否有其它故障，都切换到LOG1 */
                if((LOG1Error != LOGERR_OFFLINE) && (LOG2Error == LOGERR_OFFLINE))
                {
                    autoLOG = 1;
                }
            }
            #endif
        }
        
        if(autoLOG == 1)    
        {
            setWorkLOGID(COMMOBJ_LOG1);
        }
        else
        {
            setWorkLOGID(COMMOBJ_LOG2);
        }
    }
    
    /* 判断 LOG 板的错误情况 */
    checkLOGPcbErrors(workLOGID);
    
    #endif
}


/* =============================================================================
 * 给 切换板的 IO 输出数据
 *  */
tCmdWriteIO_SFSW switchOutput;
#if 0
extern tCmdWriteIO_SFSW testSFSWOutput;
#endif
int8u* getSwitchOutputIODataPtr(int *DataLen)
{
    static int8u output_Prev = 0xff;
    
    
    /* 先禁止输出 */
    memset((void*)switchOutput.IOData, 0, sizeof(switchOutput));
    
    #if (SWITCH_RESET_SELECT != 0)
    if((autoLOG == 1) || (autoLOG == 2))
    {
        /* 1=自动选择LOG1 时，请求 切换板输出控制 IO 复位继电器 K0
         * 注意：输出 0 是驱动 切换板的 继电器K0
         *  */
        switchOutput.IOOut.RESET_En = 1;
        switchOutput.IOOut.RESET    = 1;
        if(autoLOG == 1)
        {
            /* 这里是为了在自动模式下，降低 K0 线圈的通电时间，延长继电器寿命
             *  */
            if(output_Prev != autoLOG)
            {
                checkingTim[RTIM_SWITCHER_RESET_SELECT].posTim = 0;
            }

            /* 继电器K0 通电时间 5000ms */
            if(checkingTim[RTIM_SWITCHER_RESET_SELECT].posTim < (5000/RELAY_TIMER_INTERVAL))
            {
                switchOutput.IOOut.RESET = 0;
            }
        }

        output_Prev = autoLOG;
    }
    #endif
    
    *DataLen = sizeof(tCmdWriteIO_SFSW);
    
    int8u *pOut;
    #if 1
    /* 输出数据 发送给切换板
     *  */
    pOut = (int8u*)&switchOutput.IOData[0];
    #else

    /* 是否处于 IO 输出测试模式?
     * 是则用 测试输出数据 发送给切换板
     *  */
    if(chkIfTestOutputIO() == FALSE)
    {
        pOut = (int8u*)&switchOutput.IOData[0];
    }
    else
    {
        pOut = (int8u*)&testSFSWOutput.IOData[0];
    }
    #endif

    return (int8u*)pOut;
}


