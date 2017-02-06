#ifndef __LINKLAYER_H
#define __LINKLAYER_H

#include "protocol_typedef.h"
#include "types.h"


void ProtocolInitial(void);

void LinkPrintPortRecv(void);

void ServerCommStateInit(void);
void Server_CommTask(void);
void Server_RcvDataHandle(void);

extern unsigned char g_arrRecvBuffer[];
extern unsigned char g_arrSendBuffer[];
extern unsigned short g_iRevStartPos;

extern PProtocol_ST Pprotocol;
extern SERVER_COMM_CTRL g_stServerComCtrl;

extern unsigned char  g_bServerUploadReq;			//数据上传请求，间隔时间超过1分钟且有数据，或缓存超过10条将触发上传
extern unsigned short g_iServerUploadCommTimeCnt;		//数据发送间隔计时，1秒钟加1。
extern unsigned short g_iServerHeartCommTimeCnt;		//数据发送间隔计时，1秒钟加1。
extern volatile  unsigned char  g_bServerHeartbeatReq;
extern volatile unsigned char  g_bServerHeartbeatSucReq;

extern unsigned char  g_bServerLoginSucFlag;

extern unsigned char  g_bStartUpgradeFlag;
extern BOOL  g_bUploadOkFlag;

extern SERVER_REPLY_CODE ServerReplyCode;
volatile extern unsigned char  g_bCommReceiveFlag;

extern const unsigned char g_arrHttp500Error[];
extern const unsigned char g_arrHttp400Error[];
extern const unsigned char g_arrHttp20[];

#endif

