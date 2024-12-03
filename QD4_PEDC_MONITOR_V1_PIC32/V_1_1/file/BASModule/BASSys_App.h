/* =============================================================================
 * Project : 
 * Module  : BAS系统处理应用层
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
 * 通信数据结构
 *
 */

/* 主程序循环调用， 与 BAS系统 通信处理 */
PUBLIC void processBASSysComm(void);

PUBLIC int8u * getRxFromBASSys(int8u * len);

PUBLIC int8u * getTxToBASSys(int8u * len);

//**********************************************
#endif  //_BASSYS_APP_H



