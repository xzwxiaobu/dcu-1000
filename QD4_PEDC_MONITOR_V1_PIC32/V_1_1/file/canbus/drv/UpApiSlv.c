/*=============================================================================
* Project:		WH2-LINE PEDC
* PEDC MCU:		PIC32MX795F512H + PIC18F23K22
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.0
* Author:		lly
* Date:			2011-4-1
* =============================================
* ��	 ��	:	UpApiSlv.C
*			:	ͨ��CAN����������SLAVE ��
* =============================================================================*/
#include  "..\_g_canbus.h"

#if ((UPDATE_FUN_EN == TRUE))
#include "glbCan.h"
#include "glbUpdate.h"
#include "_glbApi.h"

#include "Update.h"
#include "CanTgt.h"
#include "CanDrv.h"

#include "..\ObjDict.h"

//**********************************************************************************************
//                                                                                             *
//                                     Slave ��������                                          *
//                                                                                             *
//**********************************************************************************************
#if ((CDRV_TYPE != CDRV_PEDC))
	#include "..\canbus.h"

	int8u __attribute__ ((aligned(4))) FileDataBuf[UPFW_CODE_BUF_MAX];  //�׵�ַ������뵽 int32u


	int8u u8UpdateState = 0;	    //UPFW_STATE_SEC_ERR, UPFW_STATE_SEC_OK, UPFW_STATE_SEC_FAIL...
	int8u u8ProFlag     = 0;     //fgSlvUpgrading, fgSlvRxData,...

	extern tUpFwState UpFwState;
	extern int8u UpdateCmdFromMaster(tThreadPara * pTh, tCdrvMsg * pMsg);

	static int8u IfUpgradeEnable(void);

	extern int16u GetDcuState(void);
	extern int16u GetDoorState(void);

//---------------------------------------------------------------------------
// Function:    UpdateCmdOfMaster
// Description: Slave ��������
// Parameters:  ��Ϣ pMsg
// Return:      unused
// State:
//---------------------------------------------------------------------------
int8u UpdateProcess(void)
{
	int8u Ret = FALSE;
	int16u wNow;
	static int16u wTickWaiting;

	switch(ChkTransState(FUNID_UPDATE_FIRMWARE))
	{
		case fgTransOK:
			if((u8ProFlag & fgSlvRxData) == 0)
			{
				u8ProFlag |= fgSlvRxData | fgSlvProcess;
				GET_TICK(wTickWaiting);
			}
			break;

		case fgRunning:
		case fgAbort:
			u8ProFlag &= ~fgSlvRxData;
			break;

		default:
			break;
	}

	if(u8ProFlag & fgSlvProcess)
	{
		if(u8ProFlag & fgSlvUpgrading)
		{
			u8ProFlag    &= ~fgSlvProcess;
			u8UpdateState = UpFwProcessData((int8u*)FileDataBuf);
		}
		else if((u8ProFlag & fgSlvUpgradeCmd) == 0)
		{//�������������У���������Ƿ�������ʼ
			if(ChkDataPack((int8u*)FileDataBuf) < 8)
			{
				return FALSE;
			}
			else
			{
				if(*((int16u*)FileDataBuf + 1) == PACK_CMD_START)
				{
					u8ProFlag |= fgSlvUpgradeCmd | fgSlvWaitDoorCls;
					u8UpdateState = UPFW_STATE_WAITSTATE;
				}
			}
		}
	}

	if((u8ProFlag & (fgSlvWaitDoorCls | fgSlvUpgradeCmd)) == (fgSlvWaitDoorCls | fgSlvUpgradeCmd))
	{
		if(IfUpgradeEnable() == TRUE)
		{
			u8ProFlag |= fgSlvUpgrading;
			u8ProFlag &= ~fgSlvWaitDoorCls;
		}
	}

	GET_TICK(wNow);
	if((int16u)(wNow - wTickWaiting) > 5000) //ms
	{
		u8ProFlag = 0;
		u8UpdateState = UPFW_STATE_NOUPDATE;
	}

	return Ret;
}

//---------------------------------------------------------------------------
// Function:    UpdateCmdOfMaster
// Description: Slave ���� Master �ķ�����Ϣ
// Parameters:  ��Ϣ pMsg
// Return:      TRUE -- ��Ϣ��Ч
// State:
//---------------------------------------------------------------------------
int8u UpdateCmdFromMaster(tThreadPara * pTh, tCdrvMsg * pMsg)
{
	int8u u8ExtCmd;
	int8u u8CS;
	tCdrvMsg MsgBuf;

	if((pMsg->m_bData[1] != 0x55) && (pMsg->m_bData[2] != 0xAA))
	{
		return FALSE;
	}

	u8CS = CalcChkSum(&pMsg->m_bData[0], pMsg->m_bSize-1);
	if(pMsg->m_bData[pMsg->m_bSize-1] != u8CS)                 //У�� checksum
	{
		return FALSE;
	}

	u8ExtCmd = pMsg->m_bData[0] & (~TRANS_DIR);

	switch(u8ExtCmd)
	{
		case UPFW_CMD_QUERY:
			MsgBuf.m_CanId    = (pTh->dwId & (int32u)(~0x7F)) | (int32u)g_NodeId;
			MsgBuf.m_bData[0] = UPFW_CMD_QUERY | TRANS_DIR;
			MsgBuf.m_bData[1] = 0x5A;
			MsgBuf.m_bData[2] = 0x5A;
			MsgBuf.m_bData[3] = u8UpdateState;

			if(u8UpdateState == UPFW_STATE_UPDATEOK)
			{
				if(IfUpgradeEnable() != TRUE)
				{
					MsgBuf.m_bData[3] = UPFW_STATE_WAITDOOR;
				}
			}

			#ifndef QUERY_UPFW_SECTION
				MsgBuf.m_bData[4] = CalcChkSum(&MsgBuf.m_bData[0], 4);       //��󱣴�CheckSum
				MsgBuf.m_bSize    = 5;
			#else
				if(u8UpdateState != UPFW_STATE_UPDATEOK)
				{
					MsgBuf.m_bData[4] = UpFwState.wCurrentSections-1;
				}
				else
				{
					MsgBuf.m_bData[4] = UpFwState.wCurrentSections;
				}

				MsgBuf.m_bData[5] = CalcChkSum(&MsgBuf.m_bData[0], 5);       //��󱣴�CheckSum
				MsgBuf.m_bSize    = 6;
			#endif

			CdrvWriteMsg(pTh->u8CanNo, (tCdrvMsg*)&MsgBuf);

			if(u8UpdateState == UPFW_STATE_UPDATEOK)
			{
				//�յ�������������, �ȴ�CAN���ͻ�������
				//�˴����ܵ��ú��� CapiProcess, ��Ϊ UpdateCmdFromMaster �ǴӸú��������
				do
				{
					u8CS  = CdrvCheckTransmit (CAN1);
					#ifndef NO_CAN_BUS2
					u8CS |= CdrvCheckTransmit (CAN2);
					#endif
				}while(u8CS != 0);

				if(IfUpgradeEnable() == TRUE)
				{//�������ݽ�����ɣ�ͬʱ�����Ŵ��ڹر�
					bios_Finished(&UpFwState,&UpdateTxMsg);
				}
				else
				{
					UpFw_SetCopyEnable();
				}

				u8ProFlag = 0;
				u8UpdateState = UPFW_STATE_NOUPDATE;
			}
			else
			{
				//if(u8UpdateState != UPFW_STATE_SEC_OK)
				u8UpdateState = UPFW_STATE_UPDATING;
				return TRUE;
			}
			break;

		default:
			break;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
// Function:    IfUpgradeEnable
// Description: �Ƿ�����������
// Parameters:  NONE
// Return:      TRUE : ����
// State:
//---------------------------------------------------------------------------
static int8u IfUpgradeEnable(void)
{
	int8u ret;

	ret = (int8u)GetDcuState();

	switch(ret)
	{
		case DCU_STATE_IDLE:
		case DCU_STATE_STOP:
			return TRUE;

		case DCU_STATE_AUTO:
			ret = (int8u)GetDoorState();
			if(ret == DOOR_STATE_CLOSE)
			{
				return TRUE;
			}
			break;

		case DCU_STATE_HAND:
			ret = (int8u)GetDoorState();
			if((ret == DOOR_STATE_CLOSE) || (ret == DOOR_STATE_OPEN))
			{
				return TRUE;
			}
			break;
	}

	return FALSE;
}

//---------------------------------------------------------------------------
// Function:    GetUpgradeState
// Description: ���DCU�Ƿ�������״̬���ṩ�����ܵ���
// Parameters:  NONE
// Return:      ����״̬( 0 ��ʾû�д�������)
// State:
//---------------------------------------------------------------------------
int8u GetUpgradeState(void)
{
	return (u8ProFlag & fgSlvUpgrading);
}

#endif //#if ((CDRV_TYPE == CDRV_PEDC))
#endif //#if ((UPDATE_FUN_EN == TRUE))




