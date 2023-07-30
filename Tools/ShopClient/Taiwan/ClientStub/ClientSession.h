#pragma once

/**************************************************************************************************

�ۼ���: 2008-07-10
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: Ŭ���̾�Ʈ���� ���ǰ����� �Ҽ� �ִ� �������̽� ��ü

**************************************************************************************************/

#include <Net/ServerSession.h>
#include "NetworkManager.h"

class CClientSession : public WBANetwork::ServerSession
{
public:
	CClientSession(void);
	virtual ~CClientSession(void);

	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);
	//void Connect(TCHAR* ipAddress, unsigned short portNo);
	//void CloseSocket();
	bool SendData(void* buffer, DWORD size);
	

protected:
	virtual	void OnAccept() {};
	virtual	void OnSend( int size ) {};
	//virtual	void OnReceive( PBYTE buffer, int size ) {};
	virtual	void OnClose( DWORD error ) {};	

	CNetworkManager		m_NetworkManager;
};
