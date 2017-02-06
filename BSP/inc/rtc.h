#ifndef __RTCRTC_h
#define __RTCRTC_h

#include "stm32f4xx_rtc.h"

void RTC_Config(void);
void SetDateAndTime(char *DateBuff,char *TimeBuff);
void SetDefaultTime(void);

void GetRecordDate(unsigned char *date_str,unsigned char *tim_src);
void GetRecordTime(unsigned char *time_str,unsigned char *tim_src);

unsigned char CheckRecordDate(unsigned char *tim_src);
unsigned char CheckRecordTime(unsigned char *tim_src);
void TimeFormatToInt(void);

void GetRecordDateOld(char *date_str,unsigned char *tim_src);
void GetRecordTimeOld(char *time_str,unsigned char *tim_src);

extern RTC_TimeTypeDef SystemTime;
extern RTC_DateTypeDef SystemDate;
extern unsigned int SystemTimeSimple;		//32Î»Ê±¼äÂë

extern u32_t rtc_get_curTime(char * pTime, char * format);


#endif

