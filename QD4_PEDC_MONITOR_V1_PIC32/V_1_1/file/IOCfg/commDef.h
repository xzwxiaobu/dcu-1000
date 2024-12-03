#ifndef _COMM_DEF_H
#define _COMM_DEF_H

#define PCB_ID_SERVER       200     //PEDC ���ذ��ID���� MONITOR

#define MAX_IODATA_SIZE             40
#define PIC32_IODATA_SIZE           12
#define PCA9505_IODATA_SIZE         5

#include "SWLogic.h"

/* =============================================================================
 *  Э��淶ͨ������֡�ĸ�ʽ�����£�
 *  �˹淶�� MMS �� PEDC ͨ�ŵĸ�ʽ��ͬ
 *  ������  ��  ��  ���ȣ��ֽڣ�    ֵ(����)    ˵  ��
 *  1.      ����֡ͷ1   1           0x10    
 *  2.      ����֡ͷ2   1           0x02   
 *  3.      ����֡���  1           INT8U   ��������������͡��ӻ�����������������Ÿ��Ƶ���������������
 *  4.      Դ�ڵ�      1           INT8U   ͨ������Դ�ڵ�
 *  5.      Ŀ��ڵ�    1           INT8U   ͨ������Ŀ��ڵ�
 *  6.      ��Ϣ����    1           INT8U   ͨ�����ݹ�������
 *  7.      ���ݳ���H   1           INT8U   ͨ�����ݳ��ȵ��ֽ�
 *  8.      ���ݳ���L   1           N       ��Э��֧��ͨ�����ݿ�ĳ������Ϊ255�ֽ�
 *  9.      ���ݿ�      N           INT8U   ���� N ���ֽڵ�����
 *  10.     ����֡β1   1           0x10    
 *  11.     ����֡β2   1           0x03   
 * 
 *     ֡У�鷽���ɶ���  FRAME_VERIFY_CRC16�����޸�Ϊ CRC У�飬ע���޸ĳ���
 *  12.     ֡У��      1           ֡ͷ��֡β���������ݰ��ֽڽ���������Ľ��
 * 
 *  CRC ???
 *  12.     ֡У��      2           INT16U  ֡���ݵ�У��ֵ�����Ͳ���С��ģʽ�����ȷ��͵��ֽڣ�����֡ͷ��֡β���������ݰ��ֽڼ���CRC16
 * */

/**
 */
#define HEADER_DLE      0x10
#define HEADER_STX      0x02
#define TAIL_DLE        0x10
#define TAIL_ETX        0x03

/* ����֡ͷͳһ��ʽ */
//#pragma pack(1)
typedef struct
{
    int8u header1;      /* ֡ͷ1    */
    int8u header2;      /* ֡ͷ2    */
    int8u seq;          /* ֡���   */
    int8u srcNode;      /* Դ�ڵ�   */
    int8u dstNode;      /* Ŀ��ڵ� */
    int8u msgType;      /* ͨ���������� tEnmCommObjCmd  */
    int8u dataLenH;     /* ���ݳ��� */
    int8u dataLenL;     /* ���ݳ��� */
}tPktHeader ;

#define FRAME_VERIFY_CRC16      0   /* 1=CRC16 */

/* ����֡βͳһ��ʽ */
//#pragma pack(1)
typedef struct
{
    int8u Tail_DLE;     /* ������� */
    int8u Tail_ETX;     /* ������� */
    #if (FRAME_VERIFY_CRC16 != 1)
    int8u crcXOR;       /* ����У�飬���ֵ */
    #else
    int8u crc16L;       /* ����У��CRC16 ���ֽ� */
    int8u crc16H;       /* ����У��CRC16 ���ֽ� */
    #endif
}tPktTail ;


/* ����ͨ�����ݶ���ö������ */
typedef enum
{
    COMMOBJ_MONITOR = 200,  /* 0 :0xC8, MONITOR-PIC32  */
    //COMMOBJ_PSL     = 201,  /* 1 :0xC9, PSL �� */
    //COMMOBJ_SIG,            /* 2 :0xCA, SIG �� */
    //COMMOBJ_IBP,            /* 3 :0xCB, IBP �� */
    COMMOBJ_IORW_A = 204,   /* 4 :0xCC, MONITOR-PIC24A */
    COMMOBJ_IORW_B,         /* 5 :0xCD, MONITOR-PIC24B */
    COMMOBJ_SWITCHER,       /* 6 :0xCE, SZ16-�л���  */
    COMMOBJ_LOG_A_UA1 ,     /* 7 :0xCF, SZ16-�߼���A-UA1 */
    COMMOBJ_LOG_A_UB1 ,     /* 8 :0xD0, SZ16-�߼���A-UB1 */
    COMMOBJ_LOG_B_UA1 ,     /* 9 :0xD1, SZ16-�߼���B-UA1 */
    COMMOBJ_LOG_B_UB1 ,     /* 10:0xD2, SZ16-�߼���B-UB1 */
    COMMOBJ_COUNTER,    /* �����Զ��������� */
    COMMOBJ_UNDEF = 0xff,
}tEnmCommObj;

#define COMMOBJ_FIRST   COMMOBJ_MONITOR
#define COMMOBJ_TOTAL   ((int32u)(COMMOBJ_COUNTER - COMMOBJ_MONITOR))

#define COMMOBJ_LOG1            COMMOBJ_LOG_A_UB1
#define COMMOBJ_LOG2            COMMOBJ_LOG_B_UB1


/* ����ͨ����������ö������ */
#define CMD_DIR_BIT     0x80
typedef enum
{
    OBJ_CMD_IOR = 0,    /* ��IO����     */
    OBJ_CMD_CFG_WR,     /* д������Ϣ   */
    OBJ_CMD_CFG_RD,     /* ��������Ϣ   */
    OBJ_CMD_RELAY_LIFE, /* ���̵�����Ϣ */
            
    OBJ_CMD_TESTDATA    = 0xAA,         /* ��������A0 ~ AF */
    OBJ_CMD_INI_TEST    = 0xB0,         /* ���ò��Թ��� */
            
    /* ����������ص����� 
     * ע���� glbUpdate.h һ��
     */
    OBJ_CMD_UPDATE      = 0xE0,         /* ��������         */
    OBJ_CMD_UPDATE_RPLY = 0xE1,         /* �ظ���������״̬ */
            
    OBJ_CMD_UPDATESTS   = 0xE2,         /* ������״̬       */
            
    OBJ_CMD_DEVINFO     = 0xE5,         /* ���豸��Ϣ       */
    OBJ_CMD_RPLYINFO_OK = 0xE6,         /* �ظ����豸��Ϣ   */
    OBJ_CMD_RPLYINFO_NG = 0xE7,         /* �ظ����豸��Ϣ   */
            
    OBJ_CMD_BOOTLOADER  = 0xF0,         /* �� Bootloader ͨ�� */
    OBJ_CMD_BL_CTRL     = 0xF1,         /* ���� Bootloader ����  */
}tEnmCommObjCmd;


#define PCA9505_1   (0x40 | (0 << 1))    //PCA9505(1)
#define PCA9505_2   (0x40 | (1 << 1))    //PCA9505(2)
#define PCA9505_3   (0x40 | (3 << 1))    //PCA9505(3)

/* ���尲ȫ��·�������˿�
 */
typedef enum
{
    SF_INPUT_PORT_NONE = 0xff , //�޼���
    
    /* MCU GPIO ����� 
     * �μ�prots.h --> IoPortId ����
     */
    SF_INPUT_PB     = 0,     //MCU GPIOB
    SF_INPUT_PC        ,     //MCU GPIOC
    SF_INPUT_PD        ,     //MCU GPIOD
    SF_INPUT_PE        ,     //MCU GPIOE
    SF_INPUT_PF        ,     //MCU GPIOF
    SF_INPUT_PG        ,     //MCU GPIOG
    
    /* PCA9505 ����� */
    SF_INPUT_PCA1      = PCA9505_1,   //PCA9505(1)
    SF_INPUT_PCA2      = PCA9505_2,   //PCA9505(2)
    SF_INPUT_PCA3      = PCA9505_3,   //PCA9505(3)
}tEnmSFInputPort;

/* ���尲ȫ��·�������˿����� */
typedef enum
{
    SF_TYPE_DCU_K31PN = 0 ,         //DCU ��ȫ�̵��� ��Ȧ K31K32PN
    SF_TYPE_DCU_K31NC     ,         //DCU ��ȫ�̵��� ���� K31K32
    SF_TYPE_DCU_K33NO     ,         //DCU ��ȫ�̵��� ���� K33
    SF_TYPE_PDS_coil      ,         //PDS ��ȫ�̵��� ��Ȧ
    SF_TYPE_PDS_contact   ,         //PDS ��ȫ�̵��� ����
    SF_TYPE_PDS_contact2  ,         //PDS ��ȫ�̵��� ����
            
    SF_SCAN_TYPES                   //���ڼ���
}tEnmSFType;

    
/* 
 * ����ɨ�谲ȫ�̵����õ�GPIO
 */
typedef struct
{
    tEnmSFType          type;   /* ����         */
    tEnmSFInputPort     port;   /* �˿�         */
    int16u              pin;    /* ��λ BIT_xx  */
}tSFScanCfgs;

/* ����Ķ�����Ҫ����PCBԭ��ͼȷ��
 * ʵ��PCA9505��A2 A1 A0
 * ���֧������3��PCA9505
 */
typedef enum
{
    PCA9505_EN1  = PCA9505_1,   //PCA9505(1)ʹ��
    PCA9505_EN2  = PCA9505_2,   //PCA9505(2)ʹ��
    PCA9505_EN3  = PCA9505_3,   //PCA9505(3)ʹ��
    PCA9505_DIS  = 0xff     ,   //��ֹ
}tEnmPCA9505Cfg;


/* ����������Ч����
 */
typedef enum
{
    CFG_INVALID = 0 ,           //������Ч
    CFG_VALID   = 0xAB,         //������Ч
}tEnmCfgValid;

/* ����MCU GPIO ���ó���
 */
typedef enum
{
    GPIOCFG_DIS = 0xff ,        //��ֹ MCU GPIO ����
    GPIOCFG_EN  = 12,           //ʹ�� MCU GPIO ����
}tEnmGPIOCfg;

/* ͨ������֡��׼�ṹ
 */
//#pragma pack(1)
typedef struct
{
    tPktHeader  header;
    int8u      *pData;          /* ������   */
    tPktTail    tail;
}tCommObjCmd_Req;

/* ״̬��־ */
typedef  union
{
    struct 
    {
        int8u pca9505Err1   : 1;   /* ��дPCA9505(1)�����־��1 = ����*/
        int8u pca9505Err2   : 1;   /* ��дPCA9505(1)�����־��1 = ����*/
        int8u pca9505Err3   : 1;   /* ��дPCA9505(1)�����־��1 = ����*/
        int8u uartCommErr   : 1;   /* ����ͨ�Ŵ����־��1 = ����      */
        int8u dcuSFRelayPow : 1;   /* 0 = �ϵ�, 1 = ͨ�� */
        int8u dcuSFRelayErr : 1;   /* 1 = ���� */
        int8u pdsSFRelayPow : 1;   /* 0 = �ϵ�, 1 = ͨ�� */
        int8u pdsSFRelayErr : 1;   /* 1 = ���� */
    }bits;

    int8u byte;
}tSlaveStsFlags;

typedef struct
{    
    int8u  gpioEn;              /* ��Ч��־, tEnmGPIOCfg */
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6��16λGPIO(PB~PG, 12�ֽ�IO���ݣ�С��ģʽ */
}tPic32IOData;

typedef struct
{    
    int8u  pca9505_En;          /* PCA9505 IO�����Ч��־, tEnmPCA9505Cfg   */
    int8u  pca9505Data[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
}tPCA9505IOData;



/* ��ȫ��·���� */
typedef union
{
    struct 
    {
        int8u ConnBrks : 4;   /* �Ͱ��ֽ��Ǵ������� */
        int8u CoilBrks : 4;   /* �߰��ֽ���Ȧ����   */
    }flash;
    
    int8u byte;
}tSF_Flash;


/* ====================================================== */
/* IBP�ظ���IO���� 
 * �ظ����������ݽṹ
 */
//#pragma pack(1)
typedef struct
{
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6��16λGPIO(PB~PG, 12�ֽ�IO���ݣ�С��ģʽ */
    
    tSF_Flash dcuSFFlash;           /* DCU��ȫ��·���ϴ��� */

    tSF_Flash pdsSFFlash;           /* PDS��ȫ��·���ϴ��� */

    tSlaveStsFlags stsFlags;        /* ״̬��־ */
}tReadIBP_IO;

/* PSL�ظ���IO���� 
 * �ظ����������ݽṹ
 */
typedef struct
{
    int8u  gpioData[PIC32_IODATA_SIZE];     /* 6��16λGPIO(PB~PG, 12�ֽ�IO���ݣ�С��ģʽ */
    
    tSF_Flash dcuSFFlash;           /* DCU��ȫ��·���ϴ��� */

    tSF_Flash pdsSFFlash;           /* PDS��ȫ��·���ϴ��� */

    tSlaveStsFlags stsFlags;        /* ״̬��־ */
}tReadPSL_IO;

/* SIG�ظ���IO���� 
 * �ظ����������ݽṹ
 */
typedef struct
{
    int8u  pca9505Data[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    
    tSF_Flash dcuSFFlash;           /* DCU��ȫ��·���ϴ��� */

    tSF_Flash pdsSFFlash;           /* PDS��ȫ��·���ϴ��� */

    tSlaveStsFlags stsFlags;        /* ״̬��־ */
}tReadSIG_IO;


/* IPS �ظ�����IO���� 
 * �ظ����������ݽṹ
 */
//#pragma pack(1)
typedef struct
{
    int8u  outData[PIC32_IODATA_SIZE];      /* 6��16λGPIO(PB~PG, 12�ֽ�IO���ݣ�С��ģʽ */
    
}tWriteIPS_IO;




/* ====================================================== */
/* PIC24 �ظ���IO���� 
 * �ظ����������ݽṹ
 */
//#pragma pack(1)
typedef struct
{
    /* �������Ч��־��Ч
     * ��������ݱ�����
     */
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    
    tSF_Flash dcuSFFlash;           /* DCU��ȫ��·���ϴ��� */
    tSF_Flash pdsSFFlash;           /* PDS��ȫ��·���ϴ��� */
    tSlaveStsFlags stsFlags;        /* ״̬��־ */
}tReadPic24_IO;



/* �������󣺴�PIC24 �� IO���� 
 * Я����������ݽṹ
 */
//#pragma pack(1)
typedef struct
{
    /* �������Ч��־��Ч
     * ��������ݱ�����
     */
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
}tWritePic24_IO;

/* �߼���ظ���IO���� 
 * �ظ����������ݽṹ
 */
typedef struct
{
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    
    tSF_Flash dcuSFFlash;           /* 1�ֽ� DCU��ȫ��·���ϴ��� */

    tSF_Flash pdsSFFlash;           /* 1�ֽ� PDS��ȫ��·���ϴ��� */

    tSlaveStsFlags stsFlags;        /* 1�ֽ� ״̬��־ */
    
    int8u       UB1_Flag;           /* 1�ֽ� UB1״̬��־ */
    int8u       UB1_OutLogicNo;     /* 1�ֽ� UB1����߼���ţ�0xff ��ʾ������߼���� */
    tOutputCmd  UB1_outLogic;       /* 4�ֽ� UB1�����Ϣ */
    union
    {
        struct
        {
            int8u   testEnable: 1;      /* ʹ��״̬ */
            int8u   isRunning : 1;      /* ����״̬ */
            int8u             : 6;
        };
        
        int8u data;
    }testRunSts;                    /* 1�ֽ� UB1��������״̬ */
    int8u       UB1_TestRunCnt[4];  /* 4�ֽ� UB1�������м����� */
}tReadLOG_IO;

/* д�߼����IO
 */
typedef struct
{
    int8u  pca9505Data1[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data2[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
    int8u  pca9505Data3[PCA9505_IODATA_SIZE];    /* 5�ֽ�IO����              */
}tWriteLOG_IO;

typedef union
{
    tReadIBP_IO     ibpIn;
    tReadPSL_IO     pslIn;
    tReadSIG_IO     sigIn;
    tReadLOG_IO     logIn;
    tReadPic24_IO   pic24In;
    unsigned char   rdBuf[MAX_IODATA_SIZE];
}tCmdReadIO_s;


typedef union
{
    tWriteIPS_IO    IPSOutput;
    tWritePic24_IO  Pic24Output;
    tWriteLOG_IO    logOut;
    unsigned char   wrBuf[MAX_IODATA_SIZE];
}tCmdWriteIO_m;

/* 2024-6-17 : �����л�ʱ�ı������ӣ��ۺϼ�ر�PEDC���ϣ����Լ����Ӱ�1�Ͱ�2֮����Զ��л����ܡ� */
typedef union
{
    struct
    {
        int8u RESET_En      : 1;
        int8u Q1Switch_En   : 1;
        int8u Q2Switch_En   : 1;
        int8u PN0_En        : 1;
        int8u               : 4;

        int8u RESET         : 1;
        int8u Q1Switch      : 1;
        int8u Q2Switch      : 1;
        int8u PN0           : 1;
        int8u               : 4;
    }IOOut;
    
    int8u IOData[2];
}tCmdWriteIO_SFSW;


#endif  //_COMM_DEF_H
