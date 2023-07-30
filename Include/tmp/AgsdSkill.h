/******************************************************************************
Module:  AgsdSkill.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 03. 24
******************************************************************************/

#if !defined(__AGSDSKILL_H__)
#define __AGSDSKILL_H__

#include "ApBase.h"

#include "AsDefine.h"
#include <atltime.h>

#define	AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER			10
#define AGSMSKILL_MAX_SKILLTREE_LENGTH				512

typedef struct _stMasteryInfo {
	CHAR		szSkillTree[AGSMSKILL_MAX_SKILLTREE_LENGTH + 1];
	CHAR		szSpecialize[33];
	CHAR		szProductCompose[AGSMSKILL_MAX_SKILLTREE_LENGTH + 1];	// compose list of product skill
} stMasteryInfo;

// 2005.09.15. steeple
typedef enum _eAgsdSkillMissedReason
{
	AGSDSKILL_MISSED_REASON_NORMAL = 0,
	AGSDSKILL_MISSED_REASON_DURATION_UNLIMITED,
	AGSDSKILL_MISSED_REASON_TARGET_NUM_ZERO,
	AGSDSKILL_MISSED_REASON_CANNOT_TRANSPARENT_FOR_COMBAT,
	AGSDSKILL_MISSED_REASON_TAME_FAILURE_ABNORMAL,
} eAgsdSkillMissedReason;

// 2007.10.23. steeple
typedef enum _eAgsdSkillConnectionType
{
	AGSDSKILL_CONNECTION_TYPE_NONE			= 0x00,
	AGSDSKILL_CONNECTION_TYPE_DAMAGE		= 0x01,
	AGSDSKILL_CONNECTION_TYPE_BUFF			= 0x02,
} eAgsdSkillConnectionType;

typedef enum _eAgsdSkillConnectionConditionType
{
	AGSDSKILL_CONNECTION_CONDITION_TYPE_NONE		= 0x00,
	AGSDSKILL_CONNECTION_CONDITION_TYPE_BUFF		= 0x01,
	AGSDSKILL_CONNECTION_CONDITION_TYPE_TRANSPARENT	= 0x02,
} eAgsdSkillConnectionConditionType;

// 2007.10.23. steeple
struct AgsdSkillConnectionInfo
{
	BOOL m_bNeedCheck;
	BOOL m_bAuthorized;		// �̹� üũ�� ����ؼ� �����߿��� ��� ���־�� �Ѵ�.
	eAgsdSkillConnectionType m_eType;

	eAgsdSkillConnectionConditionType m_eConditionType;
	INT32 m_lConditionTID;

	void init()
	{
		m_bNeedCheck = FALSE;
		m_bAuthorized = FALSE;
		m_eType = AGSDSKILL_CONNECTION_TYPE_NONE;
		m_eConditionType = AGSDSKILL_CONNECTION_CONDITION_TYPE_NONE;
		m_lConditionTID = 0;
	}
};

// 2007.10.26. steeple
// Union Skill �� Control �ϴ� Skill ����
struct stUnionControlInfo
{
	INT32 m_lUnionID;
	INT32 m_lMaxCount;
	INT32 m_lCurrentCount;

	void init()
	{
		m_lUnionID = 0;
		m_lMaxCount = 1;	// �⺻���� 1�̴�.
		m_lCurrentCount = 0;
	}

	bool operator == (INT32 lUnionID)
	{
		return m_lUnionID == lUnionID;
	}
};

#define AGSMSKILL_MAX_UNION_CONTROL_COUNT		5		// 5���� ����ҵ�.

struct UnionControlArray
{
	typedef ApSafeArray<stUnionControlInfo, AGSMSKILL_MAX_UNION_CONTROL_COUNT>::iterator	iterator;

	ApSafeArray<stUnionControlInfo, AGSMSKILL_MAX_UNION_CONTROL_COUNT>		m_astUnionControlInfo;
};

class AgsdSkill {
public:
	/*
	UINT32					m_ulStartTime;
	UINT32					m_ulEndTime;
	UINT32					m_ulInterval;
	UINT32					m_ulNextProcessTime;
	UINT32					m_ulRecastDelay;
	*/

	AgpdFactor				*m_pcsUpdateFactorResult;
	AgpdFactor				*m_pcsUpdateFactorPoint;		// factor�� �����Ű�� ��ų�� ��� ����Ǵ� ���� ����� �ִ´�.
	AgpdFactor				*m_pcsUpdateFactorPercent;
	
	AgpdFactor				*m_pcsUpdateFactorItemPoint;
	AgpdFactor				*m_pcsUpdateFactorItemPercent;

														// ���߿� �̰��� ���� ���� ������ �ǵ�����.

	//AgpdFactor				m_csModifyFactorPoint[AGPMSKILL_MAX_AFFECTED_BASE];		// factor�� �����Ű�� ��ų�� ��� ����Ǵ� ���� ����� �ִ´�.
	//AgpdFactor				m_csModifyFactorPercent[AGPMSKILL_MAX_AFFECTED_BASE];
														// ���߿� �̰��� ���� ���� ������ �ǵ�����.

	INT32					m_lModifySkillLevel;
	INT32					m_lModifyCharLevel;

	INT32					m_nMeleeDefensePoint;			// ���� ����� �� �ִ� ������
	INT32					m_nMeleeReflectPoint;			// ���� �ݻ��ų �� �ִ� ������
	INT32					m_nMagicDefensePoint;			// ���� ����� �� �ִ� ������
	INT32					m_nMagicReflectPoint;			// ���� �ݻ��ų �� �ִ� ������
	
	INT32					m_lDamageShield;

	AgpdSkillBuffedCombatEffectArg	m_stBuffedSkillCombatEffectArg;				// �� ��ų�� ���� ����� ������ �����Ѵ�.
	AgpdSkillBuffedFactorEffectArg	m_stBuffedSkillFactorEffectArg;

	BOOL					m_bIsCastSkill;
	INT32					m_lSkillScrollIID;

	UINT32					m_ulLastCastTwiceSkillClock;	// ������ ĳ��Ʈ�� ��ų Clock
	ApSafeArray<INT32, 33>	m_alLastTwiceSkillTargetCID;	// ������ ĳ��Ʈ�� �� ������ Ÿ�� �迭
	INT32					m_lLastTwiceSkillNumTarget;		// ���� �迭�� �� Ÿ�� ��

	eAgsdSkillMissedReason	m_eMissedReason;				// ��ų�� ������ ���� 2005.09.15. steeple
	BOOL					m_bApplyDamageOnly;				// �������� ������� ���� �ִ� ��ų�ε�, �������� �ִ� ��� TRUE

	INT16					m_nAffectedDOTCount;			// ���� �Ⱓ���� ����� DOT Ƚ��
	INT16					m_nTotalDOTCount;				// �� �����ؾ��� DOT Ƚ��

	InvincibleInfo			m_stInvincibleInfo;				// �׸� ���� Ȯ�� ����.

	AgsdSkillConnectionInfo	m_stConnectionInfo;				// � ��ų�� �����Ǿ �߰� ȿ���� �־����� ����.
	stUnionControlInfo		m_stUnionInfo;					// 

	ApSafeArray<INT32, AGPMSKILL_MAX_SKILL_LEVELUP_TID>		m_alLevelUpSkillTID;
};

class AgsdSkillProcessInfo {
public:
	/*
	INT32					m_lSkillID;
	AgpdSkillTemplate*		m_pcsTemplate;

	INT32					m_lSkillLevel;

	UINT32					m_tmEnd;
	UINT32					m_tmInterval;
	UINT32					m_tmNextProcess;

	AgpdFactor				m_csModifyFactorPoint;
	AgpdFactor				m_csModifyFactorPercent;
	*/

	ApBaseExLock			m_csSkillOwner;
	INT32					m_lMasteryPoint;

	AgpdSkill*				m_pcsTempSkill;
};

typedef vector<AgsdEquipSkill> CEquipSkillArray;

class AgsdSkillADBase {
public:
//	AgsdSkillProcessInfo	m_csBuffedSkillProcessInfo[AGPMSKILL_MAX_SKILL_BUFF];
	ApSafeArray<AgsdSkillProcessInfo, AGPMSKILL_MAX_SKILL_BUFF>		m_csBuffedSkillProcessInfo;

	INT32					m_lDamageAdjustRate;				// damage adjust skill�� ���� DamageAdjust rate

	stMasteryInfo			*m_pstMasteryInfo;

	// ���������� ���Ǵ�.. Ȥ�� �� TID�� ���������� ���Ǵ� ��ų���� ���⿡ �����ؼ� ����Ѵ�.
//	AgpdSkill				*m_ppcsSkillItem[AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER];
	ApSafeArray<AgpdSkill *, AGSMSKILL_MAX_TEMPLATE_SKILL_BUFFER>	m_ppcsSkillItem;

	UnionControlArray		m_UnionControlArray;

	UINT32					m_ulLastReleasedTransparentClock;	// ���������� ��ų������ ������ �ø� Ŭ��.
	INT32					m_lReleaseTransparentSkillTID;		// ���������� ������ Ǯ���� Skill TID

	CEquipSkillArray		m_csEquipSkillArray;
};

/*
class AgsdSkillTemplate {
public:
	INT32					m_lUsedConstFactor[AGSMSKILL_CONST_MAX][AGSMSKILL_MAX_SKILL_CAP];
														// ��ų ��꿡 ���Ǵ� �����
};
*/

struct AgsdCastSkill
{
	ApBase *pcsBase;
	ApBase *pcsTargetBase;
	INT32 lSkillTID;
	INT32 lSkillLevel;
	INT32 lSkillScrollIID;
	BOOL bForceAttack;
	BOOL bMonsterUse;
	SaveSkillData m_pSaveSkillData;

	AgsdCastSkill()
		: pcsBase(NULL), lSkillTID(0), lSkillLevel(0)
		, pcsTargetBase(NULL), bForceAttack(FALSE), bMonsterUse(FALSE)
		, lSkillScrollIID(AP_INVALID_IID)
	{
		m_pSaveSkillData.Clear();
	}
};

#endif	//__AGSDSKILL_H__