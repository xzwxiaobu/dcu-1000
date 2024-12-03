
#ifndef SYS_TIMER_H
#define SYS_TIMER_H

#include "_cpu.h"
#include <plib.h>

/* ϵͳ��ʱ�����������ͣ���1ms��λ����
 * ���� int16u ������ 65535ms      = 65s
 * ���� int32u ������ 4294967295ms = 49.7 ��
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

//ϵͳʱ�ӽṹ������
#pragma pack(1)
typedef struct
{
    int16u year;    // �� (��16���Ʊ�ʾ���� 0x7E4 = 2020��)
    int8u  month;   // ��
    int8u  day;     // ��
    int8u  hour;    // ʱ
    int8u  min;     // ��
    int8u  second;  // ��
}tSysTime;

PUBLIC void initSysTimer(void);

/* ����ϵͳʱ�䣬����û���ж������Ч��Χ
 * ���� TRUE  ��ʾ���óɹ�
 * ���� FALSE ��ʾ����ʧ��
 *  */
PUBLIC BOOL setPedcSysTime(tSysTime * tim);

/* ȡPEDCϵͳʱ��
 *  */
PUBLIC BOOL getPedcSysTime(tSysTime * tim);


/* PEDC ϵͳʱ���Ƿ���Ч
 * ���� TRUE ��ʾ��Ч
 *  */
PUBLIC BOOL ifPEDCSysTimeValid(void);




/* ��ʱ���ж��е��ã�1 ms�ж�һ�Σ�����ϵͳ��ʱ
 */
PUBLIC void procSysTimer(void);

/* ����ǰϵͳ��ʱ��������ֵ��
 * ֵ�ĵ�λ�� ms */
PUBLIC tSYSTICK getSysTick(void);
#define getSysTimeDiff(t)       ((tSYSTICK)(getSysTick() - (t)))        /* ��ȡʱ��� */

/* ��鵱ǰϵͳ��ʱ���ı�־λ 
 * ����־λΪ�棬�򷵻� TRUE�����򷵻�FALSE
 * ע�⣺
 *   ��ȡ�ı�־λ���Զ�����������ó�������Ҫ����ʹ�ã������б���
 */
PUBLIC __inline BOOL chkSysTmrFlag(tEnmSysTimerFlag flag);


/* ���ϵͳ��ʱ�������б�־λ */
PUBLIC __inline void clrAllSysTmrFlag(void);


/* ����ʱ */
PUBLIC void startCntdownTimer(tSYSTICK dn_time);
PUBLIC tSYSTICK getCntdownTime(void);

/* ���ò����� RTCC
 * PUBLIC BOOL startRTCC(rtccDate dt, rtccTime tm);
 *  */


#endif  /* SYS_TIMER_H */

