
#include "global.h"
#include "linklayer.h"
#include "W5500.h"
#include "Uart.h"
#include "protocol_typedef.h"
#include "protocol.h"
#include "gpio_conf.h"
#include "upgrade.h"
#include "memory.h"
#include "log.h"
#include "main.h"
#include "rs485.h"
#include "rtc.h"
#include "wifi.h"
#include "SystemParameter.h"
#include "debug.h"
#include "base64.h"

extern unsigned int TestTotal;

unsigned char g_arrRecvBuffer[TCP_MAX_RCV_LEN];
unsigned char g_arrSendBuffer[TCP_MAX_SEND_LEN];
unsigned short g_iRevStartPos = 0;

SERVER_COMM_CTRL g_stServerComCtrl;
PProtocol_ST Pprotocol;

SERVER_REPLY_CODE ServerReplyCode;

unsigned char  g_bServerUploadReq = 0;			//�����ϴ����󣬼��ʱ�䳬��1�����������ݣ��򻺴泬��10���������ϴ�
unsigned char g_bServerUploadSucFlag = 0;			//�����ϱ��ɹ���־
unsigned short g_iServerUploadCommTimeCnt = 0;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
unsigned short g_iServerHeartCommTimeCnt = 0;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
volatile unsigned char  g_bServerHeartbeatReq = 0;
volatile unsigned char  g_bServerHeartbeatSucReq = 0;  //hcp 2015-10-19
unsigned char  g_bServerLoginReq = 0;
unsigned char  g_bServerLoginSucFlag = 0;

unsigned short g_iTotalSendRecordCnt = 0;
unsigned short g_iMaxSendRecordCnt = 0xFFFF;

unsigned char  g_bStartUpgradeFlag = 0;
unsigned char  g_bSendAgainFlag = 0;
unsigned short g_iSendLength = 0;

BOOL  g_bUploadOkFlag = FALSE;

volatile unsigned char  g_bCommReceiveFlag = 0;

static unsigned int RepeatLoginTimeCnt = 15;			//��¼���ɹ����·���ʱ�����
static unsigned int g_iResendTimes = 0;

static int g_ttlLoginOtCnt = 0;
#define RESTART_LOGIN_TIMEOUT       10           //10min  ����  hcp

Pchar buff = 0;

enum 
{
	comm_reg,
	comm_login,
	comm_upload
}EmServerCommState;


const unsigned char g_arrHttp500Error[]={"HTTP/1.1 500"};
const unsigned char g_arrHttp400Error[]={"HTTP/1.1 400"};
const unsigned char g_arrHttp20[]      ={"HTTP/1.1 20"};

void linksend(unsigned char* data,unsigned short datalen)
{
    memcpy(&g_arrSendBuffer[g_iSendLength], data, datalen);
    g_iSendLength = g_iSendLength + datalen;
	
    //printf(data);
    //printf("\r\ndatalen = %d\r\n",datalen);
}

void LinkSendDataFunc(void)
{
	int i=0;
	int iRound = 0;
	if(sys_check_linktype(COMM_PORT_LAN))
	{
		//if(TestReq&TEST_REQ_PRINT_COMM)
			//	DEBUG_PRINTF(">>Sent total:%d bytes\r\n",g_iSendLength);
		
		if(LanConnectFlag)
		{
			FeedDog();
			if(g_iSendLength > TCP_MAX_SEND_LEN)
				return;
		//	W5500_SocketSend(0, g_arrSendBuffer, g_iSendLength);	// ��Ϊһ���Է���,gaojun 2015-07-16
			
			if(g_iSendLength % 1024 == 0)
				iRound = g_iSendLength / 1024;
			else 
				iRound = g_iSendLength / 1024 + 1;
			
			for(i = 0; i<iRound-1; i++)
			{   
				FeedDog();
				W5500_SocketSend(0, &g_arrSendBuffer[1024*i], 1024);
				DelayMs(20);
				FeedDog();
			}
			W5500_SocketSend(0, &g_arrSendBuffer[1024*i], g_iSendLength - i*1024);
			
			FeedDog();
			
		}
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		WIFI_SendStart(g_arrSendBuffer, g_iSendLength);
	}
	else
	{
		rs485_SendStart(g_arrSendBuffer, g_iSendLength);
	}
}

unsigned char* linkrecv(unsigned short *datalen)
{
	*datalen = g_iRevStartPos;
	return g_arrRecvBuffer;
}

void linkfree()
{
	memset(g_arrRecvBuffer, 0, g_iRevStartPos);
	g_iRevStartPos = 0;  
	g_iSendLength = 0;
	//printf("linkfree\r\n");
}

void ProtocolInitial(void)
{
	char accountID[22];
	char str[20]={0};
//	u8_t sn[20] = {0};
	
	memset(accountID, 0, 22);
	//sys_get_params(sn,SYS_SN);
	if(strlen((char *)strSystemParameter.ProductSn) != 16) 
		strcpy((char *)strSystemParameter.ProductSn,(char *)"0000000000000001");
	sprintf((char *)accountID, "%s%s", BOARDVersion, strSystemParameter.ProductSn);

	DelayMs(100);
	RTC_GetDate(RTC_Format_BIN, &SystemDate); //��ȡ����	
	RTC_GetTime(RTC_Format_BIN, &SystemTime); //��ȡʱ��

	if(strSystemParameter.WorkMode == COMM_PORT_RS485)
	{
		Pprotocol = Protocol_Builder(PT_485,linksend,linkrecv,linkfree);
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		Pprotocol = Protocol_Builder(PT_LAN,linksend,linkrecv,linkfree);
	}
	else
	{
		Pprotocol = Protocol_Builder(PT_LAN,linksend,linkrecv,linkfree);
	}
	Pprotocol->setparam(Pprotocol,PP_ACCOUNT,accountID);
	Pprotocol->setparam(Pprotocol,PP_MAGIC,"02");
	Pprotocol->setparam(Pprotocol,PP_SESSIONID,"0");

	sprintf(str,"%02d-%02d-%02d",SystemDate.RTC_Year,SystemDate.RTC_Month,SystemDate.RTC_Date);
	Pprotocol->setparam(Pprotocol,PP_CREATEDATE,str);
	sprintf(str,"%02d:%02d:%02d",SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);
	Pprotocol->setparam(Pprotocol,PP_CREATETIME,str);
	if(strSystemParameter.WorkMode == COMM_PORT_RS485)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"485");
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_LAN)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"lan");
	}
	else if(strSystemParameter.WorkMode == COMM_PORT_WIFI)
	{
		Pprotocol->setparam(Pprotocol,PP_WEBTYPE,"wifi");
	}
	sprintf(str, "%d.%d.%d.%d",strSystemParameter.DistinIp[0],strSystemParameter.DistinIp[1],\
							  strSystemParameter.DistinIp[2],strSystemParameter.DistinIp[3]);
	Pprotocol->setparam(Pprotocol,PP_HOST,str);
	sprintf(str, "%d",strSystemParameter.DistinPort);
	Pprotocol->setparam(Pprotocol,PP_PORT,str);
	Pprotocol->setparam(Pprotocol,PP_PASSWWORD,"a4F06cKAil8=");
	Pprotocol->setparam(Pprotocol,PP_PSDMD5,"6E0C2BC650D2810B");

//	g_stServerComCtrl.bState = SERVER_PKT_NONE;
}

void FlushProtocalTime(void)
{
	char str[32];
	sprintf(str,"20%02d-%02d-%02d",SystemDate.RTC_Year,SystemDate.RTC_Month,SystemDate.RTC_Date);
	Pprotocol->setparam(Pprotocol,PP_CREATEDATE,str);
	sprintf(str,"%02d:%02d:%02d",SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);
	Pprotocol->setparam(Pprotocol,PP_CREATETIME,str);
}
void LinkPrintPortRecv(void)
{
	if(DebugUartPort.rcv_all_flag)
	{
		DebugUartPort.rcv_len=0;
		DebugUartPort.rcv_all_flag=0; //�������
		DebugUartPort.rcv_state=0;
		
		if (DebugUartPort.g_bluetooth_flag == 1)
		{
			DealSysParamBlue();
		}
		else
		{
			DealSysParameter();
		}
	}
}

void ServerCommStateInit(void)
{
	EmServerCommState = comm_login;
	g_stServerComCtrl.tx_rx_state = 0;
	g_stServerComCtrl.tx_flag= 0;
	g_stServerComCtrl.rx_flag = 0;
	g_bServerHeartbeatReq = 0;
	g_bServerUploadReq = 0;
	g_bServerLoginReq = 0; 
	g_bServerLoginSucFlag = 0 ;
	g_bServerUploadSucFlag = 0;
	g_bSendAgainFlag = 0;
}
void InitServerValue(void)
{
	g_bServerHeartbeatReq = 0; //����������
	g_iServerHeartCommTimeCnt = 0; // 2016-02-17 hcp
	g_bServerUploadReq = 0;
	g_iServerUploadCommTimeCnt = 0;
}

/************************************************************************************************* 
* ��������	: ���ݽ��մ���                                                         
* ����			:  ��                                                            
*                               
* ����			:                                                              
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 
void Server_RcvDataHandle(void)
{
	unsigned char *para_data;
	unsigned char i = 0;
	unsigned char len = 0;
	char *pTemp = NULL, *pData = NULL, *pTime = NULL;
	char *pParam = NULL, *pParamver = NULL;
	int  iReturn = 0;

	/*���޸� ����������Ҫ���ж�����PP_ERRORCODE��Ȼ�����ж�PP_RESULTCODE*/
	pTemp = Pprotocol->getparam(Pprotocol,PP_ERRORCODE);
//	printf("PP_ERRORCODE :%s\r\n",pTemp);
	iReturn = atoi(pTemp);
	if(iReturn == 0x01) 
	{
		if(TestReq&TEST_REQ_PRINT_COMM)
			DEBUG_PRINTF(">>Relogin...\r\n");
		ServerCommStateInit();	//���¿�ʼͨ������
		return;
	}
	pTemp = Pprotocol->getparam(Pprotocol,PP_RESULTCODE);		//���ؿ�?????????
	ServerReplyCode.err_code = atoi(pTemp);  
//	printf("PP_RESULTCODE :%s\r\n",pTemp);
	if(ServerReplyCode.err_code == 99) 		//���ݰ�δ������
		return;
	
	if(ServerReplyCode.err_code != 0)			//���ݰ����ؽ������
	{
		g_bServerLoginSucFlag = 0;
		g_stServerComCtrl.rx_flag = 1;
		return;
	}
	
	pData = Pprotocol->getparam(Pprotocol,PP_CREATEDATE);
	pTime = Pprotocol->getparam(Pprotocol,PP_CREATETIME);

	SetDateAndTime(pData,pTime);
	if(EmServerCommState == comm_login) 
	{
		g_bServerLoginSucFlag = 1;
		g_stServerComCtrl.rx_flag = 1;
		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		return;
	}
	if(g_bServerHeartbeatReq)
	{
		g_bUploadOkFlag = TRUE;
		//	StartBeep(20,1);//�ϵ�����
		g_bServerHeartbeatReq = 0; //����������
		g_iServerHeartCommTimeCnt = 0;
		#ifdef EN_USING_LOG
		AddLog("Heartbeet replay.\r\n");
		#endif
		if(TestReq&TEST_REQ_PRINT_COMM)
		{
			printf("Heartbeet replay.\r\n");
		}
		len = 0;
		pParamver = Pprotocol->getparam(Pprotocol,PP_APPARAMVER);
		if(pParamver != NULL)
		{
			if(*pParamver >= '0')
			{
				len = strlen(pParamver);
				if(len <= 15)
				{
					if(memcmp(&strSystemParameter.Param_Ver[0],pParamver,len))
					{
						memset(strSystemParameter.Param_Ver,0,16);
						memcpy(strSystemParameter.Param_Ver,pParamver,len+1);
						printf("%s\r\n",strSystemParameter.Param_Ver);
					}
					else
					{
						printf("PararmVer Same!\r\n");
					}
				}
			}
			else
			{
				printf("PararmVer Err!\r\n");
			}
		}
		else
		{
			printf("No PararmVer!\r\n");
		}
		if(len)
		{
			pParam = Pprotocol->getparam(Pprotocol,PP_APPARAM);
			if(pParam != NULL)
			{
				i = 0;
				para_data = base64_decode((uint8_t *)pParam);
				while(para_data[i] == 0xF0)
				{
					if(para_data[i+1] == 1)
						strSystemParameter.HraerBeatTimeout = ((para_data[i+4]<<8) + para_data[i+5])*60+1;
					else if(para_data[i+1] == 2)
						strSystemParameter.SendDataDealyTime = ((para_data[i+4]<<8) + para_data[i+5])*60+1;
					else if(para_data[i+1] == 3)
						strSystemParameter.SendDataMaxCnt = (para_data[i+4]<<8) + para_data[i+5];
					else
						break;
					printf("%02X %02X %02X %02X %02X %02X\r\n",para_data[i+0],para_data[i+1],para_data[i+2],para_data[i+3],para_data[i+4],para_data[i+5]);
					i += 6;
				}
				if(i > 0)
					i = 0xAA;
				free(para_data);
				para_data = NULL;
			}
		}
		if(i == 0xAA)
		{
			printf("Save PararmVer!\r\n");
			sys_write_param();
		}
		
		pTemp = Pprotocol->getparam(Pprotocol,PP_SOFTVERSION);

		//printf("PP_SOFTVERSION:%s\r\n",pTemp);
		
		pTemp = Pprotocol->getparam(Pprotocol,PP_URL);
		
		//printf("PP_URL:%s\r\n",pTemp);
		
		if(pTemp != NULL) 
		{
	//		printf("Start Upgrade...\r\n");
			
			strcpy((char *)UpgradeList.arrUrl, pTemp); 
			pTemp = Pprotocol->getparam(Pprotocol,PP_SOFTVERSION);
			UpgradeList.bVersion = atoi(pTemp); 
			
			if(TestReq&TEST_REQ_PRINT_COMM)
							printf("ver:%d url:%s \r\n",UpgradeList.bVersion,UpgradeList.arrUrl);
			
			if(MYVERSION != UpgradeList.bVersion             //hcp 2015-10-24 ��ǿ�°汾�ж�����
				 &&UpgradeList.bVersion != 0)
			{
				g_bStartUpgradeFlag = 1;
				printf(" Start upgrade...\r\n");
			}
		}
		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		g_stServerComCtrl.rx_flag= 1;
		g_bServerHeartbeatSucReq = 1;
		g_iServerHeartCommTimeCnt = 0;
		g_bSendAgainFlag = 0;
		return;
	}
	if(g_bServerUploadReq)
	{
		//��� flash ������ temp flash���е�ͬ��У�鲢д��־ hcp 2015-10-12
		if (TagRecordFlashQueue.tail_addr != FlashTempQueue.bk_tail_addr
			|| TagRecordFlashQueue.cnt < FlashTempQueue.bk_cnt
		  || TagRecordFlashQueue.cnt > TagRecordFlashQueue.max_cnt)
		{
			//����ع� ���������ع�
			//TagRecordFlashQueue.tail_addr = FlashTempQueue.bk_tail_addr;
			//TagRecordFlashQueue.cnt = FlashTempQueue.bk_cnt;
			
			sprintf(LogStrBuf,"TagRecordFlashQueue error tail:%08X-%08X cnt:%d-%d \r\n",
			TagRecordFlashQueue.tail_addr,FlashTempQueue.bk_tail_addr,
			TagRecordFlashQueue.cnt,FlashTempQueue.bk_cnt);	
			
			AddLog(LogStrBuf);		
		}
		else
		{
			TagRecordFlashQueue.tail_addr = FlashTempQueue.tail_addr;
			TagRecordFlashQueue.cnt -= g_iTotalSendRecordCnt;
		}
		
		FlashQueue_SaveParam(&TagRecordFlashQueue);
		
		g_iTotalSendRecordCnt = 0;
		
//		printf("Data upload code %dr\n",ServerReplyCode.result_code);
		
		Pprotocol->setparam(Pprotocol,PP_RESULTCODE,"99");
		g_stServerComCtrl.rx_flag= 1;
		g_bServerUploadSucFlag = 1;
		g_iMaxSendRecordCnt = 0xFFFF;
		
		return;
	}
}

void ChangeDataFormat(unsigned char *pDest, unsigned char *pSrc)
{
	memcpy(pDest, pSrc, 4);
	GetRecordDate(&pDest[4], &pSrc[7]);
	pDest[6] = pSrc[4];
	GetRecordTime(&pDest[7], &pSrc[7]);
	pDest[9] = pSrc[5];
	pDest[10] = pSrc[6];
}

/************************************************************************************************* 
* ��������	: ����ͨ�����̿���                                                        
* ����			:  ��                                                            
*                               
* ����			:  ��                                                               
*        			                                   
* ��ע:                                                                  
*                                                                        
**************************************************************************************************/ 

#define SERVER_COMM_LOGIN_REPEAT   20 //20 SECC

void Server_CommTask(void)	
{
	unsigned char err_string[16];
	unsigned char strSrc[16];
	unsigned char strDst[16];
	unsigned char strTemp[48];
	unsigned short iRet = 0,iFlashQueueRet = 0; 
	BOOL   bTagDatOk = FALSE;
	
//	uint8  errorFlag = 0;
//	uint32 bWifiErrorTime = 0;
	int  serverCommTimeout = SERVER_COMM_TIMEOUT;
	int temp = 0;
	
	if (strSystemParameter.WorkMode == COMM_PORT_RS485)   //hcp
		 serverCommTimeout = SERVER_COMM_TIMEOUT*2;

	if((LanConnectFlag == 0)&&(WifiConnectFlag == 0) && 
		 strSystemParameter.WorkMode != COMM_PORT_RS485)		// ���������ж�
	{
		if(EmServerCommState == comm_upload)
		{
			if(TestReq&TEST_REQ_PRINT_COMM)
			{
				DEBUG_PRINTF(">>Net error,Repair data, Buf tag cnt:%d \r\n",TagRecordFlashQueue.cnt);
			}
			#ifdef EN_USING_LOG
			AddLog("Net error.");
			#endif
		}
		ServerCommStateInit();
		CLR_LED2();		
		
		/////hcp 2015-10-30
		temp = RESTART_LOGIN_TIMEOUT*60;
		if (TestReq != 0) temp = temp*RESTRAT_TEST_TIMEOUT;
		if (g_ttlLoginOtCnt++ >= temp)  // ���س�ʱ����
		{
			g_ttlLoginOtCnt = 0;
			g_sysrestart_flag = 1;
		}
		
		return;
	}
	RepeatLoginTimeCnt++;
	switch(EmServerCommState)
	{
		case comm_login://��¼
			if(g_stServerComCtrl.tx_rx_state == 0)		//����״̬,Uart2BusyFlag�����ĵ����ͳ�����ȥ�ж�
			{
				if(RepeatLoginTimeCnt > SERVER_COMM_LOGIN_REPEAT)
				{
					RepeatLoginTimeCnt = 0;
					Pprotocol->clear(Pprotocol);
					linkfree();
					FlushProtocalTime();
					Pprotocol->login(Pprotocol); 
					LinkSendDataFunc();//��¼
						if(TestReq&TEST_REQ_PRINT_COMM)
						{
							DEBUG_PRINTF(">>Login...\r\n");
						}
						#ifdef EN_USING_LOG
						AddLog("Login...");
						#endif
					g_stServerComCtrl.tx_rx_state = 1;
					g_stServerComCtrl.timeout_cnt = 0;
				}
				break;
			}
			else											 //����״̬
			{
				if(g_stServerComCtrl.rx_flag)		
				{
					g_stServerComCtrl.rx_flag = 0;
					if(g_bServerLoginSucFlag)			//��¼�ɹ�
					{
						EmServerCommState = comm_upload;		//�������ݽ����׶�		
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Login Server sucess\r\n");
						#ifdef EN_USING_LOG
						AddLog("Login Server sucess");
						#endif
						 
						g_ttlLoginOtCnt = 0;
					}
					else
					{
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Login error\r\n");
					}
					g_stServerComCtrl.tx_rx_state = 0;
					RepeatLoginTimeCnt = 0;
					InitServerValue();
					break;
				}
				else
				{
					g_stServerComCtrl.timeout_cnt++;
					if(g_stServerComCtrl.timeout_cnt >= serverCommTimeout)		//��ʱ20��
					{
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Login timeout cnt = %d\r\n",g_ttlLoginOtCnt);
						RepeatLoginTimeCnt = 0;
						g_stServerComCtrl.tx_rx_state = 0;
						InitServerValue();
						if(strSystemParameter.WorkMode == COMM_PORT_WIFI)		//WIFI �ӿڣ�����
						{
							  WIFI_Reset();		//����ǲ��Ϸ�����������WIFI����û����gaojun 2015-07-23
						}
						else if(strSystemParameter.WorkMode == COMM_PORT_RS485) 
						{
								//
						}
						
						/////hcp 2015-10-30
						temp = RESTART_LOGIN_TIMEOUT*2;
						if (strSystemParameter.WorkMode == COMM_PORT_RS485)
						{
							temp = RESTART_LOGIN_TIMEOUT;
						}
						
//						if (TestReq != 0) temp = temp*RESTRAT_TEST_TIMEOUT;
						if (g_ttlLoginOtCnt++ >= temp)  // ���س�ʱ����
						{
							g_ttlLoginOtCnt = 0;
							g_sysrestart_flag = 1;
							DEBUG_PRINTF(">>ReLogin cnt Max-sys restart!\r\n");
						}
					}
					
					
				}
			}				 
			break;
		case comm_upload:
			if(g_stServerComCtrl.tx_rx_state == 0)						//����״̬
			{
			   if(g_bServerHeartbeatReq)	//����������
				{
					if(g_bSendAgainFlag == 0)	
					{
						Pprotocol->clear(Pprotocol);
						Pprotocol->setparam(Pprotocol,PP_MODEL,BOARDVersion);
						sprintf((char *)strTemp, "%d", MYVERSION);
						Pprotocol->setparam(Pprotocol,PP_SOFTVERSION,(char *)strTemp); 
						Pprotocol->setparam(Pprotocol,PP_APPARAMVER,strSystemParameter.Param_Ver); //��Ӳ����汾 --LD 20160729 
						base64_encode1(Err_Code,err_string,ERR_CODE_LEN);
						Pprotocol->setparam(Pprotocol,PP_APWARNING,(char *)err_string); 		//��Ӿ�����--LD 20160831
						linkfree();
						FlushProtocalTime();
						Pprotocol->heartpack(Pprotocol);
						LinkSendDataFunc();
						g_stServerComCtrl.tx_rx_state = 1;
						g_stServerComCtrl.timeout_cnt = 0;	
						g_bServerHeartbeatSucReq = 0;
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Heartbeat...\r\n");
						
						g_iResendTimes = 3;
					}
					else
					{
						if (!g_iResendTimes)    // ���޸� 2015-07-09 �������ط��������ƣ�����ط�����3����Ϊ�Ͽ�������
						{
								g_bSendAgainFlag = 0;
								ServerCommStateInit();			//���¿�ʼ��¼
								break;
						}
							linkfree();
							FlushProtocalTime();
							Pprotocol->resend(Pprotocol); 
							LinkSendDataFunc();
							g_iResendTimes--;
							if(TestReq&TEST_REQ_PRINT_COMM)
								DEBUG_PRINTF(">>Resend... \r\n"); 
					}
				}
			  else if(g_bServerUploadReq)		//���ݰ�����
				{
					if(g_bSendAgainFlag == 0)		//���ݰ��״�
					{
						if(TagRecordFlashQueue.cnt == 0)	//����Ϊ��
						{
							g_bServerUploadReq = 0;
							break;
						}
						if(TestReq&TEST_REQ_PRINT_COMM)
				     			DEBUG_PRINTF(">>Buf tag cnt:%d\r\n\0",TagRecordFlashQueue.cnt); 
						  
						g_iTotalSendRecordCnt = 0;

						Pprotocol->clear(Pprotocol);
						iFlashQueueRet = 1;
						
						FlashQueue_Copy(&FlashTempQueue,&TagRecordFlashQueue);
						//printf("msg1: %32X - %32X - %d - %d!\n",FlashTempQueue.tail_addr, FlashTempQueue.head_addr, FlashTempQueue.cnt, FlashTempQueue.data_len);
						while(iFlashQueueRet)
						{
							iFlashQueueRet = FlashQueue_GetTagData((FLASH_QUEUE *)&FlashTempQueue,strSrc);
							
							
							
							if(iFlashQueueRet == 0) 
							{
//								errorFlag  =1;
								break;  //û��������
							}
							
							if((strSrc[1] == 0xFF)&&(strSrc[2] == 0xFF)&&(strSrc[3] == 0xFF)&&(strSrc[4] == 0xFF))							
							{
//								errorFlag  =2;
								break;	//���ݴ���						gaojun   2015-09-11
							}
							
							FeedDog();
							
							//��֤flash ����  2016-1-20 hcp
							bTagDatOk = FALSE;
							if (strSrc[15]  == CheckSum(strSrc,15))
							{
								bTagDatOk = TRUE;
							}
							//bTagDatOk = FlashQueue_CheckTagData((FLASH_QUEUE *)&FlashTempQueue,strSrc,1);
							if (!bTagDatOk)
							{
								printf("flash error:%02X%02X%02X%02X %02X%02X%02X%02X %02X%02X%02X%02X\r\n",
								strSrc[0],strSrc[1],strSrc[2],strSrc[3],
								strSrc[4],strSrc[5],strSrc[6],strSrc[7],
								strSrc[8],strSrc[9],strSrc[10],strSrc[11]);
								#ifdef EN_USING_LOG
								sprintf(LogStrBuf,"flash error addr:%08x\r\n",FlashTempQueue.tail_addr);
								AddLog(LogStrBuf);
								#endif
							}
							

							ChangeDataFormat(strDst, &strSrc[1]);
							iRet = Pprotocol->puttag(Pprotocol,PT_DEVICE,strDst,11);
							if(iRet == 0)
							{
								FlashQueue_FinTagGet((FLASH_QUEUE *)&FlashTempQueue);  //hcp 2015-10-21 �ɹ�ȡ�������޸�tail
								g_iTotalSendRecordCnt++;

								}
							else 
							{
								//if(TestReq&TEST_REQ_PRINT_COMM)
								//printf("msg:puttag full!\n");
								break;
							}
						}
						
						if (!g_iTotalSendRecordCnt)	//  ���޸� 2015-07-09 ��������Ч���ݵ��жϣ��������������Ϊ��ʱ�������з���
						{
							if(TestReq&TEST_REQ_PRINT_COMM)
								DEBUG_PRINTF("err:send null package!\n");
							break;
						}
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF("Send Cnt = %d\r\n",g_iTotalSendRecordCnt);    
						#ifdef EN_USING_LOG
						sprintf(LogStrBuf,"Upload %d records...",g_iTotalSendRecordCnt);
						AddLog(LogStrBuf);
						#endif
						g_iSendLength = 0;
						linkfree();
						FlushProtocalTime();
										
						Pprotocol->syncbatch(Pprotocol); 
						LinkSendDataFunc();
						
						g_iResendTimes = 3;		//�ط�������ʼ��
						
						
					}
					else												//���ݰ��ط�
					{
					    
						if (!g_iResendTimes)    // ���޸� 2015-07-09 �������ط��������ƣ�����ط�����3����Ϊ�Ͽ�������
						{
							g_bSendAgainFlag = 0;
						//	FlashQueue_GetParam(&TagRecordFlashQueue);	//�ָ�FLASH �洢����
							ServerCommStateInit();			//���¿�ʼ��¼
							if(TestReq&TEST_REQ_PRINT_COMM)
								DEBUG_PRINTF(">>Send failed,Repair data. Buf tag cnt:%d \r\n",TagRecordFlashQueue.cnt); 
//							if(strSystemParameter.WorkMode == COMM_PORT_LAN)
//							{
//								W5500_SocketClose(0);		//�����Ͽ�����	
//								g_iMaxSendRecordCnt = 400;
//							}
							break;
						}
						
						if(strSystemParameter.WorkMode == COMM_PORT_WIFI)		//WIFI �ӿڣ�����
						{
							  WIFI_Reset();		//20160415 ���ط� ��������ģ��
						}
						
						linkfree();
						FlushProtocalTime();
						Pprotocol->resend(Pprotocol); 
						LinkSendDataFunc();
						g_iResendTimes--;
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Resend... \r\n"); 
					}
          
				
					g_stServerComCtrl.tx_rx_state = 1;
					g_stServerComCtrl.timeout_cnt = 0;		

				}
				
				break;
			}
			else
			{
				if(g_stServerComCtrl.rx_flag)
				{
					g_stServerComCtrl.rx_flag = 0;
					g_stServerComCtrl.tx_rx_state = 0;
					g_iServerUploadCommTimeCnt = 0;
					if(g_bServerUploadSucFlag)			//�����ϱ��ɹ�
					{
						g_bServerUploadSucFlag = 0;
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Data Upload sucess.\r\n");
						#ifdef EN_USING_LOG
						sprintf((char *)strTemp,"Data Upload sucess. tail:%08x head:%08x\r\n\0",FlashTempQueue.bk_tail_addr,TagRecordFlashQueue.head_addr);
						AddLog((char *)strTemp);
						#endif
						g_bSendAgainFlag = 0;
						g_bServerUploadReq = 0;
						g_bUploadOkFlag = TRUE;
					}
					else if(g_bServerHeartbeatSucReq)
					{
						g_bServerHeartbeatSucReq = 0;
					}
					else
					{
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">>Data Upload error.\r\n");
						
						g_bSendAgainFlag = 1;				//���������ط�
					}
				}
				else
				{
					g_stServerComCtrl.timeout_cnt++;
					if(g_stServerComCtrl.timeout_cnt >= serverCommTimeout)		//��ʱ30 ��
					{
						g_stServerComCtrl.tx_rx_state = 0;
						if(TestReq&TEST_REQ_PRINT_COMM)
							DEBUG_PRINTF(">> Comm time out\r\n");		
						g_bSendAgainFlag = 1;				//���������ط�

					}
				}
			}
			break;

		default:
			break;
	}
	
					//	if(TestReq&TEST_REQ_PRINT_COMM && errorFlag > 0)
				//	printf("msg: error flag :%d  %d\n",errorFlag, TagRecordFlashQueue.cnt);
						
	if(g_bServerLoginSucFlag)
	{
		SET_LED2();
	}
	else
	{
		CLR_LED2();

	}
}
