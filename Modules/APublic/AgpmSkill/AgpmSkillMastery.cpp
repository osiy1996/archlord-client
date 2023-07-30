
#include <AgpmSkill/AgpmSkill.h>
#include <AgpmCharacter/AgpmCharacter.h>

INT32	g_alReceivedSkillPoint[2][3] =
{
	{	40,			100,		0	},
	{	30,			60,			100	}
};

extern INT32	g_lSpecializeLevelPoint[AGPMSKILL_MAX_SPECIALIZE_LEVEL_POINT];

const INT32		AGPMSKILL_BONUS_POINT_START_LEVEL		= 50;
const INT32		AGPMSKILL_BONUS_POINT_END_LEVEL			= 70;

const INT32		AGPMSKILL_BASE_HEROIC_POINT				= 1;
const INT32		AGPMSKILL_DOUBLE_HEROIC_POINT_START_LEVEL = 91;
const INT32		AGPMSKILL_DOUBLE_HEROIC_POINT_END_LEVEL = 110;
const INT32		AGPMSKILL_TRIPLE_HEROIC_POINT_START_LEVEL = 111;
const INT32		AGPMSKILL_TRIPLE_HEROIC_POINT_END_LEVEL = 120;

//		InitMastery
//	Functions
//		- pcsBase�� ��� �����͸��� �ʱ�ȭ�Ѵ�. (�������ΰ�� ȣ������ �ʴ´�)
//			1. ActiveSkill�� �ִ°�� �����Ѵ�.
//			2. SP�� ��� pcsBase�� SP�� �ٽ� �ǵ�����.
//	Arguments
//		- pcsBase : 
//	Return value
//		- BOOL : 
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmSkill::InitMastery(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//
//	// �����ϰ� �ִ� ��� ��ų�� �����Ѵ�.
//	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
//	{
//		if (pcsAttachData->m_alSkillID[i] == AP_INVALID_SKILLID)
//			break;
//
//		RemoveSkill(pcsAttachData->m_alSkillID[i], TRUE);
//	}
//
//	for (i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		if (pcsAttachData->m_csMastery[i].m_ucTotalInputSP > 0)
//		{
//			if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
//				m_pagpmCharacter->UpdateSkillPoint((AgpdCharacter *) pcsBase, pcsAttachData->m_csMastery[i].m_ucTotalInputSP);
//		}
//	}
//
//	// ���� ����Ÿ�� �ʱ�ȭ�Ѵ�.
//	ZeroMemory(pcsAttachData->m_alSkillID, sizeof(INT32) * AGPMSKILL_MAX_SKILL_OWN);
//	ZeroMemory(pcsAttachData->m_alUseSkillID, sizeof(INT32) * AGPMSKILL_MAX_SKILL_USE);
//
//	pcsAttachData->m_lModifyCharLevel	= 0;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate = GetMasteryTemplate(pcsBase);
//	if (pcsMasteryTemplate)
//		CopyMemory(&pcsAttachData->m_csMasteryTemplate, pcsMasteryTemplate, sizeof(AgpdSkillMasteryTemplate));
//
//	ZeroMemory(pcsAttachData->m_csMastery, sizeof(AgpdSkillMastery) * AGPMSKILL_MAX_MASTERY);
//
//	for (i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		pcsAttachData->m_csMastery[i].m_ucMasteryIndex	= (UINT8) i;
//	}
//
//	return TRUE;
//}

//		GetMastery
//	Functions
//		- pcsBase�� Attach�� skill mastery�� �����´�.
//	Arguments
//		- pcsBase : 
//	Return value
//		- AgpdSkillMastery * : 
///////////////////////////////////////////////////////////////////////////////
//AgpdSkillMastery* AgpmSkill::GetMastery(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return NULL;
//
//	// �����͸��� �����´�.
//	AgpdSkillAttachData	*pcsAttachData = GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	return pcsAttachData->m_csMastery;
//}

//AgpdSkillMasteryTemplate* AgpmSkill::GetMasteryTemplate(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return NULL;
//
//	// ����� �����͸��� ĳ���� Ŭ������ ���� �ٸ��� ������ ĳ���Ͱ� �ƴϸ� ���ø��� ���� �� ����.
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return NULL;
//
//	INT32	lCharRace	= AURACE_TYPE_NONE;
//	if (!m_pagpmFactors->GetValue(&((AgpdCharacter *) pcsBase)->m_csFactor, &lCharRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE))
//		return NULL;
//
//	if (lCharRace <= AURACE_TYPE_NONE || lCharRace >= AURACE_TYPE_MAX)
//		return NULL;
//
//	INT32	lCharClass	= AUCHARCLASS_TYPE_NONE;
//	if (!m_pagpmFactors->GetValue(&((AgpdCharacter *) pcsBase)->m_csFactor, &lCharClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS))
//		return NULL;
//
//	if (lCharClass <= AUCHARCLASS_TYPE_NONE || lCharClass >= AUCHARCLASS_TYPE_MAX)
//		return NULL;
//
//	return (m_csSkillMasteryTemplate + (lCharRace - 1) * AUCHARCLASS_TYPE_MAX + lCharClass);
//}

//INT32 AgpmSkill::GetMasteryPoint(AgpdSkill *pcsSkill)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase)
//		return 0;
//
//	AgpdSkillMastery *pcsMastery = GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return 0;
//
//	return (INT32) pcsMastery[pcsSkill->m_lMasteryIndex].m_ucTotalInputSP;
//}

BOOL AgpmSkill::AdjustSkillPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT32				lLevel			= m_pagpmCharacter->GetLevel(pcsCharacter);
	INT32				lBeforeLevel	= m_pagpmCharacter->GetLevelBefore(pcsCharacter);

	if (lLevel < 1 || lLevel > AGPMCHAR_MAX_LEVEL)
		return FALSE;

	INT32				lTotalSkillPoint	= lLevel;
	INT32				lCurrentSkillPoint	= 0;
	
	if(lBeforeLevel > lLevel)
		return FALSE;
	
	// 2007.07.08. steeple
	// ���� �Ǹ� �߰� ��ų ����Ʈ�� �־�� �Ѵ�.
	if(lLevel >= AGPMSKILL_BONUS_POINT_START_LEVEL && lLevel <= AGPMSKILL_BONUS_POINT_END_LEVEL)
	{
		lTotalSkillPoint += lLevel - AGPMSKILL_BONUS_POINT_START_LEVEL + 1;
	}
	else if(lLevel > AGPMSKILL_BONUS_POINT_END_LEVEL)
	{
		lTotalSkillPoint += AGPMSKILL_BONUS_POINT_END_LEVEL - AGPMSKILL_BONUS_POINT_START_LEVEL + 1;
	}

	INT32	lIndexTable	= 0;

	INT32	lInputCostSP	= 0;

	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData((ApBase *) pcsCharacter);

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		//		AgpdSkill	*pcsSkill	= GetSkill(pcsAttachData->m_alSkillID[i]);
		AgpdSkill	*pcsSkill	= pcsAttachData->m_apcsSkill[i];
		if (!pcsSkill)
			break;

		lInputCostSP	= 0;
		EnumCallback(AGPMSKILL_CB_GET_INPUT_TOTAL_COST_SP, pcsSkill, &lInputCostSP);

		lTotalSkillPoint -= lInputCostSP;
	}

	// ���� pcsCharacter�� SP�� ������ ���´�.
	//////////////////////////////////////////////////////////////////////////

	lCurrentSkillPoint	= m_pagpmCharacter->GetSkillPoint(pcsCharacter);

	INT32	lUpdateSkillPoint	= lTotalSkillPoint - lCurrentSkillPoint;
	if (lUpdateSkillPoint == 0)
		return TRUE;

	return m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, lUpdateSkillPoint);
	
}

BOOL AgpmSkill::AdjustHeroicPoint(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	INT32 lTotalHeroicPoint = 0;
	INT32 lAddHeroicPoint = 0;

	INT32 lCurrentLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
	if(lCurrentLevel < 1 || lCurrentLevel >= AGPMCHAR_MAX_LEVEL)
		return FALSE;

	// This Part is Getting Total Heroic Point
	if(lCurrentLevel >= AGPMCHARACTER_MINIMUM_GET_HEROIC_POINT_LEVEL)
	{
		// First if a user's Level is more than 90Lv set a basic Heroic Point
		lTotalHeroicPoint = AGPMSKILL_BASE_HEROIC_POINT;

		if(lCurrentLevel >= AGPMSKILL_DOUBLE_HEROIC_POINT_START_LEVEL)
		{
			if(lCurrentLevel >= AGPMSKILL_TRIPLE_HEROIC_POINT_START_LEVEL)
			{
				lAddHeroicPoint = AGPMSKILL_TRIPLE_HEROIC_POINT_START_LEVEL - AGPMSKILL_DOUBLE_HEROIC_POINT_START_LEVEL;
				lTotalHeroicPoint += lAddHeroicPoint * 2;

				lAddHeroicPoint = lCurrentLevel - AGPMSKILL_TRIPLE_HEROIC_POINT_START_LEVEL + 1;
				lTotalHeroicPoint += lAddHeroicPoint * 3;
			}
			else
			{
				lAddHeroicPoint = lCurrentLevel - AGPMSKILL_DOUBLE_HEROIC_POINT_START_LEVEL + 1;
				lTotalHeroicPoint += lAddHeroicPoint * 2;
			}
		}
	}

	// Realization Here that Sub HeroicPoint by HeroicSkill
	AgpdSkillAttachData *pcsAttachSkill = GetAttachSkillData((ApBase*)pcsCharacter);
	if(NULL == pcsAttachSkill)
		return FALSE;

	INT32 lInputHeroicPoint = 0;

	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
	{
		AgpdSkill *pcsSkill = pcsAttachSkill->m_apcsSkill[i];
		if(NULL == pcsSkill)
			break;

		if(IsHeroicSkill((AgpdSkillTemplate*)pcsSkill->m_pcsTemplate) == FALSE)
			continue;
	
		lInputHeroicPoint = 0;
		EnumCallback(AGPMSKILL_CB_GET_INPUT_TOTAL_COST_HEROIC_POINT, pcsSkill, &lInputHeroicPoint);

		lTotalHeroicPoint -= lInputHeroicPoint;
	}

	return m_pagpmCharacter->UpdateHeroicPoint(pcsCharacter, lTotalHeroicPoint, TRUE);
}

//		IsMasteryActive
//	Functions
//		- skill mastery �� lMasteryIndex �����͸��� ���� Active �������� �����Ѵ�.
//	Arguments
//		- pcsBase : �����͸��� ������ base
//		- lMasteryIndex : �˻��� �����͸� �ε���
//	Return value
//		- BOOL : 
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmSkill::IsMasteryActive(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	// �����͸��� �����´�.
//	AgpdSkillMastery	*pcsSkillMastery	= GetMastery(pcsBase);
//	if (!pcsSkillMastery)
//		return FALSE;
//
//	if (pcsSkillMastery[lMasteryIndex].m_ucTotalInputSP > 0)
//		return TRUE;
//
//	return FALSE;
//}

//BOOL AgpmSkill::IsMasterySPFull(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	// �����͸��� �����´�.
//	AgpdSkillMastery	*pcsSkillMastery	= GetMastery(pcsBase);
//	if (!pcsSkillMastery)
//		return FALSE;
//
//	return FALSE;
//}

//		GetInputTotalSP
//	Functions
//		- skill mastery �� lMasteryIndex �����͸��� ���ݱ��� ���Ե� SP�� ������ �����Ѵ�.
//	Arguments
//		- pcsBase : �����͸��� ������ base
//		- lMasteryIndex : �����͸� �ε���
//	Return value
//		- INT32 : Total SP
///////////////////////////////////////////////////////////////////////////////
//INT32 AgpmSkill::GetInputTotalSP(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return (-1);
//
//	// �����͸��� �����´�.
//	AgpdSkillMastery	*pcsSkillMastery	= GetMastery(pcsBase);
//	if (!pcsSkillMastery)
//		return FALSE;
//
//	return (INT32) pcsSkillMastery[lMasteryIndex].m_ucTotalInputSP;
//}

//INT64 AgpmSkill::GetRollbackCost(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return (-1);
//
//	// ���⼭ RollbackCost�� ���Ѵ�.
//
//	INT32	lMasteryTotalSP	= 0;
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		lMasteryTotalSP += GetInputTotalSP(pcsBase, i);
//	}
//
//	// rollback cost formula
//	// MAX( Total Mastery Skill Point - 5, 0 ) ^ 2 (5 ���ϴ� ��¥)
//
//	if (lMasteryTotalSP <= 5)
//		return 0;
//
//	return (lMasteryTotalSP - 5) * (lMasteryTotalSP - 5);
//}

//BOOL AgpmSkill::CheckMasteryRollbackCost(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	INT64	lCost	= GetRollbackCost(pcsBase);
//	if (lCost < 0)
//		return FALSE;
//	else if (lCost == 0)
//		return TRUE;
//
//	INT64	llMoney	= 0;
//	if (!m_pagpmCharacter->GetMoney((AgpdCharacter *) pcsBase, &llMoney))
//		return FALSE;
//
//	if (llMoney >= lCost)
//		return TRUE;
//
//	return FALSE;
//}

//BOOL AgpmSkill::PayMasteryRollbackCost(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	AgpdSkillAttachData *pcsAttachData = GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	INT64	lCost	= GetRollbackCost(pcsBase);
//	if (lCost < 0)
//		return FALSE;
//	else if (lCost == 0)
//		return TRUE;
//
//	return m_pagpmCharacter->SubMoney((AgpdCharacter *) pcsBase, (INT64) lCost);
//}

//BOOL AgpmSkill::ResetAllMastery(ApBase *pcsBase)
//{
//	if (!pcsBase || pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return FALSE;
//
//	// Mastery�� �ʱ�ȭ ��Ų��.
//	// ���� �����͸��� �� �ִ� ��� skill point�� ����, ���� �����ߴ� ��� ��ų�鵵 �� �ʱ�ȭ ��Ų��.
//
//	// Rollback Process�� �� ��� �� �Լ��� �ҷ��ش�.
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	// first, remove all skill
//	for (int i = 0; i < AGPMSKILL_MAX_SKILL_OWN; ++i)
//	{
//		if (pcsAttachData->m_alSkillID[i] == AP_INVALID_SKILLID)
//			break;
//
//		RemoveSkill(pcsAttachData->m_alSkillID[i]);
//	}
//
//	// reset mastery tree
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= GetMasteryTemplate(pcsBase);
//	if (!pcsMasteryTemplate)
//		return FALSE;
//
//	CopyMemory(&pcsAttachData->m_csMasteryTemplate, pcsMasteryTemplate, sizeof(AgpdSkillMasteryTemplate));
//
//	// reset mastery point
//	for (i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		pcsAttachData->m_csMastery[i].m_ucTotalInputSP	= 0;
//	}
//
//	AdjustSkillPoint((AgpdCharacter *) pcsBase);
//
//	return TRUE;
//}

BOOL AgpmSkill::CheckOwnerSP(ApBase *pcsBase)
{
	if (!pcsBase)
		return FALSE;

	INT32	lSkillPoint	= 0;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			lSkillPoint = m_pagpmCharacter->GetSkillPoint((AgpdCharacter *) pcsBase);
		}
		break;
	}

	if (lSkillPoint > 0)
		return TRUE;

	return FALSE;
}

//BOOL AgpmSkill::OnReceiveSkillMastery(BOOL bReceivedFromServer, INT8 cOperation, ApBase *pcsBase, INT32 lMasteryIndex, PVOID pvPacketMastery, INT8 cAddSPResult, PVOID pvSelectSkillTID, INT16 nSelectSkillTIDSize)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	INT8	cMasteryIndex			= (-1);
//	UINT8	ucTotalInputSP			= 0;
//	UINT8	aucTotalInputSP[AGPMSKILL_MAX_MASTERY];
//	ZeroMemory(aucTotalInputSP, sizeof(UINT8) * AGPMSKILL_MAX_MASTERY);
//
//	PVOID	pvPacketMasteryNode[128];
//	ZeroMemory(pvPacketMasteryNode, sizeof(PVOID) * 128);
//
//	if (pvPacketMastery)
//		m_csPacketMastery.GetField(FALSE, pvPacketMastery, 0,
//									&cMasteryIndex,
//									&ucTotalInputSP,
//									&aucTotalInputSP,
//									&pvPacketMasteryNode
//									);
//
//	BOOL	bRetval = FALSE;
//
//	switch (cOperation) {
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE:
//		{
//			//bRetval = OnReceiveMasterySpecialize(pcsBase, ppvPacketMasteryTree);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP:
//		{
//			bRetval = OnReceiveMasteryAddSP(pcsBase, lMasteryIndex, (INT32 *) pvSelectSkillTID, nSelectSkillTIDSize);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ADD_SP_RESULT:
//		{
//			if (bReceivedFromServer)
//				bRetval = OnReceiveMasteryAddSPResult(pcsBase, lMasteryIndex, cAddSPResult, (INT32 *) pvSelectSkillTID, nSelectSkillTIDSize);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_UPDATE:
//		{
//			if (bReceivedFromServer)
//				bRetval = OnReceiveMasteryUpdate(pcsBase, cMasteryIndex, ucTotalInputSP, aucTotalInputSP);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_NODE_UPDATE:
//		{
//			if (bReceivedFromServer)
//				bRetval = OnReceiveMasteryNodeUpdate(pcsBase, pvPacketMasteryNode);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_SUCCESS:
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_SPECIALIZE_FAIL:
//		{
//			bRetval = EnumCallback(AGPMSKILL_CB_MASTERY_SPECIALIZE_RESULT, pcsBase, &cOperation);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK:
//		{
//			bRetval = EnumCallback(AGPMSKILL_CB_MASTERY_ROLLBACK, pcsBase, NULL);
//		}
//		break;
//
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_SUCCESS:
//	case AGPMSKILL_PACKET_OPERATION_MASTERY_ROLLBACK_FAIL:
//		{
//			bRetval = EnumCallback(AGPMSKILL_CB_MASTERY_ROLLBACK_RESULT, pcsBase, &cOperation);
//		}
//		break;
//
//	default:
//		break;
//	}
//
//	return bRetval;
//}

//BOOL AgpmSkill::OnReceiveMasteryAddSP(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectSkillTID, INT16 nSelectSkillTIDSize)
//{
//	if (!pcsBase ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	INT32	lSelectTID[10];
//	ZeroMemory(lSelectTID, sizeof(INT32) * 10);
//
//	if (plSelectSkillTID)
//	{
//		CopyMemory(lSelectTID, plSelectSkillTID, nSelectSkillTIDSize);
//	}
//
//	AgpmSkillAddSPToMasteryResult	eResult	= (AgpmSkillAddSPToMasteryResult) AddSPToMastery(pcsBase, lMasteryIndex, lSelectTID, nSelectSkillTIDSize);
//
//	INT32	lNumSelectTID	= 0;
//
//	for (int i = 0; i < 10; ++i)
//	{
//		if (lSelectTID[i] == 0)
//		{
//			lNumSelectTID	= i;
//			break;
//		}
//	}
//
//	PVOID	pvBuffer[4];
//	pvBuffer[0]	= (PVOID) eResult;
//	pvBuffer[1]	= (PVOID) lSelectTID;
//	pvBuffer[2]	= (PVOID) lNumSelectTID;
//	pvBuffer[3]	= (PVOID) lMasteryIndex;
//
//	EnumCallback(AGPMSKILL_CB_MASTERY_ADD_SP_RESULT, pcsBase, pvBuffer);
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::OnReceiveMasteryAddSPResult(ApBase *pcsBase, INT32 lMasteryIndex, INT8 cAddSPResult, INT32 *plSelectSkillTID, INT16 nSelectSkillTIDSize)
//{
//	if (!pcsBase ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	PVOID	pvBuffer[4];
//	pvBuffer[0]	= (PVOID) cAddSPResult;
//	pvBuffer[1]	= (PVOID) plSelectSkillTID;
//	pvBuffer[2]	= (PVOID) nSelectSkillTIDSize;
//	pvBuffer[3]	= (PVOID) lMasteryIndex;
//
//	EnumCallback(AGPMSKILL_CB_MASTERY_ADD_SP_RESULT, pcsBase, pvBuffer);
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::OnReceiveMasteryUpdate(ApBase *pcsBase, INT32 lMasteryIndex, UINT8 ucTotalInputSP, UINT8 *pucTotalInputSP)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData = GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	if (lMasteryIndex >= 0 &&
//		lMasteryIndex < AGPMSKILL_MAX_MASTERY)
//	{
//		pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP	= ucTotalInputSP;
//
//		EnumCallback(AGPMSKILL_CB_MASTERY_UPDATE, pcsBase, &lMasteryIndex);
//	}
//	else
//	{
//		if (!pucTotalInputSP)
//			return FALSE;
//
//		for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//		{
//			pcsAttachData->m_csMastery[i].m_ucTotalInputSP	= pucTotalInputSP[i];
//		}
//
//		EnumCallback(AGPMSKILL_CB_MASTERY_UPDATE, pcsBase, NULL);
//	}
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::OnReceiveMasteryNodeUpdate(ApBase *pcsBase, PVOID *ppvPacketMasteryNode)
//{
//	if (!pcsBase ||
//		!ppvPacketMasteryNode)
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData = GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	INT8	cMastery		= 0;
//	INT8	cSkillTreeIndex	= 0;
//	INT8	cNodeIndex		= 0;
//	INT32	lSkillID		= 0;
//	UINT8	ucSPForActive	= 0;
//
//	for (int i = 0; i < 128; ++i)
//	{
//		if (!ppvPacketMasteryNode[i])
//			break;
//
//		cMastery		= 0;
//		cSkillTreeIndex	= 0;
//		cNodeIndex		= 0;
//		lSkillID		= 0;
//		ucSPForActive	= 0;
//
//		m_csPacketMasteryNode.GetField(FALSE, ppvPacketMasteryNode[i], 0,
//										&cMastery,
//										&cSkillTreeIndex,
//										&cNodeIndex,
//										&lSkillID,
//										&ucSPForActive);
//
//		if (cMastery < 0 || cMastery >= AGPMSKILL_MAX_MASTERY ||
//			cSkillTreeIndex < 0 || cSkillTreeIndex >= AGPMSKILL_MAX_NUM_SKILL_NODE ||
//			cNodeIndex < 0 || cNodeIndex >= AGPMSKILL_MAX_MASTERY_TREE_DEPTH)
//			continue;
//
//		pcsAttachData->m_csMasteryTemplate.m_csMastery[cMastery].m_csSkillTree[cSkillTreeIndex][cNodeIndex].m_bIsChanged	= TRUE;
//		pcsAttachData->m_csMasteryTemplate.m_csMastery[cMastery].m_csSkillTree[cSkillTreeIndex][cNodeIndex].m_lSkillID		= lSkillID;
//		pcsAttachData->m_csMasteryTemplate.m_csMastery[cMastery].m_csSkillTree[cSkillTreeIndex][cNodeIndex].m_ucSPForActive	= ucSPForActive;
//	}
//
//	EnumCallback(AGPMSKILL_CB_MASTERY_NODE_UPDATE, pcsBase, NULL);
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::OnReceiveMasterySpecialize(ApBase *pcsBase, PVOID *ppvPacketMasteryTree)
//{
//	if (!pcsBase || !ppvPacketMasteryTree)
//		return FALSE;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		if (ppvPacketMasteryTree[i])
//		{
//			INT8	cMasteryIndex			= (-1);
//			INT8	cConstSpecialization	= (-1);
//			INT8	cInstantSpecialization	= (-1);
//
//			m_csPacketMasteryTree.GetField(FALSE, ppvPacketMasteryTree[i], 0,
//								&cMasteryIndex,
//								NULL,
//								&cConstSpecialization,
//								&cInstantSpecialization,
//								NULL,
//								NULL,
//								NULL,
//								NULL);
//
//			if (cMasteryIndex == (-1))
//				break;
//
//			if (cConstSpecialization != (-1))
//			{
//				MasterySpecialize(pcsBase, (INT32) cMasteryIndex, (eAgpmSkillMasterySpecializedType) cConstSpecialization, FALSE);
//			}
//
//			if (cInstantSpecialization != (-1))
//			{
//				MasterySpecialize(pcsBase, (INT32) cMasteryIndex, (eAgpmSkillMasterySpecializedType) cInstantSpecialization, TRUE);
//			}
//		}	
//	}
//
//	return TRUE;
//}

//INT64 AgpmSkill::GetSpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return (-1);
//
//	// �ӽ� Ưȭ�� ����� 10+2*(�ش� ��ų �����͸� Point)^2 (��ȭ����)�̴�.
//	////////////////////////////////////////////////////////////////////////
//
//	INT32	lMasterySP = GetInputTotalSP(pcsBase, lMasteryIndex);
//
//	/*
//	if (lMasterySP <= 0)
//		return (-1);
//	*/
//
//	return (INT64) (10 + 2 * pow(lMasterySP, 2));
//}
//
//BOOL AgpmSkill::CheckSpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	if (!IsMasteryActive(pcsBase, lMasteryIndex))
//		return FALSE;
//
//	INT64	lCost	= GetSpecializeCost(pcsBase, lMasteryIndex);
//	if (lCost < 0)
//		return FALSE;
//
//	INT64	llMoney	= 0;
//	if (!m_pagpmCharacter->GetMoney((AgpdCharacter *) pcsBase, &llMoney))
//		return FALSE;
//
//	if (llMoney >= lCost)
//		return TRUE;
//
//	return FALSE;
//}
//
//BOOL AgpmSkill::CalcSpecializeLevel(ApBase *pcsBase)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return TRUE;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	// specialize�� �� �� �ִ� ����Ʈ���� �˻��Ѵ�.
//	INT32	lLevel	= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsBase);
//	if (lLevel < 0)
//		return FALSE;
//
//	BOOL	bSpecialize = FALSE;
//	for (int i = 0; i < AGPMSKILL_MAX_SPECIALIZE_LEVEL_POINT; ++i)
//	{
//		if (g_lSpecializeLevelPoint[i] == 0)
//			break;
//
//		if (lLevel >= g_lSpecializeLevelPoint[i])
//		{
//			if (!pcsAttachData->m_bSpecializeLevelPoint[i])
//			{
//				pcsAttachData->m_bSpecializeLevelPoint[i] = TRUE;
//				bSpecialize = TRUE;
//			}
//		}
//		else
//			break;
//	}
//
//	return bSpecialize;
//}
//
//BOOL AgpmSkill::CheckSpecializeLevel(ApBase *pcsBase, INT32 *plLevel)
//{
//	if (!pcsBase)
//		return FALSE;
//
//	if (pcsBase->m_eType != APBASE_TYPE_CHARACTER)
//		return TRUE;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	AgpdSkillMastery	*pcsMastery		= GetMastery(pcsBase);
//	if (!pcsMastery)
//		return FALSE;
//
//	INT8	cSpecialize					= 0;
//
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		cSpecialize |= pcsMastery[i].cConstSpecialization;
//	}
//
//	INT32	lNumSpecialize				= 0;
//	for (i = 1; i <= AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_DISTANCE; i = i << 1)
//	{
//		if (cSpecialize & i)
//			++lNumSpecialize;
//	}
//
//	CalcSpecializeLevel(pcsBase);
//
//	if (plLevel)
//		*plLevel = g_lSpecializeLevelPoint[lNumSpecialize];
//
//	return pcsAttachData->m_bSpecializeLevelPoint[lNumSpecialize];
//}
//
//BOOL AgpmSkill::PaySpecializeCost(ApBase *pcsBase, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	INT64	lCost	= GetSpecializeCost(pcsBase, lMasteryIndex);
//	if (lCost < 0)
//		return FALSE;
//
//	return m_pagpmCharacter->SubMoney((AgpdCharacter *) pcsBase, (INT64) lCost);
//}
//
//BOOL AgpmSkill::CheckSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	// check already specialized type
//	//if (pcsAttachData->m_cMasterySpecialize & eType)
//	//	return FALSE;
//
//	AgpdSkillMastery	*pcsMastery		= GetMastery(pcsBase);
//	if (!pcsMastery)
//		return FALSE;
//
//	INT8	cConstSpecialize	= 0;
//	for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//	{
//		cConstSpecialize |= pcsMastery[i].cConstSpecialization;
//	}
//
//	//if (cConstSpecialize & eType ||
//	//	pcsMastery[lMasteryIndex].cInstantSpecialization & eType)
//	if (cConstSpecialize & eType)
//		return FALSE;
//
//	INT32	lNumConstSpecialize	= 0;
//	INT32	lNumInstantSpecialize = 0;
//	for (i = 1; i <= AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_DISTANCE; i = i << 1)
//	{
//		if (pcsMastery[lMasteryIndex].cConstSpecialization & i)
//			++lNumConstSpecialize;
//
//		if (pcsMastery[lMasteryIndex].cInstantSpecialization & i)
//			++lNumInstantSpecialize;
//	}
//
//	if (bInstant)
//	{
//		if (lNumInstantSpecialize >= AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY)
//			return FALSE;
//
//		if (!CheckSpecializeCost(pcsBase, lMasteryIndex))
//			return FALSE;
//	}
//	else
//	{
//		if (lNumConstSpecialize >= AGPMSKILL_MAX_CONST_SPECIALIZE_IN_MASTERY)
//			return FALSE;
//
//		if (!CheckSpecializeLevel(pcsBase, NULL))
//			return FALSE;
//	}
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::ProcessSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	if (!CheckSpecialize(pcsBase, lMasteryIndex, eType, bInstant))
//		return FALSE;
//
//	AgpdSkillAttachData	*pcsAttachData = GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	if (!bInstant)
//	{
//		// eType Ưȭ�� �ӽ� Ưȭ�� �̹� Ưȭ�� �Ǿ� �ִ��� ���캻��.
//		// Ưȭ�Ǿ� �ִٸ� �ӽ�Ưȭ�� ���ش�.
//		for (int i = 0; i < AGPMSKILL_MAX_MASTERY; ++i)
//		{
//			if (pcsAttachData->m_csMastery[i].cInstantSpecialization & eType)
//			{
//				MasteryUnSpecialize(pcsBase, i, eType);
//				break;
//			}
//		}
//
//		pcsAttachData->m_csMastery[lMasteryIndex].cConstSpecialization		|= eType;
//	}
//	else
//	{
//		if (!PaySpecializeCost(pcsBase, lMasteryIndex))
//			return FALSE;
//
//		pcsAttachData->m_csMastery[lMasteryIndex].cInstantSpecialization	|= eType;
//	}
//
//	return TRUE;
//}
//
//BOOL AgpmSkill::MasterySpecializeCheckOnly(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant)
//{
//	BOOL		bSuccess = CheckSpecialize(pcsBase, lMasteryIndex, eType, bInstant);
//
//	INT32		lBuffer[4];
//	lBuffer[0]	= lMasteryIndex;
//	lBuffer[1]	= (INT32) eType;
//	lBuffer[2]	= (INT32) bInstant;
//	lBuffer[3]	= (INT32) bSuccess;
//
//	return EnumCallback(AGPMSKILL_CB_MASTERY_SPECIALIZE, pcsBase, lBuffer);
//}
//
//BOOL AgpmSkill::MasterySpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType, BOOL bInstant)
//{
//	BOOL		bSuccess = ProcessSpecialize(pcsBase, lMasteryIndex, eType, bInstant);
//
//	INT32		lBuffer[4];
//	lBuffer[0]	= lMasteryIndex;
//	lBuffer[1]	= (INT32) eType;
//	lBuffer[2]	= (INT32) bInstant;
//	lBuffer[3]	= (INT32) bSuccess;
//
//	return EnumCallback(AGPMSKILL_CB_MASTERY_SPECIALIZE, pcsBase, lBuffer);
//}
//
//BOOL AgpmSkill::MasteryUnSpecialize(ApBase *pcsBase, INT32 lMasteryIndex, eAgpmSkillMasterySpecializedType eType)
//{
//	if (!pcsBase || lMasteryIndex < 0 || lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return FALSE;
//
//	AgpdSkillMastery	*pcsMastery	= GetMastery(pcsBase);
//	if (!pcsMastery)
//		return FALSE;
//
//	pcsMastery[lMasteryIndex].cInstantSpecialization ^= eType;
//
//	for (int i = 0; i < AGPMSKILL_MAX_INSTANT_SPECIALIZE_IN_MASTERY; ++i)
//	{
//		if (pcsMastery[lMasteryIndex].cInstantSpecializedType[i] == (INT8) eType)
//		{
//			pcsMastery[lMasteryIndex].ulStartInstantSpecializedTime[i] = 0;
//			pcsMastery[lMasteryIndex].ulEndInstantSpecializedTime[i] = 0;
//			break;
//		}
//	}
//
//	return EnumCallback(AGPMSKILL_CB_MASTERY_UNSPECIALIZE, pcsBase, &lMasteryIndex);
//}
//
//INT32 AgpmSkill::CheckAddSPToMastery(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize)
//{
//	if (!pcsBase ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!plSelectTID)
//		return AGPMSKILL_MASTERY_RESULT_FAIL;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return AGPMSKILL_MASTERY_RESULT_FAIL;
//
//	// if base type is character, check base's race and class
//	if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
//	{
//		INT32	lSkillPoint	= m_pagpmCharacter->GetSkillPoint((AgpdCharacter *) pcsBase);
//		if (lSkillPoint < 1)
//			return AGPMSKILL_MASTERY_RESULT_FAIL;
//
//		INT32	lLevel		= m_pagpmCharacter->GetLevel((AgpdCharacter *) pcsBase);
//		if (lLevel <= (INT32) pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP)
//			return AGPMSKILL_MASTERY_RESULT_SP_FULL;
//
//		INT32	lRace	= m_pagpmFactors->GetRace(&((AgpdCharacter *) pcsBase)->m_csFactor);
//		INT32	lClass	= m_pagpmFactors->GetClass(&((AgpdCharacter *) pcsBase)->m_csFactor);
//
//		if (pcsAttachData->m_csMasteryTemplate.m_lCharRaceType != lRace ||
//			pcsAttachData->m_csMasteryTemplate.m_lCharClassType != lClass)
//			return AGPMSKILL_MASTERY_RESULT_FAIL;
//	}
//
//	if (lMasteryIndex >= pcsAttachData->m_csMasteryTemplate.m_ucNumMastery)
//		return AGPMSKILL_MASTERY_RESULT_FAIL;
//
//	UINT8	ucInputSP	= pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP + 1;
//
//	UINT8	ucDepth		= 0;
//
//	INT32	lSelectTID[AGPMSKILL_MAX_NUM_SKILL_NODE][5];
//	INT32	lSelectDepth[AGPMSKILL_MAX_NUM_SKILL_NODE];
//	INT32	lNumSelectSkill[AGPMSKILL_MAX_NUM_SKILL_NODE];
//
//	ZeroMemory(lSelectTID, sizeof(INT32) * 5 * AGPMSKILL_MAX_NUM_SKILL_NODE);
//	ZeroMemory(lSelectDepth, sizeof(INT32) * AGPMSKILL_MAX_NUM_SKILL_NODE);
//	ZeroMemory(lNumSelectSkill, sizeof(INT32) * AGPMSKILL_MAX_NUM_SKILL_NODE);
//
//	BOOL	bIsAlreadyMaxInputSP	= TRUE;
//
//	for (int i = 0; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//	{
//		if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][0].m_ucMaxInputSP == 0)
//			continue;
//
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucMaxInputSP < ucInputSP)
//				continue;
//
//			ucDepth	= pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth;
//
//			break;
//		}
//
//		if (j == AGPMSKILL_MAX_MASTERY_TREE_DEPTH)
//			continue;
//
//		bIsAlreadyMaxInputSP	= FALSE;
//
//		for ( ; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth != ucDepth)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucSPForActive == ucInputSP)
//			{
//				++lSelectDepth[i];
//
//				if (plSelectTID[0] != 0)
//				{
//					for (int k = 0; k < lArraySize; ++k)
//					{
//						if (plSelectTID[k] == 0)
//							break;
//
//						if (plSelectTID[k] == pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID)
//							break;
//					}
//
//					if (k == lArraySize)
//						lSelectTID[i][lNumSelectSkill[i]++]	= pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID;
//				}
//				else
//				{
//					lSelectTID[i][lNumSelectSkill[i]++] = pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID;
//				}
//			}
//		}
//	}
//
//	INT32	lSelectIndex			= 0;
//
//	for (i = 0; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//	{
//		if (lNumSelectSkill[i] < 2 ||
//			lNumSelectSkill[i] < lSelectDepth[i])
//			continue;
//
//		CopyMemory(plSelectTID + lSelectIndex, lSelectTID[i], sizeof(INT32) * lNumSelectSkill[i]);
//		
//		plSelectTID[lNumSelectSkill[i]]	= (-1);
//
//		lSelectIndex	= lNumSelectSkill[i] + 1;
//	}
//
//	if (lSelectIndex > 0)
//		return AGPMSKILL_MASTERY_RESULT_NEED_SELECT;
//
//	if (bIsAlreadyMaxInputSP)
//		return AGPMSKILL_MASTERY_RESULT_SP_FULL;
//
//	return AGPMSKILL_MASTERY_RESULT_OK;
//}
//
//INT32 AgpmSkill::AddSPToMastery(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize)
//{
//	if (!pcsBase ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!plSelectTID)
//		return AGPMSKILL_MASTERY_RESULT_FAIL;
//
//	INT32	lCheckResult	= CheckAddSPToMastery(pcsBase, lMasteryIndex, plSelectTID, lArraySize);
//
//	if ((AgpmSkillAddSPToMasteryResult) lCheckResult == AGPMSKILL_MASTERY_RESULT_OK)
//	{
//		if (AddSP(pcsBase, lMasteryIndex, plSelectTID, lArraySize))
//			return AGPMSKILL_MASTERY_RESULT_OK;
//		else
//			lCheckResult	= (INT32) AGPMSKILL_MASTERY_RESULT_FAIL;
//	}
//
//	return lCheckResult;
//}
//
//BOOL AgpmSkill::AddSP(ApBase *pcsBase, INT32 lMasteryIndex, INT32 *plSelectTID, INT32 lArraySize)
//{
//	if (!pcsBase ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY ||
//		!plSelectTID)
//		return FALSE;
//
//	for (int i = 0; i < lArraySize; ++i)
//	{
//		if (plSelectTID[i] == 0)
//			break;
//
//		if (!CheckLearnableSkill(pcsBase, plSelectTID[i]))
//			return FALSE;
//	}
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return FALSE;
//
//	AgpdMasteryTree	csTempMastery;
//	CopyMemory(&csTempMastery, pcsAttachData->m_csMasteryTemplate.m_csMastery + lMasteryIndex, sizeof(AgpdMasteryTree));
//
//	INT32	lChangedNode[20][2];
//	INT32	lIndexChangedNode	= 0;
//	ZeroMemory(lChangedNode, sizeof(INT32) * 20 * 2);
//
//	AgpdSkill	*pcsLearnSkill[20];
//	INT32	lIndexLearnSkill	= 0;
//	ZeroMemory(pcsLearnSkill, sizeof(AgpdSkill *) * 20);
//
//	AgpdSkill	*pcsAddSPSkill[40];
//	INT32	lIndexAddSPSkill	= 0;
//	ZeroMemory(pcsAddSPSkill, sizeof(AgpdSkill *) * 40);
//
//	UINT8	ucDepth		= 0;
//	UINT8	ucInputSP	= pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP + 1;
//
//	BOOL	bIsFailed	= FALSE;
//
//	for (i = 0; i < AGPMSKILL_MAX_NUM_SKILL_NODE; ++i)
//	{
//		if (bIsFailed)
//			break;
//
//		if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][0].m_ucMaxInputSP == 0)
//			continue;
//
//		for (int j = 0; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucMaxInputSP < ucInputSP)
//				continue;
//
//			ucDepth	= pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth;
//
//			break;
//		}
//
//		if (j == AGPMSKILL_MAX_MASTERY_TREE_DEPTH)
//			continue;
//
//		INT32	lNumDepthNode	= 0;
//		for (int nIndex = j; nIndex < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++nIndex)
//		{
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][nIndex].m_ucDepth != ucDepth)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][nIndex].m_ucSPForActive == ucInputSP)
//				++lNumDepthNode;
//		}
//
//		for ( ; j < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++j)
//		{
//			if (bIsFailed)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucDepth != ucDepth)
//				break;
//
//			if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucSPForActive == ucInputSP)
//			{
//				if (lNumDepthNode <= 1 || plSelectTID[0] == 0)
//				{
//					AgpdSkill *pcsSkill	= LearnSkill(pcsBase, pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID, lMasteryIndex, ucInputSP);
//					if (!pcsSkill)
//					{	
//						bIsFailed	= TRUE;
//						break;
//					}
//
//					if (lIndexLearnSkill == 20)
//					{
//						bIsFailed	= TRUE;
//						break;
//					}
//					else
//						pcsLearnSkill[lIndexLearnSkill++]	= pcsSkill;
//
//					pcsSkill->m_lMaxInputSkillPoint	= (INT32) pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucMaxInputSP;
//
//					pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID	= pcsSkill->m_lID;
//
//					pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_bIsChanged	= TRUE;
//
//					if (lIndexChangedNode == 20)
//					{
//						bIsFailed	= TRUE;
//						break;
//					}
//					else
//					{
//						lChangedNode[lIndexChangedNode][0]		= i;
//						lChangedNode[lIndexChangedNode++][1]	= j;
//					}
//				}
//				else
//				{
//					for (int k = 0; k < lArraySize; ++k)
//					{
//						if (bIsFailed)
//							break;
//
//						if (plSelectTID[k] == 0)
//							break;
//
//						if (plSelectTID[k] == pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID)
//							break;
//					}
//
//					if (k == lArraySize)
//					{
//						pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucSPForActive	= ucInputSP + AGPMSKILL_MAX_UPGRADE_SP;
//
//						pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_bIsChanged	= TRUE;
//
//						if (lIndexChangedNode == 20)
//						{
//							bIsFailed	= TRUE;
//							break;
//						}
//						else
//						{
//							lChangedNode[lIndexChangedNode][0]		= i;
//							lChangedNode[lIndexChangedNode++][1]	= j;
//						}
//
//						/*
//						// ������Ʈ �Ǿ���. ����/Ŭ���̾�Ʈ�� ��ũ ��������.
//						PVOID	pvBuffer[3];
//						pvBuffer[0]	= (PVOID) lMasteryIndex;
//						pvBuffer[1]	= (PVOID) i;
//						pvBuffer[2]	= (PVOID) j;
//
//						EnumCallback(AGPMSKILL_CB_MASTERY_NODE_UPDATE, pcsBase, pvBuffer);
//						*/
//					}
//					else
//					{
//						AgpdSkill *pcsSkill	= LearnSkill(pcsBase, pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillTID, lMasteryIndex, ucInputSP);
//						if (!pcsSkill)
//						{	
//							bIsFailed	= TRUE;
//							break;
//						}
//
//						if (lIndexLearnSkill == 20)
//						{
//							bIsFailed	= TRUE;
//							break;
//						}
//						else
//							pcsLearnSkill[lIndexLearnSkill++]	= pcsSkill;
//
//						pcsSkill->m_lMaxInputSkillPoint	= (INT32) pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucMaxInputSP;
//
//						pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID	= pcsSkill->m_lID;
//
//						pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_bIsChanged	= TRUE;
//
//						if (lIndexChangedNode == 20)
//						{
//							bIsFailed	= TRUE;
//							break;
//						}
//						else
//						{
//							lChangedNode[lIndexChangedNode][0]		= i;
//							lChangedNode[lIndexChangedNode++][1]	= j;
//						}
//					}
//				}
//			}
//			else if (pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_ucSPForActive < ucInputSP)
//			{
//				if (lIndexAddSPSkill == 40)
//				{
//					bIsFailed	= TRUE;
//					break;
//				}
//
//				pcsAddSPSkill[lIndexAddSPSkill++]	= GetSkill(pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID);
//				//AddSkillPoint(GetSkill(pcsAttachData->m_csMasteryTemplate.m_csMastery[lMasteryIndex].m_csSkillTree[i][j].m_lSkillID), 1);
//			}
//			else
//			{
//				// Do nothing!
//			}
//		}
//	}
//
//	if (bIsFailed)
//	{
//		// ���д�. ���� �ǵ�����.
//		CopyMemory(pcsAttachData->m_csMasteryTemplate.m_csMastery + lMasteryIndex, &csTempMastery, sizeof(AgpdMasteryTree));
//
//		for (i = 0; i < lIndexLearnSkill; ++i)
//		{
//			if (pcsLearnSkill[i] == NULL)
//				continue;
//
//			RemoveSkill(pcsLearnSkill[i]->m_lID);
//		}
//
//		return FALSE;
//	}
//	else
//	{
//		// �����̴�. ��� �����Ų��.
//		PVOID	pvBuffer[3];
//		pvBuffer[0]	= (PVOID) lMasteryIndex;
//		pvBuffer[1]	= (PVOID) lChangedNode;
//		pvBuffer[2]	= (PVOID) lIndexChangedNode;
//		EnumCallback(AGPMSKILL_CB_MASTERY_NODE_UPDATE, pcsBase, pvBuffer);
//
//		for (i = 0; i < lIndexAddSPSkill; ++i)
//		{
//			if (pcsAddSPSkill[i] == NULL)
//				continue;
//
//			AddSkillPoint(pcsAddSPSkill[i], 1);
//		}
//
//		++pcsAttachData->m_csMastery[lMasteryIndex].m_ucTotalInputSP;
//
//		EnumCallback(AGPMSKILL_CB_MASTERY_UPDATE, pcsBase, &lMasteryIndex);
//
//		if (pcsBase->m_eType == APBASE_TYPE_CHARACTER)
//			m_pagpmCharacter->UpdateSkillPoint((AgpdCharacter *) pcsBase, -1);
//
//		return TRUE;
//	}
//
//	return FALSE;
//}
//
//INT32 AgpmSkill::GetSpecializedCost(AgpdSkill *pcsSkill, INT32 lOriginalCost)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase || lOriginalCost <= 0)
//		return lOriginalCost;
//
//	AgpdSkillMastery	*pcsMastery = GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return lOriginalCost;
//
//	INT32	lSpecializedCost	= lOriginalCost;
//
//	// 1.	����� ���� : ������ ����� ��� SP/MP�� 10% ���ҵȴ�. (��ü ����)
//	if ((pcsMastery[pcsSkill->m_lMasteryIndex].cConstSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_DECREASE_COST) ||
//		(pcsMastery[pcsSkill->m_lMasteryIndex].cInstantSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_DECREASE_COST))
//	{
//		lSpecializedCost	= (INT32) (lSpecializedCost * 0.9);
//	}
//
//	return lOriginalCost;
//}
//
//INT32 AgpmSkill::GetSpecializedDamage(AgpdSkill *pcsSkill, INT32 lOriginalDamage)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase || lOriginalDamage <= 0)
//		return lOriginalDamage;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= GetMasteryTemplate(pcsSkill->m_pcsBase);
//	if (!pcsMasteryTemplate)
//		return lOriginalDamage;
//
//	// 2.	������ ���� : ������ ����� ������ ����� 10% ���ʽ��� �޴´� (���� �����͸� �ܴ� �Ұ�)
//	if (pcsMasteryTemplate->m_csMastery[pcsSkill->m_lMasteryIndex].m_eMasteryType != AGPMSKILL_MASTERY_TYPE_ATTACK)
//		return lOriginalDamage;
//
//	AgpdSkillMastery			*pcsMastery			= GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return lOriginalDamage;
//
//	INT32	lSpecializedDamage	= lOriginalDamage;
//
//	if ((pcsMastery[pcsSkill->m_lMasteryIndex].cConstSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DMG) ||
//		(pcsMastery[pcsSkill->m_lMasteryIndex].cInstantSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DMG))
//	{
//		lSpecializedDamage	= (INT32) (lSpecializedDamage * 1.1);
//	}
//
//	return lSpecializedDamage;
//}
//
//INT32 AgpmSkill::GetSpecializedDuration(AgpdSkill *pcsSkill, INT32 lOriginalDuration)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase || lOriginalDuration <= 0)
//		return lOriginalDuration;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= GetMasteryTemplate(pcsSkill->m_pcsBase);
//	if (!pcsMasteryTemplate)
//		return lOriginalDuration;
//
//	// 3.	���� �ð� ���� : ������ ����� ���� �ð��� 25% ����Ѵ�. (���� �����͸� ����)
//	if (pcsMasteryTemplate->m_csMastery[pcsSkill->m_lMasteryIndex].m_eMasteryType == AGPMSKILL_MASTERY_TYPE_ATTACK)
//		return lOriginalDuration;
//
//	AgpdSkillMastery			*pcsMastery			= GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return lOriginalDuration;
//
//	INT32	lSpecializedDuration	= lOriginalDuration;
//
//	if ((pcsMastery[pcsSkill->m_lMasteryIndex].cConstSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DURATION) ||
//		(pcsMastery[pcsSkill->m_lMasteryIndex].cInstantSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_DURATION))
//	{
//		lSpecializedDuration	= (INT32) (lSpecializedDuration * 1.25);
//	}
//
//	return lSpecializedDuration;
//}
//
//INT32 AgpmSkill::GetSpecializedArea(AgpdSkill *pcsSkill, INT32 lOriginalArea)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase || lOriginalArea <= 0)
//		return lOriginalArea;
//
//	// 4.	���� ���� : ������ ����� ��� Area Factor�� 10% ����Ѵ�
//	AgpdSkillMastery			*pcsMastery			= GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return lOriginalArea;
//
//	INT32	lSpecializedArea	= lOriginalArea;
//
//	if ((pcsMastery[pcsSkill->m_lMasteryIndex].cConstSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_AREA) ||
//		(pcsMastery[pcsSkill->m_lMasteryIndex].cInstantSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_AREA))
//	{
//		lSpecializedArea	= (INT32) (lSpecializedArea * 1.1);
//	}
//
//	return lSpecializedArea;
//}
//
//INT32 AgpmSkill::GetSpecializedDistance(AgpdSkill *pcsSkill, INT32 lOriginalDistance)
//{
//	if (!pcsSkill || !pcsSkill->m_pcsBase || lOriginalDistance <= 0)
//		return lOriginalDistance;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= GetMasteryTemplate(pcsSkill->m_pcsBase);
//	if (!pcsMasteryTemplate)
//		return lOriginalDistance;
//
//	// 5.	�Ÿ� ���� : ������ ����� Range Factor�� 20% ����Ѵ�.(���� ���� �����͸� ����)
//	if (pcsMasteryTemplate->m_csMastery[pcsSkill->m_lMasteryIndex].m_eMasteryType == AGPMSKILL_MASTERY_TYPE_ATTACK &&
//		pcsMasteryTemplate->m_lCharClassType == AUCHARCLASS_TYPE_KNIGHT)
//	{
//		return lOriginalDistance;
//	}
//
//	AgpdSkillMastery			*pcsMastery			= GetMastery(pcsSkill->m_pcsBase);
//	if (!pcsMastery)
//		return lOriginalDistance;
//
//	INT32	lSpecializedDistance	= lOriginalDistance;
//
//	if ((pcsMastery[pcsSkill->m_lMasteryIndex].cConstSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_DISTANCE) ||
//		(pcsMastery[pcsSkill->m_lMasteryIndex].cInstantSpecialization & AGPMSKILL_MASTERY_SPECIALIZED_INCREASE_TARGET_DISTANCE))
//	{
//		lSpecializedDistance	= (INT32) (lSpecializedDistance * 1.2);
//	}
//
//	return lSpecializedDistance;
//}
//
//INT32 AgpmSkill::GetBaseSPForActive(ApBase *pcsBase, INT32 lSkillTID)
//{
//	if (!pcsBase || lSkillTID == 0)
//		return 0;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return 0;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= &pcsAttachData->m_csMasteryTemplate;
//	if (!pcsMasteryTemplate)
//		return 0;
//
//	for (int i = 0; i < pcsMasteryTemplate->m_ucNumMastery; ++i)
//	{
//		INT32	lActiveSP	= GetBaseSPForActive(pcsBase, lSkillTID, i);
//
//		if (lActiveSP > 0)
//			return lActiveSP;
//	}
//
//	return 0;
//}
//
//INT32 AgpmSkill::GetBaseSPForActive(ApBase *pcsBase, INT32 lSkillTID, INT32 lMasteryIndex)
//{
//	if (!pcsBase || lSkillTID == 0 ||
//		lMasteryIndex < 0 ||
//		lMasteryIndex >= AGPMSKILL_MAX_MASTERY)
//		return 0;
//
//	AgpdSkillAttachData	*pcsAttachData	= GetAttachSkillData(pcsBase);
//	if (!pcsAttachData)
//		return 0;
//
//	AgpdSkillMasteryTemplate	*pcsMasteryTemplate	= &pcsAttachData->m_csMasteryTemplate;
//	if (!pcsMasteryTemplate)
//		return 0;
//
//	for (int j = 0; j < AGPMSKILL_MAX_MASTERY_SKILL; ++j)
//	{
//		if (pcsMasteryTemplate->m_csMastery[lMasteryIndex].m_csSkillTree[j][0].m_lSkillTID == 0)
//			break;
//
//		for (int k = 0; k < AGPMSKILL_MAX_MASTERY_TREE_DEPTH; ++k)
//		{
//			AgpdSkillTemplate	*pcsSkillTemplate	= GetSkillTemplate(pcsMasteryTemplate->m_csMastery[lMasteryIndex].m_csSkillTree[j][k].m_lSkillTID);
//			if (pcsSkillTemplate && pcsSkillTemplate->m_lID == lSkillTID)
//			{
//				return (INT32) pcsMasteryTemplate->m_csMastery[lMasteryIndex].m_csSkillTree[j][k].m_ucSPForActive;
//			}
//		}
//	}
//
//	return 0;
//}