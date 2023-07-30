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

BOOL
AgcmUIItem::CBItemLockMoveItem(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIItem *			pThis = (AgcmUIItem *) pClass;
	AgcdUIControl *			pcsControl = (AgcdUIControl *) pData1;
	AgpdGridSelectInfo *	pstGridSelectInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;
	INT32					lItemID;
	AgpdItem *				pcsItem;

	if (pThis->m_bIsProcessConvert)
		return TRUE;
	if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_GRID)
		return FALSE;
	pcsUIGrid = (AcUIGrid *)pcsControl->m_pcsBase;
	pstGridSelectInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstGridSelectInfo)
		return FALSE;
	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if (!pstGrid)
		return FALSE;
	lItemID = pstGridSelectInfo->pGridItem->m_lItemID;
	pcsItem = pThis->m_pcsAgpmItem->GetItem(lItemID);
	if (!pcsItem || pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return FALSE;
	pThis->m_pcsAgpmGrid->Reset(pstGrid);
	pThis->m_pcsAgpmGrid->AddItem(pstGrid, pcsItem->m_pcsGridItem);
	if (!pThis->m_pcsAgpmGrid->Add(pstGrid, pcsItem->m_pcsGridItem, 1, 1))
		return FALSE;
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsItemLockGridUD);
	return TRUE;
}

BOOL
AgcmUIItem::CBItemLockConfirm(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIItem *	pThis = (AgcmUIItem *)pClass;
	AgcdUIControl *	pcsControl = (AgcdUIControl *)pData1;
	AgpdGrid *		pstGrid;
	INT32			lItemID;
	AgpdGridItem *	pstGridItem;
	AgpdItem *		pcsItem;
	PVOID			pvPacket;
	INT16			lPacketSize;

	if (pThis->m_bIsProcessConvert)
		return TRUE;
	if (!pcsControl || pcsControl->m_lType != AcUIBase::TYPE_GRID)
		return FALSE;
	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if (!pstGrid)
		return FALSE;
	pstGridItem = pThis->m_pcsAgpmGrid->GetItem(pstGrid, 0);
	if (!pstGridItem)
		return FALSE;
	lItemID = pstGridItem->m_lItemID;
	pThis->m_pcsAgpmGrid->Reset(pstGrid);
	pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_pcsItemLockGridUD);
	pcsItem = pThis->m_pcsAgpmItem->GetItem(lItemID);
	if (!pcsItem || pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY)
		return FALSE;
	pvPacket = pThis->m_pcsAgpmItem->MakePacketUpdateStatusFlag(
		pcsItem, &lPacketSize);
	if (!pvPacket || !lPacketSize)
		return FALSE;
	pThis->SendPacket(pvPacket, lPacketSize);
	pThis->m_pcsAgpmItem->m_csPacket.FreePacket(pvPacket);
	return TRUE;
}
