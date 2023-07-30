#include <AgcmUIItem/AgcmUIItem.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmItem/AgpmItem.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgcmEventNPCTrade/AgcmEventNPCTrade.h>
#include <AgcmUIChatting2/AgcmUIChatting2.h>
#include <AgcmUIControl/AcUIGrid.h>
#include <AgcModule/AgcEngine.h>

BOOL AgcmUIItem::CBNPCTradeUIOpen(PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis != NULL )
	{
		//거래를 하기위해서 NPC창과 인벤창을 연다.
		if( pThis->m_bUIOpenNPCTrade == FALSE )
		{
			AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pcsSelfCharacter)
			{
				pThis->m_stNPCTradeOpenPos.x		= pcsSelfCharacter->m_stPos.x;
				pThis->m_stNPCTradeOpenPos.z		= pcsSelfCharacter->m_stPos.z;
			}

			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeOpenUI);
			pThis->m_bUIOpenNPCTrade = TRUE;
		}

		pThis->OpenInventory();

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return bResult;
}

BOOL AgcmUIItem::CBNPCTradeMoveItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *pcsControl = (AgcdUIControl *) pData1;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;

	BOOL					bResult;

	bResult = FALSE;

	//원래 위치에서 이동된 Layer, Row, Column로 아이템을 옮긴다는 패킷을 보낸다.
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
			//인벤에서 NPCTrade창으로 옮겨진 경우
			if( pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_INVENTORY || 
				pcsAgpdItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY )
			{
				pThis->m_lNPCTradeSellItemID = lItemID;

				AgcdUIHotkeyType	eHotKeyType	= pThis->m_pcsAgcmUIManager2->GetKeyStatus();

				BOOL	bProcess	= FALSE;

				if (eHotKeyType == AGCDUI_HOTKEY_TYPE_SHIFT)
				{
				}

				if (!bProcess)
				{
					bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
				}
			}
		}
	}

	return bResult;

}

BOOL AgcmUIItem::CBNPCTradeUIOpen(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL			bResult;

	pThis->m_pcsAgpmCharacter->StopCharacter(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), NULL);

	bResult = FALSE;

	if( pThis )
	{
		if( pThis->m_bUIOpenNPCTrade == FALSE )
		{
			AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
			if (pcsSelfCharacter)
			{
				pThis->m_stNPCTradeOpenPos.x		= pcsSelfCharacter->m_stPos.x;
				pThis->m_stNPCTradeOpenPos.z		= pcsSelfCharacter->m_stPos.z;
			}

			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeOpenUI );
			pThis->m_bUIOpenNPCTrade = TRUE;
		}

		pThis->OpenInventory();

		pThis->m_lOpenNPCTradeLayer	= 0;

		//pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
		pThis->m_pcsAgcmUIManager2->RefreshUserData( pThis->m_pcsNPCTrade );
	}

	return bResult;
}

BOOL AgcmUIItem::CBNPCTradeUIClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( pThis->m_bUIOpenNPCTrade )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeCloseUI );
			pThis->m_bUIOpenNPCTrade = FALSE;
		}

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return bResult;
}

BOOL	AgcmUIItem::CBNPCTradeOpenNextLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	if (pThis->m_lOpenNPCTradeLayer >= AGPMITEM_NPCTRADEBOX_LAYER - 1)
		return TRUE;

	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

	AgpdGrid		*pcsGrid		= pcsUIGrid->GetAgpdGrid();
	if (!pcsGrid)
		return FALSE;

	if (!pThis->m_pcsAgpmGrid)
		return FALSE;

	// 비어 있는 그리드면 넘어가지 않는다.
	if (pThis->m_pcsAgpmGrid->IsEmptyGrid(pcsGrid, pThis->m_lOpenNPCTradeLayer + 1, 0, 0, 1, 1))
		return TRUE;

	pThis->m_lOpenNPCTradeLayer++;

	pThis->m_pcsNPCTrade->m_lSelectedIndex = pThis->m_lOpenNPCTradeLayer;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );
//	pcsUIGrid->SetNowLayer(++pThis->m_lOpenNPCTradeLayer);

	return TRUE;
}

BOOL	AgcmUIItem::CBNPCTradeOpenPrevLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;

	if (pThis->m_lOpenNPCTradeLayer <= 0)
		return TRUE;

	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

	pThis->m_lOpenNPCTradeLayer--;

	pThis->m_pcsNPCTrade->m_lSelectedIndex = pThis->m_lOpenNPCTradeLayer;

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );

//	pcsUIGrid->SetNowLayer(--pThis->m_lOpenNPCTradeLayer);

	return TRUE;
}

BOOL	AgcmUIItem::CBNPCTradeInitLayer( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
	
	AgcdUIControl	*pcsControl		= (AgcdUIControl *)		pData1;

	if( (pcsControl->m_lType != AcUIBase::TYPE_GRID) || !pcsControl->m_pcsBase)
		return FALSE;

	AcUIGrid		*pcsUIGrid		= (AcUIGrid *)			pcsControl->m_pcsBase;

//	pcsUIGrid->SetNowLayer(pThis->m_lOpenNPCTradeLayer);

	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh( pThis->m_pcsNPCTrade );

	return TRUE;
}

//NPC Trade MSG----------------------------------------------------------------------------------------------------------
BOOL AgcmUIItem::CBUINPCTradeMSGFullInven( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGInventoryFull);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuyStackCountOK( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	if (!pClass || !pData1)
		return FALSE;

	AgcmUIItem		*pThis				= (AgcmUIItem *)		pClass;

	AgcdUIControl	*pcsCountControl	= (AgcdUIControl *)		pData1;
	if (pcsCountControl->m_lType != AcUIBase::TYPE_EDIT ||
		!pcsCountControl->m_pcsBase)
		return FALSE;

	const CHAR			*szStackCount		= ((AcUIEdit *) (pcsCountControl->m_pcsBase))->GetText();
	if (!szStackCount ||
		!szStackCount[0])
		return FALSE;

	INT32			lStackCount		= atoi(szStackCount);
	if (lStackCount <= 0)
		return FALSE;

	AgpdItem		*pcsAgpdItem	= pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeBuyItemID );
	if (pcsAgpdItem && pcsAgpdItem->m_pcsItemTemplate)
	{
		if (!((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_bStackable ||
			((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount < 1)
			return FALSE;

		if (((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount < lStackCount)
			lStackCount = ((AgpdItemTemplate *) pcsAgpdItem->m_pcsItemTemplate)->m_lMaxStackableCount;

		pcsAgpdItem->m_nCount	= lStackCount;

		pThis->m_pcsAgcmEventNPCTrade->SendBuy( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
												pcsAgpdItem->m_lID,
												lStackCount,
												pThis->m_nNPCTradeBuyStatus);

		pThis->m_lNPCTradeBuyItemID = 0;
		pThis->m_nNPCTradeBuyStatus = -1;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmBuyStackCountCancel( PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl )
{
	return TRUE;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmSell( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGConfirmSell);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGConfirmSell( PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		if( lTrueCancel )
		{
			AgpdItem			*pcsAgpdItem;

			pcsAgpdItem = pThis->m_pcsAgpmItem->GetItem( pThis->m_lNPCTradeSellItemID );

			if( pcsAgpdItem != NULL )
			{
				AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
				if( ppmItem )
				{
					// 이 아이템이 NPC에게 판매불가능한 아이템이라면 판매할수 없다는 메세지를 출력하고 종료
					// 1. 퀘스트아이템의 경우
					if( ppmItem->IsQuestItem( pcsAgpdItem ) )
					{
						AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
						if( pcmUIManager )
						{
							char* pMsg = pcmUIManager->GetUIMessage( "CannotNPCTradeQuestItem" );
							if( pMsg && strlen( pMsg ) > 0 )
							{
								pcmUIManager->ActionMessageOKDialog( pMsg );
							}
						}

						return TRUE;
					}
				}

				bResult = pThis->m_pcsAgcmEventNPCTrade->SendSell( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
																pcsAgpdItem->m_lID,
																pcsAgpdItem->m_nCount );
			}
		}

	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGNotEnoughGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;

	BOOL			bResult;

	bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGNotEnoughGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::CBUINPCTradeMSGNotEnoughSpaceToGetGhelld( PVOID pData, PVOID pClass, PVOID pCustData )
{
	AgcmUIItem *pThis = (AgcmUIItem *) pClass;
	BOOL bResult = FALSE;

	if( pThis )
	{
		bResult = pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeMSGNotEnoughSpaceToGetGhelld);
	}

	return bResult;
}

BOOL AgcmUIItem::SelfUpdatePositionCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	PROFILE("AgcmUIItem::SelfUpdatePositionCB");

	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)		pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)		pData;

	if (pThis->m_bUIOpenNPCTrade)
	{
		FLOAT	fDistance = AUPOS_DISTANCE_XZ(pcsCharacter->m_stPos, pThis->m_stNPCTradeOpenPos);

		if ((INT32) fDistance < AGCMUIITEM_CLOSE_UI_DISTANCE)
			return TRUE;

		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lNPCTradeCloseUI );
		pThis->m_bUIOpenNPCTrade = FALSE;

		if( pThis->m_bUIOpenInventory )
		{
			pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lInventoryUIClose);
			pThis->m_bUIOpenInventory = FALSE;
		}

		pThis->m_lOpenNPCTradeLayer	= 0;

		pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeInitLayer);
	}

	return TRUE;
}

BOOL AgcmUIItem::CBBuyItemDirect(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
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

	if (!pThis->m_bUIOpenNPCTrade)
		return FALSE;

	pThis->m_lNPCTradeBuyItemID = pcsItem->m_lID;
	return pThis->OnOpenMsgBoxConfirmBuyNPCTradeItem();
}

BOOL AgcmUIItem::CBNPCTradeSuccess(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;

	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lNPCTradeSuccess);

	return TRUE;
}

BOOL AgcmUIItem::CBSplitItemNPCTrade(PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIItem			*pThis				= (AgcmUIItem *)			pClass;
	INT32				lStackCount			= *(INT32 *)					pData;

	if (!pThis->m_pcsAgpdSplitItem)
		return FALSE;

	return pThis->m_pcsAgcmEventNPCTrade->SendSell( pThis->m_pcsAgcmCharacter->m_lSelfCID, 
													pThis->m_pcsAgpdSplitItem->m_lID,
													pThis->m_pcsAgpdSplitItem->m_nCount );
}

BOOL AgcmUIItem::CBNPCTradeDeleteGrid(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIItem *	pThis	= (AgcmUIItem *)	pClass;
	AgpdGrid *		pcsGrid	= (AgpdGrid *)		pData;

	if (!pcsGrid)
		return TRUE;

	if (pThis->m_pcsNPCTradeGrid == pcsGrid)
	{
		//pThis->m_pcsAgcmEventNPCTrade	=	NULL;
		pThis->m_pcsNPCTradeGrid		=	NULL;
		pThis->m_pcsNPCTrade->m_stUserData.m_pvData	=	NULL;
		pThis->m_lOpenNPCTradeLayer		=	0;

		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsNPCTrade);
	}

	return TRUE;
}

BOOL AgcmUIItem::OnOpenMsgBoxConfirmBuyNPCTradeItem( int nLayer, int nRow, int nCol )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	// 거래가 가능한지 검사
	if( !_CheckEnableNPCTradeItem() ) return FALSE;

	// 거래시 출력할 문구 생성
	std::string strMsg = _MakeConfirmMsgBuyNPCTradeItem();
	if( strlen( strMsg.c_str() ) <= 0 ) return FALSE;

	// 문구 출력
	int nResult = pcmUIManager->ActionMessageOKCancelDialog( ( char* )strMsg.c_str() );

	// 서버에 거래 요청
	return nResult != 0 ? OnSendBuyNPCTradeItem( m_lNPCTradeBuyItemID, nLayer, nRow, nCol ) : TRUE;
}

BOOL AgcmUIItem::OnSendBuyNPCTradeItem( int nItemCID, int nLayer, int nRow, int nCol )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmEventNPCTrade* pcmEventNPCTrade = ( AgcmEventNPCTrade* )g_pEngine->GetModule( "AgcmEventNPCTrade" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmItem || !pcmEventNPCTrade || !pcmCharacter ) return FALSE;

	AgpdItem* ppdItem = ppmItem->GetItem( nItemCID );
	if( !ppdItem ) return FALSE;

	return pcmEventNPCTrade->SendBuy( pcmCharacter->m_lSelfCID, ppdItem->m_lID,	ppdItem->m_nCount, ( INT16 )m_nNPCTradeBuyStatus, (INT8)nLayer, (INT8)nRow, (INT8)nCol );
}

BOOL AgcmUIItem::OnCheckEquipEnable( AgpdItem* ppdItem )
{
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return FALSE;

	// 인벤토리 및 서브 인벤토리에 있지 않은 아이템은 착용할 수 없다.
	if( ppdItem->m_eStatus != AGPDITEM_STATUS_INVENTORY && ppdItem->m_eStatus != AGPDITEM_STATUS_SUB_INVENTORY ) return FALSE;

	AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItem->m_pcsItemTemplate;
	switch( ppdItemTemplateEquip->m_nKind )
	{
	case AGPMITEM_EQUIP_KIND_RIDE :
		{
			// 탈것인 경우 악당 1단계까지는 탈수 있다.
			if( !IsHaveMurdererLevel( AGPMCHAR_MURDERER_LEVEL2_POINT ) ) return TRUE;

			// 2단계 이상인 경우 시스템 메세지로 탈것을 탈수 없다고 알린다.
			_NoticeBySystemMessage( "Alert_CannotUseContents_ByMurderer" );
			return FALSE;
		}
		break;
	}

	// 그 외는 가능한 것으로..
	return TRUE;
}

BOOL AgcmUIItem::OnCheckUseEnable( AgpdItem* ppdItem )
{
	if( !ppdItem ) return FALSE;

	AgpdItemTemplateUsable* ppdItemTemplateUsable = ( AgpdItemTemplateUsable* )ppdItem->m_pcsItemTemplate;
	if( !ppdItemTemplateUsable || ppdItemTemplateUsable->m_nType != AGPMITEM_TYPE_USABLE ) return FALSE;

	switch( ppdItemTemplateUsable->m_nUsableItemType )
	{
	case AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL :
		{
			// 탈것인 경우 악당 1단계까지는 쓸수 있다.
			if( !IsHaveMurdererLevel( AGPMCHAR_MURDERER_LEVEL2_POINT ) ) return TRUE;

			// 2단계 이상인 경우 시스템 메세지로 탈것을 탈수 없다고 알린다.
			_NoticeBySystemMessage( "Alert_CannotUseContents_ByMurderer" );
			return FALSE;
		}
		break;
	}

	// 그 외는 가능한 것으로..
	return TRUE;
}

BOOL AgcmUIItem::OnCheckUseEnable( AgpdItem* ppdItem, AgpdCharacter* ppdCharacter )
{
	if( !ppdItem ) return FALSE;

	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !pcmCharacter) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	// 만약 ppdCharacter 가 나 자신이 아니라면 검사하지 않다. 이러한 경우 사용적합성에 대한 여부는 서버에서 판단된 이후니까
	if( ppdCharacter && ppdCharacter != ppdSelfCharacter ) return TRUE;

	return OnCheckUseEnable( ppdItem );
}

int AgcmUIItem::GetPriceNPCTradeItem( AgpdItem* ppdItem )
{
	if( !ppdItem ) return 0;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmItem || !ppmCharacter || !pcmCharacter ) return 0;

	AgpdCharacter* ppdCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdCharacter ) return 0;

	// 카운트 체크하여 가격계산
	float fItemNPCPrice = ppmItem->GetNPCPrice( ppdItem );
	int nTotalPrice = ppdItem->m_nCount > 0 ? ( int )( fItemNPCPrice * ppdItem->m_nCount ) : ( int )fItemNPCPrice;
		
	// 세율 적용
	int nTaxRatio = ppmCharacter->GetTaxRatio( ppdCharacter );
	int nTax = nTaxRatio > 0 ? ( nTotalPrice * nTaxRatio ) / 100 : 0;

	return nTotalPrice + nTax;
}

BOOL AgcmUIItem::IsHaveMurdererLevel( int nMurdererLevel )
{
	int nCurrLevel = GetMurdererLevel();
	return nCurrLevel >= nMurdererLevel ? TRUE : FALSE;
}

int AgcmUIItem::GetMurdererLevel( void )
{
	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )g_pEngine->GetModule( "AgpmCharacter" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	if( !ppmCharacter || !pcmCharacter ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return 0;

	return ppmCharacter->GetMurdererLevel( ppdSelfCharacter );
}

BOOL AgcmUIItem::_CheckEnableNPCTradeItem( void )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !ppmItem || !pcmUIManager ) return FALSE;

	AgpdItem* ppdItem = ppmItem->GetItem( m_lNPCTradeBuyItemID );
	if( !ppdItem || !ppdItem->m_pcsItemTemplate ) return FALSE;

	int nMurdererLevel = GetMurdererLevel();
	switch( nMurdererLevel )
	{
	// 현재는 1단계에서 받는 제약은 없음
	case AGPMCHAR_MURDERER_LEVEL1_POINT :					break;

	// 2단계 이상에 적용되는 제약사항
	case AGPMCHAR_MURDERER_LEVEL2_POINT :
	case AGPMCHAR_MURDERER_LEVEL3_POINT :
		{
			// 아이템이 탈것인 경우 구매할 수 없다고 알려준다.
			AgpdItemTemplateEquip* ppdItemTemplateEquip = ( AgpdItemTemplateEquip* )ppdItem->m_pcsItemTemplate;
			if( ppdItemTemplateEquip->m_nKind == AGPMITEM_EQUIP_KIND_RIDE )
			{
				char* pMsg = pcmUIManager->GetUIMessage( "Alert_NPCTrade_MurdererRejectBuyItem" );
				if( pMsg && strlen( pMsg ) > 0 )
				{
					pcmUIManager->ActionMessageOKDialog( pMsg );
				}

				return FALSE;
			}
		}
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::_CheckIsMurderer( void )
{
	int nMurdererLevel = GetMurdererLevel();
	return nMurdererLevel > 0 ? TRUE : FALSE;
}

std::string AgcmUIItem::_MakeConfirmMsgBuyNPCTradeItem( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return "";

	BOOL bIsMurderer = _CheckIsMurderer();
	if( bIsMurderer )
	{
		char* pMsg = pcmUIManager->GetUIMessage( "ConfirmMsg_BuyNPCTradeItemMurderer" );
		if( !pMsg || strlen( pMsg ) <= 0 ) return "";

		__int64 nPrice = _CalcItemPriceByMurdererLevel( m_lNPCTradeBuyItemID, bIsMurderer );

		char strBuffer[ 256 ] = { 0, };
		sprintf_s( strBuffer, sizeof( char ) * 256, pMsg, nPrice );
		return strBuffer;
	}
	else
	{
		char* pMsg = pcmUIManager->GetUIMessage( "ConfirmMsg_BuyNPCTradeItemNormal" );
		if( !pMsg || strlen( pMsg ) <= 0 ) return "";
		return pMsg;
	}

	return "";
}

__int64 AgcmUIItem::_CalcItemPriceByMurdererLevel( int nItemCID, BOOL bIsMurderer )
{
	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	if( !ppmItem ) return 0;

	AgpdItem* ppdItem = ppmItem->GetItem( nItemCID );
	if( !ppdItem ) return 0;

	__int64 nItemPrice = GetPriceNPCTradeItem( ppdItem );
	return bIsMurderer ? nItemPrice * 2 : nItemPrice;
}

void AgcmUIItem::_NoticeBySystemMessage( char* pMsgName )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	AgcmChatting2* pcmChatting2 = ( AgcmChatting2* )g_pEngine->GetModule( "AgcmChatting2" );
	if( !pcmUIManager || !pcmChatting2 ) return;

	char* pMsg = pcmUIManager->GetUIMessage( pMsgName );
	if( !pMsg || strlen( pMsg ) <= 0 ) return;
	
	pcmChatting2->AddSystemMessage( pMsg );
}
