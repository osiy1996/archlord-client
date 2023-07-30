// AgpmItemSubInventory.cpp
// (C) NHN Games - ArchLord Development Team
// steeple, 2008.05.08.

#include <AgpmItem/AgpmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmGrid/AgpmGrid.h>

BOOL AgpmItem::SetCallbackSubInventoryCheckAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_SUB_INVENTORY_CHECK_ADD, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackSubInventoryAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_SUB_INVENTORY_ADD, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackSubInventoryUpdate(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_SUB_INVENTORY_UPDATE, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackSubInventoryRemove(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_SUB_INVENTORY_REMOVE, pfCallback, pClass);
}


AgpdGrid* AgpmItem::GetSubInventory(INT32 lCID)
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);
	if(!pcsCharacter)
		return NULL;

	return GetSubInventory(pcsCharacter);
}

AgpdGrid* AgpmItem::GetSubInventory(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return NULL;

	AgpdItemADChar* pcsAgpdItemADChar = GetADCharacter(pcsCharacter);
	if(pcsAgpdItemADChar)
		return &pcsAgpdItemADChar->m_csSubInventoryGrid;

	return NULL;
}

BOOL AgpmItem::AddItemCheckToSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	return AddItemCheckToSubInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bStackMerge);
}

BOOL AgpmItem::AddItemCheckToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	// ������ �̱���
	return TRUE;
}

BOOL AgpmItem::AddItemToSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	return AddItemToSubInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bStackMerge);
}

BOOL AgpmItem::AddItemToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if(!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_SUB_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if(IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// Cannot insert money to a sub inventory
	if(((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		return FALSE;
	}

	if (lLayer < 0 && lRow < 0 && lColumn < 0)
		return AddItemToSubInventory(pcsCharacter, pcsAgpdItem, bStackMerge);

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if(bStackMerge)
		eResult	= InsertStackMerge(pcsAgpdItem, GetSubInventory(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= Insert(pcsAgpdItem, GetSubInventory(pcsCharacter), lLayer, lRow, lColumn);

	return AddItemToSubInventoryResult(eResult,
										pcsAgpdItem,
										pcsCharacter);
}

BOOL AgpmItem::AddItemToSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_SUB_INVENTORY, -1, -1, -1))
		return FALSE;

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// Cannot insert money to a sub inventory
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		return FALSE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= AutoInsertStackMerge(pcsAgpdItem, GetSubInventory(pcsCharacter));
	else
		eResult	= AutoInsert(pcsAgpdItem, GetInventory(pcsCharacter));

	return AddItemToSubInventoryResult(eResult,
										pcsAgpdItem,
										pcsCharacter);
}

BOOL AgpmItem::AddItemToSubInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter)
{
	if (!pcsAgpdItem || !pcsCharacter)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// ���� ���¸� Release ��Ű�� ���� ���¸� Sub Inventory�� �ٲ۴�.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_SUB_INVENTORY);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_SUB_INVENTORY;

		ChangeItemOwner(pcsAgpdItem, pcsCharacter);

		// ���� ȭ����� �����̶�� ī��Ʈ�� ����Ѵ�.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAttachData->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAttachData->m_lNumBoltCount += lStackCount;
		}

		EnumCallback(ITEM_CB_ID_SUB_INVENTORY_ADD, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		// ���� ȭ����� �����̶�� ī��Ʈ�� ����Ѵ�.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAttachData->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAttachData->m_lNumBoltCount += lStackCount;
		}

		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//������� �������� ���ش�.
		RemoveItem( pcsAgpdItem, TRUE );

		return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::UpdateItemInSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return UpdateItemInSubInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::UpdateItemInSubInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_SUB_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY)
		return AddItemToSubInventory(pcsCharacter, pcsAgpdItem, lLayer, lRow, lColumn);

	INT32	lPrevLayer	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	INT32	lPrevRow	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	INT32	lPrevColumn	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	AgpdItemGridResult	eResult	= InsertStackMerge(pcsAgpdItem, &pcsAttachData->m_csSubInventoryGrid, lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		m_pagpmGrid->Clear(&pcsAttachData->m_csSubInventoryGrid, lPrevLayer, lPrevRow, lPrevColumn, 1, 1);
		m_pagpmGrid->DeleteItem(&pcsAttachData->m_csSubInventoryGrid, pcsAgpdItem->m_pcsGridItem);

		// Admin Client �� ���ؼ� Update �Ǿ��� ���� �ҷ��ش�.
		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pcsAgpdItem, NULL);

		BOOL	bIsUpdate	= TRUE;

		EnumCallback(ITEM_CB_ID_SUB_INVENTORY_ADD, pcsAgpdItem, &bIsUpdate);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "SubInventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//������� �������� ���ش�.
		RemoveItem( pcsAgpdItem, TRUE );

		return  TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_NOT_EMPTY)
	{
		AgpdItem	*pcsAgpdExistItem = GetItem(m_pagpmGrid->GetItem(&pcsAttachData->m_csSubInventoryGrid, lLayer, lRow, lColumn));
		if (!pcsAgpdExistItem)
			return FALSE;

		// ���� �ѳ��� ���ɼ��ΰ�� ���� ó���� �Ѵ�.
		//		pcsAgpdExistItem ���� pcsAgpdItem�� ������ ����̴�.
		//		�̶� pcsAgpdItem �� ���ɼ��ΰ�� ������ �������� ���θ� �Ǵ��� �� �� ������ ó���Ѵ�.
		//////////////////////////////////////////////////////////////////////////
		EnumCallback(ITEM_CB_ID_REQUEST_SPIRITSTONE_CONVERT, pcsAgpdExistItem, pcsAgpdItem);

		return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}

BOOL AgpmItem::RemoveItemFromSubInventory(INT32 lCID, AgpdItem *pcsAgpdItem)
{
	return RemoveItemFromSubInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem);
}

BOOL AgpmItem::RemoveItemFromSubInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromInventory() Error (2) !!!\n");
		return FALSE;
	}

	pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NONE;

	// ���� ȭ����� �����̶�� ī��Ʈ�� ����Ѵ�.
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
	{
		pcsAgpdItemADChar->m_lNumArrowCount -= pcsAgpdItem->m_nCount;
	}
	else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
	{
		pcsAgpdItemADChar->m_lNumBoltCount -= pcsAgpdItem->m_nCount;
	}

	EnumCallback(ITEM_CB_ID_SUB_INVENTORY_REMOVE, (PVOID)pcsAgpdItem, NULL);

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csSubInventoryGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}