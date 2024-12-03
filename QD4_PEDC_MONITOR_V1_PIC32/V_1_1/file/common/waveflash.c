
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
//位设置
PRIVATE void SetBUFBit(int8u *p,int8u bit)
{
    p  += bit>>3;
    *p |= 1<<(bit & 7);
}

//位清除
PRIVATE void ClrBUFBit(int8u *p,int8u bit)
{
    p  += bit>>3;
    *p &= ~(1<<(bit & 7));
}

//位测试
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

/* 初始化 
 * 清所有点缓存
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
 * 清指定扫描点缓存
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
    level == 0 检测低电平； 1 检测高电平
    
out:
.15  == 1 缓冲还有数据
.0-.7 获取个数

*/
PRIVATE int16u getDotLevelnum(int8u DotSort, int8u level)
{
    int16u num = 0;
    
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];

    if((sort->DotStart >= CREADDOTNUM) || (sort->DotEnd >= CREADDOTNUM) || (sort ->DotStart == sort->DotEnd))
    {//保护用
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
            DCU安全回路触点  闪断程序
        OR  DCU安全回路端电压闪断程序
        OR  PDS安全回路触点  闪断程序
        OR  PDS安全回路端电压闪断程序
*/
PRIVATE void HandleSF_FLASH(void)
{
    int8u i;

    for(i = 0; i < READDDOT_NUM; i ++)
    {
        get_SF_FLASH(i); 
    }

}

//检测
//out: flashNum
PRIVATE void Handle_flash(int8u DotSort)
{
    int8u loop;
    int16u ret;
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];
    int8u temp0;

    for(loop = 0; loop < 255; loop ++) //保护用的循环
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
                {//检测到高电平
                    sort->CNT = LevelHigh;
                }   
                else if(ret & BUFHAVEDATA)
                {//检测到低电平
                    sort->CNT = LevelLow;
                }   
                
                break;
                
            case LevelHigh:          //稳定的高电平
                ret = getDotLevelnum(DotSort, 1);
                if((ret & BUFHAVEDATA))
                {//检测到低脉冲
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
                {//长低电平
                
                    sort->CNT = LevelLow;
                    
                    break;
                    
                }   
                
                if(sort->shake_time >= SHAKE_TIME)
                {//超过总时间后，把闪断次数上传MMS
                    
                    sort->flashmsg_to_mms += sort->flashNum;
                    sort->flashNum = 0;
                    
                }   
                if((ret & BUFHAVEDATA))
                {//检测到高脉冲
                    sort->CNT = H_testH;
                    sort->level_stable_time = 0;
                }   
                break;
            
            case    H_testH:
                ret = getDotLevelnum(DotSort, 1);
    
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->shake_time >= SHAKE_TIME)
                {//超过总时间后，把闪断次数上传MMS
                    
                    sort->flashmsg_to_mms += sort->flashNum;
                    sort->flashNum = 0;
                    
                }   
                
                if(sort->level_stable_time >= LEVEL_STABLE_TIME)
                {//长高电平
                
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
            case LevelLow:          //稳定的低电平
                ret = getDotLevelnum(DotSort, 0);
                if((ret & BUFHAVEDATA))
                {//检测到高脉冲
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
                {//长高电平
                
                    sort->CNT = LevelHigh;
                    
                    break;
                    
                }   
                
                if((ret & BUFHAVEDATA))
                {//检测到低脉冲
                    sort->CNT = L_testL;
                    sort->level_stable_time = 0;
                }   
                break;
            
            case    L_testL:
                ret = getDotLevelnum(DotSort, 0);
    
                addNum((int8u *)&sort->level_stable_time, ret & 0xff);
                addNum((int8u *)&sort->shake_time, ret & 0xff);
    
                if(sort->level_stable_time > LEVEL_STABLE_TIME)
                {//长低电平
                
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
安全继电器端电压:  K31P/K31N     I48  RD.7              I10        RB14       I12  RB3   K31K32_PN
                   K31K32        I5   0-IO4_3(K-S11)    I4 (K-S6)  RB11       I5   RB7   K31K32_NC(K-S11)
                   K33           I9   0-IO3_7(K-S12)    I1 (K-S7)  RB2        I2   RG9   K33_KE_NO(K-S12)

通过 K31K32_PN 和 K31K32_NC 判断继电器是否有故障
通过 K31K32_PN 得到安全回路端电压闪断次数
通过 K31K32_NC 得到安全回路触点闪断次数
*/

//#define EMU_TEST_FLASH
PRIVATE void scan_sf_dot(void)
{
    /* DCU SF
     * TRUE 表示高电平（没有输入触发）
     *  */
    saveDot(SORT_DCUSF_coil   , chkDCUSF_K31PN()        == TRUE? 0: 1);
    saveDot(SORT_DCUSF_contact, chkDCUSF_K31NC()        == TRUE? 0: 1);
    
    /* PDS SF 
     *  */
    saveDot(SORT_PDSSF_coil   , chkPDSSF_Coil()         == TRUE? 0: 1);
    saveDot(SORT_PDSSF_contact, chkPDSSF_Contactor()    == TRUE? 0: 1);
    
}

//************************************************
//读取点，存入缓存
/*
;in: bit: 0 存入0； 1 存入 1

*/
PRIVATE void saveDot(int8u DotSort, int8u bit)
{
    tReadDot * sort;
    sort = (tReadDot * ) &ReadDot[DotSort];
    
    if((sort->DotStart >= CREADDOTNUM) || (sort->DotEnd >= CREADDOTNUM) || (sort ->DotStart == sort->DotEnd))
    {//保护用
            sort->DotStart = 0;
            sort->DotEnd   = 0;
    }   

    if(bit)
    {//存入1
        SetBUFBit((int8u *)&sort->DotBuf ,sort->DotEnd);
    }
    else
    {//存入0
        ClrBUFBit((int8u *)&sort->DotBuf ,sort->DotEnd);
    }   
            
    sort->DotEnd ++;
    if(sort->DotEnd >= CREADDOTNUM)
    {
        sort->DotEnd = 0;
    }   
    
}

//合并取得的两闪断 (高四位 + 低四位)
/*
* val     : 上次合并的保存地址
sort_low4:  合并在低四位
sort_high4: 合并在高四位
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

//清所有的 flashmsg_to_mms
PRIVATE void cls_all_flashmsg_to_mms(void)
{
    int8u i;
    for(i = 0; i < READDDOT_NUM; i ++)
    {
        ReadDot[i].flashmsg_to_mms    = 0;
    }
}



/* =============================================================================
 * 安全回路闪断检测
 *  scanInterval 是扫描时间间隔，默认 CSCAN_SF_TIME
 * 注意：
 *  闪断值是半字节保存，最大为15
 *  当检测到闪断以后，需要一直保留直到被主机成功读取，然后将闪断值清除
 *  如果
 */
PUBLIC void SFDectectProc(int8u scanInterval)
{
    if(scan_sf_time < scanInterval);
        return;
        
    scan_sf_time = 0;
    
    scan_sf_dot();
    
    HandleSF_FLASH();
}

/* 初始化 */
PUBLIC void SFDectectInit(void)
{
    ini_DotBuf_all();
}

/* 计时器 1ms */
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

/* 读取 DCU 或 PDS 安全回路闪断次数 
 *  高半字节线圈闪断, 低半字节是触点闪断
 *  type    : 只能是 SORT_DCUSF_coil 或 SORT_PDSSF_coil
 *  rstLast : TRUE = 重置上一次的数据，表示闪断数据已经被成功读取
 *  
 */
PRIVATE int8u lastFlash[READDDOT_NUM];  //记录上一次的闪断次数

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
    
    /* 线圈闪断 */
    SFCoil = ReadDot[type].flashmsg_to_mms + lastFlash[type];
    if(SFCoil >= 15)
    {
        SFCoil = 15;
    }
    
    /* 触点闪断 */
    SFContactor = ReadDot[type+1].flashmsg_to_mms + lastFlash[type+1];
    if(SFContactor >= 15)
    {
        SFContactor = 15;
    }
    
    /* 闪断值 */
    SFResult = (SFCoil << 4) | SFContactor;
    
    /* 备份 */
    lastFlash[type  ]   = ReadDot[type  ].flashmsg_to_mms;
    lastFlash[type+1]   = ReadDot[type+1].flashmsg_to_mms;
    
    /* 清除已经取走的值 */
    ReadDot[type  ].flashmsg_to_mms = 0;
    ReadDot[type+1].flashmsg_to_mms = 0;
}
