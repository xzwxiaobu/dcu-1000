/* =============================================================================
 * Project : 
 * Module  : �ź�ϵͳ����Ӧ�ó�
 * Date    : 
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _SIGSYS_APP_H
#define _SIGSYS_APP_H

#include "_cpu.h"

/* �ź�ϵͳϵͳ��վ̨��ϵͳͨ�ŵĵ�Ԫ��ʶ */
#define UNIT_ID     255

/* =========================================================
 * ͨ�����ݽṹ
 *
 */

/* ������ѭ�����ã� �� �ź�ϵͳ ͨ�Ŵ��� */
PUBLIC void processSigSysComm(void);

PUBLIC int8u * getRxFromSigSys(int8u * len);

PUBLIC int8u * getTxToSigSys(int8u * len);

//**********************************************
#endif  //_SIGSYS_APP_H



