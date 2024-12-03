
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

    #ifdef DEBUG_PRINTF_ERR                       //定义则打印错误信息
    if((i == ican_read_Err) || (i == ican_write_Err))
        Printf_ERR(uFUNID, defcERR_SORT_00);
    #endif

    return i;
}

//读所有DCU 参数
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

    if(NextDcu == 1)  //查找下一个 DCU
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


//下载所选DCU参数
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

                //设置该DCU 下载成功标志
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

        //查找下一个 DCU
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

//读开门曲线
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

//读关门曲线
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

//读在线的DCU 状态【自动开门命令，列车进站等】
//如果读不到该 DCU N 次 ,则会置 不在线标志
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
                    /* 清除不在线DCU的信息，
                     * 会造成 MMS 界面上有关DCU显示内容闪烁，所以取消 */
                    for(j = 0; j < ONE_DCU_STATE_LEN; j ++)
                    {
                        *ptr1++ = 0;
                    }
                    #endif
                    DcuInc(&tagDcuState.uID_DCU, ID_DCU_MAX);
                }

                else
                {
                    //头几次失败就连续读
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
        /* 参考 NCRTC :
         * 在线DCU的数量数量越少，读DCU状态间隔的时间越长，避免DCU没有及时更新状态信息
         *                     DCU 数量           1,  2,  3,  4,  5,  6,  7,  8,  9+  */
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

                if(DcuIfOnLine(tagDcuState.uID_DCU) == cDcuOnLineF)   //只读取在线的 DCU
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

//查询 读取DCU state 数据的错误信息
//in:  id (0 -- n)
//out: = true 表示错误次数超出设置范围。 = false 表示错误次数在设置范围之内
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

//清除缓存
void ClsReadDcuStateFailBuf(int8u id)
{
    ReadDcuStateFailBuf[id] = 0;
}

//查询DCU 软件信息
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
 * 广播命令管理
 * 
 */

/* 添加广播命令 
 * 输入
 *  command     : 广播命令数据结构体
 *                此函数自动计算广播命令数据的CRC
 *  cmdNum      : 广播命令类型
 *  sendAtOnce  : 立即发送标志
 *               TRUE， 则立即添加广播发送任务
 *               FALSE，则判断连续2次待发送命令相同时的时间间隔，再决定是否启动添加到广播发送任务
 * 返回
 *  TRUE        : 添加成功
 *  FALSE       : 添加失败，失败的原因
 *                  - 是命令类型错误
 *                  - 连续两次相同命令的时间间隔不足
 * 注意
 *  同一类型的广播命令只能有一个
 */
PRIVATE BOOL addBroadCmdTask_Com(tBroadCmdPkg *command, enmBroadCmdType cmdType, BOOL sendAtOnce)
{
    unsigned short CRC;
    if(cmdType < BROAD_CMD_NUM)
    {
        #if 0
        /* 这个条件判断的作用：如果同一个类型的广播命令正在发送中，则添加失败 
         * 若跳过此条件判断，则同一类型的命令，
         *   1-正在发送，在广播发送完成后，会继续发送此命令
         *   2-没有发送，则新的广播命令会覆盖未发送的命令
         */
        if((CANSendBroadCmdManage.isBroading != 0) && ( CANSendBroadCmdManage.BroadingType == cmdType))
        {
            return FALSE;
        }
        #endif
        
        /* 此命令类型发送的时间间隔，若小于设定值
         * 需要： 
         *   本次待发送的是否与上次发送的比较
         *   1. 若不相同，则立即启动发送
         *   2. 若相同，  则根据应用层的设置, sendAtOnce = TRUE 立即发送 
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
            
            /* 相同命令 
             * 且不需要立即发送，则退出
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

/* 根据发送内容及时间间隔，添加一个待发送的广播命令，
 * 参阅 addBroadCmdTask_Com 的参数说明
 *  */
PUBLIC BOOL addBroadCmdTask(tBroadCmdPkg *command, enmBroadCmdType cmdType)
{    
    return addBroadCmdTask_Com(command, cmdType, FALSE);
}

/* 立即添加一个待发送的广播命令，
 * 参阅 addBroadCmdTask_Com 的参数说明
 *  */
PUBLIC BOOL addBroadCmdTask_AtOnce(tBroadCmdPkg *command, enmBroadCmdType cmdType)
{
    return addBroadCmdTask_Com(command, cmdType, TRUE);
}

/* 
 * 执行广播发送命令，间隔一定时间按设定的次数重复发送
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

/* 准备发送广播命令
 * 将待发送的广播命令复制到发送区，启动发送 
 */
PRIVATE void readyToBroadcast(int num)
{
    CANSendBroadCmdPkg = broadCmdList[num].cmdFrame;
    
    CANSendBroadCmdManage.Tx_intervals = BROADCMD_INTERVAL_MS;  /* 快速启动发送，不需要等待时间间隔 */
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

/* 设置发送广播命令的参数
 * 如果主程序没有设置，则采用默认值
 * 若设置参数 = 0，则软件检测到以后将用默认值重新设置 (readyToBroadcast)
 */
PUBLIC void setRepeatPara(int8u repeatTimes, int8u repeatInterval)
{
    CANSendBroadCmdManage.setRepeatTimes = repeatTimes;
    CANSendBroadCmdManage.setInterval_ms = repeatInterval;
}


#if 0
/* 测试发送给DCU广播命令 
 * 通过不同指令的组合，测试对DCU本地命令字的操作，并分析DCU的反馈判断操作是否正确
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

/* 需要在主程序中轮流调用
 * 轮流扫描待发送的广播命令
 * 每个广播命令发送完成以后，查找下一个需要发送的广播命令
 */
PUBLIC void procBroadcastTask(void)
{
    static int8u broadCmdTurn = 0;      /* 上一次发送广播命令的顺序号 */
    static int16u prevTick = 0;
    static int16u prevTickMs = 0;
    
    int16u i;
    int16u intervalTemp;
    
    /* 处理发送时间间隔 */
    intervalTemp = (int16u)(TgtGetTickCount() - prevTickMs);
    prevTickMs = TgtGetTickCount();
    
    i = (int16u)CANSendBroadCmdManage.Tx_intervals + intervalTemp;
    if(i > 255)
    {
        i = 255;
    }
    CANSendBroadCmdManage.Tx_intervals = i;
    
    /* 计时：用于控制同一个命令多次发送 */
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
    /* 测试发送给DCU广播命令 */
    testBroadcastCmd();    
    #endif
    
    /* 广播命令发送管理 */
    if(CANSendBroadCmdManage.isBroading == 0)
    {
        /* 没有发送广播命令
         * 查找是否有待发送的广播命令
         * 发现待发送的广播命令后，启动发送任务并退出
         * 
         */
        for(; broadCmdTurn < BROAD_CMD_NUM; broadCmdTurn++)
        {
            if(broadCmdList[broadCmdTurn].startSend > 0)
            {
                broadCmdList[broadCmdTurn].startSend = 0;
                readyToBroadcast(broadCmdTurn);
                
                /* 下一次查找的位置 */
                broadCmdTurn++;
                break;
            }
        }
        
        if(broadCmdTurn >= BROAD_CMD_NUM)
        {
            broadCmdTurn = 0;
        }
    }
    
    /* 执行广播发送命令 */
    doBroadcastTask();
}


/* =============================================================================
 * 2022-4-12
 *  批量读指定 DCU 的IP参数
 *  */
void Can_UploadDCU_IPPara(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. 启动任务初始化 */
    if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_START)
    {
        /* 重置读取结果 */
        for(i = 0; i<sizeof(uploadDCU_IPPara.uploadDCUObjs); i++)
        {
            uploadDCU_IPPara.uploadIPResult[i]  = 0;
        }

        /* 清除所有的结果存储区 */
        pDst = (int8u*)uploadDCU_IPPara.allDCU_IPPara;
        for(i = 0; i < sizeof(uploadDCU_IPPara.allDCU_IPPara); i ++)
        {
            *pDst ++ = 0;
        }

        uploadDCU_IPPara.uploadCnt          = 0;
        uploadDCU_IPPara.currentDCUID       = ID_DCU_MIN - 1;
        uploadDCU_IPPara.uploadTask         = IPPARA_TASK_SENDING;
    }
    
    /* 2. 发送命令 */
    if(uploadDCU_IPPara.uploadTask == IPPARA_TASK_SENDING)
    { 
        /* 查找设置的读取DCU，若该DCU 在线且需要读取时，返回值为 OK */
        bNext = FindNextDcu((int8u*) &uploadDCU_IPPara.uploadDCUObjs, (int8u*) &uploadDCU_IPPara.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            wtj_CapiStartTrans(FUNID_UPLOAD_DCUIPPARA, uploadDCU_IPPara.currentDCUID);
            uploadDCU_IPPara.uploadTask         = IPPARA_TASK_WAITING;
            uploadDCU_IPPara.uploadStartTick    = getSysTick();
        }
        else
        {
            /* 所有 DCU 读取完成 */
            uploadDCU_IPPara.uploadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. 等待结果     *    这里限制等待时间     *  */
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
                    //设置该DCU 读取成功标志
                    SetpBufBit((int8u*) &uploadDCU_IPPara.uploadIPResult, uploadDCU_IPPara.currentDCUID - ID_DCU_MIN);

                    //存储该DCU 的IP参数
                    pSrc  = (int8u*)  uploadDCU_IPPara.oneDCUIPPara;
                    pDst = (int8u*)  uploadDCU_IPPara.allDCU_IPPara[uploadDCU_IPPara.currentDCUID - ID_DCU_MIN];
                    for(j = 0; j < ONE_DCU_IPPARA_BUFLEN; j ++)
                    {
                        *pDst++ = *pSrc++;
                    }

                    /* 读取成功计数器 */
                    uploadDCU_IPPara.uploadCnt ++;

                case  ican_write_Err:
                default:
                    /* 读取下一个 DCU */
                    uploadDCU_IPPara.uploadTask = IPPARA_TASK_SENDING;
                    break;
            }
        }
    }
}



/* =============================================================================
 * 2022-4-12
 *  批量下载指定的 DCU IP参数，需要轮流下载
 *  */
void Can_DownDCU_IPPara(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. 启动任务初始化 */
    if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_START)
    {
        //清除下载结果标志
        for(i = 0; i<sizeof(DownloadDCU_IPPara.downloadIPResult); i++)
        {
            DownloadDCU_IPPara.downloadIPResult[i]  = 0;
        }

        //初始化下载任务
        DownloadDCU_IPPara.downloadCnt  = 0;
        DownloadDCU_IPPara.currentDCUID = ID_DCU_MIN - 1;       /* -1 为了查找第一个DCU */
        DownloadDCU_IPPara.downloadTask = IPPARA_TASK_SENDING;
    }
    
    /* 2. 发送命令数据 */
    if(DownloadDCU_IPPara.downloadTask == IPPARA_TASK_SENDING)
    {
        /* 查找设置的DCU，若该DCU 在线且需要下载时，返回值为 OK */
        bNext = FindNextDcu((int8u*) &DownloadDCU_IPPara.dcuObjects, (int8u*) &DownloadDCU_IPPara.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            /* 准备发送的数据 */
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
            
            /* 通过 CAN 发送下载 IP 参数 */
            wtj_CapiStartTrans(FUNID_DOWNLOAD_DCUIPPARA, DownloadDCU_IPPara.currentDCUID);
            DownloadDCU_IPPara.downloadTask         = IPPARA_TASK_WAITING;
            DownloadDCU_IPPara.downloadStartTick    = getSysTick();
            
            /* 下载计数器 */
            DownloadDCU_IPPara.downloadCnt++;
        }
        else
        {
            /* 所有 DCU 下载完成 */
            DownloadDCU_IPPara.downloadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. 等待结果，这里限制等待时间  *  */
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
                    /* 设置该DCU 下载成功标志 */
                    SetpBufBit((int8u*) &DownloadDCU_IPPara.downloadIPResult, DownloadDCU_IPPara.currentDCUID - ID_DCU_MIN);
                case  ican_read_Err:
                default:
                    /* 准备下载下一个 DCU */
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
 *  批量读指定 DCU 的里程数据
 *  */
void Can_UploadDCU_TripData(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. 启动任务初始化 */
    if(uploadDCU_Trip.uploadTask == IPPARA_TASK_START)
    {
        /* 重置读取结果 */
        for(i = 0; i<sizeof(uploadDCU_Trip.uploadDCUObjs); i++)
        {
            uploadDCU_Trip.uploadTripResult[i]  = 0;
        }

        /* 清除所有的结果存储区 */
        pDst = (int8u*)uploadDCU_Trip.allDCU_Trip;
        for(i = 0; i < sizeof(uploadDCU_Trip.allDCU_Trip); i ++)
        {
            *pDst ++ = 0;
        }

        uploadDCU_Trip.uploadCnt        = 0;
        uploadDCU_Trip.currentDCUID     = ID_DCU_MIN - 1;
        uploadDCU_Trip.uploadTask       = IPPARA_TASK_SENDING;
    }
    
    /* 2. 发送命令 */
    if(uploadDCU_Trip.uploadTask == IPPARA_TASK_SENDING)
    { 
        /* 查找设置的读取DCU，若该DCU 在线且需要读取时，返回值为 OK */
        bNext = FindNextDcu((int8u*) &uploadDCU_Trip.uploadDCUObjs, (int8u*) &uploadDCU_Trip.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            wtj_CapiStartTrans(FUNID_UPLOAD_DCUTRIP, uploadDCU_Trip.currentDCUID);
            uploadDCU_Trip.uploadTask         = IPPARA_TASK_WAITING;
            uploadDCU_Trip.uploadStartTick    = getSysTick();
        }
        else
        {
            /* 所有 DCU 读取完成 */
            uploadDCU_Trip.uploadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. 等待结果     *    这里限制等待时间     *  */
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
                    //设置该DCU 读取成功标志
                    SetpBufBit((int8u*) &uploadDCU_Trip.uploadTripResult, uploadDCU_Trip.currentDCUID - ID_DCU_MIN);

                    //存储该DCU 的IP参数
                    pSrc  = (int8u*)  uploadDCU_Trip.oneDCUTripData;
                    pDst = (int8u*)  uploadDCU_Trip.allDCU_Trip[uploadDCU_Trip.currentDCUID - ID_DCU_MIN];
                    for(j = 0; j < DCU_TRIPDATA_LEN; j ++)
                    {
                        *pDst++ = *pSrc++;
                    }

                    /* 读取成功计数器 */
                    uploadDCU_Trip.uploadCnt ++;

                case  ican_write_Err:
                default:
                    /* 读取下一个 DCU */
                    uploadDCU_Trip.uploadTask = IPPARA_TASK_SENDING;
                    break;
            }
        }
    }
}

/* =============================================================================
 * 2023-8-21
 *  下载指定的 DCU 里程数据，轮流下载
 *  */
void Can_DownDCU_TripData(void)
{
    int i,j;
    int8u sts, bNext;
    int8u* pSrc;
    int8u* pDst;
    
    /* 1. 启动任务初始化 */
    if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_START)
    {
        //清除下载结果标志
        for(i = 0; i<sizeof(DownloadDCU_Trip.downloadTripResult); i++)
        {
            DownloadDCU_Trip.downloadTripResult[i]  = 0;
        }

        //初始化下载任务
        DownloadDCU_Trip.downloadCnt  = 0;
        DownloadDCU_Trip.currentDCUID = ID_DCU_MIN - 1;       /* -1 为了查找第一个DCU */
        DownloadDCU_Trip.downloadTask = IPPARA_TASK_SENDING;
    }
    
    /* 2. 发送命令数据 */
    if(DownloadDCU_Trip.downloadTask == IPPARA_TASK_SENDING)
    {
        /* 查找设置的DCU，若该DCU 在线且需要下载时，返回值为 OK */
        bNext = FindNextDcu((int8u*) &DownloadDCU_Trip.dcuObjects, (int8u*) &DownloadDCU_Trip.currentDCUID, MAX_DCU);

        if(bNext == FindOk)
        {
            /* 准备发送的数据 */
            pDst = CANBus_downloadTrip;
            
            /* DCU 里程数据 */
            pSrc = &DownloadDCU_Trip.dcuTripData[0];
            for(i=0; i<DCU_TRIPDATA_LEN; i++)
            {
                *pDst++ = *pSrc++;
            }
            
            /* 通过 CAN 发送下载 IP 参数 */
            wtj_CapiStartTrans(FUNID_DOWNLOAD_DCUTRIP, DownloadDCU_Trip.currentDCUID);
            DownloadDCU_Trip.downloadTask         = IPPARA_TASK_WAITING;
            DownloadDCU_Trip.downloadStartTick    = getSysTick();
            
            /* 下载计数器 */
            DownloadDCU_Trip.downloadCnt++;
        }
        else
        {
            /* 所有 DCU 下载完成 */
            DownloadDCU_Trip.downloadTask = IPPARA_TASK_FINISHED;
        }
    }
    /* 3. 等待结果，这里限制等待时间  *  */
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
                    /* 设置该DCU 下载成功标志 */
                    SetpBufBit((int8u*) &DownloadDCU_Trip.downloadTripResult, DownloadDCU_Trip.currentDCUID - ID_DCU_MIN);
                case  ican_read_Err:
                default:
                    /* 准备下载下一个 DCU */
                    DownloadDCU_Trip.downloadTask = IPPARA_TASK_SENDING;
                    break;

                case  ican_read_ing:
                    break;
            }
        }
    }
}


/* =============================================================================
 * CAN 总线通信及管理
 * 
 */
PUBLIC void processCANComm(void)                
{
    procBroadcastTask();                    /* 发送广播命令到DCU */

    DcuCan12State(TRUE);                    //定时 将DCU can12 的信息传递给 uniDcuData

    Can_UpOneDcuState();                    //如果读不到该 DCU N 次 ,则会置 不在线标志

    Can_Up_MoreDcuParameter();              //读所有DCU 参数
    Can_Down_MoreDcuParameter();            //下载所选DCU参数
    Can_UpOpenDoorCurve();                  //读开门曲线
    Can_UpCloseDoorCurve();                 //读关门曲线
    Can_QUERY();                            //查询DCU 软件信息


    Can_UploadDCU_IPPara();                 //2022-4-11 批量读指定 DCU 的IP参数
    Can_DownDCU_IPPara();                   //2022-4-11 下载指定的 DCU IP参数
    
    Can_UploadDCU_TripData();               //2023-8-21 批量读指定的 DCU 里程数据
    Can_DownDCU_TripData();                 //2023-8-21 下载指定的 DCU 里程数据
}


