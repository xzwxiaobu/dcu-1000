#ifndef _APIFUN_H_
#define _APIFUN_H_


#include "glbCan.h"

#if (UPDATE_FUN_EN == TRUE)
#include "glbUpdate.h"
#endif

// ==========================================================================
//
//NOTICE: 节点 ID 必须是 7bits, 必须是从 1 开始
//        广播发送 ID 必须是 '0'
//
// ==========================================================================
#define SYS_TICK_1MS         1         //ms

#ifndef DCU_STATE_IDLE
//DCU工作状态定义.
//EXTN int16u dcu_state;
//dcu_state
#define DCU_STATE_IDLE              0	//隔离
#define DCU_STATE_HAND              1	//手动
#define DCU_STATE_AUTO              2	//自动
#define DCU_STATE_UNLOCK            3	//解锁
#define DCU_STATE_INIT              4	//使门置关门位置（初始化）
#define DCU_STATE_STOP              5	//停止功能操作（故障及报警）

//滑动门状态定义.
//EXTN int16u door_state;
//door_state
#define DOOR_STATE_CLOSE            0	//已关门
#define DOOR_STATE_CLOSING          1	//关门过程中
#define DOOR_STATE_CLOSING1         2	//关门遇障处理中
#define DOOR_STATE_OPEN             3	//已开门
#define DOOR_STATE_OPENING          4	//开门过程中
#define DOOR_STATE_OPENING1         5	//开门遇障处理中
#define DOOR_STATE_INIT             6	//门置关门位置（初始化）中
#define DOOR_STATE_UNLOCK           7	//门解锁处理中
#define DOOR_STATE_FREE             8	//门自由状态中
#endif

//**********************************************************************************************
//                                                                                             *
//                                      API 函数定义                                           *
//                                                                                             *
//**********************************************************************************************

extern void ApiMainProcess();
extern void ApiMainInit();
extern void ApiGetDevieceInfo(void);

extern int8u __attribute__((aligned (2))) DeviceInfo[];

#endif //_APIFUN_H_
