/*============================================================================

	AgsmLogin.cpp
	
		Game server side Login module

============================================================================*/


#include "AgsmLogin.h"


/****************************************************/
/*		The Implementation of AgsmLogin class		*/
/****************************************************/
//
AgsmLogin::AgsmLogin()
	{
	SetModuleName(_T("AgsmLogin"));
	SetModuleType(APMODULE_TYPE_SERVER);
	SetPacketType(AGSMLOGIN_PACKET_TYPE);
	}


AgsmLogin::~AgsmLogin()
	{
	}




//	ApModule inherited
//==========================================
//
BOOL AgsmLogin::OnAddModule()
	{
	m_pAgpmLogin = (AgpmLogin *) GetModule(_T("AgpmLogin"));
	m_pAgpmCharacter = (AgpmCharacter *) GetModule(_T("AgpmCharacter"));
	m_pAgsmCharacter = (AgsmCharacter *) GetModule(_T("AgsmCharacter"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmCharManager = (AgsmCharManager *) GetModule(_T("AgsmCharManager"));
	m_pAgsmAccountManager	= (AgsmAccountManager *) GetModule(_T("AgsmAccountManager"));

	if (!m_pAgpmLogin || !m_pAgpmCharacter || !m_pAgsmCharacter || !m_pAgsmServerManager
		|| !m_pAgsmCharManager || !m_pAgsmAccountManager
		)
		return FALSE;

	m_pAgsmCharacter->SetCallbackDisconnectFromGameServer(CBDisconnectFromGameServer, this);

	return TRUE;
	}


BOOL AgsmLogin::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
	{
	BOOL	bResult = FALSE;

	INT8	cOperation = 0;
	TCHAR*	pszAccountID = NULL;
	INT32	lCID = AP_INVALID_CID;
	PVOID	pvCharDetailInfo = NULL;
	INT32	lResult;

	m_pAgpmLogin->m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						NULL,
						&pszAccountID,
						NULL,
						NULL,
						NULL,
						&lCID,
						NULL,
						&pvCharDetailInfo,
						NULL,
						&lResult,
						NULL,
						NULL,
						NULL,
						NULL,
						NULL);

	if (pstCheckArg->bReceivedFromServer)		// from server
		{
		if (AGPMLOGIN_REMOVE_DUPLICATED_ACCOUNT == cOperation)
			{
			// get account
			AgsdAccount *pAgsdAccount = m_pAgsmAccountManager->GetAccount(pszAccountID);
			if (pAgsdAccount)
				{
				INT32			lIndex;
				AgpdCharacter	*pAgpdCharacter;
				AgsdCharacter	*pAgsdCharacter;

				for (lIndex = 0; lIndex < pAgsdAccount->m_nNumChar && lIndex < AGSMACCOUNT_MAX_ACCOUNT_CHARACTER; ++lIndex)
					{
					pAgpdCharacter = m_pAgpmCharacter->GetCharacter(pAgsdAccount->m_lCID[lIndex]);
					if (pAgpdCharacter)
						{
						pAgsdCharacter = m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);

						// 강제 삭제라고 세팅한다.
						pAgsdCharacter->m_bDisconnectByDuplicateUser = TRUE;

						if (0 == _tcscmp(pAgsdCharacter->m_szAccountID, pszAccountID))
							{
							UINT32 ulNID = pAgsdCharacter->m_dpnidCharacter;

							// notify to client
							m_pAgsmCharacter->SendDisconnectByAnotherUser(ulNID);

							//if (m_pAgpmCharacter->IsNPC(pAgpdCharacter) || pAgpdCharacter->m_bIsProtectedNPC)
							//	AuLogFile("RemoveNPC.log", "Removed by login module (remove duplicated character)\n");

							// remove
							m_pAgpmCharacter->RemoveCharacter(pAgpdCharacter->m_lID);
							
							// disconnect
							DestroyClient(ulNID);
							}
						}
					}

				m_pAgsmAccountManager->RemoveAccount(pszAccountID, TRUE);
				}
			}
		else if (AGPMLOGIN_RENAME_CHARACTER == cOperation)
			{
			if (NULL != pvCharDetailInfo)
				{
				TCHAR *pszNewChar = NULL;
				TCHAR *pszOldChar = NULL;
				m_pAgpmLogin->m_csPacketCharInfo.GetField(FALSE, pvCharDetailInfo, 0, 
														NULL,
														&pszNewChar,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														NULL,
														&pszOldChar
														#ifndef _AREA_CHINA_
														,NULL			// Jumping Event Character Flag
														#endif
														);
				if (NULL != pszNewChar && NULL != pszOldChar)
					EnumCallback(AGSMLOGIN_CB_CHARACTER_RENAMED, pszOldChar, pszNewChar);
				}
			}
		}

	return bResult;
}




//	Callback
//==================================================
//
BOOL AgsmLogin::CBDisconnectFromGameServer(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (NULL == pClass || NULL == pData)
		return FALSE;
	
	AgsmLogin		*pThis = (AgsmLogin *) pClass;
	AgpdCharacter	*pAgpdCharacter = (AgpdCharacter *) pData;

	AgsdCharacter *pAgsdCharacter = pThis->m_pAgsmCharacter->GetADCharacter(pAgpdCharacter);
	if (NULL == pAgsdCharacter || TRUE == pAgsdCharacter->m_bDisconnectByDuplicateUser)
		return FALSE;

	// notify(one of available login server) character disconnected from game
	// to reset loginstatus table
	BOOL bResult = FALSE;
	INT16 nIndex = 0;
	for ( ;; )
		{
		AgsdServer *pAgsdServer = pThis->m_pAgsmServerManager->GetLoginServers(&nIndex);
		if (NULL == pAgsdServer)
			break;
		
		if (pAgsdServer->m_bIsConnected)
			{
			//bResult = pThis->SendCharacterDisconnectedFromGameServer(pAgsdCharacter->m_szAccountID, pAgpdCharacter->m_lID, pAgsdServer->m_dpnidServer);
			break;
			}
		}

	return bResult;
	}




//	Callback setting
//=============================================
//
BOOL AgsmLogin::SetCallbackCharacterRenamed(ApModuleDefaultCallBack pfCallback, PVOID pClass)
	{
	return SetCallback(AGSMLOGIN_CB_CHARACTER_RENAMED, pfCallback, pClass);
	}

