// AgsmStartupEncryption.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2006.04.11.

#include "AgsmStartupEncryption.h"

AgsmStartupEncryption::AgsmStartupEncryption()
{
	SetModuleName("AgsmStartupEncryption");
	SetModuleType(APMODULE_TYPE_SERVER);

	EnableIdle(FALSE);
	EnableIdle2(FALSE);

	m_pcsAgpmStartupEncryption = NULL;
	
	m_pcsAgsmAOIFilter = NULL;

	m_pcsAgpmConfig = NULL;

	m_pPublicKey = NULL;
	m_pEncryptedPublicKey = NULL;

	m_lPublicKeySize = 0;
	m_lEncryptedKeySize = 0;

	m_csStartupActor.SetCryptType(AUCRYPT_TYPE_PUBLIC);	// Startup �� Public �� ����.
}

AgsmStartupEncryption::~AgsmStartupEncryption()
{
	if(m_pPublicKey)
		delete [] m_pPublicKey;
	if(m_pEncryptedPublicKey)
		delete [] m_pEncryptedPublicKey;
}

BOOL AgsmStartupEncryption::OnAddModule()
{
	m_pcsAgpmStartupEncryption = (AgpmStartupEncryption*)GetModule("AgpmStartupEncryption");
	m_pcsAgsmAOIFilter = (AgsmAOIFilter*)GetModule("AgsmAOIFilter");

	m_pcsAgpmConfig = (AgpmConfig*)GetModule("AgpmConfig");

	if(!m_pcsAgpmStartupEncryption || !m_pcsAgsmAOIFilter)
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackRequestPublic(CBRequestPublic, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackMakePrivate(CBMakePrivate, this))
		return FALSE;

	if(!m_pcsAgpmStartupEncryption->SetCallbackCheckComplete(CBCheckComplete, this))
		return FALSE;

#ifdef _USE_GPK_
	m_lPublicKeySize = 0;
	m_lEncryptedKeySize = 0;
#else
	// ������ Main Key �� ���� ���� ����.
	m_lPublicKeySize = AuPacket::GetCryptManager().GetPublicKeySize();
	m_lEncryptedKeySize = AuPacket::GetCryptManager().GetOutputSize(AUCRYPT_TYPE_PUBLIC, m_lPublicKeySize);
	if(m_lPublicKeySize < 1 || m_lEncryptedKeySize < 1)
	{
		ASSERT(!"Key Size �ʱ�ȭ ����");
		return FALSE;
	}

	m_pPublicKey = new BYTE[m_lPublicKeySize];
	m_pEncryptedPublicKey = new BYTE[m_lEncryptedKeySize];

	memset(m_pPublicKey, 0, m_lPublicKeySize);
	memset(m_pEncryptedPublicKey, 0, m_lEncryptedKeySize);
#endif
	return TRUE;
}

// 2006.06.14. steeple
// AgpmConfig �� ���� �о���δ�.
BOOL AgsmStartupEncryption::SetAlgorithm()
{
	if(!m_pcsAgpmConfig)
		return FALSE;

	LPCTSTR szPublicAlgorithm = m_pcsAgpmConfig->GetEncPublic();
	if(szPublicAlgorithm && _tcslen(szPublicAlgorithm) > 0)
	{
		if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_BLOWFISH, _tcslen(AUCRYPT_ALGORITHM_NAME_BLOWFISH)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_BLOWFISH);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_MD5MAC, _tcslen(AUCRYPT_ALGORITHM_NAME_MD5MAC)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_MD5MAC);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_RIJNDAEL, _tcslen(AUCRYPT_ALGORITHM_NAME_RIJNDAEL)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_RIJNDAEL);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_SEED, _tcslen(AUCRYPT_ALGORITHM_NAME_SEED)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_SEED);
		else if(strnicmp(szPublicAlgorithm, AUCRYPT_ALGORITHM_NAME_DYNCODE, _tcslen(AUCRYPT_ALGORITHM_NAME_DYNCODE)) == 0)
			AuPacket::GetCryptManager().SetPublicAlgorithm(AUCRYPT_ALGORITHM_DYNCODE);
	}

	LPCTSTR szPrivateAlgorithm = m_pcsAgpmConfig->GetEncPrivate();
	if(szPrivateAlgorithm && _tcslen(szPrivateAlgorithm) > 0)
	{
		if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_BLOWFISH, _tcslen(AUCRYPT_ALGORITHM_NAME_BLOWFISH)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_BLOWFISH);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_MD5MAC, _tcslen(AUCRYPT_ALGORITHM_NAME_MD5MAC)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_MD5MAC);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_RIJNDAEL, _tcslen(AUCRYPT_ALGORITHM_NAME_RIJNDAEL)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_RIJNDAEL);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_SEED, _tcslen(AUCRYPT_ALGORITHM_NAME_SEED)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_SEED);
		else if(strnicmp(szPrivateAlgorithm, AUCRYPT_ALGORITHM_NAME_DYNCODE, _tcslen(AUCRYPT_ALGORITHM_NAME_DYNCODE)) == 0)
			AuPacket::GetCryptManager().SetPrivateAlgorithm(AUCRYPT_ALGORITHM_DYNCODE);
	}

	// Print Selected Algorithm
	szPublicAlgorithm = AuPacket::GetCryptManager().GetPublicAlgorithmString();
	printf("Public  Encryption Algorithm : %s\n", szPublicAlgorithm ? szPublicAlgorithm : _T("None"));
	szPrivateAlgorithm = AuPacket::GetCryptManager().GetPrivateAlgorithmString();
	printf("Private Encryption Algorithm : %s\n", szPrivateAlgorithm ? szPrivateAlgorithm : _T("None"));

	return TRUE;
}

// ���� Ű�� �����ؼ� �����Ѵ�.
BOOL AgsmStartupEncryption::InitializePublicKey()
{
	AuPacket::GetCryptManager().MakeRandomKey(m_pPublicKey, m_lPublicKeySize);
	return InitializePublicKey(m_pPublicKey, m_lPublicKeySize);
}

BOOL AgsmStartupEncryption::InitializePublicKey(BYTE* pKey, INT32 lSize)
{
	if(!pKey || lSize < 1 || lSize > m_lPublicKeySize)
		return FALSE;

	// �̹� ���õǼ� �Ѿ�� �� �ƴ϶�� ī��
	if(m_pPublicKey != pKey)
		memcpy(m_pPublicKey, pKey, lSize);

	AuPacket::GetCryptManager().InitializeActor(m_csStartupActor, m_pPublicKey, m_lPublicKeySize);

	// public key �� ��ȣȭ �Ѵ�.
	m_lEncryptedKeySize = AuPacket::GetCryptManager().Encrypt(m_csStartupActor, m_pPublicKey, m_pEncryptedPublicKey, m_lPublicKeySize);

	// �׽�Ʈ ������ �ѹ� ��ȣȭ �غô�.
	//pEngine->m_csCryptManager.Decrypt(m_csStartupActor, m_pEncryptedPublicKey, m_pEncryptedPublicKey, lSize);

	// ��Ƽĳ��Ʈ�� CryptActor �� ��ȣȭ�� Public Key �� �ʱ�ȭ�Ѵ�.
	// ��� ������ Public �� ��ȣȭ�� Public Key �� �ʱ�ȭ�Ѵ�.

	// Ŭ���� ����ϴ� �˰����̶�� �ڿ� TRUE
	if(AuPacket::GetCryptManager().IsPublicClone())
		AuPacket::GetCryptManager().InitializeActorPublic(m_pcsAgsmAOIFilter->GetCryptActorRef(), m_pEncryptedPublicKey, m_lEncryptedKeySize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPublic(m_pcsAgsmAOIFilter->GetCryptActorRef(), m_pEncryptedPublicKey, m_lEncryptedKeySize);

	return TRUE;
}

#ifdef _USE_GPK_
// 2006.06.10. steeple
// Ŭ���̾�Ʈ�� ������ �ϸ� �Ҹ��� �Ǵ� �Լ�.
// Client Socket �� Crypt Actor �� Public, Private ��� �������ش�.
// �׷� �� �Ϸ��ߴٰ� ��Ŷ ������.
BOOL AgsmStartupEncryption::InitializePrivateKeyAtDynCode(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	AuPacket::GetCryptManager().InitializeActorPrivateAtDynCode(pSocket->GetCryptActorRef());
	// pSocket ������ DynCode Index, pCode �����ؼ� �������� ����.
	SendDynCodePrivate(ulNID);

	// �ش� Ŭ���̾�Ʈ���� ��ȣȭ �ʱ�ȭ�� �Ϸ� �ߴٰ� �˸���.
	BOOL bResult = SendComplete(ulNID);

	if(bResult)
	{
		// private �� ����.
		pSocket->GetCryptActorRef().SetCryptType(AUCRYPT_TYPE_PRIVATE);
	}

	return bResult;
}
#endif//_USE_GPK_

BOOL AgsmStartupEncryption::SendAlgorithmType(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	// �� Ŭ���̾�Ʈ�� ���� �˰��� Ÿ�� �����ش�.
	BYTE pData[sizeof(INT32) * 2];
	INT32 lPublicAlgorithm = (INT32)AuPacket::GetCryptManager().GetPublicAlgorithm();
	INT32 lPrivateAlgorithm = (INT32)AuPacket::GetCryptManager().GetPrivateAlgorithm();

	memcpy(pData, &lPublicAlgorithm, sizeof(INT32));
	memcpy(pData + sizeof(INT32), &lPrivateAlgorithm, sizeof(INT32));

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
														AGPMSTARTUPENCRYPTION_PACKET_ALGORITHM_TYPE,
														pData,
														sizeof(INT32) * 2);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendEncryptedPublicKey(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(&nPacketLength,
														AGPMSTARTUPENCRYPTION_PACKET_PUBLIC,
														m_pEncryptedPublicKey,
														m_lEncryptedKeySize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendComplete(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(
													&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_COMPLETE
													);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::SendDynCodePrivate(UINT32 ulNID)
{
	if(ulNID == 0)
		return FALSE;

	AgsEngine* pEngine = static_cast<AgsEngine*>(GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	const BYTE* pCode = NULL;
	INT32 lCodeSize = 0;

	lCodeSize = AuPacket::GetCryptManager().GetClientDynCodePrivate(pSocket->GetCryptActorRef(), &pCode);
	
	if(lCodeSize < 1)
		return FALSE;
	
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pcsAgpmStartupEncryption->MakeStartupPacket(
													&nPacketLength,
													AGPMSTARTUPENCRYPTION_PACKET_DYNCODE_PRIVATE,
													const_cast<BYTE*>(pCode),
													(INT16)lCodeSize);
	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pcsAgpmStartupEncryption->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmStartupEncryption::CBRequestPublic(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pClass || !pCustData)
		return FALSE;

	// Ŭ���̾�Ʈ���� Public Ű�� ������.
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	//AuLogFile("Enc.log", "sm;1-1 NID:%u\n", ulNID);

	// 2006.06.14. steeple
	// ��ȣȭ �˰������ �����ش�.
	pThis->SendAlgorithmType(ulNID);

#ifdef _USE_GPK_
	return pThis->InitializePrivateKeyAtDynCode(ulNID);
#endif
	//AuLogFile("Enc.log", "sm;1-3 NID:%u\n", ulNID);
	return pThis->SendEncryptedPublicKey(ulNID);
}

BOOL AgsmStartupEncryption::CBMakePrivate(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	// Ŭ���̾�Ʈ���� ���� ���� ������ ����Ű�� �����.
	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32 ulNID = *static_cast<UINT32*>(pCustData);

	BYTE* pKey = static_cast<BYTE*>(ppvBuffer[0]);
	INT16 nSize = *static_cast<INT16*>(ppvBuffer[1]);

	//AuLogFile("Enc.log", "sm;2-1 NID:%u\n", ulNID);

	AgsEngine* pEngine = static_cast<AgsEngine*>(pThis->GetModuleManager());
	if(!pEngine)
		return FALSE;

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[ulNID];
	if(!pSocket)
		return FALSE;

	//AuLogFile("Enc.log", "sm;2-2 NID:%u\n", ulNID);

	// �ش� ������ Public �ʱ�ȭ
	// Ŭ���� ����ϴ� �˰����̶�� �ڿ� TRUE
	if(AuPacket::GetCryptManager().IsPublicClone())
		AuPacket::GetCryptManager().InitializeActorPublic(pSocket->GetCryptActorRef(), pThis->m_pEncryptedPublicKey, pThis->m_lEncryptedKeySize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPublic(pSocket->GetCryptActorRef(), pThis->m_pEncryptedPublicKey, pThis->m_lEncryptedKeySize);

	INT32 lOutputSize = AuPacket::GetCryptManager().GetOutputSize(AUCRYPT_TYPE_PUBLIC, (INT32)nSize);
	//AuLogFile("Enc.log", "sm;2-3 NID:%u, lOutputSize:%d\n", ulNID, lOutputSize);
	if(lOutputSize < 1)
		return FALSE;

	// ��ȣȭ �� ���� ����.
	BYTE* pOutput = new BYTE[lOutputSize];

	AuPacket::GetCryptManager().DecryptByPublic(pSocket->GetCryptActorRef(), pKey, pOutput, (INT32)nSize);

	// ������ �ش� Ŭ���̾�Ʈ�� Ű�� pOutput ���� �ȴ�.
	// �׷��Ƿ� �ش� ���Ͽ� �ִ� Actor �� pOutput ���� Initialize ���ش�.
	if(AuPacket::GetCryptManager().IsPrivateClone())
		AuPacket::GetCryptManager().InitializeActorPrivate(pSocket->GetCryptActorRef(), pOutput, (INT32)nSize, TRUE);
	else
		AuPacket::GetCryptManager().InitializeActorPrivate(pSocket->GetCryptActorRef(), pOutput, (INT32)nSize);

	delete [] pOutput;

	//AuLogFile("Enc.log", "sm;2-4 NID:%u\n", ulNID, lOutputSize);

	// �ش� Ŭ���̾�Ʈ���� ��ȣȭ �ʱ�ȭ�� �Ϸ� �ߴٰ� �˸���.
	BOOL bResult = pThis->SendComplete(ulNID);

	if(bResult)
	{
		//AuLogFile("Enc.log", "sm;2-5 NID:%u\n", ulNID, lOutputSize);

		// private �� ����.
		pSocket->GetCryptActorRef().SetCryptType(AUCRYPT_TYPE_PRIVATE);
	}

	//AuLogFile("Enc.log", "sm;2-6 NID:%u\n", ulNID, lOutputSize);
	return bResult;
}

BOOL AgsmStartupEncryption::CBCheckComplete(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	BOOL* pbResult = static_cast<BOOL*>(pData);
	AgsmStartupEncryption* pThis = static_cast<AgsmStartupEncryption*>(pClass);
	UINT32* pulNID = static_cast<UINT32*>(pCustData);

	AgsEngine* pEngine = static_cast<AgsEngine*>(pThis->GetModuleManager());
	if(!pEngine)
	{
		*pbResult = FALSE;
		return FALSE;
	}

	AsServerSocket* pSocket = pEngine->m_AsIOCPServer.m_csSocketManager[*pulNID];
	if(!pSocket)
	{
		*pbResult = FALSE;
		return FALSE;
	}

	// ��ȣȭ�� �ؾ� �ϴ� ��Ȳ�ε� �ش� ������ ��ȣȭ ���� ���°� �ƴ϶�� FALSE �� �������ش�.
	if(AuPacket::GetCryptManager().IsUseCrypt() &&
		pSocket->GetCryptActorRef().GetCryptType() == AUCRYPT_TYPE_NONE)
	{
		*pbResult = FALSE;
		return TRUE;
	}

	return TRUE;
}