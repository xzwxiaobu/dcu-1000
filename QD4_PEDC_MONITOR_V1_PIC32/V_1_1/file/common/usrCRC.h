
#ifndef USR_CRC_H
#define USR_CRC_H

#include "_cpu.h"

#define DFT_CRC_INI     0x0000

/***************************************************************************
 * ��������:    ����У���㷨
 * Ӧ    ��:    PEDC-MONITOR �� MMS ͨ������У��
 */
/* ����У��
 */
PUBLIC int8u checksum(int8u * ptr, int16u len);

/* ����У��,�ж��е���
 */
PUBLIC int8u _checksum(int8u * ptr, int16u len);         

/***************************************************************************
 * ��������:    ����У���㷨(CRC16)
 * Ӧ    ��:    PEDC-MONITOR �� IBP / PSL / SIG / PIC24 / �ź�ϵͳ ͨ������У��
 */
/* ����У��
 */
unsigned short CalcCRC16_A001(unsigned char * dat, unsigned short len, unsigned short startCrc);

/* ������ǰ�ĺ����� */
unsigned short CRC16_Dn_Cal(unsigned char * dat, unsigned long len, unsigned short startCrc);

/* ���� MODBUS CRC-16 */
unsigned short CRC16_Modbus(unsigned char * dat, unsigned long len);

#endif /* USR_CRC_H */

