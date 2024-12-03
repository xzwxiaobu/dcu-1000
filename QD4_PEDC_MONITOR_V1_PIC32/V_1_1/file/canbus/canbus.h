
#ifndef _CANOPEN_H
#define _CANOPEN_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif
/****************** DATA TYPES Complier Specific ******************************/

typedef struct tagKBITINT
{
	int8u B0  : 1;
	int8u B1  : 1;
	int8u B2  : 1;
	int8u B3  : 1;
	int8u B4  : 1;
	int8u B5  : 1;
	int8u B6  : 1;
	int8u B7  : 1;
	int8u B8  : 1;
	int8u B9  : 1;
	int8u B10 : 1;
	int8u B11 : 1;
	int8u B12 : 1;
	int8u B13 : 1;
	int8u B14 : 1;
	int8u B15 : 1;
} KBITINT;

typedef union
{
	unsigned int Dbyte;
	KBITINT Dfield;
} INT16UField;


EXTN union
{
	struct
	{
		unsigned b0  : 1;     //自动开门命令1
		unsigned b1  : 1;     //自动关门命令1
		unsigned b2  : 1;     //自动柔性关门1
		unsigned b3  : 1;     //自动开门命令2
		unsigned b4  : 1;     //自动关门命令2
		unsigned b5  : 1;     //自动柔性关门2
		unsigned b6  : 1;     //bk
		unsigned b7  : 1;     //bk

		unsigned b8  : 1;     //隔离模式
		unsigned b9  : 1;     //自动/手动模式
		unsigned b10 : 1;     //手动开门命令
		unsigned b11 : 1;     //手动关门命令
		unsigned b12 : 1;     //右滑动门复位开关
		unsigned b13 : 1;     //左滑动门复位开关
		unsigned b14 : 1;     //右滑动门开门到位开关 (虚拟的)
		unsigned b15 : 1;     //左滑动门开门到位开关 (虚拟的)

		unsigned b16 : 1;     //右电磁铁复位开关
		unsigned b17 : 1;     //右电磁铁到位开关
		unsigned b18 : 1;     //左电磁铁复位开关
		unsigned b19 : 1;     //左电磁铁到位开关
		unsigned b20 : 1;     //右手动解锁开关
		unsigned b21 : 1;     //左手动解锁开关
		unsigned b22 : 1;     //乘客探测器旁路开关
		unsigned b23 : 1;     //应急门光电开关

		unsigned b24 : 1;     //司机门光电开关
		unsigned b25 : 1;     //端头门光电开关
		unsigned b26 : 1;     //右门指示灯检测开关
		unsigned b27 : 1;     //左门指示灯检测开关
		unsigned b28 : 1;     //门后光电开关1
		unsigned b29 : 1;     //门后光电开关2
		unsigned b30 : 1;     //门后光电开关3
		unsigned b31 : 1;     //安全回路状态

		unsigned b32_35 : 4;  //DCU工作模式
		unsigned b36_39 : 4;  //门状态

//报警
		unsigned b40 : 1;    //err_init_door
		unsigned b41 : 1;    //err_open_door
		unsigned b42 : 1;    //err_close_door
		unsigned b43 : 1;    //err_unlook_prc
		unsigned b44 : 1;    //err_mt1_origin
		unsigned b45 : 1;    //err_mt2_origin
		unsigned b46 : 1;    //err_mag1
		unsigned b47 : 1;    //err_mag2

		unsigned b48 : 1;    //err_mt1       r
		unsigned b49 : 1;    //err_mt2       l
		unsigned b50 : 1;    //err_mt1_hot   r
		unsigned b51 : 1;    //err_mt2_hot   l
		unsigned b52 : 1;    //err_hall1     r
		unsigned b53 : 1;    //err_hall2     l
		unsigned b54 : 1;    //err_end_door_ov_time
		unsigned b55 : 1;    //err_saft_loop_flag

		unsigned b56 : 1;    //err_cmd_flag
		unsigned b57 : 1;    //err_lamp_r
		unsigned b58 : 1;    //err_lamp_l
		unsigned b59 : 1;    //err_after_door_sw1
		unsigned b60 : 1;    //err_after_door_sw2
		unsigned b61 : 1;    //err_after_door_sw3
		unsigned b62 : 1;    //
		unsigned b63 : 1;    //

		unsigned b64_79   : 16;   //右滑动门位置
		unsigned b80_95   : 16;   //左滑动门位置
		unsigned b96_111  : 16;   //右滑动门速度
		unsigned b112_127 : 16;   //左滑动门速度
		unsigned b128_143 : 16;   //电源电压
		unsigned b144_159 : 16;   //
		unsigned b160_175 : 16;   //
	} bits;

	struct
	{
		int8u byte0;
		int8u byte1;
		int8u byte2;
		int8u byte3;
		int8u byte4;
		int8u byte5;
		int8u byte6;
		int8u byte7;

		int16u word4;
		int16u word5;
		int16u word6;
		int16u word7;
		int16u word8;
		int16u word9;
		int16u word10;
		
		int8u CanBusCheck;  //最后一字节 CAN 通讯 效验和用
		int8u reserve;      //最后一字节 CAN 通讯 效验和用
		
	}bytes ;
} DcuStateUp;           //上传 PEDC Dcu 状态 ,最后一字节 can 通讯效验和用


EXTN struct
{
	int16u dcu_para0;    //开门宽度(右)
	int16u dcu_para1;    //开门宽度(左)
	int8u dcu_para2;     //开门时间
	int8u dcu_para3;     //关门时间
	int8u dcu_para4;     //受阻力等级(关门)
	int8u dcu_para5;     //受阻力等级(开门)
	int8u dcu_para6;     //端门报警延时
	int8u dcu_para7;     //遇障重开次数
	int8u dcu_para8;     //重开后关闭等待时间
	int8u dcu_para9;     //遇障重开宽度
	int8u dcu_para10;    //状态灯开门时闪烁频率
	int8u dcu_para11;    //状态灯关门时闪烁频率
	int8u dcu_para12;    //状态灯故障时闪烁频率
	int8u dcu_para13;    //手动解锁后关门延时时间

	int8u CanBusCheck;   //最后一字节 CAN 通讯 效验和用
} DcuParaDown;         //PEDC 读取 DCU参数 BUF


EXTN int8u OpenSpeedCurve[CURVE_LEN + 1];             //开门速度曲线 ,最后一字节 can 通讯效验和用
EXTN int8u CloseSpeedCurve[CURVE_LEN + 1];            //关门速度曲线 ,最后一字节 can 通讯效验和用

EXTN int8u can_down_dcuPara[ONE_DCU_PARA_LEN + 1];    //pedc 下载 DCU参数 并立标志New_para， 最后一字节 CAN 通讯 效验和用
EXTN int8u DcuONLINE[2];                              //是否在线测试

//**************************************************************************

#define ican_read_OK   fgTransOK   //读ok
#define ican_write_OK  fgTransOK   //写ok
#define ican_read_Err  fgAbort     //读err
#define ican_write_Err fgAbort     //写rrr
#define ican_read_ing  fgRunning   //读进行中
#define ican_write_ing fgRunning   //写进行中

//读取 功能状态
#define Bus_DownDcuParaState()            ChkTransState(FUNID_Down_MoreDcuParameter)
#define Bus_ReadDcuParaState()            ChkTransState(FUNID_Up_MoreDcuParameter)
#define Bus_ReadDcuState_State()          ChkTransState(FUNID_UpOneDcuState)
#define Bus_ReadOpenCurveState()          ChkTransState(FUNID_Up_OpenDoorCurve)
#define Bus_ReadCloseCurveState()         ChkTransState(FUNID_Up_CloseDoorCurve)

//清除功能 状态
#define clsDCU_DownDcuParaState()  ClrTransState(FUNID_Down_MoreDcuParameter);

EXTN INT16UField  Flag0;
#define New_para                    Flag0.Dfield.B0      //1 有参数下载
#define State_Reading               Flag0.Dfield.B1      //1 正在读取 状态 ，22 字节
#define Para_Reading                Flag0.Dfield.B2      //1 正在读取 参数 ,16 字节
#define OpenSpeedCurve_Reading      Flag0.Dfield.B3      //1 正在读取 开门曲线
#define CloseSpeedCurve_Reading     Flag0.Dfield.B4      //1 正在读取 关门曲线

EXTN void CanFun(void);    //CAN 通讯，键值 ，参数下载

#endif

//--------------------------------------
