// Plugin_NPC.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "Plugin_NPC.h"
#include "MainWindow.h"
#include "MainFrm.h"

#include "NPCNameDlg.h"
#include "PositionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void __DrawWireFrame( RpClump * pClump );
void _GetIntersectPos( AuPOS *pPos , RwV3d *pNormal , RwV3d *pStart , RwV3d *pEnd , RwV3d *pCollisionPoint );

/////////////////////////////////////////////////////////////////////////////
// CPlugin_NPC

CPlugin_NPC::CPlugin_NPC()
{
	// �ʱ� ���̵� �ʸ����� ����..
	m_nCharacterIDCounter	= 100000;
	m_pSelectedClump		= NULL	;

	m_PrevMouseStatus.pos.x	= 0;
	m_PrevMouseStatus.pos.y	= 0;

	// �ɼǼ���..
	m_stOption.bShowSphere			= true;
	m_stOption.bExcludeWhenExport	= true;

	m_strShortName = "NPC";
}

CPlugin_NPC::~CPlugin_NPC()
{
}


BEGIN_MESSAGE_MAP(CPlugin_NPC, CWnd)
	//{{AFX_MSG_MAP(CPlugin_NPC)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlugin_NPC message handlers

void CPlugin_NPC::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 255 , 0 ) );
	dc.TextOut( 0 , 40 , "�ɸ��͸� ����Ŭ���ϸ� �̸� ����.." );
	dc.TextOut( 0 , 60 , "�����Ҷ� �ɼ� �Ű澲����~" );
	dc.TextOut( 0 , 80 , "����(�Ǵ� �߸�) UI�̱� ������ ," );
	dc.TextOut( 0 , 100 , "���߿� Ȯ �ٲ����� �𸨴ϴ�.." );
	dc.TextOut( 0 , 120 , "�� �׸��� ������ ��ư���� �̺�Ʈ ����" );
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL CPlugin_NPC::OnSelectedPlugin		()
{
	m_pCurrentCharacter	= NULL;
	return TRUE;
}

BOOL CPlugin_NPC::OnDeSelectedPlugin	()
{
	return TRUE;
}

BOOL CPlugin_NPC::OnLButtonDownGeometry	( RwV3d * pPos )
{
	AgpdCharacter * pCharacter;

	int	nSelect = m_wndCombo.GetCurSel();
	if( nSelect == CB_ERR )
	{
		MD_SetErrorMessage( "No item is selected-_-\n" );
		return TRUE;
	}

	CString	str;
	m_wndCombo.GetLBText( nSelect , str );

	int	nTemplateID = 1;	// ���ø��� �� 

	nTemplateID = atoi( ( LPCTSTR ) str );

	// ���ø� ������üũ..
	if( nTemplateID < 0 )
	{
		MD_SetErrorMessage( "�ɸ��� ���ø� ���̵� �̻�..\n" );
		return FALSE;
	}

	/*
	BOOL bMovable = FALSE;
	
	{
		DWORD uRet = MessageBox( "NPC�� �����̴� �༮�Դϱ�?.." , "NPC Plugin" , MB_YESNOCANCEL );
		switch( uRet )
		{
		case IDYES:	bMovable = TRUE	; break;
		case IDNO:	bMovable = FALSE; break;
		default:
			// do nothing..
			return FALSE;
		}
	}
	*/

	// �̷��� �ϸ� �ȵ���... -_-;
	// m_nCharacterIDCounter NPC������ ������ �������� ��� �������... -_-; By Parn
	char	strNPCName[ 256 ];

	m_nCharacterIDCounter = rand() % 10000;
	wsprintf( strNPCName , "NPC_%04d" , m_nCharacterIDCounter );

	g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ) );		// modify by gemani
	pCharacter = g_pcsAgpmCharacter->AddCharacter( m_nCharacterIDCounter++ , nTemplateID , strNPCName );

	if( NULL == pCharacter )
	{
		DisplayMessage( RGB( 232 , 121 , 25 ) , "�ش� �ɸ��Ϳ� ���� ���ø��� �����~" );
		return TRUE;
	}

	// �ش� ��ġ�� ���ø� ����..

	AuPOS	pos;
	pos.x	= pPos->x;
	pos.y	= 0.0f;
	pos.z	= pPos->z;

	ApWorldSector * pWorldSector;
	pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

	if( pWorldSector )
	{
		pos.y = AGCMMAP_THIS->GetHeight_Lowlevel( pos.x , pos.z , SECTOR_MAX_HEIGHT);
		//g_pcsAgcmCamera.UpdateCamera();
		pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD	;
		pCharacter->m_stPos				= pos							;

		g_pcsAgpmCharacter->UpdateInit( pCharacter );

		g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos					); 
		//g_pcsAgcmCharacter->TurnSelfCharacter( 0.0f , 0 );//(float) 0 * 180.0f / 3.1415927f	);
	}

	SetSaveData();

	return TRUE;
}

BOOL CPlugin_NPC::OnLButtonUpGeometry	( RwV3d * pPos )
{
	m_pCurrentCharacter	= NULL;
	return TRUE;
}

int CPlugin_NPC::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect	rect ;
	rect.SetRect( 0 , 0 , 300 , 200 );
	m_wndCombo.Create( CBS_DROPDOWNLIST | WS_VISIBLE | WS_CHILD | WS_VSCROLL | CBS_SORT , rect , this , 21532 );

	// �ɸ��� �̴��ӷ����� �Ͽ�..
	// "%04d , �ɸ��� ����" ���� �޺��� �����Ѵ�..

	if( g_pcsAgpmCharacter )
	{
		INT32						lIndex		= 0;
		AgpdCharacterTemplate	*	pTemplate	;
		char						str[ 256 ]	;
		for (	pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex);
				pTemplate	;
				pTemplate	= g_pcsAgpmCharacter->GetTemplateSequence(&lIndex))
		{
			wsprintf( str , "%04d, %s" , pTemplate->m_lID , pTemplate->m_szTName );
			m_wndCombo.AddString( str );
		}
	}

	return 0;
}

void CPlugin_NPC::OnDestroy() 
{
	CWnd::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CPlugin_NPC::OnSelectObject	( RpClump * pClump )
{
	TRACE( "Select Object\n" );
	INT32	nID;
	switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID ) )
	{
	case ACUOBJECT_TYPE_CHARACTER:
		{
			if( ISNPCID( nID ) )
			{
				m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == m_pCurrentCharacter )
				{
					return FALSE;
				}
			}
		}
		break;
	default:
		return FALSE;
	}

	return TRUE;
}

BOOL CPlugin_NPC::OnMouseMoveGeometry	( RwV3d * pPos )
{
	if( m_pCurrentCharacter && pPos )
	{
		// �ɸ��͸� pPos ��ġ�� �̵���Ŵ..
		AuPOS	pos;
		pos.x	= pPos->x;
		pos.y	= AGCMMAP_THIS->GetHeight_Lowlevel( pPos->x , pPos->z , SECTOR_MAX_HEIGHT);
		pos.z	= pPos->z;

		g_pcsAgpmCharacter->UpdatePosition( m_pCurrentCharacter , &pos , FALSE , FALSE );

		SetSaveData();
	}

	return TRUE;
}

BOOL CPlugin_NPC::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "NPC ���� ����" );

	return TRUE;
}

BOOL CPlugin_NPC::OnSaveData			()
{
	if( ALEF_LOADING_MODE == LOAD_NORMAL )
		return g_pcsAgpmCharacter->StreamWriteStaticCharacter( "ini\\npc.ini" );
	else
		return TRUE;
}

BOOL CPlugin_NPC::OnLoadData			()
{
	if( ALEF_LOADING_MODE == LOAD_NORMAL )
		return g_pcsAgpmCharacter->StreamReadStaticCharacter( "ini\\npc.ini" , TRUE );
	else
		return TRUE;
}

BOOL CPlugin_NPC::Window_OnLButtonDblClk	( RsMouseStatus *ms )
{
	RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

	if( pClump )
	{
		INT32	nID;
		switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID ) )
		{
		case ACUOBJECT_TYPE_CHARACTER:
			{
				AgpdCharacter * pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == pCurrentCharacter )
				{
					return FALSE;
				}

				CNPCNameDlg dlg;
				dlg.m_strNameOrigin	= pCurrentCharacter->m_szID;
				dlg.m_strName		= pCurrentCharacter->m_szID;

				if( IDOK == dlg.DoModal() )
				{
					strncpy( pCurrentCharacter->m_szID , (LPCTSTR) dlg.m_strName , AGPACHARACTER_MAX_ID_STRING );
					// ������Ʈ..
					DisplayMessage( "NPC �̸� �����߾�� (%s -> %s)" , dlg.m_strNameOrigin , dlg.m_strName );

					SetSaveData();
				}
			}
			break;
		default:
			return FALSE;
		}

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_NPC::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

	if( pClump )
	{
		INT32	nID;
		switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID )  )
		{
		case ACUOBJECT_TYPE_CHARACTER:
			{
				m_pCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				if( NULL == m_pCurrentCharacter )
				{
					return FALSE;
				}

				Invalidate( FALSE );
			}
			break;
		default:
			return FALSE;
		}

		m_pSelectedClump	= pClump;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_NPC::Window_OnLButtonUp	( RsMouseStatus *ms )
{
	m_pSelectedClump	= NULL;

	return FALSE;
}

BOOL CPlugin_NPC::Window_OnMouseMove		( RsMouseStatus *ms )
{
	if( m_pCurrentCharacter )
	{
		if (ms->control && ms->shift )
		{
			FLOAT	fDeltaX	= ms->pos.x - m_PrevMouseStatus.pos.x;	
			FLOAT	fDeltaY	= ms->pos.y - m_PrevMouseStatus.pos.y;
			
			g_pcsAgpmCharacter->TurnCharacter( m_pCurrentCharacter , 0 , fDeltaY );

			SetSaveData();

			return TRUE;
		}
		else if( ms->shift )
		{
			// ���� ����..

			// y�����θ� ������..
			//g_pMainFrame->m_pTileList->m_pObjectWnd->TransformObject(
			//	GetSelectedObject() , ms , fDeltaX , fDeltaY , &m_Position, 0 );

			// �׳� Translate..
			FLOAT	fHeightDistance = 0.0f;

			// ��ȭ�� ���� ���� ����.
			{
				RwV3d			*	camLookAt		= RwMatrixGetAt	( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );

				RwV3d normalvector	;
				RwV3d CollisionPoint;

				// ���� ��ֺ���..
				normalvector.x	= - camLookAt->x;
				normalvector.y	= 0.0f			;
				normalvector.z	= - camLookAt->z;

				RwLine	CameraPixelRay		;

				RwCameraCalcPixelRay( g_pEngine->m_pCamera , &CameraPixelRay	, &ms->pos );
				
				_GetIntersectPos( 
					&m_pCurrentCharacter->m_stPos	,
					&normalvector					,
					&CameraPixelRay.start			,
					&CameraPixelRay.end				,
					&CollisionPoint					);

				fHeightDistance = CollisionPoint.y - m_pCurrentCharacter->m_stPos.y;
			}

			m_pCurrentCharacter->m_stPos.y += fHeightDistance;
			g_pcsAgpmCharacter->UpdatePosition	( m_pCurrentCharacter , &m_pCurrentCharacter->m_stPos ); 
			return TRUE;
		}
	}

	m_PrevMouseStatus = * ms;
	return FALSE;
}


BOOL CPlugin_NPC::OnWindowRender			()
{
	__DrawWireFrame( m_pSelectedClump );
	return TRUE;
}

BOOL CPlugin_NPC::Window_OnKeyDown		( RsKeyStatus *ks	)
{
	if( NULL == m_pCurrentCharacter ) return TRUE;

    switch( ks->keyCharCode )
    {
	case rsDEL:
		{
			// ������ �ɸ��� ����..
			g_pcsAgpmCharacter->RemoveCharacter( m_pCurrentCharacter->m_lID , FALSE , FALSE );
			m_pCurrentCharacter	= NULL;
			m_pSelectedClump	= NULL;

			SetSaveData();
		}
		break;
	default:
		break;
	}

	return TRUE;
}


void __DrawWireFrame( RpClump * pClump )
{
	if( NULL == pClump ) return;
	// ����Ʈ�� ��ϵǾ��ִ� ������Ʈ������ WireFrame�� �׸���.
	static
    RwImVertexIndex indicesSample[24] = {	0, 1, 1, 3, 3, 2, 2, 0,
											4, 5, 5, 7, 7, 6, 6, 4,
											0, 4, 1, 5, 2, 6, 3, 7 };

    RwMatrix       *ltm			;// = RwFrameGetLTM(RpClumpGetFrame(m_pSelectedClump));
    RwIm3DVertex	pVertex		[ 8		];
    RwInt32         count = 0 , i;
	RwBBox			bbox		;

	ClumpGetBBox( pClump , &bbox );

	for (i = 0; i < 8; i++)
	{
		RwIm3DVertexSetPos(
			pVertex + 8 * count + i ,
			i&1 ? bbox.sup.x : bbox.inf.x,
			i&2 ? bbox.sup.y : bbox.inf.y,
			i&4 ? bbox.sup.z : bbox.inf.z);
		RwIm3DVertexSetRGBA(pVertex + 8 * count + i, 255, 0, 0, 128); 
	}

	ltm			= RwFrameGetLTM(RpClumpGetFrame(pClump));
	if (RwIm3DTransform( pVertex + 8 * count , 8 , ltm , rwIM3D_VERTEXRGBA ) )
	{
		RwIm3DRenderIndexedPrimitive(rwPRIMTYPELINELIST, indicesSample , 24 );
		RwIm3DEnd();
	}
}

BOOL CPlugin_NPC::Window_OnRButtonDown	( RsMouseStatus *ms )
{
	if( GetAsyncKeyState( VK_SHIFT ) < 0 )
	{
		RpClump * pClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

		if( pClump )
		{
			INT32	nID;
			switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID )  )
			{
			case ACUOBJECT_TYPE_CHARACTER:
				{
					m_pPopupCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
					if( NULL == m_pPopupCurrentCharacter )
					{
						return FALSE;
					}

					Invalidate( FALSE );
				}
				break;
			default:
				return FALSE;
			}

			m_pPopupSelectedClump	= pClump;

			CPositionDlg	dlg;

			dlg.m_fX		= m_pPopupCurrentCharacter->m_stPos.x;
			dlg.m_fY		= m_pPopupCurrentCharacter->m_stPos.y;
			dlg.m_fZ		= m_pPopupCurrentCharacter->m_stPos.z;
			dlg.m_fDegreeX	= m_pPopupCurrentCharacter->m_fTurnX;
			dlg.m_fDegreeY	= m_pPopupCurrentCharacter->m_fTurnY;

			if( IDOK == dlg.DoModal() )
			{
				AuPOS	pos;
				pos.x	= dlg.m_fX;
				pos.y	= dlg.m_fY;
				pos.z	= dlg.m_fZ;

				g_pcsAgpmCharacter->UpdatePosition( m_pPopupCurrentCharacter , &pos , FALSE , FALSE );
				g_pcsAgpmCharacter->TurnCharacter( m_pPopupCurrentCharacter , 0 , dlg.m_fDegreeY );
			}

			m_pPopupSelectedClump		= NULL;
			m_pPopupCurrentCharacter	= NULL;

			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_NPC::Window_OnRButtonUp		( RsMouseStatus *ms )
{
	if(	abs( m_PrevMouseStatus.pos.x - ms->pos.x ) < 5	&&
		abs( m_PrevMouseStatus.pos.y - ms->pos.y ) < 5	&&
		GetAsyncKeyState( VK_SHIFT ) >= 0 )
	{
		m_pPopupSelectedClump = AGCMMAP_THIS->GetCursorClump( g_pEngine->m_pCamera , &ms->pos , AgcmMap::PICKINGCHARACTER );

		if( m_pPopupSelectedClump )
		{
			INT32	nID;
			switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( m_pPopupSelectedClump , &nID ) )
			{
			case ACUOBJECT_TYPE_CHARACTER:
				{
					m_pPopupCurrentCharacter	= g_pcsAgpmCharacter->GetCharacter( nID );
				}
				break;
			default:
				break;
			}

			if( NULL == m_pPopupCurrentCharacter )
			{
				m_pPopupSelectedClump		= NULL;
				m_pPopupCurrentCharacter	= NULL;
			}
		}

		if( NULL == m_pPopupSelectedClump )
		{
			return FALSE;
		}
			
		try
		{
			g_pcsApmEventManagerDlg->Open( m_pPopupCurrentCharacter );
			SetSaveData();
		}
		catch(...)
		{
			char	str[] = "Dialog�� ���������� ������� �ʾҽ��ϴ�.. �ĵ� ���̺��ϰ� �ٽ��Ѽ���. ���̺갡 �� ������ �� �ֽ��ϴ�! -_-; �׷��� ���̺� �Ͻðڽ��ϱ�?.";
			if( IDYES == g_pMainFrame->MessageBox( str , "����" , MB_ICONERROR | MB_YESNO ) ) 
			{
				g_pMainFrame->Save();
			}
		}
	}

	return FALSE;
}

//INT32 g_nDebugX = 376;
//INT32 g_nDebugZ = 408;

// ������ (2004-06-23 ���� 11:01:59) : �������� ��� �׽�Ʈ �ڵ�.

void CPlugin_NPC::OnChangeSectorGeometry	( ApWorldSector * pSector )
{
//	if( g_nDebugX != pSector->GetArrayIndexX()	&&
//		g_nDebugZ != pSector->GetArrayIndexZ()	)
//		return;
//
//	FLOAT	fStartX , fStartZ;
//	FLOAT	fHeight;
//
//	fStartX = pSector->GetXStart();
//	fStartZ = pSector->GetZStart();
//
//	int x , z;
//
//	char	str[ 1024 ];
//	char	strTmp[ 256 ];
//
//	for( z = 0 ; z < MAP_DEFAULT_DEPTH ; z ++ )
//	{
//		strcpy( str , "" );
//
//		for( x = 0 ; x < MAP_DEFAULT_DEPTH ; x ++ )
//		{
//			fHeight	= AGCMMAP_THIS->GetHeight_Lowlevel_HeightOnly( fStartX + x * MAP_STEPSIZE , fStartZ + z * MAP_STEPSIZE );
//
//			sprintf( strTmp , "%d " , ( INT32 ) fHeight );
//
//			strcat( str , strTmp );
//		}
//	}
}

BOOL CPlugin_NPC::OnCleanUpData			()
{
	g_pcsAgpmCharacter->RemoveAllCharacters();

	return TRUE;
}
