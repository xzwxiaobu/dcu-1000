
#include "..\Include.h"

#include "SigSys_Reg.h"
#include "SigSys_App.h"
#include "dcu.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

/* 与信号系统通信的数据寄存器 */
tNJ7SigRegister NJ7SigRegister;

PRIVATE int8u PSDInhibitTrain[(MAX_DCU+7)/8];

/* 与信号系统通信的数据寄存器
 * 1. 车门对位隔离站台门 (信号系统发送的)
 * 2. 站台门对位隔离车门 (根据DCU信息判断得到，信号系统轮训读取)
 *  */
void GetMODBUSMessage(int8u *pBuf)
{
	int i;
    int8u *p;
    int8u *pInhibit;
        
    /* 得到站台门对位隔离列车门的信息 */
    getPSDInhibitTrain(PSDInhibitTrain, sizeof(PSDInhibitTrain));
    
    NJ7SigRegister.rPSDFault0 = ((int16u)PSDInhibitTrain[1] << 8) + (int16u)PSDInhibitTrain[0];
    NJ7SigRegister.rPSDFault1 = ((int16u)PSDInhibitTrain[3] << 8) + (int16u)PSDInhibitTrain[2];
    
    /* 列车门发送给站台门的对位隔离标志，
     * PEDC接收到的信号系统发送的列车门状态后，保存在 aMcpOpenCloseDoorCommand 
     */
    pInhibit = &uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[0];
    NJ7SigRegister.wTrainDoorFault0 = ((int16u)pInhibit[1] << 8) + (int16u)pInhibit[0];
    NJ7SigRegister.wTrainDoorFault1 = ((int16u)pInhibit[3] << 8) + (int16u)pInhibit[2];

    NJ7SigRegister.rsv4 = 0;
    NJ7SigRegister.rsv5 = 0;
    NJ7SigRegister.rsv6 = 0;
    NJ7SigRegister.rsv7 = 0;
    
    /* MODBUS 协议
     * 高字节和低字节交换位置
     *  */
    p = (int8u*)&NJ7SigRegister;
    for(i=0; i<sizeof(NJ7SigRegister); i+=2)
    {
        *pBuf++ = *(p+1);
        *pBuf++ = *p;
        
        p += 2;
    }
}


