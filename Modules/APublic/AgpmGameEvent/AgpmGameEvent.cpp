#include <AgpmGameEvent/AgpmGameEvent.h>
#include <AgpmGameEvent/AgpdGameEvent.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <ApmEventManager/ApmEventManager.h>

AgpmGameEvent::AgpmGameEvent()
{
	SetModuleName(_T("AgpmGameEvent"));
	SetModuleType(APMODULE_TYPE_PUBLIC);
	SetPacketType(AGPM_GAME_EVENT_PACKET_TYPE);
	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
		AUTYPE_INT8,	1,	// Operation
		AUTYPE_PACKET,	1,	// Team
		AUTYPE_UINT32,	1,	// System message id
		AUTYPE_CHAR,	256,// System message arg
		AUTYPE_END,		0);
	m_csTeamPacket.SetFlagLength(sizeof(INT8));
	m_csTeamPacket.SetFieldType(
		AUTYPE_CHAR,	AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN,		// Team name
		AUTYPE_CHAR,	AGPM_GAME_EVENT_MAX_TEAM_STATUS_LEN,	// Team status
		AUTYPE_PACKET,	AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT,	// Team member
		AUTYPE_CHAR,	AGPDCHARACTER_MAX_ID_LENGTH + 1,		// Member name
		AUTYPE_END,		0);
	m_csMemberPacket.SetFlagLength(sizeof(INT8));
	m_csMemberPacket.SetFieldType(
		AUTYPE_CHAR,	AGPDCHARACTER_MAX_ID_LENGTH + 1,// Name
		AUTYPE_UINT32,	1,								// Template Id
		AUTYPE_UINT32,	1,								// Level
		AUTYPE_END,		0);
}

AgpmGameEvent::~AgpmGameEvent()
{
}

BOOL
AgpmGameEvent::OnAddModule()
{
	return TRUE;
}

BOOL
AgpmGameEvent::OnReceive(
	UINT32			ulType, 
	PVOID			pvPacket, 
	INT16			nSize, 
	UINT32			ulNID, 
	DispatchArg *	pstCheckArg)
{
	INT8						cOperation = AGPM_GAME_EVENT_OPERATION_NONE;
	PVOID						pTeamPacket = NULL;
	AgpdGameEventTeam			stTeam;
	AgpdGameEventCallbackData	stData;

	stData.m_pstTeam = NULL;
	stData.m_eSystemMessage = AGPM_GAME_EVENT_SYSTEM_MESSAGE_NONE;
	stData.m_pszSystemMessageArg = NULL;
	if (!pvPacket || nSize == 0)
		return FALSE;
	m_csPacket.GetField(TRUE, pvPacket, nSize,
		&cOperation,
		&pTeamPacket,
		&stData.m_eSystemMessage,
		&stData.m_pszSystemMessageArg);
	if (pTeamPacket) {
		const char *	pszName = NULL;
		const char *	pszStatus = NULL;
		const char *	pszMemberName = NULL;
		PVOID			pMemberPacket[AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT];
		UINT			ulCount = 0;

		memset(&stTeam, 0, sizeof(stTeam));
		memset(pMemberPacket, 0, sizeof(pMemberPacket));
		m_csTeamPacket.GetField(FALSE, pTeamPacket, 0,
			&pszName,
			&pszStatus,
			pMemberPacket,
			&pszMemberName);
		if (pszName)
			strcpy_s(stTeam.m_szName, sizeof(stTeam.m_szName), pszName);
		if (pszStatus)
			strcpy_s(stTeam.m_szStatus, sizeof(stTeam.m_szStatus), pszStatus);
		for (UINT i = 0; i < AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT; i++) {
			if (!pMemberPacket[i])
				continue;
			m_csMemberPacket.GetField(FALSE, pMemberPacket[i], 0,
				&pszName,
				&stTeam.m_stMember[ulCount].m_ulTemplateId,
				&stTeam.m_stMember[ulCount].m_ulLevel);
			if (pszName)
				strcpy_s(stTeam.m_stMember[ulCount].m_szName, 
					sizeof(stTeam.m_stMember[ulCount].m_szName), pszName);
			ulCount++;
		}
		stTeam.m_ulMemberCount = ulCount;
		stData.m_pstTeam = &stTeam;
	}
	switch (cOperation) {
	case AGPM_GAME_EVENT_OPERATION_CREATE_TEAM:
		EnumCallback(AGPM_GAME_EVENT_CB_CREATE_TEAM, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_SET_TEAM:
		EnumCallback(AGPM_GAME_EVENT_CB_SET_TEAM, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_INVITATION:
		EnumCallback(AGPM_GAME_EVENT_CB_INVITATION, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_ACCEPT_INVITATION:
		EnumCallback(AGPM_GAME_EVENT_CB_ACCEPT_INVITATION, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_DECLINE_INVITATION:
		EnumCallback(AGPM_GAME_EVENT_CB_DECLINE_INVITATION, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_KICK_MEMBER:
		EnumCallback(AGPM_GAME_EVENT_CB_KICK_MEMBER, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_LEAVE_OR_DISBAND:
		EnumCallback(AGPM_GAME_EVENT_CB_LEAVE_OR_DISBAND, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_SET_STATUS:
		EnumCallback(AGPM_GAME_EVENT_CB_SET_STATUS, &stData, NULL);
		break;
	case AGPM_GAME_EVENT_OPERATION_SYSTEM_MESSAGE:
		EnumCallback(AGPM_GAME_EVENT_CB_SYSTEM_MESSAGE, &stData, NULL);
		break;
	default:
		break;
	}
	return TRUE;
}

BOOL
AgpmGameEvent::SetCallbackCreateTeam(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_CREATE_TEAM, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackSetTeam(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_SET_TEAM, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_INVITATION, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackAcceptInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_ACCEPT_INVITATION, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackDeclineInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_DECLINE_INVITATION, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackKickMember(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_KICK_MEMBER, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackLeaveOrDisband(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_LEAVE_OR_DISBAND, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackSetStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_SET_STATUS, pfCallback, pClass);
}

BOOL
AgpmGameEvent::SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGPM_GAME_EVENT_CB_SYSTEM_MESSAGE, pfCallback, pClass);
}

PVOID
AgpmGameEvent::MakePacketKickMember(
	const char *	pszMemberName, 
	INT16 *			pnPacketLength)
{
	INT8	cOperation = AGPM_GAME_EVENT_OPERATION_KICK_MEMBER;
	PVOID	pvPacketTeam;
	PVOID	pvPacket;
	char	szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	memset(szName, 0, sizeof(szName));
	strlcpy(szName, pszMemberName, sizeof(szName));
	pvPacketTeam = m_csTeamPacket.MakePacket(
		FALSE, NULL, 0,
		NULL,
		NULL,
		NULL,
		szName);
	if (!pvPacketTeam)
		return NULL;
	pvPacket = m_csPacket.MakePacket(
		TRUE, pnPacketLength, AGPM_GAME_EVENT_PACKET_TYPE,
		&cOperation,
		pvPacketTeam,
		NULL,
		NULL);
	return pvPacket;
}

PVOID
AgpmGameEvent::MakePacketDisbandOrLeave(INT16 * pnPacketLength)
{
	INT8	cOperation = AGPM_GAME_EVENT_OPERATION_LEAVE_OR_DISBAND;
	PVOID	pvPacket;

	pvPacket = m_csPacket.MakePacket(
		TRUE, pnPacketLength, AGPM_GAME_EVENT_PACKET_TYPE,
		&cOperation,
		NULL,
		NULL,
		NULL);
	return pvPacket;
}

PVOID
AgpmGameEvent::MakePacketCreateTeam(
	const char *	pszTeamName, 
	INT16 *			pnPacketLength)
{
	INT8	cOperation = AGPM_GAME_EVENT_OPERATION_CREATE_TEAM;
	PVOID	pvPacketTeam;
	PVOID	pvPacket;
	char	szName[AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN + 1];

	memset(szName, 0, sizeof(szName));
	strlcpy(szName, pszTeamName, sizeof(szName));
	pvPacketTeam = m_csTeamPacket.MakePacket(
		FALSE, NULL, 0,
		szName,
		NULL,
		NULL,
		NULL);
	if (!pvPacketTeam)
		return NULL;
	pvPacket = m_csPacket.MakePacket(
		TRUE, pnPacketLength, AGPM_GAME_EVENT_PACKET_TYPE,
		&cOperation,
		pvPacketTeam,
		NULL,
		NULL);
	return pvPacket;
}

PVOID
AgpmGameEvent::MakePacketInviteMember(
	const char *	pszMemberName, 
	INT16 *			pnPacketLength)
{
	INT8	cOperation = AGPM_GAME_EVENT_OPERATION_INVITATION;
	PVOID	pvPacketTeam;
	PVOID	pvPacket;
	char	szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	memset(szName, 0, sizeof(szName));
	strlcpy(szName, pszMemberName, sizeof(szName));
	pvPacketTeam = m_csTeamPacket.MakePacket(
		FALSE, NULL, 0,
		NULL,
		NULL,
		NULL,
		szName);
	if (!pvPacketTeam)
		return NULL;
	pvPacket = m_csPacket.MakePacket(
		TRUE, pnPacketLength, AGPM_GAME_EVENT_PACKET_TYPE,
		&cOperation,
		pvPacketTeam,
		NULL,
		NULL);
	return pvPacket;
}

PVOID
AgpmGameEvent::MakePacketRespondInvitation(
	const char *	pszTeamName, 
	BOOL			bAccept,
	INT16 *			pnPacketLength)
{
	INT8	cOperation;
	PVOID	pvPacketTeam;
	PVOID	pvPacket;
	char	szName[AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN + 1];

	if (bAccept)
		cOperation = AGPM_GAME_EVENT_OPERATION_ACCEPT_INVITATION;
	else
		cOperation = AGPM_GAME_EVENT_OPERATION_DECLINE_INVITATION;
	memset(szName, 0, sizeof(szName));
	strlcpy(szName, pszTeamName, sizeof(szName));
	pvPacketTeam = m_csTeamPacket.MakePacket(
		FALSE, NULL, 0,
		szName,
		NULL,
		NULL,
		NULL);
	if (!pvPacketTeam)
		return NULL;
	pvPacket = m_csPacket.MakePacket(
		TRUE, pnPacketLength, AGPM_GAME_EVENT_PACKET_TYPE,
		&cOperation,
		pvPacketTeam,
		NULL,
		NULL);
	return pvPacket;
}
