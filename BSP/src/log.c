/**************************************************************************
文件名	: log.c
功能		: 日志文件的产生和输出


****************************************************************************/

#include "global.h"
#include "memory.h"
#include "stdio.h"
#include "stdint.h"
#include "log.h"
#include "rtc.h"
#include "string.h"

FLASH_QUEUE LogFlashQueue;
char LogStrBuf[64];

void Flash_LogQueueInit(void)
{
	LogFlashQueue.flash_addr_start = FLASH_ADDR_LOG_DATA;
	LogFlashQueue.flash_addr_end = FLASH_ADDR_LOG_DATA + FLASH_LOG_MAX_SIZE;
	LogFlashQueue.flash_addr_param = FLASH_ADDR_LOG_CNT;
	LogFlashQueue.data_len = LOG_UNIT_LEN;
	LogFlashQueue.max_cnt = (FLASH_LOG_MAX_SIZE-4096)/LOG_UNIT_LEN;
	FlashQueue_GetParam(&LogFlashQueue);
}


u32_t GetLogCnt(void)
{
	return LogFlashQueue.cnt;
}

void EraseLogMemory(void)		//擦除日志存储区
{
//	int i;
	Flash_SectorErase(FLASH_ADDR_LOG_CNT);
	Flash_SectorErase(FLASH_ADDR_LOG_CNT+4096);
	
	Flash_LogQueueInit();
}

void LogInit(void)
{
	Flash_LogQueueInit();
}
void AddLog(char *str)				//添加一行记录
{
	uint8 i;
	char buf[LOG_UNIT_LEN+2];
	sprintf(buf,"[%02d:%02d:%02d] ",SystemTime.RTC_Hours,SystemTime.RTC_Minutes,SystemTime.RTC_Seconds);
	i = strlen(str);
	if(i > LOG_UNIT_LEN-11)
		i = LOG_UNIT_LEN-11;
	memcpy(buf+11,str,i);
	buf[i+11] = 0;
	FlashQueue_EnQueue(&LogFlashQueue, (uint8 *)buf);
}

uint8_t PrintOneLog(void)
{
	char str[LOG_UNIT_LEN];
	TEMP_FLASH_QUEUE TempLogFlashQueue;
	

	FlashQueue_Copy(&TempLogFlashQueue, &LogFlashQueue); //hcp 20151120 读日志不清除记录，只能用擦除
	if(FlashQueue_DeQueue((FLASH_QUEUE *)&TempLogFlashQueue, (uint8 *)str))
	{
		str[LOG_UNIT_LEN-1] = 0;
		printf(str);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
**获取一条记录，返回记录长度
*/
uint8 log_get_one(uint8 * buf)
{
	static TEMP_FLASH_QUEUE TempLogFlashQueue;
	
	if (TempLogFlashQueue.cnt == 0 && TempLogFlashQueue.bk_cnt == 0)
	{
	   FlashQueue_Copy(&TempLogFlashQueue, &LogFlashQueue); //hcp 20151120 读日志不清除记录，只能用擦除
	}
	if(FlashQueue_DeQueue_NoSave((FLASH_QUEUE *)&TempLogFlashQueue,buf))
	{
		buf[LOG_UNIT_LEN-1] = 0;
		
		return LOG_UNIT_LEN;
	}
	else
	{
		TempLogFlashQueue.cnt = TempLogFlashQueue.bk_cnt = 0;
		return 0;
	}
}

