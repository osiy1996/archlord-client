// AgpmSystemMessage.h
// (C) NHN Games - ArchLord Development Team
// kelovon, 20050906

#ifndef _AGPM_SYSTEM_MESSAGE_H_
#define _AGPM_SYSTEM_MESSAGE_H_

#include <ApModule/ApModule.h>
#include <AuPacket/AuPacket.h>
#include <ApSingleton/SingleTon.h>

#define AGPMSYSTEMMESSAGE_MAX_STRING_LENGTH		64

typedef enum _eAgpmSystemMessagePacketType
{
	AGPMSYSTEMMESSAGE_PACKET_SYSTEM_MESSAGE = 0	,
	AGPMSYSTEMMESSAGE_PACKET_MODAL_MESSAGE		,
	AGPMSYSTEMMESSAGE_PACKET_MODALLESS_MESSAGE	,
	AGPMSYSTEMMESSAGE_PACKET_CHAT_MESSAGE		,
	AGPMSYSTEMMESSAGE_PACKET_MAX
} eAgpmSystemMessagePacketType;

typedef enum _eAgpmSystemMessageCallback
{
	AGPMSYSTEMMESSAGE_CB_SYSTEM_MESSAGE = 0,
	AGPMSYSTEMMESSAGE_CB_MAX,
} eAgpmSystemMessageCallback;

typedef enum _eAgpmSystemMessageCode
{
	AGPMSYSTEMMESSAGE_CODE_LOTTERY_ITEM_TO_POT_ITEM			= 0,	// ���� ���� ������ ��ȯ
	AGPMSYSTEMMESSAGE_CODE_LOTTERY_KEY_NOT_USABLE,					// ���� ���� ����� �ܵ� ��� �Ұ�
	AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_GOLD_KEY,				// ���� ���ڸ� ���� ���� ���� �ʿ�
	AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_SILVER_KEY,				// ���� ���ڸ� ���� ���� ���� �ʿ�
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_STOP_USING_ITEM,				// %s �������� ��� ���� �Ǿ����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_END_USING_ITEM,				// %s �������� ��� ���� �Ǿ����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_ITEM,				// ĳ�� �������� ��� ������ �Ұ����մϴ�.
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_SAME_KIND_ALREADY_IN_USE,		// ���� ����� �������� �̹� ������Դϴ�.
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_PAUSE,							// ĳ�� �������� ���� ���� �Ǿ����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_CASH_ITEM_CANNOT_UNUSE_STATUS,			// ĳ�� �������� ��� ���� �� �� ���� �����Դϴ�.
	AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MAX_COUNT,
	AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_INVALID_TARGET,
	AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_ALREADY_TAMING,
	AGPMSYSTEMMESSAGE_CODE_TAMABLE_FAILURE_MISS,
	AGPMSYSTEMMESSAGE_CODE_TRANSPARENT_FAILURE_FOR_COMBAT,
	AGPMSYSTEMMESSAGE_CODE_FIXED_FAILURE_MAX_COUNT,
	AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_MAX_COUNT,
	AGPMSYSTEMMESSAGE_CODE_SUMMONS_FAILURE_LEVEL_LIMIT_REGION,
	AGPMSYSTEMMESSAGE_CODE_MUTATION_FAILURE_INVALID_TARGET,
	AGPMSYSTEMMESSAGE_CODE_ITEM_CANNOT_BUY,							// �� �� ���� ������.
	AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS,					// ����� �� ���� �����Դϴ�.
	AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_REUSE_INTERVAL,			// ���� �ð��� ���� �ʾҽ��ϴ�.
	AGPMSYSTEMMESSAGE_CODE_JUMP_ITEM_USED,							// ������������ ���Ǿ����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_BRONZE_KEY,				// ���� ���ڸ� ���� ���� ���� �ʿ�
	AGPMSYSTEMMESSAGE_CODE_CANNOT_RIDE_WHILE_COMBAT,				// �����߿��� Ż�� Ż �� ����.
	AGPMSYSTEMMESSAGE_CODE_CANNOT_ATTACKABLE_TARGET,				// ������ �� ���� ����Դϴ�.
	AGPMSYSTEMMESSAGE_CODE_CANNOT_INITIALIZE_SKILL,					// ��ų �ʱ�ȭ �Ұ�. ������ ��ų�� ����.
	AGPMSYSTEMMESSAGE_CODE_ITEM_FOR_GUILDMASTER,					// ��� ������ ���� ������
	AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_PLATINUM_KEY,			// ���� ���ڸ� ���� ���� ���� �ʿ�
	AGPMSYSTEMMESSAGE_CODE_BANK_SLOT_IS_MAX,						// �� �̻� Ȯ���� �� �����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_BANK_ADD_SLOT_SUCCESS,					// â�� Ȯ���� ���������� �Ͽ����ϴ�.
	AGPMSYSTEMMESSAGE_CODE_BANK_MONEY_FIRST,						// ������ ���� Ȯ���ϼ���.
	AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_DEAD_MEMBER		,	// �ɹ� '%s' ������ ������
	AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_IN_SECRET_DUNGEON	,	// �ɹ� '%s' ������ ������
	AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_IN_SIEGE_WAR		,	// �ɹ� '%s' ������ ������
	AGPMSYSTEMMESSAGE_CODE_PARTY_PORTAL_ERROR_LACK_OF_LEVEL		,	// �ɹ� '%s' ������ ������
	AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_WANGBOK_SCROLL			,	// �պ������� ���� ����.
	AGPMSYSTEMMESSAGE_CODE_GO_COMMAND_LACK_OF_LEVEL				,	// ��������� ������.
	AGPMSYSTEMMESSAGE_CODE_INVEN_MONEY_FULL,						// �κ� �ֵ� �ʰ�
	AGPMSYSTEMMESSAGE_CODE_PET_IS_HUNGRY,							// a hungry pet.
	AGPMSYSTEMMESSAGE_CODE_DISABLE_AUCTION,
	AGPMSYSTEMMESSAGE_CODE_NOT_ENOUGH_INVENTORY,					// inventory ����
	AGPMSYSTEMMESSAGE_CODE_DISABLE_EQUIP_ITEM_THIS_REGION,			// �ش� Region������ �ش�ITEM�� �����Ҽ� ����.
	AGPMSYSTEMMESSAGE_CODE_DISABLE_USE_THIS_REGION,					// �ش� Region������ ����� �� �����ϴ�.

	AGPMSYSTEMMESSAGE_CODE_LOTTERYBOX_NEEDS_100BAG_KEY,				// ���� ���ڸ� ���� ���� ���� �ʿ� // �߱������� ����

	AGPMSYSTEMMESSAGE_CODE_GENERAL_STRING					= 400,
	AGPMSYSTEMMESSAGE_CODE_DEBUG_STRING						= 500,
} eAgpmSystemMessageCode;

class AgpdCharacter;

class AgpmSystemMessage 
	:	public ApModule	,
		public SingleTon< AgpmSystemMessage >
{
public:
	AuPacket m_csPacket;

public:
	AgpmSystemMessage( VOID );
	virtual ~AgpmSystemMessage( VOID );

	BOOL	OnAddModule	( VOID );
	BOOL	OnInit		( VOID );
	BOOL	OnIdle2		( UINT32 ulClockCount );
	BOOL	OnDestroy	( VOID );

	// OnReceive
	BOOL	OnReceive				( UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg );
	
	// System Message ó��
	BOOL	ProcessSystemMessage	( INT32 eSysMsgType , INT32 lCode , INT32 lIntParam1 = -1, INT32 lIntParam2 = -1, CHAR* szStringParam1 = NULL, CHAR* szStringParam2 = NULL, AgpdCharacter* pcsCharacter = NULL );
	BOOL	ProcessSystemMessage	( CHAR* szMsg		, DWORD dwColor = 0x00 , INT32 eSysMsgType = AGPMSYSTEMMESSAGE_PACKET_SYSTEM_MESSAGE  );

	// Make Packet
	PVOID	MakeSystemMessagePacket	( INT16* pnPacketLength, INT32 lCode, INT32 lIntParam1, INT32 lIntParam2, CHAR* szStringParam1, CHAR* szStringParam2 );

	// Callback Registration
	BOOL	SetCallbackSystemMessage( ApModuleDefaultCallBack pfCallback , PVOID pClass);

};

#define SystemMessage	AgpmSystemMessage::GetSingleTon()


#endif // _AGPM_SYSTEM_MESSAGE_H_
