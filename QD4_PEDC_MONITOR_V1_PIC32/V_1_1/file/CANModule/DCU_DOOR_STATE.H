
#ifndef _DCU_DOOR_STATE_H
#define _DCU_DOOR_STATE_H


//dcu_state
#define DCU_STATE_IDLE              0	//隔离	0
#define DCU_STATE_HAND              1	//手动	1
#define DCU_STATE_AUTO              2	//自动	2
#define DCU_STATE_UNLOCK          	3	//解锁	3
#define DCU_STATE_INIT              4	//使门置关门位置（初始化）	4
#define DCU_STATE_STOP              5	//停止功能操作（故障及报警）	5


//door_state
#define DOOR_STATE_CLOSE            0	//已关门	0
#define DOOR_STATE_CLOSING        	1	//关门过程中	1
#define DOOR_STATE_CLOSING1      	  2	//关门遇障处理中	2
#define DOOR_STATE_OPEN             3	//已开门	3
#define DOOR_STATE_OPENING        	4	//开门过程中	4
#define DOOR_STATE_OPENING1      	  5	//开门遇障处理中	5
#define DOOR_STATE_INIT             6	//门置关门位置（初始化）中	6
#define DOOR_STATE_UNLOCK         	7	//门解锁处理中	7
#define DOOR_STATE_FREE             8	//门自由状态中	8


#endif

