/*=============================================================================
* Project:		WH6-LINE PEDC
* PEDC MCU:		PIC32MX795F512H + PIC18F23K22
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.0
* Author:		lly
* Date:			2011-6-16
* =============================================
* ��	 ��	:	ApiCanCom.C
*			:	Can bus Ӧ�ã�MASTER��SLAVE�Ĺ��к���
* =============================================================================*/
#include  "_g_canbus.h"
#include  "define.h"
#include  "saveIbpX.h"
#include  "can.h"

#include  "_glbApi.h"
#include  "glbCan.h"

#include  "CanDrv.h"
#include  "CanTgt.h"
#include  "ObjDict.h"

#if (UPDATE_FUN_EN == TRUE)
#include  "glbUpdate.h"
#endif

//#include "UpEEPROM.h"

extern void CapiProcessTransTimer(int8u u8CanNo);                 //�������ݴ��ͽ��̵�ʱ��
extern void CapiProcessRxMsg(int8u u8CanNo, tCdrvMsg * pMsg);     //�����������

extern void CapiCanManage(void);

//---------------------------------------------------------------------------
	int16u    wCanBusTime[2]={0,0};   //unit : 1ms, cleared by can bus interrupt (Rx or Tx)
																	//reset can-bus and can-open
																	//if can interrupt is not happened within (CAN_BUS_ERR_TIME*1ms)

	tThreadPara aTransThread1[MAX_TRANS_THREAD];    //CAN1 �������
	tThreadPara aTransThread2[MAX_TRANS_THREAD];    //CAN2 �������

	int8u  CanOnLine[2];

	int8u  CalcChkSumLEN = 0;                       //== 0 16λ���� != 0 8λ��
    
    int8u  sendDCUSysTime[BYTES_SYSTIME_TODCU + 2]; //������Ҫ��1�ֽ�����У���
//---------------------------------------------------------------------------
// Function:    CapiProcess()
// Description: goes here every loop
// Parameters:  void
// Return:
// State:
//---------------------------------------------------------------------------
int8u CapiProcess(void)
{
	tCdrvMsg  RxMsgBuf;
	static  int16u  wCapiTimer = 0;

	//������ջ��� CAN1 / CAN2
	if(CdrvReadMsg(CAN1, (tCdrvMsg *)&RxMsgBuf) == TRUE)
	{
		CapiProcessRxMsg(CAN1,(tCdrvMsg *)&RxMsgBuf);
	}

	#ifndef NO_CAN_BUS2
	if(CdrvReadMsg(CAN2, (tCdrvMsg *)&RxMsgBuf) == TRUE)
	{
		CapiProcessRxMsg(CAN2,(tCdrvMsg *)&RxMsgBuf);
	}
	#endif

	//�����ͻ��� CAN1 / CAN2
	CdrvCheckTransmit (CAN1);
	#ifndef NO_CAN_BUS2
	CdrvCheckTransmit (CAN2);
	#endif
/*
	//�������״̬ CAN1 / CAN2
	CdrvProcessError(CAN1);
	#ifndef NO_CAN_BUS2
		CdrvProcessError(CAN2);
	#endif
*/
	//���ݴ��ͼ�ʱ CAN1 / CAN2
	CapiProcessTransTimer(CAN1);
	#ifndef NO_CAN_BUS2
	CapiProcessTransTimer(CAN2);
	#endif

	//��ʱ����
	if((int16u)(TgtGetTickCount () - wCapiTimer) > 10)     //10ms
	{
		GET_TICK(wCapiTimer);

		//here every 10ms

		#if (BUS_TIMEROVER == TRUE)
			wCanBusTime[0]++;                             //CanBus1 offline timer
			wCanBusTime[1]++;                             //CanBus2 offline timer

			if(wCanBusTime[0] > CAN_BUS_ERR_TIME)
			{
				wCanBusTime[0] = 0;
				CdrvReset(CAN1,1);
				CapiFreeAllThread(CAN1);
			}

			#ifndef NO_CAN_BUS2
			if(wCanBusTime[1] > CAN_BUS_ERR_TIME)
			{
				wCanBusTime[1] = 0;
				CdrvReset(CAN2,1);
				CapiFreeAllThread(CAN2);
			}
			#endif
		#endif
	}

	CapiCanManage();

	return 1;
}

//---------------------------------------------------------------------------
// Function:    ChkTransState()
// Description: ȡ��ָ�����̵�ǰ״̬
// Parameters:  ���̹��ܺ� u8FunId, �˹��ܺ��봴��(CapiStartTrans)�Ĳ�����һ�µ�
// Return:
//        fgRunning -- ���ڴ���, ������� 0���� fgAbort �� fgTransOK ��Ч
//        fgAbort   -- �쳣����
//        fgTransOK -- �ɹ�
//Note:   ��������CAN���н���ŷ��ؽ��, ��ֻҪ����һ�� OK �ͻ᷵��OK, ����ͬʱʧ�ܾͷ���ʧ��
//---------------------------------------------------------------------------
int8u ChkTransState(int8u u8FunId)
{
	int8u State1,State2, R1,R2;

	State1 = aTransThread1[u8FunId].u8State;
	State2 = aTransThread2[u8FunId].u8State;

	if(State1 & fgRunning)
	{
		if(State1 & fgTransOK)
			R1 = fgTransOK;
		else if(State1 & fgAbort)
			R1 = fgAbort;
		else
			R1 = fgRunning;
	}
	else
		R1 = 0;

	#ifndef NO_CAN_BUS2
	if(State2 & fgRunning)
	{
		if(State2 & fgTransOK)
			R2 = fgTransOK;
		else if(State2 & fgAbort)
			R2 = fgAbort;
		else
			R2 = fgRunning;
	}
	else
	#endif
		R2 = 0;

	if(R1 && R2)  //����ͬʱ����
	{
		//����һ����������, ���� fgRunning
		if((R1 == fgRunning) || (R2 == fgRunning))
			return fgRunning;

		//������ʧ��, �򷵻� fgAbort
		if((R1 == fgAbort) && (R2 == fgAbort))
			return fgAbort;

		//����һ���Ѿ��ɹ�ͨѶ, ���� fgTransOK, ͬʱ�����һ�� ???
		if((R1 == fgTransOK) || (R2 == fgTransOK))
			return fgTransOK;

		/*
		if(R1 == fgTransOK)
		{
			aTransThread2[u8FunId].u8State = 0;         //need or not ?????
			return fgTransOK;
		}
		if(R2 == fgTransOK)
		{
			aTransThread1[u8FunId].u8State = 0;         //need or not ?????
			return fgTransOK;
		}
		*/

		//������û�н��
		return 0;
	}

	else   //ֻ��һ������
	{
		if(R1)
			return R1;
		#ifndef NO_CAN_BUS2
		if(R2)
			return R2;
		#endif
		return 0;
	}
}

//---------------------------------------------------------------------------
// Function:    ClrTransState()
// Description: ���ָ�����̵�ǰ״̬
// Parameters:  ���̹��ܺ� u8FunId, �˹��ܺ��봴��(CapiStartTrans)�Ĳ�����һ�µ�
// Return:      State (��λ������ı�־λ����������Ч)
//---------------------------------------------------------------------------
void ClrTransState(int8u u8FunId)
{
	aTransThread1[u8FunId].u8State = 0;
	aTransThread2[u8FunId].u8State = 0;
    
    aTransThread1[u8FunId].wBufLen = 0;
    aTransThread2[u8FunId].wBufLen = 0;
}

//---------------------------------------------------------------------------
// Function:    CapiFreeAllThread()
// Description: ɾ��ĳ��CAN�����н���
// Parameters:
// Return:
//---------------------------------------------------------------------------
void CapiFreeAllThread (int8u u8CanNo)
{
	int i;
	tThreadPara* pTh;

	if(u8CanNo == CAN1)
		pTh = (tThreadPara*)aTransThread1;
	else
		pTh = (tThreadPara*)aTransThread2;

	for(i=0; i<MAX_TRANS_THREAD; i++,pTh++)
	{
		pTh->u8State = 0;
	}
}

//---------------------------------------------------------------------------
// Function:    CalcChkSum()
// Description: ����ָ��Buffer��CheckSum
// Parameters:  Buffer �׵�ַ��Buffer�ĳ���
// Return:      ����һ�ֽ� CheckSum
// �㷨:        �ۼӺ�
//	int8u  CalcChkSumLEN;          //== 0 16λ���� != 0 8λ��
//---------------------------------------------------------------------------
/*
			�޸�ԭ����汾��CalcChkSum,����Ϊ int8u ;
			�°汾�ĳ���Ϊ�� int16u;Ϊ�����ϰ汾�ģ�����
			ƥ��DCU�ļ���ͣ����˸ô���
*/
int8u CalcChkSum(int8u* Buf, int16u u8Size)
{
	int8u Chksum = 0;

	int16 Len;
	Len = u8Size;
	if(UpdateMsg.u8MstState != MST_STATE_IDLE)
	{	
		if(CalcChkSumLEN != 0)
		{
			Len = (int16u)((int8u)Len);	
		}	
	}	

	while(Len)
	{
		Chksum += *Buf++;
		Len--;
	}

	return Chksum;
}

//---------------------------------------------------------------------------
// Function:    CapiCanDebugPrint()
// Description: ͨ��Can���͵�����Ϣ, �ڼ�����ϲ鿴
// Parameters:  ����ָ�뼰����, ���� < 8
// Return:
//---------------------------------------------------------------------------
void CapiCanDebugPrint(int8u * pData, int8u Len, int8u id)
{
	int8u *p;
	tCdrvMsg  MsgBuf;

	MsgBuf.m_CanId = SET_COBID(GET_FUNID(0), id);
	MsgBuf.m_bSize = Len;

	p = &MsgBuf.m_bData[0];
	while(Len)
	{
		*p++ = *pData++;
		Len--;
	}

	CdrvWriteMsg(CAN1, &MsgBuf);
	#ifndef NO_CAN_BUS2
	CdrvWriteMsg(CAN2, &MsgBuf);
	#endif
}

//---------------------------------------------------------------------------
// Function:    SetRamBit / ClrRamBit / ChkRamBit / TogRamBit
// Description: λ����
// Parameters:  ע�� ���� ���������ı߽� !!!
// Return:
// State:
//---------------------------------------------------------------------------
//λ����
void SetRamBit(int8u *p,int8u bit)
{
	p  += bit>>3;
	*p |= 1<<(bit & 7);
}

//λ���
void ClrRamBit(int8u *p,int8u bit)
{
	p  += bit>>3;
	*p &= ~(1<<(bit & 7));
}

//λ����
int8u ChkRamBit(int8u *p,int8u bit)
{
	p += bit>>3;
	if(*p & (1<<(bit & 7)))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//λ�л�
void TogRamBit(int8u *p,int8u bit)
{
	p += bit>>3;
	*p ^= (1<<(bit & 7));
}
















