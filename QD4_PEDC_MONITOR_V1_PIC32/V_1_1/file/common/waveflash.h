
#ifndef _WAVEFLASH_H
#define _WAVEFLASH_H

//*********************************************
#define CREADDOTPERIOD      5       //��ȡ���� ms
#define CREADDOTNUM         120     //��ȡ���� ; <= 255

#define LEVEL_STABLE_TIME   60      //(300/CREADDOTPERIOD)    //300ms ,������ĵ�ĵ�ƽ��ά�� level_stable_time ������Ϊ��ƽ�ȶ�
#define SHAKE_TIME          120     //(600 / CREADDOTPERIOD)  //600ms ,�������> 255; �ڴ�ʱ���ڣ���������һ�ֵ�ƽ���ȶ�������Ϊ���ȶ�ǰ�Ķ������������� ; 

extern BOOL chkDCUSF_K31PN(void);
extern BOOL chkDCUSF_K31NC(void);
extern BOOL chkPDSSF_Coil(void);;
extern BOOL chkPDSSF_Contactor(void);
//
//#define CHK_SF_RELAY_VOLTAGE()      chkDCUSF_K31PN()
//#define CHK_SF_RELAY_CONTACTOR()    chkDCUSF_K31NC()

typedef struct
{
	int8u readDotPeriod;                    //��ȡ����
	int8u DotStart;                         //��ȡ�㿪ʼλ��
	int8u DotEnd;                           //��ȡ�����λ�� (��λ��û������)
	int8u levelnum;                         //��ƽ��
	int8u DotBuf[CREADDOTNUM / 8 + 1];      //��ȡ�㻺��
	int8u level_stable_time;                //��⵽��ƽ�ȶ�ʱ��
	int8u shake_time;                       //��������ʱ��
	int8u CNT;
	int8u time;                             //ʱ��
	int8u flashNum;                         //���ϴ��� �� 0 û������
	
	int8u flashmsg_to_mms;                  //������Ϣ��MMS
	
}tReadDot;

//*********************************************
#define READDDOT_NUM            4

#define SORT_DCUSF_coil         0       //DCU��ȫ�̵�����Ȧ��ѹ
#define SORT_DCUSF_contact      1       //DCU��ȫ�̵��������ѹ
#define SORT_PDSSF_coil         2       //PDS��ȫ�̵�����Ȧ��ѹ
#define SORT_PDSSF_contact      3       //PDS��ȫ�̵��������ѹ

//*********************************************

//tReadDot.CNT
#define powOnPhase        0              //���߻�͵�ƽ
#define LevelHigh         1              //�ȶ��ĸߵ�ƽ
#define H_testL           2              //��⵽��
#define H_testH           3              //��⵽��

#define LevelLow          0x81           //�ȶ��ĵ͵�ƽ
#define L_testL           0x82           //��⵽��
#define L_testH           0x83           //��⵽��

//*********************************************
//EXTN int8u scan_sf_time;
#define CSCAN_SF_TIME      5            //5ms

//ReadDot[].Time;      
#define CHANDLETIME        200          //ms

//*********************************************
//EXTN int8u sf_dcu_flash;                    //DCU��¼��ȫ��·���ϴ���
//EXTN int8u sf_pds_flash;                    //PDS��¼��ȫ��·���ϴ���

//EXTN int8u i2c_err_cal;                     //��������ﵽ�趨����������� ReadDot
//#define Cls_ReadDotNum      5              //Լ 5 * 5ms


//*********************************************
//
//PUBLIC void ini_DotBuf_all(void);  //��ʼ��
//PUBLIC void HandleSF_FLASH(void);

/* ��ʼ��������MCU��������� */
PUBLIC void  SFDectectInit(void);   

/* ��⴦������������ѭ������ */
PUBLIC void  SFDectectProc(int8u scanInterval);

/* ��ʱ�������������ж�1ms���� */
PUBLIC void  SFDectectTimer(void);

/* ��ȡ������� */
PUBLIC int8u getSFFlashResult(int8u type, BOOL rstLast);



//*********************************************
#endif
