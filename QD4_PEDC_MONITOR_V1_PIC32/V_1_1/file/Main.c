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
 *              系统：MONITOR -- 安全回路切换板 |-- 逻辑板1(UA1 + UB1)
 *                                              |-- 逻辑板2(UA1 + UB1)
 *              1. 支持在线升级其它板，支持 PIC24 IAP在线升级
 *                 注意 逻辑板有 PIC32UA1 PIC32UB1 2个MCU需要更新
 *                      逻辑板UA1芯片本项目暂定不用
 *              2. 逻辑板1 2是冗余方式，MONITOR发命令给安全回路板进行切换
 *              3. 2022-6 新增支持批量升级DCU功能（需要MMS和DCU配合）
 *              4. 2023-8-21 新增 DCU里程修改功能 0x23 0x28
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

/* 信号系统功能设置 
 * 0 = disable， else = enable
 */
#define SIGSYS_FUNCT_EN     0

/* BAS系统功能设置 
 * 0 = disable， else = enable
 */
#define BASSYS_FUNCT_EN     0

#if (USR_WDT_ON != 0)
PRIVATE volatile BOOL wdtReset;
#endif

/* 上电后等待一段时间再处理 IO 数据、然后上传给MMS
 * 避免开始处理时，收集的IO数据准确或不完整，造成有报继电器故障之类的现象
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
    /* 定时检查串口通信错误状态 */
    static tSYSTICK     tickChkUartErr;
    
	int i;
    tSYSTICK flashLEDTick;
    tSYSTICK tickNow;
    
	SYSTEMConfig(GetSystemClock(), SYS_CFG_WAIT_STATES | SYS_CFG_PCACHE);

	CpuInit();                              /* cpu initial */
    initPic32GPIOs();                       /* 初始化 GPIO */

	asm volatile("di");                     /*  Disable all interrupts          */
	reset_Read_IbpAlarmX();                 /* 上电读取 IBP ALARM               */
	reset_Read_IbpAlarmX();                 /* 上电读取 IBP ALARM               */
	Read_DCUsetting_From_Flash();           /* 上电读取 PEDC Hardware settings  */
    /* 用于风阀全开或全关的参数 */
    setWVOpnClsPara(tagDownIbpAlarmParameter.uRead_WVTotalNum, \
                    tagDownIbpAlarmParameter.uRead_WVNum_Opn,  \
                    tagDownIbpAlarmParameter.uRead_WVNum_Cls);

    /* 初始化CAN */
	ApiMainInit();

	CpuInit();                              /*cpu initial 应在 ApiMainInit()之后*/

    /* 初始化存储器 */
	RamIni();           
    initSysTimer();                    
    resetSlaveInfo();
    
    /* 读取PEDC ID拨码 */
	scanPEDC_ID();	                        
        
	/* 初始化：MMS 通信 */
    initMMSModule();
    
    /* 初始化：IBP PSL SIG ... 板通信 */
   //  initIPSModule();
    
    /* 初始化：PIC24 板通信 */
    initPic24Module();
    
    #if (SIGSYS_FUNCT_EN != 0)
    /* 初始化：信号系统通信 
     * initSigSysModule();
     */
    initSigSysModule();
    #endif
    
    #if (BASSYS_FUNCT_EN != 0)
    /* 初始化：BAS系统通信 */
    initBASSysModule();
    #endif
    
    /* enable interrupts */
	INTEnableInterrupts();
    
    #if (USR_WDT_ON != 0)   
    /* 获取 WDT 复位信息，
     * 使能 WDT 
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
    
    /* 上电默认选择 逻辑板1 */
    setWorkLOGID(COMMOBJ_LOG1);
    
    sf_flash_AccRcv = 0;    /* 累加接收的安全回路闪断次数 */                  
    sf_flash_toMMS  = 0;    /* 发送给MMS的，安全回路闪断次数 */
    
    delayProcessIO = DELAY_PROCESSIO_MS;
    /*=======================================
     * 主循环
     */
	while(1)
	{
        /* 重置 WDT 计时 */
        #if (USR_WDT_ON != 0)
        ClearWDT();
        #endif
        
        /* 串口通信数据处理
         * 1. 处理软件接收缓存中的数据
         * 2. DMA 完成传输发送数据以后的处理 */
        #ifdef     UART_RX_SOFTBUFF
        processAllUartRxDataBuff();
        #endif
        #ifdef     UART_TX_BY_DMA
        checkUartTxDMAFinished();
        #endif
        
        /* 在线升级处理程序
         * CAN 总线通信处理程序 
         */
		ApiMainProcess();

        /* 与 MMS 通信管理
         *  */
        MMS_AppModule();

        /* 管理与子板的通信
         * 以及在线升级
         *  */
        #if (SHOW_PIC24_BY_UART_IPS == 0)
        processIPSComm();
        #endif
        
        /* 定时任务
         * 闪烁 LED，如与 MMS 链接正常则 1HZ 闪烁，否则 2HZ 闪烁
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
        
        /* 定时检测串口错误 */
        if((tSYSTICK)(tickNow - tickChkUartErr) >= 50)
        {
            tickChkUartErr = tickNow;
            checkAllUartErrorSta();
            
            /* 对计时要求不严格，这里简单化处理了 */
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
        {   /* 升级中 */
			continue;
        }

        /* ================================================================== */
        /* 用于继电器错误判断的  时间计时 
         *  */
		RelayTimerRun();
        
        /* ================================================================== */
        /* 与 PIC24 通信管理，包括: 
         * 轮询、自动切换主从PIC24
         *  */
        processPic24Comm();
        
        /* 汇总处理 IO 输入输出，逻辑判断
         */
////        mainIOProcess();
        
        #if (SIGSYS_FUNCT_EN != 0)
        /* PEDC 与信号系统通信 
         * processSigSysComm();
         */
        processSigSysComm();
        #endif
        
        #if (BASSYS_FUNCT_EN != 0)
        /* PEDC 与BAS系统通信 
         */
        processBASSysComm();
        #endif
        
        /* ===================  CAN 总线相关功能 ============================ */
        processCANComm();
    }
}

