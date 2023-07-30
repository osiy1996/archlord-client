// AgsmDBPoolEvents.cpp: implementation of the AgsmDBPoolEvents class.
//
//////////////////////////////////////////////////////////////////////

#include "AgsmDBPoolEvents.h"
#include "ApMemoryTracker.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AgsmDBPoolEvents::AgsmDBPoolEvents()
{
	m_pHandles	= NULL;
	m_pOwner	= NULL;
	m_lEventCount = 0;	

}

AgsmDBPoolEvents::~AgsmDBPoolEvents()
{
	if (m_pHandles) delete [] m_pHandles;
	if (m_pOwner)	delete [] m_pOwner;
}

// DB�� ����Ǿ����� Ȯ���� ���� Event�� �����Ѵ�.
BOOL AgsmDBPoolEvents::Create(INT32 lEventCount)
{
	AuAutoLock Lock(m_Muex);

	// �ڵ��� �Ҵ�Ǿ� ������ ������Ű�� �ʴ´�.
	if (m_pHandles) return FALSE;

	// Event ����
	m_lEventCount = lEventCount;
	m_pHandles	= new HANDLE[m_lEventCount];
	m_pOwner	= new BOOL[m_lEventCount];

	for (INT32 i = 0; i < m_lEventCount; ++i)
	{
		m_pHandles[i]	= ::CreateEvent(NULL, FALSE, FALSE, NULL);
		m_pOwner[i]		= NULL;

		if (NULL == m_pHandles[i]) return FALSE;
	}
	
	return TRUE;
}

INT32 AgsmDBPoolEvents::GetIndex()
{
	AuAutoLock Lock(m_Muex);

	// �����ڰ� ���� Event index�� �˷��ش�.
	for (INT32 i = 0; i < m_lEventCount; ++i)
	{
		if (FALSE == m_pOwner[i])
		{
			m_pOwner[i] = TRUE;
			return i;
		}
	}

	return INVALID_INDEX;
}

BOOL AgsmDBPoolEvents::Wait(DWORD dwTime)
{
	// JNY TODO : ���ϰ��� �м��ؼ� ������ ���ϰ��� �־�� �Ѵ�.
	DWORD dwResult = WaitForMultipleObjects(m_lEventCount, m_pHandles, TRUE, dwTime);
	return TRUE;
}

BOOL AgsmDBPoolEvents::SetDBEvent(INT32 lIndex)
{
	AuAutoLock Lock(m_Muex);

	ASSERT((lIndex < m_lEventCount) && "�ε����� �߸� �����Ǿ����ϴ�.");
	ASSERT((lIndex >= 0) && "�ε����� �߸� �����Ǿ����ϴ�.");
	
	return SetEvent(m_pHandles[lIndex]);
}

BOOL AgsmDBPoolEvents::ResetDBEvent(INT32 lIndex)
{
	AuAutoLock Lock(m_Muex);

	ASSERT((lIndex < m_lEventCount) && "�ε����� �߸� �����Ǿ����ϴ�.");
	ASSERT((lIndex >= 0) && "�ε����� �߸� �����Ǿ����ϴ�.");

	return ResetEvent(m_pHandles[lIndex]);
}