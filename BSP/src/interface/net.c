/********************************************************
 * File	       :  net.c
 * Description :  以太网功能接口
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
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




