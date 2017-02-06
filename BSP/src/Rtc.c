
#include "global.h"
#include "rtc.h"


RTC_TimeTypeDef SystemTime;
RTC_DateTypeDef SystemDate;

unsigned int SystemTimeSimple = 0;
/*
void RTC_Config(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
    RCC_LSEConfig(RCC_LSE_ON);
    while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    RTC_WaitForSynchro();
    RCC_RTCCLKCmd(ENABLE);
}
*/
void RTC_Config(void)
{
	RTC_InitTypeDef RTC_InitStructure;
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR,ENABLE);
	PWR_BackupAccessCmd(ENABLE);//使能备份寄存器操作
  
	RCC_LSEConfig(RCC_LSE_ON);
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	RTC_WaitForSynchro();
  
	if(RTC_ReadBackupRegister(RTC_BKP_DR0) != 0x9527)   //一个变量，看看RTC初始化没
	{
    
		RTC_WriteProtectionCmd(DISABLE);
		
		RTC_EnterInitMode();
		RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
		RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
		RTC_InitStructure.RTC_SynchPrediv = 0xFF;
		RTC_Init(&RTC_InitStructure);

		RTC_TimeStructure.RTC_Seconds = 0x00;
		RTC_TimeStructure.RTC_Minutes = 0x01;
		RTC_TimeStructure.RTC_Hours = 0x01;
		RTC_TimeStructure.RTC_H12 = RTC_H12_AM;
		RTC_SetTime(RTC_Format_BCD,&RTC_TimeStructure);

		RTC_DateStructure.RTC_Date = 1;
		RTC_DateStructure.RTC_Month = 1;
		RTC_DateStructure.RTC_WeekDay= 4;
		RTC_DateStructure.RTC_Year = 15;
		RTC_SetDate(RTC_Format_BCD,&RTC_DateStructure);

		RTC_ExitInitMode();
		RTC_WriteBackupRegister(RTC_BKP_DR0,0X9527);
		RTC_WriteProtectionCmd(ENABLE);
		RTC_WriteBackupRegister(RTC_BKP_DR0,0x9527);  //初始化完成，设置标志
	}
//	PWR_BackupAccessCmd(DISABLE);
}
void SetDefaultTime(void)
{
/*
    RTC_InitTypeDef RTC_InitStructure;
    if(RTC_ReadBackupRegister(RTC_BKP_DR0) == 0xA5A5) return;
    
    RTC_InitStructure.RTC_AsynchPrediv = 0x7F; 
    RTC_InitStructure.RTC_SynchPrediv = 0XFF;
    RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24; 
	RTC_Init(&RTC_InitStructure);
	
	SystemDate.RTC_Year = 15;
	SystemDate.RTC_Month = 1;
	SystemDate.RTC_Date = 1;
	SystemDate.RTC_WeekDay = 4;

	SystemTime.RTC_Hours = 1;
	SystemTime.RTC_Minutes = 10;
	SystemTime.RTC_Seconds = 0;
	RTC_SetDate(RTC_Format_BIN, &SystemDate); 
	Delay_ms(1);  
	RTC_SetTime(RTC_Format_BIN, &SystemTime);
    RTC_WriteBackupRegister(RTC_BKP_DR0,0xA5A5);
    */
}

void SetDateAndTime(char *DateBuff,char *TimeBuff)
{
//	RTC_InitTypeDef RTC_InitStructure;	
	RTC_TimeTypeDef RTC_TimeStructure;
	RTC_DateTypeDef RTC_DateStructure;
	unsigned char TempData=0;
	
	TempData=DateBuff[2]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(DateBuff[3]-0x30);	
	RTC_DateStructure.RTC_Year = TempData; 

	TempData=DateBuff[5]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(DateBuff[6]-0x30);	
	RTC_DateStructure.RTC_Month = TempData; 

	TempData=DateBuff[8]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(DateBuff[9]-0x30);		
	RTC_DateStructure.RTC_Date = TempData; 
	RTC_DateStructure.RTC_WeekDay = 0x04; 
	
	//10:31:12
	TempData=TimeBuff[0]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(TimeBuff[1]-0x30);		
	RTC_TimeStructure.RTC_Hours = TempData; 

	TempData=TimeBuff[3]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(TimeBuff[4]-0x30);	
	RTC_TimeStructure.RTC_Minutes = TempData; 
	
	TempData=TimeBuff[6]-0x30;
	TempData=TempData<<4;
	TempData=TempData|(TimeBuff[7]-0x30);		
	RTC_TimeStructure.RTC_Seconds = TempData; 
	
	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
	RTC_SetDate(RTC_Format_BCD, &RTC_DateStructure);
	
}

void TimeFormatToInt(void)
{
	SystemTimeSimple = SystemDate.RTC_Year;
	SystemTimeSimple <<= 26;
	SystemTimeSimple += ((SystemDate.RTC_Month)<<22);
	SystemTimeSimple += ((SystemDate.RTC_Date)<<17);
	SystemTimeSimple += ((SystemTime.RTC_Hours)<<12);
	SystemTimeSimple += ((SystemTime.RTC_Minutes)<<6);
	SystemTimeSimple += (SystemTime.RTC_Seconds);
}

void GetRecordDate(unsigned char *date_str,unsigned char *tim_src)
{
	unsigned short reduce_tim;
	uint8 day = 0;

	RTC_DateTypeDef rtc_date;	

	reduce_tim = tim_src[0]*256 + tim_src[1];

	rtc_date.RTC_Month = tim_src[2];
	day = tim_src[3];
	rtc_date.RTC_Year = SystemDate.RTC_Year;
	
	rtc_date.RTC_Date = reduce_tim>>11;	//date		
	
	if(day < rtc_date.RTC_Date)		
  {
		if (rtc_date.RTC_Month == 1)
		{
			rtc_date.RTC_Month = 12;
		}
		else
		{
			rtc_date.RTC_Month = rtc_date.RTC_Month-1;
		}
	}
	
	if (rtc_date.RTC_Month > SystemDate.RTC_Month)
	{
		rtc_date.RTC_Year = SystemDate.RTC_Year -1;
	}
	
	
//  	if(rtc_date.RTC_Date > SystemDate.RTC_Date)		
//  	{
//  		if(SystemDate.RTC_Month == 1)
//  		{
//  			rtc_date.RTC_Month = 12;
//			rtc_date.RTC_Year = SystemDate.RTC_Year -1;
//  		}
//		else
//		{
//			rtc_date.RTC_Month = SystemDate.RTC_Month -1;
//			rtc_date.RTC_Year = SystemDate.RTC_Year;
//		}
//  	}
//	else
//	{
//		rtc_date.RTC_Month = SystemDate.RTC_Month;
//		rtc_date.RTC_Year = SystemDate.RTC_Year;		
//	}
    
    reduce_tim = (unsigned short)(rtc_date.RTC_Year)<<10;
    reduce_tim = reduce_tim + (unsigned short)(rtc_date.RTC_Month<<5);
    reduce_tim = reduce_tim + (unsigned short)(rtc_date.RTC_Date);
    date_str[0] = (unsigned char)(reduce_tim / 256);
    date_str[1] = (unsigned char)(reduce_tim % 256); 
}


void GetRecordTime(unsigned char *time_str,unsigned char *tim_src)
{
	unsigned short reduce_tim;
	reduce_tim = tim_src[0]*256 + tim_src[1];
	time_str[0] = ((reduce_tim>>6)&0x1F);
	time_str[1] = (reduce_tim&0x3F);
}

void GetRecordDateOld(char *date_str,unsigned char *tim_src)
{
	unsigned short reduce_tim;

	RTC_DateTypeDef rtc_date;	

	reduce_tim = tim_src[0]*256 + tim_src[1];

	rtc_date.RTC_Date = reduce_tim>>11;	//date		
	
  	if(rtc_date.RTC_Date > SystemDate.RTC_Date)		
  	{
  		if(SystemDate.RTC_Month == 1)
  		{
  			rtc_date.RTC_Month = 12;
			rtc_date.RTC_Year = SystemDate.RTC_Year -1;
  		}
		else
		{
			rtc_date.RTC_Month = SystemDate.RTC_Month -1;
			rtc_date.RTC_Year = SystemDate.RTC_Year;
		}
  	}
	else
	{
		rtc_date.RTC_Month = SystemDate.RTC_Month;
		rtc_date.RTC_Year = SystemDate.RTC_Year;		
	}
	sprintf(date_str,"20%02d-%02d-%02d",rtc_date.RTC_Year,rtc_date.RTC_Month,rtc_date.RTC_Date);
}

void GetRecordTimeOld(char *time_str,unsigned char *tim_src)
{
	RTC_TimeTypeDef rtc_time;
	unsigned short reduce_tim;

	reduce_tim = tim_src[0]*256 + tim_src[1];
	
	rtc_time.RTC_Hours = ((reduce_tim>>6)&0x1F);
	rtc_time.RTC_Minutes = (reduce_tim&0x3F);
    
	sprintf(time_str,"%02d:%02d:%02d",rtc_time.RTC_Hours,rtc_time.RTC_Minutes,0);
}

unsigned char DateCheck(short year,unsigned char month,unsigned char day)
{
    unsigned char Monthtable[12]={31,29,31,30,31,30,31,31,30,31,30,31};
    if (month == 0)
    {
        return 0;
    }
    if (day > Monthtable[month-1])
    {
            return 0;
    }
    if (month == 2)
    {
        if((year % 400 == 0)|| ((year % 4 == 0) && (year % 100 != 0)))
        {
            if (day > 29)
            {
                return 0;
            }
        }
        else
        {
            if (day > 28)
            {
                return 0;
            }
        }
    }
    return 1;
}


unsigned char CheckRecordDate(unsigned char *tim_src)
{
	unsigned short reduce_tim;

	RTC_DateTypeDef rtc_date;	

	reduce_tim = tim_src[0]*256 + tim_src[1];

	rtc_date.RTC_Date = reduce_tim>>11;	//date		
	
  	if(rtc_date.RTC_Date > SystemDate.RTC_Date)		
  	{
  		if(SystemDate.RTC_Month == 1)
  		{
  			rtc_date.RTC_Month = 12;
			rtc_date.RTC_Year = SystemDate.RTC_Year -1;
  		}
		else
		{
			rtc_date.RTC_Month = SystemDate.RTC_Month -1;
			rtc_date.RTC_Year = SystemDate.RTC_Year;
		}
  	}
	else
	{
		rtc_date.RTC_Month = SystemDate.RTC_Month;
		rtc_date.RTC_Year = SystemDate.RTC_Year;		
	}
    if(rtc_date.RTC_Year >20)   {return 2;}
    if(rtc_date.RTC_Month<1  || rtc_date.RTC_Month >12) {return 0;}
    if(rtc_date.RTC_Date <1  || rtc_date.RTC_Date >31)  {return 0;} 
    return DateCheck(rtc_date.RTC_Year, rtc_date.RTC_Month, rtc_date.RTC_Date);
}

unsigned char CheckRecordTime(unsigned char *tim_src)
{
	RTC_TimeTypeDef rtc_time;
	unsigned short reduce_tim;

	reduce_tim = tim_src[0]*256 + tim_src[1];
	
	rtc_time.RTC_Hours = ((reduce_tim>>6)&0x1F);
	rtc_time.RTC_Minutes = (reduce_tim&0x3F);
    
    if(rtc_time.RTC_Hours >24)   {return 0;}
    if(rtc_time.RTC_Minutes >60) {return 0;}
    
    return 1;
}


////时间格式定义
typedef struct time_format_s
{
	char strFmt[2];
	char t[6];
}time_format_t;

typedef struct time_s
{
	time_format_t Year;
	time_format_t Month;
	time_format_t Day;
	time_format_t Hour;
	time_format_t Min;
	time_format_t Sec;
}time_t;

typedef struct time_u
{
	time_t tt;
	time_format_t tu[6];
}time_u_t;



//获取当时系统时间 
u32_t rtc_get_curTime(char * pTime, char * format)
{
	int i = 0;
	u32_t ret = 0;
	BOOL bFirst = FALSE;
	time_u_t time;
	
	sprintf(time.tt.Year.strFmt,"Y");
	sprintf(time.tt.Month.strFmt,"M");
	sprintf(time.tt.Day.strFmt,"D");
	sprintf(time.tt.Hour.strFmt,"h");
	sprintf(time.tt.Min.strFmt,"m");
	sprintf(time.tt.Sec.strFmt,"s");
	

	sprintf(time.tt.Year.t, "%d", 2000+SystemDate.RTC_Year);
	sprintf(time.tt.Month.t, "%2d", SystemDate.RTC_Month);
  if (SystemDate.RTC_Month < 10) time.tt.Month.t[0] = '0';
	
	sprintf(time.tt.Day.t, "%2d", SystemDate.RTC_Date);
	if (SystemDate.RTC_Date < 10) time.tt.Day.t[0] = '0';
	
	sprintf(time.tt.Hour.t, "%2d", SystemTime.RTC_Hours);
	if (SystemTime.RTC_Hours < 10) time.tt.Hour.t[0] = '0';
	
	sprintf(time.tt.Min.t, "%2d", SystemTime.RTC_Minutes);
	if (SystemTime.RTC_Minutes < 10) time.tt.Min.t[0] = '0';
	
	sprintf(time.tt.Sec.t, "%2d", SystemTime.RTC_Seconds);
	if (SystemTime.RTC_Seconds < 10) time.tt.Sec.t[0] = '0';
	
	memset(pTime, 16, 0);
	for (i=0; i<6; i++)
	{
		if(strstr(format,time.tu[i].strFmt)){
			
			if (bFirst)
			{
				if (i < 3) // M D
				{
					strcat(pTime,"-");
				}
				else if (i == 3)
				{
					strcat(pTime," ");
				}
				else if (i > 3)  // h s
				{
					strcat(pTime,":");
				}
			}
			
			strcat(pTime, time.tu[i].t);
			
		  bFirst = TRUE;
			
		}else { // not find
			
		}
	}
	
	ret = strlen(pTime);
	
	return ret;
}






