//	AgpmCombat.cpp
////////////////////////////////////////////////////////////////

#include <AgpmCombat/AgpmCombat.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmFactors/AgpmFactors.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmItemConvert/AgpmItemConvert.h>

AgpmCombat::AgpmCombat()
{
	SetModuleName("AgpmCombat");
}

AgpmCombat::~AgpmCombat()
{
}

BOOL AgpmCombat::OnAddModule()
{
	m_pcsAgpmFactors			= (AgpmFactors *)			GetModule("AgpmFactors");
	m_pcsAgpmCharacter			= (AgpmCharacter *)			GetModule("AgpmCharacter");
	m_pcsAgpmItem				= (AgpmItem *)				GetModule("AgpmItem");
	m_pcsAgpmItemConvert		= (AgpmItemConvert *)		GetModule("AgpmItemConvert");
	m_pcsAgpmSkill				= (AgpmSkill *)				GetModule("AgpmSkill");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmItemConvert ||
		!m_pcsAgpmSkill)
		return FALSE;

	return TRUE;
}

BOOL AgpmCombat::OnInit()
{
	return TRUE;
}

BOOL AgpmCombat::OnDestroy()
{
	return TRUE;
}

FLOAT AgpmCombat::GetAR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass)
{
	if (!pcsCharacter)
		return 0;

	FLOAT	fAR		= 0.0f;

	if (m_pcsAgpmCharacter->IsMonster(pcsCharacter))
	{
//		if (lAttackerClass == AUCHARCLASS_TYPE_MAGE)
//		{
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR);
//		}
//		else
		{
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);
		}
	}
	else
	{
//		if (lAttackerClass == AUCHARCLASS_TYPE_MAGE)
//		{
//			FLOAT	fWIS	= 0.0f;
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fWIS, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
//
//			FLOAT	fMAR	= 0.0f;
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fMAR, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAR);
//
//			fAR	= ((m_pcsAgpmFactors->GetLevel(&pcsCharacter->m_csFactor) + fWIS + 20) * 0.6) + fMAR;
//		}
//		else
		{
			FLOAT	fDEX	= 0.0f;
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDEX, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

			fAR	= 0.0f;
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fAR, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);

			fAR	= ((m_pcsAgpmCharacter->GetLevel(pcsCharacter) + fDEX + 20.f) * 0.6f) + fAR;
		}
	}

	return fAR;
}

FLOAT AgpmCombat::GetBaseAR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass)
{
	if (!pcsCharacter)
		return 0;

	INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	if (lLevel < 1 || lLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate	*pcsOriginalTemplate	= NULL;
	if (pcsCharacter->m_bIsTrasform)
		pcsOriginalTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(pcsCharacter->m_lOriginalTID);
	else
		pcsOriginalTemplate	= pcsCharacter->m_pcsCharacterTemplate;

	FLOAT	fDEX	= 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fDEX, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

	FLOAT	fAR	= 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fAR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);

	return ((lLevel + fDEX + 20) * 0.6f) + fAR;
}

FLOAT AgpmCombat::GetDR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass)
{
	if (!pcsCharacter)
		return 0;

	if (m_pcsAgpmCharacter->IsMonster(pcsCharacter))
	{
		FLOAT	fDR	= 0.0f;
//		if (lAttackerClass == AUCHARCLASS_TYPE_MAGE)
//		{
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MDR);
//		}
//		else
		{
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);
		}

		return fDR;
	}
	else
	{
//		if (lAttackerClass == AUCHARCLASS_TYPE_MAGE)
//		{
//			FLOAT	fWIS	= 0.0f;
//			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fWIS, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
//
//			return ((m_pcsAgpmFactors->GetLevel(&pcsCharacter->m_csFactor) + fWIS + 20) * 0.3);
//		}
//		else
		{
			FLOAT	fDEX	= 0.0f;
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDEX, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

			FLOAT	fDR	= 0.0f;
			m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDR, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);

			return ((m_pcsAgpmCharacter->GetLevel(pcsCharacter) + fDEX + 20.f) * 0.4f) + fDR;
		}
	}

	return 0.0f;
}

FLOAT AgpmCombat::GetBaseDR(AgpdCharacter *pcsCharacter, INT32 lAttackerClass)
{
	if (!pcsCharacter)
		return 0;

	INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsCharacter);
	if (lLevel < 1 || lLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate	*pcsOriginalTemplate	= NULL;
	if (pcsCharacter->m_bIsTrasform)
		pcsOriginalTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(pcsCharacter->m_lOriginalTID);
	else
		pcsOriginalTemplate	= pcsCharacter->m_pcsCharacterTemplate;

	FLOAT	fDEX	= 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fDEX, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

	FLOAT	fDR	= 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fDR, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_DR);

	return ((lLevel + fDEX + 20.f) * 0.4f) + fDR;
}

// 2005.01.15. steeple
FLOAT AgpmCombat::GetPhysicalDefense(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0.0f;

	FLOAT fPhysicalDefense = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fPhysicalDefense, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	// 2005.11.30. steeple
	// PC ���, ���̿� ���� �г�Ƽ�� �־�� �Ѵ�. 2����° ���� �ش�.
	//if(m_pcsAgpmCharacter->IsPC(pcsCharacter) && pcsCharacter->m_lCountMobMoli > 1)
	//{
	//	FLOAT fPenalty = fPhysicalDefense * 0.02f * (FLOAT)pcsCharacter->m_lCountMobMoli;
	//	fPhysicalDefense -= fPenalty;

	//	if(fPhysicalDefense < 0.0f)
	//		fPhysicalDefense = 0.0f;
	//}

	return fPhysicalDefense;
}

FLOAT AgpmCombat::GetHeroicDefense(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicDefense = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicDefense, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);

	return fHeroicDefense;
}

FLOAT AgpmCombat::GetHeroicDamageMin(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicDamageMin = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicDamageMin, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);

	return fHeroicDamageMin;
}

FLOAT AgpmCombat::GetHeroicDamageMax(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicDamageMax = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicDamageMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC);

	return fHeroicDamageMax;
}

FLOAT AgpmCombat::GetHeroicMeleeResistance(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicMeleeResistance = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicMeleeResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_MELEE);

	return fHeroicMeleeResistance;
}

FLOAT AgpmCombat::GetHeroicRangedResistance(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicRangedResistance = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicRangedResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_RANGE);

	return fHeroicRangedResistance;
}

FLOAT AgpmCombat::GetHeroicMagicResistance(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	FLOAT fHeroicMagicResistance = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fHeroicMagicResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC_MAGIC);

	return fHeroicMagicResistance;
}

FLOAT AgpmCombat::GetHeroicResistanceByHeroicClass(AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsCharacter)
		return 0.0f;

	eAgpmCharacterRoleType eHeroicType;
	FLOAT fHeroicResistance = 0.0f;

	eHeroicType = m_pcsAgpmSkill->GetCharacterSkillRoleType(pcsCharacter);
	switch(eHeroicType)
	{
	case AGPMSKILL_CHARACTER_ROLETYPE_MELEE:
		{
			fHeroicResistance = GetHeroicMeleeResistance(pcsCharacter);
		}break;

	case AGPMSKILL_CHARACTER_ROLETYPE_RANGED:
		{
			fHeroicResistance = GetHeroicRangedResistance(pcsCharacter);
		}break;

	case AGPMSKILL_CHARACTER_ROLETYPE_MAGIC:
		{
			fHeroicResistance = GetHeroicMagicResistance(pcsCharacter);
		}break;
	}

	return fHeroicResistance;
}

// �� �Լ��� �ش� ĳ������ Class�� �´� HeroicResistance�� ���� ���� �ݸ鿡
// �� �Լ��� �������� Class�� �´� �ǰ������� HeroicResistance�� ���� ���� �Լ��̴�.
FLOAT AgpmCombat::GetHeroicResistanceByAttackerHeroicClass(AgpdCharacter* pcsAttacker, AgpdCharacter* pcsCharacter)
{
	if(NULL == pcsAttacker || NULL == pcsCharacter)
		return 0.0f;

	eAgpmCharacterRoleType		eHeroicType;
	FLOAT						fHeroicResistance = 0.0f;

	eHeroicType = m_pcsAgpmSkill->GetCharacterSkillRoleType(pcsAttacker);
	switch(eHeroicType)
	{
	case AGPMSKILL_CHARACTER_ROLETYPE_MELEE:
		{
			fHeroicResistance = GetHeroicMeleeResistance(pcsCharacter);
		}break;

	case AGPMSKILL_CHARACTER_ROLETYPE_RANGED:
		{
			fHeroicResistance = GetHeroicRangedResistance(pcsCharacter);
		}break;

	case AGPMSKILL_CHARACTER_ROLETYPE_MAGIC:
		{
			fHeroicResistance = GetHeroicMagicResistance(pcsCharacter);
		}break;
	}

	return fHeroicResistance;
}

FLOAT AgpmCombat::GetPhysicalResistance(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0.0f;

	FLOAT fPhysicalResistance = 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fPhysicalResistance, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	// 2008.03.24. steeple
	// 0�϶� 1�� �����ϴ� �� ����̰� ������ ����.
	if(fPhysicalResistance > 80.0f)
		fPhysicalResistance = 80.0f;

	return fPhysicalResistance;
}

// 2007.07.31. steeple
INT32 AgpmCombat::GetPhysicalBlockRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lPhysicalBlockRate = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lPhysicalBlockRate, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL_BLOCK);

	if(lPhysicalBlockRate > 80)
		lPhysicalBlockRate = 80;

	return lPhysicalBlockRate;
}

// 2007.07.31. steeple
INT32 AgpmCombat::GetSkillBlockRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lSkillBlockRate = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lSkillBlockRate, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK);

	if(lSkillBlockRate > 80)
		lSkillBlockRate = 80;

	return lSkillBlockRate;
}

// 2005.12.22. steeple
INT32 AgpmCombat::GetAdditionalHitRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lHitRate = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lHitRate, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HIT_RATE);
	return lHitRate;
}

// 2005.12.22. steeple
INT32 AgpmCombat::GetAdditionalEvadeRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lEvadeRate = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lEvadeRate, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE);
	
	if(lEvadeRate > 80)
		lEvadeRate = 80;

	return lEvadeRate;
}

// 2005.12.22. steeple
INT32 AgpmCombat::GetAdditionalDodgeRate(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	INT32 lDodgeRate = 0;
	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lDodgeRate, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE);

	if(lDodgeRate > 80)
		lDodgeRate = 80;

	return lDodgeRate;
}

//		IsShieldDefenseSuccess
//	Functions
//		- ������ �����ߴ����� ���θ� ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCombat::IsShieldDefenseSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar)
{
	if (!pcsAttacker || !pcsTargetChar)
		return FALSE;

	INT32	lPanelty	=	CalcDefPenaltyLevelGap(pcsAttacker, pcsTargetChar);
	
//	// �޼տ� �� ��� �ִ��� ����.
//	AgpdItem *pcsItem = m_pcsAgpmItem->GetEquipSlotItem(pcsTargetChar, AGPMITEM_PART_HAND_LEFT);
//	if (!pcsItem)
//		return FALSE;
//
//	// ��� �ִ°� �������� ����.
//	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_SHIELD)
//		return FALSE;
//
//	INT32	lShieldDefenseRatePhysical	= 0;
//	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lShieldDefenseRatePhysical, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	INT32	lShieldDefenseRatePhysical	= GetPhysicalBlockRate(pcsTargetChar) - lPanelty;
	if (m_csRandom.randInt(100) <= lShieldDefenseRatePhysical)
		return TRUE;

	return FALSE;
}

//		IsAttackSuccess
//	Functions
//		- ������ �����ߴ����� ���θ� ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCombat::IsAttackSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar)
{
	if (!pcsAttacker || !pcsTargetChar)
		return FALSE;

	INT32		lAttackClass	= (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsAttacker->m_csFactor, &lAttackClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	FLOAT	fAttackAR		= GetAR(pcsAttacker, lAttackClass);
	FLOAT	fTargetDR		= GetDR(pcsTargetChar, lAttackClass);

	INT32	lAttackLevel	= m_pcsAgpmCharacter->GetLevel(pcsAttacker);
	INT32	lTargetLevel	= m_pcsAgpmCharacter->GetLevel(pcsTargetChar);

	INT32	lHitRate		= 0;

	if (fAttackAR + fTargetDR)
	{
//		if (lAttackClass == AUCHARCLASS_TYPE_MAGE)
//		{
//			// Magical Hit Rate = {Attack_Magical AR / (Attack_Magical AR + Defense_Magical DR)} * 2.35 * {Attack_LEVEL / (Attack_LEVEL + Defense_LEVEL)} * 100
//			lHitRate	= (INT32) ((fAttackAR / (float) (fAttackAR + fTargetDR)) * 2.35 * (lAttackLevel / (float) (lAttackLevel + lTargetLevel)) * 100);
//		}
//		else
		{
			// Physical Hit Rate = {Attack_Physical AR / (Attack_Physical AR + Defense_Physical DR)} * 2.84 * {Attack_LEVEL / (Attack_LEVEL + Defense_LEVEL)} * 100
			lHitRate	= (INT32) ((fAttackAR / (float) (fAttackAR + fTargetDR)) * 2.9 * (lAttackLevel / (float) (lAttackLevel + lTargetLevel)) * 100);
		}
	}
	else
		lHitRate	= 0;
	
	if (m_pcsAgpmCharacter->IsPC(pcsAttacker))
	{
		if (lAttackLevel < lTargetLevel)
	//	if (!m_pcsAgpmCharacter->IsPC(pcsAttacker) ||
	//		!m_pcsAgpmCharacter->IsPC(pcsTargetChar))
		{
			INT32	lMinHitRate	= 90 - 7 * abs(lAttackLevel - lTargetLevel);

			//if (lMinHitRate > 0 &&
			//	lHitRate < lMinHitRate)
				lHitRate	= lMinHitRate;
		}

		// 2005.12.22. steeple
		// �߰����� HitRate �� ���Ϳ��� �����´�.
		lHitRate += GetAdditionalHitRate(pcsAttacker);

		if (lHitRate < 10)
			lHitRate	= 10;
		else if (lHitRate > 95)
			lHitRate	= 95;
	}
	else
	{
		if (lAttackLevel < lTargetLevel)
	//	if (!m_pcsAgpmCharacter->IsPC(pcsAttacker) ||
	//		!m_pcsAgpmCharacter->IsPC(pcsTargetChar))
		{
			// 2005.03.28. steeple. 88 >> 90 ���� ����
			INT32	lMinHitRate	= 85 - 3 * abs(lAttackLevel - lTargetLevel);

			if (lMinHitRate > 0 &&
				lHitRate < lMinHitRate)
				lHitRate	= lMinHitRate;
		}

		// 2005.12.22. steeple
		// �߰����� HitRate �� ���Ϳ��� �����´�.
		lHitRate += GetAdditionalHitRate(pcsAttacker);

		if (lHitRate < 30)
			lHitRate	= 30;
		else if (lHitRate > 95)
			lHitRate	= 95;
	}

	if (m_csRandom.randInt(100) <= lHitRate)
		return TRUE;

	return FALSE;
}

//		IsFirstSpiritAttackSuccess
//	Functions
//		- ������ �����ߴ����� ���θ� ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCombat::IsFirstSpiritAttackSuccess(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar, INT32 lAttributeType)
{
	if (!pcsAttacker ||
		lAttributeType <= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_AIR)
		return FALSE;

	INT32	lActiveRate	= 0;

	if (m_pcsAgpmCharacter->IsPC(pcsAttacker))
	{
		AgpdItem	*pcsWeapon	= m_pcsAgpmItem->GetEquipWeapon(pcsAttacker);
		if (!pcsWeapon)
			return FALSE;

		INT32		lNumConvertAttr		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsWeapon, lAttributeType);

		// 2006.01.10. steeple
		// �ɼ� �ʿ� ���� �Ǹ鼭, �̰� üũ ���ؾ� �Ѵ�.
		//if (lNumConvertAttr < 1)
		//	return FALSE;

		lActiveRate			= 70 + lNumConvertAttr;
	}
	else
	{
		FLOAT	fAttrDamageMax	= 0.0f;
		m_pcsAgpmFactors->GetValue(&pcsAttacker->m_csFactor, &fAttrDamageMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

		if (fAttrDamageMax <= 0.0f)
			return FALSE;

		INT32	lAttackLevel	= m_pcsAgpmCharacter->GetLevel(pcsAttacker);
		INT32	lTargetLevel	= m_pcsAgpmCharacter->GetLevel(pcsTargetChar);

		lActiveRate			= 80 + (lAttackLevel - lTargetLevel);
	}

	if (lActiveRate > 100)
		lActiveRate	= 100;
	else if (lActiveRate < 60)
		lActiveRate	= 60;

	if (m_csRandom.randInt(100) <= lActiveRate)
		return TRUE;

	return FALSE;
}

//		IsSecondSpiritAttackSuccess
//	Functions
//		- ������ �����ߴ����� ���θ� ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgpmCombat::IsSecondSpiritAttackSuccess(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType)
{
	if (!pcsAttackChar ||
		!pcsTargetChar ||
		lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_POISON ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return FALSE;

	INT32	lActiveRate	= 0;

	if (m_pcsAgpmCharacter->IsPC(pcsAttackChar))
	{
		lActiveRate	= 70;
	}
	else
	{
		FLOAT	fAttrDamageMax	= 0.0f;
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttrDamageMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

		if (fAttrDamageMax <= 0.0f)
			return FALSE;

		INT32	lAttackLevel	= m_pcsAgpmCharacter->GetLevel(pcsAttackChar);
		INT32	lTargetLevel	= m_pcsAgpmCharacter->GetLevel(pcsTargetChar);

		lActiveRate			= 80 + (lAttackLevel - lTargetLevel);
	}

	if (lActiveRate > 100)
		lActiveRate	= 100;

	if (m_csRandom.randInt(100) <= lActiveRate)
		return TRUE;

	return FALSE;
}

//		CalcPhysicalAttack
//	Functions
//		- ���� ���� �������� ������ ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- INT32 : ���� ��� ������
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcPhysicalAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec, BOOL bIsMin, int nCombatPreCheck)
{
	if (!pcsAttackChar)
		return 0;

	INT32		lAttackerClass		= (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &lAttackerClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	INT32		lPenalty			= CalcDefPenaltyLevelGap(pcsAttackChar, pcsTargetChar);

	FLOAT		fPhysicalDefense	= GetPhysicalDefense(pcsTargetChar);
	FLOAT		fPhysicalResistance	= GetPhysicalResistance(pcsTargetChar) - lPenalty;
	
	if(fPhysicalResistance < 0)
		fPhysicalResistance = 0;

	if(nCombatPreCheck & AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_PHYSICAL_DEFENCE)
		fPhysicalDefense = 0;

	// 2005.04.09. steeple
	// �ü� ������ ������ �ٲ�.
	if(m_pcsAgpmCharacter->IsPC(pcsAttackChar))
	{
		FLOAT fAttackMinDmg = 0.0f;
		FLOAT fAttackMaxDmg = 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMinDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMaxDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

		FLOAT fSTR, fDEX, fINT;
		fSTR = fDEX = fINT = 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fSTR, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fDEX, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fINT, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

		FLOAT fMinDamage = 0.0f;
		FLOAT fMaxDamage = 0.0f;

		switch((AuCharClassType)lAttackerClass)
		{
			// 2005.11.30. steeple
			// �ּ� ������ ���ϴ� ���� 0.95 ���� 1.0 ���� ����.

			// 2005.04.09. steeple
			// �ü��� STR, DEX �� �� ����ϰ� Ÿ Ŭ������ STR �� ����.
			case AUCHARCLASS_TYPE_RANGER:
				fMinDamage = ((fSTR / 9.0f) + (fDEX / 4.5f)) * 1.0f + fAttackMinDmg;
				fMaxDamage = ((fSTR / 9.0f) + (fDEX / 4.5f)) * 1.05f + fAttackMaxDmg;
				break;

			// 2005.11.30. steeple
			// ���� ���� ��3
			case AUCHARCLASS_TYPE_MAGE:
				fMinDamage = fINT / 2.5f * 1.0f + fAttackMinDmg;
				fMaxDamage = fINT / 2.5f * 1.05f + fAttackMaxDmg;
				break;

			default:
				fMinDamage	= fSTR / 3.0f * 1.0f + fAttackMinDmg;
				fMaxDamage	= fSTR / 3.0f * 1.05f + fAttackMaxDmg;
				break;
		}

		if(bIsSpec)
		{
			// UI ������ �����ִ� ���̶�� �׳� �ٷ� ����
			if(bIsMin)
				return (INT32)fMinDamage;
			else
				return (INT32)fMaxDamage;
		}
		else
		{
			// �ƴ϶�� ���������� ����Ѵ�.
			eAgpmSkillWeaponDamageType eDamageType = m_pcsAgpmSkill->GetWeaponDamageType(pcsAttackChar);

			if(eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN)
			{
				fMaxDamage	= fMinDamage;
			}
			else if(eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX)
			{
				fMinDamage	= fMaxDamage;
			}

			//return (INT32)((fMinDamage + (FLOAT)m_csRandom.randInt((INT32)(fMaxDamage - fMinDamage))
			//				* (1.0f - (FLOAT)(fPhysicalResistance / 100.0f)))
			//				- fPhysicalDefense);

			// 2005.11.08. steeple
			// ���� ������ ����� ���� �ʾƼ� �Ʒ��� ���� �˾ƺ��� ���� �и��Ͽ���.
			FLOAT fRandDamage = (FLOAT)m_csRandom.randInt((INT32)(fMaxDamage - fMinDamage));
			FLOAT fFinalDamage = fMinDamage + fRandDamage;
			
			// Defense �� ���� �� �� Resistance ����
			fFinalDamage -= fPhysicalDefense;

			FLOAT fResi = 1.0f - (FLOAT)(fPhysicalResistance / 100.0f);
			fFinalDamage *= fResi;

			return (INT32)fFinalDamage;
		}
	}
	else
	{
		// 2005.06.07. steeple
		// �ƴ� �Ʒ� �ּ��� ����ü ����.........
		// �� ������ FACTOR �� RESULT �� �ƴ� �׳� ������ -_-;;;;;;;;;;;
		//
		//
		//
		// ����
		// ���� ������ ������ PC �� ���� ����. (FACTOR �� RESULT �� �ƴ� �׳� ��� ���� �ٸ�)
		FLOAT		fAttackMinDmg		= 0.0f;
		FLOAT		fAttackMaxDmg		= 0.0f;

		// 2005.06.07. steeple
		// Result ���� ��� �ɷ� ����
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMinDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMaxDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

		eAgpmSkillWeaponDamageType	eDamageType	= m_pcsAgpmSkill->GetWeaponDamageType(pcsAttackChar);

		FLOAT fFinalDamage = 0.0f;

		if(eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN)
			fFinalDamage = fAttackMinDmg - fPhysicalDefense;
		else if (eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX)
			fFinalDamage = fAttackMaxDmg - fPhysicalDefense;
		else
		{
			//return (INT32)((fAttackMinDmg + (FLOAT)m_csRandom.randInt((INT32)(fAttackMaxDmg - fAttackMinDmg))
			//				* (1.0f - (FLOAT)(fPhysicalResistance / 100.0f)))
			//				- fPhysicalDefense);

			// 2005.11.08. steeple
			// ���� ������ ����� ���� �ʾƼ� �Ʒ��� ���� �˾ƺ��� ���� �и��Ͽ���.
			FLOAT fRandDamage = (FLOAT)m_csRandom.randInt((INT32)(fAttackMaxDmg - fAttackMinDmg));
			fFinalDamage = fAttackMinDmg + fRandDamage;
			
			// Defense �� ���� �� �� Resistance ����
			fFinalDamage -= fPhysicalDefense;
		}
		FLOAT fResi = 1.0f - (FLOAT)(fPhysicalResistance / 100.0f);
		fFinalDamage *= fResi;

		return (INT32)fFinalDamage;
	}

	return 0;
}

//		CalcHeroicAttack
//	Functions
//		- ���� ���� �������� ������ ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- INT32 : ���� ��� ������
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcHeroicAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec, BOOL bIsMin, int nCombatPrecheck)
{
	if(NULL == pcsAttackChar || NULL == pcsTargetChar)
		return 0;

	FLOAT		fHeroicAttackMaxDmg = 0.0f;
	FLOAT		fHeroicAttackMinDmg = 0.0f;
	FLOAT		fHeroicDefense		= 0.0f;
	FLOAT		fHeroicResistance	= 0.0f;

	// Calc Attacker's Heroic Parameter(Attack)
	fHeroicAttackMaxDmg				= GetHeroicDamageMax(pcsAttackChar);
	fHeroicAttackMinDmg				= GetHeroicDamageMin(pcsAttackChar);

	// Calc Target's Heroic Parameter(Defense)
	fHeroicDefense					= GetHeroicDefense(pcsTargetChar);

	// Calc Target's Heroic Parameter(RoleType Resistance)
	fHeroicResistance				= GetHeroicResistanceByAttackerHeroicClass(pcsAttackChar, pcsTargetChar);

	eAgpmSkillWeaponDamageType eDamage = m_pcsAgpmSkill->GetWeaponDamageType(pcsAttackChar);

	FLOAT		fMinDamage			= fHeroicAttackMaxDmg;
	FLOAT		fMaxDamage			= fHeroicAttackMinDmg;

	if(eDamage == AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN)
	{
		fMaxDamage = fMinDamage;
	}
	else if(eDamage == AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX)
	{
		fMinDamage = fMaxDamage;
	}

	FLOAT fRandomDamage = (FLOAT)m_csRandom.randInt((INT32)(fMaxDamage - fMinDamage));
	FLOAT fFinalDamage  = fMinDamage + fRandomDamage;

	// Sub Target's Heroic Defense Point
	fFinalDamage -= fHeroicDefense;

	// Calc Heroic Resistance
	FLOAT fResistance = 1.0f - (FLOAT)(fHeroicResistance / 100);
	fFinalDamage *= fResistance;

	return (INT32)fFinalDamage;
}

//		CalcHeroicAttackForSkill
//	Functions
//		- ���� ���� �������� ������ ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- INT32 : ���� ��� ������
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcHeroicAttackForSkill(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec /* = FALSE */)
{
	if(NULL == pcsAttackChar || NULL == pcsTargetChar)
		return 0;

	// �Ϲݰ��ݿ����� �������� ���� ���Ƽ�� �̹� PhysicalAttack���� ����Ͽ����� 
	// ��ų���� ���߱⶧���� ��ų�ʿ��� �г�Ƽ ����� ���� ���ش�.
	INT32 lPenalty		= CalcDefPenaltyLevelGap(pcsAttackChar, pcsTargetChar);
	INT32 lFinalDamage	= CalcHeroicAttack(pcsAttackChar, pcsTargetChar, bIsEquipWeapon);
	if(lFinalDamage <= 0)
	{
		lFinalDamage = m_csRandom.randInt(1) + 1;
	}
	
	// �г�Ƽ ������ �� 0 �Ǵ� �������� ������ �ּҴ�������� ���� Damage�� ����Ѵ�.
	lFinalDamage -= lPenalty;
	if(lFinalDamage <= 0)
	{
		lFinalDamage = m_csRandom.randInt(1) + 1;
	}

	return lFinalDamage;
}


INT32 AgpmCombat::CalcBasePhysicalAttack(AgpdCharacter *pcsAttackChar, BOOL bIsMin)
{
	if (!pcsAttackChar)
		return 0;

	INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsAttackChar);
	if (lLevel < 1 || lLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate	*pcsOriginalTemplate	= NULL;
	if (pcsAttackChar->m_bIsTrasform)
		pcsOriginalTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(pcsAttackChar->m_lOriginalTID);
	else
		pcsOriginalTemplate	= pcsAttackChar->m_pcsCharacterTemplate;

	INT32		lAttackerClass		= (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csFactor, &lAttackerClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	FLOAT fAttackMinDmg = 0.0f;
	FLOAT fAttackMaxDmg = 0.0f;

	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fAttackMinDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fAttackMaxDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	FLOAT fSTR, fDEX, fINT;;
	fSTR = fDEX = fINT = 0.0f;

	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fSTR, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fDEX, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fINT, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

	FLOAT fMinDamage = 0.0f;
	FLOAT fMaxDamage = 0.0f;

	switch((AuCharClassType)lAttackerClass)
	{
		// 2005.04.09. steeple
		// �ü��� STR, DEX �� �� ����ϰ� Ÿ Ŭ������ STR �� ����.
		case AUCHARCLASS_TYPE_RANGER:
			fMinDamage = ((fSTR / 9.0f) + (fDEX / 4.5f)) * 0.95f + fAttackMinDmg;
			fMaxDamage = ((fSTR / 9.0f) + (fDEX / 4.5f)) * 1.05f + fAttackMaxDmg;
			break;

		// 2005.11.30. steeple
		// ���� ���� ��3
		case AUCHARCLASS_TYPE_MAGE:
			fMinDamage = fINT / 2.5f * 1.0f + fAttackMinDmg;
			fMaxDamage = fINT / 2.5f * 1.05f + fAttackMaxDmg;
			break;

		default:
			fMinDamage	= fSTR / 3.0f * 0.95f + fAttackMinDmg;
			fMaxDamage	= fSTR / 3.0f * 1.05f + fAttackMaxDmg;
			break;
	}

	if(bIsMin)
		return (INT32)fMinDamage;
	else
		return (INT32)fMaxDamage;
}

// 2004.12.29. steeple
// ��ų�� ���� �������� �⺻ ������ INT �� ����ϱ� ������ �и��Ͽ���.
// 2005.01.15. steeple
// �߿�!!! ���⼭�� �������� Target �� PhysicalDefense �� ���� �ʴ´�.
INT32 AgpmCombat::CalcPhysicalAttackForSkill(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar, BOOL bIsEquipWeapon, BOOL bIsSpec)
{
	if(!pcsAttackChar)
		return 0;

	// ���Ͷ��
	if(!m_pcsAgpmCharacter->IsPC(pcsAttackChar))
		return CalcPhysicalAttack(pcsAttackChar, pcsTargetChar, bIsEquipWeapon);

	INT32 lAttackerClass = (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &lAttackerClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	FLOAT fAttackFactor = 0.0f;
	FLOAT fDivideValue = 0.0f;
	switch((AuCharClassType)lAttackerClass)
	{
		case AUCHARCLASS_TYPE_MAGE:
		{
			m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
			fDivideValue = 2.5f;
			break;
		}

		default:
		{
			// �����簡 �ƴ϶�� ���� ���Ŀ��� �������� ���´�.
			INT32 lDamage = CalcPhysicalAttack(pcsAttackChar, pcsTargetChar, bIsEquipWeapon, bIsSpec, FALSE);
			FLOAT fPhysicalDefense = GetPhysicalDefense(pcsTargetChar);

			// ���� ���Ŀ����� fPhysicalDefense �� ���� ���ϵǹǷ� �����ش�.
			return lDamage + (INT32)fPhysicalDefense;
			break;
		}
	}

	if(fDivideValue == 0.0f)
		fDivideValue = 2.5f;

	// ������� ������ �� ������ ��ų �⺻ ���ݷ��� �����ش�.
	FLOAT fAttackMinDmg = 0.0f;
	FLOAT fAttackMaxDmg	= 0.0f;

	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMinDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttackMaxDmg, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	FLOAT fMinDamage = 0.0f;
	FLOAT fMaxDamage = 0.0f;

	eAgpmSkillWeaponDamageType eDamageType = m_pcsAgpmSkill->GetWeaponDamageType(pcsAttackChar);

	if(eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN)
	{
		fMinDamage	= fAttackFactor / fDivideValue * 0.95f + fAttackMinDmg;
		fMaxDamage	= fMinDamage;
	}
	else if(eDamageType == AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX)
	{
		fMaxDamage	= fAttackFactor / fDivideValue * 1.05f + fAttackMaxDmg;
		fMinDamage	= fMaxDamage;
	}
	else
	{
		fMinDamage	= fAttackFactor / fDivideValue * 0.95f + fAttackMinDmg;
		fMaxDamage	= fAttackFactor / fDivideValue * 1.05f + fAttackMaxDmg;
	}

	// �߿�!!!! �Ϲ� ���� ���İ��� �ٸ��� ������� PhysicalDefense �� ���� �ʴ´�.
	// �� Heroic ������ ���־�� �Ѵ�.
	FLOAT fHeroicDefense = GetHeroicDefense(pcsTargetChar);

	if(bIsSpec)
		return (INT32) fMaxDamage;
	else
		return (INT32) ((fMinDamage + m_csRandom.randInt((INT32) (fMaxDamage - fMinDamage))) - fHeroicDefense); // - fPhysicalDefense);

	return 0;
}

INT32 AgpmCombat::CalcBasePhysicalAttackForSkill(AgpdCharacter* pcsAttackChar)
{
	if(!pcsAttackChar)
		return 0;
	
	INT32	lLevel	= m_pcsAgpmCharacter->GetLevel(pcsAttackChar);
	if (lLevel < 1 || lLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	AgpdCharacterTemplate	*pcsOriginalTemplate	= NULL;
	if (pcsAttackChar->m_bIsTrasform)
		pcsOriginalTemplate	= m_pcsAgpmCharacter->GetCharacterTemplate(pcsAttackChar->m_lOriginalTID);
	else
		pcsOriginalTemplate	= pcsAttackChar->m_pcsCharacterTemplate;


	INT32 lAttackerClass = (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csFactor, &lAttackerClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	FLOAT fAttackFactor = 0.0f;
	FLOAT fDivideValue = 0.0f;
	switch((AuCharClassType)lAttackerClass)
	{
		case AUCHARCLASS_TYPE_MAGE:
		{
			m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fAttackFactor, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
			fDivideValue = 2.5f;
			break;
		}

		default:
		{
			return CalcBasePhysicalAttack(pcsAttackChar, FALSE);
			break;
		}
	}

	if(fDivideValue == 0.0f)
		fDivideValue = 2.5f;

	// ������� ������ �� ������ ��ų �⺻ ���ݷ��� �����ش�.
	FLOAT fAttackMaxDmg	= 0.0f;

	m_pcsAgpmFactors->GetValue(&pcsOriginalTemplate->m_csLevelFactor[lLevel], &fAttackMaxDmg, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	return INT32( fAttackFactor / fDivideValue * 1.05f + fAttackMaxDmg );
}

//	2005.11.22. steeple
//	1��, 2�� ���ɼ��� ���յǾ���. -0-;;;
//
//
//		CalcFirstSpiritAttack
//	Functions
//		- 1�� ���ɼ��� ���� ������ �������� ��� ���ɼ� �Ӽ��� ���� �������� ����Ѵ�.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//	Return value
//		- INT32 : ���� ��� ������
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcFirstSpiritAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType, BOOL bIsSpec, BOOL bIsMin, BOOL bForSkill, int nCombatPreCheck)
{
	if (!pcsAttackChar ||
		lAttributeType <= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)		// 1��, 2�� ����
		//lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_AIR)
		return 0;

	FLOAT		fAttrDamage		= 0.0f;
	FLOAT		fAttrDefense	= 0.0f;

	// calculate Penalty For Level Gap
	INT32		lPenalty		= CalcDefPenaltyLevelGap(pcsAttackChar, pcsTargetChar);

	// calculate damage
	//m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttrDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

	if (m_pcsAgpmCharacter->IsPC(pcsAttackChar))
	{
		/*
		AgpdItem	*pcsWeapon			= m_pcsAgpmItem->GetEquipWeapon(pcsAttackChar);
		if (!pcsWeapon ||
			!pcsWeapon->m_pcsItemTemplate)
			return 0;
		*/

		FLOAT		fMinDamage			= 0.0f;
		FLOAT		fMaxDamage			= 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fMinDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, lAttributeType);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fMaxDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

		// Ran(MaxDMG - MinDMG) - (MaxDMG - MinDMG) / 2 +
		//	(((A ^ 1.46) * 7 * 0.069 + 14) * 0.2) ^ 1.68 * WeaponTypeValue
		//	(((A ^ 1.46) * (3.4 + 3.6 * IF(A > 80, 1 + (A - 80) * 0.01, 1)) * 0.069 + 14) * 0.2) ^ 1.68 * WeaponTypeValue
		//
		//		A : Weapon Require Level + # of SpiritStone * 2
		///////////////////////////////////////////////////////////////////////////////////////////////////////////

		if (bIsSpec)
		{
			if (bIsMin)
			{
				//fAttrDamage	+= CalcItemFirstSpiritAttack(pcsWeapon, lAttributeType, TRUE);
				fAttrDamage	+= fMinDamage;
			}
			else
			{
				//fAttrDamage	+= CalcItemFirstSpiritAttack(pcsWeapon, lAttributeType, FALSE);
				fAttrDamage	+= fMaxDamage;
			}
		}
		else
		{
			//FLOAT	fMinDamage	= CalcItemFirstSpiritAttack(pcsWeapon, lAttributeType, TRUE);
			//FLOAT	fMaxDamage	= CalcItemFirstSpiritAttack(pcsWeapon, lAttributeType, FALSE);

			fAttrDamage	+= fMinDamage + m_csRandom.randInt((INT32) (fMaxDamage - fMinDamage));
		}
	}
	else
	{
		FLOAT	fMinAttrDamage	= 0.0f;
		FLOAT	fMaxAttrDamage	= 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fMinAttrDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, lAttributeType);
		m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fMaxAttrDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

		if (fMinAttrDamage >= fMaxAttrDamage)
			fAttrDamage	= fMaxAttrDamage;
		else
			fAttrDamage = fMinAttrDamage + m_csRandom.randInt((INT32) (fMaxAttrDamage - fMinAttrDamage));
	}

	if (pcsTargetChar)
	{
		fAttrDefense = (FLOAT)CalcFirstSpiritDefense(pcsTargetChar, lAttributeType) - lPenalty;

		if(fAttrDefense < 0) fAttrDefense = 0;
	}

	if(nCombatPreCheck & AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_ATTRIBUTE_DEFENCE)
		fAttrDefense = 0;

	// 2005.01.15. steeple ����
	INT32	lFinalDamage	= 0;
	if(!bForSkill && !bIsSpec)
		lFinalDamage = (INT32) (fAttrDamage * (100 - fAttrDefense) / 100.0f * 2);
	else
		lFinalDamage = (INT32) fAttrDamage;

	// 2007.07.04. steeple
	// �Ʒ� ���� 0 ���ϰ� �� �� �ִ�.
	//
	//if (lFinalDamage < 0)
	//	lFinalDamage = 0;

	return lFinalDamage;
}

INT32 AgpmCombat::CalcSecondSpiritAttack(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, INT32 lAttributeType, BOOL bIsSpec, BOOL bIsMin, BOOL bForSkill)
{
	if (!pcsAttackChar ||
		lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_POISON ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	FLOAT		fAttrDamage		= 0.0f;
	m_pcsAgpmFactors->GetValue(&pcsAttackChar->m_csFactor, &fAttrDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

	return (INT32) fAttrDamage;
}

INT32 AgpmCombat::CalcItemFirstSpiritAttack(AgpdItem *pcsItem, INT32 lAttributeType, BOOL bIsMin)
{
	if (!pcsItem ||
		!pcsItem->m_pcsItemTemplate ||
		lAttributeType <= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	INT32	lDamage	= 0;

	if (bIsMin)
		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, lAttributeType);
	else
		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

	return lDamage;

	/*
	INT32		lNumConvertAttr		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, lAttributeType);
	if (lNumConvertAttr < 1)
		return 0;

	INT32		lRank				= 0;

	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	FLOAT		fWeaponTypeValue	= GetWeaponTypeValue(pcsItem);

	FLOAT		fDamage				= 0;
	
	INT32		lRequireLevel		= 0;
	m_pcsAgpmFactors->GetValue(&((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_csRestrictFactor, &lRequireLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	if (bIsMin)
	{
		fDamage	= ((((pow(lNumConvertAttr, 2) + lNumConvertAttr) * 0.7) + (2.85 * pow((lNumConvertAttr + 0.5), 1.1))) * (lRank * 2 - 1) * fWeaponTypeValue + lRequireLevel) * 0.8;
	}
	else
	{
		fDamage	= ((((pow(lNumConvertAttr, 2) + lNumConvertAttr) * 0.7) + (2.85 * pow((lNumConvertAttr + 0.5), 1.1))) * (lRank * 2 - 1) * fWeaponTypeValue + lRequireLevel) * 1.2;
	}

	return (INT32) fDamage;
	*/

	/*
	INT32		lRequireLevel		= 0;

	m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lRequireLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	INT32		lTempAValue			= lRequireLevel + lNumConvertAttr * 2;

	// Ran(MaxDMG - MinDMG) - (MaxDMG - MinDMG) / 2 +
	//	(((A ^ 1.46) * 7 * 0.069 + 14) * 0.2) ^ 1.68 * WeaponTypeValue
	//	(((A ^ 1.46) * (3.4 + 3.6 * IF(A > 80, 1 + (A - 80) * 0.01, 1)) * 0.069 + 14) * 0.2) ^ 1.68 * WeaponTypeValue
	//
	//		A : Weapon Require Level + # of SpiritStone * 2
	///////////////////////////////////////////////////////////////////////////////////////////////////////////

	FLOAT		fTempBValue			= pow(lTempAValue, 1.46);
	FLOAT		fTempCValue			= (fTempBValue
										* (7) * 0.069 + 14)
//											* (3.4 + 3.6 * ((lTempAValue > 80) ? (1 + (lTempAValue - 80) * 0.01) : 1)) * 0.069 + 14)
										* 0.2;
	FLOAT		fTempDValue			= pow(fTempCValue, 1.68);

	return fTempDValue * fWeaponTypeValue;
	*/
}

// 2005.11.22. steeple
// 1�� 2�� �������� ������ ���� ó���ϰ� ����Ǿ���. �� �ƴ϶� ���� AGPD_FACTORS_ATTRIBUTE_TYPE_ICE ���� üũ�߾���!!!
INT32 AgpmCombat::CalcFirstSpiritDefense(AgpdCharacter *pcsCharacter, INT32 lAttributeType)
{
	if (!pcsCharacter ||
		lAttributeType <= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	FLOAT	fDefense	= 0.0f;

	m_pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &fDefense, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, lAttributeType);

	// 2006.01.16. steeple
	// PC ��� 80 �� �ְ�ġ�̴�.
	if(m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return fDefense > 80.0f ? 80 : (INT32)fDefense;
	else
		return (INT32)fDefense;
}

INT32 AgpmCombat::CalcItemFirstSpiritDefense(AgpdItem *pcsItem, INT32 lAttributeType)
{
	if (!pcsItem || !pcsItem->m_pcsItemTemplate || !pcsItem->m_pcsCharacter)
		return 0;

	INT32	lDefense	= 0;

	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDefense, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, lAttributeType);

	return lDefense;

	/*
	if (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
		return 0;

	INT32		lNumConvertAttr		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsItem, lAttributeType);
	if (lNumConvertAttr < 1)
		return 0;

	INT32		lCharacterClass		= (INT32) AUCHARCLASS_TYPE_NONE;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lCharacterClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

	INT32		lRank				= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);

	INT32		lRequireLevel		= 0;
	m_pcsAgpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lRequireLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	switch ((AuCharClassType) lCharacterClass) {
	case AUCHARCLASS_TYPE_KNIGHT:
		{
			return (INT32) ((((4 * (pow(lNumConvertAttr, 2)) - 0.25) / 7.0) * 0.7 + (2.85 * (pow((2 * lNumConvertAttr), 1.1) / 7.0))) * (lRank * 2 - 1) + lRequireLevel / 7.0);
		}
		break;

	case AUCHARCLASS_TYPE_MAGE:
	case AUCHARCLASS_TYPE_RANGER:
	case AUCHARCLASS_TYPE_SCION:
		{
			return (INT32) ((((4 * (pow(lNumConvertAttr, 2)) - 0.25) / 6.0) * 0.7 + (2.85 * (pow((2 * lNumConvertAttr), 1.1) / 6.0))) * (lRank * 2 - 1) + lRequireLevel / 6.0);
		}
		break;
	}
	*/

	/*
	switch ((AuCharClassType) lCharacterClass) {
	case AUCHARCLASS_TYPE_KNIGHT:
		{
			return (INT32) ((((4 * (pow(lNumConvertAttr, 2)) - 0.25) / 7.0) * 0.7 + (pcsItem->m_stConvertHistory.fConvertConstant * (pow((2 * lNumConvertAttr), 1.1) / 7.0))) * (lRank * 2 - 1) + lRequireLevel / 7.0);
		}
		break;

	case AUCHARCLASS_TYPE_MAGE:
	case AUCHARCLASS_TYPE_RANGER:
	case AUCHARCLASS_TYPE_SCION:
		{
			return (INT32) ((((4 * (pow(lNumConvertAttr, 2)) - 0.25) / 6.0) * 0.7 + (pcsItem->m_stConvertHistory.fConvertConstant * (pow((2 * lNumConvertAttr), 1.1) / 6.0))) * (lRank * 2 - 1) + lRequireLevel / 6.0);
		}
		break;
	}
	*/

	return 0;
}

INT32 AgpmCombat::CalcItemSecondSpiritAttack(AgpdItem *pcsItem, INT32 lAttributeType, BOOL bIsMin)
{
	if (!pcsItem ||
		!pcsItem->m_pcsItemTemplate ||
		lAttributeType <= AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL ||
		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
		return 0;

	INT32	lDamage	= 0;

	if (bIsMin)
		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, lAttributeType);
	else
		m_pcsAgpmFactors->GetValue(&pcsItem->m_csFactor, &lDamage, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, lAttributeType);

	return lDamage;
}

FLOAT AgpmCombat::GetWeaponTypeValue(AgpdItem *pcsItem)
{
	if (!pcsItem ||
		!pcsItem->m_pcsItemTemplate ||
		((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP ||
		((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON)
		return 0;

	switch (((AgpdItemTemplateEquipWeapon *) pcsItem->m_pcsItemTemplate)->m_nWeaponType) {
	case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_SWORD:
	case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_AXE:
		return (FLOAT) 1.0;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_MACE:
	case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_HAMMER:
		return (FLOAT) 1.0641;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SWORD:
	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_AXE:
	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_MACE:
	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_HAMMER:
	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_POLEARM:
	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_SCYTHE:
		return (FLOAT) 1.1857;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_STAFF:
		return (FLOAT) 1.3175;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_TROPHY:
		return (FLOAT) 1.537;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WAND:
		return (FLOAT) 1.844;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:
		return (FLOAT) 0.6385;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:
		return (FLOAT) 0.83;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CLAW:
		return (FLOAT) 0.051;
		break;

	case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_WING:
		return (FLOAT) 0.8058;
		break;
	}

	return 0;
}

FLOAT AgpmCombat::GetCharacterTypeValue(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter ||
		!m_pcsAgpmCharacter->IsPC(pcsCharacter))
		return 0;

	switch (m_pcsAgpmFactors->GetClass(&pcsCharacter->m_csFactor)) {
	case AUCHARCLASS_TYPE_KNIGHT:
		return (FLOAT) (8.5 / 7.5);
		break;

	case AUCHARCLASS_TYPE_RANGER:
		return (FLOAT) (5.25 / 7.5);
		break;

	case AUCHARCLASS_TYPE_SCION:
		return (FLOAT) (6 / 7.6);
		break;

	case AUCHARCLASS_TYPE_MAGE:
		return (FLOAT) (11.83 / 7.5);
		break;
	}

	return 0;
}

BOOL AgpmCombat::GetArmourAttributeInfo(AgpdCharacter *pcsCharacter, INT32 *plMinRequireLevel, INT32 *plNumConvertAttr, INT32 lAttributeType)
{
	if (!pcsCharacter ||
		!plMinRequireLevel ||
		!plNumConvertAttr)
		return FALSE;

	*plMinRequireLevel	= AGPMCHAR_MAX_LEVEL + 10;
	*plNumConvertAttr	= 0;

	for (int i = AGPMITEM_PART_BODY; i < AGPMITEM_PART_ARMS2; ++i)
	{
		AgpdItem	*pcsEquipItem	= m_pcsAgpmItem->GetEquipSlotItem(pcsCharacter, (AgpmItemPart) i);
		if (pcsEquipItem)
		{
			INT32	lNumConvert		= m_pcsAgpmItemConvert->GetNumConvertAttr(pcsEquipItem, lAttributeType);
			if (lNumConvert < 1)
				continue;

			*plNumConvertAttr		+= lNumConvert;

			INT32	lRequireLevel	= AGPMCHAR_MAX_LEVEL;

			m_pcsAgpmFactors->GetValue(&pcsEquipItem->m_csRestrictFactor, &lRequireLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

			if (lRequireLevel < *plMinRequireLevel)
				*plMinRequireLevel	= lRequireLevel;
		}
	}

	if (*plMinRequireLevel == AGPMCHAR_MAX_LEVEL + 10)
		*plMinRequireLevel	= 0;

	return TRUE;
}

//		ProcessCombat
//	Functions
//		- ĳ���Ͱ� ������ ó���Ѵ�.
//			1. ���� ������ �����Ѵ�.
//			2. ���� ������ ���� Factor���� �����ؼ� Damage�� �����س���.
//			3. ����� �ݿ��ϰ� �ش� ĳ���͵�� �����͸� ������.
//	Arguments
//		- pcsAttackChar : ���� ĳ����
//		- pcsTargetChar : ���� ��� ĳ����
//		- pcsUpdateFactor : �������� ������ ���� ������
//		- pabIsSuccessSecondSpirit : 2�� ���ɼ��� ���� �Ӽ����� ���� ���� (array size : AGPD_FACTORS_ATTRIBUTE_MAX_TYPE)
//	Return value
//		- INT32 : Total Damage
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::ProcessCombat(AgpdCharacter *pcsAttackChar, AgpdCharacter *pcsTargetChar, AgpdFactor *pcsUpdateFactor, BOOL *pabIsSuccessSecondSpirit, AgpdCharacterActionResultType *pstResult, BOOL bIsEquipWeapon, BOOL bIsAttrInvincible, INT32 *pHeroicDamage, int nCombatPreCheck)
{
	if (!pcsAttackChar || !pcsTargetChar || !pcsUpdateFactor || !pabIsSuccessSecondSpirit || !pstResult || !pHeroicDamage)
		return 0;

	// �������� ����Ѵ�.
	///////////////////////////////////////////////////////////////////

	INT32	lTotalDamage	= 0;
	INT32	lMeleeDamage	= 0;

	ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lSpiritDamage;
	lSpiritDamage.MemSetAll();

	// ���� �и� ����
	///////////////////////////////////////////////////////////////////
	if (!IsShieldDefenseSuccess(pcsAttackChar, pcsTargetChar))
	{
		if (IsAttackSuccess(pcsAttackChar, pcsTargetChar))
		{
			lMeleeDamage = CalcPhysicalAttack(pcsAttackChar, pcsTargetChar, bIsEquipWeapon, FALSE, FALSE, nCombatPreCheck);
			if (lMeleeDamage <= 0)
			{
				lMeleeDamage	= m_csRandom.randInt(1) + 1;
				/*
				*pstResult = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;
				return 0;
				*/
			}
			
			*pstResult = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;
		}
		else
		{
			*pstResult = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;
			return 0;
		}
	}
	else
	{
		*pstResult = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK;
		return 0;
	}

	/*
	if (lMeleeDamage < 1)
		lMeleeDamage = 100;
	*/

	lTotalDamage	+=	lMeleeDamage;

	// 2007.07.10. steeple
	// �Ӽ������� �ƴ� ���� �Ʒ� ������ ó�����ش�.
	if(bIsAttrInvincible == FALSE)
	{
		// 1�� ���ɼ��� ���� ������ ���
		///////////////////////////////////////////////////////////////////
		for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_AIR; ++i)
		{
			if (IsFirstSpiritAttackSuccess(pcsAttackChar, pcsTargetChar, i))
			{
				lSpiritDamage[i] = CalcFirstSpiritAttack(pcsAttackChar, pcsTargetChar, i, FALSE, FALSE, FALSE, nCombatPreCheck);
				if(lSpiritDamage[i] <= 0)
					lSpiritDamage[i] = 0;

				lTotalDamage += lSpiritDamage[i];
			}
		}

		// 2�� ���ɼ��� ���� ���߿��� �˻�
		///////////////////////////////////////////////////////////////////
		for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_POISON; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		{
			pabIsSuccessSecondSpirit[i] = IsSecondSpiritAttackSuccess(pcsAttackChar, pcsTargetChar, i);
			if (pabIsSuccessSecondSpirit[i])
			{
				// ó�� ����� 1��, 2�� �� ��������. 2005.11.22. steeple
				lSpiritDamage[i] = CalcFirstSpiritAttack(pcsAttackChar, pcsTargetChar, i, nCombatPreCheck);
				if(lSpiritDamage[i] <= 0)
					lSpiritDamage[i] = 0;
				//lSpiritDamage[i] = CalcSecondSpiritAttack(pcsAttackChar, pcsTargetChar, i);
				lTotalDamage += lSpiritDamage[i];
			}
		}
	}

	INT32 lHeroicDamage = CalcHeroicAttack(pcsAttackChar, pcsTargetChar, bIsEquipWeapon, FALSE, FALSE, nCombatPreCheck);
	if(lHeroicDamage <= 0)
	{
		lHeroicDamage = m_csRandom.randInt(1) + 1;
	}
	else
	{
		*pHeroicDamage = lHeroicDamage;
	}

	// calculate Final Damage Including heroic damage
	lTotalDamage += lHeroicDamage;

	// Last, Sub Heroic Defense to totaldamage
	INT32 lHeroicDefense = (INT32)GetHeroicDefense(pcsTargetChar);
	if(lHeroicDefense > 0)
	{
		lTotalDamage -= lHeroicDefense;
	}

	if (lTotalDamage > 0)
	{
		m_pcsAgpmFactors->InitFactor(pcsUpdateFactor);

		// ���⼭ ������ ���� �������� ���� ĳ������ status�� �ݿ��Ѵ�.
		AgpdFactor			*pcsUpdateFactorResult = (AgpdFactor *) m_pcsAgpmFactors->SetFactor(pcsUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
		if (!pcsUpdateFactorResult)
			return (0);

		AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pcsAgpmFactors->InitCalcFactor(pcsUpdateFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
		{
			m_pcsAgpmFactors->DestroyFactor(pcsUpdateFactor);

			return (0);
		}

		m_pcsAgpmFactors->SetValue(pcsUpdateFactor, (-lTotalDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

		if (lMeleeDamage > 0)
			m_pcsAgpmFactors->SetValue(pcsUpdateFactor, (-lMeleeDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

		if (lHeroicDamage > 0)
			m_pcsAgpmFactors->SetValue(pcsUpdateFactor, (-lHeroicDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC);

		int	j = 0;
		for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		{
			if (lSpiritDamage[i] > 0)
				m_pcsAgpmFactors->SetValue(pcsUpdateFactor, (-lSpiritDamage[i]), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC + j);

			++j;
		}
	}

	return lTotalDamage ;
}

// 2005.12.22. steeple
// Evade ������ �� üũ�� �Ѵ�.
// Evade ���� �ÿ��� AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE �� return
// Dodge ���� �ÿ��� AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE �� return
// �ƹ��͵� �ƴ� ���� AGPDCHAR_ACTION_RESULT_TYPE_NONE �� return
AgpdCharacterActionResultType AgpmCombat::ProcessEvade(AgpdCharacter* pcsAttackChar, AgpdCharacter* pcsTargetChar)
{
	if(!pcsAttackChar || !pcsTargetChar)
		return AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	INT32 lRand = m_csRandom.randInt(100);

	INT32 lPenalty = CalcDefPenaltyLevelGap(pcsAttackChar, pcsTargetChar);

	// 2007.12.05. steeple
	// Range Type Check �߰��Ͽ���.
	//
	// �������� ���� üũ
	if(pcsAttackChar->m_pcsCharacterTemplate->IsRangeCharacter() ||
		m_pcsAgpmItem->IsEquipStaff(pcsAttackChar) ||
		m_pcsAgpmItem->IsEquipWand(pcsAttackChar) ||
		m_pcsAgpmItem->IsEquipBow(pcsAttackChar) ||
		m_pcsAgpmItem->IsEquipCrossBow(pcsAttackChar) ||
		m_pcsAgpmItem->IsEquipKatariya(pcsAttackChar) ||
		m_pcsAgpmItem->IsEquipChakram(pcsAttackChar))
	{
		// Dodge üũ
		INT32 lDodgeRate = GetAdditionalDodgeRate(pcsTargetChar) - lPenalty;
		if(lDodgeRate > 0 && lDodgeRate >= lRand)
			return AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_DODGE;
	}
	else
	{
		// Evade üũ
		INT32 lEvadeRate = GetAdditionalEvadeRate(pcsTargetChar) - lPenalty;
		if(lEvadeRate > 0 && lEvadeRate >= lRand)
			return AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE;
	}

	return AGPDCHAR_ACTION_RESULT_TYPE_NONE;
}

//		CalcPenaltyExp
//	Functions
//		- ���� ĳ���Ϳ� ���� Penalty Exp ���� ó���� ���⼭ ����Ѵ�.
//	Arguments
//		- pcsFactor : ������ factor
//		- pvPacket : ����� factor�� ���� ��Ŷ
//	Return value
//		- INT16 : �������� ��Ƹ��� Exp
///////////////////////////////////////////////////////////////////////////////
INT16 AgpmCombat::CalcPenaltyExp(AgpdFactor *pcsFactor, PVOID *pvPacket)
{
	if (!pcsFactor || !pvPacket)
		return (-1);

	INT16	nPenaltyExp = 0;

	// �� ĳ������ Level, ���� Exp�� �����´�.
	AgpdFactorCharStatus	*pcsFactorCharStatus	= (AgpdFactorCharStatus *)	m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);
	if (!pcsFactorCharStatus)
		return (-1);
	
	AgpdFactorCharPoint		*pcsFactorCharPoint		= (AgpdFactorCharPoint *)	m_pcsAgpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
		return (-1);

	// ���� ������ �ö󰡱� ���� ����ġ�� 1%�� ��´�.
	//
	//		������ ���� ������ �ö󰡱� ���� ����ġ�� ������ �𸥴�.
	//
	//

	// Exp�� ������Ʈ ��Ű�� ��Ŷ�� �ѱ��.
	//
	//

	return nPenaltyExp;
}

//		CalcDefPenaltyExp
//	Functions
//		- ���� ���̿� ���� Target�� ��� Ȯ�� ���Ƽ�� ó���Ѵ�.
//	Arguments
//		- pcsAttacker	: ������
//		- pcsTargetCahr : ���ݴ�����
//	Return value
//		- INT16 : �ش�Ǵ� Penalty
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcDefPenaltyLevelGap(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar)
{
	if(!pcsAttacker || !pcsTargetChar || !m_pcsAgpmCharacter)
		return 0;

	if(!m_pcsAgpmCharacter->IsPC(pcsAttacker) && !m_pcsAgpmCharacter->IsPC(pcsTargetChar))
		return 0;

	INT32	lAttackerLevel	 = 0;
	INT32	lTargetCharLevel = 0;
	INT32	lLevelGap		 = 0;
	INT32	lPenaltyPercent	 = 0;

	lAttackerLevel   = m_pcsAgpmCharacter->GetLevel(pcsAttacker);
	lTargetCharLevel = m_pcsAgpmCharacter->GetLevel(pcsTargetChar);

	lLevelGap		 = lAttackerLevel - lTargetCharLevel;

	if(lLevelGap > 0)
	{
		if (lLevelGap > 1 && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_ONE)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_ONE;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_ONE && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_TWO)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_TWO;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_TWO && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_THREE)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_THREE;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_THREE && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_FIVE)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_FOUR;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_FIVE && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_SIX)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_SIX;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_SIX && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_SEVEN)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_SEVEN;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_EIGHT)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_EIGHT;
	}
	
	return lPenaltyPercent;
}

//		CalcAtkPenaltyExp
//	Functions
//		- ���� ���̿� ���� Attacker�� ���� Ȯ�� ���Ƽ�� ó���Ѵ�.
//	Arguments
//		- pcsAttacker	: ������
//		- pcsTargetCahr : ���ݴ�����
//	Return value
//		- INT16 : �ش�Ǵ� Penalty
///////////////////////////////////////////////////////////////////////////////
INT32 AgpmCombat::CalcAtkPenaltyLevelGap(AgpdCharacter *pcsAttacker, AgpdCharacter *pcsTargetChar)
{
	if(!pcsAttacker || !pcsTargetChar || !m_pcsAgpmCharacter)
		return 0;

	if(!m_pcsAgpmCharacter->IsPC(pcsAttacker) || !m_pcsAgpmCharacter->IsPC(pcsTargetChar))
		return 0;

	INT32	lAttackerLevel	 = 0;
	INT32	lTargetCharLevel = 0;
	INT32	lLevelGap		 = 0;
	INT32	lPenaltyPercent	 = 0;

	lAttackerLevel   = m_pcsAgpmCharacter->GetLevel(pcsAttacker);
	lTargetCharLevel = m_pcsAgpmCharacter->GetLevel(pcsTargetChar);

	lLevelGap		 = lTargetCharLevel - lAttackerLevel;

	if(lLevelGap > 0)
	{
		if (lLevelGap > 1 && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_ONE)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_ONE;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_ONE && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_TWO)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_TWO;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_TWO && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_THREE)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_THREE;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_THREE && lLevelGap <= AGPM_COMBAT_PENALTY_LEVEL_GAP_FOUR)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_FOUR;
		else if (lLevelGap > AGPM_COMBAT_PENALTY_LEVEL_GAP_FOUR)
			lPenaltyPercent = AGPM_COMBAT_PENALTY_LEVEL_GAP_RESULT_FIVE;
	}

	return lPenaltyPercent;
}