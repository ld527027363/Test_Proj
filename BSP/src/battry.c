


#include <stdio.h>
#include "adc.h" 
#include "timer.h" 
#include "uart.h"
#include "utility.h" 
#include "linklayer.h" 
#include "SystemParameter.h"


// 3.3v 基准电压
// 12位ADC采样
// 47K+150K 分压
#define BATTRY_FACTOR			(10000*33/4096*197/47)	//计算电池电压的计算因子

void Battry_AdcInit(void)
{
	STM32_ADC_Init(BATTRY_ADC,BATTRY_ADC_POWER_PORT,BATTRY_ADC_RCC_AHB,BATTRY_ADC_CHANNEL);
	STM32_ADC_Init(POWER_ADC,POWER_ADC_POWER_PORT,POWER_ADC_RCC_AHB,POWER_ADC_CHANNEL);
}

	 
unsigned int Get_Batry_Value(u16 data)
{
	return(data * BATTRY_FACTOR);
}


void Battry_Task(void)
{
	static uint8_t send_temp_hearbeat = 0;
	static uint8_t det_battry_cnt = 0;
	static uint8_t det_power_cnt = 0;
	uint16_t adc_data = 0;
	
	
	if((Battry_Flag == 1)||(Battry_Flag == 2))
	{
		adc_data = Get_Adc(BATTRY_ADC_CHANNEL);
		Battry_Value = Get_Batry_Value(adc_data);
		
		adc_data = Get_Adc(POWER_ADC_CHANNEL);
		Power_Value = Get_Batry_Value(adc_data);
		
		
		if(Battry_Value > Power_Value)
		{
			det_power_cnt = 0;
			if(Err_Code[1] == (ERR_LOSE_POWER & 0x00FF))
			{
				Battry_Flag = 0;
			}
			else
			{
				Battry_Flag = 2;
				det_battry_cnt++;
				if(det_battry_cnt > 100)
					det_battry_cnt = 100;
				if(det_battry_cnt == 6)
				{
					send_temp_hearbeat = 1;
					if(TestReq)
						printf("external power supply off!\r\n");
				}
				
				if(det_battry_cnt > 5)
				{
					Battry_Flag = 0;
					Battry_Alert_Flag = 1;
					Err_Code[0] = ERR_LOSE_POWER >> 8;
					Err_Code[1] = ERR_LOSE_POWER & 0x00FF;
				}
			}
		}
		else
		{
			det_power_cnt++;
			if(det_power_cnt > 100)
				det_power_cnt = 100;
			if(det_power_cnt == 1)
			{
				send_temp_hearbeat = 1;
				if(TestReq)
					printf("external power supply on!\r\n");
			}
			det_battry_cnt = 0;
			Battry_Flag = 0;
			Battry_Alert_Flag = 0;
			Err_Code[0] = ERR_NULL >> 8;
			Err_Code[1] = ERR_NULL;
		}
		if((send_temp_hearbeat == 1)&&(!g_stServerComCtrl.rx_flag))	//不处于接收平台数据状态则立即发送心跳包
		{
			send_temp_hearbeat = 0;
			g_iServerHeartCommTimeCnt = strSystemParameter.HraerBeatTimeout;
		}
		if(TestReq)
		{
			printf("Power  Value = %d\r\n",Power_Value);
			printf("Battry Value = %d\r\n",Battry_Value);
		}
	}
			
}














