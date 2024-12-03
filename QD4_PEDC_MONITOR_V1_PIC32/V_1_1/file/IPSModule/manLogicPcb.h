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
    
    
/* ����ǰ����SIG��ID */
PUBLIC tEnmCommObj getWorkSigID(void);

/* ���� SIG������ */
PUBLIC void manageSig1Sig2(void);
    
/* �� SIG ��Ĺ���״̬
 * ��4λ�� SIG2����4λ�� SIG1
 *  */
PUBLIC int8u getLOGPcbState(void);


#ifdef	__cplusplus
}
#endif

#endif	/* MANSIGPCB_H */

