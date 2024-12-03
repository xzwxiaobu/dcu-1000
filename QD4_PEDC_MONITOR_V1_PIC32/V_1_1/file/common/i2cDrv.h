#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include "I2CPrivate.h"

#define CMD_WRITE                   0b01000000
#define CMD_READ                    0b01000001
#define ADDR_AI                     0x80            /* address auto increase */

//address	0100 A2 A1 A0 R/W
#define GET_WR_ADDR(slave_addr)     (((slave_addr&7) << 1) | CMD_WRITE)
#define GET_RD_ADDR(slave_addr)     (((slave_addr&7) << 1) | CMD_READ )

//------------------------------------------------------------------------------
/* I2C1 的 IO脚位
 *  PD9=SDA, PD10=SCL */
//#define I2C_SDA_PortBit       (1 << 9 )
//#define I2C_SCL_PortBit       (1 << 10)

/* I2C3 的 IO脚位: 
 *  PD2=SDA, PD3=SCL */
#define I2C_SDA_PortBit       (1 << 2 )
#define I2C_SCL_PortBit       (1 << 3 )


#define I2C_SCL_SETOUTPUT()   (mPORTDSetPinsDigitalOut(I2C_SCL_PortBit))    //SCL SET OUTPUT
#define I2C_SDA_SETINPUT()    (mPORTDSetPinsDigitalIn(I2C_SDA_PortBit))     //SDA SET INPUT
#define I2C_SDA_SETOUTPUT()   (mPORTDSetPinsDigitalOut(I2C_SDA_PortBit))    //SDA SET INPUT

#define I2C_SCL_OUT1()        (mPORTDSetBits(I2C_SCL_PortBit))
#define I2C_SCL_OUT0()        (mPORTDClearBits(I2C_SCL_PortBit))
#define I2C_SDA_OUT1()        (mPORTDSetBits(I2C_SDA_PortBit))
#define I2C_SDA_OUT0()        (mPORTDClearBits(I2C_SDA_PortBit))

/* =============================================================================
 * 接口函数
 */
PUBLIC void  initI2CModule(I2C_MODULE id, UINT32 I2C_BRG);

PUBLIC int8u I2C_WriteOneByte(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u Data);
PUBLIC int8u I2C_WriteBuf(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData, int16u len);

PUBLIC int8u I2C_ReadOneByte(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData);
PUBLIC int8u I2C_ReadBytes(I2C_MODULE id, int8u slaveAddr, int8u regAddr, int8u *pData, int8u len);

#endif  /*I2C_DRIVER_H*/

