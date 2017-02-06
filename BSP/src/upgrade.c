#include "global.h"
#include "upgrade.h"
#include "gpio_conf.h"
#include "linklayer.h"
#include "W5500.h"
#include "wifi.h"
#include "rs485.h"
#include "uart.h"
#include "SystemParameter.h"
#include "debug.h"


UpgradeStruct UpgradeList;

static unsigned short g_iCrcReadValue =0;
static unsigned short g_iCrcCalValue = 0;

static unsigned int iStartUpgrade =0;
static unsigned int iTotalUpgradePack = 100; 

const char *UpgradeHeadData ={"GET %s HTTP/1.1\r\n"\
"Host: %s:%d\r\n" \
"Connection: keep-alive\r\n"\
"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n"\
"User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1; SV1)\r\n"\
"Accept-Encoding: gzip,deflate,sdch\r\n"\
"Accept-Language: zh-CN,zh;q=0.8\r\n"\
"RANGE:bytes=%d-%d\r\n\r\n"};


unsigned int g_iTotalBinLength =0;
unsigned int g_iTotalLength =0;
unsigned int g_iStart = 0, g_iEndpos=0;
unsigned int g_iGetRealLength = 0;
volatile unsigned char g_bWifiUpgradeFlag = 0;

unsigned short g_iUpgradePackNum = 1024;

//uint8 buf_tmp[1024] = {0};
void SendUpgradeData(unsigned int iNow)
{
	unsigned char  temp_buf[5];
	unsigned short lenth=0;
//	unsigned char  *tx_ptr = NULL;

	unsigned char  *p_buf = NULL;
	unsigned char  ip[16] = {0};
	unsigned int   port = 0;
	unsigned int  iEnd = 0;
	uint8 * pip = 0;

	sys_get_params(&port, SYS_RPORT);
	//sys_get_params(ip, SYS_RIP);
	
	pip = strSystemParameter.DistinIp;

	sprintf((char *)ip, "%d.%d.%d.%d",pip[0],pip[1],\
							  pip[2],pip[3]);
	FeedDog();
	if(g_iStart == 0)
	{
		g_iGetRealLength = g_iUpgradePackNum-1;
	} 
	else if( (g_iTotalBinLength - g_iStart) < g_iUpgradePackNum)
	{
		g_iGetRealLength = g_iTotalBinLength - g_iStart-1;
	}
	else g_iGetRealLength = g_iUpgradePackNum-1;

	iEnd = iNow +g_iGetRealLength;
//	p_buf = (unsigned char *)malloc(strlen(UpgradeHeadData) + 100);
//   
//	if (p_buf == NULL)  
//	{
//		printf("Request memory failed \r\n");
//		return;
//	}
  
	p_buf = &g_arrSendBuffer[3];
	if (rs485_get_pro() == RS485_PRO_P2)
	{
		p_buf = &g_arrSendBuffer[5];
	}
	
	memset(p_buf,0,strlen(UpgradeHeadData)  + 100);

	sprintf((char *)p_buf,UpgradeHeadData,(char *)UpgradeList.arrUrl,ip,port,iNow,iEnd);
  
	
	lenth=strlen((char *)p_buf);  //2015-4-16	
	FeedDog();	
	if(strSystemParameter.WorkMode == COMM_PORT_RS485)
	{

		if (rs485_get_pro() == RS485_PRO_P1)
		{
			temp_buf[0]=0x02;
			lenth = lenth + 3;
			temp_buf[1]=lenth>>8;
			temp_buf[2]=lenth;	
			memcpy(g_arrSendBuffer,temp_buf,3);
			//tx_ptr = g_arrSendBuffer+3;
			FeedDog();
			
			crc_ccitt_upgrade(&g_arrSendBuffer[lenth], &g_arrSendBuffer[3], lenth-3);
			g_arrSendBuffer[lenth+2]= 0x03;
			
			
			rs485_SendStart(g_arrSendBuffer, lenth+3);			//SOF+LEN(2)+
		}
		else
		{
			lenth  = rs485_pack(g_arrSendBuffer,RS485_CMD_SEND,  p_buf, lenth);	
			rs485_SendStart(g_arrSendBuffer, lenth);
		}
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_LAN)
	{
		FeedDog();
		//tx_ptr = g_arrSendBuffer;
		//memcpy(tx_ptr,p_buf,lenth);
		W5500_SocketSend(0, p_buf, lenth);
	}
	else
	{
		FeedDog();
		//tx_ptr = g_arrSendBuffer;
	//	memcpy(tx_ptr,p_buf,lenth);
		WIFI_SendStart(p_buf, lenth);
		//WIFI_SendStart(buf_tmp, lenth);  //hcp
	}
	//printf(p_buf);

	/*
	printf(" send uuuuuuuuuuuudata\r\n\r\n");
	if(strSystemParameter.WorkMode == COMM_PORT_LAN)
		printf(SockParameters[0].tx_buf);
	else if(strSystemParameter.WorkMode == COMM_PORT_RS485)
		printf(Rs485UartPort.send_buf);
	else 
	   printf(WifiUartPort.send_buf);*/
	//free(p_buf);
}

int my_search(const char* str, const char* pat)
{
	char* p = strstr(str, pat);
	if (p == NULL)
		return -1;
	else
		return (int)(p - str);
}

unsigned char *g_pUartStartPoint = NULL;

unsigned char ProtocalUpgrade(unsigned int iLength)
{
	unsigned char *pBuf = NULL ,*pTemp=NULL;
	unsigned char arrTemp[16];

	int iSum = 0;
	int iFind =0;

	pBuf = g_pUartStartPoint;

	FeedDog();
	iSum = my_search((char *)pBuf, "\r\n\r\n");
	if(iSum == -1) 
		return 0;

	if(pBuf[0] != 'H') 
		return 0;
	
	pTemp = (unsigned char *)strstr((char *)pBuf, "Content-Length: ");
	if(pTemp == NULL) 
		return 0;
	
	pBuf = pTemp + 16;
	iFind = my_search((char *)pBuf, "\r\n");
	if(iFind == -1) 
		return 0;
	
	FeedDog();
	memset(arrTemp,0,16);
	memcpy(arrTemp, pBuf, iFind%7);
	g_iTotalLength = (unsigned int )atoi((char *)arrTemp);

	pTemp = (unsigned char *)strstr((char *)pBuf, "bytes ");
	if(pTemp == NULL) 
		return 0;
	pBuf = pTemp + 6;
	iFind = my_search((char *)pBuf, "-");
	if(iFind == -1) 
		return 0; 
	FeedDog();
	memset(arrTemp,0,16);
	memcpy(arrTemp, pBuf, iFind%7);

	if(g_iStart !=  (unsigned int )atoi((char *)arrTemp)) 
		return 0;
	g_iStart = (unsigned int )atoi((char *)arrTemp);
	
	pBuf = pBuf + iFind+1;
	iFind = my_search((char *)pBuf, "/");
	if(iFind == -1) 
		return 0;

	memset(arrTemp,0,16);
	memcpy(arrTemp, pBuf, iFind%7);
	g_iEndpos = atoi((char *)arrTemp);
	pBuf = pBuf + iFind+1;

	iFind = my_search((char *)pBuf, "\r\n\r\n");
	if(iFind == -1) 
		return 0;
	FeedDog();
	memset(arrTemp,0,16);
	memcpy(arrTemp, pBuf, iFind%7);
	g_iTotalBinLength = (unsigned int )atoi((char *)arrTemp);
	pBuf = pBuf + iFind+4;

	//printf("iLength = %d, iSum = %d, g_iGetRealLength = %d\r\n", iLength,iSum,g_iGetRealLength);
	//if(iLength != (iSum+g_iGetRealLength + 5)) {printf("rev pack error\r\n");return 0;}
	//printf("iTotalLength=%d, iStart = %d,iEndpos = %d, iSum= %d, iTotalBinLength=%d\r\n",g_iTotalLength,g_iStart,g_iEndpos,iSum,g_iTotalBinLength);
	//printf("iLength =%d\r\n",iLength);
	FeedDog();
	OwnSaveUpgrade(g_iStart, pBuf, g_iTotalLength); 

	if(g_iStart == 0) g_iCrcCalValue = 0; 
	if(g_iTotalLength == g_iUpgradePackNum)
		g_iCrcCalValue = CalBinCRC16(g_iCrcCalValue, pBuf, g_iTotalLength);
	else
	{
		g_iCrcCalValue = CalBinCRC16(g_iCrcCalValue, pBuf, g_iTotalLength-2);
		g_iCrcReadValue = pBuf[g_iTotalLength-2] * 256 + pBuf[g_iTotalLength-1];
		if(g_iCrcCalValue != ((~g_iCrcReadValue)&0xFFFF)) 
		{
			printf("Upgradebin file crc is not OK\r\n");
			iStartUpgrade = 0;g_iStart = 0;
			return 2;
		}
	}

	return 1;
}


unsigned char UartDealUpgrade(void)
{
	if(Rs485UartPort.rcv_all_flag == 0) 
		return 0;
	g_pUartStartPoint = NULL;
	g_pUartStartPoint = (unsigned char *)strstr((char *)Rs485UartPort.rcv_buf, "HTTP");
	if(g_pUartStartPoint == NULL) 
	{
		Rs485UartPort.rcv_all_flag= 0;
		return 0;
	}
	if( ProtocalUpgrade(Rs485UartPort.rcv_len) == 1) 
	{
		Rs485UartPort.rcv_all_flag= 0; 
		return 1;
	}
	Rs485UartPort.rcv_all_flag= 0;
	return 0;
}

unsigned char WIFIDealUpgrade(void)
{
	if( (WIFIUpgradeStartFlag == 1) && (WIFITimeTic >5))
	{
		g_pUartStartPoint = NULL;
		g_pUartStartPoint = (unsigned char *)strstr((char *)WifiUartPort.rcv_buf, "HTTP");
		if(g_pUartStartPoint == NULL) 
		{
			WifiUartPort.rcv_index = 0; 
			WifiUartPort.rcv_state=0;
			return 0;
		}
		
		if( ProtocalUpgrade(WifiUartPort.rcv_index) == 1) 
		{	
			WifiUartPort.rcv_index = 0; 
			WifiUartPort.rcv_state=0;
			return 1;
		}
		else
		{
			WifiUartPort.rcv_index = 0; 
			WifiUartPort.rcv_state=0;
			return 0;
		}
	}
	return 0;
}

unsigned char TCPDealUpgrade(void)
{
	unsigned short len = 0;
	
		u8_t state = 0;
	
	state = W5500_getSn_SR(0);
	
	if (state !=0x17)
	printf("tcp state:0x%02x\r\n", state);
	
	
	switch(state)											
	{
		case SOCK_INIT:								
			W5500_SocketConnect(0, strSystemParameter.DistinIp,strSystemParameter.DistinPort);	
			LanConnectFlag = 0;		
			break;
		case SOCK_ESTABLISHED:								
			if(W5500_getSn_IR(0) & Sn_IR_CON)   					
			{
				W5500_setSn_IR(0, Sn_IR_CON);
				LanConnectFlag = 1;
				//printf("Lan Connect Server Suc\r\n");
			}
			len=W5500_getSn_RX_RSR(0);			
			if(len>0)
			{
				len = W5500_SocketRecv(0,&g_arrRecvBuffer[0],len);
				g_pUartStartPoint = NULL;
				g_pUartStartPoint = (unsigned char *)strstr((char *)g_arrRecvBuffer, "HTTP");
				if(g_pUartStartPoint == NULL) {return 0;}               
				if( ProtocalUpgrade(len) == 1) return 1;    
			}
			break;
		case SOCK_CLOSE_WAIT:										
			W5500_SocketClose(0);	
			break;
		case SOCK_CLOSED:					
			W5500_Socket(0,Sn_MR_TCP,strSystemParameter.LocalPort,Sn_MR_ND);	
			break;
	}
	return 0;
}  

void UpgradeOperation(void)
{
	unsigned char bError= 0;
	unsigned short iUpError =0;
	int i =0;
  u32_t error_cnt = 0;
	
	if(g_bStartUpgradeFlag == 1)
	{
		
		DEBUG_PRINTF("start upgrade\r\n");
		FeedDog();
		if(g_iStart == 0) 
		{
			OwnDelSector();
		}
		FeedDog();    
		if(strSystemParameter.WorkMode == 1)  
			g_iUpgradePackNum = 1024;
		else 
			g_iUpgradePackNum = 4096;
		
		while(iStartUpgrade< iTotalUpgradePack)
		{
			FeedDog();
			
			if(strSystemParameter.WorkMode == 1)  
				DelayMs(10);
			else if(strSystemParameter.WorkMode == 2)  
			{
				DelayMs(100);
				WifiUartPort.rcv_state = 0;
				
			}
			DEBUG_PRINTF("iStartUpgrade = %d\r\n",iStartUpgrade);
			
			
			g_bWifiUpgradeFlag = 1;
			SendUpgradeData(g_iStart); 
			WIFIUpgradeStartFlag = 0;	
			
			for(i=0;i<20000; i++)  //10s 超时
			{
				FeedDog();
				if(strSystemParameter.WorkMode == 0)
					bError = UartDealUpgrade();
				else if(strSystemParameter.WorkMode == 1)
					bError = TCPDealUpgrade();
				else
				{
					WIFI_CommTask();
					bError = WIFIDealUpgrade();				
				}
				
				DelayMs(1);
				if(bError== 1) //suc
				{
					break;
				}
			}
			
			if(bError == 0) 
			{
				
				if (error_cnt++ > 0)
				{
					break; 
				}
				else
				{
					DEBUG_PRINTF("upgrade again\r\n");
					continue;
				}
			}
			
			
			if(g_iStart == 0)
			{
				if((g_iTotalBinLength % g_iUpgradePackNum) == 0)
					iTotalUpgradePack = g_iTotalBinLength / g_iUpgradePackNum;
				else iTotalUpgradePack = g_iTotalBinLength / g_iUpgradePackNum + 1;
			}
			g_iStart = g_iStart + g_iUpgradePackNum;
			iStartUpgrade ++;
			g_bWifiUpgradeFlag = 0; 
			iUpError++;
			if(iUpError > iTotalUpgradePack*2) break;
		}
		
		DEBUG_PRINTF("upgrade End\r\n");
		g_bStartUpgradeFlag = 0;  //这样升级优先级高  hcp 2015-12-13
		WIFIUpgradeStartFlag = 0;
		WifiUartPort.rcv_state = 0;
		g_bWifiUpgradeFlag = 0;
		if(iStartUpgrade >= iTotalUpgradePack)
		{
			g_bStartUpgradeFlag = 0; 
			DEBUG_PRINTF("upgrade OK, reboot\r\n");
			
			WriteApplicationFlag(0x55);
			SYS_RESTART();
//			WriteApplicationFlag(0x55);
//			//g_sysrestart_flag = 1;
//			RestartSys();
		}
		else
		{
		}
	}
}

