#pragma once

/**************************************************************************************************

�ۼ���: 2008-07-08
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: ��Ʈ�� ���� �κ��� ó���ϴ� ��ü 

**************************************************************************************************/

#include <Net/ServerNetwork.h>
#include "SessionMgr.h"

class CNetworkManager : public WBANetwork::Thread
{
public:
	CNetworkManager(void);
	virtual ~CNetworkManager(void);

	// ��Ʈ�� �۾��� �ϱ� ���� �ʱ�ȭ
	bool Initialize(WBANetwork::LPFN_ErrorHandler errhandler, WBANetwork::ServerSession* ClinetSession);

private:
	// WBA ��Ʈ�� ���̺귯�� ��Ʈ�� ���� ��ü
	WBANetwork::ServerNetwork	m_Network;	
	// WBA ��Ʈ�� ���̺귯�� ���ǰ��� ���� ��ü
	CSessionMgr					m_SessionMgr;

protected:
	// �θ� Ŭ�������� ����� �޼ҵ� ������ (������ ���� ���ν���)
	virtual void Run();

private:
	// �������� ���� ���θ� �˻��ϴ� �̺�Ʈ �ڵ�
	HANDLE m_hTerminateThread;
	//�ʱ�ȭ�� �Ǿ��ִ��� �˻��ϴ� �÷���
	bool m_bIsInit;

	//Ŭ���̾�Ʈ ���� ó��
	void terminate();
};
