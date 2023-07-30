/******************************************************************************
Module:  AgsmTrade.cpp
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2002. 05. 15
******************************************************************************/

#include "ApBase.h"
#include "AgsmTrade.h"
#include "AgsdCharacter.h"
#include "AgsdTrade.h"

AgsmTrade::AgsmTrade()
{
	SetModuleName("AgsmTrade");
}

AgsmTrade::~AgsmTrade()
{
}

BOOL AgsmTrade::OnAddModule()
{
	m_pagpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pagpmItem = (AgpmItem *) GetModule("AgpmItem");
	m_pagsmCharacter = (AgsmCharacter *) GetModule("AgsmCharacter");

	if (!m_pagpmCharacter || !m_pagpmItem || !m_pagsmCharacter)
		return FALSE;

	// setting module data
	// SetModuleData();

	// setting attached data(m_nKeeping) in character module
	SetAttachedModuleData(1, AGSMTRADE_AD_CHARACTER, "AgpmCharacter", 0, sizeof(AgsdCharacter));

	// setting callback functions
	SetCallback("AgsmCharacter", CHARACTER_CB_ID_MOVE, CancelTrade, this);

	// setting serialize table
	stMODULEDISP	stModuleDisp[3] = { /*{ PACKET_TYPE, PACKET_ACTION, ProcessTrade },
		{ PACKET_TYPE, PACKET_ACTION, ProcessConnectDeal },*/
		{ 0, 0, NULL } };

	SetModuleDispatch(stModuleDisp);

	// setting update functions
	// SetUpdatesFunc();

	return TRUE;
}

BOOL AgsmTrade::OnInit()
{
	return TRUE;
}

BOOL AgsmTrade::OnDestroy()
{
	return TRUE;
}

BOOL AgsmTrade::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

//		OnValid
//	Functions
//		- �Ѿ�� �����Ͱ� ��ȿ���� �˻�
//			1. hack check
//			2. valid action check
//	Arguments
//		- pszData : ����Ÿ ������
//		- nSize : ����Ÿ ũ��
//	Return value
//		- FALSE�ΰ�� ������ �߸��Ȱ���...
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::OnValid(CHAR* pszData, INT16 nSize)
{
	/*
	if (m_csValidCheck.CheckHack())
		return FALSE;

	if (m_csValidCheck.CheckAvailAction())
		return FALSE;
	*/

	return TRUE;
}

/*
BOOL AgsmTrade::IsAvailableTrade(pstAPADMINOBJECTCXT pstObjectCxt)
{
	AgsdCharacter *pAgsdCharacter = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pstObjectCxt->pObject);

	if (pAgsdCharacter->m_fCharStatus == AGSDCHAR_FLAG_ALIVE)
		return TRUE;
	else
		return FALSE;
}
*/

//		StartTrade
//	Functions
//		- trade�� �������� �˻��Ѵ�.
//		- �����ϴٸ� ĳ���� ���¸� �ŷ� ���·� �ٲ۴�.
//	Arguments
//		- pChar1 : �ŷ��ϴ� ĳ����
//		- pChar2 : �ŷ��ϴ� ĳ����
//	Return value
//		- TRUE : �ŷ� ����
//		- FALSE : �ŷ� ���� ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::StartTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	BOOL	bRetval = FALSE;

	AgsdCharacter *pAgsdCharacter1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdCharacter2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 =
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 =
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);

	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	if (pAgsdCharacter1->m_fCharStatus == AGSDCHAR_FLAG_NORMAL &&
		pAgsdCharacter2->m_fCharStatus == AGSDCHAR_FLAG_NORMAL)
	{
		pAgsdCharacter1->m_fCharStatus = AGCDCHAR_FLAG_TRADE;
		pAgsdCharacter2->m_fCharStatus = AGCDCHAR_FLAG_TRADE;

		// �ŷ�â �ʱ�ȭ
		ZeroMemory(pAgsdTrade1, sizeof(AgsdTrade));
		ZeroMemory(pAgsdTrade2, sizeof(AgsdTrade));

		bRetval = TRUE;
	}

	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	return bRetval;
}

//		UpdateTrade
//	Functions
//		- �ŷ�â�� ������Ʈ �Ѵ�. 
//			(���⿡�� ���� ĳ���͵鰣�� ������ ������ �������� �ʴ´�. ���� ������ �Ϸ� ��ƾ���� ó���Ѵ�.)
//	Arguments
//		- pCharacter : �ŷ�â�� ������ ���� �÷����� ĳ����
//		- ulIID : ���� �÷����� ������ (���ΰ�� �̰��� �����Ѵ�)
//		- nPosInventory : �÷����� �������� �κ��丮 ��ġ (�ŷ�â �κ��丮 ��ġ)
//		- ulMoney : ���� �÷����� ��� �� �׼�
//	Return value
//		- TRUE : �ŷ�â ������Ʈ ����
//		- FALSE : �ŷ�â ������Ʈ ����
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::UpdateTrade(AgpdCharacter *pCharacter, UINT32 ulIID, INT16 nPosInventory, UINT32 ulMoney)
{
	AgsdCharacter	*pAgsdCharacter = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pCharacter);

	// ���� �ŷ� �������� �˻�
	pCharacter->m_Mutex.RLock();
	if (pAgsdCharacter->m_fCharStatus != AGCDCHAR_FLAG_TRADE)
	{
		pCharacter->m_Mutex.Release();
		return FALSE;
	}
	pCharacter->m_Mutex.Release();

	// �ŷ� �κ��丮 ������ �����´�.
	AgsdTrade		*pAgsdTrade = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pCharacter);

	if (ulIID != AGPMITEM_INVALID_IID)	// ���������� �ƴ� ������ ����
	{
		// ������ ���ø� ����Ÿ�� �����´�. (������ ũ�⸦ ���ϱ� ����)
		AgpdItemTemplate	*pItemTemplate;

		pItemTemplate = m_pagpmItem->GetItemTemplateUseIID(ulIID);

		if (CheckInventoryPos(pAgsdTrade, nPosInventory, pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH], pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]))
		{
			// �� ��ġ�� �������� �� �� �ִ�.
			AddInventory(pCharacter, pAgsdTrade, ulIID, nPosInventory, pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_WIDTH], pItemTemplate->m_nSizeInInventory[AGPDITEMTEMPLATESIZE_HEIGHT]);
		}
		else
		{
			// �������� �� �� ���� ��ġ��.
			return FALSE;
		}
	}
	else	// ����Ȱ� ���̶��
	{
		pCharacter->m_Mutex.WLock();

		pAgsdTrade->ulMoney = ulMoney;

		pCharacter->m_Mutex.Release();
	}

	return TRUE;
}

//		CheckInventoryPos
//	Functions
//		- �ŷ�â���� Ư�� ��ġ�� Ư�� ũ���� �������� �ö� �� �ִ��� �˻�
//	Arguments
//		- pAgsdTrade : �ŷ�â ����Ÿ
//		- nPosInventory : �÷����� �������� �κ��丮 ��ġ (�ŷ�â �κ��丮 ��ġ)
//		- pSize : ������ ũ�� (2���� �迭 ������)
//	Return value
//		- TRUE : �ö� �� �ִ�.
//		- FALSE : ���� ��°��
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CheckInventoryPos(AgsdTrade *pAgsdTrade, INT16 nPosInventory, INT16 nWidth, INT16 nHeight)
{
	INT16	nRow;
	INT16	nColumn;

	nColumn = nPosInventory / AGSMTRADE_MAX_INVENTORY_ROW;
	nRow = nPosInventory - (nColumn * AGSMTRADE_MAX_INVENTORY_ROW);

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			if (pAgsdTrade->ulTradeInventory[nColumn + i][nRow + j] != AGPMITEM_INVALID_IID)
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

//		AddInventory
//	Functions
//		- �ŷ�â�� �������� �÷� ���´�. (�ŷ�â �迭�� ������ �Ƶڸ� ������ ũ�⸸ŭ ���ڸ��� ä���ִ´�)
//	Arguments
//		- pCharacter : ���� ��ü
//		- pAgsdTrade : �ŷ�â ����Ÿ
//		- ulIID : ������ �Ƶ�
//		- nPosInventory : �÷����� �������� �κ��丮 ��ġ (�ŷ�â �κ��丮 ��ġ)
//		- pSize : ������ ũ�� (2���� �迭 ������)
//	Return value
//		- TRUE : �� �÷ȴ�.
//		- FALSE : ��° �� �ȿö󰣴�. ��°~
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::AddInventory(AgpdCharacter *pCharacter, AgsdTrade *pAgsdTrade, 
							 UINT32 ulIID, INT16 nPosInventory, INT16 nWidth, INT16 nHeight)
{
	INT16	nRow;
	INT16	nColumn;

	nColumn = nPosInventory / AGSMTRADE_MAX_INVENTORY_ROW;
	nRow = nPosInventory - (nColumn * AGSMTRADE_MAX_INVENTORY_ROW);

	pCharacter->m_Mutex.WLock();

	for (int i = 0; i < nHeight; i++)
	{
		for (int j = 0; j < nWidth; j++)
		{
			pAgsdTrade->ulTradeInventory[nColumn + i][nRow + j] = ulIID;
		}
	}

	pAgsdTrade->nNumItem++;

	pCharacter->m_Mutex.Release();

	return TRUE;
}

//		CompleteTrade
//	Functions
//		- �ŷ��� �Ϸ��Ѵ�.
//			(���� ������ �̵��� �̶� �̷�����.)
//	Arguments
//		- pChar1 : �ŷ��ϴ� ĳ����
//		- pChar2 : �ŷ��ϴ� ĳ����
//	Return value
//		- TRUE : ������ ����.
//		- FALSE : ���� ���д�
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CompleteTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	AgsdCharacter *pAgsdChar1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdChar2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);


	// �� ĳ���� ��� ��ٴ�. ���� ���� �� ��ٴ�.
	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	// �ŷ�â�� �ִ� ���۵��� ã�´�. ã�Ƽ� ������ �ٲ۴�.
	// ĳ���Ϳ� �پ� �ִ� ���� ������ ����Ʈ�� �����Ѵ�. (������ ��⿡�� �����Ѵ�)

	for (int i = 0; i < pAgsdTrade1->nNumItem; i++)
	{
		m_pagpmItem->ChangeItemOwner(pChar1, pChar2, pChar1->m_stCharacter.m_lID, pAgsdTrade1->ulItem[i]);
	}

	for (i = 0; i < pAgsdTrade2->nNumItem; i++)
	{
		m_pagpmItem->ChangeItemOwner(pChar2, pChar1, pChar1->m_stCharacter.m_lID, pAgsdTrade2->ulItem[i]);
	}

	// �� �ŷ��� �ִٸ� ó���Ѵ�.


	// ���¸� �ٲ۴�. ���� ���·� �ٲ۴�
	pAgsdChar1->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;
	pAgsdChar2->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// ĳ���� ��ٰ� Ǭ��.
	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	return TRUE;
}

//		CancelTrade
//	Functions
//		- �ŷ��� ����Ѵ�.
//			(���� ������ �̵��� �̷�� ������ ���ⶫ�� �� �ŷ� �����鸸 �ʱ�ȭ��Ų��.)
//	Arguments
//		- pChar1 : �ŷ��ϴ� ĳ����
//		- pChar2 : �ŷ��ϴ� ĳ����
//	Return value
//		- TRUE : ������ ����.
//		- FALSE : ���� ���д�
///////////////////////////////////////////////////////////////////////////////
BOOL AgsmTrade::CancelTrade(AgpdCharacter *pChar1, AgpdCharacter *pChar2)
{
	// �� ĳ���� ��� ��ٴ�. ���� ���� ��װ�
	// ����, �ŷ�â ����Ÿ ���� �ʱ�ȭ�Ѵ�.

	AgsdCharacter *pAgsdChar1 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar1);
	AgsdCharacter *pAgsdChar2 = 
		(AgsdCharacter *) m_pagsmCharacter->GetAttachedModuleData(AGSMC_AD_CHARACTER, pChar2);

	AgsdTrade *pAgsdTrade1 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar1);
	AgsdTrade *pAgsdTrade2 = 
		(AgsdTrade *) m_pagsmCharacter->GetAttachedModuleData(AGSMTRADE_AD_CHARACTER, pChar2);

	pChar1->m_Mutex.WLock();
	pChar2->m_Mutex.WLock();

	// ���� �ʱ�ȭ
	pAgsdChar1->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// �ŷ� ����Ÿ �ʱ�ȭ
	pAgsdTrade1->nNumItem = 0;
	pAgsdTrade1->ulMoney = 0;
	//ZeroMemory(pAgsdTrade1->ulItem, sizeof(UINT32) * AGSMTRADE_MAX_ITEM);
	pAgsdTrade1->ulItem.MemSetAll();
	ZeroMemory(pAgsdTrade1->ulTradeInventory, sizeof(UINT32) * AGSMTRADE_MAX_INVENTORY_COLUMN * AGSMTRADE_MAX_INVENTORY_ROW);

	// ���� �ʱ�ȭ
	pAgsdChar2->m_fCharStatus = AGSDCHAR_FLAG_NORMAL;

	// �ŷ� �ʱ�ȭ
	pAgsdTrade2->nNumItem = 0;
	pAgsdTrade2->ulMoney = 0;
	//ZeroMemory(pAgsdTrade2->ulItem, sizeof(UINT32) * AGSMTRADE_MAX_ITEM);
	pAgsdTrade2->ulItem.MemSetAll();
	ZeroMemory(pAgsdTrade2->ulTradeInventory, sizeof(UINT32) * AGSMTRADE_MAX_INVENTORY_COLUMN * AGSMTRADE_MAX_INVENTORY_ROW);

	pChar2->m_Mutex.Release();
	pChar1->m_Mutex.Release();

	// �ƹ��� ���ٴ� ���� �����Ѵ�.

	return TRUE;
}

//		ProcessTrade
//	Functions
//		- item(money) trade�� ó���Ѵ�.
//	Arguments
//		- ulType : packet type
//		- ulAction : action number
//		- pszData : data pointer
//		- nSize : data size
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmTrade::ProcessTrade(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize)
{
	INT16				nProcessStep;	// �ŷ� ���� �ܰ� ����
										// 1. �ŷ� ���� (AGSMTRADE_TRADE_START)
										// 2. ���� ���� (AGSMTRADE_INVENTORY_UPDATE)
										// 3. �ŷ� �Ϸ� (AGSMTRADE_TRADE_COMPLETE)
										// 4. �ŷ� ��� (AGSMTRADE_TRADE_CANCEL)

	UINT32				ulCID1, ulCID2;

	AgpdCharacter		*pTrader1, *pTrader2;

	// ��Ŷ�� �м��Ѵ�.
	// ulCID1 = ;
	// ulCID2 = ;
	// nProcessStep = ;

	// �ŷ��� ������ �����´�.
	pTrader1 = m_pagpmCharacter->GetCharacter(ulCID1);
	pTrader2 = m_pagpmCharacter->GetCharacter(ulCID2);

	BOOL bRetval;

	switch (nProcessStep) {
	case AGSMTRADE_TRADE_START:
		bRetval = StartTrade(pTrader1, pTrader2);

		// ��� ����
		// �ŷ�â�� ����.

		break;

	case AGSMTRADE_INVENTORY_UPDATE:
		UINT32	ulIID;
		INT16	nPosInventory;
		UINT32	ulMoney;

		// ���� ������ ����Ÿ�� ���Ѱ� ���� ���� Ŭ���̾�Ʈ���� ���̴°͸� �ŷ�â�� �÷������� ������ �κ����� �Ⱥ��̰� �Ѵ�.
		bRetval = UpdateTrade(pTrader1, ulIID, nPosInventory, ulMoney);

		// ��� ����

		break;

	case AGSMTRADE_TRADE_COMPLETE:
		bRetval = CompleteTrade(pTrader1, pTrader2);

		// �ʿ��� ����(���� ������)�� ����� �����Ѵ�.

		// ��� ����

		break;

	case AGSMTRADE_TRADE_CANCEL:
		bRetval = CancelTrade(pTrader1, pTrader2);
		
		// ��� ����

		break;

	default:
		break;
	}

	return 0;
}

//		ProcessConnectDeal
//	Functions
//		- deal ������ ������� �ش�.
//	Arguments
//		- ulType : packet type
//		- ulAction : action number
//		- pszData : data pointer
//		- nSize : data size
//	Return value
//		- BOOL : success or fail
///////////////////////////////////////////////////////////////////////////////
INT16 AgsmTrade::ProcessConnectDeal(UINT32 ulType, UINT32 ulAction, CHAR* pszData, INT16 nSize)
{
	return 0;
}

//		CancelTrade
//	Functions
//		- �������� �ŷ��� ������Ų��. (ĳ���Ͱ� �������ٴ��� �ϴ� ���)
//	Arguments
//		- pData : character data pointer (pstAPADMINOBJECTCXT)
//		- pClass : this class pointer
//	Return value
//		- void
///////////////////////////////////////////////////////////////////////////////
VOID AgsmTrade::CancelTrade(PVOID pData, PVOID pClass)
{
	AgpdCharacter *pCharacter = (AgpdCharacter *) pData;



	return;
}