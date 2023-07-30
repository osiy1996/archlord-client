#include "AcuFrameMemory.h"

void * AcuFrameMemory::m_pHead = NULL;
size_t AcuFrameMemory::m_nCursor = 0;
size_t AcuFrameMemory::m_nSize = 0;
size_t AcuFrameMemory::m_nLastSize = 0;
DWORD AcuFrameMemory::m_dwMainThreadId = 0;

AcuFrameMemory::AcuFrameMemory()
{
}

AcuFrameMemory::~AcuFrameMemory()
{
}

void
AcuFrameMemory::Release()
{
	free(m_pHead);
	m_pHead = NULL;
}

static void
memset32(void * dest, UINT32 value, size_t size)
{
	size_t i;

	for (i = 0; i < (size & (~3)); i += 4)
		memcpy((char *)dest + i, &value, 4);
	for (; i < size; i++)
		((char*)dest)[i] = ((char*)&value)[i & 3];
}

void
AcuFrameMemory::Clear()
{
#ifdef _DEBUG
	char szBuf[128];
	static size_t nMaxUse = 0;

	if (m_nCursor > nMaxUse)
		nMaxUse = m_nCursor;
	sprintf(szBuf, "Frame mem. in use: %ukb (Max. %ukb)\n", 
		m_nCursor / 1024, nMaxUse / 1024);
	OutputDebugStringA(szBuf);
#endif
	memset32(m_pHead, 0xDEADDEAD, m_nCursor);
	m_nCursor = 0;
}

BOOL
AcuFrameMemory::InitManager(size_t nSize)
{
	m_pHead = malloc(nSize);
	if (!m_pHead)
		return FALSE;
	m_nCursor = 0;
	m_nSize = nSize;
	m_dwMainThreadId = GetCurrentThreadId();
	return TRUE;
}

static void *
allocBlocking(size_t n)
{
	void * pMem = NULL;

	while (!pMem) {
		pMem = malloc(n);
		if (!pMem)
			Sleep(1);
	}
	return pMem;
}

void *
AcuFrameMemory::Alloc(size_t nSize)
{
	void * pMem;

	if (GetCurrentThreadId() != m_dwMainThreadId ||
		m_nCursor + nSize > m_nSize) {
		ASSERT(0 && "Ran out of frame memory.");
		return allocBlocking(nSize);
	}
	pMem = (void *)((UINT_PTR)m_pHead + m_nCursor);
	m_nCursor += nSize;
	m_nLastSize = nSize;
	return pMem;
}

void *
AcuFrameMemory::AllocAppend(size_t nSize)
{
	void * pMem;

	if (GetCurrentThreadId() != m_dwMainThreadId ||
		m_nCursor + nSize > m_nSize) {
		ASSERT(0 && "Ran out of frame memory.");
		return allocBlocking(nSize);
	}
	pMem = (void *)((UINT_PTR)m_pHead + m_nCursor);
	m_nCursor += nSize;
	m_nLastSize += nSize;
	return pMem;
}

void
AcuFrameMemory::Dealloc(void * pMem)
{
	void * pValid;

	if (GetCurrentThreadId() != m_dwMainThreadId) {
		free(pMem);
		return;
	}
	pValid = (void *)((UINT_PTR)m_pHead + (m_nCursor - m_nLastSize));
	if (pMem != pValid) {
		ASSERT(0 && "Invalid dealloc.");
		return;
	}
	m_nCursor -= m_nLastSize;
}
