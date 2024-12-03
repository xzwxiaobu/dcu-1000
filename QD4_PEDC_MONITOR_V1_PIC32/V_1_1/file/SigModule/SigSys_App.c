
#include "..\Include.h"


#include "ModbusDef.h"
#include "SigSys_Uart.h"
#include "SigSys_Reg.h"
#include "SigSys_App.h"


#undef  PRIVATE 
#define PRIVATE 

/* �Ƚ�ǰ���������������ͬ�������������ͣ�������ͬ�������� CREPEATSENDTIME ʱ���ڶ�η��� */
#define CREPEATSENDTIME     5           //seconds

typedef struct
{
    int8u TxData            : 1;      /* 1 ��ʾ��������Ҫ����             */
    int8u CommErr           : 1;      /* 1 ��ʾ���ź�ϵͳͨ���쳣         */
}tSigSysCommFlag;

PRIVATE tSigSysCommFlag sigSysCommFlag;

/* MODBUS ͨ�Ŵ洢�� */
PRIVATE tModbusRequest  ModbusRequest;
PRIVATE tModbusReponse  ModbusReponse;
PRIVATE int8u Modbus_repLen;

/* ���� PEDC �� �ź�ϵͳ��ͨѶ��Ϣ */
PRIVATE int bakSigSysRxSize;
PRIVATE int8u bakSigSysRx[UART_SIGSYS_RXBUFSIZE];

PRIVATE int bakSigSysTxSize;
PRIVATE int8u bakSigSysTx[UART_SIGSYS_TXBUFSIZE];

PRIVATE void backupSigSys_Rx(int8u *pData, int8u size);
PRIVATE void backupSigSys_Tx(int8u *pData, int8u size);

/* ���ݽ��յ�д�Ĵ�������
 * �����ж��ź�ϵͳ����������ͬ������
 *  */
PRIVATE int8u a_byBak_Fun0x10_content[BROAD_CMD_NUM][mb_fun0x10_request_dataBUF_LEN+ 2];

/* =============================================================================
 * �ṩ�ӿڹ��ⲿ����
 */

/* ���յ��ź�ϵͳ��Ϣ���� */
PUBLIC int8u * getRxFromSigSys(int8u * len)
{
    *len = bakSigSysRxSize;
    return bakSigSysRx;
}

/* ���͸��ź�ϵͳ����Ϣ���� */
PUBLIC int8u * getTxToSigSys(int8u * len)
{
    *len = bakSigSysTxSize;
    return bakSigSysTx;
}

/* =============================================================================
 * �������������֡
 *  
 * ����
 *  TRUE ��ʾ��������û�з���
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
 * ������յ�����֡(��ͨ��CRCУ��)
 *  pFrame      : ����ָ֡�루MODBUSЭ��֡��
 *  FrameLen    : ֡����
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
        /* FUN 04 ������Ĵ��� 
         * Req : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2
         * Res : ��Ԫ��ʶ1 + ������1 + �ֽڸ���(=N*2) + ���������N*2
         */
        case fun_04_ReadinputRegisters:
            Quantity = (ModbusRequest.Fun0x04Request.QuantityH << 8) + ModbusRequest.Fun0x04Request.QuantityL;
            StartingAddress = (ModbusRequest.Fun0x04Request.StartingAddressH << 8) + ModbusRequest.Fun0x04Request.StartingAddressL;

            if(FrameLen < mb_fun0x4_request_fix_len)
            {
                exceptionCode = fun_ExceptionCode_0xb2;   //�Ƿ��ִ�
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
            { //��ȷӦ��RTU֡:��վ��ַ  ������  �ֽڼ���    ����    У���CRCH  У���CRCL
                //temp = Quantity * 16;
                backupSigSys_Rx(&ModbusRequest.Fun0x04Request.AdditionalAddr, FrameLen);

                ModbusReponse.Fun0x04Response.ByteCount = Quantity * 2;
                Modbus_repLen      = ModbusReponse.Fun0x04Response.ByteCount + mb_fun0x4_response_fix_len;

                /* ׼���������� */
                GetMODBUSMessage(temp_buf);

                for(i = 0; i < ModbusReponse.Fun0x04Response.ByteCount; i ++)
                {
                    ModbusReponse.Fun0x04Response.buf[i] = temp_buf[StartingAddress*2 + i];
                }

                CRC = CRC16_Dn_Cal(&ModbusReponse.Fun0x04Response.AdditionalAddr, ModbusReponse.Fun0x04Response.ByteCount + 3, 0xffff);
                ModbusReponse.Fun0x04Response.buf[ModbusReponse.Fun0x04Response.ByteCount + 1]     = (CRC >> 8) & 0xff ;   //У��H
                ModbusReponse.Fun0x04Response.buf[ModbusReponse.Fun0x04Response.ByteCount + 0]     = CRC & 0xff;           //У��L

                /* send data to sigsys */
                backupSigSys_Tx(&ModbusReponse.Fun0x04Response.AdditionalAddr,   Modbus_repLen);
            }
            break;
                
                
            /* FUN 0x10 д����Ĵ��� 
             * Req : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2 + �ֽ���1 + �Ĵ���ֵ(����=2*N)
             * Res : ��Ԫ��ʶ1 + ������1 + ��ʼ��ַ2 + �Ĵ�������(N)2
             */
            case fun_16_WriteMultipleRegisters:
                bytecount = ModbusRequest.Fun0x10Request.ByteCount;
                Quantity = (ModbusRequest.Fun0x10Request.QuantityH << 8) + ModbusRequest.Fun0x10Request.QuantityL;
                StartingAddress = (ModbusRequest.Fun0x10Request.StartingAddressH << 8) + ModbusRequest.Fun0x10Request.StartingAddressL;

                /* �ź�ϵͳ��Ӧд 2-7 �żĴ��� 
                 * Ϊ�˰�ȫ�������֧���ź�ϵͳֻд���ּĴ��������ö�λ���룩
                 * ÿ�α��뽫�����Ż��Ķ�λ������ȫ����
                 */
                
                if(FrameLen < (bytecount    + mb_fun0x10_request_fix_len))
                {
                    exceptionCode = fun_ExceptionCode_0xb2;   //�Ƿ��ִ�
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
                    {/* �Ƚ�ǰ���������������ͬ�������������ͣ�������ͬ�������� CREPEATSENDTIME ʱ���ڶ�η��� */
                        if(a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] != ModbusRequest.Fun0x10Request.buf[i])
                        {
                            broadCmdList[BROAD_CMD_STC_INHIBIT].uRepeatSendTime = CREPEATSENDTIME;   /*Ŀ�ģ��յ������������*/
                            break;
                        }
                    }
                    
                    for(i = 0; i < bytecount; i ++)
                    {
                        a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] = ModbusRequest.Fun0x10Request.buf[i];
                    }
                    
                    exceptionCode = 1;                              /* ����ָ��ʱ�����ظ����� */

                    //���û�����������ж���䣬���ÿ�ζ���������
                    if(broadCmdList[BROAD_CMD_STC_INHIBIT].uRepeatSendTime >= CREPEATSENDTIME)   /*��������*/
                    {
                        exceptionCode = 0;
                        temp = mb_fun0x10_request_ByteCount_max;
                        p = (int8u *)&ModbusRequest.Fun0x10Request.buf[0];
                        i = 0;

                        for(;;)
                        {//16λ���飬ת��8λ���飻 ��1H ��1L ��2H ��2L ... ---> ��1L ��1H ��2L ��2H...
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
                            tmpBroadCmd.broadCmdBuf[i]                      = buf[i];  //����
                        }

                        tmpBroadCmd.oprCmd1.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | DCUCMD1_STC_INHIBIT ;
                        tmpBroadCmd.oprCmd2.byte = 0;
                        tmpBroadCmd.oprCmd3.byte = 0;
                        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);
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
                    
                    /* send data to sigsys */
                    backupSigSys_Tx(&ModbusReponse.Fun0x10Response.AdditionalAddr, Modbus_repLen);
                }
                break;  
            
            default:
                
                exceptionCode = functoinCodeNotSupported;  //��֧�ָù����� 
                break;
        }
        
    sigSysCommFlag.CommErr = FALSE;
    if((exceptionCode != 0) && (exceptionCode != fun_ExceptionCode_0xb2))
    {//�쳣
        ModbusReponse.Exception.data          = exceptionCode;
        ModbusReponse.Exception.FunctionCode += 0x80;
        CRC = CRC16_Dn_Cal((int8u *)&ModbusReponse.Exception, 3, 0xffff);
        ModbusReponse.Exception.CRCH = (CRC >> 8) & 0xff;    //У��H
        ModbusReponse.Exception.CRCL = CRC & 0xff;           //У��L

        Modbus_repLen = 5;
        sigSysCommFlag.CommErr = TRUE;
        
        /* send data to sigsys */
        backupSigSys_Tx((int8u *) &ModbusReponse, Modbus_repLen);
    }   

    if(exceptionCode != fun_ExceptionCode_0xb2)
    {
        /* ������֡ ��������
         * ���淢�͵�����
         *  */
        sigSysCommFlag.TxData = 1;
    }
}

/* 
 * =============================================================================
 * �� �ź�ϵͳ ͨ�Ŵ���
 * ��Ҫ����������ѭ������
 * =============================================================================
 */
PUBLIC void processSigSysComm(void)
{
    /* ������ͨ��ʱ���ж��ź�ϵͳ��������֡�Ƿ����
     *  */
    chkSigSysFinishSend();
    
    /* �ж��Ƿ����յ�����֡��������
     *  */
    procSigSysRxFrame(SigSysApp_ProcFrame);
    
    /* �ж��Ƿ��д����͵�����֡��������
     *  */
    procSigSysTxFrame();

//    /* �ϴ���MMS��վ̨�Ŷ�λ�����г���
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
    
    /* �ж��ź�ϵͳ�Ƿ�����
     * 
     * �ź�ϵͳ��ʱ ��վ̨��ϵͳ����һ�������ģ�ͬʱվ̨�ŷ�����Ϣ�� 
     * ��վ̨��ϵͳ����һ��ʱ�� ��SIGSYS_OFFLIN_TIME��û���յ��ź�ϵͳ����ѯָ����ж�Ϊͨ�ų�ʱ��
     * ��ʱվ̨��ϵͳ�Զ���λ���Ŷ�λ����վ̨�ű�־���ڴ�����£�վ̨��ϵͳ�����ź�ϵͳӲ�߽ӿ�ָ��ִ�С�
     */
    if(uartChkLinkAndSendSts(COM_SIGSYS1) == TRUE)
    {
        /* �ź�ϵͳ����
         * �� DCU ���͹㲥������ָ���ı�־
         */
        int i;
        tBroadCmdPkg tmpBroadCmd;

        for(i = 0; i < BROADCOMMAND_LEN; i ++)
        {
            uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[i]  = 0;
            tmpBroadCmd.broadCmdBuf[i]                      = 0;  //����
        }

        /* �����������DCU�� STC inhibit */
        tmpBroadCmd.oprCmd1.byte = OPCMD_VALID | OP_DCUCMD_1 | OP_BIT_NORMAL | CMD_BIT_CPY | DCUCMD1_STC_INHIBIT ;
        tmpBroadCmd.oprCmd2.byte = 0 ;
        tmpBroadCmd.oprCmd3.byte = 0 ;
        addBroadCmdTask(&tmpBroadCmd, BROAD_CMD_STC_INHIBIT);

        /* �屸������ */
        for(i = 0; i < sizeof(a_byBak_Fun0x10_content[0]); i ++)
        {
            a_byBak_Fun0x10_content[BROAD_CMD_STC_INHIBIT][i] = 0;
        }
    }
}


/* =============================================================================
 * �������ź�ϵͳ�����ݣ���������
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

/* �ظ��ź�ϵͳ�����ݣ�
 *   - ���͸��ź�ϵͳ
 *   - ���͸�PC������
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
