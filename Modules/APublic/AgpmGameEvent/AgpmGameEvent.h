#ifndef _AGPM_GAME_EVENT_H_
#define _AGPM_GAME_EVENT_H_

#include <ApBase/ApBase.h>
#include <ApModule/ApModule.h>
#include <AuPacket/AuPacket.h>

enum AgpmGameEventOperation {
	AGPM_GAME_EVENT_OPERATION_NONE = -1,
	AGPM_GAME_EVENT_OPERATION_CREATE_TEAM = 0,
	AGPM_GAME_EVENT_OPERATION_SET_TEAM,
	AGPM_GAME_EVENT_OPERATION_INVITATION,
	AGPM_GAME_EVENT_OPERATION_ACCEPT_INVITATION,
	AGPM_GAME_EVENT_OPERATION_DECLINE_INVITATION,
	AGPM_GAME_EVENT_OPERATION_KICK_MEMBER,
	AGPM_GAME_EVENT_OPERATION_LEAVE_OR_DISBAND,
	AGPM_GAME_EVENT_OPERATION_SET_STATUS,
	AGPM_GAME_EVENT_OPERATION_SYSTEM_MESSAGE,
	AGPM_GAME_EVENT_OPERATION_COUNT
};

enum AgpmGameEventCB {
	AGPM_GAME_EVENT_CB_NONE = -1,
	AGPM_GAME_EVENT_CB_CREATE_TEAM = 0,
	AGPM_GAME_EVENT_CB_SET_TEAM,
	AGPM_GAME_EVENT_CB_INVITATION,
	AGPM_GAME_EVENT_CB_ACCEPT_INVITATION,
	AGPM_GAME_EVENT_CB_DECLINE_INVITATION,
	AGPM_GAME_EVENT_CB_KICK_MEMBER,
	AGPM_GAME_EVENT_CB_LEAVE_OR_DISBAND,
	AGPM_GAME_EVENT_CB_SET_STATUS,
	AGPM_GAME_EVENT_CB_SYSTEM_MESSAGE,
	AGPM_GAME_EVENT_CB_MAX
};

class AgpmGameEvent : public ApModule {
	public:
		AuPacket m_csPacket;
		AuPacket m_csTeamPacket;
		AuPacket m_csMemberPacket;

	public:
		AgpmGameEvent();
		virtual ~AgpmGameEvent();

		BOOL OnAddModule();
		BOOL OnReceive(
			UINT32			ulType, 
			PVOID			pvPacket, 
			INT16			nSize, 
			UINT32			ulNID, 
			DispatchArg *	pstCheckArg);

		BOOL SetCallbackCreateTeam(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackSetTeam(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackAcceptInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackDeclineInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackKickMember(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackLeaveOrDisband(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackSetStatus(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL SetCallbackSystemMessage(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		PVOID MakePacketKickMember(const char * pszMemberName, INT16 * pnPacketLength);
		PVOID MakePacketDisbandOrLeave(INT16 * pnPacketLength);
		PVOID MakePacketCreateTeam(const char * pszTeamName, INT16 * pnPacketLength);
		PVOID MakePacketInviteMember(const char * pszMemberName, INT16 * pnPacketLength);
		PVOID MakePacketRespondInvitation(
			const char *	pszTeamName, 
			BOOL			bAccept,
			INT16 *			pnPacketLength);
};

#endif