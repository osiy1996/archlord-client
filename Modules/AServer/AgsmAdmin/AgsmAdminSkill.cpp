// AgsmAdminSkill.cpp
// (C) NHN - ArchLord Development Team
// steeple, 2005. 03. 06.
//

#include "AgsmAdmin.h"

// �ڿ� �Ѿ�� ���� ����
BOOL AgsmAdmin::ProcessCharSkillEdit(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	if(m_pagpmAdmin->GetAdminLevel(pcsAgpdCharacter) < AGPMADMIN_LEVEL_3)
		return FALSE;

	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_SKILL_LEARN:
			ProcessSkillLearn(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_SKILL_LEVEL:
			ProcessSkillLevel(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_SKILL_INIT_ALL:
			ProcessSkillInitAll(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_SKILL_INIT_SPECIFIC:
			ProcessSkillInitSpecific(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_PRODUCT_EXP_CHANGE:
			ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_COMPOSE_LEARN:
			ProcessComposeLearn(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;

		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_ALL:
		case AGPMADMIN_CHAREDIT_COMPOSE_INIT_SPECIFIC:
			ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
			break;
	}

	return TRUE;
}

BOOL AgsmAdmin::ProcessSkillLearn(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmSkill || !m_pagpmEventSkillMaster || !m_pagsmEventSkillMaster || !m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	string szSkillData;
	if(pstSkillString)
		szSkillData = pstSkillString->m_szTreeNode;

	if(szSkillData.empty())
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstCharEdit->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);
	
	if(!pcsCharacter)
	{
		ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
		return TRUE;
	}

	BOOL bResult = FALSE;

	std::string szTmp;
    int iCount = (int)std::count(szSkillData.begin(), szSkillData.end(), ':');
	int iIndex = 0, iIndex2 = 0;
	for(int i = 0; i < iCount; ++i)
	{
		iIndex2 = (int)szSkillData.find(':', iIndex);

		int iBridge = (int)szSkillData.find(',', iIndex);
		if(iBridge <= iIndex || iBridge >= iIndex2)
			break;

		szTmp = szSkillData.substr(iIndex, iBridge - iIndex);
		INT32 lSkillTID = atoi(szTmp.c_str());

		AgpdSkillTemplate* pcsSkillTemplate = m_pagpmSkill->GetSkillTemplate(lSkillTID);
		if(!pcsSkillTemplate)
			break;

		// ���� ��ȿ�� üũ
		if(!m_pagpmSkill->IsOwnSkill((ApBase*)pcsCharacter, pcsSkillTemplate) &&
			m_pagpmSkill->CheckLearnableSkill((ApBase*)pcsCharacter, pcsSkillTemplate->m_szName)
			)
		{
			// SP, Level üũ
			INT32 lCostSP = m_pagpmEventSkillMaster->GetLearnCostSP(pcsSkillTemplate);
			INT32 lLearnableLevel = m_pagpmEventSkillMaster->GetLearnableLevel(pcsSkillTemplate);
			INT32 lCharLevel = m_pagpmCharacter->GetLevel(pcsCharacter);
			INT32 lCharSP = m_pagpmCharacter->GetSkillPoint(pcsCharacter);

			if(lCostSP >= 0 && lLearnableLevel >= 0 && lCharLevel >= lLearnableLevel && lCharSP >= lCostSP && lCharSP > 0)
			{
				AgpdSkill* pcsSkill = m_pagpmSkill->LearnSkill((ApBase*)pcsCharacter, lSkillTID, 1);
				if(pcsSkill)
				{
					bResult = TRUE;

					m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, -1);
					m_pagsmEventSkillMaster->EnumCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pcsCharacter, NULL);

					// Passive Skill �̶�� ��� Cast ��Ų��.
					if(m_pagpmSkill->IsPassiveSkill(pcsSkill))
						m_pagsmSkill->CastPassiveSkill(pcsSkill);
				}
			}	// SP, Level üũ
		}

		iIndex = iIndex2 + 1;
	}

	if(bResult)
		pstCharEdit->m_lEditResult = 1;

	// ���ο��� ����� ������.
	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ������ �ƴϸ� Ǭ��.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessSkillLevel(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmSkill || !m_pagpmEventSkillMaster || !m_pagsmEventSkillMaster || !m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstCharEdit->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);
	
	if(!pcsCharacter)
	{
		ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
		return TRUE;
	}

	string szSkillData;
	if(pstSkillString)
		szSkillData = pstSkillString->m_szTreeNode;

	BOOL bResult = FALSE;

	std::string szTmp;
    int iCount = (int)std::count(szSkillData.begin(), szSkillData.end(), ':');
	int iIndex = 0, iIndex2 = 0;
	for(int i = 0; i < iCount; ++i)
	{
		iIndex2 = (int)szSkillData.find(':', iIndex);

		int iBridge = (int)szSkillData.find(',', iIndex);
		if(iBridge <= iIndex || iBridge >= iIndex2)
			break;

		szTmp = szSkillData.substr(iIndex, iBridge - iIndex);
		INT32 lSkillTID = atoi(szTmp.c_str());

		AgpdSkillTemplate* pcsSkillTemplate = m_pagpmSkill->GetSkillTemplate(lSkillTID);
		if(!pcsSkillTemplate)
			break;

		// ���� ����� ��ų�̶�� ���� ��ų�� �н�
		AgpdSkill* pcsSkill = m_pagpmSkill->GetSkillByTID(pcsCharacter, lSkillTID);
		if(!pcsSkill)
		{
			iIndex = iIndex2 + 1;
			continue;
		}

		if(pstCharEdit->m_lNewValueINT > 0)	// Level Up
		{
			if(m_pagpmEventSkillMaster->IsUpgradable(pcsSkill) &&
				!m_pagpmEventSkillMaster->IsFullUpgrade(pcsSkill))
			{
				// SP, Level üũ
				INT32 lCostSP = m_pagpmEventSkillMaster->GetUpgradeCostSP(pcsSkill);
				INT32 lCharSP = m_pagpmCharacter->GetSkillPoint(pcsCharacter);
				INT32 lSkillLevel = m_pagpmSkill->GetSkillLevel(pcsSkill);
				if(lSkillLevel < 1 || lSkillLevel >= AGPMSKILL_MAX_SKILL_CAP-1)
				{
					/*char strCharBuff[256] = { 0, };
					sprintf_s(strCharBuff, sizeof(strCharBuff), "[%s][%d]: %s(%s)", __FUNCTION__, __LINE__,
						((AgpdCharacter*)pcsSkill->m_pcsBase)->m_szID, ((AgpdSkillTemplate*) pcsSkill->m_pcsTemplate)->m_szName);
					AuLogFile_s("LOG\\SkillLevelError.txt", strCharBuff);*/
					continue;
				}
				INT32 lCharLevel = m_pagpmCharacter->GetLevel(pcsCharacter);

				if(lCostSP >= 0 && lCharSP >= lCostSP && lCharSP > 0 && lSkillLevel >= 1 &&
					lCharLevel >= pcsSkillTemplate->m_fUsedConstFactor[AGPMSKILL_CONST_REQUIRE_LEVEL][lSkillLevel + 1])
				{
					bResult = TRUE;
					m_pagpmSkill->UpdateSkillPoint(pcsSkill, 1);
					m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, -lCostSP);
				}
			}
		}
		else	// Level Down
		{
			if(m_pagpmEventSkillMaster->IsUpgradable(pcsSkillTemplate))
			{
				INT32 lSkillLevel = m_pagpmSkill->GetSkillLevel(pcsSkill);
				if(lSkillLevel > 1 && lSkillLevel < AGPMPRODUCT_MAX_LEVEL)
				{
					INT32 lCostSP = 1;
					AgpdEventSkillAttachTemplateData* pcsAttachTemplateData	= m_pagpmEventSkillMaster->GetSkillAttachTemplateData(pcsSkillTemplate);
					if(pcsAttachTemplateData && pcsAttachTemplateData->lMaxUpgradeLevel >= 1)
						lCostSP = pcsAttachTemplateData->m_stSkillCost[lSkillLevel + 1].lCostSkillPoint;

					if(lCostSP < 1)
						lCostSP = 1;

					bResult = TRUE;
					m_pagpmSkill->UpdateSkillPoint(pcsSkill, -1);
					m_pagpmCharacter->UpdateSkillPoint(pcsCharacter, lCostSP);
				}
			}
		}

		iIndex = iIndex2 + 1;
	}	// for


	if(bResult)
	{
		// DB ������ �ѹ��� �ϸ� �ǹǷ� ������ ���ȴ�.
		m_pagsmEventSkillMaster->EnumCallback(AGSMEVENTSKILL_CB_UPDATE_DB, pcsCharacter, NULL);
		pstCharEdit->m_lEditResult = 1;
		//pstCharEdit->m_lNewValueINT = m_pagpmSkill->GetSkillLevel(pcsSkill);
	}

	// ���ο��� ����� ������.
	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ������ �ƴϸ� Ǭ��.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessSkillInitAll(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmSkill || !m_pagpmEventSkillMaster || !m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstCharEdit->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);
	
	if(!pcsCharacter)
	{
		ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
		return TRUE;
	}

	// ����ϰ� �ʱ�ȭ
	BOOL bResult = m_pagpmEventSkillMaster->InitializeSkillTree(pcsCharacter, TRUE);

	if(bResult)
		pstCharEdit->m_lEditResult = 1;

	// ���ο��� ����� ������.
	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ������ �ƴϸ� Ǭ��.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessSkillInitSpecific(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmSkill || !m_pagpmEventSkillMaster || !m_pagsmCharacter || !m_pagsmSkill)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstCharEdit->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);
	
	if(!pcsCharacter)
	{
		ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
		return TRUE;
	}

	string szSkillData;
	if(pstSkillString)
		szSkillData = pstSkillString->m_szTreeNode;

	BOOL bResult = FALSE;

	std::string szTmp;
    int iCount = (int)std::count(szSkillData.begin(), szSkillData.end(), ':');
	int iIndex = 0, iIndex2 = 0;
	for(int i = 0; i < iCount; ++i)
	{
		iIndex2 = (int)szSkillData.find(':', iIndex);

		int iBridge = (int)szSkillData.find(',', iIndex);
		if(iBridge <= iIndex || iBridge >= iIndex2)
			break;

		szTmp = szSkillData.substr(iIndex, iBridge - iIndex);
		INT32 lSkillTID = atoi(szTmp.c_str());

		AgpdSkill* pcsSkill = m_pagpmSkill->GetSkillByTID(pcsCharacter, lSkillTID);
		if(!pcsSkill)
		{
			iIndex = iIndex2 + 1;
			continue;
		}

		if(!m_pagpmEventSkillMaster->CheckTargetSkillForRollback(pcsSkill))
			return FALSE;

		bResult = TRUE;

		m_pagpmEventSkillMaster->EnumCallback(AGPMEVENT_SKILL_CB_END_SKILL, pcsCharacter, pcsSkill);
		m_pagpmEventSkillMaster->EnumCallback(AGPMEVENT_SKILL_CB_REMOVE_SKILL, pcsSkill, pcsCharacter);

		m_pagpmSkill->RemoveOwnSkillList(pcsSkill);
		m_pagpmSkill->RemoveSkill(pcsSkill->m_lID);

		m_pagpmSkill->AdjustSkillPoint(pcsCharacter);

		iIndex = iIndex2 + 1;
	}

	if(bResult)
	{
		// DB ������ �ѹ��� �ϸ� �ǹǷ� ������ ���ȴ�.
		m_pagpmEventSkillMaster->EnumCallback(AGPMEVENT_SKILL_CB_SAVE_ALL_DATA, pcsCharacter, NULL);
		pstCharEdit->m_lEditResult = 1;
	}

	// ���ο��� ����� ������.
	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ������ �ƴϸ� Ǭ��.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessComposeLearn(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagpmSkill || !m_pagpmEventSkillMaster || !m_pagsmCharacter || !m_pagsmSkill || !m_pagpmProduct)
		return FALSE;

	if(!pstCharEdit || !pcsAgpdCharacter)
		return FALSE;

	AgpdCharacter* pcsCharacter = NULL;
	if(strcmp(pstCharEdit->m_szCharName, pcsAgpdCharacter->m_szID) == 0)
		pcsCharacter = pcsAgpdCharacter;
	else
		pcsCharacter = m_pagpmCharacter->GetCharacterLock(pstCharEdit->m_szCharName);
	
	if(!pcsCharacter)
	{
		ProcessSkillQuery(pstCharEdit, pstSkillString, pcsAgpdCharacter);
		return TRUE;
	}

	string szCompose;
	if(pstSkillString)
		szCompose = pstSkillString->m_szProduct;

	BOOL bResult = FALSE;

	std::string szTmp;
    int iCount = (int)std::count(szCompose.begin(), szCompose.end(), ':');
	int iIndex = 0, iIndex2 = 0;
	for(int i = 0; i < iCount; ++i)
	{
		iIndex2 = (int)szCompose.find(':', iIndex);

		szTmp = szCompose.substr(iIndex, iIndex2 - iIndex);
		INT32 lComposeTID = atoi(szTmp.c_str());

		bResult = m_pagpmProduct->LearnCompose(pcsCharacter, lComposeTID);
		iIndex = iIndex2 + 1;
	}

	if(bResult)
	{
		// DB ������ �ѹ��� �ϸ� �ǹǷ� ������ ���ȴ�.
		m_pagpmEventSkillMaster->EnumCallback(AGPMEVENT_SKILL_CB_SAVE_ALL_DATA, pcsCharacter, NULL);
		pstCharEdit->m_lEditResult = 1;
	}

	// ���ο��� ����� ������.
	SendCharEditResult(pstCharEdit, pcsAgpdCharacter->m_lID, m_pagsmCharacter->GetCharDPNID(pcsCharacter));

	// ������ �ƴϸ� Ǭ��.
	if(pcsCharacter != pcsAgpdCharacter)
		pcsCharacter->m_Mutex.Release();

	return TRUE;
}

BOOL AgsmAdmin::ProcessSkillQuery(stAgpdAdminCharEdit* pstCharEdit, stAgpdAdminSkillString* pstSkillString, AgpdCharacter* pcsAgpdCharacter)
{
	if(!m_pagsmEventSkillMaster || !m_pagsmCharacter)
		return FALSE;

	if(!pstCharEdit || !pstSkillString || !pcsAgpdCharacter)
		return FALSE;

	// �¶����� ĳ���ʹ� ���� �ʴ´�.
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(pstSkillString->m_szCharName);
	if(pcsCharacter)
		return FALSE;

	// ����ϰ� ����!!!
	EnumCallback(AGSMADMIN_CB_RELAY_SKILL_UPDATE, pstSkillString, pcsAgpdCharacter);
	return TRUE;
}