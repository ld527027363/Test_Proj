/********************************************************
 * File	       :  TcpServer.c
 * Description :  tcp服务端程序
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Update      :  2015-10-13
 * History     :

*/

#include "global.h"
#include "types.h"
#include "w5500.h"
//#include "socket.h"
#include "stdint.h"
#include "mcu_flash.h"
#include "SystemParameter.h"
#include "TcpServer.h"


#define SOCKET_TCP_SERVER             2
#define TCPSERVER_SEND_BUF_SIZE       1024  //1k
#define TCPSERVER_RECV_BUF_SIZE       2048  //2k

//static uint8 s_send_buf[TCPSERVER_SEND_BUF_SIZE];
//static uint8 s_recv_buf[TCPSERVER_SEND_BUF_SIZE];

//static uint32 TcpServer_recv_callback(uint8 **pDat, uint32 *dat_len);
//static uint32 TcpServer_recvProc_callback(uint8 *pDat, uint32 dat_len);
//static uint32 TcpServer_send_callback(SOCKET s, uint8 **pDat, uint32 *dat_len);
//static uint32 TcpServer_sendProc_callback(SOCKET s, uint8 *pDat, uint32 dat_len);

uint8 g_tcpServBuf[2048];				                              	         				/*定义一个2KB的缓存*/


//开启服务器
BOOL TcpServer_listen(SOCKET s, uint16 locPort)
{
	BOOL ret = FALSE;
	
	

		ret = W5500_Socket(s, Sn_MR_TCP, locPort, 0);	
				
		ret = W5500_SocketListen(Sn_MR_TCP);

	
	return ret;
}


uint8* GetTcpServBuf(void)
{
	return (uint8*)&(g_tcpServBuf[1]);
}


static uint8 CheckDataPackage(uint8 *p,int len)
{
	uint8 bSum;
//	uint8 len;

	if (*p != 0xAA)
		return 0;
	
	p++;
	//len = p[0];
	bSum = CheckSum(&p[1], len-1);
	if ( bSum != p[len] )
		return 0;
	
	return 1;
}

void TcpServSend(uint8 *pSend, int nSendLen)
{
	W5500_SocketSend(SOCKET_TCP_SERVER, pSend, nSendLen);
}



//static uint8 s_tcp_state = 0;


#define SERVER_TIMEOUT_CNT   300
//static uint32 s_idle_cnt = 0;

void tcp_server_loopback(void)
{	
	int len=0;  
	uint8 tcp_state;
	
	tcp_state = W5500_getSn_SR(SOCKET_TCP_SERVER);
	
	//socket_set_tcpState(SOCKET_TCP_SERVER, tcp_state);
/*	
	if (s_tcp_state == tcp_state
		&& tcp_state == SOCK_INIT)
	 {
		 
		 return;
	 }
	
	 s_tcp_state = tcp_state;
	*/ 
	switch(tcp_state)									            	/*获取socket的状态*/
	{
				case SOCK_CLOSED:														/*socket处于关闭状态*/
				{		
					W5500_Socket(SOCKET_TCP_SERVER ,Sn_MR_TCP,TCP_SERVER_PORT,0);	      /*打开socket*/
					//printf(" 34SOCK_CLOSED\r\n");
					//W5500_SocketListen(SOCKET_TCP_SERVER);
										
				  break;     
				}	
				case SOCK_INIT:														    /*socket已初始化状态*/
				{
					//W5500_Socket(SOCKET_TCP_SERVER ,Sn_MR_TCP,TCP_SERVER_PORT,0);	
					W5500_SocketListen(SOCKET_TCP_SERVER);
					//printf("40SOCK_INIT\r\n");
					//s_idle_cnt = 0;
					break;
				}	
				
				case SOCK_ESTABLISHED:													/*socket处于连接建立状态*/
				{
					
#if(1)
					  //  printf("SOCK_ESTABLISHED\r\n");
							if(W5500_getSn_IR(SOCKET_TCP_SERVER) & Sn_IR_CON)
							{
								W5500_setSn_IR(SOCKET_TCP_SERVER, Sn_IR_CON);	
							}
							len = W5500_getSn_RX_RSR(SOCKET_TCP_SERVER);						/*定义len为已接收数据的长度*/
							if(len > 0)
							{

								W5500_SocketRecv(SOCKET_TCP_SERVER,g_tcpServBuf,len);					/*接收来自Client的数据*/
//								buff[len]=0x00;											/*添加字符串结束符*/
								//PRINTF("%s\r\n",buff);
								
								//W5500_SocketSend(SOCK_TCPS,g_tcpServBuf,len);					/*向Server发送数据*/
//								choice_do_something(buff);
								if (CheckDataPackage(g_tcpServBuf, len-2) == 0)
								{
									printf("error");
									//W5500_SocketClose(SOCK_TCPS);
								}
								else
								{
									//s_idle_cnt = SysTick1sCnt;
								  DealSysParamLan();
								}
								 
							}
#endif
							break;
				}
				case SOCK_CLOSE_WAIT:													/*socket处于等待关闭状态*/
				{	
					 //W5500_SocketDiscon(SOCKET_TCP_SERVER);
						W5500_SocketClose(SOCKET_TCP_SERVER);
					  //W5500_SocketListen(2);
							//printf("62SOCK_CLOSE_WAIT\r\n");
					break;
				}
	}
	if (tcp_state >= SOCK_FIN_WAIT)
	{
		W5500_SocketClose(SOCKET_TCP_SERVER);
	}
}


static uint32 nDlTolLen = 0;  //下载总长
static uint32 nDlFinLen = 0;  //已下载总长
static int bDlStartFlag = 0;
static int nDlCnt = 0;

//开始下载 记录下载信息
int TcpDownloadStart(uint8  *pDat)
{
	nDlTolLen = pDat[4-1]<<24 | pDat[5-1]<<16 | pDat[6-1]<<8 | pDat[7-1];

	printf("%d %d", pDat[5-1],nDlTolLen);
	if ((nDlTolLen<80000) || (nDlTolLen > 200000))  //80k-200k
	{
		//文件长度错误
		nDlTolLen = 0;
		
		return 0;
	}
	else		
	{
		FeedDog();
		OwnDelSector();
		
		bDlStartFlag = 1;
	  nDlFinLen = 0;
		nDlCnt = 0;
		return 1;
	}
	
}
#define TEST_DOWNLOAD_FILE_SIZE             1400


//下载文件处理
int TcpDownloading(uint8  *pDat)
{
	uint32 nDlWriteLen = TEST_DOWNLOAD_FILE_SIZE;
	
	//检查下载文件标记
	if ((nDlCnt++ != pDat[0]) || (bDlStartFlag != 1))
	{
		bDlStartFlag = 0;
		return 0;
	}
	
	//检查是否文件尾部
	if ((nDlTolLen - nDlFinLen) < TEST_DOWNLOAD_FILE_SIZE)
	{
		nDlWriteLen = nDlTolLen - nDlFinLen;
	}
		
	FeedDog();
	if (OwnSaveUpgrade(nDlFinLen, &pDat[1], nDlWriteLen) != 1)
	{
		bDlStartFlag = 0;
		return 0;
	}		
	
	nDlFinLen += nDlWriteLen;
	
	return 1;
}

//判断是否接收完
int TcpDownloadEnd(uint8  *pDat)
{
	if (nDlFinLen == nDlTolLen)
	{	
		return 1;
	}
	else
	{
		return 0;
	}
}

void BootToUpgrade(void)
{
	WriteApplicationFlag(0x55);
	g_sysrestart_flag = 1;
}

#if 0

//发送回调
uint32 TcpServer_send_callback(SOCKET s, uint8 **pDat, uint32 *dat_len)
{
	uint32 ret = 1;
	
	//ret = dat_len;
	
	return ret;
}

//发送结束回调
uint32 TcpServer_sendProc_callback(SOCKET s, uint8 *pDat, uint32 dat_len)
{
	uint32 ret = 1;
	
	//ret = dat_len;
	
	return ret;
}


//接收回调
uint32 TcpServer_recv_callback(uint8 **pDat, uint32 *dat_len)
{
  uint32 ret = MAX_RECV_PACKET_LEN;
	
	*pDat = g_tcpServBuf;
	dat_len = &ret;
	
	return ret;
}


//接收处理回调
uint32 TcpServer_recvProc_callback(uint8 *pDat, uint32 dat_len)
{
	uint32 len = dat_len;
	uint32 ret = 0;
	
	if (CheckDataPackage(pDat, len-2) == 0)
	{
		printf("error");
		W5500_SocketClose(SOCKET_TCP_SERVER);
	}
	else
	{
		DealSysParamLan();	
	}		
	
	return ret;
}

#endif
