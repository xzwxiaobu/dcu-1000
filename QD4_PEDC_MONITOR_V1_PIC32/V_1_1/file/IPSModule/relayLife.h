/* 
 * File:   relayLife.h
 * Author: LLY
 *
 * Created on 2021��9��16��, ����9:04
 */

#ifndef RELAYLIFE_H
#define	RELAYLIFE_H

#ifdef	__cplusplus
extern "C"
{
#endif

    
#include    "_cpu.h"
    
#define RELAY_NUM               21          /* �μ� PEDC_SZ16_V1.pdf */
#define RELAY_STS_LASTTIME_MAX  30000       /* ��һ��״̬�ĳ���ʱ�����ֵ 30000 ms */
    
#define DET_TYPE_NO             0       /* ������ */
#define DET_TYPE_NC             1       /* ���յ� */
#define DET_STABLE_CNT          4       /* �����Ϊ�ȶ��仯�Ĵ��� */
#define DET_TIME_WINDOW         3000    /* ���μ��ʱ�䴰 */
    
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
    int8u   RelayNo;        /* �̵������                   */
    struct
    {
        int8u DetType: 1;   /* 0=�����㣬1=���յ�           */
        int8u PrevSts: 1;   /* ��һ�δ���״̬�������ӵ��ƽ */
        int8u NowSts : 1;   /* ��ǰ����״̬    �����ӵ��ƽ */
        int8u        : 5;   
    };
    
    int8u IOIndex;          /* ������IO״̬�����е������ֽڵ�ƫ��   */
    int8u IOBit;            /* ������IO״̬�����е������ֽڵ�λ     */
    int16u  StsSeries;      /* ״̬���� */
    int16u  Counter;        /* ��ָ��ʱ����ڵ�״̬�仯���� */
}tRelayStatus;
    
    
/* ÿ���̵������������ļ�¼�ṹ */
typedef struct
{
    int8u   cs;         /* У����Ϣ����2~8�ֽ��ۼӺ�ȡ���� */
    int8u   relayNo;    /* �̵������ */
    int8u   rsv1;       /* ���� = 0 */
    int8u   rsv2;       /* ���� = 0 */
    int32u  workCnt;    /* �������������� */
}tRelayLife;

/* �̵�����¼����Ϣͷ */
typedef struct
{
    int8u   header1;        /* 'F' */
    int8u   header2;        /* 'D' */
    int8u   header3;        /* 'Z' */
    int8u   header4;        /* 'C' */
    int16u  relayNum;       /* �̵������� */
    int8u   random;         /* ͷ��Ϣ�������   */
    int8u   cs;             /* ͷ��Ϣ��У����Ϣ��ǰ7���ֽ��ۼӺ�ȡ���� */
}tRelayRecHeader;
    
/* �̵�����¼����Ϣβ */
typedef struct
{
    int8u   end1;           /* 'D' */
    int8u   end2;           /* 'E' */
    int8u   end3;           /* 'N' */
    int8u   end4;           /* 'D' */
    int16u  relayNum;       /* �̵������� */
    int8u   random;         /* β��Ϣ������� = ͷ��Ϣ����� + 1   */
    int8u   cs;             /* β��Ϣ��У����Ϣ��ǰ7���ֽ��ۼӺ�ȡ���� */
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

