#include <AgcmUICashItemCabient/AgcmUICouponBox.h>
#include <AuTimeStamp/AuTimeStamp.h>
#include <AuStrTable/AuStrTable.h>
#include <AgpmSystemMessage/AgpmSystemMessage.h>
#include <AuXmlParser/AuXmlParser.h>
#include <AgcmCashMall/AgcmCashMall.h>

#ifdef _AREA_TAIWAN_

AgcmUICouponBox::AgcmUICouponBox() 
{
	SetModuleName("AgcmUICouponBox");

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

	//m_lCurrentTab = AgcmUICouponBox_TAB_ID_ALL;

	m_bCashInvenOpenedUI = FALSE;

	m_pcsUDStamina = NULL;

	m_pcsPetState = NULL;
	m_bAddPetStateGrid = FALSE;
}

AgcmUICouponBox::~AgcmUICouponBox()
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
BOOL AgcmUICouponBox::OnAddModule()
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
	//if (!m_pcsAgpmItem->SetCallbackAddItemCashInventory(CBAddCashItem, this))	// ITEM_CB_ID_CHAR_ADD_ITEM_CASH_INVENTORY
	//	return FALSE;
	//if (!m_pcsAgpmItem->SetCallbackRemoveItemCashInventory(CBRemoveCashItem, this))
	//	return FALSE;
	if ( !m_pcsAgpmCharacter->SetCallBackRecieveCouponInfo( CBAddCoupon, this ) )
		return FALSE;
	if ( !m_pcsAgpmCharacter->SetCallBackRecieveCouponUse( CBUseCoupon, this ) )
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
BOOL AgcmUICouponBox::OnInit()
{
	for (int i = 0; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
		m_pcsAgpmGrid->Init(&m_acsGridCashItemList[i], 1, 1, 1 );

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	���� ���� �۾��� �Ѵ�.
*/
BOOL AgcmUICouponBox::OnDestroy()
{
	for (int i = 0; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
		m_pcsAgpmGrid->Remove(&m_acsGridCashItemList[i]);

	DestroyCouponItemAll();

	m_lCashItemCount = 0;

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
BOOL AgcmUICouponBox::AddUserData()
{
	m_pcsUDCashItemList = m_pcsAgcmUIManager2->AddUserData("CouponBox_Grid_Item_List", m_acsGridCashItemList, sizeof(AgpdGrid), AGCMUI_COUPON_BOX_MAX_COUNT, AGCDUI_USERDATA_TYPE_GRID);
	if(!m_pcsUDCashItemList)
		return FALSE;

	m_pcsUDStamina = m_pcsAgcmUIManager2->AddUserData("CouponBox_Stamina", &m_lUDStamina, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
	if(!m_pcsUDStamina)
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UI ����(Ŭ����)�� ���� �Լ��� UITool�� ����ϱ� ���� �̸� ��� �Ѵ�.
*/
BOOL AgcmUICouponBox::AddFunction()
{
	if(!m_pcsAgcmUIManager2->AddFunction(this, "CouponBox_UIOpen", CBCashInvenOpenUI, 0))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddFunction(this, "CouponBox_ClickUseItem", CBCashInvenClickUseItem, 0))
		return FALSE;
	
	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UITool�� ��ϵ� �̺�Ʈ�� ���� Ư�� ������ ���� ������ �����ϴ� �Լ��� ����Ѵ�.
*/
BOOL AgcmUICouponBox::AddDisplay()
{
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_UseItemButton", 0, CBDisplayCashInvenUseItemButtonText, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_UsingItem", 0, CBDisplayCashInvenUsingItem, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	//if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_ItemInfo", 0, CBDisplayCashInvenItemInfoTime, AGCDUI_USERDATA_TYPE_GRID))
	//	return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_ItemInfoName", 0, CBDisplayCashInvenItemInfoName, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_ItemInfoCount", 0, CBDisplayCashInvenItemInfoCount, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_ItemEnableTrade", 0, CBDisplayCashInvenEnableTrade, AGCDUI_USERDATA_TYPE_GRID))
		return FALSE;

	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_StaminaMax", 0, CBDisplayCashInvenStaminaMax, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;
	if(!m_pcsAgcmUIManager2->AddDisplay(this, "CouponBox_StaminaCurrent", 0, CBDisplayCashInvenStaminaCurrent, AGCDUI_USERDATA_TYPE_INT32))
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
	UITool�� ��ϵ� �̺�Ʈ�� ������ �߰��Ѵ�.
*/
BOOL AgcmUICouponBox::AddEvent()
{
	m_lEventOpenCashInvenUI = m_pcsAgcmUIManager2->AddEvent("CouponBox_OpenUI");
	if(m_lEventOpenCashInvenUI < 0)
		return FALSE;

	m_lEventCloseCashInvenUI = m_pcsAgcmUIManager2->AddEvent("CouponBox_CloseUI");
	if(m_lEventCloseCashInvenUI < 0)
		return FALSE;

	return TRUE;
}

/*
	2005.11.10. By SungHoon
*/
BOOL AgcmUICouponBox::AddBoolean()
{
	return TRUE;
}

/*
	2005.11.10. By SungHoon
*/
BOOL AgcmUICouponBox::OnReceive(UINT32 ulType, PVOID pvPacket, INT16 nSize, UINT32 ulNID, DispatchArg *pstCheckArg)
{
	return TRUE;
}

BOOL AgcmUICouponBox::OnUpdateCashItemCount( void )
{
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !pcmUIManager ) return FALSE;

	AgcdUI* pcdUI = pcmUIManager->GetUI( "UI_CouponBox" );
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
	ĳ�� �������� ���/���� �ؽ�Ʈ�� �ѷ�
*/
BOOL AgcmUICouponBox::CBDisplayCashInvenUseItemButtonText(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue , AgcdUIControl *pcsSourceControl)
{
	strcpy(szDisplay, "");

	if (!pClass || !pData || !pcsSourceControl) return FALSE;
	if (pcsSourceControl->m_lType != AcUIBase::TYPE_BUTTON) return FALSE;
	AcUIButton * pUIButton = (AcUIButton *) pcsSourceControl->m_pcsBase;
	if (!pUIButton) return FALSE;

	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;
	AgpdItem *pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsGridItem);

	pUIButton->ShowWindow( TRUE );

	CHAR *szUIMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage(UI_MESSAGE_ID_COUPON_USE);

	if (szUIMessage) strcpy( szDisplay, szUIMessage );
	return TRUE;
}

/*
	2005.12.16. by SungHoon
	������ �̸� �����ִ� Display �Լ�
*/
BOOL AgcmUICouponBox::CBDisplayCashInvenItemInfoName(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	AcUIEdit* pUIEdit = (AcUIEdit *) pcsSourceControl->m_pcsBase;
	if (!pUIEdit) return FALSE;

	pUIEdit->SetText("");

	if( (size_t)pcsSourceControl->m_lUserDataIndex < pThis->couponItems_.size() )
	{
		pUIEdit->SetText( pThis->couponItems_[ pcsSourceControl->m_lUserDataIndex ].info.m_szCouponTitle );
	}

	return TRUE;
}

/*
	2005.12.16. by SungHoon
	������ �������� �����ִ� Display �Լ�
*/
BOOL AgcmUICouponBox::CBDisplayCashInvenItemInfoCount(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pData) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	if (pcsSourceControl->m_lType != AcUIBase::TYPE_EDIT) return FALSE;
	AcUIEdit* pUIEdit = (AcUIEdit *) pcsSourceControl->m_pcsBase;
	if (!pUIEdit) return FALSE;

	pUIEdit->SetText("");

	return TRUE;
}

BOOL AgcmUICouponBox::CBDisplayCashInvenUsingItem(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue)
{
	strcpy(szDisplay, "");

	if (!pClass || !pData) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;
	AgpdGrid *pGrid = ( AgpdGrid *)pData;

	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	if( szDisplay )
		szDisplay[0] = 0;

	return TRUE;
}

BOOL AgcmUICouponBox::CBDisplayCashInvenEnableTrade(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if(!pData || !pClass || !pcsSourceControl)
		return FALSE;

	AgcmUICouponBox* pThis = (AgcmUICouponBox*)pClass;
	AgpdGrid *pcsGrid	= (AgpdGrid *)pData;
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	pcsSourceControl->m_pcsBase->SetStatus(pcsSourceControl->m_pcsBase->m_astStatus[0].m_lStatusID);

	return TRUE;
}
/*
	2005.11.14. By SungHoon
	������ ��� ��ư�� Ŭ���Ѵ�.
*/
BOOL AgcmUICouponBox::CBCashInvenClickUseItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lCashItemCount)
		return FALSE;

	AgpdGrid *pcsGrid = &pThis->m_acsGridCashItemList[pcsSourceControl->m_lUserDataIndex];
	AgpdGridItem *pcsGridItem = pThis->m_pcsAgpmGrid->GetItem(pcsGrid, 0, 0, 0);
	if (!pcsGridItem) return FALSE;

	pThis->usedCouponIdx_ = pcsSourceControl->m_lUserDataIndex;

	pThis->m_pcsAgcmCharacter->SendRequestCouponUse( 
		pThis->m_pcsAgcmCharacter->GetSelfCID()
		, pThis->couponItems_[ pcsSourceControl->m_lUserDataIndex ].info 
	);

	return TRUE;
}

/*
	2005.11.16. By SungHoon
	�׸��带 �ٽ� �׷��ش�.
*/
BOOL AgcmUICouponBox::RefreshItemGrid()
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
BOOL AgcmUICouponBox::CBCashInvenOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;

	// �Ϻ��� ĳ���κ��丮 ���´�. ����Ű�� �����Եȴ�.
	//if (AP_SERVICE_AREA_JAPAN == g_eServiceArea)
	//	return TRUE;

	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;
	return (pThis->CashInvenOpenUI());
}

/*
	2005.11.16. By SungHoon
	ĳ���κ�Ʈ�θ��� Ư�� �������� �߰��Ǵ� �����Ǿ��ٰ� �뺸�� �Դ�.
*/
BOOL AgcmUICouponBox::CBUpdateItemCashInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox * )pClass;

	pThis->RefreshItemGrid();

	// 2007.09.03. steeple
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

// �����ڽ� ���� ����
BOOL AgcmUICouponBox::CBAddCoupon(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass||!pData) return FALSE;
	
	AgcmUICouponBox *pThis = ( AgcmUICouponBox * )pClass;

	PACKET_CHARACTER_COUPON_INFO * infoPacket = (PACKET_CHARACTER_COUPON_INFO *)pData;

	for( int i=0; i<AGPPACCOUNT_MAX_COUPON_PACKET; ++i )
	{
		pThis->AddCoupon( infoPacket->m_stCouponInfo[i] );
	}

	pThis->OnUpdateCashItemCount();

	pThis->RefreshItemGrid();

	return TRUE;
}

BOOL AgcmUICouponBox::AddCoupon( stCouponInfo & couponInfo )
{
	CouponDisplayInfo const & displayInfo = CouponDisplayInfo::get( couponInfo.m_lCouponType );

	if( couponInfo.m_szCouponNo[0] == 0 )
		return FALSE;
	
	AgpdItem * dummyItem = m_pcsAgcmCashMall->CreateDummyItem( displayInfo.iconItemID, 1 );

	if( dummyItem )
	{
		dummyItem->m_lTID = displayInfo.iconItemID;

		dummyItem->m_pcsGridItem->m_eType = AGPDGRID_ITEM_TYPE_ITEM;
		dummyItem->m_pcsGridItem->m_bMoveable = FALSE;
		dummyItem->m_pcsGridItem->m_lItemID = dummyItem->GetID();
		dummyItem->m_pcsGridItem->m_lItemTID = displayInfo.iconItemID;
		//dummyItem->m_pcsGridItem->SetTooltip( m_pcsAgcmUIManager2->GetUIMessage( (char*)displayInfo.toolTipID.c_str() ) );

		m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[m_lCashItemCount]);
		m_pcsAgpmGrid->Add(&m_acsGridCashItemList[m_lCashItemCount], 0, 0, 0,  dummyItem->m_pcsGridItem, 1, 1);

		couponItems_.push_back( CouponItem( couponInfo, dummyItem ) );

		++m_lCashItemCount;
	}

	return TRUE;
}

// �����ڽ� ���� ����
BOOL AgcmUICouponBox::CBUseCoupon(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass||!pData) return FALSE;

	AgcmUICouponBox *pThis = ( AgcmUICouponBox * )pClass;

	PACKET_CHARACTER_ANSWER_COUPON_USE * useResult = (PACKET_CHARACTER_ANSWER_COUPON_USE *)pData;

	std::stringstream stream;

	stream << "CouponResult_" << (useResult->m_lResultCode);

	std::string resultStr = stream.str();

	pThis->m_pcsAgcmUIManager2->ActionMessageOKDialog( pThis->m_pcsAgcmUIManager2->GetUIMessage( (char*)(resultStr.c_str()) ) );

	if( useResult->m_lResultCode == 0 )
	{
		pThis->RemoveCoupon( pThis->usedCouponIdx_ );
	}

	pThis->RefreshItemGrid();

	return TRUE;
}


/*
	2005.11.16. By SungHoon
	ĳ���κ��丮UI�� �����ϰų� Close�Ѵ�.
*/
BOOL AgcmUICouponBox::CashInvenOpenUI()
{
	if (m_bCashInvenOpenedUI)
	{
		Close();
	}
	else
	{
		Open();
	}

	return TRUE;
}

void AgcmUICouponBox::EraseCouponItem( size_t idx )
{
	if( idx < couponItems_.size() )
	{
		CouponItems::iterator iter = couponItems_.begin() + idx;
		if( iter->item )
		{
			eraseCouponList_[ iter->item->GetID() ] = iter->item;
		}

		couponItems_.erase( iter );
	}
}

void AgcmUICouponBox::EraseCouponItemAll()
{
	for( CouponItems::iterator iter = couponItems_.begin(); iter != couponItems_.end(); ++iter )
	{
		if( iter->item )
		{
			eraseCouponList_[iter->item->GetID()] = iter->item;
		}
	}

	couponItems_.clear();
}

void AgcmUICouponBox::DestroyCouponItemAll()
{
	for( EraseCouponList::iterator iter = eraseCouponList_.begin(); iter != eraseCouponList_.end(); ++iter )
	{
		if( iter->second )
		{
			//m_pcsAgcmItem->ReleaseGridItemAttachedTexture(iter->item->m_pcsItemTemplate);
			m_pcsAgpmItem->DestroyItem(iter->second);
		}
	}

	eraseCouponList_.clear();
}

void AgcmUICouponBox::Open()
{
	if (m_bCashInvenOpenedUI == FALSE)
	{
		m_bCashInvenOpenedUI = TRUE;

		m_pcsAgcmUIManager2->ThrowEvent(m_lEventOpenCashInvenUI);

		m_pcsAgcmCharacter->SendRequestCouponInfo( m_pcsAgcmCharacter->GetSelfCID()  );
	}
}

void AgcmUICouponBox::Close()
{
	if (m_bCashInvenOpenedUI)
	{
		EraseCouponItemAll();

		for (int i = 0; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
		{
			m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[i]);
		}

		m_lCashItemCount = 0;

		OnUpdateCashItemCount();

		RefreshItemGrid();

		m_bCashInvenOpenedUI = FALSE;
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventCloseCashInvenUI);
	}
}

/*
	2005.11.17. By SungHoon
	�ڱ��÷��̾� ������ �ɰ�� �Ҹ���.
*/
BOOL AgcmUICouponBox::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass) return FALSE;
	AgcmUICouponBox *pThis = (AgcmUICouponBox *)(pClass);
	AgpdCharacter *pCharacter	= (AgpdCharacter *)(pData);

	AgpdItemADChar *pItemADChar = pThis->m_pcsAgpmItem->GetADCharacter( pCharacter );

	if( pItemADChar != NULL )
	{
//		pThis->m_pcsGridCashItemList = &pItemADChar->m_csCashInventoryGrid;
//		pThis->m_pcsUDCashItemList->m_stUserData.m_pvData = pThis->m_pcsGridCashItemList;

		pThis->RefreshItemGrid();
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
BOOL AgcmUICouponBox::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICouponBox *pThis = (AgcmUICouponBox *)(pClass);

	//AgpdCharacter* pcsSelfCharacter = pData ? static_cast<AgpdCharacter*>(pData) : NULL;
	AgcdCharacter* pstAgcdCharacter = pCustData ? static_cast<AgcdCharacter*>(pCustData) : NULL;

	// ���� ������ �Ҹ� �Ÿ� �������� �ʴ´�. 2006.03.30. steeple
	if(pstAgcdCharacter && pstAgcdCharacter->m_bTransforming)
	{
		return TRUE;
	}

	for (int i = 0; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_acsGridCashItemList[i]);

	pThis->EraseCouponItemAll();

	pThis->m_lCashItemCount	= 0;

	return TRUE;
}

BOOL AgcmUICouponBox::CBKeydownESC(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pClass) return FALSE;
	AgcmUICouponBox *pThis = (AgcmUICouponBox *)(pClass);

	pThis->m_bCashInvenOpenedUI	= FALSE;

	return TRUE;
}

BOOL AgcmUICouponBox::CBGetItemTimeInfo(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICouponBox* pThis = static_cast<AgcmUICouponBox*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);
	CHAR* pszText = static_cast<CHAR*>(pCustData);

	//pThis->SetItemInfoTime(pcsItem, pszText, AgcmUICouponBox_CALLTYPE_TOOLTIP);

	return TRUE;
}

BOOL AgcmUICouponBox::CBGetItemStaminaTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass || !pCustData)
		return FALSE;

	AgcmUICouponBox* pThis = static_cast<AgcmUICouponBox*>(pClass);
	AgpdItem* pcsItem = static_cast<AgpdItem*>(pData);
	CHAR* pszText = static_cast<CHAR*>(pCustData);

	//pThis->GetItemStaminaTime(pcsItem, pszText);

	return TRUE;
}

BOOL AgcmUICouponBox::RemoveCoupon( size_t idx )
{
	m_lCashItemCount = 0;

 	for (int i = 0; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
 	{
 		AgpdGridItem *pcsGridItem = m_pcsAgpmGrid->GetItem(&m_acsGridCashItemList[i], 0, 0, 0);
 		
 		if (!pcsGridItem) continue;

 		if( i == idx ) continue;
 
 		m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[m_lCashItemCount]);
 
 		m_pcsAgpmGrid->Add(&m_acsGridCashItemList[m_lCashItemCount], 0, 0, 0,  pcsGridItem, 1, 1);
 		m_lCashItemCount++;
 	}
	for (int i = m_lCashItemCount ; i < AGCMUI_COUPON_BOX_MAX_COUNT; ++i)
	{
		m_pcsAgpmGrid->Reset(&m_acsGridCashItemList[i]);
	}

	EraseCouponItem( idx );

	OnUpdateCashItemCount();

	return TRUE;
}

BOOL AgcmUICouponBox::IsSelfOperation(AgpdItem* pItem, AgpdCharacter* pCharacter)
{
	if(!pItem || !pCharacter)
		return FALSE;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return TRUE;		// ���� ����ĳ���Ͱ� ���õǾ����� ���� ���¶�� TRUE �� ����������.

	return pcsSelfCharacter->m_lID == pCharacter->m_lID ? TRUE : FALSE;
}

BOOL AgcmUICouponBox::CBDisplayCashInvenStaminaMax(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;

	if( szDisplay )
		szDisplay[0] = 0;

	return TRUE;
}

BOOL AgcmUICouponBox::CBDisplayCashInvenStaminaCurrent(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl) return FALSE;
	AgcmUICouponBox *pThis = ( AgcmUICouponBox *)pClass;

	if(pcsSourceControl->m_lUserDataIndex < 0 || pcsSourceControl->m_lUserDataIndex >= pThis->m_lCashItemCount)
		return FALSE;

	if( szDisplay )
		szDisplay[0] = 0;

	return TRUE;
}

// Check pet state and show grid icon
// If you can find out at least one, show grid icon.
BOOL AgcmUICouponBox::CheckPetStateGrid()
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

				m_pcsPetState->SetTooltip( "Hungry Pet State" );
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

//-----------------------------------------------------------------------
//

CouponDisplayInfo::CouponDisplayInfos CouponDisplayInfo::_displayInfos;

void CouponDisplayInfo::load() 
{
	if( _displayInfos.empty() )
	{
		AuXmlDocument document;
		document.LoadFile( "ini\\CouponInfo.xml" );

		AuXmlNode * root = document.FirstChild( "CouponInfo" );

		if( root )
		{
			for( AuXmlNode * couponInfo = root->FirstChild( "CouponType" ); couponInfo; couponInfo = root->NextSibling() )
			{
				CouponDisplayInfo displayInfo;

				couponInfo->ToElement()->Attribute("TypeID", &displayInfo.typeID);
				couponInfo->ToElement()->Attribute("IconItemID", &displayInfo.iconItemID);

				//char const * toolTipID = couponInfo->ToElement()->Attribute("ToolTipMessageID");
				//if( toolTipID )
				//	displayInfo.toolTipID = toolTipID;

				_displayInfos[ displayInfo.typeID ] = displayInfo;
			}
		}
	}
}

CouponDisplayInfo::CouponDisplayInfo()
	: typeID(0)
	, iconItemID( 6355 )
	, toolTipID( "CouponTooltip_1" )
{}

CouponDisplayInfo const & CouponDisplayInfo::get( int couponType )
{
	load();
	CouponDisplayInfo & info = _displayInfos[couponType];
	info.typeID = couponType;
	return info;
}

//-----------------------------------------------------------------------

#endif