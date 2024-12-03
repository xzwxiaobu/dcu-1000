/*******************************************************************************
 * PIC24 ���������Ľӿ�
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
 * ���� PIC24���ж��Ƿ������� PIC24 �Ĵ�����Ľ�������֡
 *             ������
 * ���� 
 *  pUartCom        : ���ڿ��ƽṹ��ָ��
 *  dstNode         : ָ��������Ķ���
 *  incSeq          : TRUE = �������
 * ����
 *  TRUE ���յ�ָ�����������֡
 */
PUBLIC BOOL upPIC24_ChkP24Frame(void)
{
    int16u crc16;
    int16u rxDataLen;
    
    tPktHeader  *pHeader;
    tPktTail    *pTail;
    
    tUartBuf    *RxTxBufPtr;
    tUartComm   * pUartCom;
    
    /* ���ؽ�� */
    BOOL rxMsgSts = FALSE;
    
    /* ��ȡ���ڷ��Ϳ���ָ�� */
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

    /* ���յ�����֡������д��� */
    if(pUartCom->uartSts.gotFrame != 0)
    {
        do
        {
            pHeader = (tPktHeader *)(pUartCom->RxTxBuf.pRxBuf);

            rxDataLen = ((int16u)pHeader->dataLenH << 8) + (int16u)pHeader->dataLenL;

            pTail   = (tPktTail   *)(pUartCom->RxTxBuf.pRxBuf + rxDataLen + sizeof(tPktHeader));

            #if (FRAME_VERIFY_CRC16 == 1)
            /* У���������֡�Ƿ���ȷ 
             * CRC16 У��
             *  -2 �����㳤�Ȳ���������У����
             */
            crc16 = CalcCRC16_A001(pUartCom->RxTxBuf.pRxBuf,    rxDataLen + sizeof(tPktHeader) + sizeof(tPktTail) - 2, 0x00);
            if( ((int16u)(pTail->crc16L) + ((int16u)(pTail->crc16H) << 8)) != crc16)
            {
                /* У����󣬶��� */
                break;
            }

            #else
            /* У���������֡�Ƿ���ȷ 
             * ����� У��
             *  -1 �����㳤�Ȳ���������У����
             */
            crc16 = checksum(pUartCom->RxTxBuf.pRxBuf,    rxDataLen + sizeof(tPktHeader) + sizeof(tPktTail) - 1);
            if((int16u)(pTail->crcXOR) != crc16)
            {
                /* У����󣬶��� */
                break;
            }
            #endif

            /* ����������յ�����֡ 
             */
            upPIC24_ProcP24Reply(pHeader->msgType, (int8u*)(RxTxBufPtr->pRxBuf + sizeof(tPktHeader)), rxDataLen);
            
        }while(0);
        
        pUartCom->RxTxBuf.rxCnt    = 0;
        pUartCom->uartSts.gotFrame = 0;
    }
    
    return rxMsgSts;
}


/* =============================================================================
 * �����������ݰ�
 * ����
 *  msgType         : ��Ϣ����
 *  txDstNode       : ���Ͷ���
 *  txData          : ���������� 
 *  txDataLen       : ���������ݳ���
 * ���ط���״̬    
 *  TRUE ���ڳɹ�
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
        /* ��ȡ���ڷ��Ϳ���ָ�� */
        pPic24Comm = (tUartComm *)getPic24UartCommPtr();
        if(pPic24Comm == NULL)
        {
            break;
        }

        /* ����æ */
        if(ifPic24UartTxIsBusy() == TRUE)
        {
            break;
        }

        /* ���ڷ��ͻ��泤�Ȳ��� */
        if(txDataLen > pPic24Comm->RxTxBuf.txBufSize)
        {
            break;
        }

        /* ׼���������� 
         * ֱ�ӽ�����������д�봮��ָ���ķ��ͻ���
         */
        pTxPkg = pPic24Comm->RxTxBuf.pTxBuf;

        /* ��Ϣͷ */
        *pTxPkg ++= HEADER_DLE;                         /* header 1         */
        *pTxPkg ++= HEADER_STX;                         /* header 2         */
        *pTxPkg ++= 0;                                  /* ˳���           */
        *pTxPkg ++= COMMOBJ_MONITOR;                    /* ͨ������Դ�ڵ�   */
        *pTxPkg ++= txDstNode;                          /* ͨ������Ŀ��ڵ� */
        *pTxPkg ++= msgType;                            /* ͨ����������     */
        *pTxPkg ++= (int8u)(txDataLen >> 8);            /* ���ݳ��� H       */
        *pTxPkg ++= (int8u)(txDataLen);                 /* ���ݳ��� L       */

        /* ������� */
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

        txLen = pTxPkg - pPic24Comm->RxTxBuf.pTxBuf;                /* ������У������ݳ��� */

        #if (FRAME_VERIFY_CRC16 == 1)
        /* �������ݰ���У�� : CRC16
         */
        chkCode   = CalcCRC16_A001((int8u*)(pPic24Comm->RxTxBuf.pTxBuf), txLen, 0x00);
        *pTxPkg ++= chkCode;                        /* У����  L        */
        *pTxPkg ++= chkCode >> 8;                   /* У����  H        */
        #else
        /* �������ݰ���У�� : �����
         */
        chkCode   = checksum((int8u*)(pPic24Comm->RxTxBuf.pTxBuf), txLen);
        *pTxPkg ++= chkCode;                        /* У����  L        */
        #endif

        /* ���������ݳ��� */
        txLen = pTxPkg - pPic24Comm->RxTxBuf.pTxBuf;

        /* �������� */
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
 * ������������ PIC24������֪ͨ��Ӧ�ò㣬�Ա�Ӧ�ò����»�ȡоƬ�İ汾��Ϣ
 * ����
 *  ��������
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



