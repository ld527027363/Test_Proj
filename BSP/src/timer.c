#include "global.h"
#include "time.h"
#include "Rf433Protocal.h"
#include "wifi.h"
#include "testmode.h"


unsigned char BeepOn;
unsigned short BeepDelayCnt;
unsigned char BeepDelayPeriod;

unsigned int SysTick10msCnt;
unsigned char SysTick10msFlag;
unsigned char SysTick100msFlag = 0;
unsigned int SysTick1sCnt;
unsigned char SysTick1sFlag;

unsigned short Led2DelayCnt = 0;
unsigned short Led3DelayCnt = 0;
unsigned char  Led2OnFlag = 0;
unsigned char  Led3OnFlag = 0;

void Time7Init(void)  //这个是系统时间定时器
{
	TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef          NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);

	TIM_TimeBaseInitStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
	TIM_TimeBaseInitStructure.TIM_CounterMode       = TIM_CounterMode_Up;
	TIM_TimeBaseInitStructure.TIM_Period            = 52500;//1ms中断一次  是16位定时器
	TIM_TimeBaseInitStructure.TIM_Prescaler         = 8-1;// 预分频8   5.25MHz 
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0x00;

	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);   //必须先清除配置时候产生的更新标志
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE);   //使能中断
	TIM_Cmd(TIM7,ENABLE);    //使能定时器

	NVIC_InitStructure.NVIC_IRQChannel                   = TIM7_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;  
	NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
}
extern uint32  dhcp_time;
void TIM7_IRQHandler(void)
{
	TIM_ClearFlag(TIM7,TIM_FLAG_Update);  

	SysTick10msCnt++;
	SysTick10msFlag = 1;
	Sys_Tick_10ms++;
	if((SysTick10msCnt%10)== 0) //100ms
	{	
	
		WIFITimeTic++;
		if((SysTick10msCnt%100)==0)//      //1s
		{
			SysTick1sCnt++;
			SysTick1sFlag = 1;
			dhcp_time++;                
		}
	}
	if((SysTick10msCnt%25) == 0)	//250ms
	{
		SysTick100msFlag = 1;
		Rf433CurrentSlot++;
		if((Rf433CurrentSlot & 0x03) == 0)			//新的时隙开始
			Rf433SlotBusy = 0;
		if(Rf433CurrentSlot >= (Rf433SlotCap*4))
			Rf433CurrentSlot = 0;
	}
	if(BeepOn)
	{
		BeepDelayCnt++;
		if(BeepDelayCnt>BeepDelayPeriod)
		{
			CLR_BEEP();
			BeepOn=0;
			BeepDelayCnt=0;
		}
	}
	if(Led2OnFlag)
	{
		Led2DelayCnt++;
		if(Led2DelayCnt > 80)
		{
			CLR_LED2();
			Led2OnFlag=0;
			Led2DelayCnt=0;
		}
	}        
	if(Led3OnFlag)
	{
		Led3DelayCnt++;
		if(Led3DelayCnt > 100)
		{
			CLR_LED3();
			Led3OnFlag=0;
			Led3DelayCnt=0;
		}
	}
	if(Rf433SendTimeoutFlag)
	{
		Rf433SendTimeoutCnt--;		
		if(Rf433SendTimeoutCnt == 0)
		{
			Rf433SendTimeoutFlag = 1;
		}
	}

}




















