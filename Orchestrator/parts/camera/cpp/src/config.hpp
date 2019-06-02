#ifndef DEF_CAMERACONFIG
#define DEF_CAMERACONFIG

#include <stdbool.h>	// For bool
#include <string>		// For string

using namespace std;


class CameraConfig
{
	public:
		// ctor: give it the config file path
		CameraConfig(string filename);
		string toString();
		
		// config values
		int DeviceNo;
		int Width;
		int Height;
		int FPS;
		
		
	private:
		string configFilePath;
		void readConfig();
};
#endif