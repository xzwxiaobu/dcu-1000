
#include <string.h>
#include "_cpu.h"
#include "..\Include.h"
#include "IO_Handle.h"
#include "CanTgt.h"

#include "IOLogic.h"
#include "SF_IODef.h"
#include "BASSys_Reg.h"

/* ע�⣺�л����� FD_SF_SW_V1: �޸���Ӳ������
 *  */

tExtIO  log1PCBIO;                  /* �߼��� LOG1 ��IO���� */
tExtIO  log2PCBIO;                  /* �߼��� LOG2 ��IO���� */

/* ������ʱ���� */
tChkOutputCmd MON_CmdMon;           /* ���Ӱ��������ӵ�   */
tChkOutputCmd LOGOut_Relay;         /* �߼���̵������״̬ */

BIT8 TempErrMsg;
#define bMON_CtrlCmd_Fault          TempErrMsg.B7   /* ����ȱʧ�����š����ر���       */
#define bMON_OUTOPNCLS_Fault        TempErrMsg.B6   /* MON-J4-2/3/14/15: ����ȱʧ       */
#define bMON_OUTOPNSD_Fault         TempErrMsg.B5   /* MON-J4-4/5/16/17: ���ر���ȱʧ   */

#define RESET_CHECKINGTIME(inx)     (checkingTim[inx].posTim = 0)

extern BOOL chkDelayProcessIO(void);
int8u checkSysTimeForLightBelt(void);

/* ���� IO ������Ϣ */
void PCA9505_data_deal(void)
{
    int i, j;
    tAlarmFlags Temp_Alarm0;

    tOutputCmd  OCmd;
    tSlaveInfo * pSlvInfo;

    int8u   *p1, *p2;
    tExtIO  *pLogIO;

    TempErrMsg.byte = 0;

    #if 1   /* �����۵�����ʼ�� */
    /* ����״̬��ʼ�� */
    Temp_Alarm0.word     = 0;

    /* ��������͸�PEDC����Ϣ
     * */
    for(i = 0; i < AVR_DATA_LEN; i ++)
    {
        MESG_BUF[i].byte = 0;
    }

    /* IO ���Ĭ�϶��� �ߵ�ƽ 1
     * */
    for(i = 0; i < OUTPUT_DATA_LEN; i ++)
    {
        MON_output_data[i].byte = 0xff;
    }

    /* ��λ��ȴ��ӳ�ʱ�� */
    if(chkDelayProcessIO() == FALSE)
    {
        return;
    }

    /* ���� �߼���1�� IO ���� */
    p1 = (int8u *)&log1PCBIO;
    p2 = (int8u *)&LOG1_input_data;
    for(i=0; i<sizeof(log1PCBIO); i++)
    {
        *p1 = *p2;
        p1++;
        p2++;
    }

    /* ���� �߼���2�� IO ���� */
    p1 = (int8u *)&log2PCBIO;
    p2 = (int8u *)&LOG2_input_data;
    for(i=0; i<sizeof(log2PCBIO); i++)
    {
        *p1 = *p2;
        p1++;
        p2++;
    }
    #endif

    /* ���ͨѶ�ڵ����ô��� */
    for(j = 0; j < DCUsetting_LEN; j ++)
    {
        if(uDCUsetting.settingBUF[j] != uDCUsetting.ReadSettingBUF[j])
        {
            bPedc_NodeCfgErr   = 1;
        }
    }

    /* DCU ID ��ͻ��Ϣ */
    for(j = 0; j < sizeof(CanManage.IDConflict); j ++)
    {
        if(CanManage.IDConflict[j] != 0)
        {
            bPedc_DCUIDConflicted = 1;
            break;
        }
    }

    /* �� MMS�� BAS ״̬ */
    bBASSys_OffLine     = !getBASSysOnlineSts();      /* MMS 1��ʾ���ߣ�PEDC 1�����ߣ�����ȡ���� MMS */
    
    tWindValveSysSts * pWVSts = getWindValveSysSts();
    bBASOpnCmd      = pWVSts->bBASOpnCmd_sts;
    bBASClsCmd      = pWVSts->bBASClsCmd_sts;
    bPSC_OpnValve   = pWVSts->bPSCOpnCmd_sts;
    bPSC_ClsValve   = pWVSts->bPSCClsCmd_sts;
    
    tWindValveResult * pWVResult = getWindValveResult();
    bPEDC_WVAllClsed        = pWVResult->bWindAllClsed;
    bPEDC_WVAllOpned        = pWVResult->bWindAllOpned;
    bPEDC_OpnClsOverTime    = 0;//pWVResult->bWindOpnCls_OT;
    
    /* �������оƬ����״̬ */
    bPEDC_PIC24A_OFFLINE = (getObjCommLinkSts(COMMOBJ_IORW_A))  ? 0: 1; /* PIC24A ���߱�־ */
    bPEDC_PIC24B_OFFLINE = (getObjCommLinkSts(COMMOBJ_IORW_B))  ? 0: 1; /* PIC24B ���߱�־ */
    bPEDC_SWCH_OFFLINE   = (getObjCommLinkSts(COMMOBJ_SWITCHER))? 0: 1; /* �л������߱�־  */
    bPEDC_LOG1_OFFLINE   = (getObjCommLinkSts(COMMOBJ_LOG1)  )  ? 0: 1; /* LOG1   ���߱�־ */
    bPEDC_LOG2_OFFLINE   = (getObjCommLinkSts(COMMOBJ_LOG2)  )  ? 0: 1; /* LOG2   ���߱�־ */
    
    #if 1   /* �����۵� : �߼���ѡ�� */
    bLOG_Sw_Auto            = GET_INPUT_STS(MI_LOGAutoMode);    /* ѡ���߼��壺�Զ�  */
    bLOG_Sw_LOG1            = GET_INPUT_STS(MI_LOG1_Switch);    /* ѡ���߼��壺�ֶ�1 */
    bLOG_Sw_LOG2            = GET_INPUT_STS(MI_LOG2_Switch);    /* ѡ���߼��壺�ֶ�2 */

    int8u chkSwitchErr = (int8u)bLOG_Sw_Auto + (int8u)bLOG_Sw_LOG1 + (int8u)bLOG_Sw_LOG2;

    /* �߼��忪��ѡ������Ҫ��һ�������򱨴�, Ϊ�˱�����תʱ�󱨣���Ҫ����ʱ��ϳ�(6s) */
    TrueSetFLAG((chkSwitchErr != 1)  , checkingTim[TIM_LOG1LOG2_SELECT].posTim, TIME_VALUE_BASE_10(6000), MON_LOG1LOG2_SELECT);


    /* PEDC ѡ����߼���
     *  */
    if(getWorkLOGID() == COMMOBJ_LOG1)
    {
        MO_LOG_Q2_LOG2      = 1;                /* ���Ӱ�IO���1: ѡ���߼���1(ID=1) */
    }
    else
    {
        MO_LOG_Q2_LOG2      = 0;                /* ���Ӱ�IO���0: ѡ���߼���2(ID=2) */
    }

    BOOL clr_TIM_SWCH_Q1AQ2A_LOST = TRUE;
    if(bPEDC_SWCH_OFFLINE == 0)
    {
        /* �л������ߣ��� K1���յ��жϣ�
         * �����ϣ����յ�� �߼���1(IN-B0 1 2 ...)
         * �����㶯����ʾѡ�� �߼���2 */
        bLOG_WorkLOG2 = GET_INPUT_STS_1(SWCH_K1_NC);

        /* �Զ�ģʽ���� �л���ļ̵��������ж�
         * ���Ӱ���������л���������ߣ��뵱ǰ�л���ʵ�ʵļ̵�����(K1~K6)
         *      MO_LOG_Q2_LOG2      bLOG_WorkLOG2
         *      1=LOG1              0=LOG1
         *      0=LOG2              1=LOG2
         *  */
        if(bLOG_Sw_Auto)
        {           /* ���Ӱ�ѡ�� LOG x�� �л��岻�� LOG x������ */
            if((MO_LOG_Q2_LOG2 == bLOG_WorkLOG2))
            {
                TrueSetFLAG(((MO_LOG_Q2_LOG2 == bLOG_WorkLOG2))  , checkingTim[TIM_SWCH_Q1AQ2A_LOST].posTim, 120, SWCH_LinkErr_Q1AQ2A_LOST);
                clr_TIM_SWCH_Q1AQ2A_LOST = FALSE;
            }
        }
    }
    else
    {
        /* ������ʵ��ӳѡ����߼��� */
        bLOG_WorkLOG2 = !MO_LOG_Q2_LOG2;
    }
    
    if(clr_TIM_SWCH_Q1AQ2A_LOST == TRUE)
        RESET_CHECKINGTIME(TIM_SWCH_Q1AQ2A_LOST);
    #endif

    #if 1   /* �����۵� : ��ȫ��·�л��� */
    /* ��ȫ��·�л��� */
    if(bPEDC_SWCH_OFFLINE == 0)
    {
        /* �����Źر������� */
        bPedc_CLSLCK    = GET_INPUT_STS(SWCH_K11_POW);

        /* ===== ���ؼ̵���״̬ ===== */
        /* �̵��� ״̬(����λ��ʾ)  00          01        10          11
         *                        (�Ͽ�����)   ����      ����        ��
         * */
        Relay_Fault_Check((SWCH_K0_POW  == VALID)   , !SWCH_K0_NC    , SWCH_K0 , bSWCH_K0_Coil , bSWCH_K0_Fault  );

        Relay_Fault_Check((SWCH_K1_POW  == VALID)   , !SWCH_K1_NC    , SWCH_K1 , bSWCH_K1_Coil , bSWCH_K1_Fault  );
        Relay_Fault_Check((SWCH_K2_POW  == VALID)   , !SWCH_K2_NC    , SWCH_K2 , bSWCH_K2_Coil , bSWCH_K2_Fault  );
        Relay_Fault_Check((SWCH_K3_POW  == VALID)   , !SWCH_K3_NC    , SWCH_K3 , bSWCH_K3_Coil , bSWCH_K3_Fault  );

        Relay_Fault_Check((SWCH_K4_POW  == VALID)   , !SWCH_K4_NC    , SWCH_K4 , bSWCH_K4_Coil , bSWCH_K4_Fault  );
        Relay_Fault_Check((SWCH_K5_POW  == VALID)   , !SWCH_K5_NC    , SWCH_K5 , bSWCH_K5_Coil , bSWCH_K5_Fault  );
        Relay_Fault_Check((SWCH_K6_POW  == VALID)   , !SWCH_K6_NC    , SWCH_K6 , bSWCH_K6_Coil , bSWCH_K6_Fault  );

        Relay_Fault_Check((SWCH_K11_POW  == VALID)  , !SWCH_K11_NC   , SWCH_K11 , bSWCH_K11_Coil , bSWCH_K11_Fault  );
        Relay_Fault_Check((SWCH_K12_POW  == VALID)  , !SWCH_K12_NC   , SWCH_K12 , bSWCH_K12_Coil , bSWCH_K12_Fault  );

        Relay_Fault_Check(((SWCH_K11_NC  == VALID) && (SWCH_K12_NC == VALID))  ,  SWCH_K13_NO   , SWCH_K13 , bSWCH_K13_Coil , bSWCH_K13_Fault  );

        //Relay_Fault_Check((SWCH_K21_POW  == VALID)  ,  SWCH_K21_NO   , SWCH_K21 , bSWCH_K21_Coil , bSWCH_K21_Fault  );
        //Relay_Fault_Check((SWCH_K22_POW  == VALID)  ,  SWCH_K22_NO   , SWCH_K22 , bSWCH_K22_Coil , bSWCH_K22_Fault  );

        /* ��ȫ��·����?  */
        if(bSWCH_K11_Fault || bSWCH_K12_Fault || bSWCH_K13_Fault || SF_FAULT)
        {
            bSWCH_SF_Fault = SETBIT;
        }

        /* ��ȫ��·��ͨ
         * �� K11K12 ͨ��(SWCH_K11_POW)��K13�ϵ磨��ⳣ����SWCH_K13_NO��
         *   */
        bSWCH_SF_Coil = GET_INPUT_STS(SWCH_K11_POW);
    }
    else
    {
        int k;
        for(k = SWCH_K0; k<=SWCH_K23; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif

    /*===== �߼��� 1===== */
    #if 1   /* �����۵� : �߼��� 1 �̵���״̬ */
    if(bPEDC_LOG1_OFFLINE == 0)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
        bPEDC_LOG1_TestMode_En = pSlvInfo->slaveInput.logIn.testRunSts.testEnable;
        bPEDC_LOG1_TestRunning = pSlvInfo->slaveInput.logIn.testRunSts.isRunning;

        pLogIO = &log1PCBIO;

        /* ===== �̵���״̬ ===== */
        /* �̵��� ״̬(����λ��ʾ)  00          01        10          11
         *                        (�Ͽ�����)   ����      ����        ��
         * */
        Relay_Fault_Check((pLogIO->relay.K0_PN == VALID)   , pLogIO->relay.K0_NO    , LOG1_K0  , bLOG1_K0_Coil  , bLOG1_K0_Fault   );

        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1A_NO   , LOG1_K1A , bLOG1_K1A_Coil , bLOG1_K1A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1B_NO   , LOG1_K1B , bLOG1_K1B_Coil , bLOG1_K1B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2A_NO   , LOG1_K2A , bLOG1_K2A_Coil , bLOG1_K2A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2B_NO   , LOG1_K2B , bLOG1_K2B_Coil , bLOG1_K2B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3A_NO   , LOG1_K3A , bLOG1_K3A_Coil , bLOG1_K3A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3B_NO   , LOG1_K3B , bLOG1_K3B_Coil , bLOG1_K3B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4A_NO   , LOG1_K4A , bLOG1_K4A_Coil , bLOG1_K4A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4B_NO   , LOG1_K4B , bLOG1_K4B_Coil , bLOG1_K4B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5A_NO   , LOG1_K5A , bLOG1_K5A_Coil , bLOG1_K5A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5B_NO   , LOG1_K5B , bLOG1_K5B_Coil , bLOG1_K5B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6A_NO   , LOG1_K6A , bLOG1_K6A_Coil , bLOG1_K6A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6B_NO   , LOG1_K6B , bLOG1_K6B_Coil , bLOG1_K6B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7A_NO   , LOG1_K7A , bLOG1_K7A_Coil , bLOG1_K7A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7B_NO   , LOG1_K7B , bLOG1_K7B_Coil , bLOG1_K7B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8A_NO   , LOG1_K8A , bLOG1_K8A_Coil , bLOG1_K8A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8B_NO   , LOG1_K8B , bLOG1_K8B_Coil , bLOG1_K8B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K21_PN == VALID) , !pLogIO->relay.K21_NC   , LOG1_K21 , bLOG1_K21_Coil , bLOG1_K21_Fault  );
        Relay_Fault_Check((pLogIO->relay.K22_PN == VALID) ,  pLogIO->relay.K22_NO   , LOG1_K22 , bLOG1_K22_Coil , bLOG1_K22_Fault  );
        Relay_Fault_Check((pLogIO->relay.K30_PN == VALID) , !pLogIO->relay.K30_NC   , LOG1_K30 , bLOG1_K30_Coil , bLOG1_K30_Fault  );

        Relay_Fault_Check(((pLogIO->fun.JI_PSL1_EN == VALID) || (pLogIO->fun.JI_PSL2_EN == VALID) || (pLogIO->fun.JI_PSL3_EN == VALID)) ,  \
                        !pLogIO->relay.K10_NC   , LOG1_K10 , bLOG1_K10_Coil , bLOG1_K10_Fault  );

    }
    else
    {
        int k;
        for(k = LOG1_K0; k<=LOG1_K30; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif


    /*===== �߼��� 2===== */
    #if 1   /* �����۵� : �߼��� 2 �̵���״̬ */
    if(bPEDC_LOG2_OFFLINE == 0)
    {
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
        bPEDC_LOG2_TestMode_En = pSlvInfo->slaveInput.logIn.testRunSts.testEnable;
        bPEDC_LOG2_TestRunning = pSlvInfo->slaveInput.logIn.testRunSts.isRunning;

        pLogIO = &log2PCBIO;

        /* ===== �̵���״̬ ===== */
        /* �̵��� ״̬(����λ��ʾ)  00          01        10          11
         *                        (�Ͽ�����)   ����      ����        ��
         * */
        Relay_Fault_Check((pLogIO->relay.K0_PN == VALID)   , pLogIO->relay.K0_NO    , LOG2_K0  , bLOG2_K0_Coil  , bLOG2_K0_Fault   );

        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1A_NO   , LOG2_K1A , bLOG2_K1A_Coil , bLOG2_K1A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K1_PN == VALID)   , pLogIO->relay.K1B_NO   , LOG2_K1B , bLOG2_K1B_Coil , bLOG2_K1B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2A_NO   , LOG2_K2A , bLOG2_K2A_Coil , bLOG2_K2A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K2_PN == VALID)   , pLogIO->relay.K2B_NO   , LOG2_K2B , bLOG2_K2B_Coil , bLOG2_K2B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3A_NO   , LOG2_K3A , bLOG2_K3A_Coil , bLOG2_K3A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K3_PN == VALID)   , pLogIO->relay.K3B_NO   , LOG2_K3B , bLOG2_K3B_Coil , bLOG2_K3B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4A_NO   , LOG2_K4A , bLOG2_K4A_Coil , bLOG2_K4A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K4_N  == VALID)   , pLogIO->relay.K4B_NO   , LOG2_K4B , bLOG2_K4B_Coil , bLOG2_K4B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5A_NO   , LOG2_K5A , bLOG2_K5A_Coil , bLOG2_K5A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K5_N  == VALID)   , pLogIO->relay.K5B_NO   , LOG2_K5B , bLOG2_K5B_Coil , bLOG2_K5B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6A_NO   , LOG2_K6A , bLOG2_K6A_Coil , bLOG2_K6A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K6_N  == VALID)   , pLogIO->relay.K6B_NO   , LOG2_K6B , bLOG2_K6B_Coil , bLOG2_K6B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7A_NO   , LOG2_K7A , bLOG2_K7A_Coil , bLOG2_K7A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K7_PN == VALID)   , pLogIO->relay.K7B_NO   , LOG2_K7B , bLOG2_K7B_Coil , bLOG2_K7B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8A_NO   , LOG2_K8A , bLOG2_K8A_Coil , bLOG2_K8A_Fault  );
        Relay_Fault_Check((pLogIO->relay.K8_N  == VALID)   , pLogIO->relay.K8B_NO   , LOG2_K8B , bLOG2_K8B_Coil , bLOG2_K8B_Fault  );

        Relay_Fault_Check((pLogIO->relay.K21_PN == VALID) , !pLogIO->relay.K21_NC   , LOG2_K21 , bLOG2_K21_Coil , bLOG2_K21_Fault  );
        Relay_Fault_Check((pLogIO->relay.K22_PN == VALID) ,  pLogIO->relay.K22_NO   , LOG2_K22 , bLOG2_K22_Coil , bLOG2_K22_Fault  );
        Relay_Fault_Check((pLogIO->relay.K30_PN == VALID) , !pLogIO->relay.K30_NC   , LOG2_K30 , bLOG2_K30_Coil , bLOG2_K30_Fault  );

        Relay_Fault_Check(((pLogIO->fun.JI_PSL1_EN == VALID) || (pLogIO->fun.JI_PSL2_EN == VALID) || (pLogIO->fun.JI_PSL3_EN == VALID)) ,  \
                        !pLogIO->relay.K10_NC   , LOG2_K10 , bLOG2_K10_Coil , bLOG2_K10_Fault  );
    }
    else
    {
        int k;
        for(k = LOG2_K0; k<=LOG2_K30; k++)
        {
            relayWorkTime[k].posTim = 0;
            relayWorkTime[k].revTim = 0;
        }
    }
    #endif

    /* ===== ���������״̬ =====
     * ע���ⲿ�ִ�����Ҫ�õ� �߼���� bPEDC_LOG1_TestMode_En bPEDC_LOG2_TestMode_En
     *  */
    #if 1   /* �����۵� : �ж��߼���1���߼���2�����Ӱ� ���������Ϣ */
    BOOL  linkErrFlag;
    int8u lnkMsg = PCBLINK_ERR_NONE;

    tLinkSts    log1Link, log2Link;
    tLinkSts    compLink;
    int8u       log1IBPPSL_En,log2IBPPSL_En;

    tLinkSts monLink;

    for(i=0; i<sizeof(tLinkSts); i++)
    {
        log1Link.bytes[i] = 0;
        log2Link.bytes[i] = 0;
        compLink.bytes[i] = 0;
        monLink.bytes [i] = 0;
    }

    /* ���Ӱ��������� : ֻ�� �ź�ϵͳ������
     *
     *  */
    monLink.LinkBit.SIG_Opn         = GET_INPUT_STS(MI_SIG_OpnCmd);
    monLink.LinkBit.SIG_Cls         = GET_INPUT_STS(MI_SIG_ClsCmd);

    /* �߼���1�������� */
    log1Link.LinkBit.IBP_En         = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_EN);
    log1Link.LinkBit.IBP_Opn        = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_OPN8);
    log1Link.LinkBit.IBP_Cls        = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_CLS8) ;
    log1Link.LinkBit.IBP_OpnSD_En   = log1Link.LinkBit.IBP_En;/* GET_INPUT_STS(log1PCBIO.fun.JI_IBP_A_EN); */
    log1Link.LinkBit.IBP_OpnSD      = GET_INPUT_STS(log1PCBIO.fun.JI_IBP_OPN6);

    log1Link.LinkBit.PSL1_En        = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_EN);
    log1Link.LinkBit.PSL1_Opn       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_OPN8);
    log1Link.LinkBit.PSL1_Cls       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL1_CLS8);

    log1Link.LinkBit.PSL2_En        = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_EN);
    log1Link.LinkBit.PSL2_Opn       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_OPN8);
    log1Link.LinkBit.PSL2_Cls       = GET_INPUT_STS(log1PCBIO.fun.JI_PSL2_CLS8);

    log1Link.LinkBit.SIG_Opn        = GET_INPUT_STS(log1PCBIO.fun.JI_SIG_OPN8);
    log1Link.LinkBit.SIG_Cls        = GET_INPUT_STS(log1PCBIO.fun.JI_SIG_CLS);
    log1IBPPSL_En   = log1Link.LinkBit.IBP_En | log1Link.LinkBit.PSL1_En | log1Link.LinkBit.PSL2_En;

    /* �߼���2�������� */
    log2Link.LinkBit.IBP_En         = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_EN);
    log2Link.LinkBit.IBP_Opn        = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_OPN8);
    log2Link.LinkBit.IBP_Cls        = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_CLS8);
    log2Link.LinkBit.IBP_OpnSD_En   = log2Link.LinkBit.IBP_En;  /* GET_INPUT_STS(log2PCBIO.fun.JI_IBP_A_EN); */
    log2Link.LinkBit.IBP_OpnSD      = GET_INPUT_STS(log2PCBIO.fun.JI_IBP_OPN6);

    log2Link.LinkBit.PSL1_En        = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_EN);
    log2Link.LinkBit.PSL1_Opn       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_OPN8);
    log2Link.LinkBit.PSL1_Cls       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL1_CLS8);

    log2Link.LinkBit.PSL2_En        = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_EN);
    log2Link.LinkBit.PSL2_Opn       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_OPN8);
    log2Link.LinkBit.PSL2_Cls       = GET_INPUT_STS(log2PCBIO.fun.JI_PSL2_CLS8);

    log2Link.LinkBit.SIG_Opn        = GET_INPUT_STS(log2PCBIO.fun.JI_SIG_OPN8);
    log2Link.LinkBit.SIG_Cls        = GET_INPUT_STS(log2PCBIO.fun.JI_SIG_CLS);
    log2IBPPSL_En = log2Link.LinkBit.IBP_En | log2Link.LinkBit.PSL1_En | log2Link.LinkBit.PSL2_En;

    /* �߼���1 �� ���Ӱ� ������
     * ����Ŀֻ�� �źſ���������
     *  */
    if((bPEDC_LOG1_OFFLINE == 0))
    {
        compLink.bytes[0] = log1Link.bytes[0] ^ monLink.bytes[0];
        compLink.bytes[1] = log1Link.bytes[1] ^ monLink.bytes[1];
        lnkMsg = PCBLINK_ERR_NONE;

        /* �ж� SIG �ź�����ʱ��Ҫ���� IBP PSL PSC ʹ���ź��е����߼���� SIG���������� */
        {
            if((log1IBPPSL_En == 0) && (bPEDC_LOG1_TestMode_En == 0))
            {
                if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
                if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
            }
        }


        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG1MON].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG1MONLinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG1MONLinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG1MON);
    }

    /* �߼���2 �� ���Ӱ� ������
     * ����Ŀֻ�� �źſ���������
     *  */
    if((bPEDC_LOG2_OFFLINE == 0))
    {
        compLink.bytes[0] = log2Link.bytes[0] ^ monLink.bytes[0];
        compLink.bytes[1] = log2Link.bytes[1] ^ monLink.bytes[1];
        lnkMsg = PCBLINK_ERR_NONE;

        /* �ж� SIG �ź�����ʱ��Ҫ���� IBP PSL PSC ʹ���ź��е����߼���� SIG���������� */
        {
            if((log2IBPPSL_En == 0) && (bPEDC_LOG2_TestMode_En == 0))
            {
                if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
                if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
            }
        }

        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG2MON].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG2MONLinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG2MONLinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG2MON);
    }

    /* �߼���1 �� �߼���2 ������ */
    if((bPEDC_LOG1_OFFLINE == 0) && (bPEDC_LOG2_OFFLINE == 0))
    {
        compLink.bytes[0] = log1Link.bytes[0] ^ log2Link.bytes[0];
        compLink.bytes[1] = log1Link.bytes[1] ^ log2Link.bytes[1];

        if(compLink.LinkBit.IBP_En           != 0)  {lnkMsg = PCBLINK_ERR_IBP_EN;        }
        else if(compLink.LinkBit.IBP_Opn     != 0)  {lnkMsg = PCBLINK_ERR_IBP_OPN;       }
        else if(compLink.LinkBit.IBP_Cls     != 0)  {lnkMsg = PCBLINK_ERR_IBP_CLS;       }
        else if(compLink.LinkBit.IBP_OpnSD   != 0)  {lnkMsg = PCBLINK_ERR_IBP_OPNSIDE;   }

        else if(compLink.LinkBit.PSL1_En     != 0)  {lnkMsg = PCBLINK_ERR_PSL1_EN;       }
        else if(compLink.LinkBit.PSL1_Opn    != 0)  {lnkMsg = PCBLINK_ERR_PSL1_OPN;      }
        else if(compLink.LinkBit.PSL1_Cls    != 0)  {lnkMsg = PCBLINK_ERR_PSL1_CLS;      }

        else if(compLink.LinkBit.PSL2_En     != 0)  {lnkMsg = PCBLINK_ERR_PSL2_EN;       }
        else if(compLink.LinkBit.PSL2_Opn    != 0)  {lnkMsg = PCBLINK_ERR_PSL2_OPN;      }
        else if(compLink.LinkBit.PSL2_Cls    != 0)  {lnkMsg = PCBLINK_ERR_PSL2_CLS;      }

        else if(compLink.LinkBit.SIG_Opn     != 0)  {lnkMsg = PCBLINK_ERR_SIG_OPN;       }
        else if(compLink.LinkBit.SIG_Cls     != 0)  {lnkMsg = PCBLINK_ERR_SIG_CLS;       }
        else                                        {lnkMsg = PCBLINK_ERR_NONE;          }

        linkErrFlag = FALSE;
        TrueSetFLAG(lnkMsg > 0   , checkingTim[TIM_LINKERR_LOG1LOG2].posTim, 120, linkErrFlag);
        if(linkErrFlag)
        {
            PEDC_LOG1LOG2LinkMsg.byte &= ~LOGLINKERR_MASK_BITS;
            PEDC_LOG1LOG2LinkMsg.byte |= lnkMsg;
        }
    }
    else
    {
        RESET_CHECKINGTIME(TIM_LINKERR_LOG1LOG2);
    }
    #endif



    /* ===== �����߼����������� ===== */
    #if 1 /* �۵����� : �����߼����������� */
    if(bLOG_WorkLOG2 == 0)  /* ���� getWorkLOGID() == COMMOBJ_LOG1�������ֶ�ѡ��ʱ���� */
    {
        pLogIO     = &log1PCBIO;
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG1);
    }
    else
    {
        pLogIO     = &log2PCBIO;
        pSlvInfo = getSlaveInfoPtr(COMMOBJ_LOG2);
    }

    /* ��������߼��Ƿ���Ч���?
     * �����ȡ��������߼��Ĳ���������Ϣ
     *  */
    memcpy((int8u *) &OCmd, (int8u *)&pSlvInfo->slaveInput.logIn.UB1_outLogic, sizeof(OCmd));
    if(pSlvInfo->slaveInput.logIn.UB1_OutLogicNo != NONE_LOGICOUT)
    {
        switch((tLogicType)OCmd.outType)
        {
            /* IBP ���
             * ����߼���� */
            case IBP_LOGIC:
            {
                bPedcIBP_EN         = OCmd.outCmd.Enable;
                bPedcIBP_CLS        = OCmd.outCmd.DCUCLS8;  /* �������ź��ر��� */
                bPedcIBP_OPN8       = OCmd.outCmd.DCUOPN8;  /* ��������         */
                bPedcIBP_OPNSD      = OCmd.outCmd.DCUOPN6;  /* �� OPN6 ������   */
            }
            break;

            /* PSL ��� */
            case PSL_LOGIC:
            {
                switch(OCmd.outSeq)
                {
                    case 0:         /* PSL1 ��� : ʹ�ܡ����š����� */
                        bPedcPSL1_EN    = OCmd.outCmd.Enable;
                        bPedcPSL1_CLS   = OCmd.outCmd.DCUCLS8;
                        bPedcPSL1_OPN8  = OCmd.outCmd.DCUOPN8;
                        break;

                    case 1:         /* PSL2 ��� : ʹ�ܡ����š����� */
                        bPedcPSL2_EN    = OCmd.outCmd.Enable;
                        bPedcPSL2_CLS   = OCmd.outCmd.DCUCLS8;
                        bPedcPSL2_OPN8  = OCmd.outCmd.DCUOPN8;
                        break;

                    case 2:         /* PSL3 ��� : ʹ�ܡ����š����� */
                        break;

                    default:
                        break;
                }
            }
            break;

            /* PSC ��� */
            case PSC_LOGIC:
            {
            }
            break;

            default:
            {
            }
            break;
        }
    }
    else
    {
        /* ��������߼�û�� IBP PSL �������Ĭ�ϵ�ǰ�� SIG ���� */
    }

    /* PEDC ϵͳ���������ѡ����߼����������DCU��Ӳ������
     * ���߼����ϼ̵����Ĵ���״̬ */
    LOGOut_Relay.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* ��������1    DCU_CLS8_1 */
    LOGOut_Relay.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* ��������1    DCU_OPN8_1 */
    LOGOut_Relay.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* ��������2    DCU_CLS8_2 */
    LOGOut_Relay.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* ��������2    DCU_OPN8_2 */
    LOGOut_Relay.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* �ر���1                 */
    LOGOut_Relay.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* ������1                 */
    LOGOut_Relay.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* �ر���2                 */
    LOGOut_Relay.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* ������2                 */

    /* bPedc_OUT_CLS6_1  bPedc_OUT_CLS6_2 ... */
    PEDC_PSC1.byte = LOGOut_Relay.byte;

    BOOL IBPPSL_En = bPedcIBP_EN || bPedcPSL1_EN || bPedcPSL2_EN;
    #endif

    /*===== �ź�ϵͳ���������� =====*/
    #if 1   /* �����۵� : �źſ������� */
    /* ���Ӱ��ԭʼ����
     *  : MI_SIG_OpnCmd / MI_SIG_ClsCmd
     */
    /*
    if((IBPPSLPSC_En == 0))
     *  */
    {
        bPedcSIG_OPN8   = GET_INPUT_STS(MI_SIG_OpnCmd);
        bPedcSIG_CLS    = GET_INPUT_STS(MI_SIG_ClsCmd);

        //���û�� ���Ӱ���ӵ㣬��ֻ�����߼��������
        //bPedcSIG_OPN8   = GET_INPUT_STS(L1_JI_SIG_OPN8) | GET_INPUT_STS(L2_JI_SIG_OPN8);
        //bPedcSIG_CLS    = GET_INPUT_STS(L1_JI_SIG_CLS)  | GET_INPUT_STS(L1_JI_SIG_CLS);

        /* ����ź�ϵͳӲ�߿����ţ�������ͬʱ������Ч�������ô��� */
        TrueSetFLAG((bPedcSIG_CLS && bPedcSIG_OPN8) ,checkingTim[TIM_LOG_SIGCMDERR].posTim, 200, bPedc_ATCCMD_FAILURE);
    }

    #endif

    #if 1 /* �����۵�: PSC ��������� */
    /*===== PSC ��� =====*/
    /* PSL��·���� */
    bPedcPSL1_ByPass = GET_INPUT_STS(MI_PSL1_Bypass);     /* PSL1 BYPASS  */
    bPedcPSL2_ByPass = GET_INPUT_STS(MI_PSL2_Bypass);     /* PSL2 BYPASS  */

    /* ��Դ���ϼ�� */
    PEDC_POWER_WARNING.byte = 0;
    bPowerWarning_S0        = GET_INPUT_STS(MI_MainPowFail);    /* POW-S0  ����Դ����     */
    bPowerWarning_S1        = GET_INPUT_STS(MI_DriverPowFail);  /* POW-S1  ������Դ����   */
    bPowerWarning_S2        = GET_INPUT_STS(MI_CtrlPowFail);    /* POW-S2  ���Ƶ�Դ����   */
    bPowerWarning_S3        = GET_INPUT_STS(MI_DriverBatFail);  /* POW-S3  ������ع���   */
    bPowerWarning_S4        = GET_INPUT_STS(MI_CtrlBatFail);    /* POW-S4  ���Ƶ�ع���   */

    /* ��Դ����״̬   */
    if( PEDC_POWER_WARNING.byte != 0 )
    {
        Temp_Alarm0.bAlarm_POWER_FAULT = SETBIT;
    }

    /*===== ����������� =====*/
    /*
     * �������� 1��2����������1��2
     * �жϹ���״̬: ����ϵͳ���� => 4��������������
     */
    /* MON ������������ӵ� */
    MON_CmdMon.byte = 0;
    MON_CmdMon.Cls1         = GET_INPUT_STS(MI_DCU_CLS8_1);     /* ��������1    DCU_CLS6_1 */
    MON_CmdMon.Opn1         = GET_INPUT_STS(MI_DCU_OPN8_1);     /* ��������1    DCU_OPN6_1 */
    MON_CmdMon.Cls2         = GET_INPUT_STS(MI_DCU_CLS8_2);     /* ��������2    DCU_CLS6_2 */
    MON_CmdMon.Opn2         = GET_INPUT_STS(MI_DCU_OPN8_2);     /* ��������2    DCU_OPN6_2 */
    MON_CmdMon.ClsSide1     = GET_INPUT_STS(MI_DCU_CLSSD_1);    /* �ر���1                 */
    MON_CmdMon.OpnSide1     = GET_INPUT_STS(MI_DCU_OPNSD_1);    /* ������1                 */
    MON_CmdMon.ClsSide2     = GET_INPUT_STS(MI_DCU_CLSSD_2);    /* �ر���2                 */
    MON_CmdMon.OpnSide2     = GET_INPUT_STS(MI_DCU_OPNSD_2);    /* ������2                 */
    
    /* ���������
     *     1. ����������
     *     2. ���ر�������
     */
    #if 1
    int8u cmpResult;
    cmpResult = MON_CmdMon.byte ^ LOGOut_Relay.byte;
    
    TrueSetFLAG(((cmpResult & 0x0f ) !=0),  checkingTim[TIM_MON_OPNCLS_ERR].posTim  , 120, bMON_OUTOPNCLS_Fault);
    TrueSetFLAG(((cmpResult & 0xf0 ) !=0),  checkingTim[TIM_MON_IBPOPNSD_ERR].posTim, 120, bMON_OUTOPNSD_Fault);
    
    #else
    TrueSetFLAG(((LOGOut_Relay.Cls1 != MON_CmdMon.Cls1) || (LOGOut_Relay.Cls2 != MON_CmdMon.Cls2) ||    \
                 (LOGOut_Relay.Opn1 != MON_CmdMon.Opn1) || (LOGOut_Relay.Opn2 != MON_CmdMon.Opn2)),     \
                  checkingTim[TIM_MON_OPNCLS_ERR].posTim, 120, bMON_OUTOPNCLS_Fault);
    
    TrueSetFLAG(((LOGOut_Relay.ClsSide1 != MON_CmdMon.ClsSide1) || (LOGOut_Relay.ClsSide2 != MON_CmdMon.ClsSide2) ||    \
                 (LOGOut_Relay.OpnSide1 != MON_CmdMon.OpnSide1) || (LOGOut_Relay.OpnSide2 != MON_CmdMon.OpnSide2)),     \
                  checkingTim[TIM_MON_IBPOPNSD_ERR].posTim, 120, bMON_OUTOPNSD_Fault);
    #endif
    
    bMON_CtrlCmd_Fault = bMON_OUTOPNCLS_Fault | bMON_OUTOPNSD_Fault;

    /*--------------------------------------
     * PSC��PSL��IBP�ϵ������Źر�������ָʾ��
     * */
    int8u LedClsAndLocked;
    LedClsAndLocked = bPedc_CLSLCK;

    /* PEDC ����:
     * ��չ������ ( bPEDC_XXX_OFFLINE )
     * PIC24���߻�
     *      PIC24 I2C���ʴ���=> ifPCA9505Err(COMMOBJ_IORW_A) */
    BOOL PEDCFault = (bPEDC_SWCH_OFFLINE ||  (bPEDC_LOG1_OFFLINE || bPEDC_LOG2_OFFLINE) || bPEDC_PIC24A_OFFLINE || bPEDC_PIC24B_OFFLINE);
                    // (bPEDC_PIC24A_OFFLINE || ifPCA9505Err(COMMOBJ_IORW_A)) || \
                    // (bPEDC_PIC24B_OFFLINE || ifPCA9505Err(COMMOBJ_IORW_B)));

    /*--------------------------------------
     * PSC �����沼��
     * */
    /* J1-02  PSC-Q00     PSC-����״ָ̬ʾ�� 
     * PEDC�����������������һ�����Ǵ��ڿ���״̬ʱ����
     * ���Ǽ��� ��ȫ��· ͨ����ָʾ: MO_Led_PSC_OpnSta       = LEDON_IF_STS0(bPedc_CLSLCK);
     *                  */
    if(MON_CmdMon.Opn1 || MON_CmdMon.Opn2 || MON_CmdMon.OpnSide1 || MON_CmdMon.OpnSide2)
    {
        MO_Led_PSC_OpnSta = LEDON_IF_STS0(bPedc_CLSLCK);
    }
    

    /* J1-03  PSC-Q01     PSC-����ASD/EED�ر�����ָʾ��       */
    MO_Led_PSC_DoorLcked    = LEDON_IF_STS1(LedClsAndLocked);

    /* J1-04  PSC-Q02     PSC-�������ֶ�/�������ָʾ��       */
    MO_Led_PSC_ManUnlock    = LEDON_IF_STS1(Down_Message0.bMANUAL_MODE | Down_Message0.bIsolation_MODE);

    /* J1-05  PSC-Q03     PSC-�����ſ���/���Ź���ָʾ��       */
    MO_Led_PSC_OpnClsFail   = LEDON_IF_STS1(Down_Message0.bOPN_FAULT | Down_Message0.bCLS_FAULT);

    /* J1-06  PSC-Q04     PSC-IBP��������ָʾ��               */
    MO_Led_PSC_IBPEn        = LEDON_IF_STS1(bPedcIBP_EN);

    /* J1-07  PSC-Q05     PSC-PSL�������ָʾ��               */
    MO_Led_PSC_PSLByPass    = LEDON_IF_STS1(bPedcPSL1_ByPass | bPedcPSL2_ByPass);

    /* J1-08  PSC-Q06     PSC-�����Դ����ָʾ��              */
    MO_Led_PSC_PowerFail    = LEDON_IF_STS1(Temp_Alarm0.bAlarm_POWER_FAULT);

    /* J1-09  PSC-Q07     PSC-�ֳ����߹���ָʾ��              */
    MO_Led_PSC_CanFail      = LEDON_IF_STS1(Down_Message0.bBUS_FAULT);

    /* J1-10  PSC-Q08     PSC-����ϵͳ����ָʾ�� : PEDC ����ʱ����--> PEDCFault �� bMON_CtrlCmd_Fault */
    MO_Led_CtrlSystemFail   = LEDON_IF_STS1(bMON_CtrlCmd_Fault | PEDCFault);

    /* J1-12  PSC-Q10     PSC--����ϵͳ����ָʾ��: PEDC�빤�ػ�ͨ�ų�ʱ����             */
    MO_Led_MonitorSysFail   = LEDON_IF_STS1(Down_Message0.bMONITOR_FAULT);

    /* J1-13  PSC-Q11     PSC-?���ƴ�                        */
    MO_LookoutLight = checkSysTimeForLightBelt();
    
    /*  J2-14  Q-PN1       PSC-�����ƴ�(2022-4-13 �޸�)
     * ��?���ƴ�ͬ������ */
    MO_IlluminationLight = MO_LookoutLight;
    
    /* ������������Ϣ
     *  */
    Temp_Alarm0.bAlarm_BUS_FAULT      = Down_Message0.bBUS_FAULT;
    Temp_Alarm0.bAlarm_OPN_FAULT      = Down_Message0.bOPN_FAULT;
    Temp_Alarm0.bAlarm_CLS_FAULT      = Down_Message0.bCLS_FAULT;
    Temp_Alarm0.bAlarm_Monitor_Fault  = Down_Message0.bMONITOR_FAULT;
    Temp_Alarm0.bAlarm_LOGPCB_FAULT   = (getLOGPcbState() == 0) ? 0 : 1;
    
    #if 1
    /* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ�
     * ע���ʱ��λ�� RELAY_TIMER_INTERVAL */
    static int8u prev_WorkSIG = 0xff;
    if(prev_WorkSIG == 0xff)
    {
        prev_WorkSIG = bLOG_WorkLOG2;
        checkingTim[TIM_AutoSelectLOG2].posTim = 0;
    }
    else
    {
        TrueSetFLAG(((bLOG_Sw_Auto == 1) && (prev_WorkSIG != bLOG_WorkLOG2)),  checkingTim[TIM_AutoSelectLOG2].posTim  , TIME_VALUE_BASE_10(2000), Temp_Alarm0.bAlarm_AutoSelLog2);
        bLOG1_2_SwitchAlarm = Temp_Alarm0.bAlarm_AutoSelLog2;   /* ����: ���� PEDC ��������۲� */
    }
    #else
    /* 2024-6-3 .�Զ�λʱ���ư�2����Ҫ����������ע���ʱ��λԼ 8ms */
    TrueSetFLAG(((bLOG_Sw_Auto == 1) && (bLOG_WorkLOG2 == 1)),  checkingTim[TIM_AutoSelectLOG2].posTim  , 200, Temp_Alarm0.bAlarm_AutoSelLog2);
    #endif
    
    /* ��λ��ť /������ť */
    if(MI_ResetKey == Input_VALID)
    {
        PSC_AlarmFlags.word = Temp_Alarm0.word;
        
        /* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ� */
        prev_WorkSIG = bLOG_WorkLOG2;
    }

    /* �жϣ�״̬ ���޵��У��� ��0��1���򱨾� */
    if(((PSC_AlarmFlags.word ^ 0xffff) & Temp_Alarm0.word ) != 0)
    {
        /* MMS, 1��ֹ BUZZER ��� ? */
        if(!FORBID_BUZZER)
        {
            MO_PSC_Buzzer = BUZZER_ON;
        }
    }
	else
	{
        /* ���ݵĹ�����ʧ,��ô���ù��ϴӱ��������� */
		PSC_AlarmFlags.word = Temp_Alarm0.word;
	}
    #endif

    #if 1 /* �����۵� : IBP ������� */
    /*--------------------------------------
     * IBP �������
     * �޷���������������ť�����Եư�ť
     * */
    /*
     * ���ָʾ��
     *     IBP-վ̨�Źر�ָʾ��
     *     IBP-վ̨�ſ���ָʾ��
     *     IBP-��ĩ�ſ���ָʾ��
     */

    /* J3-02  IBP-Q0      IBP-վ̨�Źر�ָʾ��   */
    MO_Led_IBP_DoorClsed   = LEDON_IF_STS1(LedClsAndLocked);

    /* J3-03  IBP-Q1      IBP-վ̨�ſ���ָʾ�� (������ȫ����ʱ���� )   */
    MO_Led_IBP_DoorOpned   = LEDON_IF_STS1(Down_Message0.bDOOR_ALL_OPEN);

    /* J3-04  IBP-Q2      IBP-��ĩ�ſ���ָʾ��
     * 2022-8-22 ��Ϊ : ���ź�ĩ��ͬʱȫ����λ������������롯����ǰ�ǡ��򡯣�  */
    MO_Led_IBP_HeadTailOpn   = LEDON_IF_STS1(Down_Message0.bTailASD_Opned & Down_Message0.bHeadASD_Opned);

    /* IBP �������������� �� ��
     */

    /* IBP ��λ��ť /������ť : ��
     *  */
    #endif

    #if 1 /* �����۵� : PSL ������� */
    /*--------------------------------------
     * PSL1 & PSL2 & PSL3 �������
     * */
    /* վ̨��ȫ����ָʾ��
     * PSL 1 2 ������������
     *  */
    MO_Led_PSLx_AllOpned  = LEDON_IF_STS1(Down_Message0.bDOOR_ALL_OPEN);

    /* վ̨��ȫ��ָʾ��
     * PSL 1 2 ������������
     *  */
    MO_Led_PSLx_AllClsed  = LEDON_IF_STS1(LedClsAndLocked);

    /* PSL1  */
    MO_Led_PSL1_IBPEn       = LEDON_IF_STS1(bPedcIBP_EN);       /* J2-03  PSL-Q01     PSL1-IBP��������ָʾ��    */
    MO_Led_PSL1_EN          = LEDON_IF_STS1(bPedcPSL1_EN);      /* J2-06  PSL-Q04     PSL1-��������ָʾ��       */
    MO_Led_PSL1_Bypass      = LEDON_IF_STS1(bPedcPSL1_ByPass);  /* J2-07  PSL-Q05     PSL1-�������ָʾ��       */
    MO_Led_PSL1Interlock    = LEDON_IF_STS1(bPedcPSL2_EN);      /* J2-10  PSL-Q08     PSL1-ͬ�໥��ָʾ��       */

    /* PSL2  */
    MO_Led_PSL2_IBPEn       = LEDON_IF_STS1(bPedcIBP_EN);       /* J2-05  PSL-Q03     PSL2-IBP��������ָʾ��    */
    MO_Led_PSL2_EN          = LEDON_IF_STS1(bPedcPSL2_EN);      /* J2-08  PSL-Q06     PSL2-��������ָʾ��       */
    MO_Led_PSL2_Bypass      = LEDON_IF_STS1(bPedcPSL2_ByPass);  /* J2-09  PSL-Q07     PSL2-�������ָʾ��       */
    MO_Led_PSL2Interlock    = LEDON_IF_STS1(bPedcPSL1_EN);      /* J2-12  PSL-Q10     PSL2-ͬ�໥��ָʾ��       */
    #endif

    #if 1 /* �Եư�ť */
    /*--------------------------------------
     * PSC �Եư�ť
     * */
    if(MI_TestLight == Input_VALID)
    {
        MO_Led_PSC_OpnSta       = LED_ON;   /* J1-02  PSC-Q00     PSC-����״ָ̬ʾ��                  */
        MO_Led_PSC_DoorLcked    = LED_ON;   /* J1-03  PSC-Q01     PSC-����ASD/EED�ر�����ָʾ��       */
        MO_Led_PSC_ManUnlock    = LED_ON;   /* J1-04  PSC-Q02     PSC-�������ֶ�/�������ָʾ��       */
        MO_Led_PSC_OpnClsFail   = LED_ON;   /* J1-05  PSC-Q03     PSC-�����ſ���/���Ź���ָʾ��       */
        MO_Led_PSC_IBPEn        = LED_ON;   /* J1-06  PSC-Q04     PSC-IBP��������ָʾ��               */
        MO_Led_PSC_PSLByPass    = LED_ON;   /* J1-07  PSC-Q05     PSC-PSL�������ָʾ��               */
        MO_Led_PSC_PowerFail    = LED_ON;   /* J1-08  PSC-Q06     PSC-�����Դ����ָʾ��              */
        MO_Led_PSC_CanFail      = LED_ON;   /* J1-09  PSC-Q07     PSC-�ֳ����߹���ָʾ��              */
        MO_Led_CtrlSystemFail   = LED_ON;   /* J1-10  PSC-Q08     PSC-����ϵͳ����ָʾ��              */
        MO_PSC_Buzzer           = LED_ON;   /* J1-11  PSC-Q09     PSC-������                          */
        MO_Led_MonitorSysFail   = LED_ON;   /* J1-12  PSC-Q10     PSC--����ϵͳ����ָʾ��             */
        
        //�ԵƲ��õ���
        //MO_LookoutLight         = LED_ON;   /* J1-16&17  Q-PN0    PSC-?���ƴ�          */
        //MO_IlluminationLight    = LED_ON;   /* J2-14  Q-PN1       PSC-�����ƴ� 1       */
    }

    /*--------------------------------------
     * PSL1 �Եư�ť
     * */
    if(MI_PSL1TestLight == Input_VALID)
    {
        MO_Led_PSLx_AllOpned    = LED_ON;   /* J2-02  PSL-Q00     PSLx-վ̨��ȫ����ָʾ�� */
        MO_Led_PSLx_AllClsed    = LED_ON;   /* J2-04  PSL-Q02     PSLx-վ̨��ȫ��ָʾ��   */
        MO_Led_PSL1_IBPEn       = LED_ON;   /* J2-03  PSL-Q01     PSL1-IBP��������ָʾ��  */
        MO_Led_PSL1_EN          = LED_ON;   /* J2-06  PSL-Q04     PSL1-��������ָʾ��     */
        MO_Led_PSL1_Bypass      = LED_ON;   /* J2-07  PSL-Q05     PSL1-�������ָʾ��     */
        MO_Led_PSL1Interlock    = LED_ON;   /* J2-10  PSL-Q08     PSL1-ͬ�໥��ָʾ��     */
    }

    /*--------------------------------------
     * PSL2 �Եư�ť
     * */
    if(MI_PSL2TestLight == Input_VALID )
    {
        MO_Led_PSLx_AllOpned    = LED_ON;   /* J2-02  PSL-Q00     PSLx-վ̨��ȫ����ָʾ�� */
        MO_Led_PSLx_AllClsed    = LED_ON;   /* J2-04  PSL-Q02     PSLx-վ̨��ȫ��ָʾ��   */
        MO_Led_PSL2_IBPEn       = LED_ON;   /* J2-05  PSL-Q03     PSL2-IBP��������ָʾ��  */
        MO_Led_PSL2_EN          = LED_ON;   /* J2-08  PSL-Q06     PSL2-��������ָʾ��     */
        MO_Led_PSL2_Bypass      = LED_ON;   /* J2-09  PSL-Q07     PSL2-�������ָʾ��     */
        MO_Led_PSL2Interlock    = LED_ON;   /* J2-12  PSL-Q10     PSL2-ͬ�໥��ָʾ��     */
    }

    /*--------------------------------------
     * PSL3 �Եư�ť : ��
     * */

    /*--------------------------------------
     * IBP �Եư�ť : ��
     * */
    #endif

    /*--------------------------------------
     * �жϴ�����Ϣ
     * */
    #if 1 /* �۵�����: �жϴ�����Ϣ */
	if((bPEDC_LOG1_OFFLINE == 0) || (bPEDC_LOG2_OFFLINE == 0))
	{
        /* ��ʱ��Ϣ���ݸ� MMS ��Ϣ */
        MON_LinkErr_OPNSD       = bMON_OUTOPNSD_Fault;
        MON_LinkErr_CTRLCMD     = bMON_OUTOPNCLS_Fault;
	}

    BOOL clr_TIM_SWCH_Q1Q2          = TRUE;
    BOOL clr_TIM_LinkErr_CMD_LOG1   = TRUE;
    BOOL clr_TIM_LinkErr_CMD_LOG2   = TRUE;
    if((bPEDC_SWCH_OFFLINE == 0))
    {
        /* �ж��л�����·ѡ��:
         * �л���: J3����15��16��һ��   */
        TrueSetFLAG((SWCH_K1_POW != SWCH_K4_POW),checkingTim[TIM_SWCH_Q1Q2].posTim, 50, SWCH_LinkErr_Q1AQ2A);
        clr_TIM_SWCH_Q1Q2 = TRUE;
        
        tChkOutputCmd swOutput;     /* �л��������ʵ���Ǽ��Ӱ��������ӵ� */
        tChkOutputCmd logOutput;    /* �߼���������ӵ�                     */
        swOutput.byte   = MON_CmdMon.byte;
        logOutput.byte  = 0;

        /* �߼��� ���� SWITCHER �Ľ���
         * ������ж��ƺ��� �߼��嵽���Ӱ���ж��ظ��ˣ�
         * ��Ϊ�߼���û�м��� 7 �������������������ü��Ӱ������
         *  */

        /* ������Ϊ K1 K4 ��һ�µģ���һ�»ᱨ���� SWCH_LinkErr_Q1AQ2A */
        if(SWCH_K1_POW == INVALID)
        {
            /* K1 K2 K3 û��ͨ�磬ѡ�� IN-B������������ LOG1
             * ��ʱ�߼������������л��壬�л��峣�մ��������ӵ����Ӱ壬�л���û�м������룬
             * ֻ���ü��Ӱ����Ϣ�ж�����
             *  */
            if((bPEDC_LOG1_OFFLINE == 0))
            {
                pLogIO     = &log1PCBIO;

                logOutput.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* ��������1    DCU_CLS8_1 */
                logOutput.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* ��������1    DCU_OPN8_1 */
                logOutput.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* ��������2    DCU_CLS8_2 */
                logOutput.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* ��������2    DCU_OPN8_2 */
                logOutput.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* �ر�������1             */
                logOutput.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* ����������1             */
                logOutput.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* �ر�������2             */
                logOutput.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* ����������2             */

                BOOL tmpErr = FALSE;
                if(swOutput.byte != logOutput.byte)
                {
                    tmpErr = TRUE;
                }

                TrueSetFLAG(tmpErr   , checkingTim[TIM_LinkErr_CMD_LOG1].posTim, 120, SWCH_LinkErr_CMD_LOG1);
                clr_TIM_LinkErr_CMD_LOG1 = FALSE;
            }
        }
        else
        {
        
            /* K1 K2 K3 ͨ�磬ѡ�� IN-A������������ LOG2
             * ��ʱ�߼������������л��壬�л��峣�մ��������ӵ����Ӱ壬�л���û�м������룬
             * ֻ���ü��Ӱ����Ϣ�ж�����
             *  */
            if((bPEDC_LOG2_OFFLINE == 0))
            {
                pLogIO     = &log2PCBIO;

                logOutput.Cls1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_1);       /* ��������1    DCU_CLS8_1 */
                logOutput.Opn1     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_1);       /* ��������1    DCU_OPN8_1 */
                logOutput.Cls2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS8_2);       /* ��������2    DCU_CLS8_2 */
                logOutput.Opn2     = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN8_2);       /* ��������2    DCU_OPN8_2 */
                logOutput.ClsSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_1);       /* �ر�������1             */
                logOutput.OpnSide1 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_1);       /* ����������1             */
                logOutput.ClsSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_CLS6_2);       /* �ر�������2             */
                logOutput.OpnSide2 = GET_INPUT_STS(pLogIO->fun.CO_DCU_OPN6_2);       /* ����������2             */

                BOOL tmpErr = FALSE;
                if(swOutput.byte != logOutput.byte)
                {
                    tmpErr = TRUE;
                }

                TrueSetFLAG(tmpErr   , checkingTim[TIM_LinkErr_CMD_LOG2].posTim, 120, SWCH_LinkErr_CMD_LOG2);
                clr_TIM_LinkErr_CMD_LOG2 = FALSE;
            }
        }
    }
    
    if( clr_TIM_SWCH_Q1Q2 == TRUE )
        RESET_CHECKINGTIME(TIM_SWCH_Q1Q2);
    if( clr_TIM_LinkErr_CMD_LOG1 == TRUE )
        RESET_CHECKINGTIME(TIM_LinkErr_CMD_LOG1);
    if( clr_TIM_LinkErr_CMD_LOG2 == TRUE )
        RESET_CHECKINGTIME(TIM_LinkErr_CMD_LOG2);
    
    #endif



    //Handle_Message();                       //�������߹��ϵȵ�

    /* ����������� GPIO B ������
     * PORTRead ���Զ���������IOֵ
     *  PORTSetBits(IOPORT_B, BIT_2);
     *  outPBData = PORTRead(IOPORT_B);
     *  */
    int len;
    int8u *pOut;
    unsigned int outPBData;
    pOut = (int8u *)getPic24OutputIODataPtr(&len);
    outPBData = PORTRead(IOPORT_B);
    outPBData &= ~(BIT_2|BIT_3|BIT_5) ;
    outPBData |= pOut[MON_Output_RBL_OFFSET] & (BIT_2|BIT_3|BIT_5);
    PORTWrite(IOPORT_B, outPBData);
}

/* =============================================================================
 * ���ڼ̵��������жϵ�  ʱ���ʱ
 * ��ʱ���� 1ms
 */
void RelayTimerRun(void)
{
    static tSYSTICK run8ms = 0;
    static tSYSTICK prevTick = 0;
    tSYSTICK tickNow, xMs;
    int8u i;

    tickNow = getSysTick();
    if(tickNow != prevTick)
    {
        xMs = (tSYSTICK)(tickNow - prevTick);
        prevTick = tickNow;

        run8ms += xMs;

        if(run8ms >= RELAY_TIMER_INTERVAL)
        {
            run8ms -= RELAY_TIMER_INTERVAL;

            for(i=0; i<TOTAL_RELAYS; i++)
            {
                if(relayWorkTime[i].posTim < 255)
                {
                    relayWorkTime[i].posTim ++;
                }

                if(relayWorkTime[i].revTim < 255)
                {
                    relayWorkTime[i].revTim ++;
                }
            }

            for(i=0; i<TOTAL_CONTACTOR_TIMERS; i++)
            {
                if(checkingTim[i].posTim < 60000)
                {
                    checkingTim[i].posTim ++;
                }
            }
        }
    }
}

/* �������õ�?���ƴ���ʼʱ��ͽ���ʱ��
 * �жϵ�ǰ�Ƿ���Ҫ����?���ƴ�
 * ����ֵ ֱ�Ӹ�ֵ�� IO���ֵ (LED_ON / LED_OFF)
 *  */
int8u checkSysTimeForLightBelt(void)
{
    BOOL  bNormal = TRUE;
    int8u lightOn = LED_ON;
    
    /* ��ȡ��ǰPEDC��ϵͳ����ʱ�� */
    tSysTime nowPedcTime;
    getPedcSysTime(&nowPedcTime);
    
    int32u onSec, offSec, nowSec;
    
    do
    {
        /* �ж�PEDCϵͳ����ʱ���Ƿ���Ч : ifPEDCSysTimeValid()
         *                OVERSEE_LIGHT : ���õ�?���ƴ���Ч��־ 1=��Ч
         *  */
        if((ifPEDCSysTimeValid() == FALSE) || (OVERSEE_LIGHT == 0))
        {   /* û�յ�����ϵͳʱ������ƿ���ʱ�䣬��ʼ�յ��� */
            break;
        }
        
        /* ��ʼʱ��ͽ���ת������ */
        onSec   = (int32u)tagDownIbpAlarmParameter.uRead_On_Hour  * 3600 + (int32u)tagDownIbpAlarmParameter.uRead_On_Minute  * 60;
        offSec  = (int32u)tagDownIbpAlarmParameter.uRead_Off_Hour * 3600 + (int32u)tagDownIbpAlarmParameter.uRead_Off_Minute * 60;
        if(onSec == offSec)
        {   /* ��ʼʱ�� == ����ʱ�䣬��ʼ�յ��� */
            break;
        }

        /* �ж�ʱ��ο�ʼʱ���Ƿ�С��ʱ��ν���ʱ�䣬������Ǿͽ��� */
        if(onSec > offSec)
        {
            nowSec  = onSec;
            onSec   = offSec;
            offSec  = nowSec;
            bNormal = FALSE;
        }
        nowSec  = (int32u)nowPedcTime.hour * 3600 + (int32u)nowPedcTime.min * 60 + (int32u)nowPedcTime.second;
        
        if((nowSec >= onSec) && (nowSec < offSec))
        {
            /* ��ǰʱ������ʱ����� */
            if(bNormal == FALSE)
                lightOn = LED_OFF;  /* ���� : ������ǰʱ����������ʱ����� */
        }
        else
        {
            /* ��ǰʱ������ʱ����� */
            if(bNormal == TRUE)
                lightOn = LED_OFF;  /* ���� : ������ǰʱ����������ʱ����� */
        }
    }while(FALSE);
    
    return lightOn;
}
