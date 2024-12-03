
#include "..\Include.h"

#include "BASSys_Reg.h"
#include "BASSys_App.h"
#include "dcu.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

int8u totalOfWV;        /* 风阀总数 */
int8u numOfAllOpn;      /* 风阀全开比例（实际全开数量大于或等于此数值，给BAS报告风阀全开） */
int8u numOfAllCls;      /* 风阀全关比例（实际全关数量大于或等于此数值，给BAS报告风阀全关） */

/* 风阀全开或全关参数信息 */
PUBLIC void setWVOpnClsPara(int8u total, int8u opnNum, int8u clsNum)
{
    totalOfWV       = total;
    numOfAllOpn     = opnNum;
    numOfAllCls     = clsNum;
}

/* 风阀状态收集 */
typedef struct 
{
    int8u   opned;      /* 风阀已打开计数器 */
    int8u   clsed;      /* 风阀已关闭计数器 */
    int8u   overtime;   /* 风阀开/关超时总报警 */
}tAllWVStatus;

tAllWVStatus allWVStatus[32];

//int8u windOverTime[8];  
//int8u windOpnSts[8];    
//int8u windClsSts[8];    
tBASRegister BasRegister;

/* 与 BAS 通信的存储器 */
tBASRegister * getBASRegister(void)
{
    return &BasRegister;
}

/* 风阀相关控制状态 */
tWindValveSysSts WVSysSts;
tWindValveSysSts * getWindValveSysSts(void)
{
    return (tWindValveSysSts *)(&WVSysSts);
}

/* DCU上传的风阀结果，汇总 */
tWindValveResult WVResult;
tWindValveResult * getWindValveResult(void)
{
    return (tWindValveResult *)(&WVResult);
}


///* 所有风阀的开关状态 */
//tWVStatus   WVStatus;
//tWVStatus * getWindValveStatus(void)
//{
//    return (tWVStatus *)(&WVStatus);
//}

int8u DCUOnLineSts[(MAX_DCU+7)/8];

/* 统计风阀开启到位和关闭到位的数量 */
int8u countWindValveState(int8u *opned, int8u *clsed);


/* 检测是否所有的DCU都在线或不在线，根据输入参数设置相应位表示 在线或不在线
 * 注意：需要考虑实际设置的DCU (uDCUsetting.settingBUF)
 * chkStatus = cDcuOnLineF 表示检查在线， 如果在线，对应位设置为 1
 * chkStatus = cDcuOffLineF 表示检查离线，如果离线，对应位设置为 1
 */
void chkAllDCUOnLineSts(int8u* result, int8u DCUs, int8u chkStatus)
{
    int i,j,DCU;
    
    for(i = 0; i < (DCUs+7)/8; i ++)
    {
        result[i] = 0;
    }
    
    j = 0;
    DCU = ID_DCU_MIN;
    for(i = 0; i < DCUs; i ++)
    {
        if(ptrDcuIfOnLine((int8u*)&uDCUsetting.settingBUF, DCU) == BITEQU1)
        {
            if(DcuIfOnLine(DCU) == chkStatus)
            {
                SetpBufBit(result,j); 
            }
        }
        j ++;
        DCU ++;
    }
}


/* 与BAS系统通信的数据寄存器
 * */
void GetReplyBASMessage(int8u *pBuf)
{
    int i;
    int8u *p;
    
    if(ifTestBASRegValue() == FALSE)
    {
        tDCU_AllStsFlags *pDcuSts;
        int8u dcuNum;

        int  offset;

        /* 获取DCU总数 */
        dcuNum = calcSetting1Num((int8u*)&uDCUsetting.settingBUF, (MAX_DCU+7)/8);
        /* 所有DCU的离线状态，如果online，对应位设置为 1 */
        chkAllDCUOnLineSts(DCUOnLineSts, MAX_DCU, cDcuOnLineF);


        /* 清除所有状态 */
        for(i=0; i<32; i++)
        {
            allWVStatus[i].opned    = 0;
            allWVStatus[i].clsed    = 0;
            allWVStatus[i].overtime = 0;
        }

        /* 风阀状态 */
        for(i=0; i<dcuNum; i++)
        {
            offset = i * DCU_WAVEVALVES;

            if(ChkRamBit(DCUOnLineSts, i) != 0)
            {
                /* 在线 DCU 回传的状态 */
                pDcuSts = (tDCU_AllStsFlags*)(&uniDcuData.sortBuf.aDcuState[i]);
                
                /* NOTED !!!!
                 * QD4 - 现场 DCU接2 or 3个风阀，通过标志识别
                 */
                /* 开/关风阀 */
                if(pDcuSts->DCU_NumOfWV == 0)
                {   /* 2风阀 */
                    /* DCU 上传的风阀开到位，关到位，超时错误 */
                    allWVStatus[i].opned    = pDcuSts->WV1Opened + pDcuSts->WV2Opened;
                    allWVStatus[i].clsed    = pDcuSts->WV1Closed + pDcuSts->WV2Closed;
                    allWVStatus[i].overtime = pDcuSts->err_WV1OverTime + pDcuSts->err_WV2OverTime;
                }
                else
                {   /* 3风阀 */
                    /* DCU 上传的风阀开到位，关到位，超时错误 */
                    allWVStatus[i].opned    = pDcuSts->WV1Opened + pDcuSts->WV2Opened + pDcuSts->WV3Opened;
                    allWVStatus[i].clsed    = pDcuSts->WV1Closed + pDcuSts->WV2Closed + pDcuSts->WV3Closed;
                    allWVStatus[i].overtime = pDcuSts->err_WV1OverTime + pDcuSts->err_WV2OverTime + pDcuSts->err_WV3OverTime;
                }
            }
            else
            {
                /* 离线的DCU, 报故障 */
                #if 0
                /* 取消DCU离线的故障 */
                SetRamBit(faultSts, offset + 0);
                #endif
            }
        }

    }

    /* 
     * 因风阀无法开启或关闭到位，
     *      当已开启风阀数量超过阈值后（percentOfAllOpn），认为所有风阀开到位
     *      当已关闭风阀数量超过阈值后（percentOfAllCls）, 认为所有风阀关到位
     * 
     *  当  所有风阀都开启或关闭到位  时，清除所有的风阀故障
     *  */
    WVResult.byte = 0;
    /* 统计风阀开启到位和关闭到位的数量 */
    int8u  opned, clsed, threadOpn, threadCls;
    
    countWindValveState(&opned, &clsed);
    
    /* 2022-4-20 以前用比例值，改为数值  */
    threadOpn = (int8u)numOfAllOpn;
    threadCls = (int8u)numOfAllCls;
    
    /* 风阀总数大于0时此参数才有效，范围1-风阀总数 */
    if(totalOfWV > 0)
    {
        /* 开启到位的数量是否超过设定阈值，是则认为全部开启到位 */
        if(opned >= threadOpn)
        {
            WVResult.bWindAllOpned = 1;
        }

        /* 关闭到位的数量是否超过设定阈值，是则认为全部关闭到位 */
        if(clsed >= threadCls)
        {
            WVResult.bWindAllClsed = 1;
        }
    }
   
    /* ===== 给BAS的寄存器赋值 ===== */
    /* 风阀控制命令以及DCU反馈结果的汇总状态
     * 写入 BAS 寄存器 */
    BasRegister.reg03.bits.LocalCtrl    = WVSysSts.bWindCtrlBy_PSC;     /* 本地控制模式   */
    BasRegister.reg03.bits.BASCtrl      = !WVSysSts.bWindCtrlBy_PSC;    /* 非BAS远程控制模式   */
    BasRegister.reg03.bits.allWVOpned   = WVResult.bWindAllOpned;       /* 风阀总开 */
    BasRegister.reg03.bits.allWVClsed   = WVResult.bWindAllClsed;       /* 风阀总关 */
    if(pBuf != NULL)
    {
        /* 注意高字节和低字节交换位置 */
        p = (int8u*)&BasRegister.reg03;
        for(i=0; i<sizeof(BasRegister.reg03); i+=2)
        {
            *pBuf++ = *(p+1);
            *pBuf++ = *p;

            p += 2;
        }
    }
}


/*
 * 统计风阀开启到位和关闭到位的数量
 * 
 *  opned : 开启到位的数量
 *  clsed : 关闭到位的数量
 * 返回风阀总数
 *  */
int8u countWindValveState(int8u *opned, int8u *clsed)
{
    int8u no;
    int8u nc;
    tAllWVStatus *pSts;
    int i;
    
    /* 获取DCU总数 */
    int8u dcuNum = calcSetting1Num((int8u*)&uDCUsetting.settingBUF, (MAX_DCU+7)/8);
    
    /* 统计开启状态的风阀 */
    no = 0; 
    pSts = allWVStatus;
    for(i=0; i<dcuNum; i++)
    {
        no += allWVStatus[i].opned;
    }
    *opned = no;
    
    
    /* 统计关闭状态的风阀 */
    nc = 0;
    pSts = allWVStatus;
    for(i=0; i<dcuNum; i++)
    {
        nc += allWVStatus[i].clsed;
    }
    *clsed = nc;
    
    return dcuNum;
}



/* =============================================================================
 * 测试的功能
 * 用于与 BAS 系统接口测试
 * PC工具下传测试数据给 寄存器，发送给 BAS
 * 数据有效时间由  TestBasRegDataTime 确定，当 TestBasRegDataTime = 0 时回复正常的数据
 *  */
int16u TestBasRegDataTime = 0;

/* 判断是否处于 BAS 测试状态 */
BOOL ifTestBASRegValue(void)
{
    return (TestBasRegDataTime > 0) ? TRUE : FALSE;
}


/* 计算 剩余的测试时间
 * ms 表示当前消耗的时间值 毫秒
 *  */
void decTestBASRegTime(int16u ms)
{
    if(TestBasRegDataTime >= ms)
    {
        TestBasRegDataTime -= ms;
    }
    else
    {
        TestBasRegDataTime = 0;
    }
}

/* 前面 2 字节是时间值
 * 后面是 寄存器 值 */
void setTestBASRegValue(int8u *pReg)
{
    int i;
    int32u tst;
    int32u opnFlag,clsFlag, faultFlag;
    
    /* 2 字节测试时间 */
    TestBasRegDataTime = ((int16u)pReg[0] << 0) + ((int16u)pReg[1] << 8);
    pReg += 2;

    /* 2 字节操作信息 */
    WVSysSts.byte = pReg [ 0];
    
    /* 12 字节风阀状态 */
    pReg += 2;
    
    opnFlag     = (int32u)pReg [0 ]  | ((int32u)pReg[1 ] << 8) | ((int32u)pReg[2 ] << 16) | ((int32u)pReg[3 ] << 24);
    clsFlag     = (int32u)pReg [4 ]  | ((int32u)pReg[5 ] << 8) | ((int32u)pReg[6 ] << 16) | ((int32u)pReg[7 ] << 24);
    faultFlag   = (int32u)pReg [8 ]  | ((int32u)pReg[9 ] << 8) | ((int32u)pReg[10] << 16) | ((int32u)pReg[11] << 24);
    
    /* 清除所有状态 */
    for(i=0; i<32; i++)
    {
        allWVStatus[i].opned    = 0;
        allWVStatus[i].clsed    = 0;
        allWVStatus[i].overtime = 0;
    }
    
    /* 每个位代表一个DCU，1表示改DCU对应的风阀，全开/关/故障
     * 简单起见每个DCU都按2个风阀计算
     *  */
    tst = 1;
    for(i=0; i<32; i++)
    {
        if(tst & opnFlag)
        {
            allWVStatus[i].opned    = 2;
        }
        if(tst & clsFlag)
        {
            allWVStatus[i].clsed    = 2;
        }
        if(tst & faultFlag)
        {
            allWVStatus[i].overtime = 2;
        }
        tst <<= 1;
    }
}
