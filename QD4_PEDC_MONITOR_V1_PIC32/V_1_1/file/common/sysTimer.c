#include "sysTimer.h"
#include "include.h"

volatile tSYSTICK sysTickCount;              /* ϵͳ��ʱ�������� */

volatile PRIVATE int16u         wTimeCal;
volatile PRIVATE tSysTimeFlag   wTimeFlag;      /* ϵͳ��ʱ����־λ */

PRIVATE BOOL sysTimeValid;      //1 ��Ч��ϵͳʱ�䣻��FALSE����һ���ϵ��û�н��յ�MMS��ϵͳʱ�䣩

PUBLIC void initSysTimer(void)
{
    wTimeCal        = 0;
    wTimeFlag       = 0;
    sysTimeValid    = FALSE;
}

/* =========================================================
 * ����ʱ������
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
/* ����ǰϵͳ��ʱ��������ֵ��ֵ�ĵ�λ�� ms */
PUBLIC tSYSTICK getSysTick(void)
{
    return sysTickCount;
}

/* ��鵱ǰϵͳ��ʱ���ı�־λ 
 * ����־λΪ�棬�򷵻� TRUE�����򷵻�FALSE
 * ע�⣺
 *   ��ȡ�ı�־λ���Զ�����������ó�������Ҫ����ʹ�ã������б���
 */
PUBLIC __inline BOOL chkSysTmrFlag(tEnmSysTimerFlag flag)
{
    BOOL ret;
    ret = wTimeFlag & flag;
    wTimeFlag &= ~flag;
    return ret;
}

/* ���ϵͳ��ʱ�������б�־λ */
PUBLIC __inline void clrAllSysTmrFlag(void)
{
    wTimeFlag = 0;
}

/* ����ϵͳ��ʱ����ָ����־λ */
PRIVATE __inline void setSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag |= flag;
}

/* ���ϵͳ��ʱ����ָ����־λ */
PRIVATE __inline void clrSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag &= ~flag;
}

/* ��תϵͳ��ʱ����ָ����־λ */
PRIVATE __inline void togSysTmrFlag(tEnmSysTimerFlag flag)
{
    wTimeFlag ^= flag;
}

/* =========================================================
 * ��ʱ���ж��е��ã�1 ms�ж�һ�Σ�����ϵͳ��ʱ
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
 * ��ʱ���жϺ������
 * �ж�ʱ���� = 1 ms
 * [IPLn[SRS|SOFT|AUTO]: ������ IPL5SRS ���ֽ����жϺ��޷�ִ�г���
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
 * ����ϵͳʱ�����й���
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


/* ���ݵ�ǰ������»�ȡ�������
 * �Զ��������� */
                           //�·�  0  1,  2,  3,  4,  5,  6,  7,  8,  9,  10,  11,  12
PRIVATE int8u maxDayTab[] = {      0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,  30,  31 };
PRIVATE int8u getMaxDay(int16u year, int8u month)
{
    if(month > 12)
        return 0;
        
    int8u maxDay = maxDayTab[month];
    
    //�ܱ�400������������
    //�ܱ�4�����Ҳ��ܱ�100������Ϊ����
    if(((year % 400) == 0) || (((year % 4) == 0) && (year % 100) != 0))
    {
        //����������
        if(month == 2)
        {
            maxDay ++;
        }
    }
    
    return maxDay;
}

/* ȡPEDCϵͳʱ��
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

/* ����ϵͳʱ��
 * ���� TRUE  ��ʾ���óɹ�
 * ���� FALSE ��ʾ����ʧ��
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

/* PEDC ϵͳʱ���Ƿ���Ч
 * ���� TRUE ��ʾ��Ч
 *  */
PUBLIC BOOL ifPEDCSysTimeValid(void)
{
    return sysTimeValid;
}

/* ϵͳʱ�䰴������ 
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
 * ����ϵͳ RTCC  (�������޷�ʹ��)
 * ��Ӳ��û������ RTC ʱ��Դ��PIC32MX795F512H��֧�ֵ�Ƶʱ����ΪRTC��ʱ��Դ��
 * ����оƬ��RTCC�����޷�ʹ�ã���Ҫ���ʵ��RTCC
 * 
 * �������
 *  dt : ϵͳ����
 *  tm : ϵͳʱ��
 * ����
 *  TRUE  ��ʾ�ɹ�
 *  FALSE ��ʾʧ��
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

