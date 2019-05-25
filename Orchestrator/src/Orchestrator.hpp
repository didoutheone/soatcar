#ifndef DEF_ORCHESTRATORMAIN
#define DEF_ORCHESTRATORMAIN

#include "../tools/cpp/SoatcarState.hpp"
#include "Config.hpp"
#include "Utils.hpp"
#include "PartManager.hpp"



void usage(string cmd);

class Orchestrator
{
	public:
		SoatcarState *state;
		OrchestratorConfig config{"soatcar.conf"};
		
		bool isOrchestratorAlive();
		void status();
		bool stop();
		bool start();
};

#endif