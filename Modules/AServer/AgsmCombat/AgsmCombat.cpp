/******************************************************************************
Module:  AgsmCombat.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 14
******************************************************************************/

#include <time.h>

#include "AgpmCharacter.h"
#include "AgsmCombat.h"
#include "AgsdCharacter.h"
#include "ApMemoryTracker.h"

AgsmCombat::AgsmCombat()
{
	SetModuleName("AgsmCombat");

	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsmCombat::~AgsmCombat()
{
}

BOOL AgsmCombat::OnAddModule()
{
	m_pagpmGrid		 = (AgpmGrid *)		 GetModule("AgpmGrid");
	m_pagpmFactors	 = (AgpmFactors *)   GetModule("AgpmFactors");
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem		 = (AgpmItem *)		 GetModule("AgpmItem");
	m_pagpmCombat	 = (AgpmCombat *)	 GetModule("AgpmCombat");
	m_pagpmOptimizedPacket2	= (AgpmOptimizedPacket2 *)	GetModule("AgpmOptimizedPacket2");
	m_pagpmAI2		 = (AgpmAI2 *)		 GetModule("AgpmAI2" );
	m_pagpmSummons	 = (AgpmSummons*)	 GetModule("AgpmSummons");
	m_pagpmSiegeWar  = (AgpmSiegeWar*)	 GetModule("AgpmSiegeWar");

	m_pagsmAOIFilter = (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmItem		 = (AgsmItem *)		 GetModule("AgsmItem");
	m_pagsmFactors	 = (AgsmFactors *)   GetModule("AgsmFactors");
	m_pagsmParty	 = (AgsmParty *)	 GetModule("AgsmParty");

	if (!m_pagpmGrid || !m_pagpmFactors || !m_pagpmCharacter || !m_pagsmAOIFilter || !m_pagsmCharacter || !m_pagsmFactors || 
		!m_pagsmParty || !m_pagpmSummons || !m_pagpmSiegeWar)
		return FALSE;

	if (!m_pagpmItem ||	!m_pagsmItem ||	!m_pagpmCombat)
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackActionAttack(CBActionAttack, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmCombat::OnInit()
{
	return TRUE;
}

BOOL AgsmCombat::OnDestroy()
{
	return TRUE;
}

BOOL AgsmCombat::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgsmCombat::OnValid(CHAR* pszData, INT16 nSize)
{
	return TRUE;
}

PVOID AgsmCombat::MakePacketAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex, INT16 *pnPacketLength)
{
	if (!pAttackChar || !pTargetChar || !pnPacketLength)
		return NULL;

	if (m_pagpmOptimizedPacket2)
		return m_pagpmOptimizedPacket2->MakePacketCharAction(pAttackChar, pTargetChar, cAttackResult, pvPacketFactor, 0, cComboInfo, bForceAttack, ulAdditionalEffect, cHitIndex, pnPacketLength);
	else
	{
		INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

		PVOID	pvPacketAction	= NULL;

		INT8	cAction = AGPDCHAR_ACTION_TYPE_ATTACK;

		pvPacketAction = m_pagpmCharacter->m_csPacketAction.MakePacket(FALSE, pnPacketLength, 0,
																	 &cAction,
																	 &pTargetChar->m_lID,
																	 (lSkillTID != AP_INVALID_SKILLID) ? &lSkillTID : NULL,
																	 &cAttackResult,
																	 pvPacketFactor,
																	 &pAttackChar->m_stPos,
																	 (cComboInfo > 0) ? &cComboInfo : NULL,
																	 (bForceAttack) ? &bForceAttack : NULL,
																	 (ulAdditionalEffect) ? &ulAdditionalEffect : NULL,
																	 (cHitIndex) ? & cHitIndex : NULL);

		if (!pvPacketAction)
			return FALSE;

		PVOID pvPacket = m_pagpmCharacter->m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMCHARACTER_PACKET_TYPE,
																 &cOperation,					// Operation
																 &pAttackChar->m_lID,			// Character ID
																 NULL,							// Character Template ID
																 NULL,							// Game ID
																 NULL,							// Character Status
																 NULL,							// Move Packet
																 pvPacketAction,				// Action Packet
																 NULL,							// Factor Packet
																 NULL,							// llMoney
																 NULL,							// bank money
																 NULL,							// cash
																 NULL,							// character action status
																 NULL,							// character criminal status
																 NULL,							// attacker id (������� ������ �ʿ�)
																 NULL,							// ���� �����Ǽ� �ʿ� �������� ����
																 NULL,							// region index
																 NULL,							// social action index
																 NULL,							// special status
																 NULL,							// is transform status
																 NULL,							// skill initialization text
																 NULL,							// face index
																 NULL,							// hair index
																 NULL,							// Option Flag
																 NULL,							// bank size
																 NULL,							// event status flag
																 NULL,							// remained criminal status time
																 NULL,							// remained murderer point time
																 NULL,							// nick name
																 NULL,							// gameguard
																 NULL							// last killed time in battlesquare
																 );

		m_pagpmCharacter->m_csPacketAction.FreePacket(pvPacketAction);

		return pvPacket;
	}

	return NULL;
}

BOOL AgsmCombat::SendAttackResult(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, PVOID pvPacketFactor, INT8 cAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex)
{
	// ���� �ߴٴ� ������ �ֺ��� �ִ� ĳ���͵鿡�� ������.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, pvPacketFactor, cAttackResult, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	/*
	m_pagsmParty->SendPacketToNearPartyMember(pAttackChar, pvPacket, nPacketLength);
	m_pagsmParty->SendPacketToNearPartyMember(pTargetChar, pvPacket, nPacketLength);
	*/

	/*
	if (!m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pAttackChar->m_stPos))
	{
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

		return FALSE;
	}
	*/

	// ��Ƽ�� ��� �ִٸ� ��Ƽ���鰣�� ��ũ�� ���߾��ش�.
	//m_pagsmParty->SendPacketToParty(pAttackChar, pvPacket, nPacketLength);
	//m_pagsmParty->SendPacketToParty(pTargetChar, pvPacket, nPacketLength);

	/*
	m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pTargetChar->m_stPos);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
	*/

	// �����ڿ� ����ڿ� ���õ� �ѵ����� ��Ŷ�� ������.
	UINT32	ulAttackDPNID	= m_pagsmCharacter->GetCharDPNID(pAttackChar);
	UINT32	ulTargetDPNID	= m_pagsmCharacter->GetCharDPNID(pTargetChar);

	if (ulAttackDPNID > 0)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pAttackChar->m_lID);

		SendPacket(pvPacket, nPacketLength, ulAttackDPNID);
	}
	if (ulTargetDPNID > 0)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pTargetChar->m_lID);

		SendPacket(pvPacket, nPacketLength, ulTargetDPNID);
	}

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	// factor ������ �� ��Ŷ�� ����� �ֺ��ѵ����� �ѷ��ش�.
	nPacketLength	= 0;

	PVOID	pvPacketHP	= m_pagpmFactors->MakePacketFactorsCharHP(&pTargetChar->m_csFactor);

	pvPacket = MakePacketAttackResult(pAttackChar, pTargetChar, pvPacketHP, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex, &nPacketLength);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketHP);

	if (!pvPacket)
		return FALSE;

	UINT32	ulExceptNIDs[2];
	INT32	lNumNID		= 0;

	if (ulAttackDPNID != 0)
	{
		ulExceptNIDs[lNumNID]	= ulAttackDPNID;
		++lNumNID;
	}
	if (ulTargetDPNID != 0)
	{
		ulExceptNIDs[lNumNID]	= ulTargetDPNID;
		++lNumNID;
	}

	m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pAttackChar->m_lID);
	
	m_pagsmAOIFilter->SendPacketNearExceptNIDs(pvPacket, nPacketLength, pTargetChar->m_stPos, m_pagpmCharacter->GetRealRegionIndex(pTargetChar), ulExceptNIDs, lNumNID, PACKET_PRIORITY_5);

	// �����ڿ�, Ÿ���� �Ѵ� ���� ������Ʈ���,
	// �����ϴ� �ʿ��� �� ��Ŷ�� �����ش�.
	// �׸��� �´� �ʿ��ٰ� HP ��ũ��Ŷ�� �߰��� �����ش�.
	if(m_pagpmSiegeWar->IsSiegeWarMonster(pAttackChar) && m_pagpmSiegeWar->IsSiegeWarMonster(pTargetChar))
	{
		m_pagsmAOIFilter->SendPacketNearExceptNIDs(pvPacket, nPacketLength, pAttackChar->m_stPos, m_pagpmCharacter->GetRealRegionIndex(pAttackChar), ulExceptNIDs, lNumNID, PACKET_PRIORITY_5);
		m_pagsmCharacter->SyncHP(pTargetChar);
	}

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return TRUE;
}

BOOL AgsmCombat::SendAttackResultNotEnoughArrow(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// ���� �ߴٴ� ������ �ֺ��� �ִ� ĳ���͵鿡�� ������.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// �����ڿ� ����ڿ� ���õ� �ѵ����� ��Ŷ�� ������.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SendAttackResultNotEnoughBolt(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// ���� �ߴٴ� ������ �ֺ��� �ִ� ĳ���͵鿡�� ������.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// �����ڿ� ����ڿ� ���õ� �ѵ����� ��Ŷ�� ������.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SendAttackResultNotEnoughMP(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	// ���� �ߴٴ� ������ �ֺ��� �ִ� ĳ���͵鿡�� ������.
	INT16	nPacketLength	= 0;

	PVOID	pvPacket	= MakePacketAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP, 0, FALSE, AP_INVALID_SKILLID, 0, 0, &nPacketLength);
	if (!pvPacket)
		return FALSE;

	// �����ڿ� ����ڿ� ���õ� �ѵ����� ��Ŷ�� ������.
	BOOL	bSendResult	= SendPacket(pvPacket, nPacketLength, m_pagsmCharacter->GetCharDPNID(pAttackChar));

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmCombat::SetCallbackAttack(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_ATTACK, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackCheckDefense(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_CHECK_DEFENSE, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackDamageAdjust(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackPreCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_PRE_CHECK_COMBAT, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackPostCheckCombat(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_POST_CHECK_COMBAT, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackNPCDialog(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_NPC_DIALOG, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackConvertDamage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_CONVERT_DAMAGE, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackAttackStart(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_ATTACK_START, pfCallback, pClass);
}

BOOL AgsmCombat::SetCallbackDamageAdjustSiegeWar(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST_SIEGEWAR, pfCallback, pClass);
}

BOOL AgsmCombat::CBActionAttack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmCombat::CBActionAttack");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCombat	*pThis = (AgsmCombat *) pClass;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBActionAttack"));

	PVOID	*pvBuffer	= (PVOID *) pCustData;

	AgpdCharacter	*pAttackChar	= (AgpdCharacter *) pData;
	AgpdCharacter	*pTargetChar	= (AgpdCharacter *) pvBuffer[0];

	UINT8			cComboInfo		= (UINT8)PtrToUint(pvBuffer[3]);
	BOOL			bForceAttack	= PtrToInt(pvBuffer[4]);
	UINT32			ulAdditionalEffect = PtrToUint(pvBuffer[5]);
	UINT8			cHitIndex		= (UINT8) pvBuffer[6];

	if (!pAttackChar || !pTargetChar)
		return FALSE;

	if (pAttackChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
		pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return FALSE;

	// 2005.10.17. steeple
	if (pAttackChar->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HALT)
		return FALSE;

	// 2005.11.02. steeple
	// ������ ��� ��� ������ ���ݺҰ�. (Ż �� Ÿ�� ���� ����)
	if (pThis->m_pagpmItem->IsEquipStandard(pAttackChar) && pAttackChar->m_bRidable)
		return FALSE;

	if (!pThis->m_pagpmCharacter->CheckActionStatus(pAttackChar, AGPDCHAR_STATUS_ATTACK))
		return FALSE;

	if (pThis->m_pagpmCharacter->IsActionBlockCondition(pAttackChar))
		return FALSE;

	// 2007.10.09. steeple
	// Sleep �����̸� ���ݸ��Ѵ�.
	if (pThis->m_pagpmCharacter->IsStatusSleep(pAttackChar))
		return FALSE;

	// ������ �����̸� �������� ���Ѵ�. 2005.10.07. steeple
	if (pThis->m_pagpmCharacter->IsStatusFullTransparent(pTargetChar))
		return FALSE;

	// ������ �Ͻ� �����̸� �������� ���Ѵ�. 2006.01.11. steeple
	if (pThis->m_pagpmCharacter->IsStatusInvincible(pTargetChar))
		return FALSE;

	if (!pThis->m_pagpmCharacter->CheckVaildNormalAttackTarget(pAttackChar, pTargetChar, bForceAttack))
		return FALSE;



	// ���ݿ� �ʿ��� ������ �־�� �ϴ��� �˻��Ѵ�.
	/////////////////////////////////////////////////////////////////////////
	AgpdCharacterActionResultType	eActionResult	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	pThis->m_pagpmCharacter->CheckActionRequirement(pAttackChar, &eActionResult, AGPDCHAR_ACTION_TYPE_ATTACK);

	switch (eActionResult) {
	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_ARROW:
		{
			pThis->SendAttackResultNotEnoughArrow(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;

	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_BOLT:
		{
			pThis->SendAttackResultNotEnoughBolt(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;

	case AGPDCHAR_ACTION_RESULT_TYPE_NOT_ENOUGH_MP:
		{
			pThis->SendAttackResultNotEnoughMP(pAttackChar, pTargetChar);
			return FALSE;
		}
		break;
	}

	AgsdCharacter	*pAgsdAttackChar = pThis->m_pagsmCharacter->GetADCharacter(pAttackChar);

	if (pThis->GetClockCount() < pAgsdAttackChar->m_ulNextAttackTime)
	{
		OutputDebugString("Attack Skipped\n");
		//pAgsdAttackChar->m_ulNextAttackTime = pThis->GetClockCount();
		return FALSE;
	}

	// ���� ���ݽð��� �����Ѵ�.
	UINT32		ulAttackIntervalMSec = pThis->m_pagpmCharacter->GetAttackIntervalMSec(pAttackChar);
	if (ulAttackIntervalMSec <= 0)
		return FALSE;
	pAgsdAttackChar->m_ulNextAttackTime = (UINT32)((FLOAT)pThis->GetClockCount() + (FLOAT)ulAttackIntervalMSec * 0.9f);

	//������� �Դٸ� �켱 ������ �õ��ѰŴ�. PC�� ���� �õ��� �õ��� ��� �׿� ���� ��׷θ� �־�����.
	pThis->m_pagsmCharacter->AddAgroPoint( pTargetChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_SLOW, 1 );

	// �����ϴµ� �־ �����ؾ��� ����� �ִٸ� ���⼭ �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////
	pThis->m_pagpmCharacter->PayActionCost(pAttackChar, AGPDCHAR_ACTION_TYPE_ATTACK);

	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= pAttackChar;
	stAttackResult.pTargetChar	= pTargetChar;
	stAttackResult.nDamage		= 0;
	stAttackResult.ulCheckResult	= 0;

	// �����ڰ� �Ͻ� ���� ���¶�� Ǯ���ش�. 2006.01.09. steeple
	if(pThis->m_pagpmCharacter->IsStatusInvincible(pAttackChar))
		pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pAttackChar, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);

	// �����ڰ� ��ȯ���� ���̹��ε�, ������ �����̶�� Ǯ���ش�. 2006.01.11. steeple
	if(pThis->m_pagpmCharacter->IsStatusSummoner(pAttackChar) || pThis->m_pagpmCharacter->IsStatusTame(pAttackChar))
	{
		AgpdCharacter* pcsOwner = pThis->m_pagpmCharacter->GetCharacterLock(pThis->m_pagpmSummons->GetOwnerCID(pAttackChar));
		if(pcsOwner)
		{
			if(pThis->m_pagpmCharacter->IsStatusInvincible(pcsOwner))
				pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pcsOwner, AGPDCHAR_SPECIAL_STATUS_INVINCIBLE);

			pcsOwner->m_Mutex.Release();
		}
	}

	// ������ �ߴٴ� ���� �ҷ��ش�. For PvP
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_ATTACK_START, &stAttackResult, NULL);

	// ���� �˻� ������ ó���ؾ��� �κ��� �ִٸ� ���⼭ ó���Ѵ�.
	//
	// �Ʒ� �ݹ� ��(AgsmSkill) ���� �����ڰ� �����̸� Ǯ���� ó���ߴ�. 2005.10.07. steeple
	//
	////////////////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_PRE_CHECK_COMBAT, &stAttackResult, NULL);

	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_FAILED_ATTACK)
	{
		if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_COUNTERATTACK)
		{
			// �������� ������ ���� ����ڰ� �����ڿ��� �Ϲ� ���ݵ������� ������.

			// �ϴ� ���������� ��� ǥ�ø� ��� ����� ���� �����ؾ� �Ѵ�.
			// �������� ���� ������ ��Ŷ���� ������ �ݰ������� ���� ������ ���� ���� ���ؾ� �Ѵ�.
			//
			//
			//

			// ��ư �������� ���س��´�.

			BOOL		bIsEquipWeapon			= pThis->m_pagpmItem->IsEquipWeapon(pTargetChar);

			INT32		lCounterAttackDamage	= pThis->m_pagpmCombat->CalcPhysicalAttack(pTargetChar, pAttackChar, bIsEquipWeapon);

			return pThis->ApplyAttackDamage(pTargetChar, pAttackChar, lCounterAttackDamage, lCounterAttackDamage, NULL, AGPDCHAR_ACTION_RESULT_TYPE_COUNTER_ATTACK, 0, 0, 0, 0, cHitIndex);
		}

		//if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_EVADE_ATTACK)
		//{
		//	// �������� ������ ȸ���ߴ�.
		//	return pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_EVADE, cComboInfo, bForceAttack, stAttackResult.lSkillTID, 0);
		//}
	}

	// 2005.12.22. steeple
	AgpdCharacterActionResultType eEvadeType = pThis->m_pagpmCombat->ProcessEvade(pAttackChar, pTargetChar);
	if(eEvadeType != AGPDCHAR_ACTION_RESULT_TYPE_NONE)
	{
		// �������� ������ ȸ���ߴ�.
		return pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, eEvadeType, cComboInfo, bForceAttack, stAttackResult.lSkillTID, 0, 0);
	}

	PVOID		pvAttackPacketFactor = NULL;
	PVOID		pvTargetPacketFactor = NULL;
	AgpdFactor	csUpdateFactor;

	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

//	BOOL		abIsSuccessSecondSpirit[AGPD_FACTORS_ATTRIBUTE_MAX_TYPE];
//	ZeroMemory(abIsSuccessSecondSpirit, sizeof(BOOL) * AGPD_FACTORS_ATTRIBUTE_MAX_TYPE);

	ApSafeArray<BOOL, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>		abIsSuccessSecondSpirit;
	abIsSuccessSecondSpirit.MemSetAll();

	AgpdCharacterActionResultType	eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_NONE;

	BOOL		bIsEquipWeapon	= pThis->m_pagpmItem->IsEquipWeapon(pAttackChar);

	int			nCombatPreCheck = 0;

	BOOL		bIsAttrInvincible = pThis->IsAttrInvincible(pTargetChar);	// Ÿ���� �Ӽ� ��������

	if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_IGNORE_PHYSICAL_DEFENCE)	// PrecheckCombat ����� ������� �������� üũ
		nCombatPreCheck |= AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_PHYSICAL_DEFENCE;

	if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_IGNORE_ATTRIBUTE_DEFENCE)	// precheckCombat ����� �Ӽ���� �������� üũ
		nCombatPreCheck |= AGPMCOMBAT_PROCESS_COMBAT_ARG_IGNORE_ATTRIBUTE_DEFENCE; 

	/*
	INT16 nDamage = pThis->m_pagsmFactors->ProcessCombat(
														&pAttackChar->m_csFactor,
														&pTargetChar->m_csFactor,
														AGSM_FACTORS_MELEE_ATTACK, 
														(100 - stAttackResult.nDamage),
														lReflectDamage,
														&pvAttackPacketFactor,
														&pvTargetPacketFactor,
														&csUpdateFactor
														);
	*/
	INT32 lHeroicDamage = 0;

	//������ �Ѵ�.
	INT32 nDamage = pThis->m_pagpmCombat->ProcessCombat(pAttackChar,
														 pTargetChar,
														 &csUpdateFactor,
														 &abIsSuccessSecondSpirit[0],
														 &eResultType,
														 bIsEquipWeapon,
														 bIsAttrInvincible,
														 &lHeroicDamage,
														 nCombatPreCheck);

	AgsdCharacter	*pcsAgsdTarget	= pThis->m_pagsmCharacter->GetADCharacter(pTargetChar);

	if (pcsAgsdTarget && pcsAgsdTarget->m_bIsSuperMan)
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;

	if (pcsAgsdTarget && pcsAgsdTarget->m_bIsMaxDefence)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

		pThis->ApplyAttackDamage(pAttackChar, pTargetChar, 1, 1, NULL, eResultType, 0, 0, 0, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}

	// �Ϲ� ���ݿ� ���ؼ� ���� üũ�� �Ѵ�. 2007.07.02. steeple
	if (pThis->m_pagpmCharacter->IsStatusNormalATKInvincible(pTargetChar))
	{
		INT32 lRandom = pThis->m_pagpmCombat->m_csRandom.randInt(100);
		if(pcsAgsdTarget->m_stInvincibleInfo.lNormalATKProbability > lRandom)
			eResultType = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS;
	}

	// ���п� ���� �� �ǰų� ������ ������ ��� �� ��Ŷ�� ������ ������.
	///////////////////////////////////////////////////////////////////////////
	if (eResultType == AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS ||
		eResultType == AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_BLOCK)
	{
		//2004.10.25�� ���� ��� ���յǸ鼭 �ּ�ó����.
/*		//�̽��� ��, PC�� ������ ���ݽ� MISS,BLock�� ������ ���� ��ø��� ���� ��������.
		if( pThis->m_pagpmCharacter->IsPC( pAttackChar ) && pThis->m_pagpmCharacter->IsMonster( pTargetChar ) )
		{
			eAgpdEventNPCDialogMob		eDialogType;

			eDialogType = AGPD_NPCDIALOG_MOB_PC_MISS;

			pThis->EnumCallback( AGSMCOMBAT_CB_ID_NPC_DIALOG, (ApBase *)pAttackChar, (void *)&eDialogType );
		}*/

		pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, eResultType, cComboInfo, bForceAttack, AP_INVALID_SKILLID, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}
	else if (nDamage <= 0)
	{
		pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS, cComboInfo, bForceAttack, AP_INVALID_SKILLID, 0, 0);

		pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return TRUE;
	}

	// �ϴ� ������ ����̴�.
	///////////////////////////////////////////////////////////////////////////

	// Ÿ���� sleep �����̸� Ǯ���ش�. 2007.06.27. steeple
	if(pThis->m_pagpmCharacter->IsStatusSleep(pTargetChar))
		pThis->m_pagpmCharacter->UpdateUnsetSpecialStatus(pTargetChar, AGPDCHAR_SPECIAL_STATUS_SLEEP);

	// ��Ƽ�� ���� ���ʽ� �������� �ִ��� �˻��Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////
	if (nDamage > 0)
		nDamage += (INT32) (nDamage * (pThis->m_pagsmParty->GetPartyBonusDamage(pAttackChar, NULL) / 100.0));

	INT32	lMeleeDamage	= 0;
	pThis->m_pagpmFactors->GetValue(&csUpdateFactor, &lMeleeDamage, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
	stAttackResult.nDamage		= (-lMeleeDamage);

	stAttackResult.ulCheckResult	= 0;

	// ���� �˻� �� ó���ؾ��� �κ��� �ִٸ� ���⼭ ó���Ѵ�.
	/////////////////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_POST_CHECK_COMBAT, &stAttackResult, &csUpdateFactor);

	INT32	lReflectDamage		= 0;
	INT32	lConvertDamage		= 0;

	if (stAttackResult.nDamage > 0 && stAttackResult.ulCheckResult == 0)
	{
		// ���°� �ݰݵ������� üũ�Ѵ�.
		pThis->EnumCallback(AGSMCOMBAT_CB_ID_CHECK_DEFENSE, &stAttackResult, &lReflectDamage);
	}

	eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

	INT32	lAttackSkillTID		= AP_INVALID_SKILLID;
	INT32	lReflectSkillTId	= AP_INVALID_SKILLID;

	// Divide ó���� �� ó���� ���ش�. 2007.06.27. steeple
	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_DIVIDE)
	{
		 pThis->ProcessDivide(stAttackResult, &csUpdateFactor, &nDamage);
	}

	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_DEATH_STRIKE)
	{
		// �� �ѹ濡 �׿��� �Ѵ�.  Ÿ���� ���� HP��ŭ �������� �������� �׿�������.
		// ������ �ݹ鿡�� ó���Ǿ� ����.

		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_DEATH_STRIKE;

		lAttackSkillTID		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_MELEE_CRITICAL_STRIKE)
	{
		// ũ��Ƽ�� ��Ʈ����ũ��. �������� �ݹ鿡�� �̹� ����� �� �س��ұ�.. ���⼱ ���� ����..
		// ���� ����� ������ Ÿ���� CRITICAL�� �����Ѵ�.

		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_CRITICAL;

		lAttackSkillTID		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_REFLECT_DAMAGE)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_REFLECT_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_REDUCE_DAMAGE)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_HP ||
			 stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_MP)
	{
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_CONVERT_DAMAGE;

		lReflectSkillTId		= stAttackResult.lSkillTID;
		
		// �ӽ� ���� ��
		lConvertDamage = stAttackResult.nDamage;
		stAttackResult.nDamage = 0;
	}
	else if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_HP ||
			stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_MP)
	{
		// �ӽ� ����
		lConvertDamage = stAttackResult.nDamage;
	}
	else if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CHARGE)
	{
		eResultType = AGPDCHAR_ACTION_RESULT_TYPE_CHARGE;

		lAttackSkillTID			= stAttackResult.lSkillTID;
	}
	
	/*
	// ���� �������� ���� �ʿ��ϸ� ������ �Ѵ�.
	//	(damage adjust skill)
	//////////////////////////////////////////////////////////////
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST, pAttackChar, &stAttackResult.nDamage);
	*/

	//nDamage	= stAttackResult.nDamage;

	// ���� ���� �����°Ϳ� ���� ������ �ʿ��ϴ�.
	//		(SendAttack�� SendAttackResult�� �ϳ��� �����ϰ� ���ݰ� ��� ������ �Ѳ����� �����Ѵ�.
	///////////////////////////////////////////////////////
	//if (!bSendAttackResult)
	//	pThis->SendAttack(pAttackChar, pTargetChar);

	// ������ ���� ������ ����
	pThis->EnumCallback(AGSMCOMBAT_CB_ID_DAMAGE_ADJUST_SIEGEWAR, &stAttackResult, &csUpdateFactor);

	// ���̿� ���� ������ ó��. 2006.06.23. steeple
	stAttackResult.nDamage += pThis->AdjustHeightDamage(pAttackChar, pTargetChar, stAttackResult.nDamage);

	if (stAttackResult.nDamage > 0)
	{
		if(stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_LENS_STONE)
			ulAdditionalEffect |= AGPDCHAR_ADDITIONAL_EFFECT_LENS_STONE;

		pThis->ApplyAttackDamage(pAttackChar, pTargetChar, -(((-lMeleeDamage) - stAttackResult.nDamage) - nDamage), stAttackResult.nDamage, &csUpdateFactor, eResultType, cComboInfo, bForceAttack, lAttackSkillTID, ulAdditionalEffect, cHitIndex);

		// ĳ�� ������ó��. 2005.12.12. steeple
		pThis->m_pagpmItem->SubCashItemStackCountOnAttack(pAttackChar);
	}
	else
	{
		if(eResultType == AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE)
			pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_REDUCE_DAMAGE, cComboInfo, bForceAttack, lReflectSkillTId, ulAdditionalEffect, cHitIndex);
		else
			pThis->SendAttackResult(pAttackChar, pTargetChar, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_MISS, cComboInfo, bForceAttack, lReflectSkillTId, ulAdditionalEffect, cHitIndex);
	}

	if (lReflectDamage > 0)
	{
		// ������ Reflect �� ���� �� ���±� ������ ����� �ʱ�ȭ�� ���ش�.
		eResultType	= AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS;

		pThis->ApplyAttackDamage(pTargetChar, pAttackChar, lReflectDamage, lReflectDamage, NULL, eResultType, 0, bForceAttack, lReflectSkillTId, 0, 0);
	}

	// Convert Damage �� ���� ó���� �� �� �� ���ش�. - 2004.09.17. steeple
	if (stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_HP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_DAMAGE_TO_MP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_HP ||
		stAttackResult.ulCheckResult & AGSMCOMBAT_CHECKRESULT_CONVERT_ATK_DAMAGE_TO_MP)
	{
		// Convert Damage �� ���� �����ش�.
		stAttackResult.nDamage = lConvertDamage;
		pThis->EnumCallback(AGSMCOMBAT_CB_ID_CONVERT_DAMAGE, &stAttackResult, NULL);
	}

	pThis->m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

BOOL AgsmCombat::ApplyAttackDamage(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar, INT32 lTotalDamage, INT32 lDamageNormal, AgpdFactor *pcsUpdateFactor, AgpdCharacterActionResultType eAttackResult, UINT8 cComboInfo, BOOL bForceAttack, INT32 lSkillTID, UINT32 ulAdditionalEffect, UINT8 cHitIndex)
{
	if (!pAttackChar || !pTargetChar)
		return FALSE;

	if (lTotalDamage <= 0)
		return TRUE;

	//STOPWATCH2(GetModuleName(), _T("ApplyAttackDamage"));

	BOOL	bAllocFactor	= FALSE;
	if (!pcsUpdateFactor)
	{
		bAllocFactor	= TRUE;
		pcsUpdateFactor = new (AgpdFactor);
	}

	// ���⼭ ������ ���� �������� ���� ĳ������ status�� �ݿ��Ѵ�.
	AgpdFactor			*pcsUpdateFactorResult = (AgpdFactor *) m_pagpmFactors->SetFactor(pcsUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateFactorResult)
	{
		m_pagpmFactors->DestroyFactor(pcsUpdateFactor);
		if (bAllocFactor)
			delete(pcsUpdateFactor);

		return FALSE;
	}

	m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lDamageNormal), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);
	m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lTotalDamage), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	// ���� ���� HP�� ���������� �۴ٸ�.. (��, ���� ���̶��)
	// ���� MP, SP�� ��� 0���� ����� ������. (��ȹ���� �ش޶��� ��.��)
	///////////////////////////////////////////////////////////////////////////
	INT32	lCurrentTargetHP	= 0;
	INT32	lCurrentTargetMP	= 0;
	INT32	lCurrentTargetSP	= 0;

	m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if (lCurrentTargetHP <= lTotalDamage)
	{
		// �̳��� ���� �������̴�.
		// ���� MP, SP�� �����ͼ� 0���� ������ش�.
		m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(&pTargetChar->m_csFactor, &lCurrentTargetSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetMP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetSP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
		m_pagpmFactors->SetValue(pcsUpdateFactorResult, (-lCurrentTargetHP), AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	}

	PVOID pvTargetPacketFactor = m_pagpmFactors->CalcFactor(&pTargetChar->m_csFactor, pcsUpdateFactor, FALSE, TRUE, TRUE, FALSE);
	if (pvTargetPacketFactor)
	{
		SendAttackResult(pAttackChar, pTargetChar, pvTargetPacketFactor, eAttackResult, cComboInfo, bForceAttack, lSkillTID, ulAdditionalEffect, cHitIndex);

		m_pagpmFactors->m_csPacket.FreePacket(pvTargetPacketFactor);
	}
	m_pagsmFactors->ResetResultFactorDamageInfo(&pTargetChar->m_csFactor);
	m_pagsmFactors->ResetResultFactorDamageInfo(pcsUpdateFactor);

	// player�� �ӹ������� �˻��Ѵ�.
	if (m_pagsmCharacter->GetCharDPNID(pAttackChar) && m_pagsmCharacter->GetCharDPNID(pTargetChar))
	{
		// player���� �ο�°Ŷ�� ������� ������ ���� ������ ����Ʈ�� �߰��Ѵ�.
		m_pagpmCharacter->AddAttackerToList(pTargetChar, pAttackChar->m_lID);
	}

	// �����ڰ� PC, Ÿ���� ������ ����� ó��
	if( m_pagpmCharacter->IsPC(pAttackChar) && m_pagpmCharacter->IsMonster(pTargetChar) ||
		// �����ڰ� �����̰� Ÿ���� ��ȯ�� �϶��� ó��
		m_pagpmCharacter->IsMonster(pAttackChar) && m_pagpmCharacter->IsStatusSummoner(pTargetChar))
	{
		if (m_pagpmAI2)
		{
			//���� ������ �ִ� ���ϸ��Ͷ��?
			AgpdAI2ADChar		*pcsAgpdAI2ADChar = NULL;
			pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData( pTargetChar );

			if( pcsAgpdAI2ADChar )
			{
				if( pcsAgpdAI2ADChar->m_pcsBossCharacter )
				{
					//������ ����ġ�� ���ϸ��͸� ������ PC�� �ִ´�.
					m_pagsmCharacter->AddAgroPoint( pcsAgpdAI2ADChar->m_pcsBossCharacter, pAttackChar, AGPD_FACTORS_AGRO_TYPE_SLOW, lTotalDamage );
				}

				m_pagsmCharacter->AddAgroPoint( pTargetChar, pAttackChar, pcsUpdateFactorResult );
			}
		}
	}
	// �����ڰ� ��ȯ���� ���̹� �����̰� Ÿ���� ������ ����� ó��
	else if ( m_pagpmCharacter->IsMonster(pTargetChar) && 
			 (m_pagpmCharacter->IsStatusTame( pAttackChar ) || m_pagpmCharacter->IsStatusSummoner( pAttackChar ) ) )
	{
		INT32 lOwnerCID = m_pagpmSummons->GetOwnerCID( pAttackChar );
		if (0 != lOwnerCID)
		{
			AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacterLock(lOwnerCID);
			if (NULL != pcsCharacter)
			{
				// ��ȯ���� ���̹� ���Ͱ� ������ ��쿣 �������� �����ڰ� �ذ����� �ϰ� 
				// ��׷δ� �ش� ��ȯ���� ���̹� ���ͷ� �ο��Ѵ�.
				m_pagsmCharacter->AddHistory(pTargetChar, pcsCharacter, pcsUpdateFactorResult, GetClockCount());
				m_pagsmCharacter->AddAgroPoint(pTargetChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

				pcsCharacter->m_Mutex.Release();
			}
		}
	}
	// �����ڰ� �����̰� Ÿ���� PC�� ����� ó��
	else if ( m_pagpmCharacter->IsMonster( pAttackChar ) && m_pagpmCharacter->IsPC( pTargetChar ) )
	{
		// ���� Ÿ�� PC�� ��ȯ��, ���̹� ���͸� ������ �ִٸ� PC�� ���� ������ ��׷θ� ��ȯ��, ���̹� ���Ϳ��� �ο����ش�.
		AgpdSummonsADChar *pcsSummonsADChar = m_pagpmSummons->GetADCharacter( pTargetChar );
		ASSERT(pcsSummonsADChar);

		if (pcsSummonsADChar)
		{
			AgpdCharacter* pcsTempChar = NULL;

			if(!pcsSummonsADChar->m_SummonsArray.m_pSummons)
				return FALSE;

			AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
														pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
			AgpdSummonsArray::iterator iter = tmpVector.begin();
			while(iter != tmpVector.end())
			{
				pcsTempChar = m_pagpmCharacter->GetCharacterLock(iter->m_lCID);
				ASSERT(pcsTempChar);

				if (pcsTempChar)
				{
					m_pagsmCharacter->AddAgroPoint(pcsTempChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

					pcsTempChar->m_Mutex.Release();
				}

				++iter;
			}

			for (int i = 0; i < pcsSummonsADChar->m_TameArray.m_arrTame.size(); ++i)
			{
				pcsTempChar = m_pagpmCharacter->GetCharacterLock(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID);
				ASSERT(pcsTempChar);

				if (pcsTempChar)
				{
					m_pagsmCharacter->AddAgroPoint(pcsTempChar, pAttackChar, AGPD_FACTORS_AGRO_TYPE_STATIC, lTotalDamage);

					pcsTempChar->m_Mutex.Release();
				}
			}
		}
	}

	// 2005.11.29. steeple
	// �� ���� �г�Ƽ�� ���� �۾�
	if(m_pagpmCharacter->IsPC(pAttackChar) && m_pagpmCharacter->IsMonster(pTargetChar))
	{
		m_pagsmCharacter->AddTargetInfo(pAttackChar, pTargetChar->m_lID, GetClockCount());
	}

	// ���� �������� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////
	INT32			lItemID	= 0;

	AgpdItem	*pcsWeapon = m_pagpmItem->GetEquipWeapon( pAttackChar );

	if( pcsWeapon != NULL )
	{
		// ������ ���� ������ ����
		m_pagsmItem->ReduceDurability(pcsWeapon);
	}

	// ������ �� ������ ����
	m_pagsmItem->ReduceArmourDurability(pTargetChar);
	// ���� �������� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////////

	stAgsmCombatAttackResult	stAttackResult;
	memset(&stAttackResult, 0, sizeof(stAttackResult));
	stAttackResult.pAttackChar	= pAttackChar;
	stAttackResult.pTargetChar	= pTargetChar;
	stAttackResult.nDamage		= lTotalDamage;

	EnumCallback(AGSMCOMBAT_CB_ID_ATTACK, &stAttackResult, pcsUpdateFactor);

	m_pagpmFactors->DestroyFactor(pcsUpdateFactor);

	if (bAllocFactor)
		delete pcsUpdateFactor;

	return TRUE;
}

// 2006.06.23. steeple
// ���̿� ���� ������ ��ȭ���� �����Ѵ�.
INT32 AgsmCombat::AdjustHeightDamage(AgpdCharacter* pAttackChar, AgpdCharacter* pTargetChar, INT32 lDamage)
{
	if(!pAttackChar || !pTargetChar || !lDamage)
		return 0;

	// �ϴ� �������̰� ������ �� �ϴ��� ������ �Ѵ�.

	// ���� (2006.06.23) �� ������ ���� �۾��Ѵ�.

	return 0;
}

// 2007.06.27. steeple
// Divide Damage ���� �۾�.
INT32 AgsmCombat::ProcessDivide(stAgsmCombatAttackResult& stAttackResult, AgpdFactor* pcsUpdateFactor, INT32* plTotalDamage)
{
	if(!pcsUpdateFactor)
		return 0;

	if(!stAttackResult.nDamage && !stAttackResult.lDivideAttrRate && !stAttackResult.lDivideAttrCaster &&
		!stAttackResult.lDivideNormalRate && !stAttackResult.lDivideNormalCaster)
		return 0;

	// �Ϲݵ�����
	if(stAttackResult.lDivideNormalRate && stAttackResult.lDivideNormalCaster)
	{
		// Caster �� �־�߸� �۾��Ѵ�. ���� �ڱ� �ڽ��� �ƴϿ��� �Ѵ�.
		AgpdCharacter* pcsCaster = m_pagpmCharacter->GetCharacter(stAttackResult.lDivideNormalCaster);
		if(pcsCaster && !pcsCaster->m_bIsReadyRemove && pcsCaster->m_lID != stAttackResult.pTargetChar->m_lID)
		{
			AuAutoLock csLock(pcsCaster->m_Mutex);

			INT32 lReduceDamage = (INT32)((double)stAttackResult.nDamage * (double)stAttackResult.lDivideNormalRate / (double)100);

			if(lReduceDamage > 0)
			{
				stAttackResult.nDamage -= lReduceDamage;
				m_pagpmFactors->SetValue(pcsUpdateFactor, -(stAttackResult.nDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_NORMAL);

				// ���� ��ŭ�� �������� Caster ���� ����.
				m_pagsmCharacter->DecreaseHP(pcsCaster, lReduceDamage);

				// �߰�ȿ�� �ش�.
				m_pagsmCharacter->SendPacketCharUseEffect(pcsCaster, AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP);			
			}

			// ���� ��ŭ�� �������� Caster ���� ����.
			//ApplyAttackDamage(stAttackResult.pAttackChar, pcsCaster, lReduceDamage, lReduceDamage, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, 0, TRUE/*force*/, 0);
		}
	}

	// �Ӽ�������
	if(stAttackResult.lDivideAttrRate && stAttackResult.lDivideAttrCaster)
	{
		// Caster �� �־�߸� �۾��Ѵ�. ���� �ڱ� �ڽ��� �ƴϿ��� �Ѵ�.
		AgpdCharacter* pcsCaster = m_pagpmCharacter->GetCharacter(stAttackResult.lDivideAttrCaster);
		if(pcsCaster && !pcsCaster->m_bIsReadyRemove && pcsCaster->m_lID != stAttackResult.pTargetChar->m_lID)
		{
			AuAutoLock csLock(pcsCaster->m_Mutex);

			INT32 lReduceDamage = 0;
			INT32 lReduceDamageSum = 0;

			// �Ӽ� �������� �� ���ؿ´�.
			ApSafeArray<INT32, AGPD_FACTORS_ATTRIBUTE_MAX_TYPE>	lSpiritDamage;
			lSpiritDamage.MemSetAll();

			int j = 0;
			for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				m_pagpmFactors->GetValue(pcsUpdateFactor, &lSpiritDamage[i], AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC + j);
				lSpiritDamage[i] = -(lSpiritDamage[i]);	// ������ �Ǿ� �ִ� �� ����� �ϴ� �ٲ۴�.
				++j;
			}

			j = 0;
			for(int i = AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC; i <= AGPD_FACTORS_ATTRIBUTE_TYPE_ICE; ++i)
			{
				lReduceDamage = 0;
				lReduceDamage = (INT32)((double)lSpiritDamage[i] * (double)stAttackResult.lDivideAttrRate / (double)100);
				lReduceDamageSum += lReduceDamage;

				// �� �ٽ� �������ش�.
				if(lReduceDamage > 0)
					m_pagpmFactors->SetValue(pcsUpdateFactor, -(lSpiritDamage[i] - lReduceDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_DMG_ATTR_MAGIC + j);

				++j;
			}

			if(lReduceDamageSum > 0 && plTotalDamage && *plTotalDamage > lReduceDamageSum)
			{
				*plTotalDamage -= lReduceDamageSum;
				m_pagpmFactors->SetValue(pcsUpdateFactor, -(*plTotalDamage), AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

				// ���� ��ŭ�� �������� Caster ���� ����.
				m_pagsmCharacter->DecreaseHP(pcsCaster, lReduceDamageSum);

				// �߰�ȿ�� �ش�.
				m_pagsmCharacter->SendPacketCharUseEffect(pcsCaster, AGPDCHAR_ADDITIONAL_EFFECT_ABSORB_HP);
			}

			// ���� ��ŭ�� �������� Caster ���� ����.
			//ApplyAttackDamage(stAttackResult.pAttackChar, pcsCaster, lReduceDamageSum, lReduceDamageSum, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS, 0, TRUE/*force*/, 0);
		}
	}

	return stAttackResult.nDamage;
}

// 2007.07.10. steeple
// �Ӽ�����
BOOL AgsmCombat::IsAttrInvincible(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(m_pagpmCharacter->IsStatusAttrInvincible(pcsCharacter) == FALSE)
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	INT32 lRand = m_pagpmCombat->m_csRandom.randInt(100);
	if(pcsAgsdCharacter->m_stInvincibleInfo.lAttrProbability > lRand)
		return TRUE;

	return FALSE;
}

void AgsmCombat::KillMonster(int attackerCID, int monsterCID)
{
	AgpdCharacter *pAttacker = m_pagpmCharacter->GetCharacterLock(attackerCID);
	if (NULL == pAttacker)
		return;

	AgpdCharacter *pMonster = m_pagpmCharacter->GetCharacterLock(monsterCID);
	if (NULL == pMonster)
	{
		pAttacker->m_Mutex.Release();
		return;
	}

	INT32	lCurrentTargetHP	= 0;
	m_pagpmFactors->GetValue(&pMonster->m_csFactor, &lCurrentTargetHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	ApplyAttackDamage(pAttacker, pMonster, lCurrentTargetHP, lCurrentTargetHP, NULL, AGPDCHAR_ACTION_RESULT_TYPE_ATTACK_SUCCESS);

	if (pMonster)
		pMonster->m_Mutex.Release();
	if (pAttacker)
		pAttacker->m_Mutex.Release();
}