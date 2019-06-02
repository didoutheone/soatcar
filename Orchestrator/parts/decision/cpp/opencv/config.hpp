#ifndef DEF_CAMERACONFIG
#define DEF_CAMERACONFIG

#include <stdbool.h>	// For bool
#include <string>		// For string

using namespace std;


class AutoPilotConfig
{
	public:
		// ctor: give it the config file path
		AutoPilotConfig(string filename);
		string toString();
		
		// config values
		int takeHistoAtHeightPct;
		
	private:
		string configFilePath;
		void readConfig();
};
#endif