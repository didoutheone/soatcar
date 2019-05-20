#ifndef DEF_ORCHESTRATORCONFIG
#define DEF_ORCHESTRATORCONFIG

#include <stdbool.h>
#include <vector>
#include <sstream>
#include <string>

using namespace std;

#define MAIN_SECTION 		1
#define SETUP_SECTION 		2
#define CAMERA_SECTION 		3
#define DECISION_SECTION 	4
#define JOYSTICK_SECTION	5
#define PWM_SECTION			6
#define ULTRASONIC_SECTION	7
#define WEBSERVER_SECTION	8
#define TEARDOWN_SECTION 	9


typedef struct part_config_t {
	string dir;
	string exec;
	string pidfile;
	
	string toString()
	{
		stringstream ss;
		ss << "\tDir: " << dir << endl;
		ss << "\tExec: " << exec << endl;
		ss << "\tPidfile: " << pidfile << endl;
		
		return ss.str();
	}
} part_config;


class OrchestratorConfig
{
	public:
		// ctor: give it the config file path
		OrchestratorConfig(string filename);
		string toString();
		
		// Main
		string RootDirectory;
		int WaitForStopInMs;
		bool SteeringAuto;
		bool ThrottleAuto;
		
		// Setup
		vector<string> SetupCommands;
		
		// PARTS
		// Camera
		part_config camera;
		// Decision
		part_config decision;
		// Joystick
		part_config joystick;
		// PWM
		part_config pwm;
		// UltraSonic
		part_config ultrasonic;
		// WebServer
		part_config webserver;
		
		// TearDown
		vector<string> TearDownCommands;
	
	
	private:
		string configFilePath;
		void ReadConfig();
		int TreatSectionLine(string line);
		bool IsAuto(string token, string line);
		bool IsNumeric(string s);
		string ToLower(string s);
		vector<string> Split(string line);
		void FillPart(part_config &p, vector<string> tokens, string line, string section);
};
#endif