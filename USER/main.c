#include "delay.h"
#include "sys.h"
#include "lcd.h"
#include "digit.h"
#include "timer.h"
#include "usart.h"
#include "adc.h"

//timer.c�ж���
extern vu8 flag_timeupdate; 		//ʱ����±�־��1������£�0δ����
extern u8 hour, minute, second;  	//��ǰ��ʱ��ֵ
extern u8 _hour, _minute, _second;  //�ϴε�ʱ��ֵ
extern u16 srandnum; 				//AD���������

//digit.c�ж���
extern const vu16 Color[10];
extern vu16 segColor;
extern u8 locX_now; 				//��̬��ֵ��
extern u8 locY_now;					//ֻ��Ҫ��ֵһ��
extern const u8 locX_start[6]; 	    //��¼�������� X �����ʼλ��
extern const u8 locXY_dot[4][2];    //��¼�ĸ�������Ͻ�λ��
	
int main(void)
{		
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	uart_init(115200);	 	 //���ڳ�ʼ��Ϊ115200
	LCD_Init();
	LCD_Clear(BLACK);

	My_TIM3_Init(9999, 7199);  //1s�ж�
	ADC1_Init();

	//��ȡ���������ֵһ����ɫ
	srandnum = Get_RandNum();
	segColor = Color[srandnum];
	
	//��ð��
	DIGIT_DrawDot(0, locXY_dot); 

	//����ʼʱ��
	DIGIT_DrawStartTime(hour, minute, second, locX_start);	
	_hour = hour;
	_minute = minute;
	_second = second;
	
  	while(1) 
	{
		//����ʱ��ֵ
		if (flag_timeupdate == 1) {
			flag_timeupdate = 0;
			
			//����
			locX_now = locX_start[0];	
			DIGIT_Morph(second%10, _second%10);	 //���λ�Ķ���
			
			if (hour != _hour) {	//Сʱ���ĵ�ʱ��ȫ���ε���ɫ��Ҫ����
				DIGIT_DrawDigit(second%10);
			}
				
			if (second/10 !=  _second/10) {  
				locX_now = locX_start[1];		
				DIGIT_Morph(second/10, _second/10); //��ʮλ�Ķ���
			}
			if (hour != _hour) {
				DIGIT_DrawDigit(second/10); //Сʱ���ĵ�ʱ��ȫ���ε���ɫ��Ҫ����
				locX_now = 3; 		   		//Сʱ���ĵ�ʱ�򣬸���ð�ŵ���ɫ
				DIGIT_DrawDot(1, locXY_dot);
			}
				
			//����
			if (_minute != minute) {
				
				locX_now = locX_start[2];			//��λ�����Ӹ�λλ��
				DIGIT_Morph(minute%10, _minute%10);	//���·��ӵĸ�λ
				
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
			
			//��ʱ
			if (_hour != hour) {
				locX_now = locX_start[4]; 		 //��λ��Сʱ��λλ��
				DIGIT_Morph(hour%10, _hour%10);	 //����Сʱ�ĸ�λ
				DIGIT_DrawDigit(hour%10);  		 //����ȫ���ε���ɫ			
				locX_now = locX_start[5];		 //��λ��Сʱʮλλ��
				
				if (hour/10 != _hour/10) {
					DIGIT_Morph(hour/10, _hour/10);
				}
				DIGIT_DrawDigit(hour/10);
			}
			
			//���汾��ʱ��
			_hour = hour;
			_minute = minute;
			_second = second;			
		}
	} 
}
