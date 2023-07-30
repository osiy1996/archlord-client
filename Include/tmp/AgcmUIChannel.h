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

#include "AgcModule.h"
#include "AgcmUIMain.h"
#include "AgcmUIManager2.h"
#include "AgpmChannel.h"
#include "AgpmChatting.h"
#include "AgcmChatting2.h"
#include "AgcmUIChatting2.h"

class AgpmCharacter;
class AgcmCharacter;

//#define PUBLICCHANNELTYPE	"������"
//#define PRIVATECHANNELTYPE	"��й�"
//#define GUILDCHANNELTYPE	"����"

const UINT32 SELFCHATCOLOR		= 0xFFB8B8B8;	//. 2005. 12. 15. ����ٲ�. 0xFFFFCC66
const UINT32 OTHERCHATCOLOR		= 0xFFFFFFFF;
const UINT32 ROOMTYPECOLOR		= 0xFFFFF1C6;

//***************************************************************************************************
//
// Class Name 		: RoomWidget
//
// Last Modified 	: Nonstopdj		2005. 11. 22
// First Modified 	: Nonstopdj		2005. 11. 22
//
// Function			: ��ȭ����� widget
//
//***************************************************************************************************
class RoomWidget
{
public:
	AgcdUIUserData*	m_pstUDChatList;						//. chat����Ʈ Userdata
	AgcdUIUserData*	m_pstUDUserList;						//. user����Ʈ Userdata
	AgpdChannel*	m_pcsChannel;							//. ���� channel data
	int				m_iMsgCount;
	ApString<10240>	m_pcsMessage;							//. ��ȭ���� ����
	INT32			m_IChatList;
	ApSafeArray<INT32, AGPDCHANNEL_MEMBER_GUILD> m_arUser;	//. �ε��� ����迭

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
		//. ó�� �ѹ��� �ε����� �������ش�.
		for (INT32 i = 0; i < AGPDCHANNEL_MEMBER_GUILD; ++i)
		{
			m_arUser[i] = i;
		}
	}

	BOOL AddChatMsg(AgpdChannelParam* pParam, UINT32 Color)
	{
		//. ä�� �޼��� �����
		//. to do: ���⼭ ������� �����Ͽ� �ִ´�.
		TCHAR pString[180];

		if(m_pcsMessage.GetLength() > 0)
			_stprintf(pString, "\\n<C-26368>%s: <C%d>%s", pParam->m_strUserName.GetBuffer(), Color, pParam->m_strChatMsg.GetBuffer());
		else
			_stprintf(pString, "<C-26368>%s: <C%d>%s", pParam->m_strUserName.GetBuffer(), Color, pParam->m_strChatMsg.GetBuffer());

		INT32 lLength = (INT32)_tcslen(pString);

		if(m_pcsMessage.GetLength() + lLength >= m_pcsMessage.GetBufferLength())
		{	
			TCHAR* pSrcPos = &m_pcsMessage[lLength];
			TCHAR* pDestPos	= &m_pcsMessage[0];

			memmove(pDestPos, pSrcPos, m_pcsMessage.GetBufferLength() - lLength);

			//. 2005. 11. 28. Nonstopdj
			//. Append�� ��� ����� ���̴� m_ILength�� �ܺο��� ���������� �� �� �����Ƿ�
			//. ���� ũ�⸸ŭ ���� copy�� �� return�ع�����.
			_tcsncpy(&m_pcsMessage[m_pcsMessage.GetLength() - lLength], pString, lLength);

			return TRUE;
		}
		
		//. ���ڿ� �ֱ�
		m_pcsMessage.Append(pString);
		
		return TRUE;
	}
};

//***************************************************************************************************
//
// Class Name 		: AgcmUIChannel : public AgcModule
//
// Last Modified 	: Nonstopdj		2005. 11. 18
// First Modified 	: Nonstopdj		2005. 11. 18
//
// Function			: ��ȭ����� ���
//
//***************************************************************************************************
class AgcmUIChannel : public AgcModule
{

private:
	AgcmUIChannel*	m_pcsAgcmUIChannel;
	AgpmChannel*	m_pcsAgpmChannel;
	AgcmCharacter*	m_pcsAgcmCharacter;
	AgpmCharacter*	m_pcsAgpmCharacter;
	AgcmUIMain*		m_pcsAgcmUIMain;
	AgcmUIManager2*	m_pcsAgcmUIManager2;
	AgpmChatting*	m_pcsAgpmChatting;
	AgcmChatting2*	m_pcsAgcmChatting2;
	AgcmUIChatting2* m_pcsAgcmUIChatting2;	//. Normal Chatting UI�� ���ڿ��߰��� ����.

	AgcdUIUserData*	m_pstDummyUDChatList;	//. chat����Ʈ Userdata
	AgcdUIUserData*	m_pstDummyUDUserList;	//. user����Ʈ Userdata
	INT32			m_iDummyChatListIndex;
	INT32			m_iDummyUserListIndex;

	AgcdUIUserData* m_pstWhiperBtnData;		//. Whisper Btn Enable/Disable UserData
	INT32			m_iWhisperBtnStatus;

	EnumChannelType m_eChannelType;			//. ���� ������� ���� �ɼǻ���
	ApStrChannelName m_strChannelName;		//. ���� �����Ϸ��� ���� ��������. (�ʴ�����)
	ApStrCharacterName m_strInviteCharName;	//.	���� �ʴ��� ����� �̸� ����.

	AgcdUIUserData*	m_pstUDPublicRoom;		//. ��ȭ�� ������� ������ �ɼ� UD
	AgcdUIUserData*	m_pstUDPrivateRoom;		//. ��ȭ�� ������� ��й� �ɼ� UD
	AgcdUIUserData*	m_pstUDGuildRoom;		//. ��ȭ�� ������� ���� �ɼ� UD

	BOOL			m_bIsPublicRoom;		//. ������ �ɼǻ���
	BOOL			m_bIsPrivateRoom;		//. ��й� �ɼǻ���
	BOOL			m_bIsGuildRoom;			//. ���� �ɼǻ���
	BOOL			m_bIsCreateRoom;		//. ��ȭ�濡 �����Ͽ����� ��ȭ���� �����ߴ����� �����Ǻ���
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

	//. �ý��۸޼���
	BOOL AddSystemMessage(CHAR* szMsg);

	//. AgpdChannel �����.
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