
#ifndef _TOMMS_INFODEF_H
#define _TOMMS_INFODEF_H

/* =============================================================================
 * 适用项目：SZ16
 */
/* =============================================================================
 * 
 * 继电器 状态(两个位表示)
 *   	00		01		10		11
 *      (断开正常)	吸合		故障		无
 */

/* 切换板 */
#define PEDC_RELAY0         MESG_BUF[PEDC_RELAY_SEAT + 0]        //
#define bSWCH_K0_Coil               PEDC_RELAY0.B0      //线圈电压
#define bSWCH_K0_Fault              PEDC_RELAY0.B1      //故障位
#define bSWCH_SF_Coil               PEDC_RELAY0.B2      //SF安全继电器组
#define bSWCH_SF_Fault              PEDC_RELAY0.B3      //SF安全继电器组

#define PEDC_RELAY1         MESG_BUF[PEDC_RELAY_SEAT + 1]       //
#define bSWCH_K1_Coil               PEDC_RELAY1.B0      //线圈电压
#define bSWCH_K1_Fault              PEDC_RELAY1.B1      //故障位
#define bSWCH_K2_Coil               PEDC_RELAY1.B2      //线圈电压
#define bSWCH_K2_Fault              PEDC_RELAY1.B3      //故障位
#define bSWCH_K3_Coil               PEDC_RELAY1.B4      //线圈电压
#define bSWCH_K3_Fault              PEDC_RELAY1.B5      //故障位
#define bSWCH_K4_Coil               PEDC_RELAY1.B6      //线圈电压
#define bSWCH_K4_Fault              PEDC_RELAY1.B7      //故障位

#define PEDC_RELAY2         MESG_BUF[PEDC_RELAY_SEAT + 2]       //
#define bSWCH_K5_Coil               PEDC_RELAY2.B0      //线圈电压
#define bSWCH_K5_Fault              PEDC_RELAY2.B1      //故障位
#define bSWCH_K6_Coil               PEDC_RELAY2.B2      //线圈电压
#define bSWCH_K6_Fault              PEDC_RELAY2.B3      //故障位
#define bSWCH_K11_Coil              PEDC_RELAY2.B4      //线圈电压
#define bSWCH_K11_Fault             PEDC_RELAY2.B5      //故障位
#define bSWCH_K12_Coil              PEDC_RELAY2.B6      //线圈电压
#define bSWCH_K12_Fault             PEDC_RELAY2.B7      //故障位

#define PEDC_RELAY3         MESG_BUF[PEDC_RELAY_SEAT + 3]
#define bSWCH_K13_Coil              PEDC_RELAY3.B0      //线圈电压
#define bSWCH_K13_Fault             PEDC_RELAY3.B1      //故障位
#define bSWCH_K21_Coil              PEDC_RELAY3.B2      //线圈电压
#define bSWCH_K21_Fault             PEDC_RELAY3.B3      //故障位
#define bSWCH_K22_Coil              PEDC_RELAY3.B4      //线圈电压
#define bSWCH_K22_Fault             PEDC_RELAY3.B5      //故障位
#define bSWCH_K23_Coil              PEDC_RELAY3.B6      //线圈电压
#define bSWCH_K23_Fault             PEDC_RELAY3.B7      //故障位

/* 逻辑板 1 */
#define PEDC_RELAY4         MESG_BUF[PEDC_RELAY_SEAT + 4]
#define bLOG1_K0_Coil              PEDC_RELAY4.B0      //线圈电压
#define bLOG1_K0_Fault             PEDC_RELAY4.B1      //故障位

#define PEDC_RELAY5         MESG_BUF[PEDC_RELAY_SEAT + 5]
#define bLOG1_K1A_Coil              PEDC_RELAY5.B0      //线圈电压
#define bLOG1_K1A_Fault             PEDC_RELAY5.B1      //故障位
#define bLOG1_K1B_Coil              PEDC_RELAY5.B2      //线圈电压
#define bLOG1_K1B_Fault             PEDC_RELAY5.B3      //故障位
#define bLOG1_K2A_Coil              PEDC_RELAY5.B4      //线圈电压
#define bLOG1_K2A_Fault             PEDC_RELAY5.B5      //故障位
#define bLOG1_K2B_Coil              PEDC_RELAY5.B6      //线圈电压
#define bLOG1_K2B_Fault             PEDC_RELAY5.B7      //故障位

#define PEDC_RELAY6         MESG_BUF[PEDC_RELAY_SEAT + 6]
#define bLOG1_K3A_Coil              PEDC_RELAY6.B0      //线圈电压
#define bLOG1_K3A_Fault             PEDC_RELAY6.B1      //故障位
#define bLOG1_K3B_Coil              PEDC_RELAY6.B2      //线圈电压
#define bLOG1_K3B_Fault             PEDC_RELAY6.B3      //故障位
#define bLOG1_K4A_Coil              PEDC_RELAY6.B4      //线圈电压
#define bLOG1_K4A_Fault             PEDC_RELAY6.B5      //故障位
#define bLOG1_K4B_Coil              PEDC_RELAY6.B6      //线圈电压
#define bLOG1_K4B_Fault             PEDC_RELAY6.B7      //故障位

#define PEDC_RELAY7         MESG_BUF[PEDC_RELAY_SEAT + 7]
#define bLOG1_K5A_Coil              PEDC_RELAY7.B0      //线圈电压
#define bLOG1_K5A_Fault             PEDC_RELAY7.B1      //故障位
#define bLOG1_K5B_Coil              PEDC_RELAY7.B2      //线圈电压
#define bLOG1_K5B_Fault             PEDC_RELAY7.B3      //故障位
#define bLOG1_K6A_Coil              PEDC_RELAY7.B4      //线圈电压
#define bLOG1_K6A_Fault             PEDC_RELAY7.B5      //故障位
#define bLOG1_K6B_Coil              PEDC_RELAY7.B6      //线圈电压
#define bLOG1_K6B_Fault             PEDC_RELAY7.B7      //故障位


#define PEDC_RELAY8         MESG_BUF[PEDC_RELAY_SEAT + 8]
#define bLOG1_K7A_Coil              PEDC_RELAY8.B0      //线圈电压
#define bLOG1_K7A_Fault             PEDC_RELAY8.B1      //故障位
#define bLOG1_K7B_Coil              PEDC_RELAY8.B2      //线圈电压
#define bLOG1_K7B_Fault             PEDC_RELAY8.B3      //故障位
#define bLOG1_K8A_Coil              PEDC_RELAY8.B4      //线圈电压
#define bLOG1_K8A_Fault             PEDC_RELAY8.B5      //故障位
#define bLOG1_K8B_Coil              PEDC_RELAY8.B6      //线圈电压
#define bLOG1_K8B_Fault             PEDC_RELAY8.B7      //故障位

#define PEDC_RELAY9         MESG_BUF[PEDC_RELAY_SEAT + 9]
#define bLOG1_K10_Coil              PEDC_RELAY9.B0      //线圈电压
#define bLOG1_K10_Fault             PEDC_RELAY9.B1      //故障位
#define bLOG1_K21_Coil              PEDC_RELAY9.B2      //线圈电压
#define bLOG1_K21_Fault             PEDC_RELAY9.B3      //故障位
#define bLOG1_K22_Coil              PEDC_RELAY9.B4      //线圈电压
#define bLOG1_K22_Fault             PEDC_RELAY9.B5      //故障位
#define bLOG1_K30_Coil              PEDC_RELAY9.B6      //线圈电压
#define bLOG1_K30_Fault             PEDC_RELAY9.B7      //故障位

/* 逻辑板 2 */
#define PEDC_RELAY10         MESG_BUF[PEDC_RELAY_SEAT + 10]
#define bLOG2_K0_Coil               PEDC_RELAY10.B0     //线圈电压
#define bLOG2_K0_Fault              PEDC_RELAY10.B1     //故障位

#define PEDC_RELAY11         MESG_BUF[PEDC_RELAY_SEAT + 11]
#define bLOG2_K1A_Coil              PEDC_RELAY11.B0     //线圈电压
#define bLOG2_K1A_Fault             PEDC_RELAY11.B1     //故障位
#define bLOG2_K1B_Coil              PEDC_RELAY11.B2     //线圈电压
#define bLOG2_K1B_Fault             PEDC_RELAY11.B3     //故障位
#define bLOG2_K2A_Coil              PEDC_RELAY11.B4     //线圈电压
#define bLOG2_K2A_Fault             PEDC_RELAY11.B5     //故障位
#define bLOG2_K2B_Coil              PEDC_RELAY11.B6     //线圈电压
#define bLOG2_K2B_Fault             PEDC_RELAY11.B7     //故障位

#define PEDC_RELAY12         MESG_BUF[PEDC_RELAY_SEAT + 12]
#define bLOG2_K3A_Coil              PEDC_RELAY12.B0     //线圈电压
#define bLOG2_K3A_Fault             PEDC_RELAY12.B1     //故障位
#define bLOG2_K3B_Coil              PEDC_RELAY12.B2     //线圈电压
#define bLOG2_K3B_Fault             PEDC_RELAY12.B3     //故障位
#define bLOG2_K4A_Coil              PEDC_RELAY12.B4     //线圈电压
#define bLOG2_K4A_Fault             PEDC_RELAY12.B5     //故障位
#define bLOG2_K4B_Coil              PEDC_RELAY12.B6     //线圈电压
#define bLOG2_K4B_Fault             PEDC_RELAY12.B7     //故障位

#define PEDC_RELAY13         MESG_BUF[PEDC_RELAY_SEAT + 13]
#define bLOG2_K5A_Coil              PEDC_RELAY13.B0     //线圈电压
#define bLOG2_K5A_Fault             PEDC_RELAY13.B1     //故障位
#define bLOG2_K5B_Coil              PEDC_RELAY13.B2     //线圈电压
#define bLOG2_K5B_Fault             PEDC_RELAY13.B3     //故障位
#define bLOG2_K6A_Coil              PEDC_RELAY13.B4     //线圈电压
#define bLOG2_K6A_Fault             PEDC_RELAY13.B5     //故障位
#define bLOG2_K6B_Coil              PEDC_RELAY13.B6     //线圈电压
#define bLOG2_K6B_Fault             PEDC_RELAY13.B7     //故障位


#define PEDC_RELAY14         MESG_BUF[PEDC_RELAY_SEAT + 14]
#define bLOG2_K7A_Coil              PEDC_RELAY14.B0     //线圈电压
#define bLOG2_K7A_Fault             PEDC_RELAY14.B1     //故障位
#define bLOG2_K7B_Coil              PEDC_RELAY14.B2     //线圈电压
#define bLOG2_K7B_Fault             PEDC_RELAY14.B3     //故障位
#define bLOG2_K8A_Coil              PEDC_RELAY14.B4     //线圈电压
#define bLOG2_K8A_Fault             PEDC_RELAY14.B5     //故障位
#define bLOG2_K8B_Coil              PEDC_RELAY14.B6     //线圈电压
#define bLOG2_K8B_Fault             PEDC_RELAY14.B7     //故障位

#define PEDC_RELAY15         MESG_BUF[PEDC_RELAY_SEAT + 15]
#define bLOG2_K10_Coil              PEDC_RELAY15.B0     //线圈电压
#define bLOG2_K10_Fault             PEDC_RELAY15.B1     //故障位
#define bLOG2_K21_Coil              PEDC_RELAY15.B2     //线圈电压
#define bLOG2_K21_Fault             PEDC_RELAY15.B3     //故障位
#define bLOG2_K22_Coil              PEDC_RELAY15.B4     //线圈电压
#define bLOG2_K22_Fault             PEDC_RELAY15.B5     //故障位
#define bLOG2_K30_Coil              PEDC_RELAY15.B6     //线圈电压
#define bLOG2_K30_Fault             PEDC_RELAY15.B7     //故障位

#define PEDC_RELAY16         MESG_BUF[PEDC_RELAY_SEAT + 16]

#define PEDC_RELAY17         MESG_BUF[PEDC_RELAY_SEAT + 17]

#define PEDC_LOG_Status      MESG_BUF[PEDC_RELAY_SEAT + 18]  /* 逻辑板工作状态 */

#define PEDC_SF_FLASH_BUF    MESG_BUF[PEDC_RELAY_SEAT + 19]

/* =============================================================================
 * 操作命令状态及其它信息
 */

#define PEDC_SIG                MESG_BUF[PEDC_PSC_SEAT + 0]
#define bPedcSIG_EN                 PEDC_SIG.B0         //LOG SIG 使能-保留未用
#define bPedcSIG_CLS                PEDC_SIG.B1         //LOG SIG 关门
//#define bPedcSIG_OPN6               PEDC_SIG.B2         //LOG SIG 开6车-保留未用
#define bPedcSIG_OPN8               PEDC_SIG.B3         //LOG SIG 开8车
#define bPedc_DCUIDConflicted       PEDC_SIG.B4         //DCU ID冲突报警(1=冲突)
#define bPedc_ATCCMD_FAILURE        PEDC_SIG.B5         //逻辑板 命令错误 ，开关门同时 1
#define bPedc_CLSLCK                PEDC_SIG.B6         //所有门关闭且锁紧
#define bPedc_NodeCfgErr            PEDC_SIG.B7         //通讯节点配置错误

#define PEDC_PSL1               MESG_BUF[PEDC_PSC_SEAT + 1]
#define bPedcPSL1_EN                PEDC_PSL1.B0        //PSL1 操作允许
#define bPedcPSL1_CLS               PEDC_PSL1.B1        //PSL1 关门
//#define bPedcPSL1_OPN6              PEDC_PSL1.B2        //PSL1 开6车-保留未用
#define bPedcPSL1_OPN8              PEDC_PSL1.B3        //PSL1 开8车
#define bPedcPSL1_ByPass            PEDC_PSL1.B4        //PSL1 Bypass

#define PEDC_PSL2               MESG_BUF[PEDC_PSC_SEAT + 2]
#define bPedcPSL2_EN                PEDC_PSL2.B0        //PSL2 操作允许
#define bPedcPSL2_CLS               PEDC_PSL2.B1        //PSL2 关门
//#define bPedcPSL2_OPN6              PEDC_PSL2.B2        //PSL2 开6车-保留未用
#define bPedcPSL2_OPN8              PEDC_PSL2.B3        //PSL2 开8车
#define bPedcPSL2_ByPass            PEDC_PSL2.B4        //PSL2 Bypass

#define PEDC_PSL3               MESG_BUF[PEDC_PSC_SEAT + 3]
//#define bPedcPSL3_EN                PEDC_PSL3.B0        //PSL3 操作允许
//#define bPedcPSL3_CLS               PEDC_PSL3.B1        //PSL3 关门
//#define bPedcPSL3_OPN6              PEDC_PSL3.B2        //PSL3 开6车
//#define bPedcPSL3_OPN8              PEDC_PSL3.B3        //PSL3 开8车-保留未用
//#define bPedcPSL3_ByPass            PEDC_PSL3.B4        //PSL3 Bypass
#define bLOG1_2_SwitchAlarm         PEDC_PSL3.B7        /* 板1 板2 发生切换动作，这个点是调试用 */


#define PEDC_IBP1                MESG_BUF[PEDC_PSC_SEAT + 4]
#define bPedcIBP_EN                 PEDC_IBP1.B0        //IBP 操作允许
#define bPedcIBP_CLS                PEDC_IBP1.B1        //IBP 关门
//#define bPedcIBP_OPN6               PEDC_IBP1.B2        //IBP 开6车-保留未用
#define bPedcIBP_OPN8               PEDC_IBP1.B3        //IBP 开8车
#define bPedcIBP_OPNSD              PEDC_IBP1.B4        //IBP 开边门

#define PEDC_PSC1               MESG_BUF[PEDC_PSC_SEAT + 5]
#define bPedc_OUT_CLS8_1            PEDC_PSC1.B0        //输出命令 CLS8_1
#define bPedc_OUT_OPN8_1            PEDC_PSC1.B1        //输出命令 OPN8_1
#define bPedc_OUT_CLS8_2            PEDC_PSC1.B2        //输出命令 CLS8_2
#define bPedc_OUT_OPN8_2            PEDC_PSC1.B3        //输出命令 OPN8_2
#define bPedc_OUT_CLSSD_1           PEDC_PSC1.B4        //输出命令 关边门1
#define bPedc_OUT_OPNSD_1           PEDC_PSC1.B5        //输出命令 开边门1
#define bPedc_OUT_CLSSD_2           PEDC_PSC1.B6        //输出命令 关边门2
#define bPedc_OUT_OPNSD_2           PEDC_PSC1.B7        //输出命令 开边门2

#define PEDC_POWER_WARNING      MESG_BUF[PEDC_PSC_SEAT + 6]
#define bPowerWarning_S0            PEDC_POWER_WARNING.B0   //交流电源故障                 
#define bPowerWarning_S1            PEDC_POWER_WARNING.B1   //驱动电源故障
#define bPowerWarning_S2            PEDC_POWER_WARNING.B2   //驱动电池投入使用
#define bPowerWarning_S3            PEDC_POWER_WARNING.B3   //控制电源故障
#define bPowerWarning_S4            PEDC_POWER_WARNING.B4   //控制电源投入使用

#define PEDC_OFFLINE            MESG_BUF[PEDC_PSC_SEAT + 7]
#define bPEDC_PIC24A_OFFLINE        PEDC_OFFLINE.B0         /* PIC24A 离线标志 */
#define bPEDC_PIC24B_OFFLINE        PEDC_OFFLINE.B1         /* PIC24B 离线标志 */
#define bPEDC_SWCH_OFFLINE          PEDC_OFFLINE.B2         /* 切换板 离线标志 */
#define bPEDC_LOG1_OFFLINE          PEDC_OFFLINE.B3         /* LOG1   离线标志 */
#define bPEDC_LOG2_OFFLINE          PEDC_OFFLINE.B4         /* LOG2   离线标志 */

#define PEDC_LOG_Selector       MESG_BUF[PEDC_PSC_SEAT + 8] /*  */
#define bLOG_Sw_Auto                PEDC_LOG_Selector.B0    /* 逻辑板选择：自动   */
#define bLOG_Sw_LOG1                PEDC_LOG_Selector.B1    /* 逻辑板选择：手动LOG1   */
#define bLOG_Sw_LOG2                PEDC_LOG_Selector.B2    /* 逻辑板选择：手动LOG2   */
#define bLOG_WorkLOG2               PEDC_LOG_Selector.B3    /* 当前工作的逻辑板(0=逻辑板1,1=逻辑板2) */
#define bPEDC_LOG1_TestMode_En      PEDC_LOG_Selector.B4    /* LOG1 测试模式使能标志 */
#define bPEDC_LOG1_TestRunning      PEDC_LOG_Selector.B5    /* LOG1 测试模式运行标志 */
#define bPEDC_LOG2_TestMode_En      PEDC_LOG_Selector.B6    /* LOG2 测试模式使能标志 */
#define bPEDC_LOG2_TestRunning      PEDC_LOG_Selector.B7    /* LOG2 测试模式运行标志 */

#define PEDC_BAS_Status         MESG_BUF[PEDC_PSC_SEAT + 9] /* 风阀相关信息 */
#define bBASSys_OffLine             PEDC_BAS_Status.B0      /* BAS通信离线 */
#define bBASOpnCmd                  PEDC_BAS_Status.B1      /* BAS 开风阀 */
#define bBASClsCmd                  PEDC_BAS_Status.B2      /* BAS 关风阀 */
#define bPSC_OpnValve               PEDC_BAS_Status.B3      /* PSC 开风阀 */
#define bPSC_ClsValve               PEDC_BAS_Status.B4      /* PSC 关风阀 */
#define bPEDC_WVAllClsed            PEDC_BAS_Status.B5      /* 风阀总关 */
#define bPEDC_WVAllOpned            PEDC_BAS_Status.B6      /* 风阀总开 */
#define bPEDC_OpnClsOverTime        PEDC_BAS_Status.B7      /* 风阀开/关超时总报警 */

/* 板间连线的信息 */
#define LOGLINKERR_MASK_BITS    0x1F
#define PEDC_LOG1LOG2LinkMsg    MESG_BUF[PEDC_PSC_SEAT + 11]    /* 逻辑板1和逻辑板2 连线状态 */
#define PEDC_LOG1MONLinkMsg     MESG_BUF[PEDC_PSC_SEAT + 12]    /* 逻辑板1和监视板  连线状态 */
#define PEDC_LOG2MONLinkMsg     MESG_BUF[PEDC_PSC_SEAT + 13]    /* 逻辑板2和监视板  连线状态 */

#define MONLINKERR_MASK_BITS    0xC0
#define PEDC_LinkErrMsg1        MESG_BUF[PEDC_PSC_SEAT + 14]
#define SWCH_LinkErr_CMD_LOG2       PEDC_LinkErrMsg1.B0     /* 切换板命令缺失 J2.1-7  ，接LOG2 */
#define SWCH_LinkErr_CMD_LOG1       PEDC_LinkErrMsg1.B1     /* 切换板命令缺失 J2.13-19，接LOG1 */
#define SWCH_LinkErr_Q1AQ2A         PEDC_LinkErrMsg1.B2     /* 切换选择点不一致 J3.15和16   */
#define SWCH_LinkErr_Q1AQ2A_LOST    PEDC_LinkErrMsg1.B3     /* 切换选择点 J3.15和16 与监视板连线缺失 */
#define MON_LOG1LOG2_SELECT         PEDC_LinkErrMsg1.B4     /* 逻辑板选择开关错误 J1.20~22 */
//#define                           PEDC_LinkErrMsg1.B5     
#define MON_LinkErr_OPNSD           PEDC_LinkErrMsg1.B6     /* MON-J4-4/5/16/17: 开关边门缺失         */
#define MON_LinkErr_CTRLCMD         PEDC_LinkErrMsg1.B7     /* MON-J4-2/3/14/15: 开关门命令缺失 */

#define ToMMS_PEDC_ID           MESG_BUF[PEDC_PSC_SEAT + 15]




/* 监视板、逻辑板1、逻辑板2
 * 板间连线错误信息定义
 * 每块板子的信息用 5 个位表示:  / 
 *  */
                             /*  PEDC_LOG1LinkMsg / PEDC_LOG2LinkMsg, PEDC_MONLinkMsg            */
#define PCBLINK_ERR_NONE            0   /* 正常                无       , 无    */
#define PCBLINK_ERR_IBP_EN          1   /* IBP使能缺失         J1.2&3   , -     */
#define PCBLINK_ERR_IBP_OPN         2   /* IBP开门命令缺失     J1.5     , -     */
#define PCBLINK_ERR_IBP_CLS         3   /* IBP关门命令缺失     J1.7     , -     */
#define PCBLINK_ERR_IBP_OPNSIDE     4   /* IBP开边门命令缺失   J1.4     , -     */
#define PCBLINK_ERR_PSL1_EN         5   /* PSL1使能缺失        J1.18&19 , -     */
#define PCBLINK_ERR_PSL1_OPN        6   /* PSL1开门命令缺失    J1.21    , -     */
#define PCBLINK_ERR_PSL1_CLS        7   /* PSL1关门命令缺失    J1.23    , -     */
#define PCBLINK_ERR_PSL2_EN         8   /* PSL2使能缺失        J1.25&26 , -     */
#define PCBLINK_ERR_PSL2_OPN        9   /* PSL2开门命令缺失    J1.28    , -     */
#define PCBLINK_ERR_PSL2_CLS        10  /* PSL2关门命令缺失    J1.30    , -     */
#define PCBLINK_ERR_SIG_OPN         11  /* SIG 开门命令缺失    J2.11&12 , J6.26 */
#define PCBLINK_ERR_SIG_CLS         12  /* SIG 关门命令缺失    J2.13&14 , J6.27 */


typedef union
{   
    struct
    {
        int8u   IBP_En          : 1;    /* 00 */
        int8u   IBP_Opn         : 1;    /* 01 */
        int8u   IBP_Cls         : 1;    /* 02 */
        int8u   IBP_OpnSD_En    : 1;    /* 03 */
        int8u   IBP_OpnSD       : 1;    /* 04 */

        int8u   PSL1_En         : 1;    /* 05 */
        int8u   PSL1_Opn        : 1;    /* 06 */
        int8u   PSL1_Cls        : 1;    /* 07 */

        int8u   PSL2_En         : 1;    /* 08 */
        int8u   PSL2_Opn        : 1;    /* 09 */
        int8u   PSL2_Cls        : 1;    /* 10 */

        int8u                   : 1;    /* 11 */
        int8u                   : 1;    /* 12 */
        int8u                   : 1;    /* 13 */

        int8u   SIG_Opn         : 1;    /* 14 */
        int8u   SIG_Cls         : 1;    /* 15 */
    }LinkBit;
    
    int8u   bytes[2];
}tLinkSts;




  














/* for PEDC_InCmdLinkErr */
//#define ERR_LINK_IBP_EN         20  //IBP-使能
//#define ERR_LINK_IBP_OPN6       21  //IBP-开门6
//#define ERR_LINK_IBP_OPN8       22  //IBP-开门8
//#define ERR_LINK_IBP_CLS6       23  //IBP-关门6
//#define ERR_LINK_IBP_CLS8       24  //IBP-关门8
//#define ERR_LINK_IBP_OPNSDEN    25  //IBP-开边门使能
//#define ERR_LINK_IBP_OPNSD1     26  //IBP-开边门1
//#define ERR_LINK_IBP_CLSSD1     27  //IBP-关边门1
//#define ERR_LINK_IBP_OPNSD2     28  //IBP-开边门2
//#define ERR_LINK_IBP_CLSSD2     29  //IBP-关边门2
//#define ERR_LINK_PSL1_EN        40  //PSL1-使能
//#define ERR_LINK_PSL1_OPN6      41  //PSL1-开门6
//#define ERR_LINK_PSL1_OPN8      42  //PSL1-开门8
//#define ERR_LINK_PSL1_CLS6      43  //PSL1-关门6
//#define ERR_LINK_PSL1_CLS8      44  //PSL1-关门8
//#define ERR_LINK_PSL2_EN        50  //PSL2-使能
//#define ERR_LINK_PSL2_OPN6      51  //PSL2-开门6
//#define ERR_LINK_PSL2_OPN8      52  //PSL2-开门8
//#define ERR_LINK_PSL2_CLS6      53  //PSL2-关门6
//#define ERR_LINK_PSL2_CLS8      54  //PSL2-关门8
//#define ERR_LINK_PSL3_EN        60  //PSL3-使能
//#define ERR_LINK_PSL3_OPN6      61  //PSL3-开门6
//#define ERR_LINK_PSL3_OPN8      62  //PSL3-开门8
//#define ERR_LINK_PSL3_CLS6      63  //PSL3-关门6
//#define ERR_LINK_PSL3_CLS8      64  //PSL3-关门8
//#define ERR_LINK_PSC_EN         70  //PSC-使能
//#define ERR_LINK_PSC_OPN6       71  //PSC-开门6
//#define ERR_LINK_PSC_OPN8       72  //PSC-开门8
//#define ERR_LINK_PSC_CLS6       73  //PSC-关门6
//#define ERR_LINK_PSC_CLS8       74  //PSC-关门8
//#define ERR_LINK_SIG_OPN6       80  //SIG-开门6
//#define ERR_LINK_SIG_OPN8       81  //SIG-开门8
//#define ERR_LINK_SIG_CLS        82  //SIG-关门
//--------------------------------------
#endif  //_TOMMS_INFODEF_H






