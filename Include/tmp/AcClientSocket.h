#pragma once

#include <winsock2.h>
#include "AcQueue.h"
#include "ApModule.h"
#include "AuCircularBuffer.h"
#include "AuCrypt.h"

#pragma comment (lib, "ws2_32")

static LPSTR szClassName = "AcClientSocket";

const INT32 AC_ASYNCSELECT			= (WM_USER + 0x200);		// AsyncSelect socket�� ���� User define Window message
const INT32 MAX_PACKET_COUNT		= 10;
const INT32 MAX_CLIENT_BUFFER_SIZE	= APPACKET_MAX_PACKET_SIZE * MAX_PACKET_COUNT;

enum EnumCallback
{
	ACSOCKET_CALLBACK_CONNECT	 = 0,
	ACSOCKET_CALLBACK_DISCONNECT,
	ACSOCKET_CALLBACK_ERROR,
	ACSOCKET_CALLBACK_MAX,
};

struct CallbackFunc
{
	// pData		-> Socket Class Pointer
	// pCustData	-> OnError�� ��� Error Code �ٸ���쿣 ������� ����
	PVOID pClass;	// User�� ���� �Ѱ�����.. (User Class Pointer)
	ApModuleDefaultCallBack m_CallBackFunc[ACSOCKET_CALLBACK_MAX];

	CallbackFunc() : pClass(NULL)
	{
		::ZeroMemory(m_CallBackFunc, sizeof(m_CallBackFunc));
	}

	VOID CallFunction(EnumCallback eCallbackID, PVOID pData, PVOID pCustData)
	{
		// ��ϵ� �Լ� �����Ͱ� ���� ��쿡�� ȣ��
		if (m_CallBackFunc[eCallbackID])
			m_CallBackFunc[eCallbackID](pData, pClass, pCustData);
	}
};

enum ENUM_ACSOCKET_STATE
{
	ACSOCKET_STATE_NULL = 0,	//
	ACSOCKET_STATE_INITIALIZE,	// �ʱ�ȭ ����
	ACSOCKET_STATE_CONNECTING,	// ���� ������
	ACSOCKET_STATE_CONNECTED,	// ���� �Ϸ�
	ACSOCKET_STATE_DISCONNECTED,// ���� ����
	ACSOCKET_STATE_LISTENING,	// ������ ����
	ACSOCKET_STATE_ERROR		// ���� 
};

class AcSocketManager;
class AcClientSocket
{
public:
	AcClientSocket();
	virtual ~AcClientSocket();

	VOID Create();
	VOID Close();
	BOOL Connect( LPCTSTR pszIPAddress, INT32 lPort );

	INT32 Send( AcPacketData *pPacketData, INT32 lLength );
	INT32 Recv();

	// FD Event�� ���� ȣ��Ǵ��Լ�
	void OnRecv();
	void OnSend();
	void OnConnect();
	void OnAccept();
	void OnClose();
	void OnError(INT32 lErrorCode);

	VOID SetCallBack( PVOID pClass = NULL, ApModuleDefaultCallBack fpConnect = NULL,
					  ApModuleDefaultCallBack fpDisconnect = NULL, ApModuleDefaultCallBack fpError = NULL );
	
	VOID SetSocketManager( AcSocketManager *pcsSocketManager )	{	m_pcsSocketManager = pcsSocketManager;	}
	VOID SetMaxPacketBufferSize(INT32 lMaxPacketBufferSize)		{	m_AuCircularReceiveBuffer.Init(lMaxPacketBufferSize);	}
	BOOL SetDestroyNormal()										{	return (m_bIsDestroyNormal = TRUE);	}

	INT32	GetState()						{	return m_lSocketSatete;		}
	VOID	SetIndex(INT32 lIndex)			{	m_lIndex = lIndex;			}
	INT32	GetIndex()						{	return m_lIndex;			}

	AuCryptActor&	GetCryptActorRef()		{	return m_csCryptActor;		}
	AuCryptActor*	GetCryptActorPtr()		{	return &m_csCryptActor;		}

	UINT32	GetSendSeqID()					{	return m_ulSendSeqID;		}
	UINT32	GetRecvSeqID()					{	return m_ulRecvSeqID;		}
	void	SetSendSeqID( UINT32 ulNewID )	{	m_ulSendSeqID = ulNewID;	}
	void	SetRecvSeqID( UINT32 ulNewID )	{	m_ulRecvSeqID = ulNewID;	}

	static VOID NetworkInitialize(HINSTANCE hInstance = NULL);
	static VOID NetworkUnInitialize();
	static char* GetIPFromHost( LPCTSTR pstrHostName );

private:
	AcPacketData*	AllocPacket(INT32 lPacketSize);
	BOOL	CreateAsyncWindow();
	VOID	PushCompletePacket(INT32 lRecvLength);

private:
	CallbackFunc	m_stCallbackFunc;	// �ݹ��Լ� ���� �� ȣ��

	SOCKADDR_IN		m_stRemoteInfo;
	INT32			m_lSocketSatete;	// ���� ���� ����
	SOCKET			m_hSocket;			// ���� �ڵ�
	HWND			m_hWindow;			// Async Socket�� ���� Window Handle
	INT32			m_lIndex;			// AcSocketManager���� �ο����ִ� Index

	INT32			m_lBufferInOffset;							// m_BufferIn�� Index Offset
	BYTE			m_BufferIn[MAX_CLIENT_BUFFER_SIZE];			// �켱 �̰����� ��Ŷ�� �����Ͽ� �ϼ��� ��Ŷ��
	// Queue�� �ѱ��.

	INT32			m_lBufferOutOffset;							// m_BufferOut�� Index Offset
	BYTE			m_BufferOut[MAX_CLIENT_BUFFER_SIZE];		// Send�� �����͸� �����ϴ°�

	BOOL			m_bIsConnectedSocket;						// ������ �Ǿ����� ������ ����
	BOOL			m_bIsReceivedPacket;						// ������ �����κ��� ������ ��Ŷ�� �޾Ҵ��� ����
	BOOL			m_bIsDestroyNormal;							// ���� ����Ȱ��� ����

	AcSocketManager *	m_pcsSocketManager;

	AuCircularBuffer	m_AuCircularReceiveBuffer;

	AuCryptActor	m_csCryptActor;		// 2006.04.11. steeple ��ȣȭ ���� �߰�.
	UINT32			m_ulSendSeqID;
	UINT32			m_ulRecvSeqID;
};