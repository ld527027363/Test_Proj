#ifndef __MCUCONF_H
#define __MCUCONF_H


void NVIC_Configuration(void);
void FeedDog(void);
void InitWatchdog(unsigned short iWatch);
void EXTI_Config(void);
void MCU_Initial(void);
#endif



