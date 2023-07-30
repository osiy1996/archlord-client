#include "AcClientSocket.h"
#include "AcSocketManager.h"
#include <ApBase/MagDebug.h>
#include <ApMemory/ApMemory.h>
#include <AuPacket/AuPacket.h>

//static Function
char* AcClientSocket::GetIPFromHost(LPCTSTR pstrHostName)
{
	// Use inet_addr() to determine if we're dealing with a name or an address
	struct in_addr iaHost;
	iaHost.s_addr = inet_addr(pstrHostName);

	LPHOSTENT lpHostEntry;
	if (iaHost.s_addr == INADDR_NONE)
	{
		// Wasn't an IP address string, assume it is a name
		lpHostEntry = gethostbyname(pstrHostName);
	}
	else
	{
		// It was a valid IP address string
		lpHostEntry = gethostbyaddr((const char *)&iaHost, sizeof(struct in_addr), AF_INET);
	}

	if( !lpHostEntry )		return NULL;

	for ( int iNdx = 0; ; iNdx++)
	{
		struct in_addr* pinAddr = ((LPIN_ADDR)lpHostEntry->h_addr_list[iNdx]);
		if( !pinAddr )
			break;

		return inet_ntoa(*pinAddr);
	}

	return NULL;
}

// JNY TODO : �ٲ�ߵ�
HINSTANCE		m_hInstance = NULL;
BOOL			g_bIgnorePacket	= FALSE;

AcClientSocket::AcClientSocket()
{
	m_lBufferInOffset	= 0;
	m_lBufferOutOffset	= 0;
	m_lSocketSatete		= ACSOCKET_STATE_NULL;
	m_hSocket			= INVALID_SOCKET;
	m_hWindow			= NULL;
	m_lIndex			= 0;

	::ZeroMemory(&m_stRemoteInfo, sizeof(SOCKADDR_IN));
	::ZeroMemory(m_BufferIn, sizeof(BYTE) * MAX_CLIENT_BUFFER_SIZE);
	::ZeroMemory(m_BufferOut, sizeof(BYTE) * MAX_CLIENT_BUFFER_SIZE);

	m_bIsConnectedSocket	= FALSE;
	m_bIsReceivedPacket		= FALSE;
	m_bIsDestroyNormal		= FALSE;

	m_ulSendSeqID = m_ulRecvSeqID = 0;
}

AcClientSocket::~AcClientSocket()
{
	if( INVALID_SOCKET != m_hSocket ) 
		Close();

	if( m_hWindow )
	{
		::DestroyWindow( m_hWindow );
		m_hWindow = NULL;
	}
}

VOID AcClientSocket::NetworkInitialize(HINSTANCE hInstance)
{
	WSADATA stWSAData;
	WSAStartup( 0x0101, &stWSAData );
	m_hInstance = hInstance;
}

VOID AcClientSocket::NetworkUnInitialize()
{
	::WSACleanup();
}

static LRESULT WINAPI AcClientSocketProc(HWND hWnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	if( AC_ASYNCSELECT == iMsg )
	{
		AcClientSocket *pSocket	= (AcClientSocket*)::GetWindowLong( hWnd, GWL_USERDATA );
		ASSERT(pSocket);

		INT32	lEvent		= LOWORD(lParam);
		INT32	lErrorCode	= HIWORD(lParam);

		if ( lErrorCode ) 
		{
			CHAR strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, sizeof( CHAR ) * 256, "AcClientSocket AcClientSocketProc -> %d line : ErrorCode : %d", __LINE__, lErrorCode );
			::AuLogFile_s( ALEF_ERROR_FILENAME, strDebug );

			pSocket->OnError( lErrorCode );
			pSocket->Close();
		}
		else
		{
			switch(lEvent) 
			{
			case FD_READ:		pSocket->OnRecv();		break;
			case FD_WRITE:		pSocket->OnSend();		break;
			case FD_CONNECT:	pSocket->OnConnect();	break;
			case FD_ACCEPT:		pSocket->OnAccept();	break;
			case FD_CLOSE:		pSocket->OnClose();		break;
			default:			ASSERT(!"���ǵ��� ���� FD Event �߻�");
			}

			return TRUE;
		}
	}

	return ::DefWindowProc(hWnd, iMsg, wParam, lParam);	
}

VOID AcClientSocket::Create()
{
	if( m_hSocket == INVALID_SOCKET )
	{
		m_hSocket = ::socket(AF_INET, SOCK_STREAM, 0);
		if( INVALID_SOCKET == m_hSocket )
		{
			INT32 lError = ::WSAGetLastError();
			ASSERT( !"���� ���� ����" );
		}
	}

	if( CreateAsyncWindow() )
	{
		m_lSocketSatete = ACSOCKET_STATE_INITIALIZE;
	}
}

BOOL AcClientSocket::CreateAsyncWindow()
{
	if( m_hWindow )		return FALSE;

	// ������ Ŭ���� ����
	WNDCLASS stWindowClass;
	stWindowClass.cbClsExtra	= 0;
	stWindowClass.cbWndExtra	= 0;
	stWindowClass.style			= 0;
	stWindowClass.hbrBackground	= (HBRUSH) ::GetStockObject(WHITE_BRUSH);
	stWindowClass.hCursor		= NULL;
	stWindowClass.hIcon			= NULL;
	stWindowClass.hInstance		= m_hInstance;
	stWindowClass.lpfnWndProc	= AcClientSocketProc;
	stWindowClass.lpszClassName	= szClassName;
	stWindowClass.lpszMenuName	= NULL;
	::RegisterClass( &stWindowClass );

	m_hWindow = ::CreateWindow( szClassName, szClassName, 0, 
								CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
								NULL, NULL, m_hInstance, NULL );
	ASSERT( m_hWindow && "Window ���� ����" );

	::SetLastError(0);
	::SetWindowLong( m_hWindow, GWL_USERDATA, (LONG)this ); // ���� AcClientSocket�� �ּҸ� �������� ����ҿ� ����
	if( ::WSAAsyncSelect( m_hSocket, m_hWindow, AC_ASYNCSELECT, FD_READ | FD_WRITE | FD_CONNECT | FD_ACCEPT | FD_CLOSE ) )
	{
		INT32 lResult = ::WSAGetLastError();
		ASSERT(lResult);
	}

	return TRUE;
}

BOOL AcClientSocket::Connect(LPCTSTR pszIPAddress, INT32 lPort)
{
	if( ACSOCKET_STATE_INITIALIZE != m_lSocketSatete )		return FALSE;

	m_stRemoteInfo.sin_family		= AF_INET;
	m_stRemoteInfo.sin_addr.s_addr	= inet_addr(pszIPAddress);
	m_stRemoteInfo.sin_port			= htons(lPort);
	if( INADDR_NONE == m_stRemoteInfo.sin_addr.s_addr )
		m_stRemoteInfo.sin_addr.s_addr = inet_addr( AcClientSocket::GetIPFromHost( pszIPAddress ) );

	INT32 lResult = ::connect( m_hSocket, (struct sockaddr*)&m_stRemoteInfo, sizeof( SOCKADDR_IN ) );
	if( lResult )
	{
		INT32 lError = ::WSAGetLastError();
		if( WSAEWOULDBLOCK != lError ) 
		{
			CHAR strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, sizeof( CHAR ) * 256, "AcClientSocket Connect -> %d line : ErrorCode : %d", __LINE__, lError );
			::AuLogFile_s( ALEF_ERROR_FILENAME, strDebug );

			OnError(lError);
			return FALSE;
		}
	}

	m_bIsConnectedSocket	= TRUE;
	m_lSocketSatete			= ACSOCKET_STATE_CONNECTING;
	return TRUE;
}

VOID AcClientSocket::Close()
{
	if( INVALID_SOCKET != m_hSocket )
	{
		::closesocket( m_hSocket );
		m_hSocket = INVALID_SOCKET;
	}

	m_lSocketSatete			= ACSOCKET_STATE_NULL;	
	m_bIsConnectedSocket	= FALSE;

	if( m_hWindow )
	{
		::DestroyWindow( m_hWindow );
		m_hWindow		= NULL;
	}

	m_csCryptActor.Initialize();
	m_ulSendSeqID = m_ulRecvSeqID = 0;
}

INT32 AcClientSocket::Send(AcPacketData *pPacketData, INT32 lLength)
{
	if (ACSOCKET_STATE_CONNECTED != m_lSocketSatete)
	{
		//ASSERT(!"������ ����� ���°� �ƴմϴ�.");
		return 0;
	}

	// ��ȣȭ �� ��Ŷ�̶�� SeqID �� �ִ´�.
	UINT32 ulSendSeqID = ++m_ulSendSeqID;
	if(ulSendSeqID == 0)
		ulSendSeqID = m_ulSendSeqID = 1;

	if(m_csCryptActor.GetCryptType() != AUCRYPT_TYPE_NONE)
	{
		// ��ȣȭ Go!!!
		INT16 nOutputSize = 0;
		PVOID pOutput = AuPacket::EncryptPacket((BYTE*)pPacketData, lLength, &nOutputSize,
			m_csCryptActor, m_csCryptActor.GetCryptType());

		if(!pOutput || nOutputSize < 1)
			return 0;

		pPacketData = (AcPacketData*)pOutput;
		lLength = nOutputSize;


		((PENC_PACKET_HEADER)pPacketData)->ulSeqID = ulSendSeqID;
	}

	memcpy(m_BufferOut + m_lBufferOutOffset, pPacketData, lLength);
	m_lBufferOutOffset += lLength;

	INT32 lSendLength = ::send(m_hSocket, (const char*)pPacketData, m_lBufferOutOffset, 0);
	if ((m_lBufferOutOffset > 0) && (m_lBufferOutOffset < lSendLength))
	{
		// Packet�� �Ϻκи� ����
		memmove(m_BufferOut, m_BufferOut + lSendLength, m_lBufferOutOffset - lSendLength);
		m_lBufferOutOffset -= lSendLength;
	}
	else
	{
		// Packet�� ���ۿϷ�
		m_lBufferOutOffset = 0;
	}

	return lSendLength;
}

INT32 AcClientSocket::Recv()
{
	INT32 lRecvLength = ::recv(m_hSocket, (char*)(m_BufferIn + m_lBufferInOffset), MAX_CLIENT_BUFFER_SIZE - m_lBufferInOffset, 0);
	if( SOCKET_ERROR == lRecvLength )
	{
		INT32 lError = ::GetLastError();
		if (WSAEWOULDBLOCK != lError) 
		{
			// ���� ó��
			CHAR strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, sizeof( CHAR ) * 256, "AcClientSocket Recv -> %d line : ErrorCode : %d", __LINE__, lError );
			::AuLogFile_s( ALEF_ERROR_FILENAME, strDebug );

			OnError(lError);
			//ASSERT(!"Recv ����");
		}
	}
	else if( !g_bIgnorePacket )
	{
		m_bIsReceivedPacket	= TRUE;
		PushCompletePacket( lRecvLength );
	}

	return lRecvLength;
}

VOID AcClientSocket::PushCompletePacket(INT32 lRecvLength)
{
	INT32 lProcessIndex = 0;
	m_lBufferInOffset += lRecvLength;

	while (TRUE)
	{
		//		// ���� �ִ� ���� ������ ��Ŷ�� ����� �� ������ ���� ������ �޸𸮸� ������ �̵���Ų��.
		//		if (m_lBufferInOffset < APPACKET_MIN_PACKET_SIZE)
		//		{
		//			memmove(m_BufferIn, m_BufferIn + lProcessIndex, m_lBufferInOffset);
		//			m_lBufferInOffset = 0;
		//			return;
		//		}

		if (APPACKET_MIN_PACKET_SIZE > m_lBufferInOffset) break;
		UINT16 lPacketLength = ((PPACKET_HEADER) (m_BufferIn + lProcessIndex))->unPacketLength;

		//ASSERT(lPacketLength < 2048);

		if (lPacketLength >= MAX_CLIENT_BUFFER_SIZE ||
			lPacketLength < APPACKET_MIN_PACKET_SIZE)
		{
			// �߸��� ����Ÿ��. ���۸� ���������.
			memset(m_BufferIn, 0, sizeof(BYTE) * MAX_CLIENT_BUFFER_SIZE);
			m_lBufferInOffset	= 0;
			return;
		}

		// ��Ŷ�� �ϼ��ϱ⿡ �����Ͱ� ���ڶ�
		if (lPacketLength > m_lBufferInOffset)
		{
			break;
		}

		//		if (strcmp((char*)m_BufferIn + lProcessIndex + sizeof(INT16), "DISCONNECT_NORMAL") == 0)
		//		{
		//			m_bIsDestroyNormal	= TRUE;
		//		}
		//		else
		{
			// �ϼ��� ��Ŷ�� Queue�� ����
			//AcPacketData *pQueueData = new AcPacketData;
			//			AcPacketData *pQueueData = m_pcsSocketManager->AllocPacket();
			AcPacketData *pQueueData = AllocPacket(lPacketLength);
			memcpy(pQueueData->m_Buffer, m_BufferIn + lProcessIndex, lPacketLength);
			pQueueData->m_lLength = lPacketLength;
			pQueueData->m_lNID = m_lIndex;

			if(IS_ENC_PACKET(pQueueData->m_Buffer, (INT16)lPacketLength))
			{
				// SeqID üũ. Ŭ���̾�Ʈ�� �ϴ� ������ ok. ���常 �ϰ� �ٸ� �˻�� ���� �ʴ´�.
				SetRecvSeqID(((PENC_PACKET_HEADER)pQueueData->m_Buffer)->ulSeqID);

				// ��ȣȭ �Ѵ�.
				INT16 nOutputSize = 0;
				PVOID pOutput = AuPacket::DecryptPacket(pQueueData->m_Buffer, lPacketLength, &nOutputSize,
					m_csCryptActor, AUCRYPT_TYPE_PUBLIC);	// ��ȣȭ�� Public ���� �Ѵ�.

				// ������ �ٽ� ����
				pQueueData->m_Buffer = (BYTE*)pOutput;
				pQueueData->m_lLength = nOutputSize;
			}

			// check guard byte
			if (((PPACKET_HEADER)pQueueData->m_Buffer)->bGuardByte != APPACKET_FRONT_GUARD_BYTE ||
				*((BYTE*)pQueueData->m_Buffer + pQueueData->m_lLength - 1) != APPACKET_REAR_GUARD_BYTE)
			{
				ASSERT(!"��ȣȭ ��, �������Ʈ ��������");
				memset(m_BufferIn, 0, sizeof(BYTE) * MAX_CLIENT_BUFFER_SIZE);
				m_lBufferInOffset	= 0;
				return;
			}

			g_AcReceiveQueue.Push(pQueueData);
		}

		lProcessIndex += lPacketLength;
		m_lBufferInOffset -= lPacketLength;
	}

	// ��Ŷ�� �ϼ��ǰ� ���� ������ �̿ϼ� ��Ŷ�� Buffer�� �� ������ �̵�
	if (lProcessIndex > 0)
		memmove(m_BufferIn, m_BufferIn + lProcessIndex, m_lBufferInOffset);
}

// FD Event�� ���� ȣ��Ǵ��Լ�
void AcClientSocket::OnRecv()
{
	Recv();
}

void AcClientSocket::OnSend()
{
	if (!m_lBufferOutOffset) return;	// �����ؾߵ� �������� ���� �����Ͱ� ������ ����

	INT32 lSendLength = ::send(m_hSocket, (const char *)m_BufferOut, m_lBufferOutOffset, 0);

	if (SOCKET_ERROR == lSendLength)
	{
		// Socket ���� ó��
	}

	if ((lSendLength > 0) && (lSendLength < m_lBufferOutOffset))
	{
		// Packet�� �Ϻκи� ����
		memmove(m_BufferOut, m_BufferOut + lSendLength, m_lBufferOutOffset - lSendLength);
		m_lBufferOutOffset -= lSendLength;
	}
	else
	{
		// Packet�� ���ۿϷ�
		m_lBufferOutOffset = 0;
	}
}

void AcClientSocket::OnConnect()
{
	m_lSocketSatete = ACSOCKET_STATE_CONNECTED;
	m_stCallbackFunc.CallFunction(ACSOCKET_CALLBACK_CONNECT, (PVOID)this, NULL);
}

void AcClientSocket::OnAccept()
{
}

void AcClientSocket::OnClose()
{
	m_lSocketSatete = ACSOCKET_STATE_DISCONNECTED;
	m_stCallbackFunc.CallFunction( ACSOCKET_CALLBACK_DISCONNECT, (PVOID)this, (PVOID) &m_bIsDestroyNormal );
	m_bIsDestroyNormal	= FALSE;
	Close();
}

void AcClientSocket::OnError(INT32 lErrorCode)
{
	// JNY TODO : ErrorCode�� �Ѱ��� ����� ����ؾ� ��
	m_lSocketSatete = ACSOCKET_STATE_DISCONNECTED;

	if( !m_bIsDestroyNormal )
		m_stCallbackFunc.CallFunction(ACSOCKET_CALLBACK_ERROR, (PVOID)this, (PVOID)lErrorCode);

	if( m_bIsConnectedSocket && m_bIsReceivedPacket )
		m_stCallbackFunc.CallFunction(ACSOCKET_CALLBACK_DISCONNECT, (PVOID)this, (PVOID) &m_bIsDestroyNormal);

	m_bIsDestroyNormal	= FALSE;
}

AcPacketData * AcClientSocket::AllocPacket(INT32 lPacketSize)
{
	BYTE*			pPacketBuffer	= (BYTE *) m_AuCircularReceiveBuffer.Alloc(lPacketSize + 1);
	if( !pPacketBuffer )	return NULL;
		
	AcPacketData*	pcsPacketData	= (AcPacketData *) m_AuCircularReceiveBuffer.Alloc(sizeof(AcPacketData));
	if( !pcsPacketData )	return NULL;

	pcsPacketData->m_Buffer = pPacketBuffer;
	return pcsPacketData;
}

VOID AcClientSocket::SetCallBack(PVOID pClass, ApModuleDefaultCallBack fpConnect, ApModuleDefaultCallBack fpDisconnect, ApModuleDefaultCallBack fpError)
{
	m_stCallbackFunc.pClass		= pClass;
	m_stCallbackFunc.m_CallBackFunc[ACSOCKET_CALLBACK_CONNECT]		= fpConnect;
	m_stCallbackFunc.m_CallBackFunc[ACSOCKET_CALLBACK_DISCONNECT]	= fpDisconnect;
	m_stCallbackFunc.m_CallBackFunc[ACSOCKET_CALLBACK_ERROR]		= fpError;
}

