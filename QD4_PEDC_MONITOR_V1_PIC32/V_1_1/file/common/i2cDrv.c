
//#include "..\include.h"
//#include "Define.h"

#include <plib.h>

#include "_cpu.h"
#include "i2cDrv.h"

union 
{
    struct
    {
        int8u bI2C_InputErr  : 1;
        int8u bI2C_OutputErr : 1;
        int8u    : 6; 
    }bits;
    
    int8u byte;
}I2C_ErrFLAG;

/* ��¼I2C������Ϣ */
union 
{
    struct
    {
        int8u bI2C_ERR_INPUT_Start : 1;
        int8u bI2C_ERR_INPUT_WriteSlaveDeviceAddressW : 1;
        int8u bI2C_ERR_INPUT_WriteSlaveCommand : 1;
        int8u bI2C_ERR_INPUT_RepeatStart : 1;
        int8u bI2C_ERR_INPUT_WriteSlaveDeviceAddressR : 1;
        int8u bI2C_ERR_INPUT_ReadByte : 1;
        int8u bI2C_ERR_INPUT_Noack : 1;
        int8u bI2C_ERR_INPUT_Stop : 1;
    }bits;
    
    int8u byte;
}I2C_INPUT_Err;

union 
{
    struct
    {
        int8u bI2C_ERR_OUTPUT_Start                 : 1;
        int8u bI2C_ERR_OUTPUT_SlaveDeviceAddressW   : 1;
        int8u bI2C_ERR_OUTPUT_SlaveCommand          : 1;
        int8u bI2C_ERR_OUTPUT_WriteByte             : 1;
        int8u bI2C_ERR_OUTPUT_Stop                  : 1;
        int8u B5 : 1;
        int8u B6 : 1;
        int8u B7 : 1;
    }bits;
    
    int8u byte;
}I2C_OUTPUT_Err;


/* ========================================================= */
//#define CLR_I2C_MIF(id)     INTClearFlag((id - I2C1) + INT_I2C1M)         /* CLR _IFS0_I2C1MIF */
//#define GET_I2C_MIF(id)     INTGetFlag  ((id - I2C1) + INT_I2C1M)

PRIVATE BOOL CLR_I2C_MIF(I2C_MODULE id)
{
    if(id == I2C1)      {   INTClearFlag(INT_I2C1M);}
    else if(id == I2C3) {   INTClearFlag(INT_I2C3M);}
    else if(id == I2C4) {   INTClearFlag(INT_I2C4M);}
    else if(id == I2C5) {   INTClearFlag(INT_I2C5M);}
    else                {   return FALSE;           }
}

PRIVATE int GET_I2C_MIF(I2C_MODULE id)
{
    if(id == I2C1)      {   INTGetFlag(INT_I2C1M);  }
    else if(id == I2C3) {   INTGetFlag(INT_I2C3M);  }
    else if(id == I2C4) {   INTGetFlag(INT_I2C4M);  }
    else if(id == I2C5) {   INTGetFlag(INT_I2C5M);  }
    else                {   return 0;               }
}

PRIVATE void delay_nop(int8u  time)
{
	int8u i;
	for(i = 0; i < time; i ++)
	{
		Nop();
	}
}

/* =========================================================
 * ����ǰ I2C ģ�����״̬ 
 *
 */
PUBLIC int8u getI2CErrFlag(void)
{
    return I2C_ErrFLAG.byte;
}

/* �����ǰ I2C ģ�����״̬ 
 *
 */
PUBLIC void clrI2CErrFlag(void)
{
    I2C_ErrFLAG.byte = 0;
}


/* ================   �ײ���������  ==================== */

/* I2C �ȴ��жϱ�־
 * ����
 *     id       : I2Cģ��ID
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_Wait_IntF(I2C_MODULE id)
{
	int16u i = 0;
   	while(! GET_I2C_MIF(id) )    /* Wait for ninth clock cycle */
   	{
   		i ++;
  		if(i > 50000)
  			return FALSE;
   	}
   	
	CLR_I2C_MIF(id);            /* Clear interrupt flag */

	return TRUE;

}


/* I2C : ����λ
 * ����
 *     id       : I2Cģ��ID
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_start(I2C_MODULE id)
{
	I2C_REGISTERS * const i2cRegisters = i2cBase[id];
	i2cRegisters->I2CxSTAT = 0;									     //I2CxSTAT = 0
    
    if(id == I2C1)      {   INTClearFlag(INT_I2C1); }
    else if(id == I2C3) {   INTClearFlag(INT_I2C3); }
    else if(id == I2C4) {   INTClearFlag(INT_I2C4); }
    else if(id == I2C5) {   INTClearFlag(INT_I2C5); }
    else                {   return FALSE;           }
    
	/* ��������һ�䣬id = I2C3 ������
     * INTClearFlag((id - I2C1) + INT_I2C1);            //CLR _IFS0_I2C1BIF | _IFS0_I2C1SIF | _IFS0_I2C1MIF
     * */

	CLR_I2C_MIF(id);

	I2CStart(id);

	return i2c_Wait_IntF(id);

}

/* I2C : ֹͣλ
 * ����
 *     id       : I2Cģ��ID
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_stop(I2C_MODULE id)
{
	CLR_I2C_MIF(id);

   	I2CStop(id);             	                     //Terminate communication protocol with stop signal

	return i2c_Wait_IntF(id);

}


/* I2C : �ظ�����
 * ����
 *     id       : I2Cģ��ID
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_repeat_start(I2C_MODULE id)             
{
	CLR_I2C_MIF(id);

	I2CRepeatStart(id);                             //Restart signal

	return i2c_Wait_IntF(id);
}


/* I2C : send NACK
 * ����
 *     id       : I2Cģ��ID
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_noack(I2C_MODULE id)
{
	CLR_I2C_MIF(id);

	I2CAcknowledgeByte(id, FALSE);               

	return i2c_Wait_IntF(id);
}

/* I2C : send ACK or NACK
 * ����
 *     id       : I2Cģ��ID
 *     ack      : TRUE=ACK , FALSE=NACK
 * ����
 *     TRUE     : OK
 *     FALSE    : ��ʱ��(ʱ�� ��)
 */
PRIVATE int8u i2c_ack(I2C_MODULE id, BOOL ack)
{
	CLR_I2C_MIF(id);

    /* If TRUE, positively acknowledges (ACK) the byte of data received */
    /* If FALSE, negatively acknowledges (NAK) the byte of data received */
	I2CAcknowledgeByte(id, ack);               

	return i2c_Wait_IntF(id);
}

/* I2C : �����ֽ�
 * ����
 *     id       : I2Cģ��ID
 *     data     : ����������
 * ����
 *     TRUE     : OK
 *     FALSE    : Fail
 */
PRIVATE int8u i2c_write_byte(I2C_MODULE id, int8u data)
{
	CLR_I2C_MIF(id);
	
	if(I2CSendByte(id, data) == I2C_MASTER_BUS_COLLISION)      			//Write Slave address and set master for transmission
		return FALSE;

	if(i2c_Wait_IntF(id) == FALSE)
		return FALSE;

	if((I2CGetStatus(id) & _I2CSTAT_ACKSTAT_MASK) )
		return FALSE;

	return TRUE;

}

/* I2C : ���ֽ�
 * ����
 *     id       : I2Cģ��ID
 *     data     : �����ݱ���ָ��
 * ����
 *     TRUE     : OK
 *     FALSE    : Fail
 */
PRIVATE int8u i2c_read_byte(I2C_MODULE id, int8u * data)
{
	CLR_I2C_MIF(id);

	if(I2CReceiverEnable(id,1) == I2C_RECEIVE_OVERFLOW)
		return FALSE;

	if(i2c_Wait_IntF(id) == FALSE)
		return FALSE;

    if (I2CReceivedDataIsAvailable(id))
    {
		*data = I2CGetByte(id);
		return TRUE;
    }

	return FALSE;
}

/* =========================================================================== */
/*                                 API ����                                    */
/* =========================================================================== */

/* =========================================================
 * I2C ģ���ʼ��
 *     id       : I2Cģ��ID
 *     I2C_BRG  : ������
 */
PUBLIC void initI2CModule(I2C_MODULE id, UINT32 I2C_BRG)
{
	I2C_SCL_SETOUTPUT();
	I2C_SDA_SETOUTPUT();
	I2C_SCL_OUT1();
	I2C_SDA_OUT1();
	delay_nop(50);
	
    #if 0
    //��λPCA9505
	I2C_RST_SETOUTPUT();
    I2C_RST_OUT0();
    delay_nop(50);
    I2C_RST_OUT1();
    #endif
    
	I2C_SCL_SETOUTPUT();
	I2C_SDA_SETOUTPUT();
	I2C_SCL_OUT1();
	I2C_SDA_OUT1();

	I2C_REGISTERS * const i2cRegisters = i2cBase[id];
	i2cRegisters->I2CxCON = 0;									//I2CxCON = 0

	I2CClearStatus(id, 0xffff);							//I2CxSTAT = 0
	
    /************ I2C interrupt configuration */
	//ϵͳĬ�Ϲر��ж�

    /***************** I2C configuration *****/
	I2CConfigure(id, I2C_ENABLE_HIGH_SPEED );
    I2CSetSlaveAddress(id, 0, 0, I2C_USE_7BIT_ADDRESS);
	I2CSetFrequency(id, GetPeripheralClock(), I2C_BRG);		// Frequency set

	I2CEnable(id, TRUE);
	
    /***********************************
    *		.5 0 = ���� /ACK
    *        I2C1 enabled
    *        continue I2C module in Idle mode
    *        IPMI mode not enabled
    *        I2CADD is 7-bit address
    *        Disable Slew Rate Control for 100KHz
    *        Enable SM bus specification
    *        Disable General call address
    *        Baud @ 16MHz = 79 into I2CxBRG
    */
	delay_nop(50);
}

/* =========================================================
 * I2C : д�����ֽ�
 * ����
 *     id           : I2Cģ��ID
 *     slaveAddr    : ������ַ A2 A1 A0
 *     regAddr      : д���ַ
 *     pData        : ��д����ָ��
 *     len          : ��д���ݳ���
 * ����
 *     ����״̬��   : 0=OK, ��0=Fail
 *     
 */
PUBLIC int8u I2C_WriteBuf(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData, int16u len)
{
    int8u ret;
    int   dly = 0;
    
    /* ���ô���״̬�� */
	I2C_OUTPUT_Err.byte = 0;
	
	do
	{
        /* ��SSPxCON2 �Ĵ�����SEN λ��1����������������*/
    	ret = i2c_start(id);
    	
    	if(ret == FALSE)
        {
            I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_Start = 1;
            break;
        }
        
        /* ���ʹ�������ַ */
        ret = i2c_write_byte(id, GET_WR_ADDR(slaveAddr));
    	if(ret == FALSE)
        {
            I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_SlaveDeviceAddressW = 1;
            break;
        }
        
        /* ���ʹ������Ĵ�����ַ */
        ret = i2c_write_byte(id, regAddr | ADDR_AI);
    	if(ret == FALSE)
        {
            I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_SlaveCommand = 1;
            break;
        }
        
        
        /* �������� */
        while(len)
        {
            ret = i2c_write_byte(id, *pData);
            if(ret == FALSE)
            {
                I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_WriteByte = 1;
                break;
            }
            
            /* �ȴ�������ɣ���ʱ��ʱ�����ô��� */
            for(dly=0; dly<1000; dly++)
            {
                if (I2CTransmissionHasCompleted(id))
                {
                    if (I2CByteWasAcknowledged(id))
                    {
                        /* transmission successful */
                        break;
                    }
                }
            }
            if(dly==1000)
            {
                I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_WriteByte = 1;
                break;
            }

            /* next data */
            len--;
            pData++;
        }
        
    }while(0);

    /* ֹͣ */
    ret = i2c_stop(id);
    if(ret == FALSE)
	{
		I2C_OUTPUT_Err.bits.bI2C_ERR_OUTPUT_Stop = 1;
	}
	
	
	if(I2C_OUTPUT_Err.byte != 0)
    {
        I2C_ErrFLAG.bits.bI2C_OutputErr = 1;
    }
    else
    {
        I2C_ErrFLAG.bits.bI2C_OutputErr = 0;
    }

	return I2C_OUTPUT_Err.byte;
}



/* =========================================================
 * I2C : дһ���ֽ�
 * ����
 *     id           : I2Cģ��ID
 *     slaveAddr    : ������ַ
 *     regAddr      : д���ַ
 *     Data         : ��д����
 * ����
 *     ����״̬��   : 0=OK, ��0=Fail
 *     
 */
PUBLIC int8u I2C_WriteOneByte(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u Data)
{
    int8u ret;
    
    ret = I2C_WriteBuf(id, slaveAddr, regAddr, &Data, 1);
    return ret;

//    #if 0
//    int   dly;
//    int8u ret;
//    
//    /* ���ô���״̬�� */
//	I2C_OUTPUT_Err.byte = 0;
//	
//	do
//	{
//        /* ��SSPxCON2 �Ĵ�����SEN λ��1����������������*/
//    	ret = i2c_start(I2C_ID);
//    	
//    	if(ret == FALSE)
//        {
//            bI2C_ERR_OUTPUT_Start = 1;
//            break;
//        }
//        
//        /* ���ʹ�������ַ */
//        ret = i2c_write_byte(id, GET_WR_ADDR(slaveAddr));
//    	if(ret == FALSE)
//        {
//            bI2C_ERR_OUTPUT_SlaveDeviceAddressW = 1;
//            break;
//        }
//        
//        /* ���ʹ������Ĵ�����ַ */
//        ret = i2c_write_byte(id, regAddr);
//    	if(ret == FALSE)
//        {
//            bI2C_ERR_OUTPUT_SlaveCommand = 1;
//            break;
//        }
//        
//        /* �������� */
//        ret = i2c_write_byte(id, *pData);
//        if(ret == FALSE)
//        {
//            bI2C_ERR_OUTPUT_WriteByte = 1;
//            break;
//        }
//        
//        /* �ȴ�������ɣ���ʱ��ʱ������������д�ģ����� */
//        dly = 0;
//        while(!I2CTransmissionHasCompleted(id))
//        {
//            dly++;
//            if(dly > 10000)
//            {
//                bI2C_ERR_OUTPUT_WriteByte = 1;
//                break;
//            }
//        }
//
//    }while(0);
//
//    /* ֹͣ */
//    ret = i2c_stop(id);
//    if(ret == FALSE)
//	{
//		bI2C_ERR_OUTPUT_Stop = 1;
//	}
//	
//	return I2C_OUTPUT_Err.byte;
//	#endif
}


/* =========================================================
 * I2C : ��һ���ֽ�
 * ����
 *      id              : I2Cģ��ID
 *      slaveAddr       : ������ַ
 *      regAddr         : �������ݵ�ַ
 *      pData           : ��ȡ���ݴ洢ָ��
 *      len             : �����ݳ���
 * ����
 *     ����״̬��   : 0=OK, ��0=Fail
 *     
 */
PUBLIC int8u I2C_ReadBytes(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData, int8u len)
{
    int8u ret;
    
	I2C_INPUT_Err.byte = 0;
	
	do
	{
	    /* ���� */
	    ret = i2c_start(id);
	    if(ret == FALSE)
        {
            I2C_INPUT_Err.bits.bI2C_ERR_INPUT_Start = 1;
            break;
        }
        
	    /* д������ַ��д�� */
        ret == i2c_write_byte(id,GET_WR_ADDR(slaveAddr));
	    if(ret == FALSE)
        {
            I2C_INPUT_Err.bits.bI2C_ERR_INPUT_WriteSlaveDeviceAddressW = 1;
            break;
        }
        
	    /* д�������ݵ�ַ */
        ret == i2c_write_byte(id, regAddr | ADDR_AI);
	    if(ret == FALSE)
        {
            I2C_INPUT_Err.bits.bI2C_ERR_INPUT_WriteSlaveCommand = 1;
            break;
        }
        
        /* ������ */
	    ret == i2c_repeat_start(id);
	    if(ret == FALSE)
        {
            I2C_INPUT_Err.bits.bI2C_ERR_INPUT_RepeatStart = 1;
            break;
        }
	    
        /* д������ַ������ */
	    ret == i2c_write_byte(id,GET_RD_ADDR(slaveAddr));
	    if(ret == FALSE)
        {
            I2C_INPUT_Err.bits.bI2C_ERR_INPUT_WriteSlaveDeviceAddressR = 1;
            break;
        }
	    
        while(len)
        {
            /* �����ݱ��� */
            ret == i2c_read_byte(id, pData);
            pData++;
            
            if(ret == FALSE)
            {
                I2C_INPUT_Err.bits.bI2C_ERR_INPUT_ReadByte = 1;
                break;
            }
            
            len--;
            ret == i2c_ack(id, len > 0);
            
            if(ret == FALSE)
            {
                I2C_INPUT_Err.bits.bI2C_ERR_INPUT_Noack = 1;
                break;
            }
        }
	    
	}while(0);
	
	/* ֹͣ */
	ret = i2c_stop(id);
    if(ret == FALSE)
    {
        I2C_INPUT_Err.bits.bI2C_ERR_INPUT_Stop = 1;
    }
    
	if(I2C_INPUT_Err.byte != 0)
    {
        I2C_ErrFLAG.bits.bI2C_InputErr = 1;
    }
    else
    {
        I2C_ErrFLAG.bits.bI2C_InputErr = 0;
    }
	
	return I2C_INPUT_Err.byte;
}


/* =========================================================
 * I2C : ��һ���ֽ�
 * ����
 *     id           : I2Cģ��ID
 *     slaveAddr    : ������ַ
 *     regAddr      : �������ݵ�ַ
 *     pData        : ��ȡ���ݴ洢ָ��
 * ����
 *     ����״̬��   : 0=OK, ��0=Fail
 *     
 */
PUBLIC int8u I2C_ReadOneByte(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData)
{
    return I2C_ReadBytes(id, slaveAddr, regAddr, pData, 1);
}
