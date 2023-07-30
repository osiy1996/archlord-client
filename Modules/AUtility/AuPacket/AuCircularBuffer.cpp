// AuCircularBuffer.cpp: implementation of the AuCircularBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "AuCircularBuffer.h"
#include <ApPacket.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define AUCIRCULAR_BUFFER_GUARD_BYTE 0xFEFEFEFE


AuCircularBuffer::AuCircularBuffer()
{
	m_pBuffer		= NULL;
	m_lSize			= 0;
	m_lCurrentIndex = 0;
}

AuCircularBuffer::~AuCircularBuffer()
{
	Destroy();
}

BOOL AuCircularBuffer::Init(LONG lSize)
{
	ASSERT(0 < lSize);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return FALSE;

	m_lSize		= lSize;
	m_pBuffer	= new CHAR[m_lSize];

	if (m_pBuffer)
	{
		//VirtualLock(m_pBuffer, lSize);
		return TRUE;
	}
	else return FALSE;
}

VOID AuCircularBuffer::Destroy()
{
	AuAutoLock Lock(m_Mutex);
	if (Lock.Result())
	{
		if (m_pBuffer)
		{
			//VirtualUnlock(m_pBuffer, m_lSize);
			delete [] m_pBuffer;
		}

		m_pBuffer = NULL;
	}
}

PVOID AuCircularBuffer::Alloc(LONG lAllocSize)
{
	ASSERT(lAllocSize < 1024 * 1024);
	ASSERT(lAllocSize >= 0);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return NULL;

	if (lAllocSize > 1024 * 1024)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "1MB �̻��� CircularBuffer AllocSize : %d", lAllocSize);
		AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);
		return NULL;
	}

	if (lAllocSize <= 0)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "0 ������ CircularBuffer AllocSize : %d", lAllocSize);
		AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);
		return NULL;
	}

	// 8�� ���̴� ������ ��, �ڷ� �������Ʈ�� ���̱� ���Ѱ�
	if ((m_lCurrentIndex + lAllocSize + 8) < m_lSize)
	{
		// ���� �Ҵ�Ǿ�� �ϴ� ũ�Ⱑ ������ ũ�⸦ ����� ������
		m_lCurrentIndex += (lAllocSize + 8);

		*(DWORD*)(m_pBuffer + m_lCurrentIndex - lAllocSize - 8) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		*(DWORD*)(m_pBuffer + m_lCurrentIndex - 4) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		return (PVOID)(m_pBuffer + m_lCurrentIndex - (lAllocSize + 4));
	}
	else
	{
		// ���� �Ҵ�Ǿ�� �ϴ� ũ�Ⱑ ������ ũ�⸦ �����
		m_lCurrentIndex = lAllocSize + 8;

		*(DWORD*)(m_pBuffer + m_lCurrentIndex - lAllocSize - 8) = AUCIRCULAR_BUFFER_GUARD_BYTE;
		*(DWORD*)(m_pBuffer + m_lCurrentIndex - 4) = AUCIRCULAR_BUFFER_GUARD_BYTE;

		return (PVOID)((CHAR*)m_pBuffer + 4);
	}
}

void AuCircularBuffer::Free(PVOID pvPacket)
{
	ASSERT(pvPacket);

	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pvPacket;
	ASSERT(*(DWORD*)((CHAR*)pvPacket - 4) == AUCIRCULAR_BUFFER_GUARD_BYTE);
	ASSERT(*(DWORD*)((CHAR*)pvPacket + pHeader->unPacketLength) == AUCIRCULAR_BUFFER_GUARD_BYTE);
}