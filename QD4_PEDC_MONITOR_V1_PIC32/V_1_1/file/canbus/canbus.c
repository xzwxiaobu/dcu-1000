
#include "_g_canbus.h"
#include "canbus.h"
#include "_glbApi.h"
#include "ObjDict.h"

#define ValBit(Var,Place)       ( Var & (1<<Place) )



//下载参数
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

void    GetFunCanState(void)                          //功能状态
{

/*      if(Bus_ReadDcuParaState() == ican_read_ing)         //是否读取 参数中
        {
            Para_Reading = 1;
        }
        else
            {
                Para_Reading = 0;
            }

        if(Bus_ReadDcuState_State() == ican_read_ing) //是否读取 状态中
            {
                State_Reading = 1;
            }
        else
            {
                State_Reading = 0;
            }

        if(Bus_ReadOpenCurveState() == ican_read_ing) //是否读取开门曲线中
            {
                OpenSpeedCurve_Reading = 1;
            }
        else
            {
                OpenSpeedCurve_Reading = 0;
            }

        if(Bus_ReadCloseCurveState() == ican_read_ing) //是否读取开门曲线中
            {
                CloseSpeedCurve_Reading = 1;
            }
        else
            {
                CloseSpeedCurve_Reading = 0;
            }*/

}


//CAN 通讯，键值 ，参数下载
void    CanFun(void)
{
/*      //can 通讯
        ApiMainProcess();
        DownPara();          //DCU PARA 下载是否成功 ，如成功则 New_para == 1
        GetFunCanState();    //功能状态
*/
}


