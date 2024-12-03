
#include "..\Include.h"
#include "glbCan.h"
#include "CanDrv.h"
#include "IOFunct.h"

#include "MMSUart.h"
#include "MMS_App.h"

#include "IPSUart.h"
#include "IPS_App.h"

#include "P24UpdateDrv.h"
#include "sysTimer.h"

#include "relayLife.h"
#include "BASSys_App.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

/* �ۺϼ�صĶ�λ���빦��ͨ�� MMS ת����PEDC
 * 0 = ������, ���� = ����
 *  */
#define CMD_INHIBIT_BY_ISCS       1

/* MMS �����Ķ�λ���빦��
 * 0 = ������, ���� = ����
 *  */
#define CMD_INHIBIT_BY_MMS          0

PRIVATE defSTRU_MMS_INHIBIT RX_MMS_INHIBIT_MESSAGE; //����MMS ������PEDC ����Ϣ
PRIVATE defSTRU_MMS_INHIBIT TX_MMS_INHIBIT_MESSAGE; //PEDC������MMS ����Ϣ

PRIVATE int8u MMSTxBuf[MMS_TXBUF_SIZE];             /* ���ͻ����� */

static int32u  highdata;
static int8u   revflagstop;



//######################################################################
/*
������ �ļ�ͷDLE �ļ�ͷSTX ˳���SEQ Դ��ַ Ŀ���ַ ��������(HEX)  ���ݳ���:Len  �������� �ļ�βDLE   �ļ�βETX   У���
         0x10      0x02       0      &int8u  &int8u      ����        const int16    &int8u   0x10         0x3
*/
const KPEDC_UART2_TO_MMS  Uart2_protocol[] =
{
    {0x10, 0x02, 0, UART2_SOURCE_00, UART2_OBJECT_00, 0x10, UART2_LEN_00, UART2_Adr_00, 0x10, 0x3},   //0  ����״̬���ݻظ�
    {0x10, 0x02, 0, UART2_SOURCE_01, UART2_OBJECT_01, 0x20, UART2_LEN_01, UART2_Adr_01, 0x10, 0x3},   //1  �ϴ��������ݻظ�
    {0x10, 0x02, 0, UART2_SOURCE_02, UART2_OBJECT_02, 0x25, UART2_LEN_02, UART2_Adr_02, 0x10, 0x3},   //2  �㲥���ز���ȷ��
    {0x10, 0x02, 0, UART2_SOURCE_03, UART2_OBJECT_03, 0x30, UART2_LEN_03, UART2_Adr_03, 0x10, 0x3},   //3  �����������ݻظ�
    {0x10, 0x02, 0, UART2_SOURCE_04, UART2_OBJECT_04, 0xA0, UART2_LEN_04, UART2_Adr_04, 0x10, 0x3},   //4  �����������ݾܾ�
    {0x10, 0x02, 0, UART2_SOURCE_05, UART2_OBJECT_05, 0x31, UART2_LEN_05, UART2_Adr_05, 0x10, 0x3},   //5  �����������ݻظ�
    {0x10, 0x02, 0, UART2_SOURCE_06, UART2_OBJECT_06, 0xA1, UART2_LEN_06, UART2_Adr_06, 0x10, 0x3},   //6  �����������ݾܾ�
    {0x10, 0x02, 0, UART2_SOURCE_07, UART2_OBJECT_07, 0x50, UART2_LEN_07, UART2_Adr_07, 0x10, 0x3},   //7  ibp���������ϴ�����ظ�
    {0x10, 0x02, 0, UART2_SOURCE_08, UART2_OBJECT_08, 0x51, UART2_LEN_08, UART2_Adr_08, 0x10, 0x3},   //8  ibp�����������سɹ��ظ�
    {0x10, 0x02, 0, UART2_SOURCE_09, UART2_OBJECT_09, 0xC1, UART2_LEN_09, UART2_Adr_09, 0x10, 0x3},   //9  ibp�����������ؾܾ�
    {0x10, 0x02, 0, UART2_SOURCE_0A, UART2_OBJECT_0A, 0xE1, UART2_LEN_0A, UART2_Adr_0A, 0x10, 0x3},   //10 ��������״̬
    {0x10, 0x02, 0, UART2_SOURCE_0B, UART2_OBJECT_0B, 0xE6, UART2_LEN_0B, UART2_Adr_0B, 0x10, 0x3},   //11 ��PEDC/DCU�����Ϣ�ɹ�
    {0x10, 0x02, 0, UART2_SOURCE_0C, UART2_OBJECT_0C, 0xE7, UART2_LEN_0C, UART2_Adr_0C, 0x10, 0x3},   //12 ��PEDC/DCU�����Ϣʧ��
    {0x10, 0x02, 0, UART2_SOURCE_0D, UART2_OBJECT_0D, 0x52, UART2_LEN_0D, UART2_Adr_0D, 0x10, 0x3},   //13 �ϴ�DCU λ������
    {0x10, 0x02, 0, UART2_SOURCE_0E, UART2_OBJECT_0E, 0x53, UART2_LEN_0E, UART2_Adr_0E, 0x10, 0x3},   //14 ����DCU λ�����óɹ���ʧ�ܻظ�
    {0x10, 0x02, 0, UART2_SOURCE_0F, UART2_OBJECT_0F, 0x11, UART2_LEN_0F, UART2_Adr_0F, 0x10, 0x3},   /*15 MMS inhibit �ظ� */
    
    //2022-4-12
    {0x10, 0x02, 0, ((int8u*) &uSource), ((int8u*) &uObject), 0x21, 1, 0, 0x10, 0x3},                                   /*16 cUP_UPLOAD_IPPARA_NO */
    {0x10, 0x02, 0, ((int8u*) &uSource), ((int8u*) &uObject), 0x26, 8, DownloadDCU_IPPara.downloadIPResult, 0x10, 0x3}, /*17 cUP_DOWNLOAD_IPPARA_NO */
    
    //2023-8-21
    {0x10, 0x02, 0, ((int8u*) &uSource), ((int8u*) &uObject), 0x23, 1, 0, 0x10, 0x3},                                   /*18 cUP_UPLOAD_DCUTRIP_NO   */
    {0x10, 0x02, 0, ((int8u*) &uSource), ((int8u*) &uObject), 0x28, 8, DownloadDCU_Trip.downloadTripResult, 0x10, 0x3}, /*19 cUP_DOWNLOAD_DCUTRIP_NO */
};

#define StartUart2      1
#define NotStartUart2   0

/* =============================================================================
 * ׼�����͸� MMS ������
 *  TabNo           : Э������
 *  startUART_flag  : startUART == 1 ���� UART , startUART == 0 ������
 *  */
void  readyPkgToMMS(int8u  TabNo, int8u startUART_flag)
{
    pINT8U   Ptr;
    int16u   TxDataLen;
    
    KPEDC_UART2_TO_MMS   tagUart2_To_MMS;

    int8u* MMSTxBufPtr;
    MMSTxBufPtr = MMSTxBuf;
    
    Ptr = &(MMSTxBufPtr[8]);

    tagUart2_To_MMS = Uart2_protocol[TabNo];

    //������Uart2_protocol[]�е����ݳ��� == USE_DATALEN ,�� ʹ�� wDataLen ����ʾ�ĳ���
    if(tagUart2_To_MMS.Data_Len == USE_DATALEN)
    {
        tagUart2_To_MMS.Data_Len = wDataLen;
    }
    
    
    MMSTxBufPtr[0] = tagUart2_To_MMS.FileHead_DLE;
    MMSTxBufPtr[1] = tagUart2_To_MMS.FileHead_STX;
    MMSTxBufPtr[2] = tagUart2_To_MMS.Sequence;
    MMSTxBufPtr[3] = *tagUart2_To_MMS.Source;
    MMSTxBufPtr[4] = *tagUart2_To_MMS.Object;
    MMSTxBufPtr[5] = tagUart2_To_MMS.DataSORT;


    int i,j;
    pINT8U ptr1;
    int ID;
    int number = 0;

    if(TabNo == cUp_WorkState_protocol_No)           //��Э��֮���ر���
    {
        ID = ID_DCU_MIN;

        ptr1 = (pINT8U) &uniDcuData.aDcuData;
        for(i = 0; i < AVR_LEN; i ++)
        {
            *Ptr++ = *ptr1++;
        }

        for(i = 0; i < MAX_DCU; i ++)
        {
            if(ptrDcuIfOnLine((pINT8U)&uniDcuData.sortBuf.aDcuCfgBUF, ID) == BITEQU1)
            {
                number ++;

                ptr1 = (pINT8U) &uniDcuData.sortBuf.aDcuState[(ID - ID_DCU_MIN)][0];

                for(j = 0; j < ONE_DCU_STATE_LEN; j ++)
                {
                    *Ptr++ = *ptr1++;
                }
            }

            ID ++;
        }

        tagUart2_To_MMS.Data_Len = AVR_LEN + number * ONE_DCU_STATE_LEN;
    }
    //2022-4-8 ����: �ϴ� DCU IP�������⴦��
    else if(TabNo == cUP_UPLOAD_IPPARA_NO)
    {
        /* ���������DCU��־ */
        ptr1 = (int8u*) &uploadDCU_IPPara.uploadIPResult;
        for(i = 0; i < sizeof(uploadDCU_IPPara.uploadIPResult); i ++)
        {
            *Ptr++ = *ptr1++;
        }

        /* �������Ľ�����Ƶ����ͻ����� */
        ID      = ID_DCU_MIN;
        number  = 0;
        for(i = 0; i < MAX_DCU; i ++)
        {
            if(ptrDcuIfOnLine((int8u*)uploadDCU_IPPara.uploadIPResult, ID) == BITEQU1)
            {
                number ++;

                ptr1 = (int8u*) &uploadDCU_IPPara.allDCU_IPPara[(ID - ID_DCU_MIN)][0];
                for(j = 0; j < ONE_DCU_IPPARA_BUFLEN; j ++)
                {
                    *Ptr++ = *ptr1++;
                }
            }

            ID ++;
        }

        tagUart2_To_MMS.Data_Len = sizeof(uploadDCU_IPPara.uploadIPResult) + number * ONE_DCU_IPPARA_BUFLEN;
    }
    //2023-8-21 ���⴦�� : �ϴ� DCU ���
    else if(TabNo == cUP_UPLOAD_DCUTRIP_NO)
    {
        /* ���������DCU��־ */
        ptr1 = (int8u*) &uploadDCU_Trip.uploadTripResult;
        for(i = 0; i < sizeof(uploadDCU_Trip.uploadTripResult); i ++)
        {
            *Ptr++ = *ptr1++;
        }

        /* �������Ľ�����Ƶ����ͻ����� */
        ID      = ID_DCU_MIN;
        number  = 0;
        for(i = 0; i < MAX_DCU; i ++)
        {
            if(ptrDcuIfOnLine((int8u*)uploadDCU_Trip.uploadTripResult, ID) == BITEQU1)
            {
                number ++;

                ptr1 = (int8u*) &uploadDCU_Trip.allDCU_Trip[(ID - ID_DCU_MIN)][0];
                for(j = 0; j < DCU_TRIPDATA_LEN; j ++)
                {
                    *Ptr++ = *ptr1++;
                }
            }

            ID ++;
        }

        tagUart2_To_MMS.Data_Len = sizeof(uploadDCU_Trip.uploadTripResult) + number * DCU_TRIPDATA_LEN;
    }
    else
    {
        int16u   Len;
        Len = tagUart2_To_MMS.Data_Len;
        while(Len)
        {
            *Ptr++ = *tagUart2_To_MMS.DataContentSP++;
            Len --;
        }
    }

    MMSTxBufPtr[6] = tagUart2_To_MMS.Data_Len / 256;
    MMSTxBufPtr[7] = tagUart2_To_MMS.Data_Len % 256;
    
    *Ptr++ = tagUart2_To_MMS.FileEnd_DLE;
    *Ptr++ = tagUart2_To_MMS.FileEnd_ETX;
    *Ptr   = checksum(&(MMSTxBufPtr[0]), tagUart2_To_MMS.Data_Len + (UART2_TX_FILEMESSAGE_LEN - 1));

    TxDataLen = tagUart2_To_MMS.Data_Len + (UART2_TX_FILEMESSAGE_LEN );
    

    
    /* �������ͣ� */
    if(startUART_flag)
    {
        sendPackageToMMS(MMSTxBuf, TxDataLen);
    }
}


/* =============================================================================
 * PIC24 �����������������ݸ� MMS 
 */
PUBLIC BOOL upPIC24_SendMsgToMMS(int8u msgType, int8u srcNode, int8u * data, int16u dataLen)
{
    int8u* ptr;
    BOOL ret = FALSE;
    
    if(dataLen >= MMS_TXBUF_SIZE - sizeof(tPktHeader) - sizeof(tPktTail))
        return FALSE;
    
    ptr = MMSTxBuf;
    *ptr ++= HEADER_DLE;        /* ֡ͷ1    */
    *ptr ++= HEADER_STX;        /* ֡ͷ2    */
    *ptr ++;                    /* ֡���   */
    *ptr ++= srcNode;           /* Դ�ڵ�   */
    *ptr ++= OBJECT_MMS;        /* Ŀ��ڵ� */
    *ptr ++= msgType;           /* ͨ���������� tEnmCommObjCmd  */
    *ptr ++= dataLen >> 8;      /* ���ݳ��� */
    *ptr ++= dataLen;           /* ���ݳ��� */
    
    memcpy(ptr, data, dataLen); /* �������� */
    ptr += dataLen;

    *ptr ++= TAIL_DLE;
    *ptr ++= TAIL_ETX;
    *ptr ++= checksum(MMSTxBuf, ptr - MMSTxBuf);
    
    
    tEnmComTxSts sts = sendPackageToMMS(MMSTxBuf, ptr - MMSTxBuf);
    if(sts == COM_TX_OK)
    {
        ret = TRUE;
    }
    
    return ret;
}

//######################################################################
void Up_WorkState(void)
{
    pINT8U pStr1, pStr2;
    int8u i;
    if(!two_can)  //0 ��CAN; 1 ˫CAN 
    { //��CAN: CAN1 |= CAN2, CAN2 = 0; Ŀ�ģ�ƥ��MMS��CAN��Ӧ CAN1������PEDC������CAN1�ڻ�CAN2��
        pStr1 = (pINT8U)&uniDcuData.sortBuf.aDcuCan12State;
        pStr2 = (pINT8U)&uniDcuData.sortBuf.aDcuCan12State[DCU_ONECAN_ONLINE_LEN];
        
        for(i = 0; i < DCU_ONECAN_ONLINE_LEN; i ++)
        {
            *pStr1 |= (*pStr2);
            
            *pStr2 = 0;
            
            pStr1++;
            pStr2++;
        }
        
    }   
    
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;

    for(i = 0; i < DCUsetting_LEN; i ++)  //��FLASH ��ȡ�� ���ø�ֵ�� settingBUF
    {
            uniDcuData.sortBuf.aDcuCfgBUF[i] = uDCUsetting.settingBUF[i];
    }
    
    for(i = 0; i < DCUsetting_LEN; i ++)  //��FLASH ��ȡ�� ���ø�ֵ�� settingBUF
    {
            uniDcuData.sortBuf.aDcuCan12State[i] = uDCUsetting.settingBUF[i];
    }
    
    for(i = 0; i < DCUsetting_LEN; i ++)  //��FLASH ��ȡ�� ���ø�ֵ�� settingBUF
    {
            uniDcuData.sortBuf.aDcuCan12State[8+i] = uDCUsetting.settingBUF[i];
    }
    

    wDataLen = DCU_cfg_LEN + AVR_DATA_LEN + MCP_OPENCLOSE_DOOR_LEN + \
          + DCU_CAN12_STATE_LEN + 24 * ONE_DCU_STATE_LEN;
    
    
        int32u REVtime ;
        int32u Highdate_cnt ;

        REVtime =(int32u)getPedcID()*1000  ;
        
    
    
        static tSYSTICK DateREVTick;
        static tSYSTICK tickNow;
        static BOOL  revFLAG;
  
        tickNow = getSysTick();   
        
    if(revflagstop==0)
    {
        if((tSYSTICK)(tickNow - DateREVTick) >= REVtime)
        {   
               scanPEDC_ID();
               DateREVTick = tickNow;
               revFLAG= !revFLAG;
                 
               if( revFLAG == true)
               {
                 highdata++;
               
               }
               
               
        }

        if( revFLAG == true)
        {    
             //revFLAG = false ;
             memset((int8u *) &uniDcuData.aDcuData[24], 0xff, AVR_ONLINE_ALLDCUSTATE_LEN-24);
             
        }
        else 
        {
            //revFLAG = true  ;
            memset((int8u *) &uniDcuData.aDcuData[24], 0x00, AVR_ONLINE_ALLDCUSTATE_LEN-24);

        }

        uniDcuData.aDcuData[84] = (highdata >>24)& 0xff  ;
        uniDcuData.aDcuData[85] = (highdata >>16)& 0xff  ;
        uniDcuData.aDcuData[86] = (highdata >>8) & 0xff  ;
        uniDcuData.aDcuData[87] = (highdata    ) & 0xff  ;

    }
    else
    {
             memset((int8u *) &uniDcuData.aDcuData[24], 0x00, 59-24+1);
    
    
    }
        
        
        
        
        
        
        
        
    readyPkgToMMS(cUp_WorkState_protocol_No, StartUart2);
    
//    if(revFLAG == false)
//    {
//        if(MMSTxBuf[40] != 0xff)
//        {
//            MMSTxBuf[40] = 0xff;
//        }
//    }
    

    if(!two_can)  //0 ��CAN; 1 ˫CAN 
    {   
        DcuCan12State(FALSE);                        //��DCU can12 ����Ϣ���ݸ� uniDcuData
    }
}


void UpMoreDcuParameter(void)
{
    uSource = SOURE_BROADCAST;
    uObject = OBJECT_MMS;
    wDataLen = (int16u)(defPARAMETER_FLAG_LEN + ONE_DCU_PARA_LEN * tagUpDcuParameter.aUpDcuParameter_DcuNum);
    readyPkgToMMS(cUpMoreDcuParameter_protocol_No, StartUart2);
}

void DownMoreDcuParameter(void)
{
    uSource = SOURE_BROADCAST;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cDownMoreDcuParameter_protocol_No, StartUart2);
}

void UpOpenDoorCurveOK(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUpOpenDoorCurveOK_protocol_No, StartUart2);
}

void UpOpenDoorCurveFAIL(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUpOpenDoorCurveFAIL_protocol_No, StartUart2);
}

void UpCloseDoorCurveOK(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUpCloseDoorCurveOK_protocol_No, StartUart2);
}

void UpCloseDoorCurveFAIL(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUpCloseDoorCurveFAIL_protocol_No, StartUart2);
}

void Up_IbpAlarmParameter(void)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUp_IbpAlarmParameter_protocol_No, StartUart2);
}

//�ܽ�=2,Success=1,Fail=0
void Down_IbpAlarmParameter(int8u flag)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    tagDownIbpAlarmParameter.DownFlag = flag;
    readyPkgToMMS(cDown_IbpAlarmParameterOK_protocol_No, StartUart2);
}

void Down_IbpAlarmParameterFAIL(void)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cDown_IbpAlarmParameterFAIL_protocol_No, StartUart2);
}

void Up_PEDCHardwareSetting(void)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUp_PEDCHardwareSetting_No, StartUart2);
}

void Down_PEDCHardwareSetting(void)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cDown_PEDCHardwareSetting_No, StartUart2);
}

void Up_MMS_inhibit(void)
{
    uSource = SOURE_PEDC;
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUP_MMS_inhibit_No, StartUart2);
}

//lly ��������״̬
extern BOOL ifBatchUpdateDCUs(void);
extern int getReplyDCUUpdateState(int8u* pData, int bufLen);
void UPDATE_FIRMWAREStateToMMS(void)
{
    #if 0
    /* ��ǰ�İ汾 */
    uObject = OBJECT_MMS;
    readyPkgToMMS(cUpgradeState_protocol_No, StartUart2);
    
    #else
    uObject = OBJECT_MMS;
    if(MMS_OP_DST_ID != UP_PEDC_MORE_DCUS)          /* ifBatchUpdateDCUs() */
    {
        readyPkgToMMS(cUpgradeState_protocol_No, StartUart2);
    }
    else
    {
        /* ��������DCU�Ļظ� */
        int8u*  pTxPtr;
        int8u*  Ptr;
        int16u  Len;
        int16u  TxDataLen;
        KPEDC_UART2_TO_MMS   tagUart2_To_MMS;

        pTxPtr = MMSTxBuf;

        tagUart2_To_MMS = Uart2_protocol[cUpgradeState_protocol_No];
        pTxPtr[0] = tagUart2_To_MMS.FileHead_DLE;
        pTxPtr[1] = tagUart2_To_MMS.FileHead_STX;
        pTxPtr[2] = tagUart2_To_MMS.Sequence;
        pTxPtr[3] = *tagUart2_To_MMS.Source;
        pTxPtr[4] = *tagUart2_To_MMS.Object;
        pTxPtr[5] = tagUart2_To_MMS.DataSORT;

        Len = getReplyDCUUpdateState(&pTxPtr[8], MMS_TXBUF_SIZE - UART2_TX_FILEMESSAGE_LEN);
        pTxPtr[6] = Len >> 8;
        pTxPtr[7] = Len & 0xff;
        
        Ptr = &(pTxPtr[8]) + Len;
        *Ptr++ = tagUart2_To_MMS.FileEnd_DLE;
        *Ptr++ = tagUart2_To_MMS.FileEnd_ETX;
        *Ptr   = checksum(&(pTxPtr[0]), Len + (UART2_TX_FILEMESSAGE_LEN - 1));

        TxDataLen = Len + (UART2_TX_FILEMESSAGE_LEN );

        /* �������ͣ� */
        sendPackageToMMS(MMSTxBuf, TxDataLen);
    }
    #endif
}

void UpFwStateToMMS(void)    //����״̬���� TO MMS
{
    bUART2_UPDATE_FIRMWARE = 1;
}

void QueryOK(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cQUERYOK, StartUart2);
}

void QueryFAIL(void)
{
    uObject = OBJECT_MMS;
    readyPkgToMMS(cQUERYFAIL, StartUart2);
}

/* =============================================================================
 * �������� MMS ������֡ (Uart_MMS_Rx_Handle)
 * ����
 *  mmsRxBufPtr �ǽ�������
 *  mmsRxLen    �ǽ��ճ���
 * ����
 *  TRUE ��ʾ����֡������
 *  FALSE ��ʾ����֡δ����
 */
PRIVATE BOOL MMS_AppRxMsg(int8u* mmsRxBufPtr, int mmsRxLen)
{
    int i;
    int8u * pStr1;
    int8u * pStr2;
    int8u * ptr1;
    BOOL   ret = TRUE;
    
    int16u dataLen;
    int8u  *ptrData;
    
    /* ���ݵ�ַ */
    ptrData = &mmsRxBufPtr[8];
    /* ���ݳ��� */
    dataLen = ((int16u)mmsRxBufPtr[6] << 8) + (int16u)mmsRxBufPtr[7];
    
    
    /* һ���յ��ǲ��Թ��ܵ�������˳�����״̬ */
    setDoTestFunct(FALSE);
    
    /* �ж� MMS ���͵��������� */
    switch(mmsRxBufPtr[5])
    {
        case 0x10:
            if(bFun_Up_WorkState == 1)
                break;

            /* �ϰ汾 �� ���� + ʱ�� */
            if(dataLen >= 15)                   
            {
                bFun_Up_WorkState = 1;        //����״̬��������
                
                /* ������ѯ DCU, 8 bytes */
                for(i = 0; i < DCUsetting_LEN; i ++)
                {
                    uDCUsetting.settingBUF[i] = *ptrData ++;
                }

                /* ϵͳʱ�� 7 bytes */
                tSysTime tmpTime;
                tmpTime.year    = (int16u)(mmsRxBufPtr[8+i+0]) + ((int16u)(mmsRxBufPtr[8+i+1]) << 8);
                tmpTime.month   = mmsRxBufPtr[8+i+2];
                tmpTime.day     = mmsRxBufPtr[8+i+3];
                tmpTime.hour    = mmsRxBufPtr[8+i+4];
                tmpTime.min     = mmsRxBufPtr[8+i+5];
                tmpTime.second  = mmsRxBufPtr[8+i+6];
                setPedcSysTime(&tmpTime);
                
                /* �°汾 �� ���� + ʱ�� + ��λ���� */
                if(dataLen == 25)
                {
                    #if  (CMD_INHIBIT_BY_ISCS != 0)
                    /* ״̬��         1 byte (bit0 ��־= 1: SIG-MMSͨѶ�ж�)
                     * ����δ����     1 byte
                     * ��λ��������   8 bytes
                     *  */
                    tBroadCmdPkg tmpBroadCmd;
                    int8u sigSysSts;
                    
                    ptrData += sizeof(tSysTime);
                    sigSysSts = *ptrData++;
                    ptrData++;
                    
                     if((sigSysSts & BIT_7) != 0)
                     {
                     
                        highdata =0;
  
                     }
                     
                     if((sigSysSts & BIT_6) !=  0)
                     {
  
                        revflagstop =1;
                  
                     }
                     else
                     {
                     
                         revflagstop =0;
                     
                     }
                    
                    
                    
                    
                    /* �ź�ϵͳ���͸� MMS �ĳ��Ŷ�λ����վ̨�� */
                    tmpBroadCmd.oprCmd1.byte = STC_INHIBIT_ENABLE;
                    tmpBroadCmd.oprCmd2.byte = 0;
                    tmpBroadCmd.oprCmd3.byte = 0;
                    if((sigSysSts & BIT_0) == 0)
                    {
                        /* SIG-MMSͨѶ���� */
                        for(i = 0; i<sizeof(tmpBroadCmd.broadCmdBuf); i++)
                        {
                            tmpBroadCmd.broadCmdBuf[i] = ptrData[i];
                        }
                    }
                    else
                    {
                        /* SIG-MMSͨѶ�жϣ�������Ŷ�λ����վ̨�� */
                        for(i = 0; i<sizeof(tmpBroadCmd.broadCmdBuf); i++)
                        {
                            tmpBroadCmd.broadCmdBuf[i] = 0;
                        }
                    }

                    /* ���һ���㲥����Ͷ�λ���� */
                    addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);
                    
                    /* �洢���͸�DCU�ĳ��Ŷ�λ����վ̨����Ϣ����MMS��ѯʱ�ظ� */
                    memcpy((int8u*)&uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[0], (int8u*)tmpBroadCmd.broadCmdBuf, sizeof(tmpBroadCmd.broadCmdBuf));
                    
         
                    #endif
                }
            }
            break;

        #if (CMD_INHIBIT_BY_MMS != 0)
        case 0x11:                      /*inhibit doors*/
        {
            if(bUART2_InhibitDoor)
                break;

            /* ʶ�����������Ӧ��վ̨ 
             * ����RX_MMS_INHIBIT_MESSAGE.ID
             */
            //if(mmsRxBufPtr[8 + 7] == getPedcID())
            {
                int8u cmdSrc;

                pStr1 = (pINT8U) &(mmsRxBufPtr[8]);
                pStr2 = (pINT8U) &RX_MMS_INHIBIT_MESSAGE;

                for(i = 0; i < sizeof(RX_MMS_INHIBIT_MESSAGE); i ++)
                {/*�յ�INHIBIT��Ϣ*/
                    *pStr2 ++ = *pStr1 ++;
                }

                bUART2_InhibitDoor = 1;

                pStr1 = (pINT8U) &RX_MMS_INHIBIT_MESSAGE; 
                pStr2 = (pINT8U) &TX_MMS_INHIBIT_MESSAGE; 
                for(i = 0; i < sizeof(TX_MMS_INHIBIT_MESSAGE); i ++)
                {/*�յ�INHIBIT��Ϣ*/
                    *pStr2 ++ = *pStr1 ++;
                }

                /* ������ID,255��ʶΪMMS���ط���������ΪԶ�̿ͻ��˷��� */
                //cmdSrc = 0x11;
                cmdSrc = RX_MMS_INHIBIT_MESSAGE.source_inhibit;
                if(RX_MMS_INHIBIT_MESSAGE.source_inhibit == 255)
                {
                    cmdSrc = BROAD_CMD_MMS_INHIBIT;
                }
                else
                {
                    cmdSrc = BROAD_CMD_STC_INHIBIT;
                }

                tBroadCmdPkg tmpBroadCmd;
                pStr1 = (pINT8U) &tmpBroadCmd.broadCmdBuf[0];
                pStr2 = (pINT8U) &RX_MMS_INHIBIT_MESSAGE.inhibit_buf[0]; 
                for(i = 0; i < BROADCOMMAND_LEN; i ++)
                {
                    *pStr1 ++  = *pStr2 ++; 
                }

                
                if(cmdSrc == BROAD_CMD_MMS_INHIBIT)
                { 
                    /* MMS inhibit */
                    if(RX_MMS_INHIBIT_MESSAGE.operation == COPR_inhibit)
                   {
                       /*1:��ֹ����*/
                       tmpBroadCmd.oprCmd1.byte = MMS_INHIBIT_ENABLE;
                   }    
                   else
                   {
                       /*0:ȡ����ֹ����*/
                       tmpBroadCmd.oprCmd1.byte = MMS_INHIBIT_DISABLE;
                   }    
                }
                else
                {
                    /* Զ�̿ͻ��� inhibit */
                    tmpBroadCmd.oprCmd1.byte = STC_INHIBIT_ENABLE;
                }
                tmpBroadCmd.oprCmd2.byte = 0;
                tmpBroadCmd.oprCmd3.byte = 0;

                addBroadCmdTask(&tmpBroadCmd, cmdSrc);

                bUART2_InhibitDoor = 1;                 /**/
            }
        }
        break;
        #endif

        #ifdef MMS_WINDVALVE_CTRL
        /* ========== �緧���ؿ������� ========== */
        case 0x12:                      
        {
            int8u ctrlCmd;
            int8u sendDcu = 0xff;
            ctrlCmd = mmsRxBufPtr[8] & 7;
            tWindValveSysSts * pWVSts = getWindValveSysSts();

            /* bit2=1������bit1=1�ط���bit0=1MMSʹ�� */
            if(ctrlCmd == 0b101)
            {
                /* ���� */
                sendDcu =  DCUCMD1_OPN | OP_DCUCMD_1 | OP_BIT_EXCLUDE | CMD_BIT_SET | OPCMD_VALID;
                pWVSts->bMMSOpnCmd_sts = 1;
                pWVSts->bMMSClsCmd_sts = 0;
                pWVSts->bWindCtrlBy_MMS = 1;
            }
            else if(ctrlCmd == 0b011)
            {
                /* �ط� */
                sendDcu =  DCUCMD1_CLS | OP_DCUCMD_1 | OP_BIT_EXCLUDE | CMD_BIT_SET | OPCMD_VALID;
                pWVSts->bMMSClsCmd_sts = 1;
                pWVSts->bMMSOpnCmd_sts = 0;
                pWVSts->bWindCtrlBy_MMS = 1;
            }
            else
            {
                /* ��ֹMMS */
                pWVSts->bWindCtrlBy_MMS  = 0;
                pWVSts->bMMSOpnCmd_sts = 0;
                pWVSts->bMMSClsCmd_sts = 0;
            }

            /* ��Ҫ����������� DCU ? */
            if(pWVSts->bWindCtrlBy_MMS != 0)
            {
                tBroadCmdPkg tmpBroadCmd;

                /* �緧��������� DCU ��Ч */
                for(i=0; i<BROADCOMMAND_LEN; i++)
                {
                    tmpBroadCmd.broadCmdBuf[i] = 0xff;
                }
                tmpBroadCmd.oprCmd1.byte = sendDcu;
                tmpBroadCmd.oprCmd2.byte = 0;
                tmpBroadCmd.oprCmd3.byte = 0;

                addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
            }
        }
        break;
        #endif

        case 0x20:                              //�㲥��������
            #if 1                               /* �����۵� */
            if((bFun_Up_MoreDcuParameter == 1) || (bUART2_UpMoreDcuParaOK == 1))
                break;                               //�ϴ�û��ɣ��ô��������

            tagUpDcuParameter.aUpDcuParameter_DcuNum = 0;

            for(i = 0; i < defPARAMETER_FLAG_LEN; i ++)
            {
                tagUpDcuParameter.aUpDcuParameterRequest[i] = mmsRxBufPtr[8+i];

                tagUpDcuParameter.aUpDcuParameterResult[i] = 0;
            }

            ptr1 = (pINT8U)& tagUpDcuParameter.aAllDcuPara[0][0];

            for(i = 0; i < ALL_DCU_PARA_LEN; i ++)
            {
                *ptr1 ++ = 0;
            }

            tagUpDcuParameter.uUpDcuParameter_IdDcu = ID_DCU_MIN - 1;

            if(FindNextDcu((pINT8U) &tagUpDcuParameter.aUpDcuParameterRequest, (pINT8U) &tagUpDcuParameter.uUpDcuParameter_IdDcu ,MAX_DCU) == FindOk)
            {
                bFun_Up_MoreDcuParameter = 1;           //�㲥��������
                Build_UpMoreDcuPara(tagUpDcuParameter.uUpDcuParameter_IdDcu);
            }
            else
            {
                bFun_Up_MoreDcuParameter = 0;           //�㲥��������
                bUART2_UpMoreDcuParaOK = 1;
            }
            #endif
            break;

        case 0x25:                              //�㲥��������
            #if 1                               /* �����۵� */
            if((bFun_Down_MoreDcuParameter == 1) || (bUART2_DownMoreDcuParaOK == 1))
                break;                                    //�ϴ�û��ɣ��ô��������

            for(i = 0; i < defPARAMETER_FLAG_LEN; i ++)
            {
                tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterRequest[i] = mmsRxBufPtr[8+i];
                tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterResult[i] = 0;
            }

            for(i = 0; i < ONE_DCU_PARA_LEN; i ++)
            {
                tagDOWN_MORE_DCU_PARAMETER.aDownParameterBuf[i] = mmsRxBufPtr[8+defPARAMETER_FLAG_LEN+i];
            }

            tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu = ID_DCU_MIN - 1;

            if(FindNextDcu((pINT8U) &tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterRequest, (pINT8U) &tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu ,MAX_DCU) == FindOk)
            {
                bFun_Down_MoreDcuParameter = 1;       //�㲥��������
                Build_DownMoreDcuPara(tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu);
            }
            else
            {
                bFun_Down_MoreDcuParameter = 0;
                bUART2_DownMoreDcuParaOK = 1;

            }
            #endif
            break;
            
        /* ========================== 
         * 2022-4-8 ���� :  ������ָ��DCU�� IP ����
         * 0x21:DCU Paras Read Cmd(Query) */
        case 0x21:
            #if 1                               /* �����۵� */
            if(uploadDCU_IPPara.uploadTask != IPPARA_TASK_IDLE)
            {
                break;
            }
            
            /* ��ȡ��ѯDCU�����б� */
            for(i=0; i<sizeof(uploadDCU_IPPara.uploadDCUObjs); i++)
            {
                uploadDCU_IPPara.uploadDCUObjs[i]   = mmsRxBufPtr[8+i];
            }
            uploadDCU_IPPara.uploadTask = IPPARA_TASK_START;
            #endif
            break;
            
        /* ========================== 
         * 2022-4-8 ���� :  �´� IP ������DCU
         * 0x26:DCU Paras Write Cmd(Query) */
        case 0x26:
            #if 1                               /* �����۵� */
            //0��ʾδִ��
            if(DownloadDCU_IPPara.downloadTask != IPPARA_TASK_IDLE)
            {   
                break;
            }
            
            ptrData = &mmsRxBufPtr[8];
            dataLen = ((int16u)mmsRxBufPtr[6] << 8) + (int16u)mmsRxBufPtr[7];
            
            if( dataLen < sizeof(DownloadDCU_IPPara.dcuObjects) + ONE_DCU_IPPARA_BUFLEN )
            {
                break;
            }
    
            //�洢�����ص�DCU IP����
            for(i=0; i<sizeof(DownloadDCU_IPPara.dcuObjects); i++)
            {
                DownloadDCU_IPPara.dcuObjects[i]   = *ptrData++;
            }
            dataLen -= sizeof(DownloadDCU_IPPara.dcuObjects);
            
            //�洢IP���� - Server IP & Port
            for(i = 0; i < DCU_SERVERIP_LEN; i ++)
            {
                DownloadDCU_IPPara.ServerIPPort[i] = *ptrData++;
            }
            dataLen -= DCU_SERVERIP_LEN;
            
            //�洢IP���� - ����DCU�� IP & Token
            pStr1 = &DownloadDCU_IPPara.dcuIPToken[0][0];
            for(i = 0; i < dataLen; i ++)
            {
                *pStr1++ = *ptrData++;
            }
            
            //������������
            DownloadDCU_IPPara.downloadTask = IPPARA_TASK_START;
            #endif
            break;

        /* ========================== 
         * 2023-8-21 ���� :  
         * 0x23::Get DCU Total Trip(Query) */
        case 0x23:
            #if 1                               /* �����۵� */
            if(uploadDCU_Trip.uploadTask != IPPARA_TASK_IDLE)
            {
                break;
            }
            
            /* ��ȡ��ѯDCU�����б� */
            for(i=0; i<sizeof(uploadDCU_Trip.uploadDCUObjs); i++)
            {
                uploadDCU_Trip.uploadDCUObjs[i]   = mmsRxBufPtr[8+i];
            }
            uploadDCU_Trip.uploadTask = IPPARA_TASK_START;
            #endif
            break;
            
        /* ========================== 
         * 2023-8-21 ���� :  
         * 0x28:Set DCU Total T &Hrip(Query) */
        case 0x28:
            #if 1                               /* �����۵� */
            //0��ʾδִ��
            if(DownloadDCU_Trip.downloadTask != IPPARA_TASK_IDLE)
            {   
                break;
            }
            
            ptrData = &mmsRxBufPtr[8];
            dataLen = ((int16u)mmsRxBufPtr[6] << 8) + (int16u)mmsRxBufPtr[7];
            
            if( dataLen < sizeof(DownloadDCU_Trip.dcuObjects) + DCU_TRIPDATA_LEN )
            {
                break;
            }
    
            //�洢�����ص�DCU IP����
            for(i=0; i<sizeof(DownloadDCU_Trip.dcuObjects); i++)
            {
                DownloadDCU_Trip.dcuObjects[i]   = *ptrData++;
            }
            dataLen -= sizeof(DownloadDCU_Trip.dcuObjects);
            
            //�ݴ���Ҫ���͸� DCU���������
            pStr1 = &DownloadDCU_Trip.dcuTripData[0];
            for(i = 0; i < dataLen; i ++)
            {
                *pStr1++ = *ptrData++;
            }
            
            //������������
            DownloadDCU_Trip.downloadTask = IPPARA_TASK_START;
            #endif
            break;
            
        case 0x30:                                     //�����������ݻ�ȡ
            if((bFun_Up_OpenDoorCurve == 1) || (bUART2_UpOpenDoorCurveOK == 1) || (bUART2_UpOpenDoorCurveFail == 1))
                break;                                   //�ϴ�û��ɣ��ô��������


            tagUPOPENDOORCURVE.uUpOpenDoorCurve_IdDcu = mmsRxBufPtr[4];

            if(DcuIfOnLine(tagUPOPENDOORCURVE.uUpOpenDoorCurve_IdDcu) == cDcuOnLineF)
            {
                bFun_Up_OpenDoorCurve = 1;
                Build_UpOpenDoorCurve(tagUPOPENDOORCURVE.uUpOpenDoorCurve_IdDcu);
            }
            else
            {
                bUART2_UpOpenDoorCurveFail = 1;
            }

            break;

        case 0x31:                                      //�����������ݻ�ȡ
            if((bFun_Up_CloseDoorCurve == 1) || (bUART2_UpCloseDoorCurveOK == 1) || (bUART2_UpCloseDoorCurveFail == 1))
                break;                                    //�ϴ�û��ɣ��ô��������


            tagUPCLOSEDOORCURVE.uUpCloseDoorCurve_IdDcu = mmsRxBufPtr[4];

            if(DcuIfOnLine(tagUPCLOSEDOORCURVE.uUpCloseDoorCurve_IdDcu) == cDcuOnLineF)
            {
                bFun_Up_CloseDoorCurve = 1;
                Build_UpCloseDoorCurve(tagUPCLOSEDOORCURVE.uUpCloseDoorCurve_IdDcu);
            }
            else
            {
                bUART2_UpCloseDoorCurveFail = 1;
            }
            break;

        case 0x50:
            if(bUART2_Up_IbpAlarmParameter == 1)
                break;
            bUART2_Up_IbpAlarmParameter = 1;          //ibp���������ϴ�

            break;

        case 0x51:                                   //ibp����������������
            if(bUART2_Down_IbpAlarmParameter == 1)
                break;

            bUART2_Down_IbpAlarmParameter = 1;        //ibp����������������

            pStr1 = (pINT8U)&tagDownIbpAlarmParameter.uDOWN_IbpAlarmParameter;
            pStr2 = (pINT8U)(&mmsRxBufPtr[8]);

            for(i = 0; i < IBPALARM_LEN; i ++)
            {
                *pStr1 = *pStr2;

                pStr1++;
                pStr2++;

            }

            break;

        case 0x52:
            bFun_UpPEDCHardwareSetting = 1;          //�ϴ�

            break;

        case 0x53:
            if(bFun_DownPEDCHardwareSetting == 1)
                break;

            bFun_DownPEDCHardwareSetting = 1;        //����

            for(i = 0; i < DCUsetting_LEN; i ++)
            {
                uDCUsetting.DownsettingBUF[i] = mmsRxBufPtr[8+i];
            }

            break;

        /* �������ݰ��Ĵ��� */
        case 0xE0:
                MMS_Command(mmsRxBufPtr, mmsRxLen);
            break;
            
        /* ��ѯ�豸�����Ϣ */
        case 0xE5:
            if((bFun_QUERY == 1) || (bUART2_QueryOK == 1) || (bUART2_QueryFAIL == 1))
                break;                               //�ϴ�û��ɣ��ô��������

            takQUERY.uID = mmsRxBufPtr[4];          //��ѯ����
            
            int8u queryObj;
            queryObj = takQUERY.uID ;

            /* ��ѯ IBP PSL SIG�� */
            if ((queryObj == COMMOBJ_SWITCHER) || (queryObj == COMMOBJ_LOG1) || (queryObj == COMMOBJ_LOG2) ||
                (queryObj == COMMOBJ_IORW_A) || (queryObj == COMMOBJ_IORW_B))
            {
                /* if(IPSObj_chkOnline(queryObj) == TRUE) */
                if(getObjCommLinkSts(queryObj) == TRUE)
                {
                    //�豸���ߣ��ж��Ƿ��Ѿ���ȡ���豸��Ϣ
                    tSlaveInfo* pSlvInfo = getSlaveInfoPtr((tEnmCommObj) queryObj);
                    
                    if(pSlvInfo->VersionFlag)
                    {
                        memcpy( DeviceInfo, (int8u *)(pSlvInfo->slaveDevInfo),BYTES_INFO);
                        bUART2_QueryOK = 1;
                    }
                    else
                    {
                        bUART2_QueryFAIL = 1;
                    }
                }
                else
                {
                    bUART2_QueryFAIL = 1;
                }
            }
            /* ��ѯPEDC - MONITOR PIC32 */
            else if(queryObj == COMMOBJ_MONITOR) 
            {
                ApiGetDevieceInfo();
                bUART2_QueryOK = 1;
            }
            /* ��ѯDCU 
             */
            else
            {                
                if(DcuIfOnLine(queryObj) == cDcuOnLineF)
                {                                        //����
                    bFun_QUERY = 1;
                    Build_QUERY(queryObj);
                }
                else
                {                                        //������
                    bUART2_QueryFAIL = 1;
                }
            }
            break;

        /* �����Զ����Թ���
         * ���߲����߼������������߼�
         * ת���� �߼��� */
        case OBJ_CMD_INI_TEST:      /* �����Զ����Թ��� */
        case 0xAF:  /* ���ô��������ʾ����߼�ʹ�� */
        case 0xAE:  /* ���ô��ڷ���ģ�� IO �ź� */
        case 0xA0:  /* ���� */
        case 0xA1:  /* ����ָ���Ŀ����߼� */
        case 0xA2:  /* ����ָ���Ŀ����߼��������źţ���������) */
        case 0xA3:  /* ����ָ���Ŀ����߼��������źţ���������) */
        case 0xA4:  /* �����߼����������һ���ֽڶ�Ӧһ���߼������������˳���� */
        {
            int8u *p;
            tPktHeader sendPkgHeader;
            p = (int8u*)&sendPkgHeader;
            for(i=0; i<sizeof(sendPkgHeader); i++)
            {
                *p++ = mmsRxBufPtr[i];
            }
            sendPkgHeader.dstNode = COMMOBJ_LOG1;
            createTx2LogPackage(&sendPkgHeader, ptrData, dataLen, TRUE);
            
            setDoTestFunct(TRUE);
        }
        break;
        
        case MMS_CMD_TEST:                              //���Ͳ��Դ���
            if((mmsRxBufPtr[UARTDATASEAT + 0] == TEST_CODE_AA) && (mmsRxBufPtr[UARTDATASEAT + 1] == TEST_CODE_BB) \
              && (mmsRxBufPtr[UARTDATASEAT + 2] == TEST_CODE_CC) && (mmsRxBufPtr[UARTDATASEAT + 3] == TEST_CODE_DD))
            {
                setDoTestFunct(TRUE);
                setDoTestFunctDCUNo(mmsRxBufPtr[UARTDATASEAT + 4]);
                
                #if 0
                /* ���ڲ��Ե�����
                 * ���ֽ���ǰ */
                int8u *pReg;

                pReg = &(mmsRxBufPtr[UARTDATASEAT + 4]);
                #endif

            }
            break;

        case MMS_CMD_TEST_OUT:                          //�������
            if((mmsRxBufPtr[UARTDATASEAT + 0] == TEST_CODE_AA) && (mmsRxBufPtr[UARTDATASEAT + 1] == TEST_CODE_BB))
            {
                /* ������� IO ����Чʱ�� */
                int testOutputMs;
                testOutputMs = mmsRxBufPtr[UARTDATASEAT + 2] * 256 + mmsRxBufPtr[UARTDATASEAT + 3];
                
                /* ������� IO ������     */
                int8u* pcData = &mmsRxBufPtr[UARTDATASEAT + 4];
                
                /* ���������� IO ������ */
                setPic24TestOutputData(pcData, OUTPUT_DATA_LEN, testOutputMs);
            }

            break;
            
        /* BAS �ӿڲ��� */
        case MMS_CMD_TEST_BASREG:
        {
            if((mmsRxBufPtr[UARTDATASEAT + 0] == TEST_CODE_AA) && (mmsRxBufPtr[UARTDATASEAT + 1] == TEST_CODE_BB))
            {
                setDoTestFunct(TRUE);
                
                /* ���յ������ǵ��ֽ���ǰ
                 * 2Bytes ʱ�� + �Ĵ���ֵ
                 *  */
                int8u *pReg;
                pReg = &mmsRxBufPtr[UARTDATASEAT + 2];
                setTestBASRegValue(pReg);
            }
        }
        break;

        #if (PIC24_BL_ENABLE == 1)
        /* ���� PIC24 Bootloader ���� PIC24  */
        case OBJ_CMD_BL_CTRL:         
            if((mmsRxBufPtr[UARTDATASEAT + 0] == PIC24APP_CMD_ENTER_BL) && 
               (mmsRxBufPtr[UARTDATASEAT + 1] == 0x55) &&
               (mmsRxBufPtr[UARTDATASEAT + 2] == 0xAA))
            {
                tFlashInfo info;
                if(mmsRxBufPtr[UARTDATASEAT + 3] == UPMODE_BYCMD)
                {
                    memcpy((int8u*)&info, &mmsRxBufPtr[UARTDATASEAT + 3], sizeof(tFlashInfo));
                }
                
                upPIC24_InitStart(OBJECT_MMS, mmsRxBufPtr[UARTDATASEAT + 3], &info);
            }
            break;
        #endif
            

        default:
            ret = FALSE;
            break;
    }
    
    return ret;
}

/* =============================================================================
 * ���������񣺷������ݸ� MMS (Uart_MMS_Tx_Handle)
 * ������� ��ѯ UART2 tx ,�Ƿ���Ҫ tx ,�����Ҫ ���ж� tx �Ƿ� ���� 
 */
PRIVATE void  MMS_ProcTxFunct(void)
{
    int8u i, j;
    pINT8U pStr1, pStr2;
    static int8u UART_MMS_TX_No;                      //������� ��ѯ UART2 tx ,�Ƿ���Ҫ tx ,�����Ҫ ���ж� tx �Ƿ� ���� ��

    if(ifMMSUartTxIsBusy() == TRUE)
        return;
    
    
    #if (PIC24_BL_ENABLE == 1)
    /* ���� PIC24 Bootloader ���� PIC24  
     * ��ʱ PIC32 ������Ӧ MMS �������
     * �ظ� MMS ����Ϣ��PIC24Update.c ͨ���ص� upPIC24_SendMsgToMMS ���
     */
    if(chkIsUpdatingPic24() == TRUE)
    {
        return;
    }
    #endif
    
    /* ��������£��ظ� MMS */
    for(i = 0; i < cUART_MMS_TX_NUM; i ++)
    {
        j = 1;

        UART_MMS_TX_No ++;
        if(UART_MMS_TX_No >= cUART_MMS_TX_NUM)
        {
            UART_MMS_TX_No = 0;
        }

        switch(UART_MMS_TX_No)
        {
            case  0:
                if(bUART2_CrcErr == 1)
                {
                    bUART2_CrcErr = 0;
                    //CrcErr();
                    j = 0;
                }
                break;

            case  1: //�ϴ�MMS ����״̬����
                if(bFun_Up_WorkState == 1)
                {
                    //��� Ҫ�󴫵�DCU λ�ú� FLASH����Ĳ�һ�£������ö�Ӧ�Ĵ��󱨾�λ

                    Up_WorkState();
                    bFun_Up_WorkState = 0;
                    sf_flash_toMMS    = 0;

                    bClsAvrDataBuf = 1; 

                    j = 0;
                }
                break;

            case  2:
                if(bUART2_UpMoreDcuParaOK == 1)
                {
                    UpMoreDcuParameter();
                    bUART2_UpMoreDcuParaOK = 0;
                    j = 0;
                }
                break;
            case  3:
                if(bUART2_DownMoreDcuParaOK == 1)
                {
                    DownMoreDcuParameter();
                    bUART2_DownMoreDcuParaOK = 0;
                    j = 0;
                }
                break;
            case  4:
                if(bUART2_UpOpenDoorCurveOK == 1)
                {
                    UpOpenDoorCurveOK();
                    bUART2_UpOpenDoorCurveOK = 0;
                    j = 0;
                }

                break;

            case  5:
                if(bUART2_UpOpenDoorCurveFail == 1)
                {
                    UpOpenDoorCurveFAIL();
                    bUART2_UpOpenDoorCurveFail = 0;
                    j = 0;
                }

                break;
            case  6:
                if(bUART2_UpCloseDoorCurveOK == 1)
                {
                    UpCloseDoorCurveOK();
                    bUART2_UpCloseDoorCurveOK = 0;
                    j = 0;
                }

                break;
            case  7:
                if(bUART2_UpCloseDoorCurveFail == 1)
                {
                    UpCloseDoorCurveFAIL();
                    bUART2_UpCloseDoorCurveFail = 0;
                    j = 0;
                }
                break;

            case 8:
                if(bUART2_Up_IbpAlarmParameter == 1)
                {
                    Read_IbpAlarmX_From_Flash();
                    Up_IbpAlarmParameter();
                    bUART2_Up_IbpAlarmParameter = 0;
                    j = 0;
                }

                break;

            case 9:
                if(bUART2_Down_IbpAlarmParameter == 1)
                {
                    #ifdef NO_CAN_BUS2
                    if(tagDownIbpAlarmParameter.uDOWN_IbpAlarmParameter & 1) //��������(0����(Ĭ��)��1˫������)
                    {
                        //�ܽ�=2,Success=1,Fail=0
                        Down_IbpAlarmParameter(2);
                    }
                    #else
                    if(0)
                    {
                    }
                    #endif

                    else if(write_IbpAlarmX_To_Flash() == OK)
                    {
                    //�ܽ�=2,Success=1,Fail=0
                        Down_IbpAlarmParameter(1);

                        tagDownIbpAlarmParameter.uIbpAlarmParameter.byte = tagDownIbpAlarmParameter.uDOWN_IbpAlarmParameter;

                        pStr1 = (pINT8U)&tagDownIbpAlarmParameter.uRead_IbpAlarmParameter;
                        pStr2 = (pINT8U)&tagDownIbpAlarmParameter.uDOWN_IbpAlarmParameter;

                        int8u kkk;
                        for(kkk = 0; kkk < IBPALARM_LEN;  kkk++)
                        {
                            *pStr1 = *pStr2;

                            pStr1++;
                            pStr2++;

                        }

                        /* ���ڷ緧ȫ����ȫ�صĲ��� */
                        setWVOpnClsPara(tagDownIbpAlarmParameter.uRead_WVTotalNum, \
                                        tagDownIbpAlarmParameter.uRead_WVNum_Opn,  \
                                        tagDownIbpAlarmParameter.uRead_WVNum_Cls);
                    }

                    else
                    {
                        //�ܽ�=2,Success=1,Fail=0
                        Down_IbpAlarmParameter(0);
                    }

                    bUART2_Down_IbpAlarmParameter = 0;
                    j = 0;
                }
                break;

            case 10:
                if(bUART2_UPDATE_FIRMWARE == 1)
                {
                    UPDATE_FIRMWAREStateToMMS();
                    bUART2_UPDATE_FIRMWARE = 0;
                    j = 0;
                }
                break;
            case 11:
                if(bUART2_QueryOK == 1)
                {
                    QueryOK();
                    bUART2_QueryOK = 0;
                    j = 0;
                }
                break;

            case 12:
                if(bUART2_QueryFAIL == 1)
                {
                    QueryFAIL();
                    bUART2_QueryFAIL = 0;
                    j = 0;
                }
                break;

            case 13:
                if(bFun_UpPEDCHardwareSetting == 1)
                {
                    Up_PEDCHardwareSetting();

                    bFun_UpPEDCHardwareSetting = 0;
                    j = 0;
                }

                break;

            case 14:
                if(bFun_DownPEDCHardwareSetting == 1)
                {
                    int8u write = 0;
                    if(Read_DCUsetting_From_Flash() == OK)
                    {
                        for(i = 0; i < DCUsetting_LEN; i ++)  //�Ƚ�FLASH ��ȡ�� �� ���صģ����һ�£����ñ���
                        {
                            if(uDCUsetting.ReadSettingBUF[i] != uDCUsetting.DownsettingBUF[i])
                            {
                                write = 1;
                                break;
                            }
                        }
                    }
                    else
                    {
                        write = 1;
                    }

                    uDCUsetting.DownFlag = 1;

                    if(write == 1)
                    {
                        if(write_DCUsetting_To_Flash() == OK)
                        {
                            for(i = 0; i < DCUsetting_LEN; i ++)  //��FLASH ��ȡ�� ���ø�ֵ�� settingBUF
                            {
                                uDCUsetting.ReadSettingBUF[i] = uDCUsetting.DownsettingBUF[i];
                            }
                        }
                        else
                        {
                            uDCUsetting.DownFlag = 0;
                        }

                    }

                    Down_PEDCHardwareSetting();

                    bFun_DownPEDCHardwareSetting = 0;
                    j = 0;
                }
                break;

            /*========== &11:Inhibit Doors Reply ==========*/
            case 15:
                if(bUART2_InhibitDoor)
                {   
                    Up_MMS_inhibit();
                    bUART2_InhibitDoor = 0;
                    j = 0;
                }   
                break;  

            /*========== 2022-4-8 ���� ==========*/
            /* �ϴ�DCU IP������������Ժ󣬻ظ� MMS */
            case 16:
                if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_FINISHED)
                {
                    uSource = SOURE_BROADCAST;
                    uObject = OBJECT_MMS;
                    readyPkgToMMS(cUP_UPLOAD_IPPARA_NO, StartUart2);
                    
                    uploadDCU_IPPara.uploadTask = IPPARA_TASK_IDLE;
                    j = 0;
                }
                break;
                
            /*========== 2022-4-8 ���� ==========*/
            /* �´�DCU IP������������Ժ󣬻ظ� MMS */
            case 17:
                if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_FINISHED)
                {
                    uSource = SOURE_BROADCAST;
                    uObject = OBJECT_MMS;
                    
                    readyPkgToMMS(cUP_DOWNLOAD_IPPARA_NO, StartUart2);
                    DownloadDCU_IPPara.downloadTask = IPPARA_TASK_IDLE;
                    j = 0;
                }
                break;
                
            /* 2023-8-21 �ظ� �ϴ� DCU ������� */
            case cUP_UPLOAD_DCUTRIP_NO:
                if(uploadDCU_Trip.uploadTask == IPPARA_TASK_FINISHED)
                {
                    uSource = SOURE_BROADCAST;
                    uObject = OBJECT_MMS;
                    readyPkgToMMS(cUP_UPLOAD_DCUTRIP_NO, StartUart2);
                    
                    uploadDCU_Trip.uploadTask = IPPARA_TASK_IDLE;
                    j = 0;
                }
                break;
                
            /* 2023-8-21 �ظ� ���� DCU ������� */
            case cUP_DOWNLOAD_DCUTRIP_NO:
                if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_FINISHED)
                {
                    uSource = SOURE_BROADCAST;
                    uObject = OBJECT_MMS;
                    
                    readyPkgToMMS(cUP_DOWNLOAD_DCUTRIP_NO, StartUart2);
                    DownloadDCU_Trip.downloadTask = IPPARA_TASK_IDLE;
                    j = 0;
                }
                break;
                
                
            /*========== ����ֵ���� ==========*/
            default:
                j = 0;
                break;
        }

        if(j == 0)
            break;
    }
}


/* =============================================================================
 * ʵ�� MMS ģ��Ĺ���
 * ��ѭ������
 *  */
PUBLIC void MMS_AppModule(void)
{
    static tSYSTICK chkLinkSts;
    tSYSTICK tickNow;
    
    tickNow = getSysTick();
    
    //����MMS ���͵���Ϣ
    MMS_ProcRxFunct(&MMS_AppRxMsg);     
    
    //������� ��ѯ UART2 tx ,�Ƿ���Ҫ tx ,�����Ҫ ���ж� tx �Ƿ� ���� ��
    MMS_ProcTxFunct();                  
    
    //���Դ���,��PC����
    testIO();        
    
    /* mms -- pedc ͨѶʧ�� ? */
    /* �жϴ�������״̬������״̬�Ƿ��쳣 
     * ��MMSͨ�ųɹ����򱾻� UART OK
     */
    if((tSYSTICK)(tickNow - chkLinkSts) >= 100)
    {
        chkLinkSts = tickNow;
        
        if(uartChkLinkAndSendSts(COM_MMS) == TRUE)
        {
            bClsAvrDataBuf = 1;
            
            #if  (CMD_INHIBIT_BY_ISCS != 0)
            /* PEDC -- MMS ͨѶ�Ͽ���Ҫȡ�� DCU�� �ź�ϵͳ��λ����
             *  */            
            int i;
            tBroadCmdPkg tmpBroadCmd;

            tmpBroadCmd.oprCmd1.byte = STC_INHIBIT_ENABLE;
            tmpBroadCmd.oprCmd2.byte = 0;
            tmpBroadCmd.oprCmd3.byte = 0;
            
            for(i = 0; i<sizeof(tmpBroadCmd.broadCmdBuf); i++)
            {
                tmpBroadCmd.broadCmdBuf[i] = 0;
            }

            /* ���һ���㲥����Ͷ�λ���� */
            addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);

            /* �洢���͸�DCU�ĳ��Ŷ�λ����վ̨����Ϣ����MMS��ѯʱ�ظ� */
            memcpy((int8u*)&uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[0], (int8u*)tmpBroadCmd.broadCmdBuf, sizeof(tmpBroadCmd.broadCmdBuf));
            #endif
        }
        
        /* ÿ��һ��Сʱ��ͨ������ϵͳʱ��� DCU ( ǰ�������ǣ��������յ� MMS ����ʱ�� )
         *  ������ FUNID_PEDC_SYSTIME�� �������ɹ㲥�������DCU�����봦��
         *  */
        if(ifPEDCSysTimeValid() == TRUE)
        {
            extern int8u  sendDCUSysTime[];
            
            tSysTime tmNow;
            static int8u prevTim = 0xff;
            static int8u sendSysTimeCnt = 0;
            static tSYSTICK prevTick = 0;
            
            getPedcSysTime(&tmNow);
            sendDCUSysTime[0] = (int8u)(tmNow.year >> 8);
            sendDCUSysTime[1] = (int8u)(tmNow.year >> 0);
            sendDCUSysTime[2] = tmNow.month;
            sendDCUSysTime[3] = tmNow.day;
            sendDCUSysTime[4] = tmNow.hour;
            sendDCUSysTime[5] = tmNow.min;
            sendDCUSysTime[6] = tmNow.second;
            
            if(prevTim != tmNow.hour)
            {
                prevTim = tmNow.hour;
                
                /* ��������ϵͳʱ��� DCU */
                sendSysTimeCnt = 3;
                prevTick = getSysTick();
            }
            
            /* ����3�Σ���� 500ms */
            if(sendSysTimeCnt > 0)
            {
                if((tSYSTICK)(getSysTick() - prevTick) >= 500)
                {
                    prevTick = getSysTick();

                    /* ����ϵͳʱ��� DCU */
                    wtj_CapiStartTrans(FUNID_PEDC_SYSTIME, 0);
                    sendSysTimeCnt--;
                }
            }
        }
    }
}



