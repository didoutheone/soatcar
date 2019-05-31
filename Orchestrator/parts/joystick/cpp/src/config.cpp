#include <fstream>		// for infile
#include <iostream>		// for cout and cerr
#include <regex>		// for remove
#include <sstream>		// For stringstream
#include "config.hpp"
#include "Utils.hpp"

using namespace std;

// -------------------------
//         PUBLIC
// -------------------------
JoystickConfig::JoystickConfig(string filename)
{
	configFilePath = filename;
	if(!Utils::exists(configFilePath))
	{
		cerr << "ERROR: Config file '" << configFilePath << "' does not exist!" << endl;
		exit(-1);
	}
	
	readConfig();
}

string JoystickConfig::toString()
{
	stringstream ss;
	ss << "\033[36m *** JOYSTICK CONFIGURATION *** \033[39m" << endl
			<< "DeviceNo is " << to_string(DeviceNo) << endl
			<< "Button1 is " << to_string(Button1) << endl
			<< "Button2 is " << to_string(Button2) << endl
			<< "Button3 is " << to_string(Button3) << endl
			<< "AxisSpeed is " << to_string(AxisSpeed) << endl
			<< "AxisDirection is " << to_string(AxisDirection) << endl
			<< "AxisSpeedInverted is " << (AxisSpeedInverted ? "true" : "false") << endl
			<< "AxisDirectionInverted is " << (AxisDirectionInverted ? "true" : "false") << endl;

	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void JoystickConfig::readConfig()
{
	ifstream infile(configFilePath);
	
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
		
		// Split on equal (=) sign and get header and value
		vector<string> tokens = split(line);
		
		if(Utils::toLower(tokens[0]) == "joystickdeviceno")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: JoystickDeviceNo must be an integer!" << endl;
				exit(-2);
			}
			DeviceNo = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystickbutton1")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: joystickButton1 must be an integer!" << endl;
				exit(-3);
			}
			Button1 = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystickbutton2")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: joystickButton2 must be an integer!" << endl;
				exit(-4);
			}
			Button2 = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystickbutton3")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: joystickButton3 must be an integer!" << endl;
				exit(-5);
			}
			Button3 = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystickaxisspeed")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: joystickAxisSpeed must be an integer!" << endl;
				exit(-6);
			}
			AxisSpeed = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystikaxisdirection")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: joystikAxisDirection must be an integer!" << endl;
				exit(-7);
			}
			AxisDirection = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "joystickaxisspeedinverted")
		{
			AxisSpeedInverted = (Utils::toLower(tokens[1]) == "true");
		}
		else if(Utils::toLower(tokens[0]) == "joystickaxisdirectioninverted")
		{
			AxisDirectionInverted = (Utils::toLower(tokens[1]) == "true");
		}
		else
		{
			cerr << "**** ERROR: line not understood: " << line << endl;
			exit(-8);
		}
	}
}



vector<string> JoystickConfig::split(string line)
{
	vector<string> result(2);
	string header = line.substr(0, line.find_first_of("="));
	string value = line.substr(line.find_first_of("=")+1);
	
	result[0] = header;
	result[1] = value;
	
	return result;
}

