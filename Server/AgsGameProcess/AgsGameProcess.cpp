/******************************************************************************
Module:  AgsClientProcess.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 22
******************************************************************************/

#include "ApPacket.h"

#include "AgsGameProcess.h"
#include "AsDP8.h"

/*
#define _WIN32_WINNT	0x0500
#define	WINVER			0x0500
*/

//	constructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::AgsGameProcess(INT16 *pnServerStatus, INT16 nQueueIndex, AsDP8 *pDPModule, 
							   AsCMDQueue *pqueueRecv, AsCMDQueue *pqueueRecvSvr, ApModuleManager *pmmModuleManager)
{
	m_pnServerStatus = pnServerStatus;

	m_pDPModule = pDPModule;

	m_nQueueIndex = nQueueIndex;
	m_pqueueRecv = pqueueRecv;
	m_pqueueRecvSvr = pqueueRecvSvr;
	m_pmmModuleManager = pmmModuleManager;

	/*
	if (nQueueIndex == 0)
		m_pqueueNewClient->Initialize(AGSCP_CMD_QUEUE_SIZE);

	m_pqueueGameProc[m_nQueueIndex]->Initialize(AGSCP_CMD_QUEUE_SIZE);
	*/

	m_pqueueRecv->Initialize(AGSGP_RECV_CMD_QUEUE_SIZE, sizeof(stCOMMAND));

	m_nNumClient = 0;

	/*
	// ��� ManageProcess ���� �ϰ������� ó�����ش�. ���⼱ ���� �� �ʿ� ����.
	m_hClockTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	LARGE_INTEGER li = { 0 };
	SetWaitableTimer(m_hClockTimer, &li, AGSGP_PROCESS_CYCYE_TIME, UpdateAPCRoutine, NULL, FALSE);
	*/
}

//	destructor
///////////////////////////////////////////////////////////////////////////////
AgsGameProcess::~AgsGameProcess()
{
	CloseHandle(m_hClockTimer);
}

//		Process
//	Functions
//		- receive queue�� ������ ����Ÿ�� ó���Ѵ�.
//	Arguments
//		- none
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsGameProcess::Process()
{
	INT16	nProcessCommand = 0;

	UINT32	ulThreadID = GetCurrentThreadId();

	printf("AgsGameProcess : (thread_id : %d) enter thread main loop\n", ulThreadID);

	for ( ; ; )
	{
		nProcessCommand += ProcessServer();
		nProcessCommand += ProcessClient();

		if (!nProcessCommand)
			Sleep(50);

		nProcessCommand = 0;
	}

	return TRUE;
}

BOOL AgsGameProcess::AddNewServer(DPNID dpnidServer)
{
	return TRUE;
}

BOOL AgsGameProcess::RemoveServer(DPNID dpnidServer)
{
	return TRUE;
}

//		ProcessServer
//	Functions
//		- command queue�κ��� ����Ÿ�� ������ ó���� �Ѵ�.
//			1. ���� ������ ���� ó���� �Ѵ�.
//			2. ���� ����� ���� ó���� �Ѵ�.
//			3. �׿��� ����Ÿ�� ���޴����� ���� ���� �ѱ��.
//	Arguments
//		- none
//	Return value
//		- INT16 : ó���� command ����
///////////////////////////////////////////////////////////////////////////////
INT16 AgsGameProcess::ProcessServer()
{
	for (int i = 0; i < 5; i++)
	{
		stCOMMANDSVR	stCommandSvr;

		if (!m_pqueueRecvSvr->PopCommand(&stCommandSvr))
			return i;

		printf("AgsGameProcess : (thread_id : %d) receive server data from dplay\n", GetCurrentThreadId());

		switch (stCommandSvr.unType) {
		case ASDP_CK_CREATE_PLAYER:

			AddNewServer(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer);

			/*
			// ���ο� ������ �����ߴٴ� ���������θ� ���Ǵ� ��Ŷ�� ����� ť�� �ִ´�.
			PACKET_INTERNAL_NEW_SERVER	packetNewServer;

			packetNewServer.cType = PACKET_TYPE_INTERNAL;
			packetNewServer.cAction = PACKET_ACTION_INTERNAL_NEW_SERVER;
			packetNewServer.dpnidServer = ((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer;
			*/

			break;

		case ASDP_CK_DELETE_PLAYER:

			RemoveServer(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->dpnidPlayer);

			if (((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->lID != AP_INVALID_SERVERID)
				m_pmmModuleManager->DisconnectSvr(((AS_APP_PLAYER_INFO *) stCommandSvr.pvServer)->lID);

			break;

		case ASDP_CK_RECEIVE_DATA:

			m_pmmModuleManager->Dispatch(stCommandSvr.szData, stCommandSvr.unDataSize, stCommandSvr.dpnid);
			break;

		default:
			break;
		};
	}

	return i;
}

BOOL AgsGameProcess::AddNewClient(DPNID dpnidPlayer)
{
	if (*m_pnServerStatus != GF_SERVER_START)
	{
		m_pDPModule->DestroyClient(dpnidPlayer);
	}

	//MessageBox(NULL, "add new client", "Game Process", 1);

	return TRUE;
}

BOOL AgsGameProcess::RemoveClient(DPNID dpnidPlayer)
{
	return TRUE;
}

//		ProcessClient
//	Functions
//		- command queue�κ��� ����Ÿ�� ������ ó���� �Ѵ�.
//			1. ���� ���� ĳ���� ó���� �Ѵ�.
//			2. ���� ����� ĳ���� ó���� �Ѵ�.
//			3. �׿��� ����Ÿ�� ���޴����� ���� ���� �ѱ��.
//	Arguments
//		- none
//	Return value
//		- INT16 : ó���� command ����
///////////////////////////////////////////////////////////////////////////////
INT16 AgsGameProcess::ProcessClient()
{
	for (int i = 0; i < 5; i++)
	{
		stCOMMAND	stCommand;

		//while (!m_pqueueRecv->PopCommand(&stCommand))
		//{
		//	Sleep(50);
		//}

		if (!m_pqueueRecv->PopCommand(&stCommand))
			return i;

		printf("AgsGameProcess : (thread_id : %d) receive client data from dplay\n", GetCurrentThreadId());

		switch (stCommand.unType) {
		case ASDP_CK_CREATE_PLAYER:

			AddNewClient(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->dpnidPlayer);

			// �ʿ��ϴٸ� ���ο� Ŭ���̾�Ʈ�� �����ߴٴ� ���������θ� ���Ǵ� ��Ŷ�� ����� ť�� �ִ´�.

			break;

		case ASDP_CK_DELETE_PLAYER:

			RemoveClient(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->dpnidPlayer);

			if (((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->lID != AP_INVALID_CID)
				m_pmmModuleManager->Disconnect(((AS_APP_PLAYER_INFO *) stCommand.pvCharacter)->lID);
			
			break;

		case ASDP_CK_RECEIVE_DATA:

			//MessageBox(NULL, "Receive Data From DP. Process Dispatch()", "AgsGameProcess", 1);
			//printf("AgsGameProcess : (thread_id : %d) receive data from dplay\n", GetCurrentThreadId());

			printf("AgsGameProcess : (thread_id : %d) dispatch() received data\n", GetCurrentThreadId());
			//MessageBox(NULL, "received data from DP.", "AgsGameProcess", 1);
			m_pmmModuleManager->Dispatch(stCommand.szData, stCommand.unDataSize, stCommand.dpnid);
			break;

		default:
			break;
		};
	}

	return i;
}

/*
//		UpdateAPCRoutine
//	Functions
//		- //process clock ���� ����� Updates() �Լ��� ȣ�����ش�.
//		- character�� �� ���µ��� ������ ���¸� ������Ʈ �Ѵ�.
//	Arguments
//		- pvArgToCompletionRoutine
//		- dwTimerLowValue
//		- dwTimerHighValue
//	Return value
//		- VOID
///////////////////////////////////////////////////////////////////////////////
VOID APIENTRY AgsGameProcess::UpdateAPCRoutine(PVOID pvArgToCompletionRoutine, 
											   DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	//m_pmmModuleManager->Updates();

	return;
}
*/