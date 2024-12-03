/*=============================================================================
 * Project:     PEDC 通用功能
 * PEDC MCU:        PIC32MX795F512H
 * Fcrystal:        4M
 * PLL:         X20
 * FCY:         SYSTEM_FREQ = 80M
 *
 * Version:     2.0
 * Author:      lly
 * Date:            2022-3-28
 * =============================================
 * 文    件 :   UpApiMst.C
 *          :   通过CAN总线升级，MASTER 端
 * 
 *  -
 *          增加对升级 IBP PSL SIG板的升级功能
 *  - 2022-4-7
 *          增加对批量升级 DCU 的支持
 *
 * =============================================================================*/
#if ((UPDATE_FUN_EN == TRUE))
#include  "_cpu.h"

#include  "_g_canbus.h"
#include "glbCan.h"
#include "glbUpdate.h"
#include "_glbApi.h"

#include "Update.h"
#include "..\ObjDict.h"
#include "CanTgt.h"
#include "CanDrv.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

#define NODATA_ASK_MORE             //升级过程中指定时间内没有数据, PEDC请求数据(UPFW_STATE_SEC_ERR)
#define BATCH_UPDATE_DCUS           //批量升级 DCU 功能
//#define TEST_BAT_UPDATE_DCU       //调试批量升级DCU

/* 支持在线升级 IBP PSL SIG 板
 */
/* 串口升级回调函数接口 */
uartUpdate_UartTxPkg        cbUartTxPkg;
uartUpdate_UartTxBusy       cbUartTxBusy;       /* Uart 发送是否空闲：FALSE 表示空闲，TRUE 表示忙 */
uartUpdate_QuerySlave       cbQuerySlaveSts;    /* Uart 查询升级状态 */
uartUpdate_ChkOnline        cbChkOnline;        /* Uart 查询 在线状态 */
uartUpdate_GetSlaveReply    cbGetSlaveReply;    /* Uart 读从机回复信息 */

static void UpdateQuerySlave(int8u id);

extern void UPDATE_FIRMWAREStateToMMS(void); //wtj -> uart.c
extern BOOL chkFinishTxToMMS(void);
extern int8u CapiStartTrans(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);
    
//---------------------------------------------------------------------------
int8u __attribute__ ((aligned(4))) FileDataBuf[UPFW_CODE_BUF_MAX];  //首地址必须对齐到 int32u

int8u u8UpdateState;                 //发给 MMS 的状态, 包含数据传输错误或者是 Slave 返回的状态
int8u CmdBuf[8];                     //发送扩展命令用, 不能用临时变量, 因为发送失败以后会重新发送一次

tUpdateMsg UpdateMsg = {{0},0};
int16u wTickUpdateSection;
int8u u8StartUpgrade  = FALSE;

/* 启动升级命令标志，
 * DCU 收到启动升级命令后需要执行擦除 FLASH 的任务耗时长
 * PEDC可能需要等待较长时间才能查询到 DCU 的执行结果
 *  */
int8u u8StartCommand  = FALSE;      

int16u wUpFwSection;                //来自 MMS 的升级数据包含的段号

#ifdef  NODATA_ASK_MORE
int8u   u8AskMoreCnt = 0;
#endif

//---------------------------------------------------------------------------
#ifdef BATCH_UPDATE_DCUS
typedef struct
{
    int8u   state;                          /* DCU回复的升级状态，是为了对齐 */
    int8u   rsv;                            /* 保留，对齐 */
    int16u  section;                        /* DCU升级的段号     */
}tUpdateDcuPro;
tUpdateDcuPro updateDcuSts[MAX_DCU];

BOOL   batchUpdateDCUs  = FALSE;            /* TRUE 表示批量升级使能    */
BOOL   sendUpdatePkg    = FALSE;            /* TRUE = 升级数据包已发送  */
int8u  updateDCUSetting[BYTES_SLV_FLAG];    /* 设置待升级的DCU, 位值1对应的DCU需要升级 */

int8u  updateDCUID;                         /* 当前查询DCU的ID号 */

int16u updatePkgSize;                       /* MMS 下传的升级数据包的长度 */
int8u  updateDataPkg[UPFW_CODE_BUF_MAX];    /* MMS 下传的升级数据包数据 */

/* 获取批量升级状态
 *  */
BOOL ifBatchUpdateDCUs(void)
{
    return batchUpdateDCUs;
}
    
int8u* getBatchUpdateDCUsSetting(void)
{
    return updateDCUSetting;
}

/* 获取回复MMS时，DCU的升级状态，在 UPDATE_FIRMWAREStateToMMS 函数中调用
 *  pData : 回复数据存储缓存区
 *  bufLen: pData指向的缓冲区长度
 * 返回回复数据的长度，0 表示参数有问题
 *  */
int getReplyDCUUpdateState(int8u* pData, int bufLen)
{
    int len = 0;
    
    #if 0
    /* 单独处理 PEDC请求MMS重发数据? */
    if(u8UpdateState == UPFW_STATE_ASKFORDATA)
    {   
        *pData++ = u8UpdateState;
        len      = 1;
    }
    else
    #endif
    {
        /* 批量升级DCU时，回复 MMS 信息
         * 第一个字节是批量升级标志，0x80 表示批量升级, 0 表示不是批量升级
         *  */
        *pData++ = (batchUpdateDCUs) ? 0x80 : 0;

        len = 1 + sizeof(updateDCUSetting) + sizeof(updateDcuSts);
        if(len > bufLen)
        {
            return 0;
        }

        /* 批量升级DCU，回复的信息是
         * 1. 批量升级标志
         * 2. 设置
         * 3. 每个DCU 升级：状态和段号 */
        if(batchUpdateDCUs)
        {
            TgtMemCpy(pData, &updateDCUSetting, sizeof(updateDCUSetting));

            TgtMemCpy(pData +  sizeof(updateDCUSetting), &updateDcuSts, sizeof(updateDcuSts));
        }
        else
        {   /* 批量升级，但是没有提前设置升级DCU */
            TgtMemSet(pData, 0, sizeof(updateDCUSetting));
            TgtMemSet(pData +  sizeof(updateDCUSetting), UPFW_STATE_NOSETTING_DCU, sizeof(updateDcuSts));
        }
    }

    return len;
}
    
    
/* 设置待批量升级的DCU，位为1所对应的DCU需要升级，字节0位0 对应 DCUID为1
 * 若所有值都为0，则清除批量升级
 * 返回 TRUE 表示设置成功，批量升级DCU功能已启动
 *  */
BOOL setBatchcastUpdateDCUs(int8u * settings)
{
    int i;
    TgtMemCpy(updateDCUSetting, settings, sizeof(updateDCUSetting));

    batchUpdateDCUs = FALSE;
    sendUpdatePkg   = FALSE;

    /* 设置批量升级的DCU，如果没有值，则不认为是批量升级
     *  */
    for(i=0; i<sizeof(updateDCUSetting); i++)
    {
        if(updateDCUSetting[i] != 0)
        {
            updateDCUID         = ID_DCU_MIN;
            batchUpdateDCUs     = TRUE;
            break;
        }
    }

    /* 重置所有DCU升级状态 */
    for(i=0; i<MAX_DCU; i++)
    {
        updateDcuSts[i].state   = UPFW_STATE_NOUPDATE;
        updateDcuSts[i].section = 0;
    }

    return batchUpdateDCUs;
}

/* 取消批量升级 DCU */
BOOL clrBatchcastUpdateDCUs(void)
{
    batchUpdateDCUs = FALSE;
}

/* 搜索第一个待升级的DCU ID (ID值是从1开始)，即设置有效且在线
 * 返回值是下一个DCUID，若 > MAX_DCU 表示搜索完毕
 *  */
int8u searchFirstUpdateDCUID(void)
{
    int i;

    for(i = 0; i<MAX_DCU; i++)
    {
        if((ChkRamBit(updateDCUSetting, i) != 0) && (ChkSlaveOnLine(i + ID_DCU_MIN) == TRUE))
        {
            break;
        }
    }

    return (i + ID_DCU_MIN);
}

#endif
    
/* 初始化串口回调函数 */
PUBLIC BOOL InitUartUpdateCallBack(uartUpdate_UartTxPkg cbTxPkg, uartUpdate_UartTxBusy cbTxBz, uartUpdate_QuerySlave cbQuerySts, uartUpdate_ChkOnline chkOnline, uartUpdate_GetSlaveReply cbGetReply)
{
    cbUartTxPkg     = cbTxPkg;
    cbUartTxBusy    = cbTxBz;
    cbQuerySlaveSts = cbQuerySts;
    cbChkOnline     = chkOnline;
    cbGetSlaveReply = cbGetReply;
}

/* 根据对象ID，将对象分为4类
 * 主控板 ： 与 MMS 直接通信的板子，如 MONITOR，        PIC32MX795F512H or PIC32MK1024MCM100
 * 扩展板 :  与 主控板通信的板子，如 IBP PSL SIG ...， PIC32MX795F512H
 * PIC24  ： 目前仅限于 主控板板载的PIC24在线升级，     PIC24FJ32GA
 * DCU    :  分为批量升级或单个升级                   PIC30F6010A or PIC32MK1024MCM100
 * 
 *  */
PUBLIC int8u getUpdateType(int8u id)
{
    int8u idType;
    
    if (id == UP_PEDC_MON)
    {
        idType = UPTYPE_PEDC_HOST;              /* PEDC 主控板 */
    }
    else if   (id == UP_PEDC_MORE_DCUS)
    {
        idType = UPTYPE_MORE_DCU;               /* DCU 批量升级 */
    }
    else if ((id >= UP_PEDC_DCU_S) && (id <= UP_PEDC_DCU_E))
    {
        idType = UPTYPE_DCU;                    /* 单个 DCU升级 */
    }
    else if((id >= UP_PEDC_SLAVES_S) && (id <= UP_PEDC_SLAVES_E))
    {
        if ((id == UP_PEDC_PIC24A) || (id == UP_PEDC_PIC24B))
            idType = UPTYPE_PEDC_PIC24;         /* PIC24A 或 B */
        else
            idType = UPTYPE_PEDC_SLAVE;         /* 其它扩展板主芯片是PIC32 */
    }
    else
    {
        idType = UPTYPE_UNKNOWN;
    }
    
    return idType;
}


//**********************************************************************************************
//                                                                                             *
//                                     Master 函数功能                                         *
//                                                                                             *
//**********************************************************************************************
/*
  0            1            2            3           4            5
  文件头DLE    文件头STX    顺序号SEQ    源地址      目标地址     数据类型(HEX)
  &H10         &H02         &H0-FF
*/

//---------------------------------------------------------------------------
// Function:    MMS_Command()
// Description: 处理由上位机(MMS)发出的命令
// Parameters:  pCmd    = 数据指针,
//              pkgSize = 数据长度
// Return:      FALSE -- fail
// State:
//---------------------------------------------------------------------------
int8u MMS_Command(int8u *pCmd, int pkgSize)
{
    int8u *p1, *p2;

    int16u wTotLen;
    int16u PackCmd;
    
    int8u ret;
    int8u idType;

    ret = FALSE;

    UpdateMsg.MMSCmd.u8Dst = *(pCmd + 4);
    UpdateMsg.MMSCmd.u8Cmd = *(pCmd + 5);
    UpdateMsg.pData        = (int8u*)FileDataBuf;
    
    #ifdef TEST_BAT_UPDATE_DCU
    if((UpdateMsg.MMSCmd.u8Dst >= UP_PEDC_DCU_S) && (UpdateMsg.MMSCmd.u8Dst <= UP_PEDC_DCU_E))
        UpdateMsg.MMSCmd.u8Dst = UP_PEDC_MORE_DCUS;
    #endif
    
    wTotLen = (*(pCmd + 7)) | ((*(pCmd + 6))<<8);

    //将升级数据Copy到 FileDataBuf
    p1 = pCmd + 8;
    p2 = UpdateMsg.pData;
    while(wTotLen)
    {
        *p2++ = *p1++;
        wTotLen--;
    }

    wTotLen = ChkDataPack(UpdateMsg.pData);
    if(wTotLen < 3)
    {
        return ret;
    }

    /* 
     * 支持 IBP PSL IBP板在线升级
     * 支持 批量升级 DCU
     */
    idType = getUpdateType(UpdateMsg.MMSCmd.u8Dst);

    if(idType != UPTYPE_UNKNOWN)
    {
        if(UpdateMsg.MMSCmd.u8Cmd == MMS_CMD_UPDATE)        //只处理升级命令
        {
            GET_TICK(wTickUpdateSection);
            #ifdef  NODATA_ASK_MORE
            u8AskMoreCnt = 0;
            #endif

            PackCmd = *((int16u*)(UpdateMsg.pData) + 1);
            switch(PackCmd)
            {
                case PACK_CMD_UPDATE_DCU:
                {
                    u8UpdateState   = UPFW_STATE_UNKNOWN_CMD;
                    ret             = FALSE;
                    u8StartCommand  = TRUE;
                   
                    wUpFwSection    = 0;
                    
                    if((idType == UPTYPE_MORE_DCU) || (idType == UPTYPE_DCU))
                    {
                        //设置升级的DCU对象，仅对 DCU 有效 (4)设置待升级的DCU对象:
                        /*
                         * 0 ~ 1: PACK_PACKAGE_HEAD
                         * 2 ~ 3: PACK_CMD_UPDATE_DCU
                         * 4 ~ 5: 本次数据总长度(= 38)
                         * 6 ~13: 待升级DCU标志(8字节，每个位对应一个DCU)
                         * 14   : 下传HEX文件的DCU马达数量，值1表示单马达（全高门）, 2表示双马达（半高门）,其它值无效
                         * 15   : DCU 芯片类型，0=未定义，1=PIC30F6010A
                         * 16   : 项目校验信息长度(InfoLen, 不超过19个 ASCII字符)，0表示不校验, 否则需要校验
                         * 17~35: 项目校验信息(ASCII字符，19个字节)，
                         *        此部分内容将与 EE_SwDescription 前面的项目信息进行比较，比较长度为(InfoLen)
                         *        若校验信息相同才允许进行升级，否则拒绝升级
                         * 36~37: 校验和
                         * */
                        /* 串口头信息 8Bytes, 升级数据帧头信息 6Bytes */
                        
                        u8UpdateState   = UPFW_STATE_UPDATEFAIL;
                        if(*((int16u*)(UpdateMsg.pData) + 1) >= 38)
                        {
                            setBatchcastUpdateDCUs(pCmd + 8 + 6);
                            if(batchUpdateDCUs)
                                u8UpdateState   = UPFW_STATE_SEC_OK;
                            ret             = true;
                        }
                    }
                }
                break;
                
                case PACK_CMD_START:
                {
                    /* 如果收到启动升级前未收到批量升级命令 */
                    if(!batchUpdateDCUs)
                    {
                        /* 重置所有DCU升级状态 */
                        int i;
                        for(i=0; i<MAX_DCU; i++)
                        {
                            updateDcuSts[i].state   = UPFW_STATE_NOUPDATE;
                            updateDcuSts[i].section = 0;
                        }
                    }
                    
                    //if(UpdateMsg.u8MstState == MST_STATE_IDLE) //是否已经处于升级状态, 是则不处理
                    {                                            //此处不判断, 避免 MMS 复位以后重新启动升级而升级对象不理睬
                        //UpdateMsg.u8MstState = MST_STATE_START;
                        u8StartUpgrade = TRUE;
                        u8StartCommand = TRUE;
                        ret = TRUE;

                        wUpFwSection = 0;
                    }
                
                    #ifdef TEST_BAT_UPDATE_DCU
                    if(idType == UPTYPE_MORE_DCU)
                    {
                        int8u testSeting[] = { 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00};
                        setBatchcastUpdateDCUs(testSeting); 
                    }
                    #endif
                }
                break;

                
                case PACK_CMD_DATA:
                case PACK_CMD_END:
                {
                    u8StartCommand = FALSE;
                    wUpFwSection = *((int16u*)(UpdateMsg.pData) + 3);
                    if(UpdateMsg.u8MstState != MST_STATE_IDLE)
                    {
                        ret = TRUE;
                    }
                }
                break;
            }
            
            /* 下一步，传输升级数据包给升级对象 */
            UpdateMsg.u8MstState = MST_STATE_TRANS;
                    
            /* 备份 MMS 下传的升级数据包
             * 这里没有判断数据包的长度
             * if(pkgSize <= UPFW_CODE_BUF_MAX)
           */
            #ifdef BATCH_UPDATE_DCUS
            TgtMemCpy(updateDataPkg, pCmd, pkgSize);
            updatePkgSize   = pkgSize;
            #endif
            if(ret == TRUE)
            {
                /* 在线升级，批量升级 */
                if(idType != UPTYPE_MORE_DCU)
                {
                    clrBatchcastUpdateDCUs();
                }
                
                //在线升级 PEDC 主控板（如 MONITOR）
                if(idType == UPTYPE_PEDC_HOST)
                {
                    /* 不需要转发数据 */
                }
                //在线升级 DCU  
                else if((idType == UPTYPE_DCU) || (idType == UPTYPE_MORE_DCU))
                {
                    /* 要转发数据给 DCU */
                }
                //在线升级 PEDC 扩展板（如 IBP PSL SIG）
                else if(idType == UPTYPE_PEDC_SLAVE)
                {
                    if((cbUartTxPkg == NULL) || (cbUartTxBusy == NULL) || (cbQuerySlaveSts == NULL) || (cbChkOnline == NULL) || (cbGetSlaveReply == NULL))
                    {
                        /* 没有初始化，
                         * 设置升级失败返回 */
                        u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                        UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                    }
                    else
                    {
                        /* 判断 IBP PSL SIG 在线
                         *  */
                        if(cbChkOnline(UpdateMsg.MMSCmd.u8Dst) == FALSE)
                        {
                            /* 不在线，
                             * 设置升级失败返回 */
                            u8UpdateState        = UPFW_STATE_OBJOFFLINE;   //UPFW_STATE_UPDATEFAIL;
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                        }
                    }
                }
                else
                {
                    ret = FALSE;
                    UpdateMsg.u8MstState = MST_STATE_IDLE;
                }

            }
        }
    }
    else
    {
        ret = FALSE;
        UpdateMsg.u8MstState = MST_STATE_IDLE;
    }

    return ret;
}

//---------------------------------------------------------------------------
// Function:    UpdateProcess()
// Description: Master 升级处理主程序, 主循环调用
// Parameters:
// Return:      FALSE -- no result, TRUE -- result saved in u8UpdateState
// State:
//---------------------------------------------------------------------------
int8u UpdateProcess(void)
{
    int8u state;
    int8u Ret;
    int16u wNow,wTmp;
    static int16u wWaitSlave,wIntervals;
    static int8u u8FailTrans=0;

    Ret = FALSE;
    if (UpdateMsg.u8MstState == MST_STATE_IDLE)
        return Ret;

    if(UpdateMsg.MMSCmd.u8Cmd != MMS_CMD_UPDATE)
        return Ret;

    //指定时间内收不到上位机的数据, 停止升级
    wTmp = MMS_NODATA_TMR;
    #ifdef BATCH_UPDATE_DCUS
    if(batchUpdateDCUs)
    {   /* 批量升级状态下，增加超时时间 */
        wTmp = MMS_NODATA_TMR * 3;
    }
    #endif
    GET_TICK(wNow);
    if((int16u)(wNow - wTickUpdateSection) > wTmp)
    {
        UpdateMsg.u8MstState = MST_STATE_IDLE;
    }

    
    #ifdef  NODATA_ASK_MORE
    //测试升级功能，MMS提示“超时”的概率比较大（20~25%），根据通讯记录分析，可能是MMS的数据包PEDC没有接收到或接收失败
    //修改思路: PEDC进入升级状态后, 如果在某段时间内没有收到升级数据包, 向MMS请求数据重发。
    else
    {
        if(u8AskMoreCnt == 0)
        {
            u8AskMoreCnt++;
            wIntervals = wTickUpdateSection;
        }
        else
        {
            #ifdef BATCH_UPDATE_DCUS
            if(!batchUpdateDCUs)
            #endif
            {   /* 批量升级状态下，不请求重发数据 */
                wTmp = SYS_TICK_1MS * 2000;
            
                if((WORD)(wNow - wIntervals) > wTmp)
                {
                    wIntervals = wNow;

                    if(u8AskMoreCnt < 100)
                    {
                        u8AskMoreCnt ++;
                        //u8UpdateState = UPFW_STATE_SEC_ERR;     //
                        u8UpdateState = UPFW_STATE_ASKFORDATA;
                        return TRUE;
                    }
                }
            }
        }
    }
    #endif

    int8u idType = getUpdateType(UpdateMsg.MMSCmd.u8Dst);
    
    /* 设置升级对象的命令，仅针对 DCU */
    if(*((int16u*)FileDataBuf + 1) == PACK_CMD_UPDATE_DCU)
    {
        if((idType != UPTYPE_MORE_DCU) && (idType != UPTYPE_DCU))
        {
            UpdateMsg.u8MstState = MST_STATE_IDLE;
            Ret = UPFW_STATE_UNKNOWN_CMD;
            return Ret;
        }
    }
    
    #if 1   /* 折叠代码: 升级 MONITOR */
    if(idType == UPTYPE_PEDC_HOST)
    {
        //=================== 升级 Master
        switch(UpdateMsg.u8MstState)
        {
            case MST_STATE_TRANS:
            {
                u8UpdateState = UpFwProcessData((int8u*)FileDataBuf);
                
                UpdateMsg.u8MstState = MST_STATE_START;
                Ret = TRUE;

                /* 这里 UpdateMsg.wSlvSection 实际没用
                 * 赋值用于回复MMS时，携带段号的低4位用于观察通信交互过程
                 *  */
                UpdateMsg.wSlvSection = UpFwState.wCurrentSections;
                if(u8UpdateState != UPFW_STATE_UPDATEOK)
                {
                    UpdateMsg.wSlvSection--;
                }

                /* UPFW_STATE_UPDATEOK 表示全部升级数据包已经成功处理
                 *  */
                if(u8UpdateState == UPFW_STATE_UPDATEOK)
                {
                    UPDATE_FIRMWAREStateToMMS();
                    while(chkFinishTxToMMS() == FALSE){;}
                    UpdateMsg.u8MstState = MST_STATE_IDLE;

                    UpFw_InitShowMsg();
                    bios_Finished(&UpFwState,&UpdateTxMsg);
                }
            }
            break;

            default:
                break;
        }
    }
    #endif  /* 折叠代码: 升级 MONITOR */
    
    #if 1   /* 折叠代码: 升级 slave (IBP PSL SIG....) */
    else if(idType == UPTYPE_PEDC_SLAVE)
    {
        BOOL resendFlag = FALSE;
        
        switch(UpdateMsg.u8MstState)
        {
            case MST_STATE_TRANS:
                /* 
                 * 将 MMS 发送的数据包原样转发 
                 * 设置超时时间 200ms
                 * 最大数据发送长度约 : (115200/10) * 200/1000 = 2304 bytes
                 */
                cbUartTxPkg(updateDataPkg, updatePkgSize);

                GET_TICK(UpdateMsg.wTickOver);
                SET_TIMER_OVER(200);
                UpdateMsg.u8MstState = MST_STATE_SENDING;
                break;
                
            case MST_STATE_SENDING:
                #if 0   /*  TEST only */
                {
                u8UpdateState = UPFW_STATE_SEC_OK;
                UpdateMsg.u8MstState = MST_STATE_START;
                Ret = TRUE;
                break;
                }
                #endif
                
                /* 检查升级数据包是否发送完成，FALSE 表示串口空闲 */
                if(cbUartTxBusy()== FALSE)
                {
                    /* 发送完成 */
                    wWaitSlave  = wNow;
                    u8FailTrans = 0;
                    if(u8StartUpgrade == FALSE)
                    {
                        //SET_TIMER_OVER(2000);
                        SET_TIMER_OVER(MST_DLY_QUERY);
                    }
                    else
                    {
                        u8StartUpgrade = FALSE;
                        SET_TIMER_OVER(2000);
                    }
                    GET_TICK(UpdateMsg.wTickOver);
                    UpdateMsg.u8MstState = MST_STATE_QUERY_D;      //延时后查询
                }
                else
                {
                    IF_TIMER_OVER()
                    {
                        /* 发送超时 */
                        resendFlag = TRUE;
                    }
                }
                break;

            /* 升级数据包发送完成后，延时查询从机状态 */
            case MST_STATE_QUERY_D:
            case MST_STATE_QUERY:
                if(cbGetSlaveReply(UpdateMsg.MMSCmd.u8Dst, &UpdateMsg.u8SlvState, &UpdateMsg.wSlvSection) == FALSE)
                {
                    if(UpdateMsg.u8MstState == MST_STATE_QUERY_D)
                    {
                        IF_TIMER_OVER()
                        {
                            //发送查询命令
                            cbQuerySlaveSts(UpdateMsg.MMSCmd.u8Dst, wUpFwSection);
                            UpdateMsg.u8MstState = MST_STATE_QUERY;
                            GET_TICK(UpdateMsg.wTickOver);
                            SET_TIMER_OVER(200);
                        }
                    }
                    else
                    {
                        IF_TIMER_OVER()
                        {
                            resendFlag = TRUE;
                        }
                    }      
                }
                else
                {
                    //查询成功, Slave 的状态
                    u8UpdateState = UpdateMsg.u8SlvState;

                    if(wUpFwSection != UpdateMsg.wSlvSection)
                    {
                        if(wUpFwSection != 0)
                            u8UpdateState = UPFW_STATE_MstSlvSectionErr;
                    }
                    
                    switch(u8UpdateState)
                    {
                        case UPFW_STATE_NOUPDATE:
                        case UPFW_STATE_SEC_FAIL:
                        case UPFW_STATE_UPDATEOK:
                        case UPFW_STATE_UPDATEFAIL:
                        case UPFW_STATE_WAITDOOR:
                        case UPFW_STATE_OBJOFFLINE:
                        case UPFW_STATE_CHECK_FAIL   :      //校验失败，拒绝升级
                            UpdateMsg.u8MstState = MST_STATE_IDLE;
                            Ret = TRUE;
                            break;

                        case UPFW_STATE_MstSlvSectionErr:
                        case UPFW_STATE_SEC_ERR:
                        case UPFW_STATE_SEC_OK:
                            UpdateMsg.u8MstState = MST_STATE_START;
                            Ret = TRUE;
                            break;

                        case UPFW_STATE_WAITSTATE:
                            u8FailTrans = 0;
                            UpdateMsg.u8MstState = MST_STATE_IDLE;
                            Ret = TRUE;
                            break;


                        case UPFW_STATE_UPDATING:
                        case UPFW_STATE_COPY:
                            wTmp = 3000;
                            goto _IfUpgradeTimerOver_IPS;

                        default:
                            wTmp = 3000;
                        _IfUpgradeTimerOver_IPS:
                            if((int16u)(wNow - wWaitSlave) > wTmp)
                            {
                                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                if(u8UpdateState != UPFW_STATE_WAITSTATE)
                                    u8UpdateState  = UPFW_STATE_UPDATEFAIL;
                                Ret = TRUE;
                            }
                            else
                            {
                                GET_TICK(UpdateMsg.wTickOver);
                                SET_TIMER_OVER(MST_DLY_QUERY);
                                UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //延时后查询
                            }
                            break;
                    }
                }
                break;

            case MST_STATE_TRANS_FAIL:
                //transfer again ???
                u8FailTrans = 0;
                UpdateMsg.u8MstState = MST_STATE_IDLE;
                Ret = TRUE;
                break;

            default:
                break;
        }
        
        
        /* 是否需要重新发送 */
        if(resendFlag == TRUE)
        {                            
            u8FailTrans ++;
            if(u8FailTrans >= 3)
            {
                /* 查询超时未收到回复 */
                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                u8UpdateState = UPFW_STATE_UPDATEFAIL;
                Ret = TRUE;
            }
            else
            {
                /* 请求 MMS 重发 */
                UpdateMsg.u8MstState = MST_STATE_START;
                u8UpdateState        = UPFW_STATE_SEC_ERR;
                Ret = TRUE;
            }
        }
    }
    #endif  /* 折叠代码: 升级 slave (IBP PSL SIG....) */
    
    #if 1   /* 折叠代码: 升级 DCU */
    else if((idType == UPTYPE_DCU) || (idType == UPTYPE_MORE_DCU))
    {
        BOOL    bQueryNextDCU = FALSE;
        int8u   dst_dcu;
        int16u  wTotLen = (updateDataPkg[7]) | ((updateDataPkg[6])<<8);
        
        //=================== 升级 slave (DCU)
        switch(UpdateMsg.u8MstState)
        {
            /* ===== step 1. PEDC 发送升级数据 给 DCU ===== */
            case MST_STATE_TRANS:                   
            {
                dst_dcu = UpdateMsg.MMSCmd.u8Dst;   /* 待升级的DCU ID */
                
                #ifdef BATCH_UPDATE_DCUS
                if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                {
                    dst_dcu         = 0;            /* 批量发送升级数据包       */
                    
                    /* 从头开始查找是否有可以升级的DCU  */
                    updateDCUID     = searchFirstUpdateDCUID();
                    if(updateDCUID > MAX_DCU)
                    {
                        /* 没有符合条件的DCU对象 */
                        int ii;
                        UpdateMsg.u8MstState    = MST_STATE_IDLE;
                        Ret                     = TRUE;
                        for(ii=0; ii<MAX_DCU; ii++)
                        {
                            updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OFFLINE;
                        }
                        break;
                    }
                    
                    sendUpdatePkg   = TRUE;         /* 升级数据包已发送标志     */
                }
                else
                #endif
                {
                    if((dst_dcu < ID_DCU_MIN) || (dst_dcu > ID_DCU_MAX))
                    {
                        UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                        u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                        Ret = TRUE;
                        break;
                    }
                }

                /* 要转发数据给 DCU */
                ClrTransState(FUNID_UPDATE_FIRMWARE);
                CapiStartTrans(FUNID_UPDATE_FIRMWARE, dst_dcu, wTotLen+1, UpdateMsg.pData);
                UpdateMsg.u8MstState = MST_STATE_SENDING;
            }
            break;
                
            /* ===== step 2. 等待发送升级数据的结果 ===== */
            case MST_STATE_SENDING:
            {
                state = ChkTransState(FUNID_UPDATE_FIRMWARE);
                switch(state)
                {
                    case fgAbort:
                        //传送失败
                        u8FailTrans++;
                        if(u8FailTrans >= 3)
                        {                                                    //数据传输连续失败
                            u8FailTrans          = 0;
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                            u8UpdateState        = UPFW_STATE_UPDATEFAIL;
                            
                            #ifdef BATCH_UPDATE_DCUS
                            /* PEDC 发送失败，理应所有DCU的状态都是一样？ */
                            /* updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                             */
                            #endif

                            Ret = TRUE;
                        }
                        else
                        {
                            //if(ChkSlaveOnLine(UpdateMsg.MMSCmd.u8Dst) == TRUE)
                            {//在线，但是传送失败，重发
                                #ifdef BATCH_UPDATE_DCUS
                                if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                                {
                                    /* PEDC 自动重发 */
                                    UpdateMsg.u8MstState    = MST_STATE_TRANS;
                                    u8UpdateState           = UPFW_STATE_SEC_ERR;
                                    Ret                     = FALSE;
                                    
                                    updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                                }
                                else
                                #endif
                                {
                                    /* 以前版本程序，通知 MMS 重发 */
                                    UpdateMsg.u8MstState    = MST_STATE_START;
                                    u8UpdateState           = UPFW_STATE_SEC_ERR;     //设定 UPFW_STATE_SEC_ERR, MMS 将重发
                                    Ret                     = TRUE;
                                }
                            }
                            /*
                            else
                            {
                                UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                u8UpdateState        = UPFW_STATE_OFFLINE;
                                u8FailTrans          = 0;
                            }
                            */
                        }
                        
                        break;

                    case fgTransOK:
                        //传送完成
                        wWaitSlave  = wNow;
                        u8FailTrans = 0;
                        GET_TICK(UpdateMsg.wTickOver);
                        if(u8StartUpgrade == FALSE)
                        {
                            SET_TIMER_OVER(MST_DLY_QUERY);
                        }
                        else
                        {
                            u8StartUpgrade = FALSE;
                            SET_TIMER_OVER(500);
                        }
                        UpdateMsg.u8MstState = MST_STATE_QUERY_D;      //延时后查询
                        break;

                    case fgRunning:
                    default:
                        break;
                }
            }
            break;

            /* ===== step 3. 延时一段时间（等待DCU处理）后查询DCU ===== */
            case MST_STATE_QUERY_D:
            {
                IF_TIMER_OVER()
                {
                    #ifdef BATCH_UPDATE_DCUS
                    if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                    {
                        UpdateQuerySlave(updateDCUID);  
                    }
                    else
                    #endif
                    {
                        UpdateQuerySlave(UpdateMsg.MMSCmd.u8Dst);   /* 查询待升级DCU  */
                    }
                    
                    UpdateMsg.u8MstState = MST_STATE_QUERY;
                }
            }
            break;

            /* ===== step 4. 等待查询结果 ===== */
            case MST_STATE_QUERY:
            {
                state = ChkTransState(FUNID_UPDATE_FIRMWARE);
                switch(state)
                {
                    case fgAbort:
                    {
                        //查询失败
                        int16u dlyTime = 1000;      //2022-10-18 : 200 改为 1000
                        
                        if(u8StartCommand)
                        {   /* 只在发送启动命令后需要更长的延时，直到有查询结果 */
                            dlyTime = 3000;
                        }
                        
                        ClrTransState(FUNID_UPDATE_FIRMWARE);
                        if((int16u)(wNow - wWaitSlave) > dlyTime)
                        {
                            u8StartCommand = FALSE;
                        
                            UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                            UpdateMsg.u8SlvState = UPFW_STATE_UPDATEFAIL;
                            u8UpdateState = UPFW_STATE_UPDATEFAIL;
                            
                            #ifdef BATCH_UPDATE_DCUS
                            if((ChkSlaveOnLine(updateDCUID) == FALSE))
                            {   //不在线
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OBJOFFLINE;
                            }
                            else
                            {
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = u8UpdateState;
                            }
                            bQueryNextDCU = TRUE;
                            #endif
                            
                            Ret = TRUE;
                        }
                        else
                        {
                            GET_TICK(UpdateMsg.wTickOver);
                            SET_TIMER_OVER(MST_DLY_QUERY);
                            UpdateMsg.u8MstState = MST_STATE_QUERY_D;            //延时再查询
                        }
                    }
                    break;

                    case fgTransOK:
                        u8StartCommand = FALSE;
                        
                        //查询成功, 判断 Slave 的状态
                        ClrTransState(FUNID_UPDATE_FIRMWARE);
                        u8UpdateState = UpdateMsg.u8SlvState;

                        if((u8UpdateState == UPFW_STATE_SEC_ERR) || (u8UpdateState == UPFW_STATE_SEC_FAIL))
                        {
                            if(wUpFwSection != UpdateMsg.wSlvSection)
                            {
                                if(wUpFwSection != 0)
                                    u8UpdateState = UPFW_STATE_MstSlvSectionErr;
                            }
                        }
                        
                        #ifdef BATCH_UPDATE_DCUS
                        if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                        {
                            bQueryNextDCU = TRUE;
                            updateDcuSts[updateDCUID - ID_DCU_MIN].state    = u8UpdateState;
                            updateDcuSts[updateDCUID - ID_DCU_MIN].section  = UpdateMsg.wSlvSection;
                            
                            if((u8UpdateState == UPFW_STATE_UPDATEFAIL) && (ChkSlaveOnLine(updateDCUID) == FALSE))
                            {   //不在线
                                updateDcuSts[updateDCUID - ID_DCU_MIN].state = UPFW_STATE_OBJOFFLINE;
                            }
                        }
                        #endif

                        switch(u8UpdateState)
                        {
                            case UPFW_STATE_NOUPDATE:
                            case UPFW_STATE_SEC_FAIL:
                            case UPFW_STATE_UPDATEOK:
                            case UPFW_STATE_UPDATEFAIL:
                            case UPFW_STATE_WAITDOOR:
                            case UPFW_STATE_OBJOFFLINE:
                                
                            case UPFW_STATE_NOSETTING_DCU:      //没有设置升级DCU对象
                            case UPFW_STATE_MOTOR_ERR    :      //DCU 马达数量不匹配
                            case UPFW_STATE_DISABLE_DCU  :      //DCU 未使能升级
                            case UPFW_STATE_DCU_MCU_ERR  :      //DCU 芯片型号不匹配
                            case UPFW_STATE_CHECK_FAIL   :      //DCU 校验失败，拒绝升级
                                UpdateMsg.u8MstState = MST_STATE_IDLE;
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_MstSlvSectionErr:
                            case UPFW_STATE_SEC_ERR:
                            case UPFW_STATE_SEC_OK:
                                UpdateMsg.u8MstState = MST_STATE_START;
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_WAITSTATE:
                                UpdateMsg.u8MstState = MST_STATE_IDLE;
                                Ret = TRUE;
                                break;
                                
                            case UPFW_STATE_DCU_CONFIGED:
                                Ret = TRUE;
                                break;

                            case UPFW_STATE_UPDATING:
                                if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                                {
                                    Ret = TRUE;
                                    break;
                                }
                            case UPFW_STATE_COPY:
                                wTmp = 3000;
                                goto _IfUpgradeTimerOver;

                            default:
                                wTmp = 3000;
                            _IfUpgradeTimerOver:
                                if((int16u)(wNow - wWaitSlave) > wTmp)
                                {
                                    UpdateMsg.u8MstState = MST_STATE_TRANS_FAIL;
                                    if(u8UpdateState != UPFW_STATE_WAITSTATE)
                                        u8UpdateState  = UPFW_STATE_UPDATEFAIL;
                                    Ret = TRUE;
                                }
                                else
                                {
                                    GET_TICK(UpdateMsg.wTickOver);
                                    SET_TIMER_OVER(MST_DLY_QUERY);
                                    UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //延时后查询
                                }
                            
                                #ifdef BATCH_UPDATE_DCUS
                                if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
                                {
                                    updateDcuSts[updateDCUID - ID_DCU_MIN].state    = u8UpdateState;
                                }
                                #endif
                                break;
                        }
                        break;

                    case fgRunning:
                    default:
                        break;
                }
            }
            break;

            /* ===== PEDC传输升级数据失败后的处理 ===== */
            case MST_STATE_TRANS_FAIL:
                {
                    u8FailTrans             = 0;
                    UpdateMsg.u8MstState    = MST_STATE_IDLE;
                    Ret                     = TRUE;
                }
                break;

            default:
                break;
        }
        
        #ifdef BATCH_UPDATE_DCUS
        if(batchUpdateDCUs)                 /* 是否使能批量升级？ */
        {
            if(Ret == TRUE)
            {
                /* 本轮 DCU 已经有结果
                 * 判断是否需要查询下一个
                 *  */
                if(bQueryNextDCU == TRUE)
                {
                    Ret = FALSE;

                    /* 需要继续查询下一个 DCU，是否全部DCU已经查询完毕?
                     *  */
                    int8u nextDCU;
                    for(nextDCU = updateDCUID + 1; nextDCU<=MAX_DCU; nextDCU++)
                    {
                        /* 此DCU 设置有效，且在线
                         * ChkSlaveOnLine 的参数是 DCUID 从1开始  */
                        if((ChkRamBit(updateDCUSetting, nextDCU - 1) != 0))
                        {
                            if((ChkSlaveOnLine(nextDCU) == TRUE))
                                break;
                            else
                            {   /* 需要升级的DCU对象离线 */
                                updateDcuSts[nextDCU - ID_DCU_MIN].state    = UPFW_STATE_OFFLINE;
                            }
                        }
                    }
                    
                    if(nextDCU > MAX_DCU)
                    {
                        /* 全部DCU都查询完成 */
                        UpdateMsg.u8MstState    = MST_STATE_START;//MST_STATE_IDLE;
                        Ret                     = TRUE;
                    }
                    else
                    {
                        /* 继续查询下一个 DCU */
                        updateDCUID = nextDCU;
                        
                        /* 考虑到所有DCU处理的速度基本一致，所以这里设置的延时查询时间比较短，减少整个升级过程耗时 */
                        GET_TICK(UpdateMsg.wTickOver);
                        SET_TIMER_OVER(50);;                              //2022-10-18 : 5 改为 50
                        UpdateMsg.u8MstState = MST_STATE_QUERY_D;         //延时后查询
                        
                        //2022-8-6 批量升级查询到第6个DCU就超时了
                        //发送成功--> 延时约 60ms --> 轮询所有DCU，轮询到第6个DCU时，问了2次DCU超时没有回复
                        //  这里修改后，每个DCU独立计算超时
                        if(batchUpdateDCUs)
                        {
                            wWaitSlave  = wNow;
                        }
                    }
                }
            }
        }
        #endif
    }
    #endif  /* 折叠代码：升级DCU */

    return Ret;
}

//---------------------------------------------------------------------------
// Function:    UpdateQuerySlave
// Description: 查询 Slave 状态
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
static void UpdateQuerySlave(int8u id)
{
    ClrTransState(FUNID_UPDATE_FIRMWARE);
    
    CmdBuf[0] = UPFW_CMD_QUERY;
    CmdBuf[1] = 0x55;
    CmdBuf[2] = 0xAA;
    CmdBuf[3] = wUpFwSection;
    CapiTransExtCmd(FUNID_UPDATE_FIRMWARE, id, 5, CmdBuf);   //注意数据长度要多一个字节用于 Checksum
}

//---------------------------------------------------------------------------
// Function:    UpdateCmdFromSlave
// Description: Master 处理 Slave 的返回信息
// Parameters:  信息 pMsg
// Return:      TRUE -- 信息有效
// State:
//---------------------------------------------------------------------------
int8u UpdateCmdFromSlave(tCdrvMsg * pMsg)
{
    int8u u8ExtCmd;
    int8u u8CS;
    #if 0 
    /* 对 DCU V1.7或更早版本，前2个字节是 5A 5A 
     *        V1.8  修改后，  前2个字节是DCU当前需要的升级段号(低字节在前)
     */
    if((pMsg->m_bData[1] != 0x5A) && (pMsg->m_bData[2] != 0x5A))
    {
        return FALSE;
    }
    #endif

    u8CS = CalcChkSum(&pMsg->m_bData[0], pMsg->m_bSize-1);
    if(pMsg->m_bData[pMsg->m_bSize-1] != u8CS)                 //校验 checksum
    {
        return FALSE;
    }

    u8ExtCmd = pMsg->m_bData[0] & (~TRANS_DIR);

    switch(u8ExtCmd)
    {   
        /* DCU回复的升级状态查询，这里考虑用长度兼容旧版，新版的DCU改发送数据的长度
         *         0      1     2     3    4     5      6    7
         *V1.8前   extcmd 5A    5A    状态 段号L cs     -    -
         *V1.8及后 extcmd 段号L 段号H 状态 段号L cs     -    -
         * 新      extcmd 命令  版本  状态 段号L 段号H  保留 cs   待实现功能
         *  */
        case UPFW_CMD_QUERY:
            if(pMsg->m_bSize == 6)
            {
                UpdateMsg.u8SlvState    = pMsg -> m_bData[3];
                if((pMsg->m_bData[1] == 0x5A) && (pMsg->m_bData[2] == 0x5A))
                {   /* V1.8 前 */
                    UpdateMsg.wSlvSection   = pMsg -> m_bData[4];
                }
                else
                {   /* V1.8 及后 */
                    UpdateMsg.wSlvSection = (pMsg->m_bData[1]) + (pMsg->m_bData[2] << 8);
                }
                
                /*
                 *补丁原因：早版本的CalcChkSum,长度为 int8u ;
                 *新版本的长度为： int16u;为兼容老版本的，所以
                 *升级前先读取该长度反馈给PEDC，由PEDC做相应处理
                 *      CalcChkSumLEN;  == 0 16位长； != 0 8位长
                 * 当 DCU HEX 分段数量大于254时这个补丁存在隐患 !!!!!!
                 */
                #if 0
                if((wUpFwSection == 0) && (UpdateMsg.u8SlvState == UPFW_STATE_SEC_OK))
                {
                    if(pMsg -> m_bData[4] == 0xfe)
                    {
                        CalcChkSumLEN = 0;
                    }
                    else
                    {
                        CalcChkSumLEN = 0xfe;
                    }
                }
                #endif
            }
            else if(pMsg->m_bSize == 7)
            {
                /* 待实现功能 */
            }
            break;

        default:
            break;
    }

    return TRUE;
}

#endif //#if ((UPDATE_FUN_EN == TRUE))

