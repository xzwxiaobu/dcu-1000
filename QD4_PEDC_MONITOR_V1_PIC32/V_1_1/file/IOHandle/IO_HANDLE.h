/* PROJECT : SZ16 - prototype 
 *          PCB FD_PEDC_MONITOR_SZ20_V1.pdf 
 */
//--------------------------------------
#ifndef _IO_HANDLE_H
#define _IO_HANDLE_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
//--------------------------------------
// 
//--------------------------------------

#define VALID                   0               //�̵�������
#define INVALID                 1
#define GET_INPUT_STS(input)    (!(input))      //������״̬ 0
#define GET_INPUT_STS_1(input)  ((input))       //������״̬ 1

#define RELAY_POW_ON(relay)     (!(relay))      /* �̵���ͨ��   */
#define RELAY_POW_OFF(relay)    ( (relay))      /* �̵���δͨ�� */

#define RELAY_NC_ON(relay)      ( (relay))      /* ���մ���ͨ��   */
#define RELAY_NC_OFF(relay)     (!(relay))      /* ���մ���δͨ�� */

#define RELAY_NO_ON(relay)      (!(relay))      /* ��������ͨ��   */
#define RELAY_NO_OFF(relay)     ( (relay))      /* ��������δͨ�� */

#define LED_ON                  0               //LED ����
#define LED_OFF                 1
#define LEDON_IF_STS1(state1)   (!(state1))     //״̬��ЧʱΪ1 ����͵�ƽ���� LED
#define LEDON_IF_STS0(state0)   ( (state0))     //״̬��ЧʱΪ0 ����͵�ƽ���� LED

#define BUZZER_ON               0               //��������
#define BUZZER_OFF              1


#define SETBIT                  1               //���� 1
#define CLSBIT                  0

#define Input_VALID             0               //������
#define Input_INVALID           1               //û������

#define CBIGEQU                 0
#define CLESS                   1   
    

/* =============================================================================
 * ����չ���PIC24�������������ݶ���
 * ע�⣺��ʱ���˼�����ǰ�ĳ��ȣ�����ʵ�ʶ�ȡ���������˱任��
 *       �μ� mianIO.c : ���� getPCBInputData()
 *  */

#define    MON_INPUT_GPIO_LEN       12                  /*MCU GPIO��ȡ���������ݳ��ȣ�GPIO B~G */
#define    MON_INPUT_DATA_LEN       15                  /*��PIC24��ȡ���������ݳ��ȣ�3��PCA9505 */
#define    SWCH_INPUT_DATA_LEN      12                  /*���л�����ȡ���������ݳ��ȣ�GPIO B~G */
#define    LOG1_INPUT_DATA_LEN      15                  /*���߼���1���ȡ���������ݳ��ȣ�3��PCA9505 */
#define    LOG2_INPUT_DATA_LEN      15                  /*���߼���2���ȡ���������ݳ��ȣ�3��PCA9505 */

EXTN BIT8  MON_input_GPIO[MON_INPUT_GPIO_LEN];          /* : Monitor PCB (GPIO) */
EXTN BIT8  MON_input_data[MON_INPUT_DATA_LEN];          /* : Monitor PCB (I2C PCA9505������) */
EXTN BIT8  SWCH_input_data[SWCH_INPUT_DATA_LEN];        /* : �л��� PCB (PIC32 ������) */
EXTN BIT8  LOG1_input_data[LOG1_INPUT_DATA_LEN];        /* : �߼���1���� (PIC32 UB1 ������) */
EXTN BIT8  LOG2_input_data[LOG2_INPUT_DATA_LEN];        /* : �߼���2���� (PIC32 UB1 ������) */

#define OUTPUT_DATA_LEN             (15 + 1)            /* �����PIC24�����ݳ��ȣ�3��PCA9505 + ����PBL */
EXTN BIT8 MON_output_data[OUTPUT_DATA_LEN];             /* */

#define MON_Output_RBL_OFFSET       15
#define MON_Output_RBL              MON_output_data[MON_Output_RBL_OFFSET]

#if 0
//�л������ MON_Output_SFSW
typedef union
{
    struct
    {
        int8u   SFSW_Q0_Q1Swtich    : 1;    /* �л��� Q0 */
        int8u   SFSW_Q1_Q2Swtich    : 1;    /* �л��� Q1 */
        int8u   SFSW_Q2_Reset       : 1;    /* �л��� Q2 */
        int8u   SFSW_Q3_PN0         : 1;    /* �л��� Q3 */
        int8u                       : 4;       
    };
    
    int8u byte;
}tSFSWOutput;
#endif

/* �����PCB���������ĺ��� */
#include "PCB_IO_Def.h"

/* =============================================================================
 *  */
typedef union
{ 
    /* ע��˳���� ��MMS��Э���� ��Ӧ(PEDC_PSC1) */
    struct
    {
        int8u Cls1        : 1;       /* ��������1     */
        int8u Opn1        : 1;       /* ��������1     */
        int8u Cls2        : 1;       /* ��������2     */
        int8u Opn2        : 1;       /* ��������2     */ 
        int8u ClsSide1    : 1;       /* �ر���1       */
        int8u OpnSide1    : 1;       /* ������1       */
        int8u ClsSide2    : 1;       /* �ر���2       */
        int8u OpnSide2    : 1;       /* ������2       */
    };
    
    int8u byte;
}tChkOutputCmd;

/* =============================================================================
 * ���͸�PEDC����Ϣ
 * Relay Info = 20 bytes for SZ20 or SZ16
 * Extend data(interface IO) = 16 bytes
 */
EXTN BIT8 MESG_BUF[AVR_DATA_LEN];

#define PEDC_RELAY_SEAT     0
#define PEDC_PSC_SEAT       AVR_DATA_RELAY_LEN

/* ����� MMS �ļ̵�������չ��Ϣ */
#include "ToMMS_InfoDef.h"

/* =============================================================================
 * ���ڼ̵���״̬�жϵļ�ʱ��
 */
#define RELAY_TIMER_INTERVAL    10
#define TIME_VALUE_BASE_10(a)   ((a)/RELAY_TIMER_INTERVAL)
#define cRelay_positive_TIME  200                 //Լ 10ms * 200
#define cRelay_reverse_TIME   200                 //Լ 10ms * 200

//positive : �̵��������е�ѹ ���������һ��ʱ�䣬�̵�������û���������иü̵������ϡ�
//reverse  : �̵�������û�е�ѹ ���������һ��ʱ�䣬�̵�������һֱ���ֶ��������иü̵������ϡ�
typedef struct
{
    int8u posTim;
    int8u revTim;
}tRelayTime;

typedef enum
{    
    /* SZ16 �л��� */
    SWCH_K0     = 0 ,
    SWCH_K1         ,
    SWCH_K2         ,
    SWCH_K3         ,
    SWCH_K4         ,
    SWCH_K5         ,
    SWCH_K6         ,
    SWCH_K11        ,
    SWCH_K12        ,
    SWCH_K13        ,
    SWCH_K21        ,
    SWCH_K22        ,
    SWCH_K23        ,

    /* SZ16 �߼���1 */
    LOG1_K0         ,
    LOG1_K1A        ,
    LOG1_K1B        ,
    LOG1_K2A        ,
    LOG1_K2B        ,
    LOG1_K3A        ,
    LOG1_K3B        ,
    LOG1_K4A        ,
    LOG1_K4B        ,
    LOG1_K5A        ,
    LOG1_K5B        ,
    LOG1_K6A        ,
    LOG1_K6B        ,
    LOG1_K7A        ,
    LOG1_K7B        ,
    LOG1_K8A        ,
    LOG1_K8B        ,
    LOG1_K21        ,
    LOG1_K22        ,
    LOG1_K10        ,
    LOG1_K30        ,
            
    /* SZ16 �߼���2 */
    LOG2_K0         ,
    LOG2_K1A        ,
    LOG2_K1B        ,
    LOG2_K2A        ,
    LOG2_K2B        ,
    LOG2_K3A        ,
    LOG2_K3B        ,
    LOG2_K4A        ,
    LOG2_K4B        ,
    LOG2_K5A        ,
    LOG2_K5B        ,
    LOG2_K6A        ,
    LOG2_K6B        ,
    LOG2_K7A        ,
    LOG2_K7B        ,
    LOG2_K8A        ,
    LOG2_K8B        ,
    LOG2_K21        ,
    LOG2_K22        ,
    LOG2_K10        ,
    LOG2_K30        ,
            
    /* �̵����������������Զ����㣬���������� */
    TOTAL_RELAYS,
}tEnumRelaysList;

/* ���ڼ�¼�̵���ͨ���ϵ��ʱ��
 * �鿴 map �ļ���relayWorkTime ��ʵ��ռ�д洢�ռ䣬��TOTAL_RELAYS ��ֵ������ϵ�  
 * ���Բ���Ҫ�������� TOTAL_RELAYS
 * #define TOTAL_RELAYS          50
 */
EXTN tRelayTime relayWorkTime[TOTAL_RELAYS];

typedef enum
{
    TIM_MON_OPNCLS_ERR      = 0 ,   /* �������������������  */
    TIM_MON_SIGCMD              ,   /* �߼����źſ���������        */
    TIM_MON_IBPOPNSD_ERR        ,   /* ����������                  */
    TIM_SWCH_Q1Q2               ,   /* �л���: J3����15��16��һ��  */
    TIM_SWCH_Q1AQ2A_LOST        ,   /* �л���: J3����15��16����Ӱ�����ȱʧ  */
    TIM_LOG1LOG2_SELECT         ,   /* ���Ӱ�ѡ�񿪹��������  */
            
    /* ���������� */
    TIM_LOG1_OUT_ERR,
    TIM_LOG1_ERROR,
            
    TIM_LOG2_OUT_ERR,
    TIM_LOG2_ERROR,
            
    TIM_LINKERR_LOG1MON,
    TIM_LINKERR_LOG2MON,
    TIM_LINKERR_LOG1LOG2,
            
    /* ʱ��̵������� */
    TIM_KUT1,       //��ȫ��·ʱ��̵���KUT1��Ȧ
    TIM_KUT2,       //�źſ���ʱ��̵���KUT2��Ȧ
    TIM_KUT3,       //�źŹ���ʱ��̵���KUT3��Ȧ
            
    TIM_LOG_SIGCMDERR,
            
    TIM_LinkErr_CMD_LOG1,
    TIM_LinkErr_CMD_LOG2,
            
    TIM_AutoSelectLOG2,

    RTIM_SWITCHER_RESET_SELECT,
            
    /* ��ʱ���������������Զ����㣬���������� */
    TOTAL_CONTACTOR_TIMERS,
}tEnumTimersList;

typedef struct
{
    int16u posTim;
}tChecingTim;

/* #define TOTAL_CONTACTOR_TIMERS  */
/* �����жϴ���״̬�Ƿ���ȷ��ʱ��
 */
EXTN tChecingTim checkingTim[TOTAL_CONTACTOR_TIMERS];

//-------------------------------------- 
/* ��¼��ȫ��·���ϴ��� */
EXTN int8u sf_flash_AccRcv ;                    /* �ۼ��յ��İ�ȫ��·���ϴ����ۼ� */
EXTN int8u sf_flash_toMMS;                      /* ���͸�MMS�ģ���ȫ��·���ϴ��� */
EXTN int8u sf_flash_toTest;                     /* ���͸����Եģ���ȫ��·���ϴ��� */


/*--------------------------------------
 * ������Ϣ���� 
 */
typedef union
{
    struct
    {    
        int32u bBUS_FAULT           : 1;  //0 ���߹��ϣ����õ���ЧDCU������һ������
        int32u bOPN_FAULT           : 1;  //1 ���Ź��ϣ����� DCU ��Ϣ��־
        int32u bCLS_FAULT           : 1;  //2 ���Ź��ϣ����� DCU ��Ϣ��־
        int32u bUNLOCK              : 1;  //3 �ֶ����������� DCU ��״̬���μ� DOOR_STATE_UNLOCK
        int32u bMANUAL_MODE         : 1;  //4 �ֶ�������DCU �ֶ�ģʽ�Ҳ��Ǹ���
        int32u bMONITOR_FAULT       : 1;  //5 ���ϵͳ���ϣ��� MMS ͨ��ʧ��
        int32u bClosing             : 1;  //6 DCU���Ź��̣����� DCU ��״̬���μ� DOOR_STATE_CLOSING
        int32u bOpening             : 1;  //7 DCU���Ź��̣����� DCU ��״̬���μ� DOOR_STATE_OPENING
        int32u bPSD_FAULT           : 1;  //8 �����Ź��ϣ� ���Ź��� | ���Ź��� | ��ȫ��·���� 
        int32u bDOOR_ALL_OPEN       : 1;  //9 ��ȫ��������һ��DCU�Զ�ģʽ����DCU���߻���ϣ�
        int32u bSIG_FAULT           : 1;  //10 ����
        int32u bEmergenrydoor_opn   : 1;  //11 Ӧ���Ŵ򿪣�����һ��Ӧ�����ȱ���
        int32u bIsolation_MODE      : 1;  //12 ����ģʽ  ������һ�������Ŵ��ڸ���ģʽ
        int32u bMSD_OPEN            : 1;  //13 ���Ŵ� ��Manual Secondary Door (��ͬ�� CAD ��˾����)
        int32u bOPEN_ALARM          : 1;  //14 ��������PSDϵͳ���ڴ򿪻�رչ�����ASD/EED�ر��ź���ʧʱ����
        int32u bOPEN_Side           : 1;  //15 ������������״̬�������ţ�
        
        int32u bHeadASD_Opned       : 1;    //�׵��ſ���
        int32u bTailASD_Opned       : 1;    //ĩ���ſ���
    };
    
    int32u word;
}tDownMsg;

EXTN tDownMsg Down_Message0;

EXTN BIT16 Down_Message1;                          //��������������
#define AlarmAllow_BUS_FAULT    Down_Message1.B0   //���߹��Ϸ�������������
#define AlarmAllow_POWER_FAULT  Down_Message1.B1   //��Դ���Ϸ�������������
#define AlarmAllow_OPN_FAULT    Down_Message1.B2   //���Ź��Ϸ�������������
#define AlarmAllow_CLS_FAULT    Down_Message1.B3   //���Ź��Ϸ�������������

//--------------------------------------
typedef union
{
    struct
    {    
        int16u bAlarm_POWER_FAULT   : 1;  //0 ��Դ���ϱ���
        int16u bAlarm_Relays        : 1;  //1 �̵������ϱ���
        int16u bAlarm_BUS_FAULT     : 1;  //2 ���߹��ϱ���
        int16u bAlarm_OPN_FAULT     : 1;  //3 ���Ź��ϱ���
        int16u bAlarm_CLS_FAULT     : 1;  //4 ���Ź��ϱ���
        int16u bAlarm_Monitor_Fault : 1;  //5
        int16u bAlarm_LOGPCB_FAULT  : 1;  //6 ������߼����й���
        int16u bAlarm_AutoSelLog2   : 1;  //7 2024-6-3 .�Զ�λʱ���ư�2����Ҫ��������
        int16u          : 1;  //8
        int16u          : 1;  //9
        int16u          : 1;  //10
        int16u          : 1;  //11
        int16u          : 1;  //12
        int16u          : 1;  //13
        int16u          : 1;  //14
        int16u          : 1;  //15
    };
    
    int16u word;
}tAlarmFlags;

//alarm  ����������  �ο� Э���ܸ�ʽ �еĵ��
//����ǰ�ı��� �� ��ǰ�ı��� �Ƚ�,��� (��ǰ�ı��� > ����ǰ�ı���) �򱨾�; ��� (��ǰ�ı��� < ����ǰ�ı���) �� ����ʧ�ı�����"����ǰ�ı���"������
//PEDC�� ֻ�� ��������LED�����Ĺ��� �� ����������(��Դ����,���Ź���,���Ź���,���߹���), �̵�����������û��LED������ ����û�� ����������.
EXTN tAlarmFlags PSC_AlarmFlags;
EXTN tAlarmFlags PSL_AlarmFlags;
EXTN tAlarmFlags IBP_AlarmFlags;

//--------------------------------------
//LLY NJ7
#define Relay_Fault_Check(K_PN,bK_S,relayNum,bRelay_Coil,bRelay_Fault)      \
do{                                                                         \
    {                                                                       \
        if((K_PN))                                                          \
        {                                                                   \
            relayWorkTime[relayNum].revTim = 0;                             \
            if((bK_S) == VALID)                                             \
            {                                                               \
                bRelay_Coil = SETBIT;                                       \
                relayWorkTime[relayNum].posTim = 0;                         \
            }                                                               \
            else                                                            \
            {                                                               \
                if(relayWorkTime[relayNum].posTim > cRelay_positive_TIME)   \
                {                                                           \
                    bRelay_Fault = SETBIT;                                  \
                    bRelay_Coil = SETBIT;                                   \
                }                                                           \
            }                                                               \
        }                                                                   \
        else                                                                \
        {                                                                   \
            relayWorkTime[relayNum].posTim = 0;                             \
            if((bK_S) == VALID)                                             \
            {                                                               \
                if(relayWorkTime[relayNum].revTim > cRelay_reverse_TIME)    \
                {                                                           \
                    bRelay_Fault = SETBIT;                                  \
                }                                                           \
            }                                                               \
            else                                                            \
            {                                                               \
                relayWorkTime[relayNum].revTim = 0;                         \
            }                                                               \
        }                                                                   \
    }                                                                       \
}while(0)


#define Relay_Fault_Test(K_PN,bK_S,Relay_TIME_positive,Relay_TIME_reverse,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((K_PN))                                                  \
        {                                                           \
            Relay_TIME_reverse = 0;                                 \
            if((bK_S) == VALID)                                     \
            {                                                       \
                bRelay_Coil = SETBIT;                               \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                    bRelay_Coil = SETBIT;                           \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
            if((bK_S) == VALID)                                     \
            {                                                       \
                if(Relay_TIME_reverse > cRelay_reverse_TIME)        \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                }                                                   \
            }                                                       \
            else                                                    \
            {                                                       \
                Relay_TIME_reverse = 0;                             \
            }                                                       \
        }                                                           \
    }                                                               \
}while(0)

//��ⷽ��: ����ü̵���������·����(�ж�ѹ)��ͨ,��ü̵���Ӧ�ô���;����涨ʱ����û���� �򱨴�
#define Relay_Fault_Test_ByCircuitry_connect(k_test,bK_S,Relay_TIME_positive,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((k_test))                                                \
        {                                                           \
            if((bK_S) == VALID)                                     \
            {                                                       \
                bRelay_Coil = SETBIT;                               \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                    bRelay_Coil = SETBIT;                           \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
        }                                                           \
    }                                                               \
}while(0)

//��ⷽ��: ����ü̵���������·�յ㣨û�˵�ѹ����ͨ,��ü̵���Ӧ�ô���;����涨ʱ����û���� �򱨴�
#define Relay_Fault_Test_ByCircuitry_noPow_connect(k_test,bK_S,Relay_TIME_positive,bRelay_Coil,bRelay_Fault) \
do{                                                                 \
    {                                                               \
        if((k_test))                                                \
        {                                                           \
            if((bK_S) == VALID)                                     \
            {                                                       \
                Relay_TIME_positive = 0;                            \
            }                                                       \
            else                                                    \
            {                                                       \
                if(Relay_TIME_positive > cRelay_positive_TIME)      \
                {                                                   \
                    bRelay_Fault = SETBIT;                          \
                }                                                   \
            }                                                       \
        }                                                           \
        else                                                        \
        {                                                           \
            Relay_TIME_positive = 0;                                \
        }                                                           \
    }                                                               \
}while(0)

//��ⷽ��: ����ü̵���������· û�� ��ͨ;����涨ʱ���ڶ��ڴ��� �򱨴�
#define Relay_Fault_Test_ByCircuitry_Unconnect(k_test,bK_S,Relay_TIME_reverse,bRelay_Coil,bRelay_Fault) \
do{                                                             \
    {                                                           \
        if((k_test))                                            \
        {                                                       \
            if((bK_S) == VALID)                                 \
            {                                                   \
                if(Relay_TIME_reverse > cRelay_reverse_TIME)    \
                {                                               \
                    bRelay_Fault = SETBIT;                      \
                }                                               \
            }                                                   \
            else                                                \
            {                                                   \
                Relay_TIME_reverse = 0;                         \
            }                                                   \
        }                                                       \
        else                                                    \
        {                                                       \
            Relay_TIME_reverse = 0;                             \
        }                                                       \
    }                                                           \
}while(0)


//��ⷽ��: ����ü̵���û�����ӻ�����־����������ļ̵��������ô��㿪��־
#define Relay_trigger(k_test,bK_S,bRelay_Coil)                  \
do{                                                             \
    {                                                           \
        if((k_test))                                            \
        {                                                       \
            if((bK_S) == VALID)                                 \
            {                                                   \
                bRelay_Coil = SETBIT;                           \
            }                                                   \
        }                                                       \
    }                                                           \
}while(0)


//��ⷽ����������������ʱ֮��������Ӧ�ı�־
#define TrueSetFLAG(k_test,Delay_TIME,cTIME,bFlag)              \
    {                                                           \
            if((k_test))                                        \
            {                                                   \
                    if(Delay_TIME > cTIME)                      \
                    {                                           \
                            bFlag = SETBIT;                     \
                    }                                           \
            }                                                   \
            else                                                \
            {                                                   \
                    Delay_TIME = 0;                             \
                    bFlag      = 0;                             \
            }                                                   \
    }                                                           


#if 0
    BOOL fault = FALSE;
    static int16u TimeNG, TimeOK;
    if(SRelay_K0_PN      == VALID)
    {
        if(SRelay_K0_NO != VALID)
        {
            fault = TRUE;
        }
        else if((SRelay_K10_NO == INVALID) && (SRelay_K20_NO == INVALID) && (SRelay_K30_NC == VALID))
        {
            fault = TRUE;
        }
    }
    else
    {
        if(SRelay_K0_NO == VALID)
        {
            fault = TRUE;
        }        
        else if((SRelay_K10_NO == VALID) || (SRelay_K20_NO == VALID) || (SRelay_K30_NC == INVALID))
        {
            fault = TRUE;
        }
    }
    if(fault)
    {
        TimeOK = TgtGetTickCount();
        if((int16u)(TgtGetTickCount() - TimeNG > 1000))
        {
            bSIGRelay_K0_Fault = 1;
        }
        
    }
    else
    {
        TimeNG = TgtGetTickCount();
        if((int16u)(TgtGetTickCount() - TimeOK > 1000))
        {
            bSIGRelay_K0_Fault = 0;
        }
    }
#endif

//--------------------------------------
EXTN void PCA9505_data_deal(void);           //���ݴ���
EXTN void time(void);                        //��ʱ����
EXTN int8u cmpTime(int8u time1_hour, int8u time1_minute, int8u time2_hour, int8u time2_minute);

//--------------------------------------
#endif






