/******************************************************************************
Module:  AuPacket.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 01. 06
******************************************************************************/

#include <AuPacket/AuPacket.h>
#include <ApBase/MagDebug.h>
#include <ApPacket.h>
#include <AuProfileManager/AuProfileManager.h>
#include <ApMemoryTracker/ApMemoryTracker.h>

#define PACKET_BUFFER_SIZE 2048

AuCircularBuffer g_AuCircularBuffer;
AuCircularBuffer g_AuCircularOutBuffer;

UINT	g_ulMinCompressSize					= 80;

UINT32 g_ulFrameTick = 0;
UINT32 g_ulLastProcessedFrameTick = 0;

/*
UINT64	g_ullTotalMakePacketSize			= 0;	// Packet���� ������� �� ������
UINT64	g_ullCompressedCount				= 0;	// ������ �õ��� �� Ƚ��
UINT64	g_ullCompressedNotEfficientCount	= 0;	// ������ ������ �����ϱ� �� ���� ũ�Ⱑ �� Ŀ������� Ƚ��
UINT64	g_ullCompressedMakePacketSize		= 0;	// 80byte�� �Ѵ� �ֵ��� �����ϱ��� ����

UINT64	g_ullOriginalPacketSize[2000]		= { 0 , };
UINT16	g_ullCompressedPacketSize[2000]		= { 0 , };
*/


#ifdef _DEBUG
ApCriticalSection	g_MutexPacket;
#endif

AuPacket::AuPacket()
{
	m_bIsSetField		= FALSE;
	m_nFlagLength		= 0;

	ZeroMemory(m_cFieldType, sizeof(UINT16) * 64);
	ZeroMemory(m_cFieldLength, sizeof(UINT16) * 64);

	m_cNumField			= 0;

	/*
	strcpy( ( char * ) m_cFieldType		, "" );
	strcpy( ( char * ) m_cFieldLength	, "" );
	*/
}

AuPacket::~AuPacket()
{
//	if (m_bAllocated)
//		GlobalFree(m_pvData);
}

//		SetFieldType
//	Functions
//		- Parsing�� ���ؼ� ��� Field�� Type�� �����ش�. 
//		  GetField(), MakeField(), MakePacket()�� ���ؼ� �ʼ������� �ʿ��ϴ�.
//			Ex) SetFieldType(AUTYPE_INT16, 1, AUTYPE_CHAR, 10, AUTYPE_UINT32, 1, AUTYPE_PACKET, 1, AUTYPE_END, 0);
//				    ==> 0x01 : INT16
//						0x02 : CHAR[10]
//						0x04 : UINT32
//						0x08 : �� �ٸ� �÷��� (��ø�� �÷��״�)
//	Arguments
//		- ... : �������� ������ ���� ��Ÿ���� ���� �� �������� AUTYPE_END�� ���ڷ� �Ѱ��ش�.
//	Return value
//		- INT16 : process result
///////////////////////////////////////////////////////////////////////////////
INT16 AuPacket::SetFieldType(UINT16 cFirst, ...)
{
	va_list	ap;

	UINT16	i = cFirst;
	m_cNumField	= 0;

	va_start(ap, cFirst);
	while (i != AUTYPE_END)
	{
		ASSERT(i < AUTYPE_MAX && "AuPacket: SetFieldType ����. AUTYPE_END Ȯ�� �ٶ�");

		m_cFieldType[m_cNumField] = i;

		i = va_arg(ap, UINT16);
		ASSERT(i > 0 && "AuPacket: SetFieldType ����. Ÿ�� ũ�Ⱑ 0���Ϸ� ������... ��.��");
		m_cFieldLength[m_cNumField] = i;

		m_cNumField++;

		i = va_arg(ap, UINT16);
	}
	va_end(ap);

	m_bIsSetField = TRUE;

	return TRUE;
}

INT16 AuPacket::GetField(BOOL bIsPacket, PVOID pvPacketRaw, INT16 nPacketLength, PVOID pvFirst, ...)
{
	if (!m_nFlagLength)
	{
		return FALSE;
	}

	PVOID	pvPacket	= pvPacketRaw;
/*	
	if (bIsPacket && ((PACKET_HEADER *) pvPacket)->Flag.Compressed == true)
	{
		PVOID	pvOutBuffer	= g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		if (!pvOutBuffer)
			return FALSE;

		ZeroMemory(pvOutBuffer, APPACKET_MAX_PACKET_SIZE);

		UINT16	unOutBufferSize	= APPACKET_MAX_PACKET_SIZE - sizeof(PACKET_HEADER);

		if (!m_MiniLZO.DeCompress((BYTE *) pvPacket + sizeof(PACKET_HEADER),
								  (UINT16) ((PACKET_HEADER *) pvPacket)->unPacketLength - sizeof(PACKET_HEADER) - 1,
								  (BYTE *) pvOutBuffer + sizeof(PACKET_HEADER),
								  &unOutBufferSize))
			return FALSE;

		CopyMemory(pvOutBuffer, pvPacket, sizeof(PACKET_HEADER));

		pvPacket	= pvOutBuffer;
	}
*/
	va_list ap;

	int	nIndex = 0;
	PVOID	pvBuffer = pvFirst;
	DWORD	dwFlagMask = 0x01;		// bitmask�� �����Ѵ�.
	DWORD	dwFlag;
	PVOID	pvIndex;

	if (bIsPacket) {
		UINT32 ulLastProcessed;

		switch (m_nFlagLength) {
		case 1:
			dwFlag = ((UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		case 2:
			dwFlag = ((UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		case 4:
			dwFlag = ((UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)))[0];
			break;
		}
		ulLastProcessed = ((PACKET_HEADER *)pvPacket)->ulFrameTick;
		if (ulLastProcessed)
			g_ulLastProcessedFrameTick = ulLastProcessed;
		pvIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		switch (m_nFlagLength) {
		case 1:
			dwFlag = ((UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		case 2:
			dwFlag = ((UINT16 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		case 4:
			dwFlag = ((UINT32 *) ((CHAR *) pvPacket + sizeof(UINT16)))[0];
			break;
		}

		pvIndex = (CHAR *) pvPacket + m_nFlagLength + sizeof(UINT16) /*packet length*/;	// ù��° ����Ÿ�� ����Ų��.
	}
	
	va_start(ap, pvFirst);
	while (nIndex < m_cNumField)
	{
		if ((dwFlagMask & dwFlag))	// Flag�� �� �ʵ尡 1�� ���õǾ� ������ ����Ÿ�� �����´�.
		{
			if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
			{
				// �߸��� Packet�� ���Դ�.
				return FALSE;
			}

			if (m_cFieldType[nIndex] == AUTYPE_PACKET)		// ����Ÿ�� �Ǵٸ� Flag�� ���� ����Ÿ��.
			{
				if (m_cFieldLength[nIndex] > 1)
				{
					INT8	cArraySize = *(INT8 *) pvIndex;

					PVOID *pvPacketArray = (PVOID *) pvBuffer;

					pvIndex = (CHAR *) pvIndex + sizeof(INT8);
					for (int j = 0; j < cArraySize; ++j)
					{
						UINT16	cLength = *(UINT16 *) pvIndex;

						if (pvPacketArray)
						{
							pvPacketArray[j] = pvIndex;
						}

						pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;
					}
				}
				else
				{
					// ����Ÿ�� �÷��׸� �����ϰ� �ִ°�� Flag �տ� �ִ� ���̸� ���� �� ����Ÿ�� ũ�⸦ ���Ѵ�.
					UINT16	cLength = *(UINT16 *) pvIndex;

					if (pvBuffer)
						*(PVOID *)pvBuffer = (CHAR *) pvIndex;

					pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;
				}
			}
			else if (m_cFieldType[nIndex] == AUTYPE_MEMORY_BLOCK)
			{
				UINT16	cLength = *(UINT16 *) pvIndex;

				if (pvBuffer)
					*(PVOID *)pvBuffer = (CHAR *) pvIndex + sizeof(UINT16);

				pvIndex = (CHAR *) pvIndex + sizeof(UINT16) + cLength;

				if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// �߸��� Packet�� ���Դ�.
					return FALSE;
				}

				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
					*((UINT16 *) pvBuffer) = cLength;
			}
			else if (m_cFieldType[nIndex] == AUTYPE_CHAR)
			{
				// ���ڷ� �Ѿ�� ���� ����Ÿ �����͸� �Ѱ��ش�.
				if (pvBuffer)
					*(CHAR **)pvBuffer = (CHAR *) pvIndex;

				pvIndex = (CHAR *) pvIndex + AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex];

				if (bIsPacket && pvIndex > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// �߸��� Packet�� ���Դ�.
					return FALSE;
				}
			}
			else						// �� ������ ����Ÿ��.
			{
				if (bIsPacket && (CHAR *) pvIndex + sizeof(CHAR) * AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex] > ((CHAR *) pvPacketRaw) + nPacketLength)
				{
					// �߸��� Packet�� ���Դ�.
					return FALSE;
				}

				// ���ڷ� �Ѿ�� ���� ����Ÿ �����͸� �Ѱ��ش�.
				if (pvBuffer)
					CopyMemory(pvBuffer, pvIndex, sizeof(CHAR) * AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex]);

				pvIndex = (CHAR *) pvIndex + AuTypeSize[m_cFieldType[nIndex]] * m_cFieldLength[nIndex];
			}
		}
		else if (pvBuffer && m_cFieldType[nIndex] == AUTYPE_PACKET)
		{
			*(PVOID *)pvBuffer = NULL;
		}
		else if (m_cFieldType[nIndex] == AUTYPE_MEMORY_BLOCK)
		{
			pvBuffer = va_arg(ap, PVOID);
		}

		pvBuffer = NULL;

		nIndex++;
		dwFlagMask <<= 1;

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);

	return TRUE;
}

BOOL AuPacket::SetCID(PVOID pvPacket, INT16 nPacketLength, INT32 lCID)
{
	if (!pvPacket ||
		nPacketLength < 7 /* sizeof(UINT16) + sizeof(UINT8) + sizeof(INT32) */ ||
		lCID == 0)
		return FALSE;

	((PPACKET_HEADER) pvPacket)->lOwnerID	= lCID;

	return TRUE;
}

PVOID AuPacket::MakePacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...)
{
	PROFILE("AuPacket::MakePacket");
	
	if (!m_nFlagLength)
		return NULL;

	//m_Flag.m_ulFlag = 0;
	UINT32	ulFlag = 0;

	// ���� ũ�⸦ ���Ѵ�.
	va_list ap;

	UINT16	nLength;
	if (bIsPacket)
		//nLength = sizeof(UINT16)/*packet length*/ + sizeof(UINT8)/*packet type*/ + sizeof(INT32)/*CID*/ + m_nFlagLength;
		nLength = sizeof(PACKET_HEADER) + m_nFlagLength;
	else
		nLength = sizeof(UINT16)/*packet length*/ + m_nFlagLength;

	va_start(ap, cType);

	PVOID	pvBuffer = va_arg(ap, PVOID);
	DWORD	dwMask = 0x01;

	int i = 0, j = 0;
	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			BOOL	bSetFlag = TRUE;

			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength += sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];
					}

					if (j > 0)
					{
						// if packet type is array packet..
						nLength += sizeof(INT8);	// array�� ũ�⸦ �տ� �����Ѵ�.
					}
					else
					{
						bSetFlag = FALSE;
					}
				}
				else
				{
					// field type�� flag�� ��� flag �տ� �ִ� UINT8 ũ���� ���̸� ���� �� ���̸� ����Ѵ�.
					nLength += sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
				{
					nLength += sizeof(UINT16) + *(INT16 *) pvBuffer;
				}
			}
			else
			{
				nLength += AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}

			if (bSetFlag)
				ulFlag |= dwMask;
		}

		pvBuffer = va_arg(ap, PVOID);

		dwMask <<= 1;
	}
	va_end(ap);

	if (!nLength)
		return FALSE;

#ifdef	_DEBUG
	ASSERT(nLength > 0);
	ASSERT(nLength < PACKET_BUFFER_SIZE * 40);
#else
	if (nLength < 0 ||
		nLength >= PACKET_BUFFER_SIZE * 40)
		return FALSE;
#endif	//_DEBUG

	//ASSERT(nLength < PACKET_BUFFER_SIZE);

//	InterlockedIncrement(&g_lMakePacketCount);

	if (bIsPacket)
		//nLength += 2;
		nLength += 1;	// end guard byte

	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// ��Ŷ �յڷ� �������Ʈ�� ���δ�.

	if (!pvPacketRaw)
		return FALSE;

//	PVOID	pvPacket	= NULL;

	if (bIsPacket)
	{
		// set guard byte
		*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
		*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

		/* Set frame tick */
		((PACKET_HEADER *)pvPacketRaw)->ulFrameTick = g_ulFrameTick;

//		pvPacket	= (PVOID) ((BYTE *) pvPacketRaw + 1);
	}
//	else
//		pvPacket	= pvPacketRaw;

	PVOID	pvPacket	= pvPacketRaw;

	if (pnPacketLength)
		*pnPacketLength = nLength;

	//*((UINT16 *) pvPacket) = nLength;

	((PACKET_HEADER *) pvPacket)->unPacketLength	= nLength;

	if (bIsPacket)
	{
		//*((INT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)))	= 0;
		((PACKET_HEADER *) pvPacket)->lOwnerID	=	 0;
		((PACKET_HEADER *) pvPacket)->Flag.Compressed	= 0;
	}

	// ũ�⵵ �˾Ƴ����� ���� ���� ��Ŷ�� �����Ѵ�.

	CHAR* pIndex;
	if (bIsPacket)
	{
		// ��Ŷ Ÿ���� �����Ѵ�.
		//*(UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)) = cType;
		((PACKET_HEADER *) pvPacket)->cType	= cType;

		// Flag ũ�⸸ŭ m_pvPacket�� �����Ѵ�.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		}

		// ���� �ϳ��� ����Ÿ�� ���δ�.
		pIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		// Flag ���� �������� ���̸� �����Ѵ�.
		*(UINT16 *) pvPacket = nLength - sizeof(UINT16);

		// Flag ũ�⸸ŭ m_pvPacket�� �����Ѵ�.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		}

		// ���� �ϳ��� ����Ÿ�� ���δ�.
		pIndex = (CHAR *) pvPacket + sizeof(UINT16) + m_nFlagLength;
	}

	va_start(ap, cType);

	pvBuffer = va_arg(ap, PVOID);

	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					INT8	*pcArraySize = (INT8 *) pIndex;

					pIndex += sizeof(INT8);

					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength = sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];

						if (nLength > 0)
							CopyMemory(pIndex, pvPacketArray[j], nLength);

						pIndex += nLength;
					}

					if (j > 0)
						*pcArraySize = j;

					nLength = 0;
				}
				else
				{
					// field type�� flag�� ��� flag �տ� �ִ� UINT16 ũ���� ���̸� ���� �� ���̸� ����Ѵ�.
					nLength = sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				PVOID pvLength = va_arg(ap, PVOID);
				if (pvLength)
				{
					// ����Ÿ ���̸� ���� ������ �ִ´�.
					CopyMemory(pIndex, pvLength, sizeof(UINT16));
					pIndex += sizeof(UINT16);

					nLength = *(INT16 *) pvLength;
				}
			}
			else
			{
				nLength = AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}
			
			if (nLength > 0)
				CopyMemory(pIndex, pvBuffer, nLength);

			pIndex += nLength;

			nLength = 0;
		}

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);


	if (bIsPacket && ((PACKET_HEADER *) pvPacketRaw)->unPacketLength > g_ulMinCompressSize)
	{
		UINT16	unDataLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - sizeof(PACKET_HEADER) - 1 /* End Guard Byte */;

/*		
		PVOID	pvTempBuffer	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);

		CopyMemory(pvTempBuffer, pvPacketRaw, ((PACKET_HEADER *) pvPacketRaw)->unPacketLength);

		if (!m_MiniLZO.Compress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), &unDataLength))
			return pvPacketRaw;

		PVOID	pvTempBuffer2	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		UINT16	unDeCompressSize	= APPACKET_MAX_PACKET_SIZE - 10;

		m_MiniLZO.DeCompress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), unDataLength, (BYTE *) pvTempBuffer2 + sizeof(PACKET_HEADER), &unDeCompressSize);

		if (memcmp((CHAR *) pvTempBuffer + sizeof(PACKET_HEADER), (CHAR *) pvTempBuffer2 + sizeof(PACKET_HEADER), unDataLength) != 0)
			ASSERT(0);
		
		((PACKET_HEADER *) pvPacketRaw)->unPacketLength	= unDataLength + sizeof(PACKET_HEADER) + 1;

		*((BYTE *) pvPacketRaw + ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - 1)	= APPACKET_REAR_GUARD_BYTE;
		
		*pnPacketLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= true;
*/

#ifdef _DEBUG
		g_MutexPacket.Lock();
		
//		g_ullCompressedPacketSize[unOriginalLength / 10]	+= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		g_MutexPacket.Unlock();
#endif
	}

	return pvPacketRaw;
}

PVOID AuPacket::MakeStaticPacket(BOOL bIsPacket, INT16 *pnPacketLength, UINT8 cType, ...)
{
	PROFILE("AuPacket::MakePacket");
	
	if (!m_nFlagLength)
		return NULL;

	//m_Flag.m_ulFlag = 0;
	UINT32	ulFlag = 0;

	// ���� ũ�⸦ ���Ѵ�.
	va_list ap;

	UINT16	nLength;
	if (bIsPacket)
		//nLength = sizeof(UINT16)/*packet length*/ + sizeof(UINT8)/*packet type*/ + sizeof(INT32)/*CID*/ + m_nFlagLength;
		nLength = sizeof(PACKET_HEADER) + m_nFlagLength;
	else
		nLength = sizeof(UINT16)/*packet length*/ + m_nFlagLength;

	va_start(ap, cType);

	PVOID	pvBuffer = va_arg(ap, PVOID);
	DWORD	dwMask = 0x01;

	int i = 0, j = 0;
	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			BOOL	bSetFlag = TRUE;

			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength += sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];
					}

					if (j > 0)
					{
						// if packet type is array packet..
						nLength += sizeof(INT8);	// array�� ũ�⸦ �տ� �����Ѵ�.
					}
					else
					{
						bSetFlag = FALSE;
					}
				}
				else
				{
					// field type�� flag�� ��� flag �տ� �ִ� UINT8 ũ���� ���̸� ���� �� ���̸� ����Ѵ�.
					nLength += sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				pvBuffer = va_arg(ap, PVOID);
				if (pvBuffer)
				{
					nLength += sizeof(UINT16) + *(INT16 *) pvBuffer;
				}
			}
			else
			{
				nLength += AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}

			if (bSetFlag)
				ulFlag |= dwMask;
		}

		pvBuffer = va_arg(ap, PVOID);

		dwMask <<= 1;
	}
	va_end(ap);

	if (!nLength)
		return FALSE;

#ifdef	_DEBUG
	ASSERT(nLength > 0);
	ASSERT(nLength < PACKET_BUFFER_SIZE * 40);
#else
	if (nLength < 0 ||
		nLength >= PACKET_BUFFER_SIZE * 40)
		return FALSE;
#endif	//_DEBUG

	//ASSERT(nLength < PACKET_BUFFER_SIZE);

//	InterlockedIncrement(&g_lMakePacketCount);

	if (bIsPacket)
		//nLength += 2;
		nLength += 1;	// end guard byte

//	PVOID pvPacketRaw = g_AuCircularBuffer.Alloc(nLength);		// ��Ŷ �յڷ� �������Ʈ�� ���δ�.
	PVOID pvPacketRaw = (PVOID)new CHAR[nLength];//g_AuCircularBuffer.Alloc(nLength);

	if (!pvPacketRaw)
		return FALSE;

//	PVOID	pvPacket	= NULL;

	if (bIsPacket)
	{
		// set guard byte
		*((BYTE *) pvPacketRaw)					= APPACKET_FRONT_GUARD_BYTE;
		*((BYTE *) pvPacketRaw + nLength - 1)	= APPACKET_REAR_GUARD_BYTE;

//		pvPacket	= (PVOID) ((BYTE *) pvPacketRaw + 1);
	}
//	else
//		pvPacket	= pvPacketRaw;

	PVOID	pvPacket	= pvPacketRaw;

	if (pnPacketLength)
		*pnPacketLength = nLength;

	//*((UINT16 *) pvPacket) = nLength;

	((PACKET_HEADER *) pvPacket)->unPacketLength	= nLength;

	if (bIsPacket)
	{
		//*((INT32 *) ((CHAR *) pvPacket + sizeof(UINT16) + sizeof(UINT8)))	= 0;
		((PACKET_HEADER *) pvPacket)->lOwnerID	=	 0;
		((PACKET_HEADER *) pvPacket)->Flag.Compressed	= 0;
	}

	// ũ�⵵ �˾Ƴ����� ���� ���� ��Ŷ�� �����Ѵ�.

	CHAR* pIndex;
	if (bIsPacket)
	{
		// ��Ŷ Ÿ���� �����Ѵ�.
		//*(UINT8 *) ((CHAR *) pvPacket + sizeof(UINT16)) = cType;
		((PACKET_HEADER *) pvPacket)->cType	= cType;

		// Flag ũ�⸸ŭ m_pvPacket�� �����Ѵ�.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((CHAR *) pvPacket + sizeof(PACKET_HEADER)) = ulFlag;
			break;
		}

		// ���� �ϳ��� ����Ÿ�� ���δ�.
		pIndex = (CHAR *) pvPacket + sizeof(PACKET_HEADER) + m_nFlagLength;
	}
	else
	{
		// Flag ���� �������� ���̸� �����Ѵ�.
		*(UINT16 *) pvPacket = nLength - sizeof(UINT16);

		// Flag ũ�⸸ŭ m_pvPacket�� �����Ѵ�.
		switch (m_nFlagLength) {
		case 1:
			*(UINT8 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 2:
			*(UINT16 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		case 4:
			*(UINT32 *)((UINT8 *) pvPacket + sizeof(UINT16)) = ulFlag;
			break;
		}

		// ���� �ϳ��� ����Ÿ�� ���δ�.
		pIndex = (CHAR *) pvPacket + sizeof(UINT16) + m_nFlagLength;
	}

	va_start(ap, cType);

	pvBuffer = va_arg(ap, PVOID);

	for (i = 0; i < m_cNumField; i++)
	{
		if (pvBuffer)
		{
			if (m_cFieldType[i] == AUTYPE_PACKET)
			{
				if (m_cFieldLength[i] > 1)
				{
					INT8	*pcArraySize = (INT8 *) pIndex;

					pIndex += sizeof(INT8);

					PVOID	*pvPacketArray = (PVOID *) pvBuffer;
					for (j = 0; j < m_cFieldLength[i]; ++j)
					{
						if (!pvPacketArray[j])
							break;

						nLength = sizeof(UINT16) + *(UINT16 *) pvPacketArray[j];

						if (nLength > 0)
							CopyMemory(pIndex, pvPacketArray[j], nLength);

						pIndex += nLength;
					}

					if (j > 0)
						*pcArraySize = j;

					nLength = 0;
				}
				else
				{
					// field type�� flag�� ��� flag �տ� �ִ� UINT16 ũ���� ���̸� ���� �� ���̸� ����Ѵ�.
					nLength = sizeof(UINT16) + *(UINT16 *) pvBuffer;
				}
			}
			else if (m_cFieldType[i] == AUTYPE_MEMORY_BLOCK)
			{
				PVOID pvLength = va_arg(ap, PVOID);
				if (pvLength)
				{
					// ����Ÿ ���̸� ���� ������ �ִ´�.
					CopyMemory(pIndex, pvLength, sizeof(UINT16));
					pIndex += sizeof(UINT16);

					nLength = *(INT16 *) pvLength;
				}
			}
			else
			{
				nLength = AuTypeSize[m_cFieldType[i]] * m_cFieldLength[i];
			}
			
			if (nLength > 0)
				CopyMemory(pIndex, pvBuffer, nLength);

			pIndex += nLength;

			nLength = 0;
		}

		pvBuffer = va_arg(ap, PVOID);
	}
	va_end(ap);

	if (bIsPacket && ((PACKET_HEADER *) pvPacketRaw)->unPacketLength > g_ulMinCompressSize)
	{
		UINT16	unDataLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - sizeof(PACKET_HEADER) - 1 /* End Guard Byte */;

/*		
		PVOID	pvTempBuffer	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);

		CopyMemory(pvTempBuffer, pvPacketRaw, ((PACKET_HEADER *) pvPacketRaw)->unPacketLength);

		if (!m_MiniLZO.Compress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), &unDataLength))
			return pvPacketRaw;

		PVOID	pvTempBuffer2	= (PVOID *) g_AuCircularOutBuffer.Alloc(APPACKET_MAX_PACKET_SIZE);
		UINT16	unDeCompressSize	= APPACKET_MAX_PACKET_SIZE - 10;

		m_MiniLZO.DeCompress((BYTE *) pvPacketRaw + sizeof(PACKET_HEADER), unDataLength, (BYTE *) pvTempBuffer2 + sizeof(PACKET_HEADER), &unDeCompressSize);

		if (memcmp((CHAR *) pvTempBuffer + sizeof(PACKET_HEADER), (CHAR *) pvTempBuffer2 + sizeof(PACKET_HEADER), unDataLength) != 0)
			ASSERT(0);
		
		((PACKET_HEADER *) pvPacketRaw)->unPacketLength	= unDataLength + sizeof(PACKET_HEADER) + 1;

		*((BYTE *) pvPacketRaw + ((PACKET_HEADER *) pvPacketRaw)->unPacketLength - 1)	= APPACKET_REAR_GUARD_BYTE;
		
		*pnPacketLength	= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		((PACKET_HEADER *) pvPacketRaw)->Flag.Compressed	= true;
*/

#ifdef _DEBUG
		g_MutexPacket.Lock();
		
//		g_ullCompressedPacketSize[unOriginalLength / 10]	+= ((PACKET_HEADER *) pvPacketRaw)->unPacketLength;

		g_MutexPacket.Unlock();
#endif
	}

	return pvPacketRaw;
}

BOOL AuPacket::FreePacket(PVOID pvPacket, BOOL bEmbeddedPacket)
{
//	if (!pvPacket)
//		return FALSE;
//
//	InterlockedIncrement(&g_lFreePacketCount);

	return TRUE;
}

BOOL AuPacket::FreeStaticPacket(PVOID pvPacket, BOOL bEmbeddedPacket)
{
	if (!pvPacket)
		return FALSE;

	delete [] (CHAR*)pvPacket;

	return TRUE;
}

BOOL AuPacket::SetFlagLength(INT16 nLength)
{
	m_nFlagLength = nLength;

	return TRUE;
}

INT16 AuPacket::GetFlagLength()
{
	return m_nFlagLength;
}

AuCryptManager& AuPacket::GetCryptManager()
{
	static AuCryptManager csCryptManager;
	return csCryptManager;
}

PVOID AuPacket::EncryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType)
{
	if(!pInput || nSize < 1 || !pnOutputSize)
		return NULL;

	// ��ȣȭ ���� �ʴ� ��Ȳ�̶�� �������ְ� ������.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	if(eCryptType == AUCRYPT_TYPE_NONE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}
	else if(eCryptType == AUCRYPT_TYPE_BOTH)
	{
		// �̷��� �ϸ� �峭ģ ����.
		ASSERT(!"AuPacket::EncryptPacket(...) eCryptType is BOTH");
		return NULL;
	}

	// Packet Header �� ��´�.
	ENC_PACKET_HEADER pHeader = *(ENC_PACKET_HEADER*)pInput;

	// �̹� ��ȣȭ �Ǿ� �ִٸ� ���ϰ� �������ְ� ������.
	//if ((pHeader.bGuardByte == APENCPACKET_FRONT_PUBLIC_BYTE	&& *((BYTE*)pInput + nSize - 1) == APENCPACKET_REAR_PUBLIC_BYTE) ||
	//	(pHeader.bGuardByte == APENCPACKET_FRONT_PRIVATE_BYTE	&& *((BYTE*)pInput + nSize - 1) == APENCPACKET_REAR_PRIVATE_BYTE))
	if(IS_ENC_PACKET(pInput, nSize))
	{
		*pnOutputSize = nSize;
		return pInput;
	}

    // ��ȣȭ ���� ũ�⸦ ����Ѵ�.
	INT16 nOutputSize = (INT16)AuPacket::GetCryptManager().GetOutputSize(eCryptType, nSize);
	nOutputSize += sizeof(ENC_PACKET_HEADER) + 1;	// Encrypted Packet Header + Encrypted Rear Byte

#ifdef	_DEBUG
	ASSERT(nOutputSize > 0);
	ASSERT(nOutputSize < APPACKET_MAX_PACKET_SIZE);
#else
	if(nOutputSize < 0 || nOutputSize >= APPACKET_MAX_PACKET_SIZE)
		return NULL;
#endif//_DEBUG

	*pnOutputSize = nOutputSize;

	// �޸𸮸� �Ҵ��Ѵ�.
	PVOID pOutput = g_AuCircularBuffer.Alloc(nOutputSize);

	// Dummy ���� 2008.04.01. steeple
	//SetDummy(pInput, nSize);

	// ��ȣȭ�Ѵ�.
	if(AuPacket::GetCryptManager().Encrypt(csCryptActor, eCryptType, (BYTE*)pInput, (BYTE*)pOutput + sizeof(ENC_PACKET_HEADER), nSize) < 1)
	{
		FreeEncryptBuffer(pOutput, nOutputSize);
		ASSERT(!"AuPacket::EncryptPacket(...) m_csCryptManager.Encrypt returned failiure");
		return NULL;
	}

	// ��ȣȭ�� ��Ŷ ����� �����Ѵ�.
	((ENC_PACKET_HEADER*)pOutput)->unPacketLength = nOutputSize;
	//((ENC_PACKET_HEADER*)pOutput)->ulSeqID = ++ulSendSeqID;
	if(eCryptType == AUCRYPT_TYPE_PUBLIC)
	{
		((ENC_PACKET_HEADER*)pOutput)->bGuardByte = APENCPACKET_FRONT_PUBLIC_BYTE;
		*((BYTE*)pOutput + nOutputSize - 1) = APENCPACKET_REAR_PUBLIC_BYTE;
	}
	else if(eCryptType == AUCRYPT_TYPE_PRIVATE)
	{
		((ENC_PACKET_HEADER*)pOutput)->bGuardByte = APENCPACKET_FRONT_PRIVATE_BYTE;
		*((BYTE*)pOutput + nOutputSize - 1) = APENCPACKET_REAR_PRIVATE_BYTE;
	}

	return pOutput;
}

// 2006.04.12. steeple
PVOID AuPacket::DecryptPacket(PVOID pInput, INT16 nSize, INT16* pnOutputSize, AuCryptActor& csCryptActor, eAuCryptType eCryptType)
{
	if(!pInput || nSize < 1 || !pnOutputSize)
		return NULL;

	// ��ȣȭ ���� �ʴ� ��Ȳ�̶�� �������ְ� ������.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	if(eCryptType == AUCRYPT_TYPE_NONE)
	{
		*pnOutputSize = nSize;
		return pInput;
	}
	else if(eCryptType == AUCRYPT_TYPE_BOTH)
	{
		// �̷��� �ϸ� �峭ģ ����.
		ASSERT(!"AuPacket::DecryptPacket(...) eCryptType is BOTH");
		return NULL;
	}

	// Packet Header �� ��´�.
	ENC_PACKET_HEADER pHeader = *(ENC_PACKET_HEADER*)pInput;

	// ��ȣȭ �Ǿ� �ִ� ���°� �ƴ϶�� ���ϰ� �������ְ� ������.
	if(!IS_ENC_PACKET(pInput, nSize))
	{
		*pnOutputSize = nSize;
		return pInput;
	}

	// �Ϲ������� "��ȣȭ�� ũ�� >= ���� ũ��" �̴�.
	// �׷��� ��ȣȭ �� ���� ���� ���۸� ���� �ʰ� �׳� ���� �޸𸮸� output ���� �ִ´�.
	//
	//
	// �׷�����, �˰��� ���� Output Buffer �� �ʿ��ϴٸ� �Ҵ��Ѵ�. AuSeed �� �߰��Ǹ鼭 �����.
	BYTE* pInputStart = NULL, *pOutput = NULL;
	if(AuPacket::GetCryptManager().UseDecryptBuffer(eCryptType) == FALSE)
		pOutput = (BYTE*)pInput + sizeof(ENC_PACKET_HEADER);
	else
	{
		PVOID pPacket = g_AuCircularBuffer.Alloc(nSize - sizeof(ENC_PACKET_HEADER) - 1);
		if(!pPacket)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - Alloc fail", __FUNCTION__, __LINE__);
			AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

			return NULL;
		}

		pOutput = (BYTE*)pPacket;
	}

	pInputStart = (BYTE*)pInput + sizeof(ENC_PACKET_HEADER);	// ��ȣȭ �� �κб��� �ѱ��.
	AuPacket::GetCryptManager().Decrypt(csCryptActor, eCryptType, pInputStart, pOutput, nSize - sizeof(ENC_PACKET_HEADER) - 1);	// -(Encrypted Header) -(Encrypted Rear Byte)

	*pnOutputSize = ((PACKET_HEADER*)pOutput)->unPacketLength;

	return (PVOID)pOutput;
}

// 2006.04.12. steeple
void AuPacket::FreeEncryptBuffer(PVOID pBuffer, INT16 nSize)
{
	// ����� �ƹ� �ϵ� ���Ѵ�. 2006.04.12
	// �ֳ��ϸ� Circular Buffer �� ���� ������ ���� ������ �ʿ�� ����.
}

// 2008.04.01. steeple
//BOOL AuPacket::SetDummy(PVOID pvPacket, INT16 nSize)
//{
//	// ��ȣȭ �ϱ� ���� Header �� dummy �� �����Ѵ�.
//	// ���� ��ȣȭ ��Ŷ�� ��ȣȭ �� �Ŀ� ������ �ٸ��� �ϱ� ���� ����.
//	clock_t clock_ = clock();
//	((PACKET_HEADER*)pvPacket)->lDummy = (UINT32)clock_;
//
//	return TRUE;
//}

void
AuPacket::SetFrameTick(UINT32 ulFrameTick)
{
	g_ulFrameTick = ulFrameTick;
}

UINT32
AuPacket::GetLastProcessedFrameTick()
{
	return g_ulLastProcessedFrameTick;
}