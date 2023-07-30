// AgcmUIMain.cpp: implementation of the AgcmUIMain class.
//
//////////////////////////////////////////////////////////////////////
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgpmUIStatus/AgpmUIStatus.h>
#include <AuStrTable/AuStrTable.h>
#include <AuLocalize/AuRegionNameConvert.h>
#include <AuTimeStamp/AuTimeStamp.h>
#include <AgcmUISystemMessage/AgcmUISystemMessage.h>
#include <AgcmUIItem/AgcmUIItem.h>
#include <AgcmUISkill2/AgcmUISkill2.h>
#include <AgpmEventNPCDialog/AgpmEventNPCDialog.h>
#include <AgcmEventNPCDialog/AgcmEventNPCDialog.h>
#include <AgcmLogin/AgcmLogin.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgpmItem/AgpmItem.h>
#include <AgcmSkill/AgcmSkill.h>
#include <AgpmGrid/AgpmGrid.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgcmItem/AgcmItem.h>
#include <AgpmSkill/AgpmSkill.h>
#include <AgcmUIManager2/AgcUIWindow.h>
#include <AgcmEff2/AgcdEffPublicStructs.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUICharacter/AgcmUICharacter.h>
#include <AgcmUIOption/AgcdUIOption.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

const UINT32 AGCMUIMAIN_MEDIARATE_INTERVAL_HIDE = 50 * 60; // second
const UINT32 AGCMUIMAIN_MEDIARATE_INTERVAL_SHOW = 5; // second
const UINT32 AGCMUIMAIN_IDLE_DELAY = 100;

#define AGCMUI_MAIN_ADDICT_STATUS_LEVEL0_COLOR			"16777215"
#define AGCMUI_MAIN_ADDICT_STATUS_LEVEL1_COLOR			"15720892"
#define AGCMUI_MAIN_ADDICT_STATUS_LEVEL2_COLOR			"15640898"
#define AGCMUI_MAIN_ADDICT_STATUS_LEVEL3_COLOR			"13136840"

/* -------------------------------------------------------- */

template <class T, std::size_t N>
size_t countof(const T(&array)[N])
{
	return N;
}

static size_t
get_quickbelt_index()
{
	if (AgcEngine::IsShiftDown() || AgcEngine::IsRShiftDown())
		return 1;
	if (AgcEngine::IsCtrlDown() || AgcEngine::IsRCtrlDown())
		return 2;
	if (AgcEngine::IsAltDown() || AgcEngine::IsRAltDown())
		return 3;
	return 0;
}

/* -------------------------------------------------------- */

AgcmUIMain::AgcmUIMain()
{
	SetModuleName("AgcmUIMain");
	SetDelayTick(AGCMUIMAIN_IDLE_DELAY);

	memset(m_quickBeltUD, 0, sizeof(m_quickBeltUD));
//	m_apstVQuickBelt[0]	= NULL;
//	m_apstVQuickBelt[1]	= NULL;

	m_pstHPPotionGrid	= NULL;
	m_pstMPPotionGrid	= NULL;

	m_pstSystemMessageGrid	= NULL;

	m_quickBeltCurrentLayer = 0;

	m_pcsAlarmGrid		= NULL;

	EnableIdle(TRUE);

	m_ulPrevProcessClockCount	=	0	;
	m_lEventMainQuit			=	0	;

	m_bIsVerticalQuickBelt		= FALSE;

	m_ulRClickDownTMSec			= 0;

	m_lDragCursorID				= (-1);
	m_lPrevCursor				= 0;

	m_pcsHPPotionItem			= NULL;
	m_pcsMPPotionItem			= NULL;

	m_pcsArrowItem				= NULL;
	m_pcsBoltItem				= NULL;

	m_lTotalHPPotionCount		= 0;
	m_lTotalMPPotionCount		= 0;
	m_lTotalArrowCount			= 0;
	m_lTotalBoltCount			= 0;

	m_pcsApmMap					= NULL;
	m_szGameServerName[0]		= 0;

	m_lHPPointScrollBarValue	= 0;
	m_lMPPointScrollBarValue	= 0;

	m_pcsHPPointScrollBarControl	= NULL;
	m_pcsMPPointScrollBarControl	= NULL;
	
	m_bMediaRateWindow_Show			= FALSE;
	m_ulMediaRateWindow_RemainSec	= 0;
	m_ulMediaRateWindow_LastTick	= 0;
	m_bIsNC17						= FALSE;
	
	m_bEnableExceptJapan			= TRUE;

	m_pcsOnlineTimeEdit				= NULL;
	m_pcsAddictStatusEdit			= NULL;
	m_pcsOnlineTimeAddictStatus		= NULL;
	m_ulLastUpdateOnlineTime		= 0;
	m_ulStartOnlineTime				= 0;
	m_initialHpMpBarY				= MAXINT16;
}

AgcmUIMain::~AgcmUIMain()
{

}

BOOL			AgcmUIMain::OnAddModule()
{
	m_pcsAgcmUIManager2		= ( AgcmUIManager2	*) GetModule("AgcmUIManager2");
	m_pcsAgpmGrid			= ( AgpmGrid		*) GetModule("AgpmGrid"		);
	m_pcsAgpmFactors		= ( AgpmFactors		*) GetModule("AgpmFactors"	);
	m_pcsAgpmCharacter		= ( AgpmCharacter	*) GetModule("AgpmCharacter");
	m_pcsAgpmItem			= ( AgpmItem		*) GetModule("AgpmItem"		);
	m_pcsAgcmCharacter		= ( AgcmCharacter	*) GetModule("AgcmCharacter");
	m_pcsAgcmSkill			= ( AgcmSkill		*) GetModule("AgcmSkill"	);
	m_pcsAgcmItem			= ( AgcmItem		*) GetModule("AgcmItem"		);
	m_pcsAgpmSkill			= ( AgpmSkill		*) GetModule("AgpmSkill"	);
	m_pcsApmMap				= ( ApmMap			*) GetModule("ApmMap"		);
	m_pcsAgcmEff2			= ( AgcmEff2		*) GetModule("AgcmEff2"		);
	m_pcsAgcmLogin			= ( AgcmLogin		*) GetModule("AgcmLogin"	);

	if (!m_pcsAgcmUIManager2	||
		!m_pcsAgpmGrid			||
		!m_pcsAgpmFactors		||
		!m_pcsAgpmCharacter		||
		!m_pcsAgpmItem			||
		!m_pcsAgcmCharacter		||
		!m_pcsAgcmSkill			||
		!m_pcsAgcmItem			||
		!m_pcsAgpmSkill			||
		!m_pcsApmMap			||
		!m_pcsAgcmEff2			||
		!m_pcsAgcmLogin )
		return FALSE;

//	if (!m_pcsAgpmItem->SetCallbackInit(CBInitItem, this))
//		return FALSE;
//	if (!m_pcsAgpmItem->SetCallbackRemove(CBRemoveItem, this))
//		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackInventory(CBAddItemToInventory, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackAddItemCashInventory(CBAddItemToInventory, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemoveFromInventory(CBRemoveItemFromInventory, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackRemoveItemCashInventory(CBRemoveItemFromInventory, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUpdateStackCount(CBUpdateStackCount, this))
		return FALSE;

	if (!m_pcsAgpmItem->SetCallbackEquip(CBEquipItem, this))
		return FALSE;
	if (!m_pcsAgpmItem->SetCallbackUnEquip(CBUnEquipItem, this))
		return FALSE;

	if (!m_pcsAgcmCharacter->SetCallbackSetSelfCharacter(CBSetSelfCharacter, this))
		return FALSE;
	if (!m_pcsAgcmCharacter->SetCallbackReleaseSelfCharacter(CBReleaseSelfCharacter, this))
		return FALSE;

	if (!m_pcsAgcmSkill->SetCallbackGetRegisteredMPPotionTID(CBGetRegisteredMPPotionTID, this))
		return FALSE;

	if( !m_pcsAgpmCharacter->SetCallbackBindingRegionChange( CBChangeCharacterBindingRegion , this ) )
		return FALSE;

	if (!m_pcsAgcmLogin->SetCallbackSignOnSuccess(CBIDPasswordOK, this))
		return FALSE;

	if (!m_pcsAgpmCharacter->SetCallbackOnlineTime(CBUpdateOnlineTime, this))
		return FALSE;

	if (m_pcsAgpmGrid)
	{
		for (size_t i = 0; i < countof(m_quickBeltGrid); i++) {
			m_quickBeltGrid[i].m_lGridType = AGPDGRID_ITEM_TYPE_ITEM | AGPDGRID_ITEM_TYPE_SKILL | AGPDGRID_ITEM_TYPE_SOCIALACTION;
			if (i >= 2)
				m_pcsAgpmGrid->Init(&m_quickBeltGrid[i], 1, 1, AGPMUISTATUS_HALF_QUICKBELT_COLUMN);
			else
				m_pcsAgpmGrid->Init(&m_quickBeltGrid[i], AGCMUIMAIN_MAX_QUICKBELT, 1, AGPMUISTATUS_HALF_QUICKBELT_COLUMN);
		}
//		m_astVQuickBeltGrid[0].m_lGridType = AGPDGRID_ITEM_TYPE_ITEM | AGPDGRID_ITEM_TYPE_SKILL;
//		m_astVQuickBeltGrid[1].m_lGridType = AGPDGRID_ITEM_TYPE_ITEM | AGPDGRID_ITEM_TYPE_SKILL;
//		m_stSkillGrid.m_lGridType = AGPDGRID_ITEM_TYPE_SKILL;

		m_stHPPotionGrid.m_lGridType		= AGPDGRID_ITEM_TYPE_ITEM;
		m_stMPPotionGrid.m_lGridType		= AGPDGRID_ITEM_TYPE_ITEM;

		m_stSystemMessageGrid.m_lGridType	= AGPDGRID_ITEM_TYPE_ITEM;

//		m_pcsAgpmGrid->Init(&m_astVQuickBeltGrid[0], AGCMUIMAIN_MAX_QUICKBELT, 4, 1);
//		m_pcsAgpmGrid->Init(&m_astVQuickBeltGrid[1], AGCMUIMAIN_MAX_QUICKBELT, 4, 1);
//		m_pcsAgpmGrid->Init(&m_stSkillGrid, 1, 1, 1);

#ifndef _AREA_CHINA_
		m_pcsAgpmGrid->Init(&m_csAlarmGrid, 1, 11, 9);
#else
		m_pcsAgpmGrid->Init(&m_csAlarmGrid, 1, 11, 3);
#endif

		m_pcsAgpmGrid->Init(&m_stHPPotionGrid, 1, 1, 1);
		m_pcsAgpmGrid->Init(&m_stMPPotionGrid, 1, 1, 1);

		m_pcsAgpmGrid->Init(&m_stSystemMessageGrid, 1, 1, AGCMUI_MAIN_MAX_SYSTEM_GRID_COLUMN);
	}

	// Quick Belt(User Data)를 등록한다.
	{
		for (size_t i = 0; i < countof(m_quickBeltUD) / 2; i++) {
			char name[128];

			sprintf(name, "Quick_Belt_%u(L)", (UINT32)i);
			m_quickBeltUD[i * 2]	= m_pcsAgcmUIManager2->AddUserData(
				name, &m_quickBeltGrid[i * 2], 
				sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
			if (!m_quickBeltUD[i * 2])
				return FALSE;
			sprintf(name, "Quick_Belt_%u(R)", (UINT32)i);
			m_quickBeltUD[i * 2 + 1]	= m_pcsAgcmUIManager2->AddUserData(
				name, &m_quickBeltGrid[i * 2 + 1], 
				sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
			if (!m_quickBeltUD[i * 2 + 1])
				return FALSE;
		}

//		m_apstVQuickBelt[0]	= m_pcsAgcmUIManager2->AddUserData("Quick Belt V(L)", &m_astVQuickBeltGrid[0], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
//		if (!m_apstVQuickBelt[0])
//			return FALSE;
//
//		m_apstVQuickBelt[1]	= m_pcsAgcmUIManager2->AddUserData("Quick Belt V(R)", &m_astVQuickBeltGrid[1], sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
//		if (!m_apstVQuickBelt[1])
//			return FALSE;
//
//		m_pstSkill = m_pcsAgcmUIManager2->AddUserData("Quick Belt Skill", &m_stSkillGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
//		if (!m_pstSkill)
//			return FALSE;

		m_apstQBLayer[0]	= m_pcsAgcmUIManager2->AddUserData("QB_Layer(L)", &m_quickBeltCurrentLayer, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
		if (!m_apstQBLayer[0])
			return FALSE;

		m_apstQBLayer[1]	= m_pcsAgcmUIManager2->AddUserData("QB_Layer(R)", &m_quickBeltCurrentLayer, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_INT32);
		if (!m_apstQBLayer[1])
			return FALSE;

		m_pcsAlarmGrid		= m_pcsAgcmUIManager2->AddUserData("Alarm_Grid", &m_csAlarmGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pcsAlarmGrid)
			return FALSE;

		m_pstHPPotionGrid	= m_pcsAgcmUIManager2->AddUserData("HPPotion_Grid", &m_stHPPotionGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pstHPPotionGrid)
			return FALSE;
		m_pstMPPotionGrid	= m_pcsAgcmUIManager2->AddUserData("MPPotion_Grid", &m_stMPPotionGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pstMPPotionGrid)
			return FALSE;

		m_pstSystemMessageGrid	= m_pcsAgcmUIManager2->AddUserData("SystemMessage_Grid", &m_stSystemMessageGrid, sizeof(AgpdGrid), 1, AGCDUI_USERDATA_TYPE_GRID);
		if (!m_pstSystemMessageGrid)
			return FALSE;
			
		m_pcsEnableExceptJapan	= m_pcsAgcmUIManager2->AddUserData("EnableExceptJapan", &m_bEnableExceptJapan, sizeof(INT32), 1, AGCDUI_USERDATA_TYPE_BOOL);
		if (!m_pcsEnableExceptJapan)
			return FALSE;
	}

	// Function을 등록한다.
	{
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Add", CBAddShortcut, 1, "Self Grid Control"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Remove", CBRemoveShortcut, 1, "Self Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "HPPotion Add", CBAddHPPotion, 1, "Self Grid Control"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "MPPotion Add", CBAddMPPotion, 1, "Self Grid Control"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Potion Remove", CBRemovePotion, 0))
			return FALSE;


		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_1, CBQuickBeltUse_1, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_2, CBQuickBeltUse_2, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_3, CBQuickBeltUse_3, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_4, CBQuickBeltUse_4, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_5, CBQuickBeltUse_5, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_6, CBQuickBeltUse_6, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_7, CBQuickBeltUse_7, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_8, CBQuickBeltUse_8, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_9, CBQuickBeltUse_9, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_10, CBQuickBeltUse_10, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_HPPOTION, CBQuickBeltUseHPPotion, 1, "QuickBelt Grid Control"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_USE_MPPOTION, CBQuickBeltUseMPPotion, 1, "QuickBelt Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_LAYER_UP, CBQuickBeltLayerUp, 2, "QBelt(L) Grid", "QBelt(R) Grid"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, AGCMUI_MAIN_FUNCTION_QUICK_BELT_LAYER_DOWN, CBQuickBeltLayerDown, 2, "QBelt(L) Grid", "QBelt(R) Grid"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Close UI Window", CBCloseUIWindow, 0))
			return FALSE;

		// Control 별 Action Function
		if (!m_pcsAgcmUIManager2->AddFunction(this, "Control Edit Init", CBEditControlInit, 1, "Edit Contol"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Control Edit Active", CBEditControlActive, 1, "Edit Active"))
			return FALSE;	
		
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Left Click Down", CBLClickDown, 0))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Right Click Down", CBRClickDown, 0))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB_Right_Click_Up", CBRClickUp, 1, "QBSkill Grid Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB PotionGrid RightClickUp", CBPotionGridRClickUp, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Click System Grid NoAction", CBClickSystemGridNoAction, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Click System Grid", CBClickSystemGrid, 0))
			return FALSE;

#ifndef _AREA_CHINA_
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Open System Grid Tooltip", CBOpenSystemGridToolTip, 0))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QB Close System Grid Tooltip", CBCloseSystemGridToolTip, 0))
			return FALSE;
#endif

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Control Button Disable", CBButtonControlDisable, 1, "Button Contol Disable"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Control Button Enable", CBButtonControlEnable, 1, "Button Contol Enable"))
			return FALSE;

		// tool tip
		if (!m_pcsAgcmUIManager2->AddFunction(this, "Open QuickBelt ToolTip", CBOpenToolTip, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Close QuickBelt ToolTip", CBCloseToolTip, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "Close All Opened UI ToolTip", CBCloseAllUIToolTip, 0))
			return FALSE;

		// Event Call Funciton
		if (!m_pcsAgcmUIManager2->AddFunction(this, "Quit Event Call", CBApplicationQuitEventCall, 0))
			return FALSE;

		// QB Position Adjustment Function
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QuickBelt Position Adjust", CBQuickBeltAdjustPos, 0))
			return FALSE;

		// QBV Position Adjustment Function 
		if (!m_pcsAgcmUIManager2->AddFunction(this, "QuickBeltV Position Adfjust", CBQuickBeltVAdjustPos, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "SetFocusMovableItem", CBSetFocusMovableItem, 0))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "KillFocusMovableItem", CBKillFocusMovableItem, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "ChangeHPPointScrollBarValue", CBChangeHPPointScollBarValue, 0))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, "ChangeMPPointScrollBarValue", CBChangeMPPointScollBarValue, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "GetPotionScrollBarControl", CBGetPotionScollBarControl, 2, "AutoUseHPGageControl", "AutoUseMPGageControl"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "MainUIAllClose_KillFocus", CBMainUIAllClose, 0))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "GetOnlineTimeEditControl", CBGetOnlineTimeEditControl, 1, "OnlineTime Edit Control"))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddFunction(this, "GetAddictStatusEditControl", CBGetAddictStatusEditControl, 1, "AddictStatus Edit Control"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this,
			"ShowAdditionalQuickBelt", CBShowAdditionalQuickBelt,
			1, "QB Grid Base (n)"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, 
			"HideAdditionalQuickBelt", CBHideAdditionalQuickBelt, 
			1, "QB Grid Base (n)"))
			return FALSE;
		if (!m_pcsAgcmUIManager2->AddFunction(this, 
			"UpdateHpMpBarPosition", CBUpdateHpMpBarPosition, 
			1, "QB Hp/Mp Bar"))
			return FALSE;
	}

	// Display를 등록한다.
	{
		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Exp 현단계", AGCM_UI_MAIN_EXP_STEP_REMAIN, CBDisplayExp, AGCDUI_USERDATA_TYPE_CHARACTER))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Exp 현단계 백분율", AGCM_UI_MAIN_EXP_STEP_PERCENTAGE, CBDisplayExp, AGCDUI_USERDATA_TYPE_CHARACTER))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Exp 현단계 포인트", AGCM_UI_MAIN_EXP_STEP_CURRENT, CBDisplayExp, AGCDUI_USERDATA_TYPE_CHARACTER))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Exp 현단계 최대치", AGCM_UI_MAIN_EXP_STEP_MAX, CBDisplayExp, AGCDUI_USERDATA_TYPE_CHARACTER))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "_Default", AGCM_UI_MAIN_DISPLAY_INT32, CBDisplayDefault, AGCDUI_USERDATA_TYPE_ALL))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "Main_INT32", AGCM_UI_MAIN_DISPLAY_INT32, CBDisplayInt32, AGCDUI_USERDATA_TYPE_INT32))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddDisplay(this, "QBelt Layer", AGCM_UI_MAIN_DISPLAY_QBELT_LAYER, CBDisplayCurrentLayer, AGCDUI_USERDATA_TYPE_INT32))
			return FALSE;
	}

	// Event를 등록한다.
	{
		/*
		m_lEventID = m_pcsAgcmUIManager2->AddEvent("Test_Event");
		if (m_lEventID < 0)
			return FALSE;
		*/

		m_lEventIDAddInvalidItem	= m_pcsAgcmUIManager2->AddEvent("QB_Add_Invalid_Item");
		if (m_lEventIDAddInvalidItem < 0)
			return FALSE;

		m_lEventMainQuit			= m_pcsAgcmUIManager2->AddEvent("MainUI_Quit_Question", CBUIReturnAppQuit, this )	;
		if ( m_lEventMainQuit < 0 )
			return FALSE;

		m_lEventIDRemoveItem		= m_pcsAgcmUIManager2->AddEvent("QB_Remove_Item_Sound");
		if (m_lEventIDRemoveItem < 0)
			return FALSE;

		m_lEventMainUIAllClose		= m_pcsAgcmUIManager2->AddEvent("MainUI_AllClose");
		if (m_lEventMainUIAllClose < 0)
			return FALSE;

		m_lEventGetPotionScrollBarControl	= m_pcsAgcmUIManager2->AddEvent("MainUI_GetPotionScrollBarControl");
		if (m_lEventGetPotionScrollBarControl < 0)
			return FALSE;

		m_lEventOpenMediaRate = m_pcsAgcmUIManager2->AddEvent("MainUI_OpenMediaRate");
		if (m_lEventOpenMediaRate < 0)
			return FALSE;
			
		m_lEventOpenMediaRate18 = m_pcsAgcmUIManager2->AddEvent("MainUI_OpenMediaRate18");
		if (m_lEventOpenMediaRate18 < 0)
			return FALSE;
			
		m_lEventCloseMediaRate = m_pcsAgcmUIManager2->AddEvent("MainUI_CloseMediaRate");
		if (m_lEventCloseMediaRate < 0)
			return FALSE;

		m_lEventGetOnlineTimeEdit = m_pcsAgcmUIManager2->AddEvent("MainUI_GetOnlineTimeEdit");
		if (m_lEventGetOnlineTimeEdit < 0)
			return FALSE;

		m_lEventGetAddictStatusEdit = m_pcsAgcmUIManager2->AddEvent("MainUI_GetAddictStatusEdit");
		if (m_lEventGetAddictStatusEdit < 0)
			return FALSE;
		m_eventShowAdditionalQuickBelt1 = m_pcsAgcmUIManager2->AddEvent("Event_ShowAdditionalQuickBelt1");
		if (m_eventShowAdditionalQuickBelt1 < 0)
			return FALSE;
		m_eventHideAdditionalQuickBelt1 = m_pcsAgcmUIManager2->AddEvent("Event_HideAdditionalQuickBelt1");
		if (m_eventHideAdditionalQuickBelt1 < 0)
			return FALSE;
		m_eventShowAdditionalQuickBelt2 = m_pcsAgcmUIManager2->AddEvent("Event_ShowAdditionalQuickBelt2");
		if (m_eventShowAdditionalQuickBelt2 < 0)
			return FALSE;
		m_eventHideAdditionalQuickBelt2 = m_pcsAgcmUIManager2->AddEvent("Event_HideAdditionalQuickBelt2");
		if (m_eventHideAdditionalQuickBelt2 < 0)
			return FALSE;
		m_eventShowAdditionalQuickBelt3 = m_pcsAgcmUIManager2->AddEvent("Event_ShowAdditionalQuickBelt3");
		if (m_eventShowAdditionalQuickBelt3 < 0)
			return FALSE;
		m_eventHideAdditionalQuickBelt3 = m_pcsAgcmUIManager2->AddEvent("Event_HideAdditionalQuickBelt3");
		if (m_eventHideAdditionalQuickBelt3 < 0)
			return FALSE;
		m_eventUpdateHpMpBarPosition = m_pcsAgcmUIManager2->AddEvent(
			"Event_UpdateHpMpBarPosition");
		if (m_eventUpdateHpMpBarPosition < 0)
			return FALSE;
	}

	// Boolean을 등록한다.
	{
		if (!m_pcsAgcmUIManager2->AddBoolean(this, "_Default", CBDefaultBoolean, AGCDUI_USERDATA_TYPE_BOOL))
			return FALSE; 

		if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsCharacterDead", CBSelfCharacterDead, AGCDUI_USERDATA_TYPE_NONE))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsSelected", CBIsSelected, AGCDUI_USERDATA_TYPE_NONE))
			return FALSE;

		if (!m_pcsAgcmUIManager2->AddBoolean(this, "IsAreaKorea", CBBooleanIsKorea, AGCDUI_USERDATA_TYPE_NONE))
			return FALSE;
	}

	return TRUE;
}

BOOL AgcmUIMain::OnDestroy()
{
	if (m_pcsAgpmGrid) {
		for (size_t i = 0; i < countof(m_quickBeltGrid); i++)
			m_pcsAgpmGrid->Remove(&m_quickBeltGrid[i]);
//		m_pcsAgpmGrid->Remove(&m_astVQuickBeltGrid[0]);
//		m_pcsAgpmGrid->Remove(&m_astVQuickBeltGrid[1]);
//		m_pcsAgpmGrid->Remove(&m_stSkillGrid);
		m_pcsAgpmGrid->Remove(&m_csAlarmGrid);

		m_pcsAgpmGrid->Remove(&m_stHPPotionGrid);
		m_pcsAgpmGrid->Remove(&m_stMPPotionGrid);

		m_pcsAgpmGrid->Remove(&m_stSystemMessageGrid);

		if (m_pcsHPPotionItem)
		{
			AgpdItemTemplate	*pcsAgpdItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsHPPotionItem->m_lItemTID);
			if (pcsAgpdItemTemplate)
			{
				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);

				if (pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_pSmallTexture)
				{
					INT32	lRefCount = pcsAgcdItemTemplate->m_pSmallTexture->refCount;

					RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
					pcsAgcdItemTemplate->m_pSmallTexture = NULL;

					if (lRefCount == 1)
						pcsAgcdItemTemplate->m_pSmallTexture	= NULL;
				}
			}

			m_pcsAgpmGrid->DeleteGridItem(m_pcsHPPotionItem);
			m_pcsHPPotionItem	= NULL;
		}
		if (m_pcsMPPotionItem)
		{
			AgpdItemTemplate	*pcsAgpdItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsMPPotionItem->m_lItemTID);
			if (pcsAgpdItemTemplate)
			{
				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);

				if (pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_pSmallTexture)
				{
					INT32	lRefCount = pcsAgcdItemTemplate->m_pSmallTexture->refCount;

					RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
					pcsAgcdItemTemplate->m_pSmallTexture = NULL;

					if (lRefCount == 1)
						pcsAgcdItemTemplate->m_pSmallTexture	= NULL;
				}
			}

			m_pcsAgpmGrid->DeleteGridItem(m_pcsMPPotionItem);
			m_pcsMPPotionItem	= NULL;
		}
		if (m_pcsArrowItem)
		{
			AgpdItemTemplate	*pcsAgpdItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsArrowItem->m_lItemTID);
			if (pcsAgpdItemTemplate)
			{
				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);

				if (pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_pSmallTexture)
				{
					INT32	lRefCount = pcsAgcdItemTemplate->m_pSmallTexture->refCount;

					RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
					pcsAgcdItemTemplate->m_pSmallTexture = NULL;

					if (lRefCount == 1)
						pcsAgcdItemTemplate->m_pSmallTexture	= NULL;
				}
			}

			m_pcsAgpmGrid->DeleteGridItem(m_pcsArrowItem);
			m_pcsArrowItem		= NULL;
		}
		if (m_pcsBoltItem)
		{
			AgpdItemTemplate	*pcsAgpdItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsBoltItem->m_lItemTID);
			if (pcsAgpdItemTemplate)
			{
				AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsAgpdItemTemplate);

				if (pcsAgcdItemTemplate && pcsAgcdItemTemplate->m_pSmallTexture)
				{
					INT32	lRefCount = pcsAgcdItemTemplate->m_pSmallTexture->refCount;

					RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
					pcsAgcdItemTemplate->m_pSmallTexture = NULL;

					if (lRefCount == 1)
						pcsAgcdItemTemplate->m_pSmallTexture	= NULL;
				}
			}

			m_pcsAgpmGrid->DeleteGridItem(m_pcsBoltItem);
			m_pcsBoltItem		= NULL;
		}
	}

	return TRUE;
}

BOOL			AgcmUIMain::OnInit()
{
#ifndef _AREA_GLOBAL_
	m_SystemGridTooltip.Initialize( TRUE );
#endif
	return TRUE;
}

BOOL AgcmUIMain::OnInitQuickBeltGrid( void )
{
	//m_quickBeltCurrentLayer = 0;

	//for (size_t i = 0; i < countof(m_quickBeltUD); ++i) {
	//	if (m_quickBeltUD[i]->m_ppControls) {
	//		for (INT32 lIndex = 0; lIndex < m_quickBeltUD[i]->m_lControl; ++lIndex) {
	//			AgcdUIControl	*pcsUserDataControl	= m_quickBeltUD[i]->m_ppControls[lIndex];
	//			if (pcsUserDataControl) {
	//				AcUIGrid * pcsUIGrid;
	//				
	//				if (!pcsUserDataControl || (pcsUserDataControl->m_lType != AcUIBase::TYPE_GRID && pcsUserDataControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
	//					continue;
	//				pcsUIGrid = (AcUIGrid *)pcsUserDataControl->m_pcsBase;
	//				if (pcsUIGrid) {
	//					pcsUIGrid->SetNowLayer(m_quickBeltCurrentLayer);
	//					pcsUIGrid->UpdateUIGrid();
	//				}
	//			}
	//		}
	//	}
	//}
	for (size_t i = 0; i < countof(m_apstQBLayer); i++)
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_apstQBLayer[i]);
	return TRUE;
}

BOOL
AgcmUIMain::OnIdle(UINT32 ulClockCount)
{
	PROFILE("AgcmUIMain::OnIdle");

	UINT32 ulNextProcessClockCount;
	AgpdCharacter * pcsSelfCharacter;
	bool bCanSelfCharacterRecover;
	
	if(!CheckDelay(ulClockCount))
		return TRUE;
	ulNextProcessClockCount = m_ulPrevProcessClockCount + 
		AGCMUI_MAIN_PROCESS_IDLE_INTERVAL_MSEC;
	if (ulNextProcessClockCount < ulClockCount) {
		UpdateAlarmGrid();
		/*
		 * 메인 UI에서 오른쪽 상단에 표시되는 이미지(15세마크 같은것들..)
		 * 국내만 적용되는 사항이므로 제거
		 */
#ifdef _AREA_KOREA_ //
		UpdateMediaRateWindow(ulClockCount);
#endif
		m_ulPrevProcessClockCount = ulClockCount;
	}
	SetOnlineTimeAddictStatus(ulClockCount);
	/* 
	 * Can't use potions if character is dead.
	 */
	pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	bCanSelfCharacterRecover = (pcsSelfCharacter != NULL && 
		pcsSelfCharacter->m_unActionStatus != AGPDCHAR_STATUS_DEAD);
	if (bCanSelfCharacterRecover && m_lHPPointScrollBarValue > 0 &&
		m_pcsHPPotionItem && m_lTotalHPPotionCount > 0) {
		INT32 lCurrentHP = 0;
		INT32 lMaxHP = 0;
		INT32 lPercent;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, 
			&lCurrentHP, AGPD_FACTORS_TYPE_RESULT, 
			AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lMaxHP, 
			AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, 
			AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		lPercent = (INT32)((lCurrentHP / (FLOAT)lMaxHP) * 100.f);
		if (lPercent < m_lHPPointScrollBarValue) {
			AgpdItemTemplate * pcsItemTemplate;
			AgpdItem * pcsHPPotion;
			UINT32 ulReuseClockCount;

			/*
			 * Retrieve item from inventory and use if it's not 
			 * in cooldown.
			 *
			 * If no item of this template ID is found, reset 
			 * item count display on the UI.
			 */
			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(
				m_pcsHPPotionItem->m_lItemTID);
			if (pcsItemTemplate && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
				pcsHPPotion	= m_pcsAgpmItem->GetCashInventoryItemByTID(
					m_pcsAgcmCharacter->GetSelfCharacter(), 
					m_pcsHPPotionItem->m_lItemTID);
			else
				pcsHPPotion	= m_pcsAgpmItem->GetInventoryItemByTID(
					m_pcsAgcmCharacter->GetSelfCharacter(), 
					m_pcsHPPotionItem->m_lItemTID);
			ulReuseClockCount = (pcsHPPotion != NULL) ? 
				(pcsHPPotion->m_pcsGridItem->m_ulUseItemTime + 
				pcsHPPotion->m_pcsGridItem->m_ulReuseIntervalTime) : 0;
			if (pcsHPPotion && ulReuseClockCount <= ulClockCount)
				m_pcsAgcmItem->UseItem(pcsHPPotion);
			if (!pcsHPPotion) {
				m_lTotalHPPotionCount = 0;
				m_pcsHPPotionItem->SetRightBottomString("0");
			}
		}
	}
	if (bCanSelfCharacterRecover && m_lMPPointScrollBarValue > 0 &&
		m_pcsMPPotionItem && m_lTotalMPPotionCount > 0) {
		INT32 lCurrentMP = 0;
		INT32 lMaxMP = 0;
		INT32 lPercent;

		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, 
			&lCurrentMP, AGPD_FACTORS_TYPE_RESULT, 
			AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
		m_pcsAgpmFactors->GetValue(&pcsSelfCharacter->m_csFactor, &lMaxMP, 
			AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, 
			AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
		lPercent = (INT32)((lCurrentMP / (FLOAT)lMaxMP) * 100.f);
		if (lPercent < m_lMPPointScrollBarValue) {
			AgpdItemTemplate * pcsItemTemplate;
			AgpdItem * pcsMPPotion;
			UINT32 ulReuseClockCount;

			/*
			 * Retrieve item from inventory and use if it's not 
			 * in cooldown.
			 *
			 * If no item of this template ID is found, reset 
			 * item count display on the UI.
			 */
			pcsItemTemplate = m_pcsAgpmItem->GetItemTemplate(
				m_pcsMPPotionItem->m_lItemTID);
			if (pcsItemTemplate && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
				pcsMPPotion	= m_pcsAgpmItem->GetCashInventoryItemByTID(
					m_pcsAgcmCharacter->GetSelfCharacter(), 
					m_pcsMPPotionItem->m_lItemTID);
			else
				pcsMPPotion	= m_pcsAgpmItem->GetInventoryItemByTID(
					m_pcsAgcmCharacter->GetSelfCharacter(), 
					m_pcsMPPotionItem->m_lItemTID);
			ulReuseClockCount = (pcsMPPotion != NULL) ? 
				(pcsMPPotion->m_pcsGridItem->m_ulUseItemTime + 
				pcsMPPotion->m_pcsGridItem->m_ulReuseIntervalTime) : 0;
			if (pcsMPPotion && ulReuseClockCount <= ulClockCount)
				m_pcsAgcmItem->UseItem(pcsMPPotion);
			if (!pcsMPPotion) {
				m_lTotalMPPotionCount = 0;
				m_pcsMPPotionItem->SetRightBottomString("0");
			}
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::AddShortcut(AgpdGrid *pcsGrid, AgpdGridItem *pcsGridItem, INT32 lLayer, INT32 lRow, INT32 lColumn, BOOL bIsThrowEvent)
{
	if (!pcsGrid || !pcsGridItem)
		return FALSE;
	if (pcsGridItem->m_eType == AGPDGRID_ITEM_TYPE_ITEM) {
		if (!CheckQuickBeltItem(pcsGridItem->m_lItemID)) {
			if (bIsThrowEvent)
				m_pcsAgcmUIManager2->ThrowEvent(m_lEventIDAddInvalidItem);
			return FALSE;
		}
		else {
			pcsGridItem->m_bForceWriteBottom = TRUE;
		}
	}
	/*
	 * We want to make sure there's only one instance of a skill/item in
	 * a single row.
	 */
	for (size_t i = 0; i < countof(m_quickBeltGrid) / 2; i++) {
		bool		is_left = (pcsGrid == &m_quickBeltGrid[i * 2]);
		bool		is_right = pcsGrid == &m_quickBeltGrid[i * 2 + 1];
		AgpdGrid *	grid;

		if (!is_left && !is_right)
			continue;
		RemoveGridLayer(&m_quickBeltGrid[i * 2], pcsGridItem, lLayer);
		RemoveGridLayer(&m_quickBeltGrid[i * 2 + 1], pcsGridItem, lLayer);
		grid = is_left ? &m_quickBeltGrid[i * 2] : &m_quickBeltGrid[i * 2 + 1];
		m_pcsAgpmGrid->Clear(grid, lLayer, lRow, lColumn, 1, 1);
		m_pcsAgpmGrid->Add(grid, lLayer, lRow, lColumn, pcsGridItem, 1, 1);
	}
	RefreshQuickBeltGrid();
	RefreshQBeltGridItemStackCount();
	if (bIsThrowEvent) {
		PVOID buffer[5];

		buffer[0] = (PVOID)pcsGrid;
		buffer[1] = (PVOID)pcsGridItem;
		buffer[2] = (PVOID)lLayer;
		buffer[3] = (PVOID)lRow;
		buffer[4] = (PVOID)lColumn;
		EnumCallback(AGCMUI_MAIN_CB_ADD_SHORTCUT_ITEM, buffer, NULL);
	}
	return TRUE;
}

BOOL AgcmUIMain::RemoveGridLayer(
	AgpdGrid *		pcsGrid, 
	AgpdGridItem *	pcsGridItem, 
	INT32			lLayer)
{
	if (!pcsGrid || !pcsGridItem)
		return FALSE;
	for (int i = 0; i < pcsGrid->m_nRow; ++i) {
		for (int j = 0; j < pcsGrid->m_nColumn; ++j) {
			if (m_pcsAgpmGrid->GetItem(pcsGrid, lLayer, i, j) == pcsGridItem)
				m_pcsAgpmGrid->Clear(pcsGrid, lLayer, i, j, 1, 1);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::RemoveShortcut(
	AgpdGrid *	pcsGrid, 
	INT32		lLayer, 
	INT32		lRow, 
	INT32		lColumn, 
	BOOL		bIsThrowEvent)
{
	BOOL bIsCleared	= FALSE;

	if (!pcsGrid)
		return FALSE;
	for (size_t i = 0; i < countof(m_quickBeltGrid); i++) {
		if (pcsGrid == &m_quickBeltGrid[i])
			bIsCleared |= m_pcsAgpmGrid->Clear(&m_quickBeltGrid[i], lLayer, lRow, lColumn, 1, 1);
	}
	RefreshQuickBeltGrid();
	if (bIsCleared)
		m_pcsAgcmUIManager2->ThrowEvent(m_lEventIDRemoveItem);
	if (bIsThrowEvent) {
		PVOID buffer[6];

		buffer[0] = (PVOID)pcsGrid;
		buffer[1] = NULL;
		buffer[2] = (PVOID)lLayer;
		buffer[3] = (PVOID)lRow;
		buffer[4] = (PVOID)lColumn;
		buffer[5] = (PVOID)bIsCleared;
		EnumCallback(AGCMUI_MAIN_CB_REMOVE_SHORTCUT_ITEM, buffer, NULL);
	}
	return TRUE;
}

BOOL AgcmUIMain::AddHPPotion(INT32 lHPPotionTID)
{
	if (lHPPotionTID == AP_INVALID_IID)
		return FALSE;

	AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(lHPPotionTID);
	if (!pcsItemTemplate)
		return FALSE;
	
	// check valid hp potion
	if (pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_POTION ||
		((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType != AGPMITEM_USABLE_POTION_TYPE_HP)
	{
		if( ((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType != AGPMITEM_USABLE_POTION_TYPE_BOTH )
			return FALSE;
	}

	AgpdCharacter		*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (m_pcsHPPotionItem && m_pcsHPPotionItem->m_lItemTID == lHPPotionTID)
		return FALSE;

	if (!m_pcsHPPotionItem)
	{
		m_pcsHPPotionItem	= m_pcsAgpmGrid->CreateGridItem();
	}

	if (!m_pcsHPPotionItem)
		return FALSE;

	AgpdItemTemplate	*pcsPrevItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsHPPotionItem->m_lItemTID);
	if (pcsPrevItemTemplate)
	{
		AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsPrevItemTemplate);

		if (pcsAgcdItemTemplate->m_pSmallTexture)
		{
			RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
			pcsAgcdItemTemplate->m_pSmallTexture = NULL;
		}
	}

	m_pcsHPPotionItem->m_lItemTID			= lHPPotionTID;
	m_pcsHPPotionItem->m_bMoveable			= TRUE;

//	m_pcsHPPotionItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItemSmall;
	m_pcsHPPotionItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItem;

	m_pcsAgcmItem->SetGridItemAttachedSmallTexture(pcsItemTemplate, m_pcsHPPotionItem);

	AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsItemTemplate);

	if (pcsAgcdItemTemplate->m_pSmallTexture)
		RwTextureAddRef(pcsAgcdItemTemplate->m_pSmallTexture);

	m_lTotalHPPotionCount	= 0;

	AgpdItemADChar	*pcsItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
	if (!pcsItemADChar)
		return FALSE;

	if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
	{
		for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nLayer; ++i)
		{
			for (int j = 0; j < pcsItemADChar->m_csCashInventoryGrid.m_nColumn; ++j)
			{
				for (int k = 0; k < pcsItemADChar->m_csCashInventoryGrid.m_nRow; ++k)
				{
					AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, i, k, j);
					if (!pcsGridItem)
						continue;

					if (pcsGridItem->m_lItemTID == lHPPotionTID)
					{
						AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
						if (!pcsItem)
							continue;

						m_lTotalHPPotionCount	+= pcsItem->m_nCount;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
		{
			for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
			{
				for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
				{
					AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
					if (!pcsGridItem)
						continue;

					if (pcsGridItem->m_lItemTID == lHPPotionTID)
					{
						AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
						if (!pcsItem)
							continue;

						m_lTotalHPPotionCount	+= pcsItem->m_nCount;
					}
				}
			}
		}
	}

	CHAR	szBuffer[10];
	ZeroMemory(szBuffer, sizeof(CHAR) * 10);

	if (m_lTotalHPPotionCount > 999)
		sprintf(szBuffer, "%d", 999);
	else
		sprintf(szBuffer, "%d", m_lTotalHPPotionCount);

	m_pcsHPPotionItem->SetRightBottomString(szBuffer);

	m_pcsAgpmGrid->Reset(&m_stHPPotionGrid);

	m_pcsAgpmGrid->AddItem(&m_stHPPotionGrid, m_pcsHPPotionItem);
	m_pcsAgpmGrid->Add(&m_stHPPotionGrid, m_pcsHPPotionItem, 1, 1);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstHPPotionGrid);

	return TRUE;
}

BOOL AgcmUIMain::RemoveHPPotion()
{
	if (!m_pcsHPPotionItem)
		return TRUE;

	AgpdItemTemplate	*pcsPrevItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsHPPotionItem->m_lItemTID);
	if (pcsPrevItemTemplate)
	{
		AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsPrevItemTemplate);

		if (pcsAgcdItemTemplate->m_pSmallTexture)
		{
			RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
			pcsAgcdItemTemplate->m_pSmallTexture = NULL;
		}
	}

	m_pcsHPPotionItem->m_lItemTID			= AP_INVALID_IID;
	m_pcsHPPotionItem->m_bMoveable			= TRUE;

	m_pcsHPPotionItem->m_pcsTemplateGrid	= NULL;

	m_lTotalHPPotionCount	= 0;

	m_pcsAgpmGrid->Reset(&m_stHPPotionGrid);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstHPPotionGrid);

	return TRUE;
}

BOOL AgcmUIMain::AddMPPotion(INT32 lMPPotionTID)
{
	if (lMPPotionTID == AP_INVALID_IID)
		return FALSE;

	AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(lMPPotionTID);
	if (!pcsItemTemplate)
		return FALSE;

	// check valid mp potion
	if (pcsItemTemplate->m_nType != AGPMITEM_TYPE_USABLE ||
		((AgpdItemTemplateUsable *) pcsItemTemplate)->m_nUsableItemType != AGPMITEM_USABLE_TYPE_POTION ||
		((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType != AGPMITEM_USABLE_POTION_TYPE_MP)
	{
		if( ((AgpdItemTemplateUsablePotion *) pcsItemTemplate)->m_ePotionType != AGPMITEM_USABLE_POTION_TYPE_BOTH ) 
			return FALSE;
	}

	AgpdCharacter		*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();
	if (!pcsSelfCharacter)
		return FALSE;

	if (m_pcsMPPotionItem && m_pcsMPPotionItem->m_lItemTID == lMPPotionTID)
		return FALSE;

	if (!m_pcsMPPotionItem)
		m_pcsMPPotionItem	= m_pcsAgpmGrid->CreateGridItem();

	if (!m_pcsMPPotionItem)
		return FALSE;

	AgpdItemTemplate	*pcsPrevItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsMPPotionItem->m_lItemTID);
	if (pcsPrevItemTemplate)
	{
		AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsPrevItemTemplate);

		if (pcsAgcdItemTemplate->m_pSmallTexture)
		{
			RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
			pcsAgcdItemTemplate->m_pSmallTexture = NULL;
		}
	}

	m_pcsMPPotionItem->m_lItemTID			= lMPPotionTID;
	m_pcsMPPotionItem->m_bMoveable			= TRUE;

//	m_pcsMPPotionItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItemSmall;
	m_pcsMPPotionItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItem;

	m_pcsAgcmItem->SetGridItemAttachedSmallTexture(pcsItemTemplate, m_pcsMPPotionItem);

	AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsItemTemplate);

	if (pcsAgcdItemTemplate->m_pSmallTexture)
		RwTextureAddRef(pcsAgcdItemTemplate->m_pSmallTexture);

	m_lTotalMPPotionCount	= 0;

	AgpdItemADChar	*pcsItemADChar	= m_pcsAgpmItem->GetADCharacter(pcsSelfCharacter);
	if (!pcsItemADChar)
		return FALSE;

	if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
	{
		for (int i = 0; i < pcsItemADChar->m_csCashInventoryGrid.m_nLayer; ++i)
		{
			for (int j = 0; j < pcsItemADChar->m_csCashInventoryGrid.m_nColumn; ++j)
			{
				for (int k = 0; k < pcsItemADChar->m_csCashInventoryGrid.m_nRow; ++k)
				{
					AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csCashInventoryGrid, i, k, j);
					if (!pcsGridItem)
						continue;

					if (pcsGridItem->m_lItemTID == lMPPotionTID)
					{
						AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
						if (!pcsItem)
							continue;

						m_lTotalMPPotionCount	+= pcsItem->m_nCount;
					}
				}
			}
		}
	}
	else
	{
		for (int i = 0; i < pcsItemADChar->m_csInventoryGrid.m_nLayer; ++i)
		{
			for (int j = 0; j < pcsItemADChar->m_csInventoryGrid.m_nColumn; ++j)
			{
				for (int k = 0; k < pcsItemADChar->m_csInventoryGrid.m_nRow; ++k)
				{
					AgpdGridItem	*pcsGridItem	= m_pcsAgpmGrid->GetItem(&pcsItemADChar->m_csInventoryGrid, i, k, j);
					if (!pcsGridItem)
						continue;

					if (pcsGridItem->m_lItemTID == lMPPotionTID)
					{
						AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsGridItem->m_lItemID);
						if (!pcsItem)
							continue;

						m_lTotalMPPotionCount	+= pcsItem->m_nCount;
					}
				}
			}
		}
	}

	CHAR	szBuffer[10];
	ZeroMemory(szBuffer, sizeof(CHAR) * 10);

	if (m_lTotalMPPotionCount > 999)
		sprintf(szBuffer, "%d", 999);
	else
		sprintf(szBuffer, "%d", m_lTotalMPPotionCount);

	m_pcsMPPotionItem->SetRightBottomString(szBuffer);

	m_pcsAgpmGrid->Reset(&m_stMPPotionGrid);

	m_pcsAgpmGrid->AddItem(&m_stMPPotionGrid, m_pcsMPPotionItem);
	m_pcsAgpmGrid->Add(&m_stMPPotionGrid, m_pcsMPPotionItem, 1, 1);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMPPotionGrid);

	return TRUE;
}

BOOL AgcmUIMain::RemoveMPPotion()
{
	if (!m_pcsMPPotionItem)
		return TRUE;

	AgpdItemTemplate	*pcsPrevItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsMPPotionItem->m_lItemTID);
	if (pcsPrevItemTemplate)
	{
		AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsPrevItemTemplate);

		if (pcsAgcdItemTemplate->m_pSmallTexture)
		{
			RwTextureDestroy(pcsAgcdItemTemplate->m_pSmallTexture);
			pcsAgcdItemTemplate->m_pSmallTexture = NULL;
		}
	}

	m_pcsMPPotionItem->m_lItemTID			= AP_INVALID_IID;
	m_pcsMPPotionItem->m_bMoveable			= TRUE;

	m_pcsMPPotionItem->m_pcsTemplateGrid	= NULL;

	m_lTotalMPPotionCount	= 0;

	m_pcsAgpmGrid->Reset(&m_stMPPotionGrid);

	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pstMPPotionGrid);

	return TRUE;
}

#ifdef _AREA_GLOBAL_
BOOL AgcmUIMain::LoadSystemMessageGridToolTip( char* pFileName, BOOL bIsEncrypt )
{
	return m_SystemGridTooltip.Initialize( m_pcsAgcmUIManager2, pFileName, bIsEncrypt );
}
BOOL AgcmUIMain::AddSystemMessageGridItem(AgpdGridItem *pcsGridItem, ApModuleDefaultCallBack pfUseCallback, PVOID pvCallbackClass , BOOL bFrontAdd, UINT32 nExpireTime )
{
	return OnAddSystemGridControl( pcsGridItem, pfUseCallback, pvCallbackClass , bFrontAdd, nExpireTime );
}
#else
BOOL AgcmUIMain::AddSystemMessageGridItem(AgpdGridItem *pcsGridItem, ApModuleDefaultCallBack pfUseCallback, PVOID pvCallbackClass , BOOL bFrontAdd )
{
	return OnAddSystemGridControl( pcsGridItem, pfUseCallback, pvCallbackClass , bFrontAdd );
}
#endif

BOOL AgcmUIMain::RemoveSystemMessageGridItem(AgpdGridItem *pcsGridItem)
{
	return OnDeleteSystemGridControl( pcsGridItem );
}

BOOL AgcmUIMain::ClearSystemMessageGridItem( VOID )
{
	m_ListSystemGridControl.clear();
	return TRUE;
}
#ifdef _AREA_GLOBAL_
UINT32 AgcmUIMain::GetGridItemExpireTime( void* pGridItem )
{
	AgpdGridItem* ppdGridItem = ( AgpdGridItem* )pGridItem;
	if( !ppdGridItem ) return 0;

	ListSystemGridEntryIter Iter = m_ListSystemGridControl.begin();
	while( Iter != m_ListSystemGridControl.end() )
	{
		stSystemGridControlEntry* pEntry = &( *Iter );
		if( pEntry && pEntry->m_ppdGridItem == pGridItem )
		{
			return pEntry->m_nExpireTime;
		}

		Iter++;
	}

	return 0;
}
#endif
BOOL			AgcmUIMain::SetArrow()
{
	AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsAgpmItem->GetArrowTID());

	m_lTotalArrowCount	= m_pcsAgpmItem->GetTotalArrowCountInInventory(m_pcsAgcmCharacter->GetSelfCharacter());

	BOOL	bIsNewCreated	= FALSE;

	if (!m_pcsArrowItem)
	{
		m_pcsArrowItem	= m_pcsAgpmGrid->CreateGridItem();
		bIsNewCreated	= TRUE;
	}

	if (!m_pcsArrowItem)
		return FALSE;

	m_pcsArrowItem->m_lItemTID			= pcsItemTemplate->m_lID;
	m_pcsArrowItem->m_bMoveable			= FALSE;

//	m_pcsArrowItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItemSmall;
	m_pcsArrowItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItem;

	m_pcsAgcmItem->SetGridItemAttachedSmallTexture(pcsItemTemplate, m_pcsArrowItem);

	AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsItemTemplate);

	if (pcsAgcdItemTemplate->m_pSmallTexture)
		RwTextureAddRef(pcsAgcdItemTemplate->m_pSmallTexture);

	CHAR	szBuffer[10];
	ZeroMemory(szBuffer, sizeof(CHAR) * 10);

	if (m_lTotalArrowCount > 999)
		sprintf(szBuffer, "%d", 999);
	else
		sprintf(szBuffer, "%d", m_lTotalArrowCount);

	m_pcsArrowItem->SetRightBottomString(szBuffer);
	m_pcsArrowItem->SetTooltip(ClientStr().GetStr(STI_ALARM_ARROW));

	return AddSystemMessageGridItem(m_pcsArrowItem);
}

BOOL			AgcmUIMain::ResetArrow()
{
	return RemoveSystemMessageGridItem(m_pcsArrowItem);;
}

BOOL			AgcmUIMain::SetBolt()
{
	AgpdItemTemplate	*pcsItemTemplate	= m_pcsAgpmItem->GetItemTemplate(m_pcsAgpmItem->GetBoltTID());

	m_lTotalBoltCount	= m_pcsAgpmItem->GetTotalBoltCountInInventory(m_pcsAgcmCharacter->GetSelfCharacter());

	if (!m_pcsBoltItem)
		m_pcsBoltItem	= m_pcsAgpmGrid->CreateGridItem();

	if (!m_pcsBoltItem)
		return FALSE;

	m_pcsBoltItem->m_lItemTID			= pcsItemTemplate->m_lID;
	m_pcsBoltItem->m_bMoveable			= FALSE;

//	m_pcsBoltItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItemSmall;
	m_pcsBoltItem->m_pcsTemplateGrid	= pcsItemTemplate->m_pcsGridItem;

	m_pcsAgcmItem->SetGridItemAttachedSmallTexture(pcsItemTemplate, m_pcsBoltItem);

	AgcdItemTemplate	*pcsAgcdItemTemplate	= m_pcsAgcmItem->GetTemplateData(pcsItemTemplate);

	if (pcsAgcdItemTemplate->m_pSmallTexture)
		RwTextureAddRef(pcsAgcdItemTemplate->m_pSmallTexture);

	CHAR	szBuffer[10];
	ZeroMemory(szBuffer, sizeof(CHAR) * 10);

	if (m_lTotalBoltCount > 999)
		sprintf(szBuffer, "%d", 999);
	else
		sprintf(szBuffer, "%d", m_lTotalBoltCount);

	m_pcsBoltItem->SetRightBottomString(szBuffer);
	m_pcsBoltItem->SetTooltip(ClientStr().GetStr(STI_ALARM_BOLT));

	return AddSystemMessageGridItem(m_pcsBoltItem);
}

BOOL			AgcmUIMain::ResetBolt()
{
	return RemoveSystemMessageGridItem(m_pcsBoltItem);;
}

// 2007.07.06. steeple
BOOL AgcmUIMain::SetOnlineTimeAddictStatus(UINT32 ulClockCount)
{
	// 장궈가 아니면 안한다.
	if(g_eServiceArea != AP_SERVICE_AREA_CHINA)
		return TRUE;

	if(!m_pcsOnlineTimeEdit || !m_pcsAddictStatusEdit)
		return FALSE;

	// 중독 방지를 해줘야 하는 대상이 아니다.
	if(m_pcsAgcmLogin->m_lIsLimited == 1)
		return TRUE;

	// 부하가 걸리니깐 5초에 한번씩 한다.
	if(ulClockCount - m_ulLastUpdateOnlineTime < AGCMUI_UPDATE_ONLINE_TIME_INTERVAL)
		return TRUE;

	m_ulLastUpdateOnlineTime = ulClockCount;

	AgpdCharacter* pcsSelfCharacter = m_pcsAgcmCharacter->GetSelfCharacter();
	if(!pcsSelfCharacter)
		return FALSE;

	if ( m_ulStartOnlineTime == 0 )
		m_ulStartOnlineTime = ulClockCount;

	// ullElapsedSeconds 는 4Byte 를 넘어가지 않을 것이다.
	// pcsSelfCharacter->m_ulOnlineTime 은 분단위로 온다.
	UINT32 ellapsedSeconds = ( UINT32 ) ( double(ulClockCount - m_ulStartOnlineTime) / 1000 );
	UINT32 ulCurrentOnlineTime = pcsSelfCharacter->m_ulOnlineTime * 60 + ellapsedSeconds;
	if(ulCurrentOnlineTime == 0)
		return FALSE;
 
	UINT32 ulHour = (UINT32)((double)ulCurrentOnlineTime / (double)3600);
	UINT32 ulMin = (UINT32)((ulCurrentOnlineTime % 3600) / (double)60);

	CHAR szBuffer[128];
	memset(szBuffer, 0, sizeof(szBuffer));

	sprintf(szBuffer, "%2d: %02d", ulHour, ulMin);
	m_pcsOnlineTimeEdit->SetText(szBuffer);

	memset(szBuffer, 0, sizeof(szBuffer));
	switch(pcsSelfCharacter->m_lAddictStatus)
	{
		case 0:
			sprintf(szBuffer, "<C%s>%s", AGCMUI_MAIN_ADDICT_STATUS_LEVEL0_COLOR, ClientStr().GetStr(STI_GREEN_STATUS));
			break;

		case 1:
			sprintf(szBuffer, "<C%s>%s", AGCMUI_MAIN_ADDICT_STATUS_LEVEL1_COLOR, ClientStr().GetStr(STI_YELLOW_STATUS));
			break;

		default:
			sprintf(szBuffer, "<C%s>%s", AGCMUI_MAIN_ADDICT_STATUS_LEVEL2_COLOR, ClientStr().GetStr(STI_RED_STATUS));
			break;
	}

	m_pcsAddictStatusEdit->SetText(szBuffer);
	
	return TRUE;
}

BOOL			AgcmUIMain::CBAddShortcut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *			pThis		= (AgcmUIMain *) pClass;
	//AgcdUIControl *			pcsControl	= (AgcdUIControl *) pData1;
	AgcdUIControl *			pcsControl	= pcsSourceControl;
	AgpdGridSelectInfo *	pstDragInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
		return FALSE;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstDragInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstDragInfo)
		return FALSE;

	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);

	if (pstGrid)
	{
		pThis->AddShortcut(pstGrid, pstDragInfo->pGridItem, pcsUIGrid->m_lNowLayer, pstDragInfo->lGridRow, pstDragInfo->lGridColumn);
	}

	pcsUIGrid->UpdateUIGrid();

	return TRUE;
}

BOOL			AgcmUIMain::CBRemoveShortcut(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *			pThis		= (AgcmUIMain *) pClass;
	//AgcdUIControl *			pcsControl	= (AgcdUIControl *) pData1;
	AgcdUIControl *			pcsControl	= pcsSourceControl;
	AgpdGridSelectInfo *	pstDragInfo;
	AgpdGrid *				pstGrid;
	AcUIGrid *				pcsUIGrid;
	BOOL					bSameGridPos = FALSE;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
		return FALSE;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstDragInfo = pcsUIGrid->GetGridItemClickInfo();
	if (!pstDragInfo)
		return FALSE;

	if (((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_SKILL_TREE ||
		((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_GRID ||
		((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_GRID_ITEM)
		return TRUE;


	pstGrid = pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsControl);
	if (pstGrid)
	{
		pThis->RemoveShortcut(pstGrid, pcsUIGrid->m_lNowLayer, pstDragInfo->lGridRow, pstDragInfo->lGridColumn);
	}

	pThis->RefreshQuickBeltGrid();

	pcsUIGrid->UpdateUIGrid();

	return TRUE;
}

BOOL			AgcmUIMain::CBAddHPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *			pThis		= (AgcmUIMain *) pClass;
	//AgcdUIControl *			pcsControl	= (AgcdUIControl *) pData1;
	AgcdUIControl *			pcsControl	= pcsSourceControl;
	AgpdGridSelectInfo *	pstDragInfo;
	AcUIGrid *				pcsUIGrid;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
		return FALSE;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstDragInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstDragInfo)
		return FALSE;

	// 캐쉬아이템은 못 올리게 한다. 2006.06.30. steeple
	AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstDragInfo->pGridItem->m_lItemTID);
	if (!pcsItemTemplate)// && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
		return FALSE;

	if (!pThis->AddHPPotion(pstDragInfo->pGridItem->m_lItemTID))
		return FALSE;

	pcsUIGrid->UpdateUIGrid();

	pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_HP_POTION, &pstDragInfo->pGridItem->m_lItemTID, NULL);

	return TRUE;
}

BOOL			AgcmUIMain::CBAddMPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *			pThis		= (AgcmUIMain *) pClass;
	//AgcdUIControl *			pcsControl	= (AgcdUIControl *) pData1;
	AgcdUIControl *			pcsControl	= pcsSourceControl;
	AgpdGridSelectInfo *	pstDragInfo;
	AcUIGrid *				pcsUIGrid;

	if (!pcsControl || (pcsControl->m_lType != AcUIBase::TYPE_GRID && pcsControl->m_lType != AcUIBase::TYPE_SKILL_TREE))
		return FALSE;

	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;

	pstDragInfo = pcsUIGrid->GetDragDropMessageInfo();
	if (!pstDragInfo)
		return FALSE;

	// 캐쉬아이템은 못 올리게 한다. 2006.06.30. steeple -> 올리게 한다. 2006.08.23. parn
	AgpdItemTemplate* pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pstDragInfo->pGridItem->m_lItemTID);
	if (!pcsItemTemplate)// && IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
		return FALSE;

	if (!pThis->AddMPPotion(pstDragInfo->pGridItem->m_lItemTID))
		return FALSE;

	pcsUIGrid->UpdateUIGrid();

	pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_MP_POTION, &pstDragInfo->pGridItem->m_lItemTID, NULL);

	return TRUE;
}

BOOL			AgcmUIMain::CBRemovePotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)		pClass;

	if (!pcsSourceControl->m_pcsBase)
		return FALSE;

	AcUIGrid			*pcsUIGrid			= (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pstDragInfo		= pcsUIGrid->GetGridItemClickInfo();
	if (!pstDragInfo)
		return FALSE;

	if (((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_SKILL_TREE ||
		((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_GRID ||
		((AcUIBase *)pstDragInfo->pTargetWindow)->m_nType == AcUIBase::TYPE_GRID_ITEM)
		return TRUE;

	AgpdGrid			*pstGrid			= pThis->m_pcsAgcmUIManager2->GetControlGrid(pcsSourceControl);
	if (pstGrid)
	{
		if (pstDragInfo->pGridItem == pThis->m_pcsHPPotionItem)
		{
			if (!pThis->RemoveHPPotion())
				return FALSE;

			pcsUIGrid->UpdateUIGrid();

			pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_HP_POTION, &pstDragInfo->pGridItem->m_lItemTID, NULL);
		}
		else if (pstDragInfo->pGridItem == pThis->m_pcsMPPotionItem)
		{
			if (!pThis->RemoveMPPotion())
				return FALSE;

			pcsUIGrid->UpdateUIGrid();

			pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_MP_POTION, &pstDragInfo->pGridItem->m_lItemTID, NULL);
		}
	}

	return TRUE;
}


BOOL			AgcmUIMain::CBDisplayExp(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *) pClass;
	AgpdCharacter *	pcsCharacter = (AgpdCharacter *) pData;

	szDisplay[0] = 0;

	if (!pThis->m_pcsAgpmFactors || !pcsCharacter || eType != AGCDUI_USERDATA_TYPE_CHARACTER)
		return FALSE;

	INT64	llCurrentExp	= pThis->m_pcsAgpmFactors->GetExp((AgpdFactor *) pThis->m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	INT64	llMaxExp		= pThis->m_pcsAgpmFactors->GetMaxExp((AgpdFactor *) pThis->m_pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));

	switch (lID)
	{
	case AGCM_UI_MAIN_EXP_STEP_REMAIN:
		{
			if (llMaxExp)
				*plValue = (INT32) (10 - 10 * llCurrentExp / llMaxExp);
			else
				*plValue = 0;

			sprintf(szDisplay, "EXP %d", *plValue);
			break;
		}

	case AGCM_UI_MAIN_EXP_STEP_PERCENTAGE:
		{
//			FLOAT	fTemp;
//
//			fTemp = lExpMax / 10.0f;
//
//			if (lExpMax)
//				fTemp = (((10 * lExpCurrent) % lExpMax) / fTemp * 10);
//			else
//				fTemp = 0;

			FLOAT	fTemp	= FLOAT((llCurrentExp / (double) llMaxExp) * 100.0f);

			*plValue = (INT32) fTemp;
			sprintf(szDisplay, "EXP %.2f%%", fTemp);

			break;
		}

	case AGCM_UI_MAIN_EXP_STEP_CURRENT:
		{
			if (llMaxExp)
				*plValue = (INT32) (((llCurrentExp * 10) % llMaxExp) / (llMaxExp / 10.0f) * 10);
			else
				*plValue = 0;

			sprintf(szDisplay, "EXP %d", *plValue);

			break;
		}

	case AGCM_UI_MAIN_EXP_STEP_MAX:
		{
			if (llMaxExp)
				*plValue = 100;
			else
				*plValue = 0;

			sprintf(szDisplay, "EXP %d", *plValue);

			break;
		}
	}

	return TRUE;
}

// 2007.07.06. steeple
BOOL AgcmUIMain::CBGetOnlineTimeEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIMain* pThis = static_cast<AgcmUIMain*>(pClass);
	AgcdUIControl* pcsEditControl = static_cast<AgcdUIControl*>(pData1);

	if(pcsEditControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	pThis->m_pcsOnlineTimeEdit = static_cast<AcUIEdit*>(pcsEditControl->m_pcsBase);

	if(pThis->m_pcsOnlineTimeEdit)
		pThis->m_pcsOnlineTimeEdit->SetText("");

	return TRUE;
}

// 2007.07.06. steeple
BOOL AgcmUIMain::CBGetAddictStatusEditControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if(!pClass || !pData1)
		return FALSE;

	AgcmUIMain* pThis = static_cast<AgcmUIMain*>(pClass);
	AgcdUIControl* pcsEditControl = static_cast<AgcdUIControl*>(pData1);

	if(pcsEditControl->m_lType != AcUIBase::TYPE_EDIT)
		return FALSE;

	pThis->m_pcsAddictStatusEdit = static_cast<AcUIEdit*>(pcsEditControl->m_pcsBase);

	if(pThis->m_pcsAddictStatusEdit)
		pThis->m_pcsAddictStatusEdit->SetText("");

	return TRUE;
}

BOOL	AgcmUIMain::CBDisplayDefault(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData)
		return FALSE;

	AgcmUIMain *	pThis = (AgcmUIMain *) pClass;

	switch (eType)
	{
	case AGCDUI_USERDATA_TYPE_INT32:
		if (plValue)
			*plValue = *(INT32 *) pData;
		sprintf(szDisplay, "%d", *(INT32 *) pData);
		break;

	case AGCDUI_USERDATA_TYPE_FLOAT:
		print_compact_format(szDisplay, "%f", *(FLOAT *) pData);
		break;

	case AGCDUI_USERDATA_TYPE_CHARACTER:
		sprintf(szDisplay, "%s", ((AgpdCharacter *) pData)->m_szID);
		break;

	case AGCDUI_USERDATA_TYPE_STRING:
		if (!(*(CHAR **) pData))
			return FALSE;

		sprintf(szDisplay, "%s", *(CHAR **) pData);
		break;

	case AGCDUI_USERDATA_TYPE_UINT64:
		sprintf(szDisplay, "%ull", *(UINT64 *) pData);
		break;

	case AGCDUI_USERDATA_TYPE_BOOL:
			*plValue = *(INT32 *) pData;
		sprintf(szDisplay, "%d", *(BOOL *) pData);
		break;

	case AGCDUI_USERDATA_TYPE_TELEPORT_POINT:
	case AGCDUI_USERDATA_TYPE_POS:
	case AGCDUI_USERDATA_TYPE_ITEM:
	case AGCDUI_USERDATA_TYPE_SKILL:
	case AGCDUI_USERDATA_TYPE_GRID:
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL	AgcmUIMain::CBDisplayInt32(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *) pClass;
	INT32 *			plNumber = (INT32 *) pData;

	*plValue = *plNumber;

	if (pData == pThis->m_apstQBLayer[1]->m_stUserData.m_pvData)
		*plValue += 4;

	sprintf(szDisplay, "%d", *plValue);

	return TRUE;
}

BOOL	AgcmUIMain::CBDisplayCurrentLayer(PVOID pClass, PVOID pData, AgcdUIDataType eType, INT32 lID, CHAR *szDisplay, INT32 *plValue, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;

	sprintf(szDisplay, "%d", pThis->m_quickBeltCurrentLayer + 1);

	return TRUE;
}

BOOL	AgcmUIMain::CBButtonControlDisable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain*	pThis		= (AgcmUIMain*)pClass	;
	
	AgcdUIControl*		pControl			= (AgcdUIControl*)pData1		;
	
	if ( !pControl || pControl->m_lType != AcUIBase::TYPE_BUTTON )
		return FALSE;

	AcUIButton*			pButton			= (AcUIButton*)(pControl->m_pcsBase )			;

	pButton->SetButtonEnable(FALSE);

	return TRUE;
}

BOOL	AgcmUIMain::CBButtonControlEnable(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain*	pThis		= (AgcmUIMain*)pClass	;
	
	AgcdUIControl*		pControl			= (AgcdUIControl*)pData1		;
	
	if ( !pControl || pControl->m_lType != AcUIBase::TYPE_BUTTON )
		return FALSE;

	AcUIButton*			pButton			= (AcUIButton*)(pControl->m_pcsBase )			;

	pButton->SetButtonEnable(TRUE);

	return TRUE;
}

BOOL	AgcmUIMain::CBEditControlInit(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain*	pThis		= (AgcmUIMain*)pClass	;
	
	AgcdUIControl*		pControl			= (AgcdUIControl*)pData1		;
	
	if ( !pControl || pControl->m_lType != AcUIBase::TYPE_EDIT )
		return FALSE;

	AcUIEdit*			pEdit			= (AcUIEdit*)(pControl->m_pcsBase )			;
	
	pEdit->ClearText()		;

	return TRUE;
}

BOOL	AgcmUIMain::CBSelectListItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pcsSourceControl)
		return FALSE;

	AgcmUIMain *		pThis		= (AgcmUIMain*)pClass	;
	AgcdUIUserData *	pstUserData;

	pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);
	if (!pstUserData)
		return FALSE;

	pstUserData->m_lSelectedIndex = pcsSourceControl->m_lUserDataIndex;
	
	return TRUE;
}

BOOL	AgcmUIMain::CBEditControlActive(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain*	pThis		= (AgcmUIMain*)pClass	;
	
	AgcdUIControl*		pControl			= (AgcdUIControl*)pData1		;
	
	if ( !pControl || pControl->m_lType != AcUIBase::TYPE_EDIT )
		return FALSE;

	AcUIEdit*			pEdit			= (AcUIEdit*)(pControl->m_pcsBase )			;

	pEdit->SetMeActiveEdit();

	return TRUE;
}

BOOL AgcmUIMain::CBLClickDown(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *			pThis;
	AcUIGrid *				pcsUIGrid;
	AgpdGridSelectInfo *	pcsClickInfo;
	INT32					nItemID;

	if (!pClass || !pcsSourceControl)
		return FALSE;
	pThis			= (AgcmUIMain *) pClass;
	pcsUIGrid		= (AcUIGrid *) pcsSourceControl->m_pcsBase;
	pcsClickInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || pcsClickInfo->pGridItem)
		return FALSE;
	for (size_t i = 0; i < countof(pThis->m_quickBeltGrid); i++)
		pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem = pcsClickInfo->pGridItem;
	nItemID = pcsClickInfo->pGridItem->m_lItemID;
	switch(pcsClickInfo->pGridItem->m_eType) {
	case AGPDGRID_ITEM_TYPE_ITEM:
		pThis->m_pcsAgcmItem->UseItem(nItemID);
		break;
	case AGPDGRID_ITEM_TYPE_SKILL:
		pThis->ProcessSkill(nItemID);
		break;
	case AGPDGRID_ITEM_TYPE_SOCIALACTION:
		pThis->m_pcsAgcmCharacter->SendSocialAnimation(nItemID);
		break;
	}
	return TRUE;
}

BOOL AgcmUIMain::CBRClickDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)		pClass;

	pThis->m_ulRClickDownTMSec			= pThis->GetClockCount();

	return TRUE;
}

BOOL AgcmUIMain::CBRClickUp(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *			pThis;
	AgcdUIControl *			pcsControl;
	AcUIGrid *				pcsUIGrid;
	AgpdGridSelectInfo *	pcsClickInfo;

	if (!pClass || !pData1)
		return FALSE;
	pThis = (AgcmUIMain *) pClass;
	pcsControl = (AgcdUIControl *)	pcsSourceControl;
	pcsUIGrid = (AcUIGrid *) pcsControl->m_pcsBase;
	pcsClickInfo = pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || !pcsClickInfo->pGridItem)
		return FALSE;
	for (size_t i = 0; i < countof(pThis->m_quickBeltGrid); i++)
		pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= pcsClickInfo->pGridItem;
	if (pcsClickInfo->pGridItem->m_eType == AGPDGRID_ITEM_TYPE_ITEM) {
		if (pThis->m_pcsAgcmItem) {	
			AgpdItem * pcsItem = pThis->m_pcsAgpmItem->GetItem(pcsClickInfo->pGridItem);

			if( ((AgpdItemTemplate *)pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP )
				return pThis->UseRideItemInQuickBelt(pcsItem);					
			else
				pThis->m_pcsAgcmItem->UseItem(pcsClickInfo->pGridItem->m_lItemID);
		}
	}
	else if (pcsClickInfo->pGridItem->m_eType == AGPDGRID_ITEM_TYPE_SKILL) {
		AgcmUISkill2 *	pcmUISkill = ( AgcmUISkill2* )pThis->GetModule( "AgcmUISkill2" );
		AgpmSkill *		ppmSkill = ( AgpmSkill* )pThis->GetModule( "AgpmSkill" );
		AgcmCharacter *	pcmCharacter = ( AgcmCharacter* )pThis->GetModule( "AgcmCharacter" );
		
		if( pcmUISkill && ppmSkill && pcmCharacter ) {
			AgpdCharacter *	ppdCharacter = pcmCharacter->GetSelfCharacter();
			AgpdSkill *		ppdSkill = ppmSkill->GetSkillByTID( ppdCharacter, pcsClickInfo->pGridItem->m_lItemTID );

			if( !pcmUISkill->IsUsableSkill( ppdSkill ) )
				return TRUE;
			pThis->ProcessSkill( pcsClickInfo->pGridItem->m_lItemTID );
		}		
	}
	return TRUE;
}

BOOL	AgcmUIMain::CBPotionGridRClickUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl || !pcsSourceControl->m_pcsBase)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *) pClass;

	AcUIGrid			*pcsUIGrid		= (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pcsClickInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || !pcsClickInfo->pGridItem)
		return FALSE;

	if (pcsClickInfo->pGridItem == pThis->m_pcsHPPotionItem)
	{
		if (pThis->m_lTotalHPPotionCount <= 0)
			return TRUE;

		AgpdItem			*pcsHPPotion;
		AgpdItemTemplate	*pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_pcsHPPotionItem->m_lItemTID);

		if (!pcsItemTemplate)
			return FALSE;

		if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
			pcsHPPotion = pThis->m_pcsAgpmItem->GetCashInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsHPPotionItem->m_lItemTID);
		else
			pcsHPPotion = pThis->m_pcsAgpmItem->GetInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsHPPotionItem->m_lItemTID);

		if (!pcsHPPotion)
			return FALSE;

		return pThis->m_pcsAgcmItem->UseItem(pcsHPPotion);
	}
	else if (pcsClickInfo->pGridItem == pThis->m_pcsMPPotionItem)
	{
		if (pThis->m_lTotalMPPotionCount <= 0)
			return TRUE;

		AgpdItem	*pcsMPPotion;
		AgpdItemTemplate	*pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_pcsMPPotionItem->m_lItemTID);

		if (!pcsItemTemplate)
			return FALSE;

		if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
			pcsMPPotion = pThis->m_pcsAgpmItem->GetCashInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsMPPotionItem->m_lItemTID);
		else
			pcsMPPotion = pThis->m_pcsAgpmItem->GetInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsMPPotionItem->m_lItemTID);

		if (!pcsMPPotion)
			return FALSE;

		return pThis->m_pcsAgcmItem->UseItem(pcsMPPotion);
	}

	return TRUE;
}

BOOL	AgcmUIMain::CBClickSystemGridNoAction(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass) return FALSE;

	// 이 콜백은 단순히 이벤트를 받아서 씹어먹기 위한 콜백이므로 이 콜백 안에서는 아무짓도 하면 안된다.
	return FALSE;
}

BOOL	AgcmUIMain::CBClickSystemGrid(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *) pClass;

	AcUIGrid			*pcsUIGrid		= (AcUIGrid *) pcsSourceControl->m_pcsBase;

	AgpdGridSelectInfo	*pcsClickInfo	= pcsUIGrid->GetGridItemClickInfo();
	if (!pcsClickInfo || !pcsClickInfo->pGridItem)
		return FALSE;

	return pThis->OnClickSystemGridControl( pcsClickInfo->pGridItem );
}

BOOL AgcmUIMain::CBQuickBeltUse_1(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2;
	INT16			col = 0;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_2(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2;
	INT16			col = 1;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_3(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2;
	INT16			col = 2;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_4(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2;
	INT16			col = 3;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_5(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2;
	INT16			col = 4;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_6(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2 + 1;
	INT16			col = 0;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_7(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2 + 1;
	INT16			col = 1;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_8(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2 + 1;
	INT16			col = 2;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_9(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2 + 1;
	INT16			col = 3;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltUse_10(
	PVOID			pClass, 
	PVOID			pData1, 
	PVOID			pData2, 
	PVOID			pData3, 
	PVOID			pData4, 
	PVOID			pData5, 
	ApBase *		pcsTarget, 
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *)pClass;
	size_t			ud_index = get_quickbelt_index() * 2 + 1;
	INT16			col = 4;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	if (pThis->m_quickBeltUD[ud_index] && 
		pThis->m_quickBeltUD[ud_index]->m_stUserData.m_pvData) {
		AgcdUIUserData *	ud = pThis->m_quickBeltUD[ud_index];
		AgpdGrid *			qb_grid = (AgpdGrid *)ud->m_stUserData.m_pvData;
		AgpdGridItem *		grid_item = 
			pThis->m_pcsAgpmGrid->GetItem(qb_grid, 0, 0, col);

		if (grid_item) {
			for (size_t i = 0; i <  countof(pThis->m_quickBeltGrid); i++)
				pThis->m_quickBeltGrid[i].m_pcsSelectedGridItem	= grid_item;
			pThis->UseGridItem(grid_item);
		}
	}
	return TRUE;
}

BOOL	AgcmUIMain::UseBelt( INT32 nLayer , INT32 nIndex )
{
	//AgcmUIMain			*pThis				= this;
	//AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

	//if( !pcsSelfCharacter ) return FALSE;
	//if( nIndex < 0 || nIndex > 9 ) return FALSE;
	//if( nLayer < 0 || nLayer > 3 ) return FALSE;

	//if (pThis->m_apstQuickBelt && pThis->m_apstQuickBelt[0] && pThis->m_apstQuickBelt[0]->m_stUserData.m_pvData)
	//{
	//	AgpdGrid *pstQuickBeltGrid = (AgpdGrid *) pThis->m_apstQuickBelt[0]->m_stUserData.m_pvData;

	//	AgpdGridItem	*pcsGridItem	= pThis->m_pcsAgpmGrid->GetItem(pstQuickBeltGrid, pThis->m_alCurrentLayer[0] , nLayer, nIndex);
	//	if (pcsGridItem)
	//	{
	//		pThis->m_astQuickBeltGrid[0].m_pcsSelectedGridItem	= pcsGridItem;
	//		pThis->m_astQuickBeltGrid[1].m_pcsSelectedGridItem	= pcsGridItem;

	//		if (pcsGridItem->m_eType == AGPDGRID_ITEM_TYPE_ITEM)
	//		{
	//			AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(pcsGridItem);
	//			if (pcsItem && pcsItem->m_pcsItemTemplate)
	//			{
	//				if (((AgpdItemTemplate *) pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP)
	//				{
	//					// 
	//				}
	//				else
	//				{
	//					if (m_pcsAgcmItem)
	//						m_pcsAgcmItem->UseItem(pcsGridItem->m_lItemID);
	//				}
	//			}
	//			return TRUE;
	//		}
	//		else if (pcsGridItem->m_eType == AGPDGRID_ITEM_TYPE_SKILL)
	//		{
	//			INT32	lTargetID	= m_pcsAgcmCharacter->GetSelectTarget(pcsSelfCharacter);

	//			BOOL bForce = FALSE;

	//			if( m_pcsAgcmSkill->CastSkillCheck( pcsGridItem->m_lItemID , (ApBase *) m_pcsAgpmCharacter->GetCharacter(lTargetID) , NULL , bForce ) )
	//			{
	//				ProcessSkill(pcsGridItem->m_lItemID);
	//				return TRUE;
	//			}
	//		}
	//	}

	//	return FALSE;
	//}
	//else
	//{
	//	return FALSE;
	//}
	return TRUE;
}

BOOL	AgcmUIMain::CBQuickBeltUseHPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)			pClass;

	if (!pThis->m_pcsHPPotionItem || pThis->m_lTotalHPPotionCount <= 0)
		return TRUE;

	AgpdItemTemplate	*pcsItemTemplate;
	AgpdItem			*pcsHPPotion;

	pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_pcsHPPotionItem->m_lItemTID);
	if (!pcsItemTemplate)
		return FALSE;

	if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
	{
		pcsHPPotion	= pThis->m_pcsAgpmItem->GetCashInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsHPPotionItem->m_lItemTID);
		if (!pcsHPPotion)
			return FALSE;
	}
	else
	{
		pcsHPPotion	= pThis->m_pcsAgpmItem->GetInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsHPPotionItem->m_lItemTID);
		if (!pcsHPPotion)
			return FALSE;
	}

	return pThis->m_pcsAgcmItem->UseItem(pcsHPPotion);
}

BOOL	AgcmUIMain::CBQuickBeltUseMPPotion(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)			pClass;

	if (!pThis->m_pcsMPPotionItem || pThis->m_lTotalMPPotionCount <= 0)
		return TRUE;

	AgpdItemTemplate	*pcsItemTemplate;
	AgpdItem			*pcsMPPotion;

	pcsItemTemplate = pThis->m_pcsAgpmItem->GetItemTemplate(pThis->m_pcsMPPotionItem->m_lItemTID);
	if (!pcsItemTemplate)
		return FALSE;

	if (IS_CASH_ITEM(pcsItemTemplate->m_eCashItemType))
	{
		pcsMPPotion	= pThis->m_pcsAgpmItem->GetCashInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsMPPotionItem->m_lItemTID);
		if (!pcsMPPotion)
			return FALSE;
	}
	else
	{
		pcsMPPotion	= pThis->m_pcsAgpmItem->GetInventoryItemByTID(pThis->m_pcsAgcmCharacter->GetSelfCharacter(), pThis->m_pcsMPPotionItem->m_lItemTID);
		if (!pcsMPPotion)
			return FALSE;
	}

	return pThis->m_pcsAgcmItem->UseItem(pcsMPPotion);
}

BOOL	AgcmUIMain::CBQuickBeltLayerUp(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *		pThis = (AgcmUIMain *)pClass;
	AgcmUIManager2 *	uimgr;
	INT32				prev_layer;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	uimgr = pThis->m_pcsAgcmUIManager2;
	prev_layer = pThis->m_quickBeltCurrentLayer++;
	if (pThis->m_quickBeltCurrentLayer >= AGCMUIMAIN_MAX_QUICKBELT) {
		pThis->m_quickBeltCurrentLayer	= AGCMUIMAIN_MAX_QUICKBELT - 1;
	}
	else {
		//AgcdUI *		cinfo = uimgr->GetUI("UI_CharInfo");
		//AgcdUIControl *	base;
		//AgcdUIControl *	hp_mp;
		//AgcdUIControl *	grid;
		//char			name[128];

		//if (!cinfo)
		//	return FALSE;
		//base = uimgr->GetControl(cinfo, "CONTROL_CHARINFO_BASE");
		//if (!base)
		//	return FALSE;
		//hp_mp = uimgr->GetControl(base, "CONTROL_CHARINFO_BASE_HPMP_TEXTURE");
		//if (!hp_mp)
		//	return FALSE;
		//sprintf(name, "CONTROL_GRID_TEXTURE_%d", pThis->m_quickBeltCurrentLayer);
		//grid = uimgr->GetControl(base, name);
		//if (!grid)
		//	return FALSE;
		//if (pThis->m_initialHpMpBarY == MAXINT16)
		//	pThis->m_initialHpMpBarY = hp_mp->m_pcsBase->y;
		//hp_mp->m_pcsBase->MoveWindow(hp_mp->m_pcsBase->x, 
		//	pThis->m_initialHpMpBarY - pThis->m_quickBeltCurrentLayer * 52,
		//	hp_mp->m_pcsBase->w, hp_mp->m_pcsBase->h);
		switch (prev_layer) {
		case 0:
			uimgr->ThrowEvent(pThis->m_eventShowAdditionalQuickBelt1);
			break;
		case 1:
			uimgr->ThrowEvent(pThis->m_eventShowAdditionalQuickBelt2);
			break;
		case 2:
			uimgr->ThrowEvent(pThis->m_eventShowAdditionalQuickBelt3);
			break;
		}
	}
	for (size_t i = 0; i < countof(pThis->m_apstQBLayer); i++)
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(pThis->m_apstQBLayer[i]);
	return TRUE;
}

BOOL AgcmUIMain::CBQuickBeltLayerDown(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *	pThis;
	INT32			prev_layer;

	if (!pClass)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	prev_layer = pThis->m_quickBeltCurrentLayer--;
	if (pThis->m_quickBeltCurrentLayer < 0)
		pThis->m_quickBeltCurrentLayer = 0;
	/* 
	 * Layer hasn't changed.
	 */
	if (!prev_layer)
		return TRUE;
	switch (prev_layer) {
	case 1:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt1);
		break;
	case 2:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt2);
		break;
	case 3:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt3);
		break;
	}
	for (size_t i = 0; i < countof(pThis->m_apstQBLayer); i++)
		pThis->m_pcsAgcmUIManager2->SetUserDataRefresh(
			pThis->m_apstQBLayer[i]);
	return TRUE;
}

BOOL	AgcmUIMain::CBCloseUIWindow(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)			pClass;


	AgpdCharacter *pcsSelfCharacter = pThis->m_pcsAgcmCharacter->GetSelfCharacter();

	if( pcsSelfCharacter && pcsSelfCharacter->m_ulSpecialStatus & AGPDCHAR_SPECIAL_STATUS_DONT_MOVE )
	{
		// 블럭걸려있으니까 아무짓도 안함
		return TRUE;
	}

	AgcmEventNPCDialog* pcmEventNPCDialog = ( AgcmEventNPCDialog* )pThis->GetModule( "AgcmEventNPCDialog" );
	if( pcmEventNPCDialog )
	{
		CExNPCDialog* pExNpcDialog = pcmEventNPCDialog->GetExNPC();
		if( pExNpcDialog )
		{
			pExNpcDialog->OnHideNPCDialog();
		}

		CExNPC* pExNpc = pcmEventNPCDialog->GetExNPC();
		if( pExNpc )
		{
			// Wait 메세지박스가 떠 있는 동안에는 이거 처리하면 안된다.
			if( pExNpc->IsOpenWaitingMsgBox() ) return TRUE;
			pExNpc->OnHideNPCMsgBox();
		}
	}

	BOOL	bIsCloseWindow	= FALSE;	

	pThis->m_pcsAgcmUIManager2->CloseAllUIExceptMainUI(&bIsCloseWindow);

	//UI가 닫히는경우. 정의된 Callback에 따라서 처리가된다.
	//@{ 2006/11/02
	//pThis->EnumCallback( AGCMUI_MAIN_CB_KEYDOWN_ESC, NULL, NULL );
	pThis->EnumCallback( AGCMUI_MAIN_CB_KEYDOWN_ESC, &bIsCloseWindow, NULL );
	//@}

	if (!bIsCloseWindow)
	{
		// 닫힌 윈도우가 하나도 없는 경우이다.
		// 이런 경우엔 이넘 말고 다른넘에서 뭔 짓을 해야할듯 하다.
		// 콜백을 불러준다.
		pThis->EnumCallback(AGCMUI_MAIN_CB_KEYDOWN_CLOSE_ALL_UI_EXCEPT_MAIN, NULL, NULL);
	}

	return TRUE;
}

BOOL
AgcmUIMain::CBShowAdditionalQuickBelt(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis;
	AgcdUIControl *	grid_base;

	if (!pClass || !pData1)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	grid_base = (AgcdUIControl *)pData1;
	pThis->m_pcsAgcmUIManager2->ThrowEvent(
		pThis->m_eventUpdateHpMpBarPosition);
	grid_base->m_pcsBase->ShowWindow(1);
	return TRUE;
}

BOOL
AgcmUIMain::CBHideAdditionalQuickBelt(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis;
	AgcdUIControl *	grid_base;

	if (!pClass || !pData1)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	grid_base = (AgcdUIControl *)pData1;
	grid_base->m_pcsBase->ShowWindow(0);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(
		pThis->m_eventUpdateHpMpBarPosition);
	return TRUE;
}

BOOL
AgcmUIMain::CBUpdateHpMpBarPosition(
	PVOID			pClass,
	PVOID			pData1,
	PVOID			pData2,
	PVOID			pData3,
	PVOID			pData4,
	PVOID			pData5,
	ApBase *		pcsTarget,
	AgcdUIControl *	pcsSourceControl)
{
	AgcmUIMain *	pThis;
	AgcdUIControl *	hp_mp;

	if (!pClass || !pData1)
		return FALSE;
	pThis = (AgcmUIMain *)pClass;
	hp_mp = (AgcdUIControl *)pData1;
	if (pThis->m_initialHpMpBarY == MAXINT16)
		pThis->m_initialHpMpBarY = hp_mp->m_pcsBase->y;
	hp_mp->m_pcsBase->MoveWindow(hp_mp->m_pcsBase->x, 
		pThis->m_initialHpMpBarY - pThis->m_quickBeltCurrentLayer * 52,
		hp_mp->m_pcsBase->w, hp_mp->m_pcsBase->h);
	return TRUE;
}

BOOL AgcmUIMain::RemoveGridItemFromQuickBelt(AgpdGridItem *pcsGridItem)
{
 	if (!pcsGridItem)
		return FALSE;
	for (size_t i = 0; i < countof(m_quickBeltGrid); i++)
		m_pcsAgpmGrid->RemoveItem(&m_quickBeltGrid[i], pcsGridItem);
	for (size_t i = 0; i < countof(m_quickBeltUD); i++)
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_quickBeltUD[i]);
	return TRUE;
}

BOOL AgcmUIMain::CheckQuickBeltItem(INT32 lItemID)
{
	if (!m_pcsAgpmItem)
		return TRUE;

	AgpdItem	*pcsItem	= m_pcsAgpmItem->GetItem(lItemID);
	if (!pcsItem)
		return FALSE;

	if (!pcsItem->m_pcsCharacter || !pcsItem->m_pcsItemTemplate)
		return FALSE;

	if (pcsItem->m_eStatus != AGPDITEM_STATUS_INVENTORY && pcsItem->m_eStatus != AGPDITEM_STATUS_CASH_INVENTORY)		//	2005.12.12. By SungHoon
	{
		if(pcsItem->m_eStatus != AGPDITEM_STATUS_EQUIP || ((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE)
			return FALSE;
	}

	AgpdItemTemplateUsable	*pcsTemplateUsable = (AgpdItemTemplateUsable *) pcsItem->m_pcsItemTemplate;

	/*
	if (pcsTemplateUsable->m_nType == AGPMITEM_TYPE_EQUIP)
		return TRUE;
	else */if (pcsTemplateUsable->m_nType == AGPMITEM_TYPE_USABLE)
	{
		if (pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_POTION ||
			pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TELEPORT_SCROLL ||
			pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_TRANSFORM ||
			pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL ||
			pcsTemplateUsable->m_nUsableItemType == AGPMITEM_USABLE_TYPE_AVATAR)
			return TRUE;
	}
	else if(pcsTemplateUsable->m_nType == AGPMITEM_TYPE_EQUIP)
	{
		if(	((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_RIDE)
			return TRUE;
	}

	return FALSE;
}

BOOL AgcmUIMain::SetCallbackOpenTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_OPEN_TOOLTIP, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackCloseTooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_CLOSE_TOOLTIP, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackCloseAllUITooltip(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_CLOSE_ALL_UI_TOOLTIP, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackUpdateAlarmGrid(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_UPDATE_ALARM_GRID, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackRButtonSkillCast(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_RBUTTON_SKILL_CAST, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackKeydownCloseAllUIExceptMain(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_KEYDOWN_CLOSE_ALL_UI_EXCEPT_MAIN, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackKeydownESC(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_KEYDOWN_ESC, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackAddShortcut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_ADD_SHORTCUT_ITEM, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackRemoveShortcut(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_REMOVE_SHORTCUT_ITEM, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackUpdateHPPotion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_UPDATE_HP_POTION, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackUpdateMPPotion(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_UPDATE_MP_POTION, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackUpdateHPPointGage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_UPDATE_HPPOINT_GAGE, pfCallback, pClass);
}

BOOL AgcmUIMain::SetCallbackUpdateMPPointGage(ApModuleDefaultCallBack pfCallback, PVOID pClass)
{
	return SetCallback(AGCMUI_MAIN_CB_UPDATE_MPPOINT_GAGE, pfCallback, pClass);
}

BOOL AgcmUIMain::CBOpenToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)	pClass;

	if ( !pcsSourceControl || !pcsSourceControl->m_pcsBase )
		return FALSE;
	
	return pThis->EnumCallback(AGCMUI_MAIN_CB_OPEN_TOOLTIP, pcsSourceControl, NULL);
}

BOOL AgcmUIMain::CBCloseToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)	pClass;

	if ( !pcsSourceControl || !pcsSourceControl->m_pcsBase )
		return FALSE;
	
	return pThis->EnumCallback(AGCMUI_MAIN_CB_CLOSE_TOOLTIP, pcsSourceControl, NULL);
}

BOOL	AgcmUIMain::CBOpenSystemGridToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if( !pClass )
		return FALSE;

	AgcmUIMain			*pThis		=	static_cast< AgcmUIMain* >(pClass);

	if( !pcsSourceControl || !pcsSourceControl->m_pcsBase )
		return FALSE;

	pThis->_OpenSystemGridTooltip( pcsSourceControl );

	return TRUE;
}

BOOL	AgcmUIMain::CBCloseSystemGridToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{					
	if( !pClass )
		return FALSE;

	AgcmUIMain			*pThis		=	static_cast< AgcmUIMain* >(pClass);

	if( !pcsSourceControl || !pcsSourceControl->m_pcsBase )
		return FALSE;

	pThis->_CloseSystemGridTooltip( pcsSourceControl );

	return TRUE;
}


BOOL AgcmUIMain::CBCloseAllUIToolTip(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)	pClass;

	return pThis->EnumCallback(AGCMUI_MAIN_CB_CLOSE_ALL_UI_TOOLTIP, NULL, NULL);
}

BOOL AgcmUIMain::RefreshQuickBeltGrid()
{
	for (size_t i = 0; i < countof(m_quickBeltUD); i++)
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_quickBeltUD[i]);
	return TRUE;
}

BOOL AgcmUIMain::UpdateAlarmGrid()
{
	ClearAlarmGrid();

	AgcmUICharacter* pcmUICharacter = ( AgcmUICharacter* )g_pEngine->GetModule( "AgcmUICharacter" );
	if( pcmUICharacter )
	{
		pcmUICharacter->OnUpdateProperty();
	}

	return EnumCallback(AGCMUI_MAIN_CB_UPDATE_ALARM_GRID, NULL, NULL);
}

BOOL AgcmUIMain::AddItemToAlarmGrid(AgpdGridItem *pcsGridItem)
{
	if (!pcsGridItem)
		return FALSE;

	BOOL bResult	= FALSE;

	switch (pcsGridItem->m_eType) {
	case AGPDGRID_ITEM_TYPE_SKILL:
		{
			// 2006.09.10. steeple
			// 스킬 버프가 32개로 늘어났고, 오른쪽부터 채워나간다.
			for (int j = m_csAlarmGrid.m_nColumn - 1; j >= 0; --j)
			{
				for (int i = 0; i < m_csAlarmGrid.m_nRow; ++i)
				{
					if (m_pcsAgpmGrid->Add(&m_csAlarmGrid, 0, i, j, pcsGridItem, 1, 1))
					{
						bResult = TRUE;
						break;
					}
				}

				if (bResult)
					break;
			}

			//bResult = m_pcsAgpmGrid->Add(&m_csAlarmGrid, pcsGridItem, 1, 1);
		}
		break;

//	case AGPDGRID_ITEM_TYPE_ITEM:
//		{
//			// 이넘은 거꾸로 넣어야 한다.
//
//			INT32	nRow	= m_csAlarmGrid.m_nRow - 1;
//			//INT32	nColumn	= m_csAlarmGrid.m_nColumn - 1;
//			INT32	nColumn	= 0;
//
//			for (int i = 0; i < AGPMITEM_PART_NUM; ++i)
//			{
//				if (m_pcsAgpmGrid->Add(&m_csAlarmGrid, 0, nRow, nColumn, pcsGridItem, 1, 1))
//				{
//					bResult	= TRUE;
//					break;
//				}
//
//				--nRow;
//				if (nRow < 0)
//				{
//					++nColumn;
//					nRow	= m_csAlarmGrid.m_nRow - 1;
//
//					if (nColumn >= m_csAlarmGrid.m_nColumn)
//					{
//						bResult = FALSE;
//						break;
//					}
//				}
//
//				/*
//				--nColumn;
//				if (nColumn < 0)
//				{
//					nColumn = m_csAlarmGrid.m_nColumn;
//					--nRow;
//
//					if (nRow < 0)
//					{
//						bResult = FALSE;
//						break;
//					}
//				}
//				*/
//			}
//		}
//		break;
	}

	if (bResult)
	{
		// refresh alarm grid user data
		m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsAlarmGrid);
	}

	return bResult;
}

BOOL AgcmUIMain::ClearAlarmGrid()
{
	m_pcsAgpmGrid->Reset(&m_csAlarmGrid);

	// refresh alarm grid user data
	m_pcsAgcmUIManager2->SetUserDataRefresh(m_pcsAlarmGrid);

	return TRUE;
}

void AgcmUIMain::UpdateMediaRateWindow(UINT32 ulClockCount)
{
	if (0 == m_ulPrevProcessClockCount)
	{
		m_pcsAgcmUIManager2->ThrowEvent( m_lEventCloseMediaRate );
		m_bMediaRateWindow_Show = FALSE;
		UINT32 ulMin = ((INT32) AGCMUIMAIN_MEDIARATE_INTERVAL_HIDE/2);
		m_ulMediaRateWindow_RemainSec = ulMin + m_csRand.randInt(ulMin - 1);		//  클라 띄우고 적어도 25분후부터 보여준다.
		m_ulMediaRateWindow_LastTick = ulClockCount;
		return;
	}
	
	if (ulClockCount > (m_ulMediaRateWindow_LastTick + m_ulMediaRateWindow_RemainSec * 1000))
	{
		if (m_bMediaRateWindow_Show)
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_lEventCloseMediaRate );
			m_ulMediaRateWindow_RemainSec = AGCMUIMAIN_MEDIARATE_INTERVAL_HIDE;
			m_bMediaRateWindow_Show = FALSE;
		}
		else
		{
			m_pcsAgcmUIManager2->ThrowEvent( m_bIsNC17 ? m_lEventOpenMediaRate18 : m_lEventOpenMediaRate );
			m_ulMediaRateWindow_RemainSec = AGCMUIMAIN_MEDIARATE_INTERVAL_SHOW;
			m_bMediaRateWindow_Show = TRUE;
		}

		m_ulMediaRateWindow_LastTick = ulClockCount;
	}
}

BOOL	AgcmUIMain::CBUIReturnAppQuit	(PVOID pClass, INT32 lTrueCancel, CHAR* szInputMessage )
{
	AgcmUIMain* pThis = (AgcmUIMain*)pClass;

	// 종료하기전 AutoPickUp 설정 저장
	AgcmUIItem* pcmUIItem = ( AgcmUIItem* )pThis->GetModule( "AgcmUIItem" );
	if( pcmUIItem )
	{
		pcmUIItem->m_AutoPickUp.SaveToFile( "ini\\AutoPickUp.xml" );
	}


	if ( lTrueCancel )
		RsEventHandler(rsQUITAPP, NULL);		// 정말 종료하시겠습니까? Yes

	return TRUE;
}

BOOL	AgcmUIMain::CBApplicationQuitEventCall(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain* pThis = (AgcmUIMain*)pClass;

	if( pThis->m_pcsAgpmCharacter->IsCombatMode( pThis->m_pcsAgcmCharacter->GetSelfCharacter() ) )
	{
		char* pMessage = pThis->m_pcsAgcmUIManager2->GetUIMessage( "BattleNotUse" );
		if( pMessage )
		{
			static AgcmUISystemMessage* _spcsAgcmUISystemMessage = ( AgcmUISystemMessage * ) g_pEngine->GetModule( "AgcmUISystemMessage" );
			_spcsAgcmUISystemMessage->AddSystemMessage( pMessage );
		}

		return TRUE;
	}

	if( pClass )
	{
		pThis->m_pcsAgcmUIManager2->ThrowEvent( pThis->m_lEventMainQuit );
	}

	return TRUE;
}

BOOL	AgcmUIMain::CBQuickBeltAdjustPos(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *		pThis = (AgcmUIMain*) pClass;
	AgcdUI *			pcsUI;
	INT32				lOffset;

	if (!pcsSourceControl)
		return FALSE;

	pcsUI = pcsSourceControl->m_pcsParentUI;
	if (!pcsUI)
		return FALSE;

	lOffset = (pcsUI->m_pcsUIWindow->w - pcsUI->m_pcsUIWindow->h); // / 2;

	INT32	lTopX	= pcsUI->m_pcsUIWindow->x + lOffset;
	INT32	lTopY	= pcsUI->m_pcsUIWindow->y - lOffset;

	INT32	lBottomX	= pcsUI->m_pcsUIWindow->w;
	INT32	lBottomY	= pcsUI->m_pcsUIWindow->h;

	INT32	lMaxWidth	= 1024;
	INT32	lMaxHeight	= 768;

	if (lTopX < 0)
		lTopX = 0;
	if (lTopY < 0)
		lTopY = 0;
	if (lTopX + lBottomY > lMaxWidth)
	{
		lTopX		-= lTopX + lBottomY - lMaxWidth;
	}
	if (lTopY + lBottomX > lMaxHeight)
	{
		lTopY		-= lTopY + lBottomX - lMaxHeight;
	}

	pcsUI->m_pcsUIWindow->MoveWindow( lTopX, lTopY, lBottomX, lBottomY );

	return TRUE;
}

BOOL	AgcmUIMain::CBQuickBeltVAdjustPos(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *		pThis = (AgcmUIMain*) pClass;
	AgcdUI *			pcsUI;
	INT32				lOffset;

	if (!pcsSourceControl)
		return FALSE;

	pcsUI = pcsSourceControl->m_pcsParentUI;
	if (!pcsUI || !pcsUI->m_pcsParentUI )
		return FALSE;

	lOffset = (pcsUI->m_pcsParentUI->m_pcsUIWindow->h - pcsUI->m_pcsParentUI->m_pcsUIWindow->w); // / 2;

	INT32	lTopX	= pcsUI->m_pcsParentUI->m_pcsUIWindow->x + lOffset;
	INT32	lTopY	= pcsUI->m_pcsParentUI->m_pcsUIWindow->y - lOffset;

	INT32	lBottomX	= pcsUI->m_pcsParentUI->m_pcsUIWindow->w;
	INT32	lBottomY	= pcsUI->m_pcsParentUI->m_pcsUIWindow->h;

	INT32	lMaxWidth	= 1024;
	INT32	lMaxHeight	= 768;

	if (lTopX < 0)
		lTopX = 0;
	if (lTopY < 0)
		lTopY = 0;
	if (lTopX + lBottomX > lMaxWidth)
	{
		lTopX		-= lTopX + lBottomX - lMaxWidth;
	}
	if (lTopY + lBottomY > lMaxHeight)
	{
		lTopY		-= lTopY + lBottomY - lMaxHeight;
	}

	pcsUI->m_pcsParentUI->m_pcsUIWindow->MoveWindow( lTopX, lTopY, lBottomX, lBottomY );

	return TRUE;
}

//AgpdGridItem* AgcmUIMain::GetSkillGridItem()
//{
//	return m_stSkillGrid.m_ppcGridData[0];
//}

BOOL		AgcmUIMain::CBDefaultBoolean(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	if (eType != AGCDUI_USERDATA_TYPE_BOOL)
		return FALSE;

	return *(BOOL *) pData;
}

BOOL		AgcmUIMain::CBSelfCharacterDead(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *	pThis = (AgcmUIMain *) pClass;

	if (!pThis->m_pcsAgcmCharacter || !pThis->m_pcsAgcmCharacter->GetSelfCharacter())
		return FALSE;

	return (pThis->m_pcsAgcmCharacter->GetSelfCharacter()->m_unActionStatus == AGPDCHAR_STATUS_DEAD);
}

BOOL		AgcmUIMain::CBIsSelected(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourceControl)
{
	AgcmUIMain *		pThis = (AgcmUIMain *) pClass;
	AgcdUIUserData *	pstUserData = pThis->m_pcsAgcmUIManager2->GetControlUserData(pcsSourceControl);

	if (!pstUserData)
		return FALSE;

	if (pstUserData->m_lSelectedIndex == pcsSourceControl->m_lUserDataIndex)
		return TRUE;

	return FALSE;
}

BOOL		AgcmUIMain::CBSetFocusMovableItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)	pClass;

	if (!pcsSourceControl || 
		(pcsSourceControl->m_lType != AcUIBase::TYPE_GRID && pcsSourceControl->m_lType != AcUIBase::TYPE_SKILL_TREE) ||
		!pcsSourceControl->m_pcsBase)
		return FALSE;
	
	AcUIGrid			*pcsGrid		= (AcUIGrid *)		(pcsSourceControl->m_pcsBase);

	// 기존 커서를 저장한다. 무조건 저장한다.
	pThis->m_lPrevCursor				= pThis->m_pcsAgcmUIManager2->GetCurrentCursor();

	if (pcsGrid->m_pToolTipAgpdGridItem && pcsGrid->m_pToolTipAgpdGridItem->m_bMoveable && pcsGrid->m_bGridItemMovable)
	{
		if (pThis->m_lDragCursorID == (-1))
		{
			RwV2d	vMCPointing;
			RwV2d	vMCSize;

			vMCPointing.x	= 9;
			vMCPointing.y	= 12;

			vMCSize.x		= 64;
			vMCSize.y		= 64;

//			RwTexture*		pTexMouseCursor	= RwTextureRead("cursor_moveitem1.png", NULL);
//
//			if (pTexMouseCursor)
//				pThis->m_lDragCursorID	= pThis->m_pcsAgcmUIManager2->AddCursor(pTexMouseCursor, 700, vMCPointing, vMCSize);

			HINSTANCE		hInstance	;

			VERIFY( hInstance = ( HINSTANCE ) ::GetWindowLong( g_pEngine->GethWnd() , GWL_HINSTANCE ) );
			HCURSOR			hCursor	= ::LoadCursor( hInstance , "CURSOR_MOVEITEM1.CUR");
			if( hCursor )
			{
				pThis->m_lDragCursorID	= pThis->m_pcsAgcmUIManager2->AddCursor( hCursor , 700 );
			}
		}

		if (pThis->m_lDragCursorID >= 0)
		{
			pThis->LockRender();

			if (!pThis->m_pcsAgcmUIManager2->ChangeCursor(pThis->m_lDragCursorID, pThis))
				pThis->m_lPrevCursor	= (-1);

			pThis->UnlockRender();
		}
	}
	else
	{
		pThis->LockRender();

		pThis->m_pcsAgcmUIManager2->ChangeCursor(pThis->m_lPrevCursor, pThis);

		pThis->UnlockRender();
	}

	return TRUE;
}

BOOL		AgcmUIMain::CBKillFocusMovableItem(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain			*pThis			= (AgcmUIMain *)	pClass;

	if (pThis->m_lPrevCursor >= 0)
	{
		pThis->LockRender();
		pThis->m_pcsAgcmUIManager2->ChangeCursor(pThis->m_lPrevCursor, pThis);
		pThis->m_lPrevCursor	= (-1);
		pThis->UnlockRender();
	}

	return TRUE;
}

BOOL AgcmUIMain::SetHPPointScrollBarValue(INT32 lHPPoint)
{
	if (!m_pcsHPPointScrollBarControl)
		return FALSE;

	if (lHPPoint < 0 || lHPPoint > 100)
		return FALSE;

	AcUIScroll	*pcsAcUIScroll	= (AcUIScroll *)	m_pcsHPPointScrollBarControl->m_pcsBase;
	if (!pcsAcUIScroll)
		return FALSE;

	pcsAcUIScroll->SetScrollValue((float) lHPPoint / 100.0f);

	return TRUE;
}

BOOL AgcmUIMain::SetMPPointScrollBarValue(INT32 lMPPoint)
{
	if (!m_pcsMPPointScrollBarControl)
		return FALSE;

	if (lMPPoint < 0 || lMPPoint > 100)
		return FALSE;

	AcUIScroll	*pcsAcUIScroll	= (AcUIScroll *)	m_pcsMPPointScrollBarControl->m_pcsBase;
	if (!pcsAcUIScroll)
		return FALSE;

	pcsAcUIScroll->SetScrollValue((float) lMPPoint / 100.0f);

	return TRUE;
}

BOOL		AgcmUIMain::CBChangeHPPointScollBarValue(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMain	*pThis	= (AgcmUIMain *)	pClass;

	AcUIScroll	*pcsAcUIScroll	= (AcUIScroll *)	pcsSourceControl->m_pcsBase;
	if (!pcsAcUIScroll)
		return FALSE;

	pThis->m_lHPPointScrollBarValue	= (INT32) (pcsAcUIScroll->GetScrollValue() * 100);

	pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_HPPOINT_GAGE, NULL, NULL);

	return TRUE;
}

BOOL		AgcmUIMain::CBChangeMPPointScollBarValue(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pcsSourceControl)
		return FALSE;

	AgcmUIMain	*pThis	= (AgcmUIMain *)	pClass;

	AcUIScroll	*pcsAcUIScroll	= (AcUIScroll *)	pcsSourceControl->m_pcsBase;
	if (!pcsAcUIScroll)
		return FALSE;

	pThis->m_lMPPointScrollBarValue	= (INT32) (pcsAcUIScroll->GetScrollValue() * 100);

	pThis->EnumCallback(AGCMUI_MAIN_CB_UPDATE_MPPOINT_GAGE, NULL, NULL);

	return TRUE;
}

BOOL AgcmUIMain::CBGetPotionScollBarControl(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass || !pData1 || !pData2)
		return FALSE;

	AgcmUIMain	*pThis	= (AgcmUIMain *)	pClass;

	if( pThis->IsCurrentUI( ( AgcdUIControl* )pData1 ) )
	{
		pThis->m_pcsHPPointScrollBarControl	= (AgcdUIControl *)	pData1;
		pThis->m_pcsMPPointScrollBarControl	= (AgcdUIControl *)	pData2;
	}

	return TRUE;
}

BOOL AgcmUIMain::CBMainUIAllClose(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	if (!pClass)
		return FALSE;

	AgcmUIMain	*pThis	= (AgcmUIMain *)	pClass;
	return pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventMainUIAllClose);
}

BOOL		AgcmUIMain::ProcessSkill(INT32 lSkillID)
{
	if (m_pcsAgcmSkill)
	{
		AgpdCharacter	*pcsSelfCharacter	= m_pcsAgcmCharacter->GetSelfCharacter();

		INT32	lTargetID	= m_pcsAgcmCharacter->GetSelectTarget(pcsSelfCharacter);

		// 2005.03.28. steeple
		// 강제 스킬 공격인지 구분
		BOOL bForce = GetKeyState(VK_CONTROL) < 0 ? TRUE : FALSE;

		if (m_pcsAgcmSkill->CastSkill(lSkillID, lTargetID, bForce))
		{
			AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkillByTID(pcsSelfCharacter, lSkillID);
			if (!pcsSkill || !pcsSkill->m_pcsTemplate)
				return TRUE;

			if (!((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_bIsAutoAttack)
			{
				m_pcsAgcmCharacter->SetCastSkill(pcsSelfCharacter);
			}

			if (m_pcsAgpmSkill)
			{
				// 마법사는 Lock 하지 않는다.
				if (AgcdUIOption::IsAutoAttackEnabled() && 
					m_pcsAgpmFactors->GetClass(&pcsSelfCharacter->m_pcsCharacterTemplate->m_csFactor) != AUCHARCLASS_TYPE_MAGE) {
					// 타겟이 자기 자신이 아닌 경우에만 Lock 한다.
					//AgpdSkill	*pcsSkill	= m_pcsAgpmSkill->GetSkill(lSkillID);

					// 2005.04.04. steeple 변경
					// 체크를 보안했음
					// 2005.01.22. steeple 변경
					//if (!(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_SELF_ONLY))
					if(pcsSelfCharacter->m_lID != lTargetID &&
						!(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_SELF_ONLY) &&
						!(((AgpdSkillTemplate *) pcsSkill->m_pcsTemplate)->m_lTargetType & AGPMSKILL_TARGET_FRIENDLY_UNITS))
					{						
						m_pcsAgcmCharacter->LockTarget(pcsSelfCharacter, lTargetID);						
					}
				}

			}
			return TRUE;
		}
		else
		{
			m_pcsAgcmCharacter->ResetCastSkill(m_pcsAgcmCharacter->GetSelfCharacter());
			return FALSE;
		}
	}

	return TRUE;
}

void AgcmUIMain::RefreshItemCount( AgpdItem * item )
{
	if( !item )
		return;

	RefreshItemCount( m_lTotalHPPotionCount,	m_pcsHPPotionItem	, item );
	RefreshItemCount( m_lTotalMPPotionCount,	m_pcsMPPotionItem	, item );
	RefreshItemCount( m_lTotalArrowCount,		m_pcsArrowItem		, item );
	RefreshItemCount( m_lTotalBoltCount,		m_pcsBoltItem		, item );
}

INT32 AgcmUIMain::RefreshItemCount( INT32 & count, AgpdGridItem * grid, AgpdItem * item )
{
	if ( grid )
	{
		if( item && grid->m_lItemTID == item->m_pcsItemTemplate->m_lID )
		{
			CHAR	szBuffer[10] = { 0, };

			count = GetItemTotalCount( item );

			sprintf_s(szBuffer, sizeof(szBuffer), "%d", count > 999 ? 999 : count);

			grid->SetRightBottomString(szBuffer);
		}
	}
	else
	{
		count = 0;
	}

	return count;
}

INT32 AgcmUIMain::GetItemTotalCount( AgpdItem * item )
{
	if( !item || !item->m_pcsItemTemplate || !m_pcsAgcmCharacter )
		return 0;

	AgpdItemStatus invenType = AGPDITEM_STATUS_INVENTORY;

	if( IS_CASH_ITEM( ((AgpdItemTemplate *) item->m_pcsItemTemplate)->m_eCashItemType ) )
		invenType = AGPDITEM_STATUS_CASH_INVENTORY;

	return m_pcsAgpmItem->GetItemTotalCountFromGrid(m_pcsAgcmCharacter->GetSelfCharacter(), item->m_pcsItemTemplate->m_lID, invenType);
}

//BOOL AgcmUIMain::CBInitItem(PVOID pData, PVOID pClass, PVOID pCustData)
BOOL AgcmUIMain::CBAddItemToInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	if(!pcsItem->m_pcsItemTemplate)
		return FALSE;

	pThis->RefreshItemCount( pcsItem );

	return TRUE;
}

//BOOL AgcmUIMain::CBRemoveItem(PVOID pData, PVOID pClass, PVOID pCustData)
BOOL AgcmUIMain::CBRemoveItemFromInventory(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;

	pThis->RefreshItemCount( pcsItem );

	return TRUE;
}

BOOL AgcmUIMain::CBUpdateStackCount(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

  	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;
	AgpdItem		*pcsItem		= (AgpdItem *)		pData;
	PVOID			*ppvBuffer		= (PVOID *)			pCustData;

	// Sub Inventory 는 하지 않는다. 2008.07.16. steeple
	if(pcsItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY)
		return TRUE;

	INT32 lPrevCount		= *(INT32 *) ppvBuffer[0];

	pThis->RefreshItemCount( pcsItem );

	return TRUE;
}

BOOL AgcmUIMain::CBGetRegisteredMPPotionTID(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)		pClass;
	INT32				*plMPPotionTID		= (INT32 *)				pData;

	if (pThis->m_pcsMPPotionItem)
		*plMPPotionTID	= pThis->m_pcsMPPotionItem->m_lItemTID;

	return TRUE;
}

BOOL AgcmUIMain::CBEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)		pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsItem->m_pcsCharacter)
		return TRUE;

	AgpdItemTemplate	*pcsWeaponTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	if (pcsWeaponTemplate && ((AgpdItemTemplateEquip *) pcsWeaponTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		switch (((AgpdItemTemplateEquipWeapon *) pcsWeaponTemplate)->m_nWeaponType) {
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:
			{
				return pThis->SetArrow();
			}
			break;

		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:
			{
				return pThis->SetBolt();
			}
			break;
		}
	}

	return TRUE;
}

BOOL AgcmUIMain::CBUnEquipItem(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain			*pThis				= (AgcmUIMain *)		pClass;
	AgpdItem			*pcsItem			= (AgpdItem *)			pData;

	if (pThis->m_pcsAgcmCharacter->GetSelfCharacter() != pcsItem->m_pcsCharacter)
		return TRUE;

	AgpdItemTemplate	*pcsWeaponTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	if (pcsWeaponTemplate && ((AgpdItemTemplateEquip *) pcsWeaponTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		switch (((AgpdItemTemplateEquipWeapon *) pcsWeaponTemplate)->m_nWeaponType) {
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:
			{
				return pThis->ResetArrow();
			}
			break;

		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:
			{
				return pThis->ResetBolt();
			}
			break;
		}
	}

	return TRUE;
}

BOOL AgcmUIMain::CBSetSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;

	AgpdItem		*pcsItem		= pThis->m_pcsAgcmItem->GetCurrentEquipWeapon(pcsCharacter);
	if (!pcsItem)
		return TRUE;

	AgpdItemTemplate	*pcsWeaponTemplate	= (AgpdItemTemplate *) pcsItem->m_pcsItemTemplate;
	if (pcsWeaponTemplate && ((AgpdItemTemplateEquip *) pcsWeaponTemplate)->m_nKind == AGPMITEM_EQUIP_KIND_WEAPON)
	{
		switch (((AgpdItemTemplateEquipWeapon *) pcsWeaponTemplate)->m_nWeaponType) {
		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_BOW:
			{
				return pThis->SetArrow();
			}
			break;

		case AGPMITEM_EQUIP_WEAPON_TYPE_TWO_HAND_CROSSBOW:
			{
				return pThis->SetBolt();
			}
			break;
		}
	}

	return TRUE;
}

BOOL AgcmUIMain::CBReleaseSelfCharacter(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIMain * pThis = (AgcmUIMain *)pClass;
	AgpdCharacter * pcsSelfCharacter;
	AgcdCharacter * pstAgcdCharacter;
	INT32 lPrevLayer;

	if (!pThis)
		return FALSE;
	pcsSelfCharacter = (AgpdCharacter *)pData;
	pstAgcdCharacter = (AgcdCharacter *)pCustData;
	// 2006.03.14. steeple
	// 캐릭터가 변신중에 불린 거라면, 그리드 초기화 하지 않는다.
	if(pstAgcdCharacter && pstAgcdCharacter->m_bTransforming)
		return TRUE;
	pThis->m_pcsAgpmGrid->Reset(&pThis->m_stHPPotionGrid);
	pThis->m_pcsAgpmGrid->Reset(&pThis->m_stMPPotionGrid);
	pThis->m_pcsAgpmGrid->Reset(&pThis->m_stSystemMessageGrid);
	/*
	 * Reset additional quick belts.
	 */
	lPrevLayer = pThis->m_quickBeltCurrentLayer;
	pThis->m_quickBeltCurrentLayer = 0;
	switch (lPrevLayer) {
	case 1:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt1);
		break;
	case 2:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt2);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt1);
		break;
	case 3:
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt3);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt2);
		pThis->m_pcsAgcmUIManager2->ThrowEvent(
			pThis->m_eventHideAdditionalQuickBelt1);
		break;
	}
	for (size_t i = 0; i < countof(pThis->m_quickBeltGrid); i++)
		pThis->m_pcsAgpmGrid->Reset(&pThis->m_quickBeltGrid[i]);
	pThis->m_pcsAgpmGrid->Reset(&pThis->m_csAlarmGrid);
	pThis->RemoveHPPotion();
	pThis->RemoveMPPotion();
	return TRUE;
}

BOOL AgcmUIMain::UseGridItem(AgpdGridItem * pcsGridItem)
{
	if( !pcsGridItem )		return FALSE;

	INT32 nItemID = pcsGridItem->m_lItemID;
	switch( pcsGridItem->m_eType )
	{
	case AGPDGRID_ITEM_TYPE_ITEM:
		{
			AgpdItem* pcsItem = m_pcsAgpmItem->GetItem( pcsGridItem );
			if( pcsItem && pcsItem->m_pcsItemTemplate )
			{
				if( ((AgpdItemTemplate *)pcsItem->m_pcsItemTemplate)->m_nType == AGPMITEM_TYPE_EQUIP )
				{
					return UseRideItemInQuickBelt(pcsItem);					
				}
				else
					m_pcsAgcmItem->UseItem( nItemID, m_pcsAgcmCharacter->GetSelfCharacter() );
			}
		}
		break;
	case AGPDGRID_ITEM_TYPE_SKILL:
		ProcessSkill( pcsGridItem->m_lItemTID );
		break;
	case AGPDGRID_ITEM_TYPE_SOCIALACTION:
		m_pcsAgcmCharacter->SendSocialAnimation( nItemID );
		break;
	}

	return TRUE;
}

// 2007.08.03. steeple
BOOL AgcmUIMain::RefreshQBeltGridItemStackCount()
{
	for (size_t i = 0; i < countof(m_quickBeltUD); i++) {
		AgcdUIUserData *	ud = m_quickBeltUD[i];
		AgpdGrid *			grid = (AgpdGrid *)ud->m_stUserData.m_pvData;

		for (size_t j = 0; j < AGPMUISTATUS_HALF_QUICKBELT_COLUMN; j++) {
			AgpdGridItem *	grid_item = m_pcsAgpmGrid->GetItem(grid, 0, 0, j);
			INT32			adjust_count;
			AgpdItem *		item;

			if (!grid_item || grid_item->m_eType != AGPDGRID_ITEM_TYPE_ITEM)
				continue;
			adjust_count = 0;
			item = m_pcsAgpmItem->GetItem(grid_item);
			if (!item || !item->m_pcsItemTemplate || item->m_nCount < 0)
				continue;
			if (IS_CASH_ITEM(item->m_pcsItemTemplate->m_eCashItemType) &&
				(item->m_nInUseItem == AGPDITEM_CASH_ITEM_INUSE || item->m_nInUseItem == AGPDITEM_CASH_ITEM_PAUSE) &&
				item->m_pcsItemTemplate &&
				item->m_pcsItemTemplate->m_nType == AGPMITEM_TYPE_USABLE &&
				((AgpdItemTemplateUsable *)item->m_pcsItemTemplate)->m_nUsableItemType == AGPMITEM_USABLE_TYPE_SKILL_SCROLL &&
#ifdef _AREA_GLOBAL_
				(item->m_lRemainTime > 0 || item->m_lExpireTime > 0)) {
#else
				(item->m_lRemainTime > 0 || (item->m_lExpireTime != 0 && item->m_lExpireTime != 0xFFFFFFFF)) ) {
#endif
				++adjust_count;
			}
			_snprintf_s(grid_item->m_strRightBottom, 
				AGPDGRIDITEM_BOTTOM_STRING_LENGTH + 1, _TRUNCATE, "%d", 
				item->m_nCount- adjust_count);
		}
	}
	return TRUE;
}

BOOL	AgcmUIMain::CBChangeCharacterBindingRegion(PVOID pData, PVOID pClass, PVOID pCustData)
{
	static	INT32 lFont1	= 6;
	static	INT32 lFont2	= 7;
	static	INT32 lFont3	= 3;
	static	INT32 lOffsetY1;
	static	INT32 lOffsetY2;
	static	INT32 lOffsetY3;
	static	INT32 lDuration	= 3000;

	//static	FLOAT fScale1	= 1.2f;
	//static	FLOAT fScale2	= 0.7f;
	static	FLOAT fScale1	= 1.0f;
	static	FLOAT fScale2	= 1.0f;
	static	FLOAT fScale3	= 1.0f;

	static	UINT32 uColorSafe	= 0xff00ff00;
	static	UINT32 uColorFree	= 0xffffff00;
	static	UINT32 uColorDanger	= 0xffff0000;

	if (!pData || !pClass)
		return FALSE;

	AgcmUIMain		*pThis			= (AgcmUIMain *)	pClass;
	AgpdCharacter	*pcsCharacter	= (AgpdCharacter *)	pData;
	INT16			nPrevRegion		= *( ( INT16 * ) pCustData );

	lOffsetY1 = INT32( pThis->m_pcsAgcmUIManager2->m_csEffectWindow.h * 0.19f );
	lOffsetY2 = lOffsetY1 + 55;
	lOffsetY3 = lOffsetY1 - 40;

	// 자기케릭터만 체크한다.
	if( pThis->m_pcsAgcmCharacter->m_pcsSelfCharacter != pcsCharacter )
		return TRUE;

	// 이렇게 하면 안되고 버퍼링을 해야한다.
	/*
	static UINT32 __suLastChangeTime = 0;
	if( INT32( GetTickCount() - __suLastChangeTime ) < lDuration + 500 )
		// 500은 딜레이갭..
	{
		// 딜레이내에서 변하는것은 일단 무시해서 리턴시킨다.
		// 강제로 이전 리전으로 튕겨냄..
		//pcsCharacter->m_nBindingRegionIndex = nPrevRegion;
		return TRUE;
	}
	__suLastChangeTime = GetTickCount();
	*/
	// 이렇게 하면 안되고 버퍼링을 해야한다.

	// 리젼이 변경된 경우다..
	// 체크체크..

	TRACE( "케릭터 리젼이동 ( %d->%d )\n" , nPrevRegion , pcsCharacter->m_nBindingRegionIndex );

	ApmMap::RegionTemplate * pTemplate = pThis->m_pcsApmMap->GetTemplate( pcsCharacter->m_nBindingRegionIndex );

	ASSERT( NULL != pTemplate );

	if( pTemplate )
	{
		// ( CHAR *szText, INT32 lFont, INT32 lOffsetY, BOOL bImageFont, DWORD dwColor, FLOAT fScale, INT32 lDuration, BOOL bExistSubText = FALSE	);

		UINT32	uColor;
		char	strMessage_Region[ 256 ] , strMessage_Safe[ 256 ];

		// 안전도 표시..
		stEffUseInfo stUseInfo	;
		stUseInfo.m_ulEffID		=	3802	;
		stUseInfo.m_pFrmParent	=	NULL	;
		stUseInfo.m_ulFlags		&= ~stEffUseInfo::E_FLAG_LINKTOPARENT;

		// 3802~3804 는 형규아저씨가 작업한 이펙트 번호를
		// 하드코딩한것..

		switch( pTemplate->ti.stType.uSafetyType )
		{
		default:
		case ApmMap::ST_SAFE	:
			{
				wsprintf( strMessage_Safe , ClientStr().GetStr(STI_SAFE_AREA) );
				stUseInfo.m_ulEffID		=	3802	;
				uColor = uColorSafe;
				break;
			}
		case ApmMap::ST_FREE	:
			{
				uColor = uColorFree;
				wsprintf( strMessage_Safe , ClientStr().GetStr(STI_FREE_BATTLEFIELD) );
				stUseInfo.m_ulEffID		=	3803	;
				break;
			}
		case ApmMap::ST_DANGER	:
			{
				uColor = uColorDanger;
				wsprintf( strMessage_Safe , ClientStr().GetStr(STI_DANGEROUS_BATTLEFIELD) );
				stUseInfo.m_ulEffID		=	3804	;
				break;
			}
		}

		// 지역표시..
		std::string tname = RegionLocalName().GetStr( pTemplate->pStrName);
		wsprintf( strMessage_Region , "%s" , tname == "" ? pTemplate->pStrName : tname.c_str() );
		pThis->m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectText(
			strMessage_Region			,
			lFont1		,
			lOffsetY1	,
			FALSE		,
			uColor		,
			fScale1		,
			lDuration	, 
			TRUE		,
			TRUE		);

		// 이펙트 플레이..

		// 2005/01/22 마고자
		// 3804 이펙트가 물소리로 무한반복 돼어있어서..
		// 던젼 들어가면 물소리가 나오고 꺼지지 않음..
		// 일단 데이타 수정 됄때까지 블러킹.
		//
		// 2005/1/22 이종석
		// 사운드는 이펙트로 말고 직접 호출하주기로 해서 뺐음.
		// UIPvP에서 처리함
		//
		//pThis->m_pcsAgcmEff2->UseEffSet( &stUseInfo )	;

		pThis->m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectSubText(
			strMessage_Safe			,
			lFont2		,
			lOffsetY2	,
			FALSE		,
			uColor		,
			fScale2		);

		// 서버표시..
		pThis->m_pcsAgcmUIManager2->m_csEffectWindow.SetEffectAboveText(
			pThis->m_szGameServerName			,
			lFont3		,
			lOffsetY3	,
			FALSE		,
			uColor		,
			fScale3		);
	}

	return TRUE;
}

BOOL AgcmUIMain::CBIDPasswordOK(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUIMain *		pThis = (AgcmUIMain *)pClass;
	AgcmUIManager2 *	uimgr;
	AgcdUI *			cinfo;

	if (!pThis)
		return FALSE;
	uimgr = pThis->m_pcsAgcmUIManager2;
	uimgr->ThrowEvent(pThis->m_lEventGetPotionScrollBarControl);
	if (g_eServiceArea == AP_SERVICE_AREA_JAPAN)
		pThis->m_bEnableExceptJapan = FALSE;
	uimgr->SetUserDataRefresh(pThis->m_pcsEnableExceptJapan);
	cinfo = uimgr->GetUI("UI_CharInfo");
	if (cinfo) {
		AgcUIWindow * wnd = cinfo->m_pcsUIWindow;
		INT16 y = uimgr->GetMainWindow()->h - wnd->h;

		cinfo->m_pcsUIWindow->MoveWindow(wnd->x, y, wnd->w, wnd->h);
	}
	return TRUE;
}

BOOL AgcmUIMain::CBBooleanIsKorea(PVOID pClass, PVOID pData, AgcdUIDataType eType, AgcdUIControl *pcsSourcecontrol)
{
	return g_eServiceArea == AP_SERVICE_AREA_KOREA ? TRUE : FALSE;
}

BOOL AgcmUIMain::CBUpdateOnlineTime(PVOID pData, PVOID pClass, PVOID pCustData)
{
	if(!pData || !pClass)
		return FALSE;

	AgcmUIMain* pThis = static_cast<AgcmUIMain*>(pClass);
	static AgcmUISystemMessage * _spcsAgcmUISystemMessage = ( AgcmUISystemMessage * ) g_pEngine->GetModule( "AgcmUISystemMessage" );
	if( NULL == _spcsAgcmUISystemMessage ) return FALSE;

	// Event 를 실행시킨다.
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetOnlineTimeEdit);
	pThis->m_pcsAgcmUIManager2->ThrowEvent(pThis->m_lEventGetAddictStatusEdit);

	// 경고 메시지 출력하자.
	AgpdCharacter* pAgpdCharacter = (AgpdCharacter*)pData;

	char buffer[256] = {0, };
	int online = pAgpdCharacter->m_ulOnlineTime;

	if ( online >= 300 )
	{
		_spcsAgcmUISystemMessage->AddSystemMessage( ClientStr().GetStr(STI_RED_TEXT) );
	}
	else if ( online >= 180 )
	{
		_snprintf_s( buffer, 256, _TRUNCATE, ClientStr().GetStr(STI_YELLOW_TEXT), online / 60 );
		_spcsAgcmUISystemMessage->AddSystemMessage( buffer );
	}
	else if ( online >= 60 )
	{
		_spcsAgcmUISystemMessage->AddSystemMessage( ClientStr().GetStr(STI_GREEN_GREETING) );
		
		_snprintf_s( buffer, 256, _TRUNCATE, ClientStr().GetStr(STI_GREEN_TEXT), online / 60 );
		_spcsAgcmUISystemMessage->AddSystemMessage( buffer );
	}
	else
	{
		_spcsAgcmUISystemMessage->AddSystemMessage( ClientStr().GetStr(STI_GREEN_GREETING) );

		if ( online >= 180 )
		{
			_snprintf_s( buffer, 256, _TRUNCATE, ClientStr().GetStr(STI_YELLOW_TEXT), online / 60 );
			_spcsAgcmUISystemMessage->AddSystemMessage( buffer );
		}
		else if ( online >= 60 )
		{
			_snprintf_s( buffer, 256, _TRUNCATE, ClientStr().GetStr(STI_GREEN_TEXT), online / 60 );
			_spcsAgcmUISystemMessage->AddSystemMessage( buffer );
		}
	}

	return TRUE;
}

BOOL AgcmUIMain::IsCurrentUI( void* pControl )
{
	AgcdUIControl *		pcdControl;
	AgcdUI *			pcdUI;
	AgcmUIManager2 *	pcmUIManager;

	if ( !pControl )
		return FALSE;
	pcdControl = ( AgcdUIControl* )pControl;
	pcdUI = pcdControl->m_pcsParentUI;
	if( !pcdUI ) 
		return FALSE;
	pcmUIManager = m_pcsAgcmUIManager2;
	if( !pcmUIManager )
		return FALSE;
	return (strcmp( pcdUI->m_szUIName, "UI_CharInfo" ) == 0) ? TRUE : FALSE;
}

#ifdef _AREA_GLOBAL_
BOOL AgcmUIMain::OnAddSystemGridControl( AgpdGridItem* ppdGridItem, ApModuleDefaultCallBack fnCallBack, void* pClass , BOOL bFrontAdd, UINT32 nExpireTime )
#else
BOOL AgcmUIMain::OnAddSystemGridControl( AgpdGridItem* ppdGridItem, ApModuleDefaultCallBack fnCallBack, void* pClass , BOOL bFrontAdd )
#endif
{
	if( !ppdGridItem ) 
		return FALSE;

	ListSystemGridEntryIter		Iter	=	m_ListSystemGridControl.begin();
	for( ; Iter != m_ListSystemGridControl.end() ; ++Iter )
	{
		stSystemGridControlEntry*	pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_ppdGridItem )
		{
			if( pEntry->m_ppdGridItem == ppdGridItem )
				return TRUE;
		}
	}

	stSystemGridControlEntry NewEntry;

	NewEntry.m_ppdGridItem = ppdGridItem;
	NewEntry.m_fnCallBack = fnCallBack;
	NewEntry.m_pClass = pClass;
#ifdef _AREA_GLOBAL_
	NewEntry.m_nExpireTime = nExpireTime;
#endif

	if( bFrontAdd )
		m_ListSystemGridControl.push_front( NewEntry );

	else
		m_ListSystemGridControl.push_back( NewEntry );



	return OnRefreshSystemGrid();
}

BOOL AgcmUIMain::OnDeleteSystemGridControl( AgpdGridItem* ppdGridItem )
{
	if( !ppdGridItem ) 
		return FALSE;

	ListSystemGridEntryIter		Iter	=	m_ListSystemGridControl.begin();
	for( ; Iter != m_ListSystemGridControl.end() ; ++Iter )
	{
		stSystemGridControlEntry*	pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_ppdGridItem )
		{
			if( pEntry->m_ppdGridItem == ppdGridItem )
			{
				m_ListSystemGridControl.erase( Iter );
				break;
			}
		}
	}

	return OnRefreshSystemGrid();
}

BOOL AgcmUIMain::OnClickSystemGridControl( AgpdGridItem* ppdGridItem )
{
	if( !ppdGridItem ) 
		return FALSE;

	ListSystemGridEntryIter		Iter	=	m_ListSystemGridControl.begin();
	for( ; Iter != m_ListSystemGridControl.end() ; ++Iter )
	{
		stSystemGridControlEntry*	pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_ppdGridItem )
		{
			if( pEntry->m_ppdGridItem == ppdGridItem && pEntry->m_fnCallBack )
				return pEntry->m_fnCallBack( pEntry->m_ppdGridItem, pEntry->m_pClass, NULL );
		}
	}

	return FALSE;
}

BOOL AgcmUIMain::OnClearSystemGrid( void )
{
	AgpmGrid*			ppmGrid			= ( AgpmGrid* )g_pEngine->GetModule( "AgpmGrid" );
	AgcmUIManager2*		pcmUIManager	= ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !ppmGrid || !pcmUIManager ) 
		return FALSE;

	ppmGrid->Reset( &m_stSystemMessageGrid );
	m_ListSystemGridControl.clear();

	if( m_pstSystemMessageGrid )
	{
		pcmUIManager->SetUserDataRefresh( m_pstSystemMessageGrid );
	}

	return TRUE;
}

BOOL AgcmUIMain::OnRefreshSystemGrid( void )
{
	AgpmGrid* ppmGrid = ( AgpmGrid* )g_pEngine->GetModule( "AgpmGrid" );
	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )g_pEngine->GetModule( "AgcmUIManager2" );
	if( !ppmGrid || !pcmUIManager ) return FALSE;

	ppmGrid->Reset( &m_stSystemMessageGrid );

	ListSystemGridEntryIter		Iter	=	m_ListSystemGridControl.begin();
	for( INT i = 0 ; Iter != m_ListSystemGridControl.end() ; ++Iter , ++i )
	{
		stSystemGridControlEntry*	pEntry	=	&(*Iter);
		if( pEntry && pEntry->m_ppdGridItem )
		{
			if( ppmGrid->AddItem( &m_stSystemMessageGrid, pEntry->m_ppdGridItem ) )
				ppmGrid->Add( &m_stSystemMessageGrid, 0, 0, i , pEntry->m_ppdGridItem, 1, 1 );
		}
	}

	if( m_pstSystemMessageGrid )
	{
		pcmUIManager->SetUserDataRefresh( m_pstSystemMessageGrid );
	}

	return TRUE;
}

BOOL	AgcmUIMain::_OpenSystemGridTooltip( AgcdUIControl *pcsSourceControl )
{
	AcUIGrid*	pUIGrid	=	static_cast< AcUIGrid* >(pcsSourceControl->m_pcsBase);
	if( !pUIGrid || !pUIGrid->m_pToolTipAgpdGridItem )
		return FALSE;

	AgcdUI*	pUI	=	m_pcsAgcmUIManager2->GetUI( "UI_SystemMessageGrid" );
	if( pUI )
	{
		pUI->m_pcsUIWindow->SetFocus();
		pUI->m_pcsUIWindow->m_Property.bTopmost	= FALSE;
		
	}
#ifdef _AREA_GLOBAL_
	UINT nExpireTime = GetGridItemExpireTime( pUIGrid->m_pToolTipAgpdGridItem );
	return m_SystemGridTooltip.OpenSystemGridTooltip( pUIGrid->m_pToolTipAgpdGridItem->m_szTooltip, nExpireTime );
#else
	return m_SystemGridTooltip.OpenSystemGridTooltip( pUIGrid->m_pToolTipAgpdGridItem->m_szTooltip );
#endif
}

BOOL	AgcmUIMain::_CloseSystemGridTooltip( AgcdUIControl *pcsSourceControl )
{

	AgcdUI*	pUI	=	m_pcsAgcmUIManager2->GetUI( "UI_SystemMessageGrid" );
	if( pUI )
	{
		pUI->m_pcsUIWindow->m_Property.bTopmost	= TRUE;
	}

	return m_SystemGridTooltip.CloseSystemGridTooltip();
}

BOOL AgcmUIMain::UseRideItemInQuickBelt(AgpdItem* pcsItem)
{
	if(!pcsItem) return FALSE;

	if(	((AgpdItemTemplateEquip*)pcsItem->m_pcsItemTemplate)->m_nKind != AGPMITEM_EQUIP_KIND_RIDE)
		return FALSE;
	

	AgcmUIItem *pcsAgcmUIItem = (AgcmUIItem *)GetModule("AgcmUIItem");

	if ( pcsItem->m_eStatus == AGPDITEM_STATUS_EQUIP)
	{
		// 벗는 행동이므로 벗을 수 있는지 한번 확인을 거친다.
		if( !pcsAgcmUIItem->IsEnableUnEquipItem( pcsItem ) ) return FALSE;

		return m_pcsAgcmItem->SendInventoryInfo(pcsItem->m_lID, m_pcsAgcmCharacter->m_lSelfCID);
	}			
	else
	{
		if( !pcsAgcmUIItem->OnCheckEquipEnable( pcsItem ) )
			return FALSE;

		if ((pcsItem->m_eStatus == AGPDITEM_STATUS_INVENTORY ||	pcsItem->m_eStatus == AGPDITEM_STATUS_SUB_INVENTORY))
		{
			m_pcsAgcmItem->SendEquipInfo(pcsItem->m_lID, m_pcsAgcmCharacter->m_lSelfCID);
		}
	}
	return TRUE;
}