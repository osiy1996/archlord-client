// ApRWLock.h
// (C) NHN Games - ArchLord Development Team
// steeple, 2007. 04. 23.

#include <ApBase/ApRWLock.h>

//#define DONT_USE_RW_LOCK		1

ApRWLock::ApRWLock() : m_hNobodyIsReading(NULL),
						m_hWritingMutex(NULL),
						m_nReaders(0),
						m_bInit(0)
{
	Initialize();
}

ApRWLock::ApRWLock(const ApRWLock& param) : m_hNobodyIsReading(NULL),
											m_hWritingMutex(NULL),
											m_nReaders(0),
											m_bInit(0)
{
	// Copy Constructor 
	// �̳� ���� ���ؽ��� �ʱ�ȭ �Ѵ�.
	// �̺κ��� �Ͼ�� ���� ApWorldSector.cpp CreateDimension() ������
	// �Ͼ�� �Ѵ�.

	// ó�� vector�� �����Ҷ� ��� �Ҹ��鼭
	// �ʱ�ȭ�� �ٽõŰ� ������ �ı�ȴ�.
	Initialize();
}

void ApRWLock::Initialize()
{
#ifdef DONT_USE_RW_LOCK

	if(m_bInit)
		return;

	InitializeCriticalSection(&m_csReading);
	m_bInit = TRUE;
	
#else

	if( m_hNobodyIsReading || m_hWritingMutex ) return;

	// Reading Event
	m_hNobodyIsReading = CreateEvent(NULL, TRUE, true, NULL);

	// Writing Mutex
	m_hWritingMutex = CreateMutex(NULL, false, NULL);

	ASSERT(m_hNobodyIsReading && m_hWritingMutex);

	// to Manage m_nReaders
	InitializeCriticalSection(&m_csReading);
	
	m_nReaders = 0;

	m_bInit = TRUE;

#endif
}

ApRWLock::~ApRWLock()
{
#ifdef DONT_USE_RW_LOCK

	DeleteCriticalSection(&m_csReading);
	m_bInit = FALSE;

#else

	if(m_hNobodyIsReading || m_hWritingMutex )
	{
		CloseHandle(m_hNobodyIsReading);
		CloseHandle(m_hWritingMutex);
		DeleteCriticalSection(&m_csReading);

		ASSERT(m_nReaders == 0);

		m_bInit = FALSE;
	}

#endif
}

bool ApRWLock::LockReader(DWORD dwTimeOut)
{
#ifdef DONT_USE_RW_LOCK

	::EnterCriticalSection(&m_csReading);
	return true;

#else

	// Writer Lock ����Ѵ�.
	if(WaitForSingleObject(m_hWritingMutex, dwTimeOut) != WAIT_OBJECT_0)
		return false;

	// Reader Lock Count �� ���� CS
	::EnterCriticalSection(&m_csReading);
	{
		++m_nReaders;
		if(m_nReaders == 1)
		{
			// Reader Lock �ɷȴٰ� �˷��ش�.
			ResetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);

	// Writer Lock Ǯ���ش�.
	ReleaseMutex(m_hWritingMutex);
	
	return true;

#endif
}

void ApRWLock::UnlockReader()
{
#ifdef DONT_USE_RW_LOCK

	::LeaveCriticalSection(&m_csReading);

#else

	::EnterCriticalSection(&m_csReading);
	{
		--m_nReaders;
		ASSERT(m_nReaders >= 0);
		if(m_nReaders == 0)
		{
			// Reader Lock Ǯ���ش�.
			SetEvent(m_hNobodyIsReading);
		}
	}
	::LeaveCriticalSection(&m_csReading);

#endif
}

bool ApRWLock::LockWriter(DWORD dwTimeOut)
{
#ifdef DONT_USE_RW_LOCK

	::EnterCriticalSection(&m_csReading);
	return true;

#else

	// Writer Lock üũ
	if(WaitForSingleObject(m_hWritingMutex, dwTimeOut) != WAIT_OBJECT_0)
		return false;

	// Reader Lock �� üũ
	if(WaitForSingleObject(m_hNobodyIsReading, dwTimeOut) != WAIT_OBJECT_0)
	{
		// �̷� �� ������ �� ������ Writer Lock Ǯ���ش�.
		ReleaseMutex(m_hWritingMutex);
		return false;
	}
	
	return true;

#endif
}

void ApRWLock::UnlockWriter()
{
#ifdef DONT_USE_RW_LOCK

	::LeaveCriticalSection(&m_csReading);

#else

	// Writer Lock Ǯ���ش�.
	ReleaseMutex(m_hWritingMutex);

#endif
}


