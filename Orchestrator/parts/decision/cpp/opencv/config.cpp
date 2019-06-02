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
AutoPilotConfig::AutoPilotConfig(string filename)
{
	configFilePath = filename;
	if(!Utils::exists(configFilePath))
	{
		cerr << "ERROR: Config file '" << configFilePath << "' does not exist!" << endl;
		exit(-1);
	}
	
	readConfig();
}

string AutoPilotConfig::toString()
{
	stringstream ss;
	ss << "\033[36m *** JOYSTICK CONFIGURATION *** \033[39m" << endl
		<< "Take Histo At Height Pct is " << to_string(takeHistoAtHeightPct) << endl;

	return ss.str();
}



// -------------------------
//         PRIVATE
// -------------------------
void AutoPilotConfig::readConfig()
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
		
		if(Utils::toLower(tokens[0]) == "takehistoatheightpct")
		{
			if(!Utils::isNumeric(tokens[1]))
			{
				cerr << "**** ERROR: takeHistoAtHeightPct must be an integer!" << endl;
				exit(-2);
			}
			takeHistoAtHeightPct = stoi(tokens[1]);
		}
		else
		{
			cerr << "**** ERROR: line not understood: " << line << endl;
			exit(-8);
		}
	}
}

