#ifndef _UPDATE_H_
#define _UPDATE_H_

//===============================================
#define EE_FLG_VALID       0x5AA5

//one section (see 协议说明.txt)
#define PACK_PACKAGE_HEAD       0x55AA
#define PACK_CMD_START          0x0101      //申请升级
#define PACK_CMD_DATA           0x0102      //数据
#define PACK_CMD_END            0x0110      //数据结束

#define PACK_CMD_UPDATE_DCU     0x0130      //设置升级的DCU对象

//===============================================
#define UPFW_STATE_NOUPDATE         0       //非升级过程, MMS不处理
#define UPFW_STATE_UPDATING         1       //升级中
#define UPFW_STATE_COPY             2       //升级复制
    
#define UPFW_STATE_SEC_ERR          3       //段数据错误, MMS将再次发送, 如果错误3次, 则认为升级失败, 升级下一个对象
#define UPFW_STATE_SEC_FAIL         4       //段升级失败, MMS 结束本次升级(只是当前的升级对象,  不是全部升级对象), 升级下一个对象
#define UPFW_STATE_UPDATEFAIL       5       //升级失败, MMS 结束本次升级(只是当前的升级对象, 不是全部升级对象), 升级下一个对象
    
#define UPFW_STATE_SEC_OK           6       //段升级 OK, MMS发送下一个数据段
#define UPFW_STATE_UPDATEOK         7       //升级完成, MMS 结束本次升级(只是当前的升级对象, 不是全部升级对象), 升级下一个对象
    
#define UPFW_STATE_OFFLINE          0x80    //DCU 离线时, PEDC  反馈给 MMS 的信息, 实际上不是升级处理程序返回的信息
#define UPFW_STATE_WAITDOOR         0x81    //升级数据接收完成，但是DCU滑动门不是处于关闭状态
#define UPFW_STATE_WAITSTATE        0x82    //收到升级命令，需要DCU工作模式或门状态满足要求，才开始接收升级数据
#define UPFW_STATE_MstSlvSectionErr 0x83    //收到的升级数据段号，和主机登记的段号不符

#define UPFW_STATE_NOSETTING_DCU    0x84    //没有设置升级DCU对象
#define UPFW_STATE_MOTOR_ERR        0x85    //DCU 马达数量不匹配
#define UPFW_STATE_DISABLE_DCU      0x86    //DCU 未使能升级
#define UPFW_STATE_DCU_MCU_ERR      0x87    //DCU 芯片型号不匹配
#define UPFW_STATE_DCU_CONFIGED     0x8F    //待升级DCU收到配置命令

#define UPFW_STATE_OBJOFFLINE       8       //升级对象离线，升级失败
#define UPFW_STATE_ASKFORDATA       9       //请求主机重发段数据（当前需要的段号）

#define UPFW_STATE_UPDATES_VER      0x10    //升级对象的代码已经是最新版本（当请求升级时，被升级对象进行校验得到的结果）
#define UPFW_STATE_CHECK_FAIL       0x11    //校验失败，拒绝升级
#define UPFW_STATE_UNKNOWN_VER      0x12    //不支持的协议版本号
#define UPFW_STATE_UNKNOWN_CMD      0x13    //不支持的命令字
#define UPFW_STATE_UNSUPPORTED      0x20    //不支持升级

typedef enum
{
    MCUCHIP_UNDEF = 0,
    MCUCHIP_PIC30F6010A,
    MCUCHIP_STM32F767,
    MCUCHIP_PIC32MK1024MCM100,
}tEnmMCUChip;

typedef struct
{
    int16u Year;
    int8u Month;
    int8u Day;
    int8u Hour;
    int8u Minute;

    int8u MainVer;
    int8u SubVer;

    int32u  CS;
}tFileInfo;

typedef struct
{
    int16u      wChkSum;                      //must be here!!! checksum(byte addtion) of this struct except wChkSum
    int16u      wValidFlg;                    //must be here!!! EE_FLG_VALID -- this struct is valid
    int16u      wUpdateState;                 //
    int16u      wTotalSections;
    int16u      wCurrentSections;

    tFileInfo Info;
    tFileInfo HexInfo;
}tUpFwState;

typedef struct
{
    int32u  dwStartAdr;                 //byte address
    int16u  wSectionNo;
    int16u  wCodeBytes;                 //byte, 本段需要处理的数据长度
    int8u   *pCode;

    int16u   wUpFwTimeSet;              //ms
    int16u   wUpFwTimeCnt;              //ms
}tUpFwSecMsg;

typedef struct
{
    int32u LedEn;
    int32u *LedPort;
    int32u LedBit;

    int32u UartEn;
    int32u *UartPortSta;
    int32u *UartPortTxReg;
}tUpdate_Tx_Msg;

extern tUpdate_Tx_Msg UpdateTxMsg;

//===============================================
#define Line_Num            4                       //软件信息分为4段，每段32字节
#define Column_Num          32
#define BYTES_INFO          128                     //软件信息, 长度128字节, 必须是偶数
#ifdef _INFO_4_SEC
extern  const int8u strSwDescription[Line_Num][Column_Num];
#else
extern  const int8u strSwDescription[BYTES_INFO];
#endif
extern  int8u DeviceInfo[];

extern  tFileInfo   RxFileInfo;         //MMS升级时传送的文件信息
extern  tUpFwState  UpFwState;          //当前升级状态
extern  tUpFwSecMsg UpFwSecMsg;         //当前接收段的信息

//===============================================
extern int16u ChkDataPack(int8u * pSrc);
extern void   UpFwEraseArea(void);

extern void  UpFw_InitShowMsg  (void);
extern void  UpFw_SetCopyEnable(void);
extern void  UpFw_ChkCopyState (void);

//bios function prototype
extern void     bios_Finished(tUpFwState *pState, tUpdate_Tx_Msg *pTxMsg);
extern void     bios_CopyBakFlash(tUpdate_Tx_Msg *pTxMsg);
extern int8u    bios_WriteFlash(int32u dwSrc, int8u *pCode, int16u cntN);
extern int8u    bios_SaveState(tUpFwState *pState);
extern void     bios_ReadState(tUpFwState *pState);

extern void     bios_NVMProgram(void *Addr, int32u *pCode, int32u Len);
extern int32u   bios_NVMUnlock (int32u nvmop);
extern int32u   bios_NVMWriteUint32 (void* address, int32u data);
extern int32u   bios_NVMWriteRow (void* address, void* data);
extern int32u   bios_NVMErasePage(void* address);
extern int32u   bios_NVMErasePFM(void);
extern void     *bios_memcpy(void *dst , const void *src , int32u n);
extern int32u   bios_memcmp(const void *s1, const void *s2, size_t n);
extern void     bios_SoftReset(void);

#endif //_UPDATE_H_


