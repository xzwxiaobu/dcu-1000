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
* ��	 ��	:	UpBios.C
*  ������������ģ��(BIOS �������̻���FLASHָ����ַ)
*  ���ļ������еĺ������ܱ�����
*  ���ļ��в��ܵ��� PIC32 �Դ��ĺ�����
* =============================================================================*/

#include <p32xxxx.h>
#include <peripheral/uart.h>

#include "_g_canbus.h"

#include "glbUpdate.h"
#include "Update.h"

//#include "plib.h"			//�˴����������ļ������������ϵͳ�Ŀ⺯��

/**********************************************/
//define = ������ BootFlash Memory
//         ���� BootFlash ���ɹ���ԭ����
//         ע���޸� linker �ļ�����������Ҫ�Ĵ������� BootFlash
//         �����鿴 HEX �ļ������� 020000041f Ӧֻ��1��������
//			:020000040000fa
//			:020000041fc01b
//			:10000000009d1a3c80205a27080040030000000091
//			���ָ�������
//			  1FC0_0000                lui         k0,0x9d00
//			  1FC0_0004                addiu       k0,k0,8320
//			  1FC0_0008                jr          k0
//			  1FC0_000C                nop

#define NOT_UPGRADE_BFM

/**********************************************/

//�ɱ�̵�������ݿ�Ϊ1	�У�����512	�ֽڵ����ݡ������б�����װ��SRAM �Ļ������С�
//����������������µĵ�ַλ�����Ǵ�����ʼλ�ô���ʼ��̡�
//һ��8�����һҳ����ˣ�������8 ��	512	= 4096 �ֽڻ�1024 ��ָ���֡�����ҳ���ڵ����пɲ�������С�洢����Ԫ

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
//                                        ���������еĹ̻�����                                           *
//                                                                                                       *
//********************************************************************************************************

/********************************************************************
 *																	*
 *		FLASH ROM �Ĳ���������NVM���������û�࣬C�����޷����		*
 *																	*
 *		�˲��ֳ������� PIC �⺯��									*
 *		����Ϊ�˽�����ŵ�ָ���Σ���û����PIC�ṩ�Ŀ⺯��			*
 *																	*
 *******************************************************************/
//===============================================
/*
˵����
	PIC32MX ��CPU �ں˲��ṩ�ض���RESET ָ����ǣ�Ӳ����λ�������ִ�У������λ����
	������ִ�������λ�������С������λ���������������MCLR ��λ�������λ����Ҫ����ִ
	��ϵͳ�������У�Ȼ�����д��SWRST λ������ϵͳ��������ϸ��Ϣ����μ���6.3.6 �ڡ� ʱ��
	�л������� ��DS61112���������λ��ִ�з�ʽ���£�
	1 д��ϵͳ��������
	2 ����SWRST λ��RSWRST<0>�� = 1
	3 ��ȡRSWRST �Ĵ���
	4 ������桰while(1);�� ��4 ����NOP�� ָ��
	��RSWRST �Ĵ���д��1 �ὫSWRST λ��1���Ӷ����������λ��RSWRST �Ĵ����ĺ�����
	�����ᴥ�������λ�����ڶ�����֮�����һ��ʱ�����ڷ�����Ҫȷ���ڷ�����λ�¼�֮ǰ��ִ
	���κ������û����룬���齫4 ����NOP�� ָ���while(1);�� ������READ ָ��֮��
	SWR ״̬λ��RCON<6>������1��ָʾ�����������λ��
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
дFlash��һ��˫�֣�32Bit�ǣ�
������а������²��裺
1. ��Ҫ��̵�32 λ����д��NVMDATA �Ĵ�����
2. ��NVMADDR �Ĵ�����װ��Ҫ��̵ĵ�ַ��
3. ʹ���ֱ���������н������С�
   ���������ɣ� WR λ��NVMCON<15>����Ӳ�����㡣
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
�б�����а������²��裺
1. ��Ҫ��̵���������д��ϵͳSRAM��Դ��ַ�����ֶ��롣
2. ʹ��Ҫ��̵������е���ʼ��ַ����NVMADDR �Ĵ�����
3. ʹ�����Բ���1 ������Դ��ַ����NVMSRCADDR �Ĵ�����
4. ʹ���б���������н������С�
5. ���������ɣ� WR λ��NVMCON<15>����Ӳ�����㡣
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
ҳ�������а������²��裺
1. ʹ��Ҫ����ҳ�ĵ�ַ����NVMADDR �Ĵ�����
2. ʹ������Ĳ����������н������С�
3. ����������ɣ� WR λ��NVMCON<15>����Ӳ�����㡣
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
Ҫ������������������ϸ���˳��ִ������Ĳ���4 ������9�����δ�ϸ��ո�����ִ�У���WR ������1��
1. ��ͣ���ֹ�ɷ����������ߺ��жϽ������е����з�����������DMA ���жϡ�
2. ��WREN λ��NVMCON<14>����1��������д��WR����ʹ�õ����洢ָ�NVMOP<3:0> λ��NVMCON<3:0>������Ϊ����Ĳ�����
3. �ȴ�LVD ������
4. ��CPU �Ĵ���X ��װ��0xAA996655��
5. ��CPU �Ĵ���Y ��װ��0x556699AA��
6. ��CPU �Ĵ���Z ��װ��0x00008000��
7. ��CPU �Ĵ���X �洢��NVMKEY �С�
8. ��CPU �Ĵ���Y �洢��NVMKEY �С�
9. ��CPU �Ĵ���Z �洢��NVMCONSET �С�
10. �ȴ�WR λ��NVMCON<15>�����㡣
11. ����WREN λ��NVMCON<14>����
12. ���WRERR��NVMCON<13>����LVDERR��NVMCON<12>��λ����ȷ�����/ �������гɹ���ɡ�
��WR λ��1 ʱ�����/ �������л��������ڸ������ڼ䣬 CPU �޷���������ִ�С�

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
	asm volatile ("	addu		$s0,$a0,$zero			");	/*����												*/
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
	asm volatile ("	ori			$t9,$s0,0x4000			");	/*t9 = 0x4000 |	����								*/
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
��������PFM ���򡣸�ģʽ�Ὣ�������汣��ԭ��
PFM �������а������²��裺
1. ʹ�ó�������洢�������������н������С�
2. ����������ɣ� WR λ��NVMCON<15>����Ӳ�����㡣

   note: δ���� !!!!!!!!!!!!!
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
 *					�������ܺ���									*
 *																	*
 *******************************************************************/
//�����յĴ��븴�Ƶ�������
//ע�� BOOT FLASH �Ĵ��뱣���ڳ���������
void  bios_CopyBakFlash(tUpdate_Tx_Msg *pTxMsg)
{
	int8u	TmpCode[INST_ROW_SIZE+4];	//ָ��棬���ڶ�ȡ128��ָ��
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
	//��������Ŀռ��С���� 4KB ��������, ��˴˴�����û�������⴦��

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
//д��̶����ȵ�Flash(128��ָ��)
//���뱣֤Flash�ռ��Ѿ�������������Ϊ�����в�����ֻ��ҳ������1ҳ=8�У�
//dwSrc	: ��Ҫд��ĵ�ַ
//pCode	: ��Ҫд�������
//cntN	: д��ʧ�ܵ����������
//		  0	 ��ʾ����д��ɹ�(���д��ʼ�ղ��ɹ���������������)
//		  >= 0xFF ��ʾ����ҪУ��
//return: TRUE = д��ɹ�
int8u  bios_WriteFlash(int32u dwSrc, int8u *pCode,	int16u cntN)
{
	int16u ErrCnt,wTmp;
	int8u RdCode[INST_ROW_SIZE+4];		  //ָ��棬���ڶ�ȡ128��ָ��
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
		{//������д��ʧ�ܵĴ���
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
//������ɣ���¼�����ɹ�״̬����λ
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
// Description:	����������Ϣ(ֻ�ܱ�����Flash��)
// Parameters:
//				pData     - ����ָ��
// Return:		TRUE -- �ɹ�����
// State:
//
int8u  bios_SaveState(tUpFwState *pState)
{
	int8u *p,*p1;
	int8u tmp[FLASH_ROW_SIZE];
	int16u wSum,cnt;

	if((int32u)UPFW_STATE_ADDR & 0xFFFL)
	{	//��ַû�ж��뵽 4K
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
// Description:	��ȡ������Ϣ
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
��׼����������2.3V ��3.6V ����������������
�����¶�-40��C <= TA <= +85��C ����ҵ����

����	���
��������			��Сֵ	����ֵ(1) ���ֵ	��λ	����
���ʱ�Ĺ������		�� 		10 		�� 		mA		��
��д����				20 		�� 		40 		us		��
��д���ڣ�ע2��			3		4.5		�� 		ms		��
ҳ��������				20		��		��		ms		��
Ƭ��������				80		��		��		ms		��
��д����				1000

ע
	1�� �����������������򡰵���ֵ�����е����ݶ�����3.3V��25��C �������¸����ġ�
	2�� �б��ʱ����СSYSCLK Ϊ4 MHz���б���ڼ�ӦС�Ľ������Լ������߻�������ݹ��κδ洢������
		������DMA �����������Ҫ�����߸��أ����ܱ���ѡ�����߾����ٲ�ģʽ2 ��ѭ�����ȼ�����Ĭ���ٲ�ģ
		ʽ��ģʽ1 ��CPU ����������ȼ�����
	3�� ��μ���PIC32MX �����̹淶����DS61145G_CN�������˽��̺Ͳ��������ڼ�Ĺ���������
	4�� �ò�����Ӧ����PIC32MX534/564/664/764 ����������ϢΪ���塣
**********************************************************************/

//=========================================================
//Flash ���
//���뱣֤Flash�ռ��Ѿ�������������Ϊ�����в�����ֻ��ҳ������1ҳ=8�У�
//Addr	: ��Ҫд��ĵ�ַ(���뵽 DWORD)
//pCode	: ��Ҫд�������(���뵽 DWORD)
//Len	: д�����ݵĳ���(BYTES)
//return: none
void  bios_NVMProgram(void *Addr, int32u *pCode, int32u Len)
{
	int32u u32S,u32Tmp,u32TmpData;
	int32u *pAdr, *pData;

	//���ǵ�ַ���� 512�ֽ� �����
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
˵���� memcpy ��Դ������src ����n ���ַ���Ŀ�Ļ�����dst�� ����������ص��������δ���塣
������
	dst ���ַ����Ƶ���Ŀ�Ļ�����
	src �����и����ַ���Դ������
	n Ҫ���Ƶ��ַ���
����ֵ��
	����dst��
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
������
	s1 ��һ��������
	s2 �ڶ���������
	n Ҫ�Ƚϵ��ַ���
����ֵ��
	���s1 ����s2������һ�����������s1 ����s2������0 �����s1С��s2���򷵻�һ��������
˵����
	�ú�����s1 �е�ǰn ���ַ���s2 �е�ǰn ���ַ���Ƚϣ�����һ��
	ֵ��������һ��������С�ڡ����ڻ��Ǵ�������һ����������
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
//UART ������Ϣ���Ǳ�Ҫ�ģ���˱����ֹ�ڴ˴���ѭ��
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




