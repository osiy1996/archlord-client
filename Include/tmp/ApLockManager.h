/*
	Notices: Copyright (c) NHN Studio 2004
	Created by: Bryan Jeong (2004/03/24)
*/

/*
	���� ����
	1. 2004.5.10 (������)
		- ApLockManager�� �ߺ� Lock�� �����ϵ��� ����
		- ApMultipleLock ����
*/

// ApLockManager.h: interface for the ApLockManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_)
#define AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ApBase.h"

const INT32 MAX_LOCKTABLE_COUNT = 50;

class ApMultipleLock;
class ApLockManager
{
private:
	struct stLockElement
	{
		ApMutualEx*	pMutex;
		LONG		lCount;

		stLockElement(ApMutualEx* Mutex = NULL) : pMutex(Mutex), lCount(0) {}

		bool operator == (const stLockElement& lhs)
		{
			return ((PVOID)lhs.pMutex == (PVOID)pMutex);
		}
	};

	class FtrCompare
	{
	public:
		bool operator () (const stLockElement& lhs, const stLockElement& rhs)
		{
			return ((PVOID)lhs.pMutex < (PVOID)rhs.pMutex);
		}
	};

private:
	stLockElement	m_LockTable[MAX_LOCKTABLE_COUNT];
	size_t			m_lLockTableCount;

	INT32			m_RecursionBackupCount[MAX_LOCKTABLE_COUNT];
	INT32			m_lRecursionCount;
//	friend class ApMultipleLock;

private:
	ApLockManager();	// singleton�� ���� private�� ��ġ

	BOOL Lock(stLockElement *pLockElement);
	BOOL Unlock(stLockElement *pLockElement);

	void SequenceLock(stLockElement *pLockElement);
	void SequenceUnlock(stLockElement *pLockElement);

	void RecursionLock(stLockElement *pLockElement, INT32 lCount);
	void RecursionUnlock(stLockElement *pLockElement);

	void SequenceUnlockforMultiple(PVOID pValue);
	void SequenceLockforMultiple(PVOID pValue);

	void RemoveCheck(stLockElement *pLockElement);
//	INT32 Insert(ApMutualEx* pValue);

public:
	// TLS singleton
	__declspec (thread) static ApLockManager* pThis;

public:
	virtual ~ApLockManager();
	static ApLockManager* Instance();
	static void DeleteInstance();

	INT32 GetCount();

	BOOL InsertLock(ApMutualEx* pValue);
	void RemoveUnlock(ApMutualEx* pValue);
	void SafeRemoveUnlock(ApMutualEx* pValue);

	BOOL IsExistData();
	void AllUnlock();
};
/*
// �ߺ� Lock ����� �߰��ϸ鼭 ApMultipleLock�� �����Ǿ�� �Ѵ�.
// ������ �ߺ� Lock ����� �����Ϸ��� �߰����� ������尡 �־
// ApMultipleLock�� ȿ�뼺�� ������ �߻��Ͽ� �켱�� �����ϱ�� ����
// 2004.5.10 ������
class ApMultipleLock 
{
private:
	// �ּҰ� ����� ���� ��� ��δ� ��� ������ fuctor
	class FtrSort
	{
	public:
		bool operator () (const ApMultipleLockArray& lhs, const ApMultipleLockArray& rhs)
		{
			return ((PVOID)&lhs->m_Mutex) < ((PVOID)&rhs->m_Mutex);
		}
	};

public:
	ApMultipleLock( ApMultipleLockArray* pArray, INT32 lCount )
	{
		// ���� ���
//		sort(pArray, pArray + lCount, FtrSort());
//		PVOID Min = (PVOID) &pArray[0]->m_Mutex;

		// �ּҰ� ���
		ApMultipleLockArray *pMin = std::min_element(pArray, pArray + lCount, FtrSort());
		PVOID Min = (PVOID)&((*pMin)->m_Mutex);

		// Min�ڿ� �ִ� ����ȭ ��ü���� Release ��Ų��.
		ApLockManager::Instance()->SequenceUnlockforMultiple(Min);

		// LockManager�� ����
		for (INT32 i = 0; i < lCount; ++i)
			ApLockManager::Instance()->Insert( (PVOID) &pArray[i]->m_Mutex );

		// Min�ڿ� �ִ� ����ȭ ��ü���� Lock ��Ų��.
		ApLockManager::Instance()->SequenceLockforMultiple(Min);
	}
};
*/
#endif // !defined(AFX_APLOCKMANAGER_H__030AC40A_EE83_4C11_8942_2BCD27809B51__INCLUDED_)
