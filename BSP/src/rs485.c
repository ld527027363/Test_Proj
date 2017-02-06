#include "global.h"
#include "rs485.h"
#include "stm32f4xx_it.h"
#include "upgrade.h"
#include "gpio_conf.h"
#include "Uart.h"
#include "linklayer.h"
#include "timer.h"
#include "SystemParameter.h"
#include "rtc.h"
#include "linklayer.h"
#include "Rf433Protocal.h"

//static unsigned char g_bAFlag = 0;
//static unsigned char g_bOKFlag = 0;
//static unsigned char SetupRs485Flag = 0;
unsigned char Rs485TimeTic = 0;
unsigned char Rs485UpgradeStartFlag = 0;

unsigned char Rs485ConnectFlag = 0;
unsigned char RS485_DMA_SendOK = 0;

static RS485_PRO_TYPE rs485_pro_type = RS485_PRO_P1;


/********************** ½ÓÊÕ **************/

static u8_t * s_recv_buf = 0;
static u32_t  s_recv_len = 0;
static u32_t  s_recv_idx = 0;
static u32_t  s_recv_timeout = 0;
static u32_t  s_recv_maxlen = 0;
	
static u32_t  *s_precv_timeout = 0;
static u32_t s_last_timeout = 0;


/*
* ÉèÖÃbuf
*/
void uart_set_buf(u8_t *pBuf, u32_t maxLen,u32_t * pTimeout, u32_t timeout)
{
	 s_recv_buf = pBuf;
	
	s_precv_timeout = pTimeout;
	
	s_recv_timeout = timeout;
	
	s_recv_maxlen = maxLen;
}

u8_t * uart_get_buf(void)
{
	return s_recv_buf;
}

/*
* »ñÈ¡³¤¶È
*/
u32_t uart_get_len(void)
{
	u32_t  nRet = 0;
	
	if (s_precv_timeout != 0)
	{
		if ((*s_precv_timeout- s_last_timeout)>= s_recv_timeout 
			|| s_recv_len >= s_recv_maxlen-1)
		{
			nRet =   s_recv_len;
		}
		else
		{
			
		}
	}
	else
	{
		nRet =   s_recv_len;
	}
	
	return nRet;
}


/*
* »ñÈ¡×Ö½Ú
*/
u8_t uart_get_byte(void)
{
	return  s_recv_buf[s_recv_idx++];
}

/*
* ÖØÖÃ
*/
void uart_recv_reset(void)
{
	s_recv_idx = 0;
	s_recv_len = 0;
}



void uart_set_dat(u8_t dat)
{
	s_last_timeout = *s_precv_timeout;
	if (s_recv_buf != 0 && s_recv_len < s_recv_maxlen)
	{
		if (!Rs485UartPort.rcv_all_flag)
			s_recv_buf[s_recv_len++] = dat;
		
	}
}



void rs485_set_pro(RS485_PRO_TYPE type)
{
	//u8_t buf[16] = {0};
	
	//sprintf((char *)buf, ">>485 p:%d \r\n",rs485_pro_type);
	//Printf((char *)buf);	
		
	rs485_pro_type = type;
}

RS485_PRO_TYPE rs485_get_pro(void)
{
	
	return rs485_pro_type;
}



/********************** Ð­Òé¶ 2 ***********/



rs485_info_t s_rs485_info = {0};

//»ñÈ¡ÁîÅÆ ºó×´Ì¬
#define rs485_set_sending() do{\
	s_rs485_info.bSending = TRUE;\
}while(0);
	
#define rs485_clr_sending() do{\
	s_rs485_info.bSending = FALSE;\
}while(0);

#define rs485_get_flag() (s_rs485_info.bSending)

/*
**get set µØÖ·²Ù×÷
*/
void rs485_set_addr(u8_t addr)
{
	s_rs485_info.client_addr = addr;
}


#define rs485_get_addr() (s_rs485_info.client_addr)

void rs485_save_addr()
{
	strSystemParameter.BeepFlag = rs485_get_addr();
	sys_write_param();
}

BOOL rs485_check_addr(u8_t addr)
{
	BOOL bRet = FALSE;
	
	if (addr == s_rs485_info.client_addr || addr == 0x00) 
	{
		bRet = TRUE;
	}
	
	return (bRet);
}


/*
**¹¹Ôì·¢ËÍ°ü
*/

u32_t rs485_pack_head(u8_t *pSendBuf, u8_t cmd ,u8_t *pSendDat, u32_t len)
{
	rs485_pkt_t * ppack;
	u32_t ttl = 0;
	
	ppack = (rs485_pkt_t *)pSendBuf;
	ttl = len + RS485_LEN_OTHER ;
	
	ppack->head = RS485_BIT_HEAD;
	ppack->addr = rs485_get_addr();
	ppack->cmd = cmd;
	ppack->len[0] = (ttl >> 8)&0xff;
	ppack->len[1] = (ttl >> 0)&0xff;	
	
	return (RS485_LEN_OTHER+RS485_LEN_HEAD);
}

u32_t rs485_pack(u8_t *pSendBuf, u8_t cmd ,u8_t *pSendDat, u32_t len)
{
	rs485_pkt_t * ppack;
	u32_t ttl = 0;
	
	ppack = (rs485_pkt_t *)pSendBuf;
	ttl = len + RS485_LEN_OTHER ;
	
	rs485_pack_head(pSendBuf, cmd, pSendDat, len);
	
	memcpy(ppack->dat, pSendDat, len);
	
	//crc
	ppack->dat[len] = CheckSumEx(pSendBuf, ttl+RS485_LEN_HEAD);
	
	return (ttl+RS485_LEN_HEAD+RS485_LEN_CRC);
}


//void rs485_send(u8_t *buf, u32_t len)
//{
//	Rs485QuerySend(buf, len);
//}


//void makeRec()
//{
//	u8_t buf[16]={0};
//	static u8_t tag = 0;
//	
//  buf[1]++;
//	buf[2]--;
//	buf[3]+=3;
//	buf[4]+=5;
//	buf[5] = 0x70+ (tag++%3)+1;
//	buf[6] = tag*10/256;
//	buf[7] = tag*10;
//	buf[8] = (SysDate.daily<<3)|((SysTime.hour)>>2);
//	buf[9] = (((SysTime.hour)<<6)&0xFF)|((SysTime.minute));
//	buf[10] =  SysDate.month;
//	buf[11] =  SysDate.daily;
//	buf[12] =  SysTime.hour;
//	buf[13] =  SysTime.minute;
//	buf[14] =  SysTime.second;
//	
//	
//	
//	FlashQueue_EnQueue(&TagRecordFlashQueue, buf); 
//}


/* 
**½âÎö´¦ÀíÃüÁî   p3
*/
extern void rtc_set_flag(void);
static u8_t rs485_proc_cmd_p3(u8_t * pRecvDat)
{
	rs485_pkt_t * ppack;
	u8_t buf[512] = {0};
	u8_t dat[512] = {0};
	u32_t len = 0;
	u8_t ret = RS485_CMD_NULL;
	
	
	ppack = (rs485_pkt_t *)pRecvDat;
	
	ret = ppack->cmd;
	switch(ppack->cmd)
	{
		case RS485_CMD_ADDR:
				if (ppack->dat[0] == 0)  // get
				{
					len = rs485_pack(buf, RS485_CMD_ADDR, &ppack->addr, 1);
				}
				else if (ppack->dat[0] == 1)  //set
				{
					len = rs485_pack(buf, RS485_CMD_ADDR, &ppack->addr, 1);
					
					rs485_set_addr(ppack->dat[1]);
					rs485_save_addr();
					
				}
			break;
		
		case RS485_CMD_TIME:			
			rs485_set_pro(RS485_PRO_P3);
		
			SystemDate.RTC_Year = ppack->dat[0];
			SystemDate.RTC_Month = ppack->dat[1];
			SystemDate.RTC_Date = ppack->dat[2];
			
			SystemTime.RTC_Hours = ppack->dat[3];
			SystemTime.RTC_Minutes = ppack->dat[4];
			SystemTime.RTC_Seconds = ppack->dat[5];
			
			//rtc_set_flag();
			//RTC_SetTime();
		
			len = (ppack->len[0]<<8)|ppack->len[1];
			memcpy(dat, ppack->dat, len-2);
		  len = rs485_pack(buf, RS485_CMD_TIME, dat, len-2);
		
			g_bServerLoginSucFlag = TRUE;
			TimeFormatToInt();
		//makeRec();
		break;
		
		case RS485_CMD_DAT:
			rs485_set_pro(RS485_PRO_P3);
		
		//·¢ËÍÊý¾Ý
		len = rf433_get_rec(dat,RS485_MAX_RECORDS_PR0_3);
		len = rs485_pack(buf, RS485_CMD_DAT, dat, len);
		break;
		
		case RS485_CMD_CYPE:
			dat[0] = 0x01;
			len = rs485_pack(buf, RS485_CMD_CYPE, dat, 1);
		
		break;
		
		case RS485_CMD_VER:
			sprintf((char *)dat, VERSION_STR);
			len = rs485_pack(buf, RS485_CMD_VER, dat, sizeof(VERSION_STR));
			
			break;
		case RS485_CMD_RESET:
			SYS_RESTART();
			break;
		default:
			ret = RS485_CMD_NULL;
			break;
	
	}
	
	//send data
	if (len > 0)
	{
		Rs485QuerySend(buf, len);
	}
  
	//return cmd
	return ret;
}


/* 
**½âÎö´¦ÀíÃüÁî   p2
*/
static u8_t rs485_proc_cmd_p2(u8_t * pRecvDat)
{
	rs485_pkt_t * ppack;
	u8_t buf[64] = {0};
	u8_t dat[32] = {0};
	u32_t len = 0;
//	u32_t recv_len = 0;
	u8_t ret = RS485_CMD_NULL;
	
	
	ppack = (rs485_pkt_t *)pRecvDat;
	
	
//	recv_len = ((ppack->len[0]<<8)|ppack->len[1]);
	ret = ppack->cmd;
	
	switch(ret)
	{
		case RS485_CMD_ADDR:
			if (ppack->dat[0] == 0)  // get
			{
				len = rs485_pack(buf, RS485_CMD_ADDR, &ppack->addr, 1);
			}
			else if (ppack->dat[0] == 1)  //set
			{
				len = rs485_pack(buf, RS485_CMD_ADDR, &ppack->addr, 1);
				
				rs485_set_addr(ppack->dat[1]);
				rs485_save_addr();
				
			}
			break;
	
		case RS485_CMD_CYPE:
			dat[0] = 0x01;
			len = rs485_pack(buf, RS485_CMD_CYPE, dat, 1);
		  
		break;
		
		case RS485_CMD_VER:
			sprintf((char *)dat, VERSION_STR);
			len = rs485_pack(buf, RS485_CMD_VER, dat, sizeof(VERSION_STR));
		
			break;
		case RS485_CMD_RESET:
			SYS_RESTART();
			break;
		
		case RS485_CMD_TOKEN:
		
			rs485_set_sending();
		  
			sprintf((char *)dat, "OK");
			len = rs485_pack(buf, RS485_CMD_TOKEN, dat, 2);
			
		  rs485_set_pro(RS485_PRO_P2);
			break;
		
		case RS485_CMD_SEND:
			//Rs485UartPort.rcv_index = recv_len+4;
			//Rs485UartPort.rcv_len = recv_len+4;
			Rs485UartPort.rcv_all_flag=1; //½ÓÊÕÍê±Ï
			break;
		
		case RS485_CMD_TRANS:
			//Rs485UartPort.rcv_index = recv_len+4;
			//Rs485UartPort.rcv_len = recv_len+4;
			Rs485UartPort.rcv_all_flag=1; //½ÓÊÕÍê±Ï
			break;
		
		case RS485_CMD_END:
			
			rs485_clr_sending();
			break;
		default: ret = RS485_CMD_NULL;
	}

	
	//send data
	if (len > 0)
	{
		Rs485QuerySend(buf, len);
	}
  
	
	//return cmd
	return ret;
}
	

/*
**½âÎö´¦ÀíÃüÁî
*/
static u8_t rs485_proc_cmd(u8_t * pRecvDat)
{
	rs485_pkt_t * ppack;
	//u8_t buf[64] = {0};
	//u8_t dat[32] = {0};
	//u32_t len = 0;
	u8_t ret = 0;

	
	
	ppack = (rs485_pkt_t *)pRecvDat;
	
	
	// addr error only process token cmd
	if (!rs485_check_addr(ppack->addr))
	{
		if (ppack->cmd == RS485_CMD_TOKEN)
		{
			rs485_clr_sending();
			
		}
		
		return RS485_CMD_NULL;
	}
	
	
	ret = rs485_proc_cmd_p2(pRecvDat);
	if (ret != RS485_CMD_NULL)
	{
		return ret;
	}
	
	/******** ´¦Àí p3 Ð­Òé *********/
	ret = rs485_proc_cmd_p3(pRecvDat);
	if (ret != RS485_CMD_NULL)
	{
		return ret;
	}

	
	return ret;
}


/*
* rs485 ÖÐ¶Ï´¦Àí
*/
unsigned int Rs485Length = 0;
unsigned char  Rs485LengthBuffer[3];



BOOL rs485_proc_p1(u8_t *pDat, u32_t len)
{
	BOOL  bRet = FALSE;
	//u8_t checkSum = 0;
	u32_t tmp = 0;
	
	rs485_p1_stu_p pRecv = 0;
	
	pRecv = (rs485_p1_stu_p)pDat;
	
	//ÌØÊâÇé¿ö²»´¦Àí
	if ((g_stServerComCtrl.tx_rx_state != 1 && g_bStartUpgradeFlag != 1) ||
			(Rs485UartPort.rcv_all_flag == 1))
	{
			return bRet;
	}
		
	if (len > 4)
	{
		if (pRecv->head == 0x02)
		{
			
			tmp = (pRecv->len[0] << 8)| (pRecv->len[1] << 0);
			if (len >= tmp+3)
			{
				if (pDat[tmp+2] == 0x03)
				{
					Rs485UartPort.rcv_index = tmp+3;
					Rs485UartPort.rcv_len = tmp+3;
					Rs485UartPort.rcv_all_flag=1; //½ÓÊÕÍê±Ï
					DebugUartPort.g_bluetooth_flag = 0;
					bRet = TRUE;
				}
				else
				{

				}
			}
		}
	}
	
	return bRet;
}


void  rs485_proc_p2(u8_t *pDat, u32_t len)
{
	rs485_proc_cmd(pDat);

}



void rs485_irq_handle(u8_t *pDat, u32_t len)
{
	 //rs485_p1_stu_p pP1 = 0;  //µÚÒ»Ì×Ð­Òé 
	 //rs485_p2_stu_p pP2 = 0;  //µÚ¶þÌ×Ð­Òé 
	
	
		switch(rs485_get_pro())
		{
			case RS485_PRO_UNKNOW:
				rs485_proc_p2(pDat, len);
				if (rs485_get_pro() == RS485_PRO_UNKNOW)
				{
					if (rs485_proc_p1(pDat, len))
					{
						rs485_set_pro(RS485_PRO_P1);
					}
				}
				break;
			case RS485_PRO_P1:
				rs485_proc_p1(pDat, len);
				break;
			case RS485_PRO_P2:
				
//					break;
			case RS485_PRO_P3:
				rs485_proc_p2(pDat, len);
			
				Rs485UartPort.rcv_index = len;
				Rs485UartPort.rcv_len = len;
				break;
			default:break;
		}
			
		
		
}

void rs485_recv_ok(void)
{
	Rs485UartPort.rcv_index = 0;
	Rs485UartPort.rcv_all_flag = 0;
	Rs485UartPort.rcv_len = 0;
	Rs485UartPort.send_state = 0;
	
	uart_recv_reset();
}


void rs485_sendProcess(void)
{
	#if 1
	static unsigned short len;
	static unsigned int time_cnt;
//	uint8 head[2] = {0x03,0x03};
	
	switch(Rs485UartPort.send_state)
	{
		case 0:					//ÅäÖÃDMA¿ªÊ¼·¢ËÍ
			Send485En2(); 
	    	Delay_100us(10);
			len = (Rs485UartPort.send_len - Rs485UartPort.send_index);
			if(len > 1000)
				len = 1000;
		//	USART_DMACmd(RS485_UART,USART_DMAReq_Tx,ENABLE);   
			RS485_DMA_Stream->NDTR = len;			//Éè¶¨DMA³¤¶È
			RS485_DMA_Stream->M0AR = (uint32_t)(Rs485UartPort.send_buf)+Rs485UartPort.send_index;
			DMA_Cmd(RS485_DMA_Stream,ENABLE);
			Rs485UartPort.send_state = 1;
			
			break;
		case 1:					//µÈ´ý·¢ËÍÍê³É
			if(RS485_DMA_SendOK)
			{
				RS485_DMA_SendOK = 0;
				Rs485UartPort.send_index += len;
				if(Rs485UartPort.send_index >= Rs485UartPort.send_len)
				{
					Rs485UartPort.send_state = 0;
					Rs485UartPort.send_flag = 0;
					//Delay_100us(200);
				   
					//rs485_recv_ok();
					//Rs485QuerySend(head,1);
	        //Rev485En2();
				}
				else
				{
					Rs485UartPort.send_state = 2;
					time_cnt = SysTick10msCnt;
				}
			}
			break;
		case 2:					//ÑÓÊ±50ms
			if(( SysTick10msCnt - time_cnt) > 50)
			{
				Rs485UartPort.send_state = 0;
			
			}
			break;
		default:
			break;
	}
	#endif
}

/*
**  ´¦Àí 485½ÓÊÕ
*/
void rs485_proc_recv(void)
{
	u8_t * pDat = 0;
	u32_t  len = 0;
	
	if (uart_get_len())
	{

		pDat = uart_get_buf();
		len = uart_get_len();
	  
#if (EN_RS485_DEBUG)		
		PrintQuerySend(pDat,len);
#endif		
		
		rs485_irq_handle(pDat, len);
					
		uart_recv_reset();

	}
}


void rs485_SendStart(unsigned char *pBuf, unsigned short len)
{
	Rs485UartPort.send_state = 0;
	Rs485UartPort.send_len = len;
	Rs485UartPort.send_buf = pBuf;
	Rs485UartPort.send_flag = 1;
	Rs485UartPort.send_index = 0;
	Rs485UartPort.rcv_all_flag = 0;
	Rs485UartPort.rcv_index = 0;
	Rs485UartPort.rcv_state = 0;
   
	rs485_recv_ok();
	//printf(" Rs485 Send Start, Len = %d.\r\n",len);
	//PrintQuerySend(pBuf,len);
	//printf("\%s",pBuf);

}




/*
* rs485 ½ÓÊÕÊý¾ÝÕûÈÝ
*/
u32_t rs485_recv_pro(u8_t *pBuf, u32_t recv_len)
{
	u32_t tmp = 0;
	u32_t ret = 0;
	rs485_pkt_t *pp=0, *pTmp = 0;
	u32_t pkt_len = 0;
	u32_t tmp_len = 0;
	
	if (rs485_get_pro() == RS485_PRO_P2)
	{
		pp = (rs485_pkt_t *)pBuf;
		pkt_len = (pp->len[0]<<8)|pp->len[1];
		
		tmp = 4+pkt_len;    //add head len checksum (4bytes)
		
		while(tmp < recv_len)
		{
			//Printf("\r\n-1-\r\n");
			pTmp = (rs485_pkt_t *)&pBuf[tmp];
			
			tmp_len = (pTmp->len[0]<<8)|pTmp->len[1];
			
			memcpy(&pBuf[pkt_len+3], pTmp->dat, tmp_len-2);
			
			pkt_len += tmp_len-2;
			tmp += tmp_len+4;
		};
		
		pp->len[0] = (pkt_len>>8)&0xff;
		pp->len[1] = pkt_len&0xff;
		
		pBuf[pkt_len+3] = CheckSumEx(pBuf,pkt_len+3);
		
		ret = pkt_len + 4;
	}
	else
	{
		
		ret = recv_len;
	}
	
	
	return ret;
}


void rs485_init(void)
{
	//uart_rs485_init();
	uint8_t test_buf[3] = "123";
	
	rs485_SendStart(test_buf,3);

	rs485_set_pro(RS485_PRO_UNKNOW);
	
	uart_set_buf(Rs485UartPort.rcv_buf, TCP_MAX_RCV_LEN, &SysTick10msCnt, 50);
}


void rs485_CommTask(void)		//±¾ÈÎÎñÃ¿10msÖ´ÐÐÒ»´Î
{
	static unsigned short Rs485LastRcvLen;
	u32_t idx = 0;
	
	
	//p2 Ð­Òé ¼ì²é ·¢ËÍÁîÅÆ
	if ((!rs485_get_flag()) && (rs485_get_pro() == RS485_PRO_P2))
	{
		return;
	}
	
	
	if(Rs485UartPort.send_flag)
	{
		
		rs485_sendProcess();
		Rs485LastRcvLen = 0;
	}
  	else if(g_stServerComCtrl.tx_rx_state == 1 && g_bStartUpgradeFlag == 0)	//Rs485 ½ÓÊÕ
	{
		if((Rs485UartPort.rcv_all_flag == 1)&&(Rs485UartPort.rcv_index > Rs485LastRcvLen))
		{			
			g_iRevStartPos = Rs485UartPort.rcv_index;					
       		 
//			printf(g_arrRecvBuffer);
			if (rs485_get_pro() == RS485_PRO_P2)
			{
				idx = 5;
			}
			else
			{
				idx = 3;
			}
			
			if( memcmp(g_arrHttp500Error, &g_arrRecvBuffer[idx], 12) == 0)
			{
				printf("HTTP/1.1 500 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			else if( memcmp(g_arrHttp400Error, &g_arrRecvBuffer[idx], 12) == 0)
			{
				printf("HTTP/1.1 400 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			else if( memcmp(g_arrHttp20, &g_arrRecvBuffer[idx], 11) == 0)
			{
				
				g_iRevStartPos = rs485_recv_pro(g_arrRecvBuffer, g_iRevStartPos);
				
				g_bCommReceiveFlag = 1;
				//rs485_clr_sending();
			}
			else
			{
				printf("HTTP/1.1 200 ERROR\r\n");
				memset(g_arrRecvBuffer, 0, g_iRevStartPos);
				g_iRevStartPos = 0;
				g_bCommReceiveFlag = 0;
			}
			Rs485LastRcvLen = Rs485UartPort.rcv_index;
		}
	}

	
	
	// rs485 test
//	if (Rs485UartPort.rcv_all_flag == 1)
//	{
//		
//		Rs485QuerySend(Rs485UartPort.rcv_buf, Rs485UartPort.rcv_index);
//		Rs485UartPort.rcv_all_flag = 0;
//		Rs485UartPort.rcv_index = 0;
//	}

}


void rs485_sendUpgrade(u8_t *pBuf, u32_t len)
{
	u8_t  *tx_ptr = 0;
	
	Send485En2(); 
	Delay_100us(3);		
	pBuf[0]=0x02;
	len = len + 3;
	pBuf[1]=len>>8;
	pBuf[2]=len;	
	memcpy(g_arrSendBuffer,pBuf,3);
	tx_ptr = g_arrSendBuffer+3;
	FeedDog();
	memcpy(tx_ptr,pBuf,len);
	
	crc_ccitt_upgrade(&g_arrSendBuffer[len], &g_arrSendBuffer[3], len-3);
	g_arrSendBuffer[len+2]= 0x03;
	RS485_SendStart(g_arrSendBuffer, len+3);			//SOF+LEN(2)+

}
