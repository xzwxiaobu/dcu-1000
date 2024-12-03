/*=============================================================================
* Project:      WH2-LINE PEDC
* PEDC MCU:     PIC32MX795F512H + PIC18F23K22
* Fcrystal:     4M
* PLL:          X20
* FCY:          SYSTEM_FREQ = 80M
*
* Version:      0.1
* Author:       lly
* Date:         2020-7-29
* =============================================
* 文     件 :   Update.C
*  在线升级功能模块底层函数
* V0.1:
*   增加定义 UPFW_STATE_OBJOFFLINE，表示升级对象离线
*   strSwDescription 支持分4段方式或整个字符串方式，通过项目预定义 _INFO_4_SEC 识别
*   
* =============================================================================*/
#include "_g_canbus.h"

#include "glbUpdate.h"
#include "Update.h"

#include "plib.h"

//#include "UpEEPROM.h"
//#include "UpRTSP.h"

#define UPDATE_UNCONDITIONAL        //无条件升级

//main.h ?
#define GetSystemClock()            (80000000ul)
#define GetPeripheralClock()    (GetSystemClock()/(1 << OSCCONbits.PBDIV))
#define GetInstructionClock()   (GetSystemClock())

//默认
#define DFT_YEAR          2022
#define DFT_MONTH         4
#define DFT_DAY           7
#define DFT_HOUR          11
#define DFT_MINUTE        20

#define DFT_MAIN_VER      0
#define DFT_SUB_VER       0

//=========================================================
//var define
//=========================================================
    int8u DeviceInfo[BYTES_INFO+2];

    tUpdate_Tx_Msg UpdateTxMsg;

//---------------------------------------------------------------------------
//记录软件描述信息, 128 BYTES, MS 的升级软件需要读取此处的内容来校验
//其中 96 字节是描述, 后面流出32字节可以传输其它信息
#ifdef _INFO_4_SEC
const int8u __attribute__((section (".SEG_SW_INFO"), space (prog), aligned(4))) strSwDescription[Line_Num][Column_Num] =
{
    {PROJECT_NAME},{PROJECT_TIME},{PROJECT_VER},{PROJECT_OTHER}
};
#else
const int8u __attribute__((section (".SEG_SW_INFO"), space (prog), aligned(4))) strSwDescription[BYTES_INFO] =
{
    PROJECT_INFOS
};
#endif

//需要固定 RAM 地址的寄存器，避免代码不通用 ???
tFileInfo   __attribute__ ((section (".SEG_UPGRADE_RAM"))) __attribute__((aligned(4))) RxFileInfo;          //MMS升级时传送的文件信息
tUpFwState  __attribute__ ((section (".SEG_UPGRADE_RAM"))) __attribute__((aligned(4))) UpFwState;           //当前升级状态
tUpFwSecMsg __attribute__ ((section (".SEG_UPGRADE_RAM"))) __attribute__((aligned(4))) UpFwSecMsg;          //当前接收段的信息


/*
//必须固定地址，否则编译完成后的代码不具备通用性
int16u      __attribute__((address(0x0850))) TmpCode[WORDS_PER_ROW+1]; //指令缓存，用于读取32条指令, 对齐到双字
tFileInfo   __attribute__((address(0x08d2))) RxFileInfo;
tUpFwState  __attribute__((address(0x08de))) UpFwState;                //当前升级状态
tUpFwSecMsg __attribute__((address(0x0900))) UpFwSecMsg;               //当前接收段的信息
*/

void  UpFw_SetCopyEnable(void);
void  UpFw_ChkCopyState (void);

//=========================================================
//funciton define
//=========================================================
static void     UpFwStart(void);
static int8u    UpFwWriteCode(void);
static int32u   CalcCodeChkSumSelf(int32u * addr, int32u len);

const int8u PwrOnStr[]=
{
  "Power on test!",
};

//****************************************************************************************************************
//                                                                                                               *
//                                        升级程序中的应用程序(不固化）                                          *
//                                                                                                               *
//****************************************************************************************************************
// Function:    UpFw_ChkCopyState()
// Description: 复位以后检查是否有升级 Copy 失败的情况, 如果有就重新 Copy
// Parameters:
// Return:
// State:
void UpFw_ChkCopyState()
{
    int8u tst[32];
    int i,j;

    bios_ReadState(&UpFwState);
    if(UpFwState.wUpdateState == UPFW_STATE_COPY)
    {
        UpFw_InitShowMsg();
        UpFw_SetCopyEnable();
        bios_Finished(&UpFwState, &UpdateTxMsg);
    }
}

//=========================================================
//全部代码下载完毕, 设置允许复制标志
void UpFw_InitShowMsg(void)
{
    #if (ENABLE_UART_MSG == 1)
    UARTEnable(UPGRADE_UART, UART_DISABLE_FLAGS(UART_RX | UART_TX));     //关闭UART，清错误标志
    INTClearFlag(INT_SOURCE_UART_TX(UPGRADE_UART));                      //clear u1art err/rx/tx interrupt flag
    INTClearFlag(INT_SOURCE_UART_RX(UPGRADE_UART));
    INTClearFlag(INT_SOURCE_UART_ERROR(UPGRADE_UART));

    INTSetVectorPriority(INT_VECTOR_UART(UPGRADE_UART), 3);                      //中断优先级
    INTSetVectorSubPriority(INT_VECTOR_UART(UPGRADE_UART), INT_SUB_PRIORITY_LEVEL_0);

    UARTConfigure(UPGRADE_UART, UART_ENABLE_PINS_TX_RX_ONLY);
    UARTSetFifoMode(UPGRADE_UART, UART_INTERRUPT_ON_TX_NOT_FULL | UART_INTERRUPT_ON_RX_NOT_EMPTY);
    UARTSetLineControl(UPGRADE_UART, UART_DATA_SIZE_8_BITS | UART_PARITY_NONE | UART_STOP_BITS_1);
    UARTSetDataRate(UPGRADE_UART, GetPeripheralClock(), UPGRADE_UART_BR);
    UARTEnable(UPGRADE_UART, UART_ENABLE_FLAGS(UART_PERIPHERAL | UART_RX | UART_TX));

    INTEnable(INT_SOURCE_UART_RX(UPGRADE_UART), INT_DISABLED);  //INT_ENABLED = enable
    INTEnable(INT_SOURCE_UART_TX(UPGRADE_UART), INT_DISABLED);  //INT_ENABLED = enable
    /*
    bios_memcpy(tst,PwrOnStr,sizeof(PwrOnStr));
    for(i=0; i<sizeof(PwrOnStr); i++)
    {
        while(!UARTTransmitterIsReady(UPGRADE_UART));
        UARTSendDataByte(UPGRADE_UART, tst[i]);
    }
    */
    #endif

    UpdateTxMsg.LedEn   = ENABLE_LED_MSG;
    UpdateTxMsg.LedPort = (int32u*)(&LED_PORT);
    UpdateTxMsg.LedBit  = LED_BIT;
    UpdateTxMsg.UartEn        = ENABLE_UART_MSG;
    UpdateTxMsg.UartPortSta   = (int32u*)(&UPGRADE_UART_TX_STA);
    UpdateTxMsg.UartPortTxReg = (int32u*)(&UPGRADE_UART_TX_REG);

}

//=========================================================
//全部代码下载完毕, 设置允许复制标志
void UpFw_SetCopyEnable(void)
{
    UpFwState.wChkSum       = 0;
    UpFwState.wValidFlg     = EE_FLG_VALID;
    UpFwState.wUpdateState  = UPFW_STATE_COPY;
    UpFwState.wTotalSections= UpFwState.wCurrentSections;
    UpFwState.HexInfo       = RxFileInfo;

    //INT_DIS();
    bios_SaveState(&UpFwState);
    //INT_EN();
}

//---------------------------------------------------------------------------
// Function:    UpFwProcessData()
// Description: 解析数据包, 如果是正确的, 就写入 FLASH
// Parameters:  参数 pSrc 数据包指针,注意指向的地址对齐到 int32u !!!!
// Return:      UPFW_STATE_SEC_ERR / ....
// State:
//---------------------------------------------------------------------------
/* 数据包的格式
 (1)申请升级:
    PACK_PACKAGE_HEAD(2) + PACK_CMD_START (2) + 本次数据总长度(2 : n+24 ) + 文件时间(6) +
    版本(2,保留) + HEX升级文件校验和(4) + 数据写入地址(4) + 数据(n) + 校验和 (2)
 (2)数据: 此段数据的结构
    PACK_PACKAGE_HEAD(2) + PACK_CMD_DATA (2) + 本次数据总长度(2 : n+14 ) + 本段序号(2) +
    数据写入地址(4) + 数据(n) + 校验和 (2)
    数据是升级程序的指令,一条指令占用4字节
 (3)数据结束:
    PACK_PACKAGE_HEAD(2) + PACK_CMD_END (2) + 本次数据总长度(2 : 10) + 段总数(2) + 校验和 (2)
 * 2022/4/1
 (4)设置广播升级:
    PACK_PACKAGE_HEAD(2) + PACK_CMD_GBSJ_DCU (2) + 本次数据总长度(2 : 26) + 
    待升级DCU标志(8字节，每个位对应一个DCU) + 
    DCU马达数量(8字节，每个位对应一个DCU)，位值1表示双马达（半高门）0表示单马达（全高门）+
    校验和 (2)
*/
int8u UpFwProcessData(int8u * pSrc)
{
    int8u * p1, * p2;
    int16u wTotLen;
    int16u wTmp;
    int8u Return;
    static int8u u8EraseFlag=1;

    wTotLen = ChkDataPack(pSrc);
    if(wTotLen < 8)
        return UPFW_STATE_SEC_ERR;

    switch(*((int16u*)pSrc + 1))                       //命令
    {
        case PACK_CMD_UPDATE_DCU:
            Return = UPFW_STATE_UNKNOWN_CMD;
            break;
        
        //PACK_PACKAGE_HEAD(2) + PACK_CMD_START (2) + 本次数据总长度(2 : 20 ) +
        //文件时间(6) + 版本(2,保留) + HEX升级文件校验和(4) + 校验和 (2)
        case PACK_CMD_START:
            bios_ReadState(&UpFwState);
            //if(UpFwState.wUpdateState != UPFW_STATE_NOUPDATE)   //是否已经处于升级过程
            //  return UPFW_STATE_SEC_ERR;              //如果不判断, 则重复收到升级命令将重新启动升级

            p1 = (int8u*)&RxFileInfo;
            p2 = pSrc + 6;
            for(wTmp=0; wTmp<sizeof(tFileInfo); wTmp++) //取文件时间(6) + 版本(2,保留) + HEX升级文件校验和(4)
            {
                *p1++ = *p2++;
            }

        #ifndef UPDATE_UNCONDITIONAL
            p1 = (int8u*)&RxFileInfo;
            p2 = (int8u*)&UpFwState.HexInfo;
            for(wTmp=sizeof(tFileInfo); wTmp>0; wTmp--) //比较文件时间(6) + 版本(2,保留) + HEX升级文件校验和(4)
            {
                if(*p1 != *p2)
                    break;
            }
            if(wTmp == 0)
            {
                Return = UPFW_STATE_UPDATEOK;           //完全相同，不需要升级
            }
            else
        #endif
            {                                           //需要升级
                if(u8EraseFlag != 0)
                {
                    UpFwEraseArea();
                    u8EraseFlag = 0;
                }
                UpFwStart();
                Return = UPFW_STATE_SEC_OK;
            }
            break;

        //PACK_PACKAGE_HEAD(2) + PACK_CMD_DATA (2) + 本次数据总长度(2 : n+14 ) + 本段序号（2） +
        //数据写入地址(4) + 数据(n) + 校验和 (2)
        case PACK_CMD_DATA:
            wTmp = *((int16u*)(pSrc + 6));
            if(( UpFwState.wCurrentSections == wTmp) || ( (UpFwState.wCurrentSections - 1) == wTmp) ) //是否需要的段(序号) ;(UpFwState.wCurrentSections - 1) == wTmp) 主要是 PEDC这边升级成功 -->UpFwState.wCurrentSections ++ ,但返回 UPFW_STATE_SEC_OK -->MMS 失败 此时 MMS重新发 上一段数据
            {
                if(UpFwState.wCurrentSections == wTmp)
                    UpFwState.wCurrentSections++;

                p1 = pSrc + 8;
                UpFwSecMsg.dwStartAdr  = (int32u)(*(p1++));
                UpFwSecMsg.dwStartAdr |= (int32u)(*(p1++))<<8;
                UpFwSecMsg.dwStartAdr |= (int32u)(*(p1++))<<16;
                UpFwSecMsg.dwStartAdr |= (int32u)(*(p1++))<<24;
                UpFwSecMsg.dwStartAdr |= 0x80000000L;               //KSEG0 虚拟地址 = 物理地址 | 0x80000000

                UpFwSecMsg.wCodeBytes  = *((int16u*)(pSrc + 4))-14;
                UpFwSecMsg.pCode       = (int8u*)(pSrc + 12);

                Return = UpFwWriteCode();
                u8EraseFlag = 1;

                if(Return == 1)
                {
                    Return = UPFW_STATE_SEC_FAIL;
                }
                else
                {
                    Return = UPFW_STATE_SEC_OK;
                }
            }
            else
            {
                Return = UPFW_STATE_SEC_ERR;
            }
            break;

        //PACK_PACKAGE_HEAD(2) + PACK_CMD_END (2) + 本次数据总长度(2 : 10) + 段总数(2) + 校验和 (2)
        case PACK_CMD_END:
            wTmp = *((int16u*)(pSrc + 6));                 //段总数（2）

            if(UpFwState.wCurrentSections == (wTmp + 1))
            { // 升级成功

                UpFwState.wCurrentSections --;

                UpFw_SetCopyEnable();
                Return = UPFW_STATE_UPDATEOK;
            }
            else
            {
                Return = UPFW_STATE_UPDATEFAIL;
            }
            break;

        default:
            Return = UPFW_STATE_SEC_ERR;
            break;
    }

    return Return;
}

//---------------------------------------------------------------------------
// Function:    UpFwStart()
// Description: 升级初始化
// Parameters:
// Return:
// State:
//---------------------------------------------------------------------------
static void UpFwStart(void)
{
    UpFwState.wChkSum      = 0;
    UpFwState.wValidFlg    = EE_FLG_VALID;
    UpFwState.wUpdateState = UPFW_STATE_UPDATING;
    UpFwState.wTotalSections = 0;
    UpFwState.wCurrentSections = 1;

    bios_SaveState(&UpFwState);

    UpFwSecMsg.dwStartAdr = 0;                //__RESET_BASE
    UpFwSecMsg.wCodeBytes = 0;
    UpFwSecMsg.wSectionNo = 0;
}


//---------------------------------------------------------------------------
// Function:    ChkDataPack()
// Description: 检查数据包的信息是否有效：检查头信息 / 数据包Checksum
// Parameters:  pSrc 数据包
// Return:      返回数据包的长度, 0 = 头错误, 1 = 长度错误, 2 = CS 错误
// State:
//---------------------------------------------------------------------------
int16u ChkDataPack(int8u * pSrc)
{
    int16u *pW;
    int8u *p1;
    int16u wCS, wTmp, wCnt, wLen;

    pW = (int16u*)pSrc;                     //检查头信息
    if(*pW != PACK_PACKAGE_HEAD)
        return 0;

    pW    = (int16u*)pSrc + 2;              //本次数据总长度
    wLen  = (int16u)*pW;
    if(wLen < 8)
        return 1;

    wCnt = wLen-2;
    p1   = pSrc + wCnt;                     //校验和, 此处不用  int16u 指针, 避免长度是奇数时出错（实际是偶数）
    wCS  = (int16u)*p1++;
    wCS |= (int16u)*p1 << 8;

    p1    = pSrc;
    wTmp  = 0;
    while(wCnt>0)
    {
        wTmp += (int16u)*p1;
        p1++;
        wCnt--;
    }

    if(wTmp != wCS)                         //校验和是否正确
        return 2;

    return wLen;
}

//---------------------------------------------------------------------------
// Function:    CalcCodeChkSumSelf()
// Description: 计算代码的 checkSum(Byte加法), 地址和长度参见: xx_procdefs.ld
// Parameters:  addr -- flash地址, len -- 长度(Bytes)
// Return:
// State:      1. 80MHz 时，计算时间大约 0.115ms/KB(空指令也参与计算)
//             2. 为了方便检查, 计算的时候跳开了空指令(0xFFFFFFFF)
//             3. 不能读取4个配置字(0x9FC02FF0~0x9FC02FFF), HEX 文件中没有配置字信息
//             4. 待测试
//---------------------------------------------------------------------------
static int32u CalcCodeChkSumSelf(int32u * addr, int32u len)
{
    int32u  cs,tmp;

    cs   = 0;
    while(len)
    {
        tmp = *addr++;
        if(tmp != 0xFFFFFFFFL)
        {
            cs += (int8u)tmp;
            cs += (int8u)(tmp>>8);
            cs += (int8u)(tmp>>16);
            cs += (int8u)(tmp>>24);
        }

        len -= 4;
    }
    return cs;
}

//---------------------------------------------------------------------------
// Function:    UpFwWriteCode()
// Description: write code to flash
// Parameters:  saved in UpFwSecMsg
// Return:      return : 0 = OK, 1 = Error, 2 = skip to write
// State:
//---------------------------------------------------------------------------
//
static int8u UpFwWriteCode(void)
{
    int32u  *p,*p2;
    int32u  cnt1,cnt2,cnt3;
    int32u  dwWrAdr;
    int32u  TmpCode[INST_PER_ROW];

    cnt1 = UpFwSecMsg.wCodeBytes;          //需要写入字节数
    if(cnt1 < 4)
        return 1;

    dwWrAdr = UpFwSecMsg.dwStartAdr;

    if((dwWrAdr >= MAIN_UPDATE_ADDR) && ((dwWrAdr+cnt1 - 1) < (MAIN_UPDATE_ADDR+MAIN_UPDATE_SIZE)))
    {//程序区, 映射到下载区地址
        dwWrAdr += DOWN_CODE_ADDR - MAIN_UPDATE_ADDR + FLASH_BOOT_SIZE;
    }

    else if((dwWrAdr >= FLASH_BOOT_ADDR) && ((dwWrAdr+cnt1 - 1) < (FLASH_BOOT_ADDR + FLASH_BOOT_SIZE)))
    {//BOOT区, 映射到下载区地址
        dwWrAdr += DOWN_CODE_ADDR - FLASH_BOOT_ADDR;
    }

    else
    {//不是所需
        return 2;
    }

    cnt2 = 0;
    _RewriteCode:
        bios_NVMProgram((void*)dwWrAdr,(int32*)(UpFwSecMsg.pCode),UpFwSecMsg.wCodeBytes);

        if(bios_memcmp((void*)dwWrAdr, (void*)(UpFwSecMsg.pCode), cnt1) != 0)
        {
            cnt2 ++;
            if(cnt2 < 3)
                goto _RewriteCode;
            else
                return 1;
        }

    return 0;

/*
    cnt1    >= 2;                                       //指令数
    p        = (int32u*)(UpFwSecMsg.pCode);
    cnt2     = (int32u)((dwWrAdr) & 0x7FL);             //对齐 128  条指令的整数倍地址后剩余字数
    dwWrAdr &= (int32u)(~0x7FL);

    if((cnt2 > 0))
    {//不是 128 条指令的整数倍地址
        bios_memcpy((void*)TmpCode,(void*)dwWrAdr, INST_PER_ROW*4);
        if(cnt1 + cnt2 >= INST_PER_ROW)
        {
            cnt3 = (INST_PER_ROW - cnt2);
        }
        else
        {
            cnt3 = cnt1;
        }

        p2 = (int32u*)(&TmpCode) + cnt2;
        for(cnt2=0; cnt2<cnt3; cnt2++)
        {
            *p2++ = *p++;
        }

        if(Bios_WriteFlash(dwWrAdr, (int8u*)(&TmpCode), 3) == FALSE)
        {
            return 1;
        }

        cnt1    -= cnt3;
        dwWrAdr += INST_PER_ROW;
    }

    while(cnt1 >= INST_PER_ROW)
    {
        if(Bios_WriteFlash(dwWrAdr, (int8u*)p,  3) == FALSE)
        {
            return 1;
        }

        dwWrAdr += INST_PER_ROW;
        cnt1    -= INST_PER_ROW;
        p       += INST_PER_ROW;
    }

    //是否有不足 128 条指令的数据未写入
    //将原来的指令读出后填充满128条指令
    if((cnt1 > 0))
    {
        bios_memcpy((void*)TmpCode,(void*)dwWrAdr, INST_PER_ROW*4);

        p2 = (int32u*)(&TmpCode);
        for(cnt2=0; cnt2<cnt1; cnt2++)
            *p2++ = *p++;

        if(Bios_WriteFlash(dwWrAdr, (int8u*)(&TmpCode), 3) == FALSE)
        {
            return 1;
        }
    }

    return 0;
*/
}

//=========================================================
//启动升级时：
//擦除Flash : 252KB, 最小耗时约 20ms(page) * 252K/4K = 1260ms
void UpFwEraseArea(void)
{
    int32u dwWrAdr, cnt;

    dwWrAdr = DOWN_CODE_ADDR;

    //INT_DIS();
    for(cnt=0; cnt<(DOWN_CODE_SIZE/FLASH_PAGE_SIZE); cnt++)
    {
        bios_NVMErasePage((void*)dwWrAdr);
        dwWrAdr += FLASH_PAGE_SIZE;
    }
    //INT_EN();
}

//=========================================================
void GetAllChkSum(void)
{
    int32u  u32CS;
    int8u   flag;

    bios_ReadState(&UpFwState);
    flag = 0;

    if( ( UpFwState.Info.Year   < 2010)||
         ((UpFwState.Info.Month < 1)   || (UpFwState.Info.Month > 12)) ||
         ((UpFwState.Info.Day   < 1)   || (UpFwState.Info.Month > 31)) ||
         ((UpFwState.Info.Hour  > 23)) ||
         ((UpFwState.Info.Minute> 59)))
    {
        UpFwState.Info.Year    = DFT_YEAR;
        UpFwState.Info.Month   = DFT_MONTH;
        UpFwState.Info.Day     = DFT_DAY;
        UpFwState.Info.Hour    = DFT_HOUR;
        UpFwState.Info.Minute  = DFT_MINUTE;

        UpFwState.Info.MainVer = DFT_MAIN_VER;
        UpFwState.Info.SubVer  = DFT_SUB_VER;

        UpFwState.HexInfo.Year    = 0;
        UpFwState.HexInfo.Month   = 0;
        UpFwState.HexInfo.Day     = 0;
        UpFwState.HexInfo.Hour    = 0;
        UpFwState.HexInfo.Minute  = 0;

        UpFwState.HexInfo.MainVer = 0;
        UpFwState.HexInfo.SubVer  = 0;

        flag = 1;
    }

    //boot flash(except config words)
    u32CS = CalcCodeChkSumSelf((int32u*)FLASH_BOOT_ADDR, FLASH_BOOT_SIZE - 4*4);

    //main program flash
    u32CS += CalcCodeChkSumSelf((int32u*)MAIN_UPDATE_ADDR, MAIN_UPDATE_SIZE);

    //upgrade flash code
    u32CS += CalcCodeChkSumSelf((int32u*)UPFW_CODE_ADDR, UPFW_CODE_SIZE);

    //upgrade flash data（不能计算此处CHECHKSUM，否则与HEX文件对不上）
    //u32CS += CalcCodeChkSumSelf((int32u*)UPFW_FLASH_DATA_ADDR, UPFW_FLASH_DATA_SIZE);

    //to avoid write frequently
    if(u32CS != UpFwState.Info.CS)
    {
        UpFwState.Info.CS = u32CS;
        flag = 1;
    }

    if(flag)
    {
        bios_SaveState(&UpFwState);
    }
}


#if 0
/* 20201130 TOBE done
 * 尝试改变计算校验和的方法得到与 IPE 相同的结果
 * 测试不成功，待以后处理
 *  */
/* =========================================================
 * 
 * 
 * see PIC32_ Pgm _Spec_DS60001145X.pdf, chapter 18.0 (page 50)
 * 
 * The checksum is calculated as the 32-bit summation of
 * all bytes (8-bit quantities) in program Flash, Boot Flash(except device Configuration Words),
 * the Device ID register with applicable mask,
 * and the device Configuration Words with applicable masks. 
 * Then the 2’s complement of the summation is calculated.
 * This final 32-bit number is presented as the checksum.
 *   意思取反加一, 举例，加法结果 0x0827C469，
 *   最终checksum = (0x0827C469 ^ 0xffffffff) + 1 = 0xF7D83B97
 * 
 *   DEVICE CONFIGURATION REGISTER MASK VALUES for PIC32MX795, PIC32MKXXXXGPD/GPE/MCFXXX ...
 *              DEVCFG0    DEVCFG1    DEVCFG2    DEVCFG3    DEVID
 *   mask       0x110FF00F 0x009FF7A7 0x00078777 0xC707FFFF 0x000FF000
 *   Address    0x1FC02FFC 0x1FC02FF8 0x1FC02FF4 0x1FC02FF0
 *   value                                                  0x0430E053 for PIC32MX795F512H
 * 
 * X IPE V5.20 clear all memory, checksum = F7D837E3
 * */
#define MASK_DEVCFG0            0x110FF00FL
#define MASK_DEVCFG1            0x009FF7A7L
#define MASK_DEVCFG2            0x00078777L
#define MASK_DEVCFG3            0xC707FFFFL

#define MASK_DEVID              0x000FF000L
#define DEVID_PIC32MX795F512H   0x0430E053L

typedef union
{
    int32u W;
    int8u  B[4];
}uniDWORD;

static int32u CalcCodeChkSumSelf_new(int32u * addr, int32u len)
{
    int32u  cs;
    uniDWORD tmp;

    cs   = 0;
    while(len)
    {
        tmp.W = *addr++;
        //if(tmp.W != 0xFFFFFFFFL)
        {
            cs += tmp.B[0];
            cs += tmp.B[1];
            cs += tmp.B[2];
            cs += tmp.B[3];
        }

        len -= 4;
    }
    return cs;
}

int32u  u32HexCS[2];
void GetAllChkSum_new(void)
{
    int32u  u32CS;
    int8u   flag;

    bios_ReadState(&UpFwState);
    flag = 0;

    if( ( UpFwState.Info.Year   < 2010)||
         ((UpFwState.Info.Month < 1)   || (UpFwState.Info.Month > 12)) ||
         ((UpFwState.Info.Day   < 1)   || (UpFwState.Info.Month > 31)) ||
         ((UpFwState.Info.Hour  > 23)) ||
         ((UpFwState.Info.Minute> 59)))
    {
        UpFwState.Info.Year    = 2020;
        UpFwState.Info.Month   = 11;
        UpFwState.Info.Day     = 30;
        UpFwState.Info.Hour    = 15;
        UpFwState.Info.Minute  = 41;

        UpFwState.Info.MainVer = 0;
        UpFwState.Info.SubVer  = 1;

        UpFwState.HexInfo.Year    = 0;
        UpFwState.HexInfo.Month   = 0;
        UpFwState.HexInfo.Day     = 0;
        UpFwState.HexInfo.Hour    = 0;
        UpFwState.HexInfo.Minute  = 0;

        UpFwState.HexInfo.MainVer = 0;
        UpFwState.HexInfo.SubVer  = 0;

        flag = 1;
    }

    /* 计算校验和
     *  */
    int32u csCfg = 0;   /* 配置字和设备ID       */
    int32u csPrg = 0;   /* 主程序代码           */
    int32u csBlt = 0;   /* BOOTFlash程序代码    */
    int32u csUpd = 0;   /* 升级程序代码         */
    uniDWORD u32Data;
    
    /* 1. 
     * 4个配置字的校验和
     * 注意需要与屏蔽字做与运算
     *  */
    u32Data.W = (*((int32u*)config_BFC02FF0)) & MASK_DEVCFG3;
    csCfg += u32Data.B[0];
    csCfg += u32Data.B[1];
    csCfg += u32Data.B[2];
    csCfg += u32Data.B[3];
    
    u32Data.W = (*((int32u*)config_BFC02FF4)) & MASK_DEVCFG2;
    csCfg += u32Data.B[0];
    csCfg += u32Data.B[1];
    csCfg += u32Data.B[2];
    csCfg += u32Data.B[3];
    
    u32Data.W = (*((int32u*)config_BFC02FF8)) & MASK_DEVCFG1;
    csCfg += u32Data.B[0];
    csCfg += u32Data.B[1];
    csCfg += u32Data.B[2];
    csCfg += u32Data.B[3];
    
    u32Data.W = (*((int32u*)config_BFC02FFC)) & MASK_DEVCFG0;
    csCfg += u32Data.B[0];
    csCfg += u32Data.B[1];
    csCfg += u32Data.B[2];
    csCfg += u32Data.B[3];
    
    /* 2. 
     * DEVID 的校验和 */
    u32Data.W = DEVID_PIC32MX795F512H & MASK_DEVID;
    csCfg += u32Data.B[0];
    csCfg += u32Data.B[1];
    csCfg += u32Data.B[2];
    csCfg += u32Data.B[3];
    
    /* 3. 
     * Program Flash 的校验和 */
    csPrg = CalcCodeChkSumSelf_new((int32u*)MAIN_UPDATE_ADDR, MAIN_UPDATE_SIZE);
    
    /* 4. 
     * Bootloader Flash 的校验和 (except config words) */
    csBlt = CalcCodeChkSumSelf_new((int32u*)FLASH_BOOT_ADDR, FLASH_BOOT_SIZE - 4*4);
    
    /* 5. 
     * 升级程序的校验和 */
    csUpd = CalcCodeChkSumSelf_new((int32u*)UPFW_CODE_ADDR, UPFW_CODE_SIZE);
    
    /* 上述校验和累加 */
    u32CS = csCfg + csPrg + csBlt + csUpd;
    
    //upgrade flash data（不能计算此处CHECHKSUM，否则与HEX文件对不上）
    //u32CS += CalcCodeChkSumSelf((int32u*)UPFW_FLASH_DATA_ADDR, UPFW_FLASH_DATA_SIZE);
    u32HexCS[0] = u32CS;
    
    
    /* 补码运算 */
    u32CS ^= 0xffffffff;
    u32CS += 1;
    u32HexCS[1] = u32CS;
    
    /* 这里调用无法发送的，因为串口还没初始化
     * sendPackageToMMS((int8u*)(&u32HexCS[0]), sizeof(u32HexCS));
     *  */
    
//  //to avoid write frequently
//  if(u32CS != UpFwState.Info.CS)
//  {
//      UpFwState.Info.CS = u32CS;
//      flag = 1;
//  }
//
//  if(flag)
//  {
//      bios_SaveState(&UpFwState);
//  }
}
#endif

//=========================================================
#if (ENABLE_UART_MSG == 1)
/*
void __attribute__ ((interrupt(ipl3AUTO))) __attribute__ ((vector(_UART_4_VECTOR))) U4artISR(void)
{
    //发送
    if ( INTGetFlag(INT_SOURCE_UART_TX(UPGRADE_UART)) )
    {
        // Clear the TX interrupt Flag
        INTClearFlag(INT_SOURCE_UART_TX(UPGRADE_UART));
    }

    //接收
    if(INTGetFlag(INT_SOURCE_UART_RX(UPGRADE_UART)))
    {
        // Clear the RX interrupt Flag
        INTClearFlag(INT_SOURCE_UART_RX(UPGRADE_UART));
    }
}
*/
#endif

