/*===================================================

	RelayService.cpp :

===================================================*/

#include "RelayService.h"

/********************************************/
/*		The Implementation of Global		*/
/********************************************/
//
LPTSTR g_szRelayServiceName = _T("AlefRelay");
LPTSTR g_szRelayServiceDisplayName  = _T("Archlord Relay Server");

/****************************************************/
/*		The Implementation of RelayService class		*/
/****************************************************/
//
RelayService::RelayService(LPTSTR pszName, LPTSTR pszDisplay)
	:AuService(pszName, pszDisplay), m_hEventStop(NULL), m_hEventStopConfirm(NULL)
	{
	AuRegistry::ModuleInitialize(_T("ArchLord"));
	}

RelayService::~RelayService()
	{
	}

BOOL RelayService::OnInit(DWORD *pdwStatus)
	{
  #ifdef _DEBUG
	//DebugBreak();
  #endif

	*pdwStatus=0;
	_stprintf(m_szNotifyPath, _T("c:\\%s.log"), m_szName);

	return TRUE;
	}

void RelayService::OnStop()
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
	// clean up
	}

void RelayService::OnPause()
	{
	}

void RelayService::OnContinue()
	{
	}

void RelayService::Run()
	{
	//AllocConsole();

	SYSTEMTIME st;
	GetLocalTime(&st);

	Notify("Service Start Time : %4d-%02d-%02d-%02d %02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

	CoInitialize(NULL);

	CHAR szPath[_MAX_PATH] = {0, };
	m_Registry.Open(g_szRelayServiceName, FALSE);
	UINT32 ulSize = _MAX_PATH;
	m_Registry.ReadString(_T("Path"), szPath, ulSize);
	SetCurrentDirectory(szPath);

	m_hEventStop = CreateEvent(NULL, FALSE, FALSE, NULL);
	m_hEventStopConfirm = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!m_csEngine.OnRegisterModule())
		{
//		WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed OnRegisterModule()");
		CoUninitialize();
		return;
		}

	if (!m_csEngine.StartServer(11004, 4, 3000))
		{
		CoUninitialize();
		return;
		}

	if (!m_csEngine.InitServerManager())
		{
		CoUninitialize();
		return;
		}


	m_csEngine.CreateDBPool();
	Sleep(1000);
	
	if (!m_csEngine.StartProcess())
		{
//		WriteLog(AS_LOG_RELEASE, "AlefAuctionServer : failed StartProcess()");
		CoUninitialize();
		return;
		}

	Sleep( 1000 );	// wait thread creation

	m_csEngine.SetServerStatus(GF_SERVER_START);

	fprintf(stdout, "AlefRelay is started\n");
	fflush(stdout);

	/*
	요거는 데스크탑과 상호작용 선택하면 된다.
	while (CHAR c = getchar())
		{
		BOOL	bExit	= FALSE;

		switch(c)
			{
			case 'm':
				MemoryManager().ReportMemoryInfo();
				printf("ApMemory에서 사용중인 내용들을 AlefRelayServerD.exe_ApMemory.log에 출력했습니다.");
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
	*/
	WaitForSingleObject(m_hEventStop, INFINITE);

	m_csEngine.SetServerStatus(GF_SERVER_STOP);
	m_csEngine.StopProcess();

	m_csEngine.OnTerminate();
	m_csEngine.Stop();

	CloseLog();
    CoUninitialize();
	
	SetEvent(m_hEventStopConfirm);
		
	return;
	}

void RelayService::OnInstall()
	{
	CHAR szPath[_MAX_PATH] = {0, };
	GetCurrentDirectory(_MAX_PATH, szPath);
	m_Registry.Open(g_szRelayServiceName, TRUE);
	m_Registry.WriteString(_T("Path"), szPath);
	}

void RelayService::OnUninstall()
	{
	}

