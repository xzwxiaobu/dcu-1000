
#ifndef _Objdict_h_
#define _Objdict_h_

#include "update.h"

// ==========================================================================
typedef struct
{
		int16u    FunId;
		int16u    wLen;
		int8u    *Buf;
} tCmdPara;


#ifndef NODO
#define NODO  0xFFFF
#endif


// ==========================================================================
#define FUNID_canbus_down_cmd             0x00    //给DCU命令
#define FUNID_Down_MoreDcuParameter       0x01    //参数下载
#define FUNID_Up_OpenDoorCurve            0x02    //开门曲线数据获取
#define FUNID_Up_CloseDoorCurve           0x03    //关门曲线数据获取
#define FUNID_UpOneDcuState               0x04    //读取在线单个DCU 状态
#define FUNID_TestDcuIfOnLineDcu          0x05    //读取不在线 DCU,判断是否上线
#define FUNID_UPDATE_FIRMWARE             0x06    //PEDC 发送升级数据
#define FUNID_QUERY                       0x07    //查询DCU信息,  1字节查询内容
#define FUNID_CANMAN1                     0x08    //Can1 管理, 定时查询
#define FUNID_CANMAN2                     0x09    //Can2 管理, 定时查询
#define FUNID_Up_MoreDcuParameter         0x0A    //参数上载
//#define FUNID_Up_LightScreen              0x0B    //光幕上载
//#define FUNID_Down_LightScreen            0x0C    //光幕下载

/* 设置或读取滑动门里程
 * 字节  0 : 扩展命令 
 * 字节1~4 : 4字节数据
 * 字节  5 : 校验和
 *  */
#define FUNID_DOWNLOAD_DCUTRIP              0x0B    //2023-8-12 下载DCU 里程数据
#define FUNID_UPLOAD_DCUTRIP                0x0C    //2023-8-12 上传DCU 里程数据

#define FUNID_DOWNLOAD_DCUIPPARA            0x0D    //2022-4-8 下载DCU IP 参数
#define FUNID_UPLOAD_DCUIPPARA              0x0E    //2022-4-8 上传DCU IP 参数

#define FUNID_PEDC_SYSTIME              0x0F      //PEDC 发送的系统时间，DCU当作广播命令处理，不回复

// ==========================================================================
#if (CDRV_TYPE == CDRV_PEDC)
//	#define OBJ_LS_0_FID                     NODO        
//	#define OBJ_LS_0_LEN                     0           
//	#define OBJ_LS_0_ADR                     (int8u*)NULL
//	#define OBJ_LS_1_FID                      FUNID_Down_LightScreen
//	#define OBJ_LS_1_LEN                      (DNLIGHTSCREENLEN + 1)
//	#define OBJ_LS_1_ADR                      (int8u*)&(takLightScreen.canbusDnLightScreenBuf)

	#define OBJ_00_FID                        FUNID_canbus_down_cmd
	#define OBJ_00_LEN                        sizeof(CANSendBroadCmdPkg)
	#define OBJ_00_ADR                        (int8u*)&(CANSendBroadCmdPkg)
	#define OBJ_01_FID                        FUNID_Down_MoreDcuParameter
	#define OBJ_01_LEN                        (ONE_DCU_PARA_LEN + 1)
	#define OBJ_01_ADR                        (int8u*)&(tagDOWN_MORE_DCU_PARAMETER.aDownParameterBuf)
	#define OBJ_02_FID                        FUNID_Up_OpenDoorCurve
	#define OBJ_02_LEN                        (CURVE_LEN + 1)
	#define OBJ_02_ADR                        (int8u*)&(tagUPOPENDOORCURVE.aUpOpenDoorCurveBuf)
	#define OBJ_03_FID                        FUNID_Up_CloseDoorCurve
	#define OBJ_03_LEN                        (CURVE_LEN + 1)
	#define OBJ_03_ADR                        (int8u*)&(tagUPCLOSEDOORCURVE.aUpCloseDoorCurveBuf)
	#define OBJ_04_FID                        FUNID_UpOneDcuState
	#define OBJ_04_LEN                        (ONE_DCU_STATE_LEN + 1)
	#define OBJ_04_ADR                        (int8u*)&(tagDcuState.aUpOneDcuStateBuf)
	#define OBJ_05_FID                        NODO          //FUNID_TestDcuIfOnLineDcu
	#define OBJ_05_LEN                        0             //(1 + 1)
	#define OBJ_05_ADR                        (int8u*)NULL  //(int8u*)&(tagOffLineDcu.aOffLineTestBuf)

	#define OBJ_06_FID                        FUNID_UPDATE_FIRMWARE
	#define OBJ_06_LEN                        0
	#define OBJ_06_ADR                        (int8u*)NULL
	#define OBJ_07_FID                        FUNID_QUERY
	#define OBJ_07_LEN                        (BYTES_INFO+1)
	#define OBJ_07_ADR                        (int8u*)DeviceInfo

	#define OBJ_08_FID                        FUNID_CANMAN1
	#define OBJ_08_LEN                        sizeof(CanOnLine)
	#define OBJ_08_ADR                        (int8u*)CanOnLine

	#define OBJ_09_FID                        FUNID_CANMAN2
	#define OBJ_09_LEN                        sizeof(CanOnLine)
	#define OBJ_09_ADR                        (int8u*)CanOnLine

	#define OBJ_0A_FID                        FUNID_Up_MoreDcuParameter
	#define OBJ_0A_LEN                        (ONE_DCU_PARA_LEN + 1)
	#define OBJ_0A_ADR                        (int8u*)&(tagUpDcuParameter.aUpDcuParameterBuf)




	#define OBJ_0F_FID                        FUNID_PEDC_SYSTIME
	#define OBJ_0F_LEN                        (BYTES_SYSTIME_TODCU + 1)
	#define OBJ_0F_ADR                        (int8u*)(&sendDCUSysTime)
#else
#error "TBD !"
#endif

// ==========================================================================
extern const int8u         CmdTransType[];
extern const tCmdPara     T_CmdParaTable[];

#define GET_TRANS_TYPE(fun_id)            CmdTransType[fun_id]
#define GET_TRANS_LEN(fun_id)             T_CmdParaTable[fun_id].wLen
#define GET_TRANS_BUF(fun_id)             T_CmdParaTable[fun_id].Buf

#endif
