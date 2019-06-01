#ifndef DEF_ORCHESTRATORPARTMGR
#define DEF_ORCHESTRATORPARTMGR

#include "Config.hpp"

using namespace std;

class PartManager
{
	public:
		PartManager(part_config ppart);
		int start();
		bool isAlive();
		bool killPart();
		
	private:
		part_config part;
};

#endif