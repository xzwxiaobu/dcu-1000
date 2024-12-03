/* =============================================================================
 * Project :   
 * Module  : MODBUS 寄存器
 * Date    : 2019-11-10
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _SIGSYS_REG_H
#define _SIGSYS_REG_H

#if 1
typedef struct
{
    int16u  wTrainDoorFault0;       /* 读写寄存器 列车门故障标志 1~16   */
    int16u  wTrainDoorFault1;       /* 读写寄存器 列车门故障标志 17~24  */
    
    int16u rPSDFault0;              /* 只读寄存器 站台门故障标志 1~16   */
    int16u rPSDFault1;              /* 只读寄存器 站台门故障标志 17~24  */
    
    int16u rsv4;                    /* 预留，未定义，固定为0 */
    int16u rsv5;                    /* 预留，未定义，固定为0 */
    int16u rsv6;                    /* 预留，未定义，固定为0 */
    int16u rsv7;                    /* 预留，未定义，固定为0 */    
}tNJ7SigRegister;


#endif

//**********************************************
#endif  //_SIGSYS_REG_H



