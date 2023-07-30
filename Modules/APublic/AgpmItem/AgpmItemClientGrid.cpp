/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 2. 3
******************************************************************************/

#include <AgpmItem/AgpmItem.h>
#include <AgpmCharacter/AgpmCharacter.h>

/*BOOL AgpmItem::SetCallbackClientTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_CHAR_INVENTORY, pfCallback, pClass);
}*/

//���� TradeGrid�� ���� �������� ��뿡�� �˸���.
/*BOOL AgpmItem::NewItemToClient(AgpdItem *pcsItem)
{
	// �� ���� �߰��� ������ �ݹ� �Լ����� ȣ�����ش�.
	EnumCallback(ITEM_CB_ID_NEW, pcsItem, pcsItem->m_pcsItemTemplate);

	return TRUE;
}*/

BOOL AgpmItem::AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return AddItemToClientTradeGrid(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (lLayer >= 0)
		eResult	= Insert(pcsAgpdItem, GetClientTradeGrid(pcsAgpdCharacter), lLayer, lRow, lColumn);
	else
		eResult = AutoInsert(pcsAgpdItem, GetClientTradeGrid(pcsAgpdCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		// ���� ���¸� Release ��Ű�� ���� ���¸� Inventory�� �ٲ۴�.
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_CLIENT_TRADE_GRID);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_CLIENT_TRADE_GRID;

		EnumCallback(ITEM_CB_ID_ADD_CLIENT_TRADE_GRID, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}

	return FALSE;
}

/*
//������ ��ġ�� �������� �־��ش�.
BOOL AgpmItem::AddItemToClientTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
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

	return AddItemToClientTradeGrid(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol );
}

BOOL AgpmItem::AddItemToClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToInventory() Error (2) !!!\n");
		return FALSE;
	}

	return AddItemToGrid(&pcsAgpdItemADChar->m_csClientTradeGrid, -1, ITEM_CB_ID_ADD_CLIENT_TRADE_GRID, AGPDITEM_STATUS_CLIENT_TRADE_GRID,
						 pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);
}
*/

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromClientTradeGrid(INT32 lCID, AgpdItem* pcsAgpdItem)
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

	return RemoveItemFromClientTradeGrid(pcsAgpdCharacter, pcsAgpdItem);
}

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromClientTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromClientTradeGrid() Error (2) !!!\n");
		return FALSE;
	}

	return RemoveItemFromGrid(&pcsAgpdItemADChar->m_csClientTradeGrid, -1, pcsAgpdCharacter, pcsAgpdItem);
}
