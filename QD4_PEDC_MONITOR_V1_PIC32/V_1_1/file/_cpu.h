

#include <p32xxxx.h>
#include "GenericTypeDefs.h"

//--------------------------------------
#ifndef _CPU_H
#define _CPU_H

#define GetSystemClock()            (80000000ul)
#define GetPeripheralClock()        (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define GetInstructionClock()       (GetSystemClock())

//--------------------------------------
typedef unsigned char           int8u;
typedef signed char             int8;
typedef unsigned short          int16u;
typedef signed short            int16;
typedef unsigned long           int32u;
typedef signed long             int32;
typedef unsigned long long      int64u;
typedef signed long long        int64;
typedef float                   fp32;
typedef long double             fp64;


//"GenericTypeDefs.h"已经定于枚举类型
//#define TRUE                   1
//#define FALSE                  0
//

#define true                     TRUE
#define false                    FALSE

//--------------------------------------
#endif


