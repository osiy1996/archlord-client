// ApMemoryManager.cpp: implementation of the ApMemoryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ApMemoryManager.h"
#include <ApBase/MagDebug.h>
#include <time.h>

#pragma warning( disable : 4312 )

#define APMEMORY_DEBUG


namespace {
	// 8����Ʈ ����
	int AlignQuad(int size)
	{
		return ((size + 7) & ~7);
	}
}


ApMemoryManager& ApMemoryManager::GetInstance()
{
	static ApMemoryManager _instance;
	return _instance;
}

void ApMemoryManager::DestroyInstance()
{
	GetInstance().Destroy();
}


ApMemoryManager::ApMemoryManager()
{
	Initialize();
}

ApMemoryManager::~ApMemoryManager()
{
}

VOID ApMemoryManager::Destroy()
{
	AuAutoLock Lock(m_cs);
	if (!Lock.Result()) return;

	// �Ҵ�� �޸� ����
	for (INT32 i = 0; i < m_lTypeIndex; ++i)
	{
		VirtualFree(m_pHeadArray[i], m_lArraySize[i] * m_lAllocSize[i], MEM_DECOMMIT);
		m_pHeadArray[i] = NULL;
	}

	Initialize();
}

VOID ApMemoryManager::Initialize()
{
	m_lTypeIndex = 0;

	ZeroMemory(m_pHeadArray, sizeof(m_pHeadArray));
	ZeroMemory(m_pHeadEmpty, sizeof(m_pHeadEmpty));

	ZeroMemory(m_lDataSize,  sizeof(m_lDataSize));
	ZeroMemory(m_lAllocSize, sizeof(m_lAllocSize));
	ZeroMemory(m_lArraySize, sizeof(m_lArraySize));

	ZeroMemory(m_lAllocCount,	 sizeof(m_lAllocCount));
	ZeroMemory(m_lAllocCountPeak,sizeof(m_lAllocCountPeak));

	ZeroMemory(m_lAddedAllocCount,sizeof(m_lAddedAllocCount));
	ZeroMemory(m_lFlagAssert,	  sizeof(m_lFlagAssert));

	ZeroMemory(m_bEnable, sizeof(m_bEnable));

#ifdef _CPPRTTI
	ZeroMemory(m_TypeName, sizeof(m_TypeName));
#endif	

	::GetLocalTime(&m_stStartTime);
}

// �޸��� ��� ��Ȳ�� txt���Ϸ� ����Ʈ�Ѵ�.
VOID ApMemoryManager::ReportMemoryInfo()
{
	AuAutoLock Lock(m_cs);
	if (!Lock.Result()) return;
	
	CHAR szFileName[_MAX_PATH] = {0, };
	CHAR szTimeBuf[32] = {0, };

	::GetModuleFileName(NULL, szFileName, _MAX_PATH);

	SYSTEMTIME st;
	::GetLocalTime(&st);

	sprintf(szTimeBuf, "_%d_%2d%2d_%2d%2d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute);

	::strcat(szFileName, szTimeBuf);
	::strcat(szFileName, "_ApMemory.log");

	FILE* fp = fopen(szFileName, "w");
	if (NULL == fp) return;

	fprintf(fp, "--- ApMemory Report ---\n");

	fprintf(fp, "Start time : %d-%2d-%2d %2d:%2d:%2d\n\n", m_stStartTime.wYear, m_stStartTime.wMonth, m_stStartTime.wDay,
													m_stStartTime.wHour, m_stStartTime.wMinute, m_stStartTime.wSecond);

	fprintf(fp, "Log time : %d-%2d-%2d %2d:%2d:%2d\n\n", st.wYear, st.wMonth, st.wDay,
													st.wHour, st.wMinute, st.wSecond);

	INT64 lTotalAllocSize = 0, lTotalUseSize = 0;
	for (INT32 i = 0; i < m_lTypeIndex; ++i)
	{
#ifdef _CPPRTTI
		fprintf(fp, "[%s]Index : %d, DataSize : %d, AllocSize : %d, ArraySize : %d, AllocCount : %d, AllocPeakCount : %d, NotAllocSpace: %d\n",
				m_TypeName[i], i, m_lDataSize[i], m_lAllocSize[i], m_lArraySize[i], m_lAllocCount[i], m_lAllocCountPeak[i],m_lArraySize[i] - m_lAllocCountPeak[i]);
#else
		fprintf(fp, "Index : %d, DataSize : %d, AllocSize : %d, ArraySize : %d, AllocCount : %d, AllocPeakCount : %d, NotAllocSpace: %d\n",
					i, m_lDataSize[i], m_lAllocSize[i], m_lArraySize[i], m_lAllocCount[i], m_lAllocCountPeak[i],m_lArraySize[i] - m_lAllocCountPeak[i]);
#endif
		// ��ü �Ҵ�� �޸�, ���� �޸� ���
		lTotalAllocSize += (m_lAllocSize[i] * m_lArraySize[i]);
		lTotalUseSize += (m_lAllocSize[i] * m_lAllocCount[i]);
	}
	
	fprintf(fp, "\n\n");
	fprintf(fp, "Total Allocated Memory : %I64d Bytes\n", lTotalAllocSize);
	fprintf(fp, "Total Used Memory : %I64d Bytes\n", lTotalUseSize);

	fprintf(fp, "\n\n\n=== Overflow Nodes === \n", lTotalAllocSize);
	for(INT32 i=0;i<m_lTypeIndex;++i)
	{
		if(m_lFlagAssert[i])
		{
			fprintf(fp, "index: %d, AllocCount:%d, OverflowCount:%d\n",i, m_lAllocCount[i],m_lAddedAllocCount[i]);
		}
	}

	fprintf(fp, "--- Report End ---");
	fclose(fp);
}

INT32 ApMemoryManager::AddType(SIZE_T lSize, SIZE_T lArraySize, const CHAR* pTypeName)
{
	if (MAX_TYPE_SIZE <= m_lTypeIndex)
	{
		ASSERT(!"MAX_TYPE_SIZE�� ������ �÷��� ��!!");
		return -1;
	}

	m_cs.Lock();	// lock
	
// �ּ� ũ��� sizeof(PVOID)���� Ŀ���ϰ� 8�� ��������Ѵ�.
#ifndef APMEMORY_DEBUG
	m_lAllocSize[m_lTypeIndex] = AlignQuad( max(lSize, sizeof(PVOID)) );
#else
	m_lAllocSize[m_lTypeIndex] = AlignQuad( sizeof(PVOID)		// Next Node Address
											+ sizeof(INT32)		// Alloc Flag
											+ sizeof(PVOID)		// Guard Byte
											+ (INT32)lSize		// Data
											+ sizeof(PVOID) );	// Guard Byte
#endif

	// �޸� �Ҵ� �� �ʱ�ȭ ����
	m_lArraySize[m_lTypeIndex]		 = (INT32)lArraySize;
	m_lDataSize[m_lTypeIndex]		 = (INT32)lSize;
	m_lAllocCount[m_lTypeIndex]		 = 0;
	m_lAllocCountPeak[m_lTypeIndex]  = 0;
	m_lFlagAssert[m_lTypeIndex]		 = 0;
	m_lAddedAllocCount[m_lTypeIndex] = 0;

#ifdef _CPPRTTI
	if (pTypeName)
		strncpy(m_TypeName[m_lTypeIndex], pTypeName, MAX_TYPE_NAME_SIZE);
#endif
		
	m_pHeadArray[m_lTypeIndex] = VirtualAlloc(NULL,
											  m_lArraySize[m_lTypeIndex] * m_lAllocSize[m_lTypeIndex],
											  MEM_RESERVE | MEM_COMMIT,
											  PAGE_READWRITE);

	INT32 lInitializeTypeIndex = m_lTypeIndex++;	// �����ϰ� �ε��� ����

	m_cs.Unlock();	// Unlock

	InitializeNode(lInitializeTypeIndex);

	return lInitializeTypeIndex;
}

INT32 ApMemoryManager::GetAllocatedCount(INT32 lTypeIndex)
{
	return m_lAllocCount[lTypeIndex];
}

VOID ApMemoryManager::InitializeNode(INT32 lTypeIndex)
{
	AuAutoLock Lock(m_cs);
	if (!Lock.Result()) return;

	m_pHeadEmpty[lTypeIndex] = m_pHeadArray[lTypeIndex];
	
	BYTE *pCurrent = (BYTE*)m_pHeadArray[lTypeIndex];
	BYTE *pNext = pCurrent + m_lAllocSize[lTypeIndex];

#ifndef APMEMORY_DEBUG
	// linked list ��带 ����
	for (INT32 i = 0; i < m_lArraySize[lTypeIndex] - 1; ++i)
	{
		*(PVOID*)pCurrent = pNext;
		pCurrent = pNext;
		pNext += m_lAllocSize[lTypeIndex];
	}

	*(PVOID*)pCurrent = NULL;
#else
	// .NET������ ���� ���� �ذ���ٵ�... ��.��
	for (INT32 i = 0; i < m_lArraySize[lTypeIndex]; ++i)
	{
		// next node 
		if (i < m_lArraySize[lTypeIndex] - 1)
			*(PVOID*)pCurrent = pNext;
		else
			*(PVOID*)pCurrent = (PVOID)LAST_NODE;
		pCurrent += sizeof(PVOID);

		// alloc flag
		*(BOOL*)pCurrent = FALSE;
		pCurrent += sizeof(INT32);

		// guard byte
		memset(pCurrent, GUARD_BYTE_BEGIN, sizeof(PVOID));
		pCurrent += sizeof(PVOID);

		// alloc byte
		memset(pCurrent, ALLOC_BYTE, m_lDataSize[lTypeIndex]);
		pCurrent += m_lDataSize[lTypeIndex];

		// guard byte
		memset(pCurrent, GUARD_BYTE_END, sizeof(PVOID));

		pCurrent = pNext;

		if (i < m_lArraySize[lTypeIndex] - 1)
			pNext += m_lAllocSize[lTypeIndex];
	}
#endif

	m_lAllocCount[lTypeIndex]		= 0;
	m_lAllocCountPeak[lTypeIndex]	= 0;
	m_lAddedAllocCount[lTypeIndex]	= 0;
	m_lFlagAssert[lTypeIndex]		= 0;
	m_bEnable[lTypeIndex]			= TRUE;
}

BOOL ApMemoryManager::CheckBeginGuardByte(PVOID pObject)
{
	BYTE *pCurrent = (BYTE*)pObject;
	
	pCurrent += sizeof(PVOID) + sizeof(INT32);
#ifdef WIN64
	return (*(DWORD_PTR*)pCurrent == 0xBBBBBBBBBBBBBBBB);
#else
	return (*(DWORD_PTR*)pCurrent == 0xBBBBBBBB);
#endif
}

BOOL ApMemoryManager::CheckEndGuardByte(PVOID pObject, INT32 lTypeIndex)
{
	BYTE *pCurrent = (BYTE*)pObject;

	pCurrent += (sizeof(PVOID) + sizeof(INT32) + sizeof(PVOID) + m_lDataSize[lTypeIndex]);
#ifdef WIN64
	return (*(DWORD_PTR*)pCurrent == 0xCCCCCCCCCCCCCCCC);
#else
	return (*(DWORD_PTR*)pCurrent == 0xCCCCCCCC);
#endif
}

BOOL ApMemoryManager::GetAllocated(PVOID pObject)
{
	BYTE *pCurrent = (BYTE*)pObject;
	pCurrent += sizeof(PVOID);
	return *(BOOL*)pCurrent;
}

VOID ApMemoryManager::SetAllocation(PVOID pObject, BOOL bAllocation)
{
	BYTE *pCurrent = (BYTE*)pObject;
	pCurrent += sizeof(PVOID);
	*(BOOL*)pCurrent = bAllocation;
}

PVOID ApMemoryManager::NewObject(INT32 lTypeIndex)
{
	if(lTypeIndex < 0 || lTypeIndex > MAX_TYPE_SIZE)
		return NULL;

	ASSERT(m_bEnable[lTypeIndex]);
	if (!m_bEnable[lTypeIndex]) 
		return NULL;

	if (m_lAllocCount[lTypeIndex] >= m_lArraySize[lTypeIndex])
	{
		//ASSERT(m_lFlagAssert[lTypeIndex] || !"�޸� Ǯ�� ������ ���ڶ� �߰��� �Ҵ� ������");
		m_lFlagAssert[lTypeIndex] = 1;
		++m_lAddedAllocCount[lTypeIndex];
		return malloc(m_lDataSize[lTypeIndex]);
	}

	++m_lAllocCount[lTypeIndex];

	PVOID pNewObject = m_pHeadEmpty[lTypeIndex];

#ifndef APMEMORY_DEBUG
	// �Ҵ�� ������Ʈ�� �ּҰ� ��ȿ�� �������� �ִ��� �˻�
	INT32 lMemorySize = m_lAllocSize[lTypeIndex] * m_lArraySize[lTypeIndex];
	ASSERT( m_pHeadArray[lTypeIndex] <= m_pHeadEmpty[lTypeIndex] && "NewObject : ������ �޸� �ּҰ� �߸� ���Խ��ϴ�." );
	ASSERT( ((CHAR*)m_pHeadArray[lTypeIndex] + lMemorySize) >= m_pHeadEmpty[lTypeIndex] && "NewObject : ������ �޸� �ּҰ� �߸� ���Խ��ϴ�.");

	// ����ִ� ��� �Ҵ�
//	pNewObject = m_pHeadEmpty[lTypeIndex];

	if (NULL != *(PVOID*)m_pHeadEmpty[lTypeIndex])
	{
		// �Ҵ�� ������Ʈ�� �ּҰ� ��ȿ�� �������� �ִ��� �˻�
		ASSERT( m_pHeadArray[lTypeIndex] <= *(PVOID*)m_pHeadEmpty[lTypeIndex] && "NewObject : ������ �޸� �ּҰ� �߸� ���Խ��ϴ�." );
		ASSERT( ((CHAR*)m_pHeadArray[lTypeIndex] + lMemorySize) >= *(PVOID*)m_pHeadEmpty[lTypeIndex] && "NewObject : ������ �޸� �ּҰ� �߸� ���Խ��ϴ�.");
	}

	// ����ִ� ���� ��带 HeadEmpty�� ����
	m_pHeadEmpty[lTypeIndex] = *(PVOID*)m_pHeadEmpty[lTypeIndex];
#else
	ASSERT(CheckBeginGuardByte(pNewObject));
	ASSERT(CheckEndGuardByte(pNewObject, lTypeIndex));
	ASSERT(GetAllocated(pNewObject) == FALSE);

	// ����ִ� ���� ��带 HeadEmpty�� ����
	m_pHeadEmpty[lTypeIndex] = *(PVOID*)pNewObject;
	SetAllocation(pNewObject, TRUE);

	// ������ ����� ��쿡�� ���� ����� ��ȿ�� �˻縦 �� �ʿ䰡 ����.
	if ((PVOID)LAST_NODE != m_pHeadEmpty[lTypeIndex])
	{
		ASSERT(CheckBeginGuardByte(m_pHeadEmpty[lTypeIndex]));
		ASSERT(CheckEndGuardByte(m_pHeadEmpty[lTypeIndex], lTypeIndex));
		ASSERT(GetAllocated(m_pHeadEmpty[lTypeIndex]) == FALSE);
	}

#endif

	// ���� �Ҵ�� ������ m_lAllocCountPeak���� ������ �����Ѵ�.
	if (m_lAllocCount[lTypeIndex] > m_lAllocCountPeak[lTypeIndex])
		m_lAllocCountPeak[lTypeIndex] = m_lAllocCount[lTypeIndex];

	return ((BYTE*)pNewObject + (sizeof(PVOID) + sizeof(INT32) + sizeof(PVOID)));
}

VOID ApMemoryManager::DeleteObject(INT32 lTypeIndex, PVOID pObject)
{
	ASSERT(m_bEnable[lTypeIndex]);

	INT32 lMemorySize = m_lAllocSize[lTypeIndex] * m_lArraySize[lTypeIndex];
	if (m_pHeadArray[lTypeIndex] > pObject || ((CHAR*)m_pHeadArray[lTypeIndex] + lMemorySize) < pObject)
	{
		--m_lAddedAllocCount[lTypeIndex];
		free(pObject);
		return;
	}

#ifndef APMEMORY_DEBUG
	// ������ ��带 HeadEmpty�� ����
	*(PVOID*)pObject = m_pHeadEmpty[lTypeIndex];
	m_pHeadEmpty[lTypeIndex] = pObject;	
	--m_lAllocCount[lTypeIndex];
#else
	BYTE *pBytes = (BYTE*)pObject;
	pBytes -= (sizeof(PVOID) + sizeof(INT32) + sizeof(PVOID));

	ASSERT(CheckBeginGuardByte(pBytes));
	ASSERT(CheckEndGuardByte(pBytes, lTypeIndex));

	if (GetAllocated(pBytes) == FALSE) 
	{
		printf("second delete size : %d\n", m_lAllocSize[lTypeIndex]);
		return;
	}

	SetAllocation((PVOID)pBytes, FALSE);

	// ������ ��带 HeadEmpty�� ����
	*(PVOID*)pBytes = m_pHeadEmpty[lTypeIndex];
	m_pHeadEmpty[lTypeIndex] = (PVOID)pBytes;	
	--m_lAllocCount[lTypeIndex];
#endif
}

BOOL ApMemoryManager::SetEnable(INT32 lTypeIndex, BOOL bEnable)
{
	AuAutoLock Lock(m_cs);
	if (!Lock.Result()) return FALSE;

	if (m_bEnable[lTypeIndex] == bEnable) 
		return FALSE;

	m_bEnable[lTypeIndex] = bEnable;

	if (m_bEnable[lTypeIndex])
	{
		// Reserve�� ������ Commit ��Ų��.
		m_pHeadArray[lTypeIndex] = VirtualAlloc(m_pHeadArray[lTypeIndex], m_lArraySize[lTypeIndex] * m_lAllocSize[lTypeIndex], 
													MEM_COMMIT, PAGE_READWRITE);
		InitializeNode(lTypeIndex);
	}
	else
	{
		// Commit�� �޸𸮸� ����, Reserve�� ������ �״�� �д�.
		VirtualFree(m_pHeadArray[lTypeIndex], m_lArraySize[lTypeIndex] * m_lAllocSize[lTypeIndex], MEM_DECOMMIT);

		//@{ 2006/06/05 burumal
		m_pHeadArray[lTypeIndex] = NULL;
		//@}
	}

	return TRUE;
}

BOOL ApMemoryManager::GetEnable(INT32 lTypeIndex)
{
	ASSERT(lTypeIndex < MAX_TYPE_SIZE);
	return m_bEnable[lTypeIndex];
}
