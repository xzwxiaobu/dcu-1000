#ifndef _GLB_CAN_API_
#define _GLB_CAN_API_

#include "plib.h"

//**********************************************************************************************
//                                                                                             *
//                                           CAN 功能基本设置                                  *
//                                                                                             *
//**********************************************************************************************
// master or slave define
#ifndef CDRV_TYPE
#error 'CDRV_TYPE not defined!'
#endif

// ========= CAN 冗余的实现方式, 仅仅针对 Master 有效
// TURE  -- 两个CAN在其中一个失败以后，自动切换到另一个
// FALSE -- 两个CAN同步通讯, 数据是完全相同的传输
#define CAN_MANAGE_TOGGLE   TRUE
#define CAN_ERR_CNT_MAX     15          //连续 N 次错误认为 CAN 有问题
#if (CAN_ERR_CNT_MAX > 15)
#error "CAN_ERR_CNT_MAX must be less than 16"
#endif

// ========= 缓存数量
#if (CDRV_TYPE == CDRV_PEDC)
	//PEDC
	#define MAX_RX_BUF          20        //接收缓存,  由于读写指针相等是认为没有数据, 因此实际上可以用的缓存少了一级
	#define MAX_TX_BUF          12        //发送缓存
#else
	//DCU or MCP
	#define MAX_RX_BUF          12        //接收缓存
	#define MAX_TX_BUF          12        //发送缓存
#endif

// ========= 数据传输进程的数量, 必须大于 CanApi.h 中的命令个数
#define MAX_TRANS_THREAD       16

// ========= 数据传送超时: 定义为相邻两个数据包的时间间隔
#define TRANS_TIMEROVER        TRUE      //TRUE - enable, FALSE - disable
#define MASTER_DFT_OV          30        //ms, 这是最小值, 根据功能码的优先级调整, 优先级越高, 超时设置越低
#define SLAVE_DFT_OV           1100      //ms, 对于 Slave 实际意义不大

#define MASTER_BROAD_DLY       5         //ms, 当 master 发送广播命令时, 不需要 Slave 应答, master 定时发送

// ========= 数据传送超时: 定义全包中各个传输段可以重复传输的个数。 如：24字节传输，分4个传输段，如果其中一个传输段传输失败，则主机重新发送该传输段，但重发的各个传输段次数之和有限制，防止死循环。
#define Min_RepeatPacket       3
#define MAX_RepeatPacket       40

// ========= CAN 硬件缓冲的使用
#define FILL_HWTXBUF_ALL    FALSE        //从软件发送缓冲取数据, 一次将所有硬件发送缓冲填满(有足够的发送数据)
#define CAN_TX_HW_BUF_1ST   FLASE        //TRUE -- CdrvWriteMsg 函数优先查找空的硬件发送缓冲进行发送
													  //FALSE - CdrvWriteMsg 函数仅仅将发送数据保存到软件发送缓冲

// ========= Can 复位控制
#define BUS_TIMEROVER       TRUE         //
#define CAN_BUS_ERR_TIME    500          //unit = 10ms， 如果指定时间内没有接收到数据则认为 Can 总线错误计时器,

// ========= PEDC 通讯计时打印（通过CAN发送，监控器查看, 仅供调试)
#if (CDRV_TYPE == CDRV_PEDC)
	#define PRINT_TIME      FALSE
	#define PRINT_TIME_ID   0x55          //打印时间占用的 ID 号，不能与节点 ID 相同，小于 99 便于监控
#else
	#define PRINT_TIME      FALSE
#endif

// ========= PEDC 调试错误信息
#define    DEBUG_PRINT_ID  0x56

// ========= PEDC 发送: 每发送 5 条消息后，必须等待所有硬件缓冲空才继续发送
// 针对现场测试 “读曲线时会造成整侧DCU瞬时断线” 做处理, 避免读曲线占有总线时间太长
#if (CDRV_TYPE == CDRV_PEDC)
  #define TX_WAIT_EMPTY   TRUE
  #define TX_WAIT_CNT     3
#else
  #define TX_WAIT_EMPTY   FALSE
#endif


// ==========================================================================
//masked by lly 2011-6-9, use CAN1 / CAN2, please see : CAN_MODULE of PIC32 lib
//#define CANBUS_0           0
//#define CANBUS_1           1

//#define NO_CAN_BUS2        //不允许 CAN2 功能 (SZ4 没有CAN2，且 CAN2 的IO口用于马达控制，必须禁止 CAN2)

// Can Spec define
#define CAN20A             1
#define CAN20B             2
#define CDRV_CAN_SPEC      CAN20A          //only support CAN20A now!!!

//**********************************************************************************************
//                                                                                             *
//                                          定义                                               *
//                                                                                             *
//**********************************************************************************************
//dwID    : bit10-7 -- 命令字, bit6-0 -- Node id, 此字节是发送数据包的ID
//        : 数据包第一个字节信息是扩展命令, 启动/完成/终止/应答
//        : 因此, 每次传输中, 数据包中的有效数据最大只有 7 字节, 此信息是接收数据长度 (DLC - 1)
//说明    : 命令只有 Client 发送给 Server, Server 不能主动发送命令而只能应答命令
//          这里定义的数值不要随意改动, 否则需要调整表格 T_CmdParaTable/CmdTransType
//        : 开门/关门/查询命令,

#define GET_FUNID(a)        (((a)>>7)&0x0F)
#define GET_NODEID(a)       ((a)&0x7F)
#define SET_FUNID(a)        (((a)&0x0F)<<7)
#define SET_COBID(a,b)      (SET_FUNID(a) | b)

#define TRANS_DIR           0x80
#define RSV_CMD_S           120
#define BYTES_PER_PACK      7           //数据包中的最大有效数据长度(字节), 必须 < 8

//查询命令: FUNID_QUERY
#define FUN_QUERY_VER       1           //查询Slave软件版本, 返回 YY/MM/DD/? MAINVERSION/SUBVERSION 6字节
#define FUN_QUERY_CS        2           //查询Slave软件CHECKSUM,

//prototypes
typedef struct
{
	int32u dwId;           //节点ID
	int8u  u8State;        //状态
	int8u  u8PacketCnt;    //数据包计数器

	int8u  u8CanNo;
	int8u  u8TimeCnt;      //测试用，记录通讯的时间，仅仅对 PEDC 有效，计时从 PEDC 调用 CapiStartTrans 开始到结束

	int16u  wTickTot;       //计时(系统计时基准 1ms)，从启动到结束的计时
	int16u  wTick;          //计时(系统计时基准 1ms)，每个数据包之间的时间间隔
	int16u  wTransOV;       //超时设定(系统计时基准 1ms), 0 -- 不检测

	int16u  wRxBytes;       //已经接收的数据长度
	int16u  wTxBytes;       //已经发送的数据长度

	int16u  wBufLen;        //接收或发送数据的长度
	int8u* pBufPtr;         //接收或发送数据的首地址
	
	int8u  wPrevPacketRxOrTxLen;//上一次接收或发送的包长度
	int8u  wRepeatPacket;       //主机重发包次数
	
} tThreadPara;

//-----------------------------------------------
#define DCUID_CONFLICTED    0xEF        // V1.8 : DCU ID冲突标记
typedef struct
{
	int8u  SlvCanNo  [BYTES_SLV_FLAG];      //1 -- CanBus1, 0 -- CanBus0
	int8u  SlvCan0Err[BYTES_SLV_FLAG];      //1 -- CanBus0 错误
	int8u  SlvCan1Err[BYTES_SLV_FLAG];      //1 -- CanBus1 错误
	int8u  IDConflict[BYTES_SLV_FLAG];      //1 -- CAN ID 冲突 for dcu-can V1.8

	int8u  ErrCnt[BYTES_SLV_NUM];       //CanBus 的错误次数, bit[7:4]用于CAN1, bit[3:0]用于CAN0
}tCanManage;


typedef struct
{
		int32u   m_CanId;
		int8u    m_MsgType;
		int8u    m_bSize;
		int8u    m_bData[8];
} tCdrvMsg;

// ==========================================================================
extern  int16u         wCanBusTime[];

extern  tThreadPara  aTransThread1[MAX_TRANS_THREAD];    //CAN1 传输进程
extern  tThreadPara  aTransThread2[MAX_TRANS_THREAD];    //CAN2 传输进程

extern 	int8u  CalcChkSumLEN;          //== 0 16位长； != 0 8位长
// ==========================================================================
extern void  CapiFreeAllThread (int8u u8CanNo);
extern int8u  CalcChkSum(int8u* Buf, int16u u8Size);
extern void  CapiCanDebugPrint(int8u * pData, int8u Len, int8u id);

	extern void CapiInitCanManage(void);
	extern int8u CanOnLine[2];
	extern tCanManage CanManage;
        extern int8u  sendDCUSysTime[];

	#define GET_CAN1_ERR_PTR()       (int8u*)&CanManage.SlvCan0Err
	#define GET_CAN2_ERR_PTR()       (int8u*)&CanManage.SlvCan1Err

	#define GET_CURRENT_CHNL(id)   ChkRamBit(&CanManage.SlvCanNo[0],  id)
	#define TOG_CAN_CHNL(id)       TogRamBit(&CanManage.SlvCanNo[0],  id)
	#define SET_CUR_CAN1(id)       ClrRamBit(&CanManage.SlvCanNo[0],  id)
	#define SET_CUR_CAN2(id)       SetRamBit(&CanManage.SlvCanNo[0],  id)

	#define CHK_CAN1_ERR(id)       ChkRamBit(&CanManage.SlvCan0Err[0], id)
	#define SET_CAN1_ERR(id)       SetRamBit(&CanManage.SlvCan0Err[0], id)
	#define CLR_CAN1_ERR(id)       ClrRamBit(&CanManage.SlvCan0Err[0], id)

	#define CHK_CAN2_ERR(id)       ChkRamBit(&CanManage.SlvCan1Err[0], id)
	#define SET_CAN2_ERR(id)       SetRamBit(&CanManage.SlvCan1Err[0], id)
	#define CLR_CAN2_ERR(id)       ClrRamBit(&CanManage.SlvCan1Err[0], id)

        /* dcu-can V1.8 */
	#define CHK_IDCOFLICT(id)      ChkRamBit(&CanManage.IDConflict[0], id)
	#define SET_IDCOFLICT(id)      SetRamBit(&CanManage.IDConflict[0], id)
	#define CLR_IDCOFLICT(id)      ClrRamBit(&CanManage.IDConflict[0], id)

// 数据段传输状态 tThreadPara . u8State
#define fgRunning         (1<<7)                                              //1 -- 进程被使用
#define fgUpload          (1<<6)                                              //1 -- 上传(从主机方向看)
#define fgDnload          (0<<6)                                              //0 -- 下载(从主机方向看)
#define fgAbort           (1<<5)                                              //1 -- 数据传输异常终止
#define fgTransOK         (1<<4)                                              //1 -- 传输完成
#define fgCmdFlag         (1<<1)
#define fgResult          (1<<0)                                              //有结果处理

//**********************************************************************************************
//                                                                                             *
//                                      API 函数定义                                           *
//                                                                                             *
//**********************************************************************************************
//===== Can 初始化, 主程序中必须调用
//  参数: 无
//  返回: 无
extern void  TgtInitCan (void);

//===== Can 相关功能：u8CanNo 是CAN通道号, CAN1 或 CAN2
extern void  CdrvReset (CAN_MODULE u8CanNo,int8u u8RstCanDrv);
extern void  CdrvInterruptHandler (int8u u8CanNo);
extern int8u CdrvCheckTransmit(CAN_MODULE u8CanNo);
extern int8u CdrvReadMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p);
extern int8u CdrvWriteMsg (CAN_MODULE u8CanNo, tCdrvMsg * pCanMsg_p);

//===== 提供给 Can 的时钟函数, 在 1ms 中断中调用
//  参数: 无
//  返回: 无
extern void  SysTick1Ms(void);

//===== Can 通讯处理, 主循环调用
//  参数: 无
//  返回: 无
extern int8u  CapiProcess(void);

//===== 启动数据传输进程
//  参数: u8Fun 是功能号, u8NodeId 是节点ID, len 是数据长度, pBuf 是指向传输的数据指针
//  返回: TRUE = 成功, FALSE = 失败
extern int8u  CapiStartTrans(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);
extern int8u  CapiTransExtCmd(int8u u8Fun, int8u u8NodeId, int16u len, int8u *pBuf);

//===== 取得指定传输进程当前状态,
//  参数: u8FunId 是功能号
//  返回: fgRunning(进行中) / fgAbort(错误) / fgTransOK(成功), 如果不是这三种状态则返回 0
extern int8u  ChkTransState(int8u u8FunId);

//===== 清除指定传输进程当前状态
//  参数: u8FunId 是功能号
//  返回: 无
extern void  ClrTransState(int8u u8FunId);

//===== 位操作: 位设置, 位清除, 位取反, 位判断
//  参数: p 是存储器的指针(允许是一个连续的区域), bit 是存储器的位(最大是 255),
//  返回: 除了位判断返回 0或1， 其它没有返回值
//  注意: 函数不检查参数的边界 !!!
extern void  SetRamBit(int8u *p,int8u bit);
extern void  ClrRamBit(int8u *p,int8u bit);
extern void  TogRamBit(int8u *p,int8u bit);
extern int8u  ChkRamBit(int8u *p,int8u bit);

//===== 取发送信息的 CAN 通道
int8u GetTxChnl(int8u fun_id,int8u node_id);

//===== 检测Slave是否在线
int8u ChkSlaveOnLine(int8u Slave);


//static volatile	BYTE CAN1MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN1 FIFO	message	area.
//static volatile	BYTE CAN2MessageFifoArea[(MAX_TX_BUF + MAX_RX_BUF) * 16];	//This is the CAN2 FIFO	message	area.

#endif //_GLB_CAN_API_



