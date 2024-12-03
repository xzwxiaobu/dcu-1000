
#ifndef _TEST_H
#define _TEST_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

#define MMS_CMD_TEST            0xDD    /* �����ã��鿴PEDC���� */
#define MMS_CMD_TEST_OUT        0xDE    /* ��������� */
#define MMS_CMD_TEST_BASREG     0xDF    /* BAS �Ĵ��� */

//--------------------------------------
//PEDC ��ʱͨ����MMSͨ�ŵĴ��ڷ��Ͳ����õ����ݣ�ֱ��PEDC���յ� ������ͨѶ����Ϊֹ
//PEDC �յ� 10 02 00 00 C8 cmd 00 04 AA BB CC DD ... ������ PEDC ִ�в��Դ��롣
//   cmd = DD       ����PEDC���Թ��ܣ�ͬʱ��Ҫ������������Ƿ�Ϊ AA BB CC DD��Ԥ���󴥷�
//   ... = �������ݣ�������Ҳ����û��
#define TEST_CODE_AA 0xAA          
#define TEST_CODE_BB 0xBB
#define TEST_CODE_CC 0xCC
#define TEST_CODE_DD 0xDD

//PC����  10 02 00 00 C8 cmd 00 04 AA BB msH msL (output data) ...  �� PEDC ,���� PEDC ����IO�������
//   cmd = DE      ����PEDC����������ܣ�ͬʱ��Ҫ������������Ƿ�Ϊ AA BB��Ԥ���󴥷�
//  (msH msL)      ���Ե����������Чʱ�䣬��λ�� ms���� ������ݾ��� (msH msL)ʱ��󣬻ָ��������
//  (output data)  ���Ե��������

//--------------------------------------
#define cTEST_CODE_SEND_TIME  400        //400 ms

//--------------------------------------
PUBLIC void testIO(void);

PUBLIC void setDoTestFunct(BOOL run);
PUBLIC void setDoTestFunctDCUNo(int8u DcuNo);

/* ���ò��Ե��������
 *  */
PUBLIC int setPic24TestOutputData(int8u *data, int len, int time);

PUBLIC BOOL chkIfTestOutputIO(void);

PUBLIC int8u* getTestOutputIOData(void);

//--------------------------------------
#endif

