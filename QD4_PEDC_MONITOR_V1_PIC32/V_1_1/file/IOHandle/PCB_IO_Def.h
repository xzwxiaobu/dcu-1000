
#ifndef _PCB_IO_DEF_H
#define _PCB_IO_DEF_H

/* =============================================================================
 * 适用项目：青岛4
 * 原理图： PEDC_MONITOR_SZ16.pdf - PIC24
 */

/* =============================================================================
 * PEDC_MONITOR_SZ16 PCB IO defines
 *  3 颗PCA9505的输入输出
 *  input:  U20PCA -
 *          U21PCA      IO1  IO2  IO3  IO4
 *          U22PCA IO0  IO1  IO2  IO3  IO4
 *  output: U20PCA IO0  IO1  IO2  IO3  IO4
 *          U21PCA IO0  IO1
 *          U22PCA -
 */
#define MON_input_RB_L          MON_input_GPIO[0]    
#define MON_input_RB_H          MON_input_GPIO[1]    
#define MON_input_RC_L          MON_input_GPIO[2]    
#define MON_input_RC_H          MON_input_GPIO[3]    
#define MON_input_RD_L          MON_input_GPIO[4]    
#define MON_input_RD_H          MON_input_GPIO[5]    
#define MON_input_RE_L          MON_input_GPIO[6]    
#define MON_input_RE_H          MON_input_GPIO[7]    
#define MON_input_RF_L          MON_input_GPIO[8]    
#define MON_input_RF_H          MON_input_GPIO[9]    
#define MON_input_RG_L          MON_input_GPIO[10]   
#define MON_input_RG_H          MON_input_GPIO[11]   

#define IN_U21_IO1              MON_input_data[1 + 5 * 1]   /* U21 */
#define IN_U21_IO2              MON_input_data[2 + 5 * 1]   
#define IN_U21_IO3              MON_input_data[3 + 5 * 1]   
#define IN_U21_IO4              MON_input_data[4 + 5 * 1]   
#define IN_U22_IO0              MON_input_data[0 + 5 * 2]   /* U22 */
#define IN_U22_IO1              MON_input_data[1 + 5 * 2]   
#define IN_U22_IO2              MON_input_data[2 + 5 * 2]   
#define IN_U22_IO3              MON_input_data[3 + 5 * 2]   
#define IN_U22_IO4              MON_input_data[4 + 5 * 2]   
   
#define OUT_U20_IO0             MON_output_data[0]          /* U20 */
#define OUT_U20_IO1             MON_output_data[1]      
#define OUT_U20_IO2             MON_output_data[2]      
#define OUT_U20_IO3             MON_output_data[3]      
#define OUT_U20_IO4             MON_output_data[4]      
#define OUT_U21_IO0             MON_output_data[0 + 5]      /* U21 */
#define OUT_U21_IO1             MON_output_data[1 + 5]  

#define MON_I00         IN_U21_IO1.B4
#define MON_I01         IN_U21_IO1.B6
#define MON_I02         IN_U21_IO1.B7
#define MON_I03         IN_U21_IO1.B5
#define MON_I04         IN_U21_IO2.B0
#define MON_I05         IN_U21_IO2.B2
#define MON_I06         IN_U21_IO2.B3
#define MON_I07         IN_U21_IO2.B1
#define MON_I08         IN_U22_IO0.B0
#define MON_I09         IN_U22_IO0.B2
#define MON_I10         IN_U22_IO0.B3
#define MON_I11         IN_U22_IO0.B1
#define MON_I12         IN_U22_IO0.B4
#define MON_I13         IN_U22_IO0.B6
#define MON_I14         IN_U22_IO0.B7
#define MON_I15         IN_U22_IO0.B5
#define MON_I16         IN_U22_IO1.B0
#define MON_I17         IN_U22_IO1.B2
#define MON_I18         IN_U22_IO1.B3
#define MON_I19         IN_U22_IO1.B1
#define MON_I20         IN_U22_IO1.B4
#define MON_I21         IN_U22_IO1.B6
#define MON_I22         IN_U22_IO1.B7
#define MON_I23         IN_U22_IO1.B5
#define MON_I24         IN_U22_IO2.B0
#define MON_I25         IN_U22_IO2.B2
#define MON_I26         IN_U22_IO2.B3
#define MON_I27         IN_U22_IO2.B1
#define MON_I28         IN_U21_IO4.B4
#define MON_I29         IN_U21_IO4.B6
#define MON_I30         IN_U21_IO4.B7
#define MON_I31         IN_U21_IO4.B5
#define MON_I32         IN_U21_IO4.B0
#define MON_I33         MON_input_RB_H.B4       /* RB12 */ 
#define MON_I34         IN_U21_IO4.B3
#define MON_I35         IN_U21_IO4.B1
#define MON_I36         MON_input_RB_H.B5       /* RB13 */ 
#define MON_I37         IN_U21_IO3.B6
#define MON_I38         IN_U21_IO3.B7
#define MON_I39         IN_U21_IO3.B5
#define MON_I40         IN_U21_IO3.B0
#define MON_I41         IN_U21_IO3.B2
#define MON_I42         MON_input_RE_L.B2       /* RE2 */ 
#define MON_I43         MON_input_RE_L.B4       /* RE4 */ 
#define MON_I44         IN_U21_IO2.B4
#define MON_I45         IN_U21_IO2.B6
#define MON_I46         MON_input_RE_L.B3       /* RE3 */ 
#define MON_I47         IN_U21_IO2.B5
#define MON_I48         IN_U22_IO4.B4
#define MON_I49         IN_U22_IO4.B6
#define MON_I50         IN_U22_IO4.B7
#define MON_I51         IN_U22_IO4.B5
#define MON_I52         IN_U22_IO4.B0
#define MON_I53         IN_U22_IO4.B2
#define MON_I54         IN_U22_IO4.B3
#define MON_I55         IN_U22_IO4.B1
#define MON_I56         IN_U22_IO3.B4
#define MON_I57         IN_U22_IO3.B6
#define MON_I58         IN_U22_IO3.B7
#define MON_I59         IN_U22_IO3.B5
#define MON_I60         IN_U22_IO3.B0
#define MON_I61         IN_U22_IO3.B2
#define MON_I62         IN_U22_IO3.B3
#define MON_I63         IN_U22_IO3.B1
#define MON_I64         IN_U22_IO2.B4
#define MON_I65         IN_U22_IO2.B6
#define MON_I66         IN_U22_IO2.B7
#define MON_I67         IN_U22_IO2.B5


#define MON_Q00         OUT_U20_IO4.B7   
#define MON_Q01         OUT_U20_IO4.B6   
#define MON_Q02         OUT_U20_IO0.B0   
#define MON_Q03         OUT_U20_IO0.B1   
#define MON_Q04         OUT_U20_IO0.B2   
#define MON_Q05         OUT_U20_IO0.B3   
#define MON_Q06         OUT_U20_IO0.B4   
#define MON_Q07         OUT_U20_IO0.B5   
#define MON_Q08         OUT_U20_IO0.B6   
#define MON_Q09         OUT_U20_IO0.B7   
#define MON_Q10         OUT_U20_IO1.B0   
#define MON_Q11         OUT_U20_IO1.B1   
#define MON_Q12         OUT_U20_IO1.B2   
#define MON_Q13         OUT_U20_IO1.B3   
#define MON_Q14         OUT_U20_IO1.B4   
#define MON_Q15         OUT_U20_IO1.B5   
#define MON_Q16         OUT_U20_IO1.B6   
#define MON_Q17         OUT_U20_IO1.B7   
#define MON_Q18         OUT_U20_IO2.B0   
#define MON_Q19         OUT_U20_IO2.B1   
#define MON_Q20         OUT_U20_IO2.B2   
#define MON_Q21         OUT_U20_IO2.B3   
#define MON_Q22         OUT_U21_IO0.B0   
#define MON_Q23         OUT_U21_IO0.B1   
#define MON_Q24         OUT_U21_IO0.B2   
#define MON_Q25         OUT_U21_IO0.B3   
#define MON_Q26         OUT_U21_IO0.B4   
#define MON_Q27         OUT_U21_IO0.B5   
#define MON_Q28         OUT_U21_IO0.B6   
#define MON_Q29         OUT_U21_IO0.B7   
#define MON_Q30         OUT_U21_IO1.B0   
#define MON_Q31         OUT_U21_IO1.B1   
#define MON_Q32         OUT_U21_IO1.B2   
#define MON_Q33         OUT_U21_IO1.B3   
#define MON_Q34         MON_Output_RBL.B5       /* GPIO B.5 */
#define MON_Q35         MON_Output_RBL.B3       /* GPIO B.3 */    
#define MON_Q36         MON_Output_RBL.B2       /* GPIO B.2 */    
#define MON_Q37         OUT_U20_IO3.B1   
#define MON_Q38         OUT_U20_IO3.B2   
#define MON_Q39         OUT_U20_IO3.B3   
#define MON_Q40         OUT_U20_IO3.B4   
#define MON_Q41         OUT_U20_IO3.B5   
#define MON_Q42         OUT_U20_IO3.B6   
#define MON_Q43         OUT_U20_IO3.B7   
#define MON_Q44         OUT_U20_IO4.B0   
#define MON_Q45         OUT_U20_IO4.B1   
#define MON_Q46         OUT_U20_IO4.B2   
#define MON_Q47         OUT_U20_IO4.B3   
#define MON_Q48         OUT_U20_IO4.B4   

/* ===== PEDC_MONITOR_SZ16: 端子  J1 ===== */
#define MO_Led_PSC_OpnSta       MON_Q03  /* J1-02  PSC-Q00     PSC-开门状态指示灯                  */
#define MO_Led_PSC_DoorLcked    MON_Q05  /* J1-03  PSC-Q01     PSC-所有ASD/EED关闭锁紧指示灯       */
#define MO_Led_PSC_ManUnlock    MON_Q07  /* J1-04  PSC-Q02     PSC-滑动门手动/隔离操作指示灯       */
#define MO_Led_PSC_OpnClsFail   MON_Q06  /* J1-05  PSC-Q03     PSC-滑动门开门/关门故障指示灯       */
#define MO_Led_PSC_IBPEn        MON_Q09  /* J1-06  PSC-Q04     PSC-IBP操作允许指示灯               */
#define MO_Led_PSC_PSLByPass    MON_Q08  /* J1-07  PSC-Q05     PSC-PSL互锁解除指示灯               */
#define MO_Led_PSC_PowerFail    MON_Q11  /* J1-08  PSC-Q06     PSC-供电电源故障指示灯              */
#define MO_Led_PSC_CanFail      MON_Q10  /* J1-09  PSC-Q07     PSC-现场总线故障指示灯              */
#define MO_Led_CtrlSystemFail   MON_Q13  /* J1-10  PSC-Q08     PSC-控制系统故障指示灯   10-11 修改错误  */
#define MO_PSC_Buzzer           MON_Q12  /* J1-11  PSC-Q09     PSC-蜂鸣器                          */
#define MO_Led_MonitorSysFail   MON_Q15  /* J1-12  PSC-Q10     PSC--监视系统故障指示灯             */
//#define MO_LookoutLight         MON_Q01  /* J1-13  PSC-Q11     PSC-照明灯带(2022-4-13 修改为Q04) */
#define MO_LOG_Q2_LOG2          MON_Q00  /* J1-14  PSC-Q12     PSC-自动位钥匙开关选择逻辑板        */
//#define                       MON_Q02  /* J1-15  PSC-Q13                                         */
#define MO_LookoutLight         MON_Q04  /* J1-16&17  Q-PN0    PSC-?望灯带(2022-4-13 修改)        */
#define MI_ResetKey             MON_I00  /* J1-18  PSC-OP0     PSC-消音按钮                        */
#define MI_TestLight            MON_I03  /* J1-19  PSC-OP1     PSC-试灯按钮                        */
#define MI_LOG2_Switch          MON_I01  /* J1-20  PSC-OP2     PSC-钥匙开关B档监视                 */
#define MI_LOGAutoMode          MON_I02  /* J1-21  PSC-OP3     PSC-钥匙开关自动挡监视              */
#define MI_LOG1_Switch          MON_I04  /* J1-22  PSC-OP4     PSC-钥匙开关A档监视                 */
#define MI_JG_SaftyLoop         MON_I07  /* J1-23  PSC-OP5     PSC-JG安全回路监视                  */

/* ===== PEDC_MONITOR_SZ16: 端子  J2 ===== */
#define MO_Led_PSLx_AllOpned    MON_Q16  /* J2-02  PSL-Q00     PSLx-站台门全开启指示灯  */
#define MO_Led_PSL1_IBPEn       MON_Q18  /* J2-03  PSL-Q01     PSL1-IBP操作允许指示灯   */
#define MO_Led_PSLx_AllClsed    MON_Q20  /* J2-04  PSL-Q02     PSLx-站台门全关指示灯    */
#define MO_Led_PSL2_IBPEn       MON_Q22  /* J2-05  PSL-Q03     PSL2-IBP操作允许指示灯   */
#define MO_Led_PSL1_EN          MON_Q25  /* J2-06  PSL-Q04     PSL1-操作允许指示灯      */
#define MO_Led_PSL1_Bypass      MON_Q24  /* J2-07  PSL-Q05     PSL1-互锁解除指示灯      */
#define MO_Led_PSL2_EN          MON_Q27  /* J2-08  PSL-Q06     PSL2-操作允许指示灯      */
#define MO_Led_PSL2_Bypass      MON_Q26  /* J2-09  PSL-Q07     PSL2-互锁解除指示灯      */
#define MO_Led_PSL1Interlock    MON_Q23  /* J2-10  PSL-Q08     PSL1-同侧互锁指示灯      */
//#define MO_                   MON_Q21  /* J2-11  PSL-Q09     PSL1-                   */
#define MO_Led_PSL2Interlock    MON_Q19  /* J2-12  PSL-Q10     PSL2-同侧互锁指示灯      */
//#define MO_                   MON_Q17  /* J2-13  PSL-Q11     PSL2-                   */
#define MO_IlluminationLight    MON_Q14  /* J2-14  Q-PN1       PSC-照明灯带(2022-4-13 修改) */
//#define MO_                   MON_Q14  /* J2-15  Q-PN1           - */
#define MI_PSL1TestLight        MON_I15  /* J2-16  PSL-OP00    PSL1-试灯按钮 */
//#define                       MON_I05  /* J2-17  PSL-OP01    PSL - */
//#define                       MON_I06  /* J2-18  PSL-OP02    PSL - */
//#define                       MON_I08  /* J2-19  PSL-OP03    PSL - */
#define MI_PSL1_Bypass          MON_I11  /* J2-20  PSL-OP04    PSL1-互锁解除开关触点监视 */

#define MI_PSL2TestLight        MON_I09  /* J2-21  PSL-OP05    PSL2-试灯按钮 */
//#define                       MON_I10  /* J2-22  PSL-OP06    PSL - */
//#define                       MON_I12  /* J2-23  PSL-OP07    PSL - */
//#define                       MON_I13  /* J2-24  PSL-OP08    PSL - */
#define MI_PSL2_Bypass          MON_I14  /* J2-25  PSL-OP09    PSL2-互锁解除开关触点监视    */

//#define                       MON_I16  /* J2-26  PSL-OP10    PSL - */
//#define                       MON_I19  /* J2-27  PSL-OP11    PSL - */
//#define                       MON_I17  /* J2-28  PSL-OP12    PSL - */
//#define                       MON_I18  /* J2-29  PSL-OP13    PSL - */
//#define                       MON_I20  /* J2-30  PSL-OP14    PSL - */
//#define                       MON_I23  /* J2-31  PSL-OP15          */


/* ===== PEDC_MONITOR_SZ16: 端子  J3 ===== */
#define MO_Led_IBP_DoorClsed    MON_Q29  /* J3-02  IBP-Q0      IBP-站台门关闭指示灯   */
#define MO_Led_IBP_DoorOpned    MON_Q28  /* J3-03  IBP-Q1      IBP-站台门开启指示灯   */
#define MO_Led_IBP_HeadTailOpn  MON_Q31  /* J3-04  IBP-Q2      IBP-首末门开启指示灯   */
//#define MO_                   MON_Q30  /* J3-05  IBP-Q3      IBP-电源故障指示灯     */
//#define MO_                   MON_Q33  /* J3-06  IBP-Q4      IBP-蜂鸣器             */
//#define MO_                   MON_Q32  /* J3-07&08  Q-PN2    */
//#define MI_                   MON_I25  /* J3-09  IBP-OP0     */
//#define MI_                   MON_I26  /* J3-10  IBP-OP1     */
//#define MI_                   MON_I21  /* J3-11  IBP-OP2     */
//#define MI_                   MON_I22  /* J3-12  IBP-OP3     */
//#define MI_                   MON_I24  /* J3-13  IBP-OP4     */
//#define MI_                   MON_I27  /* J3-14  IBP-OP5     */
//#define MI_                   MON_I64  /* J3-15  IBP-OP6      */
//#define MI_                   MON_I67  /* J3-16&17  IN-PN0    */
//#define MI_                   MON_I65  /* J3-18&19  IN-PN1    */ 


/* ===== PEDC_MONITOR_SZ16: 端子  J4 ===== */
#define MI_DCU_OPN8_1           MON_I34 /* J4-02  DCU-IN0 DCU-开门命令1       */
#define MI_DCU_CLS8_1           MON_I28 /* J4-03  DCU-IN1 DCU-关门命令1       */
#define MI_DCU_OPNSD_1          MON_I31 /* J4-04  DCU-IN2 DCU-开边门命令1     */
#define MI_DCU_CLSSD_1          MON_I29 /* J4-05  DCU-IN3 DCU-关边门命令1     */
#define MI_DCU_IN4              MON_I30 /* J4-06  DCU-IN4                    */
#define MO_DCU_Q0               MON_Q39 /* J4-07  DCU-Q0                     */
#define MO_DCU_Q1               MON_Q41 /* J4-08  DCU-Q1                     */
#define MO_DCU_Q2               MON_Q43 /* J4-09  DCU-Q2                     */
#define MO_DCU_Q3               MON_Q45 /* J4-10  DCU-Q3                     */
#define MO_DCU_Q4               MON_Q47 /* J4-11  DCU-Q4                     */
#define MI_DCU_OPN8_2           MON_I39 /* J4-14  DCU-IN5 DCU-开门命令2       */
#define MI_DCU_CLS8_2           MON_I37 /* J4-15  DCU-IN6 DCU-关门命令2       */
#define MI_DCU_OPNSD_2          MON_I38 /* J4-16  DCU-IN7 DCU-开边门命令2     */
#define MI_DCU_CLSSD_2          MON_I32 /* J4-17  DCU-IN8 DCU-关边门命令2     */
#define MI_DCU_IN9              MON_I35 /* J4-18  DCU-IN9                    */
#define MO_DCU_Q5               MON_Q40 /* J4-19  DCU-Q5                     */
#define MO_DCU_Q6               MON_Q42 /* J4-20  DCU-Q6                     */
#define MO_DCU_Q7               MON_Q44 /* J4-21  DCU-Q7                     */
#define MO_DCU_Q8               MON_Q46 /* J4-22  DCU-Q8                     */
#define MO_DCU_Q9               MON_Q48 /* J4-23  DCU-Q9                     */

/* ===== PEDC_MONITOR_SZ16: 端子  J5 ===== */
#define MI_SF_NO                MON_I46 /* J5-03  SF_NO                          */
#define MI_SF_NC                MON_I43 /* J5-04  SF_NC                          */
#define MI_IN_PN2               MON_I42 /* J5-06-07  IN-PN2   */
#define MI_IN_PN3               MON_I36 /* J5-08-09  IN-PN3   */
#define MI_Cls_WindWindow       MON_I33 /* J5-10-11  IN-PN4  通风窗手动关        */
#define MI_Opn_WindWindow       MON_I49 /* J5-12-13  IN-PN5  通风窗手动开        */
#define MI_MainPowFail          MON_I50 /* J5-14  POW-S0      主电源故障         */
#define MI_DriverPowFail        MON_I44 /* J5-15  POW-S1      驱动电源故障       */
#define MI_CtrlPowFail          MON_I47 /* J5-16  POW-S2      控制电源故障       */
#define MI_DriverBatFail        MON_I45 /* J5-17  POW-S3      驱动电池故障       */
#define MI_CtrlBatFail          MON_I40 /* J5-18  POW-S4      控制电池故障       */
#define MI_POW_S5               MON_I41 /* J5-19  POW-S5                         */
#define MO_Q_PN3                MON_Q34 /* J5-21-22  Q-PN3                       */
#define MO_Q_PN4                MON_Q35 /* J5-23-24  Q-PN4                       */

/* ===== PEDC_MONITOR_SZ16: 端子  J6 ===== */
//#define MI_       MON_I59 /* J6-01-2   */
//#define MI_       MON_I58 /* J6-03-4   */
//#define MI_       MON_I52 /* J6-06     */
//#define MI_       MON_I55 /* J6-07     */
//#define MI_       MON_I53 /* J6-10     */
//#define MI_       MON_I54 /* J6-11     */
//#define MI_       MON_I48 /* J6-13-14  */
//#define MI_       MON_I51 /* J6-15-16  */
//#define MI_       MON_I66 /* J6-18     */
//#define MI_       MON_I60 /* J6-19     */
//#define MI_       MON_I63 /* J6-22     */
//#define MI_       MON_I61 /* J6-23     */
#define MI_SIG_OpnCmd              MON_I62 /* J6-26     信号开门监视  */
#define MI_SIG_ClsCmd              MON_I56 /* J6-27     信号关门监视  */
//#define MI_       MON_I57 /* J6-28     */
//#define MO_       MON_Q38 /* J6-29     */
//#define MO_       MON_Q37 /* J6-30     */
//#define MO_       MON_Q36 /* J6-31     */

/* =============================================================================
 * 切换板 PCB IO defines
 */
#define SWCH_input_RB_L         SWCH_input_data[0]
#define SWCH_input_RB_H         SWCH_input_data[1]
#define SWCH_input_RC_L         SWCH_input_data[2]
#define SWCH_input_RC_H         SWCH_input_data[3]
#define SWCH_input_RD_L         SWCH_input_data[4]
#define SWCH_input_RD_H         SWCH_input_data[5]
#define SWCH_input_RE_L         SWCH_input_data[6]
#define SWCH_input_RE_H         SWCH_input_data[7]
#define SWCH_input_RF_L         SWCH_input_data[8]
#define SWCH_input_RF_H         SWCH_input_data[9]
#define SWCH_input_RG_L         SWCH_input_data[10]
#define SWCH_input_RG_H         SWCH_input_data[11]

#define SWCH_I00                SWCH_input_RD_H.B0     //RD8  K-S0    K0_NC
#define SWCH_I01                SWCH_input_RD_H.B1     //RD9  K-S2    K2_NC
#define SWCH_I02                SWCH_input_RD_H.B2     //RD10 K-S3    K3_NC
#define SWCH_I03                SWCH_input_RD_H.B3     //RD11 K-S1    K1_NC
#define SWCH_I04                SWCH_input_RD_L.B0     //RD0  K-S5    K5_NC
#define SWCH_I05                SWCH_input_RC_H.B5     //RC13 K-S11   K22_NO
#define SWCH_I06                SWCH_input_RC_H.B6     //RC14 K-S6    K6_NC
#define SWCH_I07                SWCH_input_RD_L.B1     //RD1  K-S7    K11_NC DCU-SF继电器触点
#define SWCH_I08                SWCH_input_RD_L.B6     //RD6  K-S8    K12_NC
#define SWCH_I09                SWCH_input_RD_L.B7     //RD7  K-S9    K13_NO
#define SWCH_I10                SWCH_input_RF_L.B0     //RF0  K-S10   K21_NO DCU-BY继电器触点
#define SWCH_I11                SWCH_input_RF_L.B1     //RF1  K-S4    K4_NC
#define SWCH_I12                SWCH_input_RE_L.B0     //RE0  K-S12   K23_NC
#define SWCH_I13                SWCH_input_RE_L.B1     //RE1  K11-PN  DCU-SF 继电器线圈
#define SWCH_I14                SWCH_input_RE_L.B2     //RE2  K21-PN  DCU-BY 继电器线圈
#define SWCH_I15                SWCH_input_RE_L.B3     //RE3  K0-N    K0   继电器线圈
#define SWCH_I16                SWCH_input_RE_L.B5     //RE5  K1-N    K123 继电器线圈并联
#define SWCH_I17                SWCH_input_RE_L.B6     //RE6  K2-N    K456 继电器线圈并联
#define SWCH_I18                SWCH_input_RE_L.B7     //RE7  IN-0
#define SWCH_I19                SWCH_input_RG_L.B6     //RG6  IN-1
#define SWCH_I20                SWCH_input_RB_L.B4     //RB4  IN-2
#define SWCH_I21                SWCH_input_RB_L.B3     //RB3  IN-PN0

#define SWCH_Q00                SWCH_input_RB_L.B2     //Q1-SWITCH 
#define SWCH_Q01                SWCH_input_RB_L.B6     //Q2-SWITCH
#define SWCH_Q02                SWCH_input_RB_L.B7     //RESET/SELECT1-Q
#define SWCH_Q03                SWCH_input_RB_H.B3     //Q-PN0

#define SWCH_K0_POW             SWCH_I15

#define SWCH_K1_POW             SWCH_I16
#define SWCH_K2_POW             SWCH_K1_POW
#define SWCH_K3_POW             SWCH_K1_POW

#define SWCH_K4_POW             SWCH_I17
#define SWCH_K5_POW             SWCH_K4_POW
#define SWCH_K6_POW             SWCH_K4_POW

#define SWCH_K11_POW            SWCH_I13
#define SWCH_K12_POW            SWCH_K11_POW

#define SWCH_K21_POW            SWCH_I14
#define SWCH_K22_POW            SWCH_K21_POW

#define SWCH_K0_NC              SWCH_I00
#define SWCH_K1_NC              SWCH_I03
#define SWCH_K2_NC              SWCH_I01
#define SWCH_K3_NC              SWCH_I02
#define SWCH_K4_NC              SWCH_I11
#define SWCH_K5_NC              SWCH_I04
#define SWCH_K6_NC              SWCH_I06
#define SWCH_K11_NC             SWCH_I07
#define SWCH_K12_NC             SWCH_I08
#define SWCH_K13_NO             SWCH_I09
#define SWCH_K21_NO             SWCH_I10
#define SWCH_K22_NO             SWCH_I05
#define SWCH_K23_NC             SWCH_I12


//#define SWCH_DCUSF1_POW             SWCH_K11_POW    //DCU-SF1,      J1-18 安全回路返回-


//#define SWCH_Q0_DCU_OPN6_1          SWCH_K1_NC /* J3.1    开门1  */
//#define SWCH_Q1_DCU_CLS6_1          SWCH_K1_NC /* J3.2    关门1  */
//#define SWCH_Q2_DCU_RECLS           SWCH_K2_NC /* J3.3    重关门 */
//#define SWCH_Q3_DCU_IBPPSL_OPN      SWCH_K2_NC /* J3.4    IBPPSL开门 */
//#define SWCH_Q4_DCU_DCU_OPSD        SWCH_K3_NC /* J3.5    开边门 */
//#define SWCH_Q5_DCU_OPN6_2          SWCH_K3_NC /* J3.6    开门2  */
//#define SWCH_Q6_DCU_CLS6_2          SWCH_K4_NC /* J3.7    关门2  */


/* =============================================================================
 * LOG1 PCB IO defines
 * 注意：PEDC_SZ16_V1.pdf 与 PEDC_SZ16.pdf 有差异
 * */


/* 逻辑板 1 */
#define LOG1_U20_IO0    LOG1_input_data[00]
#define LOG1_U20_IO1    LOG1_input_data[01]
#define LOG1_U20_IO2    LOG1_input_data[02]
#define LOG1_U20_IO3    LOG1_input_data[03]
#define LOG1_U20_IO4    LOG1_input_data[04]
#define LOG1_U21_IO0    LOG1_input_data[05]
#define LOG1_U21_IO1    LOG1_input_data[06]
#define LOG1_U21_IO2    LOG1_input_data[07]
#define LOG1_U21_IO3    LOG1_input_data[08]
#define LOG1_U21_IO4    LOG1_input_data[09]
#define LOG1_U22_IO0    LOG1_input_data[10]
#define LOG1_U22_IO1    LOG1_input_data[11]
#define LOG1_U22_IO2    LOG1_input_data[12]
#define LOG1_U22_IO3    LOG1_input_data[13]
#define LOG1_U22_IO4    LOG1_input_data[14]

#define LOG1_I00     LOG1_U20_IO0.B0    //IBP_OP1         JI_IBP_OPN6     J1.4    监视输入命令 IBPOPN6
#define LOG1_I01     LOG1_U20_IO0.B2    //IBP_OP2         JI_IBP_OPN8     J1.5    监视输入命令 IBPOPN8
#define LOG1_I02     LOG1_U20_IO0.B3    //PSL1_OP2        JI_PSL1_OPN8    J1.21
#define LOG1_I03     LOG1_U20_IO0.B1    //PSL1_OP1        JI_PSL1_OPN6    J1.20
#define LOG1_I04     LOG1_U20_IO0.B4    //IBP_OP3         JI_IBP_CLS6     J1.6    监视输入命令 IBPCLS6
#define LOG1_I05     LOG1_U20_IO0.B6    //IBP_OP4         JI_IBP_CLS8     J1.7    监视输入命令 IBPCLS8
#define LOG1_I06     LOG1_U20_IO0.B7    //PSL1_OP4        JI_PSL1_CLS8    J1.23
#define LOG1_I07     LOG1_U20_IO0.B5    //PSL1_OP3        JI_PSL1_CLS6    J1.22
#define LOG1_I08     LOG1_U20_IO1.B0    //IBP_OP5         JI_IBP_OPNSD1   J1.10   监视输入命令
#define LOG1_I09     LOG1_U20_IO1.B2    //IBP_OP6         JI_IBP_OPNSD2   J1.11   监视输入命令
#define LOG1_I10     LOG1_U20_IO1.B3    //PSL2_OP1        JI_PSL2_OPN6    J1.27
#define LOG1_I11     LOG1_U20_IO1.B1    //IN_BK0                          J1.24
#define LOG1_I12     LOG1_U20_IO1.B4    //IBP_OP7         JI_IBP_CLSSD1   J1.12   监视输入命令
#define LOG1_I13     LOG1_U20_IO1.B6    //IBP_OP8         JI_IBP_CLSSD2   J1.13   监视输入命令
#define LOG1_I14     LOG1_U20_IO1.B7    //PSL2_OP3        JI_PSL2_CLS6    J1.29
#define LOG1_I15     LOG1_U20_IO1.B5    //PSL2_OP2        JI_PSL2_OPN8    J1.28
#define LOG1_I16     LOG1_U20_IO2.B0    //PSC_OP3         JI_PSC_CLS6     J2.20
#define LOG1_I17     LOG1_U20_IO2.B2    //PSC_OP1         JI_PSC_OPN6     J2.18
#define LOG1_I18     LOG1_U20_IO2.B3    //IN_PN0                          J1.14&15
#define LOG1_I19     LOG1_U20_IO2.B1    //PSL2_OP4        JI_PSL2_CLS8    J1.30
#define LOG1_I20     LOG1_U21_IO0.B0    //PSC_OP2         JI_PSC_OPN8     J2.19
#define LOG1_I21     LOG1_U21_IO0.B2    //PSC_OP4         JI_PSC_CLS8     J2.21
#define LOG1_I22     LOG1_U21_IO0.B3    //PSL3_OP1        JI_PSL3_OPN6    J2.4
#define LOG1_I23     LOG1_U21_IO0.B1    //IN_BK1                          J1.31
#define LOG1_I24     LOG1_U21_IO0.B4    //PSL3_OP2        JI_PSL3_OPN8    J2.5
#define LOG1_I25     LOG1_U21_IO0.B6    //IN_PN1                          J2.25&26        输入，接地用于测试自动运行的功能
#define LOG1_I26     LOG1_U21_IO0.B7    //PSL3_OP4        JI_PSL3_CLS8    J2.7
#define LOG1_I27     LOG1_U21_IO0.B5    //PSL3_OP3        JI_PSL3_CLS6    J2.6
#define LOG1_I28     LOG1_U22_IO3.B6    //DCU_Q03         CO_DCU_CLS8_1       J3.5    板子输出命令监视点
#define LOG1_I29     LOG1_U22_IO3.B7    //DCU_Q00         CO_DCU_OPN6_1       J3.2    板子输出命令监视点
#define LOG1_I30     LOG1_U22_IO2.B5    //DCU_Q10         CO_DCU_CLS8_2       J3.17   板子输出命令监视点
#define LOG1_I31     LOG1_U22_IO2.B4    //DCU_Q08         CO_DCU_OPN8_2       J3.15   板子输出命令监视点
#define LOG1_I32     LOG1_U22_IO4.B0    //DCU_Q04         CO_DCU_RECLS_1      J3.6    板子输出命令监视点
#define LOG1_I33     LOG1_U22_IO4.B1    //DCU_Q02         CO_DCU_CLS6_1       J3.4    板子输出命令监视点
#define LOG1_I34     LOG1_U22_IO3.B0    //K8_N            ALL_OPN8_LOCK           自锁输出OPN8
#define LOG1_I35     LOG1_U22_IO2.B6    //DCU_Q09         CO_DCU_CLS6_2       J3.16   板子输出命令监视点
#define LOG1_I36     LOG1_U22_IO4.B2    //K7_PN           ALL_OPN6_LOCK           自锁输出OPN6
#define LOG1_I37     LOG1_U22_IO4.B3    //DCU_Q06         CO_DCU_DCU_OPSD1    J3.8    板子输出命令监视点
#define LOG1_I38     LOG1_U22_IO3.B2    //DCU_Q13         CO_DCU_DCU_OPSD2    J3.20   板子输出命令监视点
#define LOG1_I39     LOG1_U22_IO3.B1    //DCU_Q11         CO_DCU_RECLS_2      J3.18   板子输出命令监视点
#define LOG1_I40     LOG1_U22_IO4.B4    //DCU_Q05         CO_DCU_IBPPSL_OPN1  J3.7    板子输出命令监视点
#define LOG1_I41     LOG1_U22_IO4.B5    //DCU_Q01         CO_DCU_OPN8_1       J3.3    板子输出命令监视点
#define LOG1_I42     LOG1_U21_IO2.B5    //DCU_Q12         CO_DCU_IBPPSL_OPN2  J3.19   板子输出命令监视点
#define LOG1_I43     LOG1_U22_IO3.B4    //DCU_Q07         CO_DCU_OPN6_2       J3.14   板子输出命令监视点
#define LOG1_I44     LOG1_U22_IO4.B6    //K_S0            K0_NO           IBP_EN | IBPSD_EN | PSLX_EN | PSC_EN
#define LOG1_I45     LOG1_U22_IO4.B7    //K_S13           K7A_NO
#define LOG1_I46     LOG1_U21_IO3.B3    //K_S15           K8A_NO
#define LOG1_I47     LOG1_U21_IO3.B0    //K_S14           K7B_NO
#define LOG1_I48     LOG1_U21_IO3.B6    //K_S16           K8B_NO
#define LOG1_I49     LOG1_U21_IO4.B0    //K_S18           K22_NO
#define LOG1_I50     LOG1_U21_IO4.B1    //K_S9            K5A_NO
#define LOG1_I51     LOG1_U21_IO3.B7    //K_S12           K6B_NO
#define LOG1_I52     LOG1_U21_IO4.B2    //K_S10           K5B_NO
#define LOG1_I53     LOG1_U21_IO4.B4    //K_S19           K10_NC          PSL1_EN | PSL2_EN | PSL3_EN
#define LOG1_I54     LOG1_U21_IO4.B5    //K_S17           K21_NC
#define LOG1_I55     LOG1_U21_IO4.B3    //K_S11           K6A_NO
#define LOG1_I56     LOG1_U21_IO4.B6    //K_S20           K30_NC
#define LOG1_I57     LOG1_U20_IO2.B4    //K_S1            K1B_NO
#define LOG1_I58     LOG1_U20_IO2.B5    //K_S2            K1A_NO
#define LOG1_I59     LOG1_U21_IO4.B7    //K_S4            K2A_NO
#define LOG1_I60     LOG1_U20_IO2.B6    //K_S3            K2B_NO
#define LOG1_I61     LOG1_U20_IO3.B0    //K_S5            K3B_NO
#define LOG1_I62     LOG1_U20_IO3.B1    //K_S7            K4A_NO
#define LOG1_I63     LOG1_U20_IO2.B7    //K_S6            K3A_NO
#define LOG1_I64     LOG1_U20_IO3.B2    //K_S8            K4B_NO
#define LOG1_I65     LOG1_U20_IO3.B4    //K3_PN           JI_SIG_CLS      J2.13&14        信号系统命令 关车门
#define LOG1_I66     LOG1_U20_IO3.B5    //K1_PN           JI_SIG_OPN6     J2.9&10 信号系统命令 开6车
#define LOG1_I67     LOG1_U20_IO3.B3    //K2_PN           JI_SIG_OPN8     J2.11&12信号系统命令 开8车
#define LOG1_I68     LOG1_U20_IO3.B6    //K0_PN           IBPPSLPSC_EN            IBP_EN | PSLX_EN | PSC_TEST
#define LOG1_I69     LOG1_U20_IO4.B0    //K4_N            ALL_OPN6                IBP / PSL / PSC / SIG OPN6
#define LOG1_I70     LOG1_U20_IO4.B1    //K5_N            ALL_OPN8                IBP / PSL / PSC / SIG OPN8
#define LOG1_I71     LOG1_U20_IO3.B7    //K6_N            ALL_CLS                 IBP / PSL / PSC / SIG CLS
#define LOG1_I72     LOG1_U20_IO4.B2    //K30_PN          JI_PSC_TEST     J2.16&17PSC测试使能
#define LOG1_I73     LOG1_U20_IO4.B4    //K22_PN          JI_IBP_A_EN     J1.8&9  IBP开边门使能
#define LOG1_I74     LOG1_U20_IO4.B5    //K21_PN          JI_IBP_EN       J1.2&3  IBP 使能
#define LOG1_I75     LOG1_U20_IO4.B3    //PSL1_OP0_N      JI_PSL1_EN      J1.18&19PSL1 使能
#define LOG1_I76     LOG1_U20_IO4.B6    //PSL2_OP0_N      JI_PSL2_EN      J1.25&26PSL2 使能
#define LOG1_I77     LOG1_U20_IO4.B7    //PSL3_OP0_N      JI_PSL3_EN      J2.2&3  PSL3 使能
#define LOG1_Q00     LOG1_U22_IO0.B7    //PSL1_Q_CL       OUT_PSLx_CLS            MCU 控制IO输出点，板内输出
#define LOG1_Q01     LOG1_U21_IO2.B0    //OUT_PN0                         J2.27&28端子输出点，备用
#define LOG1_Q02     LOG1_U22_IO0.B5    //PSL2_Q_OP_6     OUT_PSLx_OPN6           MCU 控制IO输出点，板内输出
#define LOG1_Q03     LOG1_U22_IO0.B6    //IBP_Q_OP_8      OUT_IBP_OPN8            MCU 控制IO输出点，板内输出
#define LOG1_Q04     LOG1_U22_IO0.B3    //PSL2_Q_OP_8     OUT_PSLx_OPN8           MCU 控制IO输出点，板内输出
#define LOG1_Q05     LOG1_U22_IO0.B4    //IBP_Q_CL        OUT_IBP_CLS             MCU 控制IO输出点，板内输出
#define LOG1_Q06     LOG1_U22_IO0.B1    //PSC_Q_OP_8      OUT_PSC_OPN8            MCU 控制IO输出点，板内输出
#define LOG1_Q07     LOG1_U22_IO0.B2    //IBP_Q_OP_6      OUT_IBP_OPN6            MCU 控制IO输出点，板内输出
#define LOG1_Q08     LOG1_U21_IO1.B7    //PSC_Q_CL        OUT_PSC_CLS             MCU 控制IO输出点，板内输出
#define LOG1_Q09     LOG1_U22_IO0.B0    //PSC_Q_OP_6      OUT_PSC_OPN6            MCU 控制IO输出点，板内输出
#define LOG1_Q10     LOG1_U21_IO1.B0    //COM_Q_0         CO_Q0           J3.9    备用输出给DCU
#define LOG1_Q11     LOG1_U21_IO1.B1    //COM_Q_1         CO_Q1           J3.21   备用输出给DCU
#define LOG1_Q12     LOG1_U21_IO1.B2    //OUT_PN1                         J3.11&12端子输出点，备用
//#define LOG1_Q13     LOG1_U21_IO1.B3    //OUT_PN2                       J3.23&24端子输出点，备用 ==> PEDC_SZ16_V1.pdf 有变化
#define LOG1_Q14     LOG1_U21_IO1.B3    //IBP_Q_OPSD1     OUT_IBP_OPNSD0  J3.19   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD0
#define LOG1_Q15     LOG1_U21_IO1.B4    //IBP_Q_OPSD2     OUT_IBP_OPNSD1  J3.20   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD1
#define LOG1_Q16     LOG1_U21_IO1.B5    //IBP_PSL_Q1      OUT_IBPPSL_Q1   J3.8    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q1
#define LOG1_Q17     LOG1_U21_IO1.B6    //IBP_PSL_Q0      OUT_IBPPSL_Q0   J3.7    MCU 控制IO输出点，板内输出到端

#define L1_JI_IBP_EN            LOG1_I74    //J1.2&3  IBP 使能
#define L1_JI_IBP_OPN6          LOG1_I00    //J1.4    监视输入命令 IBPOPN6
#define L1_JI_IBP_OPN8          LOG1_I01    //J1.5    监视输入命令 IBPOPN8
#define L1_JI_IBP_CLS6          LOG1_I04    //J1.6    监视输入命令 IBPCLS6
#define L1_JI_IBP_CLS8          LOG1_I05    //J1.7    监视输入命令 IBPCLS8
#define L1_JI_IBP_OPNSD1        LOG1_I08    //J1.10   监视输入命令
#define L1_JI_IBP_OPNSD2        LOG1_I09    //J1.11   监视输入命令
#define L1_JI_IBP_CLSSD1        LOG1_I12    //J1.12   监视输入命令
#define L1_JI_IBP_CLSSD2        LOG1_I13    //J1.13   监视输入命令
#define L1_JI_IBP_A_EN          LOG1_I73    //J1.8&9  IBP开边门使能
                                
#define L1_JI_PSL1_EN           LOG1_I75    //J1.18&19PSL1 使能
#define L1_JI_PSL1_OPN8         LOG1_I02    //J1.21
#define L1_JI_PSL1_OPN6         LOG1_I03    //J1.20
#define L1_JI_PSL1_CLS8         LOG1_I06    //J1.23
#define L1_JI_PSL1_CLS6         LOG1_I07    //J1.22

#define L1_JI_PSL2_EN           LOG1_I76    //J1.25&26PSL2 使能
#define L1_JI_PSL2_OPN8         LOG1_I15    //J1.28
#define L1_JI_PSL2_OPN6         LOG1_I10    //J1.27
#define L1_JI_PSL2_CLS6         LOG1_I14    //J1.29
#define L1_JI_PSL2_CLS8         LOG1_I19    //J1.30

#define L1_JI_PSL3_EN           LOG1_I77    //J2.2&3  PSL3 使能
#define L1_JI_PSL3_OPN6         LOG1_I22    //J2.4
#define L1_JI_PSL3_OPN8         LOG1_I24    //J2.5
#define L1_JI_PSL3_CLS6         LOG1_I27    //J2.6
#define L1_JI_PSL3_CLS8         LOG1_I26    //J2.7

#define L1_JI_PSC_TEST          LOG1_I72    //J2.16&17PSC测试使能
#define L1_JI_PSC_OPN6          LOG1_I17    //J2.18
#define L1_JI_PSC_OPN8          LOG1_I20    //J2.19
#define L1_JI_PSC_CLS6          LOG1_I16    //J2.20
#define L1_JI_PSC_CLS8          LOG1_I21    //J2.21

#define L1_IN_BK0               LOG1_I11    //J1.24
#define L1_IN_PN0               LOG1_I18    //J1.14&15
#define L1_IN_BK1               LOG1_I23    //J1.31
#define L1_IN_PN1               LOG1_I25    //J2.25&26        输入，接地用于测试自动运行的功能

#define L1_ALL_OPN6_LOCK        LOG1_I36    //   自锁输出OPN6
#define L1_ALL_OPN8_LOCK        LOG1_I34    //   自锁输出OPN8

#define L1_CO_DCU_OPN6_1        LOG1_I29    //J3.2    板子输出命令监视点
#define L1_CO_DCU_OPN8_1        LOG1_I41    //J3.3    板子输出命令监视点
#define L1_CO_DCU_CLS6_1        LOG1_I33    //J3.4    板子输出命令监视点
#define L1_CO_DCU_CLS8_1        LOG1_I28    //J3.5    板子输出命令监视点
#define L1_CO_DCU_RECLS_1       LOG1_I32    //J3.6    板子输出命令监视点
#define L1_CO_DCU_IBPPSL_OPN1   LOG1_I40    //J3.7    板子输出命令监视点
#define L1_CO_DCU_DCU_OPSD1     LOG1_I37    //J3.8    板子输出命令监视点

#define L1_CO_DCU_OPN6_2        LOG1_I43    //J3.14   板子输出命令监视点
#define L1_CO_DCU_OPN8_2        LOG1_I31    //J3.15   板子输出命令监视点
#define L1_CO_DCU_CLS6_2        LOG1_I35    //J3.16   板子输出命令监视点
#define L1_CO_DCU_CLS8_2        LOG1_I30    //J3.17   板子输出命令监视点
#define L1_CO_DCU_RECLS_2       LOG1_I39    //J3.18   板子输出命令监视点
#define L1_CO_DCU_IBPPSL_OPN2   LOG1_I42    //J3.19   板子输出命令监视点
#define L1_CO_DCU_DCU_OPSD2     LOG1_I38    //J3.20   板子输出命令监视点

#define L1_JI_SIG_OPN6          LOG1_I66    //J2.9&10 信号系统命令 开6车
#define L1_JI_SIG_OPN8          LOG1_I67    //J2.11&12信号系统命令 开8车
#define L1_JI_SIG_CLS           LOG1_I65    //J2.13&14        信号系统命令 关车门

#define L1_IBPPSLPSC_EN         LOG1_I68    //        IBP_EN | PSLX_EN | PSC_TEST
#define L1_ALL_OPN6             LOG1_I69    //        IBP / PSL / PSC / SIG OPN6
#define L1_ALL_OPN8             LOG1_I70    //        IBP / PSL / PSC / SIG OPN8
#define L1_ALL_CLS              LOG1_I71    //        IBP / PSL / PSC / SIG CLS

#define L1_K0_NO                LOG1_I44    //        IBP_EN | IBPSD_EN | PSLX_EN | PSC_EN
#define L1_K7A_NO               LOG1_I45    //
#define L1_K8A_NO               LOG1_I46    //
#define L1_K7B_NO               LOG1_I47    //
#define L1_K8B_NO               LOG1_I48    //
#define L1_K22_NO               LOG1_I49    //
#define L1_K5A_NO               LOG1_I50    //
#define L1_K6B_NO               LOG1_I51    //
#define L1_K5B_NO               LOG1_I52    //
#define L1_K10_NC               LOG1_I53    //        PSL1_EN | PSL2_EN | PSL3_EN
#define L1_K21_NC               LOG1_I54    //
#define L1_K6A_NO               LOG1_I55    //
#define L1_K30_NC               LOG1_I56    //
#define L1_K1B_NO               LOG1_I57    //
#define L1_K1A_NO               LOG1_I58    //
#define L1_K2A_NO               LOG1_I59    //
#define L1_K2B_NO               LOG1_I60    //
#define L1_K3B_NO               LOG1_I61    //
#define L1_K4A_NO               LOG1_I62    //
#define L1_K3A_NO               LOG1_I63    //
#define L1_K4B_NO               LOG1_I64    //
#define L1_OUT_PSLx_CLS         LOG1_Q00    //        MCU 控制IO输出点，板内输出
#define L1_OUT_PN0              LOG1_Q01    //J2.27&28端子输出点，备用
#define L1_OUT_PSLx_OPN6        LOG1_Q02    //        MCU 控制IO输出点，板内输出
#define L1_OUT_IBP_OPN8         LOG1_Q03    //        MCU 控制IO输出点，板内输出
#define L1_OUT_PSLx_OPN8        LOG1_Q04    //        MCU 控制IO输出点，板内输出
#define L1_OUT_IBP_CLS          LOG1_Q05    //        MCU 控制IO输出点，板内输出
#define L1_OUT_PSC_OPN8         LOG1_Q06    //        MCU 控制IO输出点，板内输出
#define L1_OUT_IBP_OPN6         LOG1_Q07    //        MCU 控制IO输出点，板内输出
#define L1_OUT_PSC_CLS          LOG1_Q08    //        MCU 控制IO输出点，板内输出
#define L1_OUT_PSC_OPN6         LOG1_Q09    //        MCU 控制IO输出点，板内输出
#define L1_CO_Q0                LOG1_Q10    //J3.9    备用输出给DCU
#define L1_CO_Q1                LOG1_Q11    //J3.21   备用输出给DCU
#define L1_OUT_PN1              LOG1_Q12    //J3.11&12端子输出点，备用
//#define L1_OUT_PN2            LOG1_Q13    //J3.23&24端子输出点，备用
#define L1_OUT_IBP_OPNSD0       LOG1_Q14    //J3.19   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD0
#define L1_OUT_IBP_OPNSD1       LOG1_Q15    //J3.20   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD1
#define L1_OUT_IBPPSL_Q1        LOG1_Q16    //J3.8    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q1
#define L1_OUT_IBPPSL_Q0        LOG1_Q17    //J3.7    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q0

/* 逻辑板 2 */
#define LOG2_U20_IO0    LOG2_input_data[00]
#define LOG2_U20_IO1    LOG2_input_data[01]
#define LOG2_U20_IO2    LOG2_input_data[02]
#define LOG2_U20_IO3    LOG2_input_data[03]
#define LOG2_U20_IO4    LOG2_input_data[04]
#define LOG2_U21_IO0    LOG2_input_data[05]
#define LOG2_U21_IO1    LOG2_input_data[06]
#define LOG2_U21_IO2    LOG2_input_data[07]
#define LOG2_U21_IO3    LOG2_input_data[08]
#define LOG2_U21_IO4    LOG2_input_data[09]
#define LOG2_U22_IO0    LOG2_input_data[10]
#define LOG2_U22_IO1    LOG2_input_data[11]
#define LOG2_U22_IO2    LOG2_input_data[12]
#define LOG2_U22_IO3    LOG2_input_data[13]
#define LOG2_U22_IO4    LOG2_input_data[14]

#define LOG2_I00     LOG2_U20_IO0.B0    //IBP_OP1         JI_IBP_OPN6     J1.4    监视输入命令 IBPOPN6
#define LOG2_I01     LOG2_U20_IO0.B2    //IBP_OP2         JI_IBP_OPN8     J1.5    监视输入命令 IBPOPN8
#define LOG2_I02     LOG2_U20_IO0.B3    //PSL2_OP2        JI_PSL2_OPN8    J1.21
#define LOG2_I03     LOG2_U20_IO0.B1    //PSL2_OP1        JI_PSL2_OPN6    J1.20
#define LOG2_I04     LOG2_U20_IO0.B4    //IBP_OP3         JI_IBP_CLS6     J1.6    监视输入命令 IBPCLS6
#define LOG2_I05     LOG2_U20_IO0.B6    //IBP_OP4         JI_IBP_CLS8     J1.7    监视输入命令 IBPCLS8
#define LOG2_I06     LOG2_U20_IO0.B7    //PSL2_OP4        JI_PSL2_CLS8    J1.23
#define LOG2_I07     LOG2_U20_IO0.B5    //PSL2_OP3        JI_PSL2_CLS6    J1.22
#define LOG2_I08     LOG2_U20_IO1.B0    //IBP_OP5         JI_IBP_OPNSD1   J1.10   监视输入命令
#define LOG2_I09     LOG2_U20_IO1.B2    //IBP_OP6         JI_IBP_OPNSD2   J1.11   监视输入命令
#define LOG2_I10     LOG2_U20_IO1.B3    //PSL2_OP1        JI_PSL2_OPN6    J1.27
#define LOG2_I11     LOG2_U20_IO1.B1    //IN_BK0                          J1.24
#define LOG2_I12     LOG2_U20_IO1.B4    //IBP_OP7         JI_IBP_CLSSD1   J1.12   监视输入命令
#define LOG2_I13     LOG2_U20_IO1.B6    //IBP_OP8         JI_IBP_CLSSD2   J1.13   监视输入命令
#define LOG2_I14     LOG2_U20_IO1.B7    //PSL2_OP3        JI_PSL2_CLS6    J1.29
#define LOG2_I15     LOG2_U20_IO1.B5    //PSL2_OP2        JI_PSL2_OPN8    J1.28
#define LOG2_I16     LOG2_U20_IO2.B0    //PSC_OP3         JI_PSC_CLS6     J2.20
#define LOG2_I17     LOG2_U20_IO2.B2    //PSC_OP1         JI_PSC_OPN6     J2.18
#define LOG2_I18     LOG2_U20_IO2.B3    //IN_PN0                          J1.14&15
#define LOG2_I19     LOG2_U20_IO2.B1    //PSL2_OP4        JI_PSL2_CLS8    J1.30
#define LOG2_I20     LOG2_U21_IO0.B0    //PSC_OP2         JI_PSC_OPN8     J2.19
#define LOG2_I21     LOG2_U21_IO0.B2    //PSC_OP4         JI_PSC_CLS8     J2.21
#define LOG2_I22     LOG2_U21_IO0.B3    //PSL3_OP1        JI_PSL3_OPN6    J2.4
#define LOG2_I23     LOG2_U21_IO0.B1    //IN_BK1                          J1.31
#define LOG2_I24     LOG2_U21_IO0.B4    //PSL3_OP2        JI_PSL3_OPN8    J2.5
#define LOG2_I25     LOG2_U21_IO0.B6    //IN_PN1                          J2.25&26        输入，接地用于测试自动运行的功能
#define LOG2_I26     LOG2_U21_IO0.B7    //PSL3_OP4        JI_PSL3_CLS8    J2.7
#define LOG2_I27     LOG2_U21_IO0.B5    //PSL3_OP3        JI_PSL3_CLS6    J2.6
#define LOG2_I28     LOG2_U22_IO3.B6    //DCU_Q03         CO_DCU_CLS8_1       J3.5    板子输出命令监视点
#define LOG2_I29     LOG2_U22_IO3.B7    //DCU_Q00         CO_DCU_OPN6_1       J3.2    板子输出命令监视点
#define LOG2_I30     LOG2_U22_IO2.B5    //DCU_Q10         CO_DCU_CLS8_2       J3.17   板子输出命令监视点
#define LOG2_I31     LOG2_U22_IO2.B4    //DCU_Q08         CO_DCU_OPN8_2       J3.15   板子输出命令监视点
#define LOG2_I32     LOG2_U22_IO4.B0    //DCU_Q04         CO_DCU_RECLS_1      J3.6    板子输出命令监视点
#define LOG2_I33     LOG2_U22_IO4.B1    //DCU_Q02         CO_DCU_CLS6_1       J3.4    板子输出命令监视点
#define LOG2_I34     LOG2_U22_IO3.B0    //K8_N            ALL_OPN8_LOCK           自锁输出OPN8
#define LOG2_I35     LOG2_U22_IO2.B6    //DCU_Q09         CO_DCU_CLS6_2       J3.16   板子输出命令监视点
#define LOG2_I36     LOG2_U22_IO4.B2    //K7_PN           ALL_OPN6_LOCK           自锁输出OPN6
#define LOG2_I37     LOG2_U22_IO4.B3    //DCU_Q06         CO_DCU_DCU_OPSD1    J3.8    板子输出命令监视点
#define LOG2_I38     LOG2_U22_IO3.B2    //DCU_Q13         CO_DCU_DCU_OPSD2    J3.20   板子输出命令监视点
#define LOG2_I39     LOG2_U22_IO3.B1    //DCU_Q11         CO_DCU_RECLS_2      J3.18   板子输出命令监视点
#define LOG2_I40     LOG2_U22_IO4.B4    //DCU_Q05         CO_DCU_IBPPSL_OPN1  J3.7    板子输出命令监视点
#define LOG2_I41     LOG2_U22_IO4.B5    //DCU_Q01         CO_DCU_OPN8_1       J3.3    板子输出命令监视点
#define LOG2_I42     LOG2_U21_IO2.B5    //DCU_Q12         CO_DCU_IBPPSL_OPN2  J3.19   板子输出命令监视点
#define LOG2_I43     LOG2_U22_IO3.B4    //DCU_Q07         CO_DCU_OPN6_2       J3.14   板子输出命令监视点
#define LOG2_I44     LOG2_U22_IO4.B6    //K_S0            K0_NO           IBP_EN | IBPSD_EN | PSLX_EN | PSC_EN
#define LOG2_I45     LOG2_U22_IO4.B7    //K_S13           K7A_NO
#define LOG2_I46     LOG2_U21_IO3.B3    //K_S15           K8A_NO
#define LOG2_I47     LOG2_U21_IO3.B0    //K_S14           K7B_NO
#define LOG2_I48     LOG2_U21_IO3.B6    //K_S16           K8B_NO
#define LOG2_I49     LOG2_U21_IO4.B0    //K_S18           K22_NO
#define LOG2_I50     LOG2_U21_IO4.B1    //K_S9            K5A_NO
#define LOG2_I51     LOG2_U21_IO3.B7    //K_S12           K6B_NO
#define LOG2_I52     LOG2_U21_IO4.B2    //K_S10           K5B_NO
#define LOG2_I53     LOG2_U21_IO4.B4    //K_S19           K10_NC          PSL2_EN | PSL2_EN | PSL3_EN
#define LOG2_I54     LOG2_U21_IO4.B5    //K_S17           K21_NC
#define LOG2_I55     LOG2_U21_IO4.B3    //K_S11           K6A_NO
#define LOG2_I56     LOG2_U21_IO4.B6    //K_S20           K30_NC
#define LOG2_I57     LOG2_U20_IO2.B4    //K_S1            K1B_NO
#define LOG2_I58     LOG2_U20_IO2.B5    //K_S2            K1A_NO
#define LOG2_I59     LOG2_U21_IO4.B7    //K_S4            K2A_NO
#define LOG2_I60     LOG2_U20_IO2.B6    //K_S3            K2B_NO
#define LOG2_I61     LOG2_U20_IO3.B0    //K_S5            K3B_NO
#define LOG2_I62     LOG2_U20_IO3.B1    //K_S7            K4A_NO
#define LOG2_I63     LOG2_U20_IO2.B7    //K_S6            K3A_NO
#define LOG2_I64     LOG2_U20_IO3.B2    //K_S8            K4B_NO
#define LOG2_I65     LOG2_U20_IO3.B4    //K3_PN           JI_SIG_CLS      J2.13&14        信号系统命令 关车门
#define LOG2_I66     LOG2_U20_IO3.B5    //K1_PN           JI_SIG_OPN6     J2.9&10 信号系统命令 开6车
#define LOG2_I67     LOG2_U20_IO3.B3    //K2_PN           JI_SIG_OPN8     J2.11&12信号系统命令 开8车
#define LOG2_I68     LOG2_U20_IO3.B6    //K0_PN           IBPPSLPSC_EN            IBP_EN | PSLX_EN | PSC_TEST
#define LOG2_I69     LOG2_U20_IO4.B0    //K4_N            ALL_OPN6                IBP / PSL / PSC / SIG OPN6
#define LOG2_I70     LOG2_U20_IO4.B1    //K5_N            ALL_OPN8                IBP / PSL / PSC / SIG OPN8
#define LOG2_I71     LOG2_U20_IO3.B7    //K6_N            ALL_CLS                 IBP / PSL / PSC / SIG CLS
#define LOG2_I72     LOG2_U20_IO4.B2    //K30_PN          JI_PSC_TEST     J2.16&17PSC测试使能
#define LOG2_I73     LOG2_U20_IO4.B4    //K22_PN          JI_IBP_A_EN     J1.8&9  IBP开边门使能
#define LOG2_I74     LOG2_U20_IO4.B5    //K21_PN          JI_IBP_EN       J1.2&3  IBP 使能
#define LOG2_I75     LOG2_U20_IO4.B3    //PSL2_OP0_N      JI_PSL2_EN      J1.18&19PSL1 使能
#define LOG2_I76     LOG2_U20_IO4.B6    //PSL2_OP0_N      JI_PSL2_EN      J1.25&26PSL2 使能
#define LOG2_I77     LOG2_U20_IO4.B7    //PSL3_OP0_N      JI_PSL3_EN      J2.2&3  PSL3 使能
#define LOG2_Q00     LOG2_U22_IO0.B7    //PSL2_Q_CL       OUT_PSLx_CLS            MCU 控制IO输出点，板内输出
#define LOG2_Q01     LOG2_U21_IO2.B0    //OUT_PN0                         J2.27&28端子输出点，备用
#define LOG2_Q02     LOG2_U22_IO0.B5    //PSL2_Q_OP_6     OUT_PSLx_OPN6           MCU 控制IO输出点，板内输出
#define LOG2_Q03     LOG2_U22_IO0.B6    //IBP_Q_OP_8      OUT_IBP_OPN8            MCU 控制IO输出点，板内输出
#define LOG2_Q04     LOG2_U22_IO0.B3    //PSL2_Q_OP_8     OUT_PSLx_OPN8           MCU 控制IO输出点，板内输出
#define LOG2_Q05     LOG2_U22_IO0.B4    //IBP_Q_CL        OUT_IBP_CLS             MCU 控制IO输出点，板内输出
#define LOG2_Q06     LOG2_U22_IO0.B1    //PSC_Q_OP_8      OUT_PSC_OPN8            MCU 控制IO输出点，板内输出
#define LOG2_Q07     LOG2_U22_IO0.B2    //IBP_Q_OP_6      OUT_IBP_OPN6            MCU 控制IO输出点，板内输出
#define LOG2_Q08     LOG2_U21_IO1.B7    //PSC_Q_CL        OUT_PSC_CLS             MCU 控制IO输出点，板内输出
#define LOG2_Q09     LOG2_U22_IO0.B0    //PSC_Q_OP_6      OUT_PSC_OPN6            MCU 控制IO输出点，板内输出
#define LOG2_Q10     LOG2_U21_IO1.B0    //COM_Q_0         CO_Q0           J3.9    备用输出给DCU
#define LOG2_Q11     LOG2_U21_IO1.B1    //COM_Q_1         CO_Q1           J3.21   备用输出给DCU
#define LOG2_Q12     LOG2_U21_IO1.B2    //OUT_PN1                         J3.11&12端子输出点，备用
//#define LOG2_Q13     LOG2_U21_IO1.B3    //OUT_PN2                       J3.23&24端子输出点，备用 ==> PEDC_SZ16_V1.pdf 有变化
#define LOG2_Q14     LOG2_U21_IO1.B3    //IBP_Q_OPSD1     OUT_IBP_OPNSD0  J3.19   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD0
#define LOG2_Q15     LOG2_U21_IO1.B4    //IBP_Q_OPSD2     OUT_IBP_OPNSD1  J3.20   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD1
#define LOG2_Q16     LOG2_U21_IO1.B5    //IBP_PSL_Q1      OUT_IBPPSL_Q1   J3.8    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q1
#define LOG2_Q17     LOG2_U21_IO1.B6    //IBP_PSL_Q0      OUT_IBPPSL_Q0   J3.7    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q0

#define L2_JI_IBP_EN            LOG2_I74    //J1.2&3  IBP 使能
#define L2_JI_IBP_OPN6          LOG2_I00    //J1.4    监视输入命令 IBPOPN6
#define L2_JI_IBP_OPN8          LOG2_I01    //J1.5    监视输入命令 IBPOPN8
#define L2_JI_IBP_CLS6          LOG2_I04    //J1.6    监视输入命令 IBPCLS6
#define L2_JI_IBP_CLS8          LOG2_I05    //J1.7    监视输入命令 IBPCLS8
#define L2_JI_IBP_OPNSD1        LOG2_I08    //J1.10   监视输入命令
#define L2_JI_IBP_OPNSD2        LOG2_I09    //J1.11   监视输入命令
#define L2_JI_IBP_CLSSD1        LOG2_I12    //J1.12   监视输入命令
#define L2_JI_IBP_CLSSD2        LOG2_I13    //J1.13   监视输入命令
#define L2_JI_IBP_A_EN          LOG2_I73    //J1.8&9  IBP开边门使能
                                
#define L2_JI_PSL1_EN           LOG2_I75    //J1.18&19PSL1 使能
#define L2_JI_PSL1_OPN8         LOG2_I02    //J1.21
#define L2_JI_PSL1_OPN6         LOG2_I03    //J1.20
#define L2_JI_PSL1_CLS8         LOG2_I06    //J1.23
#define L2_JI_PSL1_CLS6         LOG2_I07    //J1.22

#define L2_JI_PSL2_EN           LOG2_I76    //J1.25&26PSL2 使能
#define L2_JI_PSL2_OPN8         LOG2_I15    //J1.28
#define L2_JI_PSL2_OPN6         LOG2_I10    //J1.27
#define L2_JI_PSL2_CLS6         LOG2_I14    //J1.29
#define L2_JI_PSL2_CLS8         LOG2_I19    //J1.30

#define L2_JI_PSL3_EN           LOG2_I77    //J2.2&3  PSL3 使能
#define L2_JI_PSL3_OPN6         LOG2_I22    //J2.4
#define L2_JI_PSL3_OPN8         LOG2_I24    //J2.5
#define L2_JI_PSL3_CLS6         LOG2_I27    //J2.6
#define L2_JI_PSL3_CLS8         LOG2_I26    //J2.7

#define L2_JI_PSC_TEST          LOG2_I72    //J2.16&17PSC测试使能
#define L2_JI_PSC_OPN6          LOG2_I17    //J2.18
#define L2_JI_PSC_OPN8          LOG2_I20    //J2.19
#define L2_JI_PSC_CLS6          LOG2_I16    //J2.20
#define L2_JI_PSC_CLS8          LOG2_I21    //J2.21

#define L2_IN_BK0               LOG2_I11    //J1.24
#define L2_IN_PN0               LOG2_I18    //J1.14&15
#define L2_IN_BK1               LOG2_I23    //J1.31
#define L2_IN_PN1               LOG2_I25    //J2.25&26        输入，接地用于测试自动运行的功能

#define L2_ALL_OPN6_LOCK        LOG2_I36    //   自锁输出OPN6
#define L2_ALL_OPN8_LOCK        LOG2_I34    //   自锁输出OPN8

#define L2_CO_DCU_OPN6_1        LOG2_I29    //J3.2    板子输出命令监视点
#define L2_CO_DCU_OPN8_1        LOG2_I41    //J3.3    板子输出命令监视点
#define L2_CO_DCU_CLS6_1        LOG2_I33    //J3.4    板子输出命令监视点
#define L2_CO_DCU_CLS8_1        LOG2_I28    //J3.5    板子输出命令监视点
#define L2_CO_DCU_RECLS_1       LOG2_I32    //J3.6    板子输出命令监视点
#define L2_CO_DCU_IBPPSL_OPN1   LOG2_I40    //J3.7    板子输出命令监视点
#define L2_CO_DCU_DCU_OPSD1     LOG2_I37    //J3.8    板子输出命令监视点

#define L2_CO_DCU_OPN6_2        LOG2_I43    //J3.14   板子输出命令监视点
#define L2_CO_DCU_OPN8_2        LOG2_I31    //J3.15   板子输出命令监视点
#define L2_CO_DCU_CLS6_2        LOG2_I35    //J3.16   板子输出命令监视点
#define L2_CO_DCU_CLS8_2        LOG2_I30    //J3.17   板子输出命令监视点
#define L2_CO_DCU_RECLS_2       LOG2_I39    //J3.18   板子输出命令监视点
#define L2_CO_DCU_IBPPSL_OPN2   LOG2_I42    //J3.19   板子输出命令监视点
#define L2_CO_DCU_DCU_OPSD2     LOG2_I38    //J3.20   板子输出命令监视点

#define L2_JI_SIG_CLS           LOG2_I65    //J2.13&14        信号系统命令 关车门
#define L2_JI_SIG_OPN6          LOG2_I66    //J2.9&10 信号系统命令 开6车
#define L2_JI_SIG_OPN8          LOG2_I67    //J2.11&12信号系统命令 开8车

#define L2_IBPPSLPSC_EN         LOG2_I68    //        IBP_EN | PSLX_EN | PSC_TEST
#define L2_ALL_OPN6             LOG2_I69    //        IBP / PSL / PSC / SIG OPN6
#define L2_ALL_OPN8             LOG2_I70    //        IBP / PSL / PSC / SIG OPN8
#define L2_ALL_CLS              LOG2_I71    //        IBP / PSL / PSC / SIG CLS

#define L2_K0_NO                LOG2_I44    //        IBP_EN | IBPSD_EN | PSLX_EN | PSC_EN
#define L2_K7A_NO               LOG2_I45    //
#define L2_K8A_NO               LOG2_I46    //
#define L2_K7B_NO               LOG2_I47    //
#define L2_K8B_NO               LOG2_I48    //
#define L2_K22_NO               LOG2_I49    //
#define L2_K5A_NO               LOG2_I50    //
#define L2_K6B_NO               LOG2_I51    //
#define L2_K5B_NO               LOG2_I52    //
#define L2_K10_NC               LOG2_I53    //        PSL2_EN | PSL2_EN | PSL3_EN
#define L2_K21_NC               LOG2_I54    //
#define L2_K6A_NO               LOG2_I55    //
#define L2_K30_NC               LOG2_I56    //
#define L2_K1B_NO               LOG2_I57    //
#define L2_K1A_NO               LOG2_I58    //
#define L2_K2A_NO               LOG2_I59    //
#define L2_K2B_NO               LOG2_I60    //
#define L2_K3B_NO               LOG2_I61    //
#define L2_K4A_NO               LOG2_I62    //
#define L2_K3A_NO               LOG2_I63    //
#define L2_K4B_NO               LOG2_I64    //
#define L2_OUT_PSLx_CLS         LOG2_Q00    //        MCU 控制IO输出点，板内输出
#define L2_OUT_PN0              LOG2_Q01    //J2.27&28端子输出点，备用
#define L2_OUT_PSLx_OPN6        LOG2_Q02    //        MCU 控制IO输出点，板内输出
#define L2_OUT_IBP_OPN8         LOG2_Q03    //        MCU 控制IO输出点，板内输出
#define L2_OUT_PSLx_OPN8        LOG2_Q04    //        MCU 控制IO输出点，板内输出
#define L2_OUT_IBP_CLS          LOG2_Q05    //        MCU 控制IO输出点，板内输出
#define L2_OUT_PSC_OPN8         LOG2_Q06    //        MCU 控制IO输出点，板内输出
#define L2_OUT_IBP_OPN6         LOG2_Q07    //        MCU 控制IO输出点，板内输出
#define L2_OUT_PSC_CLS          LOG2_Q08    //        MCU 控制IO输出点，板内输出
#define L2_OUT_PSC_OPN6         LOG2_Q09    //        MCU 控制IO输出点，板内输出
#define L2_CO_Q0                LOG2_Q10    //J3.9    备用输出给DCU
#define L2_CO_Q1                LOG2_Q11    //J3.21   备用输出给DCU
#define L2_OUT_PN1              LOG2_Q12    //J3.11&12端子输出点，备用
//#define L2_OUT_PN2            LOG2_Q13    //J3.23&24端子输出点，备用
#define L2_OUT_IBP_OPNSD0       LOG2_Q14    //J3.19   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD0
#define L2_OUT_IBP_OPNSD1       LOG2_Q15    //J3.20   MCU 控制IO输出点，板内输出到端子 IBP_Q_OPSD1
#define L2_OUT_IBPPSL_Q1        LOG2_Q16    //J3.8    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q1
#define L2_OUT_IBPPSL_Q0        LOG2_Q17    //J3.7    MCU 控制IO输出点，板内输出到端子 IBP_PSL_Q0

//--------------------------------------
#endif  //_PCB_IO_DEF_H






