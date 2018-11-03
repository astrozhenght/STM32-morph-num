#include "digit.h"
#include "math.h"
#include "lcd.h"
#include "delay.h"

#define ANIM_SPEED  	25  //�����ٶ�ms

vu16 segColor;				//�洢�ε���ɫ

//��ʾ��ܵ��������
u8 locX_now = 3;  			//0-63 ȡ�����ֵ
u8 locY_now = 16;			//0-47 �̶�����

/*������ɫ�������ȡ*/
const vu16 Color[10] = {OLIVE, WHITE, BLUE, BRED, GRED, GBLUE, RED, GREEN, YELLOW, BRRED};

/*��¼�������� X �����ʼλ��*/
const u8 locX_start[] = {54, 45, 33, 24, 12, 3};

/*��¼�ĸ�������Ͻ�λ��*/
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

//��һ�����úõ����ؿ�
void DIGIT_DrawPixelBlock(u16 x, u16 y, u16 color)
{
	LCD_Fill((locX_now+x)*LEN_UNIT + LEN_GAP, 
		(locY_now+y)*LEN_UNIT + LEN_GAP, 
		(locX_now+x)*LEN_UNIT + LEN_GAP + LEN_DIS,
		(locY_now+y)*LEN_UNIT + LEN_GAP + LEN_DIS, color);
}

//�������úõ����ؿ黭��
void DIGIT_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)		//x��Χ0-58��y��Χ0-14
{
	int i;
	if (x1 == x2) {
		/*
		 * ������:��y1�������y2Сʱ,�����κ�����ʼλ����(x1,y1);
		 * ��y1�������y2��ʱ,�����κ�����ʼλ����(x1,y2).
		 */
		if (y1 <= y2)
			for (i  = 0; i <=  y2 - y1; i++)
				DIGIT_DrawPixelBlock(x1, y1+i, color);
		else if (y1 > y2)
			for (i  = 0; i <=  y1 - y2; i++)
				DIGIT_DrawPixelBlock(x1, y2+i, color);
	} else if (y1 == y2) { 
		/*
		 * ������:��x1�������x2Сʱ,�����κ�����ʼλ����(x1,y1);
		 * ��x1�������x2��ʱ,�����κ�����ʼλ����(x2,y1).
		 */
		if (x1 <= x2)
			for (i  = 0; i <=  x2 - x1; i++)
				DIGIT_DrawPixelBlock(x1+i, y1, color);
		else if (x1 > x2)
			for (i  = 0; i <=  x1 - x2; i++)
				DIGIT_DrawPixelBlock(x2+i, y1, color);
	}
}

//�����ֵ�ĳһ�Σ����ַ�Ϊ7�Σ�segΪ�κ�
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

//������
void DIGIT_DrawDigit(u8 num)  //������Χ��Χ0-9
{
	u8 value;
	if (num > 9)  //��������Χ֮�����˳�
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

//����1�䵽2
void DIGIT_Morph2(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) { 
		if (i < SEG_WIDTH) {
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor);  //��A
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, segColor);   //��G
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //��D		
		}
		//��ƽ��E
		DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK); 
		DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

//����2�䵽3
void DIGIT_Morph3(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//��ƽ��E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph4(void) 
{
	int i;
	for (i = 0; i < SEG_WIDTH; i++) {		
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, BLACK);  	  //��A
		DIGIT_DrawPixelBlock(0, 1 + i, segColor);			  //��F
		DIGIT_DrawPixelBlock(1 + i, 2*SEG_HEIGHT + 2, BLACK); //��D
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph5(void) 
{
	int i;
	for (i = 0; i < SEG_WIDTH; i++) {		
		DIGIT_DrawPixelBlock(SEG_WIDTH + 1, SEG_HEIGHT - i, BLACK);  	 //��B
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor); 				 //��A
		DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //��D
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph6(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//��ƽ��C
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
		//��ƽ��B	
		DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);
		DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);
		
		//��ƽ��E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		
		DIGIT_DrawPixelBlock(1 + i, SEG_HEIGHT + 1, BLACK);  	//��G
		DIGIT_DrawPixelBlock(1 + i, 2*SEG_HEIGHT + 2, BLACK);   //��D
		
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph8(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		
		//��ƽ��B
		if (i > 0)
			DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
		DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);
		
		//��ƽ��C
		if (i > 0)
			DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
		DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
		
		if (i < SEG_WIDTH) {			
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, segColor); 			//��G
			DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); 		//��D		
		}
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph9(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//��ƽ��E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);		
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		delay_ms(ANIM_SPEED);
	}
}

//_valueΪ��һ�ε�����ֵ�������ϴε�����ֵ�������˴εĶ���Ч��
void DIGIT_Morph0(u8 _value) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {
		if (_value == 1) {
			//��ƽ��B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);
			
			//��ƽ��C			
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, 0, segColor); //��A
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, 2*SEG_HEIGHT + 2, segColor); //��D		
			}
		}
		
		if (_value == 2) {
			//��ƽ��B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);	

			//��ƽ��E
			if (i > 0)
				DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(i + 1, SEG_HEIGHT + 1, BLACK); //��G
			}			
		}
		
		if (_value == 3) {
			//��ƽ��B
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, 1, SEG_WIDTH - i + 1, SEG_HEIGHT, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, 1, SEG_WIDTH - i, SEG_HEIGHT, segColor);	
			
			//��ƽ��C
			if (i > 0) 
				DIGIT_DrawLine(SEG_WIDTH - i + 1, SEG_HEIGHT + 2, SEG_WIDTH - i + 1, 2*SEG_HEIGHT + 1, BLACK);
			DIGIT_DrawLine(SEG_WIDTH - i, SEG_HEIGHT + 2, SEG_WIDTH - i, 2*SEG_HEIGHT + 1, segColor);
			
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, BLACK);  //��G
			} 
		}
		
		if (_value == 5) {
			if (i <= SEG_WIDTH) {
				//��ƽ��F
				if (i > 0) 
					DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);
				DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);					
			}
		}
		
		if (_value == 5 || _value == 9) {
			if (i < SEG_WIDTH) {
				DIGIT_DrawPixelBlock(SEG_WIDTH - i, SEG_HEIGHT + 1, BLACK); //��G
				DIGIT_DrawPixelBlock(0, SEG_HEIGHT + 2 + i, segColor); 		//��E
			}
		}
		delay_ms(ANIM_SPEED);
	}
}

void DIGIT_Morph1(void) 
{
	int i;
	for (i = 0; i <= SEG_WIDTH; i++) {	
		//��ƽ��F
		DIGIT_DrawLine(i, 1, i, SEG_HEIGHT, BLACK);		
		DIGIT_DrawLine(i + 1, 1, i + 1, SEG_HEIGHT, segColor);
		
		//��ƽ��E
		DIGIT_DrawLine(i, SEG_HEIGHT + 2, i, 2*SEG_HEIGHT + 1, BLACK);		
		DIGIT_DrawLine(i + 1, SEG_HEIGHT + 2, i + 1, 2*SEG_HEIGHT + 1, segColor);
		
		if (i < SEG_WIDTH) {
			DIGIT_DrawPixelBlock(i + 1, 0, BLACK);  			   //��A
			DIGIT_DrawPixelBlock(i + 1, SEG_HEIGHT + 1, BLACK);    //��G
			DIGIT_DrawPixelBlock(i + 1, 2*SEG_HEIGHT + 2, BLACK);  //��D
		}
		delay_ms(ANIM_SPEED);
	}
}

/* ����: ���ֱ任
 * ����: value->��ǰ������ֵ
 *		 _value->�ϴε�����ֵ
 * ����ֵ: none
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
 * ����: ��ϵͳ��ʼʱ��
 * ����: ʱ���룬locX[]�洢�������ֵ���ʼλ��
 */
void DIGIT_DrawStartTime(u8 hour, u8 minute, u8 second, u8 const locX[])
{	
	locX_now = locX[0];			 //��λ
	DIGIT_DrawDigit(second%10);  //����ĸ�λ
	locX_now = locX[1];			 //��λ
	DIGIT_DrawDigit(second/10);  //�����ʮλ
	locX_now = locX[2];			 //��λ
	DIGIT_DrawDigit(minute%10);  //���ֵĸ�λ
	locX_now = locX[3];		 	 //��λ
	DIGIT_DrawDigit(minute/10);  //���ֵ�ʮλ
	locX_now = locX[4];			 //��λ
	DIGIT_DrawDigit(hour%10);    //��ʱ�ĸ�λ
	locX_now = locX[5];			 //��λ
	DIGIT_DrawDigit(hour/10);    //��ʱ��ʮλ	
}

/*
 * ����: ��ʾʱ��֣�������֮���ð��
 * ����: locXY[][]: �洢ð��λ�õ�����
 * 		mode: 0:������ð��
 *   		  1:���ұߵ�ð��
 *   		  2:����ߵ�ð��
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



