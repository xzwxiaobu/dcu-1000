
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

PRIVATE tUartComm uartMMS;                      /* ����ͨ�ſ��� */
PRIVATE int8u RxMMSDataBuf[MMS_RXBUF_SIZE];     /* ����ͨ�ſ���-���ջ��� */
PRIVATE int8u TxMMSDataBuf[MMS_TXBUF_SIZE];     /* ����ͨ�ſ���-���ͻ��� */

/* ��ȡ MMS ͨ��״̬ */
PUBLIC int8u getUartMMSLinkSts(void)
{
    return uartMMS.uartSts.linkState;
}

/* ��ȡ MMS ͨ�� Uart ���ͻ�����ָ�� */
PUBLIC int8u* getMMSTxBufPtr(void)
{
    return uartMMS.RxTxBuf.pTxBuf;
}

/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
PUBLIC BOOL ifMMSUartTxIsBusy(void)
{
    return (BOOL)(uartMMS.uartSts.isSending);
}

/* ���������õĺ��� 
 * TRUE ��ʾ���ڷ��ͽ���
 */
PUBLIC BOOL chkFinishTxToMMS(void)
{
    return (BOOL)(!ifMMSUartTxIsBusy());
}


/* �������ݵ�MMS
 *  pTxData     : ���������ݻ���ָ��
 *  TxDataLen   : ���������ݵĳ���
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

/* ���ڽ��յ����ݣ�һ���ֽڣ���Ĵ������
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
    pCom->RxTxBuf.pRxBuf[pCom->RxTxBuf.rxCnt] = rxData;

    if(pCom->RxTxBuf.rxCnt >= 10)
    {
        j = (int16u)MMSRxBufPtr[7] + (((int16u) MMSRxBufPtr[6]) << 8);

        if((j + 10) == pCom->RxTxBuf.rxCnt)
        {
            if((MMSRxBufPtr[j + 8] != UARTDLE) || (MMSRxBufPtr[j + 9] != UARTETX))
            {
                /* β��Ǵ������½��� */
                pCom->RxTxBuf.rxCnt = 1;
                MMSRxBufPtr[0] = rxData;
                return;
            }
            
            /* �õ�����������֡
             * ��¼��ǰ��ʱ��� 
             * ���ﲻ������У�� ( _checksum )�������ж�ռ�� CPU ʱ�����
             *  */
            pCom->RxTxBuf.rxCnt ++;             //���һ���ֽڣ���������
            pCom->uartSts.gotFrame  = 1;        //bFun_Uart2_Receive_Handle = 1;
            pCom->gotFrameTick      = nowTick;  //MMS_PEDC_UART_OverTime = 0 ;    //UART ͨѶ��ʱ��
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
                if( (MMSRxBufPtr[2] != 0x0) && (MMSRxBufPtr[2] != 0x1) )   // == 0 �����߱�־  ��== 1 �����߱�־
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

            /* ͨ�Ŷ���PEDC=200, , DCU 
             * 0xFF = ͨ�ö���
             * 200  = PEDC-MON
             * 1~64 = DCU
             * 0x80 = �������� DCU
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
 * ��ʼ�� MMS ͨ��ģ��
 */
PUBLIC void initMMSModule(void)
{
    DeInitUartCom(COM_MMS, &uartMMS);
    
    uartMMS.RxTxBuf.pRxBuf      = RxMMSDataBuf;         /* ����������ָ��               */
    uartMMS.RxTxBuf.rxBufSize   = sizeof(RxMMSDataBuf); /* ������������С               */
    uartMMS.RxTxBuf.rxCnt       = 0;                    /* �ѽ��յ����ݳ���             */

    uartMMS.RxTxBuf.pTxBuf      = TxMMSDataBuf;         /* ����������ָ��               */
    uartMMS.RxTxBuf.txBufSize   = sizeof(TxMMSDataBuf); /* ������������С               */
    uartMMS.RxTxBuf.txCnt       = 0;                    /* �ѷ������ݵĳ���             */
    uartMMS.RxTxBuf.txDataLen   = 0;                    /* ���������ݵĳ��ȣ�0��ʾû��  */
    
    uartMMS.funRx               = &mms_UartReceiveData; /* �����жϽ���һ���ֽں����           */
    uartMMS.funTx               = NULL;                 /* �������ݽ������жϵ���, null��ʾû�� */
        
    uartMMS.RS485_4Wire         = TRUE;                 /* ȫ˫�� 485  */
    uartMMS.fpDE_Dis            = &uartMMS_DE_Dis;
    uartMMS.fpDE_En             = &uartMMS_DE_En;
    uartMMS.fpRE_Dis            = NULL;
    uartMMS.fpRE_En             = NULL;
    
    uartMMS.frameOverTime       = 5;            /* ms��֡���ʱ�䣬����ʱ����û��������Ϊ֡���� */
    uartMMS.sendOverTime        = 300;          /* ms�����÷������ݳ�ʱʱ�䳤��                 */
    uartMMS.abortFrameTime      = 2000;         /* ms, ������ָ����ʱ��û����������֡����   */
    uartMMS.linkFailTime        = cMMS_PEDC_UART_OverTime; /* MMS ͨ�ŶϿ�ʱ������ */

    uartCOM_Init(COM_MMS, UART_MMS_ID, &uartMMS, UART_MMS_BR, UART_MMS_IPL);
}

/* =============================================================================
 * ������յ� MMS ����֡
 * ѭ������
 * ������
 *  appMMS_ProcRxPkg : APP�Ļص����������������ݰ����պ����
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

    /* �Ƿ��յ�����������֡ */
    if(uartMMS.uartSts.gotFrame)
    {
        dataLen = (int16u)MMSRxBufPtr[7] + (((int16u) MMSRxBufPtr[6]) << 8);
        
        /* ����У�� */
        if(MMSRxBufPtr[dataLen + 10] != _checksum(&(MMSRxBufPtr[0]), dataLen+10))
        {
            uartMMS.uartSts.errFrame    = 1;
            uartMMS.RxTxBuf.rxCnt       = 0;
            uartMMS.uartSts.gotFrame    = 0;
            return;
        }
        
        /* �� MMS ����ͨ��״̬OK */
        uartMMS.uartSts.linkState   = 1;
        uartMMS.uartSts.errFrame    = 0;
        
        ret = appMMS_ProcRxPkg((int8u*)uartMMS.RxTxBuf.pRxBuf, uartMMS.RxTxBuf.rxCnt);
        
        uartMMS.RxTxBuf.rxCnt    = 0;
        uartMMS.uartSts.gotFrame = 0;
    }
    
    return ret;
}
