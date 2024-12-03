
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "_cpu.h"
#include "flash.h"

//*******************************************
#define CDRV_PEDC            1
#define CDRV_DCU             2
#define CDRV_MCP             3                       //注意: 目前只有台湾线才会有 MCP
#define CDRV_TYPE            CDRV_PEDC

//*******************************************
//WTJ
#define MAX_DCU                 64
#define ONE_DCU_STATE_LEN       24
#define ONE_DCU_PARA_LEN        22
#define CURVE_LEN               242

#define ID_DCU_MIN              1       // Id_Dcu_Min =< ID <= Id_Dcu_Max
#define ID_DCU_MAX              (ID_DCU_MIN + MAX_DCU - 1)

#define ID_MCP                  (ID_DCU_MIN + MAX_DCU - 1)

#ifdef DEBUG_PRINTF_ERR
extern int8u aPRINTF_ERR_BUF[3 + 1];
#endif

extern void   DcuInc(unsigned char  * pID_DCU ,unsigned char DcuNum);                                    //DCU 加一 ，超出最大值循环到最小值

//*******************************************
#define SLAVE_NUM         (MAX_DCU)          //管理 Slave 数量，注意是否包括 MCP
#define BYTES_SLV_FLAG    8                  //用位表示的信息，每个 Slave 对应一个位，总字节数对齐到 int16u
#define BYTES_SLV_NUM     (MAX_DCU)          //每个 Slave 对应一个字节，总字节数对齐到 int16u

#define GET_SLV_ID0(id)   (id - ID_DCU_MIN)  //Slave 的 id 转换到从 0 开始

#if (CDRV_TYPE != CDRV_PEDC)
extern unsigned char canbus_adr;
#define g_NodeId     canbus_adr              //节点自己的id, 通过拨码开关设定
#endif

#if (CDRV_TYPE == CDRV_PEDC)
extern int8u g_DcuNum;
#endif

/*==============================================================================
 * 软件描述, 长度最大 96 字节(包含结束符)
 * 此描述字符串是保存在 
 * - PIC6010A EEDATA 中(Address = 0x7FF080)
 * - PIC32MX  FLASH  中(Address = MAIN_CODE_ADDR)
 *
 * -----------------------------------------------------------------------------
 *   项目的软件描述信息规则如下:
 *   - "PEDC", "DCU ", 等关键字(4字符必须大写,不足补空格)必须放在首位, MMS 升级时检查这个字符串, 避免升级文件(HEX)错误
 *   - 总长度（包括空白字符和0x00）最大96字节
 *   - 以下描述信息项
 *      A. 软件类型，与PCB板对应，是固定信息（PEDC / PEDC-PIC24 / PEDC-IBP / PEDC-PSL / PEDC-SIG）
 *      B. 工程项目（若直接使用原始的通用软件，则用 UNIVERSAL 表示）
 *      C. 软件日期时间
 *      D. 工程项目软件版本，若移植到旧的工程，请根据以前的版本号进行修改
 *      E. 软件适用的PCB板丝印与PCB板对应，是固定信息
 *      F. 通用软件的版本号，对于没有以通用软件为基础版本的软件，此项可以不需要
 *      G. 校验和，可选，用 （）进行分割
 *   - E F 两项，用一对中括号 " [ ] " 进行分割
 *   - 校验和信息的获取
 *      1. 将 () 内不填充任何信息，然后编译，
 *      2. 用ICD工具将程序下载到芯片中（否则步骤3得不到信息）
 *      3. 打开 “窗口”-->“项目环境”，先刷新，然后复制校验和粘贴在()中
 *      4. 再次编译，得到项目的HEX，此时的校验和与步骤3的校验和数值不相同
 */

#if (CDRV_TYPE == CDRV_PEDC)       
        #define PROJECT_INFOS                       \
            "PEDC: "                                \
            "QD4-ENG, "                             \
            "2024/06/18 11:00:04,"                  \
            "V1.0,"                                 \
            "[PEDC_MONITOR_SZ16"                    \
            "]"

#elif  (CDRV_TYPE == CDRV_MCP)                          
    #define PROJECT_NAME      "MCP: xxxx,"                   //<=30字节
    #define PROJECT_TIME      "20yy/mm/dd hh:mm:ss,"           //<=30字节
    #define PROJECT_VER       "V1.0,"                          //<=30字节
    #define PROJECT_OTHER     "FANGDA.LTD,FD-PEDC-IBP_200_CD3" //<=30字节
    
#endif

//=========================================== 在线升级软件功能 : TRUE -- 支持, FALSE -- 不支持
#define UPDATE_FUN_EN     TRUE

#if (UPDATE_FUN_EN == TRUE)
    #define UPFW_EE(N) __attribute__((space(eedata_UpFw), aligned(N)))
    #define UPFW_COD   __attribute__((section(".CodeSec_Update")))

#else
    #define UPFW_EE(N) __attribute__((aligned(N)))
    #define UPFW_COD
#endif

#define UPFW_CODE_BUF_MAX  (512+32)    //段数据长度最大 512B，信息最大 24B，另外所有数据的校验和 1B

#endif
