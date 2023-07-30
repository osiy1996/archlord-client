#include "AgsmSkillEffect.h"
#include "AgsmSkill.h"
#include "AuMath.h"
#include "AgsdDatabase.h"
#include "AgpmItem.h"
#include "AgpmAI2.h"

typedef std::multimap<INT32, INT32>						MapMob;
typedef std::multimap<INT32, INT32>::iterator			MapIterMob;
typedef std::multimap<INT32, INT32>::reverse_iterator	MapRIterMob;

extern INT32	g_alFactorTable[AGPMSKILL_CONST_PERCENT_END + 1][3];

AgsmSkillEffect::AgsmSkillEffect()
	: m_pcsApModuleManager(NULL)
{
	m_pagpmSkill = NULL;
	m_pagsmSkill = NULL;
	m_pagpmCharacter = NULL;
	m_pagsmCharacter = NULL;
	m_pagpmFactors = NULL;
	m_pagpmCombat = NULL;
	m_pagsmParty = NULL;
	m_pagpmSummons = NULL;
	m_pagsmSummons = NULL;
	m_pagpmEventNature = NULL;
	m_pagsmEventNature = NULL;
	m_pagsmFactors = NULL;
	m_pagpmSiegeWar = NULL;
	m_pagpmWantedCriminal = NULL;
	m_pagsmCombat = NULL;
	m_pagpmItem = NULL;
	m_pagpmTimer = NULL;
	m_pagsmTimer = NULL;
	m_papmEventManager = NULL;
	m_pagpmPvP = NULL;
	m_pagpmEventSkillMaster = NULL;
	m_pagsmItem	= NULL;
	m_pagpmAI2 = NULL;

	m_csRandom = NULL;
}

AgsmSkillEffect::~AgsmSkillEffect()
{
	if(m_csRandom)
		delete m_csRandom;
}

BOOL AgsmSkillEffect::Initialize(ApModuleManager* pModuleManager)
{
	m_pcsApModuleManager = pModuleManager;
	if(!m_pcsApModuleManager)
		return FALSE;

	m_pagpmSkill = (AgpmSkill*)m_pcsApModuleManager->GetModule("AgpmSkill");
	m_pagsmSkill = (AgsmSkill*)m_pcsApModuleManager->GetModule("AgsmSkill");
	m_pagpmCharacter = (AgpmCharacter*)m_pcsApModuleManager->GetModule("AgpmCharacter");
	m_pagsmCharacter = (AgsmCharacter*)m_pcsApModuleManager->GetModule("AgsmCharacter");
	m_pagpmFactors = (AgpmFactors*)m_pcsApModuleManager->GetModule("AgpmFactors");
	m_pagsmFactors = (AgsmFactors*)m_pcsApModuleManager->GetModule("AgsmFactors");
	m_pagpmCombat = (AgpmCombat*)m_pcsApModuleManager->GetModule("AgpmCombat");
	m_pagsmCombat = (AgsmCombat*)m_pcsApModuleManager->GetModule("AgsmCombat");
	m_pagpmItem = (AgpmItem*)m_pcsApModuleManager->GetModule("AgpmItem");
	m_pagsmParty = (AgsmParty*)m_pcsApModuleManager->GetModule("AgsmParty");
	m_pagpmSummons = (AgpmSummons*)m_pcsApModuleManager->GetModule("AgpmSummons");
	m_pagsmSummons = (AgsmSummons*)m_pcsApModuleManager->GetModule("AgsmSummons");
	m_pagpmEventNature = (AgpmEventNature*)m_pcsApModuleManager->GetModule("AgpmEventNature");
	m_pagsmEventNature = (AgsmEventNature*)m_pcsApModuleManager->GetModule("AgsmEventNature");
	m_pagpmSiegeWar = (AgpmSiegeWar*)m_pcsApModuleManager->GetModule("AgpmSiegeWar");
	m_pagpmWantedCriminal = (AgpmWantedCriminal*)m_pcsApModuleManager->GetModule("AgpmWantedCriminal");
	m_pagpmTimer = (AgpmTimer*)m_pcsApModuleManager->GetModule("AgpmTimer");
	m_pagsmTimer = (AgsmTimer*)m_pcsApModuleManager->GetModule("AgsmTimer");
	m_papmEventManager = (ApmEventManager*)m_pcsApModuleManager->GetModule("ApmEventManager");
	m_pagpmPvP = (AgpmPvP*)m_pcsApModuleManager->GetModule("AgpmPvP");
	m_pagpmEventSkillMaster = (AgpmEventSkillMaster*)m_pcsApModuleManager->GetModule("AgpmEventSkillMaster");
	m_pagsmItem = (AgsmItem*)m_pcsApModuleManager->GetModule("AgsmItem");
	m_pagpmAI2 = (AgpmAI2*)m_pcsApModuleManager->GetModule("AgpmAI2");

	m_csRandom = new MTRand;

	return TRUE;
}

// 3. process skill effect functions
///////////////////////////////////////////////////////////////////////////////
//		ProcessSkillEffect
//	Functions
//		- pcsSkill �� skill effect type�� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	: ó���� skill type index
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffect");

	if (!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsSkill->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32	nRepeatCount = 1;

	BOOL	bRetval	= FALSE;

	for (int i = 0; i < nRepeatCount; ++i)
	{
		if (bProcessInterval)
		{
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
				bRetval |= ProcessSkillEffectTransformTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			//if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MOVE_TARGET)
			//	bRetval |= ProcessSkillEffectMoveTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR)
				bRetval |= ProcessSkillEffectUpdateFactor(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectReflectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectHeroicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);
			
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP)
				bRetval |= ProcessSkillEffectMagicResistLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DISPEL_MAGIC)
				bRetval |= ProcessSkillEffectDispelMagic(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_SPECIAL_STATUS)
				bRetval |= ProcessSkillEffectSpecialStatus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_LIFE_PROTECTION)
				bRetval |= ProcessSkillEffectLifeProtection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE)
				bRetval |= ProcessSkillEffectDOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DMG_ADJUST)
				bRetval |= ProcessSkillEffectDMGAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST)
				bRetval |= ProcessSkillEffectSkillFactorAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);	
		
			// Skill Effect 2
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT)
				bRetval |= ProcessSkillEffect2UpdateCombatPoint(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_HP)
				bRetval |= ProcessSkillEffect2RegenHP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_MP)
				bRetval |= ProcessSkillEffect2RegenMP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_CONVERT)
				bRetval |= ProcessSkillEffect2Convert(pcsSkill, pcsTarget, nIndex, bProcessInterval);
			
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_CHARGE)
				bRetval |= ProcessSkillEffect2Charge(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_LEVELUP)
				bRetval |= ProcessSkillEffect2SkillLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_PRODUCT)
				bRetval |= ProcessSkillEffectProduct(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION)
			{
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
					bRetval = TRUE;	// 1Ÿ���̶�� �� TRUE �ش�.
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
					bRetval |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
					bRetval |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);
			}

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_AT_FIELD)
				bRetval |= ProcessSkillEffect2ATField(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SUMMONS)
				bRetval |= ProcessSkillEffect2Summons(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTION_PASSIVE)
				bRetval |= ProcessSkillActionPassive(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_BONUS)
				bRetval |= ProcessSkillEffect2GameBonus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DETECT)
				bRetval |= ProcessSkillEffect2Detect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_RIDE)
				bRetval |= ProcessSkillEffect2Ride(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_EFFECT)
				bRetval |= ProcessSkillEffect2GameEffect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DIVIDE)
				bRetval |= ProcessSkillEffect2Divide(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_RESURRECTION)
				bRetval |= ProcessSkillEffect2Resurrection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_UNION)
				bRetval |= ProcessSkillEffect2Union(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_MOVE_POS)
				bRetval |= ProcessSkillEffect2MovePos(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2DisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex] & AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2ToleranceDisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);
		}
		else
		{
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
				bRetval |= ProcessSkillEffectTransformTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG)
				bRetval |= ProcessSkillEffectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			//if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MOVE_TARGET)
			//	bRetval |= ProcessSkillEffectMoveTarget(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR)
				bRetval |= ProcessSkillEffectUpdateFactor(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectReflectMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_HEROIC_ATTACK)
				bRetval |= ProcessSkillEffectReflectHeroicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMeleeAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK)
				bRetval |= ProcessSkillEffectDefenseMagicAttack(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP)
				bRetval |= ProcessSkillEffectMagicResistLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DISPEL_MAGIC)
				bRetval |= ProcessSkillEffectDispelMagic(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_SPECIAL_STATUS)
				bRetval |= ProcessSkillEffectSpecialStatus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_LIFE_PROTECTION)
				bRetval |= ProcessSkillEffectLifeProtection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE)
				bRetval |= ProcessSkillEffectDOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DMG_ADJUST)
				bRetval |= ProcessSkillEffectDMGAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST)
				bRetval |= ProcessSkillEffectSkillFactorAdjust(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// Skill Effect 2
			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT)
				bRetval |= ProcessSkillEffect2UpdateCombatPoint(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_HP)
				bRetval |= ProcessSkillEffect2RegenHP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_REGEN_MP)
				bRetval |= ProcessSkillEffect2RegenMP(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_CONVERT)
				bRetval |= ProcessSkillEffect2Convert(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_CHARGE)
				bRetval |= ProcessSkillEffect2Charge(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_LEVELUP)
				bRetval |= ProcessSkillEffect2SkillLevelUp(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_PRODUCT)
				bRetval |= ProcessSkillEffectProduct(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION)
			{
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
					bRetval = TRUE;	// 1Ÿ���̶�� �� TRUE �ش�.
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
					bRetval |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);
				if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
					bRetval |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);
			}

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_AT_FIELD)
				bRetval |= ProcessSkillEffect2ATField(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SUMMONS)
				bRetval |= ProcessSkillEffect2Summons(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_ACTION_PASSIVE)
				bRetval |= ProcessSkillActionPassive(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_BONUS)
				bRetval |= ProcessSkillEffect2GameBonus(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DETECT)
				bRetval |= ProcessSkillEffect2Detect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_RIDE)
				bRetval |= ProcessSkillEffect2Ride(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_GAME_EFFECT)
				bRetval |= ProcessSkillEffect2GameEffect(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DIVIDE)
				bRetval |= ProcessSkillEffect2Divide(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_RESURRECTION)
				bRetval |= ProcessSkillEffect2Resurrection(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_SKILL_UNION)
				bRetval |= ProcessSkillEffect2Union(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_MOVE_POS)
				bRetval |= ProcessSkillEffect2MovePos(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2DisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			if (((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex] & AGPMSKILL_EFFECT2_TOLERANCE_DISTURB_CHARACTER_ACTION)
				bRetval |= ProcessSkillEffect2ToleranceDisturbCharacterAction(pcsSkill, pcsTarget, nIndex, bProcessInterval);
		}
	}

	if(!bRetval)
	{
		if(pcsAgsdSkill->m_eMissedReason == AGSDSKILL_MISSED_REASON_NORMAL)
			m_pagsmSkill->SendMissCastSkill(pcsSkill, pcsTarget);
	}

	return bRetval;
}

BOOL AgsmSkillEffect::ProcessSkillEffectCancel(AgpdSkill *pcsSkill, AgpdCharacter* pcsCharacter)
{
	if(!pcsSkill || !pcsCharacter)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	for (INT32 i = 0; i < AGPMSKILL_MAX_SKILL_CONDITION; ++i)
	{
		if (pcsSkillTemplate->m_lEffectType[i] & AGPMSKILL_EFFECT_TRANSFORM_TARGET)
		{
			if (pcsSkillTemplate->m_lEffectType[i] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
			{
				bResult = ProcessSkillEffectTransformTargetType5Cancel(pcsSkill, pcsCharacter);
				//if(pcsSkill->m_pSaveSkillData.eStep != SAVESKILL_NONE)
				{
					pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkillTemplate->m_lID;
					m_pagsmSkill->ProcessSkillSave(pcsSkill, pcsCharacter, AGSMDATABASE_OPERATION_DELETE);
				}
			}
		}
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectTransformTarget");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1)
			bRetval &= ProcessSkillEffectTransformTargetType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2)
			bRetval &= ProcessSkillEffectTransformTargetType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3)
			bRetval &= ProcessSkillEffectTransformTargetType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4)
			bRetval &= ProcessSkillEffectTransformTargetType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
		{
			bRetval &= ProcessSkillEffectTransformTargetType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// ��ȭ ��ų�� �����Ѵ�. - arycoat 2008.10
			if(pcsSkill->m_pSaveSkillData.eStep == SAVESKILL_NONE)
			{
				pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkill->m_pcsTemplate->m_lID;
				m_pagsmSkill->ProcessSkillSave(pcsSkill, (AgpdCharacter*)pcsTarget, AGSMDATABASE_OPERATION_INSERT);
			}
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE1)
			bRetval &= ProcessSkillEffectTransformTargetType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE2)
			bRetval &= ProcessSkillEffectTransformTargetType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE3)
			bRetval &= ProcessSkillEffectTransformTargetType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE4)
			bRetval &= ProcessSkillEffectTransformTargetType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_TRANSFORM_TARGET_TYPE5)
		{
			bRetval &= ProcessSkillEffectTransformTargetType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

			// ��ȭ ��ų�� �����Ѵ�. - arycoat 2008.10
			if(pcsSkill->m_pSaveSkillData.eStep == SAVESKILL_NONE)
			{
				pcsSkill->m_pSaveSkillData.m_pSaveData.SkillTID = pcsSkill->m_pcsTemplate->m_lID;
				m_pagsmSkill->ProcessSkillSave(pcsSkill, (AgpdCharacter*)pcsTarget, AGSMDATABASE_OPERATION_INSERT);
			}
		}
	}

	return bRetval;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
		if(!lSkillLevel)
			return FALSE;

		AgpdSkillTemplate		*pcsSkillTemplate		= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

		AgpdCharacterTemplate	*pcsMonsterTemplate		= m_pagpmCharacter->GetCharacterTemplate((INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_CHANGE_MONSTER][lSkillLevel]);
		if (!pcsMonsterTemplate)
			return FALSE;

		INT32					lRace					= m_pagpmFactors->GetRace(&((AgpdCharacter *) pcsTarget)->m_csFactor);
		INT32					lGender					= m_pagpmFactors->GetGender(&((AgpdCharacter *) pcsTarget)->m_csFactor);

		INT32					lIndex					= 0;
		AgpdCharacterTemplate	*pcsCharacterTemplate	= NULL;

		pcsCharacterTemplate	= m_pagpmCharacter->GetTemplateSequence(&lIndex);
		while (pcsCharacterTemplate)
		{
			INT32	lTemplateRace	= m_pagpmFactors->GetRace(&pcsCharacterTemplate->m_csFactor);
			//INT32	lTemplateGender	= m_pagpmFactors->GetGender(&pcsCharacterTemplate->m_csFactor);
			INT32	lTemplateClass	= m_pagpmFactors->GetClass(&pcsCharacterTemplate->m_csFactor);

			if (lTemplateRace == lRace &&
				//lTemplateGender == lGender &&
				lTemplateClass == AUCHARCLASS_TYPE_KNIGHT &&
				m_pagpmFactors->IsUseFactor(&pcsCharacterTemplate->m_csLevelFactor[(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_WARRIOR_LEVEL][lSkillLevel]]))
				break;

			pcsCharacterTemplate	= m_pagpmCharacter->GetTemplateSequence(&lIndex);
		}

		if (!pcsCharacterTemplate)
			return FALSE;

		return m_pagpmCharacter->TransformCharacter((AgpdCharacter *) pcsTarget,
												AGPMCHAR_TRANSFORM_TYPE_APPEAR_STATUS_ALL,
												pcsMonsterTemplate,
												&pcsCharacterTemplate->m_csLevelFactor[(INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_WARRIOR_LEVEL][lSkillLevel]],
												TRUE);
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
	}

	return TRUE;
}

// 2005.09.01. steeple
// Type1 �� ����ѵ�, Race, Gender �� �����ϰ�, Type1 ���� TargetFactor ��� �κ��� ���� �ܼ�ȭ ��Ŵ.
// ������ �� �ڵ忴��.
BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// �����ε� �ٲ� �� ���� ���̶�� �� return FALSE. �� �� ���� Cast �������� Fail �������� �Ѵ�.
	if(m_pagpmCharacter->IsMonster((AgpdCharacter*)pcsTarget) &&
		((AgpdCharacter*)pcsTarget)->m_pcsCharacterTemplate->m_eTamableType != AGPDCHAR_TAMABLE_TYPE_BY_FORMULA)
		return FALSE;

	// 2005.10.24. steeple
	// ���� �� ����.
	INT32 lCharacterLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsSkill->m_pcsBase);
	INT32 lTargetLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);

	// �̰� ���!!!
	if(lCharacterLevel + lTargetLevel == 0)
		return FALSE;

	INT32 lRandom = m_csRandom->randInt(100);
	if(lRandom > (INT32)(((FLOAT)lCharacterLevel / (FLOAT)(lCharacterLevel + lTargetLevel)) * 100.0f))
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;
	
	INT32 lCreatureTID = m_pagpmSkill->GetRandomCreatureTID(pcsSkillTemplate, lSkillLevel);

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lCreatureTID);
	if(!pcsTransformTemplate)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	if(ulDuration == 0)
		return FALSE;

	if(!m_pagpmCharacter->TransformCharacter((AgpdCharacter*)pcsTarget,
											AGPMCHAR_TRANSFORM_TYPE_APPEAR_ONLY,
											pcsTransformTemplate,
											&pcsTransformTemplate->m_csFactor,
											TRUE))
		return FALSE;

	// �������� �ɸ����� �����Ѵ�. 2005.10.24. steeple
	((AgpdCharacter*)pcsTarget)->m_lTransformedByCID = pcsSkill->m_pcsBase->m_lID;

	// Ǯ���� �ð� �������ش�.
	m_pagsmCharacter->SetTransformTimeout((AgpdCharacter*)pcsTarget, ulDuration);

	// �ϴ� ���� ��Ų��. 2005.10.03. steeple
	m_pagpmCharacter->StopCharacter((AgpdCharacter*)pcsTarget, NULL);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	AgpdCharacter* pcsCharacter = (AgpdCharacter*)pcsTarget;

	if(!m_pagsmSkill->CheckEnableEvolution(pcsCharacter, pcsSkill))
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lCreatureTID = m_pagpmSkill->GetRandomCreatureTID(pcsSkillTemplate, lSkillLevel);

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lCreatureTID);
	if(!pcsTransformTemplate)
		return FALSE;

	m_pagpmCharacter->EvolutionCharacter(pcsCharacter, pcsTransformTemplate);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectTransformTargetType5Cancel(AgpdSkill* pcsSkill, AgpdCharacter* pcsCharacter)
{
	if(!pcsSkill || !pcsCharacter)
		return FALSE;

	INT32 lPrevTID = m_pagpmEventSkillMaster->m_vcSkillMasteryEvolution.GetBeforeEvolutionTID(pcsCharacter->m_lTID1);
	if(!lPrevTID)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdCharacterTemplate* pcsTransformTemplate = m_pagpmCharacter->GetCharacterTemplate(lPrevTID);
	if(!pcsTransformTemplate)
		return FALSE;

	m_pagpmCharacter->RestoreEvolutionCharacter(pcsCharacter, pcsTransformTemplate);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttack
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectMeleeAttack");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1)
			bRetval &= ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2)
			bRetval &= ProcessSkillEffectMeleeAttackType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3)
			bRetval &= ProcessSkillEffectMeleeAttackType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4)
//			bRetval &= ProcessSkillEffectMeleeAttackType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5)
//			bRetval &= ProcessSkillEffectMeleeAttackType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMeleeAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH)
			bRetval &= ProcessSkillEffectMeleeAttackDeath(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6)
			bRetval &= ProcessSkillEffectMeleeAttackType6(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY)
			// ���⿡ Durability �Լ� �߰�
			bRetval &= ProcessSkillEffectMeleeAttackDurability(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		// ����� Heroic Melee Attack ���� ��ų ó��
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC)
			bRetval &= ProcessSkillEffectMeleeAttackHeroic(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1)
			bRetval &= ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2)
			bRetval &= ProcessSkillEffectMeleeAttackType2(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3)
			bRetval &= ProcessSkillEffectMeleeAttackType3(pcsSkill, pcsTarget, nIndex, bProcessInterval);

//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4)
//			bRetval &= ProcessSkillEffectMeleeAttackType4(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5)
//			bRetval &= ProcessSkillEffectMeleeAttackType5(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMeleeAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH)
			bRetval &= ProcessSkillEffectMeleeAttackDeath(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE6)
			bRetval &= ProcessSkillEffectMeleeAttackType6(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY)
			// ���⿡ Durability �Լ� �߰�
			bRetval &= ProcessSkillEffectMeleeAttackDurability(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		// ����� Heroic Melee Attack ���� ��ų ó��
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC)
			bRetval &= ProcessSkillEffectMeleeAttackHeroic(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectMagicAttack
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MAGIC_ATTACK_DMG) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectMagicAttack");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval	= TRUE;

	if (bProcessInterval)
	{
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMagicAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_MAGIC)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_FIRE)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_EARTH)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_AIR)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
//
//		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_DMG_USE_WATER)
//			bRetval &= ProcessSkillEffectMagicAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL)
			bRetval &= ProcessSkillEffectMagicAttackCritical(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectUpdateFactor
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_UPDATE_FACTOR) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactor(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	PROFILE("AgsmSkill::ProcessSkillEffectUpdateFactor");

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL	bRetval = TRUE;

	// 2006.01.18. steeple
	// Attack Damage �� ���õǾ� �ִ� ��ų�̶��, Factor �ø��� �۾��� ���� �ʴ´�.
	if(m_pagsmSkill->IsSetAttackDamageOnly(pcsSkill))
		return TRUE;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT)
			bRetval &= ProcessSkillEffectUpdateFactorHOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY)
			bRetval &= ProcessSkillEffectUpdateFactorMagnify(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE)
			bRetval &= ProcessSkillEffectUpdateFactorAdd(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM)
			bRetval &= ProcessSkillEffectUpdateFactorItem(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME)
			bRetval &= ProcessSkillEffectUpdateFactorTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT)
			bRetval &= ProcessSkillEffectUpdateFactorHOT(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY)
			bRetval &= ProcessSkillEffectUpdateFactorMagnify(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_FOR_A_WHILE)
			bRetval &= ProcessSkillEffectUpdateFactorAdd(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_ITEM)
			bRetval &= ProcessSkillEffectUpdateFactorItem(pcsSkill, pcsTarget, nIndex, bProcessInterval);

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_UPDATE_FACTOR_TIME)
			bRetval &= ProcessSkillEffectUpdateFactorTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return bRetval;
}

//		ProcessSkillEffectReflectMeleeAttack
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_REFLECT_MELEE_ATTACK) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectReflectMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	//////////////////////////////////////////////////////////////////////////
	// 2004.09.21. steeple ����
	
	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	UINT64 ullEffectType = 0;
	if(bProcessInterval)
		ullEffectType = pcsSkillTemplate->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = pcsSkillTemplate->m_lEffectType[nIndex];

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE �� Damage �� �ݰ��ϰ�,
	if(ullEffectType & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
		}

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount;
	}

	// AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD �� Damage �� �����Ѵ�.
	if(ullEffectType & AGPMSKILL_EFFECT_REFLECT_DAMAGE_SHIELD)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		if(bCheckConnection)
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageIgnoreProbability;
	}

	// 
	// Effect_Detail_Reflect_Melee_Attack�� ó�����ش�. 2008.06.11. iluvs
	if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK])
	{
		ProcessSkillEffect2ReflectMeleeAtk(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return TRUE;
}

//		ProcessSkillEffectReflectMagicAttack
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectReflectMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_MAGIC_ATTACK;

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
			pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
		else
		{
			pcsAgsdSkill->m_nMagicReflectPoint 
				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			
			if(bCheckConnection)
			{
				pcsAgsdSkill->m_nMagicReflectPoint 
					+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			}

			pcsAttachData->m_nMagicReflectPoint += pcsAgsdSkill->m_nMagicReflectPoint;
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE)
			pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE;
		else
		{
			pcsAgsdSkill->m_nMagicReflectPoint 
				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];

			if(bCheckConnection)
			{
				pcsAgsdSkill->m_nMagicReflectPoint 
					+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REFLECT_MAX][lSkillLevel];
			}

			pcsAttachData->m_nMagicReflectPoint += pcsAgsdSkill->m_nMagicReflectPoint;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectReflectHeroicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// AGPMSKILL_EFFECT_REFLECT_RECEIVED_DAMAGE �� Damage �� �ݰ��ϰ�,
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_HEROIC_REFLECT_MAX][lSkillLevel];

	if(bCheckConnection)
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_HEROIC_REFLECT_MAX][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicProbability += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamageReflectHeroicAmount += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamageReflectAmount;

	return TRUE;
}

//		ProcessSkillEffectDefenseMeleeAttack
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDefenseMeleeAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// ���� ������ 100% ���� Melee Damage��ŭ �������� �ش�.
	if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK)
	{
		pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_COUNTER_ATTACK;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lCounterAttackProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lCounterAttackProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	}

	// 2005.03.27. steeple
	// ������ ������ ȸ���Ѵ�. arg1 �� 0�̸� EVADE. 1�̸� DODGE ��.
	//
	// ������ ��� ����/��Ÿ� ������ arg1�� Ȯ���� ȸ���Ͽ� ��ȿȭ�Ѵ�.
	// arg1 �� 0 �̸� Const ���� �����´�. - 2004.09.15. steeple
	else if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_EVADE)
	{
		// 2005.12.22. steeple
		// Evade üũ�� �ٲ����.
		//
		//pcsAttachData->m_ullBuffedSkillCombatEffect						|= AGPMSKILL_EFFECT_DEFENSE_EVADE;

		//switch(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1)
		//{
		//	case 0:		// Evade ��
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		break;

		//	case 1:		// Dodge ��
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDodgeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDodgeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		break;

		//	case 2:		// �Ѵ�
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_EVADE_RATE][lSkillLevel];
		//		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDodgeProbability	+= ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDodgeProbability = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DODGE_RATE][lSkillLevel];
		//		break;
		//}
	}

	//defense_max ��ŭ physical �������� ���´�.
	else if (ullEffectType & AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT) 
	{
		pcsAttachData->m_ullBuffedSkillCombatEffect	|= AGPMSKILL_EFFECT_CRITICAL_DEFENCE_PERCENT;
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDefenceCriticalAttack	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDefenceCriticalAttack	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL_DEFENCE_PERCENT][lSkillLevel]; 
	}
	else 
	{
		pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

		pcsAgsdSkill->m_nMeleeDefensePoint 
			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_nMeleeDefensePoint 
				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];
		}

		pcsAttachData->m_nMeleeDefensePoint += pcsAgsdSkill->m_nMeleeDefensePoint;
	}

	return TRUE;
}

//		ProcessSkillEffectDefenseMagicAttack
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDefenseMagicAttack(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget) return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	pcsAttachData->m_ullBuffedSkillCombatEffect				|= AGPMSKILL_EFFECT_DEFENSE_MELEE_ATTACK;

	// ������ ��� ���������� arg1�� Ȯ���� ȸ���Ͽ� ��ȿȭ�Ѵ�.
	if (ullEffectType & AGPMSKILL_EFFECT_DEFENSE_EVADE)
	{
		// 2005.12.22. steeple
		// Evade üũ�� �ٲ����.
		//
		//pcsAttachData->m_ullBuffedSkillCombatEffect						|= AGPMSKILL_EFFECT_DEFENSE_EVADE;
		//pcsAttachData->m_stBuffedSkillCombatEffectArg.lEvadeProbability	+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
		//pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lEvadeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	// defense_max ��ŭ magic �������� ���´�.
	else
	{
		pcsAttachData->m_nDefenseTypeEffect |= AGPMSKILL_EFFECT_DEFENSE_MAGIC_ATTACK;

		pcsAgsdSkill->m_nMagicDefensePoint 
			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAgsdSkill->m_nMagicDefensePoint 
				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEFENSE_MAX][lSkillLevel];
		}

		pcsAttachData->m_nMagicDefensePoint += pcsAgsdSkill->m_nMagicDefensePoint;
	}

	return TRUE;
}

//		ProcessSkillEffectMagicResistLevelUp
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MAGIC_RESIST_LEVEL_UP) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicResistLevelUp(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	return m_pagsmSkill->SetModifiedCharLevel(pcsTarget, pcsSkill);
}

//		ProcessSkillEffectDispelMagic
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_DISPEL_MAGIC) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDispelMagic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkillADBase* pcsADBase = m_pagsmSkill->GetADBase((ApBase*)pcsTarget);
	if(!pcsADBase)
		return TRUE;

	AgpdSkillTemplate* pcsTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	UINT64 ullEffectType = 0;
	if(bProcessInterval)
		ullEffectType = pcsTemplate->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = pcsTemplate->m_lEffectType[nIndex];

	if (ullEffectType & AGPMSKILL_EFFECT_ADD_SPELLCOUNT_MASTERY_POINT)
	{
		INT32	lMasteryPoint	= lSkillLevel;
		if (lMasteryPoint)
		{
			INT32	lDispelSkillIndex	= (-1);
			m_pagsmSkill->ReduceSpellCount(pcsTarget, lMasteryPoint, &lDispelSkillIndex);

			if (lDispelSkillIndex >= 0)
			{
				// spell count�� 0�� �Ǿ���. ��ų�� ���־� �Ѵ�.
				m_pagsmSkill->FreeSkillEffect(pcsADBase->m_csBuffedSkillProcessInfo[lDispelSkillIndex].m_pcsTempSkill, pcsTarget, lDispelSkillIndex);
			}
		}
	}

	// 2004. 09. 21. steeple
	if(ullEffectType & AGPMSKILL_EFFECT_DISPEL_MAGIC_CURE)
	{
		m_pagsmSkill->EndAllDebuffSkill((ApBase*)pcsTarget, TRUE, ( INT32 ) pcsTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel]);
	}

	// Effect Detail ó�����ش�. 2007.07.03. steeple
	if(pcsTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL])
	{
		ProcessSkillEffect2Dispel(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	return TRUE;
}

//		ProcessSkillEffectSpecialStatus
//	Functions
//		- pcsSkill �� skill effect type () �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32	lAtkPenalty		= m_pagpmCombat->CalcAtkPenaltyLevelGap((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget);

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		UINT64 lSpecialStatus = (UINT64)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lSpecialStatus;

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN)
		{
			INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 lStunProbability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				lStunProbability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(lStunProbability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			// �ٸ� ��⿡�� Ư�����¿� ���õ� �����ؾ��� ���� �ִ��� �����´�.

			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_STUN;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[4];
			pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
			pvBuffer[1]	= (PVOID)	pcsSkill;
			pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
			pvBuffer[3]	= (PVOID)	&lAdjustProbability;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

			// 2004.12.16. steeple
			// ����ڰ� Stun Protect �����̸� Stun �� �ɸ� Ȯ���� �����ش�.
			if(((AgpdCharacter*)pcsTarget)->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT)
			{
				AgsdCharacter* pcsAgsdTarget = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
				if(pcsAgsdTarget)
					lAdjustProbability += pcsAgsdTarget->m_stInvincibleInfo.lStunProbability;
			}

			if (lAdjustProbability > 0)
			{
				// ���� ����...
				if (m_csRandom->randInt(100) < lAdjustProbability)
				{
					pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
					return FALSE;
				}
			}

			UINT32	ulStunDuration	= (UINT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_TIME][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				ulStunDuration += (UINT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_STUN_TIME][lSkillLevel];

			if (ulStunDuration == 0)
				ulStunDuration	= 3000;			// �⺻�� 3�ʷ� �Ѵ�.

			ulStunDuration -= ulAdjustDuration;
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN, ulStunDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FREEZE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_FREEZE, ulDuration);
		}
		
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLOW)
		{
			// Slow ���� ���� UpdateFactor�� movement�� ���ҽ�Ű�� ��ų�̴�.

			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLOW, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_ATTRIBUTE_INVINCIBLE, ulDuration);

			// Ȯ���� �������ش�. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lAttrProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lAttrProbability += pcsAgsdSkill->m_stInvincibleInfo.lAttrProbability;
			}
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_NOT_ADD_AGRO)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO, ulDuration);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HIDE_AGRO)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO, ulDuration);
		}

		// 2004.12.13. steeple �߰�
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_STUN_PROTECT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT, ulDuration);

			// Ȯ���� �������ش�. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lStunProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lStunProbability += pcsAgsdSkill->m_stInvincibleInfo.lStunProbability;
			}
		}

		// 2005.07.07. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_TRANSPARENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, ulDuration);
		}

		// 2005.10.17. steeple �߰�
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HALT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALT, ulDuration);
		}

		// 2006.09.20. steeple �߰�
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HALF_TRANSPARENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_NORMAL_ATK_INVINCIBLE, ulDuration);

			// Ȯ���� �������ش�. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lNormalATKProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lNormalATKProbability += pcsAgsdSkill->m_stInvincibleInfo.lNormalATKProbability;
			}
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SKILL_ATK_INVINCIBLE, ulDuration);

			// Ȯ���� �������ش�. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lSkillATKProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lSkillATKProbability += pcsAgsdSkill->m_stInvincibleInfo.lSkillATKProbability;
			}
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_SKILL)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_SKILL, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_NORMAL_ATK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_NORMAL_ATK, ulDuration);
		}

		// 2007.06.01. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLEEP)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLEEP, ulDuration);
		}

		// 2007.06.30. steeple
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SLOW_INVINCIBLE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SLOW_INVINCIBLE, ulDuration);

			// Ȯ���� �������ش�. - 2007.06.30. steeple
			AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
			AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
			if(pcsAgsdSkill && pcsAgsdCharacter)
			{
				pcsAgsdSkill->m_stInvincibleInfo.lSlowProbability = m_pagsmSkill->GetSkillRate(pcsSkill);
				pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability += pcsAgsdSkill->m_stInvincibleInfo.lSlowProbability;
			}
		}

		// 2008.11.03. iluvs
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISARMAMENT)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			INT32 lPart = 0;

			if(!ProcessSkillEffectSpecialStatusDisArmament(pcsSkill, pcsTarget, &lPart))
				return FALSE;

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT, ulDuration);

			// ĳ������ Special Status �� Set ���ش�.
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISARMAMENT, ulDuration, lPart);
		}

		// 2009.04.23. iluvs
		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISABLE_CHATTING)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			m_pagpmCharacter->UpdateSetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISABLE_CHATTING);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HOLD)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;
		
			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HOLD, ulDuration);

			//Implementation process about special_status_hold 
			ProcessSkillEffectSpecialStatusHold(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_CONFUSION)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_CONFUSION, ulDuration);

			//Implementation process about special_status_confusion
			ProcessSkillEffectSpecialStatusConfusion(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FEAR)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_FEAR, ulDuration);

			//Implementation process about special_status_fear
			ProcessSkillEffectSpecialStatusFear(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISEASE)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_DISEASE, ulDuration);

			//Implementation process about special_status_disease
			ProcessSkillEffectSpecialStatusDisease(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_BERSERK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_BERSERK, ulDuration);

			//Implementation process about special_status_berserk
			ProcessSkillEffectSpecialStatusBerserk(pcsSkill, pcsTarget);
		}

		if(lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SHRINK)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			if(!lSkillLevel)
				return FALSE;

			INT32 Probability = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			if(bCheckConnection && pcsSkillTemplate->m_lSpecialStatusAdjustType == 1)
				Probability += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			// ���� �������� ���� Ȯ���� üũ�Ѵ�.
			if(Probability - lAtkPenalty < m_csRandom->randInt(100))
			{
				pcsAgsdSkill->m_bDamageOnlyTemporary = TRUE;
				return FALSE;
			}

			UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
			if(ulDuration == 0)
				return FALSE;

			m_pagsmCharacter->SetSpecialStatusTime((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_SHRINK, ulDuration);

			//Implementation process about special_status_shrink
			ProcessSkillEffectSpecialStatusShrink(pcsSkill, pcsTarget);
		}
	}

	return TRUE;
}

// 2008.11.04. iluvs
// DisArmament Effect�� Skill�� ó���Ѵ�.
// (�� ���� ĳ���Ϳ� ����Ǵ� ���̾ƴ� �����ۿ� ����ǹǷ� ���� ó���Ѵ�.)
BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusDisArmament(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT32 *lParts)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;
/*		
	// �� ��ų�� Ȯ���� ���ؼ� �����϶��� �����Ų��.
	INT32 lRandom	 = m_csRandom->randInt(100);
	INT32 nSkillRate = m_pagsmSkill->GetSkillRate(pcsSkill);
	INT16 nSize		 = 0;
	
	if(nSkillRate < lRandom) 
		return FALSE;
*/
	// ���� Parts�� �����ִ� ���¸� �ϴ� �ߺ����δ� �ȵǰ� �Ѵ�.
	if(((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved != 0)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	AgpmItemPart eParts = (AgpmItemPart)((INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ITEM_PARTS][lSkillLevel]);

	AgpdItem *pcsEquipItem = NULL;

	BOOL bDefaultParts = TRUE;

	// eParts�� �ƹ��͵� ������ �ȵǾ������� default�� ���⸦ ������.
	if(eParts < AGPMITEM_PART_BODY || eParts > AGPMITEM_PART_V_LANCER || eParts == AGPMITEM_PART_HAND_LEFT || eParts == AGPMITEM_PART_HAND_RIGHT)
	{
		pcsEquipItem = m_pagpmItem->GetEquipWeapon((AgpdCharacter*)pcsTarget);
		if(!pcsEquipItem)
			return FALSE;
	}
	else
	{
		pcsEquipItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, eParts);
		if(NULL == pcsEquipItem)
			return FALSE;

		bDefaultParts = FALSE;

		*lParts = (INT32)eParts;
	}

	BOOL bEquipDefaultItem = TRUE;

	if(m_pagpmItem->GetWeaponType(pcsEquipItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	{
		AgpdItem* pcsLeftItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, AGPMITEM_PART_HAND_LEFT);
		if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsEquipItem->m_lID &&
			m_pagpmItem->GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
		{
			if(pcsLeftItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
			{
				m_pagpmItem->EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsLeftItem, &bEquipDefaultItem);

				pcsLeftItem->m_lStatusFlag |= AGPMITEM_STATUS_DISARMAMENT;
			}

			m_pagpmItem->EnumCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pcsLeftItem, NULL);
		}
	}

	if(pcsEquipItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
	{
		m_pagpmItem->EnumCallback(ITEM_CB_ID_CHAR_UNEQUIP, pcsEquipItem, &bEquipDefaultItem);

		// ������� ������ �������� ���¸� AGPMITEM_STATUS_DISARMAMENT ���·� �ٲ��ش�.
		pcsEquipItem->m_lStatusFlag |= AGPMITEM_STATUS_DISARMAMENT;
	}

	m_pagpmItem->EnumCallback(ITEM_CB_ID_REMOVE_FOR_NEAR_CHARACTER, pcsEquipItem, NULL);


	// � Part�� �������� ����
	if(bDefaultParts)
	{
		((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved = (INT32)AGPMITEM_PART_HAND_RIGHT;
	}
	else
	{
		((AgpdCharacter*)pcsTarget)->m_ulSpecialStatusReserved = (INT32)eParts;
	}
	
	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusConfusion(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_CONFUSION;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusHold(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_HOLD;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusFear(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_FEAR;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusDisease(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_DISEASE;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here
	INT32 lSkillLevel		= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lStunDurationTime = static_cast<INT32>(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_TIME][lSkillLevel]);
	INT32 lStunGenerateTime = static_cast<INT32>(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_STUN_GENERATE_TIME][lSkillLevel]);

	if(lStunDurationTime < 0 || lStunGenerateTime < 0)
		return FALSE;

	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam1 = lStunDurationTime;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam2 = lStunGenerateTime;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusBerserk(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_BERSERK;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectSpecialStatusShrink(AgpdSkill *pcsSkill, ApBase *pcsTarget)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AuAutoLock Lock(pcsTarget->m_Mutex);
	if(!Lock.Result())
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_NONE)
		return FALSE;

	//////////////////////////////////////////////////////////////////////////////
	// Setting state
	pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI		= AGPDAI2_STATE_PC_AI_SHRINK;
	pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID	= pcsSkill->m_pcsBase->m_lID;

	// if it has more detail effect, Implementation here

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectLifeProtection(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgsdCharacter	*pcsAgsdCharacter		= m_pagsmCharacter->GetADCharacter((AgpdCharacter *) pcsTarget);
		if (!pcsAgsdCharacter)
			return FALSE;

		INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
		if(!lSkillLevel)
			return FALSE;

		pcsAgsdCharacter->m_bResurrectWhenDie	= TRUE;
		pcsAgsdCharacter->m_lResurrectHP		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HP_RECOVERY][lSkillLevel];
	}

	return TRUE;
}

//		ProcessSkillEffectDOT
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_DOT_DAMAGE) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectDOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	if (pcsTarget->m_eType == APBASE_TYPE_CHARACTER)
	{
		AgsdCharacter	*pcsAgsdTarget	= m_pagsmCharacter->GetADCharacter((AgpdCharacter *) pcsTarget);
		if (pcsAgsdTarget && pcsAgsdTarget->m_bIsMaxDefence)
			return TRUE;
		if (pcsAgsdTarget && pcsAgsdTarget->m_bIsSuperMan)
			return TRUE;
	}

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	if(!bProcessInterval)
	{
		// �� ����Ǿ� �ϴ� Ƚ���� ���Ѵ�.
		// 1�� ���ִ� ������, DOT ��ų�� Duration �� Interval ��ŭ �� ���ߴ�. �׷��� �ؾ��� ��꺸�� 1�� �� ���� ������ 1 ���ش�.
		UINT32 ulSkillInterval = m_pagpmSkill->GetSkillInterval(pcsSkill, lSkillLevel);
		if(ulSkillInterval != 0)
			pcsAgsdSkill->m_nTotalDOTCount = (INT16)(m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel) / ulSkillInterval) - 1;

		return TRUE;
	}


	// ���� ������ �κ�
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	AgpdSkillTemplate	*pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	nDamage		= 0;
	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	BOOL	bIsPoisonDamage	= FALSE;

	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	if (bProcessInterval)
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			}

			// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
			//////////////////////////////////////////////////////////////////////////////////
//			if (nDamage > 0 && !pcsSkill->m_bCloneObject)
//				nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			}		
		}


		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE)
		{
			INT32 lSpiritConstIndex = 0;
			// ���⿡�� Poison ���� 0���� ũ�� bIsPoisonDamage�� TRUE�� ��������� �Ѵ�.
			for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL + 1; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lSpiritConstIndex = m_pagsmSkill->GetSpiritDOTConstIndexByFactor((AgpdFactorAttributeType)i);
				if(lSpiritConstIndex < AGPMSKILL_CONST_DOT_DMG_MAGIC || 
					lSpiritConstIndex > AGPMSKILL_CONST_DOT_DMG_THUNDER)
					continue;

				nDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];
				stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];

				// 2007.10.24. steeple
				if(bCheckConnection)
				{
					nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
					stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
				}

				if (i == AGPD_FACTORS_ATTRIBUTE_TYPE_POISON && stAttrDamage.lValue[i] > 0)
					bIsPoisonDamage	= TRUE;
			}
		}

		// �ٸ� ��⿡�� Poison�� ���õ� �����ؾ��� ���� �ִ��� �����´�.
		if (bIsPoisonDamage)
		{
			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[2];
			pvBuffer[0]	= (PVOID)	&lAdjustProbability;
			pvBuffer[1]	= (PVOID)	&ulAdjustDuration;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_POISON_STATUS_FACTOR, pcsTarget, pvBuffer);

			if (ulAdjustDuration > 0)
			{
				pcsSkill->m_ulEndTime	-= ulAdjustDuration;
			}
		}
	}
	else
	{
		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_PHYSICAL)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_A][lSkillLevel];
			}

			// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
			//////////////////////////////////////////////////////////////////////////////////
//			if (nDamage > 0 && !pcsSkill->m_bCloneObject)
//				nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_HEROIC)
		{
			nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];

			// 2007.10.24. steeple
			if(bCheckConnection)
			{
				nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
				stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DOT_DMG_HEROIC][lSkillLevel];
			}		
		}

		if (((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex] & AGPMSKILL_EFFECT_DOT_DAMAGE_ATTRIBUTE)
		{
			INT32 lSpiritConstIndex = 0;
			// ���⿡�� Poison ���� 0���� ũ�� bIsPoisonDamage�� TRUE�� ��������� �Ѵ�.
			for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL + 1; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lSpiritConstIndex = m_pagsmSkill->GetSpiritDOTConstIndexByFactor((AgpdFactorAttributeType)i);
				if(lSpiritConstIndex < AGPMSKILL_CONST_DOT_DMG_MAGIC || 
					lSpiritConstIndex > AGPMSKILL_CONST_DOT_DMG_THUNDER)
					continue;

				nDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];
				stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[lSpiritConstIndex][lSkillLevel];

				// 2007.10.24. steeple
				if(bCheckConnection)
				{
					nDamage	+= (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
					stAttrDamage.lValue[i] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[lSpiritConstIndex][lSkillLevel];
				}

				if (i == AGPD_FACTORS_ATTRIBUTE_TYPE_POISON && stAttrDamage.lValue[i] > 0)
					bIsPoisonDamage	= TRUE;
			}
		}

		// �ٸ� ��⿡�� Poison�� ���õ� �����ؾ��� ���� �ִ��� �����´�.
		if (bIsPoisonDamage)
		{
			UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
			UINT32	ulAdjustDuration	= 0;
			INT32	lAdjustProbability	= 0;

			PVOID	pvBuffer[2];
			pvBuffer[0]	= (PVOID)	&lAdjustProbability;
			pvBuffer[1]	= (PVOID)	&ulAdjustDuration;

			m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_POISON_STATUS_FACTOR, pcsTarget, pvBuffer);

			if (ulAdjustDuration > 0)
			{
				pcsSkill->m_ulEndTime	-= ulAdjustDuration;
			}
		}
	}

	if (nDamage < 1)
		return TRUE;

	/*
	// defense�� reflect�� �ϴ��� �˻��Ѵ�.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage�� �ݿ��Ѵ�.
	//		���� ĳ���Ϳ� �ݿ��ϰ� History���� �ݿ��Ѵ�.
	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	// 2005.04.01. steeple
	// ���������� BuffedSkill �� ����Ǵ� �Ŷ�� �������� �׳� �����ϰ� �ƴϸ� �����ش�.
	if(bProcessInterval)
	{
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] = (-nDamage);
		for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] = (-stAttrDamage.lValue[i]);	// ��⵵ �׳� ����

		// Heroic Damage�� ���� ���
		if(stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] > 0)
			pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] = (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

		// �̰� �ϳ� ���������ش�. 2006.11.14. steeple
		++pcsAgsdSkill->m_nAffectedDOTCount;
	}
	else
	{
		//pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
		//for (int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
		//	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	}

	/*
	if (lReflectDamage > 0)
	{
		// ������ ������ lReflectDamage �� �ǵ����ش�.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectDMGAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget || !pcsSkill->m_pcsTemplate)
		return FALSE;

	AgsdSkillADBase	*pcsADBase		= m_pagsmSkill->GetADBase(pcsTarget);
	if (!pcsADBase)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;

	INT32	lSkillLevel				= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32	lDMGAdjust				= (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DMG_ADJUST][lSkillLevel];

	//pcsADBase->m_lDamageAdjustRate	= (INT32) (100 / (1 + lDMGAdjust / 100.0));
	pcsADBase->m_lDamageAdjustRate	= (INT32) (1 + lDMGAdjust / 100.0);

	return TRUE;
}

//		ProcessSkillEffectSkillFactorAdjust
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			(arg1�� Ȯ���� �Ʒ� ����(arg2)�� �ϳ��� ��� ��ų�� ���ÿ� üũ�Ͽ� �����Ѵ�. (��ȭ ���� : %))
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME
//						AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME	20057.09. steeple
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectSkillFactorAdjust(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	UINT64				ullEffectType	= 0;

	if (bProcessInterval)
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType[nIndex];
	else
		ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType[nIndex];

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	//	cost hp
	if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_HP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostHP				+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostHPProbability	+= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;

		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostHP				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostHPProbability		= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	//	cost mp
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_MP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMP				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMP				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMP += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMP += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_COST_MP_DECREASE][lSkillLevel];
		}

		// 2005.04.28. steeple
		// ��Ƽ�� ��ų ������ lCostMP �ణ ��ȭ ����.

		// 2005.01.31. steeple
		// �Ʒ� ���� ���õǾ� �ִٸ� SKILL_RATE �� Ȯ���� COSTMP �� 0 ���� �Ѵ�.
		if(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMPProbability = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCostMPProbability  = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		}
		
		//pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostMPProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	/*
	//	cost sp
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_COST_SP;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostSP				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCostSPProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}
	*/

	//	range
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect									|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_RANGE;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent			+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent			= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lRangeAdjustPercent += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RANGE][lSkillLevel];
		}

		/*
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lMagicSkillRange				= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg2;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lMagicSkillRangeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
		*/
	}

	//	cast time
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_CAST_TIME;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTime				+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCastTime				= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTime += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lCastTime += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CAST_TIME_DECREASE][lSkillLevel];
		}

		//pcsAttachData->m_stBuffedSkillFactorEffectArg.lCastTimeProbability	= ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_stConditionArg[nIndex].lArg1;
	}

	//	Duration Time 2005.07.09. steeple
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_DURATION_TIME;
		pcsAttachData->m_stBuffedSkillFactorEffectArg.lDurationTime			+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lDurationTime			= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];

		// 2007.10.25. steeple
		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillFactorEffectArg.lDurationTime += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lDurationTime += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_ADDITIONAL_DURATION][lSkillLevel];
		}
	}

	// Buffed Skill Duration Time
	else if (ullEffectType & AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION)
	{
		pcsAttachData->m_ullBuffedSkillFactorEffect							|= AGPMSKILL_EFFECT_SKILL_FACTOR_ADJUST_BUFFED_SKILL_DURATION;

		ProcessSkillEffectBuffedSkillDurationTime(pcsSkill, pcsTarget, nIndex, bProcessInterval);
	}

	else
		return FALSE;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectBuffedSkillDurationTime(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillAttachData *pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(NULL == pcsAttachData)
		return FALSE;

	AgsdSkillADBase *pcsADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(NULL == pcsADBase)
		return FALSE;

	// Check Target has buffedSkill
	if(pcsAttachData->m_astBuffSkillList[0].lSkillID == AP_INVALID_SKILLID)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	for(int i=0; i < AGPMSKILL_MAX_SKILL_BUFF; ++i)
	{
		if(pcsAttachData->m_astBuffSkillList[i].lSkillID == AP_INVALID_SKILLID ||
			!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill)
			break;

		AgpdSkillTemplate *pcsBuffedSkillTemplate = pcsAttachData->m_astBuffSkillList[i].pcsSkillTemplate;
		if(NULL == pcsBuffedSkillTemplate)
			continue;

		if(m_pagpmSkill->IsDebuffSkill(pcsBuffedSkillTemplate) == FALSE)
			continue;

		BOOL bAdjust = FALSE;

		// Check All or Not Debuffed Skill Update Duration Time
		if(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_bSpecialStatusMatchingAll == TRUE)
		{
			bAdjust = TRUE;
		}
		else
		{
			if(((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lSpecialStatusMatching & pcsBuffedSkillTemplate->m_lSpecialStatus)
			{
				bAdjust = TRUE;
			}
		}

		// Adjust Buffed Skill Duration at the Buffed Skill
		//////////////////////////////////////////////////////////////////////////////////////////
		if(bAdjust)
		{
			INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
			INT32 lAdjustTime = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_TIME_CONTROL][lSkillLevel];

			pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_ulEndTime += lAdjustTime;

			if(bCheckConnection)
			{
				lAdjustTime = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_TIME_CONTROL][lSkillLevel];
				pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_ulEndTime += lAdjustTime;
			}
		}
	}

	return TRUE;
}

//		ProcessSkillEffect2UpdateCombatPoint
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			ullEffectType �� �� ���� ���� �������ش�.
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE1
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE2
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE3
//						AGPMSKILL_EFFECT_UPDATE_COMBAT_POINT_TYPE4
//
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect2UpdateCombatPoint(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	//UINT64				ullEffectType	= 0;

	//if (bProcessInterval)
	//	ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex];
	//else
	//	ullEffectType = ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex];

	UINT32 ulEffectDetailType = 0;
	if(bProcessInterval)
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT];
	else
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT];

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// 2005.12.22. steeple
	// Hit Rate �� Factor �� ����Ǿ���. 
	//
	//
	//// Hit Rate
	//if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1)
	//{
	//	pcsAttachData->m_stBuffedSkillCombatEffectArg.lHitRate += ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HIT_RATE][lSkillLevel];
	//	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lHitRate = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_HIT_RATE][lSkillLevel];

	//	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
	//	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE1;
	//}

	// ���� ������ �ִ� �������� ����
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX;
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MAX;

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE2;
	}

	// ���� ������ �ּ� �������� ����
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN;
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lWeaponDamageType = AGPMSKILL_WEAPON_DAMAGE_TYPE_MIN;

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_UPDATE_COMBAT_POINT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT] |= AGPMSKILL_EFFECT_DETAIL_UPDATE_COMBAT_POINT_TYPE3;
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2RegenHP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_HP][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REGEN_HP][lSkillLevel];

	pcsAttachData->m_stBuffedSkillFactorEffectArg.lHPRegen += pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lHPRegen;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2RegenMP(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lMPRegen = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lMPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	pcsAttachData->m_stBuffedSkillFactorEffectArg.lMPRegen += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_REGEN_MP][lSkillLevel];

	return TRUE;
}

//		ProcessSkillEffectAbsorb
//	Functions
//		- HP, MP �� ��ȯ�� ó��
//
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffect2Convert(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	//UINT64 ullEffectType = 0;

	//if(bProcessInterval)
	//	ullEffectType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lProcessIntervalEffectType2[nIndex];
	//else
	//	ullEffectType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_lEffectType2[nIndex];

	UINT32 ulEffectDetailType = 0;
	if(bProcessInterval)
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT];
	else
		ulEffectDetailType = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_CONVERT];

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bCheckConnection = m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval);

	// �������� HP �� ���
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_HP;
	}

	// �������� MP �� ���
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[0] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[0] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_DAMAGE_TO_MP;
	}

	// 2005.03.30. steeple �ణ ����
	// CONST ��ġ�� MP ���� �ʰ� �� HP �� �����ش�.
	//
	// MP �� HP �� ���
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_MP_TO_HP)
	{
		AgpdCharacter* pcsAgpdTarget = (AgpdCharacter*)pcsTarget;
		INT32 lChangeValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_MP_CONVERT_HP][lSkillLevel];
		
		if(bCheckConnection)
			lChangeValue += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_MP_CONVERT_HP][lSkillLevel];

		// ���� HP, MP, MAX HP �� ��´�.
		INT32 lCurrentHP, lCurrentMP, lMaxHP;
		lCurrentHP = lCurrentMP = lMaxHP = 0;

		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(&pcsAgpdTarget->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

		//if(lChangeValue > lCurrentMP)
		//	lChangeValue = lCurrentMP;

		if((lChangeValue + lCurrentHP) > lMaxHP)
			lChangeValue = lMaxHP - lCurrentHP;

		//lCurrentMP -= lChangeValue;		// MP �� ���ְ�,
		lCurrentHP += lChangeValue;		// HP �� �����ش�.

		// �ٽ� ����
		PVOID pvPacket = NULL;
		m_pagsmFactors->UpdateCharPoint(&pcsAgpdTarget->m_csFactor, &pvPacket, lCurrentHP, lCurrentMP, 0);

		// ���� �ߴٸ� ��Ŷ ����������.
		if(pvPacket)
		{
			m_pagsmCharacter->SendPacketFactor(pvPacket, pcsAgpdTarget, PACKET_PRIORITY_4);
			m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
		}
	}

	// 2005.07.12. steeple
	// ���� �������� HP �� ��ȯ
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2HPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_HP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP;
	}

	// 2005.07.12. steeple
	// ���� �������� MP �� ��ȯ
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP)
	{
		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] = (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

		pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];

		if(bCheckConnection)
		{
			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPProbability[1] += (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDamage2MPAmount[1] += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_CONVERT_MP][lSkillLevel];
		}

		pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CONVERT;
		pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_CONVERT] |= AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_MP;
	}

	// 2007.07.04. steeple
	// ���� �������� HP �� �ٷ� ��ȯ. �ѹ���
	if(ulEffectDetailType & AGPMSKILL_EFFECT_DETAIL_CONVERT_ATK_DAMAGE_TO_HP_INSTANT)
	{
		// ���⼭ ũ�� �� ���� ����.
		// �� �������� ���� �κп��� ó�����ش�.
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2Charge(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// Charge �� �������ش�. ù �ܰ�� NONE
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_CHARGE;

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lChargeLevel = AGPMSKILL_CHARGE_NONE;
	pcsAttachData->m_lMeleeComboInCharge = 0;

	// AgsdSkill �� ù �ܰ赵 NONE, �׷��� Damage �� ���� ����.
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lChargeLevel = AGPMSKILL_CHARGE_NONE;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lChargeDamage = 1;

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUp(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1)
			bResult |= ProcessSkillEffect2SkillLevelUPType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2)
			bResult |= ProcessSkillEffect2SkillLevelUPType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3)
			bResult |= ProcessSkillEffect2SkillLevelUPType3(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE1)
			bResult |= ProcessSkillEffect2SkillLevelUPType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE2)
			bResult |= ProcessSkillEffect2SkillLevelUPType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP] & AGPMSKILL_EFFECT_DETAIL_SKILL_LEVELUP_TYPE3)
			bResult |= ProcessSkillEffect2SkillLevelUPType3(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffectProduct(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// do nothing
	return TRUE;
}

// 2005.07.18. steeple
// Action on Action Type3 
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// �׳� �ϴ� ���ø� �س��´�.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lActionOnActionType3Count++;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lActionOnActionType3Count = 1;

	return TRUE;
}

// 2005.12.13. steeple
// Action on Action Type4. ex) Lens Stone
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// �׳� �ϴ� ���ø� �س��´�.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lActionOnActionType4Count = 1;

	return TRUE;
}

// 2005.07.07. steeple
// AT Field �� ��ų�� ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2ATField(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK)
			bResult |= ProcessSkillEffect2ATFieldAttack(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2)
			bResult |= ProcessSkillEffect2ATFieldAttack2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA)
			bResult |= ProcessSkillEffect2ATFieldAura(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK)
			bResult |= ProcessSkillEffect2ATFieldAttack(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_ATTACK2)
			bResult |= ProcessSkillEffect2ATFieldAttack2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_AT_FIELD] & AGPMSKILL_EFFECT_DETAIL_AT_FIELD_AURA)
			bResult |= ProcessSkillEffect2ATFieldAura(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.15. steeple
// ��ȯ�� ����
BOOL AgsmSkillEffect::ProcessSkillEffect2Summons(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1)
			bResult |= ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2)
			bResult |= ProcessSkillEffect2SummonsType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3)
			bResult |= ProcessSkillEffect2SummonsType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4)
			bResult |= ProcessSkillEffect2SummonsType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5)
			bResult |= ProcessSkillEffect2SummonsType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6)
			bResult |= ProcessSkillEffect2SummonsType6(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7)
			bResult |= ProcessSkillEffect2SummonsType7(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8)
			bResult |= ProcessSkillEffect2SummonsType8(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE9)
			bResult |= ProcessSkillEffect2SummonsType9(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10)
			bResult |= ProcessSkillEffect2SummonsType10(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE1)
			bResult |= ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE2)
			bResult |= ProcessSkillEffect2SummonsType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE3)
			bResult |= ProcessSkillEffect2SummonsType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE4)
			bResult |= ProcessSkillEffect2SummonsType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5)
			bResult |= ProcessSkillEffect2SummonsType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE6)
			bResult |= ProcessSkillEffect2SummonsType6(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE7)
			bResult |= ProcessSkillEffect2SummonsType7(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE8)
			bResult |= ProcessSkillEffect2SummonsType8(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE9)
			bResult |= ProcessSkillEffect2SummonsType9(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SUMMONS] & AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE10)
			bResult |= ProcessSkillEffect2SummonsType10(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.15. steeple
// Action Passive Skill
BOOL AgsmSkillEffect::ProcessSkillActionPassive(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// ���⼭�� �״��� ���� ����.
	// Post Attack ���� Passive Skill �� �ѹ��� ���鼭 üũ�ؾ� �� �� �ʹ�.

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonus(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP)
			bResult |= ProcessSkillEffect2GameBonusExp(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY)
			bResult |= ProcessSkillEffect2GameBonusMoney(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE)
			bResult |= ProcessSkillEffect2GameBonusDropRate(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2)
			bResult |= ProcessSkillEffect2GameBonusDropRate2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE)
			bResult |= ProcessSkillEffect2GameBonusCharismaRate(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_EXP)
			bResult |= ProcessSkillEffect2GameBonusExp(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_MONEY)
			bResult |= ProcessSkillEffect2GameBonusMoney(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE)
			bResult |= ProcessSkillEffect2GameBonusDropRate(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_DROP_RATE2)
			bResult |= ProcessSkillEffect2GameBonusDropRate2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_BONUS] & AGPMSKILL_EFFECT_DETAIL_GAME_BONUS_CHARISMA_RATE)
			bResult |= ProcessSkillEffect2GameBonusCharismaRate(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.07.04. steeple
// Detect Type �� �þ�鼭 �߰� �ڵ� �ߴ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2Detect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1)
			bResult |= ProcessSkillEffect2DetectType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2)
			bResult |= ProcessSkillEffect2DetectType2(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE1)
			bResult |= ProcessSkillEffect2DetectType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DETECT] & AGPMSKILL_EFFECT_DETAIL_DETECT_TYPE2)
			bResult |= ProcessSkillEffect2DetectType2(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2006.10.10. steeple
// Ride Skill
// ��ų ���� Ż�Ϳ� Ÿ��, �ѹ� �� ����ϸ� �����´�.
// �׷��Ƿ�, ��Ÿ���� ���� ����� ��.
BOOL AgsmSkillEffect::ProcessSkillEffect2Ride(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 2007.04.24. steeple
	// ��ũ�ε� ��ų�̰�, ���� ��ũ�ε� ������ �������̸� �� �� ����.
	if(m_pagpmSkill->IsArchlordSkill(pcsSkillTemplate) && m_pagpmCharacter->IsArchlord((AgpdCharacter*)pcsTarget))
	{
		AgpdSiegeWar* pcsSiegeWar = m_pagpmSiegeWar->GetArchlordCastle();
		if(pcsSiegeWar && m_pagpmSiegeWar->IsStarted(pcsSiegeWar))
			return TRUE;
	}

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lRideTID = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];

	m_pagsmSkill->EnumCallback(AGSMSKILL_CB_RIDE, static_cast<AgpdCharacter*>(pcsTarget), &lRideTID);

	return TRUE;
}

// 2006.10.10. steeple
// Game Effect. Take some effect to the game environment.
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffect(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT)
			bResult |= ProcessSkillEffect2GameEffectDayNight(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN)
			bResult |= ProcessSkillEffect2GameEffectRain(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW)
			bResult |= ProcessSkillEffect2GameEffectSnow(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_DAY_NIGHT)
			bResult |= ProcessSkillEffect2GameEffectDayNight(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_RAIN)
			bResult |= ProcessSkillEffect2GameEffectRain(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT] & AGPMSKILL_EFFECT_DETAIL_GAME_EFFECT_SNOW)
			bResult |= ProcessSkillEffect2GameEffectSnow(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.06.26. steeple
// �������� ������.
BOOL AgsmSkillEffect::ProcessSkillEffect2Divide(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR)
			bResult |= ProcessSkillEffect2DivideAttr(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL)
			bResult |= ProcessSkillEffect2DivideNormal(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR)
			bResult |= ProcessSkillEffect2DivideAttr(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DIVIDE] & AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL)
			bResult |= ProcessSkillEffect2DivideNormal(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.08.23. steeple
// ����� ��Ȱ��Ű���� �޽����� ������ �Ѵ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2Resurrection(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!m_pagpmWantedCriminal || !pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	if(m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget) == FALSE || _tcslen(((AgpdCharacter*)pcsTarget)->m_szID) == 0)
		return FALSE;

	// ���� ������� ��Ȱ ��ų �� ����.
	if(m_pagpmWantedCriminal->GetWantedCriminal(((AgpdCharacter*)pcsTarget)->m_szID))
		return FALSE;

	AgsdCharacter* pcsAgsdTarget = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
	if(pcsAgsdTarget)
		_tcsncpy(pcsAgsdTarget->m_szResurrectionCaster, ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID, _tcslen(((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID));

	// Ÿ�ٿ��� ��Ȱ�ҰųĴ� Ȯ��â�� ����ش�.
	m_pagsmCharacter->SendPacketResurrectionByOther((AgpdCharacter*)pcsTarget, ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID, 0);

	return TRUE;
}

// 2007.10.29. steeple
// Union Type ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2Union(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1)
			bResult |= ProcessSkillEffect2UnionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2)
			bResult |= ProcessSkillEffect2UnionType2(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE1)
			bResult |= ProcessSkillEffect2UnionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_SKILL_UNION] & AGPMSKILL_EFFECT_DETAIL_SKILL_UNION_TYPE2)
			bResult |= ProcessSkillEffect2UnionType2(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}


//		ProcessSkillEffectMeleeAttackType1
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE1) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// ������ ���� ���� �����ϰ�, �ڵ� �����Ͽ���. ���ʿ��� �ּ��� ���� ����.

	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel =  m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// ��� ��ų���� ���� �������� �˻��Ͽ��� �Ѵ�.
	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lNormalSkillDamage = m_pagsmSkill->CalcNormalSkillDamage(pcsSkill, pcsTarget);
	INT32 lSpiritSkillDamage = m_pagsmSkill->CalcSpiritSkillDamage(pcsSkill, pcsTarget, &stAttrDamage);
	INT32 lHeroicSkillDamage = m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);	

	if(lNormalSkillDamage <= 0)
		lNormalSkillDamage = 1;

	if(lSpiritSkillDamage <= 0)
		lSpiritSkillDamage = 1;

	if(lHeroicSkillDamage <= 0)
		lHeroicSkillDamage = 1;

	INT32 lDamage = lNormalSkillDamage;
	if(lDamage <= 0)
		lDamage	= 1;

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage ó�����ش�. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, &stAttrDamage, &lDamage, &lSpiritSkillDamage);
	}

	// ���̿� ���� ������ ��ȭ. 2006.06.23. steeple
	if(!bProcessInterval)		// �������� ���� �� ���� �ʴ´�.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
		lSpiritSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lSpiritSkillDamage);
		lHeroicSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lHeroicSkillDamage);
	}

	// �� ���� �������� �������ش�.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lHeroicSkillDamage;
	lDamage += lSpiritSkillDamage;
	lDamage += lHeroicSkillDamage;

	// Damage�� �ݿ��Ѵ�.
	// ���� ĳ���Ϳ� �ݿ��ϰ� History���� �ݿ��Ѵ�.
	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor* pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	// ���� ������ ����
	for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; i++)
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	// Heroic ������ ����
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType2
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE2) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//		
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType2(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// ���ʿ��� �ּ��� ���� ����.

	if (!pcsSkill || !pcsTarget)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	lDamage	= (INT32) pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];
	}

	// Divide Damage ó�����ش�. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &lDamage, NULL);
	}

	// ���̿� ���� ������ ��ȭ. 2006.06.23. steeple
	if(!bProcessInterval)		// �������� ���� �� ���� �ʴ´�.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
	}

	// Damage�� �ݿ��Ѵ�.
	// ���� ĳ���Ϳ� �ݿ��ϰ� History���� �ݿ��Ѵ�.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-lDamage);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType3
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE3) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType3(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// MeleeAttackType3 �� MeleeAttackType1 �� ����������.
	return ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, nIndex, bProcessInterval);
}

//		ProcessSkillEffectMeleeAttackType4
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE4) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���� : {������ ��� * (Air ���� ����) + ������ ��� * (1-((1-(400/Defense))/2)) (����)}
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType4(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdFactor	*pcsAttackFactorResult	= m_pagsmSkill->GetOwnerFactorResult(pcsSkill);
	AgpdFactor	*pcsTargetFactorResult	= m_pagsmSkill->GetResultFactor(pcsTarget);

	if (!pcsAttackFactorResult || !pcsTargetFactorResult)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	lAttackMAP			= 0;
	FLOAT	fTargetAirResist	= 0.0f;

	m_pagpmFactors->GetValue(pcsAttackFactorResult, &lAttackMAP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MAP);
	m_pagpmFactors->GetValue(pcsTargetFactorResult, &fTargetAirResist, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	BOOL	bIsEquipWeapon		= m_pagpmItem->IsEquipWeapon((AgpdCharacter *) pcsSkill->m_pcsBase);

	// 2004.12.29. steeple ������ ������..
	INT32	lNormalAttackDamage	= m_pagpmCombat->CalcPhysicalAttackForSkill((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	//INT32	lNormalAttackDamage	= m_pagpmCombat->CalcPhysicalAttack((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	if (lNormalAttackDamage < 0)
		lNormalAttackDamage	= 0;

	FLOAT	fAirDefense	= (FLOAT)(lNormalAttackDamage * (lAttackMAP / (fTargetAirResist + 0.0)));

	INT32	nDamage = (INT32)((FLOAT)m_pagsmSkill->GetConstDamageA(pcsSkill, pcsTarget) * fAirDefense);

	FLOAT	fDefensePoint	= 0.0f;
	m_pagpmFactors->GetValue(pcsAttackFactorResult, &fDefensePoint, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);

	FLOAT	fTemp			= 0;
	if (fDefensePoint != 0) fTemp = (FLOAT)(400.0 / (fDefensePoint + 0.0));

	INT32 lConstDamage = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_B][lSkillLevel];
	
	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		lConstDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_B][lSkillLevel];
	}

	nDamage	+= (INT32) (lConstDamage * (1 - ((1 - fTemp) / 2.0)));

	if (nDamage <= 0)
		return FALSE;

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0 && !pcsSkill->m_bCloneObject)
		nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage ó�����ش�. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &nDamage, NULL);
	}

	// ���̿� ���� ������ ��ȭ. 2006.06.23. steeple
	if(!bProcessInterval)		// �������� ���� �� ���� �ʴ´�.
	{
		nDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, nDamage);
	}

	/*
	// defense�� reflect�� �ϴ��� �˻��Ѵ�.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage�� �ݿ��Ѵ�.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-nDamage);

	/*
	if (lReflectDamage > 0)
	{
		// ������ ������ lReflectDamage �� �ǵ����ش�.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackType5
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MELEE_ATTACK_DMG_TYPE5) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���� : �Ϲݵ����� ���� + DOT_DMG (Damage On Time)
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType5(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdFactor	*pcsAttackFactorResult	= m_pagsmSkill->GetOwnerFactorResult(pcsSkill);
	AgpdFactor	*pcsTargetFactorResult	= m_pagsmSkill->GetResultFactor(pcsTarget);

	if (!pcsAttackFactorResult || !pcsTargetFactorResult)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	INT32	nDamage	= 0;

	if (!bProcessInterval)
	{
		// physical damage�� ���Ѵ�.
		BOOL	bIsEquipWeapon	= m_pagpmItem->IsEquipWeapon((AgpdCharacter *) pcsSkill->m_pcsBase);

		// 2004.12.29. steeple ������ ������...
		nDamage = m_pagpmCombat->CalcPhysicalAttackForSkill((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
		//nDamage = m_pagpmCombat->CalcPhysicalAttack((AgpdCharacter *) pcsSkill->m_pcsBase, (AgpdCharacter *) pcsTarget, bIsEquipWeapon);
	}

	//nDamage += pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DOT_DMG][lSkillLevel];

	if (nDamage <= 0)
		return FALSE;

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0 && !pcsSkill->m_bCloneObject)
		nDamage += (INT32) (nDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// Divide Damage ó�����ش�. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, NULL, &nDamage, NULL);
	}

	// ���̿� ���� ������ ��ȭ. 2006.06.23. steeple
	if(!bProcessInterval)		// �������� ���� �� ���� �ʴ´�.
	{
		nDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, nDamage);
	}
	/*
	// defense�� reflect�� �ϴ��� �˻��Ѵ�.
	stAgsmCombatAttackResult	stAttackResult;
	stAttackResult.pAttackChar = (AgpdCharacter *) pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar = (AgpdCharacter *) pcsTarget;
	stAttackResult.nDamage = nDamage;

	INT32	lReflectDamage = 0;

	EnumCallback(AGSMSKILL_CB_MELEE_ATTACK_CHECK, &stAttackResult, &lReflectDamage);

	nDamage = stAttackResult.nDamage;
	*/

	// Damage�� �ݿ��Ѵ�.
	AgsdSkill	*pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor	*pcsFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if (!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-nDamage);
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL] += (-nDamage);

	/*
	if (lReflectDamage > 0)
	{
		// ������ ������ lReflectDamage �� �ǵ����ش�.
		ProcessReflectDamage(pcsTarget, pcsSkill->m_pcsBase, lReflectDamage);
	}
	*/

	return TRUE;
}

//	2004.09.02. steeple
//		ProcessSkillEffectMeleeAttackType6
//	Functions
//		- ProcessSkillEffectMeleeAttackType1 �� ���� ���� ����, 
//		- ���� �ٸ� Ÿ�ٿ��� �߰� �������� ����.
//
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackType6(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	// 2004.12.29. steeple
	// ������ ���� �������� ���ؼ� ��¦ ����

	if(!pcsSkill || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel =  m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// ��� ��ų���� ���� �������� �˻��Ͽ��� �Ѵ�.
	AgpdFactorAttribute	stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lNormalSkillDamage = m_pagsmSkill->CalcNormalSkillDamage(pcsSkill, pcsTarget);
	INT32 lSpiritSkillDamage = m_pagsmSkill->CalcSpiritSkillDamage(pcsSkill, pcsTarget, &stAttrDamage);
	INT32 lHeroicSkillDamage = m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);

	if(lNormalSkillDamage <= 0)
		lNormalSkillDamage = 1;

	if(lSpiritSkillDamage <= 0)
		lSpiritSkillDamage = 1;

	if(lHeroicSkillDamage <= 0)
		lHeroicSkillDamage = 1;

	INT32 lDamage = lNormalSkillDamage;
	if(lDamage <= 0)
		lDamage	= 1;

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// 2004.09.02. steeple
	if(pcsSkill->m_csTargetBase.m_lID != pcsTarget->m_lID)
	{
		// Ÿ���� ���� �ٸ� ���̴�.
		// �� �� �߰� �������� �ش�.
		lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];

		// 2007.10.25. steeple
		if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		{
			lDamage += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_DAMAGE_A][lSkillLevel];
		}
	}
	
	// Divide Damage ó�����ش�. 2007.06.27. steeple
	if(!bProcessInterval)
	{
		m_pagsmSkill->ProcessDivideDamage(pcsSkill, (AgpdCharacter*)pcsTarget, &stAttrDamage, &lDamage, &lSpiritSkillDamage);
	}

	// ���̿� ���� ������ ��ȭ. 2006.06.23. steeple
	if(!bProcessInterval)		// �������� ���� �� ���� �ʴ´�.
	{
		lDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lDamage);
		lSpiritSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lSpiritSkillDamage);
		lHeroicSkillDamage += m_pagsmCombat->AdjustHeightDamage((AgpdCharacter*)pcsSkill->m_pcsBase, (AgpdCharacter*)pcsTarget, lHeroicSkillDamage);
	}

	// �� ���� �������� �������ش�.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lHeroicSkillDamage;
	lDamage += lSpiritSkillDamage;
	lDamage += lHeroicSkillDamage;

	// Damage�� �ݿ��Ѵ�.
	// ���� ĳ���Ϳ� �ݿ��ϰ� History���� �ݿ��Ѵ�.
	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor* pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint* pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*) m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);
	// ���� ������ ����
	for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; i++)
		pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL + i] += (-stAttrDamage.lValue[i]);

	// Heroic Damage����
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC]);

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackCritical
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//					(���� Ȯ���� ���� ������ ���� (�߻�Ȯ��:arg1, ���� ������(%): arg2))
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	// Attack Damage �� ���õǾ� �ִ� ��ų�̶��, Factor �ø��� �۾��� ���� �ʴ´�.
	if(m_pagsmSkill->IsSetAttackDamageOnly(pcsSkill))
		return TRUE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MELEE_ATTACK_CRITICAL;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CRITICAL][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate		+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeCriticalProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMeleeDamageAdjustRate;

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackDeath
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//					(ũ��Ƽ�� ����(melee attack 106) �� �� ��� arg1�� Ȯ���� Ÿ���� ����Ų��.)
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackDeath(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MELEE_ATTACK_DEATH;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability			= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount				= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_DEATH][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability		+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount			+= (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_DEATH][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDeathProbability			+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDeathAmount				+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDeathAmount;

	return TRUE;
}

//		ProcessSkillEffectMagicAttackCritical
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttackCritical(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	INT32	lSkillLevel	= m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if (lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect								|= AGPMSKILL_EFFECT_MAGIC_ATTACK_CRITICAL;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate		= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_CRITICAL][lSkillLevel];

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
	{
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_RATE][lSkillLevel];
		pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate		+= (INT32)((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_CRITICAL][lSkillLevel];
	}

	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicCriticalProbability;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate	+= pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lMagicDamageAdjustRate;

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackDurability
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_DURABILITY) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackDurability(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �� ��ų�� Ȯ���� ���ؼ� �����϶��� �����Ų��.
	INT32 lRandom	 = m_csRandom->randInt(100);
	INT32 nSkillRate = m_pagsmSkill->GetSkillRate(pcsSkill);

	if(nSkillRate < lRandom) 
		return FALSE;

	INT32	  ArrEquipItem[AGPM_DURABILITY_EQUIP_PART_MAX] = { -1, };

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(lSkillLevel < 1) return FALSE;

	INT32 lCount		 = 0;
	INT32 lMax			 = m_pagpmItem->GetEquipItems((AgpdCharacter *)pcsTarget, ArrEquipItem, (INT32)AGPM_DURABILITY_EQUIP_PART_MAX);
	INT32 lDurabilityNum = (INT32)((AgpdSkillTemplate*)(pcsSkill->m_pcsTemplate))->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_NUM][lSkillLevel];
	FLOAT lDurabilityPer = ((AgpdSkillTemplate*)(pcsSkill->m_pcsTemplate))->m_fUsedConstFactor2[AGPMSKILL_CONST_ARMOR_DURABILITY_PERCENT][lSkillLevel];

	if(lMax < lDurabilityNum) lDurabilityNum = lMax;

	if(lMax < 1) return FALSE;

	for(INT32 lCount = 0; lCount < lDurabilityNum; ++lCount)
	{
		AgpdItem* pEquipItem = NULL;
		INT32 lRandPart = m_csRandom->randInt(lMax);

		if(ArrEquipItem[lRandPart] > AGSMSKILL_EFFECT_NONCHECK_DURABILITY && ArrEquipItem[lRandPart] < AGPM_DURABILITY_EQUIP_PART_MAX)
		{
			pEquipItem = m_pagpmItem->GetEquipSlotItem((AgpdCharacter*)pcsTarget, (AgpmItemPart)AgpmItem::s_lBeDurabilityEquipItem[ArrEquipItem[lRandPart]]);
		}

		if(pEquipItem)
		{
			INT32 lMaxDurability	 = m_pagpmItem->GetItemDurabilityMax(pEquipItem);
			INT32 lCurrentDurability = m_pagpmItem->GetItemDurabilityCurrent(pEquipItem);
			INT32 lChangeDurability  = (INT32)((lMaxDurability * (FLOAT)(1 + lDurabilityPer / 100.0)) - lMaxDurability);

			if(lCurrentDurability + lChangeDurability < AGSMSKILL_EFFECT_LIMIT_DURABILITY) 
			{
				lChangeDurability = AGSMSKILL_EFFECT_LIMIT_DURABILITY - lCurrentDurability;
			}
										
			if(!m_pagsmItem->UpdateItemDurability(pEquipItem, lChangeDurability))
			{
				//
			}

			ArrEquipItem[lRandPart] = AGSMSKILL_EFFECT_NONCHECK_DURABILITY;			
		}
	}

	return TRUE;
}

//		ProcessSkillEffectMeleeAttackHeroic
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_MELEE_ATTACK_HEROIC) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMeleeAttackHeroic(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsBase)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	AgpdFactorAttribute stAttrDamage;
	ZeroMemory(&stAttrDamage, sizeof(AgpdFactorAttribute));

	INT32 lHeroicDamage		= m_pagsmSkill->CalcNormalHeroicSkillDamage(pcsSkill, pcsTarget);

	INT32 lDamage = lHeroicDamage;
	if(lDamage <= 0)
		lDamage = 1;

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	if(lDamage > 0 && !pcsSkill->m_bCloneObject)
		lDamage += (INT32) (lDamage * (m_pagsmParty->GetPartyBonusDamage(pcsSkill->m_pcsBase, pcsSkill) / 100.0));

	// �� ���� �������� �������ش�
	// ������ HeroicDamage�� Heroic�� ���� ���̹Ƿ� ���� = �������̴�.
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL] = lDamage;
	stAttrDamage.lValue[AGPD_FACTORS_ATTRIBUTE_TYPE_HEROIC] = lDamage;

	// Damage�� �ݿ�
	AgsdSkill *pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(NULL == pcsAgsdSkill)
		return FALSE;
	
	if(!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor *pcsFactorResult = (AgpdFactor*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if(NULL == pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->GetFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
	if(!pcsFactorCharPoint)
	{
		pcsFactorCharPoint = (AgpdFactorCharPoint*)m_pagpmFactors->InitCalcFactor(pcsFactorResult, AGPD_FACTORS_TYPE_CHAR_POINT);
		if(!pcsFactorCharPoint)
			return FALSE;
	}

	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (-lDamage);

	// Heroic Facotr Setting
	pcsFactorCharPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_DMG_HEROIC] += (-lDamage);

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorHOT
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_HOT) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ union factor point�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorHOT(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// �ѹ��� ���� ����Ǵ� ��ų���� üũ�Ѵ�.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	if (!m_pagsmSkill->SetUpdateFactorResult(pcsAgsdSkill))
		return FALSE;

	AgpdFactor			*pcsFactorResult	= (AgpdFactor *)			m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorResult, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsFactorResult)
		return FALSE;

	AgpdFactorCharPoint	*pcsFactorPoint		= (AgpdFactorCharPoint *)	m_pagpmFactors->SetFactor(pcsFactorResult, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorPoint)
		return FALSE;

	pcsFactorPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_HOT][lSkillLevel];
	
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		pcsFactorPoint->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] += (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_HOT][lSkillLevel];

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorMagnify
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_MAGNIFY) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ union factor point�� ���Ѵ�. ((���� FactorValue)*(1+(FactorValue/100)))
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorMagnify(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;
	
	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// �ѹ��� ���� ����Ǵ� ��ų���� üũ�Ѵ�.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	AgpdFactorDIRT		*pcsFactorDIRT		= (AgpdFactorDIRT *)	m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	for (int i = AGPMSKILL_CONST_MAGNIFY_START + 1; i < AGPMSKILL_CONST_MAGNIFY_END; ++i)
	{
		// '0' ���� �ƴ����� �˻��ؼ� '0'�� �ƴϸ� target�� factor point�� ���Ѵ�.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				if (fPrevValue > 0.0f)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, (FLOAT) (fPrevValue * (1 + fPrevValue / 100.0)), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				if (fPrevValue > 0.0f)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, (FLOAT) (fPrevValue * (1 + fPrevValue / 100.0)), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorAdd
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_UPDATE_FACTOR) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorAdd(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill			*pcsAgsdSkill		= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate	= (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// 2006.11.27. steeple
	// �ѹ��� ���� ����Ǵ� ��ų���� üũ�Ѵ�.
	if(bProcessInterval && m_pagsmSkill->CheckOnceAffected(pcsSkillTemplate))
		return TRUE;

	AgpdFactorDIRT		*pcsFactorDIRT		= (AgpdFactorDIRT *)	m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if (!pcsFactorDIRT)
		return FALSE;

	INT32	lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL	bIsUpdateMovement	= FALSE;
	BOOL	bCheckConnection	= FALSE;

	// 2007.10.25. steeple
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		bCheckConnection = TRUE;

	for (int i = AGPMSKILL_CONST_POINT_START + 1; i < AGPMSKILL_CONST_POINT_END; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		if(i == AGPMSKILL_CONST_POINT_CHA) continue; // ī�������� ������ �ƴ�. arycoat - 2008.7

		//////////////////////////////////////////////////////////////////////////
		// '0' ���� �ƴ����� �˻��ؼ� '0'�� �ƴϸ� target�� factor point�� ���Ѵ�.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] �� 0 ���� ���� �� �ؾ� �Ѵ�.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// �Ʒ��� ���� ��Ȳ�̶�� ���ο� ���� �ʴ´�.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				// AttackRange �� Factor ���� cm ������ ����ȴ�. Skill_Const ������ m ������ �´�. 2006.03.28. steeple
				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_ATTACK && g_alFactorTable[i][1] == AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + (fConstValue * 100.0f), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				else
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	for (int i = AGPMSKILL_CONST_PERCENT_START + 1; i < AGPMSKILL_CONST_PERCENT_END; ++i)
	{
		//////////////////////////////////////////////////////////////////////////
		//
		if(i == AGPMSKILL_CONST_PERCENT_CHA) continue; // ī�������� ������ �ƴ�. arycoat - 2008.7

		// '0' ���� �ƴ����� �˻��ؼ� '0'�� �ƴϸ� target�� factor percent�� ���Ѵ�.
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if (!m_pagsmSkill->SetUpdateFactorPercent(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus	*pcsFactorTemp  = (AgpdFactorCharStatus *) m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPercent, NULL, g_alFactorTable[i][0]);
			if (!pcsFactorTemp)
				continue;

			if (g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] �� 0 ���� ���� �� �ؾ� �Ѵ�.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// �Ʒ��� ���� ��Ȳ�̶�� ���ο� ���� �ʴ´�.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	if (bIsUpdateMovement)
	{
		// �ٸ� ��⿡�� Slow�� ���õ� �����ؾ��� ���� �ִ��� �����´�.

		UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
		UINT32	ulAdjustDuration	= 0;
		INT32	lAdjustProbability	= 0;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
		pvBuffer[1]	= (PVOID)	pcsSkill;
		pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
		pvBuffer[3]	= (PVOID)	&lAdjustProbability;

		m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

		if (ulAdjustDuration > 0)
		{
			pcsSkill->m_ulEndTime	-= ulAdjustDuration;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorItem(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	BOOL retval = m_pagsmSkill->ApplyUpdateFactorItem(pcsSkill, pcsTarget, nIndex, TRUE, TRUE, FALSE);
	
	return retval;
}

// 2005.07.07. steeple
// �ð��� ���� �÷��� Factor �� �ٲ��.
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorTime(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate	= (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgpdFactorDIRT* pcsFactorDIRT = (AgpdFactorDIRT*)m_pagpmFactors->GetFactor(&pcsSkill->m_csFactor, AGPD_FACTORS_TYPE_DIRT);
	if(!pcsFactorDIRT)
		return FALSE;

	// ���� �ð��� ���Ѵ�.
	UINT32 ulHour = m_pagpmEventNature->GetHour(m_pagpmEventNature->GetGameTime());
	BOOL bIsNight = FALSE;
	if(ulHour < 6 || ulHour >= 18)
		bIsNight = TRUE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bIsUpdateMovement = FALSE;
	BOOL bCheckConnection = FALSE;
	if(m_pagsmSkill->CheckConnectionInfo(pcsSkill, pcsTarget, bProcessInterval))
		bCheckConnection = TRUE;

	for(INT32 i = AGPMSKILL_CONST_POINT_START + 1; i < AGPMSKILL_CONST_POINT_END; i++)
	{
		// '0' ���� �ƴ����� �˻��ؼ� '0'�� �ƴϸ� target�� factor point�� ���Ѵ�.
		if(pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel] != 0)
		{
			if(pcsSkillTemplate->m_stConditionArg[nIndex].lArg1 == 1)	// ������ ����. �㿡�� ���ݷ� ����.
			{
				if(!bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DEFENSE)
					continue;		// ���ε� Factor Table �� AGPD_FACTORS_TYPE_DEFENSE �� �ƴϸ� Skip

				if(bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DAMAGE)
					continue;		// ���ε� Factor Table �� AGPD_FACTORS_TYPE_DAMAGE �� �ƴϸ� Skip
			}

			if (!m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus* pcsFactorTemp = (AgpdFactorCharStatus*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPoint, NULL, g_alFactorTable[i][0]);
			if(!pcsFactorTemp)
				continue;

			if(g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT fPrevValue = 0.0f;
				FLOAT fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue	= pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] �� 0 ���� ���� �� �ؾ� �Ѵ�.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// �Ʒ��� ���� ��Ȳ�̶�� ���ο� ���� �ʴ´�.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				// AttackRange �� Factor ���� cm ������ ����ȴ�. Skill_Const ������ m ������ �´�. 2006.03.28. steeple
				if (g_alFactorTable[i][1] == AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE)
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + (fConstValue * 100.0f), (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
				else
					m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fPrevValue + fConstValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	for(INT32 i = AGPMSKILL_CONST_PERCENT_START + 1; i < AGPMSKILL_CONST_PERCENT_END; i++)
	{
		// '0' ���� �ƴ����� �˻��ؼ� '0'�� �ƴϸ� target�� factor percent�� ���Ѵ�.
		if(pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] != 0)
		{
			if(pcsSkillTemplate->m_stConditionArg[nIndex].lArg1 == 1)	// ������ ����. �㿡�� ���ݷ� ����.
			{
				if(!bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DEFENSE)
					continue;		// ���ε� Factor Table �� AGPD_FACTORS_TYPE_DEFENSE �� �ƴϸ� Skip

				if(bIsNight && g_alFactorTable[i][0] != AGPD_FACTORS_TYPE_DAMAGE)
					continue;		// ���ε� Factor Table �� AGPD_FACTORS_TYPE_DAMAGE �� �ƴϸ� Skip
			}

			if (!m_pagsmSkill->SetUpdateFactorPercent(pcsAgsdSkill))
				continue;

			AgpdFactorCharStatus* pcsFactorTemp  = (AgpdFactorCharStatus*)m_pagpmFactors->SetFactor(pcsAgsdSkill->m_pcsUpdateFactorPercent, NULL, g_alFactorTable[i][0]);
			if(!pcsFactorTemp)
				continue;

			if(g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DAMAGE || g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_DEFENSE)
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1], g_alFactorTable[i][2]);
			}
			else
			{
				FLOAT	fPrevValue	= 0.0f;
				FLOAT	fConstValue = pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel];
				if(bCheckConnection)
					fConstValue += pcsSkillTemplate->m_fUsedConstFactor2[i][lSkillLevel];

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType) g_alFactorTable[i][0], g_alFactorTable[i][1]);

				if ((eAgpdFactorsType) g_alFactorTable[i][0] == AGPD_FACTORS_TYPE_CHAR_STATUS &&
					(g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT || g_alFactorTable[i][1] == AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST))
				{
					// pcsSkillTemplate->m_fUsedConstFactor[i][lSkillLevel] �� 0 ���� ���� �� �ؾ� �Ѵ�.
					AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter((AgpdCharacter*)pcsTarget);
					if(fConstValue < 0.0f && pcsAgsdCharacter && pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > 0)
					{
						// �Ʒ��� ���� ��Ȳ�̶�� ���ο� ���� �ʴ´�.
						INT32 lRandom = m_csRandom->randInt(100);
						if(pcsAgsdCharacter->m_stInvincibleInfo.lSlowProbability > lRandom)
							continue;
					}

					bIsUpdateMovement	= TRUE;
				}

				m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fPrevValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1]);
				m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fPrevValue + fConstValue, (eAgpdFactorsType)g_alFactorTable[i][0], g_alFactorTable[i][1]);
			}
		}
	}

	if (bIsUpdateMovement)
	{
		// �ٸ� ��⿡�� Slow�� ���õ� �����ؾ��� ���� �ִ��� �����´�.

		UINT64	ulSpecialStatus		= (UINT64)	AGPDCHAR_SPECIAL_STATUS_SLOW;
		UINT32	ulAdjustDuration	= 0;
		INT32	lAdjustProbability	= 0;

		PVOID	pvBuffer[4];
		pvBuffer[0]	= (PVOID)	&ulSpecialStatus;
		pvBuffer[1]	= (PVOID)	pcsSkill;
		pvBuffer[2]	= (PVOID)	&ulAdjustDuration;
		pvBuffer[3]	= (PVOID)	&lAdjustProbability;

		m_pagsmSkill->EnumCallback(AGSMSKILL_CB_CHECK_SPECIAL_STATUS_FACTOR, pcsTarget, pvBuffer);

		if (ulAdjustDuration > 0)
		{
			pcsSkill->m_ulEndTime	-= ulAdjustDuration;
		}
	}

	return TRUE;
}

// 2008.06.11. iluvs.
// Detail Reflect_Melee_Attack
BOOL AgsmSkillEffect::ProcessSkillEffect2ReflectMeleeAtk(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval /* = FALSE */)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData	*pcsAttachData	= m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if (!pcsAttachData) return FALSE;

	AgsdSkill			*pcsAgsdSkill	= m_pagsmSkill->GetADSkill(pcsSkill);
	if (!pcsAgsdSkill) return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(lSkillLevel < 1)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence;
		}

	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL)
		{
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_PHYSICAL;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_PHYSICAL_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnorePhysicalDefence;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] & AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE)
		{	
			pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK] |= AGPMSKILL_EFFECT_DETAIL_REFLECT_MELEE_ATK_IGNORE_ATTRIBUTE;

			pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_IGNORE_ATTRIBUTE_DEFENCE_PERCENT][lSkillLevel];

			pcsAttachData->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence += pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIgnoreAttributeDefence;
		}
		
	}

	return bResult;
}

// 2007.07.03. steeple
// Dispel �� �þ��.
BOOL AgsmSkillEffect::ProcessSkillEffect2Dispel(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN)
			bResult |= ProcessSkillEffect2DispelStun(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW)
			bResult |= ProcessSkillEffect2DispelSlow(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF)
			bResult |= ProcessSkillEffect2DispelAllBuff(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT)
			bResult |= ProcessSkillEffect2DispelTransparent(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS)
			bResult |= ProcessSkillEffect2DispelSpecialStatus(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_STUN)
			bResult |= ProcessSkillEffect2DispelStun(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SLOW)
			bResult |= ProcessSkillEffect2DispelSlow(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_ALL_BUFF)
			bResult |= ProcessSkillEffect2DispelAllBuff(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_TRANSPARENT)
			bResult |= ProcessSkillEffect2DispelTransparent(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISPEL] & AGPMSKILL_EFFECT_DETAIL_DISPEL_SPECIAL_STATUS)
			bResult |= ProcessSkillEffect2DispelSpecialStatus(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// Level Up ��ġ�� �������ش�.
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lSkillLevelUpPoint += (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lSkillLevelUpPoint = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];

	// 2007.02.06. steeple
	// Modified Skill Level �� �ٽ� ������ش�.
	m_pagpmSkill->UpdateModifiedSkillLevel(pcsSkill->m_pcsBase);

	return TRUE;
}

// 2007.07.07. steeple
// ĳ���� ������ �÷��ش�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lCharLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);
	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SKILL_LEVELUP][lSkillLevel];
	//INT32 lValue = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsTarget);

	// 2008.04.08. steeple
	// Ÿ���� ���� ���� ������ �ְ�, ���� ������ �ö󰡴� ������ ���� ���� ������ �Ѿ�ٸ� �׳� ������.
	// Ÿ�ٿ��� ������ �� �־, �����ڷ� ����.
	INT32 lLimitedLevel = m_pagpmCharacter->GetCurrentRegionLevelLimit((AgpdCharacter*)pcsSkill->m_pcsBase);
	if(lLimitedLevel != 0 && (lCharLevel + lValue) > lLimitedLevel)
		return FALSE;

	// Level Up ��ġ�� �������ش�.
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lCharLevelUpPoint += lValue;
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lCharLevelUpPoint = lValue;

	if(m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget))
	{
		// PC ��� UpdateCharLevel �ҷ��ְ�
		//
		// ���߿� �۾�.
	}
	else
	{
		// Mob �̶�� �׳� ���Ϳ� �����Ѵ�.
		if(m_pagsmSkill->SetUpdateFactorPoint(pcsAgsdSkill))
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	}

	return TRUE;
}

// 2007.11.14. steeple
// Ư�� ��ų�� ������ 1 �÷��ش�. ���� ��� ��ų�鸸.
BOOL AgsmSkillEffect::ProcessSkillEffect2SkillLevelUPType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);

	int j = 0;
	pcsAgsdSkill->m_alLevelUpSkillTID.MemSetAll();

	for(int i = 0; i < AGPMSKILL_MAX_SKILL_LEVELUP_TID; ++i)
	{
		if(pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i] == 0)
			break;

		AgpdSkill* pcsOwnSkill = m_pagpmSkill->GetSkillByTID(static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase),
															pcsSkillTemplate->m_allLevelUpSkillTID[lSkillLevel][i]);
		if(pcsOwnSkill)
		{
			++pcsOwnSkill->m_lModifiedLevel;
			pcsAgsdSkill->m_alLevelUpSkillTID[j++] = pcsOwnSkill->m_pcsTemplate->m_lID;

			// ��Ŷ�� ������.
			m_pagsmSkill->SendSpecificSkillLevel(pcsOwnSkill);

			// Passive Skill �̸� �ٽ� ĳ�����Ѵ�.
			if(m_pagpmSkill->IsPassiveSkill(pcsOwnSkill))
			{
				m_pagsmSkill->EndPassiveSkill(pcsOwnSkill);
				m_pagsmSkill->CastPassiveSkill(pcsOwnSkill);
			}
		}
	}

	return TRUE;
}

// 2005.07.07. steeple
// AT Field Attack ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAttack(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// ���� ���¿��� �ҷȴ�.
		// �� ������ ������ ĳ���͸� ���ͼ� ������ �� �ִ� �ֵ� ������ ���ش�.
		//
		if(!pcsTarget)
			return FALSE;

		ApdEvent* pcsEvent = NULL;
		pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		if(!pcsEvent)
			pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, m_pagsmSkill->GetClockCount());

		if(!pcsEvent)
			return FALSE;

		ApSafeArray<INT_PTR, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer;
		lCIDBuffer.MemSetAll();
		ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer2;
		lCIDBuffer2.MemSetAll();

		// ���� range conditon�� �˻��� Ÿ���� �����´�.
		INT32 lNumTarget = m_pagsmSkill->RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], AGSMSKILL_MAX_TARGET_NUM - 1, &lCIDBuffer2[0], AGSMSKILL_MAX_TARGET_NUM - 1, pcsEvent);
		AgpdCharacter* pcsTargetCharacter = NULL;
		ApBase* pcsTargetBase = NULL;

		// ������ ���鼭 �������ش�.
		for(INT32 i = 0; i < lNumTarget; i++)
		{
			// �����̴ϱ� �ڱ�� �Ѿ��
			if(pcsTarget->m_lID == lCIDBuffer2[i])
				continue;

			pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(lCIDBuffer2[i]);
			if(!pcsTargetCharacter)
				continue;

			if(pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			{
				pcsTargetCharacter->m_Mutex.Release();
				continue;
			}

			// �������̸� �� �༮�� ���̴� 
			if(m_pagpmCharacter->IsPet(pcsTargetCharacter))
			{
				pcsTargetCharacter->m_Mutex.Release();
				continue;
			}

			m_pagsmSkill->ProcessATFieldAttack(pcsSkill, (AgpdCharacter*)pcsTarget, pcsTargetCharacter);

			pcsTargetCharacter->m_Mutex.Release();

			// ������ �� ���͵��� �ʱ�ȭ �Ѵ�.
			m_pagsmSkill->DestroyUpdateFactorPoint(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorPercent(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorResult(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorItemPoint(pcsAgsdSkill);
			m_pagsmSkill->DestroyUpdateFactorItemPercent(pcsAgsdSkill);
		}

		m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// �״��� �� �� ����. -0-;;
	}

	return TRUE;
}

// 2005.10.06. steeple
// AT Field Attack2 ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAttack2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// ���� ���¿��� �ҷȴ�.
		// �� ������ ������ ĳ���͸� ���ͼ� ������ �� �ִ� �ֵ� ������ ���ش�.
		//
		if(!pcsTarget)
			return FALSE;

		ApdEvent* pcsEvent = NULL;
		pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		if(!pcsEvent)
			pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, m_pagsmSkill->GetClockCount());

		if(!pcsEvent)
			return FALSE;

		ApSafeArray<INT_PTR, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer;
		lCIDBuffer.MemSetAll();
		ApSafeArray<INT32, AGSMSKILL_MAX_TARGET_NUM> lCIDBuffer2;
		lCIDBuffer2.MemSetAll();

		// ���� range conditon�� �˻��� Ÿ���� �����´�.
		INT32 lNumTarget = m_pagsmSkill->RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], AGSMSKILL_MAX_TARGET_NUM - 1, &lCIDBuffer2[0], AGSMSKILL_MAX_TARGET_NUM - 1, pcsEvent);
		AgpdCharacter* pcsTargetCharacter = NULL;
		ApBase* pcsTargetBase = NULL;

		// ������ ���鼭 �������ش�.
		for(INT32 i = 0; i < lNumTarget; i++)
		{
			pcsTargetBase = (ApBase*)lCIDBuffer[i];
			if(!pcsTargetBase)
				break;

			// �����̴ϱ� �ڱ�� �Ѿ��
			if(pcsTarget->m_lID == pcsTargetBase->m_lID)
				continue;

			pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(pcsTargetBase->m_lID);
			if(!pcsTargetCharacter)
				continue;

			// ������ ĳ���Ϳ��� �ٷ� ���� ȣ��
			m_pagpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, (AgpdCharacter*)pcsTarget, (ApBase*)pcsTargetCharacter, NULL);

			pcsTargetCharacter->m_Mutex.Release();
		}

		m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// �״��� �� �� ����. -0-;;
	}

	return TRUE;
}

// 2005.07.07. steeple
// AT Field Aura ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2ATFieldAura(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// �� ��ų�� Additional SkillTID �� ������ ���� ���Ѵ�.
	if(pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel) == 0)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	if(bProcessInterval)
	{
		// ���� ���¿��� �ҷȴ�.
		//
		if(!pcsTarget)
			return FALSE;

		// �׳� Ÿ���� ���� ��ǥ�� �������ٰ� �Žñ� �Ѵ�.
		AuPOS stPos = ((AgpdCharacter*)pcsTarget)->m_stPos;
		m_pagsmSkill->CastSkill(pcsTarget,
					pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel),
					lSkillLevel,
					&stPos);


		//ApdEvent* pcsEvent = NULL;
		//pcsEvent = m_papmEventManager->GetEvent(pcsTarget, APDEVENT_FUNCTION_SKILL);
		//if(!pcsEvent)
		//	pcsEvent = m_papmEventManager->AddEvent(m_papmEventManager->GetEventData(pcsTarget), APDEVENT_FUNCTION_SKILL, pcsTarget, FALSE, 0, GetClockCount());

		//if(!pcsEvent)
		//	return FALSE;

		//ApSafeArray<INT32, 33> lCIDBuffer;
		//lCIDBuffer.MemSetAll();

		//// ���� range conditon�� �˻��� Ÿ���� �����´�.
		//INT32 lNumTarget = RangeCheckSphereForATField(pcsSkill, pcsTarget, &lCIDBuffer[0], 32, pcsEvent);

		//// ��� ATFieldAttack ���� �ٸ��� ���⼭ TargetFiltering �ѹ� ����� �Ѵ�.
		//// pcsSkill->m_pcsBase �� pcsTarget ���� ��� �ٲ��ش�.
		//ApBase* pcsOriginalSkillBase = pcsSkill->m_pcsBase;
		//pcsSkill->m_pcsBase = pcsTarget;

		//BOOL bIsTargetSelfCharacter	= FALSE;

		//ApSafeArray<AgpdCharacter*, 33>	pacsCharacter;
		//pacsCharacter.MemSetAll();

		//if(lNumTarget > 32)
		//	lNumTarget	= 32;

		//if(lNumTarget > 0)
		//{
		//	for(INT32 i = 0; i < lNumTarget; i++)
		//	{
		//		if(lCIDBuffer[i] == (INT32)pcsSkill->m_pcsBase)
		//		{
		//			bIsTargetSelfCharacter	= TRUE;
		//			break;
		//		}
		//	}

		//	if(!bIsTargetSelfCharacter)
		//	{
		//		lCIDBuffer[lNumTarget++] = (INT32)pcsSkill->m_pcsBase;
		//	}

		//	if(!(lNumTarget == 1 && bIsTargetSelfCharacter))
		//	{
		//		for(INT32 j = 0; j < lNumTarget; j++)
		//			pacsCharacter[j] = (AgpdCharacter*)lCIDBuffer[j];
		//	}
		//	else
		//	{
		//		pacsCharacter[0] = (AgpdCharacter*)pcsSkill->m_pcsBase;
		//	}
		//}

		//ApSafeArray<AgpdCharacter*, 33> pacsRemoveTarget;
		//pacsRemoveTarget.MemSetAll();

		//lNumTarget = TargetFiltering(pcsSkill, &pacsCharacter[0], lNumTarget, &pacsRemoveTarget[0]);

		//lCIDBuffer.MemSetAll();
		//for(INT32 i = 0; i < lNumTarget; ++i)
		//	lCIDBuffer[i] = pacsCharacter[i]->m_lID;

		//pcsSkill->m_pcsBase = pcsOriginalSkillBase;

		//// Ÿ���� ������ �� ������.
		//if(lNumTarget < 0)
		//	return TRUE;

		//AgpdCharacter* pcsTargetCharacter = NULL;

		//// ������ ���鼭 ���� ��ų�� �ɾ��ش�.
		//// �̹� ������ �ɷ������� �߰��� �����ϱ� ������ �� �ɸ� �ȴ�.
		//for(INT32 i = 0; i < lNumTarget; i++)
		//{
		//	pcsTargetCharacter = m_pagpmCharacter->GetCharacterLock(lCIDBuffer[i]);
		//	if(!pcsTargetCharacter)
		//		continue;

		//	if(pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		//	{
		//		pcsTargetCharacter->m_Mutex.Release();
		//		continue;
		//	}

		//	m_pagsmSkill->CastSkill(static_cast<ApBase*>(pcsTarget),
		//		pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_ADDITIONAL_SKILLTID][lSkillLevel],
		//		lSkillLevel,
		//		static_cast<ApBase*>(pcsTargetCharacter));

		//	pcsTargetCharacter->m_Mutex.Release();
		//}

		//m_papmEventManager->RemoveEvent(pcsEvent);
	}
	else
	{
		// �״��� �� �� ����. -0-;;
	}

	return TRUE;
}

// 2005.07.15. steeple
// ��ȯ Ÿ��1. ������ ��ȯ�Ѵ�. �����̴� �ַ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// ���� ������ üũ�ؼ� �ƽ� ��ȯ ��ġ�� �ٴ޾Ҵٸ� 
	// ���� �� �Ÿ� �����ְ� ���� ��ȯ�ϴ��� ��ȯ�� �ȵǰ� ó������.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	
	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
													AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// ���� �޽����� ������� �ϴµ�
		return FALSE;
	}

	// ��ȯ�� �� CBEnterGameWorld �� Ÿ�� Passive ��ų���� �� �� Cast �ȴ�.
	// �׷� ���� �� �� ����???? -_-

	return TRUE;
}

// 2005.10.05. steeple
// Fixed ����� �߰��� ���ؼ� ������ ��ȯ Ÿ��1 �� ���̰� ���� ���� ���� ����.
//
// 2005.07.15. steeple
// ��ȯ Ÿ��2. �����ٰ� �ɴ´�. ex) ����Ʈ�� Ʈ��, Ȧ�� �׶���
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons || !m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lFixedTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lFixedCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lFixedCount == 0)
		lFixedCount = 1;

	// �������� �� Ÿ�Ժ��� Max ġ�� ��ų�� ����Ǿ� �ִ�.
	INT32 lMaxEachCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];

	// Owner �� Fixed List ���� �ش� TID �� ���� üũ�� �Ѵ�.
	INT32 lCurrentEachCount = m_pagpmSummons->GetEachFixedCount(static_cast<AgpdCharacter*>(pcsTarget), lFixedTID);
	if(lCurrentEachCount >= lMaxEachCount)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	INT32 lCreateNum = m_pagsmSummons->ProcessFixed(static_cast<AgpdCharacter*>(pcsTarget), lFixedTID,
											ulDuration, lFixedCount, lSkillLevel, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// ���� �޽����� ������� �ϴµ�
		return FALSE;
	}

	return TRUE;
}

// 2005.07.15. steeple
// ��ȯ Ÿ��3. Ÿ���� ���� ������ �����. �̶� Ÿ���� only ����.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType3(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	if(!m_pagpmCharacter->IsMonster(static_cast<AgpdCharacter*>(pcsTarget)))
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	// 2005.10.05. steeple
	// Taming �� �� ��ų���� Max ������ ��������. 
	// ����� ��ų�� �ϳ����� �̷��� ó���ص� �ȴ�����......
	// ��ų�� �������� ����� �� Taming �� �ֵ��� SkillTID �� �ְ�, �װſ� ���� ���� �񱳸� �ؾ��� �� �ʹ�.
	//
    
	INT32 lMaxTameCountSkill = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
	if(m_pagpmSummons->GetTameCount(static_cast<AgpdCharacter*>(pcsTarget)) >= lMaxTameCountSkill)
		return FALSE;

	INT32 lTamingNum = m_pagsmSummons->ProcessTaming(static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase),
													static_cast<AgpdCharacter*>(pcsTarget),
													ulDuration,
													lSkillLevel,
													pcsSkill->m_bForceAttack);
	if(lTamingNum < 1)
	{
		// �����޽����� �뷫 �����ְ�
		AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
		if(pcsAgsdSkill)
			pcsAgsdSkill->m_eMissedReason = AGSDSKILL_MISSED_REASON_TAME_FAILURE_ABNORMAL;

		return FALSE;
	}

	return TRUE;
}

// 2005.08.31. steeple
// ��ȯ Ÿ��4. �ƽ� ��ȯ�� �ø���.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType4(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons || !m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// �ٷ� �߰����ش�.
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIncreaseSummonsNum = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lIncreaseSummonsNum);

	// Max Count ��Ŷ �����ش�.
	m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));

	return TRUE;
}

// 2005.09.18. steeple
// ��ȯ Ÿ��5. ��ȯ������ ������ �ɾ��ִµ�, �̹� ��ȯ�Ǿ� �ִ� �����̸� �µ鿡�� �ɾ��ְ�,
// ���� ��ȯ�Ǵ� �ֵ鿡�Ե� �ڵ����� �ɾ���� �Ѵ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �� ��ų�� PC �����̴�!
	if(!m_pagpmCharacter->IsPC((AgpdCharacter*)pcsTarget))
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// ADDITIONAL_SKILL_TID �� ������ GG
	//INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lSkillLevel = m_pagpmSkill->GetSkillLevel(pcsSkill);
	if(pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel) == 0.0f)
		return FALSE;

	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_SUMMONS;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_SUMMONS] |= AGPMSKILL_EFFECT_DETAIL_SUMMONS_TYPE5;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lSummonsType5Count++;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lSummonsType5Count = 1;

	// ���� ��ȯ�� ��ȯ���� �ִٸ� ���鼭 �߰� ȿ���� Cast ���ش�.
	if(m_pagpmSummons->GetSummonsCount((AgpdCharacter*)pcsTarget) < 1)
		return TRUE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pagpmSummons->GetADCharacter((AgpdCharacter*)pcsTarget);
	if(!pcsSummonsADChar)
		return TRUE;

	AgpdCharacter* pcsSummons = NULL;

	// ����.
	
	if(!pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	AgpdSummonsArray::iterator iter = tmpVector.begin();
	while(iter != tmpVector.end())
	{
		// ��ȯ���� ����.
		if(iter->m_eType != AGPMSUMMONS_TYPE_ELEMENTAL)
		{
			++iter;
			continue;
		}

		pcsSummons = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsSummons)
		{
			++iter;
			continue;
		}

		m_pagsmSkill->CastSkill(	pcsTarget,
					pcsSkillTemplate->GetAdditionalSkillTID(lSkillLevel),
					lSkillLevel,
					pcsSummons);

		pcsSummons->m_Mutex.Release();

		++iter;
	}

	return TRUE;
}

// 2006.10.31. steeple
// bForce �� �߰��ϸ鼭 �̷��� ���� �� �ʿ䰡 ��������.
// �׳� Type1 ȣ���Ѵ�.
//
// 2006.08.22. steeple
// SiegeWar �� Summons. �ӽ÷� MaxSummonsCount �� �ϳ� �÷���� �Ѵ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	return ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval);
	//if(!m_pagsmSummons)
	//	return FALSE;

	//if(!pcsSkill || !pcsTarget)
	//	return FALSE;

	//if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
	//	return FALSE;

	//// �ӽ÷� �ϳ� �÷��ش�. ������ ���� �� �ϳ� �ٿ��� �Ѵ�.
	//m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);
	//m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));

	//if(ProcessSkillEffect2SummonsType1(pcsSkill, pcsTarget, bProcessInterval) == FALSE)
	//{
	//	// �ѹ�
	//	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);
	//	m_pagsmSummons->SendSetMaxSummons((AgpdCharacter*)pcsTarget, m_pagpmSummons->GetMaxSummonsCount((AgpdCharacter*)pcsTarget));
	//	return FALSE;
	//}

	//return TRUE;
}

// 2006.09.28. steeple
// ĳ���͸� �������� �ϴ� ���͸� �����Ѵ�. �Ϲ����� ������ ����Ѵ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType7(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		return FALSE;	// ��� ���� ���� �ȵǾ� ������ ������.
		//lSummonsCount = 10;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	if(ulDuration == 0)
		return FALSE;

	if(lSummonsTID > 0)
	{
		// ������ ���ڸ� FALSE �� �־ ��Ŷ�� ������ �ʴ´�.
		INT32 lCount = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID,
										ulDuration, lSummonsCount, lSkillLevel, AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_ATTACK,
										pcsSkill->m_bForceAttack, FALSE);
		if(1 < lCount)
			return FALSE;
	}
	else
	{
		// Caster �� ���� +-5 ������ ���͸� ���� Random �ϰ� �����.
		INT32 lCasterLevel = m_pagpmCharacter->GetLevel((AgpdCharacter*)pcsSkill->m_pcsBase);

		// ĳ������ ������ �´� ���� TID �� ���Ϳ� �����Ѵ�
		static MapMob MapTemplate;
		if(MapTemplate.size() == 0)
		{
			INT32 lIndex = 0;
			for(AgpdCharacterTemplate* pcsTemplate = m_pagpmCharacter->GetTemplateSequence(&lIndex);
				pcsTemplate;
				pcsTemplate = m_pagpmCharacter->GetTemplateSequence(&lIndex))
			{
				// ���Ͱ� �ƴϸ� ��������.
				if(!(pcsTemplate->m_ulCharType & AGPMCHAR_TYPE_MONSTER))
					continue;

				// ���ʹ� Race �� 11 �� ���õǾ� �ִ�. (CharacterDataTable ����)
				if(m_pagpmCharacter->GetRaceFromTemplate(pcsTemplate) != 11)
					continue;

				// Taming ���� �� �ֵ鸸 ��ȯ�Ѵ�.
				if(pcsTemplate->m_eTamableType == AGPDCHAR_TAMABLE_TYPE_NONE)
					continue;

				INT32 lMonsterLevel = m_pagpmCharacter->GetLevelFromTemplate(pcsTemplate);

				MapTemplate.insert(std::make_pair(lMonsterLevel, pcsTemplate->m_lID));
			}
		}

		if(MapTemplate.size() == 0)
			return FALSE;

		// 2007.09.12. steeple
		// ���� ���̰� ���� ���� �ص� ���������� ���� �� �ְ� �Ѵ�.
		INT32 lMinLevel, lMaxLevel, lMaxMobLevel;
		lMinLevel = lMaxLevel = lMaxMobLevel = 0;

		MapRIterMob riter = MapTemplate.rbegin();
		if(riter != MapTemplate.rend())
			lMaxMobLevel = riter->first;

		if(lMaxMobLevel < lCasterLevel + 5)
			lMaxLevel = lMaxMobLevel;
		else
			lMaxLevel = lCasterLevel + 5;

		lMinLevel = lMaxLevel - 10;

		MapIterMob iterStart	= MapTemplate.lower_bound(lMinLevel);
		MapIterMob iterEnd		= MapTemplate.upper_bound(lMaxLevel);

		if(iterStart == MapTemplate.end() || iterEnd == MapTemplate.begin())
			return FALSE;

		std::vector<INT32> vcTemplate;
		for(iterStart; iterStart != iterEnd; ++iterStart)
		{
			vcTemplate.push_back(iterStart->second);
		}
		
		if(vcTemplate.size() == 0)
			return FALSE;

		for(INT32 i = 0; i < lSummonsCount; ++i)
		{
			INT32 lRand = m_csRandom->randInt((int)vcTemplate.size());
			if(lRand >= (int)vcTemplate.size())
				lRand = (int)vcTemplate.size() - 1;
			lSummonsTID = vcTemplate[lRand];

			// ����� �������̴�. ������.
			// ������ ���ڸ� FALSE �� �־ ��Ŷ�� ������ �ʴ´�.
			// ���⼭�� ����ó�� ���� �ʴ´�.
			m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, 1, lSkillLevel,
											AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_ATTACK, pcsSkill->m_bForceAttack, FALSE);
		}
	}

	return TRUE;
}

// 2007.03.20. steeple
// Pet �� ��ȯ�Ѵ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType8(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// ���� ������ üũ�ؼ� �ƽ� ��ȯ ��ġ�� �ٴ޾Ҵٸ� 
	// ���� �� �Ÿ� �����ְ� ���� ��ȯ�ϴ��� ��ȯ�� �ȵǰ� ó������.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);
	
	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
													AGPMSUMMONS_TYPE_PET, AGPMSUMMONS_PROPENSITY_SHADOW, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// ���� �޽����� ������� �ϴµ�
		return FALSE;
	}

	// ��ȯ�� �� CBEnterGameWorld �� Ÿ�� Passive ��ų���� �� �� Cast �ȴ�.
	// �׷� ���� �� �� ����???? -_-

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType9(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// ���� ������ üũ�ؼ� �ƽ� ��ȯ ��ġ�� �ٴ޾Ҵٸ� 
	// ���� �� �Ÿ� �����ְ� ���� ��ȯ�ϴ��� ��ȯ�� �ȵǰ� ó������.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	AuPOS stSummonPosition = m_pagsmSummons->GetFrontofCharacterPos((AgpdCharacter*)pcsTarget);

	// ��ȯ�Ҽ� �ִ� Maxġ�� �ӽ÷� �ϳ� �÷��ش�.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);

	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(stSummonPosition, static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
														AGPMSUMMONS_TYPE_ELEMENTAL, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// ���� �޽����� ������� �ϴµ�
		return FALSE;
	}

	// �÷����Ҵ� Maxġ�� �ٽ� �ϳ� �����ش�.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);

	// ��ȯ�� �� CBEnterGameWorld �� Ÿ�� Passive ��ų���� �� �� Cast �ȴ�.
	// �׷� ���� �� �� ����???? -_-

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2SummonsType10(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmSummons)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// ���� ������ üũ�ؼ� �ƽ� ��ȯ ��ġ�� �ٴ޾Ҵٸ� 
	// ���� �� �Ÿ� �����ְ� ���� ��ȯ�ϴ��� ��ȯ�� �ȵǰ� ó������.

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	INT32 lSummonsTID = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_TID][lSkillLevel];
	INT32 lSummonsCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_SUMMONS_COUNT][lSkillLevel];
	if(lSummonsCount == 0)
		lSummonsCount = 1;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill, lSkillLevel);

	AuPOS stSummonPosition = m_pagsmSummons->GetFrontofCharacterPos((AgpdCharacter*)pcsTarget);

	// ��ȯ�Ҽ� �ִ� Maxġ�� �ӽ÷� �ϳ� �÷��ش�.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, 1);

	INT32 lCreateNum = m_pagsmSummons->ProcessSummons(static_cast<AgpdCharacter*>(pcsTarget), lSummonsTID, ulDuration, lSummonsCount, lSkillLevel,
														AGPMSUMMONS_TYPE_MONSTER, AGPMSUMMONS_PROPENSITY_DEFENSE, pcsSkill->m_bForceAttack);
	if(lCreateNum < 1)
	{
		// ���� �޽����� ������� �ϴµ�
		return FALSE;
	}

	// �÷����Ҵ� Maxġ�� �ٽ� �ϳ� �����ش�.
	m_pagpmSummons->AddMaxSummonsCount((AgpdCharacter*)pcsTarget, -1);

	// ��ȯ�� �� CBEnterGameWorld �� Ÿ�� Passive ��ų���� �� �� Cast �ȴ�.
	// �׷� ���� �� �� ����???? -_-

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Exp Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusExp(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_EXP][lSkillLevel];

	// PC �� ���������� üũ�Ѵ�.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusExpRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusExpRate += lValue;
	
	m_pagpmCharacter->AddGameBonusExp((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2005.11.25. steeple
// Game Bonus Money Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusMoney(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_MONEY][lSkillLevel];

	// PC �� ���������� üũ�Ѵ�.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusMoneyRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusMoneyRate += lValue;

	m_pagpmCharacter->AddGameBonusMoney((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2006.06.01. steeple
// Game Bonus Drop Rate Skill
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusDropRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_DROP_RATE][lSkillLevel];

	// PC �� ���������� üũ�Ѵ�.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropRate += lValue;

	m_pagpmCharacter->AddGameBonusDropRate((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2008.07.10. iluvs
// Game Bonus Drop Rate Skill2
BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusDropRate2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor2[AGPMSKILL_CONST_BONUS_DROP_RATE2][lSkillLevel];

	// PC �� ���������� üũ�Ѵ�.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropRate2 = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropRate2 += lValue;

	m_pagpmCharacter->AddGameBonusDropRate2((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2GameBonusCharismaRate(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32 lValue = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_BONUS_CHARISMA_RATE][lSkillLevel];

	// PC �� ���������� üũ�Ѵ�.
	double lPCBangBonusRate =  m_pagsmSkill->GetPCBangItemSkillBonusRate(pcsSkill);
	if(lPCBangBonusRate != 0)
		lValue = (INT32)((double)lValue * lPCBangBonusRate);

	pcsAgsdSkill->m_stBuffedSkillFactorEffectArg.lBonusDropCharismaRate = lValue;
	pcsAttachData->m_stBuffedSkillFactorEffectArg.lBonusDropCharismaRate += lValue;

	m_pagpmCharacter->AddGameBonusCharismaRate((AgpdCharacter*)pcsTarget, lValue);

	return TRUE;
}

// 2007.07.04. steeple
// Detect Type1. ������ ���������� �����.
BOOL AgsmSkillEffect::ProcessSkillEffect2DetectType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �̰����� ���� �� �� ����.

	return TRUE;
}

// 2007.07.04. steeple
// Detect Type2. �׳� ������  �� ������.
BOOL AgsmSkillEffect::ProcessSkillEffect2DetectType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �׳� Ǯ���ش�.
	m_pagsmSkill->EndTransparentBuffedSkill(pcsTarget);

	// Ȥ�� �𸣴� üũ �ѹ� �� �Ѵ�.
	if(m_pagpmCharacter->IsStatusFullTransparent((AgpdCharacter*)pcsTarget))
		m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);
	if(m_pagpmCharacter->IsStatusHalfTransparent((AgpdCharacter*)pcsTarget))
		m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_HALF_TRANSPARENT);

	return TRUE;
}

// 2006.10.10. steeple
// Game Effect. Change day to night or night to day.
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectDayNight(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagpmTimer || !m_pagsmTimer)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �ð��� �ٲ��ش�. �׳� 12�ð� �÷�������.
	UINT8 ucHour = m_pagpmTimer->GetCurHour();
	UINT8 ucHourConverted;
	ucHourConverted = ( ucHour + 12 ) % 24;

	m_pagpmTimer->SetTime((INT32)ucHourConverted, 0, 0, NULL);
	
	// ��ü �������� �Ѹ���.
	INT16 nPacketLength = 0;
	PVOID pvPacket = m_pagpmTimer->MakePacketTime(&nPacketLength);
	if(!pvPacket || nPacketLength < 1)
	{
		m_pagpmTimer->SetTime((INT32)ucHour, 0, 0, NULL);
		return FALSE;
	}

	m_pagsmSkill->SendPacketAllUser(pvPacket, nPacketLength, PACKET_PRIORITY_6);
	m_pagpmTimer->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

// 2006.11.08. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectRain(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmEventNature)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill);
	m_pagsmEventNature->SetWeatherWithDuration(AGPDNATURE_WEATHER_TYPE_RAINY, ulDuration);

	return TRUE;
}

// 2006.11.08. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2GameEffectSnow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!m_pagsmEventNature)
		return FALSE;

	if(!pcsSkill || !pcsTarget)
		return FALSE;

	UINT32 ulDuration = m_pagpmSkill->GetSkillDurationTime(pcsSkill);
	m_pagsmEventNature->SetWeatherWithDuration(AGPDNATURE_WEATHER_TYPE_SNOWY, ulDuration);

	return TRUE;
}

// 2007.06.26. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2DivideAttr(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrRate = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DIVIDE_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster = pcsSkill->m_pcsBase->m_lID;

	// ��� ������ ����ģ��.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_DIVIDE;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_ATTR;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideAttrRate = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrRate;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideAttrCaster;

	return TRUE;
}

// 2007.06.26. steeple
BOOL AgsmSkillEffect::ProcessSkillEffect2DivideNormal(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalRate = (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_DIVIDE_RATE][lSkillLevel];
	pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster = pcsSkill->m_pcsBase->m_lID;

	// ��� ������ ����ģ��.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_DIVIDE;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_DIVIDE] |= AGPMSKILL_EFFECT_DETAIL_DIVIDE_NORMAL;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideNormalRate = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalRate;
	pcsAttachData->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster = pcsAgsdSkill->m_stBuffedSkillCombatEffectArg.lDivideNormalCaster;

	return TRUE;
}

// 2007.10.29. steeple
// Union Type1 ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2UnionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgsdSkillADBase* pcsAgsdADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsAgsdADBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stUnionInfo.m_lUnionID = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_UNION][lSkillLevel];
	
	UnionControlArray::iterator iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
												pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
												pcsAgsdSkill->m_stUnionInfo.m_lUnionID);
	if(iter != pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
	{
		++iter->m_lCurrentCount;
	}
	else
	{
		iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
						pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
						0);
		if(iter == pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
			return FALSE;	// �̷��� �Ǹ� �� ���ִ� �Ŷ� �迭�� ũ�⸦ �ø����� �ؾ��Ѵ�.

		iter->m_lUnionID = pcsAgsdSkill->m_stUnionInfo.m_lUnionID;
		iter->m_lCurrentCount = 1;
	}

	return TRUE;
}

// 2007.10.29. steeple
// Union Type2 ó��
BOOL AgsmSkillEffect::ProcessSkillEffect2UnionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	AgsdSkillADBase* pcsAgsdADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsAgsdADBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	if(!pcsSkillTemplate)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	pcsAgsdSkill->m_stUnionInfo.m_lUnionID = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_SKILL_UNION_CONTROL][lSkillLevel];
	pcsAgsdSkill->m_stUnionInfo.m_lMaxCount = (INT32)pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_MAX_SKILL_UNION][lSkillLevel];

	UnionControlArray::iterator iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
												pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
												pcsAgsdSkill->m_stUnionInfo.m_lUnionID);
	if(iter != pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
	{
		// ������ Union Max Control ġ�� �ִٸ� �����ְ� ���̴�.
		iter->m_lMaxCount += pcsAgsdSkill->m_stUnionInfo.m_lMaxCount;
	}
	else
	{
		iter = std::find(pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.begin(),
						pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end(),
						0);
		if(iter == pcsAgsdADBase->m_UnionControlArray.m_astUnionControlInfo.end())
			return FALSE;	// �̷��� �Ǹ� �� ���ִ� �Ŷ� �迭�� ũ�⸦ �ø����� �ؾ��Ѵ�.

		iter->m_lUnionID = pcsAgsdSkill->m_stUnionInfo.m_lUnionID;
		iter->m_lMaxCount += pcsAgsdSkill->m_stUnionInfo.m_lMaxCount;
	}

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Stun
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelStun(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �׳� Ǯ���ָ� �ȴ�.
	m_pagpmCharacter->UpdateUnsetSpecialStatus((AgpdCharacter*)pcsTarget, AGPDCHAR_SPECIAL_STATUS_STUN);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Slow
// ������ ��ų���� Slow �Ӽ��� �� 0���� �ٲ��ְ�, RecalcFactor �θ���.
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelSlow(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgsdSkillADBase* pcsADBase = m_pagsmSkill->GetADBase(pcsTarget);
	if(!pcsADBase)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = NULL;
	BOOL bUpdated = FALSE;

	for(INT32 i = 0; i < AGPMSKILL_MAX_SKILL_BUFF; i++)
	{
		if(!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill ||
			!pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill->m_pcsTemplate)
			break;

		pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsADBase->m_csBuffedSkillProcessInfo[i].m_pcsTempSkill);
		if(!pcsAgsdSkill)
			break;

		// Factor �� ã�ƺ���.
		FLOAT fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// �ϴ� ���͸� ����
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorPoint, FALSE, FALSE, FALSE, FALSE);

			// 0 ���� ������ ��
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// �ٽ� ���Ѵ�.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorPoint, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// �ϴ� ���͸� ����
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorPercent, FALSE, FALSE, FALSE, FALSE);

			// 0 ���� ������ ��
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// �ٽ� ���Ѵ�.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorPercent, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue < 0)
		{
			bUpdated = TRUE;

			// �ϴ� ���͸� ����
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorItemPoint, FALSE, FALSE, FALSE, FALSE);

			// 0 ���� ������ ��
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPoint, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// �ٽ� ���Ѵ�.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPoint, pcsAgsdSkill->m_pcsUpdateFactorItemPoint, FALSE, FALSE, TRUE, FALSE);
		}

		fValue = 0.0f;
		m_pagpmFactors->GetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, &fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
		if((INT32)fValue > 0)
		{
			bUpdated = TRUE;

			// �ϴ� ���͸� ����
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorItemPercent, FALSE, FALSE, FALSE, FALSE);

			// 0 ���� ������ ��
			fValue = 0.0f;
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT);
			m_pagpmFactors->SetValue(pcsAgsdSkill->m_pcsUpdateFactorItemPercent, fValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);

			// �ٽ� ���Ѵ�.
			m_pagpmFactors->CalcFactor(&((AgpdCharacter*)pcsTarget)->m_csFactorPercent, pcsAgsdSkill->m_pcsUpdateFactorItemPercent, FALSE, FALSE, TRUE, FALSE);
		}
	}

	if(bUpdated)
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter*)pcsTarget, TRUE);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel All Buff / Debuff except cash, summons buff
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelAllBuff(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	// �׳� Ǯ���ش�. -0-;
	BOOL bInitCoolTime = ((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_stConditionArg[0].lArg1;
	INT32 lMaxCount = (INT32)((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_LIMIT_QUANTITY][lSkillLevel];

	m_pagsmSkill->EndAllBuffedSkillExceptCash_SummonsSkill(pcsTarget, TRUE, bInitCoolTime, lMaxCount);

	return TRUE;
}

// 2007.07.03. steeple
// Dispel Transparent
BOOL AgsmSkillEffect::ProcessSkillEffect2DispelTransparent(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	// �׳� Ǯ���ش�.
	m_pagsmSkill->EndTransparentBuffedSkill(pcsTarget);

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2DispelSpecialStatus(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate *)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData((AgpdCharacter*)pcsTarget);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	// If AIState is none, there is nothing to do dispel specialstatus
	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_NONE)
		return TRUE;

	BOOL bInitState = FALSE;

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_CONFUSION)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_CONFUSION)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_HOLD)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_HOLD)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_FEAR)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_FEAR)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_DISEASE)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_DISEASE)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_BERSERK)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_BERSERK)
		{
			bInitState = TRUE;
		}
	}

	if(pcsSkillTemplate->m_lSpecialStatus & AGPMSKILL_SPECIAL_STATUS_SHRINK)
	{
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI == AGPDAI2_STATE_PC_AI_SHRINK)
		{
			bInitState = TRUE;
		}
	}

	if(bInitState)
		m_pagpmAI2->InitPCAIInfo((AgpdCharacter*)pcsTarget);

	return TRUE;
}

// 2005.07.05. steeple
// � Action �� �Ͼ�鼭 �Ͼ�� �ϴ� Action ó��
// �� �Լ��� ProcessSkillEffect ���� ó���ϴ� ���� �ƴ϶� �ʿ��� ������ �ƹ����Գ� �θ��� �ȴ�.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnAction(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT32 lActionOnActionType, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	BOOL bResult = FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
			bResult |= ProcessSkillEffect2ActionOnActionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2)
			bResult |= ProcessSkillEffect2ActionOnActionType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
			bResult |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
			bResult |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5)
			bResult |= ProcessSkillEffect2ActionOnActionType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6)
			bResult |= ProcessSkillEffect2ActionOnActionType6(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE1)
			bResult |= ProcessSkillEffect2ActionOnActionType1(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE2)
			bResult |= ProcessSkillEffect2ActionOnActionType2(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE3)
			bResult |= ProcessSkillEffect2ActionOnActionType3(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE4)
			bResult |= ProcessSkillEffect2ActionOnActionType4(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE5)
			bResult |= ProcessSkillEffect2ActionOnActionType5(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6 &&
			lActionOnActionType & AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6)
			bResult |= ProcessSkillEffect2ActionOnActionType6(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2005.07.05. steeple
// ������ ������ ������ �Ҹ��� �Ǹ�, �Ҹ��� �������� �ش�.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType1(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// Ȥ�� pcsSkill->m_pcsBase �� NULL �̸� �����ش�.
	BOOL bIsBaseNULL = FALSE;
	if(!pcsSkill->m_pcsBase)
	{
		pcsSkill->m_pcsBase = (ApBase*)m_pagpmCharacter->GetCharacterLock(pcsSkill->m_csBase.m_lID);
		bIsBaseNULL = TRUE;
	}

	// �� ���ߴٸ� ������.
	if(!pcsSkill->m_pcsBase)
		return FALSE;

	// 2005.11.01. steeple
	// �� Ÿ���� ��ų�� ���� ������ ��׷� ���̰�, �������ǰ�, �����ǰ� �׷��� �Ѵ�.
	// Attack ��ų�� �ƴϰ� ����� ��ų�� �ƴϰ� ���õǾ� �ִ� Ÿ��.
	// �׷��Ƿ� ���� ���Ƿ� �ҷ���� �Ѵ�.
	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= (AgpdCharacter*)pcsSkill->m_pcsBase;
	stAttackResult.pTargetChar	= (AgpdCharacter*)pcsTarget;
	m_pagsmCombat->EnumCallback(AGSMCOMBAT_CB_ID_ATTACK_START, &stAttackResult, NULL);

	// Melee Attack 1 ������ �������� ����ϴ�. ���� Const �� ���� �Ǿ� �־�� �Ѵ�.
	ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, 0, bProcessInterval);

	// Additional Effect �� ���� ������
	m_pagsmSkill->SendAdditionalEffect(pcsTarget, AGPMSKILL_ADDITIONAL_EFFECT_END_BUFF_EXPLOSION);

	// factor�� ���� ������Ʈ ��Ų��.
	m_pagsmSkill->UpdateSkillFactor(pcsSkill, pcsTarget, TRUE, bProcessInterval);

	// 2005.01.28. steeple. Attack ���� ���� ���� üũ�Ѵ�.
	m_pagsmSkill->ApplySkillAttack(pcsSkill, pcsTarget);

	// pcsBase �� NULL �̾ ���� ���� �Ŷ��
	if(bIsBaseNULL)
	{
		pcsSkill->m_pcsBase->m_Mutex.Release();
		pcsSkill->m_pcsBase = NULL;
	}

	return TRUE;
}

// 2005.07.05. steeple
// ĳ��Ʈ�� ������ ������ �Ҹ��� �Ǹ�, ��ų �����ڸ� Target ���� ���� ��Ű�� ���ݵ���.
//
// 2005.07.05. steeple
// Skill_Spec �������� �������� �����ϰ� Melee Attack �� �����ϰ�
// Action On Action �� 2�� �ָ� ���⼭�� �ڷ���Ʈ�� �����ָ� �� �� ����. ���߿� ������ �������� üũ����.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType2(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsBase)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	if(pcsTarget->m_eType != APBASE_TYPE_CHARACTER)
		return FALSE;

	// ���� ������Ʈ���� ���Ұ�. 2007.05.07. steeple
	AgpdSiegeWarMonsterType eType = m_pagpmSiegeWar->GetSiegeWarMonsterType((AgpdCharacter*)pcsTarget);
	if(eType >= AGPD_SIEGE_MONSTER_GUARD_TOWER && eType <= AGPD_SIEGE_MONSTER_CATAPULT)
		return FALSE;

	// ���� �����ڸ� ���� ��Ų��.
	m_pagpmCharacter->StopCharacter((AgpdCharacter*)pcsSkill->m_pcsBase, NULL);

	AuPOS stTargetPos = ((AgpdCharacter*)pcsTarget)->m_stPos;

	if(m_pagpmCharacter->UpdatePosition((AgpdCharacter*)pcsSkill->m_pcsBase, &stTargetPos, TRUE, TRUE))
	{
		// �ڷ���Ʈ �ߴٴ� ����Ʈ�� �����ش�.
		m_pagsmSkill->SendAdditionalEffect(pcsSkill->m_pcsBase, AGPMSKILL_ADDITIONAL_EFFECT_TELEPORT);

		//// Melee Attack 1 ������ �������� ����ϴ�. ���� Const �� ���� �Ǿ� �־�� �Ѵ�.
		//ProcessSkillEffectMeleeAttackType1(pcsSkill, pcsTarget, 0, bProcessInterval);

		//// factor�� ���� ������Ʈ ��Ų��.
		//UpdateSkillFactor(pcsSkill, pcsTarget, TRUE, bProcessInterval);

		//// 2005.01.28. steeple. Attack ���� ���� ���� üũ�Ѵ�.
		//ApplySkillAttack(pcsSkill, pcsTarget);
	}

	return TRUE;
}

// 2006.08.31. steeple
// Action on Action Type5. ex) ������ ������ž. (������ �ɸ� ������ �������� ���¸� ��� üũ�Ѵ�.)
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType5(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	// �� �ȿ��� �� �� ���� ����. Pass
	return TRUE;
}

// 2007.07.10. steeple
// Action on Action Type6. �ױ� ������ Additional Skill TID �� �������ش�.
BOOL AgsmSkillEffect::ProcessSkillEffect2ActionOnActionType6(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillAttachData* pcsAttachData = m_pagpmSkill->GetAttachSkillData(pcsTarget);
	if(!pcsAttachData)
		return FALSE;

	AgsdSkill* pcsAgsdSkill = m_pagsmSkill->GetADSkill(pcsSkill);
	if(!pcsAgsdSkill)
		return FALSE;

	// �׳� �ϴ� ���ø� �س��´�.
	pcsAttachData->m_ullBuffedSkillCombatEffect2 |= AGPMSKILL_EFFECT2_ACTOIN_ON_ACTION;
	pcsAttachData->m_ulBuffedSkillEffectDetail[AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION] |= AGPMSKILL_EFFECT_DETAIL_ACTION_ON_ACTION_TYPE6;

	return TRUE;
}

//		ProcessSkillEffectMoveTarget
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MOVE_TARGET) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			target�� �����̵�
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMoveTarget(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsSkill->m_pcsBase || !pcsSkill->m_bCloneObject || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	INT32	lMoveDistance = (INT32) ((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

	// AGSMSKILL_CONST_MOVE_DISTANCE ��ŭ �ű��. ��������..
	////////////////////////////////////////////////////////////////////////////
	return m_pagpmCharacter->MoveCharacterHorizontal((AgpdCharacter *) pcsTarget, m_papmEventManager->GetBasePos(pcsSkill->m_pcsBase, NULL), lMoveDistance);
}

//		ProcessSkillEffectMagicAttackType1
//	Functions
//		- pcsSkill �� skill effect type (AGSMSKILL_EFFECT_MAGIC_ATTACK_DMG_TYPE1) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectMagicAttackType1(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorPermanently
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�. ���������� ���ϴ� ���̴�. result factor�� �ƴ϶� character factor�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorPermanently(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// target�� ������ ���Ѵ�.
	INT32	lTargetLevel = 0;

	switch (pcsTarget->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			lTargetLevel	= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsTarget);
		}
		break;

	default:
		break;
	}

	if (lTargetLevel <= 0)
		return FALSE;

	BOOL	bIsUpdated = FALSE;

	// factor�� �÷��ش�.
	for (int i = AGPMSKILL_CONST_POINT_START; i < AGPMSKILL_CONST_PERCENT_END; ++i)
	{
		if (pcsSkillTemplate->m_fUsedConstFactor[i][lTargetLevel] != 0 || pcsSkillTemplate->m_fUsedConstFactor2[i][lTargetLevel] != 0)
			if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) i))
				bIsUpdated = TRUE;
	}

	if (bIsUpdated)
	{
		// ���̾� factor�� �ٲ������ result factor�� �ٽ� ����� ��� �Ѵ�.
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter *) pcsTarget);
	}

	return TRUE;
}

//		ProcessSkillEffectUpdateFactorPermanentlyRandomStatus
//	Functions
//		- pcsSkill �� skill effect type (AGPMSKILL_EFFECT_UPDATE_FACTOR_PERMANENTLY_RANDOM_STATUS) �� ���� �Լ��� ȣ���� ���� ��ų effect�� �����Ѵ�.
//			���õ� ������ factor point�� ���Ѵ�. ���������� ���ϴ� ���̴�. result factor�� �ƴ϶� character factor�� ���Ѵ�.
//	Arguments
//		- pcsSkill	: ó���� AgpdSkill data pointer
//		- pcsTarget : target base pointer
//		- nIndex	:
//	Return value
//		- BOOL : check result
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmSkillEffect::ProcessSkillEffectUpdateFactorPermanentlyRandomStatus(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if (!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate	*pcsSkillTemplate =  (AgpdSkillTemplate *) pcsSkill->m_pcsTemplate;
	if (!pcsSkillTemplate)
		return FALSE;

	// target�� ������ ���Ѵ�.
	INT32	lTargetLevel = 0;

	switch (pcsTarget->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			lTargetLevel	= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsTarget);
		}
		break;

	default:
		break;
	}

	if (lTargetLevel <= 0)
		return FALSE;

	// factor status�� �ϳ��� ����.
	INT32	lRandomStatus = m_csRandom->randInt(5);

	BOOL	bIsUpdated	= FALSE;

	// ���� UsedConstFactor �� ���õǾ��ִ��� ����.
	if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_POINT_CON + lRandomStatus][lTargetLevel] != 0 ||
		pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_POINT_CON + lRandomStatus][lTargetLevel] != 0 )
	{
		if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) (AGPMSKILL_CONST_POINT_CON + lRandomStatus)))
			bIsUpdated = TRUE;
	}
	else if (pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_PERCENT_CON + lRandomStatus][lTargetLevel] != 0 ||
				pcsSkillTemplate->m_fUsedConstFactor2[AGPMSKILL_CONST_PERCENT_CON + lRandomStatus][lTargetLevel] != 0)
	{
		if (m_pagsmSkill->UpdateFactorPermanently(pcsSkill, pcsTarget, (eAgpmSkillUsedConstFactor) (AGPMSKILL_CONST_PERCENT_CON + lRandomStatus)))
			bIsUpdated = TRUE;
	}

	if (bIsUpdated)
	{
		// ���̾� factor�� �ٲ������ result factor�� �ٽ� ����� ��� �Ѵ�.
		m_pagsmCharacter->ReCalcCharacterResultFactors((AgpdCharacter *) pcsTarget);
	}

	return TRUE;
}


// 2007.12.05. steeple
// Move Position
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePos(AgpdSkill* pcsSkill, ApBase* pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(!pcsSkillTemplate)
		return FALSE;

	// 2012. 01. 10. silvermoo
	// ���� �������� �����̶�� �̵���Ű�� ����
	BOOL bIsInSiegeWarIngArea = FALSE;
	PVOID pvBuffer[2];
	pvBuffer[0] = &bIsInSiegeWarIngArea;
	pvBuffer[1] = NULL;
	m_pagpmPvP->EnumCallback(AGPMPVP_CB_IS_IN_SIEGEWAR_ING_AREA, pcsTarget, pvBuffer);
	if (bIsInSiegeWarIngArea)
		return TRUE;

	BOOL bResult = FALSE;
	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET)
			bResult |= ProcessSkillEffect2MovePosTarget(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF)
			bResult |= ProcessSkillEffect2MovePosSelf(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME)
			bResult |= ProcessSkillEffect2MovePosTargetToMe(pcsSkill, pcsTarget, bProcessInterval);
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET)
			bResult |= ProcessSkillEffect2MovePosTarget(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_SELF)
			bResult |= ProcessSkillEffect2MovePosSelf(pcsSkill, pcsTarget, bProcessInterval);

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_MOVE_POS] & AGPMSKILL_EFFECT_DETAIL_MOVE_POS_TARGET_TO_ME)
			bResult |= ProcessSkillEffect2MovePosTargetToMe(pcsSkill, pcsTarget, bProcessInterval);
	}

	return bResult;
}

// 2007.12.05. steeple
// Move Position Target
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosTarget(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	BOOL bResult = FALSE;
	if(pcsSkill->m_pcsBase)
	{
		AuPOS stFrom = ((AgpdCharacter*)pcsTarget)->m_stPos;
		AuPOS stDest, stDirection;

		AuPOS stDelta = ((AgpdCharacter*)pcsTarget)->m_stPos - ((AgpdCharacter*)pcsSkill->m_pcsBase)->m_stPos;
		FLOAT fDistance = pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

		// ���� ���� ��ֶ�����
		AuMath::V3DNormalize(&stDirection, &stDelta);

		stDest.x = stFrom.x + stDirection.x * fDistance;
		stDest.y = stFrom.y + stDirection.y * fDistance;
		stDest.z = stFrom.z + stDirection.z * fDistance;

		AuPOS stValidDest = stDest;
		ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

		m_pagpmCharacter->GetValidDestination(&stFrom, &stDest, &stValidDest, eType);

		bResult = m_pagpmCharacter->UpdatePosition((AgpdCharacter*)pcsTarget, &stValidDest);
	}
	else
	{
		bResult = m_pagpmCharacter->MoveCharacterHorizontal((AgpdCharacter*)pcsTarget, &((AgpdCharacter*)pcsTarget)->m_stPos, (INT32)pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel]);
	}

	return bResult;
}

// 2007.12.05. steeple
// Move Position Self
BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosSelf(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(!pcsSkill || !pcsSkill->m_pcsTemplate || !pcsSkill->m_pcsBase || !pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase);
	AgpdCharacter* pcsTargetCharacter = static_cast<AgpdCharacter*>(pcsTarget);

	AuPOS stFrom = pcsCharacter->m_stPos;
	AuPOS stDest, stDirection;

	AuPOS stDelta = pcsTargetCharacter->m_stPos - pcsCharacter->m_stPos;
	FLOAT fDistance = pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_MOVE_DISTANCE][lSkillLevel];

	// ���� ���� ��ֶ�����
	AuMath::V3DNormalize(&stDirection, &stDelta);

	stDest.x = stFrom.x + stDirection.x * fDistance;
	stDest.y = stFrom.y + stDirection.y * fDistance;
	stDest.z = stFrom.z + stDirection.z * fDistance;

	AuPOS stValidDest = stDest;
	ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

	m_pagpmCharacter->GetValidDestination(&stFrom, &stDest, &stValidDest, eType);

	// �ϴ� UpdatePosition ���� ó��������, ī�޶� ��ũ�� ������ Express Move ���� ó���� �ʿ��ϴ�.
	BOOL bResult = m_pagpmCharacter->UpdatePosition(pcsCharacter, &stValidDest);
	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2MovePosTargetToMe(AgpdSkill* pcsSkill, ApBase* pcsTarget, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsSkill->m_pcsTemplate || NULL == pcsSkill->m_pcsBase || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate* pcsSkillTemplate = static_cast<AgpdSkillTemplate*>(pcsSkill->m_pcsTemplate);
	INT32 lSkillLevel = m_pagsmSkill->GetModifiedSkillLevel(pcsSkill);
	if(!lSkillLevel)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pcsSkill->m_pcsBase);
	AgpdCharacter* pcsTargetCharacter = static_cast<AgpdCharacter*>(pcsTarget);

	AuAutoLock Lock(pcsTargetCharacter->m_Mutex);

	AuPOS stCastCharacterPos = pcsCharacter->m_stPos;
	AuPOS stDest, stDirection;

	AuPOS stDelta = pcsTargetCharacter->m_stPos - pcsCharacter->m_stPos;
	
	// ���� ���� ��ֶ�����
	AuMath::V3DNormalize(&stDirection, &stDelta);

	FLOAT fDistance = 10.0f;

	stDest.x = stCastCharacterPos.x + stDirection.x * fDistance;
	stDest.y = stCastCharacterPos.y + stDirection.y * fDistance;
	stDest.z = stCastCharacterPos.z + stDirection.z * fDistance;

	AuPOS stValidDest = stDest;
	ApmMap::BLOCKINGTYPE eType = m_pagpmCharacter->GetBlockingType((AgpdCharacter*)pcsTarget);

	m_pagpmCharacter->GetValidDestination(&stCastCharacterPos, &stDest, &stValidDest, eType);

	// �ϴ� UpdatePosition ���� ó��������, ī�޶� ��ũ�� ������ Express Move ���� ó���� �ʿ��ϴ�.
	BOOL bResult = m_pagpmCharacter->UpdatePosition(pcsTargetCharacter, &stValidDest, FALSE, TRUE);

	// ��ȯ���� �Ű��ش�.
	if(m_pagpmSummons)
		m_pagpmSummons->UpdateAllSummonsPosToOwner(pcsTargetCharacter);
	
	return bResult;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2DisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget || NULL == pcsSkill->m_pcsBase || NULL == pcsSkill->m_pcsTemplate)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	// �������� ������ ��ųȿ���� ������� �ʴ� ���¶�� ó������ �ʴ´�
	if (m_pagsmSkill->IsSetDamageOnlyTemporary(pcsSkill))
		return FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_SKILL;
		}	
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			((AgpdCharacter*)pcsTarget)->m_unDisturbAction |= AGPDCHAR_DISTURB_ACTION_SKILL;
		}
	}

	return TRUE;
}

BOOL AgsmSkillEffect::ProcessSkillEffect2ToleranceDisturbCharacterAction(AgpdSkill *pcsSkill, ApBase *pcsTarget, INT16 nIndex, BOOL bProcessInterval)
{
	if(NULL == pcsSkill || NULL == pcsTarget)
		return FALSE;

	AgpdSkillTemplate *pcsSkillTemplate = (AgpdSkillTemplate*)pcsSkill->m_pcsTemplate;
	if(NULL == pcsSkillTemplate)
		return FALSE;

	if(bProcessInterval)
	{
		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_ATTACK)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_MOVE)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_USE_ITEM)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nProcessIntervalEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_SKILL)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_SKILL;
		}	
	}
	else
	{
		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_ATTACK)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_ATTACK)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_ATTACK;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_MOVE)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_MOVE)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_MOVE;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_USE_ITEM)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_USE_ITEM)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_USE_ITEM;
		}

		if(pcsSkillTemplate->m_nEffectDetailType[AGPMSKILL_EFFECT_DETAIL_TOLERANCE_DISTURB_CHARACTER_ACTION] & AGPMSKILL_EFFECT_DETAIL_DISTURB_CHARACTER_ACTION_SKILL)
		{
			if(((AgpdCharacter*)pcsTarget)->m_unDisturbAction & AGPDCHAR_DISTURB_ACTION_SKILL)
				((AgpdCharacter*)pcsTarget)->m_unDisturbAction -= AGPDCHAR_DISTURB_ACTION_SKILL;
		}
	}

	return TRUE;
}