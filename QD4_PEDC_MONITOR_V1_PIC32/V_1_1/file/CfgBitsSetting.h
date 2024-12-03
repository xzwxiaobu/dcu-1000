
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


/* �û��趨ʹ�� Watchdog
 * 0 = disable�� else = enable
 * */
#define USR_WDT_ON          1
#if (USR_WDT_ON != 0)
/* WDT clock : LPRC freq = 31.25KHz  (ƫ�� +/- 15%)
 * WDT ����ʹ���ⲿʱ��Դ���ڲ� LPRC ���������Ƶ��Ϊ 32 kHz����Ϊʱ��Դ��
 * ��ʹ�ú��Ƶ��ʱ������ WDT ����� 1 ����ı�Ƴ�ʱ���� �� TWDT�� ��
 * 
 * �������ʱ����뿼����������ʱ���ĵ����ʱ��
 * ��д���� ����ֵ 4.5 ms��һ�а��� 128 �� 32 λָ���ֻ� 512 �ֽڣ�
 * �����������ݸ��Ƶ����������������ʱ�䣺240K / 0.5K �� 4.5 �� 2.16s
 */
#pragma config WDTPS = PS8192       /* ~8192 ms ��λ */

#endif

