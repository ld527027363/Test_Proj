/*!
 * File:
 *  radio_hal.c
 *
 * Description:
 *  This file contains RADIO HAL.
 *
 * Silicon Laboratories Confidential
 * Copyright 2011 Silicon Laboratories, Inc.
 */


#include "si446x_bsp.h"
#include "gpio_conf.h"
#include "stm32f4xx.h"
#include "spi.h"

uint32_t RF433_SPI_IRQ()		
{
	return(GPIO_ReadInputDataBit(RF433_IRQ_PORT,RF433_IRQ));
}

/***************************************************************************
·µ»O1 ±iE?O?O?¶ICeCo
****************************************************************************/
uint8_t Rf433IrqDet(void)
{
	return(GPIO_ReadInputDataBit(RF433_IRQ_PORT,RF433_IRQ));
}

void RF_NSEL(uint8_t b)
{
	if(b)
	{
		GPIO_SetBits(  RF433_NSEL_PORT,RF433_NSEL);
	}
	else
	{
		GPIO_ResetBits(RF433_NSEL_PORT,RF433_NSEL);
	}
}

void radio_hal_AssertShutdown(void)
{
    GPIO_SetBits(RF433_POWER_PORT,RF433_POWER);
}

void radio_hal_DeassertShutdown(void)
{
    GPIO_ResetBits(RF433_POWER_PORT,RF433_POWER);
}


void radio_hal_ClearNsel(void)
{
	RF_NSEL(0);
}

void radio_hal_SetNsel(void)
{
	RF_NSEL(1);
}


bit radio_hal_NirqLevel(void)
{
	U8 return_data = 0;
	return_data = RF433_SPI_IRQ();
	return return_data;
}


void radio_hal_SpiWriteByte(U8 byteToWrite)
{
	RF433_SPI(byteToWrite);
}

U8 radio_hal_SpiReadByte(void)
{
	return RF433_SPI(0xff);//0xFF
}

void radio_hal_SpiWriteData(U8 byteCount, U8* pData)
{
	U8 i;
	for(i = 0;i < byteCount;i++)
	{
		RF433_SPI(pData[i]);
	}
}

void radio_hal_SpiReadData(U8 byteCount, U8* pData)
{
	U8 i;
	for(i = 0;i < byteCount;i++)
	{
		pData[i] = RF433_SPI(0xff);//0XFF
	}
}



