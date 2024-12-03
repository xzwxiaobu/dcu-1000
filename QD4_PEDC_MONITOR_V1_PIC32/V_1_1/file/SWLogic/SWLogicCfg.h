/* 
 * File:    SWLogicCfg.h
 *          设计为独立的模块，不依赖外部定义
 * 
 * Author: LLY
 *
 * Created on 2021-1-13
 */

#ifndef SWLOGIC_CFG_H
#define	SWLOGIC_CFG_H

/* =============================================================================
 * 定义各控制逻辑功能的数量，总数 1~9
 *  */
#define IBP_NUM             1   /* IBP 功能数量，0 表示不需要软件逻辑   */
#define PSL_NUM             3   /* PSL 功能数量，0 表示不需要软件逻辑   */
#define SIG_NUM             0   /* SIG 功能数量，0 表示不需要软件逻辑   */
#define PSC_NUM             1   /* PSC 功能数量，0 表示不需要软件逻辑   */
#define OCC_NUM             0   /* OCC 功能数量，0 表示不需要软件逻辑   */
#define ATC_NUM             0   /* ATC 功能数量，0 表示不需要软件逻辑   */
#define BAK_NUM1            0   /* BAK1 功能数量，0 表示不需要软件逻辑  */
#define BAK_NUM2            0   /* BAK2 功能数量，0 表示不需要软件逻辑  */

#define CTRL_LOGIC_NUM      (IBP_NUM + PSL_NUM + SIG_NUM + PSC_NUM + OCC_NUM + ATC_NUM + BAK_NUM1 + BAK_NUM2)
#if((CTRL_LOGIC_NUM <= 0) || (CTRL_LOGIC_NUM > 9))
#error  "CTRL_LOGIC_NUM must be 1~9 !"
#endif


#endif
