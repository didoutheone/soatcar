#include <iostream>		// for cout, cerr
#include "camera.hpp"
#include "config.hpp"
#include "Utils.hpp"

using namespace std;
								
void usage(string cmd)
{
	cout << "\033[36m USAGE \033[39m : la commande " << cmd << " peut prendre comme argument: configtest ou rien pour lancer la part" << endl;
}

int main(int argc, char** argv)
{
	if(argc == 2)
	{
		if(Utils::toLower(argv[1]) == "configtest")
		{
			CameraConfig config{"camera.conf"};
			cout << config.toString();
			exit(0);
		}
		else
		{
			usage(argv[0]);
			exit(0);
		}
	}
	
	CameraPart c;
	c.run();
	
	return 0;
}
								
