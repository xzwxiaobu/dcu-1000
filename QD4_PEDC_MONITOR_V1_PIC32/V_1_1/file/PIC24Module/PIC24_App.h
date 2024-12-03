#ifndef _PIC24COMM_H
#define _PIC24COMM_H


#define UART_PIC24_ID               UART3                   /* ͨ�Ŵ��ں�       */
#define UART_PIC24_BR               57600                   /* ������           */
#define UART_PIC24_IPL              INT_PRIORITY_LEVEL_5    /* �ж����ȼ�       */

#define MAINWORK_PIC24A             FALSE
#define MAINWORK_PIC24B             TRUE

/* ��ѯ���� 
 * ���� RS485-2W��uartDrv �����ײ��Զ�����
 * ���ڷ��ͳ�ʱ��
 * �� 57600(1+8+1 bits)��������100�ֽڣ�100*(1+8+1)/57600 ~= 17.4ms
 * ȡ���ͳ�ʱ 30ms
 * ���ǶԷ��ظ���Ҫ��ʱ�䣬ȡ������ѯ��ʱ�� 60ms
 */

#define PIC24_INTURN_TIME           60      /* ��ѯһ�������ʱ�����ƣ�������� PIC24_TX_OVERTIME */
#define PIC24_TX_OVERTIME           30      /* ����ʱ������,ms */
#define PIC24_OFFLINE_NUM           5       /* ͨ�Ŵ���ﵽ�趨ֵ�� �������߱�� */
#if (PIC24_INTURN_TIME <= PIC24_TX_OVERTIME)
#error "PIC24_INTURN_TIME must be lager than PIC24_TX_OVERTIME"
#endif

#define PIC24_SWITCH_ERRNUM         30      /* ͨ�Ŵ���ﵽ�趨ֵ�� �л��� PIC24 */
#define SELECTORPIC24_DELAY_MS      5       /* ѡ��PIC24����ʱ */

PUBLIC tUartComm * getPic24UartCommPtr(void);
PUBLIC BOOL ifPic24UartTxIsBusy(void);

/* ����PEDC���ݰ������н�����ִ����Ӧ�����񲢻ظ�
 * ��Ҫ����������ѭ������
 */
PUBLIC void processPic24Comm(void);

/* ͨ�ô�����ؿ�������MCU���ڳ�ʼ�� */
PUBLIC void initPic24Module(void);

/* ��ȡ��Ҫʵ���������ָ�� */
PUBLIC int8u * getPic24OutputIODataPtr(int * len);

/* ���� PIC24 ����� IO ���� (ͬʱ���� A and B) */
PUBLIC int updatePic24Outdata(int8u *outData, int8u outLen);

PUBLIC int8u getPCA9505ErrCnt(int8u node);
PUBLIC BOOL ifPCA9505Err(int8u node);

#endif //_PIC24COMM_H

