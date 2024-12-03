
#ifndef _CAN_H
#define _CAN_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif


#define defOFFLINEJUDGETIME                4         //在线 DCU 登记连续离线次数 ，如超过该次数 则认为离线，作离线登记
#define defSTATEREADERR_CONTINUEREAD 2         //如果读取错误则会连读次数,该数要小于 c_OffLinejudgeTime
#define defUPONEDCUSTATETIME         8         // 8ms  ，读取下一个 DCU 状态的时间间隔

#define BITEQU1                      1
#define BITEQU0                      0

#define cDcuOnLineF                  1           //在线标志
#define cDcuOffLineF                 0           //离线

#define FindOk                       1
#define FindFail                     0

#define ican_read_OK                 fgTransOK   //读ok
#define ican_write_OK                fgTransOK   //写ok
#define ican_read_Err                fgAbort     //读err
#define ican_write_Err               fgAbort     //写rrr
#define ican_read_ing                fgRunning   //读进行中
#define ican_write_ing               fgRunning   //写进行中

#define DCU_cfg_LEN                  8                         //定义存在DCU 对应的BUF
#define DCU_ONECAN_ONLINE_LEN        8                         //每条CAN总线对应各个DCU在线标志
#define MCP_OPENCLOSE_DOOR_LEN       24                        //MCP 对 DCU 的开关门命令
#define AVR_DATA_RELAY_LEN           20                        //继电器状态（开关，故障）
#define AVR_DATA_CMD_POW_LEN         16                        //SIG,PSL,IBP,PSC等命令,电源故障
#define AVR_DATA_LEN                 (AVR_DATA_RELAY_LEN + AVR_DATA_CMD_POW_LEN) //avr 传输的数据长度36
#define DCU_CAN12_STATE_LEN          (DCU_ONECAN_ONLINE_LEN*2) //DCU CAN12 信息登记， 1 表示通讯错误
#define ALL_DCU_STATE_LEN            (ONE_DCU_STATE_LEN * MAX_DCU)
#define ALL_DCU_PARA_LEN             (ONE_DCU_PARA_LEN * MAX_DCU)
#define UPLIGHTSCREENLEN             10
#define DNLIGHTSCREENLEN             10
#define BYTES_SYSTIME_TODCU          7      //发送系统时间给DCU

#define BYTES_PER_RELAYSLIFE        4       /* 每个继电器寿命的长度(字节)   */
#define TOTAL_RELAYS_LOGPCB         21      /* 逻辑控制板的继电器数量       */
#define NUM_OF_LOGPCB               2       /* 项目中使用逻辑控制板的数量   */
#define TOTAL_RELAYSLIFE_LEN        (BYTES_PER_RELAYSLIFE * TOTAL_RELAYS_LOGPCB * NUM_OF_LOGPCB)
                                      
#define AVR_LEN                      (DCU_cfg_LEN + DCU_CAN12_STATE_LEN + AVR_DATA_LEN + MCP_OPENCLOSE_DOOR_LEN + TOTAL_RELAYSLIFE_LEN) 
#define AVR_ONLINE_ALLDCUSTATE_LEN   (AVR_LEN + ALL_DCU_STATE_LEN)

typedef  union                               //对应协议
{
    struct
    {
        int8u aDcuCfgBUF[DCU_cfg_LEN];
        int8u aDcuCan12State[DCU_CAN12_STATE_LEN];
        int8u aAvrDataBuf[AVR_DATA_LEN];
        int8u aMcpOpenCloseDoorCommand[MCP_OPENCLOSE_DOOR_LEN];
        int8u aRelayLifes[NUM_OF_LOGPCB][BYTES_PER_RELAYSLIFE * TOTAL_RELAYS_LOGPCB];    /* 继电器寿命 */
        int8u aDcuState[MAX_DCU][ONE_DCU_STATE_LEN];
    }sortBuf;

    int8u aDcuData[AVR_ONLINE_ALLDCUSTATE_LEN];

}UNIDcuData;
EXTN UNIDcuData uniDcuData;

//typedef struct tagKUP_DCU_STATE
//{
////    int8u uUp_State_DcuNum;                             //须上传状态的DCU个数
//
//}KUP_DCU_STATE;
//EXTN KUP_DCU_STATE tagUP_DCU_STATE;

#define defPARAMETER_FLAG_LEN (DCU_ONECAN_ONLINE_LEN)
typedef struct tagKUP_MORE_DCU_PARAMETER                //上传所选的 DCU 参数
{
    int8u aUpDcuParameterRequest[defPARAMETER_FLAG_LEN]; //请求的DCU
    int8u aUpDcuParameterResult[defPARAMETER_FLAG_LEN];  //读取结果
    int8u aAllDcuPara[MAX_DCU][ONE_DCU_PARA_LEN];        //依次保存读取DCU 参数，（该保存的位置依次和需要上传的DCU位置一致，不上传的DCU不占位置）
    int8u uUpDcuParameter_IdDcu;                         //现读取的 DCU ID
    int8u aUpDcuParameterBuf[ONE_DCU_PARA_LEN + 1];      //CAN 总线读取单个 DCU参数 存放位置，最后一字节为CAN通讯CRC用
    int8u aUpDcuParameter_DcuNum;                        //读取参数的DCU总数; 统计要读的DCU的总数

}KUP_DCU_PARAMETER;
EXTN KUP_DCU_PARAMETER tagUpDcuParameter;

typedef struct tagKDOWN_MORE_DCU_PARAMETER               //下载所选的 DCU 参数
{
    int8u aDownDcuParameterRequest[defPARAMETER_FLAG_LEN];//请求的DCU
    int8u aDownDcuParameterResult[defPARAMETER_FLAG_LEN]; //下载结果
    int8u uDownDcuParameter_IdDcu;                        //现下载的 DCU ID
    int8u aDownParameterBuf[ONE_DCU_PARA_LEN + 1];        //下载的参数
}KDOWN_MORE_DCU_PARAMETER;
EXTN KDOWN_MORE_DCU_PARAMETER tagDOWN_MORE_DCU_PARAMETER;

typedef struct tagKUPOPENDOORCURVE                        //开门曲线数据获取
{
    int8u aUpOpenDoorCurveBuf[CURVE_LEN + 1];              //曲线数据, DCU参数 存放位置，最后一字节为CAN通讯CRC用
    int8u uUpOpenDoorCurve_IdDcu;                          //读取开门曲线的 DCU
}KUPOPENDOORCURVE;
EXTN KUPOPENDOORCURVE tagUPOPENDOORCURVE;

typedef struct tagKUPCLOSEDOORCURVE                       //关门曲线数据获取
{
    int8u aUpCloseDoorCurveBuf[CURVE_LEN + 1];             //曲线数据
    int8u uUpCloseDoorCurve_IdDcu;                         //读关门曲线的 DCU
}KUPCLOSEDOORCURVE;
EXTN KUPCLOSEDOORCURVE tagUPCLOSEDOORCURVE;

typedef struct tagKDOWNIBPALARMPARAMETER                  //ibp报警参数下载请求
{
    int8u uRead_IbpAlarmParameter;                         //将读取 FLASH 的数据存储位置
    int8u uRead_WVTotalNum;         //风阀总数( 默认DCU数量 )
    int8u uRead_On_Hour;            // ?望灯带开机时间--小时 （Hex）
    int8u uRead_On_Minute;          // ?望灯带开机时间--分钟 （Hex）
    int8u uRead_Off_Hour;           // ?望灯带关机时间--小时 （Hex）
    int8u uRead_Off_Minute;         // ?望灯带关机时间--分钟 （Hex）
    int8u uRead_WVNum_Opn;          //风阀实际全开数量大于或等于此数值时向BAS输出全开。风阀总数大于0时此参数才有效，范围1-风阀总数
    int8u uRead_WVNum_Cls;          //风阀实际全开数量大于或等于此数值时向BAS输出全关。风阀总数大于0时此参数才有效，范围1-风阀总数

    int8u uDOWN_IbpAlarmParameter;                         //MMS 下载的IBP报警的参数
    int8u uDOWN_WVTotalNum;         //风阀总数
    int8u uDOWN_On_Hour;            // ?望灯带开机时间--小时 （Hex）
    int8u uDOWN_On_Minute;          // ?望灯带开机时间--分钟 （Hex）
    int8u uDOWN_Off_Hour;           // ?望灯带关机时间--小时 （Hex）
    int8u uDOWN_Off_Minute;         // ?望灯带关机时间--分钟 （Hex）
    int8u uDOWN_WVNum_Opn;          //风阀实际全开数量大于或等于此数值时向BAS输出全开。风阀总数大于0时此参数才有效，范围1-风阀总数
    int8u uDOWN_WVNum_Cls;          //风阀实际全开数量大于或等于此数值时向BAS输出全关。风阀总数大于0时此参数才有效，范围1-风阀总数

    BIT8 uIbpAlarmParameter;                              //IBP报警的参数

    int8u DownFlag;                                      //下载标志，0--成功  1--失败

}KDOWNIBPALARMPARAMETER;
EXTN KDOWNIBPALARMPARAMETER tagDownIbpAlarmParameter;

typedef struct tagKDCUSETTING                           //DCU 序号设置
{
    int8u settingBUF[DCUsetting_LEN];

    int8u DownsettingBUF[DCUsetting_LEN];
    int8u ReadSettingBUF[DCUsetting_LEN];                //从FLASH 读取的
    
    int8u DownFlag;                                      //下载标志，0--成功  1--失败
}KDCUSETTING;
EXTN  KDCUSETTING uDCUsetting;

typedef struct tagKDCUSTATE                               //DCU 状态
{
    int8u aDCUOffLinejudgeTime[MAX_DCU];                   //在线DCU 读取状态是离线的次数，如果超过设定次数 则设置离线
    int8u aUpOneDcuStateBuf[ONE_DCU_STATE_LEN +1];         //读取一个DCU状态 存放BUF ,最后一字节为CAN通讯CRC用
    int8u uUpOneDcuStateTime;                              //读取下一个DCU状态 最小的时间 间隔
    int8u uID_DCU;                                         //读取状态的DCU
}KDCUSTATE;
EXTN KDCUSTATE tagDcuState;

//typedef struct tagKOFFLINEDCU                             //离线DCU
//{
//  int8u uID_DCU;                                                       //测试离线DCU
//  int8u uTestDcuIfOnLineTime;                            //测试间隔
//  int8u aOffLineTestBuf[1+1];                            //测试用的BUF ,最后一字节为CAN通讯CRC用
//
//}KOFFLINEDCU;
//EXTN KOFFLINEDCU tagOffLineDcu;

typedef struct tagKQUERY                                  //查询PEDC/DCU 软件信息
{
    int8u uID;                                             //要查询的 ID (PEDC,DCU)

}KQUERY;
EXTN KQUERY takQUERY;

typedef struct tagKCANFUN
{
    BIT16 uwCANFUN_FLAG0;                                   //标志 1 表示 有该功能

    UNIDcuData        * ptagDcuData;                        //指向 DCU 数据结构指针
   KUP_DCU_PARAMETER * ptagUpDcuParameter;                 //读取 DCU 参数

}KCANFUN;

typedef struct                                             //光幕
{
    int8u  canbusUpLightScreenBuf[UPLIGHTSCREENLEN + 1];
    int8u  canbusDnLightScreenBuf[DNLIGHTSCREENLEN + 1];
}KLIGHTSCREEN;

EXTN KLIGHTSCREEN takLightScreen;

EXTN KCANFUN tagCANFUN;

/* 2022-4-8 新增: MMS 下传 IP 参数给DCU */
#define IPPARA_TASK_IDLE        0   //0表示未执行
#define IPPARA_TASK_START       1   //1表示准备执行
#define IPPARA_TASK_SENDING     2   //2表示发送命令
#define IPPARA_TASK_WAITING     3   //3表示等待结果
#define IPPARA_TASK_FINISHED    4   //4表示执行完毕

#define DCU_SERVERIP_LEN        6   //服务器IP地址:端口
#define DCU_LOCALIP_LEN         4   //DCU本地IP地址
#define DCU_NETMASK_LEN         4   //DCU本地子网掩码
#define DCU_GATEWAY_LEN         4   //DCU本地网关
#define DCU_TOKEN_LEN           10  //Token(会话主题)
#define DCU_IPSETTING_LEN       (DCU_LOCALIP_LEN + DCU_NETMASK_LEN + DCU_GATEWAY_LEN + DCU_TOKEN_LEN)
#define ONE_DCU_IPPARA_BUFLEN   (DCU_SERVERIP_LEN + DCU_IPSETTING_LEN)
typedef struct                                          //下载所选的 DCU IP参数
{
    int8u   downloadTask;                               //下载任务，0表示未执行
    int8u   downloadCnt;
    int8u   currentDCUID;                               //下载对象的 DCU ID
    int32u  downloadStartTick;                          //启动下载任务的时间点
    
    int8u   downloadIPResult[8];                        //下载结果标志
    
    /* 存储 MMS 命令字 0x21 中的信息  */
    int8u   dcuObjects[8];                              //下载对象的 DCU 集合标记
    int8u   ServerIPPort[6];                            //dcu IP 参数中的 SERVER 
    int8u   dcuIPToken[MAX_DCU][DCU_IPSETTING_LEN];     //所有DCU的 IP参数
}tDownloadDCU_IPPara;
EXTN tDownloadDCU_IPPara DownloadDCU_IPPara;
EXTN int8u  CANBus_downloadIPPara[ONE_DCU_IPPARA_BUFLEN + 1];   //通过CAN总线下载的参数，最后一字节为CAN通讯校验字节


/* 2022-4-8 新增: MMS 批量读指定DCU的 IP 参数 */
typedef struct                                          //上传指定的 DCU IP参数
{
    int8u   uploadTask;                                 //上传任务，0表示未执行
    int8u   uploadCnt;                                  //上传成功的DCU总数
    int8u   currentDCUID;                               //上传对象的 DCU ID
    int8u   uploadNextTime;                             //
    
    int32u  uploadStartTick;                            //启动读取的时间点
    
    int8u   uploadDCUObjs[8];                           //读取对象的 DCU 集合标记
    int8u   uploadIPResult[8];                          //读取结果标志
    int8u   allDCU_IPPara[MAX_DCU][ONE_DCU_IPPARA_BUFLEN];   //依次保存读取DCU IP参数
    
    int8u   oneDCUIPPara[ONE_DCU_IPPARA_BUFLEN + 1];    //CAN 总线读取单个 DCU IP参数 存放位置，最后一字节为CAN通讯校验字节
}tUploadDCU_IPPara;
EXTN tUploadDCU_IPPara uploadDCU_IPPara;

/* =============================================================================
 * 2023-8-21 新增功能：读、写 DCU 行程数据 */
#define DCU_TRIPDATA_LEN        4   //DCU 滑动门里程
typedef struct                                          //下载所选的 DCU 的里程数据
{
    int8u   downloadTask;                               //下载任务，0表示未执行
    int8u   downloadCnt;
    int8u   currentDCUID;                               //下载对象的 DCU ID
    int32u  downloadStartTick;                          //启动下载任务的时间点
    
    int8u   downloadTripResult[8];                      //下载结果标志
    
    /* 存储 MMS 命令字 0x23 中的信息  */
    int8u   dcuObjects[8];                              //下载对象的 DCU 集合标记
    int8u   dcuTripData[DCU_TRIPDATA_LEN];              //设置DCU的 里程数据
}tDownloadDCU_Trip;
EXTN tDownloadDCU_Trip DownloadDCU_Trip;
EXTN int8u  CANBus_downloadTrip[DCU_TRIPDATA_LEN + 1];  //通过CAN总线下载里程数据，最后一字节为CAN通讯校验字节

typedef struct                                          //上传指定的 DCU 的里程数据
{
    int8u   uploadTask;                                 //上传任务，0表示未执行
    int8u   uploadCnt;                                  //上传成功的DCU总数
    int8u   currentDCUID;                               //上传对象的 DCU ID
    int8u   uploadNextTime;                             //
    
    int32u  uploadStartTick;                            //启动读取的时间点
    
    int8u   uploadDCUObjs[8];                           //读取对象的 DCU 集合标记
    int8u   uploadTripResult[8];                        //读取结果标志
    int8u   allDCU_Trip[MAX_DCU][DCU_TRIPDATA_LEN];     //依次保存读取DCU 的里程数据
    
    int8u   oneDCUTripData[DCU_TRIPDATA_LEN + 1];       //CAN 总线读取单个 DCU 里程数据 存放位置，最后一字节为CAN通讯校验字节
}tUploadDCU_Trip;
EXTN tUploadDCU_Trip uploadDCU_Trip;


EXTN int8u ReadDcuStateFailBuf[MAX_DCU];   //读取在线DCU 22字节，失败+1，成功-1； 目的是观察CAN通讯


//uwCANFUN_FLAG0; //标志 1 表示 有该功能
#define bFun_Up_WorkState                tagCANFUN.uwCANFUN_FLAG0.B0  //工作状态数据申请
#define bFun_Up_MoreDcuParameter         tagCANFUN.uwCANFUN_FLAG0.B1  //参数上载
#define bFun_Down_MoreDcuParameter       tagCANFUN.uwCANFUN_FLAG0.B2  //参数下载
#define bFun_Up_OpenDoorCurve            tagCANFUN.uwCANFUN_FLAG0.B3  //开门曲线数据获取
#define bFun_Up_CloseDoorCurve           tagCANFUN.uwCANFUN_FLAG0.B4  //关门曲线数据获取
#define bFun_UpOneDcuState               tagCANFUN.uwCANFUN_FLAG0.B5  //读取在线单个DCU 状态
#define bFun_TestDcuIfOnLine             tagCANFUN.uwCANFUN_FLAG0.B6  //读取不在线 DCU,判断是否上线
#define bFun_QUERY                       tagCANFUN.uwCANFUN_FLAG0.B7  //查询PEDC/DCU 软件信息
#define bFun_UpLightScreen               tagCANFUN.uwCANFUN_FLAG0.B8  //上传 光幕 信息
#define bFun_UpPEDCHardwareSetting       tagCANFUN.uwCANFUN_FLAG0.B9  //上传 DCU 位置设置
#define bFun_DownPEDCHardwareSetting     tagCANFUN.uwCANFUN_FLAG0.B10 //下载 DCU 位置设置

//读取 功能状态
#define BusState_Down_TOSLVCOMMAND()     wtj_ChkTransState(FUNID_canbus_down_cmd)     //读取 MVB命令下载至DCU 状态
#define BusState_Up_MoreDcuParameter()   wtj_ChkTransState(FUNID_Up_MoreDcuParameter)
#define BusState_Down_MoreDcuParameter() wtj_ChkTransState(FUNID_Down_MoreDcuParameter)
#define BusState_Up_OpenDoorCurve()      wtj_ChkTransState(FUNID_Up_OpenDoorCurve)
#define BusState_Up_CloseDoorCurve()     wtj_ChkTransState(FUNID_Up_CloseDoorCurve)
#define BusState_UpOneDcuState()         wtj_ChkTransState(FUNID_UpOneDcuState)
#define BusState_TestDcuIfOnLine()       wtj_ChkTransState(FUNID_TestDcuIfOnLineDcu)
#define BusState_QUERY()                 wtj_ChkTransState(FUNID_QUERY)
#define BusState_UpLightScreen()         wtj_ChkTransState(FUNID_Up_LightScreen)

//建立 功能连接
#define Build_Down_TOSLVCOMMAND()        wtj_CapiStartTrans(FUNID_canbus_down_cmd, 0) //把MVB命令下载至DCU
#define Build_UpMoreDcuPara(ID)          wtj_CapiStartTrans(FUNID_Up_MoreDcuParameter, ID)
#define Build_DownMoreDcuPara(ID)        wtj_CapiStartTrans(FUNID_Down_MoreDcuParameter, ID)
#define Build_UpOpenDoorCurve(ID)        wtj_CapiStartTrans(FUNID_Up_OpenDoorCurve, ID)
#define Build_UpCloseDoorCurve(ID)       wtj_CapiStartTrans(FUNID_Up_CloseDoorCurve, ID)
#define Build_UpOneDcuState(ID)          wtj_CapiStartTrans(FUNID_UpOneDcuState, ID)
#define Build_TestDcuIfOnLine(ID)        wtj_CapiStartTrans(FUNID_TestDcuIfOnLineDcu, ID)
#define Build_QUERY(ID)                  wtj_CapiStartTrans(FUNID_QUERY, ID)
#define Build_UpLightScreen()            wtj_CapiStartTrans(FUNID_Up_LightScreen, ID_MCP)
#define Build_DownLightScreen()          wtj_CapiStartTrans(FUNID_Down_LightScreen, ID_MCP)

//清除功能 状态


//***********************************************************
EXTN void wtj_CapiStartTrans(int8u aFUNID_ReadOneDcuState, int8u ID);
EXTN int8u wtj_ChkTransState(int8u aFUNID_ReadOneDcuState);
EXTN void Can_Up_MoreDcuParameter(void);                              //读所有DCU 参数
EXTN void Can_Down_MoreDcuParameter(void);                            //下载所选DCU参数
EXTN void Can_UpOpenDoorCurve(void);                                  //读开门曲线
EXTN void Can_UpCloseDoorCurve(void);                                 //读关门曲线
EXTN void Can_UpOneDcuState(void);                                    //如果读不到该 DCU N 次 ,则会置 不在线标志
EXTN void Can_QUERY(void);                                            //查询DCU 软件信息
EXTN int8u ReadDcuStateFail(int8u id);                                //out: = true 表示错误次数超出设置范围。 = false 表示错误次数在设置范围之内
EXTN void ClsReadDcuStateFailBuf(int8u id);                           //清除缓存



/*******************************************************************************
 *                                                                             *
 * 2019-11-15                                                                  *
 * DCU-CAN V1.6                                                                *
 *                                                                             *
 * PEDC发给DCU的 Inhibit、remote OPEN、remote CLOSE 等命令，通过广播方式发送   *
 *                                                                             *
 ******************************************************************************/

/* 广播命令类型枚举
 */
typedef enum
{
    BROAD_CMD_MMS_INHIBIT = 0,      /* inhibit 信息:  MMS          */
    BROAD_CMD_STC_INHIBIT,          /* inhibit 信息:  STC          */
    BROAD_CMD_CTRL_DOOR,            /* remote open or close door   */
    BROAD_CMD_CTRL_WIND,            /* 开风阀/关风阀               */
    /* 在此添加新的类型 */
    
    /* 同时支持的广播命令个数，此定义必须放在最后，自动计算命令类型总数 */
    BROAD_CMD_NUM                   
}enmBroadCmdType;    


typedef union
{
    struct
    {
        int8u  opBit    : 3;   /* bit2-0：指定 bit4-3需要操作的DCU本地命令字的某个位    */
        int8u  opCmd    : 2;   /* bit4-3: 对DCU本地命令字进行操作                       */
        int8u  exclude  : 1;   /* bit5  ：互斥指令标志，若 bit4-3 = 0b11，本位无效      */
        int8u  dcuCmd   : 1;   /* bit6  ：指定 DCU本地命令字                            */
        int8u  Valid    : 1;   /* bit7  ：操作命令有效标志, 1=valid                     */
    }bits;
    
    int8u byte;
}tPEDCOpCmd;

/* PEDC 通过广播发给 DCU 的命令结构
 *  - 8 字节标志位，对应64个DCU
 *  - 3 字节操作控制字
 *  - 2 字节CRC校验
 *  - 1 字节校验和，此字节是兼容以前的校验方式
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
    tBroadCmdPkg cmdFrame;          /* 广播命令包   */ 
    int8u   startSend;              /* 启动发送标记 */ 
    int8u   uRepeatSendTime;        /* 连续2次发送相同命令的最小时间间隔，时间单位0.1s */
}tBroadCmd;

/* 广播命令任务存储区
 * 每种广播命令类型只有一个存储区
 */
EXTN tBroadCmd  broadCmdList[BROAD_CMD_NUM];


/* 广播命令发送管理机制 */
typedef struct
{
    int8u  isBroading;                  /* 0=没有执行广播发送命令   */
    int8u  BroadingType;                /* 正在广播发送命令的类型   */
    int8u  Tx_NUM;                      /* 发送次数                 */
    int8u  Tx_intervals;                /* 发送的间隔时间           */
    int8u  setRepeatTimes;              /* 重复发送次数             */
    int8u  setInterval_ms;              /* 重复发送间隔时间         */
}tBroadCmdManage;

#define BROADCMD_INTERVAL_MS                100     /* 重复发送间隔时间默认值 ms, <256   */
#define BROADCMD_REPEAT_TIMES               5       /* 重复发送次数默认值           */
EXTN tBroadCmdPkg       CANSendBroadCmdPkg;         /* 当前CAN总线发送的广播命令    */
EXTN tBroadCmdManage    CANSendBroadCmdManage;      /* 管理发送广播命令             */


/* ================================================
 * 定义 DCU 本地命令字
 * 
 * @@@！！！  注意 ！！！@@@
 * 本定义需要根据实际项目的需求来确定
 * 2020-4-24 太原2
 * PEDC只能接收MMS发下来的对位隔离，由MMS确定对位隔离命令的来源
 * DCU 使用了 DCUCMD1_STC_INHIBIT
 */

#define DCUCMD1_STC_INHIBIT     0           /* DCU 本地命令字1：STC Inhibit      */
#define DCUCMD1_OPN             1           /* DCU 本地命令字1：open  */
#define DCUCMD1_CLS             2           /* DCU 本地命令字1：close */
#define DCUCMD1_3               3
#define DCUCMD1_4               4
#define DCUCMD1_5               5
#define DCUCMD1_6               6
#define DCUCMD1_7               7

#define DCUCMD2_MMS_INHIBIT     0           /* DCU 本地命令字2：MMS Inhibit */
#define DCUCMD2_OPN             1           /* DCU 本地命令字2：open  */
#define DCUCMD2_CLS             2           /* DCU 本地命令字2：close */
#define DCUCMD2_3               3
#define DCUCMD2_4               4
#define DCUCMD2_5               5
#define DCUCMD2_6               6
#define DCUCMD2_7               7


/* ================================================
 * 
 * 2020/3/23   DCU-CAN V1.7
 *  DCU-CAN V1.7功能与1.6完全一致（只是修改命令字的命名），PEDC 软件的使用保持不变
 * 
 * DCU - CAN  V1.6
 * 2019-11-15 
 * 增加 PEDC 操作DCU本地命令字的灵活性，使DCU程序能有更大的适用范围
 *  */
/*
1. DCU本地命令字
   DCU在内存中有2个命令字，PEDC通过CAN总线操作DCU本地命令字
   
   - 命令字1(int8u) ：在DCU掉线以后会自动清除
     bit0 - S&TC Inhibit
     bit1 - Open Command
     bit2 - Close Command
     bit3 - 保留
     bit4 - 保留
     bit5 - 保留
     bit6 - 保留
     bit7 - 保留

   - 命令字2(int8u) ：在DCU掉线以后不会自动清除，只能通过命令清除
     bit0 - MMS Inhibit
     bit1 - Open Command
     bit2 - Close Command
     bit3 - 保留
     bit4 - 保留
     bit5 - 保留
     bit6 - 保留
     bit7 - 保留
     
    - 关于位互斥功能
     DCU本地命令字1、2支持位互斥操作，每2个位为一组，设置其中一位，另一个位被强制清除，清除位不影响另一个位
     Bit 1&2 为一组，支持位互斥操作
     Bit 3&4 为一组，支持位互斥操作
     Bit 5&6 为一组，支持位互斥操作

2.  PEDC 与 DCU 通信协议
2.1 PEDC通过广播命令包操作DCU本地命令字

    命令包格式：
    共14个字节，分成2个CAN数据帧发送。
    对应DCU的位(8Bytes) + 操作命令1(1Byte) + 操作命令2(1Byte)  + 操作命令3(1Byte) + CRC(2Bytes) + 累加和(1Byte)
 
 *  注意：3个操作命令可以分别操作 DCU本地命令字，DCU执行时按 操作命令1到3进行执行，
 *        若操作同一个位或字节，需要PEDC来保证避免命令字的冲突情况
    
2.2 命令包含义
2.2.1   对应DCU的位 
        8 字节: 每一个bit对应一个DCU，最大支持64个DCU
2.2.2   操作命令
        3个操作命令可以同时操作3个位，可以同时对 DCU本地命令字(1 & 2)进行位操作
     
        3个操作命令的格式统一，如下：
        bit7  : 操作命令有效标志
            0=本操作命令无效
            1=本操作命令有效
            
        bit6  : 指定 DCU本地命令字
            0=DCU本地命令字1
            1=DCU本地命令字2
        
        
        bit5  : 互斥指令标志，若 bit4-3 = 0b11，本位无效
            0，表示正常位操作
            1，表示互斥位操作，本地命令字中的有3组互斥位，若设置其中一位必然清除另一位，但是清除其中一位则不影响另一位
             
        bit4-3: 对DCU本地命令字进行操作
            0=位复制，即将8字节中的对应本DCU的位无条件复制到本地命令字的指定位，注意处理互斥位，
                      本操作可以同时写所有DCU不同的命令（设置和清除）
                      互斥位处理：若复制的位
            1=位设置，即将8字节中的对应本DCU的位为1时， 设置本地命令字的指定位，注意处理互斥位
            2=位清除，即将8字节中的对应本DCU的位为1时， 清除本地命令字的指定位
            3=字节操作，强制清除指定的 'DCU本地命令字'
                
        bit2-0：指定 bit4-3需要操作的DCU本地命令字的某个位
 */

/* bit7  ：操作命令有效标志 */
#define OPCMD_INVALID       (0 << 7)
#define OPCMD_VALID         (1 << 7)

/* bit6  ：指定 DCU本地命令字 */
#define OP_DCUCMD_1         (0 << 6)
#define OP_DCUCMD_2         (1 << 6)

/* bit5  ：互斥指令标志，若 bit4-3 = 0b11，本位无效 */
#define OP_BIT_NORMAL       (0 << 5)
#define OP_BIT_EXCLUDE      (1 << 5)

/* bit4-3：对DCU本地命令字进行操作 */
#define CMD_BIT_CPY         (0x00 << 3)                 /* 位复制 */
#define CMD_BIT_SET         (0x01 << 3)                 /* 位设置 */
#define CMD_BIT_CLR         (0x02 << 3)                 /* 位清除 */
#define CMD_BYTE_CLR        (0x03 << 3)                 /* 字节操作，清除 */


#define STC_INHIBIT_ENABLE              (DCUCMD1_STC_INHIBIT | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | OPCMD_VALID)
#define MMS_INHIBIT_ENABLE              (DCUCMD2_MMS_INHIBIT | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_SET | OPCMD_VALID)
#define MMS_INHIBIT_DISABLE             (DCUCMD2_MMS_INHIBIT | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_CLR | OPCMD_VALID)

#define CREPEATSEND_SEC     5           /* 相同命令发送的时间间隔, seconds */
#if (CREPEATSEND_SEC > 26 )
#error "CREPEATSEND_SEC must be less than 26"
#endif

/* ================================================ 
 * 发送广播命令的接口函数
 */
/* 需要在主程序中轮流调用 */
PUBLIC void procBroadcastTask(void);

/* 添加广播命令任务 
 * addBroadCmdTask_AtOnce  ==> 立即发送
 * addBroadCmdTask         ==> 检测相同命令的发送时间间隔决定是否重复发送
 */
PUBLIC BOOL addBroadCmdTask(tBroadCmdPkg * command, enmBroadCmdType cmdType);
PUBLIC BOOL addBroadCmdTask_AtOnce(tBroadCmdPkg * command, enmBroadCmdType cmdType);

/* 设置发送广播命令的参数，如果没有设置，则采用默认值 */
PUBLIC void setRepeatPara(int8u repeatTimes, int8u repeatInterval);

#endif

PUBLIC void processCANComm(void);


