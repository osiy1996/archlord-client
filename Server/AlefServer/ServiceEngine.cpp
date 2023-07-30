/*===================================================

	ServiceEngine.cpp :

===================================================*/

#include "ServiceEngine.h"

/********************************************/
/*		The Implementation of Global		*/
/********************************************/
//
GUID	g_guidApp = { 0xede9493e, 0x6ac8, 0x4f15, { 0x8d, 0x1, 0x8b, 0x16, 0x32, 0x0, 0xb9, 0x66 } };

LPTSTR	g_szServiceEngineName = _T("AlefGame");
LPTSTR	g_szServiceEngineDisplayName  = _T("Archlord Game Server");

/****************************************************/
/*		The Implementation of ServiceEngine class		*/
/****************************************************/
//
ServiceEngine::ServiceEngine(LPTSTR pszName, LPTSTR pszDisplay)
	:AuService(pszName, pszDisplay), m_csEngine(g_guidApp), m_hEventStop(NULL), m_hEventStopConfirm(NULL)
	{
	AuRegistry::ModuleInitialize(_T("ArchLord"));
	}

ServiceEngine::~ServiceEngine()
	{
	}

BOOL ServiceEngine::OnInit(DWORD *pdwStatus)
	{
  #ifdef _DEBUG
	//DebugBreak();
  #endif

	*pdwStatus=0;
	_stprintf(m_szNotifyPath, _T("c:\\%s.log"), m_szName);

	return TRUE;
	}

void ServiceEngine::OnStop()
	{
	ASSERT(NULL != m_hEventStop);
	//PostMessage(m_hwnd, WM_QUIT, (WPARAM)0, NULL);
	SetEvent(m_hEventStop);

	// 20초 기다려도 안죽으면 자살
	DWORD dwWait = WaitForSingleObject(m_hEventStopConfirm, 20 * 1000);

	if (WAIT_TIMEOUT == dwWait)
		{
		CloseHandle(m_hEventStop);
		CloseHandle(m_hEventStopConfirm);	
		ExitProcess(99);
		}
	
	CloseHandle(m_hEventStop);
	CloseHandle(m_hEventStopConfirm);
	}

void ServiceEngine::OnPause()
	{
	}

void ServiceEngine::OnContinue()
	{
	}

void ServiceEngine::Run()
	{
	//AllocConsole();

	SYSTEMTIME st;
	GetLocalTime(&st);

	Notify("Service Start Time : %4d-%02d-%02d-%02d %02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

#ifndef _WIN64
	CoInitializeEx( NULL, COINIT_MULTITHREADED );
#endif

	CHAR szPath[_MAX_PATH] = {0, };
	m_Registry.Open(g_szServiceEngineName, FALSE);
	UINT32 ulSize = _MAX_PATH;
	m_Registry.ReadString(_T("Path"), szPath, ulSize);
	SetCurrentDirectory(szPath);

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventStopConfirm = CreateEvent(NULL, FALSE, FALSE, NULL);

#ifndef _M_X64
	int	iSSE2Enable	= _set_SSE2_enable(1);
	if (iSSE2Enable)
		printf("SSE2 enabled. \n");
	else
		printf("SSE2 not enabled; processor does not supoort SSE2.\n");
#endif

	//OpenLog(AS_LOG_FILE);
	OpenLog(AS_LOG_CONSOLE);

	m_csEngine.SetCrashReportExceptionHandler();

	m_csEngine.SetClockInterval(200);
	m_csEngine.SetNumInitGameThreads(2);

	SYSTEM_INFO		stSystemInfo;
	GetSystemInfo(&stSystemInfo);

	//if (!serverEngine.StartDP(9998, 5000, ASDP_CONNECT_RETRIES, 5000))
	if (!m_csEngine.StartServer(11008, stSystemInfo.dwNumberOfProcessors, 5000, NULL, NULL, 100))
	{
		CoUninitialize();
		return;
	}

	if (!m_csEngine.OnRegisterModule())
	{
//		TRACEFILE("AlefServer : failed OnRegisterModule()");

		CoUninitialize();
		return;
	}

	if (!m_csEngine.StartProcess())
	{
//		TRACEFILE("AlefServer : failed StartProcess()");

		CoUninitialize();
		return;
	}

	//if (!m_csEngine.StartDBProcess())
	//{
	//	WriteLog(AS_LOG_RELEASE, "AlefServer : failed StartDBProcess()");

	//	CoUninitialize();
	//	return 0;
	//}

	if (!m_csEngine.ConnectRelayServer())
	{
		CoUninitialize();
		return;
	}

	m_csEngine.ConnectServers();

	/*
	if (!m_csEngine.ConnectServers())
	{
		WriteLog(AS_LOG_RELEASE, "AlefServer : failed ConnectServers()");

		CoUninitialize();
		
		// DB 연결 해제하고 나감. 2004.02.17. steeple
		m_csEngine.StopDBProcess();

		return 0;
	}
	*/

/*
	INT16	nServerStatus = m_csEngine.GetServerStatus();
	while (nServerStatus != GF_SERVER_COMPLETE_CONNECTION && nServerStatus != GF_SERVER_START)
	{
		Sleep(1000);
		nServerStatus = m_csEngine.GetServerStatus();
	}
*/
	m_csEngine.SetServerStatus(GF_SERVER_START);

	printf("AlefServer is started\n");

	/*
	fprintf(stdout, "AlefServer is started\n");
	fflush(stdout);
	*/

	HANDLE	m_ahEventStop[2];
	m_ahEventStop[0]	= m_hEventStop;
	m_ahEventStop[1]	= m_csEngine.m_hEventStop;

	WaitForMultipleObjects(2, m_ahEventStop, FALSE, INFINITE);

	m_csEngine.Shutdown();

#ifdef	_AP_MEMORY_TRACKER_
	ApMemoryTracker::GetInstance().ReportViolatioinGuardBytes(REPORT_MODE_FILE);

	ApMemoryTracker::DestroyInstance();
#endif	//_AP_MEMORY_TRACKER_

	CloseLog();
    CoUninitialize();
	
	SetEvent(m_hEventStopConfirm);
		
	return;
	}

void ServiceEngine::OnInstall()
	{
	CHAR szPath[_MAX_PATH] = {0, };
	GetCurrentDirectory(_MAX_PATH, szPath);
	m_Registry.Open(g_szServiceEngineName, TRUE);
	m_Registry.WriteString(_T("Path"), szPath);
	}

void ServiceEngine::OnUninstall()
	{
	}

