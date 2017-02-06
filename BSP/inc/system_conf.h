#ifndef __SYSTEMCONF_H
#define __SYSTEMCONF_H

#include "types.h"

//#define uint8 unsigned char 
//#define uint16 unsigned short
//#define uint32 unsigned int

//#define int8 char
//#define int16 short

#define PACKET_SOF 0x02

#define ApplicationAddress     0x8020000

#define FLASH_Sector_0_Adress  0x08000000  //16k
#define FLASH_Sector_1_Adress  0x08004000  //16k
#define FLASH_Sector_2_Adress  0x08008000  //16k
#define FLASH_Sector_3_Adress  0x0800C000  //16k
#define FLASH_Sector_4_Adress  0x08010000  //64k
#define FLASH_Sector_5_Adress  0x08020000  //128k
#define FLASH_Sector_6_Adress  0x08040000  //128k
#define FLASH_Sector_7_Adress  0x08060000  //128k

#define FLASH_Sector_8_Adress  0x08080000  //128k
#define FLASH_Sector_9_Adress  0x080A0000  //128k
#define FLASH_Sector_10_Adress 0x080C0000  //128k
#define FLASH_Sector_11_Adress 0x080E0000  //128k



//Òì³£Êı¾İ
typedef struct
{
	 unsigned short battery_value; //µçÁ¿
	 float          bodytemperatrue_value; //ÌåÎÂ
	 unsigned char  heartrate_value;  //ĞÄÂÊ
}ExceptionStruct;					

//·şÎñÆ÷ÏÂÔØµÄ²ÎÊı½á¹¹Ìå
typedef struct
{
	 unsigned short  bodytemperatrue_interval; //ÌåÎÂ¼ì²â¼ä¸ôÚ
	 unsigned short  heartrate_interval; //ĞÄÌø¼ì²â¼ä¸ô
	 unsigned short  report_interval;  //ÉÏ±¨ÖÜÆÚ
	 unsigned short  incident_interval; //ÊÂ¼şÉÏ±¨ÖÜÆÚ
	 unsigned char   sedentary_remind_enable;//¾Ã×øÊ¹ÄÜ¿ª¹Ø
	 unsigned char   retrytimes;//ÖØÊÔ´ÎÊı
	 unsigned short  handling;//´¦Àí·½Ê½
	 unsigned char   temperature_threshold;//ÎÂ¶È¸æ¾¯ãĞÖµ
	 unsigned short  heartrate_threshold;//ĞÄÂÊ¸æ¾¯ãĞÖµ
	 unsigned short  activedaycycle;//½áËãÈÕ	
	 unsigned int  naptimeperiod;//ÎçË¯Ê±¼ä¶Î
	 unsigned int  sleeptimeperiod;//ÍíË¯Ê±¼ä¶Î

}ParameterStruct;		
		
#endif



