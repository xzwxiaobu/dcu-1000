#include "sysTimer.h"
#include "include.h"

volatile tSYSTICK sysTickCount;              /* 系统定时器计数器 */

volatile PRIVATE int16u         wTimeCal;
volatile PRIVATE tSysTimeFlag   wTimeFlag;      /* 系统定时器标志位 */

PRIVATE BOOL sysTimeValid;      //1 有效的系统时间；（FALSE：第一次上电后没有接收到MMS的系统时间）

PUBLIC void initSysTimer(void)
{
    wTimeCal        = 0;
    wTimeFlag       = 0;
    sysTimeValid    = FALSE;
}

/* =========================================================
 * 倒计时计数器
 * */
PRIVATE volatile tSYSTICK cntDownTmr;
PUBLIC void startCntdownTimer(tSYSTICK dn_time)
{
    cntDownTmr = dn_time;
}


PUBLIC tSYSTICK getCntdownTime(void)
{
    return cntDownTmr;
}

/* =========================================================
 */
/* 读当前系统定时器计数器值，值的单位是 ms */
PUBLIC tSYSTICK getSysTick(void)
{
    return sysTickCount;
}

/* 检查当前系统定时器的标志位 
 * 若标志位为真，则返回 TRUE，否则返回FALSE
 * 注意：
 *   读取的标志位会自动被清除，调用程序若需要继续使用，需自行保存
 */
PUBLIC __inline BOOL chkSysTmrFlag(tEnmSysTimerFlag flag)
{
    BOOL ret;
    ret = wTimeFlag & flag;
    wTimeFlag &= ~flag;
    return ret;
}

/* 清除系统定时器的所有标志位 */
PUBLIC __inline void clrAllSysTmrFlag(void)
{
    wTimeFlag = 0;
}

/* 设置系统定时器的指定标志位 */
PRIVATE __inline void setSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag |= flag;
}

/* 清除系统定时器的指定标志位 */
PRIVATE __inline void clrSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag &= ~flag;
}

/* 翻转系统定时器的指定标志位 */
PRIVATE __inline void togSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag ^= flag;
}

/* =========================================================
 * 定时器中断中调用，1 ms中断一次，用于系统计时
 *
 */
PRIVATE void runSysTime(void);
PUBLIC void procSysTimer(void)
{
    int8u i;
    mT2ClearIntFlag();

    setSysTmrFlag(b1ms);
    
    runSysTime();
    
    sysTickCount++;
    if(cntDownTmr > 0)
        cntDownTmr--;

    wTimeCal ++;
    if((wTimeCal & 0x0001) == 0)
    {
        setSysTmrFlag(b2ms);

        if((wTimeCal % (4)) == 0)
            setSysTmrFlag(b4ms);

        if((wTimeCal % (8)) == 0)
        {   
            setSysTmrFlag(b8ms);
            setSysTmrFlag(bSysTime8ms);
        }

        if((wTimeCal % (16)) == 0)
        {
            setSysTmrFlag(b16ms);
        }
        
        if((wTimeCal % (32)) == 0)
            setSysTmrFlag(b32ms);

        if((wTimeCal % (64)) == 0)
        {   
            setSysTmrFlag(b64ms);
        }   

        if((wTimeCal % (128)) == 0)
            setSysTmrFlag(b128ms);

        if((wTimeCal % (256)) == 0)
        {
            togSysTmrFlag(b256ms);
        }

        if((wTimeCal % (512)) == 0)
        {
            setSysTmrFlag(b512ms);
            togSysTmrFlag(bIO_512ms);
        }

        if((wTimeCal % (1024)) == 0)
        {   
            setSysTmrFlag(b1024ms);
        }

        if((wTimeCal % (2048)) == 0)
        {
            setSysTmrFlag(b2048ms);
        }   
    }
}

/* =========================================================
 * 定时器中断函数入口
 * 中断时间间隔 = 1 ms
 * [IPLn[SRS|SOFT|AUTO]: 测试用 IPL5SRS 出现进入中断后无法执行程序
 */
//void __attribute__((section (".SEG_Time2HANDLER"))) __attribute__ ((interrupt(ipl5))) __attribute__ ((vector(_TIMER_2_VECTOR))) Timer2Handler(void)
void __section__(".SEG_Time2HANDLER") __ISR(_TIMER_2_VECTOR, IPL5SOFT) Timer2Handler(void)       /* interrupt() */
{
    mT2ClearIntFlag();
    //b1msF = 1;

	SysTick1Ms();
    procSysTimer();
}


/* =============================================================================
 * 新增系统时间运行功能
 *  */
volatile tSysTime pedcSysTime = 
{
    .year   = 2021,
    .month  = 1,
    .day    = 1,
    .hour   = 0,
    .min    = 0,
    .second = 0,
};


/* 根据当前的年和月获取最大日期
 * 自动计算闰年 */
                           //月份  0  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12
PRIVATE int8u maxDayTab[] = {      0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,  30,  31 };
PRIVATE int8u getMaxDay(int16u year, int8u month)
{
    if(month > 12)
        return 0;
        
    int8u maxDay = maxDayTab[month];
    
    //能被400整除的是闰年
    //能被4整除且不能被100整除的为闰年
    if(((year % 400) == 0) || (((year % 4) == 0) && (year % 100) != 0))
    {
        //这里是闰年
        if(month == 2)
        {
            maxDay ++;
        }
    }
    
    return maxDay;
}

/* 取PEDC系统时间
 *  */
PUBLIC BOOL getPedcSysTime(tSysTime * tim)
{
    if(tim != NULL)
    {
        *tim = pedcSysTime;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* 设置系统时间
 * 返回 TRUE  表示设置成功
 * 返回 FALSE 表示设置失败
 *  */
PUBLIC BOOL setPedcSysTime(tSysTime * tim)
{
    BOOL ret = FALSE; 
    if(tim != NULL)
    {
        if((tim->hour   < 24) && (tim->min   <  60)  && (tim->second < 60) && 
           ((tim->year > 1900 ) && (tim->year <= 2099)) &&
           ((tim->month > 0 ) && (tim->month <= 12)) &&
           ((tim->day   > 0 ) && (tim->day   <= getMaxDay(tim->year, tim->month))))
        {
            pedcSysTime = *tim;

            sysTimeValid = TRUE;
            ret = TRUE;
        }
    }
    
    return ret;
}

/* PEDC 系统时钟是否有效
 * 返回 TRUE 表示有效
 *  */
PUBLIC BOOL ifPEDCSysTimeValid(void)
{
    return sysTimeValid;
}

/* 系统时间按秒增加 
 * 
 */
#define ONE_SECOND_MS       1000
PRIVATE void runSysTime(void)
{
    static int cnt1000ms = 0;
    
    cnt1000ms++;
    if(cnt1000ms >= ONE_SECOND_MS)
    {
        cnt1000ms -= ONE_SECOND_MS;
        
        pedcSysTime.second ++;
        if(pedcSysTime.second >= 60)
        {
            pedcSysTime.second = 0;
            
            pedcSysTime.min++;
            if(pedcSysTime.min >= 60)
            {
                pedcSysTime.min = 0;
                
                pedcSysTime.hour ++;
                if(pedcSysTime.hour >= 24)
                {
                    pedcSysTime.hour = 0;
                    
                    pedcSysTime.day ++;
                    if(pedcSysTime.day > getMaxDay(pedcSysTime.year, pedcSysTime.month))
                    {
                        pedcSysTime.day = 1;
                        
                        pedcSysTime.month ++;
                        if(pedcSysTime.month > 12)
                        {
                            pedcSysTime.month = 1;
                            
                            pedcSysTime.year  ++;
                        }
                    }
                }
            }
        }
    }
}




#if 0
/* =========================================================
 * 启动系统 RTCC  (本函数无法使用)
 * 因硬件没有外置 RTC 时钟源且PIC32MX795F512H不支持低频时钟作为RTC的时钟源，
 * 所以芯片的RTCC功能无法使用，需要软件实现RTCC
 * 
 * 输入参数
 *  dt : 系统日期
 *  tm : 系统时间
 * 返回
 *  TRUE  表示成功
 *  FALSE 表示失败
 * */
PUBLIC BOOL startRTCC(rtccDate dt, rtccTime tm)
{
    rtccTime tm1;
    rtccDate dt1;

    /* init the RTCC */
    RtccInit();
    
    /* wait for the SOSC to be actually running and RTCC to have its clock source,
     * could wait here at most 32ms
     * 
     * while(RtccGetClkStat()!=RTCC_CLK_ON);
     */

    // when using the RtccSetTimeDate() function, the write operation is enabled if needed and then restored to the initial value
    // so that we don't have to worry about calling RtccWrEnable()/RtccWrDisable() functions

    /* setting the current date 
     * time is MSb: hour, min, sec, rsvd=0.
     * date is MSb: year, mon, mday, wday.
     *  RtccSetTimeDate(0x10073000, 0x07011602);  
     */
    RtccSetTimeDate(tm.l, dt.l);

    /* 
     * check that the time and date
     * 
     * read the time and date by 
     *   RtccGetTimeDate(&tm1, &dt1);
     *     or
     *   tm1.l=RtccGetTime();
     *   dt1.l=RtccGetDate();
     * */
    RtccGetTimeDate(&tm1, &dt1);

    if(tm.hour!=tm1.hour ||tm.min!=tm1.min)
    {
        return FALSE;
    }
    if(dt.l!=dt1.l)
    {
        return FALSE;
    }

    /* the RTCC clock is up and running, to start from fresh
     * set time, date and calibration in a single operation */
    RtccOpen(tm.l, dt.l, 0);

    /* another way to see the RTCC is tunning: check the SYNC bit
    while(RtccGetSync());   // wait sync to be low
    while(!RtccGetSync());  // wait to be high
    while(RtccGetSync());   // wait sync to be low again
     * */


    // adjust the RTCC timing
    //RtccSetCalibration(200);    // value to calibrate with at each minute

//    // enabling the RTCC output pin
//    RtccSelectPulseOutput(1);       // select the seconds clock pulse as the function of the RTCC output pin
//    RtccSelectPulseOutput(0);       // select the alarm pulse as the function of the RTCC output pin
//    RtccOutputEnable(1);            // enable the Output pin of the RTCC

    return TRUE;
}
#endif

