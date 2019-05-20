#include <fstream>		// for infile
#include <iostream>		// for cout
#include <algorithm>	// for transform to lower case
#include <regex>		// for regex_replace
#include "Config.hpp"

using namespace std;

// -------------------------
//         PUBLIC
// -------------------------
OrchestratorConfig::OrchestratorConfig(string filename)
{
	configFilePath = filename;
	ReadConfig();
}

string OrchestratorConfig::toString()
{
	stringstream ss;
	ss << "- CONFIGURATION -" << endl
			<< "RootDir is " << RootDirectory << endl
			<< "WaitForStop is " << to_string(WaitForStopInMs) << endl
			<< "Steering is in " << ((SteeringAuto) ? "Auto" : "Manual") << endl
			<< "Throttle is in " << ((ThrottleAuto) ? "Auto" : "Manual") << endl;
	
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
	ss << "Caméra part:" << endl;
	ss << camera.toString();
	
	ss << "Decision part:" << endl;
	ss << decision.toString();
	
	ss << "Joystick part:" << endl;
	ss << joystick.toString();
	
	ss << "PWM part:" << endl;
	ss << pwm.toString();
	
	ss << "UltraSonic part:" << endl;
	ss << ultrasonic.toString();
	
	ss << "WebServer part:" << endl;
	ss << webserver.toString();
	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void OrchestratorConfig::ReadConfig()
{
	ifstream infile(configFilePath);
	
	int section = 0;
	
	string line;
	while (getline(infile, line))
	{
		//cout << line << endl; // debug
		
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
			section = TreatSectionLine(line);
			continue;
		}
		
		// Split on equal (=) sign and get header and value
		vector<string> tokens = Split(line);
		
		// Main section
		if(section == MAIN_SECTION)
		{
			// Here we need Steering, Throttle, Root and WaitStop
			if(ToLower(tokens[0]) == "steering")
			{
				SteeringAuto = IsAuto(tokens[1], line);
			}
			else if(ToLower(tokens[0]) == "throttle")
			{
				ThrottleAuto = IsAuto(tokens[1], line);
			}
			else if(ToLower(tokens[0]) == "root")
			{
				RootDirectory = tokens[1];
			}
			else if(ToLower(tokens[0]) == "waitstop")
			{
				if(!IsNumeric(tokens[1]))
				{
					cerr << "**** ERROR: WaitStop must be an integer!" << endl;
					exit(2);
				}
				
				WaitForStopInMs = stoi(tokens[1]);
			}
			else
			{
				cerr << "**** ERROR: line not understood in MAIN section: " << line << endl;
				exit(4);
			}
		}
		// Setup Section
		else if(section == SETUP_SECTION)
		{
			// Retrieve exec commands
			if(ToLower(tokens[0]) == "exec")
			{
				SetupCommands.push_back(tokens[1]);
			}
			else
			{
				cerr << "**** ERROR: line not understood in SETUP section: " << line << endl;
				exit(5);
			}
		}
		// TearDown Section
		else if(section == TEARDOWN_SECTION)
		{
			// Retrieve exec commands
			if(ToLower(tokens[0]) == "exec")
			{
				TearDownCommands.push_back(tokens[1]);
			}
			else
			{
				cerr << "**** ERROR: line not understood in TEARDOWN section: " << line << endl;
				exit(6);
			}
		}
		// Camera Section
		else if(section == CAMERA_SECTION)
		{
			FillPart(camera, tokens, line, "[Camera]");
		}
		// Decision Section
		else if(section == DECISION_SECTION)
		{
			FillPart(decision, tokens, line, "[Camera]");
		}
		// Joystick Section
		else if(section == JOYSTICK_SECTION)
		{
			FillPart(joystick, tokens, line, "[Camera]");
		}
		// PWM Section
		else if(section == PWM_SECTION)
		{
			FillPart(pwm, tokens, line, "[Camera]");
		}
		// UltraSonic Section
		else if(section == ULTRASONIC_SECTION)
		{
			FillPart(ultrasonic, tokens, line, "[Camera]");
		}
		// WebServer Section
		else if(section == WEBSERVER_SECTION)
		{
			FillPart(webserver, tokens, line, "[Camera]");
		}
		else
		{
			cerr << "**** ERROR: config line found outside of any section! " << line << endl;
			exit(7);
		}
		
	}
}


int OrchestratorConfig::TreatSectionLine(string line)
{
	if(ToLower(line) == "[main]") return MAIN_SECTION;
	if(ToLower(line) == "[setup]") return SETUP_SECTION;
	if(ToLower(line) == "[camera]") return CAMERA_SECTION;
	if(ToLower(line) == "[decision]") return DECISION_SECTION;
	if(ToLower(line) == "[joystick]") return JOYSTICK_SECTION;
	if(ToLower(line) == "[pwm]") return PWM_SECTION;
	if(ToLower(line) == "[ultrasonic]") return ULTRASONIC_SECTION;
	if(ToLower(line) == "[webserver]") return WEBSERVER_SECTION;
	if(ToLower(line) == "[teardown]") return TEARDOWN_SECTION;

	return 0;
}

bool OrchestratorConfig::IsAuto(string token, string line)
{
	if(ToLower(token) == "auto")
	{
		return true;
	}
	else if(ToLower(token) == "manual")
	{
		return false;
	}
	else
	{
		cerr << "**** ERROR: boolean config (" << token << ") should be auto or manual!" << endl << "Line is: " << line << endl;
		exit(1);
	}
}

bool OrchestratorConfig::IsNumeric(string s)
{
    return !s.empty() && find_if(s.begin(), s.end(), [](char c) { return !isdigit(c); }) == s.end();
}

string OrchestratorConfig::ToLower(string s)
{
	// convert to lower case
	string res = string(s);
    transform(s.begin(), s.end(), res.begin(), ::tolower);
	return res;
}

vector<string> OrchestratorConfig::Split(string line)
{
	vector<string> result(2);
	string header = line.substr(0, line.find_first_of("="));
	string value = line.substr(line.find_first_of("=")+1);
	string resValue = regex_replace(value, regex("\\[Root]"), RootDirectory);
	
	result[0] = header;
	result[1] = resValue;
	
	return result;
}

void OrchestratorConfig::FillPart(part_config &p, vector<string> tokens, string line, string section)
{
	// We need dir, exec and pidfile
	if(ToLower(tokens[0]) == "dir")
	{
		p.dir = tokens[1];
	}
	else if(ToLower(tokens[0]) == "exec")
	{
		p.exec = tokens[1];
	}
	else if(ToLower(tokens[0]) == "pidfile")
	{
		p.pidfile = tokens[1];
	}
	else
	{
		cerr << "**** ERROR: header " << tokens[0] << " not understood in " << section << " section: " << line << endl;
		exit(3);
	}
}
