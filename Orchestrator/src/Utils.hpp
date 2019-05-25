#ifndef DEF_ORCHESTRATORUTILS
#define DEF_ORCHESTRATORUTILS

#include <stdbool.h>
#include <string>

using namespace std;

class Utils
{
	public:
		static bool isNumeric(string s);
		static string toLower(string s);
		static int WriteFile(string filename, string text);
		static string ReadFileFirstLine(string filename);
		static int ExecuteShellCommand(string cmd);
		static bool exists(string filename);
};
#endif