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



//�쳣����
typedef struct
{
	 unsigned short battery_value; //����
	 float          bodytemperatrue_value; //����
	 unsigned char  heartrate_value;  //����
}ExceptionStruct;					

//���������صĲ����ṹ��
typedef struct
{
	 unsigned short  bodytemperatrue_interval; //���¼�����
	 unsigned short  heartrate_interval; //���������
	 unsigned short  report_interval;  //�ϱ�����
	 unsigned short  incident_interval; //�¼��ϱ�����
	 unsigned char   sedentary_remind_enable;//����ʹ�ܿ���
	 unsigned char   retrytimes;//���Դ���
	 unsigned short  handling;//����ʽ
	 unsigned char   temperature_threshold;//�¶ȸ澯��ֵ
	 unsigned short  heartrate_threshold;//���ʸ澯��ֵ
	 unsigned short  activedaycycle;//������	
	 unsigned int  naptimeperiod;//��˯ʱ���
	 unsigned int  sleeptimeperiod;//��˯ʱ���

}ParameterStruct;		
		
#endif



