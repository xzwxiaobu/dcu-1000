/* 
 * File:   SWLogic.h
 * Author: LLY
 *
 * Created on 2021-1-13
 */

#ifndef SWLOGIC_H
#define	SWLOGIC_H

#include    "SWLogicCfg.h"

//#ifdef	__cplusplus
//extern "C"
//{
//#endif

#ifndef PUBLIC
#define PUBLIC                                  
#endif

/* =============================================================================
 * 数据类型定义
 * 这样定义，不依赖外部的定义
 *  */
typedef unsigned char   U8;
typedef unsigned short  U16;
typedef unsigned int    U32;

#ifndef NULL
#define NULL ((void *)0)
#endif

/* 
 * 回调函数类型
 *  */
typedef     U32 (*cbSysTick)(void);

/* =============================================================================
 *  */

#define LOGIC_PRI_HIGH      9       /* 控制优先级最高       */
#define LOGIC_PRI_LOW       1       /* 控制优先级最低       */
#define LOGIC_PRI_NONE      0       /* 控制优先级未设置     */

#define INPUTCMD_SIZE       4       /* 输入命令结构体长度   */

/* ====================
 * 定义控制逻辑类型
 * 注意：类型值必须从 0 开始且必须保持连续
 *  */
typedef enum
{
    IBP_LOGIC       = 0 ,   /* 0 */
    PSL_LOGIC           ,   /* 1 */
    SIG_LOGIC           ,   /* 2 */
    PSC_LOGIC           ,   /* 3 */
    OCC_LOGIC           ,   /* 4 */
    ATC_LOGIC           ,   /* 5 */
    BAK_LOGIC1          ,   /* 6 预留1 */
    BAK_LOGIC2          ,   /* 7 预留2 */
            
    LOGICTYPE_NUM           /* 必须放在最后用于自动统计数量 */
}tLogicType;

#define NONE_LOGICOUT       0xFF            /* 没有可用的输出控制逻辑 */

typedef enum
{ 
    BITCLR = 0, 
    BITSET 
}BITVAL;

typedef enum 
{ 
    CLS_HIGH_PRI = 0,           /* 关门优先 */
    OPN_HIGH_PRI,               /* 开门优先 */
    MAINTAIN_STS,               /* 状态保持 */
}enmOpnClsMode;

/* ====================
 * 定义控制逻辑命令
 *  */
typedef enum
{
    CMDTYPE_EN          = 0 ,   /* 0 使能       */
    CMDTYPE_OPNSD_EN        ,   /* 1 开边门使能 */
    CMDTYPE_OPN6            ,   /* 2 开6门命令  */
    CMDTYPE_CLS6            ,   /* 3 关6门命令  */
    CMDTYPE_OPN8            ,   /* 4 开8门命令  */
    CMDTYPE_CLS8            ,   /* 5 关8门命令  */
    CMDTYPE_OPNSD1          ,   /* 6 开边门命令1 */
    CMDTYPE_CLSSD1          ,   /* 7 关边门命令1 */
    CMDTYPE_OPNSD2          ,   /* 8 开边门命令2 */
    CMDTYPE_CLSSD2          ,   /* 9 关边门命令2 */
    
    #if 0
    CMDTYPE_REOPN6          ,   /* 10 重开6车门，保留未处理 */
    CMDTYPE_RECLS6          ,   /* 11 重关6车门，保留未处理 */
    CMDTYPE_REOPN8          ,   /* 12 重开8车门，保留未处理 */
    CMDTYPE_RECLS8          ,   /* 13 重关8车门，保留未处理 */
    #endif
            
    CMDTYPE_NUM                 /* 必须放在最后用于自动统计数量 */
}tLogicCmdType;

/* ====================
 * 输入的控制命令结构定义
 *  */

typedef union
{        
    struct
    {
        /* byte 0 : 每个位为1表示对应命令有效*/
        U8  Enable          : 1;    /* 使能信号         */
        U8  Enable_Trg      : 1;    /* 使能信号有变化   */
        U8  opnSideEn       : 1;    /* 开边门使能       */
        U8  opnSideEn_Trg   : 1;    /* 开边门使能有变化 */
        U8                  : 4;    /* 4-7  预留        */
        
        /* byte 1 : 每个位为1表示对应命令有效*/
        U8  opn6Cmd         : 1;    /* 开6门命令        */
        U8  opn6Cmd_Trg     : 1;    /* 开6门命令有变化  */
        U8  cls6Cmd         : 1;    /* 关6门命令        */
        U8  cls6Cmd_Trg     : 1;    /* 关6门命令有变化  */
        U8  opn8Cmd         : 1;    /* 开8门命令        */
        U8  opn8Cmd_Trg     : 1;    /* 开8门命令有变化  */
        U8  cls8Cmd         : 1;    /* 关8门命令        */
        U8  cls8Cmd_Trg     : 1;    /* 关8门命令有变化  */

        /* byte 2 : 每个位为1表示对应命令有效 */
        U8  opnSide1        : 1;    /* 开边门命令1       */
        U8  opnSide1_Trg    : 1;    /* 开边门命令1有变化 */
        U8  clsSide1        : 1;    /* 关边门命令1       */
        U8  clsSide1_Trg    : 1;    /* 关边门命令1有变化 */
        U8  opnSide2        : 1;    /* 开边门命令2       */
        U8  opnSide2_Trg    : 1;    /* 开边门命令2有变化 */
        U8  clsSide2        : 1;    /* 关边门命令2       */
        U8  clsSide2_Trg    : 1;    /* 关边门命令2有变化 */
        
        /* byte 3 : 每个位为1表示对应命令有效 */
        U8                  : 8;    /* 0-7  预留        */
    }bits;
    
    U8 bytes[INPUTCMD_SIZE];
}tInputCmd;

/* ====================
 * 输出的控制命令结构定义
 *  */
#pragma pack(1)
typedef struct
{        
    U8          outType;            /* 输出控制逻辑的类型，不用枚举类型 tLogicType */
    U8          outSeq;             /* 输出控制逻辑的在本类型中的序号，从 0 开始   */
    
    struct
    {
        /* byte 0 : 每个位为1表示对应命令有效*/
        U8  Enable      : 1;        /* 0 使能           */
        U8  IBPPSL_OPN  : 1;        /* 1 IBPPSL 输出有效    */
        U8  DCUOPN6     : 1;        /* 2 输出到DCU的OPN6    */
        U8  DCUCLS6     : 1;        /* 3 输出到DCU的CLS6    */
        U8  DCUOPN8     : 1;        /* 4 输出到DCU的OPN8    */
        U8  DCUCLS8     : 1;        /* 5 输出到DCU的CLS8    */
        U8              : 2;        /* 6-7  预留            */

        /* byte 1 : 每个位为1表示对应命令有效 */
        U8  opnSideEn   : 1;        /* 0 开边门使能         */
        U8  opnSide1    : 1;        /* 1 开边门命令1        */
        U8  clsSide1    : 1;        /* 2 关边门命令1        */
        U8  opnSide2    : 1;        /* 3 开边门命令2        */
        U8  clsSide2    : 1;        /* 4 关边门命令2        */
        U8              : 3;        /* 5-7  预留            */
    }outCmd;
    
}tOutputCmd;

/* ====================
 * 控制逻辑结构定义
 *  */
#pragma pack(4)
typedef struct
{
    U8          logicType;          /* 本控制逻辑的类型 */
    U8          priLev;             /* 控制优先级, 9最高 ~ 1最低，0 表示未设置  */
    U8          timeSeq;            /* 在控制优先级相同时, 先到(数字最小)先获取控制权，0 表示无控制权
                                     * 此属性外部设置 */
    struct
    {
        U8      ibppsl_out  : 1;    /* 1 - 表示 IBP、PSL 操作已使能，需要输出 IBPPSL_OPN   */
        U8      OpnClsMode  : 2;    /* 同时存在开关门命令时的处理模式
                                     * 0-表示关门优先（切开门信号），默认值
                                     * 1-表示开门优先（切关门信号）
                                     * 2-表示维持之前的输出状态
                                     * 3-作关门优先处理  */
        U8      outOpn6Lock : 1;    /* 1 - 自锁输出OPN6，即一直输出直到输入关门   */
        U8      outOpn8Lock : 1;    /* 1 - 自锁输出OPN8，即一直输出直到输入关门   */
        U8      outCls6Lock : 1;    /* 1 - 自锁输出CLS6，即一直输出直到输入开门   */
        U8      outCls8Lock : 1;    /* 1 - 自锁输出CLS8，即一直输出直到输入开门   */
        U8                  : 1;    /* 未使用  */
    }setting;
    
    tInputCmd   InCmd;              /* 输入命令 */
    
}tCtrlLogic;

/* =============================================================================
 * 
 *                          提供的函数接口
 * 
 * =============================================================================
 */
/* =============================================================================
 * 获取软件控制逻辑库软件的版本号
 *  返回 十六进制值表示版本号
 *      如 版本 1 返回 0x01
 *  */
PUBLIC U8 getCtrlLogicLibVersion(void);

/* =============================================================================
 * 获取软件控制逻辑的日期
 *  返回 32位长度的日期数据，数据类型是16进制，格式为 YYYYMMDD
 *      如 2021.1.20 返回信息为 0x20210120L
 *  */
PUBLIC U32 getCtrlLogicLibDate(void);

/* =============================================================================
 * 获取当前的输出控制逻辑序号
 *  返回
 *      当前的输出控制逻辑序号(从 0 ~(CTRL_LOGIC_NUM-1)，不区分控制类别)
 *  */
PUBLIC U8 getLogicOutNo(void);


/* 获取输出命令
 *  输入
 *      pOutCmd     用于存储输出命令的存储器指针
 *  返回 
 *      0           表示成功
 *      -1          表示输入参数错误
 */
PUBLIC int getLogicOutputCmd(tOutputCmd * pOutCmd);


/* =============================================================================
 * 应用层必须调用此函数设置回调函数，软件逻辑获取系统定时器周期值(ms)
 *  输入
 *      fun     是回调函数的指针
 *  举例
 *      setLogicSysTickFunct((cbSysTick)getSysTick);
 *  */
PUBLIC int setLogicSysTickFunct(cbSysTick fun);


/* 
 * =============================================================================
 * 重置全部软件控制逻辑
 *      - 控制逻辑的优先级全部被初始化为无设置
 *      - 输出同时被重置
 * 输入
 *      无
 * 返回
 *       0          设置成功
 *  */
PUBLIC int resetAllCtrlLogic(void);

/* =============================================================================
 * 获取指定类型及序号控制逻辑控制信息
 *      例：获取PSL2 控制逻辑的信息
 *      readLogicInfo( PSL_LOGIC, 1, readLog);
 * 
 * 输入
 *      type        控制逻辑类型
 *      num         该类型中控制逻辑的序号, 从 0 开始
 *      readLog     存储逻辑控制信息数据的指针
 * 返回
 *      -1          表示输入参数错误或不支持该控制类型
 *      >0          控制逻辑的总数
 *  */
PUBLIC int readLogicInfo(tLogicType type, U8 seq, tCtrlLogic * readLog);


/* =============================================================================
 * 获取指定序号的控制逻辑控制信息
 * 
 * 输入
 *      numAll      控制逻辑的序号, 从 0 ~ (CTRL_LOGIC_NUM-1)
 *      readLog     存储逻辑控制信息数据的指针
 * 返回
 *      -1          表示输入参数错误
 *       0          控制逻辑的指针
 *  */
PUBLIC int readLogicInfoByNum(U8 numAll, tCtrlLogic * readLog);


/* =============================================================================
 * 初始化软件控制逻辑
 * 
 *  输入
 *      type        控制逻辑类型
 *      seq         该类型中控制逻辑的序号, , 从 0 ~ ( 该控制逻辑支持的最大数量 - 1)
 *                  若 seq == 0xff，则该逻辑类型的控制逻辑，将被统一初始化
 *      setLogic    设置内容的指针
 *  返回
 *       0          设置成功
 *      -1          设置失败：类型参数错误
 *      -2          设置失败：不支持该类型
 *      -3          设置失败：控制逻辑序号错误
 *      -4          设置失败：找不到控制逻辑
 *      -5          设置失败：输入参数错误
 *  */
PUBLIC int initCtrlLogic(tLogicType type, U8 seq, tCtrlLogic *setLogic);


/* =============================================================================
 * 设置指定控制逻辑的优先级
 *  注意：被设置优先级的控制逻辑，除优先级信息外其它全部信息被重置为 0
 * 
 *  输入
 *      type        控制逻辑类型
 *      seq         该类型中控制逻辑的序号, , 从 0 ~ ( 该控制逻辑支持的最大数量 - 1)
 *                  若 seq == 0xff，则该逻辑类型的控制逻辑，将被设定为统一的优先级
 *      pri         指定的优先级，9最高 ~ 1最低
 *  返回
 *       0          设置成功
 *      -1          设置失败：类型参数错误
 *      -2          设置失败：不支持该类型
 *      -3          设置失败：控制逻辑序号错误
 *      -4          设置失败：找不到控制逻辑
 *  */
PUBLIC int setCtrlLogicPri(tLogicType type, U8 seq, U8 pri);


/* =============================================================================
 * 
 * 设置指定控制逻辑的设置项
 * 
 *  输入
 *      type        控制逻辑类型
 *      seq         该类型中控制逻辑的序号, , 从 0 ~ ( 该控制逻辑支持的最大数量 - 1)
 *                  若 seq == 0xff，则该逻辑类型的控制逻辑，将被统一设定
 *      ipbpsl      1- 本逻辑操作使能时需要同时输出 IBPPSL_OPN 信号
 *                  对于 IBP_LOGIC或PSL_LOGIC 类型可以不用设置
 *      opnMode     开关门模式设置
 *  返回
 *       0          设置成功
 *      -1          设置失败：类型参数错误
 *      -2          设置失败：不支持该类型
 *      -3          设置失败：控制逻辑序号错误
 *      -4          设置失败：找不到控制逻辑
 *  */
PUBLIC int setCtrlLogicSetting(tLogicType type, U8 seq, BITVAL ipbpsl, enmOpnClsMode opnMode);


/* =============================================================================
 * 设置指定控制逻辑的输入命令
 * 
 *  输入
 *      type        控制逻辑类型
 *      seq         该类型中控制逻辑的序号, 从 0 ~ ( 该控制逻辑支持的最大数量 - 1) 
 *      cmdType     输入命令类型
 *      bitVal      输入命令的值 ( 0 or 1 )
 *  返回
 *       0          设置成功
 *      -1          设置失败：类型参数错误
 *      -2          设置失败：不支持该类型
 *      -3          设置失败：控制逻辑序号错误
 *      -4          设置失败：命令类型错误
 *      -5          设置失败：控制逻辑未初始化（优先级）
 *  */
PUBLIC int setCtrlLogicInpCmd(tLogicType logicType, U8 seq, tLogicCmdType cmdType, BITVAL bitVal);


/* =============================================================================
 * 全类型序号分成逻辑类型及在该类型中的序号
 * 序号都是从 0 开始
 *  输入
 *      allSeq      全类型序号
 *      type        全类型序号所属的单类型
 *      seq         全类型序号在所属的单类型中序号
 * 返回
 *      0           OK
 *      -1          输入参数错误
 * */
PUBLIC int departAllSeqToTypeAndSeq(U8 allSeq, U8 *type, U8 *seq);

/* =============================================================================
 * 已知控制逻辑的类型及在该类型中的序号，获得该控制逻辑在全类型中的序号
 * 序号都是从 0 开始
 *  输入
 *      type        指定逻辑类型
 *      seq         在指定逻辑类型中的序号
 * 返回
 *      >=0         全类型序号
 *      -1          输入参数错误
 * */
PUBLIC int unionTypeAndSeqToAllSeq(tLogicType type, U8 seq);


/* =============================================================================
 * 软件控制逻辑的处理主函数，应用层循环调用
 *  输入
 *      无
 *  输出
 *      无
 *  */
PUBLIC void softLogicProc(void);


/* =============================================================================
 *  */
//#ifdef	__cplusplus
//}
//#endif

#endif	/* SWLOGIC_H */

