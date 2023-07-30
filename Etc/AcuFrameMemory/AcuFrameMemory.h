#ifndef _ACUFRAMEMEMORY_H_
#define _ACUFRAMEMEMORY_H_

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AcuFrameMemoryD" )
#else
#pragma comment ( lib , "AcuFrameMemory" )
#endif
#endif

#include <ApBase/ApBase.h>

class AcuFrameMemory {
private:
	static void *	m_pHead;
	static size_t	m_nCursor;
	static size_t	m_nSize;
	static size_t	m_nLastSize;
	static DWORD	m_dwMainThreadId;

public:
	AcuFrameMemory();
	~AcuFrameMemory();

	static void Clear();
	static void Release();
	static BOOL InitManager(size_t nSize);
	static void * Alloc(size_t nSize);
	static void * AllocAppend(size_t nSize);
	static void Dealloc(void * pMem);
};

template <class TObject>
class AcuFrameMemoryLocal
{
	INT		size;
	PVOID	memory;

public:
	AcuFrameMemoryLocal(UINT nNum)
		: size(sizeof(TObject) * nNum)
		, memory(AcuFrameMemory::Alloc(sizeof(TObject) * nNum))
	{
		ASSERT(nNum);
		ASSERT(memory);
	}

	operator TObject*()
	{
		return (TObject *)(memory);
	}
};

#endif