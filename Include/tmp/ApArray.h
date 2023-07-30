#if !defined(__APARRAY_H__)
#define __APARRAY_H__

#include "ApBase.h"
#include "AuParallel/for_each.hpp"

/******************************************************************************
Module:  ApList.h
Notices: Copyright (c) 2002 netong
Purpose: 
Last Update: 2003. 11. 17
			 ApArray�� ��ð� ����Ͽ����� GetSequence ���� ��ȿ������ ���� �߻�
			 �迭 �ε����� ���ʸ��� double-linked list�� ����Ͽ� �������� �ϸ�
			 ����ȭ ���� -> ����
******************************************************************************/

// Node ������ ���� ����ü
struct stDLISTDATA;
struct stNodeData
{
	stDLISTDATA	*pPrev;
	stDLISTDATA	*pNext;

	BOOL		bIsSetData;		// �����Ͱ� pData�� �����Ǿ� ������ TRUE, �׷��� ������ FALSE
	INT32		lCurrentIndex;
};

struct stDLISTDATA : public stNodeData
{
	BYTE		*pData;
};

const INT32 ARRAY_OFFSET_COUNT = 2;	// Head�� Tail Node�� �߰��ؾ� �ϹǷ� 2���� �� �ٴ´�.

/******************************************************************************
�ʵ� :	VC++ 6.0 �����Ϸ��� SGI STL���� pair������ iterator�� �������� ���ϱ� ������
		return���� 0�̸� �����Ͱ� ���°ɷ� �Ǻ�.
		
		�׷��� ApArray�� �ε����� 0 base�� �ƴ� 1 base �迭 �ε����� ����
		(���������δ� 0 base�� ���, �ܺο����� 1 base�� ���)
******************************************************************************/

// ApArray class
///////////////////////////////////////////////////////////////////////////////
template <INT32 TUseAutoLock> 
class ApArray {
public:
	BYTE		*m_pHeadArray;			// array pointer
	stDLISTDATA	*m_pstHead;				// 
	stDLISTDATA	*m_pstTail;				// 

	stDLISTDATA	*m_pstHeadEmpty;		// 

	INT32		m_lArraySize;			// array size
	INT32		m_lDataSize;			// data size
	INT32		m_lAllocDataSize;		// ���� �޸𸮿� �Ҵ�� Node�� ������ data size

	INT32		m_lDataCount;			// # of inserted data

	ApMutualEx	m_Mutex;

private:
	BOOL		InitializeNode();		// double linked-list node ����
	stDLISTDATA* operator [] (INT32 lIndex);

public:
	ApArray();
	~ApArray();

	BOOL Initialize(INT32 lArraySize, INT32 lDataSize);
	INT32 Add(PVOID pData);
	BOOL Delete(INT32 lIndex);
	BOOL DeletePrev(INT32 lIndex);

	PVOID GetData(INT32 lIndex);
	PVOID GetSequence(INT32* plIndex);

	BOOL Reset();

	BOOL SetNotUseLockManager();
};

// ApArray member functions
///////////////////////////////////////////////////////////////////////////////

template <INT32 TUseAutoLock>
inline ApArray<TUseAutoLock>::ApArray()
{
	m_pHeadArray		= NULL	;
	m_pstHeadEmpty		= NULL	;

	m_lArraySize		= 0		;
	m_lDataSize			= 0		;
	m_lDataCount		= 0		;
	m_lAllocDataSize	= 0		;

	m_pstHead			= NULL	;
	m_pstTail			= NULL	;
}

#pragma warning(disable:4786)
template <INT32 TUseAutoLock>
inline ApArray<TUseAutoLock>::~ApArray()
{
	if (m_pHeadArray)
	{
		free(m_pHeadArray);
	}
}
#pragma warning(default:4786)

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Initialize(INT32 lArraySize, INT32 lDataSize)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	m_lAllocDataSize = lDataSize + sizeof(stNodeData);	// double linked-list�� ����

	if (m_pHeadArray)
	{
		free(m_pHeadArray);
		m_pHeadArray	= NULL;
	}

	ASSERT( NULL == m_pHeadArray );
	//m_pHeadArray = (BYTE *) GlobalAlloc(GMEM_FIXED, (lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);
	m_pHeadArray = (BYTE *) malloc((lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);
	if (m_pHeadArray == NULL) return FALSE;	// memory allocation error...

	m_pstHeadEmpty	= (stDLISTDATA*)m_pHeadArray;
	m_lArraySize	= lArraySize;
	m_lDataSize		= lDataSize;

	InitializeNode();

	return TRUE;
}

template< typename T1, typename T2 >
struct InitNodeFuctor // ���δ��� ����� �� ���� ������ ���͸� ���� ���� ����մϴ�
{
	InitNodeFuctor( T1 head, T2 size ) : head(head), size(size) {}

	void operator()(int i, bool & breakFlag)
	{
		stDLISTDATA * pstCurrent = (stDLISTDATA*)(head + size*(i));
		stDLISTDATA * pstNext	= (stDLISTDATA*)(head + size*(i+1));

		pstCurrent->lCurrentIndex = i;
		pstCurrent->pNext = pstNext;
		pstNext->pPrev = pstCurrent;
	}

	T1 head;
	T2 size;
};

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::InitializeNode()
{
	// �ʱ�ȭ�� ���� ���� �����̹Ƿ� FALSE�� ����
	if (0 == m_lArraySize || 0 == m_lDataSize) return FALSE;

	ZeroMemory(m_pHeadArray, (m_lArraySize + ARRAY_OFFSET_COUNT) * m_lAllocDataSize);

	/*
	stDLISTDATA *pstCurrent, *pstNext;
	pstCurrent	= (stDLISTDATA*)m_pHeadArray;
	pstNext		= (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize);
	m_pstHead   = pstCurrent;

	// double linked list���·� node ����
	for(INT32 i = 0; i < (m_lArraySize + ARRAY_OFFSET_COUNT - 1); ++i) 
	{
		pstCurrent->lCurrentIndex = i;
		pstCurrent->pNext = pstNext;
		pstNext->pPrev = pstCurrent;

		pstCurrent = pstNext;
		pstNext = (stDLISTDATA*)((BYTE*)pstNext + m_lAllocDataSize);
	}

	m_pstTail = pstCurrent;
	m_pstTail->pNext = NULL;

	// Head�� GetSequence�� �����Ҷ� ���������� ���ǹǷ� ���� ����
	m_pstHeadEmpty = (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize);

	return TRUE;
	*/


	//-----------------------------------------------------------------------
	//
	// �ε��ӵ� ����� ���� ����ó�� (2011.01.13 - kdi)

	m_pstHead = (stDLISTDATA*)m_pHeadArray;

	int maxCount = (m_lArraySize + ARRAY_OFFSET_COUNT - 1);

	InitNodeFuctor< BYTE*, INT32 > functor( m_pHeadArray, m_lAllocDataSize );

	parallel::_for( 0, maxCount, functor, 8 ); // 8���� ������� ����ó���մϴ�.

	m_pstTail = (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize*maxCount);
	m_pstTail->pNext = NULL;

	// Head�� GetSequence�� �����Ҷ� ���������� ���ǹǷ� ���� ����
	m_pstHeadEmpty = (stDLISTDATA*)((BYTE*)m_pHeadArray + m_lAllocDataSize);

	return TRUE;
}

template <INT32 TUseAutoLock>
inline stDLISTDATA* ApArray<TUseAutoLock>::operator [] (INT32 lIndex)
{
	if ((m_lArraySize + 1 < lIndex) || (lIndex <= 0))	return NULL;

	--lIndex;
	stDLISTDATA *pstCurrent = (stDLISTDATA*)((BYTE*)m_pHeadArray + (lIndex * m_lAllocDataSize));

	return pstCurrent;

	/*
	if (pstCurrent->bIsSetData) return pstCurrent;
	else return NULL;
	*/
}

template <INT32 TUseAutoLock>
inline INT32 ApArray<TUseAutoLock>::Add(PVOID pData)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if ((!pData) || (m_lDataCount >= m_lArraySize))
		return (-1);

	// Node ����
	stDLISTDATA *pstCurrent = m_pstHeadEmpty;

	m_pstHeadEmpty = pstCurrent->pNext;

	// ������ ����
	CopyMemory(&pstCurrent->pData, pData, m_lDataSize);
	pstCurrent->bIsSetData = TRUE;

	++m_lDataCount;

	return pstCurrent->lCurrentIndex + 1;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Delete(INT32 lIndex)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if ((m_lArraySize + 1 < lIndex) || (lIndex <= 0) || (m_lDataCount <= 0))
		return FALSE;

	stDLISTDATA *pstCurrent = (*this)[lIndex];
	if (!pstCurrent || !pstCurrent->bIsSetData) return FALSE;	// bIsSetData�� ���õ��� ���� ���

	// ���� ��尡 m_pstHeadEmpty��� ��� �������ִ� �۾��� �ʿ����� ����
	if (pstCurrent->pNext != m_pstHeadEmpty)
	{
		// ������ ��带 HeadEmpty�� �ø��� ������ ��� ���� �۾� ����
		stDLISTDATA *pstPrev, *pstNext, *pstHeadEmptyPrev;
		pstPrev = pstCurrent->pPrev;
		pstNext = pstCurrent->pNext;
		pstHeadEmptyPrev = m_pstHeadEmpty->pPrev;

		// Current�� ������ ��, ���� ��带 ����
		pstPrev->pNext = pstNext;
		pstNext->pPrev = pstPrev;

		// HeadEmpty�� HeadEmpty�� Prev ���̷� Current Node�� �ű��.
		pstCurrent->pPrev = pstHeadEmptyPrev;
		pstHeadEmptyPrev->pNext = pstCurrent;

		// HeadEmpty�� �� ���� ����
		pstCurrent->pNext = m_pstHeadEmpty;
		m_pstHeadEmpty->pPrev = pstCurrent;
	}

	// ������ ���� �ȵȰɷ� ����
	pstCurrent->bIsSetData = FALSE;

	// ���ο� HeadEmpty�� ���
	m_pstHeadEmpty = pstCurrent;

	--m_lDataCount;

	return TRUE;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::DeletePrev(INT32 lIndex)
{
	// operator []������ AutoLock2�� ������� �����Ƿ� 
	// ���⼭ AutoLock2�� ������ش�.
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lArraySize + 1 < lIndex)
		return FALSE;

	stDLISTDATA *pstCurrent = (*this)[lIndex];
	if (!pstCurrent) return FALSE;
	
	return Delete(pstCurrent->pPrev->lCurrentIndex + 1);
}

template <INT32 TUseAutoLock>
inline PVOID ApArray<TUseAutoLock>::GetData(INT32 lIndex)
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lArraySize + 1 < lIndex || lIndex < 0)
		return NULL;

	stDLISTDATA *pstCurrent = (*this)[lIndex];

	if (pstCurrent && pstCurrent->bIsSetData) return &pstCurrent->pData;
	else return NULL;
}

template <INT32 TUseAutoLock>
inline PVOID ApArray<TUseAutoLock>::GetSequence(INT32* plIndex)
{
	if (!plIndex || *plIndex < 0 || m_lArraySize + 1 < *plIndex)
		return NULL;

	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (m_lDataCount <= 0)
		return NULL;

	if (!m_pstHead) return NULL;

	stDLISTDATA *pstCurrent;
	if (0 == *plIndex)
		pstCurrent = m_pstHead->pNext;
	else 
		pstCurrent = (*this)[*plIndex];

	PVOID	pvRetVal	= NULL;

	if (pstCurrent)
	{
		if (pstCurrent->bIsSetData)
		{
			*plIndex = pstCurrent->pNext->lCurrentIndex + 1;

			pvRetVal	= (PVOID)&pstCurrent->pData;
		}
	}

	return pvRetVal;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::Reset()
{
	AuAutoLock2<TUseAutoLock> Lock(m_Mutex);

	if (!InitializeNode()) return FALSE;
	m_lDataCount	= 0;

	return TRUE;
}

template <INT32 TUseAutoLock>
inline BOOL ApArray<TUseAutoLock>::SetNotUseLockManager()
{
	return m_Mutex.SetNotUseLockManager();
}

#endif	// __APARRAY_H__