
//�̵��� ״̬(����λ��ʾ)
//  	00		01		10		11
//     (�Ͽ�����)	����		����		��
typedef struct
{
    //BYTE 0 
    int8u   bSIGRelay_K0_Coil           : 1;    //0.0   ��Ȧ��ѹ  SIG-k0
    int8u   bSIGRelay_K0_Fault          : 1;    //0.1   ����λ
    int8u   bSIGRelay_K1AK1B_Coil       : 1;    //0.2   ��Ȧ��ѹ  SIG-k1
    int8u   bSIGRelay_K1AK1B_Fault      : 1;    //0.3   ����λ
    int8u   bSIGRelay_K2AK2B_Coil       : 1;    //0.4   ��Ȧ��ѹ  SIG-k2
    int8u   bSIGRelay_K2AK2B_Fault      : 1;    //0.5   ����λ
    int8u   bSIGRelay_K3AK3B_Coil       : 1;    //0.6   ��Ȧ��ѹ  SIG-k3
    int8u   bSIGRelay_K3AK3B_Fault      : 1;    //0.7   ����λ
                                                    
    //BYTE 1                                        
    int8u   bSIGRelay_K4AK4B_Coil       : 1;    //1.0   ��Ȧ��ѹ  SIG-k4
    int8u   bSIGRelay_K4AK4B_Fault      : 1;    //1.1   ����λ
    int8u   bSIGRelay_K5AK5B_Coil       : 1;    //1.2   ��Ȧ��ѹ  SIG-k5
    int8u   bSIGRelay_K5AK5B_Fault      : 1;    //1.3   ����λ
    int8u   bSIGRelay_K10_Coil          : 1;    //1.4   ��Ȧ��ѹ  SIG-k10
    int8u   bSIGRelay_K10_Fault         : 1;    //1.5   ����λ
    int8u   bSIGRelay_K20_Coil          : 1;    //1.6   ��Ȧ��ѹ  SIG-k20
    int8u   bSIGRelay_K20_Fault         : 1;    //1.7   ����λ
                                                    
    //BYTE 2                                        
    int8u   bSIGRelay_K30_Coil          : 1;    //2.0   ��Ȧ��ѹ  SIG-k30
    int8u   bSIGRelay_K30_Fault         : 1;    //2.1   ����λ
    int8u   bSIGRelay_K31_Coil          : 1;    //2.2   ��Ȧ��ѹ  SIG-k31
    int8u   bSIGRelay_K31_Fault         : 1;    //2.3   ����λ
    int8u   bSIGRelay_K32_Coil          : 1;    //2.4   ��Ȧ��ѹ  SIG-k32
    int8u   bSIGRelay_K32_Fault         : 1;    //2.5   ����λ
    int8u   __bSIGRelay_K41K42_Coil     : 1;    //2.6   ��Ȧ��ѹ  SIG-k41&k42
    int8u   __bSIGRelay_K41K42_Fault    : 1;    //2.7   ����λ
                                                    
                                                          
    //BYTE 3                                        
    int8u   __bSIGRelay_K43_Coil        : 1;    //3.0   ��Ȧ��ѹ  SIG-k43 
    int8u   __bSIGRelay_K43_Fault       : 1;    //3.1   ����λ            
    int8u                               : 1;    //3.2   ��Ȧ��ѹ  
    int8u                               : 1;    //3.3   ����λ
    int8u                               : 1;    //3.4   ��Ȧ��ѹ  
    int8u                               : 1;    //3.5   ����λ
    int8u                               : 1;    //3.6   ��Ȧ��ѹ  
    int8u                               : 1;    //3.7   ����λ
                                                    
    //BYTE 4                                        
    int8u   bPSLRelay_K1_Coil           : 1;    //4.0   ��Ȧ��ѹ  PSL-k1
    int8u   bPSLRelay_K1_Fault          : 1;    //4.1   ����λ
    int8u   bPSLRelay_K10_Coil          : 1;    //4.2   ��Ȧ��ѹ  PSL-k10
    int8u   bPSLRelay_K10_Fault         : 1;    //4.3   ����λ
    int8u   bPSLRelay_K11_Coil          : 1;    //4.4   ��Ȧ��ѹ  PSL-k11
    int8u   bPSLRelay_K11_Fault         : 1;    //4.5   ����λ
    int8u   bPSLRelay_K12_Coil          : 1;    //4.6   ��Ȧ��ѹ  PSL-k12
    int8u   bPSLRelay_K12_Fault         : 1;    //4.7   ����λ
                                                    
    //BYTE 5                                        
    int8u   bPSLRelay_K13_Coil          : 1;    //5.0   ��Ȧ��ѹ  PSL-k13
    int8u   bPSLRelay_K13_Fault         : 1;    //5.1   ����λ
    int8u   bPSLRelay_K14_Coil          : 1;    //5.2   ��Ȧ��ѹ  PSL-k14
    int8u   bPSLRelay_K14_Fault         : 1;    //5.3   ����λ
    int8u   bPSLRelay_K15_Coil          : 1;    //5.4   ��Ȧ��ѹ  PSL-k15
    int8u   bPSLRelay_K15_Fault         : 1;    //5.5   ����λ
    int8u   bPSLRelay_K16_Coil          : 1;    //5.6   ��Ȧ��ѹ  PSL-k16
    int8u   bPSLRelay_K16_Fault         : 1;    //5.7   ����λ
                                                    
    //BYTE 6                                        
    int8u   bIBPRelay_K1_Coil           : 1;    //6.0   ��Ȧ��ѹ  IBP-k1
    int8u   bIBPRelay_K1_Fault          : 1;    //6.1   ����λ    IBP
    int8u   __bIBPRelay_K10_Coil        : 1;    //6.2   ��Ȧ��ѹ  IBP-k10
    int8u   __bIBPRelay_K10_Fault       : 1;    //6.3   ����λ    IBP
    int8u   bIBPRelay_K21_Coil          : 1;    //6.4   ��Ȧ��ѹ  IBP-k21
    int8u   bIBPRelay_K21_Fault         : 1;    //6.5   ����λ    IBP
    int8u   bIBPRelay_K22_Coil          : 1;    //6.6   ��Ȧ��ѹ  IBP-k22
    int8u   bIBPRelay_K22_Fault         : 1;    //6.7   ����λ    IBP
                                                    
    //BYTE 7                                        
    int8u   bIBPRelay_K23_Coil          : 1;    //7.0   ��Ȧ��ѹ  IBP-k23
    int8u   bIBPRelay_K23_Fault         : 1;    //7.1   ����λ    IBP
    int8u   bIBPRelay_K24_Coil          : 1;    //7.2   ��Ȧ��ѹ  IBP-k24
    int8u   bIBPRelay_K24_Fault         : 1;    //7.3   ����λ    IBP
    int8u   __bIBPRelay_K25_Coil        : 1;    //7.4   ��Ȧ��ѹ  IBP-k25
    int8u   __bIBPRelay_K25_Fault       : 1;    //7.5   ����λ    IBP
    int8u   __bIBPRelay_K26_Coil        : 1;    //7.6   ��Ȧ��ѹ  IBP-k26
    int8u   __bIBPRelay_K26_Fault       : 1;    //7.7   ����λ    IBP
                                                    
                                                    
    //BYTE 8                                        
    int8u   __bIBPRelay_K27_Coil        : 1;    //8.0   ��Ȧ��ѹ  IBP-k27
    int8u   __bIBPRelay_K27_Fault       : 1;    //8.1   ����λ    IBP
    int8u   bIBPRelay_K28_Coil          : 1;    //8.2   ��Ȧ��ѹ  IBP-k28
    int8u   bIBPRelay_K28_Fault         : 1;    //8.3   ����λ    IBP
    int8u   bIBPRelay_K29_Coil          : 1;    //8.4   ��Ȧ��ѹ  IBP-k29
    int8u   bIBPRelay_K29_Fault         : 1;    //8.5   ����λ    IBP
    int8u                               : 1;    //8.6   ��Ȧ��ѹ  IBP-k33
    int8u                               : 1;    //8.7   ����λ    IBP
                                                    
    //BYTE 9                                        
    int8u   bIBPRelay_K31K32_Coil       : 1;    //9.0   ��Ȧ��ѹ  KT1KT4
    int8u   bIBPRelay_K31K32_Fault      : 1;    //9.1   ����λ
    int8u   bIBPRelay_K33_Coil          : 1;    //9.2   ��Ȧ��ѹ  KT2KT5
    int8u   bIBPRelay_K33_Fault         : 1;    //9.3   ����λ
    int8u                               : 1;    //9.4   ��Ȧ��ѹ  KT3KT6
    int8u                               : 1;    //9.5   ����λ
    int8u   bIBPRelay_SF_Coil           : 1;    //9.6   ��Ȧ��ѹ  SF
    int8u   bIBPRelay_SF_Fault          : 1;    //9.7   ����λ
    
    //BYTE 10
    int8u   Byte10                      : 8;
    
    //BYTE 11
    int8u   PEDC_SF_FLASH_BUF           : 8;

}tToMMSRelay;



typedef struct
{                                                     
    //BYTE 0                                          
    int8u   __bPedcSIG_EN               : 1;    //0.0   SIGʹ��
    int8u   bPedcSIG_CLS                : 1;    //0.1   SIG����
    int8u   bPedcSIG_OPN6               : 1;    //0.2   SIG��6��
    int8u   __bPedcSIG_OPN8             : 1;    //0.3   SIG��8��
    int8u                               : 1;    //0.4   
    int8u   bPedc_CLSLCK                : 1;    //0.5   �����Źر�������
    int8u   ATC_COMMAND_FAILURE         : 1;    //0.6   ������� ��������ͬʱ 1
    int8u   bPedc_NodeCfgErr            : 1;    //0.7   ͨѶ�ڵ����ô���
                                                      
    //BYTE 1                                          
    int8u   bPedcPSL1_EN                : 1;    //1.0   PSL1��������
    int8u   bPedcPSL1_CLS               : 1;    //1.1   PSL1����
    int8u   bPedcPSL1_OPN6              : 1;    //1.2   PSL1��6��
    int8u   __bPedcPSL1_OPN8            : 1;    //1.3   PSL1��8��
    int8u   bPedcPSL1_BYPASS            : 1;    //1.4   PSL1�������
    int8u                               : 1;    //1.5 
    int8u                               : 1;    //1.6 
    int8u                               : 1;    //1.7 
                                                      
                                                      
                                                      
    //BYTE 2                                          
    int8u   bPedcPSL2_EN                : 1;    //2.0   PSL2��������
    int8u   bPedcPSL2_CLS               : 1;    //2.1   PSL2����
    int8u   bPedcPSL2_OPN6              : 1;    //2.2   PSL2��6��
    int8u   __bPedcPSL2_OPN8            : 1;    //2.3   PSL2��8��
    int8u   bPedcPSL2_BYPASS            : 1;    //2.4   PSL2�������
    int8u                               : 1;    //2.5 
    int8u                               : 1;    //2.6 
    int8u                               : 1;    //2.7 
                                                      
    //BYTE 3                                          
    int8u   bPedcPSL3_EN                : 1;    //3.0   PSL3��������
    int8u   bPedcPSL3_CLS               : 1;    //3.1   PSL3����
    int8u   bPedcPSL3_OPN6              : 1;    //3.2   PSL3��6��
    int8u   __bPedcPSL3_OPN8            : 1;    //3.3   PSL3��8��
    int8u   bPedcPSL3_BYPASS            : 1;    //3.4   PSL3�������
    int8u                               : 1;    //3.5 
    int8u                               : 1;    //3.6 
    int8u                               : 1;    //3.7 
                                                      
    //BYTE 4                                          
    int8u   bPedcIBP_EN                 : 1;    //4.0   IBP��������
    int8u   __bPedcIBP_CLS              : 1;    //4.1   IBP����
    int8u   bPedcIBP_OPN6               : 1;    //4.2   IBP��6��
    int8u   __bPedcIBP_OPN8             : 1;    //4.3   IBP��8��
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
    int8u   bPedcPSC_IBPPSL_OPN1        : 1;    //6.0   IBP/PSL����1
    int8u   bPedcPSC_IBPPSL_OPN2        : 1;    //6.1   IBP/PSL����2
    int8u                               : 1;    //6.2 
    int8u                               : 1;    //6.3 
    int8u                               : 1;    //6.4 
    int8u                               : 1;    //6.5 
    int8u                               : 1;    //6.6 
    int8u                               : 1;    //6.7 
                                                      
                                                      
    //BYTE 7                                          
    int8u   bPowerWarning_S0            : 1;    //7.0   ������Դ����                 
    int8u   bPowerWarning_S1            : 1;    //7.1   ������Դ����
    int8u   bPowerWarning_S2            : 1;    //7.2   �������Ͷ��ʹ��
    int8u   bPowerWarning_S3            : 1;    //7.3   ���Ƶ�Դ����
    int8u   bPowerWarning_S4            : 1;    //7.4   ���Ƶ�ԴͶ��ʹ��
    int8u                               : 1;    //7.5 
    int8u                               : 1;    //7.6 
    int8u                               : 1;    //7.7 
                                                      
    //BYTE 8                                          
    int8u   bPEDC_PIC24A_OFFLINE        : 1;    //8.0   PIC24A ���߱�־
    int8u   bPEDC_PIC24B_OFFLINE        : 1;    //8.1   PIC24B ���߱�־
    int8u   bPEDC_PSL_OFFLINE           : 1;    //8.2   PSL    ���߱�־
    int8u   bPEDC_SIG_OFFLINE           : 1;    //8.3   SIG    ���߱�־
    int8u   bPEDC_IBP_OFFLINE           : 1;    //8.4   IBP    ���߱�־
    int8u                               : 1;    //8.5 
    int8u                               : 1;    //8.6 
    int8u                               : 1;    //8.7 
                                                      
    //BYTE 9      /* �緧 */                          
    int8u   bPEDC_SigSys_OffLine        : 1;    //9.0   �ź�ϵͳ���߱�־
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
                                                     
    /* ������ߵ���Ϣ */                             
    //BYTE 11     //MONITOR                          
    int8u                               : 1;    //11.0  
    int8u                               : 1;    //11.1  
    int8u                               : 1;    //11.2
    int8u                               : 1;    //11.3
    int8u                               : 1;    //11.4
    int8u   MON_LinkErr_OPNSD           : 1;    //11.5  MON�忪��������ȱʧ
    int8u   MON_LinkErr_IBPPSL_OPN      : 1;    //11.6  MON��IBP-PSL ����ȱʧ
    int8u   MON_LinkErr_OPNCLS          : 1;    //11.7  MON�忪��������ȱʧ
                                                     
                                                     
    //BYTE 12     //IBP                              
    int8u   IBP_LinkErr_IBPEN           : 1;    //12.0  IBP��IBP-ENȱʧ
    int8u   IBP_LinkErr_OPN6            : 1;    //12.1  IBP��OPN6ȱʧ
    int8u                               : 1;    //12.2
    int8u                               : 1;    //12.3
    int8u                               : 1;    //12.4
    int8u                               : 1;    //12.5
    int8u   IBP_LinkErr_IBPPSL_Opn      : 1;    //12.6  IBP��IBP-PSL ����ȱʧ
    int8u   IBP_LinkErr_OPNCLS          : 1;    //12.7  IBP�忪��������ȱʧ
                                                     
    //BYTE 13     //PSL                              
    int8u   PSL_LinkErr_IBPEN           : 1;    //13.0  PSL��IBP-ENȱʧ
    int8u   PSL_LinkErr_OPN6            : 1;    //13.1  PSL��OPN6ȱʧ
    int8u   PSL_LinkErr_PSLEN           : 1;    //13.2  PSL�� PSL1/2/3-EN ȱʧ
    int8u                               : 1;    //13.3
    int8u                               : 1;    //13.4
    int8u                               : 1;    //13.5
    int8u   PSL_LinkErr_IBPPSL_Opn      : 1;    //13.6  PSL��IBP-PSL ����ȱʧ
    int8u   PSL_LinkErr_OPNCLS          : 1;    //13.7  PSL�忪��������ȱʧ
                                                     
    //BYTE 14     //SIG                              
    int8u   SIG_LinkErr_IBPEN           : 1;    //14.0  SIG��IBP-ENȱʧ
    int8u   SIG_LinkErr_OPN6            : 1;    //14.1  SIG��OPN6ȱʧ
    int8u   SIG_LinkErr_PSLEN           : 1;    //14.2  SIG�� PSL1/2/3-EN ȱʧ
    int8u                               : 1;    //14.3
    int8u                               : 1;    //14.4
    int8u                               : 1;    //14.5
    int8u                               : 1;    //14.6
    int8u   SIG_LinkErr_OPNCLS          : 1;    //14.7  SIG�忪��������ȱʧ

    //BYTE 15
    int8u   ToMMS_PEDC_ID               : 8;    //15
}tToMMSExtendIO;



