
#include "Include.h"
#include "waveflash.h"

PRIVATE int16u getDotLevelnum(int8u DotSort, int8u level);
PRIVATE void Handle_flash(int8u DotSort);
PRIVATE void saveDot(int8u DotSort, int8u bit);
PRIVATE void scan_sf_dot(void);
PRIVATE void cls_all_flashmsg_to_mms(void);
PRIVATE void uniteFlash(int8u sort_low4, int8u sort_high4,  int8u * val);

PRIVATE tReadDot  ReadDot[READDDOT_NUM];
PRIVATE int8u scan_sf_time;

/* ========================================================================== */
//λ����
PRIVATE void SetBUFBit(int8u *p,int8u bit)
{
    p  += bit>>3;
    *p |= 1<<(bit & 7);
}

//λ���
PRIVATE void ClrBUFBit(int8u *p,int8u bit)
{
    p  += bit>>3;
    *p &= ~(1<<(bit & 7));
}

//λ����
PRIVATE int8u ChkBUFBit(int8u *p,int8u bit)
{
    p += bit>>3;
    if(*p & (1<<(bit & 7)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/* ��ʼ�� 
 * �����е㻺��
 * */
PRIVATE void ini_DotBuf_all(void)
{
    int i,j;
    int8u *p;
    for(i = 0; i < READDDOT_NUM; i ++)
    {
        p = (int8u *) &ReadDot[i];
        
        for(j = 0; j < sizeof(ReadDot[i]); j ++)
        {
            *p ++ = 0;
        }
    }
    
}

/*  
 * ��ָ��ɨ��㻺��
 * */
PRIVATE void ClsDotBuf(int8u DotSort)
{
    int i;
    for(i = 0; i < sizeof(ReadDot[DotSort].DotBuf); i ++)
    {
        ReadDot[DotSort].DotBuf[i] = 0;
    }
}


#define BUFHAVEDATA 0x8000
/*
in: 
    level == 0 ���͵�ƽ�� 1 ���ߵ�ƽ
    
out:
.15  == 1 ���廹������
.0-.7 ��ȡ����

*/
PRIVATE int16u getDotLevelnum(int8u DotSort, int8u level)
{
    int16u num = 0;
    
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];

    if((sort->DotStart >= CREADDOTNUM) || (sort->DotEnd >= CREADDOTNUM) || (sort ->DotStart == sort->DotEnd))
    {//������
            sort->DotStart = 0;
            sort->DotEnd   = 0;
            return num;
    }   
    
    while(1)
    {   
        if(sort->DotStart == sort->DotEnd)
        {
            sort->DotStart = 0;
            sort->DotEnd   = 0;
            return num;
        }   
        
        if(ChkBUFBit((int8u *) &sort->DotBuf,sort->DotStart) == level)
        {
            num ++;
        }
        else
        {
            return num + BUFHAVEDATA;
        }       
        
        
        sort->DotStart ++;
        if(sort->DotStart >= CREADDOTNUM)
        {
            sort->DotStart = 0;
        }   
        
    }       
    
}

PRIVATE void addNum(int8u * p, int8u num)
{
    if( (*p + num) > 255)
    {
        *p = 255;
    }   
    else
    {
        *p += num;
    }   
}

PRIVATE void get_SF_FLASH(int8u SORT_SF)
{
    
    int8u i;
    
    if(ReadDot[SORT_SF].DotEnd >= ReadDot[SORT_SF].DotStart)
    {   
        i = ReadDot[SORT_SF].DotEnd - ReadDot[SORT_SF].DotStart;
    }
    else
    {
        i = 0xff;   
        
    }   
        
    if((ReadDot[SORT_SF].time > CHANDLETIME) || (i > (CREADDOTNUM -  5)))
    {   
        ReadDot[SORT_SF].time = 0;
        Handle_flash(SORT_SF);
        
    }
    
}

/*
            DCU��ȫ��·����  ���ϳ���
        OR  DCU��ȫ��·�˵�ѹ���ϳ���
        OR  PDS��ȫ��·����  ���ϳ���
        OR  PDS��ȫ��·�˵�ѹ���ϳ���
*/
PRIVATE void HandleSF_FLASH(void)
{
    int8u i;

    for(i = 0; i < READDDOT_NUM; i ++)
    {
        get_SF_FLASH(i); 
    }

}

//���
//out: flashNum
PRIVATE void Handle_flash(int8u DotSort)
{
    int8u loop;
    int16u ret;
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];
    int8u temp0;

    for(loop = 0; loop < 255; loop ++) //�����õ�ѭ��
    {
        if(sort->DotStart == sort->DotEnd)
        {
            break;
        }   
        
        switch(sort->CNT)
        {
            case powOnPhase:
                ret = getDotLevelnum(DotSort, 1);
                if(ret & 0xff)
                {//��⵽�ߵ�ƽ
                    sort->CNT = LevelHigh;
                }   
                else if(ret & BUFHAVEDATA)
                {//��⵽�͵�ƽ
                    sort->CNT = LevelLow;
                }   
                
                break;
                
            case LevelHigh:          //�ȶ��ĸߵ�ƽ
                ret = getDotLevelnum(DotSort, 1);
                if((ret & BUFHAVEDATA))
                {//��⵽������
                    sort->flashNum = 1;
                    sort->level_stable_time = 0;
                    sort->shake_time = 0;
                    
                    sort->CNT = H_testL;
                }   
                
                break;
    
            case H_testL:
                ret = getDotLevelnum(DotSort, 0);
                
                if(sort->shake_time < SHAKE_TIME)
                {
                    if((sort->shake_time + (ret & 0xff)) >= SHAKE_TIME)
                    {
                        temp0 = SHAKE_TIME - sort->shake_time;
                        
                        if((sort->level_stable_time + temp0) < LEVEL_STABLE_TIME)
                        {
                            sort->flashmsg_to_mms += sort->flashNum;
                            sort->flashNum = 0;
                        }   
                        
                    }   
                    
                }
                
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->level_stable_time >= LEVEL_STABLE_TIME)
                {//���͵�ƽ
                
                    sort->CNT = LevelLow;
                    
                    break;
                    
                }   
                
                if(sort->shake_time >= SHAKE_TIME)
                {//������ʱ��󣬰����ϴ����ϴ�MMS
                    
                    sort->flashmsg_to_mms += sort->flashNum;
                    sort->flashNum = 0;
                    
                }   
                if((ret & BUFHAVEDATA))
                {//��⵽������
                    sort->CNT = H_testH;
                    sort->level_stable_time = 0;
                }   
                break;
            
            case    H_testH:
                ret = getDotLevelnum(DotSort, 1);
    
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->shake_time >= SHAKE_TIME)
                {//������ʱ��󣬰����ϴ����ϴ�MMS
                    
                    sort->flashmsg_to_mms += sort->flashNum;
                    sort->flashNum = 0;
                    
                }   
                
                if(sort->level_stable_time >= LEVEL_STABLE_TIME)
                {//���ߵ�ƽ
                
                    sort->flashmsg_to_mms += sort->flashNum;
                    sort->CNT = LevelHigh;
                }   
                else if((ret & BUFHAVEDATA))
                {
                    sort->flashNum ++;
                    sort->CNT = H_testL;
                    sort->level_stable_time = 0;
                }   
                
            
                break;
                
                //************************************************
            case LevelLow:          //�ȶ��ĵ͵�ƽ
                ret = getDotLevelnum(DotSort, 0);
                if((ret & BUFHAVEDATA))
                {//��⵽������
                    sort->flashNum = 0;
                    sort->level_stable_time = 0;
                    sort->shake_time = 0;
                    
                    sort->CNT = L_testH;
                }   
                
                break;
    
            case L_testH:
                ret = getDotLevelnum(DotSort, 1);
                
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->level_stable_time > (LEVEL_STABLE_TIME * 3))
                {//���ߵ�ƽ
                
                    sort->CNT = LevelHigh;
                    
                    break;
                    
                }   
                
                if((ret & BUFHAVEDATA))
                {//��⵽������
                    sort->CNT = L_testL;
                    sort->level_stable_time = 0;
                }   
                break;
            
            case    L_testL:
                ret = getDotLevelnum(DotSort, 0);
    
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->level_stable_time > LEVEL_STABLE_TIME)
                {//���͵�ƽ
                
                    //sort->flashmsg_to_mms += sort->flashNum;
                    sort->CNT = LevelLow;
                }   
                else if((ret & BUFHAVEDATA))
                {
                    sort->CNT = L_testH;
                    sort->level_stable_time = 0;
                }   
            
                break;

                
            default:
                sort->CNT = 0;
                break;  
            
        }
    }   
}

/*
                                 WTJ-GY2                NJ7                   JNR2(TY2)
��ȫ�̵����˵�ѹ:  K31P/K31N     I48  RD.7              I10        RB14       I12  RB3   K31K32_PN
                   K31K32        I5   0-IO4_3(K-S11)    I4 (K-S6)  RB11       I5   RB7   K31K32_NC(K-S11)
                   K33           I9   0-IO3_7(K-S12)    I1 (K-S7)  RB2        I2   RG9   K33_KE_NO(K-S12)

ͨ�� K31K32_PN �� K31K32_NC �жϼ̵����Ƿ��й���
ͨ�� K31K32_PN �õ���ȫ��·�˵�ѹ���ϴ���
ͨ�� K31K32_NC �õ���ȫ��·�������ϴ���
*/

//#define EMU_TEST_FLASH
PRIVATE void scan_sf_dot(void)
{
    /* DCU SF
     * TRUE ��ʾ�ߵ�ƽ��û�����봥����
     *  */
    saveDot(SORT_DCUSF_coil   , chkDCUSF_K31PN()        == TRUE? 0: 1);
    saveDot(SORT_DCUSF_contact, chkDCUSF_K31NC()        == TRUE? 0: 1);
    
    /* PDS SF 
     *  */
    saveDot(SORT_PDSSF_coil   , chkPDSSF_Coil()         == TRUE? 0: 1);
    saveDot(SORT_PDSSF_contact, chkPDSSF_Contactor()    == TRUE? 0: 1);
    
}

//************************************************
//��ȡ�㣬���뻺��
/*
;in: bit: 0 ����0�� 1 ���� 1

*/
PRIVATE void saveDot(int8u DotSort, int8u bit)
{
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];
    
    if((sort->DotStart >= CREADDOTNUM) || (sort->DotEnd >= CREADDOTNUM) || (sort ->DotStart == sort->DotEnd))
    {//������
            sort->DotStart = 0;
            sort->DotEnd   = 0;
    }   

    if(bit)
    {//����1
        SetBUFBit((int8u *)&sort->DotBuf ,sort->DotEnd);
    }
    else
    {//����0
        ClrBUFBit((int8u *)&sort->DotBuf ,sort->DotEnd);
    }   
            
    sort->DotEnd ++;
    if(sort->DotEnd >= CREADDOTNUM)
    {
        sort->DotEnd = 0;
    }   
    
}

//�ϲ�ȡ�õ������� (����λ + ����λ)
/*
* val     : �ϴκϲ��ı����ַ
sort_low4:  �ϲ��ڵ���λ
sort_high4: �ϲ��ڸ���λ
*/
PRIVATE void uniteFlash(int8u sort_low4, int8u sort_high4,  int8u * val)
{
    int8u sf_flash;
    
    sf_flash = * val;
    if(((ReadDot[sort_low4].flashmsg_to_mms) + (sf_flash & 0xf)) > 0xf)
    {
        sf_flash |= 0x0f;
    }   
    else
    {
        sf_flash += ReadDot[sort_low4].flashmsg_to_mms;
    }   

    if(((ReadDot[sort_high4].flashmsg_to_mms) + (sf_flash>> 4)) > 0xf)
    {
        sf_flash |= 0xf0;
    }   
    else
    {
        sf_flash += (ReadDot[sort_high4].flashmsg_to_mms << 4);
    }   
    
    *val = sf_flash;
}

//�����е� flashmsg_to_mms
PRIVATE void cls_all_flashmsg_to_mms(void)
{
    int8u i;
    for(i = 0; i < READDDOT_NUM; i ++)
    {
        ReadDot[i].flashmsg_to_mms    = 0;
    }
}



/* =============================================================================
 * ��ȫ��·���ϼ��
 *  scanInterval ��ɨ��ʱ������Ĭ�� CSCAN_SF_TIME
 * ע�⣺
 *  ����ֵ�ǰ��ֽڱ��棬���Ϊ15
 *  ����⵽�����Ժ���Ҫһֱ����ֱ���������ɹ���ȡ��Ȼ������ֵ���
 *  ���
 */
PUBLIC void SFDectectProc(int8u scanInterval)
{
    if(scan_sf_time < scanInterval);
        return;
        
    scan_sf_time = 0;
    
    scan_sf_dot();
    
    HandleSF_FLASH();
}

/* ��ʼ�� */
PUBLIC void SFDectectInit(void)
{
    ini_DotBuf_all();
}

/* ��ʱ�� 1ms */
PUBLIC void  SFDectectTimer(void)
{
    int i;
    
    if(scan_sf_time < 255)
        scan_sf_time ++;
    
	for(i = 0; i < READDDOT_NUM; i++)
	{
		if(ReadDot[i].time < 255)
			 ReadDot[i].time ++;
	}
}

/* ��ȡ DCU �� PDS ��ȫ��·���ϴ��� 
 *  �߰��ֽ���Ȧ����, �Ͱ��ֽ��Ǵ�������
 *  type    : ֻ���� SORT_DCUSF_coil �� SORT_PDSSF_coil
 *  rstLast : TRUE = ������һ�ε����ݣ���ʾ���������Ѿ����ɹ���ȡ
 *  
 */
PRIVATE int8u lastFlash[READDDOT_NUM];  //��¼��һ�ε����ϴ���

PUBLIC int8u getSFFlashResult(int8u type, BOOL rstLast)
{
    int8u SFResult = 0;
    int8u SFCoil, SFContactor;
    
    if((type != SORT_DCUSF_coil)  && (type != SORT_PDSSF_coil))
        return 0;
    
    if(rstLast)
    {
        lastFlash[type    ] = 0;
        lastFlash[type + 1] = 0;
    }
    
    /* ��Ȧ���� */
    SFCoil = ReadDot[type].flashmsg_to_mms + lastFlash[type];
    if(SFCoil >= 15)
    {
        SFCoil = 15;
    }
    
    /* �������� */
    SFContactor = ReadDot[type+1].flashmsg_to_mms + lastFlash[type+1];
    if(SFContactor >= 15)
    {
        SFContactor = 15;
    }
    
    /* ����ֵ */
    SFResult = (SFCoil << 4) | SFContactor;
    
    /* ���� */
    lastFlash[type  ]   = ReadDot[type  ].flashmsg_to_mms;
    lastFlash[type+1]   = ReadDot[type+1].flashmsg_to_mms;
    
    /* ����Ѿ�ȡ�ߵ�ֵ */
    ReadDot[type  ].flashmsg_to_mms = 0;
    ReadDot[type+1].flashmsg_to_mms = 0;
}
