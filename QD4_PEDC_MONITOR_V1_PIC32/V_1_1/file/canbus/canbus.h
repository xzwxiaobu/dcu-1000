
#ifndef _CANOPEN_H
#define _CANOPEN_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
/****************** DATA TYPES Complier Specific ******************************/

typedef struct tagKBITINT
{
	int8u B0  : 1;
	int8u B1  : 1;
	int8u B2  : 1;
	int8u B3  : 1;
	int8u B4  : 1;
	int8u B5  : 1;
	int8u B6  : 1;
	int8u B7  : 1;
	int8u B8  : 1;
	int8u B9  : 1;
	int8u B10 : 1;
	int8u B11 : 1;
	int8u B12 : 1;
	int8u B13 : 1;
	int8u B14 : 1;
	int8u B15 : 1;
} KBITINT;

typedef union
{
	unsigned int Dbyte;
	KBITINT Dfield;
} INT16UField;


EXTN union
{
	struct
	{
		unsigned b0  : 1;     //�Զ���������1
		unsigned b1  : 1;     //�Զ���������1
		unsigned b2  : 1;     //�Զ����Թ���1
		unsigned b3  : 1;     //�Զ���������2
		unsigned b4  : 1;     //�Զ���������2
		unsigned b5  : 1;     //�Զ����Թ���2
		unsigned b6  : 1;     //bk
		unsigned b7  : 1;     //bk

		unsigned b8  : 1;     //����ģʽ
		unsigned b9  : 1;     //�Զ�/�ֶ�ģʽ
		unsigned b10 : 1;     //�ֶ���������
		unsigned b11 : 1;     //�ֶ���������
		unsigned b12 : 1;     //�һ����Ÿ�λ����
		unsigned b13 : 1;     //�󻬶��Ÿ�λ����
		unsigned b14 : 1;     //�һ����ſ��ŵ�λ���� (�����)
		unsigned b15 : 1;     //�󻬶��ſ��ŵ�λ���� (�����)

		unsigned b16 : 1;     //�ҵ������λ����
		unsigned b17 : 1;     //�ҵ������λ����
		unsigned b18 : 1;     //��������λ����
		unsigned b19 : 1;     //��������λ����
		unsigned b20 : 1;     //���ֶ���������
		unsigned b21 : 1;     //���ֶ���������
		unsigned b22 : 1;     //�˿�̽������·����
		unsigned b23 : 1;     //Ӧ���Ź�翪��

		unsigned b24 : 1;     //˾���Ź�翪��
		unsigned b25 : 1;     //��ͷ�Ź�翪��
		unsigned b26 : 1;     //����ָʾ�Ƽ�⿪��
		unsigned b27 : 1;     //����ָʾ�Ƽ�⿪��
		unsigned b28 : 1;     //�ź��翪��1
		unsigned b29 : 1;     //�ź��翪��2
		unsigned b30 : 1;     //�ź��翪��3
		unsigned b31 : 1;     //��ȫ��·״̬

		unsigned b32_35 : 4;  //DCU����ģʽ
		unsigned b36_39 : 4;  //��״̬

//����
		unsigned b40 : 1;    //err_init_door
		unsigned b41 : 1;    //err_open_door
		unsigned b42 : 1;    //err_close_door
		unsigned b43 : 1;    //err_unlook_prc
		unsigned b44 : 1;    //err_mt1_origin
		unsigned b45 : 1;    //err_mt2_origin
		unsigned b46 : 1;    //err_mag1
		unsigned b47 : 1;    //err_mag2

		unsigned b48 : 1;    //err_mt1       r
		unsigned b49 : 1;    //err_mt2       l
		unsigned b50 : 1;    //err_mt1_hot   r
		unsigned b51 : 1;    //err_mt2_hot   l
		unsigned b52 : 1;    //err_hall1     r
		unsigned b53 : 1;    //err_hall2     l
		unsigned b54 : 1;    //err_end_door_ov_time
		unsigned b55 : 1;    //err_saft_loop_flag

		unsigned b56 : 1;    //err_cmd_flag
		unsigned b57 : 1;    //err_lamp_r
		unsigned b58 : 1;    //err_lamp_l
		unsigned b59 : 1;    //err_after_door_sw1
		unsigned b60 : 1;    //err_after_door_sw2
		unsigned b61 : 1;    //err_after_door_sw3
		unsigned b62 : 1;    //
		unsigned b63 : 1;    //

		unsigned b64_79   : 16;   //�һ�����λ��
		unsigned b80_95   : 16;   //�󻬶���λ��
		unsigned b96_111  : 16;   //�һ������ٶ�
		unsigned b112_127 : 16;   //�󻬶����ٶ�
		unsigned b128_143 : 16;   //��Դ��ѹ
		unsigned b144_159 : 16;   //
		unsigned b160_175 : 16;   //
	} bits;

	struct
	{
		int8u byte0;
		int8u byte1;
		int8u byte2;
		int8u byte3;
		int8u byte4;
		int8u byte5;
		int8u byte6;
		int8u byte7;

		int16u word4;
		int16u word5;
		int16u word6;
		int16u word7;
		int16u word8;
		int16u word9;
		int16u word10;
		
		int8u CanBusCheck;  //���һ�ֽ� CAN ͨѶ Ч�����
		int8u reserve;      //���һ�ֽ� CAN ͨѶ Ч�����
		
	}bytes ;
} DcuStateUp;           //�ϴ� PEDC Dcu ״̬ ,���һ�ֽ� can ͨѶЧ�����


EXTN struct
{
	int16u dcu_para0;    //���ſ��(��)
	int16u dcu_para1;    //���ſ��(��)
	int8u dcu_para2;     //����ʱ��
	int8u dcu_para3;     //����ʱ��
	int8u dcu_para4;     //�������ȼ�(����)
	int8u dcu_para5;     //�������ȼ�(����)
	int8u dcu_para6;     //���ű�����ʱ
	int8u dcu_para7;     //�����ؿ�����
	int8u dcu_para8;     //�ؿ���رյȴ�ʱ��
	int8u dcu_para9;     //�����ؿ����
	int8u dcu_para10;    //״̬�ƿ���ʱ��˸Ƶ��
	int8u dcu_para11;    //״̬�ƹ���ʱ��˸Ƶ��
	int8u dcu_para12;    //״̬�ƹ���ʱ��˸Ƶ��
	int8u dcu_para13;    //�ֶ������������ʱʱ��

	int8u CanBusCheck;   //���һ�ֽ� CAN ͨѶ Ч�����
} DcuParaDown;         //PEDC ��ȡ DCU���� BUF


EXTN int8u OpenSpeedCurve[CURVE_LEN + 1];             //�����ٶ����� ,���һ�ֽ� can ͨѶЧ�����
EXTN int8u CloseSpeedCurve[CURVE_LEN + 1];            //�����ٶ����� ,���һ�ֽ� can ͨѶЧ�����

EXTN int8u can_down_dcuPara[ONE_DCU_PARA_LEN + 1];    //pedc ���� DCU���� ������־New_para�� ���һ�ֽ� CAN ͨѶ Ч�����
EXTN int8u DcuONLINE[2];                              //�Ƿ����߲���

//**************************************************************************

#define ican_read_OK   fgTransOK   //��ok
#define ican_write_OK  fgTransOK   //дok
#define ican_read_Err  fgAbort     //��err
#define ican_write_Err fgAbort     //дrrr
#define ican_read_ing  fgRunning   //��������
#define ican_write_ing fgRunning   //д������

//��ȡ ����״̬
#define Bus_DownDcuParaState()            ChkTransState(FUNID_Down_MoreDcuParameter)
#define Bus_ReadDcuParaState()            ChkTransState(FUNID_Up_MoreDcuParameter)
#define Bus_ReadDcuState_State()          ChkTransState(FUNID_UpOneDcuState)
#define Bus_ReadOpenCurveState()          ChkTransState(FUNID_Up_OpenDoorCurve)
#define Bus_ReadCloseCurveState()         ChkTransState(FUNID_Up_CloseDoorCurve)

//������� ״̬
#define clsDCU_DownDcuParaState()  ClrTransState(FUNID_Down_MoreDcuParameter);

EXTN INT16UField  Flag0;
#define New_para                    Flag0.Dfield.B0      //1 �в�������
#define State_Reading               Flag0.Dfield.B1      //1 ���ڶ�ȡ ״̬ ��22 �ֽ�
#define Para_Reading                Flag0.Dfield.B2      //1 ���ڶ�ȡ ���� ,16 �ֽ�
#define OpenSpeedCurve_Reading      Flag0.Dfield.B3      //1 ���ڶ�ȡ ��������
#define CloseSpeedCurve_Reading     Flag0.Dfield.B4      //1 ���ڶ�ȡ ��������

EXTN void CanFun(void);    //CAN ͨѶ����ֵ ����������

#endif

//--------------------------------------
