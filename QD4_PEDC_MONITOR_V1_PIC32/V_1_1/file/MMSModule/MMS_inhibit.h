
#ifndef _MMS_INHIBIT_H
#define _MMS_INHIBIT_H

typedef struct MMS_INHIBIT
{
	int8u source_inhibit;                /*������ID,255��ʶΪMMS���ط���������ΪԶ�̿ͻ��˷���*/
	int8u IP_Field1;                     /*�ͻ���IP.Field1�������һ*/
	int8u IP_Field2;                     /*�ͻ���IP.Field1������ڶ�*/
	int8u IP_Field3;                     /*�ͻ���IP.Field1���������*/
	int8u IP_Field4;                     /*�ͻ���IP.Field1���������*/
	int8u HiB;                           /*�ͻ��˶˿�HiB�����ֽ�*/
	int8u LoB;                           /*�ͻ��˶˿�LoB�����ֽ�*/
	int8u ID;                            /*ʶ�����������Ӧ��վ̨*/
	int8u operation;                     /*0:ȡ����ֹ����;1:��ֹ����*/
	int8u inhibit_buf[8];                /*DCU �����*/
}defSTRU_MMS_INHIBIT;

/*TX_MMS_INHIBIT_MESSAGE.operation*/
#define COPR_cancelinhibit      0
#define COPR_inhibit            1

#endif

