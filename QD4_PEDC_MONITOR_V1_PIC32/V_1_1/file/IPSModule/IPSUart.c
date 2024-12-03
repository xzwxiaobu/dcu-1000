/*******************************************************************************
 * ����ͨ�� ����ѯ����
 *  Uni_PollingSlaves   : ��ѯ��������֧����ѯ IBP/PSL/SIG����ѯ PIC24A/PIC24B 
 *  Uni_ProcRecvedFrame : �ж�ָ�������Ƿ��з���Ҫ�������֡��ͨ���ص���������
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
 * ���ر���
 */
PRIVATE int8u RxIPSDataBuf[UART_COM_RXBUFSIZE];         /* ����ͨ�ſ���-���ջ��� */
PRIVATE int8u TxIPSDataBuf[UART_COM_TXBUFSIZE];         /* ����ͨ�ſ���-���ͻ��� */

PRIVATE tUartComm * pUartComm;

PUBLIC void setCommunObjAB(tEnmCommObj obj);

#define QUERY_EXTCMD_INTURN     /* MMS_App.c ������������(createTx2LogPackage) */
#ifdef  QUERY_EXTCMD_INTURN
/* ���Ϳ��������õĻ���
 * ����ѯ���͵Ļ���ֿ�
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
/* ���ڳ�ʼ��      
 *  pUart       : ����ͨ�Žṹָ��
 *  */
PUBLIC void initIPSComm(tUartComm * pUart)
{
    pUartComm = pUart;
    
    DeInitUartCom(COM_IPS, pUartComm);                      /* COM_IPS �����⴮�ں� */
    
    pUartComm->RxTxBuf.pRxBuf       = RxIPSDataBuf;         /* ����������ָ��               */
    pUartComm->RxTxBuf.rxBufSize    = sizeof(RxIPSDataBuf); /* ������������С               */
    pUartComm->RxTxBuf.rxCnt        = 0;                    /* �ѽ��յ����ݳ���             */
   
    pUartComm->RxTxBuf.pTxBuf       = TxIPSDataBuf;         /* ����������ָ��               */
    pUartComm->RxTxBuf.txBufSize    = sizeof(TxIPSDataBuf); /* ������������С               */
    pUartComm->RxTxBuf.txCnt        = 0;                    /* �ѷ������ݵĳ���             */
    pUartComm->RxTxBuf.txDataLen    = 0;                    /* ���������ݵĳ��ȣ�0��ʾû��  */
     
    pUartComm->funRx                = &IPS_UartReceiveData; /* �����жϽ���һ���ֽں����           */
    pUartComm->funTx                = NULL;                 /* �������ݽ������жϵ���, null��ʾû�� */
         
    pUartComm->RS485_4Wire          = TRUE;                 /* ȫ˫�� 485  */
    pUartComm->fpDE_Dis             = &uartIPS_DE_Dis;
    pUartComm->fpDE_En              = &uartIPS_DE_En;
    pUartComm->fpRE_Dis             = &uartIPS_RE_Dis;
    pUartComm->fpRE_En              = &uartIPS_RE_En;
     
    pUartComm->frameOverTime        = 10;           /* ms��֡���ʱ�䣬����ʱ����û��������Ϊ֡���� */
    pUartComm->sendOverTime         = 200;          /* ms�����÷������ݳ�ʱʱ�䳤��                 */
    pUartComm->abortFrameTime       = 500;          /* ms, ������ָ����ʱ��û����������֡����   */
    pUartComm->linkFailTime         = IPS_LINKFAIL_TIME;

    uartCOM_Init(COM_IPS, UART_IPS_ID, pUartComm, UART_IPS_BR, UART_IPS_IPL);
}

/* =============================================================================
 * ִ����ѯ����
 * ����
 *  pPollingPtr     : ��ѯ����ָ��
 * ����
 *  ������ѯ����
 */
PUBLIC tEnmCommObj Uni_PollingSlaves(void *pPollingPtr)
{
    tEnmCommObj     pollingObj;         /* ��ѯ���� */
    tSlaveInfo   *  pPollingSlvInfo;    /* ��ѯ������Ϣ */
    
    int     txDataLen;
    int8u * txDataPtr = NULL;
    BOOL    uartTxBusy;                 /* ָ�����ڵķ���״̬, TRUE = æ */
    
    if(pPollingPtr == NULL)
        return COMMOBJ_UNDEF;
    
    tPollingTask *pPollingTask = (tPollingTask *)pPollingPtr;
    
    /* ��ѯ��������� */
    if(pPollingTask->pollingObjNum == 0)
    {
        return COMMOBJ_UNDEF;
    }
    
    /* ���β�ѯ��˳��� */
    if(pPollingTask->pollingTurn >= pPollingTask->pollingObjNum)
    {
        pPollingTask->pollingTurn = 0;
    }

    /* ���β�ѯ�Ķ��� */
    pollingObj      = pPollingTask->pollingObjList[pPollingTask->pollingTurn];
    pPollingSlvInfo = getSlaveInfoPtr(pollingObj);
    
    /* ��ѯ���� */
    uartTxBusy  = ((pPollingTask->pollingCom)->uartSts).isSending;
    switch(pPollingTask->sendPkgStep)
    {
        /* û�з�������             
         */
        case SENDSTEP_IDLE:
            /* �������ͬʱ��ѯ IBP PSL SIG PIC24A PIC24B 
             *       �򵥶���ѯ PIC24A PIC24B 
             */
            
            setCommunObjAB(pollingObj);
                
            if((pollingObj == COMMOBJ_IORW_A) || (pollingObj == COMMOBJ_IORW_B))
            {
                /* ����ͨ�Ŷ�������IO��� 
                 */
                pPollingTask->sendPkgStep = SENDSTEP_WAITTX;
            }
            else
            {
                /* IBP PSL SIG �岻��Ҫ�ȴ���ʱ */
                pPollingTask->sendPkgStep = SENDSTEP_NEWTASK;
            }
            
            pPollingTask->pollingStepTime = getSysTick();
            break;
            
        /* ��ʱ����ҪĿ�ı�֤Ӳ��IO�л����ȶ� */
        case SENDSTEP_WAITTX:
            if(getSysTimeDiff(pPollingTask->pollingStepTime) < pPollingTask->setWaitTXTime)
                break;
            pPollingTask->sendPkgStep = SENDSTEP_NEWTASK;
            //break;        //�� break ֱ�ӽ�����һ��
            
        case SENDSTEP_NEWTASK:
            /* �ȴ����ڿ��� */
            if(uartTxBusy)
                break;
            
            (pPollingTask->sendPkgHeader).dstNode   = (int8u)pollingObj;
            (pPollingTask->sendPkgHeader).seq       = pPollingSlvInfo->sendSeq;
            
            /* ��ʱ��ȡ����ļ̵�������������¼ */
            if(ifReadRelayLife((int8u)pollingObj))
            {
                /* �����Ƿ��ȡ�ɹ�����һ�ζ�ִ����������ѯ���� */
                clearReadRelayLife((int8u)pollingObj);
                
                (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_RELAY_LIFE;    /* ���̵���������Ϣ */
                
                txDataLen = 0;

                /* �����������ݰ������� */
                pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
            }
            /* ������ѯ */
            else
            {
                if(pPollingSlvInfo->VersionFlag == 0)
                {
                    /* tPktRequestVersion */
                    (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_DEVINFO;    /* ������汾��Ϣ */
                    txDataLen = 0;

                    /* �����������ݰ������� */
                    pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                }
                else
                {
                    (pPollingTask->sendPkgHeader).msgType = OBJ_CMD_IOR;        /* ��IO����     */

                    if((pollingObj == COMMOBJ_IORW_A) || (pollingObj == COMMOBJ_IORW_B))
                    {
                        txDataPtr = (int8u*)getPic24OutputIODataPtr(&txDataLen);

                        /* �����������ݰ������� */
                        pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                    }
                    /* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ� */
                    else if((pollingObj == COMMOBJ_SWITCHER))
                    {
                        extern int8u* getSwitchOutputIODataPtr(int *DataLen);
                        
                        txDataPtr = (int8u*)getSwitchOutputIODataPtr(&txDataLen);
                        /* �����������ݰ������� */
                        pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                    }
                    else
                    {
                        #ifdef  QUERY_EXTCMD_INTURN
                        if((bSendExtCmd) && ((pollingObj == COMMOBJ_LOG1) || (pollingObj == COMMOBJ_LOG2)))
                        {
                            /* ����ѯ�����в��뷢�Ϳ������� */
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

                            /* �����������ݰ������� */
                            pPollingTask->sendPkgStep = createUniTxPackage((void *)pPollingTask, txDataPtr, txDataLen, TRUE);
                        }
                    }
                }
            }

                    
            pPollingTask->pollingStepTime = getSysTick();
            (pPollingTask->pollingCom)->RxTxBuf.rxCnt = 0;
            (pPollingTask->pollingCom)->uartSts.gotFrame = 0;
            break;
        
        /* �ȴ��������             
         */
        case SENDSTEP_SENDING:  
            if(uartTxBusy)
            {
                /* 
                 * �������ָ��ʱ��û�з��ͽ���
                 * ǿ��ֹͣ����
                 */
                if(getSysTimeDiff(pPollingTask->pollingStepTime) < pPollingTask->PollingTxOvTime)
                {
                    break;
                }
                else
                {
                    /* ���ͳ�ʱ��ǿ��ֹͣ����
                     * ͨ�Ŵ������������ */
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
            
        /* ���Ͳ�ѯ������ɺ󣬵ȴ��ظ�         
         */
        case SENDSTEP_WAITRX:   
            /* ����������� 
             *
             * ��ѯ�Ƿ�����Ҫ�������Ϣ
             * ���� TRUE  ���մӻ���Ϣ��ȷ
             */
            if(pPollingTask->fpCbProcRxPkg != NULL)
            {
                if(Uni_ProcRecvedFrame(pPollingTask->pollingCom, pPollingTask->fpCbProcRxPkg) == TRUE)
                {
                    /* ��ȷ���� */
                    pPollingTask->sendPkgStep = SENDSTEP_FINISH;
                }
                else if(getSysTimeDiff(pPollingTask->pollingStepTime) >= pPollingTask->PollingTurnTime)
                {
                    /* ���յȴ���ʱ */
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
            
        /* ���β�ѯ���     
         */
        case SENDSTEP_FINISH:   
            if(getSysTimeDiff(pPollingTask->pollingStepTime) >= pPollingTask->PollingTurnTime)
            {
                if(pPollingSlvInfo->commErrCnt >= pPollingTask->ErrMaxNumOffLine)
                {
                    pPollingSlvInfo->slaveLinkOK    = 0;
                    
                    /* �������ߺ���Ҫ����ǰ�漸�ε����� */
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
 *      ͨ�ô��ڹ��ܽӿں���
 * 
 *      ������ͳһ�շ�ͨ��Э��������£�֧��:
 *      1. ͬһ��������ѯ��ͬ��ϣ���  (IPB + PSL + SIG)��(IPB + PSL + SIG + PIC24A + PIC24B) 
 *      2. ��ͬ���ڷֱ���ѯ  (IPB + PSL + SIG)  (PIC24A + PIC24B)
 * 
 *******************************************************************************
 */

/* =============================================================================
 * ͨ�ô��ڽ����ֽڴ�����
 *  pUartCom    : ���ڽṹָ��
 *  data        �����յ��ֽ�����
 */
//PUBLIC void Uni_UartReceiveData(tUartComm *pUartCom, int8u data)
PUBLIC void Uni_UartReceiveData(tUartComm *pCom, int8u data)
{
    volatile int16u dataLen; 
    
    tSYSTICK nowTick;
    
    nowTick = getSysTick();

    /* ���յ����ϴ�����û�д���
     * ������ָ����ʱ��û����������֡����
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

    /* 1. ���յ����ݳ��ȳ������ջ���������������ս������¼��� 
     * 2. ���ֽ�֮��Ľ���ʱ�����ƣ�����ʱ������Ϊ���µ�һ֡���ݣ����½��ռ��� 
     * 
     * �Ѿ��������㴦��uartDrv.c --> lastByteTick��
     */
    
    /* save the received data */
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = data;
    pCom->RxTxBuf.rxCnt ++;

    /* ��������֡ */
    /* �Ƿ��Ѿ��ҵ�֡ͷ��Ϣ */
    if(pCom->RxTxBuf.rxCnt >= sizeof(tPktHeader) + sizeof(tPktTail) )
    {
        /* ���ݰ����� */
        dataLen = ((int16u)pCom->RxTxBuf.pRxBuf[6] << 8) + (int16u)(pCom->RxTxBuf.pRxBuf[7]);
        
        /* �жϽ��յ���������  */
        if((pCom->RxTxBuf.rxCnt >= (dataLen + sizeof(tPktHeader) + sizeof(tPktTail))))
        {
            if((pCom->RxTxBuf.pRxBuf[dataLen + 8] == TAIL_DLE) && (pCom->RxTxBuf.pRxBuf[dataLen + 9] == TAIL_ETX))
            {
                /* �õ�����������֡
                 * ��¼��ǰ��ʱ��� 
                 */
                pCom->uartSts.gotFrame  = 1;
                pCom->gotFrameTick      = nowTick;
                
                /* ���ﲻ����Ϊͨ����������Ϊ����֡��û�н���У�� 
                 * pCom->uartSts.linkState = 1;
                 */
            }
            else
            {
                /* ���½��� */
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
        /* ����֡ͷ��Ϣ */
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
                
            case 3:     /* 2: ֡���                        */
            case 4:     /* 3: Դ�ڵ�,                       */
            case 5:     /* 4: ����ڵ� tEnmCommObj          */
            case 6:     /* 5: ͨ���������� tEnmCommObjCmd   */
            case 7:     /* 6: ���ݳ���H */     
            case 8:     /* 7: ���ݳ���L */    
                break;
                
            default :
                break;
        }
        
    }
}

/* =============================================================================
 * ͨ�ô��ڽ�������֡������
 *  pUartCom        : ִ�д��������ָ��
 *  pCbProcFrame    : ���յ�����������֡��ص�����
 */
PUBLIC BOOL Uni_ProcRecvedFrame(tUartComm * pUartCom, cbProcRxPkg pCbProcFrame)
{
    int16u crc16;
    int16u rxDataLen;
    
    tPktHeader *pHeader;
    tPktTail   *pTail;
    
    tUartBuf    *RxTxBufPtr;
    
    /* ���ؽ�� */
    BOOL rxMsgSts = FALSE;
    
    /* �����õ���ָ�� */
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

            /* �д����������֡
             * ͨ���ص��������� */
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
 * �����������ݰ�
 * ����
 *  pPollingTask    : ��ѯ����ָ��
 *  data            : ����������ָ��
 *  dataLen         : ���������ݳ���
 *  sendNow         : ���ͱ�־��= true, ��ʾ������������
 * ���ط��ʹ���״̬
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
    *pTxPkg ++= pkgHeader->seq;                     /* ˳���           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* ͨ������Դ�ڵ�   */
    *pTxPkg ++= pkgHeader->dstNode;                 /* ͨ������Ŀ��ڵ� */
    *pTxPkg ++= pkgHeader->msgType;                 /* ͨ����������     */
    *pTxPkg ++= (int8u)(dataLen >> 8);              /* ���ݳ��� H       */
    *pTxPkg ++= (int8u)(dataLen);                   /* ���ݳ��� L       */
    
    
    /* ������� */
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
    
    txLen = pTxPkg - (pPollingTask->sendPkgBuf);           /* ������У������ݳ��� */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* �������ݰ���У�� : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), txLen, 0x00);
    *pTxPkg ++= chkCode;                        /* У����  L        */
    *pTxPkg ++= chkCode >> 8;                   /* У����  H        */
    #else
    /* �������ݰ���У�� : �����
     */
    chkCode   = checksum((int8u*)(&(pPollingTask->sendPkgBuf)), txLen);
    *pTxPkg ++= chkCode;                        /* У����  L        */
    #endif
    
    /* ���������ݳ��� */
    txLen = pTxPkg - (pPollingTask->sendPkgBuf);
    
    /* �������� */
    if(txLen > 0)
    {
        pPollingTask->sendPkgLen = txLen;
        
        if((sendNow) && (pPollingTask->pollingCom->uartSts.isSending == 0))
        {
            /* ������������ */
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
            /* �ȴ���һ�εķ��ͽ��� */
            retStep = SENDSTEP_NEWTASK;
        }
    }
    else
    {
        /* ����ʧ�� */
        retStep = SENDSTEP_IDLE;
    }
    
    return retStep;
}


#ifdef  QUERY_EXTCMD_INTURN
/* =============================================================================
 * �����������ݰ������͸� �߼���
 *  pkgHeader   : ͷ��Ϣָ��
 *  data        �� ����ָ��
 *  dataLen     �� ���ݳ���
 *  sendNow     : ���ͱ�־��= true, ��ʾ������������
 * ���ش����͵����ݳ��ȣ�0 ��ʾ����ʧ��
 */
PUBLIC int16u createTx2LogPackage(tPktHeader *pkgHeader, int8u* data, int dataLen, BOOL sendNow)
{
    int i;
    int8u *pTxPkg;
    int8u *pSrc;
    int16u chkCode;
    (void)  sendNow;
    
    /* �жϻ������Ƿ��㹻 */
    if(dataLen > sizeof(SendExtCmdBuf) - 11)
        return 0;
    
    pTxPkg = SendExtCmdBuf;
    
    *pTxPkg ++= HEADER_DLE;                         /* header 1         */
    *pTxPkg ++= HEADER_STX;                         /* header 2         */
    *pTxPkg ++= pkgHeader->seq;                     /* ˳���           */
    *pTxPkg ++= PCB_ID_SERVER;                      /* ͨ������Դ�ڵ�   */
    *pTxPkg ++= pkgHeader->dstNode;                 /* ͨ������Ŀ��ڵ� */
    *pTxPkg ++= pkgHeader->msgType;                 /* ͨ����������     */
    *pTxPkg ++= (int8u)(dataLen >> 8);              /* ���ݳ��� H       */
    *pTxPkg ++= (int8u)(dataLen);                   /* ���ݳ��� L       */
    
    
    /* ������� */
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
    
    SendExtCmdLen = pTxPkg - SendExtCmdBuf;     /* ������У������ݳ��� */
    
    #if (FRAME_VERIFY_CRC16 == 1)
    /* �������ݰ���У�� : CRC16
     */
	chkCode   = CalcCRC16_A001((int8u*)(&sendPkgBuf[0]), SendExtCmdLen, 0x00);
    *pTxPkg ++= chkCode;                        /* У����  L        */
    *pTxPkg ++= chkCode >> 8;                   /* У����  H        */
    #else
    /* �������ݰ���У�� : �����
     */
    chkCode   = checksum((int8u*)(SendExtCmdBuf), SendExtCmdLen);
    *pTxPkg ++= chkCode;                        /* У����  L        */
    #endif
    
    /* ���������ݳ��� */
    SendExtCmdLen = pTxPkg - SendExtCmdBuf;
    
    bSendExtCmd = TRUE;
    
    return SendExtCmdLen;
}

PRIVATE tEnmSendPkgStep sendExtendCommand()
{
    tEnmSendPkgStep retStep = SENDSTEP_IDLE;
    
    /* �������� */
    if((bSendExtCmd == TRUE) && (SendExtCmdLen > 0))
    {
        if((pUartComm->uartSts.isSending == 0))
        {
            /* ������������ */
            clrIPSUpdatePkgFlag();
            uartStartSend(pUartComm->usrComNo, SendExtCmdBuf, SendExtCmdLen);
            
            retStep     = SENDSTEP_SENDING;
            bSendExtCmd = FALSE;
        }
        else
        {
            /* �ȴ���һ�εķ��ͽ��� */
            retStep = SENDSTEP_NEWTASK;
        }
    }
    
    return retStep;
}
#endif



