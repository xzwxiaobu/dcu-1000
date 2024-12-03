#ifndef _COMMOBJ_CFG_H
#define _COMMOBJ_CFG_H

#include "_g_canbus.h"
#include "glbUpdate.h"

#if (defined (PCB_MON_V1))
    #define PCB_TYPE_ID         COMMOBJ_MONITOR
#elif (defined (PCB_FD_PEDC_SZ3Q_M))
    #define PCB_TYPE_ID         COMMOBJ_MONITOR
#elif (defined (PCB_MON_SZ20_V1))
    #define PCB_TYPE_ID         COMMOBJ_MONITOR
#else
    #error "UNKNOWN PCB!"
#endif

#include "commDef.h"


#endif  //_COMMOBJ_CFG_H
