/*******************************************************************************
 * 串口通信 模块
 * -> 主程序调用串口初始化      initPic24Comm 
 * -> 主循环调用串口通信处理    processPic24Comm 
 * 
 * 
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "waveflash.h"

#include "PIC24_App.h"
#include "IPSUart.h"
#include "IPS_App.h"
#include "mainIO.h"

#include "P24UpdateDrv.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* 调试用 
 * 0 = 正常切换 A B
 * 1 = 固定芯片 A
 * 2 = 固定芯片 B
 *  */
#define DBG_FIX_CHIP    0       

/* =============================================================================
 * 本地变量
 */
PRIVATE tUartComm uartPic24;                              /* 串口通信控制 */
PRIVATE int8u RxPic24DataBuf[UART_COM_RXBUFSIZE];         /* 串口通信控制-接收缓存 */
PRIVATE int8u TxPic24DataBuf[UART_COM_TXBUFSIZE];         /* 串口通信控制-发送缓存 */
    PUBLIC tUartComm * getPic24UartCommPtr(void)
    {
        return &uartPic24;
    }
    
    /* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
    PUBLIC BOOL ifPic24UartTxIsBusy(void)
    {
        return (BOOL)(uartPic24.uartSts.isSending);
    }
    
/* 发送数据包控制量 */

/* 需要轮询的对象列表 
 * 主工作芯片轮询频次更高
 */
PRIVATE const int8u pollingPic24List_A[] = 
{
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_A,
    COMMOBJ_IORW_B,
};
PRIVATE const int8u pollingPic24List_B[] = 
{
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_B,
    COMMOBJ_IORW_A,
};

PRIVATE tPollingTask PollingPic24;

/* =============================================================================*/
/* 主工作芯片 FALSE = A, TRUE = B */
PRIVATE BOOL MainWorkChipAB; 
PRIVATE BOOL prevWorkChipAB;

/* 判断主工作芯片
 * 返回 TRUE 表示主工作芯片是 PIC24A
 *  */
PUBLIC BOOL ifMainWorkPic24A(void)      
{
    return (MainWorkChipAB == MAINWORK_PIC24A);   //(bPEDC_CNT_PIC18 == choicePIC18A) 
}

/* AB-CTR0 控制选择主工作芯片 PIC24A 或 B，主工作芯片负责读取 I2C 扩展IO
 * A0_B1 
 *   = MAINWORK_PIC24A 选择A，
 *   = MAINWORK_PIC24B 选择B
 * PEDC_MONITOR_V1 : AB-CTR0( PE0 )
 * 
 * PIC24 的软件：检测
 * 
 */
PRIVATE void selectWorkChipAB(BOOL A0_B1)
{
    tSlaveInfo * pSlave;
    
    mPORTESetPinsDigitalOut(BIT_0); 
    #if (DBG_FIX_CHIP == 1)
    {
        A0_B1 = MAINWORK_PIC24A;
    }
    #elif (DBG_FIX_CHIP == 2)
    {
        A0_B1 = MAINWORK_PIC24B;
    }
    #endif
    if(A0_B1 == MAINWORK_PIC24A)
    {
        mPORTEClearBits(BIT_0);

        MainWorkChipAB = MAINWORK_PIC24A;
    }
    else
    {
        mPORTESetBits(BIT_0);

        MainWorkChipAB = MAINWORK_PIC24B;
    }
    
    if(prevWorkChipAB != MainWorkChipAB)
    {
        prevWorkChipAB = MainWorkChipAB;
        
        if(MainWorkChipAB == MAINWORK_PIC24A)
        {
            pSlave = getSlaveInfoPtr(COMMOBJ_IORW_A);
        }
        else
        {
            pSlave = getSlaveInfoPtr(COMMOBJ_IORW_B);
        }
        
        /* 切换主芯片以后，需要丢弃前面几次的数据 */
        pSlave->dataValidCnt   = 3;  
    }
}

/* COMM-CTR IO 控制选择通信对象 PIC24A 或 B
 *  tEnmCommObj = 
 *      COMMOBJ_IORW_A 选择主 PIC24
 *      COMMOBJ_IORW_B 选择从 PIC24
 * PEDC_MONITOR_V1 : COMM-CTR (PE7)
 */
PRIVATE tEnmCommObj communicationObj;
PUBLIC void setCommunObjAB(tEnmCommObj obj)
{
    #if (DBG_FIX_CHIP == 1)
    {
        obj = COMMOBJ_IORW_A;
    }
    #elif (DBG_FIX_CHIP == 2)
    {
        obj = COMMOBJ_IORW_B;
    }
    #else
    #endif
    
    communicationObj = obj;
    
    mPORTESetPinsDigitalOut(BIT_7); 
    
    if(obj == COMMOBJ_IORW_A)
    {
        mPORTEClearBits(BIT_7);
    }
    else if(obj == COMMOBJ_IORW_B)
    {
        mPORTESetBits(BIT_7);
    }
}

/* =========================================================
 * 通信中断处理函数 : 与 IBP PSL SIG 板通信
 * 输入：
 *   module_id  : 串口ID
 *   data       : 一个字节数据
 */
PRIVATE void Pic24_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    /* 升级过程 或 正常轮询过程
     * 采用相同的协议接收数据帧
     *  */
    Uni_UartReceiveData(PollingPic24.pollingCom, data);
}

/* =============================================================================
 * 检查接收数据是否有 PCA9505 错误
 * 
 */
#define PCA9505ERR_MAX      10              /* 连续错误次数达到设置值后，强制切换 PIC24 */
PRIVATE int8u PCA9505ErrCnt[2] = {0, 0};    /* 错误次数计数器 */

PRIVATE void checkPCA9505Err(int8u node)
{
    int8u inx;
    if(node == COMMOBJ_IORW_A)
        inx = 0;
    else if(node == COMMOBJ_IORW_B)
        inx = 1;
    else
        return;
    
    /* 判断 I2C 错误 */
    tSlaveInfo * pSlvInfo = getSlaveInfoPtr(node);
    if(pSlvInfo != NULL)
    {
        if  ( (pSlvInfo->slaveLinkOK) && 
             ((pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err1) || 
              (pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err2) || 
              (pSlvInfo->slaveInput.pic24In.stsFlags.bits.pca9505Err3))
            )
        {
            if(PCA9505ErrCnt[inx] < PCA9505ERR_MAX)
            {
                PCA9505ErrCnt[inx] += 1;
            }
        }
        else
        {
            PCA9505ErrCnt[inx] = 0;
        }
    }
    /* 10.13 测试用：
     *  发现PIC24A没有烧录程序时，PIC32 偶尔能收到PIC24A的数据
     *  测试结果：MONITOR-V1 有问题，是张正业在工厂使用的，PIC24B访问PCA9505失败
    if(PCA9505ErrCnt[0] > 0)
    {
        PCA9505ErrCnt[0] = 0;
    }
     */
}

/* 读PIC24访问 PCA9505 错误计数器
 * 返回 
 *  对应的错误计数器值 
 */
PUBLIC int8u getPCA9505ErrCnt(int8u node)
{
    if(node == COMMOBJ_IORW_A)
        return PCA9505ErrCnt[0];
    else if(node == COMMOBJ_IORW_B)
        return PCA9505ErrCnt[1];
    else
        return 0;
}

/* 判断PIC24访问 PCA9505 错误状态
 * 返回 
 *  TRUE 表示有错误 
 */
PUBLIC BOOL ifPCA9505Err(int8u node)
{
    if( getPCA9505ErrCnt(node) >= PCA9505ERR_MAX )
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* =============================================================================
 * 查询是否有需要处理的消息
 * 返回
 *  TRUE  接收从机信息正确
 *  FALSE 没有接收到或接收到错误的信息
 */
PRIVATE BOOL Pic24_PollingProcRxMsg(void)
{
    BOOL ret;
    
    /* PIC24 Bootloader 启动升级过程后接收数据，不在这里处理
     *  */
    
    /* 正常轮询：数据接收 */
    ret = IPS_ProcRxFrame(&uartPic24, PollingPic24.sendPkgHeader.dstNode, TRUE);
    
    if(ret == TRUE)
    {
        checkPCA9505Err(PollingPic24.sendPkgHeader.dstNode);
    }
    
    return ret;
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART 
 * 
 *******************************************************************************
 *******************************************************************************/
PRIVATE void initPic24Comm(void)
{
    DeInitUartCom(COM_PIC24, &uartPic24);
    
    uartPic24.RxTxBuf.pRxBuf        = RxPic24DataBuf;           /* 接收数据区指针               */
    uartPic24.RxTxBuf.rxBufSize     = sizeof(RxPic24DataBuf);   /* 接收数据区大小               */
    uartPic24.RxTxBuf.rxCnt         = 0;                        /* 已接收的数据长度             */
  
    uartPic24.RxTxBuf.pTxBuf        = TxPic24DataBuf;           /* 发送数据区指针               */
    uartPic24.RxTxBuf.txBufSize     = sizeof(TxPic24DataBuf);   /* 发送数据区大小               */
    uartPic24.RxTxBuf.txCnt         = 0;                        /* 已发送数据的长度             */
    uartPic24.RxTxBuf.txDataLen     = 0;                        /* 待发送数据的长度，0表示没有  */
      
    uartPic24.funRx                 = &Pic24_UartReceiveData;   /* 串口中断接收一个字节后调用           */
    uartPic24.funTx                 = NULL;                     /* 发送数据结束后中断调用, null表示没用 */
          
    uartPic24.RS485_4Wire           = TRUE;                     /* 软件角度，RS232可以看成全双工RS485 */
    uartPic24.fpDE_Dis              = NULL;
    uartPic24.fpDE_En               = NULL;
    uartPic24.fpRE_Dis              = NULL;
    uartPic24.fpRE_En               = NULL;
      
    uartPic24.frameOverTime         = 5;            /* ms，帧间隔时间，若此时间内没有数据认为帧结束 */
    uartPic24.sendOverTime          = 100;          /* ms，设置发送数据超时时间长度                 */
    uartPic24.abortFrameTime        = 500;          /* ms, 若超过指定的时间没处理，则丢弃该帧数据   */
    uartPic24.linkFailTime          = 3000;

    uartCOM_Init(COM_PIC24, UART_PIC24_ID, &uartPic24, UART_PIC24_BR, UART_PIC24_IPL);
}

/* =========================================================
 * 模块初始化
 */
PUBLIC void initPic24Module(void)
{
    tSlaveInfo *pSlaveInfo;
    
    /* 串口初始化 */
    initPic24Comm();
    
    /* 存储器初始化 */
    memset((int8u*)&PollingPic24, 0, sizeof(PollingPic24));
    
    PollingPic24.pollingCom = &uartPic24;
    PollingPic24.pollingObjList = (int8u *)pollingPic24List_A;
    PollingPic24.pollingObjNum  = sizeof(pollingPic24List_A);
    PollingPic24.setWaitTXTime  = SELECTORPIC24_DELAY_MS;
    
    PollingPic24.fpCbProcRxPkg      = Pic24_PollingProcRxMsg;
    PollingPic24.PollingTurnTime    = PIC24_INTURN_TIME;
    PollingPic24.PollingTxOvTime    = PIC24_TX_OVERTIME;
    PollingPic24.ErrMaxNumOffLine   = PIC24_OFFLINE_NUM;
    
    /* 工作主PIC24 选择 */
    MainWorkChipAB = MAINWORK_PIC24A;
    prevWorkChipAB = !MainWorkChipAB;
    selectWorkChipAB(MainWorkChipAB);
    
    /* 复制 A 到 B */
    memcpy((int8u*)(getSlaveInfoPtr(COMMOBJ_IORW_B)), getSlaveInfoPtr(COMMOBJ_IORW_A), sizeof(pSlaveInfo));
}


/* 
 * =============================================================================
 * 与 PIC24 通信处理
 * 需要在主程序中循环调用
 * =============================================================================
 */
#define UPDATE_INTERVAL_MS      10      /* 查询升级状态间隔时间 ms */
PUBLIC void processPic24Comm(void)
{
    static  tSYSTICK changePic24Time;
    static  tSYSTICK upPic24Tick;
    tSYSTICK tickNow;
    tSlaveInfo *pSlaveInfo;
    tSlaveInfo *pSlaveInfo_s;
    int8u pcaErrCnt;
    
    tickNow = getSysTick();
    
    
    #if (PIC24_BL_ENABLE == 1)
    if(chkIsUpdatingPic24() == TRUE)
    {
        /* 定时执行升级 */
        if((tickNow - upPic24Tick) >= UPDATE_INTERVAL_MS)
        {
            upPic24Tick = tickNow;
            Pic24UpdateProcess(&uartPic24);
        }
        
        /* 是否有待处理的数据帧
         *  */
        upPIC24_ChkP24Frame();
    }
    else
    #endif
    {
        /* 轮询 PIC24A + PIC24B
         */
        if(ifMainWorkPic24A())
        {
            pSlaveInfo  = getSlaveInfoPtr(COMMOBJ_IORW_A);
            pSlaveInfo_s= getSlaveInfoPtr(COMMOBJ_IORW_B);
            PollingPic24.pollingObjList = (int8u *)pollingPic24List_A;
            PollingPic24.pollingObjNum  = sizeof(pollingPic24List_A);
        }
        else
        {
            pSlaveInfo  = getSlaveInfoPtr(COMMOBJ_IORW_B);
            pSlaveInfo_s= getSlaveInfoPtr(COMMOBJ_IORW_A);
            PollingPic24.pollingObjList = (int8u *)pollingPic24List_B;
            PollingPic24.pollingObjNum  = sizeof(pollingPic24List_B);
        }

        /* 当接收到数据帧以后，此函数会回调 Pic24_PollingProcRxMsg
         *  */
        Uni_PollingSlaves((void *)&PollingPic24);
    
        #if 1   //0=屏蔽  调试使用
        /* 根据通信状态，判断是否需要切换 PIC24主从关系 
         * 与主PIC24的通信错误次数达到某个值后，或者主 PIC24 与 I2C芯片PCA9505通信错误，
         * 自动切换到另一个PIC24
         * 主机轮询一次的时间最少时间为 55ms (= IPS_INTURN_TIME + SELECTOR_DELAY_MS)
         * 每 4 次轮询主PIC24 3次，在出错情况下约 220ms 有3次错误，按 30/3 * 220 = 2200ms
         * 如果同一个串口轮询 IBP PSL SIG PIC24A PIC24B，注意 PIC24_SWITCH_ERRNUM 的设置值
         */
        if(ifMainWorkPic24A())
            pcaErrCnt = PCA9505ErrCnt[0];
        else
            pcaErrCnt = PCA9505ErrCnt[1];
    
        if((pSlaveInfo->commErrCnt >= PIC24_SWITCH_ERRNUM) || (pcaErrCnt >= PCA9505ERR_MAX))
        {
            /* 为了避免频繁切换，限制最小切换时间间隔 */
            if((tSYSTICK)(tickNow - changePic24Time) >= 2000)
            {
                changePic24Time = tickNow;

                /* 停止收发 */
                uartResetRecv(uartPic24.usrComNo);
                uartStopSend(uartPic24.usrComNo);

                /* 切换主 PIC24 */
                selectWorkChipAB(!MainWorkChipAB);
            }
        }
        #endif
    }
    
    /* 定时清除离线的PIC24的版本信息，再次上线后需要读取版本信息 
     * 主要是考虑在线升级后会自动重启，MON 自动再次重读
     */
    static tSYSTICK tickReadVer;
    tSlaveInfo* pSlvInfo;
    if((tSYSTICK)(tickNow - tickReadVer) > 100)
    {
        tickReadVer = tickNow;
        
        if(getObjCommLinkSts(COMMOBJ_IORW_A) == FALSE)
        {
            pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_IORW_A);
            pSlvInfo->VersionFlag = 0;
        }

        if(getObjCommLinkSts(COMMOBJ_IORW_B) == FALSE)
        {
            pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_IORW_B);
            pSlvInfo->VersionFlag = 0;
        }
        
        
        /* 判断串口链接状态及发送状态是否异常 
         * 与任意一个PIC24通信成功，则本机 UART OK
         */
        if(uartChkLinkAndSendSts(COM_PIC24) == TRUE)
        {
            /* 检测串口通信错误标志
             * 若出错，则重新初始化串口
             *  */
            if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART_PIC24_ID)) )
            {
                INTClearFlag(INT_SOURCE_UART_ERROR(UART_PIC24_ID));
                initPic24Comm();
            }
        }
    }
}

/* =============================================================================
 * 获取输出数据指针
 *    PC 测试软件输出时间完成后(testPic24OutputMs = 0)，需要切换到正常的输出值
 */
PUBLIC int8u * getPic24OutputIODataPtr(int * len)
{
    *len = sizeof(MON_output_data);
    
    int8u *pSrc;
    if(chkIfTestOutputIO() == FALSE)
    {
        pSrc = (int8u*)&MON_output_data[0].byte;
    }
    else
    {
        pSrc = getTestOutputIOData();
    }
    
    return pSrc;
}

#if 0
/* =============================================================================
 * 更新 PIC24 的输出 IO 数据 (A and B)
 */
PUBLIC int updatePic24Outdata(int8u *outData, int8u outLen)
{
    int ret = 0;
    //TODO
    tSlaveInfo *pic24Info;
    
    //PIC24A
    pic24Info = (tSlaveInfo *)getSlaveInfoPtr(COMMOBJ_IORW_A);
    if(pic24Info != NULL)
    {
        if(outLen <= sizeof(pic24Info->slaveOutput.wrBuf))
        {
            pic24Info->WrDataLen = outLen;
            memcpy((int8u*)pic24Info->slaveOutput.wrBuf, outData, outLen);
            ret |= 1;
        }
    }
    
    //PIC24B
    pic24Info = (tSlaveInfo *)getSlaveInfoPtr(COMMOBJ_IORW_B);
    if(pic24Info != NULL)
    {
        if(outLen > sizeof(pic24Info->slaveOutput.wrBuf))
        {            
            pic24Info->WrDataLen = outLen;
            memcpy((int8u*)pic24Info->slaveOutput.wrBuf, outData, outLen);
            ret |= 2;
        }
    }
    
    return ret;
}
#endif

