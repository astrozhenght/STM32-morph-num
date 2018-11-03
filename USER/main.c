#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "digit.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"

//timer.c中定义
extern vu8 flag_timeupdate; 		//时间更新标志，1代表更新，0未更新
extern u8 hour, minute, second;  	//当前的时间值
extern u8 _hour, _minute, _second;  //上次的时间值
extern u16 srandnum; 				//AD产生随机数

//digit.c中定义
extern const vu16 Color[10];
extern vu16 segColor;
extern u8 locX_now; 				//动态赋值的
extern u8 locY_now;					//只需要赋值一次
extern const u8 locX_start[6]; 	    //记录各个数字 X 轴的起始位置
extern const u8 locXY_dot[4][2];    //记录四个点的左上角位置
	
int main(void)
{		
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	uart_init(115200);	 	 //串口初始化为115200
	LCD_Init();
	LCD_Clear(BLACK);

	My_TIM3_Init(9999, 7199);  //1s中断
	ADC1_Init();

	//获取随机数，赋值一个颜色
	srandnum = Get_RandNum();
	segColor = Color[srandnum];
	
	//画冒号
	DIGIT_DrawDot(0, locXY_dot); 

	//画起始时间
	DIGIT_DrawStartTime(hour, minute, second, locX_start);	
	_hour = hour;
	_minute = minute;
	_second = second;
	
  	while(1) 
	{
		//更新时间值
		if (flag_timeupdate == 1) {
			flag_timeupdate = 0;
			
			//画秒
			locX_now = locX_start[0];	
			DIGIT_Morph(second%10, _second%10);	 //秒个位的动画
			
			if (hour != _hour) {	//小时更改的时候，全部段的颜色都要更新
				DIGIT_DrawDigit(second%10);
			}
				
			if (second/10 !=  _second/10) {  
				locX_now = locX_start[1];		
				DIGIT_Morph(second/10, _second/10); //秒十位的动画
			}
			if (hour != _hour) {
				DIGIT_DrawDigit(second/10); //小时更改的时候，全部段的颜色都要更新
				locX_now = 3; 		   		//小时更改的时候，更新冒号的颜色
				DIGIT_DrawDot(1, locXY_dot);
			}
				
			//画分
			if (_minute != minute) {
				
				locX_now = locX_start[2];			//定位到分钟个位位置
				DIGIT_Morph(minute%10, _minute%10);	//更新分钟的个位
				
				if (hour != _hour) {
					DIGIT_DrawDigit(minute%10);
				}
				if (minute/10 != _minute/10) {	
					locX_now = locX_start[3];		
					DIGIT_Morph(minute/10, _minute/10);
				}
				if (hour != _hour) {
					DIGIT_DrawDigit(minute/10);
					locX_now = 3;
					DIGIT_DrawDot(2, locXY_dot);
				}
			}
			
			//画时
			if (_hour != hour) {
				locX_now = locX_start[4]; 		 //定位到小时个位位置
				DIGIT_Morph(hour%10, _hour%10);	 //更新小时的个位
				DIGIT_DrawDigit(hour%10);  		 //更新全部段的颜色			
				locX_now = locX_start[5];		 //定位到小时十位位置
				
				if (hour/10 != _hour/10) {
					DIGIT_Morph(hour/10, _hour/10);
				}
				DIGIT_DrawDigit(hour/10);
			}
			
			//储存本次时间
			_hour = hour;
			_minute = minute;
			_second = second;			
		}
	} 
}
