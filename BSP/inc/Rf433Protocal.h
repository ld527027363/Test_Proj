#ifndef __RF433PROTOCAL_H__
#define __RF433PROTOCAL_H__

#include <stdint.h>

#define RF_RESP_NULL				0x00
#define RF_RESP_TEST				   0x10
#define RF_RESP_RESET				   0x12 //�ָ���������
#define RF_RESP_WRITE_ID			   0x13//д��ǩID
#define RF_RESP_SYNC					0x20
#define RF_RESP_SET_TIME			   0x21//�·�ʱ��
#define RF_RESP_GET_HR_INTERVAL		   0x22 //��ȡ�������¶�ȡ���
#define RF_RESP_SET_HR_INTERVAL		   0x23 //�����������¶�ȡ���
#define RF_RESP_GET_HR_THRESHOLD	   0x24 //��ȡ�������¸澯��ֵ
#define RF_RESP_SET_HR_THRESHOLD	   0x25 //�����������¸澯��ֵ
#define RF_RESP_GET_DATAFULL_H		   0x26 //��ȡ������ʱ����ʽ
#define RF_RESP_SET_DATAFULL_H		   0x27 //����������ʱ����ʽ
#define RF_RESP_GET_RETX_CNT		   0x28 //��ȡ�����ϱ�����ط�����
#define RF_RESP_SET_RETX_CNT	       0x29 //���������ϱ�����ط�����
#define RF_RESP_GET_BAT_LEVLE	       0x2A //��ȡ��ѹֵ
#define RF_RESP_SET_ALARM			   0x2B //
#define RF_RESP_APDU				   0x5F //

#define TAG_NULL			           0x00
#define TAG_TEST			           0x10
#define TAG_GET_PAR		               0x20
#define TAG_HR				           0x50				//�����ϱ�����
#define TAG_STEP			           0x51				//�����ϱ�����
#define TAG_RUN_STEP		           0x52				//�ܲ�����

#define TAG_SLEEP_TIME_NOON		       0x53				//���˯���ܳ��ϱ�����
#define TAG_SLEEP_DETAIL_NOON 	       0x54				//���˯�������ϱ�
#define TAG_SLEEP_TIME_EVEN		       0x55				//���˯���ܳ��ϱ�����
#define TAG_SLEEP_DETAIL_EVEN          0x56				//���˯�������ϱ�

#define TAG_RESET			           0x12
#define TAG_WRITE_ID		           0x13
#define TAG_CLEAR_BUF		           0x20
#define TAG_GET_HR_INTERVAL	           0x22				//���ʼ��ʱ����
#define TAG_SET_HR_INTERVAL	           0x23
#define TAG_GET_HR_THRESHOLD	       0x24				//���ʼ��澯����
#define TAG_SET_HR_THRESHOLD	       0x25
#define TAG_GET_FULL_MATHOD	           0x26				//����������취
#define TAG_SET_FULL_MATHOD	           0x27
#define TAG_GET_MAX_REPEAT	           0x28				//����ط�����
#define TAG_SET_MAX_REPEAT	           0x29
#define TAG_GET_BAT_LEVLE		       0x2A				//��ص���
#define TAG_SET_ALARM			       0x2B
#define TAG_GET_SP_SEND_INTERVAL	   0x2C		//�˶������ϱ�Ƶ��
#define TAG_SET_SP_SEND_INTERVAL	   0x2D
#define TAG_GET_SP_SUM_TIME			   0x2E		//�˶������չ���
#define TAG_SET_SP_SUM_TIME			   0x2F
#define TAG_GET_SLEEP_TIME_NOON		   0x30		//���˯�߼��ʱ��
#define TAG_SET_SLEEP_TIME_NOON		   0x31	
#define TAG_GET_SLEEP_TIME_EVEN		   0x32		//���˯�߼��ʱ��
#define TAG_SET_SLEEP_TIME_EVEN		   0x33	
#define TAG_GET_KEY_FUN				   0x34		//��������,�����ֽڲ������ֱ����������,0��ʾ�������ܹرգ�1Ϊ��������ʾ������δ����
#define TAG_SET_KEY_FUN				   0x35	
#define TAG_UPDATE_START		       0x40				//������ʼ
#define TAG_UPDATE_DATA		           0x41				//��������
#define TAG_UPDATE_END			       0x42				//��������

#define TEST_GET_HR			           0x01
#define TEST_RF433			           0x03
#define TEST_BT				           0x04
#define TEST_LCD			           0x05

#define RF_FREQ_BASE			       470000000
#define RF_FREQ_TX_CHANNEL		      26//50
#define RF_FREQ_RX_CHANNEL		     6 

#define RF_TX_LEN		               10//20
#define RF_RX_LEN		               10//20

#define RF_433_TX_TIMEOUT              33000
#define RF_433_RX_TIMEOUT		       33000

#define RF_ERROR_OK		               0
#define RF_ERROR_SEND_ERR              1
#define RF_ERROR_RCV_ERR	           2

typedef struct
{
	unsigned char sof;
	unsigned char id[4];
	unsigned char tag;
	unsigned char value[2];
	unsigned char tim[2];	
	unsigned char APtim[6];
}STRUCT_RF433_RX_FRAME;

typedef struct
{
	unsigned char sof;
	unsigned char tag_crc[2];
	unsigned char reader_id;	
	unsigned char rsv;
  unsigned char cfg[5];
}STRUCT_RF433_TX_FRAME;

void Rf433_SlotTabInit(void);
uint8_t Rf433_RspTag(unsigned char *tag_data,unsigned char cmd,unsigned char*param,unsigned char stop_flag);
uint8_t Rf433_CheckTagSlot(uint8_t *tag_id, uint32_t *correct_slot);
uint32_t Rf433_ObtainNewSlot(uint8_t *tag_id);
uint32_t Rf433_ObtainCurrentSlot(uint8_t *tag_id);
uint32_t Rf433_CorrectTagSync(uint8_t *tag_data);
uint8_t Rf433_SyncTag(uint8_t * tag_data);
void Rf433_RcvDataHandle(unsigned char *rf_data);
void Rf433_IntHandle(void);
void RF433_Task(void);

void Rf433_SlotRecycle(void);

extern unsigned char Rf433RxData[];
//extern unsigned short Rf433ChAndCap;
//extern unsigned short Rf433Slot;

extern unsigned char Rf433SendTimeoutCnt;
extern unsigned char Rf433SendTimeoutFlag;

extern unsigned short Rf433CurrentSlot;	//��ǰʱ϶��������������λ�ĸ�ʱ϶���
extern unsigned short Rf433ChNum;		// Ƶ����
extern uint32_t Rf433SlotCap;					//ʱ϶����
extern uint32_t Rf433EmptySlotCnt;
extern uint8_t Rf433SlotBusy;

extern volatile unsigned char g_rf433IntRecvFlag;

extern uint32_t rf433_get_rec(uint8_t * buf, uint32_t maxLen);

uint8_t Rf433_SendRtcPkt(uint8_t * tag_data);

#endif

