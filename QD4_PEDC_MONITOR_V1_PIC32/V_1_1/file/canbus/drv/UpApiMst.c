/*=============================================================================
 * Project:     PEDC ͨ�ù���
 * PEDC MCU:        PIC32MX795F512H
 * Fcrystal:        4M
 * PLL:         X20
 * FCY:         SYSTEM_FREQ = 80M
 *
 * Version:     2.0
 * Author:      lly
 * Date:            2022-3-28
 * =============================================
 * ��    �� :   UpApiMst.C
 *          :   ͨ��CAN����������MASTER ��
 * 
 *  -
 *          ���Ӷ����� IBP PSL SIG�����������
 *  - 2022-4-7
 *          ���Ӷ��������� DCU ��֧��
 *
 * =============================================================================*/
#if ((UPDATE_FUN_EN == TRUE))
#include  "_cpu.h"

#include  "_g_canbus.h"
#include "glbCan.h"
#include "glbUpdate.h"
#include "_glbApi.h"

#include "Update.h"
#include "..\ObjDict.h"
#include "CanTgt.h"
#include "CanDrv.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

#define NODATA_ASK_MORE             //����������ָ��ʱ����û������, PEDC��������(UPFW_STATE_SEC_ERR)
#define BATCH_UPDATE_DCUS           //�������� DCU ����
//#define TEST_BAT_UPDATE_DCU       //������������DCU

/* ֧���������� IBP PSL SIG ��
 */
/* ���������ص������ӿ� */
uartUpdate_UartTxPkg        cbUartTxPkg;
uartUpdate_UartTxBusy       cbUartTxBusy;       /* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
uartUpdate_QuerySlave       cbQuerySlaveSts;    /* Uart ��ѯ����״̬ */
uartUpdate_ChkOnline        cbChkOnline;        /* Uart ��ѯ ����״̬ */
uartUpdate_GetSlaveReply    cbGetSlaveReply;    /* Uart ���ӻ��ظ���Ϣ */

static void UpdateQuerySlave(int8u id);

extern void UPDATE_FIRMWAREStateToMMS(void); //wtj -> uart.c
extern BOOL chkFinishTxToMMS(void);
extern int8u CapiStartTrans(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);
    
//---------------------------------------------------------------------------
int8u __attribute__ ((aligned(4))) FileDataBuf[UPFW_CODE_BUF_MAX];  //�׵�ַ������뵽 int32u

int8u u8UpdateState;                 //���� MMS ��״̬, �������ݴ����������� Slave ���ص�״̬
int8u CmdBuf[8];                     //������չ������, ��������ʱ����, ��Ϊ����ʧ���Ժ�����·���һ��

tUpdateMsg UpdateMsg = {{0},0};
int16u wTickUpdateSection;
int8u u8StartUpgrade  = FALSE;

/* �������������־��
 * DCU �յ����������������Ҫִ�в��� FLASH �������ʱ��
 * PEDC������Ҫ�ȴ��ϳ�ʱ����ܲ�ѯ�� DCU ��ִ�н��
 *  */
int8u u8StartCommand  = FALSE;      

int16u wUpFwSection;                //���� MMS ���������ݰ����Ķκ�

#ifdef  NODATA_ASK_MORE
int8u   u8AskMoreCnt = 0;
#endif

//---------------------------------------------------------------------------
#ifdef BATCH_UPDATE_DCUS
typedef struct
{
    int8u   state;                          /* DCU�ظ�������״̬����Ϊ�˶��� */
    int8u   rsv;                            /* ���������� */
    int16u  section;                        /* DCU�����Ķκ�     */
}tUpdateDcuPro;
tUpdateDcuPro updateDcuSts[MAX_DCU];

BOOL   batchUpdateDCUs  = FALSE;            /* TRUE ��ʾ��������ʹ��    */
BOOL   sendUpdatePkg    = FALSE;            /* TRUE = �������ݰ��ѷ���  */
int8u  updateDCUSetting[BYTES_SLV_FLAG];    /* ���ô�������DCU, λֵ1��Ӧ��DCU��Ҫ���� */

int8u  updateDCUID;                         /* ��ǰ��ѯDCU��ID�� */

int16u updatePkgSize;                       /* MMS �´����������ݰ��ĳ��� */
int8u  updateDataPkg[UPFW_CODE_BUF_MAX];    /* MMS �´����������ݰ����� */

/* ��ȡ��������״̬
 *  */
BOOL ifBatchUpdateDCUs(void)
{
    return batchUpdateDCUs;
}
    
int8u* getBatchUpdateDCUsSetting(void)
{
    return updateDCUSetting;
}

/* ��ȡ�ظ�MMSʱ��DCU������״̬���� UPDATE_FIRMWAREStateToMMS �����е���
 *  pData : �ظ����ݴ洢������
 *  bufLen: pDataָ��Ļ���������
 * ���ػظ����ݵĳ��ȣ�0 ��ʾ����������
 *  */
int getReplyDCUUpdateState(int8u* pData, int bufLen)
{
    int len = 0;
    
    #if 0
    /* �������� PEDC����MMS�ط�����? */
    if(u8UpdateState == UPFW_STATE_ASKFORDATA)
    {   
        *pData++ = u8UpdateState;
        len      = 1;
    }
    else
    #endif
    {
        /* ��������DCUʱ���ظ� MMS ��Ϣ
         * ��һ���ֽ�������������־��0x80 ��ʾ��������, 0 ��ʾ������������
         *  */
        *pData++ = (batchUpdateDCUs) ? 0x80 : 0;

        len = 1 + sizeof(updateDCUSetting) + sizeof(updateDcuSts);
        if(len > bufLen)
        {
            return 0;
        }

        /* ��������DCU���ظ�����Ϣ��
         * 1. ����������־
         * 2. ����
         * 3. ÿ��DCU ������״̬�Ͷκ� */
        if(batchUpdateDCUs)
        {
            TgtMemCpy(pData, &updateDCUSetting, sizeof(updateDCUSetting));

            TgtMemCpy(pData +  sizeof(updateDCUSetting), &updateDcuSts, sizeof(updateDcuSts));
        }
        else
        {   /* ��������������û����ǰ��������DCU */
            TgtMemSet(pData, 0, sizeof(updateDCUSetting));
            TgtMemSet(pData +  sizeof(updateDCUSetting), UPFW_STATE_NOSETTING_DCU, sizeof(updateDcuSts));
        }
    }

    return len;
}
    
    
/* ���ô�����������DCU��λΪ1����Ӧ��DCU��Ҫ�������ֽ�0λ0 ��Ӧ DCUIDΪ1
 * ������ֵ��Ϊ0���������������
 * ���� TRUE ��ʾ���óɹ�����������DCU����������
 *  */
BOOL setBatchcastUpdateDCUs(int8u * settings)
{
    int i;
    TgtMemCpy(updateDCUSetting, settings, sizeof(updateDCUSetting));

    batchUpdateDCUs = FALSE;
    sendUpdatePkg   = FALSE;

    /* ��������������DCU�����û��ֵ������Ϊ����������
     *  */
    for(i=0; i<sizeof(updateDCUSetting); i++)
    {
        if(updateDCUSetting[i] != 0)
        {
            updateDCUID         = ID_DCU_MIN;
            batchUpdateDCUs     = TRUE;
            break;
        }
    }

    /* ��������DCU����״̬ */
    for(i=0; i<MAX_DCU; i++)
    {
        updateDcuSts[i].state   = UPFW_STATE_NOUPDATE;
        updateDcuSts[i].section = 0;
    }

    return batchUpdateDCUs;
}

/* ȡ���������� DCU */
BOOL clrBatchcastUpdateDCUs(void)
{
    batchUpdateDCUs = FALSE;
}

/* ������һ����������DCU ID (IDֵ�Ǵ�1��ʼ)����������Ч������
 * ����ֵ����һ��DCUID���� > MAX_DCU ��ʾ�������
 *  */
int8u searchFirstUpdateDCUID(void)
{
    int i;

    for(i = 0; i<MAX_DCU; i++)
    {
        if((ChkRamBit(updateDCUSetting, i) != 0) && (ChkSlaveOnLine(i + ID_DCU_MIN) == TRUE))
        {
            break;
        }
    }

    return (i + ID_DCU_MIN);
}

#endif
    
/* ��ʼ�����ڻص����� */
PUBLIC BOOL InitUartUpdateCallBack(uartUpdate_UartTxPkg cbTxPkg, uartUpdate_UartTxBusy cbTxBz, uartUpdate_QuerySlave cbQuerySts, uartUpdate_ChkOnline chkOnline, uartUpdate_GetSlaveReply cbGetReply)
{
    cbUartTxPkg     = cbTxPkg;
    cbUartTxBusy    = cbTxBz;
    cbQuerySlaveSts = cbQuerySts;
    cbChkOnline     = chkOnline;
    cbGetSlaveReply = cbGetReply;
}

/* ���ݶ���ID���������Ϊ4��
 * ���ذ� �� �� MMS ֱ��ͨ�ŵİ��ӣ��� MONITOR��        PIC32MX795F512H or PIC32MK1024MCM100
 * ��չ�� :  �� ���ذ�ͨ�ŵİ��ӣ��� IBP PSL SIG ...�� PIC32MX795F512H
 * PIC24  �� Ŀǰ������ ���ذ���ص�PIC24����������     PIC24FJ32GA
 * DCU    :  ��Ϊ���������򵥸�����                   PIC30F6010A or PIC32MK1024MCM100
 * 
 *  */
PUBLIC int8u getUpdateType(int8u id)
{
    int8u idType;
    
    if (id == UP_PEDC_MON)
    {
        idType = UPTYPE_PEDC_HOST;              /* PEDC ���ذ� */
    }
    else if   (id == UP_PEDC_MORE_DCUS)
    {
        idType = UPTYPE_MORE_DCU;               /* DCU �������� */
    }
    else if ((id >= UP_PEDC_DCU_S) && (id <= UP_PEDC_DCU_E))
    {
        idType = UPTYPE_DCU;                    /* ���� DCU���� */
    }
    else if((id >= UP_PEDC_SLAVES_S) && (id <= UP_PEDC_SLAVES_E))
    {
        if ((id == UP_PEDC_PIC24A) || (id == UP_PEDC_PIC24B))
            idType = UPTYPE_PEDC_PIC24;         /* PIC24A �� B */
        else
            idType = UPTYPE_PEDC_SLAVE;         /* ������չ����оƬ��PIC32 */
    }
    else
    {
        idType = UPTYPE_UNKNOWN;
    }
    
    return idType;
}


//**********************************************************************************************
//                                                                                             *
//                                     Master ��������                                         *
//                                                                                             *
//**********************************************************************************************
/*
  0            1            2            3           4            5
  �ļ�ͷDLE    �ļ�ͷSTX    ˳���SEQ    Դ��ַ      Ŀ���ַ     ��������(HEX)
  &H10         &H02         &H0-FF
*/

//---------------------------------------------------------------------------
// Function:    MMS_Command()
// Description: ��������λ��(MMS)����������
// Parameters:  pCmd    = ����ָ��,
//              pkgSize = ���ݳ���
// Return:      FALSE -- fail
// State:
//---------------------------------------------------------------------------
int8u MMS_Command(int8u *pCmd, int pkgSize)
{
    int8u *p1, *p2;

    int16u wTotLen;
    int16u PackCmd;
    
    int8u ret;
    int8u idType;

    ret = FALSE;

    UpdateMsg.MMSCmd.u8Dst = *(pCmd + 4);
    UpdateMsg.MMSCmd.u8Cmd = *(pCmd + 5);
    UpdateMsg.pData        = (int8u*)FileDataBuf;
    
    #ifdef TEST_BAT_UPDATE_DCU
    if((UpdateMsg.MMSCmd.u8Dst >= UP_PEDC_DCU_S) && (UpdateMsg.MMSCmd.u8Dst <= UP_PEDC_DCU_E))
        UpdateMsg.MMSCmd.u8Dst = UP_PEDC_MORE_DCUS;
    #endif
    
    wTotLen = (*(pCmd + 7)) | ((*(pCmd + 6))<<8);

    //����������Copy�� FileDataBuf
    p1 = pCmd + 8;
    p2 = UpdateMsg.pData;
    while(wTotLen)
    {
        *p2++ = *p1++;
        wTotLen--;
    }

    wTotLen = ChkDataPack(UpdateMsg.pData);
    if(wTotLen < 3)
    {
        return ret;
    }

    /* 
     * ֧�� IBP PSL IBP����������
     * ֧�� �������� DCU
     */
    idType = getUpdateType(UpdateMsg.MMSCmd.u8Dst);

    if(idType != UPTYPE_UNKNOWN)
    {
        if(UpdateMsg.MMSCmd.u8Cmd == MMS_CMD_UPDATE)        //ֻ������������
        {
            GET_TICK(wTickUpdateSection);
            #ifdef  NODATA_ASK_MORE
            u8AskMoreCnt = 0;
            #endif

            PackCmd = *((int16u*)(UpdateMsg.pData) + 1);
            switch(PackCmd)
            {
                case PACK_CMD_UPDATE_DCU:
                {
                    u8UpdateState   = UPFW_STATE_UNKNOWN_CMD;
                    ret             = FALSE;
                    u8StartCommand  = TRUE;
                   
                    wUpFwSection    = 0;
                    
                    if((idType == UPTYPE_MORE_DCU) || (idType == UPTYPE_DCU))
                    {
                        //����������DCU���󣬽��� DCU ��Ч (4)���ô�������DCU����:
                        /*
                         * 0 ~ 1: PACK_PACKAGE_HEAD
                         * 2 ~ 3: PACK_CMD_UPDATE_DCU
                         * 4 ~ 5: ���������ܳ���(= 38)
                         * 6 ~13: ������DCU��־(8�ֽڣ�ÿ��λ��Ӧһ��DCU)
                         * 14   : �´�HEX�ļ���DCU���������ֵ1��ʾ����ȫ���ţ�, 2��ʾ˫������ţ�,����ֵ��Ч
                         * 15   : DCU оƬ���ͣ�0=δ���壬1=PIC30F6010A
                         * 16   : ��ĿУ����Ϣ����(InfoLen, ������19�� ASCII�ַ�)��0��ʾ��У��, ������ҪУ��
                         * 17~35: ��ĿУ����Ϣ(ASCII�ַ���19���ֽ�)��
                         *        �˲������ݽ��� EE_SwDescription ǰ�����Ŀ��Ϣ���бȽϣ��Ƚϳ���Ϊ(InfoLen)
                         *        ��У����Ϣ��ͬ�������������������ܾ�����
                         * 36~37: У���
                         * */
                        /* ����ͷ��Ϣ 8Bytes, ��������֡ͷ��Ϣ 6Bytes */
                        
                        u8UpdateState   = UPFW_STATE_UPDATEFAIL;
                        if(*((int16u*)(UpdateMsg.pData) + 1) >= 38)
                        {
                            setBatchcastUpdateDCUs(pCmd + 8 + 6);
                            if(batchUpdateDCUs)
                                u8UpdateState   = UPFW_STATE_SEC_OK;
                            ret             = true;
                        }
                    }
                }
                break;
                
                case PACK_CMD_START:
                {
                    /* ����յ���������ǰδ�յ������������� */
                    if(!batchUpdateDCUs)
                    {
                        /* ��������DCU����״̬ */
                        int i;
                        for(i=0; i<MAX_DCU; i++)
                        {
                            updateDcuSts[i].state   = UPFW_STATE_NOUPDATE;
                            updateDcuSts[i].section = 0;
                        }
                    }
                    
                    //if(UpdateMsg.u8MstState == MST_STATE_IDLE) //�Ƿ��Ѿ���������״̬, ���򲻴���
                    {                                            //�˴����ж�, ���� MMS ��λ�Ժ��������������������������
                        //UpdateMsg.u8MstState = MST_STATE_START;
                        u8StartUpgrade = TRUE;
                        u8StartCommand = TRUE;
                        ret = TRUE;

                        wUpFwSection = 0;
                    }
                
                    #ifdef TEST_BAT_UPDATE_DCU
                    if(idType == UPTYPE_MORE_DCU)
                    {
                        int8u testSeting[] = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
                        setBatchcastUpdateDCUs(testSeting); 
                    }
                    #endif
                }
                break;

                
                case PACK_CMD_DATA:
                case PACK_CMD_END:
                {
                    u8StartCommand = FALSE;
                    wUpFwSection = *((int16u*)(UpdateMsg.pData) + 3);
                    if(UpdateMsg.u8MstState != MST_STATE_IDLE)
                    {
                        ret = TRUE;
                    }
                }
                break;
            }
            
            /* ��һ���������������ݰ����������� */
            UpdateMsg.u8MstState = MST_STATE_TRANS;
                    
            /* ���� MMS �´����������ݰ�
             * ����û���ж����ݰ��ĳ���
             * if(pkgSize <= UPFW_CODE_BUF_MAX)
           */
            #ifdef BATCH_UPDATE_DCUS
            TgtMemCpy(updateDataPkg, pCmd, pkgSize);
            updatePkgSize   = pkgSize;
            #endif
            if(ret == TRUE)
            {
                /* ������������������ */
                if(idType != UPTYPE_MORE_DCU)
                {
                    clrBatchcastUpdateDCUs();
                }
                
                //�������� PEDC ���ذ壨�� MONITOR��
                if(idType == UPTYPE_PEDC_HOST)
                {
                    /* ����Ҫת������ */
                }
                //�������� DCU  
                else if((idType == UPTYPE_DCU) || (idType == UPTYPE_MORE_DCU))
                {
                    /* Ҫת�����ݸ� DCU */
                }
                //�������� PEDC ��չ�壨�� IBP PSL SIG��
                else if(idType == UPTYPE_PEDC_SLAVE)
                {
                    if((cbUartTxPkg == NULL) || (cbUartTxBusy == NULL) || (cbQuerySlaveSts == NULL) || (cbChkOnline == NULL) || (cbGetSlaveReply == NULL))
                    {
                        /* û�г�ʼ����
                         * ��������ʧ�ܷ��� */
                        u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                        UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                    }
                    else
                    {
                        /* �ж� IBP PSL SIG ����
                         *  */
                        if(cbChkOnline(UpdateMsg.MMSCmd.u8Dst) == FALSE)
                        {
                            /* �����ߣ�
                             * ��������ʧ�ܷ��� */
                            u8UpdateState        = UPFW_STATE_OBJOFFLINE;   //UPFW_STATE_UPDATEFAIL;
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                        }
                    }
                }
                else
                {
                    ret = FALSE;
                    UpdateMsg.u8MstState = MST_STATE_IDLE;
                }

            }
        }
    }
    else
    {
        ret = FALSE;
        UpdateMsg.u8MstState = MST_STATE_IDLE;
    }

    return ret;
}

//---------------------------------------------------------------------------
// Function:    UpdateProcess()
// Description: Master ��������������, ��ѭ������
// Parameters:
// Return:      FALSE -- no result, TRUE -- result saved in u8UpdateState
// State:
//---------------------------------------------------------------------------
int8u UpdateProcess(void)
{
    int8u state;
    int8u Ret;
    int16u wNow,wTmp;
    static int16u wWaitSlave,wIntervals;
    static int8u u8FailTrans=0;

    Ret = FALSE;
    if (UpdateMsg.u8MstState == MST_STATE_IDLE)
        return Ret;

    if(UpdateMsg.MMSCmd.u8Cmd != MMS_CMD_UPDATE)
        return Ret;

    //ָ��ʱ�����ղ�����λ��������, ֹͣ����
    wTmp = MMS_NODATA_TMR;
    #ifdef BATCH_UPDATE_DCUS
    if(batchUpdateDCUs)
    {   /* ��������״̬�£����ӳ�ʱʱ�� */
        wTmp = MMS_NODATA_TMR * 3;
    }
    #endif
    GET_TICK(wNow);
    if((int16u)(wNow - wTickUpdateSection) > wTmp)
    {
        UpdateMsg.u8MstState = MST_STATE_IDLE;
    }

    
    #ifdef  NODATA_ASK_MORE
    //�����������ܣ�MMS��ʾ����ʱ���ĸ��ʱȽϴ�20~25%��������ͨѶ��¼������������MMS�����ݰ�PEDCû�н��յ������ʧ��
    //�޸�˼·: PEDC��������״̬��, �����ĳ��ʱ����û���յ��������ݰ�, ��MMS���������ط���
    else
    {
        if(u8AskMoreCnt == 0)
        {
            u8AskMoreCnt++;
            wIntervals = wTickUpdateSection;
        }
        else
        {
            #ifdef BATCH_UPDATE_DCUS
            if(!batchUpdateDCUs)
            #endif
            {   /* ��������״̬�£��������ط����� */
                wTmp = SYS_TICK_1MS * 2000;
            
                if((WORD)(wNow - wIntervals) > wTmp)
                {
                    wIntervals = wNow;

                    if(u8AskMoreCnt < 100)
                    {
                        u8AskMoreCnt ++;
                        //u8UpdateState = UPFW_STATE_SEC_ERR;     //
                        u8UpdateState = UPFW_STATE_ASKFORDATA;
                        return TRUE;
                    }
                }
            }
        }
    }
    #endif

    int8u idType = getUpdateType(UpdateMsg.MMSCmd.u8Dst);
    
    /* ��������������������� DCU */
    if(*((int16u*)FileDataBuf + 1) == PACK_CMD_UPDATE_DCU)
    {
        if((idType != UPTYPE_MORE_DCU) && (idType != UPTYPE_DCU))
        {
            UpdateMsg.u8MstState = MST_STATE_IDLE;
            Ret = UPFW_STATE_UNKNOWN_CMD;
            return Ret;
        }
    }
    
    #if 1   /* �۵�����: ���� MONITOR */
    if(idType == UPTYPE_PEDC_HOST)
    {
        //=================== ���� Master
        switch(UpdateMsg.u8MstState)
        {
            case MST_STATE_TRANS:
            {
                u8UpdateState = UpFwProcessData((int8u*)FileDataBuf);
                
                UpdateMsg.u8MstState = MST_STATE_START;
                Ret = TRUE;

                /* ���� UpdateMsg.wSlvSection ʵ��û��
                 * ��ֵ���ڻظ�MMSʱ��Я���κŵĵ�4λ���ڹ۲�ͨ�Ž�������
                 *  */
                UpdateMsg.wSlvSection = UpFwState.wCurrentSections;
                if(u8UpdateState != UPFW_STATE_UPDATEOK)
                {
                    UpdateMsg.wSlvSection--;
                }

                /* UPFW_STATE_UPDATEOK ��ʾȫ���������ݰ��Ѿ��ɹ�����
                 *  */
                if(u8UpdateState == UPFW_STATE_UPDATEOK)
                {
                    UPDATE_FIRMWAREStateToMMS();
                    while(chkFinishTxToMMS() == FALSE){;}
                    UpdateMsg.u8MstState = MST_STATE_IDLE;

                    UpFw_InitShowMsg();
                    bios_Finished(&UpFwState,&UpdateTxMsg);
                }
            }
            break;

            default:
                break;
        }
    }
    #endif  /* �۵�����: ���� MONITOR */
    
    #if 1   /* �۵�����: ���� slave (IBP PSL SIG....) */
    else if(idType == UPTYPE_PEDC_SLAVE)
    {
        BOOL resendFlag = FALSE;
        
        switch(UpdateMsg.u8MstState)
        {
            case MST_STATE_TRANS:
                /* 
                 * �� MMS ���͵����ݰ�ԭ��ת�� 
                 * ���ó�ʱʱ�� 200ms
                 * ������ݷ��ͳ���Լ : (115200/10) * 200/1000 = 2304 bytes
                 */
                cbUartTxPkg(updateDataPkg, updatePkgSize);

                GET_TICK(UpdateMsg.wTickOver);
                SET_TIMER_OVER(200);
                UpdateMsg.u8MstState = MST_STATE_SENDING;
                break;
                
            case MST_STATE_SENDING:
                #if 0   /*  TEST only */
                {
                u8UpdateState = UPFW_STATE_SEC_OK;
                UpdateMsg.u8MstState = MST_STATE_START;
                Ret = TRUE;
                break;
                }
                #endif
                
                /* ����������ݰ��Ƿ�����ɣ�FALSE ��ʾ���ڿ��� */
                if(cbUartTxBusy()== FALSE)
                {
                    /* ������� */
                    wWaitSlave  = wNow;
                    u8FailTrans = 0;
                    if(u8StartUpgrade == FALSE)
                    {
                        //SET_TIMER_OVER(2000);
                        SET_TIMER_OVER(MST_DLY_QUERY);
                    }
                    else
                    {
                        u8StartUpgrade = FALSE;
                        SET_TIMER_OVER(2000);
                    }
                    GET_TICK(UpdateMsg.wTickOver);
                    UpdateMsg.u8MstState = MST_STATE_QUERY_D;      //��ʱ���ѯ
                }
                else
                {
                    IF_TIMER_OVER()
                    {
                        /* ���ͳ�ʱ */
                        resendFlag = TRUE;
                    }
                }
                break;

            /* �������ݰ�������ɺ���ʱ��ѯ�ӻ�״̬ */
            case MST_STATE_QUERY_D:
            case MST_STATE_QUERY:
                if(cbGetSlaveReply(UpdateMsg.MMSCmd.u8Dst, &UpdateMsg.u8SlvState, &UpdateMsg.wSlvSection) == FALSE)
                {
                    if(UpdateMsg.u8MstState == MST_STATE_QUERY_D)
                    {
                        IF_TIMER_OVER()
                        {
                            //���Ͳ�ѯ����
                            cbQuerySlaveSts(UpdateMsg.MMSCmd.u8Dst, wUpFwSection);
                            UpdateMsg.u8MstState = MST_STATE_QUERY;
                            GET_TICK(UpdateMsg.wTickOver);
                            SET_TIMER_OVER(200);
                        }
                    }
                    else
                    {
                        IF_TIMER_OVER()
                        {
                            resendFlag = TRUE;
                        }
                    }      
                }
                else
                {
                    //��ѯ�ɹ�, Slave ��״̬
                    u8UpdateState = UpdateMsg.u8SlvState;

                    if(wUpFwSection != UpdateMsg.wSlvSection)
                    {
                        if(wUpFwSection != 0)
                            u8UpdateState = UPFW_STATE_MstSlvSectionErr;
                    }
                    
                    switch(u8UpdateState)
                    {
                        case UPFW_STATE_NOUPDATE:
                        case UPFW_STATE_SEC_FAIL:
                        case UPFW_STATE_UPDATEOK:
                        case UPFW_STATE_UPDATEFAIL:
                        case UPFW_STATE_WAITDOOR:
                        case UPFW_STATE_OBJOFFLINE:
                        case UPFW_STATE_CHECK_FAIL   :      //У��ʧ�ܣ��ܾ�����
                            UpdateMsg.u8MstState = MST_STATE_IDLE;
                            Ret = TRUE;
                            break;

                        case UPFW_STATE_MstSlvSectionErr:
                        case UPFW_STATE_SEC_ERR:
                        case UPFW_STATE_SEC_OK:
                            UpdateMsg.u8MstState = MST_STATE_START;
                            Ret = TRUE;
                            break;

                        case UPFW_STATE_WAITSTATE:
                            u8FailTrans = 0;
                            UpdateMsg.u8MstState = MST_STATE_IDLE;
                            Ret = TRUE;
                            break;


                        case UPFW_STATE_UPDATING:
                        case UPFW_STATE_COPY:
                            wTmp = 3000;
                            goto _IfUpgradeTimerOver_IPS;

                        default:
                            wTmp = 3000;
                        _IfUpgradeTimerOver_IPS:
                            if((int16u)(wNow - wWaitSlave) > wTmp)
                            {
                                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                if(u8UpdateState != UPFW_STATE_WAITSTATE)
                                    u8UpdateState  = UPFW_STATE_UPDATEFAIL;
                                Ret = TRUE;
                            }
                            else
                            {
                                GET_TICK(UpdateMsg.wTickOver);
                                SET_TIMER_OVER(MST_DLY_QUERY);
                                UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //��ʱ���ѯ
                            }
                            break;
                    }
                }
                break;

            case MST_STATE_TRANS_FAIL:
                //transfer again ???
                u8FailTrans = 0;
                UpdateMsg.u8MstState = MST_STATE_IDLE;
                Ret = TRUE;
                break;

            default:
                break;
        }
        
        
        /* �Ƿ���Ҫ���·��� */
        if(resendFlag == TRUE)
        {                            
            u8FailTrans ++;
            if(u8FailTrans >= 3)
            {
                /* ��ѯ��ʱδ�յ��ظ� */
                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                u8UpdateState = UPFW_STATE_UPDATEFAIL;
                Ret = TRUE;
            }
            else
            {
                /* ���� MMS �ط� */
                UpdateMsg.u8MstState = MST_STATE_START;
                u8UpdateState        = UPFW_STATE_SEC_ERR;
                Ret = TRUE;
            }
        }
    }
    #endif  /* �۵�����: ���� slave (IBP PSL SIG....) */
    
    #if 1   /* �۵�����: ���� DCU */
    else if((idType == UPTYPE_DCU) || (idType == UPTYPE_MORE_DCU))
    {
        BOOL    bQueryNextDCU = FALSE;
        int8u   dst_dcu;
        int16u  wTotLen = (updateDataPkg[7]) | ((updateDataPkg[6])<<8);
        
        //=================== ���� slave (DCU)
        switch(UpdateMsg.u8MstState)
        {
            /* ===== step 1. PEDC ������������ �� DCU ===== */
            case MST_STATE_TRANS:                   
            {
                dst_dcu = UpdateMsg.MMSCmd.u8Dst;   /* ��������DCU ID */
                
                #ifdef BATCH_UPDATE_DCUS
                if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                {
                    dst_dcu         = 0;            /* ���������������ݰ�       */
                    
                    /* ��ͷ��ʼ�����Ƿ��п���������DCU  */
                    updateDCUID     = searchFirstUpdateDCUID();
                    if(updateDCUID > MAX_DCU)
                    {
                        /* û�з���������DCU���� */
                        int ii;
                        UpdateMsg.u8MstState    = MST_STATE_IDLE;
                        Ret                     = TRUE;
                        for(ii=0; ii<MAX_DCU; ii++)
                        {
                            updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OFFLINE;
                        }
                        break;
                    }
                    
                    sendUpdatePkg   = TRUE;         /* �������ݰ��ѷ��ͱ�־     */
                }
                else
                #endif
                {
                    if((dst_dcu < ID_DCU_MIN) || (dst_dcu > ID_DCU_MAX))
                    {
                        UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                        u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                        Ret = TRUE;
                        break;
                    }
                }

                /* Ҫת�����ݸ� DCU */
                ClrTransState(FUNID_UPDATE_FIRMWARE);
                CapiStartTrans(FUNID_UPDATE_FIRMWARE, dst_dcu, wTotLen+1, UpdateMsg.pData);
                UpdateMsg.u8MstState = MST_STATE_SENDING;
            }
            break;
                
            /* ===== step 2. �ȴ������������ݵĽ�� ===== */
            case MST_STATE_SENDING:
            {
                state = ChkTransState(FUNID_UPDATE_FIRMWARE);
                switch(state)
                {
                    case fgAbort:
                        //����ʧ��
                        u8FailTrans++;
                        if(u8FailTrans >= 3)
                        {                                                    //���ݴ�������ʧ��
                            u8FailTrans          = 0;
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                            u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                            
                            #ifdef BATCH_UPDATE_DCUS
                            /* PEDC ����ʧ�ܣ���Ӧ����DCU��״̬����һ���� */
                            /* updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                             */
                            #endif

                            Ret = TRUE;
                        }
                        else
                        {
                            //if(ChkSlaveOnLine(UpdateMsg.MMSCmd.u8Dst) == TRUE)
                            {//���ߣ����Ǵ���ʧ�ܣ��ط�
                                #ifdef BATCH_UPDATE_DCUS
                                if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                                {
                                    /* PEDC �Զ��ط� */
                                    UpdateMsg.u8MstState    = MST_STATE_TRANS;
                                    u8UpdateState           = UPFW_STATE_SEC_ERR;
                                    Ret                     = FALSE;
                                    
                                    updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                                }
                                else
                                #endif
                                {
                                    /* ��ǰ�汾����֪ͨ MMS �ط� */
                                    UpdateMsg.u8MstState    = MST_STATE_START;
                                    u8UpdateState           = UPFW_STATE_SEC_ERR;     //�趨 UPFW_STATE_SEC_ERR, MMS ���ط�
                                    Ret                     = TRUE;
                                }
                            }
                            /*
                            else
                            {
                                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                u8UpdateState        = UPFW_STATE_OFFLINE;
                                u8FailTrans          = 0;
                            }
                            */
                        }
                        
                        break;

                    case fgTransOK:
                        //�������
                        wWaitSlave  = wNow;
                        u8FailTrans = 0;
                        GET_TICK(UpdateMsg.wTickOver);
                        if(u8StartUpgrade == FALSE)
                        {
                            SET_TIMER_OVER(MST_DLY_QUERY);
                        }
                        else
                        {
                            u8StartUpgrade = FALSE;
                            SET_TIMER_OVER(500);
                        }
                        UpdateMsg.u8MstState = MST_STATE_QUERY_D;      //��ʱ���ѯ
                        break;

                    case fgRunning:
                    default:
                        break;
                }
            }
            break;

            /* ===== step 3. ��ʱһ��ʱ�䣨�ȴ�DCU�������ѯDCU ===== */
            case MST_STATE_QUERY_D:
            {
                IF_TIMER_OVER()
                {
                    #ifdef BATCH_UPDATE_DCUS
                    if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                    {
                        UpdateQuerySlave(updateDCUID);  
                    }
                    else
                    #endif
                    {
                        UpdateQuerySlave(UpdateMsg.MMSCmd.u8Dst);   /* ��ѯ������DCU  */
                    }
                    
                    UpdateMsg.u8MstState = MST_STATE_QUERY;
                }
            }
            break;

            /* ===== step 4. �ȴ���ѯ��� ===== */
            case MST_STATE_QUERY:
            {
                state = ChkTransState(FUNID_UPDATE_FIRMWARE);
                switch(state)
                {
                    case fgAbort:
                    {
                        //��ѯʧ��
                        int16u dlyTime = 1000;      //2022-10-18 : 200 ��Ϊ 1000
                        
                        if(u8StartCommand)
                        {   /* ֻ�ڷ��������������Ҫ��������ʱ��ֱ���в�ѯ��� */
                            dlyTime = 3000;
                        }
                        
                        ClrTransState(FUNID_UPDATE_FIRMWARE);
                        if((int16u)(wNow - wWaitSlave) > dlyTime)
                        {
                            u8StartCommand = FALSE;
                        
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                            UpdateMsg.u8SlvState = UPFW_STATE_UPDATEFAIL;
                            u8UpdateState = UPFW_STATE_UPDATEFAIL;
                            
                            #ifdef BATCH_UPDATE_DCUS
                            if((ChkSlaveOnLine(updateDCUID) == FALSE))
                            {   //������
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OBJOFFLINE;
                            }
                            else
                            {
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                            }
                            bQueryNextDCU = TRUE;
                            #endif
                            
                            Ret = TRUE;
                        }
                        else
                        {
                            GET_TICK(UpdateMsg.wTickOver);
                            SET_TIMER_OVER(MST_DLY_QUERY);
                            UpdateMsg.u8MstState = MST_STATE_QUERY_D;            //��ʱ�ٲ�ѯ
                        }
                    }
                    break;

                    case fgTransOK:
                        u8StartCommand = FALSE;
                        
                        //��ѯ�ɹ�, �ж� Slave ��״̬
                        ClrTransState(FUNID_UPDATE_FIRMWARE);
                        u8UpdateState = UpdateMsg.u8SlvState;

                        if((u8UpdateState == UPFW_STATE_SEC_ERR) || (u8UpdateState == UPFW_STATE_SEC_FAIL))
                        {
                            if(wUpFwSection != UpdateMsg.wSlvSection)
                            {
                                if(wUpFwSection != 0)
                                    u8UpdateState = UPFW_STATE_MstSlvSectionErr;
                            }
                        }
                        
                        #ifdef BATCH_UPDATE_DCUS
                        if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                        {
                            bQueryNextDCU = TRUE;
                            updateDcuSts[updateDCUID - ID_DCU_MIN].state    = u8UpdateState;
                            updateDcuSts[updateDCUID - ID_DCU_MIN].section  = UpdateMsg.wSlvSection;
                            
                            if((u8UpdateState == UPFW_STATE_UPDATEFAIL) && (ChkSlaveOnLine(updateDCUID) == FALSE))
                            {   //������
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OBJOFFLINE;
                            }
                        }
                        #endif

                        switch(u8UpdateState)
                        {
                            case UPFW_STATE_NOUPDATE:
                            case UPFW_STATE_SEC_FAIL:
                            case UPFW_STATE_UPDATEOK:
                            case UPFW_STATE_UPDATEFAIL:
                            case UPFW_STATE_WAITDOOR:
                            case UPFW_STATE_OBJOFFLINE:
                                
                            case UPFW_STATE_NOSETTING_DCU:      //û����������DCU����
                            case UPFW_STATE_MOTOR_ERR    :      //DCU ���������ƥ��
                            case UPFW_STATE_DISABLE_DCU  :      //DCU δʹ������
                            case UPFW_STATE_DCU_MCU_ERR  :      //DCU оƬ�ͺŲ�ƥ��
                            case UPFW_STATE_CHECK_FAIL   :      //DCU У��ʧ�ܣ��ܾ�����
                                UpdateMsg.u8MstState = MST_STATE_IDLE;
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_MstSlvSectionErr:
                            case UPFW_STATE_SEC_ERR:
                            case UPFW_STATE_SEC_OK:
                                UpdateMsg.u8MstState = MST_STATE_START;
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_WAITSTATE:
                                UpdateMsg.u8MstState = MST_STATE_IDLE;
                                Ret = TRUE;
                                break;
                                
                            case UPFW_STATE_DCU_CONFIGED:
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_UPDATING:
                                if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                                {
                                    Ret = TRUE;
                                    break;
                                }
                            case UPFW_STATE_COPY:
                                wTmp = 3000;
                                goto _IfUpgradeTimerOver;

                            default:
                                wTmp = 3000;
                            _IfUpgradeTimerOver:
                                if((int16u)(wNow - wWaitSlave) > wTmp)
                                {
                                    UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                    if(u8UpdateState != UPFW_STATE_WAITSTATE)
                                        u8UpdateState  = UPFW_STATE_UPDATEFAIL;
                                    Ret = TRUE;
                                }
                                else
                                {
                                    GET_TICK(UpdateMsg.wTickOver);
                                    SET_TIMER_OVER(MST_DLY_QUERY);
                                    UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //��ʱ���ѯ
                                }
                            
                                #ifdef BATCH_UPDATE_DCUS
                                if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
                                {
                                    updateDcuSts[updateDCUID - ID_DCU_MIN].state    = u8UpdateState;
                                }
                                #endif
                                break;
                        }
                        break;

                    case fgRunning:
                    default:
                        break;
                }
            }
            break;

            /* ===== PEDC������������ʧ�ܺ�Ĵ��� ===== */
            case MST_STATE_TRANS_FAIL:
                {
                    u8FailTrans             = 0;
                    UpdateMsg.u8MstState    = MST_STATE_IDLE;
                    Ret                     = TRUE;
                }
                break;

            default:
                break;
        }
        
        #ifdef BATCH_UPDATE_DCUS
        if(batchUpdateDCUs)                 /* �Ƿ�ʹ������������ */
        {
            if(Ret == TRUE)
            {
                /* ���� DCU �Ѿ��н��
                 * �ж��Ƿ���Ҫ��ѯ��һ��
                 *  */
                if(bQueryNextDCU == TRUE)
                {
                    Ret = FALSE;

                    /* ��Ҫ������ѯ��һ�� DCU���Ƿ�ȫ��DCU�Ѿ���ѯ���?
                     *  */
                    int8u nextDCU;
                    for(nextDCU = updateDCUID + 1; nextDCU<=MAX_DCU; nextDCU++)
                    {
                        /* ��DCU ������Ч��������
                         * ChkSlaveOnLine �Ĳ����� DCUID ��1��ʼ  */
                        if((ChkRamBit(updateDCUSetting, nextDCU - 1) != 0))
                        {
                            if((ChkSlaveOnLine(nextDCU) == TRUE))
                                break;
                            else
                            {   /* ��Ҫ������DCU�������� */
                                updateDcuSts[nextDCU - ID_DCU_MIN].state    = UPFW_STATE_OFFLINE;
                            }
                        }
                    }
                    
                    if(nextDCU > MAX_DCU)
                    {
                        /* ȫ��DCU����ѯ��� */
                        UpdateMsg.u8MstState    = MST_STATE_START;//MST_STATE_IDLE;
                        Ret                     = TRUE;
                    }
                    else
                    {
                        /* ������ѯ��һ�� DCU */
                        updateDCUID = nextDCU;
                        
                        /* ���ǵ�����DCU������ٶȻ���һ�£������������õ���ʱ��ѯʱ��Ƚ϶̣����������������̺�ʱ */
                        GET_TICK(UpdateMsg.wTickOver);
                        SET_TIMER_OVER(50);;                              //2022-10-18 : 5 ��Ϊ 50
                        UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //��ʱ���ѯ
                        
                        //2022-8-6 ����������ѯ����6��DCU�ͳ�ʱ��
                        //���ͳɹ�--> ��ʱԼ 60ms --> ��ѯ����DCU����ѯ����6��DCUʱ������2��DCU��ʱû�лظ�
                        //  �����޸ĺ�ÿ��DCU�������㳬ʱ
                        if(batchUpdateDCUs)
                        {
                            wWaitSlave  = wNow;
                        }
                    }
                }
            }
        }
        #endif
    }
    #endif  /* �۵����룺����DCU */

    return Ret;
}

//---------------------------------------------------------------------------
// Function:    UpdateQuerySlave
// Description: ��ѯ Slave ״̬
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
static void UpdateQuerySlave(int8u id)
{
    ClrTransState(FUNID_UPDATE_FIRMWARE);
    
    CmdBuf[0] = UPFW_CMD_QUERY;
    CmdBuf[1] = 0x55;
    CmdBuf[2] = 0xAA;
    CmdBuf[3] = wUpFwSection;
    CapiTransExtCmd(FUNID_UPDATE_FIRMWARE, id, 5, CmdBuf);   //ע�����ݳ���Ҫ��һ���ֽ����� Checksum
}

//---------------------------------------------------------------------------
// Function:    UpdateCmdFromSlave
// Description: Master ���� Slave �ķ�����Ϣ
// Parameters:  ��Ϣ pMsg
// Return:      TRUE -- ��Ϣ��Ч
// State:
//---------------------------------------------------------------------------
int8u UpdateCmdFromSlave(tCdrvMsg * pMsg)
{
    int8u u8ExtCmd;
    int8u u8CS;
    #if 0 
    /* �� DCU V1.7�����汾��ǰ2���ֽ��� 5A 5A 
     *        V1.8  �޸ĺ�  ǰ2���ֽ���DCU��ǰ��Ҫ�������κ�(���ֽ���ǰ)
     */
    if((pMsg->m_bData[1] != 0x5A) && (pMsg->m_bData[2] != 0x5A))
    {
        return FALSE;
    }
    #endif

    u8CS = CalcChkSum(&pMsg->m_bData[0], pMsg->m_bSize-1);
    if(pMsg->m_bData[pMsg->m_bSize-1] != u8CS)                 //У�� checksum
    {
        return FALSE;
    }

    u8ExtCmd = pMsg->m_bData[0] & (~TRANS_DIR);

    switch(u8ExtCmd)
    {   
        /* DCU�ظ�������״̬��ѯ�����￼���ó��ȼ��ݾɰ棬�°��DCU�ķ������ݵĳ���
         *         0      1     2     3    4     5      6    7
         *V1.8ǰ   extcmd 5A    5A    ״̬ �κ�L cs     -    -
         *V1.8���� extcmd �κ�L �κ�H ״̬ �κ�L cs     -    -
         * ��      extcmd ����  �汾  ״̬ �κ�L �κ�H  ���� cs   ��ʵ�ֹ���
         *  */
        case UPFW_CMD_QUERY:
            if(pMsg->m_bSize == 6)
            {
                UpdateMsg.u8SlvState    = pMsg -> m_bData[3];
                if((pMsg->m_bData[1] == 0x5A) && (pMsg->m_bData[2] == 0x5A))
                {   /* V1.8 ǰ */
                    UpdateMsg.wSlvSection   = pMsg -> m_bData[4];
                }
                else
                {   /* V1.8 ���� */
                    UpdateMsg.wSlvSection = (pMsg->m_bData[1]) + (pMsg->m_bData[2] << 8);
                }
                
                /*
                 *����ԭ����汾��CalcChkSum,����Ϊ int8u ;
                 *�°汾�ĳ���Ϊ�� int16u;Ϊ�����ϰ汾�ģ�����
                 *����ǰ�ȶ�ȡ�ó��ȷ�����PEDC����PEDC����Ӧ����
                 *      CalcChkSumLEN;  == 0 16λ���� != 0 8λ��
                 * �� DCU HEX �ֶ���������254ʱ��������������� !!!!!!
                 */
                #if 0
                if((wUpFwSection == 0) && (UpdateMsg.u8SlvState == UPFW_STATE_SEC_OK))
                {
                    if(pMsg -> m_bData[4] == 0xfe)
                    {
                        CalcChkSumLEN = 0;
                    }
                    else
                    {
                        CalcChkSumLEN = 0xfe;
                    }
                }
                #endif
            }
            else if(pMsg->m_bSize == 7)
            {
                /* ��ʵ�ֹ��� */
            }
            break;

        default:
            break;
    }

    return TRUE;
}

#endif //#if ((UPDATE_FUN_EN == TRUE))

