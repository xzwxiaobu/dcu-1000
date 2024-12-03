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
* 文	 件	:	CanDrv.C
*			:	Can bus 应用驱动
* =============================================================================*/

#include "_g_canbus.h"
#include "glbCan.h"
#include "CanDrv.h"

/*****************************************************************************/
//u8Can1MsgRxCnt / u8Can2MsgRxCnt is updated in	the	CAN1 / CAN2	ISR.
static volatile	BYTE u8Can1MsgRxCnt	= 0;
static volatile	BYTE u8Can2MsgRxCnt	= 0;

static volatile	BYTE CAN1MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN1 FIFO	message	area.
static volatile	BYTE CAN2MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN2 FIFO	message	area.

volatile int16u wTgtTimerTick_l;             // Current Time Tick

/*****************************************************************************/
//
//					source code
//
/*****************************************************************************/
//---------------------------------------------------------------------------
// intialize CAN module
// CAN1	and	CAN2 are configed as the same
//---------------------------------------------------------------------------
void CdrvInit(CAN_MODULE CanNo)
{
	CAN_BIT_CONFIG canBitConfig;
/*
	// These functions are from	interrupt peripheral library.
	if(CanNo == CAN1)
	{
		Can1IntDisable();
	}
	else
	{
		Can2IntDisable();
	}
*/
	// Switch the CAN module ON	and	switch it to Configuration mode.
	// Wait	till the switch	is complete.
	CANEnableModule(CanNo,TRUE);
	CANSetOperatingMode(CanNo, CAN_CONFIGURATION);
	while(CANGetOperatingMode(CanNo) !=	CAN_CONFIGURATION);

	// Configure the CAN Module	Clock.
	canBitConfig.phaseSeg2Tq			= CAN_BIT_3TQ;
	canBitConfig.phaseSeg1Tq			= CAN_BIT_3TQ;
	canBitConfig.propagationSegTq		= CAN_BIT_3TQ;
	canBitConfig.phaseSeg2TimeSelect	= TRUE;
	canBitConfig.sample3Time			= TRUE;
	canBitConfig.syncJumpWidth			= CAN_BIT_2TQ;

	CANSetSpeed(CanNo,&canBitConfig,SYSTEM_FREQ,CAN_BUS_SPEED);

	// Assign the buffer area to the CAN module.
	if(CanNo ==	CAN1)
	{
		u8Can1MsgRxCnt = 0;
		CANAssignMemoryBuffer(CanNo,(void*)CAN1MessageFifoArea,sizeof(CAN1MessageFifoArea));
	}
	else
	{
		u8Can2MsgRxCnt = 0;
		CANAssignMemoryBuffer(CanNo,(void*)CAN2MessageFifoArea,sizeof(CAN2MessageFifoArea));
	}

	// Configure channel 0 for TX and message buffers with RTR disabled and low medium priority.
	// Configure channel 1 for RX and message buffers and receive the full message.
	CANConfigureChannelForTx(CanNo,	CAN_CHANNEL0, MAX_TX_BUF, CAN_TX_RTR_DISABLED, CAN_LOW_MEDIUM_PRIORITY);
	CANConfigureChannelForRx(CanNo,	CAN_CHANNEL1, MAX_RX_BUF, CAN_RX_FULL_RECEIVE);

	// Configure filters and mask.
	// Configure filter	0 to accept	SID	messages.
	// Configure filter	mask 0 to compare all the ID bits and
	//	 to	filter by the ID type specified	in the filter configuration.
	// Messages	accepted by	filter 0 should	be stored in channel 1.
	CANConfigureFilter		(CanNo,	CAN_FILTER0, 0xFFFF, CAN_SID);
	CANConfigureFilterMask	(CanNo,	CAN_FILTER_MASK0, 0x0000, CAN_SID, CAN_FILTER_MASK_IDE_TYPE);
	CANLinkFilterToChannel	(CanNo,	CAN_FILTER0, CAN_FILTER_MASK0, CAN_CHANNEL1);
	CANEnableFilter			(CanNo,	CAN_FILTER0, TRUE);

		/*
		CANEnableFilter			(CanNo,	CAN_FILTER0, FALSE);
		while(CANIsFilterDisabled(CanNo, CAN_FILTER0) == FALSE);
		*/

	// Enable interrupt	and	events.
	// Enable the receive channel not empty	event (CAN_CHANNEL_EVENT) and the receive channel event	(CAN_MODULE_EVENT).
	// The interrrupt peripheral library is	used to	enable the CAN interrupt to	the	CPU.
	CANEnableChannelEvent(CanNo, CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY, TRUE);
	CANEnableModuleEvent (CanNo, CAN_RX_EVENT, TRUE);

	// These functions are from	interrupt peripheral library.
	if(CanNo ==	CAN1)
	{
		INTSetVectorPriority(INT_CAN_1_VECTOR, INT_PRIORITY_LEVEL_6);
		INTSetVectorSubPriority(INT_CAN_1_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
		Can1IntEnable();
	}
	else
	{
		INTSetVectorPriority(INT_CAN_2_VECTOR, INT_PRIORITY_LEVEL_6);
		INTSetVectorSubPriority(INT_CAN_2_VECTOR, INT_SUB_PRIORITY_LEVEL_0);
		Can2IntEnable();
	}

	// Switch the CAN mode to normal mode.
	CANSetOperatingMode(CanNo, CAN_NORMAL_OPERATION);
	while(CANGetOperatingMode(CanNo) !=	CAN_NORMAL_OPERATION);
}

//---------------------------------------------------------------------------
// Description	: resets specific parts	of CAN driver
// Parameters	: u8CanNo(CAN1 or CAN2), u8RstCanDrv
// Returns		: none
//
//---------------------------------------------------------------------------
void CdrvReset (CAN_MODULE u8CanNo,int8u u8RstCanDrv)
{
	if(u8CanNo == CAN1)
	{
		Can1IntDisable();
		u8Can1MsgRxCnt = 0;
		if(u8RstCanDrv)
		{
			CdrvInit(CAN1);
		}
		Can1IntEnable();
	}
	else
	{
		Can2IntDisable();
		u8Can2MsgRxCnt = 0;
		if(u8RstCanDrv)
		{
			CdrvInit(CAN2);
		}
		Can2IntEnable();
	}
	return;
}

//---------------------------------------------------------------------------
//
// Description:	reads a	CAN	message	from buffer
// Parameters:	u8CanNo   = CAN1 or CAN2
//				pCanMsg_p = pointer of data saving
// Return:		TRUE or	FALSE
//
// Note: Call the CANUpdateChannel() function to let
//       CAN module know that the message processing
//       is done. Enable the receive channale not empty event
//       so that the CAN module generates an interrupt when
//       the event occurs the next time.
//---------------------------------------------------------------------------
int8u CdrvReadMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p)
{
	CANRxMessageBuffer * message;

	if(u8CanNo == CAN1)
	{
		if(u8Can1MsgRxCnt == 0)			   //没有数据
		{
			return FALSE;
		}

		message	= CANGetRxMessage(CAN1,CAN_CHANNEL1);
		CANUpdateChannel(CAN1, CAN_CHANNEL1);
		Can1IntDisable();
		u8Can1MsgRxCnt --;
		if(u8Can1MsgRxCnt == 0)			   //没有数据
		{
			CANEnableChannelEvent(CAN1,	CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY,	TRUE);
		}
		Can1IntEnable();
	}
	else
	{
		if(u8Can2MsgRxCnt == 0)			   //没有数据
		{
			return FALSE;
		}

		message	= CANGetRxMessage(CAN2,CAN_CHANNEL1);
		CANUpdateChannel(CAN2, CAN_CHANNEL1);
		Can2IntDisable();
		u8Can2MsgRxCnt --;
		if(u8Can2MsgRxCnt == 0)			   //没有数据
		{
			CANEnableChannelEvent(CAN2,	CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY,	TRUE);
		}
		Can2IntEnable();
	}

	if(message == NULL)
		return FALSE;

	wCanBusTime[u8CanNo] = 0;

	pCanMsg_p->m_CanId = message->msgSID.SID;
	pCanMsg_p->m_bSize = message->msgEID.DLC;
	TgtMemCpy((void*)pCanMsg_p->m_bData,(void*)message->data,(int16u)8);
	return TRUE;
}

//---------------------------------------------------------------------------
//
// Description: writes a CAN message to the CAN RX buffer
// Parameters:	u8CanNo = CAN1 or CAN2
// Return:      TRUE or FALSE
//
//---------------------------------------------------------------------------
int8u CdrvWriteMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p)
{
	CANTxMessageBuffer * message;

	//Returns a CANTxMessageBuffer type pointer to an empty message buffer in the TX channel.
	//Returns NULL if the channel is full and there aren't any empty message buffers
	message	= CANGetTxMessageBuffer(u8CanNo, CAN_CHANNEL0);
	if(message == NULL)
	{
		return FALSE;
	}

	message->msgSID.SID = pCanMsg_p->m_CanId;
	message->msgEID.DLC = pCanMsg_p->m_bSize;
	message->msgEID.EID = 0;
	message->msgEID.IDE = 0;
	TgtMemCpy(message->data,pCanMsg_p->m_bData,pCanMsg_p->m_bSize);

	 /*
		This function updates the CAN Channel internal pointers.
		This function should be called when a message has been read or processed completely
		from a CAN RX Channel (using the CANGetRxMessage() function).
		It should be called after a new message has been written to a CAN TX Channel
		(using the CANGetTxMessageBuffer() function) and before the CANFlushTxChannel() function.
		Writing to a CAN TX Channel that has not been updated will cause the last written message to be overwritten.
	*/
	CANUpdateChannel(u8CanNo,CAN_CHANNEL0);

	/*
		This routine causes all messages in the specified TX channel to be transmitted.
		All messages in the channel at the time of the flush operation will be transmitted.
		The TX channel flush operation should preferrably be called immediately after the CANUpdateChannel() function.
		This will ensure that messages are transmitted promptly.
	*/
	CANFlushTxChannel(u8CanNo,CAN_CHANNEL0);

	return TRUE;
}

// ----------------------------------------------------------------------------
//
// Function:   CdrvCheckTransmit
// Description:  check if it is a message in buffer to send
// Parameters:
// Returns:    0 -- Tx buffer is empty
//
// ----------------------------------------------------------------------------
int8u CdrvCheckTransmit(CAN_MODULE u8CanNo)
{
	CANFlushTxChannel(u8CanNo,CAN_CHANNEL0);
	return 1;												//to avoid warning by assembly
}

// ----------------------------------------------------------------------------
// Description:	function copies	a buffer to	an other one
// Parameters:	pDst_p	= desination pointer
//				pSrc_p	= source pointer
//				wSiz_p	= number of	bytes to copy (max.	65535)
// Returns:		void
// ----------------------------------------------------------------------------
void TgtInitCan(void)
{
	CdrvInit(CAN1);
	Can1IntEnable();

	#ifndef NO_CAN_BUS2
	CdrvInit(CAN2);
	Can2IntEnable();
	#endif
}

// ----------------------------------------------------------------------------
// Description:	function copies	a buffer to	an other one
// Parameters:	pDst_p	= desination pointer
//				pSrc_p	= source pointer
//				wSiz_p	= number of	bytes to copy (max.	65535)
// Returns:		void
// ----------------------------------------------------------------------------
void TgtMemCpy (void * pDst_p, void	* pSrc_p, int16u wSiz_p)
{
	int8u *	dst	  =	pDst_p;
	int8u *	src	  =	pSrc_p;

	while (wSiz_p >	0)
	{
		*dst = *src;
		src++;
		dst++;
		wSiz_p--;
	}
}

// ----------------------------------------------------------------------------
// Description:	function sets a	value to a buffer
// Parameters:	pDst_p	= desination pointer
//				bVal_p	= value	to be filled in
//				wSiz_p	= number of	bytes in buffer	(max. 65535)
// Returns:		void
// ----------------------------------------------------------------------------
void TgtMemSet (void * pDst_p, int8u bVal_p, int16u	wSiz_p)
{
	int8u *	dst	  =	pDst_p;

	while (wSiz_p >	0)
	{
		*dst = bVal_p;
		dst++;
		wSiz_p--;
	}
}

//---------------------------------------------------------------------------
// Description:	ISR	for	CAN1, Note that	there are many source events
//				These events are enabled by	the	CANEnableModuleEvent()
//				Here ONLY RX(CAN_RX_EVENT) and TX(CAN_TX_EVENT)	interrupts are enbaled!!!
//				See	CAN_MODULE_EVENT!
// Parameters:	void
// Return:		void
// State:
// NOTE:
//				1.
//				2.Interrupt flag is cleared at the end of the interrupt routine.
//				   This is because the event source that could have caused this interrupt to occur
//				  (CAN_RX_CHANNEL_NOT_EMPTY) is disabled. Attempting to
//				  clear the CAN1 interrupt flag when the the CAN_RX_CHANNEL_NOT_EMPTY
//				  interrupt is enabled will	not	have any effect	because	the base event is still present.
//---------------------------------------------------------------------------
void CAN1InterruptHandler(void)
{
	CAN_EVENT_CODE	eventCode;

	eventCode = CANGetModuleEvent(CAN1);
	if((eventCode & CAN_RX_EVENT) != 0)
	{
		switch(CANGetPendingEventCode(CAN1))
		{
			case CAN_CHANNEL1_EVENT:
				u8Can1MsgRxCnt ++;

				/*
				 * Note	that leaving the event enabled would
				 * cause the CPU to	keep executing the ISR since
				 * the CAN_RX_CHANNEL_NOT_EMPTY	event is persistent	(unless
				 * the not empty condition is cleared.)
				 */
				CANEnableChannelEvent(CAN1,	CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY,	FALSE);
				break;

		}
	}

	INTClearFlag(INT_CAN1);
}

//---------------------------------------------------------------------------
// Description:	ISR	for	CAN2, Note that	there are many source events
//				These events are enabled by	the	CANEnableModuleEvent()
//				Here ONLY RX(CAN_RX_EVENT) and TX(CAN_TX_EVENT)	interrupts are enbaled!!!
//				See	CAN_MODULE_EVENT!
// Parameters:	void
// Return:		void
// State:
//---------------------------------------------------------------------------
void CAN2InterruptHandler(void)
{
	CAN_EVENT_CODE	eventCode;

	eventCode = CANGetModuleEvent(CAN2);
	if((eventCode & CAN_RX_EVENT) != 0)
	{
		switch(CANGetPendingEventCode(CAN2))
		{
			case CAN_CHANNEL1_EVENT:
				u8Can2MsgRxCnt ++;

				/*
				 * Note	that leaving the event enabled would
				 * cause the CPU to	keep executing the ISR since
				 * the CAN_RX_CHANNEL_NOT_EMPTY	event is persistent	(unless
				 * the not empty condition is cleared.)
				 */
				CANEnableChannelEvent(CAN2,	CAN_CHANNEL1, CAN_RX_CHANNEL_NOT_EMPTY,	FALSE);
				break;
		}
	}

	INTClearFlag(INT_CAN2);
}

//---------------------------------------------------------------------------
// Function:    TgtGetTickCount()
// Description: function returns actual timer tick
// Parameters:  void
// Return:      int32u
// State:
//---------------------------------------------------------------------------
int16u TgtGetTickCount (void)
{
	return wTgtTimerTick_l;
}

//---------------------------------------------------------------------------
// Function:    SysTick1Ms()
// Description:
// Parameters:  void
// Return:      void
// State:
//---------------------------------------------------------------------------
void SysTick1Ms(void)
{
	wTgtTimerTick_l ++ ;   // Increment Time Tick
}

//---------------------------------------------------------------------------
// Function:    ISR_Handler_Can1()
// Description:
// Parameters:  void
// Return:      void
// State:
//---------------------------------------------------------------------------
void __attribute__((section (".SEG_Can1HANDLER"))) __attribute__ ((interrupt(ipl6AUTO))) __attribute__ ((vector(_CAN_1_VECTOR))) ISR_Handler_Can1(void)
{
	CAN1InterruptHandler();
}

//---------------------------------------------------------------------------
// Function:    ISR_Handler_Can2()
// Description:
// Parameters:  void
// Return:      void
// State:
//---------------------------------------------------------------------------
void __attribute__((section (".SEG_Can2HANDLER"))) __attribute__ ((interrupt(ipl6AUTO))) __attribute__ ((vector(_CAN_2_VECTOR))) ISR_Handler_Can2(void)
{
	CAN2InterruptHandler();
}


//=========================================================
void __attribute__((section (".SEG_NothingHANDLER"))) __attribute__ ((interrupt(ipl1AUTO))) __attribute__ ((vector(63))) ISR_Handler_Nothing(void)
{
	asm volatile ("nop");
	asm volatile ("nop");
	while(1);
}
