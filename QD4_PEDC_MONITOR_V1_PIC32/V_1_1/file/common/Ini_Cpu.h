
#ifndef _INI_CPU_H
#define _INI_CPU_H
//***********************************************************

//***********************************************************
#define LedOn()           mPORTBClearBits(BIT_6);
#define LedOff()          mPORTBSetBits(BIT_6);
#define LedToggle()       mPORTBToggleBits(BIT_6)

EXTN BIT16 wUserFlag0;
#define bLEDFLASH      wUserFlag0.B0

extern void CpuInit(void);
extern void RamIni(void);
extern void time_matter(void);          //时间间隔事件

//EXTN int8u uUART_ERR_TEST_TIME;

//***********************************************************

#endif

