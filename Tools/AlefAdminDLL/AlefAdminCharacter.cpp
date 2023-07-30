// AlefAdminCharacter.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "AlefAdminDLL.h"
#include "AlefAdminCharacter.h"
#include ".\alefadmincharacter.h"
#include "AlefAdminManager.h"

#include "AuTimeStamp.h"
#include "AgcmUIChatting2.h"
#include "AgcmUIGuild.h"
#include "AgcmUICharacter.h"


// AlefAdminCharacter

// Edit Field String Table
#define EDIT_FIELD_HP						"HP"
#define EDIT_FIELD_MP						"MP"
//#define EDIT_FIELD_LEVEL					"Level"
//#define EDIT_FIELD_EXP						"Exp"
//#define EDIT_FIELD_MONEY_INV				"Inventory"
//#define EDIT_FIELD_MONEY_BANK				"Bank"
//#define EDIT_FIELD_POS						"Position"
//#define EDIT_FIELD_GUILD					"Guild Name"
//#define EDIT_FIELD_CRIMINAL_STATUS			"��������"
//#define EDIT_FIELD_MURDERER_POINT			"�Ǵ�����Ʈ"
//#define EDIT_FIELD_REMAINED_CRIMINAL_TIME	"���������ð�"
//#define EDIT_FIELD_REMAINED_MURDERER_TIME	"�����Ǵ�ð�"


IMPLEMENT_DYNCREATE(AlefAdminCharacter, CFormView)

AlefAdminCharacter::AlefAdminCharacter()
	: CFormView(AlefAdminCharacter::IDD)
{
	m_pcsCharGrid = NULL;

	m_pWCView = NULL;
	m_pCharMacroView = NULL;
	m_pCharQuest = NULL;

	InitCharData();
	InitCharDataSub();

    //xtAfxData.bXPMode = TRUE;
}

AlefAdminCharacter::~AlefAdminCharacter()
{
	if(m_pcsCharGrid)
	{
		m_pcsCharGrid->DestroyWindow();
		delete m_pcsCharGrid;
	}
}

void AlefAdminCharacter::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_S_CHAR_GRID_PLACE, m_csCharGridPlace);
	DDX_Control(pDX, IDC_TAB_CHAR, m_csSubTab);
}

BEGIN_MESSAGE_MAP(AlefAdminCharacter, CFormView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


// AlefAdminCharacter �����Դϴ�.

#ifdef _DEBUG
void AlefAdminCharacter::AssertValid() const
{
	CFormView::AssertValid();
}

void AlefAdminCharacter::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// AlefAdminCharacter �޽��� ó�����Դϴ�.

void AlefAdminCharacter::OnInitialUpdate()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CFormView::OnInitialUpdate();

	InitCharGrid();
	InitCharGridData();

	InitSubTab();
	CreateSubViews();
	AddSubTabViews();

	// Callback ������ش�.
	AgpmGuild* ppmGuild = ( AgpmGuild* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmGuild" );
	ppmGuild->SetCallbackCharData(CBReceiveCharGuildData, this);
}

BOOL AlefAdminCharacter::PreTranslateMessage(MSG* msg)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );
	return CWnd::PreTranslateMessage(msg);
}

// Sub �� ���� bzero
BOOL AlefAdminCharacter::InitCharData()
{
	memset(&m_stCharData.m_stBasic, 0, sizeof(m_stCharData.m_stBasic));
	memset(&m_stCharData.m_stStat, 0, sizeof(m_stCharData.m_stStat));
	memset(&m_stCharData.m_stStatus, 0, sizeof(m_stCharData.m_stStatus));
	memset(&m_stCharData.m_stCombat, 0, sizeof(m_stCharData.m_stCombat));
	memset(&m_stCharData.m_stMoney, 0, sizeof(m_stCharData.m_stMoney));

	return TRUE;
}

BOOL AlefAdminCharacter::InitCharDataSub()
{
	memset(&m_stCharData.m_stSub, 0, sizeof(m_stCharData.m_stSub));
	return TRUE;
}

BOOL AlefAdminCharacter::InitCharGrid()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CRect rc;
	m_csCharGridPlace.GetWindowRect(&rc);
	ScreenToClient(&rc);

	if(m_pcsCharGrid->Create(rc, this, IDC_PROPERTY_GRID_CHAR))
	{
		m_pcsCharGrid->SetFont(GetFont());
		
		// Create Basic Category
		CXTPPropertyGridItem* pBasic = m_pcsCharGrid->AddCategory(_T("Basic"));
		pBasic->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(_T("CID"), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ACCNAME), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(_T("TID"), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RACE), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CLASS), _T("")));
		pBasic->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SERVER), _T("")));

		pBasic->Expand();

		// Create Level Category
		CXTPPropertyGridItem* pLevel = m_pcsCharGrid->AddCategory(_T("Level"));
		pLevel->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL), _T("")));
		pLevel->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP), _T("")));

		pLevel->Expand();

		// Create Play-Sub Category
		CXTPPropertyGridItem* pPlaySub = m_pcsCharGrid->AddCategory(_T("Play Data"));
		pPlaySub->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LOGIN_TIME), _T("")));
		pPlaySub->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PLAY_TIME), _T("")));
		pPlaySub->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION), _T("")));
		pPlaySub->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_IP), _T("")));
		pPlaySub->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID), _T("")));

		// Status
		CXTPPropertyGridItem* pStatus = m_pcsCharGrid->AddCategory(_T("Status"));
		pStatus->AddChildItem(new CXTPSetValueGridItem(_T("HP"), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(_T("MaxHP"), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(_T("MP"), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(_T("MaxMP"), _T("")));

		pStatus->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CRIMINAL_STATUS), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MURDERER_POINT), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RCF), _T("")));
		pStatus->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RMF), _T("")));

		pStatus->Expand();

		// Status - 2
		CXTPPropertyGridItem* pStat = m_pcsCharGrid->AddCategory(_T("Stat"));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CON), _T("")));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_STR), _T("")));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WIS), _T("")));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INT), _T("")));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DEX), _T("")));
		pStat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA), _T("")));

		// Combat Data
		CXTPPropertyGridItem* pCombat = m_pcsCharGrid->AddCategory(_T("Combat Data"));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_NORMAL_ATTACK), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DEFENSE), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_BLOCK), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PHYSICAL_RES), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_AR), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ADDITIONAL_AR), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BLOCK), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EVADE), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ATTACK_SPEED), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MOVE_SPEED), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_MAGIC), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_FIRE), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_AIR), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_WATER), _T("")));
		pCombat->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_EARTH), _T("")));

		// Ghelld
		CXTPPropertyGridItem* pMoney = m_pcsCharGrid->AddCategory(_T("Money"));
		pMoney->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY), _T("")));
		pMoney->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK), _T("")));

		pMoney->Expand();

		// Etc
		CXTPPropertyGridItem* pEtc = m_pcsCharGrid->AddCategory(_T("Etc"));
		pEtc->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHAR_CREATION_DATE), _T("")));
		pEtc->AddChildItem(new CXTPSetValueGridItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LAST_IP), _T("")));

		// ���� �ٲ��ش�.
		m_pcsCharGrid->SetCustomColors(RGB(200, 200, 200), 0, RGB(187, 216, 178), RGB(253, 255, 253), 0);

		// Description �� ����.
		m_pcsCharGrid->ShowHelp(FALSE);
	}
	
	return TRUE;
}

BOOL AlefAdminCharacter::InitCharGridData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	INT32 lIndex = 0, lCount = 0;

	pCategory = m_pcsCharGrid->FindItem(_T("Basic"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Level"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Play Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Status"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Stat"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Combat Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Money"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Etc"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			lCount = pSubItemList->GetCount();
			for(lIndex = 0; lIndex < lCount; lIndex++)
				((CXTPSetValueGridItem*)pSubItemList->GetAt(lIndex))->SetValue(_T(""));
		}
	}

	return TRUE;
}

BOOL AlefAdminCharacter::InitSubTab()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	m_csSubTab.SendInitialUpdate(TRUE);
	m_csSubTab.Invalidate(FALSE);

	m_csSubTab.ModifyStyle(0L, WS_CLIPCHILDREN | WS_CLIPSIBLINGS);

	m_csSubTab.m_bXPBorder = true;

	return TRUE;
}

BOOL AlefAdminCharacter::CreateSubViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CRect rectDummy(0, 0, 0, 0);

	// Wanted Criminal View
	CCreateContext cc;
	ZeroMemory(&cc, sizeof(cc));
	m_pWCView = (AlefAdminWC*)RUNTIME_CLASS(AlefAdminWC)->CreateObject();
	if(!m_pWCView || !((CWnd *)m_pWCView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csSubTab, IDD_WC, &cc))
	{
		return FALSE;
	}

	// Char Macro View
	CCreateContext cc2;
	ZeroMemory(&cc2, sizeof(cc2));
	m_pCharMacroView = (AlefAdminCharMacro*)RUNTIME_CLASS(AlefAdminCharMacro)->CreateObject();
	if(!m_pCharMacroView || !((CWnd *)m_pCharMacroView)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csSubTab, IDD_CHARMACRO, &cc2))
	{
		return FALSE;
	}

	CCreateContext cc3;
	ZeroMemory(&cc3, sizeof(cc3));
	m_pCharQuest = (AlefAdminQuest*)RUNTIME_CLASS(AlefAdminQuest)->CreateObject();
	if(!m_pCharQuest || !((CWnd *)m_pCharQuest)->Create(
		NULL, NULL, WS_CHILD | WS_VISIBLE | LBS_NOINTEGRALHEIGHT, rectDummy, &m_csSubTab, IDD_QUEST, &cc2))
	{
		return FALSE;
	}

	m_pWCView->OnInitialUpdate();
	m_pCharMacroView->OnInitialUpdate();
	m_pCharQuest->OnInitialUpdate();

	return TRUE;
}

BOOL AlefAdminCharacter::AddSubTabViews()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!m_csSubTab.AddView(_T("Wanted"), m_pWCView, 0, 0))
	{
		return FALSE;
	}

	if(!m_csSubTab.AddView(_T("Macro"), m_pCharMacroView, 1, 1))
	{
		return FALSE;
	}

	if(!m_csSubTab.AddView(_T("Quest"), m_pCharQuest, 2, 2))
	{
		return FALSE;
	}

	m_csSubTab.SetActiveView(0);

	return TRUE;
}

BOOL AlefAdminCharacter::OnReceive(AgpdCharacter* pcsCharacter)
{
	if(!pcsCharacter)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pcsCharacter->m_szID) == FALSE)
		return FALSE;

	if(!SetCharData(pcsCharacter))
		return FALSE;

	return ShowCharData();
}

BOOL AlefAdminCharacter::OnReceive(stAgpdAdminCharData* pstCharData)
{
	if(!pstCharData)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstCharData->m_stBasic.m_szCharName) == FALSE)
		return FALSE;

	if(!SetCharData(pstCharData))
		return FALSE;

	return ShowCharData();
}

// 2005.05.06.
BOOL AlefAdminCharacter::OnReceiveBasic(stAgpdAdminCharDataBasic* pstBasic)
{
	if(!pstBasic)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstBasic->m_szCharName) == FALSE)
		return FALSE;

	if(!SetCharDataBasic(pstBasic))
		return FALSE;

	return ShowCharDataBasic();
}

// 2005.05.06.
BOOL AlefAdminCharacter::OnReceiveStat(stAgpdAdminCharDataStat* pstStat)
{

	if(!pstStat)
		return FALSE;

	if(!SetCharDataStat(pstStat))
		return FALSE;

	return ShowCharDataStat();
}

// 2005.05.06.
BOOL AlefAdminCharacter::OnReceiveStatus(stAgpdAdminCharDataStatus* pstStatus)
{
	if(!pstStatus)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	if(!SetCharDataStatus(pstStatus))
		return FALSE;

	return ShowCharDataStatus();
}

// 2005.05.06.
BOOL AlefAdminCharacter::OnReceiveMoney(stAgpdAdminCharDataMoney* pstMoney)
{
	if(!pstMoney)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstMoney->m_szCharName) == FALSE)
		return FALSE;

	if(!SetCharDataMoney(pstMoney))
		return FALSE;

	return ShowCharDataMoney();
}

// Sub �����͸� �����Ѵ�.
BOOL AlefAdminCharacter::OnReceiveSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	if(!pstCharDataSub)
		return FALSE;

	// ���� üũ
	if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		return FALSE;

	if(!SetCharDataSub(pstCharDataSub))
		return FALSE;

	return ShowCharDataSub();
}

BOOL AlefAdminCharacter::SetCharData(AgpdCharacter* pcsCharacter)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pcsCharacter)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(strcmp(pcsCharacter->m_szID, AlefAdminManager::Instance()->m_pMainDlg->m_szCharName) != 0)
		return FALSE;

	AgpmFactors* pcsAgpmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	if(!pcsAgpmFactors)
		return FALSE;

	AgpmCombat* pcsAgpmCombat = ( AgpmCombat* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCombat" );
	if(!pcsAgpmCombat)
		return FALSE;

	AgpmItem* pcsAgpmItem = ( AgpmItem* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmItem" );
	if(!pcsAgpmItem)
		return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
	AgpdCharacterTemplate* pcsTemplate = NULL;
	if(pcsCharacter->m_bIsTrasform)
		pcsTemplate	= ppmCharacter->GetCharacterTemplate(pcsCharacter->m_lOriginalTID);
	else
		pcsTemplate	= (AgpdCharacterTemplate*)pcsCharacter->m_pcsCharacterTemplate;

	if(!pcsTemplate)
		return FALSE;

	//AgpdFactor* pcsFactorResult = (AgpdFactor*)pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT);
	//if(!pcsFactorResult)
	//	pcsFactorResult	= &pcsCharacter->m_csFactor;	// DB ���� ���� ���� RESULT �� �����Ƿ� ���� ������ �Ѵ�. 2003.11.13.

	//if(!pcsFactorResult)
	//	return FALSE;

	// ���� ������ �ʱ�ȭ
	InitCharData();

	INT32 lLevel = ppmCharacter->GetLevel(pcsCharacter);
	
	// Basic
	m_stCharData.m_stBasic.m_lCID = pcsCharacter->m_lID;
	strcpy(m_stCharData.m_stBasic.m_szCharName, pcsCharacter->m_szID);
	m_stCharData.m_stBasic.m_lTID = pcsTemplate->m_lID;

	INT32 lValue = 0;

	// Stat
	// Con
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);
	m_stCharData.m_stStat.m_lCON = lValue;

	// Str
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);
	m_stCharData.m_stStat.m_lSTR = lValue;

	// Int
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);
	m_stCharData.m_stStat.m_lINT = lValue;

	// Dex
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);
	m_stCharData.m_stStat.m_lDEX = lValue;

	// Cha
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);
	m_stCharData.m_stStat.m_lCHA = lValue;

	// Wis
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);
	m_stCharData.m_stStat.m_lWIS = lValue;

	// Race
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
	m_stCharData.m_stStat.m_lRace = lValue;

	// Class
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);
	m_stCharData.m_stStat.m_lClass = lValue;


	// Status
	// HP
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_HP);
	m_stCharData.m_stStatus.m_lHP = lValue;

	// MP
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT, AGPD_FACTORS_CHARPOINT_TYPE_MP);
	m_stCharData.m_stStatus.m_lMP = lValue;

	// Max HP
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
	m_stCharData.m_stStatus.m_lMaxHP = lValue;

	// Max MP
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);
	m_stCharData.m_stStatus.m_lMaxMP = lValue;

	// Exp
	m_stCharData.m_stStatus.m_llExp	= pcsAgpmFactors->GetExp((AgpdFactor*)pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));

	// Level
	m_stCharData.m_stStatus.m_lLevel = lLevel;

	// Max Exp
	m_stCharData.m_stStatus.m_llMaxExp	= pcsAgpmFactors->GetMaxExp((AgpdFactor*)pcsAgpmFactors->GetFactor(&pcsCharacter->m_csFactor, AGPD_FACTORS_TYPE_RESULT));
	if(m_stCharData.m_stStatus.m_llMaxExp == 0)
	{
		m_stCharData.m_stStatus.m_llMaxExp	= pcsAgpmFactors->GetMaxExp(&pcsTemplate->m_csLevelFactor[lLevel]);
	}

	// Position
	m_stCharData.m_stStatus.m_stPos.x = pcsCharacter->m_stPos.x;
	m_stCharData.m_stStatus.m_stPos.y = pcsCharacter->m_stPos.y;
	m_stCharData.m_stStatus.m_stPos.z = pcsCharacter->m_stPos.z;

	// Skill Point
	m_stCharData.m_stStatus.m_lSkillPoint = ppmCharacter->GetSkillPoint(pcsCharacter);

	// Is Transform
	m_stCharData.m_stStatus.m_bIsTransform = pcsCharacter->m_bIsTrasform;

	// Original TID
	m_stCharData.m_stStatus.m_lOriginalTID = pcsCharacter->m_lOriginalTID;

	// CriminalPoint (Status)
	m_stCharData.m_stStatus.m_lCriminalPoint = pcsCharacter->m_unCriminalStatus;

	// MurdererPoint
	m_stCharData.m_stStatus.m_lMurdererPoint = ppmCharacter->GetMurdererPoint(pcsCharacter);

	// Remained Criminal Time
	m_stCharData.m_stStatus.m_lRemainedCriminalTime = ppmCharacter->GetRemainedCriminalTime(pcsCharacter);

	// Remained Murderer Time
	m_stCharData.m_stStatus.m_lRemainedMurdererTime = ppmCharacter->GetRemainedMurdererTime(pcsCharacter);



	// Combat Data
	//
	// Normal Attack Damage
	m_stCharData.m_stCombat.m_lNormalAttackDamage[0] = pcsAgpmCombat->CalcPhysicalAttack(pcsCharacter, NULL, pcsAgpmItem->IsEquipWeapon(pcsCharacter), TRUE, FALSE);
	m_stCharData.m_stCombat.m_lNormalAttackDamage[1] = pcsAgpmCombat->CalcBasePhysicalAttack(pcsCharacter, FALSE);

	// Defense
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_stCharData.m_stCombat.m_lDefense[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_POINT, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_stCharData.m_stCombat.m_lDefense[1] = lValue;

	// Skill Block
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK);
	m_stCharData.m_stCombat.m_lSkillBlock[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_SKILL_BLOCK);
	m_stCharData.m_stCombat.m_lSkillBlock[1] = lValue;

	// Physical Resistance
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_stCharData.m_stCombat.m_lPhysicalRes[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_DEFENSE, AGPD_FACTORS_DEFENSE_TYPE_DEFENSE_RATE, AGPD_FACTORS_ATTRIBUTE_TYPE_PHYSICAL);
	m_stCharData.m_stCombat.m_lPhysicalRes[1] = lValue;

	// Attack Rate
	m_stCharData.m_stCombat.m_lAttackRate[0] = pcsAgpmCombat->GetAR(pcsCharacter, pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor));
	m_stCharData.m_stCombat.m_lAttackRate[1] = pcsAgpmCombat->GetBaseAR(pcsCharacter, pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor));

	// Additional AR
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HIT_RATE);
	m_stCharData.m_stCombat.m_lAdditionalAR[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_HIT_RATE);
	m_stCharData.m_stCombat.m_lAdditionalAR[1] = lValue;

	// Block
	m_stCharData.m_stCombat.m_lBlock[0] = pcsAgpmCombat->GetDR(pcsCharacter, pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor));
	m_stCharData.m_stCombat.m_lBlock[1] = pcsAgpmCombat->GetBaseDR(pcsCharacter, pcsAgpmFactors->GetClass(&pcsCharacter->m_pcsCharacterTemplate->m_csFactor));

	// Additional Evade
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE);
	m_stCharData.m_stCombat.m_lAdditionalEvade[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_EVADE_RATE);
	m_stCharData.m_stCombat.m_lAdditionalEvade[1] = lValue;
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE);
	m_stCharData.m_stCombat.m_lAdditionalEvade[2] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_DODGE_RATE);
	m_stCharData.m_stCombat.m_lAdditionalEvade[3] = lValue;

	// AttackSpeed
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	m_stCharData.m_stCombat.m_lAttackSpeed[0] = lValue;

	// Movement
	pcsAgpmFactors->GetValue(&pcsCharacter->m_csFactor, &lValue, AGPD_FACTORS_TYPE_RESULT, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	m_stCharData.m_stCombat.m_lMoveSpeed[0] = lValue;
	pcsAgpmFactors->GetValue(&pcsTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_MOVEMENT_FAST);
	m_stCharData.m_stCombat.m_lMoveSpeed[1] = lValue;

	AgpdItem* pcsWeapon = pcsAgpmItem->GetEquipWeapon(pcsCharacter);
	if(pcsWeapon)
		pcsAgpmFactors->GetValue(&pcsWeapon->m_csFactor, &lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	else
		pcsAgpmFactors->GetValue(&pcsTemplate->m_csFactor, &lValue, AGPD_FACTORS_TYPE_ATTACK, AGPD_FACTORS_ATTACK_TYPE_SPEED);
	m_stCharData.m_stCombat.m_lAttackSpeed[1] = lValue;

	// Damage Magic
	m_stCharData.m_stCombat.m_lMagic[0] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, TRUE);
	m_stCharData.m_stCombat.m_lMagic[1] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC, TRUE, FALSE);

	// Damage Fire
	m_stCharData.m_stCombat.m_lFire[0] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, TRUE);
	m_stCharData.m_stCombat.m_lFire[1] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE, TRUE, FALSE);

	// Damage Air
	m_stCharData.m_stCombat.m_lAir[0] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, TRUE);
	m_stCharData.m_stCombat.m_lAir[1] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR, TRUE, FALSE);

	// Damage Water
	m_stCharData.m_stCombat.m_lWater[0] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, TRUE);
	m_stCharData.m_stCombat.m_lWater[1] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER, TRUE, FALSE);

	// Damage Earth
	m_stCharData.m_stCombat.m_lEarth[0] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, TRUE);
	m_stCharData.m_stCombat.m_lEarth[1] = pcsAgpmCombat->CalcFirstSpiritAttack(pcsCharacter, NULL, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH, TRUE, FALSE);

	// Defense Magic
	m_stCharData.m_stCombat.m_lMagic[2] = pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_MAGIC);

	// Defense Fire
	m_stCharData.m_stCombat.m_lFire[2] = pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_FIRE);

	// Defense Air
	m_stCharData.m_stCombat.m_lAir[2] = pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_AIR);

	// Defense Water
	m_stCharData.m_stCombat.m_lWater[2] = pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_WATER);

	// Defense Earth
	m_stCharData.m_stCombat.m_lEarth[2] = pcsAgpmCombat->CalcFirstSpiritDefense(pcsCharacter, AGPD_FACTORS_ATTRIBUTE_TYPE_EARTH);


	
	// Money
	// Inven Money
	ppmCharacter->GetMoney(pcsCharacter, &m_stCharData.m_stMoney.m_llInvenMoney);

	// Bank Money
	m_stCharData.m_stMoney.m_llBankMoney = ppmCharacter->GetBankMoney(pcsCharacter);


	return TRUE;
}

BOOL AlefAdminCharacter::SetCharData(stAgpdAdminCharData* pstCharData)
{
	if(!pstCharData)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstCharData->m_stBasic.m_szCharName) == FALSE)
		return FALSE;

	// Sub �� ���� �˴� ī���Ѵ�.
	memcpy(&m_stCharData.m_stBasic, &pstCharData->m_stBasic, sizeof(m_stCharData.m_stBasic));
	memcpy(&m_stCharData.m_stStat, &pstCharData->m_stStat, sizeof(m_stCharData.m_stStat));
	memcpy(&m_stCharData.m_stStatus, &pstCharData->m_stStatus, sizeof(m_stCharData.m_stStatus));
	memcpy(&m_stCharData.m_stCombat, &pstCharData->m_stCombat, sizeof(m_stCharData.m_stCombat));
	memcpy(&m_stCharData.m_stMoney, &pstCharData->m_stMoney, sizeof(m_stCharData.m_stMoney));

	return TRUE;
}

BOOL AlefAdminCharacter::SetCharDataBasic(stAgpdAdminCharDataBasic* pstBasic)
{
	if(!pstBasic)
		return FALSE;

	memcpy(&m_stCharData.m_stBasic, pstBasic, sizeof(m_stCharData.m_stBasic));
	return TRUE;
}

BOOL AlefAdminCharacter::SetCharDataStat(stAgpdAdminCharDataStat* pstStat)
{
	if(!pstStat)
		return FALSE;

	memcpy(&m_stCharData.m_stStat, pstStat, sizeof(m_stCharData.m_stStat));
	return TRUE;
}

BOOL AlefAdminCharacter::SetCharDataStatus(stAgpdAdminCharDataStatus* pstStatus)
{
	if(!pstStatus)
		return FALSE;

	memcpy(&m_stCharData.m_stStatus, pstStatus, sizeof(m_stCharData.m_stStatus));

	// Level �� TID �� Max ������ ��´�.
	AgpmFactors* pcsAgpmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	if(!pcsAgpmFactors)
		return TRUE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
	AgpdCharacterTemplate* pcsTemplate = ppmCharacter->GetCharacterTemplate(m_stCharData.m_stBasic.m_lTID);
	if(pcsTemplate && m_stCharData.m_stStatus.m_lLevel > 0 && m_stCharData.m_stStatus.m_lLevel <= AGPMCHAR_MAX_LEVEL)
	{
		pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[m_stCharData.m_stStatus.m_lLevel], &m_stCharData.m_stStatus.m_lMaxHP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_HP);
		pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[m_stCharData.m_stStatus.m_lLevel], &m_stCharData.m_stStatus.m_lMaxMP, AGPD_FACTORS_TYPE_CHAR_POINT_MAX, AGPD_FACTORS_CHARPOINTMAX_TYPE_MP);

		m_stCharData.m_stStatus.m_llMaxExp	= pcsAgpmFactors->GetMaxExp(&pcsTemplate->m_csLevelFactor[m_stCharData.m_stStatus.m_lLevel]);
	}

	return TRUE;
}

BOOL AlefAdminCharacter::SetCharDataMoney(stAgpdAdminCharDataMoney* pstMoney)
{
	if(!pstMoney)
		return FALSE;

	memcpy(&m_stCharData.m_stMoney, pstMoney, sizeof(m_stCharData.m_stMoney));
	return TRUE;
}

BOOL AlefAdminCharacter::SetCharDataSub(stAgpdAdminCharDataSub* pstCharDataSub)
{
	if(!pstCharDataSub)
		return FALSE;

	// �˻��� ���� ����°��� Ȯ��
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pstCharDataSub->m_szCharName) == FALSE)
		return FALSE;

	memcpy(&m_stCharData.m_stSub, pstCharDataSub, sizeof(m_stCharData.m_stSub));

	return TRUE;
}

BOOL AlefAdminCharacter::ShowCharData()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	// ��� ������ ���� �Ǿ� �ִ� �� ������ ������.
	if(strlen(m_stCharData.m_stBasic.m_szCharName) == 0)
		return FALSE;

	AgpmFactors* pcsAgpmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	if(!pcsAgpmFactors)
		return FALSE;

	AgpmCombat* pcsAgpmCombat = ( AgpmCombat* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCombat" );
	if(!pcsAgpmCombat)
		return FALSE;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
	AgpdCharacterTemplate* pcsTemplate = NULL;
	if(m_stCharData.m_stStatus.m_bIsTransform)
		pcsTemplate = ppmCharacter->GetCharacterTemplate(m_stCharData.m_stStatus.m_lOriginalTID);
	else
		pcsTemplate = ppmCharacter->GetCharacterTemplate(m_stCharData.m_stBasic.m_lTID);

	if(!pcsTemplate)
		return FALSE;

	INT32 lLevel = m_stCharData.m_stStatus.m_lLevel;
	INT32 lOriginalValue = 0;

	// ���� �ִ��� �����
	InitCharGridData();

	// �ܿ���ų����Ʈ, TID �� ���� ��ų ����Ʈ�� �����Ѵ�.
	AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->SetSkillPoint(m_stCharData.m_stStatus.m_lSkillPoint);
	AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->SetAllSkillByTID(m_stCharData.m_stBasic.m_lTID);

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;
	
	pCategory = m_pcsCharGrid->FindItem(_T("Basic"));
	if(pCategory)	// Basic Category
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stBasic.m_szCharName;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("CID"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stBasic.m_lCID);
				pSubItem->SetValue(szValue);
			}

			//pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Account Name"));
			//if(pSubItem)
			//{
			//	szValue = m_stCharData.m_stSub.m_szAccName;
			//	pSubItem->SetValue(szValue);
			//}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("TID"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stBasic.m_lTID);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RACE));
			if(pSubItem)
			{
				szValue = pcsAgpmFactors->GetCharacterRaceName(m_stCharData.m_stStat.m_lRace);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CLASS));
			if(pSubItem)
			{
				szValue = pcsAgpmFactors->GetCharacterClassName(m_stCharData.m_stStat.m_lRace, m_stCharData.m_stStat.m_lClass);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SERVER));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stBasic.m_szServerName;
				pSubItem->SetValue(szValue);
			}
		}	// if(pSubItemList)
	}	// if(pCategory)

	pCategory = m_pcsCharGrid->FindItem(_T("Level"));
	if(pCategory)	// Level
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lLevel);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP));
			if(pSubItem)
			{
				if(m_stCharData.m_stStatus.m_llMaxExp > 0)
					szValue.Format("%.2f(%%), %I64d / %I64d",
									(FLOAT)((FLOAT)m_stCharData.m_stStatus.m_llExp * 100.0f / (FLOAT)m_stCharData.m_stStatus.m_llMaxExp),
									m_stCharData.m_stStatus.m_llExp, m_stCharData.m_stStatus.m_llMaxExp);
				else
					szValue.Format("%I64d (Can't find Max Exp)", m_stCharData.m_stStatus.m_llExp);

				pSubItem->SetValue(szValue);
			}
		}	// if(pSubItemList)
	}	// if(pCategory)

	pCategory = m_pcsCharGrid->FindItem(_T("Play Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LOGIN_TIME));
			if(pSubItem)
			{
				TCHAR szTmp[255];
				memset(szTmp, 0, sizeof(szTmp));

				AlefAdminManager::Instance()->ConvertTimeStampToString(m_stCharData.m_stSub.m_ulConnectedTimeStamp, szTmp);
				szValue = szTmp;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PLAY_TIME));
			if(pSubItem)
			{
				if(m_stCharData.m_stSub.m_ulConnectedTimeStamp != 0)
				{
					szValue.Format("%d %s", (INT32)((AuTimeStamp::GetCurrentTimeStamp() - m_stCharData.m_stSub.m_ulConnectedTimeStamp) / 60) + 1,
											AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				}
				else
					szValue = _T("");
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION));
			if(pSubItem)
			{
				szValue.Format("%ld, %ld, %ld",
									(INT32)m_stCharData.m_stStatus.m_stPos.x,
									(INT32)m_stCharData.m_stStatus.m_stPos.y,
									(INT32)m_stCharData.m_stStatus.m_stPos.z);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_IP));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stSub.m_szIP;
				pSubItem->SetValue(szValue);
			}

			//pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("Guild Name"));
			//if(pSubItem)
			//{
			//	szValue.Format("%d", m_stCharData.m_stSub.m_lPlayTime);
			//	pSubItem->SetValue(szValue);
			//}
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Status"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("HP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lHP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MaxHP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMaxHP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MaxMP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMaxMP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CRIMINAL_STATUS));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lCriminalPoint);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MURDERER_POINT));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMurdererPoint);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RCF));
			if(pSubItem)
			{
				szValue.Format("%d %s", (INT32)((FLOAT)m_stCharData.m_stStatus.m_lRemainedCriminalTime / 60.0f + 0.999f),
										AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RMF));
			if(pSubItem)
			{
				szValue.Format("%d %s", (INT32)((FLOAT)m_stCharData.m_stStatus.m_lRemainedMurdererTime / 60.0f + 0.999f),
										AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				pSubItem->SetValue(szValue);
			}

		}	// pSubItemList
	}	// pCategory

	pCategory = m_pcsCharGrid->FindItem(_T("Stat"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CON));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CON);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lCON, lOriginalValue);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_STR));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_STR);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lSTR, lOriginalValue);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WIS));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_WIS);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lWIS, lOriginalValue);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INT));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_INT);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lINT, lOriginalValue);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DEX));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_DEX);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lDEX, lOriginalValue);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA));
			if(pSubItem)
			{
				pcsAgpmFactors->GetValue(&pcsTemplate->m_csLevelFactor[lLevel], &lOriginalValue, AGPD_FACTORS_TYPE_CHAR_STATUS, AGPD_FACTORS_CHARSTATUS_TYPE_CHA);

				SetFormatType1(szValue, m_stCharData.m_stStat.m_lCHA, lOriginalValue);
				pSubItem->SetValue(szValue);
			}
		}	// pSubItemList
	}	// pCategory

	pCategory = m_pcsCharGrid->FindItem(_T("Combat Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_NORMAL_ATTACK));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lNormalAttackDamage[0], m_stCharData.m_stCombat.m_lNormalAttackDamage[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DEFENSE));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lDefense[0], m_stCharData.m_stCombat.m_lDefense[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SKILL_BLOCK));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lSkillBlock[0], m_stCharData.m_stCombat.m_lSkillBlock[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PHYSICAL_RES));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lPhysicalRes[0], m_stCharData.m_stCombat.m_lPhysicalRes[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_AR));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lAttackRate[0], m_stCharData.m_stCombat.m_lAttackRate[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ADDITIONAL_AR));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lAdditionalAR[0], m_stCharData.m_stCombat.m_lAdditionalAR[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BLOCK));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lBlock[0], m_stCharData.m_stCombat.m_lBlock[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EVADE));
			if(pSubItem)
			{
				SetFormatType1(szValue, m_stCharData.m_stCombat.m_lAdditionalEvade[0], m_stCharData.m_stCombat.m_lAdditionalEvade[2]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ATTACK_SPEED));
			if(pSubItem)
			{
				SetFormatType2(szValue, m_stCharData.m_stCombat.m_lAttackSpeed[0], m_stCharData.m_stCombat.m_lAttackSpeed[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MOVE_SPEED));
			if(pSubItem)
			{
				SetFormatType2(szValue, m_stCharData.m_stCombat.m_lMoveSpeed[0], m_stCharData.m_stCombat.m_lMoveSpeed[1]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_MAGIC));
			if(pSubItem)
			{
				SetFormatType3(szValue, m_stCharData.m_stCombat.m_lMagic[0], m_stCharData.m_stCombat.m_lMagic[1], m_stCharData.m_stCombat.m_lMagic[2]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_FIRE));
			if(pSubItem)
			{
				SetFormatType3(szValue, m_stCharData.m_stCombat.m_lFire[0], m_stCharData.m_stCombat.m_lFire[1], m_stCharData.m_stCombat.m_lFire[2]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_AIR));
			if(pSubItem)
			{
				SetFormatType3(szValue, m_stCharData.m_stCombat.m_lAir[0], m_stCharData.m_stCombat.m_lAir[1], m_stCharData.m_stCombat.m_lAir[2]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_WATER));
			if(pSubItem)
			{
				SetFormatType3(szValue, m_stCharData.m_stCombat.m_lWater[0], m_stCharData.m_stCombat.m_lWater[1], m_stCharData.m_stCombat.m_lWater[2]);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_ID_CHARACTER_ATTR_EARTH));
			if(pSubItem)
			{
				SetFormatType3(szValue, m_stCharData.m_stCombat.m_lEarth[0], m_stCharData.m_stCombat.m_lEarth[1], m_stCharData.m_stCombat.m_lEarth[2]);
				pSubItem->SetValue(szValue);
			}
		}	// pSubItemList
	}	// Category

	pCategory = m_pcsCharGrid->FindItem(_T("Money"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY));
			if(pSubItem)
			{
				szValue.Format("%I64d", m_stCharData.m_stMoney.m_llInvenMoney);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK));
			if(pSubItem)
			{
				szValue.Format("%I64d", m_stCharData.m_stMoney.m_llBankMoney);
				pSubItem->SetValue(szValue);
			}
		}
	}

	// Etc �� Sub �����͸� ���� �� �Ѵ�.
	//
	//pCategory = m_pcsCharGrid->FindItem(_T("Etc"));
	//if(pCategory)
	//{
	//	pSubItemList = pCategory->GetChilds();
	//	if(pSubItemList)
	//	{
	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("�������ӽð�"));
	//		if(pSubItem)
	//		{
	//			szValue.Format("%d", m_stCharData.m_stSub.);
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("ĳ���ͻ�������"));
	//		if(pSubItem)
	//		{
	//			szValue = _T("�޷�~");
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("��������ð�"));
	//		if(pSubItem)
	//		{
	//			szValue = m_stCharData.m_stSub.m_szLastLogOut;
	//			pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("����������ġ"));
	//		if(pSubItem)
	//		{
	//			//szValue = m_stCharData.m_stSub.m_szLastLogOut;
	//			//pSubItem->SetValue(szValue);
	//		}

	//		pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("����IP"));
	//		if(pSubItem)
	//		{
	//			szValue = m_stCharData.m_stSub.m_szLastIP;
	//			pSubItem->SetValue(szValue);
	//		}
	//	}
	//}

	return TRUE;
}

// 2005.05.06.
BOOL AlefAdminCharacter::ShowCharDataBasic()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	AgpmFactors* pcsAgpmFactors = ( AgpmFactors* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmFactors" );
	if(!pcsAgpmFactors)
		return FALSE;

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;

	AgpmCharacter* ppmCharacter = ( AgpmCharacter* )AlefAdminManager::Instance()->GetAdminModule()->GetModule( "AgpmCharacter" );
	AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->SetAllSkillByTID(m_stCharData.m_stBasic.m_lTID);
	
	pCategory = m_pcsCharGrid->FindItem(_T("Basic"));
	if(pCategory)	// Basic Category
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHARNAME));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stBasic.m_szCharName;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("CID"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stBasic.m_lCID);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("TID"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stBasic.m_lTID);
				pSubItem->SetValue(szValue);
			}

			// 2005.05.18. steeple �ӽö���~
			AgpdCharacterTemplate* pcsCharacterTemplate = ppmCharacter->GetCharacterTemplate(m_stCharData.m_stBasic.m_lTID);
			if(pcsCharacterTemplate)
			{
				INT32 lRace = 0, lClass = 0;
				pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lRace,	AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_RACE);
				pcsAgpmFactors->GetValue(&pcsCharacterTemplate->m_csFactor, &lClass, AGPD_FACTORS_TYPE_CHAR_TYPE, AGPD_FACTORS_CHARTYPE_TYPE_CLASS);

				m_stCharData.m_stStat.m_lRace = lRace;
				m_stCharData.m_stStat.m_lClass = lClass;
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RACE));
			if(pSubItem)
			{
				szValue = pcsAgpmFactors->GetCharacterRaceName(m_stCharData.m_stStat.m_lRace);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CLASS));
			if(pSubItem)
			{
				szValue = pcsAgpmFactors->GetCharacterClassName(m_stCharData.m_stStat.m_lRace, m_stCharData.m_stStat.m_lClass);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SERVER));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stBasic.m_szServerName;
				pSubItem->SetValue(szValue);
			}
		}
	}

	return TRUE;
}

// 2005.05.06.
BOOL AlefAdminCharacter::ShowCharDataStat()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;

	pCategory = m_pcsCharGrid->FindItem(_T("Stat"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CON));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lCON);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_STR));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lSTR);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INT));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lINT);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_WIS));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lWIS);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_DEX));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lDEX);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStat.m_lCHA);
				pSubItem->SetValue(szValue);
			}
		}
	}
	return TRUE;
}

// 2005.05.06.
BOOL AlefAdminCharacter::ShowCharDataStatus()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;

	AlefAdminManager::Instance()->m_pMainDlg->m_pSkillView->SetSkillPoint(m_stCharData.m_stStatus.m_lSkillPoint);
	
	pCategory = m_pcsCharGrid->FindItem(_T("Level"));
	if(pCategory)	// Level
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lLevel);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP));
			if(pSubItem)
			{
				if(m_stCharData.m_stStatus.m_llMaxExp > 0)
					szValue.Format("%.2f(%%), %I64d / %I64d (%s)",
									(double)(m_stCharData.m_stStatus.m_llExp * 100.0 / m_stCharData.m_stStatus.m_llMaxExp),
									m_stCharData.m_stStatus.m_llExp, m_stCharData.m_stStatus.m_llMaxExp,
									AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EDIT_EXP));
				else
					szValue.Format("%I64d (Can't Find Max Exp)", m_stCharData.m_stStatus.m_llExp);

				pSubItem->SetValue(szValue);
			}
		}	// if(pSubItemList)
	}	// if(pCategory)

	pCategory = m_pcsCharGrid->FindItem(_T("Play Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LOGIN_TIME));
			if(pSubItem)
			{
				TCHAR szTmp[255];
				memset(szTmp, 0, sizeof(szTmp));

				AlefAdminManager::Instance()->ConvertTimeStampToString(m_stCharData.m_stSub.m_ulConnectedTimeStamp, szTmp);
				szValue = szTmp;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PLAY_TIME));
			if(pSubItem)
			{
				if(m_stCharData.m_stSub.m_ulConnectedTimeStamp != 0)
				{
					szValue.Format("%d %s", (INT32)((AuTimeStamp::GetCurrentTimeStamp() - m_stCharData.m_stSub.m_ulConnectedTimeStamp) / 60) + 1,
											AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				}
				else
					szValue = _T("");
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_IP));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stSub.m_szIP;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION));
			if(pSubItem)
			{
				szValue.Format("%ld, %ld, %ld",
									(INT32)m_stCharData.m_stStatus.m_stPos.x,
									(INT32)m_stCharData.m_stStatus.m_stPos.y,
									(INT32)m_stCharData.m_stStatus.m_stPos.z);
				pSubItem->SetValue(szValue);
			}
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Status"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("HP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lHP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MaxHP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMaxHP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMP);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(_T("MaxMP"));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMaxMP);
				pSubItem->SetValue(szValue);

			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CRIMINAL_STATUS));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lCriminalPoint);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MURDERER_POINT));
			if(pSubItem)
			{
				szValue.Format("%d", m_stCharData.m_stStatus.m_lMurdererPoint);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RCF));
			if(pSubItem)
			{
				szValue.Format("%d %s", (INT32)((FLOAT)m_stCharData.m_stStatus.m_lRemainedCriminalTime / 60.0f + 0.999f),
										AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RMF));
			if(pSubItem)
			{
				szValue.Format("%d %s", (INT32)((FLOAT)m_stCharData.m_stStatus.m_lRemainedMurdererTime / 60.0f + 0.999f),
										AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				pSubItem->SetValue(szValue);
			}
		}	// pSubItemList
	}	// pCategory

	//m_pSkillView->SetSkillPoint(m_stCharData.m_stStatus.m_lSkillPoint);

	return TRUE;
}

BOOL AlefAdminCharacter::ShowCharDataMoney()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;
	
	pCategory = m_pcsCharGrid->FindItem(_T("Money"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY));
			if(pSubItem)
			{
				szValue.Format("%I64d", m_stCharData.m_stMoney.m_llInvenMoney);
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK));
			if(pSubItem)
			{
				szValue.Format("%I64d", m_stCharData.m_stMoney.m_llBankMoney);
				pSubItem->SetValue(szValue);
			}
		}
	}

	return TRUE;
}

BOOL AlefAdminCharacter::ShowCharDataSub()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CXTPPropertyGridItem* pCategory = NULL;
	CXTPSetValueGridItem* pSubItem = NULL;
	CXTPPropertyGridItems* pSubItemList = NULL;
	CString szValue;
	
	pCategory = m_pcsCharGrid->FindItem(_T("Basic"));
	if(pCategory)	// Basic Category
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ACCNAME));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stSub.m_szAccName;
				pSubItem->SetValue(szValue);
			}
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Play Data"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LOGIN_TIME));
			if(pSubItem)
			{
				TCHAR szTmp[255];
				memset(szTmp, 0, sizeof(szTmp));

				AlefAdminManager::Instance()->ConvertTimeStampToString(m_stCharData.m_stSub.m_ulConnectedTimeStamp, szTmp);
				szValue = szTmp;
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_PLAY_TIME));
			if(pSubItem)
			{
				if(m_stCharData.m_stSub.m_ulConnectedTimeStamp != 0)
				{
					szValue.Format("%d %s", (INT32)((AuTimeStamp::GetCurrentTimeStamp() - m_stCharData.m_stSub.m_ulConnectedTimeStamp) / 60) + 1,
											AlefAdminManager::Instance()->GetStringManager().GetUIMessage(UI_MESSAGE_GUILD_TIME_UNIT_MINUTE));
				}
				else
					szValue = _T("");
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_IP));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stSub.m_szIP;
				pSubItem->SetValue(szValue);
			}
		}
	}

	pCategory = m_pcsCharGrid->FindItem(_T("Etc"));
	if(pCategory)
	{
		pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHAR_CREATION_DATE));
			if(pSubItem)
			{
				szValue = _T("");
				pSubItem->SetValue(szValue);
			}

			pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LAST_IP));
			if(pSubItem)
			{
				szValue = m_stCharData.m_stSub.m_szLastIP;
				pSubItem->SetValue(szValue);
			}
		}
	}

	return TRUE;
}

BOOL AlefAdminCharacter::SetFormatType1(CString& szValue, INT32 lCurrentValue, INT32 lOriginalValue)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(lCurrentValue == lOriginalValue)
		szValue.Format("%d", lCurrentValue);
	else if(lCurrentValue > lOriginalValue)
		szValue.Format("%d (+%d)", lCurrentValue, lCurrentValue - lOriginalValue);
	else
		szValue.Format("%d (-%d)", lCurrentValue, lOriginalValue - lCurrentValue);

	return TRUE;
}

BOOL AlefAdminCharacter::SetFormatType2(CString& szValue, INT32 lCurrentValue, INT32 lOriginalValue)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(lCurrentValue == lOriginalValue)
	{
		if(lCurrentValue == 0)
			szValue.Format("0%%");
		else
			szValue.Format("100%%");
	}
	else
	{
		if(lOriginalValue == 0)
			lOriginalValue = 100;

		szValue.Format("%d%%", (INT32)(lCurrentValue / (float)lOriginalValue * 100));
	}

	return TRUE;
}

BOOL AlefAdminCharacter::SetFormatType3(CString& szValue, INT32 lValue1, INT32 lValue2, INT32 lValue3)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	szValue.Format("%s : %d ~ %d,   %s : %d%%", AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_ATTACK_POWER),
												lValue1, lValue2,
												AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RES_POWER),
												lValue3);
	return TRUE;
}

BOOL AlefAdminCharacter::EditChar(INT32 lCID, LPCTSTR szCharName, LPCTSTR szEditField, LPCTSTR szNewValue)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szCharName)
		return FALSE;

	// 2005.06.01. steeple
	// ���������� ���� ���� ���� ó���Ѵ�.
	if(lCID == 0)
		return EditCharToDB(szCharName, szEditField, szNewValue);

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lCID = lCID;
	strcpy(stCharEdit.m_szCharName, szCharName);

	BOOL bEditEnableField = FALSE;

	// szEdit Fileld �� ������ �м��ؾ� �ȴ�. -_-;;
	if(strcmp(szEditField, EDIT_FIELD_HP) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_HP;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, EDIT_FIELD_MP) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_MP;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_LEVEL;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP)) == 0)
	{
		float fExpRate = 0.0f;
		INT64 nExpCurrent = 0;
		INT64 nExpMax = 0;

		CHAR strFormat[ 32 ] = "%f(%%), %i64d / %i64d";
		sscanf( szNewValue, strFormat, &fExpRate, &nExpCurrent, &nExpMax );

		if( nExpCurrent != m_stCharData.m_stStatus.m_llExp )
		{
			stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_EXP;
			stCharEdit.m_llNewValueExp = nExpCurrent - m_stCharData.m_stStatus.m_llExp;

			bEditEnableField = TRUE;
		}

		//// ��üũ
		//INT32 lIndex = 0;
		//for(lIndex; lIndex < strlen(szNewValue); lIndex++)
		//{
		//	if(szNewValue[lIndex] == '%')
		//		break;
		//}

		//INT64 llNewExpValue = 0;
		//if(lIndex > 0 && lIndex < strlen(szNewValue) && lIndex < 50)
		//{
		//	CHAR szTmp[50];
		//	memcpy(szTmp, szNewValue, lIndex);
		//	INT32 lNewExp = atoi(szTmp);
		//	if(lNewExp >= 0 && lNewExp <= 99)
		//		llNewExpValue = (INT64)(m_stCharData.m_stStatus.m_llMaxExp * (double)lNewExp / 100.0f);
		//}
		//else
		//{
		//	// �׳� ���ڷ� ������.
		//	llNewExpValue = _atoi64(szNewValue);
		//}

		//if(llNewExpValue)
		//{
		//	// �������� ���� ������ش�. ���� ���� ���� ���� �����ϸ� m_lNewValueINT �� ������ ���� ����
		//	stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_EXP;
		//	stCharEdit.m_llNewValueExp = llNewExpValue - m_stCharData.m_stStatus.m_llExp;

		//	bEditEnableField = TRUE;
		//}
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_MONEY_INV;
		stCharEdit.m_llNewValueINT64 = _atoi64(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_MONEY_BANK;
		stCharEdit.m_llNewValueINT64 = _atoi64(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CRIMINAL_STATUS)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_CRIMINAL_POINT;
		stCharEdit.m_lNewValueINT = atoi(szNewValue) > 0 ? AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED : AGPDCHAR_CRIMINAL_STATUS_INNOCENT;

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MURDERER_POINT)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_MURDERER_POINT;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RCF)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_CRIMINAL_TIME;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RMF)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_MURDERER_TIME;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_CHARISMA_POINT;
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		bEditEnableField = TRUE;
	}


	// ����üũ 2006.01.11. steeple
	if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION)) == 0)
	{
		if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		{
			AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
			bEditEnableField = FALSE;
		}
	}
	else
	{
		if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
		{
			AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
			bEditEnableField = FALSE;
		}
	}

	if(bEditEnableField)
	{
		CString szFormat = _T("");
		szFormat.LoadString(IDS_EDIT_CONFIRM);

		if(::AfxMessageBox((LPCTSTR)szFormat, MB_YESNO) == IDYES)
			AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit);
		else
		{
			ShowCharData();
			ShowCharDataSub();
		}
	}
	else
	{
		ShowCharData();
		ShowCharDataSub();
	}

	return TRUE;
}

// 2005.06.01. steeple
// ���������� ���� ���� ó���� ���� �����͸� ��°�� �� ������ �Ѵ�.
// ��� � �׸��� ����Ʈ�ϴ� �� üũ�ؼ� ����Ʈ ������ �׸��̶��
// ������Ʈ �Ǵ� ��� ������ (�ٸ� �� �ȵ����� ������ �ϳ��� �Ǳ� ������ �� ������ �Ѵ�.) �� �˴� �����־�� �Ѵ�.
BOOL AlefAdminCharacter::EditCharToDB(LPCTSTR szCharName, LPCTSTR szEditField, LPCTSTR szNewValue)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szCharName || !szEditField || !szNewValue)
		return FALSE;

	stAgpdAdminCharEdit stCharEdit;
	memset(&stCharEdit, 0, sizeof(stCharEdit));

	stCharEdit.m_lCID = m_stCharData.m_stBasic.m_lTID;		// CID �� TID
	strcpy(stCharEdit.m_szCharName, szCharName);

	if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL)) == 0 ||
		strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP)) == 0 ||
		strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY)) == 0 ||
		strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_DB;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_BANK_MONEY_DB;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA)) == 0)
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_CHARISMA_POINT_DB;
	}
	else
	{
		stCharEdit.m_lEditField = AGPMADMIN_CHAREDIT_DB2;
	}



	// �����͸� �� �����Ѵ�.
	// �ΰ��� ��������.  2006.02.07. steeple
	if(stCharEdit.m_lEditField == AGPMADMIN_CHAREDIT_DB)
	{
		stCharEdit.m_lNewValueINT = m_stCharData.m_stStatus.m_lLevel;
		stCharEdit.m_llNewValueExp = m_stCharData.m_stStatus.m_llExp;
		sprintf(stCharEdit.m_szNewValueChar, "%ld,%ld,%ld", 
												(INT32)m_stCharData.m_stStatus.m_stPos.x,
												(INT32)m_stCharData.m_stStatus.m_stPos.y,
												(INT32)m_stCharData.m_stStatus.m_stPos.z);
		stCharEdit.m_lNewValueCharLength = strlen(stCharEdit.m_szNewValueChar);
		stCharEdit.m_llNewValueINT64 = m_stCharData.m_stMoney.m_llInvenMoney;
	}
	else if(stCharEdit.m_lEditField == AGPMADMIN_CHAREDIT_DB2)
	{
		stCharEdit.m_lNewValueINT = m_stCharData.m_stStatus.m_lCriminalPoint;
		stCharEdit.m_lNewValueFloat = m_stCharData.m_stStatus.m_lMurdererPoint;
		stCharEdit.m_llNewValueINT64 = (INT32)((FLOAT)m_stCharData.m_stStatus.m_lRemainedCriminalTime / 60.0f + 0.999f);
		stCharEdit.m_lNewValueCharLength = (INT16)((FLOAT)m_stCharData.m_stStatus.m_lRemainedMurdererTime / 60.0f + 0.999f);
	}
	else if(stCharEdit.m_lEditField == AGPMADMIN_CHAREDIT_BANK_MONEY_DB)
	{
		stCharEdit.m_lNewValueINT = m_stCharData.m_stStatus.m_lLevel;
		sprintf(stCharEdit.m_szCharName, "%s", m_stCharData.m_stSub.m_szAccName);
		sprintf(stCharEdit.m_szNewValueChar, "%s", m_stCharData.m_stBasic.m_szServerName);
		stCharEdit.m_lNewValueCharLength = strlen(stCharEdit.m_szNewValueChar);
		stCharEdit.m_llNewValueINT64 = m_stCharData.m_stMoney.m_llBankMoney;
	}
	else if(stCharEdit.m_lEditField == AGPMADMIN_CHAREDIT_CHARISMA_POINT_DB)
	{
		sprintf(stCharEdit.m_szCharName, "%s", m_stCharData.m_stSub.m_szCharName);
	}


	// ����Ʈ �� �� �ִ� �׸����� üũ�Ѵ�.
	BOOL bEditEnableField = FALSE;
	if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_LEVEL)) == 0)
	{
		stCharEdit.m_lNewValueINT = atoi(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_EXP)) == 0)
	{
		stCharEdit.m_llNewValueExp = _atoi64(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_INVENTORY)) == 0)
	{
		stCharEdit.m_llNewValueINT64 = _atoi64(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION)) == 0)
	{
		strncpy(stCharEdit.m_szNewValueChar, szNewValue, AGPMADMIN_MAX_CHAT_STRING);
		stCharEdit.m_lNewValueCharLength = strlen(stCharEdit.m_szNewValueChar);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CRIMINAL_STATUS)) == 0)
	{
		stCharEdit.m_lNewValueINT = atoi(szNewValue);

		if(stCharEdit.m_lNewValueINT != 0)
			stCharEdit.m_lNewValueINT = AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED;
		else
		{
			stCharEdit.m_lNewValueINT = AGPDCHAR_CRIMINAL_STATUS_INNOCENT;
			stCharEdit.m_llNewValueINT64 = 0L;	// ���˷� �ٲ�� �Ŷ�� �ð��� 0 ���� ���ش�.
		}

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_MURDERER_POINT)) == 0)
	{
		stCharEdit.m_lNewValueFloat = atoi(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RCF)) == 0)
	{
		stCharEdit.m_llNewValueINT64 = atoi(szNewValue);

		// 0 �϶��� �������� �ƴ� ������ ����� �Ѵ�.
		if(stCharEdit.m_llNewValueINT64 == 0L)
			stCharEdit.m_lNewValueINT = AGPDCHAR_CRIMINAL_STATUS_INNOCENT;
		else
			stCharEdit.m_lNewValueINT = AGPDCHAR_CRIMINAL_STATUS_CRIMINAL_FLAGGED;

		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_RMF)) == 0)
	{
		stCharEdit.m_lNewValueCharLength = atoi(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_BANK)) == 0)
	{	
		stCharEdit.m_llNewValueINT64 = _atoi64(szNewValue);
		bEditEnableField = TRUE;
	}
	else if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_CHA)) == 0)
	{
		stCharEdit.m_lNewValueINT = _atoi64(szNewValue);
		bEditEnableField = TRUE;
	}



	// ����üũ 2006.01.11. steeple
	if(strcmp(szEditField, (LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_POSITION)) == 0)
	{
		if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_2)
		{
			AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
			bEditEnableField = FALSE;
		}
	}
	else
	{
		if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
		{
			AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
			bEditEnableField = FALSE;
		}
	}

	if(bEditEnableField)
	{
		CString szFormat = _T("");
		szFormat.LoadString(IDS_EDIT_DB_CHAR);

		if(::AfxMessageBox((LPCTSTR)szFormat, MB_YESNO) == IDYES)
		{
			AlefAdminManager::Instance()->GetAdminData()->SendCharacterModification(&stCharEdit);

			szFormat.LoadString(IDS_EDIT_DB_REFRESH);
			::AfxMessageBox((LPCTSTR)szFormat, MB_OK);
		}
		else
		{
			ShowCharData();
			ShowCharDataSub();
		}
	}
	else
	{
		// �ٽ� �ѷ��ְ� return
		ShowCharData();
		ShowCharDataSub();
	}

	return TRUE;
}

BOOL AlefAdminCharacter::OnReceiveEditResult(stAgpdAdminCharEdit* pstCharEdit)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pstCharEdit)
		return FALSE;

	// ���� ĳ������ ������ Ȯ��
	if(strcmp(m_stCharData.m_stBasic.m_szCharName, pstCharEdit->m_szCharName) != 0)
		return FALSE;

	CString szMessage = _T("");
	CString szFormat = _T("");

	switch(pstCharEdit->m_lEditField)
	{
		case AGPMADMIN_CHAREDIT_HP:
			if(pstCharEdit->m_lEditResult != 0)
			{
				szFormat.LoadString(IDS_EDIT_RESULT_1);
				szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, _T("HP"), pstCharEdit->m_lNewValueINT);
			}
			else
			{
				szFormat.LoadString(IDS_EDIT_RESULT_FAILURE);
				szMessage = szFormat;
			}

			break;

		case AGPMADMIN_CHAREDIT_MP:
			if(pstCharEdit->m_lEditResult != 0)
			{
				szFormat.LoadString(IDS_EDIT_RESULT_1);
				szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, _T("MP"), pstCharEdit->m_lNewValueINT);
			}
			else
			{
				szFormat.LoadString(IDS_EDIT_RESULT_FAILURE);
				szMessage = szFormat;
			}

			break;

		case AGPMADMIN_CHAREDIT_LEVEL:
			if(pstCharEdit->m_lEditResult != 0)
			{
				szFormat.LoadString(IDS_EDIT_RESULT_1);
				szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, _T("Level"), pstCharEdit->m_lNewValueINT);
			}
			//else
			//{
			//	szFormat.LoadString(IDS_EDIT_RESULT_FAILURE);
			//	szMessage = szFormat;
			//}

			break;

		case AGPMADMIN_CHAREDIT_MONEY_INV:
			if(pstCharEdit->m_lEditResult != 0)
			{
				szFormat.LoadString(IDS_EDIT_RESULT_3);
				szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, _T("Inventory Money"), pstCharEdit->m_llNewValueINT64);
			}
			else
			{
				szFormat.LoadString(IDS_EDIT_RESULT_FAILURE);
				szMessage = szFormat;
			}

			break;

		case AGPMADMIN_CHAREDIT_MONEY_BANK:
			if(pstCharEdit->m_lEditResult != 0)
			{
				szFormat.LoadString(IDS_EDIT_RESULT_3);
				szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, _T("Bank Money"), pstCharEdit->m_llNewValueINT64);
			}
			else
			{
				szFormat.LoadString(IDS_EDIT_RESULT_FAILURE);
				szMessage = szFormat;
			}

			break;

		//case AGPMADMIN_CHAREDIT_SKILL_LEARN:
		//	if(pstCharEdit->m_lEditResult != 0)
		//	{
		//		szFormat.LoadString(IDS_SKILL_LEARN_SUCCESS);
		//		szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, pstCharEdit->m_szNewValueChar);
		//	}
		//	else
		//	{
		//		szFormat.LoadString(IDS_SKILL_LEARN_FAILURE);
		//		szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, pstCharEdit->m_szNewValueChar);
		//	}

		//	break;

		//case AGPMADMIN_CHAREDIT_SKILL_LEVEL:
		//	if(pstCharEdit->m_lEditResult != 0)
		//	{
		//		szFormat.LoadString(IDS_SKILL_UPGRADE_SUCCESS);
		//		szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, pstCharEdit->m_szNewValueChar, pstCharEdit->m_lNewValueINT);
		//	}
		//	else
		//	{
		//		szFormat.LoadString(IDS_SKILL_UPGRADE_FAILURE);
		//		szMessage.Format((LPCTSTR)szFormat, pstCharEdit->m_szCharName, pstCharEdit->m_szNewValueChar);
		//	}

		//	break;
	}

	if(szMessage.GetLength() > 0)
		AlefAdminManager::Instance()->m_pMainDlg->ProcessAdminMessage((LPCTSTR)szMessage);

	// �ѹ� �ٽ� �˻��� �ؼ� �������� ��Ų��.
	AlefAdminManager::Instance()->m_pMainDlg->SearchCharacter(m_stCharData.m_stBasic.m_szCharName);

	return TRUE;
}

// 2005.04.25. steeple
// ���ο� ������ �غ� �ݹ�. �Ƿ���~
BOOL AlefAdminCharacter::CBReceiveCharGuildData(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!pData || !pClass)
		return FALSE;

	AgpdCharacter* pcsCharacter = static_cast<AgpdCharacter*>(pData);
	AlefAdminCharacter* pThis = static_cast<AlefAdminCharacter*>(pClass);
	CHAR* szGuildID = pCustData ? static_cast<CHAR*>(pCustData) : NULL;

	// �˻��� ĳ���Ͱ� �ƴ϶�� ������~
	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(pcsCharacter->m_szID) == FALSE)
		return TRUE;

	return pThis->SetGuildID(szGuildID);
}

// 2005.05.18. steeple
BOOL AlefAdminCharacter::SetGuildID(LPCTSTR szGuildID)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szGuildID)
		return FALSE;

	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(m_stCharData.m_stBasic.m_szCharName) == FALSE)
		return FALSE;

	strncpy(m_stCharData.m_stSub.m_szGuildID, szGuildID, AGPMGUILD_MAX_GUILD_ID_LENGTH);

	CXTPPropertyGridItem* pCategory = m_pcsCharGrid->FindItem(_T("Play Data"));
	if(pCategory)
	{
		CXTPPropertyGridItems* pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			CXTPPropertyGridItem* pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID));
			if(pSubItem)
			{
				CString szValue = _T("");
				szValue.Format("%s", szGuildID ? szGuildID : _T(""));
				pSubItem->SetValue(szValue);
			}
		}
	}

	return TRUE;
}

// 2005.09.14. steeple
BOOL AlefAdminCharacter::SetServerName(LPCTSTR szServerName)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(!szServerName)
		return FALSE;

	if(AlefAdminManager::Instance()->m_pMainDlg->IsSearchCharacter(m_stCharData.m_stBasic.m_szCharName) == FALSE)
		return FALSE;

	strncpy(m_stCharData.m_stBasic.m_szServerName, szServerName, AGPMADMIN_MAX_SERVERNAME_LENGTH);

	CXTPPropertyGridItem* pCategory = m_pcsCharGrid->FindItem(_T("Basic"));
	if(pCategory)
	{
		CXTPPropertyGridItems* pSubItemList = pCategory->GetChilds();
		if(pSubItemList)
		{
			CXTPPropertyGridItem* pSubItem = (CXTPSetValueGridItem*)pSubItemList->FindItem(AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_SERVER));
			if(pSubItem)
			{
				CString szValue = _T("");
				szValue = szServerName;
				pSubItem->SetValue(szValue);
			}
		}
	}

	return TRUE;
}

// Dialog �� �ִ� � Grid �� �ǵ���� �ϴ� �̸��� Message �� ����´�.
// �׷��Ƿ�, � Grid ���� �˾Ƴ��� �Ѵ�.
LRESULT AlefAdminCharacter::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(wParam == XTP_PGN_ITEMVALUE_CHANGED)
	{
		CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;
		if(pItem == NULL)
			return FALSE;

		CXTPPropertyGridItem* pCategory = pItem->GetParentItem();
		if(!pCategory || !pCategory->IsCategory())
			return FALSE;

		// �������� �𸣰�����, Grid �� �ƴ϶� GridView �� �Ѿ�´�. -0-
		CXTPPropertyGridView* pGridView = (CXTPPropertyGridView*)pCategory->GetGrid();
		if(!pGridView)
			return FALSE;

		if(pGridView == &(m_pcsCharGrid->GetGridView()))	// Char Grid
			OnCharGridItemValueChanged(pItem);
	}
	
	return FALSE;
}

LRESULT AlefAdminCharacter::OnCharGridItemValueChanged(CXTPPropertyGridItem* pItem)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(pItem == NULL)
		return FALSE;

	CString szCaption = pItem->GetCaption();
	CString szValue = pItem->GetValue();

	// 2006.01.24. steeple
	// ����� �ٸ��� ó���Ѵ�.
	if(szCaption.Compare((LPCTSTR)AlefAdminManager::Instance()->GetStringManager().GetResourceMessage(IDS_GUILD_ID)) == 0)
	{
		// ����üũ�� �� �� �� ��
		if(AlefAdminManager::Instance()->GetAdminLevel() < AGPMADMIN_LEVEL_3)
		{
			AlefAdminManager::Instance()->OpenNotPrivilegeDlg();
			ShowCharData();
			ShowCharDataSub();
			return FALSE;
		}

		CString szMsg = _T("");
		szMsg.Format(IDS_GUILD_MEMBER_FORCED_LEAVE, m_stCharData.m_stBasic.m_szCharName, m_stCharData.m_stSub.m_szGuildID);
		if(::AfxMessageBox(szMsg, MB_YESNO) == IDNO)
		{
			ShowCharData();
			ShowCharDataSub();
			return FALSE;
		}

		// ����ߴٸ�, ������.
		AlefAdminManager::Instance()->m_pMainDlg->m_pGuildView->SendForcedMemberLeave(m_stCharData.m_stSub.m_szGuildID, m_stCharData.m_stBasic.m_szCharName);
		return FALSE;
	}

	EditChar(m_stCharData.m_stBasic.m_lCID, m_stCharData.m_stBasic.m_szCharName, (LPCTSTR)szCaption, (LPCTSTR)szValue);

	return FALSE;
}











int AlefAdminCharacter::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	if(CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pcsCharGrid = new CXTPPropertyGrid;

	return 0;
}

BOOL AlefAdminCharacter::OnEraseBkgnd(CDC* pDC)
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	UNREFERENCED_PARAMETER(pDC);	
	return TRUE;
	//return CFormView::OnEraseBkgnd(pDC);
}

void AlefAdminCharacter::OnPaint()
{
	AFX_MANAGE_STATE( AfxGetStaticModuleState() );

	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CFormView::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CRect r;
	GetClientRect(&r);
	CXTMemDC memDC(&dc, r);

	CFormView::DefWindowProc(WM_PAINT, (WPARAM)memDC.m_hDC, 0);
}

