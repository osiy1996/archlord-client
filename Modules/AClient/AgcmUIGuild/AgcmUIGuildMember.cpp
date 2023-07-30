// AgcmUIGuildMember.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2004. 07. 12.

#include <AgcmUIGuild/AgcmUIGuild.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmSiegeWar/AgpmSiegeWar.h>
#include <AgpmParty/AgpmParty.h>
#include <AgcmUIPartyOption/AgcmUIPartyOption.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgpmGuild/AgppGuild.h>

BOOL AgcmUIGuild::InitMemberUIData()
{
	memset(m_szSelfGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH + 1));
	
	m_listAgcdGuildMember.clear();

	memset(m_szSelectedMemberID, 0, sizeof(CHAR) * (AGPACHARACTER_MAX_ID_STRING + 1));
	m_lSelectedMemberIndex = -1;

	m_bGuildInfoUIOpened = FALSE;
	m_cGuildInfoUILastTab = 0;
	m_lCurrentPage = 0;

	return TRUE;
}

BOOL AgcmUIGuild::SetCurrentPage(INT32 lPage)
{
	m_lCurrentPage = lPage;
	RefreshMemberPageUI();
	return TRUE;
}

BOOL AgcmUIGuild::RefreshMemberPageUI(INT32 lSelectIndex )
{
	OutputDebugString("[AgcmUIGuild] :: RefreshMemberPageUI �ҷȴ�~~~!~!~!~!~!~!\n");
	
	GetMemberListPage();

	m_lGuildMemberCount = 0;

	m_bEnableAppointButton		=	FALSE;
	m_bEnableSuccessionButton	=	FALSE;

	AgpdGuild* pcsGuild = m_pcsAgcmGuild->GetSelfGuildLock();
	
	if( pcsGuild )
	{
		m_lGuildMemberCount = m_pcsAgpmGuild->GetAllMemberCount( pcsGuild );
		pcsGuild->m_Mutex.Release();
	}

	if (m_lGuildMemberCount > 0 )
		m_lMaxMemberListPage = ((m_lGuildMemberCount-1) / AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMemberListPage);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMemberListMaxPage);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableAppoint);
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableSuccession);

	SelectMember(lSelectIndex);

	return TRUE;
}

BOOL AgcmUIGuild::GetMemberListPage()
{
	m_pcsAgcmGuild->GetMemberList( m_lCurrentPage * AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST,
									m_listAgcdGuildMember, AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST);
	m_pstMemberIndexUD->m_stUserData.m_lCount = m_listAgcdGuildMember.size();

	return TRUE;
}

AgcdGuildMember* AgcmUIGuild::GetGuildMember( INT32 lIndex )
{
	if( lIndex < 0 )									return NULL;
	if( (INT32)m_listAgcdGuildMember.size() <= lIndex )	return NULL;
	
	INT32 lCount = 0;
	for( AgcdGuildMemberListItr Itr = m_listAgcdGuildMember.begin(); Itr != m_listAgcdGuildMember.end(); ++Itr, ++lCount )
	{
		if( lCount == lIndex )
			return (AgcdGuildMember*)&(*Itr);
	}

	return NULL;
}

BOOL AgcmUIGuild::CBMemberToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->RefreshMemberPageUI();
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstUDGuildMemberDetailInfo);

	return TRUE;
}

BOOL AgcmUIGuild::CBMemberTabClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_cGuildInfoUILastTab = 1;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildNoticeUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildGuildListUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildMemberUI);

	return TRUE;
}

// ��ư ������ �� �Ҹ���.
BOOL AgcmUIGuild::CBMemberSelect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if( !pThis->GetGuildMember( pcsSourceControl->m_lUserDataIndex ) )			return FALSE;

	if (pThis->m_lSelectedMemberIndex == pcsSourceControl->m_lUserDataIndex)	// ���� ������ �����ϸ�
		pThis->AllowSelectMember();

	else
		pThis->SelectMember(pcsSourceControl->m_lUserDataIndex);

	return TRUE;
}

//	��� ����� ���õǾ���.

VOID AgcmUIGuild::SelectMember(INT32 lSelectIndex)
{
	// List Item �� üũ���ش�.
	m_lSelectedMemberIndex = lSelectIndex;
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMemberIndexUD);

	// ���� ������ ���� ���� �����Ѵ�.
	AgcdGuildMember* pMember = GetGuildMember( m_lSelectedMemberIndex );
	if( pMember )
	{
		sprintf_s( m_szSelectedMemberID, "%s", pMember->m_szMemberID );
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstSelectedMemberIDUD);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDGuildMemberDetailInfo);

		RefreshGuildMemberButton();
	}

}

/*
	��� ��� ����Ʈ ���� â���� ���� ��ư���� Ȱ��ȭ�� ���Ѵ�.
*/
VOID AgcmUIGuild::RefreshGuildMemberButton()
{

	m_bEnableLeaveGuildMemberList	= FALSE;
	m_bEnableInviteParty			= FALSE;
	m_bEnableChatWhisper			= FALSE;
	m_bEnableAllowGuildMemberList	= FALSE;
	m_bEnableAppointButton			= FALSE;			//	�θ����� �Ӹ� �� ���� ��ư
	m_bEnableSuccessionButton		= FALSE;			//	���°� ��ư

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)m_pcsAgcmCharacter->GetSelfCharacter();
	AgcdGuildMember* pMember = GetGuildMember( m_lSelectedMemberIndex );

	if( !pcsCharacter|| !pMember )
	{
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableChatWhisper);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableLeaveGuildMemberList);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableInviateParty);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableAllowGuildMemberList);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableAppoint);
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstEnableSuccession);
		return;
	}

	BOOL bLoginUser		= pMember->m_cStatus == AGPMGUILD_MEMBER_STATUS_ONLINE ? TRUE : FALSE;
	AgpdGuild* pcsGuild = m_pcsAgcmGuild->GetSelfGuildLock();

	if( pcsGuild )
	{
		if( m_pcsAgpmGuild->IsMaster	( pcsGuild , pcsCharacter->m_szID ) ||
			m_pcsAgpmGuild->IsSubMaster	( pcsGuild , pcsCharacter->m_szID )		)
		{
			// �θ����ʹ� ����Ż�� ������ ����
			if( !m_pcsAgpmGuild->IsSubMaster( pcsGuild , pcsCharacter->m_szID ) )
				m_bEnableLeaveGuildMemberList = TRUE;

			if ( AGPMGUILD_MEMBER_RANK_JOIN_REQUEST == pMember->m_lRank || 
				 AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST == pMember->m_lRank )
			{
				// �α渶�� Ż���㰡 ������ ����
				if( m_pcsAgpmGuild->IsSubMaster( pcsGuild , pcsCharacter->m_szID) &&
					AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST == pMember->m_lRank )
						m_bEnableAllowGuildMemberList	=	FALSE;

				else	m_bEnableAllowGuildMemberList	=	TRUE;
			}
			
			// ��帶���� ������ �ƴ϶�� �θ����� �Ӹ� ��ư�� Ȱ��ȭ ��Ű��
			if( !m_pcsAgpmGuild->IsMaster( pcsGuild , pMember->m_szMemberID ) 
				&& !m_pcsAgpmGuild->IsSubMaster( pcsGuild , pcsCharacter->m_szID ) )
			{
				
				if( ( !strlen(pcsGuild->m_szSubMasterID) && bLoginUser ) ||
					m_pcsAgpmGuild->IsSubMaster( pcsGuild , pMember->m_szMemberID )   )
				{
					if( pMember->m_lLevel >= 20 )
						m_bEnableAppointButton		=	TRUE;
				}
				
				// �ش� ������ �θ������̰� ��尡 ��Ʋ���� �ƴϸ� �������� �ƴ϶��
				// ���°� ��ư�� Ȱ��ȭ
				if( m_pcsAgpmGuild->IsSubMaster( pcsGuild , pMember->m_szMemberID ) &&
					pcsGuild->m_cStatus	== AGPMGUILD_STATUS_NONE					&&
					!m_pcsAgpmSiegeWar->IsGuildSiegeWarEntry( pcsGuild->m_szID )	&&
					bLoginUser	)
				{
					m_bEnableSuccessionButton	=	TRUE;			// ��� �°� ��ư Ȱ��ȭ
				}
			}

		}

		// �θ����� ������ ��ư�̸��� �ٲ۴�
		if( strlen(pcsGuild->m_szSubMasterID) )
		{
			strcpy_s( m_szAppointAndCancelBtnID , AGPMGUILD_MAX_GUILD_ID_LENGTH ,
				ClientStr().GetStr( STI_STATICTEXT_SUBMASTER_CANCEL ) );
		}
		// �θ����� �Ӹ����� ��ư�̸��� �ٲ۴�
		else
		{
			strcpy_s( m_szAppointAndCancelBtnID , AGPMGUILD_MAX_GUILD_ID_LENGTH ,
				ClientStr().GetStr( STI_STATICTEXT_SUBMASTER_APPOINT ) );
		}

		pcsGuild->m_Mutex.Release();
	}

	if (bLoginUser)		//	���õ� ������ �¶����̸�
	{
		m_bEnableChatWhisper = TRUE;

		AgpdParty* pcsSelfParty = m_pcsAgpmParty->GetParty(pcsCharacter);
		if ( pcsSelfParty && pcsSelfParty->m_lMemberListID[0] == pcsCharacter->m_lID)		//	��Ƽ�� �ְ� ��Ƽ���̸�
			m_bEnableInviteParty = TRUE;
		else
			m_bEnableInviteParty = TRUE;		//	��Ƽ ������ �Ǿ� ���� ������
	}	

	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableChatWhisper );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableLeaveGuildMemberList );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableInviateParty );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableAllowGuildMemberList );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableAppoint );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstEnableSuccession );
	m_pcsAgcmUIManager2->SetUserDataRefresh( m_pstAppointAndCancelBtnID );
}

BOOL AgcmUIGuild::CBMemberLeaveClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgpdCharacter* pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return FALSE;
	
	if(strlen(pThis->m_pcsAgcmGuild->GetSelfGuildID()) == 0)
		return FALSE;

	// ���� �����Ͷ�� ������ ����� ���� ��Ű�� ���̴�.
	if(pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID) )
	{
		if(strlen(pThis->m_szSelectedMemberID) > 0)
		{
			// �����Ͱ� �ڽ��� �����Ϸ� �Ѵ�
			if(strcmp(pThis->m_szSelectedMemberID, pcsCharacter->m_szID) == 0 )
			{
				CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SYSTEM_MESSAGE_COLOR);
				DWORD dwColor = 0xFFFFFF33;
				if(szColor) dwColor = atoi(szColor);

				CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LEAVE_ERROR_MASTER);
				if (szBuffer) SystemMessage.ProcessSystemMessage(szBuffer, dwColor);
			}
			else
			{
				pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenForcedLeaveConfirmUI);
			}
		}
	}
	else
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenLeaveConfirmUI);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBMemberLeftClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// ���� �������� 0(ù������) �� ���ö���
	if(pThis->m_lCurrentPage == 0)
		return FALSE;
	
	--pThis->m_lCurrentPage;
	pThis->RefreshMemberPageUI();

	return TRUE;
}

BOOL AgcmUIGuild::CBMemberRightClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	// ȸ������ �� ȭ�鿡�� �� ���� �ʾ����� ����
	if(pThis->m_lCurrentPage >= pThis->m_lMaxMemberListPage)
		return FALSE;

	// 2005.07.18. steeple. ����
	// �̷� ���̳��� ���װ�!!!!!!!!!!
	if(pThis->m_lGuildMemberCount > (pThis->m_lCurrentPage + 1) * AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST)
	{
		++pThis->m_lCurrentPage;
		pThis->RefreshMemberPageUI();
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBMemberClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_bGuildInfoUIOpened = FALSE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);

	return TRUE;
}

BOOL AgcmUIGuild::CBMemberSelectDClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	//����� �ɹ����ù�� ����
	return TRUE;

	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgcdGuildMember* pMember = pThis->GetGuildMember( pcsSourceControl->m_lUserDataIndex );
	if( !pMember )										return FALSE;
	if( !pThis->m_cUIGuildBattleMember.IsOpen() )		return FALSE;

	if( AGPMGUILD_MEMBER_STATUS_OFFLINE == pMember->m_cStatus )
	{
		SystemMessage.ProcessSystemMessage( "�������� ���� �����Դϴ�" );
		return FALSE;
	}
	else
		pThis->m_cUIGuildBattleMember.PushList( pMember->m_szMemberID, pMember->m_lLevel );

	return TRUE;
}


BOOL AgcmUIGuild::CBDisplayMemberStatus(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	AgcdGuildMember* pMember = pThis->GetGuildMember( *pcsIndex );
	if( !pMember )		return TRUE;

	if( pMember->m_cStatus == AGPMGUILD_MEMBER_STATUS_ONLINE)
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);
	else
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID);

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayMemberName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	szDisplay[0] = NULL;

	AgcdGuildMember* pMember = pThis->GetGuildMember( *pcsIndex );
	if( pMember )
	{
		CHAR szBuffer[AGPACHARACTER_MAX_ID_STRING + 10];
		memset(szBuffer, 0, AGPACHARACTER_MAX_ID_STRING + 10);
		strncpy(szBuffer, pMember->m_szMemberID, AGPACHARACTER_MAX_ID_STRING);
		for (UINT16 i = 0 ; i < strlen(szBuffer); ++i)
		{
			if (szBuffer[i] == '@')
			{
				strcpy(&szBuffer[i], "...");
				break;
			}
		}
		strncpy(szDisplay, szBuffer, AGPACHARACTER_MAX_ID_STRING);
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayMemberLevel(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	szDisplay[0] = NULL;	

	AgcdGuildMember* pMember = pThis->GetGuildMember( *pcsIndex );
	if( pMember )
	{
		CHAR *szBuffer = NULL;
		if (pMember->m_lRank == AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
			szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_MEMBER_JOIN_REQUEST);
		if (pMember->m_lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST)
			szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_MEMBER_LEAVE_REQUEST);

		if (szBuffer)	strcpy(szDisplay, szBuffer);
		else			sprintf(szDisplay, "%d", pMember->m_lLevel);
	}

	return TRUE;
}

/*
	2005.07.05 By SungHoon
	Ŭ���� �����ֱ� ���� Diplay Func
*/
BOOL AgcmUIGuild::CBDisplayMemberClass(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	szDisplay[0] = NULL;

	AgcdGuildMember* pMember = pThis->GetGuildMember( *pcsIndex );
	if( pMember )
	{
		INT32 lTID = pMember->m_lTID;
		AgpdCharacterTemplate *pcsTemplate = pThis->m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
		if( !pcsTemplate )		return TRUE;

		INT32 lRace = pThis->m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor);
		INT32 lClass = pThis->m_pcsAgpmFactors->GetClass(&pcsTemplate->m_csFactor);
		CHAR *szClass = pThis->m_pcsAgpmFactors->GetCharacterClassName(lRace, lClass);
		if( szClass ) 
			strcpy( szDisplay, szClass );
	}

	return TRUE;
}

/*
	2005.07.05 By SungHoon
	���� �����ֱ� ���� Diplay Func
*/
BOOL AgcmUIGuild::CBDisplayMemberRace(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	szDisplay[0] = NULL;

	AgcdGuildMember* pMember = pThis->GetGuildMember( *pcsIndex );
	if( pMember )
	{
		INT32 lTID = pMember->m_lTID;
		AgpdCharacterTemplate *pcsTemplate = pThis->m_pcsAgpmCharacter->GetCharacterTemplate(lTID);
		if( !pcsTemplate )	return TRUE;

		INT32 lRace = pThis->m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor);
		CHAR* szRace = pThis->m_pcsAgpmFactors->GetCharacterRaceName(lRace);
		if( szRace )
			strcpy( szDisplay, szRace );
	}

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayMemberSelect(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	INT32* pcsIndex = (INT32*)pData;

	szDisplay[0] = NULL;

	if(pThis->m_lSelectedMemberIndex == pcsSourceControl->m_lUserDataIndex)
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);
	else
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID);


	return TRUE;
}

// ����, Ż�� ��ư ������ Dialog �� �� ����
BOOL AgcmUIGuild::CBDisplaySelectedMember(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if(!pClass || !pData || eType != AGCDUI_USERDATA_TYPE_STRING || !szDisplay)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	sprintf(szDisplay, "%s", pThis->m_szSelectedMemberID);
	
	return TRUE;
}
/*
	2005.07.06. By SungHoon
	��� ����� �������� �����ֱ� ���� �ҷ�����.
*/
BOOL AgcmUIGuild::CBDisplayMemberDetailInfo(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if(!pData || !pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");

	AgcdGuildMember *pcsGuildMember = pThis->GetGuildMember( pThis->m_lSelectedMemberIndex );
	if (!pcsGuildMember)	return TRUE;
	
	AgpdCharacterTemplate *pcsTemplate = pThis->m_pcsAgpmCharacter->GetCharacterTemplate(pcsGuildMember->m_lTID);
	if( !pcsTemplate )		return TRUE;

	
	INT32 lRace = pThis->m_pcsAgpmFactors->GetRace(&pcsTemplate->m_csFactor);
	INT32 lClass = pThis->m_pcsAgpmFactors->GetClass(&pcsTemplate->m_csFactor);
	CHAR *szRace = pThis->m_pcsAgpmFactors->GetCharacterRaceName(lRace);
	CHAR *szClass = pThis->m_pcsAgpmFactors->GetCharacterClassName(lRace, lClass);
	if(szRace && szClass)
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

		CHAR	szBuffer[300];
		CHAR	szRank[ AGPACHARACTER_MAX_ID_STRING + 1 ];
		CHAR	*szUIMessage =  NULL;
		ZeroMemory( szBuffer	, sizeof(szBuffer)	);
		ZeroMemory( szRank		, sizeof(szRank)	);

		BOOL	bLeadMember	= FALSE;

		//  ��帶���� ���� Ȯ��
		bLeadMember		= pThis->m_pcsAgpmGuild->IsMaster(pThis->m_pcsAgcmGuild->GetSelfGuildID() , pcsGuildMember->m_szMemberID );
		if( bLeadMember )	szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_MASTER );

		// ��帶���Ͱ� �ƴ϶�� �α渶 ���� Ȯ��
		if( !bLeadMember )
		{
			bLeadMember	=	pThis->m_pcsAgpmGuild->IsSubMaster( pThis->m_pcsAgcmGuild->GetSelfGuildID() , pcsGuildMember->m_szMemberID );
			if( bLeadMember )	szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_SUBMASTER );
		}
		
		// ��� �����ͳ� ��� �θ����͸� ǥ�����ش�
		if( bLeadMember )
		{
			strcpy_s( szRank , sizeof(szRank) , szUIMessage );
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_LEADMEMBER_DETAIL_INFO);
			wsprintf( szBuffer , szUIMessage , pcsGuildMember->m_szMemberID , pcsGuildMember->m_lLevel , szRank , szRace , szClass );
		}

		// �Ϲݱ����� ǥ�þ��Ѵ�
		else
		{
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_GUILD_MEMBER_DETAIL_INFO );
			wsprintf( szBuffer , szUIMessage , pcsGuildMember->m_szMemberID , pcsGuildMember->m_lLevel , szRace , szClass );
		}

		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	}

	return TRUE;
}

/*
	2005.07.15. By SungHoon
	���ã�⸦ ������ ���.
*/
BOOL AgcmUIGuild::CBGuildMemberSearch(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1 )
		return FALSE;

	AgcmUIGuild		*pThis						= (AgcmUIGuild *)	pClass;
	AgcdUIControl	*pcsUIGuildSearchControl	= (AgcdUIControl *)	pData1;

	if (pcsUIGuildSearchControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	AcUIEdit *	pcsUIGuildSearchEdit = (AcUIEdit *) pcsUIGuildSearchControl->m_pcsBase;
	if (!pcsUIGuildSearchEdit) return FALSE;

	CHAR szSearchID[AGPACHARACTER_MAX_ID_STRING+1];
	memset(szSearchID,0,AGPACHARACTER_MAX_ID_STRING+1);

	CHAR *szMemberID = (CHAR*)pcsUIGuildSearchEdit->GetText();
	if (!szMemberID) return FALSE;
	strncpy(szSearchID,szMemberID,AGPACHARACTER_MAX_ID_STRING);

	pcsUIGuildSearchEdit->SetMeActiveEdit();
	pcsUIGuildSearchEdit->ClearText();

	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SYSTEM_MESSAGE_COLOR);
	DWORD dwColor = 0xFFFFFF33;
	if(szColor) dwColor = atoi(szColor);

	if (strlen(szSearchID) < 1 || pThis->m_pcsAgpmCharacter->CheckCharName(szSearchID, FALSE) == FALSE) 
	{
		CHAR *szBuffer = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_INVALID_SEARCH_ID);
		if( szBuffer )
			SystemMessage.ProcessSystemMessage(szBuffer, dwColor);

		return FALSE;
	}

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;

	INT32 lCount = pThis->m_pcsAgcmGuild->SearchGuildMember(pcsGuild, szSearchID);
	pcsGuild->m_Mutex.Release();

	if (lCount == -1)
	{
		CHAR* szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_NO_EXIST_MEMBER);
		if( szMessage )
		{
			CHAR szBuffer[512];
			sprintf( szBuffer, szMessage, szSearchID );
			SystemMessage.ProcessSystemMessage(szBuffer, dwColor);
		}

		return FALSE;
	}
	else
	{
		INT32 lPage = 0;
		INT32 lSeletedIndex = 0;
		if (lCount > 0)
		{
			lPage = lCount / AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST;
			lSeletedIndex = lCount % AGCMUIGUILD_MAX_VISIBLE_MEMBER_LIST;
		}
		
		if (lPage == pThis->m_lCurrentPage)	//	���� ������ �̸� ���� �κи� �ٲ۴�.
		{
			pThis->SelectMember(lSeletedIndex);
		}
		else
		{
			pThis->m_lCurrentPage = lPage;
			pThis->RefreshMemberPageUI(lSeletedIndex);
	
		}
	}
	return ( TRUE );
}

/*
	2005.07.20. By SungHoon
	��� �ο����� ������ ����Ʈ �����ֱ�
*/
BOOL AgcmUIGuild::CBDisplayGuildMemberListPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl )
{
	if(!pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if (pThis->m_lGuildMemberCount <= 0)
	{
		((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText("");
		return TRUE;
	}

//	INT32 ii = pThis->m_lCurrentPage;
//	INT32 jj = pThis->m_lMaxMemberListPage;
//	pThis->m_lCurrentPage = 5;
//	pThis->m_lMaxMemberListPage = 13;

	INT32 lFirstPage = pThis->CalcFirstPage(pThis->m_lCurrentPage, pThis->m_lMaxMemberListPage);
	INT32 lLastPage = min(pThis->m_lMaxMemberListPage, lFirstPage + AGCMUIGUILD_MAX_VIEW_PAGE);

	UINT32 lDeselectColor = 4286611584;
	UINT32 lSelectColor = 4294963400;

	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_SELECT_PAGE_COLOR);
	if(szColor) lSelectColor = atol(szColor);

	szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_DESELECT_PAGE_COLOR);
	if(szColor) lDeselectColor = atol(szColor);

	CHAR szBuffer[512] = { 0 };
	for ( INT i = lFirstPage ; i <= lLastPage; i++ )
	{
		CHAR szPage[ 50 ];
		if (i == pThis->m_lCurrentPage) wsprintf(szPage, "<C%d>%d", lSelectColor, i+1);
		else wsprintf(szPage, "<C%d>%d", lDeselectColor, i + 1);
		if (i != 0) strcat(szBuffer, " ");
		strcat(szBuffer, szPage);
	}
//	pThis->m_lCurrentPage = ii;
//	pThis->m_lMaxMemberListPage = jj;

	((AcUIEdit*)pcsSourceControl->m_pcsBase)->SetText(szBuffer);
	return TRUE;
}

/*
	2005.07.20. By SungHoon
	��� �ο����� �ִ� ������ �����ֱ�
*/
BOOL AgcmUIGuild::CBDisplayGuildMemberListMaxPage(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl )
{
	if(!pClass || eType != AGCDUI_USERDATA_TYPE_INT32 || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if (pThis->m_lGuildMemberCount > 0)
	{
		CHAR szBuffer[ 50 ];
		wsprintf( szBuffer, "%d", (pThis->m_lMaxMemberListPage+1));
		strcpy(szDisplay, szBuffer);
	}
	else szDisplay[0] = '\0';

	return TRUE;
}

/*
	2005.07.21. By SungHoon
	���� ���⿡�� �Ӹ������� ��ư�� ������ ���
*/
BOOL AgcmUIGuild::CBGuildClickWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventWhisperChat);
	return TRUE;
}

/*
	2005.07.21. By SungHoon
	���� ���⿡�� ��Ƽ �ʴ� ��ư�� ������ ���
*/
BOOL AgcmUIGuild::CBGuildClickInviteParty(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgcdGuildMember *pcsGuildMember = pThis->GetGuildMember( pThis->m_lSelectedMemberIndex );
	if( !pcsGuildMember )	return TRUE;
		
	pThis->m_bGuildInfoUIOpened = FALSE;		//	��� UI �ݴ´�
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);

	strncpy( pThis->m_pcsAgcmUIPartyOption->m_szDefaultID, pcsGuildMember->m_szMemberID, AGPACHARACTER_MAX_ID_STRING);
	pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIPartyOption->m_lEventPartyOptionUIOpenClick);

	return TRUE;
}

/*
	2005.07.22. By SungHoon
	�������⿡�� �Ӹ� �����⸦ Ŭ������ ���
*/
BOOL AgcmUIGuild::CBGuildWhisperChat(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild		*pThis				= (AgcmUIGuild *)	pClass;

	if (strlen(pThis->m_szSelectedMemberID) <= 0) return FALSE;

	pThis->m_bGuildInfoUIOpened = FALSE;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMemberUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenChatUI);

	pThis->m_pcsAgcmUIChatting2->SetWhisperTo(pThis->m_szSelectedMemberID);

	/*
	AgcdUIControl	*pcsUIChatControl	= (AgcdUIControl *)	pData1;
	AcUIEdit *	pcsUIChatEdit = (AcUIEdit *) pcsUIChatControl->m_pcsBase;
	if (!pcsUIChatEdit) return FALSE;

	CHAR szBuffer[512];
	wsprintf(szBuffer,"%s %s ", ClientStr().GetStr(STI_CHAT_WHISPER_LOWER), pThis->m_szSelectedMemberID);
	pcsUIChatEdit->SetText(szBuffer);

	pcsUIChatEdit->SetMeActiveEdit();
	*/


	return TRUE;

}

/*
	��� ���� ��û�� �㰡 �� ���
*/
BOOL AgcmUIGuild::CBGuildJoinAllowMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	AgcmUIGuild	*pThis	= (AgcmUIGuild *)	pClass;

	AgcdGuildMember* pMember = pThis->GetGuildMember( pThis->m_lSelectedMemberIndex );
	if( !pMember )		return FALSE;

	if ( pMember->m_lRank != AGPMGUILD_MEMBER_RANK_JOIN_REQUEST)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
		pThis->m_pcsAgcmGuild->SendJoinAllow( pMember->m_szMemberID );
	return TRUE;
}

/*
	��� Ż�� ��û�� �㰡 �� ���
*/
BOOL AgcmUIGuild::CBGuildLeaveAllowMember(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	AgcmUIGuild	*pThis	= (AgcmUIGuild *)	pClass;

	AgcdGuildMember* pMember = pThis->GetGuildMember( pThis->m_lSelectedMemberIndex );
	if( !pMember )		return FALSE;

	if( pMember->m_lRank != AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST )
		return FALSE;

	if( lTrueCancel == (INT32) TRUE )
		pThis->m_pcsAgcmGuild->SendLeaveAllow(pMember->m_szMemberID);

	return TRUE;
}

/*
	2005.08.24. By SungHoon
	����� ���⿡�� �����㰡 Ż�� ��ư�ؽ�Ʈ	
*/
BOOL AgcmUIGuild::CBDisplayGuildAllowButton(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;
	CHAR *szMessage =  pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_MEMBERLIST_JOIN_ALLOW_BUTTON_TEXT);

	AgcdGuildMember* pMember = pThis->GetGuildMember( pThis->m_lSelectedMemberIndex );
	if( pMember )
	{
		if( pMember->m_lRank == AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST )
			szMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_MEMBERLIST_LEAVE_ALLOW_BUTTON_TEXT);
	}

	if( szMessage )		strcpy( szDisplay, szMessage);
	else				szDisplay[0] = '\0';

	return TRUE;
}

/*
	2005.08.24. By SungHoon
	��� ������� �㰡 ��ư�� Ŭ��������
*/
BOOL AgcmUIGuild::CBGuildAllowClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if( pThis->GetGuildMember( pThis->m_lSelectedMemberIndex ) )
		pThis->AllowSelectMember();

	return TRUE;
}

VOID AgcmUIGuild::AllowSelectMember()
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if( !pcsCharacter )		return;
	AgcdGuildMember* pMember = GetGuildMember( m_lSelectedMemberIndex );
	if( !pMember )			return;

	if( m_pcsAgpmGuild->IsMaster( m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID )	||
		m_pcsAgpmGuild->IsSubMaster( m_pcsAgcmGuild->GetSelfGuildID() , pcsCharacter->m_szID)  )
	{
		if ( AGPMGUILD_MEMBER_RANK_JOIN_REQUEST == pMember->m_lRank )
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventConfirmAllowJoin);

		if ( AGPMGUILD_MEMBER_RANK_LEAVE_REQUEST == pMember->m_lRank &&
			 m_pcsAgpmGuild->IsMaster( m_pcsAgcmGuild->GetSelfGuildID(), pcsCharacter->m_szID ) )
			m_pcsAgcmUIManager2->ThrowEvent(m_lEventConfirmAllowLeave);
	}
}

// ��� �α渶 �Ӹ� �� ���� ��ư�� ������
BOOL AgcmUIGuild::CBGuildAppointClick( PVOID pClass , PVOID pData1 , PVOID pData2 , PVOID pData3 , PVOID pData4 , PVOID pData5 , ApBase* pcsTarget , AgcdUIControl* pcsSourceControl )
{
	if( !pClass )	return FALSE;

	AgcmUIGuild*	pThis			=	static_cast< AgcmUIGuild* >(pClass );
	AgpdCharacter*	pcsCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	CHAR*			szGuildID		=	pThis->m_pcsAgcmGuild->GetSelfGuildID();
	AgpdGuild*		pdGuild			=	pThis->m_pcsAgpmGuild->GetGuild( pcsCharacter );
	INT				nRank			=	0;
	INT				nBeforeRank		=	0;

	if( !szGuildID	||	!strlen(szGuildID)	)	return FALSE;
	if( !pcsCharacter )							return FALSE;

	// ������ ����� �α渶 ���ٸ�... �α渶 ���� ��Ŷ�� ������
	if( pThis->m_pcsAgpmGuild->IsSubMaster( szGuildID , pThis->m_szSelectedMemberID ) )		
	{
		nRank		=	AGPMGUILD_MEMBER_RANK_NORMAL;
		nBeforeRank	=	AGPMGUILD_MEMBER_RANK_SUBMASTER;
	}

	else
		nRank	=	AGPMGUILD_MEMBER_RANK_SUBMASTER;

	// ���� �κ��� ���̾�α׸� ���� ����;
	if( nRank	==	AGPMGUILD_MEMBER_RANK_NORMAL )
	{
		CHAR*	szUIMessage	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_GUILD_APPOINTCANCEL_CONFIRM );
		CHAR	szMessage[ 300 ];

		sprintf_s( szMessage , 300 , szUIMessage , szGuildID , pThis->m_szSelectedMemberID , pThis->GetMemberRankText(nBeforeRank) );

		if( !pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage ) )
			return TRUE;

	}

	///// ���⼭ ��Ŷ�� ������
	return pThis->m_pcsAgcmGuild->SendAppointRequest( pcsCharacter->m_szID , pThis->m_szSelectedMemberID , nRank );
	////////////////////////////////
}

// ��� �°� ��ư�� ������.
// ����� , ������ , �����Ǿ������ �߿��� ���°��ư�� ��Ȱ�� �˴ϴ�
// ���õ� ������ �������� �����϶��� ��ư�� ��Ȱ��
BOOL AgcmUIGuild::CBGuildSuccessionClick( PVOID pClass , PVOID pData1 , PVOID pDta2 , PVOID pData3 , PVOID pData4 , PVOID pData5 , ApBase* pcsTarget , AgcdUIControl* pcsSourceControl )
{
	if( !pClass	)	return FALSE;

	AgcmUIGuild*	pThis			=	static_cast< AgcmUIGuild* >(pClass);
	AgpdCharacter*	pcsCharacter	=	pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	CHAR*			szGuildID		=	pThis->m_pcsAgcmGuild->GetSelfGuildID();
	AgpdGuild*		pdGuild			=	pThis->m_pcsAgpmGuild->GetGuild( pcsCharacter );

	if( !szGuildID	|| !strlen(szGuildID) )				return FALSE;
	if( !pcsCharacter )									return FALSE;

	// ���°� ��ư�� �������� ó���� ���ָ� �ȴ�....
	return pThis->m_pcsAgcmGuild->SendSuccessionRequest( pcsCharacter->m_szID , pThis->m_szSelectedMemberID );
}

// �Ӹ� �޼��� �ݹ� 
BOOL AgcmUIGuild::CBGuildAppointAsk( PVOID pData , PVOID pClass , PVOID pCustData )
{
	PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK*		pAsk
		=	static_cast< PACKET_GUILD_CLASS_SOCIETY_APPOINTMENT_ASK* >(pData);

	AgcmUIGuild*	pThis	=	static_cast< AgcmUIGuild* >(pClass);

	if( !pAsk || !pThis )		return FALSE;

	CHAR	szRankName[ 50 ];

	// ��ũ�� �´� ������ �����´�
	switch( pAsk->Rank )
	{
	case	AGPMGUILD_MEMBER_RANK_NORMAL:		strcpy_s( szRankName , 50 , pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_MEMBER ) );
	case	AGPMGUILD_MEMBER_RANK_SUBMASTER:	strcpy_s( szRankName , 50 , pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_ID_GUILD_SUBMASTER) );
	}

	CHAR*		szUIMessage	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_GUILD_APPOINT_CONFIRM );
	CHAR		szMessage[ 300 ];
	sprintf_s( szMessage , 300 , szUIMessage , pAsk->strGuildID , pAsk->strMasterID , pAsk->strChar , szRankName  );
	INT32		Answer =  pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage );

	return pThis->m_pcsAgcmGuild->SendAppointAnswer( pAsk->strMasterID , pAsk->strChar , pAsk->Rank , Answer );
}

// �ý��� �޽��� ó�� �ݹ�
BOOL AgcmUIGuild::CBGuildAppointSysMsg( PVOID pData , PVOID pClass , PVOID pCustData )
{
	PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE*		pAsk
		=	static_cast< PACKET_GUILD_CLASS_SOCIETY_SYSTEM_MESSAGE* >(pData);

	AgcmUIGuild*	pThis				=	static_cast< AgcmUIGuild* >(pClass);

	if( !pAsk || !pThis )	return FALSE;

	// ���߿� ��Ŷ������ �ٲ�� �Ʒ� ���ǹ��� �� �ٲ���� �Ѵ�
	if	( 1 )	SystemMessage.ProcessSystemMessage( pAsk->strMessage , 0x00 , AGPMSYSTEMMESSAGE_PACKET_MODAL_MESSAGE );
	else 		SystemMessage.ProcessSystemMessage( pAsk->strMessage );

	return TRUE;
}

BOOL AgcmUIGuild::CBGuildSuccessionAsk( PVOID pData , PVOID pClass , PVOID pCustData )
{

	PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK	*pAsk	
		=	static_cast< PACKET_GUILD_CLASS_SOCIETY_SUCCESSION_ASK* >(pData);

	AgcmUIGuild*	pThis	=	static_cast< AgcmUIGuild* >(pClass);

	CHAR*		szUIMessage	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_GUILD_SUCCESSION_CONFIRM );
	CHAR		szMessage[ 300 ];

	if( !szUIMessage )	return FALSE;
	sprintf_s( szMessage , 300 , szUIMessage , pAsk->strGuildID , pAsk->strMasterID , pAsk->strChar );

	INT32		Answer	=	pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog( szMessage );
	return pThis->m_pcsAgcmGuild->SendSuccessionAnswer( pAsk->strMasterID , pAsk->strChar , Answer );
}

// ��� ��� ��ũ ���� �ݹ� �Լ�
BOOL AgcmUIGuild::CBGuildMemberRankChange( PVOID pData , PVOID pClass , PVOID pCustData )
{
	PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK	*pvPacket
		=	static_cast< PACKET_GUILD_CLASS_SOCIETY_UPDATE_GUILD_MEMBER_RANK* >(pData);

	AgcmUIGuild*	pThis	=	static_cast< AgcmUIGuild* >(pClass );

	CHAR*		szUIMessage	=	pThis->m_pcsAgcmUIManager2->GetUIMessage( UI_MESSAGE_GUILD_MEMBERRANK_UPDATE );
	CHAR		szMessage[ 300 ];

	if( !szUIMessage )	return FALSE;

#ifdef _AREA_GLOBAL_	// ��������� ���Ͽ� �۷ι��� �Ű����� ������ �����Ѵ�.
	sprintf_s( szMessage , 300 , szUIMessage , pvPacket->strCCharID  , pThis->GetMemberRankText( pvPacket->Rank), pvPacket->strGuildID );
#else
	sprintf_s( szMessage , 300 , szUIMessage , pvPacket->strCCharID , pvPacket->strGuildID , pThis->GetMemberRankText( pvPacket->Rank) );
#endif

	AgpdGuild*		pdGuild	=	pThis->m_pcsAgpmGuild->GetGuild( pvPacket->strGuildID );
	if( !pdGuild )	return FALSE;

	// ��ũ�� ���� ��Ų��
	pdGuild->SetMemberRankChange( pvPacket->strCCharID , pvPacket->Rank );

	// �ý��� �޼����� �����ش�
	SystemMessage.ProcessSystemMessage( szMessage );

	return TRUE;
}

BOOL AgcmUIGuild::CBDisplayAppointAndCancelBtnText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl* pcsSourceControl)
{
	if( !pClass || !pcsSourceControl )
		return FALSE;

	AgcmUIGuild* pThis = static_cast<AgcmUIGuild*>(pClass);
	sprintf( szDisplay , "%s" , pThis->m_szAppointAndCancelBtnID );

	return TRUE;
}

