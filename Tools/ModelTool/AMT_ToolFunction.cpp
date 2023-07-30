#include "stdafx.h"
#include "ModelTool.h"
#include "TargetDlg.h"
#include "AgcmIDBoard.h"

BOOL CModelToolApp::F_SetOffset()
{
	COffsetDlg dlg(&m_csCharacterOffset, &m_csCameraOffset, &m_csEditOffset);
	dlg.DoModal();

	return TRUE;
}

RpAtomic* CModelToolApp::MatD3DFxCB( RpAtomic* pstAtomic, void* pvData )
{
	AgcmRender* pAgcmRender = CModelToolApp::GetInstance()->GetEngine()->GetAgcmRenderModule();
	pAgcmRender->AtomicFxCheck( pstAtomic );
	return pstAtomic;
}

VOID CModelToolApp::SetMatD3DFxCB()
{
	int nID = GetCurrentID();

	AgpmCharacter* pAgpmCharacter = m_csAgcEngine.GetAgpmCharacterModule();
	AgcmCharacter* pAgcmCharacter = m_csAgcEngine.GetAgcmCharacterModule();

	AgpdCharacter *pcsAgpdCharacter	= pAgpmCharacter->GetCharacter( nID );
	if( pcsAgpdCharacter == NULL )
		return;

	AgcdCharacter *pcsAgcdCharacter = pAgcmCharacter->GetCharacterData( pcsAgpdCharacter );
	if( pcsAgcdCharacter == NULL )
		return;

	AgcmRender* pAgcmRender = m_csAgcEngine.GetAgcmRenderModule();
	if( m_csRenderOption.m_bMatD3DFx ) {
		pAgcmRender->EnableMatD3DFx();
	}
	else {
		pAgcmRender->DisableMatD3DFx();
	}

	RpClumpForAllAtomics( pcsAgcdCharacter->m_pClump, CModelToolApp::MatD3DFxCB, NULL );
}

BOOL CModelToolApp::F_SetRenderOption()
{
	//�̰�����.. ����� �־���� �ϳ� ^^;
	/*
		2008. 06. 09. pm 15:49 by ����
		Resource Tree Width ���� ������� �ʴ� ���� �ذ��� ���� ������ ���� �����մϴ�.

		1. AMT_Initialize.cpp �� CModelToolApp::LoadToolData() ���� dwWidth �� �о���̴� ����
		   m_csRenderOption.m_dwResourceTreeWidth �� ���� �ε�.
		2. AMT_ToolFunction.cpp �� CModelToolApp::F_SetRenderOption() �Լ����� 
		   m_csRenderOption.m_dwResourceTreeWidth �� ���� ����ؼ� �ʱ�ȭ ��Ű�� �κ��� �ּ�ó��
	    3. CModelToolApp::F_SetRenderOption() ���� Resource Form �� ũ�⸦ �����ϴ� ������
		   CModelToolApp::InitInstance() ���� �ʱ�ȭ ������ ����� �� �ѹ� �������ֵ��� ����

	*/
	//m_csRenderOption.m_dwResourceTreeWidth = CModelToolDlg::GetInstance()->m_rtResourceForm.right - CModelToolDlg::GetInstance()->m_rtResourceForm.left;

	CRenderOption csRenderOption;
	csRenderOption = m_csRenderOption;
	CRenderOptionDlg dlg( &csRenderOption );
	if( dlg.DoModal() != IDOK )
		return TRUE;

	m_csRenderOption = csRenderOption;

	//Rectó���� ����߰ڴµ�. @@; -> ��ǥ����~~~
	CModelToolDlg::GetInstance()->ResizeResourceForm( m_csRenderOption.m_dwResourceTreeWidth );
	
	if ( m_csAgcEngine.GetAgcmSound() )
		EnableSound( csRenderOption.m_bEnableSound );

	//. 2006. 2. 7. Nonstopdj
	//. anm���� read�� rate�� �����Ͽ� �о����.
	m_csAgcEngine.GetAgcmResourceLoaderModule()->SetCurrentAnimKeyFrameRate( m_csRenderOption.m_iKeyFrameRate + 1 );
	m_csAgcEngine.GetAgcmResourceLoaderModule()->SetSaveAnmfile( m_csRenderOption.m_bCheckSaveFile, NULL );

	SetMatD3DFxCB();

	AgcmIDBoard* pIDBoard = NULL;
	if( !m_csAgcEngine.GetAgcmTextBoardModule()->GetBoardDataList().empty() )
		pIDBoard = (*m_csAgcEngine.GetAgcmTextBoardModule()->GetBoardDataList().begin())->pIDBoard;

   	if( m_csRenderOption.m_bShowTextBoarder == FALSE ) 
	{
		if ( pIDBoard )		pIDBoard->SetEnabled( FALSE );
			
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawHPMP( FALSE );		//. HP
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawNameMine( FALSE );	//. "AMT1",
	}
	else
	{
		if ( pIDBoard )		pIDBoard->SetEnabled( TRUE );

		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawHPMP( TRUE );
		m_csAgcEngine.GetAgcmTextBoardModule()->SetDrawNameMine( TRUE );
	}

	m_csAgcEngine.GetAgcmTextBoardModule()->SetAllIDEnable(TB_MAINCHARACTER,m_csRenderOption.m_bShowTextBoarder);

	return TRUE;
}

BOOL CModelToolApp::F_SetAnimationOption()
{
	if(CModelToolDlg::GetInstance())
	{
		return CModelToolDlg::GetInstance()->OpenAnimationOptionDlg();
	}

	return TRUE;
}

BOOL CModelToolApp::F_ResetView()
{
	return ResetCamera();
}

BOOL CModelToolApp::F_SetTargetOption()
{
	CTargetDlg dlg;

	dlg.m_lTargetTID	=	m_lTargetTID			;
	dlg.m_lTargetNum	=	m_lNumTargets			;
	dlg.m_fOffsetX		=	m_v3dTargetPosOffset.x	;
	dlg.m_fOffsetY		=	m_v3dTargetPosOffset.y	;
	dlg.m_fOffsetZ		=	m_v3dTargetPosOffset.z	;

	if (dlg.DoModal() == IDOK)
	{
		m_lTargetTID			= dlg.m_lTargetTID	;
		m_lNumTargets			= dlg.m_lTargetNum	;
		m_v3dTargetPosOffset.x	= dlg.m_fOffsetX	;
		m_v3dTargetPosOffset.y	= dlg.m_fOffsetY	;
		m_v3dTargetPosOffset.z	= dlg.m_fOffsetZ	;

		m_csAgcEngine.GetAgpmCharacterModule()->RemoveAllCharacters();

		INT32		lTID				= m_lTID;
		HTREEITEM	hCurrentTreeItem	= m_hCurrentTreeItem;

		MakeTargetCharacter();

		if (!SetCharacter(lTID, hCurrentTreeItem, FALSE, FALSE))
			return FALSE;

//		ApplyObject();
	}

	return TRUE;
}
