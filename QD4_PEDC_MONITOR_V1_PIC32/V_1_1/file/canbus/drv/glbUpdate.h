#ifndef _GLB_UPDATE_H_
#define _GLB_UPDATE_H_

#include "_cpu.h"

//===================================================================
#if (UPDATE_FUN_EN == TRUE)
typedef struct
{
    int8u u8Cmd;                              //����, 0 -- ʲôҲ����
    int8u u8Dst;                              //��������
}tMMSCmd;

#define MMS_CMD_UPDATE        0xE0          //MMS ��������������
#define FB_TO_MMS_UPDATE      0xE1          //������ MMS ����״̬

#define MMS_CMD_QUERY         0xE5          //MMS �����Ĳ�ѯ����
#define FB_TO_MMS_QUERY       0xE6          //��ѯ�ɹ�: ������ MMS ��Ϣ
#define FB_TO_MMS_QUERY_FAIL  0xE7          //��ѯʧ��: ������ MMS ��Ϣ


//�������״ָ̬ʾ
#define ENABLE_LED_MSG      1           //1 = ���� LED ָʾ,  0 = ������
#define ENABLE_UART_MSG     0           //1 = ���� UART ָʾ, 0 = ������

#define LED_PORT            PORTB       //PB.6
#define LED_BIT             (1<<6)      //

#define UPGRADE_UART        UART4       //ע�⣺1.��Ҫ�����޸� 2.���ܴ��ж�����
#define UPGRADE_UART_TX_REG U4TXREG
#define UPGRADE_UART_TX_STA U4STA
#define UPGRADE_UART_BR     56000

//master �����������ݸ� Slave �Ժ�, Ϊ�˱�֤ Slave ���㹻ʱ��д�� FLASH, ��ʱһ��ʱ����ѯ Slave �����
//ÿ��32��ָ���̻���������������Զ���ʱ������ʱ��ı��ֵΪ2 ms
//�������ݵ���󳤶��� 512 bytes(4��), ����ʱ��Ϊ: 4*(2+2) = 16ms
//���Ǳ�Ҫ��һЩʱ������, �趨 40ms
//Master ���Ͳ�ѯ�����Ժ�, �ȴ��ظ��ĳ�ʱ��ʱ���� 35ms(glbCan.h �� MASTER_DFT_OV + ApiCanMst.c �� MstOverTime)
//��� Slave �ڽ��յ��������ݺ�, �� (40+35)ms ʱ���ڱ������д�� FLASH ͬʱӦ�� Master
#define MST_DLY_QUERY         (SYS_TICK_1MS * 65)

//ָ��ʱ�����ղ�����λ��������, ֹͣ����
#define MMS_NODATA_TMR          (SYS_TICK_1MS * 1000 * 6)

#define UPFW_OVER_TIME        (5000)               //ms, ��һ�ζ�д��ɹ���ʼ��ʱ, ��ʱ��Ŀǰû����

//for u8ProFlag
#define fgSlvUpgrading    (1<<0)                   //1��������
#define fgSlvRxData       (1<<1)                   //1���յ�����
#define fgSlvProcess      (1<<2)                   //1����������Ҫ����
#define fgSlvUpgradeCmd   (1<<3)                   //1���յ���������
#define fgSlvWaitDoorCls  (1<<4)                   //1���ȴ��Źر�



//------------------------------------------------
//������:   FUNID_UPDATE_FIRMWARE
//��չ����
// bit7   ��ʾ���ݷ���, 0=PEDC->DCU, 1=DCU->PEDC
// bit6~0 ���������������ݰ�������, ���ڼ������Ƿ�ʧ���ݰ�, ȡֵ 0~119, Slave���յ�0����г�ʼ��,
//        ��Э������������� 120*7 = 840
//        120~127 ������Ϊ����, Ŀǰ��������ֻʹ���� UPFW_CMD_QUERY
#define UPFW_CMD_START    (RSV_CMD_S+0) //��������      : 0x55AA+�����ļ�CS(4)+CS(1), Slave ����������������
#define UPFW_CMD_DATA     (RSV_CMD_S+1) //�δ�������    : 0x55AA+�κ�(2)+�����ݳ���(2)+CS(1)
#define UPFW_CMD_WRITE    (RSV_CMD_S+2) //д�� Flash    : 0x55AA+�κ�(2)+CS(1)
#define UPFW_CMD_FINISH   (RSV_CMD_S+3) //�����������  : 0x55AA+CS(1)
#define UPFW_CMD_ABORT    (RSV_CMD_S+4) //����������ֹ  : 0x55AA+CS(1)
#define UPFW_CMD_QUERY    (RSV_CMD_S+5) //��ѯ����״̬  : 0x55AA+CS(1)
                                        //    SlaveӦ�� : 0x5A5A+״̬(1)+CS(1)
//------------------------------------------------
//for master
//tUpdateMsg.u8MstState
#define MST_STATE_IDLE              0       //����
#define MST_STATE_START             1       //��������
#define MST_STATE_TRANS             2       //������������
#define MST_STATE_QUERY_D           3       //�����������ʱ��ѯSlave
#define MST_STATE_QUERY             4       //���Ͳ�ѯ�����Slave�ȴ�Ӧ��
#define MST_STATE_TRANS_FAIL        5       //����ʧ��
//����
#define MST_STATE_INIT_FAIL         6       //δ��ʼ���ص�����
#define MST_STATE_REPLY_STATE       7       //�ظ�����״̬
#define MST_STATE_REPLY_REQUEST     8       //�ظ���������
#define MST_STATE_WAIT_REQUEST      9       /* �ȴ��ӻ��ظ� �������� */
#define MST_STATE_WAITDATA          10      /* �ȴ��������ݰ� */
#define MST_STATE_SENDING           0x10    //���ڷ������ݰ�

typedef struct
{
    tMMSCmd  MMSCmd;                         //MMS ������

    int8u  u8MstState;                        //Master ״̬
    int8u  u8SlvState;                        //Slave ����״̬
    int16u  wSlvSection;                       //Slave Ӧ��Ķκ�

    int16u  wTimerAuto;                        //�Զ���ʱ���е���һ������, ��Ҫ����Slave�Ĵ���ʱ��, 0 ��ʾ���Զ�����
    int16u  wTickAuto;                         //��ʱ����ʼֵ
    int16u  wTimerOver;                        //�趨��ʱ��, 0 ��ʾû�г�ʱ��
    int16u  wTickOver;                         //��ʱ����ʼֵ

    int32u dwCS;                              //�����ļ�Checksum
    int16u  wTotSections;                      //�����ļ��и�Ķ�����

    int8u *pData;                             //���ݰ�ָ��
}tUpdateMsg;

extern tUpdateMsg          UpdateMsg;

#define MMS_OP_COMAND      UpdateMsg.MMSCmd.u8Cmd  //��������
#define MMS_OP_DST_ID      UpdateMsg.MMSCmd.u8Dst  //��������

#define MMS_UPDATE_STATE() UpdateMsg.u8MstState    //== MST_STATE_IDLE ��ʾû������

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

#define CHK_TIMER(s,now,ov)   if((ov != 0) && ((int16u)(now - s) > ov))

#define IF_TIMER_AUTO()       CHK_TIMER(UpdateMsg.wTickAuto, wNow, UpdateMsg.wTimerAuto)
#define IF_TIMER_OVER()       CHK_TIMER(UpdateMsg.wTickOver, wNow, UpdateMsg.wTimerOver)

#define SET_TIMER_OVER(a)     UpdateMsg.wTimerOver = (a)
#define SET_TIMER_AUTO(a)     UpdateMsg.wTimerAuto = (a)

#define DCU_ON_LINE_REG       DcuData.MsgBuf.DcuOnLineState     //main.h

//**********************************************************************************************
//                                                                                             *
//                                      ��������                                               *
//                                                                                             *
//**********************************************************************************************
    #if (CDRV_TYPE == CDRV_PEDC)
    extern tMMSCmd    RxMMScmd;            //�����λ������������, ��������Ĳ������浽�˽ṹ����
    #endif

    //#define UPFW_CODE_BUF_MAX  (512+32)    //�����ݳ������ 512B����Ϣ��� 24B�������������ݵ�У��� 1B
 extern  int8u __attribute__ ((aligned(4))) FileDataBuf[UPFW_CODE_BUF_MAX]; //���ݳ������ 512B����Ϣ��� 24B�������������ݵ�У��� 1B
                                                            //�׵�ַ������뵽 int32u

    extern int8u u8UpdateState;

//**********************************************************************************************
//                                                                                             *
//                                      API ��������                                           *
//                                                                                             *
//**********************************************************************************************
//===== ��������
extern int8u UpdateProcess(void);
extern void GetAllChkSum(void);
extern int8u UpFwProcessData(int8u * pSrc);

#if (CDRV_TYPE == CDRV_PEDC)
//===== ��������λ��(MMS)����������
//  ����: pCmd = ����ָ��
//  ����: FALSE -- fail
extern int8u MMS_Command(int8u * pCmd, int pkgSize);
#endif //#if (CDRV_TYPE == CDRV_PEDC)

#if (CDRV_TYPE == CDRV_DCU)
extern void SetDoorState(int8u state);
extern void SetDcuState (int8u state);
extern int8u GetUpgradeState(void);
#endif






/* 0 = ��֧���������� IBP PSL SIG �壬����֧�� 
 */
#define UPDATE_IPS_EN       1       
#define UP_PEDC_MON         200
#define UP_PEDC_PSL         201
#define UP_PEDC_SIG         202
#define UP_PEDC_IBP         203
#define UP_PEDC_PIC24A      204
#define UP_PEDC_PIC24B      205
#define UP_PEDC_SWITCHER    206
#define UP_PEDC_LOG_A_UA1   207
#define UP_PEDC_LOG_A_UB1   208
#define UP_PEDC_LOG_B_UA1   209
#define UP_PEDC_LOG_B_UB1   210

/* �ӻ�ID: 201 ~ 220 */
#define UP_PEDC_SLAVES_S    201
#define UP_PEDC_SLAVES_E    220
            
#define UP_PEDC_DCU_S       ID_DCU_MIN
#define UP_PEDC_DCU_E       ID_DCU_MAX

#define UP_PEDC_MORE_DCUS   0x80    /* ��������DCU */

/* ������������ */
#define UPTYPE_UNKNOWN          0       /* δ�������� */
#define UPTYPE_PEDC_HOST        1       /* PEDC���ذ�: ����Щ�������� MONITOR �壬��Щ�������� IBP �� */
#define UPTYPE_PEDC_SLAVE       2       /* PEDC��չ�� */
#define UPTYPE_PEDC_PIC24       3       /* PEDC���ذ��PIC24оƬ */
#define UPTYPE_DCU              4       /* ָ��DCU */
#define UPTYPE_MORE_DCU         5       /* ����DCU */

#if (UPDATE_IPS_EN != 0)

/* ���ڷ���: �������ݰ� */
typedef BOOL (*uartUpdate_UartTxPkg)(int8u *pCmd, int pkgSize);

/* Uart �����Ƿ���У�FALSE ��ʾ���У�TRUE ��ʾæ */
typedef BOOL (*uartUpdate_UartTxBusy)(void);

/* ���ڷ���: ��ѯ����״̬ */
typedef BOOL (*uartUpdate_QuerySlave)(int8u id, int16u secNo);

/* ��ѯָ�����������״̬ */
typedef BOOL (*uartUpdate_ChkOnline)(int8u id);

/* ��ѯָ������ķ��ص�����״̬ */
typedef BOOL (*uartUpdate_GetSlaveReply)(int8u id, int8u *sts, int16u *secNo);


PUBLIC BOOL InitUartUpdateCallBack(uartUpdate_UartTxPkg cbTxPkg, uartUpdate_UartTxBusy cbTxBz, uartUpdate_QuerySlave cbQuerySts, uartUpdate_ChkOnline chkOnline, uartUpdate_GetSlaveReply cbGetReply);

#endif






#endif //#if (UPDATE_FUN_EN == TRUE)




/* ==============================================
 * ������������
 *  */
BOOL ifBatchUpdateDCUs(void);
BOOL setBatchcastUpdateDCUs(int8u * settings);
int8u* getBatchUpdateDCUsSetting(void);

#endif //_GLB_UPDATE_H_

