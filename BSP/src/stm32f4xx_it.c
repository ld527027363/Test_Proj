
/* Includes ------------------------------------------------------------------*/
#include "global.h"
#include "stm32f4xx_it.h"
#include "linklayer.h"
#include "interface.h"
#include "debug.h"
#include "rs485.h"
#include "testmode.h"

void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{}


/******************************************************************************/
/*            STM32F4xx Peripherals Interrupt Handlers                        */
/******************************************************************************/
void PRINTF_IRQHandler(void)
{
	uint8_t rcv_byte=0, bSum=0; 
	
	if (USART_GetITStatus(PRINTF_UART, USART_IT_RXNE) != RESET) //判断为接收中断
	{ 
		rcv_byte = USART_ReceiveData(PRINTF_UART); 
		//USART_SendData(USART6, rcv_byte); 	
		//USART_SendData(WIFI_UART, rcv_byte); 	
		
	//	intf_get_profile(intf_get_handle(INTF_FLAG_UART1))->intf->check(&rcv_byte, 1);
		 
		switch(DebugUartPort.rcv_state)
		{
			case 0x00: //判断帧头
				if(rcv_byte==0xAA) //收到帧头
				{
					DebugUartPort.rcv_state =1;
					DebugUartPort.rcv_len=0;
				}
				
				break;
			case 0x01://长度
				if(rcv_byte == 0)
				{
					DebugUartPort.rcv_state =1;
					DebugUartPort.rcv_len=0;      
				}
				DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = rcv_byte;
				DebugUartPort.rcv_state=2; //开始接收数据
				break;
			case 0x02: //数据
				if(DebugUartPort.rcv_len < (DebugUartPort.rcv_buf[0]))
				{
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = rcv_byte;
				}
				else
				{
					bSum = CheckSum(&DebugUartPort.rcv_buf[1], DebugUartPort.rcv_buf[0]-1);
					if(bSum == rcv_byte)
					{
						DebugUartPort.rcv_all_flag=1; //接收完毕
						DebugUartPort.g_bluetooth_flag = 0;
					}
					else
					{
						DebugUartPort.rcv_len=0;
						DebugUartPort.rcv_all_flag=0; //接收完毕
						DebugUartPort.rcv_state=0;
						
					}
				}			  
				break;
			default:
				DebugUartPort.rcv_state=0;
				DebugUartPort.rcv_all_flag =0;				
				break;
			
		}
		
		USART_ClearITPendingBit(PRINTF_UART,USART_IT_RXNE); 
	}     
}

void BLUETOOTH_IRQHandler(void)
{
	uint8_t rcv_byte=0, bSum=0; 
	uint32 len = 0;
	
	if (USART_GetITStatus(BLUETOOTH_UART, USART_IT_RXNE) != RESET) //判断为接收中断
	{ 
		rcv_byte = USART_ReceiveData(BLUETOOTH_UART); 
		
		//USART_SendData(PRINTF_UART, rcv_byte); 	
		
		switch(DebugUartPort.rcv_state)
		{
			case 0x00: //判断帧头
				if(rcv_byte==0x55) //收到帧头
				{
					DebugUartPort.rcv_state =3;
					DebugUartPort.rcv_len=0;
				}	
				break;
			case 0x03: //tag				
					DebugUartPort.rcv_state =1;
					DebugUartPort.rcv_len=0;
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = rcv_byte;
				break;
			case 0x01://长度
				DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = rcv_byte;
				
				if (DebugUartPort.rcv_len == 3)
				{
					if (((DebugUartPort.rcv_buf[1] << 8) | DebugUartPort.rcv_buf[2]) < 200)
					{
						DebugUartPort.rcv_state=2; //开始接收数据
					}
					else
					{
					  DebugUartPort.rcv_state=0;	
					}
				}
				break;
			case 0x02: //数据
				len = (DebugUartPort.rcv_buf[1] << 8) | DebugUartPort.rcv_buf[2];
				if(DebugUartPort.rcv_len < len+2)
				{
					DebugUartPort.rcv_buf[DebugUartPort.rcv_len++] = rcv_byte;
				}
				else if (DebugUartPort.rcv_len == len+2)
				{						
					bSum = CheckSum(&DebugUartPort.rcv_buf[3], len-1);
					if(bSum == rcv_byte || len == 1)
					{
						DebugUartPort.rcv_all_flag=1; //接收完毕
						DebugUartPort.g_bluetooth_flag = 1;

					}
					else
					{
						DebugUartPort.rcv_len=0;
						DebugUartPort.rcv_all_flag=0; 
					  DebugUartPort.rcv_state=0;							
					}
				}			  
				break;
			default:
				DebugUartPort.rcv_state=0;
				DebugUartPort.rcv_all_flag =0;				
				break;
			
		}
		USART_ClearITPendingBit(BLUETOOTH_UART,USART_IT_RXNE); 
	}     
}

//unsigned int Rs485Length = 0;
//unsigned char  Rs485LengthBuffer[3];

void RS485_IRQHandler(void)
{
	uint8_t rcv_byte;
	//unsigned char bTemp[2];
  

	if (USART_GetITStatus(RS485_UART, USART_IT_RXNE) != RESET) //判断为接收中断
	{ 
		
		rcv_byte = USART_ReceiveData(RS485_UART); 
		
		uart_set_dat(rcv_byte);
		Sys_TestMode_RecvData(rcv_byte);
		USART_ClearITPendingBit(RS485_UART,USART_IT_RXNE); 

#if 0		
		if ((g_stServerComCtrl.tx_rx_state != 1) ||
			(Rs485UartPort.rcv_all_flag == 1)
		)
		{
			return ;
		}
		
		
		
		
		switch(Rs485UartPort.rcv_state)
		{
			case 0x00: //判断帧头
				if(rcv_byte == 0x02) //收到帧头
				{
					Rs485UartPort.rcv_state =1;
					Rs485UartPort.rcv_len=0;
					Rs485UartPort.rcv_buf[Rs485UartPort.rcv_len++]=rcv_byte;
					//printf("485 head \n\r");
					
				}
				break;
			case 0x01://长度
				Rs485UartPort.rcv_buf[Rs485UartPort.rcv_len++] = rcv_byte;
				//Rs485LengthBuffer[Rs485UartPort.rcv_len++]=rcv_byte;
				if(Rs485UartPort.rcv_len == 3) //长度接收完毕
				{
					  
						Rs485UartPort.rcv_state = 2; //开始接收数据
				//	Rs485UartPort.rcv_len=0;
						Rs485Length = (Rs485UartPort.rcv_buf[1]<<8) + Rs485UartPort.rcv_buf[2];
					  if (Rs485Length >= TCP_MAX_RCV_LEN || Rs485Length > 3)	
						{
							Rs485UartPort.rcv_len=0;
							Rs485UartPort.rcv_all_flag=0; //接收完毕
							Rs485UartPort.rcv_state=0x00;	

							//printf("\r\n len error %d \r\n",Rs485Length);
						}	
						else
						{
							StartBeep(5,1);//上电鸣叫
						}
						//FeedDog();
						
						
				   // printf("len: %d \n\r", Rs485Length);
				}
				break;
			case 0x02: //数据
				if(Rs485UartPort.rcv_len == Rs485Length+2) //收到帧尾
				{
					if (rcv_byte == 0x03)
					{
						Rs485UartPort.rcv_buf[Rs485UartPort.rcv_len++]=rcv_byte;
						//printf("485 recv ok \r\n");
						Rs485UartPort.rcv_state=0x00;
						Rs485UartPort.rcv_index = Rs485UartPort.rcv_len;
						Rs485UartPort.rcv_all_flag=1; //接收完毕
						
					   
					}
					else
					{
						Rs485UartPort.rcv_state=0x00;
						Rs485UartPort.rcv_all_flag =0;
						//printf("\r\n crc error 2\r\n");
					}
/*					
					crc_ccitt_old(bTemp, &Rs485UartPort.rcv_buf[1], Rs485Length-3);
					if((bTemp[0] == Rs485UartPort.rcv_buf[Rs485Length-2]) && (bTemp[1] == Rs485UartPort.rcv_buf[Rs485Length-1]))
					{
						Rs485UartPort.rcv_index = Rs485UartPort.rcv_len;
						Rs485UartPort.rcv_all_flag=1; //接收完毕
						//printf("485 recv ok \r\n");
					}
					else
					{
						printf("crc error\r\n");
					}
*/			
				}
				else
				{
					Rs485UartPort.rcv_buf[Rs485UartPort.rcv_len] = rcv_byte;
					Rs485UartPort.rcv_len++;	
					if(Rs485UartPort.rcv_len>=TCP_MAX_RCV_LEN)	
					{
						Rs485UartPort.rcv_len=0;
						Rs485UartPort.rcv_all_flag=0; //接收完毕
						Rs485UartPort.rcv_state=0x00;			
						//printf("\r\n crc error 3\r\n");
					}						
				}			  
				break;
			default:
				Rs485UartPort.rcv_len=0;
				Rs485UartPort.rcv_state=0x00;
				Rs485UartPort.rcv_all_flag =0;	
					//printf("\r\n crc error 1\r\n");
				break;
			
		}
#endif      
	
	}	
}

extern unsigned char RS485_DMA_SendOK ;

void RS485_DMA_Stream_IRQHandler(void)
{
	if(DMA_GetITStatus(RS485_DMA_Stream,RS485_DMA_IT_TCIF) != RESET)   
	{  
		DMA_ClearFlag(RS485_DMA_Stream,RS485_DMA_FLAG_TCIF);
		DMA_Cmd(RS485_DMA_Stream,DISABLE);
		Delay_100us(3);
		Rev485En2();
		RS485_DMA_SendOK = 1;
	}
}

void WIFI_DMA_Stream_IRQHandler(void)
{
	if(DMA_GetITStatus(WIFI_DMA_Stream,WIFI_DMA_IT_TCIF) != RESET)   
	{  
		DMA_ClearFlag(WIFI_DMA_Stream,WIFI_DMA_FLAG_TCIF);
		DMA_Cmd(WIFI_DMA_Stream,DISABLE);
		WIFI_DMA_SendOK = 1;
	}
}
