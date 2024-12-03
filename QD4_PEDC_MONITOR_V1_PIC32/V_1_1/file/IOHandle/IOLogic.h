

#ifndef _IOLOGIC_H
#define _IOLOGIC_H

#include "_cpu.h"
#include "commDef.h"

/* =============================================================================
 *  */
typedef struct
{
    int8u   b0      : 1;
    int8u   b1      : 1;
    int8u   b2      : 1;
    int8u   b3      : 1;
    int8u   b4      : 1;
    int8u   b5      : 1;
    int8u   b6      : 1;
    int8u   b7      : 1;
}tPCA9505IO;

typedef union
{
    struct
    {
        int8u   eIBP        : 1;
        int8u   ePSL        : 1;
        int8u   eSIG        : 1;
        int8u   ePSC        : 1;
        int8u   eOCC        : 1;
        int8u   eATC        : 1;
        int8u   eBAK1       : 1;
        int8u   eBAK2       : 1;
        
        int8u   eOut        : 1;
    }errBits;
    
    int16u errWord;
}tErrCmd;

/* =============================================================================
 * PCA9505 U20 U21 U22 位功能定义
 * 注意：
 *      PEDC_SZ_V1.pdf  和 PEDC_SZ.pdf IO有区别
 * JI   - jack input
 * JO   - jack output
 * CO   - command output，MCU输入监视
 * IN   - 板内输入点
 * OUT  - 板内输出点
 *  */
typedef union
{
    struct
    {
        /* ===== U20 ===== */
        int8u   JI_IBP_OPN6         : 1;    /* U20_0.0 J1.4      监视输入命令 IBPOPN6                 */
        int8u   JI_PSL1_OPN6        : 1;    /* U20_0.1 J1.20     监视输入命令 PSL1 OPN6               */
        int8u   JI_IBP_OPN8         : 1;    /* U20_0.2 J1.5      监视输入命令 IBPOPN8                 */
        int8u   JI_PSL1_OPN8        : 1;    /* U20_0.3 J1.21     监视输入命令 PSL1 OPN8               */
        int8u   JI_IBP_CLS6         : 1;    /* U20_0.4 J1.6      监视输入命令 IBPCLS6                 */
        int8u   JI_PSL1_CLS6        : 1;    /* U20_0.5 J1.22     监视输入命令 PSL1 CLS6               */
        int8u   JI_IBP_CLS8         : 1;    /* U20_0.6 J1.7      监视输入命令 IBPCLS8                 */
        int8u   JI_PSL1_CLS8        : 1;    /* U20_0.7 J1.23     监视输入命令 PSL1 CLS8               */

        int8u   JI_IBP_OPNSD1       : 1;    /* U20_1.0 J1.10     监视输入命令 IBP 开边门1             */
        int8u   JI_IN_BK0           : 1;    /* U20_1.1 J1.24                                          */
        int8u   JI_IBP_OPNSD2       : 1;    /* U20_1.2 J1.11     监视输入命令 IBP 开边门2             */
        int8u   JI_PSL2_OPN6        : 1;    /* U20_1.3 J1.27     监视输入命令 PSL2 OPN6               */
        int8u   JI_IBP_CLSSD1       : 1;    /* U20_1.4 J1.12     监视输入命令 IBP 关边门1             */
        int8u   JI_PSL2_OPN8        : 1;    /* U20_1.5 J1.28     监视输入命令 PSL2 OPN8               */
        int8u   JI_IBP_CLSSD2       : 1;    /* U20_1.6 J1.13     监视输入命令 IBP 关边门2             */
        int8u   JI_PSL2_CLS6        : 1;    /* U20_1.7 J1.29     监视输入命令 PSL2 CLS6               */

        int8u   JI_PSC_CLS6         : 1;    /* U20_2.0 J2.20     监视输入命令 PSC  CLS6               */
        int8u   JI_PSL2_CLS8        : 1;    /* U20_2.1 J1.30     监视输入命令 PSL2 CLS6               */
        int8u   JI_PSC_OPN6         : 1;    /* U20_2.2 J2.18     监视输入命令 PSC  OPN6               */
        int8u   JI_IN_PN0           : 1;    /* U20_2.3 J1.14&15                                       */
        int8u   IN_K1B_NO           : 1;    /* U20_2.4                                                */
        int8u   IN_K1A_NO           : 1;    /* U20_2.5                                                */
        int8u   IN_K2B_NO           : 1;    /* U20_2.6                                                */
        int8u   IN_K3A_NO           : 1;    /* U20_2.7                                                */

        int8u   IN_K3B_NO           : 1;    /* U20_3.0                                                */
        int8u   IN_K4A_NO           : 1;    /* U20_3.1                                                */
        int8u   IN_K4B_NO           : 1;    /* U20_3.2                                                */
        int8u   JI_SIG_OPN8         : 1;    /* U20_3.3 J2.11&12    信号系统命令 开8车                 */
        int8u   JI_SIG_CLS          : 1;    /* U20_3.4 J2.13&14    信号系统命令 关车门                */
        int8u   JI_SIG_OPN6         : 1;    /* U20_3.5 J2.9&10     信号系统命令 开6车                 */
        int8u   IN_IBPPSLPSC_EN     : 1;    /* U20_3.6             IBP_EN | PSLX_EN | PSC_TEST        */
        int8u   IN_ALL_CLS          : 1;    /* U20_3.7             IBP / PSL / PSC / SIG CLS          */

        int8u   IN_ALL_OPN6         : 1;    /* U20_4.0             IBP / PSL / PSC / SIG OPN6         */
        int8u   IN_ALL_OPN8         : 1;    /* U20_4.1             IBP / PSL / PSC / SIG OPN8         */
        int8u   JI_PSC_TEST         : 1;    /* U20_4.2 J2.16&17    PSC测试使能                        */
        int8u   JI_PSL1_EN          : 1;    /* U20_4.3 J1.18&19    PSL1 使能                          */
        int8u   JI_IBP_A_EN         : 1;    /* U20_4.4 J1.8&9      IBP开边门使能                      */
        int8u   JI_IBP_EN           : 1;    /* U20_4.5 J1.2&3      IBP 使能                           */
        int8u   JI_PSL2_EN          : 1;    /* U20_4.6 J1.25&26    PSL2 使能                          */
        int8u   JI_PSL3_EN          : 1;    /* U20_4.7 J2.2&3      PSL3 使能                          */
        
        /* ===== U21 ===== */
        int8u   JI_PSC_OPN8         : 1;    /* U21_0.0 PSC-OP2      J2.19                                                   */
        int8u   IN_BK1              : 1;    /* U21_0.1 IN-BK1       J1.31                                                   */
        int8u   JI_PSC_CLS8         : 1;    /* U21_0.2 PSC-OP4      J2.21                                                   */
        int8u   JI_PSL3_OPN6        : 1;    /* U21_0.3 PSL3-OP1     J2.4                                                    */
        int8u   JI_PSL3_OPN8        : 1;    /* U21_0.4 PSL3-OP2     J2.5                                                    */
        int8u   JI_PSL3_CLS6        : 1;    /* U21_0.5 PSL3-OP3     J2.6                                                    */
        int8u   IN_PN1              : 1;    /* U21_0.6 IN-PN1       J2.25&26    输入，接地用于测试自动运行的功能            */
        int8u   JI_PSL3_CLS8        : 1;    /* U21_0.7 PSL3-OP4     J2.7                                                    */
        
        int8u   CO_Q0               : 1;    /* U21_1.0 COM-Q-0      J3.9    备用输出给DCU                                   */
        int8u   CO_Q1               : 1;    /* U21_1.1 COM-Q-1      J3.21   备用输出给DCU                                   */
        int8u   JO_PN1              : 1;    /* U21_1.2 OUT-PN1      J3.11&12    端子输出点，备用                            */
        int8u   OUT_IBP_OPNSD0      : 1;    /* U21_1.3 IBP-Q-OPSD1  J3.19   MCU 控制IO输出点，板内输出到端子 IBP-Q-OPSD0    */
        int8u   OUT_IBP_OPNSD1      : 1;    /* U21_1.4 IBP-Q-OPSD2  J3.20   MCU 控制IO输出点，板内输出到端子 IBP-Q-OPSD1    */
        int8u   OUT_IBPPSL_Q1       : 1;    /* U21_1.5 IBP-PSL-Q1   J3.8    MCU 控制IO输出点，板内输出到端子 IBP-PSL-Q1     */
        int8u   OUT_IBPPSL_Q0       : 1;    /* U21_1.6 IBP-PSL-Q0   J3.7    MCU 控制IO输出点，板内输出到端子 IBP-PSL-Q0     */
        int8u   OUT_PSC_CLS         : 1;    /* U21_1.7 PSC-Q-CL     MCU 控制IO输出点，板内输出                              */
        
        int8u   OUT_PN0             : 1;    /* U21_2.0 OUT-PN0      J2.27&28    端子输出点，备用                            */
        int8u                       : 1;    /* U21_2.1                                                                      */
        int8u                       : 1;    /* U21_2.2                                                                      */
        int8u                       : 1;    /* U21_2.3                                                                      */
        int8u                       : 1;    /* U21_2.4                                                                      */
        int8u   CO_DCU_IBPPSL_OPN2  : 1;    /* U21_2.5 DCU-Q12      J3.19   板子输出命令监视点                              */
        int8u                       : 1;    /* U21_2.6                                                                      */
        int8u                       : 1;    /* U21_2.7                                                                      */
        
        int8u   IN_K7B_NO           : 1;    /* U21_3.0 K-S14                                                                */
        int8u                       : 1;    /* U21_3.1                                                                      */
        int8u                       : 1;    /* U21_3.2                                                                      */
        int8u   IN_K8A_NO           : 1;    /* U21_3.3 K-S15                                                                */
        int8u                       : 1;    /* U21_3.4                                                                      */
        int8u                       : 1;    /* U21_3.5                                                                      */
        int8u   IN_K8B_NO           : 1;    /* U21_3.6 K-S16                                                                */
        int8u   IN_K6B_NO           : 1;    /* U21_3.7 K-S12                                                                */
        
        int8u   IN_K22_NO           : 1;    /* U21_4.0 K-S18                                                                */
        int8u   IN_K5A_NO           : 1;    /* U21_4.1 K-S9                                                                 */
        int8u   IN_K5B_NO           : 1;    /* U21_4.2 K-S10                                                                */
        int8u   IN_K6A_NO           : 1;    /* U21_4.3 K-S11                                                                */
        int8u   IN_K10_NC           : 1;    /* U21_4.4 K-S19            PSL1_EN | PSL2_EN | PSL3_EN                         */
        int8u   IN_K21_NC           : 1;    /* U21_4.5 K-S17                                                                */
        int8u   IN_K30_NC           : 1;    /* U21_4.6 K-S20                                                                */
        int8u   IN_K2A_NO           : 1;    /* U21_4.7 K-S4                                                                 */
        
        /* ===== U22 ===== */
        int8u   OUT_PSC_OPN6        : 1;    /* U22_0.0 MCU 控制IO输出点，板内输出           */
        int8u   OUT_PSC_OPN8        : 1;    /* U22_0.1 MCU 控制IO输出点，板内输出           */
        int8u   OUT_IBP_OPN6        : 1;    /* U22_0.2 MCU 控制IO输出点，板内输出           */
        int8u   OUT_PSLx_OPN8       : 1;    /* U22_0.3 MCU 控制IO输出点，板内输出           */
        int8u   OUT_IBP_CLS         : 1;    /* U22_0.4 MCU 控制IO输出点，板内输出           */
        int8u   OUT_PSLx_OPN6       : 1;    /* U22_0.5 MCU 控制IO输出点，板内输出           */
        int8u   OUT_IBP_OPN8        : 1;    /* U22_0.6 MCU 控制IO输出点，板内输出           */
        int8u   OUT_PSLx_CLS        : 1;    /* U22_0.7 MCU 控制IO输出点，板内输出           */
        
        int8u                       : 1;    /* U22_1.0                                      */
        int8u                       : 1;    /* U22_1.1                                      */
        int8u                       : 1;    /* U22_1.2                                      */
        int8u                       : 1;    /* U22_1.3                                      */
        int8u                       : 1;    /* U22_1.4                                      */
        int8u                       : 1;    /* U22_1.5                                      */
        int8u                       : 1;    /* U22_1.6                                      */
        int8u                       : 1;    /* U22_1.7                                      */
        
        int8u                       : 1;    /* U22_2.0                                      */
        int8u                       : 1;    /* U22_2.1                                      */
        int8u                       : 1;    /* U22_2.2                                      */
        int8u                       : 1;    /* U22_2.3                                      */
        int8u   CO_DCU_OPN8_2       : 1;    /* U22_2.4 J3.15   板子输出命令监视点           */
        int8u   CO_DCU_CLS8_2       : 1;    /* U22_2.5 J3.17   板子输出命令监视点           */
        int8u   CO_DCU_CLS6_2       : 1;    /* U22_2.6 J3.16   板子输出命令监视点           */
        int8u                       : 1;    /* U22_2.7                                      */
        
        int8u   LOCK_OPN8           : 1;    /* U22_3.0 自锁输出OPN8                         */
        int8u   CO_DCU_RECLS_2      : 1;    /* U22_3.1 J3.18   板子输出命令监视点           */
        int8u   CO_DCU_DCU_OPSD2    : 1;    /* U22_3.2 J3.20   板子输出命令监视点           */
        int8u                       : 1;    /* U22_3.3                                      */
        int8u   CO_DCU_OPN6_2       : 1;    /* U22_3.4 J3.14   板子输出命令监视点           */
        int8u                       : 1;    /* U22_3.5                                      */
        int8u   CO_DCU_CLS8_1       : 1;    /* U22_3.6 J3.5    板子输出命令监视点           */
        int8u   CO_DCU_OPN6_1       : 1;    /* U22_3.7 J3.2    板子输出命令监视点           */
        
        int8u   CO_DCU_RECLS_1      : 1;    /* U22_4.0 J3.6    板子输出命令监视点           */
        int8u   CO_DCU_CLS6_1       : 1;    /* U22_4.1 J3.4    板子输出命令监视点           */
        int8u   OUT_LOCK_OPN6       : 1;    /* U22_4.2 自锁输出OPN6                         */
        int8u   CO_DCU_DCU_OPSD1    : 1;    /* U22_4.3 J3.8    板子输出命令监视点           */
        int8u   CO_DCU_IBPPSL_OPN1  : 1;    /* U22_4.4 J3.7    板子输出命令监视点           */
        int8u   CO_DCU_OPN8_1       : 1;    /* U22_4.5 J3.3    板子输出命令监视点           */
        int8u   IN_K0_NO            : 1;    /* U22_4.6 IBP_EN | IBPSD_EN | PSLX_EN | PSC_EN */
        int8u   IN_K7A_NO           : 1;    /* U22_4.7                                      */
    }fun;


    /* ===== 输入输出点 ===== */
    struct
    {
        /* ===== U20 ===== */
        int8u   I00 : 1;    /* U20_0.0  IBP_OP1       */ 
        int8u   I03 : 1;    /* U20_0.1  PSL1_OP1      */ 
        int8u   I01 : 1;    /* U20_0.2  IBP_OP2       */ 
        int8u   I02 : 1;    /* U20_0.3  PSL1_OP2      */ 
        int8u   I04 : 1;    /* U20_0.4  IBP_OP3       */ 
        int8u   I07 : 1;    /* U20_0.5  PSL1_OP3      */ 
        int8u   I05 : 1;    /* U20_0.6  IBP_OP4       */ 
        int8u   I06 : 1;    /* U20_0.7  PSL1_OP4      */ 
        
        int8u   I08 : 1;    /* U20_1.0  IBP_OP5       */ 
        int8u   I11 : 1;    /* U20_1.1  IN_BK0        */ 
        int8u   I09 : 1;    /* U20_1.2  IBP_OP6       */ 
        int8u   I10 : 1;    /* U20_1.3  PSL2_OP1      */ 
        int8u   I12 : 1;    /* U20_1.4  IBP_OP7       */ 
        int8u   I15 : 1;    /* U20_1.5  PSL2_OP2      */ 
        int8u   I13 : 1;    /* U20_1.6  IBP_OP8       */ 
        int8u   I14 : 1;    /* U20_1.7  PSL2_OP3      */ 
        
        int8u   I16 : 1;    /* U20_2.0  PSC_OP3       */ 
        int8u   I19 : 1;    /* U20_2.1  PSL2_OP4      */ 
        int8u   I17 : 1;    /* U20_2.2  PSC_OP1       */ 
        int8u   I18 : 1;    /* U20_2.3  IN_PN0        */ 
        int8u   I57 : 1;    /* U20_2.4  K_S1          */ 
        int8u   I58 : 1;    /* U20_2.5  K_S2          */ 
        int8u   I60 : 1;    /* U20_2.6  K_S3          */ 
        int8u   I63 : 1;    /* U20_2.7  K_S6          */ 
        
        int8u   I61 : 1;    /* U20_3.0  K_S5          */ 
        int8u   I62 : 1;    /* U20_3.1  K_S7          */ 
        int8u   I64 : 1;    /* U20_3.2  K_S8          */ 
        int8u   I67 : 1;    /* U20_3.3  K2_PN         */ 
        int8u   I65 : 1;    /* U20_3.4  K3_PN         */ 
        int8u   I66 : 1;    /* U20_3.5  K1_PN         */ 
        int8u   I68 : 1;    /* U20_3.6  K0_PN         */ 
        int8u   I71 : 1;    /* U20_3.7  K6_N          */ 
        
        int8u   I69 : 1;    /* U20_4.0  K4_N          */ 
        int8u   I70 : 1;    /* U20_4.1  K5_N          */ 
        int8u   I72 : 1;    /* U20_4.2  K30_PN        */ 
        int8u   I75 : 1;    /* U20_4.3  PSL1_OP0_N    */ 
        int8u   I73 : 1;    /* U20_4.4  K22_PN        */ 
        int8u   I74 : 1;    /* U20_4.5  K21_PN        */ 
        int8u   I76 : 1;    /* U20_4.6  PSL2_OP0_N    */ 
        int8u   I77 : 1;    /* U20_4.7  PSL3_OP0_N    */ 
        
        /* ===== U21 ===== */
        int8u   I20 : 1;    /* U21_0.0  PSC_OP2       */
        int8u   I23 : 1;    /* U21_0.1  IN_BK1        */
        int8u   I21 : 1;    /* U21_0.2  PSC_OP4       */
        int8u   I22 : 1;    /* U21_0.3  PSL3_OP1      */
        int8u   I24 : 1;    /* U21_0.4  PSL3_OP2      */
        int8u   I27 : 1;    /* U21_0.5  PSL3_OP3      */
        int8u   I25 : 1;    /* U21_0.6  IN_PN1        */
        int8u   I26 : 1;    /* U21_0.7  PSL3_OP4      */
        
        int8u   Q10 : 1;    /* U21_1.0  COM_Q_0       */
        int8u   Q11 : 1;    /* U21_1.1  COM_Q_1       */
        int8u   Q12 : 1;    /* U21_1.2  OUT_PN1       */
        int8u   Q14 : 1;    /* U21_1.3  IBP_Q_OPSD1   */
        int8u   Q15 : 1;    /* U21_1.4  IBP_Q_OPSD2   */
        int8u   Q16 : 1;    /* U21_1.5  IBP_PSL_Q1    */
        int8u   Q17 : 1;    /* U21_1.6  IBP_PSL_Q0    */
        int8u   Q08 : 1;    /* U21_1.7  PSC_Q_CL      */
        
        int8u   Q01 : 1;    /* U21_2.0  OUT_PN0       */
        int8u       : 1;    /* U21_2.1                */
        int8u       : 1;    /* U21_2.2                */
        int8u       : 1;    /* U21_2.3                */
        int8u       : 1;    /* U21_2.4                */
        int8u   I42 : 1;    /* U21_2.5  DCU_Q12       */
        int8u       : 1;    /* U21_2.6                */
        int8u       : 1;    /* U21_2.7                */
        
        int8u   I47 : 1;    /* U21_3.0  K_S14         */
        int8u       : 1;    /* U21_3.1                */
        int8u       : 1;    /* U21_3.2                */
        int8u   I46 : 1;    /* U21_3.3  K_S15         */
        int8u       : 1;    /* U21_3.4                */
        int8u       : 1;    /* U21_3.5                */
        int8u   I48 : 1;    /* U21_3.6  K_S16         */
        int8u   I51 : 1;    /* U21_3.7  K_S12         */
        
        int8u   I49 : 1;    /* U21_4.0  K_S18         */
        int8u   I50 : 1;    /* U21_4.1  K_S9          */
        int8u   I52 : 1;    /* U21_4.2  K_S10         */
        int8u   I55 : 1;    /* U21_4.3  K_S11         */
        int8u   I53 : 1;    /* U21_4.4  K_S19         */
        int8u   I54 : 1;    /* U21_4.5  K_S17         */
        int8u   I56 : 1;    /* U21_4.6  K_S20         */
        int8u   I59 : 1;    /* U21_4.7  K_S4          */
        
        /* ===== U22 ===== */
        int8u   Q09 : 1;    /* U22_0.0  PSC_Q_OP_6    */
        int8u   Q06 : 1;    /* U22_0.1  PSC_Q_OP_8    */
        int8u   Q07 : 1;    /* U22_0.2  IBP_Q_OP_6    */
        int8u   Q04 : 1;    /* U22_0.3  PSL1_Q_OP_8   */
        int8u   Q05 : 1;    /* U22_0.4  IBP_Q_CL      */
        int8u   Q02 : 1;    /* U22_0.5  PSL1_Q_OP_6   */
        int8u   Q03 : 1;    /* U22_0.6  IBP_Q_OP_8    */
        int8u   Q00 : 1;    /* U22_0.7  PSL1_Q_CL     */
        
        int8u       : 1;    /* U22_1.0                */
        int8u       : 1;    /* U22_1.1                */
        int8u       : 1;    /* U22_1.2                */
        int8u       : 1;    /* U22_1.3                */
        int8u       : 1;    /* U22_1.4                */
        int8u       : 1;    /* U22_1.5                */
        int8u       : 1;    /* U22_1.6                */
        int8u       : 1;    /* U22_1.7                */
        
        int8u       : 1;    /* U22_2.0                */
        int8u       : 1;    /* U22_2.1                */
        int8u       : 1;    /* U22_2.2                */
        int8u       : 1;    /* U22_2.3                */
        int8u   I31 : 1;    /* U22_2.4  DCU_Q08       */
        int8u   I30 : 1;    /* U22_2.5  DCU_Q10       */
        int8u   I35 : 1;    /* U22_2.6  DCU_Q09       */
        int8u       : 1;    /* U22_2.7                */
        
        int8u   I34 : 1;    /* U22_3.0  K8_N          */
        int8u   I39 : 1;    /* U22_3.1  DCU_Q11       */
        int8u   I38 : 1;    /* U22_3.2  DCU_Q13       */
        int8u       : 1;    /* U22_3.3                */
        int8u   I43 : 1;    /* U22_3.4  DCU_Q07       */
        int8u       : 1;    /* U22_3.5                */
        int8u   I28 : 1;    /* U22_3.6  DCU_Q03       */
        int8u   I29 : 1;    /* U22_3.7  DCU_Q00       */
        
        int8u   I32 : 1;    /* U22_4.0  DCU_Q04       */
        int8u   I33 : 1;    /* U22_4.1  DCU_Q02       */
        int8u   I36 : 1;    /* U22_4.2  K7_PN         */
        int8u   I37 : 1;    /* U22_4.3  DCU_Q06       */
        int8u   I40 : 1;    /* U22_4.4  DCU_Q05       */
        int8u   I41 : 1;    /* U22_4.5  DCU_Q01       */
        int8u   I44 : 1;    /* U22_4.6  K_S0          */
        int8u   I45 : 1;    /* U22_4.7  K_S13         */
    }IONum;
    
    /* 原理图标记的名字 */
    struct
    {
        /* ===== U20 ===== */
        int8u   IBP_OP1         : 1;    /* I00 U20_0.0 */
        int8u   PSL1_OP1        : 1;    /* I03 U20_0.1 */
        int8u   IBP_OP2         : 1;    /* I01 U20_0.2 */
        int8u   PSL1_OP2        : 1;    /* I02 U20_0.3 */
        int8u   IBP_OP3         : 1;    /* I04 U20_0.4 */
        int8u   PSL1_OP3        : 1;    /* I07 U20_0.5 */
        int8u   IBP_OP4         : 1;    /* I05 U20_0.6 */
        int8u   PSL1_OP4        : 1;    /* I06 U20_0.7 */
                                               
        int8u   IBP_OP5         : 1;    /* I08 U20_1.0 */
        int8u   IN_BK0          : 1;    /* I11 U20_1.1 */
        int8u   IBP_OP6         : 1;    /* I09 U20_1.2 */
        int8u   PSL2_OP1        : 1;    /* I10 U20_1.3 */
        int8u   IBP_OP7         : 1;    /* I12 U20_1.4 */
        int8u   PSL2_OP2        : 1;    /* I15 U20_1.5 */
        int8u   IBP_OP8         : 1;    /* I13 U20_1.6 */
        int8u   PSL2_OP3        : 1;    /* I14 U20_1.7 */
                                               
        int8u   PSC_OP3         : 1;    /* I16 U20_2.0 */
        int8u   PSL2_OP4        : 1;    /* I19 U20_2.1 */
        int8u   PSC_OP1         : 1;    /* I17 U20_2.2 */
        int8u   IN_PN0          : 1;    /* I18 U20_2.3 */
        int8u   K_S1            : 1;    /* I57 U20_2.4 */
        int8u   K_S2            : 1;    /* I58 U20_2.5 */
        int8u   K_S3            : 1;    /* I60 U20_2.6 */
        int8u   K_S6            : 1;    /* I63 U20_2.7 */
                                               
        int8u   K_S5            : 1;    /* I61 U20_3.0 */
        int8u   K_S7            : 1;    /* I62 U20_3.1 */
        int8u   K_S8            : 1;    /* I64 U20_3.2 */
        int8u   K2_PN           : 1;    /* I67 U20_3.3 */
        int8u   K3_PN           : 1;    /* I65 U20_3.4 */
        int8u   K1_PN           : 1;    /* I66 U20_3.5 */
        int8u   K0_PN           : 1;    /* I68 U20_3.6 */
        int8u   K6_N            : 1;    /* I71 U20_3.7 */
                                               
        int8u   K4_N            : 1;    /* I69 U20_4.0 */
        int8u   K5_N            : 1;    /* I70 U20_4.1 */
        int8u   K30_PN          : 1;    /* I72 U20_4.2 */
        int8u   PSL1_OP0_N      : 1;    /* I75 U20_4.3 */
        int8u   K22_PN          : 1;    /* I73 U20_4.4 */
        int8u   K21_PN          : 1;    /* I74 U20_4.5 */
        int8u   PSL2_OP0_N      : 1;    /* I76 U20_4.6 */
        int8u   PSL3_OP0_N      : 1;    /* I77 U20_4.7 */
        
        /* ===== U21 ===== */
        int8u   PSC_OP2         : 1;    /* I20 U21_0.0 */
        int8u   IN_BK1          : 1;    /* I23 U21_0.1 */
        int8u   PSC_OP4         : 1;    /* I21 U21_0.2 */
        int8u   PSL3_OP1        : 1;    /* I22 U21_0.3 */
        int8u   PSL3_OP2        : 1;    /* I24 U21_0.4 */
        int8u   PSL3_OP3        : 1;    /* I27 U21_0.5 */
        int8u   IN_PN1          : 1;    /* I25 U21_0.6 */
        int8u   PSL3_OP4        : 1;    /* I26 U21_0.7 */
                                               
        int8u   COM_Q_0         : 1;    /* Q10 U21_1.0 */
        int8u   COM_Q_1         : 1;    /* Q11 U21_1.1 */
        int8u   OUT_PN1         : 1;    /* Q12 U21_1.2 */
        int8u   IBP_Q_OPSD1     : 1;    /* Q14 U21_1.3 */
        int8u   IBP_Q_OPSD2     : 1;    /* Q15 U21_1.4 */
        int8u   IBP_PSL_Q1      : 1;    /* Q16 U21_1.5 */
        int8u   IBP_PSL_Q0      : 1;    /* Q17 U21_1.6 */
        int8u   PSC_Q_CL        : 1;    /* Q08 U21_1.7 */
        
        int8u   OUT_PN0         : 1;    /* Q01 U21_2.0 */
        int8u                   : 1;    /*     U21_2.1 */
        int8u                   : 1;    /*     U21_2.2 */
        int8u                   : 1;    /*     U21_2.3 */
        int8u                   : 1;    /*     U21_2.4 */
        int8u   DCU_Q12         : 1;    /* I42 U21_2.5 */
        int8u                   : 1;    /*     U21_2.6 */
        int8u                   : 1;    /*     U21_2.7 */
                                               
        int8u   K_S14           : 1;    /* I47 U21_3.0 */
        int8u                   : 1;    /*     U21_3.1 */
        int8u                   : 1;    /*     U21_3.2 */
        int8u   K_S15           : 1;    /* I46 U21_3.3 */
        int8u                   : 1;    /*     U21_3.4 */
        int8u                   : 1;    /*     U21_3.5 */
        int8u   K_S16           : 1;    /* I48 U21_3.6 */
        int8u   K_S12           : 1;    /* I51 U21_3.7 */
                                               
        int8u   K_S18           : 1;    /* I49 U21_4.0 */
        int8u   K_S9            : 1;    /* I50 U21_4.1 */
        int8u   K_S10           : 1;    /* I52 U21_4.2 */
        int8u   K_S11           : 1;    /* I55 U21_4.3 */
        int8u   K_S19           : 1;    /* I53 U21_4.4 */
        int8u   K_S17           : 1;    /* I54 U21_4.5 */
        int8u   K_S20           : 1;    /* I56 U21_4.6 */
        int8u   K_S4            : 1;    /* I59 U21_4.7 */
        
        /* ===== U22 ===== */
        int8u   PSC_Q_OP_6      : 1;    /* Q09 U22_0.0 */
        int8u   PSC_Q_OP_8      : 1;    /* Q06 U22_0.1 */
        int8u   IBP_Q_OP_6      : 1;    /* Q07 U22_0.2 */
        int8u   PSL1_Q_OP_8     : 1;    /* Q04 U22_0.3 */
        int8u   IBP_Q_CL        : 1;    /* Q05 U22_0.4 */
        int8u   PSL1_Q_OP_6     : 1;    /* Q02 U22_0.5 */
        int8u   IBP_Q_OP_8      : 1;    /* Q03 U22_0.6 */
        int8u   PSL1_Q_CL       : 1;    /* Q00 U22_0.7 */
                                               
        int8u                   : 1;    /*     U22_1.0 */
        int8u                   : 1;    /*     U22_1.1 */
        int8u                   : 1;    /*     U22_1.2 */
        int8u                   : 1;    /*     U22_1.3 */
        int8u                   : 1;    /*     U22_1.4 */
        int8u                   : 1;    /*     U22_1.5 */
        int8u                   : 1;    /*     U22_1.6 */
        int8u                   : 1;    /*     U22_1.7 */
                                               
        int8u                   : 1;    /*     U22_2.0 */
        int8u                   : 1;    /*     U22_2.1 */
        int8u                   : 1;    /*     U22_2.2 */
        int8u                   : 1;    /*     U22_2.3 */
        int8u   DCU_Q08         : 1;    /* I31 U22_2.4 */
        int8u   DCU_Q10         : 1;    /* I30 U22_2.5 */
        int8u   DCU_Q09         : 1;    /* I35 U22_2.6 */
        int8u                   : 1;    /*     U22_2.7 */
                                               
        int8u   K8_N            : 1;    /* I34 U22_3.0 */
        int8u   DCU_Q11         : 1;    /* I39 U22_3.1 */
        int8u   DCU_Q13         : 1;    /* I38 U22_3.2 */
        int8u                   : 1;    /*     U22_3.3 */
        int8u   DCU_Q07         : 1;    /* I43 U22_3.4 */
        int8u                   : 1;    /*     U22_3.5 */
        int8u   DCU_Q03         : 1;    /* I28 U22_3.6 */
        int8u   DCU_Q00         : 1;    /* I29 U22_3.7 */
                                               
        int8u   DCU_Q04         : 1;    /* I32 U22_4.0 */
        int8u   DCU_Q02         : 1;    /* I33 U22_4.1 */
        int8u   K7_PN           : 1;    /* I36 U22_4.2 */
        int8u   DCU_Q06         : 1;    /* I37 U22_4.3 */
        int8u   DCU_Q05         : 1;    /* I40 U22_4.4 */
        int8u   DCU_Q01         : 1;    /* I41 U22_4.5 */
        int8u   K_S0            : 1;    /* I44 U22_4.6 */
        int8u   K_S13           : 1;    /* I45 U22_4.7 */
    }alias;
   
    /* 继电器线圈及触点 */
    struct
    {
        /* ===== U20 ===== */
        int8u                   : 1;    /* I00 U20_0.0      */
        int8u                   : 1;    /* I03 U20_0.1      */
        int8u                   : 1;    /* I01 U20_0.2      */
        int8u                   : 1;    /* I02 U20_0.3      */
        int8u                   : 1;    /* I04 U20_0.4      */
        int8u                   : 1;    /* I07 U20_0.5      */
        int8u                   : 1;    /* I05 U20_0.6      */
        int8u                   : 1;    /* I06 U20_0.7      */
                                                            
        int8u                   : 1;    /* I08 U20_1.0      */
        int8u                   : 1;    /* I11 U20_1.1      */
        int8u                   : 1;    /* I09 U20_1.2      */
        int8u                   : 1;    /* I10 U20_1.3      */
        int8u                   : 1;    /* I12 U20_1.4      */
        int8u                   : 1;    /* I15 U20_1.5      */
        int8u                   : 1;    /* I13 U20_1.6      */
        int8u                   : 1;    /* I14 U20_1.7      */
                                                            
        int8u                   : 1;    /* I16 U20_2.0      */
        int8u                   : 1;    /* I19 U20_2.1      */
        int8u                   : 1;    /* I17 U20_2.2      */
        int8u                   : 1;    /* I18 U20_2.3      */
        int8u   K1B_NO          : 1;    /* I57 U20_2.4 K_S1 */
        int8u   K1A_NO          : 1;    /* I58 U20_2.5 K_S2 */
        int8u   K2B_NO          : 1;    /* I60 U20_2.6 K_S3 */
        int8u   K3A_NO          : 1;    /* I63 U20_2.7 K_S6 */
                                                    
        int8u   K3B_NO          : 1;    /* I61 U20_3.0 K_S5 */
        int8u   K4A_NO          : 1;    /* I62 U20_3.1 K_S7 */
        int8u   K4B_NO          : 1;    /* I64 U20_3.2 K_S8 */
        int8u   K2_PN           : 1;    /* I67 U20_3.3      */
        int8u   K3_PN           : 1;    /* I65 U20_3.4      */
        int8u   K1_PN           : 1;    /* I66 U20_3.5      */
        int8u   K0_PN           : 1;    /* I68 U20_3.6      */
        int8u   K6_N            : 1;    /* I71 U20_3.7      */
                                                            
        int8u   K4_N            : 1;    /* I69 U20_4.0      */
        int8u   K5_N            : 1;    /* I70 U20_4.1      */
        int8u   K30_PN          : 1;    /* I72 U20_4.2      */
        int8u                   : 1;    /* I75 U20_4.3      */
        int8u   K22_PN          : 1;    /* I73 U20_4.4      */
        int8u   K21_PN          : 1;    /* I74 U20_4.5      */
        int8u                   : 1;    /* I76 U20_4.6      */
        int8u                   : 1;    /* I77 U20_4.7      */
        
        /* ===== U21 ===== */
        int8u                   : 1;    /* I20 U21_0.0      */
        int8u                   : 1;    /* I23 U21_0.1      */
        int8u                   : 1;    /* I21 U21_0.2      */
        int8u                   : 1;    /* I22 U21_0.3      */
        int8u                   : 1;    /* I24 U21_0.4      */
        int8u                   : 1;    /* I27 U21_0.5      */
        int8u                   : 1;    /* I25 U21_0.6      */
        int8u                   : 1;    /* I26 U21_0.7      */
                                                            
        int8u                   : 1;    /* Q10 U21_1.0      */
        int8u                   : 1;    /* Q11 U21_1.1      */
        int8u                   : 1;    /* Q12 U21_1.2      */
        int8u                   : 1;    /* Q14 U21_1.3      */
        int8u                   : 1;    /* Q15 U21_1.4      */
        int8u                   : 1;    /* Q16 U21_1.5      */
        int8u                   : 1;    /* Q17 U21_1.6      */
        int8u                   : 1;    /* Q08 U21_1.7      */
        
        int8u                   : 1;    /* Q01 U21_2.0      */
        int8u                   : 1;    /*     U21_2.1      */
        int8u                   : 1;    /*     U21_2.2      */
        int8u                   : 1;    /*     U21_2.3      */
        int8u                   : 1;    /*     U21_2.4      */
        int8u                   : 1;    /* I42 U21_2.5      */
        int8u                   : 1;    /*     U21_2.6      */
        int8u                   : 1;    /*     U21_2.7      */
                                               
        int8u  K7B_NO           : 1;    /* I47 U21_3.0 K_S14*/
        int8u                   : 1;    /*     U21_3.1      */
        int8u                   : 1;    /*     U21_3.2      */
        int8u  K8A_NO           : 1;    /* I46 U21_3.3 K_S15*/
        int8u                   : 1;    /*     U21_3.4      */
        int8u                   : 1;    /*     U21_3.5      */
        int8u  K8B_NO           : 1;    /* I48 U21_3.6 K_S16*/
        int8u  K6B_NO           : 1;    /* I51 U21_3.7 K_S12*/
                                                      
        int8u  K22_NO           : 1;    /* I49 U21_4.0 K_S18*/
        int8u  K5A_NO           : 1;    /* I50 U21_4.1 K_S9 */
        int8u  K5B_NO           : 1;    /* I52 U21_4.2 K_S10*/
        int8u  K6A_NO           : 1;    /* I55 U21_4.3 K_S11*/
        int8u  K10_NC           : 1;    /* I53 U21_4.4 K_S19*/
        int8u  K21_NC           : 1;    /* I54 U21_4.5 K_S17*/
        int8u  K30_NC           : 1;    /* I56 U21_4.6 K_S20*/
        int8u  K2A_NO           : 1;    /* I59 U21_4.7 K_S4 */
        
        /* ===== U22 ===== */
        int8u                   : 1;    /* Q09 U22_0.0      */
        int8u                   : 1;    /* Q06 U22_0.1      */
        int8u                   : 1;    /* Q07 U22_0.2      */
        int8u                   : 1;    /* Q04 U22_0.3      */
        int8u                   : 1;    /* Q05 U22_0.4      */
        int8u                   : 1;    /* Q02 U22_0.5      */
        int8u                   : 1;    /* Q03 U22_0.6      */
        int8u                   : 1;    /* Q00 U22_0.7      */
                                                            
        int8u                   : 1;    /*     U22_1.0      */
        int8u                   : 1;    /*     U22_1.1      */
        int8u                   : 1;    /*     U22_1.2      */
        int8u                   : 1;    /*     U22_1.3      */
        int8u                   : 1;    /*     U22_1.4      */
        int8u                   : 1;    /*     U22_1.5      */
        int8u                   : 1;    /*     U22_1.6      */
        int8u                   : 1;    /*     U22_1.7      */
                                                            
        int8u                   : 1;    /*     U22_2.0      */
        int8u                   : 1;    /*     U22_2.1      */
        int8u                   : 1;    /*     U22_2.2      */
        int8u                   : 1;    /*     U22_2.3      */
        int8u                   : 1;    /* I31 U22_2.4      */
        int8u                   : 1;    /* I30 U22_2.5      */
        int8u                   : 1;    /* I35 U22_2.6      */
        int8u                   : 1;    /*     U22_2.7      */
                                                            
        int8u   K8_N            : 1;    /* I34 U22_3.0      */
        int8u                   : 1;    /* I39 U22_3.1      */
        int8u                   : 1;    /* I38 U22_3.2      */
        int8u                   : 1;    /*     U22_3.3      */
        int8u                   : 1;    /* I43 U22_3.4      */
        int8u                   : 1;    /*     U22_3.5      */
        int8u                   : 1;    /* I28 U22_3.6      */
        int8u                   : 1;    /* I29 U22_3.7      */
                                                            
        int8u                   : 1;    /* I32 U22_4.0      */
        int8u                   : 1;    /* I33 U22_4.1      */
        int8u   K7_PN           : 1;    /* I36 U22_4.2      */
        int8u                   : 1;    /* I37 U22_4.3      */
        int8u                   : 1;    /* I40 U22_4.4      */
        int8u                   : 1;    /* I41 U22_4.5      */
        int8u  K0_NO            : 1;    /* I44 U22_4.6 K_S0 */
        int8u  K7A_NO           : 1;    /* I45 U22_4.7 K_S13*/
    }relay;
    
    
    tPCA9505IO  ios[15];        /* U20 U21 U22 */
    int8u       bytes[15];
}tExtIO;


#define ADDR_K_S1       (((0 * 5 + 2) << 8) + (1<<4))   /* I57 U20_2.4 : K1B */
#define ADDR_K_S2       (((0 * 5 + 2) << 8) + (1<<5))   /* I58 U20_2.5 : K1A */
#define ADDR_K_S3       (((0 * 5 + 2) << 8) + (1<<6))   /* I60 U20_2.6 : K2B */
#define ADDR_K_S6       (((0 * 5 + 2) << 8) + (1<<7))   /* I63 U20_2.7 : K3A */
#define ADDR_K_S5       (((0 * 5 + 3) << 8) + (1<<0))   /* I61 U20_3.0 : K3B */
#define ADDR_K_S7       (((0 * 5 + 3) << 8) + (1<<1))   /* I62 U20_3.1 : K4A */
#define ADDR_K_S8       (((0 * 5 + 3) << 8) + (1<<2))   /* I64 U20_3.2 : K4B */
#define ADDR_K_S14      (((1 * 5 + 3) << 8) + (1<<0))   /* I47 U21_3.0 : K7B */
#define ADDR_K_S15      (((1 * 5 + 3) << 8) + (1<<3))   /* I46 U21_3.3 : K8A */
#define ADDR_K_S16      (((1 * 5 + 3) << 8) + (1<<6))   /* I48 U21_3.6 : K8B */
#define ADDR_K_S12      (((1 * 5 + 3) << 8) + (1<<7))   /* I51 U21_3.7 : K6B */
#define ADDR_K_S18      (((1 * 5 + 4) << 8) + (1<<0))   /* I49 U21_4.0 : K22 */
#define ADDR_K_S9       (((1 * 5 + 4) << 8) + (1<<1))   /* I50 U21_4.1 : K5A */
#define ADDR_K_S10      (((1 * 5 + 4) << 8) + (1<<2))   /* I52 U21_4.2 : K5B */
#define ADDR_K_S11      (((1 * 5 + 4) << 8) + (1<<3))   /* I55 U21_4.3 : K6A */
#define ADDR_K_S19      (((1 * 5 + 4) << 8) + (1<<4))   /* I53 U21_4.4 : K10 */
#define ADDR_K_S17      (((1 * 5 + 4) << 8) + (1<<5))   /* I54 U21_4.5 : K21 */
#define ADDR_K_S20      (((1 * 5 + 4) << 8) + (1<<6))   /* I56 U21_4.6 : K30 */
#define ADDR_K_S4       (((1 * 5 + 4) << 8) + (1<<7))   /* I59 U21_4.7 : K2A */
#define ADDR_K_S0       (((2 * 5 + 4) << 8) + (1<<6))   /* I44 U22_4.6 : K0  */
#define ADDR_K_S13      (((2 * 5 + 4) << 8) + (1<<7))   /* I45 U22_4.7 : K7A */

/* =============================================================================
 * 功能接口函数
 *  */
PUBLIC tErrCmd processIOLogic(tReadLOG_IO* pIO);


#endif  //_IOLOGIC_H

