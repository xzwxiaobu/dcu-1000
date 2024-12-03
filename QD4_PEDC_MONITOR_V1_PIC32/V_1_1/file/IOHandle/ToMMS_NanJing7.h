
//继电器 状态(两个位表示)
//  	00		01		10		11
//     (断开正常)	吸合		故障		无
typedef struct
{
    //BYTE 0 
    int8u   bSIGRelay_K0_Coil           : 1;    //0.0   线圈电压  SIG-k0
    int8u   bSIGRelay_K0_Fault          : 1;    //0.1   故障位
    int8u   bSIGRelay_K1AK1B_Coil       : 1;    //0.2   线圈电压  SIG-k1
    int8u   bSIGRelay_K1AK1B_Fault      : 1;    //0.3   故障位
    int8u   bSIGRelay_K2AK2B_Coil       : 1;    //0.4   线圈电压  SIG-k2
    int8u   bSIGRelay_K2AK2B_Fault      : 1;    //0.5   故障位
    int8u   bSIGRelay_K3AK3B_Coil       : 1;    //0.6   线圈电压  SIG-k3
    int8u   bSIGRelay_K3AK3B_Fault      : 1;    //0.7   故障位
                                                    
    //BYTE 1                                        
    int8u   bSIGRelay_K4AK4B_Coil       : 1;    //1.0   线圈电压  SIG-k4
    int8u   bSIGRelay_K4AK4B_Fault      : 1;    //1.1   故障位
    int8u   bSIGRelay_K5AK5B_Coil       : 1;    //1.2   线圈电压  SIG-k5
    int8u   bSIGRelay_K5AK5B_Fault      : 1;    //1.3   故障位
    int8u   bSIGRelay_K10_Coil          : 1;    //1.4   线圈电压  SIG-k10
    int8u   bSIGRelay_K10_Fault         : 1;    //1.5   故障位
    int8u   bSIGRelay_K20_Coil          : 1;    //1.6   线圈电压  SIG-k20
    int8u   bSIGRelay_K20_Fault         : 1;    //1.7   故障位
                                                    
    //BYTE 2                                        
    int8u   bSIGRelay_K30_Coil          : 1;    //2.0   线圈电压  SIG-k30
    int8u   bSIGRelay_K30_Fault         : 1;    //2.1   故障位
    int8u   bSIGRelay_K31_Coil          : 1;    //2.2   线圈电压  SIG-k31
    int8u   bSIGRelay_K31_Fault         : 1;    //2.3   故障位
    int8u   bSIGRelay_K32_Coil          : 1;    //2.4   线圈电压  SIG-k32
    int8u   bSIGRelay_K32_Fault         : 1;    //2.5   故障位
    int8u   __bSIGRelay_K41K42_Coil     : 1;    //2.6   线圈电压  SIG-k41&k42
    int8u   __bSIGRelay_K41K42_Fault    : 1;    //2.7   故障位
                                                    
                                                          
    //BYTE 3                                        
    int8u   __bSIGRelay_K43_Coil        : 1;    //3.0   线圈电压  SIG-k43 
    int8u   __bSIGRelay_K43_Fault       : 1;    //3.1   故障位            
    int8u                               : 1;    //3.2   线圈电压  
    int8u                               : 1;    //3.3   故障位
    int8u                               : 1;    //3.4   线圈电压  
    int8u                               : 1;    //3.5   故障位
    int8u                               : 1;    //3.6   线圈电压  
    int8u                               : 1;    //3.7   故障位
                                                    
    //BYTE 4                                        
    int8u   bPSLRelay_K1_Coil           : 1;    //4.0   线圈电压  PSL-k1
    int8u   bPSLRelay_K1_Fault          : 1;    //4.1   故障位
    int8u   bPSLRelay_K10_Coil          : 1;    //4.2   线圈电压  PSL-k10
    int8u   bPSLRelay_K10_Fault         : 1;    //4.3   故障位
    int8u   bPSLRelay_K11_Coil          : 1;    //4.4   线圈电压  PSL-k11
    int8u   bPSLRelay_K11_Fault         : 1;    //4.5   故障位
    int8u   bPSLRelay_K12_Coil          : 1;    //4.6   线圈电压  PSL-k12
    int8u   bPSLRelay_K12_Fault         : 1;    //4.7   故障位
                                                    
    //BYTE 5                                        
    int8u   bPSLRelay_K13_Coil          : 1;    //5.0   线圈电压  PSL-k13
    int8u   bPSLRelay_K13_Fault         : 1;    //5.1   故障位
    int8u   bPSLRelay_K14_Coil          : 1;    //5.2   线圈电压  PSL-k14
    int8u   bPSLRelay_K14_Fault         : 1;    //5.3   故障位
    int8u   bPSLRelay_K15_Coil          : 1;    //5.4   线圈电压  PSL-k15
    int8u   bPSLRelay_K15_Fault         : 1;    //5.5   故障位
    int8u   bPSLRelay_K16_Coil          : 1;    //5.6   线圈电压  PSL-k16
    int8u   bPSLRelay_K16_Fault         : 1;    //5.7   故障位
                                                    
    //BYTE 6                                        
    int8u   bIBPRelay_K1_Coil           : 1;    //6.0   线圈电压  IBP-k1
    int8u   bIBPRelay_K1_Fault          : 1;    //6.1   故障位    IBP
    int8u   __bIBPRelay_K10_Coil        : 1;    //6.2   线圈电压  IBP-k10
    int8u   __bIBPRelay_K10_Fault       : 1;    //6.3   故障位    IBP
    int8u   bIBPRelay_K21_Coil          : 1;    //6.4   线圈电压  IBP-k21
    int8u   bIBPRelay_K21_Fault         : 1;    //6.5   故障位    IBP
    int8u   bIBPRelay_K22_Coil          : 1;    //6.6   线圈电压  IBP-k22
    int8u   bIBPRelay_K22_Fault         : 1;    //6.7   故障位    IBP
                                                    
    //BYTE 7                                        
    int8u   bIBPRelay_K23_Coil          : 1;    //7.0   线圈电压  IBP-k23
    int8u   bIBPRelay_K23_Fault         : 1;    //7.1   故障位    IBP
    int8u   bIBPRelay_K24_Coil          : 1;    //7.2   线圈电压  IBP-k24
    int8u   bIBPRelay_K24_Fault         : 1;    //7.3   故障位    IBP
    int8u   __bIBPRelay_K25_Coil        : 1;    //7.4   线圈电压  IBP-k25
    int8u   __bIBPRelay_K25_Fault       : 1;    //7.5   故障位    IBP
    int8u   __bIBPRelay_K26_Coil        : 1;    //7.6   线圈电压  IBP-k26
    int8u   __bIBPRelay_K26_Fault       : 1;    //7.7   故障位    IBP
                                                    
                                                    
    //BYTE 8                                        
    int8u   __bIBPRelay_K27_Coil        : 1;    //8.0   线圈电压  IBP-k27
    int8u   __bIBPRelay_K27_Fault       : 1;    //8.1   故障位    IBP
    int8u   bIBPRelay_K28_Coil          : 1;    //8.2   线圈电压  IBP-k28
    int8u   bIBPRelay_K28_Fault         : 1;    //8.3   故障位    IBP
    int8u   bIBPRelay_K29_Coil          : 1;    //8.4   线圈电压  IBP-k29
    int8u   bIBPRelay_K29_Fault         : 1;    //8.5   故障位    IBP
    int8u                               : 1;    //8.6   线圈电压  IBP-k33
    int8u                               : 1;    //8.7   故障位    IBP
                                                    
    //BYTE 9                                        
    int8u   bIBPRelay_K31K32_Coil       : 1;    //9.0   线圈电压  KT1KT4
    int8u   bIBPRelay_K31K32_Fault      : 1;    //9.1   故障位
    int8u   bIBPRelay_K33_Coil          : 1;    //9.2   线圈电压  KT2KT5
    int8u   bIBPRelay_K33_Fault         : 1;    //9.3   故障位
    int8u                               : 1;    //9.4   线圈电压  KT3KT6
    int8u                               : 1;    //9.5   故障位
    int8u   bIBPRelay_SF_Coil           : 1;    //9.6   线圈电压  SF
    int8u   bIBPRelay_SF_Fault          : 1;    //9.7   故障位
    
    //BYTE 10
    int8u   Byte10                      : 8;
    
    //BYTE 11
    int8u   PEDC_SF_FLASH_BUF           : 8;

}tToMMSRelay;



typedef struct
{                                                     
    //BYTE 0                                          
    int8u   __bPedcSIG_EN               : 1;    //0.0   SIG使能
    int8u   bPedcSIG_CLS                : 1;    //0.1   SIG关门
    int8u   bPedcSIG_OPN6               : 1;    //0.2   SIG开6车
    int8u   __bPedcSIG_OPN8             : 1;    //0.3   SIG开8车
    int8u                               : 1;    //0.4   
    int8u   bPedc_CLSLCK                : 1;    //0.5   所有门关闭且锁紧
    int8u   ATC_COMMAND_FAILURE         : 1;    //0.6   命令错误 ，开关门同时 1
    int8u   bPedc_NodeCfgErr            : 1;    //0.7   通讯节点配置错误
                                                      
    //BYTE 1                                          
    int8u   bPedcPSL1_EN                : 1;    //1.0   PSL1操作允许
    int8u   bPedcPSL1_CLS               : 1;    //1.1   PSL1关门
    int8u   bPedcPSL1_OPN6              : 1;    //1.2   PSL1开6车
    int8u   __bPedcPSL1_OPN8            : 1;    //1.3   PSL1开8车
    int8u   bPedcPSL1_BYPASS            : 1;    //1.4   PSL1互锁解除
    int8u                               : 1;    //1.5 
    int8u                               : 1;    //1.6 
    int8u                               : 1;    //1.7 
                                                      
                                                      
                                                      
    //BYTE 2                                          
    int8u   bPedcPSL2_EN                : 1;    //2.0   PSL2操作允许
    int8u   bPedcPSL2_CLS               : 1;    //2.1   PSL2关门
    int8u   bPedcPSL2_OPN6              : 1;    //2.2   PSL2开6车
    int8u   __bPedcPSL2_OPN8            : 1;    //2.3   PSL2开8车
    int8u   bPedcPSL2_BYPASS            : 1;    //2.4   PSL2互锁解除
    int8u                               : 1;    //2.5 
    int8u                               : 1;    //2.6 
    int8u                               : 1;    //2.7 
                                                      
    //BYTE 3                                          
    int8u   bPedcPSL3_EN                : 1;    //3.0   PSL3操作允许
    int8u   bPedcPSL3_CLS               : 1;    //3.1   PSL3关门
    int8u   bPedcPSL3_OPN6              : 1;    //3.2   PSL3开6车
    int8u   __bPedcPSL3_OPN8            : 1;    //3.3   PSL3开8车
    int8u   bPedcPSL3_BYPASS            : 1;    //3.4   PSL3互锁解除
    int8u                               : 1;    //3.5 
    int8u                               : 1;    //3.6 
    int8u                               : 1;    //3.7 
                                                      
    //BYTE 4                                          
    int8u   bPedcIBP_EN                 : 1;    //4.0   IBP操作允许
    int8u   __bPedcIBP_CLS              : 1;    //4.1   IBP关门
    int8u   bPedcIBP_OPN6               : 1;    //4.2   IBP开6车
    int8u   __bPedcIBP_OPN8             : 1;    //4.3   IBP开8车
    int8u                               : 1;    //4.4   
    int8u                               : 1;    //4.5 
    int8u                               : 1;    //4.6 
    int8u                               : 1;    //4.7 
                                                      
    //BYTE 5                                          
    int8u   bPedcPSC_CLS6_1             : 1;    //5.0   PSC CLS6_1
    int8u   bPedcPSC_OPN6_1             : 1;    //5.1   PSC OPN6_1
    int8u   bPedcPSC_CLS6_2             : 1;    //5.2   PSC CLS6_2
    int8u   bPedcPSC_OPN6_2             : 1;    //5.3   PSC OPN6_2
    int8u   __bPedcPSC_CLS8_1           : 1;    //5.4   PSC CLS8_1
    int8u   __bPedcPSC_OPN8_1           : 1;    //5.5   PSC OPN8_1
    int8u   __bPedcPSC_CLS8_2           : 1;    //5.6   PSC CLS8_2
    int8u   __bPedcPSC_OPN8_2           : 1;    //5.7   PSC OPN8_2
                                                      
    //BYTE 6                                          
    int8u   bPedcPSC_IBPPSL_OPN1        : 1;    //6.0   IBP/PSL开门1
    int8u   bPedcPSC_IBPPSL_OPN2        : 1;    //6.1   IBP/PSL开门2
    int8u                               : 1;    //6.2 
    int8u                               : 1;    //6.3 
    int8u                               : 1;    //6.4 
    int8u                               : 1;    //6.5 
    int8u                               : 1;    //6.6 
    int8u                               : 1;    //6.7 
                                                      
                                                      
    //BYTE 7                                          
    int8u   bPowerWarning_S0            : 1;    //7.0   交流电源故障                 
    int8u   bPowerWarning_S1            : 1;    //7.1   驱动电源故障
    int8u   bPowerWarning_S2            : 1;    //7.2   驱动电池投入使用
    int8u   bPowerWarning_S3            : 1;    //7.3   控制电源故障
    int8u   bPowerWarning_S4            : 1;    //7.4   控制电源投入使用
    int8u                               : 1;    //7.5 
    int8u                               : 1;    //7.6 
    int8u                               : 1;    //7.7 
                                                      
    //BYTE 8                                          
    int8u   bPEDC_PIC24A_OFFLINE        : 1;    //8.0   PIC24A 离线标志
    int8u   bPEDC_PIC24B_OFFLINE        : 1;    //8.1   PIC24B 离线标志
    int8u   bPEDC_PSL_OFFLINE           : 1;    //8.2   PSL    离线标志
    int8u   bPEDC_SIG_OFFLINE           : 1;    //8.3   SIG    离线标志
    int8u   bPEDC_IBP_OFFLINE           : 1;    //8.4   IBP    离线标志
    int8u                               : 1;    //8.5 
    int8u                               : 1;    //8.6 
    int8u                               : 1;    //8.7 
                                                      
    //BYTE 9      /* 风阀 */                          
    int8u   bPEDC_SigSys_OffLine        : 1;    //9.0   信号系统离线标志
    int8u                               : 1;    //9.1   
    int8u                               : 1;    //9.2   
    int8u                               : 1;    //9.3   
    int8u                               : 1;    //9.4 
    int8u                               : 1;    //9.5 
    int8u                               : 1;    //9.6 
    int8u                               : 1;    //9.7   
                                                     
    //BYTE 10                                        
    int8u                               : 1;    //10.0  
    int8u                               : 1;    //10.1  
    int8u                               : 1;    //10.2
    int8u                               : 1;    //10.3
    int8u                               : 1;    //10.4
    int8u                               : 1;    //10.5  
    int8u                               : 1;    //10.6  
    int8u                               : 1;    //10.7  
                                                     
    /* 板间连线的信息 */                             
    //BYTE 11     //MONITOR                          
    int8u                               : 1;    //11.0  
    int8u                               : 1;    //11.1  
    int8u                               : 1;    //11.2
    int8u                               : 1;    //11.3
    int8u                               : 1;    //11.4
    int8u   MON_LinkErr_OPNSD           : 1;    //11.5  MON板开边门连线缺失
    int8u   MON_LinkErr_IBPPSL_OPN      : 1;    //11.6  MON板IBP-PSL 连线缺失
    int8u   MON_LinkErr_OPNCLS          : 1;    //11.7  MON板开关门命令缺失
                                                     
                                                     
    //BYTE 12     //IBP                              
    int8u   IBP_LinkErr_IBPEN           : 1;    //12.0  IBP板IBP-EN缺失
    int8u   IBP_LinkErr_OPN6            : 1;    //12.1  IBP板OPN6缺失
    int8u                               : 1;    //12.2
    int8u                               : 1;    //12.3
    int8u                               : 1;    //12.4
    int8u                               : 1;    //12.5
    int8u   IBP_LinkErr_IBPPSL_Opn      : 1;    //12.6  IBP板IBP-PSL 连线缺失
    int8u   IBP_LinkErr_OPNCLS          : 1;    //12.7  IBP板开关门命令缺失
                                                     
    //BYTE 13     //PSL                              
    int8u   PSL_LinkErr_IBPEN           : 1;    //13.0  PSL板IBP-EN缺失
    int8u   PSL_LinkErr_OPN6            : 1;    //13.1  PSL板OPN6缺失
    int8u   PSL_LinkErr_PSLEN           : 1;    //13.2  PSL板 PSL1/2/3-EN 缺失
    int8u                               : 1;    //13.3
    int8u                               : 1;    //13.4
    int8u                               : 1;    //13.5
    int8u   PSL_LinkErr_IBPPSL_Opn      : 1;    //13.6  PSL板IBP-PSL 连线缺失
    int8u   PSL_LinkErr_OPNCLS          : 1;    //13.7  PSL板开关门命令缺失
                                                     
    //BYTE 14     //SIG                              
    int8u   SIG_LinkErr_IBPEN           : 1;    //14.0  SIG板IBP-EN缺失
    int8u   SIG_LinkErr_OPN6            : 1;    //14.1  SIG板OPN6缺失
    int8u   SIG_LinkErr_PSLEN           : 1;    //14.2  SIG板 PSL1/2/3-EN 缺失
    int8u                               : 1;    //14.3
    int8u                               : 1;    //14.4
    int8u                               : 1;    //14.5
    int8u                               : 1;    //14.6
    int8u   SIG_LinkErr_OPNCLS          : 1;    //14.7  SIG板开关门命令缺失

    //BYTE 15
    int8u   ToMMS_PEDC_ID               : 8;    //15
}tToMMSExtendIO;



