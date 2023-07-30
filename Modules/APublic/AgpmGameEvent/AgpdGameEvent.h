#ifndef _AGPD_GAME_EVENT_H_
#define _AGPD_GAME_EVENT_H_

#include <ApBase/ApBase.h>
#include <ApModule/ApModule.h>
#include <AgpmCharacter/AgpdCharacter.h>

#define AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN 32
#define AGPM_GAME_EVENT_MAX_TEAM_STATUS_LEN 512
#define AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT 5

enum AgpmGameEventSystemMessage {
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_NONE = -1,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_REGISTER_CLOSED = 0,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_LEVEL,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_NAME_EXISTS,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_INVALID_TARGET,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_ONLY_LEADER_CAN_INVITE,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_REQ_UNIQUE_CLASS,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_JOINED,
	AGPM_GAME_EVENT_SYSTEM_MESSAGE_MEMBER_LEFT,
};

struct AgpdGameEventTeamMember {
	char	m_szName[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	UINT	m_ulTemplateId;
	UINT	m_ulLevel;
};

struct AgpdGameEventTeam {
	char					m_szName[AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN];
	char					m_szStatus[AGPM_GAME_EVENT_MAX_TEAM_STATUS_LEN];
	AgpdGameEventTeamMember	m_stMember[AGPM_GAME_EVENT_MAX_TEAM_MEMBER_COUNT];
	UINT					m_ulMemberCount;
};

struct AgpdGameEventCallbackData {
	AgpdGameEventTeam *			m_pstTeam;
	AgpmGameEventSystemMessage	m_eSystemMessage;
	const char *				m_pszSystemMessageArg;
};

#endif
