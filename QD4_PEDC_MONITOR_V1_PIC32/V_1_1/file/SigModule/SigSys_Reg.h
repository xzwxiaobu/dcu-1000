/* =============================================================================
 * Project :   
 * Module  : MODBUS �Ĵ���
 * Date    : 2019-11-10
 * Auth.   : LLY
 * Note    : 
 */

#ifndef _SIGSYS_REG_H
#define _SIGSYS_REG_H

#if 1
typedef struct
{
    int16u  wTrainDoorFault0;       /* ��д�Ĵ��� �г��Ź��ϱ�־ 1~16   */
    int16u  wTrainDoorFault1;       /* ��д�Ĵ��� �г��Ź��ϱ�־ 17~24  */
    
    int16u rPSDFault0;              /* ֻ���Ĵ��� վ̨�Ź��ϱ�־ 1~16   */
    int16u rPSDFault1;              /* ֻ���Ĵ��� վ̨�Ź��ϱ�־ 17~24  */
    
    int16u rsv4;                    /* Ԥ����δ���壬�̶�Ϊ0 */
    int16u rsv5;                    /* Ԥ����δ���壬�̶�Ϊ0 */
    int16u rsv6;                    /* Ԥ����δ���壬�̶�Ϊ0 */
    int16u rsv7;                    /* Ԥ����δ���壬�̶�Ϊ0 */    
}tNJ7SigRegister;


#endif

//**********************************************
#endif  //_SIGSYS_REG_H



