
#include <p32xxxx.h>


.section SEG_UPGRADE_BIOS


/* ===========================
unsigned int bios_EnInt();
	enables the core to handle any pending interrupt requests.
   ===========================*/
bios_EnInt:
		jr			$ra
		ei			$v0

/* ===========================
unsigned int bios_DisInt(void)
	disables the core from handling any pending interrupt requests
   ===========================*/
bios_DisInt:
		jr			$ra
		di			$v0

/* ===========================
void bios_ResInt(unsigned int status)
	restores the core to the previous interrupt handling state.
   ===========================*/
bios_ResInt:
		andi		$a0,$a0,0x1
		beq			$a0,$zero,1f
		nop
		jr			$ra
		ei
	1:
		jr			$ra
		di

/* ===========================
void bios_NVMClearError(void)
   ===========================*/
bios_NVMClearError:
	addiu		$sp,$sp,-24
	sw			$ra,16($sp)

	addiu		$a0,$zero,0x4000
	/*jal		bios_NVMOperation				*/
	nop

	lw			$ra,16($sp)
	addiu		$sp,$sp,24
	jr			$ra
	nop

/* ===========================
unsigned int bios_NVMErasePage(void* address)
	input :	$a0	= address
	return '0' if operation	completed successfully.
	NVMADDR	= BF80F420
	address  Destination page address to Erase 
   ===========================*/
bios_NVMErasePage:
	addiu		$sp,$sp,-24
	sw			$ra,16($sp)

	li			$a1,0x1fffffff
	and			$v1,$a0,$a1
	la			$v0,NVMADDR
	sw			$v1,0($v0)

	addiu		$a0,$zero,0x4004
	/*jal		bios_NVMOperation*/
	nop

	lw			$ra,16($sp)
	addiu		$sp,$sp,24
	jr			$ra
	nop

/* ===========================
void bios_NVMOperation(int opcode)
	input :	$a0	= address
	return '0' if operation	completed successfully.
	NVMADDR	= BF80F420
   ===========================*/
bios_NVMOperation:
	asm("	addiu		$sp,$sp,-24				");	
	asm("	sw			$s0,16($sp)				");	/*push s0											*/
	asm("	sw			$ra,20($sp)				");	/*push ra											*/
	asm("	jal			bios_DisInt				");	/*disable interrupt: return	v0						*/
	asm("										");	
	asm("	addu		$s0,$a0,$zero			");	/*参数												*/
	asm("	lui			$a2,0xbf88				");	/*													*/
	asm("	addu		$t0,$v0,$zero			");	/*t0 = v0											*/
	asm("	lw			$v0,12288($a2)			");	/*v0 = (0xbf883000)	(reg:DMACON)					*/
	asm("	ext			$a3,$v0,12,1			");	/*a3 = v0(bit12): DMA suspend(0	= normal)			*/
	asm("	bne			$a3,$zero,_wr_nvmkey	");	/*jump if DMA is suspend							*/
	asm("	lui			$t8,0xaa99				");	/*													*/
	asm("	addiu		$v1,$zero,4096			");	/*v1 =	0x1000										*/
	asm("	lui			$a0,0xbf88				");	/*													*/
	asm("	sw			$v1,12296($a0)			");	/*(0xbf883008) = $v1 (reg: DMACONSET)				*/
	asm("1:										");	/*													*/
	asm("	lw			$t1,12288($a2)			");	/*t1 =	(0xbf883000) (reg:DMACON)					*/
	asm("	ext			$a1,$t1,11,1			");	/*a1 =	$t1(bit11) : DMA busy						*/
	asm("	bne			$a1,$zero,1b			");	/*													*/
	asm("_wr_nvmkey:							");	 /*													 */
	asm("	lui			$t8,0xaa99				");	/*t8 = 0xaa990000									*/
	asm("	lui			$t6,0x5566				");	/*t6 = 0x55660000									*/
	asm("	ori			$t9,$s0,0x4000			");	/*t9 = 0x4000 |	参数								*/
	asm("	lui			$t4,0xbf81				");	/*t4 = 0xbf810000									*/
	asm("	lui			$s0,0xbf81				");	/*s0 = 0xbf810000									*/
	asm("	ori			$t7,$t8,0x6655			");	/*t7 = 0xaa996655									*/
	asm("	ori			$t5,$t6,0x99aa			");	/*t5 = 0x556699aa									*/
	asm("	ori			$t3,$zero,0x8000		");	/*t3 = 0x8000										*/
	asm("	lui			$t2,0xbf81				");	/*t2 = 0xbf810000									*/
	asm("	sw			$t9,-3072($s0)			");	/*(0xBF80F400) = t9	: NVMCON = t9					*/
	asm("	sw			$t7,-3056($t4)			");	/*(0xBF80F410) = t7	: NVMKEY = 0xaa996655			*/
	asm("	sw			$t5,-3056($t4)			");	/*(0xBF80F410) = t5	: NVMKEY = 0x556699aa			*/
	asm("	sw			$t3,-3064($t2)			");	/*(0xBF80F400) = t3	: NVMKEY = 0x8000				*/
	asm("										");	
	asm("	/*Wait for operation to	complete*/	");	/*													*/
	asm("_wt_op_end:							");	 /*													*/
	asm("	lw			$v0,-3072($s0)			");	/*v0 = NVMCON(0xBF80F400)							*/
	asm("	andi		$ra,$v0,0x8000			");	/*NVMCON: bit15	- WREN,	0 =	finish					*/
	asm("	bne			$ra,$zero,_wt_op_end	");	/*													*/
	asm("	addiu		$v1,$zero,16384			");	/*0x4000											*/
	asm("	lui			$a0,0xbf81				");	/*													*/
	asm("	sw			$v1,-3068($a0)			");	/*(NVMCONCLR) =	v1									*/
	asm("	beq			$a3,$zero,_exit_nvm_op	");	/*jump if DMA is normal								*/
	asm("	addiu		$t5,$zero,4096			");	/*t5 = 0x1000										*/
	asm("	lw			$a1,12288($a2)			");	/*a1 = DMACON (0xbf883000)							*/
	asm("	ext			$a3,$a1,12,1			");	/*DMACON: bit12	- DMA suspend(0	= normal)			*/
	asm("	bne			$a3,$zero,5f			");	/*jump if DMA is suspend							*/
	asm("	addiu		$t2,$zero,4096			");	/*t2 = 0x1000										*/
	asm("	lui			$t1,0xbf88				");	/*													*/
	asm("	sw			$t2,12296($t1)			");	/*DMACONSET(0xbf883008)								*/
	asm("2:										");	/*													*/
	asm("	lw			$t4,12288($a2)			");	/*													*/
	asm("	ext			$t3,$t4,11,1			");	/*													*/
	asm("	bne			$t3,$zero,2b			");	/*													*/
	asm("	nop									");	/*can not delete this instruction!					*/
	asm("										");	
	asm("5:										");	
	asm("	jal			bios_ResInt				");	/*restore int :	"di" or	"ei"						*/
	asm("	addu		$a0,$t0,$zero			");	/*													*/
	asm("	lw			$t0,-3072($s0)			");	/*t0 = NVMCON (s0 =	0xbf810000)						*/
	asm("	lw			$ra,20($sp)				");	/*pop ra											*/
	asm("	lw			$s0,16($sp)				");	/*pop s0											*/
	asm("	andi		$v0,$t0,0x3000			");	/*v0 = WRERR | LVDERR								*/
	asm("	jr			$ra						");	/*return v0											*/
	asm("	addiu		$sp,$sp,24				");	/*													*/
	asm("										");	/*													*/
	asm("_exit_nvm_op:							");	 /*													*/
	asm("	lui			$a2,0xbf88				");	/*													*/
	asm("	addu		$a0,$t0,$zero			");	/*													*/
	asm("	sw			$t5,12292($a2)			");	/*DMACONCLR(0xbf883004)								*/
	asm("										");	
	asm("	jal			bios_ResInt				");	/*restore int :	"di" or	"ei"						*/
	asm("	nop									");	/*													*/
	asm("	lw			$t0,-3072($s0)			");	/*t0 = NVMCON (s0 =	0xbf810000)						*/
	asm("	lw			$ra,20($sp)				");	/*pop ra											*/
	asm("	lw			$s0,16($sp)				");	/*pop s0											*/
	asm("	andi		$v0,$t0,0x3000			");	/*v0 = WRERR | LVDERR								*/
	asm("	jr			$ra						");	/*return v0											*/
	asm("	addiu		$sp,$sp,24				");	/*													*/

/* ===========================
	unsigned int bios_NVMWriteRow(void* address, void* data)
	address  Destination Row address to write.  
	data  Location of data to write.  
	'0' if operation completed successfully. 
   ===========================*/
 bios_NVMWriteRow:
 		lui			$t0,0x1fff
 		ori			$a3,$t0,0xffff
 		addiu		$sp,$sp,-24
		and			$a2,$a0,$a3
		and			$a1,$a1,$a3
		lui			$v0,0xbf81
		lui			$v1,0xbf81
		addiu		$a0,$zero,16387		/*0x4003					*/
		sw			$ra,16($sp)
		sw			$a2,-3040($v0)		/*NVMADDR(0xBF80F420)		*/
		sw			$a1,-3008($v1)		/*NVMSRCADDR(0xBF80F440)	*/
		jal			bios_NVMOperation
		nop
		lw			$ra,16($sp)
		jr			$ra
		addiu		$sp,$sp,24

/* ===========================
unsigned int bios_NVMWriteWord(void* address, unsigned int data);
	address  Destination address to write.  
	data  Word to write.  
	'0' if operation completed successfully. 
   ===========================*/
bios_NVMWriteWord:
		lui			$t0,0x1fff
		ori			$a3,$t0,0xffff
		addiu		$sp,$sp,-24
		and			$a2,$a0,$a3
		lui			$v0,0xbf81
		lui			$v1,0xbf81
		addiu		$a0,$zero,16385		/*0x4001					*/
		sw			$ra,16($sp)
		sw			$a2,-3040($v0)		/*NVMADDR(0xBF80F420)		*/
		sw			$a1,-3024($v1)		/*NVMDATA(0xBF80F430)		*/
		jal			bios_NVMOperation
		nop     	
		lw			$ra,16($sp)
		jr			$ra
		addiu		$sp,$sp,24

/* ===========================
unsigned int NVMProgram(void* address, const void* data, unsigned int size void * pagebuff)
	address  Destination address to start writing from.  
	data  Location of data to write.  
	size  Number of bytes to write.  
	pagebuff  Ram buffer to preserve 1 page worth of data  
	'0' if operation completed successfully. 
   ===========================*/
 /*
 43878 1D00_AD8C   9D00_AD8C   NVMProgram		  addiu		  $sp,$sp,-48
 43879 1D00_AD90   9D00_AD90					  andi		  $v0,$a2,0x3
 43880 1D00_AD94   9D00_AD94					  sw		  $s6,40($sp)
 43881 1D00_AD98   9D00_AD98					  sw		  $s5,36($sp)
 43882 1D00_AD9C   9D00_AD9C					  sw		  $s4,32($sp)
 43883 1D00_ADA0   9D00_ADA0					  sw		  $s3,28($sp)
 43884 1D00_ADA4   9D00_ADA4					  sw		  $ra,44($sp)
 43885 1D00_ADA8   9D00_ADA8					  sw		  $s2,24($sp)
 43886 1D00_ADAC   9D00_ADAC					  sw		  $s1,20($sp)
 43887 1D00_ADB0   9D00_ADB0					  sw		  $s0,16($sp)
 43888 1D00_ADB4   9D00_ADB4					  addu		  $s3,$a2,$zero
 43889 1D00_ADB8   9D00_ADB8					  addu		  $s5,$a0,$zero
 43890 1D00_ADBC   9D00_ADBC					  addu		  $s6,$a1,$zero
 43891 1D00_ADC0   9D00_ADC0					  bne		  $v0,$zero,0x1d00add4
 43892 1D00_ADC4   9D00_ADC4					  addu		  $s4,$a3,$zero
 43893 1D00_ADC8   9D00_ADC8					  andi		  $v1,$a3,0x3
 43894 1D00_ADCC   9D00_ADCC					  beq		  $v1,$zero,0x1d00ae00
 43895 1D00_ADD0   9D00_ADD0					  nop
 43896 1D00_ADD4   9D00_ADD4					  addiu		  $v0,$zero,1
 43897 1D00_ADD8   9D00_ADD8					  lw		  $ra,44($sp)
 43898 1D00_ADDC   9D00_ADDC					  lw		  $s6,40($sp)
 43899 1D00_ADE0   9D00_ADE0					  lw		  $s5,36($sp)
 43900 1D00_ADE4   9D00_ADE4					  lw		  $s4,32($sp)
 43901 1D00_ADE8   9D00_ADE8					  lw		  $s3,28($sp)
 43902 1D00_ADEC   9D00_ADEC					  lw		  $s2,24($sp)
 43903 1D00_ADF0   9D00_ADF0					  lw		  $s1,20($sp)
 43904 1D00_ADF4   9D00_ADF4					  lw		  $s0,16($sp)
 43905 1D00_ADF8   9D00_ADF8					  jr		  $ra
 43906 1D00_ADFC   9D00_ADFC					  addiu		  $sp,$sp,48
 43907 1D00_AE00   9D00_AE00					  beq		  $a2,$zero,0x1d00add8
 43908 1D00_AE04   9D00_AE04					  addu		  $v0,$zero,$zero
 43909 1D00_AE08   9D00_AE08					  addiu		  $a0,$zero,-4096
 43910 1D00_AE0C   9D00_AE0C					  and		  $s1,$s5,$a0
 43911 1D00_AE10   9D00_AE10					  andi		  $s2,$s5,0xfff
 43912 1D00_AE14   9D00_AE14					  addu		  $a0,$a3,$zero
 43913 1D00_AE18   9D00_AE18					  addu		  $a1,s1,$zero
 43914 1D00_AE1C   9D00_AE1C					  jal		  0x1d00b09c
 43915 1D00_AE20   9D00_AE20					  addu		  $a2,s2,$zero
 43916 1D00_AE24   9D00_AE24					  addiu		  $a2,$zero,4096
 43917 1D00_AE28   9D00_AE28					  subu		  $s0,$a2,s2
 43918 1D00_AE2C   9D00_AE2C					  sltu		  $a1,$s0,s3
 43919 1D00_AE30   9D00_AE30					  bne		  $a1,$zero,0x1d00aeac
 43920 1D00_AE34   9D00_AE34					  addu		  $a1,s6,$zero
 43921 1D00_AE38   9D00_AE38					  addu		  $s2,$s4,$s2
 43922 1D00_AE3C   9D00_AE3C					  addu		  $a2,s3,$zero
 43923 1D00_AE40   9D00_AE40					  addu		  $a0,s2,$zero
 43924 1D00_AE44   9D00_AE44					  jal		  0x1d00b09c
 43925 1D00_AE48   9D00_AE48					  addu		  $a1,s6,$zero
 43926 1D00_AE4C   9D00_AE4C					  subu		  $a2,$s0,s3
 43927 1D00_AE50   9D00_AE50					  bne		  $a2,$zero,0x1d00ae9c
 43928 1D00_AE54   9D00_AE54					  addu		  $a0,s2,s3
 43929 1D00_AE58   9D00_AE58					  addu		  s3,$zero,$zero
 43930 1D00_AE5C   9D00_AE5C					  jal		  0x1d00ac4c		/*NVMErasePage
 43931 1D00_AE60   9D00_AE60					  addu		  $a0,s1,$zero
 43932 1D00_AE64   9D00_AE64					  addu		  $s0,$zero,$zero
 43933 1D00_AE68   9D00_AE68					  sll		  $t0,$s0,0x9
 43934 1D00_AE6C   9D00_AE6C					  addu		  $a1,$t0,s4
 43935 1D00_AE70   9D00_AE70					  addiu		  $s0,$s0,1
 43936 1D00_AE74   9D00_AE74					  jal		  0x1d00aec8		/*NVMWriteRow
 43937 1D00_AE78   9D00_AE78					  addu		  $a0,$t0,s1
 43938 1D00_AE7C   9D00_AE7C					  sltiu		  $a3,$s0,8
 43939 1D00_AE80   9D00_AE80					  bne		  $a3,$zero,0x1d00ae6c
 43940 1D00_AE84   9D00_AE84					  sll		  $t0,$s0,0x9
 43941 1D00_AE88   9D00_AE88					  addu		  $s2,$zero,$zero
 43942 1D00_AE8C   9D00_AE8C					  bne		  $s3,$zero,0x1d00ae24
 43943 1D00_AE90   9D00_AE90					  addu		  $s1,$s5,$zero
 43944 1D00_AE94   9D00_AE94					  beq		  $zero,$zero,0x1d00add8
 43945 1D00_AE98   9D00_AE98					  addu		  $v0,$zero,$zero
 43946 1D00_AE9C   9D00_AE9C					  jal		  0x1d00b09c
 43947 1D00_AEA0   9D00_AEA0					  addu		  $a1,$s5,$s3
 43948 1D00_AEA4   9D00_AEA4					  beq		  $zero,$zero,0x1d00ae5c
 43949 1D00_AEA8   9D00_AEA8					  addu		  $s3,$zero,$zero
 43950 1D00_AEAC   9D00_AEAC					  addu		  $a0,s4,s2
 43951 1D00_AEB0   9D00_AEB0					  jal		  0x1d00b09c
 43952 1D00_AEB4   9D00_AEB4					  addu		  $a2,$s0,$zero
 43953 1D00_AEB8   9D00_AEB8					  addu		  $s6,$s6,$s0
 43954 1D00_AEBC   9D00_AEBC					  subu		  $s3,$s3,$s0
 43955 1D00_AEC0   9D00_AEC0					  beq		  $zero,$zero,0x1d00ae5c
 43956 1D00_AEC4   9D00_AEC4					  addu		  $s5,$s5,$s0
*/