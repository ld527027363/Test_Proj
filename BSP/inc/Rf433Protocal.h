#ifndef __RF433PROTOCAL_H__
#define __RF433PROTOCAL_H__

#include <stdint.h>

#define RF_RESP_NULL				0x00
#define RF_RESP_TEST				   0x10
#define RF_RESP_RESET				   0x12 //恢复出厂参数
#define RF_RESP_WRITE_ID			   0x13//写标签ID
#define RF_RESP_SYNC					0x20
#define RF_RESP_SET_TIME			   0x21//下发时间
#define RF_RESP_GET_HR_INTERVAL		   0x22 //读取心率体温读取间隔
#define RF_RESP_SET_HR_INTERVAL		   0x23 //配置心率体温读取间隔
#define RF_RESP_GET_HR_THRESHOLD	   0x24 //读取心率体温告警阈值
#define RF_RESP_SET_HR_THRESHOLD	   0x25 //配置心率体温告警阈值
#define RF_RESP_GET_DATAFULL_H		   0x26 //读取数据满时处理方式
#define RF_RESP_SET_DATAFULL_H		   0x27 //配置数据满时处理方式
#define RF_RESP_GET_RETX_CNT		   0x28 //读取数据上报最大重发次数
#define RF_RESP_SET_RETX_CNT	       0x29 //配置数据上报最大重发次数
#define RF_RESP_GET_BAT_LEVLE	       0x2A //读取电压值
#define RF_RESP_SET_ALARM			   0x2B //
#define RF_RESP_APDU				   0x5F //

#define TAG_NULL			           0x00
#define TAG_TEST			           0x10
#define TAG_GET_PAR		               0x20
#define TAG_HR				           0x50				//心率上报数据
#define TAG_STEP			           0x51				//步数上报数据
#define TAG_RUN_STEP		           0x52				//跑步数据

#define TAG_SLEEP_TIME_NOON		       0x53				//午间睡眠总长上报数据
#define TAG_SLEEP_DETAIL_NOON 	       0x54				//午间睡眠详情上报
#define TAG_SLEEP_TIME_EVEN		       0x55				//晚间睡眠总长上报数据
#define TAG_SLEEP_DETAIL_EVEN          0x56				//晚间睡眠详情上报

#define TAG_RESET			           0x12
#define TAG_WRITE_ID		           0x13
#define TAG_CLEAR_BUF		           0x20
#define TAG_GET_HR_INTERVAL	           0x22				//心率监测时间间隔
#define TAG_SET_HR_INTERVAL	           0x23
#define TAG_GET_HR_THRESHOLD	       0x24				//心率监测告警门限
#define TAG_SET_HR_THRESHOLD	       0x25
#define TAG_GET_FULL_MATHOD	           0x26				//缓存满处理办法
#define TAG_SET_FULL_MATHOD	           0x27
#define TAG_GET_MAX_REPEAT	           0x28				//最大重发次数
#define TAG_SET_MAX_REPEAT	           0x29
#define TAG_GET_BAT_LEVLE		       0x2A				//电池电量
#define TAG_SET_ALARM			       0x2B
#define TAG_GET_SP_SEND_INTERVAL	   0x2C		//运动数据上报频率
#define TAG_SET_SP_SEND_INTERVAL	   0x2D
#define TAG_GET_SP_SUM_TIME			   0x2E		//运动数据日归结点
#define TAG_SET_SP_SUM_TIME			   0x2F
#define TAG_GET_SLEEP_TIME_NOON		   0x30		//午间睡眠监测时间
#define TAG_SET_SLEEP_TIME_NOON		   0x31	
#define TAG_GET_SLEEP_TIME_EVEN		   0x32		//晚间睡眠监测时间
#define TAG_SET_SLEEP_TIME_EVEN		   0x33	
#define TAG_GET_KEY_FUN				   0x34		//按键定义,两个字节参数，分别代表两按键,0表示按键功能关闭，1为开背光显示，其它未定义
#define TAG_SET_KEY_FUN				   0x35	
#define TAG_UPDATE_START		       0x40				//升级开始
#define TAG_UPDATE_DATA		           0x41				//升级数据
#define TAG_UPDATE_END			       0x42				//升级结束

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

extern unsigned short Rf433CurrentSlot;	//当前时隙计数，包含低两位的副时隙序号
extern unsigned short Rf433ChNum;		// 频道数
extern uint32_t Rf433SlotCap;					//时隙容量
extern uint32_t Rf433EmptySlotCnt;
extern uint8_t Rf433SlotBusy;

extern volatile unsigned char g_rf433IntRecvFlag;

extern uint32_t rf433_get_rec(uint8_t * buf, uint32_t maxLen);

uint8_t Rf433_SendRtcPkt(uint8_t * tag_data);

#endif

