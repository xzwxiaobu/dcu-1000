/* =============================================================================
 * Project :   
 * Module  : MODBUS �Ĵ���
 * Date    : 2021-9-18
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _BASSYS_REG_H
#define _BASSYS_REG_H

#include "_cpu.h"

/* 
 * 2021-10-15
 *   QD4 PEDC -- BAS ���� MODBUS Э��
 *  */

#define DCU_WAVEVALVES      1       /* ֻ��1��2��1��DCU��1���緧 */
#if (DCU_WAVEVALVES == 1)
#define REG4_STS_SIZE       2
#else
#define REG4_STS_SIZE       4
#endif

typedef union
{
    struct
    {
        int8u bBASOpnCmd_sts    : 1;        /* 0 BAS�������� */
        int8u bBASClsCmd_sts    : 1;        /* 1 BAS�ط����� */
        int8u                   : 1;        /* 2 MMS�������� */
        int8u                   : 1;        /* 3 MMS�ط����� */
        int8u bPSCOpnCmd_sts    : 1;        /* 4 PSC �������� */
        int8u bPSCClsCmd_sts    : 1;        /* 5 PSC �ط����� */
        int8u bWindCtrlBy_PSC   : 1;        /* 6 �緧���ƣ�0=BAS, 1=PSC */
        int8u                   : 1;        /* 7  */
    };
    
    int8u byte;
}tWindValveSysSts;

typedef union
{
    struct
    {
        int8u                   : 5;
        int8u bWindAllClsed     : 1;        /* 5 �緧�ܹ� */
        int8u bWindAllOpned     : 1;        /* 6 �緧�ܿ� */
        int8u                   : 1;        /* 7 �緧���س�ʱ���� */
    };
    
    int8u byte;
}tWindValveResult;


/* =========================================================
 * ͨ�����ݽṹ
 * �緧������վ̨���п���
 */
/* ֻ���Ĵ��� */
typedef union
{
    struct 
    {
        int16u allWVOpned       : 1;    /* 0: 0=��վ̨�緧�ܿ���λ״̬, 1=վ̨�緧�ܿ���λ״̬ */
        int16u allWVClsed       : 1;    /* 1: 0=��վ̨�緧�ܹص�λ״̬, 1=վ̨�緧�ܹص�λ״̬ */
        int16u BASCtrl          : 1;    /* 2: 0=��BASԶ�̿���ģʽ��     1=BASԶ�̿���ģʽ */
        int16u LocalCtrl        : 1;    /* 3: 0=�Ǳ��ؿ���ģʽ��        1=���ؿ���ģʽ */
        int16u                  : 12;
    }bits;

    int16u wBASSts;                     /* ��д�Ĵ��� 0 */
}bas_reg03;
    

    /* ��д�Ĵ��� */
typedef union
{
    struct 
    {
        int16u BAS_OpnCmd       : 1;    /* 0: 0=ָ����Ч, 1=ָ����Ч */
        int16u BAS_ClsCmd       : 1;    /* 1: 0=ָ����Ч, 1=ָ����Ч */
        int16u                  : 14;
    }bits;

    int16u wBASCmd;                     /* ��д�Ĵ��� 0 */
}bas_reg10;
    
typedef struct
{
    bas_reg03   reg03;
    bas_reg10   reg10;
}tBASRegister;

///* �緧״̬�Ĵ��� */
//typedef struct
//{
//    /* �緧����״̬��� 
//     * 0 = �緧δ�ڴ򿪹�����
//     * 1 = �緧�򿪹�����
//     */
//    int16u  rWindStateOpnSts[REG4_STS_SIZE];        /* 0 */
//
//    /* �緧�ر�״̬��� 
//     * 0 = �緧δ�رյ�λ
//     * 1 = �緧�رյ�λ
//     */
//    int16u  rWindStateClsSts[REG4_STS_SIZE];        /* 1 */
//
//    /* �緧��/�ع���
//     * 0 = ����
//     * 1 = ����
//     */
//    int16u  rWindFaultSts[REG4_STS_SIZE];           /* 2 */
//    
//#if 0
//    /* �緧�رչ����б�� 
//     * 0 = �緧δ�ڹرչ�����
//     * 1 = �緧�رչ�����
//     */
//    int16u  rWindStateClsing[REG4_STS_SIZE];        /* 3 */
//
//    /* �緧�򿪵�λ��� 
//     * 0 = �緧δ�򿪵�λ
//     * 1 = �緧�򿪵�λ
//     */
//    int16u  rWindStateOpning[REG4_STS_SIZE];        /* 4 */
//#endif
//}tWVStatus;

PUBLIC tWindValveSysSts * getWindValveSysSts(void);
PUBLIC tWindValveResult * getWindValveResult(void);
PUBLIC tBASRegister * getBASRegister(void);

extern void GetReplyBASMessage(int8u *pBuf);
extern int8u findFirstBit(pINT8U p, int8u Len);
extern int8u findLastBit(pINT8U p, int8u Len);
extern int8u calcSetting1Num(pINT8U p, int8u Len);
extern void setWVOpnClsPara(int8u total, int8u opnNum, int8u clsNum);

BOOL ifTestBASRegValue(void);
void decTestBASRegTime(int16u ms);
void setTestBASRegValue(int8u *pReg);

//**********************************************
#endif  //_BASSYS_REG_H



