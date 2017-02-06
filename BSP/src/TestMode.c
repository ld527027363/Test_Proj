
#include "testmode.h"
#include "utility.h"
#include "global.h"
#include "uart.h"
#include "log.h"
#include "SystemParameter.h"
#include "memory.h"
#include "wifi.h"
#include "W5500.h"
#include "dhcp.h"
#include "timer.h"
#include "TcpServer.h"
#include "Rf433Protocal.h"
#include "si446x_api_lib.h"
#include "si446x_bsp.h"

#define TEST_CMD_START					0xF0
#define TEST_CMD_LED						0xF1
#define TEST_CMD_FLASH					0xF2
#define TEST_CMD_BUTTON					0xF3
#define TEST_CMD_RF433					0xF4
#define TEST_CMD_BLE						0xF5
#define TEST_CMD_WIFI						0xF6
#define TEST_CMD_W5500					0xF7
#define TEST_CMD_CLEAR_RECORD		0xF8
#define TEST_CMD_BEEP						0xF9
#define TEST_CMD_END						0xFF

#define TEST_MODE_TIMEOUT_CNT		(20*1000)	//1分钟超时退出测试模式

uint8_t TestMode_Flag = 0;
uint32_t TestMode_Timeout_Cnt = TEST_MODE_TIMEOUT_CNT;
uint32_t Sys_Tick_10ms = 0;

typedef enum
{
	Test_Start,
	Test_LED,
	Test_Flash,
	Test_Beep,
	Test_Rf433,
	Test_Ble,
	Test_Rs485,
	Test_Wifi,
	Test_W5500,
	Test_Waite_Resp,
	Test_Clear_Record,
	Test_End
}TestMode_State_t;

TestMode_State_t TestMode_State;

typedef struct
{
	uint8_t Test_Cmd;
	uint8_t Len;
	uint8_t *Test_Data;
	uint8_t Check_Sum;
	uint8_t Recv_Flag;
}TestMode_Recv_t;

TestMode_Recv_t TestMode_Recv;


typedef struct
{
	uint8_t Len;
	uint8_t Index;
	uint8_t Data[128];
	uint8_t Flag;
}Rs485_Recv_t;

Rs485_Recv_t Rs485_Recv;

typedef struct
{
	uint8_t rx_cnt;
	int8_t rssi_avg;
	int16_t rssi_sum;
}RF433_Recv_t;

RF433_Recv_t RF433_Recv;



uint8_t TestMode_Recv_Index = 0;
uint8_t TestMode_Recv_Len = 0;
uint8_t TestMode_Recv_Buf[128] = {0};


#define TEST_MODE_HEAD		0xAA
typedef struct
{
	uint8_t Test_Head;
	uint8_t Len;
	uint8_t Test_Cmd;
	uint8_t Send_Data[61];
}TestMode_Send_t;

typedef struct
{
	uint8_t Server_IP[4];
	uint16_t Server_Port;
}TestMode_Recv_Info_t;

TestMode_Recv_Info_t TestMode_Recv_Info;

typedef struct
{
	uint8_t BLE_Mac[6];
	uint16_t Server_Port;
}TestMode_Send_Info_t;

static void TestMode_Timeout_Set(uint32_t cnt)
{
	TestMode_Timeout_Cnt = cnt;
}

static void TestMode_Timeout_Check(void)
{
	if(TestMode_Flag)
	{
		TestMode_Timeout_Cnt--;
		DelayMs(1);
		if(!TestMode_Timeout_Cnt)		//测试模式超时重启系统
			sys_restart();
	}
}

#define RS485_RECV_HEAD			0
#define RS485_RECV_LEN			1
#define RS485_RECV_DATA			2
#define RS485_RECV_CHECKSUM		3

#define RS485_RECV_TIMEOUT_CNT	(10)	//100毫秒超时

void Sys_TestMode_RecvData(uint8_t recv_data)
{
	static uint8_t recv_state = 0;
	static uint32_t recv_tick_timeout = 0;

	if(Rs485_Recv.Flag)
		return;
	
	if(recv_tick_timeout <= Sys_Tick_10ms)
		recv_state = RS485_RECV_HEAD;
	recv_tick_timeout = Sys_Tick_10ms + RS485_RECV_TIMEOUT_CNT;
	
	switch(recv_state)
	{

		case RS485_RECV_HEAD:
			if(recv_data == TEST_MODE_HEAD)
			{
				recv_state = RS485_RECV_LEN;
				memset(&Rs485_Recv,0,sizeof(Rs485_Recv));
			}
			break;
			
		case RS485_RECV_LEN:
			if(recv_data < 100)
				recv_state = RS485_RECV_DATA;
			else
				recv_state = RS485_RECV_HEAD;
			Rs485_Recv.Len= recv_data;
			Rs485_Recv.Index = 0;
			break;
				
		case RS485_RECV_DATA:
			Rs485_Recv.Data[Rs485_Recv.Index++] = recv_data;
			if(Rs485_Recv.Index >= (Rs485_Recv.Len-1))
			{
				recv_state = RS485_RECV_CHECKSUM;
			}
			break;
	
		case RS485_RECV_CHECKSUM:
			if(CheckSum(Rs485_Recv.Data,Rs485_Recv.Len) == recv_data)
				Rs485_Recv.Flag = 1;
			recv_state = RS485_RECV_HEAD;
			break;
	
		default:
			break;
	}
}


void Rs485_Start_Recv(void)
{
	Rs485_Recv.Flag = 0;
}

void TestMode_Start_Recv(void)
{
	TestMode_Recv.Recv_Flag = 0;
}

void TestMode_Init(void)
{
	memset(&TestMode_Recv,0,sizeof(TestMode_Recv));
	TestMode_Recv.Test_Cmd = TEST_CMD_END;
}

static void TestMode_Print_RX_Data(uint8_t *data,uint8_t data_len)
{
	uint8_t i = 0;
	
	printf("RX DATA:");
	while(i < data_len)
	{
		printf("%02X ",data[i++]);
	}
	printf("\r\n");
}

void Sys_TestMode_Recv(void)
{
	if(!Rs485_Recv.Flag)
		return;
	Rs485_Start_Recv();
	if(TestMode_Recv.Recv_Flag)
		return;
	
	if((TestMode_Recv.Test_Cmd == TEST_CMD_END)&&(Rs485_Recv.Data[0] != TEST_CMD_START))
		return;
	TestMode_Print_RX_Data(Rs485_Recv.Data,Rs485_Recv.Len);
	if(Rs485_Recv.Data[0] == TEST_CMD_START)
	{
		if((Rs485_Recv.Data[1] != 192)||(Rs485_Recv.Data[2] != 168))
			return;
		TestMode_Recv.Test_Cmd = Rs485_Recv.Data[0];
		strSystemParameter.DistinIp[0] = Rs485_Recv.Data[1];
		strSystemParameter.DistinIp[1] = Rs485_Recv.Data[2];
		strSystemParameter.DistinIp[2] = Rs485_Recv.Data[3];
		strSystemParameter.DistinIp[3] = Rs485_Recv.Data[4];
		strSystemParameter.DistinPort = (Rs485_Recv.Data[5] << 8) + Rs485_Recv.Data[6];
		TestMode_Recv.Recv_Flag = 1;
		TestMode_Flag = 1;
		//初始化参数。。。
	}
	else
	{
		TestMode_Recv.Test_Cmd = Rs485_Recv.Data[0];
		TestMode_Recv.Len = Rs485_Recv.Len;
		TestMode_Recv.Test_Data = Rs485_Recv.Data + 1;
		TestMode_Recv.Check_Sum = Rs485_Recv.Data[Rs485_Recv.Len];
		TestMode_Recv.Recv_Flag = 1;
	}
}

void Sys_TestMode_Resp(uint8_t cmd,uint8_t *data,uint8_t len)
{
	TestMode_Send_t TestMode_Send;
	
	TestMode_Send.Test_Head = TEST_MODE_HEAD;
	TestMode_Send.Len = len + 2;
	TestMode_Send.Test_Cmd = cmd;
	memcpy(TestMode_Send.Send_Data,data,len);
	TestMode_Send.Send_Data[len] = CheckSum(&TestMode_Send.Test_Cmd,len+1);
	Rs485QuerySend((uint8_t *)&TestMode_Send,len+4);
}

void Sys_TestMode_Hannde(void)
{
	uint8_t resp_data[32] = {0};
	uint8_t resp_len = 0;
	
	if((TestMode_Flag)&&(TestMode_Recv.Recv_Flag))
	{	
		TestMode_Start_Recv();
		switch(TestMode_Recv.Test_Cmd)
		{
			case TEST_CMD_START:
				printf("Test Mode Start !\r\n");
				resp_data[0] = 2;			
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_START,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_LED:
				printf("Test LED !\r\n");
				TestMode_LED();
				resp_data[0] = 1;
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_LED,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_FLASH:
				printf("Test Flash !\r\n");
				TestMode_Flash(resp_data);
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_FLASH,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_BUTTON:
				DelayMs(100);
				resp_data[0] = 1;
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_BUTTON,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_BEEP:
				printf("Test Beep !\r\n");
				TestMode_Beep();
				resp_data[0] = 1;
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_BEEP,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_RF433:
				DelayMs(100);
				TestMode_RF433(resp_data);
				resp_data[1] = RF433_Recv.rssi_avg;
				resp_data[2] = RF433_Recv.rx_cnt;
				resp_len = 3;
				Sys_TestMode_Resp(TEST_CMD_RF433,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			case TEST_CMD_BLE:
				printf("Test BLE !\r\n");
				DelayMs(1000);
				FeedDog();
				DelayMs(1000);
				FeedDog();
				DelayMs(1000);
				FeedDog();
				DelayMs(1000);
				FeedDog();
				resp_data[0] = 1;
				resp_data[1] = 'C';
				resp_data[2] = 'o';
				resp_data[3] = 'A';
				resp_data[4] = 'p';
				resp_len = 5;
				Sys_TestMode_Resp(TEST_CMD_BLE,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
//			
//			case TEST_CMD_RS485:
//				printf("Test 485 !\r\n");
//				DelayMs(100);
//				resp_data[0] = 1;
//				resp_len = 1;
//				Sys_TestMode_Resp(TEST_CMD_RS485,resp_data,resp_len);
//				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
//				break;
			
			case TEST_CMD_WIFI:
				printf("Test WIFI !\r\n");
				TestMode_WiFi(resp_data);
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_WIFI,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_W5500:
				printf("Test W5500 !\r\n");
				TestMode_W5500(resp_data);
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_W5500,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;
			
			case TEST_CMD_CLEAR_RECORD:
				printf("Test Clear Records !\r\n");
				TestMode_Clear_Record();
				resp_data[0] = 1;
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_CLEAR_RECORD,resp_data,resp_len);
				TestMode_Timeout_Set(TEST_MODE_TIMEOUT_CNT);
				break;

			case TEST_CMD_END:	
				printf("Test END !\r\n");
				resp_data[0] = 1;
				resp_len = 1;
				Sys_TestMode_Resp(TEST_CMD_END,resp_data,resp_len);	
				AddLog("Test Mode End !\r\n");
		printf("System restart..\r\n");
				DelayMs(10);
				RestartSys();
				break;
			
			default:
				break;
		}
	}
}


void Sys_TestMode_Task(void)
{
	while(TestMode_Flag)
	{
		Sys_TestMode_Recv();
		Sys_TestMode_Hannde();
		TestMode_Timeout_Check();
		FeedDog();
	}
}


void TestMode_LED(void)
{
	uint8_t i = 0;

	for(i = 0; i < 4; i++)
	{
		SET_LED1();
		SET_LED2();
		SET_LED3();
		DelayMs(200);
		CLR_LED1();
		CLR_LED2();
		CLR_LED3();
		DelayMs(300);
		FeedDog();
	}
}


void TestMode_Flash(uint8_t *test_flag)
{
	#if 0
	FLASH_QUEUE temp_queue;

	memcpy(&temp_queue,&TagRecordFlashQueue,sizeof(TagRecordFlashQueue));
	FlashQueue_SaveParam(&temp_queue);

	temp_queue.cnt = 0xFFFF;
	FlashQueue_GetParam(&temp_queue);
	if(temp_queue.cnt == 0xAA55)
		*test_flag = 1;
	else
		*test_flag = 0;

	FlashQueue_SaveParam(&TagRecordFlashQueue);
	#else
	uint8_t temp_buf[32];
	uint8_t tx_buf[32];
	uint8_t rx_buf[32];
	
	Flash_BufferRead(temp_buf,0,32);
	
	Flash_SectorErase(0);
	memset(tx_buf,0xAA,32);
	memset(rx_buf,0,32);
	Flash_Write(tx_buf,0,32);
	Flash_BufferRead(rx_buf,0,32);
	
	Flash_Write(temp_buf,0,32);
	if(memcmp(tx_buf,rx_buf,32))
		*test_flag = 0;
	else
		*test_flag = 1;
	#endif
}


void TestMode_Beep(void)
{
	uint8_t i = 0;

	for(i = 0; i < 5; i++)
	{
		SET_BEEP();
		DelayMs(200);
		CLR_BEEP();
		DelayMs(300);
		FeedDog();
	}
}

static uint8_t rf433_id[4] = {0};


void TestMode_RF433(uint8_t *id)
{
	uint32_t timeout_cnt;
	
	*id = 0;
	memset(&RF433_Recv,0,sizeof(RF433_Recv));
	memcpy(rf433_id,id,4);
	timeout_cnt = 500;
	
	strSystemParameter.RFRxChannel = 1;
	strSystemParameter.RFTxChannel = 21;
	Rf433SendTimeoutFlag = 0;
	si446x_get_int_status(0,0,0);
	vRadio_StartRX(strSystemParameter.RFRxChannel,RF_RX_LEN);
	
	while(1)
	{
		
		if(SysTick10msFlag)
		{
			FeedDog();
			SysTick10msFlag = 0;
			if((RF433_Recv.rx_cnt >= 20)||(timeout_cnt == 1))
			{
				RF433_Recv.rssi_avg = RF433_Recv.rssi_sum/RF433_Recv.rx_cnt;
				*id = 1;
				return;
			}
			timeout_cnt--;	
		}
	}
}

uint8_t TestMode_RF433_Recv(uint8_t *rx_data)
{
	
	if(TestMode_Flag)
	{
//		if(   (rx_data[1] == rf433_id[0])
//			&&(rx_data[2] == rf433_id[1])
//			&&(rx_data[3] == rf433_id[2])
//			&&(rx_data[4] == rf433_id[3]))
		if(   (rx_data[1] < 30)
			&&(rx_data[2] < 30)
			&&(rx_data[3] < 30)
			&&(rx_data[4] < 30))
			{
				RF433_Recv.rssi_sum += (rx_data[10]/2 - 116);
				RF433_Recv.rx_cnt++;
				Rf433_SendRtcPkt(rx_data);
				DelayMs(10);		
				Rf433SendTimeoutFlag = 0;
				si446x_get_int_status(0,0,0);
				vRadio_StartRX(strSystemParameter.RFRxChannel,RF_RX_LEN);
				return 1;
			}
	}
	return 0;
}


uint8_t TestMode_WiFi_Flag = 0;

void TestMode_WiFi(uint8_t *test_flag)
{
	uint8_t wifi_init_cnt = 0;

	*test_flag = 0;
	TestMode_WiFi_Flag = 0;
	WIFI_ready();
	while( WIFI_Init()!= 1)
	{
		wifi_init_cnt++;
		DelayMs(100);
		WIFI_ready();
		DelayMs(100);
		FeedDog();
		if(wifi_init_cnt >= 5) 
			return;
	}
	*test_flag = 1;
}


void TestMode_W5500(uint8_t *test_flag)
{
	uint32_t timeout_cnt = 2000;		//20秒超时

	*test_flag = 0;
	
	W5500_Reset();

	Init_DHCP_Cient();	

	FeedDog();//初始化DHCP客户端 
	
	Init_DHCP_Socket();  

	FeedDog();

	while(1)
	{
		if(SysTick10msFlag)
		{
			FeedDog();
			SysTick10msFlag = 0;
			DHCP_QueryTask();
			if(DHCP_GetSuccessFlag == 1)  //Dhcp suc
			{
				DHCP_GetSuccessFlag = 0;
				DHCP_Req = 0;
				printf("DHCP Get IP Suc\r\n");
				*test_flag = 1;
				return;
			}
			if(!timeout_cnt--)
				return;			
		}
	}
	
	
//	if( W5500Init() == 1) 
//	{
//		TcpServer_listen(2,5432);
//		printf("NetChip is OK\r\n");
//	}
//	else 
//	{
//		printf("NetChip is Error\r\n");
//	}
}


void TestMode_Clear_Record(void)
{
	printf("Del Tag Data Start!\r\n");
	Flash_ChipErase();
	printf("Del Tag Data OK!\r\n");
}




















