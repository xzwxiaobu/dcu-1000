
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

//cDcuOnLineF 在线 ，cDcuOffLineF 离线
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

//cDcuOnLineF 在线 ，cDcuOffLineF 离线
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

//BITEQU1 测试位 == 1 ，BITEQU0 测试位 == 0
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

//该位置是否存在 DCU
//node_No , 1 ---
//存在 return 1 ; 不存在 return 0
int8u NodeJudge(int8u Id)
{
     return ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF , Id);
}

//查找下一个 在线 DCU ,找到 在线DCU 为止，如果循环了一次，则返回 FindFail （相当于找到最大DCU，再从头找）
//DcuNum DCU数量 1 = 1
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

//DCU 加一 ，超出最大值循环到最小值
//DcuNum DCU数量 1 = 1
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
 * 将DCU can 1和2 的信息传递给 uniDcuData
 * 
 * DCU can1, can2 是否在线, b0-dcu1 ,b1-dcu2... ,1错误
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
 * 辅助功能函数
 * */
//查找缓存区第一位不为0的序号
//Len 字节数
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

//查找缓存区最后一位不为0的序号
//Len 字节数
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

/* 计算BUFFER中1的总数 
 * 前提条件：设置是连续的，且是从第一个位开始
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
/* 收集 DCU 状态 AlarmParaTxToAvr
 * 每个DCU上传的信息共24Bytes，Byte0~9是 该DCU的状态信息，请参阅相关协议文件
 */
PUBLIC void AlarmParaTxToAvr(void)
{
    int8u i,k, temp = 0;
    tDownMsg DownMsg_Tmp;

    tDCU_AllStsFlags *ptrDcuAllSts;
    
    DownMsg_Tmp.word = 0;

    int8u DCU = ID_DCU_MIN ;
    int8u Door_OPN = 1;                 /*   0 = 只要有一个DCU离线或故障，收不到全开启信息*/
    int8u OnLineDcuCnt = 0;             /*   DCU在线计数器 */
    int8u ALL_DOOR_MANUAL_MODE = 1;     /*   1 所有门都在手动模式*/ 
    int8u DCU_No = 0xff;                /*   从0开始， 的DCU序号（包含在线和不在线）*/

    #if 1
    int8u first_DCU;   /*0 开始*/
    int8u last_DCU;    /*0 开始*/
    first_DCU = findFirstBit( (pINT8U)&uDCUsetting.settingBUF, sizeof(uDCUsetting.settingBUF));
    last_DCU  = findLastBit( (pINT8U)&uDCUsetting.settingBUF, sizeof(uDCUsetting.settingBUF));
    #endif

    for(i = 0; i < MAX_DCU; i ++)
    {
        /* 此编号设置有效？ */
        if(ptrDcuIfOnLine((pINT8U)&uDCUsetting.settingBUF, DCU) == BITEQU1)
        {
            DCU_No ++;  
                    
            if(DCU_No > cDefault_DCU_NUM)
                break;       /*保护用*/

            if(DcuIfOnLine(DCU) == cDcuOnLineF)
            {
                OnLineDcuCnt++;     /* DCU 在线计数器 +1 */
                
                /* DCU 上传的状态标志 */
                ptrDcuAllSts = (tDCU_AllStsFlags *)(&uniDcuData.sortBuf.aDcuState[i][0]);
                
                /* 开门故障? */
                if(ptrDcuAllSts->err_open_door)
                {
                    DownMsg_Tmp.bOPN_FAULT = 1;
                }
                
                /* 关门故障? */
                if(ptrDcuAllSts->err_close_door)
                {
                    DownMsg_Tmp.bCLS_FAULT = 1;
                }
                 
                /* 滑动门故障?
                 * 开门故障 | 关门故障 | 安全回路故障 */
                if(ptrDcuAllSts->err_open_door | ptrDcuAllSts->err_close_door | ptrDcuAllSts->err_saft_loop_flag)
                {
                    DownMsg_Tmp.bPSD_FAULT = 1;
                }
                
                if(ptrDcuAllSts->LCB_Auto)
                {
                    /*自动*/
                    ALL_DOOR_MANUAL_MODE = 0;
                }   
                else
                {
                    /* 隔离或手动*/
                    if(ptrDcuAllSts->LCB_Isolate)
                    {
                        DownMsg_Tmp.bIsolation_MODE = 1; /*隔离*/
                        ALL_DOOR_MANUAL_MODE = 0;
                    }
                    else
                    {
                        DownMsg_Tmp.bMANUAL_MODE = 1;    /*手动操作*/
                    }   
                }

                /*取门状态*/
                switch(ptrDcuAllSts->DoorMode)
                {
                    case DOOR_STATE_UNLOCK:                 /*手动解锁*/
                        DownMsg_Tmp.bUNLOCK = 1;       
                        break;
                    case DOOR_STATE_CLOSING:                /*关门过程*/
                        DownMsg_Tmp.bClosing = 1;       
                        break;
                    case DOOR_STATE_OPENING:                /*开门过程*/
                        DownMsg_Tmp.bOpening = 1;      
                        break;
                        
                    case DOOR_STATE_OPEN:                   /* 门已开 */
                        /* 首门 */
                        if(i == first_DCU)
                        {
                            DownMsg_Tmp.bHeadASD_Opned = 1;
                        }
                        /* 尾门 */
                        if(i == last_DCU)
                        {
                            DownMsg_Tmp.bTailASD_Opned = 1;
                        }   
                        break;
                        
                    case DOOR_STATE_CLOSING1:               /* 关门遇障处理中 */
                    case DOOR_STATE_OPENING1:               /* 开门遇障处理中 */
                        break;
                }
                
                //if(temp1.bMANUAL_MODE != 1) /*1手动操作*/
                {
                    if(ptrDcuAllSts->DoorMode != DOOR_STATE_OPEN)   /*是否开门状态*/
                    {
                        Door_OPN = 0;               /*门全开启（只要有一个DCU离线或故障，收不到全开启信息，都不亮）*/
                    }
                }   
                
                /* 每个ASD有2个 EED */
                /* 应急门扇1(右)打开? 应急门扇2(左)?*/
                if(ptrDcuAllSts->Door_EED1_Switch_R || ptrDcuAllSts->Door_EED2_Switch_L)
                {
                    DownMsg_Tmp.bEmergenrydoor_opn = 1;    
                }
                
                /* 端门打开? */
                if(ptrDcuAllSts->MSD1_Open_Switch || ptrDcuAllSts->MSD2_Open_Switch) 
                {
                    DownMsg_Tmp.bMSD_OPEN = 1; 
                }
                
                #if 0
                /* 信号禁止开门? */
                if(ptrDcuAllSts->STC_Inhibit)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Iso_Feedback[0] , DCU_No); 
                }
                
                /* 信号远程使能? */
                if(ptrDcuAllSts->Remote_Enable_Command_1 || ptrDcuAllSts->Remote_Enable_Command_2)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ATSRemoteEn[0] , DCU_No);
                }        
                
                /* 信号远程开门? */
                if(ptrDcuAllSts->Remote_Open_Command)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Open_Feedback[0] , DCU_No); 
                }
                
                /* 信号远程关门? */
                if(ptrDcuAllSts->Remote_Close_Command)
                {
                    /*Remote close*/
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_Close_Feedback[0] , DCU_No); 
                }
                
                /* 关闭锁紧? */
                if(ptrDcuAllSts->C_L_Switch_Status)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_ClosedAndLocked[0] , DCU_No); 
                }
                
                /* 非法开门? */
                if(ptrDcuAllSts->illegal_Open)
                {
                    SetpBufBit((pINT8U)&PDS_messagebuf.reg_ASD_IllegalOpen[0] , DCU_No); 
                }
                
                /*首端门*/
                if(i == first_DCU)
                {
                    if(ptrDcuAllSts->MSD1_Open_Switch) 
                    {/*首端门关闭*/
                        DownMsg_Tmp.bMSD1_OPEN = 1;
                    }
                }   
                /*尾端门*/
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
                DownMsg_Tmp.bBUS_FAULT = 1;    /* CAN总线故障*/
                clr_CLSCLK(DCU);
            }
        }

        DCU ++;
    }

    /*
    OnLineDcuCnt > 0         : 至少有一个DCU在线
    ALL_DOOR_MANUAL_MODE = 1 : 所有门都在手动模式 
    部分门单元LCB位于手动模式时，本滑动门全开和全关闭信号被忽略；
    整侧门单元LCB位于手动模式时，整侧只显示站台滑动门全关闭信号
    */
    if((Door_OPN == 1) && (OnLineDcuCnt != 0) && (ALL_DOOR_MANUAL_MODE != 1))
    {
        DownMsg_Tmp.bDOOR_ALL_OPEN = 1;      /*门全开启*/
    }

    /*PEDC 和 MMS 通讯超时 : 设置与 MMS 连接状态错误标志 MONITOR_FAULT */
    DownMsg_Tmp.bMONITOR_FAULT = (getUartMMSLinkSts() == 0)? 1 : 0;

    if(tagDownIbpAlarmParameter.uIbpAlarmParameter.byte != Down_Message1.byte0)  //报警允许如有修改，则清报警的备份
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
 * 收集 DCU 滑动门状态，得到站台门对位隔离列车门的信息
 *  */
PRIVATE int8u DCUOnLineSts[(MAX_DCU+7)/8];

/* 检测是否所有的DCU都在线或不在线，根据输入参数设置相应位表示 在线或不在线
 * 输入参数
 *  DCUs : DCU总数
 *  chkStatus
 *      = cDcuOnLineF 表示检查在线， 如果在线，对应位设置为 1
 *      = cDcuOffLineF 表示检查离线，如果离线，对应位设置为 1
 *  setting : 配置的DCU
 *      对应位为1表示该DCU设置为有效，0表示无效，不会检查无效DCU在线状态
 * 返回：
 *  结果数据存储的指针
 * 
 * 注意：需要考虑实际设置的DCU (uDCUsetting.settingBUF)
 *       对于屏蔽的DCU，对应位都是 0
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
 * 根据 DCU 滑动门状态，得到站台门对位隔离列车门的信息
 * 输入
 *  pInhibit : 返回数据指向的存储区
 *  len      : 存储区长度
 * 输出
 *  保存到 pInhibit 存储区
 * 返回
 *  TRUE 表示成功
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
    
    /* 所有DCU的在线状态，如果在线，对应位设置为 1 */
    getAllDCUOnLineSts(MAX_DCU, cDcuOnLineF, (int8u*)&uDCUsetting.settingBUF);
    
    /* 滑动门状态 */
    for(i=0; i<MAX_DCU; i++)
    {
        pDcuStsFlags = (tDCU_AllStsFlags*)(&uniDcuData.sortBuf.aDcuState[i]);
        
        /* SZ20YQ 待明确
         * 南京7，当出现以下情况之一，定义为站台门故障，回复给信号系统：
         * LCB隔离档位，                                 BYTE 0.6 = 1 表示LCB隔离模式
         * LCB关门档位，                                 BYTE 0.7 = 0 手动模式
         * LCB开门档位，                                 BYTE 0.7 = 0 手动模式
         * LCB在自动档位，且站台门故障，开关门功能失效。 BYTE 5.0~3 DCU模式 = 5 (停止功能操作（故障及报警）)
         */
        if(pDcuStsFlags->LCB_Isolate || (pDcuStsFlags->LCB_Auto == 0) || (pDcuStsFlags->DCUMode == DCU_STATE_STOP))
        {
            SetRamBit(result, i);
        }
    }
    
    /* 离线的DCU处理 */
    for(i=0; i<sizeof(DCUOnLineSts); i++)
    {
        result[i] &= DCUOnLineSts[i];               /* 离线的DCU不报故障 */
        result[i] &= uDCUsetting.settingBUF[i];     /* 根据设置的DCU屏蔽相应的隔离信息 */
    }
    
    if(len > sizeof(result))
        len = sizeof(result);
    
    for(i=0; i<len; i++)
    {
        *pInhibit ++ = result[i];
    }
    
    return TRUE;
}






