/*******************************************************************************
 * 串口通信 ：轮询功能
 *  Uni_PollingSlaves   : 轮询任务函数，支持轮询 IBP/PSL/SIG，轮询 PIC24A/PIC24B 
 *  Uni_ProcRecvedFrame : 判断指定串口是否有符合要求的数据帧并通过回调函数处理
 * 
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "IOFunct.h"
#include "IOConfig.h"
#include "waveflash.h"

#include "IPSUart.h"
#include "IPS_App.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

/* =============================================================================
 * 本地变量
 */
PRIVATE int8u RxIPSDataBuf[UART_COM_RXBUFSIZE];         /* 串口通信控制-接收缓存 */
PRIVATE int8u TxIPSDataBuf[UART_COM_TXBUFSIZE];         /* 串口通信控制-发送缓存 */

PRIVATE tUartComm * pUartComm;

PUBLIC void setCommunObjAB(tEnmCommObj obj);

#define QUERY_EXTCMD_INTURN     /* MMS_App.c 会插入控制命令(createTx2LogPackage) */
#ifdef  QUERY_EXTCMD_INTURN
/* 发送控制命令用的缓存
 * 与轮询发送的缓存分开
 *  */
PRIVATE BOOL    bSendExtCmd = FALSE;
PRIVATE int8u   SendExtCmdBuf[UART_COM_TXBUFSIZE];
PRIVATE int16u  SendExtCmdLen;
PRIVATE tEnmSendPkgStep sendExtendCommand(void);
#endif

/* =============================================================================*/
PRIVATE void uartIPS_DE_Dis(void)
{
    DE_IPS_DISABLE();
}

PRIVATE void uartIPS_DE_En(void)
{
    DE_IPS_ENABLE();
}

PRIVATE void uartIPS_RE_Dis(void)
{
    RE_IPS_DISABLE();
}

PRIVATE void uartIPS_RE_En(void)
{
    RE_IPS_ENABLE();
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART 
 * 
 *******************************************************************************
 *******************************************************************************/
/* 串口初始化      
 *  pUart       : 串口通信结构指针
 *  */
PUBLIC void initIPSComm(tUartComm * pUart)
{
    pUartComm = pUart;
    
    DeInitUartCom(COM_IPS, pUartComm);                      /* COM_IPS 是虚拟串口号 */
    
    pUartComm->RxTxBuf.pRxBuf       = RxIPSDataBuf;         /* 接收数据区指针               */
    pUartComm->RxTxBuf.rxBufSize    = sizeof(RxIPSDataBuf); /* 接收数据区大小               */
    pUartComm->RxTxBuf.rxCnt        = 0;                    /* 已接收的数据长度             */
   
    pUartComm->RxTxBuf.pTxBuf       = TxIPSDataBuf;         /* 发送数据区指针               */
    pUartComm->RxTxBuf.txBufSize    = sizeof(TxIPSDataBuf); /* 发送数据区大小               */
    pUartComm->RxTxBuf.txCnt        = 0;                    /* 已发送数据的长度             */
    pUartComm->RxTxBuf.txDataLen    = 0;                    /* 待发送数据的长度，0表示没有  */
     
    pUartComm->funRx                = &IPS_UartReceiveData; /* 串口中断接收一个字节后调用           */
    pUartComm->funTx                = NULL;                 /* 发送数据结束后中断调用, null表示没用 */
         
    pUartComm->RS485_4Wire          = TRUE;                 /* 全双工 485  */
    pUartComm->fpDE_Dis             = &uartIPS_DE_Dis;
    pUartComm->fpDE_En              = &uartIPS_DE_En;
    pUartComm->fpRE_Dis             = &uartIPS_RE_Dis;
    pUartComm->fpRE_En              = &uartIPS_RE_En;
     
    pUartComm->frameOverTime        = 10;           /* ms，帧间隔时间，若此时间内没有数据认为帧结束 */
    pUartComm->sendOverTime         = 200;          /* ms，设置发送数据超时时间长度                 */
    pUartComm->abortFrameTime       = 500;          /* ms, 若超过指定的时间没处理，则丢弃该帧数据   */
    pUartComm->linkFailTime         = IPS_LINKFAIL_TIME;

    uartCOM_Init(COM_IPS, UART_IPS_ID, pUartComm, UART_IPS_BR, UART_IPS_IPL);
}

/* =============================================================================
 * 执行轮询任务
 * 参数
 *  pPollingPtr     : 轮询任务指针
 * 返回
 *  本次轮询对象
 */
PUBLIC tEnmCommObj Uni_PollingSlaves(void *pPollingPtr)
{
    tEnmCommObj     pollingObj;         /* 轮询对象 */
    tSlaveInfo   *  pPollingSlvInfo;    /* 轮询对象信息 */
    
    int     txDataLen;
    int8u * txDataPtr = NULL;
    BOOL    uartTxBusy;                 /* 指定串口的发送状态, TRUE = 忙 */
    
    if(pPollingPtr == NULL)
        return COMMOBJ_UNDEF;
    
    tPollingTask *pPollingTask = (tPollingTask *)pPollingPtr;
    
    /* 轮询对象的数量 */
    if(pPollingTask->pollingObjNum == 0)
    {
        return COMMOBJ_UNDEF;
    }
    
    /* 本次查询的顺序号 */
    if(pPollingTask->pollingTurn >= pPollingTask->pollingObjNum)
    {
        pPollingTask->pollingTurn = 0;
    }

    /* 本次查询的对象 */
    pollingObj      = pPollingTask->pollingObjList[pPollingTask->pollingTurn];
    pPollingSlvInfo = getSlaveInfoPtr(pollingObj);
    
    /* 轮询步骤 */
    uartTxBusy  = ((pPollingTask->pollingCom)->uartSts).isSending;
    switch(pPollingTask->sendPkgStep)
    {
        /* 没有发送任务             
         */
        case SENDSTEP_IDLE:
            /* 这里兼容同时轮询 IBP PSL SIG PIC24A PIC24B 
             *       或单独轮询 PIC24A PIC24B 
             */
            
            setCommunObjAB(pollingObj);
                
            if((pollingObj == COMMOBJ_IORW_A) || (pollingObj == COMMOBJ_IORW_B))
            {
                /* 根据通信对象设置IO输出 
                 */
                pPollingTask->sendPkgStep = SENDSTEP_WAITTX;
            }
            else
            {
                /* IBP PSL SIG 板不需要等待延时 */
                pPollingTask->sendPkgStep = SENDSTEP_NEWTASK;
            }
            
            pPollingTask->pollingStepTime = getSysTick();
            break;
            
        /* 延时，主要目的保证硬件IO切换后稳定 */
        case SENDSTEP_WAITTX:
            if(getSysTimeDiff(pPollingTask->pollingStepTime) < pPollingTask->setWaitTXTime)
                break;
            pPollingTask->sendPkgStep = SENDSTEP_NEWTASK;
            //break;        //不 break 直接进入下一步
            
        case SENDSTEP_NEWTASK:
            /* 等待串口空闲 */
            if(uartTxBusy)
                break;
            
            (pPollingTask->sendPkgHeader).dstNode   = (int8u)pollingObj;
            (pPollingTask->sendPkgHeader).seq       = pPollingSlvInfo->sendSeq;
            
            /* 定时读取对象的继电器工作寿命记录 */
            if(ifReadRelayLife((int8u)pollingObj))
            {
                /* 无论是否读取成功，下一次都执行正常的轮询任务 */
                clearReadRelayLife((int8u)pollingObj);
                
                (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_RELAY_LIFE;    /* 读继电器寿命信息 */
                
                txDataLen = 0;

                /* 创建发送数据包并发送 */
                pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
            }
            /* 正常轮询 */
            else
            {
                if(pPollingSlvInfo->VersionFlag == 0)
                {
                    /* tPktRequestVersion */
                    (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_DEVINFO;    /* 读软件版本信息 */
                    txDataLen = 0;

                    /* 创建发送数据包并发送 */
                    pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                }
                else
                {
                    (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_IOR;        /* 读IO数据     */

                    if((pollingObj == COMMOBJ_IORW_A) || (pollingObj == COMMOBJ_IORW_B))
                    {
                        txDataPtr = (int8u*)getPic24OutputIODataPtr(&txDataLen);

                        /* 创建发送数据包并发送 */
                        pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                    }
                    /* 2024-6-17 : 增加切换时的报警监视（综合监控报PEDC故障），以及增加板1和板2之间的自动切换功能。 */
                    else if((pollingObj == COMMOBJ_SWITCHER))
                    {
                        extern int8u* getSwitchOutputIODataPtr(int *DataLen);
                        
                        txDataPtr = (int8u*)getSwitchOutputIODataPtr(&txDataLen);
                        /* 创建发送数据包并发送 */
                        pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                    }
                    else
                    {
                        #ifdef  QUERY_EXTCMD_INTURN
                        if((bSendExtCmd) && ((pollingObj == COMMOBJ_LOG1) || (pollingObj == COMMOBJ_LOG2)))
                        {
                            /* 在轮询任务中插入发送控制命令 */
                            pPollingTask->sendPkgStep = sendExtendCommand();
                        }
                        else
                        #endif
                        {
                            #if 1
                            txDataPtr = NULL;
                            txDataLen = 0;
                            #else
                            txDataPtr = (int8u*)(&pPollingSlvInfo->slaveOutput.wrBuf);
                            txDataLen = pPollingSlvInfo->WrDataLen;
                            #endif

                            /* 创建发送数据包并发送 */
                            pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                        }
                    }
                }
            }

                    
            pPollingTask->pollingStepTime = getSysTick();
            (pPollingTask->pollingCom)->RxTxBuf.rxCnt = 0;
            (pPollingTask->pollingCom)->uartSts.gotFrame = 0;
            break;
        
        /* 等待发送完成             
         */
        case SENDSTEP_SENDING:  
            if(uartTxBusy)
            {
                /* 
                 * 如果超过指定时间没有发送结束
                 * 强制停止发送
                 */
                if(getSysTimeDiff(pPollingTask->pollingStepTime) < pPollingTask->PollingTxOvTime)
                {
                    break;
                }
                else
                {
                    /* 发送超时，强制停止发送
                     * 通信错误计数器自增 */
                    uartStopSend((pPollingTask->pollingCom)->usrComNo);
                    
                    pPollingSlvInfo->commErrCnt ++;
                    if(pPollingSlvInfo->commErrCnt > IPS_ERRCNT_MAX)
                    {
                        pPollingSlvInfo->commErrCnt = IPS_ERRCNT_MAX;
                    }

                    pPollingTask->sendPkgStep = SENDSTEP_FINISH;
                }
            }
            else
            {
                pPollingTask->pollingStepTime   = getSysTick();
                pPollingTask->sendPkgStep       = SENDSTEP_WAITRX;
            }
            break;
            
        /* 发送查询命令完成后，等待回复         
         */
        case SENDSTEP_WAITRX:   
            /* 处理接收命令 
             *
             * 查询是否有需要处理的消息
             * 返回 TRUE  接收从机信息正确
             */
            if(pPollingTask->fpCbProcRxPkg != NULL)
            {
                if(Uni_ProcRecvedFrame(pPollingTask->pollingCom, pPollingTask->fpCbProcRxPkg) == TRUE)
                {
                    /* 正确接收 */
                    pPollingTask->sendPkgStep = SENDSTEP_FINISH;
                }
                else if(getSysTimeDiff(pPollingTask->pollingStepTime) >= pPollingTask->PollingTurnTime)
                {
                    /* 接收等待超时 */
                    pPollingSlvInfo->commErrCnt ++;
                    if(pPollingSlvInfo->commErrCnt > IPS_ERRCNT_MAX)
                    {
                        pPollingSlvInfo->commErrCnt = IPS_ERRCNT_MAX;
                    }

                    pPollingTask->sendPkgStep = SENDSTEP_FINISH;
                }
            }
            else
            {
                pPollingTask->sendPkgStep = SENDSTEP_FINISH;
            }
            break;
            
        /* 单次查询完成     
         */
        case SENDSTEP_FINISH:   
            if(getSysTimeDiff(pPollingTask->pollingStepTime) >= pPollingTask->PollingTurnTime)
            {
                if(pPollingSlvInfo->commErrCnt >= pPollingTask->ErrMaxNumOffLine)
                {
                    pPollingSlvInfo->slaveLinkOK    = 0;
                    
                    /* 重新连线后需要丢弃前面几次的数据 */
                    pPollingSlvInfo->dataValidCnt   = 3;   
                }

                pPollingTask->pollingTurn ++;
                pPollingTask->sendPkgStep = SENDSTEP_IDLE;
            }
            break;
    }
    
    return pollingObj;
}


/******************************************************************************
 * 
 *      通用串口功能接口函数
 * 
 *      必须在统一收发通信协议的条件下，支持:
 *      1. 同一个串口轮询不同组合，如  (IPB + PSL + SIG)、(IPB + PSL + SIG + PIC24A + PIC24B) 
 *      2. 不同串口分别轮询  (IPB + PSL + SIG)  (PIC24A + PIC24B)
 * 
 *******************************************************************************
 */

/* =============================================================================
 * 通用串口接收字节处理函数
 *  pUartCom    : 串口结构指针
 *  data        ：接收的字节数据
 */
//PUBLIC void Uni_UartReceiveData(tUartComm *pUartCom, int8u data)
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data)
{
    volatile int16u dataLen; 
    
    tSYSTICK nowTick;
    
    nowTick = getSysTick();

    /* 接收到的上次命令没有处理
     * 若超过指定的时间没处理，则丢弃该帧数据
     */
    if(pCom->uartSts.gotFrame != 0)
    {
        if(pCom->abortFrameTime > 0)
        {
            if((tSYSTICK)(nowTick - pCom->gotFrameTick) >= pCom->abortFrameTime)
            {
                pCom->RxTxBuf.rxCnt     = 0;
                pCom->uartSts.gotFrame  = 0;
                pCom->gotFrameTick      = nowTick;
            }
        }
        
        if(pCom->uartSts.gotFrame != 0)
        {
            return;
        }
    }

    /* 1. 接收的数据长度超过接收缓冲区容量，则清空接收重新计数 
     * 2. 两字节之间的接收时间限制，超过时间则认为是新的一帧数据，重新接收计数 
     * 
     * 已经在驱动层处理（uartDrv.c --> lastByteTick）
     */
    
    /* save the received data */
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = data;
    pCom->RxTxBuf.rxCnt ++;

    /* 搜索数据帧 */
    /* 是否已经找到帧头信息 */
    if(pCom->RxTxBuf.rxCnt >= sizeof(tPktHeader) + sizeof(tPktTail) )
    {
        /* 数据包长度 */
        dataLen = ((int16u)pCom->RxTxBuf.pRxBuf[6] << 8) + (int16u)(pCom->RxTxBuf.pRxBuf[7]);
        
        /* 判断接收的数据总数  */
        if((pCom->RxTxBuf.rxCnt >= (dataLen + sizeof(tPktHeader) + sizeof(tPktTail))))
        {
            if((pCom->RxTxBuf.pRxBuf[dataLen + 8] == TAIL_DLE) && (pCom->RxTxBuf.pRxBuf[dataLen + 9] == TAIL_ETX))
            {
                /* 得到完整的数据帧
                 * 记录当前的时间点 
                 */
                pCom->uartSts.gotFrame  = 1;
                pCom->gotFrameTick      = nowTick;
                
                /* 这里不能认为通信正常，因为数据帧还没有进行校验 
                 * pCom->uartSts.linkState = 1;
                 */
            }
            else
            {
                /* 重新接收 */
                pCom->RxTxBuf.pRxBuf[0] = data;
                pCom->RxTxBuf.rxCnt = 1;
                
                #if 0
                if(uartUni.errFrame < 200)
                {
                    uartUni.errFrame ++;
                }
                #endif
            }
        }
    }
    else
    {
        /* 搜索帧头信息 */
        switch(pCom->RxTxBuf.rxCnt)
        {
            case 1:     /* 0: Header 1 */
                if (pCom->RxTxBuf.pRxBuf[0] != HEADER_DLE)
                {
                    pCom->RxTxBuf.rxCnt = 0;
                    return;
                }
                break;
            case 2:     /* 1: Header 2 */
                if (pCom->RxTxBuf.pRxBuf[1] != HEADER_STX)
                {
                    pCom->RxTxBuf.pRxBuf[0] = data;
                    pCom->RxTxBuf.rxCnt = 1;
                    return;
                }
                break;
                
            case 3:     /* 2: 帧序号                        */
            case 4:     /* 3: 源节点,                       */
            case 5:     /* 4: 对象节点 tEnmCommObj          */
            case 6:     /* 5: 通信数据类型 tEnmCommObjCmd   */
            case 7:     /* 6: 数据长度H */     
            case 8:     /* 7: 数据长度L */    
                break;
                
            default :
                break;
        }
        
    }
}

/* =============================================================================
 * 通用串口接收数据帧处理函数
 *  pUartCom        : 执行串口任务的指针
 *  pCbProcFrame    : 接收到完整的数据帧后回调函数
 */
PUBLIC BOOL Uni_ProcRecvedFrame(tUartComm * pUartCom, cbProcRxPkg pCbProcFrame)
{
    int16u crc16;
    int16u rxDataLen;
    
    tPktHeader *pHeader;
    tPktTail   *pTail;
    
    tUartBuf    *RxTxBufPtr;
    
    /* 返回结果 */
    BOOL rxMsgSts = FALSE;
    
    /* 避免用到空指针 */
    if(pUartCom == NULL)
        return rxMsgSts;
        
    RxTxBufPtr = &(pUartCom->RxTxBuf);

    /* got a error frame ? */
	if(pUartCom->uartSts.errFrame != 0)
	{
		pUartCom->uartSts.errFrame = 0;
        pUartCom->uartSts.gotFrame = 0;
        RxTxBufPtr->rxCnt = 0;
		//CrcErr();
        return rxMsgSts;
	}

    /* 若收到数据帧，则进行处理 */
    if(pUartCom->uartSts.gotFrame != 0)
    {
        do
        {
            pHeader = (tPktHeader *)(pUartCom->RxTxBuf.pRxBuf);

            rxDataLen = ((int16u)pHeader->dataLenH << 8) + (int16u)pHeader->dataLenL;

            pTail   = (tPktTail   *)(pUartCom->RxTxBuf.pRxBuf + rxDataLen + sizeof(tPktHeader));

            #if (FRAME_VERIFY_CRC16 == 1)
            /* 校验接收数据帧是否正确 
             * CRC16 校验
             *  -2 ：计算长度不包含最后的校验码
             */
            crc16 = CalcCRC16_A001(pUartCom->RxTxBuf.pRxBuf,    rxDataLen + sizeof(tPktHeader) + sizeof(tPktTail) - 2, 0x00);
            if( ((int16u)(pTail->crc16L) + ((int16u)(pTail->crc16H) << 8)) != crc16)
            {
                /* 校验错误，丢弃 */
                break;
            }

            #else
            /* 校验接收数据帧是否正确 
             * 异或码 校验
             *  -1 ：计算长度不包含最后的校验码
             */
            crc16 = checksum(pUartCom->RxTxBuf.pRxBuf,    rxDataLen + sizeof(tPktHeader) + sizeof(tPktTail) - 1);
            if((int16u)(pTail->crcXOR) != crc16)
            {
                /* 校验错误，丢弃 */
                break;
            }
            #endif

            /* 有待处理的数据帧
             * 通过回调函数处理 */
            if(pCbProcFrame != NULL)
            {
                rxMsgSts = pCbProcFrame();
            }
            
        }while(0);
        
        pUartCom->RxTxBuf.rxCnt    = 0;
        pUartCom->uartSts.gotFrame = 0;
    }
    
    
    return rxMsgSts;
}


/* =============================================================================
 * 创建发送数据包
 * 输入
 *  pPollingTask    : 轮询任务指针
 *  data            : 待发送数据指针
 *  dataLen         : 待发送数据长度
 *  sendNow         : 发送标志，= true, 表示立即启动发送
 * 返回发送创建状态
 */
PUBLIC tEnmSendPkgStep createUniTxPackage(void *pPollingPtr, int8u* data, int dataLen, BOOL sendNow)
{
    int i;
    int8u *pTxPkg;
    int8u *pSrc;
    int16u chkCode;
    (void)  sendNow;
    int     txLen;
    
    tEnmSendPkgStep retStep;
    
    tPollingTask *pPollingTask = (tPollingTask *)pPollingPtr;
            
    pTxPkg = (pPollingTask->sendPkgBuf);
    tPktHeader *pkgHeader = &(pPollingTask->sendPkgHeader);
    
    
    *pTxPkg ++= HEADER_DLE;                         /* header 1         */
    *pTxPkg ++= HEADER_STX;                         /* header 2         */
    *pTxPkg ++= pkgHeader->seq;                     /* 顺序号           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* 通信数据源节点   */
    *pTxPkg ++= pkgHeader->dstNode;                 /* 通信数据目标节点 */
    *pTxPkg ++= pkgHeader->msgType;                 /* 通信数据类型     */
    *pTxPkg ++= (int8u)(dataLen >> 8);              /* 数据长度 H       */
    *pTxPkg ++= (int8u)(dataLen);                   /* 数据长度 L       */
    
    
    /* 填充数据 */
    if(data != NULL)
    {
        pSrc = data;
        for(i=0; i<dataLen; i++)
        {
            *pTxPkg ++ = *pSrc++;
        }
    }
    
    *pTxPkg ++= TAIL_DLE;                       /* tail             */
    *pTxPkg ++= TAIL_ETX;                       /* tail             */
    
    txLen = pTxPkg - (pPollingTask->sendPkgBuf);           /* 待计算校验的数据长度 */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* 计算数据包的校验 : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), txLen, 0x00);
    *pTxPkg ++= chkCode;                        /* 校验码  L        */
    *pTxPkg ++= chkCode >> 8;                   /* 校验码  H        */
    #else
    /* 计算数据包的校验 : 异或码
     */
    chkCode   = checksum((int8u*)(&(pPollingTask->sendPkgBuf)), txLen);
    *pTxPkg ++= chkCode;                        /* 校验码  L        */
    #endif
    
    /* 待发送数据长度 */
    txLen = pTxPkg - (pPollingTask->sendPkgBuf);
    
    /* 启动发送 */
    if(txLen > 0)
    {
        pPollingTask->sendPkgLen = txLen;
        
        if((sendNow) && (pPollingTask->pollingCom->uartSts.isSending == 0))
        {
            /* 立即启动发送 */
            clrIPSUpdatePkgFlag();
            uartStartSend(pPollingTask->pollingCom->usrComNo, pPollingTask->sendPkgBuf, txLen);
            
            retStep = SENDSTEP_SENDING;
            
            #if (SHOW_PIC24_BY_UART_IPS == 1)
            if(!ifIPSUartTxIsBusy())
            {
                uartStartSend(getIPSUartCommPtr()->usrComNo, pPollingTask->sendPkgBuf, txLen);
            }
            #endif
        }
        else
        {
            /* 等待上一次的发送结束 */
            retStep = SENDSTEP_NEWTASK;
        }
    }
    else
    {
        /* 创建失败 */
        retStep = SENDSTEP_IDLE;
    }
    
    return retStep;
}


#ifdef  QUERY_EXTCMD_INTURN
/* =============================================================================
 * 创建发送数据包待发送给 逻辑板
 *  pkgHeader   : 头信息指针
 *  data        ： 数据指针
 *  dataLen     ： 数据长度
 *  sendNow     : 发送标志，= true, 表示立即启动发送
 * 返回待发送的数据长度，0 表示创建失败
 */
PUBLIC int16u createTx2LogPackage(tPktHeader *pkgHeader, int8u* data, int dataLen, BOOL sendNow)
{
    int i;
    int8u *pTxPkg;
    int8u *pSrc;
    int16u chkCode;
    (void)  sendNow;
    
    /* 判断缓冲区是否足够 */
    if(dataLen > sizeof(SendExtCmdBuf) - 11)
        return 0;
    
    pTxPkg = SendExtCmdBuf;
    
    *pTxPkg ++= HEADER_DLE;                         /* header 1         */
    *pTxPkg ++= HEADER_STX;                         /* header 2         */
    *pTxPkg ++= pkgHeader->seq;                     /* 顺序号           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* 通信数据源节点   */
    *pTxPkg ++= pkgHeader->dstNode;                 /* 通信数据目标节点 */
    *pTxPkg ++= pkgHeader->msgType;                 /* 通信数据类型     */
    *pTxPkg ++= (int8u)(dataLen >> 8);              /* 数据长度 H       */
    *pTxPkg ++= (int8u)(dataLen);                   /* 数据长度 L       */
    
    
    /* 填充数据 */
    if(data != NULL)
    {
        pSrc = data;
        for(i=0; i<dataLen; i++)
        {
            *pTxPkg ++ = *pSrc++;
        }
    }
    
    *pTxPkg ++= TAIL_DLE;                       /* tail             */
    *pTxPkg ++= TAIL_ETX;                       /* tail             */
    
    SendExtCmdLen = pTxPkg - SendExtCmdBuf;     /* 待计算校验的数据长度 */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* 计算数据包的校验 : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), SendExtCmdLen, 0x00);
    *pTxPkg ++= chkCode;                        /* 校验码  L        */
    *pTxPkg ++= chkCode >> 8;                   /* 校验码  H        */
    #else
    /* 计算数据包的校验 : 异或码
     */
    chkCode   = checksum((int8u*)(SendExtCmdBuf), SendExtCmdLen);
    *pTxPkg ++= chkCode;                        /* 校验码  L        */
    #endif
    
    /* 待发送数据长度 */
    SendExtCmdLen = pTxPkg - SendExtCmdBuf;
    
    bSendExtCmd = TRUE;
    
    return SendExtCmdLen;
}

PRIVATE tEnmSendPkgStep sendExtendCommand()
{
    tEnmSendPkgStep retStep = SENDSTEP_IDLE;
    
    /* 启动发送 */
    if((bSendExtCmd == TRUE) && (SendExtCmdLen > 0))
    {
        if((pUartComm->uartSts.isSending == 0))
        {
            /* 立即启动发送 */
            clrIPSUpdatePkgFlag();
            uartStartSend(pUartComm->usrComNo, SendExtCmdBuf, SendExtCmdLen);
            
            retStep     = SENDSTEP_SENDING;
            bSendExtCmd = FALSE;
        }
        else
        {
            /* 等待上一次的发送结束 */
            retStep = SENDSTEP_NEWTASK;
        }
    }
    
    return retStep;
}
#endif



