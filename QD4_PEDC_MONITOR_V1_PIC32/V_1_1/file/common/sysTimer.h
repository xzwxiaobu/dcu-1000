
#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include "_cpu.h"
#include <plib.h>

/* 系统定时器计数器类型，按1ms单位计数
 * 对于 int16u 最大计数 65535ms      = 65s
 * 对于 int32u 最大计数 4294967295ms = 49.7 天
 */
typedef int32u          tSYSTICK;

typedef int16u          tSysTimeFlag;

typedef enum
{
    b1ms         = ( 1 << 0  ),   /*bit 0  */
    b2ms         = ( 1 << 1  ),   /*bit 1  */
    b4ms         = ( 1 << 2  ),   /*bit 2  */
    b8ms         = ( 1 << 3  ),   /*bit 3  */
    b16ms        = ( 1 << 4  ),   /*bit 4  */
    b32ms        = ( 1 << 5  ),   /*bit 5  */
    b64ms        = ( 1 << 6  ),   /*bit 6  */
    b128ms       = ( 1 << 7  ),   /*bit 7  */
    b256ms       = ( 1 << 8  ),   /*bit 8  */
    b512ms       = ( 1 << 9  ),   /*bit 9  */
    b1024ms      = ( 1 << 10 ),   /*bit 10 */
    b2048ms      = ( 1 << 11 ),   /*bit 11 */
    bIO_512ms    = ( 1 << 12 ),  /*bit 12 */
    bSysTime8ms  = ( 1 << 13 ),   /*bit 13 */
}tEnmSysTimerFlag;

//系统时钟结构体类型
#pragma pack(1)
typedef struct
{
    int16u year;    // 年 (用16进制表示，如 0x7E4 = 2020年)
    int8u  month;   // 月
    int8u  day;     // 日
    int8u  hour;    // 时
    int8u  min;     // 分
    int8u  second;  // 秒
}tSysTime;

PUBLIC void initSysTimer(void);

/* 设置系统时间，这里没有判断年的有效范围
 * 返回 TRUE  表示设置成功
 * 返回 FALSE 表示设置失败
 *  */
PUBLIC BOOL setPedcSysTime(tSysTime * tim);

/* 取PEDC系统时间
 *  */
PUBLIC BOOL getPedcSysTime(tSysTime * tim);


/* PEDC 系统时钟是否有效
 * 返回 TRUE 表示有效
 *  */
PUBLIC BOOL ifPEDCSysTimeValid(void);




/* 定时器中断中调用，1 ms中断一次，用于系统计时
 */
PUBLIC void procSysTimer(void);

/* 读当前系统定时器计数器值，
 * 值的单位是 ms */
PUBLIC tSYSTICK getSysTick(void);
#define getSysTimeDiff(t)       ((tSYSTICK)(getSysTick() - (t)))        /* 获取时间差 */

/* 检查当前系统定时器的标志位 
 * 若标志位为真，则返回 TRUE，否则返回FALSE
 * 注意：
 *   读取的标志位会自动被清除，调用程序若需要继续使用，需自行保存
 */
PUBLIC __inline BOOL chkSysTmrFlag(tEnmSysTimerFlag flag);


/* 清除系统定时器的所有标志位 */
PUBLIC __inline void clrAllSysTmrFlag(void);


/* 倒计时 */
PUBLIC void startCntdownTimer(tSYSTICK dn_time);
PUBLIC tSYSTICK getCntdownTime(void);

/* 设置并启动 RTCC
 * PUBLIC BOOL startRTCC(rtccDate dt, rtccTime tm);
 *  */


#endif  /* SYS_TIMER_H */

