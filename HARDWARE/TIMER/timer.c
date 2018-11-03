#include "timer.h"
#include "sys.h"
#include "adc.h"
#include "digit.h"

//�趨��ʼʱ��
u8 	hour = 19;
u8 	minute = 59;
u8 	second = 55;
u8 	_hour;  	//�ϴε�ֵ
u8 	_minute;   	//�ϴε�ֵ
u8 	_second;    //�ϴε�ֵ

u16 srandnum = 2; //�����

//ʱ����±�־
vu8 flag_timeupdate = 0;

//digit.c�ж���
extern const vu16 Color[10];
extern vu16 segColor;
	
void My_TIM3_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInit_Struce;
	NVIC_InitTypeDef NVIC_Init_Struce;
	
	TIM_TimeBaseInit_Struce.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInit_Struce.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
	TIM_TimeBaseInit_Struce.TIM_Period = arr; //�Զ�װ��ֵ
	TIM_TimeBaseInit_Struce.TIM_Prescaler = psc; //Ԥ��Ƶϵ��
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE); //ʹ�ܶ�ʱ��ʱ��
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInit_Struce); //��ʼ����ʱ��
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //������ʱ���ж�

	NVIC_Init_Struce.NVIC_IRQChannel = TIM3_IRQn; 
	NVIC_Init_Struce.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init_Struce.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_Init_Struce.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_Init_Struce); //�ж����ȼ���ʼ��
	
	TIM_Cmd(TIM3,ENABLE);//ʹ�ܶ�ʱ��
}

void TIM3_IRQHandler() //��д�жϷ�����
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) == SET)
	{		
		flag_timeupdate = 1;
		second++;
		if (second == 60) {
			second = 0;
			minute++; 
			if (minute == 60) {  //Сʱ����
				minute = 0;
				hour++;				
				srandnum = Get_RandNum();	//��ȡ�����
				segColor = Color[srandnum]; //���������ɫ
				if (hour == 24) { 			//ʱ�����24���0
					hour = 0;
				}
			}
		} 		
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //����жϱ�־λ
	}
}

