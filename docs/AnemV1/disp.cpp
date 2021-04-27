#include  "winstar.hpp"
#include  "main.hpp"
#include  "datastruct.hpp"
#include  <math.h>
#include  <stdio.h>
const int DeviceData::BG_LEVELS [8] = {0,1,2,5,10,20,50,100};
const float PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899;
extern BWPict navbg;
extern BWPict navfnt;

NavData navData;
DeviceData deviceData;
Settings settings;
UIManager uiManager;


AffineTransform affineTransform = new AffineTransform(& winstarLCD);

void writeArrow(GraphSurface & graphSurface)
{
	int c = graphSurface.G_WIDTH / 2;
	graphSurface.line(c , 5 , c, graphSurface.G_HEIGHT / 2,3);
}

void writeArrowOut(GraphSurface & graphSurface)
{
	int c = graphSurface.G_WIDTH / 2;
	graphSurface.line(c, 3, c - 5, 12);
	graphSurface.line(c, 3, c + 5, 12);
	graphSurface.line(c - 5, 12 , c + 5, 12);
}

void writeArrowIn(GraphSurface & graphSurface)
{
	int c = graphSurface.G_WIDTH / 2;
	graphSurface.line(c - 5, 5, c , 12);
	graphSurface.line(c, 12, c + 5, 5);
	graphSurface.line(c - 5, 5 , c + 5, 5);
}
void setBgLightLevel(int level)
{
	if(level < 0) level = 0;
	if(level >= sizeof(DeviceData::BG_LEVELS) ) level = sizeof(DeviceData::BG_LEVELS)-1;
  	winstarLCD.bgLight(DeviceData::BG_LEVELS[level]);
}
void drawMainBackground(void)
{
  /*BackGround*/
  winstarLCD.writePict(0,0,160,navbg,0,160);
}
void MainScreen::drawScreen(void)
{
 winstarLCD.clearGraphBuff();
  /*Battery*/
  if(deviceData.isBlink() && deviceData.battLevel < settings.battAlarm)
  {
	  winstarLCD.writePict(0,128,32,navfnt,11*32,11*32);
  }
  /*Display BG sign*/
  if(deviceData.bgLight > 0)
  {
	  winstarLCD.writePict(16,128,32,navfnt,10*32,10*32);
  }
  float windSpeed = navData.getWindSpeed();
  float windAngle = navData.getWindAngle();
  char str [5];
  if(windAngle >= 0 && windSpeed > settings.windSpeedThreshold)
  {
  	affineTransform.reset();
	/* Display Wind angle mark */
  	affineTransform.rotate(windAngle /180.0f * PI ,winstarLCD.G_WIDTH/2,winstarLCD.G_HEIGHT/2);
  	writeArrow(affineTransform);
	drawMainBackground();
	sprintf (str, "%3.0f", windAngle);
	/* Display Wind angle digits */
	for(int i = 0; str[i] !=0;i++)
	{	
		int offset = (str[i] -'0');
  		if(offset >=0 && offset<=9) 
		{
			winstarLCD.writePict(4 + i * 4,62,32,navfnt,offset*32,offset*32);
		}
	}

	  /*Arrows*/
//  affineTransform.reset();
//  affineTransform.rotate(-1,winstarLCD.G_WIDTH/2,winstarLCD.G_HEIGHT/2);
//  writeArrowOut(affineTransform);
//  affineTransform.reset();
//  affineTransform.rotate(2.4,winstarLCD.G_WIDTH/2,winstarLCD.G_HEIGHT/2);
//  writeArrowIn(affineTransform);

  }
  else
  {
  	/* Display hiphens insead of Wind digits */
	drawMainBackground();
	winstarLCD.line(36,78,60,78,6);
	winstarLCD.line(68,78,92,78,6);
	winstarLCD.line(98,78,122,78,6);
  }
  winstarLCD.updateGraphScreen();
  setBgLightLevel(deviceData.bgLight);
}

void sysTick(void)
{
	navData.tick();
	deviceData.tick();
	if(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)==SET)
	{
  		uint16_t value = ADC_GetConversionValue(ADC1);
		deviceData.battLevel = ((value & 0xFFF) * VREF * settings.battCoeff * 4  / 4095 + deviceData.battLevel) / 5;
	}
	ADC_SoftwareStartConv(ADC1);
}

void DeviceData::tick()
{	
	
	blinkTicks++;
	if(GPIO_ReadInputDataBit(BTN_UP_PORT, BTN_UP_PIN)) 
	{
		deviceData.buttonUpPressTicks = 0;
		deviceData.buttonUpReleaseTicks++;
	}
	else
	{
		deviceData.buttonUpReleaseTicks = 0;
		deviceData.buttonUpPressTicks++;
	}
	if(GPIO_ReadInputDataBit(BTN_DOWN_PORT, BTN_DOWN_PIN)) 
	{
		deviceData.buttonDownPressTicks = 0;
		deviceData.buttonDownReleaseTicks++;
	}
	else
	{
		deviceData.buttonDownReleaseTicks = 0;
		deviceData.buttonDownPressTicks++;
	}
	if(GPIO_ReadInputDataBit(BTN_OK_PORT, BTN_OK_PIN)) 
	{
		deviceData.buttonOkPressTicks = 0;
		deviceData.buttonOkReleaseTicks++;
	}
	else
	{
		deviceData.buttonOkReleaseTicks = 0;
		deviceData.buttonOkPressTicks++;
	}
}

bool DeviceData::isBlink()
{
	 return (bool) (blinkTicks & 1024);
}

void NavData::tick()
{
	windUpdateTicks++;
	gpsUpdatePosTicks++;
  	gpsUpdateSpeedTicks++;
}

float NavData::getWindAngle()
{
	return windUpdateTicks > 5000 ? -1 : windAngle;
}

float NavData::getWindSpeed()
{
	return windUpdateTicks > 5000 ? -1 : windSpeed;
}
void NavData::acceptNmea(char * nmeaLine)
{
}


void MainScreen::tick(void)
{
}

void UIManager::init(void)
{
	setViewScreen(mainScreen);
	setActiveScreen(mainScreen);
}

void UIManager::drawScreen(void)
{
 	bool lightAlarm = false;
  	float windSpeed = navData.getWindSpeed();
  	float windAngle = navData.getWindAngle();
	if(windAngle >= 0 && windSpeed > settings.windSpeedThreshold)
	{
		/* Display Check for alarm lamp*/
 		if(!deviceData.isBlink())
		{
			lightAlarm = (windAngle < settings.forwardAlarmAngle) || 
				(windAngle > 360 - settings.forwardAlarmAngle) ||
				(windAngle > settings.backwardAlarmAngle && windAngle < (360 - settings.backwardAlarmAngle));
		}
	}
  	GPIO_WriteBit(FRONT_LED_PORT, FRONT_LED_PIN, lightAlarm ? Bit_SET : Bit_RESET);
	viewScreen->drawScreen();
}

void UIManager::tick(void)
{
	activeScreen -> tick();
}

void UIManager::setViewScreen(UIScreen & newViewScreen)
{
	viewScreen = & newViewScreen;
	drawScreen();
}


void UIManager::setActiveScreen(UIScreen & newActiveScreen)
{
	activeScreen = & newActiveScreen;
}
