/*******************************************************************************
 * ����ͨ�� ģ��
 * -> ��������ô��ڳ�ʼ��      initIPSComm 
 * -> ��ѭ�����ô���ͨ�Ŵ���    processIPSComm 
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

/* �̵�������������Ϣ
 * ��¼�Ӽ̵�����ʼʹ�ú�Ķ������� */
PRIVATE tRelayRecord    relayRecord[2];

/* ��Ҫ��ʱ��ȡ �߼����ư�洢 �ļ̵����������� */
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
 * ���ر���
 */
PRIVATE int8u LogicTestData[2][100];
PRIVATE int16u LogicTestDataLen[2] = {0, 0};
PUBLIC int8u* getLogicTestData(int16u * len)
{
    if(len != NULL)
    {
        if(bLOG_WorkLOG2 == 0)          /* ��1 */
        {
            *len = LogicTestDataLen[0];
            return LogicTestData[0];
        }
        else                            /* ��2 */
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

/* ��ѯ�Ķ����б������ 
 */
PRIVATE const int8u pollingIPSList[] = 
{
    COMMOBJ_LOG1,           /* LOG ��1 */
    COMMOBJ_LOG2,           /* LOG ��2 */
    COMMOBJ_SWITCHER,       /* ��ȫ��·�л��� */
};

/* ��ѯ������� */
PRIVATE tPollingTask PollingIPS;

/* ָ�򴮿ڿ��Ʊ��� */
PRIVATE tUartComm uartIPS;                              /* ����ͨ�ſ��� */
PRIVATE tUartComm * pUartIPS;
    PUBLIC tUartComm * getIPSUartCommPtr(void)
    {
        return &uartIPS;
    }
    
/* =============================================================================*/
//���ڷ��͵����ݰ����ܣ�TRUE ��ʾ�������ݰ�
PRIVATE BOOL txIPSUpdatePkgFlag;
    PUBLIC void clrIPSUpdatePkgFlag(void)
    {
        txIPSUpdatePkgFlag = FALSE;
    }


/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
PUBLIC BOOL ifIPSUartTxIsBusy(void)
{
    return (BOOL)(uartIPS.uartSts.isSending);
}

/* Uart ��ѯ�ⲿPCB������״̬ 
 * ���� FALSE ��ʾ�����ѶϿ���TRUE ��ʾ��������
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

/* ���ڽ��յ�����֡�Ĵ������
 *  */
PRIVATE BOOL IPS_PollingProcRxMsg(void);

/* =========================================================
 * ͨ���жϴ����� : ���ⲿPCB��ͨ��
 * ���룺
 *   module_id  : ����ID
 *   data       : һ���ֽ�����
 */
PUBLIC void IPS_UartReceiveData(int8u module_id, int8u data)
{
    (void)module_id;
    
    Uni_UartReceiveData(PollingIPS.pollingCom, data);
}

/* =============================================================================
 * 
 *                  �ⲿPCB�����������Ĺ��ܣ�ͨ�Žӿں���
 * 
 * =============================================================================
 */

/*
 * �����������ݰ�
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
      
/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ 
 * ������û�з����������ݰ������Ƿ����������ݰ����򷵻� TRUE
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

/* �������ײ���ã���ѯ IBP PSL SIG ����״̬ 
 *  id    : ��ѯ����� ID
 *  secNo : �����κ�
 * ���أ�
 *  TRUE  ������ѯOK
 *  FALSE ������ѯʧ�ܣ����ڷ���æ��
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
    *pTxPkg ++= pSlvInfo->sendSeq;                  /* ˳���           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* ͨ������Դ�ڵ�   */
    *pTxPkg ++= id;                                 /* ͨ������Ŀ��ڵ� */
    *pTxPkg ++= OBJ_CMD_UPDATESTS;                  /* ͨ����������     */
    
    *pTxPkg ++= 0;                                  /* ���ݳ��� H       */
    *pTxPkg ++= 2;                                  /* ���ݳ��� L       */    
    *pTxPkg ++= (int8u)(secNo >> 8);                /* �����κ� H       */
    *pTxPkg ++= (int8u)(secNo);                     /* �����κ� L       */
    
    *pTxPkg ++= TAIL_DLE;                           /* tail             */
    *pTxPkg ++= TAIL_ETX;                           /* tail             */
    
    queryPkgLen = pTxPkg - queryPkgBuf;             /* ������У������ݳ��� */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* �������ݰ���У�� : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), sendPkgLen, 0x00);
    *pTxPkg ++= chkCode;                        /* У����  L        */
    *pTxPkg ++= chkCode >> 8;                   /* У����  H        */
    #else
    /* �������ݰ���У�� : �����
     */
    chkCode   = checksum((int8u*)(queryPkgBuf), queryPkgLen);
    *pTxPkg ++= chkCode;                        /* У����           */
    #endif
    
    /* ���������ݳ��� */
    queryPkgLen = pTxPkg - queryPkgBuf;
    
    /* �����ڿ����������������� */
    txIPSUpdatePkgFlag = FALSE;
    if((ifIPSUartTxIsBusy() == FALSE))
    {
        /* ������������ */
        if(uartStartSend(pUartIPS->usrComNo, queryPkgBuf, queryPkgLen) == COM_TX_OK)
        {
            txIPSUpdatePkgFlag = TRUE;
        }
    }
    
    return txIPSUpdatePkgFlag;
}

/* �������ײ���ã��Ƿ��յ��ӻ��Ļظ���������־��
 * ���� TRUE ��ʾ�յ���ͬʱ���� ״ֵ̬�Ͷκ�
 * ע�⣺
 *  �˺����������־
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

/* �����������֡ 
 * ���� 
 *  pUartCom        : ���ڿ��ƽṹ��ָ��
 *  dstNode         : ָ��������Ķ���
 *  incSeq          : TRUE = �������
 * ����
 *  TRUE ���յ�ָ�����������֡
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
        
        /* �����յ��������� */
        pPkgHeader = (tPktHeader *)(pUartCom->RxTxBuf.pRxBuf);
        rxPkgHeader = * pPkgHeader;

        rxDataLen = ((int16u)rxPkgHeader.dataLenH << 8) + (int16u)rxPkgHeader.dataLenL;

        rxSrcNode = rxPkgHeader.srcNode;
        if(rxSrcNode != dstNode)
        {
            /* ���ն����뷢�Ͷ��� ���Ƕ�Ӧ�� */
            break;
        }

        pSlvInfo = getSlaveInfoPtr(dstNode);
        if(pSlvInfo == NULL)
        {
            break;
        }

        #if (PIC24_BL_ENABLE != 0)
        /* ������ѯʱ�������յ� PIC24 �ظ����� Bootloader ��ص���Ϣ 
         */
        if(((tEnmCommObjCmd)(rxPkgHeader.msgType) == OBJ_CMD_BL_CTRL) || 
           ((tEnmCommObjCmd)(rxPkgHeader.msgType) == OBJ_CMD_BOOTLOADER))
        {
            break;
        }
        #endif
                
                
        rxMsgSts = TRUE;

        /* ��������ȷ��ͨ������֡��ͨ������ OK */
        pUartCom->uartSts.linkState = 1;
        pSlvInfo->slaveLinkOK       = 1;
        pSlvInfo->commErrCnt        = 0;
        if(pSlvInfo->dataValidCnt > 0)
            pSlvInfo->dataValidCnt --;
        
        if(rxSrcNode == COMMOBJ_SWITCHER)
        {
            pSlvInfo->slaveLinkOK       = 1;
        }

        /* �жϴӻ��ظ���֡���
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

        /* �ӻ��ظ��������� BIT7 ������Ϊ1
         *  */
        //rxPkgHeader.msgType &= ~CMD_DIR_BIT;
        switch((tEnmCommObjCmd)(rxPkgHeader.msgType))
        {
            /* �ӻ��ظ�����IO����     */
            case OBJ_CMD_IOR:
                /* ����ӻ����������� */
                //pSlvInfo->slaveInput = *((tCmdReadIO_s *)(RxTxBufPtr->pRxBuf + sizeof(tPktHeader)));
                
                /* pSlvInfo ��ָ�� MONITOR ��ѯ�Ķ�����ظ����ݵ�Դ�޹�
                 * �������������ǲ�ѯ����ظ��� */
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
                /* LOG_PIC32_B �ĳ���: �̵���������Ϣ������ IO ���ݺ��� */
                if((dstNode == COMMOBJ_LOG1) || (dstNode == COMMOBJ_LOG2))
                {
                    int c;
                    int8* psrc =RxTxBufPtr->pRxBuf + sizeof(tPktHeader) + sizeof(tReadLOG_IO);
                    int8* pdst = (int8*)getRelayRecordPtr(dstNode);
                    
                    for(c = 0; c<sizeof(tRelayRecord); c++)
                    {
                        *pdst++ = * psrc++;
                    }
                    
                    /* ÿ���̵�������Ϣ(tRelayLife)������4���ֽ���Ϣ + 4�ֽ�����
                     * ֻ�Ѽ̵����������µ����͸� MMS �����ݴ洢��
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
            /* �ӻ��ظ����̵���������Ϣ */
            case OBJ_CMD_RELAY_LIFE:
            {
                if((dstNode == COMMOBJ_LOG1) || (dstNode == COMMOBJ_LOG2))
                {
                    memcpy((int8*)getRelayRecordPtr(dstNode), RxTxBufPtr->pRxBuf + sizeof(tPktHeader) + sizeof(tReadLOG_IO), sizeof(tRelayRecord));
                }
            }
            break;
            #endif

            /* �ӻ��ظ�������汾��Ϣ */       
            case OBJ_CMD_DEVINFO:
            case OBJ_CMD_RPLYINFO_OK:
                /* �յ��ӻ�������汾��Ϣ */
                if(rxDataLen >  sizeof (pSlvInfo->slaveDevInfo))
                {
                    rxDataLen = sizeof(pSlvInfo->slaveDevInfo);
                }
                
                memcpy((int8u *)(pSlvInfo->slaveDevInfo), RxTxBufPtr->pRxBuf + sizeof(rxPkgHeader), rxDataLen);
                pSlvInfo->VersionFlag = 1;

                /* ����������ͣ���IO���ݣ��� ��������Ϣ��ȴ�յ��ظ��汾��
                 * ˵���ӻ�û�����ã���Ҫ�Դӻ��������� */
                /* �ӻ�δ���� */
                //pSlvInfo->ConfigFlag = CFG_INVALID;
                break;

            /* �ӻ��ظ����������״̬   
             * �ظ����ݵĸ�ʽ:
             *     0   0x5A
             *     1   0x5A     
             *     2   ����״̬(u8UpdateState)
             *     3   �ֶκ� L(���������ɣ��κ������һ�Σ���������һ����)
             *     4   �ֶκ� H
             *  */    
            case OBJ_CMD_UPDATESTS:
            case OBJ_CMD_UPDATE_RPLY:
                pData = RxTxBufPtr->pRxBuf + sizeof(rxPkgHeader);
                pData += 2;

                pSlvInfo->slaveReplyUpdate  = TRUE;
                pSlvInfo->slaveUpdateSts    = *pData++;
                pSlvInfo->slaveUpdateSec    = ((int16u)(*(pData + 1)) << 8) + (int16u)(*(pData + 0));
                break;
                
            /* �������� */
            case OBJ_CMD_TESTDATA:
            {
                if((rxSrcNode == COMMOBJ_LOG1) || (rxSrcNode == COMMOBJ_LOG2))
                {
                    /* ���� IO ����
                     * ����1�ֽ� + IO������Ϣ
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

                    /* Ȼ���ǿ����߼�������
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
 * ��ѯ���̣��������Դӻ�����Ϣ
 * ����
 *  TRUE  ���մӻ���Ϣ��ȷ
 *  FALSE û�н��յ�����յ��������Ϣ
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
 * IPSģ����ؿ�������MCU���ڳ�ʼ��
 */
PUBLIC void initIPSModule(void)
{
    /* ���ڳ�ʼ�� */
    initIPSComm(&uartIPS);
    
    pUartIPS = getIPSUartCommPtr();
    
    /* �洢����ʼ�� */
    memset((int8u*)&PollingIPS, 0, sizeof(PollingIPS));
    
    PollingIPS.pollingCom           = pUartIPS;
    PollingIPS.pollingObjList       = (int8u *)pollingIPSList;
    PollingIPS.pollingObjNum        = sizeof(pollingIPSList);
    PollingIPS.fpCbProcRxPkg        = IPS_PollingProcRxMsg;
    
    PollingIPS.PollingTurnTime      = IPS_INTURN_TIME;  /* ��ѯʱ�䣬ֻ����ѯ��Ч */
    PollingIPS.PollingTxOvTime      = IPS_TX_OVERTIME;  /* ���ͳ�ʱ��ֻ����ѯ��Ч */
    PollingIPS.ErrMaxNumOffLine     = IPS_OFFLINE_NUM;  /* �������������ж�����   */
    PollingIPS.setWaitTXTime        = 0;                /* �л������͵���ʱʱ��   */
    
    /* ��ʼ�����������Ļص����� */
    InitUartUpdateCallBack(IPSUpdate_TxPkg2IPS, IPSUpdate_TxBusy, IPSUpdate_QuerySlave, IPSObj_chkOnline, IPSUpdate_GetUpdateInfo);
    
}


/* =============================================================================
 * �� IBP PSL SIG ��չ���ͨ�Ź���
 * ����������������
 * 
 * ��Ҫ����������ѭ������
 * =============================================================================
 */
PUBLIC void processIPSComm(void)
{
    tSYSTICK tickNow;
    
    /* �ж���ӻ���ͨ������ */
    tickNow = getSysTick();
    
    if(MMS_UPDATE_STATE() != MST_STATE_IDLE)
    {
        /* �������� IBP PSL SIG
         * ����ֻѭ��������Ϣ�������������ݣ��� MMS_Command ���
         */
        if((MMS_OP_DST_ID == COMMOBJ_SWITCHER) || (MMS_OP_DST_ID == COMMOBJ_LOG1) || (MMS_OP_DST_ID == COMMOBJ_LOG2))
        {
            Uni_ProcRecvedFrame(&uartIPS, &IPS_UpdateProcRxMsg);
        }
    }
    else
    {
        /* ��ѯ�ӻ� 
         */
        PollingIPS.fpCbProcRxPkg = IPS_PollingProcRxMsg;
        Uni_PollingSlaves((void *)&PollingIPS);
        
        /* ��ʱ������ߵ���չ��İ汾��Ϣ���ٴ����ߺ���Ҫ��ȡ�汾��Ϣ 
         * ��Ҫ�ǿ�����չ��������������Զ�������MON �Զ��ٴ��ض�
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
        
        
        /* �ൺ4����ʱ��ȡ�̵�����������¼
         *  */
        static tSYSTICK tickReadRelayRecord;
        if((tSYSTICK)(tickNow - tickReadRelayRecord) > 3000)
        {
            tickReadRelayRecord = tickNow;
            
            bReadRelayLife[0] = TRUE;
            bReadRelayLife[1] = TRUE;
            
        }
    }
    
    
    
    /* �жϴ�������״̬������״̬�Ƿ��쳣 
     * �� IBP PSL SIG ����һ��ͨ�ųɹ����򱾻� UART OK
     */
    if(uartChkLinkAndSendSts(COM_IPS) == TRUE)
    {
        /* ��⴮��ͨ�Ŵ����־
         * �����������³�ʼ������
         *  */
        if ( INTGetFlag(INT_SOURCE_UART_ERROR(UART_IPS_ID)) )
        {
            INTClearFlag(INT_SOURCE_UART_ERROR(UART_IPS_ID));
            initIPSComm(&uartIPS);
        }
    }
}


