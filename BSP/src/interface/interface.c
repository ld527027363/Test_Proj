/********************************************************
 * File	       :  interface.c
 * Description :  通用传输接口 485 LAN WIFI
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Creat       :  2015-11-20
 * Update      :  
 * History     :
*/

#include "types.h"

#include "wifi.h"
#include "uart.h"
#include "ble.h"
#include "net.h"
#include "rs485.h"
#include "TcpServer.h"


#include "interface.h"


//系统接口信息
static intf_profile_t s_intf[SYS_INTF_CNT+1] = {0};

//接口列表信息
static intf_list_t s_intf_list[INTF_TYPE_MAX] = {0};


static u32_t s_intf_cnt = 0;
HANDLE g_intf_cur = 0;

//handle 记录
HANDLE g_handle_list[SYS_INTF_CNT+1] = {0};

intf_profile_p g_intf_p = 0;


#define intf_set_cur(hdl) do{g_intf_cur = hdl;}while(0);

#define intf_check_type(type) ((type>INTF_TYPE_NULL&&type<INTF_TYPE_MAX)?TRUE:FALSE)

/*
* 接口 检查handle
*/
BOOL check_handle(HANDLE hdl, BOOL bSetCur) 
{
	BOOL ret = FALSE;
	
	if(hdl<=SYS_INTF_CNT)
	{
		if (bSetCur)
		{
			intf_set_cur(hdl);
		}
		
		ret = TRUE;
	}
	
	return ret;
}

/*
* 接口初始化
*/
void intf_init(void)
{
	u32_t i = 0;
	
	for (i=0 ;i<SYS_INTF_CNT; i++)
	{
		s_intf[SYS_INTF_CNT].state = INTF_INIT;
	}
	
	g_intf_p = s_intf;
	
	uart1_init();
}

/*
* 增加接口  返回接口ID
*/
void intf_add_type(intf_type_em_t intf_type, __intf_send send_callback, __intf_recv recv_callback)
{
	
	if (intf_check_type(intf_type))
	{
		s_intf_list[intf_type].send = send_callback;
		s_intf_list[intf_type].recv = recv_callback;
		s_intf_list[intf_type].type = intf_type;
	}
	
}


/*
* 增加接口  返回接口ID
*/
HANDLE intf_add(intf_type_em_t intf_type,intf_flag_em_t intf_flag, __intf_send send, __intf_recv recv_callback, __intf_check check)
{
	HANDLE ret = 0;
	
	if (intf_check_type(intf_type))
	{
		s_intf_list[intf_type].send = send;
		s_intf_list[intf_type].recv = recv_callback;
		s_intf_list[intf_type].check = check;
		s_intf_list[intf_type].type = intf_type;
		
		if (s_intf_cnt < SYS_INTF_CNT)
		{
			s_intf_cnt++; //从1开始
		
			intf_get_profile(s_intf_cnt)->intf = &s_intf_list[intf_type];
			intf_get_profile(s_intf_cnt)->state = INTF_INIT;
		
			g_handle_list[intf_flag] = s_intf_cnt;
		}
	
		ret = s_intf_cnt;
	}

	return ret;
}


/*
* 接口发送
*/
u32_t intf_send(HANDLE hdl, u8_t * pDat, u32_t len)
{
	u32_t ret = 0;
	
	if (check_handle(hdl, TRUE))
	{
		intf_get_profile(hdl)->state = INTF_SEND;
		ret = intf_get_profile(hdl)->intf->send(pDat, len);
	}
	
	return ret;
}


/*
* 接口接收
*/
u32_t intf_recv(HANDLE hdl, u8_t * pBuf, u32_t len)
{
	u32_t ret = 0;
	
	if (check_handle(hdl, TRUE))
	{
		intf_get_profile(hdl)->state = INTF_RECV;
		ret = intf_get_profile(hdl)->intf->recv(pBuf, len);
	}
	
	return ret;
}


///*
//* 接口获取接口类型
//*/

//intf_type_em_t intf_get_type(HANDLE hdl)
//{
//	intf_type_em_t ret = INTF_TYPE_NULL;
//	
//	if (check_handle(hdl))
//	{
//		ret = s_intf[hdl].intf->type;
//	}
//	
//	return ret;
//}


/*
* 接口获取当前操作接口
*/
//HANDLE intf_get_cur(void)
//{
//	HANDLE ret = 0;
//	
//	return ret;
//}



