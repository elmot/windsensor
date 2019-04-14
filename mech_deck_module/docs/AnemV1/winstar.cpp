#include "winstar.hpp"
#include <string.h>
#include <math.h>
#include "stm32f4xx_tim.h"

WinstarLCD winstarLCD;

WinstarLCD::WinstarLCD()
{
 	graphMode = false;
}



void WinstarLCD::init()
{
	//Config ports for LCD
  GPIO_InitTypeDef  GPIO_InitStructure;
  
  /* Enable the GPIO LCD Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

  /* Configure the GPIO_LCD pins */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  
  /* Configure the GPIO_LED_PM pin */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* TIM4 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

  /* Connect TIM4 pins to AF2 */  
  GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
  
  /* -----------------------------------------------------------------------
	TIM4 Configuration: generate PWM signal

	To get TIM4 counter clock at 100 kHz, the prescaler is computed as follows:
	   Prescaler = (TIM4CLK / TIM4 counter clock) - 1
	   Prescaler = ((SystemCoreClock /2) /1 MHz) - 1
											  
	To get TIM4 output clock at 1 kHz, the period (ARR)) is computed as follows:
	   ARR = (TIM4 counter clock / TIM4 output clock) - 1
		   = 99
				  
	Note: 
	 SystemCoreClock variable holds HCLK frequency and is defined in system_stm32f4xx.c file.
	 Each time the core clock (HCLK) changes, user had to call SystemCoreClockUpdate()
	 function to update SystemCoreClock variable value. Otherwise, any configuration
	 based on this variable will be incorrect.	  
  ----------------------------------------------------------------------- */   
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

  /* Compute the prescaler value */
  int PrescalerValue = (uint16_t) ((SystemCoreClock /2) / 100000) - 1;

  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 99;
  TIM_TimeBaseStructure.TIM_Prescaler = PrescalerValue;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

  /* PWM1 Mode configuration: Channel1 */
  bgLight(0);
  TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(TIM4, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(TIM4, ENABLE);

  /*Send reset to LCD*/
  GPIO_ResetBits(GPIOB, GPIO_Pin_1);
  delay();
  GPIO_SetBits(GPIOB, GPIO_Pin_1);
  // Display Duty
  writeByte(3,159);
}

void WinstarLCD::writeByte(uint8_t regNum, uint8_t value)
{
  wait();
  /*Prepare port to write*/
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
   		GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_ResetBits(GPIOD, GPIO_Pin_10); //CS
  GPIO_SetBits(GPIOD, GPIO_Pin_11);	  //RS
  GPIO_Write(GPIOE, regNum << 8); //Data and RW down
  delay();
  GPIO_SetBits(GPIOB, GPIO_Pin_0);    //E
  delay();
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);    //E down
  delay();
  /*Read busy flag*/
  GPIO_ResetBits(GPIOD, GPIO_Pin_11);	  //RS
  GPIO_Write(GPIOE, value << 8); //Data and RW down
  delay();
  GPIO_SetBits(GPIOB, GPIO_Pin_0);    //E
  delay();
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);    //E down
  delay();
}

void WinstarLCD::delay (void)
{//TODO make with timer
	for(int w = 0; w <100;w++)
	{__NOP();}
}

void WinstarLCD::wait(void)
{
  /*Prepare port to read*/
  GPIO_InitTypeDef  GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
   		GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  /*Read busy flag*/
  GPIO_ResetBits(GPIOD, GPIO_Pin_10); //CS
  GPIO_SetBits(GPIOE, GPIO_Pin_7);	  //RW
  GPIO_SetBits(GPIOD, GPIO_Pin_11);	  //RS
  delay();
  GPIO_SetBits(GPIOB, GPIO_Pin_0);    //E
  delay();
  while (GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)) {};
  delay();
  GPIO_ResetBits(GPIOB, GPIO_Pin_0);    //E down
}


void WinstarLCD::bgLight(int percentage)
{
  TIM_OCInitTypeDef  TIM_OCInitStructure;
  if(percentage > 0)
  {
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = percentage;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_Cmd(TIM4, ENABLE);
  }
  else
  {
  	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  	TIM_OCInitStructure.TIM_Pulse = 0;
  	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    TIM_Cmd(TIM4, ENABLE);
  }
}

	/* Graph routines */
void WinstarLCD::updateGraphScreen(void)
{
   if(!graphMode)
   {
   		graphMode = true;
		//Graph mode
  		writeByte(0,0x32);
  		//Set Pitch 8
  		writeByte(1,7);
  		//Set 20 char/line
  		writeByte(2,G_WIDTH_BYTES - 1);
		//Set display start at 0
  		writeByte(8,0);
  		writeByte(9,0);
   }
  //Set cursor start at 0
  writeByte(10,0);
  writeByte(11,0);
  //Write something
  for(int y = 0; y < G_HEIGHT ;y++)
  {
	  for(int x = 0; x < G_WIDTH_BYTES; x++)
	  {
	    writeByte(12,displayBuffer[G_WIDTH_BYTES*y+x]);
	  }
	}
}
void WinstarLCD::clearGraphBuff(void)
{
	memset(displayBuffer,0,G_BUFF_SIZE);
}

void WinstarLCD::writePict(int dispX8, int dispY, int height,BWPict pict,int pictOrY,int pictAndY)
{
	for(int yShift = 0; yShift < height; yShift++)
	{
		for(int bx = 0; bx < pict.G_WIDTH_BYTES; bx++)
		{
			unsigned char * b = & winstarLCD.displayBuffer[dispX8 + bx + (dispY + yShift) * winstarLCD.G_WIDTH_BYTES];
			* b |= pict.BYTES[(pictOrY+yShift) * pict.G_WIDTH_BYTES + bx];
			* b &= pict.BYTES[(pictAndY+yShift) * pict.G_WIDTH_BYTES + bx];
		}
	}
}

void WinstarLCD::pixel(int x,int y, int color)
{
	if( x < 0 || x >= G_WIDTH || y < 0 ||y >= G_HEIGHT) return;
	uint8_t mask = 1 << (x & 7);
	uint8_t * addr  = & displayBuffer[(x >> 3) + (G_WIDTH_BYTES*y)];
	if(color) *addr |=mask; else *addr &= ~mask;
}
int _abs(int a)
{
	return a > 0 ? a : -a;
}
inline void swap(int & a , int & b)
{
	a = a ^ b;
	b = a ^ b;
	a = a ^ b;
}
void WinstarLCD::line(int x1, int y1, int x2,int y2,int width, char * pattern)
{
     bool steep = _abs((int)y2 - y1) > _abs((long)x2 - x1);
     if (steep)
     {
		 swap(x1, y1);
         swap(x2, y2);
	 }
     int deltax = _abs(x2 - x1);
     int deltay = _abs(y2 - y1);
     int err = deltax / 2;
     int y = y1;
  
     int inc = (x1 < x2) ? 1 : -1;
     int ystep = (y1 < y2) ?  1 : -1;
	 char * patternChar = pattern;
     for (int x = x1; (x2-x)*inc >=0; x+= inc)
     {
	 	 
		 if(* patternChar == 0)
		 {
		 	patternChar = pattern;
		 }
		 int color = *(patternChar++) & 1;
		 for(int d = 0; d < width; d++)
		 {
		 	int dy =d - width/2;
			if (steep) pixel(y + dy,x ,color); else pixel(x ,y + dy,color);
		 }
         err -= deltay;
         if (err < 0 )
		 	{
				y += ystep;
             	err += deltax;
			}
	 }
}

void GraphSurface::box(int x1, int y1, int x2,int y2, int width, char * pattern)
{
	line(x1,y1,x1,y2,width,pattern);
	line(x1,y2,x2,y2,width,pattern);
	line(x2,y2,x2,y1,width,pattern);
	line(x1,y1,x2,y1,width,pattern);
};



AffineTransform::AffineTransform(GraphSurface * _graphSurface)
{
  graphSurface = _graphSurface;
  reset();
}
void AffineTransform::pixel(int x,int y, int color)
{
	int tX = 0.5f + m00 * x + m01 * y + m02;
	int tY = 0.5f + m10 * x + m11 * y + m12;
	graphSurface->pixel(tX,tY,color);
}
void AffineTransform::reset(void )
{
	m00 = m11 = 1;
 	m10 = m01 = m02 = m12 = 0;
}
void AffineTransform::rotate(float theta) {
	float aSin = sin(theta);
	float aCos = cos(theta);
	float M0, M1;
	M0 = m00;
	M1 = m01;
	m00 =  aCos * M0 + aSin * M1;
	m01 = -aSin * M0 + aCos * M1;
	M0 = m10;
	M1 = m11;
	m10 =  aCos * M0 + aSin * M1;
	m11 = -aSin * M0 + aCos * M1;
}

void AffineTransform::translate(float tx, float ty) 
{
    m02 = tx * m00 + ty * m01 + m02;
    m12 = tx * m10 + ty * m11 + m12;
}

void AffineTransform::rotate(float theta, float anchorx, float anchory) {
	translate(anchorx, anchory);
	rotate(theta);
	translate(-anchorx, -anchory);
}

void AffineTransform::line(int x1, int y1, int x2,int y2,int width, char * pattern)
{
	int tX1 = 0.5f + m00 * x1 + m01 * y1 + m02;
	int tY1 = 0.5f + m10 * x1 + m11 * y1 + m12;
	int tX2 = 0.5f + m00 * x2 + m01 * y2 + m02;
	int tY2 = 0.5f + m10 * x2 + m11 * y2 + m12;
	graphSurface->line(tX1, tY1, tX2, tY2,width, pattern);
}

void WinstarLCD::setupText(void)
{
   		graphMode = false;
		//Text mode
  		writeByte(0,0x3C);
		//Cursor
  		writeByte(0,0x3C);
  		//Set Pitch 10/6
  		writeByte(1,0x95);
  		//Set char/line
  		writeByte(2,T_WIDTH);
		//Set Cursor pos at 6
  		writeByte(4,5);
		//Set display start at 0:0
  		writeByte(8,0);
  		writeByte(9,0);
		//Set cursor start at 0:0
  		writeByte(10,0);
  		writeByte(11,0);
		for(int i = 0; i < T_WIDTH * (T_HEIGHT+1); i ++)
		{
			writeByte(12,i);
		}

}

void WinstarLCD::setCursorPos(int cursorPos)
{
  	writeByte(10,cursorPos & 0xff);
  	writeByte(11,cursorPos >> 8);
}

void WinstarLCD::writeText(char * text,int textX, int textY,int cursorX,int cursorY)
{
	int cursorPos = textX + textY * T_WIDTH;
	//Set cursor start
	setCursorPos(cursorPos);
	for(char * cPnt = text; (*cPnt);cPnt++)
	{
		switch(*cPnt)
		{
		 	case '\n': 
				cursorPos += T_WIDTH; 
				setCursorPos(cursorPos);
				break;
		 	case '\r': 
				cursorPos = cursorPos - (cursorPos  % T_WIDTH); 
				setCursorPos(cursorPos);
				break;
		 	case '\t': 
				cursorPos = (cursorPos + 7) & 0xFFF8; 
				setCursorPos(cursorPos);
				break;
			default:
				writeByte(12,* cPnt);
				cursorPos++;

		}
	}
	//Set cursor start
	setCursorPos(cursorX + cursorY * T_WIDTH);

}
