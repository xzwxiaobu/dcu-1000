

#ifndef _MMS_APP_H
#define _MMS_APP_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

#define UART2_TX_FILEMESSAGE_LEN    11      //uart --> mms 时各种文件信息长度

#define UARTDLE        0x10
#define UARTSTX        0x02
#define UARTETX        0x03

//域名称	文件头DLE	文件头STX	顺序号SEQ	源地址	目标地址	数据类型(HEX)	数据长度:Len		数据内容	文件尾DLE	文件尾ETX	校验和
#define UART_DLE_SEAT      0
#define UART_STX_SEAT      1
#define UART_SEQ_SEAT      2
#define UARTSOURCEADD_SEAT 3
#define UARTOBJ_SEAT       4
#define UARTDATASORT_SEAT  5
#define UARTLENH_SEAT      6
#define UARTLENL_SEAT      7
#define UARTDATASEAT       8      //数据位开始位置
//--------------------------------------
#define TEST_FRAME_END_CC   0xCC   //pedc 要求发送测试代码
//
//***********************************************************
//UART2 Tx 传输控制位 ,1 表示有传输任务
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

#define  cUART_MMS_TX_NUM               20      //1 = 1 ,uart2 tx 种类最大数

//***********************************************************
EXTN BIT8 DataFLAG;
#define bClsAvrDataBuf     DataFLAG.B0 //1 清除uniDcuData.sortBuf.aAvrDataBuf 数据


//***********************************************************
#define SOURE_MMS        0
#define SOURE_PEDC       200
#define SOURE_BROADCAST  0xff                      //广播
#define OBJECT_MMS       0
#define OBJECT_PEDC      200
#define OBJECT_BROADCAST 0xff                      //广播
#define USE_DATALEN      0xffff                    //使用 wDataLen 所表示的长度

EXTN int8u uSource;
EXTN int8u uObject;
EXTN int16u wDataLen;    //如果表格Uart2_protocol[]中的数据长度 == 0xffff ,则 使用 wDataLen 所表示的长度

typedef  struct tagKPEDC_UART2_TO_MMS
{
	int8u  FileHead_DLE;  //文件头DLE
	int8u  FileHead_STX;  //文件头STX
	int8u  Sequence;      //顺序号
	pINT8U Source;        //源地址 指针
	pINT8U Object;        //目标地址 指针
	int8u  DataSORT;      //数据类型
	int16u Data_Len;      //数据长度:Len
	pINT8U DataContentSP; //数据内容
	int8u  FileEnd_DLE;   //文件尾DLE
	int8u  FileEnd_ETX;   //文件尾ETX
}KPEDC_UART2_TO_MMS;



enum enmPedcReplyMMS
{
    cUp_WorkState_protocol_No          = 0  ,   //0  Uart2_protocol[] 中的位置排列
    cUpMoreDcuParameter_protocol_No         ,   //1  上传参数数据回复
    cDownMoreDcuParameter_protocol_No       ,   //2  下载参数数据回复
    cUpOpenDoorCurveOK_protocol_No          ,   //3  开门曲线数据回复
    cUpOpenDoorCurveFAIL_protocol_No        ,   //4  开门曲线数据拒绝
    cUpCloseDoorCurveOK_protocol_No         ,   //5  开门曲线数据回复
    cUpCloseDoorCurveFAIL_protocol_No       ,   //6  开门曲线数据拒绝
    cUp_IbpAlarmParameter_protocol_No       ,   //7  ibp报警参数上传回复
    cDown_IbpAlarmParameterOK_protocol_No   ,   //8  ibp报警参数下载成功
    cDown_IbpAlarmParameterFAIL_protocol_No ,   //9  ibp报警参数下载拒绝
    cUpgradeState_protocol_No               ,   //10 返回升级状态
    cQUERYOK                                ,   //11 查询软件信息成功
    cQUERYFAIL                              ,   //12 查询软件信息失败
    cUp_PEDCHardwareSetting_No              ,   //13 上传DCU 位置设置
    cDown_PEDCHardwareSetting_No            ,   //14 下载DCU 位置设置回复
    cUP_MMS_inhibit_No                      ,   //15 MMS inhibit 回复
                                                
    cUP_UPLOAD_IPPARA_NO                    ,   //16 回复 上传 DCU IP参数
    cUP_DOWNLOAD_IPPARA_NO                  ,   //17 回复 下载 DCU IP参数
    cUP_UPLOAD_DCUTRIP_NO                   ,   //18 2023-8-21 回复 上传 DCU 里程数据
    cUP_DOWNLOAD_DCUTRIP_NO                 ,   //19 2023-8-21 回复 下载 DCU 里程数据
    REPLY_MMS_TOTAL_TASKS,
};


//***********************************************************
//UART2 总线(PEDC -- MMS)出错判断
//MMS-PEDC UART 通讯超时 ： 正常状态 MMS 每隔 300 ms 主动 向 PEDC 发请求 ，PEDC 回应 该请求
//同时 MMS 有 3 秒的等待超时错 ； PEDC 方面如 发现 MMS 没有 请求或 请求时间超过 3 秒，
//证明	UART 总线出错

#define cMMS_PEDC_UART_OverTime		3000            //  ms

EXTN BIT8 UP_PEDC;
#define SF_FAULT               UP_PEDC.B0        //安全继电器故障（K31_K32端电压 和 K33触点 来回跳动）

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

