/*******************************************************************************
 * PIC24 在线升级的接口
 * 
 * 
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"

#include "IPSUart.h"
#include "sysTimer.h"

#include "P24UpdateDrv.h"

#undef  PRIVATE
#define PRIVATE


/* =============================================================================
 * 升级 PIC24，判断是否有来自 PIC24 的待处理的接收数据帧
 *             有则处理
 * 参数 
 *  pUartCom        : 串口控制结构体指针
 *  dstNode         : 指定待处理的对象
 *  incSeq          : TRUE = 序号自增
 * 返回
 *  TRUE 接收到指定对象的数据帧
 */
PUBLIC BOOL upPIC24_ChkP24Frame(void)
{
    int16u crc16;
    int16u rxDataLen;
    
    tPktHeader  *pHeader;
    tPktTail    *pTail;
    
    tUartBuf    *RxTxBufPtr;
    tUartComm   * pUartCom;
    
    /* 返回结果 */
    BOOL rxMsgSts = FALSE;
    
    /* 获取串口发送控制指针 */
    pUartCom = (tUartComm *)getPic24UartCommPtr();
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

            /* 立即处理接收的数据帧 
             */
            upPIC24_ProcP24Reply(pHeader->msgType, (int8u*)(RxTxBufPtr->pRxBuf + sizeof(tPktHeader)), rxDataLen);
            
        }while(0);
        
        pUartCom->RxTxBuf.rxCnt    = 0;
        pUartCom->uartSts.gotFrame = 0;
    }
    
    return rxMsgSts;
}


/* =============================================================================
 * 创建发送数据包
 * 输入
 *  msgType         : 消息类型
 *  txDstNode       : 发送对象
 *  txData          : 待发送数据 
 *  txDataLen       : 待发送数据长度
 * 返回发送状态    
 *  TRUE 等于成功
 */
PUBLIC BOOL upPIC24_SendToPic24(int8u msgType, int8u txDstNode, int8u * txData, int16u txDataLen)
{
    int i;
    int8u *pTxPkg;
    int16u chkCode;
    int16u txLen;
    
    tUartComm * pPic24Comm;
    BOOL    ret = FALSE;
    
    do
    {
        /* 获取串口发送控制指针 */
        pPic24Comm = (tUartComm *)getPic24UartCommPtr();
        if(pPic24Comm == NULL)
        {
            break;
        }

        /* 串口忙 */
        if(ifPic24UartTxIsBusy() == TRUE)
        {
            break;
        }

        /* 串口发送缓存长度不足 */
        if(txDataLen > pPic24Comm->RxTxBuf.txBufSize)
        {
            break;
        }

        /* 准备发送数据 
         * 直接将待发送数据写入串口指定的发送缓存
         */
        pTxPkg = pPic24Comm->RxTxBuf.pTxBuf;

        /* 信息头 */
        *pTxPkg ++= HEADER_DLE;                         /* header 1         */
        *pTxPkg ++= HEADER_STX;                         /* header 2         */
        *pTxPkg ++= 0;                                  /* 顺序号           */
        *pTxPkg ++= COMMOBJ_MONITOR;                    /* 通信数据源节点   */
        *pTxPkg ++= txDstNode;                          /* 通信数据目标节点 */
        *pTxPkg ++= msgType;                            /* 通信数据类型     */
        *pTxPkg ++= (int8u)(txDataLen >> 8);            /* 数据长度 H       */
        *pTxPkg ++= (int8u)(txDataLen);                 /* 数据长度 L       */

        /* 填充数据 */
        int8u* data = txData;
        if(txDataLen > 0)
        {
            data = txData;
            for(i=0; i<txDataLen; i++)
            {
                *pTxPkg ++ = *data++;
            }
        }

        *pTxPkg ++= TAIL_DLE;                       /* tail             */
        *pTxPkg ++= TAIL_ETX;                       /* tail             */

        txLen = pTxPkg - pPic24Comm->RxTxBuf.pTxBuf;                /* 待计算校验的数据长度 */

        #if (FRAME_VERIFY_CRC16 == 1)
        /* 计算数据包的校验 : CRC16
         */
        chkCode   = CalcCRC16_A001((int8u*)(pPic24Comm->RxTxBuf.pTxBuf), txLen, 0x00);
        *pTxPkg ++= chkCode;                        /* 校验码  L        */
        *pTxPkg ++= chkCode >> 8;                   /* 校验码  H        */
        #else
        /* 计算数据包的校验 : 异或码
         */
        chkCode   = checksum((int8u*)(pPic24Comm->RxTxBuf.pTxBuf), txLen);
        *pTxPkg ++= chkCode;                        /* 校验码  L        */
        #endif

        /* 待发送数据长度 */
        txLen = pTxPkg - pPic24Comm->RxTxBuf.pTxBuf;

        /* 启动发送 */
        if(uartStartSend(pPic24Comm->usrComNo, pPic24Comm->RxTxBuf.pTxBuf, txLen) == COM_TX_OK)
        {
            ret = TRUE;
        }

        #if (SHOW_PIC24_BY_UART_IPS == 1)
        if(!ifIPSUartTxIsBusy())
        {
            uartStartSend(getIPSUartCommPtr()->usrComNo, pPic24Comm->RxTxBuf.pTxBuf, txLen);
        }
        #endif
    }while(0);
    
    return ret;
}


/* =============================================================================
 * 升级程序重启 PIC24，发送通知给应用层，以便应用层重新获取芯片的版本信息
 * 输入
 *  重启对象
 * */
PUBLIC BOOL upPIC24_ResetChip(tEnmCommObj updatePic24Obj)
{
    tSlaveInfo *pSlvInfo;
    if((updatePic24Obj == COMMOBJ_IORW_A) || (updatePic24Obj == COMMOBJ_IORW_B))
    {
        pSlvInfo = getSlaveInfoPtr(updatePic24Obj);
        pSlvInfo->VersionFlag = 0;
    }
}



