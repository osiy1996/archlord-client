// AgsmAdminCharacter.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 03. 29.
//
// Code ���� �ʹ� ������� ������ ���ؼ� �и�

#include "AgsmAdmin.h"
#include "AgsmTitle.h"
#include <list>

///////////////////////////////////////////////////////////////////////////////////////
// Search

// pstSearch �� Search �� ������, pcsAgpdCharacter �� Admin �� AgpdCharacter �̴�.
BOOL AgsmAdmin::SearchCharacter(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	// ���� ���� 2006.01.11. steeple
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	// 2004.08.16. steeple
	if(pstSearch->m_iField == 1)	// Account
	{
		// 2005.05.02. steeple
		// ���� Relay Search �����Ѵ�~~~
		return SearchCharacterByAccount2(pstSearch, pcsAgpdCharacter);
		//return SearchCharacterByAccount(pstSearch, pcsAgpdCharacter);
	}

	AgpdAdmin* pcsAgpdAdmin = m_pagpmAdmin->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdAdmin)
		return FALSE;

	// Admin �� DPNID �� �ް�
	DPNID dpnAdminID = m_pagsmCharacter->GetCharDPNID(pcsAgpdAdmin->m_lAdminCID);
	dpnAdminID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstSearch->m_lObjectCID != 0)
	{
		// Lock �۾� - 2004.01.16.
		if(pcsAgpdCharacter->m_lID == pstSearch->m_lObjectCID)
			pcsSearchCharacter = pcsAgpdCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSearch->m_lObjectCID);
	}
	else
	{
		if(strcmp(pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName) == 0)
			pcsSearchCharacter = pcsAgpdCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSearch->m_szSearchName);
	}

	if(!pcsSearchCharacter)// || m_pagsmCharacter->GetCharDPNID(pcsSearchCharacter) == 0)
	{
		// ������ ���� ���� ������ �� �ִ�.
		// DB ���� ã�ƺ���.
		//SearchCharacterFromDB(pstSearch->m_szSearchName, pstSearch->m_lObjectCID, pcsAgpdCharacter);
		SearchCharacterDB(pstSearch, pcsAgpdCharacter);
		return TRUE;
	}

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsSearchCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// ��� �ʿ���� -_-;;
	// 2004.03.15. - ServerID �� üũ�ؼ� �ٸ� ������ �ִ� ĳ���Ͷ�� �ش� ������ �˻� ��Ŷ�� ������.
	//{
	//	if(pcsAgsdCharacter->m_ulServerID != GetThisServerID())
	//	{
	//		SendSearchOtherServer(pstSearch, pcsAgpdCharacter->m_lID, GetGameServerNID(pcsAgsdCharacter->m_ulServerID));

	//		if(pcsSearchCharacter != pcsAgpdCharacter)
	//			pcsSearchCharacter->m_Mutex.Release();

	//		return TRUE;
	//	}
	//}

	//////////////////////////////////////////////////////////////////////////
	// ������� �°��̶��, ���� �������� ��Ʈ�� �ϴ� ĳ�����̴�. ������ ������.

	// �⺻������ ������.
	m_pagsmCharacter->SendCharacterAllInfo(pcsSearchCharacter, dpnAdminID);

	// Item ���� ���� �� ������.
	SendCharAllItemUseCustom(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// Search Result �� ������.
	SendSearchResult(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// �߰������� ������.
	SendSearchCharDataSub(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// Party ������ ������. 2004.03.22.
	//SendSearchCharParty(pcsSearchCharacter, pcsAgpdCharacter);

	// Char Guild ID �� ������. 2005.04.25.
	m_pagsmGuild->SendCharGuildData(m_pagpmGuild->GetJoinedGuildID(pcsSearchCharacter), pcsSearchCharacter->m_szID, NULL,
									pcsSearchCharacter->m_lID, dpnAdminID);

	// Money �� ���� �� ������. 2005.05.18.
	SendSearchCharMoney(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);

	// 2005.09.14.
	// ���� ���� ���� ������ Bank ���� �˾ƾ� �ϴ� ���� ��û�Ѵ�.
	SearchAccountBank(pcsAgsdCharacter->m_szAccountID, pcsAgpdCharacter);

	// ������� ���� �����ش�. 2006.12.05. steeple
	SendCharWantedCriminal(pcsSearchCharacter->m_szID, pcsAgpdCharacter->m_lID, dpnAdminID);

	//����Ʈ ������ ���� �����ش�.
	SendCharQuestInfo(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);
#ifndef _AREA_CHINA_
	//Ÿ��Ʋ ������ ���� �����ش�.
	SendCharTitleInfo(pcsSearchCharacter, pcsAgpdCharacter->m_lID, dpnAdminID);
#endif

	if(pcsAgpdCharacter != pcsSearchCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return TRUE;
}

// DB �� �˻��� ������.
// pcsAgpdCharacter �� Admin �̴�. ������~
// ����. 2004.03.15 - Game Server ��� Relay ������ ��Ŷ�� ������.
BOOL AgsmAdmin::SearchCharacterFromDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	//if(!m_pagsmAdminDB)
	//	return FALSE;

	//if(IsGameServer())
	//{
	//	SendSearchOtherServer(pstSearch, pcsAgpdCharacter->m_lID, GetRelayServerNID());
	//	return TRUE;
	//}

	//stAgsdAdminSearchMatch stMatch;
	//ZeroMemory(&stMatch, sizeof(stMatch));

	//stMatch.m_lAdminCID = pcsAgpdCharacter->m_lID;
	//stMatch.m_lObjectCID = pstSearch->m_lObjectCID;
	//if(strlen(pstSearch->m_szSearchName) <= AGPACHARACTER_MAX_ID_STRING)
	//	strncpy(stMatch.m_szName, pstSearch->m_szSearchName, AGPACHARACTER_MAX_ID_STRING);
	
	// �̿� ���� �ݹ��� �̹� ����� ���´�.
	// AgsmAdminDB ���� ��� �۾��� �����ϸ� Callback �ѹ� �ҷ��ش�.
	//m_pagsmAdminDB->SearchCharacter(&stMatch);

	return TRUE;
}

// AgsmAdminDB �� ���ؼ� �Ҹ��� �ȴ�. - EnumCallback ����..
// DB �˻��� ������ �������� �� �Ҹ��� �ǹǷ�, ���� ���� ������ ������ �ȴ�.
// ���� lCID �� �˻��� Admin �� CID �� ���� �ȴ�.
BOOL AgsmAdmin::SearchCharacterResultDB(CHAR* szName, INT32 lCID)
{
	if(!m_pagsmCharacter)
		return FALSE;

	AgpdCharacter* pcsAgpdCharacter = NULL;

	// �̸��� �ִٸ� �̸����� �˻�
	if(szName)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(szName);

	// �� ã���� �� �� ��ģ��.
	if(!pcsAgpdCharacter)
		return FALSE;

	DPNID dpnAdminID = m_pagsmCharacter->GetCharDPNID(lCID);

	m_pagsmCharacter->SendCharacterAllInfo(pcsAgpdCharacter, dpnAdminID);
	SendSearchResult(pcsAgpdCharacter, lCID, dpnAdminID);
	SendSearchCharDataSub(pcsAgpdCharacter, lCID, dpnAdminID);

	return TRUE;
}

// 2004.08.16.
// SearchCharacter �Լ����� Flag �� ���� Account �˻��̸� �� �Լ��� ���� �ȴ�.
BOOL AgsmAdmin::SearchCharacterByAccount(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_SEARCH_ACCOUNT, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);

	return TRUE;
}

// 2005.05.02. steeple
// Relay ������ �ٷ� �����ش�.
BOOL AgsmAdmin::SearchCharacterByAccount2(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;

	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);

	return TRUE;
}

// 2005.05.18. steeple
// Relay ������ �ٷ� �����ش�.
BOOL AgsmAdmin::SearchCharacterDB(stAgpdAdminSearch* pstSearch, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstSearch || !pcsAgpdCharacter)
		return FALSE;
	if(!pstSearch->m_szSearchName || strlen(pstSearch->m_szSearchName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_CHARACTER, pcsAgpdCharacter->m_szID, pstSearch->m_szSearchName);
	return TRUE;
}
// 2005.09.14. steeple
// Relay ������ �ٷ� �����ش�.
BOOL AgsmAdmin::SearchAccountBank(CHAR* szAccountName, AgpdCharacter* pcsAgpdCharacter)
{
	if(!szAccountName || !pcsAgpdCharacter)
		return FALSE;

	if(strlen(szAccountName) == 0)
		return FALSE;

	EnumCallback(AGSMADMIN_CB_RELAY_SEARCH_ACCOUNT_BANK, pcsAgpdCharacter->m_szID, szAccountName);

	return TRUE;
}

// 2005.05.02. steeple
// AgsmRelay ���� �Ҹ���.
BOOL AgsmAdmin::ProcessSearchAccountDB(CHAR* szAdminCharName, CHAR* szAccountName, stAgpdAdminSearchResult* pstSearchResult)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szAccountName || !pstSearchResult)
		return FALSE;

	// ������ ã��
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
	if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	if(m_pagpmCharacter->GetCharacter(pstSearchResult->m_szCharName))
		pstSearchResult->m_lStatus = 1;

	// ��~ Ŀ���� ��Ŷ �� �������~~~
	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, pstSearchResult, sizeof(stAgpdAdminSearchResult), lCID, ulNID);
	return TRUE;
}

// 2005.05.06. steeple
// AgsmRelay ���� �Ҹ���.
BOOL AgsmAdmin::ProcessSearchCharacterDB(CHAR* szAdminCharName, CHAR* szCharName, stAgpdAdminSearchResult* pstSearchResult,
										stAgpdAdminCharDataBasic* pstBasic, stAgpdAdminCharDataStatus* pstStatus,
										stAgpdAdminCharDataStat* pstStat, stAgpdAdminCharDataMoney* pstMoney,
										CHAR* szTreeNode, CHAR* szProduct)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szCharName)
		return FALSE;

	// ������ ã��
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	// Ŀ���� ��Ŷ�� ������� ������. Result �� ���߿� ������.
	if(pstBasic)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_BASIC, pstBasic, sizeof(stAgpdAdminCharDataBasic), lCID, ulNID);
	if(pstStatus)
	{
		pstStatus->m_llMaxExp = m_pagpmCharacter->GetLevelUpExp(pstStatus->m_lLevel);
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STATUS, pstStatus, sizeof(stAgpdAdminCharDataStatus), lCID, ulNID);
	}
	if(pstStat)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_STAT, pstStat, sizeof(stAgpdAdminCharDataStat), lCID, ulNID);
	if(pstMoney)
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY, pstMoney, sizeof(stAgpdAdminCharDataMoney), lCID, ulNID);

	if((szTreeNode && strlen(szTreeNode) > 0) || (szProduct && strlen(szProduct) > 0))
	{
		stAgpdAdminSkillString stSkill;
		memset(&stSkill, 0, sizeof(stSkill));
		_tcsncpy(stSkill.m_szCharName, szCharName, AGPDCHARACTER_NAME_LENGTH);
		_tcsncpy(stSkill.m_szTreeNode, szTreeNode, AGPMADMIN_MAX_SKILL_STRING_LENGTH);
		_tcsncpy(stSkill.m_szProduct, szProduct, AGPMADMIN_MAX_SKILL_STRING_LENGTH);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SKILL, &stSkill, sizeof(stSkill), lCID, ulNID);
	}

	if(pstSearchResult)
	{
		stAgpdAdminCharDataSub stSub;
		memset(&stSub, 0, sizeof(stSub));

		strncpy(stSub.m_szAccName, pstSearchResult->m_szAccName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(stSub.m_szCharName, szCharName, AGPACHARACTER_MAX_ID_STRING);
		strncpy(stSub.m_szLastIP, pstSearchResult->m_szIP, AGPMADMIN_MAX_IP_STRING);

		if(pstBasic)
			pstSearchResult->m_lTID = pstBasic->m_lTID;

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_SUB, &stSub, sizeof(stSub), lCID, ulNID);
		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, pstSearchResult, sizeof(stAgpdAdminSearchResult), lCID, ulNID);

		// ������� ���� �����ش�. 2006.12.05. steeple
		SendCharWantedCriminal(szCharName, lCID, ulNID);
	}

	return TRUE;
}

// 2005.05.16. steeple
// AgsmRelay ���� �θ���.
// 2005.06.08. steeple
// Bank Item �� ����� ���� szTargetName �� AccountName �� �´�.
BOOL AgsmAdmin::ProcessSearchCharItemDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminItem* pstItem)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szTargetName || !pstItem)
		return FALSE;

	// ������ ã��
	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT16 nAdminLevel = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		nAdminLevel = m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;

	// ĳ�� �������� �� ���� 5 �� ���� ������ �������� �ʴ´�.
	AgpdItemTemplate* pcsItemTemplate = m_pagpmItem->GetItemTemplate(pstItem->m_lTID);
	if(pcsItemTemplate && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType) && nAdminLevel < AGPMADMIN_LEVEL_4)
		return FALSE;

	// Custom Packet �����ش�.
	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, pstItem, sizeof(stAgpdAdminItem), lCID, ulNID);

	return TRUE;
}

BOOL AgsmAdmin::ProcessSearchCharTitleDB(CHAR* szAdminCharName, CHAR* szTargetName, stAgpdAdminCharTitle* pstTitle)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!szAdminCharName || !szTargetName || !pstTitle)
		return FALSE;

	INT32 lCID = 0;
	UINT32 ulNID = 0;
	INT16 nAdminLevel = 0;
	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacterLock(szAdminCharName);
    if(pcsAdminCharacter)
	{
		lCID = pcsAdminCharacter->m_lID;
		ulNID = m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter);
		nAdminLevel = m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter);
		pcsAdminCharacter->m_Mutex.Release();
	}
	else
		return FALSE;


	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, pstTitle, sizeof(stAgpdAdminCharTitle), lCID, ulNID);

	return TRUE;
}









//////////////////////////////////////////////////////////////////////////
// Character Operation Processing

// Move ó��
BOOL AgsmAdmin::ProcessCharacterMove(stAgpdAdminCharDataPosition* pstPosition, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstPosition || !pcsAgpdCharacter)
		return FALSE;

	if(!pstPosition->m_szName || strlen(pstPosition->m_szName) > AGPACHARACTER_MAX_ID_STRING)
		return FALSE;

	if(m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) == FALSE)
		return FALSE;

	// ���� ���� üũ. 2006.01.11. steeple
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	AgpdCharacter* pcsMoveCharacter = NULL;
	if(strcmp(pcsAgpdCharacter->m_szID, pstPosition->m_szName) == 0)
		pcsMoveCharacter = pcsAgpdCharacter;
	else
		pcsMoveCharacter = m_pagpmCharacter->GetCharacterLock(pstPosition->m_szName);

	if(!pcsMoveCharacter)
	{
		// ������ ������
		// �ٸ� ������ �ִ� �� Ȯ���ϰ�, ������ �������� �̵� ��Ŷ�� ������.
		// �̵����� ������, DB �� �ٷ� ����.
		//
		//
		//

		return TRUE;
	}

	AuPOS auPos;
	memset(&auPos, 0, sizeof(auPos));
	BOOL bValid = FALSE;
	
	if(strlen(pstPosition->m_szTargetName) > 0)
	{
		// 2004.01.16. Move ĳ����, Target ĳ���� �޶����
		if(strcmp(pcsMoveCharacter->m_szID, pstPosition->m_szTargetName) != 0)
		{
			// 2004.04.05. - Taget ĳ���� Admin �̸� �̹� Lock �� �Ǿ� �ִ� �����̴�. -0-
			// ������ ���ɷ��� �˰� �Ǿ���.... TT
			AgpdCharacter* pcsTargetCharacter = NULL;
			if(strcmp(pcsAgpdCharacter->m_szID, pstPosition->m_szTargetName) == 0)
				pcsTargetCharacter = pcsAgpdCharacter;
			else
				pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(pstPosition->m_szTargetName);

			if(pcsTargetCharacter)
			{
				bValid = TRUE;

				auPos.x = pcsTargetCharacter->m_stPos.x;
				auPos.y = pcsTargetCharacter->m_stPos.y;
				auPos.z = pcsTargetCharacter->m_stPos.z;

				if(pcsTargetCharacter != pcsAgpdCharacter)
					pcsTargetCharacter->m_Mutex.Release();
			}
		}
	}
	else
	{
		bValid = TRUE;

		auPos.x = (FLOAT)pstPosition->m_fX;
		auPos.y = (FLOAT)pstPosition->m_fY;
		auPos.z = (FLOAT)pstPosition->m_fZ;
	}

	if(bValid && m_pagpmCharacter->UpdatePosition(pcsMoveCharacter, &auPos, TRUE, TRUE))
	{
		OutputDebugString("AgsmAdmin::ProcessCharacterMove(...) Move-Teleport Success!!!\n");
	}

	if(pcsMoveCharacter != pcsAgpdCharacter)
		pcsMoveCharacter->m_Mutex.Release();

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Edit ó��
BOOL AgsmAdmin::ProcessCharacterEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmFactors || !m_pagsmFactors || !m_pagsmDeath)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter) == FALSE)
		return FALSE;

	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_2)
		return FALSE;

	AgpdCharacter* pcsEditCharacter = NULL;
	if(strcmp(pcsAgpdCharacter->m_szID, pstCharEdit->m_szCharName) == 0)
		pcsEditCharacter = pcsAgpdCharacter;
	else
		pcsEditCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);

	char strCharBuff[256] = { 0, };
	sprintf_s(strCharBuff, sizeof(strCharBuff), "[CHARACTEREDIT] %s(%s) : Operation(%d), Target(%s), NewValueInt(%d), NewValueFloat(%f), NewValueINT64(%I64d)\n",
		pcsAgpdCharacter->m_szID,
		pcsAgsdCharacter->m_szAccountID,
		pstCharEdit->m_lEditField,
		pstCharEdit->m_szCharName,
		pstCharEdit->m_lNewValueINT,
		pstCharEdit->m_lNewValueFloat,
		pstCharEdit->m_llNewValueINT64);
	AuLogFile_s(AGSMADMIN_COMMAND_LOG_FILE, strCharBuff);
	
	if(!pcsEditCharacter)
	{
		// ������ ������ DB �� ���� �����Ѵ�.
		// 2005.05.31. steeple
		// ���� � �������. -_-;;
		if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB || pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_BANK_MONEY_DB)
			return ProcessCharacterEditToDB(pstCharEdit, pcsAgpdCharacter);
		else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB2)
			return ProcessCharacterEditToDB2(pstCharEdit, pcsAgpdCharacter);
		else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_CHARISMA_POINT_DB)
			return ProcessCharismaEditToDB(pstCharEdit, pcsAgpdCharacter);
		else
			return FALSE;
	}

	// DB �� ���� �� ���� ������ ��ǥ�� �����ؾ� �ϹǷ� ����3 üũ�� ���⼭ �Ѵ�.
	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
		return FALSE;

	AgpdFactor* pcsResultFactor = (AgpdFactor*)m_pagpmFactors->GetFactor(&pcsEditCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsResultFactor)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Point
	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	// Max Point
	AgpdFactorCharPoint* pcsFactorCharPointMax = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT_MAX);
	if(!pcsFactorCharPointMax)
	{
		if(pcsEditCharacter != pcsAgpdCharacter)
			pcsEditCharacter->m_Mutex.Release();

		return FALSE;
	}

	BOOL bResult = FALSE;
	PVOID pvFactorPacket = NULL;

	// Edit �׸� ��� �� ó���ؾ� �Ѵ�. -0-
	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_HP:	// HP ����
		{
			INT32 lNewHP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP])
				lNewHP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP];
			else
				lNewHP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, lNewHP, 0, 0);
			
			break;
		}

		case AGPMADMIN_CHAREDIT_MP:	// MP ����
		{
			INT32 lNewMP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP])
				lNewMP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP];
			else
				lNewMP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, 0, lNewMP, 0);

			break;
		}

		case AGPMADMIN_CHAREDIT_SP:	// SP ����
		{
			INT32 lNewSP;
			if(pstCharEdit->m_lNewValueINT > pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP])
				lNewSP = pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP];
			else
				lNewSP = pstCharEdit->m_lNewValueINT;

			m_pagsmFactors->UpdateCharPoint(&pcsEditCharacter->m_csFactor, &pvFactorPacket, 0, 0, lNewSP);

			break;
		}

		case AGPMADMIN_CHAREDIT_LEVEL:	// Level ����
		{
			INT32 lCurrentLevel = m_pagpmCharacter->GetLevel(pcsEditCharacter);
			INT32 lNewLevel = pstCharEdit->m_lNewValueINT;

			if(lNewLevel >= AGPMCHAR_MAX_LEVEL)
				lNewLevel = AGPMCHAR_MAX_LEVEL - 1;

			if(lNewLevel == lCurrentLevel)
				break;

			m_pagsmCharacter->SetCharacterLevel(pcsEditCharacter, lNewLevel, FALSE);
			WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITLEVEL, pcsAgpdCharacter, pcsEditCharacter);
			break;
		}

		case AGPMADMIN_CHAREDIT_EXP:	// EXP ����
			bResult = m_pagsmDeath->AddBonusExpToChar(pcsEditCharacter, NULL, pstCharEdit->m_llNewValueExp);
			WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITEXP, pcsAgpdCharacter, pcsEditCharacter);
			break;

		case AGPMADMIN_CHAREDIT_MONEY_INV:	// Inventory �� �ִ� Ghelld ����
			bResult = m_pagpmCharacter->SetMoney(pcsEditCharacter, pstCharEdit->m_llNewValueINT64);
			WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsEditCharacter);
			break;
		case AGPMADMIN_CHAREDIT_MONEY_BANK:
			bResult = m_pagpmCharacter->SetBankMoney(pcsEditCharacter, pstCharEdit->m_llNewValueINT64);
			WriteLog_Gheld(AGPDLOGTYPE_GHELD_GMEDIT, pcsAgpdCharacter, pcsEditCharacter);
			break;
		case AGPMADMIN_CHAREDIT_CRIMINAL_POINT:	// Criminal Point ����.
			m_pagpmCharacter->UpdateCriminalStatus(pcsEditCharacter, (AgpdCharacterCriminalStatus)pstCharEdit->m_lNewValueINT);
			break;

		case AGPMADMIN_CHAREDIT_MURDERER_POINT:	// Murderer Point ����.
			bResult = m_pagpmCharacter->UpdateMurdererPoint(pcsEditCharacter, pstCharEdit->m_lNewValueINT);
			if(bResult && m_pagsmRemission)
			{
				// ����� �����ش�.
				m_pagsmRemission->SendRemitResult(pcsEditCharacter, AGPMREMISSION_TYPE_INDULGENCE, AGPMREMISSION_RESULT_SUCCESS);
			}
			// �α� �����.
			break;

		case AGPMADMIN_CHAREDIT_CRIMINAL_TIME:	// ���� Criminal Time ����.
			if(pstCharEdit->m_lNewValueINT > 0 && pstCharEdit->m_lNewValueINT < 7 * 24 * 60)
			{
				pcsEditCharacter->m_unCriminalStatus = AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED;
				m_pagpmCharacter->SetLastCriminalTime(pcsEditCharacter, pstCharEdit->m_lNewValueINT * 60);
				m_pagsmCharacter->SendPacketNearCharCriminalStatus(pcsEditCharacter);
			}
			else
			{
				bResult = m_pagpmCharacter->UpdateCriminalStatus(pcsEditCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
				pcsEditCharacter->m_lLastUpdateCriminalFlagTime = 0;
				m_pagsmCharacter->SendPacketNearCharCriminalStatus(pcsEditCharacter);
			}
				
			// �α� �����.
			break;
		
		case AGPMADMIN_CHAREDIT_MURDERER_TIME:	// ���� Murderer Time ����.
			if(pstCharEdit->m_lNewValueINT > 0)
			{
				// pstCharEdit->m_lNewValueINT �� ���� ������ �Ѿ�´�.
				m_pagpmCharacter->SetLastMurdererTime(pcsEditCharacter, pstCharEdit->m_lNewValueINT * 60);
				m_pagsmCharacter->SendPacketMurdererPoint(pcsEditCharacter);
				bResult = TRUE;
			}
			else
			{
				bResult = m_pagpmCharacter->UpdateMurdererPoint(pcsEditCharacter, 0);
			}
			
			break;
		case AGPMADMIN_CHAREDIT_CHARISMA_POINT:
			{
				bResult = m_pagpmCharacter->UpdateCharismaPoint(pcsEditCharacter, pstCharEdit->m_lNewValueINT);

				m_pagpmLog->WriteLog_CharismaUpByGm(0,
					&pcsAgsdCharacter->m_strIPAddress[0],
					pcsAgsdCharacter->m_szAccountID,
					pcsAgsdCharacter->m_szServerName,
					pcsEditCharacter->m_szID,
					((AgpdCharacterTemplate *)(pcsEditCharacter->m_pcsCharacterTemplate))->m_lID,
					m_pagpmCharacter->GetLevel(pcsEditCharacter),
					m_pagpmCharacter->GetExp(pcsEditCharacter),
					pcsEditCharacter->m_llMoney,
					pcsEditCharacter->m_llBankMoney,
					pstCharEdit->m_lNewValueINT,
					NULL);
			}
			break;
	}

	// FactorPacket �� ������.
	if(pvFactorPacket)
	{
		bResult = m_pagsmCharacter->SendPacketFactor(pvFactorPacket, pcsEditCharacter, PACKET_PRIORITY_2);
		m_pagpmCharacter->m_csPacket.FreePacket(pvFactorPacket);
	}

	// Edit �� �����ߴٰ� ���� �ȴ�.
	if(bResult)
	{
		// ����� �������� �����Ѵ�.
		pstCharEdit->m_lEditResult = 1;

		// ��Ÿ ó���� �Ѵ�.
		//
		//
		//

	}

	if(pcsEditCharacter != pcsAgpdCharacter)
		pcsEditCharacter->m_Mutex.Release();

	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	
	return TRUE;
}

// 2005.05.31. steeple
// ����� Level, Exp, InvenMoney, Position ���� ������Ʈ��.
// �ڿ� ���� ���� �����̴�.
BOOL AgsmAdmin::ProcessCharacterEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	// DB �� �ٷ� ���� ���̴�.
	// �� ������ Ư�� ������ ����´�.
	//
	// lCID ���� TID
	// NewValueINT ���� Level
	// NewValueExp ���� Exp
	// NewValueINT64 ���� InvenMoney
	// NewValueChar ���� Position �� �´�.

	if(pstCharEdit->m_lNewValueINT < 1  || pstCharEdit->m_lNewValueINT >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetCharacterTemplate(pstCharEdit->m_lCID);
	if(!pcsTemplate)
		return FALSE;

	// Exp �˻�� ���� �ʴ´�.
	//INT32 lMaxExp = 0;
	//m_pagpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[pstCharEdit->m_lNewValueINT], &lMaxExp, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_EXP);
	//if((INT32)pstCharEdit->m_lNewValueFloat >= lMaxExp)
	//	return FALSE;

	if(pstCharEdit->m_llNewValueINT64 > (LONG)AGPDCHARACTER_MAX_INVEN_MONEY)
		return FALSE;

	if(strlen(pstCharEdit->m_szNewValueChar) >= 64)
		return FALSE;

	WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITDB, pcsAgpdCharacter, pstCharEdit->m_szCharName, pstCharEdit->m_lCID,
					pstCharEdit->m_lNewValueINT, pstCharEdit->m_llNewValueExp, pstCharEdit->m_llNewValueINT64);
	if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_DB)
		EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER, pstCharEdit, pcsAgpdCharacter);
	else if(pstCharEdit->m_lEditField == AGPMADMIN_CHAREDIT_BANK_MONEY_DB)
		EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_BANK_MONEY, pstCharEdit, pcsAgpdCharacter);
		

	return TRUE;
}

// 2006.02.06. steeple
// ����� Criminal, Murderer ���� Update ��.
// �ڿ� ���� ���� �����̴�.
BOOL AgsmAdmin::ProcessCharacterEditToDB2(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	// DB �� �ٷ� ���� ���̴�.
	// �� ������ Ư�� ������ ����´�.
	//
	// lCID ���� TID
	// NewValueINT ���� CriminalPoint
	// NewValueFloat ���� MurdererPoint
	// NewValueINT64 ���� RemainedCriminalTime
	// NewValueCharLength ���� RemainedMurdererTime

	// �ٵ� 0 ���� ������ �̻��� ����3.
	if(pstCharEdit->m_lNewValueINT < 0 || pstCharEdit->m_lNewValueFloat < 0.0f ||
		pstCharEdit->m_llNewValueINT64 < 0L || pstCharEdit->m_lNewValueCharLength < 0)
		return FALSE;

	// �ð��� ���ϰ� ������ �̻� �����ϴ� ���� ������ ����.
	// �ð��� �д����� ���ƿ´�.
	INT32 lOneWeekPerMin = 7 * 24 * 60;
	if((INT32)pstCharEdit->m_llNewValueINT64 > lOneWeekPerMin ||
		pstCharEdit->m_lNewValueCharLength > lOneWeekPerMin)
		return FALSE;

	//WriteLog_Char(AGPDLOGTYPE_PLAY_GMEDITDB, pcsAgpdCharacter, pstCharEdit->m_szCharName, pstCharEdit->m_lCID,
	//				pstCharEdit->m_lNewValueINT, pstCharEdit->m_lNewValueFloat, pstCharEdit->m_llNewValueINT64);

	EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARACTER2, pstCharEdit, pcsAgpdCharacter);

	return TRUE;
}

BOOL AgsmAdmin::ProcessCharismaEditToDB(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(pstCharEdit->m_lNewValueINT < 1)
		pstCharEdit->m_lNewValueINT = 0;

	EnumCallback(AGSMADMIN_CB_RELAY_UPDATE_CHARISMA_POINT, pstCharEdit, pcsAgpdCharacter);

	return TRUE;
}

// 2006.12.01. steeple
// ������� �����ֱ�
BOOL AgsmAdmin::ProcessWCEdit(stAgpdAdminCharEdit* pstCharEdit, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmWantedCriminal || !m_pagsmWantedCriminal)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(_tcslen(pstCharEdit->m_szCharName) == 0)
		return FALSE;

	pstCharEdit->m_lEditResult = (INT8)m_pagsmWantedCriminal->DeleteWCWithNoEffect(pstCharEdit->m_szCharName);

	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Ban ó��
BOOL AgsmAdmin::ProcessBan(stAgpdAdminBan* pstBan, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmAdmin || !m_pagpmCharacter)
		return FALSE;

	if(!pstBan || !pcsAgpdCharacter)
		return FALSE;

	// Relay Server ��� DB ���常 �Ѵ�. - 2004.03.30.
	if(IsRelayServer())
	{
		ProcessBanUpdateDB(pstBan);
		return TRUE;
	}

	AgpdCharacter* pcsBanCharacter = NULL;
	if(strcmp(pstBan->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsBanCharacter = pcsAgpdCharacter;
	else
		pcsBanCharacter = m_pagpmCharacter->GetCharacterLock(pstBan->m_szCharName);

	if(!pcsBanCharacter)
	{
		OutputDebugString("AgsmAdmin::ProcessBan(...) pcsBanCharacter is NULL\n");

		// DB ���� �о�´�.
		//
		//
		
		return FALSE;
	}

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(pcsAgsdCharacter)
	{
		if(pcsAgsdCharacter->m_ulServerID != GetThisServerID())
		{
			SendBanData(pstBan, pcsAgpdCharacter->m_lID, GetGameServerNID(pcsAgsdCharacter->m_ulServerID));
			
			if(pcsBanCharacter != pcsAgpdCharacter)
				pcsBanCharacter->m_Mutex.Release();

			return TRUE;
		}
	}
	

	// ���ڷ� �Ѿ�� pstBan �� �м��ؾ� �ȴ�.
	
	// 1. Logout ��Ű��.
	if(pstBan->m_lLogout == 1)
	{
		INT32 lCID = pcsBanCharacter->m_lID;
		DPNID dpnID = m_pagsmCharacter->GetCharDPNID(pcsBanCharacter);

		if(pcsBanCharacter != pcsAgpdCharacter)
			pcsBanCharacter->m_Mutex.Release();

		DestroyClient(dpnID);
		//m_pagsmCharManager->OnDisconnect(lCID, dpnID);
		return TRUE;
	}

	// ������ Ban ���¸� �����Ѵ�. - 2004.04.01.
	ProcessBanReCalcTime(pcsBanCharacter);
	
	// 2. Ban ����
	if(pstBan->m_lBanFlag)	// ���Ѱ� �ִٸ�~
	{
		ProcessBanApplyCharacter(pstBan, pcsBanCharacter);
	}

	// �⺻ ���� ��û �Ǵ� ��ȭ ������ �� �� �̸��� �´�.
	stAgpdAdminBan stNewBan;
	memset(&stNewBan, 0, sizeof(stNewBan));

	// Ban �����͸� ��´�.
	ProcessBanGetData(&stNewBan, pcsBanCharacter);

	// ���Ѱ� �ִٸ� ���� �����ְ� DB ������ �Ѵ�.
	if(pstBan->m_lBanFlag)
	{
		// �ش� ĳ���Ϳ��� ���� ������ ������.
		//m_pagpmCharacter->SendBanData(pcsBanCharacter);

		// DB ������ �Ѵ�. - Relay Server �� ������.
		SendBanData(&stNewBan, pcsAgpdCharacter->m_lID, GetRelayServerNID());
	}

	if(pcsBanCharacter != pcsAgpdCharacter)
		pcsBanCharacter->m_Mutex.Release();

	// ��ڿ��Ե� ������.
	SendBanData(&stNewBan, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

BOOL AgsmAdmin::ProcessBanApplyCharacter(stAgpdAdminBan* pstBan, AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstBan || !pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;
	
	// ���� �ð��� ��´�.
	INT32 lCurrentTime = m_pagpmAdmin->GetCurrentTimeStamp();

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_CHAT)	// Chat Ban
	{
		pcsAgsdCharacter->m_stBan.m_lChatBanStartTime = pstBan->m_lChatBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime = pstBan->m_lChatBanKeepTime;
	}

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_CHAR)	// Char Ban
	{
		pcsAgsdCharacter->m_stBan.m_lCharBanStartTime = pstBan->m_lCharBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime = pstBan->m_lCharBanKeepTime;
	}

	if(pstBan->m_lBanFlag & AGPMADMIN_BAN_FLAG_ACC)		// Acc Ban
	{
		pcsAgsdCharacter->m_stBan.m_lAccBanStartTime = pstBan->m_lAccBanKeepTime != 0 ? lCurrentTime : 0;
		pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime = pstBan->m_lAccBanKeepTime;
	}

	return TRUE;
}

// pcsAgpdCharacter �� Ban ������ pstBan �� �ִ´�.
BOOL AgsmAdmin::ProcessBanGetData(stAgpdAdminBan* pstBan, AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pstBan || !pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;
	
	ZeroMemory(pstBan->m_szCharName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	ZeroMemory(pstBan->m_szAccName, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));

	strncpy(pstBan->m_szCharName, pcsBanCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(pstBan->m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);

	// Chat Ban
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lChatBanStartTime ? AGPMADMIN_BAN_FLAG_CHAT : 0;
	pstBan->m_lChatBanStartTime = pcsAgsdCharacter->m_stBan.m_lChatBanStartTime;
	pstBan->m_lChatBanKeepTime = pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime;

	// Char Ban
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lCharBanStartTime ? AGPMADMIN_BAN_FLAG_CHAR : 0;
	pstBan->m_lCharBanStartTime = pcsAgsdCharacter->m_stBan.m_lCharBanStartTime;
	pstBan->m_lCharBanKeepTime = pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime;

	// Acc �� ���� �ϴµ�..... 2004.05.18. �۾�
	pstBan->m_lBanFlag |= pcsAgsdCharacter->m_stBan.m_lAccBanStartTime ? AGPMADMIN_BAN_FLAG_ACC : 0;
	pstBan->m_lAccBanStartTime = pcsAgsdCharacter->m_stBan.m_lAccBanStartTime;
	pstBan->m_lAccBanKeepTime = pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime;

	return TRUE;
}

// pcsAgpdCharacter �� Ban ������ �˻��� ��, �ð��� �Ϸ�� ���� �ʱ�ȭ �Ѵ�.
BOOL AgsmAdmin::ProcessBanReCalcTime(AgpdCharacter* pcsBanCharacter)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pcsBanCharacter)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsBanCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32 lCurrentTimeStamp = m_pagpmAdmin->GetCurrentTimeStamp();
	INT32 lKeepTime = 0;

	if(pcsAgsdCharacter->m_stBan.m_lChatBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lChatBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lChatBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lChatBanKeepTime = 0;
		}
	}

	if(pcsAgsdCharacter->m_stBan.m_lCharBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lCharBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lCharBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lCharBanKeepTime = 0;
		}
	}

	if(pcsAgsdCharacter->m_stBan.m_lAccBanStartTime != 0)
	{
		lKeepTime = ProcessBanGetKeepTime(pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime);
		if(lKeepTime != -1 && (lCurrentTimeStamp - pcsAgsdCharacter->m_stBan.m_lAccBanStartTime) > lKeepTime)
		{
			pcsAgsdCharacter->m_stBan.m_lAccBanStartTime = 0;
			pcsAgsdCharacter->m_stBan.m_lAccBanKeepTime = 0;
		}
	}

	return TRUE;
}

// Define �Ǿ� �ִ� KeepTime �� �ʴ����� �ٲ��ش�.
INT32 AgsmAdmin::ProcessBanGetKeepTime(INT8 cKeepTime)
{
	INT32 lReturn = 0;
	switch(cKeepTime)
	{
		case AGSDCHAR_BAN_KEEPTIME_5H:
			lReturn = (60 * 60) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_1D:
			lReturn = 60 * 60 * 24;
			break;

		case AGSDCHAR_BAN_KEEPTIME_5D:
			lReturn = (60 * 60 * 24) * 5;
			break;

		case AGSDCHAR_BAN_KEEPTIME_10D:
			lReturn = (60 * 60 * 24) * 10;
			break;

		case AGSDCHAR_BAN_KEEPTIME_30D:
			lReturn = (60 * 60 * 24) * 30;
			break;

		case AGSDCHAR_BAN_KEEPTIME_90D:
			lReturn = (60 * 60 * 24) * 90;
			break;

		case AGSDCHAR_BAN_KEEPTIME_UNLIMIT:
			lReturn = -1;
			break;
	}

	return lReturn;
}

BOOL AgsmAdmin::ProcessBanUpdateDB(stAgpdAdminBan* pstBan)
{
	if(!pstBan)
		return FALSE;

	// Relay Server �� ���� �Ѵ�.
	if(!IsRelayServer())
		return FALSE;

	EnumCallback(AGSMADMIN_CB_BAN_UPDATE_DB, pstBan, NULL);

	return TRUE;
}








//////////////////////////////////////////////////////////////////////////
// Send Packet - Public
// 2004.03.1
BOOL AgsmAdmin::SendSearchOtherServer(stAgpdAdminSearch* pstSearch, INT32 lCID, UINT32 ulNID)
{
	if(!pstSearch || !lCID || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvSearchPacket = NULL;
	INT16 nPacketLength = 0;

	pvSearchPacket = m_pagpmAdmin->MakeSearchPacket(&nPacketLength, 
															&pstSearch->m_iType,
															&pstSearch->m_iField,
															&pstSearch->m_lObjectCID,
															pstSearch->m_szSearchName,
															NULL);

	if(!pvSearchPacket)
	{
		OutputDebugString("AgsmAdmin::SendSearchOtherServer(...) pvSearchPakcet is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeSearchPacket(&nPacketLength, &lCID, pvSearchPacket);

	m_pagpmAdmin->m_csSearchPacket.FreePacket(pvSearchPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendSearchOtherServer(...) pvPakcet is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmAdmin::SendSearchResult(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmFactors || !m_pagpmGuild || !m_pagsmCharacter)
		return FALSE;

	if(!pcsAgpdCharacter || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// 2005.09.14. steeple
	// Server Name �߰� �ϸ鼭 �� ���ϰ� Custom Packet ���� ����.
	stAgpdAdminSearchResult stSearchResult;
	memset(&stSearchResult, 0, sizeof(stSearchResult));

	strncpy(stSearchResult.m_szCharName, pcsAgpdCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(stSearchResult.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
	strncpy(stSearchResult.m_szGuildID,
				m_pagpmGuild->GetJoinedGuildID(pcsAgpdCharacter) ? m_pagpmGuild->GetJoinedGuildID(pcsAgpdCharacter) : _T(""),
				AGPACHARACTER_MAX_ID_STRING);

	stSearchResult.m_lTID = pcsAgpdCharacter->m_pcsCharacterTemplate->m_lID;
	m_pagpmFactors->GetValue(&pcsAgpdCharacter->m_csFactor, &stSearchResult.m_lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	stSearchResult.m_lStatus = 1;	// ĳ���Ͱ� �ִٴ� ���� �¶����̶� �Ҹ�.

	strcpy(stSearchResult.m_szIP, &pcsAgsdCharacter->m_strIPAddress[0]);
	
	strncpy(stSearchResult.m_szServerName, pcsAgsdCharacter->m_szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);

	return SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_SEARCH_RESULT, &stSearchResult, sizeof(stSearchResult), lCID, ulNID);
}

// AgsmRelay ���� �θ���.
// ���� Account �˻��� ���� �� �̷��� �Ҹ��� �ȴ�.
// ������ ���� ���� ĳ���� �˻��϶��� �Ҹ� �� �ִ�.
//
// DB �˻��̴� ����, lRace �� CharTID �� �´�.
BOOL AgsmAdmin::SendSearchResult(CHAR* szAdminName, CHAR* szCharName, CHAR* szAccName, INT32 lLevel,
								 INT32 lTID, INT32 lStatus, CHAR* szCreationDate)
{
	// 2005.09.14.
	// ���캸�� �� �Լ� ���� �Ⱦ���. ������.
	return TRUE;
}


// dpnAdminID ���� pcsAgpdCharacter �� ������ ������.
BOOL AgsmAdmin::SendSearchCharDataSub(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagsmCharacter)
		return FALSE;

	if(!pcsAgpdCharacter || !ulNID)
		return FALSE;

	// �߰������� ������.
	PVOID pvPacket = NULL;
	PVOID pvSearchCharDataPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_SEARCH_CHARDATA;

	stAgpdAdminCharDataSub stCharDataSub;
	memset(&stCharDataSub, 0, sizeof(stCharDataSub));

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsAgpdCharacter);
	if(pcsAgsdCharacter)
	{
		strncpy(stCharDataSub.m_szAccName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
		stCharDataSub.m_ulConnectedTimeStamp = pcsAgsdCharacter->m_ulConnectedTimeStamp;
	}

	// IP
	CHAR* szIP = GetPlayerIPAddress(m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));
	if(szIP && strlen(szIP) <= AGPMADMIN_MAX_IP_STRING)
		strncpy(stCharDataSub.m_szIP, szIP, AGPMADMIN_MAX_IP_STRING);

	pvSearchCharDataPacket = m_pagpmAdmin->MakeCharDataSubPacket(&nPacketLength,
															&pcsAgpdCharacter->m_lID,
															pcsAgpdCharacter->m_szID,
															stCharDataSub.m_szAccName,
															stCharDataSub.m_ulConnectedTimeStamp,
															stCharDataSub.m_szIP,
															stCharDataSub.m_szLastIP);

	if(!pvSearchCharDataPacket)
	{
		OutputDebugString("AgsmAdmin::SearchCharacter(...) pvSearchDataPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeCharDataSubPacket(&nPacketLength, &lCID, pvSearchCharDataPacket);

	m_pagpmAdmin->m_csSearchCharDataPacket.FreePacket(pvSearchCharDataPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SearchCharacter(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);
	
	return bResult;
}

// 2005.05.18. steeple
BOOL AgsmAdmin::SendSearchCharMoney(AgpdCharacter* pcsAgpdCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsAgpdCharacter || !lCID || !ulNID)
		return FALSE;

	stAgpdAdminCharDataMoney stMoney;
	memset(&stMoney, 0, sizeof(stMoney));

	strcpy(stMoney.m_szCharName, pcsAgpdCharacter->m_szID);
	m_pagpmCharacter->GetMoney(pcsAgpdCharacter, &stMoney.m_llInvenMoney);
	stMoney.m_llBankMoney = m_pagpmCharacter->GetBankMoney(pcsAgpdCharacter);

	SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_MONEY, &stMoney, sizeof(stMoney), lCID, ulNID);
	return TRUE;
}

// 2006.12.05. steeple
BOOL AgsmAdmin::SendCharWantedCriminal(CHAR* szCharName, INT32 lCID, UINT32 ulNID)
{
	if(!szCharName || !ulNID)
		return FALSE;

	if(!m_pagpmWantedCriminal || !m_pagsmWantedCriminal)
		return FALSE;

	AgpdWantedCriminal *pAgpdWantedCriminal = m_pagpmWantedCriminal->GetWantedCriminal((TCHAR*)szCharName);
	if(pAgpdWantedCriminal)
	{
		// Money ���ٰ� ������. -_-
		stAgpdAdminCharDataMoney stWantedCriminal;
		_tcscpy(stWantedCriminal.m_szCharName, pAgpdWantedCriminal->m_szCharID);
		stWantedCriminal.m_llInvenMoney = pAgpdWantedCriminal->m_llMoney;
		stWantedCriminal.m_llBankMoney = m_pagsmWantedCriminal->GetRank(szCharName);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_WC, &stWantedCriminal, sizeof(stWantedCriminal), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendCharQuestInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !ulNID)
		return FALSE;

	if(!m_pagsmQuest)
		return FALSE;

	AgpdQuest* pAgpdQuest = m_pagsmQuest->m_pcsAgpmQuest->GetAttachAgpdQuestData(pcsCharacter);

	if (!pAgpdQuest)
		return FALSE;

	for( AgpdQuest::IterQuest iter = pAgpdQuest->m_vtQuest.begin(); iter != pAgpdQuest->m_vtQuest.end(); ++iter)
	{
		stAgpdAdminCharQuest stQuest;
		stQuest.lQuestID = (*iter).lQuestID;
		stQuest.lParam1 = (*iter).lParam1;
		stQuest.lParam2 = (*iter).lParam2;
		_tcscpy(stQuest.szNPCName, (*iter).szNPCName);

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_QUEST, &stQuest, sizeof(stQuest), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendCharTitleInfo(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !ulNID)
		return FALSE;

	if(!m_pagsmTitle)
		return FALSE;

	if(!pcsCharacter->m_csTitle || !pcsCharacter->m_csTitleQuest)
		return FALSE;

	for(IterTitle iter = pcsCharacter->m_csTitle->m_vtTitle.begin(); iter != pcsCharacter->m_csTitle->m_vtTitle.end(); ++iter)
	{
		stAgpdAdminCharTitle stTitle;
		strncpy(stTitle.m_szCharName,pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
		stTitle.lTitleTid = (*iter).lTitleID;
		stTitle.bIsTitle = TRUE;
		stTitle.bUseTitle = (*iter).bUsingTitle;

		for(INT32 i = 0; i < AGPDTITLE_MAX_TITLE_CHECK; i++)
			stTitle.CheckValue[i] = 0;

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, &stTitle, sizeof(stTitle), lCID, ulNID);
	}

	for(IterCurrentTitleQuest iter = pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.begin(); iter != pcsCharacter->m_csTitleQuest->m_vtCurrentTitleQuest.end(); ++iter)
	{
		stAgpdAdminCharTitle stTitle;
		strncpy(stTitle.m_szCharName,pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
		stTitle.lTitleTid = (*iter).m_nTitleTid;
		stTitle.bIsTitle = FALSE;
		stTitle.bUseTitle = FALSE;

		INT32 i = 0;
		for(IterTitleCurrentCheck iter2 = (*iter).m_vtTitleCurrentCheck.begin(); iter2 != (*iter).m_vtTitleCurrentCheck.end(); ++iter2)
		{
			stTitle.CheckValue[i] = (*iter2).m_nTitleCurrentValue;
			i++;
		}

		SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_CHAR_TITLE, &stTitle, sizeof(stTitle), lCID, ulNID);
	}

	return TRUE;
}

BOOL AgsmAdmin::SendSearchCharItem(stAgpdAdminCharItem* pstItem, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagpmItem || !m_pagsmItem || !m_pagsmCharacter)
		return FALSE;

	// �˻��� ĳ���͸� ã�´�.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstItem->m_lCID != 0)
	{
		if(pstItem->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstItem->m_lCID);
	}
	else
	{
		if(strcmp(pstItem->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstItem->m_szCharName);
	}

	if(!pcsSearchCharacter)
	{
		// DB �˻��� �غ���.
		//SearchCharacterFromDB(pstItem->m_szCharName, pstItem->m_lCID, pcsAdminCharacter);
		return TRUE;
	}

	BOOL bResult = m_pagsmItem->SendPacketItemAll(pcsSearchCharacter, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();
	
	return bResult;
}

// ����, �̷��� �Ҹ��� �Ǵ� �� AgsmAdminDB �� ���ؼ��̴�.
BOOL AgsmAdmin::SendSearchCharItem(AgpdItem* pcsAgpdItem, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagsmItem)
		return FALSE;

	if(lCID == 0 || ulNID == 0 || !pcsAgpdItem)
		return FALSE;

	BOOL bResult = m_pagsmItem->SendPacketItem(pcsAgpdItem, ulNID);
	return bResult;
}

// 2005.05.18. steeple
// ���������� �ص� Item ������ ���� �� �׳� Add �� �ؼ� ó���Ͽ�����
// �׳� Add �� ��� ����, �ߺ� �� �������� ������ ���ؼ� Add �� ��ƾ�� Ÿ��Ʈ������.
// �׷� ���ؼ� �׳� Add �� ��� ���� Ŭ���̾�Ʈ���� �ݹ��� �� �޴� ��찡 ����.
// ���� Custom Packet ���� ������ ó���ϴ� �� ���� �Ŷ�� �Ǵ��ؼ� �̷��� �ٲ�.
BOOL AgsmAdmin::SendCharAllItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	SendCharEquipItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharInventoryItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharBankItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharSalesBoxItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharMailBoxItemUseCustom(pcsCharacter, lCID, ulNID);
	SendCharPetInventoryItemUseCustom(pcsCharacter, lCID, ulNID);

	AgpdCharacter* pcsAdminCharacter = m_pagpmCharacter->GetCharacter(lCID);
	if(pcsAdminCharacter && m_pagpmAdmin->GetAdminLevel(pcsAdminCharacter) >= AGPMADMIN_LEVEL_4)
		SendCharCashItemUseCustom(pcsCharacter, lCID, ulNID);

	return TRUE;
}

// 2005.05.18. steeple
// Equip Item �� �����ش�.
BOOL AgsmAdmin::SendCharEquipItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;

	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

	for(INT32 lPart = AGPMITEM_PART_NONE + 1; lPart < AGPMITEM_PART_NUM; lPart++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem(pcsCharacter, lPart);
		if(pcsGridItem)
		{
			pcsItem = m_pagpmItem->GetItem(pcsGridItem);
			if(pcsItem && pcsItem->m_pcsItemTemplate)
			{
				memset(&stItem, 0, sizeof(stItem));
				ConvertItemData(pcsItem, &stItem);
				SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
			}
		}
	}

	return TRUE;
}

// 2005.05.18. steeple
// Inventory Item �� �����ش�.
BOOL AgsmAdmin::SendCharInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetInventory(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
// Bank Item �� �����ش�.
BOOL AgsmAdmin::SendCharBankItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetBank(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
// Sales Box Item �� �����ش�.
BOOL AgsmAdmin::SendCharSalesBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetSalesBox(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2009.06.17. shootingstart
// Pet Inventory Item �� ������.
BOOL AgsmAdmin::SendCharPetInventoryItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetSubInventory(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2007.03.19. steeple
// Mail Box Item �� ������.
BOOL AgsmAdmin::SendCharMailBoxItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem || !m_pagpmMailBox || !m_pagsmCharacter)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	AgpdMailCAD* pcsMailCAD = m_pagpmMailBox->GetCAD((ApBase*)pcsCharacter);
	if(!pcsMailCAD)
		return FALSE;

	for(int i = 0; i < pcsMailCAD->GetCount(); ++i)
	{
		INT32 lMailID = pcsMailCAD->Get(i);
		AgpdMail* pcsMail = m_pagpmMailBox->GetMail(lMailID);
		if(pcsMail)
		{
			// ��� AgpdItem ��ü�� ��� �Ʒ��� ���� ��� ������ ������� �Ѵ�.
			stAgpdAdminItem stItem;
			memset(&stItem, 0, sizeof(stItem));

			_tcsncpy(stItem.m_szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
			_tcsncpy(stItem.m_szAccountName, pcsAgsdCharacter->m_szAccountID, AGPACHARACTER_MAX_ID_STRING);
			_tcsncpy(stItem.m_szServerName, pcsAgsdCharacter->m_szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);

			stItem.m_lTID = pcsMail->m_lItemTID;
			stItem.m_ullDBID = pcsMail->m_ullItemSeq;

			stItem.m_lPos = (INT32)AGPDITEM_STATUS_MAILBOX;
			stItem.m_lLayer = 0;
			stItem.m_lRow = i;		// Row �� ���ڷ� �ش�.
			stItem.m_lCol = 0;
			stItem.m_lCount = pcsMail->m_nItemQty;

			// ������.
			SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
		}
	}

	return TRUE;
}

// 2005.12.29. steeple
// Cash Inventory �� ������.
BOOL AgsmAdmin::SendCharCashItemUseCustom(AgpdCharacter* pcsCharacter, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsCharacter || !lCID || !ulNID)
		return FALSE;

	AgpdGrid* pcsGrid = m_pagpmItem->GetCashInventoryGrid(pcsCharacter);
	if(!pcsGrid)
		return FALSE;

	return SendGridItemUseCustom(pcsGrid, lCID, ulNID);
}

// 2005.05.18. steeple
BOOL AgsmAdmin::SendGridItemUseCustom(AgpdGrid* pcsGrid, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmItem)
		return FALSE;

	if(!pcsGrid)
		return FALSE;

	AgpdGridItem* pcsGridItem = NULL;
	AgpdItem* pcsItem = NULL;

	stAgpdAdminItem stItem;
	memset(&stItem, 0, sizeof(stItem));

    for(INT32 lLayer = 0; lLayer < pcsGrid->m_nLayer; lLayer++)
	{
		for(INT32 lRow = 0; lRow < pcsGrid->m_nRow; lRow++)
		{
			for(INT32 lColumn = 0; lColumn < pcsGrid->m_nColumn; lColumn++)
			{
				pcsGridItem = m_pagpmGrid->GetItem(pcsGrid, lLayer, lRow, lColumn);
				if(pcsGridItem)
				{
					pcsItem	= m_pagpmItem->GetItem(pcsGridItem);
					if(pcsItem && pcsItem->m_pcsItemTemplate)
					{
						memset(&stItem, 0, sizeof(stItem));
						ConvertItemData(pcsItem, &stItem);
						SendCustomPacket((INT32)AGPMADMIN_CUSTOM_DATA_TYPE_ITEM, (PVOID)&stItem, sizeof(stItem), lCID, ulNID);
					}	// pcsItem
				}	// pcsGridItem
			}
		}
	}

	return TRUE;
}

BOOL AgsmAdmin::SendSearchCharSkill(stAgpdAdminCharSkill* pstSkill, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;
	
	// �˻��� ĳ���͸� ã�´�.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstSkill->m_lCID!= 0)
	{
		if(pstSkill->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSkill->m_lCID);
	}
	else
	{
		if(strcmp(pstSkill->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstSkill->m_szCharName);
	}

	if(!pcsSearchCharacter)
	{
		// DB �˻��� �غ���.
		//SearchCharacterFromDB(pstSkill->m_szCharName, pstSkill->m_lCID, pcsAdminCharacter);
		return TRUE;
	}

	// ���� �� ����������. Ǫ��������!!!
	BOOL bResult = m_pagsmSkill->SendSkillAll(pcsSearchCharacter, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return bResult;
}

BOOL AgsmAdmin::SendSearchCharParty(stAgpdAdminCharParty* pstParty, AgpdCharacter* pcsAdminCharacter)
{
	if(!m_pagpmCharacter || !m_pagsmCharacter || !m_pagpmParty || !m_pagsmParty)
		return FALSE;

	// �˻��� ĳ���͸� ã�´�.
	AgpdCharacter* pcsSearchCharacter = NULL;
	if(pstParty->m_lCID != 0)
	{
		if(pstParty->m_lCID == pcsAdminCharacter->m_lID)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstParty->m_lCID);
	}
	else
	{
		if(strcmp(pstParty->m_szCharName, pcsAdminCharacter->m_szID) == 0)
			pcsSearchCharacter = pcsAdminCharacter;
		else
			pcsSearchCharacter = m_pagpmCharacter->GetCharacterLock(pstParty->m_szCharName);
	}

	// Party �� �� ���ϸ� �ٷ� ��~ DB �˻��� ���Ѵ�.
	if(!pcsSearchCharacter)
		return FALSE;

	// Party ���� Attach
	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsSearchCharacter);
	if(!pcsPartyADChar || pcsPartyADChar->lPID == AP_INVALID_PARTYID)
	{
		if(pcsSearchCharacter != pcsAdminCharacter)
			pcsSearchCharacter->m_Mutex.Release();
		
		return FALSE;
	}

	// Party �� �ƴϴ�.
	if(pcsPartyADChar->pcsParty == NULL)
	{
		if(pcsSearchCharacter != pcsAdminCharacter)
			pcsSearchCharacter->m_Mutex.Release();
		
		return FALSE;
	}

	BOOL bResult = FALSE;
	// Type �˻�
	switch(pstParty->m_lType)
	{
		case 0:	// Party ���� Send
			bResult = m_pagsmParty->SendPartyInfo(pcsPartyADChar->pcsParty, m_pagsmCharacter->GetCharDPNID(pcsAdminCharacter));
			break;

		case 10:	// �˻��� ĳ������ Ż��!!!
			// ���� ó�� ����. 2003.10.24.
			break;
	}

	if(pcsSearchCharacter != pcsAdminCharacter)
		pcsSearchCharacter->m_Mutex.Release();

	return bResult;
}

// 2004.03.22.
// pcsSearchCharacter �� pcsAgpdCharacter �� �̹� Lock �Ǿ� �ִ�.
BOOL AgsmAdmin::SendSearchCharParty(AgpdCharacter* pcsSearchCharacter, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmParty || !m_pagpmFactors)
		return FALSE;

	if(!pcsSearchCharacter || !pcsAgpdCharacter)
		return FALSE;

	// Party ���� Attach
	AgpdPartyADChar* pcsPartyADChar = m_pagpmParty->GetADCharacter(pcsSearchCharacter);
	if(!pcsPartyADChar || pcsPartyADChar->lPID == AP_INVALID_PARTYID)
		return FALSE;

	// Party �� �ƴϴ�.
	if(pcsPartyADChar->pcsParty == NULL || pcsPartyADChar->pcsParty->m_nCurrentMember == 0)
		return FALSE;

	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock(pcsPartyADChar->lPID);
	if (!pcsParty)
		return FALSE;

	stAgpdAdminCharParty stParty;
	memset(&stParty, 0, sizeof(stParty));

	// �⺻ ���� ����
	stParty.m_lCID = pcsSearchCharacter->m_lID;
	strncpy(stParty.m_szCharName, pcsSearchCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

	INT32 lValue = 0;

	for(int i = 0; i < AGPMPARTY_MAX_PARTY_MEMBER; i++)
	{
		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pcsPartyADChar->pcsParty->m_lMemberListID[i]);
		if (!pcsCharacter)
			continue;

		memset(&stParty.m_stMemberInfo, 0, sizeof(stAgpdAdminCharPartyMember));

		// �� ó�� ���� ������� ��. -0-
		if(i == 0)
			stParty.m_stMemberInfo.m_lLeader = 1;

		// ĳ���� �̸�
		strncpy(stParty.m_stMemberInfo.m_szCharName, pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);

		// Level
		m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
		stParty.m_stMemberInfo.m_lLevel = (INT16)lValue;

		// Admin ���� ������~
		SendSearchCharParty(&stParty, pcsAgpdCharacter->m_lID, GetCharacterNID(pcsAgpdCharacter));

		pcsCharacter->m_Mutex.Release();
	}

	pcsParty->m_Mutex.Release();

	return TRUE;
}

// 2004.03.22.
BOOL AgsmAdmin::SendSearchCharParty(stAgpdAdminCharParty* pstParty, INT32 lCID, UINT32 ulNID)
{
	return TRUE;
}

// 2005.05.06. steeple
BOOL AgsmAdmin::SendCustomPacket(INT32 lCustomType, PVOID pvCustomData, INT16 nDataLength, INT32 lCID, UINT32 ulNID)
{
	if(!pvCustomData || nDataLength < 1 || !ulNID)
		return FALSE;

	INT16 nPacketLength = 0;
	PVOID pvCustomPacket = m_pagpmAdmin->MakeCustomPacket(&nPacketLength, &lCustomType, pvCustomData, nDataLength);
	if(!pvCustomPacket)
		return FALSE;

	PVOID pvPacket = m_pagpmAdmin->MakeCustomPacket(&nPacketLength, &lCID, pvCustomPacket);
	m_pagpmAdmin->m_csSearchCustomPacket.FreePacket(pvCustomPacket);

	if(!pvPacket || nPacketLength < 1)
		return FALSE;

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Edit Result ��Ŷ�� ������.
BOOL AgsmAdmin::SendCharEditResult(stAgpdAdminCharEdit* pstCharEdit, INT32 lCID, UINT32 ulNID)
{
	if(!pstCharEdit || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvCharEditPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_CHAREDIT;

	pvCharEditPacket = m_pagpmAdmin->MakeCharEditPacket(&nPacketLength, pstCharEdit, sizeof(stAgpdAdminCharEdit));
	if(!pvCharEditPacket)
	{
		OutputDebugString("AgsmAdmin::SendCharEditResult(...) pvCharEditPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeCharEditPacket(&nPacketLength, &lCID, pvCharEditPacket);
	
	m_pagpmAdmin->m_csCharEditPacket.FreePacket(pvCharEditPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendCharEditResult(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

// Send Ban Packet
BOOL AgsmAdmin::SendBanData(stAgpdAdminBan* pstBan, INT32 lCID, UINT32 ulNID)
{
	if(!pstBan || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvBanPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_BAN;

	pvBanPacket = m_pagpmAdmin->MakeBanPacket(&nPacketLength,
															&lCID,
															pstBan->m_szCharName,
															pstBan->m_szAccName,
															&pstBan->m_lLogout,
															&pstBan->m_lBanFlag,
															&pstBan->m_lChatBanStartTime,
															&pstBan->m_lChatBanKeepTime,
															&pstBan->m_lCharBanStartTime,
															&pstBan->m_lCharBanKeepTime,
															&pstBan->m_lAccBanStartTime,
															&pstBan->m_lAccBanKeepTime);
	if(!pvBanPacket)
	{
		OutputDebugString("AgsmAdmin::SendBanData(...) pvBanPacket is NULL \n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeBanPacket(&nPacketLength, &lCID, pvBanPacket);

	m_pagpmAdmin->m_csBanPacket.FreePacket(pvBanPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendBanData(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}








//////////////////////////////////////////////////////////////////////////
// Callback
BOOL AgsmAdmin::CBSearchCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminSearch* pstSearch = (stAgpdAdminSearch*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pCustData;

	if(!pstSearch || !pThis || !pcsCharacter)
		return FALSE;

	return pThis->SearchCharacter(pstSearch, pcsCharacter);
}

BOOL AgsmAdmin::CBSearchCharItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharItem* pstItem = (stAgpdAdminCharItem*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstItem || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->SendSearchCharItem(pstItem, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBSearchCharSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharSkill* pstSkill = (stAgpdAdminCharSkill*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstSkill || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->SendSearchCharSkill(pstSkill, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBSearchCharParty(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

BOOL AgsmAdmin::CBCharMove(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminCharDataPosition* pstPosition = (stAgpdAdminCharDataPosition*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstPosition || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->ProcessCharacterMove(pstPosition, pcsAdminCharacter);
}

BOOL AgsmAdmin::CBCharEdit(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	PVOID* ppvBuffer = static_cast<PVOID*>(pData);
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	stAgpdAdminCharEdit* pstCharEdit = (stAgpdAdminCharEdit*)ppvBuffer[0];
	if(!pstCharEdit)
		return FALSE;
	
	// ���⼭ �����ش�.
	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_SKILL_LEARN:
		case AGPMADMIN_CHAREDIT_SKILL_LEVEL:
		case AGPMADMIN_CHAREDIT_SKILL_INIT_ALL:
		case AGPMADMIN_CHAREDIT_SKILL_INIT_SPECIFIC:
		case AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE:
		case AGPMADMIN_CHAREDIT_COMPOSE_LEARN:
		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_ALL:
		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_SPECIFIC:
			return pThis->ProcessCharSkillEdit(pstCharEdit, static_cast<stAgpdAdminSkillString*>(ppvBuffer[1]), pcsAdminCharacter);
			break;

		case AGPMADMIN_CHAREDIT_WANTED_CRIMINAL:
			return pThis->ProcessWCEdit(pstCharEdit, pcsAdminCharacter);
			break;

		default:
			return pThis->ProcessCharacterEdit(pstCharEdit, pcsAdminCharacter);
			break;
	}

	return TRUE;
}

BOOL AgsmAdmin::CBBan(PVOID pData, PVOID pClass, PVOID pCustData)
{
	stAgpdAdminBan* pstBan = (stAgpdAdminBan*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	AgpdCharacter* pcsAdminCharacter = (AgpdCharacter*)pCustData;

	if(!pstBan || !pThis || !pcsAdminCharacter)
		return FALSE;

	return pThis->ProcessBan(pstBan, pcsAdminCharacter);
}









//////////////////////////////////////////////////////////////////////////
// From AgsmAdminDB
BOOL AgsmAdmin::CBSearchCharacterResultDB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CHAR* szName = (CHAR*)pData;
	AgsmAdmin* pThis = (AgsmAdmin*)pClass;
	INT32* plCID = (INT32*)pCustData;

	if(!szName || !pThis || !plCID)
		return FALSE;

	pThis->SearchCharacterResultDB(szName, *plCID);
	return TRUE;

	//AgpdCharacter* pcsAgpdCharacter = (AgpdCharacter*)pData;
	//AgsmAdmin* pThis = (AgsmAdmin*)pClass;

	//if(!pcsAgpdCharacter || !pThis)
	//	return FALSE;

	//pThis->SendSearchCharacterToSearchingAdmin(pcsAgpdCharacter);

	return TRUE;
}
