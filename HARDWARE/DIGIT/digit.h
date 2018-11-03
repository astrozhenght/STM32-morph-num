#ifndef DIGIT_H
#define DIGIT_H
#include "sys.h"	 

#define SA 		 0
#define SB 		 1
#define SC 		 2
#define SD 		 3
#define SE 		 4
#define SF 		 5
#define SG 		 6

#define LEN_UNIT   			(5)  	 	//���ؿ鳤��Ϊ5
#define LEN_DIS    			(3 - 1)  	//��ʾ���س���Ϊ3
#define LEN_GAP    			(2 - 1)  	//��϶����Ϊ2
#define SEG_WIDTH     		6        	//�ο�
#define SEG_HEIGHT    		6        	//�θ�


//С�ֵĿ�ܳ���
#define AREA_X      		59       	//������Чֵ59
#define AREA_Y      		15       	//������Чֵ15


void DIGIT_Morph(u8 value, u8 _value);
void DIGIT_DrawDigit(u8 num);

void DIGIT_DrawStartTime(u8 hour, u8 minute, u8 second, u8 const locX[]);
void DIGIT_DrawDot(u8 mode, u8 const locXY[][2]);



#endif

