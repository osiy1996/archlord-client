/******************************************************************************
Module:  AgsdCharacter.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 03
******************************************************************************/

#if !defined(__AGSDCHARACTER_H__)
#define __AGSDCHARACTER_H__

//#include <time.h>
#include <dplay8.h>
#include "ApBase.h"
#include "AsDefine.h"
//#include "AgpdParty.h"
#include "AgsdAccount.h"
#include "AgpmEventBinding.h"

#ifndef _AREA_CHINA_
#include "AuNProtectAuth.h"
#endif

#define AGSD_CHARACTER_HISTORY_NUMBER				31	// ���� : �� ���ڸ� �ٲٸ� Packet ���ǵ� �ٲ�� �Ѵ�.

#define AGSD_CHARACTER_HISTORY_DEFAULT_DURATION		30000
#define AGSD_CHARACTER_HISTORY_DEFAULT_RANGE		10000.0f

#define AGSM_CHARACTER_MAX_SHOUT_WORD				64

#define	AGSMCHARACTER_MAX_RECEIVED_SKILL_POINT		3

#define	AGSM_CHARACTER_NUM_START_PACKET_BUFFERING	12

const int AGSDCHAR_FLAG_NORMAL			= 1;
const int AGSDCHAR_FLAG_PREDEAD			= 2;
const int AGSDCHAR_FLAG_DEAD			= 3;

const int AGCDCHAR_FLAG_READY_SKILL		= 11;
const int AGCDCHAR_FLAG_DURATION_SKILL	= 12;

const int AGCDCHAR_FLAG_TRADE			= 21;

typedef struct
{
	ApBaseExLock	m_csSource;
	AgpdFactor		*m_pcsUpdateFactor;
	UINT32			m_ulFirstHitTime;
	UINT32			m_ulLastHitTime;
	BOOL			m_bFirstHit;
	FLOAT			m_fAgro;
	INT32			m_lPartyID;
} AgsdCharacterHistoryEntry;

typedef struct
{
	INT32						m_lEntryNum;
	INT32						m_lLastAgroTick;
	//AgsdCharacterHistoryEntry	m_astEntry[AGSD_CHARACTER_HISTORY_NUMBER];
	ApSafeArray<AgsdCharacterHistoryEntry, AGSD_CHARACTER_HISTORY_NUMBER>	m_astEntry;

	//AgsdCharacterHistoryEntry	m_stLastEntry;
	//eAgpmPvPTargetType			m_eLastTargetType;

	INT32						m_lLastEnemyTID;	// ������. -_-; 2006.06.02. steeple
} AgsdCharacterHistory;

// 2005.11.29. steeple
// �� ���� ���������� ����.
const int AGSDCHARACTER_MAX_TARGET_INFO			= 30;
const int AGSDCHARACTER_TARGET_INFO_PERIOD		= 30000;	// 30 ��

typedef struct
{
	INT32			m_lTargetCID;
	UINT32			m_ulLastAttackTime;

	bool operator==(INT32 lCID)
	{
		if(m_lTargetCID == lCID)
			return true;

		return false;
	}
} AgsdCharacterTargetInfo;

typedef struct
{
	typedef ApSafeArray<AgsdCharacterTargetInfo, AGSDCHARACTER_MAX_TARGET_INFO>::iterator iterator;

	ApVector<AgsdCharacterTargetInfo, AGSDCHARACTER_MAX_TARGET_INFO> m_arrTargetInfo;
} AgsdCharacterTargetInfoArray;


typedef enum _AgsmCharacterWaitOperation {
	AGSMCHARACTER_WAIT_OPERATION_SEND_CHAR_INFO		= 0,
	AGSMCHARACTER_WAIT_OPERATION_REQUEST_NEW_CID,
	AGSMCHARACTER_WAIT_OPERATION_ENTER_GAME_WORLD,
	AGSMCHARACTER_WAIT_OPERATION_MAX
} AgsmCharacterWaitOperation;

typedef struct _AgsdCharacterCBWaitOperation {
	CHAR		szAccountName[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];
	INT32		lCID;
	INT32		lTID;
	CHAR		szCharName[AGPACHARACTER_MAX_ID_STRING + 1];
	INT32		lServerID;
	UINT32		dpnidClient;
} AgsdCharacterCBWaitOperation;

typedef enum _eAgpdCharacterBanKeepTime	// AgpdAdmin �� eAgpmAdminBanKeepTime �� ����. (2004.05.18 ����)
{
	AGSDCHAR_BAN_KEEPTIME_ZERO = 0,
	AGSDCHAR_BAN_KEEPTIME_5H,
	AGSDCHAR_BAN_KEEPTIME_1D,
	AGSDCHAR_BAN_KEEPTIME_5D,
	AGSDCHAR_BAN_KEEPTIME_10D,
	AGSDCHAR_BAN_KEEPTIME_30D,
	AGSDCHAR_BAN_KEEPTIME_90D,
	AGSDCHAR_BAN_KEEPTIME_UNLIMIT,
} eAgpdCharacterBanTime;

typedef struct _AgsdCharacterBan	// 2004.03.30. steeple - AgpdCharacter �� �ִ� ���� ���⼭ �� ��ħ
{
	INT32 m_lChatBanStartTime;
	INT8 m_lChatBanKeepTime;

	INT32 m_lCharBanStartTime;
	INT8 m_lCharBanKeepTime;

	INT32 m_lAccBanStartTime;
	INT8 m_lAccBanKeepTime;
} AgsdCharacterBan;

typedef	enum	_AgsdCharacterIdleType {
	AGSDCHAR_IDLE_TYPE_TOTAL				= 0,
	AGSDCHAR_IDLE_TYPE_AI,
	AGSDCHAR_IDLE_TYPE_CHARACTER,
	AGSDCHAR_IDLE_TYPE_DEATH,
	AGSDCHAR_IDLE_TYPE_ITEM,
	AGSDCHAR_IDLE_TYPE_SKILL,
	AGSDCHAR_IDLE_TYPE_PVP,
	AGSDCHAR_IDLE_TYPE_MAX
} AgsdCharacterIdleType;

typedef enum	_AgsdCharacterIdleInterval {
	AGSDCHAR_IDLE_INTERVAL_NEXT_TIME		= 1,
	AGSDCHAR_IDLE_INTERVAL_100_MS			= 100,
	AGSDCHAR_IDLE_INTERVAL_200_MS			= 200,
	AGSDCHAR_IDLE_INTERVAL_HALF_SECOND		= 500,
	AGSDCHAR_IDLE_INTERVAL_ONE_SECOND		= 1000,
	AGSDCHAR_IDLE_INTERVAL_ONE_HALF_SECOND	= 1500,
	AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS		= 2000,
	AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS		= 4000,
	AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS		= 10000,
	AGSDCHAR_IDLE_INTERVAL_THIRTY_SECONDS	= 30000,
	AGSDCHAR_IDLE_INTERVAL_ONE_HOUR			= 3600000,
	AGSDCHAR_IDLE_INTERVAL_MAX
} AgsdCharacterIdleInterval;

typedef enum	_AgsdCharacterReturnStatus {
	AGSDCHAR_RETURN_STATUS_NONE				= 0,
	AGSDCHAR_RETURN_STATUS_WAIT_SAVE,
	AGSDCHAR_RETURN_STATUS_FINISH_SAVE,
} AgsdCharacterReturnStatus;

#define AGSDCHARACTER_BAN_CHAT_SYSTEM_MESSAGE	"ä�� �Ұ��� �����Դϴ�"

const int	AGSMCHARACTER_MAX_WAIT_OPERATION_TIME	= 15000;


struct DetailInfo
{
	bool bRemoveByDead				: 1;
	bool bRemoveByEndPeriod			: 1;
	bool bRemoveByOwnerRemove		: 1;

	DetailInfo()
	{
		clear();
	}

	void clear()
	{
		bRemoveByDead			= 0;	
		bRemoveByEndPeriod		= 0;
		bRemoveByOwnerRemove	= 0;
	}
};

class AgsdCharacter {
public:
	DPNID					m_dpnidCharacter;					// character DPNID

	/*
	INT16					m_bMoveSector;						// ���͸� �̵��ߴ��� ����
	ApWorldSector*			m_pPrevSector;						// �̵� �� ����
	ApWorldSector*			m_pCurrentSector;					// �̵� �� ����
	*/

	BOOL					m_bMoveCell;
	AgsmAOICell*			m_pcsPrevCell;
	AgsmAOICell*			m_pcsCurrentCell;
	AuPOS					m_stPrevPos;

	//INT16					m_nLastLoseExp;						// ���������� ���� Exp (������ ���� �������� Exp ���� ������ ���ȴ�.)

	INT32					m_ulServerID;						// ��ĳ���͸� ó���ϰ� �ִ� ������ �Ƶ�
	CHAR					m_szServerName[AGSM_MAX_SERVER_NAME + 1];

	CHAR					m_szAccountID[AGSMACCOUNT_MAX_ACCOUNT_NAME + 1];		// �� ĳ���� ������ ���� �Ƶ�

	UINT32					m_ulPrevSendMoveTime;				// ���ɸ��� �������� ���������� ���� �ð�
	UINT32					m_ulNextAttackTime;					// ���� ������ �� �� �ִ� �ð�
	UINT32					m_ulNextSkillTime;					// ���� ��ų�� �� �� �ִ� �ð�

	UINT32					m_ulRoundTripLatencyMS;				// �� ĳ������ ������ ���� Latency

	BOOL					m_bDestroyWhenDie;					// �׾�����, �ɸ��͸� ���ٰŸ� TRUE
	BOOL					m_bResurrectWhenDie;				// �׾�����, ĳ���͸� ��Ȱ��ų�� ����
	INT32					m_lResurrectHP;						// ��Ȱ�Ҷ� ä���� HP��

	UINT32					m_ulHistoryDuration;				// History�� �����ִ� �ð� (�Ѿ�� ��������.)
	FLOAT					m_fHistoryRange;					// History�� �����ִ� �Ÿ� (����� ��������.)

	AgsdCharacterHistory	m_stHistory;						// �����ο� ���� �����丮
	AgsdCharacterHistory	m_stHistoryParty;					// ��Ƽ�� ���� �����丮

	UINT64					m_ullDBID;							// ĳ������ DB ID���.

	CHAR					m_szLastShoutWord[AGSM_CHARACTER_MAX_SHOUT_WORD + 1];

	UINT32					m_ulLastUpdateMukzaPointTime;		// ���� ����Ʈ�� ����� ������ �ð�
	UINT32					m_ulRemainUpdateMukzaPointTime;		// ���� ����Ʈ�� ����� ���� �ð�

	INT32					m_lReceivedSkillPoint;				// �� level���� ���� ��ų ����Ʈ

	UINT32					m_ulLastUpdateActionStatusTime;		// ���������� action status�� ����� �ð�

	BOOL					m_bWaitOperationBeforeRemove;
	UINT32					m_ulMaxWaitTime;					// �󸶳� ��ٸ���..
	//BOOL					m_bWaitOperation[AGSMCHARACTER_WAIT_OPERATION_MAX];
	ApSafeArray<BOOL, AGSMCHARACTER_WAIT_OPERATION_MAX>		m_bWaitOperation;

	UINT32					m_ulReleaseStunStatusTime;			// ������ Ǯ�� �ð�
	UINT32					m_ulReleaseFreezeStatusTime;		// Freeze �� Ǯ�� �ð�
	UINT32					m_ulReleaseSlowStatusTime;			// Slow �� Ǯ�� �ð�
	UINT32					m_ulReleaseInvincibleStatusTime;	// ���� Ǯ�� �ð�
	UINT32					m_ulReleaseAttributeInvincibleStatusTime;		// �Ӽ� ���� Ǯ�� �ð�
	UINT32					m_ulReleaseNotAddAgroStatusTime;	// Not Add Agro Ǯ�� �ð�
	UINT32					m_ulReleaseHideAgroStatusTime;		// Hide Agro Ǯ�� �ð�
	UINT32					m_ulReleaseStunProtectStatusTime;	// Stun Protect Ǯ�� �ð�
	UINT32					m_ulReleaseTransparentTime;			// ���� Ǯ�� �ð�
	UINT32					m_ulReleasePvPInvincibleTime;		// PvP �� ���� Ǯ�� �ð�
	UINT32					m_ulReleaseHaltStatusTime;			// Halt Ǯ�� �ð�
	UINT32					m_ulReleaseHalfTransparentTime;		// Half Transparent Release Time
	UINT32					m_ulReleaseNormalATKInvincibleTime;	// Normal Attack Invincible Release Time
	UINT32					m_ulReleaseSkillATKInvincibleTime;	// Skill Attack Invincible Release Time
	UINT32					m_ulReleaseDisableSkillTime;		// Disable Skill Release Time
	UINT32					m_ulReleaseDisableNormalATKTime;	// Disable Normal Attack Release Time
	UINT32					m_ulReleaseSleepTime;				// Sleep Release Time
	UINT32					m_ulReleaseDisArmamentTime;			// DisArmament Release Time
	UINT32					m_ulReleaseDisableChatting;			// DisableChatting Release Time
	UINT32					m_ulReleaseHoldTime;				// Hold Release Time
	UINT32					m_ulReleaseConfusionTime;			// Confusion Release Time
	UINT32					m_ulReleaseFearTime;				// Fear Release Time
	UINT32					m_ulReleaseDiseaseTime;				// Disease Release Time
	UINT32					m_ulReleaseBerserkTime;				// Berserk Release Time
	UINT32					m_ulReleaseShrinkTime;				// Shrink Release Time

	InvincibleInfo			m_stInvincibleInfo;					// �׸� ���� Ȯ�� ����.

	INT32					m_lSlowStatusValue;					// Slow ����ġ (% ������ 0~100)

	UINT32					m_ulNextBackupCharacterDataTime;	// ���� ����Ÿ�� ����� �ð�

	UINT32					m_ulReserveTimeForDestroy;

	AgsdCharacterBan m_stBan;	// 2004.03.30. steeple

	BOOL					m_bIsRecvAllDataFromLoginSvr;
	INT32					m_nRecvLoginServer;

	// �α� ���� ������̴�.
	INT32					m_bDisconnectByDuplicateUser;

	UINT32					m_ulStartClockCount;
	BOOL					m_bIsPartyPlay;

	UINT32					m_ulPartyPlayTime;			// ������ �α׸� �� ������ ����ġ
	UINT32					m_ulSoloPlayTime;
	UINT32					m_ulKillMonCount;
	UINT32					m_ulKillPCCount;
	UINT32					m_ulDeadByMonCount;
	UINT32					m_ulDeadByPCCount;
	
	BOOL					m_bIsNewCID;

	INT32					m_lOldCID;

	UINT32					m_ulTimeOutTransform;				// ������ ��� ������ �ð� ����
	//UINT32					m_ulTimeOutPaid;					// ������������ Ȯ�� �ϴ� �ð�

	BOOL					m_bIsCreatedChar;

//	UINT32						m_ulLastIdleProcessTime[AGSDCHAR_IDLE_TYPE_MAX];
//	AgsdCharacterIdleInterval	m_eIdleProcessInterval[AGSDCHAR_IDLE_TYPE_MAX];

	ApSafeArray<UINT32, AGSDCHAR_IDLE_TYPE_MAX>						m_ulLastIdleProcessTime;
	ApSafeArray<AgsdCharacterIdleInterval, AGSDCHAR_IDLE_TYPE_MAX>	m_eIdleProcessInterval;

	INT32					m_lAuthKey;
	BOOL					m_bIsValidAuthKey;

	ApSafeArray<CHAR, 16>	m_strIPAddress;

	BOOL					m_bIsNotLogout;
	BOOL					m_bIsResurrectingNow;

	BOOL					m_bIsSuperMan;
	BOOL					m_bIsMaxDefence;

	INT64					m_llPrevUpdateMoney;

	AuPOS					m_stComePosition;
	
	UINT32					m_ulPlayLogCount;

	BOOL					m_bRemoveByPeriod;		// 2005.10.10. steeple. ��ȯ�� �ð� üũ���� ���� �� ������ �Ҹ��� ���� ����.

	DetailInfo				m_DetailInfo;

	AgsdCharacterTargetInfoArray m_TargetInfoArray;
	
	BOOL					m_bIsAuctionBlock;		// 2006.02.07. laki. ���� ���� ���ŷ.
	BOOL					m_bIsGachaBlock;		// 2006.02.07. laki. ���� ���� ���ŷ.
	BOOL					m_bIsTeleportBlock;

	AgsdCharacterReturnStatus	m_eReturnStatus;

	UINT32					m_ulConnectedTimeStamp;	// 2006.03.09. steeple �������� ���� TimeStamp
	INT32					m_lLoginLevel;			// 2007.11.06. steeple �α��� ���� ���� Level

	BOOL					m_bNeedReinitialize;

	INT32					m_lOriginalRegionIndex;

	BOOL					m_bIsAutoPickupItem;
	
	stOptionSkillData		m_stOptionSkillData;	// 2006.12.12. steeple. ������ �ɼǿ� ���ؼ� ����Ǵ� ��ų������.
	BOOL					m_bLevelUpForced;		// Exp ����� �ƴ� ���� �������� ������ ����� ���
	INT32					m_lLevelBeforeForced;	// ���� ���� ������ ����.
	INT32					m_lMemberBillingNum;		// �Ϻ�: ����� ���� ��ȣ
	char					m_szMemberBillingNum[32];	// �Ϻ�: ����� ���� ��ȣ
	CHAR					m_szResurrectionCaster[AGPDCHARACTER_MAX_ID_LENGTH + 1];

	CHAR					m_szCRMCode[AGPDLOG_MAX_PCROOM_CRM_CODE + 1];	// 2007.11.08. steeple PCRoom CRMCode
	CHAR					m_szGrade[AGPDLOG_MAX_GRADE + 1];				// 2007.11.12. steeple PCRoom Grade

	struct	GachaDelayItemUpdateData
	{
		enum	TYPE
		{
			NONE,
			NORMAL,
			STACKABLE
		};
		
		TYPE		eType;
		AgpdItem	*pcsItem;

		GachaDelayItemUpdateData():eType(NONE) , pcsItem(NULL){}
		GachaDelayItemUpdateData(TYPE _eType , AgpdItem * _pItem):eType(_eType) , pcsItem(_pItem){}
	};

	vector< GachaDelayItemUpdateData >	vecGachaItemQueue;

	void	SetGachaBlock( BOOL bBlock ) { m_bIsGachaBlock = bBlock; }
	BOOL	GetGachaBlock() { return m_bIsGachaBlock; }

	BOOL	AddGachaItemUpdate( GachaDelayItemUpdateData::TYPE eType , AgpdItem * pcsItem )
	{
		if( GetGachaBlock() )	vecGachaItemQueue.push_back( GachaDelayItemUpdateData( eType , pcsItem ) );
		return TRUE;
	}

	void	ClearGachaItemUpdate() { vecGachaItemQueue.clear(); }

#ifdef _USE_NPROTECT_GAMEGUARD_
	CCSAuth2*	csa;
	CTime		m_GGLastQueryTime;
#endif
};

#endif //__AGSDCHARACTER_H__