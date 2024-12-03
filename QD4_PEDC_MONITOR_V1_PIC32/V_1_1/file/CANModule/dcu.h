
#ifndef _DCU_H
#define _DCU_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif


/* =========================================================
 * DCU״̬�ṹ�֣���DCU��ȡ����Ϣ
 * QD4 - 2022/5/30 ���
 * 
 */

/* ���忪�أ�
 * ������ DCU_STS_UNIONBYTES ��ʹ�ö����İ��ֽڶ��� DCU �ϴ���״̬�֣��ֽ� 0~9 ��
 */
//#define DCU_STS_UNIONBYTES

#ifndef DCU_STS_UNIONBYTES
typedef struct
{
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 0
typedef union
{
    struct
    {
#endif
        int8u Open_Command_OP1          : 1;     //BYTE 0.0 �Զ���������1
        int8u Close_Command_OP1         : 1;     //BYTE 0.1 �Զ���������1
        int8u                           : 1;     //BYTE 0.2 IBPPSL ��������
        int8u Open_Command_OP2          : 1;     //BYTE 0.3 �Զ���������2
        int8u Close_Command_OP2         : 1;     //BYTE 0.4 �Զ���������2
        int8u                           : 1;     //BYTE 0.5 ������
        int8u LCB_Isolate               : 1;     //BYTE 0.6 LCB����ģʽ
        int8u LCB_Auto                  : 1;     //BYTE 0.7 LCB 1=�Զ�/0=�ֶ�ģʽ
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts0;
#endif


#ifdef DCU_STS_UNIONBYTES
//BYTE 1
typedef union
{
    struct
    {
#endif
        int8u LCB_Manual_Open           : 1;     //BYTE 1.0 LCB�ֶ���������
        int8u LCB_Manual_Close          : 1;     //BYTE 1.1 LCB�ֶ���������
        int8u Solenoid_Release_R        : 1;     //BYTE 1.2 �����1��λ���� R
        int8u Solenoid_Energize_R       : 1;     //BYTE 1.3 �����1����λ���� R
        int8u Solenoid_Release_L        : 1;     //BYTE 1.4 �����2��λ���� L
        int8u Solenoid_Energize_L       : 1;     //BYTE 1.5 �����2����λ���� L
        int8u Door_Close_Switch_R       : 1;     //BYTE 1.6 ��������1�ص�λ����(R)
        int8u Door_Close_Switch_L       : 1;     //BYTE 1.7 ��������2�ص�λ����(L)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts1;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 2
typedef union
{
    struct
    {
#endif
        int8u Door_Fully_Open_Switch_R  : 1;     //BYTE 2.0 ��������1���ŵ�λ���� (�����)(R)
        int8u Door_Fully_Open_Switch_L  : 1;     //BYTE 2.1 ��������2���ŵ�λ���� (�����)( L)
        int8u Door_EED1_Switch_R        : 1;     //BYTE 2.2 Ӧ������1��翪��(����)
        int8u Door_EED2_Switch_L        : 1;     //BYTE 2.3 Ӧ������2��翪��(��)
        int8u Safety_Loop_Status_Dect   : 1;     //BYTE 2.4 ��ȫ��·״̬(˫�м��)
        int8u C_L_Switch_Status_Input   : 1;     //BYTE 2.5 ��ȫ��·״̬(����ڼ���)
        int8u Manual_Release_Switch_R   : 1;     //BYTE 2.6 �ֶ�����1����(R)
        int8u Manual_Release_Switch_L   : 1;     //BYTE 2.7 �ֶ�����2����(L)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts2;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 3
typedef union
{
    struct
    {
#endif
        int8u MSD1_Open_Switch          : 1;    //BYTE 3.0 MSD1 ��ͷ��1��翪��(R)
        int8u MSD2_Open_Switch          : 1;    //BYTE 3.1 ��ͷ��2��翪��(L)
        int8u EED_ByPass                : 1;    //BYTE 3.2 Ӧ������· (Emergency Egress Door))
        int8u                           : 1;    //BYTE 3.3  
        int8u OutputFuse1_On            : 1;    //BYTE 3.4 �������˿1��ͨ״̬��J4���Ϸ��Ǹ���,true��ʾ��ͨ��false��ʾ�Ͽ���
        int8u InputFuse1_Off            : 1;    //BYTE 3.5 ���뱣��˿1��ͨ״̬��J3���Ϸ��Ǹ���
        int8u InputFuse2_Off            : 1;    //BYTE 3.6 ���뱣��˿2��ͨ״̬��J2���뿪���ұ��Ǹ���
        int8u OutputFuse2_On            : 1;    //BYTE 3.7 �������˿2��ͨ״̬��J1�����Ǹ���
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts3;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 4
typedef union
{
    struct
    {
#endif
        int8u DCU_NumOfWV               : 1; //BYTE 4.0 �緧�������(0=2�緧��1=3�緧)
        int8u                           : 1; //BYTE 4.1 
        int8u                           : 1; //BYTE 4.2 
        int8u                           : 1; //BYTE 4.3 
        int8u                           : 1; //BYTE 4.4 δ����
        int8u                           : 1; //BYTE 4.5 δ����
        int8u                           : 1; //BYTE 4.6 δ����
        int8u Dual_Motor_Configure      : 1; //BYTE 4.7 �����(1=˫��0=�����)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts4;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 5
typedef union
{
    struct
    {
#endif
        int8u DCUMode       : 4; //BYTE 5.0~3 DCUģʽ
        int8u DoorMode      : 4; //BYTE 5.4~7 ��ģʽ 
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts5;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 6
typedef union
{
    struct
    {
#endif
        int8u WVCmdOpn                  : 1; //BYTE 6.0 �յ����緧����
        int8u WVCmdCls                  : 1; //BYTE 6.1 �յ��ط緧����
        int8u WV1Opened                 : 1; //BYTE 6.2 �緧1����λ
        int8u WV1Closed                 : 1; //BYTE 6.3 �緧1�ص�λ
        int8u WV2Opened                 : 1; //BYTE 6.4 �緧2����λ
        int8u WV2Closed                 : 1; //BYTE 6.5 �緧2�ص�λ
        int8u WV3Opened                 : 1; //BYTE 6.6 �緧3����λ
        int8u WV3Closed                 : 1; //BYTE 6.7 �緧3�ص�λ
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts6;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 7
typedef union
{
    struct
    {
#endif
        int8u err_init_door  : 1; //BYTE 7.0 err_init_door
        int8u err_open_door  : 1; //BYTE 7.1 err_open_door
        int8u err_close_door : 1; //BYTE 7.2 err_close_door
        int8u err_unlook_prc : 1; //BYTE 7.3 err_unlook_prc
        int8u err_mt1_origin : 1; //BYTE 7.4 err_mt1_origin
        int8u err_mt2_origin : 1; //BYTE 7.5 err_mt2_origin
        int8u err_mag1_flag  : 1; //BYTE 7.6 err_mag1_flag
        int8u err_mag2_flag  : 1; //BYTE 7.7 err_mag2_flag
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts7_Err1;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 8
typedef union
{
    struct
    {
#endif
        int8u err_mt1_flag              : 1; /* BYTE 8.0 : err_mt1_flag                 */
        int8u err_mt2_flag              : 1; /* BYTE 8.1 : err_mt2_flag                 */
        int8u err_mt1_hot_flag          : 1; /* BYTE 8.2 : err_mt1_hot_flag             */
        int8u err_mt2_hot_flag          : 1; /* BYTE 8.3 : err_mt2_hot_flag             */
        int8u err_hall_1_flag           : 1; /* BYTE 8.4 : err_hall_1_flag              */
        int8u err_hall_2_flag           : 1; /* BYTE 8.5 : err_hall_2_flag              */
        int8u err_end_door_ov_time_flag : 1; /* BYTE 8.6 : err_end_door_ov_time_flag    */
        int8u err_saft_loop_flag        : 1; /* BYTE 8.7 : err_saft_loop_flag           */
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts8_Err2;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 9
typedef union
{
    struct
    {
#endif
        int8u err_cmd_flag              : 1; /* BYTE 9.0 : err_cmd_flag */
        int8u err_fuse                  : 1; /* BYTE 9.1 : ����˿����, byte 3��bit4��5��6��7�κ�һ��Ϊfalse�����ﱨtrue */
        int8u err_CPUOverHeat           : 1; /* BYTE 9.2 : ��CPU���±��� */
        int8u err_OpnClsDoorOT          : 1; /* BYTE 9.3 : �����ų�ʱ���� */
        int8u err_SFLoopFlash           : 1; /* BYTE 9.4 : ��ȫ��·���� */
        int8u err_WV1OverTime           : 1; /* BYTE 9.5 : �緧1��/�س�ʱ���� */
        int8u err_WV2OverTime           : 1; /* BYTE 9.6 : �緧2��/�س�ʱ���� */
        int8u err_WV3OverTime           : 1; /* BYTE 9.7 : �緧3��/�س�ʱ���� */
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts9_Err3;
#endif


#ifndef DCU_STS_UNIONBYTES
}tDCU_AllStsFlags;
#endif


#define DEF_UPDATE_CAN12_STATE_TIME                 100             //ms

//***********************************************************
EXTN void   SetpBufBit(pINT8U ptr , int8u bit);
EXTN void   ClrpBufBit(pINT8U ptr , int8u bit);
EXTN int8u  pBufIfOnLine(pINT8U ptr ,int8u uBit);                   //cDcuOnLineF ���� ��cDcuOffLineF ����
EXTN void   SetDcuOnLine(int8u uID_DCU);                            //dcu ��������
EXTN void   SetOffLineDcu(int8u uID_DCU);                           //dcu ��������
EXTN int8u  DcuIfOnLine(int8u uID_DCU);                             //cDcuOnLineF ���� ��cDcuOffLineF ����
EXTN int8u  ptrDcuIfOnLine(pINT8U ptr , int8u Id);                  //cDcuOnLineF ���� ��cDcuOffLineF ����
EXTN int8u  FindNextDcu(pINT8U ptr ,pINT8U pID_DCU ,int8u DcuNum);  //������һ�� DCU ,�ҵ� ���� DCU Ϊֹ������ѭ��Ѱ��
EXTN void   DcuInc(pINT8U pID_DCU ,int8u DcuNum);                                    //DCU ��һ ���������ֵѭ������Сֵ
EXTN int16u GetCrc(pINT8U ptr, int16u Len);
EXTN int16u _GetCrc(pINT8U ptr, int16u Len);                            //�ж���
EXTN void DcuCan12State(BOOL wait);                                 //��DCU can12 ����Ϣ���ݸ� uniDcuData



PUBLIC int8u* getAllDCUOnLineSts(int8u DCUs, int8u chkStatus, int8u* setting);
PUBLIC BOOL getPSDInhibitTrain(int8u *pInhibit, int len);

PUBLIC int8u findFirstBit(pINT8U p, int8u Len);
PUBLIC int8u findLastBit(pINT8U p, int8u Len);
PUBLIC int8u calcSetting1Num(pINT8U p, int8u Len);

#endif



