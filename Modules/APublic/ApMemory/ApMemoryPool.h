/*
	Notices: Copyright (c) NHN Studio 2002
	Created by: Bryan Jeong (2003/11/21)
*/

#if !defined(__APMEMORYPOOL_H__)
#define __APMEMORYPOOL_H__

#include "ApMemoryManager.h"

/**************************************************************************
ApMemory�� ���� Ŭ������ �����Ͽ� ������ Ÿ�ӿ��� ����� Ȯ���Ҽ� ���� ���
����Ҽ� �ִ�.

  Initialize�� ȣ���Ͽ� lDataSize�� lArraySize�� ȣ���Ͽ� ����Ѵ�.
  Initialize�� ���� ������ Alloc, Free�� �����Ҽ� ����.
**************************************************************************/

class ApMemoryPool
{
private:
	ApCriticalSection	m_Mutex;
	INT32				m_lTypeIndex;
	
public:
	ApMemoryPool() : m_lTypeIndex(-1) {}
	virtual ~ApMemoryPool() {}

	VOID Initialize(INT32 lDataSize, INT32 lArraySize, CHAR* pTypeName = NULL)
	{
		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		m_lTypeIndex = ApMemoryManager::GetInstance().AddType(lDataSize, lArraySize, pTypeName);
	}
	
	PVOID Alloc()
	{
		if (-1 == m_lTypeIndex) return NULL;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return NULL;
		return ApMemoryManager::GetInstance().NewObject(m_lTypeIndex);
	}

	VOID Free(PVOID pObject)
	{
		if (-1 == m_lTypeIndex) return;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return;
		ApMemoryManager::GetInstance().DeleteObject(m_lTypeIndex, pObject);
	}

	INT32 GetAllocatedCount()
	{
		if (-1 == m_lTypeIndex) return -1;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return 0;
		return ApMemoryManager::GetInstance().GetAllocatedCount(m_lTypeIndex);
	}

	BOOL IsEmptyPool()
	{
		if (-1 == m_lTypeIndex) return FALSE;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return FALSE;

		return (ApMemoryManager::GetInstance().m_lAllocCount[m_lTypeIndex] >= ApMemoryManager::GetInstance().m_lArraySize[m_lTypeIndex]);
	}

	INT32 GetRemainCount()
	{
		if (-1 == m_lTypeIndex) return FALSE;

		AuAutoLock Lock(m_Mutex);
		if (!Lock.Result()) return 0;

		return ApMemoryManager::GetInstance().m_lArraySize[m_lTypeIndex] - ApMemoryManager::GetInstance().m_lAllocCount[m_lTypeIndex];
	}
	
	inline INT32 GetTypeIndex()
	{
		return m_lTypeIndex;
	}
};

#endif	// __APMEMORYPOOL_H__