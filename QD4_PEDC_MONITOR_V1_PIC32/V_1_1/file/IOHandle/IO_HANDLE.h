/* PROJECT : SZ16 - prototype 
 *          PCB FD_PEDC_MONITOR_SZ20_V1.pdf 
 */
//--------------------------------------
#ifndef _IO_HANDLE_H
#define _IO_HANDLE_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
//--------------------------------------
// 
//--------------------------------------

#define VALID                   0               //继电器吸合
#define INVALID                 1
#define GET_INPUT_STS(input)    (!(input))      //读输入状态 0
#define GET_INPUT_STS_1(input)  ((input))       //读输入状态 1

#define RELAY_POW_ON(relay)     (!(relay))      /* 继电器通电   */
#define RELAY_POW_OFF(relay)    ( (relay))      /* 继电器未通电 */

#define RELAY_NC_ON(relay)      ( (relay))      /* 常闭触点通电   */
#define RELAY_NC_OFF(relay)     (!(relay))      /* 常闭触点未通电 */

#define RELAY_NO_ON(relay)      (!(relay))      /* 常开触点通电   */
#define RELAY_NO_OFF(relay)     ( (relay))      /* 常开触点未通电 */

#define LED_ON                  0               //LED 点亮
#define LED_OFF                 1
#define LEDON_IF_STS1(state1)   (!(state1))     //状态有效时为1 输出低电平点亮 LED
#define LEDON_IF_STS0(state0)   ( (state0))     //状态有效时为0 输出低电平点亮 LED

#define BUZZER_ON               0               //蜂鸣器响
#define BUZZER_OFF              1


#define SETBIT                  1               //设置 1
#define CLSBIT                  0

#define Input_VALID             0               //有输入
#define Input_INVALID           1               //没有输入

#define CBIGEQU                 0
#define CLESS                   1   
    

/* =============================================================================
 * 从扩展板或PIC24读到的输入数据定义
 * 注意：暂时用了兼容以前的长度，所以实际读取的数据做了变换，
 *       参见 mianIO.c : 函数 getPCBInputData()
 *  */

#define    MON_INPUT_GPIO_LEN       12                  /*MCU GPIO读取的输入数据长度，GPIO B~G */
#define    MON_INPUT_DATA_LEN       15                  /*从PIC24读取的输入数据长度，3颗PCA9505 */
#define    SWCH_INPUT_DATA_LEN      12                  /*从切换板板读取的输入数据长度，GPIO B~G */
#define    LOG1_INPUT_DATA_LEN      15                  /*从逻辑板1板读取的输入数据长度，3颗PCA9505 */
#define    LOG2_INPUT_DATA_LEN      15                  /*从逻辑板2板读取的输入数据长度，3颗PCA9505 */

EXTN BIT8  MON_input_GPIO[MON_INPUT_GPIO_LEN];          /* : Monitor PCB (GPIO) */
EXTN BIT8  MON_input_data[MON_INPUT_DATA_LEN];          /* : Monitor PCB (I2C PCA9505的输入) */
EXTN BIT8  SWCH_input_data[SWCH_INPUT_DATA_LEN];        /* : 切换板 PCB (PIC32 的输入) */
EXTN BIT8  LOG1_input_data[LOG1_INPUT_DATA_LEN];        /* : 逻辑板1输入 (PIC32 UB1 的输入) */
EXTN BIT8  LOG2_input_data[LOG2_INPUT_DATA_LEN];        /* : 逻辑板2输入 (PIC32 UB1 的输入) */

#define OUTPUT_DATA_LEN             (15 + 1)            /* 输出给PIC24的数据长度，3颗PCA9505 + 本地PBL */
EXTN BIT8 MON_output_data[OUTPUT_DATA_LEN];             /* */

#define MON_Output_RBL_OFFSET       15
#define MON_Output_RBL              MON_output_data[MON_Output_RBL_OFFSET]

#if 0
//切换板输出 MON_Output_SFSW
typedef union
{
    struct
    {
        int8u   SFSW_Q0_Q1Swtich    : 1;    /* 切换板 Q0 */
        int8u   SFSW_Q1_Q2Swtich    : 1;    /* 切换板 Q1 */
        int8u   SFSW_Q2_Reset       : 1;    /* 切换板 Q2 */
        int8u   SFSW_Q3_PN0         : 1;    /* 切换板 Q3 */
        int8u                       : 4;       
    };
    
    int8u byte;
}tSFSWOutput;
#endif

/* 定义各PCB输入输出点的含义 */
#include "PCB_IO_Def.h"

/* =============================================================================
 *  */
typedef union
{ 
    /* 注意顺序与 给MMS的协议点表 对应(PEDC_PSC1) */
    struct
    {
        int8u Cls1        : 1;       /* 关门命令1     */
        int8u Opn1        : 1;       /* 开门命令1     */
        int8u Cls2        : 1;       /* 关门命令2     */
        int8u Opn2        : 1;       /* 开门命令2     */ 
        int8u ClsSide1    : 1;       /* 关边门1       */
        int8u OpnSide1    : 1;       /* 开边门1       */
        int8u ClsSide2    : 1;       /* 关边门2       */
        int8u OpnSide2    : 1;       /* 开边门2       */
    };
    
    int8u byte;
}tChkOutputCmd;

/* =============================================================================
 * 发送给PEDC的信息
 * Relay Info = 20 bytes for SZ20 or SZ16
 * Extend data(interface IO) = 16 bytes
 */
EXTN BIT8 MESG_BUF[AVR_DATA_LEN];

#define PEDC_RELAY_SEAT     0
#define PEDC_PSC_SEAT       AVR_DATA_RELAY_LEN

/* 定义给 MMS 的继电器、扩展信息 */
#include "ToMMS_InfoDef.h"

/* =============================================================================
 * 用于继电器状态判断的计时器
 */
#define RELAY_TIMER_INTERVAL    10
#define TIME_VALUE_BASE_10(a)   ((a)/RELAY_TIMER_INTERVAL)
#define cRelay_positive_TIME  200                 //约 10ms * 200
#define cRelay_reverse_TIME   200                 //约 10ms * 200

//positive : 继电器两端有电压 ，如果超过一定时间，继电器触点没动作，则判该继电器故障。
//reverse  : 继电器两端没有电压 ，如果超过一定时间，继电器触点一直保持动作，则判该继电器故障。
typedef struct
{
    int8u posTim;
    int8u revTim;
}tRelayTime;

typedef enum
{    
    /* SZ16 切换板 */
    SWCH_K0     = 0 ,
    SWCH_K1         ,
    SWCH_K2         ,
    SWCH_K3         ,
    SWCH_K4         ,
    SWCH_K5         ,
    SWCH_K6         ,
    SWCH_K11        ,
    SWCH_K12        ,
    SWCH_K13        ,
    SWCH_K21        ,
    SWCH_K22        ,
    SWCH_K23        ,

    /* SZ16 逻辑板1 */
    LOG1_K0         ,
    LOG1_K1A        ,
    LOG1_K1B        ,
    LOG1_K2A        ,
    LOG1_K2B        ,
    LOG1_K3A        ,
    LOG1_K3B        ,
    LOG1_K4A        ,
    LOG1_K4B        ,
    LOG1_K5A        ,
    LOG1_K5B        ,
    LOG1_K6A        ,
    LOG1_K6B        ,
    LOG1_K7A        ,
    LOG1_K7B        ,
    LOG1_K8A        ,
    LOG1_K8B        ,
    LOG1_K21        ,
    LOG1_K22        ,
    LOG1_K10        ,
    LOG1_K30        ,
            
    /* SZ16 逻辑板2 */
    LOG2_K0         ,
    LOG2_K1A        ,
    LOG2_K1B        ,
    LOG2_K2A        ,
    LOG2_K2B        ,
    LOG2_K3A        ,
    LOG2_K3B        ,
    LOG2_K4A        ,
    LOG2_K4B        ,
    LOG2_K5A        ,
    LOG2_K5B        ,
    LOG2_K6A        ,
    LOG2_K6B        ,
    LOG2_K7A        ,
    LOG2_K7B        ,
    LOG2_K8A        ,
    LOG2_K8B        ,
    LOG2_K21        ,
    LOG2_K22        ,
    LOG2_K10        ,
    LOG2_K30        ,
            
    /* 继电器数量，编译器自动计算，必须放在最后 */
    TOTAL_RELAYS,
}tEnumRelaysList;

/* 用于记录继电器通电或断电的时间
 * 查看 map 文件，relayWorkTime 的实际占有存储空间，与TOTAL_RELAYS 的值是相符合的  
 * 所以不需要单独定义 TOTAL_RELAYS
 * #define TOTAL_RELAYS          50
 */
EXTN tRelayTime relayWorkTime[TOTAL_RELAYS];

typedef enum
{
    TIM_MON_OPNCLS_ERR      = 0 ,   /* 控制命令（输出开关门命令）  */
    TIM_MON_SIGCMD              ,   /* 逻辑板信号开关门命令        */
    TIM_MON_IBPOPNSD_ERR        ,   /* 开边门命令                  */
    TIM_SWCH_Q1Q2               ,   /* 切换板: J3端子15和16不一致  */
    TIM_SWCH_Q1AQ2A_LOST        ,   /* 切换板: J3端子15和16与监视板连线缺失  */
    TIM_LOG1LOG2_SELECT         ,   /* 监视板选择开关输入错误  */
            
    /* 输出命令错误 */
    TIM_LOG1_OUT_ERR,
    TIM_LOG1_ERROR,
            
    TIM_LOG2_OUT_ERR,
    TIM_LOG2_ERROR,
            
    TIM_LINKERR_LOG1MON,
    TIM_LINKERR_LOG2MON,
    TIM_LINKERR_LOG1LOG2,
            
    /* 时间继电器监视 */
    TIM_KUT1,       //安全回路时间继电器KUT1线圈
    TIM_KUT2,       //信号开门时间继电器KUT2线圈
    TIM_KUT3,       //信号关门时间继电器KUT3线圈
            
    TIM_LOG_SIGCMDERR,
            
    TIM_LinkErr_CMD_LOG1,
    TIM_LinkErr_CMD_LOG2,
            
    TIM_AutoSelectLOG2,

    RTIM_SWITCHER_RESET_SELECT,
            
    /* 计时器数量，编译器自动计算，必须放在最后 */
    TOTAL_CONTACTOR_TIMERS,
}tEnumTimersList;

typedef struct
{
    int16u posTim;
}tChecingTim;

/* #define TOTAL_CONTACTOR_TIMERS  */
/* 用于判断触点状态是否正确的时间
 */
EXTN tChecingTim checkingTim[TOTAL_CONTACTOR_TIMERS];

//-------------------------------------- 
/* 记录安全回路闪断次数 */
EXTN int8u sf_flash_AccRcv ;                    /* 累加收到的安全回路闪断次数累加 */
EXTN int8u sf_flash_toMMS;                      /* 发送给MMS的，安全回路闪断次数 */
EXTN int8u sf_flash_toTest;                     /* 发送给测试的，安全回路闪断次数 */


/*--------------------------------------
 * 故障信息汇总 
 */
typedef union
{
    struct
    {    
        int32u bBUS_FAULT           : 1;  //0 总线故障：设置的有效DCU至少有一个离线
        int32u bOPN_FAULT           : 1;  //1 开门故障：来自 DCU 信息标志
        int32u bCLS_FAULT           : 1;  //2 关门故障：来自 DCU 信息标志
        int32u bUNLOCK              : 1;  //3 手动解锁：来自 DCU 门状态，参见 DOOR_STATE_UNLOCK
        int32u bMANUAL_MODE         : 1;  //4 手动操作：DCU 手动模式且不是隔离
        int32u bMONITOR_FAULT       : 1;  //5 监控系统故障：与 MMS 通信失败
        int32u bClosing             : 1;  //6 DCU关门过程：来自 DCU 门状态，参见 DOOR_STATE_CLOSING
        int32u bOpening             : 1;  //7 DCU开门过程：来自 DCU 门状态，参见 DOOR_STATE_OPENING
        int32u bPSD_FAULT           : 1;  //8 滑动门故障： 开门故障 | 关门故障 | 安全回路故障 
        int32u bDOOR_ALL_OPEN       : 1;  //9 门全开：至少一个DCU自动模式且无DCU离线或故障，
        int32u bSIG_FAULT           : 1;  //10 保留
        int32u bEmergenrydoor_opn   : 1;  //11 应急门打开：至少一道应急门扇被打开
        int32u bIsolation_MODE      : 1;  //12 隔离模式  ：至少一道滑动门处于隔离模式
        int32u bMSD_OPEN            : 1;  //13 端门打开 ，Manual Secondary Door (等同于 CAD ，司机门)
        int32u bOPEN_ALARM          : 1;  //14 保留，当PSD系统不在打开或关闭过程中ASD/EED关闭信号消失时发起
        int32u bOPEN_Side           : 1;  //15 保留，开边门状态（排烟门）
        
        int32u bHeadASD_Opned       : 1;    //首道门开启
        int32u bTailASD_Opned       : 1;    //末道门开启
    };
    
    int32u word;
}tDownMsg;

EXTN tDownMsg Down_Message0;

EXTN BIT16 Down_Message1;                          //蜂鸣器报警允许
#define AlarmAllow_BUS_FAULT    Down_Message1.B0   //总线故障蜂鸣器报警允许
#define AlarmAllow_POWER_FAULT  Down_Message1.B1   //电源故障蜂鸣器报警允许
#define AlarmAllow_OPN_FAULT    Down_Message1.B2   //开门故障蜂鸣器报警允许
#define AlarmAllow_CLS_FAULT    Down_Message1.B3   //关门故障蜂鸣器报警允许

//--------------------------------------
typedef union
{
    struct
    {    
        int16u bAlarm_POWER_FAULT   : 1;  //0 电源故障报警
        int16u bAlarm_Relays        : 1;  //1 继电器故障报警
        int16u bAlarm_BUS_FAULT     : 1;  //2 总线故障报警
        int16u bAlarm_OPN_FAULT     : 1;  //3 开门故障报警
        int16u bAlarm_CLS_FAULT     : 1;  //4 关门故障报警
        int16u bAlarm_Monitor_Fault : 1;  //5
        int16u bAlarm_LOGPCB_FAULT  : 1;  //6 冗余的逻辑板有故障
        int16u bAlarm_AutoSelLog2   : 1;  //7 2024-6-3 .自动位时控制板2工作要产生报警
        int16u          : 1;  //8
        int16u          : 1;  //9
        int16u          : 1;  //10
        int16u          : 1;  //11
        int16u          : 1;  //12
        int16u          : 1;  //13
        int16u          : 1;  //14
        int16u          : 1;  //15
    };
    
    int16u word;
}tAlarmFlags;

//alarm  蜂鸣器报警  参考 协议总格式 中的点表
//消音前的报警 和 当前的报警 比较,如果 (当前的报警 > 消音前的报警) 则报警; 如果 (当前的报警 < 消音前的报警) 则 将消失的报警从"消音前的报警"中消除
//PEDC柜 只对 柜体上有LED警报的故障 报 蜂鸣器故障(电源故障,开门故障,关门故障,总线故障), 继电器故障由于没有LED警报灯 所以没有 蜂鸣器报警.
EXTN tAlarmFlags PSC_AlarmFlags;
EXTN tAlarmFlags PSL_AlarmFlags;
EXTN tAlarmFlags IBP_AlarmFlags;

//--------------------------------------
//LLY NJ7
#define Relay_Fault_Check(K_PN,bK_S,relayNum,bRelay_Coil,bRelay_Fault)      \
do{                                                                         \
    {                                                                       \
        if((K_PN))                                                          \
        {                                                                   \
            relayWorkTime[relayNum].revTim = 0;                             \
            if((bK_S) == VALID)                                             \
            {                                                               \
                bRelay_Coil = SETBIT;                                       \
                relayWorkTime[relayNum].posTim = 0;                         \
            }                                                               \
            else                                                            \
            {                                                               \
                if(relayWorkTime[relayNum].posTim > cRelay_positive_TIME)   \
                {                                                           \
                    bRelay_Fault = SETBIT;                                  \
                    bRelay_Coil = SETBIT;                                   \
                }                                                           \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            relayWorkTime[relayNum].posTim = 0;                             \
            if((bK_S) == VALID)                                             \
            {                                                               \
                if(relayWorkTime[relayNum].revTim > cRelay_reverse_TIME)    \
                {                                                           \
                    bRelay_Fault = SETBIT;                                  \
                }                                                           \
            }                                                               \
            else                                                            \
            {                                                               \
                relayWorkTime[relayNum].revTim = 0;                         \
            }                                                               \
        }                                                                   \
    }                                                                       \
}while(0)


#define Relay_Fault_Test(K_PN,bK_S,Relay_TIME_positive,Relay_TIME_reverse,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((K_PN))                                                  \
        {                                                           \
            Relay_TIME_reverse = 0;                                 \
            if((bK_S) == VALID)                                     \
            {                                                       \
                bRelay_Coil = SETBIT;                               \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                    bRelay_Coil = SETBIT;                           \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
            if((bK_S) == VALID)                                     \
            {                                                       \
                if(Relay_TIME_reverse > cRelay_reverse_TIME)        \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                }                                                   \
            }                                                       \
            else                                                    \
            {                                                       \
                Relay_TIME_reverse = 0;                             \
            }                                                       \
        }                                                           \
    }                                                               \
}while(0)

//检测方法: 如果该继电器所在线路开点(有端压)接通,则该继电器应该触发;如果规定时间内没触发 则报错
#define Relay_Fault_Test_ByCircuitry_connect(k_test,bK_S,Relay_TIME_positive,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((k_test))                                                \
        {                                                           \
            if((bK_S) == VALID)                                     \
            {                                                       \
                bRelay_Coil = SETBIT;                               \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                    bRelay_Coil = SETBIT;                           \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
        }                                                           \
    }                                                               \
}while(0)

//检测方法: 如果该继电器所在线路闭点（没端电压）接通,则该继电器应该触发;如果规定时间内没触发 则报错
#define Relay_Fault_Test_ByCircuitry_noPow_connect(k_test,bK_S,Relay_TIME_positive,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((k_test))                                                \
        {                                                           \
            if((bK_S) == VALID)                                     \
            {                                                       \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
        }                                                           \
    }                                                               \
}while(0)

//检测方法: 如果该继电器所在线路 没有 接通;如果规定时间内都在触发 则报错
#define Relay_Fault_Test_ByCircuitry_Unconnect(k_test,bK_S,Relay_TIME_reverse,bRelay_Coil,bRelay_Fault) \
do{                                                             \
    {                                                           \
        if((k_test))                                            \
        {                                                       \
            if((bK_S) == VALID)                                 \
            {                                                   \
                if(Relay_TIME_reverse > cRelay_reverse_TIME)    \
                {                                               \
                    bRelay_Fault = SETBIT;                      \
                }                                               \
            }                                                   \
            else                                                \
            {                                                   \
                Relay_TIME_reverse = 0;                         \
            }                                                   \
        }                                                       \
        else                                                    \
        {                                                       \
            Relay_TIME_reverse = 0;                             \
        }                                                       \
    }                                                           \
}while(0)


//检测方法: 如果该继电器没有连接或出错标志，则由自身的继电器触点置触点开标志
#define Relay_trigger(k_test,bK_S,bRelay_Coil)                  \
do{                                                             \
    {                                                           \
        if((k_test))                                            \
        {                                                       \
            if((bK_S) == VALID)                                 \
            {                                                   \
                bRelay_Coil = SETBIT;                           \
            }                                                   \
        }                                                       \
    }                                                           \
}while(0)


//检测方法：条件成立，延时之后，则立对应的标志
#define TrueSetFLAG(k_test,Delay_TIME,cTIME,bFlag)              \
    {                                                           \
            if((k_test))                                        \
            {                                                   \
                    if(Delay_TIME > cTIME)                      \
                    {                                           \
                            bFlag = SETBIT;                     \
                    }                                           \
            }                                                   \
            else                                                \
            {                                                   \
                    Delay_TIME = 0;                             \
                    bFlag      = 0;                             \
            }                                                   \
    }                                                           


#if 0
    BOOL fault = FALSE;
    static int16u TimeNG, TimeOK;
    if(SRelay_K0_PN      == VALID)
    {
        if(SRelay_K0_NO != VALID)
        {
            fault = TRUE;
        }
        else if((SRelay_K10_NO == INVALID) && (SRelay_K20_NO == INVALID) && (SRelay_K30_NC == VALID))
        {
            fault = TRUE;
        }
    }
    else
    {
        if(SRelay_K0_NO == VALID)
        {
            fault = TRUE;
        }        
        else if((SRelay_K10_NO == VALID) || (SRelay_K20_NO == VALID) || (SRelay_K30_NC == INVALID))
        {
            fault = TRUE;
        }
    }
    if(fault)
    {
        TimeOK = TgtGetTickCount();
        if((int16u)(TgtGetTickCount() - TimeNG > 1000))
        {
            bSIGRelay_K0_Fault = 1;
        }
        
    }
    else
    {
        TimeNG = TgtGetTickCount();
        if((int16u)(TgtGetTickCount() - TimeOK > 1000))
        {
            bSIGRelay_K0_Fault = 0;
        }
    }
#endif

//--------------------------------------
EXTN void PCA9505_data_deal(void);           //数据处理
EXTN void time(void);                        //定时处理
EXTN int8u cmpTime(int8u time1_hour, int8u time1_minute, int8u time2_hour, int8u time2_minute);

//--------------------------------------
#endif






