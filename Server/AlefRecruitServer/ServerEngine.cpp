#include <stdio.h>
#include "ServerEngine.h"

AuOLEDBManager			g_csAuOLEDBManager;
AgsmServerManager		g_csAgsmServerManager;
AgsmConnectionSetupGame	g_csAgsmConnectionSetupGame;
AgsmRecruitServer		g_csAgsmRecruitServer;
AgsmRecruitClient		g_csAgsmRecruitClient;
AgsmRecruitServerDB		g_csAgsmRecruitServerDB;

ServerEngine::ServerEngine(GUID guidApp)
{
	SetGUID(guidApp);
}

ServerEngine::~ServerEngine()
{
}

BOOL ServerEngine::OnRegisterModule()
{

	if (!g_csAuOLEDBManager.	AddModule(this))
	{
		ASSERT( !"g_csAuOLEDBManager ��Ͻ���" );
		return FALSE;
	}
	
	if (!g_csAgsmServerManager.	AddModule(this))
	{
		ASSERT( !"g_csAgsmServerManager ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmConnectionSetupGame.	AddModule(this))
	{
		ASSERT( !"g_csAgsmConnectionSetupGame ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmRecruitServer.	AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitServer ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmRecruitClient.	AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitClient ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmRecruitServerDB.AddModule(this))
	{
		ASSERT( !"g_csAgsmRecruitServerDB ��Ͻ���" );
		return FALSE;
	}

	g_csAgsmServerManager.	SetMaxServer	( 20		);
	g_csAgsmServerManager.	SetMaxTemplate	( 100		);
	g_csAgsmServerManager.	SetMaxIdleEvent	( 100		);

	// module start
	if (!Initialize())
		return FALSE;

	//OLEDBManager�� �ʱ�ȭ���ش�.
	g_csAuOLEDBManager.Init( 10, 10000 );

	g_csAuOLEDBManager.SetStatus( DBTHREAD_ACTIVE );

	//Recruit DB ������ �������� �����带 ����. �켱 10��~ �������. ����
	for( int i=0; i<10; ++i )
	{
		g_csAuOLEDBManager.RegisterThreadFunction( AgsmRecruitServerDB::RecruitDBProcessThread, &g_csAgsmRecruitServerDB, i );
	}

	if (!g_csAgsmServerManager.	StreamReadTemplate("Ini\\ServerTemplate.ini"		)	)
	{
		ASSERT( !"g_csAgsmServerManager StreamReadTemplate ����" );
		return FALSE;
	}

	if (!g_csAgsmServerManager.InitServerData())
	{
		ASSERT( !"g_csAgsmServerManager InitServerData ����" );
		return FALSE;
	}

	// �Ƶ� �տ� ���� �����÷��׸� �����ͼ� �ʿ� ���鿡 �����Ѵ�.
	INT16	nServerFlag = 0;
	INT16	nFlagSize	= 0;
	if (!g_csAgsmServerManager.	GetServerFlag(&nServerFlag, &nFlagSize))
	{
		ASSERT( !"g_csAgsmServerManager.GetServerFlag() ����");
		return FALSE;
	}

	WriteLog(AS_LOG_RELEASE, "ServerEngine : module is started");
	//MessageBox(NULL, "Module is started", "AlefRecruitServer", 1);

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
	//return TRUE;
}

BOOL ServerEngine::ConnectServers()
{
	return g_csAgsmConnectionSetupGame.ConnectGameServers();
	//return g_csAgsmServerManager.SetupConnection();
}

BOOL ServerEngine::DisconnectServers()
{
	return g_csAgsmServerManager.DisconnectAllServers();
}

//DB�������� ���¸� �ٲ۴�. DBó���߿� ������ �ʵ���.....
BOOL ServerEngine::SetDBThreadStatus( int iStatus )
{
	g_csAuOLEDBManager.SetStatus( iStatus );

	return TRUE;
}

//DB ��� �����尡 ������ ��ٸ���.
BOOL ServerEngine::WaitDBThreads()
{
	g_csAuOLEDBManager.WaitThreads();

	return TRUE;
}
