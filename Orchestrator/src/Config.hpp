#ifndef DEF_ORCHESTRATORCONFIG
#define DEF_ORCHESTRATORCONFIG

#include <stdbool.h>	// For bool
#include <vector>		// For vector
#include <sstream>		// For stringstream
#include <string>		// For string

using namespace std;

#define MAIN_SECTION 		1
#define SETUP_SECTION 		2
#define PART_SECTION 		3
#define TEARDOWN_SECTION 	4


typedef struct part_config_t {
	string name;
	string dir;
	string exec;
	string pidfile;
	string logfile;
	
	string toString()
	{
		stringstream ss;
		ss << "Part " << name << endl;
		ss << "\tDir: " << dir << endl;
		ss << "\tExec: " << exec << endl;
		ss << "\tPidfile: " << pidfile << endl;
		ss << "\tLogfile: " << logfile << endl;
		
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
		int MaxThrottleLimit;
		bool ConstantThrottleActive;
		int ConstantThrottleValue;
		string PidFile;
		
		// Setup
		vector<string> SetupCommands;
		
		// PARTS
		vector<part_config> Parts;
		
		// TearDown
		vector<string> TearDownCommands;
	
	
	private:
		string configFilePath;
		void readConfig();
		int treatSectionLine(string line);
		string getSectionName(string line);
		bool isAuto(string token, string line);
		void fillMain(vector<string> tokens, string line);
		void fillSetup(vector<string> tokens, string line);
		void fillTearDown(vector<string> tokens, string line);
		void fillPart(string section, vector<string> tokens, string line);
};
#endif