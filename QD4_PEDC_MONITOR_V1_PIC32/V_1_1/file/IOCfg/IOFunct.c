
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
 * ������������Ϣ
 * �ϵ��Ĭ��������Ϣ��Ч
 */
PRIVATE tSlaveIOScanCfg IOScanCfg;

/* �����ɼ������� IO���� ��������Ϣ 
 */
PRIVATE tCmdReadIO_s slvReadIO;

/* ����������ü�����
 */
PRIVATE tCmdWriteIO_m mstWriteIO;

/* ��ȫ��·������� 
 */
PRIVATE tSFScanCfgs SFScanList[SF_SCAN_TYPES];

/* 
 * ���������ȥ�����Ļ��� 
 */
#if (!defined(PCB_PIC24))
PRIVATE int16u readPortBuf[DBNCIN_BUF_NUM][MCU_GPIO_NUM];   /* �� MCU GPIO */
#endif
PRIVATE int8u readIOBufPCA1[DBNCIN_BUF_NUM][5];             /* �� PCA9505-1 IO */
PRIVATE int8u readIOBufPCA2[DBNCIN_BUF_NUM][5];             /* �� PCA9505-2 IO */
PRIVATE int8u readIOBufPCA3[DBNCIN_BUF_NUM][5];             /* �� PCA9505-3 IO */

/* ɨ���������ڵļ����� 
 * ÿ��ɨ�����ֱ����
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

/* ��I2C��չоƬ��������� 
 * ������ɨ����ִ�������ﵽ����ֵ������I2C������Ϣ�����͸�����
 */
#define I2CERR_NUM      20
PRIVATE int8u i2cErrCnt[SCAN_TYPES];

/* 
 * ����ɨ�谲ȫ��·�̵����Ĵ洢��
 * ����PIC24 �ı����� C30�����ֻ֧�� 32λ
 * Ϊ�˼��� PIC32 �����ﲻʹ��64λ�ı��� 
 */
static int32u dcuK31PNBuf[2];
static int32u dcuK33Buf[2];
static int32u pdsK31PNBuf[2];
static int32u pdsK33Buf[2];


/* ========================================================= 
 * ��ȡ����ָ��Ľӿں������������ļ�����
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
 * ��ʼ��ɨ�����
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
 * ��ʼ��ɨ��IO�Ĵ洢��
 *  pCfg    : ������
 *  ExeCfg  : TRUE = ִ������
 */
PUBLIC void initIOScanReg(tSlaveIOScanCfg *pCfg, BOOL ExeCfg)
{
    IOScanCfg = *pCfg;
    if(ExeCfg == FALSE)
        return;
                    
    #if (!defined(PCB_PIC24))
    /* GPIO ����ֵ */
    if(pCfg->gpioInEn == GPIOCFG_EN)
    {
        /* 8λ�Զ�תΪ16λ */
        int16u gpioCfg[6];
        memcpy((void*)gpioCfg, (void*)pCfg->gpioCfg, sizeof(gpioCfg));

        extern void initChipGPIOs(int16u * pCfg);
        initChipGPIOs(gpioCfg);
    }
    #endif

    /* I2C ���� */
    if((pCfg->pca9505_En1 != PCA9505_DIS) || (pCfg->pca9505_En2 != PCA9505_DIS) ||  (pCfg->pca9505_En3 != PCA9505_DIS))
    {
        /* ���������ʼ��       */
        resetPCA9505(pCfg);
    }

    /* ��������   */
    /* DCU��ȫ��·  */
    configScanGPIO(pCfg, SF_TYPE_DCU_K31PN,  pCfg->dcuK31PN_Port, pCfg->dcuK31PN_Pin);
    configScanGPIO(pCfg, SF_TYPE_DCU_K31NC,  pCfg->dcuK31NC_Port, pCfg->dcuK31NC_Pin);
    configScanGPIO(pCfg, SF_TYPE_DCU_K33NO,  pCfg->dcuK33NO_Port, pCfg->dcuK33NO_Pin);

    /* PDS ��� */
    configScanGPIO(pCfg, SF_TYPE_PDS_coil    ,  pCfg->pds_Port1, pCfg->pds_Pin1);
    configScanGPIO(pCfg, SF_TYPE_PDS_contact ,  pCfg->pds_Port2, pCfg->pds_Pin2);
    configScanGPIO(pCfg, SF_TYPE_PDS_contact2,  pCfg->pds_Port3, pCfg->pds_Pin3);
    
    /* ����IO������ɨ�������� */
    memset((int8u*)&slvReadIO, 0xff, sizeof(tCmdReadIO_s));
    
    InitScanInputResult();
}

/* =========================================================
 * ����ɨ��IO�洢��
 */
PUBLIC void resetScanIORegister(void)
{
    int i;
    
    /* �洢����ʼ�� */
    memset((void*)&IOScanCfg, 0, sizeof(IOScanCfg));
    IOScanCfg.cfgValid         = 0;
    IOScanCfg.ioScanInterval   = 10;
    IOScanCfg.ioScanDbnc       = 5;
    IOScanCfg.sfScanInterval   = 5;
    
    memset((int8u*)&slvReadIO, 0xff, sizeof(tCmdReadIO_s));
    
    /* ��ʼ��
     * ɨ�谲ȫ��·�Ķ˿� */
    for(i=0; i<SF_SCAN_TYPES; i++)
    {
        SFScanList[i].type = SF_INPUT_PORT_NONE;
    }
}
#endif

/* =========================================================
 * ͨ�� reset ���Ÿ�λ PCA9505 
 * д������
 */
PUBLIC void resetPCA9505(tSlaveIOScanCfg *cfgPtr)
{
    int i;
    
    /* ͨ��MCU���������λ PCA9505 
     * tw(rst) reset pulse width ��Сֵ = 4ns
     * ����ͨ��ѭ��������ʱ��֤�����λ�źŵĿ�ȣ�ʵ������������δ���
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
    
    /* д���������������Ϣ
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
    
    /* Ĭ�������������� 1 */
    if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
    {
        iniErr <<= 1;
        iniErr ++;
    }

    /* ����������� */
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
 * ���� PCA9505 ���������
 * ����:
 *  TRUE  : ���óɹ�
 *  FALSE : ����ʧ��
 */
PUBLIC BOOL configPCA9505_IOC(tSlaveIOScanCfg *cfg)
{
    #if 0
    int8u iniErr;
    int8u chipAddr;
    int8u IO_outputH[5] = {0xff,0xff,0xff,0xff,0xff};
    
    iniErr = 0;
    chipAddr = 0;
    
    /* PCA9505 ������������� */
    if((cfg->pca9505_En1 != PCA9505_DIS) && (cfg->pca9505_En1 != 0))
    {
        chipAddr = (cfg->pca9505_En1 >> 1) & 0x07;
        
        /* Ĭ�������������� 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* ����������� */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg1, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }

    if((cfg->pca9505_En2 != PCA9505_DIS) && (cfg->pca9505_En2 != 0))
    {
        chipAddr = (cfg->pca9505_En2 >> 1) & 0x07;
        
        /* Ĭ�������������� 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* ����������� */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg2, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }

    if((cfg->pca9505_En3 != PCA9505_DIS) && (cfg->pca9505_En3 != 0))
    {
        chipAddr = (cfg->pca9505_En3 >> 1) & 0x07;
        
        /* Ĭ�������������� 1 */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_OP0 | AUTO_INC,  IO_outputH, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
        
        /* ����������� */
        if(I2C_WriteBuf(I2C_ID, chipAddr, COMMAND_05_IOC0 | AUTO_INC, cfg->pca9505Cfg3, 5) == FALSE)
        {
            iniErr <<= 1;
            iniErr ++;
        }
    }
    
    /* ����TRUE  : ���óɹ� */
    return (iniErr == 0);
    
    #endif
}



/* ���ü�ⰲȫ�̵����Ķ˿�
 *  cfg     : ����ָ��
 *  type    : �������
 *  port    : ����      ������PCA9505��оƬ��ַ = 0x40 | (A2A1A0 << 1)
 *  pinNum  : ����(���), ����PCA9505��4λ��IO����4λ��λ
 */
PUBLIC void configScanGPIO(tSlaveIOScanCfg *cfg, tEnmSFType type, int8u port, int8u pinNum)
{
    int8u *pcaCfg;
    
    if(type >= SF_SCAN_TYPES)
        return;
    
    
    switch((tEnmSFInputPort)port)
    {
        case SF_INPUT_PORT_NONE:    //�޼���
            SFScanList[type].type = SF_INPUT_PORT_NONE;
            break;

        /* MCU GPIO ����� 
         * �μ� IoPortId �Ķ���
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

            /* PCA9505 ����� */
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

                /* ����������� */
                I2C_WriteOneByte(I2C_ID, chipAddr, COMMAND_05_IOC0 + pcaIOPort , (cfg->pca9505Cfg1[pcaIOPort] & (~pcaIOPin)) | pcaIOPin);

                SFScanList[type].type = type;
                SFScanList[type].port = (tEnmSFInputPort)pcaIOPort;
                SFScanList[type].pin  = pcaIOPin;
            }
            break;
    }
}

/* =========================================================
 * ÿ 1ms ���ã����Ե�����ź�
 */
/* ����������Ե�
 * PORT / PIN ������������ú�����ͬ
 * Freq ��ʾ����仯��Ƶ��
 * 0 = ���0��FF = ���1������ֵ��ʾ�����ת��ʱ��������λ�� ms
 * ��������ѭ��������Ϊ�˱�֤�����תʱ���׼ȷ�ȣ�������1ms�ж��д���
 * ���������Ҫд PCA9505����Ҫ�������ж���д I2C���ܻ�����жϴ���ʱ�����������
 *   ��ʱ��֧�� PIC24 I2C ���
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

    int8u  tstOut_Port1;    /* ������Ե�1 */
    int8u  tstOut_Pin1;
    int8u  tstOut_Freq1;
    int8u  tstOut_Port2;    /* ������Ե�2 */
    int8u  tstOut_Pin2;
    int8u  tstOut_Freq2;

    
PRIVATE void readMcuGPIOInput(void)
{
    int i;
    
    //RB~RG����6�� GPIO��12�ֽ�
    int16u PortMsk[MCU_GPIO_NUM];
    int16u gpioDataTmp[MCU_GPIO_NUM];    
    int16u gpioData0[MCU_GPIO_NUM];
    int16u gpioData1[MCU_GPIO_NUM];    
    
    int8u ioScanDbnc = DBNCIN_BUF_NUM;

    /* GPIO MASK λ��1��ʾ���룩 */
    memcpy((int8u*)PortMsk, (int8u*)(getPic32GPIOsCfg()), sizeof(PortMsk));

    gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
    gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
    gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
    gpioDataTmp[3] = mPORTERead() & PortMsk[3];
    gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
    gpioDataTmp[5] = mPORTGRead() & PortMsk[5];

    /* ����ÿ�ζ�������� */
    memcpy((int8u*)readPortBuf[readInputCnt[SCAN_GPIO]], gpioDataTmp, sizeof(gpioDataTmp));
    readInputCnt[SCAN_GPIO]++;

    /* �Ƿ�ﵽȥ�����Ĵ���N ?
     * ��N�ε�״̬����ͬ��������ɨ����
     * ��N�ε�״̬�ڱ仯����������Ӱ��
     *  */
    if(readInputCnt[SCAN_GPIO] >= ioScanDbnc)
    {
        readInputCnt[SCAN_GPIO] = ioScanDbnc;

        memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
        memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
        for(i=0; i<ioScanDbnc; i++)
        {
            /* �õ��ȶ��ĵ͵�ƽ */
            gpioData0[0] |= readPortBuf[i][0];  //PORTB
            gpioData0[1] |= readPortBuf[i][1];  //PORTC
            gpioData0[2] |= readPortBuf[i][2];  //PORTD
            gpioData0[3] |= readPortBuf[i][3];  //PORTE
            gpioData0[4] |= readPortBuf[i][4];  //PORTF
            gpioData0[5] |= readPortBuf[i][5];  //PORTG

            /* �õ��ȶ��ĸߵ�ƽ */
            gpioData1[0] &= readPortBuf[i][0];  //PORTB
            gpioData1[1] &= readPortBuf[i][1];  //PORTC
            gpioData1[2] &= readPortBuf[i][2];  //PORTD
            gpioData1[3] &= readPortBuf[i][3];  //PORTE
            gpioData1[4] &= readPortBuf[i][4];  //PORTF
            gpioData1[5] &= readPortBuf[i][5];  //PORTG
        }

        /* ������ǰ�Ľ�� */
        memcpy((int8u*)gpioDataTmp, (int8u*)slvReadIO.gpioIn, sizeof(slvReadIO.gpioIn));

        for(i=0; i<MCU_GPIO_NUM; i++)
        {
            /* ���ȶ��ĵ͵�ƽ������ */
            gpioDataTmp[i] &= gpioData0[i];

            /* ���ȶ��ĸߵ�ƽ������ */
            gpioDataTmp[i] |= gpioData1[i];
        }

        /* ���汾�εĽ�� */
        memcpy((int8u*)slvReadIO.gpioIn, (int8u*)gpioDataTmp, sizeof(slvReadIO.gpioIn));

        /* �ƶ�ɨ�軺�� 
         * ��������ķ�ʽ��������ȫ����ɨ��N�Σ��� GPIO �ı仯��Ӧ�ȽϿ�
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

        /* ɨ��õ���� */
        //TODO ����
    }
}
    
PUBLIC void readPCA9505Input(void)
{
    
}
    
/* =========================================================
 * ����ѭ���е��ã���ȡ�����
 * ���� i2c ��д������������ô�����Ϣ
 */
PUBLIC void getPCBAllInput(void)
{
    static tSYSTICK prevScanTick;
    
    int i;
    tSYSTICK tickNow;
    
    tSlaveIOScanCfg *cfgPtr = &IOScanCfg;
    
    
    /* �ж϶������ʱ���� */
    tickNow = getSysTick();
    if(getSysTimeDiff(prevScanTick) <  cfgPtr->ioScanInterval)
        return;
    
    prevScanTick = tickNow;
    
    readMcuGPIOInput();
    
    readPCA9505Input();
    
    
    /* �ж����õ�ȥ�������� */
    if(cfgPtr->ioScanDbnc > DBNCIN_BUF_NUM)
    {
        cfgPtr->ioScanDbnc = DBNCIN_BUF_NUM;
    }
    
    /* �Ƿ��� MCU GPIO ��Ҫ���� */
    #if (!defined (PCB_PIC24))
    if(cfgPtr->gpioInEn != GPIOCFG_DIS)
    {
        //RB~RG����6�� GPIO��12�ֽ�
        int16u PortMsk[MCU_GPIO_NUM];
        int16u gpioDataTmp[MCU_GPIO_NUM];    
        int16u gpioData0[MCU_GPIO_NUM];
        int16u gpioData1[MCU_GPIO_NUM];    
        
        /* GPIO MASK λ��1��ʾ���룩 */
        memcpy((int8u*)PortMsk, (int8u*)(cfgPtr->gpioCfg), sizeof(PortMsk));
        
        gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
        gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
        gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
        gpioDataTmp[3] = mPORTERead() & PortMsk[3];
        gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
        gpioDataTmp[5] = mPORTGRead() & PortMsk[5];
        
        /* ����ÿ�ζ�������� */
        memcpy((int8u*)readPortBuf[readInputCnt[SCAN_GPIO]], gpioDataTmp, sizeof(gpioDataTmp));
        readInputCnt[SCAN_GPIO]++;
        
        /* �Ƿ�ﵽȥ�����Ĵ���N ?
         * ��N�ε�״̬����ͬ��������ɨ����
         * ��N�ε�״̬�ڱ仯����������Ӱ��
         *  */
        if(readInputCnt[SCAN_GPIO] >= cfgPtr->ioScanDbnc)
        {
            readInputCnt[SCAN_GPIO] = cfgPtr->ioScanDbnc;
            
            memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
            memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
            for(i=0; i<cfgPtr->ioScanDbnc; i++)
            {
                /* �õ��ȶ��ĵ͵�ƽ */
                gpioData0[0] |= readPortBuf[i][0];  //PORTB
                gpioData0[1] |= readPortBuf[i][1];  //PORTC
                gpioData0[2] |= readPortBuf[i][2];  //PORTD
                gpioData0[3] |= readPortBuf[i][3];  //PORTE
                gpioData0[4] |= readPortBuf[i][4];  //PORTF
                gpioData0[5] |= readPortBuf[i][5];  //PORTG
                
                /* �õ��ȶ��ĸߵ�ƽ */
                gpioData1[0] &= readPortBuf[i][0];  //PORTB
                gpioData1[1] &= readPortBuf[i][1];  //PORTC
                gpioData1[2] &= readPortBuf[i][2];  //PORTD
                gpioData1[3] &= readPortBuf[i][3];  //PORTE
                gpioData1[4] &= readPortBuf[i][4];  //PORTF
                gpioData1[5] &= readPortBuf[i][5];  //PORTG
            }
            
            /* ������ǰ�Ľ�� */
            memcpy((int8u*)gpioDataTmp, (int8u*)slvReadIO.gpioIn, sizeof(slvReadIO.gpioIn));
            
            for(i=0; i<MCU_GPIO_NUM; i++)
            {
                /* ���ȶ��ĵ͵�ƽ������ */
                gpioDataTmp[i] &= gpioData0[i];
                
                /* ���ȶ��ĸߵ�ƽ������ */
                gpioDataTmp[i] |= gpioData1[i];
            }
            
            /* ���汾�εĽ�� */
            memcpy((int8u*)slvReadIO.gpioIn, (int8u*)gpioDataTmp, sizeof(slvReadIO.gpioIn));
            
            /* �ƶ�ɨ�軺�� 
             * ��������ķ�ʽ��������ȫ����ɨ��N�Σ��� GPIO �ı仯��Ӧ�ȽϿ�
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
            
            /* ɨ��õ���� */
            slvReadIO.gpioInEn = cfgPtr->gpioInEn;
        }
        else
        {
            /* ɨ��û�н�� */
            slvReadIO.gpioInEn = GPIOCFG_DIS;
        }
    }
    else
    #endif
    {
        /* ��� */
        memset((int8u*)slvReadIO.gpioIn, 0xff, sizeof(slvReadIO.gpioIn));
    }
    
    
    /* 
     * �� PCA9505 ���� 
     */
    int   pca9505;
    int8u i2cReadErr;
    for(pca9505 = 0; pca9505<3; pca9505++)
    {
        int8u chipAddr ;
        int8u PCAIOMsk[5];              /* ��������λ */
        int8u PCAIODataTmp[5];          /* ��ʱ�洢 PCA9505 ������ */

        int8u pcaIOData0[MCU_GPIO_NUM]; /* �ȶ����� 0 ��IO */
        int8u pcaIOData1[MCU_GPIO_NUM]; /* �ȶ����� 1 ��IO */

        int8u (*pPCAIOBuf)[5];          /* ��ά����ָ�룬����ȥ�����ţ�ָ����뻺���� */
        int8u  *pPCAIOIn;               /* ִ�н�� */
        
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

        /* �Ƿ��� PCA9505 IO ��Ҫ���� */
        if((PCAIOEn != PCA9505_DIS) && (PCAIOEn != 0))
        {
            /* ָ����PCA9505 i2c ��ַ */
            chipAddr = (PCAIOEn >> 1) & 0x07;
            
            /* ��ȡ I2C ���ݣ���������־ 
             */
            i2cReadErr = I2C_ReadBytes(I2C_ID, chipAddr, COMMAND_05_IP0, PCAIODataTmp, 5);
            if(i2cReadErr == false)
            {
                /* ���ζ�ȡ�����Ƿ���Ҫ���ö�ȡ�������� */
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

                /* ����ÿ�ζ�������� */
                memcpy((int8u*)pPCAIOBuf[readInputCnt[pca9505 + SCAN_PCA9505_1]], PCAIODataTmp, sizeof(PCAIODataTmp));
                readInputCnt[pca9505 + SCAN_PCA9505_1] ++;

                /* IO ��ȥ��������
                 * �ﵽָ�������Ժ�������
                 */
                if((readInputCnt[pca9505 + SCAN_PCA9505_1]) >= cfgPtr->ioScanDbnc)
                {
                    readInputCnt[pca9505 + SCAN_PCA9505_1] = cfgPtr->ioScanDbnc + 1;

                    memset((int8u*)pcaIOData0, 0x00, sizeof(pcaIOData0));
                    memset((int8u*)pcaIOData1, 0xff, sizeof(pcaIOData1));

                    for(i=0; i<cfgPtr->ioScanDbnc; i++)
                    {
                        /* �õ��ȶ��ĵ͵�ƽ */
                        pcaIOData0[0] |= pPCAIOBuf[i][0];
                        pcaIOData0[1] |= pPCAIOBuf[i][1];
                        pcaIOData0[2] |= pPCAIOBuf[i][2];
                        pcaIOData0[3] |= pPCAIOBuf[i][3];
                        pcaIOData0[4] |= pPCAIOBuf[i][4];

                        /* �õ��ȶ��ĸߵ�ƽ */
                        pcaIOData1[0] &= pPCAIOBuf[i][0];
                        pcaIOData1[1] &= pPCAIOBuf[i][1];
                        pcaIOData1[2] &= pPCAIOBuf[i][2];
                        pcaIOData1[3] &= pPCAIOBuf[i][3];
                        pcaIOData1[4] &= pPCAIOBuf[i][4];
                    }
                    
                    if(firstScanInput[pca9505 + SCAN_PCA9505_1] == 0)
                    {
                        /* ��һ�δﵽɨ���������Ҫ���� */
                        firstScanInput[pca9505 + SCAN_PCA9505_1] = 1;
                        memset((int8u*)PCAIODataTmp, 0x00, sizeof(PCAIODataTmp));
                    }
                    else
                    {
                        /* ������ǰ�Ľ�� */
                        memcpy((int8u*)PCAIODataTmp, pPCAIOIn, sizeof(PCAIODataTmp));
                    }
                    
                    for(i=0; i<sizeof(PCAIODataTmp); i++)
                    {
                        /* ���ȶ��ĵ͵�ƽ������ */
                        PCAIODataTmp[i] &= pcaIOData0[i];

                        /* ���ȶ��ĸߵ�ƽ������ */
                        PCAIODataTmp[i] |= pcaIOData1[i];
                    }

                    /* ���汾�εĽ�� */
                    memcpy(pPCAIOIn, (int8u*)PCAIODataTmp, sizeof(PCAIODataTmp));

                    /* �ƶ�ɨ�軺�� 
                     * ��������ķ�ʽ��������ȫ����ɨ��N�Σ��� GPIO �ı仯��Ӧ����
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

                    /* ɨ��õ���� */
                    slvReadIO.pca9505_En1 = cfgPtr->pca9505_En1;
                    slvReadIO.pca9505_En2 = cfgPtr->pca9505_En2;
                    slvReadIO.pca9505_En3 = cfgPtr->pca9505_En3;
                }
                else
                {
                    /* ɨ��û�н�� */
                    slvReadIO.pca9505_En1 = PCA9505_DIS;
                    slvReadIO.pca9505_En2 = PCA9505_DIS;
                    slvReadIO.pca9505_En3 = PCA9505_DIS;
                }
            }
        }
        else
        {
            /* ��� 
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
 * ��ȫ��·���ϼ��
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
            /* ��Ȧ����ͨ���ȶ��󣬼�鴥���Ƿ��ȶ�                             \
             */                                                                 \
            if(((conbuf[0] & conmask[0])  != conmask[0]) &&                     \
               ((conbuf[1] & conmask[1])  != conmask[1]))                       \
            {                                                                   \
                /* ���� */                                                      \
                flag = TRUE;                                                    \
            }                                                                   \
        }                                                                       \
    }

/*
 * 
 * ����TRUE��ʾ�ߵ�ƽ��δ���� ��FALSE��ʾ�͵�ƽ
 * 
 *                                  WTJ-GY2                NJ7                   JNR2(TY2)
 * ��ȫ�̵����˵�ѹ:  K31P/K31N     I48  RD.7              I10        RB14       I12  RB3   K31K32-PN
 *                    K31K32        I5   0-IO4_3(K-S11)    I4 (K-S6)  RB11       I5   RB7   K31K32_NC(K-S11)
 *                    K33           I9   0-IO3_7(K-S12)    I1 (K-S7)  RB2        I2   RG9   K33_KE_NO(K-S12)
 * 
 * ͨ�� K31K32_PN �� K31K32_NC �жϼ̵����Ƿ��й���   SF_FAULT
 * ͨ�� K31K32_PN �õ���ȫ��·�˵�ѹ���ϴ���          TX1_data_toPEDC() -> flash
 * ͨ�� K31K32_NC �õ���ȫ��·�������ϴ���            TX1_data_toPEDC() -> flash
 */

/* ��ָ�����͵������ */
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


/* ��״̬��DCU ��ȫ�̵��� ��Ȧ K31K32PN
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
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

/* ��״̬��DCU ��ȫ�̵��� ���� K31K32
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkDCUSF_K31NC(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 2));
    #else
    return readSFInput(SF_TYPE_DCU_K31NC);
    #endif
}

/* ��״̬��DCU ��ȫ��· ���� K33
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkDCUSF_K33NO(void)
{
    return readSFInput(SF_TYPE_DCU_K31NC);
}

/* ��״̬��PDS ��ȫ�̵��� ��Ȧ
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkPDSSF_Coil(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 4));
    #else
    return readSFInput(SF_TYPE_PDS_coil);
    #endif
}

/* ��״̬��PDS ��ȫ�̵��� ����
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkPDSSF_Contactor(void)
{
    #ifdef EMU_TEST_FLASH
	return (BOOL)((testIO & 2));
    #else
    return readSFInput(SF_TYPE_PDS_contact);
    #endif
}

/* ��״̬��PDS ��ȫ�̵��� ����
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ��δ���� */
PUBLIC BOOL chkPDSSF_Contactor2(void)
{
    return readSFInput(SF_TYPE_PDS_contact2);
}

/* =========================================================
 * ���̵���״̬
 * ÿ�� 15ms ���һ��״̬�����棬��󱣴�״̬�ĳ���ʱ�� 64 * 15 = 960ms
 *  - �� K31 ��Ȧͨ��״̬�ж� K33 ����
 *  - 
 */
PUBLIC void detectSFRelayState(void)
{
    BOOL coilPow ;
    BOOL contactorPow;
    BOOL error;
    
    /* �����Ƿ���Ч */
    if((IOScanCfg.cfgValid == CFG_INVALID))
        return;
    
    /* ʱ���� */
    static tSYSTICK scanSFTick;
    tSYSTICK tNow;
    tNow = getSysTick();
    
    if(getSysTimeDiff(scanSFTick) < 15)
    {
        return;
    }
    scanSFTick = tNow;

    /* Mask ȷ���ж��ȶ��ĳ���ʱ��
     * eg. 0x0000000000FFFFFF ��ʾ����ʱ�� 24*15 = 360ms
     *     0x0000FFFFFFFFFFFF ��ʾ����ʱ�� 48*15 = 720ms
     *  */
    int32u PowMask[2] = {0xFFFFL, 0x0000L};
    int32u ConMask[2] = {0xFFFFL, 0x0000L};
    
    /* =========================
     * DCU ��ȫ�̵���
     */
    slvReadIO.stsFlags.bits.dcuSFRelayPow = 0;
    slvReadIO.stsFlags.bits.dcuSFRelayErr = 0;
    if((SFScanList[SF_TYPE_DCU_K31PN].type != SF_INPUT_PORT_NONE) &&
       (SFScanList[SF_TYPE_DCU_K33NO].type != SF_INPUT_PORT_NONE))
    {
        //��ȫ�̵������� K31K32_PN �� K33_NO ����жϰ�ȫ�̵�������
        coilPow        = chkDCUSF_K31PN();      /* FALSE ��ʾͨ�� */
        contactorPow   = chkDCUSF_K33NO();      /* FALSE ��ʾͨ�� */
        
        /* �̵���״̬������0 = ͨ��
         */
        slvReadIO.stsFlags.bits.dcuSFRelayPow = coilPow ? 0 : 1;
        
        /* �洢�̵������ IO �����ƽ���� IO ��״̬ 
         */
        int32u IOSts = coilPow ? 1 : 0;
        SHL1_INT32U_X2(dcuK31PNBuf, IOSts);
        SHL1_INT32U_X2(dcuK33Buf  , IOSts);
        
        /* �жϳ����ȶ���״̬
         */
        CHK_STABLE_STS( dcuK31PNBuf, PowMask, dcuK33Buf, ConMask, error );
        slvReadIO.stsFlags.bits.dcuSFRelayErr = error ? 1 : 0;
    }
    
    /* =========================
     * PDS ��ȫ�̵���
     */
    slvReadIO.stsFlags.bits.pdsSFRelayPow = 0;
    slvReadIO.stsFlags.bits.pdsSFRelayErr = 0;
    if((SFScanList[SF_TYPE_PDS_coil].type     != SF_INPUT_PORT_NONE) &&
       (SFScanList[SF_TYPE_PDS_contact2].type != SF_INPUT_PORT_NONE))
    {
        coilPow        = chkPDSSF_Coil();       /* FALSE ��ʾͨ�� */
        contactorPow   = chkPDSSF_Contactor2(); /* FALSE ��ʾͨ�� */
        
        /* �̵���״̬������0 = ͨ��
         *  */
        slvReadIO.stsFlags.bits.pdsSFRelayPow = coilPow ? 0 : 1;
        
        int32u IOSts = coilPow ? 1 : 0;
        SHL1_INT32U_X2(pdsK31PNBuf, IOSts);
        SHL1_INT32U_X2(pdsK33Buf  , IOSts);
        
        slvReadIO.stsFlags.bits.pdsSFRelayPow = coilPow ? 0: 1;
        
        /* �жϳ����ȶ���״̬
         *  */
        CHK_STABLE_STS( pdsK31PNBuf, PowMask, pdsK33Buf, ConMask, error );
        slvReadIO.stsFlags.bits.pdsSFRelayErr = error ? 1 : 0;
    }
}

#if 0
/* =============================================================================
 * �������
 *  pOutputIO   : ������ü�����
 *  ipsFlag     : TRUE ��ʾ IBP / PSL / SIG ��
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
        /* ָ����PCA9505 i2c ��ַ */
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
        /* ָ����PCA9505 i2c ��ַ */
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
        /* ָ����PCA9505 i2c ��ַ */
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
