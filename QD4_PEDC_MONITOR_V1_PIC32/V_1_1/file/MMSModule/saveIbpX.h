
#ifndef _SAVEIBPX_H_
#define _SAVEIBPX_H_
#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

//*****************************************************

#define OK                 0
#define FAIL               1

#define IBPALARM_LEN	     8                 //�´����� ����
#define IBPALARM_BUFLEN	   (IBPALARM_LEN + 4)

#define cDefault_DCU_NUM    24                  //Ĭ��DCU ��: 24 for QD4
#define cDefault_Parameter (0xff - 0b010)       //.0 = 1 CAN˫�ߣ� .1 = 0������Ĭ�Ͽ�
#define cDefault_On_Hour    4                   // ?���ƴ�����ʱ��--Сʱ ��Hex��   1:30-4:30 ������ʱ����
#define cDefault_On_Minute  30                  // ?���ƴ�����ʱ��--���� ��Hex��
#define cDefault_Off_Hour   1                   // ?���ƴ��ػ�ʱ��--Сʱ ��Hex��
#define cDefault_Off_Minute 30                  // ?���ƴ��ػ�ʱ��--���� ��Hex��

#define cDefault_WVTotal        50              // �緧����
#define cDefault_WVNum_Opned    40              // �緧ȫ����
#define cDefault_WVNum_Clsed    40              // �緧ȫ����

#define IBPALARM_0XAA	     0xaa
#define IBPALARM_0X55	     0x55
//
#define DCUsetting_LEN     8                 //dcu ��ű�־λ��ռ����
#define DCUsetting_BUFLEN  (DCUsetting_LEN + 3)

#define PARA_START_SEAT    0                 //������BUF�еĿ�ʼλ��
#define DCUsetting_START_SEAT  (PARA_START_SEAT + IBPALARM_BUFLEN)

#define DCUsetting_SEAT0     (DCUsetting_START_SEAT + 0)
#define DCUsetting_SEAT1     (DCUsetting_START_SEAT + 1)
#define DCUsetting_SEAT2     (DCUsetting_START_SEAT + 2)
#define DCUsetting_SEAT3     (DCUsetting_START_SEAT + 3)
#define DCUsetting_SEAT4     (DCUsetting_START_SEAT + 4)
#define DCUsetting_SEAT5     (DCUsetting_START_SEAT + 5)
#define DCUsetting_SEAT6     (DCUsetting_START_SEAT + 6)
#define DCUsetting_SEAT7     (DCUsetting_START_SEAT + 7)
#define DCUsetting_0AA_SEAT  (DCUsetting_START_SEAT + 8)
#define DCUsetting_055_SEAT  (DCUsetting_START_SEAT + 9)
#define DCUsetting_CHS_SEAT  (DCUsetting_START_SEAT + 10)



//*****************************************************
#define DownSet_Canbus       (tagDownIbpAlarmParameter.uIbpAlarmParameter.B0) //ֻ��ϵ�� PSC BUS���߱���
#define two_can              (DownSet_Canbus)

#define FORBID_BUZZER        (tagDownIbpAlarmParameter.uIbpAlarmParameter.B1) //MMS ��1 ��ֹ BUZZER ���

#define OVERSEE_LIGHT        (tagDownIbpAlarmParameter.uIbpAlarmParameter.B2) //���byte 0��bit2=0,��?���ƴ���������������ԤԼʱ�����ƣ���������pedc����ʱ��pedc�յ���Ϣ�󲻸���byte2-byte5��byte 0��bit2=1ʱpedc�Ÿ���byte2-byte5
   
//*****************************************************
EXTN int8u write_IbpAlarmX_To_Flash(void);
EXTN int8u Read_IbpAlarmX_From_Flash(void);
EXTN void reset_Read_IbpAlarmX(void);

EXTN int8u write_DCUsetting_To_Flash(void);
EXTN int8u Read_DCUsetting_From_Flash(void);

#endif



