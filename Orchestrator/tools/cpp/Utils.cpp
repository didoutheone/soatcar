#include <iostream>		// for cerr
#include <fstream>		// for ifstrem, ofstream
#include <string>		// for string
#include <algorithm>	// for find_if, transform
#include <sys/stat.h>	// for stat
#include <vector>		// For vector
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

int Utils::writeFile(string filename, string text)
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

string Utils::readFileFirstLine(string filename)
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

int Utils::executeShellCommand(string cmd)
{
	return system(cmd.c_str());
}

bool Utils::exists(string filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

vector<string> Utils::split(string line)
{
	vector<string> result(2);
	string header = line.substr(0, line.find_first_of("="));
	string value = line.substr(line.find_first_of("=")+1);
	
	result[0] = header;
	result[1] = value;
	
	return result;
}

clock_type::time_point Utils::getBeginChrono()
{
	return clock_type::now();
}

int Utils::getElapsedMillisSince(clock_type::time_point begin)
{
	return chrono::duration_cast<chrono::milliseconds>(clock_type::now() - begin).count();
}

int Utils::getFrequency(clock_type::time_point begin)
{
	int elapsedmilli = chrono::duration_cast<chrono::milliseconds>(clock_type::now() - begin).count();
	if(elapsedmilli > 0)
	{
		return (int)((double)1000 / (double)elapsedmilli);
	}
	else
	{
		return 0;
	}
}