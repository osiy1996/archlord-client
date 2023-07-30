#include "AgsmCashMall.h"
#include "AgsmSystemMessage.h"
#include "AgsmBillingChina.h"
#include "AgpmReturnToLogin.h"

AgsmCashMall::AgsmCashMall()
{
	SetModuleName("AgsmCashMall");

	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;
	m_pcsAgpmCashMall		= NULL;
	m_pcsAgpmBillInfo		= NULL;

	m_pcsAgsmCharacter		= NULL;
	m_pcsAgsmCharManager	= NULL;
	m_pcsAgsmItem			= NULL;
	m_pcsAgsmItemManager	= NULL;
	m_pcsAgsmBilling		= NULL;

	m_pcsAgsmSystemMessage	= NULL;


	
}

AgsmCashMall::~AgsmCashMall()
{
}

BOOL AgsmCashMall::OnAddModule()
{
	m_pcsAgpmCharacter		= (AgpmCharacter *)		GetModule("AgpmCharacter");
	m_pcsAgpmItem			= (AgpmItem *)			GetModule("AgpmItem");
	m_pcsAgpmCashMall		= (AgpmCashMall *)		GetModule("AgpmCashMall");
	m_pcsAgpmBillInfo		= (AgpmBillInfo *)		GetModule("AgpmBillInfo");

	m_pcsAgsmCharacter		= (AgsmCharacter *)		GetModule("AgsmCharacter");
	m_pcsAgsmCharManager	= (AgsmCharManager *)	GetModule("AgsmCharManager");
	m_pcsAgsmItem			= (AgsmItem *)			GetModule("AgsmItem");
	m_pcsAgsmItemManager	= (AgsmItemManager *)	GetModule("AgsmItemManager");
	m_pcsAgsmBilling		= (AgsmBilling *)		GetModule("AgsmBilling");

	m_pcsAgpmReturnToLogin	= (AgpmReturnToLogin *) GetModule("AgpmReturnToLogin");

	if (!m_pcsAgpmCharacter ||
		!m_pcsAgpmItem ||
		!m_pcsAgpmCashMall ||
		!m_pcsAgpmBillInfo ||
		!m_pcsAgsmCharacter ||
		!m_pcsAgsmCharManager ||
		!m_pcsAgsmItem ||
		!m_pcsAgsmItemManager ||
		!m_pcsAgsmBilling ||
		!m_pcsAgpmReturnToLogin)
		return FALSE;
#ifdef _AREA_CHINA_
	if (!m_pcsAgsmCharManager->SetCallbackEnterGameWorld(CBEnterGame, this))
		return FALSE;
#else
	if (!m_pcsAgsmCharManager->SetCallbackConnectedChar(CBEnterGame, this))
		return FALSE;
#endif
	if (!m_pcsAgpmCashMall->SetCallbackRequestMallProductList(CBRequestMallProductList, this))
		return FALSE;
	if (!m_pcsAgpmCashMall->SetCallbackRequestBuyItem(CBRequestBuyItem, this))
		return FALSE;
	if (!m_pcsAgpmCashMall->SetCallbackRefreshCash(CBRefreshCash, this))
		return FALSE;
	if (!m_pcsAgpmCashMall->SetCallbackCheckListVersion(CBCheckListVersion, this))
		return FALSE;
	if (!m_pcsAgsmBilling->SetCallbackBuyCashItem(CBResultBuyItem, this))
		return FALSE;
	if (!m_pcsAgsmBilling->SetCallbackGetCashMoney(CBResultCashMoney, this))
		return FALSE;

	if (!m_pcsAgpmCashMall->SetCallbackDisableForaWhile(CBDisableForaWhile, this))
		return FALSE;

	if (g_eServiceArea == AP_SERVICE_AREA_CHINA)
	{
		g_billingBridge.SetBilling(m_pcsAgsmBilling);

		g_billingBridge.SetCharManager(m_pcsAgpmCharacter, m_pcsAgsmCharacter);
		g_billingBridge.SetItemManager(m_pcsAgpmItem, m_pcsAgsmItem, m_pcsAgsmItemManager);
		g_billingBridge.SetCashMallManager(m_pcsAgpmCashMall, this);
#ifdef _AREA_CHINA_
		if(!m_pcsAgsmCharManager->SetCallbackDisconnectCharacter(CBDisconnect, this))
			return FALSE;

		if(!m_pcsAgpmReturnToLogin->SetCallbackRequest(CBDisconnect, this))
			return FALSE;
#endif
	}

	return TRUE;
}

BOOL AgsmCashMall::OnInit()
{
#ifndef _AREA_CHINA_
	m_pcsAgsmSystemMessage		= (AgsmSystemMessage *)		GetModule("AgsmSystemMessage");

	if(NULL == m_pcsAgsmSystemMessage)
		return FALSE;
#endif
	return TRUE;
}

BOOL AgsmCashMall::OnDestroy()
{
	return TRUE;
}

BOOL AgsmCashMall::CBDisableForaWhile(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass || !pData)
		return FALSE;

	AgsmCashMall	*pThis			= (AgsmCashMall *)  pClass;
	INT32			*CID			= (INT32 *) pData;

	AgpdCharacter   *pcsCharacter	= pThis->m_pcsAgpmCharacter->GetCharacter(*CID);
	if(NULL == pcsCharacter)
		return FALSE;

	pThis->m_pcsAgsmSystemMessage->SendSystemMessage(pcsCharacter, AGPMSYSTEMMESSAGE_CODE_CANNOT_USE_BY_STATUS);

	return TRUE;
}

BOOL AgsmCashMall::CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCashMall	*pThis			= (AgsmCashMall *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	if (!pcsCharacter || pcsCharacter->m_szID[0] == 0)
		return FALSE;
	
	return pThis->m_pcsAgsmBilling->SendGetCashMoney(pcsCharacter);
}

BOOL AgsmCashMall::CBRequestMallProductList(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCashMall	*pThis			= (AgsmCashMall *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT32			lTab			= *static_cast<INT32*>(pCustData);

	return	pThis->SendMallProductList(pcsCharacter, lTab);
}

BOOL AgsmCashMall::CBRequestBuyItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCashMall	*pThis			= (AgsmCashMall *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	INT32			lProductID		= PtrToInt(ppvBuffer[0]);
	UINT8			ucMallListVersion	= (UINT8)		ppvBuffer[1];
	INT32			lType			= PtrToInt(ppvBuffer[2]);

	return pThis->ProcessBuyItem(pcsCharacter, lProductID, ucMallListVersion, lType);
}

BOOL AgsmCashMall::CBRefreshCash(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgsmCashMall	*pThis			= (AgsmCashMall *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdCashMall	*pcsAttachData	= pThis->m_pcsAgpmCashMall->GetADCharacter(pcsCharacter);
	if (!pcsAttachData)
		return FALSE;

	if (pcsAttachData->m_ulLastRefreshCashTimeMsec + AGPMCASHMALL_MIN_INTERVAL_REFRESH_CASH > pThis->GetClockCount())
		return TRUE;

	pcsAttachData->m_ulLastRefreshCashTimeMsec	= pThis->GetClockCount() + AGPMCASHMALL_MIN_INTERVAL_REFRESH_CASH;

	// request to billing server
	return pThis->m_pcsAgsmBilling->SendGetCashMoney(pcsCharacter);
}

BOOL AgsmCashMall::CBCheckListVersion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCashMall	*pThis				= (AgsmCashMall *)	pClass;
	AgpdCharacter	*pcsCharacter		= (AgpdCharacter *)	pData;
	UINT8			*pucMallListVersion	= (UINT8 *)			pCustData;

	AuAutoLock	Lock(pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_Mutex);
	if (!Lock.Result()) return FALSE;

	if (pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_ucMallListVersion != *pucMallListVersion)
	{
		// 탭 갯수만큼 보내준다.
		for(int i = 0; i < pThis->m_pcsAgpmCashMall->GetCashMallInfo()->m_lNumTab; ++i)
			pThis->SendMallProductList(pcsCharacter, i);
	}

	return TRUE;
}

INT32 AgsmCashMall::CheckBuyRequirement(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion, INT32 lType)
{
	if (!pcsCharacter)
		return AGPMCASH_BUY_RESULT_FAIL;

	if (m_pcsAgpmCashMall->GetCashMallInfo()->m_ucMallListVersion != ucMallListVersion)
	{
		return AGPMCASH_BUY_RESULT_NEED_NEW_ITEM_LIST;
	}

	pAgpmCashItemInfo	pCashItemInfo	= m_pcsAgpmCashMall->GetCashItem(lProductID);
	if (!pCashItemInfo)
	{
		return AGPMCASH_BUY_RESULT_FAIL;
	}

	// PC 방 체크 2008.01.28. steeple
	if(pCashItemInfo->m_lPCBangBuy & AGPMITEM_PCBANG_TYPE_BUY_ONLY)
	{
		if(m_pcsAgpmBillInfo->IsPCBang(pcsCharacter) == FALSE)
#ifndef _AREA_CHINA_
			return AGPMCASH_BUY_RESULT_NOT_S_PCBANG;
#else
			return AGPMCASH_BUY_RESULT_PCBANG_ONLY;
#endif

		if(pCashItemInfo->m_lPCBangBuy & AGPMITEM_PCBANG_TYPE_LIMITED_COUNT)
		{
			// Cash Inventory 에 해당 TID 의 Item 이 하나라도 있으면 살 수 없다. 200802.20. steeple
			for(int i = 0; i < AGPMCASHMALL_MAX_ITEM_TID; ++i)
			{
				if(pCashItemInfo->m_alItemTID[i] == 0)
					break;

				if(m_pcsAgpmItem->GetCashItemByTID(pcsCharacter, pCashItemInfo->m_alItemTID[i]))
					return AGPMCASH_BUY_RESULT_LIMITED_COUNT;
			}
		}
	}
#ifndef _AREA_CHINA_
	// TPack PC방 체크
	if(pCashItemInfo->m_lSpecialFlag == AGPMCASH_SPECIALFLAG_TPACK)
	{
		INT32 ulPCRoomType = 0;

		m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsCharacter, &ulPCRoomType);
		if(!(ulPCRoomType & AGPDPCROOMTYPE_HANGAME_TPACK))
			return AGPMCASH_BUY_RESULT_NOT_T_PCBANG;

		if(pCashItemInfo->m_lPCBangBuy & AGPMITEM_PCBANG_TYPE_LIMITED_COUNT)
		{
			// Cash Inventory 에 해당 TID 의 Item 이 하나라도 있으면 살 수 없다. 200802.20. steeple
			for(int i = 0; i < AGPMCASHMALL_MAX_ITEM_TID; ++i)
			{
				if(pCashItemInfo->m_alItemTID[i] == 0)
					break;

				if(m_pcsAgpmItem->GetCashItemByTID(pcsCharacter, pCashItemInfo->m_alItemTID[i]))
					return AGPMCASH_BUY_RESULT_LIMITED_COUNT;
			}
		}
	}

	// GPremium check
	if(pCashItemInfo->m_lPCBangBuy & AGPMITEM_PCBANG_TYPE_USE_ONLY_GPREMIUM)
	{
		INT32 ulPCRoomType = 0;

		m_pcsAgpmCharacter->EnumCallback(AGPMCHAR_CB_ID_CHECK_PCROOM_TYPE, pcsCharacter, &ulPCRoomType);
		if(!(ulPCRoomType & AGPDPCROOMTYPE_WEBZEN_GPREMIUM))
			return AGPMCASH_BUY_RESULT_NOT_GPREMIUM;

		if(pCashItemInfo->m_lPCBangBuy & AGPMITEM_PCBANG_TYPE_LIMITED_COUNT)
		{
			// Cash Inventory 에 해당 TID 의 Item 이 하나라도 있으면 살 수 없다. 200802.20. steeple
			for(int i = 0; i < AGPMCASHMALL_MAX_ITEM_TID; ++i)
			{
				if(pCashItemInfo->m_alItemTID[i] == 0)
					break;

				if(m_pcsAgpmItem->GetCashItemByTID(pcsCharacter, pCashItemInfo->m_alItemTID[i]))
					return AGPMCASH_BUY_RESULT_LIMITED_COUNT;
			}
		}
	}
#endif

#ifndef _AREA_GLOBAL_
	if (pCashItemInfo->m_llPrice > m_pcsAgpmCharacter->GetCash(pcsCharacter))
	{
		return AGPMCASH_BUY_RESULT_NOT_ENOUGH_CASH;
	}
#else
	CashInfoGlobal pCash;
	m_pcsAgpmBillInfo->GetCashGlobal(pcsCharacter, pCash.m_WCoin, pCash.m_PCoin);

	switch(lType)
	{
		case AGPMCASHMALL_TYPE_WCOIN:
			{
				if( pCashItemInfo->m_lPPCardOnly )
					return AGPMCASH_BUY_RESULT_PPCARD_ONLY;

				if( pCashItemInfo->m_llPrice > pCash.m_WCoin )
					return AGPMCASH_BUY_RESULT_NOT_ENOUGH_CASH;
			} break;
		case AGPMCASHMALL_TYPE_WCOIN_PPCARD:
			{
				if( pCashItemInfo->m_llPrice > pCash.m_PCoin )
					return AGPMCASH_BUY_RESULT_NOT_ENOUGH_CASH;
			} break;
		default:
			return AGPMCASH_BUY_RESULT_FAIL;
	}
#endif

	if (!m_pcsAgpmItem->IsAnyEmptyCashInventory(pcsCharacter))
	{
		return AGPMCASH_BUY_RESULT_INVENTORY_FULL;
	}
	
	AgpdGrid *pcsGrid = m_pcsAgpmItem->GetCashInventoryGrid(pcsCharacter);
	if (pCashItemInfo->m_lNumTotalItems > pcsGrid->m_lGridCount - pcsGrid->m_lItemCount)
	{
		return AGPMCASH_BUY_RESULT_INVENTORY_FULL;
	}
	
	return AGPMCASH_BUY_RESULT_SUCCESS;
}

BOOL AgsmCashMall::CBResultBuyItem(PVOID pData, PVOID pClass, PVOID pCustData)	
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCashMall		*pThis			= (AgsmCashMall *)	pClass;
	AgsdBillingItem		*pcsBilling		= (AgsdBillingItem *) pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;

	return pThis->ProcessBuyItemResult(pcsCharacter, pcsBilling);
}

BOOL AgsmCashMall::CBResultCashMoney(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgsmCashMall		*pThis			= (AgsmCashMall *)	pClass;
	AgsdBillingMoney	*pcsBilling		= (AgsdBillingMoney *) pData;
	AgpdCharacter		*pcsCharacter	= (AgpdCharacter *)	pCustData;
	
	if (AGSMBILLING_RESULT_SUCCESS == pcsBilling->m_lResult)
		{
		INT64 llTotalMoney = pcsBilling->m_llMoney + pcsBilling->m_llCouponMoney
							+ pcsBilling->m_llExternalEventMoney + pcsBilling->m_llInternalEventMoney;
		pThis->m_pcsAgpmCharacter->SetCash(pcsCharacter, llTotalMoney);
		}
	// else
	// 실패면 메시지?	
	
	return TRUE;
}

BOOL AgsmCashMall::ProcessBuyItem(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion, INT32 lType)
{
	if (!pcsCharacter)
		return FALSE;

	//STOPWATCH2(GetModuleName(), _T("ProcessBuyItem"));

	AuAutoLock	Lock(m_pcsAgpmCashMall->GetCashMallInfo()->m_Mutex);
	if (!Lock.Result()) return FALSE;

	INT32	lCheckResult	= CheckBuyRequirement(pcsCharacter, lProductID, ucMallListVersion, lType);
	if (lCheckResult != AGPMCASH_BUY_RESULT_SUCCESS)
	{
		SendBuyResult(pcsCharacter, lCheckResult);
		return TRUE;
	}

	pAgpmCashItemInfo	pCashItemInfo	= m_pcsAgpmCashMall->GetCashItem(lProductID);
	if (!pCashItemInfo)
	{
		SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}
	
	stCashItemBuyList stList;
	ZeroMemory(&stList, sizeof(stList));
	stList.m_lItemTID = pCashItemInfo->m_alItemTID[0];
	stList.m_lItemQty = pCashItemInfo->m_alItemQty[0];
	stList.m_llMoney = pCashItemInfo->m_llPrice;
	stList.m_cStatus = AGSDITEM_CASHITEMBUYLIST_STATUS_REQUEST;
	stList.m_lType   = lType;

	m_pcsAgsmItem->WriteCashItemBuyList(pcsCharacter, &stList);
	
	// send to billing server
	if (!m_pcsAgsmBilling->SendBuyCashItem(pcsCharacter, lProductID,pCashItemInfo->m_szDescription, pCashItemInfo->m_llPrice, stList))
	{
		SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}
	
	return TRUE;
}	

BOOL AgsmCashMall::ProcessBuyItemResult(AgpdCharacter *pcsCharacter, AgsdBillingItem *pcsBilling)
{
	if (!pcsCharacter || !pcsBilling)
		return FALSE;

	if (AGSMBILLING_RESULT_SUCCESS != pcsBilling->m_lResult)
	{
		SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}

	pAgpmCashItemInfo	pCashItemInfo	= m_pcsAgpmCashMall->GetCashItem(pcsBilling->m_lProductID);
	if (!pCashItemInfo)
	{
		SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}
	
	ApSafeArray<AgpdItem *, AGPMCASHMALL_MAX_ITEM_TID> 	apcsItem;
	apcsItem.MemSetAll();
	INT32 lAdded = 0;
	
	for (INT32 i = 0; i < pCashItemInfo->m_lNumTotalItems; i++)
	{
		AgpdItem	*pcsItem	= m_pcsAgsmItemManager->CreateItem(pCashItemInfo->m_alItemTID[i],
																   pcsCharacter,
																   pCashItemInfo->m_alItemQty[i]);
		if (!pcsItem)
		{
			if (0 == i)
			{
				SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
				return FALSE;
			}
			
			continue;
		}

#ifndef _AREA_CHINA_
		if(pcsBilling->m_lType == AGPMCASHMALL_TYPE_WCOIN_PPCARD)
		{
			pcsItem->m_lStatusFlag |= AGPMITEM_STATUS_CASH_PPCARD;
		}
#endif
		// insert item into cash inventory
		if (!m_pcsAgpmItem->AddItemToCashInventory(pcsCharacter, pcsItem))
		{
			if (0 == i)
			{
				m_pcsAgpmItem->RemoveItem(pcsItem, TRUE);
				SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
				return FALSE;
			}
		
			continue;
		}

		apcsItem[lAdded++] = pcsItem;
	}

#ifndef _AREA_GLOBAL_
	if (!m_pcsAgpmCharacter->SubCash(pcsCharacter, pCashItemInfo->m_llPrice))
	{
		for (INT32 i = 0; i < lAdded; i++)
		{
			m_pcsAgpmItem->RemoveItem(apcsItem[i], TRUE);
		}
		SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_FAIL);
		return FALSE;
	}
#endif

	AgsdItem *pcsAgsdItem = m_pcsAgsmItem->GetADItem(apcsItem[0]);
	m_pcsAgsmItem->UpdateCashItemBuyList_Complete(pcsAgsdItem->m_ullDBIID, pcsBilling->m_ullOrderNo, pcsBilling->m_ullListSeq);

	// log			
	CHAR szProduct[16];
	for (INT32 i = 0; i < lAdded; i++)
	{
		AgpdItem *pcsItem = apcsItem[i];
		sprintf(szProduct, "#%d-%d/%d", pCashItemInfo->m_lProductID, i+1, pCashItemInfo->m_lNumTotalItems);
		m_pcsAgsmItem->WriteItemLog(AGPDLOGTYPE_ITEM_NPC_BUY, pcsCharacter->m_lID, pcsItem, pcsItem->m_nCount, (INT32)pCashItemInfo->m_llPrice, szProduct);
	}


	SendBuyResult(pcsCharacter, AGPMCASH_BUY_RESULT_SUCCESS);
	return TRUE;
}

BOOL AgsmCashMall::SendBuyResult(AgpdCharacter *pcsCharacter, INT32 lResult)
{
	if (!pcsCharacter)
		return FALSE;

	INT16	nPacketLength	= 0;
	PVOID	pvPacket		= m_pcsAgpmCashMall->MakePacketBuyResult((INT8) lResult, &nPacketLength);

	if (!pvPacket || nPacketLength < sizeof(PACKET_HEADER))
		return FALSE;

	m_pcsAgpmCashMall->m_csPacket.SetCID(pvPacket, nPacketLength, pcsCharacter->m_lID);

	SendPacket(pvPacket, nPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);

	return TRUE;
}

BOOL AgsmCashMall::SendMallProductList(AgpdCharacter *pcsCharacter, INT32 lTab)
{
	if (!pcsCharacter)
		return FALSE;

	AuAutoLock	Lock(m_pcsAgpmCashMall->GetCashMallInfo()->m_Mutex);
	if (!Lock.Result()) return FALSE;

	INT16	nMallTabPacketLength	= 0;
	PVOID	pvPacketMallTabInfo		= m_pcsAgpmCashMall->MakePacketMallTabInfo(&nMallTabPacketLength);

	if (nMallTabPacketLength >= sizeof(PACKET_HEADER) && pvPacketMallTabInfo)
	{
		m_pcsAgpmCashMall->m_csPacket.SetCID(pvPacketMallTabInfo, nMallTabPacketLength, pcsCharacter->m_lID);

		SendPacket(pvPacketMallTabInfo, nMallTabPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);
	}

	INT16	nProductPacketLength	= 0;
	PVOID	pvPacketProductInfo		= m_pcsAgpmCashMall->MakePacketMallProductInfo(&nProductPacketLength, lTab);

	if (nProductPacketLength >= sizeof(PACKET_HEADER) && pvPacketProductInfo)
	{
		m_pcsAgpmCashMall->m_csPacket.SetCID(pvPacketProductInfo, nProductPacketLength, pcsCharacter->m_lID);

		SendPacket(pvPacketProductInfo, nProductPacketLength, m_pcsAgsmCharacter->GetCharDPNID(pcsCharacter), PACKET_PRIORITY_1);
	}

	return TRUE;
}

#ifdef _AREA_CHINA_
BOOL AgsmCashMall::CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(NULL == pData || NULL == pClass)
		return FALSE;

	AgsmCashMall *pThis				= (AgsmCashMall *) pClass;
	AgpdCharacter* pcsCharacter		= (AgpdCharacter *) pData;

	AgsdCharacter *pAgsdCharacter	= g_billingBridge.GetAgsmChar()->GetADCharacter(pcsCharacter);
	if(NULL == pAgsdCharacter)
		return FALSE;

	AgsdBilling *pcsAgsdBilling		= g_billingBridge.Find(pAgsdCharacter->m_szAccountID);
	if(pcsAgsdBilling)
	{
		pcsAgsdBilling->Release();
		g_billingBridge.Remove(pAgsdCharacter->m_szAccountID);
		g_billingBridge.GetAgsmBilling()->Remove(pcsAgsdBilling->m_lID);
		g_billingBridge.GetAgsmBilling()->GetAuGenerateID().AddRemoveID(pcsAgsdBilling->m_lID);
	}

	return TRUE;
}
#endif

