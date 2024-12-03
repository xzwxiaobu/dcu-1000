/* =============================================================================
 * Project :   
 * Module  : MODBUS 寄存器
 * Date    : 2021-9-18
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _BASSYS_REG_H
#define _BASSYS_REG_H

#include "_cpu.h"

/* 
 * 2021-10-15
 *   QD4 PEDC -- BAS 采用 MODBUS 协议
 *  */

#define DCU_WAVEVALVES      1       /* 只能1或2，1个DCU接1个风阀 */
#if (DCU_WAVEVALVES == 1)
#define REG4_STS_SIZE       2
#else
#define REG4_STS_SIZE       4
#endif

typedef union
{
    struct
    {
        int8u bBASOpnCmd_sts    : 1;        /* 0 BAS开阀命令 */
        int8u bBASClsCmd_sts    : 1;        /* 1 BAS关阀命令 */
        int8u                   : 1;        /* 2 MMS开阀命令 */
        int8u                   : 1;        /* 3 MMS关阀命令 */
        int8u bPSCOpnCmd_sts    : 1;        /* 4 PSC 开阀命令 */
        int8u bPSCClsCmd_sts    : 1;        /* 5 PSC 关阀命令 */
        int8u bWindCtrlBy_PSC   : 1;        /* 6 风阀控制：0=BAS, 1=PSC */
        int8u                   : 1;        /* 7  */
    };
    
    int8u byte;
}tWindValveSysSts;

typedef union
{
    struct
    {
        int8u                   : 5;
        int8u bWindAllClsed     : 1;        /* 5 风阀总关 */
        int8u bWindAllOpned     : 1;        /* 6 风阀总开 */
        int8u                   : 1;        /* 7 风阀开关超时故障 */
    };
    
    int8u byte;
}tWindValveResult;


/* =========================================================
 * 通信数据结构
 * 风阀按整侧站台进行控制
 */
/* 只读寄存器 */
typedef union
{
    struct 
    {
        int16u allWVOpned       : 1;    /* 0: 0=非站台风阀总开到位状态, 1=站台风阀总开到位状态 */
        int16u allWVClsed       : 1;    /* 1: 0=非站台风阀总关到位状态, 1=站台风阀总关到位状态 */
        int16u BASCtrl          : 1;    /* 2: 0=非BAS远程控制模式，     1=BAS远程控制模式 */
        int16u LocalCtrl        : 1;    /* 3: 0=非本地控制模式，        1=本地控制模式 */
        int16u                  : 12;
    }bits;

    int16u wBASSts;                     /* 读写寄存器 0 */
}bas_reg03;
    

    /* 读写寄存器 */
typedef union
{
    struct 
    {
        int16u BAS_OpnCmd       : 1;    /* 0: 0=指令无效, 1=指令有效 */
        int16u BAS_ClsCmd       : 1;    /* 1: 0=指令无效, 1=指令有效 */
        int16u                  : 14;
    }bits;

    int16u wBASCmd;                     /* 读写寄存器 0 */
}bas_reg10;
    
typedef struct
{
    bas_reg03   reg03;
    bas_reg10   reg10;
}tBASRegister;

///* 风阀状态寄存器 */
//typedef struct
//{
//    /* 风阀开启状态标记 
//     * 0 = 风阀未在打开过程中
//     * 1 = 风阀打开过程中
//     */
//    int16u  rWindStateOpnSts[REG4_STS_SIZE];        /* 0 */
//
//    /* 风阀关闭状态标记 
//     * 0 = 风阀未关闭到位
//     * 1 = 风阀关闭到位
//     */
//    int16u  rWindStateClsSts[REG4_STS_SIZE];        /* 1 */
//
//    /* 风阀开/关故障
//     * 0 = 正常
//     * 1 = 故障
//     */
//    int16u  rWindFaultSts[REG4_STS_SIZE];           /* 2 */
//    
//#if 0
//    /* 风阀关闭过程中标记 
//     * 0 = 风阀未在关闭过程中
//     * 1 = 风阀关闭过程中
//     */
//    int16u  rWindStateClsing[REG4_STS_SIZE];        /* 3 */
//
//    /* 风阀打开到位标记 
//     * 0 = 风阀未打开到位
//     * 1 = 风阀打开到位
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



