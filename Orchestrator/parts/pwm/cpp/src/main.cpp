#include <iostream>			// for cout, cerr
#include <thread>       	// for this_thread

#include "pwm.hpp"
#include "config.hpp"
#include "SoatcarState.hpp"
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
			PWMConfig config{"pwm.conf"};
			cout << config.toString();
			exit(0);
		}
		else
		{
			usage(argv[0]);
			exit(0);
		}
	}
	
	// PWM direction
	PWM Direction{I2C_STEERING};
	// PWM throttle
	PWM Throttle{I2C_THROTTLE};
	
	SoatcarState state{"/var/tmp/soatcarmmf.tmp"};
	state.setActuatorReady(true);
	while(!state.getStopFlag())
	{
		Direction.work();
		Throttle.work();
		this_thread::sleep_for(chrono::milliseconds(5));
	}
	
	state.setActuatorReady(false);
	
	return 0;
}
								
