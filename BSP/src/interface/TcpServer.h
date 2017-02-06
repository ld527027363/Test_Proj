#ifndef __TCPSERVER_H__
#define __TCPSERVER_H__
#include "types.h"


#define TCP_SERVER_PORT				 				5432


void tcp_server_loopback(void);//TCP Server回环演示函数


extern uint8* GetTcpServBuf(void);

extern void TcpServSend(uint8 *pSend, int nSendLen);

extern int TcpDownloadStart(uint8  *pDat);
extern int TcpDownloading(uint8  *pDat);
extern int TcpDownloadEnd(uint8  *pDat);
extern void BootToUpgrade(void);

//开启服务器
extern BOOL TcpServer_listen(SOCKET s, uint16 locPort);
#endif 

