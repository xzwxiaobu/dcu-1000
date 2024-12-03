/*=============================================================================
* Project:		WH2-LINE PEDC
* PEDC MCU:		PIC32MX795F512H + PIC18F23K22
* Fcrystal:		4M
* PLL:			X20
* FCY:			SYSTEM_FREQ = 80M
*
* Version:		0.0
* Author:		lly
* Date:			2011-4-1
* =============================================
* 文	 件	:	UpBios.C
*  在线升级功能模块(BIOS 函数，固化在FLASH指定地址)
*  此文件中所有的函数不能被升级
*  此文件中不能调用 PIC32 自带的函数库
* =============================================================================*/

#include <p32xxxx.h>
#include <peripheral/uart.h>

#include "_g_canbus.h"

#include "glbUpdate.h"
#include "Update.h"

//#include "plib.h"			//此处不包含此文件，避免调用了系统的库函数

/**********************************************/
//define = 不升级 BootFlash Memory
//         擦除 BootFlash 不成功，原因不明
//         注意修改 linker 文件，避免有需要的代码落在 BootFlash
//         编译后查看 HEX 文件，搜索 020000041f 应只有1条内容是
//			:020000040000fa
//			:020000041fc01b
//			:10000000009d1a3c80205a27080040030000000091
//			汇编指令代码是
//			  1FC0_0000                lui         k0,0x9d00
//			  1FC0_0004                addiu       k0,k0,8320
//			  1FC0_0008                jr          k0
//			  1FC0_000C                nop

#define NOT_UPGRADE_BFM

/**********************************************/

//可编程的最大数据块为1	行，等于512	字节的数据。数据行必须先装入SRAM 的缓冲区中。
//控制器会忽略行以下的地址位，总是从行起始位置处开始编程。
//一组8行组成一页；因此，它包含8 ×	512	= 4096 字节或1024 个指令字。闪存页是在单次中可擦除的最小存储器单元

//=========================================================
//funciton define
//=========================================================
void  __attribute__((section (".SEG_UPGRADE_BIOS"),	unique_section))  bios_CopyBakFlash(tUpdate_Tx_Msg *pTxMsg);
int8u __attribute__((section (".SEG_UPGRADE_BIOS"),	unique_section))  bios_WriteFlash(int32u dwSrc, int8u *pCode, int16u cntN);
void  __attribute__((section (".SEG_UPGRADE_BIOS"),	unique_section))  bios_Finished(tUpFwState *pState,tUpdate_Tx_Msg *pTxMsg);
int8u __attribute__((section (".SEG_UPGRADE_BIOS"),	unique_section))  bios_SaveState(tUpFwState *pState);
void  __attribute__((section (".SEG_UPGRADE_BIOS"),	unique_section))  bios_ReadState(tUpFwState *pState);

void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_SoftReset(void);
void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMClearError(void);
void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_ResInt(int32u status);

void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMProgram(void *Addr, int32u *pCode, int32u Len);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMOperation (int32u opcode);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMWriteUint32 (void* address, int32u data);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMWriteRow (void* address, void* data);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMErasePage(void* address);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NVMErasePFM(void);
void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) *bios_memcpy(void *dst , const void *src , int32u n);
int32u __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_memcmp(const void *s1, const void *s2, size_t n);

//********** uart **********
void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_TxMsg(tUpdate_Tx_Msg *pTxMsg, char *buffer, int32u size);
void   __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_TxInt(tUpdate_Tx_Msg *pTxMsg, int32u Data, char *p);
int8u  __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) bios_NibbleToAsc(int8u data);

void __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) LedOn(tUpdate_Tx_Msg *pLed);
void __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) LedOff(tUpdate_Tx_Msg *pLed);
void __attribute__((section (".SEG_UPGRADE_BIOS"), unique_section)) LedToggle(tUpdate_Tx_Msg *pLed);


#define IF_UART_TX_READY()		(BOOL)(!(*(pTxMsg->UartPortSta) & _U1STA_UTXBF_MASK))
#define UART_TX_BYTE( data )	*(pTxMsg->UartPortTxReg) = (int8u)data
#define IF_UART_TX_FINISHED()	(BOOL)((*(pTxMsg->UartPortSta) & _U1STA_TRMT_MASK) == _U1STA_TRMT_MASK)

/*
#define LedOn()				*(pTxMsg->LedPort) &= pTxMsg->LedBit
#define LedOff()			*(pTxMsg->LedPort) |= pTxMsg->LedBit
#define LedToggle()			*(pTxMsg->LedPort) ^= pTxMsg->LedBit
*/

//**************************
//#define TST_MICROCHIP_NVM

#ifdef  TST_MICROCHIP_NVM
//system lib function
#define Flash_ClearError	NVMClearError
#define Flash_Program		NVMProgram
#define Flash_Write32		NVMWriteWord
#define Flash_WriteRow		NVMWriteRow
#define Flash_ErasePage		NVMErasePage

#else
//Upgrade bios function
#define Flash_ClearError	bios_NVMClearError
#define Flash_Program		bios_NVMProgram
#define Flash_Write32		bios_NVMWriteUint32
#define Flash_WriteRow		bios_NVMWriteRow
#define Flash_ErasePage		bios_NVMErasePage
#endif


//********************************************************************************************************
//                                                                                                       *
//                                        升级程序中的固化程序                                           *
//                                                                                                       *
//********************************************************************************************************

/********************************************************************
 *																	*
 *		FLASH ROM 的操作函数（NVM），必须用汇编，C函数无法完成		*
 *																	*
 *		此部分程序来自 PIC 库函数									*
 *		但是为了将程序放到指定段，而没调用PIC提供的库函数			*
 *																	*
 *******************************************************************/
//===============================================
/*
说明：
	PIC32MX 的CPU 内核不提供特定的RESET 指令；但是，硬件复位用软件来执行（软件复位），
	方法是执行软件复位命令序列。软件复位命令的作用类似于MCLR 复位。软件复位序列要求先执
	行系统解锁序列，然后才能写入SWRST 位。关于系统解锁的详细信息，请参见第6.3.6 节“ 时钟
	切换操作” （DS61112）。软件复位的执行方式如下：
	1 写入系统解锁序列
	2 设置SWRST 位（RSWRST<0>） = 1
	3 读取RSWRST 寄存器
	4 后面跟随“while(1);” 或4 条“NOP” 指令
	向RSWRST 寄存器写入1 会将SWRST 位置1，从而激活软件复位。RSWRST 寄存器的后续读
	操作会触发软件复位，它在读操作之后的下一个时钟周期发生。要确保在发生复位事件之前不执
	行任何其他用户代码，建议将4 条“NOP” 指令或“while(1);” 语句放在READ 指令之后。
	SWR 状态位（RCON<6>）被置1，指示发生了软件复位。
*/
void bios_SoftReset(void)
{
	asm volatile ("		addiu       $sp,$sp,-24	            ");
	asm volatile ("		sw          $ra,16($sp)             ");
	asm volatile ("		di			$v0						");	/*disable interrupt: v0 = status					*/
	asm volatile ("		nop                                 ");
	asm volatile ("		lui         $a0,0xbf88              ");
	asm volatile ("		lw          $v1,12288($a0)            "); /*BF883000 : DMACON 	*/
	asm volatile ("		ext         $v0,$v1,12,1              ");
	asm volatile ("		bne         $v0,$zero,_rst_SWRST 		");
	asm volatile ("		lui         $t7,0xaa99               ");
	asm volatile ("		addiu       $a2,$zero,4096            ");
	asm volatile ("		lui         $a1,0xbf88               ");
	asm volatile ("		sw          $a2,12296($a1)            "); /*BF883008 : DMACONSET	*/
	asm volatile ("_rst_wait0:               				");
	asm volatile ("		lw          $t0,12288($a0)            ");
	asm volatile ("		ext         $a3,$t0,11,1              ");
	asm volatile ("		bne         $a3,$zero,_rst_wait0     	");
	asm volatile ("		lui         $t7,0xaa99               ");
	asm volatile ("_rst_SWRST:               				");
	asm volatile ("		lui         $t5,0x5566               ");
	asm volatile ("		lui         $t3,0xbf81               ");
	asm volatile ("		ori         $t6,$t7,0x6655            ");
	asm volatile ("		ori         $t4,$t5,0x99aa            ");
	asm volatile ("		addiu       $t2,$zero,1               ");
	asm volatile ("		lui         $t1,0xbf81               ");
	asm volatile ("		sw          $zero,-3536($t3)          "); /* BF80F230 : SYSKEY	*/
	asm volatile ("		lui         $a0,0xbf81               ");
	asm volatile ("		sw          $t6,-3536($t3)            ");
	asm volatile ("		sw          $t4,-3536($t3)            "); /* SYSKEY = 0xaa996655	*/
	asm volatile ("		sw          $t2,-2536($t1)            "); /* SYSKEY = 0x556699aa	*/
	asm volatile ("		lw          $v0,-2544($a0)            "); /* BF80F610 : RSWRST	*/
	asm volatile ("_rst_wait1:               				");
	asm volatile ("		beq         $zero,$zero,_rst_wait1	"); /* while(1)				*/
	asm volatile ("		nop  								");
}

//===============================================
/*
写Flash：一个双字（32Bit是）
编程序列包含以下步骤：
1. 将要编程的32 位数据写入NVMDATA 寄存器。
2. 在NVMADDR 寄存器中装入要编程的地址。
3. 使用字编程命令运行解锁序列。
   编程序列完成， WR 位（NVMCON<15>）由硬件清零。
	address : Destination address to write.
	data    : Word to write.
	return  : '0' if operation completed successfully.
*/
int32u bios_NVMWriteUint32 (void* address, int32u data)
{
	asm volatile ("	lui			$t0,0x1fff			");
	asm volatile ("	ori			$a3,$t0,0xffff      ");
	asm volatile ("	addiu		$sp,$sp,-24         ");
	asm volatile ("	and			$a2,$a0,$a3         ");
	asm volatile ("	lui			$v0,0xbf81          ");
	asm volatile ("	lui			$v1,0xbf81          ");
	asm volatile ("	addiu		$a0,$zero,16385		");	/*0x4001: Unlock and Write Word				*/
	asm volatile ("	sw			$ra,16($sp)         ");
	asm volatile ("	sw			$a2,-3040($v0)		");	/*NVMADDR(0xBF80F420):address to program	*/
	asm volatile ("	sw			$a1,-3024($v1)		");	/*NVMDATA(0xBF80F430):data					*/
	asm volatile ("	jal			bios_NVMOperation   ");
	asm volatile ("	nop     	                    ");
	asm volatile ("	lw			$ra,16($sp)         ");
	asm volatile ("	addiu		$sp,$sp,24          ");
	//asm volatile ("	jr			$ra                 ");
	//asm volatile ("	nop								");
}

//===============================================
/*
行编程序列包含以下步骤：
1. 将要编程的整行数据写入系统SRAM。源地址必须字对齐。
2. 使用要编程的闪存行的起始地址设置NVMADDR 寄存器。
3. 使用来自步骤1 的物理源地址设置NVMSRCADDR 寄存器。
4. 使用行编程命令运行解锁序列。
5. 编程序列完成， WR 位（NVMCON<15>）由硬件清零。
	address : Destination Row address to write.
	data    : Location of data to write.
	return  : '0' if operation completed successfully.
*/
int32u bios_NVMWriteRow (void* address, void* data)
{
 	asm volatile ("	lui			$t0,0x1fff			");
 	asm volatile ("	ori			$a3,$t0,0xffff      ");
 	asm volatile ("	addiu		$sp,$sp,-24         ");
	asm volatile ("	and			$a2,$a0,$a3         ");
	asm volatile ("	and			$a1,$a1,$a3         ");
	asm volatile ("	lui			$v0,0xbf81          ");
	asm volatile ("	lui			$v1,0xbf81          ");
	asm volatile ("	addiu		$a0,$zero,16387		");	/*0x4003: Unlock and Write Row					*/
	asm volatile ("	sw			$ra,16($sp)         ");
	asm volatile ("	sw			$a2,-3040($v0)		");	/*NVMADDR:   Start Address of row to program	*/
	asm volatile ("	sw			$a1,-3008($v1)		");	/*NVMSRCADDR: SRAM data buffer Address			*/
	asm volatile ("	jal			bios_NVMOperation   ");
	asm volatile ("	nop                             ");
	asm volatile ("	lw			$ra,16($sp)         ");
	asm volatile ("	addiu		$sp,$sp,24          ");
	//asm volatile ("	jr			$ra             	");
	//asm volatile ("	nop								");
}

//===============================================
/*
页擦除序列包含以下步骤：
1. 使用要擦除页的地址设置NVMADDR 寄存器。
2. 使用所需的擦除命令运行解锁序列。
3. 擦除序列完成， WR 位（NVMCON<15>）由硬件清零。
	input :	$a0	= address ( Destination page address to Erase )
	return: '0' if operation	completed successfully.
*/
int32u bios_NVMErasePage(void* address)
{
	asm	volatile ("addiu		$sp,$sp,-24				");
	asm	volatile ("sw			$ra,16($sp)				");
	asm	volatile ("li			$a1,0x1fffffff			");
	asm	volatile ("and			$v1,$a0,$a1				");
	asm	volatile ("la			$v0,NVMADDR				");	/*Set NVMADDR to the Start Address of page to erase*/
	asm	volatile ("sw			$v1,0($v0)				");
	asm	volatile ("addiu		$a0,$zero,0x4004		");	/*0x4004: Unlock and Erase Page	*/
	asm	volatile ("jal			bios_NVMOperation		");
	asm	volatile ("nop									");
	asm	volatile ("lw			$ra,16($sp)				");
	asm	volatile ("addiu		$sp,$sp,24				");
	//asm volatile ("jr			$ra						");
	//asm volatile ("nop								");
}

//===============================================
void bios_NVMClearError(void)
{
	asm volatile ("	addiu		$sp,$sp,-24				");
	asm volatile ("	sw			$ra,16($sp)             ");
	asm volatile ("	addiu		$a0,$zero,0x4000        ");
	asm volatile ("	jal		bios_NVMOperation			");
	asm volatile ("	nop                                 ");
	asm volatile ("	lw			$ra,16($sp)             ");
	asm volatile ("	addiu		$sp,$sp,24              ");
	//asm volatile ("	jr			$ra                     ");
	//asm volatile ("	nop                                 ");
}

//===============================================
/*
	restores the core to the previous interrupt handling state.
*/
void bios_ResInt(int32u status)
{
	asm volatile ("	andi		$a0,$a0,0x1		");
	asm volatile ("	beq			$a0,$zero,1f	");
	asm volatile ("	nop							");
	asm volatile ("	ei							");
	asm volatile ("	b			4f				");
	asm volatile ("1:							");
	asm volatile ("	di							");
	asm volatile ("4:							");
	//asm volatile ("	jr			$ra				");
	//asm volatile ("	nop							");
}

//===============================================
/*
要解锁闪存操作，必须严格按照顺序执行下面的步骤4 至步骤9：如果未严格按照该序列执行，则WR 不会置1。
1. 暂停或禁止可访问外设总线和中断解锁序列的所有发起器，例如DMA 和中断。
2. 将WREN 位（NVMCON<14>）置1，以允许写入WR，并使用单条存储指令将NVMOP<3:0> 位（NVMCON<3:0>）设置为所需的操作。
3. 等待LVD 启动。
4. 向CPU 寄存器X 中装入0xAA996655。
5. 向CPU 寄存器Y 中装入0x556699AA。
6. 向CPU 寄存器Z 中装入0x00008000。
7. 将CPU 寄存器X 存储到NVMKEY 中。
8. 将CPU 寄存器Y 存储到NVMKEY 中。
9. 将CPU 寄存器Z 存储到NVMCONSET 中。
10. 等待WR 位（NVMCON<15>）清零。
11. 清零WREN 位（NVMCON<14>）。
12. 检查WRERR（NVMCON<13>）和LVDERR（NVMCON<12>）位，以确保编程/ 擦除序列成功完成。
当WR 位置1 时，编程/ 擦除序列会启动，在该序列期间， CPU 无法从闪存中执行。

	input :	$a0	= opration code
	return: '0' if operation	completed successfully.
*/
int32u bios_NVMOperation (int32u opcode)
{
	asm volatile ("	addiu		$sp,$sp,-24				");
	asm volatile ("	sw			$s0,16($sp)				");	/*push s0											*/
	asm volatile ("	sw			$ra,20($sp)				");	/*push ra											*/
	asm volatile ("	di			$v0						");	/*disable interrupt: v0 = status					*/
	asm volatile ("										");
	asm volatile ("	addu		$s0,$a0,$zero			");	/*参数												*/
	asm volatile ("	lui			$a2,0xbf88				");	/*													*/
	asm volatile ("	addu		$t0,$v0,$zero			");	/*t0 = v0											*/
	asm volatile ("	lw			$v0,12288($a2)			");	/*v0 = (0xbf883000)	(reg:DMACON)					*/
	asm volatile ("	ext			$a3,$v0,12,1			");	/*a3 = v0(bit12): DMA suspend(0	= normal)			*/
	asm volatile ("	bne			$a3,$zero,_wr_nvmkey	");	/*jump if DMA is suspend							*/
	asm volatile ("	lui			$t8,0xaa99				");	/*													*/
	asm volatile ("	addiu		$v1,$zero,4096			");	/*v1 =	0x1000										*/
	asm volatile ("	lui			$a0,0xbf88				");	/*													*/
	asm volatile ("	sw			$v1,12296($a0)			");	/*(0xbf883008) = $v1 (reg: DMACONSET)				*/
	asm volatile ("1:									");	/*													*/
	asm volatile ("	lw			$t1,12288($a2)			");	/*t1 =	(0xbf883000) (reg:DMACON)					*/
	asm volatile ("	ext			$a1,$t1,11,1			");	/*a1 =	$t1(bit11) : DMA busy						*/
	asm volatile ("	bne			$a1,$zero,1b			");	/*													*/
	asm volatile ("	lui			$t8,0xaa99				");	/*t8 = 0xaa990000									*/
	asm volatile ("_wr_nvmkey:							"); /*													*/
	asm volatile ("	lui			$t6,0x5566				");	/*t6 = 0x55660000									*/
	asm volatile ("	ori			$t9,$s0,0x4000			");	/*t9 = 0x4000 |	参数								*/
	asm volatile ("	lui			$t4,0xbf81				");	/*t4 = 0xbf810000									*/
	asm volatile ("	lui			$s0,0xbf81				");	/*s0 = 0xbf810000									*/
	asm volatile ("	ori			$t7,$t8,0x6655			");	/*t7 = 0xaa996655									*/
	asm volatile ("	ori			$t5,$t6,0x99aa			");	/*t5 = 0x556699aa									*/
	asm volatile ("	ori			$t3,$zero,0x8000		");	/*t3 = 0x8000										*/
	asm volatile ("	lui			$t2,0xbf81				");	/*t2 = 0xbf810000									*/
	asm volatile ("	sw			$t9,-3072($s0)			");	/*(0xBF80F400) = t9	: NVMCON = t9					*/
	asm volatile ("	sw			$t7,-3056($t4)			");	/*(0xBF80F410) = t7	: NVMKEY = 0xaa996655			*/
	asm volatile ("	sw			$t5,-3056($t4)			");	/*(0xBF80F410) = t5	: NVMKEY = 0x556699aa			*/
	asm volatile ("	sw			$t3,-3064($t2)			");	/*(0xBF80F400) = t3	: NVMKEY = 0x8000				*/
	asm volatile ("										");
	asm volatile ("	/*Wait for operation to	complete*/	");	/*													*/
	asm volatile ("_wt_op_end:							");	 /*													*/
	asm volatile ("	lw			$v0,-3072($s0)			");	/*v0 = NVMCON(0xBF80F400)							*/
	asm volatile ("	andi		$ra,$v0,0x8000			");	/*NVMCON: bit15	- WREN,	0 =	finish					*/
	asm volatile ("	bne			$ra,$zero,_wt_op_end	");	/*													*/
	asm volatile ("	addiu		$v1,$zero,16384			");	/*0x4000											*/
	asm volatile ("	lui			$a0,0xbf81				");	/*													*/
	asm volatile ("	sw			$v1,-3068($a0)			");	/*(NVMCONCLR) =	v1									*/
	asm volatile ("	beq			$a3,$zero,_exit_nvm_op	");	/*jump if DMA is normal								*/
	asm volatile ("	addiu		$t5,$zero,4096			");	/*t5 = 0x1000										*/
	asm volatile ("	lw			$a1,12288($a2)			");	/*a1 = DMACON (0xbf883000)							*/
	asm volatile ("	ext			$a3,$a1,12,1			");	/*DMACON: bit12	- DMA suspend(0	= normal)			*/
	asm volatile ("	bne			$a3,$zero,5f			");	/*jump if DMA is suspend							*/
	asm volatile ("	addiu		$t2,$zero,4096			");	/*t2 = 0x1000										*/
	asm volatile ("	lui			$t1,0xbf88				");	/*													*/
	asm volatile ("	sw			$t2,12296($t1)			");	/*DMACONSET(0xbf883008)								*/
	asm volatile ("2:									");	/*													*/
	asm volatile ("	lw			$t4,12288($a2)			");	/*													*/
	asm volatile ("	ext			$t3,$t4,11,1			");	/*													*/
	asm volatile ("	bne			$t3,$zero,2b			");	/*													*/
	asm volatile ("	nop									");	/*can not delete this instruction!					*/
	asm volatile ("5:									");
	asm volatile ("	b			_nvm_op_end				");	/*													*/

	asm volatile ("_exit_nvm_op:						");	/*													*/
	asm volatile ("	lui			$a2,0xbf88				");	/*													*/
	asm volatile ("	sw			$t5,12292($a2)			");	/*DMACONCLR(0xbf883004)								*/

	asm volatile ("_nvm_op_end:							");	/*													*/
	asm volatile ("	addu		$a0,$t0,$zero			");	/*													*/
	asm volatile ("	jal			bios_ResInt				");	/*restore int :	"di" or	"ei"						*/
	asm volatile ("	nop									");	/*													*/
	asm volatile ("	lw			$t0,-3072($s0)			");	/*t0 = NVMCON (s0 =	0xbf810000)						*/
	asm volatile ("	lw			$ra,20($sp)				");	/*pop ra											*/
	asm volatile ("	lw			$s0,16($sp)				");	/*pop s0											*/
	asm volatile ("	andi		$v0,$t0,0x3000			");	/*v0 = WRERR | LVDERR								*/
	asm volatile ("	addiu		$sp,$sp,24				");	/*													*/
	//asm volatile ("	jr			$ra                 ");
	//asm volatile ("	nop								");
}

//===============================================
/*
擦除整个PFM 区域。该模式会将引导闪存保持原样
PFM 擦除序列包含以下步骤：
1. 使用程序闪存存储器擦除命令运行解锁序列。
2. 擦除序列完成， WR 位（NVMCON<15>）由硬件清零。

   note: 未测试 !!!!!!!!!!!!!
*/
int32u bios_NVMErasePFM(void)
{
	int32u res;

	// Unlock and Erase Program Flash
	res = bios_NVMOperation(0x4005);

	// Return Result
	return res;
}

/********************************************************************
 *																	*
 *					升级功能函数									*
 *																	*
 *******************************************************************/
//将接收的代码复制到程序区
//注意 BOOT FLASH 的代码保存在程序闪存中
void  bios_CopyBakFlash(tUpdate_Tx_Msg *pTxMsg)
{
	int8u	TmpCode[INST_ROW_SIZE+4];	//指令缓存，用于读取128条指令
	void	*pSrc, *pDst;
	int32u	u32Tmp,u32Ret,u32Cnt;
	char  strMsg[16];

	LedOff(pTxMsg);

  //Tx "ERASE:M "
	strMsg[0] = 'E';
	strMsg[1] = 'R';
	strMsg[2] = 'A';
	strMsg[3] = 'S';
	strMsg[4] = 'E';
	strMsg[5] = ':';
	strMsg[6] = 'M';
	strMsg[7] = ' ';
	bios_TxMsg(pTxMsg, strMsg, 8);

	//copy flash code
	//两个区域的空间大小都是 4KB 的整数倍, 因此此处程序没有做特殊处理

	//****** erase main flash ******
	pDst = (void*)MAIN_UPDATE_ADDR;
	for( u32Tmp=0,u32Cnt=0; u32Tmp < (MAIN_UPDATE_SIZE/FLASH_PAGE_SIZE); u32Cnt++ )
	{
		if((u32Cnt & 0x03L) == 0)
		{
			LedToggle(pTxMsg);
		}
		//for(u32Ret = 0 ; u32Ret < 1000; u32Ret++);

		u32Ret = Flash_ErasePage( pDst );
		bios_TxInt(pTxMsg, u32Tmp, strMsg);

		if(u32Ret == 0)
		{
			u32Tmp++;
			pDst += FLASH_PAGE_SIZE;
		}
	}

	//copy code(main code)
	//Tx "COPY:M "
	strMsg[0] = 'C';
	strMsg[1] = 'O';
	strMsg[2] = 'P';
	strMsg[3] = 'Y';
	strMsg[4] = ':';
	strMsg[5] = 'M';
	strMsg[6] = ' ';
	strMsg[7] = ' ';
	bios_TxMsg(pTxMsg, strMsg, 8);

	pDst = (void*)MAIN_UPDATE_ADDR;
	pSrc = (void*)(DOWN_CODE_ADDR + FLASH_BOOT_SIZE);
	for( u32Tmp=0,u32Cnt=0; u32Tmp < (MAIN_UPDATE_SIZE/INST_ROW_SIZE); u32Cnt++ )
	{
		if((u32Cnt & 0x3FL) == 0)
		{
			LedToggle(pTxMsg);
		}
		//for(u32Ret = 0 ; u32Ret < 100; u32Ret++);

		//read then write code
		bios_memcpy((void*)TmpCode,pSrc,(INST_ROW_SIZE));
		u32Ret = Flash_WriteRow(pDst, (void*)TmpCode);
		bios_TxInt(pTxMsg, u32Tmp, strMsg);

		if(u32Ret == 0)
		{
			pSrc += INST_ROW_SIZE;
			pDst += INST_ROW_SIZE;
			u32Tmp++;
		}
		else
		{
			u32Ret = u32Tmp;
		}
	}

	#ifndef NOT_UPGRADE_BFM
	//****** erase boot flash ******
	pDst = (void*)FLASH_BOOT_ADDR;
	for( u32Tmp = 0; u32Tmp < (FLASH_BOOT_SIZE/FLASH_PAGE_SIZE);  )
	{
		if((u32Tmp & 0x01L) == 0)
		{
			LedToggle(pTxMsg);
		}
		//for(u32Ret = 0 ; u32Ret < 1000; u32Ret++);

		u32Ret = Flash_ErasePage( pDst );
		if(u32Ret == 0)
		{
			u32Tmp++;
			pDst += FLASH_PAGE_SIZE;
		}
		else
		{
			u32Ret = u32Tmp;
		}
	}

	//copy code(boot code)
	pDst = (void*)FLASH_BOOT_ADDR;
	pSrc = (void*)DOWN_CODE_ADDR;
	for( u32Tmp = 0; u32Tmp < (FLASH_BOOT_SIZE/INST_ROW_SIZE);  )
	{
		if((u32Tmp & 0x07L) == 0)
		{
			LedToggle(pTxMsg);
		}
		//for(u32Ret = 0 ; u32Ret < 100; u32Ret++);

		//read then write code
		bios_memcpy((void*)TmpCode,pSrc,(INST_ROW_SIZE));
		u32Ret = Flash_WriteRow(pDst, (void*)TmpCode);
		if(u32Ret == 0)
		{
			pSrc += INST_ROW_SIZE;
			pDst += INST_ROW_SIZE;
			u32Tmp++;
		}
	}
	#endif //NOT_UPGRADE_BFM

	//finished to copy code
}

//=========================================================
//写入固定长度的Flash(128条指令)
//必须保证Flash空间已经被擦除过（因为不能行擦除，只能页擦除，1页=8行）
//dwSrc	: 需要写入的地址
//pCode	: 需要写入的内容
//cntN	: 写入失败的允许次数，
//		  0	 表示必须写入成功(如果写入始终不成功，出现死机现象)
//		  >= 0xFF 表示不需要校验
//return: TRUE = 写入成功
int8u  bios_WriteFlash(int32u dwSrc, int8u *pCode,	int16u cntN)
{
	int16u ErrCnt,wTmp;
	int8u RdCode[INST_ROW_SIZE+4];		  //指令缓存，用于读取128条指令
	int32u u32Ret;

	ErrCnt = 0;
_WrCodeVerify:
	//write code
	u32Ret = Flash_WriteRow((void*)dwSrc, (void*)pCode);
	if(cntN	>= 0xFF)
		return TRUE;

	//verify code
	if(bios_memcmp((void*)pCode, (void*)dwSrc, INST_ROW_SIZE) != 0)
	{
		if(cntN	== 0)
		{//不考虑写入失败的次数
			goto _WrCodeVerify;
		}
		else
		{
			ErrCnt ++;
			if(ErrCnt <	cntN)
				goto _WrCodeVerify;
			else
				return FALSE;
		}
	}

	return TRUE;
}

//=========================================================
//升级完成，记录升级成功状态，复位
void  bios_Finished(tUpFwState *pState, tUpdate_Tx_Msg *pTxMsg)
{
	int32u j;

	//finished to upgrade
	INT_DIS();


	bios_CopyBakFlash(pTxMsg);
	//INT_EN();


	pState->wChkSum		   = 0;
	pState->wValidFlg	   = EE_FLG_VALID;
	pState->wUpdateState   = UPFW_STATE_UPDATEOK;
	pState->wTotalSections = pState->wCurrentSections;
	pState->wCurrentSections = 0;
	bios_SaveState(pState);

	for(j=0; j<50000; j++);

	bios_SoftReset();			//here can not use lib function SoftReset()
	while(1);
}

//=========================================================
// Function:	bios_SaveState
// Description:	保存升级信息(只能保存在Flash中)
// Parameters:
//				pData     - 数据指针
// Return:		TRUE -- 成功保存
// State:
//
int8u  bios_SaveState(tUpFwState *pState)
{
	int8u *p,*p1;
	int8u tmp[FLASH_ROW_SIZE];
	int16u wSum,cnt;

	if((int32u)UPFW_STATE_ADDR & 0xFFFL)
	{	//地址没有对齐到 4K
		return FALSE;
	}

	pState->wChkSum = 0;
	p =	(int8u*)pState;

	for(wSum=0,cnt=0;cnt<sizeof(tUpFwState);cnt++)
		wSum +=	*p++;

	pState->wChkSum = wSum;

	//compare data to avoid erease flash frequently
	bios_memcpy((void*)tmp,(void*)UPFW_STATE_ADDR,FLASH_ROW_SIZE);
	p1 = (int8*)pState;
	p  = (int8*)(&tmp + (UPFW_STATE_ADDR - UPFW_FLASH_DATA_ADDR));
	for(cnt=sizeof(tUpFwState);cnt>0;cnt--)
	{
		if(*p++ != *p1++)
			break;
	}
	if(cnt == 0)
	{
		return TRUE;			//the data is the same!
	}

	//save to flash
	//read one ROW, and modify, then write back
	p1 = (int8*)pState;
	p  = (int8*)(&tmp + (UPFW_STATE_ADDR - UPFW_FLASH_DATA_ADDR));
	for(cnt=0;cnt<sizeof(tUpFwState);cnt++)
	{
		*p++ = *p1++;
	}

	cnt = (int16u)Flash_ErasePage((void*)UPFW_FLASH_DATA_ADDR);
	cnt|= (int16u)Flash_WriteRow ((void*)UPFW_FLASH_DATA_ADDR,(void*)(&tmp[0]));

	if(cnt == 0)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// Function:	bios_ReadState()
// Description:	读取升级信息
// Parameters:
// Return:
// State:
//
void  bios_ReadState(tUpFwState	*pState)
{
	int8u *p,cnt;
	int16u wSum;

	bios_memcpy((void*)pState,(void*)UPFW_STATE_ADDR,sizeof(tUpFwState));

	p =	(int8u*)(&pState->wValidFlg);
	if(pState->wValidFlg == EE_FLG_VALID)
	{
		for(wSum=0,cnt=0; cnt<sizeof(tUpFwState)-sizeof(pState->wChkSum); cnt++)
			wSum += *p++;

		if(wSum	!= pState->wChkSum)
			pState->wUpdateState = UPFW_STATE_NOUPDATE;
	}
	else
	{
		pState->wUpdateState = UPFW_STATE_NOUPDATE;
	}
}

/**********************************************************************
标准工作条件：2.3V 至3.6V （除非另外声明）
工作温度-40°C <= TA <= +85°C （工业级）

参数	编号
符号特性			最小值	典型值(1) 最大值	单位	条件
编程时的供电电流		— 		10 		— 		mA		—
字写周期				20 		— 		40 		us		—
行写周期（注2）			3		4.5		— 		ms		—
页擦除周期				20		—		—		ms		—
片擦除周期				80		—		—		ms		—
擦写次数				1000

注
	1： 除非另外声明，否则“典型值”栏中的数据都是在3.3V、25°C 的条件下给出的。
	2： 行编程时的最小SYSCLK 为4 MHz。行编程期间应小心谨慎，以减少总线活动，例如暂挂任何存储器到存
		储器的DMA 操作。如果需要重总线负载，可能必须选择总线矩阵仲裁模式2 （循环优先级）。默认仲裁模
		式是模式1 （CPU 具有最低优先级）。
	3： 请参见《PIC32MX 闪存编程规范》（DS61145G_CN），以了解编程和擦除周期期间的工作条件。
	4： 该参数仅应用于PIC32MX534/564/664/764 器件。该信息为初稿。
**********************************************************************/

//=========================================================
//Flash 编程
//必须保证Flash空间已经被擦除过（因为不能行擦除，只能页擦除，1页=8行）
//Addr	: 需要写入的地址(对齐到 DWORD)
//pCode	: 需要写入的内容(对齐到 DWORD)
//Len	: 写入内容的长度(BYTES)
//return: none
void  bios_NVMProgram(void *Addr, int32u *pCode, int32u Len)
{
	int32u u32S,u32Tmp,u32TmpData;
	int32u *pAdr, *pData;

	//考虑地址对齐 512字节 的情况
	u32S  = 512 - (int32u )Addr & 0x1FF;
	pAdr  = (int32u*)Addr;
	pData = (int32u*)pCode;
	while(u32S)
	{
		if(Len == 0)
		{
			return;
		}

		if(Len < 4)
		{
			u32Tmp = *pAdr;
			u32TmpData = *pData;
			switch(Len)
			{
				case 1:
					u32Tmp &= 0xFFFFFF00L;
					u32TmpData &= ~0xFFFFFF00L;
					break;
				case 2:
					u32Tmp &= 0xFFFF0000L;
					u32TmpData &= ~0xFFFF0000L;
					break;
				default:
					u32Tmp &= 0xFF000000L;
					u32TmpData &= ~0xFF000000L;
					break;
			}
			u32TmpData |= u32Tmp;

			Flash_Write32((void*)pAdr, u32TmpData);
			return;
		}

		Flash_Write32((void*)pAdr, *pData);
/*
		u32TmpData = *pAdr;
		if(u32TmpData != *pData)
		{
			u32TmpData = *pData;
		}
*/
		pAdr	++;
		pData	++;
		Len		-=4;
		u32S	-=4;
	}

	while(Len >= INST_ROW_SIZE)
	{
		Flash_WriteRow((void*)pAdr, (void*)pData);
		Len  -= INST_ROW_SIZE;
		pAdr += INST_PER_ROW;
		pData+= INST_PER_ROW;
	}

	while(Len)
	{
		if(Len < 4)
		{
			u32Tmp = *pAdr;
			u32TmpData = *pData;
			switch(Len)
			{
				case 1:
					u32Tmp &= 0xFFFFFF00L;
					u32TmpData &= ~0xFFFFFF00L;
					break;
				case 2:
					u32Tmp &= 0xFFFF0000L;
					u32TmpData &= ~0xFFFF0000L;
					break;
				default:
					u32Tmp &= 0xFF000000L;
					u32TmpData &= ~0xFF000000L;
					break;
			}
			u32TmpData |= u32Tmp;

			Flash_Write32((void*)pAdr, u32TmpData);
			return;
		}

		Flash_Write32((void*)pAdr, *pData);
		pAdr	++;
		pData	++;
		Len		-=4;
	}
}

//===============================================
/*
说明： memcpy 从源缓冲区src 复制n 个字符到目的缓冲区dst。 如果缓冲区重叠，则操作未定义。
参数：
	dst 将字符复制到的目的缓冲区
	src 从其中复制字符的源缓冲区
	n 要复制的字符数
返回值：
	返回dst。
*/
void *bios_memcpy(void *dst , const void *src , int32u n)
{
	int8u *p1,*p2;

	p1 = (int8u*)dst;
	p2 = (int8u*)src;
	while(n!=0)
	{
		*p1++ = *p2++;
		n--;
	}
}

//===============================================
/*
参数：
	s1 第一个缓冲区
	s2 第二个缓冲区
	n 要比较的字符数
返回值：
	如果s1 大于s2，返回一个正数；如果s1 等于s2，返回0 ；如果s1小于s2，则返回一个负数。
说明：
	该函数将s1 中的前n 个字符与s2 中的前n 个字符相比较，返回一个
	值表明其中一个缓冲区小于、等于还是大于另外一个缓冲区。
*/
int32u bios_memcmp(const void *s1, const void *s2, int32u n)
{
	int8u *pS = (int8u*)s1;
	int8u *pD = (int8u*)s2;

	int8u i,j;
	while(n!=0)
	{
		i = *pS;
		j = *pD;
		if(*pS != *pD)
		{
			return (pS - pD);
		}
		pS++;
		pD++;
		n--;
	}

	return 0;
}

//===============================================
//UART 传送信息不是必要的，因此必须防止在此处死循环
void bios_TxMsg(tUpdate_Tx_Msg *pTxMsg, char *buffer, int32u size)
#if (ENABLE_UART_MSG == 1)
{
	int i;

	if(pTxMsg->UartEn == 0)	//disable UART output msg
	{
	  return;
	}

	while(size)
	{
		for(i=0; i<1000; i++)
		{
			if(IF_UART_TX_READY())
				break;
		}
		if(i == 1000)
			return;

	    UART_TX_BYTE(*buffer);

	    buffer++;
	    size--;
	}

	for(i=0; i<1000; i++)
	{
		if(IF_UART_TX_FINISHED())
			break;
	}
}
#else
	{
	}
#endif

//===============================================
void bios_TxInt(tUpdate_Tx_Msg *pTxMsg, int32u Data, char *p)
{
	char *p1 = p;

	*p++ = '0';
	*p++ = 'x';
//	*p++ = bios_NibbleToAsc(Data >>28);
//	*p++ = bios_NibbleToAsc(Data >>24);
//	*p++ = bios_NibbleToAsc(Data >>20);
//	*p++ = bios_NibbleToAsc(Data >>16);
	*p++ = bios_NibbleToAsc(Data >>12);
	*p++ = bios_NibbleToAsc(Data >>8);
	*p++ = bios_NibbleToAsc(Data >>4);
	*p++ = bios_NibbleToAsc(Data >>0);
	*p++ = ' ';
	*p++ = ' ';
	bios_TxMsg(pTxMsg, p1,8);
}

int8u bios_NibbleToAsc(int8u data)
{
	data &= 0x0F;
	if(data > 9)
	{
		return (data+'A'-10);
	}
	else
	{
		return (data+'0');
	}
}

//===============================================
void LedOn(tUpdate_Tx_Msg *pLed)
{
	if(pLed->LedEn == 1){*(pLed->LedPort) &= pLed->LedBit;}
}

void LedOff(tUpdate_Tx_Msg *pLed)
{
	if(pLed->LedEn == 1){*(pLed->LedPort) |= pLed->LedBit;}
}

void LedToggle(tUpdate_Tx_Msg *pLed)
{
	if(pLed->LedEn == 1){*(pLed->LedPort) ^= pLed->LedBit;}
}




