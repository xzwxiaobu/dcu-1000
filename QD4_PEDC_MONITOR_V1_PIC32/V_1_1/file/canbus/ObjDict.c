#include "_g_canbus.h"

#include "drv\_glbApi.h"

#include "..\Include.h"

 //---------------------------------------------------------------------------
const int8u  CmdTransType[]=
{
      fgDnload,              //0 ��DCU����
      fgDnload,              //1 ��������
      fgUpload,              //2 �����������ݻ�ȡ
      fgUpload,              //3 �����������ݻ�ȡ
      fgUpload,              //4 ��ȡ���ߵ���DCU ״̬
      fgUpload,              //5 ��ȡ������ DCU,�ж��Ƿ�����
      fgDnload,              //6 PEDC ������������
      fgUpload,              //7 ��ѯDCU��Ϣ,  1�ֽڲ�ѯ����
      fgUpload,              //8 Can1 ����, ��ʱ��ѯ
      fgUpload,              //9 Can2 ����, ��ʱ��ѯ
      fgUpload,              //a ��������
      fgDnload,                     //11, 2023-8-21 ���� DCU �������
      fgUpload,                     //12, 2023-8-21 ��ȡ DCU �������
      fgDnload,                     //13  2022-4-8  ����DCU IP ����
      fgUpload,                     //14  2022-4-8  �ϴ�DCU IP ����
      fgDnload,              //f ϵͳʱ������
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

    {FUNID_DOWNLOAD_DCUTRIP  ,(DCU_TRIPDATA_LEN + 1)   ,    (int8u*)CANBus_downloadTrip             },  //11, 2023-8-21 ���� DCU �������
    {FUNID_UPLOAD_DCUTRIP    ,(DCU_TRIPDATA_LEN + 1)   ,    (int8u*)uploadDCU_Trip.oneDCUTripData   },  //12, 2023-8-21 ��ȡ DCU �������
    
    {FUNID_DOWNLOAD_DCUIPPARA,(ONE_DCU_IPPARA_BUFLEN+1),    (int8u*)CANBus_downloadIPPara           },  //2022-4-8 ����DCU IP ����
    {FUNID_UPLOAD_DCUIPPARA,  (ONE_DCU_IPPARA_BUFLEN+1),    (int8u*)uploadDCU_IPPara.oneDCUIPPara   },  //2022-4-8 �ϴ�DCU IP ����
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

