#include <iostream>		// for cout, cerr
#include <thread>       // for this_thread
#include <unistd.h> 	// for getpid
#include "Orchestrator.hpp"

using namespace std;

bool Orchestrator::isOrchestratorAlive()
{
	// First the orchestrator. Create à virtual part_config
	part_config po;
	po.name = "Orchestrator";
	po.pidfile = config.PidFile;
	PartManager mgro(po);
	return mgro.isAlive();
}

void Orchestrator::status()
{
	cout << "\033[36m *** SOATCAR STATUS *** \033[39m" << endl;
	
	// Check MMF file
	string mmf("MMF File");
	mmf.insert(mmf.end(), 15 - mmf.size(), ' '); // Just for text alignment
	cout << "- Status of " << mmf << " is: \t" << ((Utils::exists(DEFAULT_MMF_FILE)) ? "\033[32m Existing" : "\033[31m NOT FOUND") << "\033[39m" << endl;
	
	// Check status of each part
	// First the orchestrator
	string s("Orchestrator");
	s.insert(s.end(), 15 - s.size(), ' '); // Just for text alignment
	cout << "- Status of " << s << " is: \t" << (isOrchestratorAlive() ? "\033[32m Alive" : "\033[31m Dead Cold") << "\033[39m" << endl;
	
	// Then each part
	for(part_config p : config.Parts)
	{
		PartManager mgr(p);
		string s = p.name;
		s.insert(s.end(), 15 - s.size(), ' '); // Just for text alignment
		cout << "- Status of " << s << " is: \t" << (mgr.isAlive() ? "\033[32m Alive" : "\033[31m Dead Cold") << "\033[39m" << endl;
	}
}

bool Orchestrator::stop()
{
	bool hasError = false;
	
	cout << "\033[36m *** SOATCAR STOP *** \033[39m" << endl;
	
	// Check if MMF file exists
	if(!Utils::exists(DEFAULT_MMF_FILE))
	{
		cout << "MMF File '" << DEFAULT_MMF_FILE << "' not found so parts cant be running." << endl;
		return true;
	}
	
	cout << "Setting stop flag to true and wait for " << config.WaitForStopInMs << "ms" << endl;
	state = new SoatcarState(DEFAULT_MMF_FILE);
	state->setStopFlag(true);
		
	// Wait for config.WaitForStopInMs for each parts to stop
	this_thread::sleep_for(chrono::milliseconds(config.WaitForStopInMs));
	
	// Check that parts are stopped and force them to stop if needed
	// First Orchestrator because it may respawn the parts, but dont kill if it is the current process...
	string strPid = Utils::readFileFirstLine(config.PidFile);
	int opid = -1;
	if(strPid.length() != 0) opid = stoi(strPid);
	if(opid != -1 && opid != getpid())
	{
		bool isAlive = isOrchestratorAlive();
		if(isAlive)
		{
			part_config po;
			po.name = "Orchestrator";
			po.pidfile = config.PidFile;
			PartManager mgro(po);
			bool res = mgro.killPart();
			if(!res)
			{
				cerr << "ERROR: can't kill orchestrator! Do it manually :(" << endl;
				hasError = true;
			}
		}
		cout << "Orchestrator was " << (isAlive ? "ALIVE" : "STOPPED") << " and is now " << (isOrchestratorAlive() ? "ALIVE" : "STOPPED") << endl;
	}
	
	// Then kill parts
	for(part_config p : config.Parts)
	{
		PartManager mgr(p);
		bool isAlive = mgr.isAlive();
		if(isAlive)
		{
			bool res = mgr.killPart();
			if(!res)
			{
				cerr << "ERROR: can't kill part " << p.name <<"! Do it manually :(" << endl;
				hasError = true;
			}
		}
		
		cout << "Part " << p.name << " was " << (isAlive ? "ALIVE" : "STOPPED") << " and is now " << (mgr.isAlive() ? "ALIVE" : "STOPPED") << endl;
	}
	
	// Launch Teardown commands if no errro found
	if(!hasError)
	{
		for(string tdCmd : config.TearDownCommands)
		{
			int ret = Utils::executeShellCommand(tdCmd);
			cout << "Teardown command (" << tdCmd << ") launched with status " << ret << endl;
		}
	}
	
	return !hasError;
}

bool Orchestrator::start()
{
	cout << "\033[36m *** SOATCAR START *** \033[39m" << endl;
	
	// Check if parts are running: exit if it is the case
	bool isSomethingAlive = false;
	if(isOrchestratorAlive())
	{
		cout << "Orchestrator is alive. Stop it before starting." << endl;
		isSomethingAlive = true;
	}

	for(part_config p : config.Parts)
	{
		PartManager mgr(p);
		if(mgr.isAlive())
		{
			cout << "Part " << p.name << " is alive. Stop it before starting." << endl;
			isSomethingAlive = true;
		}
	}
	
	if(isSomethingAlive) return false;
	
	// Set my pid in pidfile
	int mypid = getpid();
	Utils::writeFile(config.PidFile, to_string(mypid));
	
	// Launch setup commands
	for(string stCmd : config.SetupCommands)
	{
		int ret = Utils::executeShellCommand(stCmd);
		cout << "Setup command (" << stCmd << ") launched with status " << ret << endl;
	}
	
	// Init State
	state = new SoatcarState(DEFAULT_MMF_FILE);
	state->setThrottleAuto(config.ThrottleAuto);
	state->setSteeringAuto(config.SteeringAuto);
	state->setMaxThrottleLimit(config.MaxThrottleLimit);
	state->setConstantThrottleActive(config.ConstantThrottleActive);
	state->setConstantThrottleValue(config.ConstantThrottleValue);
	// Host name
	char hostname[64];
	gethostname(hostname, 64);
	state->setHostName(hostname);
	state->setStopFlag(false);
	
	// Launch parts
	for(part_config p : config.Parts)
	{
		PartManager mgr(p);
		int ret = mgr.start();
		if(ret != 0)
		{
			cerr << "ERROR: Part " << p.name << " could not be started (error " << ret << ")" << endl;
			return false;
		}
		
		// Sleep a bit to let the part launch and eventually fail
		this_thread::sleep_for(chrono::milliseconds(500));
		if(!mgr.isAlive())
		{
			cerr << "ERROR: Part " << p.name << " could not be started (not alive)!" << endl;
			return false;
		}
		
		cout << "Part " << p.name << " launched" << endl;
	}
	
	// Loop till the end
	bool partFailed = false;
	while(state->getStopFlag() == 0)
	{
		// Work
		for(part_config p : config.Parts)
		{
			PartManager mgr(p);
			if(!mgr.isAlive())
			{
				cout << "Part " << p.name << " is dead!" << endl;
				partFailed = true;
				// XXX Relaunch? Stop?
				break;
			}
		}
		this_thread::sleep_for(chrono::milliseconds(1000));
	}
	
	if(!partFailed)
	{
		// Launch Teardown commands
		for(string tdCmd : config.TearDownCommands)
		{
			int ret = Utils::executeShellCommand(tdCmd);
			cout << "Teardown command (" << tdCmd << ") launched with status " << ret << endl;
		}
	}
	
	return !partFailed;
}


