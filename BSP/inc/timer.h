#ifndef __TIME_H
#define __TIME_H

#include "stm32f4xx_rtc.h"

#define BACK_LINK_FREQ		320//160	
#define Fpclk				42000000

extern unsigned int SysTick10msCnt;
extern unsigned char SysTick10msFlag;
extern unsigned char SysTick100msFlag ;
extern unsigned int SysTick1sCnt;
extern unsigned char SysTick1sFlag;
extern unsigned char BeepOn;
extern unsigned short BeepDelayCnt;
extern unsigned char BeepDelayPeriod;

extern unsigned short Led2DelayCnt;
extern unsigned short Led3DelayCnt;
extern unsigned char Led2OnFlag;
extern unsigned char Led3OnFlag;

void Time7Init(void);
#endif




