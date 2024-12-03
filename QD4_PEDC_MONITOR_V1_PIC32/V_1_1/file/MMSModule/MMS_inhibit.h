
#ifndef _MMS_INHIBIT_H
#define _MMS_INHIBIT_H

typedef struct MMS_INHIBIT
{
	int8u source_inhibit;                /*监听器ID,255标识为MMS本地发出，其他为远程客户端发出*/
	int8u IP_Field1;                     /*客户端IP.Field1，左起第一*/
	int8u IP_Field2;                     /*客户端IP.Field1，左起第二*/
	int8u IP_Field3;                     /*客户端IP.Field1，左起第三*/
	int8u IP_Field4;                     /*客户端IP.Field1，左起第四*/
	int8u HiB;                           /*客户端端口HiB，高字节*/
	int8u LoB;                           /*客户端端口LoB，低字节*/
	int8u ID;                            /*识别本组参数所对应的站台*/
	int8u operation;                     /*0:取消禁止开门;1:禁止开门*/
	int8u inhibit_buf[8];                /*DCU 命令缓存*/
}defSTRU_MMS_INHIBIT;

/*TX_MMS_INHIBIT_MESSAGE.operation*/
#define COPR_cancelinhibit      0
#define COPR_inhibit            1

#endif

