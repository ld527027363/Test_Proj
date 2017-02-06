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

extern unsigned char  g_bServerUploadReq;			//�����ϴ����󣬼��ʱ�䳬��1�����������ݣ��򻺴泬��10���������ϴ�
extern unsigned short g_iServerUploadCommTimeCnt;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
extern unsigned short g_iServerHeartCommTimeCnt;		//���ݷ��ͼ����ʱ��1���Ӽ�1��
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

