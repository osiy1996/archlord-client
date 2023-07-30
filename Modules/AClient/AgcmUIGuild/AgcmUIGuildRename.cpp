// AgcmUIGuildBattle.cpp
// steeple, 2005. 08. 26.

#include <AgcmUIGuild/AgcmUIGuild.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmGuild/AgpmGuild.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>

/*
	2005.08.26 By SungHoon
	��� UI �̺�Ʈ�� ���ϴ� ���Ҹ� �Ѵ�.
*/

BOOL AgcmUIGuild::CBGuildRenameUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;

	AgcmUIGuild		*pThis				= (AgcmUIGuild *)	pClass;

//	�渶���� Ȯ�� �ϱ�
	if(!pThis->m_pcsAgcmGuild->IsSelfGuildMaster()) return FALSE;

	if (!pThis->m_pcsAgpmGuild->IsDuplicatedCharacterOfMigration(pThis->m_pcsAgcmGuild->GetSelfGuildID()))
		return FALSE;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventOpenGuildRenameUI);		
	return TRUE;
}

/*
	2005.08.26. By SungHoon
	��� UI ���� ��Ʈ�ѵ��� ������ �ʱ�ȭ �Ѵ�.
*/
BOOL AgcmUIGuild::CBGuildRenameUIInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pData2) return FALSE;

	AgcmUIGuild		*pThis				= (AgcmUIGuild *)	pClass;

	AgcdUIControl	*pcsOriginalIDControl = (AgcdUIControl *)pData1;
	AgcdUIControl	*pcsRenameIDControl = (AgcdUIControl *)pData2;
	if (pcsOriginalIDControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	if (pcsRenameIDControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	AcUIEdit *	pcsOriginalIDEdit = (AcUIEdit *) pcsOriginalIDControl->m_pcsBase;
	AcUIEdit *	pcsRenameIDEdit = (AcUIEdit *) pcsRenameIDControl->m_pcsBase;
	CHAR *szSelfGuildID = pThis->m_pcsAgcmGuild->GetSelfGuildID();
	if (szSelfGuildID) pcsOriginalIDEdit->SetText(szSelfGuildID);
	else pcsOriginalIDEdit->ClearText();
	pcsRenameIDEdit->SetMeActiveEdit();
	pcsRenameIDEdit->ClearText();

	return TRUE;
}

/*
	2005.08.26. By SungHoon
	����̸� �ٲٱ� ��ư�� Ŭ���ߴ�.
*/
BOOL AgcmUIGuild::CBGuildRenameOKClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pData2) return FALSE;

	AgcmUIGuild		*pThis				= (AgcmUIGuild *)	pClass;

	AgcdUIControl	*pcsOriginalIDControl = (AgcdUIControl *)pData1;
	AgcdUIControl	*pcsRenameIDControl = (AgcdUIControl *)pData2;

	if (pcsRenameIDControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;

	AcUIEdit *	pcsRenameIDEdit = (AcUIEdit *) pcsRenameIDControl->m_pcsBase;
	const CHAR *szRenameGuildID = pcsRenameIDEdit->GetText();
	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsCharacter) return FALSE;

	pThis->m_pcsAgcmGuild->SendGuildRenameGuildID(pcsCharacter->m_lID ,(CHAR *)szRenameGuildID, pcsCharacter->m_szID );

	return TRUE;
}

/*
	2005.08.31. By SungHoon
	��� �̸� �ٲٱⰡ �����ؼ� ��� �̸� �ٲٱ� â�� �ݴ´�.
*/
BOOL AgcmUIGuild::CBGuildRenameGuildID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData) return FALSE;

	AgcmUIGuild *pThis = (AgcmUIGuild *)pClass;
	INT32 *pCID = ( INT32 *)pData;
	PVOID* ppvBuffer = (PVOID*)pCustData;

	CHAR *szGuildID = ( CHAR *)ppvBuffer[0];
	CHAR *szMemberID = ( CHAR *)ppvBuffer[1];

	memset(pThis->m_szSelfGuildID, 0, sizeof(CHAR) * (AGPMGUILD_MAX_GUILD_ID_LENGTH+1));
	strncpy( pThis->m_szSelfGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);


	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventCloseGuildRenameUI);

	CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_GUILD_RENAME_SUCCESS);
	if (szUIMessage) pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog(szUIMessage);

	return TRUE;
}