/******************************************************************************
Module:  AgsManageProcess.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 19
******************************************************************************/

#include "Windows.h"

#include "AgsManageProcess.h"
#include "AsDefine.h"

//		AgsManageProcess
//	Functions
//		- AgsManageProcess �ʱ�ȭ
//	Arguments
//		- pnServerStatus : ���� ���� ����
//		- pDPModule : DirectPlay server module
//		- pmmModuleManager : module manager
//		- pqueueRecv : receive queue from client
//		- pqueueRecvSvr : receive queue from server
//		- pqueueResult : send queue to client
//		- pqueueResultSvr : send queue to server
//		- nClockInterval : engine clock interval (�� Ŭ�� ������ Idle()�� ȣ��ȴ�)
//	Return value
//		- INT16 : ó������
///////////////////////////////////////////////////////////////////////////////
AgsManageProcess::AgsManageProcess(INT16 *pnServerStatus, AsDP8 *pDPModule, ApModuleManager *pmmModuleManager, 
								   AsCMDQueue* pqueueRecv, AsCMDQueue* pqueueRecvSvr/*, AsCMDQueue* pqueueResult, AsCMDQueue* pqueueResultSvr*/,
								   INT16 nClockInterval, INT16 nDefaultGameProc/*, INT16 nDefaultNotiProc*/)
{
	m_pnServerStatus = pnServerStatus;
	m_pDPModule = pDPModule;
	m_pmmModuleManager = pmmModuleManager;
	m_pqueueRecv = pqueueRecv;
	m_pqueueRecvSvr = pqueueRecvSvr;
	//m_pqueueResult = pqueueResult;
	//m_pqueueResultSvr = pqueueResultSvr;

	m_nNumGameProc		= 0;
	//m_nNumNotiProc		= 0;

	m_nNumTotalProc		= 0;

	m_ulClockCount		= 0;

	m_nClockInterval	= nClockInterval;

	//m_nNumDPClient		= 0;

	m_nDefaultGameProc	= nDefaultGameProc;
	//m_nDefaultNotiProc	= nDefaultNotiProc;
}

AgsManageProcess::~AgsManageProcess()
{
}

//		Process
//	Functions
//		- game process, noti process�� �����Ѵ�.
//		- ServerStatus�� GF_SERVER_STOP �϶����� ��ٸ���. (�̸� game process, noti process�� �����ϰ� ������)
//	Arguments
//		- pvParam : this class (AgsManageProcess) pointer
//	Return value
//		- INT16 : ó������
///////////////////////////////////////////////////////////////////////////////
DWORD WINAPI AgsManageProcess::Process(PVOID pvParam)
{
	AgsManageProcess* pThis = (AgsManageProcess *) pvParam;

	//MessageBox(NULL, "Start ManageProcess", "ManageProcess", 1);

	/*
	pThis->SystemStart();

	if (*pThis->m_pnServerStatus == GF_SERVER_MODULE_START)
	{
		// ���� ������ �����ߴ�. �����Ѱ�� GF_SERVER_ENGINE_START�� ���õȴ�.
		return FALSE;
	}
	*/

	// main engine start...
	pThis->Start();

	*(pThis->m_pnServerStatus) = GF_SERVER_ENGINE_START;

	//MessageBox(NULL, "CreateWaitableTimer()", "ManageProcess", 1);

	// Ÿ�̸� ����
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	LARGE_INTEGER li = { 0 };	// ������ۺ���
	SetWaitableTimer(hTimer, &li, pThis->m_nClockInterval /* milisecond ������ Ÿ�̸� ���� */, 
					 pThis->TimerAPCRoutine, pThis, FALSE);

	//DWORD	dwResult;
	//BOOL	bRetval;
	//MSG		msg;

	for ( ; ; )
	{
		SleepEx(5000, FALSE);

		//dwResult = MsgWaitForMultipleObjects(pThis->m_nNumTotalProc, pThis->m_hWaitHandles, FALSE, INFINITE, QS_ALLINPUT);
		//dwResult = WaitForMultipleObjects(pThis->m_nNumTotalProc, pThis->m_hWaitHandles, FALSE, INFINITE);

		/*
		if (dwResult == (WAIT_OBJECT_0 + pThis->m_nNumTotalProc))
		{
			while ((bRetval = GetMessage(&msg, NULL, 0, 0)) != 0)
			{ 
				if (bRetval == -1)
				{
					// handle the error and possibly exit
				}
				else
				{
					// �޽��� ó��...
					pThis->MessageHandler(&msg);
				}
			}
		}
		*/

		if (*(pThis->m_pnServerStatus) == GF_SERVER_STOP)
		{
			// ���� ����
			pThis->Stop();

			CloseHandle(hTimer);

			return 0;
		}
	}

	return (1);
}

/*
//		SystemStart
//	Functions
//		- ���� ������ ����Ǳ� ���� (Ŭ���̾�Ʈ�� �޾Ƶ��̱� ����) ������ �������� ������ �Ѵ�.
//	Arguments
//		- none
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsManageProcess::SystemStart()
{
	AgsSystemStart	csSystemStart(m_pnServerStatus, m_pDPModule, m_pmmModuleManager, m_pqueueRecvSvr,
								  &m_nNumDPClient, m_pDPClientModule);

	DWORD	ulThreadID;
	HANDLE hThread = CreateThread(NULL,
								  0,
								  csSystemStart.Process,
								  &csSystemStart,
								  0,
								  &ulThreadID);

	if (hThread == NULL)
	{
		// thread creation failed...
	}
	else
	{
		// wait thread stop
		WaitForSingleObject(hThread, INFINITE);
	}

	return TRUE;
}
*/

/*
//		MessageHandler
//	Functions
//		- �ٸ� thread �κ��� �Ѿ�� �޽����� ó���Ѵ�.
//	Arguments
//		- pMsg : �Ѿ�� MSG data pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsManageProcess::MessageHandler(MSG *pMsg)
{
	switch (pMsg->message) {
	case ASDP_CK_CREATE_PLAYER:

		UINT32 ulThreadID;

		if (!SelectThreadForNewPlayer(&ulThreadID))
		{
			return FALSE;
		}

		for ( ; PostThreadMessage(pMsg->wParam, ASDP_CK_CREATE_PLAYER, ulThreadID, NULL) == 0; )
		{
			Sleep(50);
		}

		break;

	case ASDP_CK_DELETE_PLAYER:
		break;

	default:
		break;
	}

	return TRUE;
}

//		SelectThreadForNewPlayer
//	Functions
//		- ���� ������ �÷��̾ ���� game process �� �Ѱ��� thread�� ã�� pulThreadID �� �־��ش�.
//	Arguments
//		- pulThreadID : thread id (return value)
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsManageProcess::SelectThreadForNewPlayer(UINT32 *pulThreadID)
{
	for (int i = 0; i < m_nNumGameProc; i++)
	{
		if (m_nNumProcessClient[i] < AGSGP_MAX_PROCESS_CLIENT_PER_THR)
		{
			*pulThreadID = i;
			m_nNumProcessClient[i]++;

			return TRUE;
		}
	}

	// ---------- Start By Parn
	// game process�� �� ����, thread�� �� á��.
	if (m_nNumGameProc >= AGSGP_MAX_PROCESS_CLIENT_PER_THR)
		return FALSE;
	// ---------- End By Parn

	// game process �� �� á��.
	// ���ο� thread�� �����ؼ� ���ο� Ŭ���̾�Ʈ�� ó���ϰ� �Ѵ�.

	m_stGameProcPool[m_nNumGameProc].hThread = 
		CreateThread(NULL,
						0,
						StartGameProcess,
						this,					// NULL -> this (Edited by Parn)
						0,
						&m_stGameProcPool[m_nNumGameProc].ulThreadID);

	if (m_stGameProcPool[m_nNumGameProc].hThread == NULL)
	{
		// thread creation failed...
	}
	else
	{
		// success thread creation
		//SetSecurityInfo(m_stGameProcPool[m_nNumGameProc].hThread,
		//				SE_KERNEL_OBJECT,
		//				DACL_SECURITY_INFORMATION,
		//				NULL,
		//				NULL,
		//				NULL,
		//				NULL);
		m_hWaitHandles[m_nNumTotalProc] = m_stGameProcPool[m_nNumGameProc].hThread;

		*pulThreadID = m_nNumGameProc;
		m_nNumProcessClient[m_nNumGameProc]++;

		m_nNumGameProc++;
		m_nNumTotalProc++;

		return TRUE;
	}

	return FALSE;
}
*/

//		Start
//	Functions
//		- classify, game, noti process �� �����Ѵ�. 
//			(�ش� process�� �ʱ� thread ������ŭ thread�� �����Ѵ�.)
//	Arguments
//		- none
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AgsManageProcess::Start()
{
	INT32 i;

/* ------ Start By Parn
 * ����� Classify Process�� ������ �ʴ´�. AsDP8���� �ٷ� Recv Queue�� �ִ´�.
	// classify process start...
	for (i = 0; i < AGSMP_DEFAULT_CLASSIFYPROC; i++)
	{
		// classify process thread start...
		/*
		m_stClassifyProcPool[i].hThread = 
			(HANDLE) _beginthreadex(NULL, 
			0, 
			(PTHREAD_START) StartClassifyProcess, 
			NULL, 
			0, 
			(unsigned int *) &m_stClassifyProcPool[i].ulThreadID);
		*
		m_stClassifyProcPool[i].hThread = 
			CreateThread(NULL, 
							0, 
							StartClassifyProcess, 
							this, 					// NULL -> this (Edited by Parn)
							0, 
							&m_stClassifyProcPool[i].ulThreadID);

		if (m_stClassifyProcPool[i].hThread == NULL)
		{
			// thread creation failed...
		}
		else
		{
			// success thread creation
			//m_hWaitHandles[m_nNumTotalProc] = m_stClassifyProcPool[i].hThread;

			m_nNumClassifyProc++;
			m_nNumTotalProc++;
		}
	}
 * ------ End By Parn
 */

	// game process start...
	for (i = 0; i < m_nDefaultGameProc; i++)
	{
		m_stGameProcPool[i].hThread = 
			CreateThread(NULL,
							0,
							StartGameProcess,
							this, 					// &i -> this (Edited by Parn)
							0,
							&m_stGameProcPool[i].ulThreadID);

		if (m_stGameProcPool[i].hThread == NULL)
		{
			// thread creation failed...
		}
		else
		{
			// success thread creation
			//m_hWaitHandles[m_nNumTotalProc] = m_stGameProcPool[i].hThread;

			printf("AgsManageProcess : create AgsGameProcess thread (ThreadID : %d)\n", m_stGameProcPool[i].ulThreadID);

			m_nNumGameProc++;
			m_nNumTotalProc++;
		}
	}

	/*
	// noti process start...
	for (i = 0; i < m_nDefaultNotiProc; i++)
	{
		m_stNotiProcPool[i].hThread = 
			CreateThread(NULL,
							0,
							StartNotiProcess,
							this, 					// NULL -> this (Edited by Parn)
							0,
							&m_stNotiProcPool[i].ulThreadID);

		if (m_stNotiProcPool[i].hThread == NULL)
		{
			// thread creation failed...
		}
		else
		{
			// success thread creation
			//m_hWaitHandles[m_nNumTotalProc] = m_stNotiProcPool[i].hThread;

			m_nNumNotiProc++;
			m_nNumTotalProc++;
		}
	}
	*/

	return TRUE;
}

//		Stop
//	Functions
//		- classify, game, noti process �� �����
//			(�ش� process�� ��� thread�� �����Ѵ�)
//	Arguments
//		- none
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AgsManageProcess::Stop()
{
	/*
	// close classify process thread
	for (int i = 0; i < AGSMP_POOL_CLASSIFYPROC; i++)
	{
		if (m_stClassifyProcPool[i].hThread != NULL)
		{
			CloseHandle(m_stClassifyProcPool[i].hThread);
		}
	}
	m_nNumClassifyProc = 0;
	*/

	// close game process thread
	for (int i = 0; i < AGSMP_POOL_GAMEPROC; i++)
	{
		if (m_stGameProcPool[i].hThread != NULL)
		{
			CloseHandle(m_stGameProcPool[i].hThread);
		}
	}
	m_nNumGameProc = 0;

	/*
	// close noti process thread
	for (i = 0; i < AGSMP_POOL_NOTIPROC; i++)
	{
		if (m_stNotiProcPool[i].hThread != NULL)
		{
			CloseHandle(m_stNotiProcPool[i].hThread);
		}
	}
	m_nNumNotiProc = 0;
	*/

	m_nNumTotalProc = 0;

	/*
	// DPClient stop
	for (i = 0; i < m_nNumDPClient; i++)
	{
		m_pDPClientModule[i].StopDP();
	}

	GlobalFree(m_pDPClientModule);
	*/

	return TRUE;
}

BOOL AgsManageProcess::ResetClockCount(UINT32 ulNewClockCount)
{
	m_ulClockCount = ulNewClockCount;

	return TRUE;
}

BOOL AgsManageProcess::AddClockCount()
{
	m_ulClockCount++;

	return TRUE;
}

UINT32 AgsManageProcess::GetClockCount()
{
	return m_ulClockCount;
}

VOID APIENTRY AgsManageProcess::TimerAPCRoutine(PVOID pvArgToCompletionRoutine,
	DWORD dwTimerLowValue, DWORD dwTimerHighValue)
{
	AgsManageProcess* pThis = (AgsManageProcess *) pvArgToCompletionRoutine;

	MessageBox(NULL, "Enter TimerAPCRoutine", "ManageProcess", 1);

	pThis->AddClockCount();

	//pThis->m_pmmModuleManager->Updates();
	pThis->m_pmmModuleManager->Idle(pThis->GetClockCount());

	MessageBox(NULL, "Update...", "AgsManageProcess", 1);
}

//		thread start routine
///////////////////////////////////////////////////////////////////////////////
/*
DWORD WINAPI AgsManageProcess::StartClassifyProcess(PVOID pvParam)
{
	AgsClassifyProcess	csClassifyProcess;

	return (csClassifyProcess.Start());
}
*/

DWORD WINAPI AgsManageProcess::StartGameProcess(PVOID pvParam)
{
	AgsManageProcess* pThis = (AgsManageProcess *) pvParam;
	INT16	nQueueIndex = *(INT16 *)pvParam;

	AgsGameProcess	csGameProcess(pThis->m_pnServerStatus, nQueueIndex, pThis->m_pDPModule, 
		pThis->m_pqueueRecv, pThis->m_pqueueRecvSvr, pThis->m_pmmModuleManager);

	return (csGameProcess.Process());
}

/*
DWORD WINAPI AgsManageProcess::StartNotiProcess(PVOID pvParam)
{
	AgsManageProcess* pThis = (AgsManageProcess *) pvParam;
	AgsNotiProcess	csNotiProcess(pThis->m_pDPModule, pThis->m_pqueueResult, pThis->m_pqueueResultSvr);

	return (csNotiProcess.ProcessResult());
}
*/