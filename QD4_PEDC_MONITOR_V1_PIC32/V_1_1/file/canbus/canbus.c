
#include "_g_canbus.h"
#include "canbus.h"
#include "_glbApi.h"
#include "ObjDict.h"

#define ValBit(Var,Place)       ( Var & (1<<Place) )



//���ز���
void    DownPara(void)
{
/*      int8u i;
        int8u * ptr;

        if(Bus_DownDcuParaState() != ican_write_OK)
            return;

        clsDCU_DownDcuParaState();

        ptr = (int8u *) & DcuParaDown;
        for(i = 0; i < ONE_DCU_PARA_LEN;  i ++)
        {
            *ptr++ = can_down_dcuPara[i];
        }

        New_para = 1;
*/
}

void    GetFunCanState(void)                          //����״̬
{

/*      if(Bus_ReadDcuParaState() == ican_read_ing)         //�Ƿ��ȡ ������
        {
            Para_Reading = 1;
        }
        else
            {
                Para_Reading = 0;
            }

        if(Bus_ReadDcuState_State() == ican_read_ing) //�Ƿ��ȡ ״̬��
            {
                State_Reading = 1;
            }
        else
            {
                State_Reading = 0;
            }

        if(Bus_ReadOpenCurveState() == ican_read_ing) //�Ƿ��ȡ����������
            {
                OpenSpeedCurve_Reading = 1;
            }
        else
            {
                OpenSpeedCurve_Reading = 0;
            }

        if(Bus_ReadCloseCurveState() == ican_read_ing) //�Ƿ��ȡ����������
            {
                CloseSpeedCurve_Reading = 1;
            }
        else
            {
                CloseSpeedCurve_Reading = 0;
            }*/

}


//CAN ͨѶ����ֵ ����������
void    CanFun(void)
{
/*      //can ͨѶ
        ApiMainProcess();
        DownPara();          //DCU PARA �����Ƿ�ɹ� ����ɹ��� New_para == 1
        GetFunCanState();    //����״̬
*/
}


