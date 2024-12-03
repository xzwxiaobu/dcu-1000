//#ifndef PEDC_MONITOR_V2
//#define PEDC_MONITOR_V2  0      //0=PEDC-MONITOR-V1,  1=PEDC-MONITOR-V2
//#endif

#include <p32xxxx.h>
#include <plib.h>
//#include "C:\Program Files (x86)\Microchip\MPLAB C32 Suite\pic32-libs\peripheral\i2c\source\I2CPrivate.h"
#include <string.h>

#define SHOW_PIC24_BY_UART_IPS      0   //1 = 用 PSL 的通信口观察PIC24收发数据

#include "_cpu.h"
#include "Define.h"
#include "saveIbpX.h"

#include "canbus\_g_canbus.h"
#include "canbus\canbus.h"
#include "canbus\ObjDict.h"
#include "canbus\drv\_glbApi.h"
#include "canbus\drv\glbCan.h"
#include "canbus\flash.h"
#include "can.h"
#include "dcu.h"
#include "DCU_DOOR_STATE.H"

#include "Ini_Cpu.h"
#include "io_handle.h"
#include "test.h"

//#include "Ini_UART.h"
//#include "Interrupt.h"
//#include "errorMessage.h"
//#include "waveflash.h"
//#include "time.h"

#include "MonCommCfg.h"
#include "MMSUart.h"
#include "MMS_App.h"
#include "MMS_inhibit.h"

#include "ModbusDef.h"
#include "SigSys_App.h"
#include "SigSys_Uart.h"
#include "SigSys_Reg.h"


#include "mainIO.h"


