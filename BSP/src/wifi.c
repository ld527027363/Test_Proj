#include "global.h"
#include "wifi.h"
#include "stm32f4xx_it.h"
#include "upgrade.h"
#include "gpio_conf.h"
#include "Uart.h"
#include "Uart1.h"
#include "linklayer.h"
#include "timer.h"
#include "SystemParameter.h"

//static unsigned char g_bAFlag = 0;
static volatile unsigned char g_bOKFlag = 0;
static volatile unsigned char SetupWifiFlag = 0;
volatile unsigned char WIFITimeTic = 0;
volatile unsigned char WIFIUpgradeStartFlag = 0;

volatile unsigned char WifiConnectFlag = 0;

uint8 g_get_ap_flag = 0;
uint8 g_get_ap_ok_flag = 0;

//等待接收超时
BOOL wait_flag_10ms(volatile uint8 *flag, int delay)
{
	int i;

	for (i=0; i<delay;i++)
	{
		if (*flag) 
		{
			DelayMs(20);
			break;
		}
		
		DelayMs(10);
	}
	
	return *flag;
}



void WIFI_IRQHandler(void)
{
	unsigned char rcv_byte; 

//	if (USART_GetITStatus(WIFI_UART, USART_IT_RXNE) != RESET) //改为下面简化版,节省时间gaojun 2015-07-21
//	{
	  
	if(WIFI_UART->SR & (1<<5))
	{
		 // 
		//rcv_byte = USART_ReceiveData(WIFI_UART); //改为下面简化版,节省时间
		rcv_byte = (WIFI_UART->DR&0xFF);  		
//		uart1_send(&rcv_byte,1);	//debug !!!!!!!!!!!!!!1
		switch(WifiUartPort.rcv_state)
		{
			case 0:
				if((rcv_byte == '+') && SetupWifiFlag)
				{
					WifiUartPort.rcv_state = 11;
				}
				else if((rcv_byte == 'a') && SetupWifiFlag)
				{
					//g_bAFlag = 1;
				}
				else if((g_bWifiUpgradeFlag == 1)&& ((rcv_byte == 'H') || (rcv_byte == 0x5b)))
				{
				//	DEBUG_PRINTF("1/r/n");
					WifiUartPort.rcv_index =0;
					WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
					WifiUartPort.rcv_state = 21; 
					
					WIFIUpgradeStartFlag = 1;  
					WIFITimeTic = 0;                
				}
				else if( (!SetupWifiFlag) && (rcv_byte == 'H'))
				{
					WifiUartPort.rcv_index =0;
					WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
					WifiUartPort.rcv_state = 1;
				}

				break;
			case 0x01: //
				{
					if(rcv_byte != '}')
						WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
					else
					{
						WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
						WifiUartPort.rcv_all_flag=1; //????
					}                        
					if(WifiUartPort.rcv_index >= (TCP_MAX_RCV_LEN-1))	
					{
						WifiUartPort.rcv_index=0;
						WifiUartPort.rcv_all_flag=0; //????
						WifiUartPort.rcv_state=0x00;						
					}						
				}			  
				break;  
	 
			case 11:
				if(rcv_byte == 'o')
				{
					WifiUartPort.rcv_state = 12;
				}
				else
					WifiUartPort.rcv_state = 0;
				break;
			case 12:
				if(rcv_byte == 'k')
				{
					WifiUartPort.rcv_state = 13;
					WifiUartPort.rcv_index= 0;
				}
				else
					WifiUartPort.rcv_state = 0;
				break;	
			case 13:
				if (g_get_ap_flag)
				{
						//g_get_ap_flag = 0;
						
					WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
					
				//	if (WifiUartPort.rcv_index >= 4 && rcv_byte == 0x0a)
					{
				//		if (WifiUartPort.rcv_buf[WifiUartPort.rcv_index - 2] == 0x0d
				//			&&WifiUartPort.rcv_buf[WifiUartPort.rcv_index - 3] == 0x0d
				//			&&WifiUartPort.rcv_buf[WifiUartPort.rcv_index - 4] == 0x0a)
						{
							g_get_ap_ok_flag = 1;
						}
					}
					
				}
				else if((rcv_byte != '\r')&&(rcv_byte != 0))
				{
					
				}
				else if(rcv_byte == '\r')
				{
					g_bOKFlag =1;
					WifiUartPort.rcv_state = 0;						
				}
				else
				{
					WifiUartPort.rcv_state = 0;
				}
				break;
			case 21:
				WifiUartPort.rcv_buf[WifiUartPort.rcv_index++]=rcv_byte;
				
				WIFITimeTic = 0;
				break;

			default:
				WifiUartPort.rcv_state=0x00;
				WifiUartPort.rcv_all_flag =0;				
				break;
		}
		
	   USART_ClearITPendingBit(WIFI_UART,USART_IT_RXNE);
  }
}

/*
**分析字符串参数，返回下标和参数长度
*/
int util_div_params(char * inputStr, int start_idx, char sepChar, int end_index, char endChar,int *params_start, int *params_len)
{
	int i,j,k;
	int dat_len;
	int end_flag;
	
	dat_len = strlen(inputStr);
	
	params_start[0] = start_idx;
	end_flag = 0;
	j=0;
	k=0;
	for(i=start_idx; i<dat_len; i++,k++)
	{
		 if (inputStr[i] == sepChar)  //中间分割
		{
			params_len[j] = k;
			params_start[j+1] = i+1;	
			k = -1;
			j++;
		}
		else if (inputStr[i] == endChar) //解析一行
		{
			params_len[j] = k;
			i += end_index;
			j++;
			end_flag = 1;
		}
		else if(end_flag)
		{
			k = -1;
			j++;
		}
	}
	
	return j;
}


void WIFI_Reset(void)
{
	DelayMs(50);
	GPIO_ResetBits(WIFI_NRST_PORT, WIFI_NRST);
	DelayMs(50);
	GPIO_SetBits(WIFI_NRST_PORT, WIFI_NRST); 	
//	DelayMs(100);
//	WIFI_ready();
//	DelayMs(100);
}


void WIFI_quTrans(void) // 进入透传模式
{
	//WifiQuerySend("+++",3);
	USART_SendData(WIFI_UART,'+');
	DelayMs(50);
	USART_SendData(WIFI_UART,'+');
	DelayMs(50);
	USART_SendData(WIFI_UART,'+');
	DelayMs(100);
	FeedDog();
	
		//WifiQuerySend("a",1);
	USART_SendData(WIFI_UART,'a');
	DelayMs(60);FeedDog();

	WifiUartPort.rcv_state = 0;
}

void WIFI_enTrans(void) //退出透传模式
{
	WIFI_SendStr("AT+Z\r\n");
}

//获取指定 SSID 的加密方式
void WIFI_getCypt(char * ssid, uint8 * cypt)
{	
	int param_start[10] = {0};
	int param_len[10] = {0};
	uint8 ret = 0;
	char cmd[10] = {0};
	uint8 back_SetupWifiFlag = SetupWifiFlag;
	uint8 retry_cnt = 10;

	while(retry_cnt--)
	{
		SetupWifiFlag = 1;
		g_get_ap_ok_flag = 0;
		g_get_ap_flag = 1;
		DelayMs(100);
		WIFI_SendStr("AT+WSCAN=");
		WIFI_SendStr(ssid);
		WIFI_SendStr("\r\n");

		FeedDog();
		if(wait_flag_10ms(&g_get_ap_ok_flag, 400))
		{
			if(memcmp(WifiUartPort.rcv_buf,"=noAP",5))
			{
				ret = util_div_params((char *)WifiUartPort.rcv_buf, 37,',',1,'\r',param_start,param_len);
//				printf("\r\n param_len[3] = %d \r\n",param_len[3]);
				if ((ret)&&(param_len[3]))
				{
					cmd[0] = '1';
					memcpy(&cmd[1],&(WifiUartPort.rcv_buf[param_start[3]]), param_len[3]);
					if(param_len[3] == 0)
					{
						ret = ret;
					}
					if(strstr(cmd,"OPEN") != NULL)
					{
						strcpy((char *)cypt, "OPEN,NONE,");
						break;
					}
					else if(strstr(cmd,"WPA2PSK") != NULL)
					{
						if (strstr(cmd,"TKIP") != NULL)
						{
							strcpy((char *)cypt, "WPA2PSK,TKIP,");
							break;
						}
						else if (strstr(cmd,"WEP") != NULL)
						{
							strcpy((char *)cypt, "WPA2PSK,WEP,");
							break;
						}
						else 
						{
							strcpy((char *)cypt, "WPA2PSK,AES,");
							break;
						}
					}
					else if(strstr(cmd,"WPAPSK") != NULL)
					{
						if (strstr(cmd,"TKIP") != NULL)
						{
							strcpy((char *)cypt, "WPAPSK,TKIP,");
							break;
						}
						else if (strstr(cmd,"WEP") != NULL)
						{
							strcpy((char *)cypt, "WPAPSK,WEP,");
							break;
						}
						else 
						{
							strcpy((char *)cypt, "WPAPSK,AES,");
							break;
						}
					}
					else
					{
						DelayMs(10);
					}
				}
				else
				{
				}
				//printf("\r\n %d _ %d cypt: %s  ok!!",param_start[3],param_len[3],cmd);
			}
		}
		DelayMs(400);
	}
	g_get_ap_flag = 0;
	
	g_get_ap_ok_flag = 0;
//	uart1_send(&retry_cnt,1);
	SetupWifiFlag = back_SetupWifiFlag;
}



void WIFI_ready(void)
{
	uint8_t retry_cnt = 3;
	
	SetupWifiFlag = 1;
	while(retry_cnt--)
	{
		GPIO_SetBits(WIFI_REBLOAD_PORT,WIFI_REBLOAD); 
		GPIO_ResetBits(WIFI_NRST_PORT, WIFI_NRST);
		DelayMs(50);
		GPIO_SetBits(WIFI_NRST_PORT, WIFI_NRST); 

		DelayMs(600);FeedDog();  DelayMs(600);FeedDog();
		DelayMs(600);FeedDog();DelayMs(600);FeedDog();
		DelayMs(600);FeedDog();DelayMs(600);FeedDog();
		
		WIFI_quTrans();
		
		g_bOKFlag = 0;
		WIFI_SendStr("AT+WMODE=STA\r\n");
		
		FeedDog();
		if (!wait_flag_10ms(&g_bOKFlag, 100))
		{
			printf("WIFI error!4\r\n");
		}
		else
		{
			break;
		}
	}
	SetupWifiFlag = 0;
}


/*
**格式化AP 列表数据
*/
uint8 WIFI_set_dat(uint8 * apDat, uint8 *apBuf)
{
	uint8 head_len = 37;
	int dat_len = 0;
	int i = 0,j=0,k=0;
	int dot_cnt = 0;
	int nPower = 0;
	uint8 powerBuf[3] = {0};
	uint8 powerBufCnt = 0;
	uint8 wifi_list[32][32] = {0};
	uint8 wifi_len[32] = {0};
	uint8 wifi_pow[32] = {0};
	uint8 wifi_cnt = 0;
  uint8 wifi_one_index = 0;
	uint8 wifi_dui[16][2] ={0};
	uint8 wif_min = 0;

	uint8 dot_start_flag = 0,pw_end_flag = 0;
	
	dat_len = WifiUartPort.rcv_index - head_len;
	apDat = &apDat[head_len];


	for(i=0;i<dat_len;i++)
	{
		 if (apDat[i] == ',')
		{
			dot_cnt++;
			if ((dot_cnt%4) == 2)    //ssid end
			{
				apBuf[j++] = ':';
				apBuf[j++] = ':';
				
			}
		}
		else if((dot_cnt%4) == 0 && dot_cnt>0 )
		{
			//if (apDat[i] == 0x0a) { pw_end_flag = 1;}
			if (pw_end_flag == 0)
			{
				if (apDat[i] == 0x0a || apDat[i] == 0x0d)
				{
					pw_end_flag = 1;
					
					wifi_len[wifi_cnt] = wifi_one_index;
				
				//	printf("\r\n %s \r\n",powerBuf);
					nPower = atoi((char *)powerBuf);
					wifi_pow[wifi_cnt] = nPower;
					
					wifi_one_index = 0;
					if (wifi_cnt < 30)wifi_cnt++;
					
					nPower = nPower/26 + (((nPower%26)!=0)? 1 : 0);
					
					
					//把信号强度 化成等级
					apBuf[j++] = nPower + '0';
					
					memset(powerBuf,0,3);
					powerBufCnt = 0;
					//sprintf(&apBuf[j++],"%d", nPower);
					
				}
				else
				{
					//apBuf[j] = apDat[i];
					//j++;
					if (powerBufCnt < 3)
					{
						powerBuf[powerBufCnt++] = apDat[i];
					}
					
					dot_start_flag = 1;
				}
			}
		}
		else if ((dot_cnt%4) == 1) //ssid start
		{
			if (dot_cnt/4 && dot_start_flag)
			{
				dot_start_flag = 0;
				pw_end_flag = 0;
				apBuf[j++] = '#';
				apBuf[j++] = '#';
			}
			
			wifi_list[wifi_cnt][wifi_one_index++] = apDat[i];
      apBuf[j] = apDat[i];
			j++;
		}
	}
	
	  //挑选最大 那几个
		memset(apBuf,0,200);
		#define wifi_max   5
		for(i=0;i<wifi_cnt;i++)
		{
			for(j=0;j<wifi_max;j++)
			{
				//获取更大的
				if (wifi_pow[i] > wifi_dui[j][0])
				{
					wif_min = 0;
					for (k=0;k<wifi_max;k++)
					{
						//查找最小值
						if (wifi_dui[wif_min][0] > wifi_dui[k][0])
						{
							wif_min = k;
						}
					}
					
					wifi_dui[wif_min][0] = wifi_pow[i];
					wifi_dui[wif_min][1] = i;
					break;
					
				}
			}
		}
		
		//重新拼凑
		for (i=0,j=0;i<wifi_max && i <wifi_cnt;i++)
		{
			k = wifi_dui[i][1];
			memcpy(&apBuf[j],wifi_list[k],wifi_len[k]);
			j += wifi_len[k];
			
			apBuf[j++] = ':';
			apBuf[j++] = ':';
			
			nPower = wifi_pow[k];
			nPower = nPower/26 + (((nPower%26)!=0)? 1 : 0);					
			//把信号强度 化成等级
			apBuf[j++] = nPower + '0';
			
			if (i!= wifi_max-1)
			{
				apBuf[j++] = '#';
				apBuf[j++] = '#';
			}
			
		}
		
	
	return j;
}

/*
**获取AP列表
*/
uint32 WIFI_get_ap(uint8 *apBuf)
{
	uint32 ret = 0;
//	unsigned char chTemp[64];
//	unsigned char bTemp =0;
	
	SetupWifiFlag = 1;
	
	if(GET_WIFI_READY() != 0)
	{
		return 0;
	}
	
	/* 先退出透传模式 才能发指令*/
	if (sys_check_linktype(COMM_PORT_WIFI))
	{
		WIFI_quTrans();
  }
	
	
	g_get_ap_ok_flag = 0;
	WIFI_SendStr("AT+WSCAN\r\n");
	FeedDog();
	if (wait_flag_10ms((u8_t *)&ret, 500))
	{
		
	}
	ret = WIFI_set_dat(WifiUartPort.rcv_buf, apBuf);	
	//DelayMs(3000);FeedDog();  //DelayMs(600);FeedDog();  
	 
 // if (g_get_ap_ok_flag)
	//{
	//	ret = WIFI_set_dat(WifiUartPort.rcv_buf, apBuf);
		//PrintQuerySend(WifiUartPort.rcv_buf, WifiUartPort.rcv_index); 	
		//PrintQuerySend(apBuf, WIFI_set_dat(WifiUartPort.rcv_buf, apBuf)); 
//	}
	
	g_get_ap_flag = 0;
	WifiUartPort.rcv_state = 0;
	
	SetupWifiFlag = 0;
	if (sys_check_linktype(COMM_PORT_WIFI))
	{
		WIFI_enTrans();
	}
	
	return ret;
}

unsigned char WIFI_Init(void)
{
	unsigned char chTemp[64];
	char cypt[16] = {0};
	
	SetupWifiFlag = 1; 
   
  if(GET_WIFI_READY() != 0)
	{
		return 0;
	}

	g_bOKFlag = 0;
	WIFI_SendStr((char *)"AT+TCPTO=0\r\n");
	FeedDog();
	if (!wait_flag_10ms(&g_bOKFlag, 150))
	{
		SetupWifiFlag = 0;
		printf("WIFI error!5\r\n");
		return 0;
	}
	printf("WIFI set tcp ok !\r\n");
	g_bOKFlag = 0;
	memset(chTemp,0,sizeof(chTemp));
	sprintf((char *)chTemp,"AT+NETP=TCP,CLIENT,%d,%d.%d.%d.%d\r\n", \
									strSystemParameter.DistinPort,\
									strSystemParameter.DistinIp[0],\
									strSystemParameter.DistinIp[1],\
									strSystemParameter.DistinIp[2],\
									strSystemParameter.DistinIp[3]);
	WIFI_SendStr((char *)chTemp);
	if (!wait_flag_10ms(&g_bOKFlag, 200))
	{
		SetupWifiFlag = 0;
		printf("WIFI error!6\r\n");
		return 0;
	}
	
	printf("WIFI set client ok !\r\n");
	WIFI_getCypt((char *)strSystemParameter.WIFISSID,(u8_t *)cypt);

	memset(chTemp,0,64);
	strcpy((char *)chTemp, "AT+WSKEY="); 
	strcat((char *)chTemp, cypt);
	strcat((char *)chTemp, (char *)strSystemParameter.WIFIPass);  
	strcat((char *)chTemp, "\r\n");
	g_bOKFlag = 0;
//	uart1_send(chTemp,64);
	WIFI_SendStr((char *)chTemp);
	if (!wait_flag_10ms(&g_bOKFlag, 200))
	{
		SetupWifiFlag = 0;
		printf("WIFI error!22\r\n");
		return 0;
	}	
	printf("WIFI set wskey ok !\r\n");
	g_bOKFlag = 0;
	DelayMs(200);
	memset(chTemp,0,sizeof(chTemp));
	strcpy((char *)chTemp,"AT+WSSSID=");
	strcat((char *)chTemp, (char *)strSystemParameter.WIFISSID);
	strcat((char *)chTemp, "\r\n");
	//printf(chTemp);
	
	WifiUartPort.rcv_state = 0; 
	WIFI_SendStr((char *)chTemp);
	if (!wait_flag_10ms(&g_bOKFlag, 400))
	{
		SetupWifiFlag = 0;
		printf("WIFI error!7\r\n");
		return 0;
	}

	printf("WIFI set wsssid ok !\r\n");
	WIFI_enTrans();
	SetupWifiFlag = 0;
	printf("WIFI Module is OK\r\n");
	
	return 1;
}

void WIFI_SendProcess(void)
{
	static unsigned short len;
	static unsigned int time_cnt;
	switch(WifiUartPort.send_state)
	{
		case 0:					//配置DMA开始发送
			len = (WifiUartPort.send_len - WifiUartPort.send_index);
			if(len > 1000)
				len = 1000;
		//	USART_DMACmd(WIFI_UART,USART_DMAReq_Tx,ENABLE);   
			WIFI_DMA_Stream->NDTR = len;			//设定DMA长度
			WIFI_DMA_Stream->M0AR = (uint32_t)(WifiUartPort.send_buf)+WifiUartPort.send_index;
			DMA_Cmd(WIFI_DMA_Stream,ENABLE);
			WifiUartPort.send_state = 1;
			break;
		case 1:					//等待发送完成
			if(WIFI_DMA_SendOK)
			{
				WIFI_DMA_SendOK = 0;
				WifiUartPort.send_index += len;
				if(WifiUartPort.send_index >= WifiUartPort.send_len)
				{
					WifiUartPort.send_state = 0;
					WifiUartPort.send_flag = 0;
				}
				else
				{
					WifiUartPort.send_state = 2;
					time_cnt = SysTick10msCnt;
				}
			}
			break;
		case 2:					//延时50ms
			if(( SysTick10msCnt - time_cnt) > 5)
			{
				WifiUartPort.send_state = 0;
			}
			break;
		default:
			break;
	}
}

void WIFI_SendStart(unsigned char *pBuf, unsigned short len)
{
	WifiUartPort.send_state = 0;
	WifiUartPort.send_len = len;
	WifiUartPort.send_buf = pBuf;
	WifiUartPort.send_flag = 1;
	WifiUartPort.send_index = 0;
	WifiUartPort.rcv_all_flag = 0;
	WifiUartPort.rcv_index = 0;
	WifiUartPort.rcv_state = 0;
//	printf(" Wifi Send Start, Len = %d.\r\n",len);

}

void WIFI_CommTask(void)		//本任务每10ms执行一次
{
	static unsigned short WifiLastRcvLen;
	if(GET_WIFI_LINK_STATUS() == 0)
	{
		SET_LED3();
		WifiConnectFlag = 1;
	}
	else
	{
		CLR_LED3();
		WifiConnectFlag = 0;
		return;
	}
	if(WifiUartPort.send_flag)
	{
		WIFI_SendProcess();
		WifiLastRcvLen = 0;
	}
	else if((g_stServerComCtrl.tx_rx_state == 1)&&(strSystemParameter.WorkMode == 0x02))	//WIFI 接收
	{
		if((WifiUartPort.rcv_all_flag == 1)&&(WifiUartPort.rcv_index > WifiLastRcvLen))
		{			
			g_iRevStartPos = WifiUartPort.rcv_index;					

			if( memcmp(g_arrHttp500Error, g_arrRecvBuffer, 12) == 0)
			{
				printf("HTTP/1.1 500 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			else if( memcmp(g_arrHttp400Error, g_arrRecvBuffer, 12) == 0)
			{
				printf("HTTP/1.1 400 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			else if( memcmp(g_arrHttp20, g_arrRecvBuffer, 11) == 0)
			{
				g_bCommReceiveFlag = 1;
						//printf(g_arrRecvBuffer);
			}
			else
			{
				printf("HTTP/1.1 200 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			WifiLastRcvLen = WifiUartPort.rcv_index;
		}
	}

}
