#ifndef DEF_PWMCONFIG
#define DEF_PWMCONFIG

#include <stdbool.h>	// For bool
#include <string>		// For string

using namespace std;


class PWMConfig
{
	public:
		// ctor: give it the config file path
		PWMConfig(string filename);
		string toString();
		
		// config values
		int Address;
		int BusNo;
		int SpeedChannel;
		int DirectionChannel;
		int PulseFrequency;
		int SpeedMinValue;
		int SpeedMaxValue;
		int DirectionMinValue;
		int DirectionMaxValue;
		
	private:
		string configFilePath;
		void readConfig();
};
#endif