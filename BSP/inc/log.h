#ifndef __LOG_H__
#define __LOG_H__

#include <stdint.h>
#include "memory.h"

extern FLASH_QUEUE LogFlashQueue;

extern void LogInit(void);
extern void EraseLogMemory(void);
extern void AddLog(char *str);
extern unsigned char PrintOneLog(void);
uint32_t GetLogCnt(void);

extern char LogStrBuf[];

 /*
**获取一条记录，返回记录长度
*/
extern uint8_t log_get_one(uint8_t * buf);


#endif

