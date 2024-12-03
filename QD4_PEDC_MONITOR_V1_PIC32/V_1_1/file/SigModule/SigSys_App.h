/* =============================================================================
 * Project : 
 * Module  : 信号系统处理应用成
 * Date    : 
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _SIGSYS_APP_H
#define _SIGSYS_APP_H

#include "_cpu.h"

/* 信号系统系统与站台门系统通信的单元标识 */
#define UNIT_ID     255

/* =========================================================
 * 通信数据结构
 *
 */

/* 主程序循环调用， 与 信号系统 通信处理 */
PUBLIC void processSigSysComm(void);

PUBLIC int8u * getRxFromSigSys(int8u * len);

PUBLIC int8u * getTxToSigSys(int8u * len);

//**********************************************
#endif  //_SIGSYS_APP_H



