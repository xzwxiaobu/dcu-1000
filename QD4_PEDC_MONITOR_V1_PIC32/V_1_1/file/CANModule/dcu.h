
#ifndef _DCU_H
#define _DCU_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif


/* =========================================================
 * DCU状态结构字，从DCU读取的信息
 * QD4 - 2022/5/30 点表
 * 
 */

/* 定义开关：
 * 若定义 DCU_STS_UNIONBYTES 则使用独立的按字节定义 DCU 上传的状态字（字节 0~9 ）
 */
//#define DCU_STS_UNIONBYTES

#ifndef DCU_STS_UNIONBYTES
typedef struct
{
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 0
typedef union
{
    struct
    {
#endif
        int8u Open_Command_OP1          : 1;     //BYTE 0.0 自动开门命令1
        int8u Close_Command_OP1         : 1;     //BYTE 0.1 自动关门命令1
        int8u                           : 1;     //BYTE 0.2 IBPPSL 开门命令
        int8u Open_Command_OP2          : 1;     //BYTE 0.3 自动开门命令2
        int8u Close_Command_OP2         : 1;     //BYTE 0.4 自动关门命令2
        int8u                           : 1;     //BYTE 0.5 开边门
        int8u LCB_Isolate               : 1;     //BYTE 0.6 LCB隔离模式
        int8u LCB_Auto                  : 1;     //BYTE 0.7 LCB 1=自动/0=手动模式
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts0;
#endif


#ifdef DCU_STS_UNIONBYTES
//BYTE 1
typedef union
{
    struct
    {
#endif
        int8u LCB_Manual_Open           : 1;     //BYTE 1.0 LCB手动开门命令
        int8u LCB_Manual_Close          : 1;     //BYTE 1.1 LCB手动关门命令
        int8u Solenoid_Release_R        : 1;     //BYTE 1.2 电磁铁1复位开关 R
        int8u Solenoid_Energize_R       : 1;     //BYTE 1.3 电磁铁1吸到位开关 R
        int8u Solenoid_Release_L        : 1;     //BYTE 1.4 电磁铁2复位开关 L
        int8u Solenoid_Energize_L       : 1;     //BYTE 1.5 电磁铁2吸到位开关 L
        int8u Door_Close_Switch_R       : 1;     //BYTE 1.6 滑动门扇1关到位开关(R)
        int8u Door_Close_Switch_L       : 1;     //BYTE 1.7 滑动门扇2关到位开关(L)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts1;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 2
typedef union
{
    struct
    {
#endif
        int8u Door_Fully_Open_Switch_R  : 1;     //BYTE 2.0 滑动门扇1开门到位开关 (虚拟的)(R)
        int8u Door_Fully_Open_Switch_L  : 1;     //BYTE 2.1 滑动门扇2开门到位开关 (虚拟的)( L)
        int8u Door_EED1_Switch_R        : 1;     //BYTE 2.2 应急门扇1光电开关(右扇)
        int8u Door_EED2_Switch_L        : 1;     //BYTE 2.3 应急门扇2光电开关(左)
        int8u Safety_Loop_Status_Dect   : 1;     //BYTE 2.4 安全回路状态(双切检测)
        int8u C_L_Switch_Status_Input   : 1;     //BYTE 2.5 安全回路状态(输入口计算)
        int8u Manual_Release_Switch_R   : 1;     //BYTE 2.6 手动解锁1开关(R)
        int8u Manual_Release_Switch_L   : 1;     //BYTE 2.7 手动解锁2开关(L)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts2;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 3
typedef union
{
    struct
    {
#endif
        int8u MSD1_Open_Switch          : 1;    //BYTE 3.0 MSD1 端头门1光电开关(R)
        int8u MSD2_Open_Switch          : 1;    //BYTE 3.1 端头门2光电开关(L)
        int8u EED_ByPass                : 1;    //BYTE 3.2 应急门旁路 (Emergency Egress Door))
        int8u                           : 1;    //BYTE 3.3  
        int8u OutputFuse1_On            : 1;    //BYTE 3.4 输出保险丝1导通状态（J4右上方那个）,true表示导通，false表示断开了
        int8u InputFuse1_Off            : 1;    //BYTE 3.5 输入保险丝1导通状态（J3右上方那个）
        int8u InputFuse2_Off            : 1;    //BYTE 3.6 输入保险丝2导通状态（J2拨码开关右边那个）
        int8u OutputFuse2_On            : 1;    //BYTE 3.7 输出保险丝2导通状态（J1上面那个）
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts3;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 4
typedef union
{
    struct
    {
#endif
        int8u DCU_NumOfWV               : 1; //BYTE 4.0 风阀数量标记(0=2风阀，1=3风阀)
        int8u                           : 1; //BYTE 4.1 
        int8u                           : 1; //BYTE 4.2 
        int8u                           : 1; //BYTE 4.3 
        int8u                           : 1; //BYTE 4.4 未定义
        int8u                           : 1; //BYTE 4.5 未定义
        int8u                           : 1; //BYTE 4.6 未定义
        int8u Dual_Motor_Configure      : 1; //BYTE 4.7 马达数(1=双马达，0=单马达)
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts4;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 5
typedef union
{
    struct
    {
#endif
        int8u DCUMode       : 4; //BYTE 5.0~3 DCU模式
        int8u DoorMode      : 4; //BYTE 5.4~7 门模式 
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts5;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 6
typedef union
{
    struct
    {
#endif
        int8u WVCmdOpn                  : 1; //BYTE 6.0 收到开风阀命令
        int8u WVCmdCls                  : 1; //BYTE 6.1 收到关风阀命令
        int8u WV1Opened                 : 1; //BYTE 6.2 风阀1开到位
        int8u WV1Closed                 : 1; //BYTE 6.3 风阀1关到位
        int8u WV2Opened                 : 1; //BYTE 6.4 风阀2开到位
        int8u WV2Closed                 : 1; //BYTE 6.5 风阀2关到位
        int8u WV3Opened                 : 1; //BYTE 6.6 风阀3开到位
        int8u WV3Closed                 : 1; //BYTE 6.7 风阀3关到位
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts6;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 7
typedef union
{
    struct
    {
#endif
        int8u err_init_door  : 1; //BYTE 7.0 err_init_door
        int8u err_open_door  : 1; //BYTE 7.1 err_open_door
        int8u err_close_door : 1; //BYTE 7.2 err_close_door
        int8u err_unlook_prc : 1; //BYTE 7.3 err_unlook_prc
        int8u err_mt1_origin : 1; //BYTE 7.4 err_mt1_origin
        int8u err_mt2_origin : 1; //BYTE 7.5 err_mt2_origin
        int8u err_mag1_flag  : 1; //BYTE 7.6 err_mag1_flag
        int8u err_mag2_flag  : 1; //BYTE 7.7 err_mag2_flag
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts7_Err1;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 8
typedef union
{
    struct
    {
#endif
        int8u err_mt1_flag              : 1; /* BYTE 8.0 : err_mt1_flag                 */
        int8u err_mt2_flag              : 1; /* BYTE 8.1 : err_mt2_flag                 */
        int8u err_mt1_hot_flag          : 1; /* BYTE 8.2 : err_mt1_hot_flag             */
        int8u err_mt2_hot_flag          : 1; /* BYTE 8.3 : err_mt2_hot_flag             */
        int8u err_hall_1_flag           : 1; /* BYTE 8.4 : err_hall_1_flag              */
        int8u err_hall_2_flag           : 1; /* BYTE 8.5 : err_hall_2_flag              */
        int8u err_end_door_ov_time_flag : 1; /* BYTE 8.6 : err_end_door_ov_time_flag    */
        int8u err_saft_loop_flag        : 1; /* BYTE 8.7 : err_saft_loop_flag           */
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts8_Err2;
#endif

#ifdef DCU_STS_UNIONBYTES
//BYTE 9
typedef union
{
    struct
    {
#endif
        int8u err_cmd_flag              : 1; /* BYTE 9.0 : err_cmd_flag */
        int8u err_fuse                  : 1; /* BYTE 9.1 : 保险丝故障, byte 3的bit4、5、6、7任何一个为false，这里报true */
        int8u err_CPUOverHeat           : 1; /* BYTE 9.2 : 主CPU过温报警 */
        int8u err_OpnClsDoorOT          : 1; /* BYTE 9.3 : 开关门超时报警 */
        int8u err_SFLoopFlash           : 1; /* BYTE 9.4 : 安全回路闪断 */
        int8u err_WV1OverTime           : 1; /* BYTE 9.5 : 风阀1开/关超时报警 */
        int8u err_WV2OverTime           : 1; /* BYTE 9.6 : 风阀2开/关超时报警 */
        int8u err_WV3OverTime           : 1; /* BYTE 9.7 : 风阀3开/关超时报警 */
#ifdef DCU_STS_UNIONBYTES
    }flags;
    
    int8u data;
}tDCU_Sts9_Err3;
#endif


#ifndef DCU_STS_UNIONBYTES
}tDCU_AllStsFlags;
#endif


#define DEF_UPDATE_CAN12_STATE_TIME                 100             //ms

//***********************************************************
EXTN void   SetpBufBit(pINT8U ptr , int8u bit);
EXTN void   ClrpBufBit(pINT8U ptr , int8u bit);
EXTN int8u  pBufIfOnLine(pINT8U ptr ,int8u uBit);                   //cDcuOnLineF 在线 ，cDcuOffLineF 离线
EXTN void   SetDcuOnLine(int8u uID_DCU);                            //dcu 设置在线
EXTN void   SetOffLineDcu(int8u uID_DCU);                           //dcu 设置离线
EXTN int8u  DcuIfOnLine(int8u uID_DCU);                             //cDcuOnLineF 在线 ，cDcuOffLineF 离线
EXTN int8u  ptrDcuIfOnLine(pINT8U ptr , int8u Id);                  //cDcuOnLineF 在线 ，cDcuOffLineF 离线
EXTN int8u  FindNextDcu(pINT8U ptr ,pINT8U pID_DCU ,int8u DcuNum);  //查找下一个 DCU ,找到 最大的 DCU 为止，不可循环寻找
EXTN void   DcuInc(pINT8U pID_DCU ,int8u DcuNum);                                    //DCU 加一 ，超出最大值循环到最小值
EXTN int16u GetCrc(pINT8U ptr, int16u Len);
EXTN int16u _GetCrc(pINT8U ptr, int16u Len);                            //中断用
EXTN void DcuCan12State(BOOL wait);                                 //将DCU can12 的信息传递给 uniDcuData



PUBLIC int8u* getAllDCUOnLineSts(int8u DCUs, int8u chkStatus, int8u* setting);
PUBLIC BOOL getPSDInhibitTrain(int8u *pInhibit, int len);

PUBLIC int8u findFirstBit(pINT8U p, int8u Len);
PUBLIC int8u findLastBit(pINT8U p, int8u Len);
PUBLIC int8u calcSetting1Num(pINT8U p, int8u Len);

#endif



