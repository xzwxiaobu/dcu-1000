#include "_g_canbus.h"

#include "drv\_glbApi.h"

#include "..\Include.h"

 //---------------------------------------------------------------------------
const int8u  CmdTransType[]=
{
      fgDnload,              //0 给DCU命令
      fgDnload,              //1 参数下载
      fgUpload,              //2 开门曲线数据获取
      fgUpload,              //3 关门曲线数据获取
      fgUpload,              //4 读取在线单个DCU 状态
      fgUpload,              //5 读取不在线 DCU,判断是否上线
      fgDnload,              //6 PEDC 发送升级数据
      fgUpload,              //7 查询DCU信息,  1字节查询内容
      fgUpload,              //8 Can1 管理, 定时查询
      fgUpload,              //9 Can2 管理, 定时查询
      fgUpload,              //a 参数上载
      fgDnload,                     //11, 2023-8-21 下载 DCU 里程数据
      fgUpload,                     //12, 2023-8-21 读取 DCU 里程数据
      fgDnload,                     //13  2022-4-8  下载DCU IP 参数
      fgUpload,                     //14  2022-4-8  上传DCU IP 参数
      fgDnload,              //f 系统时间下载
};

const tCmdPara T_CmdParaTable[]=
{
    {OBJ_00_FID,              OBJ_00_LEN,                    OBJ_00_ADR},
    {OBJ_01_FID,              OBJ_01_LEN,                    OBJ_01_ADR},
    {OBJ_02_FID,              OBJ_02_LEN,                    OBJ_02_ADR},
    {OBJ_03_FID,              OBJ_03_LEN,                    OBJ_03_ADR},
    {OBJ_04_FID,              OBJ_04_LEN,                    OBJ_04_ADR},
    {OBJ_05_FID,              OBJ_05_LEN,                    OBJ_05_ADR},

    {OBJ_06_FID,              OBJ_06_LEN,                    OBJ_06_ADR},
    {OBJ_07_FID,              OBJ_07_LEN,                    OBJ_07_ADR},
    {OBJ_08_FID,              OBJ_08_LEN,                    OBJ_08_ADR},
    {OBJ_09_FID,              OBJ_09_LEN,                    OBJ_09_ADR},

    {OBJ_0A_FID,              OBJ_0A_LEN,                    OBJ_0A_ADR},

    {FUNID_DOWNLOAD_DCUTRIP  ,(DCU_TRIPDATA_LEN + 1)   ,    (int8u*)CANBus_downloadTrip             },  //11, 2023-8-21 下载 DCU 里程数据
    {FUNID_UPLOAD_DCUTRIP    ,(DCU_TRIPDATA_LEN + 1)   ,    (int8u*)uploadDCU_Trip.oneDCUTripData   },  //12, 2023-8-21 读取 DCU 里程数据
    
    {FUNID_DOWNLOAD_DCUIPPARA,(ONE_DCU_IPPARA_BUFLEN+1),    (int8u*)CANBus_downloadIPPara           },  //2022-4-8 下载DCU IP 参数
    {FUNID_UPLOAD_DCUIPPARA,  (ONE_DCU_IPPARA_BUFLEN+1),    (int8u*)uploadDCU_IPPara.oneDCUIPPara   },  //2022-4-8 上传DCU IP 参数
    {OBJ_0F_FID,              OBJ_0F_LEN,                    OBJ_0F_ADR},
};




int16u GetDcuState(void)
{
	return DCU_STATE_IDLE;
}

int16u GetDoorState(void)
{
	return DOOR_STATE_CLOSE;
}

