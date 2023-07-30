/******************************************************************************
Module:  AgsmFactors.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 02. 05
******************************************************************************/

#include "AgsmFactors.h"
#include "AuMath.h"

#include <math.h>

AgsmFactors::AgsmFactors()
{
	SetModuleName("AgsmFactors");

	SetModuleType(APMODULE_TYPE_SERVER);
}

AgsmFactors::~AgsmFactors()
{
}

BOOL AgsmFactors::OnInit()
{
	m_pagpmFactors		= (AgpmFactors *) GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem			= (AgpmItem *) GetModule("AgpmItem");
	m_pagpmTitle		= (AgpmTitle *) GetModule("AgpmTitle");

	if (!m_pagpmFactors ||
		!m_pagpmCharacter ||
		!m_pagpmItem ||
		!m_pagpmTitle
		)
		return FALSE;

	return TRUE;
}

BOOL AgsmFactors::OnDestroy()
{
	return TRUE;
}

FLOAT AgsmFactors::Max(FLOAT lValue1, FLOAT lValue2)
{
	if (lValue1 > lValue2)
		return lValue1;

	return lValue2;
}

FLOAT AgsmFactors::Min(FLOAT lValue1, FLOAT lValue2)
{
	if (lValue1 < lValue2)
		return lValue1;

	return lValue2;
}

BOOL AgsmFactors::ResetResultFactorDamageInfo(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	AgpdFactor	*pcsResultFactor	= (AgpdFactor *) m_pagpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsResultFactor)
		return FALSE;

	return m_pagpmFactors->InitDamageFactors(pcsResultFactor);
}

//		IsDead
//	Functions
//		- pcsCharacter�� ���� �׾����� �˻��Ѵ�. ���� �׾��ٸ� ProcessDeath()�� ȣ���Ѵ�.
//	Arguments
//		- pcsFactor : �˻��� ĳ���� factor
//	Return value
//		- BOOL : �׾����� TRUE��... �ƴ� FALSE�� �����Ѵ�.
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmFactors::IsDead(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lHP		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	if (lHP <= 0)
	{
		//ProcessDead(pcsFactor);

		return TRUE;
	}
	else
	{
		// ��Ҵ�.
		return FALSE;
	}

	return FALSE;
}

//		Resurrection
//	Functions
//		- ���� ĳ������ factor���� ��Ȱ�ķ� �ʱ�ȭ ��Ų��.
//	Arguments
//		- pcsCharacter : ����� ĳ����
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmFactors::Resurrection(AgpdFactor *pcsFactor, PVOID *pvPacket)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lMaxHP	= 0;
	INT32	lMaxMP	= 0;
	INT32	lMaxSP	= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	// Full�� ä���ֳ�.
	return UpdateCharPoint(pcsFactor,
						 pvPacket,
						 lMaxHP,
						 lMaxMP,
						 lMaxSP
						 );
}

PVOID AgsmFactors::SetCharPointFull(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lMaxHP	= 0;
	INT32	lMaxMP	= 0;
	INT32	lMaxSP	= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	PVOID	pvPacketFactor	= NULL;

	/*
	return UpdateCharPoint(pcsFactor,
						 pvPacket,
						 (INT32) (pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_HP] / 2.0),
						 (INT32) (pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_MP] / 2.0),
						 (INT32) (pcsFactorCharPointMax->lValue[AGPD_FACTORS_CHARPOINT_TYPE_SP] / 2.0)
						 );
	*/

	// Full�� ä���ֳ�.
	if (!UpdateCharPoint(pcsFactor,
						 &pvPacketFactor,
						 lMaxHP,
						 lMaxMP,
						 lMaxSP
						 ))
		return NULL;

	return pvPacketFactor;
}

BOOL AgsmFactors::ResetCharMaxPoint(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	// HP = CON * ((CON * 0.17 + 2.5) - (CON * 0.085))
	// MP = WIS * ((WIS * 0.17 + 2.5) - (WIS * 0.085))
	// SP = DEX * ((DEX * 0.17 + 2.5) - (DEX * 0.085))

	FLOAT	fCon	= 0.0f;
	FLOAT	fWis	= 0.0f;
	FLOAT	fDex	= 0.0f;

	m_pagpmFactors->GetValue(pcsFactor, &fCon, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
	m_pagpmFactors->GetValue(pcsFactor, &fWis, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
	m_pagpmFactors->GetValue(pcsFactor, &fDex, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

	if (fCon > 0.0f)
	{
		INT32	lMaxHP	= (INT32) (fCon / 5.0f * 40);

		m_pagpmFactors->SetValue(pcsFactor, lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	}

	if (fWis > 0.0f)
	{
		INT32	lMaxMP	= (INT32) (fWis / 6.5f * 35);

		m_pagpmFactors->SetValue(pcsFactor, lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	}

	if (fDex > 0.0f)
	{
		INT32	lMaxSP	= (INT32) (fDex / 6.5f * 35);

		m_pagpmFactors->SetValue(pcsFactor, lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);
	}

	return TRUE;
}

BOOL AgsmFactors::AdjustCharMaxPoint(AgpdFactor *pcsFactor)
{
	// �ִ�ġ���� ���� ����Ʈ���� �� ũ�ٸ� ������ ���ش�.
	
	INT32	lMaxHP		= 0;
	INT32	lMaxMP		= 0;
	INT32	lMaxSP		= 0;

	INT32	lCurrentHP	= 0;
	INT32	lCurrentMP	= 0;
	INT32	lCurrentSP	= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	m_pagpmFactors->GetValue(pcsFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (lMaxHP > 0 &&
		lMaxHP < lCurrentHP)
		m_pagpmFactors->SetValue(pcsFactor, lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	if (lMaxMP > 0 &&
		lMaxMP < lCurrentMP)
		m_pagpmFactors->SetValue(pcsFactor, lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);

	if (lMaxSP > 0 &&
		lMaxSP < lCurrentSP)
		m_pagpmFactors->SetValue(pcsFactor, lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	return TRUE;
}

BOOL AgsmFactors::AdjustCharMaxPoint(AgpdFactor *pcsBaseFactor, AgpdFactor *pcsTargetFactor)
{
	if (!pcsBaseFactor || !pcsTargetFactor)
		return FALSE;

	INT32	lMaxHP		= 0;
	INT32	lMaxMP		= 0;
	INT32	lMaxSP		= 0;

	INT32	lCurrentHP	= 0;
	INT32	lCurrentMP	= 0;
	INT32	lCurrentSP	= 0;

	m_pagpmFactors->GetValue(pcsBaseFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsBaseFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsBaseFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	INT32	lNewHP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewMP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;
	INT32	lNewSP		= AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE;

	if (lMaxHP > 0 &&
		lMaxHP < lCurrentHP)
		lNewHP	= lMaxHP;

	if (lMaxMP > 0 &&
		lMaxMP < lCurrentMP)
		lNewMP	= lMaxMP;

	if (lMaxSP > 0 &&
		lMaxSP < lCurrentSP)
		lNewSP	= lMaxSP;

	if (lNewHP != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE ||
		lNewMP != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE ||
		lNewSP != AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE)
	{
		AgpdFactor	*pcsResultFactor	= (AgpdFactor *) m_pagpmFactors->SetFactor(pcsTargetFactor, NULL, AGPD_FACTORS_TYPE_RESULT);
		if (pcsResultFactor)
		{
			AgpdFactorCharPoint	*pcsFactorCharPoint	= (AgpdFactorCharPoint *) m_pagpmFactors->InitUpdateFactor(pcsResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
			if (pcsFactorCharPoint)
			{
				m_pagpmFactors->SetValue(pcsTargetFactor, lNewHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
				m_pagpmFactors->SetValue(pcsTargetFactor, lNewMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
				m_pagpmFactors->SetValue(pcsTargetFactor, lNewSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
			}
		}
	}

	return TRUE;
}

// 2005.06.29. steeple
// ���� Update Factor ���� ������ �� Current ���� ��ȭ�� ���ٸ�,
// Current ���� Base Factor ���� �����ͼ� Update Factor �� �������ش�.
BOOL AgsmFactors::AdjustCharMaxPointFromBaseFactor(AgpdFactor* pcsUpdateFactor, AgpdFactor* pcsBaseFactor)
{
	if (!m_pagpmFactors->GetFactor((AgpdFactor *) m_pagpmFactors->GetFactor(pcsUpdateFactor, AGPD_FACTORS_TYPE_RESULT), AGPD_FACTORS_TYPE_CHAR_POINT))
		return TRUE;

	// �ִ�ġ���� ���� ����Ʈ���� �� ũ�ٸ� ������ ���ش�.
	
	INT32	lMaxHP		= 0;
	INT32	lMaxMP		= 0;
	INT32	lMaxSP		= 0;

	INT32	lCurrentHP	= 0;
	INT32	lCurrentMP	= 0;
	INT32	lCurrentSP	= 0;

	m_pagpmFactors->GetValue(pcsUpdateFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsUpdateFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsUpdateFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsUpdateFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	// ������ 0 �϶��� Base Factor ���� �����´�.
	if(lCurrentHP == 0 || lCurrentHP == AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE)
		m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if(lCurrentMP == 0 || lCurrentMP == AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE)
		m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	if(lCurrentSP == 0 || lCurrentSP == AGPM_FACTORS_UPDATE_FACTOR_INIT_VALUE)
		m_pagpmFactors->GetValue(pcsBaseFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (lMaxHP > 0 &&
		lMaxHP < lCurrentHP)
		m_pagpmFactors->SetValue(pcsUpdateFactor, lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	if (lMaxMP > 0 &&
		lMaxMP < lCurrentMP)
		m_pagpmFactors->SetValue(pcsUpdateFactor, lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);

	if (lMaxSP > 0 &&
		lMaxSP < lCurrentSP)
		m_pagpmFactors->SetValue(pcsUpdateFactor, lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	return TRUE;
}

PVOID AgsmFactors::AddCharPoint(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor, FLOAT fApplyPercent)
{
	if(!m_pagpmTitle)
		return FALSE;

	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdFactor *pcsDestFactor = &pcsCharacter->m_csFactor;
	AgpdFactor *pcsAddFactor = &((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor;

	if(!pcsDestFactor)
		return FALSE;

	INT32	lDestHP			= 0;
	INT32	lDestMP			= 0;
	INT32	lDestSP			= 0;

	INT32	lDestMaxHP		= 0;
	INT32	lDestMaxMP		= 0;
	INT32	lDestMaxSP		= 0;

	INT32	lAddHP			= 0;
	INT32	lAddMP			= 0;
	INT32	lAddSP			= 0;

	INT32 nEffectSet = 0;
	INT32 nEffectValue1 = 0;
	INT32 nEffectValue2 = 0;

	m_pagpmTitle->GetEffectValue(pcsCharacter, AGPMTITLE_TITLE_EFFECT_POTION_BONUS, &nEffectSet, &nEffectValue1, &nEffectValue2);


	m_pagpmFactors->GetValue(pcsDestFactor, &lDestHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (bIsFromResultFactor)
	{
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}
	else
	{
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}

	if (lAddHP > 0 && lDestMaxHP > lDestHP)
	{
		if(nEffectSet == 0 && pcsItem->m_lTID == nEffectValue1)
			lAddHP += nEffectValue2;

		lDestHP	+= (INT32) (lAddHP * fApplyPercent);

		if (lDestHP > lDestMaxHP)
			lDestHP = lDestMaxHP;
	}
	else
		lDestHP = 0;

	if (lAddMP > 0 && lDestMaxMP > lDestMP)
	{
		if(nEffectSet == 0 && pcsItem->m_lTID == nEffectValue1)
			lAddMP += nEffectValue2;

		lDestMP	+= (INT32) (lAddMP * fApplyPercent);

		if (lDestMP > lDestMaxMP)
			lDestMP = lDestMaxMP;
	}
	else
		lDestMP = 0;

	if (lAddSP > 0 && lDestMaxSP > lDestSP)
	{
		if(nEffectSet == 0 && pcsItem->m_lTID == nEffectValue1)
			lAddSP += nEffectValue2;

		lDestSP	+= (INT32) (lAddSP * fApplyPercent);

		if (lDestSP > lDestMaxSP)
			lDestSP = lDestMaxSP;
	}
	else
		lDestSP = 0;

	PVOID	pvPacket	= NULL;

	if (lDestHP == 0 && lDestMP == 0 && lDestSP == 0)
		return NULL;

	UpdateCharPoint(pcsDestFactor, &pvPacket, lDestHP, lDestMP, lDestSP);

	return pvPacket;
}

PVOID AgsmFactors::AddCharPointByPercent(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem, BOOL bIsFromResultFactor, FLOAT fApplyPercent)
{
	if(!m_pagpmTitle)
		return FALSE;

	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdFactor *pcsDestFactor = &pcsCharacter->m_csFactor;
	AgpdFactor *pcsAddFactor = &((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_csEffectFactor;

	if(!pcsDestFactor)
		return FALSE;

	INT32	lDestHP			= 0;
	INT32	lDestMP			= 0;
	INT32	lDestSP			= 0;

	INT32	lDestMaxHP		= 0;
	INT32	lDestMaxMP		= 0;
	INT32	lDestMaxSP		= 0;

	INT32	lAddHP			= 0;
	INT32	lAddMP			= 0;
	INT32	lAddSP			= 0;

	INT32 nEffectSet = 0;
	INT32 nEffectValue1 = 0;
	INT32 nEffectValue2 = 0;

	m_pagpmTitle->GetEffectValue(pcsCharacter, AGPMTITLE_TITLE_EFFECT_POTION_BONUS, &nEffectSet, &nEffectValue1, &nEffectValue2);


	m_pagpmFactors->GetValue(pcsDestFactor, &lDestHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsDestFactor, &lDestMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (bIsFromResultFactor)
	{
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}
	else
	{
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->GetValue(pcsAddFactor, &lAddSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}

	if (lAddHP > 0 && lDestMaxHP > lDestHP)
	{
		if(nEffectSet == 1 && pcsItem->m_lTID == nEffectValue1)
			lAddHP += nEffectValue2;

		lDestHP	+= (INT32) (lDestMaxHP * (lAddHP / 100.0f) * fApplyPercent);

		if (lDestHP > lDestMaxHP)
			lDestHP = lDestMaxHP;
	}
	else
		lDestHP = 0;

	if (lAddMP > 0 && lDestMaxMP > lDestMP)
	{
		if(nEffectSet == 1 && pcsItem->m_lTID == nEffectValue1)
			lAddMP += nEffectValue2;

		lDestMP	+= (INT32) (lDestMaxMP * (lAddMP / 100.0f) * fApplyPercent);

		if (lDestMP > lDestMaxMP)
			lDestMP = lDestMaxMP;
	}
	else
		lDestMP = 0;

	if (lAddSP > 0 && lDestMaxSP > lDestSP)
	{
		if(nEffectSet == 1 && pcsItem->m_lTID == nEffectValue1)
			lAddSP += nEffectValue2;

		lDestSP	+= (INT32) (lDestMaxSP * (lAddSP / 100.0f) * fApplyPercent);

		if (lDestSP > lDestMaxSP)
			lDestSP = lDestMaxSP;
	}
	else
		lDestSP = 0;

	PVOID	pvPacket	= NULL;

	if (lDestHP == 0 && lDestMP == 0 && lDestSP == 0)
		return NULL;

	UpdateCharPoint(pcsDestFactor, &pvPacket, lDestHP, lDestMP, lDestSP);

	return pvPacket;
}

BOOL AgsmFactors::UpdateCharPoint(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lHP, INT32 lMP, INT32 lSP)
{
	if (!pcsFactor)
		return FALSE;

	if (lHP == 0 && lMP == 0 && lSP == 0)
		return TRUE;

	AgpdFactor			csUpdateFactor;
	ZeroMemory(&csUpdateFactor, sizeof(AgpdFactor));

	m_pagpmFactors->InitFactor((AgpdFactor *) m_pagpmFactors->SetFactor(&csUpdateFactor, NULL, AGPD_FACTORS_TYPE_RESULT));

	AgpdFactor			*pcsUpdateResultFactor = (AgpdFactor *) m_pagpmFactors->GetFactor(&csUpdateFactor, AGPD_FACTORS_TYPE_RESULT);
	if (!pcsUpdateResultFactor)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return FALSE;
	}

	AgpdFactorCharPoint *pcsFactorCharPoint = (AgpdFactorCharPoint *) m_pagpmFactors->InitUpdateFactor(pcsUpdateResultFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	if (!pcsFactorCharPoint)
	{
		m_pagpmFactors->DestroyFactor(&csUpdateFactor);

		return FALSE;
	}

	if (lHP > 0)
		m_pagpmFactors->SetValue(&csUpdateFactor, lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	if (lMP > 0)
		m_pagpmFactors->SetValue(&csUpdateFactor, lMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	if (lSP > 0)
		m_pagpmFactors->SetValue(&csUpdateFactor, lSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	if (pvPacket)
	{
		*pvPacket = m_pagpmFactors->UpdateFactor(pcsFactor, &csUpdateFactor, TRUE);
		if (!*pvPacket)
		{
			m_pagpmFactors->DestroyFactor(&csUpdateFactor);

			return FALSE;
		}
	}
	else
		m_pagpmFactors->UpdateFactor(pcsFactor, &csUpdateFactor, FALSE);

	m_pagpmFactors->DestroyFactor(&csUpdateFactor);

	return TRUE;
}

BOOL AgsmFactors::RecoveryPointInterval(AgpdFactor *pcsFactor, PVOID *pvPacket, INT32 lModifiedPointHP, INT32 lModifiedPointMP, BOOL bIsRecoveryHP, BOOL bIsRecoveryMP)
{
	if (!pcsFactor || !pvPacket)
		return FALSE;

	INT32	lMaxHP		= 0;
	INT32	lMaxMP		= 0;

	INT32	lHP			= 0;
	INT32	lMP			= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

	m_pagpmFactors->GetValue(pcsFactor, &lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);

	if (lMaxHP	<= lHP &&
		lMaxMP	<= lMP)
	{
		*pvPacket = NULL;

		return TRUE;
	}

	// HP_Regeneration = 1 + (CON / 50) -> 10�ʴ� 1���� ä���ش�
	// MP_Regeneration = 1 + (WIS / 50) -> 10�ʴ� 1���� ä���ش�
	// SP_Regeneration = 1 + (DEX / 50) -> 10�ʴ� 1���� ä���ش�

	INT32	lNewHP		= 0;
	INT32	lNewMP		= 0;

	FLOAT	fCon	= 0.0f;
	FLOAT	fInt	= 0.0f;

	m_pagpmFactors->GetValue(pcsFactor, &fCon, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
	m_pagpmFactors->GetValue(pcsFactor, &fInt, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

	if (bIsRecoveryHP && lMaxHP > lHP)
	{
		lNewHP = lHP + (INT32)(1 + (fCon / 10.0f)) + lModifiedPointHP;

		if (lNewHP > lMaxHP)
			lNewHP = lMaxHP;
	}

	if (bIsRecoveryMP && lMaxMP > lMP)
	{
		lNewMP = lMP + (INT32)(1 + (fInt / 10.0f)) + lModifiedPointMP;

		if (lNewMP > lMaxMP)
			lNewMP = lMaxMP;
	}

	return UpdateCharPoint(pcsFactor, pvPacket, lNewHP, lNewMP, 0);
}

//		SetCharPoint
//	Functions
//		- pcsFactor���� CharMaxPoint(HP, MP, SP, etc)���� CharPoint�� �����Ѵ�.
//			(ĳ���Ͱ� ó���� �ʱ�ȭ�ɶ� �ִ� ����Ʈ ������ ���� ����Ʈ�� ���õǼ� ���ӿ� �����ϰ� �ȴ�.)
//	Arguments
//		- pcsFactor : factor pointer
//	Return value
//		- PVOID : ����� ���� ����Ÿ (��Ŷ)
///////////////////////////////////////////////////////////////////////////////
PVOID AgsmFactors::SetCharPoint(AgpdFactor *pcsFactor, BOOL bResultFactor, BOOL bMakePacket)
{
	if (!pcsFactor)
		return NULL;

	INT32	lMaxHP	= 0;
	INT32	lMaxMP	= 0;
	INT32	lMaxSP	= 0;

	if (bResultFactor)
	{
		m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

		AgpdFactor	*pcsResultFactor	= (AgpdFactor *) m_pagpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT);
		m_pagpmFactors->SetFactor(pcsResultFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);

		m_pagpmFactors->SetValue(pcsFactor, lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->SetValue(pcsFactor, lMaxMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->SetValue(pcsFactor, lMaxSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}
	else
	{
		m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

		m_pagpmFactors->SetFactor(pcsFactor, NULL, AGPD_FACTORS_TYPE_CHAR_POINT);

		m_pagpmFactors->SetValue(pcsFactor, lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pagpmFactors->SetValue(pcsFactor, lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pagpmFactors->SetValue(pcsFactor, lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	}

	if (bMakePacket)
	{
		if (bResultFactor)
			return m_pagpmFactors->MakePacketFactors(pcsFactor, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT);
		else
			return m_pagpmFactors->MakePacketFactors(pcsFactor, AGPD_FACTORS_TYPE_CHAR_POINT);
	}

	return NULL;
}

BOOL AgsmFactors::SaveResultPoint(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lCurrentHP		= 0;
	INT32	lCurrentMP		= 0;
	INT32	lCurrentSP		= 0;
	INT64	llCurrentEXP	= 0;

	INT32	lMaxHP			= 0;
	INT32	lMaxMP			= 0;
	INT32	lMaxSP			= 0;
	INT64	llMaxEXP		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lCurrentHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lCurrentMP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lCurrentSP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	llCurrentEXP	= m_pagpmFactors->GetExp((AgpdFactor *) m_pagpmFactors->GetFactor(pcsFactor, AGPD_FACTORS_TYPE_RESULT));

	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxSP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_SP);

	llMaxEXP		= m_pagpmFactors->GetMaxExp(pcsFactor);

	if (lCurrentHP > lMaxHP)
		lCurrentHP = lMaxHP;
	if (lCurrentMP > lMaxMP)
		lCurrentMP = lMaxMP;
	if (lCurrentSP > lMaxSP)
		lCurrentSP = lMaxSP;
	if (llCurrentEXP > llMaxEXP)
		llCurrentEXP = llMaxEXP;

	m_pagpmFactors->SetValue(pcsFactor, lCurrentHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->SetValue(pcsFactor, lCurrentMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->SetValue(pcsFactor, lCurrentSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);

	m_pagpmFactors->SetExp(pcsFactor, llCurrentEXP);

	return TRUE;
}

//		GetAttackSpeed
//	Functions
//		- factor result�� ����ִ� attack speed�� �������ش�.
//	Arguments
//		- pcsFactor : factor pointer
//	Return value
//		- INT16 : �����Ұ�� 0���� ���� ����, ������ ��� Attack Speed�� �����Ѵ�.
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmFactors::GetAttackSpeed(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return FALSE;

	INT32	lAttackSpeed	= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lAttackSpeed, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

	return lAttackSpeed;
}

//		MakePacketSyncParty
//	Functions
//		- Party ���鰣 ����Ÿ ������ �ʿ��� �׸�鿡 ���� ���� ��Ŷ�� ����� �����Ѵ�.
//	Arguments
//		- pcsFactor : factor pointer
//	Return value
//		- PVOID : �����ϸ� NULL��, �����ϸ� ������� factor packet�� ����
///////////////////////////////////////////////////////////////////////////////
PVOID AgsmFactors::MakePacketSyncParty(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	INT16	nPacketLength = 0;

	PVOID	pvPacketFactorCharStatus	= NULL;
	PVOID	pvPacketFactorCharPoint		= NULL;
	PVOID	pvPacketFactorCharMaxPoint	= NULL;

	INT32	lLevel		= 0;
	INT32	lHP			= 0;
	INT32	lMaxHP		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lLevel, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	m_pagpmFactors->GetValue(pcsFactor, &lHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMaxHP, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINT_TYPE_HP);

	pvPacketFactorCharStatus	= m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, &nPacketLength, 0,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					&lLevel,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL);

	pvPacketFactorCharPoint		= m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].MakePacket(FALSE, &nPacketLength, 0,
																					&lHP,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL);

	pvPacketFactorCharMaxPoint	= m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].MakePacket(FALSE, &nPacketLength, 0,
																					&lMaxHP,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL);
	
	if (!pvPacketFactorCharStatus && !pvPacketFactorCharPoint && !pvPacketFactorCharMaxPoint)
		return NULL;

	PVOID	pvPacketFactor = m_pagpmFactors->m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
																					NULL,
																					pvPacketFactorCharStatus,
																					NULL,
																					pvPacketFactorCharPoint,
																					pvPacketFactorCharMaxPoint,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL,
																					NULL);

	if (pvPacketFactorCharStatus)
		m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].FreePacket(pvPacketFactorCharStatus);
	if (pvPacketFactorCharPoint)
		m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].FreePacket(pvPacketFactorCharPoint);
	if (pvPacketFactorCharMaxPoint)
		m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT_MAX].FreePacket(pvPacketFactorCharMaxPoint);

	if (!pvPacketFactor)
		return NULL;

	PVOID	pvPacket = m_pagpmFactors->m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
																pvPacketFactor,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactor);

	return pvPacket;
}

PVOID AgsmFactors::MakePacketUpdateLevel(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	INT16	nPacketLength	= 0;

	INT32	lLevel		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);

	PVOID pvPacketFactorCharStatus = m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, &nPacketLength, 0,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						&lLevel,
																						NULL,
																						NULL,
																						NULL,
																						NULL);
	if (!pvPacketFactorCharStatus)
		return NULL;

	PVOID	pvPacket = m_pagpmFactors->m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
																NULL,
																pvPacketFactorCharStatus,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactorCharStatus);

	return pvPacket;
}

PVOID AgsmFactors::MakePacketUpdateUnionRank(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	INT16	nPacketLength	= 0;

	INT32	lUnionRank		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lUnionRank, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_UNION_RANK);

	PVOID pvPacketFactorCharStatus = m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, &nPacketLength, 0,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						&lUnionRank,
																						NULL,
																						NULL);
	if (!pvPacketFactorCharStatus)
		return NULL;

	PVOID	pvPacket = m_pagpmFactors->m_csPacket.MakePacket(FALSE, &nPacketLength, 0,
																NULL,
																pvPacketFactorCharStatus,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL,
																NULL);

	m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactorCharStatus);

	return pvPacket;
}

PVOID AgsmFactors::MakePacketDBData(AgpdFactor *pcsFactor)
{
	if (!pcsFactor)
		return NULL;

	INT16	nPacketLength	= 0;

	INT32	lLevel			= 0;
	INT32	lMurdererPoint	= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
	m_pagpmFactors->GetValue(pcsFactor, &lMurdererPoint, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MURDERER);

	PVOID pvPacketFactorCharStatus	= m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_STATUS].MakePacket(FALSE, &nPacketLength, 0,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						&lLevel,
																						NULL,
																						NULL,
																						NULL,
																						&lMurdererPoint,
																						NULL);

	INT32	lHP				= 0;
	INT32	lMP				= 0;
	INT32	lSP				= 0;
	INT32	lEXPLow			= 0;
	INT32	lEXPHigh		= 0;

	m_pagpmFactors->GetValue(pcsFactor, &lHP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_pagpmFactors->GetValue(pcsFactor, &lMP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_pagpmFactors->GetValue(pcsFactor, &lSP, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_SP);
	m_pagpmFactors->GetValue(pcsFactor, &lEXPLow, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_LOW);
	m_pagpmFactors->GetValue(pcsFactor, &lEXPHigh, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_EXP_HIGH);

	PVOID pvPacketFactorCharPoint	= m_pagpmFactors->m_acsPacket[AGPD_FACTORS_TYPE_CHAR_POINT].MakePacket(FALSE, &nPacketLength, 0,
																						&lHP,
																						&lMP,
																						&lSP,
																						&lEXPLow,
																						&lEXPHigh,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL);

	PVOID pvPacketFactors			= m_pagpmFactors->m_csPacket.MakePacket(TRUE, &nPacketLength, 0,
																						NULL,
																						pvPacketFactorCharStatus,
																						NULL,
																						pvPacketFactorCharPoint,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL,
																						NULL);

	if (pvPacketFactorCharStatus)
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactorCharStatus);
	if (pvPacketFactorCharPoint)
		m_pagpmFactors->m_csPacket.FreePacket(pvPacketFactorCharPoint);

	return pvPacketFactors;
}

BOOL AgsmFactors::AdjustWeaponFactor(AgpdCharacter* pcsCharacter, AgpdFactor* pcsUpdateResultFactor)
{
	if(!pcsCharacter || !pcsUpdateResultFactor)
		return FALSE;

	AgpdItem* pcsItemL = m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
	AgpdItem* pcsItemR = m_pagpmItem->GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_RIGHT);

	//////////////////////////////////////////////////////////////////////////
	// ����/ī���� ���ÿ� �����ϰ� ���� ���, ū���� �����.
	if( pcsItemL && m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemL) &&
		((AgpdItemTemplateEquipWeapon*)pcsItemL->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_CHARON &&
		pcsItemR && m_pagpmItem->CheckUseItem(pcsCharacter, pcsItemR) &&
		((AgpdItemTemplateEquipWeapon*)pcsItemR->m_pcsItemTemplate)->m_nWeaponType == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_ZENON )
	{
		INT32 lItemAttackRangeL	= 0;
		INT32 lItemAttackSpeedL	= 0;
		INT32 lItemAttackRangeR = 0;
		INT32 lItemAttackSpeedR = 0;

		m_pagpmFactors->GetValue(&pcsItemL->m_csFactor, &lItemAttackRangeL, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
		m_pagpmFactors->GetValue(&pcsItemL->m_csFactor, &lItemAttackSpeedL, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
		m_pagpmFactors->GetValue(&pcsItemR->m_csFactor, &lItemAttackRangeR, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
		m_pagpmFactors->GetValue(&pcsItemR->m_csFactor, &lItemAttackSpeedR, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);

		INT32 lRange = max(lItemAttackRangeL, lItemAttackRangeR);
		INT32 lSpeed = max(lItemAttackSpeedL, lItemAttackSpeedR);

		m_pagpmFactors->SetValue(pcsUpdateResultFactor, lRange, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_ATTACKRANGE);
		m_pagpmFactors->SetValue(pcsUpdateResultFactor, lSpeed, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	}

	return TRUE;
}