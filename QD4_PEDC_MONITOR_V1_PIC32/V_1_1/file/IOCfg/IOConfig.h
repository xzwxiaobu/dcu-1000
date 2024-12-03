#ifndef _IOCONFIG_H
#define _IOCONFIG_H

#include "MonCommCfg.h"

PUBLIC void initPic32GPIOs(void);

PUBLIC int16u * getPic32GPIOsCfg(void);

///* 
// * 获取PCB板的IO配置
// *  commObj         : PCB板类型
// *  pIOConfigure    : 存储指针
// * */
//PUBLIC void getIOConfig(tEnmCommObj commObj, tSlaveIOScanCfg *pIOConfigure);
//
///* 获取PCB板的输出使能配置 */
////PUBLIC tSlaveOutputEnCfg * getOutputEnCfg(tEnmCommObj commObj);
//
//PUBLIC tSlaveIOScanCfg *getDefaultIOConfigure(void);
//
//PUBLIC void initChipGPIOs(int16u * pCfg);
//
//PUBLIC void initAssignedGPIOs(tEnmCommObj commObj);

#endif  //_IOCONFIG_H

