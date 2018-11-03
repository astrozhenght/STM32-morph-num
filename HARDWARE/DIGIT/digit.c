#include "digit.h"
#include "math.h"
#include "lcd.h"
#include "delay.h"

#define ANIM_SPEED  	25  //动画速度ms

vu16 segColor;				//存储段的颜色

//显示框架的起点坐标
u8 locX_now = 3;  			//0-63 取数组的值
u8 locY_now = 16;			//0-47 固定不变

/*字体颜色，随机获取*/
const vu16 Color[10] = {OLIVE, WHITE, BLUE, BRED, GRED, GBLUE, RED, GREEN, YELLOW, BRRED};

/*记录各个数字 X 轴的起始位置*/
const u8 locX_start[] = {54, 45, 33, 24, 12, 3};

/*记录四个点的左上角位置*/
const u8 locXY_dot[4][2] = {18, 4, 18, 9, 39, 4, 39, 9};

const u8 digitBits[] = {
  0xFC, //B11111100, 0,  ABCDEF--
  0x60, //B01100000, 1,  -BC-----
  0xDA, //B11011010, 2,  AB-DE-G-
  0xF2, //B11110010, 3,  ABCD--G-
  0x66, //B01100110, 4,  -BC--FG-
  0xB6, //B10110110, 5,  A-CD-FG-
  0xBE, //B10111110, 6,  A-CDEFG-
  0xE0, //B11100000, 7,  ABC-----
  0xFE, //B11111110, 8,  ABCDEFG-
  0xF6, //B11110110, 9,  ABCD_FG-
};

//画一个配置好的像素块
void DIGIT_DrawPixelBlock(u16 x, u16 y, u16 color)
{
	LCD_Fill((locX_now+x)*LEN_UNIT + LEN_GAP, 
		(locY_now+y)*LEN_UNIT + LEN_GAP, 
		(locX_now+x)*LEN_UNIT + LEN_GAP + LEN_DIS,
		(locY_now+y)*LEN_UNIT + LEN_GAP + LEN_DIS, color);
}

//根据设置好的像素块画线
void DIGIT_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)		//x范围0-58，y范围0-14
{
	int i;
	if (x1 == x2) {
		/*
		 * 画竖线:当y1的坐标比y2小时,填充矩形函数起始位置用(x1,y1);
		 * 当y1的坐标比y2大时,填充矩形函数起始位置用(x1,y2).
		 */
		if (y1 <= y2)
			for (i  = 0; i <=  y2 - y1; i++)
				DIGIT_DrawPixelBlock(x1, y1+i, color);
		else if (y1 > y2)
			for (i  = 0; i <=  y1 - y2; i++)
				DIGIT_DrawPixelBlock(x1, y2+i, color);
	} else if (y1 == y2) { 
		/*
		 * 画横线:当x1的坐标比x2小时,填充矩形函数起始位置用(x1,y1);
		 * 当x1的坐标比x2大时,填充矩形函数起始位置用(x2,y1).
		 */
		if (x1 <= x2)
			for (i  = 0; i <=  x2 - x1; i++)
				DIGIT_DrawPixelBlock(x1+i, y1, color);
		else if (x1 > x2)
			for (i  = 0; i <=  x1 - x2; i++)
				DIGIT_DrawPixelBlock(x2+i, y1, color);
	}
}

//画数字的某一段，数字分为7段，seg为段号
void DIGIT_DrawSeg(u8 seg)
{
  switch (seg) {
	  case SA: DIGIT_DrawLine(1, 0, SEG_WIDTH, 0, segColor); break;
	  case SB: DIGIT_DrawLine(SEG_WIDTH+1, 1, SEG_WIDTH+1, SEG_HEIGHT, segColor); break;
	  case SC: DIGIT_DrawLine(SEG_WIDTH+1, SEG_HEIGHT+2, SEG_WIDTH+1, 2*SEG_HEIGHT+1, segColor); break;
	  case SD: DIGIT_DrawLine(1, 2*SEG_HEIGHT+2, SEG_WIDTH, 2*SEG_HEIGHT+2, segColor); break;
	  case SE: DIGIT_DrawLine(0, SEG_HEIGHT+2, 0, 2*SEG_HEIGHT+1, segColor); break;
	  case SF: DIGIT_DrawLine(0, 1, 0, SEG_HEIGHT, segColor); break;
	  case SG: DIGIT_DrawLine(1, SEG_HEIGHT+1, SEG_WIDTH, SEG_HEIGHT+1, segColor); break;
	  default: break;
  }
}

//画数字
void DIGIT_DrawDigit(u8 num)  //参数范围范围0-9
{
	u8 value;
	if (num > 9)  //不在允许范围之内则退出
		return;
	value = digitBits[num];
	if (value & 0x80)	DIGIT_DrawSeg(SA);
	if (value & 0x40)	DIGIT_DrawSeg(SB);
	if (value & 0x20)	DIGIT_DrawSeg(SC);
	if (value & 0x10)	DIGIT_DrawSeg(SD);
	if (value & 0x08)	DIGIT_DrawSeg(SE);
	if (value & 0x04)	DIGIT_DrawSeg(SF);
	if (value & 0x02)	DIGIT_DrawSeg(SG);
}

//动画1变到2
void DIGIT_Morph2(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) { 
		if (i < SEG_WIDTH) {
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor);  //画A
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, segColor);   //画G
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //画D		
		}
		//左平移E
		DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK); 
		DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

//动画2变到3
void DIGIT_Morph3(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//右平移E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph4(void) 
{
	int i;
	for (i = 0; i < SEG_WIDTH; i++) {		
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, BLACK);  	  //擦A
		DIGIT_DrawPixelBlock(0, 1 + i, segColor);			  //画F
		DIGIT_DrawPixelBlock(1 + i, 2*SEG_HEIGHT + 2, BLACK); //擦D
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph5(void) 
{
	int i;
	for (i = 0; i < SEG_WIDTH; i++) {		
		DIGIT_DrawPixelBlock(SEG_WIDTH + 1, SEG_HEIGHT - i, BLACK);  	 //擦B
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor); 				 //画A
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //画D
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph6(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//左平移C
		if (i > 0)
			DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK); 
		DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph7(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//右平移B	
		DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);
		DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);
		
		//右平移E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		
		DIGIT_DrawPixelBlock(1 + i, SEG_HEIGHT + 1, BLACK);  	//擦G
		DIGIT_DrawPixelBlock(1 + i, 2*SEG_HEIGHT + 2, BLACK);   //擦D
		
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph8(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		
		//左平移B
		if (i > 0)
			DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
		DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);
		
		//左平移C
		if (i > 0)
			DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
		
		if (i < SEG_WIDTH) {			
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, segColor); 			//画G
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); 		//画D		
		}
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph9(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//右平移E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);		
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

//_value为上一次的数字值，根据上次的数字值来决定此次的动画效果
void DIGIT_Morph0(u8 _value) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {
		if (_value == 1) {
			//左平移B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);
			
			//左平移C			
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor); //画A
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //画D		
			}
		}
		
		if (_value == 2) {
			//左平移B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);	

			//右平移E
			if (i > 0)
				DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(i + 1, SEG_HEIGHT + 1, BLACK); //擦G
			}			
		}
		
		if (_value == 3) {
			//左平移B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);	
			
			//左平移C
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, BLACK);  //擦G
			} 
		}
		
		if (_value == 5) {
			if (i <= SEG_WIDTH) {
				//右平移F
				if (i > 0) 
					DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);
				DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);					
			}
		}
		
		if (_value == 5 || _value == 9) {
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, BLACK); //擦G
				DIGIT_DrawPixelBlock(0, SEG_HEIGHT + 2 + i, segColor); 		//画E
			}
		}
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph1(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//右平移F
		DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);		
		DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);
		
		//右平移E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);		
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		
		if (i < SEG_WIDTH) {
			DIGIT_DrawPixelBlock(i + 1, 0, BLACK);  			   //擦A
			DIGIT_DrawPixelBlock(i + 1, SEG_HEIGHT + 1, BLACK);    //擦G
			DIGIT_DrawPixelBlock(i + 1, 2*SEG_HEIGHT + 2, BLACK);  //擦D
		}
		delay_ms(ANIM_SPEED);
	}
}

/* 作用: 数字变换
 * 参数: value->当前的数字值
 *		 _value->上次的数字值
 * 返回值: none
 */
void DIGIT_Morph(u8 value, u8 _value) 
{
	switch (value) {
		case 0: DIGIT_Morph0(_value); break;
		case 1: DIGIT_Morph1(); break;
		case 2: DIGIT_Morph2(); break;
		case 3: DIGIT_Morph3(); break;
		case 4: DIGIT_Morph4(); break;
		case 5: DIGIT_Morph5(); break;
		case 6: DIGIT_Morph6(); break;
		case 7: DIGIT_Morph7(); break;
		case 8: DIGIT_Morph8(); break;
		case 9: DIGIT_Morph9(); break;
	}
}

/*
 * 作用: 画系统起始时间
 * 参数: 时分秒，locX[]存储各个数字的起始位置
 */
void DIGIT_DrawStartTime(u8 hour, u8 minute, u8 second, u8 const locX[])
{	
	locX_now = locX[0];			 //定位
	DIGIT_DrawDigit(second%10);  //画秒的个位
	locX_now = locX[1];			 //定位
	DIGIT_DrawDigit(second/10);  //画秒的十位
	locX_now = locX[2];			 //定位
	DIGIT_DrawDigit(minute%10);  //画分的个位
	locX_now = locX[3];		 	 //定位
	DIGIT_DrawDigit(minute/10);  //画分的十位
	locX_now = locX[4];			 //定位
	DIGIT_DrawDigit(hour%10);    //画时的个位
	locX_now = locX[5];			 //定位
	DIGIT_DrawDigit(hour/10);    //画时的十位	
}

/*
 * 作用: 显示时与分，分与秒之间的冒号
 * 参数: locXY[][]: 存储冒号位置的数组
 * 		mode: 0:画两个冒号
 *   		  1:画右边的冒号
 *   		  2:画左边的冒号
 */
void DIGIT_DrawDot(u8 mode, u8 const locXY[][2])
{	
	u8 i, start, end;
	switch (mode) {
		case 0: start = 0; end = 4; break;
		case 1: start = 2; end = 4; break;
		case 2: start = 0; end = 2; break;
		default: break;
	}
	for (i = start; i < end; i++) {
		DIGIT_DrawPixelBlock(locXY[i][0], locXY[i][1], segColor);	
		DIGIT_DrawPixelBlock(locXY[i][0]+1, locXY[i][1], segColor);	
		DIGIT_DrawPixelBlock(locXY[i][0], locXY[i][1]+1, segColor);	
		DIGIT_DrawPixelBlock(locXY[i][0]+1, locXY[i][1]+1, segColor);			
	}	
}



