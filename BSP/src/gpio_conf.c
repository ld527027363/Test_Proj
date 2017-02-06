#include "global.h"
#include "gpio_conf.h"
#include "timer.h"

void NormalGpioConfig(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;

	GPIO_InitStructure.GPIO_Pin = RS485_CONTROL; //´®¿Ú6 485¿ØÖÆ½Å
	GPIO_Init(RS485_CONTROL_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = RF433_POWER; 
	GPIO_Init(RF433_POWER_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = MCU_LED1 | MCU_LED2 | MCU_LED3; 
	GPIO_Init(MCU_LED_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = FLASH_CS_PIN; 
	GPIO_Init(FLASH_CS_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = BUZZER_ON; //·äÃùÆ÷
	GPIO_Init(BUZZER_ON_PORT, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = RF433_NSEL;
	GPIO_Init(RF433_NSEL_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = WIFI_NRST; 
	GPIO_Init(WIFI_NRST_PORT, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin = WIFI_REBLOAD; 
	GPIO_Init(WIFI_REBLOAD_PORT, &GPIO_InitStructure);  
    
    GPIO_InitStructure.GPIO_Pin   = W5500_CS_PIN;
	GPIO_Init(W5500_CS_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin   = W5500_RST_PIN;
	GPIO_Init(W5500_RST_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;//GPIO_PuPd_UP;
	
	GPIO_InitStructure.GPIO_Pin = RF433_IRQ;//GPIO_Pin_4; //433M FINT
	GPIO_Init(RF433_IRQ_PORT, &GPIO_InitStructure);	
    
	GPIO_InitStructure.GPIO_Pin = WIFI_LINK;
	GPIO_Init(WIFI_LINK_PORT, &GPIO_InitStructure);	
    
	GPIO_InitStructure.GPIO_Pin = WIFI_READY;
	GPIO_Init(WIFI_READY_PORT, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    
	GPIO_InitStructure.GPIO_Pin  = W5500_ENET_INTN;
	GPIO_Init(W5500_ENET_INTN_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = POWER_DETECT;
	GPIO_Init(POWER_DETECT_PORT, &GPIO_InitStructure);
}


void StartBeep(unsigned int time,unsigned char flag)
{
    if(flag == 0);
	else SET_BEEP();
	BeepOn=1;
	BeepDelayCnt=0;
	BeepDelayPeriod=time;	
}

void StartLed2(void)
{
	SET_LED2();
	Led2OnFlag = 1;
	Led2DelayCnt = 0;
}

void StartLed3(void)
{
	SET_LED3();
	Led3OnFlag=1;
	Led3DelayCnt=0;
}







