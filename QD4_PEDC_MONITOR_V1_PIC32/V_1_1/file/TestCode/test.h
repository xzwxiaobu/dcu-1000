
#ifndef _TEST_H
#define _TEST_H

#ifndef ROOT
#define EXTN extern
#else
#define EXTN
#endif

#define MMS_CMD_TEST            0xDD    /* 调试用，查看PEDC数据 */
#define MMS_CMD_TEST_OUT        0xDE    /* 测试输出点 */
#define MMS_CMD_TEST_BASREG     0xDF    /* BAS 寄存器 */

//--------------------------------------
//PEDC 定时通过与MMS通信的串口发送测试用的数据，直到PEDC接收到 其它的通讯代码为止
//PEDC 收到 10 02 00 00 C8 cmd 00 04 AA BB CC DD ... ，启动 PEDC 执行测试代码。
//   cmd = DD       启动PEDC测试功能，同时需要检测数据内容是否为 AA BB CC DD，预防误触发
//   ... = 测试数据，可以有也可以没有
#define TEST_CODE_AA 0xAA          
#define TEST_CODE_BB 0xBB
#define TEST_CODE_CC 0xCC
#define TEST_CODE_DD 0xDD

//PC发送  10 02 00 00 C8 cmd 00 04 AA BB msH msL (output data) ...  至 PEDC ,触发 PEDC 测试IO输出功能
//   cmd = DE      启动PEDC测试输出功能，同时需要检测数据内容是否为 AA BB，预防误触发
//  (msH msL)      测试的输出数据有效时间，单位是 ms，即 输出数据经过 (msH msL)时间后，恢复正常输出
//  (output data)  测试的输出数据

//--------------------------------------
#define cTEST_CODE_SEND_TIME  400        //400 ms

//--------------------------------------
PUBLIC void testIO(void);

PUBLIC void setDoTestFunct(BOOL run);
PUBLIC void setDoTestFunctDCUNo(int8u DcuNo);

/* 设置测试的输出数据
 *  */
PUBLIC int setPic24TestOutputData(int8u *data, int len, int time);

PUBLIC BOOL chkIfTestOutputIO(void);

PUBLIC int8u* getTestOutputIOData(void);

//--------------------------------------
#endif

