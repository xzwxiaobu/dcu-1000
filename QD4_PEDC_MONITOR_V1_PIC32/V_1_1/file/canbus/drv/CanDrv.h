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
* 文	 件	:	CanDrv.h
*			:	Can bus 应用驱动
* =============================================================================*/
#ifndef _TASK_CAN_H
#define _TASK_CAN_H

/*
// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "croutine.h"
*/

#include "GenericTypeDefs.h"
#include <p32xxxx.h>
#include <sys/kmem.h>
#include "plib.h"

#define SYSTEM_FREQ		80000000
#define CAN_BUS_SPEED	125000

#define CAN1_BRPVAL	0x7			/* CAN speed select - 0x7 for 500kbps, 0xF for 250Kbps, 0x3 for 1Mbps 		*/
#define CAN2_BRPVAL 0x7			/* Use same speed for both CAN modules. Time quanta per bit is set to 10.	*/
								/* See function CAN1Init() and CAN2Init().*/

#define Can1IntEnable()			INTEnable(INT_CAN1, INT_ENABLED)
#define Can1IntDisable()		INTEnable(INT_CAN1, INT_DISABLED)
#define Can2IntEnable()			INTEnable(INT_CAN2, INT_ENABLED)
#define Can2IntDisable()		INTEnable(INT_CAN2, INT_DISABLED)

/****************************************************************************
  Description:
	This function initializes CAN1 or CAN2 module.
	It sets up speed, FIFOs,filters and interrupts.
	FIFO0 is set up for TX with 8 message buffers.
	FIFO1 is set up for RX with 8 message buffers.
	Only RXNEMPTY interrupt and RBIF interrupt is enabled.
  ***************************************************************************/
void CdrvInit(CAN_MODULE CanNo);

/****************************************************************************
  Description:
	This function checks if a message is available to be read in CAN1 FIFO1.
  ***************************************************************************/
void CAN1RxMsgProcess(void);

/****************************************************************************
  Description:
	This function checks if a message is available to be read in
	CAN2 FIFO1. If a message is available, the function will check
	byte 0 (boolean flag) of the CAN message payload and will switch
	LED5 accordingly. It then calls CAN2TxSendLEDMsg().
  ***************************************************************************/
void CAN2RxMsgProcess(void);

/****************************************************************************
  Description:
  ***************************************************************************/
void taskCAN(void* pvParameter);





extern volatile int16u wTgtTimerTick_l;             // Current Time Tick
extern int16u  TgtGetTickCount (void);
extern void  SysTick1Ms(void);

extern void  TgtMemCpy (void * pDst_p, void * pSrc_p, int16u wSiz_p);
extern void  TgtMemSet (void * pDst_p, BYTE bVal_p, int16u wSiz_p);


#endif
