
#ifndef _SAVEIBPX_H_
#define _SAVEIBPX_H_
#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

//*****************************************************

#define OK                 0
#define FAIL               1

#define IBPALARM_LEN	     8                 //下传参数 个数
#define IBPALARM_BUFLEN	   (IBPALARM_LEN + 4)

#define cDefault_DCU_NUM    24                  //默认DCU 数: 24 for QD4
#define cDefault_Parameter (0xff - 0b010)       //.0 = 1 CAN双线； .1 = 0蜂鸣器默认开
#define cDefault_On_Hour    4                   // ?望灯带开机时间--小时 （Hex）   1:30-4:30 灭，其他时间亮
#define cDefault_On_Minute  30                  // ?望灯带开机时间--分钟 （Hex）
#define cDefault_Off_Hour   1                   // ?望灯带关机时间--小时 （Hex）
#define cDefault_Off_Minute 30                  // ?望灯带关机时间--分钟 （Hex）

#define cDefault_WVTotal        50              // 风阀总数
#define cDefault_WVNum_Opned    40              // 风阀全开数
#define cDefault_WVNum_Clsed    40              // 风阀全关数

#define IBPALARM_0XAA	     0xaa
#define IBPALARM_0X55	     0x55
//
#define DCUsetting_LEN     8                 //dcu 序号标志位所占长度
#define DCUsetting_BUFLEN  (DCUsetting_LEN + 3)

#define PARA_START_SEAT    0                 //保存在BUF中的开始位置
#define DCUsetting_START_SEAT  (PARA_START_SEAT + IBPALARM_BUFLEN)

#define DCUsetting_SEAT0     (DCUsetting_START_SEAT + 0)
#define DCUsetting_SEAT1     (DCUsetting_START_SEAT + 1)
#define DCUsetting_SEAT2     (DCUsetting_START_SEAT + 2)
#define DCUsetting_SEAT3     (DCUsetting_START_SEAT + 3)
#define DCUsetting_SEAT4     (DCUsetting_START_SEAT + 4)
#define DCUsetting_SEAT5     (DCUsetting_START_SEAT + 5)
#define DCUsetting_SEAT6     (DCUsetting_START_SEAT + 6)
#define DCUsetting_SEAT7     (DCUsetting_START_SEAT + 7)
#define DCUsetting_0AA_SEAT  (DCUsetting_START_SEAT + 8)
#define DCUsetting_055_SEAT  (DCUsetting_START_SEAT + 9)
#define DCUsetting_CHS_SEAT  (DCUsetting_START_SEAT + 10)



//*****************************************************
#define DownSet_Canbus       (tagDownIbpAlarmParameter.uIbpAlarmParameter.B0) //只关系到 PSC BUS总线报警
#define two_can              (DownSet_Canbus)

#define FORBID_BUZZER        (tagDownIbpAlarmParameter.uIbpAlarmParameter.B1) //MMS ，1 禁止 BUZZER 输出

#define OVERSEE_LIGHT        (tagDownIbpAlarmParameter.uIbpAlarmParameter.B2) //如果byte 0的bit2=0,则?望灯带持续点亮，不受预约时间限制，并且下载pedc参数时，pedc收到信息后不更新byte2-byte5。byte 0的bit2=1时pedc才更新byte2-byte5
   
//*****************************************************
EXTN int8u write_IbpAlarmX_To_Flash(void);
EXTN int8u Read_IbpAlarmX_From_Flash(void);
EXTN void reset_Read_IbpAlarmX(void);

EXTN int8u write_DCUsetting_To_Flash(void);
EXTN int8u Read_DCUsetting_From_Flash(void);

#endif



