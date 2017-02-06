#ifndef __GPIO_CONF_H__
#define __GPIO_CONF_H__


//#define USING_MCU407  1//选择407单片机
#define USING_MCU405  1//选择407单片机
//#define USING_MCU406  1//选择407单片机


#if USING_MCU405

///////////////////////////////////////??////////////////////////////////////////////
#define UART1_GPIO_TX		GPIO_Pin_6
#define UART1_TX_PIN_SOURCE GPIO_PinSource6
#define UART1_GPIO_RX		GPIO_Pin_7
#define UART1_RX_PIN_SOURCE GPIO_PinSource7
#define UART1_GPIO			GPIOB
#define UART1_GPIO_RCC      RCC_AHB1Periph_GPIOB
#define PRINT_APBPeriph_UART	RCC_APB2Periph_USART1
#define UART1_TX_DMA		DMA1_Stream1
#define UART1_RX_DMA		DMA1_Stream3

#define UART2_GPIO_TX	    GPIO_Pin_2
#define UART2_TX_PIN_SOURCE GPIO_PinSource2
#define UART2_GPIO_RX	    GPIO_Pin_3
#define UART2_RX_PIN_SOURCE GPIO_PinSource3
#define UART2_GPIO	    	GPIOA
#define UART2_GPIO_RCC   	RCC_AHB1Periph_GPIOA
#define RCC_APBPeriph_UART2	RCC_APB1Periph_USART2
#define UART2_TX_DMA		DMA1_Stream6
//#define UART2_RX_DMA		DMA1_Stream3

#define UART3_GPIO_TX	    GPIO_Pin_10
#define UART3_TX_PIN_SOURCE GPIO_PinSource10
#define UART3_GPIO_RX	    GPIO_Pin_11
#define UART3_RX_PIN_SOURCE GPIO_PinSource11
#define UART3_GPIO	    	GPIOB
#define UART3_GPIO_RCC   	RCC_AHB1Periph_GPIOB
#define RCC_APBPeriph_UART3	RCC_APB1Periph_USART3
#define UART3_TX_DMA		DMA1_Stream7



#define UART6_GPIO_TX		GPIO_Pin_6
#define UART6_TX_PIN_SOURCE GPIO_PinSource6
#define UART6_GPIO_RX		GPIO_Pin_7
#define UART6_RX_PIN_SOURCE GPIO_PinSource7
#define UART6_GPIO			GPIOC
#define UART6_GPIO_RCC   	RCC_AHB1Periph_GPIOC
#define RCC_APBPeriph_UART6	RCC_APB2Periph_USART6


#define RS485_CONTROL      GPIO_Pin_8
#define RS485_CONTROL_PORT GPIOC


#define PRINTF_IRQHandler   USART1_IRQHandler
#define PRINTF_UART         USART1
//#define DebugUartPort    Uart1
//#define PRINTF_SendStr   

#define RS485_IRQHandler   USART6_IRQHandler
#define RS485_UART         USART6
//#define Rs485UartPort    Uart2
//#define RS485_SendStr  

#define WIFI_IRQHandler   USART2_IRQHandler
#define WIFI_UART         USART2
//#define WifiUartPort    Uart2
//#define WIFI_SendStr  

#define BLUETOOTH_IRQHandler   USART3_IRQHandler
#define BLUETOOTH_UART         USART3

#define RS485_RCC_AHB1Periph_DMA        RCC_AHB1Periph_DMA2
#define RS485_DMA_Channel               DMA_Channel_5
#define RS485_DMA_Stream                DMA2_Stream6
#define RS485_DMA_Stream_IRQn           DMA2_Stream6_IRQn
#define RS485_DMA_Stream_IRQHandler     DMA2_Stream6_IRQHandler
#define RS485_DMA_IT_TCIF               DMA_IT_TCIF6
#define RS485_DMA_FLAG_TCIF             DMA_FLAG_TCIF6

#define WIFI_RCC_AHB1Periph_DMA         RCC_AHB1Periph_DMA1
#define WIFI_DMA_Channel                DMA_Channel_4
#define WIFI_DMA_Stream                 DMA1_Stream6
#define WIFI_DMA_Stream_IRQn            DMA1_Stream6_IRQn
#define WIFI_DMA_Stream_IRQHandler      DMA1_Stream6_IRQHandler
#define WIFI_DMA_IT_TCIF                DMA_IT_TCIF6
#define WIFI_DMA_FLAG_TCIF              DMA_FLAG_TCIF6    

//////////////////////////////////////////////////////////////////////////////////

#define BATTRY_ADC        					GPIO_Pin_5
#define BATTRY_ADC_POWER_PORT 		  GPIOC
#define BATTRY_ADC_RCC_AHB					RCC_AHB1Periph_GPIOC
#define BATTRY_ADC_CHANNEL					ADC_Channel_15 

#define POWER_ADC        						GPIO_Pin_4
#define POWER_ADC_POWER_PORT  			GPIOC
#define POWER_ADC_RCC_AHB						RCC_AHB1Periph_GPIOC
#define POWER_ADC_CHANNEL						ADC_Channel_14

#define HARDWARE_H_ADC 							GPIO_Pin_0
#define HARDWARE_H_ADC_POWER_PORT  	GPIOB
#define HARDWARE_H_ADC_RCC_AHB			RCC_AHB1Periph_GPIOB
#define HARDWARE_H_ADC_CHANNEL			ADC_Channel_8

#define HARDWARE_L_ADC 							GPIO_Pin_1
#define HARDWARE_L_ADC_POWER_PORT  	GPIOB
#define HARDWARE_L_ADC_RCC_AHB			RCC_AHB1Periph_GPIOB
#define HARDWARE_L_ADC_CHANNEL			ADC_Channel_9

////////////////////////////////////////////////////////////////////////////////////

#define RF433_POWER        GPIO_Pin_8
#define RF433_POWER_PORT   GPIOA

#define MCU_LED1           GPIO_Pin_8
#define MCU_LED2           GPIO_Pin_5
#define MCU_LED3           GPIO_Pin_4
#define MCU_LED_PORT       GPIOB

////////////////////////////////////// ????//////////////////////////////////////
#define FLASH_CS_PIN	   GPIO_Pin_4
#define FLASH_CS_PORT	   GPIOA

#define BUZZER_ON          GPIO_Pin_0
#define BUZZER_ON_PORT	   GPIOC

#define RF433_NSEL         GPIO_Pin_12
#define RF433_NSEL_PORT	   GPIOB

#define RF433_IRQ          GPIO_Pin_9
#define RF433_IRQ_PORT	   GPIOC

#define WIFI_NRST          GPIO_Pin_0
#define WIFI_NRST_PORT	   GPIOA

#define WIFI_REBLOAD       GPIO_Pin_3
#define WIFI_REBLOAD_PORT  GPIOC

#define WIFI_LINK          GPIO_Pin_2
#define WIFI_LINK_PORT	   GPIOC

#define WIFI_READY         GPIO_Pin_1
#define WIFI_READY_PORT	   GPIOA
//////////////////////////////////////  ??	////////////////////////////////////////
#define W5500_CS_PORT	   GPIOB
#define W5500_CS_PIN	   GPIO_Pin_3

#define W5500_RST_PORT	   GPIOA
#define W5500_RST_PIN	   GPIO_Pin_15

#define W5500_ENET_INTN             GPIO_Pin_2
#define W5500_ENET_INTN_PORT        GPIOD
#define W5500_ENET_INTN_Source      EXTI_PortSourceGPIOD
#define W5500_ENET_INTN_PinSource   EXTI_PinSource2
#define W5500_ENET_INTN_EXTI_Line   EXTI_Line2
#define W5500_ENET_INTN_EXTI_IRQn   EXTI2_IRQn
#define W5500_ENET_INTN_EXTI_IRQHandler   EXTI2_IRQHandler



///////////////////////////////SPI //////////////////////////////
#define W5500_SPI                   SPI3_WR
#define RF433_SPI                   SPI2_WR
#define FLASH_SPI                   SPI1_WR

#define SPI1_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOA
#define SPI1_PORT                  GPIOA
#define SPI1_Source1               GPIO_PinSource5
#define SPI1_Source2               GPIO_PinSource6
#define SPI1_Source3               GPIO_PinSource7
#define SPI1_PIN1                  GPIO_Pin_5                
#define SPI1_PIN2                  GPIO_Pin_6  
#define SPI1_PIN3                  GPIO_Pin_7  

#define SPI2_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOB
#define SPI2_PORT                  GPIOB
#define SPI2_Source1               GPIO_PinSource13
#define SPI2_Source2               GPIO_PinSource14
#define SPI2_Source3               GPIO_PinSource15
#define SPI2_PIN1                  GPIO_Pin_13               
#define SPI2_PIN2                  GPIO_Pin_14  
#define SPI2_PIN3                  GPIO_Pin_15 

#define SPI3_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOC
#define SPI3_PORT                  GPIOC
#define SPI3_Source1               GPIO_PinSource10
#define SPI3_Source2               GPIO_PinSource11
#define SPI3_Source3               GPIO_PinSource12
#define SPI3_PIN1                  GPIO_Pin_10               
#define SPI3_PIN2                  GPIO_Pin_11  
#define SPI3_PIN3                  GPIO_Pin_12 

#define POWER_DETECT               GPIO_Pin_11
#define POWER_DETECT_PORT          GPIOA

#endif

#if USING_MCU406

#define BOARDVersion "0406"
///////////////////////////////////////??////////////////////////////////////////////
#define UART1_GPIO_TX		GPIO_Pin_10
#define UART1_TX_PIN_SOURCE GPIO_PinSource10
#define UART1_GPIO_RX		GPIO_Pin_11
#define UART1_RX_PIN_SOURCE GPIO_PinSource11
#define UART1_GPIO			GPIOB
#define UART1_GPIO_RCC      RCC_AHB1Periph_GPIOB
#define PRINT_APBPeriph_UART	RCC_APB1Periph_USART3
//#define UART1_TX_DMA		DMA1_Stream1
//#define UART1_RX_DMA		DMA1_Stream3

#define UART2_GPIO_TX	    GPIO_Pin_2
#define UART2_TX_PIN_SOURCE GPIO_PinSource2
#define UART2_GPIO_RX	    GPIO_Pin_3
#define UART2_RX_PIN_SOURCE GPIO_PinSource3
#define UART2_GPIO	    	GPIOA
#define UART2_GPIO_RCC   	RCC_AHB1Periph_GPIOA
#define RCC_APBPeriph_UART2	RCC_APB1Periph_USART2
#define UART2_TX_DMA		DMA1_Stream6
//#define UART2_RX_DMA		DMA1_Stream3

#define UART6_GPIO_TX		GPIO_Pin_6
#define UART6_TX_PIN_SOURCE GPIO_PinSource6
#define UART6_GPIO_RX		GPIO_Pin_7
#define UART6_RX_PIN_SOURCE GPIO_PinSource7
#define UART6_GPIO			GPIOC
#define UART6_GPIO_RCC   	RCC_AHB1Periph_GPIOC
#define RCC_APBPeriph_UART6	RCC_APB2Periph_USART6


#define RS485_CONTROL      GPIO_Pin_8
#define RS485_CONTROL_PORT GPIOC


#define PRINTF_IRQHandler   USART3_IRQHandler
#define PRINTF_UART         USART3
//#define DebugUartPort    Uart1
//#define PRINTF_SendStr   

#define RS485_IRQHandler   USART6_IRQHandler
#define RS485_UART         USART6
//#define Rs485UartPort    Uart2
//#define RS485_SendStr  

#define WIFI_IRQHandler   USART2_IRQHandler
#define WIFI_UART         USART2
//#define WifiUartPort    Uart2
//#define WIFI_SendStr  

#define RS485_RCC_AHB1Periph_DMA        RCC_AHB1Periph_DMA2
#define RS485_DMA_Channel               DMA_Channel_5
#define RS485_DMA_Stream                DMA2_Stream6
#define RS485_DMA_Stream_IRQn           DMA2_Stream6_IRQn
#define RS485_DMA_Stream_IRQHandler     DMA2_Stream6_IRQHandler
#define RS485_DMA_IT_TCIF               DMA_IT_TCIF6
#define RS485_DMA_FLAG_TCIF             DMA_FLAG_TCIF6

#define WIFI_RCC_AHB1Periph_DMA         RCC_AHB1Periph_DMA1
#define WIFI_DMA_Channel                DMA_Channel_4
#define WIFI_DMA_Stream                 DMA1_Stream6
#define WIFI_DMA_Stream_IRQn            DMA1_Stream6_IRQn
#define WIFI_DMA_Stream_IRQHandler      DMA1_Stream6_IRQHandler
#define WIFI_DMA_IT_TCIF                DMA_IT_TCIF6
#define WIFI_DMA_FLAG_TCIF              DMA_FLAG_TCIF6    

#define RF433_POWER        GPIO_Pin_8
#define RF433_POWER_PORT   GPIOA

#define MCU_LED1           GPIO_Pin_8
#define MCU_LED2           GPIO_Pin_5
#define MCU_LED3           GPIO_Pin_4
#define MCU_LED_PORT       GPIOB

////////////////////////////////////// ????//////////////////////////////////////
#define FLASH_CS_PIN	   GPIO_Pin_4
#define FLASH_CS_PORT	   GPIOA

#define BUZZER_ON          GPIO_Pin_0
#define BUZZER_ON_PORT	   GPIOC

#define RF433_NSEL         GPIO_Pin_12
#define RF433_NSEL_PORT	   GPIOB

#define RF433_IRQ          GPIO_Pin_9
#define RF433_IRQ_PORT	   GPIOC

#define WIFI_NRST          GPIO_Pin_0
#define WIFI_NRST_PORT	   GPIOA

#define WIFI_REBLOAD       GPIO_Pin_3
#define WIFI_REBLOAD_PORT  GPIOC

#define WIFI_LINK          GPIO_Pin_2
#define WIFI_LINK_PORT	   GPIOC

#define WIFI_READY         GPIO_Pin_1
#define WIFI_READY_PORT	   GPIOA
//////////////////////////////////////  ??	////////////////////////////////////////
#define W5500_CS_PORT	   GPIOB
#define W5500_CS_PIN	   GPIO_Pin_3

#define W5500_RST_PORT	   GPIOA
#define W5500_RST_PIN	   GPIO_Pin_15

#define W5500_ENET_INTN             GPIO_Pin_2
#define W5500_ENET_INTN_PORT        GPIOD
#define W5500_ENET_INTN_Source      EXTI_PortSourceGPIOD
#define W5500_ENET_INTN_PinSource   EXTI_PinSource2
#define W5500_ENET_INTN_EXTI_Line   EXTI_Line2
#define W5500_ENET_INTN_EXTI_IRQn   EXTI2_IRQn
#define W5500_ENET_INTN_EXTI_IRQHandler   EXTI2_IRQHandler



///////////////////////////////SPI //////////////////////////////
#define W5500_SPI                   SPI3_WR
#define RF433_SPI                   SPI2_WR
#define FLASH_SPI                   SPI1_WR

#define SPI1_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOA
#define SPI1_PORT                  GPIOA
#define SPI1_Source1               GPIO_PinSource5
#define SPI1_Source2               GPIO_PinSource6
#define SPI1_Source3               GPIO_PinSource7
#define SPI1_PIN1                  GPIO_Pin_5                
#define SPI1_PIN2                  GPIO_Pin_6  
#define SPI1_PIN3                  GPIO_Pin_7  

#define SPI2_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOB
#define SPI2_PORT                  GPIOB
#define SPI2_Source1               GPIO_PinSource13
#define SPI2_Source2               GPIO_PinSource14
#define SPI2_Source3               GPIO_PinSource15
#define SPI2_PIN1                  GPIO_Pin_13               
#define SPI2_PIN2                  GPIO_Pin_14  
#define SPI2_PIN3                  GPIO_Pin_15 

#define SPI3_RCC_AHBPeriph_GPIO    RCC_AHB1Periph_GPIOC
#define SPI3_PORT                  GPIOC
#define SPI3_Source1               GPIO_PinSource10
#define SPI3_Source2               GPIO_PinSource11
#define SPI3_Source3               GPIO_PinSource12
#define SPI3_PIN1                  GPIO_Pin_10               
#define SPI3_PIN2                  GPIO_Pin_11  
#define SPI3_PIN3                  GPIO_Pin_12 

#define POWER_DETECT               GPIO_Pin_11
#define POWER_DETECT_PORT          GPIOA

#endif
//////////////////////////////////////////////////////////////////////////////////////////

#define SET_W5500_CS()		W5500_CS_PORT->BSRRL = W5500_CS_PIN
#define CLR_W5500_CS()		W5500_CS_PORT->BSRRH = W5500_CS_PIN

#define SET_W5500_RST()		W5500_RST_PORT->BSRRL = W5500_RST_PIN
#define CLR_W5500_RST()		W5500_RST_PORT->BSRRH = W5500_RST_PIN
///////////////////////////////////////////////////////////////////////////////////////
#define Send485En2()  GPIO_SetBits(  RS485_CONTROL_PORT,RS485_CONTROL); //发送
#define Rev485En2()   GPIO_ResetBits(RS485_CONTROL_PORT,RS485_CONTROL);

#define SET_BEEP()    BUZZER_ON_PORT->BSRRL = BUZZER_ON
#define CLR_BEEP()    BUZZER_ON_PORT->BSRRH = BUZZER_ON

#define SET_LED1()     MCU_LED_PORT->BSRRH   = MCU_LED1
#define CLR_LED1()     MCU_LED_PORT->BSRRL   = MCU_LED1

#define SET_LED2()     MCU_LED_PORT->BSRRH   = MCU_LED2
#define CLR_LED2()     MCU_LED_PORT->BSRRL   = MCU_LED2

#define SET_LED3()     MCU_LED_PORT->BSRRH   = MCU_LED3
#define CLR_LED3()     MCU_LED_PORT->BSRRL   = MCU_LED3
//////////////////////////////////////////////////////////////

#define DisableIrq()		__disable_irq()
#define EnableIrq()		    __enable_irq()

#define Read_Power_DetectPin()   GPIO_ReadInputDataBit(POWER_DETECT_PORT,POWER_DETECT)
#define GET_WIFI_LINK_STATUS()	 GPIO_ReadInputDataBit(WIFI_LINK_PORT,WIFI_LINK)
#define GET_WIFI_READY()         GPIO_ReadInputDataBit(WIFI_READY_PORT,WIFI_READY)

void NormalGpioConfig(void);
void StartBeep(unsigned int time,unsigned char flag);
void StartLed2(void);
void StartLed3(void);

#endif


