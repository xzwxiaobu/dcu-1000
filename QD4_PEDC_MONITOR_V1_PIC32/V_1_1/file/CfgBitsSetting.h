
/*******************************************************************************
 * Configuration Bit settings for PIC32MX795F512H
 * SYSCLK = 80 MHz (8MHz Crystal/ FPLLIDIV * FPLLMUL / FPLLODIV)
 * PBCLK = 40 MHz
 * Primary Osc w/PLL (XT+,HS+,EC+PLL)
 * WDT OFF
 */
#pragma config FPLLMUL = MUL_20, FPLLIDIV = DIV_2, FPLLODIV = DIV_1, FWDTEN = OFF
#pragma config POSCMOD = HS, FNOSC = PRIPLL, FPBDIV = DIV_2
#pragma config FSOSCEN = OFF          /* Secondary oscillator Enable bit: Disabled                        */                         

#pragma config FVBUSONIO = OFF        /* VBUS_ON pin is controlled by the Port Function                   */
#pragma config FUSBIDIO  = OFF        /* USBID pin is controlled by the Port Function                     */
#pragma config FETHIO    = OFF        /* Alternate Ethernet IO Pins                                       */
#pragma config FSRSSEL   = PRIORITY_7 /* SRS Interrupt Priority Level 7                                   */
#pragma config OSCIOFNC  = OFF        /* CLKO Disabled                                                    */
#pragma config CP        = OFF        /* Code Protect Enable bit: Enabled                                 */
#pragma config BWP       = OFF        /* Boot Flash Write Protect bit                                     */
#pragma config ICESEL    = ICS_PGx1   /* ICE/ICD Comm Channel Select: ICE pins are shared with PGC1, PGD1 */
#pragma config IESO      = ON         /* Internal External Switch Over                                    */
#pragma config PWP       = OFF        /* Program Flash Write Protect                                      */
#pragma config DEBUG     = OFF        /* Background Debugger Enable                                       */
#pragma config FCKSM     = CSDCMD     /* Clock Switching and Monitor Selection                            */


/* 用户设定使能 Watchdog
 * 0 = disable， else = enable
 * */
#define USR_WDT_ON          1
#if (USR_WDT_ON != 0)
/* WDT clock : LPRC freq = 31.25KHz  (偏差 +/- 15%)
 * WDT 可以使用外部时钟源或内部 LPRC 振荡器（标称频率为 32 kHz）作为时钟源。
 * 不使用后分频器时，对于 WDT 会产生 1 毫秒的标称超时周期 （ TWDT） 。
 * 
 * 设置这个时间必须考虑在线升级时消耗的最大时间
 * 行写周期 典型值 4.5 ms（一行包含 128 个 32 位指令字或 512 字节）
 * 将下载区内容复制到主程序区所需典型时间：240K / 0.5K × 4.5 ＝ 2.16s
 */
#pragma config WDTPS = PS8192       /* ~8192 ms 复位 */

#endif

