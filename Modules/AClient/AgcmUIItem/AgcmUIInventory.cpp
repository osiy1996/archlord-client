#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmUIControl/AcUIGrid.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUISplitItem/AgcmUISplitItem.h>
#include <AgcmPrivateTrade/AgcmPrivateTrade.h>
#include <AgpmItemConvert/AgpmItemConvert.h>
#include <AgcmItemConvert/AgcmItemConvert.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmChatting2/AgcmChatting2.h>
#include <AgcmUIControl/AcUIButton.h>
#include <AgcmUIControl/AcUIEdit.h>

//���� �׸��峻���� �̵�������~
BOOL AgcmUIItem::CBInventoryMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;

	BOOL					bResult;

	bResult = FALSE;

	// ���� �������̶�� ������ �ϸ� �ȵȴ�.
	if (pThis->m_bIsProcessConvert)
		return TRUE;

	//���� ��ġ���� �̵��� Layer, Row, Column�� �������� �ű�ٴ� ��Ŷ�� ������.
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return bResult;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return bResult;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);

	if (pstGrid)
	{
		INT32			lItemID;
		AgpdItem		*pcsAgpdItem;

		lItemID = pstGridSelectInfo->pGridItem->m_lItemID;

		pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( lItemID );

		if( pcsAgpdItem != NULL )
		{
			//�ʵ忡�� �κ����� �Ű������.
			if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_FIELD )
			{
				bResult = pThis->m_pcsAgcmItem->PickupItem(pcsAgpdItem, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn);

				if( bResult )
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryMSGGetItem);
				}
			}
			//�κ����� �κ����� �Ű����� ���.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY )
			{
				// ���� �ڸ����� ���캻��.
				// ���� �ڸ��� �ƴѰ�츸 ������ ������.
				if (pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB] == pcsUIGrid->m_lNowLayer &&
					pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW] == pstGridSelectInfo->lGridRow &&
					pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN] == pstGridSelectInfo->lGridColumn)
				{
					AgpdItemTemplate *pcsAgpdItemTemplate = (AgpdItemTemplate *)(pcsAgpdItem->m_pcsItemTemplate);
					if( pcsAgpdItemTemplate )
					{
						AgcdUIMessageMap *	pstMessageMap = pcsControl->m_pstMessageMaps + 11;
						AuList <AgcdUIAction> *plistActions = &pstMessageMap->m_listAction;
						AgcdUIAction * pstAction = &((*plistActions)[plistActions->GetCount() - 1]);
						if( pstAction->m_eType == AGCDUI_ACTION_SOUND )
						{
							if( ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_ARMOUR )
							{
								pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIManager2->m_nEventArmourEquipSound );
							}
							else if( ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON 
								|| ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_SHIELD )
							{
								pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIManager2->m_nEventWeaponEquipSound );
							}
							else if( ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_NECKLACE
								|| ((AgpdItemTemplateEquip *)(pcsAgpdItemTemplate))->m_nKind == AGPMITEM_EQUIP_KIND_RING )
							{
								//�Ǽ��縮.
								pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_pcsAgcmUIManager2->m_nEventAccessorySound );
							}
						}
					}
					return TRUE;
				}

				// JNY 2005.6.28
				// GetAsyncKeyState�� Release �������� ������ ���ܼ� g_pEngine->IsShiftDown()�� ����
				AgcdUIHotkeyType eHotKeyType;
				if (FALSE == g_pEngine->IsShiftDown())
					eHotKeyType = AGCDUI_HOTKEY_TYPE_NORMAL;
				else
					eHotKeyType = AGCDUI_HOTKEY_TYPE_SHIFT;

				BOOL	bProcess	= FALSE;

				if (eHotKeyType == AGCDUI_HOTKEY_TYPE_SHIFT)
				{
					// ���ڸ����� ����.
					if (pThis->m_pcsAgpmGrid->IsEmptyGrid(pstGrid, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn, 1, 1) &&
						pcsAgpdItem->m_pcsItemTemplate->m_bStackable)
					{
						pThis->m_pcsAgpdSplitItem	= pcsAgpdItem;

						pThis->m_lSplitItemTargetLayer		= pcsUIGrid->m_lNowLayer;
						pThis->m_lSplitItemTargetRow		= pstGridSelectInfo->lGridRow;
						pThis->m_lSplitItemTargetColumn		= pstGridSelectInfo->lGridColumn;

						pThis->m_pcsAgcmUISplitItem->SetCondition(SplitItem_Item, pThis->CBSplitItemInventory, pThis, 1, pcsAgpdItem->m_nCount, 0);

						pThis->m_pcsAgcmUISplitItem->OpenSplitItemUI();

						bProcess	= TRUE;
					}
				}

				if (!bProcess)
					bResult = pThis->m_pcsAgcmItem->SendInventoryInfo( lItemID, pcsAgpdItem->m_ulCID, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );
			}
			// Sub Inventory ���� �κ����� �Ű������.
			else if(pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY)
			{
				bResult = pThis->m_pcsAgcmItem->SendInventoryInfo( lItemID, pcsAgpdItem->m_ulCID, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );
			}
			//Equip���� �κ����� �Ű������.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_EQUIP )
			{
				// ���� �� �ִ� ���������� �ѹ� üũ�Ѵ�.
				if( !pThis->IsEnableUnEquipItem( pcsAgpdItem ) ) return FALSE;
				bResult = pThis->m_pcsAgcmItem->SendInventoryInfo( lItemID, pcsAgpdItem->m_ulCID, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );
			}
			//Bank���� �κ����� �Ű������.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_BANK )
			{
				bResult = pThis->m_pcsAgcmItem->SendInventoryInfo( lItemID, pcsAgpdItem->m_ulCID, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );
			}
			//Trade Grid���� �κ����� �Ű������.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID )
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeRemoveItemFromTradeGrid( pcsAgpdItem->m_ulCID, lItemID, 
					pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
					pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
					pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
					pcsUIGrid->m_lNowLayer, 
					pstGridSelectInfo->lGridRow, 
					pstGridSelectInfo->lGridColumn,
					AGPDITEM_STATUS_INVENTORY);

			}
			//SalesBox���� �κ����� �Ű������.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SALESBOX_GRID )
			{
				if( pcsAgpdItem )
				{
					pThis->m_pcsCancelItemInfo = pcsAgpdItem;

					pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lSalesBoxCancelMessageBox );
				}
			}
			//NPC �ŷ�â���� �κ����� �Ű������.
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_NPC_TRADE )
			{
				pThis->m_lNPCTradeBuyItemID = lItemID;
				pThis->m_nNPCTradeBuyStatus = AGPDITEM_STATUS_INVENTORY;

				if (pThis->m_pcsAgpmGrid->IsEmptyGrid(pstGrid, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn, 1, 1))
				{
					bResult = pThis->OnOpenMsgBoxConfirmBuyNPCTradeItem( pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );
				}
			}
			// from guild warehouse
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_GUILD_WAREHOUSE)
			{
				INT32 lStatus = (INT32)AGPDITEM_STATUS_INVENTORY;

				// toss to AgcmUIGuildWarehouse
				PVOID pvBuffer[4];
				pvBuffer[0] = &pcsUIGrid->m_lNowLayer;
				pvBuffer[1] = &pstGridSelectInfo->lGridRow;
				pvBuffer[2] = &pstGridSelectInfo->lGridColumn;
				pvBuffer[3] = &lStatus;
				pThis->EnumCallback(AGCMUIITEM_CB_MOVED_FROM_GUILDWAREHOUSE, pcsAgpdItem, pvBuffer);
			}
			else if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SIEGEWAR_OBJECT)
			{
				bResult = pThis->m_pcsAgcmItem->SendInventoryInfo( lItemID, pcsAgpdItem->m_ulCID, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn );

				pThis->EnumCallback(AGCMUIITEM_CB_REMOVE_SIEGEWAR_GRID, pcsAgpdItem, NULL);
			}
			else if (pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_CASH_INVENTORY)
			{
				AgpdItem	*pcsAgpdExistItem = pThis->m_pcsAgpmItem->GetItem(pThis->m_pcsAgpmGrid->GetItem(pstGrid, pcsUIGrid->m_lNowLayer, pstGridSelectInfo->lGridRow, pstGridSelectInfo->lGridColumn));
				if (pcsAgpdExistItem)
				{
					if (AGPMITEM_TYPE_USABLE == pcsAgpdItem->m_pcsItemTemplate->m_nType)
					{
						if (AGPMITEM_USABLE_TYPE_RUNE == ((AgpdItemTemplateUsable *) pcsAgpdItem->m_pcsItemTemplate)->m_nUsableItemType)	// ������� ��츸 ����� ������ ���� ���ȴ�.
							pThis->m_pcsAgcmItemConvert->SendCheckCashRuneConvert(pThis->m_pcsAgcmCharacter->GetSelfCID(), pcsAgpdExistItem->m_lID, lItemID);
						else if (AGPMITEM_USABLE_TYPE_SOCKET_INIT == ((AgpdItemTemplateUsable *) pcsAgpdItem->m_pcsItemTemplate)->m_nUsableItemType)		// ���� �ʱ�ȭ�� ���� ���
						{
							// check initializable
							if (FALSE == pThis->m_pcsAgpmItemConvert->IsSocketInitializable(pcsAgpdExistItem))
							{
								pThis->m_pcsAgcmChatting2->AddSystemMessage(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_NOT_SOCKET_INITIALIZABLE));
								return FALSE;
							}

							// confirm
							if (IDOK != pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog(pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_ITEM_SOCKET_INIT_CONFIRM)))
								return FALSE;

							bResult = pThis->m_pcsAgcmItemConvert->SendSocketInitialize(pThis->m_pcsAgcmCharacter->GetSelfCID(), pcsAgpdExistItem->m_lID, lItemID);
						}
					}
				}
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventoryMoveEndItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *			pThis;
	AgcdUIControl *			pcsControl;
	AcUIGrid *				pcsUIGrid;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGridItem *			pcsGridItem;

	if (!pClass || !pData1)
		return FALSE;
	pThis		= (AgcmUIItem *)pClass;
	pcsControl	= (AgcdUIControl *)pData1;
	pcsUIGrid	= (AcUIGrid *)pcsControl->m_pcsBase;
	if (!pcsUIGrid)
		return FALSE;
	pstGridSelectInfo = pcsUIGrid->GetGridItemClickInfo();
	if (!pstGridSelectInfo || !pstGridSelectInfo->pGridItem)
		return FALSE;
	pcsGridItem = pstGridSelectInfo->pGridItem;
	if (pThis->m_pcsAgcmUIManager2->IsMainWindow(
		pstGridSelectInfo->pTargetWindow)) {
		AgpdItem * pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem(
			pcsGridItem->m_lItemID);

		if (!pcsAgpdItem)
			return FALSE;
		if (pcsAgpdItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_EQUIP) {
			AgpdItemTemplateEquip * equip = 
				(AgpdItemTemplateEquip *)pcsAgpdItem->m_pcsItemTemplate;

			if (equip->m_nPart == AGPMITEM_PART_ACCESSORY_NECKLACE ||
				equip->m_nPart == AGPMITEM_PART_ACCESSORY_RING1 ||
				equip->m_nPart == AGPMITEM_PART_ACCESSORY_RING2)
				return FALSE;
		}
		if (pcsAgpdItem->m_lStatusFlag & AGPMITEM_STATUS_CANNOT_BE_SOLD)
			return FALSE;
		pThis->m_pcsAgpdDestroyItem	= pcsAgpdItem;
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_lEventConfirmDestroyItem);
	}
	return TRUE;
}

//���������� ����~ ���������� �ݴ´�.
BOOL AgcmUIItem::CBInventoryUIUpdate(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( pThis->m_bUIOpenInventory )
		{
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lInventoryUIClose );
			pThis->m_bUIOpenInventory = FALSE;

			if( pThis->m_bUIOpenNPCTrade )
			{
				bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeCloseUI );
				pThis->m_bUIOpenNPCTrade = FALSE;
			}

			// â�� ���� �� �������� ���� ���� ���̸� ����Ѵ�, kelovon, 20050920
			pThis->m_pcsAgcmItem->CancelCoolDown();
		}
		else
		{
			pThis->OpenInventory();
		}
	}

	return bResult;
}

BOOL	AgcmUIItem::OpenInventory()
{
	BOOL	bResult = FALSE;

	AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if( !m_bUIOpenInventory && pcsSelfCharacter )
	{
		// Self Character�� ������Ʈ�� Ȯ���Ѵ�.
		if( pcsSelfCharacter->IsDead() )
		{
			// do nothing..
		}
		else
		{
			bResult = m_pcsAgcmUIManager2->ThrowEvent( m_lInventoryUIOpen );
			m_bUIOpenInventory = TRUE;
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBInventoryOpenInvenLayer(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *		pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *		pcsGridControl = (AgcdUIControl *) pData1;
	AgcdUIUserData *	pstUserData = NULL;

	if (pcsGridControl)
	{
		pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
		if (pstUserData && pstUserData->m_lSelectedIndex != -1)
			((AcUIGrid *) pcsGridControl->m_pcsBase)->SetNowLayer(pstUserData->m_lSelectedIndex);
	}

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventory );
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsInventoryLayerIndex );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayInventoryMoney(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	if (!pData ||
		eType != AGCDUI_USERDATA_TYPE_UINT64 ||
		lID != AGCMUI_ITEM_DISPLAY_ID_INVENTORY_MONEY ||
		!szDisplay)
		return FALSE;

	sprintf(szDisplay, "%I64d", *(INT64 *) pData);

	return TRUE;
}

BOOL AgcmUIItem::CBUpdateInventoryMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis				= (AgcmUIItem *)	pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if (!pcsSelfCharacter)
		return FALSE;

	if (pcsSelfCharacter != pcsCharacter)
		return TRUE;

	pThis->m_pcsAgpmCharacter->GetMoney(pcsCharacter, &pThis->m_llInventoryMoney);

	pThis->m_pcsInventoryMoney->m_stUserData.m_pvData = &pThis->m_llInventoryMoney;

	if (!pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsInventoryMoney))
		return FALSE;

	return TRUE;
}

BOOL AgcmUIItem::CBUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIItem			*pThis			= (AgcmUIItem *)	pClass;

	AcUIGrid*	pGrid = (AcUIGrid*)( pcsSourceControl->m_pcsBase );

	AgpdGridSelectInfo	*pcsSelectInfo	= pGrid->GetGridItemClickInfo();
	if (!pcsSelectInfo || !pcsSelectInfo->pGridItem || pcsSelectInfo->pGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
		return FALSE;

	AgpdItem			*pcsItem		= pThis->m_pcsAgpmItem->GetItem(pcsSelectInfo->pGridItem->m_lItemID);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (pThis->m_bUIOpenNPCTrade)
	{
		pThis->m_lNPCTradeSellItemID = pcsItem->m_lID;

		return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
	}
	else
	{
		if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType != AGPMITEM_TYPE_USABLE)
			return FALSE;

		return pThis->m_pcsAgcmItem->UseItem(pcsItem);
	}

	return TRUE;
}

//Inventory MSG----------------------------------------------------------------------------------------------------------

BOOL AgcmUIItem::CBUIInventoryFull( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryMSGFull);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIInventoryNotEnoughSpaceToGetGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryMSGNotEnoughSpaceToGetGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIInventoryDropItem( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	return TRUE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( lTrueCancel )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( pThis->m_lInventoryItemID );

			if( pcsAgpdItem != NULL )
			{
				bResult = pThis->m_pcsAgcmItem->DropItem( pcsAgpdItem );
			}
		}

	}

	return bResult;
}

BOOL AgcmUIItem::CBUIInventoryDropGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	return TRUE;

	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryMSGDropGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIInventoryDropGhelld( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	return TRUE;
}

BOOL AgcmUIItem::CBDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass ||
		!pcsSourceControl ||
		!pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;
	AcUIButton			*pcsUIButton		= (AcUIButton *)			pcsSourceControl->m_pcsBase;

	AcUIGridItem		*pcsDragDropItem	= pcsUIButton->GetDragDropGridItem();
	if (!pcsDragDropItem || !pcsDragDropItem->m_ppdGridItem)
		return FALSE;

	AgpdGridItem		*pcsGridItem		= pcsDragDropItem->m_ppdGridItem;
	if (pcsGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM ||
		pcsGridItem->m_lItemID == AP_INVALID_IID)
		return FALSE;

	INT32				lLayerIndex			= pcsSourceControl->m_lUserDataIndex;

	AgpdItem			*pcsAgpdItem		= pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (pcsAgpdItem &&
		pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_TRADE_GRID)
	{
		return FALSE;
	}

	return pThis->m_pcsAgcmItem->SendInventoryInfo(pcsGridItem->m_lItemID, pThis->m_pcsAgcmCharacter->GetSelfCID(), lLayerIndex);
}

BOOL AgcmUIItem::CBDropMoneyToField(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;

	//	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryDropMoneyUIOpen);
	return TRUE;
}

BOOL AgcmUIItem::CBDropMoneyToFieldYes(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis				= (AgcmUIItem *)		pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	const CHAR			*szDropGhelldCount	= ((AcUIEdit *) (pcsCountControl->m_pcsBase))->GetText();
	if (!szDropGhelldCount ||
		!szDropGhelldCount[0])
		return FALSE;

	INT32			lGhelldCount		= atoi(szDropGhelldCount);
	if (lGhelldCount <= 0)
		return FALSE;

	pThis->m_pcsAgcmItem->SendDropMoneyToField( pThis->m_pcsAgcmCharacter->m_lSelfCID, lGhelldCount );

	((AcUIEdit *) (pcsCountControl->m_pcsBase))->SetText( "" );

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryDropMoneyUIClose);
}

BOOL AgcmUIItem::CBDropMoneyToFieldCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	((AcUIEdit *) (pcsCountControl->m_pcsBase))->SetText( "" );

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryDropMoneyUIClose);
}

BOOL AgcmUIItem::CBReturnConfirmCancelItemFromAuction(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage)
{
	if (!pClass)
		return FALSE;

	if (lTrueCancel == (INT32) TRUE)
	{
		AgcmUIItem	*pThis			= (AgcmUIItem *)pClass;

		if( pThis->m_pcsCancelItemInfo )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = pThis->m_pcsCancelItemInfo;

			//��ҽ�ȣ�� ������.
			//pThis->m_pcsAgcmAuction->SendCancel( pcsAgpdItem->m_ulCID, pcsAgpdItem->m_lID, pcsAgpdItem->m_anGridPos[0], pcsAgpdItem->m_anGridPos[1], pcsAgpdItem->m_anGridPos[2] );
		}
	}

	return TRUE;	
}

BOOL AgcmUIItem::CBSplitItemInventory(PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;
	INT32				lStackCount			= *(INT32 *)				pData;
	INT32				lStatus				= pCustData ? *(INT32 *)pCustData : AGPDITEM_STATUS_INVENTORY;

	if (!pThis->m_pcsAgpdSplitItem)
		return FALSE;

	return pThis->m_pcsAgcmItem->SplitItem(pThis->m_pcsAgpdSplitItem->m_lID, lStackCount, (AgpdItemStatus)lStatus,
		pThis->m_lSplitItemTargetLayer,
		pThis->m_lSplitItemTargetRow,
		pThis->m_lSplitItemTargetColumn);
}

BOOL AgcmUIItem::CBWasteDragDropItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *	pThis;
	AcUIButton *	pcsUIButton;
	AcUIGridItem *	pcsDragDropItem;
	AgpdGridItem *	pcsGridItem;
	AgpdItem *		pcsAgpdItem;

	if (!pClass ||
		!pcsSourceControl ||
		!pcsSourceControl->m_pcsBase)
		return FALSE;
	pThis = (AgcmUIItem *)pClass;
	pcsUIButton = (AcUIButton *)pcsSourceControl->m_pcsBase;
	pcsDragDropItem	= pcsUIButton->GetDragDropGridItem();
	if (!pcsDragDropItem || !pcsDragDropItem->m_ppdGridItem)
		return FALSE;
	pcsGridItem = pcsDragDropItem->m_ppdGridItem;
	if (pcsGridItem->m_eType != AGPDGRID_ITEM_TYPE_ITEM ||
		pcsGridItem->m_lItemID == AP_INVALID_IID)
		return FALSE;
	pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsAgpdItem)
		return FALSE;
	if (pcsAgpdItem->m_lStatusFlag & AGPMITEM_STATUS_CANNOT_BE_SOLD)
		return TRUE;
	if (AGPDITEM_CASH_ITEM_UNUSE != pcsAgpdItem->m_nInUseItem) {
		char * pMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "DestoryUseItem" );
		pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pMessage ? pMessage : "������� �������� �����Ҽ� �����ϴ�" );
		return TRUE;
	}
	switch( pcsAgpdItem->m_eStatus ) {
	case AGPDITEM_STATUS_INVENTORY:
	case AGPDITEM_STATUS_SUB_INVENTORY:
		pThis->m_pcsAgpdDestroyItem	= pcsAgpdItem;
		return pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_lEventConfirmDestroyItem);
	case AGPDITEM_STATUS_CASH_INVENTORY: {
			char * pMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(
				"DestoryCashItem");
			char szText[256];

			sprintf(szText, "%s %s", 
				pcsAgpdItem->m_pcsItemTemplate->m_szName, pMessage);
			if (IDOK == pThis->m_pcsAgcmUIManager2->ActionMessageOKCancelDialog(szText)) {
				pThis->m_pcsAgpdDestroyItem	= pcsAgpdItem;
				return pThis->m_pcsAgcmUIManager2->ThrowEvent(
					pThis->m_lEventConfirmDestroyCashItem);
			}
		}
	default:
		break;
	}
	return FALSE;
}

BOOL AgcmUIItem::CBUIInventoryDestroyItem( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	if (!pClass)
		return FALSE;

	AgcmUIItem	*pThis = (AgcmUIItem *)	pClass;

	if (lTrueCancel &&
		pThis->m_pcsAgpdDestroyItem)
	{
		return pThis->m_pcsAgcmItem->SendRequestDestroyItem(pThis->m_pcsAgcmCharacter->GetSelfCID(), pThis->m_pcsAgpdDestroyItem->m_lID);
	}

	return TRUE;
}