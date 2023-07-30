#include "PatchServerIOCP.h"
#include "AuIniManager.h"

unsigned short	g_lPatchServerPort = 11000; //��ũ�ε��
unsigned short	g_lPatchSMPort = 11001; //����͸���

void main()
{
	g_AuCircularBuffer.Init(300 * 1024 * 1024);
	g_AuCircularOutBuffer.Init(1024 * 1024);

	CIOCPNetworkModule		cIOCP;
	CIOCPNetworkModule		cIOCPSM;

	//PatchCheckCode�� Ȯ���Ѵ�.
	if( cIOCP.LoadPatchCode( "PatchCode.Dat" ) == false )
	{
		printf( "PatchCode.Dat not found !!!\n" );
		getchar();

		return;
	}

	// Patch Server FTP �ּҸ� �о�´�.
	AuIniManagerA	csIniManager;

	csIniManager.SetPath("PatchDownload.ini");
	if (csIniManager.ReadFile())
	{
		char *			szServerIP	= csIniManager.GetValue("DownloadServer", "IP");
		unsigned short	iServerPort	= csIniManager.GetValueI("DownloadServer", "Port");

		if (szServerIP)
		{
			printf( "Download Server : %s (%d)\n", szServerIP, iServerPort);
			cIOCP.SetDownloadServer(szServerIP, iServerPort);
		}
	}

	cIOCPSM.m_iVersion	= cIOCP.m_iVersion;

	//Create IOCP
	cIOCP.CreateIOCP( 0 );
	cIOCP.SetIPBlocking( false );

	cIOCP.m_cWinSockLib.startupWinsock( 0x0202 );

	cIOCPSM.CreateIOCP( 0 );
	cIOCPSM.SetIPBlocking( false );

	cIOCPSM.m_cWinSockLib.startupWinsock( 0x0202 );

/*	char			strHostName[255];
	char			strIP[255];
	HOSTENT			*pcsHostEnt;

	gethostname( strHostName, sizeof(strHostName) );
	pcsHostEnt = gethostbyname( strHostName );

	sprintf( strIP, "%d.%d.%d.%d", (BYTE)pcsHostEnt->h_addr_list[0][0],
									(BYTE)pcsHostEnt->h_addr_list[0][1],
									(BYTE)pcsHostEnt->h_addr_list[0][2],
									(BYTE)pcsHostEnt->h_addr_list[0][3] );

	printf( "Local IP:%s, Port:%d\n", strIP, g_lPatchServerPort );*/

	if( cIOCP.m_cWinSockLib.initBind( true, NULL, g_lPatchServerPort, 0 ) == false )
	{
		printf( "Bind Error\n" );
		getchar();
		exit(1);
	}

	if( cIOCPSM.m_cWinSockLib.initBind( true, NULL, g_lPatchSMPort, 0 ) == false )
	{
		printf( "Bind Error\n" );
		getchar();
		exit(1);
	}

//	cIOCP.m_cIPBlockingManager.LoadBlockIPFromFile( "IPBlockData.txt" );

	//���ϰ� IOCP�� �����Ѵ�.
	cIOCP.AssociateIOCP( (HANDLE)cIOCP.m_cWinSockLib.m_hSocket, (void *)IOCP_ACCEPTEX );

	cIOCPSM.AssociateIOCP( (HANDLE)cIOCPSM.m_cWinSockLib.m_hSocket, (void *)IOCP_ACCEPTEX );

	//Sleep.... Thread���� �ð����� ��ٷ��ش�. ���߿� Event ������ �����Ѵ�.

	//IOCP�� ���õ� Thread�� ����Ѵ�.
	cIOCP.registerThread( &CIOCPNetworkModule::IOCPGQCSThread );
	cIOCP.registerThread( &CIOCPNetworkModule::IOCPSendThread );
	cIOCP.registerThread( &CIOCPNetworkModule::IOCPRecvThread );

	cIOCPSM.registerThread( &CIOCPNetworkModule::IOCPGQCSThreadSM );
	cIOCPSM.registerThread( &CIOCPNetworkModule::IOCPSendThread );

	//IOCP�� ���õ� Thread�� �����Ѵ�.
	cIOCP.beginRegisteredThread();
	cIOCPSM.beginRegisteredThread();

    printf( "Waiting any Connection...\n" );

	cIOCP.postAccept();
	cIOCPSM.postAccept();

	while (char c = getchar())
	{
		if (c == 'z')
			break;
	}

	cIOCP.cleanup();
	cIOCPSM.cleanup();
}
