/******************************************************************************
Module:  AgsmShrine.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 24
******************************************************************************/

#include "AgsmShrine.h"

AgsmShrine::AgsmShrine()
{
	SetModuleName("AgsmShrine");

	EnableIdle2(TRUE);

	m_lShrineGenerateID	= 1;
}

AgsmShrine::~AgsmShrine()
{
}

BOOL AgsmShrine::OnAddModule()
{
	m_papmObject		= (ApmObject *)			GetModule("ApmObject");
	m_papmMap			= (ApmMap *)			GetModule("ApmMap");

	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pagpmSkill		= (AgpmSkill *)			GetModule("AgpmSkill");
	m_pagpmShrine		= (AgpmShrine *)		GetModule("AgpmShrine");

	m_pagsmAOIFilter	= (AgsmAOIFilter *)		GetModule("AgsmAOIFilter");
	m_pAgsmServerManager	= (AgsmServerManager *)	GetModule("AgsmServerManager2");
	m_pagsmCharacter	= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pagsmSkill		= (AgsmSkill *)			GetModule("AgsmSkill");
	m_pagsmSkillManager	= (AgsmSkillManager *)	GetModule("AgsmSkillManager");

	if (!m_papmObject || !m_papmMap || !m_pagpmFactors || !m_pagpmCharacter || !m_pagpmSkill || !m_pagpmShrine || !m_pagsmAOIFilter || !m_pagsmCharacter || !m_pagsmSkill || !m_pagsmSkillManager || !m_pAgsmServerManager)
		return FALSE;

	m_nIndexADSector = m_papmMap->AttachSectorData(this, sizeof(AgsdShrineADSector), CBConAgsdADSector, CBDesAgsdADSector);
	if (m_nIndexADSector < 0)
		return FALSE;

	if (!m_pagpmShrine->SetCallbackGenerateShrineEvent(CBGenerateShrineEvent, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackActiveShrine(CBActiveShrine, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackInactiveShrine(CBInactiveShrine, this))
		return FALSE;

	if (!m_pagsmCharacter->SetCallbackReCalcResultFactor(CBReCalcResultFactor, this))
		return FALSE;

	if (!m_pagpmShrine->SetCallbackAddShrineEvent(CBAddShrineEvent, this))
		return FALSE;

	if (!m_pagsmAOIFilter->SetCallbackAddChar(CBAddChar, this))
		return FALSE;
	if (!m_pagsmAOIFilter->SetCallbackMoveChar(CBMoveChar, this))
		return FALSE;

	return TRUE;
}

BOOL AgsmShrine::OnInit()
{
	for (int i = 0; i < AGPMCHAR_MAX_UNION; i++)
	{
		m_pagpmFactors->InitFactor(&m_csUnionFactor[i]);
		m_csUnionFactor[i].m_bPoint = TRUE;
	}

	return TRUE;
}

BOOL AgsmShrine::OnDestroy()
{
	return TRUE;
}

BOOL AgsmShrine::OnIdle2(UINT32 ulClockCount)
{
	// shrine active, inactive ó���� ���ش�.

	INT32		lIndex		= 0;
	for (AgpdShrine *pcsShrine = m_pagpmShrine->GetShrineSequence(&lIndex); pcsShrine; pcsShrine = m_pagpmShrine->GetShrineSequence(&lIndex))
	{
		if (!pcsShrine->m_Mutex.WLock())
			continue;

		if (pcsShrine->m_bIsActive)
		{
			// active�� ��� inactive �Ǵ� ������ �˻��ؼ� ������ �����Ѵٸ� inactive �����ش�.
			switch (pcsShrine->m_pcsTemplate->m_eActiveCondition) {
			case AGPMSHRINE_ACTIVE_ALWAYS:
				break;

			case AGPMSHRINE_ACTIVE_DAY_ONLY:
				{
					// ���� �ƴ϶�� inactive
					if (!m_pagpmShrine->CheckDay())
					{
						m_pagpmShrine->InactiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;

			case AGPMSHRINE_ACTIVE_SPAWN:
				{
					// ���� active duration �ð��� �������� �˻��Ѵ�.
					if (pcsShrine->m_ulLastActiveDurationMSec < ulClockCount - pcsShrine->m_ulLastActiveTimeMSec)
					{
						// inactive ��Ų��.
						m_pagpmShrine->InactiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;
			}
		}
		else
		{
			// inactive�� ��� active�� ������ �˻��ؼ� ������ �����ϸ� active �����ش�.
			switch (pcsShrine->m_pcsTemplate->m_eReActiveCondition) {
			case AGPMSHRINE_REACTIVE_DAY_ONLY:
				{
					// ���̶�� active
					if (m_pagpmShrine->CheckDay())
					{
						m_pagpmShrine->ActiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;

			case AGPMSHRINE_REACTIVE_TWICE_LAST_ACTIVE_TIME:
				{
					// ���� last active duration �ð��� 2�谡 �������� �˻��Ѵ�.
					if (pcsShrine->m_ulLastActiveDurationMSec * 2 < ulClockCount - pcsShrine->m_ulLastInactiveTimeMSec)
					{
						// active ��Ų��.
						m_pagpmShrine->ActiveShrine(pcsShrine, ulClockCount);
					}
				}
				break;
			}
		}

		pcsShrine->m_Mutex.Release();
	}

	return TRUE;
}

BOOL AgsmShrine::CBGenerateShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmShrine		*pThis			= (AgsmShrine *)	pClass;
	ApdEvent		*pcsEvent		= (ApdEvent *)		pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *) pCustData;

	AgpdShrine		*pcsShrine		= (AgpdShrine *)	pcsEvent->m_pvData;
	if (pcsShrine)
	{
		// shrine�� active �������� ����.
		if (!pcsShrine->m_bIsActive)
			return FALSE;

		// ��ų�� Ÿ���� pcsCharacter�� �����Ѵ�.
		for (int i = 0; pcsShrine->m_lSkillID[i] != AP_INVALID_SKILLID; i++)
		{
			AgpdSkill *pcsSkill = pThis->m_pagpmSkill->GetSkill(pcsShrine->m_lSkillID[i]);
			if (pcsSkill)
			{
				pcsSkill->m_csTargetBase.m_eType	= pcsCharacter->m_eType;
				pcsSkill->m_csTargetBase.m_lID		= pcsCharacter->m_lID;
			}
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBActiveShrine(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmShrine		*pThis		= (AgsmShrine *) pClass;
	AgpdShrine		*pcsShrine	= (AgpdShrine *) pData;

	if (!pcsShrine->m_pcsBase || !pcsShrine->m_pcsTemplate)
		return FALSE;

	// shrine object�� active ���·� �����.
	pThis->m_papmObject->UpdateStatus(pcsShrine->m_pcsBase->m_lID, APDOBJ_STATUS_ACTIVE_TYPE1);

	// �� shrine�� ���� ��ų�� �����ٰ� object�� ���δ�.
	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; i++)
	{
		if (!pcsShrine->m_pcsTemplate->m_szSkillName[i] || !strlen(pcsShrine->m_pcsTemplate->m_szSkillName[i]))
			continue;

		AgpdSkillTemplate *pcsSkillTemplate = pThis->m_pagpmSkill->GetSkillTemplate(pcsShrine->m_pcsTemplate->m_szSkillName[i]);
		if (pcsSkillTemplate)
		{
			AgpdSkill *pcsSkill = pThis->m_pagsmSkillManager->CreateSkill(pcsShrine->m_pcsBase, pcsSkillTemplate->m_lID);
			if (!pcsSkill)
				continue;

			pcsSkill->m_csTargetBase.m_eType	= APBASE_TYPE_NONE;
			pcsSkill->m_csTargetBase.m_lID		= 0;

			if (!pThis->m_pagsmSkill->CastSkill(pcsSkill))
			{
				pThis->m_pagpmSkill->RemoveSkill(pcsSkill->m_lID);
				continue;
			}

			pcsShrine->m_lSkillID[i] = pcsSkill->m_lID;
		}
	}

	// �� shrine�� ��ġ�� �ִ� sector�� attach �س��� ����Ÿ�� ������Ʈ �Ѵ�.
	//		(�ش� sector�� �� shrine ������ �����Ѵ�.)

	ApWorldSector *pcsSector = pThis->m_papmMap->GetSector(pcsShrine->m_posShrine);
	if (pcsSector)
	{
		AgsdShrineADSector *pcsADSector = pThis->GetADSector(pcsSector);
		if (pcsADSector)
		{
			pcsADSector->pcsShrine = pcsShrine;
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBInactiveShrine(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;
	
	AgsmShrine		*pThis		= (AgsmShrine *) pClass;
	AgpdShrine		*pcsShrine	= (AgpdShrine *) pData;

	// shrine object�� inactive ���·� �����.
	pThis->m_papmObject->UpdateStatus(pcsShrine->m_pcsBase->m_lID, APDOBJ_STATUS_NORMAL);

	// �� shrine�� ���� ��ų�� ���ش�.
	for (int i = 0; i < AGPMSHRINE_MAX_LEVEL; i++)
	{
		if (pcsShrine->m_lSkillID[i] != AP_INVALID_SKILLID)
		{
			pThis->m_pagpmSkill->RemoveSkill(pcsShrine->m_lSkillID[i]);

			pcsShrine->m_lSkillID[i] = AP_INVALID_SKILLID;
		}
	}

	// �� shrine�� ��ġ�� �ִ� sector�� attach �س��� ����Ÿ�� ������Ʈ �Ѵ�.
	//		(�� shrine�� ��ġ�� �ִ� sector�� shrine������ �ʱ�ȭ�Ѵ�)

	ApWorldSector *pcsSector = pThis->m_papmMap->GetSector(pcsShrine->m_posShrine);
	if (pcsSector)
	{
		AgsdShrineADSector *pcsADSector = pThis->GetADSector(pcsSector);
		if (pcsADSector)
		{
			pcsADSector->pcsShrine = NULL;
		}
	}

	return TRUE;
}

BOOL AgsmShrine::CBReCalcResultFactor(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmShrine		*pThis			= (AgsmShrine *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	INT32	lUnion = pThis->m_pagpmCharacter->GetUnion(pcsCharacter);
	if (lUnion < 0 || lUnion >= AGPMCHAR_MAX_UNION)
		return FALSE;

	pThis->m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, &pThis->m_csUnionFactor[lUnion], TRUE, FALSE, TRUE, FALSE);

	return TRUE;
}

BOOL AgsmShrine::CBAddShrineEvent(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	ApdEvent			*pstEvent		= (ApdEvent *)				pData;
	AgsmShrine			*pThis			= (AgsmShrine *)			pClass;
	AgpdShrineTemplate	*pcsTemplate	= (AgpdShrineTemplate *)	pCustData;

	// shrine�� �߰��Ѵ�.
	AgpdShrine			*pcsShrine		= pThis->m_pagpmShrine->AddShrine(pcsTemplate->m_lID, pcsTemplate->m_lID);
	if (!pcsShrine)
		return FALSE;

	++pThis->m_lShrineGenerateID;

	//pstEvent->m_pvData = pcsShrine;

	return TRUE;
}

BOOL AgsmShrine::CBConAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	ApWorldSector	*pcsSector	= (ApWorldSector *)	pData;
	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;

	AgsdShrineADSector	*pcsADSector	= pThis->GetADSector(pcsSector);
	if (!pcsADSector)
		return FALSE;

	pcsADSector->pcsShrine = NULL;

	return TRUE;
}

BOOL AgsmShrine::CBDesAgsdADSector(PVOID pData, PVOID pClass, PVOID pCustData)
{
	return TRUE;
}

AgsdShrineADSector*	AgsmShrine::GetADSector(PVOID pvSector)
{
	if (!pvSector)
		return FALSE;

	return (AgsdShrineADSector *) m_papmMap->GetAttachedModuleData(m_nIndexADSector, pvSector);
}

BOOL AgsmShrine::CBAddChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
//2004.7.16 ���ؿ��� ���÷� �ּ�ó������.
/*	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;
	ApWorldSector	*pSector	= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter) return FALSE;

	// �� ������ �����ϴ� ���Ͱ� �ƴѰ�� �ƹ����� ���Ѵ�.
	AgsdServer *pcsThisServer			= pThis->m_pAgsmServerManager->GetThisServer();

	if (pcsAgsdCharacter->m_ulServerID != pcsThisServer->m_lServerID)
		return FALSE;

	for (int i = (int) pSector->GetIndexX() - 1; i <= (int) pSector->GetIndexX() + 1; i++)
	{
		for (int j = (int) pSector->GetIndexZ() - 1; j <= (int) pSector->GetIndexZ() + 1; j++)
		{
			ApWorldSector *pAroundSector = pThis->m_pagsmCharacter->GetSectorBySectorIndex(i, 0, j);
			if (!pAroundSector)
				continue;

			AgsdShrineADSector *pcsADSector = pThis->GetADSector(pAroundSector);
			if (!pcsADSector)
				continue;

			if (pcsADSector->pcsShrine)
			{
				INT16	nPacketLength = 0;

				PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketAddShrine(pcsADSector->pcsShrine, &nPacketLength);

				if (!pvPacket || nPacketLength < 1)
					continue;

				pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

				pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
			}
		}
	}*/

	return TRUE;
}

BOOL AgsmShrine::CBMoveChar(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	/*

	AgsmShrine		*pThis		= (AgsmShrine *)	pClass;
	ApWorldSector	*pSector	= (ApWorldSector *)	pData;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pCustData;

	AgsdCharacter	*pcsAgsdCharacter	= pThis->m_pagsmCharacter->GetADCharacter(pcsCharacter);
	if (!pcsAgsdCharacter) return FALSE;

	// �� ������ �����ϴ� ���Ͱ� �ƴѰ�� �ƹ����� ���Ѵ�.
	AgsdServer *pcsThisServer			= pThis->m_pagsmServerManager->GetThisServer();

	if (pcsAgsdCharacter->m_ulServerID != pcsThisServer->m_lServerID)
		return FALSE;

	ApWorldSector	*paForwardSector[9];
	ApWorldSector	*paBackwardSector[9];
	ZeroMemory(&paForwardSector, sizeof(ApWorldSector *) * 9);
	ZeroMemory(&paBackwardSector, sizeof(ApWorldSector *) * 9);

	// �̵� ������ ���� ���� ����Ÿ ��ũ�� ���߾�� �ϴ� ���͸� ���Ѵ�.
	if (!pThis->m_pagsmCharacter->GetForwardSector(pcsAgsdCharacter->m_pPrevSector, pcsAgsdCharacter->m_pCurrentSector, paForwardSector, paBackwardSector))
	{
		return FALSE;
	}

	for (int i = 0; i < 9; i++)
	{
		if (!paBackwardSector[i])
			continue;

		AgsdShrineADSector *pcsADSector = pThis->GetADSector(paBackwardSector[i]);
		if (!pcsADSector)
			continue;

		if (pcsADSector->pcsShrine)
		{
			INT16	nPacketLength = 0;

			PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketRemoveShrine(pcsADSector->pcsShrine, &nPacketLength);

			if (!pvPacket || nPacketLength < 1)
				continue;

			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
		}

	}

	for (i = 0; i < 9; i++)
	{
		if (!paForwardSector[i])
			continue;

		AgsdShrineADSector *pcsADSector = pThis->GetADSector(paForwardSector[i]);
		if (!pcsADSector)
			continue;

		if (pcsADSector->pcsShrine)
		{
			INT16	nPacketLength = 0;

			PVOID	pvPacket = pThis->m_pagpmShrine->MakePacketAddShrine(pcsADSector->pcsShrine, &nPacketLength);

			if (!pvPacket || nPacketLength < 1)
				continue;

			pThis->SendPacket(pvPacket, nPacketLength, pThis->m_pagsmCharacter->GetCharDPNID(pcsCharacter));

			pThis->m_pagpmShrine->m_csPacket.FreePacket(pvPacket);
		}
	}

	*/

	return TRUE;
}

BOOL AgsmShrine::CBSpawnCharacter(PVOID pvData, PVOID pvClass, PVOID pvCustData)
{
	ApdEvent*			pcsEvent = (ApdEvent *) pvData;
	AgpdCharacter *		pcsCharacter = (AgpdCharacter *) pvCustData;
	AgsmShrine *		pThis = (AgsmShrine *) pvClass;

	ASSERT(pcsEvent && pcsCharacter && pThis);

	if (!pcsEvent->m_pvData)
		return FALSE;

	AgpdSpawn			*pcsSpawn = (AgpdSpawn *) pcsEvent->m_pvData;

	AgpdShrineADSpawn	*pcsAttachSpawnData = pThis->m_pagpmShrine->GetADSpawn(pcsSpawn);

	if (strlen(pcsAttachSpawnData->m_szShrineName))
	{
		// spawn�� shrine ������ ���õǾ� �ִ� ����̴�.
		// spawn�� character�� shrine spawn ������ �����Ѵ�.

		if (pcsAttachSpawnData->m_lShrineID == 0)
		{
			AgpdShrineTemplate	*pcsShrineTemplate = pThis->m_pagpmShrine->GetShrineTemplate(pcsAttachSpawnData->m_szShrineName);
			if (pcsShrineTemplate)
			{
				// shrine data id�� shrine template data id�� �����ϴٴ� �����Ͽ� ���ߵǾ���.
				pcsAttachSpawnData->m_lShrineID = pcsShrineTemplate->m_lID;
			}
			else
				return FALSE;
		}

		AgpdShrineADChar	*pcsAttachCharData = pThis->m_pagpmShrine->GetADCharacter(pcsCharacter);

		pcsAttachCharData->m_bIsGuardian	= TRUE;
		pcsAttachCharData->m_lShrineID		= pcsAttachSpawnData->m_lShrineID;
		pcsAttachCharData->m_lGuardianLevel	= pcsAttachSpawnData->m_lShrineLevel;
	}

	return TRUE;
}