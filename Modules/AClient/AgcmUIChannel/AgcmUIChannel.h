// -----------------------------------------------------------------------------
//                                _    _ _____  _____ _                            _     _     
//     /\                        | |  | |_   _|/ ____| |                          | |   | |    
//    /  \    __ _  ___ _ __ ___ | |  | | | | | |    | |__   __ _ _ __  _ __   ___| |   | |__  
//   / /\ \  / _` |/ __| '_ ` _ \| |  | | | | | |    | '_ \ / _` | '_ \| '_ \ / _ \ |   | '_ \ 
//  / ____ \| (_| | (__| | | | | | |__| |_| |_| |____| | | | (_| | | | | | | |  __/ | _ | | | |
// /_/    \_\\__, |\___|_| |_| |_|\____/|_____|\_____|_| |_|\__,_|_| |_|_| |_|\___|_|(_)|_| |_|
//            __/ |                                                                            
//           |___/                                                                             
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 11/18/2005 by Nonstopdj
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------
#pragma once

#include <AgcModule/AgcModule.h>
#include <AgpmChannel/AgpdChannel.h>

//#define PUBLICCHANNELTYPE	"공개방"
//#define PRIVATECHANNELTYPE	"비밀방"
//#define GUILDCHANNELTYPE	"길드방"

const UINT32 SELFCHATCOLOR		= 0xFFB8B8B8;	//. 2005. 12. 15. 색상바뀜. 0xFFFFCC66
const UINT32 OTHERCHATCOLOR		= 0xFFFFFFFF;
const UINT32 ROOMTYPECOLOR		= 0xFFFFF1C6;

struct AgcdUIUserData;
struct AgpdChannelParam;

//***************************************************************************************************
//
// Class Name 		: RoomWidget
//
// Last Modified 	: Nonstopdj		2005. 11. 22
// First Modified 	: Nonstopdj		2005. 11. 22
//
// Function			: 대화방단위 widget
//
//***************************************************************************************************
class RoomWidget
{
public:
	AgcdUIUserData*	m_pstUDChatList;						//. chat리스트 Userdata
	AgcdUIUserData*	m_pstUDUserList;						//. user리스트 Userdata
	AgpdChannel*	m_pcsChannel;							//. 로컬 channel data
	int				m_iMsgCount;
	ApString<10240>	m_pcsMessage;							//. 대화내용 저장
	INT32			m_IChatList;
	ApSafeArray<INT32, AGPDCHANNEL_MEMBER_GUILD> m_arUser;	//. 인덱스 저장배열

	RoomWidget() : m_pstUDChatList(NULL), m_pstUDUserList(NULL), m_iMsgCount(0)
	{
		// construct
		clear();
	}
	~RoomWidget()
	{
		clear();
	}

	void clear()
	{
		m_pcsChannel = NULL;
		m_pcsMessage.Clear();
		m_iMsgCount = 0;
	}

	void InitUserIndex()
	{
		//. 처음 한번만 인덱스를 설정해준다.
		for (INT32 i = 0; i < AGPDCHANNEL_MEMBER_GUILD; ++i)
		{
			m_arUser[i] = i;
		}
	}

	BOOL AddChatMsg(AgpdChannelParam* pParam, UINT32 Color);
};

class AgcdUI;
class AgcdUIControl;
enum AgcdUIDataType;

//***************************************************************************************************
//
// Class Name 		: AgcmUIChannel : public AgcModule
//
// Last Modified 	: Nonstopdj		2005. 11. 18
// First Modified 	: Nonstopdj		2005. 11. 18
//
// Function			: 대화방관련 모듈
//
//***************************************************************************************************
class AgcmUIChannel : public AgcModule
{

private:
	class AgcmUIChannel*	m_pcsAgcmUIChannel;
	class AgpmChannel*		m_pcsAgpmChannel;
	class AgcmCharacter*	m_pcsAgcmCharacter;
	class AgpmCharacter*	m_pcsAgpmCharacter;
	class AgcmUIMain*		m_pcsAgcmUIMain;
	class AgcmUIManager2*	m_pcsAgcmUIManager2;
	class AgpmChatting*		m_pcsAgpmChatting;
	class AgcmChatting2*	m_pcsAgcmChatting2;
	class AgcmUIChatting2*	m_pcsAgcmUIChatting2;	//. Normal Chatting UI에 문자열추가를 위해.

	AgcdUIUserData*	m_pstDummyUDChatList;	//. chat리스트 Userdata
	AgcdUIUserData*	m_pstDummyUDUserList;	//. user리스트 Userdata
	INT32			m_iDummyChatListIndex;
	INT32			m_iDummyUserListIndex;

	AgcdUIUserData* m_pstWhiperBtnData;		//. Whisper Btn Enable/Disable UserData
	INT32			m_iWhisperBtnStatus;

	EnumChannelType m_eChannelType;			//. 현재 만들려는 방의 옵션상태
	ApStrChannelName m_strChannelName;		//. 현재 입장하려는 방의 제목저장. (초대포함)
	ApStrCharacterName m_strInviteCharName;	//.	현재 초대한 사람의 이름 저장.

	AgcdUIUserData*	m_pstUDPublicRoom;		//. 대화방 만들기의 공개방 옵션 UD
	AgcdUIUserData*	m_pstUDPrivateRoom;		//. 대화방 만들기의 비밀방 옵션 UD
	AgcdUIUserData*	m_pstUDGuildRoom;		//. 대화방 만들기의 길드방 옵션 UD

	BOOL			m_bIsPublicRoom;		//. 공개방 옵션상태
	BOOL			m_bIsPrivateRoom;		//. 비밀방 옵션상태
	BOOL			m_bIsGuildRoom;			//. 길드방 옵션상태
	BOOL			m_bIsCreateRoom;		//. 대화방에 입장하였는지 대화방을 개설했는지의 여부판별용
	static BOOL		m_sbCloneUI;
	static BOOL		m_bisWhisperBtnEnable;

	INT32	m_iEventCreateChannel;			//. Event Index
	INT32	m_iEventJoinChannel;
	INT32	m_iEventPasswordOpen;
	INT32	m_iEventPasswordClose;
	INT32	m_iEventInviteOpenUI;
	INT32	m_iEventInviteCloseUI;
	INT32	m_iEventInviteSendUI;
	INT32	m_iEventFindUserUI;
	INT32	m_iEventWhisperBtnStatus;
	INT32	m_iEventAlreadyExist;			//. Message Code Event
	INT32	m_iEventNotExistChannel;
	INT32	m_iEventNotExistCharacter;
	INT32	m_iEvnetFullChannel;
	INT32	m_iEventNotGuildMaster;
	INT32	m_iEventNotGuildMember;
	INT32	m_iEventAlreadyGuildChannel;
	INT32	m_iEventInvalidPassword;
	INT32	m_iEventAlreadyJoin;

	ApSafeArray<RoomWidget, AGPDCHANNEL_MAX_JOIN> m_arRoomWidget;
	ApSafeArray<AgcdUI*,  AGPDCHANNEL_MAX_JOIN> m_arAgcdUI;

public:
	AgcmUIChannel();
	virtual ~AgcmUIChannel();

	BOOL OnAddModule();
	BOOL OnInit();
	BOOL OnDestroy();
	BOOL OnIdle(UINT32 ulClockCount);

	BOOL AddEvent();
	BOOL AddBoolean();
	BOOL AddFunction();
	BOOL AddUserData();
	BOOL AddDisplay();

	RoomWidget*	GetRoomBaseDataFromChatUD(AgcdUIUserData* pUD)
	{
		for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
		{
			if(m_arRoomWidget[nCount].m_pstUDChatList == pUD)	
				return &m_arRoomWidget[nCount];
		}

		return NULL;
	}

	RoomWidget*	GetRoomBaseDataFromUserUD(AgcdUIUserData* pUD)
	{
		for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
		{
			if(m_arRoomWidget[nCount].m_pstUDUserList == pUD)	
				return &m_arRoomWidget[nCount];
		}

		return NULL;
	}

	//. 시스템메세지
	BOOL AddSystemMessage(CHAR* szMsg);

	//. AgpdChannel 만들기.
	AgpdChannel*	CreateChannelData(AgpdChannelParam *pChannelParam);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. Refresh function
	BOOL RefreshCreateRoomOption();
	BOOL RefreshAllChatRoom();
	BOOL RefreshChatRoom(INT32 nIndex);
	BOOL RefreshChatRoomUserList(INT32 nIndex);
	BOOL RefreshWhisperBtnStatus();

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. send packet
	BOOL SendCreateChannel(TCHAR* szRoomName, TCHAR* szPassword);
	BOOL SendPacketChatMsg(TCHAR *szChannelName, TCHAR *szChatMsg);
	BOOL SendPacketJoinChannel(TCHAR *szChannelName);
	BOOL SendPacketLeaveChannel(TCHAR *szChannelName);
	BOOL SendPacketPassword(TCHAR *szChannelName, TCHAR *szPassword);
	BOOL SendPacketInvitation(TCHAR *szChannelName, EnumChannelType eType, TCHAR *szDestUser);
	BOOL SendPacketFindUser(CHAR *szFindUserName);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. callback functions
	static BOOL CBCloneUI(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyChannel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoin(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLeave(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPassword(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBChatMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBFindUserResult(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBAddUser(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRemoveUser(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBMsgCode(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBCloneUIClear(PVOID pData, PVOID pClass, PVOID pCustData);

	
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. packet functions
	BOOL OperationJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannel* pcsChannel);
	BOOL OperationCloseChannel(AgpdChannel* pChannel);
	//BOOL OperationDestroyChannelAgpdCharacter *pcsCharacter);
	//BOOL OperationMsgCode(AgpdCharacter *pcsCharacter);
	BOOL OperationAddUser(AgpdChannel* pChannel, AgpdChannelParam *pChannelParam);
	BOOL OperationRemoveUser(AgpdChannel* pChannel, AgpdChannelParam *pChannelParam);
	BOOL OperationChatMsg(AgpdChannel* pcsChannel, AgpdChannelParam *pChannelParam, INT32 IsSelf);
	BOOL OperationFindUser(BOOL bResult);
	

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. display
	static BOOL CBDisplayChatList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayUserList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);
	static BOOL CBDisplayChannelTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl);

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//. functions
	//. clear function
	static BOOL CBCreateRoomClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBInviteRoomInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBEditBoxClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBFindUserClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	
	//. normal function
	static BOOL	CBCreateRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCheckPulicOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCheckPrivateOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBCheckGuildOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL CBEnterChatMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInviteSend(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBFindUser(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBSendWhisper(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBJoinRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInviteOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBInviteJoinRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);
	static BOOL	CBLeaveRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	static BOOL	CBQueryPassword(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl);

	//. boolean function
	static BOOL	CBWhiperBtnStatus(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl);


	// script
	void CreateChannel();
	void JoinChannel();
	void FindUser();
};