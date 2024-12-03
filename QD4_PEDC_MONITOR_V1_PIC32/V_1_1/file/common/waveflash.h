
#ifndef _WAVEFLASH_H
#define _WAVEFLASH_H

//*********************************************
#define CREADDOTPERIOD      5       //读取周期 ms
#define CREADDOTNUM         120     //读取点数 ; <= 255

#define LEVEL_STABLE_TIME   60      //(300/CREADDOTPERIOD)    //300ms ,如果读的点的电平数维持 level_stable_time ，则认为电平稳定
#define SHAKE_TIME          120     //(600 / CREADDOTPERIOD)  //600ms ,结果不可> 255; 在此时间内，如果变成另一种电平且稳定，则认为是稳定前的抖动，而非闪断 ; 

extern BOOL chkDCUSF_K31PN(void);
extern BOOL chkDCUSF_K31NC(void);
extern BOOL chkPDSSF_Coil(void);;
extern BOOL chkPDSSF_Contactor(void);
//
//#define CHK_SF_RELAY_VOLTAGE()      chkDCUSF_K31PN()
//#define CHK_SF_RELAY_CONTACTOR()    chkDCUSF_K31NC()

typedef struct
{
	int8u readDotPeriod;                    //读取周期
	int8u DotStart;                         //读取点开始位置
	int8u DotEnd;                           //读取点结束位置 (该位置没有数据)
	int8u levelnum;                         //电平数
	int8u DotBuf[CREADDOTNUM / 8 + 1];      //读取点缓存
	int8u level_stable_time;                //检测到电平稳定时间
	int8u shake_time;                       //抖动的总时间
	int8u CNT;
	int8u time;                             //时长
	int8u flashNum;                         //闪断次数 ； 0 没有闪断
	
	int8u flashmsg_to_mms;                  //闪断信息给MMS
	
}tReadDot;

//*********************************************
#define READDDOT_NUM            4

#define SORT_DCUSF_coil         0       //DCU安全继电器线圈电压
#define SORT_DCUSF_contact      1       //DCU安全继电器触点电压
#define SORT_PDSSF_coil         2       //PDS安全继电器线圈电压
#define SORT_PDSSF_contact      3       //PDS安全继电器触点电压

//*********************************************

//tReadDot.CNT
#define powOnPhase        0              //检测高或低电平
#define LevelHigh         1              //稳定的高电平
#define H_testL           2              //检测到低
#define H_testH           3              //检测到高

#define LevelLow          0x81           //稳定的低电平
#define L_testL           0x82           //检测到低
#define L_testH           0x83           //检测到高

//*********************************************
//EXTN int8u scan_sf_time;
#define CSCAN_SF_TIME      5            //5ms

//ReadDot[].Time;      
#define CHANDLETIME        200          //ms

//*********************************************
//EXTN int8u sf_dcu_flash;                    //DCU记录安全回路闪断次数
//EXTN int8u sf_pds_flash;                    //PDS记录安全回路闪断次数

//EXTN int8u i2c_err_cal;                     //错误次数达到设定次数，则清除 ReadDot
//#define Cls_ReadDotNum      5              //约 5 * 5ms


//*********************************************
//
//PUBLIC void ini_DotBuf_all(void);  //初始化
//PUBLIC void HandleSF_FLASH(void);

/* 初始化函数，MCU重启后调用 */
PUBLIC void  SFDectectInit(void);   

/* 检测处理函数，主程序循环调用 */
PUBLIC void  SFDectectProc(int8u scanInterval);

/* 计时函数，主程序中断1ms调用 */
PUBLIC void  SFDectectTimer(void);

/* 读取结果函数 */
PUBLIC int8u getSFFlashResult(int8u type, BOOL rstLast);



//*********************************************
#endif
