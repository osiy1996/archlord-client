#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/menu.h>
#include <AgcSkeleton/camera.h>

#include <rwcore.h>
#include <rpworld.h>
#include <rphanim.h>
#include <rpskin.h>

#include <ApModule/ApModule.h>
#include <AgcModule/AgcEngine.h>
#include "AgcmConnectManager.h"

#include <ApBase/MagDebug.h>


AgcmConnectManager::AgcmConnectManager()
{
	SetModuleName("AgcmConnectManager");

	SetPacketType(AGCMCONNECTMANAGER_PACKET_TYPE);

	m_csPacket.SetFlagLength(sizeof(CHAR));
	m_csPacket.SetFieldType(
							AUTYPE_INT8,			1,		// operation
							AUTYPE_INT32,			1,		// character id
							AUTYPE_INT8,			1,		// operation�� ���� status
							AUTYPE_CHAR,			23,		// zone server address (xxx.xxx.xxx.xxx:port)
							AUTYPE_INT32,			1,		// zone server ID
							AUTYPE_PACKET,			1,		// character server data		// not used
							AUTYPE_END,				0);

	ZeroMemory(m_stConnectionArray, sizeof(stConnectionArray) * AGCMCONNECTMANAGER_MAX_CONNECTLIST);

	nControlServerNID = (-1);
}

AgcmConnectManager::~AgcmConnectManager()
{
}

BOOL AgcmConnectManager::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket)
		return FALSE;

	INT32	lServerID	= 0;

	INT8	cOperation	= (-1);
	INT32	lCID		= 0;
	INT8	cStatus		= (-1);
	CHAR*	szAddress	= NULL;
	PVOID	pvTempPacket;

	m_csPacket.GetField(TRUE, pvPacket, nSize, 
						&cOperation,
						&lCID,
						&cStatus,
						&szAddress,
						&lServerID,
						&pvTempPacket);

	switch (cOperation) {
	case AGCMCONNECTMANAGER_PACKET_OPERATION_SEND_ZONESERVER_ADDR:			// �ٸ� ������ �����϶�� ����̴�.
		{
			if (!szAddress || !strlen(szAddress))
				return FALSE;

			// szAddress�� ������ �õ��Ѵ�.
			INT16 nNID = Connect(szAddress, ACDP_SERVER_TYPE_GAMESERVER);
			if (nNID < 0)
				return FALSE;

			strncpy(m_stConnectionArray[nNID].szAddress, szAddress, 23);

			SendZoningMessage(lCID, (INT16) nNID);
		}
		break;

	case AGCMCONNECTMANAGER_PACKET_OPERATION_CONNECT_ZONESERVER_RESULT:	// ������ ������ ���
		{
			if (cStatus == FALSE)
			{
			}
			else
			{
			}

			m_stConnectionArray[ulNID].lServerID = lServerID;

			SendZoningResult(lCID, cStatus, /* (INT16) ulNID */ 0, lServerID);
		}
		break;

	case AGCMCONNECTMANAGER_PACKET_OPERATION_PASS_CONTROL:					// ������ �Ѿ��. ���... �� ĳ���͸� �����ϴ� ������ �ٲ��.
		{
			nControlServerNID = (INT16) ulNID;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

INT16 AgcmConnectManager::Connect(CHAR *szAddress, INT16 nServerType, PVOID pClass, ApModuleDefaultCallBack fpConnect, 
									ApModuleDefaultCallBack fpDisconnect, ApModuleDefaultCallBack fpError)
{
	ASSERT( NULL != szAddress		);
	ASSERT( 0 < strlen(szAddress)	);

	int len = strlen(szAddress);

	if (!szAddress || !strlen(szAddress))
		return (-1);

	AgcEngine *pcsModuleManager = (AgcEngine *) GetModuleManager();

	ASSERT( NULL != pcsModuleManager);
	if (!pcsModuleManager)
		return (-1);

	INT16 nNID = pcsModuleManager->connect(szAddress, nServerType, pClass, fpConnect, fpDisconnect, fpError);
	if (nNID < 0)
		return nNID;

	strncpy(m_stConnectionArray[nNID].szAddress, szAddress, 23);

	if (nControlServerNID < 0)
		nControlServerNID = nNID;

	m_stConnectionArray[nNID].nServerType = nServerType;

	return nNID;
}

BOOL AgcmConnectManager::Disconnect(INT16 nNID)
{
	ASSERT( 0 <= nNID	);
	if (nNID < 0)
		return (-1);

	AgcEngine *pcsModuleManager = (AgcEngine *) GetModuleManager();

	ASSERT( NULL != pcsModuleManager);
	if (!pcsModuleManager)
		return (-1);

	ZeroMemory(m_stConnectionArray + nNID, sizeof(stConnectionArray));

	return pcsModuleManager->disconnect(nNID);
}

BOOL AgcmConnectManager::DisconnectAll()
{
	for (int i = 0; i < AGCMCONNECTMANAGER_MAX_CONNECTLIST; ++i)
	{
		if (strlen(m_stConnectionArray[i].szAddress))
		{
			Disconnect(i);
		}
	}

	return TRUE;
}

BOOL AgcmConnectManager::SendZoningMessage(INT32 lCID, INT16 nNID)
{
	INT8	cOperation	= AGCMCONNECTMANAGER_PACKET_OPERATION_CONNECT_ZONESERVER;
	INT16	nPacketLength;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGCMCONNECTMANAGER_PACKET_TYPE, 
											&cOperation,
											&lCID,
											NULL,
											NULL,
											NULL,
											NULL);
	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, ACDP_SEND_SERVER, PACKET_PRIORITY_NONE, nNID))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgcmConnectManager::SendZoningResult(INT32 lCID, INT8 cStatus, INT16 nNID, INT32 lServerID)
{
	INT8	cOperation	= AGCMCONNECTMANAGER_PACKET_OPERATION_CONNECT_ZONESERVER_RESULT;
	INT16	nPacketLength;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGCMCONNECTMANAGER_PACKET_TYPE, 
											&cOperation,
											&lCID,
											&cStatus,
											NULL,
											&lServerID,
											NULL);

	if (!pvPacket)
		return FALSE;

	if (!SendPacket(pvPacket, nPacketLength, ACDP_SEND_GAMESERVERS, PACKET_PRIORITY_NONE, nNID))
	{
		m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}

	m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//		GetControlServerID
//	Functions
//		- �� Ŭ���̾�Ʈ�� ��Ʈ���� ����ϰ� �ִ� ������ �Ƶڸ� �˾ƿ´�.
//	Arguments
//		- none
//	Return value
//		- INT32 : ServerID or AP_INVALID_SERVERID
///////////////////////////////////////////////////////////////////////////////
INT32 AgcmConnectManager::GetControlServerID()
{
	if (nControlServerNID < 0 || nControlServerNID >= AGCMCONNECTMANAGER_MAX_CONNECTLIST)
		return AP_INVALID_SERVERID;

	return m_stConnectionArray[nControlServerNID].lServerID;
}

//		GetControlServerNID
//	Functions
//		- �� Ŭ���̾�Ʈ�� ��Ʈ���� ����ϰ� �ִ� ������ NID�� �˾ƿ´�.
//	Arguments
//		- none
//	Return value
//		- INT16 : ServerNID or ���н� 0���� ������
///////////////////////////////////////////////////////////////////////////////
INT16 AgcmConnectManager::GetControlServerNID()
{
	return nControlServerNID;
}

//		GetDealServerNID
//	Functions
//		- �� ������ NID�� �����Ѵ�.
//	Arguments
//		- none
//	Return value
//		- INT16 : ServerNID or ���н� 0���� ������
///////////////////////////////////////////////////////////////////////////////
INT16 AgcmConnectManager::GetDealServerNID()
{
	for (int i = 0; i < AGCMCONNECTMANAGER_MAX_CONNECTLIST; ++i)
	{
		if ( m_stConnectionArray[i].nServerType == ACDP_SERVER_TYPE_DEALSERVER )
			return i;
	}

	return (-1);
}

//		GetRecruitServerNID
//	Functions
//		- Recruit ������ NID�� �����Ѵ�.
//	Arguments
//		- none
//	Return value
//		- INT16 : ServerNID or ���н� 0���� ������
///////////////////////////////////////////////////////////////////////////////
INT16 AgcmConnectManager::GetRecruitServerNID()
{
	for (int i = 0; i < AGCMCONNECTMANAGER_MAX_CONNECTLIST; ++i)
	{
		if ( m_stConnectionArray[i].nServerType == ACDP_SERVER_TYPE_RECRUITSERVER )
			return i;
	}

	return (-1);
}

//		GetLoginServerNID
//	Functions
//		- Recruit ������ NID�� �����Ѵ�.
//	Arguments
//		- none
//	Return value
//		- INT16 : ServerNID or ���н� 0���� ������
///////////////////////////////////////////////////////////////////////////////
INT16 AgcmConnectManager::GetLoginServerNID()
{
	for (int i = 0; i < AGCMCONNECTMANAGER_MAX_CONNECTLIST; ++i)
	{
		if ( m_stConnectionArray[i].nServerType == ACDP_SERVER_TYPE_LOGINSERVER )
			return i;
	}

	return (-1);
}
