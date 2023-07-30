/*=============================================================================

	AgsmWorld.cpp

=============================================================================*/


#include "AgsmWorld.h"


/****************************************************/
/*		The Implementation of AgsmWorld class		*/
/****************************************************/
//
AgsmWorld::AgsmWorld()
	{
	SetModuleName(_T("AgsmWorld"));
	SetModuleType(APMODULE_TYPE_SERVER);

	m_pAgpmWorld = NULL;
	m_pAgsmServerManager = NULL;
	}


AgsmWorld::~AgsmWorld()
	{
	}




//	ApModule inherited
//===========================================================
//
BOOL AgsmWorld::OnAddModule()
	{
	// get related module
	m_pAgpmWorld = (AgpmWorld *) GetModule(_T("AgpmWorld"));
	m_pAgsmServerManager = (AgsmServerManager *) GetModule(_T("AgsmServerManager2"));
	m_pAgsmInterServerLink = (AgsmInterServerLink *) GetModule(_T("AgsmInterServerLink"));
	m_pAgsmLoginClient = (AgsmLoginClient *) GetModule(_T("AgsmLoginClient"));
	if (!m_pAgpmWorld || !m_pAgsmServerManager || !m_pAgsmInterServerLink || !m_pAgsmLoginClient)
		return FALSE;

	m_pAgsmServerManager->SetCallbackAddServer(CBAddServer, this);
	m_pAgsmServerManager->SetCallbackUpdateServerStatus(CBUpdateServerStatus, this);
	m_pAgpmWorld->SetCallbackGetWorld(CBGetWorld, this);
	m_pAgsmInterServerLink->SetCallbackDisconnect(CBDisconnectServer, this);

	return TRUE;
	}	


BOOL AgsmWorld::OnInit()
	{
	return TRUE;
	}


BOOL AgsmWorld::OnDestroy()
	{
	return TRUE;
	}




//	Packet send(internal)
//============================================================
//
#define _ctextend				_T('\0')

BOOL AgsmWorld::_SendPacketGetWorldResult(TCHAR* pszWorld, UINT32 ulNID)
	{
	ASSERT(NULL != m_pAgpmWorld);

	AgsdServer *pcsServer = m_pAgsmServerManager->GetGameServerOfWorld(pszWorld);
	
	if (NULL == pszWorld)
		{
		TCHAR	szEncoded[AGPMWORLD_MAX_ENCODED_LENGTH];
		TCHAR*	pszEncoded = szEncoded;
		ZeroMemory(szEncoded, sizeof(szEncoded));
		AgpdWorld* pAgpdWorld = NULL;
		for (INT32 l = 0; l < m_pAgpmWorld->GetWorldCount(); l++)
		{
			pAgpdWorld = m_pAgpmWorld->GetWorld(l);
			if (NULL == pAgpdWorld)
				return FALSE;

			pcsServer = m_pAgsmServerManager->GetGameServerOfWorld(pAgpdWorld->m_szName);
			
			pszEncoded += _stprintf(pszEncoded, _T("%s%c%s%c%d%c%d%c%d%c%s%c"), 
									pAgpdWorld->m_szName, AGPMWORLD_DELIM1,
									pAgpdWorld->m_szGroupName, AGPMWORLD_DELIM1,
									pAgpdWorld->m_ulPriority, AGPMWORLD_DELIM1,
									pAgpdWorld->m_nStatus, AGPMWORLD_DELIM1,
									pAgpdWorld->m_ulFlag, AGPMWORLD_DELIM1,
									pcsServer ? pcsServer->m_szServerRegion : "Common", AGPMWORLD_DELIM2
									);
			ASSERT(_tcslen(pszEncoded) < AGPMWORLD_MAX_ENCODED_LENGTH);
		}
		return _SendPacketGetWorldResultAll(szEncoded, ulNID);
		}
	else
		{
		AgpdWorld* pcsAgpdWorld = m_pAgpmWorld->GetWorld(pszWorld);
		if (NULL == pcsAgpdWorld || !_SendPacketGetWorldResult(pcsAgpdWorld, ulNID))
			return FALSE;
		}
	return TRUE;
	}

#undef _ctextend


BOOL AgsmWorld::_SendPacketGetWorldResult(AgpdWorld* pcsAgpdWorld, UINT32 ulNID)
	{
	ASSERT(NULL != m_pAgpmWorld);

	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD;
	//INT8	cOp = cOperation;
	INT16	nLength;

	AgsdServer *pcsServer	= m_pAgsmServerManager->GetServer(pcsAgpdWorld->m_szGroupName);

	// make packet
	PVOID	pvPacket = m_pAgpmWorld->m_csPacket.MakePacket(TRUE, &nLength, AGPMWORLD_PACKET_TYPE,
											 &cOperation,
											 pcsAgpdWorld->m_szName,
											 &(pcsAgpdWorld->m_nStatus),
											 NULL,
											 pcsServer ? pcsServer->m_szServerRegion : "Common"
											 );
	
	// send
	if (pvPacket && SendPacket(pvPacket, nLength, ulNID))
		bResult = TRUE;
	m_pAgpmWorld->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}


BOOL AgsmWorld::_SendPacketGetWorldResultAll(TCHAR* pszEncoded, UINT32 ulNID)
	{
	ASSERT(NULL != m_pAgpmWorld);

	BOOL	bResult = FALSE;
	INT8	cOperation = AGPMWORLD_PACKET_OPERATION_RESULT_GETWORLD_ALL;
	INT16	nLength;

	INT32	lEncodedLength = sizeof(TCHAR) * ((INT32)_tcslen(pszEncoded) + 1);

	// make packet
	PVOID	pvPacket = m_pAgpmWorld->m_csPacket.MakePacket(TRUE, &nLength, AGPMWORLD_PACKET_TYPE,
											 &cOperation,
											 NULL,
											 NULL,
											 (PVOID)pszEncoded,
											 &lEncodedLength,
											 NULL
											 );
	
	// send
	if (pvPacket && SendPacket(pvPacket, nLength, ulNID))
		bResult = TRUE;
	m_pAgpmWorld->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Packet send(external)
//============================================================
//
BOOL AgsmWorld::SendPacketCharCount(TCHAR *pszAccount, TCHAR *pszEncoded, UINT32 ulNID)
	{
	BOOL	bResult = FALSE;
	INT16	nLength;
	INT8	cOperation = AGPMWORLD_PACKET_OPERATION_RESULT_CHAR_COUNT;

	INT32	lEncodedLength = sizeof(TCHAR) * ((INT32)_tcslen(pszEncoded) + 1);
	if (NULL == pszEncoded || 0 >= lEncodedLength)
		return FALSE;

	// make packet
	PVOID	pvPacket = m_pAgpmWorld->m_csPacket.MakePacket(TRUE, &nLength, AGPMWORLD_PACKET_TYPE,
											 &cOperation,
											 pszAccount,
											 NULL,
											 pszEncoded,
											 &lEncodedLength,
											 NULL
											 );
	
	// send
	if (pvPacket && SendPacket(pvPacket, nLength, ulNID))
		bResult = TRUE;

	m_pAgpmWorld->m_csPacket.FreePacket(pvPacket);

	return bResult;
	}




//	Callback methods
//===========================================================
//
BOOL AgsmWorld::CBGetWorld(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass)
		return FALSE;

	AgsmWorld	*pThis = (AgsmWorld *) pClass;
	TCHAR *pszWorld = (TCHAR *) pData;
	UINT32 ulNID = PtrToUint(pCustData);

	return pThis->_SendPacketGetWorldResult(pszWorld, ulNID);
	}


BOOL AgsmWorld::CBDisconnectServer(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmWorld	*pThis = (AgsmWorld *) pClass;
	AgsdServer	*pcsAgsdServer = (AgsdServer *) pData;

	return  pThis->_UpdateStatus(pcsAgsdServer);
	}


BOOL AgsmWorld::CBUpdateServerStatus(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmWorld	*pThis = (AgsmWorld *) pClass;
	AgsdServer	*pcsAgsdServer = (AgsdServer *) pData;
	INT16 nOldStatus = pCustData ? *((INT16 *) pCustData) : 0;

	return  pThis->_UpdateStatus(pcsAgsdServer);
	}


BOOL AgsmWorld::CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData)
	{
	if (!pClass || !pData)
		return FALSE;

	AgsmWorld	*pThis = (AgsmWorld *) pClass;
	AgsdServer	*pcsAgsdServer = (AgsdServer *) pData;
	INT16 nOldStatus = pCustData ? *((INT16 *) pCustData) : 0;

	return  pThis->_UpdateStatus(pcsAgsdServer);
	}




//	Status
//=====================================================
//
BOOL AgsmWorld::_UpdateStatus(AgsdServer* pcsServer)
	{
	ASSERT(NULL != m_pAgpmWorld);
	// ###############################################################
	// pcsServer�� Status�� ���� Players�� ����ؾ� �Ѵ�.
	// 0 �ΰ�쵵 �����Ƿ�...
	// ###############################################################

	if (AGSMSERVER_TYPE_GAME_SERVER != pcsServer->m_cType
		|| FALSE == pcsServer->m_bEnable
		)
		return TRUE;

	// main world�� sub world�� ���...
	AgpdWorld *pcsWorld = m_pAgpmWorld->GetWorld(pcsServer->m_szWorld);
	if (!pcsWorld)
		{
		pcsWorld = m_pAgpmWorld->UpdateWorld(pcsServer->m_szWorld, _CalculateStatus(pcsServer));
		pcsWorld->m_Mutex.WLock();
		
		CHAR *psz = NULL;
		// NC17 ?
		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_ADULT_SERVER);
		if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
			pcsWorld->SetNC17(TRUE);
		
		// new server ?
		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_NEW_SERVER);
		if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
			pcsWorld->SetNew(TRUE);
			
		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_EVENT_SERVER);
		if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
			pcsWorld->SetEvent(TRUE);

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_AIM_EVENT_SERVER);
		if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
			pcsWorld->SetAimEvent(TRUE);

		psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_TEST_SERVER);
		if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
			pcsWorld->SetTestServer(TRUE);
			
		strcpy(pcsWorld->m_szGroupName, (*pcsServer->m_szGroupName) ? pcsServer->m_szGroupName : pcsServer->m_szWorld);
		pcsWorld->m_ulPriority = pcsServer->m_lOrder;
		
		pcsWorld->m_ServerList.push_back((PVOID) pcsServer);
		pcsWorld->m_Mutex.Release();
		}
	else
		_UpdateStatus(pcsServer, pcsWorld);
	
	for (INT16 i=0; i<pcsServer->m_nSubWorld; ++i)
		{
		if (FALSE == pcsServer->m_SubWorld[i].m_bShow)
			continue;
		
		pcsWorld = m_pAgpmWorld->GetWorld(pcsServer->m_SubWorld[i].m_szName);
		if (!pcsWorld)
			{
			pcsWorld = m_pAgpmWorld->UpdateWorld(pcsServer->m_SubWorld[i].m_szName, _CalculateStatus(pcsServer));
			pcsWorld->m_Mutex.WLock();

			CHAR *psz = NULL;
			// NC17 ?
			psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_ADULT_SERVER);
			if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
				pcsWorld->SetNC17(TRUE);
			
			// new server ?
			psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_NEW_SERVER);
			if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
				pcsWorld->SetNew(TRUE);			
				
			psz = pcsServer->GetExtraValue(AGSMSERVER_EXTRA_KEY_IS_EVENT_SERVER);
			if (NULL != psz && 0 == _tcsnicmp(psz, _T("y"), 1))
				pcsWorld->SetNew(TRUE);	
				
			strcpy(pcsWorld->m_szGroupName, (*pcsServer->m_szGroupName) ? pcsServer->m_szGroupName : pcsServer->m_szWorld);
			pcsWorld->m_ulPriority = pcsServer->m_lOrder;
			
			pcsWorld->m_ServerList.push_back((PVOID) pcsServer);
			pcsWorld->m_Mutex.Release();
			}
		else
			_UpdateStatus(pcsServer, pcsWorld);
		}
	
	return TRUE;
	}

	
BOOL AgsmWorld::_UpdateStatus(AgsdServer *pcsServer, AgpdWorld *pcsWorld)
	{
	AgsdServer	*pcsTemp = NULL;
	AgsdServer	*pcsFujikuri = NULL;
	BOOL bFind = FALSE;

	// find given server from list
	for (int i=0; i < pcsWorld->m_ServerList.size(); i++)
		{
		pcsTemp = (AgsdServer *) pcsWorld->m_ServerList[i];
		if (pcsServer == pcsTemp)
			{
			bFind = TRUE;
			break;
			}
		}

	// if not found, add given server to list
	if (!bFind)
		{
		pcsWorld->m_Mutex.WLock();
		pcsWorld->m_ServerList.push_back((PVOID) pcsServer);
		pcsWorld->m_Mutex.Release();
		}

	// find fujikuri server
	INT16 nWorst = 30000;
	for (int i = 0; i < pcsWorld->m_ServerList.size(); i++)
		{
		pcsTemp = (AgsdServer *) pcsWorld->m_ServerList[i];
		if (pcsTemp->m_nNumPlayers < nWorst)
			{
			nWorst = pcsTemp->m_nNumPlayers;
			pcsFujikuri = pcsTemp;
			}
		}
	
	m_pAgpmWorld->UpdateWorld(pcsWorld->m_szName, _CalculateStatus(pcsFujikuri), pcsWorld->m_szGroupName, pcsWorld->m_ulPriority, pcsWorld->m_ulFlag);

	return TRUE;
	}


INT16 AgsmWorld::_CalculateStatus(AgsdServer* pcsServer)
	{
	if (!pcsServer || pcsServer->m_dpnidServer == 0)
		return AGPDWORLD_STATUS_UNKNOWN;

	INT32	lMaxProcessUser	= m_pAgsmLoginClient->GetMaxCreateProcessUserCount();

	// �Ϲ�:
	// 1500�� �̻� High, ���� Medium
	// 2800�� �̻� Full
	if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea )
	{
		if (pcsServer->m_nNumPlayers <= 1200 )
			return AGPDWORLD_STATUS_GOOOOOOD;
		else if (pcsServer->m_nNumPlayers <= 1950 )
			return AGPDWORLD_STATUS_NORMAL;
		else
			return AGPDWORLD_STATUS_BAD;
	}
	else
	{
		//if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_UNKNOWN] / 100.0f)
		//	return AGPDWORLD_STATUS_UNKNOWN;
		//else if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_GOOOOOOD] / 100.0f)
		if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_GOOOOOOD] / 100.0f)
			return AGPDWORLD_STATUS_GOOOOOOD;
		else if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_ABOVE_NORMAL] / 100.0f)
			return AGPDWORLD_STATUS_ABOVE_NORMAL;
		else if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_NORMAL] / 100.0f)
			return AGPDWORLD_STATUS_NORMAL;
		else if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_BELOW_NORMAL] / 100.0f)
			return AGPDWORLD_STATUS_BELOW_NORMAL;
		//else if (pcsServer->m_nNumPlayers <= (INT32) lMaxProcessUser * g_nAgpdWorldStatusNum[AGPDWORLD_STATUS_BAD] / 100.0f)
		//	return AGPDWORLD_STATUS_BAD;
		else
			return AGPDWORLD_STATUS_BAD;
	}

	return AGPDWORLD_STATUS_UNKNOWN;
	}

