

#ifndef _MMS_APP_H
#define _MMS_APP_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

#define UART2_TX_FILEMESSAGE_LEN    11      //uart --> mms ʱ�����ļ���Ϣ����

#define UARTDLE        0x10
#define UARTSTX        0x02
#define UARTETX        0x03

//������	�ļ�ͷDLE	�ļ�ͷSTX	˳���SEQ	Դ��ַ	Ŀ���ַ	��������(HEX)	���ݳ���:Len		��������	�ļ�βDLE	�ļ�βETX	У���
#define UART_DLE_SEAT      0
#define UART_STX_SEAT      1
#define UART_SEQ_SEAT      2
#define UARTSOURCEADD_SEAT 3
#define UARTOBJ_SEAT       4
#define UARTDATASORT_SEAT  5
#define UARTLENH_SEAT      6
#define UARTLENL_SEAT      7
#define UARTDATASEAT       8      //����λ��ʼλ��
//--------------------------------------
#define TEST_FRAME_END_CC   0xCC   //pedc Ҫ���Ͳ��Դ���
//
//***********************************************************
//UART2 Tx �������λ ,1 ��ʾ�д�������
EXTN BIT16 UART2_TX_Cnt;
#define bUART2_CrcErr                    UART2_TX_Cnt.B0
#define bUART2_Up_WorkState              UART2_TX_Cnt.B1
#define bUART2_UpMoreDcuParaOK           UART2_TX_Cnt.B2
#define bUART2_DownMoreDcuParaOK         UART2_TX_Cnt.B3
#define bUART2_UpOpenDoorCurveOK         UART2_TX_Cnt.B4
#define bUART2_UpOpenDoorCurveFail       UART2_TX_Cnt.B5
#define bUART2_UpCloseDoorCurveOK        UART2_TX_Cnt.B6
#define bUART2_UpCloseDoorCurveFail      UART2_TX_Cnt.B7
#define bUART2_Up_IbpAlarmParameter      UART2_TX_Cnt.B8
#define bUART2_Down_IbpAlarmParameter    UART2_TX_Cnt.B9
#define bUART2_UPDATE_FIRMWARE           UART2_TX_Cnt.B10
#define bUART2_QueryOK                   UART2_TX_Cnt.B11
#define bUART2_QueryFAIL                 UART2_TX_Cnt.B12
#define bUART2_UpPEDCHardwareSetting     UART2_TX_Cnt.B13
#define bUART2_DownPEDCHardwareSetting   UART2_TX_Cnt.B14
#define bUART2_InhibitDoor               UART2_TX_Cnt.B15

#define  cUART_MMS_TX_NUM               20      //1 = 1 ,uart2 tx ���������

//***********************************************************
EXTN BIT8 DataFLAG;
#define bClsAvrDataBuf     DataFLAG.B0 //1 ���uniDcuData.sortBuf.aAvrDataBuf ����


//***********************************************************
#define SOURE_MMS        0
#define SOURE_PEDC       200
#define SOURE_BROADCAST  0xff                      //�㲥
#define OBJECT_MMS       0
#define OBJECT_PEDC      200
#define OBJECT_BROADCAST 0xff                      //�㲥
#define USE_DATALEN      0xffff                    //ʹ�� wDataLen ����ʾ�ĳ���

EXTN int8u uSource;
EXTN int8u uObject;
EXTN int16u wDataLen;    //������Uart2_protocol[]�е����ݳ��� == 0xffff ,�� ʹ�� wDataLen ����ʾ�ĳ���

typedef  struct tagKPEDC_UART2_TO_MMS
{
	int8u  FileHead_DLE;  //�ļ�ͷDLE
	int8u  FileHead_STX;  //�ļ�ͷSTX
	int8u  Sequence;      //˳���
	pINT8U Source;        //Դ��ַ ָ��
	pINT8U Object;        //Ŀ���ַ ָ��
	int8u  DataSORT;      //��������
	int16u Data_Len;      //���ݳ���:Len
	pINT8U DataContentSP; //��������
	int8u  FileEnd_DLE;   //�ļ�βDLE
	int8u  FileEnd_ETX;   //�ļ�βETX
}KPEDC_UART2_TO_MMS;



enum enmPedcReplyMMS
{
    cUp_WorkState_protocol_No          = 0  ,   //0  Uart2_protocol[] �е�λ������
    cUpMoreDcuParameter_protocol_No         ,   //1  �ϴ��������ݻظ�
    cDownMoreDcuParameter_protocol_No       ,   //2  ���ز������ݻظ�
    cUpOpenDoorCurveOK_protocol_No          ,   //3  �����������ݻظ�
    cUpOpenDoorCurveFAIL_protocol_No        ,   //4  �����������ݾܾ�
    cUpCloseDoorCurveOK_protocol_No         ,   //5  �����������ݻظ�
    cUpCloseDoorCurveFAIL_protocol_No       ,   //6  �����������ݾܾ�
    cUp_IbpAlarmParameter_protocol_No       ,   //7  ibp���������ϴ��ظ�
    cDown_IbpAlarmParameterOK_protocol_No   ,   //8  ibp�����������سɹ�
    cDown_IbpAlarmParameterFAIL_protocol_No ,   //9  ibp�����������ؾܾ�
    cUpgradeState_protocol_No               ,   //10 ��������״̬
    cQUERYOK                                ,   //11 ��ѯ�����Ϣ�ɹ�
    cQUERYFAIL                              ,   //12 ��ѯ�����Ϣʧ��
    cUp_PEDCHardwareSetting_No              ,   //13 �ϴ�DCU λ������
    cDown_PEDCHardwareSetting_No            ,   //14 ����DCU λ�����ûظ�
    cUP_MMS_inhibit_No                      ,   //15 MMS inhibit �ظ�
                                                
    cUP_UPLOAD_IPPARA_NO                    ,   //16 �ظ� �ϴ� DCU IP����
    cUP_DOWNLOAD_IPPARA_NO                  ,   //17 �ظ� ���� DCU IP����
    cUP_UPLOAD_DCUTRIP_NO                   ,   //18 2023-8-21 �ظ� �ϴ� DCU �������
    cUP_DOWNLOAD_DCUTRIP_NO                 ,   //19 2023-8-21 �ظ� ���� DCU �������
    REPLY_MMS_TOTAL_TASKS,
};


//***********************************************************
//UART2 ����(PEDC -- MMS)�����ж�
//MMS-PEDC UART ͨѶ��ʱ �� ����״̬ MMS ÿ�� 300 ms ���� �� PEDC ������ ��PEDC ��Ӧ ������
//ͬʱ MMS �� 3 ��ĵȴ���ʱ�� �� PEDC ������ ���� MMS û�� ����� ����ʱ�䳬�� 3 �룬
//֤��	UART ���߳���

#define cMMS_PEDC_UART_OverTime		3000            //  ms

EXTN BIT8 UP_PEDC;
#define SF_FAULT               UP_PEDC.B0        //��ȫ�̵������ϣ�K31_K32�˵�ѹ �� K33���� ����������

//***********************************************************
#define UART2_SOURCE_00 ((pINT8U) &uSource)
#define UART2_OBJECT_00 ((pINT8U) &uObject)
#define UART2_LEN_00    USE_DATALEN //(AVR_DATA_LEN + DCU_CAN12_STATE_LEN + ALL_DCU_STATE_LEN)
#define UART2_Adr_00    ((pINT8U) &uniDcuData.aDcuData)

#define UART2_SOURCE_01 ((pINT8U) &uSource)
#define UART2_OBJECT_01 ((pINT8U) &uObject)
#define UART2_LEN_01    USE_DATALEN //(defPARAMETER_FLAG_LEN + ALL_DCU_PARA_LEN)
#define UART2_Adr_01    ((pINT8U) &tagUpDcuParameter.aUpDcuParameterResult)

#define UART2_SOURCE_02 ((pINT8U) &uSource)
#define UART2_OBJECT_02 ((pINT8U) &uObject)
#define UART2_LEN_02    defPARAMETER_FLAG_LEN
#define UART2_Adr_02    ((pINT8U) &tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterResult)

#define UART2_SOURCE_03 ((pINT8U) &tagUPOPENDOORCURVE.uUpOpenDoorCurve_IdDcu)
#define UART2_OBJECT_03 ((pINT8U) &uObject)
#define UART2_LEN_03    CURVE_LEN
#define UART2_Adr_03    ((pINT8U) &tagUPOPENDOORCURVE.aUpOpenDoorCurveBuf)

#define UART2_SOURCE_04 ((pINT8U) &tagUPOPENDOORCURVE.uUpOpenDoorCurve_IdDcu)
#define UART2_OBJECT_04 ((pINT8U) &uObject)
#define UART2_LEN_04    0
#define UART2_Adr_04    ((pINT8U) &tagUPOPENDOORCURVE.aUpOpenDoorCurveBuf)

#define UART2_SOURCE_05 ((pINT8U) &tagUPCLOSEDOORCURVE.uUpCloseDoorCurve_IdDcu)
#define UART2_OBJECT_05 ((pINT8U) &uObject)
#define UART2_LEN_05    CURVE_LEN
#define UART2_Adr_05    ((pINT8U) &tagUPCLOSEDOORCURVE.aUpCloseDoorCurveBuf)

#define UART2_SOURCE_06 ((pINT8U) &tagUPCLOSEDOORCURVE.uUpCloseDoorCurve_IdDcu)
#define UART2_OBJECT_06 ((pINT8U) &uObject)
#define UART2_LEN_06    0
#define UART2_Adr_06    ((pINT8U) &tagUPCLOSEDOORCURVE.aUpCloseDoorCurveBuf)

#define UART2_SOURCE_07 ((pINT8U) &uSource)
#define UART2_OBJECT_07 ((pINT8U) &uObject)
#define UART2_LEN_07    IBPALARM_LEN
#define UART2_Adr_07    ((pINT8U) &tagDownIbpAlarmParameter.uRead_IbpAlarmParameter)

#define UART2_SOURCE_08 ((pINT8U) &uSource)
#define UART2_OBJECT_08 ((pINT8U) &uObject)
#define UART2_LEN_08    1
#define UART2_Adr_08    ((pINT8U) &tagDownIbpAlarmParameter.DownFlag)

#define UART2_SOURCE_09 ((pINT8U) &uSource)
#define UART2_OBJECT_09 ((pINT8U) &uObject)
#define UART2_LEN_09    0
#define UART2_Adr_09    ((pINT8U) &uSource)

#define UART2_SOURCE_0A ((pINT8U) &MMS_OP_DST_ID)
#define UART2_OBJECT_0A ((pINT8U) &uObject)
#define UART2_LEN_0A    1
#define UART2_Adr_0A    ((pINT8U) &u8UpdateState)

#define UART2_SOURCE_0B ((pINT8U) &takQUERY.uID)
#define UART2_OBJECT_0B ((pINT8U) &uObject)
#define UART2_LEN_0B    (BYTES_INFO)
#define UART2_Adr_0B    ((pINT8U) &DeviceInfo)

#define UART2_SOURCE_0C ((pINT8U) &takQUERY.uID)
#define UART2_OBJECT_0C ((pINT8U) &uObject)
#define UART2_LEN_0C    0
#define UART2_Adr_0C    ((pINT8U) &uSource)

#define UART2_SOURCE_0D ((pINT8U) &uSource)
#define UART2_OBJECT_0D ((pINT8U) &uObject)
#define UART2_LEN_0D    8
#define UART2_Adr_0D    ((pINT8U) &uDCUsetting.ReadSettingBUF)

#define UART2_SOURCE_0E ((pINT8U) &uSource)
#define UART2_OBJECT_0E ((pINT8U) &uObject)
#define UART2_LEN_0E    1
#define UART2_Adr_0E    ((pINT8U) &uDCUsetting.DownFlag)

#define UART2_SOURCE_0F ((pINT8U) &uSource)
#define UART2_OBJECT_0F ((pINT8U) &uObject)
#define UART2_LEN_0F    sizeof(TX_MMS_INHIBIT_MESSAGE)
#define UART2_Adr_0F    ((pINT8U) &TX_MMS_INHIBIT_MESSAGE)

//***********************************************************
PUBLIC void initMMSModule(void);

PUBLIC int8u getUartMMSLinkSts(void);


#endif

