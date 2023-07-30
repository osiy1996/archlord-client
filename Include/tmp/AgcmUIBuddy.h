#pragma once

#include "AgcModule.h"
#include "AgcmUIMain.h"
#include "AgcmUIManager2.h"
#include "AgpmBuddy.h"
#include "AgpmChatting.h"


#define		MENTOR_BUTTON_MAX_LENGTH			10

class AgcmCharacter;
class AgcmChatting2;
class AgcmParty;
class AgcmUIChatting2;

class AgcmUIBuddy : public AgcModule
{
private:
	AgpmBuddy*														m_pcsAgpmBuddy;
	AgcmCharacter*													m_pcsAgcmCharacter;
	AgpmCharacter*													m_pcsAgpmCharacter;
	AgcmUIMain*														m_pcsAgcmUIMain;
	AgcmUIManager2*													m_pcsAgcmUIManager2;
	AgpmChatting*													m_pcsAgpmChatting;
	AgcmChatting2*													m_pcsAgcmChatting2;
	AgcmUIChatting2*												m_pcsAgcmUIChatting2;
	ApmMap*															m_pcsApmMap;
	AgcmParty*														m_pcsAgcmParty;

	AgcdUIUserData* 												m_pstUDFriendList;
	AgcdUIUserData* 												m_pstUDBanList;
	AgcdUIUserData* 												m_pstUDBuddyDetailInfo;		//. Buddy Detail Info User Data (Dummy)
	AgcdUIUserData* 												m_pstUDBlockWhisper;		//. Rejection List Block Whisper Option 
	AgcdUIUserData* 												m_pstUDBlockTrade;			//. Rejection List Block Trade Option 
	AgcdUIUserData* 												m_pstUDBlockInvitation;		//. Rejection List Block Invitation Option 
	AgcdUIUserData* 												m_pstUDAddRequestBuddy;		//. Buddy request to me
	BOOL															m_bBlockWhisper;			//. Block Option User Data
	BOOL															m_bBlockTrade;				//. Block Option User Data
	BOOL															m_bBlockInvitation;			//. Block Option User Data

	// Mentor
	AgcdUIUserData*													m_pstUDMentorList;



	INT32															m_lBuddyDetailInfo;			//. Buddy Detail Info ID


	INT32															m_lEventMainUIOpen;				// Main UI(Buddy List) Open
	INT32															m_lEventMainUIClose;			// Main UI(Buddy List) Close
	INT32															m_lEventAddBuddyOpen;			// ģ�� �߰� UI Open
	INT32															m_lEventBanUIOpen;				//. �źθ���Ʈ Open
	INT32															m_lEventBanUIClose;				//. �źθ���Ʈ Close
	INT32															m_lEventBuddyBanAddOpen;		//. �ź� ģ�� �߰� UI open
	INT32															m_IEventBuddyDeleteQueryOpen;	//. ģ�� ���� ���� UI Open
	INT32															m_IEventBuddyBanDeleteQueryOpen;//. �ź�ģ�� ���� ���� UI Open
	INT32															m_lEventAddFriendDlgSetFocus;	// ģ�� ��� UI���� ĳ�� �ʱ� ��ġ ����
	INT32															m_lEventAddBanDlgSetFocus;		// �ź� ��� UI���� ĳ�� �ʱ� ��ġ ����

	INT32															m_lEventAddRequestOpen;			// ģ�� ��� ��û UI Open
	INT32															m_lEventAddRejectOpen;			// ģ�� ��� ���� UI Open

	INT32															m_lMsgBoxAlreayExist;		// �̹� ��ϵ� ĳ���͸� ����Ϸ��� ���
	INT32															m_lMsgBoxFullFriendList;	// ģ���� ��� ������ ������ ���� á��.
	INT32															m_lMsgBoxFullBanList;		// �źη� ��� ������ ������ ���� á��.
	INT32															m_lMsgBoxOfflineOrNotExist;	// ���������̰ų� �������� ����
	INT32															m_lMsgBoxInvalidUser;		// ����� �� ���� ĳ����
	INT32															m_lMsgBoxBanUser;			// ���ܵ� ���
	INT32															m_IMsgBoxInvalidUserIDLength;	//. ����Ϸ��� ���̵� �⺻ ID�� ���̺��� Ŭ ��� �ߴ� dlg

	INT32															m_ICurrentBuddySelectIndex; //. ���� ���õ� Buddy�� Index
	INT32															m_ICurrentBanSelectIndex;	//. ���� ���õ� �ź�Buddy�� Index
	INT32															m_lCurrentMentorSelectIndex; // Current Mentor Index

	BOOL															m_bInitEnd;					// �ʱ�ȭ �����͸� ��δ� �����ߴ��� ���θ� Ȯ�� TRUE�� ���ۿϷ�

	CHAR															m_szIDBuffer[AGPDCHARACTER_NAME_LENGTH + 1];
	CHAR															m_szRegionBuffer[128 + 1];

	INT32															m_lEventMentorUIOpen;		// Mentor UI Open
	INT32															m_lEventMentorUIClose;		// Mentor UI Close

	AgcdUIUserData*													m_pstMentorBtnID;
	CHAR															m_szMentorBtnID	[ MENTOR_BUTTON_MAX_LENGTH ];

	AgcdUIUserData*													m_pstEnableInviteBtn;
	BOOL															m_bEnableInviteButton;
    
	ApSafeArray< INT32, AGPD_BUDDY_MAX >							m_arMentor;
	ApSafeArray< INT32, AGPD_BUDDY_MAX > 							m_arFriend;
	ApSafeArray< INT32, AGPD_BUDDY_MAX > 							m_arBan;

public:
	AgcmUIBuddy( void );
	virtual ~AgcmUIBuddy( void );

	BOOL 						OnAddModule							( void );
	BOOL 						OnInit								( void );
	BOOL 						OnDestroy							( void );
	BOOL 						OnIdle								( UINT32 ulClockCount );

	BOOL 						AddEvent							( void );
	BOOL 						AddFunction							( void );
	BOOL 						AddUserData							( void );
	BOOL 						AddDisplay							( void );

	BOOL						AddSystemMessage					( CHAR* szMsg );

	//. Buddylist, Banlist�� ����������� ����ϴ� ������ �ʱ�ȭ�Ѵ�.
	static BOOL 				CBBuddyListMemberClear				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBanListMemberClear				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBuddyUIKeydownESC					( PVOID pData, PVOID pClass, PVOID pCustData );


	///////////////////////////////////////////////////////////////////////////////////////////
	// refresh 
	BOOL 						BuddyListUpdate						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						RefreshFriendList					( void );
	BOOL 						RefreshBanList						( void );
	BOOL						RefreshMentorList					( void );

	//. 2005. 11. 16. Nonstopdj
	//. ģ������Ʈ�� Ŭ������ ��� ���õ� ���̵��� �������������� ����
	BOOL						RefreshBuddyDetailInfo				( void );

	//. 2005. 11. 17. Nonstopdj
	//. BanListUI�� check box �ɼ� Update & Refresh
	BOOL						BlockOptionUpdate					( AgpdBuddyElement* pcsBuddyElement );
	BOOL						RefreshBlockOptions					( void );

	///////////////////////////////////////////////////////////////////////////////////////////
	// option callback
	static BOOL 				CBCheckBlockWhisper					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCheckBlockTrade					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBCheckBlockInvitation				( PVOID pData, PVOID pClass, PVOID pCustData );

	///////////////////////////////////////////////////////////////////////////////////////////
	// operation callback
	static BOOL 				CBAddBuddy							( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBAddBuddyRequest					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBAddBuddyReject					( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBRemoveBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBOptionsBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBOnlineBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBMsgCodeBuddy						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBRegionInfo						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL 				CBInitEnd							( PVOID pData, PVOID pClass, PVOID pCustData );

	static BOOL					CBMentorUIOpen						( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorRequestConfirm				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorDelete				( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL					CBMentorRequestAccept				( PVOID pData, PVOID pClass, PVOID pCustData );

	///////////////////////////////////////////////////////////////////////////////////////////
	// packet operation
	BOOL 						OperationAddBuddy					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationAddRequest					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationAddReject					( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationRemoveBuddy				( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationOptionsBuddy				( AgpdCharacter *pcsCharacter, AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationOnlineBuddy				( AgpdBuddyElement *pcsBuddyElement );
	BOOL 						OperationMsgCodeBuddy				( EnumAgpmBuddyMsgCode eMsgCode );
	BOOL 						OperationRegionInfo					( AgpdCharacter *pcsCharacter, AgpdBuddyRegion *pcsBuddyRegion );

	///////////////////////////////////////////////////////////////////////////////////////////
	// send packet
	BOOL 						SendAddBuddy						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendAddRequest						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendAddReject						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendRemoveBuddy						( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendRegion							( AgpdBuddyElement* pcsBuddyElement );
	BOOL 						SendOption							( AgpdBuddyElement* pcsBuddyElement );

	BOOL						SendMentorRequestAccept				( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorRequestReject				( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorDelete					( AgpdBuddyElement* pcsBuddyElement );
	BOOL						SendMentorRequest					( AgpdBuddyElement* pcsBuddyElement );

	///////////////////////////////////////////////////////////////////////////////////////////
	// event
	//. 2005. 11. 15. Nonstopdj
	//. ģ��/�ź� ����Ʈ���� �ش� ĳ��������� Event
	static BOOL 				CBReturnBuddyOkCancelDelete			( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL 				CBReturnBanOkCancelDelete			( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );
	static BOOL 				CBReturnOkCancelBuddyAddRequest		( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage );

	///////////////////////////////////////////////////////////////////////////////////////////
	// display : buddylist
	static BOOL					CBDisplayFriendList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 15. Nonstopdj
	//. List�� ��ϵ� Buddy�� ���ӻ���(on/off)�� display�ϴ� callback
	static BOOL					CBDisplayFriendStatus				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. List�� ��ϵ� Buddy�� �������� ��� ǥ�õǾ�� �ϴ� �������� Display�ϴ� callback
	static BOOL					CBDisplayFriendDetailInfo			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	//. item�� �������� ���� Display callback.
	static BOOL					CBDisplayFriendSelect				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayAddRequestBuddy			( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayMentorBtnText				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR* szDIsplay, INT32* plValue, AgcdUIControl* pcsSourceControl );

	static BOOL					CBDisplayMentorSelect				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	static BOOL					CBDisplayMentorRelation				( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );

	///////////////////////////////////////////////////////////////////////////////////////////
	// display : banlist
	static BOOL 				CBDisplayBanList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBDisplayBanSelect					( PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl );


	///////////////////////////////////////////////////////////////////////////////////////////
	// function : buddylist UI button control
	static BOOL 				CBBtnAddDlgOpenClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnAddBuddyClick					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 15. Nonstopdj
	//. ģ������ Button Click Function
	static BOOL					CBBtnDeleteBuddyClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. ��Ƽ�ʴ� Button Click Function 
	static BOOL					CBBtnInviteBuddyClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. �Ӹ� Button Click Function 
	static BOOL					CBBtnWhisperMsgBuddyClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. 2005. 11. 16. Nonstopdj
	//. ��ϵ� Buddy Click Function
	static BOOL					CBBuddySelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	// Buddy ����Ʈ ���â�� �������� ĳ�� ��ġ �ʱ�ȭ
	static BOOL					CBBuddyAddDlgSetFocus				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );

	static BOOL					CBMentorSelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	
	///////////////////////////////////////////////////////////////////////////////////////////
	// function : banlist UI button control
	//. ��ȭ/�ŷ�/�ʴ� ����
	static BOOL 				CBBtnBlockWhisperClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnBlockTradeClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnBlockInvitationClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. �źθ���Ʈ�� �߰�
	static BOOL 				CBBtnAddBanDlgOpenClick				( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBBtnAddRejectionListClick			( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. �źθ���Ʈ���� ����
	static BOOL					CBBtnDeleteRejectionListClick		( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	//. ��ϵ� Ban Click Function
	static BOOL					CBBanListSelect						( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	// Ban ����Ʈ ���â�� �������� ĳ�� ��ġ �ʱ�ȭ
	static BOOL					CBBanAddDlgSetFocus					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	///////////////////////////////////////////////////////////////////////////////////////////

	// Boolean : ģ��/�ź� ����Ʈ�� ��ϵǾ��ִ���
	static BOOL 				CBNotInBuddyList					( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBNotInBanList						( PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl );

	// Mentor
	static BOOL					CBMentorAddAndDel					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );
	static BOOL 				CBMentorListRefresh					( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl );


	VOID						OperationMentorUIOpen						( VOID );
	VOID						OperationMentorUIClose						( VOID );
	VOID						OperationMentorRequestConfirm				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );
	VOID						OperationMentorDelete				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );
	VOID						OperationMentorRequestAccept				( AgpdCharacter* pcsCharacter , AgpdBuddyElement* pcsBuddyElement );

	// Script
	void 						AddFriend							( void );
	void 						RemoveFriend						( void );
	void 						AddBan								( void );
	void 						RemoveBan							( void );
	void 						OpenAddDialog						( void );
	void 						OpenFriendList						( void );
	void 						OpenBanList							( void );
};
