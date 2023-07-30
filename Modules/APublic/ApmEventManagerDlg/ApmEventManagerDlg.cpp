// ApmEventManagerDlg.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include <afxdllx.h>

#include "ApmEventManagerDlg.h"
#include "ApmEventListDlg.h"
#include "ApmEventConditionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ApmEventListDlg *		g_pcsEventListDialog;
ApmEventConditionDlg *	g_pcsEventConditionDialog;
ApmEventDialog *		g_apcsEventDialog[APDEVENT_MAX_FUNCTION];

CHAR *aszEventFunctionName[APDEVENT_MAX_FUNCTION] =
{
	""					,//APDEVENT_FUNCTION_NONE					
	"Spawn"				,//APDEVENT_FUNCTION_SPAWN					
	"Update Factor"		,//APDEVENT_FUNCTION_FACTOR				
	"Vehicle"			,//APDEVENT_FUNCTION_VEHICLE				
	"Schedule"			,//APDEVENT_FUNCTION_SCHEDULE				
	"Hidden"			,//APDEVENT_FUNCTION_HIDDEN				
	"Shop"				,//APDEVENT_FUNCTION_SHOP					
	"Information"		,//APDEVENT_FUNCTION_INFORMATION			
	"Teleport"			,//APDEVENT_FUNCTION_TELEPORT				
	"NPCTrade"			,//APDEVENT_FUNCTION_NPCTRADE				
	"Conversation"		,//APDEVENT_FUNCTION_CONVERSATION			
	"Nature"			,//APDEVENT_FUNCTION_NATURE				
	"Status"			,//APDEVENT_FUNCTION_STATUS				
	"Action"			,//APDEVENT_FUNCTION_ACTION				
	"Skill"				,//APDEVENT_FUNCTION_SKILL					
	"Shrine"			,//APDEVENT_FUNCTION_SHRINE				
	"UvUReward"			,//APDEVENT_FUNCTION_UVU_REWARD			
	"ItemRepair"		,//APDEVENT_FUNCTION_ITEM_REPAIR			
	"MasterySpecialize"	,//APDEVENT_FUNCTION_MASTERY_SPECIALIZE	
	"Binding"			,//APDEVENT_FUNCTION_BINDING				
	"Bank"				,//APDEVENT_FUNCTION_BANK					
	"NPCDialog"			,//APDEVENT_FUNCTION_NPCDAILOG				
	"ItemConvert"		,//APDEVENT_FUNCTION_ITEMCONVERT			
	"Guild"				,//APDEVENT_FUNCTION_GUILD					
	"Product"			,//APDEVENT_FUNCTION_PRODUCT	
	"SkillMaster"		,//APDEVENT_FUNCTION_SKILLMASTER
	"Cook"				,//APDEVENT_FUNCTION_COOK
	"Quest"				,//APDEVENT_FUNCTION_QUEST
	"Auction"			//APDEVENT_FUNCTION_AUCTION
	//APDEVENT_MAX_FUNCTION
};

static AFX_EXTENSION_MODULE ApmEventManagerDlgDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// Remove this if you use lpReserved
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ApmEventManagerDLG.DLL Initializing!\n");
		
		// Extension DLL one-time initialization
		if (!AfxInitExtensionModule(ApmEventManagerDlgDLL, hInstance))
			return 0;

		// Insert this DLL into the resource chain
		// NOTE: If this Extension DLL is being implicitly linked to by
		//  an MFC Regular DLL (such as an ActiveX Control)
		//  instead of an MFC application, then you will want to
		//  remove this line from DllMain and put it in a separate
		//  function exported from this Extension DLL.  The Regular DLL
		//  that uses this Extension DLL should then explicitly call that
		//  function to initialize this Extension DLL.  Otherwise,
		//  the CDynLinkLibrary object will not be attached to the
		//  Regular DLL's resource chain, and serious problems will
		//  result.

		new CDynLinkLibrary(ApmEventManagerDlgDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ApmEventManagerDLG.DLL Terminating!\n");
		// Terminate the library before destructors are called
		AfxTermExtensionModule(ApmEventManagerDlgDLL);
	}
	return 1;   // ok
}

ApmEventDialog::ApmEventDialog()
{
}

ApmEventDialog::~ApmEventDialog()
{
}

ApmEventManagerDlg::ApmEventManagerDlg()
{
	SetModuleName("ApmEventManagerDlg");

	g_pcsEventListDialog = NULL;

	memset(g_apcsEventDialog, 0, sizeof(ApmEventDialog *) * APDEVENT_MAX_FUNCTION);
	
	m_pcsApmEventManager	= NULL;
	m_pcsAgpmCharacter		= NULL;
	m_pcsAgpmItem			= NULL;
}

ApmEventManagerDlg::~ApmEventManagerDlg()
{
	if (g_pcsEventListDialog)
		delete g_pcsEventListDialog;

	if (g_pcsEventConditionDialog)
		delete g_pcsEventConditionDialog;
}

BOOL	ApmEventManagerDlg::OnAddModule()
{
	m_pcsApmEventManager = (ApmEventManager *) GetModule("ApmEventManager");
	m_pcsAgpmCharacter = (AgpmCharacter *) GetModule("AgpmCharacter");
	m_pcsAgpmItem = (AgpmItem *) GetModule("AgpmItem");

	if (!m_pcsApmEventManager)
		return FALSE;

	return TRUE;
}

BOOL	ApmEventManagerDlg::OnInit()
{
	return TRUE;
}

BOOL	ApmEventManagerDlg::Open(ApBase *pcsBase, ApdEventAttachData *pstEventAD)
{
	/*
	if (!g_pcsEventListDialog)
	{
		g_pcsEventListDialog = new ApmEventListDlg;
		g_pcsEventListDialog->Create();
	}

	g_pcsEventListDialog->InitData(pcsBase, m_pcsApmEventManager);

	g_pcsEventListDialog->ShowWindow(SW_SHOW);
	*/
	ApmEventListDlg	dlgEventList;

	dlgEventList.InitData(pcsBase, pstEventAD ? pstEventAD : m_pcsApmEventManager->GetEventData(pcsBase), m_pcsApmEventManager);

	if (dlgEventList.DoModal() == IDOK)
	{
	}

	return TRUE;
}

BOOL	ApmEventManagerDlg::OpenCondition(ApdEvent *pstEvent)
{
	/*
	if (!g_pcsEventConditionDialog)
	{
		g_pcsEventConditionDialog = new ApmEventConditionDlg;
		g_pcsEventConditionDialog->Create();
	}

	g_pcsEventConditionDialog->InitData(pstEvent, m_pcsApmEventManager, m_pcsAgpmItem);

	g_pcsEventConditionDialog->ShowWindow(SW_SHOW);
	*/
	ApmEventConditionDlg	dlgCondition;

	dlgCondition.InitData(pstEvent, m_pcsApmEventManager, m_pcsAgpmItem);

	if (dlgCondition.DoModal() == IDOK)
	{
	}

	return TRUE;
}


void	ApmEventManagerDlg::Close()
{
	if (g_pcsEventListDialog)
		g_pcsEventListDialog->CloseWindow();
}

void	ApmEventManagerDlg::CloseCondition()
{
	if (g_pcsEventConditionDialog)
		g_pcsEventConditionDialog->CloseWindow();
}

BOOL	ApmEventManagerDlg::AddEventDialog(ApmEventDialog *pcsDialog, ApdEventFunction eFunction)
{
	if (g_apcsEventDialog[eFunction])
		return FALSE;

	g_apcsEventDialog[eFunction] = pcsDialog;

	return TRUE;
}
