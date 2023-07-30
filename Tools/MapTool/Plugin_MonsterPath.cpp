#include "stdafx.h"
#include "Plugin_MonsterPath.h"

CPlugin_MonsterPath::CPlugin_MonsterPath( VOID )
{
	m_strShortName		=	"M.Path";
	m_bPathData			=	FALSE;
}

CPlugin_MonsterPath::~CPlugin_MonsterPath( VOID )
{
}

BOOL	CPlugin_MonsterPath::OnSelectedPlugin( VOID )
{
	return TRUE;
}

BOOL	CPlugin_MonsterPath::OnDeSelectedPlugin( VOID )
{
	return TRUE;
}

BOOL	CPlugin_MonsterPath::OnWindowRender( VOID )
{
	return TRUE;
}

BOOL	CPlugin_MonsterPath::OnLButtonDownGeometry( RwV3d * pPos )
{
	ApWorldSector *			pWorldSector	=	NULL;
	STMonsterPathInfoPtr	pMonsterPath	=	NULL;
	INT						nCurrentSel		=	0;
	CString					strSpawnName;
	AuPOS					pos;

	pos.x	=	pPos->x;
	pos.y	=	0;
	pos.z	=	pPos->z;

	nCurrentSel		=	m_comboSpawnName.GetCurSel();
	if( nCurrentSel == -1 )
		return FALSE;

	m_comboSpawnName.GetLBText( nCurrentSel , strSpawnName );
	_AddMonsterPath( strSpawnName , pPos->x , pPos->z );

	return TRUE;
}

BOOL	CPlugin_MonsterPath::OnLButtonUpGeometry( RwV3d * pPos )
{
	return TRUE;
}

BOOL	CPlugin_MonsterPath::OnMouseMoveGeometry( RwV3d * pPos )
{
	return TRUE;
}

BEGIN_MESSAGE_MAP(CPlugin_MonsterPath, CUITileList_PluginBase)
ON_WM_PAINT()
ON_WM_CREATE()
END_MESSAGE_MAP()

VOID	CPlugin_MonsterPath::OnPaint( VOID )
{
	CPaintDC dc(this); 

	INT			nCurrentSel	=	m_comboSpawnName.GetCurSel();
	CString		strData;

	if( nCurrentSel != -1 )
	{
		m_comboSpawnName.GetLBText( nCurrentSel , strData );
		if( !m_MonsterPath.GetMonsterPath( strData.GetString() ) )
			dc.TextOut( 50 , 300 , _T("경로를 새로 만드세요") );
	}

}

INT CPlugin_MonsterPath::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_SpawnDataLoad.Init( ".//ini//spawnExcelData.txt" );
	m_pEventSpawn		=	g_pcsAgpmEventSpawn;

	CRect	rect;

	rect.SetRect( 0 , 0 , 300 , 200 );
	m_comboSpawnName.Create( CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL , rect , this , ID_COMBO_SPAWNNAME );

	rect.SetRect( 0 , 50 , 300 , 200 );
	m_comboPathType.Create( CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL , rect , this , ID_COMBO_PATHTYPE );

	rect.SetRect( 0, 200 , 100 , 300 );
	m_btnDeletePath.Create( "경로 삭제" , WS_VISIBLE | BS_PUSHBUTTON , rect , this , ID_BUTTON_DELETE_PATH );

	rect.SetRect( 100 , 200 , 200  , 300 );
	m_btnSave.Create( "저장" , WS_VISIBLE | BS_PUSHBUTTON , rect , this , ID_BUTTON_ONE_SAVE );

	rect.SetRect( 200 , 200 , 300 , 300 );
	m_btnAllSave.Create( "모두저장" , WS_VISIBLE | BS_PUSHBUTTON , rect , this , ID_BUTTON_ALL_SAVE );

	rect.SetRect( 0 , 300 , 300 , 400 );
	m_btnCameraMove.Create( "스폰지점으로이동" , WS_VISIBLE | BS_PUSHBUTTON , rect , this , ID_BUTTON_CAMERA_MOVE );


	setSpawnData&	pSpawnData	=	m_SpawnDataLoad.GetSpawnDataSet();
	setSpawnDataIter	Iter	=	pSpawnData.begin();

	for( ; Iter != pSpawnData.end() ; ++Iter )
	{
		m_comboSpawnName.AddString( Iter->c_str() );
	}

	m_comboPathType.AddString( "LOOP"		);
	m_comboPathType.AddString( "REVERSE"	);

	m_MonsterPath.LoadPath( "INI\\MonsterPath" );


	return 0;
}

LRESULT CPlugin_MonsterPath::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class
	switch(message)
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_COMBO_SPAWNNAME:
			switch(HIWORD(wParam))
			{
			case CBN_SELCHANGE:
				_ChangePathUpdate();
				break;
			}
			break;

		case ID_COMBO_PATHTYPE:
			switch( HIWORD(wParam) )
			{
			case CBN_SELCHANGE:
				_ChangePathType();
				break;
			}
			break;

		case ID_BUTTON_DELETE_PATH:
			switch( HIWORD(wParam) )
			{
			case BN_CLICKED:
				_PathDelete();
				break;
			}
			break;

		case ID_BUTTON_ONE_SAVE:
			switch( HIWORD(wParam) )
			{
			case BN_CLICKED:
				_SaveData();
				break;
			}
			break;

		case ID_BUTTON_ALL_SAVE:
			switch( HIWORD(wParam) )
			{
			case BN_CLICKED:
				_AllSaveData();
				break;
			}
			break;

		case ID_BUTTON_CAMERA_MOVE:
			switch( HIWORD(wParam) )
			{
			case BN_CLICKED:
				_MoveCamera();
				break;
			}
			break;

		}
		break;
	}

	return CWnd::WindowProc(message, wParam, lParam);
}

VOID	CPlugin_MonsterPath::_ChangePathUpdate( VOID )
{
	INT						nCurrentSel		=	m_comboSpawnName.GetCurSel();
	STMonsterPathInfoPtr	pMonsterPath	=	NULL;
	CString					strSpawnName;
	
	if( nCurrentSel == -1 )
		return;

	m_comboSpawnName.GetLBText( nCurrentSel , strSpawnName );
	pMonsterPath	=	m_MonsterPath.GetMonsterPath( strSpawnName.GetString() );

	if( !pMonsterPath )
	{
		pMonsterPath	=	m_MonsterPath.CreatePathInfo( strSpawnName.GetString() );
	}

	_UpdateClumpList( pMonsterPath );
	m_comboPathType.SetCurSel( pMonsterPath->eType );
}

VOID	CPlugin_MonsterPath::_ChangePathType( VOID )
{
	INT						nCurrentTypeSel		=	m_comboPathType.GetCurSel();
	INT						nCurrentNameSel		=	m_comboSpawnName.GetCurSel();
	STMonsterPathInfoPtr	pMonsterPath		=	NULL;
	CString					strSpawnName;

	if( nCurrentTypeSel == -1 || nCurrentNameSel == -1 )
		return;

	m_comboSpawnName.GetLBText( nCurrentNameSel , strSpawnName );

	pMonsterPath		=	m_MonsterPath.GetMonsterPath( strSpawnName.GetString() );
	if( !pMonsterPath )
		return;

	pMonsterPath->eType	=	(ePathType)nCurrentTypeSel;

}

VOID	CPlugin_MonsterPath::_UpdateClumpList( STMonsterPathInfoPtr pMonsterPath )
{
	_ClearClumpList();

	if( !pMonsterPath )
		return;

	MonsterPathListIter		Iter		=	pMonsterPath->PathList.begin();
	for( ; Iter != pMonsterPath->PathList.end() ; ++Iter )
	{
		_AddClump( Iter->nX , Iter->nZ );
	}
}

VOID	CPlugin_MonsterPath::_ClearClumpList( VOID )
{
	PathClumpListIter	Iter		=	m_ClumpList.begin();

	for( ; Iter != m_ClumpList.end() ; ++Iter )
	{

		_DelClump( (*Iter) );
	}

	m_ClumpList.clear();
}

BOOL	CPlugin_MonsterPath::_AddMonsterPath( STMonsterPathInfoPtr pMonsterPath , INT nX , INT nZ )
{
	if( !pMonsterPath )
		return FALSE;

	if( !m_MonsterPath.AddMonsterPath( pMonsterPath , nX , nZ ) )
		return FALSE;

	_AddClump( nX , nZ );

	return TRUE;
}

BOOL	CPlugin_MonsterPath::_AddMonsterPath( const CString& strSpawnName , INT nX , INT nZ )
{
	STMonsterPathInfoPtr		pMonsterPath		=	m_MonsterPath.GetMonsterPath( strSpawnName.GetString() );

	return _AddMonsterPath( pMonsterPath , nX , nZ );
}

BOOL	CPlugin_MonsterPath::_DeleteMonsterPath( STMonsterPathInfoPtr pMonsterPath , INT nIndex , RpClump* pClump )
{
	if( !pMonsterPath )
		return FALSE;

	if( m_MonsterPath.DelMonsterPath( pMonsterPath , nIndex ) == -1 )
		return FALSE;

	_DelClump( pClump );

	return TRUE;
}

BOOL	CPlugin_MonsterPath::_AddClump( INT nX , INT nZ )
{
	AuPOS	pos;

	pos.x	=	nX;
	pos.y	=	0;
	pos.z	=	nZ;
	
	pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( nX, nZ , SECTOR_MAX_HEIGHT);

	RpClump*		pClump	=	g_pcsAgcmObject->LoadClump( "Mon_Tiger.rws");
	RwFrameTranslate(RpClumpGetFrame(pClump), (RwV3d *)(&pos), rwCOMBINEREPLACE);
	g_pcsAgcmRender->AddClumpToWorld( pClump );

	m_ClumpList.push_back( pClump );

	return TRUE;
}

BOOL	CPlugin_MonsterPath::_DelClump( RpClump* pClump )
{
	g_pcsAgcmRender->RemoveClumpFromWorld( pClump );
	return TRUE;
}

VOID	CPlugin_MonsterPath::_PathDelete( VOID )
{
	INT						nCurrentSel		=	m_comboSpawnName.GetCurSel();
	STMonsterPathInfoPtr	pMonsterPath	=	NULL;
	CString					strSpawnName;

	if( nCurrentSel == -1 )
		return;

	_ClearClumpList();

	m_comboSpawnName.GetLBText( nCurrentSel , strSpawnName );
	m_MonsterPath.DestroyPathInfo( strSpawnName.GetString() );

	
}

VOID	CPlugin_MonsterPath::_SaveData( VOID )
{
	INT			nCurrentSel		=	m_comboSpawnName.GetCurSel();
	CString		strSpawnName;
	CString		strMessageBox;

	if( nCurrentSel == -1 )
		return;

	m_comboSpawnName.GetLBText( nCurrentSel , strSpawnName );
	if( m_MonsterPath.OneSavePath( ".\\INI\\MonsterPath" , strSpawnName.GetString() ) )
	{
		strMessageBox.Format( "%s 저장 완료" ,strSpawnName );
		AfxMessageBox( strMessageBox );
	}



}

VOID	CPlugin_MonsterPath::_AllSaveData( VOID )
{

	if( m_MonsterPath.SavePath( ".\\INI\\MonsterPath" , FALSE ) )
	{
		AfxMessageBox( "모두 저장 완료" );
	}

}

VOID	CPlugin_MonsterPath::_MoveCamera( VOID )
{
	INT			nCurrentSel		=	m_comboSpawnName.GetCurSel();
	CString		strSpawnName;
	CString		strMessageBox;

	if( nCurrentSel		==	-1 )
		return;

	m_comboSpawnName.GetLBText( nCurrentSel , strSpawnName );
	
	STMonsterPathInfoPtr		pMonsterPath		=	m_MonsterPath.GetMonsterPath( strSpawnName.GetBuffer() );
	if( !pMonsterPath )
		return;

	RwCamera*	pCamera		=	g_pEngine->m_pCamera;
	if( !pCamera )
		return;

	RwV3d		vTranslate;
	RwFrame		*pCameraFrame	=	NULL;
	RwMatrix	*pCameraMatrix	=	NULL;
	RwV3d		*vUP			=	NULL;
	RwV3d		*vRight			=	NULL;
	RwV3d		*vAT			=	NULL;

	pCameraFrame	= RwCameraGetFrame(pCamera);
	pCameraMatrix	= RwFrameGetMatrix(pCameraFrame);
	vAT				= RwMatrixGetAt(pCameraMatrix);
	vUP				= RwMatrixGetUp(pCameraMatrix);
	vRight			= RwMatrixGetRight(pCameraMatrix);

	RwFrameTranslate(pCameraFrame, &vTranslate, rwCOMBINEREPLACE );
	return;

}