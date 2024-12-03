#ifndef _FLASH_H_
#define _FLASH_H_

/*
û��EEDATA��һЩ��Ҫ���籣�����Ϣֻ�ܱ�����Flash��
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
    |   +             |  ��������
    |   Main Code     |
    |   116KB         |
    |                 |
    |_________________|
    |                 |  0x9D01C000 (len = 0x1C000 + 12K = 0x1F000)
    |   Mapping Code  |
    |   124KB         |  ������
    |   (boot 12KB)   |
    |   (main 112KB)  |
    |                 |
    |                 |
    |_________________|
    |                 |  0x9D03B000 (len = 0x2000)
    |   8KB flash     |
    |   reserved      |  ������
    |                 |
    |_________________|
    |                 |  0x9D03D000 (len = 0x2000)
    |  Update         |
    |  Firmware       |  ��������
    |  8KB            |
    |_________________|
    |                 |  0x9D03F000 (len = 0x1000)
    |  Update         |
    |  Data Saving    |  �����������Ϣ����
    |  4KB            |
    |_________________|
*/

#define INT_DIS()		asm volatile ("di")
#define INT_EN()		asm volatile ("ei")
#define REAL_ADDR(a)	(a)							//(a & 0x1FFFFFFFL)

#define FLASH_BOOT_ADDR			(0x9FC00000L)		//������
#define FLASH_BOOT_SIZE			(0x3000L)			//�������ռ��С��12KB)

#define	FLASH_BASE_ADDR		 	(0x9D000000L)		//FLASH����ʼ��ַ
#define	FLASH_SIZE_WORDS		(0x40000L)			//FLASH�ռ�Ĵ�С
#define FLASH_PAGE_SIZE			0x1000L				//ҳ�ռ��С��4096B)
#define FLASH_ROW_SIZE			512
#define	INST_PER_ROW			128					//һ�а���128 ��32 λָ���ֻ�512 �ֽ�
#define	INST_ROW_SIZE			(INST_PER_ROW*4)	//128 instructions

//kseg0_main_flash_data
#define	MAIN_FLASH_DATA_ADDR	(0x9D000000L)		//���������ڱ������ݵ�FLASH�ռ���ʼ��ַ
#define	MAIN_FLASH_DATA_SIZE	(0x1000L)			//���������ڱ������ݵ�FLASH�ռ��С
#define	MAIN_CODE_ADDR			(0x9D001000L)		//�������FLASH�ռ���ʼ��ַ
#define	MAIN_CODE_SIZE	 		(0x1B000L)			//�������FLASH�ռ��С
#define	MAIN_UPDATE_ADDR		MAIN_FLASH_DATA_ADDR	//����������µ�FLASH�ռ���ʼ��ַ(�����������ݵ�FLASH�ռ�)
#define	MAIN_UPDATE_SIZE	 	(MAIN_CODE_SIZE+MAIN_FLASH_DATA_SIZE)		//�������FLASH�ռ��С(�����������ݵ�FLASH�ռ�)

//kseg0_upgrade_dnld
#define	DOWN_CODE_ADDR			(0x9D01C000L)		//����ӳ��������ʼ��ַ
#define	DOWN_CODE_SIZE			(FLASH_BOOT_SIZE+MAIN_FLASH_DATA_SIZE+MAIN_CODE_SIZE)		//����ӳ�����Ĵ�С

//kseg0_upgrade_bios
#define	UPFW_CODE_ADDR			(0x9D03D000L)		//���������FLASH�ռ���ʼ��ַ
#define	UPFW_CODE_SIZE			(0x2000L)			//���������FLASH�ռ��С
//kseg0_upgrade_bios_d
#define UPFW_FLASH_DATA_ADDR	(0x9D03F000L)		//�����������ڱ������ݵ�FLASH�ռ���ʼ��ַ
#define UPFW_FLASH_DATA_SIZE	(0x1000L)			//�����������ڱ������ݵ�FLASH�ռ��С

//config
#define	config_BFC02FF0			(0x9FC02FF0L)
#define	config_BFC02FF4			(0x9FC02FF4L)
#define	config_BFC02FF8			(0x9FC02FF8L)
#define	config_BFC02FFC			(0x9FC02FFCL)

//=====================================
//�����������ڱ������ݵ�FLASH�ռ����(bytes)����Ҫ����512Bytes���Ա�������д��һ����ɡ�
//�˲�������ֻ������������ܷ���
//linker address : kseg0_upgrade_bios_d
#if ((UPFW_FLASH_DATA_ADDR & 0xFFFL) != 0x0L)
#error "UPFW_FLASH_DATA_ADDR must be aligned to 4K!"
#endif

#define	UPFW_STATE_ADDR			(UPFW_FLASH_DATA_ADDR)
#define UPFW_STATE_SIZE			(128)
#define UPFW_STATE_OFFSET		(UPFW_STATE_ADDR - UPFW_FLASH_DATA_ADDR)

#define UPFW_BACK_CFG_ADDR		(UPFW_STATE_ADDR+UPFW_STATE_SIZE)	//������ʱ����������
#define UPFW_BACK_CFG_SIZE		(16)
#define UPFW_BACK_CFG_OFFSET	(UPFW_BACK_CFG_ADDR - UPFW_FLASH_DATA_ADDR)

//=====================================
//���������ڱ������ݵ�FLASH�ռ����(bytes)����Ҫ����512Bytes���Ա�������д��һ����ɡ�
//������Ϣ
#define SW_DISCRIPTION_ADDR		(MAIN_CODE_ADDR)
#define SW_DISCRIPTION_SIZE		(128)

//������Ҫ�������Ϣ
//linker address : kseg0_main_flash_data
#if ((MAIN_FLASH_DATA_ADDR & 0xFFFL) != 0x0L)
#error "MAIN_FLASH_DATA_ADDR must be aligned to 4K!"
#endif

#define SW_DATA_ADDR			(MAIN_FLASH_DATA_ADDR)
#define SW_DATA_SIZE			(128)




















#endif //_FLASH_H_

