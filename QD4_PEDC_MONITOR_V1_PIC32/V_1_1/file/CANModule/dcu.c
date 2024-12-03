
#include "..\Include.h"
#include "..\canbus\drv\glbCan.h"

void SetpBufBit(pINT8U ptr , int8u bit)
{
    int8u temp1, temp2;

    temp1 = bit / 8;
    temp2 = bit % 8;

    *(ptr + temp1) |= (1 << temp2);
}

void ClrpBufBit(pINT8U ptr , int8u bit)
{
    int8u temp1, temp2;

    temp1 = bit / 8;
    temp2 = bit % 8;

    *(ptr + temp1) &= ((1 << temp2) ^ 0xff);
}

//cDcuOnLineF ���� ��cDcuOffLineF ����
int8u pBufIfOnLine(pINT8U ptr ,int8u uBit)
{
    int8u temp1, temp2, temp3;

    temp1 = uBit / 8;
    temp2 = uBit % 8;
    temp3 = *(ptr + temp1);

    temp3 = (temp3 >> temp2) & 0x01;

    if(temp3)
        return cDcuOnLineF;
    else
        return cDcuOffLineF;
}

//cDcuOnLineF ���� ��cDcuOffLineF ����
int8u DcuIfOnLine(int8u uID_DCU)
{
    int8u i, j;
    i = pBufIfOnLine((pINT8U)&uniDcuData.sortBuf.aDcuCan12State, uID_DCU - ID_DCU_MIN);
    j = pBufIfOnLine((pINT8U)&uniDcuData.sortBuf.aDcuCan12State[DCU_ONECAN_ONLINE_LEN], uID_DCU - ID_DCU_MIN);

    if((i == cDcuOnLineF) || (j == cDcuOnLineF))
    {
        return cDcuOnLineF;
    }

    return cDcuOffLineF;

}

//BITEQU1 ����λ == 1 ��BITEQU0 ����λ == 0
int8u ptrDcuIfOnLine(pINT8U ptr , int8u Id)
{
    int8u temp1, temp2, temp3;

    Id = Id - ID_DCU_MIN;

    temp1 = Id / 8;
    temp2 = Id % 8;
    temp3 = * (ptr+ temp1);

    temp3 = (temp3 >> temp2) & 0x01;

    if(temp3)
        return BITEQU1;
    else
        return BITEQU0;
}

//��λ���Ƿ���� DCU
//node_No , 1 ---
//���� return 1 ; ������ return 0
int8u NodeJudge(int8u Id)
{
     return ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF , Id);
}

//������һ�� ���� DCU ,�ҵ� ����DCU Ϊֹ�����ѭ����һ�Σ��򷵻� FindFail ���൱���ҵ����DCU���ٴ�ͷ�ң�
//DcuNum DCU���� 1 = 1
int8u FindNextDcu(pINT8U ptr , pINT8U pID_DCU, int8u DcuNum)
{
        int8u i;
        for(i = 0; i < DcuNum; i ++)
        {
            (*pID_DCU) ++;

            if((*pID_DCU) > (ID_DCU_MIN + DcuNum - 1))
                {
                    return FindFail;
                }

            if(ptrDcuIfOnLine(ptr, *pID_DCU) == BITEQU1)
                {
                    if(DcuIfOnLine(*pID_DCU) == cDcuOnLineF)
                    {
                        return FindOk;
                    }
                }
        }

        return FindFail;
}

//DCU ��һ ���������ֵѭ������Сֵ
//DcuNum DCU���� 1 = 1
void DcuInc(pINT8U pID_DCU ,int8u DcuNum)
{
        int8u i;
        for(i = 0; i < DcuNum; i ++)
        {
            (*pID_DCU) ++;

            if((*pID_DCU) > DcuNum)
                {
                    *pID_DCU = ID_DCU_MIN;
                }

            if(ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF, *pID_DCU) == BITEQU1)
                {
                        break;
                }
        }

}

/* =============================================================================
 * ��DCU can 1��2 ����Ϣ���ݸ� uniDcuData
 * 
 * DCU can1, can2 �Ƿ�����, b0-dcu1 ,b1-dcu2... ,1����
 *      GET_CAN1_ERR_PTR()       &CanManage.SlvCan0Err
 *      GET_CAN2_ERR_PTR()       &CanManage.SlvCan1Err
 * */
PUBLIC void DcuCan12State(BOOL wait)
{
    tSYSTICK tickNow;
    static tSYSTICK tickUpdateCanState;
    
    pINT8U pStr1, pStr2, pStr2Setting;

    int8u i;
    
    tickNow = getSysTick();
    if(wait == TRUE)
    {
        if((tSYSTICK)(tickNow - tickUpdateCanState) < DEF_UPDATE_CAN12_STATE_TIME)
        {
            return;
        }
    }
    
    tickUpdateCanState = tickNow;

    pStr1 = GET_CAN1_ERR_PTR();
    pStr2 = (pINT8U)&uniDcuData.sortBuf.aDcuCan12State;
    
    pStr2Setting = (pINT8U)&uDCUsetting.settingBUF;

    for(i = 0; i < DCU_ONECAN_ONLINE_LEN; i ++)
    {
        *pStr2 = (~(*pStr1)) & ((*pStr2Setting));

        pStr1++;
        pStr2++;
        
        pStr2Setting++;

    }

    pStr1 = GET_CAN2_ERR_PTR();
    pStr2 = (pINT8U)&uniDcuData.sortBuf.aDcuCan12State[DCU_ONECAN_ONLINE_LEN];

    pStr2Setting = (pINT8U)&uDCUsetting.settingBUF;

    for(i = 0; i < DCU_ONECAN_ONLINE_LEN; i ++)
    {
        *pStr2 = (~(*pStr1)) & ((*pStr2Setting));

        pStr1++;
        pStr2++;

        pStr2Setting++;
    }
    
}

/* =============================================================================
 * �������ܺ���
 * */
//���һ�������һλ��Ϊ0�����
//Len �ֽ���
PUBLIC int8u findFirstBit(pINT8U p, int8u Len)
{
    int8u i,j;
    int8u L;
    int8u k = 0;
    int8u Num = 0;
    int8u mask = 0x1;
    
    L = Len;
    
    for(i = 0; i < L; i ++)
    {
        if( *(p + i))
        {
            k = 1;
            for(j = 0; j < 8; j ++)
            {
                if(*(p + i) & mask)
                {
                    Num += j;
                    break;
                }    
                mask = mask << 1;
            }
            break;          
        }   
        
        Num += 8;
    }
    
    if(k)
        return Num; 
    else
        return 0;   
    
    
}

//���һ��������һλ��Ϊ0�����
//Len �ֽ���
PUBLIC int8u findLastBit(pINT8U p, int8u Len)
{
    int8u i,j;
    int8u L;
    int8u k = 0;
    int8u Num = Len * 8 - 1;
    int8u bit = 0x80;
    
    if(Len == 0)
        return 0;
        
    L = Len;
    
    for(i = 0; i < L; i ++)
    {
        if( *(p + L - 1 - i))
        {
            k = 1;
            for(j = 0; j < 8; j ++)
            {
                if(*(p + L - 1 - i) & bit)
                {
                    Num -= j;
                    break;
                }    
                bit = bit >> 1;
            
            }
            
            break;          
        }   
        
        Num -= 8;
    }
    
    if(k)
        return Num; 
    else
        return 0;   
}

/* ����BUFFER��1������ 
 * ǰ�������������������ģ����Ǵӵ�һ��λ��ʼ
 */
PUBLIC int8u calcSetting1Num(pINT8U p, int8u Len)
{
    int i;
    int8u data,bits;
    int counter;
    
    counter = 0;
    
    for(i=0; i<Len; i++)
    {
        data = *p ++;
        if(data == 0xff)
        {
            counter += 8;
        }
        else
        {
            for(bits=0x01; bits>0; bits<<=1)
            {
                if(data & bits)
                {
                    counter++;
                }
                else
                {
                    break;
                }
            }
            break;
        }
    }
    
    return counter;
}

/* =============================================================================
 * */
/* �ռ� DCU ״̬ AlarmParaTxToAvr
 * ÿ��DCU�ϴ�����Ϣ��24Bytes��Byte0~9�� ��DCU��״̬��Ϣ����������Э���ļ�
 */
PUBLIC void AlarmParaTxToAvr(void)
{
    int8u i,k, temp = 0;
    tDownMsg DownMsg_Tmp;

    tDCU_AllStsFlags *ptrDcuAllSts;
    
    DownMsg_Tmp.word = 0;

    int8u DCU = ID_DCU_MIN ;
    int8u Door_OPN = 1;                 /*   0 = ֻҪ��һ��DCU���߻���ϣ��ղ���ȫ������Ϣ*/
    int8u OnLineDcuCnt = 0;             /*   DCU���߼����� */
    int8u ALL_DOOR_MANUAL_MODE = 1;     /*   1 �����Ŷ����ֶ�ģʽ*/ 
    int8u DCU_No = 0xff;                /*   ��0��ʼ�� ��DCU��ţ��������ߺͲ����ߣ�*/

    #if 1
    int8u first_DCU;   /*0 ��ʼ*/
    int8u last_DCU;    /*0 ��ʼ*/
    first_DCU = findFirstBit( (pINT8U)&uDCUsetting.settingBUF, sizeof(uDCUsetting.settingBUF));
    last_DCU  = findLastBit( (pINT8U)&uDCUsetting.settingBUF, sizeof(uDCUsetting.settingBUF));
    #endif

    for(i = 0; i < MAX_DCU; i ++)
    {
        /* �˱��������Ч�� */
        if(ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF, DCU) == BITEQU1)
        {
            DCU_No ++;  
                    
            if(DCU_No > cDefault_DCU_NUM)
                break;       /*������*/

            if(DcuIfOnLine(DCU) == cDcuOnLineF)
            {
                OnLineDcuCnt++;     /* DCU ���߼����� +1 */
                
                /* DCU �ϴ���״̬��־ */
                ptrDcuAllSts = (tDCU_AllStsFlags *)(&uniDcuData.sortBuf.aDcuState[i][0]);
                
                /* ���Ź���? */
                if(ptrDcuAllSts->err_open_door)
                {
                    DownMsg_Tmp.bOPN_FAULT = 1;
                }
                
                /* ���Ź���? */
                if(ptrDcuAllSts->err_close_door)
                {
                    DownMsg_Tmp.bCLS_FAULT = 1;
                }
                 
                /* �����Ź���?
                 * ���Ź��� | ���Ź��� | ��ȫ��·���� */
                if(ptrDcuAllSts->err_open_door | ptrDcuAllSts->err_close_door | ptrDcuAllSts->err_saft_loop_flag)
                {
                    DownMsg_Tmp.bPSD_FAULT = 1;
                }
                
                if(ptrDcuAllSts->LCB_Auto)
                {
                    /*�Զ�*/
                    ALL_DOOR_MANUAL_MODE = 0;
                }   
                else
                {
                    /* ������ֶ�*/
                    if(ptrDcuAllSts->LCB_Isolate)
                    {
                        DownMsg_Tmp.bIsolation_MODE = 1; /*����*/
                        ALL_DOOR_MANUAL_MODE = 0;
                    }
                    else
                    {
                        DownMsg_Tmp.bMANUAL_MODE = 1;    /*�ֶ�����*/
                    }   
                }

                /*ȡ��״̬*/
                switch(ptrDcuAllSts->DoorMode)
                {
                    case DOOR_STATE_UNLOCK:                 /*�ֶ�����*/
                        DownMsg_Tmp.bUNLOCK = 1;       
                        break;
                    case DOOR_STATE_CLOSING:                /*���Ź���*/
                        DownMsg_Tmp.bClosing = 1;       
                        break;
                    case DOOR_STATE_OPENING:                /*���Ź���*/
                        DownMsg_Tmp.bOpening = 1;      
                        break;
                        
                    case DOOR_STATE_OPEN:                   /* ���ѿ� */
                        /* ���� */
                        if(i == first_DCU)
                        {
                            DownMsg_Tmp.bHeadASD_Opned = 1;
                        }
                        /* β�� */
                        if(i == last_DCU)
                        {
                            DownMsg_Tmp.bTailASD_Opned = 1;
                        }   
                        break;
                        
                    case DOOR_STATE_CLOSING1:               /* �������ϴ����� */
                    case DOOR_STATE_OPENING1:               /* �������ϴ����� */
                        break;
                }
                
                //if(temp1.bMANUAL_MODE != 1) /*1�ֶ�����*/
                {
                    if(ptrDcuAllSts->DoorMode != DOOR_STATE_OPEN)   /*�Ƿ���״̬*/
                    {
                        Door_OPN = 0;               /*��ȫ������ֻҪ��һ��DCU���߻���ϣ��ղ���ȫ������Ϣ����������*/
                    }
                }   
                
                /* ÿ��ASD��2�� EED */
                /* Ӧ������1(��)��? Ӧ������2(��)?*/
                if(ptrDcuAllSts->Door_EED1_Switch_R || ptrDcuAllSts->Door_EED2_Switch_L)
                {
                    DownMsg_Tmp.bEmergenrydoor_opn = 1;    
                }
                
                /* ���Ŵ�? */
                if(ptrDcuAllSts->MSD1_Open_Switch || ptrDcuAllSts->MSD2_Open_Switch) 
                {
                    DownMsg_Tmp.bMSD_OPEN = 1; 
                }
                
                #if 0
                /* �źŽ�ֹ����? */
                if(ptrDcuAllSts->STC_Inhibit)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Iso_Feedback[0] , DCU_No); 
                }
                
                /* �ź�Զ��ʹ��? */
                if(ptrDcuAllSts->Remote_Enable_Command_1 || ptrDcuAllSts->Remote_Enable_Command_2)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ATSRemoteEn[0] , DCU_No);
                }        
                
                /* �ź�Զ�̿���? */
                if(ptrDcuAllSts->Remote_Open_Command)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Open_Feedback[0] , DCU_No); 
                }
                
                /* �ź�Զ�̹���? */
                if(ptrDcuAllSts->Remote_Close_Command)
                {
                    /*Remote close*/
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Close_Feedback[0] , DCU_No); 
                }
                
                /* �ر�����? */
                if(ptrDcuAllSts->C_L_Switch_Status)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_ClosedAndLocked[0] , DCU_No); 
                }
                
                /* �Ƿ�����? */
                if(ptrDcuAllSts->illegal_Open)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_IllegalOpen[0] , DCU_No); 
                }
                
                /*�׶���*/
                if(i == first_DCU)
                {
                    if(ptrDcuAllSts->MSD1_Open_Switch) 
                    {/*�׶��Źر�*/
                        DownMsg_Tmp.bMSD1_OPEN = 1;
                    }
                }   
                /*β����*/
                if(i == last_DCU)
                {
                    if(ptrDcuAllSts->MSD2_Open_Switch) 
                    {
                        DownMsg_Tmp.bMSD2_OPEN = 1;
                    }
                }   
                #endif

            }
            else
            {
                DownMsg_Tmp.bBUS_FAULT = 1;    /* CAN���߹���*/
                clr_CLSCLK(DCU);
            }
        }

        DCU ++;
    }

    /*
    OnLineDcuCnt > 0         : ������һ��DCU����
    ALL_DOOR_MANUAL_MODE = 1 : �����Ŷ����ֶ�ģʽ 
    �����ŵ�ԪLCBλ���ֶ�ģʽʱ����������ȫ����ȫ�ر��źű����ԣ�
    �����ŵ�ԪLCBλ���ֶ�ģʽʱ������ֻ��ʾվ̨������ȫ�ر��ź�
    */
    if((Door_OPN == 1) && (OnLineDcuCnt != 0) && (ALL_DOOR_MANUAL_MODE != 1))
    {
        DownMsg_Tmp.bDOOR_ALL_OPEN = 1;      /*��ȫ����*/
    }

    /*PEDC �� MMS ͨѶ��ʱ : ������ MMS ����״̬�����־ MONITOR_FAULT */
    DownMsg_Tmp.bMONITOR_FAULT = (getUartMMSLinkSts() == 0)? 1 : 0;

    if(tagDownIbpAlarmParameter.uIbpAlarmParameter.byte != Down_Message1.byte0)  //�������������޸ģ����屨���ı���
    {
        PSC_AlarmFlags.word = 0;
        
        PSL_AlarmFlags.word = 0;
        
        IBP_AlarmFlags.word = 0;
    }

    Down_Message0.word = DownMsg_Tmp.word;
    
    Down_Message1.byte0 = tagDownIbpAlarmParameter.uIbpAlarmParameter.byte;
    Down_Message1.byte1 = 0;
}


/* =============================================================================
 * �ռ� DCU ������״̬���õ�վ̨�Ŷ�λ�����г��ŵ���Ϣ
 *  */
PRIVATE int8u DCUOnLineSts[(MAX_DCU+7)/8];

/* ����Ƿ����е�DCU�����߻����ߣ������������������Ӧλ��ʾ ���߻�����
 * �������
 *  DCUs : DCU����
 *  chkStatus
 *      = cDcuOnLineF ��ʾ������ߣ� ������ߣ���Ӧλ����Ϊ 1
 *      = cDcuOffLineF ��ʾ������ߣ�������ߣ���Ӧλ����Ϊ 1
 *  setting : ���õ�DCU
 *      ��ӦλΪ1��ʾ��DCU����Ϊ��Ч��0��ʾ��Ч����������ЧDCU����״̬
 * ���أ�
 *  ������ݴ洢��ָ��
 * 
 * ע�⣺��Ҫ����ʵ�����õ�DCU (uDCUsetting.settingBUF)
 *       �������ε�DCU����Ӧλ���� 0
 */
PUBLIC int8u* getAllDCUOnLineSts(int8u DCUs, int8u chkStatus, int8u* setting)
{
    int i,j,DCU;
    
    for(i = 0; i <= (DCUs+7)/8; i ++)
    {
        DCUOnLineSts[i] = 0;
    }
    
    j = 0;
    DCU = ID_DCU_MIN;
    for(i = 0; i < DCUs; i ++)
    {
        if(ptrDcuIfOnLine(setting, DCU) == 1)
        {
            if(DcuIfOnLine(DCU) == chkStatus)
            {
                SetpBufBit(DCUOnLineSts,j); 
            }
        }
        j ++;
        DCU ++;
    }
    
    return DCUOnLineSts;
}

/*
 * ���� DCU ������״̬���õ�վ̨�Ŷ�λ�����г��ŵ���Ϣ
 * ����
 *  pInhibit : ��������ָ��Ĵ洢��
 *  len      : �洢������
 * ���
 *  ���浽 pInhibit �洢��
 * ����
 *  TRUE ��ʾ�ɹ�
 *  */
PUBLIC BOOL getPSDInhibitTrain(int8u *pInhibit, int len)
{
    int i;
    tDCU_AllStsFlags *pDcuStsFlags;
    int8u result[(MAX_DCU+7)/8];
    
    for(i=0; i<sizeof(result); i++)
    {
        result[i] = 0;
    }
    
    /* ����DCU������״̬��������ߣ���Ӧλ����Ϊ 1 */
    getAllDCUOnLineSts(MAX_DCU, cDcuOnLineF, (int8u*)&uDCUsetting.settingBUF);
    
    /* ������״̬ */
    for(i=0; i<MAX_DCU; i++)
    {
        pDcuStsFlags = (tDCU_AllStsFlags*)(&uniDcuData.sortBuf.aDcuState[i]);
        
        /* SZ20YQ ����ȷ
         * �Ͼ�7���������������֮һ������Ϊվ̨�Ź��ϣ��ظ����ź�ϵͳ��
         * LCB���뵵λ��                                 BYTE 0.6 = 1 ��ʾLCB����ģʽ
         * LCB���ŵ�λ��                                 BYTE 0.7 = 0 �ֶ�ģʽ
         * LCB���ŵ�λ��                                 BYTE 0.7 = 0 �ֶ�ģʽ
         * LCB���Զ���λ����վ̨�Ź��ϣ������Ź���ʧЧ�� BYTE 5.0~3 DCUģʽ = 5 (ֹͣ���ܲ��������ϼ�������)
         */
        if(pDcuStsFlags->LCB_Isolate || (pDcuStsFlags->LCB_Auto == 0) || (pDcuStsFlags->DCUMode == DCU_STATE_STOP))
        {
            SetRamBit(result, i);
        }
    }
    
    /* ���ߵ�DCU���� */
    for(i=0; i<sizeof(DCUOnLineSts); i++)
    {
        result[i] &= DCUOnLineSts[i];               /* ���ߵ�DCU�������� */
        result[i] &= uDCUsetting.settingBUF[i];     /* �������õ�DCU������Ӧ�ĸ�����Ϣ */
    }
    
    if(len > sizeof(result))
        len = sizeof(result);
    
    for(i=0; i<len; i++)
    {
        *pInhibit ++ = result[i];
    }
    
    return TRUE;
}






