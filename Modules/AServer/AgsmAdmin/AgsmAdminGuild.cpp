// AgsmAdminGuild.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2005.04.25.
//
// ��� ���� ���۷��̼� ����

#include "AgsmAdmin.h"

//////////////////////////////////////////////////////////////////////////
// Guild Operation

// ����ü�� CharEdit ���� �� ����.
BOOL AgsmAdmin::ProcessGuildOperation(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	if(!m_pagpmAdmin->IsAdminCharacter(pcsAgpdCharacter))
		return FALSE;

	// ��Ʈ�� ĸ �Ǽ��� �̰����ٰ� ���۷��̼��� ���� �Ѵ�.-_-;;;
	switch(pstGuild->m_lCID)
	{
		case AGPMADMIN_GUILD_SEARCH:	// Search ��. ������~ ����
			ProcessGuildSearch(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_SEARCH_PASSWORD:
			ProcessGuildSearchPassword(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_TOTAL_INFO:
			ProcessGuildTotalInfo(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_LEAVE_MEMBER:
			ProcessGuildMemberLeave(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_DESTROY:
			ProcessGuildDestroy(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_CHANGE_MASTER:
			ProcessGuildChangeMaster(pstGuild, pcsAgpdCharacter);
			break;

		case AGPMADMIN_GUILD_CHANGE_NOTICE:
			ProcessGuildChangeNotice(pstGuild, pcsAgpdCharacter);
			break;
	}

	return TRUE;
}

// ����ü�� CharEdit ���� �� ����.
// �ش� ����� ��� ������ ���.
BOOL AgsmAdmin::ProcessGuildSearch(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

    // ��� ���̵�� ĳ�� �̸��� ���´�. -_-;;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pstGuild->m_szCharName);
	if(!pcsGuild)
		return FALSE;

	// �ش� Admin ���� Create ������ ��� ����� Join ��Ų��. ���������� Battle Info �ѹ��� �����ش�.
	UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	m_pagsmGuild->SendCreateGuild(pcsGuild, pcsAgpdCharacter->m_lID, ulNID);

	AgpdGuildMember** ppcsMember = NULL;
	INT32 lIndex = 0;
	for(ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex); ppcsMember;
		ppcsMember = (AgpdGuildMember**)pcsGuild->m_pMemberList->GetObjectSequence(&lIndex))
	{
		m_pagsmGuild->SendJoinMember(pcsGuild, *ppcsMember, pcsAgpdCharacter->m_lID, ulNID);
	}

	m_pagsmGuild->SendBattleInfo(pcsGuild, ulNID);

	// Password �� ������.
	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_SEARCH_PASSWORD;
	strcpy(stGuild.m_szCharName, pcsGuild->m_szID);
	strcpy(stGuild.m_szNewValueChar, pcsGuild->m_szPassword);

	SendGuildOperation(&stGuild, pcsAgpdCharacter->m_lID, ulNID);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessGuildSearchPassword(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

    // ��� ���̵�� ĳ�� �̸��� ���´�. -_-;;
	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pstGuild->m_szCharName);
	if(!pcsGuild)
		return FALSE;

	UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_SEARCH_PASSWORD;
	strcpy(stGuild.m_szCharName, pcsGuild->m_szID);
	strcpy(stGuild.m_szNewValueChar, pcsGuild->m_szPassword);

	SendGuildOperation(&stGuild, pcsAgpdCharacter->m_lID, ulNID);

	pcsGuild->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessGuildTotalInfo(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	UINT32 ulNID = m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter);

	stAgpdAdminCharEdit stGuild;
	memset(&stGuild, 0, sizeof(stGuild));

	stGuild.m_lCID = AGPMADMIN_GUILD_TOTAL_INFO;
	stGuild.m_lNewValueINT = m_pagpmGuild->GetGuildCount();

	INT32 lIndex = 0;
	for(AgpdGuild* pcsGuild = (AgpdGuild*)m_pagpmGuild->GetGuildSequence(&lIndex); pcsGuild;
					pcsGuild = (AgpdGuild*)m_pagpmGuild->GetGuildSequence(&lIndex))
	{
		pcsGuild->m_Mutex.WLock();
		stGuild.m_llNewValueINT64 += pcsGuild->m_pMemberList->GetObjectCount();
		pcsGuild->m_Mutex.Release();
	}

	SendGuildOperation(&stGuild, pcsAgpdCharacter->m_lID, ulNID);

	return TRUE;
}

// 2006.01.19. steeple
BOOL AgsmAdmin::ProcessGuildMemberLeave(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	// �۾��� ĳ�� �̸��� ��3
	if(strlen(pstGuild->m_szCharName) == 0)
		return FALSE;

	// �̰Ŵ� ������ �峭ġ�� ����3.
	if(strcmp(pstGuild->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		return FALSE;

	BOOL bResult = FALSE;

	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstGuild->m_szCharName);
	if(pcsCharacter)
	{
		// ��尡 �����ϴ��� Ȯ��.
		AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pstGuild->m_szNewValueChar);
		if(pcsGuild)
		{
			// ��尡 �����Ѵٸ�
			bResult = m_pagsmGuild->Leave(pstGuild->m_szNewValueChar, pstGuild->m_szCharName);
		}
		else
		{
			// ��尡 ���ٸ�, ĳ���� ������ �ʱ�ȭ�Ѵ�.
			// ������ ���̵��.. ����.. �� ���� �̸��� ����.
			bResult = m_pagsmGuild->LeaveChar(pstGuild->m_szNewValueChar, pcsAgpdCharacter->m_szID, pstGuild->m_szCharName);
		}
		
		pcsCharacter->m_Mutex.Release();
	}
	else
	{
		// ��尡 �����ϴ��� Ȯ��.
		AgpdGuild* pcsGuild = m_pagpmGuild->GetGuild(pstGuild->m_szNewValueChar);
		if(pcsGuild)
		{
			// 2006.09.07. steeple
			bResult = m_pagpmGuild->OnOperationForcedLeave(1, pcsGuild->m_szID, pcsGuild->m_szMasterID, pstGuild->m_szCharName);
			// ��尡 �����Ѵٸ� ����� ó��.
			//bResult = m_pagsmGuild->ForcedLeave(pstGuild->m_szNewValueChar, pstGuild->m_szCharName);
		}
		else
		{
			// ��尡 ���ٸ�, DB ������ �����ָ� �ȴ�.
			bResult = m_pagsmGuild->EnumCallback(AGSMGUILD_CB_DB_MEMBER_DELETE, pstGuild->m_szNewValueChar, pstGuild->m_szCharName);
		}
	}

	pstGuild->m_lCID = AGPMADMIN_GUILD_RESULT;
	if(bResult)
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_SUCCESS_LEAVE_MEMBER;
	else
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_FAILED_LEAVE_MEMBER;

	SendGuildOperation(pstGuild, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

// 2006.01.19. steeple
BOOL AgsmAdmin::ProcessGuildDestroy(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	BOOL bResult = FALSE;

    AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pstGuild->m_szNewValueChar);
	if(pcsGuild)
	{
		// ������ ���̵�� �н����带 ��´�.
		CHAR szMasterID[AGPACHARACTER_MAX_ID_STRING + 1];
		CHAR szPassword[AGPMGUILD_MAX_PASSWORD_LENGTH + 1];

		memset(szMasterID, 0, sizeof(szMasterID));
		memset(szPassword, 0, sizeof(szPassword));

		strncpy(szMasterID, pcsGuild->m_szMasterID, AGPACHARACTER_MAX_ID_STRING);
		strncpy(szPassword, pcsGuild->m_szPassword, AGPMGUILD_MAX_PASSWORD_LENGTH);

		// ���� Ǯ�� �ؾ� �Ѵ�.
		pcsGuild->m_Mutex.Release();

		bResult = m_pagpmGuild->OnOperationDestroy(pcsAgpdCharacter->m_lID, pstGuild->m_szNewValueChar, szMasterID, szPassword, TRUE);
	}

	pstGuild->m_lCID = AGPMADMIN_GUILD_RESULT;
	if(bResult)
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_SUCCESS_DESTROY_GUILD;
	else
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_FAILED_DESTROY_GUILD;

	SendGuildOperation(pstGuild, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

// 2006.01.19. steeple
BOOL AgsmAdmin::ProcessGuildChangeMaster(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	BOOL bResult = FALSE;

	AgpdGuild* pcsGuild = m_pagpmGuild->GetGuildLock(pstGuild->m_szNewValueChar);
	if(pcsGuild)
	{
        // ������ ���� ���� �𸣴�, �ƹ��� ���������� ���� ���¿��� �Ѵ�.
		if(m_pagpmGuild->GetOnlineMemberCount(pcsGuild) <= 0)
		{
			// ���� �����Ϳ�, ���� �� �������� ����� ��´�.
			AgpdGuildMember* pcsOldMaster = m_pagpmGuild->GetMaster(pcsGuild);
			AgpdGuildMember* pcsNewMaster = m_pagpmGuild->GetMember(pcsGuild, pstGuild->m_szCharName);

			// �� �� ���� ���� �Ѵ�.
			if(pcsOldMaster && pcsNewMaster)
			{
				// OldMaster �� ���� ��Ż.
				pcsOldMaster->m_lRank = AGPMGUILD_MEMBER_RANK_NORMAL;
				m_pagsmGuild->EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuild->m_szID, pcsOldMaster);

				// NewMaster ����.
				m_pagpmGuild->SetMaster(pcsGuild, pcsNewMaster);
				m_pagsmGuild->EnumCallback(AGSMGUILD_CB_DB_MEMBER_UPDATE, pcsGuild->m_szID, pcsNewMaster);

				// ��� ������ ������Ʈ
				m_pagsmGuild->EnumCallback(AGSMGUILD_CB_DB_GUILD_UPDATE, pcsGuild, NULL);

				bResult = TRUE;
			}
		}

		pcsGuild->m_Mutex.Release();
	}

	pstGuild->m_lCID = AGPMADMIN_GUILD_RESULT;
	if(bResult)
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_MASTER;
	else
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_MASTER;

	SendGuildOperation(pstGuild, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

// 2006.02.02. steeple
BOOL AgsmAdmin::ProcessGuildChangeNotice(stAgpdAdminCharEdit* pstGuild, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstGuild || !pcsAgpdCharacter)
		return FALSE;

	BOOL bResult = FALSE;

	// ������ ���̵�� �������� �־���.
	bResult = m_pagpmGuild->OnOperationUpdateNotice(1, pstGuild->m_szCharName, _T(""), pstGuild->m_szNewValueChar,
													pstGuild->m_lNewValueCharLength, TRUE);

	pstGuild->m_lCID = AGPMADMIN_GUILD_RESULT;
	if(bResult)
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_SUCCESS_CHANGE_NOTICE;
	else
		pstGuild->m_lNewValueINT = AGPMADMIN_GUILD_RESULT_FAILED_CHANGE_NOTICE;

	SendGuildOperation(pstGuild, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsAgpdCharacter));

	return TRUE;
}

BOOL AgsmAdmin::SendGuildOperation(stAgpdAdminCharEdit* pstCharEdit, INT32 lCID, UINT32 ulNID)
{
	if(!m_pagpmGuild || !m_pagsmGuild)
		return FALSE;

	if(!pstCharEdit || !ulNID)
		return FALSE;

	PVOID pvPacket = NULL;
	PVOID pvCharEditPacket = NULL;
	INT16 nPacketLength = 0;
	INT8 cOperation = AGPMADMIN_PACKET_GUILD;

	pvCharEditPacket = m_pagpmAdmin->MakeCharEditPacket(&nPacketLength, pstCharEdit, sizeof(stAgpdAdminCharEdit));
	if(!pvCharEditPacket)
	{
		OutputDebugString("AgsmAdmin::SendGuildOperation(...) pvCharEditPacket is NULL\n");
		return FALSE;
	}

	pvPacket = m_pagpmAdmin->MakeAdminPacket(TRUE, &nPacketLength, AGPMADMIN_PACKET_TYPE, &cOperation, &lCID, NULL,
												NULL, NULL, pvCharEditPacket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
	m_pagpmAdmin->m_csCharEditPacket.FreePacket(pvCharEditPacket);

	if(!pvPacket || nPacketLength < 1)
	{
		OutputDebugString("AgsmAdmin::SendGuildOperation(...) pvPacket is Invalid\n");
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength, ulNID);
	m_pagpmAdmin->m_csPacket.FreePacket(pvPacket);

	return bResult;
}

BOOL AgsmAdmin::CBGuildOperation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	stAgpdAdminCharEdit* pstGuild = static_cast<stAgpdAdminCharEdit*>(pData);
	AgsmAdmin* pThis = static_cast<AgsmAdmin*>(pClass);
	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pCustData);

	return pThis->ProcessGuildOperation(pstGuild, pcsCharacter);
}