#ifndef DEF_SOATCARUTILS
#define DEF_SOATCARUTILS

#include <stdbool.h>	// For Bool
#include <string>		// For string
#include <vector>		// For vector
#include <chrono>       // for chrono

#define DEFAULT_MMF_FILE "/var/tmp/soatcarmmf.tmp"

using namespace std;

// to measure real time, use the high resolution clock if it is steady, the steady clock overwise
using clock_type = conditional< chrono::high_resolution_clock::is_steady,
								chrono::high_resolution_clock,
								chrono::steady_clock >::type;

class Utils
{
	public:
		static bool isNumeric(string s);
		static string toLower(string s);
		static int writeFile(string filename, string text);
		static string readFileFirstLine(string filename);
		static int executeShellCommand(string cmd);
		static bool exists(string filename);
		static vector<string> split(string line);
		// Chrono:
		static clock_type::time_point getBeginChrono();
		static int getElapsedMillisSince(clock_type::time_point begin);
		static int getFrequency(clock_type::time_point begin);
};
#endif