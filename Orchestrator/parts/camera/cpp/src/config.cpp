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
CameraConfig::CameraConfig(string filename)
{
	configFilePath = filename;
	if(!Utils::exists(configFilePath))
	{
		cerr << "ERROR: Config file '" << configFilePath << "' does not exist!" << endl;
		exit(-1);
	}
	
	readConfig();
}

string CameraConfig::toString()
{
	stringstream ss;
	ss << "\033[36m *** JOYSTICK CONFIGURATION *** \033[39m" << endl
			<< "DeviceNo is " << to_string(DeviceNo) << endl
			<< "Width is " << to_string(Width) << endl
			<< "Height is " << to_string(Height) << endl
			<< "FPS is " << to_string(FPS) << endl;

	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void CameraConfig::readConfig()
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
		
		if(Utils::toLower(tokens[0]) == "cameradeviceno")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: CameraDeviceNo must be an integer!" << endl;
				exit(-2);
			}
			DeviceNo = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "camerawidth")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: CameraWidth must be an integer!" << endl;
				exit(-3);
			}
			Width = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "cameraheight")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: CameraHeight must be an integer!" << endl;
				exit(-4);
			}
			Height = stoi(tokens[1]);
		}
		else if(Utils::toLower(tokens[0]) == "camerafps")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: FPS must be an integer!" << endl;
				exit(-5);
			}
			FPS = stoi(tokens[1]);
		}
		else
		{
			cerr << "**** ERROR: line not understood: " << line << endl;
			exit(-8);
		}
	}
}

