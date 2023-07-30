#include "AgcmUICashInven.h"
#include <AuTimeStamp/AuTimeStamp.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AgcmUICashItemCabient/AgcmUICouponBox.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUIControl/AcUIButton.h>
#include <AgcmUIControl/AcUIEdit.h>
#include <AgcmItem/AgcmItem.h>
#include <AgcmUIControl/AgcmUIControl.h>
#include <AgpmSkill/AgpmSkill.h>

const INT64 ONE_DAY		= 24*60*60*1000;
const INT32 ONE_HOUR	= 60*60*1000;
const INT32 ONE_MINUTE	= 60*1000;


AgcmUICashInven::AgcmUICashInven() 
{
	SetModuleName("AgcmUICashInven");

	m_pcsAgpmCharacter = NULL;
	m_pcsAgpmGrid = NULL;
	m_pcsAgpmItem = NULL;
	m_pcsAgpmCashMall = NULL;
	m_pcsAgpmSkill = NULL;
	m_pcsAgpmSystemMessage = NULL;

	m_pcsAgcmCharacter = NULL;
	m_pcsAgcmCashMall = NULL;
	m_pcsAgcmUIManager2	= NULL;
	m_lCashItemCount = 0;
	m_pcsAgcmItem = NULL;
	m_pcsAgcmUIItem = NULL;
	m_pcsAgcmUIControl = NULL;

	m_lCurrentTab = AGCMUICASHINVEN_TAB_ID_ALL;

	m_bCashInvenOpenedUI = FALSE;

	m_pcsUDStamina = NULL;

	m_pcsPetState = NULL;
	m_bAddPetStateGrid = FALSE;
}

AgcmUICashInven::~AgcmUICashInven()
{
}

/*
	2005.11.10. By SungHoon
	���� ��⿡�� ���Ǵ� ���� ����� �����ϱ� ���� ����Ѵ�.
	�� ��⿡�� Call-Back���� �Ҹ����� static �迭�� �Լ��� ����Ѵ�.
	UI���� ó���� Event�� ����Ѵ�.
	UI������ ���� ��ϵ� �Լ��� ����Ѵ�.
	UI ��¿� �ʿ��� �Լ��� ����Ѵ�.
	���뵥��Ÿ�� ����Ѵ�.
*/
BOOL AgcmUICashInven::OnAddModule()
{
//	1st ���� GetModule ���� �ϰ�
	m_pcsAgpmCharacter = (AgpmCharacter *)GetModule("AgpmCharacter");
	m_pcsAgpmGrid = (AgpmGrid *)GetModule("AgpmGrid");
	m_pcsAgpmItem = (AgpmItem *)GetModule("AgpmItem");
	m_pcsAgpmCashMall = (AgpmCashMall *)GetModule("AgpmCashMall");
	m_pcsAgpmSkill = (AgpmSkill *)GetModule("AgpmSkill");
	m_pcsAgpmSystemMessage = (AgpmSystemMessage *)GetModule("AgpmSystemMessage");

	m_pcsAgcmCharacter = (AgcmCharacter *)GetModule("AgcmCharacter");
	m_pcsAgcmItem = (AgcmItem *)GetModule("AgcmItem");
	m_pcsAgcmCashMall = (AgcmCashMall *)GetModule("AgcmCashMall");
	m_pcsAgcmUIManager2	= (AgcmUIManager2 *)GetModule("AgcmUIManager2");
	m_pcsAgcmUIMain = (AgcmUIMain *)GetModule("AgcmUIMain");
	m_pcsAgcmUIItem = (AgcmUIItem *)GetModule("AgcmUIItem");
	m_pcsAgcmUIControl = (AgcmUIControl *)GetModule("AgcmUIControl");

	if (!m_pcsAgpmCharacter || !m_pcsAgpmGrid || !m_pcsAgpmItem || !m_pcsAgpmCashMall || !m_pcsAgpmSkill ||
		!m_pcsAgcmCharacter || !m_pcsAgcmCashMall || !m_pcsAgcmUIManager2 || !m_pcsAgcmItem ||
		!m_pcsAgcmUIMain || !m_pcsAgcmUIItem || !m_pcsAgcmUIControl)	return FALSE;
//	2nd �ݹ� ���
	if (!m_pcsAgpmItem->SetCallbackUpdateItemCashInventory(CBUpdateItemCashInventory, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackAddItemCashInventory(CBAddCashItem, this))	// ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemoveItemCashInventory(CBRemoveCashItem, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if(!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmUIMain->SetCallbackKeydownESC(CBKeydownESC, this))
		return FALSE;

	if (!m_pcsAgcmUIItem->SetCallbackGetItemTimeInfo(CBGetItemTimeInfo, this))
		return FALSE;
	if (!m_pcsAgcmUIItem->SetCallbackGetItemStaminaTime(CBGetItemStaminaTime, this))
		return FALSE;

//	3rd �̺�Ʈ ���
	if (!AddEvent())
		return FALSE;

//	4th UI �����Լ� ���
	if (!AddFunction())
		return FALSE;

//	5th UI Custom-Display �Լ� ���
	if (!AddDisplay())
		return FALSE;

//	6th Control �鿡 ����� ���� ����Ÿ ���
	if (!AddUserData())
		return FALSE;

//	7th Contril �鿡 ����� Boolean ���� ����
	if (!AddBoolean())
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	�ʱ�ȭ ���� �۾��� �Ѵ�.
*/
BOOL AgcmUICashInven::OnInit()
{
	for (int i = 0; i < AGCMUICASHINVENTORY_MAX_ITEM; ++i)
		m_pcsAgpmGrid->Init(&m_acsGridCashItemList[i], 1, 1, 1 );
	return TRUE;
}

/*
	2005.11.10. By SungHoon
	���� ���� �۾��� �Ѵ�.
*/
BOOL AgcmUICashInven::OnDestroy()
{
	for (int i = 0; i < AGCMUICASHINVENTORY_MAX_ITEM; ++i)
		m_pcsAgpmGrid->Remove(&m_acsGridCashItemList[i]);

	if(m_pcsPetState)
	{
		delete m_pcsPetState;
		m_pcsPetState = NULL;
	}

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	��� ���뵥���͸� ����Ѵ�.
*/
BOOL AgcmUICashInven::AddUserData()
{
	m_pcsUDCashItemList = m_pcsAgcmUIManager2->AddUserData("CInven_Grid_Item_List", m_acsGridCashItemList, sizeof(AgpdGrid), AGCMUICASHINVENTORY_MAX_ITEM, AGCDUI_USERDATA_TYPE_GRID);
	if(!m_pcsUDCashItemList)
		return FALSE;

	m_pcsUDStamina = m_pcsAgcmUIManager2->AddUserData("CInven_Stamina", &m_lUDStamina, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pcsUDStamina)
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UI ����(Ŭ����)�� ���� �Լ��� UITool�� ����ϱ� ���� �̸� ��� �Ѵ�.
*/
BOOL AgcmUICashInven::AddFunction()
{
	if(!m_pcsAgcmUIManager2->AddFunction(this, "CInven_UIOpen", CBCashInvenOpenUI, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "CInven_ClickUseItem", CBCashInvenClickUseItem, 0))
		return FALSE;
	
	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UITool�� ��ϵ� �̺�Ʈ�� ���� Ư�� ������ ���� ������ �����ϴ� �Լ��� ����Ѵ�.
*/
BOOL AgcmUICashInven::AddDisplay()
{
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_UseItemButton", 0, CBDisplayCashInvenUseItemButtonText, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_UsingItem", 0, CBDisplayCashInvenUsingItem, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_ItemInfo", 0, CBDisplayCashInvenItemInfoTime, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_ItemInfoName", 0, CBDisplayCashInvenItemInfoName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_ItemInfoCount", 0, CBDisplayCashInvenItemInfoCount, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_ItemEnableTrade", 0, CBDisplayCashInvenEnableTrade, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_StaminaMax", 0, CBDisplayCashInvenStaminaMax, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CInven_StaminaCurrent", 0, CBDisplayCashInvenStaminaCurrent, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UITool�� ��ϵ� �̺�Ʈ�� ������ �߰��Ѵ�.
*/
BOOL AgcmUICashInven::AddEvent()
{
	m_lEventOpenCashInvenUI = m_pcsAgcmUIManager2->AddEvent("CInven_OpenUI");
	if(m_lEventOpenCashInvenUI < 0)
		return FALSE;

	m_lEventCloseCashInvenUI = m_pcsAgcmUIManager2->AddEvent("CInven_CloseUI");
	if(m_lEventCloseCashInvenUI < 0)
		return FALSE;

#ifdef _AREA_TAIWAN_
	AgcmUICouponBox * couponBox = (AgcmUICouponBox *)GetModule( "AgcmUICouponBox" );

	if( couponBox )
		m_lEventOpenCouponBox = couponBox->m_lEventOpenCashInvenUI;
#endif

	return TRUE;
}

/*
	2005.11.10. By SungHoon
*/
BOOL AgcmUICashInven::AddBoolean()
{
	return TRUE;
}

/*
	2005.11.10. By SungHoon
*/
BOOL AgcmUICashInven::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	return TRUE;
}

BOOL AgcmUICashInven::OnUpdateCashItemCount( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CashInven" );
	if( !pcdUI ) return FALSE;

	AgcdUIControl* pcdUIControl = pcmUIManager->GetControl( pcdUI, "Text_CashItemCount" );
	if( !pcdUIControl ) return FALSE;

	AcUIEdit* pEdit = ( AcUIEdit* )pcdUIControl->m_pcsBase;
	if( !pEdit ) return FALSE;

	char* pTextFormat = pcmUIManager->GetUIMessage( "TextFormat_CashItemCount" );
	if( !pTextFormat || strlen( pTextFormat ) <= 0 ) return FALSE;

	char strBuffer[ 256 ] = { 0, };
	sprintf_s( strBuffer, sizeof( char ) * 256, pTextFormat, m_lCashItemCount, 200 );
	pEdit->SetStaticString( strBuffer );

	return TRUE;
}

/*
	2005.11.14. By SungHoon
	ĳ�� �������� ���/���� �ؽ�Ʈ�� �ѷ��ش�.
*/
BOOL AgcmUICashInven::CBDisplayCashInvenUseItemButtonText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl)
{
	strcpy(szDisplay, "");

	if (!pClass || !pData || !pcsSourceControl) return FALSE;
	if (pcsSourceControl->m_lType != AcUIBase::TYPE_BUTTON) return FALSE;
	AcUIButton * pUIButton = (AcUIButton *) pcsSourceControl->m_pcsBase;
	if (!pUIButton) return FALSE;

	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if (!pcsItem->m_pcsItemTemplate) return FALSE;

	if ( pcsItem->m_pcsItemTemplate->m_eCashItemUseType == AGPMITEM_CASH_ITEM_USE_TYPE_UNUSABLE)
	{
		pUIButton->ShowWindow( FALSE );
		return TRUE;
	}
	pUIButton->ShowWindow( TRUE );

	CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_USE);

	if (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)	//	�����
	{
		BOOL bEnableStopCashItem = pThis->m_pcsAgpmItem->CheckEnableStopCashItem(pcsItem);
		if (bEnableStopCashItem)
		{
			if((pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME ||
				pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_STAMINA) &&
				pcsItem->m_pcsItemTemplate->m_bContinuousOff)
				szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_WAIT);
			else
				szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_NO_USE);

			pUIButton->SetButtonEnable( TRUE );
		}
		else
			pUIButton->SetButtonEnable( FALSE );
	}
	else if(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE)	// �Ͻ� ����
	{
		BOOL bEnableStopCashItem = pThis->m_pcsAgpmItem->CheckEnableStopCashItem(pcsItem);
		szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_REUSE);
		if (bEnableStopCashItem)
		{
			pUIButton->SetButtonEnable( TRUE );
		}
		else
			pUIButton->SetButtonEnable( FALSE );
	}
	else
	{
		if(pcsItem->m_lCashItemUseCount == 0)
			szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_NO_USE_YET);

		pUIButton->SetButtonEnable( TRUE );
	}
	if (szUIMessage) strcpy( szDisplay, szUIMessage );
	return TRUE;
}

/*
	2005.12.16. by SungHoon
	������ �̸� �����ִ� Display �Լ�
*/
BOOL AgcmUICashInven::CBDisplayCashInvenItemInfoName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	AcUIEdit* pUIEdit = (AcUIEdit *) pcsSourceControl->m_pcsBase;
	if (!pUIEdit) return FALSE;

	pUIEdit->SetText("");
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid , 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate) return FALSE;

	if (pcsItem->m_pcsItemTemplate->m_szName)
		pUIEdit->SetText(pcsItem->m_pcsItemTemplate->m_szName);
	return TRUE;
}

/*
	2005.12.16. by SungHoon
	������ �������� �����ִ� Display �Լ�
*/
BOOL AgcmUICashInven::CBDisplayCashInvenItemInfoCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	AcUIEdit* pUIEdit = (AcUIEdit *) pcsSourceControl->m_pcsBase;
	if (!pUIEdit) return FALSE;

	pUIEdit->SetText("");
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid , 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate) return FALSE;

	CHAR szText[256] = { 0 };
	CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_COUNT);;

	if (szUIMessage)
	{
		INT32	lAdjustCount	= 0;

		if (IS_CASH_ITEM(pcsItem->m_pcsItemTemplate->m_eCashItemType) &&
			(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&
			pcsItem->m_pcsItemTemplate &&
			pcsItem->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
			((AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL &&
			(pcsItem->m_lRemainTime > 0 || pcsItem->m_lExpireTime != 0))
		{
			++lAdjustCount;
		}
		
		_snprintf_s(pcsGridItem->m_strRightBottom, AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1, _TRUNCATE, "%d", pcsItem->m_nCount - lAdjustCount);

		wsprintf(szText, szUIMessage, pcsItem->m_nCount - lAdjustCount );
		pUIEdit->SetText(szText);
	}
	return TRUE;
}

BOOL AgcmUICashInven::SetItemInfoTime(AgpdItem *pcsItem, CHAR *pszText, eAgcmUICashInven_CallType eCallType)
{
	if (!pcsItem || !pszText)
		return FALSE;

	// Stamina �ִ� ���� ���ǰ ������ �ѷ��ְ� ������. 2008.06.16. steeple
	if(eCallType == AGCMUICASHINVEN_CALLTYPE_CASH_INVEN &&
		pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_STAMINA &&
		ClientStr().GetStr(STI_CASH_INVEN_HUNGRY) != NULL)
	{
		_tcsncpy(pszText, ClientStr().GetStr(STI_CASH_INVEN_HUNGRY), _tcslen(ClientStr().GetStr(STI_CASH_INVEN_HUNGRY)));
		return TRUE;
	}

	CHAR	*szUIMessage	= NULL;

	if (pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_PLAY_TIME
			&& (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE))
	{
		INT64 lRemainTime = pcsItem->m_lRemainTime;

		if (lRemainTime >= ONE_DAY)	// 1��
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_DAY);
			if (szUIMessage)
			{
				short day = ( short ) ( lRemainTime/ONE_DAY ) ;
				short hour = ( short ) ( (lRemainTime - (day*ONE_DAY))/ONE_HOUR ) ;

				wsprintf( pszText, szUIMessage, day, hour);
			}
		}
		else if (lRemainTime >= ONE_HOUR)	// 1�ð�
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_HOUR);
			if (szUIMessage)
			{
				short hour = ( short) ( lRemainTime/ONE_HOUR );
				short minute = ( short ) ((lRemainTime - (hour*ONE_HOUR))/ONE_MINUTE ) ;

				wsprintf( pszText, szUIMessage, hour, minute);
			}
		}
		else if (lRemainTime >= ONE_MINUTE)	// 1��
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_MINUTE);
			if (szUIMessage)
			{
				wsprintf( pszText, szUIMessage, lRemainTime/ONE_MINUTE);
			}
		}
		else if (lRemainTime < ONE_MINUTE)		// 1�� �̸�
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_SECOND);
			if (szUIMessage)
			{
				wsprintf( pszText, szUIMessage, lRemainTime/1000);
			}
		}
	}
	else if (pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME
				&& (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE))
	{
		UINT32 lExpireTime = pcsItem->m_lExpireTime;
		if( lExpireTime != 0 && lExpireTime != 0xFFFFFFFF )
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_REAL_TIME);

			if (lExpireTime != 0 && lExpireTime != 0xFFFFFFFF )
			{
				TCHAR szDateString[AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1];
				AuTimeStamp::FormatTimeString(lExpireTime, "%y/%m/%d %H:%M", szDateString, AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1);

				if (szUIMessage)
				{
					wsprintf( pszText, szUIMessage, szDateString);
				}
			}
			else
			{
				if (szUIMessage)
				{
					wsprintf( pszText, szUIMessage, "-");
				}
			}
		}
	}
	else if(pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_NONE)
	{
		// �Ϲ� ������ ������ ������ �ð� UI ó��.
		UINT32 lExpireTime = pcsItem->m_lExpireTime;
		if( lExpireTime != 0 && lExpireTime != 0xFFFFFFFF )
		{
			szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO_REAL_TIME);

			if (lExpireTime != 0 && lExpireTime != 0xFFFFFFFF)
			{
				TCHAR szDateString[AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1];
				AuTimeStamp::FormatTimeString(lExpireTime, "%y/%m/%d %H:%M", szDateString, AUTIMESTAMP_SIZE_ORACLE_TIME_STRING+1);

				if (szUIMessage)
				{
					wsprintf( pszText, szUIMessage, szDateString);
				}
			}
			else
			{
				if (szUIMessage)
				{
					wsprintf( pszText, szUIMessage, "-");
				}
			}
		}
	}
	else
	{
		szUIMessage = m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_DETAIL_INFO);
		if (szUIMessage)
		{
			wsprintf( pszText, szUIMessage, " ");
		}
	}

	return TRUE;
}

BOOL AgcmUICashInven::GetItemStaminaTime(AgpdItem* pcsItem, CHAR* pszText)
{
	if(!pcsItem || !pszText)
		return FALSE;

	if(pcsItem->m_llStaminaRemainTime == 0 || pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime == 0)
		return TRUE;

	INT64 lRemainTime = pcsItem->m_llStaminaRemainTime;
	CHAR* szUIMessage = NULL;

	szUIMessage = ClientStr().GetStr(STI_REMAIN_TIME);
	if(szUIMessage)
		_tcscpy(pszText, szUIMessage);

	CHAR szTmp[128];
	ZeroMemory(szTmp, sizeof(szTmp));

	if (lRemainTime >= ONE_DAY)	// 1��
	{
		int day = ( int ) ( lRemainTime/ONE_DAY ) ;
		int hour = ( int ) ( (lRemainTime - (day*ONE_DAY))/ONE_HOUR ) ;

		wsprintf(szTmp, " : %d%s %d%s", day, ClientStr().GetStr(STI_DAY), hour, ClientStr().GetStr(STI_HOUR));
		_tcscat(pszText, szTmp);
	}
	else if (lRemainTime >= ONE_HOUR)	// 1�ð�
	{
		int hour = ( int) ( lRemainTime/ONE_HOUR );
		int minute = ( int ) ((lRemainTime - (hour*ONE_HOUR))/ONE_MINUTE ) ;

		wsprintf(szTmp, " : %d%s %d%s", hour, ClientStr().GetStr(STI_HOUR), minute, ClientStr().GetStr(STI_MINUTE));
		_tcscat(pszText, szTmp);
	}
	else if (lRemainTime >= ONE_MINUTE)	// 1��
	{
		int minute = ( int )( ( float )lRemainTime / ( float )ONE_MINUTE );
		int second = (int)(( lRemainTime - ( minute * ONE_MINUTE ) ) / 1000);
		wsprintf(szTmp, " : %d%s %d%s", minute, ClientStr().GetStr(STI_MINUTE), second, ClientStr().GetStr(STI_SECOND) );
		_tcscat(pszText, szTmp);
	}
	else if (lRemainTime < ONE_MINUTE)		// 1�� �̸�
	{
		int second = ( int )( ( float )lRemainTime / ( float )1000 );
		wsprintf(szTmp, " : %d%s", second, ClientStr().GetStr(STI_SECOND));
		_tcscat(pszText, szTmp);
	}

	return TRUE;
}

/*
	2005.11.14. By SungHoon
	ĳ�� �������� ������ �����ش�.
*/
BOOL AgcmUICashInven::CBDisplayCashInvenItemInfoTime(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	AcUIEdit* pUIEdit = (AcUIEdit *) pcsSourceControl->m_pcsBase;
	if (!pUIEdit) return FALSE;

	pUIEdit->SetText("");
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid , 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem || !pcsItem->m_pcsItemTemplate) return FALSE;

	CHAR szText[256] = { 0 };

	if (!pThis->SetItemInfoTime(pcsItem, szText))
		return FALSE;

	return pUIEdit->SetText(szText);
}

BOOL AgcmUICashInven::CBDisplayCashInvenUsingItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	strcpy(szDisplay, "");

	if (!pClass || !pData) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if (!pcsItem->m_pcsItemTemplate) return FALSE;

	if (pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE)
	{
		CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_USING);
		if (szUIMessage) strcpy( szDisplay, szUIMessage );
	}
	else if(pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_UNUSE)
	{
		CHAR *szUIMessage = NULL;

		if( pcsItem->m_lCashItemUseCount > 0 )
		{
			if(pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_REAL_TIME && pcsItem->m_pcsItemTemplate->m_bContinuousOff)
				szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_HOLDING);
			else if ( pcsItem->m_pcsItemTemplate->m_eCashItemUseType == AGPMITEM_CASH_ITEM_USE_TYPE_CONTINUOUS )
				szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_CASH_ITEM_PAUSE);
		}

		if (szUIMessage) strcpy( szDisplay, szUIMessage );
	}

	return TRUE;
}

BOOL AgcmUICashInven::CBDisplayCashInvenEnableTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pData || !pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICashInven* pThis = (AgcmUICashInven*)pClass;
	AgpdGrid *pcsGrid	= (AgpdGrid *)pData;
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if(pcsItem->m_lCashItemUseCount > 0 && pThis->m_pcsAgpmItem->IsUsingStamina(pcsItem->m_pcsItemTemplate) == FALSE)
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);
	else
		pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[1].m_lStatusID);

	return TRUE;
}
/*
	2005.11.14. By SungHoon
	������ ��� ��ư�� Ŭ���Ѵ�.
*/
BOOL AgcmUICashInven::CBCashInvenClickUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lCashItemCount)
		return FALSE;

	AgpdGrid *pcsGrid = &pThis->m_acsGridCashItemList[pcsSourceControl->m_lUserDataIndex];
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if (!pcsItem->m_pcsItemTemplate) return FALSE;

	AgpmItem* ppmItem = ( AgpmItem* )g_pEngine->GetModule( "AgpmItem" );
	AgcmCharacter* pcmCharacter = ( AgcmCharacter* )g_pEngine->GetModule( "AgcmCharacter" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !ppmItem || !pcmCharacter || !pcmUIManager ) return FALSE;

	AgpdCharacter* ppdSelfCharacter = pcmCharacter->GetSelfCharacter();
	if( !ppdSelfCharacter ) return FALSE;

	// �ƹ�Ÿ �������̰� �������ΰ��..
	if( ppmItem->IsAvatarItem( pcsItem->m_pcsItemTemplate ) && ppdSelfCharacter->m_bIsTrasform )
	{
		// �� �������� ���� ������ΰ�� ������� ����
		if( pcsItem->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE )
		{
			char* pMsg = pcmUIManager->GetUIMessage( "CannotUnEquipAvatarOnTransform" );
			if( !pMsg || strlen( pMsg ) <= 0 ) return FALSE;

			pcmUIManager->ActionMessageOKDialog( pMsg );
			return TRUE;
		}
	}

	if (pcsItem->m_pcsItemTemplate->m_eCashItemType == AGPMITEM_CASH_ITEM_TYPE_EQUIP)
		pThis->m_pcsAgcmItem->SendEquipInfo(pcsItem->m_lID, pThis->m_pcsAgcmCharacter->GetSelfCID());
	else
		pThis->m_pcsAgcmItem->UseItem(pcsItem);

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	�׸��带 �ٽ� �׷��ش�.
*/
BOOL AgcmUICashInven::RefreshItemGrid()
{
	m_pcsUDCashItemList->m_stUserData.m_lCount = m_lCashItemCount;
	m_pcsUDCashItemList->m_bUpdateList = TRUE;

	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUDCashItemList, TRUE);
	m_pcsAgcmUIManager2->RefreshUserData(m_pcsUDStamina, TRUE);

	return TRUE;
}

/*
	2005.11.14. By SungHoon
	ĳ���κ��丮�� �� �غ� �ϰ� UIOpen-Event�� Throw �Ѵ�.
*/
BOOL AgcmUICashInven::CBCashInvenOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;

	// �Ϻ��� ĳ���κ��丮 ���´�. ����Ű�� �����Եȴ�.
	//if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
	//	return TRUE;

	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;
	return (pThis->CashInvenOpenUI());
}

/*
	2005.11.16. By SungHoon
	ĳ���κ�Ʈ�θ��� Ư�� �������� �߰��Ǵ� �����Ǿ��ٰ� �뺸�� �Դ�.
*/
BOOL AgcmUICashInven::CBUpdateItemCashInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven * )pClass;

	pThis->RefreshItemGrid();
	pThis->m_pcsAgcmUIMain->RefreshQBeltGridItemStackCount();

	// 2008.06.23. steeple
	// Check a sub inventory is enable.
	if(pThis->m_pcsAgpmItem->IsEnableSubInventory(pThis->m_pcsAgcmCharacter->GetSelfCharacter()))
	{
		pThis->m_pcsAgcmUIItem->EnableSubInventory();
	}
	else
	{
		pThis->m_pcsAgcmUIItem->DisableSubInventory();
		pThis->m_pcsAgcmUIItem->CloseSubInventory();
	}

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	ĳ���κ��丮UI�� �����ϰų� Close�Ѵ�.
*/
BOOL AgcmUICashInven::CashInvenOpenUI()
{
	if (m_bCashInvenOpenedUI)
	{
		m_bCashInvenOpenedUI = FALSE;
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseCashInvenUI);

#ifdef _AREA_TAIWAN_
		AgcmUICouponBox * couponBox = (AgcmUICouponBox *)GetModule( "AgcmUICouponBox" );

		if( couponBox )
			couponBox->Close();
#endif
	}
	else
	{
		m_bCashInvenOpenedUI = TRUE;
		RefreshItemGrid();
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenCashInvenUI);

#ifdef _AREA_TAIWAN_
		AgcmUICouponBox * couponBox = (AgcmUICouponBox *)GetModule( "AgcmUICouponBox" );

		if( couponBox )
			couponBox->Open();
#endif
	}
	return TRUE;
}

/*
	2005.11.17. By SungHoon
	�ڱ��÷��̾� ������ �ɰ�� �Ҹ���.
*/
BOOL AgcmUICashInven::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass) return FALSE;
	AgcmUICashInven *pThis = (AgcmUICashInven *)(pClass);
	AgpdCharacter *pCharacter	= (AgpdCharacter *)(pData);

	AgpdItemADChar *pItemADChar = pThis->m_pcsAgpmItem->GetADCharacter( pCharacter );

	if( pItemADChar != NULL )
	{
//		pThis->m_pcsGridCashItemList = &pItemADChar->m_csCashInventoryGrid;
//		pThis->m_pcsUDCashItemList->m_stUserData.m_pvData = pThis->m_pcsGridCashItemList;

//		pThis->RefreshItemGrid();
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

/*
	2005.11.17. By SungHoon
	�ڱ��÷��̾� ������ �����ɿ� �Ҹ���.
*/
BOOL AgcmUICashInven::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICashInven *pThis = (AgcmUICashInven *)(pClass);

	//AgpdCharacter* pcsSelfCharacter = pData ? static_cast<AgpdCharacter*>(pData) : NULL;
	AgcdCharacter* pstAgcdCharacter = pCustData ? static_cast<AgcdCharacter*>(pCustData) : NULL;

	// ���� ������ �Ҹ� �Ÿ� �������� �ʴ´�. 2006.03.30. steeple
	if(pstAgcdCharacter && pstAgcdCharacter->m_bTransforming)
	{
		return TRUE;
	}

	for (int i = 0; i < AGCMUICASHINVENTORY_MAX_ITEM; ++i)
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_acsGridCashItemList[i]);

	pThis->m_lCashItemCount	= 0;

	return TRUE;
}

BOOL AgcmUICashInven::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICashInven *pThis = (AgcmUICashInven *)(pClass);

	pThis->m_bCashInvenOpenedUI	= FALSE;

	return TRUE;
}

BOOL AgcmUICashInven::CBGetItemTimeInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICashInven* pThis = static_cast<AgcmUICashInven*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);
	CHAR* pszText = static_cast<CHAR*>(pCustData);

	pThis->SetItemInfoTime(pcsItem, pszText, AGCMUICASHINVEN_CALLTYPE_TOOLTIP);

	return TRUE;
}

BOOL AgcmUICashInven::CBGetItemStaminaTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICashInven* pThis = static_cast<AgcmUICashInven*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);
	CHAR* pszText = static_cast<CHAR*>(pCustData);

	pThis->GetItemStaminaTime(pcsItem, pszText);

	return TRUE;
}

/*
	2005.11.17. By SungHoon
	�ڱ��÷��̾� ������ ������� �Ҹ���.
*/
BOOL AgcmUICashInven::CBAddCashItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven * )pClass;

	return (pThis->AddCashItem(( AgpdItem * )pData, ( AgpdCharacter * )pCustData));
}

BOOL AgcmUICashInven::AddCashItem(AgpdItem *pItem, AgpdCharacter *pCharacter)
{
	if (!pItem || !pCharacter || m_lCashItemCount >= AGCMUICASHINVENTORY_MAX_ITEM) return FALSE;

	// 2006.01.06. steeple
	if(IsSelfOperation(pItem, pCharacter) == FALSE)
		return FALSE;

	m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[m_lCashItemCount]);
	m_pcsAgpmGrid->Add(&m_acsGridCashItemList[m_lCashItemCount], 0, 0, 0,  pItem->m_pcsGridItem, 1, 1);

	m_lCashItemCount++;

	OnUpdateCashItemCount();
	return TRUE;
}
/*
	2005.11.17. By SungHoon
	�ڱ��÷��̾� ������ ������� �Ҹ���.
*/
BOOL AgcmUICashInven::CBRemoveCashItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;

	AgcmUICashInven *pThis = ( AgcmUICashInven * )pClass;
	return (pThis->RemoveCashItem(( AgpdItem * )pData, ( AgpdCharacter * )pCustData));
}

BOOL AgcmUICashInven::RemoveCashItem(AgpdItem *pItem, AgpdCharacter *pCharacter)
{
	if (!pItem || !pCharacter || m_lCashItemCount == 0 ) return FALSE;

	// 2006.01.06. steeple
	if(IsSelfOperation(pItem, pCharacter) == FALSE)
		return FALSE;

	m_lCashItemCount = 0;
	int i;
	for (i = 0 ; i < AGCMUICASHINVENTORY_MAX_ITEM; i++)
	{
		AgpdGridItem *pcsGridItem = m_pcsAgpmGrid->GetItem(&m_acsGridCashItemList[i], 0, 0, 0);
		if (!pcsGridItem) continue;
		if (pcsGridItem == pItem->m_pcsGridItem) continue;
		
		m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[m_lCashItemCount]);
		m_pcsAgpmGrid->Add(&m_acsGridCashItemList[m_lCashItemCount], 0, 0, 0,  pcsGridItem, 1, 1);
		m_lCashItemCount++;
	}
	for (i = m_lCashItemCount ; i < AGCMUICASHINVENTORY_MAX_ITEM; i++)
	{
		m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[i]);
	}

	OnUpdateCashItemCount();
	return TRUE;
}

BOOL AgcmUICashInven::IsSelfOperation(AgpdItem* pItem, AgpdCharacter* pCharacter)
{
	if(!pItem || !pCharacter)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return TRUE;		// ���� ����ĳ���Ͱ� ���õǾ����� ���� ���¶�� TRUE �� ����������.

	return pcsSelfCharacter->m_lID == pCharacter->m_lID ? TRUE : FALSE;
}

BOOL AgcmUICashInven::CBDisplayCashInvenStaminaMax(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lCashItemCount)
		return FALSE;

	AgpdGrid *pcsGrid = &pThis->m_acsGridCashItemList[pcsSourceControl->m_lUserDataIndex];
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if (!pcsItem->m_pcsItemTemplate) return FALSE;

	*plValue = (INT32)pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime;
	sprintf(szDisplay, "%d", *plValue);

	if(pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime)
	{
		pcsSourceControl->m_pcsBase->m_Property.bVisible = TRUE;
		pThis->CheckPetStateGrid();
	}
	else
	{
		pcsSourceControl->m_pcsBase->m_Property.bVisible = FALSE;
	}

	return TRUE;
}

BOOL AgcmUICashInven::CBDisplayCashInvenStaminaCurrent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICashInven *pThis = ( AgcmUICashInven *)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lCashItemCount)
		return FALSE;

	AgpdGrid *pcsGrid = &pThis->m_acsGridCashItemList[pcsSourceControl->m_lUserDataIndex];
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);
	if (!pcsItem) return FALSE;
	if (!pcsItem->m_pcsItemTemplate) return FALSE;

	*plValue = (INT32)pcsItem->m_llStaminaRemainTime;
	sprintf(szDisplay, "%d", *plValue);

	return TRUE;
}

// Check pet state and show grid icon
// If you can find out at least one, show grid icon.
BOOL AgcmUICashInven::CheckPetStateGrid()
{
	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	AgpdItemADChar* pcsItemADChar = m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
	if(!pcsItemADChar)
		return FALSE;

	// Find pet is hungry.
	AgpdItem* pcsHungryPetItem = NULL;
	INT32 lIndex = 0;
	for(AgpdGridItem* pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex);
		pcsGridItem;
		pcsGridItem = m_pcsAgpmGrid->GetItemSequence(&pcsItemADChar->m_csCashInventoryGrid, &lIndex))
	{
		AgpdItem* pcsItem = m_pcsAgpmItem->GetItem(pcsGridItem);
		if(!pcsItem || !pcsItem->m_pcsItemTemplate || pcsItem->m_pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE)
			continue;

		if(pcsItem->m_llStaminaRemainTime == 0 || pcsItem->m_pcsItemTemplate->m_llStaminaRemainTime == 0)
			continue;

		INT32 lPetTID = m_pcsAgpmSkill->GetSummonsTIDByItem(pcsItem->m_pcsItemTemplate);
		AgpdCharacterTemplate* pcsCharacterTemplate = m_pcsAgpmCharacter->GetCharacterTemplate(lPetTID);
		if(!pcsCharacterTemplate)
			continue;

		// Check Stamina
		if(pcsItem->m_llStaminaRemainTime < pcsCharacterTemplate->m_lStaminaPoint * 1000)
		{
			pcsHungryPetItem = pcsItem;
			break;
		}
	}

	if(pcsHungryPetItem)
	{
		// Show hungry state grid icon.
		if(!m_pcsPetState)
		{
			m_pcsPetState = m_pcsAgpmGrid->CreateGridItem();
			if(m_pcsPetState)
			{
				m_pcsPetState->m_eType = AGPDGRID_ITEM_TYPE_ITEM;

				RwTexture** ppGridTexture = m_pcsAgcmUIControl->GetAttachGridItemTextureData(m_pcsPetState);
				if(ppGridTexture)
					*ppGridTexture	= m_pcsAgcmUIControl->m_pPetState;

				m_pcsPetState->SetTooltip(ClientStr().GetStr(STI_PET_HUNGRY_TOOLTIP));
			}
		}

		// push a system message
		if(m_pcsAgpmSystemMessage && m_bAddPetStateGrid == FALSE)
			SystemMessage.ProcessSystemMessage( 0 , AGPMSYSTEMMESSAGE_CODE_PET_IS_HUNGRY );

			
		m_pcsAgcmUIMain->AddSystemMessageGridItem(m_pcsPetState, NULL, NULL);
		m_bAddPetStateGrid = TRUE;
	}
	else
	{
		if(m_pcsPetState)
			m_pcsAgcmUIMain->RemoveSystemMessageGridItem(m_pcsPetState);

		m_bAddPetStateGrid = FALSE;
	}

	return TRUE;
}