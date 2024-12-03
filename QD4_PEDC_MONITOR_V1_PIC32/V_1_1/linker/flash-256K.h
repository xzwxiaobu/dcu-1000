#ifndef _FLASH_H_
#define _FLASH_H_

/*
没有EEDATA，一些需要掉电保存的信息只能保存在Flash中
       FLASH MAPPING    virtual address(BYTE)
    ___________________  0x9FC00000
    |                 |
    |   boot flash    |
    |   12KB          |
    |                 |
    |_________________|

    ___________________
    |    112KB        |  0x9D000000 (len = 0x1C000)
    |                 |
    |   4KB flash     |
    |   data          |
    |   +             |  主程序区
    |   Main Code     |
    |   116KB         |
    |                 |
    |_________________|
    |                 |  0x9D01C000 (len = 0x1C000 + 12K = 0x1F000)
    |   Mapping Code  |
    |   124KB         |  下载区
    |   (boot 12KB)   |
    |   (main 112KB)  |
    |                 |
    |                 |
    |_________________|
    |                 |  0x9D03B000 (len = 0x2000)
    |   8KB flash     |
    |   reserved      |  保留区
    |                 |
    |_________________|
    |                 |  0x9D03D000 (len = 0x2000)
    |  Update         |
    |  Firmware       |  升级程序
    |  8KB            |
    |_________________|
    |                 |  0x9D03F000 (len = 0x1000)
    |  Update         |
    |  Data Saving    |  升级程序的信息数据
    |  4KB            |
    |_________________|
*/

#define INT_DIS()		asm volatile ("di")
#define INT_EN()		asm volatile ("ei")
#define REAL_ADDR(a)	(a)							//(a & 0x1FFFFFFFL)

#define FLASH_BOOT_ADDR			(0x9FC00000L)		//引导区
#define FLASH_BOOT_SIZE			(0x3000L)			//引导区空间大小（12KB)

#define	FLASH_BASE_ADDR		 	(0x9D000000L)		//FLASH的起始地址
#define	FLASH_SIZE_WORDS		(0x40000L)			//FLASH空间的大小
#define FLASH_PAGE_SIZE			0x1000L				//页空间大小（4096B)
#define FLASH_ROW_SIZE			512
#define	INST_PER_ROW			128					//一行包含128 个32 位指令字或512 字节
#define	INST_ROW_SIZE			(INST_PER_ROW*4)	//128 instructions

//kseg0_main_flash_data
#define	MAIN_FLASH_DATA_ADDR	(0x9D000000L)		//主程序用于保存数据的FLASH空间起始地址
#define	MAIN_FLASH_DATA_SIZE	(0x1000L)			//主程序用于保存数据的FLASH空间大小
#define	MAIN_CODE_ADDR			(0x9D001000L)		//主程序的FLASH空间起始地址
#define	MAIN_CODE_SIZE	 		(0x1B000L)			//主程序的FLASH空间大小
#define	MAIN_UPDATE_ADDR		MAIN_FLASH_DATA_ADDR	//主程序需更新的FLASH空间起始地址(包括保存数据的FLASH空间)
#define	MAIN_UPDATE_SIZE	 	(MAIN_CODE_SIZE+MAIN_FLASH_DATA_SIZE)		//主程序的FLASH空间大小(包括保存数据的FLASH空间)

//kseg0_upgrade_dnld
#define	DOWN_CODE_ADDR			(0x9D01C000L)		//程序映射区的起始地址
#define	DOWN_CODE_SIZE			(FLASH_BOOT_SIZE+MAIN_FLASH_DATA_SIZE+MAIN_CODE_SIZE)		//程序映射区的大小

//kseg0_upgrade_bios
#define	UPFW_CODE_ADDR			(0x9D03D000L)		//升级程序的FLASH空间起始地址
#define	UPFW_CODE_SIZE			(0x2000L)			//升级程序的FLASH空间大小
//kseg0_upgrade_bios_d
#define UPFW_FLASH_DATA_ADDR	(0x9D03F000L)		//升级程序用于保存数据的FLASH空间起始地址
#define UPFW_FLASH_DATA_SIZE	(0x1000L)			//升级程序用于保存数据的FLASH空间大小

//config
#define	config_BFC02FF0			(0x9FC02FF0L)
#define	config_BFC02FF4			(0x9FC02FF4L)
#define	config_BFC02FF8			(0x9FC02FF8L)
#define	config_BFC02FFC			(0x9FC02FFCL)

//=====================================
//升级程序用于保存数据的FLASH空间分配(bytes)，不要超过512Bytes，以便利用行写入一次完成。
//此部分数据只有升级程序才能访问
//linker address : kseg0_upgrade_bios_d
#if ((UPFW_FLASH_DATA_ADDR & 0xFFFL) != 0x0L)
#error "UPFW_FLASH_DATA_ADDR must be aligned to 4K!"
#endif

#define	UPFW_STATE_ADDR			(UPFW_FLASH_DATA_ADDR)
#define UPFW_STATE_SIZE			(128)
#define UPFW_STATE_OFFSET		(UPFW_STATE_ADDR - UPFW_FLASH_DATA_ADDR)

#define UPFW_BACK_CFG_ADDR		(UPFW_STATE_ADDR+UPFW_STATE_SIZE)	//用于临时保存配置字
#define UPFW_BACK_CFG_SIZE		(16)
#define UPFW_BACK_CFG_OFFSET	(UPFW_BACK_CFG_ADDR - UPFW_FLASH_DATA_ADDR)

//=====================================
//主程序用于保存数据的FLASH空间分配(bytes)，不要超过512Bytes，以便利用行写入一次完成。
//描述信息
#define SW_DISCRIPTION_ADDR		(MAIN_CODE_ADDR)
#define SW_DISCRIPTION_SIZE		(128)

//掉电需要保存的信息
//linker address : kseg0_main_flash_data
#if ((MAIN_FLASH_DATA_ADDR & 0xFFFL) != 0x0L)
#error "MAIN_FLASH_DATA_ADDR must be aligned to 4K!"
#endif

#define SW_DATA_ADDR			(MAIN_FLASH_DATA_ADDR)
#define SW_DATA_SIZE			(128)




















#endif //_FLASH_H_

