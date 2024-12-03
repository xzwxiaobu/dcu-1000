#include "Include.h"

#include "mainIO.h"
#include "PCBIOStruct.h"
#include "commdef.h"

#undef  PRIVATE
#define PRIVATE

/* 离线芯片的输入数据是否重置
 * 0      = 维持旧数据
 * 不为0  = 重置
 *  */
#define RESET_INPUT_OFFLINE     0


#define MCU_GPIO_NUM            6       /* GPIO : B C D E F G */
#define IO_SCAN_DBNC_NUM        5       /* 扫描IO的去抖动次数   */
#define IO_SCAN_INTERVAL        8       /* 扫描时间间隔, ms  */
PRIVATE int16u readPortBuf[IO_SCAN_DBNC_NUM][MCU_GPIO_NUM];     /* 读 MCU GPIO */
PRIVATE int8u  readInputCnt;
PRIVATE int8u  firstScanInput;

/* =============================================================================*/
/* PEDC 拨码 */
PRIVATE int8u PEDC_ID;
PUBLIC int8u getPedcID(void)
{
    return PEDC_ID;
}

PRIVATE int8u sig1PcbID,sig1PcbID_Prev;
PRIVATE int8u sig2PcbID,sig2PcbID_Prev;

/* =============================================================================*/
/* 从机相关信息存储
 * 包括 PIC24A PIC24B LOGA LOGB
 * 定义了 MONITOR 实际没用
 *  */
PRIVATE tSlaveInfo SlaveInfo[COMMOBJ_TOTAL];

/* 取从机相关信息指针，供外部获取使用
 */
PUBLIC tSlaveInfo* getSlaveInfoPtr(tEnmCommObj obj)
{
    if(obj >= (COMMOBJ_TOTAL + COMMOBJ_FIRST) )
        return NULL;
    else
        return &(SlaveInfo[obj - COMMOBJ_FIRST]);
}

/* 获取从机通信连接状态 
 * FALSE 表示连接已断开，TRUE 表示连接正常
 */
PUBLIC BOOL getObjCommLinkSts(tEnmCommObj obj)
{
    if(obj >= (COMMOBJ_TOTAL + COMMOBJ_FIRST))
        return FALSE;
    else
        return (BOOL)(SlaveInfo[obj - COMMOBJ_FIRST].slaveLinkOK);
}

/* =============================================================================
 * 初始化
 *  */
PUBLIC void resetSlaveInfo(void)
{
    int i;
    for(i=0; i<COMMOBJ_TOTAL; i++)
    {
        memset((int8u*)&SlaveInfo[i] , 0x00 ,sizeof(tSlaveInfo));
        memset((int8u*)(&SlaveInfo[i].slaveInput)   , 0xff ,sizeof(SlaveInfo[i].slaveInput));
        memset((int8u*)(&SlaveInfo[i].slaveOutput)  , 0xff ,sizeof(SlaveInfo[i].slaveOutput));
    }
    
    memset((int8u *) & MON_input_GPIO, 0xff, sizeof(MON_input_GPIO));
    memset((int8u *) & MON_input_data, 0xff, sizeof(MON_input_data));
    memset((int8u *) & SWCH_input_data, 0xff, sizeof(SWCH_input_data));
    memset((int8u *) & LOG1_input_data, 0xff, sizeof(LOG1_input_data));
    memset((int8u *) & LOG2_input_data, 0xff, sizeof(LOG2_input_data));
        
    sf_flash_toMMS = 0;             //安全回路触点闪断次数&安全回路电压闪断次数
    sf_flash_AccRcv = 0;            //
    UP_PEDC.byte = 0;               //安全继电器错误
}


/* =============================================================================
 * 读 PEDC 拨码ID
 *  */
PUBLIC void scanPEDC_ID(void)
{
	int8u Temp;
	
	Temp = 0;
	if(READ_ID_ADR3() == 0 )
		Temp |= 1<<3;
	if(READ_ID_ADR2() == 0 )
		Temp |= 1<<2;
	if(READ_ID_ADR1() == 0 )
		Temp |= 1<<1;
	if(READ_ID_ADR0() == 0 )
		Temp |= 1<<0;
    
	PEDC_ID = Temp;
}

/* 安全回路闪断次数累加：将最新的闪断次数累加计入
 * 注意高半字节和低半字节分别累加
 * Var : 待累加目标
 * add ：新累加值
 *  */
PUBLIC void accSFFlashTimes(int8u *Var, int8u add)
{
    /* 累加， */
    int8u tmpH, tmpL;
    tmpH = (*Var >> 4) & 0x0f;
    tmpL = (*Var >> 0) & 0x0f;

    tmpH += (add >> 4) & 0x0f;
    tmpL += (add >> 0) & 0x0f;

    if(tmpH > 0x0f) tmpH = 0x0f;
    if(tmpL > 0x0f) tmpL = 0x0f;

    *Var = ((tmpH << 4) + tmpL);
    
    return;
}

/* =============================================================================
 * 整合4块板的输入数据 : Monitor,(PIC24A or B), LOGA, LOGB
 * 根据在线情况，确定各个输入原始数据
 * -> GetINPUT_DATA
 * */
PRIVATE void getPCBInputData(void)
{
    BOOL slvOnline;
    tSlaveInfo * pSlvInfo;
    
    /* ====================
     * PIC24A 或 PIC24B 的输入 IO 信息 
     */
    if(ifMainWorkPic24A() == TRUE)
    {
        slvOnline = getObjCommLinkSts(COMMOBJ_IORW_A);
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_IORW_A);
    }
    else
    {
        slvOnline = getObjCommLinkSts(COMMOBJ_IORW_B);
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_IORW_B);
    }
    
    if(slvOnline == TRUE)
    {
        /* 数据有效计数器为0，表示接收的数据开始生效 */
        if(pSlvInfo->dataValidCnt == 0)
        {
            memcpy((int8u *) &MON_input_data, (int8u *)pSlvInfo->slaveInput.rdBuf, sizeof(MON_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        //clear Monitor board input buffer
        memset((int8u *) & MON_input_data, 0xff, sizeof(MON_input_data));
        #endif
    }
    
    /* ====================
     * 安全回路切换板 的输入 IO 信息 
     */
    if(getObjCommLinkSts(COMMOBJ_SWITCHER) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_SWITCHER);
        
        /* 数据有效计数器为0，表示接收的数据开始生效 */
        if(pSlvInfo->dataValidCnt == 0)
        {
            /* 
             * 这里需要注意与 上传信息相对应
             * 12 : DCU安全回路闪断次数
             * 13 : PDS安全回路闪断次数
             * 14 : 状态标志 (tSlaveStsFlags)
             */
            int8u sf_flash_recved;                     /* 单次收到的原始的安全回路闪断次数 */
            sf_flash_recved = pSlvInfo->slaveInput.rdBuf[12];
            pSlvInfo->slaveInput.rdBuf[12] = 0;
            pSlvInfo->slaveInput.rdBuf[13] = 0;

            /* 发送给测试工具观察测试 */
            /* 高低半字节分别累加，不考虑溢出 */
            {
            int8u tmpH, tmpL;
            tmpH = (sf_flash_toTest >> 4) & 0x0f;
            tmpL = (sf_flash_toTest >> 0) & 0x0f;

            tmpH += (sf_flash_recved >> 4) & 0x0f;
            tmpL += (sf_flash_recved >> 0) & 0x0f;
            sf_flash_toTest = (tmpH << 4) + (tmpL & 0x0f);
            }

            /* 累加安全闪断次数 */
            accSFFlashTimes(&sf_flash_AccRcv, sf_flash_recved);


            tSlaveStsFlags stsFlags;
            stsFlags.byte = pSlvInfo->slaveInput.rdBuf[14];
            SF_FAULT = stsFlags.bits.dcuSFRelayErr;

            memcpy((int8u *) &SWCH_input_data, pSlvInfo->slaveInput.rdBuf, sizeof(SWCH_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        memset((int8u *) & SWCH_input_data, 0xff, sizeof(SWCH_input_data));
        
        /* 初始化检测常闭点 */
        ...
        ...
        #endif
                
        sf_flash_AccRcv = 0;        //安全回路触点闪断次数&安全回路电压闪断次数
        UP_PEDC.byte    = 0;        //安全继电器错误
    }
    
    /* ====================
     * LOG1 的输入 IO 信息 
     */
    if(getObjCommLinkSts(COMMOBJ_LOG1) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
        
        /* 数据有效计数器为0，表示接收的数据开始生效 */
        if(pSlvInfo->dataValidCnt == 0)
        {
            memcpy((int8u *) &LOG1_input_data, (int8u *)pSlvInfo->slaveInput.rdBuf, sizeof(LOG1_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        memset((int8u *) & LOG1_input_data, 0xff, sizeof(LOG1_input_data));
        
        /* 初始化检测常闭点 */
        #endif
    }
        
    /* ====================
     * LOG2 的输入 IO 信息 
     */
    if(getObjCommLinkSts(COMMOBJ_LOG2) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
        
        /* 数据有效计数器为0，表示接收的数据开始生效 */
        if(pSlvInfo->dataValidCnt == 0)
        {
            memcpy((int8u *) &LOG2_input_data, (int8u *)pSlvInfo->slaveInput.rdBuf, sizeof(LOG2_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        memset((int8u *) & LOG2_input_data, 0xff, sizeof(LOG2_input_data));
        /* 初始化检测常闭点 */
        #endif  
    }
}

/* =============================================================================
 * 读取设备信息 
 *  obj     : 对象
 *  pInfo   : 存储位置
 *  infoLen : 返回信息总长度（占用字节数，不一定全部有效）
 * 返回
 *  TRUE    : 成功
 *  FALSE   : 失败
 *  */
PUBLIC BOOL readSlaveVerInfo(tEnmCommObj obj, char * pInfo, int * infoLen)
{
    BOOL ret;
    
    if(getSlaveInfoPtr(obj)->VersionFlag == 1)
    {
        memcpy(pInfo, getSlaveInfoPtr(obj)->slaveDevInfo, BYTES_INFO);
        ret = TRUE;
    }
    else
    {
        *pInfo++= '?';
        *pInfo++= '?';
        *pInfo++= '?';
        *pInfo++= 0;
        
        ret = FALSE;
    }
    
    *infoLen = BYTES_INFO;
    
    return ret;
}

/* =========================================================
 * 扫描输入 MCU-PIC32 GPIO 
 */
PRIVATE void sacnMcuGPIOInput(void)   
{   
    int i;
    int8u *pPcbIoData;

    //RB~RG，共6个 GPIO，12字节
    int16u PortMsk[MCU_GPIO_NUM];
    int16u gpioDataTmp[MCU_GPIO_NUM];    
    int16u gpioData0[MCU_GPIO_NUM];
    int16u gpioData1[MCU_GPIO_NUM];    
    
    pPcbIoData = (int8u*)MON_input_GPIO;

    /* GPIO MASK 位（1表示输入） */
    memcpy((int8u*)PortMsk, (int8u*)getPic32GPIOsCfg(), sizeof(PortMsk));

    gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
    gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
    gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
    gpioDataTmp[3] = mPORTERead() & PortMsk[3];
    gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
    gpioDataTmp[5] = mPORTGRead() & PortMsk[5];

    /* 缓存每次读入的数据 */
    memcpy((int8u*)readPortBuf[readInputCnt], gpioDataTmp, sizeof(gpioDataTmp));
    readInputCnt++;

    /* 是否达到去抖动的次数N ?
     * 若N次的状态都相同，则送入扫描结果
     * 若N次的状态在变化，则结果不受影响
     *  */
    if(readInputCnt >= IO_SCAN_DBNC_NUM)
    {
        memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
        memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
        for(i=0; i<IO_SCAN_DBNC_NUM; i++)
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

        if(firstScanInput == 0)
        {
            /* 第一次达到扫描次数，重置 */
            firstScanInput = 1;
            memset((int8u*)gpioDataTmp, 0xff, sizeof(gpioDataTmp));
        }
        else
        {
            /* 复制以前的结果 */
            /* 复制以前的结果（同时将 8位转 16位）
            */
           memcpy((int8u*)gpioDataTmp, (int8u*)pPcbIoData, sizeof(gpioDataTmp));
        }

        for(i=0; i<MCU_GPIO_NUM; i++)
        {
            /* 将稳定的低电平送入结果 */
            gpioDataTmp[i] &= gpioData0[i];

            /* 将稳定的高电平送入结果 */
            gpioDataTmp[i] |= gpioData1[i];
        }

        /* 保存本次的结果 */
        memcpy((int8u*)pPcbIoData, (int8u*)gpioDataTmp, sizeof(gpioDataTmp));

        /* 移动扫描缓存 
         * 这样处理的方式，比起完全重新扫描N次，对 GPIO 的变化反应比较快
         */
        for(i=0; i<IO_SCAN_DBNC_NUM - 1; i++)
        {
            readPortBuf[i][0] = readPortBuf[i + 1][0];
            readPortBuf[i][1] = readPortBuf[i + 1][1];
            readPortBuf[i][2] = readPortBuf[i + 1][2];
            readPortBuf[i][3] = readPortBuf[i + 1][3];
            readPortBuf[i][4] = readPortBuf[i + 1][4];
            readPortBuf[i][5] = readPortBuf[i + 1][5];
        }
        
        readInputCnt = IO_SCAN_DBNC_NUM - 1;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][0] = 0;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][1] = 0;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][2] = 0;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][3] = 0;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][4] = 0;
        readPortBuf[IO_SCAN_DBNC_NUM - 1][5] = 0;

        /* 扫描得到结果 */
    }
    else
    {
        /* 扫描没有结果 */
        /* 清除 */
        memset((int8u*)pPcbIoData, 0xff, PIC32_IODATA_SIZE);
    }
}

/* =============================================================================
 * 主程序定时汇总读入的IO输入数据，包括 MONITOR IBP PSL SIG PIC24A PIC24B 
 * 处理逻辑判断
 *  */
PUBLIC void mainIOProcess(void)
{    
    static tSYSTICK IOProcTime;
    static tSYSTICK prevScanTick;
    tSYSTICK tickNow;
    BOOL updateIO = FALSE;

    tickNow = getSysTick();
    
    /* 判断扫描 GPIO 输入口时间间隔 */
    if(getSysTimeDiff(prevScanTick) >=  IO_SCAN_INTERVAL)
    {
        sacnMcuGPIOInput();
    }
      
    
    /* 判断：若收到PIC24的 IO数据，则更新
     *       若收到IBP PSL SIG ... 的 IO数据，则更新
     *  */
    if((getSlaveInfoPtr(COMMOBJ_IORW_A)->gotIOData      == 1)    ||
       (getSlaveInfoPtr(COMMOBJ_IORW_B)->gotIOData      == 1)    ||
       (getSlaveInfoPtr(COMMOBJ_SWITCHER)->gotIOData    == 1)    ||
       (getSlaveInfoPtr(COMMOBJ_LOG1)->gotIOData        == 1)    ||
       (getSlaveInfoPtr(COMMOBJ_LOG2)->gotIOData        == 1) 
      )
    {
        updateIO = TRUE;
        
        getSlaveInfoPtr(COMMOBJ_IORW_A)->gotIOData      = 0;
        getSlaveInfoPtr(COMMOBJ_IORW_B)->gotIOData      = 0;
        getSlaveInfoPtr(COMMOBJ_SWITCHER)->gotIOData    = 0;
        getSlaveInfoPtr(COMMOBJ_LOG1)->gotIOData        = 0;
        getSlaveInfoPtr(COMMOBJ_LOG2)->gotIOData        = 0;
    }
    
    #if 1
    /* 这里处理的目的是强制定时更新
     */
    if(updateIO == FALSE)
    {        
        if((tSYSTICK)(tickNow - IOProcTime) >= 50)
        {
            updateIO   = TRUE;
        }
    }
    #endif
    
    /* 更新 IO 状态、
     * 处理相关逻辑 
     * !!!!! WTJ : Uart_Pic24_TX_Handle !!!!!
     */
    if(updateIO == TRUE)
    {
        IOProcTime = tickNow;
            
        getPCBInputData();              /* 读取各个PCB的输入信息 */
        AlarmParaTxToAvr();             /* 收集 DCU 状态 */
        PCA9505_data_deal();            /* 处理IO数据，检查PEDC的各种内部错误 */

        /* 管理 LOG1 和 LOG2  */
        manageLOG1LOG2();
        
        /* LOG PCB的状态发送给 MMS 
         * 必须在 manageLOG1LOG2() 函数之后执行
         *  */
        PEDC_LOG_Status.byte = getLOGPcbState();
        
        
        /* 得到站台门对位隔离列车门的信息
         * 保存在指定存储区，MMS 轮询时回复
         *  */
        int8u PSDInhibitTrain[8];
        getPSDInhibitTrain(PSDInhibitTrain, sizeof(PSDInhibitTrain));
        memcpy((int8u*)&uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[8], PSDInhibitTrain, sizeof(PSDInhibitTrain));
        
        /* !!!!! WTJ : Uart_Pic24_RX_Handle !!!!! */
        int i, j;
        if(Down_Message0.bMONITOR_FAULT || bClsAvrDataBuf)
        { //mms -- pedc 通讯失败
            for(j = 0; j < AVR_DATA_LEN; j ++)
            {
                uniDcuData.sortBuf.aAvrDataBuf[j] = 0;
            }
            bClsAvrDataBuf = 0;
        }
        
        /* 累加安全回路闪断次数
         *  */
        accSFFlashTimes(&sf_flash_toMMS, sf_flash_AccRcv);
        sf_flash_AccRcv = 0;

        /* 更新发送给MMS的信息 */
        PEDC_SF_FLASH_BUF.byte = sf_flash_toMMS;
        for(i = 0; i < AVR_DATA_LEN; i ++)
        {
            uniDcuData.sortBuf.aAvrDataBuf[i] |= MESG_BUF[i].byte;
        }
        
        /* PEDC-ID 拨码开关 */
        uniDcuData.sortBuf.aAvrDataBuf[AVR_DATA_LEN-1] = PEDC_ID;           
    }
}

/* =============================================================================
 * DCU 安全回路位
 * 每个位对应一个 DCU，bit: 0-没关门，  1-已关门
 */
PRIVATE int8u DOOR_CLSCLK[(MAX_DCU + 7) / 8];

/* 清除指定 DCU 安全回路位状态 */
void clr_CLSCLK(int8u dcu_no)
{
    int8u i;
    i = dcu_no - ID_DCU_MIN;
    if(i <= MAX_DCU)
    {
        ClrpBufBit((pINT8U)&DOOR_CLSCLK, i);
    }   
}

/* 设置指定 DCU 安全回路位状态 */
void set_CLSCLK(int8u dcu_no)
{
    int8u i;
    i = dcu_no - ID_DCU_MIN;
    if(i <= MAX_DCU)
    {
        SetpBufBit((pINT8U)&DOOR_CLSCLK, i);
    }   
}
