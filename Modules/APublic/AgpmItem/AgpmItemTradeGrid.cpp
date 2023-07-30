/******************************************************************************
Module:  AgpmItem.cpp
Notices: Copyright (c) NHN Studio 2002 Ashulam
Purpose: 
Last Update: 2003. 2. 3
******************************************************************************/

#include <stdio.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmCharacter/AgpmCharacter.h>

VOID AgpmItem::ResetTradeStats(AgpdItemADChar *pstItemADChar)
{
	pstItemADChar->m_lTradeTargetID = 0;
	pstItemADChar->m_lTradeStatus = AGPDITEM_TRADE_STATUS_NONE;
	pstItemADChar->m_lMoneyCount = 0;
	pstItemADChar->m_lClientMoneyCount = 0;
	pstItemADChar->m_bConfirmButtonDown = false;
	pstItemADChar->m_bTargetConfirmButtonDown = false;

	m_pagpmGrid->Reset(&pstItemADChar->m_csTradeGrid);
}

BOOL AgpmItem::SetCallbackTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_ADD_TRADE_GRID, pfCallback, pClass);
}

BOOL AgpmItem::SetCallbackRemoveTradeGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(ITEM_CB_ID_REMOVE_TRADE_GRID, pfCallback, pClass);
}

BOOL AgpmItem::AddItemToTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	return AddItemToTradeGrid(m_pagpmCharacter->GetCharacter(lCID), pcsAgpdItem, lLayer, lRow, lColumn);
}

BOOL AgpmItem::AddItemToTradeGrid(AgpdCharacter *pcsCharacter, AgpdItem *pcsAgpdItem, INT32 lLayer, INT32 lRow, INT32 lColumn)
{
	if (!pcsCharacter || !pcsAgpdItem)
		return FALSE;

	if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID)
		return FALSE;

	AgpdItemGridResult	eResult	= AGPDITEM_INSERT_FAIL;

	if (lLayer >= 0)
		eResult	= Insert(pcsAgpdItem, GetTradeGrid(pcsCharacter), lLayer, lRow, lColumn);
	else
		eResult = AutoInsert(pcsAgpdItem, GetTradeGrid(pcsCharacter));

	if (eResult == AGPDITEM_INSERT_SUCCESS)
	{
		RemoveStatus(pcsAgpdItem, AGPDITEM_STATUS_TRADE_GRID);

		pcsAgpdItem->m_eStatus	= AGPDITEM_STATUS_TRADE_GRID;

		EnumCallback(ITEM_CB_ID_ADD_TRADE_GRID, pcsAgpdItem, NULL);

		pcsAgpdItem->m_anPrevGridPos	= pcsAgpdItem->m_anGridPos;

		return TRUE;
	}

	return FALSE;
}

/*
//������ ��ġ�� �������� �־��ش�.
BOOL AgpmItem::AddItemToTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
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

	return AddItemToTradeGrid(pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol );
}

BOOL AgpmItem::AddItemToTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 *pnInvIdx, INT16 *pnInvRow, INT16 *pnInvCol )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::AddItemToTradeGrid() Error (2) !!!\n");
		return FALSE;
	}

	return AddItemToGrid(&pcsAgpdItemADChar->m_csTradeGrid, -1, ITEM_CB_ID_ADD_TRADE_GRID, AGPDITEM_STATUS_TRADE_GRID,
						 pcsAgpdCharacter, pcsAgpdItem, pnInvIdx, pnInvRow, pnInvCol);
}
*/

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromTradeGrid(INT32 lCID, AgpdItem* pcsAgpdItem)
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

	return RemoveItemFromTradeGrid(pcsAgpdCharacter, pcsAgpdItem);
}

//�κ����� �������� �����ش�.
BOOL AgpmItem::RemoveItemFromTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem* pcsAgpdItem)
{
	if (!pcsAgpdCharacter || !pcsAgpdItem)
		return FALSE;

	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::RemoveItemFromTradeGrid() Error (2) !!!\n");
		return FALSE;
	}

	if (RemoveItemFromGrid(&pcsAgpdItemADChar->m_csTradeGrid, -1, pcsAgpdCharacter, pcsAgpdItem))
	{
		EnumCallback(ITEM_CB_ID_REMOVE_TRADE_GRID, pcsAgpdItem, pcsAgpdCharacter);
		return TRUE;
	}

	return FALSE;
}

/*
BOOL AgpmItem::UpdateTradeGrid(INT32 lCID, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	if (!lCID || !pcsAgpdItem)
		return FALSE;

	AgpdCharacter  *pcsAgpdCharacter = NULL;
	
	if (m_pagpmCharacter)
		pcsAgpdCharacter = m_pagpmCharacter->GetCharacter(lCID);

	if(!pcsAgpdCharacter)
	{
		OutputDebugString("AgpmItem::UpdateTradeGrid() Error (1) !!!\n");
		return FALSE;
	}

	return UpdateTradeGrid(pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn );
}

BOOL AgpmItem::UpdateTradeGrid(AgpdCharacter *pcsAgpdCharacter, AgpdItem *pcsAgpdItem, INT16 nLayer, INT16 nRow, INT16 nColumn )
{
	AgpdItemADChar *pcsAgpdItemADChar = GetADCharacter(pcsAgpdCharacter);
	if(!pcsAgpdItemADChar)
	{
		OutputDebugString("AgpmItem::UpdateTradeGrid() Error (2) !!!\n");
		return FALSE;
	}

	return UpdateGrid(&pcsAgpdItemADChar->m_csTradeGrid,  -1, ITEM_CB_ID_ADD_TRADE_GRID, AGPDITEM_STATUS_TRADE_GRID,
					  pcsAgpdCharacter, pcsAgpdItem, nLayer, nRow, nColumn);
}
*/

eAGPM_PRIVATE_TRADE_RESULT	AgpmItem::CheckTradeCondition( AgpdCharacter* pcsRequestorChar , AgpdCharacter* pcsAcceptorChar )
{
	BOOL						bRequestor	=	_CheckInvenCondition( pcsRequestorChar );
	BOOL						bAcceptor	=	_CheckInvenCondition( pcsAcceptorChar );
	eAGPM_PRIVATE_TRADE_RESULT	eResult;

	// �ŷ� ����
	if( bRequestor && bAcceptor )
	{
		eResult		=	AGPM_PRIVATE_TRADE_RESULT_SUCCESS;
	}

	// �Ѵ� �ŷ� �Ұ�
	else if( !bRequestor && !bAcceptor )
	{
		eResult		=	AGPM_PRIVATE_TRADE_RESULT_BOTH_FAILED;
	}

	// ��û�� �ŷ��Ұ�
	else if( !bRequestor )
	{
		eResult		=	AGPM_PRIVATE_TRADE_RESULT_REQUESTOR_FAILED;
	}

	// ������ �ŷ��Ұ�
	else if( !bAcceptor )
	{
		eResult		=	AGPM_PRIVATE_TRADE_RESULT_ACCEPTOR_FAILED;
	}

	return eResult;
}

BOOL	AgpmItem::_CheckInvenCondition( AgpdCharacter* pcsCharacter )
{
	if( !pcsCharacter )
		return FALSE;

	INT32		nCashInvenPushCount			=	0;
	INT32		nInvenPushCount				=	0;
	INT32		nInventoryEmptyCount		=	m_pagpmGrid->GetEmpyGridCount( GetInventory( pcsCharacter ) );
	INT32		nCashInventoryEmptyCount	=	m_pagpmGrid->GetEmpyGridCount( GetCashInventoryGrid( pcsCharacter) );
	AgpdGrid*	pTradeGrid					=	GetClientTradeGrid( pcsCharacter );
	if( !pTradeGrid )
		return FALSE;

	// �Ϲ� �κ��丮�� �� ������ ĳ�� �κ��丮�� �� ������ ���Ѵ�
	for( INT i = 0 ; i < m_pagpmGrid->GetItemCount( pTradeGrid) ; ++i )
	{
		AgpdGridItem*	pGridItem	=	m_pagpmGrid->GetItem( pTradeGrid , i );
		if( !pGridItem )
			continue;

		AgpdItem*	pItem	=	GetItem( pGridItem );
		if( !pItem || !pItem->m_pcsItemTemplate )
			continue;

		// ĳ�� �������� ���
		if( IS_CASH_ITEM( pItem->m_pcsItemTemplate->m_eCashItemType ) )
		{
			++nCashInvenPushCount;
		}

		// ĳ�� �������� �ƴ� ���
		else
		{
			++nInvenPushCount;
		}
	}

	// Cash Inven�� �Ϲ� Inven�� ������ �����ϸ� ����
	if( (nInvenPushCount > nInventoryEmptyCount) || (nCashInvenPushCount > nCashInventoryEmptyCount) )
	{
		return FALSE;
	}

	return TRUE;
}	