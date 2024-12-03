/****************************************************************************/

#ifndef _CAN_TGT_H_
#define _CAN_TGT_H_

#define COP_MEMCPY(dst,src,siz)         TgtMemCpy((void*)(dst),(void*)(src),(int16u)(siz));    //memcpy
#define COP_MEMSET(dst,val,siz)         TgtMemSet((void*)(dst),(char)(val),(int16u)(siz));     //memset
#define GET_TICK(a)                     ((a)=TgtGetTickCount())

extern volatile int16u wTgtTimerTick_l;             // Current Time Tick

extern void __attribute__((__interrupt__)) _C1Interrupt (void);
extern void __attribute__((__interrupt__)) _C2Interrupt (void);

extern int16u  TgtGetTickCount (void);
extern void* TgtGetCanBase (int8u bCanContr_p);
extern void  TgtEnableCanInterrupt1 (int8u fEnable_p);
extern void  TgtEnableCanInterrupt2 (int8u fEnable_p);
extern void  TgtMemCpy (void * pDst_p, void * pSrc_p, int16u wSiz_p);
extern void  TgtMemSet (void * pDst_p, int8u bVal_p, int16u wSiz_p);

extern void  SoftDlyXms(int16u wDly);
extern void  SysTick1Ms(void);


#endif //_CAN_TGT_H_

