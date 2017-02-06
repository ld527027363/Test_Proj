/********************************************************
 * File	       :  uart1.c
 * Description :  UART 1接口功能
 * Author      :  何超平
 * copyright   :  (2015)深圳市爱的扣扣科技有限公司
 * Creat       :  2015-11-20
 * Update      :  
 * History     :

*/

#include "global.h"
#include "interface.h"
#include "uart.h"
#include "debug.h"
#include "uart1.h"



/*
** 清接收标志
*/
void uart1_clear_flag()
{
	//DebugUartPort.rcv_len -= DebugUartPort.last_recvLen;
	//DebugUartPort.last_recvLen=0;
	//DebugUartPort.rcv_state=0;
	
	DebugUartPort.rcv_all_flag = 0; //接收错误
	
	if (DebugUartPort.overlap_cnt <= 1)
	{
		DebugUartPort.rcv_len = 0;
	}
	
	
	DebugUartPort.overlap_cnt--;
}


/*
** 初始化
*/
void uart1_init(void)
{
	uart1_clear_flag();
}


/*
** 发送
*/
u32_t uart1_send(u8_t *pDat, u32_t len)
{
	u32_t i;
	u32_t timeout = 0;
	
	for(i = 0;i < len;i++)
	{
		USART_SendData(PRINTF_UART, pDat[i]);
		timeout = 50000;
		while (USART_GetFlagStatus(PRINTF_UART, USART_FLAG_TXE) == RESET && timeout-- != 0)
		{
			if (timeout%5000 == 0)
       FeedDog();    
		}
	}
	
	return i;
}


/*
** 接收回调
*/
u32_t uart1_recv_callback(const u8_t *pBuf, u32_t len)
{
	
	if (pBuf == 0 && len == 0)
	{
		
	}
	else
	{
		
	}
	
	uart1_clear_flag();	
	
	return len;
}


/*
** 接收验证
*/

 BOOL  uart1_check(u8_t *pDat, u32_t len)
{
	debug_pack_p ppack = 0;
	u8_t byte = 0;
	BOOL bRet = FALSE;
	
	if (len > 0)
	{
		byte = pDat[0];
		ppack = (debug_pack_p)&DebugUartPort.rcv_buf[DebugUartPort.last_index];
		
		switch(DebugUartPort.rcv_state)
		{
			case 0x00: //判断帧头
				if(debug_check_head(byte)) //收到帧头
				{
					DebugUartPort.last_index = DebugUartPort.rcv_len;
					bRet = TRUE;
					DebugUartPort.rcv_state =1;
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = byte;
					DebugUartPort.last_recvLen++;
					
					///DebugUartPort.rcv_len=0;
				}	
				break;
				
			case 0x01://长度
				if(byte != 0)
				{
					//DebugUartPort.rcv_state =1;
					//DebugUartPort.rcv_len=0; 
					bRet = TRUE;					
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = byte;
					DebugUartPort.rcv_state=2; //开始接收数据
					DebugUartPort.last_recvLen++;
				}
		
				break;
				
			case 0x02: //数据
				if(DebugUartPort.last_recvLen < ppack->len + DEBUG_HEAD_LEN)
				{
					bRet = TRUE;
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = byte;
					DebugUartPort.last_recvLen++;
				}
				
			  if (DebugUartPort.last_recvLen == ppack->len + DEBUG_HEAD_LEN-PKT_CHECKSUM_LEN)
				{
//bSum = CheckSum(&DebugUartPort.rcv_buf[1], DebugUartPort.rcv_buf[0]-1);
//					if(bSum == rcv_byte)
					{
						//DebugUartPort.rcv_all_flag=1; //接收完毕
						DebugUartPort.g_bluetooth_flag = 0;
						DebugUartPort.last_recvLen = 0;
						DebugUartPort.overlap_cnt++;
						DebugUartPort.rcv_state=0;
						debug_post_msg(intf_get_handle(INTF_FLAG_UART1), &DebugUartPort.rcv_buf[DebugUartPort.last_index]);
					}
	//					else
	//					{
	//						DebugUartPort.rcv_len=0;
	//						DebugUartPort.rcv_all_flag=0; //接收错误
	//						DebugUartPort.rcv_state=0;
	//						
	//					}
				}	
				break;
			default:
				DebugUartPort.rcv_state=0;
				DebugUartPort.rcv_all_flag =0;				
				break;
			
		}
	}
	
	return bRet;
}


