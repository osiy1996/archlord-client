// ApLockManager.cpp: implementation of the ApLockManager class.
//
//////////////////////////////////////////////////////////////////////

#include <ApBase/ApLockManager.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include <algorithm>

using namespace std;

#define DISTANCE(_Ptr) (_Ptr - m_LockTable)
ApLockManager* ApLockManager::pThis = NULL;

ApLockManager::ApLockManager()
{
	ZeroMemory(m_LockTable, sizeof(stLockElement) * MAX_LOCKTABLE_COUNT);
	ZeroMemory(m_RecursionBackupCount, sizeof(INT32) * MAX_LOCKTABLE_COUNT);
	m_lLockTableCount = 0;
	m_lRecursionCount = 0;
}

ApLockManager::~ApLockManager()
{
//	if (pThis) delete pThis;
}

// return singleton instance
ApLockManager* ApLockManager::Instance()
{
	if (!pThis)
	{
		pThis = new ApLockManager;
	}

	return pThis;
}

void ApLockManager::DeleteInstance()
{
	if( pThis )
	{
		delete pThis;
		//@{ 2006/10/18 burumal
		pThis = NULL;
		//@}
	}
}


// ���� lock table�� �ִ� ����ȭ ��ü�� ī��Ʈ
INT32 ApLockManager::GetCount()
{
	return (INT32)m_lLockTableCount;
}

BOOL ApLockManager::Lock(stLockElement *pLockElement)
{
	EnterCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
	++pLockElement->lCount;

	return TRUE;
}

BOOL ApLockManager::Unlock(stLockElement *pLockElement)
{
	LeaveCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
	--pLockElement->lCount;

	return TRUE;	
}

// Insert�� �̿��Ͽ� ������ Lock�� �ɾ��ش�.
BOOL ApLockManager::InsertLock(ApMutualEx* pValue)
{	
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return FALSE;
	//@}

	stLockElement *pLockElement = lower_bound(m_LockTable, m_LockTable + m_lLockTableCount, pValue, FtrCompare());
	BOOL bResult = FALSE;

	if (NULL != (PVOID)pLockElement->pMutex)	
	{
		// pValue�� ���ų� �� ū ���� pValue�� LockTable�� �����Ѵ�.
		if ((PVOID)pLockElement->pMutex == (PVOID)pValue)
		{
			// ��Ȯ�� ���� ApMutualEx�� �����ϴ� ���
			bResult = Lock(pLockElement);	// �̹� Lock �Ǿ� �ִ� ���¶�� SequenceLock/Unlock�� �ʿ����� �ʴ�. 
		}
		else
		{
			// arycoat 2008.10.21
			if( m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 || m_lLockTableCount < 0)
			{
				OutputDebugStr("m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 or m_lLockTableCount < 0");
				return FALSE;
			}

			// pValue���� ū ���� ApMutualEx�� �����ϴ� ��� -> Sequence Lock/Unlock�� �ʿ��ϴ�.
			size_t lDistance = DISTANCE(pLockElement);
			size_t lBytes = (m_lLockTableCount - lDistance);
			lBytes *= sizeof(ApLockManager::stLockElement);
			memmove(pLockElement + 1, pLockElement, lBytes);
			m_LockTable[lDistance] = stLockElement(pValue);
			++m_lLockTableCount;

			SequenceUnlock(pLockElement + 1);
			bResult = Lock(pLockElement);
			SequenceLock(pLockElement + 1);
		}
	}
	else
	{
		// arycoat 2008.10.21
		if( m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 || m_lLockTableCount < 0)
		{
			OutputDebugStr("m_lLockTableCount >= MAX_LOCKTABLE_COUNT-1 or m_lLockTableCount < 0");
			return FALSE;
		}

		// LockTable�� �������� �ʴ� ApMutualEx�̹Ƿ� �������� �߰����Ѵ�.
		m_LockTable[m_lLockTableCount] = stLockElement(pValue);
		bResult = Lock(&m_LockTable[m_lLockTableCount]);
		++m_lLockTableCount;
	}

	return bResult;
}

// �ش� ��Ҹ� release�� �ϰ� locktable���� �����Ѵ�.
void ApLockManager::RemoveUnlock(ApMutualEx* pValue)
{
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return;
	//@}

	stLockElement *pLockElement = find(m_LockTable, m_LockTable + m_lLockTableCount, pValue);
	if (NULL == (PVOID)pLockElement->pMutex) return;

	// ASSERT�� �߻��ϴ� ���� release�� �ߴµ� �Ǵٽ� release�� �� ��찡 ����.
//	ASSERT(LockTable.end() != iter && "Remove�� ���� Mutex ��ü�� �߸��Ǿ����ϴ�.");

	Unlock(pLockElement);
	RemoveCheck(pLockElement);
}

// ApMutualEx�� �����ɶ� LockManager�� ������ �����ϰ� ����
void ApLockManager::SafeRemoveUnlock(ApMutualEx* pValue)
{
	//@{ 2007/01/02 burumal
	if ( !pValue )
		return;
	//@}

	stLockElement *pLockElement = find(m_LockTable, m_LockTable + m_lLockTableCount, pValue);
	if (NULL == (PVOID)pLockElement->pMutex) return;

	Unlock(pLockElement);
	RemoveCheck(pLockElement);
}

// RecursionCount�� 0�� ApMutualEx�� LockTable���� �����Ѵ�.
void ApLockManager::RemoveCheck(stLockElement *pLockElement)
{
	if (0 >= pLockElement->lCount)
	{
		--m_lLockTableCount;
		size_t lDistance = DISTANCE(pLockElement);
		size_t lBytes = (m_lLockTableCount - lDistance);
		lBytes *= sizeof(ApLockManager::stLockElement);		
		memmove(pLockElement, pLockElement + 1, lBytes);
		memset(m_LockTable + m_lLockTableCount, 0x00, sizeof(ApLockManager::stLockElement));
	}
}

// pValue���� �ڿ� �ִ� ��� ��ü�� Lock ���ش�. - multiple������ ���
void ApLockManager::SequenceLockforMultiple(PVOID pValue)
{
//	PtrIter iter = lower_bound(LockTable.begin(), LockTable.end(), pValue, FtrCompare());
//	for_each(iter, LockTable.end(), FtrLock());
}

// pValue���� ���������� release �Ѵ�.
void ApLockManager::SequenceUnlockforMultiple(PVOID pValue)
{
//	PtrIter iter = lower_bound(LockTable.begin(), LockTable.end(), pValue, FtrCompare());
//	for_each(iter, LockTable.end(), FtrUnlock());
}

// iter���� �ڿ� �ִ� ��� ��ü�� RecursionCount��ŭ lock�� ���ش�.
void ApLockManager::SequenceLock(stLockElement *pLockElement)
{
	size_t lDistance = DISTANCE(pLockElement);

	for (size_t i = lDistance; i < m_lLockTableCount; ++i)
	{
		//m_LockTable[i].lCount = m_RecursionBackupCount[i - lDistance];
		RecursionLock(&m_LockTable[i], m_RecursionBackupCount[i - lDistance]);
	}
}

// iter���� ���������� release �Ѵ�.
void ApLockManager::SequenceUnlock(stLockElement *pLockElement)
{
	// LockCount�� Backup �Ѵ�.
	size_t lDistance = DISTANCE(pLockElement);
	for (size_t i = lDistance; i < m_lLockTableCount; ++i)
	{
		m_RecursionBackupCount[i - lDistance] = m_LockTable[i].lCount;
		++m_lRecursionCount;

		// LockCount��ŭ unlock �Ѵ�.
		RecursionUnlock(&m_LockTable[i]);
	}
}

// ���� unlock�� ���� ��ü�� �ִ��� Ȯ���Ѵ�.
BOOL ApLockManager::IsExistData()
{
//	if (m_lLockTableCount > 0)
//	{
//		printf("\nLockTable Count : %d", m_lLockTableCount);
//	}

	return (BOOL)m_lLockTableCount;
}

// release�� ���� ���� ��ü�� ��δ� release���ش�.
void ApLockManager::AllUnlock()
{
	INT32 lCount = (INT32)m_lLockTableCount;
	for (int i = lCount; i > 0; --i)
	{
		RecursionUnlock(&m_LockTable[i - 1]);
		RemoveCheck(&m_LockTable[i - 1]);
	}
//	for_each(LockTable.begin(), LockTable.end(), FtrRecursionUnlock());
//	LockTable.clear();
}

void ApLockManager::RecursionLock(stLockElement *pLockElement, INT32 lCount)
{
	for (int i = 0; i < lCount; ++i)
	{
		EnterCriticalSection(&pLockElement->pMutex->m_csCriticalSection);	
		++pLockElement->lCount;
	}
}

void ApLockManager::RecursionUnlock(stLockElement *pLockElement)
{
	INT32 lCount = pLockElement->lCount;
	for (int i = 0; i < lCount; ++i)
	{
		LeaveCriticalSection(&pLockElement->pMutex->m_csCriticalSection);
		pLockElement->lCount--;
	}
}
