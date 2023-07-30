#pragma once

#include "AgsEngine.h"
#include "AgpdChannel.h"
#include "AgpmChannel.h"

class AgpmChannel;
class AgsmCharacter;
class AgpdCharacter;
class AgsmGuild;
class AgpmGuild;

class AgsdChannelUserList : public ApMemory<AgsdChannelUserList, 20>
{
public:
	ApString<ApStrCharacterName::eBufferLength * AGPDCHANNEL_MEMBER_GUILD> m_strChannelUserList;
};

const INT32 AGSDCHANNEL_DELAY = 30 * 1000;	// 30��
const INT32 AGSDCHANNEL_DELAY_DESTROY = 5 * 60 * 1000;	// 5��

class AgsmChannel : public AgsModule
{
private:
	AgpmChannel		*m_pcsAgpmChannel;
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgsmCharacter	*m_pcsAgsmCharacter;
	AgsmGuild		*m_pcsAgsmGuild;
	AgpmGuild		*m_pcsAgpmGuild;

	UINT32			m_ulTickCount;

public:
	AgsmChannel();
	virtual ~AgsmChannel();

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();

	// callback functions
	static BOOL CBCreateChannel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDestroyChannel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBFindUser(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBLeaveChannel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBPassword(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBChatMsg(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBJoinChannel(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBInvitation(PVOID pData, PVOID pClass, PVOID pCustData);

	// ä���� ���������� ������ �������� Ȯ��
	BOOL IsValidCreateChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	// ä�ο� �����Ҷ� ������ �������� Ȯ��
	BOOL IsValidJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	// ä�ο� �ʴ��Ҷ� ������ �������� Ȯ��
	BOOL IsValidInvitation(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	// ��� ä�� ���������� ����
	BOOL SetCreatedGuildChannel(AgpdCharacter *pcsCharacter, BOOL bSet, ApStrGuildName &rStrGuildName);
	// PC�� �α׾ƿ� �Ҷ� ó��
	BOOL DisconnectCharacter(AgpdCharacter *pcsCharacter);
	// ä���� �����Ѵ�.
	BOOL DestroyChannel(AgpdChannel *pChannel);

	// operations
	BOOL OperationCreateChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationDestroyChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationFindUser(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationLeaveChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName);
	BOOL OperationPassword(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationChatMsg(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationJoinChannel(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);
	BOOL OperationInvitation(AgpdCharacter *pcsCharacter, AgpdChannelParam *pParam);

	// packet 
	BOOL SendPacketCreateChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType);
	BOOL SendPacketJoinChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType, AgpdChannel *pcsChannel);
	BOOL SendPacketDestroyChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType);
	BOOL SendPacketMsgCode(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode);
	BOOL SendPacketFindUser(AgpdCharacter *pcsCharacter, ApStrCharacterName &rUserName);
	BOOL SendPacketLeaveChannel(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName);
	BOOL SendPacketChatMsg(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName, ApStrCharacterName &rUserName, ApStrChannelChatMsg &rChatMsg);
	BOOL SendPacketAddUser(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsAddUser, ApStrChannelName &rChannelName);
	BOOL SendPacketRemoveUser(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsRemoveUser, ApStrChannelName &rChannelName);
	BOOL SendPacketPassword(AgpdCharacter *pcsCharacter, ApStrChannelName &rChannelName);
	BOOL SendPacketInvitation(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsInviteUser, EnumChannelType eType, ApStrChannelName &rChannelName);
};