
#include    "..\include.h"
#include <string.h>
//#include "..\canbus\drv\UpEEPROM.h"

/*
int16u _EEDATA(2)  EE_UpDownLinkState[IBPALARM_BUFLEN] =
{
    0x71,0x72,0x73,0x74
};
*/

//void WriteUpDownLink(void)
//{
//  //save to eeprom
//  fnEE_EraseData(EE_UpDownLinkState,sizeof(EE_UpDownLinkState)/2);
//  fnEE_WriteData(EE_UpDownLinkState,&awWriteIBPALARMBuf,sizeof(EE_UpDownLinkState)/2);
//  }
//void ReadUpDownLink(void)
//{
//  fnEE_ReadData(EE_UpDownLinkState,&awReadIBPALARMBuf,sizeof(EE_UpDownLinkState)/2);
//}


//**********************************************************************************
int8u write_IbpAlarmX_To_Flash(void)
{
    int8u i;
    int8u awWriteIBPALARMBuf[IBPALARM_BUFLEN];

    int8u SW_DATA_BUF[SW_DATA_SIZE * 2 + 1];   //+1 预留

    if(NVMIsError())
    {
        NVMClearError();
    }
    memcpy((void*)SW_DATA_BUF, (void*)SW_DATA_ADDR, SW_DATA_SIZE * 2);

    awWriteIBPALARMBuf[0] = (int8u) tagDownIbpAlarmParameter.uDOWN_IbpAlarmParameter;
    awWriteIBPALARMBuf[1] = (int8u) tagDownIbpAlarmParameter.uDOWN_WVTotalNum;
    awWriteIBPALARMBuf[2] = (int8u) tagDownIbpAlarmParameter.uDOWN_On_Hour;   
    awWriteIBPALARMBuf[3] = (int8u) tagDownIbpAlarmParameter.uDOWN_On_Minute; 
    awWriteIBPALARMBuf[4] = (int8u) tagDownIbpAlarmParameter.uDOWN_Off_Hour;  
    awWriteIBPALARMBuf[5] = (int8u) tagDownIbpAlarmParameter.uDOWN_Off_Minute;
    awWriteIBPALARMBuf[6] = (int8u) tagDownIbpAlarmParameter.uDOWN_WVNum_Opn;
    awWriteIBPALARMBuf[7] = (int8u) tagDownIbpAlarmParameter.uDOWN_WVNum_Cls;

    awWriteIBPALARMBuf[IBPALARM_BUFLEN-4] = 0;
    awWriteIBPALARMBuf[IBPALARM_BUFLEN-3] = IBPALARM_0XAA;
    awWriteIBPALARMBuf[IBPALARM_BUFLEN-2] = IBPALARM_0X55;
    awWriteIBPALARMBuf[IBPALARM_BUFLEN-1] = awWriteIBPALARMBuf[0] ^ awWriteIBPALARMBuf[1] ^ 0xff;

    for(i = 0; i < IBPALARM_BUFLEN; i ++)
    {
        SW_DATA_BUF[PARA_START_SEAT + i] = awWriteIBPALARMBuf[i];
    }

    for(i = 0; i < 3; i ++)
    {
         // Erase a page of Program Flash
        NVMErasePage((void *)SW_DATA_ADDR);

        // Write 128 words starting at Row Address NVM_PROGRAM_PAGE
        NVMWriteRow((void *)SW_DATA_ADDR, (void*)SW_DATA_BUF);

        if(memcmp(awWriteIBPALARMBuf, (void *)SW_DATA_ADDR, IBPALARM_BUFLEN) == 0)
        {
            return OK;
        }

    }

    return FAIL;

}


int8u Read_IbpAlarmX_From_Flash(void)
{
    int8u i;
    int8u   awReadIBPALARMBuf[IBPALARM_BUFLEN];

    for(i = 0; i < IBPALARM_BUFLEN; i ++)
    {
        awReadIBPALARMBuf[i] = 0;
    }

    for(i = 0; i < 3; i ++)
    {
        //read
        memcpy((void*)awReadIBPALARMBuf, (void*)SW_DATA_ADDR, IBPALARM_BUFLEN);

        if( (awReadIBPALARMBuf[IBPALARM_BUFLEN-3] == IBPALARM_0XAA) && (awReadIBPALARMBuf[IBPALARM_BUFLEN-2] == IBPALARM_0X55) \
            && (awReadIBPALARMBuf[IBPALARM_BUFLEN-1] == (awReadIBPALARMBuf[0] ^ awReadIBPALARMBuf[1] ^ 0xff)))
        {
            tagDownIbpAlarmParameter.uRead_IbpAlarmParameter = (int8u) awReadIBPALARMBuf[0];
            tagDownIbpAlarmParameter.uRead_WVTotalNum        = (int8u) awReadIBPALARMBuf[1];
            tagDownIbpAlarmParameter.uRead_On_Hour           = (int8u) awReadIBPALARMBuf[2];
            tagDownIbpAlarmParameter.uRead_On_Minute         = (int8u) awReadIBPALARMBuf[3];
            tagDownIbpAlarmParameter.uRead_Off_Hour          = (int8u) awReadIBPALARMBuf[4];
            tagDownIbpAlarmParameter.uRead_Off_Minute        = (int8u) awReadIBPALARMBuf[5];
            tagDownIbpAlarmParameter.uRead_WVNum_Opn         = (int8u) awReadIBPALARMBuf[6];
            tagDownIbpAlarmParameter.uRead_WVNum_Cls         = (int8u) awReadIBPALARMBuf[7];

            tagDownIbpAlarmParameter.uIbpAlarmParameter.byte = tagDownIbpAlarmParameter.uRead_IbpAlarmParameter;
            return OK;
        }
    }

    //默认值
    tagDownIbpAlarmParameter.uRead_IbpAlarmParameter = cDefault_Parameter;
    tagDownIbpAlarmParameter.uRead_WVTotalNum        = cDefault_WVTotal;  
    tagDownIbpAlarmParameter.uRead_On_Hour           = cDefault_On_Hour    ;
    tagDownIbpAlarmParameter.uRead_On_Minute         = cDefault_On_Minute  ;
    tagDownIbpAlarmParameter.uRead_Off_Hour          = cDefault_Off_Hour   ;
    tagDownIbpAlarmParameter.uRead_Off_Minute        = cDefault_Off_Minute ;
    tagDownIbpAlarmParameter.uRead_WVNum_Opn         = cDefault_WVNum_Opned;
    tagDownIbpAlarmParameter.uRead_WVNum_Cls         = cDefault_WVNum_Opned;

    tagDownIbpAlarmParameter.uIbpAlarmParameter.byte = tagDownIbpAlarmParameter.uRead_IbpAlarmParameter;


    tagDownIbpAlarmParameter.uDOWN_On_Hour          = tagDownIbpAlarmParameter.uRead_On_Hour   ;
    tagDownIbpAlarmParameter.uDOWN_On_Minute        = tagDownIbpAlarmParameter.uRead_On_Minute ;
    tagDownIbpAlarmParameter.uDOWN_Off_Hour         = tagDownIbpAlarmParameter.uRead_Off_Hour  ;
    tagDownIbpAlarmParameter.uDOWN_Off_Minute       = tagDownIbpAlarmParameter.uRead_Off_Minute;
    tagDownIbpAlarmParameter.uDOWN_WVTotalNum       = tagDownIbpAlarmParameter.uRead_WVTotalNum;
    tagDownIbpAlarmParameter.uDOWN_WVNum_Opn        = tagDownIbpAlarmParameter.uRead_WVNum_Opn;
    tagDownIbpAlarmParameter.uDOWN_WVNum_Cls        = tagDownIbpAlarmParameter.uRead_WVNum_Cls;

    return FAIL;
}


void reset_Read_IbpAlarmX(void)             //读取保存FLASH 的IBP 参数
{
    Read_IbpAlarmX_From_Flash();

}

//-------------------------------------------------------------------------------------------------------
int8u write_DCUsetting_To_Flash(void)
{
    int8u i,j;
    int8u awWriteDCUsettingBuf[DCUsetting_BUFLEN];

    int8u SW_DATA_BUF[SW_DATA_SIZE * 2 + 1];   //+1 预留

    if(NVMIsError())
    {
        NVMClearError();
    }
    memcpy((void*)SW_DATA_BUF, (void*)SW_DATA_ADDR, SW_DATA_SIZE * 2);

    for(i = 0; i < DCUsetting_BUFLEN; i ++)
    {
        awWriteDCUsettingBuf[i] = uDCUsetting.DownsettingBUF[i];
    }

    awWriteDCUsettingBuf[DCUsetting_CHS_SEAT - DCUsetting_START_SEAT] = 0;
    for(i = 0; i < DCUsetting_LEN; i ++)
    {
        awWriteDCUsettingBuf[DCUsetting_CHS_SEAT - DCUsetting_START_SEAT] ^= awWriteDCUsettingBuf[i];
    }

    awWriteDCUsettingBuf[DCUsetting_0AA_SEAT - DCUsetting_START_SEAT] = IBPALARM_0XAA;
    awWriteDCUsettingBuf[DCUsetting_055_SEAT - DCUsetting_START_SEAT] = IBPALARM_0X55;

    for(i = 0; i < DCUsetting_BUFLEN; i ++)
    {
        SW_DATA_BUF[DCUsetting_START_SEAT + i] = awWriteDCUsettingBuf[i];
    }

    for(i = 0; i < 3; i ++)
    {
         // Erase a page of Program Flash
        NVMErasePage((void *)SW_DATA_ADDR);

        // Write 128 words starting at Row Address NVM_PROGRAM_PAGE
        NVMWriteRow((void *)SW_DATA_ADDR, (void*)SW_DATA_BUF);

            memcpy((void*)SW_DATA_BUF, (void*)SW_DATA_ADDR, SW_DATA_SIZE * 2);

            for(j = 0; j < DCUsetting_BUFLEN; j ++)
            {
                if(awWriteDCUsettingBuf[j] != SW_DATA_BUF[DCUsetting_START_SEAT + j])
                    break;
            }

            if(j == DCUsetting_BUFLEN)
                return OK;

    }

    return FAIL;

}

int8u Read_DCUsetting_From_Flash(void)
{
    int8u i,j;
    int8u   awReadDCUsettingBuf[DCUsetting_BUFLEN];
    int8u SW_DATA_BUF[SW_DATA_SIZE * 2 + 1];   //+1 预留


    for(i = 0; i < DCUsetting_BUFLEN; i ++)
    {
        awReadDCUsettingBuf[i] = 0;
    }

    for(i = 0; i < DCUsetting_LEN; i ++)
    {
            uDCUsetting.ReadSettingBUF[i] = 0x0;
    }


    for(i = 0; i < 3; i ++)
    {
        //read
        memcpy((void*)SW_DATA_BUF, (void*)SW_DATA_ADDR, SW_DATA_SIZE * 2);

        for(j = 0; j < DCUsetting_BUFLEN; j ++)
        {
            awReadDCUsettingBuf[j] = SW_DATA_BUF[DCUsetting_START_SEAT + j];
        }

        j = 0;
        for(i = 0; i < DCUsetting_LEN; i ++)
        {
            j ^= awReadDCUsettingBuf[i];
        }

        if((j == awReadDCUsettingBuf[DCUsetting_CHS_SEAT - DCUsetting_START_SEAT]) && (awReadDCUsettingBuf[DCUsetting_0AA_SEAT - DCUsetting_START_SEAT] == IBPALARM_0XAA) \
             && (awReadDCUsettingBuf[DCUsetting_055_SEAT - DCUsetting_START_SEAT] == IBPALARM_0X55))
        {
            for(i = 0; i < DCUsetting_LEN; i ++)
            {
                uDCUsetting.ReadSettingBUF[i] = awReadDCUsettingBuf[i];
            }
            return OK;
        }
    }

        const int8u bit_buf[9] =    {0b00000000,
                                    0b00000001,
                                    0b00000011,
                                    0b00000111,
                                    0b00001111,
                                    0b00011111,
                                    0b00111111,
                                    0b01111111,
                                    0b11111111
                                    };
        
        j = cDefault_DCU_NUM;
        
        for(i = 0; i < DCUsetting_LEN; i ++)
        {
            if(j > 8)
            {
                j -= 8;
                uDCUsetting.ReadSettingBUF[i]   = 0xff;
            }   
            else
            {
                uDCUsetting.ReadSettingBUF[i]   = bit_buf[j];
                break;
            }   
            
        }

    return FAIL;
}

