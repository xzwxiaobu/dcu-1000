#ifndef _PIC24_UPDATEDRV_H
#define _PIC24_UPDATEDRV_H

#include "_cpu.h"

#define PIC24_BL_ENABLE     1   /* 0=disable, else = enable */

/* ����ģʽ updateMode */
#define UPMODE_AUTO     0                       /* �˶԰汾��Ϣ�����Ƿ����� */
#define UPMODE_FORCE    1                       /* ǿ�����������˶԰汾��Ϣ */
#define UPMODE_BYCMD    0x10                    /* ���ⲿ������� */

/* PIC24 FLASH ��ص�ַ��Ϣ
 */
#define P24_FLASH_SIZE_WORD     ( 0x5800 )      /* �洢����С��word��   */
#define P24_CONFIG_WORD_ADDR    ( 0x57FC )      /* �����ֵĵ�ַ��word�� */
#define BL_CODE_START           ( 0x0400 )      /* bootloader start address(word)       */
#define BL_CODE_END             ( 0x0FFF )      /* bootloader end   address(word)       */
#define BL_VERSION_ADDR         ( 0x0FF0 )      /* bootloader version location(word)    */

/* PIC24 APP FLASH��ַ���� 
 */
#define APP_START_ADDR          0x1000      /* APP ��������ʼ��ַ */
#define APP_PRJINFO_ADDR        0x1000      /* ��Ŀ��Ϣ�洢��ַ */
#define APP_LAST_FLASH          0x5400      /* APP��FLASH���һ������������֣������� */

/* �ȽϹؼ��ֵĳ���, eg. "PEDC-PIC24" 
 * 0 ��ʾ���ȽϹؼ���
 */
#define APP_KEYWORDS_LEN        10          



/* �������� FLASH ҳ������Block�飩��PIC24FJ32GA002��22ҳ 
 * ���һ������������� */
#define ER_FLASH_BLOCKS         ((APP_LAST_FLASH<<1) / 2048)

#define P24_BLANK_DATA          0x00FFFFFFL /* �հ� */
#define P24_INST_PER_ROW        64          /* ÿ��FLASH ָ��64��ָ�� */
#define P24_BYTES_PER_INST      4           /* ÿ��ָ��4�ֽ� */
#define P24_BYTES_PER_ROW       (P24_INST_PER_ROW * P24_BYTES_PER_INST) /* ÿ��FLASH ָ��64��ָ�� 256 �ֽ� */
#define P24_WRITE_INST_ROWS     1           /* ÿ��д��ָ��������Ŀǰֻ֧��1�� */
#if (P24_WRITE_INST_ROWS != 1)
#error "WRITE_INST_ROWS must be 1 NOW!"
#endif

/* ÿ�η��͵������������ 256 �ֽ� 
 * ��ÿ�����д�� 1��ָ��
 * ������Ϣ ��16Bytes ��
 *  ͷ   8Bytes
 *  β   3Bytes
 *  ���� 5Bytes
 * 
 */
#define MAX_PACKET_SIZE     (256 + 16 + 8)  


/* PIC24 Bootloader ������ */
#define RD_VER      0x00        /* �� Bootloader �汾, eg. 00 02  */
#define RD_FLASH    0x01        /* �� Flash, eg. 01 02 00 00 00   */
#define WT_FLASH    0x02        /* д Flash, eg. 02 01 80 00 00 <data> */
#define ER_FLASH    0x03        /* ���� Flash, eg. 03 15 00 00 00  = 15 ��ʾPages */
#define RD_EEDATA   0x04
#define WT_EEDATA   0x05
#define RD_CONFIG   0x06
#define WT_CONFIG   0x07
#define VERIFY_OK   0x08        /* У��OK������ user Ӧ�ó���  */
#define COMMAND_NULL    0xff

/* �� PIC24Ӧ�ò� ͨ������ 
 * ���ǵ� Bootloader ����ͬʱҲҪ�����������������ֲ�Ҫ�� Bootloader �������ظ�
 */
#define PIC24APP_CMD_GET_CS     0x50        /* ��Checksum��Ϣ */
#define PIC24APP_CMD_ENTER_BL   0x51        /* �����λ����Bootloader */



/* �� PIC24 (Ӧ�ò� & Bootloader) ͨ��״̬
 *  */
typedef enum
{
    UP_PIC24_NULL       = 0 ,   /* û���������� */
    UP_PIC24_INIT           ,   /* ������ʼ�� */
    UP_PIC24_START_BL       ,   /* ���� Bootloader */
    UP_PIC24_UPDATE_INI     ,   /* ���� ���� */
    UP_PIC24_ERASE          ,   /* �������� PIC24 Bootloader ����       */
    UP_PIC24_WRITE          ,   /* �������ݸ� PIC24 Bootloader дFLASH  */
    UP_PIC24_VERIFY_WR      ,   /* ��������� PIC24 Bootloader ��FLASH У���������  */
    UP_PIC24_VERIFY_OK      ,   /* У��OK����������� PIC24 Bootloader �������� */
            
    UP_PIC24_BL_RESET       ,   /* PIC24 Bootloader �������� */
    UP_PIC24_BL_NOTRESET    ,   /* PIC24 Bootloader û��ͨ��У�飬���������� */
            
    UP_PIC24_FINISHED   = 0x5A, /* ������������                         */
}enmUpPic24Step;

/* PIC32���ذ�ظ�MMS �� PIC24 ����״̬�� (�������� OBJ_CMD_BL_CTRL)
 *  */
typedef enum
{
    TOMMS_PIC24_NOAPP           = 0,        /* ��ǰPIC24��û��Ӧ�ò����ֻ��Bootloader */
    TOMMS_PIC24_INIT            = 1,        /* ������ʼ�������� */
    TOMMS_PIC24_INIT_OK         = 2,        /* ������ʼ���ɹ�   */
                    
    TOMMS_PIC24_UPDATING        = 4,        /* �����У���������ͨ�� ���������Ⱥ����������� ���� */
    TOMMS_PIC24_FINISHED        = 5,        /* �������                 */
    TOMMS_PIC24_RUNAPP          = 6,        /* ������ɣ���������APP    */
    TOMMS_PIC24_UPDATED         = 0x10,     /* �Ѿ������°汾           */
    TOMMS_PIC24_UPDATED_APP     = 0x11,     /* APP�����°汾������Bootloader�汾��һ��  */
            
    TOMMS_PIC24_FAIL_BINFILE    = 0x80,     /* PIC32 �洢��PIC24 BIN�ļ�����  */
    TOMMS_PIC24_FAIL_INIT       = 0x81,     /* ������ʼ��ʧ��       */
    TOMMS_PIC24_FAIL_BL_OVTM    = 0x82,     /* ����Bootloader��ʱ   */
    TOMMS_PIC24_FAIL_VERIFY     = 0x83,     /* ����������У��ʧ��   */
    TOMMS_PIC24_FAIL_OVERTIME   = 0x84,     /* ����������PIC24��ʱ����Ӧ   */
    TOMMS_PIC24_FAIL_BL_CODE    = 0x85,     /* Bootloader �����ڻ���ڲ���ȷ   */
    TOMMS_PIC24_ERRORMODE       = 0x90,     /* ����ģʽ����     */
}enmP32ToMMSState;


typedef union
{
    DWORD   Val;
    
    WORD    w[2];
    struct
    {
        WORD LW;
        WORD HW;
    }word;
    
    BYTE    v[4];
}tDWORD;

typedef	struct
{

    BYTE  majorVer;     /* ����汾 */ 
    BYTE  minorVer;
    BYTE  pro_type;     /* Э������ */
    BYTE  pro_ver;      /* Э��汾 */

    BYTE  Day;
    BYTE  Month;
    WORD  Year;
    
    WORD  signal;       /* ��� */
}tBootloaderInfo;

#pragma pack(1)
typedef struct
{
    DWORD allCodeCS;    /* PIC24 Flash ����У��� */
    DWORD onlyAppCS;    /* PIC24 Flash ��Bootloader�Ĵ���У��� */
    tBootloaderInfo  blInfo;
}tFlashInfo;

typedef struct
{
    BYTE    cmd;        /* ����Flash���� */
    BYTE    len;        /* ���ȣ�ע�ⲻͬ������Ⱥ��岻һ�� */
    BYTE    addrL;      /* ����Flash��ַ���ֵ�ַ */
    BYTE    addrM;
    BYTE    addrH;
}tBLHeader;
#define BL_HEADER_LEN   sizeof(tBLHeader)

//typedef BOOL (*cbSendMsgToMMS)(int8u msgType, int8u srcNode, int8u * data, int16u dataLen);
//PUBLIC void setCalbackToMMS(cbSendMsgToMMS fun);

/* =============================================================================
 * �ӿں�������
 * 
 */

PUBLIC BOOL upPIC24_InitStart(int8u Initiator, int8u upMode, tFlashInfo *info);
PUBLIC void upPIC24_QueryPic24App(void);

PUBLIC enmUpPic24Step startUpdatePic24(tEnmCommObj obj);
PUBLIC void Pic24UpdateProcess(tUartComm *pComm);
PUBLIC BOOL chkIsUpdatingPic24(void);

PUBLIC void getBinCodeInfo(tFlashInfo * P24CodeInfo);

PUBLIC BOOL upPIC24_ProcP24Reply(int8u msgType, int8u *rxData, int16u rxDataLen);

#endif //_PIC24_UPDATEDRV_H

