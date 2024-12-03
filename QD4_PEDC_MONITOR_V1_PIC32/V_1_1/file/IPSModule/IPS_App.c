/*******************************************************************************
 * 串口通信 模块
 * -> 主程序调用串口初始化      initIPSComm 
 * -> 主循环调用串口通信处理    processIPSComm 
 * 
 * 
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"
#include "IOFunct.h"
#include "IOConfig.h"
#include "waveflash.h"

#include "IPS_App.h"
#include "IPSUart.h"

#include "P24UpdateDrv.h"
#include "relayLife.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* 继电器动作次数信息
 * 记录从继电器开始使用后的动作次数 */
PRIVATE tRelayRecord    relayRecord[2];

/* 需要定时获取 逻辑控制板存储 的继电器寿命数据 */
BOOL bReadRelayLife[2] = {   FALSE, FALSE  };
PUBLIC BOOL ifReadRelayLife(int8u obj)
{
    if(obj == COMMOBJ_LOG1)
    {
        return bReadRelayLife[0];
    }
    if(obj == COMMOBJ_LOG2)
    {
        return bReadRelayLife[1];
    }
    
    return FALSE;
}

PUBLIC void clearReadRelayLife(int8u obj)
{
    if(obj == COMMOBJ_LOG1)
    {
        bReadRelayLife[0] = FALSE;
    }
    else if(obj == COMMOBJ_LOG2)
    {
        bReadRelayLife[1] = FALSE;
    }
}

PUBLIC void * getRelayRecordPtr(int8u obj)
{
    void *p = NULL;
    if(obj == COMMOBJ_LOG1)
    {
        p = (void *)(&relayRecord[0]);
    }
    else if(obj == COMMOBJ_LOG2)
    {
        p = (void *)(&relayRecord[1]);
    }
    
    return p;
}

/* =============================================================================
 * 本地变量
 */
PRIVATE int8u LogicTestData[2][100];
PRIVATE int16u LogicTestDataLen[2] = {0, 0};
PUBLIC int8u* getLogicTestData(int16u * len)
{
    if(len != NULL)
    {
        if(bLOG_WorkLOG2 == 0)          /* 板1 */
        {
            *len = LogicTestDataLen[0];
            return LogicTestData[0];
        }
        else                            /* 板2 */
        {            
            *len = LogicTestDataLen[1];
            return LogicTestData[1];
        }
    }
    else
    {
        return NULL;
    }
}

/* 轮询的对象列表及其控制 
 */
PRIVATE const int8u pollingIPSList[] = 
{
    COMMOBJ_LOG1,           /* LOG 板1 */
    COMMOBJ_LOG2,           /* LOG 板2 */
    COMMOBJ_SWITCHER,       /* 安全回路切换板 */
};

/* 轮询任务控制 */
PRIVATE tPollingTask PollingIPS;

/* 指向串口控制变量 */
PRIVATE tUartComm uartIPS;                              /* 串口通信控制 */
PRIVATE tUartComm * pUartIPS;
    PUBLIC tUartComm * getIPSUartCommPtr(void)
    {
        return &uartIPS;
    }
    
/* =============================================================================*/
//串口发送的数据包功能：TRUE 表示升级数据包
PRIVATE BOOL txIPSUpdatePkgFlag;
    PUBLIC void clrIPSUpdatePkgFlag(void)
    {
        txIPSUpdatePkgFlag = FALSE;
    }


/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
PUBLIC BOOL ifIPSUartTxIsBusy(void)
{
    return (BOOL)(uartIPS.uartSts.isSending);
}

/* Uart 查询外部PCB板在线状态 
 * 返回 FALSE 表示连接已断开，TRUE 表示连接正常
 */
PUBLIC BOOL IPSObj_chkOnline(int8u id)
{
    if((id == COMMOBJ_SWITCHER) || (id == COMMOBJ_LOG1) || (id == COMMOBJ_LOG2))
    {
        return getObjCommLinkSts((tEnmCommObj)id);
    }
    else
    {
        return FALSE;
    }
}

/* 串口接收到数据帧的处理程序
 *  */
PRIVATE BOOL IPS_PollingProcRxMsg(void);

/* =========================================================
 * 通信中断处理函数 : 与外部PCB板通信
 * 输入：
 *   module_id  : 串口ID
 *   data       : 一个字节数据
 */
PUBLIC void IPS_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    Uni_UartReceiveData(PollingIPS.pollingCom, data);
}

/* =============================================================================
 * 
 *                  外部PCB板在线升级的功能，通信接口函数
 * 
 * =============================================================================
 */

/*
 * 发送升级数据包
 */
PRIVATE BOOL IPSUpdate_TxPkg2IPS(int8u *pCmd, int pkgSize)
{
    if(pUartIPS == NULL)
        return FALSE;
    
    txIPSUpdatePkgFlag = FALSE;
    if(uartStartSend(pUartIPS->usrComNo, pCmd, pkgSize) == COM_TX_OK)
    {
        txIPSUpdatePkgFlag = TRUE;
    }
    
    return txIPSUpdatePkgFlag;
}
      
/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 
 * 若串口没有发送升级数据包，而是发送其它数据包，则返回 TRUE
 */
PRIVATE BOOL IPSUpdate_TxBusy(void)
{
    BOOL busy = TRUE;
    if(txIPSUpdatePkgFlag == TRUE)
    {
        if(ifIPSUartTxIsBusy() == FALSE)
        {
            busy = FALSE;
            txIPSUpdatePkgFlag = FALSE;
        }
    }
    
    return busy;
}

/* 供升级底层调用，查询 IBP PSL SIG 升级状态 
 *  id    : 查询对象的 ID
 *  secNo : 升级段号
 * 返回：
 *  TRUE  启动查询OK
 *  FALSE 启动查询失败（串口发送忙）
 */
PRIVATE BOOL IPSUpdate_QuerySlave(int8u id, int16u secNo)
{
    int8u           queryPkgLen;
    int8u           queryPkgBuf[16];
    
    int8u *pTxPkg;
    int16u chkCode;    
    
    if(pUartIPS == NULL)
        return FALSE;
    
    tSlaveInfo* pSlvInfo = getSlaveInfoPtr((tEnmCommObj) id);
    if(pSlvInfo == NULL)
        return FALSE;
            
    pTxPkg = queryPkgBuf;
    
    *pTxPkg ++= HEADER_DLE;                         /* header 1         */
    *pTxPkg ++= HEADER_STX;                         /* header 2         */
    *pTxPkg ++= pSlvInfo->sendSeq;                  /* 顺序号           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* 通信数据源节点   */
    *pTxPkg ++= id;                                 /* 通信数据目标节点 */
    *pTxPkg ++= OBJ_CMD_UPDATESTS;                  /* 通信数据类型     */
    
    *pTxPkg ++= 0;                                  /* 数据长度 H       */
    *pTxPkg ++= 2;                                  /* 数据长度 L       */    
    *pTxPkg ++= (int8u)(secNo >> 8);                /* 升级段号 H       */
    *pTxPkg ++= (int8u)(secNo);                     /* 升级段号 L       */
    
    *pTxPkg ++= TAIL_DLE;                           /* tail             */
    *pTxPkg ++= TAIL_ETX;                           /* tail             */
    
    queryPkgLen = pTxPkg - queryPkgBuf;             /* 待计算校验的数据长度 */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* 计算数据包的校验 : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), sendPkgLen, 0x00);
    *pTxPkg ++= chkCode;                        /* 校验码  L        */
    *pTxPkg ++= chkCode >> 8;                   /* 校验码  H        */
    #else
    /* 计算数据包的校验 : 异或码
     */
    chkCode   = checksum((int8u*)(queryPkgBuf), queryPkgLen);
    *pTxPkg ++= chkCode;                        /* 校验码           */
    #endif
    
    /* 待发送数据长度 */
    queryPkgLen = pTxPkg - queryPkgBuf;
    
    /* 若串口空闲则立即启动发送 */
    txIPSUpdatePkgFlag = FALSE;
    if((ifIPSUartTxIsBusy() == FALSE))
    {
        /* 立即启动发送 */
        if(uartStartSend(pUartIPS->usrComNo, queryPkgBuf, queryPkgLen) == COM_TX_OK)
        {
            txIPSUpdatePkgFlag = TRUE;
        }
    }
    
    return txIPSUpdatePkgFlag;
}

/* 供升级底层调用，是否收到从机的回复升级（标志）
 * 返回 TRUE 表示收到，同时返回 状态值和段号
 * 注意：
 *  此函数会清除标志
 */
PRIVATE BOOL IPSUpdate_GetUpdateInfo(int8u id, int8u *sts, int16u *secNo)
{
    tSlaveInfo* pSlvInfo = getSlaveInfoPtr((tEnmCommObj)id);
    
    BOOL ret = FALSE;
    if(pSlvInfo != NULL)
    {
        ret     = pSlvInfo->slaveReplyUpdate;
        
        if(ret == TRUE)
        {
            *sts    = pSlvInfo->slaveUpdateSts;
            *secNo  = pSlvInfo->slaveUpdateSec;
        }
        
        pSlvInfo->slaveReplyUpdate = FALSE;
    }
    
    return ret;
}

/* 处理接收数据帧 
 * 参数 
 *  pUartCom        : 串口控制结构体指针
 *  dstNode         : 指定待处理的对象
 *  incSeq          : TRUE = 序号自增
 * 返回
 *  TRUE 接收到指定对象的数据帧
 */
PUBLIC BOOL IPS_ProcRxFrame(tUartComm   *pUartCom, int8u dstNode, BOOL incSeq)
{
    tSlaveInfo *pSlvInfo;
    tPktHeader  rxPkgHeader;
    tPktHeader *pPkgHeader;
    int8u *     pData;
    int8u       rxSrcNode;
    int16u      rxDataLen;
    
    BOOL        rxMsgSts = FALSE;
    tUartBuf *  RxTxBufPtr = &(pUartCom->RxTxBuf);
    
    do
    {
        #if (SHOW_PIC24_BY_UART_IPS == 1)
        if(!ifIPSUartTxIsBusy())
        {
            RxTxBufPtr->pRxBuf[0] = 0xAB;
            RxTxBufPtr->pRxBuf[1] = 0xCD;
            uartStartSend(getIPSUartCommPtr()->usrComNo, RxTxBufPtr->pRxBuf, pUartCom->RxTxBuf.rxCnt);
            RxTxBufPtr->pRxBuf[0] = 0x10;
            RxTxBufPtr->pRxBuf[1] = 0x02;
        }
        #endif
        
        /* 处理收到的命令字 */
        pPkgHeader = (tPktHeader *)(pUartCom->RxTxBuf.pRxBuf);
        rxPkgHeader = * pPkgHeader;

        rxDataLen = ((int16u)rxPkgHeader.dataLenH << 8) + (int16u)rxPkgHeader.dataLenL;

        rxSrcNode = rxPkgHeader.srcNode;
        if(rxSrcNode != dstNode)
        {
            /* 接收对象与发送对象 不是对应的 */
            break;
        }

        pSlvInfo = getSlaveInfoPtr(dstNode);
        if(pSlvInfo == NULL)
        {
            break;
        }

        #if (PIC24_BL_ENABLE != 0)
        /* 正常轮询时不处理收到 PIC24 回复的与 Bootloader 相关的信息 
         */
        if(((tEnmCommObjCmd)(rxPkgHeader.msgType) == OBJ_CMD_BL_CTRL) || 
           ((tEnmCommObjCmd)(rxPkgHeader.msgType) == OBJ_CMD_BOOTLOADER))
        {
            break;
        }
        #endif
                
                
        rxMsgSts = TRUE;

        /* 接收了正确的通信数据帧，通信连接 OK */
        pUartCom->uartSts.linkState = 1;
        pSlvInfo->slaveLinkOK       = 1;
        pSlvInfo->commErrCnt        = 0;
        if(pSlvInfo->dataValidCnt > 0)
            pSlvInfo->dataValidCnt --;
        
        if(rxSrcNode == COMMOBJ_SWITCHER)
        {
            pSlvInfo->slaveLinkOK       = 1;
        }

        /* 判断从机回复的帧序号
         */
        if(incSeq == TRUE)
        {
            if((tEnmCommObjCmd)(rxPkgHeader.msgType) == OBJ_CMD_IOR)
            {
                if( pSlvInfo->sendSeq  == rxPkgHeader.seq )
                {
                    pSlvInfo->sendSeq ++;
                }
            }
        }

        /* 从机回复的命令字 BIT7 被设置为1
         *  */
        //rxPkgHeader.msgType &= ~CMD_DIR_BIT;
        switch((tEnmCommObjCmd)(rxPkgHeader.msgType))
        {
            /* 从机回复：读IO数据     */
            case OBJ_CMD_IOR:
                /* 保存从机的输入数据 */
                //pSlvInfo->slaveInput = *((tCmdReadIO_s *)(RxTxBufPtr->pRxBuf + sizeof(tPktHeader)));
                
                /* pSlvInfo 是指向 MONITOR 查询的对象，与回复数据的源无关
                 * 因此这里的数据是查询对象回复的 */
                memset(pSlvInfo->slaveInput.rdBuf, 0, sizeof(pSlvInfo->slaveInput.rdBuf));

                if(rxDataLen >= sizeof(pSlvInfo->slaveInput))
                {
                    pSlvInfo->RdDataLen = sizeof(pSlvInfo->slaveInput);
                }
                else
                {
                    pSlvInfo->RdDataLen = rxDataLen;
                }
                memcpy(pSlvInfo->slaveInput.rdBuf, RxTxBufPtr->pRxBuf + sizeof(tPktHeader), pSlvInfo->RdDataLen);

                #ifndef CMD_READ_RELAY_LIFE
                /* LOG_PIC32_B 的程序: 继电器寿命信息附加在 IO 数据后面 */
                if((dstNode == COMMOBJ_LOG1) || (dstNode == COMMOBJ_LOG2))
                {
                    int c;
                    int8* psrc =RxTxBufPtr->pRxBuf + sizeof(tPktHeader) + sizeof(tReadLOG_IO);
                    int8* pdst = (int8*)getRelayRecordPtr(dstNode);
                    
                    for(c = 0; c<sizeof(tRelayRecord); c++)
                    {
                        *pdst++ = * psrc++;
                    }
                    
                    /* 每个继电器的信息(tRelayLife)包括：4个字节信息 + 4字节寿命
                     * 只把继电器寿命更新到发送给 MMS 的数据存储区
                     *  */
                    psrc = (int8u*)getRelayRecordPtr(dstNode) + sizeof(tRelayRecHeader);
                    if((dstNode == COMMOBJ_LOG1))
                        pdst = (int8u*)&uniDcuData.sortBuf.aRelayLifes[0];
                    else
                        pdst = (int8u*)&uniDcuData.sortBuf.aRelayLifes[1];
                    for(c=0; c<RELAY_NUM; c++)
                    {
                        psrc += 4;
                        *pdst++ = *psrc++;
                        *pdst++ = *psrc++;
                        *pdst++ = *psrc++;
                        *pdst++ = *psrc++;
                    }
                }
                #endif
                
                pSlvInfo->gotIOData  = 1;
                break;
                
            #ifdef CMD_READ_RELAY_LIFE
            /* 从机回复：继电器寿命信息 */
            case OBJ_CMD_RELAY_LIFE:
            {
                if((dstNode == COMMOBJ_LOG1) || (dstNode == COMMOBJ_LOG2))
                {
                    memcpy((int8*)getRelayRecordPtr(dstNode), RxTxBufPtr->pRxBuf + sizeof(tPktHeader) + sizeof(tReadLOG_IO), sizeof(tRelayRecord));
                }
            }
            break;
            #endif

            /* 从机回复：软件版本信息 */       
            case OBJ_CMD_DEVINFO:
            case OBJ_CMD_RPLYINFO_OK:
                /* 收到从机的软件版本信息 */
                if(rxDataLen >  sizeof (pSlvInfo->slaveDevInfo))
                {
                    rxDataLen = sizeof(pSlvInfo->slaveDevInfo);
                }
                
                memcpy((int8u *)(pSlvInfo->slaveDevInfo), RxTxBufPtr->pRxBuf + sizeof(rxPkgHeader), rxDataLen);
                pSlvInfo->VersionFlag = 1;

                /* 如果主机发送：读IO数据，或 读配置信息，却收到回复版本，
                 * 说明从机没有配置，需要对从机进行配置 */
                /* 从机未配置 */
                //pSlvInfo->ConfigFlag = CFG_INVALID;
                break;

            /* 从机回复：软件升级状态   
             * 回复数据的格式:
             *     0   0x5A
             *     1   0x5A     
             *     2   升级状态(u8UpdateState)
             *     3   分段号 L(如果升级完成，段号是最后一段，否则是上一个段)
             *     4   分段号 H
             *  */    
            case OBJ_CMD_UPDATESTS:
            case OBJ_CMD_UPDATE_RPLY:
                pData = RxTxBufPtr->pRxBuf + sizeof(rxPkgHeader);
                pData += 2;

                pSlvInfo->slaveReplyUpdate  = TRUE;
                pSlvInfo->slaveUpdateSts    = *pData++;
                pSlvInfo->slaveUpdateSec    = ((int16u)(*(pData + 1)) << 8) + (int16u)(*(pData + 0));
                break;
                
            /* 测试数据 */
            case OBJ_CMD_TESTDATA:
            {
                if((rxSrcNode == COMMOBJ_LOG1) || (rxSrcNode == COMMOBJ_LOG2))
                {
                    /* 先是 IO 数据
                     * 数据1字节 + IO数据信息
                     *  */
                    int8u *pSrc = RxTxBufPtr->pRxBuf + sizeof(tPktHeader);
                    int8u IODataLen = *pSrc;
                    
                    pSrc++;
                    if(IODataLen >= sizeof(pSlvInfo->slaveInput))
                    {
                        pSlvInfo->RdDataLen = sizeof(pSlvInfo->slaveInput);
                    }
                    else
                    {
                        pSlvInfo->RdDataLen = IODataLen;
                    }
                    memcpy(pSlvInfo->slaveInput.rdBuf, pSrc, pSlvInfo->RdDataLen);

                    /* 然后是控制逻辑的数据
                     *  */
                    pSrc += pSlvInfo->RdDataLen;
                    if(rxSrcNode == COMMOBJ_LOG1)
                    {
                        memcpy(LogicTestData[0], pSrc, rxDataLen);
                        LogicTestDataLen[0] = rxDataLen;
                    }
                    else
                    {
                        memcpy(LogicTestData[1], pSrc, rxDataLen);
                        LogicTestDataLen[1] = rxDataLen;
                    }
                }
            }
            break;
                
            default:
                break;
        }
    }while(0);

    return rxMsgSts;
}


/* =============================================================================
 * 轮询过程，处理来自从机的消息
 * 返回
 *  TRUE  接收从机信息正确
 *  FALSE 没有接收到或接收到错误的信息
 */
PRIVATE BOOL IPS_PollingProcRxMsg(void)
{
    return IPS_ProcRxFrame(&uartIPS, PollingIPS.sendPkgHeader.dstNode, TRUE);
}

/* =============================================================================
 * */
PRIVATE BOOL IPS_UpdateProcRxMsg(void)
{
    return IPS_ProcRxFrame(&uartIPS, MMS_OP_DST_ID, FALSE);
}


/* =========================================================
 * IPS模块相关控制量及MCU串口初始化
 */
PUBLIC void initIPSModule(void)
{
    /* 串口初始化 */
    initIPSComm(&uartIPS);
    
    pUartIPS = getIPSUartCommPtr();
    
    /* 存储器初始化 */
    memset((int8u*)&PollingIPS, 0, sizeof(PollingIPS));
    
    PollingIPS.pollingCom           = pUartIPS;
    PollingIPS.pollingObjList       = (int8u *)pollingIPSList;
    PollingIPS.pollingObjNum        = sizeof(pollingIPSList);
    PollingIPS.fpCbProcRxPkg        = IPS_PollingProcRxMsg;
    
    PollingIPS.PollingTurnTime      = IPS_INTURN_TIME;  /* 轮询时间，只在轮询有效 */
    PollingIPS.PollingTxOvTime      = IPS_TX_OVERTIME;  /* 发送超时，只在轮询有效 */
    PollingIPS.ErrMaxNumOffLine     = IPS_OFFLINE_NUM;  /* 计数器，用于判断离线   */
    PollingIPS.setWaitTXTime        = 0;                /* 切换至发送的延时时间   */
    
    /* 初始化在线升级的回调函数 */
    InitUartUpdateCallBack(IPSUpdate_TxPkg2IPS, IPSUpdate_TxBusy, IPSUpdate_QuerySlave, IPSObj_chkOnline, IPSUpdate_GetUpdateInfo);
    
}


/* =============================================================================
 * 与 IBP PSL SIG 扩展板的通信功能
 * 包括在线升级处理
 * 
 * 需要在主程序中循环调用
 * =============================================================================
 */
PUBLIC void processIPSComm(void)
{
    tSYSTICK tickNow;
    
    /* 判断与从机的通信连接 */
    tickNow = getSysTick();
    
    if(MMS_UPDATE_STATE() != MST_STATE_IDLE)
    {
        /* 在线升级 IBP PSL SIG
         * 这里只循环接收信息，发送升级数据，在 MMS_Command 完成
         */
        if((MMS_OP_DST_ID == COMMOBJ_SWITCHER) || (MMS_OP_DST_ID == COMMOBJ_LOG1) || (MMS_OP_DST_ID == COMMOBJ_LOG2))
        {
            Uni_ProcRecvedFrame(&uartIPS, &IPS_UpdateProcRxMsg);
        }
    }
    else
    {
        /* 轮询从机 
         */
        PollingIPS.fpCbProcRxPkg = IPS_PollingProcRxMsg;
        Uni_PollingSlaves((void *)&PollingIPS);
        
        /* 定时清除离线的扩展板的版本信息，再次上线后需要读取版本信息 
         * 主要是考虑扩展板在线升级后会自动重启，MON 自动再次重读
         */
        static tSYSTICK tickReadVer;
        tSlaveInfo* pSlvInfo;
        if((tSYSTICK)(tickNow - tickReadVer) > 100)
        {
            tickReadVer = tickNow;
        
            if(IPSObj_chkOnline(COMMOBJ_SWITCHER) == FALSE)
            {
                pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_SWITCHER);
                pSlvInfo->VersionFlag = 0;
            }
            
            if(IPSObj_chkOnline(COMMOBJ_LOG1) == FALSE)
            {
                pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_LOG1);
                pSlvInfo->VersionFlag = 0;
            }
            
            if(IPSObj_chkOnline(COMMOBJ_LOG2) == FALSE)
            {
                pSlvInfo = getSlaveInfoPtr((tEnmCommObj) COMMOBJ_LOG2);
                pSlvInfo->VersionFlag = 0;
            }
        }
        
        
        /* 青岛4：定时读取继电器的寿命记录
         *  */
        static tSYSTICK tickReadRelayRecord;
        if((tSYSTICK)(tickNow - tickReadRelayRecord) > 3000)
        {
            tickReadRelayRecord = tickNow;
            
            bReadRelayLife[0] = TRUE;
            bReadRelayLife[1] = TRUE;
            
        }
    }
    
    
    
    /* 判断串口链接状态及发送状态是否异常 
     * 与 IBP PSL SIG 任意一个通信成功，则本机 UART OK
     */
    if(uartChkLinkAndSendSts(COM_IPS) == TRUE)
    {
        /* 检测串口通信错误标志
         * 若出错，则重新初始化串口
         *  */
        if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART_IPS_ID)) )
        {
            INTClearFlag(INT_SOURCE_UART_ERROR(UART_IPS_ID));
            initIPSComm(&uartIPS);
        }
    }
}


