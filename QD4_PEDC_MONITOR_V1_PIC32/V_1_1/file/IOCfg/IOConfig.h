#ifndef _IOCONFIG_H
#define _IOCONFIG_H

#include "MonCommCfg.h"

PUBLIC void initPic32GPIOs(void);

PUBLIC int16u * getPic32GPIOsCfg(void);

///* 
// * ��ȡPCB���IO����
// *  commObj         : PCB������
// *  pIOConfigure    : �洢ָ��
// * */
//PUBLIC void getIOConfig(tEnmCommObj commObj, tSlaveIOScanCfg *pIOConfigure);
//
///* ��ȡPCB������ʹ������ */
////PUBLIC tSlaveOutputEnCfg * getOutputEnCfg(tEnmCommObj commObj);
//
//PUBLIC tSlaveIOScanCfg *getDefaultIOConfigure(void);
//
//PUBLIC void initChipGPIOs(int16u * pCfg);
//
//PUBLIC void initAssignedGPIOs(tEnmCommObj commObj);

#endif  //_IOCONFIG_H

