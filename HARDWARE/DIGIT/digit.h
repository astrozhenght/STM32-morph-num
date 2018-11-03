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

#define LEN_UNIT   			(5)  	 	//像素块长宽为5
#define LEN_DIS    			(3 - 1)  	//显示像素长宽为3
#define LEN_GAP    			(2 - 1)  	//间隙像素为2
#define SEG_WIDTH     		6        	//段宽
#define SEG_HEIGHT    		6        	//段高


//小局的框架长宽
#define AREA_X      		59       	//横向有效值59
#define AREA_Y      		15       	//纵向有效值15


void DIGIT_Morph(u8 value, u8 _value);
void DIGIT_DrawDigit(u8 num);

void DIGIT_DrawStartTime(u8 hour, u8 minute, u8 second, u8 const locX[]);
void DIGIT_DrawDot(u8 mode, u8 const locXY[][2]);



#endif

