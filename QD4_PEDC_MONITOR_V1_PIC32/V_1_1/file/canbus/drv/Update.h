#ifndef _UPDATE_H_
#define _UPDATE_H_

//===============================================
#define EE_FLG_VALID       0x5AA5

//one section (see Э��˵��.txt)
#define PACK_PACKAGE_HEAD       0x55AA
#define PACK_CMD_START          0x0101      //��������
#define PACK_CMD_DATA           0x0102      //����
#define PACK_CMD_END            0x0110      //���ݽ���

#define PACK_CMD_UPDATE_DCU     0x0130      //����������DCU����

//===============================================
#define UPFW_STATE_NOUPDATE         0       //����������, MMS������
#define UPFW_STATE_UPDATING         1       //������
#define UPFW_STATE_COPY             2       //��������
    
#define UPFW_STATE_SEC_ERR          3       //�����ݴ���, MMS���ٴη���, �������3��, ����Ϊ����ʧ��, ������һ������
#define UPFW_STATE_SEC_FAIL         4       //������ʧ��, MMS ������������(ֻ�ǵ�ǰ����������,  ����ȫ����������), ������һ������
#define UPFW_STATE_UPDATEFAIL       5       //����ʧ��, MMS ������������(ֻ�ǵ�ǰ����������, ����ȫ����������), ������һ������
    
#define UPFW_STATE_SEC_OK           6       //������ OK, MMS������һ�����ݶ�
#define UPFW_STATE_UPDATEOK         7       //�������, MMS ������������(ֻ�ǵ�ǰ����������, ����ȫ����������), ������һ������
    
#define UPFW_STATE_OFFLINE          0x80    //DCU ����ʱ, PEDC  ������ MMS ����Ϣ, ʵ���ϲ�������������򷵻ص���Ϣ
#define UPFW_STATE_WAITDOOR         0x81    //�������ݽ�����ɣ�����DCU�����Ų��Ǵ��ڹر�״̬
#define UPFW_STATE_WAITSTATE        0x82    //�յ����������ҪDCU����ģʽ����״̬����Ҫ�󣬲ſ�ʼ������������
#define UPFW_STATE_MstSlvSectionErr 0x83    //�յ����������ݶκţ��������ǼǵĶκŲ���

#define UPFW_STATE_NOSETTING_DCU    0x84    //û����������DCU����
#define UPFW_STATE_MOTOR_ERR        0x85    //DCU ���������ƥ��
#define UPFW_STATE_DISABLE_DCU      0x86    //DCU δʹ������
#define UPFW_STATE_DCU_MCU_ERR      0x87    //DCU оƬ�ͺŲ�ƥ��
#define UPFW_STATE_DCU_CONFIGED     0x8F    //������DCU�յ���������

#define UPFW_STATE_OBJOFFLINE       8       //�����������ߣ�����ʧ��
#define UPFW_STATE_ASKFORDATA       9       //���������ط������ݣ���ǰ��Ҫ�Ķκţ�

#define UPFW_STATE_UPDATES_VER      0x10    //��������Ĵ����Ѿ������°汾������������ʱ���������������У��õ��Ľ����
#define UPFW_STATE_CHECK_FAIL       0x11    //У��ʧ�ܣ��ܾ�����
#define UPFW_STATE_UNKNOWN_VER      0x12    //��֧�ֵ�Э��汾��
#define UPFW_STATE_UNKNOWN_CMD      0x13    //��֧�ֵ�������
#define UPFW_STATE_UNSUPPORTED      0x20    //��֧������

typedef enum
{
    MCUCHIP_UNDEF = 0,
    MCUCHIP_PIC30F6010A,
    MCUCHIP_STM32F767,
    MCUCHIP_PIC32MK1024MCM100,
}tEnmMCUChip;

typedef struct
{
    int16u Year;
    int8u Month;
    int8u Day;
    int8u Hour;
    int8u Minute;

    int8u MainVer;
    int8u SubVer;

    int32u  CS;
}tFileInfo;

typedef struct
{
    int16u      wChkSum;                      //must be here!!! checksum(byte addtion) of this struct except wChkSum
    int16u      wValidFlg;                    //must be here!!! EE_FLG_VALID -- this struct is valid
    int16u      wUpdateState;                 //
    int16u      wTotalSections;
    int16u      wCurrentSections;

    tFileInfo Info;
    tFileInfo HexInfo;
}tUpFwState;

typedef struct
{
    int32u  dwStartAdr;                 //byte address
    int16u  wSectionNo;
    int16u  wCodeBytes;                 //byte, ������Ҫ��������ݳ���
    int8u   *pCode;

    int16u   wUpFwTimeSet;              //ms
    int16u   wUpFwTimeCnt;              //ms
}tUpFwSecMsg;

typedef struct
{
    int32u LedEn;
    int32u *LedPort;
    int32u LedBit;

    int32u UartEn;
    int32u *UartPortSta;
    int32u *UartPortTxReg;
}tUpdate_Tx_Msg;

extern tUpdate_Tx_Msg UpdateTxMsg;

//===============================================
#define Line_Num            4                       //�����Ϣ��Ϊ4�Σ�ÿ��32�ֽ�
#define Column_Num          32
#define BYTES_INFO          128                     //�����Ϣ, ����128�ֽ�, ������ż��
#ifdef _INFO_4_SEC
extern  const int8u strSwDescription[Line_Num][Column_Num];
#else
extern  const int8u strSwDescription[BYTES_INFO];
#endif
extern  int8u DeviceInfo[];

extern  tFileInfo   RxFileInfo;         //MMS����ʱ���͵��ļ���Ϣ
extern  tUpFwState  UpFwState;          //��ǰ����״̬
extern  tUpFwSecMsg UpFwSecMsg;         //��ǰ���նε���Ϣ

//===============================================
extern int16u ChkDataPack(int8u * pSrc);
extern void   UpFwEraseArea(void);

extern void  UpFw_InitShowMsg  (void);
extern void  UpFw_SetCopyEnable(void);
extern void  UpFw_ChkCopyState (void);

//bios function prototype
extern void     bios_Finished(tUpFwState *pState, tUpdate_Tx_Msg *pTxMsg);
extern void     bios_CopyBakFlash(tUpdate_Tx_Msg *pTxMsg);
extern int8u    bios_WriteFlash(int32u dwSrc, int8u *pCode, int16u cntN);
extern int8u    bios_SaveState(tUpFwState *pState);
extern void     bios_ReadState(tUpFwState *pState);

extern void     bios_NVMProgram(void *Addr, int32u *pCode, int32u Len);
extern int32u   bios_NVMUnlock (int32u nvmop);
extern int32u   bios_NVMWriteUint32 (void* address, int32u data);
extern int32u   bios_NVMWriteRow (void* address, void* data);
extern int32u   bios_NVMErasePage(void* address);
extern int32u   bios_NVMErasePFM(void);
extern void     *bios_memcpy(void *dst , const void *src , int32u n);
extern int32u   bios_memcmp(const void *s1, const void *s2, size_t n);
extern void     bios_SoftReset(void);

#endif //_UPDATE_H_


