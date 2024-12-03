#include "Include.h"

#include "mainIO.h"
#include "PCBIOStruct.h"
#include "commdef.h"

#undef  PRIVATE
#define PRIVATE

/* ����оƬ�����������Ƿ�����
 * 0      = ά�־�����
 * ��Ϊ0  = ����
 *  */
#define RESET_INPUT_OFFLINE     0


#define MCU_GPIO_NUM            6       /* GPIO : B C D E F G */
#define IO_SCAN_DBNC_NUM        5       /* ɨ��IO��ȥ��������   */
#define IO_SCAN_INTERVAL        8       /* ɨ��ʱ����, ms  */
PRIVATE int16u readPortBuf[IO_SCAN_DBNC_NUM][MCU_GPIO_NUM];     /* �� MCU GPIO */
PRIVATE int8u  readInputCnt;
PRIVATE int8u  firstScanInput;

/* =============================================================================*/
/* PEDC ���� */
PRIVATE int8u PEDC_ID;
PUBLIC int8u getPedcID(void)
{
    return PEDC_ID;
}

PRIVATE int8u sig1PcbID,sig1PcbID_Prev;
PRIVATE int8u sig2PcbID,sig2PcbID_Prev;

/* =============================================================================*/
/* �ӻ������Ϣ�洢
 * ���� PIC24A PIC24B LOGA LOGB
 * ������ MONITOR ʵ��û��
 *  */
PRIVATE tSlaveInfo SlaveInfo[COMMOBJ_TOTAL];

/* ȡ�ӻ������Ϣָ�룬���ⲿ��ȡʹ��
 */
PUBLIC tSlaveInfo* getSlaveInfoPtr(tEnmCommObj obj)
{
    if(obj >= (COMMOBJ_TOTAL + COMMOBJ_FIRST) )
        return NULL;
    else
        return &(SlaveInfo[obj - COMMOBJ_FIRST]);
}

/* ��ȡ�ӻ�ͨ������״̬ 
 * FALSE ��ʾ�����ѶϿ���TRUE ��ʾ��������
 */
PUBLIC BOOL getObjCommLinkSts(tEnmCommObj obj)
{
    if(obj >= (COMMOBJ_TOTAL + COMMOBJ_FIRST))
        return FALSE;
    else
        return (BOOL)(SlaveInfo[obj - COMMOBJ_FIRST].slaveLinkOK);
}

/* =============================================================================
 * ��ʼ��
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
        
    sf_flash_toMMS = 0;             //��ȫ��·�������ϴ���&��ȫ��·��ѹ���ϴ���
    sf_flash_AccRcv = 0;            //
    UP_PEDC.byte = 0;               //��ȫ�̵�������
}


/* =============================================================================
 * �� PEDC ����ID
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

/* ��ȫ��·���ϴ����ۼӣ������µ����ϴ����ۼӼ���
 * ע��߰��ֽں͵Ͱ��ֽڷֱ��ۼ�
 * Var : ���ۼ�Ŀ��
 * add �����ۼ�ֵ
 *  */
PUBLIC void accSFFlashTimes(int8u *Var, int8u add)
{
    /* �ۼӣ� */
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
 * ����4������������ : Monitor,(PIC24A or B), LOGA, LOGB
 * �������������ȷ����������ԭʼ����
 * -> GetINPUT_DATA
 * */
PRIVATE void getPCBInputData(void)
{
    BOOL slvOnline;
    tSlaveInfo * pSlvInfo;
    
    /* ====================
     * PIC24A �� PIC24B ������ IO ��Ϣ 
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
        /* ������Ч������Ϊ0����ʾ���յ����ݿ�ʼ��Ч */
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
     * ��ȫ��·�л��� ������ IO ��Ϣ 
     */
    if(getObjCommLinkSts(COMMOBJ_SWITCHER) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_SWITCHER);
        
        /* ������Ч������Ϊ0����ʾ���յ����ݿ�ʼ��Ч */
        if(pSlvInfo->dataValidCnt == 0)
        {
            /* 
             * ������Ҫע���� �ϴ���Ϣ���Ӧ
             * 12 : DCU��ȫ��·���ϴ���
             * 13 : PDS��ȫ��·���ϴ���
             * 14 : ״̬��־ (tSlaveStsFlags)
             */
            int8u sf_flash_recved;                     /* �����յ���ԭʼ�İ�ȫ��·���ϴ��� */
            sf_flash_recved = pSlvInfo->slaveInput.rdBuf[12];
            pSlvInfo->slaveInput.rdBuf[12] = 0;
            pSlvInfo->slaveInput.rdBuf[13] = 0;

            /* ���͸����Թ��߹۲���� */
            /* �ߵͰ��ֽڷֱ��ۼӣ���������� */
            {
            int8u tmpH, tmpL;
            tmpH = (sf_flash_toTest >> 4) & 0x0f;
            tmpL = (sf_flash_toTest >> 0) & 0x0f;

            tmpH += (sf_flash_recved >> 4) & 0x0f;
            tmpL += (sf_flash_recved >> 0) & 0x0f;
            sf_flash_toTest = (tmpH << 4) + (tmpL & 0x0f);
            }

            /* �ۼӰ�ȫ���ϴ��� */
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
        
        /* ��ʼ����ⳣ�յ� */
        ...
        ...
        #endif
                
        sf_flash_AccRcv = 0;        //��ȫ��·�������ϴ���&��ȫ��·��ѹ���ϴ���
        UP_PEDC.byte    = 0;        //��ȫ�̵�������
    }
    
    /* ====================
     * LOG1 ������ IO ��Ϣ 
     */
    if(getObjCommLinkSts(COMMOBJ_LOG1) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
        
        /* ������Ч������Ϊ0����ʾ���յ����ݿ�ʼ��Ч */
        if(pSlvInfo->dataValidCnt == 0)
        {
            memcpy((int8u *) &LOG1_input_data, (int8u *)pSlvInfo->slaveInput.rdBuf, sizeof(LOG1_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        memset((int8u *) & LOG1_input_data, 0xff, sizeof(LOG1_input_data));
        
        /* ��ʼ����ⳣ�յ� */
        #endif
    }
        
    /* ====================
     * LOG2 ������ IO ��Ϣ 
     */
    if(getObjCommLinkSts(COMMOBJ_LOG2) == TRUE)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
        
        /* ������Ч������Ϊ0����ʾ���յ����ݿ�ʼ��Ч */
        if(pSlvInfo->dataValidCnt == 0)
        {
            memcpy((int8u *) &LOG2_input_data, (int8u *)pSlvInfo->slaveInput.rdBuf, sizeof(LOG2_input_data));
        }
    }
    else
    {
        #if (RESET_INPUT_OFFLINE != 0)
        memset((int8u *) & LOG2_input_data, 0xff, sizeof(LOG2_input_data));
        /* ��ʼ����ⳣ�յ� */
        #endif  
    }
}

/* =============================================================================
 * ��ȡ�豸��Ϣ 
 *  obj     : ����
 *  pInfo   : �洢λ��
 *  infoLen : ������Ϣ�ܳ��ȣ�ռ���ֽ�������һ��ȫ����Ч��
 * ����
 *  TRUE    : �ɹ�
 *  FALSE   : ʧ��
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
 * ɨ������ MCU-PIC32 GPIO 
 */
PRIVATE void sacnMcuGPIOInput(void)   
{   
    int i;
    int8u *pPcbIoData;

    //RB~RG����6�� GPIO��12�ֽ�
    int16u PortMsk[MCU_GPIO_NUM];
    int16u gpioDataTmp[MCU_GPIO_NUM];    
    int16u gpioData0[MCU_GPIO_NUM];
    int16u gpioData1[MCU_GPIO_NUM];    
    
    pPcbIoData = (int8u*)MON_input_GPIO;

    /* GPIO MASK λ��1��ʾ���룩 */
    memcpy((int8u*)PortMsk, (int8u*)getPic32GPIOsCfg(), sizeof(PortMsk));

    gpioDataTmp[0] = mPORTBRead() & PortMsk[0];
    gpioDataTmp[1] = mPORTCRead() & PortMsk[1];
    gpioDataTmp[2] = mPORTDRead() & PortMsk[2];
    gpioDataTmp[3] = mPORTERead() & PortMsk[3];
    gpioDataTmp[4] = mPORTFRead() & PortMsk[4];
    gpioDataTmp[5] = mPORTGRead() & PortMsk[5];

    /* ����ÿ�ζ�������� */
    memcpy((int8u*)readPortBuf[readInputCnt], gpioDataTmp, sizeof(gpioDataTmp));
    readInputCnt++;

    /* �Ƿ�ﵽȥ�����Ĵ���N ?
     * ��N�ε�״̬����ͬ��������ɨ����
     * ��N�ε�״̬�ڱ仯����������Ӱ��
     *  */
    if(readInputCnt >= IO_SCAN_DBNC_NUM)
    {
        memset((int8u*)gpioData0, 0x00, sizeof(gpioData0));
        memset((int8u*)gpioData1, 0xff, sizeof(gpioData1));
        for(i=0; i<IO_SCAN_DBNC_NUM; i++)
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

        if(firstScanInput == 0)
        {
            /* ��һ�δﵽɨ����������� */
            firstScanInput = 1;
            memset((int8u*)gpioDataTmp, 0xff, sizeof(gpioDataTmp));
        }
        else
        {
            /* ������ǰ�Ľ�� */
            /* ������ǰ�Ľ����ͬʱ�� 8λת 16λ��
            */
           memcpy((int8u*)gpioDataTmp, (int8u*)pPcbIoData, sizeof(gpioDataTmp));
        }

        for(i=0; i<MCU_GPIO_NUM; i++)
        {
            /* ���ȶ��ĵ͵�ƽ������ */
            gpioDataTmp[i] &= gpioData0[i];

            /* ���ȶ��ĸߵ�ƽ������ */
            gpioDataTmp[i] |= gpioData1[i];
        }

        /* ���汾�εĽ�� */
        memcpy((int8u*)pPcbIoData, (int8u*)gpioDataTmp, sizeof(gpioDataTmp));

        /* �ƶ�ɨ�軺�� 
         * ��������ķ�ʽ��������ȫ����ɨ��N�Σ��� GPIO �ı仯��Ӧ�ȽϿ�
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

        /* ɨ��õ���� */
    }
    else
    {
        /* ɨ��û�н�� */
        /* ��� */
        memset((int8u*)pPcbIoData, 0xff, PIC32_IODATA_SIZE);
    }
}

/* =============================================================================
 * ������ʱ���ܶ����IO�������ݣ����� MONITOR IBP PSL SIG PIC24A PIC24B 
 * �����߼��ж�
 *  */
PUBLIC void mainIOProcess(void)
{    
    static tSYSTICK IOProcTime;
    static tSYSTICK prevScanTick;
    tSYSTICK tickNow;
    BOOL updateIO = FALSE;

    tickNow = getSysTick();
    
    /* �ж�ɨ�� GPIO �����ʱ���� */
    if(getSysTimeDiff(prevScanTick) >=  IO_SCAN_INTERVAL)
    {
        sacnMcuGPIOInput();
    }
      
    
    /* �жϣ����յ�PIC24�� IO���ݣ������
     *       ���յ�IBP PSL SIG ... �� IO���ݣ������
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
    /* ���ﴦ���Ŀ����ǿ�ƶ�ʱ����
     */
    if(updateIO == FALSE)
    {        
        if((tSYSTICK)(tickNow - IOProcTime) >= 50)
        {
            updateIO   = TRUE;
        }
    }
    #endif
    
    /* ���� IO ״̬��
     * ��������߼� 
     * !!!!! WTJ : Uart_Pic24_TX_Handle !!!!!
     */
    if(updateIO == TRUE)
    {
        IOProcTime = tickNow;
            
        getPCBInputData();              /* ��ȡ����PCB��������Ϣ */
        AlarmParaTxToAvr();             /* �ռ� DCU ״̬ */
        PCA9505_data_deal();            /* ����IO���ݣ����PEDC�ĸ����ڲ����� */

        /* ���� LOG1 �� LOG2  */
        manageLOG1LOG2();
        
        /* LOG PCB��״̬���͸� MMS 
         * ������ manageLOG1LOG2() ����֮��ִ��
         *  */
        PEDC_LOG_Status.byte = getLOGPcbState();
        
        
        /* �õ�վ̨�Ŷ�λ�����г��ŵ���Ϣ
         * ������ָ���洢����MMS ��ѯʱ�ظ�
         *  */
        int8u PSDInhibitTrain[8];
        getPSDInhibitTrain(PSDInhibitTrain, sizeof(PSDInhibitTrain));
        memcpy((int8u*)&uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[8], PSDInhibitTrain, sizeof(PSDInhibitTrain));
        
        /* !!!!! WTJ : Uart_Pic24_RX_Handle !!!!! */
        int i, j;
        if(Down_Message0.bMONITOR_FAULT || bClsAvrDataBuf)
        { //mms -- pedc ͨѶʧ��
            for(j = 0; j < AVR_DATA_LEN; j ++)
            {
                uniDcuData.sortBuf.aAvrDataBuf[j] = 0;
            }
            bClsAvrDataBuf = 0;
        }
        
        /* �ۼӰ�ȫ��·���ϴ���
         *  */
        accSFFlashTimes(&sf_flash_toMMS, sf_flash_AccRcv);
        sf_flash_AccRcv = 0;

        /* ���·��͸�MMS����Ϣ */
        PEDC_SF_FLASH_BUF.byte = sf_flash_toMMS;
        for(i = 0; i < AVR_DATA_LEN; i ++)
        {
            uniDcuData.sortBuf.aAvrDataBuf[i] |= MESG_BUF[i].byte;
        }
        
        /* PEDC-ID ���뿪�� */
        uniDcuData.sortBuf.aAvrDataBuf[AVR_DATA_LEN-1] = PEDC_ID;           
    }
}

/* =============================================================================
 * DCU ��ȫ��·λ
 * ÿ��λ��Ӧһ�� DCU��bit: 0-û���ţ�  1-�ѹ���
 */
PRIVATE int8u DOOR_CLSCLK[(MAX_DCU + 7) / 8];

/* ���ָ�� DCU ��ȫ��·λ״̬ */
void clr_CLSCLK(int8u dcu_no)
{
    int8u i;
    i = dcu_no - ID_DCU_MIN;
    if(i <= MAX_DCU)
    {
        ClrpBufBit((pINT8U)&DOOR_CLSCLK, i);
    }   
}

/* ����ָ�� DCU ��ȫ��·λ״̬ */
void set_CLSCLK(int8u dcu_no)
{
    int8u i;
    i = dcu_no - ID_DCU_MIN;
    if(i <= MAX_DCU)
    {
        SetpBufBit((pINT8U)&DOOR_CLSCLK, i);
    }   
}
