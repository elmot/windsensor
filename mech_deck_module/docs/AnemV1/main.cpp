#include "main.hpp"
#include "winstar.hpp"
#include "datastruct.hpp"
/* Private functions */
void TIM_Config(void);
void GPIO_Config(void);
void BAT_Config(void);
void KBD_Config(void);
int main(void)
{
	//Init GPIOs,LEDs
	//INIT UARTS
  	DBGMCU_Config(DBGMCU_SLEEP, ENABLE);
//  UART_Config();
  	winstarLCD.init();
//  	winstarLCD.updateGraphScreen();
//  GPIO_Config();
	BAT_Config();
	KBD_Config();
  	TIM_Config();
  
	uiManager.init();
	while(1)
	{
		uiManager.drawScreen();
	}
	
}

  /* SysTick end of count event each 10ms */
void TIM_Config(void)
{
  RCC_ClocksTypeDef RCC_Clocks;
  RCC_GetClocksFreq(&RCC_Clocks);
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
}

void BAT_Config(void)
{
  ADC_InitTypeDef       ADC_InitStructure;
  ADC_CommonInitTypeDef ADC_CommonInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* GPIO clock enable */
  RCC_AHB1PeriphClockCmd(BAT_ADC_CLOCK, ENABLE);
  
  /* GPIO Configuration ADC1  */
  GPIO_InitStructure.GPIO_Pin = BAT_ADC_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(GPIOC, &GPIO_InitStructure); 
  

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

  ADC_CommonStructInit(&ADC_CommonInitStructure);
  ADC_CommonInit(&ADC_CommonInitStructure);

  /* ADC1 Init ****************************************************************/
  ADC_StructInit(&ADC_InitStructure);
  ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
  ADC_Init(ADC1, &ADC_InitStructure);

  /* ADC1 regular channel14 configuration ******************************/
  ADC_RegularChannelConfig(ADC1, BAT_ADC_CHANNEL, 1, ADC_SampleTime_15Cycles);

  /* Enable ADC1 **************************************************************/
  ADC_Cmd(ADC1, ENABLE);
  ADC_SoftwareStartConv(ADC1);
}
void KBD_Config(void)
{
  	GPIO_InitTypeDef GPIO_InitStructure;
  	/* GPIO clock enable */
  	RCC_AHB1PeriphClockCmd(KEYB_CLOCK1, ENABLE);
  	RCC_AHB1PeriphClockCmd(KEYB_CLOCK2, ENABLE);

  	/* Front led GPIO, PWM*/
  	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
  	GPIO_InitStructure.GPIO_Pin = FRONT_LED_PIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  	GPIO_Init(FRONT_LED_PORT, &GPIO_InitStructure);
	GPIO_WriteBit(FRONT_LED_PORT, FRONT_LED_PIN, Bit_SET);


//#define  BTN_UP_PORT	GPIOC
//#define  BTN_UP_PIN		GPIO_Pin_4
//#define  BTN_OK_PORT	GPIOD
//#define  BTN_OK_PIN		GPIO_Pin_3
//#define  BTN_DOWN_PORT	GPIOD
//#define  BTN_OK_PIN		GPIO_Pin_4
//	GPIO_Init();
}



