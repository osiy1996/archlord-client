#ifndef	_AGPDCHARACTER_H_
#define _AGPDCHARACTER_H_

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpdTitle.h"
#include "AgpdTitleQuest.h"

#define	AGPDCHARACTER_MAX_BLOCK_INFO		12
#define AGPDCHARACTER_MAX_TARGET			99
#define	AGPDCHARACTER_MAX_ID_LENGTH			48		// ���ӿ��� ���Ǵ� ĳ���� �̸� ���� -> NPC �̸� ���� ������ �þ��
const INT32 AGPDCHARACTER_NAME_LENGTH		= 32;	// DB���� ���Ǵ� ĳ���� �̸� ����

#define	AGPDCHARACTER_MAX_INVEN_MONEY		1000000000
#define	AGPDCHARACTER_MAX_BANK_MONEY		1000000000
#define	AGPDCHARACTER_MAX_CASH				1000000000000000000

typedef ApString<AGPDCHARACTER_NAME_LENGTH> ApStrCharacterName;
/*
typedef enum _AgpdCharType {
	AGPDCHAR_TYPE_NONE						= (-1),
	AGPDCHAR_TYPE_PC,
	AGPDCHAR_TYPE_NPC,
	AGPDCHAR_TYPE_MONSTER,
	AGPDCHAR_TYPE_CREATURE
} AgpdCharType;
*/

const int	AGPMCHAR_TYPE_NONE						= 0x00000000;
const int	AGPMCHAR_TYPE_PC						= 0x00000001;
const int	AGPMCHAR_TYPE_NPC						= 0x00000002;
const int	AGPMCHAR_TYPE_MONSTER					= 0x00000004;
const int	AGPMCHAR_TYPE_CREATURE					= 0x00000008;
const int	AGPMCHAR_TYPE_GUARD						= 0x00000010;
const int	AGPMCHAR_TYPE_GM						= 0x00000020;
const int	AGPMCHAR_TYPE_ATTACKABLE				= 0x00010000;
const int	AGPMCHAR_TYPE_TARGETABLE				= 0x00020000;
const int	AGPMCHAR_TYPE_MOVABLENPC				= 0x00040000;	// ������ (2005-06-22 ���� 4:25:55) : ������ �ٴϴ� NPC �� üũ.
const int	AGPMCHAR_TYPE_POLYMORPH					= 0x00080000;	//. 2006. 6. 21. Nonstopdj. ���������� type.
const int	AGPMCHAR_TYPE_TRAP						= 0x00100000;	//. 2006. 12. 81. Magoja , ���̰� �����Ŵ� ������ ����.
const int	AGPMCHAR_TYPE_SUMMON					= 0x00200000;	//. 2008. 11. 25. iluvs. ���Դٰ� �ٷ� ������� ȯ�� �迭 ĳ����


typedef enum AgpdCharStatus {
	AGPDCHAR_STATUS_LOGOUT					= 0,	// �̳��� ���ӵ� ���ѳ��̴�.
	AGPDCHAR_STATUS_IN_LOGIN_PROCESS,				// �α��� ���� ���ӿ� ������ ����
	AGPDCHAR_STATUS_IN_GAME_WORLD,					// ���ӿ� ���ͼ� �볪�� �ϰ��ִ�.
	AGPDCHAR_STATUS_RETURN_TO_LOGIN_SERVER,
} AgpdCharStatus;

typedef enum AgpdCharActionStatus {
	AGPDCHAR_STATUS_NORMAL					= 0,	// �� ��Ƽ� ���� ����.
	AGPDCHAR_STATUS_PREDEAD,						// �ϴ��� �׾��µ�... ��Ÿ���� ���� �����Ǳ� ������ �� ���д�.
	AGPDCHAR_STATUS_DEAD,							// ������ �׾���

	AGPDCHAR_STATUS_NOT_ACTION,
	AGPDCHAR_STATUS_MOVE,
	AGPDCHAR_STATUS_ATTACK,
	AGPDCHAR_STATUS_READY_SKILL,			// ��ų�� �������� ������ �ð��� ������̴�.
	AGPDCHAR_STATUS_TRADE,					// �ŷ� ���̴�.
	AGPDCHAR_STATUS_PARTY,					// ��Ƽ�� �ΰ� �ִ� ���̴�. �ƽ�~~~!!!!
	//AGPDCHAR_STATUS_STUN					// �����ߴ�.
} AgpdCharActionStatus;

// 2004.08.25. steeple
// Special Status �� ��ĥ �� �����Ƿ� Bit ������ �����ϰ� �Ѵ�.
// 2007.05.09. steeple
// 64��Ʈ�� ����.
const UINT64 AGPDCHAR_SPECIAL_STATUS_NORMAL						= 0x0000000000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_STUN						= 0x0000000000000001;
const UINT64 AGPDCHAR_SPECIAL_STATUS_FREEZE						= 0x0000000000000002;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLOW						= 0x0000000000000004;
const UINT64 AGPDCHAR_SPECIAL_STATUS_INVINCIBLE					= 0x0000000000000008;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE		= 0x0000000000000010;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO				= 0x0000000000000020;	// ������ ���Ѵ�.
const UINT64 AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO					= 0x0000000000000040;	// Ÿ������ �ν����� ����.
const UINT64 AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT				= 0x0000000000000080;	// ���Ϲ�����.
const UINT64 AGPDCHAR_SPECIAL_STATUS_PVP_INVINCIBLE				= 0x0000000000000100;
const UINT64 AGPDCHAR_SPECIAL_STATUS_TRANSPARENT				= 0x0000000000000200;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DONT_MOVE					= 0x0000000000000400;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SUMMONER					= 0x0000000000000800;
const UINT64 AGPDCHAR_SPECIAL_STATUS_TAME						= 0x0000000000001000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HALT						= 0x0000000000002000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_WAS_TAMED					= 0x0000000000004000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_COUNSEL					= 0x0000000000008000;	// ����� 2005.10.30. steeple
const UINT64 AGPDCHAR_SPECIAL_STATUS_EVENT_GIFTBOX				= 0x0000000000010000;	// �������� �̺�Ʈ
const UINT64 AGPDCHAR_SPECIAL_STATUS_ADMIN_SPAWNED_CHAR			= 0x0000000000020000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE					= 0x0000000000040000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_USE						= 0x0000000000080000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_REPAIR						= 0x0000000000100000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT			= 0x0000000000200000;	// 2006.09.21. steeple
const UINT64 AGPDCHAR_SPECIAL_STATUS_HUMAN_CASTLE_OWNER			= 0x0000000000400000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ORC_CASTLE_OWNER			= 0x0000000001000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_MOONELF_CASTLE_OWNER		= 0x0000000002000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_ARCHLORD_CASTLE_OWNER		= 0x0000000004000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DROP_RATE_100				= 0x0000000008000000;	// ������ 100% drop
const UINT64 AGPDCHAR_SPECIAL_STATUS_RIDE_ARCADIA				= 0x0000000010000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE		= 0x0000000020000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE		= 0x0000000040000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL				= 0x0000000080000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK			= 0x0000000100000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLEEP						= 0x0000000200000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SLOW_INVINCIBLE			= 0x0000000400000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_GO							= 0x0000000800000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT					= 0x0000001000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISARMAMENT				= 0x0000002000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DRAGONSCION_CASTLE_OWNER	= 0x0000004000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISABLE_CHATTING			= 0x0000008000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_HOLD						= 0x0000010000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_CONFUSION					= 0x0000020000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_FEAR						= 0x0000040000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_BERSERK					= 0x0000080000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_SHRINK						= 0x0000100000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_DISEASE					= 0x0000200000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_VOLUNTEER					= 0x0000400000000000;
const UINT64 AGPDCHAR_SPECIAL_STATUS_NONE						= 0x8000000000000000;

// 2007.06.29. steeple
// �� �׸� ���� Ȯ��.
struct InvincibleInfo
{
	INT32 lNormalATKProbability;
	INT32 lSkillATKProbability;
	INT32 lStunProbability;
	INT32 lSlowProbability;
	INT32 lAttrProbability;

	InvincibleInfo()
	{
		clear();
	}

	void clear()
	{
		lNormalATKProbability	= 0;
		lSkillATKProbability	= 0;
		lStunProbability		= 0;
		lSlowProbability		= 0;
		lAttrProbability		= 0;
	}
};

typedef enum _AgpdCharacterCriminalStatus {
	AGPDCHAR_CRIMINAL_STATUS_INNOCENT		= 0,
	AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED,
} AgpdCharacterCriminalStatus;

typedef enum AgpdCharacterActionType {
	AGPDCHAR_ACTION_TYPE_NONE				= 0,
	AGPDCHAR_ACTION_TYPE_ATTACK,
	AGPDCHAR_ACTION_TYPE_ATTACK_MISS,
	AGPDCHAR_ACTION_TYPE_SKILL,
	AGPDCHAR_ACTION_TYPE_PICKUP_ITEM,
	AGPDCHAR_ACTION_TYPE_EVENT_TELEPORT,
	AGPDCHAR_ACTION_TYPE_EVENT_NPC_TRADE,
	AGPDCHAR_ACTION_TYPE_EVENT_MASTERY_SPECIALIZE,
	AGPDCHAR_ACTION_TYPE_EVENT_BANK,
	AGPDCHAR_ACTION_TYPE_EVENT_ITEMCONVERT,
	AGPDCHAR_ACTION_TYPE_EVENT_GUILD,
	AGPDCHAR_ACTION_TYPE_EVENT_PRODUCT,
	AGPDCHAR_ACTION_TYPE_EVENT_NPC_DIALOG,
	AGPDCHAR_ACTION_TYPE_EVENT_SKILL_MASTER,
	AGPDCHAR_ACTION_TYPE_EVENT_REFINERY,
	AGPDCHAR_ACTION_TYPE_EVENT_QUEST,
	AGPDCHAR_ACTION_TYPE_MOVE,
	AGPDCHAR_ACTION_TYPE_PRODUCT_SKILL,
	AGPDCHAR_ACTION_TYPE_EVENT_CHARCUSTOMIZE,
	AGPDCHAR_ACTION_TYPE_EVENT_ITEMREPAIR,
	AGPDCHAR_ACTION_TYPE_EVENT_REMISSION,
	AGPDCHAR_ACTION_TYPE_EVENT_WANTEDCRIMINAL,
	AGPDCHAR_ACTION_TYPE_EVENT_SIEGE_WAR,
	AGPDCHAR_ACTION_TYPE_EVENT_TAX,
	AGPDCHAR_ACTION_TYPE_USE_SIEGEWAR_ATTACK_OBJECT,
	AGPDCHAR_ACTION_TYPE_CARVE_A_SEAL,
	AGPDCHAR_ACTION_TYPE_EVENT_GUILD_WAREHOUSE,
	AGPDCHAR_ACTION_TYPE_USE_EFFECT,
	AGPDCHAR_ACTION_TYPE_EVENT_ARCHLORD,
	AGPDCHAR_ACTION_TYPE_EVENT_GAMBLE,
	AGPDCHAR_ACTION_TYPE_EVENT_GACHA,
	AGPDCHAR_MAX_ACTION_TYPE
} AgpdCharacterActionType;

typedef enum AgpdCharacterActionResultType {
	AGPDCHAR_ACTION_RESULT_TYPE_NONE		= 0,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT,
	AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_REFLECT,
	AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_ABSORB,
	AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK,
	AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE,
	AGPDCHAR_ACTION_RESULT_TYPE_CAST_SKILL,
	AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_CONVERT_DAMAGE,
	AGPDCHAR_ACTION_RESULT_TYPE_CHARGE,
} AgpdCharacterActionResultType;

typedef enum AgpdCharacterActionBlockType {
	AGPDCHAR_ACTION_BLOCK_TYPE_ALL			= 0x00000001,
	AGPDCHAR_ACTION_BLOCK_TYPE_ATTACK		= 0x00000002,
	AGPDCHAR_ACTION_BLOCK_TYPE_SKILL		= 0x00000004,
} AgpdCharacterActinoBlockType;

typedef enum AgpdCharacterAgroType
{
	AGPDCHAR_AGRO_TYPE_NORMAL = 0x00000000,
	AGPDCHAR_AGRO_TYPE_NOT_ADD = 0x00000001,
	AGPDCHAR_AGRO_TYPE_HIDE = 0x00000002,
} AgpdCharacterAgroType;

typedef enum _AgpdCharacterSocialType
{
	AGPDCHAR_SOCIAL_TYPE_GREETING	= 0,	// �λ�
	AGPDCHAR_SOCIAL_TYPE_CELEBRATION,		// ����
	AGPDCHAR_SOCIAL_TYPE_GRATITUDE,			// ����
	AGPDCHAR_SOCIAL_TYPE_ENCOURAGEMENT,		// �ݷ�
	AGPDCHAR_SOCIAL_TYPE_DISREGARD,			// ����
	AGPDCHAR_SOCIAL_TYPE_DANCING,			// ���߱�
	AGPDCHAR_SOCIAL_TYPE_DOZINESS,			// ����
	AGPDCHAR_SOCIAL_TYPE_STRETCH,			// ������
	AGPDCHAR_SOCIAL_TYPE_LAUGH,				// ����
	AGPDCHAR_SOCIAL_TYPE_WEEPING,			// ���
	AGPDCHAR_SOCIAL_TYPE_RAGE,				// �г�
	AGPDCHAR_SOCIAL_TYPE_POUT,				// �����
	AGPDCHAR_SOCIAL_TYPE_APOLOGY,			// ���
	AGPDCHAR_SOCIAL_TYPE_TOAST,				// �ǹ�
	AGPDCHAR_SOCIAL_TYPE_CHEER,				// ȯȣ
	AGPDCHAR_SOCIAL_TYPE_RUSH,				// ����
	AGPDCHAR_SOCIAL_TYPE_SIT,				// �ɱ�
	AGPDCHAR_SOCIAL_TYPE_SPECIAL1,			// ��������~

	AGPDCHAR_SOCIAL_TYPE_SELECT1,			// ����(�Ǽ�)
	AGPDCHAR_SOCIAL_TYPE_SELECT2,			// ����(�Ѽհ�,Ȱ,STAFF)
	AGPDCHAR_SOCIAL_TYPE_SELECT3,			// ����(�Ѽյб�,����,!STAFF)
	AGPDCHAR_SOCIAL_TYPE_SELECT4,			// ����(�μհ�)
	AGPDCHAR_SOCIAL_TYPE_SELECT5,			// ����(�μյб�)
	AGPDCHAR_SOCIAL_TYPE_SELECT6,			// ����(���Ϸ�)
	AGPDCHAR_SOCIAL_TYPE_SELECT7,			// ����(���̾���, īŸ����, íũ�� �߰�.)
	AGPDCHAR_SOCIAL_TYPE_SELECT8,			// ����(�Ѽ�����)
	AGPDCHAR_SOCIAL_TYPE_SELECT9,			// ����(�μ�����)
	AGPDCHAR_SOCIAL_TYPE_SELECT10,			// ����(�μ�ī��)
	
	// �α� ����â����... �ٸ� ĳ���� ���õǸ� ���ڸ��� ���ư��� �Ҽȿ��� �ߵ�
	AGPDCHAR_SOCIAL_TYPE_SELECT1_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT2_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT3_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT4_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT5_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT6_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT7_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT8_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT9_BACK,
	AGPDCHAR_SOCIAL_TYPE_SELECT10_BACK,

	AGPDCHAR_SOCIAL_TYPE_GM_GREETING	,	// �λ�
	AGPDCHAR_SOCIAL_TYPE_GM_CELEBRATION	,	// ����
	AGPDCHAR_SOCIAL_TYPE_GM_DANCING		,	// ��
	AGPDCHAR_SOCIAL_TYPE_GM_WEEPING		,	// ���
	AGPDCHAR_SOCIAL_TYPE_GM_TOAST		,	// �ǹ�
	AGPDCHAR_SOCIAL_TYPE_GM_CHEER		,	// ȯȣ
	AGPDCHAR_SOCIAL_TYPE_GM_DEEPBOW		,	// ��
	AGPDCHAR_SOCIAL_TYPE_GM_HI			,	// �ȳ��ϼ���
	AGPDCHAR_SOCIAL_TYPE_GM_WAIT		,	// ��ø� ��ٷ��ּ���
	AGPDCHAR_SOCIAL_TYPE_GM_HAPPY		,	// ��ũ�ε�� �Բ� ��ſ� �Ϸ� �Ǽ���

	AGPDCHAR_SOCIAL_TYPE_MAX
} AgpdCharacterSocialType;


typedef enum _AgpdCharacterOptionFlag			//	2005.05.31. By SungHoon	�ź� ��� ����List
{
	AGPDCHAR_OPTION_REFUSE_TRADE			=	0x00000001,		// �ŷ� �ź�
	AGPDCHAR_OPTION_REFUSE_PARTY_IN			=	0x00000002,		// ��Ƽ �ʴ� �ź�
	AGPDCHAR_OPTION_REFUSE_GUILD_BATTLE		=	0x00000004,		// ����� �ź�
	AGPDCHAR_OPTION_REFUSE_BATTLE			=	0x00000008,		// ���� �ź�
	AGPDCHAR_OPTION_REFUSE_GUILD_IN			=	0x00000010,		// ����ʴ� �ź�
	AGPDCHAR_OPTION_REFUSE_GUILD_RELATION	=	0x00000040,		// ����-���� �ź�
	AGPDCHAR_OPTION_REFUSE_BUDDY			=	0x00000080,		// ģ�� �ʴ� �ź�
	AGPDCHAR_OPTION_AUTO_PICKUP_ONOFF		=	0x00000100,		// ���κ��丮�� ���� AutoPickUP On/OFF
} AgpdCharacterOptionFlag;

typedef enum _eAgpmCharacterTamableType			// 2005.10.04. steeple
{
	AGPDCHAR_TAMABLE_TYPE_NONE				= 0,
	AGPDCHAR_TAMABLE_TYPE_BY_FORMULA,
} AgpdCharacterTamableType;

typedef enum _eAgpmCharacterRangeType
{
	AGPDCHAR_RANGE_TYPE_MELEE = 0,
	AGPDCHAR_RANGE_TYPE_RANGE,
} AgpdCharacterRangeType;

// 2005.12.14. steeple
// Skill �ʿ��� ó���ϴ� ���� �̸��� �Ű��. ���� ���� ���� �̤�
typedef enum
{
	AGPDCHAR_ADDITIONAL_EFFECT_NONE						= 0x00000000,
	AGPDCHAR_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION		= 0x00000001,
	AGPDCHAR_ADDITIONAL_EFFECT_TELEPORT					= 0x00000002,
	AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_MP				= 0x00000004,	// �������� �� ������ ���
	AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP				= 0x00000008,	// �������� �� ������ ���
	AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_MP				= 0x00000010,	// ������� �� ������ ���
	AGPDCHAR_ADDITIONAL_EFFECT_CONVERT_HP				= 0x00000020,	// ������� �� ������ ���
	AGPDCHAR_ADDITIONAL_EFFECT_RELEASE_TARGET			= 0x00000040,	// �ش� ĳ���͸� Ÿ���� PC �鿡�Լ� Ÿ�� ������ ������ ��Ų��.
	AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE				= 0x00000080,	// ���� ���� ����Ʈ
	AGPDCHAR_ADDITIONAL_EFFECT_FIRECRACKER				= 0x00000100,	// ����
	AGPDCHAR_ADDITIONAL_EFFECT_CHARISMA_POINT			= 0x00000200,	// ī������ ����Ʈ ȹ��
} AgpdCharacterAdditionalEffect;

enum eDisturbAction
{
	AGPDCHAR_DISTURB_ACTION_NONE						= 0x00,
	AGPDCHAR_DISTURB_ACTION_ATTACK						= 0x01,
	AGPDCHAR_DISTURB_ACTION_MOVE						= 0x02,
	AGPDCHAR_DISTURB_ACTION_USE_ITEM					= 0x04,
	AGPDCHAR_DISTURB_ACTION_SKILL						= 0x08
};

// bit flag options
//////////////////////////////////////////////////////////
union EventStatusFlag
{
	UINT16	BitFlag;
	struct  
	{
		UINT8 uBitFlag;
		UINT8 NationFlag;
	};
};

enum eEventFlagNationFlag
{
	AGPDCHAR_EVENTFLAG_NONE = 1,
	AGPDCHAR_EVENTFLAG_BLACKRIBBON,
	AGPDCHAR_EVENTFLAG_KOREA, 
};

const int	AGPM_CHAR_BIT_FLAG_IS_PC_ROOM			= 0x01;
const int	AGPM_CHAR_BIT_FLAG_EXPEDITION			= 0x02;

const int	AGPMCHAR_MAX_UNION				= 20;
const int	AGPMCHAR_MURDERER_LEVEL0_POINT	= 0;
const int	AGPMCHAR_MURDERER_LEVEL1_POINT	= 40;
const int	AGPMCHAR_MURDERER_LEVEL2_POINT	= 60;
const int	AGPMCHAR_MURDERER_LEVEL3_POINT	= 100;

#if defined( _AREA_GLOBAL_ )
const int	AGPMCHAR_MAX_LEVEL				= 121;	// ������� ����(�ְ��� 120���� ���)
#elif defined( _AREA_TAIWAN_ )
const int	AGPMCHAR_MAX_LEVEL				= 100;
#else
const int	AGPMCHAR_MAX_LEVEL				= 121;
#endif

const int	AGPMCHAR_LEVELUP_SKILL_POINT	= 3;

const int	AGPMCHAR_DEFAULT_MOVE_SPEED		= 4000;

const int	AGPMCHAR_MAX_ATTACKER_LIST		= 20;

const int	AGPMCHAR_MAX_TOWN_NAME			= 64;

const int	AGPMCHAR_MAX_COMBAT_MODE_TIME	= 10000;					// ���������� �ӹ��� ���Ŀ� 10�ʵ����� �ӹ��� ����̴�.

class   AgpdCharacterAgroDecreaseData
{
public:
	INT32			m_lSlowPercent;
	INT32			m_lSlowTime;
	INT32			m_lFastPercent;
	INT32			m_lFastTime;
};

class	AgpdCharacterTemplate : public ApBase
{
public:
	//CHAR					m_szTName[AGPACHARACTERT_MAX_TEMPLATE_NAME + 1];
	CHAR					m_szTName[AGPACHARACTER_MAX_ID_STRING + 1];

	INT16					m_nBlockInfo;
//	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astBlockInfo; Ȯ���ɶ����� ��� �ּ�(BOB, 181004)
	AuBLOCKING				m_astBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];

	AgpdFactor				m_csFactor;				// ���� Template Factor

	UINT32					m_ulCharType;

	//BOOL					m_bIsMonster;			// �̳��� �������� ���� (TRUE : ����, FALSE : �÷��̾�)

	ApSafeArray<AgpdFactor, AGPMCHAR_MAX_LEVEL + 1>					m_csLevelFactor;
	//AgpdFactor				m_csLevelFactor[AGPMCHAR_MAX_LEVEL + 1];	// ���� �� Ŭ���� status��... ������ �ö󰡸� �̳����� ĳ���� ������ �����.

	//������ �ð��� ��׷� ��������Ʈ. ���͸��� �����ϹǷ� AI���ٴ� ĳ���������� �ٿ���. -Ashulam-
	AgpdCharacterAgroDecreaseData	m_csDecreaseData;

	//������� ����.
	INT32					m_lDropTID;
	INT32					m_lGhelldMin;
	INT32					m_lGhelldMax;

	UINT32					m_ulIDColor;

	AgpdFactorAttributeType	m_eAttributeType;

	//customize
	INT32					m_lFaceNum;
	INT32					m_lHairNum;

	AgpdCharacterTamableType	m_eTamableType;		// 2005.10.04. steeple
	INT32						m_lLandAttachType;

	AgpdCharacterRangeType	m_eRangeType;		// 2007.12.05. steeple
	BOOL IsRangeCharacter() { return m_eRangeType == AGPDCHAR_RANGE_TYPE_RANGE; }

	//@{ 2006/08/21 burumal
	FLOAT					m_fSiegeWarCollBoxWidth		;
	FLOAT					m_fSiegeWarCollBoxHeight	;
	FLOAT					m_fSiegeWarCollSphereRadius	;
	FLOAT					m_fSiegeWarCollObjOffsetX	;	//���� �ɼ�.
	FLOAT					m_fSiegeWarCollObjOffsetZ	;

	//@}

	//@{ 2006/09/08 burumal
	BOOL					m_bSelfDestructionAttackType;
	//@}

	INT32					m_lStaminaPoint;
	INT32					m_lPetType;
	INT32					m_lStartStaminaPoint;

	BOOL IsBlocking()		{ return ( m_fSiegeWarCollBoxWidth > 0.0f || m_fSiegeWarCollBoxHeight > 0.0f || m_fSiegeWarCollSphereRadius > 0.0f ) ? TRUE : FALSE; }
	BOOL IsBoxCollision()	{ return ( m_fSiegeWarCollBoxWidth > 0.0f || m_fSiegeWarCollBoxHeight > 0.0f ) ? TRUE : FALSE; }
	CHAR * GetName()		{ return m_szTName; }
};

class ApdCharacter : public ApBase
{
public:
	INT32					m_lTID1;
	AgpdCharacterTemplate *	m_pcsCharacterTemplate;
};

typedef struct
{
	AgpdCharacterActionType	m_eActionType;
	BOOL					m_bForceAction;			// �Ÿ� üũ�� ���� ������ ����
	ApBaseExLock			m_csTargetBase;
	AuPOS					m_stTargetPos;

	ApSafeArray<INT_PTR, 5>	m_lUserData;
	//INT32					m_lUserData[5];

	UINT32					m_ulAdditionalEffect;
} AgpdCharacterAction;

typedef struct	// 2005.12.02. steeple
{
	INT32					m_lBonusExpRate;
	INT32					m_lBonusMoneyRate;
	INT32					m_lBonusDropRate;
	INT32					m_lBonusDropRate2;
	INT32					m_lBonusCharismaRate;

	void init()
	{
		m_lBonusExpRate = 0;
		m_lBonusMoneyRate = 0;
		m_lBonusDropRate = 0;
		m_lBonusDropRate2 = 0;
		m_lBonusCharismaRate = 0;
	}
} AgpdCharacterGameBonusByCash;

enum MOVE_DIRECTION
{
	MD_NODIRECTION			,	// ���� ������ ���� ���� ����.	
	MD_FORWARD				,
	MD_FORWARDRIGHT			,
	MD_RIGHT				,
	MD_BACKWARDRIGHT		,
	MD_BACKWARD				,
	MD_BACKWARDLEFT			,
	MD_LEFT					,
	MD_FORWARDLEFT			
};

// �߱� �ߵ� ���� �ý��ۿ�
enum ADDICT_STATUS
{
	AS_GREEN = 0,	// 0~3�ð� ���� �÷����� ����
	AS_YELLOW,		// 3~5�ð� ����
	AS_RED,			// 5�ð� �̻�
};

class AgpdCharacter : public ApdCharacter
{
public:
	INT32						m_nDimension;			// �� �ε���.
	//INT32						m_lTID2;
//	CHAR						m_szID[AGPACHARACTER_MAX_ID_STRING + 1];
	CHAR						m_szID[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	//CHAR						m_szAccountID[AGPACHARACTER_MAX_ID_STRING];
	BOOL						m_bMove;				// �����̰� �ִ���
	BOOL						m_bMoveFollow;			// Ÿ���� ���󰣴�.
	BOOL						m_bMoveFast;			// ���� �����δ�. �ӽ÷� ����µ�... ����ӵ��� 3��� �ڴ�.
	MOVE_DIRECTION				m_eMoveDirection;		// Directional Move����
	BOOL						m_bPathFinding;			// Path Finding Mode����
	BOOL						m_bSync;				// Sync�� ����� �Ҷ�
	BOOL						m_bHorizontal;			// ���� �̵�����

	BOOL						m_bWasMoveFollow;		// ���߱� ���� Follow �ϰ� �־����� ����

	UINT32						m_ulCharType;

	INT32						m_lAdjustMoveSpeed;		// �̵� �ӵ� ������

	INT32						m_lFollowTargetID;		// �� Ÿ���� ���� �����δ�.
	INT32						m_lFollowDistance;		// ���󰡴� �Ÿ� (Ÿ�ٰ��� �Ÿ��� �� �Ÿ� ���Ϸ� �ٸ� �����.)

	UINT32						m_ulSyncMoveTime;		// ���������� ��Ŷ�� ���� �ð�

	//CHAR						m_szTitle[AGPACHARACTER_MAX_CHARACTER_TITLE + 1];			// �úη� ���ʱ��� ������ ��ų�ʱ�ȭ���� �־ ��� ���� ¯���� �ϳ�.
	CHAR						m_szSkillInit[AGPACHARACTER_MAX_CHARACTER_SKILLINIT + 1];	// DB�÷��̸��̾� ��¿ �� ��� �̰Ŷ� �ٲ���.
	CHAR						m_szNickName[AGPACHARACTER_MAX_CHARACTER_NICKNAME + 1];	// ��Ī.
	
	AuPOS						m_stPos;
	AuPOS						m_stDestinationPos;		// �̵��Ҷ� ������ ��ǥ
	AuPOS						m_stDirection;			// �̵��Ҷ� ����

	UINT32						m_ulStartMoveTime;		// Destination���� �̵��ϱ� ������ �ð�
	UINT32						m_ulEndMoveTime;		// ������ ��ǥ���� �̵��ϴ� �ð�

	FLOAT						m_fTurnX;				// �ɸ����� ���� (X�� ����)
	FLOAT						m_fTurnY;				// �ɸ����� ���� (Y�� ����)

	AuMATRIX					m_stMatrix;				// �ɸ����� ���⼺ Matrix (�ٸ� Factor���� ���� �ȵ�)

	INT16						m_nBlockInfo;			// Blocking Info�� ����

	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astBlockInfo;
//	AuBLOCKING					m_astBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];
														// Blocking Info
	ApSafeArray<AuBLOCKING, AGPDCHARACTER_MAX_BLOCK_INFO>			m_astWorldBlockInfo;
//	AuBLOCKING					m_astWorldBlockInfo[AGPDCHARACTER_MAX_BLOCK_INFO];
														// Calculated Blocking Info (world coordinates)

	AgpdFactor					m_csFactor;				// ���� Factor�� ���⼭ ����

	// �������� ��꿡�� ���ȴ�. packet ���� �ְ� �ް� ������ �ʴ� �ѵ��̴�.
	AgpdFactor					m_csFactorPoint;		// �� ĳ������ factor�� �� (����, ��ų �� ���ļ�) Point ���� ����
	AgpdFactor					m_csFactorPercent;		// �� ĳ������ factor�� �� (����, ��ų �� ���ļ�) percent ���� ����

	AgpdTitle*					m_csTitle;
	AgpdTitleQuest*				m_csTitleQuest;

	BOOL						m_bIsTrasform;			// ���� transform�� ���������� ����
	BOOL						m_bIsEvolution;			// ��ȭ ����
	INT32						m_lOriginalTID;			// for transform tid recovery
	AgpdFactor					m_csFactorOriginal;		// for transform status recovery
	INT32						m_lTransformedByCID;	// �������� Transform ��� ��. �ڱ� �ڽ��� �� �ִ�. 2005.10.24. steeple
	INT32						m_eTransformType;		// ��� �����ߴ��� ���. 2006.08.09. steeple
	INT32						m_lFixTID;				// ó�������� TID�� ����.

	INT32						m_lTransformItemTID;	// Item �� ����ؼ� transform �� �Ŷ�� ����ٰ� TID ����. 2008.03.11. steeple
	BOOL						m_bCopyTransformFactor;	// Copy �� �Ѱ��� Add �� �Ѱ��� ����. 2008.03.11. steeple

	UINT16						m_unCurrentStatus;		// �� ĳ������ ���� ����

	INT8						m_unActionStatus;
									// AGDMCHAR_STATUS_NORMAL : ����ִ� (�Ϲ� ����)
									// AGDMCHAR_STATUS_PREDEAD : �װ� ó���� ��ٸ���. (������ ������ �ƴϴ�)
									// AGDMCHAR_STATUS_DEAD : �׾���
									// AGPDCHAR_STATUS_READY_SKILL : ��ų�� �������� ������ ������ �ð����� ��ٸ��� �ִ�.
									// AGPDCHAR_STATUS_DURATION_SKILL : ��ų�� �Ἥ ������̴�.

									// AGPDCHAR_STATUS_TRADE : �ŷ��� �����ؼ� ���� ���´�.

	INT8						m_unCriminalStatus;		// �� ĳ������ ���˻��� ���θ� ��Ÿ����.

	UINT32						m_ulPrevProcessTime;	// ������ ó���� �ð� (OnIdle����)

	BOOL						m_bIsAddMap;			// �ʿ� �߰��� �Ǿ����� ���� (TRUE : �߰�)

	INT64						m_llMoney;				//��~
	INT64						m_llBankMoney;			// ��ũ�� �ִ� ��~
	INT64						m_llCash;				// ����

	INT8						m_cBankSize;

	INT32						m_alTargetCID[AGPDCHARACTER_MAX_TARGET];

	AgpdCharacterAction			m_stNextAction;			// ���� Action (�������� ���� �Ŀ� �̷���� Action)

	//ApSafeArray<INT32, AGPMCHAR_MAX_ATTACKER_LIST>	m_lAttackerList;	// �� ���� PC �� �Ƶ� ����Ʈ

	UINT32						m_ulRemoveTimeMSec;		// �� ĳ���Ͱ� ������ Ŭ��ī��Ʈ

	//CHAR						m_szBindingAreaName[AGPMCHAR_MAX_TOWN_NAME + 1];

	// ������ (2004-09-17 ���� 5:17:28) : ���ε� ���� �ε���
	// ���� �Ҽӵ� ���� �ε��� �� ������.
	INT16						m_nBindingRegionIndex;
	INT16						m_nLastExistBindingIndex;
	INT16						m_nBeforeRegionIndex;
	UINT32						m_ulNextRegionRefreshTime;

	BOOL						m_bIsCombatMode;		// ������� == TRUE
	UINT32						m_ulSetCombatModeTime;	// ������� ���� �ð�

	UINT32						m_ulCheckBlocking;	// 0x01 : ������ŷ 0x02 : ������Ʈ ��ŷ
													// üũ�Ұ͸� �ֱ� .
													// �ڼ��Ѱ� Apmmap ApTileInfo�� ����..

	BOOL						m_bIsActionBlock;
	UINT32						m_ulActionBlockTime;
	AgpdCharacterActionBlockType	m_eActionBlockType;

	UINT64						m_ulSpecialStatus;		// Ư�� ���¸� Bit Mask �� �����Ѵ�.

	BOOL						m_bIsPostedIdleProcess;

	BOOL						m_bIsActionMoveLock;
	AgpdCharacterAction			m_stLockAction;

	INT32						m_lFaceIndex;
	INT32						m_lHairIndex;

	INT32						m_lOptionFlag;		//	2005.05.31. By SungHoon �ź� Flag
	BOOL						m_bRidable;			// Ż�� �������� �����ϰ� ���� ������ ������ �������� TRUE
	INT32						m_nRideCID;			// Ż���� ���̵�.

	UINT16						m_unEventStatusFlag;

	// 	
	INT32						m_lLastUpdateCriminalFlagTime;		// criminal flag�� ���õ� ������ �ð�
	INT32						m_lLastUpdateMurdererPointTime;	// ������ ����Ʈ�� ���� ������ �ð�
	UINT32						m_ulBattleSquareKilledTime;	// ��Ʋ ������� ���������� ���� �ð�.

	INT32						m_lCountMobMoli;	// �����ϴ� ���� �� 2005.11.29. steeple
	
	BOOL						m_bIsWantedCriminal;

	AgpdCharacterGameBonusByCash m_stGameBonusByCash;

	BOOL						m_bIsActiveData;
	BOOL						m_bIsReadyRemove;

	BOOL						m_bIsProtectedNPC;
	INT32						m_nNPCID;

	BOOL						m_bNPCDisplayForMap;
	BOOL						m_bNPCDisplayForNameBoard;

	BOOL IsDead() const { return ( m_unActionStatus == AGPDCHAR_STATUS_DEAD ) ? TRUE : FALSE; }

	// �ߵ� ���� - 2007.07.06. steeple
	UINT32						m_ulOnlineTime;
	INT32						m_lAddictStatus;

	UINT8						m_unDisturbAction;

	INT32						m_ulSpecialStatusReserved;		// SpecialStatus���� �����̻������ �ʿ��� �ڷ� ������ (ex, ������������ ������ ����)

	BOOL	GetSpecialState( UINT64 ulStatus )	{	return m_ulSpecialStatus & ulStatus ? TRUE : FALSE; 	}

	bool	IsPC()	{ return m_szID[0] ? true : false; }
};


class AgpdCharisma
	{
	public :
		INT32					m_lPoint;
		CHAR					m_szTitle[AGPACHARACTER_MAX_CHARACTER_NICKNAME + 1];
	
	public :
		AgpdCharisma()
			{
			m_lPoint = 0;
			ZeroMemory(m_szTitle, sizeof(m_szTitle));
			}
	};

#endif //_AGPDCHARACTER_H_