#ifndef _GLB_CAN_API_
#define _GLB_CAN_API_

#include "plib.h"

//**********************************************************************************************
//                                                                                             *
//                                           CAN ���ܻ�������                                  *
//                                                                                             *
//**********************************************************************************************
// master or slave define
#ifndef CDRV_TYPE
#error 'CDRV_TYPE not defined!'
#endif

// ========= CAN �����ʵ�ַ�ʽ, ������� Master ��Ч
// TURE  -- ����CAN������һ��ʧ���Ժ��Զ��л�����һ��
// FALSE -- ����CANͬ��ͨѶ, ��������ȫ��ͬ�Ĵ���
#define CAN_MANAGE_TOGGLE   TRUE
#define CAN_ERR_CNT_MAX     15          //���� N �δ�����Ϊ CAN ������
#if (CAN_ERR_CNT_MAX > 15)
#error "CAN_ERR_CNT_MAX must be less than 16"
#endif

// ========= ��������
#if (CDRV_TYPE == CDRV_PEDC)
	//PEDC
	#define MAX_RX_BUF          20        //���ջ���,  ���ڶ�дָ���������Ϊû������, ���ʵ���Ͽ����õĻ�������һ��
	#define MAX_TX_BUF          12        //���ͻ���
#else
	//DCU or MCP
	#define MAX_RX_BUF          12        //���ջ���
	#define MAX_TX_BUF          12        //���ͻ���
#endif

// ========= ���ݴ�����̵�����, ������� CanApi.h �е��������
#define MAX_TRANS_THREAD       16

// ========= ���ݴ��ͳ�ʱ: ����Ϊ�����������ݰ���ʱ����
#define TRANS_TIMEROVER        TRUE      //TRUE - enable, FALSE - disable
#define MASTER_DFT_OV          30        //ms, ������Сֵ, ���ݹ���������ȼ�����, ���ȼ�Խ��, ��ʱ����Խ��
#define SLAVE_DFT_OV           1100      //ms, ���� Slave ʵ�����岻��

#define MASTER_BROAD_DLY       5         //ms, �� master ���͹㲥����ʱ, ����Ҫ Slave Ӧ��, master ��ʱ����

// ========= ���ݴ��ͳ�ʱ: ����ȫ���и�������ο����ظ�����ĸ����� �磺24�ֽڴ��䣬��4������Σ��������һ������δ���ʧ�ܣ����������·��͸ô���Σ����ط��ĸ�������δ���֮�������ƣ���ֹ��ѭ����
#define Min_RepeatPacket       3
#define MAX_RepeatPacket       40

// ========= CAN Ӳ�������ʹ��
#define FILL_HWTXBUF_ALL    FALSE        //��������ͻ���ȡ����, һ�ν�����Ӳ�����ͻ�������(���㹻�ķ�������)
#define CAN_TX_HW_BUF_1ST   FLASE        //TRUE -- CdrvWriteMsg �������Ȳ��ҿյ�Ӳ�����ͻ�����з���
													  //FALSE - CdrvWriteMsg �����������������ݱ��浽������ͻ���

// ========= Can ��λ����
#define BUS_TIMEROVER       TRUE         //
#define CAN_BUS_ERR_TIME    500          //unit = 10ms�� ���ָ��ʱ����û�н��յ���������Ϊ Can ���ߴ����ʱ��,

// ========= PEDC ͨѶ��ʱ��ӡ��ͨ��CAN���ͣ�������鿴, ��������)
#if (CDRV_TYPE == CDRV_PEDC)
	#define PRINT_TIME      FALSE
	#define PRINT_TIME_ID   0x55          //��ӡʱ��ռ�õ� ID �ţ�������ڵ� ID ��ͬ��С�� 99 ���ڼ��
#else
	#define PRINT_TIME      FALSE
#endif

// ========= PEDC ���Դ�����Ϣ
#define    DEBUG_PRINT_ID  0x56

// ========= PEDC ����: ÿ���� 5 ����Ϣ�󣬱���ȴ�����Ӳ������ղż�������
// ����ֳ����� ��������ʱ���������DCU˲ʱ���ߡ� ������, ���������ռ������ʱ��̫��
#if (CDRV_TYPE == CDRV_PEDC)
  #define TX_WAIT_EMPTY   TRUE
  #define TX_WAIT_CNT     3
#else
  #define TX_WAIT_EMPTY   FALSE
#endif


// ==========================================================================
//masked by lly 2011-6-9, use CAN1 / CAN2, please see : CAN_MODULE of PIC32 lib
//#define CANBUS_0           0
//#define CANBUS_1           1

//#define NO_CAN_BUS2        //������ CAN2 ���� (SZ4 û��CAN2���� CAN2 ��IO�����������ƣ������ֹ CAN2)

// Can Spec define
#define CAN20A             1
#define CAN20B             2
#define CDRV_CAN_SPEC      CAN20A          //only support CAN20A now!!!

//**********************************************************************************************
//                                                                                             *
//                                          ����                                               *
//                                                                                             *
//**********************************************************************************************
//dwID    : bit10-7 -- ������, bit6-0 -- Node id, ���ֽ��Ƿ������ݰ���ID
//        : ���ݰ���һ���ֽ���Ϣ����չ����, ����/���/��ֹ/Ӧ��
//        : ���, ÿ�δ�����, ���ݰ��е���Ч�������ֻ�� 7 �ֽ�, ����Ϣ�ǽ������ݳ��� (DLC - 1)
//˵��    : ����ֻ�� Client ���͸� Server, Server �����������������ֻ��Ӧ������
//          ���ﶨ�����ֵ��Ҫ����Ķ�, ������Ҫ������� T_CmdParaTable/CmdTransType
//        : ����/����/��ѯ����,

#define GET_FUNID(a)        (((a)>>7)&0x0F)
#define GET_NODEID(a)       ((a)&0x7F)
#define SET_FUNID(a)        (((a)&0x0F)<<7)
#define SET_COBID(a,b)      (SET_FUNID(a) | b)

#define TRANS_DIR           0x80
#define RSV_CMD_S           120
#define BYTES_PER_PACK      7           //���ݰ��е������Ч���ݳ���(�ֽ�), ���� < 8

//��ѯ����: FUNID_QUERY
#define FUN_QUERY_VER       1           //��ѯSlave����汾, ���� YY/MM/DD/? MAINVERSION/SUBVERSION 6�ֽ�
#define FUN_QUERY_CS        2           //��ѯSlave���CHECKSUM,

//prototypes
typedef struct
{
	int32u dwId;           //�ڵ�ID
	int8u  u8State;        //״̬
	int8u  u8PacketCnt;    //���ݰ�������

	int8u  u8CanNo;
	int8u  u8TimeCnt;      //�����ã���¼ͨѶ��ʱ�䣬������ PEDC ��Ч����ʱ�� PEDC ���� CapiStartTrans ��ʼ������

	int16u  wTickTot;       //��ʱ(ϵͳ��ʱ��׼ 1ms)���������������ļ�ʱ
	int16u  wTick;          //��ʱ(ϵͳ��ʱ��׼ 1ms)��ÿ�����ݰ�֮���ʱ����
	int16u  wTransOV;       //��ʱ�趨(ϵͳ��ʱ��׼ 1ms), 0 -- �����

	int16u  wRxBytes;       //�Ѿ����յ����ݳ���
	int16u  wTxBytes;       //�Ѿ����͵����ݳ���

	int16u  wBufLen;        //���ջ������ݵĳ���
	int8u* pBufPtr;         //���ջ������ݵ��׵�ַ
	
	int8u  wPrevPacketRxOrTxLen;//��һ�ν��ջ��͵İ�����
	int8u  wRepeatPacket;       //�����ط�������
	
} tThreadPara;

//-----------------------------------------------
#define DCUID_CONFLICTED    0xEF        // V1.8 : DCU ID��ͻ���
typedef struct
{
	int8u  SlvCanNo  [BYTES_SLV_FLAG];      //1 -- CanBus1, 0 -- CanBus0
	int8u  SlvCan0Err[BYTES_SLV_FLAG];      //1 -- CanBus0 ����
	int8u  SlvCan1Err[BYTES_SLV_FLAG];      //1 -- CanBus1 ����
	int8u  IDConflict[BYTES_SLV_FLAG];      //1 -- CAN ID ��ͻ for dcu-can V1.8

	int8u  ErrCnt[BYTES_SLV_NUM];       //CanBus �Ĵ������, bit[7:4]����CAN1, bit[3:0]����CAN0
}tCanManage;


typedef struct
{
		int32u   m_CanId;
		int8u    m_MsgType;
		int8u    m_bSize;
		int8u    m_bData[8];
} tCdrvMsg;

// ==========================================================================
extern  int16u         wCanBusTime[];

extern  tThreadPara  aTransThread1[MAX_TRANS_THREAD];    //CAN1 �������
extern  tThreadPara  aTransThread2[MAX_TRANS_THREAD];    //CAN2 �������

extern 	int8u  CalcChkSumLEN;          //== 0 16λ���� != 0 8λ��
// ==========================================================================
extern void  CapiFreeAllThread (int8u u8CanNo);
extern int8u  CalcChkSum(int8u* Buf, int16u u8Size);
extern void  CapiCanDebugPrint(int8u * pData, int8u Len, int8u id);

	extern void CapiInitCanManage(void);
	extern int8u CanOnLine[2];
	extern tCanManage CanManage;
        extern int8u  sendDCUSysTime[];

	#define GET_CAN1_ERR_PTR()       (int8u*)&CanManage.SlvCan0Err
	#define GET_CAN2_ERR_PTR()       (int8u*)&CanManage.SlvCan1Err

	#define GET_CURRENT_CHNL(id)   ChkRamBit(&CanManage.SlvCanNo[0],  id)
	#define TOG_CAN_CHNL(id)       TogRamBit(&CanManage.SlvCanNo[0],  id)
	#define SET_CUR_CAN1(id)       ClrRamBit(&CanManage.SlvCanNo[0],  id)
	#define SET_CUR_CAN2(id)       SetRamBit(&CanManage.SlvCanNo[0],  id)

	#define CHK_CAN1_ERR(id)       ChkRamBit(&CanManage.SlvCan0Err[0], id)
	#define SET_CAN1_ERR(id)       SetRamBit(&CanManage.SlvCan0Err[0], id)
	#define CLR_CAN1_ERR(id)       ClrRamBit(&CanManage.SlvCan0Err[0], id)

	#define CHK_CAN2_ERR(id)       ChkRamBit(&CanManage.SlvCan1Err[0], id)
	#define SET_CAN2_ERR(id)       SetRamBit(&CanManage.SlvCan1Err[0], id)
	#define CLR_CAN2_ERR(id)       ClrRamBit(&CanManage.SlvCan1Err[0], id)

        /* dcu-can V1.8 */
	#define CHK_IDCOFLICT(id)      ChkRamBit(&CanManage.IDConflict[0], id)
	#define SET_IDCOFLICT(id)      SetRamBit(&CanManage.IDConflict[0], id)
	#define CLR_IDCOFLICT(id)      ClrRamBit(&CanManage.IDConflict[0], id)

// ���ݶδ���״̬ tThreadPara . u8State
#define fgRunning         (1<<7)                                              //1 -- ���̱�ʹ��
#define fgUpload          (1<<6)                                              //1 -- �ϴ�(����������)
#define fgDnload          (0<<6)                                              //0 -- ����(����������)
#define fgAbort           (1<<5)                                              //1 -- ���ݴ����쳣��ֹ
#define fgTransOK         (1<<4)                                              //1 -- �������
#define fgCmdFlag         (1<<1)
#define fgResult          (1<<0)                                              //�н������

//**********************************************************************************************
//                                                                                             *
//                                      API ��������                                           *
//                                                                                             *
//**********************************************************************************************
//===== Can ��ʼ��, �������б������
//  ����: ��
//  ����: ��
extern void  TgtInitCan (void);

//===== Can ��ع��ܣ�u8CanNo ��CANͨ����, CAN1 �� CAN2
extern void  CdrvReset (CAN_MODULE u8CanNo,int8u u8RstCanDrv);
extern void  CdrvInterruptHandler (int8u u8CanNo);
extern int8u CdrvCheckTransmit(CAN_MODULE u8CanNo);
extern int8u CdrvReadMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p);
extern int8u CdrvWriteMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p);

//===== �ṩ�� Can ��ʱ�Ӻ���, �� 1ms �ж��е���
//  ����: ��
//  ����: ��
extern void  SysTick1Ms(void);

//===== Can ͨѶ����, ��ѭ������
//  ����: ��
//  ����: ��
extern int8u  CapiProcess(void);

//===== �������ݴ������
//  ����: u8Fun �ǹ��ܺ�, u8NodeId �ǽڵ�ID, len �����ݳ���, pBuf ��ָ���������ָ��
//  ����: TRUE = �ɹ�, FALSE = ʧ��
extern int8u  CapiStartTrans(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);
extern int8u  CapiTransExtCmd(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);

//===== ȡ��ָ��������̵�ǰ״̬,
//  ����: u8FunId �ǹ��ܺ�
//  ����: fgRunning(������) / fgAbort(����) / fgTransOK(�ɹ�), �������������״̬�򷵻� 0
extern int8u  ChkTransState(int8u u8FunId);

//===== ���ָ��������̵�ǰ״̬
//  ����: u8FunId �ǹ��ܺ�
//  ����: ��
extern void  ClrTransState(int8u u8FunId);

//===== λ����: λ����, λ���, λȡ��, λ�ж�
//  ����: p �Ǵ洢����ָ��(������һ������������), bit �Ǵ洢����λ(����� 255),
//  ����: ����λ�жϷ��� 0��1�� ����û�з���ֵ
//  ע��: �������������ı߽� !!!
extern void  SetRamBit(int8u *p,int8u bit);
extern void  ClrRamBit(int8u *p,int8u bit);
extern void  TogRamBit(int8u *p,int8u bit);
extern int8u  ChkRamBit(int8u *p,int8u bit);

//===== ȡ������Ϣ�� CAN ͨ��
int8u GetTxChnl(int8u fun_id,int8u node_id);

//===== ���Slave�Ƿ�����
int8u ChkSlaveOnLine(int8u Slave);


//static volatile	BYTE CAN1MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN1 FIFO	message	area.
//static volatile	BYTE CAN2MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN2 FIFO	message	area.

#endif //_GLB_CAN_API_



