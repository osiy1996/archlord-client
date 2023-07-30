// AgcmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#include "AgcmStartupEncryption.h"
#include <AgpmStartupEncryption/AgpmStartupEncryption.h>
#include <AgcmReturnToLogin/AgcmReturnToLogin.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmLogin/AgcmLogin.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmConnectManager/AgcmConnectManager.h>

AgcmStartupEncryption::AgcmStartupEncryption()
{
	SetModuleName("AgcmStartupEncryption");
	SetModuleType(APMODULE_TYPE_CLIENT);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	m_pcsAgpmStartupEncryption = NULL;
	m_pcsAgcmLogin = NULL;
	m_pcsAgcmReturnToLogin = NULL;
	m_pcsAgcmCharacter = NULL;
	m_pcsAgcmConnectManager	= NULL;
}

AgcmStartupEncryption::~AgcmStartupEncryption()
{
}

BOOL AgcmStartupEncryption::OnAddModule()
{
	m_pcsAgpmStartupEncryption = (AgpmStartupEncryption*)GetModule("AgpmStartupEncryption");
	m_pcsAgcmLogin = (AgcmLogin*)GetModule("AgcmLogin");
	m_pcsAgcmCharacter = (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgcmReturnToLogin = (AgcmReturnToLogin*)GetModule("AgcmReturnToLogin");
	m_pcsAgcmConnectManager	= (AgcmConnectManager*)GetModule("AgcmConnectManager");

	if(!m_pcsAgpmStartupEncryption || !m_pcsAgcmLogin || !m_pcsAgcmCharacter || !m_pcsAgcmReturnToLogin ||
		!m_pcsAgcmConnectManager)
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackAlgorithmType(CBAlgorithmType, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackPublic(CBPublic, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackComplete(CBComplete, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackDynCodePublic(CBDynCodePublic, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackDynCodePrivate(CBDynCodePrivate, this))
		return FALSE;

	if(!m_pcsAgcmLogin->SetCallbackConnectLoginServer(CBConnectLoginServer, this))
		return FALSE;

	if(!m_pcsAgcmCharacter->SetCallbackSocketOnConnect(CBConnectGameServer, this))
		return FALSE;

	if(!m_pcsAgcmReturnToLogin->SetCallBackReconnectLoginServer(CBReconnectLoginServer, this))
		return FALSE;

	return TRUE;
}

BOOL AgcmStartupEncryption::SendRequestPublic(INT16 nNID)
{
	if(nNID < 0)
		return FALSE;

	AgcEngine* pEngine = static_cast<AgcEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(nNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_REQUEST_PUBLIC);
    if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, pstConnection->nServerType, PACKET_PRIORITY_NONE, nNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgcmStartupEncryption::SendEncryptedRandom(BYTE* pKey, INT32 nSize, INT16 nNID)
{
	if(nNID < 0)
		return FALSE;

	AgcEngine* pEngine = static_cast<AgcEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(nNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_MAKE_PRIVATE,
													pKey, nSize);
    if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, pstConnection->nServerType, PACKET_PRIORITY_NONE, nNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// 2006.06.14. steeple
// ������ ���� �˰��� Ÿ���� �޾Ҵ�.
BOOL AgcmStartupEncryption::CBAlgorithmType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pByte = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	// pData �� ���� 4����Ʈ�� Public, ���� 4����Ʈ�� Private �̴�.
	// ��� nSize �� 8�� �ƴϸ� ����
	if(nSize != sizeof(INT32) * 2)
		return FALSE;

	INT32 lPublicAlgorithm = -1;
	INT32 lPrivateAlgorithm = -1;
	memcpy(&lPublicAlgorithm, pByte, sizeof(INT32));
	memcpy(&lPrivateAlgorithm, pByte + sizeof(INT32), sizeof(INT32));

	// ���� ������� ��츸 ����
	if(lPublicAlgorithm > -1 && lPrivateAlgorithm > -1)
	{
		AuPacket::GetCryptManager().SetPublicAlgorithm((eAuCryptAlgorithm)lPublicAlgorithm);
		AuPacket::GetCryptManager().SetPrivateAlgorithm((eAuCryptAlgorithm)lPrivateAlgorithm);
	}

	return TRUE;
}

BOOL AgcmStartupEncryption::CBPublic(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	// �������� ���� ���� ������ Private �� ��ȣȭ �Ѵ�.
	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pKey = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	//ASSERT(nSize == AuPacket::GetCryptManager().GetPublicKeySize());

	AgcEngine* pEngine = static_cast<AgcEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(ulNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	// ���� Key �� Public ����
	if(AuPacket::GetCryptManager().IsPublicClone())
		AuPacket::GetCryptManager().InitializeActorPublic(pstConnection->Socket.GetCryptActorRef(), pKey, nSize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPublic(pstConnection->Socket.GetCryptActorRef(), pKey, nSize);

	// Private ����
	// Public, Private Ű�� ������� ���� �����ߴ�. 2006.04.20. steeple
	INT32 lPrivateKeySize = AuPacket::GetCryptManager().GetPrivateKeySize();
	INT32 lEncryptedKeySize = AuPacket::GetCryptManager().GetOutputSize(AUCRYPT_TYPE_PRIVATE, lPrivateKeySize);

	BYTE* pRandom = new BYTE[lPrivateKeySize];
	// 8�ڸ� ���ڷ� �Ѵ�.
	for(int i = 0; i < lPrivateKeySize; i++)
		pRandom[i] = (BYTE)pThis->m_csRandom.randInt(255);

	if(AuPacket::GetCryptManager().IsPrivateClone())
		AuPacket::GetCryptManager().InitializeActorPrivate(pstConnection->Socket.GetCryptActorRef(), pRandom, lPrivateKeySize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPrivate(pstConnection->Socket.GetCryptActorRef(), pRandom, lPrivateKeySize);

	BYTE* pOutput = new BYTE[lEncryptedKeySize];

	INT32 lOutputSize = AuPacket::GetCryptManager().EncryptByPublic(pstConnection->Socket.GetCryptActorRef(), pRandom, pOutput, lPrivateKeySize);

	pThis->SendEncryptedRandom(pOutput, lOutputSize, (INT16)ulNID);

	delete [] pOutput;
	delete [] pRandom;

	return TRUE;
}

BOOL AgcmStartupEncryption::CBComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	AgcEngine* pEngine = static_cast<AgcEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(ulNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	pstConnection->Socket.GetCryptActorRef().SetCryptType(AUCRYPT_TYPE_PRIVATE);

	// �α��� ������ ����� ���ϰ� ���Ӽ����� ����� ������ �����Ѵ�.
	switch(pstConnection->nServerType)
	{
		case ACDP_SERVER_TYPE_LOGINSERVER:
		{
			if(pThis->m_pcsAgcmReturnToLogin->GetAuthKey() == 0)
				pThis->m_pcsAgcmLogin->SendGetEncryptCode();				// �Ϲ� ����
			else
				pThis->m_pcsAgcmReturnToLogin->SendReconnectLoginServer();	// ĳ���� ���� �ǵ��ư��� ��
			break;
		}

		case ACDP_SERVER_TYPE_GAMESERVER:
			pThis->m_pcsAgcmCharacter->SendRequestClientConnection(ulNID);
			break;
	}

	return TRUE;
}

// Dynamic Code Public �޾Ҵ�.
BOOL AgcmStartupEncryption::CBDynCodePublic(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pCode = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	AgcEngine* pEngine = static_cast<AgcEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(ulNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	// �ʱ�ȭ ���ش�.
	// pCode, nSize �� DynCode Client Library �� �ʱ�ȭ �ϴ� �� ���δ�. Key �� �ƴ����� �׷��� �� ���� �ִ�.
#ifdef _AREA_CHINA_
	AuPacket::GetCryptManager().InitializeActorPrivateAtDynCode( pCode, nSize );
#else
	if(AuPacket::GetCryptManager().IsUseDynCode())
		AuPacket::GetCryptManager().InitializeActorPublic(pstConnection->Socket.GetCryptActorRef(), pCode, nSize);
#endif

	return TRUE;
}

// Dynamic Code Private �޾Ҵ�.
BOOL AgcmStartupEncryption::CBDynCodePrivate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pCode = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	AgcEngine* pEngine = static_cast<AgcEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	stConnectionList* pstConnection = pEngine->m_SocketManager.GetConnectionListPtr(ulNID);
	if(!pstConnection || pstConnection->Socket.GetState() == ACSOCKET_STATE_NULL)
		return FALSE;

	// �ʱ�ȭ ���ش�.
	// pCode, nSize �� DynCode Client Library �� �ʱ�ȭ �ϴ� �� ���δ�. Key �� �ƴ����� �׷��� �� ���� �ִ�.
#ifdef _AREA_CHINA_
	AuPacket::GetCryptManager().InitializeActorPrivateAtDynCode( pCode, nSize );
#else
	if(AuPacket::GetCryptManager().IsUseDynCode())
		AuPacket::GetCryptManager().InitializeActorPublic(pstConnection->Socket.GetCryptActorRef(), pCode, nSize);
#endif
	return TRUE;
}

BOOL AgcmStartupEncryption::CBConnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass)
		return FALSE;

	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);

	// pData �� ������ ������ ���̰� ������ ���� ���� ���̴�.
	// pData ���� AgcEngin::m_AcSocketManager ���� ������ Socket �� Index �� �´�. �׷��� INT16
	INT16 nNID = (pData) ? *static_cast<INT16*>(pData) : -1;

	if(nNID != -1)
	{
		if(AuPacket::GetCryptManager().IsUseCrypt())
		{
			// public Ű�� ��û�Ѵ�.
			pThis->SendRequestPublic(nNID);
		}
		else
		{
			pThis->m_pcsAgcmLogin->SendGetEncryptCode();
		}
	}
	else
	{
		// Cleanup �� AcClientSocket ���� ���� �Ѵ�.
	}

	return TRUE;
}

BOOL AgcmStartupEncryption::CBConnectGameServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	INT32 lNID = *static_cast<INT32*>(pCustData);

	if(AuPacket::GetCryptManager().IsUseCrypt())
		pThis->SendRequestPublic((INT16)lNID);
	else
		pThis->m_pcsAgcmCharacter->SendRequestClientConnection(lNID);

	return TRUE;
}

BOOL AgcmStartupEncryption::CBReconnectLoginServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass)
		return FALSE;

	AgcmStartupEncryption* pThis = static_cast<AgcmStartupEncryption*>(pClass);
	if(AuPacket::GetCryptManager().IsUseCrypt())
		pThis->SendRequestPublic((INT16)pThis->m_pcsAgcmConnectManager->GetLoginServerNID());
	else
		pThis->m_pcsAgcmReturnToLogin->SendReconnectLoginServer();
	return TRUE;
}