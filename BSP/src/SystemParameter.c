/********************************************************
 * File	       :  SystemParameter.c
 * Description :  系统参数、重启等操作
 * Author      :  hcp
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2016-01-1
 * History     :

*/

#include "global.h"
#include "log.h"
#include "main.h"
#include "Tcpserver.h"
#include "W5500.h"
#include "debug.h"
#include "memory.h"
#include "mcu_flash.h"
#include "timer.h"
#include "wifi.h"
#include "buffer.h"

#include "sys_conf.h"
#include "uart.h"
#include "SystemParameter.h"
#include "testmode.h"




u8_t DHCP_Req = 0;			//DHCP 申请

bool g_sysrestart_flag = FALSE;

uint8_t Battry_Flag = 0;
uint8_t Battry_Alert_Flag = 0;
uint8_t Err_Code[2] = {0};


#define UART_CMD_GET_PRAM					0x01
#define UART_CMD_SET_PRAM					0x02
#define UART_CMD_GET_LOG_CNT				0x03
#define UART_CMD_GET_ALL_LOG				0x04
#define UART_CMD_ERASE_ALL_DATA			0x05
#define UART_CMD_GET_FLASH_DATA			0x06
#define UART_CMD_GET_STATE					0x07
#define UART_CMD_RESET						0x08
#define UART_CMD_ENTER_TEST_MODE			0x10
#define UART_CMD_EXIT_TEST_MODE			0x11
#define UART_CMD_GET_VERSION				0x09

#define UART_CMD_GET_AP             0x0B

#define TEST_CMD_EREASE             0x30
#define TEST_CMD_DOWNLOAD_START			0x40
#define TEST_CMD_DOWNLOAD_ING       0X41
#define TEST_CMD_DOWNLOAD_END       0X42

extern uint8 g_get_ap_flag;
extern uint32 WIFI_get_ap(uint8 *apBuf);

extern u8_t  g_bServerLoginSucFlag;

Struct_System_Parameters strSystemParameter;
u8_t g_bStopReadFlag = 0;

extern u8_t TestReq;

unsigned int Battry_Value = 0;
unsigned int Power_Value = 0;

/*
* 设置系统默认参数
*
*/
void SetDefaultParameter(void)   
{
	strSystemParameter.InitialFlag = SYS_INIT_FLAG;     

	strSystemParameter.WorkMode = 0x01;
	memset(strSystemParameter.ProductSn,0,18);
	strcpy((char *)strSystemParameter.ProductSn, "0000000000000001");

	strSystemParameter.BeepFlag = 1;

	strSystemParameter.DHCPEnable = 1; 

	strSystemParameter.LocalIp[0]=192; 
	strSystemParameter.LocalIp[1]=168; 
	strSystemParameter.LocalIp[2]=1; 
	strSystemParameter.LocalIp[3]=199;

	strSystemParameter.LocalSubWay[0]=255; 
	strSystemParameter.LocalSubWay[1]=255; 
	strSystemParameter.LocalSubWay[2]=255; 
	strSystemParameter.LocalSubWay[3]=0;

	strSystemParameter.LocalGateway[0]=192; 
	strSystemParameter.LocalGateway[1]=168; 
	strSystemParameter.LocalGateway[2]=1; 
	strSystemParameter.LocalGateway[3]=2;   

	strSystemParameter.LocalPort = 65500;

	strSystemParameter.DistinIp[0]=115; 
	strSystemParameter.DistinIp[1]=29; 
	strSystemParameter.DistinIp[2]=200; 
	strSystemParameter.DistinIp[3]=219;

//	strSystemParameter.LocalPort = 5000;

//	strSystemParameter.DistinIp[0]=192; 
//	strSystemParameter.DistinIp[1]=168; 
//	strSystemParameter.DistinIp[2]=11; 
//	strSystemParameter.DistinIp[3]=100;

	strSystemParameter.DistinPort = 80; 

	strcpy((char *)strSystemParameter.WIFISSID,"MTII");
	strcpy((char *)strSystemParameter.WIFIPass,"mti26546392"); 

	strSystemParameter.WIFICypt = 0x23;

	strSystemParameter.RFTxChannel = 26;
	strSystemParameter.RFRxChannel = 6;
	
	
	strSystemParameter.HraerBeatTimeout = UPLOADHEART_TIME;
	strSystemParameter.SendDataDealyTime = UPLOADDATA_TIME;
	strSystemParameter.SendDataMaxCnt = UPLOADDATA_RECORD_CNT;
	strSystemParameter.Param_Ver[0] = 'V';
	strSystemParameter.Param_Ver[1] = '0';
	strSystemParameter.Param_Ver[2] = '.';
	strSystemParameter.Param_Ver[3] = '0';
	strSystemParameter.Param_Ver[4] = '\0';
}





/*
* 返回系统参数  长度
*
*/
u32_t sys_get_params(void *pParmas, sysParams_t param)
{
	u32_t nRetLen = 0;
	u8_t *pTemp = 0;
	bool bMemcpy = FALSE;
	
	switch(param)
	{
		case SYS_PARAM :
			pTemp = (u8_t *)&strSystemParameter;
			nRetLen = sizeof(strSystemParameter);
			bMemcpy = TRUE;
			break;
		
		case SYS_INTF:
			pTemp = (u8_t *)&strSystemParameter.WorkMode;
			nRetLen = sizeof(strSystemParameter.WorkMode);
			*(u8_t *)pParmas = strSystemParameter.WorkMode;
			break;
		
		case SYS_SN :
			pTemp = (u8_t *)strSystemParameter.ProductSn;
			nRetLen = sizeof(strSystemParameter.ProductSn);
			bMemcpy = TRUE;
			break;
		
		case SYS_DHCP:
			pTemp = (u8_t *)&strSystemParameter.DHCPEnable;
			nRetLen = sizeof(strSystemParameter.DHCPEnable);
		  *(u8_t *)pParmas = strSystemParameter.DHCPEnable;
			break;
		
		case SYS_IP :
			pTemp = (u8_t *)strSystemParameter.LocalIp;
			nRetLen = sizeof(strSystemParameter.LocalIp);
			bMemcpy = TRUE;
			break;
		case SYS_GW:
			pTemp = (u8_t *)strSystemParameter.LocalGateway;
			nRetLen = sizeof(strSystemParameter.LocalGateway);
			bMemcpy = TRUE;
			break;
		case SYS_MASK :
			pTemp = (u8_t *)strSystemParameter.LocalSubWay;
			nRetLen = sizeof(strSystemParameter.LocalSubWay);
			bMemcpy = TRUE;
			break;
		case SYS_PORT:
			pTemp = (u8_t *)&strSystemParameter.LocalPort;
			nRetLen = sizeof(strSystemParameter.LocalPort);
		*(u16_t *)pParmas = strSystemParameter.LocalPort;
			break;
		case SYS_RIP :
			pTemp = (u8_t *)strSystemParameter.DistinIp;
			nRetLen = sizeof(strSystemParameter.DistinIp);
			bMemcpy = TRUE;
			break;
		case SYS_RPORT:
			pTemp = (u8_t *)&strSystemParameter.DistinPort;
			nRetLen = sizeof(strSystemParameter.DistinPort);
		*(u16_t *)pParmas = strSystemParameter.DistinPort;
			break;
		case SYS_SSID :
			pTemp = (u8_t *)strSystemParameter.WIFISSID;
			nRetLen = sizeof(strSystemParameter.WIFISSID);
			bMemcpy = TRUE;
			break;
		case SYS_PWD:
			pTemp = (u8_t *)strSystemParameter.WIFIPass;
			nRetLen = sizeof(strSystemParameter.WIFIPass);
		  bMemcpy = TRUE;
			break;
		default:
			break;
	}
	
	//copy memory
	if (bMemcpy)
	{
		memcpy(pTemp, pParmas,nRetLen);
	}
	
	return nRetLen;
}






/*
* 写参数到flash
*/
bool sys_write_param(void)  
{
	bool bReturnValue = FALSE;
	u8_t *pTemp ;
	pTemp = (u8_t *)(&strSystemParameter);

	//ADDR_FLASH_SECTOR_2  0x08008000  setup param   ADDR_FLASH_SECTOR_3 system param 
	bReturnValue = OwnFlashSave(ADDR_FLASH_SECTOR_2, pTemp, sizeof(strSystemParameter)/sizeof(char));
	
	return bReturnValue;
}

/*
**读flash中的系统参数
*/
bool sys_read_param(u8_t *InitialFlag)  
{
	bool bReturnValue = FALSE;
	
	OwnFlashRead( ADDR_FLASH_SECTOR_2, (u8_t *)(&strSystemParameter), sizeof(strSystemParameter)/sizeof(char));
	
	if(strSystemParameter.Param_Ver[0] != 'V')
	{
		strSystemParameter.HraerBeatTimeout = UPLOADHEART_TIME;
		strSystemParameter.SendDataDealyTime = UPLOADDATA_TIME;
		strSystemParameter.SendDataMaxCnt = UPLOADDATA_RECORD_CNT;
		strSystemParameter.Param_Ver[0] = 'V';
		strSystemParameter.Param_Ver[1] = '0';
		strSystemParameter.Param_Ver[2] = '.';
		strSystemParameter.Param_Ver[3] = '0';
		strSystemParameter.Param_Ver[4] = '\0';
	}
		
	if(strSystemParameter.InitialFlag != SYS_INIT_FLAG)  //初始化
	{
		*InitialFlag = strSystemParameter.InitialFlag;
		SetDefaultParameter();
			
		bReturnValue = sys_write_param();
		
		if( bReturnValue == 0)
		{
			//printf("\r\nInit Parameter error!\r\n");
			//return 2;
		}
		else
		{
			//printf("\r\nRead Parameter and initial suc!\r\n");
			//return 3;
		}
	}
	else
	{
		*InitialFlag = SYS_INIT_FLAG;
		//printf("Read Parameter suc\r\n");
		bReturnValue = TRUE;
	}
	
	return bReturnValue;
}


/*
**获取 系统参数
*/
uint8 system_get_param(uint8 ** dat)
{
	uint8 retLen = 0;
	
	retLen = sizeof(strSystemParameter)/sizeof(char);
	//memcpy(dat,(u8_t *)(&strSystemParameter), retLen);
	*dat = (u8_t *)(&strSystemParameter);
	
	return retLen;
}

/*
**设置 系统参数
*/
unsigned char system_set_param(unsigned char * paramBuf, unsigned int len)
{
	uint8 retLen = 0;
	
	return retLen;
}

/*
**初始化 系统参数
*/
bool ParameterInitial(void)
{
	bool bValue = FALSE;
	u8_t initialFlag = 0;

	bValue = sys_read_param(&initialFlag);
	
	if(bValue)
	{
		bValue = TRUE;
	}	
	else
	{
		if(bValue == 1) //读取失败
		{
			return 1;
		}
		else 
		{
			if(initialFlag == 0xFF) //first run
			{
				//printf("first run\r\n");
			}
		}
	}
	
	return bValue;
}

void PrintSendCommand(u8_t bCommand, u8_t *pContent, unsigned short iLength)
{
	DebugUartPort.send_buf[0] = 0xBB;
	DebugUartPort.send_buf[1] = 2+iLength;
	DebugUartPort.send_buf[2] = bCommand;
	memcpy( &DebugUartPort.send_buf[3] , pContent, iLength);

	DebugUartPort.send_buf[3+iLength] = CheckSum( &DebugUartPort.send_buf[2], 1+iLength);
	PrintQuerySend(DebugUartPort.send_buf, 4+iLength);
}

void PrintBlueCommand(uint8 tag,u8_t bCommand, u8_t *pContent, unsigned short iLength)
{
	DebugUartPort.send_buf[0] = 0xAA;
	DebugUartPort.send_buf[1] = tag;
	DebugUartPort.send_buf[2] = 0;
	DebugUartPort.send_buf[3] = 6+iLength;
	DebugUartPort.send_buf[4] = 0x70;
	DebugUartPort.send_buf[5] = 0;
	DebugUartPort.send_buf[6] = 2+iLength;
	DebugUartPort.send_buf[7] = bCommand|0xA0;
	DebugUartPort.send_buf[8] = iLength;
	memcpy( &DebugUartPort.send_buf[9] , pContent, iLength);

	DebugUartPort.send_buf[9+iLength] = CheckSum( &DebugUartPort.send_buf[4], 5+iLength);
	UartBlueSend(DebugUartPort.send_buf, 10+iLength);
	
	PrintQuerySend(DebugUartPort.send_buf, 10+iLength);
}



void PrintNetSendCommand(u8_t bCommand, u8_t *pContent, unsigned short iLength)
{
	uint8 tempBuf[256];
	
	tempBuf[0] = 0xBB;
	tempBuf[1] = 2+iLength;
	tempBuf[2] = bCommand;
	memcpy( &tempBuf[3] , pContent, iLength);

	tempBuf[3+iLength] = CheckSum( &tempBuf[2], 1+iLength);
	TcpServSend(tempBuf, 4+iLength);
	FeedDog();
}

void PrintNet(u8_t * str, int len)
{
	TcpServSend(str, len);
}

//static uint8 bSysLefLen = 0;
#define SYSTEMPARAM_LEN  150


 uint32 s_time = 0;
uint32 g_time_idel = 0;
uint8 g_ble_state = 0; // 0:disconn, 1:conn,2:weixin ok 3 :data 

void sendBlueReset(void)
{

	u8_t arrTemp[8];
	//uint8 id[16] = {0};
	
	
	arrTemp[0] = 0xAA;
	arrTemp[1] = 0xA5;
	arrTemp[2] = 0x00;
	arrTemp[3] = 0x02;
	arrTemp[4] = 0xfe;

	arrTemp[5] = CheckSum(&arrTemp[4],1);
	UartBlueSend(arrTemp, 6);
	
	g_ble_state = 0;
}

void sendBlueDisconn(uint8 delay100ms)
{
 
	u8_t arrTemp[8];
	
	
	arrTemp[0] = 0xAA;
	arrTemp[1] = 0xA5;
	arrTemp[2] = 0x00;
	arrTemp[3] = 0x03;
	arrTemp[4] = 0xfd;
	arrTemp[5] = delay100ms;

	arrTemp[6] = CheckSum(&arrTemp[4],2);
	UartBlueSend(arrTemp, 7);
	
	g_ble_state = 0;
}


int bytesdec2hex(uint8 *dec,uint8 *hex,int len)
{
	unsigned long long tmp=0,base;
	int i;
	
	base = 1;
	for(i=1;i<len;i++)
	{
		base *= 10;
	}
	
	for(i=0;i<len;i++)
	{
		tmp += (dec[i]-'0')*base;
		base /=10;
	}
	
 
	hex[0] = ((tmp)>>32)&0xff;
	hex[1] = (tmp>>24)&0xff;
	hex[2] = (tmp>>16)&0xff;
	hex[3] = (tmp>>8)&0xff;
	hex[4] = (tmp>>0)&0xff;
	
  return i;
}

void sendDevId(void)
{
//	unsigned long long tmp;
	u8_t arrTemp[16];
//	uint8 id[16] = 0;
	
	  arrTemp[0] = 0xAA;
		arrTemp[1] = 0xA0;
		arrTemp[2] = 0x00;
		arrTemp[3] = 0x0a;
		arrTemp[4] = 'C';
		arrTemp[5] = 'o';
		arrTemp[6] = 'A';
		arrTemp[7] = 'p';
		
	bytesdec2hex(&strSystemParameter.ProductSn[2],&arrTemp[8],14);

		arrTemp[13] = CheckSum(&arrTemp[4],9);
		UartBlueSend(arrTemp, 14);
	//	PrintQuerySend(arrTemp, 14);
		DebugUartPort.other_flag = 0;
		return;
}


void DealSysParamBlue(void)
{
	u8_t arrTemp[256];
//	u8_t arrComp1[20]={0xff,0xff,0xff,0xff,0xff};
   	char strDate[64];
//	char strTime[32];
	unsigned int  i=0;
	u8_t ret;
	uint32 dat_len = 0;
	uint8 tag = 0;
	
//	unsigned int  iID = 0;
//	int rssi;
//	FLASH_QUEUE myFlashQueue;
	Struct_System_Parameters structTemp;
	
	tag = DebugUartPort.rcv_buf[0];
  
	//debug_set_output(PrintQuerySend);
	if (tag== 0XA0)
	{
		sendDevId();
		return;
	}
	else if (tag == 0XA5)
	{
		if(DebugUartPort.rcv_buf[3] == 2)
		{
			if (g_ble_state != 1)
			   g_time_idel = SysTick1sCnt;
			
			g_ble_state = 1;
		}
		else if(DebugUartPort.rcv_buf[3] == 1)
		{
			if ((SysTick10msCnt - s_time)<100)
			{
				//sendBlueReset();
				//return;
			}
			else
			{
				
			}
			g_ble_state = 2;
			g_time_idel = SysTick1sCnt;
			s_time++;
			SET_LED2();
		}
		else
		{
 			g_ble_state = 0;
			g_time_idel = 0;
			CLR_LED2();
			if(s_time == 10)
			{
				s_time = 0;
				sendBlueDisconn(100);
				StartBeep(200,1);
			}
		}
	}
	else if (tag== 0XA1)
	{
		g_ble_state = 3;
		g_time_idel = 0;
		SET_LED2();
		switch(DebugUartPort.rcv_buf[6])
		{
			case UART_CMD_GET_PRAM:  //read
				PrintBlueCommand(0xA1,0xA1, (u8_t *)(&strSystemParameter), sizeof(strSystemParameter)/sizeof(char)-28);
				break;

			case UART_CMD_SET_PRAM: //set					
					memcpy( (u8_t *)(&structTemp),&DebugUartPort.rcv_buf[2],sizeof(strSystemParameter)/sizeof(char)-28);
					if( memcmp((u8_t *)(&structTemp), (u8_t *)(&strSystemParameter),sizeof(strSystemParameter)/sizeof(char)-28) != 0)
					{
						memcpy((u8_t *)(&strSystemParameter),&DebugUartPort.rcv_buf[8],sizeof(strSystemParameter)/sizeof(char)-28);
						sys_write_param();
						arrTemp[0] = UART_CMD_SET_PRAM;
						PrintBlueCommand(0xA1,0xA0, arrTemp, 1); 
						g_sysrestart_flag = 1;  
		printf("System restart..\r\n");
						DelayMs(5);
						RestartSys();
						while(1);
					}
					break;
					
			case UART_CMD_GET_LOG_CNT:				//查询日志数量
				i = GetLogCnt();
				printf("Total log cnt: %d.\r\n",i);
				break;
			case UART_CMD_GET_ALL_LOG:				//打印所有日志
				for(i = 0;i < 32768;i++)
				{
					memset(strDate,0,64);
					ret = log_get_one((uint8 *)strDate);
					
					FeedDog();
					if(ret == 0)
						break;
					
					printf(strDate);
				}
				break;


			case UART_CMD_ERASE_ALL_DATA:				//擦除所有数据，包括日志和记录
				//aa 02 05 fb
				printf("Delete all data, waiting.....");
				Flash_ChipErase();
				printf("Del Tag Data OK!");
		printf("System restart..\r\n");
				RestartSys();
				while(1);
				//break;
			case UART_CMD_GET_FLASH_DATA:		//aa 02 06 fa
		/*	
				FlashQueue_Copy(&FlashTempQueue, &TagRecordFlashQueue);
				for(i = 0;i < FlashTempQueue.cnt;i++)
				{
					ret = FlashQueue_GetTagData((FLASH_QUEUE*)&FlashTempQueue,arrTemp);
					if(ret)
						PrintQuerySend(arrTemp,16);
					else
						break;
			
				 FeedDog();
				}
			*/
				for(i = 0;i < FLASH_RECORD_MAX_SIZE;i+= 16)
				{
					Flash_BufferRead(arrTemp, i, 16);
					PrintQuerySend(arrTemp,16);
					FeedDog();
				}
				for(i = 0;i < 0x4000;i += 16)
				{
					Flash_BufferRead(arrTemp, i+FLASH_ADDR_RECORD_CNT, 16);
					PrintQuerySend(arrTemp,16);
					FeedDog();
				}
				
				printf("*****");
				break;
			case UART_CMD_GET_STATE:				// 查询本机时间
				//aa 02 07 f9
			//	printf("AP Time is 20%.2d-%.2d-%.2d %.2d-%.2d-%.2d\r\n",SystemDate.RTC_Year, SystemDate.RTC_Month, SystemDate.RTC_Date ,\
															SystemTime.RTC_Hours, SystemTime.RTC_Minutes, SystemTime.RTC_Seconds);
			
			  arrTemp[0] = g_bServerLoginSucFlag+ '0';
				arrTemp[1] = WifiConnectFlag+ '0';
				arrTemp[2] = W5500_getLink()+ '0';
				PrintBlueCommand(0xA1,UART_CMD_SET_PRAM, arrTemp, 3); 
					
				break;
			case UART_CMD_RESET: //reset
				//aa 02 09 f7
				printf("Restart......\r\n");
				g_sysrestart_flag = 1;    
				break;
			//case 0x0A:
				//aa 02 0A f6
			//	printf("Log Erase.\r\n");
			//	EraseLogMemory();
			//	break;
		//	case 0x0B:
				//AA 02 0B F5
			//	for(i = 0;i < 
			//	PrintOneLog();
				//break;
			case 0x0C:
				//aa 02 0C f4
			//strDate[0] = 01 00 2B A9,21 3E 9E
				i=1000;
				while(i--)
				{
					strDate[0]++;
					strDate[1]--;
					strDate[3]+=3;
					strDate[4]+=5;
					FlashQueue_EnQueue(&TagRecordFlashQueue, (u8_t *)strDate); 
					//FlashQueue_SaveParam(&TagRecordFlashQueue);
					
					if (i%100 == 0)
					{
						FeedDog();	
					}
					if((strDate[1] == 0xFF)&&(strDate[2] == 0xFF)&&(strDate[3] == 0xFF)&&(strDate[4] == 0xFF))
								{
									printf("write error %d\n",i);
								}
				}
				printf("write 1000 tag ok\n");
				//printf(">>RF Tx Channel:%d, Rx Channel:%d\r\n",strSystemParameter.RFTxChannel,strSystemParameter.RFRxChannel);
				break;
			case 0x0D:
				//aa 02 0D   f3
				strSystemParameter.RFTxChannel = DebugUartPort.rcv_buf[2];
				strSystemParameter.RFRxChannel = DebugUartPort.rcv_buf[3];
				printf(">>RF Channel Set Ok\r\n");
				sys_write_param();
				DelayMs(5);
		printf("System restart..\r\n");
				RestartSys();
				while(1);
				//break;
			 
			case UART_CMD_GET_VERSION:
				arrTemp[0]= MYVERSION;
				PrintBlueCommand(0xA1,UART_CMD_GET_VERSION, (u8_t *)arrTemp, 1);
				StartBeep(20,1);
				break;

			case UART_CMD_ENTER_TEST_MODE:
				//AA 03 10 01 EF
				printf("Enter test mode.\r\n");
				TestReq = DebugUartPort.rcv_buf[2];
				break;
			case UART_CMD_EXIT_TEST_MODE:
				// AA 02 11 EF
				printf("Exit test mode.\r\n");
				TestReq = 0;
			
			case UART_CMD_GET_AP:
					g_get_ap_flag = 1;
					dat_len = WIFI_get_ap(arrTemp);
					PrintBlueCommand(0xA1,UART_CMD_GET_AP,arrTemp,dat_len);
			
				break;
			default:
				break;
			}
	}
}

void DealSysParamLan()
{
	u8_t arrTemp[64];
//	u8_t arrComp1[20]={0xff,0xff,0xff,0xff,0xff};
//	char strDate[32];
//	char strTime[32];
	unsigned int  i=0;
	u8_t ret;
//	unsigned int  iID = 0;
//	int rssi;
//	FLASH_QUEUE myFlashQueue;
	uint8 *pBuf = GetTcpServBuf();  //初始化为串口
	
	
	Struct_System_Parameters structTemp;
  //debug_set_output(TcpServSend);
	
	switch(pBuf[1])
	{
		case UART_CMD_GET_PRAM:  //read
			PrintNetSendCommand(0x01, (u8_t *)(&strSystemParameter), sizeof(strSystemParameter)/sizeof(char)-28);
			break;

		case UART_CMD_SET_PRAM: //set			
			memcpy( (u8_t *)(&structTemp),&pBuf[2],sizeof(strSystemParameter)/sizeof(char));
			if( memcmp((u8_t *)(&structTemp), (u8_t *)(&strSystemParameter),sizeof(strSystemParameter)/sizeof(char)-28) == 0)
			{  //not reset
				PrintNetSendCommand(0x02, arrTemp, 1);
				StartBeep(50,1);
			}
			else
			{
				memcpy((u8_t *)(&strSystemParameter),(u8_t *)(&structTemp),sizeof(strSystemParameter)/sizeof(char)-28);
				PrintNetSendCommand(0x02, arrTemp, 1); 
				sys_write_param();
				DelayMs(5);
		printf("System restart..\r\n");
				RestartSys();
				while(1);
			}
			break;
		case UART_CMD_GET_LOG_CNT:				//查询日志数量
			i = GetLogCnt();
			printf("Total log cnt: %d.\r\n",i);
			break;
		case UART_CMD_GET_ALL_LOG:				//打印所有日志
			for(i = 0;i < 32768;i++)
			{
				memset(arrTemp,0,64);
				ret = log_get_one(arrTemp);
				FeedDog();
				if(ret == 0)
					break;
				
				TcpServSend(arrTemp, ret);
			}
			break;


		case UART_CMD_ERASE_ALL_DATA:				//擦除所有数据，包括日志和记录
			//aa 02 05 fb
			printf("Delete all data, waiting.....");
			Flash_ChipErase();
			printf("Del Tag Data OK!");
		printf("System restart..\r\n");
			RestartSys();
			while(1);
			//break;
		case UART_CMD_GET_FLASH_DATA:		//aa 02 06 fa
		/*
			FlashQueue_Copy(&FlashTempQueue, &TagRecordFlashQueue);
			for(i = 0;i < FlashTempQueue.max_cnt;i++)
			{
				ret = FlashQueue_GetTagData(&FlashTempQueue,arrTemp);
				if(ret)
					PrintQuerySend(arrTemp,16);
				else
					break;
			}
		*/
			for(i = 0;i < FLASH_RECORD_MAX_SIZE;i+= 16)
			{
				Flash_BufferRead(arrTemp, i, 16);
				PrintQuerySend(arrTemp,16);
				FeedDog();
			}
			for(i = 0;i < 0x4000;i += 16)
			{
				Flash_BufferRead(arrTemp, i+FLASH_ADDR_RECORD_CNT, 16);
				PrintQuerySend(arrTemp,16);
				FeedDog();
			}
			printf("*****");
			break;
		case UART_CMD_GET_STATE:				// 查询本机时间
				//aa 02 07 f9
			//	printf("AP Time is 20%.2d-%.2d-%.2d %.2d-%.2d-%.2d\r\n",SystemDate.RTC_Year, SystemDate.RTC_Month, SystemDate.RTC_Date ,\
															SystemTime.RTC_Hours, SystemTime.RTC_Minutes, SystemTime.RTC_Seconds);
			
			  arrTemp[0] = g_bServerLoginSucFlag;
				arrTemp[1] = WifiConnectFlag;
				arrTemp[2] = W5500_getLink();
				PrintBlueCommand(0xA1,UART_CMD_SET_PRAM, arrTemp, 3); 
					
				break;
		case UART_CMD_RESET: //reset
			//aa 02 09 f7
			PrintNet("Restart......\r\n", sizeof("Restart......\r\n"));
			g_sysrestart_flag = 1;    
			break;
		case 0x0A:
			//aa 02 0A f6
			printf("Log Erase.\r\n");
			EraseLogMemory();
			break;
		case 0x0B:
			//AA 02 0B F5
		//	for(i = 0;i < 
			PrintOneLog();
			break;
		case 0x0C:
			//aa 02 0C f4
			printf(">>RF Tx Channel:%d, Rx Channel:%d\r\n",strSystemParameter.RFTxChannel,strSystemParameter.RFRxChannel);
			break;
		case 0x0D:
			//aa 02 0D   f3
			strSystemParameter.RFTxChannel = pBuf[2];
			strSystemParameter.RFRxChannel = pBuf[3];
			printf(">>RF Channel Set Ok\r\n");
			sys_write_param();
			DelayMs(5);
		printf("System restart..\r\n");
			RestartSys();
			while(1);
//			break;
		case UART_CMD_GET_VERSION:
			arrTemp[0]= MYVERSION;
			PrintNetSendCommand(UART_CMD_GET_VERSION, (u8_t *)arrTemp, 1);
			break;
		
		case TEST_CMD_DOWNLOAD_START:
				
				if(TcpDownloadStart(pBuf))
				{
					arrTemp[0]= 0;
				}
				else
				{
					arrTemp[0]= 1;
				}
				PrintNetSendCommand(TEST_CMD_DOWNLOAD_START, (u8_t *)arrTemp, 1);
			break;

		case TEST_CMD_DOWNLOAD_ING:
				
				if(TcpDownloading(&pBuf[2]))
				{
					arrTemp[0]= 0;
				}
				else
				{
					arrTemp[0]= 1;
				}
				PrintNetSendCommand(TEST_CMD_DOWNLOAD_ING, (u8_t *)arrTemp, 1);
			break;

		case TEST_CMD_DOWNLOAD_END:
				
				if(TcpDownloadEnd(pBuf))
				{
					arrTemp[0]= 0;
					PrintNetSendCommand(TEST_CMD_DOWNLOAD_END, (u8_t *)arrTemp, 1);
					sys_BootToUpgrade();
				}
				else
				{
					arrTemp[0]= 1;
					PrintNetSendCommand(TEST_CMD_DOWNLOAD_END, (u8_t *)arrTemp, 1);
				}
				
			break;				
		case UART_CMD_ENTER_TEST_MODE:
			//AA 03 10 01 EF
			PrintNet("Enter test mode.\r\n",sizeof("Enter test mode.\r\n"));
			TestReq = pBuf[2];
			break;
		case UART_CMD_EXIT_TEST_MODE:
			// AA 02 11 EF
			printf("Exit test mode.\r\n");
			TestReq = 0;
			break;
		default:
			break;
		}
}

/*
**设置 系统参数
*/
bool  system_set_pram(uint8 * dat)
{
	bool ret = FALSE;
	Struct_System_Parameters  *param;
	Struct_System_Parameters structTemp;
	
	param = (Struct_System_Parameters  *)dat;
	
	memcpy( (u8_t *)(&structTemp),param,sizeof(strSystemParameter)/sizeof(char));
	if( memcmp((u8_t *)(&structTemp), (u8_t *)(&strSystemParameter),sizeof(strSystemParameter)/sizeof(char)) == 0)
	{  //not reset
		ret = FALSE;
	}
	else
	{
		memcpy((u8_t *)(&strSystemParameter),(u8_t *)(&structTemp),sizeof(strSystemParameter)/sizeof(char));
		ret = TRUE;
	}
	
	return ret;
}





void DealSysParameter(void)
{
	u8_t arrTemp[256];
	char strDate[64];
	unsigned int  i=0;
	u8_t ret;
	uint32 dat_len = 0;
	Struct_System_Parameters structTemp;
	
	
	switch(DebugUartPort.rcv_buf[1])
	{
		case UART_CMD_GET_PRAM:  //read
			PrintSendCommand(0x01, (u8_t *)(&strSystemParameter), sizeof(strSystemParameter)/sizeof(char)-28);
			break;

		case UART_CMD_SET_PRAM: //set			
			memcpy( (u8_t *)(&structTemp),&DebugUartPort.rcv_buf[2],sizeof(strSystemParameter)/sizeof(char)-28);
			if( memcmp((u8_t *)(&structTemp), (u8_t *)(&strSystemParameter),sizeof(strSystemParameter)/sizeof(char)-28) == 0)
			{  //not reset
				PrintSendCommand(0x02, arrTemp, 1);
				StartBeep(50,1);
			}
			else
			{
				memcpy((u8_t *)(&strSystemParameter),(u8_t *)(&structTemp),sizeof(strSystemParameter)/sizeof(char)-28);
				PrintSendCommand(0x02, arrTemp, 1); 
				sys_write_param();
				DelayMs(5);
		printf("System restart..\r\n");
				RestartSys();
				while(1);
			}
			break;
		case UART_CMD_GET_LOG_CNT:				//查询日志数量
			i = GetLogCnt();
			printf("Total log cnt: %d.\r\n",i);
			break;
		case UART_CMD_GET_ALL_LOG:				//打印所有日志
			for(i = 0;i < 32768;i++)
			{
				memset(strDate,0,64);
				ret = log_get_one((u8_t *)strDate);
				
				FeedDog();
				if(ret == 0)
					break;
				
				printf(strDate);
			}
			break;


		case UART_CMD_ERASE_ALL_DATA:				//擦除所有数据，包括日志和记录
			//aa 02 05 fb
			printf("Delete all data, waiting.....");
			Flash_ChipErase();
			printf("Del Tag Data OK!");
	
		printf("System restart..\r\n");
			RestartSys();
			while(1);
			//break;
		case UART_CMD_GET_FLASH_DATA:		//aa 02 06 fa
	/*	
			FlashQueue_Copy(&FlashTempQueue, &TagRecordFlashQueue);
			for(i = 0;i < FlashTempQueue.cnt;i++)
			{
				ret = FlashQueue_GetTagData((FLASH_QUEUE*)&FlashTempQueue,arrTemp);
				if(ret)
					PrintQuerySend(arrTemp,16);
				else
					break;
		
		   FeedDog();
			}
		*/
			for(i = 0;i < FLASH_RECORD_MAX_SIZE;i+= 16)
			{
				Flash_BufferRead(arrTemp, i, 16);
				PrintQuerySend(arrTemp,16);
				FeedDog();
			}
			for(i = 0;i < 0x4000;i += 16)
			{
				Flash_BufferRead(arrTemp, i+FLASH_ADDR_RECORD_CNT, 16);
				PrintQuerySend(arrTemp,16);
				FeedDog();
			}
			
			printf("*****");
			break;
		case UART_CMD_GET_STATE:				// 查询本机时间
				//aa 02 07 f9
			//	printf("AP Time is 20%.2d-%.2d-%.2d %.2d-%.2d-%.2d\r\n",SystemDate.RTC_Year, SystemDate.RTC_Month, SystemDate.RTC_Date ,\
															SystemTime.RTC_Hours, SystemTime.RTC_Minutes, SystemTime.RTC_Seconds);
			
			  arrTemp[0] = g_bServerLoginSucFlag + '0';
				arrTemp[1] = WifiConnectFlag+ '0';
				arrTemp[2] = W5500_getLink()+ '0';
				PrintQuerySend(arrTemp, 3); 
					
				break;
		case UART_CMD_RESET: //reset
			//aa 02 09 f7
			printf("Restart......\r\n");
			g_sysrestart_flag = 1;
			RestartSys();
			break;
		//case 0x0A:
			//aa 02 0A f6
		//	printf("Log Erase.\r\n");
		//	EraseLogMemory();
		//	break;
		//case 0x0B:
			//AA 02 0B F5
		//	for(i = 0;i < 
		//	PrintOneLog();
	//		break;
		case 0x0C:
			//aa 02 0C f4
		//strDate[0] = 01 00 2B A9,21 3E 9E
		  i=1000;
		  while(i--)
			{
				strDate[0]++;
				strDate[1]--;
				strDate[3]+=3;
				strDate[4]+=5;
				strDate[15]  = CheckSum((u8_t *)strDate,15);
				FlashQueue_EnQueue(&TagRecordFlashQueue, (u8_t *)strDate); 
				//FlashQueue_SaveParam(&TagRecordFlashQueue);
				
				if (i%100 == 0)
				{
					FeedDog();	
				}
				if((strDate[1] == 0xFF)&&(strDate[2] == 0xFF)&&(strDate[3] == 0xFF)&&(strDate[4] == 0xFF))
							{
								printf("write error %d\n",i);
							}
			}
			printf("write 1000 tag ok\n");
			//printf(">>RF Tx Channel:%d, Rx Channel:%d\r\n",strSystemParameter.RFTxChannel,strSystemParameter.RFRxChannel);
			break;
		case 0x0D:
			//aa 02 0D   f3
			strSystemParameter.RFTxChannel = DebugUartPort.rcv_buf[2];
			strSystemParameter.RFRxChannel = DebugUartPort.rcv_buf[3];
			printf(">>RF Channel Set Ok\r\n");
			sys_write_param();
			DelayMs(5);
	
		printf("System restart..\r\n");
			RestartSys();
			while(1);
			//break;
     
	  case UART_CMD_GET_VERSION:
			arrTemp[0]= MYVERSION;
			PrintSendCommand(UART_CMD_GET_VERSION, (u8_t *)arrTemp, 1);
		  
	/*	  arrTemp[0] = 0xA5;
		  arrTemp[1] = 0x0;
		  arrTemp[2] = 2;
		  arrTemp[3] = 0xfe;
		  arrTemp[4] = 0x13;
		
		  UartBlueSend(arrTemp, 5); */
			break;

		case UART_CMD_ENTER_TEST_MODE:
			//AA 03 10 01 EF
			printf("Enter test mode.\r\n");
			TestReq = DebugUartPort.rcv_buf[2];
			break;
		case UART_CMD_EXIT_TEST_MODE:
			// AA 02 11 EF
			printf("Exit test mode.\r\n");
			TestReq = 0;
		
		case UART_CMD_GET_AP:
				g_get_ap_flag = 1;
				dat_len = WIFI_get_ap(arrTemp);
		    PrintSendCommand(UART_CMD_GET_AP|0xA0, (u8_t *)arrTemp, dat_len);
			break;
		default:
			break;
		}
}

#include "si446x_bsp.h"

//系统重启    hcp 2015-10-29  关闭433 检查内存队列， 最后关闭
void sys_restart(void)
{
	//return;
	radio_hal_AssertShutdown();
	
	if (BufQueueIsEmpty(&TagDataQueue))
	{
		AddLog("System restart..\r\n");
		
		DelayMs(10);
	
		printf("System restart..\r\n");
		RestartSys();
	}
	
}


void sys_BootToUpgrade(void)
{
	WriteApplicationFlag(0x55);
	g_sysrestart_flag = 1;
}

