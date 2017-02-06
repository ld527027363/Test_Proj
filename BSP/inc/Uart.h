#ifndef __SRCUART_H
#define __SRCUART_H


#define UART1_BAUDRATE		115200
#define UART2_BAUDRATE		115200
#define UART3_BAUDRATE		115200
#define UART6_BAUDRATE		115200


typedef struct
{
	unsigned char send_buf[128];
	unsigned char rcv_buf[192];                   
	unsigned short rcv_len;
	unsigned char rcv_state;
	unsigned char rcv_all_flag;         
	unsigned char send_flag ;
	unsigned short send_data_flag;
	unsigned short send_index;
	unsigned short timeout_cnt;
	unsigned char  g_bluetooth_flag;
	unsigned char  other_flag;
	unsigned char  last_recvLen;
	unsigned char  last_index;
	unsigned char  overlap_cnt;
}STRUCT_UART_PARATERS;

typedef struct
{
	unsigned char *send_buf;
	unsigned char *rcv_buf;     
	unsigned short rcv_index;               	//已接收序号
	unsigned short rcv_len;			//预接收长度
	unsigned char rcv_state;			//接收状态机值
	unsigned char rcv_all_flag;         	//接收完成标识
	unsigned char send_flag ;			//正在发送标识
	unsigned char send_state;			//发送状态机值
	unsigned short send_len;			//预发送总厂
	unsigned short send_index;		//已发送序号
	unsigned int   timeout10ms; //10ms
	unsigned int   cur10ms; //10ms
	unsigned char  timeout_flag;
}STRUCT_LONG_UART_PAR;

void USART1_Config(void);
void USART2_Config(void);

extern void USART3_Config(void);

void USART6_Config(void);
void DebugMessage(unsigned char *buf);

void USART_NVIC_Config(void);

void PrintQuerySend(unsigned char *buf,unsigned short len);
void Rs485QuerySend(unsigned char *buf,unsigned short len);
void WifiQuerySend(unsigned char *buf,unsigned short len);

void WIFI_SendStr(char *str);

void Rs485DmaInit(void);
void WIFI_SendStart(unsigned char *pBuf, unsigned short len);
void RS485SendStr(unsigned char *str);
void WifiDmaInit(void);
void RS485_SendStart(unsigned char *pBuf, unsigned short len);
void RS485AndWifiBufferConfig(void);
void UartBlueSend(unsigned char *buf,unsigned short len);

extern STRUCT_UART_PARATERS DebugUartPort;
extern STRUCT_LONG_UART_PAR Rs485UartPort,WifiUartPort;
extern unsigned char WIFI_DMA_SendOK;


#endif

