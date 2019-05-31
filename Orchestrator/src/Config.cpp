#include <fstream>		// for infile
#include <iostream>		// for cout and cerr
#include <regex>		// for regex_replace
#include "Config.hpp"
#include "Utils.hpp"

using namespace std;

// -------------------------
//         PUBLIC
// -------------------------
OrchestratorConfig::OrchestratorConfig(string filename)
{
	configFilePath = filename;
	if(!Utils::exists(configFilePath))
	{
		cerr << "ERROR: Config file '" << configFilePath << "' does not exist!" << endl;
		exit(-10);
	}
	
	readConfig();
}

string OrchestratorConfig::toString()
{
	stringstream ss;
	ss << "\033[36m *** SOATCAR CONFIGURATION *** \033[39m" << endl
			<< "RootDir is " << RootDirectory << endl
			<< "WaitForStop is " << to_string(WaitForStopInMs) << endl
			<< "Steering is in " << ((SteeringAuto) ? "Auto" : "Manual") << endl
			<< "Throttle is in " << ((ThrottleAuto) ? "Auto" : "Manual") << endl
			<< "Max Throttle Limit is " << to_string(MaxThrottleLimit) << endl
			<< "Constant Throttle is " << ((ConstantThrottleActive) ? "Active" : "Inactive") << endl
			<< "Constant Throttle is set to " << to_string(ConstantThrottleValue) << endl
			<< "PidFile is " << PidFile << endl;
	
	ss << "Setup commands :" << endl;
	for(string setupCmd : SetupCommands)
	{
		ss << "\t\"" << setupCmd << "\"" << endl;
	}
	
	ss << "TearDown commands :" << endl;
	for(string tdCmd : TearDownCommands)
	{
		ss << "\t\"" << tdCmd << "\"" << endl;
	}
	
	// Parts
	for(part_config p : Parts)
	{
		ss << p.toString();
	}
	
	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void OrchestratorConfig::readConfig()
{
	ifstream infile(configFilePath);
	
	int section = 0;
	string sectionName;
	
	string line;
	while (getline(infile, line))
	{	
		// empty line ?
		if(line.find_first_not_of(" \t\r\n") == string::npos) continue;
		
		// comment line ?
		if(line[0] == '#') continue;
		
		// remove endline (\r \n)
		line.erase(remove(line.begin(), line.end(), '\n'), line.end());
		line.erase(remove(line.begin(), line.end(), '\r'), line.end());
		
		// Section line ?
		if(line[0] == '[')
		{
			section = treatSectionLine(line);
			if(section == PART_SECTION) sectionName = getSectionName(line);
			continue;
		}
		
		// Split on equal (=) sign and get header and value
		vector<string> tokens = split(line);
		
		// Main section
		if(section == MAIN_SECTION)
		{
			fillMain(tokens, line);
		}
		// Setup Section
		else if(section == SETUP_SECTION)
		{
			fillSetup(tokens, line);
		}
		// TearDown Section
		else if(section == TEARDOWN_SECTION)
		{
			fillTearDown(tokens, line);
		}
		// Part Section
		else if(section == PART_SECTION)
		{
			fillPart(sectionName, tokens, line);
		}
		else
		{
			cerr << "**** ERROR: config line found outside of any section! " << line << endl;
			exit(-1);
		}
	}
}


int OrchestratorConfig::treatSectionLine(string line)
{
	if(Utils::toLower(line) == "[main]") return MAIN_SECTION;
	if(Utils::toLower(line) == "[setup]") return SETUP_SECTION;
	if(Utils::toLower(line) == "[teardown]") return TEARDOWN_SECTION;

	return PART_SECTION;
}

string OrchestratorConfig::getSectionName(string line)
{
	return line.substr(1, line.find_first_of("]") - 1);
}

bool OrchestratorConfig::isAuto(string token, string line)
{
	if(Utils::toLower(token) == "auto")
	{
		return true;
	}
	else if(Utils::toLower(token) == "manual")
	{
		return false;
	}
	else
	{
		cerr << "**** ERROR: boolean config (" << token << ") should be auto or manual!" << endl << "Line is: " << line << endl;
		exit(-30);
	}
}

vector<string> OrchestratorConfig::split(string line)
{
	vector<string> result(2);
	string header = line.substr(0, line.find_first_of("="));
	string value = line.substr(line.find_first_of("=")+1);
	string resValue = regex_replace(value, regex("\\[Root]"), RootDirectory);
	
	result[0] = header;
	result[1] = resValue;
	
	return result;
}

void OrchestratorConfig::fillMain(vector<string> tokens, string line)
{
	// Here we need Steering, Throttle, MaxThrottleLimit, ConstantThrottleActive, ConstantThrottleValue, Root and WaitStop
	if(Utils::toLower(tokens[0]) == "steering")
	{
		SteeringAuto = isAuto(tokens[1], line);
	}
	else if(Utils::toLower(tokens[0]) == "throttle")
	{
		ThrottleAuto = isAuto(tokens[1], line);
	}
	else if(Utils::toLower(tokens[0]) == "maxthrottlelimit")
	{
		if(!Utils::isNumeric(tokens[1]))
		{
			cerr << "**** ERROR: MaxThrottleLimit must be an integer!" << endl;
			exit(-20);
		}
		MaxThrottleLimit = stoi(tokens[1]);
	}
	else if(Utils::toLower(tokens[0]) == "ConstantThrottleActive")
	{
		ConstantThrottleActive = (Utils::toLower(tokens[1]) == "true");
	}
	else if(Utils::toLower(tokens[0]) == "ConstantThrottleValue")
	{
		if(!Utils::isNumeric(tokens[1]))
		{
			cerr << "**** ERROR: ConstantThrottleValue must be an integer!" << endl;
			exit(-21);
		}
		ConstantThrottleValue = stoi(tokens[1]);
	}
	else if(Utils::toLower(tokens[0]) == "root")
	{
		RootDirectory = tokens[1];
	}
	else if(Utils::toLower(tokens[0]) == "waitstop")
	{
		if(!Utils::isNumeric(tokens[1]))
		{
			cerr << "**** ERROR: WaitStop must be an integer!" << endl;
			exit(-22);
		}
		
		WaitForStopInMs = stoi(tokens[1]);
	}
	else if(Utils::toLower(tokens[0]) == "pidfile")
	{
		PidFile = tokens[1];
	}
	else
	{
		cerr << "**** ERROR: line not understood in MAIN section: " << line << endl;
		exit(-23);
	}
}

void OrchestratorConfig::fillSetup(vector<string> tokens, string line)
{
	// Retrieve exec commands
	if(Utils::toLower(tokens[0]) == "exec")
	{
		SetupCommands.push_back(tokens[1]);
	}
	else
	{
		cerr << "**** ERROR: line not understood in SETUP section: " << line << endl;
		exit(-40);
	}
}

void OrchestratorConfig::fillTearDown(vector<string> tokens, string line)
{
	// Retrieve exec commands
	if(Utils::toLower(tokens[0]) == "exec")
	{
		TearDownCommands.push_back(tokens[1]);
	}
	else
	{
		cerr << "**** ERROR: line not understood in TEARDOWN section: " << line << endl;
		exit(-41);
	}
}

void OrchestratorConfig::fillPart(string section, vector<string> tokens, string line)
{
	// Get part by name
	int foundPos = -1;
	for(unsigned int i=0; i < Parts.size(); ++i)
	{
		if(Parts[i].name == section)
		{
			 foundPos = (int)i;
			 break;
		}
	}
	
	if(foundPos == -1)
	{
		part_config p;
		p.name = section;
		Parts.push_back(p);
		foundPos = Parts.size() - 1;
	}
	
	// We need dir, exec and pidfile
	if(Utils::toLower(tokens[0]) == "dir")
	{
		Parts[foundPos].dir = tokens[1];
	}
	else if(Utils::toLower(tokens[0]) == "exec")
	{
		Parts[foundPos].exec = tokens[1];
	}
	else if(Utils::toLower(tokens[0]) == "pidfile")
	{
		Parts[foundPos].pidfile = tokens[1];
	}
	else if(Utils::toLower(tokens[0]) == "logfile")
	{
		Parts[foundPos].logfile = tokens[1];
	}
	else
	{
		cerr << "**** ERROR: header " << tokens[0] << " not understood in " << section << " section: " << line << endl;
		exit(-42);
	}
}
