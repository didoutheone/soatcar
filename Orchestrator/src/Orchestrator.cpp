#include <thread>		// for thread
#include <chrono>		// for chrono
#include <iostream>		// for cout
#include "../tools/cpp/SoatcarState.hpp"
#include "Orchestrator.hpp"
#include "Config.hpp"

using namespace std;

// to measure real time, use the high resolution clock if it is steady, the steady clock overwise
using clock_type = conditional< chrono::high_resolution_clock::is_steady,
                                chrono::high_resolution_clock,
                                chrono::steady_clock >::type;


SoatcarState state("/var/tmp/soatcarmmf.tmp");
OrchestratorConfig config("soatcar.conf");

int main(int argc, char** argv)
{
	clock_type::time_point begin;
	int elapsed_msecs;
	
	cout << config.toString();
	
	// Init State
	// XXX
	
	// Loop till the end
	//while(state.GetStopFlag() == 0)
	{
		begin = clock_type::now();
		
		// Work
		// XXX
		this_thread::sleep_for(chrono::milliseconds(20));
		
		// Loop end
		elapsed_msecs = chrono::duration_cast<chrono::milliseconds>(clock_type::now() - begin).count();
		// Debug
		cout << "Looped in " << elapsed_msecs << "ms" << endl;
	}
	return 0;
}
