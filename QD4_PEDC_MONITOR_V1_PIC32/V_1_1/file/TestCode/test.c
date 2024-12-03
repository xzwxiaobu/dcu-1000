#include "..\Include.h"
#include "uartDrv.h"
#include "MonCommCfg.h"

#include "IPSUart.h"
#include "IPS_App.h"
#include "Define.h"

#include "BASSys_Reg.h"
#include "BASSys_Uart.h"
#include "relayLife.h"

/* 运行测试功能的标志 
 */
struct 
{
    int8u doTestFunct   : 1;
    int8u replyTestCmd  : 1;
    int8u               : 6;
}runTestFunFlags;

/* 指定的 DCU 编号 
 */
PRIVATE int8u state_dcu_No;

/* 发送的测试数据中，包含指定编号的 DCU 信息
 * 此编号通过 PC 端发送测试代码命令时指定
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

/* 测试 MONTIOR 输出 IO 
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
 * 测试功能，主程序循环调用
 *  */
void testIO(void)
{
    static tSYSTICK lastTestTick;
    static tSYSTICK testInterval;
    
    int i, j;
    int8u *testTxPtr;

    tSYSTICK tickTmp;
    tSYSTICK tickNow;
    
    /* 测试输出的计时器 
     * 每隔至少 50ms 处理一次
     */
    
    tickNow = getSysTick();
    tickTmp = (tSYSTICK)(tickNow - lastTestTick);
    if(tickTmp < 50)
        return;
    
    lastTestTick = tickNow;
    
    /* 计算测试输出点的 IO 时间 */
    if(testPic24OutputMs >= tickTmp)
    {
        testPic24OutputMs -= tickTmp;
    }
    else
    {
        testPic24OutputMs = 0;
    }
    
    /* 是否处于测试模式 
     */
    if(runTestFunFlags.doTestFunct == 0)
        return;

    #if 0
    /* 收到测试命令后的应答
     * 可以不需要 */
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


    /* 串口通信忙，则退出 */
    if(ifMMSUartTxIsBusy())
        return;
    
    /* 定时通过 MMS 的串口发送测试数据 */
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
    for(i = 0; i < DCUsetting_LEN; i ++)  //将FLASH 读取的 设置赋值给 settingBUF
    {
        uniDcuData.sortBuf.aDcuCfgBUF[i] = uDCUsetting.settingBUF[i];
    }

    //
    //DCU 配置标志                  DCU_cfg_LEN               8
    //DCU 在线标志 CAN1 & CAN2      DCU_CAN12_STATE_LEN       8 * 2
    //继电器信息 12  + 接口IO信息   AVR_DATA_LEN              20
    //对位隔离 + MCP1 + MCP2        MCP_OPENCLOSE_DOOR_LEN    8 * 3
    //继电器寿命                                              84 * 2
    pSrc1 = &uniDcuData.aDcuData[0];
    for(i = 0; i < AVR_LEN; i++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //指定编号的DCU状态(一个DCU)：ONE_DCU_STATE_LEN         24 
    for(i = 0; i < ONE_DCU_STATE_LEN; i ++)
    {
        testTxPtr[j ++] = uniDcuData.sortBuf.aDcuState[state_dcu_No][i];
    }

    //CAN总线管理  8 * 4 + 64 
    pSrc1 = (pINT8U)&CanManage;
    for(i = 0; i < sizeof(CanManage); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //读DCU状态错误计数器   64
    for(i = 0; i < MAX_DCU; i ++)
    {
        testTxPtr[j ++] = ReadDcuStateFailBuf[i];
    }
    #endif

    /* PEDC-MONITOR-SZ20-V1.0 */
    //Monitor 板载PCA9505输入信息  MON_INPUT_DATA_LEN
    for(i = 0; i < MON_INPUT_DATA_LEN; i ++)
    {
        testTxPtr[j ++] = MON_input_data[i].byte;
    }
    /* 0-1 RBLH, 2-3 RC, 4-5 RD, 6-7 RE, 8-9 RF*/
    /* RBH : 12 13 */
    testTxPtr[j ++] = MON_input_GPIO[2*0 + 1].byte;      //MON_input_RB.byte1;
    /* REL : 2 3 4 */
    testTxPtr[j ++] = MON_input_GPIO[2*3 + 0].byte;      //MON_input_RE.byte0;
    
        
    //Monitor 板载PCA9505输出信息  MON_OUTPUT_DATA_LEN
    for(i = 0; i < OUTPUT_DATA_LEN; i ++)
    {
        testTxPtr[j ++] = MON_output_data[i].byte;
    }
    //testTxPtr[j ++] = MON_output_RB.byte0; //RBL : 2 3 5

    //切换板
    pSrc1 = (pINT8U)&SWCH_input_data;
    for(i = 0; i < sizeof(SWCH_input_data); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //逻辑板1
    pSrc1 = (pINT8U)&LOG1_input_data;
    for(i = 0; i < sizeof(LOG1_input_data) ; i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }

    //逻辑板2
    pSrc1 = (pINT8U)&LOG2_input_data;
    for(i = 0; i < sizeof(LOG2_input_data); i ++)
    {
        testTxPtr[j ++] = * pSrc1;
        pSrc1 ++;
    }
    
    //PIC24A/PIC24B/IBP/PSL/SIG 在线状态  5
    //testTxPtr[j++] = 0;IbpPslSIG_ONLINE_BUF[0];  
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_IORW_A) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_IORW_B) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_SWITCHER) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_LOG1) == TRUE) ? 1: 0;
    testTxPtr[j++] = (getObjCommLinkSts(COMMOBJ_LOG2) == TRUE) ? 1: 0;
    
    //工作的LOG板
    testTxPtr[j ++] = getWorkLOGID();

    //安全回路闪断次数 
    testTxPtr[j ++] = sf_flash_toTest;
    testTxPtr[j ++] = sf_flash_AccRcv;
    testTxPtr[j ++] = sf_flash_toMMS;
    

    //PIC24 选择   1
    testTxPtr[j ++] = (ifMainWorkPic24A()) ? 1 : 0;

    //PIC24A 错误标志 1
    testTxPtr[j ++] = 0;//PIC18A_TX_RX_FLAG.byte;

    //PIC24B 错误标志 1
    testTxPtr[j ++] = 0;//PIC18B_TX_RX_FLAG.byte;

    //PCA9505 错误计数器
    testTxPtr[j ++] = getPCA9505ErrCnt(COMMOBJ_IORW_A);
    testTxPtr[j ++] = getPCA9505ErrCnt(COMMOBJ_IORW_B);

    //软件版本信息  (128 * 5)
    int8u *pInfo;
    int len;
    //PIC24A 软件版本信息
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_IORW_A, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //PIC24B 软件版本信息
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_IORW_B, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //切换板 软件版本信息
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_SWITCHER, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;

    //逻辑板1 软件版本信息
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_LOG1, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //逻辑板2 软件版本信息
    pInfo = &testTxPtr[j+1];
    readSlaveVerInfo(COMMOBJ_LOG2, (char*)pInfo, &len);
    testTxPtr[j++] = len;
    j += len;
    
    //PEDC 软件版本信息  128
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

    /* 逻辑板自运行功能状态 
     * 工作逻辑板   - 1 
     * 自运行状态   - 1
     * 自运行计数器 - 4
     * 自运行状态   - 1
     * 自运行计数器 - 4
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
    
    /* 软件控制逻辑: 逻辑板的输入输出命令测试
     * 长度信息             2Bytes
     * 软件版本             1Byte
     * 软件日期             4Byte
     * 输出逻辑的序号       1 Byte
     * 结构体长度 size=     1 Byte
     * 输出结构体数据       (tOutputCmd) Bytes
     * 控制逻辑的配置数量   8 Bytes
     * 控制逻辑的总数       1 Byte
     * 一个控制逻辑结构体长度 bytes
     * 所有控制逻辑的状态数据
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
    /* BAS系统在线 1 */
    testTxPtr[j ++] = getBASSysOnlineSts();

    /* 与BAS系统通信的数据 BAS-->PEDC 
     * 1 + 32 bytes
     */
    pInfo = (int8u *)getRxFromBASSys((int8u *)&len);
    testTxPtr[j ++] = len;
    for(i = 0; i < UART_BASSYS_RXBUFSIZE; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    
    /* 与BAS系统通信的数据 PEDC-->BAS
     * 1 + 32 bytes ok
     */
    pInfo = (int8u *)getTxToBASSys((int8u *)&len);
    testTxPtr[j ++] = len;
    for(i = 0; i < UART_BASSYS_TXBUFSIZE; i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    
    /* 用于BAS系统通信的寄存器 */
    pInfo = (int8u*)getBASRegister();
    for(i = 0; i < sizeof(tBASRegister); i ++)
    {
        testTxPtr[j ++] =	pInfo[i];
    }
    #endif

    
    #if 0
    /* 继电器寿命信息
     * 1 字节继电器数量 N
     * 逻辑板 1 ：(8 * N)字节的数据
     * 逻辑板 2 ：(8 * N)字节的数据
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
    
    /* PEDC参数设置 */
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uIbpAlarmParameter.byte;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_On_Hour;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_On_Minute;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_Off_Hour;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_Off_Minute;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVTotalNum;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVNum_Opn;
    testTxPtr[j ++] = tagDownIbpAlarmParameter.uRead_WVNum_Cls;
    
    /* PEDC 系统时间
     * 放在最后有助于快速判断解析测试数据是否有问题 */
    tSysTime tmNow;
    getPedcSysTime(&tmNow);
    pSrc1 = (int8u*)(&tmNow);
    for(i = 0; i < sizeof(tSysTime); i ++)
    {
        testTxPtr[j ++] = *pSrc1 ++ ;
    }
    
    /* 风阀的控制命令
     * 前面发了，这样还有必要吗？ */
    testTxPtr[j ++] = (int8u)(((tBASRegister*)getBASRegister())->reg10.wBASCmd     );
    testTxPtr[j ++] = (int8u)(((tBASRegister*)getBASRegister())->reg10.wBASCmd >> 8);
        
    /* 数据结束 */
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
 * 测试 MONTIOR 输出 IO
 * 
 * ========================================================================== */

/* 测试 IO 输出点的时间是否有效
 * 返回 TRUE = 是
 *  */
PUBLIC BOOL chkIfTestOutputIO(void)
{
    return (testPic24OutputMs > 0);
}

/* 获取测试 IO 输出数据指针
 *  */
PUBLIC int8u* getTestOutputIOData(void)
{
    return test_output_data;
}

/* 设置测试的输出数据
 *  data        数据
 *  len         长度
 *  time        持续时间 ms
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
        testSFSWOutput.IOOut.PN0_En     = 1;                        /* 目前只使能输出 Q3 */
        #endif
    }
    
    testPic24OutputMs = time;
    
    return len;
}

