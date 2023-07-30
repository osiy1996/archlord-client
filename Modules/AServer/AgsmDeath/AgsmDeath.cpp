/******************************************************************************
Module:  AgsmDeath.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 09. 11
******************************************************************************/

#include "AgsmDeath.h"
#include "AgpmBattleGround.h"
#include "AgsmBattleGround.h"
#include "AgpmEpicZone.h"
#include "AgsmEpicZone.h"
#include "AgpmAI2.h"

const INT32		AGSMDEATH_EXP_LEVEL_DIFF = 12;

AgsmDeath::AgsmDeath()
{
	SetModuleName("AgsmDeath");

	//EnableIdle2(TRUE);

	SetModuleType(APMODULE_TYPE_SERVER);

	SetPacketType(AGSMDEATH_PACKET_TYPE);

	m_nIdleCount	= 0;

	m_csPacket.SetFlagLength(sizeof(INT8));
	m_csPacket.SetFieldType(
							AUTYPE_PACKET,			1,			// m_csPacketDeath
							AUTYPE_PACKET,			1,			// m_csPacketDeathADItem
							AUTYPE_END,				0
							);

	m_csPacketDeath.SetFlagLength(sizeof(INT8));
	m_csPacketDeath.SetFieldType(
							AUTYPE_INT32,			1,			// character id
							AUTYPE_UINT32,			1,			// m_ulDeadTime
							AUTYPE_INT64,			1,			// m_lLastLoseExp
							AUTYPE_PACKET,			1,			// m_csLastHitBase
							AUTYPE_UINT32,			1,			// current clock count
							AUTYPE_END,				0
							);

	m_csPacketDeathADItem.SetFlagLength(sizeof(INT8));
	m_csPacketDeathADItem.SetFieldType(
							AUTYPE_INT32,			1,			// item id
							AUTYPE_INT32,			1,			// m_lDropRate
							AUTYPE_END,				0
							);

	m_pagpmLog = NULL;
	m_pagpmPvP = NULL;
	m_pagpmBattleGround = NULL;
	m_pagsmBattleGround = NULL;

	//m_partyBonus = g_eServiceArea == AP_SERVICE_AREA_JAPAN ? 0.2 : 0.1;
#ifndef _AREA_CHINA_
	m_partyBonus = 0.2;
#else
	m_partyBonus = 0.1;
#endif
}

AgsmDeath::~AgsmDeath()
{
}

BOOL AgsmDeath::OnAddModule()
{
	m_papmMap			= (ApmMap *)		GetModule("ApmMap");
	m_pagpmGrid			= (AgpmGrid *)		GetModule("AgpmGrid");

	m_pagpmFactors		= (AgpmFactors *)	GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmUnion		= (AgpmUnion *)		GetModule("AgpmUnion");
	m_pagpmItem			= (AgpmItem *)		GetModule("AgpmItem");
	m_pagpmSkill		= (AgpmSkill *)		GetModule("AgpmSkill");
	m_pagpmItemConvert	= (AgpmItemConvert *)	GetModule("AgpmItemConvert");
	//m_pagpmShrine		= (AgpmShrine *)	GetModule("AgpmShrine");
	m_pagpmParty		= (AgpmParty *)		GetModule("AgpmParty");
	m_pagpmPvP			= (AgpmPvP *)		GetModule("AgpmPvP");
	m_pagpmSummons		= (AgpmSummons *)	GetModule("AgpmSummons");
	m_pagpmConfig		= (AgpmConfig *)	GetModule("AgpmConfig");

	m_papmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pagpmEventBinding	= (AgpmEventBinding *)	GetModule("AgpmEventBinding");
	m_pagpmLog = (AgpmLog *)	GetModule("AgpmLog");
	m_pagpmBillInfo		= (AgpmBillInfo *)	GetModule("AgpmBillInfo");

	m_pagsmAOIFilter	= (AgsmAOIFilter *) GetModule("AgsmAOIFilter");
	m_pagsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pagsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pagsmFactors		= (AgsmFactors *)	GetModule("AgsmFactors");
	m_pagsmCombat		= (AgsmCombat *)	GetModule("AgsmCombat");
	m_pagsmSummons		= (AgsmSummons *)	GetModule("AgsmSummons");
	m_pagsmSkill		= (AgsmSkill *)		GetModule("AgsmSkill");
	m_pagsmItem			= (AgsmItem *)		GetModule("AgsmItem");
	m_pagsmItemManager	= (AgsmItemManager *)	GetModule("AgsmItemManager");
	m_pagsmDropItem2	= (AgsmDropItem2 *)	GetModule("AgsmDropItem2");
	m_pagsmParty		= (AgsmParty *)		GetModule("AgsmParty");
	m_pcsAgsmUsedCharDataPool	= (AgsmUsedCharDataPool *)	GetModule("AgsmUsedCharDataPool");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *)	GetModule("AgpmEventSpawn");
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar *)	GetModule("AgpmSiegeWar");

	if (!m_papmMap ||
		!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmUnion ||
		!m_pagpmSkill ||
		//!m_pagpmShrine ||
		!m_pagsmAOIFilter ||
		!m_pagsmCharacter ||
		!m_pagsmFactors ||
		!m_pagsmCombat ||
		!m_pagsmSkill ||
		!m_pagpmItem ||
		!m_pagpmItemConvert ||
		!m_pagpmEventBinding ||
		!m_pagpmBillInfo ||
		!m_pagpmConfig ||
		!m_pcsAgsmUsedCharDataPool ||
		!m_pcsAgpmEventSpawn ||
		!m_pcsAgpmSiegeWar
		)
		return FALSE;

	if (!m_pagpmParty || !m_papmEventManager || !m_pagsmItemManager || !m_pagsmParty || !m_pagsmItem || !m_pagsmCharManager)
		return FALSE;

	// setting attached data(m_nKeeping) in character module
	m_nIndexADCharacter =
		m_pagpmCharacter->AttachCharacterData(this, sizeof(AgsdDeath), ConAgsdDeath, DesAgsdDeath);

	m_nIndexADItem = 
		m_pagpmItem->AttachItemData(this, sizeof(AgsdDeathADItem), NULL, NULL);

	if (m_nIndexADCharacter < 0 || m_nIndexADItem < 0)
		return FALSE;

	if (!m_pagsmCombat->SetCallbackAttack(CBAttackCharacter, this))
		return FALSE;

	if (!m_pagsmSkill->SetCallbackAttack(CBAttackCharacter, this))
		return FALSE;

	if (!m_pagpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackRequestResurrection(CBRequestResurrection, this))
		return FALSE;
	if (!m_pagpmCharacter->SetCallbackResurrectionByOther(CBResurrectionByOther, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllServerInfo, this))
		return FALSE;

	if (!m_pagsmCharManager->SetCallbackDisconnectCharacter(CBDisconnectCharacter, this))
		return FALSE;

	if (m_pagsmSummons)
	{
		if(!m_pagsmSummons->SetCallbackEndPeriodStart(CBEndPeriodStart, this))
			return FALSE;
	}

	return TRUE;
}

BOOL AgsmDeath::OnInit()
{
	m_pagpmBattleGround		= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagsmBattleGround		= (AgsmBattleGround*)GetModule("AgsmBattleGround");
	m_pagpmEpicZone			= (AgpmEpicZone*)GetModule("AgpmEpicZone");
	m_pagsmEpicZone			= (AgsmEpicZone*)GetModule("AgsmEpicZone");
	m_pagpmTitle			= (AgpmTitle*) GetModule("AgpmTitle");
	m_pagpmAI2				= (AgpmAI2*) GetModule("AgpmAI2");
	
	//if(!m_pagpmBattleGround || !m_pagsmBattleGround)
	//	return FALSE;
		
	return TRUE;
}

BOOL AgsmDeath::OnDestroy()
{
	return TRUE;
}

BOOL AgsmDeath::OnIdle2(UINT32 ulClockCount)
{
	PROFILE("AgsmDeath::OnIdle2");

	/*
	if (m_ulRecoveryPointTime > ulClockCount)
		return TRUE;

	m_ulRecoveryPointTime = ulClockCount + AGSMDEATH_RECOVERY_POINT_INTERVAL;
	*/

	return TRUE;
}

AgsdDeath* AgsmDeath::GetADCharacter(PVOID pData)
{
	return (AgsdDeath *) m_pagpmCharacter->GetAttachedModuleData(m_nIndexADCharacter, pData);
}

AgsdDeathADItem* AgsmDeath::GetADItem(PVOID pData)
{
	return (AgsdDeathADItem *) m_pagpmItem->GetAttachedModuleData(m_nIndexADItem, pData);
}

//		ProcessIdleDeath
//	Functions
//		- ������ ó���Ѵ�.
//	Arguments
//		- lCID : ó���� ĳ���� �Ƶ�
//		- pClass : this module pointer
//		- ulClockCount : ���� system clock count
//	Return value
//		- none
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ProcessIdleDeath(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	if (!pClass || lCID == AP_INVALID_CID)
		return FALSE;

	AgsmDeath *pThis = (AgsmDeath *) pClass;

	AgpdCharacter*	pCharacter = pThis->m_pagpmCharacter->GetCharacterLock(lCID);
	if (pCharacter == NULL)
		return FALSE;

	BOOL	bIsRemoved	= FALSE;

	pThis->ProcessIdleDeath(pCharacter, &bIsRemoved);

	if (!bIsRemoved)
		pCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmDeath::ProcessIdleDeath(AgpdCharacter *pCharacter, BOOL *pbIsRemoved)
{
	PROFILE("AgsmDeath::ProcessIdleDeath");

	if (!pCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessIdleDeatch"));

	switch (pCharacter->m_unActionStatus) {
	case AGPDCHAR_STATUS_NORMAL:
		// ������� �ϰ͵� ���Ѵ�
		break;

	case AGPDCHAR_STATUS_PREDEAD:
		{
			// AGSDCHAR_FLAG_PREDEAD ����
			if (m_pagsmFactors->IsDead(&pCharacter->m_csFactor))
			{
				AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pCharacter);
				if (pcsAgsdCharacter->m_bResurrectWhenDie && pcsAgsdCharacter->m_lResurrectHP > 0)
				{
					// ���� �������� ��ų�̳� �� ��Ÿ ����� ������ �ٽ� ���ڸ����� ��Ƴ��� ����̴�.
					ResurrectNow(pCharacter);
				}
				else if (!SetDead(pCharacter))
				{
					return FALSE;
				}
			}
			else
			{
				// �̳� ��ҳ�... RoundTripLatency ������ ���� ������ �Ծ����� ��ư... HP�� ȸ���ߴ�. �ٽ� ����ش�.
				if (!NotDead(pCharacter))
				{
					return FALSE;
				}
			}
		}

		break;

	case AGPDCHAR_STATUS_DEAD:
		// AGSDCHAR_FLAG_DEAD ����
		// ���� ��� ��Ƴ��� ���� ���ǿ� �����ϸ� �������.

		BOOL	bDestroyed;
		if (!Resurrection(pCharacter, &bDestroyed))
		{
			return FALSE;
		}

		if (bDestroyed && pbIsRemoved)
		{
			*pbIsRemoved = TRUE;
		}

		break;
	};

	return TRUE;
}

BOOL AgsmDeath::ProcessDeathResult(INT32 lCID, PVOID pClass, UINT32 ulClockCount, PVOID pvData)
{
	PROFILE("AgsmDeath::ProcessDeathResult");

	AgsmDeath		*pThis				= (AgsmDeath *)	pClass;

	AgpdCharacter	*pcsCharacter		= pThis->m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return TRUE;

	AgsdDeath		*pcsAgsdDeath		= pThis->GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	if (!pcsAgsdDeath->m_bIsNeedProcessDeath)
	{
		pcsCharacter->m_Mutex.Release();
		return TRUE;
	}

	/*
	BOOL	bAddIdleEvent	= TRUE;

	if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		bAddIdleEvent = FALSE;

	pThis->Dead(pcsCharacter, bAddIdleEvent);
	*/

	pThis->Dead(pcsCharacter, TRUE);

	pcsAgsdDeath->m_bIsNeedProcessDeath	= FALSE;

	pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmDeath::SetDead(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("SetDead"));

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
		return FALSE;
		
	// ���������� ��Ÿ�� ����ȭ - arycoat 2008.05.19
	m_pagsmItem->SendPacketUpdateReverseOrbReuseTime(pcsCharacter, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// �ױ� ���� ����ϴ� ��ų ���ش�. 2007.07.10. steeple
	m_pagsmSkill->ProcessActionOnActionType6(pcsCharacter);

	pcsAgsdDeath->m_ulDeadTime = GetClockCount();

	pcsCharacter->m_stNextAction.m_eActionType	= AGPDCHAR_ACTION_TYPE_NONE;

	m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);

	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_DEAD);

	m_pagsmSkill->EndAllBuffedSkillExceptTitleSkill((ApBase *) pcsCharacter, TRUE);

	// 2005.07.07. steeple
	// �ɷ��ִ� ��� SpecialStatus �� Ǯ���ش�.
	m_pagsmCharacter->CheckSpecialStatusIdleTime(pcsCharacter, 0xFFFFFFFF);		// ������ �ð��� ũ�� ������� ������ �ع�����.

	AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (pcsAgsdCharacter)
		pcsAgsdCharacter->m_DetailInfo.bRemoveByDead = true;

	// ��ȯ�� ��ȯ�� ���¶�� �� ���ֻ�����. 2005.10.10. steeple
	if(m_pagsmSummons)
	{
		if(m_pagpmCharacter->IsPC(pcsCharacter))
		{
			m_pagsmSummons->RemoveAllSummons(pcsCharacter, 
							AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

			// ���� �Ŀ��� Max Summons ���� �ʱ�ȭ �����ش�.
			m_pagpmSummons->SetMaxSummonsCount(pcsCharacter, 1);
		}
	}

	// 2007.07.25. steeple
	// PC ��� Idle ���� ó���� �ƴ϶� ��� ó�����ش�.
	if(m_pagpmCharacter->IsPC(pcsCharacter) && _tcslen(pcsCharacter->m_szID) > 0)
	{
		Dead(pcsCharacter, TRUE);
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
	}
	else if (pcsCharacter->m_pcsCharacterTemplate->m_ulCharType & AGPMCHAR_TYPE_SUMMON)
	{
		Dead(pcsCharacter, TRUE);
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_HALF_SECOND);
	}
	else
	{
		pcsAgsdDeath->m_bIsNeedProcessDeath = TRUE;
		m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
	}

	return TRUE;
}

//		Dead
//	Functions
//		- �̳� ������ �׾���. ó���� �Ѵ�.
//			1. ���� ������ �ֺ� �ѵ鿡�� ������.
//			2. � ������ �׾����� ���� �׿� �ش��ϴ� ó���� �Ѵ�.
//			3. ��Ƴ� �ð��� OnIdle event�� ����Ѵ�. 
//	Arguments
//		- pCharacter : ���� ��
//	Return value
//		- none
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::Dead(AgpdCharacter *pCharacter, BOOL bAddIdleEvent)
{
	PROFILE("AgsmDeath::Dead");

	if (!pCharacter)
		return FALSE;

	AgsdDeath *		pcsDeath = GetADCharacter(pCharacter);
	if (!pcsDeath)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("Dead"));

	EnumCallback(AGSMDEATH_CB_PRE_PROCESS_CHARACTER_DEAD, pCharacter, NULL);

	pcsDeath->m_bIsNeedProcessDeath	= FALSE;

	// �����ѿ� ���� Exp, Penalty ���, PK, UvU ������ �� ó���Ѵ�.
	if (!ProcessCombatResult(pCharacter))
		return FALSE;

	INT32	lMaxHP	= 0;
	m_pagpmFactors->GetValue(&pCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	if (lMaxHP < 1)
		return FALSE;

	ApBase	*pcsMostAttackerBase	= NULL;
	ApBase	*pcsRealAttackerBase	= NULL;
	AgsdCharacterHistoryEntry *pcsEntry = m_pagsmCharacter->GetMostDamager(pCharacter);
	if (pcsEntry)
	{
		pcsMostAttackerBase	= m_papmEventManager->GetBase(pcsEntry->m_csSource.m_eType, pcsEntry->m_csSource.m_lID);

		// 2005.01.06. steeple
		// pcsMostAttackerBase �� Lock �� �Ǿ� �ֱ淡 Lock �� ����
		if(pcsMostAttackerBase)
		{
			if (!pcsMostAttackerBase->m_Mutex.WLock())
				pcsMostAttackerBase = NULL;

			if(pcsMostAttackerBase && pcsMostAttackerBase->m_eType == APBASE_TYPE_CHARACTER)
			{
				AgpdCharacter *pcsAttacker = m_pagpmCharacter->GetCharacter(pcsMostAttackerBase->m_lID);

				if(pcsAttacker && (m_pagpmCharacter->IsStatusTame(pcsAttacker) || m_pagpmCharacter->IsStatusSummoner(pcsAttacker)))
				{
					INT32 lOwnerCID = m_pagpmSummons->GetOwnerCID(pcsAttacker);
					if(0 != lOwnerCID)
						pcsRealAttackerBase = (ApBase*) m_pagpmCharacter->GetCharacterLock(lOwnerCID);
				}
			}

			if(!pcsRealAttackerBase)
				pcsRealAttackerBase = pcsMostAttackerBase;
		}
	}
	
	if(pcsEntry)
	{
		AgpdCharacter* pcsMostDamager = NULL;

		// ĳ���Ͷ��
		if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
		else if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_PARTY)
		{
			pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pCharacter, pcsEntry->m_csSource.m_lID);
			if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
				pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
	
		if(m_pagsmBattleGround && m_pagpmBattleGround->IsInBattleGround(pCharacter))
			m_pagsmBattleGround->OnDead(pcsMostDamager, pCharacter);

		if(m_pagsmEpicZone && m_pagpmEpicZone->IsInEpicZone(pCharacter))
			m_pagsmEpicZone->OnDeadProcess(pCharacter, pcsMostDamager);
	}

	// 2005.07.28. steeple
	// �������� ��� �׾��� ���� �ʱ�ȭ �Ѵ�.
	// �Ʒ� �ݹ� �� AgsmPvP ���� ó���ȴ�.
	pcsDeath->m_lDeadTargetCID = 0;
	pcsDeath->m_cDeadType = AGPMPVP_TARGET_TYPE_MOB;

	EnumCallback(AGSMDEATH_CB_CHARACTER_DEAD, pCharacter, pcsRealAttackerBase);
	
	m_pagsmDropItem2->DropItem( pCharacter, pcsRealAttackerBase );

	// 2005.01.06. steeple
	// pcsRealAttackerBase �� Lock �� Ǯ����.

	if(pcsMostAttackerBase && pcsRealAttackerBase != pcsMostAttackerBase)
		pcsMostAttackerBase->m_Mutex.Release();

	if(pcsRealAttackerBase)
		pcsRealAttackerBase->m_Mutex.Release();

	AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pCharacter);
	if (pcsHistory)
	{
		for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
		{
			if (pcsHistory->m_astEntry[i].m_lPartyID != AP_INVALID_PARTYID)
				continue;

			if (pcsHistory->m_astEntry[i].m_csSource.m_eType == APBASE_TYPE_CHARACTER)
			{
				AgpdCharacter	*pcsHistoryTarget	= m_pagpmCharacter->GetCharacterLock(pcsHistory->m_astEntry[i].m_csSource.m_lID);
				if (pcsHistoryTarget)
				{
					if (m_pagsmCharacter->RemoveHistory(pcsHistoryTarget, (ApBase *) pCharacter))
						--i;

					pcsHistoryTarget->m_Mutex.Release();
				}
			}
		}
	}

	m_pagsmCharacter->InitHistory(pCharacter);

	// 2005.04.27. steeple
	m_pagsmCharacter->InitPartyHistory(pCharacter);

	// 2005.11.29. steeple
	m_pagsmCharacter->InitTargetInfoArray(pCharacter);


	// player character �� ��츸 �ٽ� �츰��.
	//if (((AgpdCharacterTemplate *) pCharacter->m_pcsCharacterTemplate)->m_lID < 7 || 
	//	((AgpdCharacterTemplate *) pCharacter->m_pcsCharacterTemplate)->m_lID > 13)
	//{
	/*
		if (bAddIdleEvent && !RegisterDeath(pCharacter))
			return FALSE;
	*/
	//}

	return TRUE;
}

//		CheckKiller
//	Functions
//		- pCharacter �� ���� pcsAttack�� ������� �˻��Ѵ�.
//			AGSMDEATH_KILLER_PK		: PK ���ߴ�.
//			AGSMDEATH_KILLER_NPC	: ���ȸ���. ���� ���ϰڴ�.
//			AGSMDEATH_KILLER_UNION	: ����� ���Ͽ����� �¾� �׾���.
//	Arguments
//		- pCharacter : ���� ��
//		- pcsAttacker : pCharacter�� ���� ��
//	Return value
//		- killer type
///////////////////////////////////////////////////////////////////////////////
INT32 AgsmDeath::CheckKiller(AgpdCharacter *pCharacter, ApBase *pcsAttacker)
{
	if (!pCharacter || !pcsAttacker)
		return (-1);

	AgpdCharacter	*pcsAttackCharacter = NULL;

	if (pcsAttacker->m_eType == APBASE_TYPE_CHARACTER)
	{
		pcsAttackCharacter = (AgpdCharacter *) pcsAttacker;
	}
	else if (pcsAttacker->m_eType == APBASE_TYPE_PARTY)
	{
		pcsAttackCharacter = m_pagpmCharacter->GetCharacter(((AgpdParty *) pcsAttacker)->m_lMemberListID[0]);
	}

	if (!pcsAttackCharacter)
		return (-1);

	// npc ���� �׾���.
	if (m_pagsmCharacter->GetCharDPNID(pcsAttackCharacter) == 0)
		return AGSMDEATH_KILLER_NPC;

	AgsdDeath	*pcsAgsdDeath = GetADCharacter((PVOID) pCharacter);
	if (!pcsAgsdDeath)
		return (-1);

	INT32	lKilledUnion = m_pagpmCharacter->GetUnion(pCharacter);

	INT32	lAttackerUnion = m_pagpmCharacter->GetUnion(pcsAttackCharacter);

	if (lKilledUnion == lAttackerUnion)
		return AGSMDEATH_KILLER_PK;
	else
		return AGSMDEATH_KILLER_UNION;

	return (-1);
}

BOOL AgsmDeath::ProcessCombatResult(AgpdCharacter *pCharacter)
{
	PROFILE("AgsmDeath::ProcessCombatResult");

	if (!pCharacter)
		return FALSE;

	if (!m_pagpmCharacter->IsMonster(pCharacter))
		return TRUE;

	// 2005.10.26. steeple
    // ��ȯ��, ���̹� �� �׾��� ���� �� ������.
	if(m_pagpmCharacter->IsStatusSummoner(pCharacter) ||
		m_pagpmCharacter->IsStatusTame(pCharacter) ||
		m_pagpmCharacter->IsStatusFixed(pCharacter))
		return TRUE;

	AgpdCharacter* pcsMostDamager = NULL;
	AgsdCharacterHistoryEntry* pcsEntry = m_pagsmCharacter->GetMostDamager(pCharacter);
	if(pcsEntry)
	{
		// ĳ���Ͷ��
		if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
		{
			pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
		else if(pcsEntry->m_csSource.m_eType == APBASE_TYPE_PARTY)
		{
			pcsEntry = m_pagsmCharacter->GetMostDamagerInParty(pCharacter, pcsEntry->m_csSource.m_lID);
			if(pcsEntry && pcsEntry->m_csSource.m_eType == APBASE_TYPE_CHARACTER)
				pcsMostDamager = m_pagpmCharacter->GetCharacter(pcsEntry->m_csSource.m_lID);
		}
	}

	// �������� ������ ����̴�.
	// �� ��쿣 �׻� PlayerVsMonster�̴�

	INT32	lTotalExp	= GetMonsterExp(pCharacter, pcsMostDamager);
	if (lTotalExp < 1)
		return FALSE;

	AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pCharacter);
	if (!pcsHistory)
		return FALSE;
		
	for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
	{
		if (pcsHistory->m_astEntry[i].m_lPartyID != AP_INVALID_PARTYID)
			continue;

		// exp ����Ѵ�.
		GetBonusExp(lTotalExp, pCharacter, &pcsHistory->m_astEntry[i]);
	}

	pcsHistory = m_pagsmCharacter->GetPartyHistory(pCharacter);
	if (pcsHistory)
	{
		for (int i = 0; i < pcsHistory->m_lEntryNum; i++)
		{
			// exp ����Ѵ�.
			GetBonusExpToParty(lTotalExp, pCharacter, &pcsHistory->m_astEntry[i]);
		}
	}

	return TRUE;
}

//		RegisterDeath
//	Functions
//		- ĳ���Ͱ� �׾���. ������ �ð��Ŀ� ��Ȱ ��ҿ��� ��Ƴ����� OnIdle Event�� ����Ѵ�.
//			ĳ���Ͱ� �׾��ٴ� ������ �ֺ��� ������.
//	Arguments
//		- pCharacter : ���� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::RegisterDeath(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// idle event�� ����Ѵ�.
	AgsdDeath	*pcsDeath = GetADCharacter(pCharacter);

	return AddIdleEvent2(pcsDeath->m_ulDeadTime + AGSMDEATH_RES_INTERVAL * 1000, pCharacter->m_lID, this, ProcessIdleDeath, NULL);
}

//		NotDead
//	Functions
//		- PreDead ���¿��� �����ʰ� ��Ƴ���. �ٽ� ���󺹱� �����ش�.
//	Arguments
//		- pCharacter : ��Ƴ� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::NotDead(AgpdCharacter *pCharacter)
{
	if (!pCharacter)
		return FALSE;

	// ĳ������ ���¸� ���� ���·� �ٲ۴�.
	m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	return TRUE;
}

// ProcessPinchCharacterDeath
// Functions
//		- Pinch Character�� ������ �����Ѵ�.
// Arguments
//		- pcsCharacter : ���� ĳ����
// Return value
//		- BOOL :��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ProcessPinchCharacterDeath(AgpdCharacter *pAttackChar, AgpdCharacter *pTargetChar)
{
	if(NULL == pTargetChar || NULL == pAttackChar)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pagpmAI2->GetCharacterData(pTargetChar);
	if(pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType == AGPMAI2_TYPE_PINCH_WANTED)
		{
			if(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter)
			{
				AgpdAI2ADChar *pcsAgpdAI2PinchChar = m_pagpmAI2->GetCharacterData(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter);
				if(pcsAgpdAI2PinchChar)
				{
					if(pcsAgpdAI2PinchChar->m_csPinchInfo.ePinchType == AGPMAI2_TYPE_PINCH_MONSTER)
					{
						vector<AgpdCharacter*>::iterator it = pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.begin(); 
						while(it != pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.end())
						{
							AgpdCharacter *pcsListCharacter = *it;
							if(pcsListCharacter)
							{
								if(pcsListCharacter->m_lID == pTargetChar->m_lID)
								{
									it = pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.erase(it);
									if(pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.empty() == TRUE)
									{
										pcsAgpdAI2PinchChar->m_csPinchInfo.pRequestMobVector.clear();
										pcsAgpdAI2PinchChar->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_NONE;

										// ������ HP�� 0�� ���� ���δ�.
										m_pagpmFactors->SetValue(&pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter->m_csFactor, 0.0f, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
										PrepareDeath(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter, pAttackChar);
									}
									break;
								}
							}

							it++;
						}

						pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_NONE;
						pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter = NULL;
					}					
				}
			}
		}
	}

	return TRUE;
}

//		Resurrection
//	Functions
//		- ���� ĳ���� ��Ȱ��Ų��.
//	Arguments
//		- pCharacter : ���� ĳ����
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::Resurrection(AgpdCharacter *pCharacter, BOOL *pbDestroyed)
{
	*pbDestroyed = FALSE;

	if (pCharacter == NULL)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("Resurrection"));

	// ���� ������ ��� ��ų�� �����Ų��.
	//m_pagsmSkill->EndAllBuffedSkill((ApBase *) pCharacter);

	/*
	// Criminal Flag�� ���õǾ� �ִٸ� �����ش�.
	if (pCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
	{
		m_pagpmCharacter->UpdateCriminalStatus(pCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
	}

	// �����Ǿ� �ִ� ��ġ���� ��Ȱ��Ų��.
	// ��ġ����, ĳ���� Factor ����� ����Ÿ�� ������Ʈ �Ѵ�.
	//
	PVOID pvPacketFactor;
	*/

	if (m_pagsmCharacter->GetADCharacter(pCharacter)->m_bDestroyWhenDie)
	{
		INT32	lCID	= pCharacter->m_lID;

		//if (m_pagpmCharacter->IsNPC(pCharacter) || pCharacter->m_bIsProtectedNPC)
		//	AuLogFile("RemoveNPC.log", "Removed by Resurrection()\n");

		pCharacter->m_Mutex.Release();

#ifndef	_USE_CHARDATAPOOL_
		m_pagpmCharacter->RemoveCharacter(lCID);
#else
		if (m_pagpmCharacter->IsPC(pCharacter) ||
			m_pagpmCharacter->IsStatusSummoner(pCharacter) || 
			m_pagpmCharacter->IsStatusFixed(pCharacter) ||
			m_pagpmCharacter->IsStatusTame(pCharacter) ||
			m_pagpmCharacter->m_csMemoryPool.GetRemainCount() < 500)
            m_pagpmCharacter->RemoveCharacter(lCID);
		else
		{
			if (pCharacter->m_Mutex.WLock())
			{
				if (m_pcsAgsmUsedCharDataPool->RemoveWorld(pCharacter))
					m_pcsAgsmUsedCharDataPool->PushUsedData(pCharacter);

				pCharacter->m_Mutex.Release();
			}
		}
#endif

		*pbDestroyed = TRUE;

		return TRUE;
	}

	/*
	if (!m_pagsmFactors->Resurrection(&pCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	// �ٽ� ��Ƴ��ٴ� ��Ŷ�� ������.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pCharacter);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pCharacter);
	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	// �׽�Ʈ�� ���� ������ġ�� ��Ȱ ��ġ�� ������ ���´�.
	AuPOS	stResPos;
	stResPos.x			= -353236;
	stResPos.y			= 6946.60;
	stResPos.z			= 107239;
	*/

	/*
	AuPOS	stResPos;

	if (!m_pagpmEventBinding->GetBindingPositionForResurrection(pCharacter, &stResPos))
	{
		// �̰� ������ ���� ���� �Ͼ�� �ȵǴ� ����̳�.. Ȥ�� �Ͼ�ԵǸ� �ƹ����γ� �ڷ���Ʈ ��ų �� ������
		// �� ���� ��ġ���� ��Ȱ��Ų��.
		stResPos	= pCharacter->m_stPos;
	}
	*/

	/*
	// update position
	m_pagpmCharacter->UpdatePosition(pCharacter, &stResPos, FALSE, TRUE);

	return bSendResult;
	*/

	return TRUE;
}

BOOL AgsmDeath::ResurrectNow(AgpdCharacter *pcsCharacter, BOOL bIsHPFull, BOOL bSetDead)
{
	if (!pcsCharacter)
		return FALSE;

	AgsdCharacter	*pcsAgsdCharacter	= m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ResurrectNow"));

	if (bSetDead)
	{
		// ��Ȱ��Ű�� ���� �ϴ� ���δ�. �����δ� �� ��Ŷ�� ������ Ŭ���̾�Ʈ���� �ִϸ��̼� ���� �ϰ� �Ѵ�.
		/////////////////////////////////////////////////////////////////////////////////////////////////
		m_pagpmCharacter->StopCharacter(pcsCharacter, NULL);
		m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_DEAD);
	}

	// ��Ȱ��Ų��. �ٷ� ���Ѿ� �ϳ�.. �ƴϸ� Delay�� ��� �ϳ��� ��ȹ���� ������ ���� �׶� ó���Ѵ�.
	/////////////////////////////////////////////////////////////////////////////////////////////////

	// HP�� lResurrectHP��ŭ ȸ�������ش�.
	PVOID	pvPacketFactor	= NULL;

	if (bIsHPFull)
	{
		if (!m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
			return FALSE;
	}
	else
		m_pagsmFactors->UpdateCharPoint(&pcsCharacter->m_csFactor, &pvPacketFactor, pcsAgsdCharacter->m_lResurrectHP, 0, 0);

	// factor ��Ŷ�� ������.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);
	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// ĳ������ ���¸� ���� ���·� �ٲ۴�.
	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);
	
	// ������ �ʱ�ȭ�Ѵ�.
	pcsAgsdCharacter->m_bResurrectWhenDie	= FALSE;
	pcsAgsdCharacter->m_lResurrectHP	= 0;

	AgsdDeath	*pcsAgsdDeath	= GetADCharacter(pcsCharacter);
	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	return bSendResult;
}

// 2007.09.06. steeple
BOOL AgsmDeath::ResurrectionNowNoPenalty(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	if(pcsCharacter->IsDead() == FALSE)
		return TRUE;

	if(m_pagpmCharacter->IsAllBlockStatus(pcsCharacter))
		return TRUE;

	BOOL bIsTransform = pcsCharacter->m_bIsTrasform;

	// �������̸� ������ �ǵ�����.
	if(pcsCharacter->m_bIsTrasform)
		m_pagpmCharacter->RestoreTransformCharacter(pcsCharacter);

	// �����Ǿ� �ִ� ��ġ���� ��Ȱ��Ų��.
	// ��ġ����, ĳ���� Factor ����� ����Ÿ�� ������Ʈ �Ѵ�.
	PVOID pvPacketFactor	= NULL;
	if(!m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	// �ٽ� ��Ƴ��ٴ� ��Ŷ�� ������.
	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_3);

	if(pvPacketFactor)
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	m_pagpmCharacter->UpdateActionStatus(pcsCharacter, AGPDCHAR_STATUS_NORMAL);

	AgsdDeath* pcsAgsdDeath = GetADCharacter(pcsCharacter);
	if(pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem = FALSE;

	AuPOS stResPos = pcsCharacter->m_stPos;

	// 2007.07.25. steeple
	// DB �����ϱ� ���� UpdatePosition ���� �Ѵ�.
	//
	// update position
	EnumCallback(AGSMDEATH_CB_RESURRECTION, pcsCharacter, &stResPos);

	// 2005.12.16. steeple
	// ���� ��������� üũ �Ǿ� �ִ� ��ų ��ũ�� ĳ�� �������� �������ش�.
	m_pagsmItem->UseAllEnableCashItem(pcsCharacter,
										0,
										AGSDITEM_PAUSE_REASON_NONE,
										bIsTransform);	// ��Ȱ �� ���°� ���� ���̾��ٸ�, '�Ͻ� ����' ������ �����۵� ������ش�.
	m_pagsmSkill->RecastSaveSkill(pcsCharacter);
	// ���� ĳ���� ����Ÿ�� ��� DB�� �����Ѵ�.
	m_pagsmCharacter->BackupCharacterData(pcsCharacter);
	m_pagsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pcsCharacter, NULL);

	return bSendResult;
}

//		AddBonusExpToChar
//	Functions
//		- Bonus Exp�� �ݿ��Ѵ�. ��, ���Ϳ� �ݿ��Ѵ�.
//	Arguments
//		- lCID : ���ʽ��� ���� ĳ����
//		- nBonusExp : ���ʽ� Exp
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::AddBonusExpToChar(INT32 lCID, INT64 llBonusExp, BOOL bIsLock, BOOL bIsPCBangBonus, BOOL bIsQuest)
{
	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
	if (!pcsCharacter)
		return FALSE;

	BOOL	bRetval = AddBonusExpToChar(pcsCharacter, NULL, llBonusExp, TRUE, bIsPCBangBonus, bIsQuest);

	pcsCharacter->m_Mutex.Release();

	return bRetval;
}

BOOL AgsmDeath::AddBonusExpToChar(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsKilledCharacter, INT64 llBonusExp, BOOL bIsLock, BOOL bIsPCBangBonus, BOOL bIsQuest)
{
	PROFILE("AgsmDeath::AddBonusExpToChar");

	if (!pcsCharacter || llBonusExp == 0)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AddBonusExpToChar"));

	/*
	AgpdBillInfo	*pcsBillInfo	= m_pagpmBillInfo->GetADCharacter(pcsCharacter);

	// pc���ΰ�� ���ʽ��� 10%�� ���ش�.
	// �̺�Ʈ�� ������ ��������.
	if (bIsPCBangBonus &&
		pcsBillInfo->m_bIsPCRoom)
	{
		nBonusExp = (INT32)	(nBonusExp * 1.10);
	}
	*/

	INT32 lTitleBonusExp = 0;

	if(pcsKilledCharacter)
	{

		INT32 nEffectSet = 0;
		INT32 nEffectValue1 = 0;
		INT32 nEffectValue2 = 0;

		m_pagpmTitle->GetEffectValue(pcsCharacter, AGPMTITLE_TITLE_EFFECT_HUNT_BONUS_EXP, &nEffectSet, &nEffectValue1, &nEffectValue2);

		if(nEffectValue1 == pcsKilledCharacter->m_pcsCharacterTemplate->m_lID && nEffectSet == 0)
			lTitleBonusExp = (INT32)((FLOAT)llBonusExp * (FLOAT)(nEffectValue2) / 100.0f);
	}

	// 2005.12.02. steeple
	// ����ġ ������ ����������� ����ϰ� �ִٸ� �������� ���ؼ� �����ش�.
	if(TRUE && !bIsQuest)	// �ش� �������� ��������� �ƴ��� üũ�Ѵ�.
	{
		INT64 llBonusExpByCash = m_pagpmCharacter->GetGameBonusExp(pcsCharacter);

		AgsdCharacter* pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pcsCharacter);
		if(pcsAgsdCharacter)
			llBonusExpByCash += pcsAgsdCharacter->m_stOptionSkillData.m_lBonusExp;
#ifndef _AREA_CHINA_
		INT32 ulPCRoomType = 0;
		m_pagpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsCharacter, &ulPCRoomType);

		if((ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK) || (ulPCRoomType & AGPDPCROOMTYPE_WEBZEN_GPREMIUM)) 
			llBonusExpByCash += m_pagpmConfig->GetTPackExpRatio();
#endif
		
		llBonusExp = llBonusExp + (INT32)((FLOAT)llBonusExp * (FLOAT)(llBonusExpByCash) / 100.0f);
	}

	if ( AP_SERVICE_AREA_CHINA == g_eServiceArea )
	{
		if ( pcsCharacter->m_lAddictStatus == 1 )
			llBonusExp = llBonusExp / 2;

		if ( pcsCharacter->m_lAddictStatus >= 2 )
			llBonusExp = 0;
	}

	llBonusExp += lTitleBonusExp;

	INT64	llOriginalBonusExp	= llBonusExp;
	BOOL	bLevelUp		= FALSE;

	// ������ ���� �� �ִ��� ����.
	INT32	lCharacterLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	BOOL	bSkipLevelUp = FALSE;

	if(llBonusExp > 0)
	{
		switch( m_pagpmCharacter->GetCharacterLevelLimit( pcsCharacter ) )
		{
		case	AgpmCharacter::LLS_NONE	:	// �� ����.. ��� ��Ȳ
		case	AgpmCharacter::LLS_LIMITED	:	// ���� ���Ѱɸ�.. �ɸ� �������� ����. ����ġ&������ ����ȹ��
			break;
		case	AgpmCharacter::LLS_BLOCKED	:	// ���� ���� �ɸ�.. ����ġ�� ������ ������ ���ö�.
			bSkipLevelUp = TRUE;
			break;
		case	AgpmCharacter::LLS_DOWNED	:	// ���� ���Ѱɸ� .. ������ �ٿ�Ǿ� ����.
			return FALSE;	// �������� �ɷ��� ����ġ ����
		}
	}

	// ������ �ö����� ����.
	INT64	llCharacterExp	= m_pagpmCharacter->GetExp(pcsCharacter);

	INT32	lNumLevelUp		= 0;

	if (llBonusExp > 0)
	{
		while (1)
		{
			INT64	llLevelUpExp		= m_pagpmCharacter->GetLevelUpExp(lCharacterLevel);

			if (llLevelUpExp <= llCharacterExp + llBonusExp)
			{
				if( bSkipLevelUp )
				{
					// ������ ����ġ�� 1�� ���ڶ��.
					llBonusExp = llLevelUpExp - llCharacterExp - 1;
					break;
				}

				if (lCharacterLevel >= (AGPMCHAR_MAX_LEVEL - 1))
				{
					// �������̴� ���⼭ �� �ö󰥵��� ����. ��.��...........
					llBonusExp = llLevelUpExp - llCharacterExp;
					if (llBonusExp < 0)
						return FALSE;

					break;
				}
				else
				{
					// ���� �� �� ����̴�.
					llBonusExp = llCharacterExp + llBonusExp - llLevelUpExp;
					++lCharacterLevel;
					++lNumLevelUp;
					llCharacterExp	= 0;

					llBonusExp	= 0;

					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		if (llCharacterExp + llBonusExp < 0)
			llBonusExp = (-llCharacterExp);
		/*
		while (1)
		{
			if (lCharacterExp + nBonusExp < 0)
			{
				if (lCharacterLevel == 1)
				{
					nBonusExp = (-lCharacterExp);
					break;
				}

				nBonusExp += lCharacterExp;
				--lCharacterLevel;
				--lNumLevelUp;
				lCharacterExp	= m_pagpmCharacter->GetLevelUpExp(lCharacterLevel);
			}
			else
			{
				break;
			}
		}
		*/
	}

	AgpdFactor		csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	AgpdFactor	*pcsUpdateResultFactor = (AgpdFactor *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	AgpdFactorCharPoint	*pcsFactorCharPoint = NULL;

	if (lNumLevelUp > 0)
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitUpdateFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	else
		pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitCalcFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);

	if (!pcsFactorCharPoint)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);
		return FALSE;
	}

	// UpdateFactor�� ���ʽ� Exp�� �����Ѵ�.

	PVOID	pvPacketFactor	= NULL;

	FLOAT	fOriginalBonusExp	= (FLOAT) llOriginalBonusExp;

	//if (bIsPCBangBonus)
	//	fOriginalBonusExp	+= 0.5f;

	// ���Ϳ� �ݿ��ϰ� ����� ���� ���� ���͸� �޴´�.
	if (lNumLevelUp != 0)
	{
		// UpdateFactor�� ���ʽ� Exp�� �����Ѵ�.
		m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), llBonusExp);
		m_pagpmFactors->SetValue(&csUpdateFactor, fOriginalBonusExp, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

		pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, &csUpdateFactor);
	}
	else
	{
		// UpdateFactor�� ���ʽ� Exp�� �����Ѵ�.
		m_pagpmFactors->SetExp((AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT), llBonusExp);
		m_pagpmFactors->SetValue(&csUpdateFactor, fOriginalBonusExp, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

		pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, &csUpdateFactor, TRUE, TRUE, TRUE, FALSE);
	}

	m_pagpmFactors->SetValue(&pcsCharacter->m_csFactor, 0, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_BONUS_EXP);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	if (!pvPacketFactor)
		return FALSE;

	BOOL bSendResult = m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pcsCharacter, PACKET_PRIORITY_4);

	m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	if (lNumLevelUp != 0)
		m_pagpmCharacter->UpdateLevel(pcsCharacter, lNumLevelUp);

	//else
	//	m_pagpmSkill->AdjustSkillPoint(pcsCharacter);

	m_pagsmCharacter->WriteExpLog(pcsCharacter, llBonusExp, bIsQuest); // arycoat 2010.07.12

	// 2004.05.18. steeple
	if(lNumLevelUp != 0)
	{
		m_pagsmCharacter->WritePlayLog(pcsCharacter, lNumLevelUp);
	}

	return bSendResult;
}

//		AddBonusExpToParty
//	Functions
//		- Bonus Exp�� �ݿ��Ѵ�. ��, ���Ϳ� �ݿ��Ѵ�.
//	Arguments
//		- lPartyID : ���ʽ��� ���� ��Ƽ
//		- nBonusExp : ���ʽ� Exp
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::AddBonusExpToParty(AgpdParty *pcsParty, INT64 llBonusExp, INT32 lMemberTotalLevel, INT32 lNumCombatMember, AgpdCharacter **pacsCombatMember)
{
	PROFILE("AgsmDeath::AddBonusExpToParty");

	if (!pcsParty ||
		llBonusExp < 1 ||
		lMemberTotalLevel < 1 ||
		lNumCombatMember < 1 ||
		!pacsCombatMember)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("AddBonusExpToParty"));

	for (int i = 0; i < lNumCombatMember; i++)
	{
		if (!pacsCombatMember[i])
			continue;

		if (pacsCombatMember[i]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			continue;

		INT32	lMemberLevel	= m_pagpmCharacter->GetLevel(pacsCombatMember[i]);

		INT64	llExp			= (INT64) (llBonusExp * (lMemberLevel / (float) lMemberTotalLevel));
		if (llExp < 1)
			continue;

		if (pacsCombatMember[i]->m_Mutex.WLock())
		{
			AddBonusExpToChar(pacsCombatMember[i], NULL, llExp, TRUE, TRUE);
			pacsCombatMember[i]->m_Mutex.Release();
		}
	}

	/*
	if (nBonusExp < 1 || nMonsterExp < 1 || !pcsKilledChar)
		return FALSE;

	AgpdParty	*pcsParty = m_pagpmParty->GetPartyLock(lPartyID);
	if (!pcsParty)
		return FALSE;

	AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
	ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

	INT32	lMemberTotalLevel	= 0;

	INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsKilledChar, pcsCombatMember, &lMemberTotalLevel);
	if (lNumCombatMember < 1 || lMemberTotalLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return FALSE;
	}

	switch (lNumCombatMember) {
	case 4:
		nBonusExp += nBonusExp * 1.30;
		break;
	case 5:
		nBonusExp += nBonusExp * 1.50;
		break;
	case 6:
		nBonusExp += nBonusExp * 1.75;
		break;
	case 7:
		nBonusExp += nBonusExp * 2.05;
		break;
	case 3:
		nBonusExp += nBonusExp * 1.15;
		break;
	case 2:
		nBonusExp += nBonusExp * 1.05;
		break;
	case 1:
		break;
	default:
		nBonusExp += nBonusExp * 2.40;
		break;
	}

	nBonusExp += nMonsterExp / 10;

	INT32	lMaximumExp	= nMonsterExp * 2;

	// pcsKilledChar���� �Ÿ��� ����ؼ� �� �ָ� �������ִ³ѵ� (������ �þ߳��� �ִ��� ����)�� ���ܽ�Ų��.

	for (int i = 0; i < lNumCombatMember; i++)
	{
		if (!pcsCombatMember[i])
			continue;

		INT32	lMemberLevel	= m_pagpmFactors->GetLevel(&pcsCombatMember[i]->m_csFactor);

		INT32	lExp			= (INT32) (nBonusExp * (lMemberLevel / (float) lMemberTotalLevel));
		if (lExp < 1)
			continue;

		if (lExp > lMaximumExp)
			lExp = lMaximumExp;

		AddBonusExpToChar(pcsCombatMember[i], lExp);
	}

	pcsParty->m_Mutex.Release();
	*/

	return TRUE;
}

//		ConAgsdDeath
//	Functions
//		- AgsdCharacter constructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::ConAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmDeath     *pThis			= (AgsmDeath *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdDeath     *pagsdDeath		= pThis->GetADCharacter(pCharacter);

	pagsdDeath->m_ulDeadTime		= 0;

	pagsdDeath->m_bDropItem			= FALSE;

	// �� ���� DB���� �о���δ�. ����... ����...
	pagsdDeath->m_llLastLoseExp		= 0;

	pagsdDeath->m_bIsNeedProcessDeath	= FALSE;

	pagsdDeath->m_ulRecoveryHPPointTime = 0;
	pagsdDeath->m_ulRecoveryMPPointTime = 0;
	pagsdDeath->m_tCharismaGiveTime = 0;

	return TRUE;
}

//		DesAgsdDeath
//	Functions
//		- AgsdCharacter destructor
//	Arguments
//		- pData : character data pointer
//		- pClass : this class pointer
//	Return value
//		- BOOL : ��������
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::DesAgsdDeath(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (pData == NULL || pClass == NULL)
	{
		// invalid constructor parameter
		return FALSE;
	}

	AgsmDeath     *pThis			= (AgsmDeath *) pClass;
	AgpdCharacter *pCharacter		= (AgpdCharacter *) pData;
	AgsdDeath     *pagsdDeath		= pThis->GetADCharacter(pCharacter);

	return TRUE;
}

//		CBAttackCharacter
//	Functions
//		- ĳ���Ͱ� ������ �ִ°�� Combat Module���� ó������ �� ����� �޾Ƽ� ���� ����� ���õ� ó������ �Ѵ�.
//			1. HitHistory�� �߰��Ѵ�.
//			2. Ÿ���� �׾����� �˻��ؼ� �׾��ٸ� PrepareDeath()�� ȣ���Ѵ�.
//	Arguments
//		- pData : attack character
//		- pClass : this module pointer
//		- pCustData : target character
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::CBAttackCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmDeath					*pThis			= (AgsmDeath *) pClass;
	pstAgsmCombatAttackResult	pstAttackResult = (pstAgsmCombatAttackResult) pData;

	if (pThis->m_pagsmFactors->IsDead(&pstAttackResult->pTargetChar->m_csFactor))
		pThis->PrepareDeath(pstAttackResult->pTargetChar, pstAttackResult->pAttackChar);
	
	// 2006.09.14. steeple
	// ���� ���Ͷ�� ���⼭ �׿��ش�.
	if(pThis->m_pagpmCharacter->IsMonster(pstAttackResult->pAttackChar) &&
			pstAttackResult->pAttackChar->m_pcsCharacterTemplate->m_bSelfDestructionAttackType)
	{
		pThis->SetDead(pstAttackResult->pAttackChar);
	}

	return TRUE;
}

//		PrepareDeath
//	Functions
//		- ĳ���Ͱ� ������� RoundTripLatency ��ŭ ��ٸ���. �� �Ŀ��� �������¸� �׶� ��¥�� ���������� �����ϰ� ó���Ѵ�.
//			1. ĳ���� ���¸� AGPDCHAR_STATUS_PREDEAD�� �����Ѵ�.
//			2. OnIdle() �̺�Ʈ�� RoundTripLatency �ð��Ŀ� �̺�Ʈ�� �߻��ϵ��� �����Ѵ�.
//				(��, RoundTripLatency �ð��Ŀ� ��¥ �׾����� �����Ѵ�)
//			3. AGPDCHAR_STATUS_PREDEAD�� �� �ð��� �����Ѵ�.
//	Arguments
//		- pTargetChar : ���� ĳ���� (HP�� 0�̵� ĳ����)
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmDeath::PrepareDeath(AgpdCharacter *pTargetChar, AgpdCharacter *pAttackChar)
{
	if (!pTargetChar)
		return FALSE;

	if (pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD ||
		pTargetChar->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
		return TRUE;

	// ĳ���� ���� AGPDCHAR_STATUS_PREDEAD�� ����
	m_pagpmCharacter->UpdateActionStatus(pTargetChar, AGPDCHAR_STATUS_PREDEAD);
	
	AgsdCharacter	*pcsAgsdCharacter = m_pagsmCharacter->GetADCharacter(pTargetChar);

	// Pinch �����Ǿ��ִ� Monster Death�� ó�����ش�.
	ProcessPinchCharacterDeath(pAttackChar, pTargetChar);

	// ���������� �������� �����Ѵ�.
	if (pAttackChar)
	{
		AgsdDeath *pcsDeath = GetADCharacter((PVOID) pTargetChar);

		pcsDeath->m_csLastHitBase.m_eType	= pAttackChar->m_eType;
		pcsDeath->m_csLastHitBase.m_lID		= pAttackChar->m_lID;
	}

	//if (pcsAgsdCharacter->m_ulRoundTripLatencyMS)		// Latency�� 0���� �ƴ��� �˻�
	//{
	/*
		if (!AddIdleEvent2(GetClockCount() + (UINT32) (pcsAgsdCharacter->m_ulRoundTripLatencyMS / 2), pTargetChar->m_lID, this, ProcessIdleDeath, NULL))
		{
			// ����� ĳ���� ���� ���󺹱�
			//

			return FALSE;
		}
	*/
	//}
	//else											// Latency�� 0�̶�� �ٷ� ������ ó���� ����. (NPC �ΰ�� ����� PC�� �̷����� ����)
	//{
	return ProcessIdleDeath(pTargetChar);
	//}

	//return TRUE;
}

BOOL AgsmDeath::CBUpdateCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			ulClockCount	= PtrToUint(pCustData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBUpdateCharacter"));

	/*
	if (!pThis->m_pagsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, ulClockCount))
		return TRUE;
	*/

	pThis->m_pagsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH);
	pThis->m_pagsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, ulClockCount);

	pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);

	// ������ 1, 2�� �Ŀ� ������ ������ ����� �۾��� �Ѵ�.
	/////////////////////////////////////////////////////////////////////
	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);

		if (pcsAgsdDeath->m_bIsNeedProcessDeath)
		{
			if (pcsAgsdDeath->m_ulDeadTime + AGSMDEATH_DROP_ITEM_INTERVAL <= ulClockCount)
			{
				pThis->Dead(pcsCharacter);

				pcsAgsdDeath->m_bIsNeedProcessDeath	= FALSE;

				if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
				{
					pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TEN_SECONDS);
				}
				else
				{
					pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
				}
			}
			else
			{
				pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
			}
		}
		else if ((((pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_TYPE_NONE || 
					pThis->m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsCharacter) == AGPD_SIEGE_MONSTER_SUMMON) &&
			     pcsAgsdDeath->m_ulDeadTime
			   + pThis->m_pagsmCharacter->GetReservedTimeForDestory(pcsCharacter)
			   + AGSMDEATH_RES_INTERVAL <= ulClockCount)) ||
			   pcsCharacter->m_ulCharType & AGPMCHAR_TYPE_SUMMON)
		{
			pThis->ProcessIdleDeath(pcsCharacter);
		}
		else
		{
			if (!pThis->m_pagpmCharacter->IsPC(pcsCharacter))
				pThis->m_pagsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_DEATH, AGSDCHAR_IDLE_INTERVAL_TWO_SECONDS);
		}

		/*
		AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
		if (pcsAgsdDeath &&
			!pcsAgsdDeath->m_bDropItem &&
			pcsAgsdDeath->m_ulDeadTime + 1700 < ulClockCount)
		{
			pThis->ProcessDropItem(pcsCharacter);

			pcsAgsdDeath->m_bDropItem	= TRUE;
		}
		*/
	}
	else
	{
		if (pThis->m_pagpmCharacter->IsPC(pcsCharacter))
		{
			// ���� Point�� ȸ����Ų��.

			if (pcsCharacter->m_unCurrentStatus != AGPDCHAR_STATUS_IN_GAME_WORLD ||
				pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD)
			{
				return TRUE;
			}

			AgsdDeath	*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
			AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);

			BOOL	bIsRecoveryHP	= FALSE;
			BOOL	bIsRecoveryMP	= FALSE;

			if (pcsAgsdDeath->m_ulRecoveryHPPointTime <= ulClockCount)
				bIsRecoveryHP	= TRUE;

			if (pcsAgsdDeath->m_ulRecoveryMPPointTime <= ulClockCount)
				bIsRecoveryMP	= TRUE;

			if (!bIsRecoveryHP && !bIsRecoveryMP)
				return TRUE;

			INT32	lCon	= 0;
			INT32	lInt	= 0;

			pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lCon, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
			pThis->m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lInt, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

			if (bIsRecoveryHP)
				pcsAgsdDeath->m_ulRecoveryHPPointTime	= ulClockCount + max(AGSMDEATH_RECOVERY_POINT_INTERVAL, (INT32) (10.5f - (lCon / 100.0f) * 1000));
			if (bIsRecoveryMP)
				pcsAgsdDeath->m_ulRecoveryMPPointTime	= ulClockCount + max(AGSMDEATH_RECOVERY_POINT_INTERVAL, (INT32) (10.5f - (lInt / 100.0f) * 1000));

			INT32	lModifiedPointHP	= 0;
			INT32	lModifiedPointMP	= 0;

			if (bIsRecoveryHP)
				lModifiedPointHP	= pThis->m_pagpmSkill->GetModifiedHPRegen(pcsCharacter)
									+ pcsAgsdCharacter->m_stOptionSkillData.m_lRegenHP;
			if (bIsRecoveryMP)
				lModifiedPointMP	= pThis->m_pagpmSkill->GetModifiedMPRegen(pcsCharacter)
									+ pcsAgsdCharacter->m_stOptionSkillData.m_lRegenMP;

			PVOID	pvPacketFactor = NULL;
			if (!pThis->m_pagsmFactors->RecoveryPointInterval(&pcsCharacter->m_csFactor, &pvPacketFactor, lModifiedPointHP, lModifiedPointMP, bIsRecoveryHP, bIsRecoveryMP) || !pvPacketFactor)
			{
				return TRUE;
			}

			INT16	nPacketLength = 0;
			INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

			PVOID	pvPacket = pThis->m_pagpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																		&cOperation,							// Operation
																		&pcsCharacter->m_lID,					// Character ID
																		NULL,									// Character Template ID
																		NULL,									// Game ID
																		NULL,									// Character Status
																		NULL,									// Move Packet
																		NULL,									// Action Packet
																		pvPacketFactor,							// Factor Packet
																		NULL,									// llMoney
																		NULL,									// bank money
																		NULL,									// cash
																		NULL,									// character action status
																		NULL,									// character criminal status
																		NULL,									// attacker id (������� ������ �ʿ�)
																		NULL,									// ���� �����Ǽ� �ʿ� �������� ����
																		NULL,									// region index
																		NULL,									// social action index
																		NULL,									// special status
																		NULL,									// is transform status
																		NULL,									// skill initialization text
																		NULL,									// face index
																		NULL,									// hair index
																		NULL,									// Option Flag
																		NULL,									// bank size
																		NULL,									// event status flag
																		NULL,									// remained criminal status time
																		NULL,									// remained murderer point time
																		NULL,									// nick name
																		NULL,									// gameguard
																		NULL									// last killed time in battlesquare
																		);

			if(pvPacketFactor)
				pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

			if(pvPacket)
			{
				pThis->m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

				if(!pThis->m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5))
				{
					//TRACEFILE("AgsmDeath::CBUpdateCharacter() ���� SendPacketNear() ����");
				}

				pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
			}
		}
	}

	return TRUE;
}

// 2005.04.09. steeple
BOOL AgsmDeath::SendUpdateRecoveryPoint(AgpdCharacter* pcsCharacter, PVOID pvPacketFactor)
{
	if(!pcsCharacter || !pvPacketFactor)
		return FALSE;

	INT16	nPacketLength = 0;
	INT8	cOperation = AGPMCHAR_PACKET_OPERATION_UPDATE;

	PVOID	pvPacket = m_pagpmCharacter->m_csPacket.MakePacket(TRUE, &nPacketLength, AGPMCHARACTER_PACKET_TYPE,
																&cOperation,							// Operation
																&pcsCharacter->m_lID,					// Character ID
																NULL,									// Character Template ID
																NULL,									// Game ID
																NULL,									// Character Status
																NULL,									// Move Packet
																NULL,									// Action Packet
																pvPacketFactor,							// Factor Packet
																NULL,									// llMoney
																NULL,									// bank money
																NULL,									// cash
																NULL,									// character action status
																NULL,									// character criminal status
																NULL,									// attacker id (������� ������ �ʿ�)
																NULL,									// ���� �����Ǽ� �ʿ� �������� ����
																NULL,									// region index
																NULL,									// social action index
																NULL,									// special status
																NULL,									// is transform status
																NULL,									// skill initialization text
																NULL,									// face index
																NULL,									// hair index
																NULL,									// Option Flag
																NULL,									// bank size
																NULL,									// event status flag
																NULL,									// remained criminal status time
																NULL,									// remained murderer point time
																NULL,									// nick name
																NULL,									// gameguard
																NULL									// last killed time in battlesquare
																);

	if(pvPacketFactor)
		m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	if(pvPacket)
	{
		m_pagpmCharacter->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

		if(!m_pagsmAOIFilter->SendPacketNear(pvPacket, nPacketLength, pcsCharacter->m_stPos, PACKET_PRIORITY_5))
		{
			//TRACEFILE("AgsmDeath::CBUpdateCharacter() ���� SendPacketNear() ����");
		}

		m_pagpmCharacter->m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmDeath::CBSendCharacterAllServerInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	return TRUE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	UINT32			*pulBuffer		= (UINT32 *)		pCustData;

	BOOL	bSendResult = pThis->SendPacketDeath(pcsCharacter, pulBuffer[0], (BOOL) pulBuffer[1]);
	bSendResult &= pThis->SendPacketDeathADItem(pcsCharacter, pulBuffer[0], (BOOL) pulBuffer[1]);

	return bSendResult;
}

BOOL AgsmDeath::CBDisconnectCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgsdDeath		*pcsAgsdDeath	= pThis->GetADCharacter(pcsCharacter);
	if (!pcsAgsdDeath)
		return FALSE;

	if (pcsCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
	{
		if (pcsAgsdDeath->m_bIsNeedProcessDeath)
		{
			pThis->Dead(pcsCharacter, FALSE);
		}

		/*
		pThis->m_pagsmFactors->Resurrection(&pcsCharacter->m_csFactor, NULL);

		AuPOS	stResPos;
		ZeroMemory(&stResPos, sizeof(AuPOS));

		if (pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pcsCharacter, &stResPos))
		{
			pThis->m_pagpmCharacter->UpdatePosition(pcsCharacter, &stResPos, FALSE, TRUE);
		}
		*/
	}

	return TRUE;
}

VOID AgsmDeath::SetItemDropRate(AgpdItem *pcsItem, INT32 lDropRate)
{
	SetItemDropRate(GetADItem(pcsItem), lDropRate);
}

VOID AgsmDeath::SetItemDropRate(AgsdDeathADItem *pstDeathItem, INT32 lDropRate)
{
	pstDeathItem->m_lDropRate = lDropRate;
}

BOOL AgsmDeath::DropItem(AgpdCharacter *pcsCharacter, ApBase *pcsAttacker)
{
	PROFILE("AgsmDeath::DropItem");

	if (!pcsCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("DropItem"));

	stAgsmDeathDropItemInfo			stDropItemInfo;

	stDropItemInfo.pvClass			= this;
	stDropItemInfo.pcsAttackerBase	= pcsAttacker;

	AgpdItem	*pcsDropItem	= NULL;
	INT32		lDropRate		= 0;
	BOOL		bRetval			= TRUE;
	if (m_pagpmCharacter->IsMurderer(pcsCharacter))
	{
		// �����ڶ�� ���濡 �ִ� ���� �ϳ��� ��� �� ������ ���� Ȯ���� �߻���Ų��.
		//
		if (m_csRandomNumber.randInt(100) < m_pagpmCharacter->GetMurdererLevel(pcsCharacter) * 10)
		{
			// ���� �ϳ��� ��� ���ž� �Ѵ�.
			AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter(pcsCharacter);
			INT32	lItemCount = m_pagpmGrid->GetItemCount(&pcsItemADChar->m_csInventoryGrid);
			if (lItemCount > 0)
			{
				// �κ��丮�� �ִ� ���߿� �ϳ��� ��󳽴�.
				INT32			lRandomNumber = m_csRandomNumber.randInt(lItemCount - 1);
				INT32			i;
				AgpdGridItem *	pcsGridItem;

				i = 0;
				for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
					 pcsGridItem;
					 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
				{
					 if (--lRandomNumber == 0)
						 break;
				}

				if (pcsGridItem)
					pcsDropItem = m_pagpmItem->GetItem(pcsGridItem);
				{
					if (pcsDropItem)
					{
						// ������ ����Ȯ�� 100%�� �����Ѵ�.
						AgsdDeathADItem	*pcsDeathADItem	= GetADItem(pcsDropItem);

						// ���� Ȯ���� �߻���Ų��.
						lDropRate = pcsDeathADItem->m_lDropRate;

						////////////////////////////////////////////////////////////
						SetItemDropRate(pcsDeathADItem, 100000);
						////////////////////////////////////////////////////////////

						bRetval = m_pagpmItem->CharacterForAllItems(pcsCharacter, CBDropItem, &stDropItemInfo);
					}
				}
			}
		}
	}
	else if (!m_pagsmCharacter->GetCharDPNID(pcsCharacter))
	{
		bRetval = m_pagpmItem->CharacterForAllItems(pcsCharacter, CBDropItem, &stDropItemInfo);

		// �ʿ��ϴٸ� ���⼭ ���� ���ž� �Ѵ�.
		// ���� �� ������ ����� �ٴۿ� ������.
		//
		//	1. AddItemMoney(INT32 lIID, INT32 lMoney, AuPOS pos) �̰ɷ� ������ �����.
		//	2. Drop Rate�� 100%�� �����Ѵ�.
		//	3. CBDropItem(pcsItem, &stDropItemInfo, pcsCharacter) �� ȣ���Ѵ�.
		//
	}

	if (pcsDropItem)
	{
		// Ȯ���� �����Ƿ� �ǵ�����.
		SetItemDropRate(pcsDropItem, lDropRate);
	}

	return bRetval;
}

BOOL AgsmDeath::CBDropItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmDeath::CBDropItem");

	if (!pData || !pClass)
		return FALSE;

	stAgsmDeathDropItemInfo		*pstDropItemInfo	= (stAgsmDeathDropItemInfo *)	pClass;

	if (!pstDropItemInfo->pvClass)
		return FALSE;

	AgsmDeath *					pThis = (AgsmDeath *) pstDropItemInfo->pvClass;
	AgpdItem *					pcsItem = (AgpdItem *) pData;
	AgpdCharacter *				pcsCharacter = (AgpdCharacter *) pCustData;
	AgsdDeathADItem *			pstDeathItem = pThis->GetADItem(pcsItem);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBDropItem"));

	if (pThis->m_csRandomNumber.randInt(100000) <= pstDeathItem->m_lDropRate)
	{
		//pThis->m_pagpmItem->ReleaseItem(pcsItem);

		// drop item�� looting �ϴ� ���ѿ� ���� ������ �����Ѵ�. (�� ���ñ� ���忡 ���� �����̴�.)
		if (pstDropItemInfo->pcsAttackerBase)
		{
			AgsdItem	*pcsAgsdItem	= pThis->m_pagsmItem->GetADItem(pcsItem);
			pcsAgsdItem->m_csFirstLooterBase.m_eType	= pstDropItemInfo->pcsAttackerBase->m_eType;
			pcsAgsdItem->m_csFirstLooterBase.m_lID		= pstDropItemInfo->pcsAttackerBase->m_lID;

			pcsAgsdItem->m_ulDropTime = pThis->GetClockCount();
		}
		//////////////////////////////////////////////////////////////////////////////////////////

		pcsItem->m_posItem = pcsCharacter->m_stPos;

		pThis->m_pagpmItem->AddItemToField(pcsItem);

		pstDeathItem->m_lDropRate = 0;
	}

	return TRUE;
}

BOOL AgsmDeath::DropSkull(AgpdCharacter *pcsCharacter, AgpmItemSkullInfo *pstSkullInfo)
{
	if (!pcsCharacter || !pstSkullInfo)
		return FALSE;

	if (strlen(pstSkullInfo->szKiller) < 1 && pstSkullInfo->lPartyMemberID[0] == AP_INVALID_CID)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("DropSkull"));

	// pcsCharacter�� union rank point�� �����ϰ� �ִ� ������ ������.
	// ���� �ױ��� rank point�� �����Ѵ�.

	AgpdItem	*pcsItemSkull = m_pagsmItemManager->CreateItemSkull();
	if (!pcsItemSkull)
		return FALSE;

	// ���� ĳ����, ���� ĳ������ �Ƶ�, union rank�� SkullItem�� �����Ѵ�.
	//////////////////////////////////////////////////////////////////////////////
	if (!pcsItemSkull->m_pstSkullInfo)
	{
		ZeroMemory(pcsItemSkull->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItemSkull->m_szDeleteReason, "Skull information error", AGPMITEM_MAX_DELETE_REASON);

		m_pagpmItem->RemoveItem(pcsItemSkull->m_lID, TRUE);
		return FALSE;
	}

	pcsItemSkull->m_nCount	= 1;

	CopyMemory(pcsItemSkull->m_pstSkullInfo, pstSkullInfo, sizeof(AgpmItemSkullInfo));

	CopyMemory(pcsItemSkull->m_pstSkullInfo->szSkullOwner, pcsCharacter->m_szID, sizeof(CHAR) * AGPACHARACTER_MAX_ID_STRING);
	pcsItemSkull->m_pstSkullInfo->lSkullUnionRank = m_pagpmUnion->GetUnionRank(pcsCharacter);
	if (pcsItemSkull->m_pstSkullInfo->lSkullUnionRank <= 0 || pcsItemSkull->m_pstSkullInfo->lSkullUnionRank > AGPMUNION_MAX_UNION_RANK)
	{
		ZeroMemory(pcsItemSkull->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsItemSkull->m_szDeleteReason, "Skull information error 2", AGPMITEM_MAX_DELETE_REASON);

		m_pagpmItem->RemoveItem(pcsItemSkull->m_lID, TRUE);
		return FALSE;
	}

	pcsItemSkull->m_posItem = pcsCharacter->m_stPos;

	return m_pagpmItem->AddItemToField(pcsItemSkull);
}

BOOL AgsmDeath::DropSkull(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	INT32		lRace			= m_pagpmFactors->GetRace(&pcsCharacter->m_csFactor);

	AgpdItem	*pcsSkullItem	= NULL;

	if (lRace == AURACE_TYPE_HUMAN)
	{
		pcsSkullItem	= m_pagsmItemManager->CreateItem(m_pagpmItem->GetHumanSkullTID());
	}
	else
	{
		pcsSkullItem	= m_pagsmItemManager->CreateItem(m_pagpmItem->GetOrcSkullTID());
	}

	if (!pcsSkullItem)
		return FALSE;

	pcsSkullItem->m_nCount	= 1;

	if (!m_papmEventManager->GetRandomPos(&pcsCharacter->m_stPos, &pcsSkullItem->m_posItem, 50, 100, TRUE))
		pcsSkullItem->m_posItem	= pcsCharacter->m_stPos;

	return m_pagpmItem->AddItemToField(pcsSkullItem);
}

BOOL AgsmDeath::SetSkullInfo(ApBase *pcsBase, AgpmItemSkullInfo *pstSkullInfo, AgpdCharacter *pcsTargetChar)
{
	if (!pcsBase || !pstSkullInfo || !pcsTargetChar)
		return FALSE;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			INT32	lUnionRank = GetUnionRank(pcsBase);

			if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
				return FALSE;

			CopyMemory(pstSkullInfo->szKiller, ((AgpdCharacter *) pcsBase)->m_szID, sizeof(CHAR) * AGPACHARACTER_MAX_ID_STRING);
			pstSkullInfo->lKillerUnionRank = lUnionRank;
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			INT32	lUnionRank = GetUnionRank(pcsBase);

			if (lUnionRank <= 0 || lUnionRank > AGPMUNION_MAX_UNION_RANK)
				return FALSE;

//			AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
//			ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

			ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsCombatMember;
			pcsCombatMember.MemSetAll();

			INT32	lMemberTotalLevel	= 0;

//			INT32	lNumCombatMember = m_pagsmParty->GetNearMember((AgpdParty *) pcsBase, pcsCombatMember, &lMemberTotalLevel);
			INT32	lNumCombatMember = m_pagsmParty->GetNearMember((AgpdParty *) pcsBase, pcsTargetChar, &pcsCombatMember[0], &lMemberTotalLevel);
			if (lNumCombatMember < 1)
			{
				return FALSE;
			}

			for (int i = 0; i < lNumCombatMember; ++i)
			{
				pstSkullInfo->lPartyMemberID[i] = pcsCombatMember[i]->m_lID;
			}

			pstSkullInfo->lKillerUnionRank = lUnionRank;
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

/*
BOOL AgsmDeath::KilledGuardian(AgpdCharacter *pCharacter, ApBase *pcsAttacker)
{
	if (!pCharacter || !pcsAttacker)
		return FALSE;

	if (!m_pagpmShrine->IsGuardian(pCharacter))
		return FALSE;

	// �������� ������̴�. history�� ���� ������� �̳��� �׿����� �˾ƺ���.

	// �⺻���� �����...
	//		1. 50% �̻��� �������� �س� (�����̰� ��Ƽ���̰� �������)
	//		2. ���� 50% �̻��� �������� ��Ƽ�� ���ѰŶ�� �ش� ��Ƽ������ ��� kill ���� �÷��ش�.

	UINT32	ulCurrentClock = GetClockCount();

	// character, party�� ���� ������� �׿��ٴ� ������ �߰��Ѵ�.
	switch (pcsAttacker->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			m_pagpmShrine->AddKillGuardian((AgpdCharacter *) pcsAttacker, pCharacter, ulCurrentClock);
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			AgpdParty		*pcsParty	= (AgpdParty *) pcsAttacker;
			if (!pcsParty)
				return FALSE;

			for (int i = 0; i < pcsParty->m_nCurrentMember; ++i)
			{
				// �����Ѱ��� �Ÿ��� �����ؼ� �þ߸� ����� �߰������� �ʴ´�.
				if (m_pagsmParty->CheckValidDistance(pcsParty->m_pcsMemberList[i], pCharacter) > 1)
					continue;

				if (!pcsParty->m_pcsMemberList[i])
				{
					pcsParty->m_pcsMemberList[i] = m_pagpmCharacter->GetCharacter(pcsParty->m_lMemberListID[i]);
					if (!pcsParty->m_pcsMemberList[i])
						continue;
				}

				m_pagpmShrine->AddKillGuardian(pcsParty->m_pcsMemberList[i], pCharacter, ulCurrentClock);
			}
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}
*/

INT32 AgsmDeath::GetRacialPoint(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	// ���� ����Ÿ�� ����. �׳� 1�� �����Ѵ�.
	//
	//
	//
	//
	//

	return 1;
}

INT32 AgsmDeath::GetMonsterExp(AgpdCharacter *pcsCharacter, AgpdCharacter *pcsMostDamager)
{
	if (!pcsCharacter)
		return FALSE;

	INT32	lBaseEXP	= 0;
	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lBaseEXP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_BASE_EXP);

	BOOL  bTPackUser	= FALSE;		
	INT32 ulPCRoomType	= 0;

	m_pagpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsMostDamager, &ulPCRoomType);
	if(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK)
		bTPackUser = TRUE;

	return (INT32)(m_pagpmConfig->GetExpAdjustmentRatio(bTPackUser) * (float)lBaseEXP);

	/*
	AgpdFactorCharStatus	*pcsCharStatus = 
		(AgpdFactorCharStatus *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_CHAR_STATUS);

	if (!pcsCharStatus)
		return 0;

	return (INT32) ((100 + (pcsCharStatus->lValue[AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL] * 10)) * GetRacialPoint(pcsCharacter));
	*/
}

INT32 AgsmDeath::GetAttackerLevel(ApBase *pcsAttackerBase)
{
	if (!pcsAttackerBase)
		return 0;

	switch (pcsAttackerBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			AgpdCharacter	*pcsAttacker = m_pagpmCharacter->GetCharacter(pcsAttackerBase->m_lID);
			if (pcsAttacker)
			{
				return m_pagpmCharacter->GetLevel(pcsAttacker);
			}
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			AgpdParty	*pcsParty	= m_pagpmParty->GetParty(pcsAttackerBase->m_lID);
			if (!pcsParty)
				return 0;

			return m_pagpmParty->GetPartyTotalMemberLevel(pcsParty) / pcsParty->m_nCurrentMember;
		}
		break;
	}

	return 0;
}

INT32 AgsmDeath::GetBonusExp(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry)
{
	PROFILE("AgsmDeath::GetBonusExp");

	if (!pcsKilledChar || !pstHistoryEntry)
		return 0;

	AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(pstHistoryEntry->m_csSource.m_lID);
	if (!pcsCharacter)
		return 0;

	AgpdPartyADChar	*pcsPartyAttachData	= m_pagpmParty->GetADCharacter(pcsCharacter);
	if (!pcsPartyAttachData || pcsPartyAttachData->lPID != AP_INVALID_PARTYID)
		return 0;

	// damage�� �����´�.

	INT32	lDamage	= 0;

	if (pstHistoryEntry->m_pcsUpdateFactor)
		m_pagpmFactors->GetValue(pstHistoryEntry->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	lDamage	= -lDamage;
	if (lDamage < 1)
	{
		pcsCharacter->m_Mutex.Release();
		return 0;
	}

	// ������ HP�� ���̾����� �˾ƿ´�.
	INT32	lTargetMaxHP	= GetMaxHP(pcsKilledChar);

	if (lDamage > lTargetMaxHP)
		lDamage = lTargetMaxHP;

	// (Mob Base EXP * 0.9) + Ran ((Mob Base EXP * 1.1) - (Mob Base EXP * 0.9))

	// 2007.01.30. steeple
	// FirstHit ���Ŀ� ������ / �ƽ�HP ���� �־���.
	FLOAT	fFirstAttackBonus	= 0.0f;
	FLOAT	fRandBonus = (FLOAT) m_csRandomNumber.rand((double)(lTotalBonusExp * 0.02f));
	if (pstHistoryEntry->m_bFirstHit)
		fFirstAttackBonus = ((FLOAT)lDamage / (FLOAT)lTargetMaxHP) * (((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.1f);

	FLOAT	fBonusExp	= 0.0f;

	FLOAT	fDMGExp			= (FLOAT)lDamage / (FLOAT) lTargetMaxHP * ((((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.9f));
	if (fDMGExp < 1.0f)
		fDMGExp	= 1.0f;

	// ���γѰ� ������ ������ �����´�.
	INT32	lAttackLevel	= GetAttackerLevel((ApBase *) &pstHistoryEntry->m_csSource);
	INT32	lTargetLevel	= m_pagpmCharacter->GetLevel(pcsKilledChar);

	if (lAttackLevel < 1 || lTargetLevel < 1)
	{
		pcsCharacter->m_Mutex.Release();
		return 0;
	}

	if (lTargetLevel >= lAttackLevel)
		fBonusExp += ((fDMGExp + fFirstAttackBonus) * (1.0f + pow((FLOAT)(lTargetLevel - lAttackLevel) * 2.0f, 2.0f) / 500.0f));
	else
		fBonusExp += ((fDMGExp + fFirstAttackBonus) * (1.0f - pow((FLOAT)(lAttackLevel - lTargetLevel) * 2.0f, 2.0f) / 500.0f));

	// �����ں��� ���� ���� ���� ������� �����ϱ�
	if ( fBonusExp < 1.0f) // 0.0f ���� 1.0f�� �������� - arycoat 2008.9.25
		fBonusExp = 1.0f;

	// �����ں��� ���� ���� ���� ������� �����ϱ�
	if ( (lTargetLevel - lAttackLevel) >= AGSMDEATH_EXP_LEVEL_DIFF)
		fBonusExp = 1.0f;

	AddBonusExpToChar(pcsCharacter, pcsKilledChar, (INT32) fBonusExp, TRUE, TRUE);

	pcsCharacter->m_Mutex.Release();

	return (INT32) fBonusExp;
}

INT32 AgsmDeath::GetBonusExpToParty(INT32 lTotalBonusExp, AgpdCharacter *pcsKilledChar, AgsdCharacterHistoryEntry *pstHistoryEntry)
{
	PROFILE("AgsmDeath::GetBonusExpToParty");

	if (!pcsKilledChar || !pstHistoryEntry)
		return 0;

	//STOPWATCH2(GetModuleName(), _T("GetBonusExpToParty"));

	// ���γѰ� ������ ������ �����´�.
	AgpdParty	*pcsParty	= m_pagpmParty->GetPartyLock(pstHistoryEntry->m_csSource.m_lID);
	if (!pcsParty)
		return 0;

	// damage�� �����´�.
	// 2007.03.09. steeple
	// Damage �������� �� �����ߴ�.
	INT32 lDamage = GetTotalDamageCurrentMember(pcsParty, pcsKilledChar);
	
	//if (pstHistoryEntry->m_pcsUpdateFactor)
	//	m_pagpmFactors->GetValue(pstHistoryEntry->m_pcsUpdateFactor, &lDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);


	//lDamage	= -lDamage;
	if (lDamage < 1)
	{
		pcsParty->m_Mutex.Release();
		return 0;
	}

	// ������ HP�� ���̾����� �˾ƿ´�.
	INT32	lTargetMaxHP	= GetMaxHP(pcsKilledChar);

	if (lDamage > lTargetMaxHP)
		lDamage = lTargetMaxHP;

	// (Mob Base EXP * 0.9) + Ran ((Mob Base EXP * 1.1) - (Mob Base EXP * 0.9))
	FLOAT	fBonusExp		= 0.0f;
	FLOAT	fRandBonus		= (FLOAT) m_csRandomNumber.rand((double)((FLOAT)lTotalBonusExp * 0.02f));
	if (pstHistoryEntry->m_bFirstHit)
		fBonusExp = ((FLOAT)lDamage / (FLOAT)lTargetMaxHP) * (((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.1f);

//	AgpdCharacter	*pcsCombatMember[AGPMPARTY_MAX_PARTY_MEMBER];
//	ZeroMemory(pcsCombatMember, sizeof(AgpdCharacter *) * AGPMPARTY_MAX_PARTY_MEMBER);

	ApSafeArray<AgpdCharacter *, AGPMPARTY_MAX_PARTY_MEMBER>	pcsCombatMember;
	pcsCombatMember.MemSetAll();

	INT32	lMemberTotalLevel	= 0;

	//INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsCombatMember, &lMemberTotalLevel);
	INT32	lNumCombatMember = m_pagsmParty->GetNearMember(pcsParty, pcsKilledChar, &pcsCombatMember[0], &lMemberTotalLevel);

	// 2007.03.09. steeple
	// ��Ƽ�� ���� ���� �Ÿ��� ������� ��ü �������� ���Ѵ�.
	lMemberTotalLevel = m_pagpmParty->GetPartyTotalMemberLevel(pcsParty);
	if (lNumCombatMember < 1 || lMemberTotalLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return FALSE;
	}

	AgpmPartyCalcExpType	eCalcExpType	= pcsParty->m_eCalcExpType;

	pcsParty->m_Mutex.Release();

	INT32	lHighestMemberLevel	= 0;
	INT32	lLowerMemberLevel = 0x00FFFFFF;
	for (int i = 0; i < lNumCombatMember; ++i)
	{
		 INT32	lCombatMemberLevel	= m_pagpmCharacter->GetLevel(pcsCombatMember[i]);

		 if (lCombatMemberLevel > lHighestMemberLevel)
			 lHighestMemberLevel	= lCombatMemberLevel;
		 if (lCombatMemberLevel < lLowerMemberLevel)
			 lLowerMemberLevel = lCombatMemberLevel;
	}

	INT32	lAttackLevel	= lMemberTotalLevel / lNumCombatMember;
	INT32	lTargetLevel	= m_pagpmCharacter->GetLevel(pcsKilledChar);

	if (lAttackLevel < 1 || lTargetLevel < 1)
	{
		pcsParty->m_Mutex.Release();
		return 0;
	}

	FLOAT	fDMGExp			= (FLOAT)lDamage / (FLOAT) lTargetMaxHP * ((((FLOAT)lTotalBonusExp * 0.99f + fRandBonus) * 0.9f));

	if (fDMGExp < 1.0f)
		fDMGExp	= 1.0f;

//	FLOAT	fLeaderCharisma	= 0.0f;
//	m_pagpmFactors->GetValue(&pcsParty->m_pcsMemberList[0]->m_csFactor, &fLeaderCharisma, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

	//BOOL	bAvailableLevelDiff = ( abs(lHighestMemberLevel - lLowerMemberLevel) <= AGPMPARTY_AVAILABLE_LEVEL_DIFF ) ? TRUE : FALSE;
	FLOAT	fFinalPartyExp	= 0.0f;

	if (lTargetLevel >= lHighestMemberLevel)
	{
		fFinalPartyExp = (fDMGExp + fBonusExp) * (1.0f + pow((FLOAT)(lTargetLevel - lHighestMemberLevel) * 2.0f, 2.0f) / 500.0f);
		//if (bAvailableLevelDiff == TRUE)
			fFinalPartyExp = (FLOAT) ( fFinalPartyExp * (1.0f + (m_partyBonus * (FLOAT)(lNumCombatMember - 1))) );
	}
	else
	{
		fFinalPartyExp = (fDMGExp + fBonusExp) * (1.0f - pow((FLOAT)(lTargetLevel - lHighestMemberLevel) * 2.0f, 2.0f) / 500.0f);
		//if (bAvailableLevelDiff == TRUE)
			fFinalPartyExp = (FLOAT) (fFinalPartyExp * (1.0f + (m_partyBonus * (FLOAT)(lNumCombatMember - 1))) );
	}


	for (int i = 0; i < lNumCombatMember; ++i)
	{
		if (!pcsCombatMember[i] ||
			pcsCombatMember[i]->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
			continue;

		if (!pcsCombatMember[i]->m_Mutex.WLock())
			continue;

		INT32	lMemberLevel		= m_pagpmCharacter->GetLevel(pcsCombatMember[i]);

		FLOAT	fBonusExpBaseDamage	= 0.0f;
		FLOAT	fBonusExpBaseLevel	= 0.0f;
		

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_DAMAGE )		// 2005.05.17. By SungHoon
//			||	eCalcExpType == AGPMPARTY_EXP_TYPE_BY_COMPLEX)
		{
			// ��� �⿩�� (����� ������)�� ���� �й�
			INT32	lMemberDamage		= 0;

			AgsdCharacterHistory	*pcsHistory = m_pagsmCharacter->GetHistory(pcsKilledChar);
			if (pcsHistory)
			{
				for (int j = 0; j < pcsHistory->m_lEntryNum; j++)
				{
					if (pcsHistory->m_astEntry[j].m_csSource.m_lID == pcsCombatMember[i]->m_lID &&
						pcsHistory->m_astEntry[j].m_lPartyID != AP_INVALID_PARTYID)
					{
						if (pcsHistory->m_astEntry[j].m_pcsUpdateFactor)
							m_pagpmFactors->GetValue(pcsHistory->m_astEntry[j].m_pcsUpdateFactor, &lMemberDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

						lMemberDamage	= -lMemberDamage;
						if (lMemberDamage < 1)
							lMemberDamage	= 0;

						break;
					}
				}

				if (lMemberDamage > 0)
				{
					if (lMemberDamage > lTargetMaxHP)
						lMemberDamage = lTargetMaxHP;

					if (lDamage < lMemberDamage)
						lMemberDamage	= lDamage;

					fBonusExpBaseDamage	= fFinalPartyExp * ((float) lMemberDamage / (float) lDamage);
				}
			}
	
		}

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_LEVEL )	// 2005.05.17. By SungHoon
//			|| eCalcExpType == AGPMPARTY_EXP_TYPE_BY_COMPLEX)
		{
			// ������ ���� �յ� �й�
			fBonusExpBaseLevel	= fFinalPartyExp * ((float) lMemberLevel / (float) lMemberTotalLevel);
		}
		FLOAT fMemberBonusExp = 0.0f;

		if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_DAMAGE)
			fMemberBonusExp	= fBonusExpBaseDamage;
		else if (eCalcExpType == AGPMPARTY_EXP_TYPE_BY_LEVEL)
			fMemberBonusExp	= fBonusExpBaseLevel;

		if (fMemberBonusExp <= 0.0f)
		{
			fMemberBonusExp	= 1.0f;
		}

		// ������ �ִ� �� ����.
		BOOL	bIsSafeArea	= FALSE;
		ApmMap::RegionTemplate	*pcsRegionTemplate	= m_papmMap->GetTemplate(m_papmMap->GetRegion(pcsCombatMember[i]->m_stPos.x , pcsCombatMember[i]->m_stPos.z));
		if (pcsRegionTemplate &&
			pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_SAFE)
			bIsSafeArea	= TRUE;

		// ���������� �̰� üũ����. 2007.02.02. steeple
		if (g_eServiceArea != AP_SERVICE_AREA_CHINA)
		{
			// 2007.04.09. steeple
			// �������̰� ���� ���� ��ġ 1 �ش�.
			if ((lTargetLevel - lMemberLevel) >= AGSMDEATH_EXP_LEVEL_DIFF)
				fMemberBonusExp = 1.0f;

			if ((lHighestMemberLevel - lMemberLevel) >= AGPMPARTY_AVAILABLE_LEVEL_DIFF)
				fMemberBonusExp = 0.0f;
		}

		if (fMemberBonusExp > 0.0f && !bIsSafeArea)
			AddBonusExpToChar(pcsCombatMember[i], NULL, (INT32) fMemberBonusExp, TRUE, TRUE);

		//AddBonusExpToParty(pcsParty, lBonusExp, lMemberTotalLevel, lNumCombatMember, pcsCombatMember);


		pcsCombatMember[i]->m_Mutex.Release();
	}

	return (INT32) fFinalPartyExp;
}

INT32 AgsmDeath::GetMaxHP(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return 0;

	INT32	lMaxHP	= 0;

	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

	return lMaxHP;
}

// 2007.03.09. steeple
// ���� ��Ƽ������ �������� ���ؿ´�. Ż���� ���� ���� �ȱ��Ѵ�.
// pcsParty �� Lock �Ǿ ���´�.
INT32 AgsmDeath::GetTotalDamageCurrentMember(AgpdParty* pcsParty, AgpdCharacter* pcsDeadCharacter)
{
	if(!pcsParty || !pcsDeadCharacter)
		return 0;

	AgsdCharacterHistory* pcsHistory = m_pagsmCharacter->GetHistory(pcsDeadCharacter);
	if(!pcsHistory)
		return 0;

	INT32 lTotalDamage, lEachDamage;
	lTotalDamage = lEachDamage = 0;

	for(INT32 i = 0; i < pcsParty->m_nCurrentMember; ++i)
	{
		for(INT32 j = 0; j < pcsHistory->m_lEntryNum; ++j)
		{
			if(pcsHistory->m_astEntry[j].m_csSource.m_lID == pcsParty->m_lMemberListID[i] &&
				pcsHistory->m_astEntry[j].m_lPartyID == pcsParty->m_lID)
			{
				if(pcsHistory->m_astEntry[j].m_pcsUpdateFactor)
					m_pagpmFactors->GetValue(pcsHistory->m_astEntry[j].m_pcsUpdateFactor, &lEachDamage, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

				lEachDamage = -lEachDamage;
				if(lEachDamage < 1)
					lEachDamage	= 0;

				lTotalDamage += lEachDamage;
			}
		}
	}

	return lTotalDamage;
}

INT64 AgsmDeath::GetPenaltyExp(AgpdCharacter *pcsCharacter, BOOL bIsResurrectionTown)
{
	// �߱������� ����ġ �г�Ƽ ���޶���.
	if (!m_pagpmConfig->DoesExpPenaltyOnDeath())
		return 0;

	if (!pcsCharacter)
		return 0;

	// murder���� �ƴ���... ����� ����ؾ��Ѵ�.

	INT32	lLevel	= m_pagpmCharacter->GetLevel(pcsCharacter);

	if (lLevel < 6)
		return 0;

	INT64	llMaxExp	= m_pagpmFactors->GetMaxExp((AgpdFactor *) m_pagpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	
	if( pcsCharacter->GetSpecialState( AGPDCHAR_SPECIAL_STATUS_LEVELLIMIT ) )
	{
		AgpdCharacterTemplate	*pcsTemplate	= pcsCharacter->m_pcsCharacterTemplate;
		INT32 lBeforeLevel = m_pagpmCharacter->GetLevelBefore(pcsCharacter);
		llMaxExp = m_pagpmFactors->GetMaxExp(&pcsTemplate->m_csLevelFactor[lBeforeLevel]);
	}

	if (llMaxExp < 1)
		return (-1);

	INT64	llPenaltyExp = (INT32) ((FLOAT)(llMaxExp * GetPenaltyExpRate(pcsCharacter, bIsResurrectionTown)) / 100.0f);

	if (m_pagpmCharacter->HasPenalty(pcsCharacter, AGPMCHAR_PENALTY_EXP_LOSE))
	{
		llPenaltyExp = (INT64) ( llPenaltyExp * 1.1f );
	}

	return llPenaltyExp;
}

// 2005.07.28. steeple
// Penalty Exp Rate �߰�. �⺻������ 10% ��� �ɷ� �Ѵ�.
INT32 AgsmDeath::GetPenaltyExpRate(AgpdCharacter* pcsCharacter, BOOL bIsResurrectionTown)
{
	if(!pcsCharacter)
		return 0;

	AgsdDeath* pcsAgsdDeath = GetADCharacter(pcsCharacter);
	if(!pcsAgsdDeath)
		return 0;

	INT32 lRate = 5;	// �⺻������ 10% ��� �ɷ� �Ѵ�.
	switch((eAgpmPvPTargetType)pcsAgsdDeath->m_cDeadType)
	{
		case AGPMPVP_TARGET_TYPE_MOB:
		{
			if(bIsResurrectionTown)		lRate = 3;
			else						lRate = 5;

			break;
		}

		case AGPMPVP_TARGET_TYPE_NORMAL_PC:
		{
			if(bIsResurrectionTown)		lRate = 0;
			else
			{
				//�߱� �������ÿ� ����ġ ��°� ���� ;; supertj@081013
				//if (g_eServiceArea == AP_SERVICE_AREA_CHINA && m_pagpmPvP->IsInRaceBattle())
				//	lRate = 0;
				//else
					lRate = 2;
			}

			break;
		}

		case AGPMPVP_TARGET_TYPE_ENEMY_GUILD:
		{
			lRate = 0;
			break;
		}
	}

	return lRate;
}


INT32 AgsmDeath::GetUnionRank(ApBase *pcsBase)
{
	if (!pcsBase)
		return 0;

	switch (pcsBase->m_eType) {
	case APBASE_TYPE_CHARACTER:
		{
			return m_pagpmUnion->GetUnionRank((AgpdCharacter *) pcsBase);
		}
		break;

	case APBASE_TYPE_PARTY:
		{
			return m_pagpmParty->GetPartyHighestUnionRank((AgpdParty *) pcsBase);
		}
		break;

	default:
		return 0;
		break;
	}

	return 0;
}

PVOID AgsmDeath::MakePacketDeath(AgpdCharacter *pcsCharacter, INT16 *pnPacketLength, BOOL bLogin)
{
	if (!pcsCharacter || !pnPacketLength)
		return NULL;

	AgsdDeath		*pcsDeath	= GetADCharacter(pcsCharacter);
	if (!pcsDeath)
		return NULL;

	PVOID	pvPacketLastHitBase = NULL;
	
	if (!bLogin)
		pvPacketLastHitBase = m_pagsmCharacter->MakePacketBase((ApBase *) &pcsDeath->m_csLastHitBase);

	UINT32	ulClockCount	= GetClockCount();

	PVOID	pvPacketDeath = m_csPacketDeath.MakePacket(FALSE, pnPacketLength, 0,
														&pcsCharacter->m_lID,
														&pcsDeath->m_ulDeadTime,
														&pcsDeath->m_llLastLoseExp,
														pvPacketLastHitBase,
														&ulClockCount);

	if (pvPacketLastHitBase)
		m_csPacket.FreePacket(pvPacketLastHitBase);

	if (!pvPacketDeath)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMDEATH_PACKET_TYPE,
												pvPacketDeath,
												NULL);

	m_csPacket.FreePacket(pvPacketDeath);

	return pvPacket;
}

PVOID AgsmDeath::MakePacketDeathADItem(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bLogin)
{
	if (!pcsItem || !pnPacketLength)
		return NULL;

	AgsdDeathADItem		*pcsDeathADItem = GetADItem(pcsItem);
	if (!pcsDeathADItem)
		return NULL;

	PVOID	pvPacketDeathADItem = m_csPacketDeathADItem.MakePacket(FALSE, pnPacketLength, 0,
																	&pcsItem->m_lID,
																	&pcsDeathADItem->m_lDropRate);

	if (!pvPacketDeathADItem)
		return NULL;

	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGSMDEATH_PACKET_TYPE,
												NULL,
												pvPacketDeathADItem);

	m_csPacket.FreePacket(pvPacketDeathADItem);

	return pvPacket;
}

BOOL AgsmDeath::SendPacketDeath(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	PVOID	pvPacket = MakePacketDeath(pcsCharacter, &nPacketLength, bLogin);

	if (!pvPacket || nPacketLength <= 0)
		return FALSE;

	BOOL	bSendResult = SendPacket(pvPacket, nPacketLength, ulNID);

	m_csPacket.FreePacket(pvPacket);

	return bSendResult;
}

BOOL AgsmDeath::SendPacketDeathADItem(AgpdCharacter *pcsCharacter, UINT32 ulNID, BOOL bLogin)
{
	if (!pcsCharacter || ulNID == 0)
		return FALSE;

	INT16	nPacketLength = 0;
	AgpdGridItem	*pcsGridItem;
	AgpdItemADChar	*pcsItemADChar = m_pagpmItem->GetADCharacter( pcsCharacter );

	int i = 0;
	for (i = 0; i < AGPMITEM_PART_NUM; i++)
	{
		pcsGridItem = m_pagpmItem->GetEquipItem( pcsCharacter, i );

		if ( pcsGridItem == NULL )
			continue;

		AgpdItem *pcsItem = m_pagpmItem->GetItem(pcsGridItem);
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csInventoryGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	i = 0;
	for (pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i);
		 pcsGridItem;
		 pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csBankGrid, &i))
	{
		AgpdItem *pcsItem = m_pagpmItem->GetItem( pcsGridItem );
		if (!pcsItem)
			continue;

		PVOID	pvPacket = MakePacketDeathADItem(pcsItem, &nPacketLength, bLogin);
		if (!pvPacket || nPacketLength <= 0)
			continue;

		SendPacket(pvPacket, nPacketLength, ulNID);

		m_csPacket.FreePacket(pvPacket);
	}

	return TRUE;
}

BOOL AgsmDeath::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	if (!pvPacket ||
		nSize <= 0 ||
		ulNID == 0)
		return FALSE;

	if (!pstCheckArg->bReceivedFromServer)
		return FALSE;

	PVOID		pvPacketDeath			= NULL;
	PVOID		pvPacketDeathADItem		= NULL;

	m_csPacket.GetField(TRUE, pvPacket, nSize,
						&pvPacketDeath,
						&pvPacketDeathADItem);

	if (pvPacketDeath)
	{
		INT32		lCID					= AP_INVALID_CID;
		UINT32		ulDeadTime				= 0;
		INT64		llLastLoseExp			= 0;
		PVOID		pvPacketLastHitBase		= NULL;
		UINT32		ulReceivedClockCount	= 0;

		m_csPacketDeath.GetField(FALSE, pvPacketDeath, 0,
									&lCID,
									&ulDeadTime,
									&llLastLoseExp,
									&pvPacketLastHitBase,
									&ulReceivedClockCount);

		UINT32		ulCurrentClockCount		= GetClockCount();

		AgpdCharacter	*pcsCharacter	= m_pagpmCharacter->GetCharacterLock(lCID);
		if (pcsCharacter)
		{
			AgsdDeath	*pcsDeath	= GetADCharacter(pcsCharacter);
			if (!pcsDeath)
			{
				pcsCharacter->m_Mutex.Release();
				return FALSE;
			}

			pcsDeath->m_ulDeadTime = ulCurrentClockCount + (ulDeadTime - ulReceivedClockCount);
			pcsDeath->m_llLastLoseExp = llLastLoseExp;

			if (pvPacketLastHitBase)
			{
				INT8	cBaseType = 0;
				m_pagsmCharacter->m_csPacketBase.GetField(FALSE, pvPacketLastHitBase, 0,
															&cBaseType,
															&pcsDeath->m_csLastHitBase.m_lID);

				pcsDeath->m_csLastHitBase.m_eType = (ApBaseType) cBaseType;
			}

			if (!pcsCharacter->m_Mutex.Release())
				return FALSE;
		}
	}

	if (pvPacketDeathADItem)
	{
		INT32		lIID					= AP_INVALID_IID;
		INT32		lDropRate				= 0;

		m_csPacketDeathADItem.GetField(FALSE, pvPacketDeathADItem, 0,
									&lIID,
									&lDropRate);

		AgpdItem	*pcsItem	= m_pagpmItem->GetItemLock(lIID);
		if (pcsItem)
		{
			AgpdCharacter	*pcsCharacter = NULL;

			if (pcsItem->m_ulCID != AP_INVALID_CID)
			{
				pcsCharacter = m_pagpmCharacter->GetCharacterLock(pcsItem->m_ulCID);
				if (!pcsCharacter)
				{
					pcsItem->m_Mutex.Release();
					return FALSE;
				}
			}

			AgsdDeathADItem	*pcsDeathADItem = GetADItem(pcsItem);
			if (!pcsDeathADItem)
			{
				if (pcsCharacter)
					pcsCharacter->m_Mutex.Release();
				pcsItem->m_Mutex.Release();
				return FALSE;
			}

			pcsDeathADItem->m_lDropRate = lDropRate;

			if (pcsCharacter)
				pcsCharacter->m_Mutex.Release();

			if (!pcsItem->m_Mutex.Release())
				return FALSE;
		}
	}

	return TRUE;
}

BOOL AgsmDeath::ProcessDropItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return FALSE;

	m_pagsmDropItem2->DropItem(pcsCharacter);

	return TRUE;
}

BOOL AgsmDeath::CBRequestResurrection(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgsmDeath::CBRequestResurrection");

	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmDeath		*pThis			= (AgsmDeath *)			pClass;
	AgpdCharacter	*pCharacter		= (AgpdCharacter *)		pData;
	INT32			lResurrcet		= *((BOOL *)			pCustData);

	//STOPWATCH2(pThis->GetModuleName(), _T("CBRequestResurrection"));

	if (pCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
		return TRUE;

	if (pThis->m_pagpmCharacter->IsAllBlockStatus(pCharacter))
		return TRUE;

	BOOL	bSiegeWar	= FALSE;

	AgpdSiegeWar	*pcsSiegeWar	= pThis->m_pcsAgpmSiegeWar->GetSiegeWarInfo(pCharacter);

	if (pcsSiegeWar &&
		pThis->m_pcsAgpmSiegeWar->IsStarted(pcsSiegeWar) &&
		(pThis->m_pcsAgpmSiegeWar->IsAttackGuild(pCharacter, pcsSiegeWar) || pThis->m_pcsAgpmSiegeWar->IsDefenseGuild(pCharacter, pcsSiegeWar)))
		bSiegeWar	= TRUE;

	if (bSiegeWar && lResurrcet == AGPMCHAR_RESURRECT_NOW)
		return FALSE;

	// apply penalty exp
	INT64	llPenaltyExp = (bSiegeWar) ? 0 : pThis->GetPenaltyExp(pCharacter, lResurrcet == AGPMCHAR_RESURRECT_TOWN ? TRUE : FALSE);

	AgsdDeath	*pcsAgsdDeath	= pThis->GetADCharacter(pCharacter);
	if (pcsAgsdDeath->m_cDeadType == (INT8)AGPMPVP_TARGET_TYPE_MOB)
	{
		if (llPenaltyExp < 0)
			return FALSE;
	}

	if (lResurrcet == AGPMCHAR_RESURRECT_NOW)
	{
		ApmMap::RegionTemplate	*pcsRegionTemplate	= pThis->m_papmMap->GetTemplate(pCharacter->m_nBindingRegionIndex);
		if (pcsRegionTemplate)
		{
			if (pcsRegionTemplate->ti.stType.uSafetyType == ApmMap::ST_DANGER)
			{
				return FALSE;
			}
		}

		if (pThis->m_pagpmCharacter->GetExp(pCharacter) <= 0)
			return FALSE;
	}

	// ���� ��Ȱ ���� ���� Ȯ��
	if ((lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER ||
		 lResurrcet == AGPMCHAR_RESURRECT_SIEGE_OUTER) &&
		!pThis->m_pcsAgpmSiegeWar->IsResurrectableInCastle(pCharacter))
		return FALSE;

	// ���� ������ ��� ��ų�� �����Ų��.
	//pThis->m_pagsmSkill->EndAllBuffedSkill((ApBase *) pCharacter, TRUE);

	// Criminal Flag�� ���õǾ� �ִٸ� �����ش�.
	/*
	if (pCharacter->m_unCriminalStatus == AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED)
	{
		pThis->m_pagpmCharacter->UpdateCriminalStatus(pCharacter, AGPDCHAR_CRIMINAL_STATUS_INNOCENT);
	}
	*/

	BOOL bIsTransform = pCharacter->m_bIsTrasform;

	// �������̸� ������ �ǵ�����.
	if (pCharacter->m_bIsTrasform)
		pThis->m_pagpmCharacter->RestoreTransformCharacter(pCharacter);

	// �����Ǿ� �ִ� ��ġ���� ��Ȱ��Ų��.
	// ��ġ����, ĳ���� Factor ����� ����Ÿ�� ������Ʈ �Ѵ�.
	//
	PVOID pvPacketFactor	= NULL;

	// �⺻ ���� �ɾ��ش�. 2006.01.09. steeple
	pThis->m_pagsmCharacter->SetDefaultInvincible(pCharacter);

	if (!pThis->m_pagsmFactors->Resurrection(&pCharacter->m_csFactor, &pvPacketFactor))
		return FALSE;

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pCharacter);
	pcsAgsdCharacter->m_bIsResurrectingNow = TRUE;

	// �ٽ� ��Ƴ��ٴ� ��Ŷ�� ������.
	BOOL bSendResult = pThis->m_pagsmCharacter->SendPacketFactor(pvPacketFactor, pCharacter, PACKET_PRIORITY_3);

	if (pvPacketFactor)
		pThis->m_pagpmCharacter->m_csPacket.FreePacket(pvPacketFactor);

	// update character status
	pThis->m_pagpmCharacter->UpdateActionStatus(pCharacter, AGPDCHAR_STATUS_NORMAL);

	if (pcsAgsdDeath)
		pcsAgsdDeath->m_bDropItem	= FALSE;

	AuPOS	stResPos;
	ZeroMemory(&stResPos, sizeof(AuPOS));

	if (lResurrcet == AGPMCHAR_RESURRECT_TOWN)
	{
		if (!pThis->m_pagpmEventBinding->GetBindingPositionForResurrection(pCharacter, &stResPos))
		{
			// �̰� ������ ���� ���� �Ͼ�� �ȵǴ� ����̳�.. Ȥ�� �Ͼ�ԵǸ� �ƹ����γ� �ڷ���Ʈ ��ų �� ������
			// �� ���� ��ġ���� ��Ȱ��Ų��.
			stResPos	= pCharacter->m_stPos;
		}

		INT64 llNowExp = pThis->m_pagpmCharacter->GetExp(pCharacter);
		if (llNowExp > 0 && llNowExp - llPenaltyExp < 0)
			llPenaltyExp = llNowExp;

		if (!pThis->m_pagpmConfig->IsEventResurrect() && llNowExp > 0 && llPenaltyExp > 0)
		{
			AgsdDeath	*pcsAgsdDeath = pThis->GetADCharacter((PVOID) pCharacter);
			pcsAgsdDeath->m_llLastLoseExp = llPenaltyExp;

			pThis->m_pagsmCharacter->SubExp(pCharacter, llPenaltyExp);
		}
	}
	else if (lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER ||
			 lResurrcet == AGPMCHAR_RESURRECT_SIEGE_OUTER)
	{
		// 2007.01.25. laki
		BOOL bInner = (lResurrcet == AGPMCHAR_RESURRECT_SIEGE_INNER) ? TRUE : FALSE;
		
		if (!pThis->m_pcsAgpmSiegeWar->SetGuildTargetPosition(pCharacter, &stResPos, bInner))
		{
			// unknown exception, so restore orginal position
			stResPos = pCharacter->m_stPos;
		}
	}
	else
	{
		stResPos	= pCharacter->m_stPos;

		//if (!pcsAgsdDeath->m_bIsDeadByPvP)
		//{
			INT64 llNowExp = pThis->m_pagpmCharacter->GetExp(pCharacter);
			if (llNowExp > 0 && llNowExp - llPenaltyExp < 0)
				llPenaltyExp = llNowExp;

			if (!pThis->m_pagpmConfig->IsEventResurrect() && llNowExp > 0 && llPenaltyExp > 0)
			{
				AgsdDeath	*pcsAgsdDeath = pThis->GetADCharacter((PVOID) pCharacter);
				pcsAgsdDeath->m_llLastLoseExp = llPenaltyExp;

				pThis->m_pagsmCharacter->SubExp(pCharacter, llPenaltyExp);
			}
		//}
	}

	// 2007.07.25. steeple
	// DB �����ϱ� ���� UpdatePosition ���� �Ѵ�.
	//
	// update position
	pThis->EnumCallback(AGSMDEATH_CB_RESURRECTION, pCharacter, &stResPos);

	// 2005.12.16. steeple
	// ���� ��������� üũ �Ǿ� �ִ� ��ų ��ũ�� ĳ�� �������� �������ش�.
	pThis->m_pagsmItem->UseAllEnableCashItem(pCharacter,
											0,
											AGSDITEM_PAUSE_REASON_NONE,
											bIsTransform);	// ��Ȱ �� ���°� ���� ���̾��ٸ�, '�Ͻ� ����' ������ �����۵� ������ش�.
	pThis->m_pagsmSkill->RecastSaveSkill(pCharacter);
	// ���� ĳ���� ����Ÿ�� ��� DB�� �����Ѵ�.
	pThis->m_pagsmCharacter->BackupCharacterData(pCharacter);
	pThis->m_pagsmCharacter->EnumCallback(AGSMCHARACTER_CB_UPDATE_ALL_TO_DB, pCharacter, NULL);

	pcsAgsdCharacter->m_bIsResurrectingNow = FALSE;

	return bSendResult;
}

// 2007.08.24. steeple
BOOL AgsmDeath::CBResurrectionByOther(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AgsmDeath* pThis = static_cast<AgsmDeath*>(pClass);
	PVOID* ppvBuffer = static_cast<PVOID*>(pCustData);

	CHAR* szName = static_cast<CHAR*>(ppvBuffer[0]);
	INT32 lFlag = *static_cast<INT32*>(ppvBuffer[1]);

	AgsdCharacter* pcsAgsdCharacter = pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if(!pcsAgsdCharacter)
		return FALSE;

	// lFlag �� 1�̸� ��Ȱ�ϰڴٰ� �³��� ����.
	// ��Ȱ ó�� ���ش�.
	if(lFlag == 1)
	{
		// Ŭ���̾�Ʈ���� ���� �̸��� ������ ����� �̸��� ���ƾ� �Ѵ�.
		if(_tcsncmp(pcsAgsdCharacter->m_szResurrectionCaster, szName, _tcslen(pcsAgsdCharacter->m_szResurrectionCaster)) == 0)
		{
			pThis->ResurrectionNowNoPenalty(pcsCharacter);
		}
	}

	ZeroMemory(pcsAgsdCharacter->m_szResurrectionCaster, sizeof(pcsAgsdCharacter->m_szResurrectionCaster));
	return TRUE;
}

// 2005.10.14. steeple
BOOL AgsmDeath::CBEndPeriodStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsSummons = (AgpdCharacter*)pData;
	AgsmDeath* pThis = (AgsmDeath*)pClass;

	pThis->SetDead(pcsSummons);

	return TRUE;
}

// 2004.04.29. steeple
BOOL AgsmDeath::WriteDeathLog(AgpdCharacter* pAgpdCharacter)
{
	// final attacker based
	if (!m_pagpmLog)
		return FALSE;
	
	AgsdCharacter *pAgsdCharacter = m_pagsmCharacter->GetADCharacter(pAgpdCharacter);
	if (!pAgsdCharacter)
		return FALSE;

	INT32 lLevel = m_pagpmCharacter->GetLevel(pAgpdCharacter);
	INT64 llExp = m_pagpmCharacter->GetExp(pAgpdCharacter);

	CHAR szEnemy[AGPDCHARACTER_MAX_ID_LENGTH + 1];
	CHAR szDesc[128];
	
	ZeroMemory(szEnemy, sizeof(szEnemy));
	ZeroMemory(szDesc, sizeof(szDesc));
	
	AgpdCharacter *pEnemyCharacter = NULL;
	AgsdCharacterHistoryEntry *pcsEntry = m_pagsmCharacter->GetFinalAttacker(pAgpdCharacter);
	if (pcsEntry)
	{
		ApBase *pcsFinalAttacker = m_papmEventManager->GetBase(pcsEntry->m_csSource.m_eType, pcsEntry->m_csSource.m_lID);
		if (pcsFinalAttacker && pcsFinalAttacker->m_eType == APBASE_TYPE_CHARACTER)
		{
			pEnemyCharacter = (AgpdCharacter *) pcsFinalAttacker;

			if (m_pagpmCharacter->IsStatusSummoner(pEnemyCharacter)
				|| m_pagpmCharacter->IsStatusTame(pEnemyCharacter)
				|| m_pagpmCharacter->IsStatusFixed(pEnemyCharacter))
			{
				AgpdCharacter *pTemp = NULL;
				if (m_pagpmSummons)
					pTemp = m_pagpmCharacter->GetCharacter(m_pagpmSummons->GetOwnerCID(pEnemyCharacter));
				pEnemyCharacter = pTemp ? pTemp : pEnemyCharacter;
			}

			if (m_pagpmCharacter->IsPC(pEnemyCharacter))
			{
				_tcscpy(szEnemy, pEnemyCharacter->m_szID);
			}
			else
			{
				_tcscpy(szEnemy, pEnemyCharacter->m_pcsCharacterTemplate ? 
								pEnemyCharacter->m_pcsCharacterTemplate->m_szTName :
								_T("@Unknown"));
			}
		}
	}

	CHAR szPosition[33];
	ZeroMemory(szPosition, sizeof(szPosition));
	this->m_pagsmCharacter->EncodingPosition(&pAgpdCharacter->m_stPos, szPosition, 32);	

	AgsdDeath *pAgsdDeath = GetADCharacter(pAgpdCharacter);
	if (pAgsdDeath)
	{
		switch (pAgsdDeath->m_cDeadType)
		{
			case AGPMPVP_TARGET_TYPE_MOB :
				_stprintf(szDesc, _T("Monster (%s)"), szPosition);
				break;			
			case AGPMPVP_TARGET_TYPE_NORMAL_PC :
				_stprintf(szDesc, _T("Player  (%s)"), szPosition);
				break;			
			case AGPMPVP_TARGET_TYPE_ENEMY_GUILD :
				_stprintf(szDesc, _T("Guild  (%s)"), szPosition);
				break;
		}
	}		

	m_pagpmLog->WriteLog_CharDeath(0, &pAgsdCharacter->m_strIPAddress[0],
									pAgsdCharacter->m_szAccountID,
									pAgsdCharacter->m_szServerName,
									pAgpdCharacter->m_szID,
									pAgpdCharacter->m_pcsCharacterTemplate->m_lID,
									lLevel,
									llExp,
									pAgpdCharacter->m_llMoney,
									pAgpdCharacter->m_llBankMoney,
									szEnemy,
									szDesc,
									NULL // guild
									);

	return TRUE;
}


BOOL AgsmDeath::SetCallbackCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_CHARACTER_DEAD, pfCallback, pClass);
}

BOOL AgsmDeath::SetCallbackPreProcessCharacterDead(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_PRE_PROCESS_CHARACTER_DEAD, pfCallback, pClass);
}

BOOL AgsmDeath::SetCallbackResurrection(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMDEATH_CB_RESURRECTION, pfCallback, pClass);
}
