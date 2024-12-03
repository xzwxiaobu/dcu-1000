#ifndef _PIC24_UPDATEIF_H
#define _PIC24_UPDATEIF_H

#include "_cpu.h"


PUBLIC BOOL upPIC24_ChkP24Frame(void);

PUBLIC BOOL upPIC24_SendToPic24(int8u msgType, int8u txDstNode, int8u * txData, int16u txDataLen);

PUBLIC BOOL upPIC24_ResetChip(tEnmCommObj updatePic24Obj);

#endif //_PIC24_UPDATEIF_H

