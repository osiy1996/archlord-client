/******************************************************************************
Module:  AsCommonLib.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 04. 19
******************************************************************************/

#include "AsCommonLib.h"

//	AsQueueLIFO class member functions
///////////////////////////////////////////////////////////////////////////////

//		constructor
//	Functions
//		- none
///////////////////////////////////////////////////////////////////////////////
AsQueueLIFO::AsQueueLIFO()
{
}

//		destructor
//	Functions
//		- ó�� ť�� �Ҵ�� �޸𸮸� �����Ѵ�.
///////////////////////////////////////////////////////////////////////////////
AsQueueLIFO::~AsQueueLIFO()
{
	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}
}

//		Initialize
//	Functions
//		- initialize queue member variables
//		- memory allocate
//	Arguments
//		- nQueueSize : 
//		- nDataSize :
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::Initialize(INT16 nQueueSize, INT16 nDataSize)
{
	//m_Mutex.WLock();

	m_nQueueSize = nQueueSize;
	m_nDataSize = nDataSize;

	m_pQueueData = (PVOID) GlobalAlloc(GMEM_FIXED, nDataSize);
	if (m_pQueueData == NULL)
	{
		// memory allocation error. log error
		return FALSE;
	}

	m_nNumData = 0;

	//m_Mutex.Release();

	return TRUE;
}

//		PopQueue
//	Functions
//		- ť���ִ� ����Ÿ�� �� �ڿ� (���߿� ���³�) �ִ� ���� ������.
//	Arguments
//		- pBuffer : ���� ����Ÿ ������ ������
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::PopQueue(PVOID pBuffer)
{
	PROFILE("AsQueueLIFO::PopQueue");

	m_Mutex.WLock();

	if (m_nNumData == 0)
	{
		// queue is empty
		m_Mutex.Release();
		return FALSE;
	}

	CopyMemory(pBuffer, ((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), m_nDataSize);

	m_nNumData--;

	m_Mutex.Release();

	return TRUE;
}

//		PushQueue
//	Functions
//		- queue�� ����Ÿ�� ���� �ִ´�.
//	Arguments
//		- pBuffer : ������� ������
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::PushQueue(PVOID pBuffer)
{
	PROFILE("AsQueueLIFO::PushQueue");

	m_Mutex.WLock();

	if (m_nNumData == m_nQueueSize)
	{
		// queue is full
		m_Mutex.Release();
		return FALSE;
	}

	m_nNumData++;

	CopyMemory(((CHAR *) m_pQueueData) + (m_nDataSize * m_nNumData), pBuffer, m_nDataSize);

	m_Mutex.Release();

	return TRUE;
}

//		ResetQueue
//	Functions
//		- ť�� �ʱ�ȭ�Ѵ�.
//	Arguments
//		- none
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AsQueueLIFO::ResetQueue()
{
	m_Mutex.WLock();

	m_nQueueSize = 0;
	m_nDataSize = 0;

	if (m_pQueueData != NULL)
	{
		GlobalFree(m_pQueueData);
	}

	m_nNumData = 0;

	m_Mutex.Release();

	return TRUE;
}

/*
//	AsIOCP class member functions
///////////////////////////////////////////////////////////////////////////////

//		constructor
//	Functions
//		- i/o completion port ����
///////////////////////////////////////////////////////////////////////////////
AsIOCP::AsIOCP()
{
}

//		destructor
//	Functions
//		- ������ completion port ����
///////////////////////////////////////////////////////////////////////////////
AsIOCP::~AsIOCP()
{
	if (m_hCompPort != NULL)
	{
		CloseHandle(m_hCompPort);
	}
}

//		CreateIoCompletionPort (wrap function)
//	Functions
//		- CreateIoCompletionPort()�� ȣ���� ������ �ڵ��� m_hCompPort�� �ִ´�.
//	Arguments
//		- dwNumberOfConcurrentThreads : ������ worker thread �� ('0'�̸� CPU ������ŭ ����)
//	Return value
//		- TRUE
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::CreateIoCompletionPort(DWORD dwNumberOfConcurrentThreads)
{
	m_hCompPort = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, dwNumberOfConcurrentThreads);

	return TRUE;
}

//		CreateIoCompletionPort (wrap function)
//	Functions
//		- CreateIoCompletionPort()�� ȣ���� ������ �ڵ��� m_hCompPort�� �ִ´�.
//	Arguments
//		- dwNumberOfConcurrentThreads : ������ worker thread �� ('0'�̸� CPU ������ŭ ����)
//	Return value
//		- TRUE
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::AssociateDeviceWithCompletionPort(HANDLE hDevice, DWORD dwCompKey)
{
	HANDLE hTemp = ::CreateIoCompletionPort(hDevice, m_hCompPort, dwCompKey, 0);

	return TRUE;
}

//		GetQueuedCompletionStatus (wrap function)
//	Functions
//		- GetQueuedCompletionStatus()�� ȣ���� �� validate check
//	Arguments
//		- lpNumberOfBytesTransTransferred : ���۵� ����Ʈ ��
//		- lpCompletionKey : ���۵� Ű ����Ÿ
//		- lpOverlapped : ���۵� ����Ÿ ������
//	Return value
//		- TRUE : ����
//		- dwError : ���� ������... ����...
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::GetQueuedCompletionStatus(LPDWORD lpNumberOfBytesTransferred, 
				LPDWORD lpCompletionKey, LPOVERLAPPED *lpOverlapped)
{
	BOOL fOK = ::GetQueuedCompletionStatus(m_hCompPort, lpNumberOfBytesTransferred, 
						lpCompletionKey, lpOverlapped, INFINITE);
	DWORD dwError = GetLastError();

	if (fOK)
	{
		MessageBox(NULL, "Ok... now process", "AsIOCP", 1);
		return TRUE;
	}
	else
	{
		if (lpOverlapped != NULL) {
			MessageBox(NULL, "Process a failed completed I/O request. dwError contains the reason for failure", "AsIOCP", 1);
		} else {
			if (dwError == WAIT_TIMEOUT) {
				MessageBox(NULL, "Time-out while waiting for completed I/O entry", "AsIOCP", 1);
			} else {
				MessageBox(NULL, "Bad call to GetQueuedCompletionStatus. dwError contails the reason for the bad call", "AsIOCP", 1);
			}
		}

		return dwError;
	}
}

//		PostQueuedCompletionStatus (wrap function)
//	Functions
//		- PostQueuedCompletionStatus()�� ȣ���� �� result return
//	Arguments
//		- dwNumBytes : 
//	Return value
//		- PostQueuedCompletionStatus return value
///////////////////////////////////////////////////////////////////////////////
UINT32 AsIOCP::PostQueuedCompletionStatus(DWORD dwNumBytes, ULONG_PTR CompKey, OVERLAPPED* pOverlapped)
{
	return (::PostQueuedCompletionStatus(m_hCompPort, dwNumBytes, CompKey, pOverlapped));
}
*/

//		IsObjectInArea
//	Functions
//		- object�� � ���� �ȿ� �ִ��� �˻��Ѵ�.
//	Arguments
//		- stStartPos : ���� ������
//		- stEndPos : ���� ������
//		- stObjectPos : object ��ġ
//	Return value
//		- TRUE : �����ȿ� ����
//		- FALSE : ���� �ۿ� ����
///////////////////////////////////////////////////////////////////////////////
BOOL IsObjectInArea(AuPOS stStartPos, AuPOS stEndPos, AuPOS stObjectPos)
{
	if ((stStartPos.x <= stObjectPos.x) && (stObjectPos.x <= stEndPos.x) &&
		(stStartPos.y <= stObjectPos.y) && (stObjectPos.y <= stEndPos.y))
		return TRUE;
	else
		return FALSE;
}