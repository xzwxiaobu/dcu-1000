
#include <string.h>
#include "_cpu.h"
#include "..\Include.h"
#include "IO_Handle.h"
#include "CanTgt.h"

#include "IOLogic.h"
#include "SF_IODef.h"
#include "BASSys_Reg.h"

/* 注意：切换板用 FD_SF_SW_V1: 修改了硬件问题
 *  */

tExtIO  log1PCBIO;                  /* 逻辑板 LOG1 的IO数据 */
tExtIO  log2PCBIO;                  /* 逻辑板 LOG2 的IO数据 */

/* 本地临时变量 */
tChkOutputCmd MON_CmdMon;           /* 监视板的命令监视点   */
tChkOutputCmd LOGOut_Relay;         /* 逻辑板继电器输出状态 */

BIT8 TempErrMsg;
#define bMON_CtrlCmd_Fault          TempErrMsg.B7   /* 命令缺失开关门、开关边门       */
#define bMON_OUTOPNCLS_Fault        TempErrMsg.B6   /* MON-J4-2/3/14/15: 命令缺失       */
#define bMON_OUTOPNSD_Fault         TempErrMsg.B5   /* MON-J4-4/5/16/17: 开关边门缺失   */

#define RESET_CHECKINGTIME(inx)     (checkingTim[inx].posTim = 0)

extern BOOL chkDelayProcessIO(void);
int8u checkSysTimeForLightBelt(void);

/* 处理 IO 数据信息 */
void PCA9505_data_deal(void)
{
    int i, j;
    tAlarmFlags Temp_Alarm0;

    tOutputCmd  OCmd;
    tSlaveInfo * pSlvInfo;

    int8u   *p1, *p2;
    tExtIO  *pLogIO;

    TempErrMsg.byte = 0;

    #if 1   /* 代码折叠：初始化 */
    /* 报警状态初始化 */
    Temp_Alarm0.word     = 0;

    /* 先清除发送给PEDC的信息
     * */
    for(i = 0; i < AVR_DATA_LEN; i ++)
    {
        MESG_BUF[i].byte = 0;
    }

    /* IO 输出默认都是 高电平 1
     * */
    for(i = 0; i < OUTPUT_DATA_LEN; i ++)
    {
        MON_output_data[i].byte = 0xff;
    }

    /* 复位后等待延迟时间 */
    if(chkDelayProcessIO() == FALSE)
    {
        return;
    }

    /* 复制 逻辑板1的 IO 数据 */
    p1 = (int8u *)&log1PCBIO;
    p2 = (int8u *)&LOG1_input_data;
    for(i=0; i<sizeof(log1PCBIO); i++)
    {
        *p1 = *p2;
        p1++;
        p2++;
    }

    /* 复制 逻辑板2的 IO 数据 */
    p1 = (int8u *)&log2PCBIO;
    p2 = (int8u *)&LOG2_input_data;
    for(i=0; i<sizeof(log2PCBIO); i++)
    {
        *p1 = *p2;
        p1++;
        p2++;
    }
    #endif

    /* 检测通讯节点配置错误 */
    for(j = 0; j < DCUsetting_LEN; j ++)
    {
        if(uDCUsetting.settingBUF[j] != uDCUsetting.ReadSettingBUF[j])
        {
            bPedc_NodeCfgErr   = 1;
        }
    }

    /* DCU ID 冲突信息 */
    for(j = 0; j < sizeof(CanManage.IDConflict); j ++)
    {
        if(CanManage.IDConflict[j] != 0)
        {
            bPedc_DCUIDConflicted = 1;
            break;
        }
    }

    /* 给 MMS的 BAS 状态 */
    bBASSys_OffLine     = !getBASSysOnlineSts();      /* MMS 1表示离线，PEDC 1表在线，所以取反给 MMS */
    
    tWindValveSysSts * pWVSts = getWindValveSysSts();
    bBASOpnCmd      = pWVSts->bBASOpnCmd_sts;
    bBASClsCmd      = pWVSts->bBASClsCmd_sts;
    bPSC_OpnValve   = pWVSts->bPSCOpnCmd_sts;
    bPSC_ClsValve   = pWVSts->bPSCClsCmd_sts;
    
    tWindValveResult * pWVResult = getWindValveResult();
    bPEDC_WVAllClsed        = pWVResult->bWindAllClsed;
    bPEDC_WVAllOpned        = pWVResult->bWindAllOpned;
    bPEDC_OpnClsOverTime    = 0;//pWVResult->bWindOpnCls_OT;
    
    /* 工作板或芯片在线状态 */
    bPEDC_PIC24A_OFFLINE = (getObjCommLinkSts(COMMOBJ_IORW_A))  ? 0: 1; /* PIC24A 离线标志 */
    bPEDC_PIC24B_OFFLINE = (getObjCommLinkSts(COMMOBJ_IORW_B))  ? 0: 1; /* PIC24B 离线标志 */
    bPEDC_SWCH_OFFLINE   = (getObjCommLinkSts(COMMOBJ_SWITCHER))? 0: 1; /* 切换板离线标志  */
    bPEDC_LOG1_OFFLINE   = (getObjCommLinkSts(COMMOBJ_LOG1)  )  ? 0: 1; /* LOG1   离线标志 */
    bPEDC_LOG2_OFFLINE   = (getObjCommLinkSts(COMMOBJ_LOG2)  )  ? 0: 1; /* LOG2   离线标志 */
    
    #if 1   /* 代码折叠 : 逻辑板选择 */
    bLOG_Sw_Auto            = GET_INPUT_STS(MI_LOGAutoMode);    /* 选择逻辑板：自动  */
    bLOG_Sw_LOG1            = GET_INPUT_STS(MI_LOG1_Switch);    /* 选择逻辑板：手动1 */
    bLOG_Sw_LOG2            = GET_INPUT_STS(MI_LOG2_Switch);    /* 选择逻辑板：手动2 */

    int8u chkSwitchErr = (int8u)bLOG_Sw_Auto + (int8u)bLOG_Sw_LOG1 + (int8u)bLOG_Sw_LOG2;

    /* 逻辑板开关选择至少要有一个，否则报错, 为了避免旋转时误报，需要持续时间较长(6s) */
    TrueSetFLAG((chkSwitchErr != 1)  , checkingTim[TIM_LOG1LOG2_SELECT].posTim, TIME_VALUE_BASE_10(6000), MON_LOG1LOG2_SELECT);


    /* PEDC 选择的逻辑板
     *  */
    if(getWorkLOGID() == COMMOBJ_LOG1)
    {
        MO_LOG_Q2_LOG2      = 1;                /* 监视板IO输出1: 选择逻辑板1(ID=1) */
    }
    else
    {
        MO_LOG_Q2_LOG2      = 0;                /* 监视板IO输出0: 选择逻辑板2(ID=2) */
    }

    BOOL clr_TIM_SWCH_Q1AQ2A_LOST = TRUE;
    if(bPEDC_SWCH_OFFLINE == 0)
    {
        /* 切换板在线，用 K1常闭点判断，
         * 电气上，常闭点接 逻辑板1(IN-B0 1 2 ...)
         * 若触点动作表示选择 逻辑板2 */
        bLOG_WorkLOG2 = GET_INPUT_STS_1(SWCH_K1_NC);

        /* 自动模式下用 切换板的继电器动作判断
         * 监视板输出控制切换板的命令线，与当前切换板实际的继电器组(K1~K6)
         *      MO_LOG_Q2_LOG2      bLOG_WorkLOG2
         *      1=LOG1              0=LOG1
         *      0=LOG2              1=LOG2
         *  */
        if(bLOG_Sw_Auto)
        {           /* 监视板选择 LOG x， 切换板不是 LOG x，报错 */
            if((MO_LOG_Q2_LOG2 == bLOG_WorkLOG2))
            {
                TrueSetFLAG(((MO_LOG_Q2_LOG2 == bLOG_WorkLOG2))  , checkingTim[TIM_SWCH_Q1AQ2A_LOST].posTim, 120, SWCH_LinkErr_Q1AQ2A_LOST);
                clr_TIM_SWCH_Q1AQ2A_LOST = FALSE;
            }
        }
    }
    else
    {
        /* 不是真实反映选择的逻辑板 */
        bLOG_WorkLOG2 = !MO_LOG_Q2_LOG2;
    }
    
    if(clr_TIM_SWCH_Q1AQ2A_LOST == TRUE)
        RESET_CHECKINGTIME(TIM_SWCH_Q1AQ2A_LOST);
    #endif

    #if 1   /* 代码折叠 : 安全回路切换板 */
    /* 安全回路切换板 */
    if(bPEDC_SWCH_OFFLINE == 0)
    {
        /* 所有门关闭且锁定 */
        bPedc_CLSLCK    = GET_INPUT_STS(SWCH_K11_POW);

        /* ===== 板载继电器状态 ===== */
        /* 继电器 状态(两个位表示)  00          01        10          11
         *                        (断开正常)   吸合      故障        无
         * */
        Relay_Fault_Check((SWCH_K0_POW  == VALID)   , !SWCH_K0_NC    , SWCH_K0 , bSWCH_K0_Coil , bSWCH_K0_Fault  );

        Relay_Fault_Check((SWCH_K1_POW  == VALID)   , !SWCH_K1_NC    , SWCH_K1 , bSWCH_K1_Coil , bSWCH_K1_Fault  );
        Relay_Fault_Check((SWCH_K2_POW  == VALID)   , !SWCH_K2_NC    , SWCH_K2 , bSWCH_K2_Coil , bSWCH_K2_Fault  );
        Relay_Fault_Check((SWCH_K3_POW  == VALID)   , !SWCH_K3_NC    , SWCH_K3 , bSWCH_K3_Coil , bSWCH_K3_Fault  );

        Relay_Fault_Check((SWCH_K4_POW  == VALID)   , !SWCH_K4_NC    , SWCH_K4 , bSWCH_K4_Coil , bSWCH_K4_Fault  );
        Relay_Fault_Check((SWCH_K5_POW  == VALID)   , !SWCH_K5_NC    , SWCH_K5 , bSWCH_K5_Coil , bSWCH_K5_Fault  );
        Relay_Fault_Check((SWCH_K6_POW  == VALID)   , !SWCH_K6_NC    , SWCH_K6 , bSWCH_K6_Coil , bSWCH_K6_Fault  );

        Relay_Fault_Check((SWCH_K11_POW  == VALID)  , !SWCH_K11_NC   , SWCH_K11 , bSWCH_K11_Coil , bSWCH_K11_Fault  );
        Relay_Fault_Check((SWCH_K12_POW  == VALID)  , !SWCH_K12_NC   , SWCH_K12 , bSWCH_K12_Coil , bSWCH_K12_Fault  );

        Relay_Fault_Check(((SWCH_K11_NC  == VALID) && (SWCH_K12_NC == VALID))  ,  SWCH_K13_NO   , SWCH_K13 , bSWCH_K13_Coil , bSWCH_K13_Fault  );

        //Relay_Fault_Check((SWCH_K21_POW  == VALID)  ,  SWCH_K21_NO   , SWCH_K21 , bSWCH_K21_Coil , bSWCH_K21_Fault  );
        //Relay_Fault_Check((SWCH_K22_POW  == VALID)  ,  SWCH_K22_NO   , SWCH_K22 , bSWCH_K22_Coil , bSWCH_K22_Fault  );

        /* 安全回路错误?  */
        if(bSWCH_K11_Fault || bSWCH_K12_Fault || bSWCH_K13_Fault || SF_FAULT)
        {
            bSWCH_SF_Fault = SETBIT;
        }

        /* 安全回路导通
         * 则 K11K12 通电(SWCH_K11_POW)，K13断电（检测常开点SWCH_K13_NO）
         *   */
        bSWCH_SF_Coil = GET_INPUT_STS(SWCH_K11_POW);
    }
    else
    {
        int k;
        for(k = SWCH_K0; k<=SWCH_K23; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif

    /*===== 逻辑板 1===== */
    #if 1   /* 代码折叠 : 逻辑板 1 继电器状态 */
    if(bPEDC_LOG1_OFFLINE == 0)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
        bPEDC_LOG1_TestMode_En = pSlvInfo->slaveInput.logIn.testRunSts.testEnable;
        bPEDC_LOG1_TestRunning = pSlvInfo->slaveInput.logIn.testRunSts.isRunning;

        pLogIO = &log1PCBIO;

        /* ===== 继电器状态 ===== */
        /* 继电器 状态(两个位表示)  00          01        10          11
         *                        (断开正常)   吸合      故障        无
         * */
        Relay_Fault_Check((pLogIO->relay.K0_PN == VALID)   , pLogIO->relay.K0_NO    , LOG1_K0  , bLOG1_K0_Coil  , bLOG1_K0_Fault   );

        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1A_NO   , LOG1_K1A , bLOG1_K1A_Coil , bLOG1_K1A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1B_NO   , LOG1_K1B , bLOG1_K1B_Coil , bLOG1_K1B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2A_NO   , LOG1_K2A , bLOG1_K2A_Coil , bLOG1_K2A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2B_NO   , LOG1_K2B , bLOG1_K2B_Coil , bLOG1_K2B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3A_NO   , LOG1_K3A , bLOG1_K3A_Coil , bLOG1_K3A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3B_NO   , LOG1_K3B , bLOG1_K3B_Coil , bLOG1_K3B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4A_NO   , LOG1_K4A , bLOG1_K4A_Coil , bLOG1_K4A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4B_NO   , LOG1_K4B , bLOG1_K4B_Coil , bLOG1_K4B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5A_NO   , LOG1_K5A , bLOG1_K5A_Coil , bLOG1_K5A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5B_NO   , LOG1_K5B , bLOG1_K5B_Coil , bLOG1_K5B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6A_NO   , LOG1_K6A , bLOG1_K6A_Coil , bLOG1_K6A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6B_NO   , LOG1_K6B , bLOG1_K6B_Coil , bLOG1_K6B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7A_NO   , LOG1_K7A , bLOG1_K7A_Coil , bLOG1_K7A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7B_NO   , LOG1_K7B , bLOG1_K7B_Coil , bLOG1_K7B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8A_NO   , LOG1_K8A , bLOG1_K8A_Coil , bLOG1_K8A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8B_NO   , LOG1_K8B , bLOG1_K8B_Coil , bLOG1_K8B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K21_PN == VALID) , !pLogIO->relay.K21_NC   , LOG1_K21 , bLOG1_K21_Coil , bLOG1_K21_Fault  );
        Relay_Fault_Check((pLogIO->relay.K22_PN == VALID) ,  pLogIO->relay.K22_NO   , LOG1_K22 , bLOG1_K22_Coil , bLOG1_K22_Fault  );
        Relay_Fault_Check((pLogIO->relay.K30_PN == VALID) , !pLogIO->relay.K30_NC   , LOG1_K30 , bLOG1_K30_Coil , bLOG1_K30_Fault  );

        Relay_Fault_Check(((pLogIO->fun.JI_PSL1_EN == VALID) || (pLogIO->fun.JI_PSL2_EN == VALID) || (pLogIO->fun.JI_PSL3_EN == VALID)) ,  \
                        !pLogIO->relay.K10_NC   , LOG1_K10 , bLOG1_K10_Coil , bLOG1_K10_Fault  );

    }
    else
    {
        int k;
        for(k = LOG1_K0; k<=LOG1_K30; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif


    /*===== 逻辑板 2===== */
    #if 1   /* 代码折叠 : 逻辑板 2 继电器状态 */
    if(bPEDC_LOG2_OFFLINE == 0)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
        bPEDC_LOG2_TestMode_En = pSlvInfo->slaveInput.logIn.testRunSts.testEnable;
        bPEDC_LOG2_TestRunning = pSlvInfo->slaveInput.logIn.testRunSts.isRunning;

        pLogIO = &log2PCBIO;

        /* ===== 继电器状态 ===== */
        /* 继电器 状态(两个位表示)  00          01        10          11
         *                        (断开正常)   吸合      故障        无
         * */
        Relay_Fault_Check((pLogIO->relay.K0_PN == VALID)   , pLogIO->relay.K0_NO    , LOG2_K0  , bLOG2_K0_Coil  , bLOG2_K0_Fault   );

        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1A_NO   , LOG2_K1A , bLOG2_K1A_Coil , bLOG2_K1A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1B_NO   , LOG2_K1B , bLOG2_K1B_Coil , bLOG2_K1B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2A_NO   , LOG2_K2A , bLOG2_K2A_Coil , bLOG2_K2A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2B_NO   , LOG2_K2B , bLOG2_K2B_Coil , bLOG2_K2B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3A_NO   , LOG2_K3A , bLOG2_K3A_Coil , bLOG2_K3A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3B_NO   , LOG2_K3B , bLOG2_K3B_Coil , bLOG2_K3B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4A_NO   , LOG2_K4A , bLOG2_K4A_Coil , bLOG2_K4A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4B_NO   , LOG2_K4B , bLOG2_K4B_Coil , bLOG2_K4B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5A_NO   , LOG2_K5A , bLOG2_K5A_Coil , bLOG2_K5A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5B_NO   , LOG2_K5B , bLOG2_K5B_Coil , bLOG2_K5B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6A_NO   , LOG2_K6A , bLOG2_K6A_Coil , bLOG2_K6A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6B_NO   , LOG2_K6B , bLOG2_K6B_Coil , bLOG2_K6B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7A_NO   , LOG2_K7A , bLOG2_K7A_Coil , bLOG2_K7A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7B_NO   , LOG2_K7B , bLOG2_K7B_Coil , bLOG2_K7B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8A_NO   , LOG2_K8A , bLOG2_K8A_Coil , bLOG2_K8A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8B_NO   , LOG2_K8B , bLOG2_K8B_Coil , bLOG2_K8B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K21_PN == VALID) , !pLogIO->relay.K21_NC   , LOG2_K21 , bLOG2_K21_Coil , bLOG2_K21_Fault  );
        Relay_Fault_Check((pLogIO->relay.K22_PN == VALID) ,  pLogIO->relay.K22_NO   , LOG2_K22 , bLOG2_K22_Coil , bLOG2_K22_Fault  );
        Relay_Fault_Check((pLogIO->relay.K30_PN == VALID) , !pLogIO->relay.K30_NC   , LOG2_K30 , bLOG2_K30_Coil , bLOG2_K30_Fault  );

        Relay_Fault_Check(((pLogIO->fun.JI_PSL1_EN == VALID) || (pLogIO->fun.JI_PSL2_EN == VALID) || (pLogIO->fun.JI_PSL3_EN == VALID)) ,  \
                        !pLogIO->relay.K10_NC   , LOG2_K10 , bLOG2_K10_Coil , bLOG2_K10_Fault  );
    }
    else
    {
        int k;
        for(k = LOG2_K0; k<=LOG2_K30; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif

    /* ===== 检查板间连线状态 =====
     * 注意这部分代码需要用到 逻辑板的 bPEDC_LOG1_TestMode_En bPEDC_LOG2_TestMode_En
     *  */
    #if 1   /* 代码折叠 : 判断逻辑板1、逻辑板2、监视板 板间连线信息 */
    BOOL  linkErrFlag;
    int8u lnkMsg = PCBLINK_ERR_NONE;

    tLinkSts    log1Link, log2Link;
    tLinkSts    compLink;
    int8u       log1IBPPSL_En,log2IBPPSL_En;

    tLinkSts monLink;

    for(i=0; i<sizeof(tLinkSts); i++)
    {
        log1Link.bytes[i] = 0;
        log2Link.bytes[i] = 0;
        compLink.bytes[i] = 0;
        monLink.bytes [i] = 0;
    }

    /* 监视板输入命令 : 只有 信号系统开关门
     *
     *  */
    monLink.LinkBit.SIG_Opn         = GET_INPUT_STS(MI_SIG_OpnCmd);
    monLink.LinkBit.SIG_Cls         = GET_INPUT_STS(MI_SIG_ClsCmd);

    /* 逻辑板1输入命令 */
    log1Link.LinkBit.IBP_En         = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_EN);
    log1Link.LinkBit.IBP_Opn        = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_OPN8);
    log1Link.LinkBit.IBP_Cls        = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_CLS8) ;
    log1Link.LinkBit.IBP_OpnSD_En   = log1Link.LinkBit.IBP_En;/* GET_INPUT_STS(log1PCBIO.fun.JI_IBP_A_EN); */
    log1Link.LinkBit.IBP_OpnSD      = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_OPN6);

    log1Link.LinkBit.PSL1_En        = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_EN);
    log1Link.LinkBit.PSL1_Opn       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_OPN8);
    log1Link.LinkBit.PSL1_Cls       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_CLS8);

    log1Link.LinkBit.PSL2_En        = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_EN);
    log1Link.LinkBit.PSL2_Opn       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_OPN8);
    log1Link.LinkBit.PSL2_Cls       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_CLS8);

    log1Link.LinkBit.SIG_Opn        = GET_INPUT_STS(log1PCBIO.fun.JI_SIG_OPN8);
    log1Link.LinkBit.SIG_Cls        = GET_INPUT_STS(log1PCBIO.fun.JI_SIG_CLS);
    log1IBPPSL_En   = log1Link.LinkBit.IBP_En | log1Link.LinkBit.PSL1_En | log1Link.LinkBit.PSL2_En;

    /* 逻辑板2输入命令 */
    log2Link.LinkBit.IBP_En         = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_EN);
    log2Link.LinkBit.IBP_Opn        = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_OPN8);
    log2Link.LinkBit.IBP_Cls        = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_CLS8);
    log2Link.LinkBit.IBP_OpnSD_En   = log2Link.LinkBit.IBP_En;  /* GET_INPUT_STS(log2PCBIO.fun.JI_IBP_A_EN); */
    log2Link.LinkBit.IBP_OpnSD      = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_OPN6);

    log2Link.LinkBit.PSL1_En        = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_EN);
    log2Link.LinkBit.PSL1_Opn       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_OPN8);
    log2Link.LinkBit.PSL1_Cls       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_CLS8);

    log2Link.LinkBit.PSL2_En        = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_EN);
    log2Link.LinkBit.PSL2_Opn       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_OPN8);
    log2Link.LinkBit.PSL2_Cls       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_CLS8);

    log2Link.LinkBit.SIG_Opn        = GET_INPUT_STS(log2PCBIO.fun.JI_SIG_OPN8);
    log2Link.LinkBit.SIG_Cls        = GET_INPUT_STS(log2PCBIO.fun.JI_SIG_CLS);
    log2IBPPSL_En = log2Link.LinkBit.IBP_En | log2Link.LinkBit.PSL1_En | log2Link.LinkBit.PSL2_En;

    /* 逻辑板1 和 监视板 的连线
     * 本项目只有 信号开关门命令
     *  */
    if((bPEDC_LOG1_OFFLINE == 0))
    {
        compLink.bytes[0] = log1Link.bytes[0] ^ monLink.bytes[0];
        compLink.bytes[1] = log1Link.bytes[1] ^ monLink.bytes[1];
        lnkMsg = PCBLINK_ERR_NONE;

        /* 判断 SIG 信号命令时需要考虑 IBP PSL PSC 使能信号切掉了逻辑板的 SIG开关门命令 */
        {
            if((log1IBPPSL_En == 0) && (bPEDC_LOG1_TestMode_En == 0))
            {
                if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
                if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
            }
        }


        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG1MON].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG1MONLinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG1MONLinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG1MON);
    }

    /* 逻辑板2 和 监视板 的连线
     * 本项目只有 信号开关门命令
     *  */
    if((bPEDC_LOG2_OFFLINE == 0))
    {
        compLink.bytes[0] = log2Link.bytes[0] ^ monLink.bytes[0];
        compLink.bytes[1] = log2Link.bytes[1] ^ monLink.bytes[1];
        lnkMsg = PCBLINK_ERR_NONE;

        /* 判断 SIG 信号命令时需要考虑 IBP PSL PSC 使能信号切掉了逻辑板的 SIG开关门命令 */
        {
            if((log2IBPPSL_En == 0) && (bPEDC_LOG2_TestMode_En == 0))
            {
                if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
                if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
            }
        }

        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG2MON].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG2MONLinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG2MONLinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG2MON);
    }

    /* 逻辑板1 和 逻辑板2 的连线 */
    if((bPEDC_LOG1_OFFLINE == 0) && (bPEDC_LOG2_OFFLINE == 0))
    {
        compLink.bytes[0] = log1Link.bytes[0] ^ log2Link.bytes[0];
        compLink.bytes[1] = log1Link.bytes[1] ^ log2Link.bytes[1];

        if(compLink.LinkBit.IBP_En           != 0)  {lnkMsg = PCBLINK_ERR_IBP_EN;        }
        else if(compLink.LinkBit.IBP_Opn     != 0)  {lnkMsg = PCBLINK_ERR_IBP_OPN;       }
        else if(compLink.LinkBit.IBP_Cls     != 0)  {lnkMsg = PCBLINK_ERR_IBP_CLS;       }
        else if(compLink.LinkBit.IBP_OpnSD   != 0)  {lnkMsg = PCBLINK_ERR_IBP_OPNSIDE;   }

        else if(compLink.LinkBit.PSL1_En     != 0)  {lnkMsg = PCBLINK_ERR_PSL1_EN;       }
        else if(compLink.LinkBit.PSL1_Opn    != 0)  {lnkMsg = PCBLINK_ERR_PSL1_OPN;      }
        else if(compLink.LinkBit.PSL1_Cls    != 0)  {lnkMsg = PCBLINK_ERR_PSL1_CLS;      }

        else if(compLink.LinkBit.PSL2_En     != 0)  {lnkMsg = PCBLINK_ERR_PSL2_EN;       }
        else if(compLink.LinkBit.PSL2_Opn    != 0)  {lnkMsg = PCBLINK_ERR_PSL2_OPN;      }
        else if(compLink.LinkBit.PSL2_Cls    != 0)  {lnkMsg = PCBLINK_ERR_PSL2_CLS;      }

        else if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
        else if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
        else                                        {lnkMsg = PCBLINK_ERR_NONE;          }

        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG1LOG2].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG1LOG2LinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG1LOG2LinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG1LOG2);
    }
    #endif



    /* ===== 工作逻辑板的输出命令 ===== */
    #if 1 /* 折叠代码 : 工作逻辑板的输出命令 */
    if(bLOG_WorkLOG2 == 0)  /* 不用 getWorkLOGID() == COMMOBJ_LOG1，以免手动选择时出错 */
    {
        pLogIO     = &log1PCBIO;
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
    }
    else
    {
        pLogIO     = &log2PCBIO;
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
    }

    /* 软件控制逻辑是否有效输出?
     * 是则获取软件控制逻辑的操作控制信息
     *  */
    memcpy((int8u *) &OCmd, (int8u *)&pSlvInfo->slaveInput.logIn.UB1_outLogic, sizeof(OCmd));
    if(pSlvInfo->slaveInput.logIn.UB1_OutLogicNo != NONE_LOGICOUT)
    {
        switch((tLogicType)OCmd.outType)
        {
            /* IBP 输出
             * 软件逻辑输出 */
            case IBP_LOGIC:
            {
                bPedcIBP_EN         = OCmd.outCmd.Enable;
                bPedcIBP_CLS        = OCmd.outCmd.DCUCLS8;  /* 关整侧门含关边门 */
                bPedcIBP_OPN8       = OCmd.outCmd.DCUOPN8;  /* 开整侧门         */
                bPedcIBP_OPNSD      = OCmd.outCmd.DCUOPN6;  /* 用 OPN6 开边门   */
            }
            break;

            /* PSL 输出 */
            case PSL_LOGIC:
            {
                switch(OCmd.outSeq)
                {
                    case 0:         /* PSL1 输出 : 使能、关门、开门 */
                        bPedcPSL1_EN    = OCmd.outCmd.Enable;
                        bPedcPSL1_CLS   = OCmd.outCmd.DCUCLS8;
                        bPedcPSL1_OPN8  = OCmd.outCmd.DCUOPN8;
                        break;

                    case 1:         /* PSL2 输出 : 使能、关门、开门 */
                        bPedcPSL2_EN    = OCmd.outCmd.Enable;
                        bPedcPSL2_CLS   = OCmd.outCmd.DCUCLS8;
                        bPedcPSL2_OPN8  = OCmd.outCmd.DCUOPN8;
                        break;

                    case 2:         /* PSL3 输出 : 使能、关门、开门 */
                        break;

                    default:
                        break;
                }
            }
            break;

            /* PSC 输出 */
            case PSC_LOGIC:
            {
            }
            break;

            default:
            {
            }
            break;
        }
    }
    else
    {
        /* 软件控制逻辑没有 IBP PSL 输出，则默认当前是 SIG 控制 */
    }

    /* PEDC 系统的输出，即选择的逻辑板输出给到DCU的硬线命令
     * 是逻辑板上继电器的触点状态 */
    LOGOut_Relay.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* 关门命令1    DCU_CLS8_1 */
    LOGOut_Relay.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* 开门命令1    DCU_OPN8_1 */
    LOGOut_Relay.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* 关门命令2    DCU_CLS8_2 */
    LOGOut_Relay.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* 开门命令2    DCU_OPN8_2 */
    LOGOut_Relay.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* 关边门1                 */
    LOGOut_Relay.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* 开边门1                 */
    LOGOut_Relay.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* 关边门2                 */
    LOGOut_Relay.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* 开边门2                 */

    /* bPedc_OUT_CLS6_1  bPedc_OUT_CLS6_2 ... */
    PEDC_PSC1.byte = LOGOut_Relay.byte;

    BOOL IBPPSL_En = bPedcIBP_EN || bPedcPSL1_EN || bPedcPSL2_EN;
    #endif

    /*===== 信号系统开关门命令 =====*/
    #if 1   /* 代码折叠 : 信号开关命令 */
    /* 监视板接原始输入
     *  : MI_SIG_OpnCmd / MI_SIG_ClsCmd
     */
    /*
    if((IBPPSLPSC_En == 0))
     *  */
    {
        bPedcSIG_OPN8   = GET_INPUT_STS(MI_SIG_OpnCmd);
        bPedcSIG_CLS    = GET_INPUT_STS(MI_SIG_ClsCmd);

        //如果没有 监视板监视点，就只能用逻辑板的输入
        //bPedcSIG_OPN8   = GET_INPUT_STS(L1_JI_SIG_OPN8) | GET_INPUT_STS(L2_JI_SIG_OPN8);
        //bPedcSIG_CLS    = GET_INPUT_STS(L1_JI_SIG_CLS)  | GET_INPUT_STS(L1_JI_SIG_CLS);

        /* 检测信号系统硬线开关门，开关门同时命令有效，则设置错误 */
        TrueSetFLAG((bPedcSIG_CLS && bPedcSIG_OPN8) ,checkingTim[TIM_LOG_SIGCMDERR].posTim, 200, bPedc_ATCCMD_FAILURE);
    }

    #endif

    #if 1 /* 代码折叠: PSC 柜输入输出 */
    /*===== PSC 面板 =====*/
    /* PSL旁路开关 */
    bPedcPSL1_ByPass = GET_INPUT_STS(MI_PSL1_Bypass);     /* PSL1 BYPASS  */
    bPedcPSL2_ByPass = GET_INPUT_STS(MI_PSL2_Bypass);     /* PSL2 BYPASS  */

    /* 电源故障检测 */
    PEDC_POWER_WARNING.byte = 0;
    bPowerWarning_S0        = GET_INPUT_STS(MI_MainPowFail);    /* POW-S0  主电源故障     */
    bPowerWarning_S1        = GET_INPUT_STS(MI_DriverPowFail);  /* POW-S1  驱动电源故障   */
    bPowerWarning_S2        = GET_INPUT_STS(MI_CtrlPowFail);    /* POW-S2  控制电源故障   */
    bPowerWarning_S3        = GET_INPUT_STS(MI_DriverBatFail);  /* POW-S3  驱动电池故障   */
    bPowerWarning_S4        = GET_INPUT_STS(MI_CtrlBatFail);    /* POW-S4  控制电池故障   */

    /* 电源报警状态   */
    if( PEDC_POWER_WARNING.byte != 0 )
    {
        Temp_Alarm0.bAlarm_POWER_FAULT = SETBIT;
    }

    /*===== 命令输出监视 =====*/
    /*
     * 开门命令 1、2，关门命令1、2
     * 判断故障状态: 控制系统故障 => 4条开关门命令线
     */
    /* MON 开关门命令监视点 */
    MON_CmdMon.byte = 0;
    MON_CmdMon.Cls1         = GET_INPUT_STS(MI_DCU_CLS8_1);     /* 关门命令1    DCU_CLS6_1 */
    MON_CmdMon.Opn1         = GET_INPUT_STS(MI_DCU_OPN8_1);     /* 开门命令1    DCU_OPN6_1 */
    MON_CmdMon.Cls2         = GET_INPUT_STS(MI_DCU_CLS8_2);     /* 关门命令2    DCU_CLS6_2 */
    MON_CmdMon.Opn2         = GET_INPUT_STS(MI_DCU_OPN8_2);     /* 开门命令2    DCU_OPN6_2 */
    MON_CmdMon.ClsSide1     = GET_INPUT_STS(MI_DCU_CLSSD_1);    /* 关边门1                 */
    MON_CmdMon.OpnSide1     = GET_INPUT_STS(MI_DCU_OPNSD_1);    /* 开边门1                 */
    MON_CmdMon.ClsSide2     = GET_INPUT_STS(MI_DCU_CLSSD_2);    /* 关边门2                 */
    MON_CmdMon.OpnSide2     = GET_INPUT_STS(MI_DCU_OPNSD_2);    /* 开边门2                 */
    
    /* 输出命令线
     *     1. 开关门命令
     *     2. 开关边门命令
     */
    #if 1
    int8u cmpResult;
    cmpResult = MON_CmdMon.byte ^ LOGOut_Relay.byte;
    
    TrueSetFLAG(((cmpResult & 0x0f ) !=0),  checkingTim[TIM_MON_OPNCLS_ERR].posTim  , 120, bMON_OUTOPNCLS_Fault);
    TrueSetFLAG(((cmpResult & 0xf0 ) !=0),  checkingTim[TIM_MON_IBPOPNSD_ERR].posTim, 120, bMON_OUTOPNSD_Fault);
    
    #else
    TrueSetFLAG(((LOGOut_Relay.Cls1 != MON_CmdMon.Cls1) || (LOGOut_Relay.Cls2 != MON_CmdMon.Cls2) ||    \
                 (LOGOut_Relay.Opn1 != MON_CmdMon.Opn1) || (LOGOut_Relay.Opn2 != MON_CmdMon.Opn2)),     \
                  checkingTim[TIM_MON_OPNCLS_ERR].posTim, 120, bMON_OUTOPNCLS_Fault);
    
    TrueSetFLAG(((LOGOut_Relay.ClsSide1 != MON_CmdMon.ClsSide1) || (LOGOut_Relay.ClsSide2 != MON_CmdMon.ClsSide2) ||    \
                 (LOGOut_Relay.OpnSide1 != MON_CmdMon.OpnSide1) || (LOGOut_Relay.OpnSide2 != MON_CmdMon.OpnSide2)),     \
                  checkingTim[TIM_MON_IBPOPNSD_ERR].posTim, 120, bMON_OUTOPNSD_Fault);
    #endif
    
    bMON_CtrlCmd_Fault = bMON_OUTOPNCLS_Fault | bMON_OUTOPNSD_Fault;

    /*--------------------------------------
     * PSC，PSL，IBP上的所有门关闭且锁紧指示灯
     * */
    int8u LedClsAndLocked;
    LedClsAndLocked = bPedc_CLSLCK;

    /* PEDC 故障:
     * 扩展板离线 ( bPEDC_XXX_OFFLINE )
     * PIC24离线或
     *      PIC24 I2C访问错误=> ifPCA9505Err(COMMOBJ_IORW_A) */
    BOOL PEDCFault = (bPEDC_SWCH_OFFLINE ||  (bPEDC_LOG1_OFFLINE || bPEDC_LOG2_OFFLINE) || bPEDC_PIC24A_OFFLINE || bPEDC_PIC24B_OFFLINE);
                    // (bPEDC_PIC24A_OFFLINE || ifPCA9505Err(COMMOBJ_IORW_A)) || \
                    // (bPEDC_PIC24B_OFFLINE || ifPCA9505Err(COMMOBJ_IORW_B)));

    /*--------------------------------------
     * PSC 柜盘面布置
     * */
    /* J1-02  PSC-Q00     PSC-开门状态指示灯 
     * PEDC输出开门命令且至少一道门是处于开启状态时点亮
     * 不是简单用 安全回路 通断来指示: MO_Led_PSC_OpnSta       = LEDON_IF_STS0(bPedc_CLSLCK);
     *                  */
    if(MON_CmdMon.Opn1 || MON_CmdMon.Opn2 || MON_CmdMon.OpnSide1 || MON_CmdMon.OpnSide2)
    {
        MO_Led_PSC_OpnSta = LEDON_IF_STS0(bPedc_CLSLCK);
    }
    

    /* J1-03  PSC-Q01     PSC-所有ASD/EED关闭锁紧指示灯       */
    MO_Led_PSC_DoorLcked    = LEDON_IF_STS1(LedClsAndLocked);

    /* J1-04  PSC-Q02     PSC-滑动门手动/隔离操作指示灯       */
    MO_Led_PSC_ManUnlock    = LEDON_IF_STS1(Down_Message0.bMANUAL_MODE | Down_Message0.bIsolation_MODE);

    /* J1-05  PSC-Q03     PSC-滑动门开门/关门故障指示灯       */
    MO_Led_PSC_OpnClsFail   = LEDON_IF_STS1(Down_Message0.bOPN_FAULT | Down_Message0.bCLS_FAULT);

    /* J1-06  PSC-Q04     PSC-IBP操作允许指示灯               */
    MO_Led_PSC_IBPEn        = LEDON_IF_STS1(bPedcIBP_EN);

    /* J1-07  PSC-Q05     PSC-PSL互锁解除指示灯               */
    MO_Led_PSC_PSLByPass    = LEDON_IF_STS1(bPedcPSL1_ByPass | bPedcPSL2_ByPass);

    /* J1-08  PSC-Q06     PSC-供电电源故障指示灯              */
    MO_Led_PSC_PowerFail    = LEDON_IF_STS1(Temp_Alarm0.bAlarm_POWER_FAULT);

    /* J1-09  PSC-Q07     PSC-现场总线故障指示灯              */
    MO_Led_PSC_CanFail      = LEDON_IF_STS1(Down_Message0.bBUS_FAULT);

    /* J1-10  PSC-Q08     PSC-控制系统故障指示灯 : PEDC 故障时点亮--> PEDCFault 或 bMON_CtrlCmd_Fault */
    MO_Led_CtrlSystemFail   = LEDON_IF_STS1(bMON_CtrlCmd_Fault | PEDCFault);

    /* J1-12  PSC-Q10     PSC--监视系统故障指示灯: PEDC与工控机通信超时点亮             */
    MO_Led_MonitorSysFail   = LEDON_IF_STS1(Down_Message0.bMONITOR_FAULT);

    /* J1-13  PSC-Q11     PSC-?望灯带                        */
    MO_LookoutLight = checkSysTimeForLightBelt();
    
    /*  J2-14  Q-PN1       PSC-照明灯带(2022-4-13 修改)
     * 与?望灯带同步亮灭 */
    MO_IlluminationLight = MO_LookoutLight;
    
    /* 蜂鸣器报警信息
     *  */
    Temp_Alarm0.bAlarm_BUS_FAULT      = Down_Message0.bBUS_FAULT;
    Temp_Alarm0.bAlarm_OPN_FAULT      = Down_Message0.bOPN_FAULT;
    Temp_Alarm0.bAlarm_CLS_FAULT      = Down_Message0.bCLS_FAULT;
    Temp_Alarm0.bAlarm_Monitor_Fault  = Down_Message0.bMONITOR_FAULT;
    Temp_Alarm0.bAlarm_LOGPCB_FAULT   = (getLOGPcbState() == 0) ? 0 : 1;
    
    #if 1
    /* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。
     * 注意计时单位是 RELAY_TIMER_INTERVAL */
    static int8u prev_WorkSIG = 0xff;
    if(prev_WorkSIG == 0xff)
    {
        prev_WorkSIG = bLOG_WorkLOG2;
        checkingTim[TIM_AutoSelectLOG2].posTim = 0;
    }
    else
    {
        TrueSetFLAG(((bLOG_Sw_Auto == 1) && (prev_WorkSIG != bLOG_WorkLOG2)),  checkingTim[TIM_AutoSelectLOG2].posTim  , TIME_VALUE_BASE_10(2000), Temp_Alarm0.bAlarm_AutoSelLog2);
        bLOG1_2_SwitchAlarm = Temp_Alarm0.bAlarm_AutoSelLog2;   /* 调试: 便于 PEDC 测试软件观察 */
    }
    #else
    /* 2024-6-3 .自动位时控制板2工作要产生报警，注意计时单位约 8ms */
    TrueSetFLAG(((bLOG_Sw_Auto == 1) && (bLOG_WorkLOG2 == 1)),  checkingTim[TIM_AutoSelectLOG2].posTim  , 200, Temp_Alarm0.bAlarm_AutoSelLog2);
    #endif
    
    /* 复位按钮 /消音按钮 */
    if(MI_ResetKey == Input_VALID)
    {
        PSC_AlarmFlags.word = Temp_Alarm0.word;
        
        /* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。 */
        prev_WorkSIG = bLOG_WorkLOG2;
    }

    /* 判断：状态 从无到有，即 从0到1，则报警 */
    if(((PSC_AlarmFlags.word ^ 0xffff) & Temp_Alarm0.word ) != 0)
    {
        /* MMS, 1禁止 BUZZER 输出 ? */
        if(!FORBID_BUZZER)
        {
            MO_PSC_Buzzer = BUZZER_ON;
        }
    }
	else
	{
        /* 备份的故障消失,那么将该故障从备份中消除 */
		PSC_AlarmFlags.word = Temp_Alarm0.word;
	}
    #endif

    #if 1 /* 代码折叠 : IBP 操作面板 */
    /*--------------------------------------
     * IBP 操作面板
     * 无蜂鸣器，无消音按钮，无试灯按钮
     * */
    /*
     * 输出指示灯
     *     IBP-站台门关闭指示灯
     *     IBP-站台门开启指示灯
     *     IBP-首末门开启指示灯
     */

    /* J3-02  IBP-Q0      IBP-站台门关闭指示灯   */
    MO_Led_IBP_DoorClsed   = LEDON_IF_STS1(LedClsAndLocked);

    /* J3-03  IBP-Q1      IBP-站台门开启指示灯 (滑动门全开启时点亮 )   */
    MO_Led_IBP_DoorOpned   = LEDON_IF_STS1(Down_Message0.bDOOR_ALL_OPEN);

    /* J3-04  IBP-Q2      IBP-首末门开启指示灯
     * 2022-8-22 改为 : 首门和末门同时全开到位后点亮，即‘与’（以前是‘或’）  */
    MO_Led_IBP_HeadTailOpn   = LEDON_IF_STS1(Down_Message0.bTailASD_Opned & Down_Message0.bHeadASD_Opned);

    /* IBP 报警蜂鸣器处理 ： 无
     */

    /* IBP 复位按钮 /消音按钮 : 无
     *  */
    #endif

    #if 1 /* 代码折叠 : PSL 操作面板 */
    /*--------------------------------------
     * PSL1 & PSL2 & PSL3 操作面板
     * */
    /* 站台门全开启指示灯
     * PSL 1 2 都用这个输出点
     *  */
    MO_Led_PSLx_AllOpned  = LEDON_IF_STS1(Down_Message0.bDOOR_ALL_OPEN);

    /* 站台门全关指示灯
     * PSL 1 2 都用这个输出点
     *  */
    MO_Led_PSLx_AllClsed  = LEDON_IF_STS1(LedClsAndLocked);

    /* PSL1  */
    MO_Led_PSL1_IBPEn       = LEDON_IF_STS1(bPedcIBP_EN);       /* J2-03  PSL-Q01     PSL1-IBP操作允许指示灯    */
    MO_Led_PSL1_EN          = LEDON_IF_STS1(bPedcPSL1_EN);      /* J2-06  PSL-Q04     PSL1-操作允许指示灯       */
    MO_Led_PSL1_Bypass      = LEDON_IF_STS1(bPedcPSL1_ByPass);  /* J2-07  PSL-Q05     PSL1-互锁解除指示灯       */
    MO_Led_PSL1Interlock    = LEDON_IF_STS1(bPedcPSL2_EN);      /* J2-10  PSL-Q08     PSL1-同侧互锁指示灯       */

    /* PSL2  */
    MO_Led_PSL2_IBPEn       = LEDON_IF_STS1(bPedcIBP_EN);       /* J2-05  PSL-Q03     PSL2-IBP操作允许指示灯    */
    MO_Led_PSL2_EN          = LEDON_IF_STS1(bPedcPSL2_EN);      /* J2-08  PSL-Q06     PSL2-操作允许指示灯       */
    MO_Led_PSL2_Bypass      = LEDON_IF_STS1(bPedcPSL2_ByPass);  /* J2-09  PSL-Q07     PSL2-互锁解除指示灯       */
    MO_Led_PSL2Interlock    = LEDON_IF_STS1(bPedcPSL1_EN);      /* J2-12  PSL-Q10     PSL2-同侧互锁指示灯       */
    #endif

    #if 1 /* 试灯按钮 */
    /*--------------------------------------
     * PSC 试灯按钮
     * */
    if(MI_TestLight == Input_VALID)
    {
        MO_Led_PSC_OpnSta       = LED_ON;   /* J1-02  PSC-Q00     PSC-开门状态指示灯                  */
        MO_Led_PSC_DoorLcked    = LED_ON;   /* J1-03  PSC-Q01     PSC-所有ASD/EED关闭锁紧指示灯       */
        MO_Led_PSC_ManUnlock    = LED_ON;   /* J1-04  PSC-Q02     PSC-滑动门手动/隔离操作指示灯       */
        MO_Led_PSC_OpnClsFail   = LED_ON;   /* J1-05  PSC-Q03     PSC-滑动门开门/关门故障指示灯       */
        MO_Led_PSC_IBPEn        = LED_ON;   /* J1-06  PSC-Q04     PSC-IBP操作允许指示灯               */
        MO_Led_PSC_PSLByPass    = LED_ON;   /* J1-07  PSC-Q05     PSC-PSL互锁解除指示灯               */
        MO_Led_PSC_PowerFail    = LED_ON;   /* J1-08  PSC-Q06     PSC-供电电源故障指示灯              */
        MO_Led_PSC_CanFail      = LED_ON;   /* J1-09  PSC-Q07     PSC-现场总线故障指示灯              */
        MO_Led_CtrlSystemFail   = LED_ON;   /* J1-10  PSC-Q08     PSC-控制系统故障指示灯              */
        MO_PSC_Buzzer           = LED_ON;   /* J1-11  PSC-Q09     PSC-蜂鸣器                          */
        MO_Led_MonitorSysFail   = LED_ON;   /* J1-12  PSC-Q10     PSC--监视系统故障指示灯             */
        
        //试灯不用点亮
        //MO_LookoutLight         = LED_ON;   /* J1-16&17  Q-PN0    PSC-?望灯带          */
        //MO_IlluminationLight    = LED_ON;   /* J2-14  Q-PN1       PSC-照明灯带 1       */
    }

    /*--------------------------------------
     * PSL1 试灯按钮
     * */
    if(MI_PSL1TestLight == Input_VALID)
    {
        MO_Led_PSLx_AllOpned    = LED_ON;   /* J2-02  PSL-Q00     PSLx-站台门全开启指示灯 */
        MO_Led_PSLx_AllClsed    = LED_ON;   /* J2-04  PSL-Q02     PSLx-站台门全关指示灯   */
        MO_Led_PSL1_IBPEn       = LED_ON;   /* J2-03  PSL-Q01     PSL1-IBP操作允许指示灯  */
        MO_Led_PSL1_EN          = LED_ON;   /* J2-06  PSL-Q04     PSL1-操作允许指示灯     */
        MO_Led_PSL1_Bypass      = LED_ON;   /* J2-07  PSL-Q05     PSL1-互锁解除指示灯     */
        MO_Led_PSL1Interlock    = LED_ON;   /* J2-10  PSL-Q08     PSL1-同侧互锁指示灯     */
    }

    /*--------------------------------------
     * PSL2 试灯按钮
     * */
    if(MI_PSL2TestLight == Input_VALID )
    {
        MO_Led_PSLx_AllOpned    = LED_ON;   /* J2-02  PSL-Q00     PSLx-站台门全开启指示灯 */
        MO_Led_PSLx_AllClsed    = LED_ON;   /* J2-04  PSL-Q02     PSLx-站台门全关指示灯   */
        MO_Led_PSL2_IBPEn       = LED_ON;   /* J2-05  PSL-Q03     PSL2-IBP操作允许指示灯  */
        MO_Led_PSL2_EN          = LED_ON;   /* J2-08  PSL-Q06     PSL2-操作允许指示灯     */
        MO_Led_PSL2_Bypass      = LED_ON;   /* J2-09  PSL-Q07     PSL2-互锁解除指示灯     */
        MO_Led_PSL2Interlock    = LED_ON;   /* J2-12  PSL-Q10     PSL2-同侧互锁指示灯     */
    }

    /*--------------------------------------
     * PSL3 试灯按钮 : 无
     * */

    /*--------------------------------------
     * IBP 试灯按钮 : 无
     * */
    #endif

    /*--------------------------------------
     * 判断错误信息
     * */
    #if 1 /* 折叠代码: 判断错误信息 */
	if((bPEDC_LOG1_OFFLINE == 0) || (bPEDC_LOG2_OFFLINE == 0))
	{
        /* 临时信息传递给 MMS 信息 */
        MON_LinkErr_OPNSD       = bMON_OUTOPNSD_Fault;
        MON_LinkErr_CTRLCMD     = bMON_OUTOPNCLS_Fault;
	}

    BOOL clr_TIM_SWCH_Q1Q2          = TRUE;
    BOOL clr_TIM_LinkErr_CMD_LOG1   = TRUE;
    BOOL clr_TIM_LinkErr_CMD_LOG2   = TRUE;
    if((bPEDC_SWCH_OFFLINE == 0))
    {
        /* 判断切换板两路选择:
         * 切换板: J3端子15和16不一致   */
        TrueSetFLAG((SWCH_K1_POW != SWCH_K4_POW),checkingTim[TIM_SWCH_Q1Q2].posTim, 50, SWCH_LinkErr_Q1AQ2A);
        clr_TIM_SWCH_Q1Q2 = TRUE;
        
        tChkOutputCmd swOutput;     /* 切换板输出，实际是监视板的输入监视点 */
        tChkOutputCmd logOutput;    /* 逻辑板输出监视点                     */
        swOutput.byte   = MON_CmdMon.byte;
        logOutput.byte  = 0;

        /* 逻辑板 连到 SWITCHER 的接线
         * 这里的判断似乎与 逻辑板到监视板的判断重复了，
         * 因为逻辑板没有监视 7 个命令的输入输出，借用监视板的输入
         *  */

        /* 这里认为 K1 K4 是一致的，不一致会报错误 SWCH_LinkErr_Q1AQ2A */
        if(SWCH_K1_POW == INVALID)
        {
            /* K1 K2 K3 没有通电，选择 IN-B，电气接线是 LOG1
             * 此时逻辑板的输出进入切换板，切换板常闭触点其连接到监视板，切换板没有监视输入，
             * 只能用监视板的信息判断连线
             *  */
            if((bPEDC_LOG1_OFFLINE == 0))
            {
                pLogIO     = &log1PCBIO;

                logOutput.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* 关门命令1    DCU_CLS8_1 */
                logOutput.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* 开门命令1    DCU_OPN8_1 */
                logOutput.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* 关门命令2    DCU_CLS8_2 */
                logOutput.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* 开门命令2    DCU_OPN8_2 */
                logOutput.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* 关边门命令1             */
                logOutput.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* 开边门命令1             */
                logOutput.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* 关边门命令2             */
                logOutput.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* 开边门命令2             */

                BOOL tmpErr = FALSE;
                if(swOutput.byte != logOutput.byte)
                {
                    tmpErr = TRUE;
                }

                TrueSetFLAG(tmpErr   , checkingTim[TIM_LinkErr_CMD_LOG1].posTim, 120, SWCH_LinkErr_CMD_LOG1);
                clr_TIM_LinkErr_CMD_LOG1 = FALSE;
            }
        }
        else
        {
        
            /* K1 K2 K3 通电，选择 IN-A，电气接线是 LOG2
             * 此时逻辑板的输出进入切换板，切换板常闭触点其连接到监视板，切换板没有监视输入，
             * 只能用监视板的信息判断连线
             *  */
            if((bPEDC_LOG2_OFFLINE == 0))
            {
                pLogIO     = &log2PCBIO;

                logOutput.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* 关门命令1    DCU_CLS8_1 */
                logOutput.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* 开门命令1    DCU_OPN8_1 */
                logOutput.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* 关门命令2    DCU_CLS8_2 */
                logOutput.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* 开门命令2    DCU_OPN8_2 */
                logOutput.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* 关边门命令1             */
                logOutput.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* 开边门命令1             */
                logOutput.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* 关边门命令2             */
                logOutput.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* 开边门命令2             */

                BOOL tmpErr = FALSE;
                if(swOutput.byte != logOutput.byte)
                {
                    tmpErr = TRUE;
                }

                TrueSetFLAG(tmpErr   , checkingTim[TIM_LinkErr_CMD_LOG2].posTim, 120, SWCH_LinkErr_CMD_LOG2);
                clr_TIM_LinkErr_CMD_LOG2 = FALSE;
            }
        }
    }
    
    if( clr_TIM_SWCH_Q1Q2 == TRUE )
        RESET_CHECKINGTIME(TIM_SWCH_Q1Q2);
    if( clr_TIM_LinkErr_CMD_LOG1 == TRUE )
        RESET_CHECKINGTIME(TIM_LinkErr_CMD_LOG1);
    if( clr_TIM_LinkErr_CMD_LOG2 == TRUE )
        RESET_CHECKINGTIME(TIM_LinkErr_CMD_LOG2);
    
    #endif



    //Handle_Message();                       //报板间接线故障等等

    /* 单独处理输出 GPIO B 的数据
     * PORTRead 可以读到输出点的IO值
     *  PORTSetBits(IOPORT_B, BIT_2);
     *  outPBData = PORTRead(IOPORT_B);
     *  */
    int len;
    int8u *pOut;
    unsigned int outPBData;
    pOut = (int8u *)getPic24OutputIODataPtr(&len);
    outPBData = PORTRead(IOPORT_B);
    outPBData &= ~(BIT_2|BIT_3|BIT_5) ;
    outPBData |= pOut[MON_Output_RBL_OFFSET] & (BIT_2|BIT_3|BIT_5);
    PORTWrite(IOPORT_B, outPBData);
}

/* =============================================================================
 * 用于继电器错误判断的  时间计时
 * 计时精度 1ms
 */
void RelayTimerRun(void)
{
    static tSYSTICK run8ms = 0;
    static tSYSTICK prevTick = 0;
    tSYSTICK tickNow, xMs;
    int8u i;

    tickNow = getSysTick();
    if(tickNow != prevTick)
    {
        xMs = (tSYSTICK)(tickNow - prevTick);
        prevTick = tickNow;

        run8ms += xMs;

        if(run8ms >= RELAY_TIMER_INTERVAL)
        {
            run8ms -= RELAY_TIMER_INTERVAL;

            for(i=0; i<TOTAL_RELAYS; i++)
            {
                if(relayWorkTime[i].posTim < 255)
                {
                    relayWorkTime[i].posTim ++;
                }

                if(relayWorkTime[i].revTim < 255)
                {
                    relayWorkTime[i].revTim ++;
                }
            }

            for(i=0; i<TOTAL_CONTACTOR_TIMERS; i++)
            {
                if(checkingTim[i].posTim < 60000)
                {
                    checkingTim[i].posTim ++;
                }
            }
        }
    }
}

/* 根据设置的?望灯带开始时间和结束时间
 * 判断当前是否需要点亮?望灯带
 * 返回值 直接赋值给 IO输出值 (LED_ON / LED_OFF)
 *  */
int8u checkSysTimeForLightBelt(void)
{
    BOOL  bNormal = TRUE;
    int8u lightOn = LED_ON;
    
    /* 获取当前PEDC的系统日期时间 */
    tSysTime nowPedcTime;
    getPedcSysTime(&nowPedcTime);
    
    int32u onSec, offSec, nowSec;
    
    do
    {
        /* 判断PEDC系统日期时间是否有效 : ifPEDCSysTimeValid()
         *                OVERSEE_LIGHT : 设置的?望灯带有效标志 1=有效
         *  */
        if((ifPEDCSysTimeValid() == FALSE) || (OVERSEE_LIGHT == 0))
        {   /* 没收到设置系统时间或不限制开关时间，则始终点亮 */
            break;
        }
        
        /* 开始时间和结束转换成秒 */
        onSec   = (int32u)tagDownIbpAlarmParameter.uRead_On_Hour  * 3600 + (int32u)tagDownIbpAlarmParameter.uRead_On_Minute  * 60;
        offSec  = (int32u)tagDownIbpAlarmParameter.uRead_Off_Hour * 3600 + (int32u)tagDownIbpAlarmParameter.uRead_Off_Minute * 60;
        if(onSec == offSec)
        {   /* 开始时间 == 结束时间，则始终点亮 */
            break;
        }

        /* 判断时间段开始时间是否小于时间段结束时间，如果不是就交换 */
        if(onSec > offSec)
        {
            nowSec  = onSec;
            onSec   = offSec;
            offSec  = nowSec;
            bNormal = FALSE;
        }
        nowSec  = (int32u)nowPedcTime.hour * 3600 + (int32u)nowPedcTime.min * 60 + (int32u)nowPedcTime.second;
        
        if((nowSec >= onSec) && (nowSec < offSec))
        {
            /* 当前时间落在时间段内 */
            if(bNormal == FALSE)
                lightOn = LED_OFF;  /* 反序 : 表明当前时间落在设置时间段外 */
        }
        else
        {
            /* 当前时间落在时间段外 */
            if(bNormal == TRUE)
                lightOn = LED_OFF;  /* 正序 : 表明当前时间落在设置时间段外 */
        }
    }while(FALSE);
    
    return lightOn;
}
