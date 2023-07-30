// AgpmItemUnseenInventory.cpp
// (C) NHN Games - ArchLord Development Team
// iluvs. 2008.11.07.

#include "AgpmItem.h"

BOOL AgpmItem::SetCallbackUnseenInventoryAdd(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_UNSEEN_INVENTORY_ADD, pfCallback, pClass);
}

AgpdGrid* AgpmItem::GetUnseenInventory(INT32 lCID )
{
	AgpdCharacter* pcsCharacter = m_pagpmCharacter->GetCharacter(lCID);
	if(!pcsCharacter)
		return NULL;

	return GetSubInventory(pcsCharacter);
}

AgpdGrid* AgpmItem::GetUnseenInventory(AgpdCharacter *pcsAgpdCharacter )
{
	if(!pcsAgpdCharacter)
		return NULL;

	AgpdItemADChar* pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
		return NULL;

	return &pcsAgpdItemADChar->m_csUnseenInventoryGrid;
}

BOOL AgpmItem::AddItemToUnseenInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_UNSEEN_INVENTORY)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_UNSEEN_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;
	
	if(((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		return FALSE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bStackMerge)
		eResult	= InsertStackMerge(pcsAgpdItem, GetUnseenInventory(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= Insert(pcsAgpdItem, GetUnseenInventory(pcsCharacter), lLayer, lRow, lColumn);

	return AddItemToUnseenInventoryResult(eResult,
										 pcsAgpdItem,
										 pcsCharacter);
}

BOOL AgpmItem::AddItemToUnseenInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bStackMerge)
{
	return AddItemToUnseenInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bStackMerge);
}

BOOL AgpmItem::AddItemAutoToUnSeenInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	if(!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_UNSEEN_INVENTORY)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_UNSEEN_INVENTORY, -1, -1, -1))
		return FALSE;

	if(((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		return FALSE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= AutoInsertStackMerge(pcsAgpdItem, GetUnseenInventory(pcsCharacter));
	else
		eResult	= AutoInsert(pcsAgpdItem, GetUnseenInventory(pcsCharacter));

	return AddItemToUnseenInventoryResult(eResult,
										  pcsAgpdItem,
										  pcsCharacter);
}

BOOL AgpmItem::AddItemToUnseenInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter)
{
	if(!pcsAgpdItem || !pcsCharacter)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// 이전 상태를 Release 시키고 현재 상태를 Sub Inventory로 바꾼다.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_UNSEEN_INVENTORY);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_UNSEEN_INVENTORY;

		ChangeItemOwner(pcsAgpdItem, pcsCharacter);

		// 만약 화살류의 아템이라면 카운트를 계산한다.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAttachData->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAttachData->m_lNumBoltCount += lStackCount;
		}

		EnumCallback(ITEM_CB_ID_UNSEEN_INVENTORY_ADD, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		// 만약 화살류의 아템이라면 카운트를 계산한다.
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
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return TRUE;
	}

	return FALSE;
}

BOOL AgpmItem::RemoveItemFromUnSeenInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem)
{
	if(!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if( !pcsAgpdItemADChar )
	{
		OutputDebugString("AgpmItem::RemoveItemFromInventory() Error (2) !!!\n");
		return FALSE;
	}

	pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_NONE;

	// 만약 화살류의 아템이라면 카운트를 계산한다.
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
	{
		pcsAgpdItemADChar->m_lNumArrowCount -= pcsAgpdItem->m_nCount;
	}
	else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
	{
		pcsAgpdItemADChar->m_lNumBoltCount -= pcsAgpdItem->m_nCount;
	}

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csUnseenInventoryGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}

BOOL AgpmItem::RemoveItemFromUnSeenInventory(INT32 lCID, AgpdItem *pcsAgpdItem)
{
	return RemoveItemFromUnSeenInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem);
}

BOOL AgpmItem::UpdateItemInUnseenInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return UpdateItemInUnseenInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::UpdateItemInUnseenInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if(!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_UNSEEN_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_UNSEEN_INVENTORY)
		return AddItemAutoToUnSeenInventory(pcsAgpdCharacter, pcsAgpdItem, FALSE);

	INT32	lPrevLayer	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	INT32	lPrevRow	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	INT32	lPrevColumn	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsAgpdCharacter);
	if(!pcsAttachData)
		return FALSE;

	AgpdItemGridResult	eResult	= InsertStackMerge(pcsAgpdItem, &pcsAttachData->m_csUnseenInventoryGrid, lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		m_pagpmGrid->Clear(&pcsAttachData->m_csUnseenInventoryGrid, lPrevLayer, lPrevRow, lPrevColumn, 1, 1);
		m_pagpmGrid->DeleteItem(&pcsAttachData->m_csUnseenInventoryGrid, pcsAgpdItem->m_pcsGridItem);

		BOOL	bIsUpdate	= TRUE;

		EnumCallback(ITEM_CB_ID_UNSEEN_INVENTORY_ADD, pcsAgpdItem, &bIsUpdate);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "SubInventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//만들어진 아이템을 없앤다.
		RemoveItem( pcsAgpdItem, TRUE );

		return  TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_NOT_EMPTY)
	{
		AgpdItem	*pcsAgpdExistItem = GetItem(m_pagpmGrid->GetItem(&pcsAttachData->m_csUnseenInventoryGrid, lLayer, lRow, lColumn));
		if (!pcsAgpdExistItem)
			return FALSE;

		// 둘중 한넘이 정령석인경우 개조 처리를 한다.
		//		pcsAgpdExistItem 위에 pcsAgpdItem을 얻어놓은 경우이다.
		//		이때 pcsAgpdItem 이 정령석인경우 개조가 가능한지 여부를 판단해 그 뒤 과정을 처리한다.
		//////////////////////////////////////////////////////////////////////////
		EnumCallback(ITEM_CB_ID_REQUEST_SPIRITSTONE_CONVERT, pcsAgpdExistItem, pcsAgpdItem);

		return FALSE;
	}
	else
		return FALSE;

	return TRUE;
}