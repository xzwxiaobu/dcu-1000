
#include "..\Include.h"


#include "ModbusDef.h"
#include "SigSys_Uart.h"
#include "SigSys_Reg.h"
#include "SigSys_App.h"


#undef  PRIVATE 
#define PRIVATE 

/* 比较前后两次命令，若不相同则立即启动发送，避免相同的命令在 CREPEATSENDTIME 时间内多次发送 */
#define CREPEATSENDTIME     5           //seconds

typedef struct
{
    int8u TxData            : 1;      /* 1 表示有数据需要发送             */
    int8u CommErr           : 1;      /* 1 表示与信号系统通信异常         */
}tSigSysCommFlag;

PRIVATE tSigSysCommFlag sigSysCommFlag;

/* MODBUS 通信存储器 */
PRIVATE tModbusRequest  ModbusRequest;
PRIVATE tModbusReponse  ModbusReponse;
PRIVATE int8u Modbus_repLen;

/* 保存 PEDC 与 信号系统的通讯信息 */
PRIVATE int bakSigSysRxSize;
PRIVATE int8u bakSigSysRx[UART_SIGSYS_RXBUFSIZE];

PRIVATE int bakSigSysTxSize;
PRIVATE int8u bakSigSysTx[UART_SIGSYS_TXBUFSIZE];

PRIVATE void backupSigSys_Rx(int8u *pData, int8u size);
PRIVATE void backupSigSys_Tx(int8u *pData, int8u size);

/* 备份接收的写寄存器命令
 * 用于判断信号系统连续发送相同的命令
 *  */
PRIVATE int8u a_byBak_Fun0x10_content[BROAD_CMD_NUM][mb_fun0x10_request_dataBUF_LEN+ 2];

/* =============================================================================
 * 提供接口供外部调用
 */

/* 接收的信号系统信息数据 */
PUBLIC int8u * getRxFromSigSys(int8u * len)
{
    *len = bakSigSysRxSize;
    return bakSigSysRx;
}

/* 发送给信号系统的信息数据 */
PUBLIC int8u * getTxToSigSys(int8u * len)
{
    *len = bakSigSysTxSize;
    return bakSigSysTx;
}

/* =============================================================================
 * 处理待发送数据帧
 *  
 * 返回
 *  TRUE 表示还有数据没有发送
 *  */
PRIVATE BOOL procSigSysTxFrame(void)
{
    if(sigSysCommFlag.TxData)
    {
        if(sendDataFrameToSisSys(bakSigSysTx, bakSigSysTxSize) == TRUE)
        {
            sigSysCommFlag.TxData = 0;
        }
    }
    
    return (BOOL)(sigSysCommFlag.TxData);
}

/* =============================================================================
 * 处理接收的数据帧(已通过CRC校验)
 *  pFrame      : 数据帧指针（MODBUS协议帧）
 *  FrameLen    : 帧长度
 *  */
PRIVATE BOOL SigSysApp_ProcFrame(int8u * pFrame, int FrameLen)
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
    int8u  opCmd;
    int8u  wrReg;
    
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
        /* FUN 04 读输入寄存器 
         * Req : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2
         * Res : 单元标识1 + 功能码1 + 字节个数(=N*2) + 请求的数据N*2
         */
        case fun_04_ReadinputRegisters:
            Quantity = (ModbusRequest.Fun0x04Request.QuantityH << 8) + ModbusRequest.Fun0x04Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x04Request.StartingAddressH << 8) + ModbusRequest.Fun0x04Request.StartingAddressL;

            if(FrameLen < mb_fun0x4_request_fix_len)
            {
                exceptionCode = fun_ExceptionCode_0xb2;   //非法字串
            }
            else if((Quantity < 0x1) || (Quantity > (sizeof(tNJ7SigRegister)/sizeof(int16u))))
            {
                /* number of register is illegal */
                exceptionCode = fun04_ExceptionCode_03;
            }
            else if((StartingAddress < fun04_Write_StartingAddress) || ((StartingAddress + Quantity) > (sizeof(tNJ7SigRegister)/sizeof(int16u))))
            {
                /* start address is illegal */
                exceptionCode = fun04_ExceptionCode_02; 
            }

            if(exceptionCode == 0)
            { //正确应答RTU帧:从站地址  功能码  字节计数    数据    校验和CRCH  校验和CRCL
                //temp = Quantity * 16;
                backupSigSys_Rx(&ModbusRequest.Fun0x04Request.AdditionalAddr, FrameLen);

                ModbusReponse.Fun0x04Response.ByteCount = Quantity * 2;
                Modbus_repLen      = ModbusReponse.Fun0x04Response.ByteCount + mb_fun0x4_response_fix_len;

                /* 准备发送数据 */
                GetMODBUSMessage(temp_buf);

                for(i = 0; i < ModbusReponse.Fun0x04Response.ByteCount; i ++)
                {
                    ModbusReponse.Fun0x04Response.buf[i] = temp_buf[StartingAddress*2 + i];
                }

                CRC = CRC16_Dn_Cal(&ModbusReponse.Fun0x04Response.AdditionalAddr, ModbusReponse.Fun0x04Response.ByteCount + 3, 0xffff);
                ModbusReponse.Fun0x04Response.buf[ModbusReponse.Fun0x04Response.ByteCount + 1]     = (CRC >> 8) & 0xff ;   //校验H
                ModbusReponse.Fun0x04Response.buf[ModbusReponse.Fun0x04Response.ByteCount + 0]     = CRC & 0xff;           //校验L

                /* send data to sigsys */
                backupSigSys_Tx(&ModbusReponse.Fun0x04Response.AdditionalAddr,   Modbus_repLen);
            }
            break;
                
                
            /* FUN 0x10 写输入寄存器 
             * Req : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2 + 字节数1 + 寄存器值(长度=2*N)
             * Res : 单元标识1 + 功能码1 + 起始地址2 + 寄存器数量(N)2
             */
            case fun_16_WriteMultipleRegisters:
                bytecount = ModbusRequest.Fun0x10Request.ByteCount;
                Quantity = (ModbusRequest.Fun0x10Request.QuantityH << 8) + ModbusRequest.Fun0x10Request.QuantityL;
                StartingAddress = (ModbusRequest.Fun0x10Request.StartingAddressH << 8) + ModbusRequest.Fun0x10Request.StartingAddressL;

                /* 信号系统不应写 2-7 号寄存器 
                 * 为了安全起见，不支持信号系统只写部分寄存器（设置对位隔离）
                 * 每次必须将所有门机的对位隔离完全设置
                 */
                
                if(FrameLen < (bytecount    + mb_fun0x10_request_fix_len))
                {
                    exceptionCode = fun_ExceptionCode_0xb2;   //非法字串
                }   
                else if(((Quantity < 0x1) || (Quantity > 2)) || ((bytecount/2) != Quantity))
                {
                    exceptionCode = fun0x10_ExceptionCode_03; 
                }
                else if((StartingAddress != mb_fun0x10_StartingAddress) || ((StartingAddress + Quantity) != mb_fun0x10_request_ByteCount_max/2))
                {
                    exceptionCode = fun0x10_ExceptionCode_02; 
                }
                else
                {
                    backupSigSys_Rx(&ModbusRequest.Fun0x10Request.AdditionalAddr, FrameLen);
                    
                    tBroadCmdPkg tmpBroadCmd;
                    
                    //sent to mms
                    for(j = 0; j < BROADCOMMAND_LEN; j ++)
                    {
                        uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[j] = 0;
                        tmpBroadCmd.broadCmdBuf[j] = 0;
                    }

                    for(i = 0; i <= sizeof(buf); i ++)
                    {
                        buf[i] = 0;
                    }
                    
                    
                    for(i = 0; i < bytecount; i ++)
                    {/* 比较前后两次命令，若不相同则立即启动发送，避免相同的命令在 CREPEATSENDTIME 时间内多次发送 */
                        if(a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] != ModbusRequest.Fun0x10Request.buf[i])
                        {
                            broadCmdList[BROAD_CMD_STC_INHIBIT].uRepeatSendTime = CREPEATSENDTIME;   /*目的：收到命令，立即发送*/
                            break;
                        }
                    }
                    
                    for(i = 0; i < bytecount; i ++)
                    {
                        a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] = ModbusRequest.Fun0x10Request.buf[i];
                    }
                    
                    exceptionCode = 1;                              /* 报错：指定时间内重复发送 */

                    //如果没有下面这条判断语句，则会每次都启动发送
                    if(broadCmdList[BROAD_CMD_STC_INHIBIT].uRepeatSendTime >= CREPEATSENDTIME)   /*立即发送*/
                    {
                        exceptionCode = 0;
                        temp = mb_fun0x10_request_ByteCount_max;
                        p = (int8u *)&ModbusRequest.Fun0x10Request.buf[0];
                        i = 0;

                        for(;;)
                        {//16位数组，转成8位数组； 数1H 数1L 数2H 数2L ... ---> 数1L 数1H 数2L 数2H...
                            buf[i] =  *(p + 1);

                            buf[i + 1] =  *p;
                            i += 2;
                            if(i >= temp)
                                break;
                            p += 2; 

                        }   

                        j = bytecount;
                        if(j > BROADCOMMAND_LEN)
                            j = BROADCOMMAND_LEN;

                        for(i = 0; i < j; i ++)
                        {
                            uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[i]  = buf[i];
                            tmpBroadCmd.broadCmdBuf[i]                      = buf[i];  //命令
                        }

                        tmpBroadCmd.oprCmd1.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | DCUCMD1_STC_INHIBIT ;
                        tmpBroadCmd.oprCmd2.byte = 0;
                        tmpBroadCmd.oprCmd3.byte = 0;
                        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);
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
                    
                    /* send data to sigsys */
                    backupSigSys_Tx(&ModbusReponse.Fun0x10Response.AdditionalAddr, Modbus_repLen);
                }
                break;  
            
            default:
                
                exceptionCode = functoinCodeNotSupported;  //不支持该功能码 
                break;
        }
        
    sigSysCommFlag.CommErr = FALSE;
    if((exceptionCode != 0) && (exceptionCode != fun_ExceptionCode_0xb2))
    {//异常
        ModbusReponse.Exception.data          = exceptionCode;
        ModbusReponse.Exception.FunctionCode += 0x80;
        CRC = CRC16_Dn_Cal((int8u *)&ModbusReponse.Exception, 3, 0xffff);
        ModbusReponse.Exception.CRCH = (CRC >> 8) & 0xff;    //校验H
        ModbusReponse.Exception.CRCL = CRC & 0xff;           //校验L

        Modbus_repLen = 5;
        sigSysCommFlag.CommErr = TRUE;
        
        /* send data to sigsys */
        backupSigSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
    }   

    if(exceptionCode != fun_ExceptionCode_0xb2)
    {
        /* 有数据帧 待发送至
         * 保存发送的数据
         *  */
        sigSysCommFlag.TxData = 1;
    }
}

/* 
 * =============================================================================
 * 与 信号系统 通信处理
 * 需要在主程序中循环调用
 * =============================================================================
 */
PUBLIC void processSigSysComm(void)
{
    /* 这里是通过时间判断信号系统发送数据帧是否结束
     *  */
    chkSigSysFinishSend();
    
    /* 判断是否有收到数据帧，并处理
     *  */
    procSigSysRxFrame(SigSysApp_ProcFrame);
    
    /* 判断是否有待发送的数据帧，有则发送
     *  */
    procSigSysTxFrame();

//    /* 上传给MMS，站台门对位隔离列车门
//     * */
//    {
//        extern tNJ7SigRegister NJ7SigRegister;
//        static int16u updateInhibitTime = 0;
//        int16u tickNow = TgtGetTickCount();
//        if((int16u)(tickNow - updateInhibitTime) > 100)
//        {
//            updateInhibitTime = tickNow;
//
//            int8u  temp_buf[mb_fun0x4_response_ByteCount_max + 10];
//            int8u *pPSDFault = (int8u*)(&NJ7SigRegister.rPSDFault0);
//            int8u *pToMMS = (int8u*)(&uniDcuData.sortBuf.aMcpOpenCloseDoorCommand) + 8;
//
//            GetMODBUSMessage(temp_buf);
//            *pToMMS ++= *pPSDFault++;
//            *pToMMS ++= *pPSDFault++;
//            *pToMMS ++= *pPSDFault++;
//            *pToMMS ++= *pPSDFault++;
//            *pToMMS ++ = 0;
//            *pToMMS ++ = 0;
//            *pToMMS ++ = 0;
//            *pToMMS ++ = 0;
//        }
//    }
    
    /* 判断信号系统是否离线
     * 
     * 信号系统定时 向站台门系统发送一次请求报文，同时站台门反馈信息。 
     * 若站台门系统持续一段时间 （SIGSYS_OFFLIN_TIME）没有收到信号系统的轮询指令，则判定为通信超时，
     * 此时站台门系统自动复位车门对位隔离站台门标志，在此情况下，站台门系统将按信号系统硬线接口指令执行。
     */
    if(uartChkLinkAndSendSts(COM_SIGSYS1) == TRUE)
    {
        /* 信号系统离线
         * 向 DCU 发送广播命令，清除指定的标志
         */
        int i;
        tBroadCmdPkg tmpBroadCmd;

        for(i = 0; i < BROADCOMMAND_LEN; i ++)
        {
            uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[i]  = 0;
            tmpBroadCmd.broadCmdBuf[i]                      = 0;  //命令
        }

        /* 发送命令清除DCU的 STC inhibit */
        tmpBroadCmd.oprCmd1.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | DCUCMD1_STC_INHIBIT ;
        tmpBroadCmd.oprCmd2.byte = 0 ;
        tmpBroadCmd.oprCmd3.byte = 0 ;
        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);

        /* 清备份内容 */
        for(i = 0; i < sizeof(a_byBak_Fun0x10_content[0]); i ++)
        {
            a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] = 0;
        }
    }
}


/* =============================================================================
 * 备份来信号系统的数据，供测试用
 */
PRIVATE void backupSigSys_Rx(int8u *pData, int8u size)
{
    int i;
    
    bakSigSysRxSize = size;
    if(bakSigSysRxSize > sizeof(bakSigSysRx))
    {
        bakSigSysRxSize = sizeof(bakSigSysRx);
    }
    
    for(i=0; i<bakSigSysRxSize; i++ )
    {
        bakSigSysRx[i] = pData[i];
    }
}

/* 回复信号系统的数据，
 *   - 发送给信号系统
 *   - 发送给PC测试用
 */
PRIVATE void backupSigSys_Tx(int8u *pData, int8u size)
{
    int i;
    
    bakSigSysTxSize = size;
    if(bakSigSysTxSize > sizeof(bakSigSysTx))
    {
        bakSigSysTxSize = sizeof(bakSigSysTx);
    }
    
    for(i=0; i<bakSigSysTxSize; i++ )
    {
        bakSigSysTx[i] = pData[i];
    }
}
