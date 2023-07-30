#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmItemConvert/AgpmItemConvert.h>
#include <AgcmItemConvert/AgcmItemConvert.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmItem/AgcmItem.h>

BOOL AgcmUIItem::CBAskReallyConvert( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUIItem		*pThis			= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
	AgpdItem		*pcsSpiritStone	= (AgpdItem *)		pCustData;

	// ���� �������ΰ� ���� �ϰ͵� ���Ѵ�.
	if (pThis->m_bIsProcessConvert)
		return TRUE;

	pThis->m_bIsProcessConvert		= TRUE;
	pThis->m_lConvertItemID			= pcsItem->m_lID;
	pThis->m_lSpiritStoneID			= pcsSpiritStone->m_lID;

	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lUIOpenAskConvert);
}

BOOL AgcmUIItem::CBConvertResult( PVOID pData, PVOID pClass, PVOID pCustData )
{
	if (!pData || !pClass || !pCustData || !((PVOID *) pCustData)[0])
		return FALSE;

	AgcmUIItem		*pThis		= (AgcmUIItem *)	pClass;
	AgpdItem		*pcsItem	= (AgpdItem *)		pData;
	AgpdItemConvertSpiritStoneResult	*peResult			= (AgpdItemConvertSpiritStoneResult *)	pCustData;

	//	������ ���� ��û�� ���� ������ ó���� �� �� ����� �޾Ҵ�.
	//
	//		AGPMITEM_CONVERT_SUCCESS	: ������ �����ߴ�.
	//		AGPMITEM_CONVERT_FAIL		: ���� ���д�. ��ȣ ������...
	//		AGPMITEM_CONVERT_RUIN		: �̰� ���� ���л� �ƴ϶� ������ �����Ѱű��� ���ư���.
	//
	//		�ʿ��ϴٸ� UI�󿡼� ������ ǥ�����ش�.
	//

	switch (*peResult) {
	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS:
		{
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lUIOpenConvertResultSuccess);
		}
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_FAILED:
		{
		}
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_ALREADY_FULL:
		{
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lUIOpenConvertResultFull);
		}
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_IS_EGO_ITEM:
		{
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lUIOpenConvertResultEgoItem);
		}
		break;

	case AGPDITEMCONVERT_SPIRITSTONE_RESULT_INVALID_RANK:
		{
			return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lUIOpenConvertResultDifferRank);
		}
		break;

	default:
		return FALSE;
		break;
	}

	return TRUE;
}

BOOL AgcmUIItem::CBSendConvertItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis				= (AgcmUIItem *)	pClass;

	if (!pThis->m_bIsProcessConvert ||
		pThis->m_lConvertItemID	== AP_INVALID_IID ||
		pThis->m_lSpiritStoneID	== AP_INVALID_IID)
		return FALSE;

	AgpdCharacter	*pcsSelfCharacter	= pThis->m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	AgpdItem	*pcsItem	= pThis->m_pcsAgpmItem->GetItem(pThis->m_lSpiritStoneID);
	if (!pcsItem)
		return FALSE;

	BOOL		bSendResult	= FALSE;
	if (AGPDITEMCONVERT_SPIRITSTONE_RESULT_SUCCESS == pThis->m_pcsAgpmItemConvert->IsValidSpiritStone(pcsItem))
        bSendResult	= pThis->m_pcsAgcmItem->SendRequestConvert(pThis->m_lConvertItemID, pcsSelfCharacter->m_lID, pThis->m_lSpiritStoneID);
	else
		bSendResult	= pThis->m_pcsAgcmItemConvert->SendRequestRuneConvert(pcsSelfCharacter->m_lID, pThis->m_lConvertItemID, pThis->m_lSpiritStoneID);

	pThis->m_bIsProcessConvert		= FALSE;
	pThis->m_lConvertItemID			= AP_INVALID_IID;
	pThis->m_lSpiritStoneID			= AP_INVALID_IID;

	return bSendResult;
}

BOOL AgcmUIItem::CBCancelConvertItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIItem		*pThis				= (AgcmUIItem *)	pClass;

	pThis->m_bIsProcessConvert			= FALSE;
	pThis->m_lConvertItemID				= AP_INVALID_IID;
	pThis->m_lSpiritStoneID				= AP_INVALID_IID;

	return TRUE;
}