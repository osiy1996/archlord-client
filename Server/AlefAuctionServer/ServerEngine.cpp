#include <stdio.h>
#include "ServerEngine.h"

AuOLEDBManager		g_csAuOLEDBManager;
AgpmFactors			g_csAgpmFactors;
AgpmItem			g_csAgpmItem;
AgsmServerManager	g_csAgsmServerManager;
AgsmAuctionServer   g_csAgsmAuctionServer;
AgsmAuctionClient	g_csAgsmAuctionClient;
AgsmAuctionServerDB	g_csAgsmAuctionServerDB;

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
	
	if (!g_csAgpmFactors.	AddModule(this))
	{
		ASSERT( !"g_csAgpmFactors ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgpmItem.		AddModule(this))
	{
		ASSERT( !"g_csAgpmItem ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmServerManager.	AddModule(this))
	{
		ASSERT( !"g_csAgsmServerManager ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmAuctionServer.	AddModule(this))
	{
		ASSERT( !"g_csAgsmAuctionServer ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmAuctionClient.	AddModule(this))
	{
		ASSERT( !"g_csAgsmAuctionClient ��Ͻ���" );
		return FALSE;
	}

	if (!g_csAgsmAuctionServerDB.AddModule(this))
	{
		ASSERT( !"g_csAgsmAuctionServerDB ��Ͻ���" );
		return FALSE;
	}

	g_csAgsmServerManager.	SetMaxServer	( 20		);
	g_csAgsmServerManager.	SetMaxTemplate	( 100		);
	g_csAgsmServerManager.	SetMaxIdleEvent	( 100		);

	// module start
	if (!Initialize())
		return FALSE;

	//OLEDBManager�� �ʱ�ȭ���ش�.
	g_csAuOLEDBManager.Init( 10, 1000 );

	g_csAuOLEDBManager.SetStatus( DBTHREAD_ACTIVE );

	//Recruit DB ������ �������� �����带 ����. �켱 10��~ �������. ����
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 0 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 1 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 2 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 3 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 4 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 5 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 6 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 7 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 8 );
	g_csAuOLEDBManager.RegisterThreadFunction( AgsmAuctionServerDB::AuctionDBProcessThread, &g_csAgsmAuctionServerDB, 9 );

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
	MessageBox(NULL, "Module is started", "AlefAuctionServer", 1);

	return TRUE;
}

BOOL ServerEngine::OnTerminate()
{
	return Destroy();
	//return TRUE;
}

BOOL ServerEngine::ConnectServers()
{
	return g_csAgsmServerManager.SetupConnection();
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
