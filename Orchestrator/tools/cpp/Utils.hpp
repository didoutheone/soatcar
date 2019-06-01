#ifndef DEF_SOATCARUTILS
#define DEF_SOATCARUTILS

#include <stdbool.h>	// For Bool
#include <string>		// For string
#include <vector>		// For vector

#define DEFAULT_MMF_FILE "/var/tmp/soatcarmmf.tmp"

using namespace std;

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
};
#endif