
#ifndef _CAN_H
#define _CAN_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif


#define defOFFLINEJUDGETIME                4         //���� DCU �Ǽ��������ߴ��� ���糬���ô��� ����Ϊ���ߣ������ߵǼ�
#define defSTATEREADERR_CONTINUEREAD 2         //�����ȡ���������������,����ҪС�� c_OffLinejudgeTime
#define defUPONEDCUSTATETIME         8         // 8ms  ����ȡ��һ�� DCU ״̬��ʱ����

#define BITEQU1                      1
#define BITEQU0                      0

#define cDcuOnLineF                  1           //���߱�־
#define cDcuOffLineF                 0           //����

#define FindOk                       1
#define FindFail                     0

#define ican_read_OK                 fgTransOK   //��ok
#define ican_write_OK                fgTransOK   //дok
#define ican_read_Err                fgAbort     //��err
#define ican_write_Err               fgAbort     //дrrr
#define ican_read_ing                fgRunning   //��������
#define ican_write_ing               fgRunning   //д������

#define DCU_cfg_LEN                  8                         //�������DCU ��Ӧ��BUF
#define DCU_ONECAN_ONLINE_LEN        8                         //ÿ��CAN���߶�Ӧ����DCU���߱�־
#define MCP_OPENCLOSE_DOOR_LEN       24                        //MCP �� DCU �Ŀ���������
#define AVR_DATA_RELAY_LEN           20                        //�̵���״̬�����أ����ϣ�
#define AVR_DATA_CMD_POW_LEN         16                        //SIG,PSL,IBP,PSC������,��Դ����
#define AVR_DATA_LEN                 (AVR_DATA_RELAY_LEN + AVR_DATA_CMD_POW_LEN) //avr ��������ݳ���36
#define DCU_CAN12_STATE_LEN          (DCU_ONECAN_ONLINE_LEN*2) //DCU CAN12 ��Ϣ�Ǽǣ� 1 ��ʾͨѶ����
#define ALL_DCU_STATE_LEN            (ONE_DCU_STATE_LEN * MAX_DCU)
#define ALL_DCU_PARA_LEN             (ONE_DCU_PARA_LEN * MAX_DCU)
#define UPLIGHTSCREENLEN             10
#define DNLIGHTSCREENLEN             10
#define BYTES_SYSTIME_TODCU          7      //����ϵͳʱ���DCU

#define BYTES_PER_RELAYSLIFE        4       /* ÿ���̵��������ĳ���(�ֽ�)   */
#define TOTAL_RELAYS_LOGPCB         21      /* �߼����ư�ļ̵�������       */
#define NUM_OF_LOGPCB               2       /* ��Ŀ��ʹ���߼����ư������   */
#define TOTAL_RELAYSLIFE_LEN        (BYTES_PER_RELAYSLIFE * TOTAL_RELAYS_LOGPCB * NUM_OF_LOGPCB)
                                      
#define AVR_LEN                      (DCU_cfg_LEN + DCU_CAN12_STATE_LEN + AVR_DATA_LEN + MCP_OPENCLOSE_DOOR_LEN + TOTAL_RELAYSLIFE_LEN) 
#define AVR_ONLINE_ALLDCUSTATE_LEN   (AVR_LEN + ALL_DCU_STATE_LEN)

typedef  union                               //��ӦЭ��
{
    struct
    {
        int8u aDcuCfgBUF[DCU_cfg_LEN];
        int8u aDcuCan12State[DCU_CAN12_STATE_LEN];
        int8u aAvrDataBuf[AVR_DATA_LEN];
        int8u aMcpOpenCloseDoorCommand[MCP_OPENCLOSE_DOOR_LEN];
        int8u aRelayLifes[NUM_OF_LOGPCB][BYTES_PER_RELAYSLIFE * TOTAL_RELAYS_LOGPCB];    /* �̵������� */
        int8u aDcuState[MAX_DCU][ONE_DCU_STATE_LEN];
    }sortBuf;

    int8u aDcuData[AVR_ONLINE_ALLDCUSTATE_LEN];

}UNIDcuData;
EXTN UNIDcuData uniDcuData;

//typedef struct tagKUP_DCU_STATE
//{
////    int8u uUp_State_DcuNum;                             //���ϴ�״̬��DCU����
//
//}KUP_DCU_STATE;
//EXTN KUP_DCU_STATE tagUP_DCU_STATE;

#define defPARAMETER_FLAG_LEN (DCU_ONECAN_ONLINE_LEN)
typedef struct tagKUP_MORE_DCU_PARAMETER                //�ϴ���ѡ�� DCU ����
{
    int8u aUpDcuParameterRequest[defPARAMETER_FLAG_LEN]; //�����DCU
    int8u aUpDcuParameterResult[defPARAMETER_FLAG_LEN];  //��ȡ���
    int8u aAllDcuPara[MAX_DCU][ONE_DCU_PARA_LEN];        //���α����ȡDCU ���������ñ����λ�����κ���Ҫ�ϴ���DCUλ��һ�£����ϴ���DCU��ռλ�ã�
    int8u uUpDcuParameter_IdDcu;                         //�ֶ�ȡ�� DCU ID
    int8u aUpDcuParameterBuf[ONE_DCU_PARA_LEN + 1];      //CAN ���߶�ȡ���� DCU���� ���λ�ã����һ�ֽ�ΪCANͨѶCRC��
    int8u aUpDcuParameter_DcuNum;                        //��ȡ������DCU����; ͳ��Ҫ����DCU������

}KUP_DCU_PARAMETER;
EXTN KUP_DCU_PARAMETER tagUpDcuParameter;

typedef struct tagKDOWN_MORE_DCU_PARAMETER               //������ѡ�� DCU ����
{
    int8u aDownDcuParameterRequest[defPARAMETER_FLAG_LEN];//�����DCU
    int8u aDownDcuParameterResult[defPARAMETER_FLAG_LEN]; //���ؽ��
    int8u uDownDcuParameter_IdDcu;                        //�����ص� DCU ID
    int8u aDownParameterBuf[ONE_DCU_PARA_LEN + 1];        //���صĲ���
}KDOWN_MORE_DCU_PARAMETER;
EXTN KDOWN_MORE_DCU_PARAMETER tagDOWN_MORE_DCU_PARAMETER;

typedef struct tagKUPOPENDOORCURVE                        //�����������ݻ�ȡ
{
    int8u aUpOpenDoorCurveBuf[CURVE_LEN + 1];              //��������, DCU���� ���λ�ã����һ�ֽ�ΪCANͨѶCRC��
    int8u uUpOpenDoorCurve_IdDcu;                          //��ȡ�������ߵ� DCU
}KUPOPENDOORCURVE;
EXTN KUPOPENDOORCURVE tagUPOPENDOORCURVE;

typedef struct tagKUPCLOSEDOORCURVE                       //�����������ݻ�ȡ
{
    int8u aUpCloseDoorCurveBuf[CURVE_LEN + 1];             //��������
    int8u uUpCloseDoorCurve_IdDcu;                         //���������ߵ� DCU
}KUPCLOSEDOORCURVE;
EXTN KUPCLOSEDOORCURVE tagUPCLOSEDOORCURVE;

typedef struct tagKDOWNIBPALARMPARAMETER                  //ibp����������������
{
    int8u uRead_IbpAlarmParameter;                         //����ȡ FLASH �����ݴ洢λ��
    int8u uRead_WVTotalNum;         //�緧����( Ĭ��DCU���� )
    int8u uRead_On_Hour;            // ?���ƴ�����ʱ��--Сʱ ��Hex��
    int8u uRead_On_Minute;          // ?���ƴ�����ʱ��--���� ��Hex��
    int8u uRead_Off_Hour;           // ?���ƴ��ػ�ʱ��--Сʱ ��Hex��
    int8u uRead_Off_Minute;         // ?���ƴ��ػ�ʱ��--���� ��Hex��
    int8u uRead_WVNum_Opn;          //�緧ʵ��ȫ���������ڻ���ڴ���ֵʱ��BAS���ȫ�����緧��������0ʱ�˲�������Ч����Χ1-�緧����
    int8u uRead_WVNum_Cls;          //�緧ʵ��ȫ���������ڻ���ڴ���ֵʱ��BAS���ȫ�ء��緧��������0ʱ�˲�������Ч����Χ1-�緧����

    int8u uDOWN_IbpAlarmParameter;                         //MMS ���ص�IBP�����Ĳ���
    int8u uDOWN_WVTotalNum;         //�緧����
    int8u uDOWN_On_Hour;            // ?���ƴ�����ʱ��--Сʱ ��Hex��
    int8u uDOWN_On_Minute;          // ?���ƴ�����ʱ��--���� ��Hex��
    int8u uDOWN_Off_Hour;           // ?���ƴ��ػ�ʱ��--Сʱ ��Hex��
    int8u uDOWN_Off_Minute;         // ?���ƴ��ػ�ʱ��--���� ��Hex��
    int8u uDOWN_WVNum_Opn;          //�緧ʵ��ȫ���������ڻ���ڴ���ֵʱ��BAS���ȫ�����緧��������0ʱ�˲�������Ч����Χ1-�緧����
    int8u uDOWN_WVNum_Cls;          //�緧ʵ��ȫ���������ڻ���ڴ���ֵʱ��BAS���ȫ�ء��緧��������0ʱ�˲�������Ч����Χ1-�緧����

    BIT8 uIbpAlarmParameter;                              //IBP�����Ĳ���

    int8u DownFlag;                                      //���ر�־��0--�ɹ�  1--ʧ��

}KDOWNIBPALARMPARAMETER;
EXTN KDOWNIBPALARMPARAMETER tagDownIbpAlarmParameter;

typedef struct tagKDCUSETTING                           //DCU �������
{
    int8u settingBUF[DCUsetting_LEN];

    int8u DownsettingBUF[DCUsetting_LEN];
    int8u ReadSettingBUF[DCUsetting_LEN];                //��FLASH ��ȡ��
    
    int8u DownFlag;                                      //���ر�־��0--�ɹ�  1--ʧ��
}KDCUSETTING;
EXTN  KDCUSETTING uDCUsetting;

typedef struct tagKDCUSTATE                               //DCU ״̬
{
    int8u aDCUOffLinejudgeTime[MAX_DCU];                   //����DCU ��ȡ״̬�����ߵĴ�������������趨���� ����������
    int8u aUpOneDcuStateBuf[ONE_DCU_STATE_LEN +1];         //��ȡһ��DCU״̬ ���BUF ,���һ�ֽ�ΪCANͨѶCRC��
    int8u uUpOneDcuStateTime;                              //��ȡ��һ��DCU״̬ ��С��ʱ�� ���
    int8u uID_DCU;                                         //��ȡ״̬��DCU
}KDCUSTATE;
EXTN KDCUSTATE tagDcuState;

//typedef struct tagKOFFLINEDCU                             //����DCU
//{
//  int8u uID_DCU;                                                       //��������DCU
//  int8u uTestDcuIfOnLineTime;                            //���Լ��
//  int8u aOffLineTestBuf[1+1];                            //�����õ�BUF ,���һ�ֽ�ΪCANͨѶCRC��
//
//}KOFFLINEDCU;
//EXTN KOFFLINEDCU tagOffLineDcu;

typedef struct tagKQUERY                                  //��ѯPEDC/DCU �����Ϣ
{
    int8u uID;                                             //Ҫ��ѯ�� ID (PEDC,DCU)

}KQUERY;
EXTN KQUERY takQUERY;

typedef struct tagKCANFUN
{
    BIT16 uwCANFUN_FLAG0;                                   //��־ 1 ��ʾ �иù���

    UNIDcuData        * ptagDcuData;                        //ָ�� DCU ���ݽṹָ��
   KUP_DCU_PARAMETER * ptagUpDcuParameter;                 //��ȡ DCU ����

}KCANFUN;

typedef struct                                             //��Ļ
{
    int8u  canbusUpLightScreenBuf[UPLIGHTSCREENLEN + 1];
    int8u  canbusDnLightScreenBuf[DNLIGHTSCREENLEN + 1];
}KLIGHTSCREEN;

EXTN KLIGHTSCREEN takLightScreen;

EXTN KCANFUN tagCANFUN;

/* 2022-4-8 ����: MMS �´� IP ������DCU */
#define IPPARA_TASK_IDLE        0   //0��ʾδִ��
#define IPPARA_TASK_START       1   //1��ʾ׼��ִ��
#define IPPARA_TASK_SENDING     2   //2��ʾ��������
#define IPPARA_TASK_WAITING     3   //3��ʾ�ȴ����
#define IPPARA_TASK_FINISHED    4   //4��ʾִ�����

#define DCU_SERVERIP_LEN        6   //������IP��ַ:�˿�
#define DCU_LOCALIP_LEN         4   //DCU����IP��ַ
#define DCU_NETMASK_LEN         4   //DCU������������
#define DCU_GATEWAY_LEN         4   //DCU��������
#define DCU_TOKEN_LEN           10  //Token(�Ự����)
#define DCU_IPSETTING_LEN       (DCU_LOCALIP_LEN + DCU_NETMASK_LEN + DCU_GATEWAY_LEN + DCU_TOKEN_LEN)
#define ONE_DCU_IPPARA_BUFLEN   (DCU_SERVERIP_LEN + DCU_IPSETTING_LEN)
typedef struct                                          //������ѡ�� DCU IP����
{
    int8u   downloadTask;                               //��������0��ʾδִ��
    int8u   downloadCnt;
    int8u   currentDCUID;                               //���ض���� DCU ID
    int32u  downloadStartTick;                          //�������������ʱ���
    
    int8u   downloadIPResult[8];                        //���ؽ����־
    
    /* �洢 MMS ������ 0x21 �е���Ϣ  */
    int8u   dcuObjects[8];                              //���ض���� DCU ���ϱ��
    int8u   ServerIPPort[6];                            //dcu IP �����е� SERVER 
    int8u   dcuIPToken[MAX_DCU][DCU_IPSETTING_LEN];     //����DCU�� IP����
}tDownloadDCU_IPPara;
EXTN tDownloadDCU_IPPara DownloadDCU_IPPara;
EXTN int8u  CANBus_downloadIPPara[ONE_DCU_IPPARA_BUFLEN + 1];   //ͨ��CAN�������صĲ��������һ�ֽ�ΪCANͨѶУ���ֽ�


/* 2022-4-8 ����: MMS ������ָ��DCU�� IP ���� */
typedef struct                                          //�ϴ�ָ���� DCU IP����
{
    int8u   uploadTask;                                 //�ϴ�����0��ʾδִ��
    int8u   uploadCnt;                                  //�ϴ��ɹ���DCU����
    int8u   currentDCUID;                               //�ϴ������ DCU ID
    int8u   uploadNextTime;                             //
    
    int32u  uploadStartTick;                            //������ȡ��ʱ���
    
    int8u   uploadDCUObjs[8];                           //��ȡ����� DCU ���ϱ��
    int8u   uploadIPResult[8];                          //��ȡ�����־
    int8u   allDCU_IPPara[MAX_DCU][ONE_DCU_IPPARA_BUFLEN];   //���α����ȡDCU IP����
    
    int8u   oneDCUIPPara[ONE_DCU_IPPARA_BUFLEN + 1];    //CAN ���߶�ȡ���� DCU IP���� ���λ�ã����һ�ֽ�ΪCANͨѶУ���ֽ�
}tUploadDCU_IPPara;
EXTN tUploadDCU_IPPara uploadDCU_IPPara;

/* =============================================================================
 * 2023-8-21 �������ܣ�����д DCU �г����� */
#define DCU_TRIPDATA_LEN        4   //DCU ���������
typedef struct                                          //������ѡ�� DCU ���������
{
    int8u   downloadTask;                               //��������0��ʾδִ��
    int8u   downloadCnt;
    int8u   currentDCUID;                               //���ض���� DCU ID
    int32u  downloadStartTick;                          //�������������ʱ���
    
    int8u   downloadTripResult[8];                      //���ؽ����־
    
    /* �洢 MMS ������ 0x23 �е���Ϣ  */
    int8u   dcuObjects[8];                              //���ض���� DCU ���ϱ��
    int8u   dcuTripData[DCU_TRIPDATA_LEN];              //����DCU�� �������
}tDownloadDCU_Trip;
EXTN tDownloadDCU_Trip DownloadDCU_Trip;
EXTN int8u  CANBus_downloadTrip[DCU_TRIPDATA_LEN + 1];  //ͨ��CAN��������������ݣ����һ�ֽ�ΪCANͨѶУ���ֽ�

typedef struct                                          //�ϴ�ָ���� DCU ���������
{
    int8u   uploadTask;                                 //�ϴ�����0��ʾδִ��
    int8u   uploadCnt;                                  //�ϴ��ɹ���DCU����
    int8u   currentDCUID;                               //�ϴ������ DCU ID
    int8u   uploadNextTime;                             //
    
    int32u  uploadStartTick;                            //������ȡ��ʱ���
    
    int8u   uploadDCUObjs[8];                           //��ȡ����� DCU ���ϱ��
    int8u   uploadTripResult[8];                        //��ȡ�����־
    int8u   allDCU_Trip[MAX_DCU][DCU_TRIPDATA_LEN];     //���α����ȡDCU ���������
    
    int8u   oneDCUTripData[DCU_TRIPDATA_LEN + 1];       //CAN ���߶�ȡ���� DCU ������� ���λ�ã����һ�ֽ�ΪCANͨѶУ���ֽ�
}tUploadDCU_Trip;
EXTN tUploadDCU_Trip uploadDCU_Trip;


EXTN int8u ReadDcuStateFailBuf[MAX_DCU];   //��ȡ����DCU 22�ֽڣ�ʧ��+1���ɹ�-1�� Ŀ���ǹ۲�CANͨѶ


//uwCANFUN_FLAG0; //��־ 1 ��ʾ �иù���
#define bFun_Up_WorkState                tagCANFUN.uwCANFUN_FLAG0.B0  //����״̬��������
#define bFun_Up_MoreDcuParameter         tagCANFUN.uwCANFUN_FLAG0.B1  //��������
#define bFun_Down_MoreDcuParameter       tagCANFUN.uwCANFUN_FLAG0.B2  //��������
#define bFun_Up_OpenDoorCurve            tagCANFUN.uwCANFUN_FLAG0.B3  //�����������ݻ�ȡ
#define bFun_Up_CloseDoorCurve           tagCANFUN.uwCANFUN_FLAG0.B4  //�����������ݻ�ȡ
#define bFun_UpOneDcuState               tagCANFUN.uwCANFUN_FLAG0.B5  //��ȡ���ߵ���DCU ״̬
#define bFun_TestDcuIfOnLine             tagCANFUN.uwCANFUN_FLAG0.B6  //��ȡ������ DCU,�ж��Ƿ�����
#define bFun_QUERY                       tagCANFUN.uwCANFUN_FLAG0.B7  //��ѯPEDC/DCU �����Ϣ
#define bFun_UpLightScreen               tagCANFUN.uwCANFUN_FLAG0.B8  //�ϴ� ��Ļ ��Ϣ
#define bFun_UpPEDCHardwareSetting       tagCANFUN.uwCANFUN_FLAG0.B9  //�ϴ� DCU λ������
#define bFun_DownPEDCHardwareSetting     tagCANFUN.uwCANFUN_FLAG0.B10 //���� DCU λ������

//��ȡ ����״̬
#define BusState_Down_TOSLVCOMMAND()     wtj_ChkTransState(FUNID_canbus_down_cmd)     //��ȡ MVB����������DCU ״̬
#define BusState_Up_MoreDcuParameter()   wtj_ChkTransState(FUNID_Up_MoreDcuParameter)
#define BusState_Down_MoreDcuParameter() wtj_ChkTransState(FUNID_Down_MoreDcuParameter)
#define BusState_Up_OpenDoorCurve()      wtj_ChkTransState(FUNID_Up_OpenDoorCurve)
#define BusState_Up_CloseDoorCurve()     wtj_ChkTransState(FUNID_Up_CloseDoorCurve)
#define BusState_UpOneDcuState()         wtj_ChkTransState(FUNID_UpOneDcuState)
#define BusState_TestDcuIfOnLine()       wtj_ChkTransState(FUNID_TestDcuIfOnLineDcu)
#define BusState_QUERY()                 wtj_ChkTransState(FUNID_QUERY)
#define BusState_UpLightScreen()         wtj_ChkTransState(FUNID_Up_LightScreen)

//���� ��������
#define Build_Down_TOSLVCOMMAND()        wtj_CapiStartTrans(FUNID_canbus_down_cmd, 0) //��MVB����������DCU
#define Build_UpMoreDcuPara(ID)          wtj_CapiStartTrans(FUNID_Up_MoreDcuParameter, ID)
#define Build_DownMoreDcuPara(ID)        wtj_CapiStartTrans(FUNID_Down_MoreDcuParameter, ID)
#define Build_UpOpenDoorCurve(ID)        wtj_CapiStartTrans(FUNID_Up_OpenDoorCurve, ID)
#define Build_UpCloseDoorCurve(ID)       wtj_CapiStartTrans(FUNID_Up_CloseDoorCurve, ID)
#define Build_UpOneDcuState(ID)          wtj_CapiStartTrans(FUNID_UpOneDcuState, ID)
#define Build_TestDcuIfOnLine(ID)        wtj_CapiStartTrans(FUNID_TestDcuIfOnLineDcu, ID)
#define Build_QUERY(ID)                  wtj_CapiStartTrans(FUNID_QUERY, ID)
#define Build_UpLightScreen()            wtj_CapiStartTrans(FUNID_Up_LightScreen, ID_MCP)
#define Build_DownLightScreen()          wtj_CapiStartTrans(FUNID_Down_LightScreen, ID_MCP)

//������� ״̬


//***********************************************************
EXTN void wtj_CapiStartTrans(int8u aFUNID_ReadOneDcuState, int8u ID);
EXTN int8u wtj_ChkTransState(int8u aFUNID_ReadOneDcuState);
EXTN void Can_Up_MoreDcuParameter(void);                              //������DCU ����
EXTN void Can_Down_MoreDcuParameter(void);                            //������ѡDCU����
EXTN void Can_UpOpenDoorCurve(void);                                  //����������
EXTN void Can_UpCloseDoorCurve(void);                                 //����������
EXTN void Can_UpOneDcuState(void);                                    //����������� DCU N �� ,����� �����߱�־
EXTN void Can_QUERY(void);                                            //��ѯDCU �����Ϣ
EXTN int8u ReadDcuStateFail(int8u id);                                //out: = true ��ʾ��������������÷�Χ�� = false ��ʾ������������÷�Χ֮��
EXTN void ClsReadDcuStateFailBuf(int8u id);                           //�������



/*******************************************************************************
 *                                                                             *
 * 2019-11-15                                                                  *
 * DCU-CAN V1.6                                                                *
 *                                                                             *
 * PEDC����DCU�� Inhibit��remote OPEN��remote CLOSE �����ͨ���㲥��ʽ����   *
 *                                                                             *
 ******************************************************************************/

/* �㲥��������ö��
 */
typedef enum
{
    BROAD_CMD_MMS_INHIBIT = 0,      /* inhibit ��Ϣ:  MMS          */
    BROAD_CMD_STC_INHIBIT,          /* inhibit ��Ϣ:  STC          */
    BROAD_CMD_CTRL_DOOR,            /* remote open or close door   */
    BROAD_CMD_CTRL_WIND,            /* ���緧/�ط緧               */
    /* �ڴ�����µ����� */
    
    /* ͬʱ֧�ֵĹ㲥����������˶�������������Զ����������������� */
    BROAD_CMD_NUM                   
}enmBroadCmdType;    


typedef union
{
    struct
    {
        int8u  opBit    : 3;   /* bit2-0��ָ�� bit4-3��Ҫ������DCU���������ֵ�ĳ��λ    */
        int8u  opCmd    : 2;   /* bit4-3: ��DCU���������ֽ��в���                       */
        int8u  exclude  : 1;   /* bit5  ������ָ���־���� bit4-3 = 0b11����λ��Ч      */
        int8u  dcuCmd   : 1;   /* bit6  ��ָ�� DCU����������                            */
        int8u  Valid    : 1;   /* bit7  ������������Ч��־, 1=valid                     */
    }bits;
    
    int8u byte;
}tPEDCOpCmd;

/* PEDC ͨ���㲥���� DCU ������ṹ
 *  - 8 �ֽڱ�־λ����Ӧ64��DCU
 *  - 3 �ֽڲ���������
 *  - 2 �ֽ�CRCУ��
 *  - 1 �ֽ�У��ͣ����ֽ��Ǽ�����ǰ��У�鷽ʽ
 */
#define BROADCOMMAND_LEN    8
typedef struct                                      
{
    int8u broadCmdBuf[BROADCOMMAND_LEN];
    tPEDCOpCmd oprCmd1;
    tPEDCOpCmd oprCmd2;
    tPEDCOpCmd oprCmd3;
    int8u CRCL;
    int8u CRCH;
    int8u checksum;
}tBroadCmdPkg;

typedef struct
{
    tBroadCmdPkg cmdFrame;          /* �㲥�����   */ 
    int8u   startSend;              /* �������ͱ�� */ 
    int8u   uRepeatSendTime;        /* ����2�η�����ͬ�������Сʱ������ʱ�䵥λ0.1s */
}tBroadCmd;

/* �㲥��������洢��
 * ÿ�ֹ㲥��������ֻ��һ���洢��
 */
EXTN tBroadCmd  broadCmdList[BROAD_CMD_NUM];


/* �㲥����͹������ */
typedef struct
{
    int8u  isBroading;                  /* 0=û��ִ�й㲥��������   */
    int8u  BroadingType;                /* ���ڹ㲥�������������   */
    int8u  Tx_NUM;                      /* ���ʹ���                 */
    int8u  Tx_intervals;                /* ���͵ļ��ʱ��           */
    int8u  setRepeatTimes;              /* �ظ����ʹ���             */
    int8u  setInterval_ms;              /* �ظ����ͼ��ʱ��         */
}tBroadCmdManage;

#define BROADCMD_INTERVAL_MS                100     /* �ظ����ͼ��ʱ��Ĭ��ֵ ms, <256   */
#define BROADCMD_REPEAT_TIMES               5       /* �ظ����ʹ���Ĭ��ֵ           */
EXTN tBroadCmdPkg       CANSendBroadCmdPkg;         /* ��ǰCAN���߷��͵Ĺ㲥����    */
EXTN tBroadCmdManage    CANSendBroadCmdManage;      /* �����͹㲥����             */


/* ================================================
 * ���� DCU ����������
 * 
 * @@@������  ע�� ������@@@
 * ��������Ҫ����ʵ����Ŀ��������ȷ��
 * 2020-4-24 ̫ԭ2
 * PEDCֻ�ܽ���MMS�������Ķ�λ���룬��MMSȷ����λ�����������Դ
 * DCU ʹ���� DCUCMD1_STC_INHIBIT
 */

#define DCUCMD1_STC_INHIBIT     0           /* DCU ����������1��STC Inhibit      */
#define DCUCMD1_OPN             1           /* DCU ����������1��open  */
#define DCUCMD1_CLS             2           /* DCU ����������1��close */
#define DCUCMD1_3               3
#define DCUCMD1_4               4
#define DCUCMD1_5               5
#define DCUCMD1_6               6
#define DCUCMD1_7               7

#define DCUCMD2_MMS_INHIBIT     0           /* DCU ����������2��MMS Inhibit */
#define DCUCMD2_OPN             1           /* DCU ����������2��open  */
#define DCUCMD2_CLS             2           /* DCU ����������2��close */
#define DCUCMD2_3               3
#define DCUCMD2_4               4
#define DCUCMD2_5               5
#define DCUCMD2_6               6
#define DCUCMD2_7               7


/* ================================================
 * 
 * 2020/3/23   DCU-CAN V1.7
 *  DCU-CAN V1.7������1.6��ȫһ�£�ֻ���޸������ֵ���������PEDC �����ʹ�ñ��ֲ���
 * 
 * DCU - CAN  V1.6
 * 2019-11-15 
 * ���� PEDC ����DCU���������ֵ�����ԣ�ʹDCU�������и�������÷�Χ
 *  */
/*
1. DCU����������
   DCU���ڴ�����2�������֣�PEDCͨ��CAN���߲���DCU����������
   
   - ������1(int8u) ����DCU�����Ժ���Զ����
     bit0 - S&TC Inhibit
     bit1 - Open Command
     bit2 - Close Command
     bit3 - ����
     bit4 - ����
     bit5 - ����
     bit6 - ����
     bit7 - ����

   - ������2(int8u) ����DCU�����Ժ󲻻��Զ������ֻ��ͨ���������
     bit0 - MMS Inhibit
     bit1 - Open Command
     bit2 - Close Command
     bit3 - ����
     bit4 - ����
     bit5 - ����
     bit6 - ����
     bit7 - ����
     
    - ����λ���⹦��
     DCU����������1��2֧��λ���������ÿ2��λΪһ�飬��������һλ����һ��λ��ǿ����������λ��Ӱ����һ��λ
     Bit 1&2 Ϊһ�飬֧��λ�������
     Bit 3&4 Ϊһ�飬֧��λ�������
     Bit 5&6 Ϊһ�飬֧��λ�������

2.  PEDC �� DCU ͨ��Э��
2.1 PEDCͨ���㲥���������DCU����������

    �������ʽ��
    ��14���ֽڣ��ֳ�2��CAN����֡���͡�
    ��ӦDCU��λ(8Bytes) + ��������1(1Byte) + ��������2(1Byte)  + ��������3(1Byte) + CRC(2Bytes) + �ۼӺ�(1Byte)
 
 *  ע�⣺3������������Էֱ���� DCU���������֣�DCUִ��ʱ�� ��������1��3����ִ�У�
 *        ������ͬһ��λ���ֽڣ���ҪPEDC����֤���������ֵĳ�ͻ���
    
2.2 ���������
2.2.1   ��ӦDCU��λ 
        8 �ֽ�: ÿһ��bit��Ӧһ��DCU�����֧��64��DCU
2.2.2   ��������
        3�������������ͬʱ����3��λ������ͬʱ�� DCU����������(1 & 2)����λ����
     
        3����������ĸ�ʽͳһ�����£�
        bit7  : ����������Ч��־
            0=������������Ч
            1=������������Ч
            
        bit6  : ָ�� DCU����������
            0=DCU����������1
            1=DCU����������2
        
        
        bit5  : ����ָ���־���� bit4-3 = 0b11����λ��Ч
            0����ʾ����λ����
            1����ʾ����λ�����������������е���3�黥��λ������������һλ��Ȼ�����һλ�������������һλ��Ӱ����һλ
             
        bit4-3: ��DCU���������ֽ��в���
            0=λ���ƣ�����8�ֽ��еĶ�Ӧ��DCU��λ���������Ƶ����������ֵ�ָ��λ��ע�⴦����λ��
                      ����������ͬʱд����DCU��ͬ��������ú������
                      ����λ���������Ƶ�λ
            1=λ���ã�����8�ֽ��еĶ�Ӧ��DCU��λΪ1ʱ�� ���ñ��������ֵ�ָ��λ��ע�⴦����λ
            2=λ���������8�ֽ��еĶ�Ӧ��DCU��λΪ1ʱ�� ������������ֵ�ָ��λ
            3=�ֽڲ�����ǿ�����ָ���� 'DCU����������'
                
        bit2-0��ָ�� bit4-3��Ҫ������DCU���������ֵ�ĳ��λ
 */

/* bit7  ������������Ч��־ */
#define OPCMD_INVALID       (0 << 7)
#define OPCMD_VALID         (1 << 7)

/* bit6  ��ָ�� DCU���������� */
#define OP_DCUCMD_1         (0 << 6)
#define OP_DCUCMD_2         (1 << 6)

/* bit5  ������ָ���־���� bit4-3 = 0b11����λ��Ч */
#define OP_BIT_NORMAL       (0 << 5)
#define OP_BIT_EXCLUDE      (1 << 5)

/* bit4-3����DCU���������ֽ��в��� */
#define CMD_BIT_CPY         (0x00 << 3)                 /* λ���� */
#define CMD_BIT_SET         (0x01 << 3)                 /* λ���� */
#define CMD_BIT_CLR         (0x02 << 3)                 /* λ��� */
#define CMD_BYTE_CLR        (0x03 << 3)                 /* �ֽڲ�������� */


#define STC_INHIBIT_ENABLE              (DCUCMD1_STC_INHIBIT | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | OPCMD_VALID)
#define MMS_INHIBIT_ENABLE              (DCUCMD2_MMS_INHIBIT | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_SET | OPCMD_VALID)
#define MMS_INHIBIT_DISABLE             (DCUCMD2_MMS_INHIBIT | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_CLR | OPCMD_VALID)

#define CREPEATSEND_SEC     5           /* ��ͬ����͵�ʱ����, seconds */
#if (CREPEATSEND_SEC > 26 )
#error "CREPEATSEND_SEC must be less than 26"
#endif

/* ================================================ 
 * ���͹㲥����Ľӿں���
 */
/* ��Ҫ������������������ */
PUBLIC void procBroadcastTask(void);

/* ��ӹ㲥�������� 
 * addBroadCmdTask_AtOnce  ==> ��������
 * addBroadCmdTask         ==> �����ͬ����ķ���ʱ���������Ƿ��ظ�����
 */
PUBLIC BOOL addBroadCmdTask(tBroadCmdPkg * command, enmBroadCmdType cmdType);
PUBLIC BOOL addBroadCmdTask_AtOnce(tBroadCmdPkg * command, enmBroadCmdType cmdType);

/* ���÷��͹㲥����Ĳ��������û�����ã������Ĭ��ֵ */
PUBLIC void setRepeatPara(int8u repeatTimes, int8u repeatInterval);

#endif

PUBLIC void processCANComm(void);


