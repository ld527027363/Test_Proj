/********************************************************
 * File	       :  net.c
 * Description :  ��̫�����ܽӿ�
 * Author      :  �γ�ƽ
 * copyright   :  (2015)�����а��ĿۿۿƼ����޹�˾
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/

#include "global.h"
#include "w5500.h"
#include "TcpServer.h"
#include "net.h"




void net_send(u8_t * pDat, u32_t len)
{
	TcpServSend(pDat, len);
	
}




