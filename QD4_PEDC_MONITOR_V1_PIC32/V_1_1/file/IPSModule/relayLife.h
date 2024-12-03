/* 
 * File:   relayLife.h
 * Author: LLY
 *
 * Created on 2021年9月16日, 上午9:04
 */

#ifndef RELAYLIFE_H
#define	RELAYLIFE_H

#ifdef	__cplusplus
extern "C"
{
#endif

    
#include    "_cpu.h"
    
#define RELAY_NUM               21          /* 参见 PEDC_SZ16_V1.pdf */
#define RELAY_STS_LASTTIME_MAX  30000       /* 上一次状态的持续时间最大值 30000 ms */
    
#define DET_TYPE_NO             0       /* 常开点 */
#define DET_TYPE_NC             1       /* 常闭点 */
#define DET_STABLE_CNT          4       /* 检测认为稳定变化的次数 */
#define DET_TIME_WINDOW         3000    /* 单次检测时间窗 */
    
#if     (DET_STABLE_CNT == 1)   
#define DET_STABLE_MASK         0x01
#elif   (DET_STABLE_CNT == 2)   
#define DET_STABLE_MASK         0x03
#elif   (DET_STABLE_CNT == 3)   
#define DET_STABLE_MASK         0x07
#elif   (DET_STABLE_CNT == 4)   
#define DET_STABLE_MASK         0x0F
#elif   (DET_STABLE_CNT == 5)   
#define DET_STABLE_MASK         0x1F
#elif   (DET_STABLE_CNT == 6)   
#define DET_STABLE_MASK         0x3F
#elif   (DET_STABLE_CNT == 7)   
#define DET_STABLE_MASK         0x7F
#elif   (DET_STABLE_CNT == 8)   
#define DET_STABLE_MASK         0xFF
#else
#error "DET_STABLE_CNT must be 1 ~ 8"
#endif

typedef struct
{
    int8u   RelayNo;        /* 继电器编号                   */
    struct
    {
        int8u DetType: 1;   /* 0=常开点，1=常闭点           */
        int8u PrevSts: 1;   /* 上一次触点状态，即监视点电平 */
        int8u NowSts : 1;   /* 当前触点状态    即监视点电平 */
        int8u        : 5;   
    };
    
    int8u IOIndex;          /* 在输入IO状态数据中的索引字节的偏移   */
    int8u IOBit;            /* 在输入IO状态数据中的索引字节的位     */
    int16u  StsSeries;      /* 状态序列 */
    int16u  Counter;        /* 在指定时间段内的状态变化次数 */
}tRelayStatus;
    
    
/* 每个继电器工作寿命的记录结构 */
typedef struct
{
    int8u   cs;         /* 校验信息（第2~8字节累加和取反） */
    int8u   relayNo;    /* 继电器编号 */
    int8u   rsv1;       /* 保留 = 0 */
    int8u   rsv2;       /* 保留 = 0 */
    int32u  workCnt;    /* 动作次数计数器 */
}tRelayLife;

/* 继电器记录的信息头 */
typedef struct
{
    int8u   header1;        /* 'F' */
    int8u   header2;        /* 'D' */
    int8u   header3;        /* 'Z' */
    int8u   header4;        /* 'C' */
    int16u  relayNum;       /* 继电器总数 */
    int8u   random;         /* 头信息的随机数   */
    int8u   cs;             /* 头信息的校验信息（前7个字节累加和取反） */
}tRelayRecHeader;
    
/* 继电器记录的信息尾 */
typedef struct
{
    int8u   end1;           /* 'D' */
    int8u   end2;           /* 'E' */
    int8u   end3;           /* 'N' */
    int8u   end4;           /* 'D' */
    int16u  relayNum;       /* 继电器总数 */
    int8u   random;         /* 尾信息的随机数 = 头信息随机数 + 1   */
    int8u   cs;             /* 尾信息的校验信息（前7个字节累加和取反） */
}tRelayRecTail;
    
typedef struct
{
    tRelayRecHeader header;
    tRelayLife      relays[RELAY_NUM];
    tRelayRecTail   tail;
}tRelayRecord;

#define REC_HEADER1     'F'
#define REC_HEADER2     'D'
#define REC_HEADER3     'Z'
#define REC_HEADER4     'C'

#define REC_END1        'D'
#define REC_END2        'E'
#define REC_END3        'N'
#define REC_END4        'D'


#ifdef	__cplusplus
}
#endif

#endif	/* RELAYLIFE_H */

