#ifndef	_AGPDAI_H_
#define _AGPDAI_H_

#include "ApBase.h"
#include "AgpmFactors.h"
#include "AgpdCharacter.h"
#include "AgpdItem.h"
#include "AgpdEventSpawn.h"
#include "AuList.h"

#define AGPDAI_TEMPLATE_NAME_LENGTH		20
#define AGPDAI_MAX_USABLE_ITEM_COUNT	10
#define AGPDAI_MAX_USABLE_SKILL_COUNT	10
#define AGPDAI_MAX_HELP_CHARTYPE_COUNT  10

// Action Type
typedef enum
{
	AGPDAI_ACTION_TYPE_HOLD = 0,		// ���ڸ����� ���
	AGPDAI_ACTION_TYPE_MOVE,			// Ư�� ��ġ�� �̵�
	AGPDAI_ACTION_TYPE_FOLLOW,			// Target�� ����
	AGPDAI_ACTION_TYPE_MOVE_AWAY,		// Target���� ������
	AGPDAI_ACTION_TYPE_WANDER,			// ��ȸ
	AGPDAI_ACTION_TYPE_GUARD,			// Target�� ��Ŵ
	AGPDAI_ACTION_TYPE_ATTACK,			// Target�� ����
	AGPDAI_ACTION_TYPE_ASSIST,			// Target�� ������
	AGPDAI_ACTION_TYPE_YELL,			// Target�� ������ȯ
	AGPDAI_ACTION_TYPE_PICKUP,			// Target�� �ݴ´�.
	AGPDAI_ACTION_TYPE_USE_ITEM,		// Item�� ���
	AGPDAI_ACTION_TYPE_USE_SKILL,		// Skill�� ���
	AGPDAI_ACTION_TYPE_SCREAM,			// Scream Fear���
	AGPDAI_ACTION_MAX_TYPE
} AgpdAIActionType;

typedef enum
{
	AGPDAI_TARGET_FLAG_RELATION_AGGRESSIVE	= 0x0001,	// Relation�� -�� ������ �����̴�.
	AGPDAI_TARGET_FLAG_RELATION_FRIENDLY	= 0x0002,	// Relation�� +�� ���� �����̴�.
	AGPDAI_TARGET_FLAG_AGRO					= 0x0004,	// Agro Point�� �ִ�.
	AGPDAI_TARGET_FLAG_HISTORY_DAMAGE		= 0x0010,	// History�� �ظ� ���� ������ �Ǿ��ִ�.
	AGPDAI_TARGET_FLAG_HISTORY_BUFFER		= 0x0020,	// History�� ������ �� ������ �Ǿ��ִ�.
	AGPDAI_TARGET_FLAG_STRONG				= 0x0040,	// ���� Target
	AGPDAI_TARGET_FLAG_WEAK					= 0x0080,	// ���� Target
	AGPDAI_TARGET_FLAG_BOSS					= 0x0100,	// Boss
	AGPDAI_TARGET_FLAG_FOLLOWER				= 0x0200,	// Follower
	AGPDAI_TARGET_FLAG_SELF					= 0x0400,	// �ڱ� �ڽ�
	AGPDAI_TARGET_FLAG_OTHER				= 0x0800,	// �ٸ� ��
	AGPDAI_TARGET_MAX_FLAG
} AgpdAITargetFlag;

typedef enum
{
	AGPDAI_FACTOR_TYPE_DEFAULT = 0,				// �⺻ Factor
	AGPDAI_FACTOR_TYPE_CHAR_FACTOR,				// Character�� Factor
	AGPDAI_FACTOR_TYPE_CHAR_STATUS,				// Character�� Status
	AGPDAI_FACTOR_TYPE_CHAR_ITEM,				// Character�� ���� Item
	AGPDAI_FACTOR_TYPE_ACTION_HISTORY,			// Action Histroy
	AGPDAI_FACTOR_TYPE_ENV_POSITION,			// Target���κ����� ��ġ
	AGPDAI_FACTOR_TYPE_EMOTION,					// ���� ���
	AGPDAI_FACTOR_MAX_TYPE
} AgpdAIFactorType;

typedef enum
{
	AGPDAI_EMOTION_TYPE_NORMAL = 0,				// ����
	AGPDAI_EMOTION_TYPE_PEACE,					// ��ȭ�ο�
	AGPDAI_EMOTION_TYPE_NERVOUS,				// ���
	AGPDAI_EMOTION_TYPE_ANGER,					// ����
	AGPDAI_EMOTION_MAX_TYPE
} AgpdAIEmotionType;

class AgpdAIScream
{
public:
	bool				m_bUseScream;
	INT32				m_lFearLV1Agro;
	INT32				m_lFearLV2HP;
	bool				m_bFearLV3;

	INT32				m_lHelpLV1CopyAgro;
	INT32				m_lHelpLV2Heal;
	INT32				m_lHelpLV3;
	//INT32				m_alHelpCharTID[AGPDAI_MAX_HELP_CHARTYPE_COUNT];
	ApSafeArray<INT32, AGPDAI_MAX_HELP_CHARTYPE_COUNT>	m_alHelpCharTID;
	bool				m_bHelpAll;
};

class AgpdAIUseItem
{
public:
	INT32				m_lLastTime;
	INT32				m_lHP;
//	INT32				m_alTID[AGPDAI_MAX_USABLE_ITEM_COUNT];
//	INT32				m_alRate[AGPDAI_MAX_USABLE_ITEM_COUNT];

	ApSafeArray<INT32, AGPDAI_MAX_USABLE_ITEM_COUNT>	m_alTID;
	ApSafeArray<INT32, AGPDAI_MAX_USABLE_ITEM_COUNT>	m_alRate;

	AgpdAIUseItem()
	{
		m_lLastTime = 0;
		m_lHP = 0;
//		memset( m_alTID, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );
//		memset( m_alRate, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_ITEM_COUNT );

		m_alTID.MemSetAll();
		m_alRate.MemSetAll();
	}
};

class AgpdAIUseSkill
{
public:
	INT32				m_lLastTime;
	INT32				m_lHP;
//	INT32				m_alTID[AGPDAI_MAX_USABLE_SKILL_COUNT];
//	INT32				m_alRate[AGPDAI_MAX_USABLE_SKILL_COUNT];

	ApSafeArray<INT32, AGPDAI_MAX_USABLE_SKILL_COUNT>	m_alTID;
	ApSafeArray<INT32, AGPDAI_MAX_USABLE_SKILL_COUNT>	m_alRate;

	AgpdAIUseSkill()
	{
		m_lLastTime = 0;
		m_lHP = 0;
//		memset( m_alTID, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_SKILL_COUNT );
//		memset( m_alRate, 0, sizeof(INT32)*AGPDAI_MAX_USABLE_SKILL_COUNT );

		m_alTID.MemSetAll();
		m_alRate.MemSetAll();
	}
};

struct AgpdAITargetWeight
{
	INT8				m_lRelationAggressive;	// Relation�� -�ϼ��� ���� ����ġ							( * -Relation)
	INT8				m_lRelationFriendly;	// Relation�� +�ϼ��� ���� ����ġ							( * Relation)
	INT8				m_lAgro;				// Agro Point�� �������� ���� ����ġ						( * Agro / 1000)
	INT8				m_lHistoryDamage;		// History�� Damage�ϼ��� ���� ����ġ						( * Damage * 10 / MaxHP)
	INT8				m_lHistoryBuffer;		// History�� Buff�ϼ��� ���� ����ġ							( * Buff * 10 / MaxHP)
	INT8				m_lNear;				// �Ÿ��� �������� ���� ����ġ							( * 10 * (1 - Distance / Visibility))
	INT8				m_lFar;					// �Ÿ��� �ּ��� ���� ����ġ								( * 10 * (Distance / Visibility))
	INT8				m_lStrong;				// Target�� �ڽź��� ������ ���� ����ġ						( * -LevelDiff)
	INT8				m_lWeak;				// Target�� �ڽź��� ���Ҽ��� ���� ����ġ					( * Leveldiff)
	INT8				m_lBoss;				// Target�� Boss�϶� ������ ����ġ
	INT8				m_lFollower;			// Target�� Follower�϶� ������ ����ġ
	INT8				m_lSelf;				// Target�� Self�϶� ������ ����ġ
	INT8				m_lOther;				// Target�� Self�� �ƴҶ� ������ ����ġ
};

struct AgpdAITarget
{
	ApBase				m_csTargetBase;			// Target�� Base Type�� ID(�ʿ��ϴٸ�)
	INT32				m_lTargetFlags;			// �ݵ�� �ʿ��� ��ġ Flag (�� Flag�� ��갪�� 0�̸�, Target�� �� �� ����.)
	AgpdAITargetWeight	m_stTargetWeight;		// Target�� �����ϴ� �� �ʿ��� ��ġ��
	AuPOS				m_stTargetPos;			// Target Position
	FLOAT				m_fRadius;				// Target Radius
};

struct AgpdAIAction
{
	AgpdAIActionType	m_eActionType;			// Action Type
	INT32				m_lActionRate;			// Action Rate

	AgpdAITarget		m_stTarget;

	AgpdAIAction *		m_pstNext;				// ���� �����(����) Action �� ���� ����
};

struct AgpdAIFactorCharFactor
{
	INT32				m_lCompareValue;
	AgpdFactor			m_stFactor;
};

struct AgpdAIFactorCharStatus
{
	AgpdCharStatus		m_eStatus;
};

struct AgpdAIFactorCharItem
{
	INT32				m_lItemTID;
};

struct AgpdAIFactorActionHistory
{
};

struct AgpdAIFactorEnvPosition
{
};

struct AgpdAIFactorEmotion
{
	AgpdAIEmotionType	m_eEmotionType;
};

struct AgpdAIActionResult
{
	AgpdAIActionType	m_eActionType;
	INT32				m_lActionRate;
	AgpdAIAction *		m_pstActions;			// ���� �����(����) Action���� Rate�� ����
};

struct AgpdAIFactor
{
	INT32				m_lFactorID;			// AIFactor ID (���������� ���)
	AgpdAIFactorType	m_eType;				// Factor Type
	union
	{
		PVOID						m_pvData;
		AgpdAIFactorCharFactor *	m_pstCharFactor;
		AgpdAIFactorCharStatus *	m_pstCharStatus;
		AgpdAIFactorCharItem *		m_pstCharItem;
		AgpdAIFactorActionHistory *	m_pstActionHistory;
		AgpdAIFactorEnvPosition *	m_pstEnvPosition;
		AgpdAIFactorEmotion *		m_pstEmotion;
	} m_uoData;									// �� Factor Type�� ���� Data

	//AgpdAIAction		m_astActions[AGPDAI_ACTION_MAX_TYPE];	// Action ����
	ApSafeArray<AgpdAIAction, AGPDAI_ACTION_MAX_TYPE>	m_astActions;
};

typedef struct
{
	FLOAT					m_fVisibility;				// �þ߰Ÿ�
	UINT32					m_ulProcessInterval;

	//������ �������.
	AgpdAIUseItem			m_csAIUseItem;
	//��ų �������
	AgpdAIUseItem			m_csAIUseSkill;
	//Scream ����
	AgpdAIScream			m_csAIScream;

	AuList< AgpdAIFactor >	m_listAIFactors;
} AgpdAI;

typedef struct
{
	INT32					m_lID;
	CHAR					m_szName[AGPDAI_TEMPLATE_NAME_LENGTH];

	AgpdAI					m_stAI;
} AgpdAITemplate;

typedef struct
{
	BOOL					m_bUseAI;
	INT32					m_lTID;

	AgpdAI					m_stAI;

	UINT32					m_ulPrevProcessTime;
	//AgpdAIActionResult		m_astResult[AGPDAI_ACTION_MAX_TYPE];
	ApSafeArray<AgpdAIActionResult, AGPDAI_ACTION_MAX_TYPE>	m_astResult;
} AgpdAIADChar;

typedef struct
{
	//AgpdAIADChar			m_astAIADChar[AGPDSPAWN_MAX_CHAR_NUM];
	ApSafeArray<AgpdAIADChar, AGPDSPAWN_MAX_CHAR_NUM>		m_astAIADChar;
} AgpdAIADSpawn;

//Agro���� ����ü
typedef enum
{
	AGPDAI_AGRO_TYPE_TRY_ATTACK = 0,
	AGPDAI_AGRO_TYPE_DEMAGE,
	AGPDAI_AGRO_TYPE_TRY_SKILL,
	AGPDAI_AGRO_TYPE_SKILL,
	AGPDAI_AGRO_TYPE_TRY_HEAL,
	AGPDAI_AGRO_TYPE_HEAL,
	AGPDAI_AGRO_TYPE_TRY_BUFF,
	AGPDAI_AGRO_TYPE_BUFF,
	AGPDAI_AGRO_TYPE_TRY_DEBUFF,
	AGPDAI_AGRO_TYPE_DEBUFF,
	AGPDAI_AGRO_TYPE_TRY_TAUNT,
	AGPDAI_AGRO_TYPE_TAUNT,
	AGPDAI_AGRO_TYPE_MAX,
} AgpdAIAgroType;

#endif //_AGPDAI_H_
