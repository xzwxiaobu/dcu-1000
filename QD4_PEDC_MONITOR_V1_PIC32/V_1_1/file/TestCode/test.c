#include "..\Include.h"
#include "uartDrv.h"
#include "MonCommCfg.h"

#include "IPSUart.h"
#include "IPS_App.h"
#include "Define.h"

#include "BASSys_Reg.h"
#include "BASSys_Uart.h"
#include "relayLife.h"

/* ���в��Թ��ܵı�־ 
 */
struct 
{
    int8u doTestFunct   : 1;
    int8u replyTestCmd  : 1;
    int8u               : 6;
}runTestFunFlags;

/* ָ���� DCU ��� 
 */
PRIVATE int8u state_dcu_No;

/* ���͵Ĳ��������У�����ָ����ŵ� DCU ��Ϣ
 * �˱��ͨ�� PC �˷��Ͳ��Դ�������ʱָ��
 * */
PUBLIC void setDoTestFunct(BOOL run)
    {
        if(run == TRUE)
        {
            runTestFunFlags.doTestFunct     = 1;
            runTestFunFlags.replyTestCmd    = 1;
        }
        else
        {
            runTestFunFlags.doTestFunct     = 0;
            runTestFunFlags.replyTestCmd    = 0;
        }
    }
PUBLIC void setDoTestFunctDCUNo(int8u DcuNo)
    {
        state_dcu_No = DcuNo;
    }

/* ���� MONTIOR ��� IO 
 */
PRIVATE  tSYSTICK   testPic24OutputMs;
PRIVATE  int        testOutDataLen;
PRIVATE  int8u      test_output_data[MAX_IODATA_SIZE];
tCmdWriteIO_SFSW    testSFSWOutput;

/* =============================================================================
 * */
extern BOOL ifMMSUartTxIsBusy(void);
extern int8u* getMMSTxBufPtr(void);
                                      
/* =============================================================================
 * ���Թ��ܣ�������ѭ������
 *  */
void testIO(void)
{
    static tSYSTICK lastTestTick;
    static tSYSTICK testInterval;
    
    int i, j;
    int8u *testTxPtr;

    tSYSTICK tickTmp;
    tSYSTICK tickNow;
    
    /* ��������ļ�ʱ�� 
     * ÿ������ 50ms ����һ��
     */
    
    tickNow = getSysTick();
    tickTmp = (tSYSTICK)(tickNow - lastTestTick);
    if(tickTmp < 50)
        return;
    
    lastTestTick = tickNow;
    
    /* ������������� IO ʱ�� */
    if(testPic24OutputMs >= tickTmp)
    {
        testPic24OutputMs -= tickTmp;
    }
    else
    {
        testPic24OutputMs = 0;
    }
    
    /* �Ƿ��ڲ���ģʽ 
     */
    if(runTestFunFlags.doTestFunct == 0)
        return;

    #if 0
    /* �յ�����������Ӧ��
     * ���Բ���Ҫ */
    if(runTestFunFlags.replyTestCmd)
    {
        int8u   test_success_msg[] = 
                {
                    0x10, 0x02, 0x00, 0x00, 0xC8, 0xDD, 0x00, 0x04, 0xAA, 0xBB, 0xCC, 0xDD, 0x10, 0x03, 0x10
                };
        runTestFunFlags.replyTestCmd = 0;
        INTEnable(INT_SOURCE_UART_TX(MMS_UART), INT_DISABLED);

        uartStartSend(COM_MMS, (int8u*)test_success_msg, sizeof(test_success_msg));
        return;
    }
    #endif


    /* ����ͨ��æ�����˳� */
    if(ifMMSUartTxIsBusy())
        return;
    
    /* ��ʱͨ�� MMS �Ĵ��ڷ��Ͳ������� */
    testInterval += tickTmp;
    if(testInterval < cTEST_CODE_SEND_TIME)
        return;
    testInterval -= cTEST_CODE_SEND_TIME;
    
    bClsAvrDataBuf = 1;

    //INTEnable(INT_SOURCE_UART_TX(MMS_UART), INT_DISABLED);

    testTxPtr = getMMSTxBufPtr();
    
    testTxPtr[UART_DLE_SEAT] = UARTDLE;
    testTxPtr[UART_STX_SEAT] = UARTSTX;
    testTxPtr[UART_SEQ_SEAT] = 0;
    testTxPtr[UARTSOURCEADD_SEAT] = 0;
    testTxPtr[UARTOBJ_SEAT] = 0;
    testTxPtr[UARTDATASORT_SEAT] = 0xAA;
    //testTxPtr[UARTLENL_SEAT] = 0;
    //testTxPtr[UARTLENH_SEAT] = ?;
    j = UARTDATASEAT;

    int8u* pSrc1;

    #if 1
    for(i = 0; i < DCUsetting_LEN; i ++)  //��FLASH ��ȡ�� ���ø�ֵ�� settingBUF
    {
        uniDcuData.sortBuf.aDcuCfgBUF[i] = uDCUsetting.settingBUF[i];
    }

    //
    //DCU ���ñ�־                  DCU_cfg_LEN               8
    //DCU ���߱�־ CAN1 & CAN2      DCU_CAN12_STATE_LEN       8 * 2
    //�̵�����Ϣ 12  + �ӿ�IO��Ϣ   AVR_DATA_LEN              20
    //��λ���� + MCP1 + MCP2        MCP_OPENCLOSE_DOOR_LEN    8 * 3
    //�̵�������                                              84 * 2
    pSrc1 = &uniDcuData.aDcuData[0];
    for(i = 0; i < AVR_LEN; i++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //ָ����ŵ�DCU״̬(һ��DCU)��ONE_DCU_STATE_LEN         24 
    for(i = 0; i < ONE_DCU_STATE_LEN; i ++)
    {
        testTxPtr[j ++] = uniDcuData.sortBuf.aDcuState[state_dcu_No][i];
    }

    //CAN���߹���  8 * 4 + 64 
    pSrc1 = (pINT8U)&CanManage;
    for(i = 0; i < sizeof(CanManage); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //��DCU״̬���������   64
    for(i = 0; i < MAX_DCU; i ++)
    {
        testTxPtr[j ++] = ReadDcuStateFailBuf[i];
    }
    #endif

    /* PEDC-MONITOR-SZ20-V1.0 */
    //Monitor ����PCA9505������Ϣ  MON_INPUT_DATA_LEN
    for(i = 0; i < MON_INPUT_DATA_LEN; i ++)
    {
        testTxPtr[j ++] = MON_input_data[i].byte;
    }
    /* 0-1 RBLH, 2-3 RC, 4-5 RD, 6-7 RE, 8-9 RF*/
    /* RBH : 12 13 */
    testTxPtr[j ++] = MON_input_GPIO[2*0 + 1].byte;      //MON_input_RB.byte1;
    /* REL : 2 3 4 */
    testTxPtr[j ++] = MON_input_GPIO[2*3 + 0].byte;      //MON_input_RE.byte0;
    
        
    //Monitor ����PCA9505�����Ϣ  MON_OUTPUT_DATA_LEN
    for(i = 0; i < OUTPUT_DATA_LEN; i ++)
    {
        testTxPtr[j ++] = MON_output_data[i].byte;
    }
    //testTxPtr[j ++] = MON_output_RB.byte0; //RBL : 2 3 5

    //�л���
    pSrc1 = (pINT8U)&SWCH_input_data;
    for(i = 0; i < sizeof(SWCH_input_data); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //�߼���1
    pSrc1 = (pINT8U)&LOG1_input_data;
    for(i = 0; i < sizeof(LOG1_input_data) ; i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //�߼���2
    pSrc1 = (pINT8U)&LOG2_input_data;
    for(i = 0; i < sizeof(LOG2_input_data); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }
    
    //PIC24A/PIC24B/IBP/PSL/SIG ����״̬  5
    //testTxPtr[j++] = 0;IbpPslSIG_ONLINE_BUF[0];  
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_IORW_A) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_IORW_B) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_SWITCHER) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_LOG1) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_LOG2) == TRUE) ? 1: 0;
    
    //������LOG��
    testTxPtr[j ++] = getWorkLOGID();

    //��ȫ��·���ϴ��� 
    testTxPtr[j ++] = sf_flash_toTest;
    testTxPtr[j ++] = sf_flash_AccRcv;
    testTxPtr[j ++] = sf_flash_toMMS;
    

    //PIC24 ѡ��   1
    testTxPtr[j ++] = (ifMainWorkPic24A()) ? 1 : 0;

    //PIC24A �����־ 1
    testTxPtr[j ++] = 0;//PIC18A_TX_RX_FLAG.byte;

    //PIC24B �����־ 1
    testTxPtr[j ++] = 0;//PIC18B_TX_RX_FLAG.byte;

    //PCA9505 ���������
    testTxPtr[j ++] = getPCA9505ErrCnt(COMMOBJ_IORW_A);
    testTxPtr[j ++] = getPCA9505ErrCnt(COMMOBJ_IORW_B);

    //����汾��Ϣ  (128 * 5)
    int8u *pInfo;
    int len;
    //PIC24A ����汾��Ϣ
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_IORW_A, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //PIC24B ����汾��Ϣ
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_IORW_B, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //�л��� ����汾��Ϣ
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_SWITCHER, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;

    //�߼���1 ����汾��Ϣ
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_LOG1, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //�߼���2 ����汾��Ϣ
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_LOG2, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //PEDC ����汾��Ϣ  128
    pSrc1 = (int8u *)strSwDescription;
    for(i=0; i<sizeof(strSwDescription); i++)
    {
        testTxPtr[j ++] = *pSrc1++;
    }

//    /* PCB_BUF 7 */
//    pSrc1 =(int8u *) &PCB_BUF;
//    for(i = 0; i < sizeof(PCB_BUF); i ++)
//    {
//        testTxPtr[j ++] = *pSrc1 ++ ;
//    }
//
//    /* MESSAGE_BUF 10*2 */
//    pSrc1 =(int8u *) &MESSAGE_BUF;
//    for(i = 0; i < sizeof(MESSAGE_BUF); i ++)
//    {
//        testTxPtr[j ++] = *pSrc1 ++ ;
//    }

    /* �߼��������й���״̬ 
     * �����߼���   - 1 
     * ������״̬   - 1
     * �����м����� - 4
     * ������״̬   - 1
     * �����м����� - 4
     */
    tSlaveInfo * pSlvInfo;
    testTxPtr[j ++] = bLOG_WorkLOG2;
    
    pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.testRunSts.data;
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[0];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[1];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[2];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[3];
    
    pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.testRunSts.data;
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[0];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[1];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[2];
    testTxPtr[j ++] = pSlvInfo->slaveInput.logIn.UB1_TestRunCnt[3];
    
    /* ��������߼�: �߼������������������
     * ������Ϣ             2Bytes
     * ����汾             1Byte
     * �������             4Byte
     * ����߼������       1 Byte
     * �ṹ�峤�� size=     1 Byte
     * ����ṹ������       (tOutputCmd) Bytes
     * �����߼�����������   8 Bytes
     * �����߼�������       1 Byte
     * һ�������߼��ṹ�峤�� bytes
     * ���п����߼���״̬����
     *  */
    int16u Len;
    int8u * logData = (int8u*)getLogicTestData((int16u*)&Len);
    
    testTxPtr[j ++] = Len ;
    testTxPtr[j ++] = Len >> 8;
    
    if(Len > 0)
    {
        if(logData != NULL)
        {
            for(i = 0; i < Len; i ++)
            {
                testTxPtr[j ++] = *logData ++ ;
            }
        }
    }
    
    #if 1
    /* BASϵͳ���� 1 */
    testTxPtr[j ++] = getBASSysOnlineSts();

    /* ��BASϵͳͨ�ŵ����� BAS-->PEDC 
     * 1 + 32 bytes
     */
    pInfo = (int8u *)getRxFromBASSys((int8u *)&len);
    testTxPtr[j ++] = len;
    for(i = 0; i < UART_BASSYS_RXBUFSIZE; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    
    /* ��BASϵͳͨ�ŵ����� PEDC-->BAS
     * 1 + 32 bytes ok
     */
    pInfo = (int8u *)getTxToBASSys((int8u *)&len);
    testTxPtr[j ++] = len;
    for(i = 0; i < UART_BASSYS_TXBUFSIZE; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    
    /* ����BASϵͳͨ�ŵļĴ��� */
    pInfo = (int8u*)getBASRegister();
    for(i = 0; i < sizeof(tBASRegister); i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    #endif

    
    #if 0
    /* �̵���������Ϣ
     * 1 �ֽڼ̵������� N
     * �߼��� 1 ��(8 * N)�ֽڵ�����
     * �߼��� 2 ��(8 * N)�ֽڵ�����
     *  */
    testTxPtr[j ++] = RELAY_NUM;
    pInfo = (int8u*)getRelayRecordPtr(COMMOBJ_LOG1) + sizeof(tRelayRecHeader);
    for(i = 0; i < sizeof(tRelayLife) * RELAY_NUM; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    
    pInfo = (int8u*)getRelayRecordPtr(COMMOBJ_LOG2) + sizeof(tRelayRecHeader);
    for(i = 0; i < sizeof(tRelayLife) * RELAY_NUM; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    #endif
    
    /* PEDC�������� */
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uIbpAlarmParameter.byte;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_On_Hour;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_On_Minute;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_Off_Hour;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_Off_Minute;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVTotalNum;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVNum_Opn;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVNum_Cls;
    
    /* PEDC ϵͳʱ��
     * ������������ڿ����жϽ������������Ƿ������� */
    tSysTime tmNow;
    getPedcSysTime(&tmNow);
    pSrc1 = (int8u*)(&tmNow);
    for(i = 0; i < sizeof(tSysTime); i ++)
    {
        testTxPtr[j ++] = *pSrc1 ++ ;
    }
    
    /* �緧�Ŀ�������
     * ǰ�淢�ˣ��������б�Ҫ�� */
    testTxPtr[j ++] = (int8u)(((tBASRegister*)getBASRegister())->reg10.wBASCmd     );
    testTxPtr[j ++] = (int8u)(((tBASRegister*)getBASRegister())->reg10.wBASCmd >> 8);
        
    /* ���ݽ��� */
    testTxPtr[UARTLENL_SEAT] = (j - UARTDATASEAT) & 0xff;
    testTxPtr[UARTLENH_SEAT] = ((j - UARTDATASEAT) >> 8) & 0xff;
    testTxPtr[j ++] = UARTDLE;
    testTxPtr[j ++] = UARTETX;
    testTxPtr[j] = checksum(&testTxPtr[0], (testTxPtr[UARTLENH_SEAT] << 8) + testTxPtr[UARTLENL_SEAT] + (UART2_TX_FILEMESSAGE_LEN - 1));

    int16u txDataLen = (testTxPtr[UARTLENH_SEAT] << 8) + testTxPtr[UARTLENL_SEAT] + UART2_TX_FILEMESSAGE_LEN;
    uartStartSend(COM_MMS, testTxPtr, txDataLen);
}

/* =============================================================================
 * 
 * ���� MONTIOR ��� IO
 * 
 * ========================================================================== */

/* ���� IO ������ʱ���Ƿ���Ч
 * ���� TRUE = ��
 *  */
PUBLIC BOOL chkIfTestOutputIO(void)
{
    return (testPic24OutputMs > 0);
}

/* ��ȡ���� IO �������ָ��
 *  */
PUBLIC int8u* getTestOutputIOData(void)
{
    return test_output_data;
}

/* ���ò��Ե��������
 *  data        ����
 *  len         ����
 *  time        ����ʱ�� ms
 *  */
PUBLIC int setPic24TestOutputData(int8u *data, int len, int time)
{
    if(data != NULL)
    {
        if(len > sizeof(test_output_data))
        {
            len = sizeof(test_output_data);
        }

        memcpy(test_output_data, data, len);
        
        #if 0
        tSFSWOutput SFOut;
        SFOut.byte = test_output_data[MON_Output_SFSW_OFFSET];
        testSFSWOutput.IOOut.Q1Switch   = SFOut.SFSW_Q0_Q1Swtich;
        testSFSWOutput.IOOut.Q2Switch   = SFOut.SFSW_Q1_Q2Swtich;
        testSFSWOutput.IOOut.RESET      = SFOut.SFSW_Q1_Q2Swtich;
        testSFSWOutput.IOOut.PN0        = SFOut.SFSW_Q3_PN0;
        testSFSWOutput.IOOut.PN0_En     = 1;                        /* Ŀǰֻʹ����� Q3 */
        #endif
    }
    
    testPic24OutputMs = time;
    
    return len;
}

