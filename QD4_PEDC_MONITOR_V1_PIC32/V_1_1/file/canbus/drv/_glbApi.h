#ifndef _APIFUN_H_
#define _APIFUN_H_


#include "glbCan.h"

#if (UPDATE_FUN_EN == TRUE)
#include "glbUpdate.h"
#endif

// ==========================================================================
//
//NOTICE: �ڵ� ID ������ 7bits, �����Ǵ� 1 ��ʼ
//        �㲥���� ID ������ '0'
//
// ==========================================================================
#define SYS_TICK_1MS         1         //ms

#ifndef DCU_STATE_IDLE
//DCU����״̬����.
//EXTN int16u dcu_state;
//dcu_state
#define DCU_STATE_IDLE              0	//����
#define DCU_STATE_HAND              1	//�ֶ�
#define DCU_STATE_AUTO              2	//�Զ�
#define DCU_STATE_UNLOCK            3	//����
#define DCU_STATE_INIT              4	//ʹ���ù���λ�ã���ʼ����
#define DCU_STATE_STOP              5	//ֹͣ���ܲ��������ϼ�������

//������״̬����.
//EXTN int16u door_state;
//door_state
#define DOOR_STATE_CLOSE            0	//�ѹ���
#define DOOR_STATE_CLOSING          1	//���Ź�����
#define DOOR_STATE_CLOSING1         2	//�������ϴ�����
#define DOOR_STATE_OPEN             3	//�ѿ���
#define DOOR_STATE_OPENING          4	//���Ź�����
#define DOOR_STATE_OPENING1         5	//�������ϴ�����
#define DOOR_STATE_INIT             6	//���ù���λ�ã���ʼ������
#define DOOR_STATE_UNLOCK           7	//�Ž���������
#define DOOR_STATE_FREE             8	//������״̬��
#endif

//**********************************************************************************************
//                                                                                             *
//                                      API ��������                                           *
//                                                                                             *
//**********************************************************************************************

extern void ApiMainProcess();
extern void ApiMainInit();
extern void ApiGetDevieceInfo(void);

extern int8u __attribute__((aligned (2))) DeviceInfo[];

#endif //_APIFUN_H_
