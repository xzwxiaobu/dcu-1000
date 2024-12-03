
#include "..\Include.h"
#include "glbCan.h"
#include "CanDrv.h"
#include "IOFunct.h"


#include "uartDrv.h"
#include "IPSUart.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

#define DE_PC_DISABLE()     mPORTFClearBits(BIT_3)
#define DE_PC_ENABLE()      mPORTFSetBits(BIT_3)
#define DE_MMS_DISABLE()    DE_PC_DISABLE()
#define DE_MMS_ENABLE()     DE_PC_ENABLE()

PRIVATE tUartComm uartMMS;                      /* 串口通信控制 */
PRIVATE int8u RxMMSDataBuf[MMS_RXBUF_SIZE];     /* 串口通信控制-接收缓存 */
PRIVATE int8u TxMMSDataBuf[MMS_TXBUF_SIZE];     /* 串口通信控制-发送缓存 */

/* 获取 MMS 通信状态 */
PUBLIC int8u getUartMMSLinkSts(void)
{
    return uartMMS.uartSts.linkState;
}

/* 获取 MMS 通信 Uart 发送缓冲区指针 */
PUBLIC int8u* getMMSTxBufPtr(void)
{
    return uartMMS.RxTxBuf.pTxBuf;
}

/* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
PUBLIC BOOL ifMMSUartTxIsBusy(void)
{
    return (BOOL)(uartMMS.uartSts.isSending);
}

/* 供升级调用的函数 
 * TRUE 表示串口发送结束
 */
PUBLIC BOOL chkFinishTxToMMS(void)
{
    return (BOOL)(!ifMMSUartTxIsBusy());
}


/* 发送数据到MMS
 *  pTxData     : 待发送数据缓存指针
 *  TxDataLen   : 待发送数据的长度
 *  */
PUBLIC tEnmComTxSts sendPackageToMMS(int8u* pTxData, int TxDataLen)
{
    tEnmComTxSts sts;
    
    sts = uartStartSend(COM_MMS, pTxData, TxDataLen);
    
    return sts;
}

/*******************************************************************************
 *******************************************************************************
 * 
 * Initialize UART for MMS
 * 
 *******************************************************************************
 *******************************************************************************/
PRIVATE void uartMMS_DE_Dis(void)
{
    DE_MMS_DISABLE();
}

PRIVATE void uartMMS_DE_En(void)
{
    DE_MMS_ENABLE();
}

/* 串口接收到数据（一个字节）后的处理程序
 *  */
PRIVATE void mms_UartReceiveData(int8u module_id, int8u rxData)
{
    int i;
    int16u j;
    
    (void)module_id;
    
    tSYSTICK nowTick;
    tUartComm *pCom = &uartMMS;
    
    int8u * MMSRxBufPtr = uartMMS.RxTxBuf.pRxBuf;
    
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
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = rxData;

    if(pCom->RxTxBuf.rxCnt >= 10)
    {
        j = (int16u)MMSRxBufPtr[7] + (((int16u) MMSRxBufPtr[6]) << 8);

        if((j + 10) == pCom->RxTxBuf.rxCnt)
        {
            if((MMSRxBufPtr[j + 8] != UARTDLE) || (MMSRxBufPtr[j + 9] != UARTETX))
            {
                /* 尾标记错误，重新接收 */
                pCom->RxTxBuf.rxCnt = 1;
                MMSRxBufPtr[0] = rxData;
                return;
            }
            
            /* 得到完整的数据帧
             * 记录当前的时间点 
             * 这里不做数据校验 ( _checksum )，避免中断占用 CPU 时间过长
             *  */
            pCom->RxTxBuf.rxCnt ++;             //最后一个字节，计入总数
            pCom->uartSts.gotFrame  = 1;        //bFun_Uart2_Receive_Handle = 1;
            pCom->gotFrameTick      = nowTick;  //MMS_PEDC_UART_OverTime = 0 ;    //UART 通讯超时错
            return;
        }
        pCom->RxTxBuf.rxCnt++;
    }
    else
    {
        switch (pCom->RxTxBuf.rxCnt)
        {
            case 0:
                if (MMSRxBufPtr[0] != UARTDLE)
                {
                    pCom->RxTxBuf.rxCnt = 0;
                    return;
                }
                break;

            case 1:
                if (MMSRxBufPtr[1] != UARTSTX)
                {
                    pCom->RxTxBuf.rxCnt = 0;
                    MMSRxBufPtr[0] = rxData;
                    return;
                }
                break;

            case 2:
                /*
                if( (MMSRxBufPtr[2] != 0x0) && (MMSRxBufPtr[2] != 0x1) )   // == 0 上行线标志  ，== 1 下行线标志
                {
                    pCom->RxTxBuf.rxCnt = 1;
                    MMSRxBufPtr[0] = rxData;

                    return;
                }
                */
                break;

            case 3:
                if (MMSRxBufPtr[3] != 0x0)
                {
                    pCom->RxTxBuf.rxCnt = 1;
                    MMSRxBufPtr[0] = rxData;
                    return;
                }
                break;

            /* 通信对象：PEDC=200, , DCU 
             * 0xFF = 通用对象
             * 200  = PEDC-MON
             * 1~64 = DCU
             * 0x80 = 批量升级 DCU
             * PSL IBP SIG 
             *  */
            case 4:
                if((MMSRxBufPtr[4] == 0xFF) || (MMSRxBufPtr[4] == 200) || 
                  ((MMSRxBufPtr[4] <= 64)   && (MMSRxBufPtr[4] >= 1  ) || (MMSRxBufPtr[4] == UP_PEDC_MORE_DCUS)) || 
                  ((MMSRxBufPtr[4] >= COMMOBJ_FIRST) && (MMSRxBufPtr[4] < (COMMOBJ_FIRST + COMMOBJ_TOTAL)))
                )
                {
                    break;
                }
                else
                {
                    pCom->RxTxBuf.rxCnt = 1;
                    MMSRxBufPtr[0] = rxData;
                    return;
                }
                break;

            default :
                break;
        }
        pCom->RxTxBuf.rxCnt++;
    }
}

/* =============================================================================
 * 初始化 MMS 通信模块
 */
PUBLIC void initMMSModule(void)
{
    DeInitUartCom(COM_MMS, &uartMMS);
    
    uartMMS.RxTxBuf.pRxBuf      = RxMMSDataBuf;         /* 接收数据区指针               */
    uartMMS.RxTxBuf.rxBufSize   = sizeof(RxMMSDataBuf); /* 接收数据区大小               */
    uartMMS.RxTxBuf.rxCnt       = 0;                    /* 已接收的数据长度             */

    uartMMS.RxTxBuf.pTxBuf      = TxMMSDataBuf;         /* 发送数据区指针               */
    uartMMS.RxTxBuf.txBufSize   = sizeof(TxMMSDataBuf); /* 发送数据区大小               */
    uartMMS.RxTxBuf.txCnt       = 0;                    /* 已发送数据的长度             */
    uartMMS.RxTxBuf.txDataLen   = 0;                    /* 待发送数据的长度，0表示没有  */
    
    uartMMS.funRx               = &mms_UartReceiveData; /* 串口中断接收一个字节后调用           */
    uartMMS.funTx               = NULL;                 /* 发送数据结束后中断调用, null表示没用 */
        
    uartMMS.RS485_4Wire         = TRUE;                 /* 全双工 485  */
    uartMMS.fpDE_Dis            = &uartMMS_DE_Dis;
    uartMMS.fpDE_En             = &uartMMS_DE_En;
    uartMMS.fpRE_Dis            = NULL;
    uartMMS.fpRE_En             = NULL;
    
    uartMMS.frameOverTime       = 5;            /* ms，帧间隔时间，若此时间内没有数据认为帧结束 */
    uartMMS.sendOverTime        = 300;          /* ms，设置发送数据超时时间长度                 */
    uartMMS.abortFrameTime      = 2000;         /* ms, 若超过指定的时间没处理，则丢弃该帧数据   */
    uartMMS.linkFailTime        = cMMS_PEDC_UART_OverTime; /* MMS 通信断开时间设置 */

    uartCOM_Init(COM_MMS, UART_MMS_ID, &uartMMS, UART_MMS_BR, UART_MMS_IPL);
}

/* =============================================================================
 * 处理接收的 MMS 数据帧
 * 循环调用
 * 参数：
 *  appMMS_ProcRxPkg : APP的回调处理函数，当有数据包接收后调用
 * 
 *  */
PUBLIC BOOL MMS_ProcRxFunct(cbMMS_AppProcRxPkg appMMS_ProcRxPkg)
{
    BOOL ret;
    int16u dataLen;
    int8u * MMSRxBufPtr = uartMMS.RxTxBuf.pRxBuf;

    ret = FALSE;
    if(uartMMS.uartSts.errFrame)
    {
        //do nothing ?
        uartMMS.uartSts.errFrame = 0;
    }

    /* 是否收到完整的数据帧 */
    if(uartMMS.uartSts.gotFrame)
    {
        dataLen = (int16u)MMSRxBufPtr[7] + (((int16u) MMSRxBufPtr[6]) << 8);
        
        /* 数据校验 */
        if(MMSRxBufPtr[dataLen + 10] != _checksum(&(MMSRxBufPtr[0]), dataLen+10))
        {
            uartMMS.uartSts.errFrame    = 1;
            uartMMS.RxTxBuf.rxCnt       = 0;
            uartMMS.uartSts.gotFrame    = 0;
            return;
        }
        
        /* 与 MMS 串口通信状态OK */
        uartMMS.uartSts.linkState   = 1;
        uartMMS.uartSts.errFrame    = 0;
        
        ret = appMMS_ProcRxPkg((int8u*)uartMMS.RxTxBuf.pRxBuf, uartMMS.RxTxBuf.rxCnt);
        
        uartMMS.RxTxBuf.rxCnt    = 0;
        uartMMS.uartSts.gotFrame = 0;
    }
    
    return ret;
}
