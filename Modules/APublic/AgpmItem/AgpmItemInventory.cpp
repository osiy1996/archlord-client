/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 1. 8
******************************************************************************/

#include <stdio.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmGrid/AgpmGrid.h>

/******************************************************************************
* Purpose : Set call-back.
*
* 100102. Bob Jung
******************************************************************************/
BOOL AgpmItem::SetCallbackInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackInventoryForAdmin(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveFromInventory(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_REMOVE_INVENTORY, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack)
{
	return AddItemToInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn, bIsMergeStack);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsMergeStack)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// ���⿡ ���;� �� �� ����. 2007.12.12. steeple
	// ��Ţ���� ������ �������� ������ Rapier �̰�, �޼տ� ��� �ִ� �������� ���ؼ� Dagger ��� �����ش�. 2007.11.02. steeple
	if(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_EQUIP &&
		GetWeaponType(pcsAgpdItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_RAPIER)
	{
		AgpdItem* pcsLeftItem = GetEquipSlotItem(pcsCharacter, AGPMITEM_PART_HAND_LEFT);
		if(pcsLeftItem && pcsLeftItem->m_pcsItemTemplate && pcsLeftItem->m_lID != pcsAgpdItem->m_lID &&
			GetWeaponType(pcsLeftItem->m_pcsItemTemplate) == AGPMITEM_EQUIP_WEAPON_TYPE_ONE_HAND_DAGGER)
		{
			// ���� ĭ�� �� ĭ �̻��̾�� �Ѵ�.
			if(m_pagpmGrid->GetEmpyGridCount(GetInventory(pcsCharacter)) < 2)
				return FALSE;
		}
	}

	if (lLayer < 0 && lRow < 0 && lColumn < 0)
		return AddItemToInventory(pcsCharacter, pcsAgpdItem, bIsMergeStack);

	// �κ��丮�� �������� �ϴ� �������� ������ ���캻��.
	// ���̶�� pcsCharacter�� ���� �߰��ϰ� pcsAgpdItem�� �����Ѵ�. (�ֺ��� �����Ѵٴ� ������ ������ �Ѵ�.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// ���̴�.
		// character�� �� �߰���Ű�� �����Ѵ�.

		FLOAT	fPCBangBonus	= 1.0f;

		//if (m_pagpmBillInfo->IsPCBang(pcsCharacter))
		//	fPCBangBonus	= 1.5f;

		INT64 llMoney = (INT64)GetItemMoney(pcsAgpdItem);
		llMoney = (INT64)((FLOAT)llMoney * fPCBangBonus);

		m_pagpmCharacter->AddMoney(pcsCharacter, (INT64)llMoney);
		m_pagpmCharacter->UpdateMoney(pcsCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		pcsAgpdItem->m_Mutex.SafeRelease();

		RemoveItem(pcsAgpdItem->m_lID);

		return TRUE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= InsertStackMerge(pcsAgpdItem, GetInventory(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult	= Insert(pcsAgpdItem, GetInventory(pcsCharacter), lLayer, lRow, lColumn);

	return AddItemToInventoryResult(eResult,
									pcsAgpdItem,
									pcsCharacter);
}

BOOL AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	return AddItemToInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, BOOL bIsMergeStack)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, -1, -1, -1))
		return FALSE;

	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType))
	{
		return AddItemToCashInventory(pcsCharacter, pcsAgpdItem);
	}

	// �κ��丮�� �������� �ϴ� �������� ������ ���캻��.
	// ���̶�� pcsCharacter�� ���� �߰��ϰ� pcsAgpdItem�� �����Ѵ�. (�ֺ��� �����Ѵٴ� ������ ������ �Ѵ�.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// ���̴�.
		// character�� �� �߰���Ű�� �����Ѵ�.

		FLOAT	fPCBangBonus	= 1.0f;

		//if (m_pagpmBillInfo->IsPCBang(pcsCharacter))
		//	fPCBangBonus	= 1.5f;

		INT64 llMoney = (INT64)GetItemMoney(pcsAgpdItem);
		llMoney = (INT64)((FLOAT)llMoney * fPCBangBonus);

		m_pagpmCharacter->AddMoney(pcsCharacter, (INT64)llMoney);
		m_pagpmCharacter->UpdateMoney(pcsCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		pcsAgpdItem->m_Mutex.SafeRelease();

		RemoveItem(pcsAgpdItem->m_lID);

		return TRUE;
	}

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (bIsMergeStack)
		eResult	= AutoInsertStackMerge(pcsAgpdItem, GetInventory(pcsCharacter));
	else
		eResult	= AutoInsert(pcsAgpdItem, GetInventory(pcsCharacter));

	return AddItemToInventoryResult(eResult,
									pcsAgpdItem,
									pcsCharacter);
}

BOOL AgpmItem::AddItemToInventoryResult(AgpdItemGridResult eResult, AgpdItem *pcsAgpdItem, AgpdCharacter *pcsCharacter)
{
	if (!pcsAgpdItem || !pcsCharacter)
		return FALSE;

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// ���� ���¸� Release ��Ű�� ���� ���¸� Inventory�� �ٲ۴�.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

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

		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pcsAgpdItem, NULL);

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

BOOL AgpmItem::UpdateItemInInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return UpdateItemInInventory(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::UpdateItemInInventory(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (!CheckUpdateStatus(pcsAgpdItem, AGPDITEM_STATUS_INVENTORY, lLayer, lRow, lColumn))
		return FALSE;

	if (pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return AddItemToInventory(pcsCharacter, pcsAgpdItem, lLayer, lRow, lColumn);

	INT32	lPrevLayer	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB];
	INT32	lPrevRow	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW];
	INT32	lPrevColumn	= pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN];

	AgpdItemADChar	*pcsAttachData	= GetADCharacter(pcsCharacter);
	if(NULL == pcsAttachData)
		return FALSE;

	AgpdItemGridResult	eResult	= InsertStackMerge(pcsAgpdItem, &pcsAttachData->m_csInventoryGrid, lLayer, lRow, lColumn);

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		m_pagpmGrid->Clear(&pcsAttachData->m_csInventoryGrid, lPrevLayer, lPrevRow, lPrevColumn, 1, 1);
		m_pagpmGrid->DeleteItem(&pcsAttachData->m_csInventoryGrid, pcsAgpdItem->m_pcsGridItem);

		// Admin Client �� ���ؼ� Update �Ǿ��� ���� �ҷ��ش�.
		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pcsAgpdItem, NULL);

		BOOL	bIsUpdate	= TRUE;

		EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY, pcsAgpdItem, &bIsUpdate);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_SUCCESS_STACKCOUNT_ZERO)
	{
		ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
		strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:stackcount == 0", AGPMITEM_MAX_DELETE_REASON);
		//������� �������� ���ش�.
		RemoveItem( pcsAgpdItem, TRUE );

		return  TRUE;
	}
	else if (eResult == AGPDITEM_INSERT_NOT_EMPTY)
	{
		AgpdItem	*pcsAgpdExistItem = GetItem(m_pagpmGrid->GetItem(&pcsAttachData->m_csInventoryGrid, lLayer, lRow, lColumn));
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

/*
//������ ��ġ�� �������� �־��ش�.
INT32 AgpmItem::AddItemToInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol, BOOL bRemoveItem, BOOL bIsStackMerge)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (1) !!!\n");
		return FALSE;
	}

	return AddItemToInventory(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol, bRemoveItem, bIsStackMerge);
}

BOOL AgpmItem::AddItemToInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol, BOOL bRemoveItem, BOOL bIsStackMerge)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return AGPMITEM_AddItemInventoryResult_FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (2) !!!\n");
		return AGPMITEM_AddItemInventoryResult_FALSE;
	}

	INT32	lStackCount	= pcsAgpdItem->m_nCount;

	// �κ��丮�� �������� �ϴ� �������� ������ ���캻��.
	// ���̶�� pcsAgpdCharacter�� ���� �߰��ϰ� pcsAgpdItem�� �����Ѵ�. (�ֺ��� �����Ѵٴ� ������ ������ �Ѵ�.)
	if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_OTHER &&
		((AgpdItemTemplateOther *) pcsAgpdItem->m_pcsItemTemplate)->m_eOtherItemType == AGPMITEM_OTHER_TYPE_MONEY)
	{
		// ���̴�.
		// character�� �� �߰���Ű�� �����Ѵ�.
		m_pagpmCharacter->AddMoney(pcsAgpdCharacter, (INT64) GetItemMoney(pcsAgpdItem));
		m_pagpmCharacter->UpdateMoney(pcsAgpdCharacter);

		EnumCallback(ITEM_CB_ID_PICKUP_ITEM_MONEY, pcsAgpdItem, NULL);

		// �����Ѵ�.
		if (bRemoveItem)
		{
			pcsAgpdItem->m_Mutex.SafeRelease();

			RemoveItem(pcsAgpdItem->m_lID);
		}

		return AGPMITEM_AddItemInventoryResult_TRUE;
	}
	//���� �ƴϰ�~
	else
	{
		if (!(pnInvIdx && pnInvRow && pnInvCol))
		{
			BOOL				bStackStatus;

			bStackStatus = TRUE;

//			if( (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NPC_TRADE) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID ) || ( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID ) )
			if(		( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID )
				||	( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CLIENT_TRADE_GRID )
				||	( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NONE )
				)
			{
				bStackStatus = FALSE;
			}

			//����Ŀ�� �������� ���.
			if( ((AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate)->m_bStackable && bStackStatus && bIsStackMerge)
			{
				AgpdItem			*pcsAgpdStackItem;
				AgpdGrid			*pcsGrid;
				AgpdItemTemplate	*pTemplate;

				INT32			lIndex;
				INT32			lTempStackCount;

				lIndex = 0;
				pcsGrid = &pcsAgpdItemADChar->m_csInventoryGrid;
				pTemplate = (AgpdItemTemplate *)pcsAgpdItem->m_pcsItemTemplate;

				while( 1 )
				{
					AgpdGridItem		*pcsGridItem;

					pcsGridItem = m_pagpmGrid->GetItemByTemplate( lIndex, pcsGrid, AGPDGRID_ITEM_TYPE_ITEM, pTemplate->m_lID );

					if( pcsGridItem == NULL )
					{
						break;
					}
					else
					{
						pcsAgpdStackItem = GetItem( pcsGridItem );

						if( pcsAgpdStackItem )
						{
							if (CheckJoinItem(pcsAgpdStackItem, pcsAgpdItem))
							{
								//������ �������� �ʾҴٸ�?
								if( pcsAgpdStackItem->m_nCount < pTemplate->m_lMaxStackableCount )
								{
									lTempStackCount = pTemplate->m_lMaxStackableCount - pcsAgpdStackItem->m_nCount;

									if( lTempStackCount < pcsAgpdItem->m_nCount )
									{
										AddItemStackCount(pcsAgpdStackItem, lTempStackCount);
										SubItemStackCount(pcsAgpdItem, lTempStackCount);
									}
									else
									{
										AddItemStackCount(pcsAgpdStackItem, pcsAgpdItem->m_nCount);
										pcsAgpdItem->m_nCount = 0;
									}

									//����� ������ ��, pcsAgpdStackItem�� �����϶�� Ŭ���̾�Ʈ�� ��Ŷ�� ������.
									//EnumCallback(ITEM_CB_ID_UPDATE_STACK_COUNT, pcsAgpdStackItem, NULL);

									//�����ִ� ����ī��Ʈ�� ���ٸ� �극��ũ!
									if( pcsAgpdItem->m_nCount <= 0 )
									{
										break;
									}
								}
							}
						}
					}
				}

				if( pcsAgpdItem->m_nCount <= 0 )
				{
					// ���� ȭ����� �����̶�� ī��Ʈ�� ����Ѵ�.
					if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
					{
						pcsAgpdItemADChar->m_lNumArrowCount += lStackCount;
					}
					else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
					{
						pcsAgpdItemADChar->m_lNumBoltCount += lStackCount;
					}

					ZeroMemory(pcsAgpdItem->m_szDeleteReason, sizeof(CHAR) * (AGPMITEM_MAX_DELETE_REASON + 1));
					strncpy(pcsAgpdItem->m_szDeleteReason, "Inventory:����ī��Ʈ 0", AGPMITEM_MAX_DELETE_REASON);
					//������� �������� ���ش�.
					RemoveItem( pcsAgpdItem, TRUE );

					return AGPMITEM_AddItemInventoryResult_RemoveByStack;
				}
			}
		}
	}

	BOOL	bAddResult	= AddItemToGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, ITEM_CB_ID_CHAR_ADD_INVENTORY, AGPDITEM_STATUS_INVENTORY,
										pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);

	if (bAddResult)
	{
		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_INVENTORY;

		// ���� ȭ����� �����̶�� ī��Ʈ�� ����Ѵ�.
		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetArrowTID())
		{
			pcsAgpdItemADChar->m_lNumArrowCount += lStackCount;
		}
		else if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID == GetBoltTID())
		{
			pcsAgpdItemADChar->m_lNumBoltCount += lStackCount;
		}
	}

	return bAddResult;
}
*/

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromInventory(INT32 lCID, AgpdItem* pcsAgpdItem)
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::RemoveItemFromInventory() Error (1) !!!\n");
		return FALSE;
	}

	return RemoveItemFromInventory(pcsAgpdCharacter, pcsAgpdItem);
}

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
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

	EnumCallback(ITEM_CB_ID_REMOVE_INVENTORY_GRID, (PVOID)pcsAgpdItem, NULL);

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}

/*
BOOL AgpmItem::UpdateInventory(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UpdateInventory() Error (1) !!!\n");
		return FALSE;
	}

	return UpdateInventory(pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn );
}

BOOL AgpmItem::UpdateInventory(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::UpdateInventory() Error (2) !!!\n");
		return FALSE;
	}

	// ���� ���� OtherType �� ���õ��� �ʾҴ�. ���.. �� �ڵ�� �����ؾ� �Ǵµ� �ϴ� �Ʒ����� �ӽ� ó���Ѵ�.
	if (GetMoneyTID() == ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lID)
	{
		m_pagpmCharacter->AddMoney(pcsAgpdCharacter, (INT64) GetItemMoney(pcsAgpdItem));

		pcsAgpdItem->m_Mutex.Release();

		return RemoveItem(pcsAgpdItem->m_lID);
	}

	// Admin Client �� ���ؼ� Update �Ǿ��� ���� �ҷ��ش�.
	EnumCallback(ITEM_CB_ID_CHAR_ADD_INVENTORY_FOR_ADMIN, pcsAgpdItem, NULL);

	return UpdateGrid(&pcsAgpdItemADChar->m_csInventoryGrid, -1, ITEM_CB_ID_CHAR_ADD_INVENTORY, AGPDITEM_STATUS_INVENTORY,
					  pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn);
}
*/

AgpdItem* AgpmItem::GetInventoryItemByTID(AgpdCharacter *pcsCharacter, INT32 lTID)
{
	if (!pcsCharacter || lTID == AP_INVALID_IID)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
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

AgpdItem* AgpmItem::GetInventoryPotionItem(AgpdCharacter *pcsCharacter, AgpmItemUsablePotionType ePotionType)
{
	if (!pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsItemADChar	= GetADCharacter(pcsCharacter);
	if (!pcsItemADChar)
		return NULL;

	AgpdItemTemplate	*pcsItemTemplate	= NULL;
	AgpdItem			*pcsItem			= NULL;

	for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					pcsItem		= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						AgpdItemTemplate *pcsItemTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
						if (pcsItemTemplate &&
							pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
							((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION &&
							((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType == ePotionType)
						{
							return pcsItem;
						}
					}
				}
			}
		}
	}

	return NULL;
}

//�κ���ġ�� �Ľ��Ѵ�.
BOOL AgpmItem::ParseInventoryPacket(PVOID pInventory, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol)
{
	if( !pInventory )
		return FALSE;

	if( !pnInvIdx || !pnInvRow || !pnInvCol )
		return FALSE;

	m_csPacketInventory.GetField(FALSE, pInventory, *((UINT16 *)(pInventory)),
								pnInvIdx,
								pnInvRow,
								pnInvCol);

	return TRUE;
}

AgpdItem* AgpmItem::GetSkillRollbackScroll(AgpdCharacter *pcsCharacter)
{
	if (!pcsCharacter)
		return NULL;

	AgpdItemADChar	*pcsAttachData	= (AgpdItemADChar *)	GetADCharacter(pcsCharacter);

	for (int i = 0; i < pcsAttachData->m_csInventoryGrid.m_nLayer; ++i)
	{
		for (int j = 0; j < pcsAttachData->m_csInventoryGrid.m_nColumn; ++j)
		{
			for (int k = 0; k < pcsAttachData->m_csInventoryGrid.m_nRow; ++k)
			{
				AgpdGridItem	*pcsGridItem	= m_pagpmGrid->GetItem(&pcsAttachData->m_csInventoryGrid, i, k, j);
				if (pcsGridItem)
				{
					AgpdItem	*pcsItem	= GetItem(pcsGridItem);
					if (pcsItem && pcsItem->m_pcsItemTemplate)
					{
						if (pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
							((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILLROLLBACK_SCROLL)
							return pcsItem;
					}
				}
			}
		}
	}

	return NULL;
}