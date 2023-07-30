/*===========================================================================

	AgsmLoginClient.cpp

===========================================================================*/


#include "AgsmLoginClient.h"
#include "ApModuleStream.h"
#include "AuStrTable.h"
#include "AuStringConv.h"

#define LOGIN_ERROR_FILE_NAME		"LOG\\LoginError.log"


CHAR	*g_pszCharName[AUCHARCLASS_TYPE_MAX * (AURACE_TYPE_MAX - 1)]	=
	{	
	NULL,
	};
 
INT32 g_lCharTID[AUCHARCLASS_TYPE_MAX * 3]	=
	{
	0,
	};

#ifndef _AREA_CHINA_
/************************************************************************/
/* base character setting                                                                      */
/************************************************************************/
CHAR	*g_pszNormalCharName[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX] = {NULL, };
INT32	 g_lNormalCharTID[AURACE_TYPE_MAX][AUCHARCLASS_TYPE_MAX] = {0, };
#endif

/********************************************************/
/*		The Implementation of AgsmLoginClient Class		*/
/********************************************************/
//
AgsmLoginClient::AgsmLoginClient()
	{
	SetModuleName(_T("AgsmLoginClient"));
	SetModuleType(APMODULE_TYPE_SERVER);
	SetPacketType(AGPMLOGIN_PACKET_TYPE);

	SetModuleData(sizeof(AgsmBaseCharacterPos), AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS);

	ZeroMemory(m_lRaceBaseCharacter, sizeof(INT32) * AURACE_TYPE_MAX * (AUCHARCLASS_TYPE_MAX * 3));
	ZeroMemory(m_RaceBaseCharacterPos, sizeof(AgsmBaseCharacterPos) * AURACE_TYPE_MAX * (AUCHARCLASS_TYPE_MAX * 3));
	
	m_lMaxProcessUserCount			= g_lMaxProcessUserCount;
	m_lMaxCreateProcessUserCount	= g_lMaxCreateProcessUserCount;

	ZeroMemory(g_lCharTID, sizeof(INT32) * (AUCHARCLASS_TYPE_MAX * 3));

	g_pszCharName[0] = ServerStr().GetStr(STI_KNIGHT);
	g_pszCharName[1] = ServerStr().GetStr(STI_ARCHER);
	g_pszCharName[2] = ServerStr().GetStr(STI_MAGE);
	g_pszCharName[3] = ServerStr().GetStr(STI_BERSERKER);
	g_pszCharName[4] = ServerStr().GetStr(STI_HUNTER);
	g_pszCharName[5] = ServerStr().GetStr(STI_SORCERER);
	g_pszCharName[6] = ServerStr().GetStr(STI_ELEMETALIST);
	g_pszCharName[7] = ServerStr().GetStr(STI_RANGER);
	g_pszCharName[8] = ServerStr().GetStr(STI_SWASHBUKLER);
	g_pszCharName[9] = ServerStr().GetStr(STI_SCION);

	g_lCharTID[0] = 96; //�޸� ������
	g_lCharTID[1] = 1;  //�޸� �ü�
	g_lCharTID[2] = 6;  //�޸� ���� ����.
	g_lCharTID[3] = 4;  //��ũ ������
	g_lCharTID[4] = 8;  //��ũ ���ü�
	g_lCharTID[5] = 3;  //��ũ ������.
	g_lCharTID[6] = 9;  // ������ ���� ����
	g_lCharTID[7] = 460;// ������ ���� �ü�
	g_lCharTID[8] = 377;// ������ ���� ����
	g_lCharTID[9] = 1724;// �巹��ÿ� �ÿ�
	}


AgsmLoginClient::~AgsmLoginClient()
	{
	}


BOOL AgsmLoginClient::OnInit()
{
	return TRUE;
}


//	ApModule inherited
//=========================================================
//
BOOL AgsmLoginClient::OnAddModule()
	{
	m_csCertificatedNID.InitializeObject(sizeof(AgsmLoginEncryptInfo*), AGPMLOGIN_MAX_CONNECTION);

	m_pAgpmLogin = (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgpmFactors = (AgpmFactors *) GetModule(_T("AgpmFactors"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgpmItem = (AgpmItem *) GetModule(_T("AgpmItem"));
	m_pAgpmAdmin = (AgpmAdmin *) GetModule(_T("AgpmAdmin"));
	m_pAgsmAccountManager = (AgsmAccountManager *) GetModule(_T("AgsmAccountManager"));
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmCharManager = (AgsmCharManager *) GetModule(_T("AgsmCharManager"));
	m_pAgsmItem = (AgsmItem *) GetModule(_T("AgsmItem"));
	m_pAgsmItemManager = (AgsmItemManager *) GetModule(_T("AgsmItemManager"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmLoginServer = (AgsmLoginServer *) GetModule(_T("AgsmLoginServer"));
	m_pAgpmResourceInfo = (AgpmResourceInfo *) GetModule(_T("AgpmResourceInfo"));
	m_pAgsmFactors = (AgsmFactors *) GetModule(_T("AgsmFactors"));
	m_pAgsmGK = (AgsmGK *) GetModule(_T("AgsmGK"));
	m_pAgpmEventCharCustomize = (AgpmEventCharCustomize *) GetModule(_T("AgpmEventCharCustomize"));
	m_pAgpmStartupEncryption = (AgpmStartupEncryption *) GetModule(_T("AgpmStartupEncryption"));
	m_pAgpmConfig = (AgpmConfig *) GetModule("AgpmConfig");

	if (!m_pAgpmLogin || !m_pAgpmFactors || !m_pAgpmCharacter || !m_pAgsmCharacter || !m_pAgsmCharManager || 
		!m_pAgpmItem || !m_pAgsmItem || !m_pAgsmItemManager || !m_pAgsmServerManager ||
		!m_pAgsmLoginServer || !m_pAgsmAccountManager || !m_pAgpmAdmin || !m_pAgsmFactors || !m_pAgsmGK || !m_pAgpmEventCharCustomize ||
		!m_pAgpmConfig)
		{
		return FALSE;
		}

	// client disconnect callback
	AgsEngine *pModuleManager = (AgsEngine *) GetModuleManager();
	pModuleManager->SetCBLoginDisconnect(this, CBDisconnect);

	if (!m_pAgsmCharacter->SetCallbackReceiveNewCID(CBReceiveNewCID, this))
		return FALSE;
	if (!m_pAgsmCharacter->SetCallbackCompleteSendCharacterInfo(CBCompleteSendCharacterInfo, this))
		return FALSE;

	if (!AddStreamCallback(AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS, BaseCharacterPosReadCB, NULL, this))
		return FALSE;

	return TRUE;
	}


BOOL AgsmLoginClient::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	BOOL bResult = FALSE;

	if (FALSE == m_pAgsmLoginServer->CheckQueueCount())
		{
		_tprintf("!!! Warning : Queue FULL in AgsmLoginClient::OnReceive\n");
		return bResult;
		}
	
	INT8	cOperation;
	CHAR	*pszEncryptCode = NULL;
	CHAR	*pszAccountID = NULL;
	CHAR	*pszPassword = NULL;
	INT8	cAccountLen = 0;
	INT8	cPasswordLen = 0;
	INT32	lCID = AP_INVALID_CID;
	CHAR	*pszServerName = NULL;
	PVOID	pvCharDetailInfo = NULL;
	PVOID	pvDetailServerInfo = NULL;
	INT32	lResult;
	PVOID	pvPacketVersionInfo	= NULL;
	CHAR	*pszExtraForForeign = 0;		// ekey for china, authstring for japan.
	CHAR	*pszChallenge = 0;
	INT32	lIsLimited = 0;
	INT32	lIsProtected = 0;

	m_pAgpmLogin->m_csPacket.GetField(TRUE, pvPacket, nSize,
								&cOperation,
								&pszEncryptCode,
								&pszAccountID,
								&cAccountLen,
								&pszPassword,
								&cPasswordLen,
								&lCID,
								&pszServerName,
								&pvCharDetailInfo,
								&pvDetailServerInfo,
								&lResult,
								&pvPacketVersionInfo,
								&pszExtraForForeign,
								&pszChallenge,
								&lIsLimited,
								&lIsProtected
								);

	AuLogConsole_s("[%d] %s( operation %d", ulNID, __FUNCTION__, cOperation);

	if (AGPMLOGIN_ENCRYPT_CODE == cOperation)
		{
		bResult = ProcessEncryptCode(pvPacketVersionInfo, ulNID);
		if (!bResult)
			DestroyClient(ulNID);
		}
	else if (AGPMLOGIN_SIGN_ON == cOperation)
		{
		bResult = ProcessSignOn(pszAccountID, cAccountLen, pszPassword, cPasswordLen, ulNID, pszExtraForForeign);
		}
	else if (AGPMLOGIN_EKEY == cOperation)
	{
		bResult = ProcessEKey(pszExtraForForeign, ulNID);
	}
	else
		{
		if (FALSE == IsCertificatedNID(ulNID))
			return FALSE;

		AgsmLoginEncryptInfo *	pEncryptInfo = GetCertificatedNID(ulNID);
		if (!pEncryptInfo)
			return FALSE;

		if (_tcslen(pEncryptInfo->m_szAccountID) <= 0
			|| _tcscmp(pEncryptInfo->m_szAccountID, pszAccountID) != 0
			)
			return FALSE;

		if (cOperation != AGPMLOGIN_UNION_INFO)
			pszServerName = pEncryptInfo->m_szServerName;

		AgsdServer *			pcsServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
		CHAR *					pszDBName = NULL;

		if (pcsServer)
			pszDBName = pcsServer->m_szWorld;
		
		switch (cOperation)
			{
			case AGPMLOGIN_UNION_INFO :
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessGetUnion(pszAccountID, pszServerName, pszDBName, ulNID);

				_tcscpy(pEncryptInfo->m_szServerName, pszServerName);

				break;

			case AGPMLOGIN_CHARACTER_INFO :
				if (NULL == pszDBName)
					return FALSE;
				// reset compensation
				pEncryptInfo->m_lCompenID = 0;
				ZeroMemory(pEncryptInfo->m_szChar4Compen, sizeof(pEncryptInfo->m_szChar4Compen));
				bResult = ProcessGetCharacters(pszAccountID, pszServerName, pszDBName, ulNID);
				break;

			case AGPMLOGIN_ENTER_GAME :		
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessSelectCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, lCID);
				break;

			case AGPMLOGIN_RETURN_TO_SELECT_WORLD :
				bResult = ProcessReturnToSelectWorld(pszAccountID, ulNID);
				break;

			// Create Character
			case AGPMLOGIN_RACE_BASE :
				bResult = ProcessBaseCharacterOfRace(pvCharDetailInfo, ulNID);
				break;

			case AGPMLOGIN_NEW_CHARACTER_NAME :
				if (NULL == pszDBName)
					return FALSE;
#ifndef _AREA_CHINA_				
				// ���� �̺�Ʈ �Ⱓ���� Ȯ���Ѵ�
				if( m_pAgpmConfig->IsSetServerStatusFlag(AGPM_CONFIG_FLAG_CHARACTER_JUMPING_EVENT) )
				{
					bResult = ProcessCreateJumpingCharacterCheck(pszAccountID, pszServerName, pszDBName, ulNID);
				}
				else
				{
					bResult = ProcessCreateCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				}
				break;

			case AGPMLOGIN_CREATE_CHARACTER:
				if (NULL == pszDBName)
					return FALSE;
#endif
				bResult = ProcessCreateCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;

			// Rename Character
			case AGPMLOGIN_RENAME_CHARACTER :
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessRenameCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;			
			
			// Remove Character
			case AGPMLOGIN_REMOVE_CHARACTER :
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessRemoveCharacter(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID);
				break;
				
			case AGPMLOGIN_COMPENSATION_CHARACTER_SELECT :
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessCharacter4Compensation(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, TRUE);
				break;			
			
			case AGPMLOGIN_COMPENSATION_CHARACTER_CANCEL :
				if (NULL == pszDBName)
					return FALSE;
				bResult = ProcessCharacter4Compensation(pszAccountID, pszServerName, pszDBName, pvCharDetailInfo, ulNID, FALSE);
				break;
				
			default :
				break;
			}
		}
	
	return bResult;
	}




//	Admin
//==========================================
//
AgsmLoginEncryptInfo* AgsmLoginClient::GetCertificatedNID( UINT32 ulNID )
	{
	AgsmLoginEncryptInfo **ppAgsmLoginEncryptInfo;

	ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo **)m_csCertificatedNID.GetObject( ulNID );

	if (ppAgsmLoginEncryptInfo)
		return *ppAgsmLoginEncryptInfo;
		
	return NULL;
	}


BOOL AgsmLoginClient::IsCertificatedNID( UINT32 ulNID )
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		return pAgsmLoginEncryptInfo->m_bLogin;
		}
		
	return FALSE;
	}


BOOL AgsmLoginClient::SetCertificatedNID(UINT32 ulNID, CHAR *pszAccount, UINT64 ullKey)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_bLogin = TRUE;
		ZeroMemory(pAgsmLoginEncryptInfo->m_szAccountID, sizeof(pAgsmLoginEncryptInfo->m_szAccountID));
		strncpy(pAgsmLoginEncryptInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING-1);
		pAgsmLoginEncryptInfo->m_ullKey = ullKey;
		
		return TRUE;
		}
	
	return FALSE;
	}


BOOL AgsmLoginClient::SetLoginStep(UINT32 ulNID, AgpdLoginStep	eLoginStep)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_eLoginStep = eLoginStep;
		return TRUE;
		}
	
	return FALSE;
	}


AgpdLoginStep AgsmLoginClient::GetLoginStep(UINT32 ulNID)
	{
	AgpdLoginStep eLoginStep = AGPMLOGIN_STEP_NONE;
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		eLoginStep = pAgsmLoginEncryptInfo->m_eLoginStep;
		}
	
	return eLoginStep;
	}




//	Operations
//=========================================================
//
BOOL AgsmLoginClient::ProcessEncryptCode(PVOID pvPacketVersionInfo, UINT32 ulNID, BOOL bCheckVersion)
	{
	if (m_pAgpmStartupEncryption)
		{
		if(!m_pAgpmStartupEncryption->CheckCompleteStartupEncryption(ulNID))
			{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 1\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			return FALSE;
			}
		}

	if (TRUE == bCheckVersion)
		{
		if (NULL == pvPacketVersionInfo)
			{
			AuLogConsole_s("[%d] %s - Invalid Client Version ", ulNID, __FUNCTION__);
			SendInvalidClientVersion(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 2\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			return FALSE;
			}

		INT32	lMajorVersion	= 0;
		INT32	lMinorVersion	= 0;

		// check client version
		m_pAgpmLogin->m_csPacketVersionInfo.GetField(FALSE, pvPacketVersionInfo, 0, &lMajorVersion, &lMinorVersion);

		if (FALSE == m_pAgpmResourceInfo->CheckValidVersion(lMajorVersion, lMinorVersion))
			{
			AuLogConsole_s("[%d] %s - Invalid Client Version ", ulNID, __FUNCTION__);
			SendInvalidClientVersion(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 3\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

			return FALSE;
			}
		}

	BOOL bResult = FALSE;
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = new AgsmLoginEncryptInfo;

	if (NULL != m_csCertificatedNID.GetObject(ulNID))
		{
		m_pAgsmCharManager->DestroyClient( ulNID );

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 4\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		}
	else
		{
		if (NULL != m_csCertificatedNID.AddObject((PVOID *)&pAgsmLoginEncryptInfo, ulNID))
			{
			CHAR *pszEncryptCode = m_csMD5Encrypt.GetRandomHashString(ENCRYPT_STRING_SIZE);
			if (NULL != pszEncryptCode)
				{
				//EncryptData ����.
				pAgsmLoginEncryptInfo->m_pszEncryptData = pszEncryptCode;
				bResult = SendEncryptCode(pszEncryptCode, ulNID);
				}
			}
		else
			{
			m_pAgsmCharManager->DestroyClient(ulNID);

			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessEncryptCode - 5\n");
			AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
			}
		}

	return bResult;
	}

BOOL AgsmLoginClient::ProcessSignOn(CHAR *pszAccount, INT8 cAccountLen, CHAR *pszPassword, INT8 cPasswordLen, UINT32 ulNID, CHAR* pszExtraForForeign)
	{
	if (NULL == pszAccount || NULL == pszPassword)
	{
		printf("Error: Account or Password is NULL\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 1\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		return FALSE;
	}

	UINT32 ulAccountLen = (UINT32)cAccountLen;
	UINT32 ulPasswordLen = (UINT32)cPasswordLen;
	
	if (0 >= ulAccountLen || 0 >= ulPasswordLen
		|| ulAccountLen > AGPACHARACTER_MAX_ID_STRING
		|| ulPasswordLen > AGPACHARACTER_MAX_PW_STRING
		)
	{
		printf("Error: Account or Password length is wrong\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 2\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}
		
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
	{
		printf("Error: Encryption failed.\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 3\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);
		
		return FALSE;
	}

	if (false == m_csMD5Encrypt.DecryptString(pAgsmLoginEncryptInfo->m_pszEncryptData, pszAccount, ulAccountLen)
		|| false == m_csMD5Encrypt.DecryptString(pAgsmLoginEncryptInfo->m_pszEncryptData, pszPassword, ulPasswordLen)
		)
	{
		printf("Error: Decrypt failed.\n");

		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsmLoginClient::ProcessSignOn - 4\n");
		AuLogFile_s(LOGIN_ERROR_FILE_NAME, strCharBuff);

		return FALSE;
	}

	ZeroMemory(pAgsmLoginEncryptInfo->m_szAccountID, sizeof(pAgsmLoginEncryptInfo->m_szAccountID));
	strncpy_s(pAgsmLoginEncryptInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING, pszAccount, AGPACHARACTER_MAX_ID_STRING);

	//�ش� ID, Password�� DBQuery�� �ɾ��.
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_CHECK_ACCOUNT;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;

	ZeroMemory(pAgsmLoginQueueInfo->m_szAccountID, sizeof(pAgsmLoginQueueInfo->m_szAccountID));
	strncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsmLoginQueueInfo->m_szPassword, sizeof(pAgsmLoginQueueInfo->m_szPassword));
	strncpy(pAgsmLoginQueueInfo->m_szPassword, pszPassword, AGPACHARACTER_MAX_PW_STRING);

	// �Ϻ� ���� ��Ʈ��
#if defined(_AREA_KOREA_) || defined(_AREA_JAPAN_)
	strcpy( pAgsmLoginQueueInfo->m_szHanAuthString, "" );
	if ( (AP_SERVICE_AREA_KOREA == g_eServiceArea || AP_SERVICE_AREA_JAPAN == g_eServiceArea) && pszExtraForForeign != NULL )
	{
		strncpy_s( pAgsmLoginQueueInfo->m_szHanAuthString,
				   sizeof(pAgsmLoginQueueInfo->m_szHanAuthString),
				   pszExtraForForeign,
				   _TRUNCATE );
	}
#elif defined(_AREA_GLOBAL_)
	if( pszExtraForForeign != NULL )
		strncpy_s( pAgsmLoginQueueInfo->m_szGBAuthString, sizeof(pAgsmLoginQueueInfo->m_szGBAuthString), pszExtraForForeign, _TRUNCATE );
#endif
	m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
	}

BOOL AgsmLoginClient::ProcessEKey(CHAR *pszEKey, UINT32 ulNID)
{
	if ( !pszEKey || strlen(pszEKey) > AGPMLOGIN_EKEYSIZE ) return FALSE;
	
		
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;

	//�ش� ekey, nid�� DBQuery�� �ɾ��.
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;

	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_EKEY;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;

	ZeroMemory(pAgsmLoginQueueInfo->m_szAccountID, sizeof(pAgsmLoginQueueInfo->m_szAccountID));
	//strncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccount, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsmLoginQueueInfo->m_szPassword, sizeof(pAgsmLoginQueueInfo->m_szPassword));
	strncpy(pAgsmLoginQueueInfo->m_szPassword, pszEKey, AGPACHARACTER_MAX_PW_STRING);

	m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
}


BOOL AgsmLoginClient::ProcessGetUnion(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		return FALSE;
	
	if (pAgsdServer->m_nNumPlayers > pAgsdServer->m_lMaxUserCount)
		{
		SendLoginResult(AGPMLOGIN_RESULT_GAMESERVER_FULL, NULL, ulNID);
		return FALSE;
		}

	// push to queue
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_UNION;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessGetCharacters(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_GET_CHARACTERS;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	
	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessSelectCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, INT32 lMemberBillingNum)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
	{
		return FALSE;
	}
	
	CHAR *pszChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									NULL,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//race info
									NULL,
									NULL,
									NULL
									#ifndef _AREA_CHINA_
									,NULL				// Jumping Event Character Flag
									#endif
									);	
	
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar || NULL == pAgsmLoginEncryptInfo ||
		AGSMLOGIN_FLOW_SELECT_CHARACTER == pAgsmLoginEncryptInfo->m_lFlow)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
			AuLogConsole_s("[%d] %s - Error (%s, %s, %s)", ulNID, __FUNCTION__, pszServerName, pszDBName, pszAccountID);
		SendLoginResult(AGPMLOGIN_RESULT_GAMESERVER_NOT_READY, NULL, ulNID);
		return FALSE;
		}

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_SELECT_CHARACTER))
		return FALSE;

	SetLoginStep(ulNID, AGPMLOGIN_STEP_SELECT_CHARACTER);

	pAgsmLoginEncryptInfo->m_lFlow = AGSMLOGIN_FLOW_SELECT_CHARACTER;

	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_SELECT_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_lCID = lMemberBillingNum;	// �Ϻ�: ���� ��ȣ, �Ⱦ��� �ʵ�� ��� ���� ����.
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szIPAddress, pAgsdServer->m_szIP, AGPMLOGIN_IPADDRSIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessReturnToSelectWorld(CHAR *pszAccountID, UINT32 ulNID)
	{
	// �� ������ ĳ���� �������� �ʱ�ȭ ��Ų��.
	m_pAgsmAccountManager->RemoveAllCharacters(pszAccountID);

	// reset login step
	SetLoginStep(ulNID, AGPMLOGIN_STEP_PASSWORD_CHECK_OK);

	return TRUE;
	}

BOOL AgsmLoginClient::ProcessBaseCharacterOfRace(PVOID pvCharDetailInfo, UINT32 ulNID)
{
	if (NULL == pvCharDetailInfo || 0 == ulNID)
		return FALSE;
	
	INT32	lRace = AURACE_TYPE_NONE;
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
								NULL,		//TID
								NULL,		//CharName
								NULL,		//MaxRegisterChars
								NULL,		//Slot Index
								NULL,		//Union Info
								&lRace,		//race info
								NULL,
								NULL,
								NULL
								#ifndef _AREA_CHINA_
								,NULL		// Jumping Event Character Flag
								#endif
								);

	if (lRace <= AURACE_TYPE_NONE || lRace >= AURACE_TYPE_MAX)
		return FALSE;

	/*
	// race base character�� ������ �ȵǾ� �ִٸ� �����Ѵ�.
	if (AP_INVALID_CID == m_lRaceBaseCharacter[lRace][0])
	{
		if (!CreateBaseCharacterOfRace(lRace))
			return FALSE;
	}
	*/

	// ��� base race character�� ������ ulNID�� ������.
	for (INT32 i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
	{
		if (m_lRaceBaseCharacter[lRace][i] == AP_INVALID_CID)
			continue;

		AgpdCharacter* pcsCharacter = m_pAgpmCharacter->GetCharacter(m_lRaceBaseCharacter[lRace][i]);
		if(!pcsCharacter)
			continue;

		if (!m_pAgsmCharacter->SendCharacterAllInfo(pcsCharacter, ulNID, FALSE))
			break;
	}	

	return SendBaseCharacterOfRace(lRace, AUCHARCLASS_TYPE_MAX * 3, ulNID);
}

BOOL AgsmLoginClient::ProcessCreateCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
{
	if (NULL == pvCharDetailInfo || NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName)
		return FALSE;
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
	{
		if(g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "AgsdServer Pointer is NULL");
			AuLogFile_s("LOG\\CreateCharacterLog.txt", strCharBuff);
			return FALSE;
		}
		else
			return FALSE;
	}

	CHAR *pszChar = NULL;
	INT32 lTID = 0;
	INT32 lHairIndex	= 0;
	INT32 lFaceIndex	= 0;
#ifndef _AREA_CHINA_
	INT32 lIsJumpingCharacter = 0;		// 0 is FALSE
#endif
	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									&lTID,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//Race Info
									&lHairIndex,
									&lFaceIndex,
									NULL
									#ifndef _AREA_CHINA_
									,&lIsJumpingCharacter	// Jumping Event Character Flag
									#endif
									);
	
	if (NULL == pszChar || 0 == lTID)
	{
		if(g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "Character Name Or Character TID is NULL");
			AuLogFile_s("LOG\\CreateCharacterLog.txt", strCharBuff);
			return FALSE;
		}
		else
			return FALSE;
	}

	ToLowerExceptFirst( pszChar, static_cast<int>( strlen( pszChar ) ) );

	if (FALSE == CheckCharacterName(pszChar))
	{
		return SendLoginResult(AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME, NULL, ulNID);
	}

	AgpdCharacter	*pcsBaseCharacter	= NULL;

	// �α��� �������� ���� ������ ������ üũ
	int i = 0;
	for (i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
	{
		pcsBaseCharacter	= m_pAgpmCharacter->GetCharacter(m_lRaceBaseCharacter[1][i]);
		if (pcsBaseCharacter && pcsBaseCharacter->m_pcsCharacterTemplate->m_lID == lTID)
			break;
	}

	if (i == AUCHARCLASS_TYPE_MAX * 3)
		return FALSE;
	// �α��� �������� ���� ������ ������ üũ

	CHAR	szNewName[AGPDCHARACTER_MAX_ID_LENGTH * 2];
	ZeroMemory(szNewName, sizeof(szNewName));

	m_pAgsmCharacter->AddServerNameToCharName(pszChar, pszServerName, szNewName);

	AgpdCharacter *pAgpdCharacter = m_pAgpmCharacter->GetCharacterLock(szNewName);
	if (pAgpdCharacter)
	{
		AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
		if (m_pAgsmAccountManager->GetAccount(pAgsdCharacter->m_szAccountID))
		{
			SendLoginResult(AGPMLOGIN_RESULT_CHAR_NAME_ALREADY_EXIST, NULL, ulNID);
			return FALSE;
		}
		else
		{
			pAgpdCharacter->m_Mutex.Release();
			m_pAgpmCharacter->RemoveCharacter(szNewName);
		}
	}

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_CREATE_CHARACTER))
	{
		if(g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "LoginStep is not Valid");
			AuLogFile_s("LOG\\CreateCharacterLog.txt", strCharBuff);
			return FALSE;
		}
		else
			return FALSE;
	}

	// create character
	pAgpdCharacter = m_pAgsmCharManager->CreateCharacter(pszAccountID, szNewName, lTID, ulNID, TRUE, pszServerName);
	if (NULL == pAgpdCharacter)
	{
		if(g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		{
			char strCharBuff[256] = { 0, };
			sprintf_s(strCharBuff, sizeof(strCharBuff), "CreateCharacter() is Fail");
			AuLogFile_s("LOG\\CreateCharacterLog.txt", strCharBuff);
			return FALSE;
		}
		else
			return FALSE;
	}

#ifndef _AREA_CHINA_
	if (lIsJumpingCharacter)
	{
		AgsdServer *pThisServer = m_pAgsmServerManager->GetThisServer();
		if (!pThisServer)
			return FALSE;

		if (pThisServer->m_lJumpingStartLevel > 1)
		{
			m_pAgsmCharacter->SetCharacterLevel(pAgpdCharacter, pThisServer->m_lJumpingStartLevel, FALSE);
			m_pAgpmFactors->CopyFactor(&pAgpdCharacter->m_csFactor, (AgpdFactor *) m_pAgpmFactors->GetFactor(&pAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);
		}
		if (pThisServer->m_llJumpingStartGheld > 0)
		{
			m_pAgpmCharacter->SetMoney(pAgpdCharacter, pThisServer->m_llJumpingStartGheld);
			m_pAgpmCharacter->UpdateMoney(pAgpdCharacter);
		}
		if (pThisServer->m_lJumpingStartCharisma > 0)
		{
			m_pAgpmCharacter->UpdateCharismaPoint(pAgpdCharacter, pThisServer->m_lJumpingStartCharisma);
		}
		// give default item for JumpingCharacter
		if (m_pAgsmItemManager->GiveDefaultItem(pAgpdCharacter, 1, TRUE) == FALSE)
		{
			return FALSE;
		}
	}
	else
	{
#endif
		if (pAgsdServer->m_lStartLevel > 1)
		{
			m_pAgsmCharacter->SetCharacterLevel(pAgpdCharacter, pAgsdServer->m_lStartLevel, FALSE);
			m_pAgpmFactors->CopyFactor(&pAgpdCharacter->m_csFactor, (AgpdFactor *) m_pAgpmFactors->GetFactor(&pAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);
		}
		if (pAgsdServer->m_llStartGheld > 0)
		{
			m_pAgpmCharacter->SetMoney(pAgpdCharacter, pAgsdServer->m_llStartGheld);
			m_pAgpmCharacter->UpdateMoney(pAgpdCharacter);
		}
		if (pAgsdServer->m_lStartCharisma > 0)
		{
			m_pAgpmCharacter->UpdateCharismaPoint(pAgpdCharacter, pAgsdServer->m_lStartCharisma);
		}
#ifndef _AREA_CHINA_
		// give default item 
		if (m_pAgsmItemManager->GiveDefaultItem(pAgpdCharacter, 1) == FALSE)
		{
			return FALSE;
		}
	}
#endif

	INT32	lRace	= m_pAgpmFactors->GetRace(&pcsBaseCharacter->m_csFactor);
	INT32	lClass	= m_pAgpmFactors->GetClass(&pcsBaseCharacter->m_csFactor);

	if (m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_HAIR, lHairIndex) == CHARCUSTOMIZE_CASE_ALL ||
		m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_HAIR, lHairIndex) == CHARCUSTOMIZE_CHAR_LOGIN)
		pAgpdCharacter->m_lHairIndex	= lHairIndex;
	
	if (m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_FACE, lFaceIndex) == CHARCUSTOMIZE_CASE_ALL ||
		m_pAgpmEventCharCustomize->GetCustomizeCase(lRace, lClass, CHARCUSTOMIZE_TYPE_FACE, lFaceIndex) == CHARCUSTOMIZE_CHAR_LOGIN)
		pAgpdCharacter->m_lFaceIndex	= lFaceIndex;

	SetLoginStep(ulNID, AGPMLOGIN_STEP_CREATE_CHARACTER);

	AgsdCharacter *pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	ZeroMemory(pAgsdCharacter->m_szAccountID, sizeof(pAgsdCharacter->m_szAccountID));
	_tcsncpy(pAgsdCharacter->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	ZeroMemory(pAgsdCharacter->m_szServerName, sizeof(pAgsdCharacter->m_szServerName));
	_tcsncpy(pAgsdCharacter->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	// push to queue
	AgsmLoginQueueInfo	*pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_CREATE_CHARACTER;
	pAgsmLoginQueueInfo->m_lCID = pAgpdCharacter->m_lID;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
#ifndef _AREA_CHINA_
	pAgsmLoginQueueInfo->m_lIsJumpingCharacter = lIsJumpingCharacter;
#endif
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	std::cout << "AGSMLOGIN_OPERATION_CREATE_CHARACTER :" 
		<< '['<< pAgsmLoginQueueInfo->m_szAccountID   << ']' 
		<< '['<< pAgsmLoginQueueInfo->m_lCID   << ']' 
		<< '[' << pAgsmLoginQueueInfo->m_szServerName << ']' 
		<< '['<< pAgsmLoginQueueInfo->m_ulGameServerNID   << ']' 
		<< '[' << pAgsmLoginQueueInfo->m_szDBName <<']' 
#ifndef _AREA_CHINA_
		<< '[' << pAgsmLoginQueueInfo->m_lIsJumpingCharacter << ']' 
#endif
		<< std::endl;
	//pAgpdCharacter->m_Mutex.Release();

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID ) pAgsmLoginQueueInfo, __FILE__, __LINE__);
}

#ifndef _AREA_CHINA_
BOOL AgsmLoginClient::ProcessCreateJumpingCharacterCheck(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, UINT32 ulNID)
{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName )
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		return FALSE;

	// push to queue
	AgsmLoginQueueInfo	*pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_CREATE_JUMPING_CHARACTER_CHECK;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID ) pAgsmLoginQueueInfo, __FILE__, __LINE__);
}
#endif

BOOL AgsmLoginClient::ProcessRemoveCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName  || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;

	CHAR *pszChar = NULL;

	m_pAgpmLogin->m_csPacketCharInfo.GetField( FALSE, pvCharDetailInfo, 0, 
									NULL,				//TID
									&pszChar,			//CharName
									NULL,				//MaxRegisterChars
									NULL,				//Slot Index
									NULL,				//Union Info
									NULL,				//Race Info
									NULL,
									NULL,
									NULL
									#ifndef _AREA_CHINA_
									,NULL				// Jumping Event Character Flag
									#endif
									);

	if (NULL == pszChar)
		return FALSE;

	if (FALSE == IsValidLoginStep(ulNID, AGPMLOGIN_STEP_REMOVE_CHARACTER))
		return FALSE;

	//m_pAgsmAccountManager->SetLoginStep(pszAccountID, AGPMLOGIN_STEP_REMOVE_CHARACTER);
	SetLoginStep( ulNID, AGPMLOGIN_STEP_REMOVE_CHARACTER);

	// push to queue
	AgsmLoginQueueInfo	*pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_REMOVE_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessRenameCharacter(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;
	
	CHAR *pszChar = NULL;
	CHAR *pszNewChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
								NULL,				// TID
								&pszChar,			// character id
								NULL,				// MaxRegisterChars
								NULL,				// Slot Index
								NULL,				// Union Info
								NULL,				// race info
								NULL,
								NULL,
								&pszNewChar
								#ifndef _AREA_CHINA_
								,NULL				// Jumping Event Character Flag
								#endif
								);	

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar
		|| NULL == pAgsmLoginEncryptInfo
		)
		return FALSE;

	if (FALSE == CheckCharacterName(pszNewChar))
		{
		return SendLoginResult(AGPMLOGIN_RESULT_UNMAKABLE_CHAR_NAME, NULL, ulNID);
		}
	
	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	if (!IsValidLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER))
		return FALSE;

	SetLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER);

	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_RENAME_CHARACTER;
	pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pszAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pszChar, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pszServerName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szDBName, pszDBName, AGPMLOGIN_SERVERGROUPNAMESIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szIPAddress, pAgsdServer->m_szIP, AGPMLOGIN_IPADDRSIZE);
	_tcsncpy(pAgsmLoginQueueInfo->m_szNewCharacterID, pszNewChar, AGPACHARACTER_MAX_ID_STRING);

	return m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
	}


BOOL AgsmLoginClient::ProcessCharacter4Compensation(CHAR *pszAccountID, CHAR *pszServerName, CHAR *pszDBName, PVOID pvCharDetailInfo, UINT32 ulNID, BOOL bSelect)
	{
	if (NULL == pszAccountID || NULL == pszServerName || NULL == pszDBName || NULL == pvCharDetailInfo)
		return FALSE;
	
	CHAR *pszChar = NULL;
	m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0,
								NULL,				// TID
								&pszChar,			// character id
								NULL,				// MaxRegisterChars
								NULL,				// Slot Index
								NULL,				// Union Info
								NULL,				// race info
								NULL,
								NULL,
								NULL
								#ifndef _AREA_CHINA_
								,NULL				// Jumping Event Character Flag
								#endif
								);	

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pszChar
		|| NULL == pAgsmLoginEncryptInfo
		)
		return FALSE;

	AgsdServer *pAgsdServer = m_pAgsmServerManager->GetGameServerOfWorld(pszServerName);
	if (NULL == pAgsdServer)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	if (!IsValidLoginStep(ulNID, AGPMLOGIN_STEP_GET_EXIST_CHARACTER))
		return FALSE;
	
	if (!bSelect)
		ZeroMemory(pAgsmLoginEncryptInfo->m_szChar4Compen, sizeof(pAgsmLoginEncryptInfo->m_szChar4Compen));
	else
		{
		if (0 != pAgsmLoginEncryptInfo->m_lCompenID)
			_tcscpy(pAgsmLoginEncryptInfo->m_szChar4Compen, pszChar);
		}

	return TRUE;
	}




//	Callbacks
//=================================================
//
BOOL AgsmLoginClient::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass)
		return FALSE;
	
	AgsmLoginClient *pThis = (AgsmLoginClient *) pClass;
	return pThis->OnDisconnect(0, PtrToUint(pCustData));
	}


BOOL AgsmLoginClient::OnDisconnect(INT32 lCID, UINT32 ulNID)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;
	
	if (NULL != pAgsmLoginEncryptInfo->m_szAccountID
		|| _T('\0') != pAgsmLoginEncryptInfo->m_szAccountID[0])
		{
		// reset login status
		AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;
		pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_RESET_LOGINSATUS_BY_DISCONNECT_FROM_LOGINSERVER;
		pAgsmLoginQueueInfo->m_ulCharNID = ulNID;
		strncpy(pAgsmLoginQueueInfo->m_szAccountID, pAgsmLoginEncryptInfo->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	
		m_pAgsmLoginServer->CheckAndPushToQueue((PVOID) pAgsmLoginQueueInfo, __FILE__, __LINE__);
		
		/*
		if (m_pAgsmGK->GetLKServer() &&
			pAgsmLoginEncryptInfo->m_bIsPCBangIP)
			{
			PlayerData	*pPlayerData	= GetPlayerDataForID(ulNID);
			if (pPlayerData && pPlayerData->lDisconnectReason != DISCONNNECT_REASON_CLIENT_NORMAL)
				{
				// LK Server �� Logout �޽����� ������.
				// (pc�� �̺�Ʈ�� ���� �Ŵ�. �̺�Ʈ�� ������ ���ڵ�� ����.)
				m_pAgsmGK->SendPacketLogout(pAgsmLoginQueueInfo->m_szAccountID, ulNID);
				}
			}
		*/
		}

	// destroy AgsmLoginEncryptInfo
	delete pAgsmLoginEncryptInfo;
	m_csCertificatedNID.RemoveObject(ulNID);

	return TRUE;
	}


BOOL AgsmLoginClient::CBReceiveNewCID(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pData || NULL == pClass || NULL == pCustData)
		return FALSE;

	AgsmLoginClient	*pThis			= (AgsmLoginClient *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *) pData;
	PVOID			*ppvBuffer		= (PVOID *) pCustData;
	INT32			lNewCID			= PtrToInt(ppvBuffer[0]);
	UINT32			ulNID			= PtrToUint(ppvBuffer[1]);

	pThis->m_pAgsmCharacter->ResetWaitOperation(pAgpdCharacter, AGSMCHARACTER_WAIT_OPERATION_REQUEST_NEW_CID);
	pThis->m_pAgsmCharacter->SetWaitOperation(pAgpdCharacter, AGSMCHARACTER_WAIT_OPERATION_SEND_CHAR_INFO);

	AgsdCharacter	*pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	AgsdServer		*pAgsdServer = pThis->m_pAgsmServerManager->GetServer(pAgsdCharacter->m_ulServerID);

	if (!pAgsdServer)
		return FALSE;

	INT32	lOldCID = pAgpdCharacter->m_lID;
	pAgpdCharacter->m_lID = lNewCID;
	pAgsdCharacter->m_lOldCID = lOldCID;

	//���������� ĳ���͸� �о�����~ ������ �α����� ĳ���͸� �����ش�.
	pThis->m_pAgsmCharacter->SendCharacterAllInfo(pAgpdCharacter, pAgsdServer->m_dpnidServer,
													TRUE, FALSE, FALSE, lOldCID);

	return TRUE;
	}


BOOL AgsmLoginClient::CBCompleteSendCharacterInfo(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass || NULL == pData)
		return FALSE;

	AgsmLoginClient *pThis			= (AgsmLoginClient *) pClass;
	AgpdCharacter	*pAgpdCharacter	= (AgpdCharacter *)	pData;
	AgsdCharacter	*pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	AgsdServer		*pAgsdServer = pThis->m_pAgsmServerManager->GetServer(pAgsdCharacter->m_ulServerID);
	if (NULL == pAgsdCharacter || NULL == pAgsdServer)
		return FALSE;

	pThis->SendEnterGameEnd(pAgsdCharacter->m_szAccountID,
							pAgpdCharacter->m_lID,
							pAgpdCharacter->m_lTID1,
							pThis->m_pAgsmCharacter->GetRealCharName(pAgpdCharacter->m_szID),
							pAgsdServer->m_szIP,
							pAgsdCharacter->m_dpnidCharacter
							);

	// push to queue
	AgsmLoginQueueInfo *pAgsmLoginQueueInfo = new AgsmLoginQueueInfo;

	pAgsmLoginQueueInfo->m_nOperation = AGSMLOGIN_OPERATION_SET_LOGINSATUS_IN_GAMESERVER;
	_tcsncpy(pAgsmLoginQueueInfo->m_szAccountID, pAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	_tcsncpy(pAgsmLoginQueueInfo->m_szCharacterID, pAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);	
	//_tcsncpy(pAgsmLoginQueueInfo->m_szServerName, pAgsdServer->m_szGroup, AGPACHARACTER_MAX_ID_STRING);
	pAgsmLoginQueueInfo->m_lCID = pAgpdCharacter->m_lID;
	pAgsmLoginQueueInfo->m_lTID = pAgpdCharacter->m_lTID1;
	pAgsmLoginQueueInfo->m_ulCharNID = pAgsdCharacter->m_dpnidCharacter;
	pAgsmLoginQueueInfo->m_ulGameServerNID = pAgsdServer->m_dpnidServer;

	pThis->m_pAgsmLoginServer->CheckAndPushToQueue((PVOID)pAgsmLoginQueueInfo, __FILE__, __LINE__);

	return TRUE;
	}




//	Packet send
//===========================================
//
BOOL AgsmLoginClient::SendEncryptCode(CHAR *pszEncryptCode, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_ENCRYPT_CODE;
	INT16	nPacketLength = 0;

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													pszEncryptCode,		// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,				// gamestring
													NULL,				// challenge number for ekey
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendInvalidClientVersion(UINT32 ulNID)
	{
	if (0 == ulNID)
		return FALSE;

	INT8	cOperation		= AGPMLOGIN_INVALID_CLIENT_VERSION;
	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}

BOOL AgsmLoginClient::SendSignOnSuccess(UINT32 ulNID, CHAR* pszAccountID, int isLimited, int isProtected)	//SendLoginSucceeded(UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_SIGN_ON;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,				// gamestring for Japan
													NULL,				// challenge number for ekey(china)
													&isLimited,			// for china
													&isProtected		// for china
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendUnionInfo(INT32 lUnionType, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_UNION_INFO;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														NULL,			//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														&lUnionType,	//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterName(CHAR *pszCharName, INT32 lMaxChars, INT32 lIndex, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_CHARACTER_NAME;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														&lMaxChars,		//MaxRegisterChars
														&lIndex,		//Slot Index
														NULL,			//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterNameFinish(CHAR *pszAccountID, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
	
	INT8	cOperation = AGPMLOGIN_CHARACTER_NAME_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Name Finished\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterInfoFinish(UINT32 lCID, CHAR *pszAccountID, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
		
	INT8	cOperation = AGPMLOGIN_CHARACTER_INFO_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Info Finished\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendBaseCharacterOfRace(INT8 cRace, INT32 lNumChar, UINT32 ulNID)
	{
	if (0 == ulNID)
		return FALSE;

	INT8 cOperation = AGPMLOGIN_RACE_BASE;
	INT16 nPacketLength;
	INT32 lRace = (INT32) cRace;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														NULL,			//CharName
														&lNumChar,		//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														&lRace,			//race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	if (pvDetailCharInfo)
		m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendNewCharacterName(CHAR *pszCharName, INT32 lCharCount, INT32 lSlotIndex, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_NEW_CHARACTER_NAME;
	INT16	nPacketLength;
	PVOID	pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket(FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														&lCharCount,	//MaxRegisterChars
														&lSlotIndex,	//Slot Index
														NULL,			//Union Info
														NULL,			//race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendNewCharacterInfoFinish(INT32 lCID, UINT32 ulNID)
	{
	if (AP_INVALID_CID == lCID)
		{
		DestroyClient(ulNID);
		return FALSE;
		}

	INT8	cOperation = AGPMLOGIN_NEW_CHARACTER_INFO_FINISH;
	INT16	nPacketLength;
	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,				// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCharacterRemoved(CHAR *pszAccountID, CHAR *pszCharName, UINT32 ulNID)
	{
	if (NULL == pszAccountID)
		return FALSE;
	
	INT8 cOperation = AGPMLOGIN_REMOVE_CHARACTER;
	INT16 nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,
														pszCharName,	//CharName
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccountID,		// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Character Removed\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}

BOOL AgsmLoginClient::SendEKeyChallengeNum(const char* pszChallengeNum, UINT32 ulNID)
{
	INT cOperation = AGPMLOGIN_EKEY;
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													NULL,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,			// lResult
													NULL,				// version info
													NULL,				// ekey
													pszChallengeNum,	// challenge number for ekey
													NULL,				// isLimited
													NULL				// isProtected
													);
	BOOL bResult = FALSE;
	if (pvPacket)
	{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID);
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
	}

	return bResult;
}

BOOL AgsmLoginClient::SendLoginResult(INT32 lResult, CHAR *pszCharName, UINT32 ulNID)
	{
	INT8 cOperation = AGPMLOGIN_LOGIN_RESULT;
	INT16 nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														NULL,			//TID
														pszCharName,	//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														NULL,			//race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID	pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													&lResult,			// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);
	if (pvDetailCharInfo)
		m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);
	
	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendEnterGameEnd(CHAR *pszAccountID, INT32 lCID, INT32 lTID, CHAR *pszCharName, CHAR *pszIPAddress, UINT32 ulNID)
	{
	INT8	cOperation = AGPMLOGIN_ENTER_GAME;
	INT16	nPacketLength;
	PVOID pvDetailCharInfo = m_pAgpmLogin->m_csPacketCharInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														&lTID,			//TID
														pszCharName,	//CharName
														NULL,			//MaxRegisterChars
														NULL,			//Slot Index
														NULL,			//UnionInfo
														NULL,			// race info
														NULL,
														NULL,
														NULL
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);

	PVOID pvDetailServerInfo = m_pAgpmLogin->m_csPacketServerInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													pszIPAddress		// IPAddress
													);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													NULL,				// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													&lCID,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvDetailCharInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													pvDetailServerInfo, // pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCharInfo.FreePacket(pvDetailCharInfo);
	m_pAgpmLogin->m_csPacketServerInfo.FreePacket(pvDetailServerInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		printf("SendLoginComplete\n");
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}


BOOL AgsmLoginClient::SendCompensationExist(CHAR *pszAccount, INT32 lCompenID, INT32 lCompenType, CHAR *pszCharID, CHAR *pszItemDetails, CHAR *pszDescription, UINT32 ulNID)
	{
	if (NULL == pszAccount)
		return FALSE;
	
	INT16 nDescriptionLength = pszDescription ? (INT16) strlen(pszDescription) : 0;
	
	INT8 cOperation = AGPMLOGIN_COMPENSATION_INFO;
	INT16 nPacketLength;
	PVOID pvCompenInfo = m_pAgpmLogin->m_csPacketCompenInfo.MakePacket( FALSE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
														&lCompenID,
														&lCompenType,
														pszCharID,
														pszItemDetails,
														pszDescription,
														&nDescriptionLength
														);

	PVOID pvPacket = m_pAgpmLogin->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMLOGIN_PACKET_TYPE,
													&cOperation,		// cOperation
													NULL,				// EncryptCode
													pszAccount,			// AccountID
													NULL,				// AccountID Len
													NULL,				// AccountPassword
													NULL,				// AccountPassword Len
													NULL,				// lCID
													NULL,				// pvServerGroupName;���� ������ �ּҸ� �־�� ��Ŷ
													pvCompenInfo,	// pvCharDetailInfo; ĳ�������� �󼼵����͸� �ִ� ��Ŷ
													NULL,				// pvDetailServerInfo; ������ ����.
													NULL,				// lResult
													NULL,				// version info
													NULL,
													NULL,
													NULL,				// isLimited
													NULL				// isProtected
													);

	m_pAgpmLogin->m_csPacketCompenInfo.FreePacket(pvCompenInfo);

	BOOL bResult = FALSE;
	if (NULL != pvPacket)
		{
		_tprintf(_T("Send Compensation Info\n"));
		bResult = SendPacket(pvPacket, nPacketLength, ulNID );
		m_pAgpmLogin->m_csPacket.FreePacket(pvPacket);
		}

	return bResult;
	}




//	ETC
//================================================
//
BOOL AgsmLoginClient::AddInvalidPasswordLog(UINT32 ulNID)
	{
	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL != pAgsmLoginEncryptInfo)
		{
		pAgsmLoginEncryptInfo->m_lInvalidPasswordCount++;
		if (pAgsmLoginEncryptInfo->m_lInvalidPasswordCount < 3)
			return TRUE;
		}
	
	return FALSE;
	}


BOOL AgsmLoginClient::CheckCharacterName(CHAR *pszCharName)
	{
	const int tempSize = 128;
	std::vector<CHAR> tempName( tempSize );
	ConvertToLower( pszCharName, &tempName[0], tempSize );
	
	INT32 lLen = (INT32)_tcslen(&tempName[0]);
	if (lLen < 4 || lLen > 16)
		return FALSE;

	if (FALSE == this->m_pAgpmCharacter->CheckCharName( &tempName[0] ))
		return FALSE;

	// �弳���� Ȯ���Ѵ�. 2005.07.01. steeple
	if (FALSE == m_pAgpmCharacter->CheckFilterText(&tempName[0]))
		return FALSE;

	// �����ܾ ���ڿ��� ���ԵǾ� �ִ��� Ȯ���Ѵ�.
	if (TRUE == SearchContainBanWord(&tempName[0]))
		return FALSE;

	//ĳ���� ���ø��� �ִ� �̸����� Ȯ���Ѵ�.
	AgpdCharacterTemplate *pcsTemplate = m_pAgpmCharacter->GetCharacterTemplate( pszCharName );
	if (pcsTemplate)
		return FALSE;

	// ĳ�� ������ ���� ����� ���� ĳ���͵�� �̸��� ������ ����.
#ifndef _AREA_CHINA_
	for (int i = 0; i < AURACE_TYPE_MAX; ++i)
	{
		for(int j=0; j < AUCHARCLASS_TYPE_MAX; ++j)
		{
			if (g_pszNormalCharName[i][j] && strcmp(g_pszNormalCharName[i][j], pszCharName) == 0)
				return FALSE;
		}
	}
#else
	for (int i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
	{
		if (g_pszCharName[i] && strcmp(g_pszCharName[i], pszCharName) == 0)
			return FALSE;
	}
#endif

	return TRUE;
	}


BOOL AgsmLoginClient::SearchContainBanWord(CHAR* pszCharName)
{
	string szTemp(pszCharName);

	for ( vector<string>::iterator iter = m_vstrBanWord.begin(); iter != m_vstrBanWord.end(); iter++)
	{
		if (string::npos == szTemp.find( (*iter).c_str() ) )
			continue;

		return TRUE;
	}

	return FALSE;
}


BOOL AgsmLoginClient::CreateBaseCharacterOfRace(INT8 cRace)
	{
	if (cRace <= AURACE_TYPE_NONE || cRace >= AURACE_TYPE_MAX)
		return FALSE;

	for (INT32 i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		if (g_lCharTID[i] == AP_INVALID_CID)
			continue;

		/*
		if (pcsCharacterTemplate->m_lID != 1 &&
			pcsCharacterTemplate->m_lID != 5)
			continue;
		*/

		AgpdCharacter *pcsCharacter = m_pAgsmCharManager->CreateCharacter(NULL, g_pszCharName[i], g_lCharTID[i], 0, FALSE);
		if (pcsCharacter)
			{
			// ���⼭ Ŭ������ �������� �����Ѵ�.
			pcsCharacter->m_stPos	= m_RaceBaseCharacterPos[cRace][i].stPos;
			pcsCharacter->m_fTurnX	= m_RaceBaseCharacterPos[cRace][i].fDegreeX;
			pcsCharacter->m_fTurnY	= m_RaceBaseCharacterPos[cRace][i].fDegreeY;

			//�α��ο��� �׳� ���̱⸸ �ϸ� �Ǳ⶧���� �������̼� üũ�� ���Ѵ�.
			if (!m_pAgsmCharManager->EnterGameWorld(pcsCharacter, FALSE))
				{
				m_pAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
				return FALSE;
				}
			}
		else
			{
			pcsCharacter = m_pAgpmCharacter->GetCharacter(g_pszCharName[i]);
			if (!pcsCharacter)
				return FALSE;
			}

		m_pAgsmCharacter->ReCalcCharacterFactors(pcsCharacter, FALSE);
		m_lRaceBaseCharacter[cRace][i] = pcsCharacter->m_lID;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsValidLoginStep(CHAR *pszAccountID, AgpdLoginStep eLoginStep)
	{
	AgpdLoginStep eCurrentStep = m_pAgsmAccountManager->GetLoginStep(pszAccountID);

	if (eCurrentStep <= AGPMLOGIN_STEP_NONE ||
		eCurrentStep >= AGPMLOGIN_STEP_UNKNOWN)
		return FALSE;

	switch (eLoginStep)
		{
		case AGPMLOGIN_STEP_CREATE_CHARACTER:
			{
			if ((eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				&& (eCurrentStep != AGPMLOGIN_STEP_PASSWORD_CHECK_OK)
				)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_SELECT_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_REMOVE_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsValidLoginStep( UINT32 ulNID, AgpdLoginStep eLoginStep)
	{
	AgpdLoginStep eCurrentStep = GetLoginStep(ulNID);

	if (eCurrentStep <= AGPMLOGIN_STEP_NONE ||
		eCurrentStep >= AGPMLOGIN_STEP_UNKNOWN)
		return FALSE;

	switch (eLoginStep)
		{
		case AGPMLOGIN_STEP_CREATE_CHARACTER:
			{
			if ((eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				&& (eCurrentStep != AGPMLOGIN_STEP_PASSWORD_CHECK_OK)
				)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_SELECT_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;

		case AGPMLOGIN_STEP_REMOVE_CHARACTER:
			{
			if (eCurrentStep != AGPMLOGIN_STEP_GET_EXIST_CHARACTER)
				return FALSE;
			}
			break;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::SetAccountAge(UINT32 ulNID, CHAR* szAccountID, INT32 lAge)
{
	if(!szAccountID)
		return FALSE;

	AgsmLoginEncryptInfo** ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo**)m_csCertificatedNID.GetObject(ulNID);
	if(ppAgsmLoginEncryptInfo && *ppAgsmLoginEncryptInfo)
	{
		if(_tcscmp((*ppAgsmLoginEncryptInfo)->m_szAccountID, szAccountID) == 0)
		{
			(*ppAgsmLoginEncryptInfo)->m_lAge = lAge;
		}
	}

	return TRUE;
}

INT32 AgsmLoginClient::GetAccountAge(UINT32 ulNID, CHAR* szAccountID)
{
	if(!szAccountID)
		return FALSE;

	AgsmLoginEncryptInfo** ppAgsmLoginEncryptInfo = (AgsmLoginEncryptInfo**)m_csCertificatedNID.GetObject(ulNID);
	if(ppAgsmLoginEncryptInfo && *ppAgsmLoginEncryptInfo)
	{
		if(_tcscmp((*ppAgsmLoginEncryptInfo)->m_szAccountID, szAccountID) == 0)
		{
			return (*ppAgsmLoginEncryptInfo)->m_lAge;
		}
	}

	return 0;
}



//	Max user
//============================================
//
INT32 AgsmLoginClient::GetMaxProcessUserCount()
	{
	return m_lMaxProcessUserCount;
	}


BOOL AgsmLoginClient::SetMaxProcessUserCount(INT32 lMaxProcessUserCount)
	{
	if (lMaxProcessUserCount < 0)
		return FALSE;

	m_lMaxProcessUserCount	= lMaxProcessUserCount;

	return TRUE;
	}


INT32 AgsmLoginClient::GetMaxCreateProcessUserCount()
	{
	return m_lMaxProcessUserCount;
	}


BOOL AgsmLoginClient::SetMaxCreateProcessUserCount(INT32 lMaxCreateProcessUserCount)
	{
	if (lMaxCreateProcessUserCount < 0)
		return FALSE;

	m_lMaxCreateProcessUserCount	= lMaxCreateProcessUserCount;

	return TRUE;
	}




//	Stream
//==========================================
//
BOOL AgsmLoginClient::LoadCharNameFromExcel(CHAR *pszFile)
	{
	AuExcelTxtLib		csAuExcelTxtLib;

	//Excel���Ͽ��� ������ ���.
	if (FALSE == csAuExcelTxtLib.OpenExcelFile( pszFile, TRUE ))
	{
		_tprintf(_T("%s Excel������ ���� ���߽��ϴ�.\n"), pszFile);
		return FALSE;
	}

	for (INT32 iRow = 0; iRow < csAuExcelTxtLib.GetRow(); iRow++ )
	{
		for (INT32 iColumn = 0; iColumn < csAuExcelTxtLib.GetColumn(); iColumn++ )
		{
			CHAR* szBuffer = (CHAR*)csAuExcelTxtLib.GetData( iColumn, iRow );
			if(!szBuffer)
				continue;

			const string szData = szBuffer;
			m_vstrBanWord.push_back(szData);
		}
	}

	csAuExcelTxtLib.CloseFile();

	return TRUE;
	}


BOOL AgsmLoginClient::StreamReadBaseCharPos(CHAR *pszFile)
	{
	if (NULL == pszFile || _T('\0') == *pszFile)
		return FALSE;

	ApModuleStream			csStream;
	UINT16					nNumKeys;

	csStream.SetMode(APMODULE_STREAM_MODE_NAME_OVERWRITE);
	if (!csStream.Open(pszFile))
		return FALSE;

	nNumKeys = csStream.GetNumSections();

	for (INT32 i = 0; i < nNumKeys; i++)
		{
		csStream.ReadSectionName(i);

		// Stream Enumerate�� �Ѵ�. (ApModuleStream���� Ini File�� �����鼭 ������ Module�� Callback���ش�.
		if (!csStream.EnumReadCallback(AGSMLOGIN_DATA_TYPE_BASE_CHAR_POS, NULL, this))
			return FALSE;
		}

	return TRUE;
	}


BOOL AgsmLoginClient::BaseCharacterPosReadCB(PVOID pData, ApModule *pClass, ApModuleStream *pStream)
	{
	if (!pClass || !pStream)
		return FALSE;

	AgsmLoginClient	*pThis	= (AgsmLoginClient *)	pClass;

	const CHAR		*pszValueName;
	CHAR			szValue[128];
	ZeroMemory		(szValue, sizeof(szValue));

	if (!pStream->ReadNextValue())
		return FALSE;

	pszValueName = pStream->GetValueName();
	if (0 != _tcscmp(pszValueName, AGSMLOGIN_INI_RACE_NAME))
		return FALSE;

	pStream->GetValue(szValue, AGPACHARACTERT_MAX_TEMPLATE_NAME);

	INT32	lRace = pThis->m_pAgpmFactors->FindCharacterRaceIndex(szValue);
	if (lRace <= AURACE_TYPE_NONE || lRace >= AURACE_TYPE_MAX)
		return FALSE;

	INT32 i = 0;
	for (i = 0; i < AUCHARCLASS_TYPE_MAX * 3; ++i)
		{
		if (pThis->m_RaceBaseCharacterPos[lRace][i].stPos.x == 0 &&
			pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeX == 0)
			break;
		}

	if (i == AUCHARCLASS_TYPE_MAX * 3)
		return FALSE;

	while (pStream->ReadNextValue())
		{
		pszValueName = pStream->GetValueName();

		if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_POSITION))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].stPos);
			}
		else if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_DEGREEX))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeX);
			}
		else if (0 == _tcscmp(pszValueName, AGSMLOGIN_INI_DEGREEY))
			{
			pStream->GetValue(&pThis->m_RaceBaseCharacterPos[lRace][i].fDegreeY);
			}
		}

	return TRUE;
	}


BOOL AgsmLoginClient::IsReturnToLogin(UINT32 ulNID)
{
	if(!ulNID)
		return FALSE;

	AgsmLoginEncryptInfo *pAgsmLoginEncryptInfo = GetCertificatedNID(ulNID);
	if (NULL == pAgsmLoginEncryptInfo)
		return FALSE;

	return pAgsmLoginEncryptInfo->m_bReturnToLogin;
}

#ifndef _AREA_CHINA_
BOOL AgsmLoginClient::SetBaseCharacterInfo(INT32 lRace, INT32 lClass, INT32 lTID, CHAR *szCharName)
{
	if(NULL == szCharName)
		return FALSE;

	if((lRace > AURACE_TYPE_MAX || lRace <= AURACE_TYPE_NONE) || (lClass > AUCHARCLASS_TYPE_MAX || lClass <= AUCHARCLASS_TYPE_NONE))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////

	g_pszNormalCharName[lRace][lClass]	= szCharName;
	g_lNormalCharTID[lRace][lClass]		= lTID;
	
	return TRUE;
}

INT32 AgsmLoginClient::GetBaseCharacterTID(INT32 lRace, INT32 lClass)
{
	if((lRace > AURACE_TYPE_MAX || lRace <= AURACE_TYPE_NONE) || (lClass > AUCHARCLASS_TYPE_MAX || lClass <= AUCHARCLASS_TYPE_NONE))
		return FALSE;

	return g_lNormalCharTID[lRace][lClass];
}
#endif