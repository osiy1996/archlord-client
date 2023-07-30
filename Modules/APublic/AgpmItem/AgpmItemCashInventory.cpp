
#include <AgpmItem/AgpmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmCharacter/AgpmCharacter.h>

BOOL AgpmItem::SetCallbackAddItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_REMOVE_ITEM_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUpdateItemCashInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UI_UPDATE_CASH_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_USE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackUnUseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_UNUSE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackPauseCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_PAUSE_CASH_ITEM, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackGetPetTIDByItemFromSkill(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_GET_PET_TID_BY_ITEM_FROM_SKILL, pfCallback, pClass);
}

BOOL AgpmItem::IsAnyEmptyCashInventory(AgpdCharacter *pcsCharacter)
{
	// ����ִ� �׸��尡 �ִ��� ����.
	INT16	nLayer	= 0;
	INT16	nRow	= 0;
	INT16	nColumn	= 0;

	if (!m_pagpmGrid->GetEmptyGrid(GetCashInventoryGrid(pcsCharacter), &nLayer, &nRow, &nColumn, 1, 1))
		return FALSE;

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	ĳ�� �κ��丮�� �������� �߰��Ѵ�.
*/
BOOL AgpmItem::AddItemToCashInventory(INT32 lCID, AgpdItem *pcsItem )
{
	if (!lCID || !pcsItem) return FALSE;

	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToCashInventory(pcsCharacter, pcsItem );
}

/*
	2005.11.16. By SungHoon
	ĳ�� �κ��丮�� �������� �߰��Ѵ�.
*/
BOOL AgpmItem::AddItemToCashInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem )
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToCashInventory() Error (2) !!!\n");
		return FALSE;
	}
	AgpdItemGridResult eResult = AutoInsert(pcsItem, GetCashInventoryGrid(pcsCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		RemoveStatus(pcsItem, AGPDITEM_STATUS_CASH_INVENTORY);
		pcsItem->m_eStatus = AGPDITEM_STATUS_CASH_INVENTORY;

		ChangeItemOwner(pcsItem, pcsCharacter);
		EnumCallback(ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY, pcsItem, pcsCharacter);

		return TRUE;	
	}

	return FALSE;
}


/*
	2005.11.16. By SungHoon
	ĳ�� �������� ������ TID�� ����´�.
*/
AgpdGridItem *AgpmItem::GetCashItemByTID( INT32 lCID, INT32 lTID )
{
	AgpdCharacter *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if( !pcsCharacter ) return NULL;

	return GetCashItemByTID( pcsCharacter, lTID );
}

/*
	2005.11.16. By SungHoon
	ĳ�� �������� ������ TID�� ����´�.
*/
AgpdGridItem *AgpmItem::GetCashItemByTID( AgpdCharacter *pcsCharacter, INT32 lTID )
{
	if (!pcsCharacter || !lTID) return NULL;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar) return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem && pcsItem->m_pcsItemTemplate)
			{
				if (lTID == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

/*
	2005.12.02. By kevelon
	ĳ�� �������� eUsableType, lSubType�� �������� ������� ���� ó�� �������� �����Ѵ�.
*/
AgpdGridItem *AgpmItem::GetCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType)
{
	if (!pcsCharacter) return NULL;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar) return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem && pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplateUsable* pcsAgpdItemTemplateUsable = (AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate;
				if (eUsableType == pcsAgpdItemTemplateUsable->m_nUsableItemType
					&& lSubType == pcsAgpdItemTemplateUsable->m_nSubType)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

/*
	2005.12.02. By SungHoon
	ĳ�� �������� eUsableType, lSubType�� �������� ������� ���� ó�� �������� �����Ѵ�.
*/
AgpdGridItem *AgpmItem::GetUsingCashItemUsableByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eUsableType, INT32 lSubType)
{
	if (!pcsCharacter ) return NULL;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar) return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem &&
				(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	�����
				pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplateUsable* pcsAgpdItemTemplateUsable = (AgpdItemTemplateUsable*)pcsItem->m_pcsItemTemplate;
				if (eUsableType == pcsAgpdItemTemplateUsable->m_nUsableItemType
					&& lSubType == pcsAgpdItemTemplateUsable->m_nSubType)
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

// �ش� �������� Usable Type �� ���´�
AgpmItemUsableType	AgpmItem::GetUsableType( AgpdItem* pcsItem )
{
	AgpdItemTemplateUsable*		pcsAgpdItemTemplateUsable	=	static_cast< AgpdItemTemplateUsable* >(pcsItem->m_pcsItemTemplate);
	
	return static_cast< AgpmItemUsableType >(pcsAgpdItemTemplateUsable->m_nUsableItemType);
}

BOOL	AgpmItem::IsMarvelScroll( AgpdItem* pcsItem )
{
	return ( GetUsableType( pcsItem )	==	AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION );
}

/*
	2005.11.16. By SungHoon
	ĳ�� �������� Grid���� �����Ѵ�.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(INT32 lCID, AgpdItem* pcsItem)
{
	if (!lCID || !pcsItem) return FALSE;

	AgpdCharacter  *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromCashInventory(pcsCharacter, pcsItem);
}

/*
	2005.11.16. By SungHoon
	ĳ�� �������� Grid���� �����Ѵ�.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, AgpdItem* pcsItem)
{
	if (!pcsCharacter || !pcsItem)
		return FALSE;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (2) !!!\n");
		return FALSE;
	}
	pcsItem->m_eStatus	= AGPDITEM_STATUS_NONE;
	EnumCallback(ITEM_CB_ID_CHAR_REMOVE_ITEM_CASH_INVENTORY, pcsItem, pcsCharacter);

	if( m_pagpmGrid->IsExistItem(&pcsItemADChar->m_csCashInventoryGrid, AGPDGRID_ITEM_TYPE_ITEM, pcsItem->m_lID ) == FALSE )
	{ 
		OutputDebugString("AgpmItem::RemoveItemFromGrid() Error (4) !!!\n");
		return FALSE; 
	}

	m_pagpmGrid->RemoveItemAndFillFirst(&pcsItemADChar->m_csCashInventoryGrid, pcsItem->m_pcsGridItem );

//	m_anGridPos�� ������ sync ������. ���� ����� �ƴ϶�� �����Ǵµ� �Ѥ�		by SungHoon
	ArrangeCashItemGridPos(pcsCharacter);

	return TRUE;
}

// 2006.01.04. steeple
// ĳ�� �������� m_anGridPos �� ������ ���ش�. �������� �Ͻ� �� copy & paste
BOOL AgpmItem::ArrangeCashItemGridPos(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	AgpdItemADChar* pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
		return FALSE;

	for(int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0);
		if(pcsGridItem)
		{
			AgpdItem* pcsItem = GetItem(pcsGridItem);
			if(pcsItem && pcsItem->m_pcsItemTemplate)
				pcsItem->m_anGridPos[AGPDITEM_GRID_POS_ROW] = i;
		}
	}

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	lTID ĳ�� �������� Grid���� �����Ѵ�.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(INT32 lCID, INT32 lTID)
{
	if (!lCID || !lTID) return FALSE;

	AgpdCharacter  *pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromCashInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromCashInventory(pcsCharacter, lTID);
}

/*
	2005.11.16. By SungHoon
	lTID ĳ�� �������� Grid���� �����Ѵ�.
*/
BOOL AgpmItem::RemoveItemFromCashInventory(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter)
		return FALSE;

	AgpdGridItem *pGridItem = GetCashItemByTID(pcsCharacter, lTID);
	if (!pGridItem) return FALSE;

	AgpdItem *pcsItem = GetItem(pGridItem);

	return RemoveItemFromCashInventory(pcsCharacter, pcsItem);
}

BOOL AgpmItem::IsCharacterUsingCashItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
	{
		return FALSE;
	}

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
	{
		return NULL;
	}

	INT32 lIndex = 0;
	for(AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex);
		pcsGridItem;
		pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex))
	{
		AgpdItem *pcsItem = GetItem(pcsGridItem);
		if(!pcsItem)
			continue;

		if(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE ||
			pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)	// ��� ���� ����.
		{
			return TRUE;
		}

		// If stamina value started returns true.
		if(pcsItem->m_llStaminaRemainTime != 0 && pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime != 0 &&
			pcsItem->m_llStaminaRemainTime < pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL AgpmItem::IsCharacterUsingCashItemOfSameClassifyIDorTID(AgpdCharacter *pcsCharacter, AgpdItem *pcsItem)
{
	if (!pcsCharacter || !pcsItem)
	{
		return FALSE;
	}

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
	{
		return NULL;
	}

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsTmpItem = GetItem(pcsGridItem);
			if (!pcsTmpItem)
			{
				continue;
			}

			// ���� �������� Pass 2005.12.20. steeple
			if(pcsTmpItem->m_lID == pcsItem->m_lID)
				continue;

			if ((pcsTmpItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsTmpItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)
				&&
				( (pcsTmpItem->m_pcsItemTemplate->m_lClassifyID == pcsItem->m_pcsItemTemplate->m_lClassifyID && pcsItem->m_pcsItemTemplate->m_lClassifyID != 0)
				||
				(pcsItem->m_pcsItemTemplate->m_lID == pcsTmpItem->m_pcsItemTemplate->m_lID) )
				)
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL AgpmItem::UseAllAleadyInUseCashItem(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
	{
		return FALSE;
	}

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
	{
		return NULL;
	}

	BOOL bRet = FALSE;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem
				&& (pcsItem->m_nInUseItem || pcsItem->m_lExpireTime || pcsItem->m_lRemainTime < pcsItem->m_pcsItemTemplate->m_lRemainTime))
			{
				EnumCallback(ITEM_CB_ID_USE_ITEM, pcsItem, pcsCharacter);
				bRet = TRUE;
			}
		}
	}

	return bRet;
}


/*
	2005.12.09. By SungHoon
	ĳ�� �������̰� ������̰� Usable, SkillScroll, lSkillTID �� ������
*/
AgpdGridItem* AgpmItem::GetUsingCashItemBySkillTID(AgpdCharacter *pcsCharacter, INT32 lSkillTID )
{
	if (!pcsCharacter || !lSkillTID) return NULL;

	AgpdItemADChar *pcsItemADChar = GetADCharacter(pcsCharacter);
	if (!pcsItemADChar) return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem *pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if (pcsGridItem)
		{
			AgpdItem *pcsItem = GetItem(pcsGridItem);
			if (pcsItem &&
				(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	�����
					pcsItem->m_pcsItemTemplate && pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
			{
				AgpdItemTemplate* pcsAgpdItemTemplateUsable = (AgpdItemTemplate*)pcsItem->m_pcsItemTemplate;
				if (((AgpdItemTemplateUsable *)pcsAgpdItemTemplateUsable)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL
					&& (((AgpdItemTemplateUsableSkillScroll *) pcsAgpdItemTemplateUsable)->m_lSkillTID == lSkillTID))
					return pcsGridItem;
			}
		}
	}

	return NULL;
}

// 2008.06.23. steeple
// Return a pet item which has a sub inventory in using state.
// If you can't find out, returns NULL.
AgpdGridItem* AgpmItem::GetUsingCashPetItemInvolveSubInventory(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	AgpdItemADChar* pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
		return NULL;

	INT32 lIndex = 0;
	for(AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex);
		pcsGridItem;
		pcsGridItem = m_pagpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex))
	{
		AgpdItem* pcsItem = GetItem(pcsGridItem);
		if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
			continue;

		if(pcsItem->m_nInUseItem != AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime == 0)
			continue;

		INT32 lPetTID = 0;
		EnumCallback(ITEM_CB_ID_GET_PET_TID_BY_ITEM_FROM_SKILL, pcsItem->m_pcsItemTemplate, &lPetTID);
		if(lPetTID == 0)
			continue;

		AgpdCharacterTemplate* pcsCharacterTemplate = m_pagpmCharacter->GetCharacterTemplate(lPetTID);
		if(!pcsCharacterTemplate || pcsCharacterTemplate->m_lPetType != 1)
			continue;

		return pcsGridItem;
	}

	return NULL;
}

// 2005.12.12. steeple
// ĳ�� ������ �߿��� ������̰�, Usable �� ����߿��� OnAttack �ÿ� ���� ������ �ϳ� �ٿ��� �ϴ� ���� ���δ�.
// ���ϰ��� �پ�� ����
INT32 AgpmItem::SubCashItemStackCountOnAttack(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return 0;

	// PC �� �ƴ϶�� ������.
	if(m_pagpmCharacter->IsPC(pcsCharacter) == FALSE)
		return 0;

	AgpdItemADChar* pcsItemADChar = GetADCharacter(pcsCharacter);
	if(!pcsItemADChar)
		return 0;

	INT32 lCount = 0;
	for(int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, 0, i, 0 );
		if(!pcsGridItem)
			continue;

		AgpdItem* pcsItem = GetItem(pcsGridItem);
		if(!pcsItem || !pcsItem->m_pcsItemTemplate)
			continue;

		if((pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&	//	�����
			pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
			pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_ONE_ATTACK)
		{
			// AgsmItem::CallbackUpdateStackCount ���� �ű� 2006.02.08. steeple
			//if(GetItemStackCount(pcsItem) == 1)
			//	UnuseItem(pcsCharacter, pcsItem);

			SubItemStackCount(pcsItem, 1);
			lCount++;
		}
	}

	return lCount;
}


/*
	2005.12.12. By SungHoon
	pcsItem �� ĳ���������� ��������� �������� �˻��Ѵ�.
*/
BOOL AgpmItem::CheckEnableStopCashItem(AgpdItem *pcsItem)
{
	if (!pcsItem)
		return FALSE;

	return pcsItem->m_pcsItemTemplate->m_bCanStopUsingItem;

	// m_bCanStopUsingItem ������ �߰���3... 20051213, kelovon
////	if (!pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_UNUSE) return FALSE;		//	���� ������� �ƴ� �������� ���� �Ұ�.
//	// ������ �������� �� �� ����ϸ� ����� �� ����.
//	if ((pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME
//		|| pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME))
//	{
//		return FALSE;
//	}
//
//	return FALSE;		//	��ü�� ���Ұ�
//	//	������ �������� TRUE�� �����ϸ� �ȴ�.
}

AgpdItem* AgpmItem::GetCashSkillRollbackScroll(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsAttachData	= (AgpdItemADChar *)	GetADCharacter(pcsCharacter);

	for(int i = 0; i < pcsAttachData->m_csCashInventoryGrid.m_nRow; i++)
	{
		AgpdGridItem* pcsGridItem = m_pagpmGrid->GetItem(&pcsAttachData->m_csCashInventoryGrid, 0, i, 0 );
		if(!pcsGridItem)
			continue;

		AgpdItem	*pcsItem	= GetItem(pcsGridItem);
		if (pcsItem && pcsItem->m_pcsItemTemplate)
		{
			if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILLROLLBACK_SCROLL)
				return pcsItem;
		}
	}

	return NULL;
}

AgpdItem* AgpmItem::GetCashInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter || lTID == AP_INVALID_IID)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csCashInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csCashInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					AgpdItem	*pcsItem	= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						if (lTID == ((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_lID)
							return pcsItem;
					}
				}
			}
		}
	}

	return NULL;
}

AgpdItem* AgpmItem::GetCashInventoryItemByType(AgpdCharacter *pcsCharacter, AgpmItemUsableType eItemType, BOOL bInUse)
{
	if(NULL == pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csCashInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csCashInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					AgpdItem	*pcsItem	= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						if (eItemType == ((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType)
						{
							if(bInUse)
							{
								if (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)
									return pcsItem;
							}
							else
							{
								return pcsItem;
							}
							
						}
					}
				}
			}
		}
	}

	return NULL;
}
