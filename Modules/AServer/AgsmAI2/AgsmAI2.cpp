#include "AgsmAI2.h"
#include "ApAutoLockCharacter.h"
#include "AgpmBattleGround.h"
#include "AgpmMonsterPath.h"
#include "AgppCharacter.h"
#include <math.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const float			AGSMAI2_Radian				= 3.141592f/180.0f;
const float			AGSMAI2_Visibility			= 2500.0f;
const float			AGSMAI2_Summons_Visibility	= 1500.0f;		// ��ȯ�� �þߴ� 15����. 2005.10.28. steeple
const float			AGSMAI2_Summons_Max_Follow	= 2000.0f;		// ��ȯ���� ���ο��� ������ ���󰡰Բ� �ϴ� �Ÿ�. 2007.07.25. steeple
const float			AGSMAI2_Summons_Max_Jump	= 2500.0f;		// ��ȯ���� ���ο��� ������ �����ǰԲ� �ϴ� �Ÿ�. 2007.07.25. steeple
const float			AGSMAI2_Pet_Max_Jump		= 1500.0f;		// ���� ���ο��� ������ �����ǰԲ� �ϴ� �Ÿ�.

AgsmAI2::AgsmAI2()
{
	SetModuleName("AgsmAI2");
	m_lSummonAOIRange = 500;
	m_lPetAOIRange = 200;
	m_lPetAOIRange2 = 300;
	
	m_pagpmBattleGround	= NULL;
}

AgsmAI2::~AgsmAI2()
{

}

BOOL			AgsmAI2::OnAddModule()
{
	// Parent Module�� ��������
	m_pcsApmMap			= (ApmMap *) GetModule("ApmMap");
	m_pcsAgpmPathFind	= (AgpmPathFind *) GetModule("AgpmPathFind");
	m_pcsAgpmFactors	= (AgpmFactors *) GetModule("AgpmFactors");
	m_pcsAgsmFactors	= (AgsmFactors *) GetModule("AgsmFactors");
	m_pcsApmObject		= (ApmObject *) GetModule("ApmObject");
	m_pcsAgpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem		= (AgpmItem *) GetModule("AgpmItem");
	m_pcsAgpmSummons	= (AgpmSummons *) GetModule("AgpmSummons");
	m_pcsAgpmSkill		= (AgpmSkill *) GetModule("AgpmSkill");
	m_pcsAgpmPvP		= (AgpmPvP *) GetModule("AgpmPvP");
	m_pcsAgpmAI2		= (AgpmAI2 *) GetModule("AgpmAI2");
	m_pcsAgpmEventNPCDialog = (AgpmEventNPCDialog *) GetModule( "AgpmEventNPCDialog" );
	m_pcsApmEventManager= (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmEventSpawn	= (AgpmEventSpawn *) GetModule("AgpmEventSpawn");
	m_pcsAgsmEventSpawn	= (AgsmEventSpawn *) GetModule("AgsmEventSpawn");
	m_pcsAgsmCharacter	= (AgsmCharacter *) GetModule("AgsmCharacter");
	m_pcsAgsmItem		= (AgsmItem *) GetModule("AgsmItem");
	m_pcsAgsmSkill		= (AgsmSkill *) GetModule("AgsmSkill");
	m_pcsAgsmItemManager= (AgsmItemManager *) GetModule("AgsmItemManager");
	m_pcsAgsmChatting	= (AgsmChatting *) GetModule("AgsmChatting");
	m_pcsAgsmCombat		= (AgsmCombat *) GetModule("AgsmCombat" );
	m_pcsAgsmSummons	= (AgsmSummons *) GetModule("AgsmSummons");
	m_pcsAgpmSiegeWar	= (AgpmSiegeWar *) GetModule("AgpmSiegeWar");

	// Character���� Public AI ������ �ϴ� OK
	if ( !m_pcsApmMap || !m_pcsAgpmPathFind || !m_pcsAgpmCharacter || !m_pcsAgpmItem ||
		!m_pcsAgpmSummons || !m_pcsAgpmPvP || !m_pcsAgpmSkill ||
		!m_pcsAgpmAI2 || !m_pcsAgpmEventNPCDialog || !m_pcsAgsmCharacter || !m_pcsAgsmItem ||
		!m_pcsAgsmSkill || !m_pcsAgsmItemManager || !m_pcsAgsmChatting || !m_pcsAgsmCombat ||
		!m_pcsAgsmFactors || !m_pcsAgsmSummons ||
		!m_pcsAgpmSiegeWar
		)
		return FALSE;

	// Update Character Callback ����ϰ�
	if (!m_pcsAgpmCharacter->SetCallbackUpdateChar(CBUpdateCharacter, this))
		return FALSE;

	// MonsterPathFind���� Callback ���
	if (!m_pcsAgpmCharacter->SetCallbackMonsterAIPathFind( CBPathFind , this ))
		return FALSE;

	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackSpawn(CBSpawnCharacter, this))
		return FALSE;
	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackSpawnUsedData(CBSpawnUsedCharacter, this))
		return FALSE;
	if (m_pcsAgsmEventSpawn && !m_pcsAgsmEventSpawn->SetCallbackInitReusedCharacter(CBInitReusedCharacter, this))
		return FALSE;

	if( !m_pcsAgsmCombat->SetCallbackNPCDialog( CBNPCChatting, this ) )
		return FALSE;

	if(!m_pcsAgsmCombat->SetCallbackAttackStart(CBAttackStart, this))
		return FALSE;

/*	if (m_pcsAgsmCharacter && !m_pcsAgsmCharacter->SetCallbackSendCharacterAllServerInfo(CBSendCharacterAllInfo, this))
		return FALSE;*/

	if (!m_pcsAgpmCharacter->SetCallbackSetCombatMode(CBSetCombatMode, this))
		return FALSE;
	if (!m_pcsAgpmCharacter->SetCallbackResetCombatMode(CBResetCombatMode, this))
		return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackCheckActionAttackTarget(CBCheckActionAttackTarget, this))
	//	return FALSE;
	//if (!m_pcsAgpmCharacter->SetCallbackRemoveChar(CBRemoveCharacter, this))
	//	return FALSE;
	if (!m_pcsAgsmSkill->SetCallbackAffectedSkill(CBAffectedSkill, this))
		return FALSE;

	if(!m_pcsAgpmCharacter->SetCallbackPinchWantedRequest(CBPinchWantedRequest, this))
		return FALSE;

	return TRUE;
}	

BOOL AgsmAI2::OnInit()
{
	m_pagpmBattleGround	= (AgpmBattleGround*)GetModule("AgpmBattleGround");
	m_pagpmMonsterPath = (AgpmMonsterPath*)GetModule("AgpmMonsterPath");
	
	if(!m_pagpmBattleGround || !m_pagpmMonsterPath)
		return FALSE;
	
	return TRUE;
}

BOOL AgsmAI2::CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	INT32				lAI2ID = *((INT32 *)pvCustData);

	if( !pcsCharacter || !pThis )
		return FALSE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBSpawnCharacter"));

	AgpdAI2ADChar			*pcsAgpdAI2ADChar;

	pcsAgpdAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData( pcsCharacter );

	if( pcsAgpdAI2ADChar != NULL )
	{
		AgpdAI2Template			*pcsAI2Template;

		pcsAgpdAI2ADChar->m_csTargetPos = pcsCharacter->m_stPos;
		pcsAgpdAI2ADChar->m_csFirstSpawnPos = pcsCharacter->m_stPos;
		pcsAI2Template = pThis->m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( lAI2ID );
		//pcsCharacter->m_pcsCharacterTemplate->m_eCharType = AGPDCHAR_TYPE_MONSTER;

		if( pcsAI2Template == NULL )
		{
			//ǥ�� 1�� AI�� �޾Ƴ��� ����Ʈ �������Ѵ�.
			pcsAI2Template = pThis->m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( 1 );
		}

		if( pcsAI2Template != NULL )
		{
			ApdEvent			*pcsEvent;
			AuMATRIX			*pstDirection;

			pcsEvent = NULL;

			pcsEvent = pThis->m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter)->m_pstEvent;

			if( pcsEvent )
			{
				pcsAgpdAI2ADChar->m_csSpawnBasePos = *pThis->m_pcsApmEventManager->GetBasePos( pcsEvent->m_pcsSource, &pstDirection );
				pcsAgpdAI2ADChar->m_fSpawnRadius = pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

				pcsAgpdAI2ADChar->m_lAttackInterval = (INT32)pThis->m_pcsAgpmCharacter->GetAttackIntervalMSec( pcsCharacter );
				pcsAgpdAI2ADChar->m_bUseAI2 = TRUE;
				pcsAgpdAI2ADChar->m_pcsAgpdAI2Template = pcsAI2Template;

				pcsAgpdAI2ADChar->m_lWrapPosition = pThis->m_csRand.randInt(360);

				//2004.10.25�� ���� ��� ���յǸ鼭 �ּ�ó����.
				//�����ϸ� ��縦 ��������.
//				pThis->ProcessNPCDialog( (ApBase *)pcsCharacter, AGPD_NPCDIALOG_MOB_SPAWN );

				// 2005.09.06. steeple
				// Spawn �� �Ŀ� �ٷ� Cast �ؾ� �� ��ų�� ������ Cast ���ش�.
				pThis->ProcessSpawnCastSkill(pcsCharacter);
			}
		}
	}

	return TRUE;
}

BOOL AgsmAI2::InitReusedCharacterAI(AgpdCharacter *pcsCharacter, AgpdAI2Template *pcsAgpdAI2Template)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdAI2ADChar			*pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsCharacter );

	if (pcsAgpdAI2Template)
		pcsAgpdAI2ADChar->m_pcsAgpdAI2Template	= pcsAgpdAI2Template;
	else
	{
		//ǥ�� 1�� AI�� �޾Ƴ��� ����Ʈ �������Ѵ�.
		pcsAgpdAI2ADChar->m_pcsAgpdAI2Template	= m_pcsAgpmAI2->m_aAI2Template.GetAITemplate( 1 );
	}

	pcsAgpdAI2ADChar->m_ulLastAttackTime	= 0;

	pcsAgpdAI2ADChar->m_ulPrevProcessTime	= 0;
	pcsAgpdAI2ADChar->m_ulProcessInterval	= 0;
	pcsAgpdAI2ADChar->m_ulLastPathFindTime	= 0;

	pcsAgpdAI2ADChar->m_ulNextWanderingTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDialogTargetPC	= 0;

	pcsAgpdAI2ADChar->m_csItemResult.MemSetAll();
	pcsAgpdAI2ADChar->m_csSkillResult.MemSetAll();

	pcsAgpdAI2ADChar->m_bScreamUsed	= FALSE;
	pcsAgpdAI2ADChar->m_ulScreamStartTime	= 0;

	pcsAgpdAI2ADChar->m_ulNextDecStaticTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDecSlowTime	= 0;
	pcsAgpdAI2ADChar->m_ulNextDecFastTime	= 0;

	pcsAgpdAI2ADChar->m_csTargetPos	= pcsCharacter->m_stPos;
	pcsAgpdAI2ADChar->m_csFirstSpawnPos = pcsCharacter->m_stPos;


	ApdEvent			*pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsCharacter)->m_pstEvent;

	if( pcsEvent )
	{
		AuMATRIX			*pstDirection;
		pcsAgpdAI2ADChar->m_csSpawnBasePos = *m_pcsApmEventManager->GetBasePos( pcsEvent->m_pcsSource, &pstDirection );
		pcsAgpdAI2ADChar->m_fSpawnRadius = pcsEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius;

		pcsAgpdAI2ADChar->m_lAttackInterval = (INT32) m_pcsAgpmCharacter->GetAttackIntervalMSec( pcsCharacter );
		pcsAgpdAI2ADChar->m_bUseAI2 = TRUE;

	}

	pcsAgpdAI2ADChar->m_lTargetID	= AP_INVALID_CID;
	pcsAgpdAI2ADChar->m_lReadyTargetID	= AP_INVALID_CID;

	pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

	pcsAgpdAI2ADChar->m_pcsBossCharacter	= NULL;

	pcsAgpdAI2ADChar->m_pcsFollower.MemSetAll();

	pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower	= 0;
	pcsAgpdAI2ADChar->m_lCurrentFollowers	= 0;

	pcsAgpdAI2ADChar->m_lCastSkillLevel = 0;

	// 2005.09.06. steeple
	// Spawn �� �Ŀ� �ٷ� Cast �ؾ� �� ��ų�� ������ Cast ���ش�.
	ProcessSpawnCastSkill(pcsCharacter);

	return TRUE;
}

BOOL AgsmAI2::CBSpawnUsedCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvCustData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;

	if( !pcsCharacter || !pThis )
		return FALSE;

	return pThis->InitReusedCharacterAI(pcsCharacter, NULL);
}

BOOL AgsmAI2::CBInitReusedCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pData;
	AgsmAI2				*pThis = (AgsmAI2 *) pClass;

	if( !pcsCharacter || !pThis )
		return FALSE;

	return pThis->InitReusedCharacterAI(pcsCharacter, (AgpdAI2Template *) pCustData);
}

BOOL AgsmAI2::CBNPCChatting(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	ApBase				*pcsApBase = (ApBase *)pvData;
	eAgpdEventNPCDialogMob eDialogType = *((eAgpdEventNPCDialogMob *)pvCustData);

	BOOL				bResult;

	bResult = FALSE;

	if( pcsApBase )
	{
		bResult = pThis->ProcessNPCDialog( pcsApBase, eDialogType );
	}

	return bResult;
}

BOOL AgsmAI2::ProcessNPCDialog( ApBase *pcsApBase, eAgpdEventNPCDialogMob eDialogType )
{
	BOOL			bResult;
	bResult = FALSE;

	if( pcsApBase )
	{
		char			*pstrDialogText = NULL;
		INT32			lDialogLength = 0;

		//ĳ���Ͷ��......
		if( pcsApBase->m_eType == APBASE_TYPE_CHARACTER )
		{
			INT32			lNPCTID = ((AgpdCharacter *)pcsApBase)->m_pcsCharacterTemplate->m_lID;

			pstrDialogText = m_pcsAgpmEventNPCDialog->GetMobDialog( lNPCTID, eDialogType );

			if( pstrDialogText )
			{
				lDialogLength = (INT32)_tcslen( pstrDialogText );
				bResult = m_pcsAgsmChatting->SendMessageNormal( pcsApBase, pstrDialogText, lDialogLength );
			}
		}
	}
	
	return bResult;
}

BOOL AgsmAI2::CheckCondition( AgpdAI2ConditionTable	*pcsConditionTbl, AgpdAI2UseResult *pcsUseResult, UINT32 lClockCount )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_TIMER )
	{
		if( (UINT32)(pcsUseResult->m_lClock + pcsConditionTbl->m_lTimerCount) < lClockCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_COUNT )
	{
		if( pcsUseResult->m_lUsedCount < pcsConditionTbl->m_lMaxUsableCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}

	else if( pcsConditionTbl->m_lConditionCheck == AGPMAI2_CONDITION_INFINITY )
	{
		if( (UINT32)(pcsUseResult->m_lClock + pcsConditionTbl->m_lTimerCount) < lClockCount )
		{
			pcsUseResult->m_lUsedCount++;
			pcsUseResult->m_lClock = lClockCount;
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmAI2::CheckParameter( INT32 lParameter, INT32 lMaxParameter, AgpdAI2ConditionTable *pcsConditionTbl )
{
	INT32			lScalar;
	BOOL			bResult;

	bResult = FALSE;

	if( pcsConditionTbl->m_bPercent )
	{
		if( lMaxParameter > 0 )
		{
			lScalar =  (lParameter*100)/lMaxParameter;
		}
		else
		{
			return bResult;
		}
	}
	else
	{
		lScalar = lParameter;
	}

	if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_EQUAL )
	{
		if( lScalar == pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_NOT_EQUAL )
	{
		if( lScalar != pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_BIGGER )
	{
		if( lScalar > pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_BIGGER_EQUAL )
	{
		if( lScalar >= pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_LESS )
	{
		if( lScalar < pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}
	else if( pcsConditionTbl->m_lOperator == AGPMAI2_OPERATOR_LESS_EQUAL )
	{
		if( lScalar <= pcsConditionTbl->m_lParameter )
		{
			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL AgsmAI2::SetTargetPos( AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS *pcPos )
{
	BOOL			bResult;

	bResult = FALSE;

	if( pcsAgpdAI2ADChar != NULL )
	{
		pcsAgpdAI2ADChar->m_csTargetPos = *pcPos;
		bResult = TRUE;
	}

	return bResult;
}

BOOL AgsmAI2::GetTargetPosFromSpawn(AgpdCharacter* pcsCharacter, AuPOS *pcsCurrentPos, AuPOS *pcsTargetPos, AuPOS *pcsSpawnBasePos, float fRadius )
{
	FLOAT	fX, fZ;
	FLOAT	fTempX, fTempZ;

	FLOAT	fTempRadius;
	AuPOS	stTempAuPos;
	BOOL	bResult;

	//10���͸� �̵��ϴ� �Ÿ��� ��´�.
	bResult = FALSE;
	fTempRadius = 1000.0f; 

	for (INT32 nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
	{
		fTempX = (FLOAT)m_csRand.rand((double)(fTempRadius * 2.0f)) - fTempRadius;
		fTempZ = (FLOAT)m_csRand.rand((double)(fTempRadius * 2.0f)) - fTempRadius;

		fX = (pcsCurrentPos->x + fTempX) - pcsSpawnBasePos->x;
		fZ = (pcsCurrentPos->z + fTempZ) - pcsSpawnBasePos->z;

		if(!IsRunawayStateCharacter(pcsCharacter))
		{
			//�������� ���̸� �׳� ��������.
			if (fX*fX + fZ*fZ > fRadius * fRadius )
			{
				continue;
			}
		}

		stTempAuPos.x = pcsCurrentPos->x + fTempX;
		stTempAuPos.z = pcsCurrentPos->z + fTempZ;

		//���� ���ٸ� ��! �ѽξ�~
		if (m_pcsApmMap->CheckBlockingInfo(stTempAuPos , ApmMap::GROUND ) != ApTileInfo::BLOCKGEOMETRY)
		{
			pcsTargetPos->x = stTempAuPos.x;
			pcsTargetPos->z = stTempAuPos.z;

			bResult = TRUE;
			break;
		}
	}

	return bResult;
}

BOOL AgsmAI2::CBUpdateCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	PROFILE("AgsmAI2::CBUpdateCharacter");

	AgsmAI2 *			pThis = (AgsmAI2 *) pvClass;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvData;
	AgpdAI2ADChar *		pstAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	UINT32				ulClockCount = PtrToUint(pvCustData);

	if( AGPDCHAR_STATUS_DEAD == pcsCharacter->m_unActionStatus )
		return TRUE;

	if (!pcsCharacter->m_bIsAddMap)
		return TRUE;

	//STOPWATCH2(pThis->GetModuleName(), _T("CBUpdateCharacter"));

	// 2005.10.05. steeple. Fixed �߰�
	// 2005.08.03. steeple. ����
	if(pThis->m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter) || pThis->m_pcsAgpmCharacter->IsStatusTame(pcsCharacter) ||
		pThis->m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter))
	{
		if (!pThis->ProcessSummonAI(pcsCharacter, ulClockCount))
		{
			//if (pThis->m_pcsAgpmCharacter->IsNPC(pcsCharacter) || pcsCharacter->m_bIsProtectedNPC)
			//	AuLogFile("RemoveNPC.log", "Removed by AI2 (ProcessSummonAI() is failed)\n");

			pThis->m_pcsAgpmCharacter->RemoveCharacter(pcsCharacter->m_lID);
			return FALSE;
		}

		pThis->m_pcsAgsmSummons->CheckSummonsPeriod(pcsCharacter, ulClockCount);
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);

		return TRUE;
	}	

	if( NULL == pstAI2ADChar )
		return FALSE;

	// Interval��ŭ ����������, return
	if (pstAI2ADChar->m_ulPrevProcessTime + pstAI2ADChar->m_ulProcessInterval > ulClockCount)
		return TRUE;

	eAgpmAI2StatePCAI eResult = AGPDAI2_STATE_PC_AI_NONE;

	// PC SKILL AI�� �ִ� ����� PC AI�� �������ش�.
	if(TRUE == pThis->ProcessPCAI(pcsCharacter, pstAI2ADChar, ulClockCount, &eResult))
		return TRUE;

	if( FALSE == pstAI2ADChar->m_bUseAI2 )
		return TRUE;

	if(NULL == pstAI2ADChar->m_pcsAgpdAI2Template)
		return TRUE;

	//���� Ÿ���� �ְų� NPC Guard�� ���� ��������~�ϰ� �����δ�.
	if( pstAI2ADChar->m_lTargetID != 0 || pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type == AGPMAI2_TYPE_FIXED_NPC )
	{

	}
	//Ÿ���� ������ ���~
	else
	{
		if (!pThis->m_pcsAgsmCharacter->IsIdleProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, ulClockCount))
			return TRUE;
	}

	pThis->m_pcsAgsmCharacter->ResetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI);

	if (pThis->m_pcsAgpmCharacter->IsCombatMode(pcsCharacter) || pstAI2ADChar->m_lTargetID != 0)
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	else if (pstAI2ADChar->m_pcsAgpdAI2Template->m_lAgressivePoint != 0)
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_ONE_HALF_SECOND);
	else
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);

	pThis->m_pcsAgsmCharacter->SetProcessTime(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, ulClockCount);

	pstAI2ADChar->m_ulPrevProcessTime = ulClockCount;

	if( (AGPMAI2_TYPE_PATROL_NPC == pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type) || 
		(AGPMAI2_TYPE_FIXED_NPC == pstAI2ADChar->m_pcsAgpdAI2Template->m_eNPCAI2Type) )
	{
		//NPC�� ���õ� AI�� �����Ѵ�.
		pThis->ProcessGuardNPCAI(pcsCharacter, pstAI2ADChar, ulClockCount);
	}
	else if( pThis->m_pcsAgpmCharacter->IsMonster(pcsCharacter) || pThis->m_pcsAgpmCharacter->IsCreature(pcsCharacter) ||
		pThis->m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsCharacter) )
	{
		//Mob�� ���õ� AI�� �����Ѵ�.
		//pThis->ProcessAI(pcsCharacter, pstAI2ADChar, ulClockCount);
		pThis->ProcessMonsterAI(pcsCharacter, pstAI2ADChar, ulClockCount);
	}

	return TRUE;
}

BOOL AgsmAI2::CBPathFind(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	AgpdCharacter		*pcsCharacter = (AgpdCharacter *) pvData;
	AgsmAI2				*pThis = (AgsmAI2 *) pvClass;
	AgpdAI2ADChar		*pstAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	BOOL				*pbIgnoreBlocking	= (BOOL *)	pvCustData;

	if (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsCharacter->m_ulSpecialStatus)
		return TRUE;

	if (!*pbIgnoreBlocking && (
		pstAI2ADChar->m_lTargetID == AP_INVALID_CID ||
		pstAI2ADChar->m_bIsNeedPathFind))
		return TRUE;

	AgpdCharacter		*pcsTargetChar	= pThis->m_pcsAgpmCharacter->GetCharacter(pstAI2ADChar->m_lTargetID);
	if (!pcsTargetChar)
		return TRUE;

	if (pThis->ProcessCheckTargetArea(pcsCharacter, pcsTargetChar))
		return TRUE;

	pstAI2ADChar->m_bIsNeedPathFind	= TRUE;

	//�������� ���� ã�Ƶд�.
	BOOL bResult = pThis->m_pcsAgpmPathFind->pathFind(  pcsCharacter->m_stPos.x,
											pcsCharacter->m_stPos.z,
											pcsTargetChar->m_stPos.x,
											pcsTargetChar->m_stPos.z,
											pstAI2ADChar->m_pclPath );

	if (!bResult || *pbIgnoreBlocking)
	{
		pcsCharacter->m_ulCheckBlocking		= 0;
		pstAI2ADChar->m_bIsNeedPathFind	= FALSE;
	}

	//
	CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

	pcsNode = pstAI2ADChar->m_pclPath->GetNextPath();

	if( pcsNode )
	{
		AuPOS			csPos;

		csPos.x = pcsNode->data->m_fX;
		csPos.y = 0.0f;
		csPos.z = pcsNode->data->m_fY;

		pThis->m_pcsAgpmCharacter->MoveCharacter( pcsCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
	}

	return TRUE;
}

void AgsmAI2::ProcessCommonAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	if( NULL == pcsAgpdCharacter )
		return;

	//STOPWATCH2(GetModuleName(), _T("ProcesssCommonAI"));

	AgpdCharacterTemplate	*pcsTemplate;
	INT32					lMaxHP;
	INT32					lDecreaseScalar;

	pcsTemplate = pcsAgpdCharacter->m_pcsCharacterTemplate;

	if( pcsAgpdAI2ADChar->m_ulNextDecSlowTime < lClockCount )
	{
		//1/1000�� �����̹Ƿ� 1000�� ���Ѵ�. �׳� 1/1000������ �Է��� �޶�� �ϴ°� ���ڱ�.
		pcsAgpdAI2ADChar->m_ulNextDecSlowTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lSlowTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lSlowPercent)/100;

		if( lDecreaseScalar == 0 )
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory( pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseScalar  );
	}

	if( pcsAgpdAI2ADChar->m_ulNextDecFastTime < lClockCount )
	{
		//1/1000�� �����̹Ƿ� 1000�� ���Ѵ�. �׳� 1/1000������ �Է��� �޶�� �ϴ°� ���ڱ�.
		pcsAgpdAI2ADChar->m_ulNextDecFastTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lFastTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lFastPercent)/100;

		if( lDecreaseScalar == 0 )
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory( pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_FAST, lDecreaseScalar  );
	}

	//�������� ��� �ֺ��� ���� �ڽ��� ���Ϸ� ����� �ִ�.
	if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) <= 0 )
		return;

	if( m_pcsAgpmAI2->GetCumulativeFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower )
		return;

	AgpdAI2ADChar		*pcsAgpdAI2TempADChar;
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	alCID.MemSetAll();
	alCID2.MemSetAll();

	INT32				lNumCID;
	INT32				lCounter;
	float				fVisibility = 2000.0f;
	float				fTempX, fTempZ;

	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fVisibility,
										&alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										&alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	AgpdCharacter	*pcsFollowerCharacter	= NULL;

	//���ϵ��߿� ������� �̻� �ָ� �������� ����Ʈ���� �����Ѵ�.
	for( lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; ++lCounter )
	{
		if( NULL == pcsAgpdAI2ADChar->m_pcsFollower[lCounter] )
			continue;

		// 2007.03.05. steeple
		if(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_bIsReadyRemove)
			continue;

		fTempX = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		//�þ� ������ �ڽ��� ���ϰ� ������ٸ� ����~
		if( fTempX*fTempX + fTempZ*fTempZ > fVisibility*fVisibility )
		{
			pcsFollowerCharacter	= pcsAgpdAI2ADChar->m_pcsFollower[lCounter];

			pcsAgpdAI2TempADChar = m_pcsAgpmAI2->GetCharacterData( pcsFollowerCharacter );

			if( pcsAgpdAI2TempADChar )
			{
				if (pcsFollowerCharacter->m_Mutex.WLock())
				{
					//������ ������ �����ְ�...
					pcsAgpdAI2TempADChar->m_pcsBossCharacter = NULL;

					//������ ���ϸ���Ʈ���� �����Ѵ�.
					pcsAgpdAI2ADChar->m_pcsFollower[lCounter] = NULL;
					pcsAgpdAI2ADChar->m_lCurrentFollowers--;

					pcsFollowerCharacter->m_Mutex.Release();
				}
			}
		}	
	}

	for (int i = 0; i < lNumCID; ++i)
	{
		AgpdCharacter	*pcsListCharacter	= m_pcsAgpmCharacter->GetCharacter(alCID2[i]);
		AgpdAI2ADChar	*pcsAI2ADFollowerChar; 
		INT32			lFollowerTID;

		if (!pcsListCharacter)
			continue;

		if (pcsListCharacter->m_bIsReadyRemove)
			continue;

		//�ڱ� �ڽ��� ����.
		if (pcsListCharacter->m_lID == pcsAgpdCharacter->m_lID )
			continue;

		for( INT32 lFollowerCount=0; lFollowerCount<AGPDAI2_MAX_FOLLOWER_COUNT; ++lFollowerCount )
		{
			lFollowerTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFollower.m_lFollowerInfo[lFollowerCount].m_lFollowerTID;

			if( lFollowerTID )
			{
				if( pcsListCharacter->m_pcsCharacterTemplate->m_lID == lFollowerTID )
				{
					if( pcsAgpdAI2ADChar->m_lCurrentFollowers >= m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) )
					{
						break;
					}

					pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

					//������ ���� ���� ��츸 �ڽ��� ���Ϸ� ����� �ִ�.
					if( pcsAI2ADFollowerChar && pcsAI2ADFollowerChar->m_pcsBossCharacter == NULL )
					{
						BOOL				bExist;

						bExist = FALSE;

						for( int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j )
						{
							if( pcsAgpdAI2ADChar->m_pcsFollower[j] == pcsListCharacter )
							{
								bExist = TRUE;
								break;
							}
						}

						if( bExist == FALSE )
						{
							if (!pcsListCharacter->m_Mutex.WLock())
								break;

							pcsAI2ADFollowerChar->m_pcsBossCharacter = pcsAgpdCharacter;

							pcsListCharacter->m_Mutex.Release();

							for(int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k )
							{
								if( pcsAgpdAI2ADChar->m_pcsFollower[k] == NULL )
								{
									pcsAgpdAI2ADChar->m_pcsFollower[k] = pcsListCharacter;
									pcsAgpdAI2ADChar->m_lCurrentFollowers++;
									pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower++;
									break;
								}
							}
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}
}

AgpdCharacter* AgsmAI2::GetTargetPC( AgpdCharacter *pcsAgpdMobCharacter, AgpdAI2ADChar *pcsAI2ADChar, AuPOS csMobPos, float fPreemptiveRange )
{
	//���� ũ���ĸ� �ٷ� �����Ѵ�.
	if( m_pcsAgpmCharacter->IsCreature(pcsAgpdMobCharacter) )
	{
		return NULL;
	}

	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	NearCharList;
	NearCharList.MemSetAll();
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		NearCharIDList;
	NearCharIDList.MemSetAll();

	INT32	lNumCID	= 0;

	// Ÿ���� ���� ������ �꿡 ���� �������� ����� ���� ���غ���. ���⼭ �������� �Ѿ�� ����� Ÿ������ ó������.
	PVOID	pvBuffer[3];
	pvBuffer[0]	= (PVOID) pcsAgpdMobCharacter;
	pvBuffer[1]	= (PVOID) &NearCharList[0];
	pvBuffer[2]	= (PVOID) &lNumCID;

	float	fVisibility = AGSMAI2_Visibility;

	if(pcsAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility > 0.0f)
	{
		fVisibility = pcsAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility;
	}

	EnumCallback(AGSMAI2_CB_GET_TARGET, pvBuffer, NULL);

	if (lNumCID <= 0)
	{
		//�ֺ��� PC�� �ֳ�����.
		lNumCID = m_pcsApmMap->GetCharList(pcsAgpdMobCharacter->m_nDimension, APMMAP_CHAR_TYPE_PC | APMMAP_CHAR_TYPE_MONSTER, csMobPos,
												fVisibility, &NearCharList[0], AGSMAI2_MAX_PROCESS_TARGET,
												&NearCharIDList[0], AGSMAI2_MAX_PROCESS_TARGET);
	}
	else
		return (AgpdCharacter *) NearCharList[0];

	//��(����) ���� ������ �ִ°��.
	if( lNumCID <= 0 )
		return NULL;

	INT32 lMaxDamage = 0;
	AgpdAI2ADChar	*pcsTempAI2ADChar = NULL;
	AgpdCharacter	*pcsTargetCharacter	= NULL;
	AgpdCharacter	*pcsAgroTargetCharacter = NULL;

	float			fAgroDistance = 0.0f;
	INT32			lMostAgro = 0;

	for (int i = 0; i < lNumCID; ++i)
	{
		// 2007.02.27. steeple
		// CID �迭�� ���� �ٽ� Character �� �����Բ� ����.
		AgpdCharacter* pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(NearCharIDList[i]);
		if (!pcsListCharacter)
			continue;

		// 2007.03.05. steeple
		if (pcsListCharacter->m_bIsReadyRemove ||
			m_pcsAgpmCharacter->IsCreature(pcsListCharacter) ||
			pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
			pcsListCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO ||
			m_pcsAgpmCharacter->IsStatusFullTransparent(pcsListCharacter) ||
			m_pcsAgpmCharacter->IsStatusInvincible(pcsListCharacter))
			continue;

		if (TRUE == m_pcsAgpmCharacter->IsMonster(pcsListCharacter))
		{
			if (FALSE == m_pcsAgpmCharacter->IsStatusTame(pcsListCharacter)
				&& FALSE == m_pcsAgpmCharacter->IsStatusSummoner(pcsListCharacter))
				continue;
		}

		pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData( pcsListCharacter );

		if (!pcsListCharacter->m_Mutex.WLock())
			continue;
			
		if( m_pagpmBattleGround->IsInBattleGround(pcsAgpdMobCharacter) && m_pagpmBattleGround->IsInBattleGround(pcsListCharacter) )
		{
			if(m_pcsAgpmCharacter->IsSameRace(pcsAgpdMobCharacter, pcsListCharacter))
				continue;
		}

		// ��׷ΰ� ���� ���� ����(��׷� �� ���� ���� ����) ã��
		INT32 lAgro = m_pcsAgsmCharacter->GetAgroFromEntry( pcsAgpdMobCharacter, pcsListCharacter->m_lID );
		if( lAgro < lMostAgro )
		{
			pcsTargetCharacter = pcsListCharacter;
			lMostAgro = lAgro;
		}

		//���� �������� ���� PC�ΰ��.
		if( lAgro == 0 )
		{
			//���� ���̸� ���� �������� ��� Ÿ�������Ѵ�.
			if( fPreemptiveRange != 0.0f && pcsAI2ADChar->m_pcsAgpdAI2Template->m_lPreemptiveType > 0)
			{
				//fPreemptiveRange�ȿ� �����ϴ°�?
				float			fTempX, fTempZ;

				fTempX = pcsListCharacter->m_stPos.x - pcsAgpdMobCharacter->m_stPos.x;
				fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdMobCharacter->m_stPos.z;

				if( fTempX*fTempX + fTempZ*fTempZ < fPreemptiveRange*fPreemptiveRange )
				{
					//PC�� ���� ���� �÷��װ� ������ �������� �ʴ´�.
					if( pcsListCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_NOT_ADD_AGRO )
					{
						//Ÿ������ ������ �ʴ´�.
					}
					else
					{
						//��׷� ����(LV, HP, MP, SP���µ�...)�� ���� ��׷θ� �ߵ���Ų��.
						if( TRUE == CheckPreemptive( pcsAgpdMobCharacter, pcsListCharacter, pcsAI2ADChar ) )
						{
							float			fTempX, fTempZ;
							INT32			lMaxHP;
							INT32			lDecreaseAggroPointPerCycle;

							fTempX = pcsListCharacter->m_stPos.x - pcsAgpdMobCharacter->m_stPos.x;
							fTempZ = pcsListCharacter->m_stPos.z - pcsAgpdMobCharacter->m_stPos.z;

							//���� �ƹ��� Ÿ������ �������� ���� ���.
							if( pcsAgroTargetCharacter == NULL )
							{
								pcsAgroTargetCharacter = pcsListCharacter;
								fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
							}
							//������ �̹� Ÿ������ ������ ���.
							else
							{
								if( fAgroDistance > fTempX*fTempX + fTempZ*fTempZ )
								{
									pcsAgroTargetCharacter = pcsListCharacter;
									fAgroDistance = fTempX*fTempX + fTempZ*fTempZ;
								}
							}

							//������������� �켱 ��׷θ� �ְ� ����~
							lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdMobCharacter->m_csFactor);

							lDecreaseAggroPointPerCycle = (lMaxHP*pcsAgpdMobCharacter->m_pcsCharacterTemplate->m_csDecreaseData.m_lSlowPercent)/100;

							//5ȸ ���� Slow ���� ��׷� ��������ŭ�� Slow ��׷θ� �ο��Ѵ�.
							if( lDecreaseAggroPointPerCycle == 0 )
							{
								lDecreaseAggroPointPerCycle = 1;
							}

							lDecreaseAggroPointPerCycle *= 5;

							m_pcsAgsmCharacter->AddAgroPoint( pcsAgpdMobCharacter, pcsListCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseAggroPointPerCycle );
						}
						// ��ũ�� �ߵ� ������ Ȯ���Ѵ�.
						else if ( TRUE == CheckPreemptive( pcsAgpdMobCharacter, pcsListCharacter, pcsAI2ADChar ) )
						{
							//��ũ�����
							if ( pcsListCharacter && CheckUseScream( pcsAgpdMobCharacter, pcsAI2ADChar, GetClockCount() ) )
							{
								pcsAI2ADChar->m_bScreamUsed = ProcessUseScream( pcsAgpdMobCharacter, pcsAI2ADChar, pcsListCharacter );
								if (TRUE == pcsAI2ADChar->m_bScreamUsed)
								{
									pcsAI2ADChar->m_ulScreamStartTime = GetClockCount();
								}
							}
						}
					}
				}
			}
		}
		
		if(pcsListCharacter)
			pcsListCharacter->m_Mutex.Release();
	}

	if( (pcsTargetCharacter == NULL) && (pcsAgroTargetCharacter != NULL) )
	{
		pcsTargetCharacter = pcsAgroTargetCharacter;
	}

	if (!pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.empty())
	{
		//STOPWATCH2(GetModuleName(), _T("GetTargetPC;; m_csSummon.m_vtTID"));

		if (pcsAI2ADChar->m_lTargetID == AP_INVALID_CID && pcsTargetCharacter)
		{
			// ���� Summon�ϴ� ���Ͷ�� Summon����. �ϴ� Duration�� 9999999 �� ���� ���߿� ���� �ٲ㼭 ���Ѵ� ���� �ؾ���
			m_pcsAgpmSummons->SetMaxSummonsCount(pcsAgpdMobCharacter, pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_lMaxCount);

			for (ApVector<INT32, 5>::iterator iter = pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.begin();
				pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_vtTID.end() != iter;
				++iter)
			{
				m_pcsAgsmSummons->ProcessSummons(pcsAgpdMobCharacter, *iter, 99999999, pcsAI2ADChar->m_pcsAgpdAI2Template->m_csSummon.m_lMaxCount, 1);
			}
		}

		if (pcsTargetCharacter)
		{
			ProcessSetTargetToSummons(pcsAgpdMobCharacter, pcsTargetCharacter->m_lID);
		}

		// ���� Summon�ϴ� �����ε� Target�� ������, Summon�� ���� �� ������.
		if (pcsTargetCharacter == NULL)
		{
			m_pcsAgsmSummons->RemoveAllSummons(pcsAgpdMobCharacter,
						AGSMSUMMONS_CB_REMOVE_TYPE_SUMMONS | AGSMSUMMONS_CB_REMOVE_TYPE_TAME | AGSMSUMMONS_CB_REMOVE_TYPE_FIXED);

		}
	}

	//���� ����ġ�� ����PC�� ID�� �����Ѵ�!
	return pcsTargetCharacter;
}

void AgsmAI2::SetFollowersTarget( AgpdCharacter *pcsAgpdCharacter, AuPOS csMobPos, AgpdAI2ADChar *pcsAgpdAI2ADChar )
{
	//STOPWATCH2(GetModuleName(), _T("SetFollowersTarget"));

	AgpdCharacter			*pcsFollower;
	AgpdAI2ADChar			*pcsFollowerAI2ADChar;

	ApSafeArray<INT32, AGPDAI2_MAX_FOLLOWER_COUNT>		alBossTargetList;
	ApSafeArray<INT32, AGPDAI2_MAX_FOLLOWER_COUNT>		alTempBossTargetList;

	if(m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar) == FALSE)
		return;

	//Ÿ���� �����ش�.
	INT32				lTargets;
	lTargets = m_pcsAgsmCharacter->GetMobListFromHistoryEntry( pcsAgpdCharacter, &alBossTargetList[0], AGPDAI2_MAX_FOLLOWER_COUNT );

	//�������� ���� �������� �ο�� ��������, ���ϵ鿡�� Ÿ���� �����ִ°� �����ϴ�.
	if( lTargets > 0 )
	{
		for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i )
		{
			pcsFollower = pcsAgpdAI2ADChar->m_pcsFollower[i];

			if( pcsFollower != NULL && pcsFollower->m_Mutex.WLock())
			{
				pcsFollowerAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollower);

//-----------------------------------------------------------------------------------
				INT32				lTargetShareFollowers;

				lTargetShareFollowers = m_pcsAgpmAI2->GetUpkeepFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template);

				//�� n���� ������ ������ ���� Ÿ���� �����Ѵ�. ���̵� ���������� ��밡����.
				if( i < lTargetShareFollowers )
				{
					pcsFollowerAI2ADChar->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
				}
				else
				{
					//Slow, Fast����ġ�� ���� ������ ���� �Է����ְ� ������� Ÿ�ϰ��� ȿ���� �ش�.

					//���� �������� �ο�� �ִ°��~ �׳� Ÿ�� ����~ Ư���� ������ ���� �ʴ´�.
					if( pcsFollowerAI2ADChar->m_lTargetID )
					{
					}
					//Ÿ���� ������ ������ ���ø� �޾� Ÿ���� ������~
					else
					{
						BOOL			bExist;

						bExist = FALSE;

						//��׷ΰ� ���°��.
						if( m_pcsAgsmCharacter->IsNoAgro( pcsFollower ) )
						{
							AgpdCharacter			*pcsAgpdCharacter;
							AgpdAI2ADChar			*pcsTempAI2ADChar;
							INT32					lTargetPC;

							lTargetPC = 0;

							//�̸� �����صд�.
							alTempBossTargetList.MemCopy(0, &alBossTargetList[0], AGPDAI2_MAX_FOLLOWER_COUNT);

							for( int lFollowersCount=0; lFollowersCount<pcsAgpdAI2ADChar->m_lCurrentFollowers; lFollowersCount++ )
							{
								pcsAgpdCharacter = pcsAgpdAI2ADChar->m_pcsFollower[lFollowersCount];

								if( pcsAgpdCharacter )
								{
									pcsTempAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollower);

									if( pcsTempAI2ADChar )
									{
										if( pcsTempAI2ADChar->m_lTargetID )
										{
											for( int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j )
											{
												if( alTempBossTargetList[j] == pcsTempAI2ADChar->m_lTargetID )
												{
													alTempBossTargetList[j] = 0;
												}
											}
										}
									}										
								}
							}

							for( int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k )
							{
								if( alTempBossTargetList[k] )
								{
									lTargetPC = alTempBossTargetList[k];
									break;
								}
							}

							if( lTargetPC )
							{
								pcsFollowerAI2ADChar->m_lTargetID = lTargetPC;
							}
							//Ÿ���� ���� ���� ������ ���� Ÿ���� �����Ѵ�.
							else
							{
								pcsFollowerAI2ADChar->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
							}
						}
						//��׷ΰ� �ִ°��.
						else
						{
							//��׷ΰ� ���� �������� Ÿ������ ���Ѵ�.
						}
					}
				}
				
				if(pcsFollower)
					pcsFollower->m_Mutex.Release();
			}
		}
	}
}

// 2005.08.30. steeple
// Summons �� �����ؾ��� Target �� ��´�.
// return �Ǵ� AgpdCharacter �� Lock ���� ���� �����̴�.
AgpdCharacter* AgsmAI2::GetAttackTargetPC(AgpdCharacter* pcsOwner, AgpdCharacter* pcsSummons, EnumAgpdSummonsPropensity ePropensity)
{
	if(!pcsOwner || !pcsSummons)
		return NULL;

	if (AGPMSUMMONS_PROPENSITY_SHADOW == ePropensity)
		return NULL;

	AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
	if(!pcsAI2ADSummons)
		return NULL;

	//STOPWATCH2(GetModuleName(), _T("GetAttackTargetPC"));

	// 2005.09.08. steeple
	// TargetID �� Owner �̸� 0 ���� �ٲ���.
	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsSummons);
	if(pcsSummonsADChar && pcsSummonsADChar->m_lOwnerCID == pcsAI2ADSummons->m_lTargetID)
	{
		pcsAI2ADSummons->m_lTargetID = 0;
	}

	if(pcsAI2ADSummons->m_lTargetID == pcsSummons->m_lID)
		pcsAI2ADSummons->m_lTargetID = 0;

	if(pcsAI2ADSummons->m_lTargetID == 0 && pcsAI2ADSummons->m_lReadyTargetID != 0)
	{
		pcsAI2ADSummons->m_lTargetID = pcsAI2ADSummons->m_lReadyTargetID;
		pcsAI2ADSummons->m_lReadyTargetID = 0;
	}

	if(pcsAI2ADSummons->m_lTargetID != 0 && (pcsAI2ADSummons->m_lTargetID == pcsAI2ADSummons->m_lReadyTargetID))
		pcsAI2ADSummons->m_lReadyTargetID = 0;

	// Ÿ���� �ִ� ���� ���� �����̶� �ʹ� �־����� ������ ���󰡰� �Ѵ�. NULL �������ָ� �ȴ�.
	// ���� ĳ�� ����.
	AuPOS stDest;
	INT32 lTargetDistance = 0;	// IsInRange�� FALSE�� ���� ���� �����ȴ�.
	INT32 lAOIRange = m_lSummonAOIRange;
	ZeroMemory(&stDest, sizeof(stDest));
	if(FALSE == pcsSummons->m_pcsCharacterTemplate->m_bSelfDestructionAttackType &&
		FALSE == m_pcsAgpmFactors->IsInRange(&pcsOwner->m_stPos, &pcsSummons->m_stPos, lAOIRange, 0, &stDest, &lTargetDistance))
	{
		if(lTargetDistance > AGSMAI2_Summons_Max_Follow)
			return NULL;
	}

	AgpdCharacter* pcsTarget = m_pcsAgpmCharacter->GetCharacterLock(pcsAI2ADSummons->m_lTargetID);
	if(pcsTarget)
	{
		// ����� �� ���¶�� �ٷ� ����
		if(pcsTarget->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
			m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTarget, FALSE))
		{
			pcsTarget->m_Mutex.Release();
			return pcsTarget;
		}

		pcsTarget->m_Mutex.Release();
	}

	// ������� �Դٸ� ����ε� Ÿ���� �ƴ϶� �Ҹ���. �׷��ٸ� ���� Ÿ���� Ÿ�� ���̵�� �����ϰ� ����.
	pcsAI2ADSummons->m_lTargetID = pcsAI2ADSummons->m_lReadyTargetID;
	pcsAI2ADSummons->m_lReadyTargetID = 0;

	pcsTarget = m_pcsAgpmCharacter->GetCharacterLock(pcsAI2ADSummons->m_lTargetID);
	if(pcsTarget)
	{
		// ����� �� ���¶�� �ٷ� ����
		if(pcsTarget->m_unActionStatus != AGPDCHAR_STATUS_DEAD &&
			m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTarget, FALSE))
		{
			pcsTarget->m_Mutex.Release();
			return pcsTarget;
		}

		pcsTarget->m_Mutex.Release();
	}

	// ��������� ������ �ȉ�ٸ� ������ Ÿ���� ���� ����.
	AgsdCharacterHistoryEntry *pcsCharHistory = m_pcsAgsmCharacter->GetMostAgroChar(pcsSummons);
	if (pcsCharHistory)
	{
		if (APBASE_TYPE_CHARACTER == pcsCharHistory->m_csSource.m_eType)
		{
			AgpdCharacter* pcsCharacter = m_pcsAgpmCharacter->GetCharacter(pcsCharHistory->m_csSource.m_lID);
			if (pcsCharacter)
			{
				if (pcsCharacter->m_lID != m_pcsAgpmSummons->GetOwnerCID(pcsSummons) &&
					pcsCharacter->m_lID != pcsSummons->m_lID &&
					m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter) == FALSE)
				{
					pcsAI2ADSummons->m_lTargetID = pcsCharacter->m_lID;
					return pcsCharacter;
				}
			}
		}
	}

	// ��ȯ���� �������ε� Ÿ���� ���ٸ� ������ ������ �ƹ����̳� Ÿ������ ���ع�����.
	if (AGPMSUMMONS_PROPENSITY_ATTACK == ePropensity)
	{
		// 2005.10.13. steeple
		// ������ 2���� �������� �Ϳ��� 33������ ����.
		ApSafeArray<AgpdCharacter*, AGSMAI2_MAX_PROCESS_TARGET> TargetCharacter;
		TargetCharacter.MemSetAll();
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET> TargetCID;
		TargetCID.MemSetAll();

		// ����/���� ��ȯ���� ��� ���� ĳ���͸� ���� �� �� �־�� �ϱ� ������ ��쿡 ���� 
		// �˻����ǿ� APMMAP_CHAR_TYPE_PC Ÿ���� �߰��Ѵ�.
		INT32 CharType = APMMAP_CHAR_TYPE_MONSTER;

		if (AGPD_SIEGE_MONSTER_SUMMON == m_pcsAgpmSiegeWar->GetSiegeWarMonsterType(pcsSummons))
		{
			CharType |= APMMAP_CHAR_TYPE_PC;
		}

		// 2005.10.28. steeple
		// ��ȯ�� �þߴ� 15���ͷ� ����. (�������� 25 ����)
		// �ֺ��� MOB�� �ֳ�����.
		INT32 lNumCID = m_pcsApmMap->GetCharList(pcsSummons->m_nDimension, CharType, pcsSummons->m_stPos,
												AGSMAI2_Summons_Visibility, (INT_PTR*)&TargetCharacter[0], AGSMAI2_MAX_PROCESS_TARGET,
												&TargetCID[0], AGSMAI2_MAX_PROCESS_TARGET);
		ASSERT(lNumCID <= AGSMAI2_MAX_PROCESS_TARGET);

		// ������ �� �ִ� ������� üũ�Ѵ�.
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>::iterator iter = TargetCID.begin();
		if(TargetCID.IsValidIterator(iter) == TRUE)
		{
			while(iter != TargetCID.end())
			{
				if(!*iter)
				{
					++iter;
					continue;
				}

				AgpdCharacter* pcsTargetCharacter = m_pcsAgpmCharacter->GetCharacter(*iter);
				if(!pcsTargetCharacter)
				{
					++iter;
					continue;
				}

				// 2006.11.07. steeple
				// �� �ϱ� ���� ��ȿ�� ���� üũ�Ѵ�.
				if(pcsTargetCharacter->m_bIsReadyRemove ||
					!pcsTargetCharacter->m_bIsActiveData ||
					!pcsTargetCharacter->m_bIsAddMap ||
					pcsTargetCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD ||
					pcsTargetCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HIDE_AGRO ||
					m_pcsAgpmCharacter->IsStatusFullTransparent(pcsTargetCharacter) ||
					m_pcsAgpmCharacter->IsStatusInvincible(pcsTargetCharacter))
				{
					++iter;
					continue;
				}

				AuAutoLock csLock(pcsTargetCharacter->m_Mutex);
				if(!csLock.Result())
				{
					++iter;
					continue;
				}

				if(pcsSummons->m_lID == pcsTargetCharacter->m_lID)
				{
					++iter;
					continue;
				}

				if(m_pcsAgpmPvP->IsAttackable(pcsSummons, pcsTargetCharacter, FALSE))
					return pcsTargetCharacter;

				++iter;
			}
		}
	}

	pcsAI2ADSummons->m_lTargetID = 0;

	return NULL;
}

// 2005.09.06. steeple
// ��ȯ������ Ÿ�� �ִ� �Լ��� �и��ߴ�.
BOOL AgsmAI2::ProcessSetTargetToSummons(AgpdCharacter* pcsOwner, INT32 lTargetCID)
{
	if(!pcsOwner || lTargetCID == 0)
		return FALSE;

	//if(!m_pcsAgpmCharacter->IsPC(pcsOwner))
	//	return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsOwner);
	if(!pcsSummonsADChar || !pcsSummonsADChar->m_SummonsArray.m_pSummons)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessSetTargetToSummons"));

	// �ڱ� Summons ���� ���鼭 Ÿ���� �������ش�.

	AgpdSummonsArray::SummonsVector tmpVector(pcsSummonsADChar->m_SummonsArray.m_pSummons->begin(),
												pcsSummonsADChar->m_SummonsArray.m_pSummons->end());
	AgpdSummonsArray::iterator iter = tmpVector.begin();
	while(iter != tmpVector.end())
	{
		AgpdCharacter* pcsSummons = m_pcsAgpmCharacter->GetCharacterLock(iter->m_lCID);
		if(!pcsSummons)
		{
			++iter;
			continue;
		}

		// Summons ���� �����ؾ��� ����� �������ش�.
		AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
		if(pcsAI2ADSummons)
		{
			if(pcsAI2ADSummons->m_lTargetID == 0)
				pcsAI2ADSummons->m_lTargetID = lTargetCID;
			else if(pcsAI2ADSummons->m_lTargetID == lTargetCID)
			{}	// �ƹ��͵� ���� �ʴ´�.
			else
				pcsAI2ADSummons->m_lReadyTargetID = lTargetCID;
		}

		pcsSummons->m_Mutex.Release();
		++iter;
	}

	// �ڱ� Summons ���� ���鼭 Ÿ���� �������ش�.
	for(INT32 i = 0; i < AGPMSUMMONS_MAX_TAME; i++)
	{
		if(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID == 0)
			break;

		AgpdCharacter* pcsSummons = m_pcsAgpmCharacter->GetCharacterLock(pcsSummonsADChar->m_TameArray.m_arrTame[i].m_lCID);
		if(!pcsSummons)
			continue;

		// Summons ���� �����ؾ��� ����� �������ش�.
		AgpdAI2ADChar* pcsAI2ADSummons = m_pcsAgpmAI2->GetCharacterData(pcsSummons);
		if(pcsAI2ADSummons)
		{
			if(pcsAI2ADSummons->m_lTargetID == 0)
				pcsAI2ADSummons->m_lTargetID = lTargetCID;
			else if(pcsAI2ADSummons->m_lTargetID == lTargetCID)
			{}	// �ƹ��͵� ���� �ʴ´�.
			else
				pcsAI2ADSummons->m_lReadyTargetID = lTargetCID;
		}

		pcsSummons->m_Mutex.Release();
	}
	
	return TRUE;
}

BOOL AgsmAI2::ProcessExAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessExAI");

	//STOPWATCH2(GetModuleName(), _T("ProcessExAI"));

	BOOL			bUseExAI;
	INT32			lIndex;

	lIndex = -1;
	bUseExAI = FALSE;

	if( pcsTargetPC )
	{
		lIndex = CheckUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
	}
	else if(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) /*&& m_pcsAgpmSiegeWar->IsDoingSiegeWar(pcsAgpdCharacter)*/)
	{
		// 2006.08.29. steeple
		// ���� ���� ���ʹ� �ϴ� ���⼭�� Ÿ���� ����.
		lIndex = CheckUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
	}

	//��ų���?
	if( lIndex != -1 )
	{
		if( pcsTargetPC != NULL ||
			(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) /*&& m_pcsAgpmSiegeWar->IsDoingSiegeWar(pcsAgpdCharacter)*/))
		{
			bUseExAI = ProcessUseSkill( pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsTargetPC );
		}
	}
	else
	{
		lIndex = CheckUseItem( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );
		//�����ۻ��?
		if( lIndex != -1 )
		{
			bUseExAI = ProcessUseItem( pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsTargetPC );
		}
		else
		{
			//��ũ�����?
			if ( pcsTargetPC && CheckUseScream( pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount ) )
			{
				pcsAgpdAI2ADChar->m_bScreamUsed = ProcessUseScream( pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsTargetPC );
				if (TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
				{
					pcsAgpdAI2ADChar->m_ulScreamStartTime = GetClockCount();
				}

				bUseExAI = pcsAgpdAI2ADChar->m_bScreamUsed;
			}
		}
	}

	return bUseExAI;
}
/*
BOOL AgsmAI2::ProcessAI( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	PROFILE("AgsmAI2::ProcessAI");

	//STOPWATCH2(GetModuleName(), _T("ProcessAI"));

	AgpdCharacter	*pcsAgpdMobCharacter;
	AgpdCharacter	*pcsAgpdPCCharacter;

	BOOL			bResult;
	BOOL			bUseExAI;
	BOOL			bUseAreaAI;

	AuPOS			csMobPos;
	INT32			lMobID;
	float			fPreemptiveRange;

	pcsAgpdMobCharacter = NULL;
	pcsAgpdPCCharacter	= NULL;
	bResult				= FALSE;

	if( !pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
		return FALSE;

	pcsAgpdMobCharacter	= pcsAgpdCharacter;

	// ���� �����ΰ��� ������ �ϸ� �ȵȴ�. ����...
	// 2004.08.25. steeple
	//if (pcsAgpdMobCharacter->m_unActionStatus == AGPDCHAR_STATUS_STUN)
	if (pcsAgpdMobCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csMobPos = pcsAgpdCharacter->m_stPos;
	lMobID = pcsAgpdCharacter->m_lID;
	fPreemptiveRange = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fPreemptiveRange;

	// ��ũ���� ����ϴ� ���Ͷ�� ���� �ð��Ŀ� �ٽ� ����Ҽ� �ֵ��� �ʱ�ȭ�� ���ش�.
	if (TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
	{
		if (pcsAgpdAI2ADChar->m_ulScreamStartTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime < GetClockCount())
			pcsAgpdAI2ADChar->m_bScreamUsed = FALSE;
	}

	ProcessCommonAI(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );

	// ������ ���ٸ� ���������� Ÿ���� ã�´�. �������ڽ�, �������� ���� ���ʹ� ����� ���ð��̴�.
	if( pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL )
	{
		pcsAgpdPCCharacter = GetTargetPC( pcsAgpdCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange );

		//�������̴�!! ���ϵ鿡�� ���� ����������~
		if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template ) > 0 && pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
		{
			SetFollowersTarget( pcsAgpdCharacter, csMobPos, pcsAgpdAI2ADChar );
		}
	}
	//���� ���Ϸ� ������ �ִ� ������ �ִٸ� �׺��� ��ɿ� ���� Ÿ���� ������~
	else
	{
		//������ �ִٸ� ������ �������ֽ� Target�� Ÿ������ ������~
		if( pcsAgpdAI2ADChar->m_lTargetID != 0 )
		{
			pcsAgpdPCCharacter = m_pcsAgpmCharacter->GetCharacter( pcsAgpdAI2ADChar->m_lTargetID );
		}
		else
		{
			pcsAgpdPCCharacter = NULL;
		}
	}

	if( pcsAgpdPCCharacter )
	{
		if (pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID)
			pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

		pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;

		//2004.10.25�� ���� ��� ���յǸ鼭 �ּ�ó����.
/*		//Ÿ���� �ٲ��� ��������.
		if( pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID )
		{
			pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;
			ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_TARGET_PC );
		}
	}
	else
	{
		pcsAgpdAI2ADChar->m_lTargetID = 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;
	}

	if (pcsAgpdPCCharacter && !pcsAgpdPCCharacter->m_Mutex.WLock())
		return FALSE;

	// Ÿ�ٰ��� �Ÿ��� ����Ѵ�. �ʹ� �ָ� �ִ� ���̸� Ÿ�ٿ��� �����Ѵ�.
	if (ProcessCheckTargetArea(pcsAgpdCharacter, pcsAgpdPCCharacter))
	{
		pcsAgpdPCCharacter->m_Mutex.Release();

		pcsAgpdPCCharacter	= NULL;
	}

	bUseAreaAI = ProcessCheckArea(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount );

	if (bUseAreaAI)
	{
		// AreaAI�� ��������� �翬��~ ���� ����.
		// �ƹ��� ����.
	}
	else
	{
		bUseExAI = ProcessExAI( pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsAgpdPCCharacter, lClockCount );

		if( bUseExAI )
		{
			//ExAI�� ��������� �翬��~ ���� ����.
			//�ƹ��� ����.
		}
		else
		{
			//Ÿ���� ������.
			//
			// ���Ͱ� ������ ���¶�� ���ݸ��ϰ� �Ѵ�. 2005.10.07. steeple
			// �� ���� ���Ͱ� �ƴ϶�, PC �� ���ؼ� ũ���� ���� �ɷ� ������ ������.
			//
			if( pcsAgpdPCCharacter && pcsAgpdMobCharacter->m_bIsTrasform == FALSE )
			{
				PROFILE("AgsmAI2::AttackTarget");

				BOOL				bInRange;
				BOOL				bPathFind;

				bInRange =  FALSE;
				bPathFind = FALSE;

				AuPOS				stDestPos;
				ZeroMemory(&stDestPos, sizeof(stDestPos));

				bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdMobCharacter->m_stPos, &pcsAgpdPCCharacter->m_stPos, &pcsAgpdMobCharacter->m_csFactor, &pcsAgpdPCCharacter->m_csFactor, &stDestPos);

				//���� ���� ���ΰ�?
				if( bInRange )
				{
					//Normal Attack�� �����ϸ� �Ÿ��� ����Ѵ�. �׿ܿ��� ����� �ʿ䰡 ������...
					if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bUseNormalAttack )
					{
						//�����ص� �Ǹ� �����Ѵ�.
						if( lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval )
						{
							//�����Ҷ� ������ �� ȣ���Ѵ�.
							ProcessNPCDialog( pcsAgpdMobCharacter, AGPD_NPCDIALOG_MOB_ATTACK );

							m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdMobCharacter, (ApBase *)pcsAgpdPCCharacter, NULL);

							pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
						}
						else
						{
							//���� �̵����̸� �����.
							if (pcsAgpdMobCharacter->m_bMove)
								m_pcsAgpmCharacter->StopCharacter(pcsAgpdMobCharacter, NULL);
						}
					}
					else
					{
						//���� �̵����̸� �����.
						if (pcsAgpdMobCharacter->m_bMove)
							m_pcsAgpmCharacter->StopCharacter(pcsAgpdMobCharacter, NULL);
					}
				}
				//���ݹ��� ���ΰ�?
				else if (!pcsAgpdAI2ADChar->m_bIsNeedPathFind)
				{
					if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
					{
						pcsAgpdAI2ADChar->m_csTargetPos	= stDestPos;
						m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );
						bPathFind = TRUE;
					}
				}
				else
				{
					if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
					{

						pcsAgpdAI2ADChar->m_csTargetPos	= pcsAgpdPCCharacter->m_stPos;

						//�������� ���� ã�Ƶд�.
						bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdCharacter->m_stPos.x,
																pcsAgpdCharacter->m_stPos.z,
																pcsAgpdAI2ADChar->m_csTargetPos.x,
																pcsAgpdAI2ADChar->m_csTargetPos.z,
																pcsAgpdAI2ADChar->m_pclPath );

						if (!bResult)
						{
							pcsAgpdCharacter->m_ulCheckBlocking	= 0;
							pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;
						}

						//
						CPriorityListNode< int , AgpdPathFindPoint *> *pcsNode;

						pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

						if( pcsNode )
						{
							AuPOS			csPos;

							csPos.x = pcsNode->data->m_fX;
							csPos.y = 0.0f;
							csPos.z = pcsNode->data->m_fY;

							m_pcsAgpmCharacter->MoveCharacter( pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE );
						}
					}
				}
			}
			//Ÿ���� ������. AND // ������ �����̸� �������� �ʴ´�.
			else if (FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				//���ϸ��̸� �������� ������ ��ġ�� �����δ�.
				if( pcsAgpdAI2ADChar->m_pcsBossCharacter != NULL )
				{
					if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
					{
						//�ƹ��Ͼ���.
					}
					else
					{
						float				fTempX, fTempZ;

						fTempX = pcsAgpdAI2ADChar->m_csTargetPos.x - pcsAgpdCharacter->m_stPos.x;
						fTempZ = pcsAgpdAI2ADChar->m_csTargetPos.z - pcsAgpdCharacter->m_stPos.z;

						//�������� 1���Ͷ�� �������.
						if( fTempX*fTempX + fTempZ*fTempZ <= 100.0f*100.0f )
						{
						}
						//�׷��� �ʴٸ�? ���� ã�� �̵���Ű��~
						else
						{
							m_pcsAgpmCharacter->MoveCharacter( pcsAgpdMobCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );
						}
					}
				}
				else
				{
					PROFILE("AgsmAI2::MoveWander");

					ApdEvent			*pcsEvent;

					float				fTempX, fTempZ, fTempDistance;

					pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsAgpdCharacter)->m_pstEvent;

					if( pcsEvent != NULL )
					{
						if( lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime )
						{
							//�ƹ��Ͼ���.
						}
						else
						{
							//�̵����� �ƴѵ� ���� �н� ����Ʈ�� ������ ���� ã�� 
							if( !pcsAgpdCharacter->m_bMove )
							{
								{
									PROFILE("AgsmAI2::GetNewTargetPosition");

									fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
									fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
									fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

									//���� ���������ȿ� �������� �ʴ´ٸ� ���� ��ġ�� ���ο� ��������Ʈ�� ��´�.
									if( fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance )
									{
										pcsAgpdAI2ADChar->m_csSpawnBasePos = pcsAgpdCharacter->m_stPos;
									}

									if( GetTargetPosFromSpawn(&pcsAgpdCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius ) )
									{
										bool			bResult;
										INT32			lRandTime;

										bResult = false;
										lRandTime = m_csRand.randInt()%15000;

										pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

										//�̵��� ���� �������� �׳� �������ش�.
										ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING );

										m_pcsAgpmCharacter->MoveCharacter( pcsAgpdMobCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION );

										if( bResult )
										{
											//AgpmCharacter::OnIdle()���� CallBackȣ��! GetNextNode�� ȣ���ϰ� �̵��Ѵ�.
										}
										else
										{
											//�̰� ���� ���ٴ� �̾߱��ε�.... �׳� �����ؾ��� ��.... ��.
										}
									}

									//�������̸� �ڽ��� ���ϵ鿡�Ե� ���� ��ġ�� �����ְ� ��ġ����� ��ã�����ó�� �̸� ���ش�.
									if( m_pcsAgpmAI2->GetMaxFollowers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 )
									{
										AgpdAI2ADChar		*pcsAI2ADFollowerChar;
										AgpdCharacter		*pcsAgpdFollowerCharacter;

										INT32				lCurrentFollowers;
										INT32				lRealCounter;
										INT32				lDegreePerFollower;
										float				fFollowerMargin;
										float				fFormationTempX, fFormationTempZ;

										lCurrentFollowers = pcsAgpdAI2ADChar->m_lCurrentFollowers;

										if( lCurrentFollowers > 0 )
										{
											lDegreePerFollower = 360/lCurrentFollowers;
											lRealCounter = 0;
											fFollowerMargin = 300.0f + lCurrentFollowers*30.0f;

											for( int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i )
											{
												pcsAgpdFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[i];

												if( pcsAgpdFollowerCharacter != NULL && pcsAgpdFollowerCharacter->m_Mutex.WLock())
												{
													fFormationTempX = sin(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;
													fFormationTempZ = cos(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;

													pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdFollowerCharacter);

													//���� �̵����̸� �����.
													if (pcsAgpdFollowerCharacter->m_bMove)
														m_pcsAgpmCharacter->StopCharacter(pcsAgpdFollowerCharacter, NULL);

													//�������� �����ش�.
													pcsAI2ADFollowerChar->m_csTargetPos.x = pcsAgpdAI2ADChar->m_csTargetPos.x + fFormationTempX;
													pcsAI2ADFollowerChar->m_csTargetPos.z = pcsAgpdAI2ADChar->m_csTargetPos.z + fFormationTempZ;

													pcsAI2ADFollowerChar->m_ulNextWanderingTime = pcsAgpdAI2ADChar->m_ulNextWanderingTime;

													//�浵 ã�´�.
													bResult = m_pcsAgpmPathFind->pathFind(  pcsAgpdFollowerCharacter->m_stPos.x,
																							pcsAgpdFollowerCharacter->m_stPos.z,
																							pcsAI2ADFollowerChar->m_csTargetPos.x,
																							pcsAI2ADFollowerChar->m_csTargetPos.z,
																							pcsAI2ADFollowerChar->m_pclPath );

													m_pcsAgpmCharacter->MoveCharacter( pcsAgpdFollowerCharacter, &pcsAI2ADFollowerChar->m_csTargetPos, MD_NODIRECTION );

													pcsAgpdFollowerCharacter->m_Mutex.Release();

													++lRealCounter;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	if (pcsAgpdPCCharacter)
		pcsAgpdPCCharacter->m_Mutex.Release();

	return bResult;
}
*/

BOOL AgsmAI2::ProcessMonsterAI(AgpdCharacter *pcsCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	AgpdCharacter			*pcsAgpdModCharacter = NULL;
	AgpdCharacter			*pcsAgpdPCCharacter = NULL;
	AgsmAI2ResultMove		eResultMove = AGSMAI2_RESULT_MOVE_FAIL;
	AgsmAI2SpecificState	eSpecificState = AGSMAI2_STATE_NONE;

	BOOL			bResult;
	BOOL			bUseAreaAI;

	AuPOS			csMobPos;
	INT32			lMobID;
	float			fPreemptiveRange;

	pcsAgpdModCharacter = NULL;
	pcsAgpdPCCharacter  = NULL;
	bResult				= FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	pcsAgpdModCharacter = pcsCharacter;

	// PinchMonster�ΰ�� ���ú��� ���ش�.
	SetPinchMonster(pcsAgpdModCharacter);

	// ���ϻ����� ���� ������ �׳� �Ѱܹ�����.
	if(pcsAgpdModCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN)
		return TRUE;

	csMobPos = pcsCharacter->m_stPos;
	lMobID	 = pcsCharacter->m_lID;
	fPreemptiveRange = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fPreemptiveRange;
	
	// ��ũ���� ����ϴ� ���Ͷ�� ���� �ð��Ŀ� �ٽ� ����Ҽ� �ֵ��� �ʱ�ȭ�� ���ش�.
	if(TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
	{
		if(pcsAgpdAI2ADChar->m_ulScreamStartTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulFellowWorkerRefreshTime < GetClockCount())
			pcsAgpdAI2ADChar->m_bScreamUsed = FALSE;
	}

	// ���� �������� ����ϴ� �Լ��� ȣ��
	MonsterCommonAI(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount);

	// �������� ��� Follower���� �����Ѵ�.
	if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
	{
		ManagementFollower(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	// Enemy�� ã���ش�.
	pcsAgpdPCCharacter = FindEnemyTarget(pcsAgpdModCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange);

	// AuAutoLock���� ���� 
	AuAutoLock Lock;

	if(pcsAgpdPCCharacter)
	{
		Lock.SetMutualEx(pcsAgpdPCCharacter->m_Mutex);
		Lock.Lock();
		if(!Lock.Result())
			return FALSE;
	}
	
	// AreaCheck�� ���ش�.
	bUseAreaAI = CheckArea(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	if(bUseAreaAI == TRUE)
		return TRUE;

	eSpecificState = SpecificStateAI(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	if(eSpecificState != AGSMAI2_STATE_NONE)
		return TRUE;

	// PC AI�� Monster���� ��������ش�.
	// disease�� AI�� ���°� �ƴϰ� ���¸� �����ϴ� ���̱⶧���� ������ AI�� �����ؾ��Ѵ�.
	eAgpmAI2StatePCAI eResult = AGPDAI2_STATE_PC_AI_NONE;

	if(TRUE == ProcessPCAI(pcsAgpdModCharacter, pcsAgpdAI2ADChar, lClockCount, &eResult, TRUE) && eResult != AGPDAI2_STATE_PC_AI_DISEASE)
		return TRUE;

	// Enemy���� ������ �ƴϸ� �ڱ� ������ ���ؼ� �̵��Ѵ�.
	eResultMove = MoveAI(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	
	if(eResultMove == AGSMAI2_RESULT_MOVE_TARGET_IN_RANGE)
	{
		AttackEnemy(pcsAgpdModCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	return TRUE;
}

// 2005.08.03. steeple ����
BOOL AgsmAI2::ProcessSummonAI(AgpdCharacter* pcsSummons, UINT32 lClockCount)
{
	if(!pcsSummons)
		return FALSE;

	AgpdSummonsADChar* pcsSummonsADChar = m_pcsAgpmSummons->GetADCharacter(pcsSummons);
	if(!pcsSummonsADChar)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessSummonsAI"));

	if(pcsSummonsADChar->m_lOwnerCID == AP_INVALID_CID)
	{
		ASSERT(0);
		return FALSE;
	}

	// ������ ��ȯ���� ���� �׳� ������. 2005.10.05. steeple
	if(m_pcsAgpmCharacter->IsStatusFixed(pcsSummons))
		return TRUE;

	// Owner �� ���Ѵ�.
	ApAutoLockCharacter csAutoLockCharacter(m_pcsAgpmCharacter, pcsSummonsADChar->m_lOwnerCID);
	AgpdCharacter* pcsOwner = csAutoLockCharacter.GetCharacterLock();
	if(!pcsOwner)
	{
		//ASSERT(0);
		return FALSE;
	}

	// 2008.03.17. steeple
	// �Ʒ� TID �˻�� ����.
	// ����Ǯ�� �� �ַλ����� ���ǰ� �ȴ�.
	//if (pcsSummonsADChar->m_lOwnerTID != AP_INVALID_CID &&
	//	pcsSummonsADChar->m_lOwnerTID != pcsOwner->m_pcsCharacterTemplate->m_lID)
	//{
	//	//ASSERT(0);
	//	return FALSE;
	//}

	// 2005.08.03. steeple
	// �� �� AI ��û ���� �ؾ� ��. -_-;;;
	//
	//
	//
	//

	// 2005.09.06. steeple
	// ���� ������ �ϰ� ������ ����� ������ ���ο��� ���� ������ �Ѵ�.
	//
	// ĳ���Ͱ� Ÿ������ ���� ���Ͱ� �ִٸ� �����Ѵ�.
	AgpdCharacter* pcsTarget = GetAttackTargetPC(pcsOwner, pcsSummons, pcsSummonsADChar->m_stSummonsData.m_ePropensity);

	if(pcsTarget && pcsTarget->m_lID != pcsOwner->m_lID && pcsTarget->m_lID != pcsSummons->m_lID)
	{
		AuAutoLock csLock(pcsTarget->m_Mutex);
		if (!csLock.Result())
		{
			// 2007.01.24. steeple
			// �갡 �ѹ� ���µ�... ����� �ϱ� �����.
			// Ÿ�� �� ���ߴٰ� return FALSE �ع����� ��ȯ���� ������� �ȴ�.
			return TRUE;
		}

		INT32 lRange = 0;
		INT32 lTargetRange = 0;
		AuPOS stPos = pcsTarget->m_stPos;

		// ������ ActionCharacter �ȿ��� �Ÿ� üũ�ϰ� �����϶�� �Ѵ�.
		if(m_pcsAgpmFactors->IsInRange(&pcsSummons->m_stPos, &pcsTarget->m_stPos, &pcsSummons->m_csFactor, &pcsTarget->m_csFactor, &stPos, &lRange, &lTargetRange))
		{
			m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsSummons, (ApBase *)pcsTarget, NULL);
			return TRUE;
		}
		else if(lTargetRange < AGSMAI2_Summons_Max_Follow)
		{
			m_pcsAgpmCharacter->MoveCharacterFollow(pcsSummons, pcsTarget, lRange, TRUE);
			m_pcsAgsmCharacter->SetIdleInterval(pcsSummons, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_ONE_SECOND);
			return TRUE;
		}
	}

	// 2007.07.25. steeple
	// �������ӿ��� �Ұ��ϰ�, ������ �־����� ���� ������ ���󰡾� �ϹǷ� �Ʒ� �ڵ� �ּ�ó�� �Ͽ���.
	//
	//
	// ��ȯ���� �������϶��� ĳ���͸� ���󰡰ų� ���� �ʴ´�.
	//if (TRUE == m_pcsAgpmCharacter->IsCombatMode(pcsSummons))
	//	return TRUE;

	// �Ÿ��� 5m �̻� �������� ������ ��ó�� ���󰣴�.
	AuPOS stDest;
	INT32 lTargetDistance = 0;	// IsInRange�� FALSE�� ���� ���� �����ȴ�.

	INT32 lAOIRange = m_lSummonAOIRange;
	INT32 lJumpDistance = ( INT32 ) AGSMAI2_Summons_Max_Jump;
	if(m_pcsAgpmSummons->IsPet(pcsSummons))
	{
		if(pcsSummons->m_pcsCharacterTemplate->m_lPetType == 1)
			lAOIRange = m_lPetAOIRange2;
		else
			lAOIRange = m_lPetAOIRange;

		lJumpDistance = ( INT32 ) AGSMAI2_Pet_Max_Jump;
	}

	ZeroMemory(&stDest, sizeof(stDest));
	if(FALSE == m_pcsAgpmFactors->IsInRange(&pcsOwner->m_stPos, &pcsSummons->m_stPos, lAOIRange, 0, &stDest, &lTargetDistance))
	{
		if (lTargetDistance > lJumpDistance)
		{
			if (TRUE == m_pcsAgpmCharacter->IsStatusSummoner(pcsSummons))
			{
				// ��ȯ���� �Ÿ��� �ʹ� �־����� ���� ��ȯ
				m_pcsAgpmCharacter->StopCharacter(pcsSummons, NULL);
				m_pcsAgpmCharacter->UpdatePosition(pcsSummons, &pcsOwner->m_stPos, FALSE, TRUE);
			}
			else
			{
				// ���̹� ���Ͱ� �ʹ� �־����� ���̹� ����
				m_pcsAgsmSummons->EndSummonsPeriod(pcsSummons);
			}
		}
		else
		{
			// �����Ÿ� �ȿ� ������ ���󰡱�
			m_pcsAgpmCharacter->MoveCharacterFollow(pcsSummons, pcsOwner, lAOIRange, TRUE);
			m_pcsAgsmCharacter->SetIdleInterval(pcsSummons, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);
		}
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount, eAgpmAI2StatePCAI *eResult, BOOL bMonsterUse)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar || NULL == eResult)
		return FALSE;

	if(m_pcsAgpmCharacter->IsAllBlockStatus(pcsAgpdCharacter) == TRUE)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	if(!bMonsterUse)
	{
		// Monster���� ProcessMonsterAI���� ó���ϵ��� �Ѵ�.
		if( m_pcsAgpmCharacter->IsMonster(pcsAgpdCharacter) || m_pcsAgpmCharacter->IsCreature(pcsAgpdCharacter) ||
			m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter) )
			return FALSE;
	}

	eAgpmAI2StatePCAI eStatePCAI = AGPDAI2_STATE_PC_AI_NONE;

	eStatePCAI = m_pcsAgpmAI2->GetPCAIState(pcsAgpdCharacter);

	// If Character has no AI_state or has hold state skip PC AI
	if( eStatePCAI == AGPDAI2_STATE_PC_AI_NONE )
		return FALSE;

	BOOL bRet = FALSE;

	switch(eStatePCAI)
	{
	case AGPDAI2_STATE_PC_AI_CONFUSION:
		{
			bRet = ProcessPCAIConfusion(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_CONFUSION;
		}break;

	case AGPDAI2_STATE_PC_AI_BERSERK:
		{
			if(bMonsterUse)
			{
				
			}
			else
			{
				bRet = ProcessPCAIBerserk(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

				*eResult = AGPDAI2_STATE_PC_AI_BERSERK;
			}
		}break;

	case AGPDAI2_STATE_PC_AI_FEAR:
		{
			bRet = ProcessPCAIFear(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_FEAR;
		}break;

	case AGPDAI2_STATE_PC_AI_SHRINK:
		{
			bRet = TRUE;

			*eResult = AGPDAI2_STATE_PC_AI_SHRINK;
		}break;

	case AGPDAI2_STATE_PC_AI_DISEASE:
		{
			bRet = ProcessPCAIDisease(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

			*eResult = AGPDAI2_STATE_PC_AI_DISEASE;
		}break;

	case AGPDAI2_STATE_PC_AI_HOLD:
		{
			bRet = TRUE;

			*eResult = AGPDAI2_STATE_PC_AI_HOLD;
		}break;
	}

	return bRet;
}

BOOL AgsmAI2::ProcessPCAIConfusion(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_CONFUSION)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
	   pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
	   return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	// Calc Next Target Position
	//////////////////////////////////////////////////////////////////
	
	AuPOS stCurrentPOS = pcsAgpdCharacter->m_stPos;
	AuPOS stFirstPOS = pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS;

	if(stFirstPOS.x == 0.0f && stFirstPOS.z == 0.0f)
	{
		// first
		stFirstPOS = stCurrentPOS;
		pcsAgpdAI2ADChar->m_stPCAIInfo.stFirstPOS = pcsAgpdCharacter->m_stPos;

		// ���� ��ġ���� 2m������ �����δ�.
		AuPOS stDestPos = stFirstPOS;
		stDestPos.z = stDestPos.z + 300.0f;
		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDestPos, MD_NODIRECTION, FALSE, TRUE);
	}
	else
	{
		// Not First
		pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount++;
		if(pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount >= 6)
			pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount = 0;

		AuPOS stDestPos = stFirstPOS;

		INT32 lDegree = (360 / (6 - pcsAgpdAI2ADChar->m_stPCAIInfo.lAICount));

		FLOAT fTempPOSX = sin(lDegree*AGSMAI2_Radian) * 400.0f;
		FLOAT fTempPOSZ = cos(lDegree*AGSMAI2_Radian) * 400.0f;

		stDestPos.x = stDestPos.x + fTempPOSX;
		stDestPos.z = stDestPos.z + fTempPOSZ;

		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDestPos, MD_NODIRECTION, FALSE, TRUE);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIFear(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_FEAR)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////
	
	AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID);
	if(NULL == pcsTarget)
		return FALSE;

	AuPOS stDest;
	AuPOS stFrom		= pcsAgpdCharacter->m_stPos;
	AuPOS stTargetPos	= pcsTarget->m_stPos;
	FLOAT fAttackRange	= 0.0f;

	ZeroMemory(&stDest, sizeof(stDest));

	m_pcsAgpmFactors->GetValue(&pcsTarget->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

	FLOAT fTempX, fTempZ;

	fTempX = pcsTarget->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
	fTempZ = pcsTarget->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

	if(fTempX*fTempX + fTempZ*fTempZ < (fAttackRange + 5000.0f) * (fAttackRange + 5000.0f))
	{
		if(GetRunawayPosFromPC(pcsAgpdCharacter, &stFrom, &stTargetPos, &stDest) == TRUE)
		{
			m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
		}
		else
		{
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIBerserk(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_BERSERK)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////

	AgpdCharacter *pcsTarget = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_stPCAIInfo.lTargetID);
	if(NULL == pcsTarget)
		return FALSE;

	AuPOS stFrom		= pcsAgpdCharacter->m_stPos;
	AuPOS stTargetPos	= pcsTarget->m_stPos;
	FLOAT fSearchRange	= 200.0f;

	FLOAT fTempX, fTempZ;

	fTempX = pcsTarget->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
	fTempZ = pcsTarget->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

	if(fTempX*fTempX + fTempZ*fTempZ > (fSearchRange + 200.0f) * (fSearchRange + 200.0f))
	{
		m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stTargetPos, MD_NODIRECTION, FALSE, TRUE);
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

BOOL AgsmAI2::ProcessPCAIDisease(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.eStateAI != AGPDAI2_STATE_PC_AI_DISEASE)
		return FALSE;

	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN ||
		pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_HOLD)
		return FALSE;

	if(m_pcsAgpmCharacter->IsStatusSleep(pcsAgpdCharacter) == TRUE)
		return FALSE;

	INT32 lStunDurationTime = pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam1;
	INT32 lStunGenerateTime = pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam2;

	if(pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam3 + lStunGenerateTime > GetClockCount())
		return FALSE;

	///////////////////////////////////////////////////////////////////////////////////////////
	INT32 lAdjustProbability = 0;
	
	// if pcsCharacter has Stun Protect stun_probability has to decrease
	if(pcsAgpdCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_STUN_PROTECT)
	{
		AgsdCharacter *pcsAgsdCharacter = m_pcsAgsmCharacter->GetADCharacter(pcsAgpdCharacter);
		if(pcsAgsdCharacter)
			lAdjustProbability += pcsAgsdCharacter->m_stInvincibleInfo.lStunProbability;
	}

	if(lAdjustProbability > 0)
	{
		// Stun Fail
		if(m_csRand.randInt(100) < lAdjustProbability)
			return FALSE;
	}

	if(lStunDurationTime == 0)
		lStunDurationTime = 3000;

	// ĳ������ Special Status�� Set ���ش�.
	m_pcsAgsmCharacter->SetSpecialStatusTime(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_STUN, lStunDurationTime);

	// �������� ���� Stun �ɸ� �ð��� �������ش�.
	pcsAgpdAI2ADChar->m_stPCAIInfo.lReserveParam3 = GetClockCount();

	return TRUE;
}

INT32 AgsmAI2::CheckUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	//STOPWATCH2(GetModuleName(), _T("CheckUseSkill"));
	if(NULL == pcsAgpdCharacter)
		return -1;

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdAI2UseResult		*pcsUseResult;

	INT32 lUseSkillIndex = -1;

	if( pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
	{
		// 2007.01.25. steeple
		// ������ �տ��� ���� ���� �޹�ȣ�� Starvation �ɸ��� ���װ� �ִ�.
		// �׷��� ������ �����ϰ� �������ִ� ��ƾ�� �ۼ��ߴ�.
		INT32 lTotal = 0;
		for(INT32 i = 0; i < AGPDAI2_MAX_USABLE_SKILL_COUNT; ++i)
		{
			if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[i].m_lSkillID == 0)
				break;

			++lTotal;
		}

		if(lTotal == 0)
			return -1;

		if(lTotal <= 0 || lTotal >= AGPDAI2_MAX_USABLE_SKILL_COUNT)
			return -1;

		// STLPort ������ ��� ���� �׾ �迭������ ���� 2012-04-19 silvermoo
// 		std::vector<INT32> vcTmp;
// 		vcTmp.clear();
// 
// 		for(INT32 i = 0; i < lTotal; ++i)
// 			vcTmp.push_back(i);
// 
// 		std::random_shuffle(vcTmp.begin(), vcTmp.end());
// 
// 		std::vector<INT32>::iterator iter = vcTmp.begin();
// 		while(iter != vcTmp.end())

		INT32 arrTmp[AGPDAI2_MAX_USABLE_SKILL_COUNT] = {0, };

		for(INT32 i = 0; i < AGPDAI2_MAX_USABLE_SKILL_COUNT; ++i)
			arrTmp[i] = -1;

		for(INT32 i = 0; i < lTotal; ++i)
			arrTmp[i] = i;

		// ����
		for(INT32 i = lTotal; i > 1; i--)
		{
			INT32 randValue = rand() % i;

			INT32 tempValue = arrTmp[randValue];
			arrTmp[randValue] = arrTmp[i-1];
			arrTmp[lTotal-1] = tempValue;
		}

		INT32 lCounter = 0;
		INT32* plValue = NULL;

		for( plValue = arrTmp; plValue != NULL && *plValue != -1; plValue++ )
		{
			lCounter = *plValue;

			pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lCounter].m_csTable;
			pcsUseResult = &pcsAgpdAI2ADChar->m_csSkillResult[lCounter];

			if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
			{
				INT32			lHPNow, lHPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

				if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}
			else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
			{
				INT32			lMPNow, lMPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

				if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}
			else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
			{
				INT32			lSPNow, lSPMax;

				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPNow, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
				m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPMax, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

				if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
				{
					if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
					{
						lUseSkillIndex = lCounter;
						break;
					}
				}
			}
			// STLPort ������ ��� ���� �׾ �迭������ ���� 2012-04-19 silvermoo
			// ++iter;
		}
	}

	return lUseSkillIndex;
}

INT32 AgsmAI2::CheckUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	//STOPWATCH2(GetModuleName(), _T("CheckUseItem"));
	if(NULL == pcsAgpdCharacter)
		return -1;

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdAI2UseResult		*pcsUseResult;
	AgpdFactor				*pcsFactorResult;

	INT32				lCounter;
	INT32				lUseItemIndex;

	lUseItemIndex = -1;
	pcsFactorResult = NULL;

	pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);

	if(!pcsFactorResult)
		pcsFactorResult	= &pcsAgpdCharacter->m_csFactor;

	if(!pcsFactorResult)
		return lUseItemIndex;

	if( pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template )
	{
		for( lCounter=0; lCounter<AGPDAI2_MAX_USABLE_ITEM_COUNT; ++lCounter )
		{
			if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lCounter].m_lItemID != 0 )
			{
				pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lCounter].m_csTable;
				pcsUseResult = &pcsAgpdAI2ADChar->m_csItemResult[lCounter];

				if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
				{
					INT32			lHPNow, lHPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lHPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

					if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
				{
					INT32			lMPNow, lMPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lMPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

					if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
				{
					INT32			lSPNow, lSPMax;

					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
					m_pcsAgpmFactors->GetValue( &pcsAgpdCharacter->m_csFactor, &lSPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

					if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
					{
						if( CheckCondition( pcsConditionTbl, pcsUseResult, lClockCount ) )
						{
							lUseItemIndex = lCounter;
							break;
						}
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	return lUseItemIndex;
}

BOOL AgsmAI2::CheckPreemptive( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter, 
								AgpdAI2ADChar *pcsAgpdAI2ADChar, BOOL bUseScream)
{
	//STOPWATCH2(GetModuleName(), _T("CheckPreempTive"));

	AgpdAI2ConditionTable	*pcsConditionTbl;
	AgpdFactor				*pcsFactorResult;

	BOOL				bResult;
	BOOL				bAggroResult;
	INT32				lCounter = 0;
	INT32				lMaxPreemptiveCondition = 0;

	bResult = FALSE;
	pcsFactorResult = NULL;
	pcsConditionTbl = NULL;

	if (bUseScream)
	{
		// condition table 3���� ������ �׸񿡴� ��ũ�� �ߵ� ������ �־���´�.
		lCounter = 2;
		lMaxPreemptiveCondition = AGPDAI2_MAX_PREEMPTIVECONDITON;
	}
	else
	{
		lCounter = 0;
		lMaxPreemptiveCondition = AGPDAI2_MAX_PREEMPTIVECONDITON - 1;
	}

	//����� ����.
	if( pcsAgpdAI2ADChar )
	{
		for( ; lCounter < lMaxPreemptiveCondition; ++lCounter )
		{
			if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csPreemptiveCondition[lCounter].m_bSet )
			{
				pcsConditionTbl = &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csPreemptiveCondition[lCounter].m_csTable;

				if( pcsConditionTbl->m_lAggroParameter == AGPMAI2_TARGET_AGGRO_MOBSELF )
				{
					pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
				}
				else if( pcsConditionTbl->m_lAggroParameter == AGPMAI2_TARGET_AGGRO_TARGET )
				{
					pcsFactorResult = (AgpdFactor*)m_pcsAgpmFactors->GetFactor(&pcsAgpdTargetCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
				}

				if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_HP )
				{
					INT32			lHPNow, lHPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lHPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lHPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);

					if( CheckParameter( lHPNow, lHPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_MP )
				{
					INT32			lMPNow, lMPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lMPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lMPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

					if( CheckParameter( lMPNow, lMPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_SP )
				{
					INT32			lSPNow, lSPMax;

					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lSPNow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
					m_pcsAgpmFactors->GetValue( pcsFactorResult, &lSPMax, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

					if( CheckParameter( lSPNow, lSPMax, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
				else if( pcsConditionTbl->m_lTargetParameter == AGPMAI2_TARGET_PARAMETER_LV )
				{
					INT32				lMobLevel, lTargetLevel;
					INT32				lLevelGap;

					lMobLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdCharacter );
					lTargetLevel = m_pcsAgpmCharacter->GetLevel( pcsAgpdTargetCharacter );

					lLevelGap = lTargetLevel - lMobLevel;

					if( CheckParameter( lLevelGap, 0, pcsConditionTbl ) )
					{
						bResult = TRUE;
						break;
					}
				}
			}
			else
			{
				break;
			}
		}
	}

	//�Ÿ��� �ְ� ������ ���°��. �̷� ���� ������ ��׷ΰ� ���� ���� Return FALSE��.
	if( pcsConditionTbl == NULL )
	{
		bAggroResult = FALSE;
	}
	//���������� �Ÿ��� ������ �ִ� �����.
	else
	{
		//������ ���� ���.
		if( bResult == TRUE) //bResult = TRUE
		{
			//���� �����̾��ٸ�...
			if( pcsConditionTbl->m_bAggroAccept == FALSE )
			{
				bAggroResult = FALSE;
			}
			//��� 
			else
			{
				bAggroResult = TRUE;
			}		
		}
		//������ ���� ���� ���.
		else
		{
			//�������ٸ�... �ݴ�� ����Ѵ�.
			if( pcsConditionTbl->m_bAggroAccept == FALSE )
			{
				bAggroResult = TRUE;
			}
			//����̾��ٸ� �ݴ�� �����Ѵ�.
			else
			{
				bAggroResult = FALSE;
			}		
		}
	}

	return bAggroResult;
}

BOOL AgsmAI2::CheckUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount )
{
	BOOL				bResult;

	bResult = FALSE;

	//��ũ�� ������ �������� ����.
	if( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFellowWorker.m_lFellowWorkerTotalCount )
	{
		if( pcsAgpdAI2ADChar->m_bScreamUsed == FALSE )
		{
			if( m_pcsAgsmCharacter->IsNoAgro( pcsAgpdCharacter ) == FALSE )
			{
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseSkill( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC )
{
	//STOPWATCH2(GetModuleName(), _T("ProcessUseSkill"));

	BOOL				bResult;

	bResult = FALSE;

	if( pcsAgpdCharacter && pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		//��ų�� �ؿ��~ �ȳ�.
		bResult = m_pcsAgsmSkill->CastSkill( pcsAgpdCharacter, 
											pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
											pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillLevel,
											pcsTargetPC,
											FALSE,
											TRUE
											);
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseItem( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, INT32 lIndex, AgpdCharacter *pcsTargetPC )
{
	BOOL				bResult;

	bResult = FALSE;

	//�������� �������~ ������ ������ž�~~ ����~
	if( pcsAgpdCharacter && pcsAgpdAI2ADChar && pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
	{
		AgpdItem			*pcsItem;

		//�츮�� ���ʹ� TID�� �ָ� �������� ���� ���� �ڱ������� ����~
		//�̰� TID�� �ָ� ����ϰ� ��������~
		pcsItem = m_pcsAgsmItemManager->CreateItem( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseItem[lIndex].m_lItemID, pcsAgpdCharacter, 1 );

		if( pcsItem )
		{
			m_pcsAgsmItem->UseItem( pcsItem, TRUE );
		}
		else
		{
			//�������� �ȸ��������. 
			printf( "AgsmAI2::ProcessUseItem, Can't create item\n" );
		}
	}

	return bResult;
}

BOOL AgsmAI2::ProcessUseScream( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AgpdCharacter *pcsTargetPC )
{
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	AgpdCharacter		*pcsListCharacter;
	AgpdAI2ADChar		*pcsAgpdAI2ADMob;

	BOOL				bResult;
	INT32				lMaxFellowerWorkers;
	INT32				lCounter, lNumCID;
	INT32				lRealCounter;
	float				fRange;

	alCID.MemSetAll();
	alCID2.MemSetAll();

	bResult = FALSE;
	fRange = (FLOAT)m_pcsAgpmAI2->GetMaxFellowRange( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template );
	lMaxFellowerWorkers = m_pcsAgpmAI2->GetMaxFellowAcceptWorkers( pcsAgpdAI2ADChar->m_pcsAgpdAI2Template );
	lRealCounter = 0;

	//�ֺ��� �ݰ� fRange���� ���� ���͵��� ����.
	lNumCID = m_pcsApmMap->GetCharList( pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fRange,
										&alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										&alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	//MaxFellowerWorkers��ŭ�� ���͵鿡�� ����ġ�� �ش�.
	for( lCounter = 0; lCounter<lNumCID; lCounter++ )
	{
		// ��ũ���� ������ �ִ� ���͸� �����ϰ� lMaxFellowerWorkers�� ������ �Ǳ� ������ 1�� �����ش�.
		if( lRealCounter < (lMaxFellowerWorkers + 1))
		{
			pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[lCounter]);

			if( pcsListCharacter )
			{
				// 2007.03.05. steeple
				if( pcsListCharacter->m_bIsReadyRemove )
					continue;

				//���� �������� ������ �������� ����.
				if( m_pcsAgpmAI2->IsFellowWorker( &pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFellowWorker, ((AgpdCharacterTemplate *)pcsListCharacter->m_pcsCharacterTemplate)->m_lID ) )
				{
					if( pcsListCharacter->m_Mutex.WLock() )
					{
						INT32				lDecreaseAggroPointPerCycle;
						INT32				lMaxHP;

						//TargetPC�� ���ؼ� Slow��׷θ� �־��ش�.
						lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsListCharacter->m_csFactor);

						lDecreaseAggroPointPerCycle = (lMaxHP*pcsListCharacter->m_pcsCharacterTemplate->m_csDecreaseData.m_lSlowPercent)/100;

						//5ȸ ���� Slow ���� ��׷� ��������ŭ�� Slow ��׷θ� �ο��Ѵ�.
						if( lDecreaseAggroPointPerCycle == 0 )
						{
							lDecreaseAggroPointPerCycle = 1;
						}

						lDecreaseAggroPointPerCycle *= 7;

						m_pcsAgsmCharacter->AddAgroPoint( pcsListCharacter, pcsTargetPC, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseAggroPointPerCycle );

						pcsAgpdAI2ADMob = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

						//���� Ư���� Ÿ���� ���ٸ� �ٷ� ������ �� �� �ֵ��� Ÿ���� �������ش�.
						if( pcsAgpdAI2ADMob != NULL )
						{
							if( pcsAgpdAI2ADMob->m_lTargetID == 0 )
							{
								pcsAgpdAI2ADMob->m_lTargetID = pcsAgpdAI2ADChar->m_lTargetID;
							}
						}

						pcsListCharacter->m_Mutex.Release();

						lRealCounter++;

						bResult = TRUE;
					}
				}
			}
		}
		else
		{
			break;
		}
	}


	return bResult;
}

BOOL AgsmAI2::ProcessCheckArea( AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT lClockCount )
{
	if(NULL == pcsAgpdCharacter || NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if (m_pcsAgpmCharacter->IsStatusTame(pcsAgpdCharacter))
		return FALSE;

	// ������ ����� �� AreaCheck�� �������ش�.
	if(AGPDAI2_ESCAPE_ESCAPE == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
		return FALSE;

	FLOAT	fTempDistance;
	FLOAT	fTempX;
	FLOAT	fTempZ;

	if (pcsAgpdAI2ADChar->m_csFirstSpawnPos.x == 0.0f &&
		pcsAgpdAI2ADChar->m_csFirstSpawnPos.y == 0.0f &&
		pcsAgpdAI2ADChar->m_csFirstSpawnPos.z == 0.0f)
		return FALSE;

	fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;

	fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csFirstSpawnPos.x;
	fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csFirstSpawnPos.z;

	//���� ���������� 3��ȿ� �������� �ʴ´ٸ� ���� ��ġ�� ���ο� ��������Ʈ�� ��´�.
	// Spawn Radius�� �� ���� �ѵ��� �־.... �� 100m �� ����
	if( fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (10000 * 10000) )
	{
		{
			pcsAgpdAI2ADChar->m_csTargetPos	= pcsAgpdAI2ADChar->m_csFirstSpawnPos;

			//�̵��� ���� �������� �׳� �������ش�.
			//ProcessNPCDialog( (ApBase *)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING );

			m_pcsAgsmCharacter->InitHistory( pcsAgpdCharacter );
			m_pcsAgsmFactors->Resurrection( &pcsAgpdCharacter->m_csFactor, NULL );
			m_pcsAgpmCharacter->UpdatePosition( pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csFirstSpawnPos, FALSE, TRUE );

			pcsAgpdAI2ADChar->m_lTargetID	= AP_INVALID_CID;
			pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

			pcsAgpdAI2ADChar->m_pclPath->resetIndex();

			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}

		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::ProcessCheckTargetArea( AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdTargetCharacter )
{
	if (!pcsAgpdCharacter || !pcsAgpdTargetCharacter)
		return FALSE;

	if (m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdTargetCharacter))
		return FALSE;

	AgpdAI2ADChar *		pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(pcsAgpdAI2ADChar == NULL)
		return FALSE;
	
	if(IsRunawayStateCharacter(pcsAgpdCharacter) == TRUE)
		return FALSE;

	FLOAT	fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdTargetCharacter->m_stPos.x;
	FLOAT	fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdTargetCharacter->m_stPos.z;

	//���� ���������� 3��ȿ� �������� �ʴ´ٸ� ���� ��ġ�� ���ο� ��������Ʈ�� ��´�.
	// Spawn Radius�� �� ���� �ѵ��� �־.... �� 100m �� ����
	if( fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (10000 * 10000) )
	{
		// �ʹ� �ָ� �������ִ�.
		pcsAgpdAI2ADChar->m_lTargetID	= 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind	= FALSE;

		pcsAgpdAI2ADChar->m_pclPath->resetIndex();

		m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::CBSetCombatMode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAI2			*pThis			= (AgsmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdAI2ADChar	*pcsAI2ADChar	= pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	if (pcsAI2ADChar->m_bUseAI2 &&
		pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_NEXT_TIME);
	}

	return TRUE;
}

BOOL AgsmAI2::CBResetCombatMode(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmAI2			*pThis			= (AgsmAI2 *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdAI2ADChar	*pcsAI2ADChar	= pThis->m_pcsAgpmAI2->GetCharacterData(pcsCharacter);

	if (pcsAI2ADChar->m_bUseAI2 &&
		pcsCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD)
	{
		pThis->m_pcsAgsmCharacter->SetIdleInterval(pcsCharacter, AGSDCHAR_IDLE_TYPE_AI, AGSDCHAR_IDLE_INTERVAL_FOUR_SECONDS);
	}

	return TRUE;
}

// 2005.08.30. steeple
// AgsmCombat ���� Attack ���� �� �� �θ���.
BOOL AgsmAI2::CBAttackStart(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	pstAgsmCombatAttackResult pstAttackResult = static_cast<pstAgsmCombatAttackResult>(pData);
	AgsmAI2* pThis = static_cast<AgsmAI2*>(pClass);

	if(!pstAttackResult->pAttackChar || !pstAttackResult->pTargetChar)
		return FALSE;

	// Attacker �� PC �� �ƴϸ� �� �ʿ� ����.
//	if(!pThis->m_pcsAgpmCharacter->IsPC(pstAttackResult->pAttackChar))
//		return TRUE;

	return pThis->ProcessSetTargetToSummons(pstAttackResult->pAttackChar, pstAttackResult->pTargetChar->m_lID);
}

// 2005.09.06. steeple
// AgsmSkill ���� ��ų ������ �� ��Ű�� ���� ���Ŀ� �θ���.
BOOL AgsmAI2::CBAffectedSkill(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgpdSkill* pcsSkill = static_cast<AgpdSkill*>(pData);
	AgsmAI2* pThis = static_cast<AgsmAI2*>(pClass);
	AgpdCharacter* pcsTarget = static_cast<AgpdCharacter*>(pCustData);

	if(!pcsSkill->m_pcsBase)
		return FALSE;

	// Skill ���� ���� PC �� �ƴϸ� �� �ʿ� ����.
	if(!pThis->m_pcsAgpmCharacter->IsPC((AgpdCharacter*)pcsSkill->m_pcsBase))
		return TRUE;

	// Attack Skill (Debuff ����) �� �ƴϸ� �� �ʿ� ����.
//	if(!pThis->m_pcsAgpmPvP->IsAttackSkill(pcsSkill))
//		return TRUE;

	return pThis->ProcessSetTargetToSummons((AgpdCharacter*)pcsSkill->m_pcsBase, pcsTarget->m_lID);
}

// PinchWantedCharacter�� �ִ��� Request�� �ö� ó���ϴ� CB
BOOL AgsmAI2::CBPinchWantedRequest(PVOID pData, PVOID pClass, PVOID pCusData)
{
	if(!pData || !pClass)
		return FALSE;

	PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST *pRequestPacket = (PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST *)(pData);
	AgsmAI2 *pThis = static_cast<AgsmAI2*>(pClass);

	AgpdCharacter *pcsRequestUser		= pThis->m_pcsAgpmCharacter->GetCharacter(pRequestPacket->lUserID);
	AgpdCharacter *pcsPinchCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(pRequestPacket->lRequestCharacterID);
	if(!pcsRequestUser || !pcsPinchCharacter)
		return FALSE;

	AgsdCharacter* pagsdRequestUser = pThis->m_pcsAgsmCharacter->GetADCharacter(pcsRequestUser);
	if(!pagsdRequestUser)
		return FALSE;

	// ��Ŷ�� ���� 
	PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER pAnswerPacket;
	pAnswerPacket.lPinchCharacterID = pRequestPacket->lRequestCharacterID;

	AgpdAI2ADChar *pcsAI2ADChar = pThis->m_pcsAgpmAI2->GetCharacterData(pcsPinchCharacter);
	if(pcsAI2ADChar && pcsAI2ADChar->m_pcsAgpdAI2Template)
	{
		if(pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.empty() == FALSE)
		{
			INT32 lIndex = 0;

			vector<AgpdCharacter*>::iterator it = pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.begin();
			for( ; it !=  pcsAI2ADChar->m_csPinchInfo.pRequestMobVector.end(); it++)
			{
				AgpdCharacter *pcsWantedCharacter = *it;
				if(pcsWantedCharacter)
				{
					if(lIndex >= MAX_PINCH_WANTED_CHARACTER) break;

					pAnswerPacket.lPinchWantedCharacter[lIndex++] = pcsWantedCharacter->m_lID;
				}
			}
		}
	}

	AgsEngine::GetInstance()->SendPacket(pAnswerPacket, pagsdRequestUser->m_dpnidCharacter);

	return TRUE;
}

// 2005.09.06. steeple
// ���� Spawn Callback �� �ҷ��� �� �� �Լ� �ȿ��� �̳��� �ҷ��ְ� �ȴ�.
BOOL AgsmAI2::ProcessSpawnCastSkill(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// ���⼱ ���� ���ø� ���ش�.
	if(!m_pcsAgpmCharacter->IsMonster(pcsCharacter))
		return TRUE;

	AgpdAI2ADChar* pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	if(!pcsAgpdAI2ADChar || !pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return TRUE;

	AgpdSkillTemplate* pcsSkillTemplate = NULL;
	for(INT32 lIndex = 0; lIndex < AGPDAI2_MAX_USABLE_SKILL_COUNT; lIndex++)
	{
		if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID == 0)
			break;

		pcsSkillTemplate = m_pcsAgpmSkill->GetSkillTemplate(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID);
		if(!pcsSkillTemplate)
			continue;

		// Passive �� Self Buff �� ĳ��Ʈ ������.
		if(m_pcsAgpmSkill->IsPassiveSkill(pcsSkillTemplate) ||
			m_pcsAgpmSkill->IsSelfBuffSkill(pcsSkillTemplate))
		{
			if(m_pcsAgpmCharacter->IsStatusSummoner(pcsCharacter) || m_pcsAgpmCharacter->IsStatusFixed(pcsCharacter))
			{
				m_pcsAgsmSkill->CastSkill(pcsCharacter, 
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
										pcsAgpdAI2ADChar->m_lCastSkillLevel,	// �̳��� �ٸ�.
										NULL,
										FALSE,
										TRUE
										);
			}
			else
			{
				m_pcsAgsmSkill->CastSkill(pcsCharacter, 
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillID,
										pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csUseSkill[lIndex].m_lSkillLevel,
										NULL,
										FALSE,
										TRUE
										);
			}
		}
	}

	return TRUE;
}


void AgsmAI2::SetSummonAOIRange(int lRange)
{
	ASSERT(lRange);
	m_lSummonAOIRange = lRange;
}

void AgsmAI2::SetSummonPropensity(int lCID, int lPropensity)
{
	ASSERT(0 != lCID);
	
	AgpdCharacter *pcsCharacter = m_pcsAgpmCharacter->GetCharacter(lCID);

	ASSERT(pcsCharacter);
	if (NULL == pcsCharacter)
		return ;

	m_pcsAgpmSummons->SetPropensity(pcsCharacter, (EnumAgpdSummonsPropensity)lPropensity);
}

#ifdef __PROFILE__
void AgsmAI2::SetProfileDelay(int delay)
{
	SetProfileDelay((DWORD)delay);
}

void AgsmAI2::Sleep1sec()
{
	Sleep(1000);
}
#endif


BOOL AgsmAI2::SetCallbackGetTarget(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGSMAI2_CB_GET_TARGET, pfCallback, pClass);
}

BOOL AgsmAI2::CheckEscape(AgpdCharacter* pcsTarget)
{
	ASSERT(pcsTarget);

	if (NULL == pcsTarget)
		return FALSE;

	if (FALSE == m_pcsAgpmCharacter->IsMonster(pcsTarget))
		return FALSE;

	AgpdAI2ADChar *pstAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsTarget);
	if (NULL == pstAI2ADChar || 
		NULL == pstAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if (AGPDAI2_ESCAPE_ESCAPE == pstAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
	{
		// ��ó�� ������ ������ �̵��ϱ� ���� ��ǥ�� ã�´�.
		if (FALSE == GetTargetPosFromSpawn(pcsTarget, &pcsTarget->m_stPos, &pstAI2ADChar->m_csTargetPos, 
			&pstAI2ADChar->m_csSpawnBasePos, pstAI2ADChar->m_fSpawnRadius * 2.0f/* spawn ������ �ʹ� ���Ƽ� ���� ������*/))
			return FALSE;

		INT32 lRandTime = m_csRand.randInt() % 15000;
		pstAI2ADChar->m_ulNextWanderingTime = GetClockCount() + 15000 + lRandTime;
		m_pcsAgpmCharacter->MoveCharacter( pcsTarget, &pstAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE );

		return TRUE;
	}
		
	return FALSE;	
}

BOOL AgsmAI2::ManagementFollower(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	if(IsRunawayStateCharacter(pcsAgpdCharacter) == TRUE)
		return FALSE;

	// Follower���� ���� Mob�̳� ���� Follower���� �ʰ��� ����� �ѱ��.
	if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= 0)
		return FALSE;

	if(m_pcsAgpmAI2->GetCumulativeFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) <= pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower)
		return FALSE;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	
	AgpdAI2ADChar	*pcsAgpdAI2TempADChar = NULL;
	ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
	ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
	alCID.MemSetAll();
	alCID2.MemSetAll();

	INT32	lNumCID = 0;
	INT32	lCounter = 0;
	float	fVisibility = 2000.0f;
	float	fTempX, fTempZ = 0.0f;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility > 0.0f)
	{
		fVisibility = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_fVisibility;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	lNumCID = m_pcsApmMap->GetCharList(pcsAgpdCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsAgpdCharacter->m_stPos, fVisibility,
									   &alCID[0], AGSMAI2_MAX_PROCESS_TARGET, 
									   &alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

	AgpdCharacter *pcsFollowerCharacter = NULL;

	// Follower ĳ�����߿��� �þ� ������ ������ �༮���� ����Ʈ���� �����Ѵ�.
	for(lCounter=0; lCounter<AGPDAI2_MAX_FOLLOWER_COUNT; ++lCounter)
	{
		if(NULL == pcsAgpdAI2ADChar->m_pcsFollower[lCounter])
			continue;

		if(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_bIsReadyRemove)
			continue;

		if(IsRunawayStateCharacter(pcsAgpdAI2ADChar->m_pcsFollower[lCounter]) == TRUE)
			continue;

		fTempX = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdAI2ADChar->m_pcsFollower[lCounter]->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		// �þ� ������ �ڽ��� ���ϰ� ������ٸ� ����.(��κ� ����Ȯ�� ���)
		if(fTempX*fTempX + fTempZ*fTempZ > fVisibility*fVisibility)
		{
			pcsFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[lCounter];

			pcsAgpdAI2TempADChar = m_pcsAgpmAI2->GetCharacterData(pcsFollowerCharacter);

			if(pcsAgpdAI2TempADChar)
			{
				if(pcsFollowerCharacter && pcsFollowerCharacter->m_Mutex.WLock())
				{
					// ������ ������ �����ְ�..
					pcsAgpdAI2TempADChar->m_pcsBossCharacter = NULL;

					// ������ ���ϸ���Ʈ���� �����Ѵ�.
					pcsAgpdAI2ADChar->m_pcsFollower[lCounter] = NULL;
					pcsAgpdAI2ADChar->m_lCurrentFollowers--;

					pcsFollowerCharacter->m_Mutex.Release();
				}
			}
		}
	}

	for (int i=0; i<lNumCID; ++i)
	{
		AgpdCharacter *pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[i]);
		AgpdAI2ADChar *pcsAI2AdFollowerChar;
		INT32		  lFollowerTID;

		if(pcsListCharacter == NULL)
			continue;

		if(pcsListCharacter->m_bIsReadyRemove)
			continue;

		// �ڱ� �ڽ��� ����
		if(pcsListCharacter->m_lID == pcsAgpdCharacter->m_lID)
			continue;

		for( INT32 lFollowerCount=0; lFollowerCount<AGPDAI2_MAX_FOLLOWER_COUNT; ++lFollowerCount)
		{
			lFollowerTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csFollower.m_lFollowerInfo[lFollowerCount].m_lFollowerTID;

			if(lFollowerTID)
			{
				if(pcsListCharacter->m_pcsCharacterTemplate->m_lID == lFollowerTID)
				{
					if(pcsAgpdAI2ADChar->m_lCurrentFollowers >= m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template))
					{
						break;
					}

					pcsAI2AdFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);

					if(pcsListCharacter->m_Mutex.WLock())
					{
						if(m_pcsAgpmAI2->CheckIsBossMob(pcsAI2AdFollowerChar) == TRUE)
						{
							pcsListCharacter->m_Mutex.Release();
							continue;
						}

						pcsListCharacter->m_Mutex.Release();
					}

					if(pcsListCharacter->m_Mutex.WLock())
					{
						if(IsRunawayStateCharacter(pcsListCharacter) == TRUE)
						{
							pcsListCharacter->m_Mutex.Release();
							continue;
						}

						pcsListCharacter->m_Mutex.Release();
					}

					// ������ ���� ���� ��츸 �ڽ��� ���Ϸ� ����� �ִ�.
					if(pcsAI2AdFollowerChar && pcsAI2AdFollowerChar->m_pcsBossCharacter == NULL)
					{
						BOOL	bExist = FALSE;

						for(int j=0; j<AGPDAI2_MAX_FOLLOWER_COUNT; ++j)
						{
							if(pcsAgpdAI2ADChar->m_pcsFollower[j] == pcsListCharacter)
							{
								bExist = TRUE;
								break;
							}
						}
						
						if(bExist == FALSE)
						{
							if(!pcsListCharacter->m_Mutex.WLock())
								continue;

							pcsAI2AdFollowerChar->m_pcsBossCharacter = pcsAgpdCharacter;

							pcsListCharacter->m_Mutex.Release();
						

							for(int k=0; k<AGPDAI2_MAX_FOLLOWER_COUNT; ++k)
							{
								if(pcsAgpdAI2ADChar->m_pcsFollower[k] == NULL)
								{
									if(pcsListCharacter && pcsListCharacter->m_Mutex.WLock())
									{
										pcsAgpdAI2ADChar->m_pcsFollower[k] = pcsListCharacter;
										pcsAgpdAI2ADChar->m_lCurrentFollowers++;
										pcsAgpdAI2ADChar->m_lCurrentCumulativeFollower++;
										pcsListCharacter->m_Mutex.Release();
										break;
									}
								}
							}
						}
					}
				}
				else
				{
					break;
				}
			}
		}
	}

	return TRUE;
}

// FindEnemyTarget
AgpdCharacter* AgsmAI2::FindEnemyTarget(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, AuPOS csMobPos, float fPreemptiveRange)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return NULL;

	AgpdCharacter *pcsAgpdPCCharacter = NULL;

	// ������ ���� ĳ���� (������ �ڽ�, �������� �������� ���� ���͵�
	if(pcsAgpdAI2ADChar->m_pcsBossCharacter == NULL)
	{
		pcsAgpdPCCharacter = GetTargetPC(pcsAgpdCharacter, pcsAgpdAI2ADChar, csMobPos, fPreemptiveRange);

		// �������ΰ�� Follower�鿡�� ���� ������ ����Ѵ�.
		if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar))
		{
			SetFollowersTarget(pcsAgpdCharacter, csMobPos, pcsAgpdAI2ADChar);
		}
	}
	else
	{
		if(pcsAgpdAI2ADChar->m_lTargetID != 0)
		{
			pcsAgpdPCCharacter = m_pcsAgpmCharacter->GetCharacter(pcsAgpdAI2ADChar->m_lTargetID);
		}
		else
		{
			pcsAgpdPCCharacter = NULL;
		}
	}

	if(pcsAgpdPCCharacter)
	{
		if(pcsAgpdAI2ADChar->m_lTargetID != pcsAgpdPCCharacter->m_lID)
			pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;

		pcsAgpdAI2ADChar->m_lTargetID = pcsAgpdPCCharacter->m_lID;
	}
	else
	{
		pcsAgpdAI2ADChar->m_lTargetID = 0;
		pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;
	}

	return pcsAgpdPCCharacter;
}

// CheckArea 
// return ���� Spawn��ġ�� �Ű��ִ� ProcessCheckArea()�� ����ߴ��� ���ߴ��� ���θ� �����ش�. (TRUE�̸� ���� ������ϴϱ�)
BOOL AgsmAI2::CheckArea(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	BOOL bUseAreaAI = FALSE; 

	// Ÿ�ٰ��� �Ÿ��� ����Ѵ�. �ʹ� �ָ� �ִ� ���̸� Ÿ�ٿ��� �����Ѵ�.
	if(ProcessCheckTargetArea(pcsAgpdCharacter, pcsAgpdPCCharacter))
	{
		pcsAgpdPCCharacter = NULL;
	}

	bUseAreaAI = ProcessCheckArea(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);

	return bUseAreaAI;
}

// MoveToEnemy (Enemy�� �ִ� �༮���� Enemy���� ���� �༮���� Boss ��/���� ���� Boss���Է�)
// Return ���� EnemyTarget�� InRange�ȿ� �ִٴ� �Ҹ��� �� TRUE�� �������ָ� ���ݰ���
AgsmAI2ResultMove AgsmAI2::MoveAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return AGSMAI2_RESULT_MOVE_FAIL;

	AgsmAI2ResultMove eMoveReturn = AGSMAI2_RESULT_MOVE_FAIL;

	// ������ �ֿ������°� ���� �켱�����̴�.
	if(LootItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount) == TRUE)
	{
		return AGSMAI2_RESULT_MOVE_LOOT;
	}

	if(pcsAgpdPCCharacter && pcsAgpdCharacter->m_bIsTrasform == FALSE)
	{
		PROFILE("AGSMAI2::AttackTarget");

		BOOL bInRange  = FALSE;
		BOOL bPathFind = FALSE;
		BOOL bResult   = FALSE;

		AuPOS stDestPos;
		ZeroMemory(&stDestPos, sizeof(stDestPos));
		
		if(RunawayFromPC(pcsAgpdCharacter, pcsAgpdPCCharacter, pcsAgpdAI2ADChar, lClockCount) == TRUE)
		{
			eMoveReturn = AGSMAI2_RESULT_MOVE_RUNAWAY;
		}
		else
		{
			bInRange = m_pcsAgpmFactors->IsInRange(&pcsAgpdCharacter->m_stPos, &pcsAgpdPCCharacter->m_stPos, &pcsAgpdCharacter->m_csFactor,
												   &pcsAgpdPCCharacter->m_csFactor, &stDestPos);
		}

		// ���� ���� ���ΰ�?
		if(bInRange)
		{
			// �����Ҽ� �ִٴ� ���ϰ��� �ش�.
			eMoveReturn = AGSMAI2_RESULT_MOVE_TARGET_IN_RANGE;
		}
		// ���ݹ��� ���̶��
		else if (!pcsAgpdAI2ADChar->m_bIsNeedPathFind && eMoveReturn != AGSMAI2_RESULT_MOVE_RUNAWAY &&
				 pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE)
		{	
			if(FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				pcsAgpdAI2ADChar->m_csTargetPos = stDestPos;
				m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE);
			}
		}
		else if(eMoveReturn != AGSMAI2_RESULT_MOVE_RUNAWAY &&
			    pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE)
		{
			if(FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus))
			{
				pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdPCCharacter->m_stPos;

				// �������� ���� ã�Ƶд�
				bResult = m_pcsAgpmPathFind->pathFind(pcsAgpdCharacter->m_stPos.x,
													  pcsAgpdCharacter->m_stPos.z,
													  pcsAgpdAI2ADChar->m_csTargetPos.x,
													  pcsAgpdAI2ADChar->m_csTargetPos.z,
													  pcsAgpdAI2ADChar->m_pclPath);

				if(!bResult)
				{
					pcsAgpdCharacter->m_ulCheckBlocking = 0;
					pcsAgpdAI2ADChar->m_bIsNeedPathFind = FALSE;
				}

				CPriorityListNode<int, AgpdPathFindPoint*> *pcsNode;

				pcsNode = pcsAgpdAI2ADChar->m_pclPath->GetNextPath();

				if(pcsNode)
				{
					AuPOS csPos;

					csPos.x = pcsNode->data->m_fX;
					csPos.y = 0.0f;
					csPos.z = pcsNode->data->m_fY;

					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &csPos, MD_NODIRECTION, FALSE, TRUE);
				}
			}
		}
	}
	else if((FALSE == (AGPDCHAR_SPECIAL_STATUS_DONT_MOVE & pcsAgpdCharacter->m_ulSpecialStatus)) &&
		    (pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == FALSE))
	{
		if(MoveWandering(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount))
			eMoveReturn = AGSMAI2_RESULT_MOVE_WANDERING;
	}

	return eMoveReturn;
}

// AttackEnemy (Enemy�� Target���� �ȿ� ������ ������ �Ѵ�.)
BOOL AgsmAI2::AttackEnemy(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdPCCharacter == NULL || pcsAgpdAI2ADChar == NULL || pcsAgpdAI2ADChar->m_pcsAgpdAI2Template == NULL)
		return FALSE;

	// Normal Attack�� �����ϸ� �Ÿ��� ����Ѵ�. 
	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bUseNormalAttack)
	{
		// �����ص� �Ǹ� �����Ѵ�.
		if(lClockCount > pcsAgpdAI2ADChar->m_ulLastAttackTime + pcsAgpdAI2ADChar->m_lAttackInterval)
		{
			// �����Ҷ� ������ �� ȣ���Ѵ�.
			ProcessNPCDialog(pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_ATTACK);

			m_pcsAgpmCharacter->ActionCharacter(AGPDCHAR_ACTION_TYPE_ATTACK, pcsAgpdCharacter, (ApBase*)pcsAgpdPCCharacter, NULL);

			pcsAgpdAI2ADChar->m_ulLastAttackTime = lClockCount;
		}
		else
		{
			// ���� �̵����̸� �����
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}
	}
	else
	{
		// ���� �̵����̸� �����
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
	}

	return TRUE;
}

// MoveWandering (����ϴ� ���ֵ�)
BOOL AgsmAI2::MoveWandering(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	STMonsterPathInfo *pstMonsterPathInfo = NULL;

	if (pstMonsterPathInfo = m_pcsAgpmAI2->GetMonsterPathInfo(pcsAgpdCharacter))
	{
		// ���⿡ Path�� �ִ� Monster Move�� �ۼ��Ѵ�.
	}
	else
	{
		// ���ϸ��̸� �������� ������ ��ġ�� �����δ�.
		if(pcsAgpdAI2ADChar->m_pcsBossCharacter != NULL)
		{
			if(lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime)
			{
				// �ƹ��� ����
			}
			else
			{
				float		fTempX, fTempZ;

				fTempX = pcsAgpdAI2ADChar->m_csTargetPos.x - pcsAgpdCharacter->m_stPos.x;
				fTempZ = pcsAgpdAI2ADChar->m_csTargetPos.z - pcsAgpdCharacter->m_stPos.z;

				// �������� 1���Ͷ�� �������
				if(fTempX*fTempX + fTempZ*fTempZ <= 100.0f * 100.0f)
				{
				}
				else
				{
					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION, FALSE, TRUE);
				}
			}
		}
		else
		{
			// ���⼭ ���ʹ� �׳� ����ϸ鼭 �ٳ���ϴ� �༮��
			PROFILE("AgsmAI2::MoveWander");

			ApdEvent		*pcsEvent;
			pcsEvent = m_pcsAgpmEventSpawn->GetCharacterData(pcsAgpdCharacter)->m_pstEvent;

			if(pcsEvent != NULL)
			{
				if(lClockCount < pcsAgpdAI2ADChar->m_ulNextWanderingTime)
				{
					// �ƹ��� ����
				}
				else
				{
					// �̵����� �ƴѵ� ���� �н� ����Ʈ�� ������ ���� ã��
					if(!pcsAgpdCharacter->m_bMove)
					{
						if(!GetTargetPosFromSpawn(pcsAgpdCharacter, &pcsAgpdCharacter->m_stPos, &pcsAgpdAI2ADChar->m_csTargetPos, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius))
						{
							float			fTempX, fTempZ, fTempDistance;
							fTempDistance = pcsAgpdAI2ADChar->m_fSpawnRadius;
							fTempX = pcsAgpdCharacter->m_stPos.x - pcsAgpdAI2ADChar->m_csSpawnBasePos.x;
							fTempZ = pcsAgpdCharacter->m_stPos.z - pcsAgpdAI2ADChar->m_csSpawnBasePos.z;

							//���� ���������ȿ� �������� �ʴ´ٸ� ó�� ������ ��ġ�� ���ư���.
							if(fTempX*fTempX + fTempZ*fTempZ > fTempDistance*fTempDistance)
							{
								pcsAgpdAI2ADChar->m_csTargetPos = pcsAgpdAI2ADChar->m_csFirstSpawnPos;
							}
							else
							{
								return FALSE;
							}
						}

						INT32 lRandTime = m_csRand.randInt()%15000;
						pcsAgpdAI2ADChar->m_ulNextWanderingTime = lClockCount + 15000 + lRandTime;

						// �̵��� ���� �������� �׳� �������ش�.
						ProcessNPCDialog((ApBase*)pcsAgpdCharacter, AGPD_NPCDIALOG_MOB_WANDERING);
						m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsAgpdAI2ADChar->m_csTargetPos, MD_NODIRECTION);

						// �������̸� �ڽ��� ���ϵ鿡�Ե� ���� ��ġ�� �����ְ� ��ġ���� �� ��ã����� ó���� �̸� ���ش�.
						if(m_pcsAgpmAI2->GetMaxFollowers(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template) > 0 && m_pcsAgpmAI2->CheckIsBossMob(pcsAgpdAI2ADChar))
						{
							AgpdAI2ADChar	*pcsAI2ADFollowerChar;
							AgpdCharacter	*pcsAgpdFollowerCharacter;pcsAgpdFollowerCharacter;

							INT32			lCurrentFollower;
							INT32			lRealCounter;
							INT32			lDegreePerFollower;
							float			fFollowerMargin;
							float			fFormationTempX, fFormationTempZ;

							lCurrentFollower = pcsAgpdAI2ADChar->m_lCurrentFollowers;

							if(lCurrentFollower > 0)
							{
								lDegreePerFollower = 360/lCurrentFollower;
								lRealCounter = 0;
								fFollowerMargin = 300.0f + lCurrentFollower*30.0f;

								for(int i=0; i<AGPDAI2_MAX_FOLLOWER_COUNT; ++i)
								{
									pcsAgpdFollowerCharacter = pcsAgpdAI2ADChar->m_pcsFollower[i];

									if(pcsAgpdFollowerCharacter != NULL && pcsAgpdFollowerCharacter->m_Mutex.WLock())
									{
										fFormationTempX = sin(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;
										fFormationTempZ = cos(lDegreePerFollower*lRealCounter*AGSMAI2_Radian)*fFollowerMargin;

										pcsAI2ADFollowerChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdFollowerCharacter);

										// ���� �̵����̸� �����
										if(pcsAgpdFollowerCharacter->m_bMove)
											m_pcsAgpmCharacter->StopCharacter(pcsAgpdFollowerCharacter, NULL);

										// �������� �����ش�.
										pcsAI2ADFollowerChar->m_csTargetPos.x = pcsAgpdAI2ADChar->m_csTargetPos.x + fFormationTempX;
										pcsAI2ADFollowerChar->m_csTargetPos.z = pcsAgpdAI2ADChar->m_csTargetPos.z + fFormationTempZ;

										pcsAI2ADFollowerChar->m_ulNextWanderingTime = pcsAgpdAI2ADChar->m_ulNextWanderingTime;

										m_pcsAgpmCharacter->MoveCharacter(pcsAgpdFollowerCharacter, &pcsAI2ADFollowerChar->m_csTargetPos, MD_NODIRECTION);

										pcsAgpdFollowerCharacter->m_Mutex.Release();

										++lRealCounter;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	
	return TRUE;
}

// MonsterCommonAI (Monster�������� ������ϴ� AI�� ���ʿ��� ó���Ѵ�.)
BOOL AgsmAI2::MonsterCommonAI(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	AgpdCharacterTemplate *pcsTemplate = NULL;
	INT32 lMaxHP;
	INT32 lDecreaseScalar;

	pcsTemplate = pcsAgpdCharacter->m_pcsCharacterTemplate;
	if(NULL == pcsTemplate)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulNextDecSlowTime < lClockCount)
	{
		// 1/1000�� �����̹Ƿ� 1000�� ���Ѵ�.
		pcsAgpdAI2ADChar->m_ulNextDecSlowTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lSlowTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lSlowPercent)/100;

		if(lDecreaseScalar == 0)
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory(pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_SLOW, lDecreaseScalar);
	}

	if(pcsAgpdAI2ADChar->m_ulNextDecFastTime < lClockCount)
	{
		pcsAgpdAI2ADChar->m_ulNextDecFastTime = lClockCount + pcsTemplate->m_csDecreaseData.m_lFastTime*1000;

		lMaxHP = m_pcsAgpmFactors->GetMaxHP(&pcsAgpdCharacter->m_csFactor);
		lDecreaseScalar = (lMaxHP*pcsTemplate->m_csDecreaseData.m_lFastPercent)/100;

		if(lDecreaseScalar == 0)
			lDecreaseScalar = 1;

		m_pcsAgsmCharacter->DecreaseHistory(pcsAgpdCharacter, AGPD_FACTORS_AGRO_TYPE_FAST, lDecreaseScalar);
	}

	return TRUE;
}

// Runaway�߿��� �ݴ�������� �پ�� ��
BOOL AgsmAI2::RunawayFromPC(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter* pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdPCCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	BOOL bResult = FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime == 0)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == TRUE)
	{
		return FALSE;
	}

	if(lClockCount < pcsAgpdAI2ADChar->m_ulStartRunawayTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_uIEscapeTime)
	{
		AuPOS stFrom  = pcsAgpdCharacter->m_stPos;
		AuPOS stPCPos = pcsAgpdPCCharacter->m_stPos;
		AuPOS stDest;
		FLOAT fAttackRange = 0.0f;

		m_pcsAgpmFactors->GetValue(&pcsAgpdPCCharacter->m_pcsCharacterTemplate->m_csFactor, &fAttackRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);

		//fPreemptiveRange�ȿ� �����ϴ°�?
		FLOAT			fTempX, fTempZ;

		fTempX = pcsAgpdPCCharacter->m_stPos.x - pcsAgpdCharacter->m_stPos.x;
		fTempZ = pcsAgpdPCCharacter->m_stPos.z - pcsAgpdCharacter->m_stPos.z;

		if( fTempX*fTempX + fTempZ*fTempZ < (fAttackRange+5000.0f) * (fAttackRange+5000.0f) )
		{
			if(GetRunawayPosFromPC(pcsAgpdCharacter, &stFrom, &stPCPos, &stDest) == TRUE)
			{
				m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
			}
			else
			{
				if(GetTargetPosFromSpawn(pcsAgpdCharacter, &stFrom, &stDest, &pcsAgpdAI2ADChar->m_csSpawnBasePos, pcsAgpdAI2ADChar->m_fSpawnRadius))
				{
					m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &stDest, MD_NODIRECTION, FALSE, TRUE);
				}
			}	
		}
		else
		{
			if(pcsAgpdCharacter->m_bMove)
				m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);
		}

		bResult = TRUE;
	}
	else
	{
		if(pcsAgpdCharacter->m_bMove)
			m_pcsAgpmCharacter->StopCharacter(pcsAgpdCharacter, NULL);

		if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
			pcsAgpdAI2ADChar->m_ulStartRunawayTime = 0;

		bResult = TRUE;
	}

	return bResult;
}

// LOOTITEM
BOOL AgsmAI2::LootItem(AgpdCharacter *pcsAgpdCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return FALSE;

	INT32 nItemCount = 0;

	BOOL bRet = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_bDontMove == TRUE)
	{
		return FALSE;
	}

	// LootRange�� �ִٴ°Ŵ� ������ ��Ż�ڶ�� ��
	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange > 0)
	{
		ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_ITEM> arNearItemList;
		arNearItemList.MemSetAll();

		nItemCount = m_pcsApmMap->GetItemList(pcsAgpdCharacter->m_nDimension, pcsAgpdCharacter->m_stPos, pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_fLootItemRange,
										 &arNearItemList[0], AGSMAI2_MAX_PROCESS_ITEM);

		if(nItemCount > 0)
		{
			for(int i=0; i<nItemCount; ++i)
			{
				AgpdItem *pcsLootItem = m_pcsAgpmItem->GetItem(arNearItemList[i]);
				if(pcsLootItem)
				{
					// �� �������� ��ġ�� 1M�̳������� Ȯ���Ѵ�.
					FLOAT fTempX = pcsAgpdCharacter->m_stPos.x - pcsLootItem->m_posItem.x;
					FLOAT fTempZ = pcsAgpdCharacter->m_stPos.z - pcsLootItem->m_posItem.z;

					if(fTempX*fTempX + fTempZ*fTempZ > (FLOAT) (100 * 100))
					{
						m_pcsAgpmCharacter->MoveCharacter(pcsAgpdCharacter, &pcsLootItem->m_posItem, MD_NODIRECTION, FALSE, TRUE);
						break;
					}

					// �̵��ߴٸ� ���� �������� �ֿ��ش�.
					if (pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_MOVE ||
						pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_ATTACK ||
						pcsAgpdCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD)
						break;

					AgpdAI2LootItem stLootItem;

					if(pcsLootItem->m_Mutex.WLock())
					{
						stLootItem.m_ulItemID = pcsLootItem->m_lID;
						stLootItem.m_ulGetItemTime = lClockCount;
						
						pcsAgpdAI2ADChar->m_vLootItem.push_back(stLootItem);
						m_pcsAgpmItem->RemoveItemFromField(pcsLootItem, TRUE, FALSE);

						pcsLootItem->m_Mutex.Release();
					}
				}
			}

			bRet = TRUE;
		}

		CLootItemVector::iterator iter = pcsAgpdAI2ADChar->m_vLootItem.begin();

		while(iter != pcsAgpdAI2ADChar->m_vLootItem.end())
		{
			if(iter->m_ulItemID == 0) 
			{
				iter++;
				continue;
			}

			if(iter->m_ulGetItemTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csLootItemInfo.m_uILootItemTime < lClockCount)
			{
				AgpdItem *pcsItem = m_pcsAgpmItem->GetItem(iter->m_ulItemID);
				if(pcsItem)
				{
					if(m_pcsAgpmItem->DestroyItem(pcsItem))
					{
						iter = pcsAgpdAI2ADChar->m_vLootItem.erase(iter);
						continue;
					}
				}
			}

			iter++;
		}
	}
	
	return bRet;
}

AgsmAI2SpecificState AgsmAI2::SpecificStateAI(AgpdCharacter *pcsAgpdCharacter, AgpdCharacter *pcsAgpdPCCharacter, AgpdAI2ADChar *pcsAgpdAI2ADChar, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL || pcsAgpdAI2ADChar == NULL)
		return AGSMAI2_STATE_NONE;

	INT32	lIndex;
	AgsmAI2SpecificState eResult = AGSMAI2_STATE_NONE;

	lIndex = -1;

	if(pcsAgpdPCCharacter)
	{
		lIndex = CheckUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
	}
	else if(m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter))
	{
		// ���� ���� ���ʹ� �ϴ� ���⼭�� Ÿ���� ����.
		lIndex = CheckUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
	}

	// ��ų���
	if(lIndex != -1)
	{
		if(pcsAgpdPCCharacter != NULL ||
		   m_pcsAgpmSiegeWar->IsSiegeWarMonster(pcsAgpdCharacter))
		{
			BOOL bUseSkill = ProcessUseSkill(pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsAgpdPCCharacter);
			if(bUseSkill)
			{
				eResult = AGSMAI2_STATE_USE_SKILL;
			}
		}
	}
	else
	{
		lIndex = CheckUseItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount);
		// ������ ���
		if(lIndex != -1)
		{
			BOOL bUseItem = ProcessUseItem(pcsAgpdCharacter, pcsAgpdAI2ADChar, lIndex, pcsAgpdPCCharacter);
			if(bUseItem)
			{
				eResult = AGSMAI2_STATE_USE_ITEM;
			}
		}
		else
		{/*
			// ��ũ�����
			if(pcsAgpdPCCharacter && CheckUseScream(pcsAgpdCharacter, pcsAgpdAI2ADChar, lClockCount))
			{
				pcsAgpdAI2ADChar->m_bScreamUsed = ProcessUseScream(pcsAgpdCharacter, pcsAgpdAI2ADChar, pcsAgpdPCCharacter);
				if(TRUE == pcsAgpdAI2ADChar->m_bScreamUsed)
				{
					pcsAgpdAI2ADChar->m_ulScreamStartTime = GetClockCount();
					eResult = AGSMAI2_STATE_USE_SCREAM;
				}
			}
			*/
		}
	}

	if(eResult == AGSMAI2_STATE_NONE)
	{
		if(ProcessTransparentMonster(pcsAgpdCharacter, lClockCount) == TRUE)
		{
			eResult = AGSMAI2_STATE_USE_TRANSPARENT;
		}
	}

	return eResult;
}

BOOL AgsmAI2::GetRunawayPosFromPC(AgpdCharacter *pcsCharacter, AuPOS *pcsMobPos, AuPOS *pcsTargetPCPos, AuPOS *pcsMobTargetPos)
{
	if(pcsCharacter == NULL || pcsMobPos == NULL || pcsTargetPCPos == NULL || pcsMobTargetPos == NULL)
		return FALSE;

	BOOL	bResult = FALSE;

	AuPOS	stDirection;
	AuPOS	stTempAuPos;
	AuPOS	stFrom   = *pcsMobPos;
	AuPOS	stTarget = *pcsTargetPCPos;
	AuPOS	stDelta = stFrom - stTarget;
		
	FLOAT	fDistance = 500.0f;

	AuMath::V3DNormalize(&stDirection, &stDelta);


	// �������� ������ Direction�� 0.2�̸����� ���� ��Ǯ���ش�.
	if(stDirection.x < 0.2)
	{
		if(stDirection.x > 0)
		{
			stDirection.x += 0.5;
		}
		else
		{
			stDirection.x -= 0.5;
		}
	}

	if(stDirection.z < 0.2)
	{
		if(stDirection.z > 0)
		{
			stDirection.z += 0.5;
		}
		else
		{
			stDirection.z -= 0.5;
		}
	}

	stTempAuPos.x = (stFrom.x + (stDirection.x * fDistance));
	stTempAuPos.y = (stFrom.y + (stDirection.y * fDistance));
	stTempAuPos.z = (stFrom.z + (stDirection.z * fDistance));

	*pcsMobTargetPos = stTempAuPos;
	ApmMap::BLOCKINGTYPE eType = m_pcsAgpmCharacter->GetBlockingType(pcsCharacter);

	m_pcsAgpmCharacter->GetValidDestination(&stFrom, &stTempAuPos, pcsMobTargetPos, eType);

	if (m_pcsApmMap->CheckBlockingInfo(*pcsMobTargetPos , ApmMap::GROUND ) == ApTileInfo::BLOCKGEOMETRY)
		return FALSE;

	return TRUE;
	

/*
	AuPOS	stTempAuPos;
	AuPOS	stDirection = pcsCharacter->m_stDirection;

	for (INT32 nTry = 0; nTry < APMEVENT_CONDITION_MAX_TRY; ++nTry)
	{
		// �⺻ 5M�� ����ģ��.
		FLOAT fDistance = 500.0f;

		fDistance = fDistance;

		stTempAuPos.x = (pcsMobPos->x + (stDirection.x * fDistance));
		stTempAuPos.z = (pcsMobPos->z + (stDirection.z * fDistance));

		//���� ���ٸ� ��! �ѽξ�~
		if (m_pcsApmMap->CheckBlockingInfo(stTempAuPos , ApmMap::GROUND ) != ApTileInfo::BLOCKGEOMETRY)
		{
			pcsMobTargetPos->x = stTempAuPos.x;
			pcsMobTargetPos->z = stTempAuPos.z;

			bResult = TRUE;
			break;
		}
		else
		{
			AuPOS stDelta = *pcsTargetPCPos - *pcsMobPos;
			AuMath::V3DNormalize(&stDirection, &stDelta);
		}
	}
*/
	//return bResult;
}

BOOL AgsmAI2::CheckRunaway(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(pcsAgpdCharacter == NULL) 
		return FALSE;

	if (m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) && pcsAgpdCharacter->m_szID[0] != '\0')
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if (NULL == pcsAgpdAI2ADChar || 
		NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	// BOSS���� ��Ŵٸ� ����ġ���ʰ� ������ �һ縥��.
	if(pcsAgpdAI2ADChar->m_pcsBossCharacter)
		return FALSE;

	BOOL bResult = FALSE;

	// ����ġ�� ���¶�� ���⼭�� �׳� �Ѿ�ش�.
	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
		return FALSE;

	// �켱 RunawayAI�� ����ϴ� Monster�������� Ȯ���Ѵ�.
	if (AGPDAI2_ESCAPE_ESCAPE == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_eEscape)
	{
		if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_ulEscapeCount >= pcsAgpdAI2ADChar->m_ulEscapeCount)
		{
			INT32 lMaxHP = 0;
			INT32 lCurrentHP = 0;
			INT32 lRunawayHP = 0;

			lMaxHP = m_pcsAgpmCharacter->GetMaxHP(pcsAgpdCharacter);
			lCurrentHP = m_pcsAgpmCharacter->GetHP(pcsAgpdCharacter);

			lRunawayHP = (lMaxHP*pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_lEscapeHP) / 100;

			if(lRunawayHP > lCurrentHP)
			{
				pcsAgpdAI2ADChar->m_ulStartRunawayTime = lClockCount;
				pcsAgpdAI2ADChar->m_ulEscapeCount++;
				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgsmAI2::IsRunawayStateCharacter(AgpdCharacter *pcsAgpdCharacter)
{
	if(pcsAgpdCharacter == NULL)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(pcsAgpdAI2ADChar == NULL)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartRunawayTime != 0)
	{
		// ����ġ�� ������ �ð��� �ִ� ���� ���� �������̴�
		return TRUE;
	}

	return FALSE;
}

BOOL AgsmAI2::CheckHideMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter)
		return FALSE;

	if (m_pcsAgpmCharacter->IsPC(pcsAgpdCharacter) && pcsAgpdCharacter->m_szID[0] != '\0')
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(NULL == pcsAgpdAI2ADChar)
		return FALSE;

	if(!pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	BOOL bResult = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideCount > pcsAgpdAI2ADChar->m_ulHideCount)
	{
		if(pcsAgpdAI2ADChar->m_ulStartHideTime == 0)
		{
			INT32 lMaxHP = 0;
			INT32 lCurrentHP = 0;
			INT32 lHideHP = 0;
			INT32 lDuration = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_uIHideMonsterTime;

			lMaxHP = m_pcsAgpmCharacter->GetMaxHP(pcsAgpdCharacter);
			lCurrentHP = m_pcsAgpmCharacter->GetHP(pcsAgpdCharacter);

			lHideHP = (lMaxHP*pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterEscapeInfo.m_lEscapeHP) / 100;

			if(lHideHP > lCurrentHP)
			{
				pcsAgpdAI2ADChar->m_ulStartHideTime = lClockCount;
				pcsAgpdAI2ADChar->m_ulHideCount++;

				m_pcsAgsmCharacter->SetSpecialStatusTime(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT, (UINT32)lDuration);
				bResult = TRUE;
			}
		}
	}
	
	return bResult;
}

BOOL AgsmAI2::ProcessTransparentMonster(AgpdCharacter *pcsAgpdCharacter, UINT32 lClockCount)
{
	if(NULL == pcsAgpdCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsAgpdCharacter);
	if(NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(!pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_ulStartHideTime == 0)
		return FALSE;

	BOOL bRet = FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_lHideCount > 0)
	{
		if(pcsAgpdAI2ADChar->m_ulStartHideTime + pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_csMonsterHideInfo.m_uIHideMonsterTime < lClockCount)
		{
			m_pcsAgpmCharacter->UpdateUnsetSpecialStatus(pcsAgpdCharacter, AGPDCHAR_SPECIAL_STATUS_TRANSPARENT);

			pcsAgpdAI2ADChar->m_ulStartHideTime = 0;
			bRet = TRUE;
		}
	}

	return bRet;
}

BOOL AgsmAI2::SetPinchMonster(AgpdCharacter *pcsCharacter)
{
	if(NULL == pcsCharacter)
		return FALSE;

	AgpdAI2ADChar *pcsAgpdAI2ADChar = m_pcsAgpmAI2->GetCharacterData(pcsCharacter);
	if(NULL == pcsAgpdAI2ADChar || NULL == pcsAgpdAI2ADChar->m_pcsAgpdAI2Template)
		return FALSE;

	if(pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulPinchMonsterTID > 0)
	{
		if(pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter != NULL)
			return TRUE;

		AuAutoLock Lock(pcsCharacter->m_Mutex);
		if(!Lock.Result())
			return FALSE;

		UINT32 lPinchMonsterTID = pcsAgpdAI2ADChar->m_pcsAgpdAI2Template->m_ulPinchMonsterTID;

		ApSafeArray<INT_PTR, AGSMAI2_MAX_PROCESS_TARGET>	alCID;
		ApSafeArray<INT32, AGSMAI2_MAX_PROCESS_TARGET>		alCID2;
		alCID.MemSetAll();
		alCID2.MemSetAll();

		INT32	lNumCID = 0;
		float	fVisibility = 10000.0f;
		
		lNumCID = m_pcsApmMap->GetCharList(pcsCharacter->m_nDimension, APMMAP_CHAR_TYPE_MONSTER, pcsCharacter->m_stPos, fVisibility,
										   &alCID[0], AGSMAI2_MAX_PROCESS_TARGET,
										   &alCID2[0], AGSMAI2_MAX_PROCESS_TARGET);

		for(int i=0; i<lNumCID; ++i)
		{
			AgpdCharacter *pcsListCharacter = m_pcsAgpmCharacter->GetCharacter(alCID2[i]);

			if(NULL == pcsListCharacter || NULL == pcsListCharacter->m_pcsCharacterTemplate)
				continue;

			if(pcsListCharacter->m_bIsReadyRemove)
				continue;

			if (pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_PREDEAD ||
				pcsListCharacter->m_unActionStatus == AGPDCHAR_STATUS_DEAD)
				continue;

			AgpdAI2ADChar *pcsAI2ADCharacter = m_pcsAgpmAI2->GetCharacterData(pcsListCharacter);
			if(NULL == pcsAI2ADCharacter)
				continue;

			// �ڱ��ڽ��� ����.
			if(pcsListCharacter->m_lID == pcsCharacter->m_lID)
				continue;

			if(pcsListCharacter->m_pcsCharacterTemplate->m_lID == lPinchMonsterTID)
			{
				AuAutoLock Lock(pcsListCharacter->m_Mutex);
				if(!Lock.Result())
					continue;

				pcsAgpdAI2ADChar->m_csPinchInfo.ePinchType	= AGPMAI2_TYPE_PINCH_WANTED;
				pcsAI2ADCharacter->m_csPinchInfo.ePinchType = AGPMAI2_TYPE_PINCH_MONSTER;
				pcsAgpdAI2ADChar->m_csPinchInfo.pcsPinchCharcter = pcsListCharacter;
				pcsAI2ADCharacter->m_csPinchInfo.pRequestMobVector.push_back(pcsCharacter);
			}
		}
	}

	return TRUE;
}