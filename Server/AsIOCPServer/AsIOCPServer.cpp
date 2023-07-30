// AsIOCPServer.cpp: implementation of the AsIOCPServer class.
//
//////////////////////////////////////////////////////////////////////

#include "AsIOCPServer.h"

//#include "ApFilterFunction.h"
#include "ApLockManager.h"
#include "ApMemory.h"
#include <process.h>

#ifdef _AREA_CHINA_
#include "AuIPBlockingManager.h"
#endif

#ifdef	__PROFILE__
#include "API_AuFrameProfile.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

static UINT WINAPI AsMainLoop(PVOID pvParam)
{
	AsIOCPServer *pThis = (AsIOCPServer*)pvParam;

	DWORD			dwTransferred = 0;
	LPOVERLAPPED	pOverlapped = NULL;
	AsObject		*pObject = NULL;
	#ifdef _AREA_CHINA_
	BOOL			bBan;
	#endif
	DWORD			dwTickCount = 0;

#ifdef	__PROFILE__
	FrameProfile_SetEnable( TRUE );
	FrameProfile_SetThreadName( "AsMainLoop" );
#endif	//__PROFILE__

	while(TRUE)
	{
		BOOL bResult = pThis->GetStatus((PULONG_PTR)&pObject, &dwTransferred, &pOverlapped, INFINITE);

		if (bResult)
		{
#ifdef __PROFILE__
			__try
			{
#endif //__PROFILE__
				switch (pObject->GetType())
				{
				case ASOBJECT_LISTENER:
					{
						pThis->ProcessListener(pObject, (AsOverlapped*)pOverlapped);	
						printf("Accept Socket ID : %d\n", ((AsOverlapped*)pOverlapped)->pSocket->GetIndex());
#ifdef _AREA_CHINA_
						bBan = FALSE;

						if( pThis->m_bExecuteBlockIP )
						{
							bBan = pThis->CheckBlockIP(((AsOverlapped*)pOverlapped)->pSocket);
						}

						if( pThis->m_bOpenPCRoomIP )
						{
							//PC�� IP�� IP��ŷ�� ���� Ban�̾ ����� �ȵǰ����շ�~
							if (pThis->CheckPCRoomIP(((AsOverlapped*)pOverlapped)->pSocket))
							{
								((AsOverlapped*)pOverlapped)->pSocket->SetPCRoom();
								bBan = FALSE;
							}
						}

						//��ŷ������ ©���ų� Ȥ�� PC�� �÷��׿� ���� �������� ���Ѱ��~
						//�ҽ��ϰԵ� Ban~ �� KIN~
						if( bBan )
						{
							pThis->DestroyClient( ((AsOverlapped*)pOverlapped)->pSocket );
						}
#endif

						if (pThis->m_fpConnect)
						{
							pThis->m_fpConnect(NULL, pThis->m_DispatchArchlord.pvModule, ((AsOverlapped*)pOverlapped)->pSocket);
						}

						((AsOverlapped*)pOverlapped)->pSocket->m_ulConnectedTimeMSec	= ::timeGetTime();

						break;
					}
				case ASOBJECT_SOCKET:	
					{
						// 2007.04.23. steeple
						// 0 Byte �޾��� �� ó��.
						if(dwTransferred == 0)
						{
							//ASSERT(dwTransferred > 0);
							pThis->DisconnectSocket(pObject, (AsOverlapped*)pOverlapped);
							break;
						}

						AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
						if (pAsOverlapped->pSocket->IsRead(pOverlapped))
						{
#ifdef	__PROFILE__
							dwTickCount = timeGetTime();
							FrameProfile_Init();
							pThis->ProcessSocket(pObject, dwTransferred);
							if (dwTickCount + pThis->GetAcceptableDelay() < timeGetTime())
							{
								FrameProfile_WriteFile();
							}
#else
							pThis->ProcessSocket(pObject, dwTransferred);
#endif
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
#ifdef __PROFILE__
			}
			__except(FrameProfile_ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
			{
				// FrameCallStackFilter���� FrameCallstack log �����.
				// log�� ������ EXCEPTION_CONTINUE_SEARCH ���� ��Ű�� �Ǹ� Application�� ����ȴ�.
			}
#endif //__PROFILE__
		}
		else
		{
			// 2007.04.23. steeple
			pThis->DisconnectSocket(pObject, (AsOverlapped*)pOverlapped);
		}
		
		if (ApLockManager::Instance()->IsExistData())
			ApLockManager::Instance()->AllUnlock();
	}

	return FALSE;
}

BOOL AsSocketClose(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData)
{
	AsServerSocket *pSocket = (AsServerSocket*)pvData;
	AsIOCPServer *pThis = (AsIOCPServer*)pClass;
	pThis->ReUseSocket(pSocket);
	return TRUE;
}

AsIOCPServer::AsIOCPServer()
{
	m_lNIDClients			= 0;
	m_lNIDServers			= 0;

	m_pClass				= NULL;
	m_fpLoginDisconnect		= NULL;
	m_fpConnect				= NULL;

	ZeroMemory(m_szLocalAddr, sizeof(CHAR) * MAX_DEVICES * 23);
	m_nNumAddr	= 0;
#ifdef _AREA_CHINA_
	m_bExecuteBlockIP = FALSE;
	m_bOpenPCRoomIP = FALSE;
#endif

#ifdef __PROFILE__
	m_dwAcceptableDelay = 500;
#endif

	m_eDefaultSocketType = SOCKET_TYPE_ARCHLORD;

	NetworkInitialize();
}

AsIOCPServer::~AsIOCPServer()
{
	NetworkUnInitialize();
}

VOID AsIOCPServer::NetworkInitialize()
{
	WSADATA stWSAData;
	INT32 lResult = ::WSAStartup(0x0202, &stWSAData);

	ASSERT(!lResult);	// 0�� �ƴϸ� ������ �ִ°���	
}

VOID AsIOCPServer::NetworkUnInitialize()
{
	INT32 lResult = ::WSACleanup();
}

BOOL AsIOCPServer::DestroyClient(INT32 lNID, INT32 lReason, CHAR* Buffer)
{
	return DestroyClient(m_csSocketManager[lNID], lReason, Buffer);
}

BOOL AsIOCPServer::DestroyClient(AsServerSocket *pSocket, INT32 lReason, CHAR* Buffer)
{
	if (NULL == pSocket)
	{
		ASSERT(!"DestroyClient���� pSocket�� NULL�� ����. �������� �����ٶ��ϴ�.");

		return FALSE;
	}

	pSocket->GetPlayerData()->lDisconnectReason	= (INT32) lReason;

	BOOL	bIsAddTimerEvent	= FALSE;

	// �ʱ�ȭ �Ѵ�. 2006.04.12. steeple
	pSocket->InitializeAll();

	{
		AuAutoLock Lock(m_Mutex);
		if (Lock.Result())
		{
			//pSocket->m_ulConnectedTimeMSec	= 0;
			
			if (m_csSocketManager.IsValidPointer(pSocket))
			{
				// AsIOCPServer�� ���� Ǯ���̹Ƿ� AsListener�� ����Ų��.
				if (pSocket->IsActive())
				{
					if (ASSERVER_STATE_RESERVED_CLOSE == pSocket->GetState()) return FALSE;

					if ((lReason == DISCONNNECT_REASON_CLIENT_NORMAL) && (Buffer))
					{
						pSocket->AsyncSend((CHAR*)Buffer, *(INT16*)Buffer, PACKET_PRIORITY_1);
					}

		//			printf("DestroyClient reason : %d\n", lReason);
					pSocket->SetState(ASSERVER_STATE_RESERVED_CLOSE);

					bIsAddTimerEvent	= TRUE;
				}
			}
			else
			{
				// AsDP8Client�̹Ƿ� �׳� ���Ḹ �Ѵ�.
				if( pSocket != NULL )
				{
					switch (pSocket->GetSocketType())
					{
					case SOCKET_TYPE_ARCHLORD:	
						{
							if (m_DispatchArchlord.fpDisconnect && pSocket->m_bCallback) 
								m_DispatchArchlord.fpDisconnect(NULL, m_DispatchArchlord.pvModule, pSocket);
						}
						break;

					case SOCKET_TYPE_LKSERVER:
						{
							if (m_DispatchLKServer.fpDisconnect) 
								m_DispatchLKServer.fpDisconnect(NULL, m_DispatchLKServer.pvModule, pSocket);
						}
						break;

					case SOCKET_TYPE_BILLING:
						{
							if (m_DispatchBilling.fpDisconnect) 
								m_DispatchBilling.fpDisconnect(NULL, m_DispatchBilling.pvModule, pSocket);
						}
						break;

					case SOCKET_TYPE_PATCH:
						{
							if (m_DispatchPatch.fpDisconnect) 
								m_DispatchPatch.fpDisconnect(NULL, m_DispatchPatch.pvModule, pSocket);
						}
						break;
					};

					pSocket->Remove();
					pSocket->Create();
					// DefaultSocketType �߰�(Parn 20060504)
					pSocket->SetSocketType(m_eDefaultSocketType);
				}
			}
		}
	}

	if (bIsAddTimerEvent)
	{
		pSocket->m_bIsProcessReuse	= TRUE;
		m_csTimer.AddTimer(3000, pSocket->GetPlayerData()->lID, (PVOID)this, AsSocketClose, (PVOID)pSocket);
	}

	return TRUE;
}

BOOL AsIOCPServer::DestroyAllClient()
{
	AuAutoLock Lock(m_Mutex);
	if (!Lock.Result()) return FALSE;

	printf("DestroyAllClient !\n");

	m_csSocketManager.DisconnectAll();

	return TRUE;
}

BOOL AsIOCPServer::ReUseSocket(AsServerSocket *pSocket)
{
	{
		AuAutoLock Lock(m_Mutex);
		if (Lock.Result())
		{
		//	printf("\nReuse Socket ID : %d", pSocket->GetIndex());
			pSocket->SetState(ASSERVER_STATE_DEAD);
		}
	}

	switch (pSocket->GetSocketType())
	{
	case SOCKET_TYPE_ARCHLORD:
		{
			if (m_DispatchArchlord.fpDisconnect) 
				m_DispatchArchlord.fpDisconnect(NULL, m_DispatchArchlord.pvModule, pSocket);

			if (m_fpLoginDisconnect) 
				m_fpLoginDisconnect(NULL, m_pClass, IntToPtr(pSocket->GetIndex()));
		}
		break;

	case SOCKET_TYPE_LKSERVER:
		{
			if (m_DispatchLKServer.fpDisconnect)
				m_DispatchLKServer.fpDisconnect(NULL, m_DispatchLKServer.pvModule, pSocket);
		}
		break;

	case SOCKET_TYPE_BILLING:
		{
		}
		break;

	case SOCKET_TYPE_PATCH:
		{
		}
		break;
	};

	if (ApLockManager::Instance()->IsExistData())
		ApLockManager::Instance()->AllUnlock();
			
	BOOL bResult = FALSE;
	{
		AuAutoLock Lock(m_Mutex);
		if (Lock.Result())
		{
			pSocket->Remove();
			pSocket->m_bIsProcessReuse	= FALSE;
			pSocket->Create();
			// DefaultSocketType �߰�(Parn 20060504)
			pSocket->SetSocketType(m_eDefaultSocketType);

			bResult = m_csListener.AsyncAccept(pSocket);
		}
	}
//	printf("\n Active Socket Count : %d", m_csListener.GetActiveSocketCount());
	return bResult;
}

BOOL AsIOCPServer::Initialize(INT32 lThreadCount, INT32 lPort, INT32 lMaxConnectionCount, INT32 lMaxSendBufferMB)
{
	// IOCP ����
	if (!Create(lThreadCount, AsMainLoop, (PVOID)this, "PacketDispatch")) return FALSE;
	
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

	// lMaxSendBufferMB�� MByte �����̾.. byte ������ �ٲ۴�.
	m_csSocketManager.SetMaxSendBuffer(lMaxSendBufferMB * 1000000);

	m_csTimer.Initialize(this, 100);

	// 2007.09.09. steeple
	// ������� ���� ��� ���Ҵ�.
	//StartConnectionChecker();

	return TRUE;
}

BOOL AsIOCPServer::GetLocalIPAddr(CHAR **pszBuffer, INT16 *pnCount)
{
	if (!pszBuffer || !pnCount)
		return FALSE;

	*pnCount = m_nNumAddr;
	*pszBuffer = (CHAR *)m_szLocalAddr;

	return TRUE;
}

VOID AsIOCPServer::InitLocalIPAddr(INT32 lPort)
{
    CHAR strLocalHostName[MAX_PATH];
    gethostname( strLocalHostName, MAX_PATH );
    HOSTENT* pHostEnt = gethostbyname( strLocalHostName );
    if( pHostEnt )
    {
		for (int i = 0; i < MAX_DEVICES; i++)
		{
			_tcscpy( m_szLocalAddr[i], TEXT("") );

			in_addr* pInAddr = (in_addr*) pHostEnt->h_addr_list[i];
			if (!pInAddr)
				break;

			char* strLocalIP = inet_ntoa( *pInAddr );
			if( strLocalIP )
				//DXUtil_ConvertAnsiStringToGeneric( m_szLocalAddr[i], strLocalIP, strlen(strLocalIP) + 1 );
				strncpy(m_szLocalAddr[i], strLocalIP, strlen(strLocalIP) + 1);

			sprintf(m_szLocalAddr[i], "%s:%d", m_szLocalAddr[i], lPort);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AsIOCPServer::InitLocalIPAddr() [%d] %s\n", i, m_szLocalAddr[i]);
			AuLogFile_s("LOG\\IP.log", strCharBuff);

			m_nNumAddr++;
		}
    }
}


BOOL AsIOCPServer::LoadIPBlockRes( char *pstrFileName, char *pstrPCRoomIPFileName )
{
	BOOL			bResult;

	bResult = TRUE;

	if( pstrFileName != NULL )
	{
		if(	m_cIPBlockingManager.LoadBlockIPFromFile( pstrFileName ) )
		{
			m_bExecuteBlockIP = TRUE;
		}
		else
		{
			printf( "LoadIPBlockRes, %s���� ������ �����߻�!\n", pstrFileName );
		}
	}

	if( pstrPCRoomIPFileName != NULL )
	{
		if(	m_cIPBlockingManager.LoadPCRoomIPFromFile( pstrPCRoomIPFileName ) )
		{
			m_bOpenPCRoomIP = TRUE;
		}
		else
		{
			printf( "LoadIPBlockRes, %s���� ������ �����߻�!\n", pstrPCRoomIPFileName );
		}
	}

	return bResult;
}

BOOL AsIOCPServer::CheckBlockIP(AsServerSocket *pSocket)
{
	SOCKADDR_IN			*pcAddr;
	BOOL				bResult;

	pcAddr = pSocket->GetRemoteInfo();
	bResult = TRUE;

	if( pcAddr != NULL )
	{
		char			strIP[80];

		sprintf( strIP, "%d.%d.%d.%d", (unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b1,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b2,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b3,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b4
										);

		//������ IP������ �ƴϸ� �ڸ���!!
		if( m_cIPBlockingManager.SearchIPFromBlock( strIP ) == FALSE )
		{
			bResult = TRUE;
		}
		else
		{
			bResult = FALSE;
		}
	}

	return bResult;
}

BOOL AsIOCPServer::CheckPCRoomIP(AsServerSocket *pSocket)
{
	SOCKADDR_IN			*pcAddr;

	pcAddr = pSocket->GetRemoteInfo();

	if( pcAddr != NULL )
	{
		char			strIP[80];

		sprintf( strIP, "%d.%d.%d.%d", (unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b1,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b2,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b3,
										(unsigned long)pcAddr->sin_addr.S_un.S_un_b.s_b4
										);

		//������ IP������ �ƴϸ� �ڸ���!!
		return	m_cIPBlockingManager.SearchIPFromPCRoom( strIP );
	}

	return FALSE;
}


BOOL AsIOCPServer::SetIDToPlayerContext(INT32 lID, INT32 lNID)
{
	if (INVALID_INDEX == lNID)
		return FALSE;

	PlayerData	*pPlayerData;

	pPlayerData = GetPlayerDataForID(lNID);
	if (!pPlayerData)
	{
//		WriteLog(AS_LOG_DEBUG, "AsDP9 : SetIDToPlayerContext() ���� GetPlayerDataForID() ����. NULL ����");
		return FALSE;
	}

	pPlayerData->lID = lID;

	return TRUE;
}

PlayerData* AsIOCPServer::GetPlayerDataForID(INT32 lNID)
{
	if (INVALID_INDEX == lNID) return NULL;

	return m_csSocketManager[lNID]->GetPlayerData();
}

BOOL AsIOCPServer::SetAccountNameToPlayerContext(CHAR *szAccountName, INT32 lNID)
{
	if (!szAccountName || lNID == INVALID_INDEX)
		return FALSE;

	PlayerData	*pPlayerData;

	pPlayerData = GetPlayerDataForID(lNID);
	if (!pPlayerData)
	{
//		WriteLog(AS_LOG_DEBUG, "AsDP9 : SetIDToPlayerContext() ���� GetPlayerDataForID() ����. NULL ����");
		return FALSE;
	}

	strncpy(pPlayerData->szAccountName, szAccountName, 32);

	return TRUE;
}

BOOL AsIOCPServer::SetServerFlagToPlayerContext(INT32 lNID)
{
	if (lNID == INVALID_INDEX)
		return FALSE;

	PlayerData	*pPlayerData	= GetPlayerDataForID(lNID);
	if (!pPlayerData)
	{
//		WriteLog(AS_LOG_DEBUG, "AsDP9 : SetServerFlagToPlayerContext() ���� GetPlayerDataForID() ����. NULL ����");
		return FALSE;
	}

	pPlayerData->bServer	= TRUE;

	return TRUE;
}

BOOL AsIOCPServer::SetCheckValidation(INT32 lNID)
{
	if (lNID == INVALID_INDEX)
		return FALSE;

	PlayerData	*pPlayerData	= GetPlayerDataForID(lNID);
	if (!pPlayerData)
		return FALSE;

	pPlayerData->bCheckValidation	= TRUE;

	return TRUE;
}

BOOL AsIOCPServer::ResetCheckValidation(INT32 lNID)
{
	if (lNID == INVALID_INDEX)
		return FALSE;

	PlayerData	*pPlayerData	= GetPlayerDataForID(lNID);
	if (!pPlayerData)
		return FALSE;

	pPlayerData->bCheckValidation	= FALSE;

	return TRUE;
}

BOOL AsIOCPServer::ProcessListener(AsObject *pObject, AsOverlapped* pOverlapped)
{
	if (ASSERVER_STATE_ACCEPT == pOverlapped->pSocket->GetState())
	{
#ifdef _DEBUG
		CHAR strAddress[MAX_IP_ADDRESS_STRING+1] = { 0, };
		pOverlapped->pSocket->GetRemoteIPAddress(strAddress,sizeof(strAddress));
		printf("\n %s Connected(ACCEPT)...", strAddress);
#endif
		m_csListener.SetSocketOption(pOverlapped->pSocket);
		AttachHandle((HANDLE)pOverlapped->pSocket->GetHandle(), (ULONG_PTR)pOverlapped->pSocket);
		pOverlapped->pSocket->AsyncRecv();
	}
	else
	{
		printf("\n ProcessListener Socket Status : %d", pOverlapped->pSocket->GetState());
	}

	return TRUE;
}

BOOL AsIOCPServer::ProcessSocket(AsObject *pObject, DWORD dwTransferred)
{
	// GetQueuedCompletionPort�� �������� I/O �۾� �Ϸ�
	AsServerSocket	*pSocket = (AsServerSocket*)pObject;
	CHAR			*pPacket = NULL;

	// ���� ����� 0�̸� ���� ���Ḧ ���Ѵ�.
	if (0 == dwTransferred) 
	{
		printf("Disconnected [Socket Handle %d][Socket Index %d] Remote Peer\n", pSocket->GetHandle(), pSocket->GetIndex());
		DestroyClient(pSocket);
		return TRUE;
	}

	//Receive�� Overlapped�� ���� ���ŵ� ��Ŷ�� �Ľ��Ѵ�.
	pSocket->SetReadBytes(dwTransferred);

	BOOL	bIsInvalidPacket	= TRUE;

	while(TRUE)
	{
		// ��ũ�ε� �̿��� ���� (��, �̵����) ���� ����κ� ó��
		// (��Ŷ ������ �ٸ���� ���⼭ ���� ó�����ش�)

		pPacket = pSocket->GetCompletePacket(&bIsInvalidPacket);
		if (NULL == pPacket) 
			break;

		switch (pSocket->GetSocketType())
		{
		case SOCKET_TYPE_ARCHLORD:
			if (m_DispatchArchlord.fpDispatch)
				m_DispatchArchlord.fpDispatch(pPacket, m_DispatchArchlord.pvModule, pSocket);
			break;

		case SOCKET_TYPE_LKSERVER:
			if (m_DispatchLKServer.fpDispatch)
				m_DispatchLKServer.fpDispatch(pPacket, m_DispatchLKServer.pvModule, pSocket);
			break;

		case SOCKET_TYPE_BILLING:
			if (m_DispatchBilling.fpDispatch)
				m_DispatchBilling.fpDispatch(pPacket, m_DispatchBilling.pvModule, pSocket);
			break;

		case SOCKET_TYPE_PATCH:
			if (m_DispatchPatch.fpDispatch)
				m_DispatchPatch.fpDispatch(pPacket, m_DispatchPatch.pvModule, pSocket);
			break;

		default:
			ASSERT(!"Socket Type Error");
		};

		if (ApLockManager::Instance()->IsExistData())
			ApLockManager::Instance()->AllUnlock();
	}

	if (!bIsInvalidPacket)
	{
		//printf("\n���� ����(�߸��� ��Ŷ (����� �̻��ϰų� �������Ʈ�� �̻��ϳ�)�� ���Դ�.)...");

		if(pSocket->CheckInvalidPacketCount() == FALSE)
		{
			//ASSERT(!"��ȣȭ üũ ����Ʈ�� �����");
			pSocket->WriteInvalidPacketLog(INVALID_PACKET_TYPE_CHECK_LIMIT_EXCEED, NULL, 0);

			DestroyClient(pSocket);
			return FALSE;
		}

		// �� ���¿��� ��Ŷ ����̴�.
		return TRUE;
	}

	// �Ľ� �Ϸ��� Recv �ɾ��ش�.
	if (ASSERVER_STATE_DEAD != pSocket->GetState())
		pSocket->AsyncRecv();

	return TRUE;
}

BOOL AsIOCPServer::ProcessTimer(AsObject *pObject)
{
	AsTimerEventData *pTimerEventData = (AsTimerEventData*)pObject;

	if (!pTimerEventData->GetCancel())
	{
		pTimerEventData->m_fpTimerCallBack(pTimerEventData->m_lCID, pTimerEventData->m_pClass, 
											pTimerEventData->m_uDelayTime, pTimerEventData->m_pvData);
	}

	delete pTimerEventData;
	return TRUE;
}

// 2007.04.23. steeple
// 0 byte �޾��� �� ó�� ������ �Լ��� �и��Ͽ���.
BOOL AsIOCPServer::DisconnectSocket(AsObject* pObject, AsOverlapped* pOverlapped)
{
	if(!pObject)
		return FALSE;

	// ���������� I/O �۾� �Ϸ�
	// JNY TODO : Exception ó���� �ʿ��� �κ�... Ȥ�� Socket ���� ���� ���Ѿ� �Ǵºκ�
	if (ASOBJECT_LISTENER == pObject->GetType())
	{
		// Error�� ���� Socket Type�� LISTENER�̸� ServerSocket�� ReuseSocket���� ���
		printf("ASOBJECT_LISTENER Reuse Socket\n");

		if(pOverlapped)
		{
			((AsOverlapped*)pOverlapped)->pSocket->SetState(ASSERVER_STATE_ALIVE);
			DestroyClient(((AsOverlapped*)pOverlapped)->pSocket);
		}
	}
	else if (ASOBJECT_SOCKET == pObject->GetType())
	{
		AsOverlapped *pAsOverlapped = (AsOverlapped*)pOverlapped;
		if (pAsOverlapped)
		{
			AsServerSocket* pSocket = pAsOverlapped->pSocket;
			
			char* str = NULL;
			str = inet_ntoa(pSocket->GetRemoteInfo()->sin_addr);

			printf("Disconnected Socket [%s]\n",str);
			DestroyClient(pSocket);
		}
	}

	return TRUE;
}

void AsIOCPServer::SetDispatchGameModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_DispatchArchlord.pvModule = pvModule;
	m_DispatchArchlord.fpDispatch = fpDispatch;
	m_DispatchArchlord.fpDisconnect = fpDisconnect;
}

void AsIOCPServer::SetDispatchLKModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_DispatchLKServer.pvModule = pvModule;
	m_DispatchLKServer.fpDispatch = fpDispatch;
	m_DispatchLKServer.fpDisconnect = fpDisconnect;
}

void AsIOCPServer::SetDispatchBillingModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_DispatchBilling.pvModule = pvModule;
	m_DispatchBilling.fpDispatch = fpDispatch;
	m_DispatchBilling.fpDisconnect = fpDisconnect;
}

void AsIOCPServer::SetDispatchPatchModule(PVOID pvModule, AsDispatchCallBack fpDispatch, AsDispatchCallBack fpDisconnect)
{
	m_DispatchPatch.pvModule = pvModule;
	m_DispatchPatch.fpDispatch = fpDispatch;
	m_DispatchPatch.fpDisconnect = fpDisconnect;
}

void AsIOCPServer::SetConnectFunction(AsDispatchCallBack fpConnect)
{
	m_fpConnect = fpConnect;
}

#ifdef __PROFILE__
DWORD AsIOCPServer::GetAcceptableDelay()
{
	return m_dwAcceptableDelay;
}

void AsIOCPServer::SetAcceptableDelay(DWORD  dwDelayTime)
{
	m_dwAcceptableDelay = dwDelayTime;
}

#endif

void AsIOCPServer::SetDefaultSocketType(EnumSocketType eSocketType)
{
	m_eDefaultSocketType = eSocketType;

	m_csSocketManager.SetDefaultSocketType(eSocketType);
}


UINT WINAPI AsIOCPServer::ConnectionChecker(PVOID pvParam)
{
	if (!pvParam)
		return FALSE;

	AsIOCPServer	*pThis	= (AsIOCPServer *)	pvParam;

	while(1)
	{
		// Index �ο�
		for (INT32 i = 0; i < pThis->m_csSocketManager.GetMaxConnectionCount(); i++)
		{
			if (pThis->m_csSocketManager.IsValidArrayIndex(i) &&
				pThis->m_csSocketManager[i]->m_ulConnectedTimeMSec > 0 &&
				pThis->m_csSocketManager[i]->m_ulConnectedTimeMSec + 100000 < ::timeGetTime())
			{
				pThis->m_csSocketManager[i]->m_ulConnectedTimeMSec	= 0;

				// �����ϰ� ����� ������ �� ������ Ȯ���Ѵ�.
				if (!pThis->m_csSocketManager[i]->GetPlayerData()->bServer &&
					//pThis->m_csSocketManager[i]->IsActive() &&
					pThis->m_csSocketManager[i]->GetPlayerData()->lID == 0 &&
					strlen(pThis->m_csSocketManager[i]->GetPlayerData()->szAccountName) <= 0)
				{
					// �̳��� �ҷ��� ���̴�. ������ ���������.
					pThis->DestroyClient(pThis->m_csSocketManager[i]);
				}
			}
		}

		Sleep(30000);
	}

	return 1;
}

BOOL AsIOCPServer::StartConnectionChecker()
{
	m_hThread = (HANDLE)_beginthreadex(NULL, 0, ConnectionChecker, 
		(PVOID) this, 0, &m_ulThreadID);

	if ((!m_hThread) || (!m_ulThreadID)) return FALSE;

	SetThreadName(m_ulThreadID, "ConnectionChecker");

	return TRUE;
}