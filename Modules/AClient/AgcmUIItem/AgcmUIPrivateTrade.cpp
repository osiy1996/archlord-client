#include <AgcmUIItem/AgcmUIItem.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmPrivateTrade/AgcmPrivateTrade.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmUIManager2/AgcUIWindow.h>
#include <AgcmUIControl/AcUIGrid.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcModule/AgcEngine.h>

BOOL AgcmUIItem::CBClosePrivateTradeUI(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;
	INT32			lCID;

	bResult = FALSE;

	if( pThis != NULL )
	{
		//���� PrivateTradeâ�� �����־��ٸ� Cancel��Ŷ�� ������ UI�� �ݴ´�. ����~
		if( pThis->m_pcsAgcmCharacter )
		{
			lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

			if( lCID != 0 )
			{
				//�ŷ��� ������ִ°�?
				if( pThis->m_pcsPrivateTradeSetClientNameChar )
				{
					pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );
				}

				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBDisplayPrivateTraderName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_PRIVATETRADERNAME || !szDisplay)
		return FALSE;

	pThis->m_pcsPrivateTradeSetClientNameChar = pcsAgpdCharacter;

	//sprintf(szDisplay, "%s�԰� �ŷ��Ͻðڽ��ϱ�?", pcsAgpdCharacter->m_szID );
	sprintf(szDisplay, ClientStr().GetStr(STI_WANT_TRADE), pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayRequestPrivateTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_REQUEST_PRIVATETRADE || !szDisplay)
		return FALSE;

	pThis->m_pcsPrivateTradeSetClientNameChar = pcsAgpdCharacter;

	//sprintf(szDisplay, "%s���� ������ ��ٸ��� ���Դϴ�.", pcsAgpdCharacter->m_szID );
	sprintf(szDisplay, ClientStr().GetStr(STI_WAIT_TRADE), pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeSetName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_SET_NAME || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "[%s]", pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeSetClientName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgpdCharacter *pcsAgpdCharacter = (AgpdCharacter *)pData;

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_CHARACTER || lID != AGCMUI_ITEM_DISPLAY_ID_SET_CLIENTNAME || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "[%s]", pcsAgpdCharacter->m_szID );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lGhelld = *((INT32 *)pData);

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || lID != AGCMUI_ITEM_DISPLAY_ID_PT_GHELLD || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "%d", lGhelld );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeClientGhelld(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lGhelld = *((INT32 *)pData);

	if ( !pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || lID != AGCMUI_ITEM_DISPLAY_ID_PT_CLIENTGHELLD || !szDisplay)
		return FALSE;

	sprintf(szDisplay, "%d", lGhelld );

	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradeSelfCC(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *	pThis = (AgcmUIItem *) pClass;
	INT32			cc = *((INT32 *)pData);

	if (!pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || 
		lID != AGCMUI_ITEM_DISPLAY_ID_PT_SELF_CC || !szDisplay)
		return FALSE;
	sprintf(szDisplay, "%d", cc);
	return TRUE;
}

BOOL AgcmUIItem::CBDisplayPrivateTradePeerCC(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	AgcmUIItem *	pThis = (AgcmUIItem *) pClass;
	INT32			cc = *((INT32 *)pData);

	if (!pThis || !pData || eType != AGCDUI_USERDATA_TYPE_INT32 || 
		lID != AGCMUI_ITEM_DISPLAY_ID_PT_PEER_CC || !szDisplay)
		return FALSE;
	sprintf(szDisplay, "%d", cc);
	return TRUE;
}

BOOL AgcmUIItem::CBUIOpenPrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgpdCharacter	*pcsAgpdCharacter;
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	INT32			lRequesterCID = *((INT32 *)pData);

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		pThis->m_lPrivateTradeRequesterCID = lRequesterCID;

		pcsAgpdCharacter = pThis->m_pcsAgpmCharacter->GetCharacter( lRequesterCID );

		if( pcsAgpdCharacter != NULL )
		{
			pThis->m_pcsPrivateTraderName->m_stUserData.m_pvData = pcsAgpdCharacter;

			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeOpenUIYesNo);

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTraderName);

		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIClosePrivateTradeYesNo( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIOpenPrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;
	INT32			lCTargetID;

	bResult = FALSE;

	if( (pThis != NULL) && (pData != NULL) )
	{
		lCTargetID = *((INT32 *)pData);

		//PrivateTradeâ�� �߸� ��ûâ�� ������ ������.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);

		//�κ��丮�� �������� ��ư�� ��Ȱ��ȭ��Ų��.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryDisableDropGhelld );

		if( pThis->m_bUIOpenPrivateTrade == FALSE )
		{
			// �ŷ��� �� �� ����UI�� �����ϰ� ��� �ݾ��ش�.
			//pThis->m_pcsAgcmUIManager2->CloseAllUIExceptMainUI(NULL);

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeOpenUI);
			pThis->m_bUIOpenPrivateTrade = TRUE;
		}

		pThis->OpenInventory();

		// �ŷ�â Grid �ʱ�ȭ
		pThis->m_pcsAgpmGrid->Reset( &pThis->m_PrivateTradeGridEx );
		pThis->m_pcsAgpmGrid->Reset( &pThis->m_PrivateTradeClientGridEx );

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeEx	);
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsPrivateTradeClientEx );

		AgpdCharacter		*pcsAgpdTargetChar;

		pcsAgpdTargetChar = pThis->m_pcsAgpmCharacter->GetCharacter( lCTargetID );

		if( pcsAgpdTargetChar != NULL )
		{
			AgpdItemADChar		*pcsAgpdItemADTargetChar;

			//���� �̸�, �ŷ����� �̸��� ��������~ �ȳ�~
			pThis->m_pcsPrivateTradeSetName->m_stUserData.m_pvData = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			pThis->m_pcsPrivateTradeSetClientName->m_stUserData.m_pvData = pcsAgpdTargetChar;

			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeSetName);
			pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeSetClientName);
			
			pcsAgpdItemADTargetChar = pThis->m_pcsAgpmItem->GetADCharacter(pcsAgpdTargetChar);

			if( pcsAgpdItemADTargetChar != NULL )
			{
				pThis->m_pcsPrivateTradeClientGrid = &pcsAgpdItemADTargetChar->m_csTradeGrid;
				pThis->m_pcsPrivateTradeClient->m_stUserData.m_pvData	= pThis->m_pcsPrivateTradeClientGrid;
				pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClient);

				bResult = TRUE;
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBUIClosePrivateTrade( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
//		if( pThis->m_bUIOpenPrivateTrade )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lPrivateTradeCloseUI );
			pThis->m_bUIOpenPrivateTrade = FALSE;
		}

		//�κ��丮�� �������� ��ư�� Ȱ��ȭ��Ų��.
		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryEnableDropGhelld );

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		if( pThis->m_bUIOpenSubInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lSubInventoryUIClose);
			pThis->m_bUIOpenSubInventory = FALSE;
		}

		// �ŷ� Ȯ�� ���̾�αװ� �������� �ݴ´�.
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lRequestPrivateTradeUIClose );

		pThis->m_pcsPrivateTradeSetNameChar = NULL;
		pThis->m_pcsPrivateTradeSetClientNameChar = NULL;

		//���� 0���� �ٲ۴�.
		pThis->m_lPrivateTradeClientGhelld = 0;
		pThis->m_lPrivateTradeGhelld = 0;
		pThis->m_privateTradePeerCC = 0;
		pThis->m_privateTradeSelfCC = 0;

		pThis->m_pcsPrivateTradeGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeGhelld);
		pThis->m_pcsPrivateTradeClientGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeClientGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClientGhelld);
		//pThis->m_privateTradePeerCCUD->m_stUserData.m_pvData = &pThis->m_privateTradePeerCC;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_privateTradePeerCCUD);
		//pThis->m_privateTradeSelfCCUD->m_stUserData.m_pvData = &pThis->m_privateTradeSelfCC;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_privateTradeSelfCCUD);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBRequestCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmPrivateTrade != NULL && pThis->m_pcsAgcmCharacter )
		{
			if( pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter )
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter->m_lID );

				//UIâ�� ����.
				if( bResult )
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);
				}
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeAccept(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	AgcdUI*		pUI	=	pThis->m_pcsAgcmUIManager2->GetUI( "UI_Auction" );

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmPrivateTrade != NULL && pThis->m_pcsAgcmCharacter != NULL)
		{
			AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_PRVTRADE)
				|| pThis->m_pcsAgpmCharacter->HasPenalty(pcsSelfCharacter, AGPMCHAR_PENALTY_AUCTION)
				|| pUI->m_pcsUIWindow->m_bOpened	)
			{

				if( pUI->m_pcsUIWindow->m_bOpened )
				{
					char* pMsg = pThis->m_pcsAgcmUIManager2->GetUIMessage( "CannotAuctionInPrivateTrade" );
					if( pMsg && strlen( pMsg ) > 0 )
					{
						pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pMsg );
					}

					return FALSE;
				}

				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( pcsSelfCharacter->m_lID );
			}
			else
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeRequestConfirm( pThis->m_lPrivateTradeRequesterCID );
			}

			//UIâ�� ����.
			if( bResult )
			{
				bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeReject(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;
	INT32			lCID;

	if( pThis != NULL )
	{
		if( pThis->m_pcsAgcmCharacter )
		{
			lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

			if( lCID != 0 )
			{
				bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );

				//UIâ�� ����.
				if( bResult )
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeCloseUIYesNo);
				}
			}
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem					*pThis				=	static_cast< AgcmUIItem		* >(pClass);
	AgcdUIControl				*pcsControl			=	static_cast< AgcdUIControl	* >(pData1);
	AgpdGridSelectInfo			*pstGridSelectInfo	=	NULL;
	AgpdGrid					*pstGrid			=	NULL;
	AcUIGrid					*pcsUIGrid			=	NULL;
	AgpdItem					*pcsAgpdItem		=	NULL;
	eAGPM_PRIVATE_TRADE_ITEM	eType				=	AGPM_PRIVATE_TRADE_ITEM_NORMAL;
	AgpdItemTemplateUsable *	pcsItemUsable		= NULL;

	INT32						nItemID				=	0;

	//���� ��ġ���� �̵��� Layer, Row, Column�� �������� �ű�ٴ� ��Ŷ�� ������.
	if( !pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID) )
		return FALSE;

	pcsUIGrid	=	static_cast< AcUIGrid* >(pcsControl->m_pcsBase);

	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return FALSE;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if ( !pstGrid )
		return FALSE;

	nItemID = pstGridSelectInfo->pGridItem->m_lItemID;

	pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( nItemID );
	if( pcsAgpdItem == NULL )
		return FALSE;
	
	// �κ����� �Ű����� �������� �ƴϸ� ����
	if( pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY &&
		pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY &&
		pcsAgpdItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY)
	{
		return FALSE;
	}

	
	if( strcmp( pcsControl->m_szName , UI_TRADE_SUPPORT_GRID_NAME) == 0 )
	{
		// ���� �׸��忡 ������������ �ø��� �ʾҴٸ� return FALSE
		if( pThis->m_pcsAgpmItem->GetUsableType( pcsAgpdItem ) != AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION )
			return FALSE;

		else
		{
			// ������ �������� �Ѱ����� �ø��� �ִ�
			if( !pThis->_CheckSupportItemDuplicate( pcsAgpdItem ) )
				return FALSE;

			eType		=	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER_ENABLE_TRADE;
		}
	}

	// �ͼ� �������� ����ũ���� �ö������� �Ѱ��� �ŷ� d�����ϴ�
	else if ( pThis->m_pcsAgpmItem->IsBoundOnOwner(pcsAgpdItem) || pThis->m_pcsAgpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND )
	{
		eType		=	AGPM_PRIVATE_TRADE_ITEM_BOUND_ON_OWNER;

		// ����ũ���� �ö�ͼ� �ŷ��� �������� Ȯ��( �ͼ� �������� �̹� �ö�� �ִ����� üũ )
		if( !pThis->_CheckBoundItemReleaseTradeCheck( pcsAgpdItem ) )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTradeFailForBoundItem);
			return FALSE;
		}

	}

#ifdef _AREA_GLOBAL_
	if ( strcmp( pcsControl->m_szName , UI_TRADE_SUPPORT_GRID_NAME) != 0 &&
		IS_CASH_ITEM( pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType ) )
	{
		if( !AgcmUIItem::IsCashItemByPPCard( pcsAgpdItem ) )
		{
			AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )pThis->GetModule( "AgcmUIManager2" );
			if( pcmUIManager )
			{
				char* pMessage = pcmUIManager->GetUIMessage( "CannotTradeCashItem" );
				if( pMessage && strlen( pMessage ) > 0 )
				{
					pcmUIManager->ActionMessageOKDialog( pMessage );
					return FALSE;
				}
			}
		}
	}
	if (pcsAgpdItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE)
		pcsItemUsable = (AgpdItemTemplateUsable *)pcsAgpdItem->m_pcsItemTemplate;
	if (IS_CASH_ITEM(pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType) &&
		pcsAgpdItem->m_lCashItemUseCount > 0 &&
		!pThis->m_pcsAgpmItem->IsUsingStamina(pcsAgpdItem->m_pcsItemTemplate) &&
		(!pcsItemUsable || pcsItemUsable->m_nUsableItemType != 
			AGPMITEM_USABLE_TYPE_PRIVATE_TRADE_OPTION)) {
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_lEventTradeFailForUsingCashItem);
		return FALSE;
	}
#else
	if( IS_CASH_ITEM( pcsAgpdItem->m_pcsItemTemplate->m_eCashItemType ) )
	{
		if( pcsAgpdItem->m_lCashItemUseCount > 0 )
		{
			// �ѹ��̶� ����� ĳ������ �ŷ��� �� ����.
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTradeFailForUsingCashItem);
			return FALSE;
		}

		if(	pThis->m_pcsAgpmItem->IsUsingStamina( pcsAgpdItem->m_pcsItemTemplate ) && pcsAgpdItem->m_llStaminaRemainTime <= 0 )
		{
			// �������� ���ѽð��� �ִ� �������ε� �������� ���ѽð��� ������ ������ �ŷ��� �� ����.
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventTradeFailForUsingCashItem);
			return FALSE;
		}
	}
#endif
	// ������� �������� �ŷ� �� �� ����
	if( pThis->m_pcsAgpmItem->IsUsingItem( pcsAgpdItem ) )
	{
		SystemMessage.ProcessSystemMessage( pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_USE_ITEM_TRANDE_FAILED) , 0 , AGPMSYSTEMMESSAGE_PACKET_MODAL_MESSAGE );
		return FALSE;
	}

	// Trade Option �׸��忡 �ö󰡴� �������� �ƴ϶�� �̺�Ʈ ���������� 
	// �˻��Ѵ�( �̺�Ʈ �������� �ŷ� �Ұ� )
	if( strcmp( pcsControl->m_szName , UI_TRADE_MAIN_GRID_NAME) == 0 )
	{

		// �ͼ��̸鼭 �̺�Ʈ �������� �ŷ� �� �� ���� !!!
		if ( pThis->m_pcsAgpmItem->IsBoundOnOwner(pcsAgpdItem) || pThis->m_pcsAgpmItem->GetBoundType(pcsAgpdItem) != E_AGPMITEM_NOT_BOUND )
		{
			if( pThis->m_pcsAgpmItem->IsEventItem( pcsAgpdItem ) )
				return FALSE;
		}
	}

	// �ŷ� �� ����������
	if( pThis->m_pcsAgpmItem->IsTradeBlockItem( pcsAgpdItem ) )
	{
		CHAR * msg = pThis->m_pcsAgcmUIManager2->GetUIMessage( "NotTradeThisItem" );

		if( msg )
		{
			pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( msg );
		}

		return FALSE;
	}

	return  pThis->m_pcsAgcmPrivateTrade->SendTradeAddItemToTradeGrid(	eType , pcsAgpdItem->m_ulCID, nItemID, 
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_TAB],
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_ROW],
																		pcsAgpdItem->m_anGridPos[AGPDITEM_GRID_POS_COLUMN],
																		pcsUIGrid->m_lNowLayer, 
																		pstGridSelectInfo->lGridRow, 
																		pstGridSelectInfo->lGridColumn );
	
	
}

BOOL AgcmUIItem::CBPrivateTradeUpdateMoney(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis				= (AgcmUIItem *)		pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	const CHAR			*szSendGhelldCount	= ((AcUIEdit *) (pcsCountControl->m_pcsBase))->GetText();
	if (!szSendGhelldCount ||
		!szSendGhelldCount[0])
		return FALSE;

	INT32			lGhelldCount		= atoi(szSendGhelldCount);
	if (lGhelldCount <= 0)
		return FALSE;

	((AcUIEdit *) (pcsCountControl->m_pcsBase))->SetText( "" );

	return pThis->m_pcsAgcmPrivateTrade->SendTradeUpdateMoney( pThis->m_pcsAgcmCharacter->GetSelfCID(), lGhelldCount );
}

BOOL AgcmUIItem::CBPrivateTradeUpdateCC(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *	pThis = (AgcmUIItem *)pClass;
	AgcdUIControl *	pcsCountControl = (AgcdUIControl *)pData1;
	const CHAR *	cc_text;
	INT				cc;

	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;
	cc_text = ((AcUIEdit *)(pcsCountControl->m_pcsBase))->GetText();
	if (!cc_text || !cc_text[0])
		return FALSE;

	cc = atoi(cc_text);
	if (cc < 0)
		return FALSE;
	((AcUIEdit *)(pcsCountControl->m_pcsBase))->SetText("");
	return pThis->m_pcsAgcmPrivateTrade->SendTradeUpdateCC(
		pThis->m_pcsAgcmCharacter->GetSelfCID(), cc);
}

BOOL AgcmUIItem::CBPrivateTradeLock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeLock();

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUnlock(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;

	bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeUnlock();

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeCancel(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;

	BOOL			bResult;
	INT32			lCID;

	if( pThis->m_pcsAgcmCharacter )
	{
		lCID = pThis->m_pcsAgcmCharacter->m_lSelfCID;

		if( lCID != 0 )
		{
			bResult = pThis->m_pcsAgcmPrivateTrade->SendTradeCancel( lCID );
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateClientLock( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	bool			bPushDown = *((bool *)pData);

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		if( bPushDown )
		{
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientLock);
		}
		else
		{
			bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientUnlock);
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeActiveReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeActiveReadyToExchange);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeReadyToExchange(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem		*pThis = (AgcmUIItem *) pClass;
	AgcdUIControl	*pcsControl = (AgcdUIControl *) pData1;
	AcUIButton		*pcsUIButton;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis && pcsControl )
	{
		pcsUIButton = (AcUIButton *)pcsControl->m_pcsBase;

		if( pcsUIButton )
		{
			bResult = pThis->m_pcsAgcmPrivateTrade->SendReadyToExchange();

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lRequestPrivateTradeUIOpen );
		}
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeClientReadyToExchange( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeClientReadyToExchange);
	}

	return bResult;
}

/*
	2005.06.02. By SungHoon
	�ŷ� ��û�� ����ڰ� �ŷ� �ź� ���� ���
*/
BOOL AgcmUIItem::CBPrivateTradeRequestRefuseUser( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if( !pClass || !pData || !pCustData) return FALSE;
	AgcmUIItem *pThis	= (AgcmUIItem *) pClass;
	INT32 lRequestCID	= *(INT32 *)	   pData;
	INT32 lTargetCID	= *(INT32 *)	   pCustData;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lRequestPrivateTradeUIClose);
	pThis->m_pcsPrivateTradeSetClientNameChar = NULL;

	AgpdCharacter *pcsCharacter = pThis->m_pcsAgpmCharacter->GetCharacter(lTargetCID);
	if (pcsCharacter == NULL) return FALSE;

	DWORD dwColor = 0xFF0000;
	CHAR *szColor = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRADE_NEGATIVE_TEXT_COLOR);
	if (szColor) dwColor = atol(szColor);

	CHAR szMessage[ 512 ] = { 0 };
	wsprintf(szMessage, pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_TRADE_REQUEST_FAILED_REFUSE_USER), pcsCharacter->m_szID);

	SystemMessage.ProcessSystemMessage(szMessage, dwColor);

	return TRUE;
}

//Private Trade MSG------------------------------------------------------------------------------------------------------
BOOL AgcmUIItem::CBPrivateTradeMSGComplete(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		pThis->_InitPrivateOptionItem();
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeMSGTradeComplete);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeMSGCancel(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		pThis->_InitPrivateOptionItem();
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lPrivateTradeMSGTradeCancel);
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateGhelld(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem			*pThis;
	BOOL				bResult;

	pThis = (AgcmUIItem *) pClass;
	bResult = FALSE;

	if ( pThis && pData )
	{
		pThis->m_lPrivateTradeGhelld = *((INT32 *)pData);

		//Data Setting.
		pThis->m_pcsPrivateTradeGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeGhelld);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateClientGhelld(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem			*pThis;
	BOOL				bResult;

	pThis = (AgcmUIItem *) pClass;
	bResult = FALSE;

	if ( pThis && pData )
	{
		pThis->m_lPrivateTradeClientGhelld = *((INT32 *)pData);

		//Data Setting.
		pThis->m_pcsPrivateTradeClientGhelld->m_stUserData.m_pvData = &pThis->m_lPrivateTradeClientGhelld;
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsPrivateTradeClientGhelld);

		bResult = TRUE;
	}

	return bResult;
}

BOOL AgcmUIItem::CBPrivateTradeUpdateSelfCC(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem * pThis = (AgcmUIItem *)pClass;

	if (!pThis)
		return FALSE;
	pThis->m_privateTradeSelfCC = *((INT32 *)pData);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_privateTradeSelfCCUD);
	return TRUE;
}

BOOL AgcmUIItem::CBPrivateTradeUpdatePeerCC(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem * pThis = (AgcmUIItem *)pClass;

	if (!pThis)
		return FALSE;
	pThis->m_privateTradePeerCC = *((INT32 *)pData);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_privateTradePeerCCUD);
	return TRUE;
}

BOOL AgcmUIItem::_CheckBoundItemReleaseTradeCheck( AgpdItem* pItem )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );
	INT32					nCount			=	m_pcsAgpmGrid->GetItemCount( m_pcsPrivateTradeGrid );
	AgpdGridItem*			pcsGridItem		=	NULL;
	AgpdItem*				pcsItem			=	NULL;

	// ���� ��ũ���� �ö� ���ٸ� �ͼ����� �ϳ��� �ø��� �ִ�
	// �ŷ�â�� �ͼ����� �ø����� �ִ��� üũ�Ѵ�
	for( INT32 i = 0 ; i < nCount ; ++i )
	{
		pcsGridItem	=	m_pcsAgpmGrid->GetItem( m_pcsPrivateTradeGrid , i );
		pcsItem		=	m_pcsAgpmItem->GetItem( pcsGridItem );

		if( m_pcsAgpmItem->IsBoundOnOwner( pcsItem ) || m_pcsAgpmItem->GetBoundType(pcsItem) != E_AGPMITEM_NOT_BOUND )
			return FALSE;
	}

	// Private Trade Option �������� �ö� �Դ��� Ȯ�� �Ѵ�
	for( INT32 i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
			break;

		pcsItem		=	m_pcsAgpmItem->GetItem( pADChar->m_stTradeOptionItem[i].m_lItemIID );

		if( m_pcsAgpmItem->IsMarvelScroll( pcsItem ) )
			return TRUE;
	}

	// ������ �ͼ��� �� �ø���
	return FALSE;
}

BOOL AgcmUIItem::_CheckSupportItemDuplicate( AgpdItem* pItem )
{
	if( !pItem	)
		return FALSE;

	AgpdItem*				pcsItem			=	NULL;
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	// Private Trade Option �������� �����Ǽ� �ö� �� ����
	for( INT32 i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
			break;

		pcsItem		=	m_pcsAgpmItem->GetItem( pADChar->m_stTradeOptionItem[i].m_lItemIID );

		if( pcsItem->m_lTID	== pItem->m_lTID )
			return FALSE;
	}

	return TRUE;
}

BOOL	AgcmUIItem::_AddSupportItem( AgpdItem* pItem , BOOL bSelfCharacter )
{
	
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	INT32					nCount			=	_SupportItemCount( bSelfCharacter );
		
	// ����
	if( bSelfCharacter )
	{
		pADChar->m_stTradeOptionItem[ nCount ].m_lItemIID	=	pItem->m_lID;
		pADChar->m_stTradeOptionItem[ nCount ].m_lItemTID	=	pItem->m_lTID;
	}

	// ����
	else
	{
		pADChar->m_stClientTradeOptionItem[ nCount ].m_lItemIID	=	pItem->m_lID;
		pADChar->m_stClientTradeOptionItem[ nCount ].m_lItemTID	=	pItem->m_lTID;
	}

	return TRUE;
}

INT32	AgcmUIItem::_SupportItemCount( BOOL bSelfCharacter )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );


	for( INT i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		if( bSelfCharacter )
		{
			if( !pADChar->m_stTradeOptionItem[i].m_lItemIID )
				return i;
		}
		else
		{
			if( !pADChar->m_stClientTradeOptionItem[i].m_lItemIID )
				return i;
		}

	}

	return 0;
}

VOID	AgcmUIItem::_InitPrivateOptionItem( VOID )
{
	AgpdItemADChar*			pADChar			=	m_pcsAgpmItem->GetADCharacter( m_pcsAgcmCharacter->GetSelfCharacter() );

	// �ŷ�â Grid �ʱ�ȭ
	m_pcsAgpmGrid->Reset( &m_PrivateTradeGridEx );
	m_pcsAgpmGrid->Reset( &m_PrivateTradeClientGridEx );
	
	for( INT i = 0 ; i < AGPDITEM_PRIVATE_TRADE_OPTION_MAX ; ++i )
	{
		ZeroMemory( &pADChar->m_stTradeOptionItem[i] , sizeof(AgpdPrivateOptionItem) );
		ZeroMemory( &pADChar->m_stClientTradeOptionItem[i] , sizeof(AgpdPrivateOptionItem) );
	}

}

BOOL AgcmUIItem::OnSendPrivateTradeCancel( void )
{
	AgcmPrivateTrade* pcmPrivateTrade = ( AgcmPrivateTrade* )g_pEngine->GetModule( "AgcmPrivateTrade" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmPrivateTrade || !pcmCharacter ) return FALSE;

	int nMyCID = pcmCharacter->GetSelfCID();
	return pcmPrivateTrade->SendTradeCancel( nMyCID );
}
