#ifndef __RS485_H__
#define __RS485_H__


#define  EN_RS485_DEBUG	  (1)   //调试打印信息



#include "types.h"

typedef enum RS485_PRO_TYPE_E
{
	RS485_PRO_UNKNOW = 0,  //套X
	RS485_PRO_P1 ,  //套1
	RS485_PRO_P2 ,
	RS485_PRO_P3 ,
}RS485_PRO_TYPE;


typedef struct rs485_pkt_s
{
	u8_t  head;
	u8_t  len[2];
	u8_t  addr;
	u8_t  cmd;
	u8_t  dat[8192];
}rs485_pkt_t;


typedef struct rs485_p1_stu_s
{
	u8_t head;
	u8_t len[2];
}rs485_p1_stu_t,*rs485_p1_stu_p;

typedef struct rs485_p2_stu_s
{
	u8_t head;
	u8_t len[2];
}rs485_p2_stu_t,*rs485_p2_stu_p;

typedef struct rs485_p3_stu_s
{
	u8_t head;
	u8_t len[2];
}rs485_p3_stu_t,*rs485_p3_stu_p;


typedef struct rs485_info_s
{
	u8_t client_addr;
	BOOL bSending;
}rs485_info_t;

#define RS485_BIT_HEAD    (0X02)

#define RS485_LEN_HEAD		(3)       //
#define RS485_LEN_OTHER		(2) 
#define RS485_LEN_CRC			(1)

#define RS485_IDX_DAT			(5)       //数据下标

#define RF433_RECORD_LEN			19
#define RS485_MAX_RECORDS_PR0_3		(RF433_RECORD_LEN*(20+1))	//一次最多发送20条数据 + 1为预留空间--LD 20160704

#define RS485_CMD_NULL   	0X00
#define RS485_CMD_TIME   	0X04
#define RS485_CMD_ADDR   	0X03
#define RS485_CMD_DAT   	0X06		//发送记录
#define RS485_CMD_VER   	0X07
#define RS485_CMD_CYPE   	0X08
#define RS485_CMD_RESET   0X0A
#define RS485_CMD_TRANS   0X0B
#define RS485_CMD_TOKEN   0X10
#define RS485_CMD_SEND   	0X11
#define RS485_CMD_END		  0X12

void rs485_set_pro(RS485_PRO_TYPE type);

RS485_PRO_TYPE rs485_get_pro(void);


extern void rs485_set_addr(u8_t addr);

extern u32_t rs485_pack_head(u8_t *pSendBuf, u8_t cmd ,u8_t *pSendDat, u32_t len);

extern u32_t rs485_pack(u8_t *pSendBuf, u8_t cmd ,u8_t *pSendDat, u32_t len);



extern void rs485_SendStart(unsigned char *pBuf, unsigned short len);

extern void rs485_sendProcess(void);

extern void rs485_CommTask(void);


extern void uart_set_dat(u8_t dat);

void rs485_init(void);

/*
**  处理 485接收
*/
void rs485_proc_recv(void);


extern rs485_info_t s_rs485_info;

//获取令牌 后状态
#define rs485_set_sending() do{\
	s_rs485_info.bSending = TRUE;\
}while(0);
	
#define rs485_clr_sending() do{\
	s_rs485_info.bSending = FALSE;\
}while(0);


#endif

