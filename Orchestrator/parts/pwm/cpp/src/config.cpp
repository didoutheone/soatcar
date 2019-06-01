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
PWMConfig::PWMConfig(string filename)
{
	configFilePath = filename;
	if(!Utils::exists(configFilePath))
	{
		cerr << "ERROR: Config file '" << configFilePath << "' does not exist!" << endl;
		exit(-1);
	}
	
	readConfig();
}

string PWMConfig::toString()
{
	stringstream ss;
	ss << "\033[36m *** PWM CONFIGURATION *** \033[39m" << endl
			<< "PWMAddress is " << to_string(Address) << endl
			<< "PWMBusNo is " << to_string(BusNo) << endl
			<< "PWMSpeedChannel is " << to_string(SpeedChannel) << endl
			<< "PWMDirectionChannel is " << to_string(DirectionChannel) << endl
			<< "PWMPulseFrequency is " << to_string(PulseFrequency) << endl
			<< "PWMSpeedMinValue is " << to_string(SpeedMinValue) << endl
			<< "PWMSpeedMaxValue is " << to_string(SpeedMaxValue) << endl
			<< "PWMDirectionMinValue is " << to_string(DirectionMinValue) << endl
			<< "PWMDirectionMaxValue is " << to_string(DirectionMaxValue) << endl;
			
	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void PWMConfig::readConfig()
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
		vector<string> tokens = Utils::split(line);
		
		if(Utils::toLower(tokens[0]) == "pwmaddress")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMAddress must be an integer!" << endl;
				exit(-2);
			}
			Address = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmbusno")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMBusNo must be an integer!" << endl;
				exit(-3);
			}
			BusNo = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmspeedchannel")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMSpeedChannel must be an integer!" << endl;
				exit(-4);
			}
			SpeedChannel = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmdirectionchannel")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMDirectionChannel must be an integer!" << endl;
				exit(-5);
			}
			DirectionChannel = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmpulsefrequency")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMPulseFrequency must be an integer!" << endl;
				exit(-6);
			}
			PulseFrequency = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmspeedminvalue")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMSpeedMinValue must be an integer!" << endl;
				exit(-7);
			}
			SpeedMinValue = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmspeedmaxvalue")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMSpeedMaxValue must be an integer!" << endl;
				exit(-7);
			}
			SpeedMaxValue = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmdirectionminvalue")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMDirectionMinValue must be an integer!" << endl;
				exit(-7);
			}
			DirectionMinValue = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "pwmdirectionmaxvalue")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: PWMDirectionMaxValue must be an integer!" << endl;
				exit(-7);
			}
			DirectionMaxValue = stoi(tokens[1]);
		}
		else
		{
			cerr << "**** ERROR: line not understood: " << line << endl;
			exit(-8);
		}
	}
}


