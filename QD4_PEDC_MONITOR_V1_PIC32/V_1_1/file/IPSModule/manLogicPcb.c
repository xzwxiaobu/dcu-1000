/* =============================================================================*/
/* SZ16 ����������߼���
 * LOG������������෽ʽ��LOG������� IBP PSL SIG �Ĺ���
 *  ÿ��LOG���IDͨ������ȷ��
 *  Monitor ͨ�������������ȫ��·�л��壬�ð�ͨ����������ѡ������ LOG��
 *  */

#include "include.h"
#include "manLogicPcb.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

/* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ� */
#define LOG2_SWITCHTO_LOG1      1   /* 0--������ �߼���2 �л��� �߼���1���������� */
#define SWITCH_RESET_SELECT     1   /* 0--�������л������� K0 �̵�������������    */

/* ���� LOG �幤��״̬ */
#define LOGERR_NONE             0   /* �޴���           */
#define LOGERR_OFFLINE          1   /* ����             */
#define LOGERR_RELAY            2   /* �̵�������       */
#define LOGERR_LINK             3   /* ��������ߴ���   */
#define LOGERR_I2CFUNCT         4   /* I2C���ʴ���      */
#define LOGERR_CMDOUTPUT        5   /* ����������     */
#define LOG_OUTCMD_ERR_TIME     (100)   /* 100 x 8ms = 800ms */

#define LOG_ERROR_LAST_TIME     (50)    /* ��ʱȷ�� LOG ��Ĵ��󣬵�λ��8ms, 0 ��ʾ����Ҫ��ʱ */

/* LOG PCB��״̬��Ϣ 
 * �ṩ�ӿں�����ȡ LOG1 �� LOG2 ��״̬���ϳ�Ϊһ�ֽڣ���4λ�� LOG2����4λ��LOG1��
 */
PRIVATE int8u LOG1Error = LOGERR_NONE;    
PRIVATE int8u LOG2Error = LOGERR_NONE;
    PUBLIC int8u getLOGPcbState(void)
    {
        return ((LOG2Error & 0x0f) << 4) | (LOG1Error & 0x0f);
    }

/* ��ǰѡ��Ĺ��� LOG ��ID */
PRIVATE tEnmCommObj workLOGID = COMMOBJ_UNDEF;  
    /* ����ǰ�Ĺ���LOG��ID */
    PUBLIC tEnmCommObj getWorkLOGID(void)
    {
        return workLOGID;
    }

extern tChkOutputCmd MON_CmdMon;       /* ���Ӱ��������ӵ�   */
extern tChkOutputCmd LOGOut_Relay;     /* �߼���̵������״̬ */

/* =============================================================================*/
/* ���ù���LOG��ID
 * ����
 *  id
 * ����
 *  TRUE  ��ʾ���óɹ�
 *  FALSE ��ʾ����ʧ�ܣ���ά����һ�ε�ѡ��
 *  */
PUBLIC BOOL setWorkLOGID(tEnmCommObj LOGID)
{
    BOOL result;
    
    if((LOGID == COMMOBJ_LOG1) || (LOGID == COMMOBJ_LOG2))
    {
        result = TRUE;        
        
        if((workLOGID != LOGID))
        {
            /* ��ǰѡ��Ĺ��� LOG ��仯�Ժ󣬶�����ʼͨ�ŵ����ݣ������� */
            ((tSlaveInfo*)getSlaveInfoPtr(LOGID))->dataValidCnt = 5;
        }
        workLOGID = LOGID;
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

/* =============================================================================\
 * �ж� LOG ��Ĵ������
 * ע��LOG ��1��2�����˼̵�������ͨ�ϴ���
 * */
//BIT8 mon_LOGPcbCmdOut;
//#define MON_LOG_OUTCMD_OPN1     mon_LOGPcbCmdOut.B0     /* ���������������1 */
//#define MON_LOG_OUTCMD_CLS1     mon_LOGPcbCmdOut.B1     /* ���������������1 */
//#define MON_LOG_OUTCMD_OPN2     mon_LOGPcbCmdOut.B2     /* ���������������2 */
//#define MON_LOG_OUTCMD_CLS2     mon_LOGPcbCmdOut.B3     /* ���������������2 */


///* ���������ж��߼���������� */
//BIT8 LOG_CmdOutByIO;
//
//
//BIT8 LOG_CmdOutByRelay;
//#define LOG_LOG_OUTOPN1         LOG_CmdOutByRelay.B0
//#define LOG_LOG_OUTCLS1         LOG_CmdOutByRelay.B1
////#define LOG_LOG_OUTOPN2         LOG_CmdOutByRelay.B2
////#define LOG_LOG_OUTCLS2         LOG_CmdOutByRelay.B3
//#define LOG_LOG_OUT_IBPPSLOPN   LOG_CmdOutByRelay.B4
//#define LOG_LOG_OUT_OPNSIDE     LOG_CmdOutByRelay.B5

#ifdef LOG_OUT_Qn
BIT8 LOG_LOGPcbCmdOut;
#define LOG_LOG_OUTCMD_OPN1     LOG_LOGPcbCmdOut.B0     /* LOG�������������1 */
#define LOG_LOG_OUTCMD_CLS1     LOG_LOGPcbCmdOut.B1     /* LOG�������������1 */
#define LOG_LOG_OUTCMD_OPN2     LOG_LOGPcbCmdOut.B2     /* LOG�������������2 */
#define LOG_LOG_OUTCMD_CLS2     LOG_LOGPcbCmdOut.B3     /* LOG�������������2 */
#endif

/* LOGSelector ���ⲿѡ�� */
PRIVATE void checkLOGPcbErrors(tEnmCommObj LOGPcb)
{
    #if 1
    tSlaveInfo * pLOGInfo;
    int8u LOG1Err_tmp, LOG2Err_tmp;
    
    /* ========== ����� LOG1 ========== */
    do
    {
        LOG1Err_tmp             = LOGERR_NONE;
        
        #ifdef LOG_OUT_Qn
        LOG_LOGPcbCmdOut.byte   = 0;
        #endif
        
        
        /* ����? */
        if(IPSObj_chkOnline(COMMOBJ_LOG1) == FALSE)
        {
            LOG1Err_tmp = LOGERR_OFFLINE;
            break;
        }
        else
        {
            /* �ж϶������ݴ��� */
            if(((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1))->dataValidCnt > 0)
            {
                break;
            }
            
            /* �̵����Ƿ��д��� */
            LOG1Err_tmp = PEDC_RELAY4.byte | PEDC_RELAY5.byte | PEDC_RELAY6.byte | PEDC_RELAY7.byte | PEDC_RELAY8.byte | PEDC_RELAY9.byte;
            LOG1Err_tmp &= 0b10101010;
            if(LOG1Err_tmp != LOGERR_NONE)
            {
                LOG1Err_tmp = LOGERR_RELAY;
                break;
            }
            
            /* �����Ƿ��д����л���B·�ӵ���LOG1 */
            if((SWCH_LinkErr_CMD_LOG1 != 0) || (PEDC_LOG1MONLinkMsg.byte & LOGLINKERR_MASK_BITS != 0))
            {
                LOG1Err_tmp = LOGERR_LINK;
                break;
            }
            
            /* I2C�Ƿ��д��� */
            pLOGInfo = (tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1);
            if(pLOGInfo->slaveSts.bits.pca9505Err1 || pLOGInfo->slaveSts.bits.pca9505Err2 || pLOGInfo->slaveSts.bits.pca9505Err3)
            {
                LOG1Err_tmp = LOGERR_I2CFUNCT;
                break;
            }
        }
        
        /* û����������
         * ��� LOG�� ������������߼��Ƿ��д���
         * ע�⣺
         *   1. �߼����Ǹ��������ź���������������������
         *      ������ﲻ��ֱ��������õ�������ж�
         *   2. ���Ӱ�û�н������߼������������ӣ�ֻ�ܼ���ѡ�����߼���
         */
        if(LOG1Err_tmp == LOGERR_NONE)
        {
            if(bLOG_WorkLOG2 == 0)
            {
                /* ��ǰ�����߼���1���ж��Ƿ��������������״̬ */        
                if((PEDC_LinkErrMsg1.byte & MONLINKERR_MASK_BITS) != 0)
                {
                    if(checkingTim[TIM_LOG1_OUT_ERR].posTim >= LOG_OUTCMD_ERR_TIME)
                    {
                        LOG1Err_tmp = LOGERR_CMDOUTPUT;
                    }
                }
                else
                {
                    checkingTim[TIM_LOG1_OUT_ERR].posTim = 0;
                }
            }
        }
    }while(FALSE);
    
    /* �жϴ��������ʱ��
     * ��1�Ĵ���ȷ��ʱ�� + 10 ���� ��2�Ĵ���ȷ��ʱ�䣬
     * Ŀ���Ǳ����ϵ���1��2���д���ʱ����1��ȷ�ϴ�����л�����2 */
    if(LOG1Err_tmp != LOGERR_NONE)
    {
        if(checkingTim[TIM_LOG1_ERROR].posTim >= (LOG_ERROR_LAST_TIME + 10))
        {
            /* �����ߴ������ȵ��Ĵ���Ϊ׼���������������ȱ����� */
            if((LOG1Error == LOGERR_NONE) || (LOG1Err_tmp == LOGERR_OFFLINE))
            {
                LOG1Error = LOG1Err_tmp;
            }
        }
    }
    else
    {
        checkingTim[TIM_LOG1_ERROR].posTim      = 0;
        if(LOG1Error != LOGERR_NONE)
        {
            /* ����ָ�����ʱû���� */
            LOG1Error = LOGERR_NONE;
        }
    }
    
    /* ========== ����� LOG2 ========== */
    do
    {
        LOG2Err_tmp             = LOGERR_NONE;
        
        #ifdef LOG_OUT_Qn
        LOG_LOGPcbCmdOut.byte   = 0;
        #endif
        
        /* ����? */
        if(IPSObj_chkOnline(COMMOBJ_LOG2) == FALSE)
        {
            LOG2Err_tmp = LOGERR_OFFLINE;
            break;
        }
        else
        {
            /* �ж϶������ݴ��� */
            if(((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2))->dataValidCnt > 0)
            {
                break;
            }
            
            /* �̵����Ƿ��д��� */
            LOG2Err_tmp = PEDC_RELAY10.byte | PEDC_RELAY11.byte | PEDC_RELAY12.byte | PEDC_RELAY13.byte | PEDC_RELAY14.byte | PEDC_RELAY15.byte;
            LOG2Err_tmp &= 0b10101010;
            if(LOG2Err_tmp != LOGERR_NONE)
            {
                LOG2Err_tmp = LOGERR_RELAY;
                break;
            }
            
            /* �����Ƿ��д����л���A·�ӵ���LOG2 */
            if((SWCH_LinkErr_CMD_LOG2 != 0) || (PEDC_LOG2MONLinkMsg.byte & LOGLINKERR_MASK_BITS != 0))
            {
                LOG2Err_tmp = LOGERR_LINK;
                break;
            }
            
            /* I2C�Ƿ��д��� */
            pLOGInfo = (tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2);
            if(pLOGInfo->slaveSts.bits.pca9505Err1 || pLOGInfo->slaveSts.bits.pca9505Err2 || pLOGInfo->slaveSts.bits.pca9505Err3)
            {
                LOG2Err_tmp = LOGERR_I2CFUNCT;
                break;
            }
        }
        
        /* û����������
         * ��� LOG�� ������������߼��Ƿ��д���
         * ע�⣺
         *   1. �߼����Ǹ��������ź���������������������
         *      ������ﲻ��ֱ��������õ�������ж�
         *   2. ���Ӱ�û�н������߼������������ӣ�ֻ�ܼ���ѡ�����߼���
         */
        if(LOG2Err_tmp == LOGERR_NONE)
        {
            if(bLOG_WorkLOG2 == 1)
            {
                /* ��ǰ�����߼���2���ж��Ƿ��������������״̬ */        
                if((PEDC_LinkErrMsg1.byte & MONLINKERR_MASK_BITS) != 0)
                {
                    if(checkingTim[TIM_LOG2_OUT_ERR].posTim >= LOG_OUTCMD_ERR_TIME)
                    {
                        LOG2Err_tmp = LOGERR_CMDOUTPUT;
                    }
                }
                else
                {
                    checkingTim[TIM_LOG2_OUT_ERR].posTim = 0;
                }
            }
        }
    }while(FALSE);
    
    /* �жϴ��������ʱ��
     * ��1�Ĵ���ȷ��ʱ�� + 10 ���� ��2�Ĵ���ȷ��ʱ�䣬
     * Ŀ���Ǳ����ϵ���1��2���д���ʱ����1��ȷ�ϴ�����л�����2 */
    if(LOG2Err_tmp != LOGERR_NONE)
    {
        if(checkingTim[TIM_LOG2_ERROR].posTim >= (LOG_ERROR_LAST_TIME + 0))
        {
            /* �����ߴ������ȵ��Ĵ���Ϊ׼���������������ȱ����� */
            if((LOG2Error == LOGERR_NONE) || (LOG2Err_tmp == LOGERR_OFFLINE))
            {
                LOG2Error = LOG2Err_tmp;
            }
        }
    }
    else
    {
        checkingTim[TIM_LOG2_ERROR].posTim      = 0;
        if(LOG2Error != LOGERR_NONE)
        {
            /* ����ָ�����ʱû���� */
            LOG2Error = LOGERR_NONE;
        }
    }
    #endif
}


/* =============================================================================
 * ���� LOG1 �� LOG2
 * ����������3������㣬�ֱ��ʾ�ֶ�ѡ�� LOG1���Զ�ѡ���ֶ�ѡ��LOG2
 * ���ֶ�ѡ��ʱ��MONITOR��ѡ��Ӳ���жϼ���Ч
 * ���Զ�ѡ��ʱ��MONITOR�ж�LOG1�Ƿ�����������Ĭ��ѡ��LOG1������ѡ��LOG2
 * ���Զ��л���LOG2�󣬲����Զ��л���S1
 * 
 *  */
PRIVATE int8u LOGMode = 0xff;       /* 0=  �Զ�ѡ��1=�ֶ�ѡ��LOG1��2=�ֶ�ѡ��LOG2, 0xff=MCU��λ */
PRIVATE int8u autoLOG = 0xff;       /* 0=���Զ�ѡ��1=�Զ�ѡ��LOG1��2=�Զ�ѡ��LOG2, 0xff=MCU��λ */
PUBLIC void manageLOG1LOG2(void)
{
    #if 1
    /* MONITOR �ϵ��λ��ȴ�һ��ʱ������ж�
     * ����һ��������ʱ���ֵ��쳣���� LOG �廹û��ͨ�ųɹ���
     *  */
    static   tSYSTICK powerOnTick;
    tSYSTICK tickNow;
    tickNow = getSysTick();
    if(autoLOG == 0xff)
    {
        powerOnTick = tickNow;
        autoLOG = 0;
    }
    
    /* �ж���ʱʱ�� */
    if((tSYSTICK)(tickNow - powerOnTick < 3000))
    {
        /* ������ʼͨ�ŵ����ݣ������� */
        ((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG1))->dataValidCnt = 5;
        ((tSlaveInfo*)getSlaveInfoPtr(COMMOBJ_LOG2))->dataValidCnt = 5;
        return;
    }
    
    /* 
     * LOGMode = ͨ��IO�жϵ�ǰѡ���ģʽ
     * ���ݿ���״̬ȷ��
     *  */
    if(bLOG_Sw_LOG1)
    {
        /* 1 = �ֶ�ѡ��LOG1 */
        if(LOGMode != 1)
        {
            LOG1Error   = LOGERR_NONE;
            LOGMode     = 1;
        }
        autoLOG = 0;
        
        setWorkLOGID(COMMOBJ_LOG1);
    }
    else if(bLOG_Sw_LOG2)
    {
        /* 2 = �ֶ�ѡ��LOG2 */
        if(LOGMode != 2)
        {
            LOG2Error   = LOGERR_NONE;
            LOGMode     = 2;
        }
        autoLOG = 0;
        setWorkLOGID(COMMOBJ_LOG2);
    }
    else /* if(bLOG_Sw_Auto) */
    {
        if(LOGMode != 0)
        {
            LOGMode     = 0;
            LOG1Error   = LOGERR_NONE;
            LOG2Error   = LOGERR_NONE;
        }
        
        /* 0 = �Զ�ѡ�� */
        if(autoLOG == 0)
        {   
            /* �ֶ��л����Զ��������ϵ���һ��ѡ��
             * ֻҪ LOG1 ������ѡ LOG1
             * ��   LOG1 �쳣���л��� LOG2
             */
            if(LOG1Error == LOGERR_NONE)
            {
                autoLOG = 1;
            }
            else
            {
                autoLOG = 2;
            }
        }
        else if(autoLOG == 1)
        {
            /* �Զ�ѡ�� LOG1��
             * �� LOG1 �쳣�� LOG2������ѡ�� LOG2
             *  */
            if((IPSObj_chkOnline(COMMOBJ_LOG1) == TRUE) && (IPSObj_chkOnline(COMMOBJ_LOG2) == TRUE))
            {
                /* LOG1��2������ */
                if((LOG1Error != LOGERR_NONE) &&  (LOG2Error == LOGERR_NONE))
                {
                    autoLOG = 2;
                }
            }
            else
            {
                /* ���ֻ��LOG2���ߣ�����LOG2�Ƿ����������ϣ����л���LOG2 */
                if((LOG1Error == LOGERR_OFFLINE) && (LOG2Error != LOGERR_OFFLINE))
                {
                    autoLOG = 2;
                }
            }
        }
        /* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ� */
        else if(autoLOG == 2)
        {
            #if (LOG2_SWITCHTO_LOG1 != 0)
            /* �Զ�ѡ�� LOG2��
             * �� LOG2 �쳣�� LOG1������ѡ�� LOG1
             *  */
            if((IPSObj_chkOnline(COMMOBJ_LOG1) == TRUE) && (IPSObj_chkOnline(COMMOBJ_LOG2) == TRUE))
            {
                /* LOG1��2������ */
                if((LOG1Error == LOGERR_NONE) &&  (LOG2Error != LOGERR_NONE))
                {
                    autoLOG = 1;
                }
            }
            else
            {
                /* ���ֻ��LOG1���ߣ�����LOG1�Ƿ����������ϣ����л���LOG1 */
                if((LOG1Error != LOGERR_OFFLINE) && (LOG2Error == LOGERR_OFFLINE))
                {
                    autoLOG = 1;
                }
            }
            #endif
        }
        
        if(autoLOG == 1)    
        {
            setWorkLOGID(COMMOBJ_LOG1);
        }
        else
        {
            setWorkLOGID(COMMOBJ_LOG2);
        }
    }
    
    /* �ж� LOG ��Ĵ������ */
    checkLOGPcbErrors(workLOGID);
    
    #endif
}


/* =============================================================================
 * �� �л���� IO �������
 *  */
tCmdWriteIO_SFSW switchOutput;
#if 0
extern tCmdWriteIO_SFSW testSFSWOutput;
#endif
int8u* getSwitchOutputIODataPtr(int *DataLen)
{
    static int8u output_Prev = 0xff;
    
    
    /* �Ƚ�ֹ��� */
    memset((void*)switchOutput.IOData, 0, sizeof(switchOutput));
    
    #if (SWITCH_RESET_SELECT != 0)
    if((autoLOG == 1) || (autoLOG == 2))
    {
        /* 1=�Զ�ѡ��LOG1 ʱ������ �л���������� IO ��λ�̵��� K0
         * ע�⣺��� 0 ������ �л���� �̵���K0
         *  */
        switchOutput.IOOut.RESET_En = 1;
        switchOutput.IOOut.RESET    = 1;
        if(autoLOG == 1)
        {
            /* ������Ϊ�����Զ�ģʽ�£����� K0 ��Ȧ��ͨ��ʱ�䣬�ӳ��̵�������
             *  */
            if(output_Prev != autoLOG)
            {
                checkingTim[RTIM_SWITCHER_RESET_SELECT].posTim = 0;
            }

            /* �̵���K0 ͨ��ʱ�� 5000ms */
            if(checkingTim[RTIM_SWITCHER_RESET_SELECT].posTim < (5000/RELAY_TIMER_INTERVAL))
            {
                switchOutput.IOOut.RESET = 0;
            }
        }

        output_Prev = autoLOG;
    }
    #endif
    
    *DataLen = sizeof(tCmdWriteIO_SFSW);
    
    int8u *pOut;
    #if 1
    /* ������� ���͸��л���
     *  */
    pOut = (int8u*)&switchOutput.IOData[0];
    #else

    /* �Ƿ��� IO �������ģʽ?
     * ������ ����������� ���͸��л���
     *  */
    if(chkIfTestOutputIO() == FALSE)
    {
        pOut = (int8u*)&switchOutput.IOData[0];
    }
    else
    {
        pOut = (int8u*)&testSFSWOutput.IOData[0];
    }
    #endif

    return (int8u*)pOut;
}


