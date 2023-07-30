/*===============================================================

	ServerEngine.cpp

===============================================================*/

#include "ServerEngine.h"
#include "ExceptionHandler.h"

#ifdef _AGSMRELAY2_TEST


/************************************************************/
/*		The Implementation of Dummy test worker class		*/
/************************************************************/
//
TestWorker::TestWorker()
	{
	m_bStop = FALSE;
	m_pRelay = NULL;
	}
	
TestWorker::~TestWorker()
	{
	}

void TestWorker::Stop()
	{
	m_bStop = TRUE;
	}

DWORD TestWorker::Do()
	{
	while (!m_bStop)
		{
		INT16 n = rand() % 10;
		if (m_pRelay)
			m_pRelay->TestLog(n);
		Sleep(100);
		}
	
	return 0;
	}


#endif


/********************************************/
/*		The Implementation of Globals		*/
/********************************************/
//
//AuOLEDBManager*		g_pcsAuOLEDBManager;

ApmMap*				g_pcsApmMap;
AgpmFactors*		g_pcsAgpmFactors;
AgpmCharacter*		g_pcsAgpmCharacter;
AgpmBillInfo*		g_pcsAgpmBillInfo;
AgpmGrid*			g_pcsAgpmGrid;
AgpmItem*			g_pcsAgpmItem;
AgpmGuild*			g_pcsAgpmGuild;			// 이놈 REGISTERMODULE은 안하는 놈이다.
AgpmAdmin*			g_pcsAgpmAdmin;
AgpmLog*			g_pcsAgpmLog;
//AgpmScript*			g_pcsAgpmScript;
AgpmConfig*			g_pcsAgpmConfig;

AgsmAOIFilter*				g_pcsAgsmAOIFilter;
//AgsmDBStream*				g_pcsAgsmDBStream;
AgsmServerManager*			g_pcsAgsmServerManager;
AgsmAdmin*					g_pcsAgsmAdmin;
AgsmInterServerLink*		g_pcsAgsmInterServerLink;

AgsmLog*					g_pcsAgsmLog;
AgsmDatabaseConfig*			g_pcsAgsmDatabaseConfig;
AgsmDatabasePool*			g_pcsAgsmDatabasePool;
AgsmRelay2*					g_pcsAgsmRelay2;
AgsmAuctionRelay*			g_pcsAgsmAuctionRelay;

AgsmConfig*					g_pcsAgsmConfig;
//AsEchoServer*				g_pcsEchoServer;
AgsmServerStatus*			g_pcsAgsmServerStatus;
AgpmSiegeWar				*g_pcsAgpmSiegeWar;

/************************************************/
/*		The Implementation of ServerEngine		*/
/************************************************/
//
ServerEngine::ServerEngine()
	{
	m_pWorker = NULL;
	}

ServerEngine::~ServerEngine()
	{
	}

BOOL ServerEngine::OnRegisterModule()
	{
	g_iServerClientType = ASSM_SERVER_TYPE_RELAY_SERVER;

	// register module
	//g_PacketPool.Initialize(sizeof(AuPacketBuffer), 50000);
	//g_AuCircularBuffer.Init(10 * 1024 * 1024);
	g_AuCircularBuffer.Init(100 * 1024 * 1024);
	g_AuCircularOutBuffer.Init(100 * 1024 * 1024);

	//REGISTER_MODULE(g_pcsAuOLEDBManager, AuOLEDBManager);

	REGISTER_MODULE(g_pcsAgpmConfig, AgpmConfig);
	REGISTER_MODULE(g_pcsApmMap, ApmMap);
	REGISTER_MODULE(g_pcsAgpmFactors, AgpmFactors);
	REGISTER_MODULE(g_pcsAgpmCharacter, AgpmCharacter);
	//REGISTER_MODULE(g_pcsAgpmScript, AgpmScript);
	REGISTER_MODULE(g_pcsAgpmBillInfo, AgpmBillInfo);
	REGISTER_MODULE(g_pcsAgpmGrid, AgpmGrid);
	REGISTER_MODULE(g_pcsAgpmItem, AgpmItem);
	REGISTER_MODULE(g_pcsAgpmAdmin, AgpmAdmin);
	REGISTER_MODULE(g_pcsAgpmLog, AgpmLog);

	REGISTER_MODULE(g_pcsAgsmAOIFilter, AgsmAOIFilter);
	//REGISTER_MODULE(g_pcsAgsmDBStream, AgsmDBStream);
	REGISTER_MODULE(g_pcsAgsmServerManager, AgsmServerManager);
	REGISTER_MODULE(g_pcsAgsmAdmin, AgsmAdmin);
	REGISTER_MODULE(g_pcsAgsmInterServerLink, AgsmInterServerLink);

	REGISTER_MODULE(g_pcsAgsmLog, AgsmLog);
	REGISTER_MODULE(g_pcsAgsmDatabaseConfig, AgsmDatabaseConfig);
	REGISTER_MODULE(g_pcsAgsmDatabasePool, AgsmDatabasePool);	
	REGISTER_MODULE(g_pcsAgsmRelay2, AgsmRelay2);
	REGISTER_MODULE(g_pcsAgsmAuctionRelay, AgsmAuctionRelay);

	REGISTER_MODULE(g_pcsAgsmConfig, AgsmConfig);
	REGISTER_MODULE(g_pcsAgsmServerStatus, AgsmServerStatus);

	// disable idle events
	//g_pcsAgpmCharacter->EnableIdle(FALSE);

	if (!g_pcsAgsmRelay2->SetCallbackOperation())
		return FALSE;

	if (!g_pcsAgsmAuctionRelay->SetCallbackForAuctionServer())
		return FALSE;

	SetMaxCount();

	if (!Initialize())
		return FALSE;

#ifndef _AREA_CHINA_
	if (!g_pcsAgsmServerManager->ReadServerInfo())
	{
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo 실패" );
		return FALSE;
	}
#else
	TCHAR szDSN[30] = {0,};
	TCHAR szUser[30] = {0,};
	TCHAR szPwd[30] = {0,};
	TCHAR szVender[30] = {0,};

	if (!g_pcsAgsmServerManager->StreamReadDBServerInfo("CONFIG\\DBSERVERINFO.ini", TRUE, szDSN, szUser, szPwd, szVender))
	{
		ASSERT( !"g_csAgsmServerManager StreamReadDBServerInfo 실패" );
		return FALSE;
	}

	g_pcsAgsmServerManager->SetConfigDBAuthInfo(szDSN, szUser, szPwd);
	
	if (!g_pcsAgsmServerManager->StreamReadServerInfo(szVender, szDSN, szUser, szPwd, "SERVERCONFIG"))
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "%s(%s) (%s/%s)\n", szDSN, szVender, szUser, szPwd);
		AuLogFile_s("LOG\\StartError.log", strCharBuff);
		
		ASSERT( !"g_csAgsmServerManager StreamReadServerInfo 실패" );
		return FALSE;
	}
#endif

	// 2005.04.07. steeple
	// Server Status IOCP Server Start
	g_pcsAgsmServerStatus->SetServerTypeRelayServer();
	g_pcsAgsmServerStatus->Start();

	return TRUE;
	}

BOOL ServerEngine::SetMaxCount()
	{
	g_pcsAgsmServerManager->SetMaxServer(AGSMSERVER_MAX_NUM_SERVER);
	g_pcsAgsmServerManager->SetMaxIdleEvent(100);
	g_pcsAgsmInterServerLink->SetMaxIdleEvent2(15);

	return TRUE;
	}

BOOL ServerEngine::LoadTemplateData()
	{
	AgsdServer *pAgsdServer = g_pcsAgsmServerManager->GetThisServer();
	if(!pAgsdServer)
		return FALSE;

	TCHAR szQuery[100];
	ZeroMemory(szQuery, sizeof(szQuery));
	
	if (0 == _tcslen(pAgsdServer->m_szDBVender) || 0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("ORACLE")))
		{
		if (AP_SERVICE_AREA_CHINA == g_eServiceArea)
			strcpy(szQuery, "Ini\\DBQuery_CN.ini");
		else
			strcpy(szQuery, "Ini\\DBQuery.ini");
		}
	else if (0 == _tcsicmp(pAgsdServer->m_szDBVender, _T("MSSQL")))
		strcpy(szQuery, "Ini\\DBQuery_MS.ini");
	else
		return FALSE;	

	if (!g_pcsAgsmDatabaseConfig->ReadConfig("Ini\\Database.ini", szQuery))
		{
		ASSERT(!"g_pcsAgsmDatabaseConfi ReadConfig 실패");
		return FALSE;
		}

	return TRUE;
	}

BOOL ServerEngine::OnTerminate()
	{
	return Destroy();
	}

BOOL ServerEngine::InitServerManager()
	{
	if (!g_pcsAgsmServerManager->InitThisServer())
		{
		ASSERT( !"g_csAgsmServerManager InitServerData 실패" );
		return FALSE;
		}
	#ifdef _AREA_CHINA_
	if (!g_pcsAgsmConfig->LoadConfig())
		{
		ASSERT( !"g_pcsAgsmConfig LoadConfig 실패" );
		return FALSE;
		}
	#endif

	if (!LoadTemplateData())
		return FALSE;

	return TRUE;
	}

BOOL ServerEngine::CreateDBPool()
	{
	return g_pcsAgsmDatabasePool->Create();
	}



void ServerEngine::Test(BOOL bStop)
	{
  #ifdef _AGSMRELAY2_TEST
	INT16 nWorker = 10;
	
	if (!bStop)  
		{
		m_pWorker = new TestWorker[10];
		for (INT16 i=0; i< nWorker; i++)
			{
			m_pWorker[i].m_pRelay = g_pcsAgsmRelay2;
			m_pWorker[i].Start();
			}
		}
	else
		{
		if (!m_pWorker)
			return;

		for (INT16 i=0; i< nWorker; i++)
			{
			m_pWorker[i].Stop();
			m_pWorker[i].Wait(100);
			}		
			
		delete [] m_pWorker;
		}
  #endif		
	}
