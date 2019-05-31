#include <iostream>		// for cerr
#include <fstream>		// for ifstrem, ofstream
#include <string>		// for string
#include <algorithm>	// for find_if, transform
#include <sys/stat.h>	// for stat
#include "Utils.hpp"

using namespace std;

bool Utils::isNumeric(string s)
{
	return !s.empty() && find_if(s.begin(), s.end(), [](char c) { return !isdigit(c); }) == s.end();
}

string Utils::toLower(string s)
{
	// convert to lower case
	string res = string(s);
    transform(s.begin(), s.end(), res.begin(), ::tolower);
	return res;
}

int Utils::WriteFile(string filename, string text)
{
	ofstream thefile(filename);
	if (thefile.is_open())
	{ 
		thefile << text;
		thefile.close();
		return 0;
	}
	else
	{
		cerr << "ERROR: Unable to open file '" << filename << "' for writing" << endl; 
		return -1;
	}
}

string Utils::ReadFileFirstLine(string filename)
{
	if(!exists(filename)) return "";
	
	ifstream thefile(filename);
	if (thefile.is_open())
	{
		string line;
		getline(thefile, line);
		thefile.close();
		return line;
	}
	else
	{
		cerr << "ERROR: Unable to open file '" << filename << "' for reading" << endl;
		return "";
	}
}

int Utils::ExecuteShellCommand(string cmd)
{
	return system(cmd.c_str());
}

bool Utils::exists(string filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}