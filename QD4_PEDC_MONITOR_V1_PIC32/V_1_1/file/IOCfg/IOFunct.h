

#ifndef _IOFUNCT_H
#define _IOFUNCT_H

#if( defined (PCB_PIC24))
#include <p24Fxxxx.h>
#endif

#include "MonCommCfg.h"


/* I2C������ 400K */
#define I2C_CLOCK_FREQ          400000

/* PCA9505 RESET PIN */
#if( defined (PCB_PIC24))
#define I2C_ID                      0    /* оƬ I2C �ӿ�ID */

#define I2C_RST                     LATBbits.LATB7            //PCA9505 ��λ���ƽ�λ
#define PCA9505_RST_SETOUTPUT()     (TRISBbits.TRISB7 = 0)
#define PCA9505_RST_OUT1()          (I2C_RST = 1)
#define PCA9505_RST_OUT0()          (I2C_RST = 0)

#else
#define I2C_ID                      I2C1    /* оƬ I2C �ӿ�ID */
#define PCA9505_RST_PortBit       (1 << 11)
#define PCA9505_RST_SETOUTPUT()   (mPORTDSetPinsDigitalOut(PCA9505_RST_PortBit))    //RST SET OUTPUT
#define PCA9505_RST_OUT1()        (mPORTDSetBits(PCA9505_RST_PortBit))
#define PCA9505_RST_OUT0()        (mPORTDClearBits(PCA9505_RST_PortBit))
#endif

/* 9505 Command register */
#define	COMMAND_05_IP0          0x00     //Input Port registers
#define	COMMAND_05_IP1          0x01
#define	COMMAND_05_IP2          0x02
#define	COMMAND_05_IP3          0x03
#define	COMMAND_05_IP4          0x04
#define	COMMAND_05_OP0          0x08    //Output Port registers
#define	COMMAND_05_OP1          0x09
#define	COMMAND_05_OP2          0x0A
#define	COMMAND_05_OP3          0x0B
#define	COMMAND_05_OP4          0x0C
#define	COMMAND_05_PI0          0x10    //Polarity Inversion registers
#define	COMMAND_05_PI1          0x11
#define	COMMAND_05_PI2          0x12
#define	COMMAND_05_PI3          0x13
#define	COMMAND_05_PI4          0x14    //I/O Configuration registers
#define	COMMAND_05_IOC0         0x18
#define	COMMAND_05_IOC1         0x19
#define	COMMAND_05_IOC2         0x1A
#define	COMMAND_05_IOC3         0x1B
#define	COMMAND_05_IOC4         0x1C    //Mask Interrupt registers
#define	COMMAND_05_MSK0         0x20
#define	COMMAND_05_MSK1         0x21
#define	COMMAND_05_MSK2         0x22
#define	COMMAND_05_MSK3         0x23
#define	COMMAND_05_MSK4         0x24

/* Auto-Increment flag is set, the 3 least significant bits are automatically incremented after a read or write */
#define AUTO_INC                0x80    


/* ������������ PCA9505
 * ��������ű���Ϊ2���֣�
 * ��4λ��PORT(0~4)����4λ��bit(0~7)
 * ������������Ч�����λҲ��Ч
 */    
typedef struct
{
    int8u  dcuK31PN_Port;   /* DCU��ȫ��· K31K32PN ��Ȧ��������, tEnmSFInputPort */
    int8u  dcuK31PN_Pin;    /* DCU��ȫ��· K31K32PN ��Ȧ��������                  */
    
    int8u  dcuK31NC_Port;   /* DCU��ȫ��· K31K32�����������, tEnmSFInputPort */
    int8u  dcuK31NC_Pin;    /* DCU��ȫ��· K31K32�����������                  */
    
    int8u  dcuK33NO_Port;   /* DCU��ȫ��· K33   �����������, tEnmSFInputPort */
    int8u  dcuK33NO_Pin;    /* DCU��ȫ��· K33   �����������                  */
    
    int8u  pds_Port1;       /* PDS�̵����������������Ϣ */
    int8u  pds_Pin1;
    int8u  pds_Port2;       /* PDS�̵����������������Ϣ */
    int8u  pds_Pin2;
    int8u  pds_Port3;       /* PDS�̵����������������Ϣ */
    int8u  pds_Pin3;
}tSFScanCfg;

/* =============================================================================
 * ɨ��������ز���
 *  */
#define DBNCIN_BUF_NUM              5       /* ����ȥ��������  */
#define MCU_GPIO_NUM                6       /* MCU��PIC32MX795F512H����GPIO�˿���  */

/* ��ʼ��ɨ����
 *  */
PUBLIC void InitScanInputResult(void);

/* 
 * ��ȡ����ָ��Ľӿں������������ļ�����
 */
//PUBLIC tSlaveIOScanCfg * getIOScanCfgPtr(void);
PUBLIC tCmdReadIO_s    * getSlvReadIOPtr(void);
//PUBLIC tCmdWriteIO_m   * getSlvWriteIOPtr(void);

/* 
 * ��ʼ��ɨ��IO�Ĵ洢��
 */
//PUBLIC void initIOScanReg(tSlaveIOScanCfg *pCfg, BOOL ExeCfg);

/* 
 * ����ɨ��IO�洢��
 */
PUBLIC void resetScanIORegister(void);

/* ͨ�� reset ���Ÿ�λ PCA9505 
 * д������
 */
//PUBLIC void resetPCA9505(tSlaveIOScanCfg *cfgPtr);
    
/* ;
 * ���� PCA9505 ���������
 * ����:
 *  TRUE  : ���óɹ�
 *  FALSE : ����ʧ��
 */
//PUBLIC BOOL configPCA9505_IOC(tSlaveIOScanCfg *cfg);

/* ���ü�ⰲȫ�̵����Ķ˿�
 *  cfg     : ����ָ��
 *  type    : �������
 *  port    : ����      ������PCA9505��оƬ��ַ = 0x40 | (A2A1A0 << 1)
 *  pinNum  : ����(���), ����PCA9505��4λ��IO����4λ��λ
 */
//PUBLIC void configScanGPIO(tSlaveIOScanCfg *cfg, tEnmSFType type, int8u port, int8u pinNum);

/* 
 * ����ѭ���е��ã���ȡ�����
 */
PUBLIC void getPCBAllInput(void);

/* 
 * ÿ 1ms ���ã����Ե�����ź�
 */
//PUBLIC void procOutputTP(tSlaveIOScanCfg *cfg);

/* 
 * I2C ERR ���� : ��ʼ��PCA9505, ͬʱ��ʼ��оƬ I2C �ӿ�
 * 
 */
PUBLIC void I2C_ERR_INI(void);

/* ��״̬��DCU ��ȫ�̵��� ��Ȧ K31K32PN
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkDCUSF_K31PN(void);


/* ��״̬��DCU ��ȫ�̵��� ���� K31K32
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkDCUSF_K31NC(void);


/* ��״̬��DCU ��ȫ��· ���� K33
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkDCUSF_K33NO(void);

/* ��״̬��PDS ��ȫ�̵��� ��Ȧ
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkPDSSF_Coil(void);


/* ��״̬��PDS ��ȫ�̵��� ����
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ */
PUBLIC BOOL chkPDSSF_Contactor(void);


/* ��״̬��PDS ��ȫ�̵��� ����
 * ����TRUE��ʾ�ߵ�ƽ��FALSE��ʾ�͵�ƽ��δ���� */
PUBLIC BOOL chkPDSSF_Contactor2(void);


/* 
 * ���̵���״̬
 * ÿ�� 15ms ���һ��״̬�����棬��󱣴�״̬�ĳ���ʱ�� 64 * 15 = 960ms
 *  - �� K31 ��Ȧͨ��״̬�ж� K33 ����
 *  - 
 */
PUBLIC void detectSFRelayState(void);

PUBLIC void initSFDectectRegs(void);

/* 
 * �������
 *  pOutputIO   : ������ü�����
 *  ipsFlag     : TRUE ��ʾ IBP / PSL / SIG ��
 * */
//PUBLIC void updateOutputIO(tCmdWriteIO_m *pOutputIO, BOOL ipsFlag);

#endif  //_IOFUNCT_H

