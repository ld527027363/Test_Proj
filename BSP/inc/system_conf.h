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



//异常数据
typedef struct
{
	 unsigned short battery_value; //电量
	 float          bodytemperatrue_value; //体温
	 unsigned char  heartrate_value;  //心率
}ExceptionStruct;					

//服务器下载的参数结构体
typedef struct
{
	 unsigned short  bodytemperatrue_interval; //体温检测间隔�
	 unsigned short  heartrate_interval; //心跳检测间隔
	 unsigned short  report_interval;  //上报周期
	 unsigned short  incident_interval; //事件上报周期
	 unsigned char   sedentary_remind_enable;//久坐使能开关
	 unsigned char   retrytimes;//重试次数
	 unsigned short  handling;//处理方式
	 unsigned char   temperature_threshold;//温度告警阈值
	 unsigned short  heartrate_threshold;//心率告警阈值
	 unsigned short  activedaycycle;//结算日	
	 unsigned int  naptimeperiod;//午睡时间段
	 unsigned int  sleeptimeperiod;//晚睡时间段

}ParameterStruct;		
		
#endif



