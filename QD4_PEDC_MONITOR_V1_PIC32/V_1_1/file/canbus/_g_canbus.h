
#ifndef _GLOBAL_H
#define _GLOBAL_H

#include "_cpu.h"
#include "flash.h"

//*******************************************
#define CDRV_PEDC            1
#define CDRV_DCU             2
#define CDRV_MCP             3                       //ע��: Ŀǰֻ��̨���߲Ż��� MCP
#define CDRV_TYPE            CDRV_PEDC

//*******************************************
//WTJ
#define MAX_DCU                 64
#define ONE_DCU_STATE_LEN       24
#define ONE_DCU_PARA_LEN        22
#define CURVE_LEN               242

#define ID_DCU_MIN              1       // Id_Dcu_Min =< ID <= Id_Dcu_Max
#define ID_DCU_MAX              (ID_DCU_MIN + MAX_DCU - 1)

#define ID_MCP                  (ID_DCU_MIN + MAX_DCU - 1)

#ifdef DEBUG_PRINTF_ERR
extern int8u aPRINTF_ERR_BUF[3 + 1];
#endif

extern void   DcuInc(unsigned char  * pID_DCU ,unsigned char DcuNum);                                    //DCU ��һ ���������ֵѭ������Сֵ

//*******************************************
#define SLAVE_NUM         (MAX_DCU)          //���� Slave ������ע���Ƿ���� MCP
#define BYTES_SLV_FLAG    8                  //��λ��ʾ����Ϣ��ÿ�� Slave ��Ӧһ��λ�����ֽ������뵽 int16u
#define BYTES_SLV_NUM     (MAX_DCU)          //ÿ�� Slave ��Ӧһ���ֽڣ����ֽ������뵽 int16u

#define GET_SLV_ID0(id)   (id - ID_DCU_MIN)  //Slave �� id ת������ 0 ��ʼ

#if (CDRV_TYPE != CDRV_PEDC)
extern unsigned char canbus_adr;
#define g_NodeId     canbus_adr              //�ڵ��Լ���id, ͨ�����뿪���趨
#endif

#if (CDRV_TYPE == CDRV_PEDC)
extern int8u g_DcuNum;
#endif

/*==============================================================================
 * �������, ������� 96 �ֽ�(����������)
 * �������ַ����Ǳ����� 
 * - PIC6010A EEDATA ��(Address = 0x7FF080)
 * - PIC32MX  FLASH  ��(Address = MAIN_CODE_ADDR)
 *
 * -----------------------------------------------------------------------------
 *   ��Ŀ�����������Ϣ��������:
 *   - "PEDC", "DCU ", �ȹؼ���(4�ַ������д,���㲹�ո�)���������λ, MMS ����ʱ�������ַ���, ���������ļ�(HEX)����
 *   - �ܳ��ȣ������հ��ַ���0x00�����96�ֽ�
 *   - ����������Ϣ��
 *      A. ������ͣ���PCB���Ӧ���ǹ̶���Ϣ��PEDC / PEDC-PIC24 / PEDC-IBP / PEDC-PSL / PEDC-SIG��
 *      B. ������Ŀ����ֱ��ʹ��ԭʼ��ͨ����������� UNIVERSAL ��ʾ��
 *      C. �������ʱ��
 *      D. ������Ŀ����汾������ֲ���ɵĹ��̣��������ǰ�İ汾�Ž����޸�
 *      E. ������õ�PCB��˿ӡ��PCB���Ӧ���ǹ̶���Ϣ
 *      F. ͨ������İ汾�ţ�����û����ͨ�����Ϊ�����汾�������������Բ���Ҫ
 *      G. У��ͣ���ѡ���� �������зָ�
 *   - E F �����һ�������� " [ ] " ���зָ�
 *   - У�����Ϣ�Ļ�ȡ
 *      1. �� () �ڲ�����κ���Ϣ��Ȼ����룬
 *      2. ��ICD���߽��������ص�оƬ�У�������3�ò�����Ϣ��
 *      3. �� �����ڡ�-->����Ŀ����������ˢ�£�Ȼ����У���ճ����()��
 *      4. �ٴα��룬�õ���Ŀ��HEX����ʱ��У����벽��3��У�����ֵ����ͬ
 */

#if (CDRV_TYPE == CDRV_PEDC)       
        #define PROJECT_INFOS                       \
            "PEDC: "                                \
            "QD4-ENG, "                             \
            "2024/06/18 11:00:04,"                  \
            "V1.0,"                                 \
            "[PEDC_MONITOR_SZ16"                    \
            "]"

#elif  (CDRV_TYPE == CDRV_MCP)                          
    #define PROJECT_NAME      "MCP: xxxx,"                   //<=30�ֽ�
    #define PROJECT_TIME      "20yy/mm/dd hh:mm:ss,"           //<=30�ֽ�
    #define PROJECT_VER       "V1.0,"                          //<=30�ֽ�
    #define PROJECT_OTHER     "FANGDA.LTD,FD-PEDC-IBP_200_CD3" //<=30�ֽ�
    
#endif

//=========================================== ��������������� : TRUE -- ֧��, FALSE -- ��֧��
#define UPDATE_FUN_EN     TRUE

#if (UPDATE_FUN_EN == TRUE)
    #define UPFW_EE(N) __attribute__((space(eedata_UpFw), aligned(N)))
    #define UPFW_COD   __attribute__((section(".CodeSec_Update")))

#else
    #define UPFW_EE(N) __attribute__((aligned(N)))
    #define UPFW_COD
#endif

#define UPFW_CODE_BUF_MAX  (512+32)    //�����ݳ������ 512B����Ϣ��� 24B�������������ݵ�У��� 1B

#endif
