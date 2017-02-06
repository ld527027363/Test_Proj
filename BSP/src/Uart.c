
#include "global.h"
#include "linklayer.h"
#include "uart.h"

STRUCT_UART_PARATERS DebugUartPort;
STRUCT_LONG_UART_PAR Rs485UartPort,WifiUartPort;
unsigned char WIFI_DMA_SendOK = 0;

#if USING_MCU405 
void USART1_Config(void)  //test ok
{
  	GPIO_InitTypeDef  GPIO_InitStructure;
  	USART_InitTypeDef USART_InitStructure;

  	RCC_AHB1PeriphClockCmd(UART1_GPIO_RCC,        ENABLE); 
  	RCC_APB2PeriphClockCmd(PRINT_APBPeriph_UART, ENABLE);
	
  	GPIO_PinAFConfig(UART1_GPIO, UART1_TX_PIN_SOURCE, GPIO_AF_USART1);  
  	GPIO_PinAFConfig(UART1_GPIO, UART1_RX_PIN_SOURCE, GPIO_AF_USART1);
	
  	GPIO_InitStructure.GPIO_Pin   = UART1_GPIO_TX | UART1_GPIO_RX;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
  	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  	GPIO_Init(UART1_GPIO, &GPIO_InitStructure);    
  	USART_InitStructure.USART_BaudRate   = UART1_BAUDRATE;//波特率设置
  	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
  	USART_InitStructure.USART_Parity     = USART_Parity_No;
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; 
  	USART_Init(USART1, &USART_InitStructure);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE); 
	USART_Cmd(USART1, ENABLE);
  	USART_ClearFlag(USART1, USART_FLAG_TC);  
}
#endif

#if USING_MCU406 
void USART1_Config(void)  //test ok
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	USART_ClockInitTypeDef USART_ClockInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);              
	GPIO_StructInit(&GPIO_InitStructure);      
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;     
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
    
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate =115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);
	USART_ClockStructInit(&USART_ClockInitStruct);   
	USART_ClockInitStruct.USART_CPOL = USART_CPOL_High;
	USART_ClockInit(USART3, &USART_ClockInitStruct);
	
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    
	USART_Cmd(USART3, ENABLE);     
}
#endif

void USART2_Config(void)  
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); 
	RCC_AHB1PeriphClockCmd(UART2_GPIO_RCC, ENABLE);  
	GPIO_PinAFConfig(UART2_GPIO, UART2_TX_PIN_SOURCE, GPIO_AF_USART2);//这相当于M3的开启复用时钟？只配置复用的引脚，
	GPIO_PinAFConfig(UART2_GPIO, UART2_RX_PIN_SOURCE, GPIO_AF_USART2);//               
	//配置GPIOD
	GPIO_InitStructure.GPIO_Pin   = UART2_GPIO_TX | UART2_GPIO_RX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_Init(UART2_GPIO, &GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate   = UART2_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);

	USART_Cmd(USART2, ENABLE);     
	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);     
}


void USART3_Config(void)  
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
	RCC_AHB1PeriphClockCmd(UART3_GPIO_RCC, ENABLE);  
	GPIO_PinAFConfig(UART3_GPIO, UART3_TX_PIN_SOURCE, GPIO_AF_USART3);//这相当于M3的开启复用时钟？只配置复用的引脚，
	GPIO_PinAFConfig(UART3_GPIO, UART3_RX_PIN_SOURCE, GPIO_AF_USART3);//               
	//配置GPIOD
	GPIO_InitStructure.GPIO_Pin   = UART3_GPIO_TX | UART3_GPIO_RX;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_Init(UART3_GPIO, &GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate   = UART3_BAUDRATE;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits   = USART_StopBits_1;
	USART_InitStructure.USART_Parity     = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);

	USART_Cmd(USART3, ENABLE);     
	USART_ClearFlag(USART3, USART_FLAG_TC);
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);     
}

void RS485AndWifiBufferConfig(void)
{
    Rs485UartPort.rcv_buf = g_arrRecvBuffer;
    Rs485UartPort.send_buf= g_arrSendBuffer;
    WifiUartPort.rcv_buf  = g_arrRecvBuffer;
    WifiUartPort.send_buf = g_arrSendBuffer;
}


void USART6_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APBPeriph_UART6, ENABLE); //开启USART6时钟
	RCC_AHB1PeriphClockCmd(UART6_GPIO_RCC, ENABLE);  //开启GPIOC时钟
	GPIO_PinAFConfig(UART6_GPIO, UART6_TX_PIN_SOURCE, GPIO_AF_USART6);//这相当于M3的开启复用时钟？只配置复用的引脚，    
  GPIO_PinAFConfig(UART6_GPIO, UART6_RX_PIN_SOURCE, GPIO_AF_USART6);	
	/*配置GPIOC*/
	GPIO_StructInit(&GPIO_InitStructure);      //缺省值填入
	
	/*配置GPIOC_Pin6为TX输出*/
	GPIO_InitStructure.GPIO_Pin   = UART6_GPIO_TX|UART6_GPIO_RX;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;     //设置为复用，必须为AF，OUT不行
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	//GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(UART6_GPIO,&GPIO_InitStructure);
	
	USART_StructInit(&USART_InitStructure);
	USART_InitStructure.USART_BaudRate      =  UART6_BAUDRATE;
	USART_InitStructure.USART_WordLength    =  USART_WordLength_8b;
	USART_InitStructure.USART_StopBits      =  USART_StopBits_1;
	USART_InitStructure.USART_Parity        =  USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART6, &USART_InitStructure);
	
	
	USART_Cmd(USART6, ENABLE);         //使能 USART6 
	USART_ClearFlag(USART6, USART_FLAG_TC);
	USART_ITConfig(USART6, USART_IT_RXNE, ENABLE);    //使能 USART6中断

}


void WifiDmaInit(void)  
{  
	DMA_InitTypeDef  DMA_InitStructure;  
	NVIC_InitTypeDef NVIC_InitStructure;
   
	RCC_AHB1PeriphClockCmd(WIFI_RCC_AHB1Periph_DMA, ENABLE);  
 
	DMA_InitStructure.DMA_Channel               = WIFI_DMA_Channel;
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (unsigned int)(&WIFI_UART->DR);  //外设地址  
	DMA_InitStructure.DMA_Memory0BaseAddr		= (unsigned int)(WifiUartPort.send_buf);  
	DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;   //外设作为目的地址   //DMA_DIR_PeripheralSRC;   //外设作为DMA的源端  
	DMA_InitStructure.DMA_BufferSize			= 9 * 1024; //BufferSize;      //传输大小  
	DMA_InitStructure.DMA_PeripheralInc			= DMA_PeripheralInc_Disable; //外设递增模式禁止   DMA_PeripheralInc_Enable;            //外设地址增加  
	DMA_InitStructure.DMA_MemoryInc			    = DMA_MemoryInc_Enable;   //内存地址自增  
	DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte; //传输方式：字节   DMA_PeripheralDataSize_Word;    //字（32位）  
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_PeripheralDataSize_Byte;  //内存存储方式：字节  DMA_MemoryDataSize_Word;  
	DMA_InitStructure.DMA_Mode				    = DMA_Mode_Normal;  //DMA_Mode_Normal 正常模式，只传送一次;  DMA_Mode_Circular:循环模式，不停的传送;  
	DMA_InitStructure.DMA_Priority				= DMA_Priority_Medium;  // 中等优先级
	DMA_InitStructure.DMA_FIFOMode			    = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold		    = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst			= DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst		= DMA_PeripheralBurst_Single;

	DMA_Init(WIFI_DMA_Stream,&DMA_InitStructure);
	
	// Enable DMA Channel4Transfer Complete interrupt  
	DMA_ITConfig(WIFI_DMA_Stream,DMA_IT_TC, ENABLE);  
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //嵌套优先级分组为 1
	NVIC_InitStructure.NVIC_IRQChannel                   = WIFI_DMA_Stream_IRQn; //嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 4;    //响应优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     //通道中断使能
	NVIC_Init(&NVIC_InitStructure);
	//  
	//USART_DMACmd(WIFI_UART,USART_DMAReq_Rx,ENABLE);                                                                    //串口接收器DMA  
	//采用DMA方式发送  
	USART_DMACmd(WIFI_UART,USART_DMAReq_Tx,ENABLE);   
	//传输完成则进入DMA2_Channel5中断；
}

void Rs485DmaInit(void)  
{  
	DMA_InitTypeDef  DMA_InitStructure;  
    NVIC_InitTypeDef NVIC_InitStructure;
   
	RCC_AHB1PeriphClockCmd(RS485_RCC_AHB1Periph_DMA, ENABLE);  
  // DMA channel6configuration  
	DMA_InitStructure.DMA_Channel = RS485_DMA_Channel;
	DMA_InitStructure.DMA_PeripheralBaseAddr	= (unsigned int)(&RS485_UART->DR);  //外设地址  
	DMA_InitStructure.DMA_Memory0BaseAddr		= (unsigned int)(Rs485UartPort.send_buf);  
	DMA_InitStructure.DMA_DIR                   = DMA_DIR_MemoryToPeripheral;   //外设作为目的地址   //DMA_DIR_PeripheralSRC;   //外设作为DMA的源端  
	DMA_InitStructure.DMA_BufferSize			= 9 * 1024; //BufferSize;      //传输大小  
	DMA_InitStructure.DMA_PeripheralInc			= DMA_PeripheralInc_Disable; //外设递增模式禁止   DMA_PeripheralInc_Enable;            //外设地址增加  
	DMA_InitStructure.DMA_MemoryInc			    = DMA_MemoryInc_Enable;   //内存地址自增  
	DMA_InitStructure.DMA_PeripheralDataSize    = DMA_PeripheralDataSize_Byte; //传输方式：字节   DMA_PeripheralDataSize_Word;    //字（32位）  
	DMA_InitStructure.DMA_MemoryDataSize		= DMA_PeripheralDataSize_Byte;  //内存存储方式：字节  DMA_MemoryDataSize_Word;  
	DMA_InitStructure.DMA_Mode				    = DMA_Mode_Normal;  //DMA_Mode_Normal 正常模式，只传送一次;  DMA_Mode_Circular:循环模式，不停的传送;  
	DMA_InitStructure.DMA_Priority				= DMA_Priority_Medium;  // 中等优先级
	DMA_InitStructure.DMA_FIFOMode			    = DMA_FIFOMode_Disable;
	DMA_InitStructure.DMA_FIFOThreshold		    = DMA_FIFOThreshold_Full;
	DMA_InitStructure.DMA_MemoryBurst			= DMA_MemoryBurst_Single;
	DMA_InitStructure.DMA_PeripheralBurst		= DMA_PeripheralBurst_Single;

	DMA_Init(RS485_DMA_Stream,&DMA_InitStructure);
	 
	DMA_ITConfig(RS485_DMA_Stream,DMA_IT_TC, ENABLE);  
	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  //嵌套优先级分组为 1
	NVIC_InitStructure.NVIC_IRQChannel = RS485_DMA_Stream_IRQn; //嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;    //响应优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     //通道中断使能
	NVIC_Init(&NVIC_InitStructure);
	//  
	//USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);      //串口接收器DMA  
	//采用DMA方式发送  
	USART_DMACmd(RS485_UART,USART_DMAReq_Tx,ENABLE);   
}

void USART_NVIC_Config()
{
	NVIC_InitTypeDef NVIC_InitStructure;

#if USING_MCU405
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel                   = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif  
    
//#if USING_MCU406
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel                   = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
//#endif  

	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  //嵌套优先级分组为 1
	NVIC_InitStructure.NVIC_IRQChannel                   = USART2_IRQn; //嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为 0			//串口优先级最高，保证不丢数据，但中断服务程序一定要短！！！！
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;    //响应优先级为 0		 // gaojun 2015-07-21
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     //通道中断使能
	NVIC_Init(&NVIC_InitStructure);


//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);  //嵌套优先级分组为 1
	NVIC_InitStructure.NVIC_IRQChannel                   = USART6_IRQn; //嵌套通道为USART6_IRQn
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //抢占优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 1;    //响应优先级为 0
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;     //通道中断使能
	NVIC_Init(&NVIC_InitStructure);

}

#define DEBUGMESSAGE 1
int fputc(int ch, FILE *f)
{
    #if DEBUGMESSAGE
        USART_SendData(PRINTF_UART, (u8) ch);
        while (USART_GetFlagStatus(PRINTF_UART, USART_FLAG_TC) == RESET);
    #endif
  	return ch;
}


void PrintQuerySend(unsigned char *buf,unsigned short len)
{
	uint16_t i;
	for(i = 0;i < len;i++)
	{
		USART_SendData(PRINTF_UART, buf[i]);
		while (USART_GetFlagStatus(PRINTF_UART, USART_FLAG_TXE) == RESET); 	
        	FeedDog();    
		
		//if (buf[i] == 0) break;
	}
}

void UartBlueSend(unsigned char *buf,unsigned short len)
{
	uint16_t i;
	for(i = 0;i < len;i++)
	{
		USART_SendData(BLUETOOTH_UART, buf[i]);
		while (USART_GetFlagStatus(BLUETOOTH_UART, USART_FLAG_TXE) == RESET); 	
        	FeedDog();    
		
		//if (buf[i] == 0) break;
	}
}

void DebugMessage(unsigned char *buf)
{
    unsigned short i=0, iLength=0;
    iLength = strlen((char *)buf);
    for(i=0; i<iLength; i++)
    {	
        USART_SendData(USART1, buf[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);           		
    }
}

void Rs485QuerySend(unsigned char *buf,unsigned short len)
{
	uint16_t i;
	Send485En2(); Delay_100us(10);
	for(i = 0;i < len;i++)
	{
		USART_SendData(RS485_UART, buf[i]);
		while (USART_GetFlagStatus(RS485_UART, USART_FLAG_TXE) == RESET); 		
	}	
	Delay_100us(10);
	Rev485En2();
}

void RS485SendStr(unsigned char *str)
{
	unsigned char i;
	Send485En2(); 
	Delay_100us(10);
	for(i = 0;str[i] != 0;i++)
	{
		if(str[i] == 0)
		{
			return;
        }
							
		USART_SendData(RS485_UART, str[i]); 
		while (USART_GetFlagStatus(RS485_UART, USART_FLAG_TXE) == RESET); 		
	}	
	Delay_100us(10);
	Rev485En2();
}


void WIFI_SendStr(char *str)
{
	unsigned char i;
	for(i = 0;i < 100;i++)
	{
		if(str[i] == 0)
		{
			return;
        }
							
		USART_SendData(WIFI_UART, str[i]); 
		while (USART_GetFlagStatus(WIFI_UART, USART_FLAG_TXE) == RESET); 		
	}	
}

void WifiQuerySend(unsigned char *buf,unsigned short len)
{
	uint16_t i;
	for(i = 0;i < len;i++)
	{
		USART_SendData(WIFI_UART, buf[i]);
		while (USART_GetFlagStatus(WIFI_UART, USART_FLAG_TXE) == RESET); 
        //if((i % 1024) == 0)	{ DelayMs(50); FeedDog(); }
	}	
}



void RS485_SendStart(unsigned char *pBuf, unsigned short len)
{
	Send485En2();
	//RS485_DMA_Stream->NDTR = len;			//设定DMA长度
	//DMA_Cmd(RS485_DMA_Stream,ENABLE);
	DisableIrq();
	Rs485QuerySend( Rs485UartPort.send_buf,len);
	EnableIrq();
}









