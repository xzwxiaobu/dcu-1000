/* =============================================================================
 * Project : 
 * Module  : BASϵͳ����Ӧ�ò�
 * Date    : 
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _BASSYS_APP_H
#define _BASSYS_APP_H

#include "_cpu.h"
#include "BASSys_Uart.h"
#include "BASSys_Reg.h"


/* =========================================================
 * ͨ�����ݽṹ
 *
 */

/* ������ѭ�����ã� �� BASϵͳ ͨ�Ŵ��� */
PUBLIC void processBASSysComm(void);

PUBLIC int8u * getRxFromBASSys(int8u * len);

PUBLIC int8u * getTxToBASSys(int8u * len);

//**********************************************
#endif  //_BASSYS_APP_H



