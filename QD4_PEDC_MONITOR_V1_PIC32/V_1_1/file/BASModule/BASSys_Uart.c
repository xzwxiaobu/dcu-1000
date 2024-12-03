#include "..\Include.h"

#include "ModbusDef.h"
#include "BASSys_Uart.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif
PRIVATE tUartComm uartBASSys;                               /* 串口通信控制 */

PRIVATE int8u RxBASSysDataBuf[UART_BASSYS_RXBUFSIZE];       /* 串口通信控制-接收缓存 */
PRIVATE int8u TxBASSysDataBuf[UART_BASSYS_TXBUFSIZE];       /* 串口通信控制-发送缓存 */

/* =============================================================================*/
/* 获取BAS系统在线状态 ：FALSE 表示离线，TRUE 表示在线 */
PUBLIC BOOL getBASSysOnlineSts(void)
{
    return (BOOL)(uartBASSys.uartSts.linkState);
}

/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
PUBLIC BOOL ifBASSysUartTxIsBusy(void)
{
    return (BOOL)(uartBASSys.uartSts.isSending);
}

/* =============================================================================*/
PRIVATE void uartBAS_DE_Dis(void)
{
    DE_BASSYS_DISABLE();
}

PRIVATE void uartBAS_DE_En(void)
{
    DE_BASSYS_ENABLE();
}

PRIVATE void uartBAS_RE_Dis(void)
{
    RE_BASSYS_DISABLE();
}

PRIVATE void uartBAS_RE_En(void)
{
    RE_BASSYS_ENABLE();
}

/* =============================================================================
 * 通信中断处理函数 : 与信号系统通信
 * 输入：
 *   module_id  : 串口ID
 *   data       : 一个字节数据
 */
PRIVATE void BASSys_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    tSYSTICK    nowTick;
    tUartComm * pCom = &uartBASSys;
    
    tModbusFixHeader * mb_Header;
    tModbusWrHeader  * wrHeader;
    BOOL rxedFrame = FALSE;
    
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
                pCom->uartSts.receiving = 0;
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

    pCom->uartSts.receiving = 1;

    /* save the received data */
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = data;
    pCom->RxTxBuf.rxCnt ++;

    if(pCom->RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
    {
        mb_Header = (tModbusFixHeader *)(pCom->RxTxBuf.pRxBuf);
        
        switch(mb_Header->funID)
        {
            /* 读寄存器或读线圈 */
            case fun_03_ReadHoldingRegisters:
            case fun_04_ReadinputRegisters:
                //if(pCom->RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail))
                {
                    rxedFrame = TRUE;
                    pCom->uartSts.gotFrame = 1;
                }
                break;

            case fun_16_WriteMultipleRegisters:
                wrHeader = (tModbusWrHeader *)(pCom->RxTxBuf.pRxBuf);

                //len = pCom->RxTxBuf.pRxBuf[mb_ByteCount] + mb_fun0x10_request_fix_len;
                //if((pCom->RxTxBuf.rxCnt >= len) && (pCom->RxTxBuf.pRxBuf[mb_ByteCount] <= mb_fun0x10_request_dataBUF_LEN))
                if(pCom->RxTxBuf.rxCnt >= (wrHeader->dataLen + sizeof(tModbusWrHeader) + sizeof(tModbusTail)))
                {
                    rxedFrame = TRUE;
                }
                break;

            default:
                pCom->RxTxBuf.rxCnt = 0;
                break;		
        }
    }
    else
    {
        /* MODBUS RTU 协议
         * 1  : 设备地址
         * 1  : 功能码 
         * 2  : 访问寄存器地址
         * 2  : 访问寄存器长度
         * 1  : 字节数
         * N  : 数据
         * 2  : CRC 校验码(低字节，高字节)
         */
        switch(pCom->RxTxBuf.rxCnt)
        {
            case 1:     /* 设备地址 */
                if(data != LOCAL_DEV_ADDR)
                {
                    pCom->RxTxBuf.rxCnt = 0;
                }
                break;
                
            case 2:     /* 功能码 */
                break;
        }
    }
    
    if(rxedFrame)
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
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART 
 * 
 *******************************************************************************
 *******************************************************************************/
PRIVATE void initBASSysComm(void)
{
    DeInitUartCom(COM_BASSYS, &uartBASSys);
    
    uartBASSys.RxTxBuf.pRxBuf       = RxBASSysDataBuf;          /* 接收数据区指针               */
    uartBASSys.RxTxBuf.rxBufSize    = sizeof(RxBASSysDataBuf);  /* 接收数据区大小               */
    uartBASSys.RxTxBuf.rxCnt        = 0;                        /* 已接收的数据长度             */
  
    uartBASSys.RxTxBuf.pTxBuf       = TxBASSysDataBuf;          /* 发送数据区指针               */
    uartBASSys.RxTxBuf.txBufSize    = sizeof(TxBASSysDataBuf);  /* 发送数据区大小               */
    uartBASSys.RxTxBuf.txCnt        = 0;                        /* 已发送数据的长度             */
    uartBASSys.RxTxBuf.txDataLen    = 0;                        /* 待发送数据的长度，0表示没有  */
      
    uartBASSys.funRx                = &BASSys_UartReceiveData;  /* 串口中断接收一个字节后调用           */
    uartBASSys.funTx                = NULL;                     /* 发送数据结束后中断调用, null表示没用 */
          
    uartBASSys.RS485_4Wire          = RS485_BASSYS_4W;          /* TRUE = 全双工RS485 */
    uartBASSys.fpDE_Dis             = uartBAS_DE_Dis;
    uartBASSys.fpDE_En              = uartBAS_DE_En;
    uartBASSys.fpRE_Dis             = uartBAS_RE_Dis;
    uartBASSys.fpRE_En              = uartBAS_RE_En;
      
    /* 帧间隔时间：对 MODBUS 协议，帧之间的间隔不得小于 3.5个字节传输时间
     * 
     * 若按 9600bps，则每个字节传输时间约 1.05ms，即帧间隔时间不得低于约 3.7ms
     * 这里禁止 禁止底层帧间隔时间 超时后重新接收计数，用于判断 MODBUS 协议数据帧结束
     */
    uartBASSys.frameOverTime        = 0;            /* ms，禁止底层帧间隔时间，若此时间内没有数据认为帧结束 */
    uartBASSys.sendOverTime         = 200;          /* ms，设置发送数据超时时间长度                 */
    uartBASSys.abortFrameTime       = 500;          /* ms, 若超过指定的时间没处理，则丢弃该帧数据   */
    uartBASSys.linkFailTime         = BASSYS_OFFLIN_TIME;         /* 断线时间 */

    uartCOM_Init(COM_BASSYS, UART_BASSYS_ID, &uartBASSys, UART_BASSYS_BR, UART_BASSYS_IPL);
}


/* =========================================================
 * 判断是否有校验正确的接收数据帧
 * 若有，则由应用层处理
 */
PUBLIC BOOL procBASSysRxFrame(cbAppBASSysProcFrame BASSysApp)
{
    BOOL sts = FALSE;
    tUartComm * pCom = &uartBASSys;
    unsigned short CRC;
    unsigned short rxCRC;
    tModbusTail * pTail;
    
    if(pCom->uartSts.gotFrame)
    {
        CRC = CRC16_Modbus((int8u *)(pCom->RxTxBuf.pRxBuf), pCom->RxTxBuf.rxCnt - 2);
        
        pTail = (tModbusTail * )(pCom->RxTxBuf.pRxBuf + pCom->RxTxBuf.rxCnt - 2);
        rxCRC = (unsigned short)(pTail->crcL) + ((unsigned short)(pTail->crcH) << 8);
        if(CRC == rxCRC)
        {
            /* 与对方通信正常 */
            pCom->uartSts.linkState = 1;
            
            /* 回调，应用层处理接收数据 */
            BASSysApp(pCom->RxTxBuf.pRxBuf, pCom->RxTxBuf.rxCnt);
            
            sts = TRUE;
        }	
        
        pCom->uartSts.receiving = 0;
        pCom->RxTxBuf.rxCnt     = 0;
        pCom->uartSts.gotFrame  = 0;
    }
    
    return sts;
}


/* =========================================================
 * 模块初始化
 */
PUBLIC void initBASSysModule(void)
{
    /* 串口初始化 */
    uartBAS_DE_Dis();
    uartBAS_RE_Dis();
    
    initBASSysComm();
    
    uartBAS_RE_En();
}

/* =========================================================
 * 启动向 BAS系统 发送数据
 */
PUBLIC BOOL  sendDataFrameToBASSys(int8u *pTxData, int TxLen)
{
    BOOL ret = FALSE;
    
    if(uartStartSend(uartBASSys.usrComNo, pTxData, TxLen) == COM_TX_OK)
    {
        ret = TRUE;
    }
}

/* =========================================================
 * 通过定时器判断BAS系统的发送数据帧是否结束
 */
PUBLIC BOOL chkBASSysFinishSend(void)
{
    tSYSTICK nowTick;
    
    nowTick = getSysTick();
    
    if(uartBASSys.uartSts.receiving != 0)
    {
        /* 正在接收 
         * 若接收的最后一个字节过了指定时间后，认为接收完成
         * 此时判断接收的长度是否符合 MODBUS 最小帧的长度
         * 有，则需要处理
         */
        if((tSYSTICK)(nowTick - uartBASSys.lastByteTick) >= MODBUS_FRAME_FINISH_TIME)
        {
            if(uartBASSys.uartSts.gotFrame == 0)
            {
                if(uartBASSys.RxTxBuf.rxCnt >= (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
                {
                    /* 接收数据长度满足最小要求，需要处理 */
                    uartBASSys.uartSts.gotFrame  = 1;
                }
                else
                {
                    /* 接收数据长度不满足最小要求，重新接收 */
                    uartBASSys.RxTxBuf.rxCnt = 0;
                }
            }
            
            uartBASSys.uartSts.receiving = 0;
        }
    }
}


