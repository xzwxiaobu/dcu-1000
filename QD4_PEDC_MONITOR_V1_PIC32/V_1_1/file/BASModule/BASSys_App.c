
#include "..\Include.h"

#include "ModbusDef.h"
#include "BASSys_App.h"
#include "dcu.h"

#ifdef  PRIVATE_STATIC_DIS
#undef  PRIVATE 
#define PRIVATE
#endif

typedef struct
{
    int8u TxData            : 1;      /* 1 表示有数据需要发送            */
    int8u CommErr           : 1;      /* 1 表示与BAS系统通信异常         */    
}tBASSysCommFlag;

PRIVATE tBASSysCommFlag BASSysCommFlag;

/* MODBUS 通信存储器 */
PRIVATE tModbusRequest  ModbusRequest;
PRIVATE tModbusReponse  ModbusReponse;
PRIVATE int8u           Modbus_repLen;

/* 保存 PEDC 与 BAS系统的通讯信息 */
PRIVATE int     bakBASSysRxSize;
PRIVATE int8u   bakBASSysRx[UART_BASSYS_RXBUFSIZE];

PRIVATE int     bakBASSysTxSize;
PRIVATE int8u   bakBASSysTx[UART_BASSYS_TXBUFSIZE];

/* 风阀控制信号来源，优先级：IBP > MMS > BAS
 */
PRIVATE int8u windCtrlSrc;

/* =============================================================================
 * 本地函数
 */
PRIVATE void backupBASSys_Rx(int8u *pData, int8u size);
PRIVATE void backupBASSys_Tx(int8u *pData, int8u size);

/* =============================================================================
 * 提供接口供外部调用
 */

/* 接收的BAS系统信息数据 */
PUBLIC int8u * getRxFromBASSys(int8u * len)
{
    *len = bakBASSysRxSize;
    return bakBASSysRx;
}

/* 发送给BAS系统的信息数据 */
PUBLIC int8u * getTxToBASSys(int8u * len)
{
    *len = bakBASSysTxSize;
    return bakBASSysTx;
}

/* =============================================================================
 * 处理待发送数据帧
 *  
 * 返回
 *  TRUE 表示还有数据没有发送
 *  */
PRIVATE BOOL procBASSysTxFrame(void)
{
    if(BASSysCommFlag.TxData)
    {
        if(sendDataFrameToBASSys(bakBASSysTx, bakBASSysTxSize) == TRUE)
        {
            BASSysCommFlag.TxData = 0;
        }
    }
    
    return (BOOL)(BASSysCommFlag.TxData);
}

/* =============================================================================
 * 处理接收的数据帧(已通过CRC校验)
 *  pFrame      : 数据帧指针（MODBUS协议帧）
 *  FrameLen    : 帧长度
 *  */
/* 风阀 控制处理 */
PRIVATE BOOL BASSysApp_ProcFrame(int8u * pFrame, int FrameLen)
{
    int8u  *p;
    int  i,j;
    int16u temp;
    unsigned short CRC;
    int8u  exceptionCode = 0;
    int16u Quantity;
    int16u StartingAddress;
    int16u bytecount;
    int8u  buf[(MAX_DCU+7) / 8 + mb_fun0x3_response_ByteCount_max];
    int8u  temp_buf[mb_fun0x3_response_ByteCount_max + 10];
    int8u  wrReg;

    
    tWindValveSysSts * pWVSts = getWindValveSysSts();

    Modbus_repLen = 1;

    p = (int8u *) &ModbusRequest;
    for(i = 0; i < sizeof(ModbusRequest); i ++)
    {
        *p ++ = 0;  /* 清0 */
    }

    p = (int8u *) &ModbusRequest;
    for(i = 0; i < FrameLen; i ++)
    {
        *p ++ = pFrame[i];
    }

    ModbusReponse.FunAll.AdditionalAddr = ModbusRequest.FunAll.AdditionalAddr;
    ModbusReponse.FunAll.FunctionCode   = ModbusRequest.FunAll.FunctionCode;

    switch(ModbusRequest.FunAll.FunctionCode)
    {
        /* FUN 03 读保持寄存器 */
        case fun_03_ReadHoldingRegisters:

            Quantity = (ModbusRequest.Fun0x03Request.QuantityH << 8) + ModbusRequest.Fun0x03Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x03Request.StartingAddressH << 8) + ModbusRequest.Fun0x03Request.StartingAddressL;

            if(FrameLen < (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
            {
                exceptionCode = fun03_ExceptionCode_03; //fun_ExceptionCode_0xb2;   //非法字串
            }   
            else if((Quantity < 0x1) || (Quantity > (sizeof(bas_reg03)/sizeof(int16u))))
            {
                /* number of register is illegal */
                exceptionCode = fun03_ExceptionCode_03;
            }
            else if((StartingAddress < fun03_Write_StartingAddress) || ((StartingAddress + Quantity) > (sizeof(bas_reg03)/sizeof(int16u))))
            {
                /* start address is illegal */
                exceptionCode = fun03_ExceptionCode_02; 
            }

            if(exceptionCode == 0)
            { //正确应答RTU帧:从站地址  功能码  字节计数    数据    校验和CRCH  校验和CRCL
                //temp = Quantity * 16;
                backupBASSys_Rx(&ModbusRequest.Fun0x03Request.AdditionalAddr, FrameLen);

                ModbusReponse.Fun0x03Response.ByteCount = Quantity * 2;
                Modbus_repLen      = ModbusReponse.Fun0x03Response.ByteCount + mb_fun0x3_response_fix_len;

                /* 准备发送数据 */
                GetReplyBASMessage(temp_buf);

                for(i = 0; i < ModbusReponse.Fun0x03Response.ByteCount; i ++)
                {
                    ModbusReponse.Fun0x03Response.buf[i] = temp_buf[StartingAddress*2 + i];
                }

                CRC = CRC16_Modbus(&ModbusReponse.Fun0x03Response.AdditionalAddr, ModbusReponse.Fun0x03Response.ByteCount + 3);
                ModbusReponse.Fun0x03Response.buf[ModbusReponse.Fun0x03Response.ByteCount + 1]     = (CRC >> 8) & 0xff ;   //校验H
                ModbusReponse.Fun0x03Response.buf[ModbusReponse.Fun0x03Response.ByteCount + 0]     = CRC & 0xff;           //校验L

                /* for test function */
                backupBASSys_Tx(&ModbusReponse.Fun0x03Response.AdditionalAddr,   Modbus_repLen);
            }
            break;

        /* FUN 04 读输入寄存器 
         * Req : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2
         * Res : 单元标识1 + 功能码1 + 字节个数(=N*2) + 请求的数据N*2
         */
        case fun_04_ReadinputRegisters:
            break;

        /* FUN 0x10 写输入寄存器 
         * Req : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2 + 字节数1 + 寄存器值(长度=2*N)
         * Res : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2
         */
        case fun_16_WriteMultipleRegisters:
            bytecount = ModbusRequest.Fun0x10Request.ByteCount;
            Quantity = (ModbusRequest.Fun0x10Request.QuantityH << 8) + ModbusRequest.Fun0x10Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x10Request.StartingAddressH << 8) + ModbusRequest.Fun0x10Request.StartingAddressL;

            if(FrameLen < (bytecount + (sizeof(tModbusWrHeader) + sizeof(tModbusTail))))
            {
                exceptionCode = fun0x10_ExceptionCode_03; //fun_ExceptionCode_0xb2;   //非法字串
            }   
            else if(((Quantity < 0x1) || (Quantity > 0x7b)) || (bytecount != (int8u)(Quantity * 2)))
            {
                exceptionCode = fun0x10_ExceptionCode_03; 
            }

            else if((StartingAddress != mb_fun0x10_StartingAddress) || (ModbusRequest.Fun0x10Request.ByteCount != mb_fun0x10_request_ByteCount_max))
            {
                exceptionCode = fun0x10_ExceptionCode_02; 
            }
            else
            {
                /* 备份接收的BAS操作，发送到PC测试端观察 */
                backupBASSys_Rx(&ModbusRequest.Fun0x10Request.AdditionalAddr, FrameLen);

                /* modbus 协议WORD，高字节在前，低字节在后 */
                tBASRegister * pBasReg = (tBASRegister *)getBASRegister();
                
                pBasReg->reg10.wBASCmd = ((int16u)ModbusRequest.Fun0x10Request.buf[1]) | ((int16u)ModbusRequest.Fun0x10Request.buf[0] << 8);
                
                /* bit0: 0=BAS开风阀指令无效  1=BAS开风阀指令有效
                 * bit1: 0=BAS关风阀指令无效  1=BAS关风阀指令有效
                 */
                pWVSts->bBASOpnCmd_sts = pBasReg->reg10.bits.BAS_OpnCmd;
                pWVSts->bBASClsCmd_sts = pBasReg->reg10.bits.BAS_ClsCmd;

                if(pWVSts->bWindCtrlBy_PSC)
                {
                    /* PSC正在控制风阀，回复非法功能 */
                    exceptionCode = fun0x10_ExceptionCode_01;
                }
                else if((pBasReg->reg10.bits.BAS_ClsCmd == 1) && (pBasReg->reg10.bits.BAS_OpnCmd == 1))
                {
                    /* 非法命令 */
                    exceptionCode = fun0x10_ExceptionCode_01;   //fun_ExceptionCode_0xb2;
                }
                else
                {
                    /* 添加广播命令：开关风阀 */
                    tBroadCmdPkg tmpBroadCmd;
                    tmpBroadCmd.oprCmd1.byte = 0;
                    tmpBroadCmd.oprCmd2.byte = 0;
                    tmpBroadCmd.oprCmd3.byte = 0;
                    for(i = 0; i < BROADCOMMAND_LEN; i ++)
                    {
                        //uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[2*8 + i] = buf[i];
                        tmpBroadCmd.broadCmdBuf[i] = 0xff;
                    }
                    
                    if((pBasReg->reg10.bits.BAS_ClsCmd == 0) && (pBasReg->reg10.bits.BAS_OpnCmd == 0))
                    {
                        /* 发送清除开关命令给 DCU */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
                        tmpBroadCmd.oprCmd2.byte  =  DCUCMD1_CLS | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
                    }
                    else if(pBasReg->reg10.bits.BAS_OpnCmd == 1)
                    {
                        /* 开风阀 */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                    }
                    else
                    {
                        /* 关风阀 */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_CLS | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                    }

                    addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
                    exceptionCode = 0;

                    #if 0
                    //检测是否所有的DCU都在线，如果有不在线的DCU则报错
                    chkAllDCUOnLineSts(temp_buf, MAX_DCU, cDcuOnLineF);

                    for(i = 0; i < (mb_fun0x10_request_ByteCount_max); i ++)
                    {
                        if( (temp_buf[i] & buf[i])  != buf[i])
                        {//请求的DCU有不在线，则报警
                            //exceptionCode = fun0x10_ExceptionCode_04;
                            break;
                        }
                    }
                    #endif
                }
            }

            if(exceptionCode == 0)
            { //正确应答RTU帧:从站地址  功能码  起始地址高位    起始地址低位    寄存器数高位    寄存器数低位    校验和CRCH  校验和CRCL
                ModbusReponse.Fun0x10Response.StartingAddressH  = ModbusRequest.Fun0x10Request.StartingAddressH;
                ModbusReponse.Fun0x10Response.StartingAddressL  = ModbusRequest.Fun0x10Request.StartingAddressL;
                ModbusReponse.Fun0x10Response.QuantityH         = ModbusRequest.Fun0x10Request.QuantityH;
                ModbusReponse.Fun0x10Response.QuantityL         = ModbusRequest.Fun0x10Request.QuantityL;
                CRC = CRC16_Modbus(&ModbusReponse.Fun0x10Response.AdditionalAddr, sizeof(tModbusFixHeader));
                ModbusReponse.Fun0x10Response.CRCH              = (CRC >> 8) & 0xff;    //校验H
                ModbusReponse.Fun0x10Response.CRCL              = CRC & 0xff;           //校验L

                Modbus_repLen = mb_fun0x10_response_fix_len;
                
                backupBASSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
            }

            break;  

        default:

            exceptionCode = functoinCodeNotSupported;  //不支持该功能码 
            break;
    }

    BASSysCommFlag.CommErr = FALSE;
    if((exceptionCode != 0) && (exceptionCode != fun_ExceptionCode_0xb2))
    {//异常
        ModbusReponse.Exception.data          = exceptionCode;
        ModbusReponse.Exception.FunctionCode += 0x80;
        CRC = CRC16_Modbus((int8u *)&ModbusReponse.Exception, 3);
        ModbusReponse.Exception.CRCH = (CRC >> 8) & 0xff;    //校验H
        ModbusReponse.Exception.CRCL = CRC & 0xff;           //校验L

        Modbus_repLen = 5;
        BASSysCommFlag.CommErr = TRUE;
        
        backupBASSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
    }   

    if(exceptionCode != fun_ExceptionCode_0xb2)
    {
        /* 有数据帧 待发送至
         * 保存发送的数据
         *  */
        BASSysCommFlag.TxData = 1;
    }
}

/* 
 * =============================================================================
 * PSC 控制风阀的处理
 * 
 * =============================================================================
 */
extern BOOL chkDelayProcessIO(void);
PRIVATE void processPSCOprWindValve(void)
{
    static int8u WVCmd_Bak = 0xff;
    static int8u waitCmdStable = 0;
    int8u  WVCmd = 0;
    tBroadCmdPkg tmpBroadCmd;
    tWindValveSysSts * pWVSts;
    
    /* 复位后等待延迟时间 */
    if(chkDelayProcessIO() == FALSE)
    {
        return;
    }
    
    pWVSts = getWindValveSysSts();
    if(ifTestBASRegValue() == FALSE)
    {
        pWVSts->bPSCOpnCmd_sts  = GET_INPUT_STS(MI_Opn_WindWindow);
        pWVSts->bPSCClsCmd_sts  = GET_INPUT_STS(MI_Cls_WindWindow);
        pWVSts->bWindCtrlBy_PSC = pWVSts->bPSCOpnCmd_sts | pWVSts->bPSCClsCmd_sts;
    }

    static int8u bPSC_OpnWV_Bak = 0;
    static int8u bPSC_ClsWV_Bak = 0;
    tmpBroadCmd.oprCmd3.byte = 0;

    if(pWVSts->bPSCOpnCmd_sts != pWVSts->bPSCClsCmd_sts)
    {
        if(bPSC_OpnWV_Bak != (int8u)pWVSts->bPSCOpnCmd_sts)
        {
            /* 清除 BAS 控制风阀的标志 */
            pWVSts->bBASOpnCmd_sts = 0;
            pWVSts->bBASClsCmd_sts = 0;

            /* 触发开风阀 */
            if(pWVSts->bPSCOpnCmd_sts)
            {
                WVCmd = 1;
                tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                tmpBroadCmd.oprCmd2.byte = 0;
            }
        }

        if(bPSC_ClsWV_Bak != (int8u)pWVSts->bPSCClsCmd_sts)
        {
            /* 清除 BAS 控制风阀的标志 */
            pWVSts->bBASOpnCmd_sts = 0;
            pWVSts->bBASClsCmd_sts = 0;
            
            /* 触发关风阀 */
            if(pWVSts->bPSCClsCmd_sts)
            {
                WVCmd = 2;
                tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_CLS | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                tmpBroadCmd.oprCmd2.byte = 0;
            }
        }

    }
    else
    {
        /* 判断：若 PSC 风阀开关命令 切换到自动
         * 则发送清除开关命令给 DCU */
        if((pWVSts->bPSCOpnCmd_sts == 0))
        {
            if ((bPSC_OpnWV_Bak != 0 ) || (bPSC_ClsWV_Bak != 0 ))
            {
                WVCmd = 3;
                tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
                tmpBroadCmd.oprCmd2.byte  =  DCUCMD1_CLS | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
            }
        }
    }
    
    /* PSC 发送控制风阀命令
     * 为了避免旋钮过程中发送清除 开和关 命令
     * 做了稳定处理 */
    if(WVCmd != 0)
    {
        if(WVCmd_Bak != WVCmd)
        {
            WVCmd_Bak       = WVCmd;
            waitCmdStable   = 0;
        }
        else
        {
            waitCmdStable ++;
            if(waitCmdStable >= 5)
            {
                waitCmdStable = 5;
                
                int i;

                /* 添加发送命令给 DCU ： 开风阀或关风阀 */
                for(i = 0; i < BROADCOMMAND_LEN; i ++)
                {
                    tmpBroadCmd.broadCmdBuf[i] = 0xff;
                }
                addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
                
                bPSC_OpnWV_Bak = pWVSts->bPSCOpnCmd_sts;
                bPSC_ClsWV_Bak = pWVSts->bPSCClsCmd_sts;
            }
        }
    }

}

/* 
 * =============================================================================
 * 与 BAS系统 通信处理
 * 需要在主程序中循环调用
 * =============================================================================
 */
PUBLIC void processBASSysComm(void)
{
    static tSYSTICK lastTick;
    
    
    tSYSTICK tickNow;
    tickNow = getSysTick();
    if((tSYSTICK)(tickNow - lastTick) >= 200)
    {
        lastTick = tickNow;
        
        /* 更新回复 BAS系统 的寄存器 */
        GetReplyBASMessage(NULL);
        
        /* BAS 系统接口调试测试数据的时间 */
        decTestBASRegTime(200);
        
        /* 是否 PSC 控制风阀 */
        processPSCOprWindValve();
    }

    
    /* 这里是通过时间判断BAS系统发送数据帧是否结束
     *  */
    chkBASSysFinishSend();
    
    /* 判断是否有收到数据帧，并处理
     *  */
    procBASSysRxFrame(BASSysApp_ProcFrame);
    
    /* 判断是否有待发送的数据帧，有则发送
     *  */
    procBASSysTxFrame();
    
    /* 判断BAS系统是否离线
     * 
     * BAS系统定时 向站台门系统发送一次请求报文，同时站台门反馈信息。 
     * 若站台门系统持续一段时间 （BASSys_OFFLIN_TIME）没有收到BAS系统的轮询指令，则判定为通信超时，
     */
    if(uartChkLinkAndSendSts(COM_BASSYS) == TRUE)
    {
        tWindValveSysSts * pWVSts = getWindValveSysSts();
        pWVSts->bBASOpnCmd_sts = 0;
        pWVSts->bBASClsCmd_sts = 0;
        
        #if 0
        /* BAS 离线
         * 向 DCU 发送命令，清除 开风阀、关风阀 
         */
        tBroadCmdPkg tmpBroadCmd;

        memset((int8u*)tmpBroadCmd.broadCmdBuf, 0xff, sizeof(tmpBroadCmd.broadCmdBuf));
        tmpBroadCmd.oprCmd3.byte = 0;
        /* 清除: 开风阀 */
        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
        /* 清除: 关风阀 */
        tmpBroadCmd.oprCmd2.byte  =  DCUCMD1_CLS | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
        #endif
    }

}


/* =============================================================================
 * 备份来BAS系统的数据，供测试用
 */
PRIVATE void backupBASSys_Rx(int8u *pData, int8u size)
{
    int i;
    
    bakBASSysRxSize = size;
    if(bakBASSysRxSize > sizeof(bakBASSysRx))
    {
        bakBASSysRxSize = sizeof(bakBASSysRx);
    }
    
    for(i=0; i<bakBASSysRxSize; i++ )
    {
        bakBASSysRx[i] = pData[i];
    }
}

/* 回复BAS系统的数据，
 *   - 发送给BAS系统
 *   - 发送给PC测试用
 */
PRIVATE void backupBASSys_Tx(int8u *pData, int8u size)
{
    int i;
    
    bakBASSysTxSize = size;
    if(bakBASSysTxSize > sizeof(bakBASSysTx))
    {
        bakBASSysTxSize = sizeof(bakBASSysTx);
    }
    
    for(i=0; i<bakBASSysTxSize; i++ )
    {
        bakBASSysTx[i] = pData[i];
    }
}




