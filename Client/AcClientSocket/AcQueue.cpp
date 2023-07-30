// AcQueue.cpp: implementation of the AcQueue class.
//
//////////////////////////////////////////////////////////////////////

#include <ApBase/ApBase.h>
#include "AcQueue.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcQueue g_AcReceiveQueue;

AcQueue::AcQueue()
{
}

AcQueue::~AcQueue()
{
	AuAutoLock Lock(m_csMutex);
	if (Lock.Result())
	{
		// Queue�� �����ִ� Data�� ��� �����Ѵ�.
		while (!m_stlQueue.empty())
		{
			AcPacketData* pQueueData = m_stlQueue.front();
			m_stlQueue.pop();

	//		delete pQueueData;
		}
	}
}

VOID AcQueue::Push(AcPacketData* pQueueData)
{
	AuAutoLock Lock(m_csMutex);
	if (!Lock.Result()) return;

	m_stlQueue.push(pQueueData);
}

AcPacketData* AcQueue::Pop()
{
	AuAutoLock Lock(m_csMutex);
	if (!Lock.Result()) return NULL;
	
	// 2004.05.14. steeple �߰�
	// Queue Size �� 1 ���� ������ return NULL
	if(GetCount() < 1)
		return NULL;

	AcPacketData *pQueueData = m_stlQueue.front();
	if (pQueueData)
	{
		m_stlQueue.pop();	// �����Ͱ� �����ϸ� Queue���� �����ϰ� ���� �����Ѵ�.
		return pQueueData;
	}
	else
	{
		return NULL;
	}
}

BOOL AcQueue::IsEmpty()
{
	// Queue�� ����ִ��� �˻�
	return m_stlQueue.empty();
}

INT32 AcQueue::GetCount()
{
	return (INT32)m_stlQueue.size();
}