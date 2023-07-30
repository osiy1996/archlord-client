#include <AgcmUIGuild/AgcmUIGuild.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmItem/AgpmItem.h>

/*
	2005.07.15. By SungHoon
	���� ��� �ο��� ����Ѵ�.

*/
BOOL AgcmUIGuild::CBMaxMemberIncreaseUIToggle(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pData1 || !pData2 || !pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	AgcdUIControl	*pcsUIMaxMemberCurrentControl	= (AgcdUIControl *)	pData1;
	AgcdUIControl	*pcsUIMaxMemberNextControl		= (AgcdUIControl *)	pData2;
	if (pcsUIMaxMemberCurrentControl->m_lType != AcUIBase::TYPE_EDIT || pcsUIMaxMemberNextControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	AcUIEdit *	pcsUIMaxMemberCurrentEdit	= (AcUIEdit *) pcsUIMaxMemberCurrentControl->m_pcsBase;
	AcUIEdit *	pcsUIMaxMemberNextEdit		= (AcUIEdit *) pcsUIMaxMemberNextControl->m_pcsBase;

	pcsUIMaxMemberCurrentEdit->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());
	pcsUIMaxMemberNextEdit->SetLineDelimiter(pThis->m_pcsAgcmUIManager2->GetLineDelimiter());

	AgpdGuild* pcsGuild = pThis->m_pcsAgcmGuild->GetSelfGuildLock();
	if( !pcsGuild )		return FALSE;
		
	char szBuffer[512] = { 0 };
	CHAR* szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_CURRENT_MAX_MEMBER);
	if( szUIMessage )
		wsprintf(szBuffer, szUIMessage,	pcsGuild->m_lMaxMemberCount);

	pcsUIMaxMemberCurrentEdit->SetText(szBuffer);

	INT32 lNewGuildMaxMember = NEXT_GUILDMAX_MEMBER(pcsGuild);

	memset(szBuffer, 0, 512);
	AgpdRequireItemIncreaseMaxMember *pcsRequireItem = pThis->m_pcsAgpmGuild->GetRequireIncreaseMaxMember(lNewGuildMaxMember);
	if( pcsRequireItem )
	{
		CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_NEXT_MAX_MEMBER);
		if (szUIMessage)
		{
			CHAR szGheld[512];
			memset(szGheld, 0, 20);
			pThis->MakeCommaNumber(pcsRequireItem->m_lGheld, szGheld, 20);
			if (g_eServiceArea == AP_SERVICE_AREA_GLOBAL)
				wsprintf(szBuffer, szUIMessage,	lNewGuildMaxMember,  szGheld, pcsRequireItem->m_lSkullCount, pcsRequireItem->m_szSkullName);
			else
				wsprintf(szBuffer, szUIMessage,	lNewGuildMaxMember,  szGheld );
		}
	}
	else
	{
		CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_CAN_NOT_INCREASE_MAX_MEMBER);
		if (szUIMessage)
			strcpy(szBuffer, szUIMessage);
	}
	pcsUIMaxMemberNextEdit->SetText(szBuffer);

	pThis->m_bEnableIncreaseMaxMember = FALSE;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if ( pcsCharacter && pcsRequireItem )
	{
		if(pThis->m_pcsAgpmGuild->IsMaster(pcsGuild, pcsCharacter->m_szID))
		{
			if( pcsCharacter->m_llMoney >= pcsRequireItem->m_lGheld )
			{
				AgpdItem* pcsItem = pThis->m_pcsAgpmItem->GetInventoryItemByTID( pcsCharacter, pcsRequireItem->m_lSkullTID );
				if( pcsItem )
				{
					if( pcsItem->m_nCount >= pcsRequireItem->m_lSkullCount )
						pThis->m_bEnableIncreaseMaxMember = TRUE;
				}
				else
					pThis->m_bEnableIncreaseMaxMember = TRUE;
			}
		}
	}
	pcsGuild->m_Mutex.Release();
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstEnableIncreaseMaxMember );

	return  TRUE;
}

/*
	2005.07.15. By SungHoon
	��� �ִ� �ο� Ȯ�� ��ư�� ������ ��� ������ ������ ��û�Ѵ�.
*/
BOOL AgcmUIGuild::CBGuildMaxMemberIncreaseOK(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMaxMemberIncreaseOK);

	return  TRUE;
}

/*
	2005.07.18. By SungHoon
	��� �ִ� �ο� ���� UI�� ���� �ִ��� Ȯ�� �� ����.
*/
BOOL AgcmUIGuild::CBGuildMaxMemberIncreaseOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if(strlen(pThis->m_szSelfGuildID) == 0)	// ��忡 ���� �ȵǾ� �ִ� ������
		return TRUE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsCharacter)
		return TRUE;

	// �����Ͱ� �ƴϸ� ������.
	if(pThis->m_pcsAgpmGuild->IsMaster(pThis->m_szSelfGuildID, pcsCharacter->m_szID) == FALSE)
		return TRUE;

	AgpdGuild *pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(pThis->m_szSelfGuildID);
	if( !pcsGuild )		return TRUE;

	INT32 lMemberCount = pThis->m_pcsAgpmGuild->GetMemberCount(pcsGuild);
	pcsGuild->m_Mutex.Release();

	if( lMemberCount < AGPMGUILD_GUILDMEMBER_INCREASE_ENABLE_MEMBER_COUNT )
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMemberUpNotEnoughtGuildMemberCount);
		return TRUE;
	}

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildMainUI);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMaxMemberIncreaseUIOpen );
	
	return TRUE;
}

/*
	2005.07.18. By SungHoon
	���� ��û Yes/No �ڽ����� Ŭ������ ��� �Ҹ���.
*/
BOOL AgcmUIGuild::CBEventReturnGuildMaxMemberIncreaseOK(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	AgcmUIGuild* pThis = (AgcmUIGuild*)pClass;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
		if (!pcsCharacter) return FALSE;

		AgpdGuildADChar* pcsAttachedGuild = pThis->m_pcsAgpmGuild->GetADCharacter(pcsCharacter);
		if(!pcsAttachedGuild)
			return FALSE;

		AgpdGuild* pcsGuild = pThis->m_pcsAgpmGuild->GetGuildLock(pcsAttachedGuild->m_szGuildID);
		if(!pcsGuild)
			return FALSE;

		INT32 lNewGuildMaxMember = NEXT_GUILDMAX_MEMBER(pcsGuild);

		pcsGuild->m_Mutex.Release();

		pThis->m_pcsAgcmGuild->SendMaxMemberCount(pcsAttachedGuild->m_szGuildID,pcsCharacter->m_lID,lNewGuildMaxMember);

	}
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGuildMaxMemberIncreaseUIClose);

	return TRUE;
}

