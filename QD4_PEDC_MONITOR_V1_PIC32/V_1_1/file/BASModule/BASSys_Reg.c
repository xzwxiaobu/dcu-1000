
#include "..\Include.h"

#include "BASSys_Reg.h"
#include "BASSys_App.h"
#include "dcu.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

int8u totalOfWV;        /* �緧���� */
int8u numOfAllOpn;      /* �緧ȫ��������ʵ��ȫ���������ڻ���ڴ���ֵ����BAS����緧ȫ���� */
int8u numOfAllCls;      /* �緧ȫ�ر�����ʵ��ȫ���������ڻ���ڴ���ֵ����BAS����緧ȫ�أ� */

/* �緧ȫ����ȫ�ز�����Ϣ */
PUBLIC void setWVOpnClsPara(int8u total, int8u opnNum, int8u clsNum)
{
    totalOfWV       = total;
    numOfAllOpn     = opnNum;
    numOfAllCls     = clsNum;
}

/* �緧״̬�ռ� */
typedef struct 
{
    int8u   opned;      /* �緧�Ѵ򿪼����� */
    int8u   clsed;      /* �緧�ѹرռ����� */
    int8u   overtime;   /* �緧��/�س�ʱ�ܱ��� */
}tAllWVStatus;

tAllWVStatus allWVStatus[32];

//int8u windOverTime[8];  
//int8u windOpnSts[8];    
//int8u windClsSts[8];    
tBASRegister BasRegister;

/* �� BAS ͨ�ŵĴ洢�� */
tBASRegister * getBASRegister(void)
{
    return &BasRegister;
}

/* �緧��ؿ���״̬ */
tWindValveSysSts WVSysSts;
tWindValveSysSts * getWindValveSysSts(void)
{
    return (tWindValveSysSts *)(&WVSysSts);
}

/* DCU�ϴ��ķ緧��������� */
tWindValveResult WVResult;
tWindValveResult * getWindValveResult(void)
{
    return (tWindValveResult *)(&WVResult);
}


///* ���з緧�Ŀ���״̬ */
//tWVStatus   WVStatus;
//tWVStatus * getWindValveStatus(void)
//{
//    return (tWVStatus *)(&WVStatus);
//}

int8u DCUOnLineSts[(MAX_DCU+7)/8];

/* ͳ�Ʒ緧������λ�͹رյ�λ������ */
int8u countWindValveState(int8u *opned, int8u *clsed);


/* ����Ƿ����е�DCU�����߻����ߣ������������������Ӧλ��ʾ ���߻�����
 * ע�⣺��Ҫ����ʵ�����õ�DCU (uDCUsetting.settingBUF)
 * chkStatus = cDcuOnLineF ��ʾ������ߣ� ������ߣ���Ӧλ����Ϊ 1
 * chkStatus = cDcuOffLineF ��ʾ������ߣ�������ߣ���Ӧλ����Ϊ 1
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


/* ��BASϵͳͨ�ŵ����ݼĴ���
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

        /* ��ȡDCU���� */
        dcuNum = calcSetting1Num((int8u*)&uDCUsetting.settingBUF, (MAX_DCU+7)/8);
        /* ����DCU������״̬�����online����Ӧλ����Ϊ 1 */
        chkAllDCUOnLineSts(DCUOnLineSts, MAX_DCU, cDcuOnLineF);


        /* �������״̬ */
        for(i=0; i<32; i++)
        {
            allWVStatus[i].opned    = 0;
            allWVStatus[i].clsed    = 0;
            allWVStatus[i].overtime = 0;
        }

        /* �緧״̬ */
        for(i=0; i<dcuNum; i++)
        {
            offset = i * DCU_WAVEVALVES;

            if(ChkRamBit(DCUOnLineSts, i) != 0)
            {
                /* ���� DCU �ش���״̬ */
                pDcuSts = (tDCU_AllStsFlags*)(&uniDcuData.sortBuf.aDcuState[i]);
                
                /* NOTED !!!!
                 * QD4 - �ֳ� DCU��2 or 3���緧��ͨ����־ʶ��
                 */
                /* ��/�ط緧 */
                if(pDcuSts->DCU_NumOfWV == 0)
                {   /* 2�緧 */
                    /* DCU �ϴ��ķ緧����λ���ص�λ����ʱ���� */
                    allWVStatus[i].opned    = pDcuSts->WV1Opened + pDcuSts->WV2Opened;
                    allWVStatus[i].clsed    = pDcuSts->WV1Closed + pDcuSts->WV2Closed;
                    allWVStatus[i].overtime = pDcuSts->err_WV1OverTime + pDcuSts->err_WV2OverTime;
                }
                else
                {   /* 3�緧 */
                    /* DCU �ϴ��ķ緧����λ���ص�λ����ʱ���� */
                    allWVStatus[i].opned    = pDcuSts->WV1Opened + pDcuSts->WV2Opened + pDcuSts->WV3Opened;
                    allWVStatus[i].clsed    = pDcuSts->WV1Closed + pDcuSts->WV2Closed + pDcuSts->WV3Closed;
                    allWVStatus[i].overtime = pDcuSts->err_WV1OverTime + pDcuSts->err_WV2OverTime + pDcuSts->err_WV3OverTime;
                }
            }
            else
            {
                /* ���ߵ�DCU, ������ */
                #if 0
                /* ȡ��DCU���ߵĹ��� */
                SetRamBit(faultSts, offset + 0);
                #endif
            }
        }

    }

    /* 
     * ��緧�޷�������رյ�λ��
     *      ���ѿ����緧����������ֵ��percentOfAllOpn������Ϊ���з緧����λ
     *      ���ѹرշ緧����������ֵ��percentOfAllCls��, ��Ϊ���з緧�ص�λ
     * 
     *  ��  ���з緧��������رյ�λ  ʱ��������еķ緧����
     *  */
    WVResult.byte = 0;
    /* ͳ�Ʒ緧������λ�͹رյ�λ������ */
    int8u  opned, clsed, threadOpn, threadCls;
    
    countWindValveState(&opned, &clsed);
    
    /* 2022-4-20 ��ǰ�ñ���ֵ����Ϊ��ֵ  */
    threadOpn = (int8u)numOfAllOpn;
    threadCls = (int8u)numOfAllCls;
    
    /* �緧��������0ʱ�˲�������Ч����Χ1-�緧���� */
    if(totalOfWV > 0)
    {
        /* ������λ�������Ƿ񳬹��趨��ֵ��������Ϊȫ��������λ */
        if(opned >= threadOpn)
        {
            WVResult.bWindAllOpned = 1;
        }

        /* �رյ�λ�������Ƿ񳬹��趨��ֵ��������Ϊȫ���رյ�λ */
        if(clsed >= threadCls)
        {
            WVResult.bWindAllClsed = 1;
        }
    }
   
    /* ===== ��BAS�ļĴ�����ֵ ===== */
    /* �緧���������Լ�DCU��������Ļ���״̬
     * д�� BAS �Ĵ��� */
    BasRegister.reg03.bits.LocalCtrl    = WVSysSts.bWindCtrlBy_PSC;     /* ���ؿ���ģʽ   */
    BasRegister.reg03.bits.BASCtrl      = !WVSysSts.bWindCtrlBy_PSC;    /* ��BASԶ�̿���ģʽ   */
    BasRegister.reg03.bits.allWVOpned   = WVResult.bWindAllOpned;       /* �緧�ܿ� */
    BasRegister.reg03.bits.allWVClsed   = WVResult.bWindAllClsed;       /* �緧�ܹ� */
    if(pBuf != NULL)
    {
        /* ע����ֽں͵��ֽڽ���λ�� */
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
 * ͳ�Ʒ緧������λ�͹رյ�λ������
 * 
 *  opned : ������λ������
 *  clsed : �رյ�λ������
 * ���ط緧����
 *  */
int8u countWindValveState(int8u *opned, int8u *clsed)
{
    int8u no;
    int8u nc;
    tAllWVStatus *pSts;
    int i;
    
    /* ��ȡDCU���� */
    int8u dcuNum = calcSetting1Num((int8u*)&uDCUsetting.settingBUF, (MAX_DCU+7)/8);
    
    /* ͳ�ƿ���״̬�ķ緧 */
    no = 0; 
    pSts = allWVStatus;
    for(i=0; i<dcuNum; i++)
    {
        no += allWVStatus[i].opned;
    }
    *opned = no;
    
    
    /* ͳ�ƹر�״̬�ķ緧 */
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
 * ���ԵĹ���
 * ������ BAS ϵͳ�ӿڲ���
 * PC�����´��������ݸ� �Ĵ��������͸� BAS
 * ������Чʱ����  TestBasRegDataTime ȷ������ TestBasRegDataTime = 0 ʱ�ظ�����������
 *  */
int16u TestBasRegDataTime = 0;

/* �ж��Ƿ��� BAS ����״̬ */
BOOL ifTestBASRegValue(void)
{
    return (TestBasRegDataTime > 0) ? TRUE : FALSE;
}


/* ���� ʣ��Ĳ���ʱ��
 * ms ��ʾ��ǰ���ĵ�ʱ��ֵ ����
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

/* ǰ�� 2 �ֽ���ʱ��ֵ
 * ������ �Ĵ��� ֵ */
void setTestBASRegValue(int8u *pReg)
{
    int i;
    int32u tst;
    int32u opnFlag,clsFlag, faultFlag;
    
    /* 2 �ֽڲ���ʱ�� */
    TestBasRegDataTime = ((int16u)pReg[0] << 0) + ((int16u)pReg[1] << 8);
    pReg += 2;

    /* 2 �ֽڲ�����Ϣ */
    WVSysSts.byte = pReg [ 0];
    
    /* 12 �ֽڷ緧״̬ */
    pReg += 2;
    
    opnFlag     = (int32u)pReg [0 ]  | ((int32u)pReg[1 ] << 8) | ((int32u)pReg[2 ] << 16) | ((int32u)pReg[3 ] << 24);
    clsFlag     = (int32u)pReg [4 ]  | ((int32u)pReg[5 ] << 8) | ((int32u)pReg[6 ] << 16) | ((int32u)pReg[7 ] << 24);
    faultFlag   = (int32u)pReg [8 ]  | ((int32u)pReg[9 ] << 8) | ((int32u)pReg[10] << 16) | ((int32u)pReg[11] << 24);
    
    /* �������״̬ */
    for(i=0; i<32; i++)
    {
        allWVStatus[i].opned    = 0;
        allWVStatus[i].clsed    = 0;
        allWVStatus[i].overtime = 0;
    }
    
    /* ÿ��λ����һ��DCU��1��ʾ��DCU��Ӧ�ķ緧��ȫ��/��/����
     * �����ÿ��DCU����2���緧����
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
