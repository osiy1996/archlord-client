#pragma once

#include <ApModule/ApModule.h>
#include "AgpdChannel.h"
#include "AgpaChannel.h"

class AgpmCharacter;

enum EnumAgpmChannelDataType
{
	AGPMCHANNEL_DATA_CHANNEL,
};

enum EnumAgpmChannelOperation
{
	AGPMCHANNEL_OPERATION_NONE			= 0,
	AGPMCHANNEL_OPERATION_CREATE,			// ä�ù��� ����
	AGPMCHANNEL_OPERATION_DESTROY,			// ä�ù��� ����
	AGPMCHANNEL_OPERATION_JOIN,				// ä�� ����
	AGPMCHANNEL_OPERATION_LEAVE,			// ä�� ����
	AGPMCHANNEL_OPERATION_ADD_USER,			// ���� ����
	AGPMCHANNEL_OPERATION_REMOVE_USER,		// ���� ����
	AGPMCHANNEL_OPERATION_INVITATION,		// �ʴ�
	AGPMCHANNEL_OPERATION_PASSWORD,			// �н����带 �䱸
	AGPMCHANNEL_OPERATION_CHATMSG,			// ä�� �޼��� ����
	AGPMCHANNEL_OPERATION_FIND_USER,		// ����ڸ� ã��
	AGPMCHANNEL_OPERATION_MSG_CODE,			// Message code
	AGPMCHANNEL_OPERATION_MAX,
};

enum EnumAgpmChannelCallback
{
	AGPMCHANNEL_CB_NONE					= 0,
	AGPMCHANNEL_CB_CREATE,		
	AGPMCHANNEL_CB_DESTROY,		
	AGPMCHANNEL_CB_JOIN,			
	AGPMCHANNEL_CB_LEAVE,		
	AGPMCHANNEL_CB_ADD_USER,		
	AGPMCHANNEL_CB_REMOVE_USER,	
	AGPMCHANNEL_CB_INVITATION,	
	AGPMCHANNEL_CB_PASSWORD,		
	AGPMCHANNEL_CB_CHATMSG,		
	AGPMCHANNEL_CB_FIND_USER,	
	AGPMCHANNEL_CB_MSG_CODE,		
	AGPMCHANNEL_CB_MAX,
};

enum EnumAgpmChannelMsgCode
{
	AGPMCHANNEL_MSG_CODE_NONE			=0,
	AGPMCHANNEL_MSG_CODE_ALREADY_EXIST,			// �̹� �����ϴ� ä��
	AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHANNEL,		// �������� �ʴ� ä��
	AGPMCHANNEL_MSG_CODE_NOT_EXIST_CHARACTER,	// ã�� ĳ���Ͱ� �������� �ʰų� Off-line
	AGPMCHANNEL_MSG_CODE_FULL_CHANNEL,			// ���ÿ� ���� ������ ä���� ����á��.
	AGPMCHANNEL_MSG_CODE_NOT_GUILDMASTER,		// ��� �����Ͱ� �ƴϴ�.
	AGPMCHANNEL_MSG_CODE_NOT_GUILDMEMBER,		// ��� ����� �ƴϴ�.
	AGPMCHANNEL_MSG_CODE_ALREADY_GUILDCHANNEL,	// �̹� ��� ä���� �������.
	AGPMCHANNEL_MSG_CODE_INVALID_PASSWORD,		// �н����尡 ��ȿ���� �ʴ�.
	AGPMCHANNEL_MSG_CODE_VALID_PASSWORD,		// ��ȿ�ϴٰ� ������ �н�����
	AGPMCHANNEL_MSG_CODE_FULL_USER,				// ä�ο� ���� ������ ������ ����á��.
	AGPMCHANNEL_MSG_CODE_ALREADY_JOIN			// �̹� �������� ä��
};

struct AgpdChannelParam : public ApMemory<AgpdChannelParam, 20>
{
	ApStrChannelName		m_strName;
	ApStrChannelPassword	m_strPassword;
	EnumChannelType			m_eType;
	EnumAgpmChannelMsgCode	m_eMsgCode;
	ApStrChannelChatMsg		m_strChatMsg;
	ApStrCharacterName		m_strUserName;
	PVOID					m_pvUserList;
	UINT16					m_unByteLength;
};

class ApmMap;
class AgpmSystemMessage;

class AgpmChannel : public ApModule
{
private:
	AgpmCharacter*		m_pcsAgpmCharacter;	
	INT32				m_lIndexAttachData;

	ApmMap*				m_papmMap;
	AgpmSystemMessage*	m_pcsAgpmSystemMessage;

public:
	AuPacket		m_csPacket;
	AgpaChannel		m_csAdminChannel;

private:
	PVOID MakePacketChannel(INT16 *pnPacketLength, EnumAgpmChannelOperation eOperation, INT32 lCID, TCHAR *szChannelName, 
							TCHAR *szPassword, TCHAR *szCharName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode, 
							TCHAR *szChatMsg, PVOID pvUserList, UINT16 unMessageLength);

public:
	AgpmChannel();
	virtual ~AgpmChannel();

	BOOL SetMaxChannel(INT32 lMaxChannel);

	virtual BOOL OnAddModule();
	virtual BOOL OnInit();
	virtual BOOL OnIdle2(UINT32 ulClockCount);
	virtual BOOL OnDestroy();
	virtual BOOL OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg);

	static BOOL AgpdJoinChannelConstructor(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL AgpdJoinChannelDestructor(PVOID pData, PVOID pClass, PVOID pCustData);

	AgpdChannel* CreateChannelData();
	BOOL DestroyChannelData(AgpdChannel *pcsChannel);

	// callback functions
	BOOL SetCallbackCreateChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackDestroyChannel(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackJoin(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackLeave(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackInvitation(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackPassword(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackChatMsg(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackFindUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackAddUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackRemoveUser(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL SetCallbackMsgCode(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	// packet functions
	//. 2005. 11. 23 Nonstopdj
	//. ��й�ȣ �����߰�.
	PVOID MakePacketCreateChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, TCHAR* szPassword, EnumChannelType eType);

	PVOID MakePacketDestroyChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR* szChannelName, EnumChannelType eType);
	PVOID MakePacketMsgCode(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, EnumAgpmChannelMsgCode eMsgCode);
	PVOID MakePacketJoinChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, PVOID pvUserList, UINT16 unMessageLength);
	PVOID MakePacketLeaveChannel(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName);
	PVOID MakePacketAddUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName);
	PVOID MakePacketRemoveUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName);
	PVOID MakePacketChatMsg(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szUserName, TCHAR *szChatMsg);
	PVOID MakePacketPassword(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, TCHAR *szPassword);
	PVOID MakePacketInvitation(INT16 *pnPacketLength, INT32 lCID, TCHAR *szChannelName, EnumChannelType eType, TCHAR *szUserName);
	PVOID MakePacketFindUser(INT16 *pnPacketLength, INT32 lCID, TCHAR *szUserName);

	AgpdJoinChannelADChar* GetAttachAgpdJoinChannelData(AgpdCharacter *pcsCharacter);
};