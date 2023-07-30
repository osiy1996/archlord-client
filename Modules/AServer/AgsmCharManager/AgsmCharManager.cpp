/******************************************************************************
Module:  AgsmCharManager.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 08. 23
******************************************************************************/

#include "AgsmCharManager.h"
#include "AgsmTitle.h"
#include "AgsmGameholic.h"
#include "AuGameEnv.h"

#include "AgsmSkill.h"
#include "AgsmBilling.h"
#include "AgsmBillInfo.h"
#include "AgsmPrivateTrade.h" //JK_�ŷ��߱���


#ifdef _AREA_KOREA_
#include "../Server/HanGameForServer/HangameTPack/AuHangameTPack.h"
#endif

AgsmCharManager::AgsmCharManager()
{
	SetModuleName("AgsmCharManager");

	SetModuleType(APMODULE_TYPE_SERVER);

	SetPacketType(AGSMCHARMANAGER_PACKET_TYPE);

	// flag size is sizeof(CHAR)
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_INT8, 1,			// Operation
							AUTYPE_CHAR, 12,		// Account Name
							AUTYPE_INT32, 1,		// character template id
							AUTYPE_CHAR, AGPACHARACTER_MAX_ID_STRING + 1,		// Character Name
							AUTYPE_INT32, 1,		// Character id (ó���� ĳ���� �Ƶ�)
							AUTYPE_POS, 1,			// character position
							AUTYPE_INT32, 1,		// auth key
							/*
#ifdef	__ATTACH_LOGINSERVER__
							AUTYPE_CHAR, AGSM_MAX_SERVER_NAME + 1,	// server group name
#endif	//__ATTACH_LOGINSERVER__
							*/
							AUTYPE_END, 0
							);
	
	m_pagsmItem = NULL;
	m_pagsmBilling = NULL;
	m_pagsmBillInfo = NULL;
#ifdef _AREA_KOREA_
	m_csHangameTPack = NULL;
#endif
}

AgsmCharManager::~AgsmCharManager()
{
}

BOOL AgsmCharManager::OnAddModule()
{
	m_papmMap				= (ApmMap *)				GetModule("ApmMap");
	m_pagpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pagpmCharacter		= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pagpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pagpmEventBinding		= (AgpmEventBinding *)		GetModule("AgpmEventBinding");
	m_pagpmStartupEncryption= (AgpmStartupEncryption *) GetModule("AgpmStartupEncryption");
	m_pagpmLog				= (AgpmLog*)				GetModule("AgpmLog");
	m_pagpmAdmin			= (AgpmAdmin*)				GetModule("AgpmAdmin");
	m_papmEventManager		= (ApmEventManager *)		GetModule("ApmEventManager");
	m_pagpmBillInfo			= (AgpmBillInfo*)			GetModule("AgpmBillInfo");

	//m_pagsmDBStream			= (AgsmDBStream *)			GetModule("AgsmDBStream");
	m_pagsmAOIFilter		= (AgsmAOIFilter *)			GetModule("AgsmAOIFilter");
	//m_pagsmServerManager	= (AgsmServerManager *)		GetModule("AgsmServerManager");
	m_pAgsmServerManager	= (AgsmServerManager *)		GetModule("AgsmServerManager2");
	m_pagsmCharacter		= (AgsmCharacter *)			GetModule("AgsmCharacter");
	m_pagsmSystemInfo		= (AgsmSystemInfo *)		GetModule("AgsmSystemInfo");
	m_pagsmFactors			= (AgsmFactors *)			GetModule("AgsmFactors");
	m_pagsmAccountManager	= (AgsmAccountManager *)	GetModule("AgsmAccountManager");
	m_pagsmZoning			= (AgsmZoning *)			GetModule("AgsmZoning");
	m_pagsmItem				= (AgsmItem *)				GetModule("AgsmItem");
	m_pAgpmConfig			= (AgpmConfig *)			GetModule("AgpmConfig");
	// JNY TODO : Relay ���� ������ ���� m_pagsmDBStream�� üũ�ϴ� �κ��� 
	// ��� �����մϴ�. 
	// 2004.2.16
	// �ٽ� ���� �ϰų� DB��� ���濡 ���� �����۾��� �ؾ��մϴ�.

	if (!m_papmMap || 
		!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmItem ||
		!m_pagpmAdmin ||
		!m_pagpmBillInfo ||
		!m_pagsmAOIFilter ||
		!m_pAgsmServerManager || 
		!m_pagsmCharacter ||
		!m_pagsmFactors ||
		!m_pagsmAccountManager ||
		!m_pagsmSystemInfo ||
		/*!m_pagsmDBStream ||*/
		!m_pagsmZoning ||
		!m_pagsmItem ||
		!m_pagpmEventBinding ||
		!m_papmEventManager)
		return FALSE;

	//if (!m_pagsmDBStream->SetCallbackCharacterResult(CBDBOperationResult, this))
	//	return FALSE;

	//if (!m_pagsmFactors->SetCallbackFactorDB(CBDBFactor, this))
	//	return FALSE;

//	if (!m_papmMap->SetCallbackRemoveChar(CBRemoveCharacter, this))
//		return FALSE;

//	if (!m_pagpmCharacter->SetCallbackDeleteChar(CBDeleteCharacter, this))
//		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveCharacterFromMap(CBRemoveCharFromMap, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackGetNewCID(CBGetNewCID, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackRemoveID(CBRemoveCharacterID, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackGetNewCID(CBGetNewCID, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackCompleteRecvCharDataFromLoginServer(CBCompleteRecvCharFromLoginServer, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendAuthKey(CBSendAuthKey, this))
		return FALSE;

#ifndef _AREA_CHINA_
	if (!m_pagpmCharacter->SetCallbackCheckPCRoomType(CBCheckPCRoomType, this))
		return FALSE;
#endif

	m_csCertificatedAccount.InitializeObject( sizeof(INT32), AGPACHARACTER_CHARACTER_DATA_COUNT );

	return TRUE;
}

BOOL AgsmCharManager::OnInit()
{
	m_pagsmSkill			= (AgsmSkill*)GetModule("AgsmSkill");
	m_pagsmBilling			= (AgsmBilling*)GetModule("AgsmBilling");
	m_pagsmTitle			= (AgsmTitle*)GetModule("AgsmTitle");
	m_pagsmBillInfo			= (AgsmBillInfo*)GetModule("AgsmBillInfo");

	m_pcsAgsmPrivateTrade = (AgsmPrivateTrade*)GetModule("AgsmPrivateTrade");//JK_�ŷ��߱���

	if (!m_pcsAgsmPrivateTrade) return FALSE;//JK_�ŷ��߱���


	if(!m_pagsmBillInfo)
		return FALSE;

#ifdef _AREA_KOREA_
	m_csHangameTPack		= new AuHangameTPack;
	if(!m_csHangameTPack)
		return FALSE;
#endif	

	return TRUE;
}

BOOL AgsmCharManager::OnDestroy()
{
#ifdef _AREA_KOREA_
	if(m_csHangameTPack)
	{
		m_csHangameTPack->Destroy();
		delete m_csHangameTPack;
		m_csHangameTPack = NULL;
	}
#endif

	return TRUE;
}

BOOL AgsmCharManager::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmCharManager::OnValid(CHAR* szData, INT16 nSize)
{
	return TRUE;
}

BOOL AgsmCharManager::AddCertificatedAccount( char *pstrAccountID, INT32 lCID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csCertificatedAccount.AddObject( &lCID, pstrAccountID ) )
	{
		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmCharManager::RemoveCertificatedAccount( char *pstrAccountID )
{
	BOOL			bResult;

	bResult = FALSE;

	if( m_csCertificatedAccount.RemoveObject( pstrAccountID ) )
	{
		bResult = TRUE;
	}

	return bResult;
}

INT32 AgsmCharManager::GetCertificatedAccountCID( char *pstrAccountID )
{
	void			*pvData;
	INT32			*plCID;

	plCID = NULL;

	pvData = m_csCertificatedAccount.GetObject( pstrAccountID );

	if( pvData != NULL )
	{
		plCID = (INT32 *)(pvData);

		return *plCID;
	}
	else
	{
		return 0;
	}
}

/*
AgpdCharacter* AgsmCharManager::AddCharacter(CHAR *szAccountID, CHAR *szName, INT32 lTID, UINT32 ulNID)
{
	AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(szAccountID);

	return AddCharacter(pcsAccount, szName, lTID, ulNID);
}
*/

AgpdCharacter* AgsmCharManager::AddCharacter(/*AgsdAccount *pcsAccount, */CHAR *szName, INT32 lTID, UINT32 ulNID, BOOL bLoadChar)
{
	//STOPWATCH2(GetModuleName(), _T("AddCharacter"));

	//if (!szName || !strlen(szName))
	//	return NULL;

	//if (pcsAccount && pcsAccount->m_nNumChar == AGSMACCOUNT_MAX_ACCOUNT_CHARACTER)
	//	return NULL;

//	AgpdCharacter *pcsCharacter = CreateCharacterData();
	AgpdCharacter *pcsCharacter = m_pagpmCharacter->AddCharacter(m_csGenerateCID.GetID(), lTID, szName);
	if (!pcsCharacter)
		return NULL;

	AgsdCharacter *pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pcsCharacter->m_unCurrentStatus = AGPDCHAR_STATUS_LOGOUT;

	pcsAgsdCharacter->m_dpnidCharacter = ulNID;

	/*
	if (pcsAccount)
	{
		strncpy(pcsAgsdCharacter->m_szAccountID, pcsAccount->m_szName, AGPACHARACTER_MAX_ID_STRING);

		pcsAccount->m_pcsCharacter[pcsAccount->m_nNumChar] = pcsCharacter;
		pcsAccount->m_lCID[pcsAccount->m_nNumChar++] = pcsCharacter->m_lID;
	}
	*/

	// DB���� �о�� ���� DB�� �ִ� ���͵���� �����͸� �� �����Ѵ�. 
	// ���� ������ ����϶��� ���ø� ���͸� �����ϰ� ó���� �Ѵ�.
	if (!bLoadChar)
	{
		m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &((AgpdCharacterTemplate *) pcsCharacter->m_pcsCharacterTemplate)->m_csFactor, TRUE);

		/*
		INT32 lLevel = m_pagpmFactors->GetLevel(&pcsCharacter->m_csFactor);
		if (lLevel > 0 && lLevel <= AGPMCHAR_MAX_LEVEL)
			m_pagpmFactors->CopyFactor(&pcsCharacter->m_csFactor, &pcsCharacter->m_pcsCharacterTemplate->m_csLevelFactor[lLevel], TRUE, FALSE);
		*/

		AgpdFactorCharPointMax *pcsFactorCharPointMax = (AgpdFactorCharPointMax *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
		if (pcsFactorCharPointMax)
		{
			m_pagpmFactors->SetMaxExp(&pcsCharacter->m_csFactor, m_pagpmCharacter->GetLevelUpExp(pcsCharacter));

			AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);
			if (pcsFactorCharPoint)
			{
				INT32	lMaxHP	= 0;
				INT32	lMaxMP	= 0;
				INT32	lMaxSP	= 0;

				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
				m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			}
		}

		m_pagpmFactors->SetOwnerFactor(&pcsCharacter->m_csFactor, pcsCharacter->m_lID, (PVOID) pcsCharacter);
		
		//m_pagpmCharacter->UpdateInit(pcsCharacter);
	}

	//return m_csCharAdmin.AddCharacter(pcsCharacter);
	return pcsCharacter;
}

/*
BOOL AgsmCharManager::RemoveCharacter(CHAR *szAccountID, INT32 lCID)
{
	AgsdAccount *pcsAccount = m_pagsmAccountManager->GetAccount(szAccountID);

	return RemoveCharacter(pcsAccount, lCID);
}
*/

//BOOL AgsmCharManager::RemoveCharacter(/*AgsdAccount *pcsAccount,*/ INT32 lCID)
//{
//	if (lCID == AP_INVALID_CID)
//		return FALSE;

	/*
	if (pcsAccount)
	{
		if (pcsAccount->m_lSelectCharacter == lCID)
		{
			pcsAccount->m_lSelectCharacter = AP_INVALID_CID;
		}

		for (int i = 0; i < pcsAccount->m_nNumChar; ++i)
		{
			if (pcsAccount->m_lCID[i] == lCID)
			{
				CopyMemory(pcsAccount->m_lCID + i, pcsAccount->m_lCID + i + 1, 
					sizeof(INT32) * (pcsAccount->m_nNumChar - i - 1));
				CopyMemory(pcsAccount->m_pcsCharacter + i, pcsAccount->m_pcsCharacter + i + 1, 
					sizeof(AgpdCharacter *) * (pcsAccount->m_nNumChar - i - 1));

				--pcsAccount->m_nNumChar;

				break;
			}
		}
	}
	*/

//	if (!m_pagpmCharacter->RemoveCharacter(lCID))
//		return FALSE;
//
//	return TRUE;
//}


BOOL AgsmCharManager::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket || !nSize)
		return FALSE;

	INT8		cOperation = -1;
	CHAR		*szAccountName = NULL;
	CHAR		*szCharname = NULL;
	//INT32		lAccountID = 0;
	INT32		lTID = 0;

	INT32		lCID = 0;
	AuPOS		stPosition;

	CHAR		*szCharName = NULL;

	INT32		lAuthKey	= 0;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&cOperation,
						&szAccountName,
						&lTID,
						&szCharName,
						&lCID,
						&stPosition,
						&lAuthKey);

	switch (cOperation)
	{
	case AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT:
		{
			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT"));

			if(m_pagpmStartupEncryption)
			{
				if(!m_pagpmStartupEncryption->CheckCompleteStartupEncryption(ulNID))
				{
					DestroyClient(ulNID);
					return FALSE;
				}
			}

			// login server�� ���� ����Ÿ�� ��� �޾Ҵ�.
			// ����Ÿ�� ��� �޾����� ���� GameWorld�� �߰��Ѵ�.

			CHAR	szBuffer[256];
			ZeroMemory(szBuffer, sizeof(CHAR) * 256);

//			sprintf(szBuffer, "AgsmCharManager::OnReceive() try AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT");
//			ASSERT(strlen(szBuffer) < 256);
//			WriteLog(AS_LOG_RELEASE, szBuffer);

			if (!szCharName || !strlen(szCharName))
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� character name error : null string");
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}

			// Mutex Lock�� �ϰ� �Ѱ��ش�. ���� �ؾ� �Ѵ�.
			AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacterLock(m_pagsmCharacter->GetRealCharName(szCharName));
			if (!pcsCharacter)
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� GetCharacterLock(%s) failed!!!", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				// �̰� ���.. �̤� 2005.04.03. steeple
				//m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);

				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
			if (!pcsAgsdCharacter)
			{
				//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by AGSMCHARMANAGER_PACKET_OPERATION_REQUEST_CLIENT_CONNECT\n");

				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� EnterGameWorld(%s) failed(8)", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);

				return FALSE;
			}

			INT32	lNumChar	= 0;

			AgsdAccount	*pcsAccount	= m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if (pcsAccount)
			{
				lNumChar	= pcsAccount->m_nNumChar;

				m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID, TRUE);
			}
/*
			AgsdAccount	*pcsAccount	= m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if(pcsAccount)
			{
				m_pagsmAccountManager->AddCharacterToAccount(pcsAgsdCharacter->m_szAccountID, pcsCharacter->m_lID, pcsCharacter->m_szID, FALSE);
			}
			else
			{
				pcsCharacter->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);
				return FALSE;
			}
*/
			/*
			if (lNumChar > 0)
			{
				pcsCharacter->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);

				DestroyClient(ulNID);
				return FALSE;
			}
			*/

			
			if (m_pagsmAccountManager->AddAccount(pcsAgsdCharacter->m_szAccountID, ulNID))
			{
				m_pagsmAccountManager->AddCharacterToAccount(pcsAgsdCharacter->m_szAccountID, pcsCharacter->m_lID, pcsCharacter->m_szID, FALSE);
			}
			else
			{
				//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
				//	AuLogFile("RemoveNPC.log", "Removed by AddAccount()\n");

				pcsCharacter->m_Mutex.Release();
				m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
				DestroyClient(ulNID);
				return FALSE;
			}

			/*
			m_pagsmAccountManager->AddAccount(pcsAgsdCharacter->m_szAccountID, ulNID);
			m_pagsmAccountManager->AddCharacterToAccount(pcsAgsdCharacter->m_szAccountID, pcsCharacter->m_lID, pcsCharacter->m_szID);
			*/

			// �̹� ���忡 �߰��� ������ ����. �߰��� ���̶�� �߸��� ���̴�...
			if (pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� CurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD (%s)", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}

			// �α��� �����κ��� �޾Ƴ��� ����Ű�� Ŭ���̾�Ʈ���� ������ ����Ű�� ���Ѵ�.
			// �ٸ��ٸ� ©�������.
			if (!pcsAgsdCharacter->m_bIsValidAuthKey ||
				lAuthKey != pcsAgsdCharacter->m_lAuthKey)
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� (%s)�� ����Ű�� �߸��Ǿ���.", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				DestroyClient(ulNID);

				return FALSE;
			}
			
			// ����Ű�� �̿��� �ѹ� ���� ������ ����Ű�� Invalid ���� ������.
			pcsAgsdCharacter->m_bIsValidAuthKey	= FALSE;
			
			// ������� �ٸ� ������ �α��� �ߴ��� üũ.
			AgsdServer *pcsServer = m_pAgsmServerManager->GetThisServer();
			if (!pcsServer)
			{
				DestroyClient(ulNID);
				return FALSE;
			}

			BOOL bFoundWorld = FALSE;
			if (0 == strcmp(pcsServer->m_szWorld, pcsAgsdCharacter->m_szServerName))
				bFoundWorld = TRUE;

			if (!bFoundWorld)
			{
				for (INT32 i = 0; i < pcsServer->m_nSubWorld; i++)
				{
					if (0 == strcmp(pcsServer->m_SubWorld[i].m_szName, pcsAgsdCharacter->m_szServerName))
					{
						bFoundWorld = TRUE;
						break;
					}
				}
			}

			if (!bFoundWorld)
			{
				char strCharBuff[256] = { 0, };
				sprintf_s(strCharBuff, sizeof(strCharBuff), "Acc[%s]Char[%s]World[%s] entered another world[%s]\n",
																	pcsAgsdCharacter->m_szAccountID,
																	pcsCharacter->m_szID,
																	pcsAgsdCharacter->m_szServerName,
																	pcsServer->m_szWorld);
				AuLogFile_s("LOG\\DIFF_WORLD.log", strCharBuff);

				DestroyClient(ulNID);
				return FALSE;
			}

			pcsAgsdCharacter->m_dpnidCharacter = ulNID;

			// ��Ʈ�� ���� ������ ���ش�.
			GetConnectionInfo(pcsAgsdCharacter->m_dpnidCharacter, &pcsAgsdCharacter->m_ulRoundTripLatencyMS, NULL);
			SetIDToPlayerContext(pcsCharacter->m_lID, pcsAgsdCharacter->m_dpnidCharacter);
			SetAccountNameToPlayerContext(pcsAgsdCharacter->m_szAccountID, pcsAgsdCharacter->m_dpnidCharacter);	// 2006.04.28. steeple
			SetCheckValidation(ulNID);

			m_pagsmCharacter->ResetWaitOperation(pcsCharacter, AGSMCHARACTER_WAIT_OPERATION_ENTER_GAME_WORLD);

			ActiveSendBuffer(ulNID);

			if (m_pAgpmConfig)
				m_pAgpmConfig->SendConfigPacket(ulNID);

			// 2007.09.17. steeple
			// EventEffectID ������.
			m_pagsmCharacter->SendPacketEventEffectID(pcsCharacter->m_lID, m_pagpmCharacter->GetEventEffectID(), ulNID);

			SendCharacterLoadingPosition(pcsCharacter, ulNID);

			//Account Manager���� Coupon������ �����.
			

			EnumCallback(AGSMCHARMM_CB_CONNECTED_CHAR, pcsCharacter, UintToPtr(ulNID));

			// AuTimeStamp ����ȭ �����ش�.

			// GetCharacterLock()�� ȣ���ؼ� �����Ա� ������ Release()�� ����� �Ѵ�.
			pcsCharacter->m_Mutex.Release();
		}
		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE:
		{
			if (!pstCheckArg->bReceivedFromServer &&
				pstCheckArg->lSocketOwnerID != AP_INVALID_CID &&
				pstCheckArg->lSocketOwnerID != lCID)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE"));

			CHAR	szBuffer[256];
			ZeroMemory(szBuffer, sizeof(CHAR) * 256);

//			sprintf(szBuffer, "AgsmCharManager::OnReceive() try AGSMCHARMANAGER_PACKET_OPERATION_LOADING_COMPLETE");
//			ASSERT(strlen(szBuffer) < 256);
//			WriteLog(AS_LOG_RELEASE, szBuffer);

			AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
			if (!pcsCharacter)
			{
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� GetCharacterLock(%d) failed!!!", lCID);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			if (!EnterGameWorld(pcsCharacter))
			{
				pcsCharacter->m_Mutex.Release();

				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� EnterGameWorld(%d) failed!!!", lCID);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			pcsCharacter->m_Mutex.Release();
		}
 		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE"));

			// 2004.07.21. steeple
			// LoginServer ���� ĳ���� ������ �������� ��!!! �Ҹ���.
			EnumCallback(AGSMCHARMM_CB_DELETE_COMPLETE, m_pagsmCharacter->GetRealCharName(szCharName), NULL);
		}
		break;

	case AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY:
		{
			if (!pstCheckArg->bReceivedFromServer)
				return FALSE;

			//STOPWATCH2(GetModuleName(), _T("AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY"));

			AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(m_pagsmCharacter->GetRealCharName(szCharName));
			if (!pcsCharacter)
			{
				CHAR	szBuffer[256];
				ZeroMemory(szBuffer, sizeof(CHAR) * 256);

				sprintf(szBuffer, "AgsmCharManager::OnReceive() ���� GetCharacterLock(%s) failed!!!", szCharName);
				ASSERT(strlen(szBuffer) < 256);
//				TRACEFILE(szBuffer);

				return FALSE;
			}

			AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
			pcsAgsdCharacter->m_lAuthKey		= lAuthKey;
			pcsAgsdCharacter->m_bIsValidAuthKey	= TRUE;

			pcsCharacter->m_Mutex.Release();
		}
		break;

	default:
		{
			OutputDebugString("AgsmCharManager::OnReceive() Unknown Received Operation !!!\n");
			return FALSE;
		}
		break;
	}

	return TRUE;
}

BOOL AgsmCharManager::OnDisconnect(INT32 lCID, UINT32 ulNID)
{
	//STOPWATCH2(GetModuleName(), _T("OnDisconnect"));

	if (lCID != AP_INVALID_CID)		// ĳ���Ͱ� �����ϴ� ���.. �� �� ĳ���Ͱ� ���� ����Ǿ���.
	{
		AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacterLock(lCID);
		if (!pcsCharacter)
		{
//			TRACEFILE("AgsmCharManager::OnDisconnect() �������� �ʴ� CID�� �Ѿ�Դ�.");
			return FALSE;
		}

		m_pcsAgsmPrivateTrade->ProcessTradeCancelUnExpected(lCID);//JK_�ŷ��߱���

		AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
		AgsdZoning		*pcsAgsdZoning		= m_pagsmZoning->GetADCharacter(pcsCharacter);

		pcsAgsdCharacter->m_bIsNotLogout	= FALSE;

		if (pcsAgsdZoning->m_bDisconnectZoning)	// ���׶��� ������ ����� ����̴�. �� ���� �� �����۸� ���ְ� ĳ���� ����Ÿ�� ������Ų��.
		{
			pcsAgsdZoning->m_bDisconnectZoning = FALSE;

			m_pagpmCharacter->StopCharacter(pcsCharacter, &pcsCharacter->m_stPos);

			m_pagsmCharacter->RemoveItemOnly(pcsCharacter);

			pcsCharacter->m_Mutex.Release();

			return TRUE;

			//m_pagsmCharacter->SendPacketCharRemoveSync(pcsCharacter);
		}
		else	// ���������� ������ ����Ǿ���. ĳ���� ����Ÿ�� �����Ѵ�.
		{
			//m_papmMap->DeleteChar(pcsCharacter->m_stPos, pcsCharacter->m_lID);

			EnumCallback(AGSMCHARMM_CB_DISCONNECT_CHARACTER, pcsCharacter, NULL);

			// Logout Log �� �����. - 2004.05.02. steeple
			// ������ ����ÿ��� ����� ���� CBRemoveCharcter�� �̵�.
			// WriteLogoutLog(pcsCharacter);

			// Level Up Log �� �����. - 2004.05.18. steeple
			// ���̺� �������� ���ŵ�. - 2005.05.20. laki
			// m_pagsmCharacter->WriteLevelUpLog(pcsCharacter, 0, TRUE);

			//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by OnDisconnect()\n");

			pcsCharacter->m_Mutex.Release();

			AgsdAccount	*pcsAccount	= m_pagsmAccountManager->GetAccount(pcsAgsdCharacter->m_szAccountID);
			if (pcsAccount && pcsAccount->m_nNumChar <= 1)
			{
				m_pagsmAccountManager->RemoveAccount(pcsAgsdCharacter->m_szAccountID);
			}

			if (!m_pagpmCharacter->RemoveCharacter(lCID))
			{
				OutputDebugString("AgsmCharManager::OnDisconnect() Error (1) !!!\n");
				return FALSE;
			}

		}

	}

	return TRUE;
}

BOOL AgsmCharManager::IsValidLogin(CHAR *szAccountName)
{
	if (!szAccountName || !szAccountName[0])
		return FALSE;

	return TRUE;
}

//		CreateCharacter
//	Functions
//		- ���ο� ĳ���� �����.
//			(���̻� ���� �� �ִ��� �˻��Ѵ�. �״㿡 ����� �ִٸ� Ŭ���̾�Ʈ�� ������ ����Ÿ�� ĳ���� �����)
//	Arguments
//		  (ĳ���͸� ���鶧 ���ø� ����Ÿ���� ����Ǵ� ���׵��� �� ���ڷ� �Ѱ��ش�)
//		- szAccountName : ���� �̸�
//		- szCharName : ����� ���ο� ĳ���� �̸�
//		- lTID : ĳ���� ���ø� �Ƶ�
//		- ulNID : �� ����� network id
//	Return value
//		- AgpdCharacter * : ������ ĳ���� ������
///////////////////////////////////////////////////////////////////////////////
AgpdCharacter* AgsmCharManager::CreateCharacter(CHAR *szAccountName, CHAR *szCharName, INT32 lTID, UINT32 ulNID, BOOL bIsPC, CHAR *pszServerName)
{
	//STOPWATCH2(GetModuleName(), _T("CreateCharacter"));

	if (!lTID)
		return FALSE;

	// template ����Ÿ�� ������� �ؼ� ���ο� ĳ���͸� �����Ѵ�.
	// �⺻�� template ����Ÿ�� ����ϵ� ����ڰ� ������ ����Ÿ�� ���ڷ� �Ѱܹ޾� ���� �����Ų��.

	AgpdCharacterTemplate *pcsCharTemplate = m_pagpmCharacter->GetCharacterTemplate(lTID);
	if (!pcsCharTemplate)
		return FALSE;

	AgpdCharacter	*pcsCharacter = AddCharacter(/*pcsAccount, */szCharName, lTID, ulNID);
	if (!pcsCharacter)
		return FALSE;

	INT32	lRace	= m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor);

//	AuPOS	stFirstPos;
//
//	switch (lRace) {
//	case AURACE_TYPE_ORC:
//		{
//			stFirstPos.x = 94021.922f;
//			stFirstPos.y = 4133.300f;
//			stFirstPos.z = 244826.938f;
//			break;
//		}
//
//	case AURACE_TYPE_HUMAN:
//	default:
//		{
//			stFirstPos.x = -353348;
//			stFirstPos.y = 6946;
//			stFirstPos.z = 119705;
//			break;
//		}
//	}
//
//	if (!m_papmEventManager->GetRandomPos(&stFirstPos, &pcsCharacter->m_stPos, 100, 600, TRUE))
//		pcsCharacter->m_stPos	= stFirstPos;

//	if (!m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos))
//		return FALSE;

	// character point�� �����Ѵ�.
	m_pagsmFactors->SetCharPoint(&pcsCharacter->m_csFactor, FALSE, FALSE);

	if (bIsPC)
	{
		m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);

		AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);

		AgsdServer *pcsThisServer = m_pAgsmServerManager->GetThisServer();
		if (!pcsThisServer)
		{
			//if (m_pagpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by AgsmCharManager::CreateCharacter()\n");

			m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return FALSE;
		}
		// 2005.07.21	laki
		//pcsAgsdCharacter->m_ulServerID = pcsThisServer->m_lID;
		pcsAgsdCharacter->m_ulServerID = pcsThisServer->m_lServerID;

		ZeroMemory(pcsAgsdCharacter->m_szServerName, sizeof(pcsAgsdCharacter->m_szServerName));
		
		if (NULL == pszServerName)
			strncpy(pcsAgsdCharacter->m_szServerName, pcsThisServer->m_szWorld, AGSM_MAX_SERVER_NAME);
		else
			strncpy(pcsAgsdCharacter->m_szServerName, pszServerName, AGSM_MAX_SERVER_NAME);

		//m_pagsmCharacter->ReCalcCharacterFactors(pcsCharacter, FALSE);

		/*
#ifdef	__DB_OPERATION__
		// DB�� �����Ѵ�.
		stAgsmCharacterDB	stCharacterData;

		m_pagsmDBStream->InitChacterData(&stCharacterData);

		// ������ ĳ���� ����Ÿ�� �����Ѵ�.
		strcpy(stCharacterData.szName, pcsCharacter->m_szID);
		strcpy(stCharacterData.szTName, ((AgpdCharacterTemplate *) pcsCharacter->m_pcsCharacterTemplate)->m_szTName);
		strcpy(stCharacterData.szAccountName, pcsAgsdCharacter->m_szAccountID);
		strcpy(stCharacterData.szServerName, ((AgsdServerTemplate *) pcsThisServer->m_pcsTemplate)->m_szGroupName);

		stCharacterData.fPosX = pcsCharacter->m_stPos.x;
		stCharacterData.fPosY = pcsCharacter->m_stPos.y;
		stCharacterData.fPosZ = pcsCharacter->m_stPos.z;

		// ������ ������ ����Ÿ�� ������ ������.
		if (!m_pagsmDBStream->AddCharacterDB(&stCharacterData, pcsCharacter->m_lID))
		{
			m_pagpmCharacter->RemoveCharacter(pcsCharacter);
			return FALSE;
		}

#else
		*/

		/*
		SendPacketCreateCharacter(pcsCharacter, szAccountName);
		*/

		m_pagpmCharacter->UpdateInit(pcsCharacter);

		/*
		if (bIsPC)
			EnumCallback(AGSMCHARMM_CB_INSERT_CHARACTER_TO_DB, pcsCharacter, NULL);
		*/

		EnumCallbackCreateCharacter(pcsCharacter);

//#endif	//__DB_OPERATION__
	}
	else
	{
		m_pagpmCharacter->UpdateInit(pcsCharacter);

		EnumCallbackCreateCharacter(pcsCharacter);
	}

	//m_pagpmCharacter->UpdateInit(pcsCharacter);

	//EnumCallback(AGSMCHARMM_CB_CREATECHAR, pcsCharacter, NULL);

	//pcsCharacter->m_Mutex.Release();

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);

	pcsAgsdCharacter->m_bIsCreatedChar	= TRUE;

	return pcsCharacter;
}

/*
//		DeleteCharacter
//	Functions
//		- ������ �ִ� ĳ���� �����.
//			���� ĳ���Ͱ� �ִ��� �˻��Ѵ�.
//			�����Ŀ� DB�� �ݿ��Ѵ�.
//	Arguments
//		- szAccountName : ���� �̸�
//		- lCID : ���� ĳ����
//	Return value
//		- INT16 : ó�����
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::DeleteCharacter(CHAR *szAccountName, INT32 lCID)
{
	return DeleteCharacter(szAccountName, m_pagpmCharacter->GetCharacter(lCID));
}

INT16 AgsmCharManager::DeleteCharacter(CHAR *szAccountName, AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	return TRUE;
}
*/

//		PreSetting
//	Functions
//		- EnterGameWorld ���� ���� �ؾ��� ������ ���� ���⼭ �Ѵ�.
//	Arguments
//		- none
//	Return value
//		- INT16 : ó�����
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::PreSetting()
{
	return TRUE;
}

//		EnterGameWorld
//	Functions
//		- pCharacter ����Ÿ�� ���� ���ӿ� ����.
//	Arguments
//		- pCharacter : ���ӿ� �� ĳ���� ����
//	Return value
//		- INT16 : ó�����
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmCharManager::EnterGameWorld(AgpdCharacter *pcsCharacter, BOOL bIsCheckValidate)
{
	//STOPWATCH2(GetModuleName(), _T("EnterGameWorld"));

	// ĳ���� ���� ���� m_nCurrentStatus�� AGPDCHAR_STATUS_NORMAL�� �ٲ۴�.
	if (!pcsCharacter || pcsCharacter->m_unCurrentStatus == AGPDCHAR_STATUS_IN_GAME_WORLD)
		return FALSE;

	FLOAT	fOriginalPosY	= 0.0f;

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);

	if (bIsCheckValidate)
	{
		if (!pcsAgsdCharacter->m_bIsRecvAllDataFromLoginSvr ||
			pcsAgsdCharacter->m_dpnidCharacter == 0)
			return FALSE;

		pcsAgsdCharacter->m_strIPAddress.MemSetAll();
		strncpy(&pcsAgsdCharacter->m_strIPAddress[0], GetPlayerIPAddress(pcsAgsdCharacter->m_dpnidCharacter), 15);

		// ���� ����ǰ� ���� ���� Skill ���� ����Ÿ���� ���� �̷��� �����Ѵ�.
		INT32	lLevel	= 0;
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

		EnumCallback(AGSMCHARMM_CB_SET_CHAR_LEVEL, pcsCharacter, &lLevel);
		// ���� ����ǰ� ���� ���� Skill ���� ����Ÿ���� ���� �̷��� �����Ѵ�.

		if (m_pagpmCharacter->IsPC(pcsCharacter))
			m_pagsmFactors->ResetCharMaxPoint(&pcsCharacter->m_csFactor);

		fOriginalPosY	= pcsCharacter->m_stPos.y;
		pcsCharacter->m_stPos.y	= 100000;
		
		if (m_pagpmCharacter->IsPC(pcsCharacter))
		{
			if (pcsCharacter->m_stPos.x == 0.0f &&
				pcsCharacter->m_stPos.z == 0.0f)
			{
				m_pagpmEventBinding->GetBindingPositionForNewCharacter(pcsCharacter, &pcsCharacter->m_stPos);
			}
			else
			{
				EnumCallback(AGSMCHARMM_CB_CHECK_LOGIN_POSITION, pcsCharacter, NULL);

				// ���� ���� �ɸ� �����̸� ���� ������ ����������. 2008.03.25. steeple
				m_pagsmCharacter->CheckLoginPosition(pcsCharacter);
			}
		}

		// 2005.01.21. steeple
		m_pagpmCharacter->UpdateRegion(pcsCharacter);

		/*
		INT16	unCurrentStatus	= pcsCharacter->m_unCurrentStatus;
		pcsCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD;
		*/

		EnumCallback(AGSMCHARMM_CB_SET_CHARACTER_GAME_DATA, pcsCharacter, NULL);

		// character, item, etc.. ����� ����Ÿ�� �ϴ� ���� �����ش�.
		if (!m_pagsmCharacter->SendCharacterAllInfo(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter))
		{
			return FALSE;
		}

#ifdef _AREA_GLOBAL_
		if(pcsAgsdCharacter->m_dpnidCharacter > 0 && m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
		{
			m_pagsmBilling->CheckIn(pcsCharacter);
		}
#endif
#ifdef _AREA_KOREA_
		// PC Room üũ 2007.11.06. steeple
		if(pcsAgsdCharacter->m_dpnidCharacter > 0 && m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
		{
			CheckPCRoom(pcsCharacter);
		}
#endif

		//Title ���� ���� �����ش�.
		m_pagsmTitle->TitleListSend(pcsCharacter);
		m_pagsmTitle->TitleQuestListSend(pcsCharacter);

		// ������ �ִ� ��� �ش� ������ NPC�� ��� �����Ѵ�. 2005.06.27 by kelovon
		m_pagsmCharacter->SendRegionNPCToChar(m_pagpmCharacter->GetRealRegionIndex(pcsCharacter), pcsCharacter);

		/*
		pcsCharacter->m_unCurrentStatus	= unCurrentStatus;
		*/

		//m_pagsmCharacter->CheckRecvCharList(pcsCharacter->m_lID);

		// �� character�� ������ ����ɰŶ�� ��Ŷ�� �����ش�.
		if (!SendCharacterSettingOK(pcsCharacter, pcsAgsdCharacter->m_dpnidCharacter))
		{
			return FALSE;
		}
	}

	m_pagsmCharacter->ReCalcCharacterFactors(pcsCharacter, TRUE);

	if (bIsCheckValidate)
		AdjustCharMaxPoint(pcsCharacter);

	AgpdCharacter *pcsAddChar = m_pagsmCharacter->EnterGameWorld(pcsCharacter);
	if (!pcsAddChar)
		return FALSE;

	AgpdFactor	*pcsResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&pcsCharacter->m_csFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (pcsResultFactor)
	{
		m_pagpmFactors->CopyFactor(pcsResultFactor, &pcsCharacter->m_csFactor, TRUE, FALSE, AGPD_FACTORS_TYPE_CHAR_POINT);

		PVOID	pvPacketFactor = m_pagpmFactors->MakePacketFactors(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (pvPacketFactor)
		{
			m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);
		}

		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);
	}

	/*
	// ĳ���͸� AgpmCharacter�� �߰��Ѵ�. �� Instance ��ü�� �߰��ع�����. �ٽ� ���� �ʴ´�.
	AgpdCharacter *pcsAddChar = m_pagpmCharacter->AddCharacter(pCharacter);
	if (!pcsAddChar)
		return FALSE;

	//m_pagsmFactors->LoadFactor(pcsAddChar->m_szID, &pcsAddChar->m_csFactor);
	
	pcsAddChar->m_unCurrentStatus = AGPDCHAR_STATUS_NORMAL;

	EnumCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pcsAddChar, NULL);

	m_pagpmCharacter->UpdateInit(pcsAddChar);
	*/

	//m_pagpmCharacter->UpdateStatus(pcsAddChar, AGPDCHAR_STATUS_NORMAL);

	EnumCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pcsAddChar, NULL);

	// 2006.01.23. steeple
	// EnterGameWorld Callback �Ŀ� �θ��� �ɷ� ����.
	// �׷�������, Cash Item Skill �� ����� �����Ѵ�.
	//
	// 2005.12.14. steeple
	// AgsmItem ���� �ʿ��� EnterGameWorld �� ���ش�. AgsmItem �� ���� ����̶� �̷��� ���� �ҷ���.
	m_pagsmItem->EnterGameWorld(pcsAddChar);

	//////////////////////////////////////////////////////////////////////////
	// save skill - arycoat 2008.7.
	m_pagsmSkill->RecastSaveSkill(pcsCharacter);

	// ���⼭ �ٽ� PassiveCastSkill�� ���ش�.
	m_pagsmSkill->ReCastAllPassiveSkill((ApBase*)pcsCharacter);

	m_pagsmTitle->UsingTitleSendToClient(pcsCharacter);

	//JK_�нú꽺ų ǥ�� �ȵǴ� ����
	m_pagsmSkill->SendPassiveSkillAll(pcsCharacter, m_pagsmCharacter->GetCharDPNID(pcsCharacter));


	// ���� ���� �߰�
	//if ( AP_SERVICE_AREA_JAPAN == g_eServiceArea
	//	 && m_pagpmCharacter->IsPC(pcsAddChar)
	//	 && strlen(pcsAddChar->m_szID) )
	//{
	//	m_pagsmCharacter->GGSendInitialPacket( pcsAddChar );
	//}

	// �߱� �ߵ� �����ý��� �߰�
#ifdef _AREA_CHINA_
	if( _tcslen(pcsAgsdCharacter->m_szAccountID) > 0)
	{
		g_agsmGameholic.Online( pcsAgsdCharacter->m_szAccountID,
			pcsAddChar->m_szID,
			pcsAgsdCharacter->m_strIPAddress.begin());
	}
#endif

	if(m_pAgpmConfig->IsTestServer() &&
	   m_pagpmCharacter->IsPC(pcsAddChar) && _tcslen(pcsAddChar->m_szID) > 0)
	{
		INT32 lLevel = m_pagpmCharacter->GetLevel(pcsAddChar);
		INT64 lExp = m_pagpmCharacter->GetExp(pcsAddChar);

		if(lLevel == 1 && lExp == 0)
		{
			m_pagsmCharacter->SetCharacterLevel(pcsAddChar, m_pAgpmConfig->GetStartLevel(), FALSE);
			m_pagpmCharacter->AddMoney(pcsAddChar, m_pAgpmConfig->GetStartGheld());
			m_pagpmCharacter->UpdateMoney(pcsAddChar);
			m_pagpmCharacter->UpdateCharismaPoint(pcsAddChar, m_pAgpmConfig->GetStartCharisma() );
		}
	}

	// Go (Game Operator) üũ 2007.07.29. steeple
	if(pcsAddChar->m_pcsCharacterTemplate->m_lID == 886)
	{
		m_pagpmCharacter->UpdateSetSpecialStatus(pcsAddChar, AGPDCHAR_SPECIAL_STATUS_GO);
	}

	if (bIsCheckValidate && pcsAgsdCharacter->m_dpnidCharacter != 0)
	{
		pcsCharacter->m_stPos.y	= fOriginalPosY;

		// Relay, Backup Server�� pcsCharacter�� public data�� �Ѱ��ش�.
		//////////////////////////////////////////////////////////////////////////////////////
		AgsdServer	*pcsRelayServer	= m_pAgsmServerManager->GetRelayServer();
		if (pcsRelayServer)
		{
			//m_pagsmCharacter->SendPacketAllDBData(pcsCharacter, pcsRelayServer->m_dpnidServer);

			//////////////////////////////////////////////////////////////////////////
			//
			// �Ʒ� �ڵ� �ּ� ó��. 2004.07.20. steeple
			//
			// Admin �� ��쿡�� Relay, Backup Server ������ AddCharacter �ؾ� �Ѵ�. - 2004.03.04 steeple
			//if(m_pagpmAdmin->IsAdminCharacter(pcsCharacter))
			//{
			//	m_pagsmCharacter->SendCharacterAllInfo(pcsCharacter, pcsRelayServer->m_dpnidServer);
			//}
		}

		/*
		//�Ʒ��ڵ���� �α��� ������ �Ѱܾ��Ѵ�.
		//ControlServer������ ������.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_MASTER_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetThisServer() );
		//Deal Server������ ������.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_DEAL_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetDealServer() );
		//Recruit Server������ ������.
		m_pagsmSystemInfo->SendServerInfo( AGSMSERVER_TYPE_RECRUIT_SERVER, pcsCharacter->m_lID, m_pAgsmServerManager->GetRecruitServer() );
		*/

		// Login Log �� �����. - 2004.05.02. steeple
		WriteLoginLog(pcsCharacter);
	}

	return TRUE;
}

// 2007.11.06. steeple
// PC Room Module �� �ʱ�ȭ �Ѵ�.
BOOL AgsmCharManager::InitPCRoomModule()
{
#ifdef _AREA_KOREA_
	GetGameEnv().InitEnvironment();

	BOOL bTest = FALSE;
	if(GetGameEnv().IsAlpha())
		bTest = TRUE;

	int iResult = m_csHanIPCheck.AuHanIPCheckInit(bTest);

	if( m_csHangameTPack )
	{
		AgsdServer2* pGameServer = m_pAgsmServerManager->GetThisServer();
		if(!pGameServer)
			return FALSE;
		
		if(!m_csHangameTPack->Create(pGameServer->m_lServerID))
			return FALSE;
	}
#endif //_AREA_KOREA_

	return TRUE;
}

// 2007.11.06. steeple
// PC Room IP üũ�Ѵ�.
BOOL AgsmCharManager::CheckPCRoom(AgpdCharacter* pcsCharacter)
{
#ifdef _AREA_KOREA_
	if(!pcsCharacter)
		return FALSE;
	if(GetGameEnv().IsDebugTest())
		return TRUE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgpdBillInfo* pcsAttachBillInfo = m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(!pcsAttachBillInfo)
		return FALSE;

	int iResult = -1;
//	if(GetGameEnv().IsAlpha())
//		iResult = m_csHanIPCheck.AuHanIPCheckRequest(_T("121.172.155.159"), pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);
//	else
//	iResult = m_csHanIPCheck.AuHanIPCheckRequest(&pcsAgsdCharacter->m_strIPAddress[0], pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);

	printf("CharName: %s, CRM : %s, Grade : %s\n", pcsCharacter->m_szID, pcsAgsdCharacter->m_szCRMCode, pcsAgsdCharacter->m_szGrade);
	if(iResult == 0)
	{
		pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_S;
	}

	if( m_csHangameTPack )
	{
		BOOL bTPack = m_csHangameTPack->CheckIn(pcsAgsdCharacter->m_szAccountID, &pcsAgsdCharacter->m_strIPAddress[0]);
		if(bTPack)
		{
			pcsAttachBillInfo->m_ulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;
		}
	}

	if( pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_S || pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK )
	{
		m_pagpmBillInfo->UpdateIsPCRoom(pcsCharacter, TRUE);
	}
	m_pagsmBillInfo->SendCharacterBillingInfo(pcsCharacter);

#endif
	return TRUE;
}

BOOL AgsmCharManager::CheckOutPCRoom(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

#ifdef _AREA_KOREA_
	if( m_csHangameTPack )
		m_csHangameTPack->CheckOut(pcsAgsdCharacter->m_szAccountID);
#endif // _AREA_KOREA_

	return TRUE;
}

BOOL AgsmCharManager::SetCallbackCreateChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CREATECHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::EnumCallbackCreateCharacter(AgpdCharacter * pcsAgpdCharacter)
{
	return EnumCallback(AGSMCHARMM_CB_CREATECHAR, pcsAgpdCharacter, NULL);
}

BOOL AgsmCharManager::SetCallbackDeleteChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DELETECHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackLoadChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_LOADCHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackUnLoadChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_UNLOADCHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackEnterGameWorld(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_ENTERGAMEWORLD, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackConnectedChar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CONNECTED_CHAR, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackInsertCharacterToDB(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_INSERT_CHARACTER_TO_DB, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackAddCharResult(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_ADDCHAR_RESULT, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackSetCharLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_SET_CHAR_LEVEL, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackDisconnectCharacter(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DISCONNECT_CHARACTER, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackDeleteComplete(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_DELETE_COMPLETE, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackSetCharacterGameData(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_SET_CHARACTER_GAME_DATA, pfCallback, pClass);
}

BOOL AgsmCharManager::SetCallbackCheckLoginPosition(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCHARMM_CB_CHECK_LOGIN_POSITION, pfCallback, pClass);
}

BOOL AgsmCharManager::InitServer(UINT32 ulStartValue, UINT32 ulServerFlag, INT16 nSizeServerFlag, INT32 lRemoveIDQueueSize)
{
	//m_csGenerateAID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag);
	m_csGenerateCID.Initialize(ulStartValue, ulServerFlag, nSizeServerFlag, lRemoveIDQueueSize, TRUE);

	return TRUE;
}


BOOL AgsmCharManager::CBRemoveCharFromMap(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
		
	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
	{	
		AgsdCharacter *pAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if (NULL != pAgsdCharacter && FALSE == pAgsdCharacter->m_bIsNotLogout)
			pThis->WriteLogoutBankLog(pcsCharacter);
	}
	
	return TRUE;
}

//		CBRemoveCharacter
//	Functions
//		- player character�� �ʿ��� ���ð�� DB�� ����Ÿ�� ���� �����Ѵ�.
//	Arguments
//		- pData : pstAgsmDBOperationResult
//		- pClass : this module pointer
//	Return value
//		- BOOL : ó�����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharManager::CBRemoveCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
		
	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;
	
	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
	{
#ifdef _AREA_GLOBAL_
		if( pThis->m_pagsmBilling )
			pThis->m_pagsmBilling->CheckOut(pcsCharacter);
#endif
#ifdef _AREA_KOREA_
		pThis->CheckOutPCRoom(pcsCharacter);
#endif
		pThis->WriteLogoutLog(pcsCharacter);
	}
	
	return TRUE;
}

BOOL AgsmCharManager::CBRemoveCharacterID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager		*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter		*pcsCharacter		= (AgpdCharacter *)		pData;

	pThis->m_csGenerateCID.AddRemoveID(pcsCharacter->m_lID);

	return TRUE;
}

BOOL AgsmCharManager::CBDeleteCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	return TRUE;
}

BOOL AgsmCharManager::CBCompleteRecvCharFromLoginServer(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager	*pThis			= (AgsmCharManager *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;

	return (BOOL) pThis->EnterGameWorld(pcsCharacter);
}

BOOL AgsmCharManager::CBSendAuthKey(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCharManager	*pThis				= (AgsmCharManager *)	pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)		pData;
	UINT32			ulNID				= *(UINT32 *)			pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

	if (pcsAgsdCharacter->m_bIsValidAuthKey)
		return pThis->SendAuthKeyPacket(pcsCharacter->m_szID, pcsAgsdCharacter->m_lAuthKey, ulNID);

	return TRUE;
}

BOOL AgsmCharManager::CBGetNewCID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCharManager	*pThis		= (AgsmCharManager *)	pClass;
	INT32			*plCID		= (INT32 *)				pData;

	*plCID						= pThis->m_csGenerateCID.GetID();

	return TRUE;
}

//		CBDisconnectDB
//	Functions
//		- db���� ������ �����. (��, ������ �״´�.) player character ���θ� DB�� �����ϱ� ���� Remove ��Ų��.
//			(Remove callback�� �޾Ƽ� DB �����Ѵ�.)
//	Arguments
//		- pData : NULL
//		- pClass : this module pointer
//		- pCustData : NULL
//	Return value
//		- BOOL : ó�����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmCharManager::CBDisconnectDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgsmCharManager	*pThis = (AgsmCharManager *) pClass;

	INT32 lIndex = 0;

	AgpdCharacter *pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex);

	while (pcsCharacter)
	{
		AgsdCharacter	*pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

		if (pcsAgsdCharacter->m_dpnidCharacter != 0)
		{
			pThis->m_pagpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
		}

		pcsCharacter = pThis->m_pagpmCharacter->GetCharSequence(&lIndex);
	}

	return TRUE;
}

BOOL AgsmCharManager::SendCharacterSettingOK(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter)
		return FALSE;

	BOOL	bRetval = TRUE;

#ifdef	__ATTACH_LOGINSERVER__

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_SETTING_CHARACTER_OK;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											&pcsCharacter->m_lID,
											NULL,
											NULL);

	if (!pvPacket)
		return FALSE;

	bRetval	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

#endif	//__ATTACH_LOGINSERVER__

	return bRetval;
}

BOOL AgsmCharManager::SendCharacterLoadingPosition(AgpdCharacter *pcsCharacter, UINT32 ulNID)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_CHARACTER_POSITION;

	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											NULL,
											&pcsCharacter->m_lID,
											&pcsCharacter->m_stPos,
											NULL);

	if (!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bSendResult;
}

BOOL AgsmCharManager::AdjustCharMaxPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdCharacterTemplate	*pcsTemplate	= pcsCharacter->m_pcsCharacterTemplate;
	if (!pcsTemplate)
		return FALSE;

	INT32	lCharacterLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	if (lCharacterLevel < 1 || lCharacterLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AdjustCharMaxPoint"));

	INT32	lCurrentMaxHP	= 0;
	INT32	lCurrentMaxMP	= 0;
	INT32	lCurrentMaxSP	= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	INT64	llCurrentMaxExp	= m_pagpmFactors->GetMaxExp(&pcsCharacter->m_csFactor);

	// adjust current point with max point
	INT32	lCurrentHP		= 0;
	INT32	lCurrentMP		= 0;
	INT32	lCurrentSP		= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCurrentSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	INT64	llCurrentExp	= m_pagpmFactors->GetExp(&pcsCharacter->m_csFactor);

	if (lCurrentHP > lCurrentMaxHP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	}
	if (lCurrentMP > lCurrentMaxMP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	}
	if (lCurrentSP > lCurrentMaxSP)
	{
		m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, lCurrentMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}
	if (llCurrentExp > llCurrentMaxExp)
	{
		char strCharBuff[256] = { 0, };
		sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s] CurrentExp(%I64d) > MaxExp(%I64d)\n", pcsCharacter->m_szID, llCurrentExp, llCurrentMaxExp);
		AuLogFile_s("LOG\\AdjustCharPoint.txt", strCharBuff);

		m_pagpmFactors->SetExp(&pcsCharacter->m_csFactor, llCurrentMaxExp);
		
		if (pcsCharacter->m_csFactor.m_pvFactor[AGPD_FACTORS_TYPE_RESULT])
			m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT), llCurrentMaxExp);
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// Send Delete Complete
// Login Server -> Game Server �� ������.
BOOL AgsmCharManager::SendDeleteCompletePacket(CHAR* szCharName, UINT32 ulNID)
{
	if(!szCharName || !ulNID)
		return FALSE;

	INT16 nPacketLength	= 0;
	INT8 cOperation = AGSMCHARMANAGER_PACKET_OPERATION_DELETE_COMPLETE;
	PVOID pvPacket = m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
											&cOperation,
											NULL,
											NULL,
											szCharName,
											NULL,
											NULL,
											NULL);

	if(!pvPacket)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmCharManager::SendAuthKeyPacket(CHAR* szCharName, INT32 lAuthKey, UINT32 ulNID)
{
	if (!szCharName || ulNID == 0)
		return FALSE;

	INT16	nPacketLength	= 0;
	INT8	cOperation		= AGSMCHARMANAGER_PACKET_OPERATION_AUTH_KEY;

	PVOID	pvPacket		= m_csPacket.MakePacket(TRUE, &nPacketLength, AGSMCHARMANAGER_PACKET_TYPE,
													&cOperation,
													NULL,
													NULL,
													szCharName,
													NULL,
													NULL,
													&lAuthKey);

	if (!pvPacket)
		return FALSE;

	BOOL	bResult			= SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return	bResult;
}

//////////////////////////////////////////////////////////////////////////
// Log ���� - 2004.05.02. steeple
BOOL AgsmCharManager::WriteLoginLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);

	return m_pagpmLog->WriteLog_Login(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID, lCharTID, lLevel, llExp,
									pAgpdCharacter->m_llMoney, pAgpdCharacter->m_llBankMoney
									);
}

BOOL AgsmCharManager::WriteLogoutBankLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);
	
	// Bank Log
	AgpdItemADChar* pAgpdItemADChar = m_pagpmItem->GetADCharacter(pAgpdCharacter);
	if (m_pagsmItem && pAgpdItemADChar)
	{
		AgpdGridItem *pAgpdGridItem = NULL;
		INT32 lIndex = 0;
		for (pAgpdGridItem = m_pagpmItem->m_pagpmGrid->GetItemSequence(&pAgpdItemADChar->m_csBankGrid, &lIndex);
			pAgpdGridItem;
			pAgpdGridItem = m_pagpmItem->m_pagpmGrid->GetItemSequence(&pAgpdItemADChar->m_csBankGrid, &lIndex))
		{
			AgpdItem *pAgpdItem = m_pagpmItem->GetItem(pAgpdGridItem);
			if (pAgpdItem && pAgpdItem->m_pcsCharacter)
			{
				m_pagsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_BANK_LOGOUT,
										pAgpdItem->m_pcsCharacter->m_lID,
										pAgpdItem,
										pAgpdItem->m_nCount ? pAgpdItem->m_nCount : 1
										);
			}
		}
	}
	return TRUE;
}

BOOL AgsmCharManager::WriteLogoutLog(AgpdCharacter *pAgpdCharacter)
{
	if(!m_pagpmLog)
		return TRUE;

	if(!pAgpdCharacter)
		return FALSE;

	// ���� ������ �����. 2008.01.11. steeple
	if(m_pAgsmServerManager->GetThisServerType() != AGSMSERVER_TYPE_GAME_SERVER)
		return FALSE;

	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if(!pAgsdCharacter)
		return FALSE;

	INT32 lCharTID = ((AgpdCharacterTemplate*)pAgpdCharacter->m_pcsCharacterTemplate)->m_lID;
	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);
	
	// Play Log
	INT32 lPartyTime = (INT32) (m_pagsmCharacter->GetPartyPlayTimeMSec(pAgpdCharacter) / 1000 / 60);		// minute
	INT32 lSoloTime = (INT32) (m_pagsmCharacter->GetSoloPlayTimeMSec(pAgpdCharacter) / 1000 / 60);		// minute
	INT32 lKillPC = m_pagsmCharacter->GetKillMonCount(pAgpdCharacter);
	INT32 lKillMon = m_pagsmCharacter->GetKillPCCount(pAgpdCharacter);
	INT32 lDeadByPC = m_pagsmCharacter->GetDeadByMonCount(pAgpdCharacter);
	INT32 lDeadByMon = m_pagsmCharacter->GetDeadByPCCount(pAgpdCharacter);


	m_pagpmLog->WriteLog_Logout(0, &pAgsdCharacter->m_strIPAddress[0], pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
										pAgpdCharacter->m_szID, lCharTID, lLevel, llExp,
										pAgpdCharacter->m_llMoney, pAgpdCharacter->m_llBankMoney,
										lPartyTime, lSoloTime, lKillPC, lKillMon, lDeadByPC, lDeadByMon
										);

	// 2007.11.12. steeple
	// PC ���̴� �ƴϴ� �� �����.
	//if(m_pagpmBillInfo->IsPCBang(pAgpdCharacter))
	//{
		BOOL bTestServer = FALSE;
		if(GetGameEnv().IsAlpha())
			bTestServer = TRUE;

		INT64 llInvenMoney, llBankMoney;
		llInvenMoney = llBankMoney = 0;
		m_pagpmCharacter->GetMoney(pAgpdCharacter, &llInvenMoney);
		llBankMoney = m_pagpmCharacter->GetBankMoney(pAgpdCharacter);
#ifdef _AREA_KOREA_
		std::string strGameString = m_csHanIPCheck.GetGameString(bTestServer);
		m_pagpmLog->WriteLog_PCRoom(AuTimeStamp::GetCurrentTimeStamp(), strGameString.c_str(), pAgsdCharacter->m_szServerName,
									pAgsdCharacter->m_szAccountID, pAgsdCharacter->m_szServerName,
									m_pagsmCharacter->GetRealCharName(pAgpdCharacter->m_szID),
									pAgsdCharacter->m_ulConnectedTimeStamp, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_lLoginLevel, lLevel, llInvenMoney, llBankMoney,
									pAgsdCharacter->m_szCRMCode, pAgsdCharacter->m_szGrade);
#elif defined(_AREA_GLOBAL_)
		// webzen global service ���������α�
		// �α��� �α׾ƿ� ���� �ð� ���
		/*AgsdServer *pcsThisServer = m_pAgsmServerManager->GetThisServer();

		m_pagpmLog->WriteLogInOut(	pAgsdCharacter->m_szAccountID, 
									g_gbAuth.GetGameID(),
									pcsThisServer->m_lServerID,
									pAgsdCharacter->m_ulConnectedTimeStamp, //login�ð�.
									AuTimeStamp::GetCurrentTimeStamp(), //logout�ð�.
									m_pagsmCharacter->GetRealCharName(pAgpdCharacter->m_szID),
									lCharTID,
									lLevel,
									&pAgsdCharacter->m_strIPAddress[0]);
									*/
#endif
	//}

	return TRUE;
}
/*
BOOL AgsmCharManager::SendDummyPacket(INT32 lSize, INT32 lCount)
{
	PROFILE("AgsmCharManager::SendDummyPacket");

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacter("ġ�๬��");

	UINT32	ulNID	= m_pagsmCharacter->GetCharDPNID(pcsCharacter);

	CHAR	*szDummy	= (CHAR *) malloc(sizeof(CHAR) * lSize);

	while (1)
	{
		for (int i = 0; i < lCount; ++i)
		{
			SendPacket(szDummy, lSize, ulNID);
		}

		Sleep(2000);
	}

	return TRUE;
}
*/
#ifndef _AREA_CHINA_
BOOL AgsmCharManager::CBCheckPCRoomType(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCharManager *pThis			= (AgsmCharManager *) pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pData;
	INT32			*pulPCRoomType	= (INT32 *) pCustData;

	AgpdBillInfo *pcsAttachBillInfo	= pThis->m_pagpmBillInfo->GetADCharacter(pcsCharacter);
	if(pcsAttachBillInfo)
	{
		if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_S)
			*pulPCRoomType |= AGPDPCROOMTYPE_HANGAME_S;

		if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
			*pulPCRoomType |= AGPDPCROOMTYPE_HANGAME_TPACK;

		if(pcsAttachBillInfo->m_ulPCRoomType & AGPDPCROOMTYPE_WEBZEN_GPREMIUM)
			*pulPCRoomType |= AGPDPCROOMTYPE_WEBZEN_GPREMIUM;
	}

	return TRUE;
}
#endif