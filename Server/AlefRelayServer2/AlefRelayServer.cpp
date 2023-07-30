/******************************************************************************
Module:  AlefRelayServer.cpp
Notices: Copyright (c) NHN Studio 2004 
Purpose: 
Last Update: 2004. 02. 03
******************************************************************************/

#include "ApBase.h"
#include <stdio.h>
#include <objbase.h>
#include "ServerEngine.h"
#include "RelayService.h"
#include "ApMemory.h"
#include "ApMemoryChecker.h"
#include "ApMemoryTracker.h"
//#define _ALEF_SERVICE_MODE


void DoConsole();

int main(int argc, char* argv[])
	{

	// Check Process is Running(Allow only one Server) 
	if(::IsProcessRun("RelayServer"))
	{
		printf("This Program is already Running!!!");
		exit(1);
	}

  // Console(Non-service) mode
  #ifndef		_ALEF_SERVICE_MODE
	DoConsole();

  // Service mode
  #else		

	RelayService RelayService(g_szRelayServiceName, g_szRelayServiceDisplayName);
	if (FALSE == RelayService.ParseArguments(argc,argv))
		{
		RelayService.Start();
		}	

  #endif
  
#ifdef	_AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportLeaks(REPORT_MODE_FILE);
	ApMemoryTracker::DestroyInstance();
#endif	//_AP_MEMORY_TRACKER_
	
	return 0;
	}

void DoConsole()
	{
	// Init COM so we can use CoCreateInstance
	CoInitialize(NULL);

	ServerEngine serverEngine;	

	serverEngine.SetCrashReportExceptionHandler();

	if (!serverEngine.OnRegisterModule())
		{
//		WriteLog(AS_LOG_RELEASE, "AlefRelay2 : failed OnRegisterModule()");
		CoUninitialize();
		return;
		}

	if (!serverEngine.StartServer(11004, 4, 3000))
		{
		CoUninitialize();
		return;
		}
	
	if (!serverEngine.InitServerManager())
		{
		CoUninitialize();
		return;
		}

	if (!serverEngine.CreateDBPool())
		{
//		WriteLog(AS_LOG_RELEASE, "AlefRelay2 : failed StartProcess()");
		CoUninitialize();
		return;
		}

	Sleep( 1000 );	// wait thread creation

	if (!serverEngine.StartProcess())
		{
//		WriteLog(AS_LOG_RELEASE, "AlefRelay2 : failed StartProcess()");
		CoUninitialize();
		return;
		}

	Sleep( 1000 );	// wait thread creation

	serverEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefRelay2 is started\n");
	fflush(stdout);

	while (CHAR c = getchar())
		{
		BOOL	bExit	= FALSE;

		switch(c)
			{
			case 'm':
				ApMemoryManager::GetInstance().ReportMemoryInfo();
				printf("ApMemory에서 사용중인 내용들을 AlefRelayServerD.exe_ApMemory.log에 출력했습니다.\n");
				break;
			
			
			case 'b':
				serverEngine.Test(FALSE);
				break;
			
			case 'v':
				serverEngine.Test(TRUE);
				break;
				
			case 'z':
				bExit = TRUE;
				break;

			default:
				break;
			}

		if (bExit)
			break;
		}

	serverEngine.SetServerStatus(GF_SERVER_STOP);
	serverEngine.StopProcess();

	serverEngine.OnTerminate();
	serverEngine.Stop();

	CloseLog();
    CoUninitialize();
	}