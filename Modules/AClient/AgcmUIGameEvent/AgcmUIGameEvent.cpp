#include "AgcmUIGameEvent.h"
#include <AuTimeStamp/AuTimeStamp.h>
#include <AuLocalize/AuRegionNameConvert.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <AgpmGameEvent/AgpmGameEvent.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUICharacter/AgcmUICharacter.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgpmChatting/AgpmChatting.h>

static const char *
uiMsgIdFromSystemMessage(AgpmGameEventSystemMessage eMsg)
{
	switch (eMsg) {
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_REGISTER_CLOSED:
		return "GameEvent_Register_Closed";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_LEVEL:
		return "GameEvent_Invalid_Level";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_NAME_EXISTS:
		return "GameEvent_Name_Exists";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_TARGET:
		return "GameEvent_Invalid_Target";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_ONLY_LEADER_CAN_INVITE:
		return "GameEvent_Only_Leader_Can_Invite";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_REQ_UNIQUE_CLASS:
		return "GameEvent_Req_Unique_Class";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_JOINED:
		return "GameEvent_Member_Joined";
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_LEFT:
		return "GameEvent_Member_Left";
	default:
		return NULL;
	}
}

AgcmUIGameEvent::AgcmUIGameEvent()
{
	SetModuleName(_T("AgcmUIGameEvent"));
	memset(&m_stTeam, 0, sizeof(m_stTeam));
	m_pstUDTeam = NULL;
	m_pstUDList = NULL;
}

AgcmUIGameEvent::~AgcmUIGameEvent()
{
}

BOOL
AgcmUIGameEvent::OnAddModule()
{
	m_pAgpmGameEvent = (AgpmGameEvent *)GetModule(_T("AgpmGameEvent"));
	m_pAgpmCharacter = (AgpmCharacter *)GetModule(_T("AgpmCharacter"));
	m_pAgcmCharacter = (AgcmCharacter *)GetModule(_T("AgcmCharacter"));
	m_pAgcmUIManager2 = (AgcmUIManager2 *)GetModule(_T("AgcmUIManager2"));
	m_pAgcmUIControl = (AgcmUIControl *)GetModule(_T("AgcmUIControl"));
	m_pAgcmUIMain = (AgcmUIMain *)GetModule(_T("AgcmUIMain"));
	m_pAgcmChatting2 = (AgcmChatting2 *)GetModule(_T("AgcmChatting2"));
	m_pAgcmUICharacter = (AgcmUICharacter *)GetModule(_T("AgcmUICharacter"));
	if (!m_pAgpmGameEvent || !m_pAgpmCharacter || !m_pAgcmCharacter || 
		!m_pAgcmUIManager2 || !m_pAgcmUIControl || !m_pAgcmUIMain || 
		!m_pAgcmChatting2 || !m_pAgcmUICharacter)
		return FALSE;	
	if (!AddFunction() || !AddDisplay() || !AddUserData())
		return FALSE;
	m_lEventOnInvite = m_pAgcmUIManager2->AddEvent(
		"Event_E5_OnInvite",
		CBEventReturnInvitation,
		this);
	if (!m_lEventOnInvite)
		return FALSE;
	if (!m_pAgpmGameEvent->SetCallbackSetTeam(CBSetTeam, this) ||
		!m_pAgpmGameEvent->SetCallbackInvitation(CBInvitation, this) ||
		!m_pAgpmGameEvent->SetCallbackLeaveOrDisband(CBLeaveOrDisband, this) ||
		!m_pAgpmGameEvent->SetCallbackSetStatus(CBSetStatus, this) ||
		!m_pAgpmGameEvent->SetCallbackSystemMessage(CBSystemMessage, this))
		return FALSE;
	return TRUE;
}

BOOL
AgcmUIGameEvent::OnInit()
{
	return TRUE;
}

BOOL
AgcmUIGameEvent::AddFunction()
{
	if (!m_pAgcmUIManager2->AddFunction(
		this,
		"E5_KickMember",
		CBOnKickMember, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(
		this,
		"E5_DisbandOrLeave",
		CBOnDisbandOrLeave, 0))
		return FALSE;
	if (!m_pAgcmUIManager2->AddFunction(
		this,
		"E5Dlg_ExecDialog",
		CBOnExecDialog, 1))
		return FALSE;
	if (!m_pAgcmUIManager2->AddBoolean(
		this,
		"E5_IsCreateOrInviteEnabled",
		CBIsCreateOrInviteEnabled,
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	return TRUE;
}

BOOL
AgcmUIGameEvent::AddDisplay()
{
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_MemberClass",
		0, CBDisplayMemberClass, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_MemberName",
		0, CBDisplayMemberName, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_MemberLevel",
		0, CBDisplayMemberLevel, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_MemberKick",
		0, CBDisplayMemberKick, 
		AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_TeamName",
		0, CBDisplayTeamName, 
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_TeamStatus",
		0, CBDisplayTeamStatus, 
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_CreateOrInvite",
		0, CBDisplayCreateOrInvite, 
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_DisbandOrLeave",
		0, CBDisplayDisbandOrLeave, 
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5Dlg_Display_DialogTitle",
		0, CBDisplayDialogTitle, 
		AGCDUI_USERDATA_TYPE_ETC))
		return FALSE;
	if (!m_pAgcmUIManager2->AddDisplay(
		this,
		"E5_Display_TeamInviteFrom",
		0, CBDisplayTeamInviteFrom,
		AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	return TRUE;
}

BOOL
AgcmUIGameEvent::AddUserData()
{
	m_pstUDTeam = m_pAgcmUIManager2->AddUserData(
		"E5_UD",
		&m_stTeam, sizeof(m_stTeam),
		1, AGCDUI_USERDATA_TYPE_ETC);
	m_pstUDList = m_pAgcmUIManager2->AddUserData(
		"E5_List_UD",
		m_stTeam.m_stMember, sizeof(AgpdGameEventTeamMember),
		AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT,
		AGCDUI_USERDATA_TYPE_INT32);
	if (!m_pstUDTeam || !m_pstUDList)
		return FALSE;
	if (!m_pAgcmUIManager2->AddUserData(
		"E5_UDTeamInvite",
		m_szTeamInviteFrom,
		sizeof(m_szTeamInviteFrom),
		1,
		AGCDUI_USERDATA_TYPE_STRING))
		return FALSE;
	return TRUE;
}

const char *
AgcmUIGameEvent::GetMessageTxt(AgpmGameEventSystemMessage eMessage)
{
	return m_pAgcmUIManager2->GetUIMessage(
		uiMsgIdFromSystemMessage(eMessage));
	/*
	if (pszMessage && _tcslen(pszMessage) > 0)
		pThis->m_pAgcmUIManager2->ActionMessageOKDialog(pszMessage);
	*/
}

void
AgcmUIGameEvent::AddSystemMessage(CHAR * pszMessage)
{
	AgpdChatData stChatData;

	if (!pszMessage)
		return;
	ZeroMemory(&stChatData, sizeof(stChatData));
	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = pszMessage;
	stChatData.lMessageLength = strlen(stChatData.szMessage);
	m_pAgcmChatting2->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);
}

BOOL
AgcmUIGameEvent::CBSetTeam(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventCallbackData * pstData = (AgpdGameEventCallbackData *)pData;

	if (pstData->m_pstTeam)
		memcpy(&pThis->m_stTeam, pstData->m_pstTeam, sizeof(pThis->m_stTeam));
	else
		memset(&pThis->m_stTeam, 0, sizeof(pThis->m_stTeam));
	pThis->RefreshUD();
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventCallbackData * pstData = (AgpdGameEventCallbackData *)pData;

	if (pstData->m_pstTeam) {
		memcpy(pThis->m_szTeamInviteFrom, pstData->m_pstTeam->m_szName,
			AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN);
		pThis->m_pAgcmUIManager2->ThrowEvent(pThis->m_lEventOnInvite);
	}
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBLeaveOrDisband(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventCallbackData * pstData = (AgpdGameEventCallbackData *)pData;

	memset(&pThis->m_stTeam, 0, sizeof(pThis->m_stTeam));
	pThis->RefreshUD();
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBSetStatus(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventCallbackData * pstData = (AgpdGameEventCallbackData *)pData;

	strcpy(pThis->m_stTeam.m_szStatus, pstData->m_pstTeam->m_szStatus);
	pThis->RefreshUD();
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventCallbackData * pstData = (AgpdGameEventCallbackData *)pData;
	const char * pszUIMsg = pThis->GetMessageTxt(pstData->m_eSystemMessage);
	char szMsg[512];

	if (!pszUIMsg)
		return FALSE;
	switch (pstData->m_eSystemMessage) {
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_REGISTER_CLOSED:
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_NAME_EXISTS:
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_TARGET:
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_REQ_UNIQUE_CLASS:
		strcpy_s(szMsg, sizeof(szMsg), pszUIMsg);
		break;
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_LEVEL:
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_JOINED:
	case AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_LEFT:
		if (!pstData->m_pszSystemMessageArg)
			return FALSE;
		sprintf_s(szMsg, sizeof(szMsg), pszUIMsg, 
			pstData->m_pszSystemMessageArg);
		break;
	default:
		return FALSE;
	}
	pThis->AddSystemMessage(szMsg);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayMemberClass(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *			pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventTeamMember *	pMember = (AgpdGameEventTeamMember *)pData;
	INT32						lStatus = 0;

	switch (pMember->m_ulTemplateId) {
	case 96:
		lStatus = 1;
		break;
	case 1:
		lStatus = 2;
		break;
	case 6:
		lStatus = 3;
		break;
	case 4:
		lStatus = 4;
		break;
	case 8:
		lStatus = 5;
		break;
	case 3:
		lStatus = 6;
		break;
	case 377:
		lStatus = 7;
		break;
	case 460:
		lStatus = 8;
		break;
	case 9:
		lStatus = 9;
		break;
	default:
		break;
	}
	pcsSourceControl->m_pcsBase->SetStatus(lStatus, FALSE);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayMemberName(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *			pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventTeamMember *	pMember = (AgpdGameEventTeamMember *)pData;

	strcpy(szDisplay, pMember->m_szName);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayMemberLevel(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *			pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventTeamMember *	pMember = (AgpdGameEventTeamMember *)pData;
	const char *				pszTag = "";

	if (!pMember->m_ulLevel) {
		strcpy(szDisplay, "");
		return TRUE;
	}
	switch (pMember->m_ulTemplateId) {
	case 96:
		pszTag = "Knight";
		break;
	case 1:
		pszTag = "Archer";
		break;
	case 6:
		pszTag = "Mage";
		break;
	case 4:
		pszTag = "Berserker";
		break;
	case 8:
		pszTag = "Hunter";
		break;
	case 3:
		pszTag = "Sorcerer";
		break;
	case 377:
		pszTag = "Swashbuckler";
		break;
	case 460:
		pszTag = "Ranger";
		break;
	case 9:
		pszTag = "Elementalist";
		break;
	}
	sprintf(szDisplay, "Lv. %u %s", pMember->m_ulLevel, pszTag);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayMemberKick(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *			pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventTeamMember *	pMember = (AgpdGameEventTeamMember *)pData;
	AgpdCharacter *				pSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();

	if (pMember != &pThis->m_stTeam.m_stMember[0] &&
		pMember->m_szName[0] &&
		pSelf &&
		strcmp(pThis->m_stTeam.m_stMember[0].m_szName, pSelf->m_szID) == 0) {
		strcpy(szDisplay, "Kick");
		pcsSourceControl->m_pcsBase->ShowWindow(TRUE);
	}
	else {
		pcsSourceControl->m_pcsBase->ShowWindow(FALSE);
	}
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayTeamName(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;

	if (pThis->m_stTeam.m_szName[0])
		sprintf(szDisplay, "[%s]", pThis->m_stTeam.m_szName);
	else
		strcpy(szDisplay, "");
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayTeamStatus(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;

	strcpy(szDisplay, pThis->m_stTeam.m_szStatus);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayCreateOrInvite(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	AgpdCharacter *		pSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();

	if (pThis->m_stTeam.m_szName[0])
		strcpy(szDisplay, "Invite");
	else
		strcpy(szDisplay, "Create");
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayDisbandOrLeave(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *			pThis = (AgcmUIGameEvent *)pClass;
	AgpdCharacter *				pSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();

	if (pSelf &&
		strcmp(pThis->m_stTeam.m_stMember[0].m_szName, pSelf->m_szID) == 0)
		strcpy(szDisplay, "Disband");
	else
		strcpy(szDisplay, "Leave");
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayDialogTitle(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	AgpdCharacter *		pSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();

	if (pSelf &&
		!pThis->m_stTeam.m_szName[0])
		strcpy(szDisplay, "Please input your team name:");
	else
		strcpy(szDisplay, "Please input player name:");
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBDisplayTeamInviteFrom(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	INT32			lID,
	CHAR *			szDisplay,
	INT32 *			plValue)
{
	
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;

	strcpy(szDisplay, pThis->m_szTeamInviteFrom);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBOnKickMember(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	AgpdGameEventTeam *	pTeam = &pThis->m_stTeam;
	INT16				nPacketLength = 0;
	PVOID				pPacket;

	pPacket = pThis->m_pAgpmGameEvent->MakePacketKickMember(
		pTeam->m_stMember[pcsSourceControl->m_lUserDataIndex].m_szName,
		&nPacketLength);
	if (!pPacket || nPacketLength < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nPacketLength);
	pThis->m_pAgpmGameEvent->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBOnDisbandOrLeave(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	INT16				nPacketLength = 0;
	PVOID				pPacket;

	pPacket = pThis->m_pAgpmGameEvent->MakePacketDisbandOrLeave(
		&nPacketLength);
	if (!pPacket || nPacketLength < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nPacketLength);
	pThis->m_pAgpmGameEvent->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBOnExecDialog(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	AgcdUIControl *		pcsControl = (AgcdUIControl *)pData1;
	const char *		pszValue = ((AcUIEdit *)pcsControl->m_pcsBase)->GetText();
	INT16				nPacketLength = 0;
	PVOID				pPacket = NULL;

	if (!pThis->m_stTeam.m_szName[0]) {
		pPacket = pThis->m_pAgpmGameEvent->MakePacketCreateTeam(
			pszValue,
			&nPacketLength);
	}
	else if (strcmp(
		pThis->m_stTeam.m_stMember[0].m_szName, 
		pThis->m_pAgcmCharacter->GetSelfCharacter()->m_szID) == 0) {
		pPacket = pThis->m_pAgpmGameEvent->MakePacketInviteMember(
			pszValue,
			&nPacketLength);
	}
	((AcUIEdit *)pcsControl->m_pcsBase)->SetText("");
	if (!pPacket || nPacketLength < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nPacketLength);
	pThis->m_pAgpmGameEvent->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBIsCreateOrInviteEnabled(
	PVOID			pClass,
	PVOID			pData,
	AgcdUIDataType	eType,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIGameEvent * pThis = (AgcmUIGameEvent *)pClass;

	if (pThis->m_stTeam.m_szName[0]) {
		AgpdCharacter * pSelf = pThis->m_pAgcmCharacter->GetSelfCharacter();

		return (pSelf &&
			strcmp(pSelf->m_szID, pThis->m_stTeam.m_stMember[0].m_szName) == 0);
	}
	return TRUE;
}

BOOL
AgcmUIGameEvent::CBEventReturnInvitation(
	PVOID	pClass,
	INT32	lConfirmed,
	CHAR *	pszInputMessage)
{
	AgcmUIGameEvent *	pThis = (AgcmUIGameEvent *)pClass;
	INT16				nPacketLength = 0;
	PVOID				pPacket;

	pPacket = pThis->m_pAgpmGameEvent->MakePacketRespondInvitation(
		pThis->m_szTeamInviteFrom,
		lConfirmed != 0,
		&nPacketLength);
	if (!pPacket || nPacketLength < 1)
		return FALSE;
	pThis->SendPacket(pPacket, nPacketLength);
	pThis->m_pAgpmGameEvent->m_csPacket.FreePacket(pPacket);
	return TRUE;
}

void
AgcmUIGameEvent::RefreshUD()
{
	m_pAgcmUIManager2->RefreshUserData(m_pstUDTeam, TRUE);
	m_pAgcmUIManager2->RefreshUserData(m_pstUDList, TRUE);
}
