#include "mcu_conf.h"
#include "stm32f4xx_conf.h"
#include "si446x_api_lib.h"
#include "types.h"
#include "buffer.h"

//void assert_failed(uint8* file, uint32 line)
//{
//	while (1);
//}

void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x20000000 */
	NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x08000000 */
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x20000);
#endif
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);	
}

void InitWatchdog(unsigned short iWatch)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); 
	IWDG_SetPrescaler(IWDG_Prescaler_256);  
	IWDG_SetReload(iWatch);  
	IWDG_Enable();  
	IWDG_ReloadCounter();         
}

void FeedDog(void)
{
	IWDG_ReloadCounter();
}

void EXTI_Config(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);//
 
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource9);    //407使用的配置函数
	EXTI_InitStructure.EXTI_Line = EXTI_Line9;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 
	
 //	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//修改为第二高优先级，第一高让给WIFI或485串口。gaojun 2015-07-21
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
}

#if 0
void MCU_Initial(void)
{
	NVIC_Configuration();	
	NormalGpioConfig();
	InitWatchdog(2000);
	USART1_Config();
	USART_NVIC_Config();
	USART6_Config(); 
	SPI2_Config();  //FLASH
	RTC_Config();
	SetDefaultTime();

	Time7Init();
	USART2_Config();
	Rev485En2();
	Rs485DmaInit(); 
	WifiDmaInit(); 
	RS485AndWifiBufferConfig();
	FeedDog();
	ParameterInitial();
	SPI1_Config(); //433M模块
	Flash_Init();
//	Flash_ChipErase();
	
	BufQueueInit(&TagDataQueue, TagBuf,TAG_RECORD_BUF_SIZE, TAG_RECORD_LEN);
	
	Flash_TagRecordQueueInit();
	
	printf("\r\nReader Initial......\r\n\r\n");
	Delay_100us(1000);
	
	vRadio_Init();	
	si446x_part_info();	
	//vRadio_StartRX(strSystemParameter.RFRxChannel,RF_RX_LEN);
	
	vRadio_StartRX(RF_FREQ_RX_CHANNEL,RF_RX_LEN);
	printf("\r\nReader Success......\r\n\r\n");   
}
#endif
