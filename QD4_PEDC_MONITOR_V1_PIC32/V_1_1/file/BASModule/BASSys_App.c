
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
    int8u TxData            : 1;      /* 1 ��ʾ��������Ҫ����            */
    int8u CommErr           : 1;      /* 1 ��ʾ��BASϵͳͨ���쳣         */    
}tBASSysCommFlag;

PRIVATE tBASSysCommFlag BASSysCommFlag;

/* MODBUS ͨ�Ŵ洢�� */
PRIVATE tModbusRequest  ModbusRequest;
PRIVATE tModbusReponse  ModbusReponse;
PRIVATE int8u           Modbus_repLen;

/* ���� PEDC �� BASϵͳ��ͨѶ��Ϣ */
PRIVATE int     bakBASSysRxSize;
PRIVATE int8u   bakBASSysRx[UART_BASSYS_RXBUFSIZE];

PRIVATE int     bakBASSysTxSize;
PRIVATE int8u   bakBASSysTx[UART_BASSYS_TXBUFSIZE];

/* �緧�����ź���Դ�����ȼ���IBP > MMS > BAS
 */
PRIVATE int8u windCtrlSrc;

/* =============================================================================
 * ���غ���
 */
PRIVATE void backupBASSys_Rx(int8u *pData, int8u size);
PRIVATE void backupBASSys_Tx(int8u *pData, int8u size);

/* =============================================================================
 * �ṩ�ӿڹ��ⲿ����
 */

/* ���յ�BASϵͳ��Ϣ���� */
PUBLIC int8u * getRxFromBASSys(int8u * len)
{
    *len = bakBASSysRxSize;
    return bakBASSysRx;
}

/* ���͸�BASϵͳ����Ϣ���� */
PUBLIC int8u * getTxToBASSys(int8u * len)
{
    *len = bakBASSysTxSize;
    return bakBASSysTx;
}

/* =============================================================================
 * �������������֡
 *  
 * ����
 *  TRUE ��ʾ��������û�з���
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
 * ������յ�����֡(��ͨ��CRCУ��)
 *  pFrame      : ����ָ֡�루MODBUSЭ��֡��
 *  FrameLen    : ֡����
 *  */
/* �緧 ���ƴ��� */
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
        *p ++ = 0;  /* ��0 */
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
        /* FUN 03 �����ּĴ��� */
        case fun_03_ReadHoldingRegisters:

            Quantity = (ModbusRequest.Fun0x03Request.QuantityH << 8) + ModbusRequest.Fun0x03Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x03Request.StartingAddressH << 8) + ModbusRequest.Fun0x03Request.StartingAddressL;

            if(FrameLen < (sizeof(tModbusFixHeader) + sizeof(tModbusTail)))
            {
                exceptionCode = fun03_ExceptionCode_03; //fun_ExceptionCode_0xb2;   //�Ƿ��ִ�
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
            { //��ȷӦ��RTU֡:��վ��ַ  ������  �ֽڼ���    ����    У���CRCH  У���CRCL
                //temp = Quantity * 16;
                backupBASSys_Rx(&ModbusRequest.Fun0x03Request.AdditionalAddr, FrameLen);

                ModbusReponse.Fun0x03Response.ByteCount = Quantity * 2;
                Modbus_repLen      = ModbusReponse.Fun0x03Response.ByteCount + mb_fun0x3_response_fix_len;

                /* ׼���������� */
                GetReplyBASMessage(temp_buf);

                for(i = 0; i < ModbusReponse.Fun0x03Response.ByteCount; i ++)
                {
                    ModbusReponse.Fun0x03Response.buf[i] = temp_buf[StartingAddress*2 + i];
                }

                CRC = CRC16_Modbus(&ModbusReponse.Fun0x03Response.AdditionalAddr, ModbusReponse.Fun0x03Response.ByteCount + 3);
                ModbusReponse.Fun0x03Response.buf[ModbusReponse.Fun0x03Response.ByteCount + 1]     = (CRC >> 8) & 0xff ;   //У��H
                ModbusReponse.Fun0x03Response.buf[ModbusReponse.Fun0x03Response.ByteCount + 0]     = CRC & 0xff;           //У��L

                /* for test function */
                backupBASSys_Tx(&ModbusReponse.Fun0x03Response.AdditionalAddr,   Modbus_repLen);
            }
            break;

        /* FUN 04 ������Ĵ��� 
         * Req : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2
         * Res : ��Ԫ��ʶ1 + ������1 + �ֽڸ���(=N*2) + ���������N*2
         */
        case fun_04_ReadinputRegisters:
            break;

        /* FUN 0x10 д����Ĵ��� 
         * Req : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2 + �ֽ���1 + �Ĵ���ֵ(����=2*N)
         * Res : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2
         */
        case fun_16_WriteMultipleRegisters:
            bytecount = ModbusRequest.Fun0x10Request.ByteCount;
            Quantity = (ModbusRequest.Fun0x10Request.QuantityH << 8) + ModbusRequest.Fun0x10Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x10Request.StartingAddressH << 8) + ModbusRequest.Fun0x10Request.StartingAddressL;

            if(FrameLen < (bytecount + (sizeof(tModbusWrHeader) + sizeof(tModbusTail))))
            {
                exceptionCode = fun0x10_ExceptionCode_03; //fun_ExceptionCode_0xb2;   //�Ƿ��ִ�
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
                /* ���ݽ��յ�BAS���������͵�PC���Զ˹۲� */
                backupBASSys_Rx(&ModbusRequest.Fun0x10Request.AdditionalAddr, FrameLen);

                /* modbus Э��WORD�����ֽ���ǰ�����ֽ��ں� */
                tBASRegister * pBasReg = (tBASRegister *)getBASRegister();
                
                pBasReg->reg10.wBASCmd = ((int16u)ModbusRequest.Fun0x10Request.buf[1]) | ((int16u)ModbusRequest.Fun0x10Request.buf[0] << 8);
                
                /* bit0: 0=BAS���緧ָ����Ч  1=BAS���緧ָ����Ч
                 * bit1: 0=BAS�ط緧ָ����Ч  1=BAS�ط緧ָ����Ч
                 */
                pWVSts->bBASOpnCmd_sts = pBasReg->reg10.bits.BAS_OpnCmd;
                pWVSts->bBASClsCmd_sts = pBasReg->reg10.bits.BAS_ClsCmd;

                if(pWVSts->bWindCtrlBy_PSC)
                {
                    /* PSC���ڿ��Ʒ緧���ظ��Ƿ����� */
                    exceptionCode = fun0x10_ExceptionCode_01;
                }
                else if((pBasReg->reg10.bits.BAS_ClsCmd == 1) && (pBasReg->reg10.bits.BAS_OpnCmd == 1))
                {
                    /* �Ƿ����� */
                    exceptionCode = fun0x10_ExceptionCode_01;   //fun_ExceptionCode_0xb2;
                }
                else
                {
                    /* ��ӹ㲥������ط緧 */
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
                        /* ���������������� DCU */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
                        tmpBroadCmd.oprCmd2.byte  =  DCUCMD1_CLS | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
                    }
                    else if(pBasReg->reg10.bits.BAS_OpnCmd == 1)
                    {
                        /* ���緧 */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                    }
                    else
                    {
                        /* �ط緧 */
                        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_CLS | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                    }

                    addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
                    exceptionCode = 0;

                    #if 0
                    //����Ƿ����е�DCU�����ߣ�����в����ߵ�DCU�򱨴�
                    chkAllDCUOnLineSts(temp_buf, MAX_DCU, cDcuOnLineF);

                    for(i = 0; i < (mb_fun0x10_request_ByteCount_max); i ++)
                    {
                        if( (temp_buf[i] & buf[i])  != buf[i])
                        {//�����DCU�в����ߣ��򱨾�
                            //exceptionCode = fun0x10_ExceptionCode_04;
                            break;
                        }
                    }
                    #endif
                }
            }

            if(exceptionCode == 0)
            { //��ȷӦ��RTU֡:��վ��ַ  ������  ��ʼ��ַ��λ    ��ʼ��ַ��λ    �Ĵ�������λ    �Ĵ�������λ    У���CRCH  У���CRCL
                ModbusReponse.Fun0x10Response.StartingAddressH  = ModbusRequest.Fun0x10Request.StartingAddressH;
                ModbusReponse.Fun0x10Response.StartingAddressL  = ModbusRequest.Fun0x10Request.StartingAddressL;
                ModbusReponse.Fun0x10Response.QuantityH         = ModbusRequest.Fun0x10Request.QuantityH;
                ModbusReponse.Fun0x10Response.QuantityL         = ModbusRequest.Fun0x10Request.QuantityL;
                CRC = CRC16_Modbus(&ModbusReponse.Fun0x10Response.AdditionalAddr, sizeof(tModbusFixHeader));
                ModbusReponse.Fun0x10Response.CRCH              = (CRC >> 8) & 0xff;    //У��H
                ModbusReponse.Fun0x10Response.CRCL              = CRC & 0xff;           //У��L

                Modbus_repLen = mb_fun0x10_response_fix_len;
                
                backupBASSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
            }

            break;  

        default:

            exceptionCode = functoinCodeNotSupported;  //��֧�ָù����� 
            break;
    }

    BASSysCommFlag.CommErr = FALSE;
    if((exceptionCode != 0) && (exceptionCode != fun_ExceptionCode_0xb2))
    {//�쳣
        ModbusReponse.Exception.data          = exceptionCode;
        ModbusReponse.Exception.FunctionCode += 0x80;
        CRC = CRC16_Modbus((int8u *)&ModbusReponse.Exception, 3);
        ModbusReponse.Exception.CRCH = (CRC >> 8) & 0xff;    //У��H
        ModbusReponse.Exception.CRCL = CRC & 0xff;           //У��L

        Modbus_repLen = 5;
        BASSysCommFlag.CommErr = TRUE;
        
        backupBASSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
    }   

    if(exceptionCode != fun_ExceptionCode_0xb2)
    {
        /* ������֡ ��������
         * ���淢�͵�����
         *  */
        BASSysCommFlag.TxData = 1;
    }
}

/* 
 * =============================================================================
 * PSC ���Ʒ緧�Ĵ���
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
    
    /* ��λ��ȴ��ӳ�ʱ�� */
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
            /* ��� BAS ���Ʒ緧�ı�־ */
            pWVSts->bBASOpnCmd_sts = 0;
            pWVSts->bBASClsCmd_sts = 0;

            /* �������緧 */
            if(pWVSts->bPSCOpnCmd_sts)
            {
                WVCmd = 1;
                tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_EXCLUDE | CMD_BIT_SET | OP_DCUCMD_1 | OPCMD_VALID;
                tmpBroadCmd.oprCmd2.byte = 0;
            }
        }

        if(bPSC_ClsWV_Bak != (int8u)pWVSts->bPSCClsCmd_sts)
        {
            /* ��� BAS ���Ʒ緧�ı�־ */
            pWVSts->bBASOpnCmd_sts = 0;
            pWVSts->bBASClsCmd_sts = 0;
            
            /* �����ط緧 */
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
        /* �жϣ��� PSC �緧�������� �л����Զ�
         * ���������������� DCU */
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
    
    /* PSC ���Ϳ��Ʒ緧����
     * Ϊ�˱�����ť�����з������ ���͹� ����
     * �����ȶ����� */
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

                /* ��ӷ�������� DCU �� ���緧��ط緧 */
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
 * �� BASϵͳ ͨ�Ŵ���
 * ��Ҫ����������ѭ������
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
        
        /* ���»ظ� BASϵͳ �ļĴ��� */
        GetReplyBASMessage(NULL);
        
        /* BAS ϵͳ�ӿڵ��Բ������ݵ�ʱ�� */
        decTestBASRegTime(200);
        
        /* �Ƿ� PSC ���Ʒ緧 */
        processPSCOprWindValve();
    }

    
    /* ������ͨ��ʱ���ж�BASϵͳ��������֡�Ƿ����
     *  */
    chkBASSysFinishSend();
    
    /* �ж��Ƿ����յ�����֡��������
     *  */
    procBASSysRxFrame(BASSysApp_ProcFrame);
    
    /* �ж��Ƿ��д����͵�����֡��������
     *  */
    procBASSysTxFrame();
    
    /* �ж�BASϵͳ�Ƿ�����
     * 
     * BASϵͳ��ʱ ��վ̨��ϵͳ����һ�������ģ�ͬʱվ̨�ŷ�����Ϣ�� 
     * ��վ̨��ϵͳ����һ��ʱ�� ��BASSys_OFFLIN_TIME��û���յ�BASϵͳ����ѯָ����ж�Ϊͨ�ų�ʱ��
     */
    if(uartChkLinkAndSendSts(COM_BASSYS) == TRUE)
    {
        tWindValveSysSts * pWVSts = getWindValveSysSts();
        pWVSts->bBASOpnCmd_sts = 0;
        pWVSts->bBASClsCmd_sts = 0;
        
        #if 0
        /* BAS ����
         * �� DCU ���������� ���緧���ط緧 
         */
        tBroadCmdPkg tmpBroadCmd;

        memset((int8u*)tmpBroadCmd.broadCmdBuf, 0xff, sizeof(tmpBroadCmd.broadCmdBuf));
        tmpBroadCmd.oprCmd3.byte = 0;
        /* ���: ���緧 */
        tmpBroadCmd.oprCmd1.byte  =  DCUCMD1_OPN | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
        /* ���: �ط緧 */
        tmpBroadCmd.oprCmd2.byte  =  DCUCMD1_CLS | OP_BIT_NORMAL | CMD_BIT_CLR | OP_DCUCMD_1 | OPCMD_VALID;
        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_CTRL_WIND);
        #endif
    }

}


/* =============================================================================
 * ������BASϵͳ�����ݣ���������
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

/* �ظ�BASϵͳ�����ݣ�
 *   - ���͸�BASϵͳ
 *   - ���͸�PC������
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




