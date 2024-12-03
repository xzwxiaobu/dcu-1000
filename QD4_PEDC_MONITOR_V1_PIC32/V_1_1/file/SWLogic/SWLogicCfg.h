/* 
 * File:    SWLogicCfg.h
 *          ���Ϊ������ģ�飬�������ⲿ����
 * 
 * Author: LLY
 *
 * Created on 2021-1-13
 */

#ifndef SWLOGIC_CFG_H
#define	SWLOGIC_CFG_H

/* =============================================================================
 * ����������߼����ܵ����������� 1~9
 *  */
#define IBP_NUM             1   /* IBP ����������0 ��ʾ����Ҫ����߼�   */
#define PSL_NUM             3   /* PSL ����������0 ��ʾ����Ҫ����߼�   */
#define SIG_NUM             0   /* SIG ����������0 ��ʾ����Ҫ����߼�   */
#define PSC_NUM             1   /* PSC ����������0 ��ʾ����Ҫ����߼�   */
#define OCC_NUM             0   /* OCC ����������0 ��ʾ����Ҫ����߼�   */
#define ATC_NUM             0   /* ATC ����������0 ��ʾ����Ҫ����߼�   */
#define BAK_NUM1            0   /* BAK1 ����������0 ��ʾ����Ҫ����߼�  */
#define BAK_NUM2            0   /* BAK2 ����������0 ��ʾ����Ҫ����߼�  */

#define CTRL_LOGIC_NUM      (IBP_NUM + PSL_NUM + SIG_NUM + PSC_NUM + OCC_NUM + ATC_NUM + BAK_NUM1 + BAK_NUM2)
#if((CTRL_LOGIC_NUM <= 0) || (CTRL_LOGIC_NUM > 9))
#error  "CTRL_LOGIC_NUM must be 1~9 !"
#endif


#endif
