#ifndef __ADC_H
#define __ADC_H	

#include "gpio_conf.h" 
#include "stm32f4xx.h" 
#include "stm32f4xx_adc.h" 

void Adc_Init(void); 	
unsigned short Get_Adc(unsigned char ch); 			
unsigned short Get_Adc_Average(unsigned char ch,unsigned char times);


void STM32_ADC_Init(uint16_t GPIO_Num,GPIO_TypeDef *PORT_Num,uint32_t RCC_ABH_Num,uint8_t CH_Num);

void HardWare_AdcInit(void);


#endif 















