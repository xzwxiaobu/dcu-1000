/* 
 * File:   manLogicPcb.h
 * Author: LLY
 *
 * Date: 2021-2-23
 */

#ifndef MANSIGPCB_H
#define	MANSIGPCB_H

#ifdef	__cplusplus
extern "C"
{
#endif

    
#include "_cpu.h"
    
    
/* 读当前工作SIG板ID */
PUBLIC tEnmCommObj getWorkSigID(void);

/* 管理 SIG板冗余 */
PUBLIC void manageSig1Sig2(void);
    
/* 读 SIG 板的工作状态
 * 高4位是 SIG2，低4位是 SIG1
 *  */
PUBLIC int8u getLOGPcbState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MANSIGPCB_H */

