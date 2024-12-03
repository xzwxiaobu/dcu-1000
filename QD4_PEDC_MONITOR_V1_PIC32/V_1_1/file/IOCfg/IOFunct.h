

#ifndef _IOFUNCT_H
#define _IOFUNCT_H

#if( defined (PCB_PIC24))
#include <p24Fxxxx.h>
#endif

#include "MonCommCfg.h"


/* I2C波特率 400K */
#define I2C_CLOCK_FREQ          400000

/* PCA9505 RESET PIN */
#if( defined (PCB_PIC24))
#define I2C_ID                      0    /* 芯片 I2C 接口ID */

#define I2C_RST                     LATBbits.LATB7            //PCA9505 复位控制脚位
#define PCA9505_RST_SETOUTPUT()     (TRISBbits.TRISB7 = 0)
#define PCA9505_RST_OUT1()          (I2C_RST = 1)
#define PCA9505_RST_OUT0()          (I2C_RST = 0)

#else
#define I2C_ID                      I2C1    /* 芯片 I2C 接口ID */
#define PCA9505_RST_PortBit       (1 << 11)
#define PCA9505_RST_SETOUTPUT()   (mPORTDSetPinsDigitalOut(PCA9505_RST_PortBit))    //RST SET OUTPUT
#define PCA9505_RST_OUT1()        (mPORTDSetBits(PCA9505_RST_PortBit))
#define PCA9505_RST_OUT0()        (mPORTDClearBits(PCA9505_RST_PortBit))
#endif

/* 9505 Command register */
#define	COMMAND_05_IP0          0x00     //Input Port registers
#define	COMMAND_05_IP1          0x01
#define	COMMAND_05_IP2          0x02
#define	COMMAND_05_IP3          0x03
#define	COMMAND_05_IP4          0x04
#define	COMMAND_05_OP0          0x08    //Output Port registers
#define	COMMAND_05_OP1          0x09
#define	COMMAND_05_OP2          0x0A
#define	COMMAND_05_OP3          0x0B
#define	COMMAND_05_OP4          0x0C
#define	COMMAND_05_PI0          0x10    //Polarity Inversion registers
#define	COMMAND_05_PI1          0x11
#define	COMMAND_05_PI2          0x12
#define	COMMAND_05_PI3          0x13
#define	COMMAND_05_PI4          0x14    //I/O Configuration registers
#define	COMMAND_05_IOC0         0x18
#define	COMMAND_05_IOC1         0x19
#define	COMMAND_05_IOC2         0x1A
#define	COMMAND_05_IOC3         0x1B
#define	COMMAND_05_IOC4         0x1C    //Mask Interrupt registers
#define	COMMAND_05_MSK0         0x20
#define	COMMAND_05_MSK1         0x21
#define	COMMAND_05_MSK2         0x22
#define	COMMAND_05_MSK3         0x23
#define	COMMAND_05_MSK4         0x24

/* Auto-Increment flag is set, the 3 least significant bits are automatically incremented after a read or write */
#define AUTO_INC                0x80    


/* 若检测输入口是 PCA9505
 * 则检测输入脚被分为2部分：
 * 高4位是PORT(0~4)，低4位是bit(0~7)
 * 若检测输入口无效，则脚位也无效
 */    
typedef struct
{
    int8u  dcuK31PN_Port;   /* DCU安全回路 K31K32PN 线圈检测输入口, tEnmSFInputPort */
    int8u  dcuK31PN_Pin;    /* DCU安全回路 K31K32PN 线圈检测输入脚                  */
    
    int8u  dcuK31NC_Port;   /* DCU安全回路 K31K32触点检测输入口, tEnmSFInputPort */
    int8u  dcuK31NC_Pin;    /* DCU安全回路 K31K32触点检测输入脚                  */
    
    int8u  dcuK33NO_Port;   /* DCU安全回路 K33   触点检测输入口, tEnmSFInputPort */
    int8u  dcuK33NO_Pin;    /* DCU安全回路 K33   触点检测输入脚                  */
    
    int8u  pds_Port1;       /* PDS继电器的输入点配置信息 */
    int8u  pds_Pin1;
    int8u  pds_Port2;       /* PDS继电器的输入点配置信息 */
    int8u  pds_Pin2;
    int8u  pds_Port3;       /* PDS继电器的输入点配置信息 */
    int8u  pds_Pin3;
}tSFScanCfg;

/* =============================================================================
 * 扫描输入相关参数
 *  */
#define DBNCIN_BUF_NUM              5       /* 输入去抖动次数  */
#define MCU_GPIO_NUM                6       /* MCU（PIC32MX795F512H）的GPIO端口数  */

/* 初始化扫描结果
 *  */
PUBLIC void InitScanInputResult(void);

/* 
 * 获取变量指针的接口函数，供其它文件调用
 */
//PUBLIC tSlaveIOScanCfg * getIOScanCfgPtr(void);
PUBLIC tCmdReadIO_s    * getSlvReadIOPtr(void);
//PUBLIC tCmdWriteIO_m   * getSlvWriteIOPtr(void);

/* 
 * 初始化扫描IO的存储器
 */
//PUBLIC void initIOScanReg(tSlaveIOScanCfg *pCfg, BOOL ExeCfg);

/* 
 * 重置扫描IO存储器
 */
PUBLIC void resetScanIORegister(void);

/* 通过 reset 引脚复位 PCA9505 
 * 写入配置
 */
//PUBLIC void resetPCA9505(tSlaveIOScanCfg *cfgPtr);
    
/* ;
 * 配置 PCA9505 输入输出口
 * 返回:
 *  TRUE  : 配置成功
 *  FALSE : 配置失败
 */
//PUBLIC BOOL configPCA9505_IOC(tSlaveIOScanCfg *cfg);

/* 配置检测安全继电器的端口
 *  cfg     : 配置指针
 *  type    : 检测类型
 *  port    : 检测口      ，对于PCA9505是芯片地址 = 0x40 | (A2A1A0 << 1)
 *  pinNum  : 检测脚(序号), 对于PCA9505高4位是IO，低4位脚位
 */
//PUBLIC void configScanGPIO(tSlaveIOScanCfg *cfg, tEnmSFType type, int8u port, int8u pinNum);

/* 
 * 在主循环中调用，读取输入口
 */
PUBLIC void getPCBAllInput(void);

/* 
 * 每 1ms 调用，测试点输出信号
 */
//PUBLIC void procOutputTP(tSlaveIOScanCfg *cfg);

/* 
 * I2C ERR 处理 : 初始化PCA9505, 同时初始化芯片 I2C 接口
 * 
 */
PUBLIC void I2C_ERR_INI(void);

/* 读状态：DCU 安全继电器 线圈 K31K32PN
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkDCUSF_K31PN(void);


/* 读状态：DCU 安全继电器 触点 K31K32
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkDCUSF_K31NC(void);


/* 读状态：DCU 安全回路 触点 K33
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkDCUSF_K33NO(void);

/* 读状态：PDS 安全继电器 线圈
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkPDSSF_Coil(void);


/* 读状态：PDS 安全继电器 触点
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkPDSSF_Contactor(void);


/* 读状态：PDS 安全继电器 触点
 * 返回TRUE表示高电平，FALSE表示低电平或未配置 */
PUBLIC BOOL chkPDSSF_Contactor2(void);


/* 
 * 检测继电器状态
 * 每隔 15ms 检测一次状态并保存，最大保存状态的持续时间 64 * 15 = 960ms
 *  - 用 K31 线圈通电状态判断 K33 触点
 *  - 
 */
PUBLIC void detectSFRelayState(void);

PUBLIC void initSFDectectRegs(void);

/* 
 * 更新输出
 *  pOutputIO   : 输出配置及数据
 *  ipsFlag     : TRUE 表示 IBP / PSL / SIG 板
 * */
//PUBLIC void updateOutputIO(tCmdWriteIO_m *pOutputIO, BOOL ipsFlag);

#endif  //_IOFUNCT_H

