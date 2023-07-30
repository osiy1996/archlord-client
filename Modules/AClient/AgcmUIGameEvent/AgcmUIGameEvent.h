#ifndef _AGCM_UI_GAME_EVENT_H_
#define _AGCM_UI_GAME_EVENT_H_

#include <AgcModule/AgcModule.h>
#include <AgpmGameEvent/AgpdGameEvent.h>

struct AgcdUIUserData;
class AgcdUIControl;
enum AgcdUIDataType;
enum AgpmGameEventSystemMessage;
class AgpdCharacter;

class AgcmUIGameEvent : public ApModule {
	private:
		class AgpmGameEvent *		m_pAgpmGameEvent;
		class AgpmCharacter *		m_pAgpmCharacter;
		class AgcmCharacter *		m_pAgcmCharacter;
		class AgcmUIManager2 *		m_pAgcmUIManager2;
		class AgcmUIControl *		m_pAgcmUIControl;
		class AgcmUIMain *			m_pAgcmUIMain;
		class AgcmChatting2 *		m_pAgcmChatting2;
		class AgcmUICharacter *		m_pAgcmUICharacter;

		AgpdGameEventTeam			m_stTeam;
		AgcdUIUserData *			m_pstUDTeam;
		AgcdUIUserData *			m_pstUDList;
		INT32						m_lEventOnInvite;
		char						m_szTeamInviteFrom[AGPM_GAME_EVENT_MAX_TEAM_NAME_LEN];

	public:
		AgcmUIGameEvent();
		virtual ~AgcmUIGameEvent();

		BOOL OnAddModule();
		BOOL OnInit();
		
	protected:
		BOOL AddFunction();
		BOOL AddDisplay();
		BOOL AddUserData();
		
		const char * GetMessageTxt(AgpmGameEventSystemMessage eMessage);
		void AddSystemMessage(CHAR * pszMessage);

private:
		static BOOL CBSetTeam(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBLeaveOrDisband(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSetStatus(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBSystemMessage(PVOID pData, PVOID pClass, PVOID pCustData);

		static BOOL CBDisplayMemberClass(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayMemberName(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayMemberLevel(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayMemberKick(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayTeamName(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayTeamStatus(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayCreateOrInvite(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayDisbandOrLeave(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayDialogTitle(
			PVOID			pClass,
			PVOID			pData,
			AgcdUIDataType	eType,
			INT32			lID,
			CHAR *			szDisplay,
			INT32 *			plValue,
			AgcdUIControl *	pcsSourceControl);
		static BOOL CBDisplayTeamInviteFrom(
			PVOID			pClass, 
			PVOID			pData, 
			AgcdUIDataType	eType, 
			INT32			lID, 
			CHAR *			szDisplay, 
			INT32 *			plValue);

		static BOOL	CBOnKickMember(
			PVOID			pClass,
			PVOID			pData1,
			PVOID			pData2,
			PVOID			pData3,
			PVOID			pData4,
			PVOID			pData5,
			ApBase *		pcsTarget,
			AgcdUIControl *	pcsSourceControl);
		static BOOL	CBOnDisbandOrLeave(
			PVOID			pClass,
			PVOID			pData1,
			PVOID			pData2,
			PVOID			pData3,
			PVOID			pData4,
			PVOID			pData5,
			ApBase *		pcsTarget,
			AgcdUIControl *	pcsSourceControl);
		static BOOL	CBOnExecDialog(
			PVOID			pClass,
			PVOID			pData1,
			PVOID			pData2,
			PVOID			pData3,
			PVOID			pData4,
			PVOID			pData5,
			ApBase *		pcsTarget,
			AgcdUIControl *	pcsSourceControl);

		static BOOL CBIsCreateOrInviteEnabled(
			PVOID			pClass, 
			PVOID			pData, 
			AgcdUIDataType	eType, 
			AgcdUIControl *	pcsSourceControl);

		static BOOL CBEventReturnInvitation(
			PVOID	pClass, 
			INT32	lConfirmed, 
			CHAR *	pszInputMessage);

		void RefreshUD();
};

#endif