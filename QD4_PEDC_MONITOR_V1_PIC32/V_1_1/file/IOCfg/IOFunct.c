
//#include "..\include.h"

#include "_cpu.h"

#if (!defined(PCB_PIC24))
#include <plib.h>
#endif

#include "string.h"
#include "sysTimer.h"
#include "i2cdrv.h"
#include "IOFunct.h"

#undef  PRIVATE
#define PRIVATE

/* =============================================================================
 * 本机的配置信息
 * 上电后默认配置信息无效
 */
PRIVATE tSlaveIOScanCfg IOScanCfg;

/* 本机采集的输入 IO数据 和其它信息 
 */
PRIVATE tCmdReadIO_s slvReadIO;

/* 本机输出配置及数据
 */
PRIVATE tCmdWriteIO_m mstWriteIO;

/* 安全回路检测配置 
 */
PRIVATE tSFScanCfgs SFScanList[SF_SCAN_TYPES];

/* 
 * 用于输入口去抖动的缓存 
 */
#if (!defined(PCB_PIC24))
PRIVATE int16u readPortBuf[DBNCIN_BUF_NUM][MCU_GPIO_NUM];   /* 读 MCU GPIO */
#endif
PRIVATE int8u readIOBufPCA1[DBNCIN_BUF_NUM][5];             /* 读 PCA9505-1 IO */
PRIVATE int8u readIOBufPCA2[DBNCIN_BUF_NUM][5];             /* 读 PCA9505-2 IO */
PRIVATE int8u readIOBufPCA3[DBNCIN_BUF_NUM][5];             /* 读 PCA9505-3 IO */

/* 扫描读入输入口的计数器 
 * 每个扫描对象分别计数
 * 0-GPIO, 
 * 1-PCA9505(1), 
 * 2-PCA9505(2), 
 * 3-PCA9505(3)
 */
enum enmScanType
{
    SCAN_GPIO = 0,
    SCAN_PCA9505_1,
    SCAN_PCA9505_2,
    SCAN_PCA9505_3,
    
    SCAN_TYPES
};
PRIVATE int8u readInputCnt[SCAN_TYPES];
PRIVATE int8u firstScanInput[SCAN_TYPES];

/* 读I2C扩展芯片错误计数器 
 * 当连续扫描出现错误次数达到设置值后，设置I2C错误信息并发送给主机
 */
#define I2CERR_NUM      20
PRIVATE int8u i2cErrCnt[SCAN_TYPES];

/* 
 * 用于扫描安全回路继电器的存储器
 * 考虑PIC24 的编译器 C30，最大只支持 32位
 * 为了兼容 PIC32 ，这里不使用64位的变量 
 */
static int32u dcuK31PNBuf[2];
static int32u dcuK33Buf[2];
static int32u pdsK31PNBuf[2];
static int32u pdsK33Buf[2];


/* ========================================================= 
 * 获取变量指针的接口函数，供其它文件调用
 */
PUBLIC tSlaveIOScanCfg * getIOScanCfgPtr(void)
{
    return &IOScanCfg;
}

PUBLIC tCmdReadIO_s * getSlvReadIOPtr(void)
{
    return &slvReadIO;
}

PUBLIC tCmdWriteIO_m * getSlvWriteIOPtr(void)
{
    return &mstWriteIO;
}

/* ========================================================= 
 * 初始化扫描变量
 */
PUBLIC void InitScanInputResult(void)
{
    #if (!defined(PCB_PIC24))
    memset((int8u*)(readPortBuf), 0, sizeof(readPortBuf));
    #endif
    memset((int8u*)(readIOBufPCA1), 0xff, sizeof(readIOBufPCA1));
    memset((int8u*)(readIOBufPCA2), 0xff, sizeof(readIOBufPCA2));
    memset((int8u*)(readIOBufPCA3), 0xff, sizeof(readIOBufPCA3));

    memset((int8u*)(i2cErrCnt), 0, sizeof(i2cErrCnt));
    memset((int8u*)(readInputCnt), 0, sizeof(readInputCnt));
    memset((int8u*)(firstScanInput), 0, sizeof(firstScanInput));
}

PUBLIC void initSFDectectRegs(void)
{
    dcuK31PNBuf[0] = 0;
    dcuK31PNBuf[1] = 0;
    
    dcuK33Buf[0] = 0;
    dcuK33Buf[1] = 0;
    
    pdsK31PNBuf[0] = 0;
    pdsK31PNBuf[1] = 0;
    
    pdsK33Buf[0] = 0;
    pdsK33Buf[1] = 0;
}

#if 0
/* =========================================================
 * 初始化扫描IO的存储器
 *  pCfg    : 配置字
 *  ExeCfg  : TRUE = 执行配置
 */
PUBLIC void initIOScanReg(tSlaveIOScanCfg *pCfg, BOOL ExeCfg)
{
    IOScanCfg = *pCfg;
    if(ExeCfg == FALSE)
        return;
                    
    #if (!defined(PCB_PIC24))
    /* GPIO 配置值 */
    if(pCfg->gpioInEn == GPIOCFG_EN)
    {
        /* 8位自动转为16位 */
        int16u gpioCfg[6];
        memcpy((void*)gpioCfg, (void*)pCfg->gpioCfg, sizeof(gpioCfg));

        extern void initChipGPIOs(int16u * pCfg);
        initChipGPIOs(gpioCfg);
    }
    #endif

    /* I2C 配置 */
    if((pCfg->pca9505_En1 != PCA9505_DIS) || (pCfg->pca9505_En2 != PCA9505_DIS) ||  (pCfg->pca9505_En3 != PCA9505_DIS))
    {
        /* 输入输出初始化       */
        resetPCA9505(pCfg);
    }

    /* 检测口配置   */
    /* DCU安全回路  */
    configScanGPIO(pCfg, SF_TYPE_DCU_K31PN,  pCfg->dcuK31PN_Port, pCfg->dcuK31PN_Pin);
    configScanGPIO(pCfg, SF_TYPE_DCU_K31NC,  pCfg->dcuK31NC_Port, pCfg->dcuK31NC_Pin);
    configScanGPIO(pCfg, SF_TYPE_DCU_K33NO,  pCfg->dcuK33NO_Port, pCfg->dcuK33NO_Pin);

    /* PDS 检测 */
    configScanGPIO(pCfg, SF_TYPE_PDS_coil    ,  pCfg->pds_Port1, pCfg->pds_Pin1);
    configScanGPIO(pCfg, SF_TYPE_PDS_contact ,  pCfg->pds_Port2, pCfg->pds_Pin2);
    configScanGPIO(pCfg, SF_TYPE_PDS_contact2,  pCfg->pds_Port3, pCfg->pds_Pin3);
    
    /* 配置IO后，重置扫描输入结果 */
    memset((int8u*)&slvReadIO, 0xff, sizeof(tCmdReadIO_s));
    
    InitScanInputResult();
}

/* =========================================================
 * 重置扫描IO存储器
 */
PUBLIC void resetScanIORegister(void)
{
    int i;
    
    /* 存储器初始化 */
    memset((void*)&IOScanCfg, 0, sizeof(IOScanCfg));
    IOScanCfg.cfgValid         = 0;
    IOScanCfg.ioScanInterval   = 10;
    IOScanCfg.ioScanDbnc       = 5;
    IOScanCfg.sfScanInterval   = 5;
    
    memset((int8u*)&slvReadIO, 0xff, sizeof(tCmdReadIO_s));
    
    /* 初始化
     * 扫描安全回路的端口 */
    for(i=0; i<SF_SCAN_TYPES; i++)
    {
        SFScanList[i].type = SF_INPUT_PORT_NONE;
    }
}
#endif

/* =========================================================
 * 通过 reset 引脚复位 PCA9505 
 * 写入配置
 */
PUBLIC void resetPCA9505(tSlaveIOScanCfg *cfgPtr)
{
    int i;
    
    /* 通过MCU引脚输出复位 PCA9505 
     * tw(rst) reset pulse width 最小值 = 4ns
     * 这里通过循环进行延时保证输出复位信号的宽度，实际脉冲输出宽度未检测
     */
	PCA9505_RST_SETOUTPUT();
	for(i = 0; i<100; i++)
	{
	    PCA9505_RST_OUT0();
	}
    
	for(i = 0; i<100; i++)
	{
	    PCA9505_RST_OUT1();
	}
    
    /* 写入配置输入输出信息
     */
    //configPCA9505_IOC(cfgPtr);
    
	#if 1
    int8u IO_outputH[5] = {0xff,0xff,0xff,0xff,0xff};
    
    int8u PCA_CfgU20[5] = {0x00,0x00,0xf0,0xff,0xff};
    int8u PCA_CfgU21[5] = {0x0f,0x00,0xf0,0xff,0xff};

    int8u iniErr;
    int8u chipAddr;
    
    iniErr = 0;
    
    /* PCA9505 1 */
    chipAddr = 0;
    
    /* 默认所有输出口输出 1 */
    if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
    {
        iniErr <<= 1;
        iniErr ++;
    }

    /* 输入输出配置 */
    if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, PCA_CfgU20, 5) == FALSE)
    {
        iniErr <<= 1;
        iniErr ++;
    }
    
    /* PCA9505 2 */
    //add code here
    
    #endif
}
    
/* =========================================================
 * 配置 PCA9505 输入输出口
 * 返回:
 *  TRUE  : 配置成功
 *  FALSE : 配置失败
 */
PUBLIC BOOL configPCA9505_IOC(tSlaveIOScanCfg *cfg)
{
    #if 0
    int8u iniErr;
    int8u chipAddr;
    int8u IO_outputH[5] = {0xff,0xff,0xff,0xff,0xff};
    
    iniErr = 0;
    chipAddr = 0;
    
    /* PCA9505 的输入输出设置 */
    if((cfg->pca9505_En1 != PCA9505_DIS) && (cfg->pca9505_En1 != 0))
    {
        chipAddr = (cfg->pca9505_En1 >> 1) & 0x07;
        
        /* 默认所有输出口输出 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* 输入输出配置 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg1, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }

    if((cfg->pca9505_En2 != PCA9505_DIS) && (cfg->pca9505_En2 != 0))
    {
        chipAddr = (cfg->pca9505_En2 >> 1) & 0x07;
        
        /* 默认所有输出口输出 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* 输入输出配置 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg2, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }

    if((cfg->pca9505_En3 != PCA9505_DIS) && (cfg->pca9505_En3 != 0))
    {
        chipAddr = (cfg->pca9505_En3 >> 1) & 0x07;
        
        /* 默认所有输出口输出 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* 输入输出配置 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg3, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }
    
    /* 返回TRUE  : 配置成功 */
    return (iniErr == 0);
    
    #endif
}



/* 配置检测安全继电器的端口
 *  cfg     : 配置指针
 *  type    : 检测类型
 *  port    : 检测口      ，对于PCA9505是芯片地址 = 0x40 | (A2A1A0 << 1)
 *  pinNum  : 检测脚(序号), 对于PCA9505高4位是IO，低4位脚位
 */
PUBLIC void configScanGPIO(tSlaveIOScanCfg *cfg, tEnmSFType type, int8u port, int8u pinNum)
{
    int8u *pcaCfg;
    
    if(type >= SF_SCAN_TYPES)
        return;
    
    
    switch((tEnmSFInputPort)port)
    {
        case SF_INPUT_PORT_NONE:    //无检测口
            SFScanList[type].type = SF_INPUT_PORT_NONE;
            break;

        /* MCU GPIO 输入口 
         * 参见 IoPortId 的定义
         */
        case SF_INPUT_PB:        //MCU GPIOB
        case SF_INPUT_PC:        //MCU GPIOC
        case SF_INPUT_PD:        //MCU GPIOD
        case SF_INPUT_PE:        //MCU GPIOE
        case SF_INPUT_PF:        //MCU GPIOF
        case SF_INPUT_PG:        //MCU GPIOG
            SFScanList[type].type = type;
            SFScanList[type].port = (tEnmSFInputPort)port;
            SFScanList[type].pin  = (1<<pinNum);
            #if (!defined (PCB_PIC24))
            //unused for PIC24
            PORTSetPinsDigitalIn((IoPortId)(SFScanList[type].port), SFScanList[type].pin);
            #endif
            break;

            /* PCA9505 输入口 */
        case SF_INPUT_PCA1:         //PCA9505(1)
            pcaCfg = cfg->pca9505Cfg1;
            goto _SFConfigPCA_;
        case SF_INPUT_PCA2:         //PCA9505(2)
            pcaCfg = cfg->pca9505Cfg2;
            goto _SFConfigPCA_;
        case SF_INPUT_PCA3:         //PCA9505(3)    
            pcaCfg = cfg->pca9505Cfg3;
            
            _SFConfigPCA_:
            {
                int8u chipAddr = (port >> 1) & 0x07;
                int8u pcaIOPort = pinNum >> 4;
                int8u pcaIOPin  = 1<<(pinNum &  0x07); 

                /* 输入输出配置 */
                I2C_WriteOneByte(I2C_ID, chipAddr, COMMAND_05_IOC0 + pcaIOPort , (cfg->pca9505Cfg1[pcaIOPort] & (~pcaIOPin)) | pcaIOPin);

                SFScanList[type].type = type;
                SFScanList[type].port = (tEnmSFInputPort)pcaIOPort;
                SFScanList[type].pin  = pcaIOPin;
            }
            break;
    }
}

/* =========================================================
 * 每 1ms 调用，测试点输出信号
 */
/* 配置输出测试点
 * PORT / PIN 与检测输入口配置含义相同
 * Freq 表示输出变化的频率
 * 0 = 输出0，FF = 输出1，其它值表示输出翻转的时间间隔，单位是 ms
 * 可以在主循环处理，若为了保证输出翻转时间的准确度，可以在1ms中断中处理
 * 如输出是需要写 PCA9505，需要避免在中断中写 I2C可能会造成中断处理时间过长的问题
 *   暂时不支持 PIC24 I2C 输出
 */
PUBLIC void procOutputTP(tSlaveIOScanCfg *cfg)
{
    #if (!defined(PCB_PIC24))
    IoPortId port;
    int16u   bits;
    int8u    freq;
    
    port = cfg->tstOut_Port1;
    bits = 1 << (cfg->tstOut_Pin1 & 0x0f);
    freq = cfg->tstOut_Freq1;
    if(port != SF_INPUT_PORT_NONE)
    {
        static int8u freq_cnt1;
        if((port >= SF_INPUT_PB ) && (port <= SF_INPUT_PG))
        {
            if(freq == 0)
                PORTClearBits(port, bits);
            else if(freq == 0xff)
                PORTSetBits(port, bits);
            else
            {
                freq_cnt1++;
                if(freq_cnt1 == freq)
                {
                    freq_cnt1 = 0;
                    PORTToggleBits(port, bits);
                }
            }
        }
    }
    
    
    port = cfg->tstOut_Port2;
    bits = 1 << (cfg->tstOut_Pin2 & 0x0f);
    freq = cfg->tstOut_Freq2;
    if(port != SF_INPUT_PORT_NONE)
    {
        static int8u freq_cnt2;
        if((port >= SF_INPUT_PB ) && (port <= SF_INPUT_PG))
        {
            if(freq == 0)
                PORTClearBits(port, bits);
            else if(freq == 0xff)
                PORTSetBits(port, bits);
            else
            {
                freq_cnt2++;
                if(freq_cnt2 == freq)
                {
                    freq_cnt2 = 0;
                    PORTToggleBits(port, bits);
                }
            }
        }
    }
    #endif
}

    int8u  tstOut_Port1;    /* 输出测试点1 */
    int8u  tstOut_Pin1;
    int8u  tstOut_Freq1;
    int8u  tstOut_Port2;    /* 输出测试点2 */
    int8u  tstOut_Pin2;
    int8u  tstOut_Freq2;

    
PRIVATE void readMcuGPIOInput(void)
{
    int i;
    
    //RB~RG，共6个 GPIO，12字节
    int16u PortMsk[MCU_GPIO_NUM];
    int16u gpioDataTmp[MCU_GPIO_NUM];    
    int16u gpioData0[MCU_GPIO_NUM];
    int16u gpioData1[MCU_GPIO_NUM];    
    
    int8u ioScanDbnc = DBNCIN_BUF_NUM;

    /* GPIO MASK 位（1表示输入） */
    memcpy((int8u*)PortMsk, (int8u*)(getPic32GPIOsCfg()), sizeof(PortMsk));

    gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
    gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
    gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
    gpioDataTmp[3] = mPORTERead() & PortMsk[3];
    gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
    gpioDataTmp[5] = mPORTGRead() & PortMsk[5];

    /* 缓存每次读入的数据 */
    memcpy((int8u*)readPortBuf[readInputCnt[SCAN_GPIO]], gpioDataTmp, sizeof(gpioDataTmp));
    readInputCnt[SCAN_GPIO]++;

    /* 是否达到去抖动的次数N ?
     * 若N次的状态都相同，则送入扫描结果
     * 若N次的状态在变化，则结果不受影响
     *  */
    if(readInputCnt[SCAN_GPIO] >= ioScanDbnc)
    {
        readInputCnt[SCAN_GPIO] = ioScanDbnc;

        memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
        memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
        for(i=0; i<ioScanDbnc; i++)
        {
            /* 得到稳定的低电平 */
            gpioData0[0] |= readPortBuf[i][0];  //PORTB
            gpioData0[1] |= readPortBuf[i][1];  //PORTC
            gpioData0[2] |= readPortBuf[i][2];  //PORTD
            gpioData0[3] |= readPortBuf[i][3];  //PORTE
            gpioData0[4] |= readPortBuf[i][4];  //PORTF
            gpioData0[5] |= readPortBuf[i][5];  //PORTG

            /* 得到稳定的高电平 */
            gpioData1[0] &= readPortBuf[i][0];  //PORTB
            gpioData1[1] &= readPortBuf[i][1];  //PORTC
            gpioData1[2] &= readPortBuf[i][2];  //PORTD
            gpioData1[3] &= readPortBuf[i][3];  //PORTE
            gpioData1[4] &= readPortBuf[i][4];  //PORTF
            gpioData1[5] &= readPortBuf[i][5];  //PORTG
        }

        /* 复制以前的结果 */
        memcpy((int8u*)gpioDataTmp, (int8u*)slvReadIO.gpioIn, sizeof(slvReadIO.gpioIn));

        for(i=0; i<MCU_GPIO_NUM; i++)
        {
            /* 将稳定的低电平送入结果 */
            gpioDataTmp[i] &= gpioData0[i];

            /* 将稳定的高电平送入结果 */
            gpioDataTmp[i] |= gpioData1[i];
        }

        /* 保存本次的结果 */
        memcpy((int8u*)slvReadIO.gpioIn, (int8u*)gpioDataTmp, sizeof(slvReadIO.gpioIn));

        /* 移动扫描缓存 
         * 这样处理的方式，比起完全重新扫描N次，对 GPIO 的变化反应比较快
         */
        for(i=0; i<DBNCIN_BUF_NUM - 2; i++)
        {
            readPortBuf[i][0] = readPortBuf[i + 1][0];
            readPortBuf[i][1] = readPortBuf[i + 1][1];
            readPortBuf[i][2] = readPortBuf[i + 1][2];
            readPortBuf[i][3] = readPortBuf[i + 1][3];
            readPortBuf[i][4] = readPortBuf[i + 1][4];
            readPortBuf[i][5] = readPortBuf[i + 1][5];
        }
        readPortBuf[i][0] = 0;
        readPortBuf[i][1] = 0;
        readPortBuf[i][2] = 0;
        readPortBuf[i][3] = 0;
        readPortBuf[i][4] = 0;
        readPortBuf[i][5] = 0;

        /* 扫描得到结果 */
        //TODO 保存
    }
}
    
PUBLIC void readPCA9505Input(void)
{
    
}
    
/* =========================================================
 * 在主循环中调用，读取输入口
 * 根据 i2c 读写错误计数器设置错误信息
 */
PUBLIC void getPCBAllInput(void)
{
    static tSYSTICK prevScanTick;
    
    int i;
    tSYSTICK tickNow;
    
    tSlaveIOScanCfg *cfgPtr = &IOScanCfg;
    
    
    /* 判断读输入口时间间隔 */
    tickNow = getSysTick();
    if(getSysTimeDiff(prevScanTick) <  cfgPtr->ioScanInterval)
        return;
    
    prevScanTick = tickNow;
    
    readMcuGPIOInput();
    
    readPCA9505Input();
    
    
    /* 判断设置的去抖动次数 */
    if(cfgPtr->ioScanDbnc > DBNCIN_BUF_NUM)
    {
        cfgPtr->ioScanDbnc = DBNCIN_BUF_NUM;
    }
    
    /* 是否有 MCU GPIO 需要读入 */
    #if (!defined (PCB_PIC24))
    if(cfgPtr->gpioInEn != GPIOCFG_DIS)
    {
        //RB~RG，共6个 GPIO，12字节
        int16u PortMsk[MCU_GPIO_NUM];
        int16u gpioDataTmp[MCU_GPIO_NUM];    
        int16u gpioData0[MCU_GPIO_NUM];
        int16u gpioData1[MCU_GPIO_NUM];    
        
        /* GPIO MASK 位（1表示输入） */
        memcpy((int8u*)PortMsk, (int8u*)(cfgPtr->gpioCfg), sizeof(PortMsk));
        
        gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
        gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
        gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
        gpioDataTmp[3] = mPORTERead() & PortMsk[3];
        gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
        gpioDataTmp[5] = mPORTGRead() & PortMsk[5];
        
        /* 缓存每次读入的数据 */
        memcpy((int8u*)readPortBuf[readInputCnt[SCAN_GPIO]], gpioDataTmp, sizeof(gpioDataTmp));
        readInputCnt[SCAN_GPIO]++;
        
        /* 是否达到去抖动的次数N ?
         * 若N次的状态都相同，则送入扫描结果
         * 若N次的状态在变化，则结果不受影响
         *  */
        if(readInputCnt[SCAN_GPIO] >= cfgPtr->ioScanDbnc)
        {
            readInputCnt[SCAN_GPIO] = cfgPtr->ioScanDbnc;
            
            memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
            memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
            for(i=0; i<cfgPtr->ioScanDbnc; i++)
            {
                /* 得到稳定的低电平 */
                gpioData0[0] |= readPortBuf[i][0];  //PORTB
                gpioData0[1] |= readPortBuf[i][1];  //PORTC
                gpioData0[2] |= readPortBuf[i][2];  //PORTD
                gpioData0[3] |= readPortBuf[i][3];  //PORTE
                gpioData0[4] |= readPortBuf[i][4];  //PORTF
                gpioData0[5] |= readPortBuf[i][5];  //PORTG
                
                /* 得到稳定的高电平 */
                gpioData1[0] &= readPortBuf[i][0];  //PORTB
                gpioData1[1] &= readPortBuf[i][1];  //PORTC
                gpioData1[2] &= readPortBuf[i][2];  //PORTD
                gpioData1[3] &= readPortBuf[i][3];  //PORTE
                gpioData1[4] &= readPortBuf[i][4];  //PORTF
                gpioData1[5] &= readPortBuf[i][5];  //PORTG
            }
            
            /* 复制以前的结果 */
            memcpy((int8u*)gpioDataTmp, (int8u*)slvReadIO.gpioIn, sizeof(slvReadIO.gpioIn));
            
            for(i=0; i<MCU_GPIO_NUM; i++)
            {
                /* 将稳定的低电平送入结果 */
                gpioDataTmp[i] &= gpioData0[i];
                
                /* 将稳定的高电平送入结果 */
                gpioDataTmp[i] |= gpioData1[i];
            }
            
            /* 保存本次的结果 */
            memcpy((int8u*)slvReadIO.gpioIn, (int8u*)gpioDataTmp, sizeof(slvReadIO.gpioIn));
            
            /* 移动扫描缓存 
             * 这样处理的方式，比起完全重新扫描N次，对 GPIO 的变化反应比较快
             */
            for(i=0; i<DBNCIN_BUF_NUM - 2; i++)
            {
                readPortBuf[i][0] = readPortBuf[i + 1][0];
                readPortBuf[i][1] = readPortBuf[i + 1][1];
                readPortBuf[i][2] = readPortBuf[i + 1][2];
                readPortBuf[i][3] = readPortBuf[i + 1][3];
                readPortBuf[i][4] = readPortBuf[i + 1][4];
                readPortBuf[i][5] = readPortBuf[i + 1][5];
            }
            readPortBuf[i][0] = 0;
            readPortBuf[i][1] = 0;
            readPortBuf[i][2] = 0;
            readPortBuf[i][3] = 0;
            readPortBuf[i][4] = 0;
            readPortBuf[i][5] = 0;
            
            /* 扫描得到结果 */
            slvReadIO.gpioInEn = cfgPtr->gpioInEn;
        }
        else
        {
            /* 扫描没有结果 */
            slvReadIO.gpioInEn = GPIOCFG_DIS;
        }
    }
    else
    #endif
    {
        /* 清除 */
        memset((int8u*)slvReadIO.gpioIn, 0xff, sizeof(slvReadIO.gpioIn));
    }
    
    
    /* 
     * 读 PCA9505 输入 
     */
    int   pca9505;
    int8u i2cReadErr;
    for(pca9505 = 0; pca9505<3; pca9505++)
    {
        int8u chipAddr ;
        int8u PCAIOMsk[5];              /* 输入屏蔽位 */
        int8u PCAIODataTmp[5];          /* 临时存储 PCA9505 的输入 */

        int8u pcaIOData0[MCU_GPIO_NUM]; /* 稳定输入 0 的IO */
        int8u pcaIOData1[MCU_GPIO_NUM]; /* 稳定输入 1 的IO */

        int8u (*pPCAIOBuf)[5];          /* 二维数组指针，不能去掉括号，指向读入缓冲区 */
        int8u  *pPCAIOIn;               /* 执行结果 */
        
        tEnmPCA9505Cfg PCAIOEn;
        
        if(pca9505 == 0)
        {
            PCAIOEn     = (tEnmPCA9505Cfg)(cfgPtr->pca9505_En1);
            pPCAIOBuf   = readIOBufPCA1;
            pPCAIOIn    = (int8u*)slvReadIO.pca9505In1;
            memcpy(PCAIOMsk, cfgPtr->pca9505Cfg1, sizeof(PCAIOMsk));
        }
        else if(pca9505 == 1)
        {
            PCAIOEn     = (tEnmPCA9505Cfg)(cfgPtr->pca9505_En2);
            pPCAIOBuf   = readIOBufPCA2;
            pPCAIOIn    = (int8u*)slvReadIO.pca9505In2;
            memcpy(PCAIOMsk, cfgPtr->pca9505Cfg2, sizeof(PCAIOMsk));
        }
        else //if(pca9505 == 2)
        {
            PCAIOEn     = (tEnmPCA9505Cfg)(cfgPtr->pca9505_En3);
            pPCAIOBuf   = readIOBufPCA3;
            pPCAIOIn    = (int8u*)slvReadIO.pca9505In3;
            memcpy(PCAIOMsk, cfgPtr->pca9505Cfg3, sizeof(PCAIOMsk));
        }
        
        memset((int8u*)PCAIODataTmp, 0, sizeof(PCAIODataTmp));

        /* 是否有 PCA9505 IO 需要读入 */
        if((PCAIOEn != PCA9505_DIS) && (PCAIOEn != 0))
        {
            /* 指定的PCA9505 i2c 地址 */
            chipAddr = (PCAIOEn >> 1) & 0x07;
            
            /* 读取 I2C 数据，处理错误标志 
             */
            i2cReadErr = I2C_ReadBytes(I2C_ID, chipAddr, COMMAND_05_IP0, PCAIODataTmp, 5);
            if(i2cReadErr == false)
            {
                /* 本次读取错误，是否需要重置读取计数器？ */
                readInputCnt[pca9505 + SCAN_PCA9505_1] = 0;
                firstScanInput[pca9505 + SCAN_PCA9505_1] = 0;
                
                if(i2cErrCnt[pca9505] < 255)
                    i2cErrCnt[pca9505] ++;
                
                continue;
            }
            else
            {
                i2cErrCnt[pca9505 + SCAN_PCA9505_1] = 0;

                PCAIODataTmp[0] &= PCAIOMsk[0];
                PCAIODataTmp[1] &= PCAIOMsk[1];
                PCAIODataTmp[2] &= PCAIOMsk[2];
                PCAIODataTmp[3] &= PCAIOMsk[3];
                PCAIODataTmp[4] &= PCAIOMsk[4];

                /* 缓存每次读入的数据 */
                memcpy((int8u*)pPCAIOBuf[readInputCnt[pca9505 + SCAN_PCA9505_1]], PCAIODataTmp, sizeof(PCAIODataTmp));
                readInputCnt[pca9505 + SCAN_PCA9505_1] ++;

                /* IO 口去抖动处理
                 * 达到指定次数以后，输出结果
                 */
                if((readInputCnt[pca9505 + SCAN_PCA9505_1]) >= cfgPtr->ioScanDbnc)
                {
                    readInputCnt[pca9505 + SCAN_PCA9505_1] = cfgPtr->ioScanDbnc + 1;

                    memset((int8u*)pcaIOData0, 0x00, sizeof(pcaIOData0));
                    memset((int8u*)pcaIOData1, 0xff, sizeof(pcaIOData1));

                    for(i=0; i<cfgPtr->ioScanDbnc; i++)
                    {
                        /* 得到稳定的低电平 */
                        pcaIOData0[0] |= pPCAIOBuf[i][0];
                        pcaIOData0[1] |= pPCAIOBuf[i][1];
                        pcaIOData0[2] |= pPCAIOBuf[i][2];
                        pcaIOData0[3] |= pPCAIOBuf[i][3];
                        pcaIOData0[4] |= pPCAIOBuf[i][4];

                        /* 得到稳定的高电平 */
                        pcaIOData1[0] &= pPCAIOBuf[i][0];
                        pcaIOData1[1] &= pPCAIOBuf[i][1];
                        pcaIOData1[2] &= pPCAIOBuf[i][2];
                        pcaIOData1[3] &= pPCAIOBuf[i][3];
                        pcaIOData1[4] &= pPCAIOBuf[i][4];
                    }
                    
                    if(firstScanInput[pca9505 + SCAN_PCA9505_1] == 0)
                    {
                        /* 第一次达到扫描次数，需要重置 */
                        firstScanInput[pca9505 + SCAN_PCA9505_1] = 1;
                        memset((int8u*)PCAIODataTmp, 0x00, sizeof(PCAIODataTmp));
                    }
                    else
                    {
                        /* 复制以前的结果 */
                        memcpy((int8u*)PCAIODataTmp, pPCAIOIn, sizeof(PCAIODataTmp));
                    }
                    
                    for(i=0; i<sizeof(PCAIODataTmp); i++)
                    {
                        /* 将稳定的低电平送入结果 */
                        PCAIODataTmp[i] &= pcaIOData0[i];

                        /* 将稳定的高电平送入结果 */
                        PCAIODataTmp[i] |= pcaIOData1[i];
                    }

                    /* 保存本次的结果 */
                    memcpy(pPCAIOIn, (int8u*)PCAIODataTmp, sizeof(PCAIODataTmp));

                    /* 移动扫描缓存 
                     * 这样处理的方式，比起完全重新扫描N次，对 GPIO 的变化反应更快
                     */
                    for(i=0; i<DBNCIN_BUF_NUM - 2; i++)
                    {
                        pPCAIOBuf[i][0] = pPCAIOBuf[i + 1][0];
                        pPCAIOBuf[i][1] = pPCAIOBuf[i + 1][1];
                        pPCAIOBuf[i][2] = pPCAIOBuf[i + 1][2];
                        pPCAIOBuf[i][3] = pPCAIOBuf[i + 1][3];
                        pPCAIOBuf[i][4] = pPCAIOBuf[i + 1][4];
                    }
                    pPCAIOBuf[i][0] = 0;
                    pPCAIOBuf[i][1] = 0;
                    pPCAIOBuf[i][2] = 0;
                    pPCAIOBuf[i][3] = 0;
                    pPCAIOBuf[i][4] = 0;

                    /* 扫描得到结果 */
                    slvReadIO.pca9505_En1 = cfgPtr->pca9505_En1;
                    slvReadIO.pca9505_En2 = cfgPtr->pca9505_En2;
                    slvReadIO.pca9505_En3 = cfgPtr->pca9505_En3;
                }
                else
                {
                    /* 扫描没有结果 */
                    slvReadIO.pca9505_En1 = PCA9505_DIS;
                    slvReadIO.pca9505_En2 = PCA9505_DIS;
                    slvReadIO.pca9505_En3 = PCA9505_DIS;
                }
            }
        }
        else
        {
            /* 清除 
             * memset(pPCAIOIn, 0, slvReadIO.pca9505In1);
             */
            memcpy(pPCAIOIn, (int8u*)PCAIODataTmp, sizeof(PCAIODataTmp));
        }
    }

//TODO
//    slvReadIO.stsFlags.bits.pca9505Err1 = (i2cErrCnt[SCAN_PCA9505_1] > I2CERR_NUM) ? 1 : 0;
//    slvReadIO.stsFlags.bits.pca9505Err2 = (i2cErrCnt[SCAN_PCA9505_2] > I2CERR_NUM) ? 1 : 0;
//    slvReadIO.stsFlags.bits.pca9505Err3 = (i2cErrCnt[SCAN_PCA9505_3] > I2CERR_NUM) ? 1 : 0;
}

/* =============================================================================
 * 安全回路闪断检测
 * =============================================================================
 */
    #define SHL1_INT32U_X2(ptr, bit0 )                                       \
    {                                                                           \
        ptr[1] <<= 1;                                                           \
        if(ptr[0] & 0x80000000L)                                                \
        {                                                                       \
            ptr[1] |= 1;                                                        \
        }                                                                       \
        ptr[0] <<= 1;                                                           \
        ptr[0] |= bit0;                                                         \
    }
    
    #define CHK_STABLE_STS(powbuf, powmsk, conbuf, conmask, flag)               \
    {                                                                           \
        flag = FALSE;                                                          \
        if(((powbuf[0] & powmsk[0]) == 0) && ((powbuf[1] & powmsk[1]) == 0))    \
        {                                                                       \
            /* 线圈持续通电稳定后，检查触点是否稳定                             \
             */                                                                 \
            if(((conbuf[0] & conmask[0])  != conmask[0]) &&                     \
               ((conbuf[1] & conmask[1])  != conmask[1]))                       \
            {                                                                   \
                /* 错误 */                                                      \
                flag = TRUE;                                                    \
            }                                                                   \
        }                                                                       \
    }

/*
 * 
 * 返回TRUE表示高电平或未配置 ，FALSE表示低电平
 * 
 *                                  WTJ-GY2                NJ7                   JNR2(TY2)
 * 安全继电器端电压:  K31P/K31N     I48  RD.7              I10        RB14       I12  RB3   K31K32-PN
 *                    K31K32        I5   0-IO4_3(K-S11)    I4 (K-S6)  RB11       I5   RB7   K31K32_NC(K-S11)
 *                    K33           I9   0-IO3_7(K-S12)    I1 (K-S7)  RB2        I2   RG9   K33_KE_NO(K-S12)
 * 
 * 通过 K31K32_PN 和 K31K32_NC 判断继电器是否有故障   SF_FAULT
 * 通过 K31K32_PN 得到安全回路端电压闪断次数          TX1_data_toPEDC() -> flash
 * 通过 K31K32_NC 得到安全回路触点闪断次数            TX1_data_toPEDC() -> flash
 */

/* 读指定类型的输入口 */
PRIVATE BOOL readSFInput(tEnmSFType Type)
{
    if((int8u)Type >= SF_SCAN_TYPES)
        return FALSE;
    
    tSFScanCfgs *pSFCfg = &SFScanList[Type];
    
    if((IOScanCfg.cfgValid == CFG_VALID) && (pSFCfg->type != SF_INPUT_PORT_NONE))
    {
        if(pSFCfg->port <= SF_INPUT_PG)
        {
            #if (!defined (PCB_PIC24))
            /* MCU's GPIO */
            return (BOOL)(PORTReadBits((IoPortId)(pSFCfg->port), pSFCfg->pin));
            #else
            return FALSE;
            #endif
        }
        else
        {
            /* PCA9505's IO */
            int8u ioData;
            int8u chipAddr = (pSFCfg->port >> 1) & 0x07;
            int8u pcaIOPort = pSFCfg->pin >> 4;
            int8u pcaIOPin  = 1<<(pSFCfg->pin &  0x07); 
            if(I2C_ReadOneByte(I2C_ID, chipAddr, COMMAND_05_IP0 + pcaIOPort, &ioData) == 0)
            {
                return (BOOL)(ioData & pcaIOPin);
            }
        }
    }
    
    return TRUE;
}


/* 读状态：DCU 安全继电器 线圈 K31K32PN
 * 返回TRUE表示高电平，FALSE表示低电平 */
#ifdef EMU_TEST_FLASH
static int8u testIO;
#endif
    
PUBLIC BOOL chkDCUSF_K31PN(void)
{
    #ifdef EMU_TEST_FLASH
    testIO++;
	return (BOOL)((testIO & 4));
    #else
    return readSFInput(SF_TYPE_DCU_K31PN);
    #endif
}

/* 读状态：DCU 安全继电器 触点 K31K32
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkDCUSF_K31NC(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 2));
    #else
    return readSFInput(SF_TYPE_DCU_K31NC);
    #endif
}

/* 读状态：DCU 安全回路 触点 K33
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkDCUSF_K33NO(void)
{
    return readSFInput(SF_TYPE_DCU_K31NC);
}

/* 读状态：PDS 安全继电器 线圈
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkPDSSF_Coil(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 4));
    #else
    return readSFInput(SF_TYPE_PDS_coil);
    #endif
}

/* 读状态：PDS 安全继电器 触点
 * 返回TRUE表示高电平，FALSE表示低电平 */
PUBLIC BOOL chkPDSSF_Contactor(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 2));
    #else
    return readSFInput(SF_TYPE_PDS_contact);
    #endif
}

/* 读状态：PDS 安全继电器 触点
 * 返回TRUE表示高电平，FALSE表示低电平或未配置 */
PUBLIC BOOL chkPDSSF_Contactor2(void)
{
    return readSFInput(SF_TYPE_PDS_contact2);
}

/* =========================================================
 * 检测继电器状态
 * 每隔 15ms 检测一次状态并保存，最大保存状态的持续时间 64 * 15 = 960ms
 *  - 用 K31 线圈通电状态判断 K33 触点
 *  - 
 */
PUBLIC void detectSFRelayState(void)
{
    BOOL coilPow ;
    BOOL contactorPow;
    BOOL error;
    
    /* 配置是否有效 */
    if((IOScanCfg.cfgValid == CFG_INVALID))
        return;
    
    /* 时间间隔 */
    static tSYSTICK scanSFTick;
    tSYSTICK tNow;
    tNow = getSysTick();
    
    if(getSysTimeDiff(scanSFTick) < 15)
    {
        return;
    }
    scanSFTick = tNow;

    /* Mask 确定判断稳定的持续时间
     * eg. 0x0000000000FFFFFF 表示持续时间 24*15 = 360ms
     *     0x0000FFFFFFFFFFFF 表示持续时间 48*15 = 720ms
     *  */
    int32u PowMask[2] = {0xFFFFL, 0x0000L};
    int32u ConMask[2] = {0xFFFFL, 0x0000L};
    
    /* =========================
     * DCU 安全继电器
     */
    slvReadIO.stsFlags.bits.dcuSFRelayPow = 0;
    slvReadIO.stsFlags.bits.dcuSFRelayErr = 0;
    if((SFScanList[SF_TYPE_DCU_K31PN].type != SF_INPUT_PORT_NONE) &&
       (SFScanList[SF_TYPE_DCU_K33NO].type != SF_INPUT_PORT_NONE))
    {
        //安全继电器：用 K31K32_PN 和 K33_NO 组合判断安全继电器故障
        coilPow        = chkDCUSF_K31PN();      /* FALSE 表示通电 */
        contactorPow   = chkDCUSF_K33NO();      /* FALSE 表示通电 */
        
        /* 继电器状态，输入0 = 通电
         */
        slvReadIO.stsFlags.bits.dcuSFRelayPow = coilPow ? 0 : 1;
        
        /* 存储继电器检测 IO 输入电平，是 IO 的状态 
         */
        int32u IOSts = coilPow ? 1 : 0;
        SHL1_INT32U_X2(dcuK31PNBuf, IOSts);
        SHL1_INT32U_X2(dcuK33Buf  , IOSts);
        
        /* 判断持续稳定的状态
         */
        CHK_STABLE_STS( dcuK31PNBuf, PowMask, dcuK33Buf, ConMask, error );
        slvReadIO.stsFlags.bits.dcuSFRelayErr = error ? 1 : 0;
    }
    
    /* =========================
     * PDS 安全继电器
     */
    slvReadIO.stsFlags.bits.pdsSFRelayPow = 0;
    slvReadIO.stsFlags.bits.pdsSFRelayErr = 0;
    if((SFScanList[SF_TYPE_PDS_coil].type     != SF_INPUT_PORT_NONE) &&
       (SFScanList[SF_TYPE_PDS_contact2].type != SF_INPUT_PORT_NONE))
    {
        coilPow        = chkPDSSF_Coil();       /* FALSE 表示通电 */
        contactorPow   = chkPDSSF_Contactor2(); /* FALSE 表示通电 */
        
        /* 继电器状态，输入0 = 通电
         *  */
        slvReadIO.stsFlags.bits.pdsSFRelayPow = coilPow ? 0 : 1;
        
        int32u IOSts = coilPow ? 1 : 0;
        SHL1_INT32U_X2(pdsK31PNBuf, IOSts);
        SHL1_INT32U_X2(pdsK33Buf  , IOSts);
        
        slvReadIO.stsFlags.bits.pdsSFRelayPow = coilPow ? 0: 1;
        
        /* 判断持续稳定的状态
         *  */
        CHK_STABLE_STS( pdsK31PNBuf, PowMask, pdsK33Buf, ConMask, error );
        slvReadIO.stsFlags.bits.pdsSFRelayErr = error ? 1 : 0;
    }
}

#if 0
/* =============================================================================
 * 更新输出
 *  pOutputIO   : 输出配置及数据
 *  ipsFlag     : TRUE 表示 IBP / PSL / SIG 板
 * */
PUBLIC void updateOutputIO(tCmdWriteIO_m *pOutputIO, BOOL ipsFlag)
{
    int8u  i2cAddr;
    int8u  i2cErr;
    
    mstWriteIO  = *pOutputIO;
    
    /* GPIO output */
    #if (!defined (PCB_PIC24))
    if((ipsFlag == TRUE) && (pOutputIO->gpioOutEn == GPIOCFG_EN))
    {
        int16u gpioOutTmp[6];
        memcpy((int8u*)gpioOutTmp, pOutputIO->gpioOut, sizeof(gpioOutTmp));
        
        gpioOutTmp[2] |= 1<<1;              //PORTD.1 must out 1 for MON IBP PSL SIG
        
        PORTWrite(IOPORT_B, gpioOutTmp[0] );
        PORTWrite(IOPORT_C, gpioOutTmp[1]);
        PORTWrite(IOPORT_D, gpioOutTmp[2]);
        PORTWrite(IOPORT_E, gpioOutTmp[3]);
        PORTWrite(IOPORT_F, gpioOutTmp[4]);
        PORTWrite(IOPORT_G, gpioOutTmp[5]);
    }
    #endif
    
    /* PCA9505-1 output */
    if((pOutputIO->pca9505_En1 != PCA9505_DIS) && (pOutputIO->pca9505_En1 != 0))
    {
        /* 指定的PCA9505 i2c 地址 */
        i2cAddr = (pOutputIO->pca9505_En1 >> 1) & 0x07;
        i2cErr = I2C_WriteBuf(I2C_ID, i2cAddr, COMMAND_05_OP0, pOutputIO->pca9505Out1, 5);
        if(i2cErr == false)
        {
            if(i2cErrCnt[SCAN_PCA9505_1] < 255)
                i2cErrCnt[SCAN_PCA9505_1] ++;
        }
        else
        {
            i2cErrCnt[SCAN_PCA9505_1] = 0;
        }
    }
    
    /* PCA9505-2 output */
    if((pOutputIO->pca9505_En2 != PCA9505_DIS) && (pOutputIO->pca9505_En2 != 0))
    {
        /* 指定的PCA9505 i2c 地址 */
        i2cAddr = (pOutputIO->pca9505_En2 >> 1) & 0x07;
        i2cErr = I2C_WriteBuf(I2C_ID, i2cAddr, COMMAND_05_OP0, pOutputIO->pca9505Out2, 5);
        if(i2cErr == false)
        {
            if(i2cErrCnt[SCAN_PCA9505_2] < 255)
                i2cErrCnt[SCAN_PCA9505_2] ++;
        }
        else
        {
            i2cErrCnt[SCAN_PCA9505_2] = 0;
        }
    }
    
    /* PCA9505-3 output */
    if((pOutputIO->pca9505_En3 != PCA9505_DIS) && (pOutputIO->pca9505_En3 != 0))
    {
        /* 指定的PCA9505 i2c 地址 */
        i2cAddr = (pOutputIO->pca9505_En3 >> 1) & 0x07;
        i2cErr = I2C_WriteBuf(I2C_ID, i2cAddr, COMMAND_05_OP0, pOutputIO->pca9505Out3, 5);
        if(i2cErr == false)
        {
            if(i2cErrCnt[SCAN_PCA9505_3] < 255)
                i2cErrCnt[SCAN_PCA9505_3] ++;
        }
        else
        {
            i2cErrCnt[SCAN_PCA9505_3] = 0;
        }
    }
}
#endif
