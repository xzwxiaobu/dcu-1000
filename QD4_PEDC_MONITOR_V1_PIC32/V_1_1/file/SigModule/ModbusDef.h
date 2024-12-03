#ifndef _MODBUS_DEF_H_
#define _MODBUS_DEF_H_

extern  PUBLIC int8u getPedcID(void);
#define LOCAL_DEV_ADDR          getPedcID()         /* �����豸��ַ, �ò��뿪�� */
#define MODBUS_MAX_BUFSIZE      (64)

//���ֳ����ڲ�ͬ��Ŀ����Ҫ����Э�鶨��
//���ļ������� �Ͼ�7


//function codes descriptions
#define fun_01_ReadCoils                    1
#define fun_02_ReadDiscreteInputs           2
#define fun_03_ReadHoldingRegisters         3
#define fun_04_ReadinputRegisters           4
#define fun_05_WriteSingleCoil              5
#define fun_06_WriteSingleRegister          6
#define fun_07_ReadExceptionStatus          7
#define fun_11_GetCommEventCounter          11
#define fun_12_GetCommEventLog              12
#define fun_15_WriteMultipleCoils           15
#define fun_16_WriteMultipleRegisters       16

#define mb_fun0x3_request_fix_len           8           //������ 0x3,ѯ��ָ��̶����� 
#define mb_fun0x3_response_fix_len          5           //������ 0x3,Ӧ��Ĺ̶����ȣ�AdditionlAddr + FunctionCode + ByteCount + CRC��
#define mb_fun0x3_response_ByteCount_max    (1 * 2)     //������ 0x3,Ӧ���buf����ֽ�������ͬ��Ŀ��Ҫ��������
#define fun03_Write_StartingAddress         0           //Write Starting Address
    
    
#define mb_fun0x4_request_fix_len           8           //������ 0x4,ѯ��ָ��̶����� 
#define mb_fun0x4_response_fix_len          5           //������ 0x4,Ӧ��Ĺ̶����ȣ�AdditionlAddr + FunctionCode + ByteCount + CRC��
#define mb_fun0x4_response_ByteCount_max    (1 * 2)     //������ 0x4,Ӧ���buf����ֽ�������ͬ��Ŀ��Ҫ��������
#define fun04_Write_StartingAddress         0           //������ 0x4,starting address
    
    
#define mb_fun0xf_reqfix_len                9           //������ 0xf,ѯ��ָ��̶����� ��������������ĳ��ȣ�          
#define mb_fun0xf_dataLen                   6           //������ 0xf ���ݳ���
    
    
#define mb_fun0x10_request_fix_len          9           //������ 0x10,ѯ��ָ��̶����� ��������������ĳ��ȣ�          
#define mb_fun0x10_request_ByteCount_max    (2 * 1)     //������ 0x10,ѯ��������ݳ��ȣ���ͬ��Ŀ��Ҫ��������
#define mb_fun0x10_request_dataBUF_LEN      (mb_fun0x10_request_ByteCount_max + 20) //������ 0x10,����������
#define mb_fun0x10_response_fix_len         8           //������ 0x10,Ӧ��ָ��̶�����
#define mb_fun0x10_StartingAddress          0           //������ 0x10,starting address


#define fun03_ExceptionCode_01              1           //Function code not supported
#define fun03_ExceptionCode_02              2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun03_ExceptionCode_03              3           //!(0x0001 =< Quantity of input <= 0x07d0)
#define fun03_ExceptionCode_04              4           //Read Discrete Input != ok, ��ע��DCUȫ�����ߣ������04��

#define fun04_ExceptionCode_01              1           //Function code not supported
#define fun04_ExceptionCode_02              2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun04_ExceptionCode_03              3           //!(0x0001 =< Quantity of input <= 0x07d0)
#define fun04_ExceptionCode_04              4           //Read Discrete Input != ok, ��ע��DCUȫ�����ߣ������04��

#define fun0x10_ExceptionCode_01            1           //Function code not supported
#define fun0x10_ExceptionCode_02            2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun0x10_ExceptionCode_03            3           //!(0x0001 =< Quantity of input <= 0x07b) ; Byte count != Quantity * 2
#define fun0x10_ExceptionCode_04            4           //Write Multiple Outputs != ok

#define fun_ExceptionCode_0xb2              0xb2        //�Ƿ��ִ�������쳣����MODBUS��׼�������ڵ��ԣ����ܷ��͸��Է�

#define functoinCodeNotSupported            1         

//exception code          
typedef enum
{
    EXCEPTION_CODE_01 = 1,      /* �Ƿ�����        */
    EXCEPTION_CODE_02    ,      /* �Ƿ����ݵ�ַ    */
    EXCEPTION_CODE_03    ,      /* �Ƿ�����ֵ      */
    EXCEPTION_CODE_04    ,      /* ��վ���� */     
    EXCEPTION_CODE_05    ,      /* ȷ��     */     
    EXCEPTION_CODE_07 = 7,      /* �豸æ   */     
    EXCEPTION_CODE_08    ,	/* �洢��ż�Բ�� */
}tEnmMobusExceptionCode;


/* MODBUS RTU ÿ֡һ��������̶���ʽ��Э��ͷ
 *  */
typedef struct
{
    int8u devAddr;              //0 �ӻ���ַλ��          
    int8u funID;                //1 ������λ��
    int8u regAddrH;             //2 �Ĵ�����ַ
    int8u regAddrL;             //3
    int8u regNumH;              //4 �Ĵ�������
    int8u regNumL;              //5
}tModbusFixHeader;

typedef struct
{
    tModbusFixHeader    header; //�̶���ʽ��Э��ͷ
}tModbusRdHeader;

typedef struct
{
    tModbusFixHeader   header;  //�̶���ʽ��Э��ͷ
    int8u dataLen;              //�ֽ���             
}tModbusWrHeader;

typedef struct
{
    int8u crcL;                 //CRC У����ֽ�
    int8u crcH;                 //CRC У����ֽ�
}tModbusTail;



typedef union
{
	struct
	{                                                 //������
            int8u  AdditionalAddr;                                 //0 ��վ��ַ
            int8u  FunctionCode;                                   //1 ������
	}FunAll;

	struct
	{
            int8u  AdditionalAddr;                                 //0 ��վ��ַ
            int8u  FunctionCode;                                   //1 ������
            int8u  StartingAddressH;                               //2 ��ʼ��ַ��λ  
            int8u  StartingAddressL;                               //3 ��ʼ��ַ��λ
            int8u  QuantityH;                                      //4 �Ĵ�������λ
            int8u  QuantityL;                                      //5 �Ĵ�������λ
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x03Request;
	
	struct
	{
            int8u  AdditionalAddr;                                 //0 ��վ��ַ
            int8u  FunctionCode;                                   //1 ������
            int8u  StartingAddressH;                               //2 ��ʼ��ַ��λ  
            int8u  StartingAddressL;                               //3 ��ʼ��ַ��λ
            int8u  QuantityH;                                      //4 �Ĵ�������λ
            int8u  QuantityL;                                      //5 �Ĵ�������λ
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x04Request;
	
	struct
	{
            int8u  AdditionalAddr;                                  //0 ��վ��ַ
            int8u  FunctionCode;                                    //1 ������
            int8u  StartingAddressH;                                //2 ��ʼ��ַ��λ  
            int8u  StartingAddressL;                                //3 ��ʼ��ַ��λ
            int8u  QuantityH;                                       //4 �Ĵ�������λ
            int8u  QuantityL;                                       //5 �Ĵ�������λ
            int8u  ByteCount;                                       //6 �ֽ��� 
            int8u  buf[mb_fun0x10_request_ByteCount_max];           //7 Input Registers    
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x10Request;
	
	int8u  reqDataBuf[MODBUS_MAX_BUFSIZE];
}tModbusRequest;

//--------------------------------------
typedef union
{
	struct
	{                                                 //������
            int8u  AdditionalAddr;                                 //0 ��վ��ַ
            int8u  FunctionCode;                                   //1 ������
	}FunAll;

	struct
	{
            int8u  AdditionalAddr;                                  //0 ��վ��ַ
            int8u  FunctionCode;                                    //1 ������
            int8u  data;
            int8u  CRCL;
            int8u  CRCH;
	} Exception;

	struct
	{
            int8u  AdditionalAddr;                                  //0 ��վ��ַ
            int8u  FunctionCode;                                    //1 ������
            int8u  ByteCount;                                       //2 �ֽ��� 
            int8u  buf[mb_fun0x3_response_ByteCount_max];           //3 ����
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x03Response;

	struct
	{
            int8u  AdditionalAddr;                                  //0 ��վ��ַ
            int8u  FunctionCode;                                    //1 ������
            int8u  ByteCount;                                       //2 �ֽ��� 
            int8u  buf[mb_fun0x4_response_ByteCount_max];           //3 ����
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x04Response;
	
	struct
	{
            int8u  AdditionalAddr;                      //0 ��վ��ַ
            int8u  FunctionCode;                        //1 ������
            int8u  StartingAddressH;                    //2 ��ʼ��ַ��λ  
            int8u  StartingAddressL;                    //3 ��ʼ��ַ��λ
            int8u  QuantityH;                           //4 �Ĵ�������λ
            int8u  QuantityL;                           //5 �Ĵ�������λ
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x10Response;
        
	int8u  resDataBuf[MODBUS_MAX_BUFSIZE];
}tModbusReponse;






#endif  //_MODBUS_DEF_H_

