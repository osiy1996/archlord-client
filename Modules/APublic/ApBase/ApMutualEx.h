/******************************************************************************
Module:  ApMutualEx.h
Notices: Copyright (c) 2002 netong (Copyright (c) 2000 Jeffrey Richter)
Purpose: 
Last Update: 2002. 04. 02
******************************************************************************/

/*
	Notices: Copyright (c) NHN Studio 2004
	Modified by: Bryan Jeong (2004/03/24)
*/

#if !defined(__APMUTUALEX_H__)
#define __APMUTUALEX_H__

#include <winsock2.h>
#include <windows.h>
#include <winbase.h>
/******************************************************************************
Module name: Optex.h
Written by:  Jeffrey Richter
******************************************************************************/

#include "assert.h"

#pragma once

///////////////////////////////////////////////////////////////////////////////
#pragma warning( disable : 4786 )

class ApCriticalSection
{
private:
	CRITICAL_SECTION	m_CriticalSection;

public:
	BOOL				m_bInit;

public:
	ApCriticalSection()
	{
		m_bInit = TRUE;
		InitializeCriticalSection(&m_CriticalSection);
	}

	~ApCriticalSection()
	{
		if( m_bInit )
			DeleteCriticalSection(&m_CriticalSection);
	}

	void Init()
	{
		if (m_bInit) return;

		m_bInit = TRUE;
		InitializeCriticalSection(&m_CriticalSection);
	}

	void Destroy()
	{
		if( m_bInit )
		{
			DeleteCriticalSection(&m_CriticalSection);
			m_bInit = FALSE;
		}
	}

	inline void Lock()
	{
		if( m_bInit )
			EnterCriticalSection(&m_CriticalSection);

		// test code gemani 
		//DWORD	cur_thread_id = GetCurrentThreadId();
		//if(cur_thread_id == g_ulMainThreadID)
		//{
		//	DWORD	start = timeGetTime();
		//	EnterCriticalSection(&m_CriticalSection);			
		//	DWORD	end = timeGetTime();

		//	if(end - start > 3)
		//	{
		//		start = 5;
		//	}
		//}
		//else
		//{
		//	EnterCriticalSection(&m_CriticalSection);
		//}
	}

	inline void Unlock()
	{
		if( m_bInit )
			LeaveCriticalSection(&m_CriticalSection);
	}

private: // ������� �� ���Կ��� ���� - 2011.02.11 kdi
	ApCriticalSection( ApCriticalSection const & other );
	void operator=( ApCriticalSection const & other );
};

class ApMutualEx {
public:
	ApMutualEx();
	~ApMutualEx();

	VOID Init(PVOID pvParent = NULL);
	VOID Destroy();

	BOOL WLock();
	BOOL RLock();
	BOOL RemoveLock();
	VOID ResetRemoveLock();
	BOOL Release(BOOL bForce = FALSE);

	BOOL SafeRelease();

	BOOL SetNotUseLockManager();

	// 2006.05.04. steeple - �׳� ThreadID �� �����ؾ���. 
	// 2010.07.20. supertj 
	// �츮������Ʈ���� �ش� CS�� �̹� �Ҹ��ڰ� �Ҹ��� ������µ� static ��ü���� ����� ������ �ִ� �͵��� �ִ�.
	// �̶����� �ش� CS�� üũ�ϴ°��� ������ �ۿ� ���� ��;;
	
	INT_PTR GetOwningThreadID() 
	{
		return (INT_PTR)m_csCriticalSection.OwningThread; 
	}

	BOOL IsInit() { return m_bInit; };
	BOOL IsRemoveLock() { return m_bRemoveLock; };

private:
	CRITICAL_SECTION	m_csCriticalSection;

	BOOL	m_bInit;
	PVOID	m_pvParent;
	BOOL	m_bRemoveLock;

	BOOL	m_bNotUseLockManager;

	// ApLockManager���� ���
	friend class ApLockManager;
	friend class ApLockTable;
	
	friend class FtrLock;
	friend class FtrUnlock;
	friend class FtrRecursionLock;
	friend class FtrRecursionUnlock;
};

// 2003.10.6 
// Jeong Nam-young
class AuAutoLock
{
private:
	ApCriticalSection*	m_pcsCriticalSection;
	ApMutualEx*			m_pcsMutualEx;
	BOOL				m_bAutoLock;
	BOOL				m_bMutualEx;

	BOOL				m_bIsLockResult;

public:
	AuAutoLock(ApCriticalSection& csCriticalSection, BOOL bAutoLock = TRUE) 
		: m_pcsCriticalSection(&csCriticalSection), m_bAutoLock(bAutoLock), m_bMutualEx(FALSE)
	{if(m_pcsCriticalSection) m_pcsCriticalSection->Lock(); m_bIsLockResult = TRUE;}

	AuAutoLock(ApMutualEx& csMutualEx, BOOL bAutoLock = TRUE)
		: m_pcsMutualEx(&csMutualEx), m_bAutoLock(bAutoLock), m_bMutualEx(TRUE)
	{if(m_pcsMutualEx) m_bIsLockResult = m_pcsMutualEx->WLock();}

	AuAutoLock() : m_pcsCriticalSection(NULL), m_pcsMutualEx(NULL), m_bAutoLock(FALSE), m_bMutualEx(FALSE)
	{}

	void SetMutualEx(ApMutualEx& csMutualEx, BOOL bAutoLock = TRUE)
	{m_pcsMutualEx=&csMutualEx; m_bAutoLock=bAutoLock; m_bMutualEx=TRUE;}

	void Lock() {if(m_pcsMutualEx) m_bIsLockResult = m_pcsMutualEx->WLock();}

	BOOL Result() {	assert(m_bIsLockResult); return m_bIsLockResult;}

	~AuAutoLock() 
	{
		if (m_bMutualEx)
		{
			if(m_pcsMutualEx)
				m_pcsMutualEx->Release();
		}
		else
		{
			if (m_pcsCriticalSection)
				m_pcsCriticalSection->Unlock();
		}
	}
};

// 2003. 11. 19
// ������
// ApArray�� ApAdmin������ Lock�� �ʿ����� �ʰ� ApModule������ Lock�� �ʿ��Կ� ����
// AutoLock�� ����Ī �Ҽ� �ִ� ����� �ʿ�
// �׷��� AuAutoLock2�� ź��!! 
//
// --> VC++ 6.0�� �����Ϸ��� Template�� Select����� �������� ������ ����
// �Ʒ��� ���� Int2Type�� �̿��Ͽ� �ذ�

const INT32 DONT_USE_AUTOLOCK = 0;
const INT32 USE_AUTOLOCK = 1;

template <INT32 TUseAutoLock = USE_AUTOLOCK>
struct AuAutoLock2
{
private:
	template <INT32 v>
	struct Int2Type
	{
		enum { value = v };
	};
	
private:
    ApMutualEx &m_csMutalEx;

private:
	void Lock(Int2Type<USE_AUTOLOCK>) { m_csMutalEx.RLock();}
	void Lock(Int2Type<DONT_USE_AUTOLOCK>) {}

	void UnLock(Int2Type<USE_AUTOLOCK>) {m_csMutalEx.Release();}
	void UnLock(Int2Type<DONT_USE_AUTOLOCK>) {}

public:
	AuAutoLock2(ApMutualEx& csMutalEx) : m_csMutalEx(csMutalEx) { Lock(Int2Type<TUseAutoLock>()); }
	~AuAutoLock2() { UnLock(Int2Type<TUseAutoLock>());}
};

///////////////////////////////////////////////////////////////////////////////
#endif