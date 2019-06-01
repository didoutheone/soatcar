#include <unistd.h>		// for fork, chdir, execve, dup2, close, STDOUT_FILENO, STDERR_FILENO
#include <errno.h>		// for errno
#include <string>		// for string
#include <cstring>		// for strerror
#include <fcntl.h>		// for open, O_RDWR, O_CREAT
#include <iostream>		// for cout and cerr
#include <signal.h>		// for kill
#include <sys/wait.h>	// for waitpid
#include "PartManager.hpp"
#include "Config.hpp"
#include "Utils.hpp"

using namespace std;

PartManager::PartManager(part_config ppart)
{
	part = ppart;
}

int PartManager::start()
{
	int pid = fork();
	
	if(pid < 0)
	{
		// Error forking
		int errsv = errno;
		cerr << "ERROR: Fork failed with error " << errsv << ": " << strerror(errsv) << endl;
		return -1;
	}
	else if (pid == 0)
	{
		// In the child process
		if((chdir(part.dir.c_str())) == 0)
		{
			int fd;
			if((fd = open(part.logfile.c_str(), O_RDWR | O_CREAT, 0666)) == -1)
			{
				int errsv = errno;
				cerr << "ERROR: Opening logfile '" << part.logfile << "' failed with error " << errsv << ": " << strerror(errsv) << endl;
				exit(-1);
			}
			
			// Set standard and error output to fd
			dup2(fd, STDOUT_FILENO);
			dup2(fd, STDERR_FILENO);
			close(fd);
			
			// split exec by spaces
			vector<char*> args;
			stringstream ss(part.exec);
			string tok;
			while(getline(ss, tok, ' '))
			{
				args.push_back(&tok[0]);
			}
			args.push_back((char*)0);
			
			// now exec command
			execvp(args[0], &args[0]);
			
			// If we are there, execvp has failed
			int errsv = errno;
			cerr << "ERROR: Launching '" << part.exec << "' failed with error " << errsv << ": " << strerror(errsv) << endl;
			exit(-2);
		}
		else
		{
			// Error while changing directory
			int errsv = errno;
			cerr << "ERROR: Changing directory to '" << part.dir << "' failed with error " << errsv << ": " << strerror(errsv) << endl;
			exit(-3);
		}
	}
	else
	{
		// In the parent process with the pid of the children in pid
		int ret = Utils::writeFile(part.pidfile, to_string(pid));
		if(ret != 0)
		{
			cerr << "ERROR: Could not write pid file '" << part.pidfile << "' for part '" << part.name << "': part won't be seen as running!" << endl;
			return -2;
		}
	}
	
	return 0;
}

bool PartManager::isAlive()
{
	// Get pid
	string strpid = Utils::readFileFirstLine(part.pidfile);
	if(strpid.length() == 0)
	{
		return false;
	}
	
	// Child process resources are freed only when wait() is called
	// So the filename we check will exist until wait() is called
	// Conclusion: call wait !
	waitpid(stoi(strpid), NULL, WNOHANG);
	
	// test if process exists in /proc
	stringstream ss;
	ss << "/proc/" << strpid << "/cmdline";
	return Utils::exists(ss.str());
}

bool PartManager::killPart()
{
	// Get pid
	string strpid = Utils::readFileFirstLine(part.pidfile);
	
	if(strpid.length() == 0)
	{
		return false;
	}
	
	int pid = stoi(strpid);
	int res = kill(pid, SIGKILL); 
	
	if(res != 0)
	{
		int errsv = errno;
		cerr << "ERROR: Killing part " << part.name << " with pid " << pid << " failed with error " << errsv << ": " << strerror(errsv) << endl;
		return false;
	}
	
	return true;
}