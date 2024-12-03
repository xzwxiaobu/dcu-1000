
#ifndef USR_CRC_H
#define USR_CRC_H

#include "_cpu.h"

#define DFT_CRC_INI     0x0000

/***************************************************************************
 * 功能描述:    数据校验算法
 * 应    用:    PEDC-MONITOR 与 MMS 通信数据校验
 */
/* 数据校验
 */
PUBLIC int8u checksum(int8u * ptr, int16u len);

/* 数据校验,中断中调用
 */
PUBLIC int8u _checksum(int8u * ptr, int16u len);         

/***************************************************************************
 * 功能描述:    数据校验算法(CRC16)
 * 应    用:    PEDC-MONITOR 与 IBP / PSL / SIG / PIC24 / 信号系统 通信数据校验
 */
/* 数据校验
 */
unsigned short CalcCRC16_A001(unsigned char * dat, unsigned short len, unsigned short startCrc);

/* 兼容以前的函数名 */
unsigned short CRC16_Dn_Cal(unsigned char * dat, unsigned long len, unsigned short startCrc);

/* 用于 MODBUS CRC-16 */
unsigned short CRC16_Modbus(unsigned char * dat, unsigned long len);

#endif /* USR_CRC_H */

