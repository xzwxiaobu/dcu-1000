
#include "..\Include.h"

#include "SigSys_Reg.h"
#include "SigSys_App.h"
#include "dcu.h"

#ifdef PRIVATE_STATIC_DIS
#undef  PRIVATE
#define PRIVATE
#endif

/* ���ź�ϵͳͨ�ŵ����ݼĴ��� */
tNJ7SigRegister NJ7SigRegister;

PRIVATE int8u PSDInhibitTrain[(MAX_DCU+7)/8];

/* ���ź�ϵͳͨ�ŵ����ݼĴ���
 * 1. ���Ŷ�λ����վ̨�� (�ź�ϵͳ���͵�)
 * 2. վ̨�Ŷ�λ���복�� (����DCU��Ϣ�жϵõ����ź�ϵͳ��ѵ��ȡ)
 *  */
void GetMODBUSMessage(int8u *pBuf)
{
	int i;
    int8u *p;
    int8u *pInhibit;
        
    /* �õ�վ̨�Ŷ�λ�����г��ŵ���Ϣ */
    getPSDInhibitTrain(PSDInhibitTrain, sizeof(PSDInhibitTrain));
    
    NJ7SigRegister.rPSDFault0 = ((int16u)PSDInhibitTrain[1] << 8) + (int16u)PSDInhibitTrain[0];
    NJ7SigRegister.rPSDFault1 = ((int16u)PSDInhibitTrain[3] << 8) + (int16u)PSDInhibitTrain[2];
    
    /* �г��ŷ��͸�վ̨�ŵĶ�λ�����־��
     * PEDC���յ����ź�ϵͳ���͵��г���״̬�󣬱����� aMcpOpenCloseDoorCommand 
     */
    pInhibit = &uniDcuData.sortBuf.aMcpOpenCloseDoorCommand[0];
    NJ7SigRegister.wTrainDoorFault0 = ((int16u)pInhibit[1] << 8) + (int16u)pInhibit[0];
    NJ7SigRegister.wTrainDoorFault1 = ((int16u)pInhibit[3] << 8) + (int16u)pInhibit[2];

    NJ7SigRegister.rsv4 = 0;
    NJ7SigRegister.rsv5 = 0;
    NJ7SigRegister.rsv6 = 0;
    NJ7SigRegister.rsv7 = 0;
    
    /* MODBUS Э��
     * ���ֽں͵��ֽڽ���λ��
     *  */
    p = (int8u*)&NJ7SigRegister;
    for(i=0; i<sizeof(NJ7SigRegister); i+=2)
    {
        *pBuf++ = *(p+1);
        *pBuf++ = *p;
        
        p += 2;
    }
}


