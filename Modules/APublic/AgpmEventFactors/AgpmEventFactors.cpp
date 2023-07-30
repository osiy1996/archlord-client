/******************************************************************************
Module:  AgpmEventFactors.cpp
Notices: Copyright (c) NHN Studio 2003 netong
Purpose: 
Last Update: 2003. 03. 12
******************************************************************************/

#include <AgpmEventFactors/AgpmEventFactors.h>
#include <AgpmFactors/AgpmFactors.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <ApmEventManager/ApmEventManager.h>
#include <ApmMap/ApmMap.h>

AgpmEventFactors::AgpmEventFactors()
{
	SetModuleName("AgpmEventFactors");
}

AgpmEventFactors::~AgpmEventFactors()
{
}

BOOL AgpmEventFactors::OnAddModule()
{
	m_papmEventManager	= (ApmEventManager *)	GetModule("ApmEventManager");
	m_pagpmFactors		= (AgpmFactors *)		GetModule("AgpmFactors");
	m_pagpmCharacter	= (AgpmCharacter *)		GetModule("AgpmCharacter");

	if (!m_papmEventManager || !m_pagpmFactors || !m_pagpmCharacter)
		return FALSE;

	return TRUE;
}

BOOL AgpmEventFactors::OnInit()
{
	return TRUE;
}

BOOL AgpmEventFactors::OnDestroy()
{
	return TRUE;
}

//		ProcessUpdateEventFactors
//	Functions
//		- nConditionType�� ���� character�� ������ pcsUpdateFactor�� ������Ʈ �Ѵ�.
//		- ������Ʈ ����� pfnProcessResult�� �Ѱ��ش�.
//		- �Ƶ� ������ ����. ��.�� ����...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition�� ���ԵǾ� �ִ� event
//		- pcsUpdateFactor	: update�� ������ ����ִ� point factor
//		- pcsUpdateFactorPercent	: update�� ������ ����ִ� percent factor
//		- pfnCheckCondition	: factor�� �ݿ��Ҷ� ������ �˻��� �ʿ䰡 ������ ó���� �Լ� ������
//		- pfnCalcFactor		: pcsUpdateFactor ���� �˻��� Base ���� ���� UpdateFactor�� ����� �ʿ䰡 ������ ó���� �Լ�
//								(��, ��� base�� factor�� pcsUpdateFactor�� �ٲٷ��� �� �Լ� �����͸� NULL��
//								 �׷��� ������쿣 ���⿡ ó�� �Լ��� �Ѱ��ش�. �̶� pcsUpdateFactor�� �ٽ� �ʱ�ȭ�ȴ�)
//		- pfnProcessResult	: update�� �� update�� factor packet�� �޾Ƽ� ó���� �Լ� ������
//		- pvClass			: �� �Լ��� ȣ���� class (module) pointer
//		- pvCustData		: pfnProcessResult �Լ��� ���ڷ� �Ѱ���� ���� ����Ÿ�� ���� ���⿡ �����͸� �Ѱ��ش�.
//		- bIsUpdateFactor	: UpdateFactor�� ���� CalcFactor�� ������ ����
//		- bMakePacket		: MakePacket()�� ���� ����
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::ProcessUpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bIsUpdateFactor,
										 BOOL bMakePacket)
{
	if (!pcsEvent || !pcsUpdateFactor)
		return FALSE;

	switch (nConditionType) {
	case AGPM_EVENT_FACTORS_TYPE_AREA:
		{
			INT_PTR	alCID[AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS];
			INT32	alCID2[AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS];

			ZeroMemory(alCID, sizeof(INT32) * AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS);
			ZeroMemory(alCID2, sizeof(INT32) * AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS);

			// 2005/02/21 ������
			// �ϴ� ���ε����� 0���� ����..

			INT16	nNumCharacter = 0;
			if ((nNumCharacter = m_papmEventManager->GetAreaCharacters( 0 , APMMAP_CHAR_TYPE_PC | APMMAP_CHAR_TYPE_NPC | APMMAP_CHAR_TYPE_MONSTER, pcsEvent, alCID, AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS, alCID2, AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS)) < 1)
				return FALSE;
			
			if (nNumCharacter > AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS)
				nNumCharacter	= AGPM_EVENT_FACTORS_MAX_AREA_CHARACTERS;

			for (int i = 0; i < nNumCharacter; ++i)
			{
				if (alCID[i] == AP_INVALID_CID)
					continue;

				AgpdCharacter	*pcsCharacter = m_pagpmCharacter->GetCharacter((INT32)alCID[i]);
				if (!pcsCharacter)
					continue;

				INT16	nUpdateType = 0;

				// ���� �˻��Ѵ�.
				if (pfnCheckCondition)
				{
					// ���� �˻����� FALSE�� �Ѿ��.
					nUpdateType = pfnCheckCondition(pcsCharacter, pvClass, pvCustData);
					if (nUpdateType == 0)
						continue;
				}

				stAgpmEventFactorsCBArg		stCBArg;

				stCBArg.pcsEvent		= pcsEvent;
				stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
				stCBArg.pcsUpdateFactor	= pcsUpdateFactor;
				stCBArg.pcsUpdateFactorPercent	= pcsUpdateFactorPercent;
				stCBArg.pvCustData		= pvCustData;

				// pfnCalcFactor�� ���� �ƴϸ� ȣ���ؼ� pcsUpdateFactor���� ����ϰ� �Ѵ�)
				if (pfnCalcFactor)
				{
					if (!pfnCalcFactor(pcsCharacter, pvClass, &stCBArg))
						continue;
				}

				PVOID	pvPacketFactor = NULL;

				switch (nUpdateType) {
				case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, TRUE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
					{
						if (bIsUpdateFactor)
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
						else
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
					}
					break;

				case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
					{
						if (bIsUpdateFactor)
						{
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
							pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
						}
						else
						{
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
							pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
						}
					}
					break;
				}

				if (pfnProcessResult && pvClass)
				{
					stCBArg.pvPacketFactor	= pvPacketFactor;

					pfnProcessResult(&stCBArg, pvClass, NULL);
				}

				if (pvPacketFactor)
					delete [] (BYTE*)pvPacketFactor;
//					GlobalFree(pvPacketFactor);
			}

			return TRUE;
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_FACTOR:
		{
			stAgpmEventFactorsArg	stArg;

			stArg.pfnCheckCondition	= pfnCheckCondition;
			stArg.pfnCalcFactor		= pfnCalcFactor;
			stArg.pfnProcessor		= pfnProcessResult;
			stArg.pvClass			= pvClass;
			stArg.pcsEvent			= pcsEvent;
			stArg.pvUpdateFactor	= pcsUpdateFactor;
			stArg.pvUpdateFactorPercent	= pcsUpdateFactorPercent;
			stArg.pvCustData		= pvCustData;
			stArg.bMakePacket		= bMakePacket;

			if (bIsUpdateFactor)
				return m_papmEventManager->GetTargetFactorCharacters(pcsEvent, UpdateFactor, this, &stArg);
			else
				return m_papmEventManager->GetTargetFactorCharacters(pcsEvent, CalcFactor, this, &stArg);
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_CHARACTER:
		{
			if (!pcsEvent->m_pstCondition ||
				!pcsEvent->m_pstCondition->m_pstTarget || 
				pcsEvent->m_pstCondition->m_pstTarget->m_lCID == AP_INVALID_CID)
				return FALSE;

			AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(pcsEvent->m_pstCondition->m_pstTarget->m_lCID);
			if (!pcsCharacter)
				return FALSE;

			INT16	nUpdateType = 0;

			// ���� �˻��Ѵ�.
			if (pfnCheckCondition)
			{
				// ���� �˻����� FALSE�� �Ѿ��.
				nUpdateType = pfnCheckCondition(pcsCharacter, pvClass, pvCustData);
				if (nUpdateType == 0)
					return FALSE;
			}

			stAgpmEventFactorsCBArg		stCBArg;

			stCBArg.pcsEvent		= pcsEvent;
			stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
			stCBArg.pcsUpdateFactor	= pcsUpdateFactor;
			stCBArg.pcsUpdateFactorPercent	= pcsUpdateFactorPercent;
			stCBArg.pvCustData		= pvCustData;

			// pfnCalcFactor�� ���� �ƴϸ� ȣ���ؼ� pcsUpdateFactor���� ����ϰ� �Ѵ�)
			if (pfnCalcFactor)
			{
				if (!pfnCalcFactor(pcsCharacter, pvClass, &stCBArg))
					return FALSE;
			}

			PVOID	pvPacketFactor = NULL;

			switch (nUpdateType) {
			case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, pcsUpdateFactor, TRUE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
				{
					if (bIsUpdateFactor)
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
					else
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
				}
				break;

			case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
				{
					if (bIsUpdateFactor)
					{
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, bMakePacket);
						pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, bMakePacket);
					}
					else
					{
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, pcsUpdateFactor, TRUE, bMakePacket);
						pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, pcsUpdateFactorPercent, FALSE, bMakePacket);
					}
				}
				break;
			}

			if (pfnProcessResult && pvClass)
			{
				stCBArg.pvPacketFactor	= pvPacketFactor;

				pfnProcessResult(&stCBArg, pvClass, NULL);
			}

			if (pvPacketFactor)
				delete [] (BYTE*)pvPacketFactor;
//				GlobalFree(pvPacketFactor);

			return TRUE;
		}
		break;

	case AGPM_EVENT_FACTORS_TYPE_ITEM:
		{
			stAgpmEventFactorsArg stArg;

			stArg.pfnCheckCondition	= pfnCheckCondition;
			stArg.pfnCalcFactor		= pfnCalcFactor;
			stArg.pfnProcessor		= pfnProcessResult;
			stArg.pvClass			= pvClass;
			stArg.pcsEvent			= pcsEvent;
			stArg.pvUpdateFactor	= pcsUpdateFactor;
			stArg.pvUpdateFactorPercent	= pcsUpdateFactorPercent;
			stArg.pvCustData		= pvCustData;
			stArg.bMakePacket		= bMakePacket;

			if (bIsUpdateFactor)
				return m_papmEventManager->GetTargetItemCharacters(pcsEvent, UpdateFactor, this, &stArg);
			else
				return m_papmEventManager->GetTargetItemCharacters(pcsEvent, CalcFactor, this, &stArg);
		}
		break;
	};

	return FALSE;
}

//		CalcEventFactors
//	Functions
//		- nConditionType�� ���� character�� ������ pcsUpdateFactor�� CalcFactor() �Ѵ�.
//		- ������Ʈ ����� pfnProcessResult�� �Ѱ��ش�.
//		- �Ƶ� ������ ����. ��.�� ����...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition�� ���ԵǾ� �ִ� event
//		- pcsUpdateFactor	: calculate�� ������ ����ִ� point factor
//		- pcsUpdateFactorPercent	: calculate�� ������ ����ִ� percent factor
//		- pfnCheckCondition	: factor�� �ݿ��Ҷ� ������ �˻��� �ʿ䰡 ������ ó���� �Լ� ������
//		- pfnCalcFactor		: pcsUpdateFactor ���� �˻��� Base ���� ���� UpdateFactor�� ����� �ʿ䰡 ������ ó���� �Լ�
//								(��, ��� base�� factor�� pcsUpdateFactor�� �ٲٷ��� �� �Լ� �����͸� NULL��
//								 �׷��� ������쿣 ���⿡ ó�� �Լ��� �Ѱ��ش�. �̶� pcsUpdateFactor�� �ٽ� �ʱ�ȭ�ȴ�)
//		- pfnProcessResult	: update�� �� update�� factor packet�� �޾Ƽ� ó���� �Լ� ������
//		- pvClass			: �� �Լ��� ȣ���� class (module) pointer
//		- pvCustData		: pfnProcessResult �Լ��� ���ڷ� �Ѱ���� ���� ����Ÿ�� ���� ���⿡ �����͸� �Ѱ��ش�.
//		- bMakePacket		: ���״��
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket)
{
	return ProcessUpdateEventFactors(nConditionType,
									 pcsEvent,
									 pcsUpdateFactor,
									 pcsUpdateFactorPercent,
									 pfnCheckCondition,
									 pfnCalcFactor,
									 pfnProcessResult,
									 pvClass,
									 pvCustData,
									 FALSE,
									 bMakePacket);
}

//		UpdateEventFactors
//	Functions
//		- nConditionType�� ���� character�� ������ pcsUpdateFactor�� ������Ʈ �Ѵ�.
//		- ������Ʈ ����� pfnProcessResult�� �Ѱ��ش�.
//		- �Ƶ� ������ ����. ��.�� ����...
//	Arguments
//		- nConditionType	: AGPM_EVENT_FACTORS_TYPE_AREA
//							  AGPM_EVENT_FACTORS_TYPE_FACTOR
//							  AGPM_EVENT_FACTORS_TYPE_CHARACTER
//							  AGPM_EVENT_FACTORS_TYPE_ITEM
//		- pcsEvent			: condition�� ���ԵǾ� �ִ� event
//		- pcsUpdateFactor	: update�� ������ ����ִ� factor
//		- pcsUpdateFactor	: update�� ������ ����ִ� percent factor
//		- pfnCheckCondition	: factor�� �ݿ��Ҷ� ������ �˻��� �ʿ䰡 ������ ó���� �Լ� ������
//		- pfnCalcFactor		: pcsUpdateFactor ���� �˻��� Base ���� ���� UpdateFactor�� ����� �ʿ䰡 ������ ó���� �Լ�
//								(��, ��� base�� factor�� pcsUpdateFactor�� �ٲٷ��� �� �Լ� �����͸� NULL��
//								 �׷��� ������쿣 ���⿡ ó�� �Լ��� �Ѱ��ش�. �̶� pcsUpdateFactor�� �ٽ� �ʱ�ȭ�ȴ�)
//		- pfnProcessResult	: update�� �� update�� factor packet�� �޾Ƽ� ó���� �Լ� ������
//		- pvClass			: �� �Լ��� ȣ���� class (module) pointer
//		- pvCustData		: pfnProcessResult �Լ��� ���ڷ� �Ѱ���� ���� ����Ÿ�� ���� ���⿡ �����͸� �Ѱ��ش�.
//		- bMakePacket		: ���״��
//	Return value
//		- BOOL
///////////////////////////////////////////////////////////////////////////////
//BOOL AgpmEventFactors::CalcEventFactors(INT16 nConditionType, ApdEvent *pcsEvent, AgpdFactor *pcsUpdateFactor, 
//										   ApModuleDefaultCallBack pfnProcessResult, PVOID pvClass, PVOID pvCustData)
BOOL AgpmEventFactors::UpdateEventFactors(INT16 nConditionType,
										 ApdEvent *pcsEvent,
										 AgpdFactor *pcsUpdateFactor,
										 AgpdFactor *pcsUpdateFactorPercent,
										 AgpmEventFactorsCallBack pfnCheckCondition,
										 ApModuleDefaultCallBack pfnCalcFactor,
										 ApModuleDefaultCallBack pfnProcessResult,
										 PVOID pvClass,
										 PVOID pvCustData,
										 BOOL bMakePacket)
{
	return ProcessUpdateEventFactors(nConditionType,
									 pcsEvent,
									 pcsUpdateFactor,
									 pcsUpdateFactorPercent,
									 pfnCheckCondition,
									 pfnCalcFactor,
									 pfnProcessResult,
									 pvClass,
									 pvCustData,
									 TRUE,
									 bMakePacket);
}

BOOL AgpmEventFactors::ProcessUpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData, BOOL bIsUpdateFactor)
{
	if (!pvData || !pvUpdateData)
		return FALSE;

	AgpdCharacter			*pcsCharacter	= (AgpdCharacter *)			pvData;
	pstAgpmEventFactorsArg	pstArg			= (pstAgpmEventFactorsArg)	pvUpdateData;

	INT16	nUpdateType = 0;

	// ���� �˻��Ѵ�.
	if (pstArg->pfnCheckCondition)
	{
		// ���� �˻����� FALSE�� �Ѿ��.
		nUpdateType = pstArg->pfnCheckCondition(pcsCharacter, pstArg->pvClass, pstArg->pvCustData);
		if (nUpdateType == 0)
			return FALSE;
	}

	// pfnCalcFactor�� ���� �ƴϸ� ȣ���ؼ� pcsUpdateFactor���� ����ϰ� �Ѵ�)
	if (pstArg->pfnCalcFactor)
	{
		if (!pstArg->pfnCalcFactor(pcsCharacter, pvClass, pstArg))
			return FALSE;
	}

	PVOID	pvPacketFactor = NULL;

	switch (nUpdateType) {
	case AGPM_EVENT_FACTORS_UPDATE_TYPE_RESULT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactor, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactor, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_PERCENT:
		{
			if (bIsUpdateFactor)
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), pstArg->bMakePacket);
			else
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), FALSE, pstArg->bMakePacket);
		}
		break;

	case AGPM_EVENT_FACTORS_UPDATE_TYPE_POINT_PERCENT:
		{
			if (bIsUpdateFactor)
			{
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), pstArg->bMakePacket);
				pvPacketFactor = m_pagpmFactors->UpdateFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), pstArg->bMakePacket);
			}
			else
			{
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPoint, ((AgpdFactor *) pstArg->pvUpdateFactor), TRUE, pstArg->bMakePacket);
				pvPacketFactor = m_pagpmFactors->CalcFactor(&pcsCharacter->m_csFactorPercent, ((AgpdFactor *) pstArg->pvUpdateFactorPercent), FALSE, pstArg->bMakePacket);
			}
		}
		break;
	}

	if (pstArg->pfnProcessor && pstArg->pvClass)
	{
		stAgpmEventFactorsCBArg		stCBArg;
		ZeroMemory(&stCBArg, sizeof(stCBArg));

		stCBArg.pcsEvent		= pstArg->pcsEvent;
		stCBArg.pcsTargetBase	= (ApBase *) pcsCharacter;
		stCBArg.pcsUpdateFactor	= (AgpdFactor *) pstArg->pvUpdateFactor;
		stCBArg.pcsUpdateFactorPercent	= (AgpdFactor *) pstArg->pvUpdateFactorPercent;
		stCBArg.pvPacketFactor	= pvPacketFactor;
		stCBArg.pvCustData		= pstArg->pvCustData;

		pstArg->pfnProcessor(&stCBArg, pstArg->pvClass, NULL);
	}

	if (pvPacketFactor)
		delete [] (BYTE*)pvPacketFactor;
//		GlobalFree(pvPacketFactor);

	return TRUE;
}

BOOL AgpmEventFactors::CalcFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData)
{
	AgpmEventFactors	*pThis = (AgpmEventFactors *) pvClass;

	return pThis->ProcessUpdateFactor(pvData, pvClass, pvUpdateData, FALSE);
}

BOOL AgpmEventFactors::UpdateFactor(PVOID pvData, PVOID pvClass, PVOID pvUpdateData)
{
	AgpmEventFactors	*pThis = (AgpmEventFactors *) pvClass;

	return pThis->ProcessUpdateFactor(pvData, pvClass, pvUpdateData, TRUE);
}