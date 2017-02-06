#include "spi.h"
#include "gpio_conf.h"

void SPI1_Config(void)			//FLASH
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); 
	RCC_AHB1PeriphClockCmd(SPI1_RCC_AHBPeriph_GPIO, ENABLE);  
	GPIO_PinAFConfig(SPI1_PORT, SPI1_Source1, GPIO_AF_SPI1);
	GPIO_PinAFConfig(SPI1_PORT, SPI1_Source2, GPIO_AF_SPI1);               
	GPIO_PinAFConfig(SPI1_PORT, SPI1_Source3, GPIO_AF_SPI1);   

	GPIO_StructInit(&GPIO_InitStructure);      //?????
	
	GPIO_InitStructure.GPIO_Pin  =  SPI1_PIN1 | SPI1_PIN2 | SPI1_PIN3;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;     //?????,???AF,OUT????
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//gaojun    2015-06-25
	GPIO_InitStructure.GPIO_Speed=  GPIO_Speed_100MHz;
	GPIO_Init(SPI1_PORT,&GPIO_InitStructure);

	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;//SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1,&SPI_InitStructure);

	SPI_Cmd(SPI1,ENABLE);
}

void SPI2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); 
	RCC_AHB1PeriphClockCmd(SPI2_RCC_AHBPeriph_GPIO, ENABLE);  //??GPIOA??------------------
	GPIO_PinAFConfig(SPI2_PORT, SPI2_Source1, GPIO_AF_SPI2);//????M3????????????????,
	GPIO_PinAFConfig(SPI2_PORT, SPI2_Source2, GPIO_AF_SPI2);//               
	GPIO_PinAFConfig(SPI2_PORT, SPI2_Source3, GPIO_AF_SPI2);//   
	/*??GPIOC*/
	GPIO_StructInit(&GPIO_InitStructure);      //?????

	/*??GPIOA_Pin9?TX??*/
	GPIO_InitStructure.GPIO_Pin=SPI2_PIN1|SPI2_PIN2|SPI2_PIN3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;     //?????,???AF,OUT??
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;					//gaojun    2015-06-25
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI2_PORT,&GPIO_InitStructure);

	//SPI_StructInit(&SPI_InitStructure);
	SPI_Cmd(SPI2,DISABLE);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;//SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;//SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2,&SPI_InitStructure);

	SPI_Cmd(SPI2,ENABLE);
}


void SPI3_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE); 
	RCC_AHB1PeriphClockCmd(SPI3_RCC_AHBPeriph_GPIO, ENABLE);  //??GPIOA??------------------
	GPIO_PinAFConfig(SPI3_PORT, SPI3_Source1, GPIO_AF_SPI3);//????M3????????????????,
	GPIO_PinAFConfig(SPI3_PORT, SPI3_Source2, GPIO_AF_SPI3);//               
	GPIO_PinAFConfig(SPI3_PORT, SPI3_Source3, GPIO_AF_SPI3);//   
	/*??GPIOC*/
	GPIO_StructInit(&GPIO_InitStructure);      //?????
	
	/*??GPIOA_Pin9?TX??*/
	GPIO_InitStructure.GPIO_Pin=SPI3_PIN1|SPI3_PIN2|SPI3_PIN3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF;     //?????,???AF,OUT?? 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//gaojun    2015-06-25
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 
	GPIO_Init(SPI3_PORT,&GPIO_InitStructure);

	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI3,&SPI_InitStructure);

	SPI_Cmd(SPI3,ENABLE);
}


uint8_t SPI1_WR(uint8_t dat)
{
	while((SPI1->SR & SPI_I2S_FLAG_TXE) == 0);
	SPI1->DR = dat;
	while((SPI1->SR & SPI_I2S_FLAG_RXNE) == 0);
	return SPI1->DR;
}


uint8_t SPI2_WR(uint8_t dat)
{
	
	while((SPI2->SR & SPI_I2S_FLAG_TXE) == 0);
	SPI2->DR = dat;
	while((SPI2->SR & SPI_I2S_FLAG_RXNE) == 0);
	return SPI2->DR;

}

uint8_t SPI3_WR(uint8_t dat)
{
	while((SPI3->SR & SPI_I2S_FLAG_TXE) == 0);
	SPI3->DR = dat;
	while((SPI3->SR & SPI_I2S_FLAG_RXNE) == 0);
	return SPI3->DR;

}

uint8_t SPI_WR(uint8_t dev,uint8_t dat)
{
	switch(dev)
	{
		case 1:
			return (SPI1_WR(dat));
		case 2:
			return (SPI2_WR(dat));
		case 3:
			return (SPI3_WR(dat));
		default:
			return 0xFF;
	}
}
