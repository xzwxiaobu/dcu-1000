#ifndef _MODBUS_DEF_H_
#define _MODBUS_DEF_H_

extern  PUBLIC int8u getPedcID(void);
#define LOCAL_DEV_ADDR          getPedcID()         /* 本机设备地址, 用拨码开关 */
#define MODBUS_MAX_BUFSIZE      (64)

//部分常量在不同项目中需要根据协议定义
//本文件适用于 南京7


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

#define mb_fun0x3_request_fix_len           8           //功能码 0x3,询问指令固定长度 
#define mb_fun0x3_response_fix_len          5           //功能码 0x3,应答的固定长度（AdditionlAddr + FunctionCode + ByteCount + CRC）
#define mb_fun0x3_response_ByteCount_max    (1 * 2)     //功能码 0x3,应答的buf最大字节数，不同项目需要独立定义
#define fun03_Write_StartingAddress         0           //Write Starting Address
    
    
#define mb_fun0x4_request_fix_len           8           //功能码 0x4,询问指令固定长度 
#define mb_fun0x4_response_fix_len          5           //功能码 0x4,应答的固定长度（AdditionlAddr + FunctionCode + ByteCount + CRC）
#define mb_fun0x4_response_ByteCount_max    (1 * 2)     //功能码 0x4,应答的buf最大字节数，不同项目需要独立定义
#define fun04_Write_StartingAddress         0           //功能码 0x4,starting address
    
    
#define mb_fun0xf_reqfix_len                9           //功能码 0xf,询问指令固定长度 （除了数据以外的长度）          
#define mb_fun0xf_dataLen                   6           //功能码 0xf 数据长度
    
    
#define mb_fun0x10_request_fix_len          9           //功能码 0x10,询问指令固定长度 （除了数据以外的长度）          
#define mb_fun0x10_request_ByteCount_max    (2 * 1)     //功能码 0x10,询问最大数据长度，不同项目需要独立定义
#define mb_fun0x10_request_dataBUF_LEN      (mb_fun0x10_request_ByteCount_max + 20) //功能码 0x10,数据区长度
#define mb_fun0x10_response_fix_len         8           //功能码 0x10,应答指令固定长度
#define mb_fun0x10_StartingAddress          0           //功能码 0x10,starting address


#define fun03_ExceptionCode_01              1           //Function code not supported
#define fun03_ExceptionCode_02              2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun03_ExceptionCode_03              3           //!(0x0001 =< Quantity of input <= 0x07d0)
#define fun03_ExceptionCode_04              4           //Read Discrete Input != ok, （注：DCU全部离线，则输出04）

#define fun04_ExceptionCode_01              1           //Function code not supported
#define fun04_ExceptionCode_02              2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun04_ExceptionCode_03              3           //!(0x0001 =< Quantity of input <= 0x07d0)
#define fun04_ExceptionCode_04              4           //Read Discrete Input != ok, （注：DCU全部离线，则输出04）

#define fun0x10_ExceptionCode_01            1           //Function code not supported
#define fun0x10_ExceptionCode_02            2           //starting Address != ok; or starting Address + Quantity of input !=ok
#define fun0x10_ExceptionCode_03            3           //!(0x0001 =< Quantity of input <= 0x07b) ; Byte count != Quantity * 2
#define fun0x10_ExceptionCode_04            4           //Write Multiple Outputs != ok

#define fun_ExceptionCode_0xb2              0xb2        //非法字串，这个异常不是MODBUS标准，仅用于调试，不能发送给对方

#define functoinCodeNotSupported            1         

//exception code          
typedef enum
{
    EXCEPTION_CODE_01 = 1,      /* 非法功能        */
    EXCEPTION_CODE_02    ,      /* 非法数据地址    */
    EXCEPTION_CODE_03    ,      /* 非法数据值      */
    EXCEPTION_CODE_04    ,      /* 从站故障 */     
    EXCEPTION_CODE_05    ,      /* 确认     */     
    EXCEPTION_CODE_07 = 7,      /* 设备忙   */     
    EXCEPTION_CODE_08    ,	/* 存储奇偶性差错 */
}tEnmMobusExceptionCode;


/* MODBUS RTU 每帧一定有这个固定格式的协议头
 *  */
typedef struct
{
    int8u devAddr;              //0 从机地址位置          
    int8u funID;                //1 功能码位置
    int8u regAddrH;             //2 寄存器地址
    int8u regAddrL;             //3
    int8u regNumH;              //4 寄存器数量
    int8u regNumL;              //5
}tModbusFixHeader;

typedef struct
{
    tModbusFixHeader    header; //固定格式的协议头
}tModbusRdHeader;

typedef struct
{
    tModbusFixHeader   header;  //固定格式的协议头
    int8u dataLen;              //字节数             
}tModbusWrHeader;

typedef struct
{
    int8u crcL;                 //CRC 校验低字节
    int8u crcH;                 //CRC 校验高字节
}tModbusTail;



typedef union
{
	struct
	{                                                 //公共的
            int8u  AdditionalAddr;                                 //0 从站地址
            int8u  FunctionCode;                                   //1 功能码
	}FunAll;

	struct
	{
            int8u  AdditionalAddr;                                 //0 从站地址
            int8u  FunctionCode;                                   //1 功能码
            int8u  StartingAddressH;                               //2 开始地址高位  
            int8u  StartingAddressL;                               //3 开始地址低位
            int8u  QuantityH;                                      //4 寄存器数高位
            int8u  QuantityL;                                      //5 寄存器数低位
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x03Request;
	
	struct
	{
            int8u  AdditionalAddr;                                 //0 从站地址
            int8u  FunctionCode;                                   //1 功能码
            int8u  StartingAddressH;                               //2 开始地址高位  
            int8u  StartingAddressL;                               //3 开始地址低位
            int8u  QuantityH;                                      //4 寄存器数高位
            int8u  QuantityL;                                      //5 寄存器数低位
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x04Request;
	
	struct
	{
            int8u  AdditionalAddr;                                  //0 从站地址
            int8u  FunctionCode;                                    //1 功能码
            int8u  StartingAddressH;                                //2 开始地址高位  
            int8u  StartingAddressL;                                //3 开始地址低位
            int8u  QuantityH;                                       //4 寄存器数高位
            int8u  QuantityL;                                       //5 寄存器数低位
            int8u  ByteCount;                                       //6 字节数 
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
	{                                                 //公共的
            int8u  AdditionalAddr;                                 //0 从站地址
            int8u  FunctionCode;                                   //1 功能码
	}FunAll;

	struct
	{
            int8u  AdditionalAddr;                                  //0 从站地址
            int8u  FunctionCode;                                    //1 功能码
            int8u  data;
            int8u  CRCL;
            int8u  CRCH;
	} Exception;

	struct
	{
            int8u  AdditionalAddr;                                  //0 从站地址
            int8u  FunctionCode;                                    //1 功能码
            int8u  ByteCount;                                       //2 字节数 
            int8u  buf[mb_fun0x3_response_ByteCount_max];           //3 数据
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x03Response;

	struct
	{
            int8u  AdditionalAddr;                                  //0 从站地址
            int8u  FunctionCode;                                    //1 功能码
            int8u  ByteCount;                                       //2 字节数 
            int8u  buf[mb_fun0x4_response_ByteCount_max];           //3 数据
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x04Response;
	
	struct
	{
            int8u  AdditionalAddr;                      //0 从站地址
            int8u  FunctionCode;                        //1 功能码
            int8u  StartingAddressH;                    //2 开始地址高位  
            int8u  StartingAddressL;                    //3 开始地址低位
            int8u  QuantityH;                           //4 寄存器数高位
            int8u  QuantityL;                           //5 寄存器数低位
            int8u  CRCL;
            int8u  CRCH;  
	} Fun0x10Response;
        
	int8u  resDataBuf[MODBUS_MAX_BUFSIZE];
}tModbusReponse;






#endif  //_MODBUS_DEF_H_

