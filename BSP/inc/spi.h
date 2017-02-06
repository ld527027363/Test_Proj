#ifndef __SPISPI_H
#define __SPISPI_H

#include "stm32f4xx_conf.h"

void SPI1_Config(void);
void SPI2_Config(void);
void SPI3_Config(void);
uint8_t SPI1_WR(uint8_t dat);
uint8_t SPI2_WR(uint8_t dat);
uint8_t SPI3_WR(uint8_t dat);

#endif

