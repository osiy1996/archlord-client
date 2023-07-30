//
//	AgpmItemConvert.cpp
//		���� ������ ���õ� �Լ����� ����ִ�.
////////////////////////////////////////////////////////////////////

#include "AgpmItem.h"
#include "AgpdItemRuneTemplate.h"

//INT32	g_alRuneAttributeType[AGPMITEM_RUNE_ATTR_MAX] = 
//{
//	AGPMITEM_EQUIP_KIND_WEAPON,									// AGPMITEM_RUNE_ATTR_ADD_DAMAGE
//	AGPMITEM_EQUIP_KIND_WEAPON,									// AGPMITEM_RUNE_ATTR_ADD_AR
//	AGPMITEM_EQUIP_KIND_SHIELD,									// AGPMITEM_RUNE_ATTR_ADD_BLOCK_RATE
//	AGPMITEM_EQUIP_KIND_ARMOUR,									// AGPMITEM_RUNE_ATTR_ADD_AC
//	AGPMITEM_EQUIP_KIND_ARMOUR,									// AGPMITEM_RUNE_ATTR_ADD_MAX_HP
//	AGPMITEM_EQUIP_KIND_ARMOUR,									// AGPMITEM_RUNE_ATTR_ADD_MAX_MP
//	AGPMITEM_EQUIP_KIND_ARMOUR,									// AGPMITEM_RUNE_ATTR_ADD_MAX_SP
//	AGPMITEM_EQUIP_KIND_WEAPON | AGPMITEM_EQUIP_KIND_SHIELD,	// AGPMITEM_RUNE_ATTR_ADD_RECOVERY_HP
//	AGPMITEM_EQUIP_KIND_WEAPON | AGPMITEM_EQUIP_KIND_SHIELD,	// AGPMITEM_RUNE_ATTR_ADD_RECOVERY_MP
//	AGPMITEM_EQUIP_KIND_WEAPON | AGPMITEM_EQUIP_KIND_SHIELD,	// AGPMITEM_RUNE_ATTR_ADD_RECOVERY_SP
//	AGPMITEM_EQUIP_KIND_WEAPON									// AGPMITEM_RUNE_ATTR_ADD_ATTACK_SPEED
//};

//BOOL AgpmItem::CheckConvertRequest(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return FALSE;
//
//	if (!pcsItem->m_pcsCharacter)
//	{
//		pcsItem->m_pcsCharacter	= m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
//	}
//
//	if (!pcsItem->m_pcsCharacter)
//		return FALSE;
//
//	// pcsSpiritStone�� spirit stone�� �´���... ������ �� �ִ� �������� �˻�.
//	if (((AgpdItemTemplate *) pcsSpiritStone->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE ||
//		((AgpdItemTemplateUsable *) pcsSpiritStone->m_pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
//		return AGPMITEM_CONVERT_FAIL;
//
//	AgpmItemConvertResult	eCheckResult	= IsConvertable(pcsItem, pcsSpiritStone);
//
//	if (eCheckResult != AGPMITEM_CONVERT_SUCCESS)
//	{
//		// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//
//		INT32	lResult	= (INT32) eCheckResult;
//
//		PVOID	pvBuffer[3];
//		pvBuffer[0]	= &lResult;
//		pvBuffer[1]	= NULL;
//		pvBuffer[2] = NULL;
//
//		EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//
//		return FALSE;							// ������ �� ����.
//	}
//
//	// Ŭ���̾�Ʈ���� ������ ������ �Ұ����� �ٽ��ѹ� �����.
//	//		- Callback�� �ҷ��ش�.
//	/////////////////////////////////////////////////////////////////////
//	return EnumCallback(ITEM_CB_ID_ASK_REALLY_CONVERT_ITEM, pcsItem, pcsSpiritStone);
//}
//
//AgpmItemConvertResult AgpmItem::IsConvertable(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return AGPMITEM_CONVERT_FAIL;
//
//	// check pcsSpiritStone
//
//	// pcsSpiritStone�� spirit stone�� �´���... ������ �� �ִ� �������� �˻�.
//	if (((AgpdItemTemplate *) pcsSpiritStone->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE ||
//		((AgpdItemTemplateUsable *) pcsSpiritStone->m_pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SPIRIT_STONE)
//		return AGPMITEM_CONVERT_FAIL;
//
//	// check pcsItem
//
//	// ������ �� �ִ� ���������� �˻�.
//	//	1. equip �� �� �ִ� ������ �˻�
//	if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_EQUIP)
//		return AGPMITEM_CONVERT_NOT_EQUIP_ITEM;
//
//	//	2. ego item, legendary item ���� �˻�.
//	//
//	//		ego item, legendary item ��� ���� ������ �ȵǾ� �ִ�.
//	//
//	//
//	//
//
//	//	3. �̹� ������ �� �ߴ��� �˻�.
//	if (IsConvertHistoryFull(pcsItem))
//		return AGPMITEM_CONVERT_FULL;
//
//	//	4. Rank�� �´��� �˻�.
//	INT32	lRankSpiritStone;
//	INT32	lRankItem;
//
//	if (m_pagpmFactors)
//	{
//		m_pagpmFactors->GetValue(&pcsSpiritStone->m_csFactor, &lRankSpiritStone, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
//		m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lRankItem, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
//
//		if (lRankSpiritStone != lRankItem)
//			return AGPMITEM_CONVERT_DIFFER_RANK;
//	}
//
//	//	5. ���� Ÿ�԰� ���ɼ��� �Ӽ� Ÿ�԰� ��ġ�ϴ��� �˻��Ѵ�.
//	//		(weapon : Fire/Water/Earth/Air/Magic/Thunder/Ice/Poison)
//	//		(armour(shield) : Fire/Water/Earth/Air/Magic/Str/Dex/Int/Wis/Con/AC)
//	if (!CheckConvertAttr(pcsItem, pcsSpiritStone))
//		return AGPMITEM_CONVERT_DIFFER_TYPE;
//
//	return AGPMITEM_CONVERT_SUCCESS;
//}
//
//BOOL AgpmItem::CheckConvertAttr(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return FALSE;
//
//	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
//	case AGPMITEM_EQUIP_KIND_WEAPON:
//		{
//			//  weapon�� ���� Damage factor�� �Ӽ��鸸 ������ �� �ִ�.
//			//	(weapon : Fire/Water/Earth/Air/Magic/Thunder/Ice/Poison)
//			switch (((AgpdItemTemplateUsableSpiritStone *) pcsSpiritStone->m_pcsItemTemplate)->m_eSpiritStoneType) {
//			case AGPMITEM_USABLE_SS_TYPE_FIRE:
//			case AGPMITEM_USABLE_SS_TYPE_AIR:
//			case AGPMITEM_USABLE_SS_TYPE_WATER:
//			case AGPMITEM_USABLE_SS_TYPE_EARTH:
//			case AGPMITEM_USABLE_SS_TYPE_MAGIC:
//			case AGPMITEM_USABLE_SS_TYPE_ICE:
//			case AGPMITEM_USABLE_SS_TYPE_LIGHTENING:
//			case AGPMITEM_USABLE_SS_TYPE_POISON:
//				return TRUE;
//				break;
//
//			default:
//				return FALSE;
//				break;
//			}
//		}
//		break;
//
//	case AGPMITEM_EQUIP_KIND_ARMOUR:
//	case AGPMITEM_EQUIP_KIND_SHIELD:
//		{
//			//	armour(shield)�� ���� defense�� 1�� �Ӽ��� status�� ������ �� �ִ�.
//			//	(armour(shield) : Fire/Water/Earth/Air/Magic/Str/Dex/Int/Wis/Con/AC)
//			switch (((AgpdItemTemplateUsableSpiritStone *) pcsSpiritStone->m_pcsItemTemplate)->m_eSpiritStoneType) {
//			case AGPMITEM_USABLE_SS_TYPE_FIRE:
//			case AGPMITEM_USABLE_SS_TYPE_AIR:
//			case AGPMITEM_USABLE_SS_TYPE_WATER:
//			case AGPMITEM_USABLE_SS_TYPE_EARTH:
//			case AGPMITEM_USABLE_SS_TYPE_MAGIC:
//			case AGPMITEM_USABLE_SS_TYPE_CON:
//			case AGPMITEM_USABLE_SS_TYPE_WIS:
//			case AGPMITEM_USABLE_SS_TYPE_DEX:
//			case AGPMITEM_USABLE_SS_TYPE_STR:
//			case AGPMITEM_USABLE_SS_TYPE_INT:
//				return TRUE;
//				break;
//
//			default:
//				return FALSE;
//				break;
//			}
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return FALSE;
//}
//
//BOOL AgpmItem::ConvertItem(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bIsAlwaysSuccess)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return FALSE;
//
//	AgpmItemConvertResult	eCheckResult	= IsConvertable(pcsItem, pcsSpiritStone);
//
//	if (eCheckResult != AGPMITEM_CONVERT_SUCCESS)
//	{
//		// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//
//		INT32	lResult	= (INT32) eCheckResult;
//
//		PVOID	pvBuffer[3];
//		pvBuffer[0]	= &lResult;
//		pvBuffer[1]	= NULL;
//		pvBuffer[2] = NULL;
//
//		EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//
//		return FALSE;							// ������ �� ����.
//	}
//
//	// ���� ���� ���
//	INT32	lResult	= CalcConvertResult(pcsItem, pcsSpiritStone, bIsAlwaysSuccess);
//
//	// �����ߴ� ���� ���� �� �Ѿ��. ����Ұ� ���°Ŵ�.
//	INT32	lConvertCount	= pcsItem->m_stConvertHistory.lConvertLevel - GetNumRuneConvert(pcsItem);
//	if ((lResult == AGPMITEM_CONVERT_DESTROY_ATTRIBUTE || lResult == AGPMITEM_CONVERT_DESTROY_ITEM) && lConvertCount < 1)
//		lResult =  AGPMITEM_CONVERT_FAIL;
//
//	PVOID	pvPacketUpdateFactor	= NULL;
//
//	BOOL bRemoveItem = FALSE;
//
//	switch (lResult) {
//	case AGPMITEM_CONVERT_DESTROY_ATTRIBUTE:
//		{
//			// ���д�. ������ spirit stone�� ������ ���ư��Ű� �̹� �����ߴ� ���� �ϳ��� �� ������.
//			// ���� ¥��������.
//
//			// 1. �����ߴ� ���� �ϳ��� ����. (������ ���� ���ش�.)
//
//			/*
//			// �� ���������� ������ �Ӽ��� ������ ����. �� ���� �����Ѱ� �ִ��� ����.
//			for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//				if (pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//					break;
//			if (i == pcsItem->m_stConvertHistory.lConvertLevel)
//				return FALSE;
//			*/
//
//			// ������ ��
//			INT32	lRandomNumber = pcsItem->m_stConvertHistory.lConvertLevel - 1;
//
//			// �� �Ӽ��� ���� �� ����.
//			if (pcsItem->m_stConvertHistory.bUseSpiritStone[lRandomNumber])
//			{
//				// 2. ������ ������ �����丮���� ã�� ���ۿ��� �� �Ӽ��� ����.
//				if (m_pagpmFactors)
//				{
//					pvPacketUpdateFactor = m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsItem->m_stConvertHistory.csFactorHistory[lRandomNumber], TRUE, TRUE, FALSE);
//
//					// 3. �����丮���� ������ ���� �����Ѵ�.
//					RemoveConvertHistory(pcsItem, lRandomNumber);
//				}
//			}
//		}
//		break;
//
//	case AGPMITEM_CONVERT_DESTROY_ITEM:
//		{
//			// ���۱����� ���� ������ �Ѵ�. ��... ���Ǿ���.
//			//RemoveItem(pcsItem->m_lID, TRUE);
//
//			// Flag �� ������ ��, Callback �ҷ��� �� �����. - 2004.04.06 steeple
//			bRemoveItem = TRUE;
//		}
//		break;
//
//	case AGPMITEM_CONVERT_SUCCESS:
//		{
//			// ����... pcsSpiritStone�� factor�� Item�� Add �ϰ� Convert History�� �߰��Ѵ�.
//			RequireLevelUp(pcsItem, AGPMITEM_REQUIRE_LEVEL_UP_STEP);
//
//			pcsItem->m_stConvertHistory.fConvertConstant	= 2.6 + m_csRandom.randInt(40) / 100.0;
//
//			if (m_pagpmFactors)
//			{
//				// 1. spirit stone �� factor�� Item�� ADD �Ѵ�.
//				pvPacketUpdateFactor = m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor, &pcsSpiritStone->m_csFactor, TRUE, TRUE, TRUE);
//
//				//m_pagpmFactors->CalcFactor(&pcsItem->m_csRestrictFactor, &pcsSpiritStone->m_csRestrictFactor, TRUE, FALSE, TRUE);
//
//				// 2. ���� �����丮�� �߰��Ѵ�.
//				AddConvertHistory(pcsItem, pcsSpiritStone);
//			}
//		}
//		break;
//
//	case AGPMITEM_CONVERT_FAIL:
//		{
//			// �ϰ͵� ���Ѵ�. spirit stone�� ���߿� �����Ѵ�.
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//	PVOID	pvBuffer[3];
//	pvBuffer[0]	= &lResult;
//	pvBuffer[1]	= pvPacketUpdateFactor;
//	pvBuffer[2] = pcsSpiritStone;
//
//	EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//
//	if (pvPacketUpdateFactor)
//		m_csPacket.FreePacket(pvPacketUpdateFactor);
//
//	// pcsSpiritStone�� �����Ѵ�.
//	//pcsSpiritStone->m_lDeleteReason	= 2;
//	strncpy(pcsSpiritStone->m_szDeleteReason, "���������� ���ɼ� ����", AGPMITEM_MAX_DELETE_REASON);
//
//	if (!RemoveItem(pcsSpiritStone->m_lID, TRUE))
//		return FALSE;
//
//	// RemoveFlag �� TRUE ��� �����. - 2004.04.06. steeple
//	if(bRemoveItem)
//	{
//		strncpy(pcsItem->m_szDeleteReason, "�������з� ������ ����", AGPMITEM_MAX_DELETE_REASON);
//		RemoveItem(pcsItem->m_lID, TRUE);
//	}
//
//	return TRUE;
//}
//
//INT32 AgpmItem::GetNumConvertAttr(AgpdItem *pcsItem, INT32	lAttributeType)
//{
//	if (!pcsItem ||
//		lAttributeType < AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC ||
//		lAttributeType > AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
//		return 0;
//
//	// pcsItem�� ������ �̷������� ���캻��.
//	if (pcsItem->m_stConvertHistory.lConvertLevel < 0)
//		return 0;
//
//	INT32	lNumConvertAttr	= 0;
//
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (pcsItem->m_stConvertHistory.bUseSpiritStone[i] &&
//			pcsItem->m_stConvertHistory.pcsTemplate[i])
//		{
//			if (((AgpdItemTemplateUsableSpiritStone *) pcsItem->m_stConvertHistory.pcsTemplate[i])->m_eSpiritStoneType == lAttributeType)
//				++lNumConvertAttr;
//
//			/*
//			switch (((AgpdItemTemplateUsableSpiritStone *) pcsItem->m_stConvertHistory.pcsTemplate[i])->m_eSpiritStoneType) {
//			case AGPMITEM_USABLE_SS_TYPE_FIRE:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_AIR:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_AIR)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_WATER:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_WATER)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_EARTH:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_MAGIC:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_ICE:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_ICE)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_LIGHTENING:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_LIGHTENING)
//						++lNumConvertAttr;
//				break;
//			case AGPMITEM_USABLE_SS_TYPE_POISON:
//					if (lAttributeType == AGPD_FACTORS_ATTRIBUTE_TYPE_POISON)
//						++lNumConvertAttr;
//				break;
//			}
//			*/
//		}
//	}
//
//	return lNumConvertAttr;
//}
//
//BOOL AgpmItem::IsConvertHistoryFull(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return TRUE;
//
//	// validation check
//	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
//	case AGPMITEM_EQUIP_KIND_WEAPON:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_CONVERT_WEAPON)
//				return TRUE;
//		}
//		break;
//
//	case AGPMITEM_EQUIP_KIND_ARMOUR:
//	case AGPMITEM_EQUIP_KIND_SHIELD:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_CONVERT_ARMOUR)
//				return TRUE;
//		}
//		break;
//
//	default:
//		return TRUE;
//		break;
//	}
//
//	return FALSE;
//}
//
//BOOL AgpmItem::IsConvertHistoryEmpty(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return FALSE;
//
//	if (pcsItem->m_stConvertHistory.lConvertLevel > 0)
//		return FALSE;
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddConvertHistory(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bUseSpiritStone)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return FALSE;
//
//	// validation check
//	return AddConvertHistory(pcsItem, &pcsSpiritStone->m_csFactor, &pcsSpiritStone->m_csFactorPercent, bUseSpiritStone, ((AgpdItemTemplate *) pcsSpiritStone->m_pcsItemTemplate)->m_lID);
//}
//
//BOOL AgpmItem::AddConvertHistory(AgpdItem *pcsItem, AgpdFactor *pcsPointFactor, AgpdFactor *pcsPercentFactor, BOOL bUseSpiritStone, INT32 lTID)
//{
//	if (!pcsItem || (!pcsPointFactor && !pcsPercentFactor))
//		return FALSE;
//
//	if (bUseSpiritStone)
//	{
//		if (lTID == 0)
//			return FALSE;
//	}
//
//	// validation check
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	if (m_pagpmFactors)
//	{
//		if (pcsPointFactor)
//		{
//			m_pagpmFactors->CopyFactor(&pcsItem->m_stConvertHistory.csFactorHistory[pcsItem->m_stConvertHistory.lConvertLevel],
//									   pcsPointFactor,
//									   TRUE,
//									   FALSE);
//		}
//		
//		if (pcsPercentFactor)
//		{
//			m_pagpmFactors->CopyFactor(&pcsItem->m_stConvertHistory.csFactorPercentHistory[pcsItem->m_stConvertHistory.lConvertLevel],
//									   pcsPercentFactor,
//									   TRUE,
//									   FALSE);
//		}
//	}
//	else
//		return TRUE;
//
//	pcsItem->m_stConvertHistory.bUseSpiritStone[pcsItem->m_stConvertHistory.lConvertLevel] = bUseSpiritStone;
//
//	pcsItem->m_stConvertHistory.lTID[pcsItem->m_stConvertHistory.lConvertLevel] = lTID;
//	pcsItem->m_stConvertHistory.pcsTemplate[pcsItem->m_stConvertHistory.lConvertLevel] = GetItemTemplate(lTID);
//
//	++pcsItem->m_stConvertHistory.lConvertLevel;
//
//	EnumCallback(ITEM_CB_ID_ADD_CONVERT_HISTORY, pcsItem, NULL);
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddConvertHistory(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute, INT32 lAttributeValue)
//{
//	if (!pcsItem ||
//		AGPMITEM_RUNE_ATTR_NONE >= eRuneAttribute ||
//		eRuneAttribute >= AGPMITEM_RUNE_ATTR_MAX ||
//		lAttributeValue <= 0)
//		return FALSE;
//
//	// validation check
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	pcsItem->m_stConvertHistory.bUseSpiritStone[pcsItem->m_stConvertHistory.lConvertLevel]		= FALSE;
//
//	pcsItem->m_stConvertHistory.eRuneAttribute[pcsItem->m_stConvertHistory.lConvertLevel]		= eRuneAttribute;
//	pcsItem->m_stConvertHistory.lRuneAttributeValue[pcsItem->m_stConvertHistory.lConvertLevel]	= lAttributeValue;
//
//	SetRuneAttributeValue(&pcsItem->m_stConvertHistory.csFactorHistory[pcsItem->m_stConvertHistory.lConvertLevel], eRuneAttribute, lAttributeValue);
//
//	pcsItem->m_stConvertHistory.lTID[pcsItem->m_stConvertHistory.lConvertLevel]					= AP_INVALID_IID;
//	pcsItem->m_stConvertHistory.pcsTemplate[pcsItem->m_stConvertHistory.lConvertLevel]			= NULL;
//
//	++pcsItem->m_stConvertHistory.lConvertLevel;
//
//	EnumCallback(ITEM_CB_ID_ADD_CONVERT_HISTORY, pcsItem, NULL);
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddConvertHistoryFactor(AgpdItem *pcsItem, AgpdFactor *pcsFactor, INT32 lIndex)
//{
//	if (!pcsItem || !pcsFactor || lIndex < 0 || lIndex >= AGPMITEM_MAX_CONVERT)
//		return FALSE;
//
//	// validation check
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	return TRUE;
//}
//
//BOOL AgpmItem::RemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex)
//{
//	if (!pcsItem || lIndex > AGPMITEM_MAX_CONVERT)
//		return FALSE;
//
//	// validation check
//	if (IsConvertHistoryEmpty(pcsItem))
//		return FALSE;
//
//	EnumCallback(ITEM_CB_ID_REMOVE_CONVERT_HISTORY, pcsItem, &lIndex);
//
//	if (m_pagpmFactors)
//	{
//		m_pagpmFactors->DestroyFactor(pcsItem->m_stConvertHistory.csFactorHistory + lIndex);
//		m_pagpmFactors->DestroyFactor(pcsItem->m_stConvertHistory.csFactorPercentHistory + lIndex);
//	}
//
//	CopyMemory(pcsItem->m_stConvertHistory.csFactorHistory + lIndex,
//			   pcsItem->m_stConvertHistory.csFactorHistory + lIndex + 1,
//			   sizeof(AgpdFactor) * (pcsItem->m_stConvertHistory.lConvertLevel - lIndex - 1));
//
//	CopyMemory(pcsItem->m_stConvertHistory.csFactorPercentHistory + lIndex,
//			   pcsItem->m_stConvertHistory.csFactorPercentHistory + lIndex + 1,
//			   sizeof(AgpdFactor) * (pcsItem->m_stConvertHistory.lConvertLevel - lIndex - 1));
//
//	CopyMemory(pcsItem->m_stConvertHistory.bUseSpiritStone + lIndex,
//			   pcsItem->m_stConvertHistory.bUseSpiritStone + lIndex + 1,
//			   sizeof(BOOL) * (pcsItem->m_stConvertHistory.lConvertLevel - lIndex - 1));
//
//	CopyMemory(pcsItem->m_stConvertHistory.lTID + lIndex,
//			   pcsItem->m_stConvertHistory.lTID + lIndex + 1,
//			   sizeof(INT32) * (pcsItem->m_stConvertHistory.lConvertLevel - lIndex - 1));
//
//	CopyMemory(pcsItem->m_stConvertHistory.pcsTemplate + lIndex,
//			   pcsItem->m_stConvertHistory.pcsTemplate + lIndex + 1,
//			   sizeof(AgpdItemTemplate *) * (pcsItem->m_stConvertHistory.lConvertLevel - lIndex - 1));
//
//	pcsItem->m_stConvertHistory.bUseSpiritStone[pcsItem->m_stConvertHistory.lConvertLevel] = FALSE;
//
//	--pcsItem->m_stConvertHistory.lConvertLevel;
//
//	ZeroMemory(pcsItem->m_stConvertHistory.csFactorHistory + AGPMITEM_MAX_CONVERT - 1, sizeof(AgpdFactor));
//	ZeroMemory(pcsItem->m_stConvertHistory.csFactorPercentHistory + AGPMITEM_MAX_CONVERT - 1, sizeof(AgpdFactor));
//
//	ZeroMemory(pcsItem->m_stConvertHistory.bUseSpiritStone + AGPMITEM_MAX_CONVERT - 1, sizeof(BOOL));
//
//	ZeroMemory(pcsItem->m_stConvertHistory.lTID + AGPMITEM_MAX_CONVERT - 1, sizeof(INT32));
//
//	return TRUE;
//}
//
//BOOL AgpmItem::InitConvertHistory(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return FALSE;
//
//	ZeroMemory(&pcsItem->m_stConvertHistory, sizeof(pcsItem->m_stConvertHistory));
//
//	return TRUE;
//}
//
//INT32 AgpmItem::CalcConvertResult(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, BOOL bIsAlwaysSuccess)
//{
//	if (!pcsItem || !pcsSpiritStone)
//		return AGPMITEM_CONVERT_FAIL;
//
//	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
//	case AGPMITEM_EQUIP_KIND_WEAPON:
//		{
//			AgpmItemConvertResult	eResult = AGPMITEM_CONVERT_FAIL;
//
//			//	�����ۿ� ��ũ�� �ִܴ�. ����... ��.��
//			INT32	lItemRank	= 0;
//			if (m_pagpmFactors)
//				m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
//
//			INT32	lConvertCount	= pcsItem->m_stConvertHistory.lConvertLevel - GetNumRuneConvert(pcsItem);
//			if (lConvertCount < 0)
//				return FALSE;
//
//			if (bIsAlwaysSuccess)
//				eResult	= AGPMITEM_CONVERT_SUCCESS;
//			else
//			{
//				FLOAT	fRandomNumber	= (FLOAT) m_csRandom.randInt(100);
//
//				//	����1 : 2+(5-Rank)*5.8+(10-����Ƚ��)*3.87	��������Ȯ��
//				/////////////////////////////////////////////////////////////////
//				FLOAT	fFormula1		= 2 + (5 - lItemRank) * 5.8 + (10 - lConvertCount) * 3.87;
//
//				if (fRandomNumber <= fFormula1)
//					eResult	= AGPMITEM_CONVERT_SUCCESS;
//				else
//				{
//					//	����2 : 30+(Rank-1)+(����Ƚ��-1)*0.67	��������Ȯ��
//					////////////////////////////////////////////////////////////////
//					FLOAT	fFormula2		= 30 + (lItemRank - 1) + (lConvertCount - 1) * 0.67;
//
//					if (fFormula1 < fRandomNumber <= fFormula1 + fFormula2)
//					{
//						eResult	= AGPMITEM_CONVERT_FAIL;
//					}
//					else
//					{
//						//	����3 : 8+(Rank-1)*3+(����Ƚ��-1)*2		���ɼ� �ı�Ȯ��
//						////////////////////////////////////////////////////////////////
//						FLOAT	fFormula3		= 8 + (lItemRank - 1) * 3 + (lConvertCount - 1) * 2;
//
//						if (fFormula1 + fFormula2 < fRandomNumber < fFormula1 + fFormula2 + fFormula3)
//						{
//							eResult	= AGPMITEM_CONVERT_DESTROY_ATTRIBUTE;
//						}
//						else
//						{
//							eResult	= AGPMITEM_CONVERT_DESTROY_ITEM;
//						}
//					}
//				}
//			}
//
//			switch (eResult) {
//			case AGPMITEM_CONVERT_SUCCESS:
//				{
//					if (m_pagpmFactors)
//					{
//						for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
//						{
//							if (i != AGPD_FACTORS_TYPE_DAMAGE)
//							{
//								m_pagpmFactors->DestroyFactor(&pcsSpiritStone->m_csFactor, i);
//								m_pagpmFactors->DestroyFactor(&pcsSpiritStone->m_csFactorPercent, i);
//							}
//						}
//					}
//				}
//				break;
//
//			case AGPMITEM_CONVERT_FAIL:
//				{
//				}
//				break;
//
//			case AGPMITEM_CONVERT_DESTROY_ATTRIBUTE:
//				{
//				}
//				break;
//
//			case AGPMITEM_CONVERT_DESTROY_ITEM:
//				{
//				}
//				break;
//			}
//
//			return eResult;
//		}
//		break;
//
//	case AGPMITEM_EQUIP_KIND_ARMOUR:
//	case AGPMITEM_EQUIP_KIND_SHIELD:
//		{
//			AgpmItemConvertResult	eResult = AGPMITEM_CONVERT_FAIL;
//
//			//	�����ۿ� ��ũ�� �ִܴ�. ����... ��.��
//			INT32	lItemRank	= 0;
//			if (m_pagpmFactors)
//				m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lItemRank, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_RANK);
//
//			INT32	lConvertCount	= pcsItem->m_stConvertHistory.lConvertLevel - GetNumRuneConvert(pcsItem);
//			if (lConvertCount < 0)
//				return FALSE;
//
//			if (bIsAlwaysSuccess)
//				eResult	= AGPMITEM_CONVERT_SUCCESS;
//			else
//			{
//				FLOAT	fRandomNumber	= (FLOAT) m_csRandom.randInt(100);
//
//				//	����1 : 2+(5-Rank)*5.8+(5-����Ƚ��)*8.7	��������Ȯ��
//				/////////////////////////////////////////////////////////////////
//				FLOAT	fFormula1		= 2 + (5 - lItemRank) * 5.8 + (5 - lConvertCount) * 8.7;
//
//				if (fRandomNumber <= fFormula1)
//					eResult	= AGPMITEM_CONVERT_SUCCESS;
//				else
//				{
//					//	����2 : 30+(Rank-1)+(����Ƚ��-1)*1.5	��������Ȯ��
//					////////////////////////////////////////////////////////////////
//					FLOAT	fFormula2		= 30 + (lItemRank - 1) + (lConvertCount - 1) * 1.5;
//
//					if (fFormula1 < fRandomNumber <= fFormula1 + fFormula2)
//					{
//						eResult	= AGPMITEM_CONVERT_FAIL;
//					}
//					else
//					{
//						//	����3 : 8+(Rank-1)*3+(����Ƚ��-1)*4.5	���ɼ� �ı�Ȯ��
//						////////////////////////////////////////////////////////////////
//						FLOAT	fFormula3		= 8 + (lItemRank - 1) * 3 + (lConvertCount - 1) * 4.5;
//
//						if (fFormula1 + fFormula2 < fRandomNumber < fFormula1 + fFormula2 + fFormula3)
//						{
//							eResult	= AGPMITEM_CONVERT_DESTROY_ATTRIBUTE;
//						}
//						else
//						{
//							eResult	= AGPMITEM_CONVERT_DESTROY_ITEM;
//						}
//					}
//				}
//			}
//
//			switch (eResult) {
//			case AGPMITEM_CONVERT_SUCCESS:
//				{
//					if (m_pagpmFactors)
//					{
//						for (int i = 0; i < AGPD_FACTORS_MAX_TYPE; ++i)
//						{
//							if (i != AGPD_FACTORS_TYPE_DEFENSE &&
//								i != AGPD_FACTORS_TYPE_CHAR_STATUS)
//							{
//								m_pagpmFactors->DestroyFactor(&pcsSpiritStone->m_csFactor, i);
//								m_pagpmFactors->DestroyFactor(&pcsSpiritStone->m_csFactorPercent, i);
//							}
//						}
//					}
//				}
//				break;
//
//			case AGPMITEM_CONVERT_FAIL:
//				{
//				}
//				break;
//
//			case AGPMITEM_CONVERT_DESTROY_ATTRIBUTE:
//				{
//				}
//				break;
//
//			case AGPMITEM_CONVERT_DESTROY_ITEM:
//				{
//				}
//				break;
//			}
//
//			return eResult;
//		}
//		break;
//
//	default:
//		return TRUE;
//		break;
//	}
//
//	return AGPMITEM_CONVERT_FAIL;
//}
//
//BOOL AgpmItem::UpdateConvertHistory(AgpdItem *pcsItem, INT8 cConvertLevel, FLOAT fConvertConstant, INT8 *pcUseSpiritStone, INT32 *plTID, INT8 *peRuneAttribute, INT8 *plAttributeValue, PVOID pvPacketFactorPoint[], PVOID pvPacketFactorPercent[])
//{
//	if (!pcsItem ||
//		cConvertLevel <= 0 ||
//		fConvertConstant < (FLOAT) 2.6 ||
//		fConvertConstant > (FLOAT) 3.0 ||
//		!pcUseSpiritStone ||
//		!plTID ||
//		!peRuneAttribute ||
//		!plAttributeValue ||
//		!pvPacketFactorPoint ||
//		!pvPacketFactorPercent)
//		return FALSE;
//
//	if (!InitConvertHistory(pcsItem))
//		return FALSE;
//
//	for (int i = 0; i < cConvertLevel; ++i)
//	{
//		m_pagpmFactors->ReflectPacket(pcsItem->m_stConvertHistory.csFactorHistory + i, pvPacketFactorPoint[i], 0);
//		m_pagpmFactors->ReflectPacket(pcsItem->m_stConvertHistory.csFactorPercentHistory + i, pvPacketFactorPercent[i], 0);
//
//		pcsItem->m_stConvertHistory.bUseSpiritStone[i]		= (BOOL) pcUseSpiritStone[i];
//
//		pcsItem->m_stConvertHistory.lTID[i]					= plTID[i];
//		pcsItem->m_stConvertHistory.pcsTemplate[i]			= GetItemTemplate(plTID[i]);
//
//		pcsItem->m_stConvertHistory.eRuneAttribute[i]		= (AgpmItemRuneAttribute) peRuneAttribute[i];
//		pcsItem->m_stConvertHistory.lRuneAttributeValue[i]	= (INT32) plAttributeValue[i];
//
//		if (!pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//			SetRuneAttributeValue(&pcsItem->m_stConvertHistory.csFactorHistory[i], pcsItem->m_stConvertHistory.eRuneAttribute[i], pcsItem->m_stConvertHistory.lRuneAttributeValue[i]);
//	}
//
//	pcsItem->m_stConvertHistory.lConvertLevel = (INT32) cConvertLevel;
//	pcsItem->m_stConvertHistory.fConvertConstant	= fConvertConstant;
//
//	EnumCallback(ITEM_CB_ID_UPDATE_CONVERT_HISTORY, pcsItem, NULL);
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddConvertHistoryPacket(AgpdItem *pcsItem, FLOAT fConvertConstant, PVOID pvPacketAddFactorPoint, PVOID pvPacketAddFactorPercent, BOOL bSpiritStone, INT32 lTID)
//{
//	if (!pcsItem || (!pvPacketAddFactorPoint && !pvPacketAddFactorPercent))
//		return FALSE;
//
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	AgpdFactor	csUpdateFactorPoint;
//	AgpdFactor	csUpdateFactorPercent;
//
//	m_pagpmFactors->InitFactor(&csUpdateFactorPoint);
//	m_pagpmFactors->InitFactor(&csUpdateFactorPercent);
//
//	if (pvPacketAddFactorPoint)
//		m_pagpmFactors->ReflectPacket(&csUpdateFactorPoint, pvPacketAddFactorPoint, 0);
//	if (pvPacketAddFactorPercent)
//		m_pagpmFactors->ReflectPacket(&csUpdateFactorPercent, pvPacketAddFactorPercent, 0);
//
//	BOOL	bRetval = AddConvertHistory(pcsItem, &csUpdateFactorPoint, &csUpdateFactorPercent, bSpiritStone, lTID);
//
//	m_pagpmFactors->DestroyFactor(&csUpdateFactorPoint);
//	m_pagpmFactors->DestroyFactor(&csUpdateFactorPercent);
//
//	pcsItem->m_stConvertHistory.fConvertConstant	= fConvertConstant;
//
//	return bRetval;
//}
//
///*
//BOOL AgpmItem::CheckAddRuneAttribute(AgpdItem *pcsItem, INT32 lRuneTID, INT32 lRuneAttrLevel)
//{
//	if (!pcsItem || lRuneTID == 0 || lRuneAttrLevel < 1 || lRuneAttrLevel > AGPMITEM_MAX_RUNE_ATTR_LEVEL)
//		return FALSE;
//
//	if (!pcsItem->m_pcsItemTemplate)
//		return FALSE;
//
//	AgpdItemRuneTemplate	*pcsItemRuneTemplate	= GetItemRuneTemplate(lRuneTID);
//	if (!pcsItemRuneTemplate)
//		return FALSE;
//
//	// ���� ���������� �˻��Ѵ�.
//	if (IsEgoItem(pcsItem))
//		return FALSE;
//
//	// ������ �ѹ��̶� �� ���������� �˻��Ѵ�.
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//			return FALSE;
//	}
//
//	//	�̹� ������ �� �ߴ��� �˻�.
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
//	case AGPMITEM_EQUIP_KIND_WEAPON:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_WEAPON)
//				return FALSE;
//
//			if (!(pcsItemRuneTemplate->m_nType & AGPMITEM_EQUIP_KIND_WEAPON))
//				return FALSE;
//		}
//		break;
//
//	case AGPMITEM_EQUIP_KIND_ARMOUR:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_ARMOUR)
//				return FALSE;
//
//			if (!(pcsItemRuneTemplate->m_nType & AGPMITEM_EQUIP_KIND_ARMOUR))
//				return FALSE;
//		}
//	case AGPMITEM_EQUIP_KIND_SHIELD:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_ARMOUR)
//				return FALSE;
//
//			if (!(pcsItemRuneTemplate->m_nType & AGPMITEM_EQUIP_KIND_SHIELD))
//				return FALSE;
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddRuneAttribute(AgpdItem *pcsItem, INT32 lTID, INT32 lRuneAttrValue)
//{
//	if (!pcsItem || lRuneTID == 0 || lRuneAttrLevel < 1 || lRuneAttrLevel > AGPMITEM_MAX_RUNE_ATTR_LEVEL)
//		return FALSE;
//
//	if (!pcsItem->m_pcsItemTemplate)
//		return FALSE;
//
//	AgpdItemRuneTemplate	*pcsItemRuneTemplate	= GetItemRuneTemplate(lRuneTID);
//	if (!pcsItemRuneTemplate)
//		return FALSE;
//
//	if (!CheckAddRuneAttribute(pcsItem, lRuneTID, lRuneAttrLevel))
//		return FALSE;
//
//	// 1. rune template�� rune level�� �´� factor�� Item factor�� ADD �Ѵ�.
//	if (m_pagpmFactors)
//	{
//		m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor,
//								   &pcsItemRuneTemplate->m_csFactorAttrPoint[lRuneAttrLevel],
//								   TRUE,
//								   FALSE,
//								   TRUE);
//
//		m_pagpmFactors->CalcFactor(&pcsItem->m_csFactor,
//								   &pcsItemRuneTemplate->m_csFactorAttrPercent[lRuneAttrLevel],
//								   TRUE,
//								   FALSE,
//								   TRUE);
//	}
//
//	// 2. ���� �����丮�� �߰��Ѵ�.
//	return AddConvertHistory(pcsItem, &pcsItemRuneTemplate->m_csFactorAttrPoint[lRuneAttrLevel], &pcsItemRuneTemplate->m_csFactorAttrPercent[lRuneAttrLevel], FALSE, lTID);
//}
//*/
//
//BOOL AgpmItem::CheckAddRuneAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute)
//{
//	if (!pcsItem ||
//		AGPMITEM_RUNE_ATTR_NONE >= eRuneAttribute ||
//		eRuneAttribute >= AGPMITEM_RUNE_ATTR_MAX ||
//		!pcsItem->m_pcsItemTemplate)
//		return FALSE;
//
//	// ���� ���������� �˻��Ѵ�.
//	if (IsEgoItem(pcsItem))
//		return FALSE;
//
//	// ������ �ѹ��̶� �� ���������� �˻��Ѵ�.
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//			return FALSE;
//	}
//
//	//	�̹� ������ �� �ߴ��� �˻�.
//	if (IsConvertHistoryFull(pcsItem))
//		return FALSE;
//
//	switch (((AgpdItemTemplateEquip *) pcsItem->m_pcsItemTemplate)->m_nKind) {
//	case AGPMITEM_EQUIP_KIND_WEAPON:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_WEAPON)
//				return FALSE;
//
//			if (!(g_alRuneAttributeType[eRuneAttribute] & AGPMITEM_EQUIP_KIND_WEAPON))
//				return FALSE;
//		}
//		break;
//
//	case AGPMITEM_EQUIP_KIND_ARMOUR:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_ARMOUR)
//				return FALSE;
//
//			if (!(g_alRuneAttributeType[eRuneAttribute] & AGPMITEM_EQUIP_KIND_ARMOUR))
//				return FALSE;
//		}
//	case AGPMITEM_EQUIP_KIND_SHIELD:
//		{
//			if (pcsItem->m_stConvertHistory.lConvertLevel >= AGPMITEM_MAX_RUNE_ATTR_ARMOUR)
//				return FALSE;
//
//			if (!(g_alRuneAttributeType[eRuneAttribute] & AGPMITEM_EQUIP_KIND_SHIELD))
//				return FALSE;
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return TRUE;
//}
//
//BOOL AgpmItem::AddRuneAttribute(AgpdItem *pcsItem, AgpmItemRuneAttribute eRuneAttribute, INT32 lAttributeValue)
//{
//	if (!pcsItem ||
//		AGPMITEM_RUNE_ATTR_NONE >= eRuneAttribute ||
//		eRuneAttribute >= AGPMITEM_RUNE_ATTR_MAX ||
//		lAttributeValue <= 0)
//		return FALSE;
//
//	if (!CheckAddRuneAttribute(pcsItem, eRuneAttribute))
//		return FALSE;
//
//	return AddConvertHistory(pcsItem, eRuneAttribute, lAttributeValue);
//}
//
//BOOL AgpmItem::SetRuneAttributeValue(AgpdFactor *pcsFactor, AgpmItemRuneAttribute eRuneAttribute, FLOAT fAttributeValue)
//{
//	if (!pcsFactor ||
//		AGPMITEM_RUNE_ATTR_NONE >= eRuneAttribute ||
//		eRuneAttribute >= AGPMITEM_RUNE_ATTR_MAX ||
//		fAttributeValue <= 0.0f)
//		return FALSE;
//
//	switch (eRuneAttribute) {
//	case AGPMITEM_RUNE_ATTR_ADD_DAMAGE:
//		{
//			BOOL	bRetval	=  m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MIN, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
//			bRetval			&= m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_DAMAGE, AGPD_FACTORS_DAMAGE_TYPE_MAX, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
//			return	bRetval;
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_AR:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_AR);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_BLOCK_RATE:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_AC:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_MAX_HP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_ADD_MAX_HP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_MAX_MP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_ADD_MAX_MP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_MAX_SP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_ADD_MAX_SP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_RECOVERY_HP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE, AGPD_FACTORS_CHARPOINT_RECOVERY_RATE_TYPE_HP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_RECOVERY_MP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE, AGPD_FACTORS_CHARPOINT_RECOVERY_RATE_TYPE_MP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_RECOVERY_SP:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_CHAR_POINT_RECOVERY_RATE, AGPD_FACTORS_CHARPOINT_RECOVERY_RATE_TYPE_SP);
//		}
//		break;
//
//	case AGPMITEM_RUNE_ATTR_ADD_ATTACK_SPEED:
//		{
//			return m_pagpmFactors->SetValue(pcsFactor, fAttributeValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
//		}
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	return TRUE;
//}
//
//INT32 AgpmItem::GetNumRuneConvert(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return 0;
//
//	INT32	lNumRuneConvert	= 0;
//
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (!pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//			++lNumRuneConvert;
//	}
//
//	return lNumRuneConvert;
//}
//
//INT32 AgpmItem::GetNumSpiritStoneConvert(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return 0;
//
//	INT32	lNumSpiritStoneConvert	= 0;
//
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (pcsItem->m_stConvertHistory.bUseSpiritStone[i])
//			++lNumSpiritStoneConvert;
//	}
//
//	return lNumSpiritStoneConvert;
//}
//
//PVOID AgpmItem::MakePacketItemConvertAskReallyConvert(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pcsSpiritStone || !pnPacketLength)
//		return NULL;
//
//	INT8	cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_ASK_REALLY_CONVERT;
//	PVOID	pvPacketConvert		= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															   &cConvertOperation,
//															   &pcsSpiritStone->m_lID,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL,
//															   NULL);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemConvertRequest(AgpdItem *pcsItem, AgpdItem *pcsSpiritStone, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pcsSpiritStone || !pnPacketLength)
//		return NULL;
//
//	INT8	cConvertOperation = AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST;
//	PVOID	pvPacketConvert	= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															&pcsSpiritStone->m_lID,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemConvertResult(AgpdItem *pcsItem, AgpmItemConvertResult eResult, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pnPacketLength)
//		return NULL;
//
//	INT8	cConvertOperation;
//
//	switch (eResult) {
//	case AGPMITEM_CONVERT_FAIL:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_FAIL;
//		break;
//
//	case AGPMITEM_CONVERT_DESTROY_ATTRIBUTE:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ATTRIBUTE;
//		break;
//
//	case AGPMITEM_CONVERT_DESTROY_ITEM:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ITEM;
//		break;
//
//	case AGPMITEM_CONVERT_SUCCESS:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_SUCCESS;
//		break;
//
//	case AGPMITEM_CONVERT_DIFFER_RANK:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_RANK;
//		break;
//
//	case AGPMITEM_CONVERT_DIFFER_TYPE:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_TYPE;
//		break;
//
//	case AGPMITEM_CONVERT_FULL:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_FULL;
//		break;
//
//	case AGPMITEM_CONVERT_NOT_EQUIP_ITEM:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_NOT_EQUIP_ITEM;
//		break;
//
//	case AGPMITEM_CONVERT_EGO_ITEM:
//		cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_EGO_ITEM;
//		break;
//
//	default:
//		return FALSE;
//		break;
//	}
//
//	PVOID	pvPacketConvert	= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemConvertRequestHistory(AgpdItem *pcsItem, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pnPacketLength)
//		return NULL;
//
//	INT8	cConvertOperation = AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST_HISTORY;
//	
//	PVOID	pvPacketConvert	= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemConvertHistory(AgpdItem *pcsItem, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pnPacketLength)
//		return NULL;
//
//	PVOID	pvPacketConvert	= MakePacketItemConvertOnly(pcsItem, pnPacketLength);
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemAddConvertHistory(AgpdItem *pcsItem, INT16 *pnPacketLength, BOOL bIsMakeRestrictPacket)
//{
//	if (!pcsItem || !pnPacketLength)
//		return NULL;
//
//	if (pcsItem->m_stConvertHistory.lConvertLevel < 1)
//		return NULL;
//
//	PVOID	pvPacketHistoryFactorPoint		= m_pagpmFactors->MakePacketFactors(&pcsItem->m_stConvertHistory.csFactorHistory[pcsItem->m_stConvertHistory.lConvertLevel - 1]);
//	PVOID	pvPacketHistoryFactorPercent	= m_pagpmFactors->MakePacketFactors(&pcsItem->m_stConvertHistory.csFactorPercentHistory[pcsItem->m_stConvertHistory.lConvertLevel - 1]);
//
//	if (!pvPacketHistoryFactorPoint && !pvPacketHistoryFactorPercent)
//		return NULL;
//
//	BOOL	bUseSpiritStone	= pcsItem->m_stConvertHistory.bUseSpiritStone[pcsItem->m_stConvertHistory.lConvertLevel - 1];
//	INT32	lSpiritStoneTID	= pcsItem->m_stConvertHistory.lTID[pcsItem->m_stConvertHistory.lConvertLevel - 1];
//	INT8	cRuneAttribute	= (INT8) pcsItem->m_stConvertHistory.eRuneAttribute[pcsItem->m_stConvertHistory.lConvertLevel - 1];
//	INT8	cRuneValue		= (INT8) pcsItem->m_stConvertHistory.lRuneAttributeValue[pcsItem->m_stConvertHistory.lConvertLevel - 1];
//
//	INT8	cConvertOperation = AGPMITEM_PACKET_CONVERT_OPERATION_ADD_HISTORY;
//	PVOID	pvPacketConvert	= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															NULL,
//															NULL,
//															&pcsItem->m_stConvertHistory.fConvertConstant,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															&bUseSpiritStone,
//															(bUseSpiritStone) ? &lSpiritStoneTID : NULL,
//															(!bUseSpiritStone) ? &cRuneAttribute : NULL,
//															(!bUseSpiritStone) ? &cRuneValue : NULL,
//															pvPacketHistoryFactorPoint,
//															pvPacketHistoryFactorPercent);
//
//	if (pvPacketHistoryFactorPoint)
//		m_csPacket.FreePacket(pvPacketHistoryFactorPoint);
//	if (pvPacketHistoryFactorPercent)
//		m_csPacket.FreePacket(pvPacketHistoryFactorPercent);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	PVOID	pvPacketRestrictFactor	= NULL;
//	if (bIsMakeRestrictPacket)
//		pvPacketRestrictFactor	= m_pagpmFactors->MakePacketLevel(&pcsItem->m_csRestrictFactor);
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												pvPacketRestrictFactor,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	if (pvPacketRestrictFactor)
//		m_csPacket.FreePacket(pvPacketRestrictFactor);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemRemoveConvertHistory(AgpdItem *pcsItem, INT32 lIndex, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pnPacketLength)
//		return NULL;
//
//	INT8	cIndex				= (INT8) lIndex;
//	INT8	cConvertOperation	= AGPMITEM_PACKET_CONVERT_OPERATION_REMOVE_HISTORY;
//	PVOID	pvPacketConvert		= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															NULL,
//															&cIndex,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL);
//
//	if (!pvPacketConvert)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_CONVERT_ITEM;
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												&pcsItem->m_lID,
//												NULL,
//												&pcsItem->m_ulCID,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketConvert,
//												NULL,
//												NULL);
//
//	m_csPacketConvert.FreePacket(pvPacketConvert);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemConvertOnly(AgpdItem *pcsItem, INT16 *pnPacketLength)
//{
//	if (!pcsItem || !pnPacketLength)
//		NULL;
//
//	INT8	caIsUseSpriitStone[AGPMITEM_MAX_CONVERT];
//	INT8	caRuneAttribute[AGPMITEM_MAX_CONVERT];
//	INT8	caRuneAttributeValue[AGPMITEM_MAX_CONVERT];
//
//	ZeroMemory(caIsUseSpriitStone, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(caRuneAttribute, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(caRuneAttributeValue, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//
//	PVOID	pvPacketHistoryFactorPoint[AGPMITEM_MAX_CONVERT];
//	PVOID	pvPacketHistoryFactorPercent[AGPMITEM_MAX_CONVERT];
//
//	ZeroMemory(pvPacketHistoryFactorPoint, sizeof(PVOID) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(pvPacketHistoryFactorPercent, sizeof(PVOID) * AGPMITEM_MAX_CONVERT);
//
//	for (int i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++ i)
//	{
//		caIsUseSpriitStone[i]	= (INT8) pcsItem->m_stConvertHistory.bUseSpiritStone[i];
//		caRuneAttribute[i]	= (INT8) pcsItem->m_stConvertHistory.eRuneAttribute[i];
//		caRuneAttributeValue[i] = (INT8) pcsItem->m_stConvertHistory.lRuneAttributeValue[i];
//
//		pvPacketHistoryFactorPoint[i] = m_pagpmFactors->MakePacketFactors(pcsItem->m_stConvertHistory.csFactorHistory + i);
//		pvPacketHistoryFactorPercent[i] = m_pagpmFactors->MakePacketFactors(pcsItem->m_stConvertHistory.csFactorPercentHistory + i);
//	}
//
//	INT8	cConvertOperation = AGPMITEM_PACKET_CONVERT_OPERATION_HISTORY;
//	PVOID	pvPacketConvert	= m_csPacketConvert.MakePacket(FALSE, NULL, 0,
//															&cConvertOperation,
//															NULL,
//															&pcsItem->m_stConvertHistory.lConvertLevel,
//															&pcsItem->m_stConvertHistory.fConvertConstant,
//															&caIsUseSpriitStone,
//															&pcsItem->m_stConvertHistory.lTID,
//															&caRuneAttribute,
//															&caRuneAttributeValue,
//															pvPacketHistoryFactorPoint,
//															pvPacketHistoryFactorPercent,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL,
//															NULL);
//
//	for (i = 0; i < pcsItem->m_stConvertHistory.lConvertLevel; ++i)
//	{
//		if (pvPacketHistoryFactorPoint[i])
//			m_csPacket.FreePacket(pvPacketHistoryFactorPoint[i]);
//
//		if (pvPacketHistoryFactorPercent[i])
//			m_csPacket.FreePacket(pvPacketHistoryFactorPercent[i]);
//	}
//
//	return pvPacketConvert;
//}
//
//BOOL AgpmItem::OnOperationConvertItem(BOOL bReceivedFromServer, INT8 cStatus, INT32 lIID, PVOID pConvert, PVOID pRestrictFactor)
//{
//	if (lIID == AP_INVALID_IID || !pConvert)
//		return FALSE;
//
//	AgpdItem	*pcsItem = GetItem(lIID);
//	if (!pcsItem)
//		return FALSE;
//
//	if (bReceivedFromServer && pRestrictFactor && m_pagpmFactors)
//		m_pagpmFactors->ReflectPacket(&pcsItem->m_csRestrictFactor, pRestrictFactor, 0);
//
//	INT8	cConvertOperation	= (-1);
//	INT32	lSpiritStone		= AP_INVALID_IID;
//	INT8	cConvertLevel		= 0;
//	FLOAT	fConvertConstant	= 0;
//
//	INT8	cUseSpiritStone[AGPMITEM_MAX_CONVERT];
//	INT32	lTID[AGPMITEM_MAX_CONVERT];
//	INT8	caRuneAttribute[AGPMITEM_MAX_CONVERT];
//	INT8	caRuneAttributeValue[AGPMITEM_MAX_CONVERT];
//
//	ZeroMemory(cUseSpiritStone, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(lTID, sizeof(INT32) * AGPMITEM_MAX_CONVERT);
//
//	ZeroMemory(caRuneAttribute, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(caRuneAttributeValue, sizeof(INT8) * AGPMITEM_MAX_CONVERT);
//
//	BOOL	bIsUseSpiritStone	= FALSE;
//	INT32	lSpiritStoneTID		= AP_INVALID_IID;
//	INT8	cRuneAttribute		= AGPMITEM_RUNE_ATTR_NONE;
//	INT8	cRuneValue			= (-1);
//
//	PVOID	pvPacketFactorPoint[AGPMITEM_MAX_CONVERT];
//	PVOID	pvPacketFactorPercent[AGPMITEM_MAX_CONVERT];
//
//	ZeroMemory(pvPacketFactorPoint, sizeof(PVOID) * AGPMITEM_MAX_CONVERT);
//	ZeroMemory(pvPacketFactorPercent, sizeof(PVOID) * AGPMITEM_MAX_CONVERT);
//
//	PVOID	pvPacketAddFactorPoint		= NULL;
//	PVOID	pvPacketAddFactorPercent	= NULL;
//
//	m_csPacketConvert.GetField(FALSE, pConvert, 0,
//								&cConvertOperation,
//								&lSpiritStone,
//								&cConvertLevel,
//								&fConvertConstant,
//								&cUseSpiritStone,
//								&lTID,
//								&caRuneAttribute,
//								&caRuneAttributeValue,
//								&pvPacketFactorPoint,
//								&pvPacketFactorPercent,
//								&bIsUseSpiritStone,
//								&lSpiritStoneTID,
//								&cRuneAttribute,
//								&cRuneValue,
//								&pvPacketAddFactorPoint,
//								&pvPacketAddFactorPercent);
//
//	BOOL	bRetval = TRUE;
//
//	switch (cConvertOperation) {
//	case AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST:
//		{
//			AgpdItem	*pcsSpiritStone = GetItem(lSpiritStone);
//			if (!pcsSpiritStone)
//				bRetval = FALSE;
//
//			if (bRetval)
//			{
//				bRetval = ConvertItem(pcsItem, pcsSpiritStone);
//			}
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_ASK_REALLY_CONVERT:
//		{
//			AgpdItem	*pcsSpiritStone = GetItem(lSpiritStone);
//			if (!pcsSpiritStone)
//				bRetval = FALSE;
//
//			if (bRetval)
//			{
//				bRetval = EnumCallback(ITEM_CB_ID_ASK_REALLY_CONVERT_ITEM, pcsItem, pcsSpiritStone);
//			}
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_SUCCESS:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_SUCCESS;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_FAIL:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_FAIL;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ATTRIBUTE:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_DESTROY_ATTRIBUTE;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_DESTROY_ITEM:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_DESTROY_ITEM;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_RANK:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_DIFFER_RANK;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_DIFFER_TYPE:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_DIFFER_TYPE;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_FULL:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_FULL;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_NOT_EQUIP_ITEM:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_NOT_EQUIP_ITEM;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_EGO_ITEM:
//		{
//			INT32	lConvertResult	= AGPMITEM_CONVERT_EGO_ITEM;
//
//			// Buffer Array 2->3 ���� ����. - 2004.04.06. steeple
//			PVOID	pvBuffer[3];
//			pvBuffer[0]	= &lConvertResult;
//			pvBuffer[1]	= pvBuffer[2] = NULL;
//			bRetval = EnumCallback(ITEM_CB_ID_CONVERT_ITEM, pcsItem, pvBuffer);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_REQUEST_HISTORY:
//		{
//			bRetval = EnumCallback(ITEM_CB_ID_SEND_CONVERT_HISTORY, pcsItem, NULL);
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_HISTORY:
//		{
//			if (bReceivedFromServer)
//			{
//				bRetval = UpdateConvertHistory(pcsItem, cConvertLevel, fConvertConstant, cUseSpiritStone, lTID, caRuneAttribute, caRuneAttributeValue, pvPacketFactorPoint, pvPacketFactorPercent);
//
//				ProcessRefreshCount(pcsItem->m_pcsCharacter, pcsItem);
//			}
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_ADD_HISTORY:
//		{
//			if (bReceivedFromServer)
//			{
//				if (bIsUseSpiritStone)
//					bRetval = AddConvertHistoryPacket(pcsItem, fConvertConstant, pvPacketAddFactorPoint, pvPacketAddFactorPercent, TRUE, lSpiritStoneTID);
//				else
//					bRetval = AddConvertHistory(pcsItem, (AgpmItemRuneAttribute) cRuneAttribute, (INT32) cRuneValue);
//
//				ProcessRefreshCount(pcsItem->m_pcsCharacter, pcsItem);
//			}
//		}
//		break;
//
//	case AGPMITEM_PACKET_CONVERT_OPERATION_REMOVE_HISTORY:
//		{
//			if (bReceivedFromServer)
//			{
//				bRetval = RemoveConvertHistory(pcsItem, (INT32) cConvertLevel);
//
//				ProcessRefreshCount(pcsItem->m_pcsCharacter, pcsItem);
//			}
//		}
//		break;
//
//	default:
//		bRetval = FALSE;
//		break;
//	}
//
//	return bRetval;
//}
//
//BOOL AgpmItem::SetCallbackUpdateConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_UPDATE_CONVERT_HISTORY, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackAddConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_ADD_CONVERT_HISTORY, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackRemoveConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_REMOVE_CONVERT_HISTORY, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackSendConvertHistory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_SEND_CONVERT_HISTORY, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackUpdateEgoExp(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_UPDATE_EGO_EXP, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackUpdateEgoLevel(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_UPDATE_EGO_LEVEL, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackPutSoulIntoCube(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_PUT_SOUL_INTO_CUBE, pfCallback, pClass);
//}
//
//BOOL AgpmItem::SetCallbackUseSoulCube(ApModuleDefaultCallBack pfCallback, PVOID pClass)
//{
//	return SetCallback(ITEM_CB_ID_USE_SOULCUBE, pfCallback, pClass);
//}
//
//BOOL AgpmItem::IsNormalItem(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return FALSE;
//
//	// magic, convert, ego, legend item ���� �� �ϰ͵� ���� normal �������� �˻��Ѵ�.
//	// normal item �̸� TRUE��, �ƴϸ� FALSE�� �����Ѵ�.
//
//	// magic item ���� �˻��Ѵ�.
//	//
//	//
//
//	// ������ �������� �˻��Ѵ�.
//	if (pcsItem->m_stConvertHistory.lConvertLevel > 0)
//		return FALSE;
//
//	// ego item ���� �˻��Ѵ�.
//	if (IsEgoItem(pcsItem))
//		return FALSE;
//
//	// legend item ���� �˻��Ѵ�.
//	//
//	//
//
//	return TRUE;
//}
//
//BOOL AgpmItem::PutSoulIntoCube(AgpdItem *pcsItem, AgpdCharacter *pcsCharacter)
//{
//	if (!pcsItem || !pcsCharacter)
//		return FALSE;
//
//	if (!pcsItem->m_pcsCharacter)
//	{
//		if (pcsItem->m_ulCID == AP_INVALID_CID)
//			return FALSE;
//
//		pcsItem->m_pcsCharacter = m_pagpmCharacter->GetCharacter(pcsItem->m_ulCID);
//		if (pcsItem->m_pcsCharacter)
//			return FALSE;
//	}
//
//	// pcsItem�� soul cube ���� �˻��Ѵ�.
//	if (!(((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_USABLE &&
//		((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SOUL_CUBE))
//		return FALSE;
//
//	// pcsCharacter�� �׾ ��ȥ���� �Ȱ��� ���캻��.
//	//
//	//		���� ��ȥ���� �����ؾ��ϳ�... ��...
//	//
//	//
//
//	// �ϴ� ��ȥ�̶�� ġ��
//
//	// pcsCharacter�� factor�� �ʿ��� �κе��� pcsItem factor�� �����Ѵ�.
//	INT32	lSoulLevel	= (-1);
//	INT32	lSoulRace	= (-1);
//
//	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lSoulLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
//	m_pagpmFactors->GetValue(&pcsCharacter->m_csFactor, &lSoulRace, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
//
//	if (lSoulLevel == (-1) || lSoulRace == (-1))
//		return FALSE;
//
//	INT32	lSoulCubeLevel	= (-1);
//
//	m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lSoulCubeLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOULCUBE_LEVEL);
//
//	if (lSoulCubeLevel == (-1))
//		return FALSE;
//
//	// Soul Cube�� 1-5�ܰ谡 �����ϸ� ���� �ڱ� �ܰ�*20���������� ��ȥ�� ���� �� �ִ�. 
//	if (lSoulCubeLevel * 20 < lSoulLevel)
//		return FALSE;
//
//	// ��� �˻簡 �����ٸ� ��ȥ�� �����Ѵ�.
//	m_pagpmFactors->SetValue(&pcsItem->m_csFactor, lSoulLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_LEVEL);
//	m_pagpmFactors->SetValue(&pcsItem->m_csFactor, lSoulRace, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_RACE);
//
//	// ��� �۾��� ������. TRUE�� �����Ѵ�.
//
//	return TRUE;
//}
//
//BOOL AgpmItem::UseSoulCube(AgpdItem *pcsSoulCube, AgpdItem *pcsTargetItem)
//{
//	if (!pcsSoulCube || !pcsTargetItem || !pcsTargetItem->m_pcsCharacter)
//		return FALSE;
//
//	// pcsSoulCube�� soul cube ���� ��ȥ�� �����ϰ� �ִ��� �˻��Ѵ�.
//	if (((AgpdItemTemplate *) pcsSoulCube)->m_nType != AGPMITEM_TYPE_USABLE ||
//		((AgpdItemTemplateUsable *) pcsSoulCube)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_SOUL_CUBE)
//		return FALSE;
//
//	INT32	lSoulLevel	= (-1);
//	INT32	lSoulRace	= (-1);
//
//	m_pagpmFactors->GetValue(&pcsSoulCube->m_csFactor, &lSoulLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_LEVEL);
//	m_pagpmFactors->GetValue(&pcsSoulCube->m_csFactor, &lSoulRace, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_RACE);
//
//	if (lSoulLevel < 1 ||
//		lSoulRace < 1)
//		return FALSE;
//
//	// pcsTargetItem�� ��������, �ٸ� ������ �̷���� ������,, ��ȥ���� �ٸ��� �ִ� ������ �˻��Ѵ�.
//
//	// ����� weapon�� ����������� ���� �� �ִ�.
//	if (((AgpdItemTemplate *) pcsTargetItem)->m_nType != AGPMITEM_TYPE_EQUIP ||
//		((AgpdItemTemplateEquip *) pcsTargetItem)->m_nKind != AGPMITEM_EQUIP_KIND_WEAPON)
//		return FALSE;
//
//	if (!IsNormalItem(pcsTargetItem))
//	{
//		return FALSE;
//	}
//
//	INT32	lRestrictLevel = (-1);
//	m_pagpmFactors->GetValue(&pcsTargetItem->m_csRestrictFactor, &lRestrictLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
//	if (lRestrictLevel < 1)
//		return FALSE;
//
//	// �˻� ��� ������� �Դٸ� ����������� ���� �� �ִ� ����̴�.
//
//	// ����������� �����.
//	//		1. ������ �����Ѵ�.
//	//		2. ��ȥ�� level, race�� �����ϰ� ���� �������� ������ ��� ���� ������ �����Ѵ�. (exp�� 0�̴�)
//
//	strncpy(pcsTargetItem->m_szSoulMasterID, pcsTargetItem->m_pcsCharacter->m_szID, AGPACHARACTER_MAX_ID_STRING);
//
//	INT32	lEgoExp = 0;
//
//	INT32	lMaxEgoLevel = lRestrictLevel + 20;
//	if (lMaxEgoLevel > AGPMCHAR_MAX_LEVEL)
//		lMaxEgoLevel = AGPMCHAR_MAX_LEVEL;
//
//	m_pagpmFactors->SetValue(&pcsTargetItem->m_csFactor, lSoulLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_LEVEL);
//	m_pagpmFactors->SetValue(&pcsTargetItem->m_csFactor, lSoulRace, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_SOUL_RACE);
//
//	m_pagpmFactors->SetValue(&pcsTargetItem->m_csFactor, lRestrictLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_LEVEL);
//	m_pagpmFactors->SetValue(&pcsTargetItem->m_csFactor, lMaxEgoLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_MAXLEVEL);
//	m_pagpmFactors->SetValue(&pcsTargetItem->m_csFactor, lEgoExp, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_EXP);
//
//	return TRUE;
//}
//
//INT32 AgpmItem::CalcEgoExp(AgpdCharacter *pcsCharacter, INT32 lTotalExp)
//{
//	if (!pcsCharacter || lTotalExp < 2)
//		return lTotalExp;
//
//	INT32	lEgoExp = 0;
//	INT32	lNewEgoExp = 0;
//
//	// pcsCharacter�� lTotalExp�� ����ġ�� �����. �����ϰ� �ִ� ���Ⱑ ��������̸� 40%�� ������ۿ� �ش�.
//	// ������ۿ� �ְ� ���� ����ġ�� �����Ѵ�.
//
//	AgpdItem		*pcsItem  = NULL;
//	AgpdGridItem	*pcsAgpdGridItem;
//
//	pcsAgpdGridItem = GetEquipItem( pcsCharacter, AGPMITEM_PART_HAND_RIGHT );
//
//	if( pcsAgpdGridItem != NULL )
//	{
//		pcsItem = GetItem( pcsAgpdGridItem->m_lItemID );
//	}
//
//	if (pcsItem)
//	{
//		if (IsEgoItem(pcsItem))
//		{
//			// ���� ������ �ְ���� �����ߴ��� üũ�Ѵ�.
//			INT32	lEgoLevel		= 0;
//			INT32	lEgoMaxLevel	= 0;
//
//			m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lEgoLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_LEVEL);
//			m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lEgoMaxLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_MAXLEVEL);
//
//			if (lEgoMaxLevel == lEgoLevel)
//				return lTotalExp;
//
//			// pcsCharacter�� ����,����ġ���� ��������� ����,����ġ�� �� ������ �˻��Ѵ�.
//			// �� ���� �ȿ÷��ش�.
//			INT32	lCharLevel	= 0;
//			INT32	lCharExp	= 0;
//
//			lCharLevel = m_pagpmFactors->GetLevel(&pcsCharacter->m_csFactor);
//			if (lCharLevel < 1 || lCharLevel < lEgoLevel)
//				return lTotalExp;
//
//			lCharExp = m_pagpmCharacter->GetExp(pcsCharacter);
//
//			m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lNewEgoExp, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_EXP);
//
//			if (lCharExp < lNewEgoExp)
//				return lTotalExp;
//
//			lEgoExp = (INT32) (lTotalExp * 0.4);
//		}
//	}
//	else
//		return lTotalExp;
//
//	// ���� ���ۿ� �� ����ġ�� 1���� ũ�ٸ� ����ġ�� �ݿ��ϰ� �������� �ߴ��� �˻��Ѵ�.
//	if (lEgoExp > 0)
//	{
//		// ����ġ�� factor�� ���Ѵ�.
//		lNewEgoExp += lEgoExp;
//		m_pagpmFactors->SetValue(&pcsItem->m_csFactor, lNewEgoExp, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_EXP);
//
//		// ������ �ö����� ���� �ݿ��Ѵ�.
//		BOOL	bIsLevelUp = ProcessEgoLevelUp(pcsItem);
//
//		EnumCallback(ITEM_CB_ID_UPDATE_EGO_EXP, pcsItem, &bIsLevelUp);
//	}
//
//	return (lTotalExp - lEgoExp);
//}
//
//BOOL AgpmItem::ProcessEgoLevelUp(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return FALSE;
//
//	// pcsItem�� (�� �Լ������� ���̻� ����������� �˻��� �ʿ䰡 ����) �������� �ߴ��� �˻��Ѵ�.
//	INT32	lEgoLevel	= 0;
//	INT32	lEgoExp		= 0;
//
//	m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lEgoLevel, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_LEVEL);
//	m_pagpmFactors->GetValue(&pcsItem->m_csFactor, &lEgoExp, AGPD_FACTORS_TYPE_ITEM, AGPD_FACTORS_ITEM_TYPE_EGO_EXP);
//
//	INT32	lLevelUpExp = m_pagpmCharacter->GetLevelUpExp(lEgoLevel);
//
//	if (lLevelUpExp > lEgoExp)
//		return FALSE;
//
//	// ������ �ߴٸ� ó�����ش�.
//	//		1. ���밡�� ������ ������Ʈ�Ѵ�. (+1 �Ѵ�)
//	//		2. ���� �������� ����Ʈ���͸� �ٽ� ����Ѵ�.
//
//	INT32	lRestrictLevel = 0;
//	m_pagpmFactors->GetValue(&pcsItem->m_csRestrictFactor, &lRestrictLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
//
//	++lRestrictLevel;
//
//	m_pagpmFactors->SetValue(&pcsItem->m_csRestrictFactor, lRestrictLevel, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_LEVEL);
//
//	EnumCallback(ITEM_CB_ID_UPDATE_EGO_LEVEL, pcsItem, NULL);
//
//	return TRUE;
//}
//
//BOOL AgpmItem::IsEgoItem(AgpdItem *pcsItem)
//{
//	if (!pcsItem)
//		return FALSE;
//
//	if (strlen(pcsItem->m_szSoulMasterID) > 0)
//		return TRUE;
//
//	return FALSE;
//}
//
//PVOID AgpmItem::MakePacketItemPutSoulIntoCube(INT32	lSoulCubeID, INT32 lSoulCID, INT16 *pnPacketLength)
//{
//	if (lSoulCubeID == AP_INVALID_IID ||
//		lSoulCID == AP_INVALID_CID ||
//		!pnPacketLength)
//		return NULL;
//
//	INT16	nPacketLength = 0;
//	INT8	cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_INTO_CUBE;
//	PVOID	pvPacketEgo	= m_csPacketEgo.MakePacket(FALSE, &nPacketLength, 0,
//												&cEgoOperation,
//												&lSoulCubeID,
//												&lSoulCID,
//												NULL);
//
//	if (!pvPacketEgo)
//		return NULL;
//
//	INT8	cOperation = AGPMITEM_PACKET_OPERATION_EGO_ITEM;
//
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketEgo);
//
//	m_csPacketEgo.FreePacket(pvPacketEgo);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemPutSoulResult(INT32 lSoulCubeID, INT16 nResult, INT16 *pnPacketLength)
//{
//	if (lSoulCubeID == AP_INVALID_IID ||
//		pnPacketLength == NULL)
//		return NULL;
//
//	INT8	cEgoOperation;
//	if (nResult)
//		cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_SUCCESS;
//	else
//		cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_PUT_SOUL_FAIL;
//
//	PVOID	pvPacketEgo = m_csPacketEgo.MakePacket(FALSE, pnPacketLength, 0,
//												&cEgoOperation,
//												&lSoulCubeID,
//												NULL,
//												NULL);
//
//	if (!pvPacketEgo)
//		return NULL;
//
//	INT8	cOperation	= AGPMITEM_PACKET_OPERATION_EGO_ITEM;
//
//	PVOID	pvPacket = m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketEgo);
//
//	m_csPacketEgo.FreePacket(pvPacketEgo);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemUseSoulCube(INT32 lSoulCubeID, INT32 lTargetIID, INT16 *pnPacketLength)
//{
//	if (lSoulCubeID == AP_INVALID_IID ||
//		lTargetIID == AP_INVALID_IID ||
//		!pnPacketLength)
//		return NULL;
//
//	INT16	nPacketLength = 0;
//	INT8	cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE;
//	PVOID	pvPacketEgo	= m_csPacketEgo.MakePacket(FALSE, &nPacketLength, 0,
//												&cEgoOperation,
//												&lSoulCubeID,
//												NULL,
//												&lTargetIID);
//
//	if (!pvPacketEgo)
//		return NULL;
//
//	INT8	cOperation = AGPMITEM_PACKET_OPERATION_EGO_ITEM;
//
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketEgo);
//
//	m_csPacketEgo.FreePacket(pvPacketEgo);
//
//	return pvPacket;
//}
//
//PVOID AgpmItem::MakePacketItemUseSoulCubeResult(INT32 lSoulCubeID, INT16 nResult, INT16 *pnPacketLength)
//{
//	if (lSoulCubeID == AP_INVALID_IID ||
//		!pnPacketLength)
//		return NULL;
//
//	INT16	nPacketLength = 0;
//	INT8	cEgoOperation;
//
//	if (nResult)
//		cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE_SUCCESS;
//	else
//		cEgoOperation = AGPMITEM_PACKET_EGO_OPERATION_USE_SOULCUBE_FAIL;
//
//	PVOID	pvPacketEgo	= m_csPacketEgo.MakePacket(FALSE, &nPacketLength, 0,
//												&cEgoOperation,
//												&lSoulCubeID,
//												NULL,
//												NULL);
//
//	if (!pvPacketEgo)
//		return NULL;
//
//	INT8	cOperation = AGPMITEM_PACKET_OPERATION_EGO_ITEM;
//
//	PVOID	pvPacket	= m_csPacket.MakePacket(TRUE, pnPacketLength, AGPMITEM_PACKET_TYPE,
//												&cOperation,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												NULL,
//												pvPacketEgo);
//
//	m_csPacketEgo.FreePacket(pvPacketEgo);
//
//	return pvPacket;
//}
//
//BOOL AgpmItem::OnOperationPutSoulIntoCube(INT32 lIID, INT32 lTargetID)
//{
//	if (lIID ==	AP_INVALID_IID ||
//		lTargetID == AP_INVALID_CID)
//		return FALSE;
//
//	//AgpdItem	*pcsSoulCube	= GetItemLock(lIID);
//	AgpdItem	*pcsSoulCube	= GetItem(lIID);
//	if (!pcsSoulCube)
//		return FALSE;
//
//	AgpdCharacter	*pcsSoulCubeOwner = m_pagpmCharacter->GetCharacter(pcsSoulCube->m_ulCID);
//	if (!pcsSoulCubeOwner)
//	{
//		return FALSE;
//	}
//
//	INT32	alCID[2];
//	alCID[0] = pcsSoulCubeOwner->m_lID;
//	alCID[1] = lTargetID;
//
//	AgpdCharacter	*pacsCharacter[2];
//	ZeroMemory(pacsCharacter, sizeof(AgpdCharacter *) * 2);
//
//	pcsSoulCubeOwner->m_Mutex.Release();
//
//	if (!m_pagpmCharacter->GetCharacterLock(alCID, 2, pacsCharacter))
//	{
//		pcsSoulCubeOwner->m_Mutex.WLock();
//		return FALSE;
//	}
//
//	AgpdCharacter	*pcsTargetChar	= NULL;
//
//	if (pcsSoulCube->m_ulCID == pacsCharacter[0]->m_lID)
//	{
//		pcsSoulCubeOwner	= pacsCharacter[0];
//		pcsTargetChar		= pacsCharacter[1];
//	}
//	else
//	{
//		pcsSoulCubeOwner	= pacsCharacter[1];
//		pcsTargetChar		= pacsCharacter[0];
//	}
//
//	BOOL	bProcessResult = PutSoulIntoCube(pcsSoulCube, pcsTargetChar);
//	
//	AgpmItemEgoResult	stEgoResult;
//	if (bProcessResult)
//		stEgoResult = AGPMITEM_EGO_PUT_SOUL_SUCCESS;
//	else
//		stEgoResult = AGPMITEM_EGO_PUT_SOUL_FAIL;
//
//	EnumCallback(ITEM_CB_ID_PUT_SOUL_INTO_CUBE, pcsSoulCube, &stEgoResult);
//
//	pcsTargetChar->m_Mutex.Release();
//	//pcsSoulCube->m_Mutex.Release();
//
//	return bProcessResult;
//}
//
//BOOL AgpmItem::OnOperationUseSoulCube(INT32 lIID, INT32 lTargetID)
//{
//	if (lIID ==	AP_INVALID_IID ||
//		lTargetID == AP_INVALID_IID)
//		return FALSE;
//
//	//AgpdItem	*pcsSoulCube	= GetItemLock(lIID);
//	AgpdItem	*pcsSoulCube	= GetItem(lIID);
//	if (!pcsSoulCube)
//		return FALSE;
//
//	//AgpdItem	*pcsTargetItem = GetItemLock(lTargetID);
//	AgpdItem	*pcsTargetItem = GetItem(lTargetID);
//	if (!pcsTargetItem)
//	{
//		//pcsSoulCube->m_Mutex.Release();
//		return FALSE;
//	}
//
//	// soul cube ���ΰ� target item ������ ���� ���̾�� �Ѵ�.
//	if (pcsTargetItem->m_ulCID != pcsSoulCube->m_ulCID)
//	{
//		//pcsTargetItem->m_Mutex.Release();
//		//pcsSoulCube->m_Mutex.Release();
//		return FALSE;
//	}
//
//	AgpdCharacter	*pcsSoulCubeOwner = m_pagpmCharacter->GetCharacter(pcsSoulCube->m_ulCID);
//	if (!pcsSoulCubeOwner)
//	{
//		//pcsTargetItem->m_Mutex.Release();
//		//pcsSoulCube->m_Mutex.Release();
//		return FALSE;
//	}
//
//	BOOL	bProcessResult = UseSoulCube(pcsSoulCube, pcsTargetItem);
//
//	AgpmItemEgoResult	stEgoResult;
//	if (bProcessResult)
//		stEgoResult = AGPMITEM_EGO_USE_SOULCUBE_SUCCESS;
//	else
//		stEgoResult = AGPMITEM_EGO_USE_SOULCUBE_FAIL;
//
//	EnumCallback(ITEM_CB_ID_USE_SOULCUBE, pcsSoulCube, &stEgoResult);
//
//	//pcsTargetItem->m_Mutex.Release();
//	//pcsSoulCube->m_Mutex.Release();
//
//	return bProcessResult;
//}
