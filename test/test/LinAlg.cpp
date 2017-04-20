#include "LinAlg.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <Windows.h>

char * LinAlg::m_sDefaultExternalMonitorDir = NULL;

bool LinAlg::RunMatlabScript(const char * matlabScriptName,
							 const char * dataPrefix)
{
	std::string cmd(matlabScriptName);
	cmd = cmd + "('" + std::string(dataPrefix) + "');";
	
	return RunMatlab(cmd.c_str());
}

void LinAlg::WaitForFile(const char * fileName, 
						 int checkEveySeconds,
						 int maxSeconds)
{
	if (maxSeconds==-1)
		maxSeconds = 8000;
	bool foundFile = FileExists(fileName);
	
	if (foundFile)
		return;
	
	std::cout<<"Waiting for file: "<<fileName<<std::endl;
	
	time_t currTime = time(NULL);
	time_t terminateTime = currTime + maxSeconds;
	
	int nIter=0;
	while (!foundFile && currTime < terminateTime)
	{
		nIter++;
		if (FileExists(fileName))
			foundFile = true;
		Sleep(checkEveySeconds);
		currTime = time(NULL);	
		if (nIter%10==0)
			std::cout<<(currTime-terminateTime+maxSeconds)<<"s "<<std::flush;
	}
	std::cout<<std::endl;
	if (foundFile)
		std::cout<<"\tFile exists!"<<std::endl;
	else
	{
		std::cout<<"[ERROR] Could not find file: "<<fileName<<std::endl;
		assert(false);
	}
}

bool LinAlg::FileExists(const char * filename)
{
	struct stat st;
	return (stat(filename, &st) == 0);
}

std::string LinAlg::GetMatlabCMD(const char * matlabCommand)
{
	std::string matlabParams = " -nodisplay -nosplash -r \"loadPaths(); ";
	matlabParams = matlabParams + matlabCommand + "; exit;\"";
	
	std::string matlabExe1 = "matlab";
	std::string matlabExe2 = "/usr/local/matlab/bin/matlab";
	
	std::string cmd = std::string("if [ \"`command -v ")+matlabExe1+"`\" != \"\" ]; then ";
	cmd = cmd + matlabExe1 + matlabParams + "; else ";
	cmd = cmd + matlabExe2 + matlabParams + "; fi ";


	return cmd;
}

bool LinAlg::RunMatlab(const char * matlabCommand, 
					   const char * externalMonitorDir)
{
	std::string str = GetMatlabCMD(matlabCommand);
	if (externalMonitorDir==NULL && m_sDefaultExternalMonitorDir==NULL)
	{
		assert(false);
		assert(system(NULL));
		system(str.c_str());
		return true;
	}
	else
	{
		const char * oDir = (externalMonitorDir!=NULL) 
							? externalMonitorDir : m_sDefaultExternalMonitorDir;
		std::string oScript = std::string(oDir) + "MatlabScript0.sh";
		int id = 1;
		char tempChar[10];
		while (FileExists(oScript.c_str()))
		{
			sprintf (tempChar, "%d", id);
			oScript = std::string(oDir) + "MatlabScript" + tempChar+".sh";
			id++;
			assert(id < 10000000);
		}
		
		std::ofstream scriptStream(oScript.c_str());
		assert(scriptStream.is_open());
		scriptStream<<"#!/bin/bash\n";
		scriptStream<<str.c_str()<<"\n";
		scriptStream.close();	
		return false;
	}
}



