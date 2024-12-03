#ifndef _PIC24_UPDATEDRV_H
#define _PIC24_UPDATEDRV_H

#include "_cpu.h"

#define PIC24_BL_ENABLE     1   /* 0=disable, else = enable */

/* 升级模式 updateMode */
#define UPMODE_AUTO     0                       /* 核对版本信息决定是否升级 */
#define UPMODE_FORCE    1                       /* 强制升级，不核对版本信息 */
#define UPMODE_BYCMD    0x10                    /* 由外部命令决定 */

/* PIC24 FLASH 相关地址信息
 */
#define P24_FLASH_SIZE_WORD     ( 0x5800 )      /* 存储器大小（word）   */
#define P24_CONFIG_WORD_ADDR    ( 0x57FC )      /* 配置字的地址（word） */
#define BL_CODE_START           ( 0x0400 )      /* bootloader start address(word)       */
#define BL_CODE_END             ( 0x0FFF )      /* bootloader end   address(word)       */
#define BL_VERSION_ADDR         ( 0x0FF0 )      /* bootloader version location(word)    */

/* PIC24 APP FLASH地址定义 
 */
#define APP_START_ADDR          0x1000      /* APP 代码区起始地址 */
#define APP_PRJINFO_ADDR        0x1000      /* 项目信息存储地址 */
#define APP_LAST_FLASH          0x5400      /* APP的FLASH最后一块包含了配置字，不擦除 */

/* 比较关键字的长度, eg. "PEDC-PIC24" 
 * 0 表示不比较关键字
 */
#define APP_KEYWORDS_LEN        10          



/* 待擦除的 FLASH 页数量（Block块），PIC24FJ32GA002有22页 
 * 最后一块包含了配置字 */
#define ER_FLASH_BLOCKS         ((APP_LAST_FLASH<<1) / 2048)

#define P24_BLANK_DATA          0x00FFFFFFL /* 空白 */
#define P24_INST_PER_ROW        64          /* 每行FLASH 指令64条指令 */
#define P24_BYTES_PER_INST      4           /* 每条指令4字节 */
#define P24_BYTES_PER_ROW       (P24_INST_PER_ROW * P24_BYTES_PER_INST) /* 每行FLASH 指令64条指令 256 字节 */
#define P24_WRITE_INST_ROWS     1           /* 每次写入指令行数，目前只支持1行 */
#if (P24_WRITE_INST_ROWS != 1)
#error "WRITE_INST_ROWS must be 1 NOW!"
#endif

/* 每次发送的升级代码最大 256 字节 
 * 即每次最多写入 1行指令
 * 附加信息 共16Bytes ：
 *  头   8Bytes
 *  尾   3Bytes
 *  参数 5Bytes
 * 
 */
#define MAX_PACKET_SIZE     (256 + 16 + 8)  


/* PIC24 Bootloader 命令字 */
#define RD_VER      0x00        /* 读 Bootloader 版本, eg. 00 02  */
#define RD_FLASH    0x01        /* 读 Flash, eg. 01 02 00 00 00   */
#define WT_FLASH    0x02        /* 写 Flash, eg. 02 01 80 00 00 <data> */
#define ER_FLASH    0x03        /* 擦除 Flash, eg. 03 15 00 00 00  = 15 表示Pages */
#define RD_EEDATA   0x04
#define WT_EEDATA   0x05
#define RD_CONFIG   0x06
#define WT_CONFIG   0x07
#define VERIFY_OK   0x08        /* 校验OK，进入 user 应用程序  */
#define COMMAND_NULL    0xff

/* 与 PIC24应用层 通信命令 
 * 考虑到 Bootloader 可能同时也要处理，所以这里命令字不要与 Bootloader 命令字重复
 */
#define PIC24APP_CMD_GET_CS     0x50        /* 读Checksum信息 */
#define PIC24APP_CMD_ENTER_BL   0x51        /* 软件复位启动Bootloader */



/* 与 PIC24 (应用层 & Bootloader) 通信状态
 *  */
typedef enum
{
    UP_PIC24_NULL       = 0 ,   /* 没有启动升级 */
    UP_PIC24_INIT           ,   /* 升级初始化 */
    UP_PIC24_START_BL       ,   /* 启动 Bootloader */
    UP_PIC24_UPDATE_INI     ,   /* 启动 升级 */
    UP_PIC24_ERASE          ,   /* 发送命令 PIC24 Bootloader 擦除       */
    UP_PIC24_WRITE          ,   /* 发送数据给 PIC24 Bootloader 写FLASH  */
    UP_PIC24_VERIFY_WR      ,   /* 发送命令给 PIC24 Bootloader 读FLASH 校验升级结果  */
    UP_PIC24_VERIFY_OK      ,   /* 校验OK，发送命令给 PIC24 Bootloader 重启运行 */
            
    UP_PIC24_BL_RESET       ,   /* PIC24 Bootloader 启动重启 */
    UP_PIC24_BL_NOTRESET    ,   /* PIC24 Bootloader 没有通过校验，不启动重启 */
            
    UP_PIC24_FINISHED   = 0x5A, /* 结束本次升级                         */
}enmUpPic24Step;

/* PIC32主控板回复MMS 的 PIC24 升级状态字 (命令类型 OBJ_CMD_BL_CTRL)
 *  */
typedef enum
{
    TOMMS_PIC24_NOAPP           = 0,        /* 当前PIC24中没有应用层程序，只有Bootloader */
    TOMMS_PIC24_INIT            = 1,        /* 升级初始化过程中 */
    TOMMS_PIC24_INIT_OK         = 2,        /* 升级初始化成功   */
                    
    TOMMS_PIC24_UPDATING        = 4,        /* 升级中，升级进度通过 已升级长度和总升级长度 计算 */
    TOMMS_PIC24_FINISHED        = 5,        /* 升级完成                 */
    TOMMS_PIC24_RUNAPP          = 6,        /* 升级完成，重启运行APP    */
    TOMMS_PIC24_UPDATED         = 0x10,     /* 已经是最新版本           */
    TOMMS_PIC24_UPDATED_APP     = 0x11,     /* APP是最新版本，但是Bootloader版本不一致  */
            
    TOMMS_PIC24_FAIL_BINFILE    = 0x80,     /* PIC32 存储的PIC24 BIN文件错误  */
    TOMMS_PIC24_FAIL_INIT       = 0x81,     /* 升级初始化失败       */
    TOMMS_PIC24_FAIL_BL_OVTM    = 0x82,     /* 启动Bootloader超时   */
    TOMMS_PIC24_FAIL_VERIFY     = 0x83,     /* 升级过程中校验失败   */
    TOMMS_PIC24_FAIL_OVERTIME   = 0x84,     /* 升级过程中PIC24超时无响应   */
    TOMMS_PIC24_FAIL_BL_CODE    = 0x85,     /* Bootloader 不存在或入口不正确   */
    TOMMS_PIC24_ERRORMODE       = 0x90,     /* 升级模式错误     */
}enmP32ToMMSState;


typedef union
{
    DWORD   Val;
    
    WORD    w[2];
    struct
    {
        WORD LW;
        WORD HW;
    }word;
    
    BYTE    v[4];
}tDWORD;

typedef	struct
{

    BYTE  majorVer;     /* 软件版本 */ 
    BYTE  minorVer;
    BYTE  pro_type;     /* 协议类型 */
    BYTE  pro_ver;      /* 协议版本 */

    BYTE  Day;
    BYTE  Month;
    WORD  Year;
    
    WORD  signal;       /* 标记 */
}tBootloaderInfo;

#pragma pack(1)
typedef struct
{
    DWORD allCodeCS;    /* PIC24 Flash 代码校验和 */
    DWORD onlyAppCS;    /* PIC24 Flash 中Bootloader的代码校验和 */
    tBootloaderInfo  blInfo;
}tFlashInfo;

typedef struct
{
    BYTE    cmd;        /* 操作Flash命令 */
    BYTE    len;        /* 长度，注意不同命令，长度含义不一样 */
    BYTE    addrL;      /* 操作Flash地址，字地址 */
    BYTE    addrM;
    BYTE    addrH;
}tBLHeader;
#define BL_HEADER_LEN   sizeof(tBLHeader)

//typedef BOOL (*cbSendMsgToMMS)(int8u msgType, int8u srcNode, int8u * data, int16u dataLen);
//PUBLIC void setCalbackToMMS(cbSendMsgToMMS fun);

/* =============================================================================
 * 接口函数定义
 * 
 */

PUBLIC BOOL upPIC24_InitStart(int8u Initiator, int8u upMode, tFlashInfo *info);
PUBLIC void upPIC24_QueryPic24App(void);

PUBLIC enmUpPic24Step startUpdatePic24(tEnmCommObj obj);
PUBLIC void Pic24UpdateProcess(tUartComm *pComm);
PUBLIC BOOL chkIsUpdatingPic24(void);

PUBLIC void getBinCodeInfo(tFlashInfo * P24CodeInfo);

PUBLIC BOOL upPIC24_ProcP24Reply(int8u msgType, int8u *rxData, int16u rxDataLen);

#endif //_PIC24_UPDATEDRV_H

