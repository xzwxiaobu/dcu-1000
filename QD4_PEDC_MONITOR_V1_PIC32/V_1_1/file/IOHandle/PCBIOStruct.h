#ifndef _PCB_IOSTRUCT_H
#define _PCB_IOSTRUCT_H


#include "_cpu.h"

/* =============================================================================
 * 瀹?涔???杈??ヨ??虹?圭??浣?缃?????绉?
 * 寰?瀹???
 */
/* PEDC_MONITOR_V1 */
typedef struct
{
    //===== U20 =====
    //struct
    //{
    int8u Q00   : 1;        //U20 IO0_0
    int8u Q01   : 1;        //U20 IO0_1
    int8u Q02   : 1;        //U20 IO0_2
    int8u Q03   : 1;        //U20 IO0_3
    int8u Q04   : 1;        //U20 IO0_4
    int8u Q05   : 1;        //U20 IO0_5
    int8u Q06   : 1;        //U20 IO0_6
    int8u Q07   : 1;        //U20 IO0_7
                             
    int8u Q08   : 1;        //U20 IO1_0
    int8u Q09   : 1;        //U20 IO1_1
    int8u Q10   : 1;        //U20 IO1_2
    int8u Q11   : 1;        //U20 IO1_3
    int8u Q12   : 1;        //U20 IO1_4
    int8u Q13   : 1;        //U20 IO1_5
    int8u Q14   : 1;        //U20 IO1_6
    int8u Q15   : 1;        //U20 IO1_7
                              
    int8u Q16   : 1;        //U20 IO2_0
    int8u Q17   : 1;        //U20 IO2_1
    int8u Q18   : 1;        //U20 IO2_2
    int8u Q19   : 1;        //U20 IO2_3
    int8u I04   : 1;        //U20 IO2_4
    int8u I07   : 1;        //U20 IO2_5
    int8u I05   : 1;        //U20 IO2_6
    int8u I06   : 1;        //U20 IO2_7
                              
    int8u I00   : 1;        //U20 IO3_0
    int8u I03   : 1;        //U20 IO3_1
    int8u I01   : 1;        //U20 IO3_2
    int8u I02   : 1;        //U20 IO3_3
    int8u Q28   : 1;        //U20 IO3_4
    int8u Q29   : 1;        //U20 IO3_5
    int8u Q30   : 1;        //U20 IO3_6
    int8u Q31   : 1;        //U20 IO3_7
                              
    int8u Q32   : 1;        //U20 IO4_0
    int8u Q33   : 1;        //U20 IO4_1
    int8u Q34   : 1;        //U20 IO4_2
    int8u Q35   : 1;        //U20 IO4_3
    int8u Q36   : 1;        //U20 IO4_4
    int8u Q37   : 1;        //U20 IO4_5
    int8u Q38   : 1;        //U20 IO4_6
    int8u Q39   : 1;        //U20 IO4_7
    //}PCA9505_U20;
    
    //===== U21 =====
    //struct
    //{
    int8u I10   : 1;        //U21 IO0_0
    int8u I09   : 1;        //U21 IO0_1
    int8u I11   : 1;        //U21 IO0_2
    int8u I08   : 1;        //U21 IO0_3
    int8u I14   : 1;        //U21 IO0_4
    int8u I13   : 1;        //U21 IO0_5
    int8u I15   : 1;        //U21 IO0_6
    int8u I12   : 1;        //U21 IO0_7
                              
    int8u I18   : 1;        //U21 IO1_0
    int8u I17   : 1;        //U21 IO1_1
    int8u I19   : 1;        //U21 IO1_2
    int8u I16   : 1;        //U21 IO1_3
    int8u I22   : 1;        //U21 IO1_4
    int8u I21   : 1;        //U21 IO1_5
    int8u I23   : 1;        //U21 IO1_6
    int8u I20   : 1;        //U21 IO1_7
                            
    int8u I26   : 1;        //U21 IO2_0
    int8u I25   : 1;        //U21 IO2_1
    int8u I27   : 1;        //U21 IO2_2
    int8u I24   : 1;        //U21 IO2_3
    int8u I30   : 1;        //U21 IO2_4
    int8u I29   : 1;        //U21 IO2_5
    int8u I31   : 1;        //U21 IO2_6
    int8u I28   : 1;        //U21 IO2_7
                            
    int8u I34   : 1;        //U21 IO3_0
    int8u I33   : 1;        //U21 IO3_1
    int8u I35   : 1;        //U21 IO3_2
    int8u I32   : 1;        //U21 IO3_3
    int8u I38   : 1;        //U21 IO3_4
    int8u I37   : 1;        //U21 IO3_5
    int8u I39   : 1;        //U21 IO3_6
    int8u I36   : 1;        //U21 IO3_7
                              
    int8u Q27   : 1;        //U21 IO4_0
    int8u Q26   : 1;        //U21 IO4_1
    int8u Q25   : 1;        //U21 IO4_2
    int8u Q24   : 1;        //U21 IO4_3
    int8u Q23   : 1;        //U21 IO4_4
    int8u Q22   : 1;        //U21 IO4_5
    int8u Q21   : 1;        //U21 IO4_6
    int8u Q20   : 1;        //U21 IO4_7
    //}PCA9505_U21;
}tMonV1_IO;

typedef struct
{    
    /* 0, RB L */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u I04   : 1;     //RB2  
    int8u I12   : 1;     //RB3  
    int8u I03   : 1;     //RB4  
    int8u I13   : 1;     //RB5  
    int8u I11   : 1;     //RB6  
    int8u I05   : 1;     //RB7  
    
    /* 1, RB H */
    int8u I10   : 1;     //RB8  
    int8u I06   : 1;     //RB9  
    int8u I09   : 1;     //RB10 
    int8u I07   : 1;     //RB11 
    int8u I08   : 1;     //RB12 
    int8u I24   : 1;     //RB13 
    int8u       : 1;     //14
    int8u I27   : 1;     //RB15 
    
    /* 2, RC H */
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u I16   : 1;     //RC13 
    int8u I17   : 1;     //RC14 
    int8u       : 1;     //15
    
    /* 3, RD L */
    int8u I19   : 1;     //RD0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u I23   : 1;     //RD5  
    int8u       : 1;     //6
    int8u I22   : 1;     //RD7  
                
    /* 4, RD H  */
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u I18   : 1;     //RD11 
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
                   
    /* 5, RE L  */
    int8u I20   : 1;     //RE0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u I00   : 1;     //RE6  
    int8u I15   : 1;     //RE7  
                   
    /* 6, RF L  */
    int8u I21   : 1;     //RF0 
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u I26   : 1;     //RF3 
    int8u       : 1;     //4
    int8u I25   : 1;     //RF5  
    int8u       : 1;     //6
    int8u       : 1;     //7
                  
    /* 7, RG L */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u I01   : 1;     //RG6  
    int8u I14   : 1;     //RG7 
                
    /* 8, RG H  */
    int8u       : 1;     //8 
    int8u I02   : 1;     //RG9  
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
}tIBP_IN;


typedef struct
{    /* 0, RB L */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u I01   : 1;     //RB2  (I1 ,I2)  K15-PN #K15_KA,  PSL1/2/3-OP2
    int8u I15   : 1;     //RB3  IN-P0N0          #
    int8u I00   : 1;     //RB4  (I0,I11)  K-S7 #K16_KD_NO
    int8u       : 1;     //RB5 
    int8u I14   : 1;     //RB6  (I8,I14)  K1-PN  #K1_KA,  IBP-OP0
    int8u I02   : 1;     //RB7  (I1 ,I2)  K-S6 #K15_KD_NO 
                
    /* 1, RB H  */    
    int8u I13   : 1;     //RB8  (I6,I13)  K12-N  #K12_KA, PSL3-OP0
    int8u I03   : 1;     //RB9  (I3, I9)  K11-N  #K11_KA,  PSL2-OP0
    int8u I12   : 1;     //RB10 (I7,I12) K10-PN #K10_KA, PSL1-OP0
    int8u I04   : 1;     //RB11 (I4, I5) K14-PN #K14_KA,  PSL1/2/3-OP1
    int8u I11   : 1;     //RB12 (I0,I11) K16-P PSL-OPN6 #K16_KA, PSL-OPN6
    int8u I05   : 1;     //RB13 (I4, I5) K-S5 #K14_KD_NO
    int8u I10   : 1;     //RB14           K-S4 #K13_KD_NO
    int8u I06   : 1;     //RB15 (I6,I13) K-S3 #K12_KD_NO
                
    /* 2, RC H  */
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u       : 1;     //13 
    int8u       : 1;     //14 
    int8u       : 1;     //15
                         
    /* 3, RD L  */       
    int8u       : 1;     //0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5  
    int8u       : 1;     //6
    int8u       : 1;     //7  
                         
    /* 4, RD H  */       
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u       : 1;     //11 
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
                   
    /* 5, RE L  */
    int8u       : 1;     //0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u       : 1;     //6  
    int8u       : 1;     //7  
                   
    /* 6, RF L  */
    int8u       : 1;     //0 
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u I08   : 1;     //RF3  (I8,I14)  K-S0 #K1_KD_NO
    int8u I09   : 1;     //RF4  (I3, I9)  K-S2 #K11_KD_NO
    int8u I07   : 1;     //RF5  (I7,I12)  K-S1 #K10_KD_NO
    int8u       : 1;     //6
    int8u       : 1;     //7
                   
    /* 7, RG L  */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u       : 1;     //6  
    int8u       : 1;     //7 
                
    /* 8, RG H  */
    int8u       : 1;     //8 
    int8u       : 1;     //9  
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
}tPSL_IN;


/* SIG 板的输入点名称，用继电器或IO点命名
 *  */
typedef struct
{    
    int8u K30_PN        : 1;     //I12 : U20_0_0  K30-PN #K30_KA, PSC-OP0
    int8u K30_NC        : 1;     //I18 : U20_0_1  K-S2   #K30_KC_NC
    int8u K1A1B_NO      : 1;     //I17 : U20_0_2  K-S8   #K1A_KD_NO && K1B_KD_NO
    int8u K31_NO        : 1;     //I19 : U20_0_3  K-S3   #K31_KD_NO
    int8u K31_PN        : 1;     //I16 : U20_0_4  K31-PN #K31_KA, PSC-OP1
    int8u K4A4B_NO      : 1;     //I15 : U20_0_5  K-S11  #K4A_KD_NO && K4B_KD_NO
    int8u K41K42_PN     : 1;     //I13 : U20_0_6  K41-PN #K41_KA || K42_KA, NAUTO-N
    int8u K10_PN        : 1;     //I14 : U20_0_7  K10-PN #K10_KA, PSL1/2/3-OP0
                                   
    int8u K2_PN         : 1;     //I08 : U20_1_0  K2-PN, AUTO-OP1-N
    int8u K3_PN         : 1;     //I11 : U20_1_1  K3-PN #K3A_KA || K3B_KA
    int8u K2A2B_NO      : 1;     //I09 : U20_1_2  K-S9  #K2A_KD_NO && K2B_KD_NO
    int8u K10_NO        : 1;     //I10 : U20_1_3  K-S0  #K10_KD_NO
    int8u K0_PN         : 1;     //I04 : U20_1_4  K0-PN #K0_KA
    int8u K20_PN        : 1;     //I07 : U20_1_5  K20-PN#K20_KA, IBP-OP0
    int8u K41K42_NC     : 1;     //I05 : U20_1_6  K-S5  #K41_KC_NC && K42_KC_NC
    int8u K20_NO        : 1;     //I06 : U20_1_7  K-S1  #K20_KF_NO
                                   
    int8u K0_NO         : 1;     //I00 : U20_2_0  K-S7  #K0_KG_NO
    int8u K43_NO        : 1;     //I03 : U20_2_1  K-S6  #K43_KG_NO
    int8u K4A4B_PN      : 1;     //I01 : U20_2_2  K4-N  #K4A_KA || K4B_KA
    int8u K3A3B_NC      : 1;     //I02 : U20_2_3  K-S10 #K3A_KD_NC && K3B_KD_NC
    int8u               : 1;     //    : U20_2_4  
    int8u               : 1;     //    : U20_2_5  
    int8u               : 1;     //    : U20_2_6  
    int8u               : 1;     //    : U20_2_7  
                                       
    int8u               : 1;     //    : U20_3_0  
    int8u               : 1;     //    : U20_3_1  
    int8u               : 1;     //    : U20_3_2  
    int8u               : 1;     //    : U20_3_3  
    int8u IN_PN2        : 1;     //I28 : U20_3_4  IN-P2N2 #
    int8u IN_PN4        : 1;     //I31 : U20_3_5  IN-P4N4 #
    int8u IN_PN3        : 1;     //I29 : U20_3_6  IN-P3N3 #
    int8u IN_PN5        : 1;     //I30 : U20_3_7  IN-P5N5 #
                                    
    int8u IN_PN0        : 1;     //I24 : U20_4_0  IN-P0N0 #
    int8u IN_PN1        : 1;     //I27 : U20_4_1  IN-P1N1 #
    int8u IN_PN6        : 1;     //I25 : U20_4_2  IN-P6N6 #
    int8u K32_PN        : 1;     //I26 : U20_4_3  (I21,I26) K32-PN #K32_KA, PSC-OP2
    int8u K1_PN         : 1;     //I20 : U20_4_4  (I17,I20) K1-PN , AUTO-OP0-N
    int8u K5A5B_NO      : 1;     //I23 : U20_4_5  (I22,I23) K-S12  #K5A_KD_NO && K5B_KD_NO
    int8u K32_NO        : 1;     //I21 : U20_4_6  (I21,I26) K-S4   #K32_KD_NO
    int8u K5_PN         : 1;     //I22 : U20_4_7  (I22,I23) K5-P SIG-OPN6, SIG-OPN6
}tSIG_InName;

/* SIG 板的输入点功能名
 *  */
typedef struct
{    
    int8u PSC_TEST      : 1;     //I12 : K30-PN #K30_KA, PSC-OP0
    int8u               : 1;     //I18 : K-S2   #K30_KC_NC
    int8u               : 1;     //I17 : K-S8   #K1A_KD_NO && K1B_KD_NO
    int8u               : 1;     //I19 : K-S3   #K31_KD_NO
    int8u PSC_CLS       : 1;     //I16 : K31-PN #K31_KA, PSC-OP1
    int8u               : 1;     //I15 : K-S11  #K4A_KD_NO && K4B_KD_NO
    int8u               : 1;     //I13 : K41-PN #K41_KA || K42_KA, NAUTO-N
    int8u PSL123_EN     : 1;     //I14 : K10-PN #K10_KA, PSL1/2/3-OP0
                                 
    int8u SIG_CLS       : 1;     //I08 : K2-PN, AUTO-OP1-N J3-5&6 信号关门
    int8u               : 1;     //I11 : K3-PN #K3A_KA || K3B_KA
    int8u               : 1;     //I09 : K-S9  #K2A_KD_NO && K2B_KD_NO
    int8u               : 1;     //I10 : K-S0  #K10_KD_NO
    int8u               : 1;     //I04 : K0-PN #K0_KA
    int8u IBP_EN        : 1;     //I07 : K20-PN#K20_KA, IBP-OP0
    int8u               : 1;     //I05 : K-S5  #K41_KC_NC && K42_KC_NC
    int8u               : 1;     //I06 : K-S1  #K20_KF_NO
                                   
    int8u               : 1;     //I00 : K-S7  #K0_KG_NO
    int8u               : 1;     //I03 : K-S6  #K43_KG_NO
    int8u               : 1;     //I01 : K4-N  #K4A_KA || K4B_KA
    int8u               : 1;     //I02 : K-S10 #K3A_KD_NC && K3B_KD_NC
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
                                       
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
    int8u               : 1;     //    : 
    int8u               : 1;     //I28 : IN-P2N2 #
    int8u               : 1;     //I31 : IN-P4N4 #
    int8u               : 1;     //I29 : IN-P3N3 #
    int8u               : 1;     //I30 : IN-P5N5 #
                                    
    int8u               : 1;     //I24 : IN-P0N0 #
    int8u               : 1;     //I27 : IN-P1N1 #
    int8u               : 1;     //I25 : IN-P6N6 #
    int8u PSC_OPN6      : 1;     //I26 : K32-PN #K32_KA, PSC-OP2
    int8u SIG_OPN       : 1;     //I20 : K1-PN , AUTO-OP0-N J3-3&4 信号开门
    int8u               : 1;     //I23 : K-S12 #K5A_KD_NO && K5B_KD_NO
    int8u               : 1;     //I21 : K-S4 #K32_KD_NO
    int8u SIG_OPN6      : 1;     //I22 : K5-P SIG-OPN6, SIG-OPN6
}tSIG_FunName;

typedef struct
{    /* 0, RB L */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u       : 1;     //2  
    int8u       : 1;     //3  
    int8u       : 1;     //4  
    int8u       : 1;     //5  
    int8u       : 1;     //6  
    int8u       : 1;     //7  
                
    /* 1, RB H  */
    int8u       : 1;     //8  
    int8u       : 1;     //9  
    int8u Q00   : 1;     //RB10 
    int8u Q01   : 1;     //RB11 
    int8u Q02   : 1;     //RB12 
    int8u Q03   : 1;     //RB13 
    int8u Q04   : 1;     //RB14
    int8u Q05   : 1;     //RB15 
                
    /* 2, RC H  */
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u       : 1;     //13 
    int8u       : 1;     //14 
    int8u       : 1;     //15
                
    /* 3, RD L  */
    int8u       : 1;     //RD0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5  
    int8u       : 1;     //6
    int8u       : 1;     //7  
                
    /* 4, RD H  */
    int8u       : 1;     //8
    int8u       : 1;     //9
    int8u       : 1;     //10
    int8u       : 1;     //11 
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
                   
    /* 5, RE L  */
    int8u       : 1;     //0  
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u       : 1;     //6  
    int8u       : 1;     //7  
                   
    /* 6, RF L  */
    int8u       : 1;     //0 
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3 
    int8u Q06   : 1;     //RF4
    int8u Q07   : 1;     //RF5  
    int8u       : 1;     //6
    int8u       : 1;     //7
                   
    /* 7, RG L  */
    int8u       : 1;     //0
    int8u       : 1;     //1
    int8u       : 1;     //2
    int8u       : 1;     //3
    int8u       : 1;     //4
    int8u       : 1;     //5
    int8u       : 1;     //6  
    int8u       : 1;     //7 
                
    /* 8, RG H  */
    int8u       : 1;     //8 
    int8u       : 1;     //9  
    int8u       : 1;     //10
    int8u       : 1;     //11
    int8u       : 1;     //12
    int8u       : 1;     //13
    int8u       : 1;     //14
    int8u       : 1;     //15
}tSIG_OUT;      



#endif //_PCB_IOSTRUCT_H
