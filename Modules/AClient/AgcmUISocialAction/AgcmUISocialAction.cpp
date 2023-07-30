#include "AgcmUISocialAction.h"
#include <AgcmUIMain/AgcmUIMain.h>
#include <AgcmUIManager2/AgcmUIManager2.h>
#include <AgpmCharacter/AgpmCharacter.h>
#include <AgcmCharacter/AgcmCharacter.h>
#include <AgcmUIControl/AcUIGrid.h>
#include <AgcModule/AgcEngine.h>

AgcmUISocialAction::AgcmUISocialAction()
{
	SetModuleName("AgcmUISocialAction");
}

AgcmUISocialAction::~AgcmUISocialAction()
{
}

BOOL AgcmUISocialAction::OnAddModule()
{
	SetModule( (AgpmGrid*) GetModule("AgpmGrid"),
				(AgcmResourceLoader*) GetModule("AgcmResourceLoader"),
				(AgcmUIControl *) GetModule("AgcmUIControl"),
				(AgcmUIManager2 *) GetModule("AgcmUIManager2") );
			
	m_pcsAgcmCharacter	=  (AgcmCharacter*) GetModule("AgcmCharacter");
	m_pcsAgcmUIMain		=  (AgcmUIMain*) GetModule("AgcmUIMain");

	m_pcsAgcmUIMain->SetCallbackOpenTooltip( CBOpenQuickBeltToolTip, this );
	m_pcsAgcmUIMain->SetCallbackCloseTooltip( CBCloseQuickBeltToolTip, this );

	//AddFunction
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionOpenUI", CBSocialActionOpenUI, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionCloseUI", CBSocialActionCloseUI, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionToggleUI", CBSocialActionToggleUI, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionDragDrop", CBSocialActionDragDrop, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionDragEnd", CBSocialActionDragEnd, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionDLClick", CBSocialActionDLClick, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionRClick", CBSocialActionRClick, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionFocus", CBSocialActionFocus, 0 );
	m_pcsAgcmUIManager2->AddFunction( this, "CBSocialActionNotFocus", CBSocialActionNotFocus, 0 );

	//Event
	m_nEventSocialOpen	= m_pcsAgcmUIManager2->AddEvent( "SocialActionOpenUI" );
	m_nEventSocialClose	= m_pcsAgcmUIManager2->AddEvent( "SocialActionCloseUI" );

	//UserData
	m_pcsUserDataGrid	= m_pcsAgcmUIManager2->AddUserData( "SocialActionGrid", (PVOID)&m_cGrid, sizeof( AgpdGrid ), 0, AGCDUI_USERDATA_TYPE_GRID );

	return TRUE;
}

BOOL AgcmUISocialAction::OnInit()
{
	m_bIsOpen = FALSE;

	char strFileName[ 256 ] = { 0, };
	sprintf_s( strFileName, sizeof( char ) * 256, "Ini\\%sSocialAction.txt", g_pEngine->m_szLanguage );

#ifdef _BIN_EXEC_
	BOOL decrypte = FALSE;
#else
	BOOL decrypte = TRUE;
#endif

	if( Load( strFileName, decrypte ) )  
		return Create();
	
	return FALSE;
}

BOOL AgcmUISocialAction::OnDestroy()
{
	Destory();

	return TRUE;
}

BOOL AgcmUISocialAction::OnIdle(UINT32 ulClockCount)
{
	return TRUE;
}

BOOL AgcmUISocialAction::OnSocialUIOpen( void )
{
	m_bIsOpen = TRUE;
	m_pcsAgcmUIManager2->RefreshUserData( m_pcsUserDataGrid, TRUE );
	m_pcsAgcmUIManager2->ThrowEvent( m_nEventSocialOpen );
	return TRUE;
}

BOOL AgcmUISocialAction::OnSocialUIClose( void )
{
	m_bIsOpen = FALSE;
	m_pcsAgcmUIManager2->ThrowEvent( m_nEventSocialClose );
	return TRUE;
}

BOOL	AgcmUISocialAction::CBSocialActionOpenUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	return pThis->OnSocialUIOpen();
}

BOOL	AgcmUISocialAction::CBSocialActionCloseUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	return pThis->OnSocialUIClose();
}

BOOL	AgcmUISocialAction::CBSocialActionToggleUI(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	if( pThis->m_bIsOpen )
	{
		return pThis->OnSocialUIClose();
	}
	else
	{
		return pThis->OnSocialUIOpen();
	}
}

BOOL AgcmUISocialAction::CBSocialActionDragDrop(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	return TRUE;
}

BOOL AgcmUISocialAction::CBSocialActionDragEnd(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	AcUIGrid* pGrid = (AcUIGrid*)((AgcdUIControl*)pcsSourceControl)->m_pcsBase;
	AgpdGridSelectInfo * pGridSelectInfo = pGrid->GetGridItemClickInfo();
	if( !pGridSelectInfo->pGridItem )	return FALSE;

	if( pThis->m_pcsAgcmUIManager2->IsMainWindow( pGridSelectInfo->pTargetWindow ) )
	{

		return TRUE;
	}

	return FALSE;
}

BOOL AgcmUISocialAction::CBSocialActionDLClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	AgpdGridSelectInfo * pGridSelectInfo = ((AcUIGrid*)((AgcdUIControl*)pcsSourceControl)->m_pcsBase)->GetGridItemClickInfo();
	if( !pGridSelectInfo || !pGridSelectInfo->pGridItem )	return FALSE;

	return pThis->ActionStart( pGridSelectInfo->pGridItem->m_lItemID );
}

BOOL AgcmUISocialAction::CBSocialActionRClick(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	AgpdGridSelectInfo * pGridSelectInfo = ((AcUIGrid*)((AgcdUIControl*)pcsSourceControl)->m_pcsBase)->GetGridItemClickInfo();
	if( !pGridSelectInfo || !pGridSelectInfo->pGridItem )	return FALSE;

	return pThis->ActionStart( pGridSelectInfo->pGridItem->m_lItemID );
}

BOOL AgcmUISocialAction::CBSocialActionFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;

	AcUIGrid* pUIGrid = (AcUIGrid*)pcsSourceControl->m_pcsBase;
	if( !pUIGrid )							return FALSE;
	if( !pUIGrid->m_pToolTipAgpdGridItem )	return FALSE;
	if( AGPDGRID_ITEM_TYPE_SOCIALACTION != pUIGrid->m_pToolTipAgpdGridItem->m_eType )	return FALSE;
	
	return pThis->OpenTooltip( pUIGrid->m_pToolTipAgpdGridItem->m_lItemID );
}

BOOL AgcmUISocialAction::CBSocialActionNotFocus(PVOID pClass, PVOID pData1, PVOID pData2, PVOID pData3, PVOID pData4, PVOID pData5, ApBase *pcsTarget, AgcdUIControl *pcsSourceControl)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;

	pThis->CloseTooltip();
	
	return TRUE;
}

BOOL	AgcmUISocialAction::CBOpenQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	return pThis->CBSocialActionFocus( pThis, NULL, NULL, NULL, NULL, NULL, NULL, (AgcdUIControl*)pData );
}

BOOL	AgcmUISocialAction::CBCloseQuickBeltToolTip(PVOID pData, PVOID pClass, PVOID pCustData)
{
	AgcmUISocialAction* pThis = (AgcmUISocialAction*)pClass;
	return pThis->CBSocialActionNotFocus( pThis, NULL, NULL, NULL, NULL, NULL, NULL, (AgcdUIControl*)pData );
}

BOOL AgcmUISocialAction::ActionStart( INT32 nItem )
{
	return m_pcsAgcmCharacter->SendSocialAnimation( nItem );
}