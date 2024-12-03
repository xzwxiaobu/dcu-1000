
#include "..\Include.h"
#include "CanTgt.h"

void wtj_CapiStartTrans(int8u uFUNID, int8u uID_DCU)
{
    CapiStartTrans(uFUNID, uID_DCU, GET_TRANS_LEN(uFUNID), GET_TRANS_BUF(uFUNID));
}

int8u wtj_ChkTransState(int8u uFUNID)
{
    int8u i;
    i = ChkTransState(uFUNID);

    #ifdef DEBUG_PRINTF_ERR                       //�������ӡ������Ϣ
    if((i == ican_read_Err) || (i == ican_write_Err))
        Printf_ERR(uFUNID, defcERR_SORT_00);
    #endif

    return i;
}

//������DCU ����
void Can_Up_MoreDcuParameter(void)
{
    int8u i ,j;
    pINT8U ptr1;
    pINT8U ptr2;

    int8u NextDcu;

    NextDcu = 0;

    if(bFun_Up_MoreDcuParameter == 1)
    {
        i = BusState_Up_MoreDcuParameter();

        switch(i)
        {
            case  ican_read_OK:

                ptr1 = (pINT8U)& tagUpDcuParameter.aUpDcuParameterBuf;
                ptr2 = (pINT8U)& tagUpDcuParameter.aAllDcuPara[tagUpDcuParameter.aUpDcuParameter_DcuNum][0];
                for(j = 0; j < ONE_DCU_PARA_LEN; j ++)
                {
                    *ptr2++ = *ptr1++;
                }

                SetpBufBit( (pINT8U)&tagUpDcuParameter.aUpDcuParameterResult ,tagUpDcuParameter.uUpDcuParameter_IdDcu - ID_DCU_MIN);

                NextDcu = 1;

                tagUpDcuParameter.aUpDcuParameter_DcuNum ++;

                break;

            case  ican_read_ing:
                break;

            case  ican_read_Err:
                    NextDcu = 1;
                break;

            default:

                    NextDcu = 1;
                break;

        }
    }

    if(NextDcu == 1)  //������һ�� DCU
    {

        j = FindNextDcu((pINT8U) &tagUpDcuParameter.aUpDcuParameterRequest, (pINT8U) &tagUpDcuParameter.uUpDcuParameter_IdDcu, MAX_DCU);

        if(j == FindOk)
        {
            Build_UpMoreDcuPara(tagUpDcuParameter.uUpDcuParameter_IdDcu);
            return;
        }

        else
        {
            bUART2_UpMoreDcuParaOK = 1;
            bFun_Up_MoreDcuParameter = 0;

            return;
        }
    }
}


//������ѡDCU����
void Can_Down_MoreDcuParameter(void)
{
    int8u i, j;
    int8u NextDcu;

    NextDcu = 0;

    if(bFun_Down_MoreDcuParameter == 1)
    {
        i = BusState_Down_MoreDcuParameter();
        switch(i)
        {
            case  ican_write_OK:

                //���ø�DCU ���سɹ���־
                SetpBufBit((pINT8U) &tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterResult, tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu - ID_DCU_MIN);
                NextDcu = 1;
                break;

            case  ican_write_Err:

                NextDcu = 1;
                break;

            case  ican_write_ing:
                break;

            default:

                NextDcu = 1;
                break;
        }

        //������һ�� DCU
        if(NextDcu == 1)
        {
            j = FindNextDcu((pINT8U) &tagDOWN_MORE_DCU_PARAMETER.aDownDcuParameterRequest, (pINT8U) &tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu, MAX_DCU);

            if(j == FindOk)
            {
                Build_DownMoreDcuPara(tagDOWN_MORE_DCU_PARAMETER.uDownDcuParameter_IdDcu);
                return;

            }

            else
            {
                bUART2_DownMoreDcuParaOK = 1;
                bFun_Down_MoreDcuParameter = 0;
                return;

            }

        }
    }
}

//����������
void Can_UpOpenDoorCurve(void)
{
    int8u i;

    if(bFun_Up_OpenDoorCurve == 1)
    {
        i = BusState_Up_OpenDoorCurve();

        switch(i)
        {
            case  ican_read_OK:
                bUART2_UpOpenDoorCurveOK = 1;
                bFun_Up_OpenDoorCurve = 0;
                break;
            case  ican_read_Err:
                bUART2_UpOpenDoorCurveFail = 1;
                bFun_Up_OpenDoorCurve = 0;
                break;
            case  ican_read_ing:
                break;
            default:
                bUART2_UpOpenDoorCurveFail = 1;
                bFun_Up_OpenDoorCurve = 0;

                break;
        }
    }
}

//����������
void  Can_UpCloseDoorCurve(void)
{
    int8u i;

    if(bFun_Up_CloseDoorCurve == 1)
    {
        i = BusState_Up_CloseDoorCurve();

        switch(i)
        {
            case  ican_read_OK:
                bUART2_UpCloseDoorCurveOK = 1;
                bFun_Up_CloseDoorCurve = 0;
                break;

            case  ican_read_Err:
                bUART2_UpCloseDoorCurveFail = 1;
                bFun_Up_CloseDoorCurve = 0;
                break;

            case  ican_read_ing:
                break;

            default:
                bUART2_UpCloseDoorCurveFail = 1;
                bFun_Up_CloseDoorCurve = 0;

                break;
        }
    }
}

//�����ߵ�DCU ״̬���Զ���������г���վ�ȡ�
//����������� DCU N �� ,����� �����߱�־
void Can_UpOneDcuState(void)
{
    static tSYSTICK tickPrev = 0;
    tSYSTICK interval;
    tSYSTICK tickNow;
    
    tickNow = getSysTick();
    if((tickNow != tickPrev))
    {
        interval = (tSYSTICK)(tickNow - tickPrev);
        tickPrev = tickNow;
        
        if((tSYSTICK)(tagDcuState.uUpOneDcuStateTime + interval) <= 255)
            tagDcuState.uUpOneDcuStateTime += interval;
        else
            tagDcuState.uUpOneDcuStateTime  = 255;
    }
    

    int8u i,j;
    pINT8U ptr1, ptr2;

    if(bFun_UpOneDcuState == 1)
    {
        i = BusState_UpOneDcuState();
        switch(i)
        {
            case  ican_read_OK:

                bFun_UpOneDcuState = 0;

                tagDcuState.aDCUOffLinejudgeTime[tagDcuState.uID_DCU - ID_DCU_MIN] = 0;

                ptr1 = (pINT8U) &uniDcuData.sortBuf.aDcuState[(tagDcuState.uID_DCU - ID_DCU_MIN)][0];
                ptr2 = (pINT8U) &tagDcuState.aUpOneDcuStateBuf;
                for(j = 0; j < ONE_DCU_STATE_LEN; j ++)
                {
                    *ptr1++ = *ptr2++;
                }

                if(ReadDcuStateFailBuf[tagDcuState.uID_DCU  - ID_DCU_MIN] != 0)
                    ReadDcuStateFailBuf[tagDcuState.uID_DCU  - ID_DCU_MIN] --;

                DcuInc(&tagDcuState.uID_DCU, ID_DCU_MAX);

                break;

            case  ican_read_Err:
                bFun_UpOneDcuState = 0;

                tagDcuState.aDCUOffLinejudgeTime[tagDcuState.uID_DCU - ID_DCU_MIN] ++ ;
                if((tagDcuState.aDCUOffLinejudgeTime[tagDcuState.uID_DCU - ID_DCU_MIN]) > defOFFLINEJUDGETIME)
                {
                    tagDcuState.aDCUOffLinejudgeTime[tagDcuState.uID_DCU - ID_DCU_MIN] = 0;

                    ptr1 = (pINT8U) &uniDcuData.sortBuf.aDcuState[(tagDcuState.uID_DCU - ID_DCU_MIN)][0];
                    #if 0
                    /* ���������DCU����Ϣ��
                     * ����� MMS �������й�DCU��ʾ������˸������ȡ�� */
                    for(j = 0; j < ONE_DCU_STATE_LEN; j ++)
                    {
                        *ptr1++ = 0;
                    }
                    #endif
                    DcuInc(&tagDcuState.uID_DCU, ID_DCU_MAX);
                }

                else
                {
                    //ͷ����ʧ�ܾ�������
                    if(tagDcuState.aDCUOffLinejudgeTime[tagDcuState.uID_DCU - ID_DCU_MIN] >= defSTATEREADERR_CONTINUEREAD)
                    {
                        if(ReadDcuStateFailBuf[tagDcuState.uID_DCU  - ID_DCU_MIN] != 0xff)
                            ReadDcuStateFailBuf[tagDcuState.uID_DCU  - ID_DCU_MIN] ++;

                        DcuInc(&tagDcuState.uID_DCU, ID_DCU_MAX);
                    }
                }

                break;

            case  ican_read_ing:
                break;

            default:
                bFun_UpOneDcuState = 0;

                break;
        }
    }
    else 
    {
        /* �ο� NCRTC :
         * ����DCU����������Խ�٣���DCU״̬�����ʱ��Խ��������DCUû�м�ʱ����״̬��Ϣ
         *                     DCU ����           1,  2,  3,  4,  5,  6,  7,  8,  9+  */
        tSYSTICK    readInteval_time[10] = {120, 120, 60, 40, 30, 25, 20, 15, 10, 8};
        tSYSTICK    readInteval;
        j = 0;
        for(i = ID_DCU_MIN; i < ID_DCU_MAX; i ++)
        {
            if(DcuIfOnLine(i) == cDcuOnLineF)
            {
                if(j >= 9)
                {
                    j = 9;
                    break;
                }
                j++;
            }
        }
        readInteval = readInteval_time[j];
        
        if(tagDcuState.uUpOneDcuStateTime >= readInteval)    //defUPONEDCUSTATETIME)
        {
            tagDcuState.uUpOneDcuStateTime = 0;
            for(i = 0; i < MAX_DCU; i ++)
            {

                if((tagDcuState.uID_DCU > ID_DCU_MAX) || (tagDcuState.uID_DCU < ID_DCU_MIN))
                {
                    tagDcuState.uID_DCU = ID_DCU_MIN;
                }

                if(ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF, tagDcuState.uID_DCU) == BITEQU0)
                {   
                    DcuInc(&tagDcuState.uID_DCU ,ID_DCU_MAX);
                    continue;
                }

                if(DcuIfOnLine(tagDcuState.uID_DCU) == cDcuOnLineF)   //ֻ��ȡ���ߵ� DCU
                {
                    bFun_UpOneDcuState = 1;
                    Build_UpOneDcuState(tagDcuState.uID_DCU);
                    break;
                }
                else
                {
                    ptr1 = (pINT8U) &uniDcuData.sortBuf.aDcuState[(tagDcuState.uID_DCU - ID_DCU_MIN)][0];
                    for(j = 0; j < ONE_DCU_STATE_LEN; j ++)
                    {
                        //*ptr1++ = 0;
                    }
                    ReadDcuStateFailBuf[tagDcuState.uID_DCU  - ID_DCU_MIN] = 0;

                }

                DcuInc(&tagDcuState.uID_DCU ,ID_DCU_MAX);
            }

        }
    }
}

//��ѯ ��ȡDCU state ���ݵĴ�����Ϣ
//in:  id (0 -- n)
//out: = true ��ʾ��������������÷�Χ�� = false ��ʾ������������÷�Χ֮��
int8u ReadDcuStateFail(int8u id)
{
    if(ReadDcuStateFailBuf[id] > 250)
    {
        return true;
    }
    else
    {
        return false;
    }       
}

//�������
void ClsReadDcuStateFailBuf(int8u id)
{
    ReadDcuStateFailBuf[id] = 0;
}

//��ѯDCU �����Ϣ
void Can_QUERY(void)
{
    int8u i;

    if(bFun_QUERY == 1)
    {
        i = BusState_QUERY();
        switch(i)
        {
            case  ican_read_OK:
                bUART2_QueryOK = 1;
                bFun_QUERY = 0;
                break;

            case  ican_read_Err:
                bUART2_QueryFAIL = 1;
                bFun_QUERY = 0;
                break;

            case  ican_read_ing:
                break;

            default:
                bUART2_QueryFAIL = 1;
                bFun_QUERY = 0;

                break;
        }
    }
}


/* =============================================================================
 * �㲥�������
 * 
 */

/* ��ӹ㲥���� 
 * ����
 *  command     : �㲥�������ݽṹ��
 *                �˺����Զ�����㲥�������ݵ�CRC
 *  cmdNum      : �㲥��������
 *  sendAtOnce  : �������ͱ�־
 *               TRUE�� ��������ӹ㲥��������
 *               FALSE�����ж�����2�δ�����������ͬʱ��ʱ�������پ����Ƿ�������ӵ��㲥��������
 * ����
 *  TRUE        : ��ӳɹ�
 *  FALSE       : ���ʧ�ܣ�ʧ�ܵ�ԭ��
 *                  - ���������ʹ���
 *                  - ����������ͬ�����ʱ��������
 * ע��
 *  ͬһ���͵Ĺ㲥����ֻ����һ��
 */
PRIVATE BOOL addBroadCmdTask_Com(tBroadCmdPkg *command, enmBroadCmdType cmdType, BOOL sendAtOnce)
{
    unsigned short CRC;
    if(cmdType < BROAD_CMD_NUM)
    {
        #if 0
        /* ��������жϵ����ã����ͬһ�����͵Ĺ㲥�������ڷ����У������ʧ�� 
         * �������������жϣ���ͬһ���͵����
         *   1-���ڷ��ͣ��ڹ㲥������ɺ󣬻�������ʹ�����
         *   2-û�з��ͣ����µĹ㲥����Ḳ��δ���͵�����
         */
        if((CANSendBroadCmdManage.isBroading != 0) && ( CANSendBroadCmdManage.BroadingType == cmdType))
        {
            return FALSE;
        }
        #endif
        
        /* ���������ͷ��͵�ʱ��������С���趨ֵ
         * ��Ҫ�� 
         *   ���δ����͵��Ƿ����ϴη��͵ıȽ�
         *   1. ������ͬ����������������
         *   2. ����ͬ��  �����Ӧ�ò������, sendAtOnce = TRUE �������� 
         *  */
        if(broadCmdList[cmdType].uRepeatSendTime < (CREPEATSEND_SEC*10))
        {
            BOOL sameCmd = TRUE;
            
            int i;
            int8u * pNew = (int8u *)(command);
            int8u * pOld = (int8u *)(&broadCmdList[cmdType].cmdFrame);
            for(i=0; i<sizeof(tBroadCmdPkg) - 3; i++)
            {
                if(*pNew != *pOld)
                {
                    sameCmd = FALSE;
                    break;
                }
                pNew ++; 
                pOld ++;
            }
            
            /* ��ͬ���� 
             * �Ҳ���Ҫ�������ͣ����˳�
             */
            if((sameCmd == TRUE) && (sendAtOnce == FALSE))
            {
                return FALSE;
            }
        }
        
        broadCmdList[cmdType].cmdFrame = *command;
        broadCmdList[cmdType].uRepeatSendTime = 0;
        
        //CRC = CRC16_Dn_Cal(&broadCmdList[cmdType].cmdFrame.broadCmdBuf[0], BROADCOMMAND_LEN + 3, 0xffff);
        CRC = CRC16_Dn_Cal((int8u *)command, sizeof(tBroadCmdPkg)-3, 0xffff);
        broadCmdList[cmdType].cmdFrame.CRCH = (CRC >> 8) & 0xff;
        broadCmdList[cmdType].cmdFrame.CRCL = CRC & 0xff;
        broadCmdList[cmdType].startSend = 1;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/* ���ݷ������ݼ�ʱ���������һ�������͵Ĺ㲥���
 * ���� addBroadCmdTask_Com �Ĳ���˵��
 *  */
PUBLIC BOOL addBroadCmdTask(tBroadCmdPkg *command, enmBroadCmdType cmdType)
{    
    return addBroadCmdTask_Com(command, cmdType, FALSE);
}

/* �������һ�������͵Ĺ㲥���
 * ���� addBroadCmdTask_Com �Ĳ���˵��
 *  */
PUBLIC BOOL addBroadCmdTask_AtOnce(tBroadCmdPkg *command, enmBroadCmdType cmdType)
{
    return addBroadCmdTask_Com(command, cmdType, TRUE);
}

/* 
 * ִ�й㲥����������һ��ʱ�䰴�趨�Ĵ����ظ�����
 */
PRIVATE void doBroadcastTask(void)
{
    if(CANSendBroadCmdManage.isBroading != 0)
    {
        if(CANSendBroadCmdManage.Tx_intervals >= BROADCMD_INTERVAL_MS)
        {
            CANSendBroadCmdManage.Tx_NUM ++;
            if(CANSendBroadCmdManage.Tx_NUM <= CANSendBroadCmdManage.setRepeatTimes)
            {
                CANSendBroadCmdManage.Tx_intervals = 0;
                Build_Down_TOSLVCOMMAND();
            }   
            else
            {
                CANSendBroadCmdManage.isBroading = 0;
            }   
        }       
    }
}

/* ׼�����͹㲥����
 * �������͵Ĺ㲥����Ƶ����������������� 
 */
PRIVATE void readyToBroadcast(int num)
{
    CANSendBroadCmdPkg = broadCmdList[num].cmdFrame;
    
    CANSendBroadCmdManage.Tx_intervals = BROADCMD_INTERVAL_MS;  /* �����������ͣ�����Ҫ�ȴ�ʱ���� */
    CANSendBroadCmdManage.Tx_NUM = 0;
    CANSendBroadCmdManage.isBroading = 1;
    CANSendBroadCmdManage.BroadingType = num;

    if(CANSendBroadCmdManage.setRepeatTimes == 0)
    {
        CANSendBroadCmdManage.setRepeatTimes = BROADCMD_REPEAT_TIMES;
    }
    if(CANSendBroadCmdManage.setInterval_ms == 0)
    {
        CANSendBroadCmdManage.setInterval_ms = BROADCMD_INTERVAL_MS;
    }
}

/* ���÷��͹㲥����Ĳ���
 * ���������û�����ã������Ĭ��ֵ
 * �����ò��� = 0���������⵽�Ժ���Ĭ��ֵ�������� (readyToBroadcast)
 */
PUBLIC void setRepeatPara(int8u repeatTimes, int8u repeatInterval)
{
    CANSendBroadCmdManage.setRepeatTimes = repeatTimes;
    CANSendBroadCmdManage.setInterval_ms = repeatInterval;
}


#if 0
/* ���Է��͸�DCU�㲥���� 
 * ͨ����ָͬ�����ϣ����Զ�DCU���������ֵĲ�����������DCU�ķ����жϲ����Ƿ���ȷ
 */
PRIVATE void testBroadcastCmd(void)
{    int i;
    static t = 0;
    static int16u prevTick = 0;
    tBroadCmdPkg tmpBroadCmd;
    
    if((int16u)(TgtGetTickCount() - prevTick) >= 1000)
    {
        prevTick = TgtGetTickCount();
        
        for(i = 0; i < BROADCOMMAND_LEN; i ++)
        {
            tmpBroadCmd.broadCmdBuf[i] = 0xff;//i * 16 + i;
        }
        
        t++;

        tmpBroadCmd.oprCmd1.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BYTE_CLR;
        tmpBroadCmd.oprCmd2.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_SET | 5;
        tmpBroadCmd.oprCmd3.byte = 0;//OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_EXCLUDE | CMD_BIT_SET | 6;
        
        if(t & 1)
        {
            tmpBroadCmd.oprCmd3.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_SET | 7;
            tmpBroadCmd.oprCmd2.byte = OPCMD_VALID | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_SET | 6;
        }
        else
        {
            tmpBroadCmd.oprCmd3.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CLR | 7;
            tmpBroadCmd.oprCmd2.byte = OPCMD_VALID | OP_DCUCMD_2 | OP_BIT_NORMAL | CMD_BIT_CLR | 6;
        }
            
        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_MMS_INHIBIT);
    }
    
}
#endif

/* ��Ҫ������������������
 * ����ɨ������͵Ĺ㲥����
 * ÿ���㲥���������Ժ󣬲�����һ����Ҫ���͵Ĺ㲥����
 */
PUBLIC void procBroadcastTask(void)
{
    static int8u broadCmdTurn = 0;      /* ��һ�η��͹㲥�����˳��� */
    static int16u prevTick = 0;
    static int16u prevTickMs = 0;
    
    int16u i;
    int16u intervalTemp;
    
    /* ������ʱ���� */
    intervalTemp = (int16u)(TgtGetTickCount() - prevTickMs);
    prevTickMs = TgtGetTickCount();
    
    i = (int16u)CANSendBroadCmdManage.Tx_intervals + intervalTemp;
    if(i > 255)
    {
        i = 255;
    }
    CANSendBroadCmdManage.Tx_intervals = i;
    
    /* ��ʱ�����ڿ���ͬһ�������η��� */
    if((int16u)(TgtGetTickCount() - prevTick) >= 100)
    {
        prevTick = TgtGetTickCount();
        
        for(i=0; i<BROAD_CMD_NUM; i++)
        {
            if(broadCmdList[i].uRepeatSendTime < 255)
            {
                broadCmdList[i].uRepeatSendTime++;
            }
        }
    }
    
    #if 0
    /* ���Է��͸�DCU�㲥���� */
    testBroadcastCmd();    
    #endif
    
    /* �㲥����͹��� */
    if(CANSendBroadCmdManage.isBroading == 0)
    {
        /* û�з��͹㲥����
         * �����Ƿ��д����͵Ĺ㲥����
         * ���ִ����͵Ĺ㲥������������������˳�
         * 
         */
        for(; broadCmdTurn < BROAD_CMD_NUM; broadCmdTurn++)
        {
            if(broadCmdList[broadCmdTurn].startSend > 0)
            {
                broadCmdList[broadCmdTurn].startSend = 0;
                readyToBroadcast(broadCmdTurn);
                
                /* ��һ�β��ҵ�λ�� */
                broadCmdTurn++;
                break;
            }
        }
        
        if(broadCmdTurn >= BROAD_CMD_NUM)
        {
            broadCmdTurn = 0;
        }
    }
    
    /* ִ�й㲥�������� */
    doBroadcastTask();
}


/* =============================================================================
 * 2022-4-12
 *  ������ָ�� DCU ��IP����
 *  */
void Can_UploadDCU_IPPara(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. ���������ʼ�� */
    if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_START)
    {
        /* ���ö�ȡ��� */
        for(i = 0; i<sizeof(uploadDCU_IPPara.uploadDCUObjs); i++)
        {
            uploadDCU_IPPara.uploadIPResult[i]  = 0;
        }

        /* ������еĽ���洢�� */
        pDst = (int8u*)uploadDCU_IPPara.allDCU_IPPara;
        for(i = 0; i < sizeof(uploadDCU_IPPara.allDCU_IPPara); i ++)
        {
            *pDst ++ = 0;
        }

        uploadDCU_IPPara.uploadCnt          = 0;
        uploadDCU_IPPara.currentDCUID       = ID_DCU_MIN - 1;
        uploadDCU_IPPara.uploadTask         = IPPARA_TASK_SENDING;
    }
    
    /* 2. �������� */
    if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_SENDING)
    { 
        /* �������õĶ�ȡDCU������DCU ��������Ҫ��ȡʱ������ֵΪ OK */
        bNext = FindNextDcu((int8u*) &uploadDCU_IPPara.uploadDCUObjs, (int8u*) &uploadDCU_IPPara.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            wtj_CapiStartTrans(FUNID_UPLOAD_DCUIPPARA, uploadDCU_IPPara.currentDCUID);
            uploadDCU_IPPara.uploadTask         = IPPARA_TASK_WAITING;
            uploadDCU_IPPara.uploadStartTick    = getSysTick();
        }
        else
        {
            /* ���� DCU ��ȡ��� */
            uploadDCU_IPPara.uploadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. �ȴ����     *    �������Ƶȴ�ʱ��     *  */
    else if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_WAITING)
    {
        if((getSysTimeDiff(uploadDCU_IPPara.uploadStartTick) > 100))
        {
            uploadDCU_IPPara.uploadTask = IPPARA_TASK_SENDING;
            ClrTransState(FUNID_UPLOAD_DCUIPPARA);
        }
        else
        {
            sts = wtj_ChkTransState(FUNID_UPLOAD_DCUIPPARA);
            switch(sts)
            {
                case  ican_write_ing:
                    break;

                case  ican_write_OK:
                    //���ø�DCU ��ȡ�ɹ���־
                    SetpBufBit((int8u*) &uploadDCU_IPPara.uploadIPResult, uploadDCU_IPPara.currentDCUID - ID_DCU_MIN);

                    //�洢��DCU ��IP����
                    pSrc  = (int8u*)  uploadDCU_IPPara.oneDCUIPPara;
                    pDst = (int8u*)  uploadDCU_IPPara.allDCU_IPPara[uploadDCU_IPPara.currentDCUID - ID_DCU_MIN];
                    for(j = 0; j < ONE_DCU_IPPARA_BUFLEN; j ++)
                    {
                        *pDst++ = *pSrc++;
                    }

                    /* ��ȡ�ɹ������� */
                    uploadDCU_IPPara.uploadCnt ++;

                case  ican_write_Err:
                default:
                    /* ��ȡ��һ�� DCU */
                    uploadDCU_IPPara.uploadTask = IPPARA_TASK_SENDING;
                    break;
            }
        }
    }
}



/* =============================================================================
 * 2022-4-12
 *  ��������ָ���� DCU IP��������Ҫ��������
 *  */
void Can_DownDCU_IPPara(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. ���������ʼ�� */
    if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_START)
    {
        //������ؽ����־
        for(i = 0; i<sizeof(DownloadDCU_IPPara.downloadIPResult); i++)
        {
            DownloadDCU_IPPara.downloadIPResult[i]  = 0;
        }

        //��ʼ����������
        DownloadDCU_IPPara.downloadCnt  = 0;
        DownloadDCU_IPPara.currentDCUID = ID_DCU_MIN - 1;       /* -1 Ϊ�˲��ҵ�һ��DCU */
        DownloadDCU_IPPara.downloadTask = IPPARA_TASK_SENDING;
    }
    
    /* 2. ������������ */
    if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_SENDING)
    {
        /* �������õ�DCU������DCU ��������Ҫ����ʱ������ֵΪ OK */
        bNext = FindNextDcu((int8u*) &DownloadDCU_IPPara.dcuObjects, (int8u*) &DownloadDCU_IPPara.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            /* ׼�����͵����� */
            pDst = CANBus_downloadIPPara;
            
            /* server ip-port  */
            pSrc = DownloadDCU_IPPara.ServerIPPort;
            for(i=0; i<DCU_SERVERIP_LEN; i++)
            {
                *pDst++ = *pSrc++;
            }
            
            /* local  ip & token */
            pSrc = DownloadDCU_IPPara.dcuIPToken[DownloadDCU_IPPara.downloadCnt];
            for(i=0; i<DCU_IPSETTING_LEN; i++)
            {
                *pDst++ = *pSrc++;
            }
            
            /* ͨ�� CAN �������� IP ���� */
            wtj_CapiStartTrans(FUNID_DOWNLOAD_DCUIPPARA, DownloadDCU_IPPara.currentDCUID);
            DownloadDCU_IPPara.downloadTask         = IPPARA_TASK_WAITING;
            DownloadDCU_IPPara.downloadStartTick    = getSysTick();
            
            /* ���ؼ����� */
            DownloadDCU_IPPara.downloadCnt++;
        }
        else
        {
            /* ���� DCU ������� */
            DownloadDCU_IPPara.downloadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. �ȴ�������������Ƶȴ�ʱ��  *  */
    else if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_WAITING)
    {
        if((getSysTimeDiff(DownloadDCU_IPPara.downloadStartTick) > 100))
        {
            DownloadDCU_IPPara.downloadTask     = IPPARA_TASK_SENDING;
            ClrTransState(FUNID_DOWNLOAD_DCUIPPARA);
        }
        else
        {
            sts = wtj_ChkTransState(FUNID_DOWNLOAD_DCUIPPARA);
            switch(sts)
            {
                case  ican_read_OK:
                    /* ���ø�DCU ���سɹ���־ */
                    SetpBufBit((int8u*) &DownloadDCU_IPPara.downloadIPResult, DownloadDCU_IPPara.currentDCUID - ID_DCU_MIN);
                case  ican_read_Err:
                default:
                    /* ׼��������һ�� DCU */
                    DownloadDCU_IPPara.downloadTask = IPPARA_TASK_SENDING;
                    break;

                case  ican_read_ing:
                    break;
            }
        }
    }
}


/* =============================================================================
 * 2023-8-21
 *  ������ָ�� DCU ���������
 *  */
void Can_UploadDCU_TripData(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. ���������ʼ�� */
    if(uploadDCU_Trip.uploadTask == IPPARA_TASK_START)
    {
        /* ���ö�ȡ��� */
        for(i = 0; i<sizeof(uploadDCU_Trip.uploadDCUObjs); i++)
        {
            uploadDCU_Trip.uploadTripResult[i]  = 0;
        }

        /* ������еĽ���洢�� */
        pDst = (int8u*)uploadDCU_Trip.allDCU_Trip;
        for(i = 0; i < sizeof(uploadDCU_Trip.allDCU_Trip); i ++)
        {
            *pDst ++ = 0;
        }

        uploadDCU_Trip.uploadCnt        = 0;
        uploadDCU_Trip.currentDCUID     = ID_DCU_MIN - 1;
        uploadDCU_Trip.uploadTask       = IPPARA_TASK_SENDING;
    }
    
    /* 2. �������� */
    if(uploadDCU_Trip.uploadTask == IPPARA_TASK_SENDING)
    { 
        /* �������õĶ�ȡDCU������DCU ��������Ҫ��ȡʱ������ֵΪ OK */
        bNext = FindNextDcu((int8u*) &uploadDCU_Trip.uploadDCUObjs, (int8u*) &uploadDCU_Trip.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            wtj_CapiStartTrans(FUNID_UPLOAD_DCUTRIP, uploadDCU_Trip.currentDCUID);
            uploadDCU_Trip.uploadTask         = IPPARA_TASK_WAITING;
            uploadDCU_Trip.uploadStartTick    = getSysTick();
        }
        else
        {
            /* ���� DCU ��ȡ��� */
            uploadDCU_Trip.uploadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. �ȴ����     *    �������Ƶȴ�ʱ��     *  */
    else if(uploadDCU_Trip.uploadTask == IPPARA_TASK_WAITING)
    {
        if((getSysTimeDiff(uploadDCU_Trip.uploadStartTick) > 100))
        {
            uploadDCU_Trip.uploadTask = IPPARA_TASK_SENDING;
            ClrTransState(FUNID_UPLOAD_DCUTRIP);
        }
        else
        {
            sts = wtj_ChkTransState(FUNID_UPLOAD_DCUTRIP);
            switch(sts)
            {
                case  ican_write_ing:
                    break;

                case  ican_write_OK:
                    //���ø�DCU ��ȡ�ɹ���־
                    SetpBufBit((int8u*) &uploadDCU_Trip.uploadTripResult, uploadDCU_Trip.currentDCUID - ID_DCU_MIN);

                    //�洢��DCU ��IP����
                    pSrc  = (int8u*)  uploadDCU_Trip.oneDCUTripData;
                    pDst = (int8u*)  uploadDCU_Trip.allDCU_Trip[uploadDCU_Trip.currentDCUID - ID_DCU_MIN];
                    for(j = 0; j < DCU_TRIPDATA_LEN; j ++)
                    {
                        *pDst++ = *pSrc++;
                    }

                    /* ��ȡ�ɹ������� */
                    uploadDCU_Trip.uploadCnt ++;

                case  ican_write_Err:
                default:
                    /* ��ȡ��һ�� DCU */
                    uploadDCU_Trip.uploadTask = IPPARA_TASK_SENDING;
                    break;
            }
        }
    }
}

/* =============================================================================
 * 2023-8-21
 *  ����ָ���� DCU ������ݣ���������
 *  */
void Can_DownDCU_TripData(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. ���������ʼ�� */
    if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_START)
    {
        //������ؽ����־
        for(i = 0; i<sizeof(DownloadDCU_Trip.downloadTripResult); i++)
        {
            DownloadDCU_Trip.downloadTripResult[i]  = 0;
        }

        //��ʼ����������
        DownloadDCU_Trip.downloadCnt  = 0;
        DownloadDCU_Trip.currentDCUID = ID_DCU_MIN - 1;       /* -1 Ϊ�˲��ҵ�һ��DCU */
        DownloadDCU_Trip.downloadTask = IPPARA_TASK_SENDING;
    }
    
    /* 2. ������������ */
    if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_SENDING)
    {
        /* �������õ�DCU������DCU ��������Ҫ����ʱ������ֵΪ OK */
        bNext = FindNextDcu((int8u*) &DownloadDCU_Trip.dcuObjects, (int8u*) &DownloadDCU_Trip.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            /* ׼�����͵����� */
            pDst = CANBus_downloadTrip;
            
            /* DCU ������� */
            pSrc = &DownloadDCU_Trip.dcuTripData[0];
            for(i=0; i<DCU_TRIPDATA_LEN; i++)
            {
                *pDst++ = *pSrc++;
            }
            
            /* ͨ�� CAN �������� IP ���� */
            wtj_CapiStartTrans(FUNID_DOWNLOAD_DCUTRIP, DownloadDCU_Trip.currentDCUID);
            DownloadDCU_Trip.downloadTask         = IPPARA_TASK_WAITING;
            DownloadDCU_Trip.downloadStartTick    = getSysTick();
            
            /* ���ؼ����� */
            DownloadDCU_Trip.downloadCnt++;
        }
        else
        {
            /* ���� DCU ������� */
            DownloadDCU_Trip.downloadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. �ȴ�������������Ƶȴ�ʱ��  *  */
    else if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_WAITING)
    {
        if((getSysTimeDiff(DownloadDCU_Trip.downloadStartTick) > 100))
        {
            DownloadDCU_Trip.downloadTask     = IPPARA_TASK_SENDING;
            ClrTransState(FUNID_DOWNLOAD_DCUTRIP);
        }
        else
        {
            sts = wtj_ChkTransState(FUNID_DOWNLOAD_DCUTRIP);
            switch(sts)
            {
                case  ican_read_OK:
                    /* ���ø�DCU ���سɹ���־ */
                    SetpBufBit((int8u*) &DownloadDCU_Trip.downloadTripResult, DownloadDCU_Trip.currentDCUID - ID_DCU_MIN);
                case  ican_read_Err:
                default:
                    /* ׼��������һ�� DCU */
                    DownloadDCU_Trip.downloadTask = IPPARA_TASK_SENDING;
                    break;

                case  ican_read_ing:
                    break;
            }
        }
    }
}


/* =============================================================================
 * CAN ����ͨ�ż�����
 * 
 */
PUBLIC void processCANComm(void)                
{
    procBroadcastTask();                    /* ���͹㲥���DCU */

    DcuCan12State(TRUE);                    //��ʱ ��DCU can12 ����Ϣ���ݸ� uniDcuData

    Can_UpOneDcuState();                    //����������� DCU N �� ,����� �����߱�־

    Can_Up_MoreDcuParameter();              //������DCU ����
    Can_Down_MoreDcuParameter();            //������ѡDCU����
    Can_UpOpenDoorCurve();                  //����������
    Can_UpCloseDoorCurve();                 //����������
    Can_QUERY();                            //��ѯDCU �����Ϣ


    Can_UploadDCU_IPPara();                 //2022-4-11 ������ָ�� DCU ��IP����
    Can_DownDCU_IPPara();                   //2022-4-11 ����ָ���� DCU IP����
    
    Can_UploadDCU_TripData();               //2023-8-21 ������ָ���� DCU �������
    Can_DownDCU_TripData();                 //2023-8-21 ����ָ���� DCU �������
}


