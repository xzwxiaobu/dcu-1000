/*************************************************************************
 * Processor-specific object file.  Contains SFR definitions.
 *************************************************************************/
INPUT("processor.o")

/*************************************************************************
 * For interrupt vector handling
 *************************************************************************/
PROVIDE(_vector_spacing = 0x00000001);
_ebase_address = 0x9D03B000;	/*256K=1B000,512K=3B000*/

_DATA_ADDR     = 0xA0000200;
_BSS_ADDR      = 0xA0000600;

/*************************************************************************
 * Memory Address Equates
 *************************************************************************/
	_RESET_ADDR             = 0xBFC00000;
	_BEV_EXCPT_ADDR         = 0xBFC00380;
	_DBG_EXCPT_ADDR         = 0xBFC00480;	/*do not care because it is not been loaded*/
	_DBG_CODE_ADDR          = 0xBFC02000;	/*do not care because it is not been loaded*/
	_GEN_EXCPT_ADDR         = _ebase_address + 0x180;

	_kseg0_main_flash_data  = 0x9D000000;
	_exception_mem          = 0x9D03B000;               /*256K=1B000,512K=3B000*/

	_kseg0_program_sw_info		= 0x9D003000;
	_kseg0_program_mem		= 0x9D003080;
	_kseg0_upgrade_dnld		= 0x9D03C000;              /*256K=1C000,512K=3C000*/
	_kseg0_upgrade_bios		= 0x9D07D000;              /*256K=3D000,512K=7D000*/
	_kseg0_upgrade_bios_d	= 0x9D07F000;              /*256K=3F000,512K=7F000*/

	_kseg1_fix_data_mem		= 0xA0000000;
	_kseg1_data_mem			= 0xA0000200;
	_kseg1_upgrade_ram  = 0xA000FF00;

	/*wtj interrupt*/
/* 2020-1-21 modified
	_kseg0_program_U2artRXHandler = 0x9D001000;
	_kseg0_program_U3artRXHandler = 0x9D001800;
	_kseg0_program_U6artRXHandler = 0x9D001B00;
	_kseg0_program_Time2Handler   = 0x9D001E00;
	_kseg0_program_Can1Handler    = 0x9D002200;
	_kseg0_program_Can2Handler    = 0x9D002400;
	_kseg0_program_NothingHandler = 0x9D002600;
	_kseg0_program_MainHandler    = 0x9D002700;
	_kseg0_program_StarUpHandler  = 0x9D002c00;
*/
	_kseg0_program_StarUpHandler  = 0x9D001000;

/*************************************************************************
 * Memory Regions
 *
 * Memory regions without attributes cannot be used for orphaned sections.
 * Only sections specifically assigned to these regions can be allocated
 * into these regions.
 *************************************************************************/
MEMORY
{
  kseg0_main_flash_data(rx)  : ORIGIN = 0x9D000000, LENGTH = 0x1000
/*  exception_mem            : ORIGIN = 0x9D001000, LENGTH = 0x1000 */
/* 2020-1-21 modified
  kseg0_program_U2artRXHandler(rx)  : ORIGIN = 0x9D001000, LENGTH = 0x800
  kseg0_program_U3artRXHandler(rx)  : ORIGIN = 0x9D001800, LENGTH = 0x300
  kseg0_program_U6artRXHandler(rx)  : ORIGIN = 0x9D001B00, LENGTH = 0x300
  kseg0_program_Time2Handler(rx)    : ORIGIN = 0x9D001E00, LENGTH = 0x400
  kseg0_program_Can1Handler(rx)     : ORIGIN = 0x9D002200, LENGTH = 0x200
  kseg0_program_Can2Handler(rx)     : ORIGIN = 0x9D002400, LENGTH = 0x200
  kseg0_program_NothingHandler(rx)  : ORIGIN = 0x9D002600, LENGTH = 0x100
  kseg0_program_MainHandler(rx)     : ORIGIN = 0x9D002700, LENGTH = 0x500
  kseg0_program_StarUpHandler(rx)   : ORIGIN = 0x9D002c00, LENGTH = 0x300
*/
  kseg0_program_StarUpHandler(rx)   : ORIGIN = 0x9D001000, LENGTH = 0x1000


  kseg0_program_sw_info(w!x) : ORIGIN = 0x9D003000, LENGTH = 0x80
  kseg0_program_mem(rx)      : ORIGIN = 0x9D003080, LENGTH = 0x38000-0x80       /*256K=18000,512K=38000*/
  exception_mem              : ORIGIN = 0x9D03B000, LENGTH = 0x1000             /*256K=1B000,512K=3B000*/
  kseg0_upgrade_dnld         : ORIGIN = 0x9D03C000, LENGTH = 0x3F000            /*256K=1C000 1F000,512K=3C000 3F000*/
  kseg0_upgrade_bios         : ORIGIN = 0x9D07D000, LENGTH = 0x2000             /*256K=3D000,512K=7D000*/
  kseg0_upgrade_bios_d       : ORIGIN = 0x9D07F000, LENGTH = 0x1000             /*256K=3F000,512K=7F000*/

  kseg0_boot_mem             : ORIGIN = 0x9FC00490, LENGTH = 0x970
  /*exception_mem              : ORIGIN = 0x9FC01000, LENGTH = 0x1000 */
  kseg1_boot_mem             : ORIGIN = 0xBFC00000, LENGTH = 0x490
  debug_exec_mem             : ORIGIN = 0xBFC02000, LENGTH = 0xFF0

  config3                    : ORIGIN = 0xBFC02FF0, LENGTH = 0x4
  config2                    : ORIGIN = 0xBFC02FF4, LENGTH = 0x4
  config1                    : ORIGIN = 0xBFC02FF8, LENGTH = 0x4
  config0                    : ORIGIN = 0xBFC02FFC, LENGTH = 0x4
  kseg1_data_mem       (w!x) : ORIGIN = 0xA0000000, LENGTH = 0x20000-0x100
  kseg1_upgrade_ram    (w!x) : ORIGIN = 0xA000FF00, LENGTH = 0x100
  sfrs                       : ORIGIN = 0xBF800000, LENGTH = 0x100000
/*
  kseg1_upgrade_ram    (w!x) : ORIGIN = 0xA0000100, LENGTH = 0x100
  kseg1_data_mem       (w!x) : ORIGIN = 0xA0000200, LENGTH = 0x7000
  sfrs                       : ORIGIN = 0xBF800000, LENGTH = 0x100000
*/
}
SECTIONS
{
  .config_BFC02FF0 : {
    *(.config_BFC02FF0)
  } > config3
  .config_BFC02FF4 : {
    *(.config_BFC02FF4)
  } > config2
  .config_BFC02FF8 : {
    *(.config_BFC02FF8)
  } > config1
  .config_BFC02FFC : {
    *(.config_BFC02FFC)
  } > config0
}
