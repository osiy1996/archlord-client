// AgsmServerStatus_IOCPServer.h
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 31.
//

#include "AgsmServerStatus_IOCPServer.h"

static UINT WINAPI AgsmServerStatus_MainLoop(PVOID pvParam)
{
	AsIOCPServer *pThis = (AsIOCPServer*)pvParam;

	DWORD			dwTransferred = 0;
	LPOVERLAPPED	pOverlapped = NULL;
	AsObject		*pObject = NULL;
	DWORD			dwTickCount = 0;

	while(TRUE)
	{
		BOOL bResult = pThis->GetStatus((PULONG_PTR)&pObject, &dwTransferred, &pOverlapped, INFINITE);

		if (bResult)
		{
			switch (pObject->GetType())
			{
			case ASOBJECT_LISTENER:
				{
					pThis->ProcessListener(pObject, (AsOverlapped*)pOverlapped);	
					printf("[AgsmServerStatus_MainLoop] Accept Socket ID : %d\n", ((AsOverlapped*)pOverlapped)->pSocket->GetIndex());

					break;
				}
			case ASOBJECT_SOCKET:	
				{
					AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
					if (pAsOverlapped->pSocket->IsRead(pOverlapped))
					{
						pThis->ProcessSocket(pObject, dwTransferred);
					}
					else
					{
						pAsOverlapped->pSocket->SendComplete();
					}
					break;
				}
			case ASOBJECT_TIMER:
				{
					pThis->ProcessTimer(pObject);					
					break;
				}
				
			case ASOBJECT_SYSTEM:
				{
					// AsIOCPServer�� System Command
					AsSystemCommand *pSystemCommand = (AsSystemCommand*)pObject;

					// ���� ������ �����Ѵ�.
					if (ASSYSTEM_COMMAND_TERMINATE == pSystemCommand->m_eCommand)
					{
						// �����ִ� �ٸ� �����带 �����Ű�� ���� Post ��Ų��.
						pThis->PostStatus((ULONG_PTR)pSystemCommand);
						return TRUE;
					}
				}
			default:
				printf("���ǵ��� ���� Object Type �Դϴ�.\n");
			}
		}
		else
		{
			// ���������� I/O �۾� �Ϸ�
			// JNY TODO : Exception ó���� �ʿ��� �κ�... Ȥ�� Socket ���� ���� ���Ѿ� �Ǵºκ�
			if (ASOBJECT_LISTENER == pObject->GetType())
			{
				// Error�� ���� Socket Type�� LISTENER�̸� ServerSocket�� ReuseSocket���� ���
				printf("ASOBJECT_LISTENER Reuse Socket\n");
				((AsOverlapped*)pOverlapped)->pSocket->SetState(ASSERVER_STATE_ALIVE);
				pThis->DestroyClient(((AsOverlapped*)pOverlapped)->pSocket);
			}
			else if (ASOBJECT_SOCKET == pObject->GetType())
			{
				AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
				if (pAsOverlapped)
				{
					AsServerSocket* pSocket = pAsOverlapped->pSocket;

					printf("[AgsmServerStatus_MainLoop] bResult FALSE�� ���� ����\n");
					pThis->DestroyClient(pSocket);
				}
			}
		}
	}

	return FALSE;
}

AgsmServerStatus_IOCPServer::AgsmServerStatus_IOCPServer()
{
}

AgsmServerStatus_IOCPServer::~AgsmServerStatus_IOCPServer()
{
}

BOOL AgsmServerStatus_IOCPServer::Initialize(INT32 lThreadCount, INT32 lPort, INT32 lMaxConnectionCount, INT32 lMaxSendBufferMB)
{
	// IOCP ����
	if (!Create(lThreadCount, AgsmServerStatus_MainLoop, (PVOID)this, "AgsmServerStatus_IOCPServer")) return FALSE;
	
	// ���� ���� ����
	if (!m_csListener.Initialize(lPort)) return FALSE;

	// ���� �Ŵ��� �ʱ�ȭ
	if (!m_csSocketManager.Initialize(lMaxConnectionCount, &m_csTimer)) return FALSE;

	// ������ ���ϵ��� Listener�� Accept�� �ɾ��ش�.
	for (INT32 i = 0; i < lMaxConnectionCount; i++)
	{
		m_csListener.AsyncAccept(m_csSocketManager[i]);
	}

	// ���� ������ IOCP�� ����
	if (!AttachHandle((HANDLE)m_csListener.GetHandle(), (ULONG_PTR)&m_csListener)) return FALSE;

// 	// lMaxSendBufferMB�� MByte �����̾.. byte ������ �ٲ۴�.
 	m_csSocketManager.SetMaxSendBuffer(lMaxSendBufferMB * 1000000);

	m_csTimer.Initialize(this, 100);

	return TRUE;
}

