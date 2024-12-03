/*******************************************************************************
 * PIC24������������ģ��
 * -> upPIC24_InitStart     : ����PIC24 ����
 * -> chkIsUpdatingPic24    : �ж��Ƿ� PIC24 ��������״̬
 * 
 * -> Pic24UpdateProcess    : PIC24 ��������״̬ʱ���������������������     
 * -> upPIC24_ProcP24Reply  : ������յ�����֡�����������PIC24�ظ����ݺ����  
 * 
 * -> upPIC24_SendMsgToMMS  : MMS   ͨ��ģ���ṩʵ�֣����� ������Ϣ��MMS
 * -> upPIC24_SendToPic24   : PIC24 ͨ��ģ���ṩʵ�֣����� ������Ϣ��PIC24
 * -> upPIC24_ResetChip     : PIC24 ͨ��ģ���ṩʵ�֣�֪ͨӦ�ò㼴������PIC24
 *  
 *******************************************************************************/
#include "Include.h"

#include "MonCommCfg.h"
#include "uartDrv.h"

#include "IPSUart.h"
#include "sysTimer.h"

#include "P24UpdateDrv.h"
#include "P24UpdateIF.h"

#undef  PRIVATE
#define PRIVATE

/* =============================================================================
 * �������� */
#if (UART_COM_RXBUFSIZE < MAX_PACKET_SIZE)
#error "UART_COM_RXBUFSIZE must be larger than MAX_PACKET_SIZEQ"
#endif

/* ������¼�հ׵� FLASH
 * ���Է����������ɹ���PIC24 �ظ�У����󣩣��ݲ����ô˶��� */
/* #define SKIP_BLANK_FLASH */

/* �������� - ϸ�ֲ��� for updateStep
 *  */
#define P24_STEP_INIT           0
#define P24_STEP_SETOBJ         10
#define P24_STEP_TX2P24         20
#define P24_STEP_WAITRX         30
#define P24_STEP_REPLY_OK       100
#define P24_STEP_WAIT_REPLYMMS  101

/* ��������
 *  */
#define UPCMD_REPEAT_INTERVAL   200     /* �ظ�ĳ�������ʱ���� */
#define UPCMD_REPEAT_TIMES      3       /* �ظ�ĳ������������� */
#define UPCMD_TRY_VERIFY_TIMES  3       /* ����У������������ */

#define BL_ERR_UNDEFCMD         0x00    /* δ�������� */
#define BL_ERR_ENTRANCE         0x01    /* PIC24��ڴ���оƬ��λ�޷����� Bootloader�� */
#define BL_ERR_CODE             0x02    /* Bootloader ������󣨲����ڣ� */
#define BL_ERR_KEYWORDS         0x03    /* APP����ؼ��ֲ�ƥ����� */
#define BL_VERIFY_OK            0xFF    /* У��ͨ��������Bootloader */



/* ����״̬����ʱ��  ( �� )
 * ʵ�⣺����һ�� оƬ��Լ��Ҫ 20s(57600bps,  UPDATE_INTERVAL_MS = 10ms)
 *  */
#define UPDATE_MAX_TIME_SEC     40     

/* FLASH ��ַ���ֵ�ַ��ת���� BIN �ļ���ַ���ֽڵ�ַ��
 *  */
#define FLASH_TO_BIN_ADDR(adr)  ((int8u *)(P24_CodeBin + (adr << 1)))

/* =============================================================================
 * 
 * PIC24 �Ĵ��루�������ļ���
 * 
 * �� Bootloader �� Ӧ�ò����ƴ��
 * �Ƚ� Bootloader �� app �� HEX �ļ�ת���� BIN �ļ�
 * Ȼ���ȡ 
 *   Bootloader ��ǰ 8 ���ֽ� +
 *   app        ���ж�����    +
 *   Bootloader �Ĵ���        +
 *   app        �Ĵ���
 * �ϲ���ת���� "p24BL_App.c"
 *  */
#pragma pack(4)
PRIVATE const unsigned char P24_CodeBin[] = 
{
    #include "p24BL_App.c"
};

/* =============================================================================
 * ���ر��� */

/* ��������ONLY COMMOBJ_IORW_A or COMMOBJ_IORW_B */
PRIVATE tEnmCommObj updatePic24Obj;   

/* �������̿��� */
PRIVATE int8u           updateInitiator;        /* ����������               */
PRIVATE int8u           updateMode;             /* ����ģʽ  */
    
PRIVATE enmUpPic24Step  updatePic24Sts;         /* ����״̬                 */
PRIVATE int8u           updateStep;             /* ÿ��״̬�µĹ��̲���     */
PRIVATE int8u           updateRepeat;           /* ���������ظ�����         */
PRIVATE int8u           verifyErrCnt;           /* ��������У����������   */
PRIVATE tSYSTICK        updateStepTick;         /* �������̲����ʱ��       */
    
PRIVATE tSYSTICK        updateP24_StartTick;    /* ��������״̬����ʼʱ��   */
    
PRIVATE tDWORD          flashAddr;              /* ��д FLASH �ĵ�ַ������洢�ռ䰴��Ѱַ */

PRIVATE tFlashInfo hexInfo;                     /* �����������HEX��Ϣ */

/* �յ��� PIC24 ����Ϣ���� 
 * pic24ReplyBuf �е����ݲ���������֡��ͷ��Ϣ
 */
PRIVATE int16u      pic24ReplyLen;
PRIVATE int8u       pic24ReplyBuf[MAX_PACKET_SIZE];

/* ���͵� PIC24 ����Ϣ���� */
PRIVATE int16u      sendPic24Len;
PRIVATE int8u   __attribute__((aligned(4))) sendPic24Buf[MAX_PACKET_SIZE];

PRIVATE int8u       pic24BLRunning[2];      /* 0=PIC24����Ӧ�ò㣬�����ʾPIC24����Bootloader */

/* ���͵� MMS ����Ϣ���� */
PRIVATE int16u      sendMMSLen;
PRIVATE tPktHeader  sendPkgHeader;
PRIVATE int8u   __attribute__((aligned(4))) sendMMSBuf[MAX_PACKET_SIZE];

/* ���ڹ��ܽӿ� */
PRIVATE tUartComm *pP24UpComm;


/* =============================================================================
 * �������� PIC24 ����ͨ�Žӿں���
 *  */
extern BOOL upPIC24_SendMsgToMMS(int8u msgType, int8u srcNode, int8u * data, int16u dataLen);
extern BOOL upPIC24_SendToPic24(int8u msgType, int8u txDstNode, int8u * txData, int16u txDataLen);
extern BOOL upPIC24_ResetChip(tEnmCommObj updatePic24Obj);

/* �� weak ���壬���Ա����ⲿû���ṩ����ʵ��ʱ������� */
//BOOL __attribute__((weak)) upPIC24_ResetChip(tEnmCommObj updatePic24Obj);

/* =============================================================================
 * �������� PIC24 Ӧ�ú���
 *  */

/* 
 * �Ƿ������� PIC24 ��״̬
 * ���� TRUE ��ʾ������
 */
PUBLIC BOOL chkIsUpdatingPic24(void)
{
    BOOL ret = FALSE;
    if(updatePic24Sts != UP_PIC24_NULL)
    {
        ret = TRUE;
    }
    
    return ret;
}


/* =============================================================================
 * �������ݸ� PIC24 
 * ����
 *  msgType         : ��Ϣ����
 *  updatePic24Obj  : ����ȫ�ֱ���������
 *  sendPic24Buf    : ����ȫ�ֱ��������������� 
 *  sendPic24Len    : ����ȫ�ֱ��������������ݳ���
 * ���ط���״̬    
 *  TRUE ���ڳɹ�
 */
PRIVATE BOOL sendToPic24Package(int8u msgType)
{
    BOOL   ret = upPIC24_SendToPic24(msgType, updatePic24Obj, sendPic24Buf, sendPic24Len);
    return ret;
}

/* =============================================================================
 * �������ݸ� MMS 
 * ����
 *  OBJ_CMD_BL_CTRL : ����
 *  COMMOBJ_MONITOR : ����
 *  sendMMSBuf      : ����ȫ�ֱ��������������� 
 *  sendMMSLen      : ����ȫ�ֱ��������������ݳ���
 * ���ط���״̬    
 *  TRUE ���ڳɹ�
 */
PRIVATE BOOL sendToMMSPackage(void)
{
    BOOL   ret = FALSE;
    if(sendMMSLen > 0)
    {
        ret = upPIC24_SendMsgToMMS(OBJ_CMD_BL_CTRL, COMMOBJ_MONITOR, sendMMSBuf, sendMMSLen);
        sendMMSLen = 0;
    }
    
    return ret;
}

/* =============================================================================
 * ���� PIC24������ PIC24 �ظ�������֡
 * ���� 
 *  msgType         : ��Ϣ����
 *  rxData          : ��Ϣ����
 *  rxDataLen       : ��Ϣ���ݳ���
 * ����
 *  TRUE ���յ�ָ�����������֡
 */
PUBLIC BOOL upPIC24_ProcP24Reply(int8u msgType, int8u *rxData, int16u rxDataLen)
{
    /* ���ؽ�� */
    BOOL rxMsgSts = FALSE;
    
    if(rxDataLen > 0)
    {
        /* ����PIC24�ظ�����Ϣ���� */
        pic24ReplyLen = rxDataLen;
        memcpy(pic24ReplyBuf, rxData, rxDataLen);

        /* �ӻ��ظ���������
         *  */
        switch((tEnmCommObjCmd)(msgType))
        {     
            /* ====================================
             * Monitor-PIC32 �� PIC24Ӧ�ò��ͨ�� 
             */
            case OBJ_CMD_BL_CTRL:
                switch(pic24ReplyBuf[0])
                {
                    /* PIC24 �ظ�����Checksum��Ϣ�� */
                    case PIC24APP_CMD_GET_CS:
                        if((updatePic24Sts == UP_PIC24_INIT) && (updateStep == P24_STEP_WAITRX))
                        {
                            updateStep = P24_STEP_REPLY_OK;
                            rxMsgSts   = TRUE;
                        }
                        break;

                    /* �˴��յ�����ʾ�޷����� Bootloader */
                    case PIC24APP_CMD_ENTER_BL:
                        if((updatePic24Sts == UP_PIC24_START_BL) && (updateStep == P24_STEP_WAITRX))
                        {
                            if((pic24ReplyBuf[1] == 0x55) && (pic24ReplyBuf[2] == 0xAA))
                            {
                                /* pic24ReplyBuf[3] ���ǻظ���Ϣ */
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                        break;
                        
                        
                    default:
                        break;
                }
                break;

            /* ====================================
             * Monitor-PIC32 �� PIC24 Bootloader��ͨ�� 
             */
            case OBJ_CMD_BOOTLOADER:
                switch(pic24ReplyBuf[0])
                {
                    /* Bootloader �Ѿ�����
                     * 
                     * PIC24APP_CMD_ENTER_BL ��PIC32����PIC24Ӧ�ò�����
                     * PIC24 ��λ��������Bootloader�Ժ�ظ�
                     */
                    case PIC24APP_CMD_ENTER_BL:
                        if((updatePic24Sts == UP_PIC24_START_BL) && (updateStep == P24_STEP_WAITRX))
                        {
                            if((pic24ReplyBuf[1] == 0x55) && (pic24ReplyBuf[2] == 0xAA) && (pic24ReplyBuf[3] == BL_VERIFY_OK))
                            {
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                    break;

                    /* PIC24 Bootloader �ظ����� FLASH �Ľ��
                     * �� ���͵Ĳ������������ͬ( Blocks, addrȫ����0 )
                     */
                    case ER_FLASH:
                        if(updatePic24Sts == UP_PIC24_ERASE)
                        {
                            if(pic24ReplyBuf[1] == ER_FLASH_BLOCKS)
                            {
                                rxMsgSts   = TRUE;
                                updateStep = P24_STEP_REPLY_OK;
                            }
                        }
                        break;

                    /* PIC24 Bootloader �ظ�д�� FLASH �Ľ��
                     */
                    case WT_FLASH:
                        if(updatePic24Sts == UP_PIC24_WRITE)
                        {
                            rxMsgSts   = TRUE;
                            updateStep = P24_STEP_REPLY_OK;
                        }
                        break;

                    /* PIC24 Bootloader �ظ���ȡ FLASH �Ľ��
                     */
                    case RD_FLASH:
                        if(updatePic24Sts == UP_PIC24_VERIFY_WR)
                        {
                            rxMsgSts   = TRUE;
                            updateStep = P24_STEP_REPLY_OK;
                        }
                        break;


                    case RD_VER:                /* PIC24 Bootloader �ظ��� �汾��Ϣ �Ľ�� */
                    case VERIFY_OK:             /* PIC24 Bootloader �ظ�  У��OK */
                    case (VERIFY_OK + 0x80):    /* PIC24 Bootloader �ظ�  У��OK */
                        rxMsgSts   = TRUE;
                        updateStep = P24_STEP_REPLY_OK;
                        break;

                }
                break;
        }
    }
}

/* =============================================================================
 * PIC32 �� PIC24 Ӧ�ò�ͨ��Э�����
 *  Initiator : �����������ߣ����ô˺�����ԴĿ�� ���磬 MMS ���� PIC24���� ��
 *  upMode    : ����ģʽ
 *  info      : �����������HEX��Ϣ����ʱ����δ��
 *   ��ʼ��������ѯ����
 * ���� TRUE  ��ʾ�ɹ�����
 *      FALSE ��ʾ�Ѿ�������������ʧ��
 */
PUBLIC BOOL upPIC24_InitStart(int8u Initiator, int8u upMode, tFlashInfo *info)
{
    BOOL ret = FALSE;
    
    if((upMode != UPMODE_AUTO) && (upMode != UPMODE_FORCE) && (upMode != UPMODE_BYCMD))
    {
        /* ��֧�ֵ�����ģʽ */
        sendMMSBuf[0] = updatePic24Obj;                 /* ���� */
        sendMMSBuf[1] = TOMMS_PIC24_ERRORMODE;          /* ����ģʽ���� */
        sendMMSLen    = 2;
    }
    else
    {
        if(updatePic24Sts == UP_PIC24_NULL)
        {
            int32u *pBin = (int32u *)P24_CodeBin;
            
            if(upMode == UPMODE_BYCMD)
            {
                hexInfo = *info;
            }

            /* FLASH ��ַ0����һ����תָ����� Bootloader ��ִ�д���
             * FLASH ��ַ1��ָ��оƬ��λ��ĵ�һ��ָ���ַ
             * ��� BIN �ļ���С�������ԣ�
             *  */
            if((sizeof(P24_CodeBin) == (P24_FLASH_SIZE_WORD <<1)) && (pBin[0] == 0x00040400L) && (pBin[1] == 0x00000000L))
            {
                ret = TRUE;
                updateInitiator = Initiator;
                updateMode      = upMode;

                updateStep          = P24_STEP_INIT;
                updatePic24Sts      = UP_PIC24_INIT;
                updatePic24Obj      = COMMOBJ_IORW_A;
                updateP24_StartTick = getSysTick();
            }
            else
            {
                sendMMSBuf[0]   = updatePic24Obj;               /* ���� */
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_BINFILE;
                sendMMSLen      = 2;
            }
        }
        else
        {
            if(updatePic24Sts >= UP_PIC24_UPDATE_INI)
            {
                sendMMSBuf[0] = updatePic24Obj;                 /* ���� */
                sendMMSBuf[1] = TOMMS_PIC24_UPDATING;           /* �Ѿ����������� */
                sendMMSBuf[2] = flashAddr.v[0];
                sendMMSBuf[3] = flashAddr.v[1];
                sendMMSBuf[4] = flashAddr.v[2];
                sendMMSBuf[5] = (int8u)(P24_FLASH_SIZE_WORD >> 0);
                sendMMSBuf[6] = (int8u)(P24_FLASH_SIZE_WORD >> 8);
                sendMMSBuf[7] = (int8u)(P24_FLASH_SIZE_WORD >> 16);
                sendMMSLen    = 8;
            }
            else
            {
                /* �� upPIC24_App_QueryInfo() �лظ� TOMMS_PIC24_INIT
                 */
            }
        }
    }
    
    /* 
     * �ظ� MMS
     */
    sendToMMSPackage();
    
    return ret;
}

/* =============================================================================
 * PIC32 �� PIC24 Ӧ�ò�ͨ��Э�����
 *   ��ѯ����
 *   PIC32 ����  : ��ѯ PIC24 APP��Ϣ����Checksum��Ϣ��
 *                 У����Ϣ����Ҫ������������һ���������˳�
 *   PIC32 ����  : ���� Bootloader
 */
PRIVATE void upPIC24_App_QueryInfo(void)
{
    tSYSTICK    tickNow;
    tickNow = getSysTick();

    switch(updateStep)
    {
        /* ��ʼ�� */
        case P24_STEP_INIT:
            /* 
             * �ظ� MMS: 
             * ������ʼ�������У�����չ��Ϣ���������壩
             */
            sendMMSBuf[0]   = updatePic24Obj;
            sendMMSBuf[1]   = TOMMS_PIC24_INIT;
            sendMMSLen      = 2;

            /* ��¼��ʼ��ʱ��� */
            updateP24_StartTick = getSysTick();
            updateRepeat        = 0;
            updateStep          = P24_STEP_SETOBJ;

        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;

        /* �� PIC24 ���������Ϣ������ PIC24 ����
         * 10 02 00 C8 CC  F1  00 01  01  10 03
         */
        case P24_STEP_TX2P24:
            sendPic24Buf[0] = PIC24APP_CMD_GET_CS;
            sendPic24Len    = 1;
            
            if(sendToPic24Package(OBJ_CMD_BL_CTRL) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;

        /* �ȴ� PIC24 �ظ����� PIC24 ���������Ϣ */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* ��ʱδ�ظ����ظ����� '�� PIC24 ���������Ϣ'
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * ���ﲻ�ظ� MMS (��ʼ������ʧ��)
                     * ���� PIC24 ֻ�� Bootloader �����У�����Ӧ PIC24APP_CMD_GET_CS
                     */
                    #if 0
                    sendMMSBuf[0]   = updatePic24Obj;           /* ���� */
                    sendMMSBuf[1]   = TOMMS_PIC24_NOAPP;        /* ʧ�ܴ��� */
                    sendMMSBuf[1]   = TOMMS_PIC24_FAIL_INIT;    /* ʧ�ܴ��� */
                    sendMMSLen      = 2;
                    updatePic24Sts = UP_PIC24_FINISHED;         /* ���� */
                    
                    #else
                    updateStep      = P24_STEP_INIT;
                    updatePic24Sts  = UP_PIC24_START_BL;        /* �������� Bootloader */
                    
                    #endif
                }
            }
            break;

        /* �յ�PIC24APP�ظ��汾��Ϣ "PIC24APP_CMD_GET_CS" �Ժ�
         * upPIC24_ProcP24Reply() ������ֵ updateStep  = P24_STEP_REPLY_OK
         *  */
        case P24_STEP_REPLY_OK:
            {
                /* PIC24�ظ���У����Ϣ */
                tFlashInfo rxInfo;
                memcpy((int8u*)&rxInfo, (int8u*)(pic24ReplyBuf + 1), sizeof(rxInfo));

                
                BOOL runUpdate = FALSE;
                if(updateMode == UPMODE_AUTO)
                {
                    /* PIC32 ����PIC24 BIN ��У����Ϣ */
                    getBinCodeInfo(&hexInfo);
                    
                    if(memcmp(&rxInfo, &hexInfo, sizeof(tFlashInfo)) == 0)
                    {
                        /* �汾��Ϣ��ȫ��ͬ�������µİ汾 */
                        sendMMSBuf[1]   = TOMMS_PIC24_UPDATED;
                        sendMMSBuf[2]   = UP_PIC24_FINISHED;
                    }
                    else
                    {
                        /* ��Ϣ����ͬ
                         * �жϣ���Ӧ�ò�У��Ͳ�һ�£������������� */
                        if(rxInfo.onlyAppCS == hexInfo.onlyAppCS)
                        {
                            /* APP У�����ͬ���ظ� MMS Bootloader��Ϣ 
                             * ��ʱ Bootloader ����ͬ
                             */
                            sendMMSBuf[1]   = TOMMS_PIC24_UPDATED_APP;
                            sendMMSBuf[2]   = UP_PIC24_FINISHED;
                        }
                        else
                        {
                            runUpdate = TRUE;
                            
                            /* ������һ�����̣����� PIC24 Bootloader */
                            updateStep      = P24_STEP_INIT;
                            updatePic24Sts  = UP_PIC24_START_BL;

                            /* �ظ�MMS ������ʼ���ɹ� */
                            sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;
                            sendMMSBuf[2]   = UP_PIC24_INIT;
                        }
                    }
                }
                else if(updateMode == UPMODE_FORCE)
                {
                    runUpdate = TRUE;
                }
                else if(updateMode == UPMODE_BYCMD)
                {
                    /* reserved for future */
                }
                
                /* �ж��Ƿ���ͬ 
                 * һ�� PIC24 ��Ϣ�� PIC32���õ�PIC24-HEX ��Ϣ����������Ҫ��������
                 */
                if(runUpdate == FALSE)
                {
                    updatePic24Sts  = UP_PIC24_FINISHED;        /* ���� */
                }
                else
                {
                    /* ������һ�����̣����� PIC24 Bootloader */
                    updateStep      = P24_STEP_INIT;
                    updatePic24Sts  = UP_PIC24_START_BL;

                    /* �ظ�MMS ������ʼ���ɹ� */
                    sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;
                    sendMMSBuf[2]   = UP_PIC24_INIT;
                }
                
                sendMMSBuf[0]   = updatePic24Obj;           /* ���� */
                memcpy(&sendMMSBuf[3], (int8u*)&rxInfo, sizeof(rxInfo));
                sendMMSLen      = 3 + sizeof(rxInfo);
            }
            break;
    }
    
    
    /* 
     * �ظ� MMS
     */
    sendToMMSPackage();
}


/* =============================================================================
 * ���� PIC24 -- 
 * PIC32 �� PIC24 Ӧ�ò�ͨ��Э�����
 *   PIC32 ����  : ���� PIC24 Bootloader
 */
PRIVATE void upPIC24_App_StartBL(void)
{
    tSYSTICK    tickNow;

    tickNow = getSysTick();

    sendPic24Len = 0;
    sendMMSLen   = 0;
    switch(updateStep)
    {
        /* ��ʼ�� */
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            break;

        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;
        
        /* �������� PIC24-APP ����Bootloader, eg.
         * 10 02 00 C8 CC F1 00 04 50 55 AA FF 10 03
         */
        case P24_STEP_TX2P24:
            sendPic24Buf[0] = PIC24APP_CMD_ENTER_BL;
            sendPic24Buf[1] = 0x55;
            sendPic24Buf[2] = 0xAA;
            sendPic24Buf[3] = 0xFF;
            sendPic24Len    = 4;
            
            #if (APP_KEYWORDS_LEN > 0)
            /* Ҫ�����ؼ���ƥ�� 
             * �˹ؼ����� PIC24 APP ����Ŀ��Ϣ���ɺ궨�� PROJECT_INFOS ��ȷ��
             * �� "PEDC-PIC24: "
             * ע�� �� FLASH �洢�����ݲ��������ģ�����ֱ�� copy����
             * memcpy(&sendPic24Buf[4], FLASH_TO_BIN_ADDR(APP_PRJINFO_ADDR), APP_KEYWORDS_LEN);
             */
            int8u *pInfo;
            int8u *pChk;
            int8u chkLen = APP_KEYWORDS_LEN;
            
            pChk  = &sendPic24Buf[4];
            pInfo = FLASH_TO_BIN_ADDR(APP_PRJINFO_ADDR);
            for(chkLen=0; chkLen<APP_KEYWORDS_LEN; )
            {
                *pChk++ = *pInfo ++;    /*      */
                *pChk++ = *pInfo ++;
                pInfo  += 2;            /* ����� */
                chkLen += 2;
            }
            sendPic24Len += APP_KEYWORDS_LEN;
            #endif
            
            if(sendToPic24Package(OBJ_CMD_BL_CTRL) == TRUE)
            {
                updateStepTick = tickNow;
                updateStep = P24_STEP_WAITRX;
            }
            break;
        
        /* �ȴ� PIC24 �ظ�������Bootloader 
         * PIC24-APP �յ�������Bootloader���󣬲��ظ�����ʱ��λ���� Bootloader ģʽ
         * PIC Bootloader ģʽ�£��ԡ�����Bootloader��������Ӧ��
         * ����������Ҫ��2�Ρ�����Bootloader�������1����Ӧ�ò㴦��1����Bootloader����
         * ���ǵ�оƬ��λ����ʼ����ʱ�䣬����ʱ����Ҫ����������������ʱ����
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= 1000)
            {
                /* ��ʱδ�ظ����ظ����� '����Bootloader'
                 */
                updateRepeat++;
                if(updateRepeat < 5)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * ��Ҫ�ظ� MMS������Bootloaderʧ��
                     */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_BL_OVTM;       /* ʧ�ܴ��� */
                    sendMMSLen = 2;

                    updatePic24Sts = UP_PIC24_FINISHED; /* ���� */
                }
            }
            break;
            
        /* �յ� PIC24 �ظ�
         * �� upPIC24_ProcP24Reply() ����"PIC24APP_CMD_ENTER_BL"������ֵ updateStep  = P24_STEP_REPLY_OK
         *  */
        case P24_STEP_REPLY_OK:
            if((pic24ReplyBuf[3] != BL_VERIFY_OK))
            {
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_BL_CODE;     /* ������   */
                sendMMSBuf[2]   = pic24ReplyBuf[3];             /* �������  */
                sendMMSLen      = 3;
                updatePic24Sts = UP_PIC24_FINISHED;             /* ���� */
            }
            else// if(pic24ReplyBuf[3] == BL_VERIFY_OK)
            {
                /* �յ�PIC24 Bootloader�ظ�����ʾ�Ѿ����� Bootloader */
                sendMMSBuf[1]   = TOMMS_PIC24_INIT_OK;          /* �Ѿ����� Bootloader */
                sendMMSBuf[2]   = UP_PIC24_START_BL;
                sendMMSLen      = 3;

                /* ������һ�����̣� ��ʼ������ FLASH */
                updatePic24Sts = UP_PIC24_UPDATE_INI;
            }
            break;
    }
    
    /* 
     * �ظ� MMS
     */
    sendMMSBuf[0]   = updatePic24Obj;               /* ���� */
    sendToMMSPackage();
}

/* =============================================================================
 * ���� PIC24 -- 
 * PIC32 �� PIC24 Bootloader ͨ��Э�����
 *   PIC32 ����  : ���� Flash
 * һ�β��� �ж������� �� ����Ӧ���� FLASH
 * RESET��ַ��RESET����������Bootloader ���򱻱���
 */
PRIVATE void upPIC24_BL_EraseFlash(void)
{
    tSYSTICK    tickNow;

    tickNow = getSysTick();

    sendPic24Len = 0;
    switch(updateStep)
    {
        /* ��ʼ�� */
        case P24_STEP_INIT:
            flashAddr.Val   = 0;
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            break;

        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
            break;
        
        /* ��������� PIC24 Bootloader : ���� FLASH 
         * eg. 10 02 00 C8 CC F0 00 05 03 15 00 00 00 10 03 E6
         *  */
        case P24_STEP_TX2P24:
            /* cmd, Len, addrL, addrM, addH */
            sendPic24Buf[0] = ER_FLASH;         /* erase ���� */
            sendPic24Buf[1] = ER_FLASH_BLOCKS;  /* �������� FLASH ҳ������Block�飩*/
            sendPic24Buf[2] = 0x00;             /* ����������ʼҳ��ַ��3�ֽ� */
            sendPic24Buf[3] = 0x00;
            sendPic24Buf[4] = 0x00;
            sendPic24Len    = BL_HEADER_LEN;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* �ȴ� PIC24 Bootloader �ظ��������
         * ���ݹٷ����ϣ�
         *  Chip Erase Time         : min = 400ms
         *  Page Erase Time         : min = 40ms
         *  Row Programming Time    : min = 2ms
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= 1000)    /* 1000ms */
            {
                /* ��ʱδ�ظ����ظ����� '���� FLASH'
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * ��Ҫ�ظ� MMS������ Bootloader ��ʱ
                     */
                    sendMMSBuf[0] = updatePic24Obj;                 /* ���� */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;      /* ʧ�ܴ��� */
                    sendMMSBuf[2] = ER_FLASH;                       /* ʧ����Ϣ */
                    sendMMSBuf[3] = 0x00;
                    sendMMSBuf[4] = 0x00;
                    sendMMSBuf[5] = 0x00;
                    sendMMSLen = 6;
                    updatePic24Sts = UP_PIC24_FINISHED;             /* ���� */
                }
            }
            break;
            
        case P24_STEP_REPLY_OK:
            updateStep      = P24_STEP_INIT;
            updatePic24Sts  = UP_PIC24_WRITE;
            break;
    }
    
    /* 
     * �ظ� MMS
     */
    sendToMMSPackage();
}

/* =============================================================================
 * ���� PIC24 -- 
 * PIC32 �� PIC24 Bootloader ͨ��Э�����
 *   PIC32 ����  : дFLASH
 */
PRIVATE void upPIC24_BL_WriteFlash(void)
{
    int i;
    int8u * pReadBin;
    tSYSTICK    tickNow;

    tickNow = getSysTick();
    
    switch(updateStep)
    {
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            
        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep = P24_STEP_TX2P24;
        
        /* ������ bin ���ݣ����͸� PIC24 Bootloader */
        case P24_STEP_TX2P24:
            /* cmd, len, addrL, addrM, addrH */
            sendPic24Buf[0] = WT_FLASH;
            sendPic24Buf[1] = P24_WRITE_INST_ROWS;
            sendPic24Buf[2] = flashAddr.v[0];
            sendPic24Buf[3] = flashAddr.v[1];
            sendPic24Buf[4] = flashAddr.v[2];
            
            /* �� FLASH ���� 
             * ע�� FLASH ��ַ���ֵ�ַ
             */
            pReadBin = FLASH_TO_BIN_ADDR(flashAddr.Val);
            memcpy(sendPic24Buf + BL_HEADER_LEN, pReadBin, P24_BYTES_PER_ROW);
            sendPic24Len = BL_HEADER_LEN + P24_BYTES_PER_ROW;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* �ȴ� PIC24 Bootloader �ظ�д����
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* ��ʱδ�ظ����ظ�����д��
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* 
                     * ��Ҫ�ظ� MMS: д�볬ʱ
                     */
                    sendMMSBuf[0] = updatePic24Obj;                /* ���� */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;     /* ʧ�ܴ��� */
                    sendMMSBuf[2] = WT_FLASH;                      /* ʧ����Ϣ */
                    sendMMSBuf[3] = flashAddr.v[0];
                    sendMMSBuf[4] = flashAddr.v[1];
                    sendMMSBuf[5] = flashAddr.v[2];
                    sendMMSLen = 6;
                    updatePic24Sts = UP_PIC24_FINISHED; /* ���� */
                }
            }
            break;
            
        case P24_STEP_REPLY_OK:
            updateStep      = P24_STEP_INIT;
            updatePic24Sts  = UP_PIC24_VERIFY_WR;
            break;
    }
    
    
    /* 
     * �ظ� MMS
     */
    sendToMMSPackage();
}

/* =============================================================================
 * ���� PIC24 -- 
 * PIC32 �� PIC24 Bootloader ͨ��Э�����
 *   PIC32 ����  : �� FLASH У�� д����Ƿ���ȷ
 */
PRIVATE  void upPIC24_BL_VerifyProg(void)
{
    BOOL verifyOK;
    int8u * pReadBin;
    int8u * ptr;
    int i;

    tSYSTICK    tickNow;
    tickNow = getSysTick();
    
    switch(updateStep)
    {
        case P24_STEP_INIT:
            verifyErrCnt    = 0;
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;
            
        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep      = P24_STEP_TX2P24;
        
        /* ��������� PIC24 App or Bootloader
         *  */
        case P24_STEP_TX2P24:
            /* �� FLASH �ĵ�ַ����д���ַ��ͬ 
             * cmd, len, addrL, addrM, addrH
             */
            sendPic24Buf[0] = RD_FLASH;
            sendPic24Buf[1] = P24_INST_PER_ROW;
            sendPic24Buf[2] = flashAddr.v[0];
            sendPic24Buf[3] = flashAddr.v[1];
            sendPic24Buf[4] = flashAddr.v[2];
            sendPic24Len    = BL_HEADER_LEN;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* �ȴ� PIC24 App or Bootloader �ظ�
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* PIC24 ��ʱδ�ظ����ظ�ִ�з�������� PIC24
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* У��ʧ�ܣ��ض� PIC24 ��ʱ��
                     * �ظ� MMS
                     */
                    sendMMSBuf[0] = updatePic24Obj;                     /* ���� */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;          /* ʧ�ܴ��� */
                    sendMMSBuf[2] = RD_FLASH;                           /* ʧ����Ϣ */
                    sendMMSBuf[3] = flashAddr.v[0];
                    sendMMSBuf[4] = flashAddr.v[1];
                    sendMMSBuf[5] = flashAddr.v[2];
                    sendMMSLen    = 6;
                    updatePic24Sts = UP_PIC24_FINISHED; /* ���� */
                }
            }
            break;
            
        /* �յ��ظ��� FLASH ���ݣ��� д������� �Ա�У�� 
         */
        case P24_STEP_REPLY_OK:
            {
                /* Ĭ��У��ʧ�ܣ���Ҫ���·�������� PIC24 */
                verifyOK    = FALSE;            
                updateStep  = P24_STEP_TX2P24;

                /* У�������Ϣ */
                if((pic24ReplyBuf[1] == P24_INST_PER_ROW) && 
                   (pic24ReplyBuf[2] == flashAddr.v[0]) && 
                   (pic24ReplyBuf[3] == flashAddr.v[1]) && 
                   (pic24ReplyBuf[4] == flashAddr.v[2]))
                {
                    /* ��һ��У�� */
                    pReadBin = FLASH_TO_BIN_ADDR(flashAddr.Val);
                    ptr      = (int8u *)(pic24ReplyBuf + BL_HEADER_LEN);

                    /* �ȽϷ������ݺͻض������� */
                    if(memcmp(pReadBin, ptr, P24_BYTES_PER_ROW) == 0)
                    {
                        /* У��OK */
                        verifyOK  = TRUE;
                    }
                }

                if(verifyOK)
                {
                    /* У��OK */
                    verifyErrCnt    = 0;
                    
                    #ifdef SKIP_BLANK_FLASH
                    BOOL continueWr = FALSE;
                    
                    while(1)
                    {
                        ClearWDT();
                    #endif
                        
                        /* ׼��д����һ��ָ��
                         * �ж��Ƿ��Ѿ������������д�� FLASH
                         * ��Ҫ���� Bootloader ������
                         */
                        flashAddr.Val   += P24_BYTES_PER_ROW/2;             /* ע���д FLASH ���ֵ�ַ   */
                        if((flashAddr.Val >= BL_CODE_START) && (flashAddr.Val <= BL_CODE_END))
                        {
                            flashAddr.Val = BL_CODE_END + 1;
                        }

                        /* �ж��Ƿ��Ѿ������������д�� FLASH 
                         * ����û���� P24_FLASH_SIZE_WORD �ж�
                         */
                        if(flashAddr.Val >= APP_LAST_FLASH)
                        {
                            sendMMSBuf[1]   = TOMMS_PIC24_FINISHED;         /* �������   */
                            updatePic24Sts  = UP_PIC24_VERIFY_OK;           /* ȫ�����������У��Ok��׼������ PIC24 */
                            updateStep      = 0;
                            #ifdef SKIP_BLANK_FLASH
                            break;
                            #endif
                        }
                        else
                        {
                            sendMMSBuf[1]   = TOMMS_PIC24_UPDATING;         /* ����������   */
                            updatePic24Sts  = UP_PIC24_WRITE;

                            #ifdef SKIP_BLANK_FLASH
                            /* ����д���ȫ���ǿ�ָ�������������
                             *  */
                            DWORD *pInst;
                            pInst = (DWORD*)(FLASH_TO_BIN_ADDR(flashAddr.Val));
                            for(i=0; i<P24_BYTES_PER_ROW/P24_BYTES_PER_INST; i++)
                            {
                                if(*pInst != P24_BLANK_DATA)
                                {
                                    continueWr = TRUE;
                                    break;
                                }
                            }
                            #endif
                        }
                        
                    #ifdef SKIP_BLANK_FLASH
                        if(continueWr)
                            break;
                    }
                    #endif
                    
                    /* �ظ� MMS ����Ϣ */
                    sendMMSBuf[0] = updatePic24Obj;                     /* ���� */
                    sendMMSBuf[2] = flashAddr.v[0];                     /* ��������     */
                    sendMMSBuf[3] = flashAddr.v[1];
                    sendMMSBuf[4] = flashAddr.v[2];
                    sendMMSBuf[5] = (int8u)(P24_FLASH_SIZE_WORD >> 0);
                    sendMMSBuf[6] = (int8u)(P24_FLASH_SIZE_WORD >> 8);
                    sendMMSBuf[7] = (int8u)(P24_FLASH_SIZE_WORD >> 16);
                    sendMMSLen    = 8;
                }
                else
                {
                    /* �ж�У��������Ƿ���Ҫ�ٴ�У�� */
                    verifyErrCnt++;
                    if(verifyErrCnt >= UPCMD_TRY_VERIFY_TIMES)
                    {
                        /* �ظ� MMS��У��ʧ�ܣ��˳�����
                         */
                        updatePic24Sts = UP_PIC24_FINISHED;             /* ����     */
                        
                        sendMMSBuf[0] = updatePic24Obj;                 /* ����     */
                        sendMMSBuf[1] = TOMMS_PIC24_FAIL_VERIFY;        /* ʧ�ܴ��� */
                        sendMMSBuf[2] = RD_FLASH;                       /* ʧ����Ϣ */
                        sendMMSBuf[3] = flashAddr.v[0];
                        sendMMSBuf[4] = flashAddr.v[1];
                        sendMMSBuf[5] = flashAddr.v[2];
                        sendMMSLen    = 6;
                    }
                }
            }
            break;
    }
    
    /* 
     * �ظ� MMS
     */
    sendToMMSPackage();
}


/* =============================================================================
 * ���� PIC24 -- 
 * PIC32 �� PIC24 Bootloader ͨ��Э�����
 *   PIC32 ����  : ����PIC24 ��������
 */
PRIVATE void upPIC24_BL_VerifyOK(void)
{
    tFlashInfo      P24CodeInfo;
    tSYSTICK        tickNow;
    
    tickNow = getSysTick();
    
    switch(updateStep)
    {
        /* ��ʼ�� */
        case P24_STEP_INIT:
            updateRepeat    = 0;
            updateStep      = P24_STEP_SETOBJ;

        /* ѡ��ͨ�Ŷ��� */
        case P24_STEP_SETOBJ:
            setCommunObjAB(updatePic24Obj);
            updateStep      = P24_STEP_TX2P24;
        
        /* ��������� PIC24 App or Bootloader
         *  */
        case P24_STEP_TX2P24:
            getBinCodeInfo(&P24CodeInfo);
            sendPic24Buf[0] = VERIFY_OK;
            memcpy(&sendPic24Buf[1], &P24CodeInfo, sizeof(tFlashInfo));
            sendPic24Len    = sizeof(tFlashInfo) + 1;
            
            if(sendToPic24Package(OBJ_CMD_BOOTLOADER) == TRUE)
            {
                updateStepTick  = tickNow;
                updateStep      = P24_STEP_WAITRX;
            }
            break;
            
        /* �ȴ� PIC24 App or Bootloader �ظ�
         */
        case P24_STEP_WAITRX:
            if((tSYSTICK)(tickNow - updateStepTick) >= UPCMD_REPEAT_INTERVAL)
            {
                /* PIC24 ��ʱδ�ظ����ظ�ִ�з�������� PIC24
                 */
                updateRepeat++;
                if(updateRepeat < UPCMD_REPEAT_TIMES)
                {
                    updateStep  = P24_STEP_TX2P24;
                }
                else
                {
                    /* У��ʧ�ܣ��ض� PIC24 ��ʱ��
                     * �ظ� MMS
                     */
                    sendMMSBuf[0] = updatePic24Obj;                     /* ���� */
                    sendMMSBuf[1] = TOMMS_PIC24_FAIL_OVERTIME;          /* ʧ�ܴ��� */
                    sendMMSBuf[2] = VERIFY_OK;                          /* ʧ����Ϣ */
                    sendMMSLen    = 3;
                    updatePic24Sts = UP_PIC24_FINISHED; /* ���� */
                }
            }
            break;
            
        /* �յ��ظ�������״̬���˳�����״̬
         */
        case P24_STEP_REPLY_OK:
            updateStep      = UP_PIC24_INIT;
            updatePic24Sts  = UP_PIC24_FINISHED;            /* ����     */
            
            sendMMSBuf[0]   = updatePic24Obj;               /* ����     */
            sendMMSBuf[1]   = TOMMS_PIC24_RUNAPP;           /* ������ɣ�����APP   */
            sendMMSLen      = 3;
            if(pic24ReplyBuf[0] == VERIFY_OK)
            {
                sendMMSBuf[2]  = UP_PIC24_BL_RESET;         /* BOOTLOADER У��ɹ����������� */
                upPIC24_ResetChip(updatePic24Obj);
            }
            else if(pic24ReplyBuf[0] == (VERIFY_OK + 0x80))
            {
                sendMMSBuf[2]  = UP_PIC24_BL_NOTRESET;      /* BOOTLOADER У��ʧ�ܣ����������� */
            }
            break;
    }
    
    /* 
     * �ظ� MMS
     */    
    sendToMMSPackage();

}
            
/* =============================================================================
 * ���� PIC24 FLASH-HEX �ļ���ת���� BIN����У��� 
 * ����ȫ�����У��ͣ��Լ� ������ Bootloader �����У���
 */
PUBLIC void getBinCodeInfo(tFlashInfo * P24CodeInfo)
{
    tDWORD flashData;
    int32u tmpCS;
    int32u offset;
    
    int8u   *pCode;
    int8u   *pInfo;
    int     i;
    
    /* bootloader �汾��Ϣ */
    pInfo = (int8u *)(&P24CodeInfo->blInfo);
    pCode = (int8u *)(P24_CodeBin + (BL_VERSION_ADDR<<1));
    for(i=0; i<sizeof(P24CodeInfo->blInfo); i += 2)
    {
        *pInfo ++= *pCode ++;
        *pInfo ++= *pCode ++;
        pCode   += 2;           /* ����2�ֽ� */
    }
    
    offset  = 0;
    P24CodeInfo->allCodeCS  = 0;
    P24CodeInfo->onlyAppCS  = 0;
    
    /* ָ�� BIN �ļ� */
    pCode   = (int8u*)P24_CodeBin;
    
    while(offset < (P24_FLASH_SIZE_WORD<<1))
    {
        /* read flash data 
         * ����������� HEX ת���� BIN�ļ�ʱ����BIN ��ת�� C
         *  - ��λ����(BL_RESET_VICTOR)ָ�� BOOTLOADER ,
         *  - ��������(P24_CONFIG_WORD_ADDR)��2�ֽ����� 0,
         *  - FLASH 4 �ֽڣ�����ֽ�����0��
         */
        flashData.v[0] = *pCode++;
        flashData.v[1] = *pCode++;
        flashData.v[2] = *pCode++;
        flashData.v[3] = *pCode++;
        
        
        /* ���ݹ�������ϣ���������ʱ������Ϊ FF xx xx�������ֽ�����ΪFF
         * ����������� HEX �ļ��У����ֽ��� 00
         * Ϊ��ƥ��2�ߵ�У��ͣ������������⴦��
         */
        if(offset >= (P24_CONFIG_WORD_ADDR<<1))
        {
            flashData.v[2] = 0;
        }
        
        tmpCS = flashData.v[0] + flashData.v[1] + flashData.v[2] + flashData.v[3];
        P24CodeInfo->allCodeCS += tmpCS;
        
        if((offset < (BL_CODE_START<<1)) || (offset > (BL_CODE_END<<1)))
        {
            P24CodeInfo->onlyAppCS += tmpCS;
        }
        
        /* ��һ��ָ���ַƫ��
         */
        offset += P24_BYTES_PER_INST;
    }
    
    
    #if 0
    uartStartSend(COM_MMS, (int8u*)P24CodeInfo, sizeof(tFlashInfo));
    #endif
    
    return;
}

/* =============================================================================
 * ���� PIC24 ������
 * 
 */
PUBLIC void Pic24UpdateProcess(tUartComm *pComm)
{
    tSYSTICK tickNow;
    
    tickNow = getSysTick();
    pP24UpComm = pComm;
    
    {
        /* �ж�����������ʱ�� 
         * ��������ʼ���㣬��ָ��ʱ����û����Ӧ��
         *  - ��ǰ�� A�����л�����B
         *  - ��ǰ�� B�����˳�����
         */
        if(updatePic24Sts != UP_PIC24_NULL)
        {
            if((tickNow - updateP24_StartTick) >= (UPDATE_MAX_TIME_SEC * 1000))
            {
                sendMMSBuf[0]   = updatePic24Obj;               /* ����     */
                sendMMSBuf[1]   = TOMMS_PIC24_FAIL_OVERTIME;    /* ʧ�ܴ��� */
                sendMMSBuf[2]   = 0xff;                         /* ʧ����Ϣ */
                sendMMSBuf[3]   = 0xff;                         /* ʧ����Ϣ */
                sendMMSLen      = 4;

                /* 
                 * �ظ� MMS
                 */
                sendToMMSPackage();

                updatePic24Sts      = UP_PIC24_FINISHED;
                updateP24_StartTick = tickNow;              /* Ϊ�˿����л��� B       */
            }
        }
        
        switch(updatePic24Sts)
        {
            /* �����У�û���������� */
            case UP_PIC24_NULL :            
                break;

            /* ������ʼ�����Ȳ�ѯ PIC24 ��HEX��Ϣ�����������HEX�Ա�
             * �������� OBJ_CMD_BL_CTRL -> PIC24APP_CMD_GET_CS
             * 
             * �� PIC24 APP �ظ�
             *  */
            case UP_PIC24_INIT:             
                upPIC24_App_QueryInfo();    /*  */
                break;

            /* ������������ PIC24 ���� Bootloader
             * OBJ_CMD_BL_CTRL -> PIC24APP_CMD_ENTER_BL
             * 
             * �� PIC24 Bootloader �ظ�
             *  */
            case UP_PIC24_START_BL:         
                upPIC24_App_StartBL();
                break;

            /* ===== PIC24 �Ѿ����� Bootloader �������� ===== 
             * ����ȫ���� PIC32 �� PIC24 Bootloader ��ɽ�������
             *  1 �Ȳ��� FLASH
             *  2 �����������ݣ�д FLASH��
             *  3 �ض�FLASH����У�飨��FLASH��
             *  4 �ظ� 2~3��ֱ��ȫ��д��
             */
            case UP_PIC24_UPDATE_INI:               /* ����������ʼ�� */
                updateStep      = P24_STEP_INIT;
                flashAddr.Val   = 0;
                updatePic24Sts  = UP_PIC24_ERASE;   /* ������һ�����̣� PIC24 Bootloader ���� FLASH */
                break;

            case UP_PIC24_ERASE:                    /* �������� PIC24 Bootloader ����       */
                upPIC24_BL_EraseFlash();
                
                #if 0
                /* ���ԣ����Զ�ȡ Bootloader�����Ժ��FLASH���� */
                if(updatePic24Sts == UP_PIC24_WRITE)
                {
                    updatePic24Sts = UP_PIC24_FINISHED;             /* ���� */
                }
                #endif
                break;

            case UP_PIC24_WRITE:                    /* ��������� PIC24 Bootloader дFLASH  */
                //updatePic24Sts = UP_PIC24_NULL;
                upPIC24_BL_WriteFlash();
                break;

            case UP_PIC24_VERIFY_WR:                /* ��FLASH ����У��  */
                upPIC24_BL_VerifyProg();
                break;
                
            case UP_PIC24_VERIFY_OK:                /* ����PIC24����    */
                upPIC24_BL_VerifyOK();
                break;

            case UP_PIC24_FINISHED:                 /*  ������������ */
                /* �ж� A �� B ���ѽ����� */
                #if 1   /* 0 = ����ʱֻ��Ҫ�� PIC24A */
                if(updatePic24Obj == COMMOBJ_IORW_A)
                {
                    /* A ���������� B */
                    updatePic24Obj  = COMMOBJ_IORW_B;
                    updatePic24Sts  = UP_PIC24_INIT;
                    updateStep      = P24_STEP_INIT; 
                }
                else
                #endif
                {
                    /* A �� B ���ѽ��� */
                    updatePic24Sts = UP_PIC24_NULL;
                }
                break;
        }
    }
    
}
