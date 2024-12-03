
//#include "..\include.h"

#include <string.h>
#include "_cpu.h"

#if (!defined(PCB_PIC24))
#include <plib.h>
#include <peripheral/ports.h>
#endif

#include "IOConfig.h"

/* MCU 的GPIO 配置 
 * BIT位为1 表示对应的脚位配置为输入
 */
PRIVATE int16u pic32GpioConfig[6] = 
{
    #if (defined(PCB_MON_V1))
    /* PEDC_MONITOR_V1.pdf */
    0xffff ^ (BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_8  | BIT_12 | BIT_13 | BIT_15), //PB
    BIT_12 | BIT_15,                                        //PC
    BIT_2 | BIT_9,                                          //PD
    0xffff,                                                 //PE
    BIT_0 | BIT_4,                                          //PF   C1RXD RXD0                            
    BIT_7 | BIT_9,                                          //PG
    
    #elif (defined(PCB_MON_V2))
    #error "TODO"
    
    #elif (defined(PCB_MON_SZ20_V1))
    /* PEDC_MONITOR_SZ20_V1.pdf 或 
     * PEDC_MONITOR_SZ16.pdf
     */
    0xffff ^ (BIT_2 | BIT_3 | BIT_4 | BIT_5 | BIT_6 | BIT_8 | BIT_15), //PB
    BIT_12 | BIT_15,                                        //PC
    BIT_2 | BIT_9,                                          //PD
    0xffff,                                                 //PE
    BIT_0 | BIT_4,                                          //PF   C1RXD RXD0                            
    BIT_7 | BIT_9,                                          //PG

    #elif (defined(PCB_FD_PEDC_SZ3Q_M))
    BIT_0 | BIT_1 | BIT_14,                                 //PB, PortB.0.1 as input (PGEC1/PGED1) ,PB14 as input(C2RXD)
    0xffff ^ (BIT_13 | BIT_14),                             //PC, .13.14 set output
    BIT_2 | BIT_9,                                          //PD, .2(RXD2) .9(RXD1)
    0,                                                      //PE, all output
    BIT_0 | BIT_4,                                          //PF   C1RXD RXD0                        
    BIT_7 | BIT_9 | BIT_6 | BIT_8, //PG, .7(RXD3) .8&.6(TXD3 TXD6, set input for test BL PIC24)
    
    #elif (defined(PCB_IBP))
    /* PEDC_IBP_JNR2.pdf :  */
    0xffff,                                                 //PB : 0~15 input
    0xf000,                                                 //PC : 12 13 14 15    
    BIT_0 | BIT_2 | BIT_5 | BIT_7 | BIT_11,                 //PD : 0 2 5 7 11 
    BIT_0 | BIT_5 | BIT_6 | BIT_7,                          //PE : 0 5 6 7
    BIT_0 | BIT_3 | BIT_5 ,                                 //PF : 0 3 5
    BIT_6 | BIT_7 | BIT_9 ,                                 //PG : 6 7 9
    
    #elif (defined(PCB_PSL))
    /* PEDC_PSL_JNR2.pdf :  */
    0xffff - BIT_5,                                         //PB exclude RB5 as input 
    BIT_12 | BIT_15,                                        //PC .12.15 input
    BIT_2,                                                  //PD.2 set input
    BIT_5,                                                  //PE.5 set input
    BIT_3 | BIT_4 | BIT_5,                                  //PF .3.4.5 set input
    0,                                                      //PG input 
    
    #elif (defined(PCB_SIG))
    /* PEDC_SIG_JNR2.pdf */
    BIT_0 | BIT_1,                                          //PB
    BIT_12 | BIT_15,                                        //PC
    BIT_2,                                                  //PD
    BIT_5,                                                  //PE
    0,                                                      //PF
    0,                                                      //PG
    
    #endif
};

PUBLIC int16u * getPic32GPIOsCfg(void)
{
    return pic32GpioConfig;
}

/* 结合使用
 * PORTSetBits(allIOPortsList[i], pic32GpioConfig[i])
 * PORTSetPinsDigitalOut(allIOPortsList[i], ...);
 *  */
IoPortId allIOPortsList[] = 
{
    IOPORT_B, IOPORT_C, IOPORT_D, IOPORT_E, IOPORT_F, IOPORT_G
};


PUBLIC void initPic32GPIOs(void)
{
    int16u * pCfg = pic32GpioConfig;
    unsigned int setInput;
	//PORTB : 
    //SZ20YQ : 因为选择SIG板的继电器有外部自锁功能
    //         所以输出控制选择SIG板的IO口不能输出 0 （选择SIG2）
    setInput = pCfg[0];                         //Set PortB input pin
	mPORTBSetBits(0xffff);
	mPORTBSetPinsDigitalOut(0xffff);            //Set PortB as digital out
	mPORTBSetPinsDigitalIn(setInput);           //Set PortB input pin
	mPORTBSetBits(0xffff ^ setInput);           //PORTB out pin ; out 1

	//PORTC : 
    setInput = pCfg[1];                         //Set PortC input pin
	mPORTCSetBits(0xffff);
	mPORTCSetPinsDigitalOut(0xffff);                    //Set PortC as digital out
	mPORTCSetPinsDigitalIn(setInput);                   //PORTC set input
	mPORTCClearBits(0xffff ^ setInput);                 //PORTC out pin ; out 0
    
	//PORTD :  
    setInput = pCfg[2];                        //Set PortD input pin
	mPORTDSetBits(0xffff);
	mPORTDSetPinsDigitalOut(0xffff);
	mPORTDSetPinsDigitalIn( setInput );                 //PD set input
	mPORTDClearBits(0xffff ^ setInput);                 //PORTD out pin ; out 0

	//PORTE : 
    setInput = pCfg[3];                        //Set PortE input pin
	mPORTESetBits(0xFFFF);
	mPORTESetPinsDigitalOut(0xffff);                    //PORTE set output
	mPORTESetPinsDigitalIn(setInput);                   //PE set input
	mPORTEClearBits(0xffff - setInput);                 //PORTE out pin ; out 0

	//PORTF
    setInput = pCfg[4];                        //Set PortF input pin
	mPORTFSetBits(0xFFFF);
	mPORTFSetPinsDigitalOut(0xffff);                    //PORTF set output
	mPORTFSetPinsDigitalIn( setInput );                 //PORTF .3.4.5 set input
	mPORTFClearBits(0xffff ^ setInput);                 //PORTF out pin ; out 0

	//PORTG
    setInput = pCfg[5];                        //Set PortG input pin
	mPORTGSetBits(0xFFFF);
	mPORTGSetPinsDigitalOut(0xffff);                    //PORTG set output
	mPORTGSetPinsDigitalIn( setInput );                 //PORTG set input
	mPORTGClearBits(0xffff ^ setInput);                 //PORTG out 0
}


