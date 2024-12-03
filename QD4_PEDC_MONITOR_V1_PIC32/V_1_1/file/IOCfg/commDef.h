#ifndef _COMM_DEF_H
#define _COMM_DEF_H

#define PCB_ID_SERVER       200     //PEDC 主控板的ID，如 MONITOR

#define MAX_IODATA_SIZE             40
#define PIC32_IODATA_SIZE           12
#define PCA9505_IODATA_SIZE         5

#include "SWLogic.h"

/* =============================================================================
 *  协议规范通信数据帧的格式，如下：
 *  此规范与 MMS 和 PEDC 通信的格式相同
 *  数据域  含  义  长度（字节）    值(类型)    说  明
 *  1.      数据帧头1   1           0x10    
 *  2.      数据帧头2   1           0x02   
 *  3.      数据帧序号  1           INT8U   由主机负责管理发送。从机将接收自主机的序号复制到此区域反馈给主机
 *  4.      源节点      1           INT8U   通信数据源节点
 *  5.      目标节点    1           INT8U   通信数据目标节点
 *  6.      消息类型    1           INT8U   通信数据功能描述
 *  7.      数据长度H   1           INT8U   通信数据长度低字节
 *  8.      数据长度L   1           N       本协议支持通信数据块的长度最大为255字节
 *  9.      数据块      N           INT8U   包含 N 个字节的数据
 *  10.     数据帧尾1   1           0x10    
 *  11.     数据帧尾2   1           0x03   
 * 
 *     帧校验方法由定义  FRAME_VERIFY_CRC16，若修改为 CRC 校验，注意修改程序
 *  12.     帧校验      1           帧头到帧尾的所有数据按字节进行异或计算的结果
 * 
 *  CRC ???
 *  12.     帧校验      2           INT16U  帧数据的校验值，发送采用小端模式（即先发送低字节），对帧头到帧尾的所有数据按字节计算CRC16
 * */

/**
 */
#define HEADER_DLE      0x10
#define HEADER_STX      0x02
#define TAIL_DLE        0x10
#define TAIL_ETX        0x03

/* 数据帧头统一格式 */
//#pragma pack(1)
typedef struct
{
    int8u header1;      /* 帧头1    */
    int8u header2;      /* 帧头2    */
    int8u seq;          /* 帧序号   */
    int8u srcNode;      /* 源节点   */
    int8u dstNode;      /* 目标节点 */
    int8u msgType;      /* 通信数据类型 tEnmCommObjCmd  */
    int8u dataLenH;     /* 数据长度 */
    int8u dataLenL;     /* 数据长度 */
}tPktHeader ;

#define FRAME_VERIFY_CRC16      0   /* 1=CRC16 */

/* 数据帧尾统一格式 */
//#pragma pack(1)
typedef struct
{
    int8u Tail_DLE;     /* 结束标记 */
    int8u Tail_ETX;     /* 结束标记 */
    #if (FRAME_VERIFY_CRC16 != 1)
    int8u crcXOR;       /* 数据校验，异或值 */
    #else
    int8u crc16L;       /* 数据校验CRC16 低字节 */
    int8u crc16H;       /* 数据校验CRC16 高字节 */
    #endif
}tPktTail ;


/* 定义通信数据对象枚举类型 */
typedef enum
{
    COMMOBJ_MONITOR = 200,  /* 0 :0xC8, MONITOR-PIC32  */
    //COMMOBJ_PSL     = 201,  /* 1 :0xC9, PSL 板 */
    //COMMOBJ_SIG,            /* 2 :0xCA, SIG 板 */
    //COMMOBJ_IBP,            /* 3 :0xCB, IBP 板 */
    COMMOBJ_IORW_A = 204,   /* 4 :0xCC, MONITOR-PIC24A */
    COMMOBJ_IORW_B,         /* 5 :0xCD, MONITOR-PIC24B */
    COMMOBJ_SWITCHER,       /* 6 :0xCE, SZ16-切换板  */
    COMMOBJ_LOG_A_UA1 ,     /* 7 :0xCF, SZ16-逻辑板A-UA1 */
    COMMOBJ_LOG_A_UB1 ,     /* 8 :0xD0, SZ16-逻辑板A-UB1 */
    COMMOBJ_LOG_B_UA1 ,     /* 9 :0xD1, SZ16-逻辑板B-UA1 */
    COMMOBJ_LOG_B_UB1 ,     /* 10:0xD2, SZ16-逻辑板B-UB1 */
    COMMOBJ_COUNTER,    /* 用于自动计算总数 */
    COMMOBJ_UNDEF = 0xff,
}tEnmCommObj;

#define COMMOBJ_FIRST   COMMOBJ_MONITOR
#define COMMOBJ_TOTAL   ((int32u)(COMMOBJ_COUNTER - COMMOBJ_MONITOR))

#define COMMOBJ_LOG1            COMMOBJ_LOG_A_UB1
#define COMMOBJ_LOG2            COMMOBJ_LOG_B_UB1


/* 定义通信数据类型枚举类型 */
#define CMD_DIR_BIT     0x80
typedef enum
{
    OBJ_CMD_IOR = 0,    /* 读IO数据     */
    OBJ_CMD_CFG_WR,     /* 写配置信息   */
    OBJ_CMD_CFG_RD,     /* 读配置信息   */
    OBJ_CMD_RELAY_LIFE, /* 读继电器信息 */
            
    OBJ_CMD_TESTDATA    = 0xAA,         /* 测试数据A0 ~ AF */
    OBJ_CMD_INI_TEST    = 0xB0,         /* 设置测试功能 */
            
    /* 在线升级相关的命令 
     * 注意与 glbUpdate.h 一致
     */
    OBJ_CMD_UPDATE      = 0xE0,         /* 在线升级         */
    OBJ_CMD_UPDATE_RPLY = 0xE1,         /* 回复在线升级状态 */
            
    OBJ_CMD_UPDATESTS   = 0xE2,         /* 读升级状态       */
            
    OBJ_CMD_DEVINFO     = 0xE5,         /* 读设备信息       */
    OBJ_CMD_RPLYINFO_OK = 0xE6,         /* 回复读设备信息   */
    OBJ_CMD_RPLYINFO_NG = 0xE7,         /* 回复读设备信息   */
            
    OBJ_CMD_BOOTLOADER  = 0xF0,         /* 与 Bootloader 通信 */
    OBJ_CMD_BL_CTRL     = 0xF1,         /* 启动 Bootloader 控制  */
}tEnmCommObjCmd;


#define PCA9505_1   (0x40 | (0 << 1))    //PCA9505(1)
#define PCA9505_2   (0x40 | (1 << 1))    //PCA9505(2)
#define PCA9505_3   (0x40 | (3 << 1))    //PCA9505(3)

/* 定义安全回路检测输入端口
 */
typedef enum
{
    SF_INPUT_PORT_NONE = 0xff , //无检测口
    
    /* MCU GPIO 输入口 
     * 参见prots.h --> IoPortId 定义
     */
    SF_INPUT_PB     = 0,     //MCU GPIOB
    SF_INPUT_PC        ,     //MCU GPIOC
    SF_INPUT_PD        ,     //MCU GPIOD
    SF_INPUT_PE        ,     //MCU GPIOE
    SF_INPUT_PF        ,     //MCU GPIOF
    SF_INPUT_PG        ,     //MCU GPIOG
    
    /* PCA9505 输入口 */
    SF_INPUT_PCA1      = PCA9505_1,   //PCA9505(1)
    SF_INPUT_PCA2      = PCA9505_2,   //PCA9505(2)
    SF_INPUT_PCA3      = PCA9505_3,   //PCA9505(3)
}tEnmSFInputPort;

/* 定义安全回路检测输入端口类型 */
typedef enum
{
    SF_TYPE_DCU_K31PN = 0 ,         //DCU 安全继电器 线圈 K31K32PN
    SF_TYPE_DCU_K31NC     ,         //DCU 安全继电器 触点 K31K32
    SF_TYPE_DCU_K33NO     ,         //DCU 安全继电器 触点 K33
    SF_TYPE_PDS_coil      ,         //PDS 安全继电器 线圈
    SF_TYPE_PDS_contact   ,         //PDS 安全继电器 触点
    SF_TYPE_PDS_contact2  ,         //PDS 安全继电器 触点
            
    SF_SCAN_TYPES                   //用于计数
}tEnmSFType;

    
/* 
 * 配置扫描安全继电器用的GPIO
 */
typedef struct
{
    tEnmSFType          type;   /* 类型         */
    tEnmSFInputPort     port;   /* 端口         */
    int16u              pin;    /* 脚位 BIT_xx  */
}tSFScanCfgs;

/* 这里的定义需要根据PCB原理图确定
 * 实际PCA9505的A2 A1 A0
 * 最大支持配置3个PCA9505
 */
typedef enum
{
    PCA9505_EN1  = PCA9505_1,   //PCA9505(1)使能
    PCA9505_EN2  = PCA9505_2,   //PCA9505(2)使能
    PCA9505_EN3  = PCA9505_3,   //PCA9505(3)使能
    PCA9505_DIS  = 0xff     ,   //禁止
}tEnmPCA9505Cfg;


/* 定义配置有效常量
 */
typedef enum
{
    CFG_INVALID = 0 ,           //配置无效
    CFG_VALID   = 0xAB,         //配置有效
}tEnmCfgValid;

/* 定义MCU GPIO 配置常量
 */
typedef enum
{
    GPIOCFG_DIS = 0xff ,        //禁止 MCU GPIO 输入
    GPIOCFG_EN  = 12,           //使能 MCU GPIO 输入
}tEnmGPIOCfg;

/* 通信数据帧标准结构
 */
//#pragma pack(1)
typedef struct
{
    tPktHeader  header;
    int8u      *pData;          /* 数据区   */
    tPktTail    tail;
}tCommObjCmd_Req;

/* 状态标志 */
typedef  union
{
    struct 
    {
        int8u pca9505Err1   : 1;   /* 读写PCA9505(1)错误标志，1 = 出错*/
        int8u pca9505Err2   : 1;   /* 读写PCA9505(1)错误标志，1 = 出错*/
        int8u pca9505Err3   : 1;   /* 读写PCA9505(1)错误标志，1 = 出错*/
        int8u uartCommErr   : 1;   /* 串口通信错误标志，1 = 出错      */
        int8u dcuSFRelayPow : 1;   /* 0 = 断电, 1 = 通电 */
        int8u dcuSFRelayErr : 1;   /* 1 = 错误 */
        int8u pdsSFRelayPow : 1;   /* 0 = 断电, 1 = 通电 */
        int8u pdsSFRelayErr : 1;   /* 1 = 错误 */
    }bits;

    int8u byte;
}tSlaveStsFlags;

typedef struct
{    
    int8u  gpioEn;              /* 有效标志, tEnmGPIOCfg */
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6个16位GPIO(PB~PG, 12字节IO数据，小端模式 */
}tPic32IOData;

typedef struct
{    
    int8u  pca9505_En;          /* PCA9505 IO输出有效标志, tEnmPCA9505Cfg   */
    int8u  pca9505Data[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
}tPCA9505IOData;



/* 安全回路闪断 */
typedef union
{
    struct 
    {
        int8u ConnBrks : 4;   /* 低半字节是触点闪断 */
        int8u CoilBrks : 4;   /* 高半字节线圈闪断   */
    }flash;
    
    int8u byte;
}tSF_Flash;


/* ====================================================== */
/* IBP回复读IO数据 
 * 回复的输入数据结构
 */
//#pragma pack(1)
typedef struct
{
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6个16位GPIO(PB~PG, 12字节IO数据，小端模式 */
    
    tSF_Flash dcuSFFlash;           /* DCU安全回路闪断次数 */

    tSF_Flash pdsSFFlash;           /* PDS安全回路闪断次数 */

    tSlaveStsFlags stsFlags;        /* 状态标志 */
}tReadIBP_IO;

/* PSL回复读IO数据 
 * 回复的输入数据结构
 */
typedef struct
{
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6个16位GPIO(PB~PG, 12字节IO数据，小端模式 */
    
    tSF_Flash dcuSFFlash;           /* DCU安全回路闪断次数 */

    tSF_Flash pdsSFFlash;           /* PDS安全回路闪断次数 */

    tSlaveStsFlags stsFlags;        /* 状态标志 */
}tReadPSL_IO;

/* SIG回复读IO数据 
 * 回复的输入数据结构
 */
typedef struct
{
    int8u  pca9505Data[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    
    tSF_Flash dcuSFFlash;           /* DCU安全回路闪断次数 */

    tSF_Flash pdsSFFlash;           /* PDS安全回路闪断次数 */

    tSlaveStsFlags stsFlags;        /* 状态标志 */
}tReadSIG_IO;


/* IPS 回复：读IO数据 
 * 回复的输入数据结构
 */
//#pragma pack(1)
typedef struct
{
    int8u  outData[PIC32_IODATA_SIZE];      /* 6个16位GPIO(PB~PG, 12字节IO数据，小端模式 */
    
}tWriteIPS_IO;




/* ====================================================== */
/* PIC24 回复读IO数据 
 * 回复的输入数据结构
 */
//#pragma pack(1)
typedef struct
{
    /* 若输出有效标志无效
     * 则输出数据被忽略
     */
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    
    tSF_Flash dcuSFFlash;           /* DCU安全回路闪断次数 */
    tSF_Flash pdsSFFlash;           /* PDS安全回路闪断次数 */
    tSlaveStsFlags stsFlags;        /* 状态标志 */
}tReadPic24_IO;



/* 主机请求：从PIC24 读 IO数据 
 * 携带的输出数据结构
 */
//#pragma pack(1)
typedef struct
{
    /* 若输出有效标志无效
     * 则输出数据被忽略
     */
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
}tWritePic24_IO;

/* 逻辑板回复读IO数据 
 * 回复的输入数据结构
 */
typedef struct
{
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    
    tSF_Flash dcuSFFlash;           /* 1字节 DCU安全回路闪断次数 */

    tSF_Flash pdsSFFlash;           /* 1字节 PDS安全回路闪断次数 */

    tSlaveStsFlags stsFlags;        /* 1字节 状态标志 */
    
    int8u       UB1_Flag;           /* 1字节 UB1状态标志 */
    int8u       UB1_OutLogicNo;     /* 1字节 UB1输出逻辑序号，0xff 表示无软件逻辑输出 */
    tOutputCmd  UB1_outLogic;       /* 4字节 UB1输出信息 */
    union
    {
        struct
        {
            int8u   testEnable: 1;      /* 使能状态 */
            int8u   isRunning : 1;      /* 运行状态 */
            int8u             : 6;
        };
        
        int8u data;
    }testRunSts;                    /* 1字节 UB1测试运行状态 */
    int8u       UB1_TestRunCnt[4];  /* 4字节 UB1测试运行计数器 */
}tReadLOG_IO;

/* 写逻辑板的IO
 */
typedef struct
{
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5字节IO数据              */
}tWriteLOG_IO;

typedef union
{
    tReadIBP_IO     ibpIn;
    tReadPSL_IO     pslIn;
    tReadSIG_IO     sigIn;
    tReadLOG_IO     logIn;
    tReadPic24_IO   pic24In;
    unsigned char   rdBuf[MAX_IODATA_SIZE];
}tCmdReadIO_s;


typedef union
{
    tWriteIPS_IO    IPSOutput;
    tWritePic24_IO  Pic24Output;
    tWriteLOG_IO    logOut;
    unsigned char   wrBuf[MAX_IODATA_SIZE];
}tCmdWriteIO_m;

/* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。 */
typedef union
{
    struct
    {
        int8u RESET_En      : 1;
        int8u Q1Switch_En   : 1;
        int8u Q2Switch_En   : 1;
        int8u PN0_En        : 1;
        int8u               : 4;

        int8u RESET         : 1;
        int8u Q1Switch      : 1;
        int8u Q2Switch      : 1;
        int8u PN0           : 1;
        int8u               : 4;
    }IOOut;
    
    int8u IOData[2];
}tCmdWriteIO_SFSW;


#endif  //_COMM_DEF_H
