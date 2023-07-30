// -----------------------------------------------------------------------------
//                                _    _ _____  _____ _                            _                      
//     /\                        | |  | |_   _|/ ____| |                          | |                     
//    /  \    __ _  ___ _ __ ___ | |  | | | | | |    | |__   __ _ _ __  _ __   ___| |     ___ _ __  _ __  
//   / /\ \  / _` |/ __| '_ ` _ \| |  | | | | | |    | '_ \ / _` | '_ \| '_ \ / _ \ |    / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| | | | | | |__| |_| |_| |____| | | | (_| | | | | | | |  __/ | _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|_| |_| |_|\____/|_____|\_____|_| |_|\__,_|_| |_|_| |_|\___|_|(_) \___| .__/| .__/ 
//            __/ |                                                                          | |   | |    
//           |___/                                                                           |_|   |_|    
//
// 
//
// -----------------------------------------------------------------------------
// Originally created on 11/18/2005 by Nonstopdj
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include <AgcmUIChannel/AgcmUIChannel.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <AuStrTable/AuStrTable.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmChannel/AgpmChannel.h>
#include <AgcModule/AgcEngine.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmUIControl/AcUIScroll.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgpmChatting/AgpmChatting.h>

//. UI Common Message Define.
#define UI_MESSAGE_CHANNEL_NEED_PASSWORD				"Channel_Need_Password"
#define UI_MESSAGE_CHANNEL_JOIN_MESSAGE					"Channel_Join_Message"
#define UI_MESSAGE_CHANNEL_LEAVE_MESSAGE				"Channel_Leave_Message"
#define UI_MESSAGE_CHANNEL_CREATE_MESSAGE				"Channel_Create_Message"


// -----------------------------------------------------------------------------
BOOL AgcmUIChannel::m_sbCloneUI = TRUE;
BOOL AgcmUIChannel::m_bisWhisperBtnEnable = FALSE;

BOOL RoomWidget::AddChatMsg(AgpdChannelParam * pParam, UINT32 Color)
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


AgcmUIChannel::AgcmUIChannel()
{
	m_pcsAgcmUIChannel	= NULL;
	m_pcsAgpmChannel	= NULL;
	m_pcsAgcmCharacter	= NULL;
	m_pcsAgpmCharacter	= NULL;
	m_pcsAgcmUIMain		= NULL;
	m_pcsAgcmUIManager2 = NULL;
	m_pcsAgpmChatting	= NULL;
	m_pcsAgcmChatting2	= NULL;
	m_pcsAgcmUIChatting2 = NULL;

	m_pstDummyUDChatList= NULL;
	m_pstDummyUDUserList= NULL;

	m_pstUDPublicRoom	= NULL;
	m_pstUDPrivateRoom	= NULL;
	m_pstUDGuildRoom	= NULL;		//. ��ȭ�� ������� ���� �ɼ� UD

	m_bIsPublicRoom		= TRUE;		//. ������ �ɼǻ���
	m_bIsPrivateRoom	= FALSE;	//. ��й� �ɼǻ���
	m_bIsGuildRoom		= FALSE;	//. ���� �ɼǻ���
	m_bIsCreateRoom		= FALSE;

	m_iEventCreateChannel	= 0;
	m_iEventJoinChannel		= 0;
	m_iEventPasswordOpen	= 0;
	m_iEventPasswordClose	= 0;
	m_iEventInviteOpenUI	= 0;
	m_iEventInviteCloseUI	= 0;
	m_iEventInviteSendUI	= 0;
	m_iEventFindUserUI		= 0;
	m_iEventWhisperBtnStatus = 0;

	m_iEventAlreadyExist	= 0;			//. Message Code Event
	m_iEventNotExistChannel	= 0;
	m_iEventNotExistCharacter = 0;
	m_iEvnetFullChannel		= 0;
	m_iEventNotGuildMaster	= 0;
	m_iEventNotGuildMember	= 0;
	m_iEventAlreadyGuildChannel = 0;
	m_iEventInvalidPassword	= 0;
	m_iEventAlreadyJoin	= 0;

	m_eChannelType		= AGPDCHANNEL_TYPE_PUBLIC;

	for (INT32 i = 0; i < AGPDCHANNEL_MAX_JOIN; ++i)
	{
		m_arRoomWidget[i].InitUserIndex();
		m_arAgcdUI[i] = NULL;
	}

	// module initialize
	SetModuleName("AgcmUIChannel");
	EnableIdle2(FALSE);
}

// -----------------------------------------------------------------------------

AgcmUIChannel::~AgcmUIChannel()
{
	for(INT32 i = 0; i < AGPDCHANNEL_MAX_JOIN; ++i)
	{
		m_pcsAgcmUIManager2->RemoveUI(m_arAgcdUI[i]);
		m_arAgcdUI[i] = NULL;
	}
}

//.
BOOL AgcmUIChannel::OnAddModule()
{
	m_pcsAgcmUIChannel	= (AgcmUIChannel*)GetModule("AgcmUIChannel");
	m_pcsAgpmChannel	= (AgpmChannel*)GetModule("AgpmChannel");
	m_pcsAgcmCharacter	= (AgcmCharacter*)GetModule("AgcmCharacter");
	m_pcsAgpmCharacter	= (AgpmCharacter*)GetModule("AgpmCharacter");
	m_pcsAgcmUIMain		= (AgcmUIMain*)GetModule("AgcmUIMain");
	m_pcsAgcmUIManager2 = (AgcmUIManager2*)GetModule("AgcmUIManager2");
	m_pcsAgpmChatting	= (AgpmChatting*)GetModule("AgpmChatting");
	m_pcsAgcmChatting2	= (AgcmChatting2*)GetModule("AgcmChatting2");
	m_pcsAgcmUIChatting2 = (AgcmUIChatting2*)GetModule("AgcmUIChatting2");

	if (!m_pcsAgpmChannel || !m_pcsAgcmCharacter || !m_pcsAgcmUIManager2 || !m_pcsAgpmChatting)
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackJoin(CBJoin, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackChatMsg(CBChatMsg, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackLeave(CBLeave, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackAddUser(CBAddUser, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackRemoveUser(CBRemoveUser, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackPassword(CBPassword, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackFindUser(CBFindUserResult, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackInvitation(CBInvitation, this))
		return FALSE;

	if (!m_pcsAgpmChannel->SetCallbackMsgCode(CBMsgCode, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBCloneUI, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(CBCloneUIClear, this))
		return FALSE;

	if (!AddEvent())
		return FALSE;

	if(!AddBoolean())
		return FALSE;

	if (!AddFunction())
		return FALSE;

	if (!AddUserData())
		return FALSE;
	
	if (!AddDisplay())
		return FALSE;


	return TRUE;
}

//.
BOOL AgcmUIChannel::OnInit()
{
	AS_REGISTER_TYPE_BEGIN(AgcmUIChannel, AgcmUIChannel);
		AS_REGISTER_METHOD0(void, CreateChannel);
		AS_REGISTER_METHOD0(void, JoinChannel);
		AS_REGISTER_METHOD0(void, FindUser);
	AS_REGISTER_TYPE_END;

	return TRUE;
}

//.
BOOL AgcmUIChannel::OnDestroy()
{
	return TRUE;
}

//.
BOOL AgcmUIChannel::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

//.
BOOL AgcmUIChannel::AddEvent()
{
	AGCMUI_CHECK_RETURN(m_iEventCreateChannel, m_pcsAgcmUIManager2->AddEvent("CreateChannel_Open"));
	AGCMUI_CHECK_RETURN(m_iEventJoinChannel, m_pcsAgcmUIManager2->AddEvent("JoinChannel_Close"));
	AGCMUI_CHECK_RETURN(m_iEventPasswordOpen, m_pcsAgcmUIManager2->AddEvent("QueryPassword_Open"));
	AGCMUI_CHECK_RETURN(m_iEventPasswordClose, m_pcsAgcmUIManager2->AddEvent("QueryPassword_Close"));
	AGCMUI_CHECK_RETURN(m_iEventInviteOpenUI, m_pcsAgcmUIManager2->AddEvent("InviteChannel_Open"));
	AGCMUI_CHECK_RETURN(m_iEventInviteCloseUI, m_pcsAgcmUIManager2->AddEvent("InviteChannel_Close"));
	AGCMUI_CHECK_RETURN(m_iEventInviteSendUI, m_pcsAgcmUIManager2->AddEvent("InviteSend_Open"));
	AGCMUI_CHECK_RETURN(m_iEventFindUserUI, m_pcsAgcmUIManager2->AddEvent("FindUser_Open"));
	AGCMUI_CHECK_RETURN(m_iEventWhisperBtnStatus, m_pcsAgcmUIManager2->AddEvent("WhisperBtn_Status"));

	AGCMUI_CHECK_RETURN(m_iEventAlreadyExist, m_pcsAgcmUIManager2->AddEvent("ChannelAlreadyExist_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventNotExistChannel, m_pcsAgcmUIManager2->AddEvent("ChannelNotExist_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventNotExistCharacter, m_pcsAgcmUIManager2->AddEvent("NotExistCharacter_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEvnetFullChannel, m_pcsAgcmUIManager2->AddEvent("ChannelCountFull_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventNotGuildMaster, m_pcsAgcmUIManager2->AddEvent("NotGuildMaster_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventNotGuildMember, m_pcsAgcmUIManager2->AddEvent("NOtGuildMember_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventAlreadyGuildChannel, m_pcsAgcmUIManager2->AddEvent("GuildChannelAlreadyExist_MsgUI"));
	AGCMUI_CHECK_RETURN(m_iEventInvalidPassword, m_pcsAgcmUIManager2->AddEvent("InvalidPassword_MsgUI"));

	return TRUE;
}

BOOL AgcmUIChannel::AddBoolean()
{
	if (!m_pcsAgcmUIManager2->AddBoolean(this, "ChangeWhisperBtn_onoff", CBWhiperBtnStatus, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

//.
BOOL AgcmUIChannel::AddFunction()
{
	if (!m_pcsAgcmUIManager2->AddFunction(this, "CreateChannelOption_Clear", CBCreateRoomClear, 0))
		return FALSE;

	//. 2005. 12. 1. Nonstopdj
	//. EditBox�� �ϳ� ���� UI���� ����ϴ� �ʱ�ȭ function
	if (!m_pcsAgcmUIManager2->AddFunction(this, "UIEditBox_Init", CBEditBoxClear, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "FindUser_Init", CBFindUserClear, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InviteChannelReceive_Init", CBInviteRoomInit, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InvitationUI_Open", CBInviteOpen, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InviteChannelSend", CBInviteSend, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "InviteJoinChannel_Click", CBInviteJoinRoom, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "FindUser_Click", CBFindUser, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "SendWhisper_Click", CBSendWhisper, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CreateChannel", CBCreateRoom, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CreateChannelOption_PuplicCLick", CBCheckPulicOptionClick, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CreateChannelOption_PrivateCLick", CBCheckPrivateOptionClick, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "CreateChannelOption_GuildCLick", CBCheckGuildOptionClick, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ChannelSend_ChatMessage", CBEnterChatMessage, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "Channeljoin_click", CBJoinRoom, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "ChannelLeave_click", CBLeaveRoom, 0))
		return FALSE;

	if (!m_pcsAgcmUIManager2->AddFunction(this, "PasswordOK_click", CBQueryPassword, 0))
		return FALSE;

	return TRUE;
}

//.
BOOL AgcmUIChannel::AddUserData()
{
	//. ��ȭ�� dummy UD
	if (!(m_pstDummyUDChatList = m_pcsAgcmUIManager2->AddUserData("DummyChatList_UD", &m_iDummyChatListIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;
	if (!(m_pstDummyUDUserList = m_pcsAgcmUIManager2->AddUserData("DummyUserList_UD", &m_iDummyUserListIndex, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return FALSE;

	if(!(m_pstWhiperBtnData = m_pcsAgcmUIManager2->AddUserData("WhisperBtnStatus_UD", &m_iWhisperBtnStatus, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32)))
		return TRUE;

	//. �ִ� 5���� ��ȭ�濡 ���� RoomWidget(UD 2��)�����ϱ�
	TCHAR strUDName[128];
	for(int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{	
		wsprintf(strUDName, "ChatList%d_UD", nCount);
		if (!(m_arRoomWidget[nCount].m_pstUDChatList = m_pcsAgcmUIManager2->AddUserData(strUDName, 
																			&m_arRoomWidget[nCount].m_IChatList, 
																			sizeof(INT32), 
																			0, //AGPDCHANNEL_MAX_CHAT_LINE
																			AGCDUI_USERDATA_TYPE_INT32)))
			return FALSE;

		wsprintf(strUDName, "UserList%d_UD", nCount);
		if (!(m_arRoomWidget[nCount].m_pstUDUserList = m_pcsAgcmUIManager2->AddUserData(strUDName, 
																			&m_arRoomWidget[nCount].m_arUser[0], 
																			sizeof(INT32), 
																			AGPDCHANNEL_MEMBER_GUILD, //. MAX User���� Guildä�� �ο�.
																			AGCDUI_USERDATA_TYPE_INT32)))
			return FALSE;
	}


	//. ��ȭ�� ����� �������ɼ� UD
	if (!(m_pstUDPublicRoom = m_pcsAgcmUIManager2->AddUserData("CreateRoomPublicOption_UD", &m_bIsPublicRoom, sizeof(INT), 1, AGCDUI_USERDATA_TYPE_BOOL)))
		return FALSE;
	if (!(m_pstUDPrivateRoom = m_pcsAgcmUIManager2->AddUserData("CreateRoomPrivateOption_UD", &m_bIsPrivateRoom, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL)))
		return FALSE;
	if (!(m_pstUDGuildRoom = m_pcsAgcmUIManager2->AddUserData("CreateRoomGuildOption_UD", &m_bIsGuildRoom, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL)))
		return FALSE;

	return TRUE;
}

//.
BOOL AgcmUIChannel::AddDisplay()
{
	//. 
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ChatList_Display", 0, CBDisplayChatList, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "UserList_Display", 0, CBDisplayUserList, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if (!m_pcsAgcmUIManager2->AddDisplay(this, "ChannelTitle_Display", 0, CBDisplayChannelTitle, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

//.
AgpdChannel*	AgcmUIChannel::CreateChannelData(AgpdChannelParam *pChannelParam)
{
	if(!pChannelParam)
		return NULL;

	AgpdChannel* pcsChannel = m_pcsAgpmChannel->CreateChannelData();

	pcsChannel->Initialize(pChannelParam->m_eType);
	pcsChannel->m_strName.SetText(pChannelParam->m_strName);
	pcsChannel->m_strPassword.SetText(pChannelParam->m_strPassword);

	//. UserList�����ϱ�
	TCHAR* pszList = (TCHAR*)pChannelParam->m_pvUserList;
	INT32 ILength = (INT32)_tcslen(pszList);
	INT32 IStartPos = 0;

	for(INT32 nIndex = 0; nIndex < ILength; nIndex++ )
	{
		if(!_tcsncmp( ";", &pszList[nIndex], 1))
		//if(pszList[nIndex] == ';');.
		{
			TCHAR pszBuffer[AGPDCHARACTER_NAME_LENGTH];
			memset(pszBuffer, 0, sizeof(TCHAR)*AGPDCHARACTER_NAME_LENGTH);
			_tcsncpy(pszBuffer, &pszList[IStartPos], nIndex - IStartPos);

			ApStrCharacterName pName;
			pName.SetText(pszBuffer);
			
			//. ��������Ʈ�� ����
			pcsChannel->m_UserList.push_back(pName);
			IStartPos = nIndex + 1; //. ';' ���� �ּҺ��� �����̸� ����~
		}
	}

	//. Admin Channel(hash)�� ���.
	m_pcsAgpmChannel->m_csAdminChannel.Add(pChannelParam->m_strName, pcsChannel);

	return pcsChannel;
}

////////////////////////////////////////////////////////////////////////////////////
//. Refresh function
//. ��ȭ�游��� �ɼ� Refresh
BOOL AgcmUIChannel::RefreshCreateRoomOption()
{
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDPublicRoom) &&
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDPrivateRoom) &&
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstUDGuildRoom);;
}
//. ��� ��ȭ�� Refresh
BOOL AgcmUIChannel::RefreshAllChatRoom()
{
	return TRUE;
}

//. �ش� �ε����� ��ȭ�游 Refresh
BOOL AgcmUIChannel::RefreshChatRoom(INT32 nIndex)
{
	return 	m_pcsAgcmUIManager2->SetUserDataRefresh(m_arRoomWidget[nIndex].m_pstUDChatList) &&
			m_pcsAgcmUIManager2->SetUserDataRefresh(m_arRoomWidget[nIndex].m_pstUDUserList );
}

//. �ش� �ε����� ��������Ʈ�� Refresh
BOOL AgcmUIChannel::RefreshChatRoomUserList(INT32 nIndex)
{
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_arRoomWidget[nIndex].m_pstUDUserList );
}

BOOL AgcmUIChannel::RefreshWhisperBtnStatus()
{
	return m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstWhiperBtnData);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. send packet
//. chat rooom create packet
BOOL AgcmUIChannel::SendCreateChannel(TCHAR* szRoomName, TCHAR* szPassword)
{
	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketCreateChannel(&nPacketLength, pcsCharacter->m_lID, 
																szRoomName, 
																szPassword, 
																m_eChannelType);	//. �ɼ� ��ư Ŭ���� �̸� ������ ������ send
	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. chatting message packet
BOOL AgcmUIChannel::SendPacketChatMsg(TCHAR *szChannelName, TCHAR *szChatMsg)
{
	if( !szChannelName || !szChatMsg )
		return FALSE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketChatMsg(&nPacketLength, pcsCharacter->m_lID, 
																szChannelName, 
																pcsCharacter->m_szID, 
																szChatMsg);
	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. Join channel packet
BOOL AgcmUIChannel::SendPacketJoinChannel(TCHAR *szChannelName)
{
	if( !szChannelName )
		return TRUE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketJoinChannel(&nPacketLength, pcsCharacter->m_lID, 
																szChannelName, 
																AGPDCHANNEL_TYPE_NONE, 
																NULL, 
																0);
	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. Leave Channel Packet
BOOL AgcmUIChannel::SendPacketLeaveChannel(TCHAR *szChannelName)
{
	if( !szChannelName )
		return TRUE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketLeaveChannel(&nPacketLength, pcsCharacter->m_lID, szChannelName);

	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. Password packet
BOOL AgcmUIChannel::SendPacketPassword(TCHAR *szChannelName, TCHAR *szPassword)
{
	if( !szChannelName || !szPassword)
		return TRUE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketPassword(&nPacketLength, pcsCharacter->m_lID, szChannelName, szPassword );

	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. invitation packet
BOOL AgcmUIChannel::SendPacketInvitation(TCHAR *szChannelName, EnumChannelType eType, TCHAR *szDestUser)
{
	if( !szChannelName || !szDestUser)
		return TRUE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketInvitation(&nPacketLength, pcsCharacter->m_lID, szChannelName, eType, szDestUser );

	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

//. Find User packet
BOOL AgcmUIChannel::SendPacketFindUser(CHAR *szFindUserName)
{
	if( !szFindUserName)
		return TRUE;

	AgpdCharacter *pcsCharacter = m_pcsAgcmCharacter->GetSelfCharacter();

	ASSERT(NULL != pcsCharacter);
	if (NULL == pcsCharacter)
		return FALSE;

	INT16 nPacketLength = 0;

	PVOID pvPacket= m_pcsAgpmChannel->MakePacketFindUser(&nPacketLength, pcsCharacter->m_lID, szFindUserName );

	if (NULL == pvPacket)
	{
		m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);
		return FALSE;
	}

	BOOL bResult = SendPacket(pvPacket, nPacketLength);
	m_pcsAgpmChannel->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. callback functions
//.
BOOL AgcmUIChannel::CBCloneUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//. SetSelfCharacter�Ǵ� �������� ��ȭ��UI�� �����س��´�.
	if (!pClass)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter *	pcsCharacter = (AgpdCharacter *) pData;
	AgcdCharacter *	pstCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	if (pstCharacter->m_bTransforming)
		return TRUE;

	if(pThis->m_sbCloneUI)
	{
		//. UI�� ������ ���´�.
		AgcdUI* pOriginal = pThis->m_pcsAgcmUIManager2->GetUI("UI_ChatRoom");
		if(!pOriginal)
			return FALSE;

		TCHAR strUIName[128];
		for(INT32 nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
		{
			AgcdUI* pClone = NULL;
			AgcdUIControl* pControl;

			wsprintf(strUIName, "UI_ChatRoom%d", nCount);
			pClone = pThis->m_pcsAgcmUIManager2->CopyUI(pOriginal, strUIName);
			
			//. 2005. 12. 20. Nonstopdj
			//. ESCŰ �ڵ� ������ ����.
			pClone->m_bAutoClose = FALSE;

			INT32 lIndex = 0;
			for (pControl = pThis->m_pcsAgcmUIManager2->GetSequenceControl(pClone, &lIndex); pControl; pControl = pThis->m_pcsAgcmUIManager2->GetSequenceControl(pClone, &lIndex))
			{
				if (pControl->m_pstUserData == pThis->m_pstDummyUDChatList)
				{
					//. ��ȭ����Ʈ UD����
					pControl->m_pstUserData = pThis->m_arRoomWidget[nCount].m_pstUDChatList;
					pThis->m_pcsAgcmUIManager2->CountUserData(pThis->m_arRoomWidget[nCount].m_pstUDChatList, pControl);
				}
				else if (pControl->m_pstUserData == pThis->m_pstDummyUDUserList)
				{
					//. ��������Ʈ UD����
					pControl->m_pstUserData = pThis->m_arRoomWidget[nCount].m_pstUDUserList;
					pThis->m_pcsAgcmUIManager2->CountUserData(pThis->m_arRoomWidget[nCount].m_pstUDUserList, pControl);
				}
			}

			pThis->m_pcsAgcmUIManager2->SetUIData(pClone, nCount);
			pThis->m_pcsAgcmUIManager2->AddUI(pClone);
			pThis->m_arAgcdUI[nCount] = pClone;
		}
	}

	return TRUE;
}


//.
BOOL AgcmUIChannel::CBDestroyChannel(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

//.
BOOL AgcmUIChannel::CBJoin(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//. todo : ��ȭ���� �����ؼ� ����� �װͰ� ����Ǵ� UD�� ������ Refresh�Ѵ�.
	//.		   ��ȭ������(m_arRoom)�� UD�� �̿��Ͽ� ������Ʈ(�޼���, �������Ե�)�� Refresh.
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	return pThis->OperationJoinChannel(pcsCharacter, pThis->CreateChannelData(pChannelParam) );
}

//.
BOOL AgcmUIChannel::CBLeave(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	AgpdChannel* pcsRemoveChannel = pThis->m_pcsAgpmChannel->m_csAdminChannel.Get(pChannelParam->m_strName);

	if(!pcsRemoveChannel)
		return FALSE;

	return pThis->OperationCloseChannel(pcsRemoveChannel);
}

//.
BOOL AgcmUIChannel::CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	pThis->m_eChannelType = pChannelParam->m_eType;
	pThis->m_strChannelName.SetText(pChannelParam->m_strName.GetBuffer());			//. ���� �����Ϸ��� ���� ��������. (�ʴ�����)
	pThis->m_strInviteCharName.SetText(pChannelParam->m_strUserName.GetBuffer());	//.	���� �ʴ��� ����� �̸� ����.

	//. ������ �������� �� �� ��ȭ�� �ʴ�â�� �ݰ� ���� ����.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_iEventInviteCloseUI);
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_iEventInviteOpenUI);
}

//. 
BOOL AgcmUIChannel::CBPassword(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	//. �����Ϸ��� ���� ������ ������ ��. ������ ���������� �𸣴� Password�Է�â��
	//. �ݰ� �ٽ� ����.
	pThis->m_strChannelName.SetText(pChannelParam->m_strName.GetBuffer());

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_iEventPasswordClose);
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_iEventPasswordOpen);
}

//. ä�ø޼��� callback
BOOL AgcmUIChannel::CBChatMsg(PVOID pData, PVOID pClass, PVOID pCustData)
{
	//. ä�ø޼��� ������·� �����Ͽ� m_arRoomWidget[].m_pcsMessage�����ϱ�
	//. refresh -> Display - Editbox->SetText�� ������Ʈ.
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	AgpdChannel *pcsChannel = pThis->m_pcsAgpmChannel->m_csAdminChannel.Get(pChannelParam->m_strName);

	if(!pcsChannel)
		return FALSE;

	return pThis->OperationChatMsg(pcsChannel, 
								   pChannelParam, 
								  _tcscmp((TCHAR*)pcsCharacter->m_szID, pChannelParam->m_strUserName.GetBuffer()));
}

//. ����ã�� ��� �ݹ�
BOOL AgcmUIChannel::CBFindUserResult(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;
	
	//. �̰��� ������ ģ���� ã�� ����.
	pThis->m_strInviteCharName.SetText(pChannelParam->m_strUserName.GetBuffer());

	//. ��ư�� ���´� Enable.
	return pThis->OperationFindUser(TRUE);
}

//. Enter Channel
BOOL AgcmUIChannel::CBAddUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	AgpdChannel *pcsChannel = pThis->m_pcsAgpmChannel->m_csAdminChannel.Get(pChannelParam->m_strName);

	if(!pcsChannel)
		return FALSE;

	//. �ڱ��ڽ��� ������ ����/���� �ý��� �޼���
	if( _tcscmp( (TCHAR*)pcsCharacter->m_szID, pChannelParam->m_strUserName.GetBuffer()))
	{
		ApString<96> strTemp;
		strTemp.Format(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CHANNEL_JOIN_MESSAGE), pChannelParam->m_strUserName.GetBuffer());
		pThis->AddSystemMessage(strTemp);
	}

	return pThis->OperationAddUser(pcsChannel, pChannelParam);
}

//. Leave Channel
BOOL AgcmUIChannel::CBRemoveUser(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;

	AgpdChannel *pcsChannel = pThis->m_pcsAgpmChannel->m_csAdminChannel.Get(pChannelParam->m_strName);

	if(!pcsChannel )
		return FALSE;

	//. �ڱ��ڽ��� ������ ����/���� �ý��� �޼���
	if( _tcscmp( (TCHAR*)pcsCharacter->m_szID, pChannelParam->m_strUserName.GetBuffer()))
	{
		ApString<96> strTemp;
		strTemp.Format(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CHANNEL_LEAVE_MESSAGE), pChannelParam->m_strUserName.GetBuffer());
		pThis->AddSystemMessage(strTemp);
	}

	return pThis->OperationRemoveUser(pcsChannel, pChannelParam);
}


//. ����/���� �޼����ڵ� throw Event.
BOOL AgcmUIChannel::CBMsgCode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData || !pCustData)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pData;
	AgpdChannelParam *pChannelParam = (AgpdChannelParam *)pCustData;
	
	int nEventMsgCode = 0;
	switch(pChannelParam->m_eMsgCode)
	{
		case AGPMCHANNEL_MSG_CODE_ALREADY_EXIST:		// �̹� �����ϴ� ä��
			nEventMsgCode = pThis->m_iEventAlreadyExist;
			break;
		case AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHANNEL:	// �������� �ʴ� ä��
			nEventMsgCode = pThis->m_iEventNotExistChannel;
			break;
		case AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHARACTER:	// ã�� ĳ���Ͱ� �������� �ʰų� Off-line
			nEventMsgCode = pThis->m_iEventNotExistCharacter;
			break;
		case AGPMCHANNEL_MSG_CODE_FULL_CHANNEL:			// ���ÿ� ���� ������ ä���� ����á��.
			nEventMsgCode = pThis->m_iEvnetFullChannel;
			break;
		case AGPMCHANNEL_MSG_CODE_NOT_GUILDMASTER:		// ��� �����Ͱ� �ƴϴ�.
			nEventMsgCode = pThis->m_iEventNotGuildMaster;
			break;
		case AGPMCHANNEL_MSG_CODE_NOT_GUILDMEMBER:		// ��� ����� �ƴϴ�.
			nEventMsgCode = pThis->m_iEventNotGuildMember;
			break;
		case AGPMCHANNEL_MSG_CODE_ALREADY_GUILDCHANNEL:	// �̹� ��� ä���� �������.
			nEventMsgCode = pThis->m_iEventAlreadyGuildChannel;
			break;
		case AGPMCHANNEL_MSG_CODE_INVALID_PASSWORD:		// �н����尡 ��ȿ���� �ʴ�.
			nEventMsgCode = pThis->m_iEventInvalidPassword;
			break;
		case AGPMCHANNEL_MSG_CODE_ALREADY_JOIN:
			pThis->AddSystemMessage(ClientStr().GetStr(STI_ALREADYCHANNEL_JOIN));
			break;
	}

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(nEventMsgCode);
}

//. AgcdUIData Clear.
BOOL AgcmUIChannel::CBCloneUIClear(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIChannel *	pThis = (AgcmUIChannel*) pClass;
	AgpdCharacter *	pcsCharacter = (AgpdCharacter *) pData;
	AgcdCharacter *	pstCharacter = pThis->m_pcsAgcmCharacter->GetCharacterData(pcsCharacter);

	if (pstCharacter->m_bTransforming)
		return TRUE;

	for (INT32 i = 0; i < AGPDCHANNEL_MAX_JOIN; ++i)
	{
		pThis->m_arRoomWidget[i].InitUserIndex();
		pThis->m_pcsAgcmUIManager2->RemoveUI(pThis->m_arAgcdUI[i]);

		pThis->m_arAgcdUI[i] = NULL;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. Operation
//. ä������ó��.
BOOL AgcmUIChannel::OperationJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannel *pcsChannel)
{
	//. Close���¸� UI�� �ش��ϴ� AgcdUI�� ã�� �� ID�� ����� ��������
	//. pcsChannel�� �����ϰ�. UserData�� Refresh�Ѵ�.

	for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{
		if( AGCDUI_STATUS_ADDED == m_arAgcdUI[nCount]->m_eStatus)
		{
			//. ������ �����Ǿ� �������� �� ���ڿ����� Ŭ���� �Ѵ�.
			m_arRoomWidget[nCount].clear();
			m_arRoomWidget[nCount].m_pcsChannel = pcsChannel;

			//. ���������ŭ Display�Լ��� ȣ���ϱ� ����
			m_arRoomWidget[nCount].m_pstUDUserList->m_stUserData.m_lCount = pcsChannel->m_UserList.size();
			
			//. �ش�â����
			m_pcsAgcmUIManager2->OpenUI(m_arAgcdUI[nCount]);

			//. 2006. 1. 6. Nonstopdj
			//. ��ȭ�氳�� �α׳����.
			if(m_bIsCreateRoom)
			{
				ApString<256> strTemp;
				strTemp.Format(m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CHANNEL_CREATE_MESSAGE), pcsChannel->m_strName.GetBuffer());
				AddSystemMessage(strTemp);

				m_bIsCreateRoom = FALSE;
			}

			return RefreshChatRoom(nCount);
		}
	}

	//. error : �����ִ� ���� �ϳ��� �����Ƿ�. 5���ʰ��� �������� ��û�ϰų� 
	//.			m_arAgcdUI Data�� �̻�.
	return FALSE;
}

//. ä�ø޼���ó��.
BOOL AgcmUIChannel::OperationChatMsg(AgpdChannel* pcsChannel, AgpdChannelParam *pChannelParam, INT32 IsSelf)
{
	for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{
		if(m_arRoomWidget[nCount].m_pcsChannel == pcsChannel)
		{
			//. Chatting Messesage Add.
			m_arRoomWidget[nCount].AddChatMsg(pChannelParam, (IsSelf) ?  OTHERCHATCOLOR : SELFCHATCOLOR );

			return RefreshChatRoom(nCount);
		}
	}

	return FALSE;
}

BOOL AgcmUIChannel::OperationFindUser(BOOL bResult)
{
	(bResult) ? m_bisWhisperBtnEnable = TRUE :  m_bisWhisperBtnEnable = FALSE;
	return RefreshWhisperBtnStatus();
}

//. ä�γ�����.
BOOL AgcmUIChannel::OperationCloseChannel(AgpdChannel* pChannel)
{
	for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{
		//. 2006. 7. 14. Nonstopdj
		//. �� ä���̻� ������� ��ä�ο��� ������
		//. m_arRoomWidget[nCount].m_pcsChannel�� NULL�̹Ƿ�
		//. NULLüũ�� �ʿ��ϴ�.
		if( m_arRoomWidget[nCount].m_pcsChannel && m_arRoomWidget[nCount].m_pcsChannel->m_strName == pChannel->m_strName)
		{
			//. HashData Remove
			m_pcsAgpmChannel->m_csAdminChannel.Remove(pChannel->m_strName);
			//. ����Ǿ��ִ� �������� Clear
			m_pcsAgpmChannel->DestroyChannelData(m_arRoomWidget[nCount].m_pcsChannel);

			m_arRoomWidget[nCount].clear();

			//. UI�� �ݴ´�.
			return m_pcsAgcmUIManager2->CloseUI(m_arAgcdUI[nCount]);
		}
	}

	return FALSE;
}

//. User�߰�.
BOOL AgcmUIChannel::OperationAddUser(AgpdChannel* pChannel, AgpdChannelParam *pChannelParam)
{
	for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{
		if(m_arRoomWidget[nCount].m_pcsChannel && m_arRoomWidget[nCount].m_pcsChannel == pChannel)
		{
			m_arRoomWidget[nCount].m_pcsChannel->m_UserList.push_back(pChannelParam->m_strUserName);
			m_arRoomWidget[nCount].m_pstUDUserList->m_stUserData.m_lCount = m_arRoomWidget[nCount].m_pcsChannel->m_UserList.size();

			//. ��������Ʈ�� Refresh�Ѵ�.
			return RefreshChatRoomUserList(nCount);
		}
	}

	return FALSE;
}

//. User����.
BOOL AgcmUIChannel::OperationRemoveUser(AgpdChannel* pChannel, AgpdChannelParam *pChannelParam)
{
	for( int nCount = 0; nCount < AGPDCHANNEL_MAX_JOIN; nCount++)
	{
		if(m_arRoomWidget[nCount].m_pcsChannel && m_arRoomWidget[nCount].m_pcsChannel == pChannel)
		{
			//. �ش��̸��� ������ ã�´�.
			std::list<ApStrCharacterName>::iterator Iter = std::find(m_arRoomWidget[nCount].m_pcsChannel->m_UserList.begin(), 
														m_arRoomWidget[nCount].m_pcsChannel->m_UserList.end(), pChannelParam->m_strUserName);

			if (Iter != m_arRoomWidget[nCount].m_pcsChannel->m_UserList.end())
			{
				//. �ش������� �����.
				m_arRoomWidget[nCount].m_pcsChannel->m_UserList.erase(Iter);
				m_arRoomWidget[nCount].m_pstUDUserList->m_stUserData.m_lCount = m_arRoomWidget[nCount].m_pcsChannel->m_UserList.size();
			}

			//. ��������Ʈ�� Refresh�Ѵ�.
			return RefreshChatRoomUserList(nCount);
		}
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////
//. display
//. ��ȭ���� Display
BOOL AgcmUIChannel::CBDisplayChatList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	if( nIndex < 0 && nIndex > AGPDCHANNEL_MAX_JOIN)
		return FALSE;

	AcUIEdit	*pcsEdit	= (AcUIEdit *)pcsSourceControl->m_pcsBase;

	//. �̸� ����� ���ڿ��� SetText�Ѵ�.
	if(pcsEdit)
	{
		pcsEdit->SetText(pThis->m_arRoomWidget[nIndex].m_pcsMessage.GetBuffer());

		//. ������ ��ũ��max������ �����Ͽ� ���� �߰��� �� ������ ���� �Ʒ��� �����ع���.
		if(pcsEdit->GetVScroll())
			pcsEdit->GetVScroll()->SetScrollValue(1.0f);
	}
	else
		return FALSE;

	return TRUE;
}

//. ��ȭ�泻 ������� Display
BOOL AgcmUIChannel::CBDisplayUserList(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	INT32 Index = *(INT32*)pData;

	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	if( nIndex < 0 && nIndex > AGPDCHANNEL_MAX_JOIN)
		return FALSE;

	//. �Ѹ� Draw�ϹǷ� ���� Index�� �̿��Ͽ� ������ ������ ���Ѵ��� szDisplay�� copy�Ѵ�.
	if(NULL != pThis->m_arRoomWidget[nIndex].m_pcsChannel &&
		( INT32 ) pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_UserList.size() > pcsSourceControl->m_lUserDataIndex)
	{
		std::list<ApStrCharacterName>::iterator Iter = pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_UserList.begin();

		for(int nCount = 0; nCount < Index; nCount++ )
			Iter++;

		strncpy(szDisplay, (*Iter).GetBuffer(), AGPDCHARACTER_NAME_LENGTH);
	}

	return TRUE;
}

//. ��ȭ������ Display
BOOL AgcmUIChannel::CBDisplayChannelTitle(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || eType != AGCDUI_USERDATA_TYPE_INT32)
		return FALSE;
	
	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	if(pThis->m_arRoomWidget[nIndex].m_pcsChannel)
	{
		strncpy(szDisplay, pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_strName.GetBuffer(),
			pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_strName.GetBufferLength());
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////
//. functions
//. ��ȭ�� �ʱ�ȭ
BOOL AgcmUIChannel::CBCreateRoomClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pData2 )
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	//. ��ȭ�� ����� �⺻ �ɼ��� ������
	pThis->m_bIsPublicRoom		= TRUE;
	pThis->m_bIsPrivateRoom		= FALSE;
	pThis->m_bIsGuildRoom		= FALSE;
	pThis->m_eChannelType		= AGPDCHANNEL_TYPE_PUBLIC;		 //. �⺻�ɼ��� ������
	pThis->RefreshCreateRoomOption();

	AgcdUIControl* pUIRoomName = (AgcdUIControl*)pData1;
	AgcdUIControl* pUIPassword = (AgcdUIControl*)pData2;
	AcUIEdit* pUIRoomNameEdit = (AcUIEdit*)pUIRoomName->m_pcsBase;
	AcUIEdit* pUIPasswordEdit = (AcUIEdit*)pUIPassword->m_pcsBase;

	pUIRoomNameEdit->SetText("");
	pUIPasswordEdit->SetText("");
	pUIRoomNameEdit->SetMeActiveEdit();

	return TRUE;
}

//. �ʴ���� ä���� ����ǥ�ø� ���� �ʱ�ȭ.
BOOL AgcmUIChannel::CBInviteRoomInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgcdUIControl* pUIEditControl = (AgcdUIControl*)pData1;
	AcUIEdit* pUIEdit = (AcUIEdit*)pUIEditControl->m_pcsBase;

	//. todo EditBox�� ����� �ʴ�߽�ĳ�����̸��� ���������� SetText
	TCHAR pBuffer[256];
	TCHAR pChanneTypeName[32];

	//. channel Ÿ�Կ� ���� ��Ӽ����ڿ�����
	if(AGPDCHANNEL_TYPE_PUBLIC == pThis->m_eChannelType)
		_stprintf(pChanneTypeName, "%s", ClientStr().GetStr(STI_PUBLIC_CHANNEL) );
	else if(AGPDCHANNEL_TYPE_PRIVATE == pThis->m_eChannelType)
		_stprintf(pChanneTypeName, "%s", ClientStr().GetStr(STI_PRIVATE_CHANNEL) );
	else if(AGPDCHANNEL_TYPE_GUILD == pThis->m_eChannelType)
		_stprintf(pChanneTypeName, "%s", ClientStr().GetStr(STI_GUILD_CHANNEL) );

	_stprintf(pBuffer, ClientStr().GetStr(STI_CHANNEL_INVITE),
		pThis->m_strInviteCharName.GetBuffer(),
		pThis->m_strChannelName.GetBuffer(),
		ROOMTYPECOLOR,
		pChanneTypeName,
		OTHERCHATCOLOR);

	return pUIEdit->SetText(pBuffer);
}

//. EditBox�� ù��° ���ڷ� �־� �ʱ�ȭ�ϰ� ����Ʈ�ڽ��� ��Ŀ���� �����.  ; ��ȭ�� ����, ��ȭ�� �ʴ�, ĳ����ã��, ��й�ȣ �Է¿� ����
BOOL AgcmUIChannel::CBEditBoxClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgcdUIControl* pUIEditControl = (AgcdUIControl*)pData1;

	AcUIEdit* pUIEdit = (AcUIEdit*)pUIEditControl->m_pcsBase;
	pUIEdit->SetText("");
	pUIEdit->SetMeActiveEdit();

	return TRUE;
}

//. Find User UI�� ���� Clear
BOOL AgcmUIChannel::CBFindUserClear(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	AgcdUIControl* pUIEditControl = (AgcdUIControl*)pData1;

	//. ������ ã�Ҵ� ID�� Clear�ϰ� focus�� ����.
	AcUIEdit* pUIEdit = (AcUIEdit*)pUIEditControl->m_pcsBase;
	pUIEdit->SetText("");
	pUIEdit->SetMeActiveEdit();

	//. ���� ���� �Ӹ���ư�� disable��Ų��.
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pstWhiperBtnData);

	return TRUE;
}


//***************************************************************************************************
// Function Name	: bool CBCreateRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
//
// Last Modified 	: 2005. 11. 22	Nonstopdj
// First Modified	: 2005. 11. 22	Nonstopdj
//
// Function			: UI_CreateChatRoom���� ��ȭ�� ������ư�� Ŭ������ ���� ȣ��Ǵ� callback
//
// Inputs			: 
//
// Outputs			: 
//
// Returns			: Boolean
//***************************************************************************************************
BOOL AgcmUIChannel::CBCreateRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (NULL == pcsCharacter)
		return FALSE;

	AgcdUIControl* pUIRoomName = (AgcdUIControl*)pData1;
	AgcdUIControl* pUIPassword = (AgcdUIControl*)pData2;
	
	if((AcUIBase::TYPE_EDIT != pUIRoomName->m_lType) && (AcUIBase::TYPE_EDIT != pUIPassword->m_lType))
		return FALSE;

	AcUIEdit* pUIRoomNameEdit = (AcUIEdit*)pUIRoomName->m_pcsBase;
	AcUIEdit* pUIPasswordEdit = (AcUIEdit*)pUIPassword->m_pcsBase;

	if(strlen(pUIRoomNameEdit->GetText()) < ApStrChannelName::eBufferLength)
	{
		AgpdChannel channel;
		channel.Initialize(pThis->m_eChannelType);
		channel.m_strName.SetText(pUIRoomNameEdit->GetText());

		if(pThis->m_eChannelType == AGPDCHANNEL_TYPE_PRIVATE )
		{
			if( strlen(pUIPasswordEdit->GetText()) < ApStrChannelName::eBufferLength && strlen(pUIPasswordEdit->GetText()) > 0)
				pThis->SendCreateChannel((TCHAR*)pUIRoomNameEdit->GetText(), (TCHAR*)pUIPasswordEdit->GetText());
			else
			{
				//. todo: �ý��� �޼���
				//. '��й�ȣ�� �Է����ּ���.'
				pThis->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_CHANNEL_NEED_PASSWORD));
				return FALSE;
			}
		}
		else
		{
			if(pThis->SendCreateChannel((TCHAR*)pUIRoomNameEdit->GetText(), NULL))
				pThis->m_bIsCreateRoom = TRUE; //. ��ȭ�濡 ������ ������ ��ȭ���� ��������� �Ǻ��Ѵ�.
		}
		
	}
	else
	{
		//. todo : �ý��۸޼���
		//. '��ȭ�������� �Է��ϼ���'
		return FALSE;
	}

	//. error : ����Ʈ�ڽ��� ���ڿ����̰� ������ ������ ũ��.
	//.			������ ������.

	//. EditBox Clear.
	pUIRoomNameEdit->SetText("");
	pUIPasswordEdit->SetText("");

	return TRUE;
}

//. ������ �ɼ� üũ
BOOL AgcmUIChannel::CBCheckPulicOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	pThis->m_bIsPublicRoom		= TRUE;
	pThis->m_bIsPrivateRoom		= FALSE;
	pThis->m_bIsGuildRoom		= FALSE;

	pThis->m_eChannelType = AGPDCHANNEL_TYPE_PUBLIC;

	return pThis->RefreshCreateRoomOption();
}

//. ��й� �ɼ� üũ
BOOL AgcmUIChannel::CBCheckPrivateOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	pThis->m_bIsPublicRoom		= FALSE;
	pThis->m_bIsPrivateRoom		= TRUE;
	pThis->m_bIsGuildRoom		= FALSE;

	pThis->m_eChannelType = AGPDCHANNEL_TYPE_PRIVATE;

	return pThis->RefreshCreateRoomOption();
}

//. ���� �ɼ�üũ
BOOL AgcmUIChannel::CBCheckGuildOptionClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (NULL == pClass)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	pThis->m_bIsPublicRoom		= FALSE;
	pThis->m_bIsPrivateRoom		= FALSE;
	pThis->m_bIsGuildRoom		= TRUE;
	pThis->m_eChannelType = AGPDCHANNEL_TYPE_GUILD;

	return pThis->RefreshCreateRoomOption();
}

//. ��ȭ�� ���̾�α׿��� �޼��� �Է� �� Enter�� �Է����� ���.
BOOL AgcmUIChannel::CBEnterChatMessage(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	if (AcUIBase::TYPE_EDIT != pcsSourceControl->m_lType)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	if( nIndex < 0 && nIndex > AGPDCHANNEL_MAX_JOIN)
		return FALSE;

	AcUIEdit* pUIEdit = (AcUIEdit*)pcsSourceControl->m_pcsBase;
	CHAR* szDelimiter = pThis->m_pcsAgcmUIManager2->GetLineDelimiter();
	pUIEdit->SetLineDelimiter(szDelimiter);

	//. chatting �޼��� ��Ŷ ����
	//. EditBox�� �Էµ� ���ڿ��� �ִ� 120���� ������ �����س�����.
	
	//. 2005. 12. 15. Nonstopdj.
	//. �����Է��Ŀ��� �ڵ����� ��Ŀ���� ������Ű�� �ɼ��� ������. by ������PD��.
	if(strlen(pUIEdit->GetText()) > 0)
	{
		pThis->SendPacketChatMsg(pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_strName, (TCHAR*)pUIEdit->GetText());
		pUIEdit->SetText("");		//. ������ �Է��ߴ� ���������
	}

	return TRUE;
}

//. ��ȭ�� �ʴ�(Send)
BOOL AgcmUIChannel::CBInviteOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;
	
	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	//. �ʴ� ID UIâ���� ����ϱ� ���� ���̸��� ��Ӽ� �ӽ� ����
	pThis->m_strChannelName.SetText(pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_strName.GetBuffer());
	pThis->m_eChannelType = pThis->m_arRoomWidget[nIndex].m_pcsChannel->GetChannelType();

	return 	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_iEventInviteSendUI);
}

//. �ʴ� �޼��� ������
BOOL AgcmUIChannel::CBInviteSend(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	AgcdUIControl* pUIEditCtrl = (AgcdUIControl*)pData1;
	
	if(AcUIBase::TYPE_EDIT != pUIEditCtrl->m_lType)
		return FALSE;

	AcUIEdit* pUIEdit = (AcUIEdit*)pUIEditCtrl->m_pcsBase;

	return pThis->SendPacketInvitation(pThis->m_strChannelName.GetBuffer(), pThis->m_eChannelType, (TCHAR*)pUIEdit->GetText());
}

//. ���� ã��
BOOL AgcmUIChannel::CBFindUser(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	AgcdUIControl* pUIEditCtrl = (AgcdUIControl*)pData1;
	
	if(AcUIBase::TYPE_EDIT != pUIEditCtrl->m_lType)
		return FALSE;

	AcUIEdit* pUIEdit = (AcUIEdit*)pUIEditCtrl->m_pcsBase;

	return pThis->SendPacketFindUser((TCHAR*)pUIEdit->GetText());
}

//. �Ӹ������� ��ư ���¹ٲٱ�
BOOL AgcmUIChannel::CBWhiperBtnStatus(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	//. static BOOL �̹Ƿ�.
	if(m_bisWhisperBtnEnable)
	{
		//. ���� ���¸� ���Ͽ�.
		m_bisWhisperBtnEnable = FALSE;
		return TRUE;
	}

	return FALSE;
}

//. ã�� ���� �Ӹ� ������ 
BOOL AgcmUIChannel::CBSendWhisper(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	//. to do: '/w ����' -> �븻��ȭâ�� Insert
	if (!pClass)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	ApString<64> strWhisperMsg;
	strWhisperMsg.Format(_T("%s %s "), ClientStr().GetStr(STI_CHAT_WHISPER_LOWER), pThis->m_strInviteCharName.GetBuffer());

	// Chat UI�� Open�� �ӼӸ� �޼����� �߰��Ѵ�.
	pThis->m_pcsAgcmUIChatting2->ChatInputNormalUIOpen();
	return pThis->m_pcsAgcmUIChatting2->SetChattingMessage(strWhisperMsg);
}


//. �濡 ����
BOOL AgcmUIChannel::CBJoinRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	AgcdUIControl* pUIRoomName = (AgcdUIControl*)pData1;
	
	if(AcUIBase::TYPE_EDIT != pUIRoomName->m_lType)
		return FALSE;

	AcUIEdit* pUIRoomNameEdit = (AcUIEdit*)pUIRoomName->m_pcsBase;
	
	if(pThis->SendPacketJoinChannel((TCHAR*)pUIRoomNameEdit->GetText()))
		pThis->m_bIsCreateRoom = FALSE; //. ��ȭ�濡 ������ ��

	pUIRoomNameEdit->SetText("");

	return TRUE;
}

//. �ʴ뿡 ���� �濡 ����
//. �ʴ��ݹ��� ȣ��Ǹ� ȣ���� ���� ������ ������� ������ �� ����Ѵ�.
BOOL AgcmUIChannel::CBInviteJoinRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
			return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	return pThis->SendPacketJoinChannel((TCHAR*)pThis->m_strChannelName.GetBuffer());
}

//. ��ȭ���� ����
BOOL AgcmUIChannel::CBLeaveRoom(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
			return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	//. ������ UI Data�� Index�̹Ƿ�..
	int nIndex = pThis->m_pcsAgcmUIManager2->GetUIData(pcsSourceControl->m_pcsParentUI);

	return pThis->SendPacketLeaveChannel((TCHAR*)(pThis->m_arRoomWidget[nIndex].m_pcsChannel->m_strName.GetBuffer()));
}

//. �н������Է�OK Ŭ��
BOOL AgcmUIChannel::CBQueryPassword(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIChannel* pThis = (AgcmUIChannel*)pClass;

	AgcdUIControl* pUIPassword = (AgcdUIControl*)pData1;

	if(AcUIBase::TYPE_EDIT != pUIPassword->m_lType)
		return FALSE;

	AcUIEdit* pUIPasswordEdit = (AcUIEdit*)pUIPassword->m_pcsBase;

	pThis->SendPacketPassword(pThis->m_strChannelName.GetBuffer(),  (TCHAR*)pUIPasswordEdit->GetText());
	pUIPasswordEdit->SetText("");	//. Edit�ڽ����� ����

	return TRUE;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//. script
void  AgcmUIChannel::CreateChannel()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_iEventCreateChannel);
}

void  AgcmUIChannel::JoinChannel()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_iEventJoinChannel);
}
void  AgcmUIChannel::FindUser()
{
	m_pcsAgcmUIManager2->ThrowEvent(m_iEventFindUserUI);
}

//. �ý��� �޼����� �޼��� ����ϱ�
BOOL AgcmUIChannel::AddSystemMessage(CHAR* szMsg)
{
	if(!szMsg)
		return FALSE;

	AgpdChatData	stChatData;
	memset(&stChatData, 0, sizeof(stChatData));

	stChatData.eChatType = AGPDCHATTING_TYPE_SYSTEM_LEVEL1;
	stChatData.szMessage = szMsg;

	stChatData.lMessageLength = strlen(stChatData.szMessage);
		
	m_pcsAgcmChatting2->AddChatMessage(AGCMCHATTING_TYPE_SYSTEM, &stChatData);
	m_pcsAgcmChatting2->EnumCallback(AGCMCHATTING2_CB_INPUT_MESSAGE, NULL, NULL);

	return TRUE;
}

// -----------------------------------------------------------------------------
// AgcmUIChannel.cpp - End of file
// -----------------------------------------------------------------------------