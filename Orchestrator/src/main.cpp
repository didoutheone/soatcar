#include <iostream>		// for cout, cerr
#include "Orchestrator.hpp"

using namespace std;
								
void usage(string cmd)
{
	cout << "\033[36m USAGE \033[39m : la commande " << cmd << " peut prendre comme argument: configtest, status, start ou stop" << endl;
}

int main(int argc, char** argv)
{
	if(argc != 2)
	{
		usage(argv[0]);
		exit(0);
	}
	
	Orchestrator orc;
	
	if(Utils::toLower(argv[1]) == "configtest")
	{
		cout << orc.config.toString() << endl;
		exit(0);
	}
	else if(Utils::toLower(argv[1]) == "status")
	{
		orc.status();
		exit(0);
	}
	else if(Utils::toLower(argv[1]) == "stop")
	{
		bool res = orc.stop();
		if(!res)
		{
			cout << "=> Stopped with error" << endl;
			exit(-11);
		}
		else
		{
			cout << "=> Stopped successfully" << endl;
			exit(0);
		}
	}
	else if(Utils::toLower(argv[1]) == "start")
	{
		bool res = orc.start();
		if(!res)
		{
			orc.stop();
			exit(-12);
		}
		else exit(0);
	}
	else
	{
		usage(argv[0]);
		exit(0);
	}
	
	return 0;
}
								
