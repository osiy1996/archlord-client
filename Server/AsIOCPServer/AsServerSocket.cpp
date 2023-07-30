// AsServerSocket.cpp: implementation of the AsServerSocket class.
//
//////////////////////////////////////////////////////////////////////

#include "AsServerSocket.h"
#include "AsIOCPServer.h"
#include "ApMemory.h"
#include "AuCircularBuffer.h"
#include "AuPacket.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define ASSERVERSOCKET_ERROR_FILENAME "LOG\\AsServerSocket_Error.log"
#define ASSERVERSOCKET_PRIORITY_DESTROY "LOG\\AsServerSocket_PriorityDestroy.log"

AuCircularBuffer	g_AuCircularSendBuffer;

BOOL AsServerSocket::AsSendBuffer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	AsServerSocket	*pThis	= (AsServerSocket *) pClass;
	
	AuAutoLock lock(pThis->m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	if (pThis->m_WSABufferManager.bSetTimerEvent)
		pThis->m_WSABufferManager.bSetTimerEvent = FALSE;

	return pThis->SendBufferedPacket();
}

AsServerSocket::AsServerSocket() 
	: AsObject(ASOBJECT_SOCKET), m_BufferIn(NULL)
{
	m_lIndex	= INVALID_INDEX;
	m_pcsTimer	= NULL;
	m_csCriticalSection.Init();

	m_ovlOut.hEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	m_ovlOut.hEvent = UlongToHandle((HandleToUlong(m_ovlOut.hEvent) | 0x1));

	m_ulSendSeqID = m_ulRecvSeqID = 0;
	m_ulInvalidEncPacketCount = 0;

	m_BufferIn = new CHAR[MAX_BUFFER_SIZE+1];
	if( m_BufferIn )
		memset(m_BufferIn, 0, MAX_BUFFER_SIZE+1);

	Create();
}

AsServerSocket::~AsServerSocket()
{
	if (m_ovlOut.hEvent)
	{
		if ( FALSE == ::CloseHandle((HANDLE) ((DWORD_PTR)m_ovlOut.hEvent & ~0x1)) )
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "CloseHandle Error : %d, Index : %d", GetLastError(), m_lIndex);
		}

		m_ovlOut.hEvent = NULL;
	}

	if(m_BufferIn)
		delete[] m_BufferIn;

	m_BufferIn = NULL;

	m_csCriticalSection.Destroy();
}

VOID AsServerSocket::SetIndex(INT32 lIndex)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (Lock.Result())
		m_lIndex = lIndex;
}

VOID AsServerSocket::InitData()
{
	m_hSocket	= INVALID_SOCKET;
	m_eState	= ASSERVER_STATE_INIT;
	m_eSendStatus = SEND_OVERLAPPED_STATUS_READY;

	m_lBufferInOffset	= 0;
	m_lProcessIndex		= 0;
//	m_lRemainDataSize	= 0;
//	m_lRemainOverlappedCount = 0;

	::ZeroMemory(&m_ovlIn.Internal, sizeof(OVERLAPPED));
	::ZeroMemory(&m_ovlOut.Internal, sizeof(OVERLAPPED));
	::ZeroMemory(&m_ovlSend.Internal, sizeof(OVERLAPPED));

	memset(m_BufferIn, 0, MAX_BUFFER_SIZE+1);
	::ZeroMemory(&m_stPlayerData, sizeof(PlayerData));
#ifdef _AREA_CHINA_
	m_bIsPCRoom	= FALSE;
#endif
	m_bCallback = TRUE;
	m_bIsProcessReuse	= FALSE;

	m_bSendReserved = FALSE;

	m_ulConnectedTimeMSec = 0;
	m_ulLastSendTimeStamp = 0;
	m_ulLastReceiveTimeStamp = 0;

	InitializeAll();

	SetSocketType(SOCKET_TYPE_ARCHLORD);
}

SOCKET AsServerSocket::GetHandle()
{
	return m_hSocket;
}

BOOL AsServerSocket::IsRead(OVERLAPPED *povl)
{
	if (&m_ovlIn == povl) return TRUE;
	else return FALSE;
}

BOOL AsServerSocket::IsActive()
{
	if (m_eState == ASSERVER_STATE_ALIVE)
		return TRUE;
	else return FALSE;
}

SOCKADDR_IN* AsServerSocket::GetRemoteInfo()
{
	return &m_stRemoteInfo;
}

char* AsServerSocket::GetRemoteIPAddress(char* szBuff, int nSize)
{
	if (nSize < MAX_IP_ADDRESS_STRING + 1)
	{
		return NULL;
	}

	sprintf(szBuff, "%d.%d.%d.%d", (unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b1,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b2,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b3,
									(unsigned long)m_stRemoteInfo.sin_addr.S_un.S_un_b.s_b4);

	return szBuff;
}

BOOL AsServerSocket::Connect(LPCTSTR lpAddress, INT32 lPort)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return FALSE;

	m_stRemoteInfo.sin_family = AF_INET;
	m_stRemoteInfo.sin_addr.s_addr = inet_addr(lpAddress);
	m_stRemoteInfo.sin_port = htons(lPort);
#ifdef _DEBUG
	printf("::connect(%s %d)\n",lpAddress, lPort);
#endif

	BOOL				bRet = FALSE;
	INT32				secTimeOut = 10; // 10 sec
	{
		fd_set				stFDSConn;
		struct timeval		timeout;
		u_long 				uNonBlock = 1L;

		ioctlsocket( m_hSocket, FIONBIO, &uNonBlock );

		//
		connect(m_hSocket, (struct sockaddr*)&m_stRemoteInfo, sizeof(SOCKADDR_IN));

		while( secTimeOut-- )
		{
			FD_ZERO( &stFDSConn );
			FD_SET( m_hSocket, &stFDSConn );

			timeout.tv_sec	= 1;
			timeout.tv_usec	= 0;

			if (select( (int) m_hSocket+1, NULL, &stFDSConn, NULL, &timeout) > 0 )
			{
				if (FD_ISSET ( m_hSocket, &stFDSConn ))
				{
					bRet = TRUE;
					break;
				}
			}
		} // end while

		if( bRet == FALSE && secTimeOut == 0 )
		{
			SetLastError( WSAETIMEDOUT );
		}

		uNonBlock = 0;
		ioctlsocket( m_hSocket, FIONBIO, &uNonBlock );
	}

	m_stPlayerData.bServer	= bRet;

	return bRet;
}

// 2006.04.14. steeple
// const char* > CHAR* (��ȣȭ�� �ȿ��� �����ϱ� ������ ������� ������ �ȵȴ�.)
EnumAsyncSendResult AsServerSocket::AsyncSend(CHAR* pData, INT32 lLength, PACKET_PRIORITY ePriority)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return ASYNC_SEND_FAIL;

	if (!IsActive()) 
		return ASYNC_SEND_SOCKET_NOT_ACTIVE;

	if (SOCKET_TYPE_ARCHLORD != GetSocketType())
	{
		if (SendRawData((char *) pData, lLength))
			return ASYNC_SEND_SUCCESS;
		else
			return ASYNC_SEND_FAIL;
	}

	PPACKET_HEADER pHeader2 = (PPACKET_HEADER)pData;
	ASSERT ( pHeader2->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader2->unPacketLength > sizeof(PACKET_HEADER));
	ASSERT ( lLength > sizeof(PACKET_HEADER));

	if(!IS_ENC_PACKET(pData, lLength))
	{
		ASSERT ( pHeader2->bGuardByte == 0xD6 );
		ASSERT ( *(CHAR*)((CHAR*)pHeader2 + pHeader2->unPacketLength - 1) == 0x6B );
	}
	else
	{
		ASSERT ( *(BYTE*)pData == APENCPACKET_FRONT_PUBLIC_BYTE );
		ASSERT ( *((BYTE*)pData + pHeader2->unPacketLength - 1) == APENCPACKET_REAR_PUBLIC_BYTE );
	}
/*
	if (!m_stPlayerData.bServer)
	{
		AuAutoLock lock(m_csCriticalSection);
		if ((m_lRemainDataSize + lLength) > EMERGENCY_SOCKET_BUFFER_SIZE)
		{
			printf("!!!! ����� �Ѿ�� ��Ŷ�� �����Ѵ�.\n");
			return ASYNC_SEND_BUFFER_FULL;
		}
	}
*/
	if (m_stPlayerData.bServer)
	{
		if (TRUE == SendSystemData((CHAR *)pData, lLength))
			return ASYNC_SEND_SUCCESS;
		else
			return ASYNC_SEND_FAIL;
	}

	// ��ȣȭ�� �ؾ��ϴµ�, ��ȣȭ�� ��Ŷ�� �ƴ϶�� ���ش�.
	// �̹� ��ȣȭ�� ��Ŷ�̶�� (��Ƽĳ���� ��) ���� �ʴ´�.
	if(AuPacket::GetCryptManager().IsUseCrypt() && 
		m_csCryptActor.GetCryptType() != AUCRYPT_TYPE_NONE &&
		!IS_ENC_PACKET(pData, lLength))
	{
		// �������� ��ȣȭ�� Public ���� �Ѵ�.
		INT16 nOutputSize = 0;
		PVOID pOutput = AuPacket::EncryptPacket(pData, lLength, &nOutputSize, m_csCryptActor, AUCRYPT_TYPE_PUBLIC);

		(CHAR*)pData = (CHAR*)pOutput;
		lLength = (INT32)nOutputSize;

		// ��ȣȭ�� ��Ŷ�̶�� SendSeqID ���� �� ����
		UINT32 ulSeqID = ++m_ulSendSeqID;
		if(m_ulSendSeqID == 0)		// 0�� �Ǿ��ٸ� 1�� �ٲ۴�.
			ulSeqID = m_ulSendSeqID = 1;

		((PENC_PACKET_HEADER)pData)->ulSeqID = ulSeqID;
	}

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	if (IS_ENC_PACKET(pData, lLength) || pHeader->lOwnerID == 0 || m_stPlayerData.lID == 0 || pHeader->lOwnerID == m_stPlayerData.lID)
	{
		EnumBufferingResult	eResult	= PushSendBuffer((CHAR *) pData, lLength, PACKET_PRIORITY_1);
		if(eResult != ASSERVER_BUFFERING_SUCCESS)
			return ASYNC_SEND_FAIL;

		AuAutoLock lock(m_csCriticalSection);
		if (lock.Result())
		{
			if (SEND_OVERLAPPED_STATUS_SEND == m_eSendStatus)
			{
				m_bSendReserved = TRUE;
				return ASYNC_SEND_SUCCESS;
			}

			EmptyBuffer();
	//		if (TRUE == SendUniData((CHAR *) pData, lLength))
	//			return ASYNC_SEND_SUCCESS;
	//		else
	//			return ASYNC_SEND_FAIL;
		}
	}
	else
	{
		EnumBufferingResult	eResult	= PushSendBuffer((CHAR *) pData, lLength, ePriority);

		switch (eResult) {
		case ASSERVER_BUFFERING_FULL:
		case ASSERVER_BUFFERING_NOT_USE:
			{
				if (TRUE == SendUniData((CHAR *) pData, lLength))
					return ASYNC_SEND_SUCCESS;
				else
					return ASYNC_SEND_FAIL;
			}
			break;

		case ASSERVER_BUFFERING_FAIL:
			{
				return ASYNC_SEND_FAIL;
			}
		}
	}

	return ASYNC_SEND_SUCCESS;
}

BOOL AsServerSocket::AsyncRecv()
{ 
	if (!IsActive()) return FALSE;
	
	DWORD dwBytesRead = 0;
	DWORD dwFlags = 0;
//	BOOL bResult = ::ReadFile((HANDLE)m_hSocket, &m_BufferIn[m_lProcessIndex + m_lBufferInOffset], 
//						MAX_BUFFER_SIZE - m_lProcessIndex - m_lBufferInOffset, &dwBytesRead, &m_ovlIn);
	m_wsaBufferIn.len = MAX_BUFFER_SIZE - m_lProcessIndex - m_lBufferInOffset;
	m_wsaBufferIn.buf = &m_BufferIn[m_lProcessIndex + m_lBufferInOffset];
	if (SOCKET_ERROR == ::WSARecv((SOCKET)m_hSocket, &m_wsaBufferIn, 1, &dwBytesRead, &dwFlags, &m_ovlIn, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::AsyncRecv %d, bServer %d", dwError, m_stPlayerData.bServer);
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::Create()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return FALSE;

	InitData();

	m_ovlIn.pSocket = this;
	m_ovlOut.pSocket = this;
	m_ovlSend.pSocket = this;

	m_hSocket = ::WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if( m_hSocket == INVALID_SOCKET )
		return FALSE;

	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0; /* hard shutdown */
	if( ::setsockopt(m_hSocket, SOL_SOCKET, SO_LINGER, (const char*)&lin, sizeof(linger)) == -1 )
		return FALSE;

	struct timeval tv_timeo = { 3, 500000 };  /* 3.5 second */
	if( ::setsockopt( m_hSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv_timeo, sizeof( tv_timeo ) ) == -1 )
		return FALSE;

	if( ::setsockopt( m_hSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&tv_timeo, sizeof( tv_timeo ) ) == -1 )
		return FALSE;

	m_WSABufferManager.Initialize();

	return TRUE;
}

VOID AsServerSocket::Remove()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return;

	if (INVALID_SOCKET == m_hSocket) return;

	if (SOCKET_ERROR == ::closesocket(m_hSocket))
	{
		//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "CloseHandle Error : %d, Index : %d", GetLastError(), m_lIndex);
	}

	m_hSocket = INVALID_SOCKET;
}

VOID AsServerSocket::GetRemoteAddress()
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return;

	// 1. �ּ� �м� �� ����.
	LPSOCKADDR_IN lpRemoteAddr	= NULL;
	int	nRemoteSockAddrLen		= 0;

	::GetAcceptExSockaddrs((LPWSABUF)&m_BufferIn[0], 0, 0, sizeof(SOCKADDR_IN) + 16, NULL, NULL, 
							(LPSOCKADDR*)&lpRemoteAddr, &nRemoteSockAddrLen);

	if(NULL != lpRemoteAddr)
		m_stRemoteInfo = *lpRemoteAddr;
}

CHAR* AsServerSocket::GetCompletePacket(BOOL *pbIsInvalidPacket)
{
	ASSERT(SOCKET_TYPE_NONE != GetSocketType());

	switch (GetSocketType())
	{
	case SOCKET_TYPE_ARCHLORD:	return GetCompleteArchlordServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_LKSERVER:	return GetCompleteLKServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_BILLING:	return GetCompleteBillingServerPacket(pbIsInvalidPacket);
	case SOCKET_TYPE_PATCH:		return GetCompletePatchServerPacket(pbIsInvalidPacket);
	default :
		ASSERT(SOCKET_TYPE_NONE != GetSocketType());
	}

	return NULL;
}

CHAR* AsServerSocket::GetCompleteArchlordServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(PACKET_HEADER))
		return NULL;

	UINT16 lPacketLength = ((PPACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->unPacketLength;

	if (lPacketLength <= sizeof(PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// �ϳ��� ��Ŷ�� �ϼ��Ҽ� �ִ� ����� ���۸� Ȯ���ϰ� �ִ��� Ȯ��
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// ���ŵ� ��Ŷ�� ���̸� Ȯ���Ͽ� �ϳ��� ������ ��Ŷ�� ���� �Ǿ����� Ȯ���Ѵ�.
	if (lPacketLength <= m_lBufferInOffset)
	{
		// ù ����Ʈ�� ��ȣȭ�� ���̶�� ��ȣȭ ����Ʈ�� �˻��Ѵ�.
		if(AuPacket::GetCryptManager().IsUseCrypt() &&
			IS_ENC_FRONT_BYTE(*(BYTE*) &m_BufferIn[m_lProcessIndex]))
		{
			if (!IS_ENC_PACKET((BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength))
			{
				ASSERT(!"ù ����Ʈ�� ��ȣȭ ����Ʈ �ε�, ��ȣȭ ��Ŷ�� �ƴϻ�");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_NOT_ENC_PACKET, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			// �ϷĹ�ȣ �˻絵 �Ѵ�. ���÷��� ������ �����Ѵ�.
			if (((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID != 1 &&
				((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID <= GetRecvSeqID())
			{
				ASSERT(!"��ȣȭ ��Ŷ�� SeqID �� �� ���Ի�. ���÷��� ������ �ǽɵ�.");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_REPLAY_ATTACK, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}
			else if(((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID == 1 &&
					GetRecvSeqID() == 1)
			{
				ASSERT(!"��ȣȭ ��Ŷ�� SeqID �� 1�� ��� ����. ���÷��� ������ �ǽɵ�.");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				WriteInvalidPacketLog(INVALID_PACKET_TYPE_REPLAY_ATTACK, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			// �ϷĹ�ȣ�� 1 �� ������ RecvSeqID �� 1�� �������ش�.
			if (((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID == 1)
				SetRecvSeqID(1);

			// �ƴϸ� ���� ������ ����
			else
				SetRecvSeqID(((PENC_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->ulSeqID);

			// ��ȣȭ �Ѵ�.
			INT16 nOutputSize = 0;
			pResult = (CHAR*)AuPacket::DecryptPacket(&m_BufferIn[m_lProcessIndex], lPacketLength, &nOutputSize, GetCryptActorRef());
			if(!pResult)
			{
				*pbIsInvalidPacket	= FALSE;

				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}

			// ��ȣȭ�� ��Ŷ�� �ٽ� Ȯ���Ѵ�.
			if (nOutputSize <= sizeof(PACKET_HEADER) ||
				nOutputSize >= MAX_BUFFER_SIZE)
			{
				//ASSERT(!"��ȣȭ ��, ��Ŷ ����� �����");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID, (BYTE*)pResult, nOutputSize);
				
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail (2)", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}

			// check guard byte
			if (((PPACKET_HEADER)pResult)->bGuardByte != APPACKET_FRONT_GUARD_BYTE ||
				*((BYTE*)pResult + nOutputSize - 1) != APPACKET_REAR_GUARD_BYTE)
			{
				//TRACE("��ȣȭ ��, �������Ʈ ��������");
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR, (BYTE*)pResult, nOutputSize);

				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d] - DecryptPacket fail (3)", __FUNCTION__, __LINE__);
				AuLogFile_s("LOG\\CircularBuffer.log", strCharBuff);

				return NULL;
			}
		}
		else
		{
			// check guard byte
			if (((PPACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->bGuardByte != APPACKET_FRONT_GUARD_BYTE ||
				*(BYTE *) &m_BufferIn[m_lProcessIndex + lPacketLength - 1] != APPACKET_REAR_GUARD_BYTE)
			{
				*pbIsInvalidPacket	= FALSE;
				IncreasetInvalidEncPacketCount();
				//WriteInvalidPacketLog(INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR, (BYTE*)&m_BufferIn[m_lProcessIndex], lPacketLength);
				return NULL;
			}

			pResult = &m_BufferIn[m_lProcessIndex];
		}

		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// ������ ��Ŷ�� �ƴϸ� ���� ��Ŷ�� ��ٸ���.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompleteLKServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// ��� ����� �ٸ���. ���̶����,, �ϴ� �κп� ���� üũ�� �ٸ��� �������� �Ѵ�.

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(LK_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(LK_PACKET_HEADER))
		return NULL;

	UINT16 lPacketLength = ::ntohs(((PLK_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->unLen) + sizeof(LK_PACKET_HEADER);

	if (lPacketLength < sizeof(LK_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// �ϳ��� ��Ŷ�� �ϼ��Ҽ� �ִ� ����� ���۸� Ȯ���ϰ� �ִ��� Ȯ��
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// ���ŵ� ��Ŷ�� ���̸� Ȯ���Ͽ� �ϳ��� ������ ��Ŷ�� ���� �Ǿ����� Ȯ���Ѵ�.
	if (lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// ������ ��Ŷ�� �ƴϸ� ���� ��Ŷ�� ��ٸ���.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompleteBillingServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// Billing format
	// 2byte (return code)
	// 5byte (payload size - end flag ���� size)
	// nbyte (payload) 
	// end flag (\n)

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(BILLING_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(BILLING_PACKET_HEADER))
		return NULL;


	UINT16 lPacketLength = ::atoi(((PBILLING_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->cLength) + 2;

	if (lPacketLength < sizeof(BILLING_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// �ϳ��� ��Ŷ�� �ϼ��Ҽ� �ִ� ����� ���۸� Ȯ���ϰ� �ִ��� Ȯ��
	if (lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// ���ŵ� ��Ŷ�� ���̸� Ȯ���Ͽ� �ϳ��� ������ ��Ŷ�� ���� �Ǿ����� Ȯ���Ѵ�.
	if (lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// ������ ��Ŷ�� �ƴϸ� ���� ��Ŷ�� ��ٸ���.
		pResult = NULL;
	}

	return pResult;
}

CHAR* AsServerSocket::GetCompletePatchServerPacket(BOOL *pbIsInvalidPacket)
{
	AuAutoLock Lock(m_csCriticalSection);
	if (!Lock.Result()) return NULL;

	if (!pbIsInvalidPacket)
		return NULL;

	CHAR* pResult = NULL;
	if (0 == m_lBufferInOffset) return NULL;

	// Billing format
	// 2byte (return code)
	// 5byte (payload size - end flag ���� size)
	// nbyte (payload) 
	// end flag (\n)

	if (MAX_BUFFER_SIZE - m_lProcessIndex < sizeof(PATCH_PACKET_HEADER))
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}
	
	if (m_lBufferInOffset < sizeof(PATCH_PACKET_HEADER))
		return NULL;


	UINT32 lPacketLength = ((PPATCH_PACKET_HEADER)(&m_BufferIn[m_lProcessIndex]))->m_iSize;

	if (lPacketLength < sizeof(PATCH_PACKET_HEADER) ||
		lPacketLength >= MAX_BUFFER_SIZE)
	{
		*pbIsInvalidPacket	= FALSE;
		return NULL;
	}

	// �ϳ��� ��Ŷ�� �ϼ��Ҽ� �ִ� ����� ���۸� Ȯ���ϰ� �ִ��� Ȯ��
	if ((INT32)lPacketLength >= MAX_BUFFER_SIZE - m_lProcessIndex)	
	{
		// Check Array Validation
		m_BufferIn[m_lProcessIndex + m_lBufferInOffset - 1];

		// ������ ũ�Ⱑ �����ϸ� �޸𸮸� �̵���Ų��.
		::memmove(&m_BufferIn[0], &m_BufferIn[m_lProcessIndex], m_lBufferInOffset);
		m_lProcessIndex = 0;
		return NULL;
	}

	// ���ŵ� ��Ŷ�� ���̸� Ȯ���Ͽ� �ϳ��� ������ ��Ŷ�� ���� �Ǿ����� Ȯ���Ѵ�.
	if ((INT32)lPacketLength <= m_lBufferInOffset)
	{
		pResult = &m_BufferIn[m_lProcessIndex];
		m_lProcessIndex = m_lProcessIndex + lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}
	else
	{
		// ������ ��Ŷ�� �ƴϸ� ���� ��Ŷ�� ��ٸ���.
		pResult = NULL;
	}

	return pResult;
}

EnumBufferingResult AsServerSocket::PushSendBuffer(CHAR *szPacket, INT32 nSize, PACKET_PRIORITY ePriority)
{
	if (!m_pcsTimer)
		return ASSERVER_BUFFERING_NOT_USE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) 
		return ASSERVER_BUFFERING_FAIL;

	if (ADD_PACKET_RESULT_SUCCESS != m_WSABufferManager.AddPacket(szPacket, nSize, ePriority))
	{
#ifdef _DEBUG
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::PushSendBuffer SocketIndex : %d, AccountName : %s", m_lIndex, m_stPlayerData.szAccountName);
		AuLogFile_s(ASSERVERSOCKET_PRIORITY_DESTROY, strCharBuff);
#endif

		return ASSERVER_BUFFERING_FAIL;
	}

	if (m_bSendReserved) 
		return ASSERVER_BUFFERING_SUCCESS;

	if (!m_WSABufferManager.bSetTimerEvent)
	{
		m_pcsTimer->AddTimer(200, m_lIndex, (PVOID) this, AsSendBuffer, NULL);
		m_WSABufferManager.bSetTimerEvent = TRUE;
	}

	return ASSERVER_BUFFERING_SUCCESS;
}

BOOL AsServerSocket::SendBufferedPacket()
{
	return EmptyBuffer();
}

BOOL AsServerSocket::EmptyBuffer()
{
	if (SEND_OVERLAPPED_STATUS_SEND == m_eSendStatus)
	{
		m_bSendReserved = TRUE;
		return TRUE;
	}

	if (m_WSABufferManager.GetTotalBufferSize() < 1)
		return TRUE;

	INT32 lSize = sizeof(WSABUF) * m_WSABufferManager.GetCurrentCount();
	PVOID pBuffer = g_AuCircularSendBuffer.Alloc(lSize);
	memcpy(pBuffer, m_WSABufferManager.GetStartBuffer(), lSize);
	SendMultiData((LPWSABUF)pBuffer, m_WSABufferManager.GetCurrentCount(), m_WSABufferManager.GetTotalBufferSize());

	m_WSABufferManager.BufferClear();
//	m_WSABufferManager.bSetTimerEvent = FALSE;

	return TRUE;
}

BOOL AsServerSocket::SendUniData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send�� Overlapped ��ü�� ������ ����
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, pSendOverlapped, NULL))
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::SendUniData %d, bServer %d", dwError, m_stPlayerData.bServer);
			// Send�Ҽ� ���� ��Ȳ�̸� DestroyClient��Ų��.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SendSystemData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send�� Overlapped ��ü�� ������ ����
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	PPACKET_HEADER pHeader = (PPACKET_HEADER)pData;
	ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
	ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
#ifdef _DEBUG
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::SendSystemData %d, bServer %d", dwError, m_stPlayerData.bServer);
			AuLogFile_s(ASSERVERSOCKET_ERROR_FILENAME, strCharBuff);
#endif
			// Send�Ҽ� ���� ��Ȳ�̸� DestroyClient��Ų��.
//			delete pSendOverlapped;
			return FALSE;
		}
	}
	return TRUE;
}

BOOL AsServerSocket::SendMultiData(LPWSABUF lpBuffers, INT32 lBufferCount, INT32 lSize)
{
	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

#ifdef _DEBUG
	for (INT32 i = 0; i < lBufferCount; ++i)
	{
		PPACKET_HEADER pHeader = (PPACKET_HEADER)lpBuffers[i].buf;
		ASSERT ( pHeader->unPacketLength < APPACKET_MAX_PACKET_SIZE);
		ASSERT ( pHeader->unPacketLength > sizeof(PACKET_HEADER));
	}
#endif

//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuffers, lBufferCount, &dwBytesSent, 0, pSendOverlapped, NULL))
	m_eSendStatus = SEND_OVERLAPPED_STATUS_SEND;	
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuffers, lBufferCount, &dwBytesSent, 0, &m_ovlSend, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
#ifdef _DEBUG
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AsServerSocket::SendMultiData %d, bServer %d", dwError, m_stPlayerData.bServer);
			AuLogFile_s(ASSERVERSOCKET_ERROR_FILENAME, strCharBuff);
#endif
			// Send�Ҽ� ���� ��Ȳ�̸� DestroyClient��Ų��.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SendRawData(CHAR *pData, INT32 lSize)
{
	if (!pData || lSize < 1)
		return FALSE;

	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	// Send�� Overlapped ��ü�� ������ ����
	LPWSABUF lpBuf = (LPWSABUF)g_AuCircularSendBuffer.Alloc(sizeof(WSABUF));
	lpBuf->buf = pData;
	lpBuf->len = lSize;

	DWORD dwBytesSent = 0;
//	AsOverlapped* pSendOverlapped = new AsOverlapped;
//	pSendOverlapped->pSocket = this;
//	InterlockedExchangeAdd(&m_lRemainDataSize, (LONG)lSize);
//	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, pSendOverlapped, NULL))
	if (SOCKET_ERROR == ::WSASend((SOCKET)m_hSocket, lpBuf, 1, &dwBytesSent, 0, &m_ovlOut, NULL))
	{
		DWORD dwError = ::WSAGetLastError();
		if (ERROR_IO_PENDING != dwError)
		{
			//AuLogFile(ASSERVERSOCKET_ERROR_FILENAME, "AsServerSocket::SendUniData %d, bServer %d", dwError, m_stPlayerData.bServer);
			// Send�Ҽ� ���� ��Ȳ�̸� DestroyClient��Ų��.
//			delete pSendOverlapped;
			return FALSE;
		}
	}

	return TRUE;
}

BOOL AsServerSocket::SetPCRoom()
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	m_bIsPCRoom	= TRUE;

	return TRUE;
}

BOOL AsServerSocket::IsPCRoom()
{
	return m_bIsPCRoom;
}

BOOL AsServerSocket::SetSocketType(EnumSocketType eType)
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return FALSE;

	m_eSocketType	= eType;

	return TRUE;
}

EnumSocketType AsServerSocket::GetSocketType()
{
	return m_eSocketType;
}

//void AsServerSocket::SendCompletionSize(LONG lSize)
//{
//	InterlockedExchangeAdd(&m_lRemainDataSize, -lSize);
//}

VOID AsServerSocket::SendComplete()
{
	AuAutoLock lock(m_csCriticalSection);
	if (!lock.Result()) return;

	m_eSendStatus = SEND_OVERLAPPED_STATUS_READY;
	if (m_bSendReserved)
		EmptyBuffer();

	m_bSendReserved = FALSE;
}

// 2006.06.05. steeple
// ��ȣȭ �ϴ� �� ���ϴ� ���� ����, �� �� Ʋ�ȴ��Ŀ� ���� ���� ��Ȯ�ϰ� ����.
//
// 2006.04.15. steeple
// ��ȣȭ ó���� �ؾ� �Ǵ� �����ε� �������� ��Ŷ�� ���� ��.
// AuCryptManager::GetCryptCheckLevel �� ����Ͽ��� �� �̻� ������ Ŭ���̾�Ʈ�� ©�� �� �����Ѵ�.
BOOL AsServerSocket::CheckInvalidPacketCount()
{
	// ��ȣȭ�� ���� �ʾƵ� �Ǵ� ��Ȳ�� ���� �������Ʈ �ѹ��̶� �̽����� ©�������.
	if(AuPacket::GetCryptManager().IsUseCrypt() == FALSE ||
		/*m_stPlayerData.bServer == TRUE ||*/  // ������ ������ ���� ���� - arycoat 2009.12.24
		m_csCryptActor.GetCryptType() == AUCRYPT_TYPE_NONE)
	{
		// �ѹ��̶� �̽����� ©�������.
		if(m_ulInvalidEncPacketCount > 0)
			return FALSE;

		return TRUE;
	}

	// Check Level �˻�
	eAuCryptCheckLevel eCheckLevel = AuPacket::GetCryptManager().GetCryptCheckLevel();
	switch(eCheckLevel)
	{
		case AUCRYPT_CHECK_LEVEL_NONE:
			return TRUE;

		case AUCRYPT_CHECK_LEVEL_LOW:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_LOW ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_MIDDLE:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_MIDDLE ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_HIGH:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_HIGH ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_ULTRA:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_ULTRA ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_SUPER:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_SUPER ? TRUE : FALSE;

		case AUCRYPT_CHECK_LEVEL_EXTREME:
			return m_ulInvalidEncPacketCount < AUCRYPT_CHECK_LIMIT_EXTREME ? TRUE : FALSE;
	}

	return TRUE;
}

void AsServerSocket::WriteInvalidPacketLog(EnumInvalidPacketType eType, BYTE* pPacket, INT32 lPacketLength)
{
	LPCTSTR szMessage = NULL;
	switch(eType)
	{
		case INVALID_PACKET_TYPE_NOT_ENC_PACKET:				szMessage = _T("Not Encrypted Packet");					break;
		case INVALID_PACKET_TYPE_REPLAY_ATTACK:					szMessage = _T("Replay Attack");						break;
		case INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID:		szMessage = _T("After Decrypt, Packet Size Invalid");	break;
		case INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR:	szMessage = _T("After Decrypt, Guard Byte Error");		break;
		case INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR:	szMessage = _T("Non-Enc Mode. Guard Byte Error");		break;
		case INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED:			szMessage = _T("Check Limit Exceed");					break;
		default:												szMessage = _T("Unknown Error");						break;
	}

	// � ������ ���� �ʴ´�.
	// ��, �ϳ��� �����尡 ��� ������ �ٸ� ������� Write ���� ���Ѵ�.
	HANDLE hFile = CreateFile(AUCRYPT_MANAGER_INVALID_PACKET_LOG_FILENAME,
								GENERIC_READ | GENERIC_WRITE, 0, NULL,
								OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return;

	SetFilePointer(hFile, 0, 0, FILE_END);		// �� ��� ��� ����� �־���. -_-;

	CHAR pBuffer[512];
	sprintf(pBuffer, "Account : %s, %s", m_stPlayerData.szAccountName, szMessage);
	DWORD dwWritten = 0;
	WriteFile(hFile, pBuffer, (DWORD)strlen(pBuffer), &dwWritten, NULL);

	memset(pBuffer, 0, sizeof(pBuffer));
	if(eType == INVALID_PACKET_TYPE_NOT_ENC_PACKET || eType == INVALID_PACKET_TYPE_REPLAY_ATTACK)
	{
		sprintf(pBuffer, "\r\n Dump : ");
		for(size_t i = 0; i < sizeof(ENC_PACKET_HEADER); ++i)
		{
			if(pPacket && pPacket[i])
				sprintf(pBuffer, "0x%02X ", pPacket[i]);
			else
				sprintf(pBuffer, "0x   ");
		}
	}
	else if(eType == INVALID_PACKET_TYPE_AFTER_DEC_SIZE_INVALID ||
			eType == INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR ||
			eType == INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR)
	{
		sprintf(pBuffer, "\r\n Dump : ");
		for(size_t i = 0; i < sizeof(PACKET_HEADER); ++i)
		{
			if(pPacket && pPacket[i])
				sprintf(pBuffer, "0x%02X ", pPacket[i]);
			else
				sprintf(pBuffer, "0x   ");
		}

		if(eType == INVALID_PACKET_TYPE_AFTER_DEC_GUARD_BYTE_ERROR || eType == INVALID_PACKET_TYPE_NON_ENC_MODE_GUARD_BYTE_ERROR)
		{
			if(pPacket && pPacket[lPacketLength - 1])
			{
				sprintf(pBuffer, "\r\n        ");	// \r\n next 7 spaces
				sprintf(pBuffer, "Index[lPacketLength - 1] : 0x%02X", pPacket[lPacketLength - 1]); 
			}
		}
	}
	// eType == INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED �� ���� pPacket �� NULL �̱� ������ Dump ���� �ʴ´�.

	sprintf(pBuffer, "\r\n");
	WriteFile(hFile, pBuffer, (DWORD)strlen(pBuffer), &dwWritten, NULL);

	CloseHandle(hFile);
}