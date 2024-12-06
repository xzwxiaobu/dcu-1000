 /*=============================================================================
 * Project:         QD4 MONITOR PIC32
 *                  PCB: FD-PEDC-MONITOR-SZ16
 * PEDC MCU:        PIC32MX795F512H  - 512K Flash
 * Fcrystal:        8M
 * PLL:             X20
 * FCY:             SYSTEM_FREQ = 80M
 *
 * Version:     1.0
 * Author:      LLY
 * Date:        2021-9-22
 * NOTE:        
 *              ϵͳ��MONITOR -- ��ȫ��·�л��� |-- �߼���1(UA1 + UB1)
 *                                              |-- �߼���2(UA1 + UB1)
 *              1. ֧���������������壬֧�� PIC24 IAP��������
 *                 ע�� �߼����� PIC32UA1 PIC32UB1 2��MCU��Ҫ����
 *                      �߼���UA1оƬ����Ŀ�ݶ�����
 *              2. �߼���1 2�����෽ʽ��MONITOR���������ȫ��·������л�
 *              3. 2022-6 ����֧����������DCU���ܣ���ҪMMS��DCU��ϣ�
 *              4. 2023-8-21 ���� DCU����޸Ĺ��� 0x23 0x28
 *                  
 * ===========================================================================*/


#define ROOT
#include "CfgBitsSetting.h"

#include "_cpu.h"
#include "Include.h"
#include "sysTimer.h"

#include "uartDrv.h"

#include "PIC24_App.h"
#include "IPS_App.h"
#include "IPSUart.h"

#include "BASSys_App.h"


#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* �ź�ϵͳ�������� 
 * 0 = disable�� else = enable
 */
#define SIGSYS_FUNCT_EN     0

/* BASϵͳ�������� 
 * 0 = disable�� else = enable
 */
#define BASSYS_FUNCT_EN     0

#if (USR_WDT_ON != 0)
PRIVATE volatile BOOL wdtReset;
#endif

/* �ϵ��ȴ�һ��ʱ���ٴ��� IO ���ݡ�Ȼ���ϴ���MMS
 * ���⿪ʼ����ʱ���ռ���IO����׼ȷ������������б��̵�������֮�������
 *  */
#define DELAY_PROCESSIO_MS          3000    /*  ms */
PRIVATE int32u delayProcessIO;
PUBLIC BOOL chkDelayProcessIO(void)
{
    return (delayProcessIO == 0);
}

/* =============================================================================
 * main program entrance
 */

__attribute__((section (".SEG_MainHandler"))) main()
{
    /* ��ʱ��鴮��ͨ�Ŵ���״̬ */
    static tSYSTICK     tickChkUartErr;
    
	int i;
    tSYSTICK flashLEDTick;
    tSYSTICK tickNow;
    
	SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	CpuInit();                              /* cpu initial */
    initPic32GPIOs();                       /* ��ʼ�� GPIO */

	asm volatile("di");                     /*  Disable all interrupts          */
	reset_Read_IbpAlarmX();                 /* �ϵ��ȡ IBP ALARM               */
	reset_Read_IbpAlarmX();                 /* �ϵ��ȡ IBP ALARM               */
	Read_DCUsetting_From_Flash();           /* �ϵ��ȡ PEDC Hardware settings  */
    /* ���ڷ緧ȫ����ȫ�صĲ��� */
    setWVOpnClsPara(tagDownIbpAlarmParameter.uRead_WVTotalNum, \
                    tagDownIbpAlarmParameter.uRead_WVNum_Opn,  \
                    tagDownIbpAlarmParameter.uRead_WVNum_Cls);

    /* ��ʼ��CAN */
	ApiMainInit();

	CpuInit();                              /*cpu initial Ӧ�� ApiMainInit()֮��*/

    /* ��ʼ���洢�� */
	RamIni();           
    initSysTimer();                    
    resetSlaveInfo();
    
    /* ��ȡPEDC ID���� */
	scanPEDC_ID();	                        
        
	/* ��ʼ����MMS ͨ�� */
    initMMSModule();
    
    /* ��ʼ����IBP PSL SIG ... ��ͨ�� */
   //  initIPSModule();
    
    /* ��ʼ����PIC24 ��ͨ�� */
    initPic24Module();
    
    #if (SIGSYS_FUNCT_EN != 0)
    /* ��ʼ�����ź�ϵͳͨ�� 
     * initSigSysModule();
     */
    initSigSysModule();
    #endif
    
    #if (BASSYS_FUNCT_EN != 0)
    /* ��ʼ����BASϵͳͨ�� */
    initBASSysModule();
    #endif
    
    /* enable interrupts */
	INTEnableInterrupts();
    
    #if (USR_WDT_ON != 0)   
    /* ��ȡ WDT ��λ��Ϣ��
     * ʹ�� WDT 
     */
    wdtReset = FALSE;
    
    /* check for a previous WDT Event
     * */
    if ( ReadEventWDT() )
    {
        /* A WDT event did occur
         * clear the WDT event flag so a subsequent event can set the event bit
         * */
        i = 1000;
        
        DisableWDT();
        ClearEventWDT();
        while(i--);
        wdtReset = TRUE;
    }
    
    ClearWDT();
    EnableWDT();
    #endif
    
    /* �ϵ�Ĭ��ѡ�� �߼���1 */
    setWorkLOGID(COMMOBJ_LOG1);
    
    sf_flash_AccRcv = 0;    /* �ۼӽ��յİ�ȫ��·���ϴ��� */                  
    sf_flash_toMMS  = 0;    /* ���͸�MMS�ģ���ȫ��·���ϴ��� */
    
    delayProcessIO = DELAY_PROCESSIO_MS;
    /*=======================================
     * ��ѭ��
     */
	while(1)
	{
        /* ���� WDT ��ʱ */
        #if (USR_WDT_ON != 0)
        ClearWDT();
        #endif
        
        /* ����ͨ�����ݴ���
         * 1. ����������ջ����е�����
         * 2. DMA ��ɴ��䷢�������Ժ�Ĵ��� */
        #ifdef     UART_RX_SOFTBUFF
        processAllUartRxDataBuff();
        #endif
        #ifdef     UART_TX_BY_DMA
        checkUartTxDMAFinished();
        #endif
        
        /* ���������������
         * CAN ����ͨ�Ŵ������ 
         */
		ApiMainProcess();

        /* �� MMS ͨ�Ź���
         *  */
        MMS_AppModule();

        /* �������Ӱ��ͨ��
         * �Լ���������
         *  */
        #if (SHOW_PIC24_BY_UART_IPS == 0)
        processIPSComm();
        #endif
        
        /* ��ʱ����
         * ��˸ LED������ MMS ���������� 1HZ ��˸������ 2HZ ��˸
         */
        tSYSTICK tim;
        tim = getUartMMSLinkSts() ? 500: 250;
        tickNow = getSysTick();        
        if((tSYSTICK)(tickNow - flashLEDTick) >= tim)
        {
            flashLEDTick = tickNow;
            LedToggle();
            
            scanPEDC_ID();
        }
        
        /* ��ʱ��⴮�ڴ��� */
        if((tSYSTICK)(tickNow - tickChkUartErr) >= 50)
        {
            tickChkUartErr = tickNow;
            checkAllUartErrorSta();
            
            /* �Լ�ʱҪ���ϸ�����򵥻������� */
            if(delayProcessIO < 50)
            {
                delayProcessIO = 0;
            }
            else
            {
                delayProcessIO -= 50;       
            }
        }
        
		if(MMS_UPDATE_STATE() != MST_STATE_IDLE)    
        {   /* ������ */
			continue;
        }

        /* ================================================================== */
        /* ���ڼ̵��������жϵ�  ʱ���ʱ 
         *  */
		RelayTimerRun();
        
        /* ================================================================== */
        /* �� PIC24 ͨ�Ź�������: 
         * ��ѯ���Զ��л�����PIC24
         *  */
        processPic24Comm();
        
        /* ���ܴ��� IO ����������߼��ж�
         */
////        mainIOProcess();
        
        #if (SIGSYS_FUNCT_EN != 0)
        /* PEDC ���ź�ϵͳͨ�� 
         * processSigSysComm();
         */
        processSigSysComm();
        #endif
        
        #if (BASSYS_FUNCT_EN != 0)
        /* PEDC ��BASϵͳͨ�� 
         */
        processBASSysComm();
        #endif
        
        /* ===================  CAN ������ع��� ============================ */
        processCANComm();
    }
}

