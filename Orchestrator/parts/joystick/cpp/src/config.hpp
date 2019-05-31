#ifndef DEF_ORCHESTRATORCONFIG
#define DEF_ORCHESTRATORCONFIG

#include <stdbool.h>	// For bool
#include <vector>		// For vector
#include <string>		// For string

using namespace std;


class JoystickConfig
{
	public:
		// ctor: give it the config file path
		JoystickConfig(string filename);
		string toString();
		
		// config values
		int DeviceNo;
		int Button1;
		int Button2;
		int Button3;
		int AxisSpeed;
		int AxisDirection;
		bool AxisSpeedInverted;
		bool AxisDirectionInverted;
		
		
	private:
		string configFilePath;
		void readConfig();
		vector<string> split(string line);
};
#endif