#include "timer.h"
#include "sys.h"
#include "adc.h"
#include "digit.h"

//设定初始时间
u8 	hour = 19;
u8 	minute = 59;
u8 	second = 55;
u8 	_hour;  	//上次的值
u8 	_minute;   	//上次的值
u8 	_second;    //上次的值

u16 srandnum = 2; //随机数

//时间更新标志
vu8 flag_timeupdate = 0;

//digit.c中定义
extern const vu16 Color[10];
extern vu16 segColor;
	
void My_TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_Struce;
	NVIC_InitTypeDef NVIC_Init_Struce;
	
	TIM_TimeBaseInit_Struce.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit_Struce.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
	TIM_TimeBaseInit_Struce.TIM_Period = arr; //自动装载值
	TIM_TimeBaseInit_Struce.TIM_Prescaler = psc; //预分频系数
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //使能定时器时钟
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInit_Struce); //初始化定时器
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //开启定时器中断

	NVIC_Init_Struce.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_Init_Struce.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init_Struce.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init_Struce.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_Init_Struce); //中断优先级初始化
	
	TIM_Cmd(TIM3,ENABLE);//使能定时器
}

void TIM3_IRQHandler() //编写中断服务函数
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{		
		flag_timeupdate = 1;
		second++;
		if (second == 60) {
			second = 0;
			minute++; 
			if (minute == 60) {  //小时更新
				minute = 0;
				hour++;				
				srandnum = Get_RandNum();	//获取随机数
				segColor = Color[srandnum]; //随机更换颜色
				if (hour == 24) { 			//时间计满24后归0
					hour = 0;
				}
			}
		} 		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除中断标志位
	}
}

