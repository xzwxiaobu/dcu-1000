
#include "..\Include.h"

void CpuInit(void)
{

    asm volatile("di");                                // Disable all interrupts

    INTConfigureSystem(INT_SYSTEM_CONFIG_MULT_VECTOR); // configure for single vectored mode


    RCON = 0;   //复位控制寄存器
    DDPCON = 0;             //关闭JTAG
    AD1PCFGSET = 0xFFFF;    // PORTB = Digital;


    //POR复位将强制所有USB模块寄存器为其复位状态。这将关闭USB模块。
    //

    //设置TIME2 , 16位计时器，主频：Internal peripheral clock 40 m,Prescaler 1:64, PRIOR_3/SUB_PRIOR_2, PR2 = 625 ; 1ms(40m = 25ns ,80m 64分频约 25ns * 64 = 1600ns ;1ms = 1000000ns ,PR2 = 1m / 1600ns = 625)
    CloseTimer2();
    ConfigIntTimer2(T2_INT_OFF | T2_INT_PRIOR_5 |  T2_INT_SUB_PRIOR_2);
    OpenTimer2(T2_ON | T2_GATE_OFF | T2_PS_1_64 | T2_32BIT_MODE_OFF | T2_SOURCE_INT, 625-1);
    EnableIntT2;

    //IO PORT initial
    CloseADC10();                                        //ensure the ADC is off before setting the configuration
    CloseINT0();                                         //disables the external interrupt
    CloseINT1();                                         //disables the external interrupt
    CloseINT2();                                         //disables the external interrupt
    CloseINT3();                                         //disables the external interrupt
    CloseINT4();                                         //disables the external interrupt
}

void RamIni(void)
{
    int8u i;

    tagCANFUN.uwCANFUN_FLAG0.word = 0;
    UART2_TX_Cnt.word = 0;
    for(i = 0; i < DCU_CAN12_STATE_LEN; i ++)
    {
        uniDcuData.sortBuf.aDcuCan12State[i] = 0;
    }

    for(i = 0; i < OUTPUT_DATA_LEN; i ++)
    {
        MON_output_data[i].byte = 0xff;            //输出灯全灭
    }
    
    for(i = 0; i < DCUsetting_LEN; i ++)  //将FLASH 读取的 设置赋值给 settingBUF
    {
        uDCUsetting.settingBUF[i] = uDCUsetting.ReadSettingBUF[i];
    }   
}


