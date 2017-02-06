#include "global.h"
#include "radio.h"
#include "radio_hal.h"
#include "si446x_api_lib.h"
#include "Rf433Protocal.h"

#include "Protocol.h"
#include "Protocol_typedef.h"
#include "linklayer.h"
#include "log.h"
#include "buffer.h"
#include "board_conf.h"
#include "W5500.h"

#include "interface.h"
#include "wifi.h"
#include "adc.h"
#include "rs485.h"
#include "uart.h"
#include "timer.h"
#include "rtc.h"
#include "TcpServer.h"

#include "dhcp.h"
#include "upgrade.h"

#include "sys_conf.h"
#include "SystemParameter.h"

#include "debug.h"
#include "testmode.h"
#include "battry.h"


static u32_t s_bRfResetCnt = 0;
static u32_t s_bCommResetCnt = 0;
	
void PrintRestReson(void)
{
//	printf("PWR->CSR = %08X\r\n",PWR->CSR); 
//	if((PWR->CSR & (1<<31)) != 0)
//		printf("Low Power Rest\r\n"); 
//	if((PWR->CSR & (1<<30)) != 0)
//		printf("WWDG Rest\r\n"); 
//	if((PWR->CSR & (1<<29)) != 0)
//		printf("IWDG Rest\r\n"); 
//	if((PWR->CSR & (1<<28)) != 0)
//		printf("SF Rest\r\n"); 
//	if((PWR->CSR & (1<<27)) != 0)
//		printf("POR/PDR Rest\r\n"); 
//	if((PWR->CSR & (1<<26)) != 0)
//		printf("PIN Rest\r\n"); 
//	if((PWR->CSR & (1<<25)) != 0)
//		printf("BOR Rest\r\n"); 
//	PWR->CSR |= (1<<24);
}


void PrintDeviceInfo(void)
{	
	FeedDog();
	printf(">>Version:%d(%s) \r\n>>DeviceID:%s%s\r\n", MYVERSION,VERSION_STR,BOARDVersion, strSystemParameter.ProductSn);
	printf(">>Param Version:%s (--HB:%d--ST:%d--SC:%d)\r\n",strSystemParameter.Param_Ver,strSystemParameter.HraerBeatTimeout,strSystemParameter.SendDataDealyTime,strSystemParameter.SendDataMaxCnt);
	printf(">>Ip Addr:%03d.%03d.%03d.%03d\r\n", strSystemParameter.LocalIp[0],strSystemParameter.LocalIp[1],strSystemParameter.LocalIp[2],strSystemParameter.LocalIp[3]);
	printf(">>SubMask:%03d.%03d.%03d.%03d\r\n",strSystemParameter.LocalSubWay[0],strSystemParameter.LocalSubWay[1],strSystemParameter.LocalSubWay[2],strSystemParameter.LocalSubWay[3]);
	printf(">>GateWay:%03d.%03d.%03d.%03d\r\n",strSystemParameter.LocalGateway[0],strSystemParameter.LocalGateway[1],strSystemParameter.LocalGateway[2],strSystemParameter.LocalGateway[3]);
	printf(">>DistIP :%03d.%03d.%03d.%03d\r\n",strSystemParameter.DistinIp[0],strSystemParameter.DistinIp[1],strSystemParameter.DistinIp[2],strSystemParameter.DistinIp[3]);
	printf(">>DistPort:%d\r\n", strSystemParameter.DistinPort);
	printf(">>WorkMode:%.2X(0:485, 1:Lan, 2:WIFI)\r\n",strSystemParameter.WorkMode);
	//printf(">>BeepFlag:%.2X\r\n",strSystemParameter.BeepFlag);  
	printf(">>DHCPEnable:%.2X\r\n",strSystemParameter.DHCPEnable);    
	printf(">>WIFI User:%s, WIFI PWD:%s\r\n", strSystemParameter.WIFISSID,strSystemParameter.WIFIPass);
	printf(">>Flash Buf tag cnt:%d \r\n\r\n",TagRecordFlashQueue.cnt); 
	
	
}




void ServerCommPortInit(unsigned char dhcp_req)
{
	unsigned char  bWifiErrorTime = 0;
	
	if (sys_check_linktype(COMM_PORT_RS485)) //rs485
	{
		FeedDog();
		
		printf("[%02d:%02d:%02d]",SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);
		printf("RS485 init success \r\n");
	}
	else if(sys_check_linktype(COMM_PORT_WIFI))
	{
		FeedDog();
		printf("WIFI Initial\r\n");
		bWifiErrorTime = 0;
		while( WIFI_Init()!= 1) 
		{
			bWifiErrorTime++;
			DelayMs(100);
			WIFI_ready();
			DelayMs(100);
			FeedDog();
			if(bWifiErrorTime >= 3) 
			{
				printf("WIFI Module is broken\r\n");
				break;
			}
		}
	}
	// else if(strSystemParameter.WorkMode == 0x01)
	{
		FeedDog();
		printf("EtherNetInit\r\n");
		SPI3_Config(); //LAN模块
		if(dhcp_req != 1)
		{
			if( W5500Init() == 1) 
			{
				TcpServer_listen(2,5432);
				printf("NetChip is OK\r\n");
			}
			else 
				printf("NetChip is Error\r\n");
			
			
		}
		else
		{   
			W5500_Reset();

			Init_DHCP_Cient();	

			FeedDog();//初始化DHCP客户端 
			Init_DHCP_Socket();  

			FeedDog();
		}
	}
}

void LanCommQueryTask(void)
{
		if(sys_check_linktype(COMM_PORT_LAN))        
		{
			if(DHCP_Req == 1)
			{
				DHCP_QueryTask();
				if(DHCP_GetSuccessFlag == 1)  //Dhcp suc
				{
					DHCP_GetSuccessFlag = 0;
					DHCP_Req = 0;
					printf("DHCP Get IP Suc\r\n");
					ServerCommPortInit(DHCP_Req);
				}
			}
			else 
			{
				if(g_bStartUpgradeFlag != 1)
				{
					Lan_loopback_tcps();
				}
			}
		}	
		
}

void TenMiliSendcondLookup(void)
{
	if(SysTick10msFlag)
	{
		SysTick10msFlag = 0;
		if (sys_check_linktype(COMM_PORT_RS485))
		{
			//rs485_CommTask();
			
			rs485_proc_recv();
		}
		else if (sys_check_linktype(COMM_PORT_WIFI))
		{
				WIFI_CommTask();
		}
		else
		{
			LanCommQueryTask();
		}
		
		if (DHCP_Req == 0)
		tcp_server_loopback();
		
		rs485_CommTask();
		
	}
}

extern uint8 g_ble_state;
extern uint32 g_time_idel;

static uint8 g_conn_led = 1;

void OneSendcondLookup(void)
{
	unsigned char bReturn = FALSE;
	static unsigned char bLedFlag = 0;
	static BOOL bFlag = FALSE;

	if(SysTick1sFlag)
	{
		SysTick1sFlag = 0;
			
		//登录后绿灯长亮，平常闪烁
		if(g_bServerLoginSucFlag == 1)
		{
			g_iServerUploadCommTimeCnt++;	
			g_iServerHeartCommTimeCnt++;
			
		
			SET_LED1();
		
		}
		else
		{
			if(bLedFlag == 0) 
			{
				bLedFlag = 1; 
				SET_LED1();
			}
			else 
			{
				bLedFlag = 0; 
				CLR_LED1();
			}
		}
		
		if((SysTick1sCnt & BATTRY_GET_GAP) == 0)
			Battry_Flag = 1;
		else if(Battry_Flag == 2)
			Battry_Flag = 1;  
		
		RTC_GetTime(RTC_Format_BIN, &SystemTime); //获取时间
		RTC_GetDate(RTC_Format_BIN, &SystemDate); //获取日期	
		
		if(g_bServerLoginSucFlag)		//没连上服务器就不给手环配时间
		{
			g_bUploadOkFlag = TRUE;
			TimeFormatToInt();
		}
		else if (!g_bServerLoginSucFlag)
		{
			g_bUploadOkFlag = FALSE;
			SystemTimeSimple = 0;
		}
		
		if(g_bStartUpgradeFlag != 1) 
		{
			if (rs485_get_pro() != RS485_PRO_P3)
			{
				Server_CommTask();
			}
		}
			
		if(g_bCommReceiveFlag == 1)
		{
			FeedDog();
			g_bCommReceiveFlag = 0;
			bReturn = Pprotocol->getresponse(Pprotocol);
			
		
			if(bReturn == PR_SUCCESS) 
			{
				
				if(g_bStartUpgradeFlag != 1) 
					Server_RcvDataHandle();
			} 

		}
		
		if((SysTick1sCnt & 0xFF) == 0)			//   128s
		{
			Rf433_SlotRecycle();
		}
		
		if (SystemTimeSimple !=0 && SystemTime.RTC_Hours >7 && SystemTime.RTC_Hours < 18) 
		{
			
			if(!g_bUploadOkFlag && (TestReq == 0))
			{
				if ((SysTick1sCnt - s_bCommResetCnt) > 600)
				{
					SYS_RESTART();
				}
			}

			
			//2016-0305 rf433异常检测
			if (g_rf433IntRecvFlag)
			{
				g_rf433IntRecvFlag = FALSE;		
				bFlag = FALSE;
				s_bRfResetCnt = SysTick1sCnt;
			}
			else
			{		
				 if (SysTick1sCnt - s_bRfResetCnt > DEASOM_RF433_TIMER_L2)
				{
					SYS_RESTART();
				}
				else if (SysTick1sCnt - s_bRfResetCnt > DEASOM_RF433_TIMER_L1)
				{
					if (!bFlag)
					{
						vRadio_Init();	
						si446x_part_info();	
						vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
						EXTI_Config();	
						
						AddLog("rf433 chip reset.\r\n");
						
						DEBUG_PRINTF("rf433 chip reset.\r\n");  
						
						bFlag = TRUE;
					}					
				}
			}
		}
		
	
		
	//	printf("+%d.\r\n",Rf433CurrentSlot/4);
	}	
	//ble 60秒超时断开
	 switch(g_ble_state)
	{
	case 1:
		if (SysTick1sCnt - g_time_idel  >= conn_timeout_sec && g_time_idel != 0)
		{
			StartBeep(200,1);
			sendBlueDisconn(100);
			g_time_idel = 0;
		}
		//亮灯
		if (SysTick100msFlag)
		{
			SysTick100msFlag = 0;
			if (g_conn_led)
			{
				SET_LED2();
			}
			else
			{
				CLR_LED2();
			}
			g_conn_led = !g_conn_led;
		}
		
		break;
	case 2:
		if (SysTick1sCnt - g_time_idel  >= ble_timeout_sec && g_time_idel != 0)
		{
			StartBeep(200,1);
			sendBlueDisconn(100);
			g_time_idel = 0;
		}
		break;
	case 3:
		break;
	}
	
		
}


/*
** 守护任务 监控系统正常
*/
void DeamonTask(void)
{
	
	if (g_sysrestart_flag)
	{
		//g_sysrestart_flag = FALSE;
		sys_restart();
	}
	
}


void TestTask(void)
{
	unsigned char buf[10] = {0xFF};

	if(TestReq == 0)
		return;
	if(TestReq&TEST_REQ_RF_TX)
	{
		vRadio_StartTx(RF_FREQ_TX_CHANNEL, RF_TX_LEN,buf);
		Delay_100us(100);
	}	
}

void HardwareInit(void)
{
	volatile uint32_t adc_data_1 = 0;
	volatile uint32_t adc_data_2 = 0;
	volatile uint32_t adc_data_3 = 0;
	volatile uint32_t adc_data_4 = 0;
	
	NVIC_Configuration();	
	NormalGpioConfig();
	USART6_Config(); 
	PrintRestReson();
	InitWatchdog(4000);
	DelayMs(1000);
	FeedDog();
	USART1_Config();
	USART3_Config();
	USART_NVIC_Config();
	
	SPI2_Config();  //FLASH
	RTC_Config();
	SetDefaultTime();
	Battry_AdcInit();
	HardWare_AdcInit();
	Time7Init();
	USART2_Config();
	Rev485En2();
	Rs485DmaInit(); 
	WifiDmaInit(); 
	RS485AndWifiBufferConfig();
	FeedDog();
	ParameterInitial();
	SPI1_Config(); //433M模块
	Flash_Init();

	
	rs485_init();
	vRadio_Init();	
	si446x_part_info();	
	WIFI_ready();
	
	
	//if (strSystemParameter.BeepFlag < 2)
	{
		strSystemParameter.BeepFlag = 1;
	}
	rs485_set_addr(strSystemParameter.BeepFlag);
	
	printf("\r\nHardware Init Success!\r\n\r\n");   
}



int main(void)
{
	//int i,j;
	
	HardwareInit();
 
	TestMode_Init();
	Rf433_SlotTabInit();
	ProtocolInitial();
	LogInit();
	AddLog("Power on.\r\n");
	StartBeep(20,1);//上电鸣叫
	FeedDog();
 
	sendBlueReset();
	BufQueueInit(&TagDataQueue, TagBuf,TAG_RECORD_BUF_SIZE, TAG_RECORD_LEN);	
	Flash_TagRecordQueueInit();	
//	FlashQueue_ClearQueue(&TagRecordFlashQueue);
	PrintDeviceInfo();
	FeedDog();	
	vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
	EXTI_Config();		
	
	if(strSystemParameter.DHCPEnable == 1) 
		DHCP_Req = 1;
	FeedDog();

	ServerCommStateInit();	
	ServerCommPortInit(DHCP_Req);
	
	
	CLR_LED3();
	FeedDog();
	
	/* 系统接口 初始化 */
	//intf_init();
	//intf_add(INTF_TYPE_UART1,INTF_FLAG_UART1,uart1_send,uart1_recv_callback,uart1_check);
	
	DEBUG_PRINTF("System Power ON!!\r\n");
//	printf("[20%02d-%02d-%02d %02d:%02d:%02d] System Power ON!!",SystemDate.RTC_Year,SystemDate.RTC_Month,SystemDate.RTC_Date,
//	SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);
	
	s_bCommResetCnt = SysTick1sCnt;
//	strSystemParameter.HraerBeatTimeout = 60;
//	strSystemParameter.Param_Ver[0] = 'V';
//	strSystemParameter.Param_Ver[1] = '0';
//	strSystemParameter.Param_Ver[2] = '.';
//	strSystemParameter.Param_Ver[3] = '9';
//	strSystemParameter.Param_Ver[4] = '\0';
	while(1)
	{ 
		FeedDog();
		Sys_TestMode_Task();
		if(g_bStartUpgradeFlag == 1)
		{
			UpgradeOperation();
		}

		LinkPrintPortRecv();
		if((g_iServerHeartCommTimeCnt >= strSystemParameter.HraerBeatTimeout) && (g_bServerLoginSucFlag == 1)&&(g_bServerUploadReq == 0))	// gaojun 2015-08-19
		{
			//StartBeep(20,1);//
			g_iServerHeartCommTimeCnt = 0;
			g_iServerUploadCommTimeCnt = 0; 
			g_bServerUploadReq = 0;	      
			g_bServerHeartbeatReq = 1; //心跳包请求 
		}
		if((TagRecordFlashQueue.cnt)&& (g_bServerLoginSucFlag == 1))
		{
			if((g_iServerUploadCommTimeCnt >= strSystemParameter.SendDataDealyTime)||(TagRecordFlashQueue.cnt >= strSystemParameter.SendDataMaxCnt))
			{   
				//CLR_LED3();
				g_bServerUploadReq = 1;
				g_iServerUploadCommTimeCnt = 0;
			}
		}
		TenMiliSendcondLookup();
		
		OneSendcondLookup();
		
		Battry_Task();
		
		RF433_Task();
		
		TestTask();
    
		DeamonTask();
	}	
}


