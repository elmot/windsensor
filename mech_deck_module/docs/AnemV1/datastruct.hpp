class NmeaReceiver
{
	private:
	 	NmeaReceiver * nextReceiver;
	public:
		void setNextReceiver(NmeaReceiver & nextReceiver);
		virtual void acceptNmea(char * nmeaLine) = 0;
};

class NavData: public NmeaReceiver
{
	private:
  		float windSpeed;
  		float windAngle;
  		float cog;
  		float sog;
  		float lat;
  		float lng;
  		int gpsUpdatePosTicks;
  		int gpsUpdateSpeedTicks;
  		int windUpdateTicks;
	public:
		NavData()
		{
  			windSpeed = 10;
  			windAngle = 12;
  			windUpdateTicks = 0;
		}
//		void acceptWindData(char * nmeaLine);
		virtual void acceptNmea(char * nmeaLine);
  		float getWindSpeed();
  		float getWindAngle();
//  		float getCog();
//  		float getSog();
//  		float getLat();
//  		float getLng();
		void tick(void);
};
extern NavData navData;

class DeviceData
{
	public:
		DeviceData()
		{
			buttonUpPressTicks = 0;
			buttonUpReleaseTicks = 0;
			buttonDownPressTicks = 0;
			buttonDownReleaseTicks = 0;
			buttonOkPressTicks = 0;
			buttonOkReleaseTicks = 0;
		    blinkTicks = 0;
		    bgLight = 5;
			battLevel = -1;
		}	
		static const int BG_LEVELS [8];
		int buttonUpPressTicks;
		int buttonUpReleaseTicks;
		int buttonDownPressTicks;
		int buttonDownReleaseTicks;
		int buttonOkPressTicks;
		int buttonOkReleaseTicks;
		int blinkTicks;
		int bgLight;
		float battLevel;
		void tick(void);
		bool isBlink();


};
extern DeviceData deviceData;

class Settings
{	
	public:
		Settings()
		{
			battCoeff  = 5.0f;
			battAlarm = 6.0f;
			forwardAlarmAngle = 33.0f;
			backwardAlarmAngle = 150.0f;
		}
		int windAngleDiff;
		float windSpeedA;
		float windSpeedB;
		float windSpeedThreshold;
		float cogThreshold;
		float battCoeff;
		float battAlarm;
		float forwardAlarmAngle;
		float backwardAlarmAngle;
} ;
extern Settings settings;

class UIScreen
{	
	public:
		virtual void tick(void) = 0;
		virtual void drawScreen(void) = 0;
};

class MainScreen:  public UIScreen 
{	
	public:
		virtual void tick(void);
		virtual void drawScreen(void);
};

class UIManager: public UIScreen 
{
	public:
		void init(void);
		virtual void tick(void);
		virtual void drawScreen(void);
		void setViewScreen(UIScreen & newViewScreen);
		void setActiveScreen(UIScreen & newActiveScreen);
	private:
		UIScreen * viewScreen;
		UIScreen * activeScreen;
		MainScreen mainScreen;
		
};
extern UIManager uiManager;
