// UITileList_ObjectTabWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ApBase.h"
#include "MapTool.h"
#include "UITileList_ObjectTabWnd.h"
#include "ApUtil.h"
#include "MainWindow.h"
#include "MainFrm.h"
#include "ObjectDialog.h"

#include "rpusrdat.h"

#include "ApmObject.h"
#include "AgcmObject.h"
#include "rtpick.h"

#include "MyEngine.h"
#include "ProgressDlg.h"
#include "ObjectBrushSettingDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern MainWindow			g_MainWindow;
extern CMainFrame *			g_pMainFrame;

void _GetIntersectPos( AuPOS *pPos , RwV3d *pNormal , RwV3d *pStart , RwV3d *pEnd , RwV3d *pCollisionPoint );
BOOL __ObjectLoadCallback_AdjustPosition(PVOID pData, PVOID pClass, PVOID pCustData);

/////////////////////////////////////////////////////////////////////////////
// CUITileList_ObjectTabWnd

static	char	g_szButtonName[ CUITileList_ObjectTabWnd::ID_COUNT ][ 256 ]=
{
	"�׸��忡 ����"		,
	"���̿� ����"		,
	"X���̵�"			,
	"Z���̵�"			,
	"X������ ȸ��"		,
	"�귯�û��"		,
};

BOOL	__DivisionObjectLoadCallback2 ( DivisionInfo * pDivisionInfo , PVOID pData );


BOOL _ProgressCallback( char * strMessage , int nCurrent , int nTarget , void *data );

BOOL __ObjectSaveCallback_NoneEventObjectOnly(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	// ������ (2004-07-05 ���� 5:23:48) : �̺�Ʈ ������ ���� ó���Ѵ�.
	if( g_pcsApmEventManager->CheckEventAvailable( pcsApdObject ) )
		return FALSE;

	return TRUE;
}

BOOL __ObjectSaveCallback_ArtistOnly(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	//AgcdObject	*pstAgcdObject = g_pcsAgcmObject->GetObjectData(pcsApdObject);
	//if( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE ) return FALSE;

	// ������ (2004-07-05 ���� 5:23:48) : �̺�Ʈ ���°� ó������ ����..
	if( !g_pcsApmEventManager->CheckEventAvailable( pcsApdObject ) )
		return FALSE;

	// �����ĸ�.. �����Ѵ�.
	if( g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_NATURE ) )
		return TRUE;
	else
		return FALSE;
}

BOOL __ObjectSaveCallback_DesignerOnly(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	//AgcdObject	*pstAgcdObject = g_pcsAgcmObject->GetObjectData(pcsApdObject);
	//if( pstAgcdObject->m_lObjectType & ACUOBJECT_TYPE_DUNGEON_STRUCTURE ) return FALSE;

	// ������ (2004-07-05 ���� 5:23:48) : �̺�Ʈ ���°� ó������ ����..
	if( !g_pcsApmEventManager->CheckEventAvailable( pcsApdObject ) )
		return FALSE;

	// ������ �̿��� �� �� ����..
	// ������ (2005-05-19 ���� 11:13:18) : ����Ʈ ����Ʈ�� ���̿�!
	if( g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_NATURE		)	||
		g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_POINTLIGHT )	||
		g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_SPAWN		)	)	// ������ ���ܽ�Ŵ..
		return FALSE;
	else
		return TRUE;
}

CUITileList_ObjectTabWnd::CUITileList_ObjectTabWnd()
{
	m_nMaxOID = 0;
	m_nMaxTID = 0;

	m_bSnapToGrid	= TRUE	;
	m_bSnapToHeight	= TRUE	;
	m_bMoveXAxis	= TRUE	;
	m_bMoveZAxis	= TRUE	;
	m_bRotateXAxis	= FALSE	;
	m_bUseBrush		= FALSE	;

	m_fDensity		= 1.0f		;
	m_fRotateMax	= 10.0f		;
	m_fRotateMin	= 0.0f		;
	m_fRotateYMin	= 0.0f		;
	m_fRotateYMax	= 5.0f		;
	m_fScaleMax		= 1.0f		;
	m_fScaleMin		= 1.0f		;

	strcpy( m_strSubDataMainDir , "" );
}

CUITileList_ObjectTabWnd::~CUITileList_ObjectTabWnd()
{
}


BEGIN_MESSAGE_MAP(CUITileList_ObjectTabWnd, CWnd)
	//{{AFX_MSG_MAP(CUITileList_ObjectTabWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_COMMAND(IDM_OBJECT_ADDENTRY, OnObjectAddentry)
	ON_COMMAND(IDM_OBJECT_DELETEENTRY, OnObjectDeleteentry)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_OBJECT_EDITENTRY, OnObjectEditentry)
	//}}AFX_MSG_MAP
	ON_COMMAND(IDM_OBJECTPOPUP_TAKETEMPLATEINFO, OnObjectpopupTaketemplateinfo)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CUITileList_ObjectTabWnd message handlers

void CUITileList_ObjectTabWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	
	// Do not call CWnd::OnPaint() for painting messages
}

int CUITileList_ObjectTabWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndTreeCtrl.Create(AFX_WS_DEFAULT_VIEW | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | LVS_SORTASCENDING,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST))
	{
		TRACE0("Failed to create tree control window\n");
		return -1;
	}

	LoadScript();

	// ��ư����..
	for( int i = 0 ; i < ID_COUNT ; i ++ )
	{
		m_pctlButton[ i ].Create(
			g_szButtonName[ i ]			,
			WS_VISIBLE | BS_CHECKBOX	,
			CRect(0, 0, 0, 0)			,
			this						,
			ID_SNAPTOGRID + i			);

		m_pctlButton[ i ].SetCheck( TRUE );
	}

	m_pctlButton[ ID_ROTATEXAXIS		].SetCheck( FALSE );
	m_pctlButton[ ID_USE_BRUSHDRAWING	].SetCheck( FALSE );
	return 0;
}

void CUITileList_ObjectTabWnd::OnSize(UINT nType, int cx, int cy) 
{
	int	nButtonHeight = ID_COUNT * OBJECT_BUTTON_HEIGHT;
	CRect rectTreeCtrl;

	CWnd::OnSize(nType, cx, cy);
	
	rectTreeCtrl.left	= MAP_UI_MARGIN			;
	rectTreeCtrl.top	= MAP_UI_MARGIN			;
	rectTreeCtrl.right	= cx - MAP_UI_MARGIN	;
	rectTreeCtrl.bottom	= cy - MAP_UI_MARGIN - nButtonHeight;

	m_wndTreeCtrl.MoveWindow( &rectTreeCtrl );

	for( int i = 0 ; i < ID_COUNT ; i ++ )
	{
		m_pctlButton[ i ].MoveWindow(
			rectTreeCtrl.left						,
			rectTreeCtrl.bottom	+ i * OBJECT_BUTTON_HEIGHT	,
			rectTreeCtrl.right - rectTreeCtrl.left	,
			OBJECT_BUTTON_HEIGHT					,
			TRUE									);
	}
}

/*
RpMaterial *CUITileList_ObjectTabWnd::GetAmbientInfo(RpMaterial *pstMaterial, PVOID pvData)
{
	BOOL *					pbAmbient = (BOOL *) pvData;
	RwSurfaceProperties *	pstSurfaceProperties = (RwSurfaceProperties *) RpMaterialGetSurfaceProperties(pstMaterial);

	if (pstSurfaceProperties->ambient == 1 && pstSurfaceProperties->diffuse && pstSurfaceProperties->ambient == pstSurfaceProperties->specular)
		*pbAmbient = TRUE;
	else
		*pbAmbient = FALSE;

	return NULL;
}
*/

RpMaterial *CUITileList_ObjectTabWnd::SetAmbient(RpMaterial *pstMaterial, PVOID pvData)
{
	BOOL *					pbAmbient = (BOOL *) pvData;
	RwSurfaceProperties *	pstSurfaceProperties = (RwSurfaceProperties *) RpMaterialGetSurfaceProperties(pstMaterial);

	if (!*pbAmbient)
		pstSurfaceProperties->specular = pstSurfaceProperties->diffuse = pstSurfaceProperties->ambient = 1;
	else
	{
		pstSurfaceProperties->specular = pstSurfaceProperties->diffuse = 0;
		pstSurfaceProperties->ambient = 1;
	}

	return pstMaterial;
}

RpAtomic *CUITileList_ObjectTabWnd::GetClumpInfo(RpAtomic *pstAtomic, PVOID pvData)
{
	CObjectDialog *	pcsDialog = (CObjectDialog *) pvData;
	RpGeometry *	pstGeometry = RpAtomicGetGeometry(pstAtomic);
	UINT32		ulFlags = RpGeometryGetFlags(pstGeometry);

	if (ulFlags & rpGEOMETRYPRELIT)
		pcsDialog->m_bPreLight = TRUE;
	else
		pcsDialog->m_bPreLight = FALSE;

	if (ulFlags & rpGEOMETRYLIGHT)
		pcsDialog->m_bLight = TRUE;
	else
		pcsDialog->m_bLight = FALSE;

	if (ulFlags & rpGEOMETRYMODULATEMATERIALCOLOR)
		pcsDialog->m_bMaterialColor = TRUE;
	else
		pcsDialog->m_bMaterialColor = FALSE;

	if (pstGeometry->preLitLum)
	{
		pcsDialog->m_nObjectRed = pstGeometry->preLitLum[0].red;
		pcsDialog->m_nObjectGreen = pstGeometry->preLitLum[0].green;
		pcsDialog->m_nObjectBlue = pstGeometry->preLitLum[0].blue;
		pcsDialog->m_nObjectAlpha = pstGeometry->preLitLum[0].alpha;
	}
	else
	{
		pcsDialog->m_nObjectRed = 0;
		pcsDialog->m_nObjectGreen = 0;
		pcsDialog->m_nObjectBlue = 0;
		pcsDialog->m_nObjectAlpha = 255;
	}

	pcsDialog->m_bForcePreLight = FALSE;

	//RpGeometryForAllMaterials(pstGeometry, GetAmbientInfo, &pcsDialog->m_bAmbient);

	return NULL;
}

RpMaterial *CUITileList_ObjectTabWnd::SetMaterialAlpha(RpMaterial *pstMaterial, PVOID pvData)
{
	RwRGBA *	pstColor = (RwRGBA *) pvData;

	RpMaterialSetColor(pstMaterial, pstColor);

	return pstMaterial;
}

RpAtomic *CUITileList_ObjectTabWnd::SetClumpInfo(RpAtomic *pstAtomic, PVOID pvData)
{
	CObjectDialog *	pcsDialog = (CObjectDialog *) pvData;
	RpGeometry *	pstGeometry = RpAtomicGetGeometry(pstAtomic);
	UINT32			ulFlags = RpGeometryGetFlags(pstGeometry);

	if (pcsDialog->m_bPreLight)
		ulFlags |= rpGEOMETRYPRELIT;
	else
		ulFlags &= ulFlags ^ rpGEOMETRYPRELIT;

	if (pcsDialog->m_bLight)
		ulFlags |= rpGEOMETRYLIGHT;
	else
		ulFlags &= ulFlags ^ rpGEOMETRYLIGHT;

	if (pcsDialog->m_bMaterialColor)
	{
		RwRGBA	rgba = {255,255,255,0};

		ulFlags |= rpGEOMETRYMODULATEMATERIALCOLOR;
		//RpGeometryForAllMaterials(pstGeometry, SetMaterialAlpha, &rgba);
	}
	else
		ulFlags &= ulFlags ^ rpGEOMETRYMODULATEMATERIALCOLOR;

	if (pcsDialog->m_bForcePreLight || !pstGeometry->preLitLum)
	{
		RwRGBA	rgba = {pcsDialog->m_nObjectRed, pcsDialog->m_nObjectGreen, pcsDialog->m_nObjectBlue, pcsDialog->m_nObjectAlpha};

		AcuObject::SetAtomicPreLitLim(pstAtomic, &rgba);
	}

	RpGeometrySetFlags(pstGeometry, ulFlags);

	if (pcsDialog->m_bAlpha)
		pcsDialog->m_bAmbient = FALSE;

	RpGeometryForAllMaterials(pstGeometry, SetAmbient, &pcsDialog->m_bAmbient);

	return pstAtomic;
}

void CUITileList_ObjectTabWnd::LoadScript()
{
	if( NULL == g_pcsAgcmObject ||
		NULL == g_pcsAgcmObject	) return;
	char	filename[ 1024 ];

	VERIFY( LoadCategory() );

	g_pcsAgcmObject->AddObjectTemplateCreateCallBack	( TemplateCreateCB	, this	);
	g_pcsAgcmObject->AddObjectCreateCallBack			( ObjectCreateCB	, this	);

	theApp.SetStartUpProcess( 81 );
	
	CProgressDlg	dlgProgress;
	dlgProgress.StartProgress( "������Ʈ ���ø� �ε�.." , 100 , this );

	// netong �߰�
	TRACE( "Shrine. Reading..\n" );

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ShrineTemplate.ini" );
	if( g_pcsAgpmShrine && g_pcsAgpmShrine->StreamReadTemplate( filename ) == FALSE )
	{
		MessageBox( "ShrineTemplate.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}
	// ������� netong �߰�
	theApp.SetStartUpProcess( 82 );

	theApp.SetStartUpProcess( 83 );

	TRACE( "DropItemTemplate. Reading..\n" );


	theApp.SetStartUpProcess( 84 );
	TRACE( "SpawnGroup. Reading..\n" );

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "SpawnGroup.ini" );
	if( g_pcsAgpmEventSpawn && g_pcsAgpmEventSpawn->StreamReadGroup( filename ) == FALSE )
	{
		MessageBox( "SpawnGroup.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	theApp.SetStartUpProcess( 85 );
	TRACE( "Teleport. Reading..\n" );

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "TeleportGroup.ini" );
	if( g_pcsAgpmEventTeleport && g_pcsAgpmEventTeleport->StreamReadGroup( filename , FALSE ) == FALSE )
	{
		MessageBox( "TeleportGroup.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	theApp.SetStartUpProcess( 86 );

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "TeleportPoint.ini" );
	if( g_pcsAgpmEventTeleport && g_pcsAgpmEventTeleport->StreamReadPoint( filename , FALSE ) == FALSE )
	{
		MessageBox( "TeleportPoint.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	theApp.SetStartUpProcess( 87 );
	TRACE( "ObjectTemplate. Reading..\n" );

	wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ObjectTemplate.ini" );
	if( g_pcsApmObject && g_pcsApmObject->StreamReadTemplate( filename , _ProgressCallback , ( void * ) &dlgProgress	) == FALSE )
	{
		MessageBox( "ObjectTemplate.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	if( g_pcsApmObject )
	{
		FILE	* pFile = fopen( "ObjectReplace.txt" , "rt" );
		if( pFile )
		{
			if( IDYES == g_pMainFrame->MessageBox( "������Ʈ ���ø� ���� ������ �ֽ��ϴ�. �����ҷ���?" ,"Maptool" , MB_YESNOCANCEL ) )
			{
				int	nOriginal , nReplace;
				int	nCount = 0;

				while( EOF != fscanf( pFile , "%d %d" , &nOriginal , &nReplace ) )
				{
					if( nOriginal != nReplace )
					{
						nCount ++;
						g_pcsApmObject->SetTemplateReplace( nOriginal , nReplace );
					}
					nOriginal = nReplace = 0;
				}

				CString	str , strtmp;
				str.Empty();

				vector <POINT>::iterator Iter;
				for ( Iter = g_pcsApmObject->m_vectorTemplateMatchingTable.begin( ) ; Iter != g_pcsApmObject->m_vectorTemplateMatchingTable.end( ) ; Iter++ )
				{
					strtmp.Format( "(%d->%d) " , (*Iter).x , (*Iter).y );
					str += strtmp;
				}

				str += "��ȯ Ȯ��!";
				g_pMainFrame->MessageBox( str );
			}

			fclose( pFile );
		}
	}
	
	dlgProgress.EndProgress();

	theApp.SetStartUpProcess( 88 );

	if( g_pcsAgcmPreLODManager )
	g_pcsAgcmPreLODManager->ObjectPreLODStreamRead(".\\Ini\\ObjectPreLOD.ini");
	
	TRACE( "ObjectStatic. Reading..\n" );
	theApp.SetStartUpProcess( 90 );

	theApp.SetStartUpProcess( 95 );
		
	TRACE( "...................................LoadScript Completed...\n" );
}

BOOL CUITileList_ObjectTabWnd::LoadLocalObjectInfo()
{
	DivisionCallbackResult	result;
	
	result.dlgProgress.StartProgress( "������Ʈ ���� �ε�" , AGCMMAP_THIS->EnumLoadedDivisionObject( NULL , NULL ) * 3 , g_pMainFrame	);

	if( AGCMMAP_THIS && AGCMMAP_THIS->EnumLoadedDivisionObject( __DivisionObjectLoadCallback2 , ( PVOID ) &result ) )
	{
		if( result.nDivisionCount == result.nSuccessCount )
		{
			// ����..
		}
		else
		{
			if( result.nSuccessCount == 0 )
			{
				switch( MessageBox( "�� ������ �κ� ������Ʈ ������ ���� �Ǵ� �Ϻΰ� �����. ���� OBDN���� �������?" , "���� ������Ʈ �ε�" , MB_YESNO ) )
				{
				case IDYES:
					{
						g_pcsApmObject->DeleteAllObject( TRUE , TRUE );
						LoadOldObjectScript();
					}
					break;

				case IDNO:
					// do nothing.
					break;
				}
			}
			else
			{
				MD_SetErrorMessage( "�ε��� �Ϻ������� �κп�����Ʈ ������ �����ϴ�. �ش��������� ������Ʈ�� ǥ�õ��� �ʽ��ϴ�." );
			}
		}
	}
	else
	{
		MD_SetErrorMessage( "ũ��Ƽ��.. ������ CUITileList_ObjectTabWnd::LoadLocalObjectInfo()" );
	}

	result.dlgProgress.EndProgress();

	return TRUE;
}

BOOL	CUITileList_ObjectTabWnd::LoadOldObjectScript	()
{
	DivisionCallbackResult	result;

	g_pcsApmObject->StreamReadAllDivision( APMOBJECT_LOCAL_INI_FILE_NAME_STATIC );

	if( AGCMMAP_THIS && AGCMMAP_THIS->EnumLoadedDivision( __DivisionObjectLoadCallback , ( PVOID ) &result ) )
	{
		if( result.nDivisionCount == result.nSuccessCount )
		{
			// ����..
		}
		else
		{
			if( result.nSuccessCount == 0 )
			{
				ASSERT( !"�� ������ �κ� ������Ʈ ������ �����ϴ�. ���� object.ini���� �о���Դϴ�." );

				// �ʱ�ȭ �ϰ� �ٽ÷ε�..
				g_pcsApmObject->DeleteAllObject( TRUE );

				// ��� Object.INI���� �д´�.
				char strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Ini\\Object.ini" , ALEF_CURRENT_DIRECTORY );
				VERIFY( g_pcsApmObject->StreamRead( strFilename ) );
				// ��� Object.INI���� �д´�.
				wsprintf( strFilename , "%s\\Ini\\ObjectStatic.ini" , ALEF_CURRENT_DIRECTORY );
				VERIFY( g_pcsApmObject->StreamRead( strFilename ) );
			}
			else
			{
				ASSERT( !"�ε��� �Ϻ������� �κп�����Ʈ ������ �����ϴ�. �ش��������� ������Ʈ�� ǥ�õ��� �ʽ��ϴ�." );
			}
		}
		
		return TRUE;
	}
	else
	{
		ASSERT( !"ũ��Ƽ��.. ������ CUITileList_ObjectTabWnd::LoadScript()" );
		MD_SetErrorMessage( "LoadOldObjectScript���� �����߻�" );
		return FALSE;
	}
}

BOOL	__DivisionObjectSaveCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	// OBDN : Object Division Normal
	// OBDS : Object Division Static

	//TRACE( "���� �ͽ���Ʈ '%s'\b" , strFilename );

	AuBOX bbox;
	bbox.inf.x	=	pDivisionInfo->fStartX;
	bbox.inf.z	=	pDivisionInfo->fStartZ;
	bbox.inf.y	=	0.0f;
	bbox.sup.x	=	pDivisionInfo->fStartX + pDivisionInfo->fWidth;
	bbox.sup.z	=	pDivisionInfo->fStartZ + pDivisionInfo->fWidth;
	bbox.sup.y	=	0.0f;

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_NORMAL ,
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_NoneEventObjectOnly )	)
	{
		// ����
		// do nothing..
	}
	else
	{
		// ����
		MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
		return FALSE;
	}

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST ,  
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_ArtistOnly )	)
	{
		// ���� 
	}
	else
	{
		// ����..
		MD_SetErrorMessage( "ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , TRUE )\n" , strFilename);
		return FALSE;
	}

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_DESIGN ,  
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_DesignerOnly )	)
	{
		// ���� 
	}
	else
	{
		// ����..
		MD_SetErrorMessage( "ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s, TRUE )\n" , strFilename );
		return FALSE;
	}

	{
		CUITileList_ObjectTabWnd * pWnd = ( CUITileList_ObjectTabWnd * ) pData;
		//////////////////////////////////////////////////////////////////////////
		// ������ (2004-11-29 ���� 12:03:07) : 
		// SubData ����..
		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_NORMAL ,
			pWnd->m_strSubDataMainDir , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_NoneEventObjectOnly )	)
		{
			// ����
			// do nothing..
		}
		else
		{
			// ����
			MD_SetErrorMessage("ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , FALSE )\n" , strFilename );
			return FALSE;
		}

		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST ,  
			pWnd->m_strSubDataMainDir , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_ArtistOnly )	)
		{
			// ���� 
		}
		else
		{
			// ����..
			MD_SetErrorMessage( "ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s , TRUE )\n" , strFilename);
			return FALSE;
		}

		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_DESIGN ,  
			pWnd->m_strSubDataMainDir , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, __ObjectSaveCallback_DesignerOnly )	)
		{
			// ���� 
		}
		else
		{
			// ����..
			MD_SetErrorMessage( "ERROR : g_pcsApmObject->StreamWrite( bbox , 0 , %s, TRUE )\n" , strFilename );
			return FALSE;
		}

	}

	return TRUE;
}

BOOL __ObjectLoadCallback_AdjustPosition(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData	);
	ASSERT( NULL != pClass	);
	
	if( NULL == pData ) return FALSE;

	ApdObject	*		pcsApdObject = ( ApdObject * ) pData;
	char		*		pFileName	= ( char * ) pClass;

	// ����� �ε����� ����..
	UINT32				uDivisionIndex;
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( pFileName , drive, dir, fname, ext );
	uDivisionIndex = atoi( fname + 4 );

	// ����� ��ŸƮ �������� ����..
	FLOAT	fStartX , fStartZ;

	fStartX	= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( uDivisionIndex ) ) );
	fStartZ	= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( uDivisionIndex ) ) );

	// ����� ���� üũ..
	// �����̸� �׳� ����..
	if(	fStartX < pcsApdObject->m_stPosition.x && pcsApdObject->m_stPosition.x < fStartX + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH &&
		fStartZ < pcsApdObject->m_stPosition.z && pcsApdObject->m_stPosition.z < fStartZ + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH	)
	{
		// ������~
		// do nothing
	}
	else
	{
		// �㱸��!
		// ���� �Ű� �����~..

		// ������ġ�� ���..
		UINT32 nDivisionIndexOriginal = GetDivisionIndexF( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z	);

		pcsApdObject->m_stPosition.x	-= GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndexOriginal ) ) );
		pcsApdObject->m_stPosition.z	-= GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndexOriginal ) ) );


		pcsApdObject->m_stPosition.x	+= fStartX;
		pcsApdObject->m_stPosition.z	+= fStartZ;

		TRACE( "�̻�Ϸ�\n" );
	}

	{
		// ������ (2005-09-08 ���� 5:03:58) : 
		// �ε��� ���� ���� �����Ѵ�.
		// �ε��� ������ ���Ʒ��¿� 1 ���� ��ŭ�� ������ �ε��� üũ�Ѵ�.

		int x1 = ALEF_LOAD_RANGE_X1 , x2 = ALEF_LOAD_RANGE_X2 ,
			z1 = ALEF_LOAD_RANGE_Y1 , z2 = ALEF_LOAD_RANGE_Y2;

		FLOAT	_fStartX;
		FLOAT	_fStartZ;
		FLOAT	_fEndX	;
		FLOAT	_fEndZ	;

		x1 -= 1;
		z1 -= 1;
		x2 += 1;
		z2 += 1;

		_fStartX	= GetSectorStartX	( ArrayIndexToSectorIndexX( x1 ) );
		_fStartZ	= GetSectorStartZ	( ArrayIndexToSectorIndexZ( z1 ) );
		_fEndX		= GetSectorStartX	( ArrayIndexToSectorIndexX( x2 ) );
		_fEndZ		= GetSectorStartZ	( ArrayIndexToSectorIndexZ( z2 ) );

		if( _fStartX <= pcsApdObject->m_stPosition.x	&&
			pcsApdObject->m_stPosition.x <= _fEndX		&&
			_fStartZ <= pcsApdObject->m_stPosition.z	&&
			pcsApdObject->m_stPosition.z <= _fEndZ		)
		{
			// do nothing
		}
		else
		{
			// ���� �ʰ�..
			return FALSE;
		}
	}
	
	if(	g_pcsApmEventManager->GetEvent( pcsApdObject , ( ApdEventFunction ) APDEVENT_FUNCTION_SPAWN		)	||
		g_pcsApmEventManager->GetEvent( pcsApdObject , ( ApdEventFunction ) APDEVENT_FUNCTION_NATURE	)	)
	{
		// 
		// ������Ʈ�� �������� �÷� ����..
		pcsApdObject->m_stPosition.y = AGCMMAP_THIS->GetHeight( pcsApdObject->m_stPosition.x , pcsApdObject->m_stPosition.z , SECTOR_MAX_HEIGHT );
	}

	// ������Ʈ �ߺ� �˻�.
	// ������Ʈ �� �˻��Ͽ�
	// TID �� ���� , �������� ���� ���� ������
	// �߰����� �ʴ´�.
	// ��� �̺�Ʈ�� �پ� �ִ� �Ͱ� ���� ���� ������
	// ���� ���� �����Ѵ�.
	{
		INT32			lIndex = 0		;
		ApdObject	*	pcsObject		;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject												;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			if( pcsObject->m_lTID			== pcsApdObject->m_lTID			&&
				pcsObject->m_stPosition.x	== pcsApdObject->m_stPosition.x	&&
				pcsObject->m_stPosition.y	== pcsApdObject->m_stPosition.y	&&
				pcsObject->m_stPosition.z	== pcsApdObject->m_stPosition.z	&&
				pcsObject->m_fDegreeX		== pcsApdObject->m_fDegreeX		&& 
				pcsObject->m_fDegreeY		== pcsApdObject->m_fDegreeY		 
				)
			{
				// TID�� ���� ��ġ�� ������..
				// ���� ������Ʈ��� ���� ��..

				if( g_pcsApmEventManager->CheckEventAvailable( pcsObject ) )
				{
					return FALSE;
				}
				else
				{
					// �����ִ°ſ� �̺�Ʈ�� ������...
					// �ε� �� �༮���� �ֳ�?
					if( g_pcsApmEventManager->CheckEventAvailable( pcsApdObject ) )
					{
						// ���� ������Ʈ ������ �߰��Ѵ�.
						g_pcsApmObject->DeleteObject( pcsObject , TRUE , TRUE );
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
		}

		// �ߺ� �Ŵ°� ����..
	}

	return TRUE;
}

BOOL	__DivisionObjectLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	DivisionCallbackResult	* pResult = ( DivisionCallbackResult * ) pData;

	ASSERT( NULL != pResult );

	pResult->nDivisionCount++;

	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	// OBDN : Object Division Normal
	// OBDS : Object Division Static

	AuBOX bbox;
	bbox.inf.x	=	pDivisionInfo->fStartX;
	bbox.inf.z	=	pDivisionInfo->fStartZ;
	bbox.inf.y	=	0.0f;
	bbox.sup.x	=	pDivisionInfo->fStartX + pDivisionInfo->fWidth;
	bbox.sup.z	=	pDivisionInfo->fStartZ + pDivisionInfo->fWidth;
	bbox.sup.y	=	0.0f;

	CFileFind	ff;

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_NORMAL ,  
		GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition )	);
	}
	else
	{
		// ��� �׳� �����Ѵ�.
		return TRUE;
	}

	pResult->nSuccessCount++;

	return TRUE;
}

// ������ (2004-07-05 ���� 6:59:53) : ������..
BOOL	__DivisionObjectLoadCallback2 ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	DivisionCallbackResult	* pResult = ( DivisionCallbackResult * ) pData;

	ASSERT( NULL != pResult );

	pResult->nDivisionCount++;

	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	// OBDN : Object Division Normal
	// OBDS : Object Division Static

	AuBOX bbox;
	bbox.inf.x	=	pDivisionInfo->fStartX;
	bbox.inf.z	=	pDivisionInfo->fStartZ;
	bbox.inf.y	=	0.0f;
	bbox.sup.x	=	pDivisionInfo->fStartX + pDivisionInfo->fWidth;
	bbox.sup.z	=	pDivisionInfo->fStartZ + pDivisionInfo->fWidth;
	bbox.sup.y	=	0.0f;

	CFileFind	ff;
	INT32		nSuccessCount = 0;
	UINT32		uDivision	= GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ );

	INT32		nTryCount = 0;

	// 2005/01/13
	// �����ο�Ҵ� �ε��� ������ ���� �ʰ� �ٷ� �о���δ�..
	// ������Ʈ�� ���� ���ɼ��� �ֱ������� ����� �̰��ּ�å..
	// �Ȱ�ġ�� �ϱ� ���ؼ� �ٸ��ͺ��� ���� ����..

	char strMessage[ 256 ];

	nTryCount++;
	wsprintf( strMessage , "(Div %04d) ������ ������Ʈ ���� �ε�" , uDivision );
	_ProgressCallback( strMessage , pResult->dlgProgress.m_nCurrent + 1 , pResult->dlgProgress.m_nTarget , ( void * ) &pResult->dlgProgress );

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_DESIGN , uDivision );
	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition , FALSE	) 	); // �ε��� ���� FALSE
		nSuccessCount++;
	}
	else
	{
		MD_SetErrorMessage( "(Div %04d) ������ ������Ʈ ���� �� �����." , uDivision );
	}

	nTryCount++;
	wsprintf( strMessage , "(Div %04d) ��� ������Ʈ ���� �ε�" , uDivision );
	_ProgressCallback( strMessage , pResult->dlgProgress.m_nCurrent + 1 , pResult->dlgProgress.m_nTarget , ( void * ) &pResult->dlgProgress );

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_NORMAL , uDivision );
	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition , TRUE	) );
		nSuccessCount++;
	}
	else
	{
		MD_SetErrorMessage( "(Div %04d) ��� ������Ʈ ���� �� �����." , uDivision );
	}

	nTryCount++;
	wsprintf( strMessage , "(Div %04d) ��Ƽ��Ʈ ������Ʈ ���� �ε�" , uDivision );
	_ProgressCallback( strMessage , pResult->dlgProgress.m_nCurrent + 1 , pResult->dlgProgress.m_nTarget , ( void * ) &pResult->dlgProgress );

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_ARTIST , uDivision );
	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition , TRUE	) 	);
		nSuccessCount++;
	}
	else
	{
		MD_SetErrorMessage( "(Div %04d) ��Ƽ��Ʈ ������Ʈ ���� �� �����." , uDivision );
	}

	// ������ (2005-05-19 ���� 10:53:17) : 
	// ���Ͼ��ٰ� ���� �Ű�Ⱦ������.
	/*
	if( nSuccessCount == nTryCount )
	{
		pResult->nSuccessCount++;
	}
	*/
	pResult->nSuccessCount++;

	return TRUE;
}


BOOL CUITileList_ObjectTabWnd::SaveTree(FILE *pFile, HTREEITEM root, int level)
{
	// ������ (2003-10-28 ���� 3:47:57) : ��!
	ASSERT( !"�������� ���̺� �Ұ�!" );
	return FALSE;

	ASSERT( NULL != pFile );

	HTREEITEM child;
	stObjectEntry *pObject;

	if( root != TVI_ROOT )
	{
		pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( root );
		if( !pObject->tid )
			fprintf( pFile, "%d|%s\n", level, pObject->name );
	}

	for( child = m_wndTreeCtrl.GetChildItem( root ) ; child ; child = m_wndTreeCtrl.GetNextSiblingItem( child ) )
	{
		SaveTree( pFile, child, level + 1 );
	}

	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::SaveScript( BOOL bTemplateSave , BOOL bScriptSave )
{
	char	filename[ 1024 ];

	if( bTemplateSave )
	{
		// ������ (2003-10-28 ���� 3:47:57) : ��!
		ASSERT( !"�������� ���̺� �Ұ�!" );
		MessageBox( "������Ʈ ���ø��� �������� ���̺� �ϽǼ� ���ɵ�" , "������Ʈ" , MB_ICONERROR | MB_OK );
		/*
		wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , OBJECT_FILE );
		FILE	*pFile = fopen( filename, "wt" );

		ASSERT( NULL != pFile && "CUITileList_ObjectTabWnd::SaveScript" );

		if( pFile == NULL )
		{
			return FALSE;
		}

		SaveTree( pFile, TVI_ROOT, -1 );

		fclose( pFile );

		// ������ (2003-01-20 ���� 10:16:40) : ������Ʈ ���ø��� ���� ���̺����� �ʴ´�. ���������� �����ϴ�.
		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ObjectTemplate.ini"	);
		VERIFY( g_pcsApmObject->StreamWriteTemplate(filename)	);
		*/
	}

	if( bScriptSave )
	{
		//wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "Object.ini"			);
		//VERIFY( g_pcsApmObject->StreamWrite(filename, FALSE)			);
		//wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ObjectStatic.ini"	);
		//VERIFY( g_pcsApmObject->StreamWrite(filename, TRUE)			);

		GetSubDataDirectory( m_strSubDataMainDir );

		_CreateDirectory( m_strSubDataMainDir );
		_CreateDirectory( "%s\\Map"							, m_strSubDataMainDir );
		_CreateDirectory( "%s\\Map\\Data"					, m_strSubDataMainDir );
		_CreateDirectory( "%s\\Map\\Data\\Object"			, m_strSubDataMainDir );
		_CreateDirectory( "%s\\Map\\Data\\Object\\Artist"	, m_strSubDataMainDir );
		_CreateDirectory( "%s\\Map\\Data\\Object\\Design"	, m_strSubDataMainDir );

		VERIFY( AGCMMAP_THIS->EnumLoadedDivision( __DivisionObjectSaveCallback , ( PVOID ) this ) );

		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "TeleportGroup.ini"		);
		if( !g_pcsAgpmEventTeleport->StreamWriteGroup(filename , FALSE )		)
		{
			MD_SetErrorMessage( "g_pcsAgpmEventTeleport->StreamWriteGroup(%s , FALSE )" , filename );
		}

		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "TeleportPoint.ini"		);
		if( !g_pcsAgpmEventTeleport->StreamWritePoint(filename , FALSE )		)
		{
			MD_SetErrorMessage( "g_pcsAgpmEventTeleport->StreamWritePoint(%s , FALSE )" , filename );
		}

		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "SpawnGroup.ini"		);
		if( !g_pcsAgpmEventSpawn->StreamWriteGroup(filename)		)
		{
			MD_SetErrorMessage( "g_pcsAgpmEventSpawn->StreamWriteGroup(%s)" , filename );
		}

		//wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "AITemplate.ini"		);
		//VERIFY( g_pcsAgpmAI->StreamWriteTemplate(filename)		);

		//wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "AI2Template.ini"		);
		//if( !g_pcsAgpmAI2->StreamWriteTemplate(filename)		)
		//{
		//	MD_SetErrorMessage( "g_pcsAgpmAI2->StreamWriteTemplate(%s)" , filename );
		//}

//		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "DropItemTemplate.ini"		);
//		VERIFY( g_pcsAgpmDropItem->StreamWriteTemplate(filename)		);

		wsprintf( filename , "%s\\Ini\\%s" , ALEF_CURRENT_DIRECTORY , "ShrineTemplate.ini"	);
		if( !g_pcsAgpmShrine->StreamWriteTemplate(filename)	)
		{
			MD_SetErrorMessage( "g_pcsAgpmShrine->StreamWriteTemplate(%s)" , filename );
		}
	}

	return TRUE;
}

void CUITileList_ObjectTabWnd::OnDestroy() 
{
	HTREEITEM		item;

	for( item = m_wndTreeCtrl.GetRootItem() ; item ; item = m_wndTreeCtrl.GetRootItem() )
	{
		DeleteEntry( item );
	}

	CWnd::OnDestroy();
}

/*
RpClump * CUITileList_ObjectTabWnd::LoadDFF(const char *file)
{
	ASSERT( NULL != file );

	char	str[ 1024 ];
	wsprintf( str ,"%s\\Object" , ALEF_CURRENT_DIRECTORY );
	SetCurrentDirectory( str );

	char location[1024];
	RwStream	*	stream	= ( RwStream *	) NULL;
	RpClump		*	clump	= ( RpClump *	) NULL;
	RwChar		*	path	= ( RwChar *	) NULL;

	sprintf( location , "%s" , file );

	path = RsPathnameCreate(location);

	ASSERT( NULL != path );

	stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, path);

	ASSERT( NULL != stream );

	if( stream )
	{
		if( RwStreamFindChunk(stream, rwID_CLUMP,
							  (RwUInt32 *)NULL, (RwUInt32 *)NULL) )
		{
			clump = RpClumpStreamRead(stream);

			RwStreamClose (stream, NULL);
		}
	}

	RsPathnameDestroy(path);
	return clump;
}
*/

BOOL CUITileList_ObjectTabWnd::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	LPNMHDR	phnm = (LPNMHDR) lParam;

	switch( phnm->code )
	{
	case NM_RCLICK:
		{
			{
				// ����� �� �༮�� ���� �Ǿ� �ִ��� ����.
				HTREEITEM		item;
				stObjectEntry	*pObject;
				ApdObjectTemplate *		pcsApdTemplate;
				//AgcdObjectTemplate *	pcsAgcdTemplate;

				item = m_wndTreeCtrl.GetSelectedItem();
				if( !item )
					break; // ���� �Ȱ� ����.

				pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

				if( !pObject )
					break; // ������Ʈ�� �ƴϴ�.

				pcsApdTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);
				if (!pcsApdTemplate)
					break; // ���ø��� ����.
			}

			POINT point;
			CMenu *pMenu;
			CMenu menu;

			GetCursorPos( &point );

			menu.LoadMenu(IDR_OBJECTMENU);
			pMenu = menu.GetSubMenu(0);

			TrackPopupMenu( pMenu->GetSafeHmenu() , TPM_TOPALIGN | TPM_LEFTALIGN ,
				point.x , point.y , 0 , GetSafeHwnd() , NULL );
		}
		break;

	default:
		break;
	}//switch( phnm->code )

	return CWnd::OnNotify(wParam, lParam, pResult);
}

void CUITileList_ObjectTabWnd::OnObjectAddentry() 
{
	MessageBox( "������Ʈ ���ø��� �����Ҽ� �����ϴ�. ������ �̿��ϼ���." );

	return;
/*
// ������ (2003-01-20 ���� 10:18:23) : ���ø� ���� ����..

	CObjectDialog	dlg;
	HTREEITEM		item;
	int				tid = 0;

	item = m_wndTreeCtrl.GetSelectedItem();
	
	if( item == NULL && m_wndTreeCtrl.GetCount() )
	{
		MessageBox( "��Ʈ���� �����ϼ���!" , "����" , MB_ICONERROR );
		return;
	}

	if( dlg.DoModal() == IDOK )
	{
		ApdObjectTemplate *		pcsApdObjectTemplate	= NULL;
		AgcdObjectTemplate *	pcsAgcdObjectTemplate	= NULL;
		RwRGBA	stColor = {dlg.m_nObjectRed, dlg.m_nObjectGreen, dlg.m_nObjectBlue, dlg.m_nObjectAlpha};
		char	str[ 1024 ];
		wsprintf( str ,"%s" , ALEF_CURRENT_DIRECTORY );
		SetCurrentDirectory( str );

		if( dlg.m_strFile.GetLength() )
		{
			pcsApdObjectTemplate = AddTemplate( (LPCTSTR) dlg.m_strName , dlg.m_strFile , NULL , AUBLOCKING_TYPE_BOX , item );
			if (pcsApdObjectTemplate)
			{
				AddEntry( pcsApdObjectTemplate->m_lID, (LPCTSTR) dlg.m_strName , dlg.m_strFile.GetLength() ? (LPCTSTR) dlg.m_strFile : NULL , NULL , item ? item:TVI_ROOT , NULL );

				VERIFY( pcsAgcdObjectTemplate = g_pcsAgcmObject->GetTemplateData(pcsApdObjectTemplate) );
				
				AgcdObjectTemplateGroupList *pstOTGList = pcsAgcdObjectTemplate->m_stGroup.m_pstList;
				// ����Ʈ�� ���� ������ ����.
				while(pstOTGList)
				{
//					if (pcsAgcdObjectTemplate->m_pstClump)
					if (pstOTGList->m_csData.m_pstClump)
					{
						RpClumpForAllAtomics(pcsAgcdObjectTemplate->m_pstClump, SetClumpInfo, &dlg);

						if (dlg.m_bBlocking)
						{
							pcsAgcdObjectTemplate->m_lObjectType |= ACUOBJECT_TYPE_BLOCKING;
							pcsApdObjectTemplate->m_nObjectType |= APDOBJECT_TYPE_BLOCKING;
						}
						else
						{
							pcsAgcdObjectTemplate->m_lObjectType &= pcsAgcdObjectTemplate->m_lObjectType ^ ACUOBJECT_TYPE_BLOCKING;
							pcsApdObjectTemplate->m_nObjectType &= pcsApdObjectTemplate->m_nObjectType ^ APDOBJECT_TYPE_BLOCKING;
						}

						if (dlg.m_bRidable)
						{
							pcsAgcdObjectTemplate->m_lObjectType |= ACUOBJECT_TYPE_RIDABLE;
							pcsApdObjectTemplate->m_nObjectType |= APDOBJECT_TYPE_RIDABLE;
						}
						else
						{
							pcsAgcdObjectTemplate->m_lObjectType &= pcsAgcdObjectTemplate->m_lObjectType ^ ACUOBJECT_TYPE_RIDABLE;
							pcsApdObjectTemplate->m_nObjectType &= pcsApdObjectTemplate->m_nObjectType ^ APDOBJECT_TYPE_RIDABLE;
						}

						if (dlg.m_bNoCameraAlpha)
						{
							pcsAgcdObjectTemplate->m_lObjectType |= ACUOBJECT_TYPE_NO_CAMERA_ALPHA;
						}
						else
						{
							pcsAgcdObjectTemplate->m_lObjectType &= pcsAgcdObjectTemplate->m_lObjectType ^ ACUOBJECT_TYPE_NO_CAMERA_ALPHA;
						}

						g_pcsAgcmObject->UpdateAmbient(pcsAgcdObjectTemplate, dlg.m_bAmbient);
						g_pcsAgcmObject->UpdateAlpha(pcsAgcdObjectTemplate, dlg.m_bAlpha);

						VERIFY( g_pcsAgcmObject->SaveClump(pcsAgcdObjectTemplate->m_pstClump, pcsAgcdObjectTemplate->m_szDFFName) && "������Ʈ Ŭ���� ���̺� ����");
					}

					// ���� ����Ʈ�� �ѱ��
					pstOTGList = pstOTGList->m_pstNext;
				}
			}
			else
			{
				// ����!..
				MessageBox( "������Ʈ�� ������ �ֽ��ϴ�. ClumpView���� Ȯ���Ͻðų� �ٽ� �ͽ���Ʈ�ϼ���." , "����" , MB_ICONERROR );
				return;
			}
		}
		else
		{
			// ī�װ��� �߰���..
			if( dlg.m_strName.IsEmpty() )
			{
				MessageBox( "ī�װ� �̸��� �������ּ���." , "����" , MB_ICONERROR );
			}
			else
			{
				AddEntry( 0 , (LPCTSTR) dlg.m_strName , NULL , NULL , item ? item:TVI_ROOT , NULL );
			}
		}
		
		// �䶧 �������� ����.

		//SaveScript();
	}
*/
}

void CUITileList_ObjectTabWnd::OnObjectDeleteentry() 
{
	MessageBox( "������Ʈ ���ø��� �����Ҽ� �����ϴ�. ������ �̿��ϼ���." );

	return;

	stObjectEntry	*pObject;
	HTREEITEM		item;

	item = m_wndTreeCtrl.GetSelectedItem();
	if( !item )
		return;

	pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	ASSERT( NULL != pObject );
	
	CString	str;
	str.Format( "'%s' ���ø��� �����Ͻǲ�����? -_-" , pObject->name);

	if( IDYES == MessageBox( str , "���ø� ����" , MB_ICONWARNING | MB_YESNO ) )
	{
//		if( pObject->tid )
//			DeleteTemplate( pObject->tid );

		DeleteEntry( m_wndTreeCtrl.GetSelectedItem() );

		// ���� �䶧 �������� ����..
		//SaveScript();
	}
}

void CUITileList_ObjectTabWnd::OnObjectEditentry() 
{
	MessageBox( "������Ʈ ���ø��� ���� �Ҽ� �����ϴ�. ������ �̿��ϼ���." );

	return;
/*

	CObjectDialog	dlg;
	HTREEITEM		item;
	stObjectEntry	*pObject;
	ApdObjectTemplate *		pcsApdTemplate;
	AgcdObjectTemplate *	pcsAgcdTemplate;

	item = m_wndTreeCtrl.GetSelectedItem();
	if( !item )
		return;

	pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	ASSERT( NULL != pObject );

	pcsApdTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);
	if (!pcsApdTemplate)
		return;

	pcsAgcdTemplate = g_pcsAgcmObject->GetTemplateData(pcsApdTemplate);

	dlg.m_strName			= pObject->name;
	dlg.m_strFile			= pObject->file;
//	dlg.m_bAmbient			= pcsAgcdTemplate->m_bUseAmbient;
//	dlg.m_bAlpha			= pcsAgcdTemplate->m_bUseAlpha;
	dlg.m_bRidable			= (pcsAgcdTemplate->m_lObjectType  & ACUOBJECT_TYPE_RIDABLE) ? TRUE : FALSE;
	dlg.m_bBlocking			= (pcsAgcdTemplate->m_lObjectType  & ACUOBJECT_TYPE_BLOCKING) ? TRUE : FALSE;
	dlg.m_bNoCameraAlpha	= (pcsAgcdTemplate->m_lObjectType  & ACUOBJECT_TYPE_NO_CAMERA_ALPHA) ? TRUE : FALSE;

	if (pObject->pClump)
		RpClumpForAllAtomics(pObject->pClump, GetClumpInfo, &dlg);

	if( dlg.DoModal() == IDOK )
	{
		if( dlg.m_strFile.GetLength() && dlg.m_strFile.CompareNoCase( pObject->file ) )
		{
			pcsApdTemplate = EditTemplate( pObject->tid , dlg.m_strName , dlg.m_strFile , NULL );
		}
		else
		{
			pcsApdTemplate = EditTemplate( pObject->tid , dlg.m_strName , NULL , NULL );
		}

		pObject->pClump = NULL;

		if (pcsApdTemplate)
		{
			pcsAgcdTemplate = g_pcsAgcmObject->GetTemplateData(pcsApdTemplate);

			pObject->pClump = pcsAgcdTemplate->m_pstClump;
			RpClumpForAllAtomics(pcsAgcdTemplate->m_pstClump, SetClumpInfo, &dlg);

			if (dlg.m_bBlocking)
			{
				pcsAgcdTemplate->m_lObjectType |= ACUOBJECT_TYPE_BLOCKING;
				pcsApdTemplate->m_nObjectType |= APDOBJECT_TYPE_BLOCKING;
			}
			else
			{
				pcsAgcdTemplate->m_lObjectType &= pcsAgcdTemplate->m_lObjectType ^ ACUOBJECT_TYPE_BLOCKING;
				pcsApdTemplate->m_nObjectType &= pcsApdTemplate->m_nObjectType ^ APDOBJECT_TYPE_BLOCKING;
			}

			if (dlg.m_bRidable)
			{
				pcsAgcdTemplate->m_lObjectType |= ACUOBJECT_TYPE_RIDABLE;
				pcsApdTemplate->m_nObjectType |= APDOBJECT_TYPE_RIDABLE;
			}
			else
			{
				pcsAgcdTemplate->m_lObjectType &= pcsAgcdTemplate->m_lObjectType ^ ACUOBJECT_TYPE_RIDABLE;
				pcsApdTemplate->m_nObjectType &= pcsApdTemplate->m_nObjectType ^ APDOBJECT_TYPE_RIDABLE;
			}

			if (dlg.m_bNoCameraAlpha)
			{
				pcsAgcdTemplate->m_lObjectType |= ACUOBJECT_TYPE_NO_CAMERA_ALPHA;
			}
			else
			{
				pcsAgcdTemplate->m_lObjectType &= pcsAgcdTemplate->m_lObjectType ^ ACUOBJECT_TYPE_NO_CAMERA_ALPHA;
			}

			g_pcsAgcmObject->UpdateAmbient(pcsAgcdTemplate, dlg.m_bAmbient);
			g_pcsAgcmObject->UpdateAlpha(pcsAgcdTemplate, dlg.m_bAlpha);
			g_pcsAgcmObject->UpdateObjectType(pcsAgcdTemplate);

			g_pcsAgcmObject->SaveClump(pcsAgcdTemplate->m_pstClump, (LPSTR) (LPCTSTR)dlg.m_strFile);
		}

		EditEntry( item , (LPCTSTR) dlg.m_strName , dlg.m_strFile.GetLength() ? (LPCTSTR) dlg.m_strFile:NULL );

		// ���� ���� �������� ����..
		// SaveScript();
	}
*/
}

HTREEITEM CUITileList_ObjectTabWnd::AddEntry(unsigned long tid, const char *name, const char *file, RpClump *pClump, HTREEITEM entry, RwRGBA *pstColor)
{
	ASSERT( NULL != name	);
	//ASSERT( NULL != file	);
	//ASSERT( NULL != pClump	);

	stObjectEntry *pObject = new stObjectEntry;
	char strName[256];
	HTREEITEM item;

	ASSERT( NULL != pObject );

	pObject->tid = tid;

	strncpy( pObject->name, name, 256 );
	pObject->name[255] = 0;

	strcpy(strName, pObject->name);

	pObject->file[0] = 0;
	//pObject->pClump = pClump;
	if (pstColor)
		pObject->stPreLitLum = *pstColor;

	if( file )
	{
		strncpy( pObject->file, file, 256 );
		pObject->file[255] = 0;

		strcat(strName, " ( ");
		strcat(strName, pObject->file);
		strcat(strName, " )");
	}

	item = m_wndTreeCtrl.InsertItem( strName , entry , TVI_SORT );
	m_wndTreeCtrl.SetItemData(item, (DWORD) pObject);

	return item;
}

BOOL CUITileList_ObjectTabWnd::DeleteEntry(HTREEITEM item)
{
	if( !item )
		return FALSE;

	HTREEITEM child;
	HTREEITEM next;
	stObjectEntry	*pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	for( child = m_wndTreeCtrl.GetChildItem( item ) ; child ; child = next )
	{
		next = m_wndTreeCtrl.GetNextSiblingItem( child );

		DeleteEntry( child );
	}

	if( pObject )
	{
		if( pObject->tid )
			DeleteTemplate( pObject->tid );

		delete pObject;

		m_wndTreeCtrl.SetItemData( item , 0 );
	}

	m_wndTreeCtrl.DeleteItem( item );

	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::EditEntry(HTREEITEM item, const char *name, const char *file)
{
	ASSERT( NULL != name	);
	ASSERT( NULL != file	);

	char strName[256];
	stObjectEntry	*pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	ASSERT( NULL != pObject );

	strncpy( pObject->name , name , 256 );
	pObject->name[255] = 0;

	strcpy(strName, pObject->name);

	/*
	if( pObject->pClump )
		RpClumpDestroy( pObject->pClump );
	*/

	pObject->file[0] = 0;

	if( file )
	{
		strncpy( pObject->file , file , 256 );
		pObject->file[255] = 0;

		strcat(strName, " ( ");
		strcat(strName, pObject->file);
		strcat(strName, " )");
	}

	m_wndTreeCtrl.SetItemText( item, strName );

	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::TemplateCreateCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	HTREEITEM category;
	CUITileList_ObjectTabWnd *	pThis					= (CUITileList_ObjectTabWnd *) pClass;
	ApdObjectTemplate *			pstApdObjectTemplate	= (ApdObjectTemplate *) pData;
	AgcdObjectTemplate *		pstAgcdObjectTemplate	= g_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);
	RwRGBA						stColor = {0,0,0,255};

	ASSERT( NULL != pThis					);
	ASSERT( NULL != pstApdObjectTemplate	);
	ASSERT( NULL != pstAgcdObjectTemplate	);

	category = pThis->SearchItemText(pstAgcdObjectTemplate->m_szCategory);
	if( !category )
		category = TVI_ROOT;

	AgcdObjectTemplateGroupList *pstOTGList = pstAgcdObjectTemplate->m_stGroup.m_pstList;

	ASSERT( NULL != pstOTGList );
	// ������ (2003-11-05 ���� 11:08:45) : �׷� ����Ʈ�� ���Ƽ� ó�� ������ �־���ϴµ�..
	//										�̰��� ��� ���� �𸣰���..

	pThis->AddEntry(
		pstApdObjectTemplate->m_lID, pstApdObjectTemplate->m_szName, 
		pstOTGList->m_csData.m_pszDFFName	, 
		pstOTGList->m_csData.m_pstClump		,
		category , &stColor					);

	if( pThis->m_nMaxTID	<=	( int ) pstApdObjectTemplate->m_lID )
		pThis->m_nMaxTID	=	( int ) pstApdObjectTemplate->m_lID + 1;

	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::ObjectCreateCB(PVOID pData, PVOID pClass, PVOID pCustData)
{
	CUITileList_ObjectTabWnd * pThis = ( CUITileList_ObjectTabWnd * ) pClass;

	ApdObject	*pstApdObject  = (ApdObject*) pData;
	//AgcdObject	*pstAgcdObject = g_pcsAgcmObject->GetObjectData(pstApdObject);

	ASSERT( NULL != pThis			);
	ASSERT( NULL != pstApdObject	);

	if( pThis->m_nMaxOID <= ( int ) pstApdObject->m_lID )
		pThis->m_nMaxOID = ( int ) pstApdObject->m_lID + 1;

	return TRUE;
}


HTREEITEM CUITileList_ObjectTabWnd::SearchItemText(const char *strName, HTREEITEM root)
{
	CCategory *pstCategory;
	for( int i = 0 ; i < ( int ) m_listCategory.size(); i ++ )
	{
		pstCategory = &m_listCategory[ i ];
		if( !strcmp( pstCategory->str , strName ) )
		{
			return pstCategory->pos;
		}
	}

	/*
	ASSERT( NULL != strName		);

	HTREEITEM child	;
	HTREEITEM item	;

	if( root != TVI_ROOT && !strcmp( strName, m_wndTreeCtrl.GetItemText(root) ) )
		return root;

	for( child = m_wndTreeCtrl.GetChildItem( root ) ; child ; child = m_wndTreeCtrl.GetNextSiblingItem( child ) )
	{
		item = SearchItemText( strName, child );
		if( item )
			return item;
	}
	*/

	return NULL;
}

ApdObject *	CUITileList_ObjectTabWnd::AddObject( RwV3d *pos , UINT32 uAddtionalFlag , FLOAT fDegreeY )
{
	//RpClump				*	pClump					;
	HTREEITEM				item					= m_wndTreeCtrl.GetSelectedItem();
	stObjectEntry		*	pObject					;
	ApdObjectTemplate	*	pstApdObjectTemplate	;
	//RwInt32					nIndex					;
	//RwFrame				*	frame					;
	
	ASSERT( NULL != item && "���É� �������� �����ϴ�." );

	if( !item )
		return NULL;

	pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	ASSERT( pObject != NULL );
	if( NULL == pObject )
	{
		MD_SetErrorMessage( "����Ʈ���� - ������Ʈ ���� ��� ����\n" );
		return NULL;
	}

	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);
	if( !pstApdObjectTemplate )
	{
		MD_SetErrorMessage( "���ø� ID���� ���ø� ���� ��� ����..\n" );
		return NULL;
	}

	return AddObject( pstApdObjectTemplate , pos , uAddtionalFlag , fDegreeY );
}

ApdObject *	 CUITileList_ObjectTabWnd::AddObject( ApdObjectTemplate	* pstApdObjectTemplate , RwV3d *pos , UINT32 uAddtionalFlag , FLOAT fDegreeY )
{
	ApdObject			*	pstApdObject			;
	AgcdObject			*	pstAgcdObject			;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	;
	pstAgcdObjectTemplate = g_pcsAgcmObject->GetTemplateData( pstApdObjectTemplate );

	ASSERT( pstAgcdObjectTemplate != NULL );
	if( !pstAgcdObjectTemplate )
	{
		return NULL;
	}

	//AgcdObjectTemplateGroupList *pstOTGList = pstAgcdObjectTemplate->m_stGroup.m_pstList;
	//pClump = pstOTGList->m_csData.m_pstClump;

	pstApdObject = g_pcsApmObject->AddObject( 
		g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( pos->x , pos->z ) ) , pstApdObjectTemplate->m_lID );
	ASSERT( NULL != pstApdObject );
	if( !pstApdObject )
	{
		MD_SetErrorMessage( "g_pcsApmObject->AddObject ������Ʈ �߰�����.." );
		return NULL;
	}

	// ������Ʈ ��ŷ �߰�..
	AuBLOCKING			stBlockInfoCalcurated[APDOBJECT_MAX_BLOCK_INFO];		// Blocking Info
	pstApdObject->CalcWorldBlockInfo( pstApdObjectTemplate->m_nBlockInfo , pstApdObjectTemplate->m_astBlockInfo , stBlockInfoCalcurated );
	g_pcsApmMap->AddObjectBlocking( stBlockInfoCalcurated , pstApdObject->m_nBlockInfo );

	pstAgcdObject = g_pcsAgcmObject->GetObjectData(pstApdObject);

	ASSERT( pstAgcdObject != NULL );
	if( !pstAgcdObject )
	{
		MD_SetErrorMessage( "g_pcsAgcmObject->GetObjectData ������Ʈ Ŭ���̾�Ʈ ���� ��� ����" );
		return NULL;
	}

	// �÷��� ����..
	pstAgcdObject->m_lObjectType = ACUOBJECT_TYPE_OBJECT | uAddtionalFlag;

	//pstApdObject->m_stBlockInfo.type = pstApdObjectTemplate->m_stBlockInfo.type;

	pstApdObject->m_stScale.x		= 1.0f		;
	pstApdObject->m_stScale.y		= 1.0f		;
	pstApdObject->m_stScale.z		= 1.0f		;

	pstApdObject->m_stPosition.x	= pos->x	;
	pstApdObject->m_stPosition.y	= pos->y	;
	pstApdObject->m_stPosition.z	= pos->z	;

	pstApdObject->m_fDegreeX		= 0.0f		;
	pstApdObject->m_fDegreeY		= fDegreeY	;

	if( g_pcsApmObject->UpdateInit( pstApdObject ) )
	{
		// ����� �׽�Ʈ
		BOOL __bTestOn = FALSE;
		if( __bTestOn )
		{
				// ó���༮�� üũ�Ѵ�..
			RpAtomic * __TestAtomicCallback(RpAtomic *atomic, void *data);

			RpClump * pClump = pstAgcdObject->m_pstTemplate->m_stGroup.m_pstList->m_csData.m_pstClump;

			RpClumpForAllAtomics( pClump , __TestAtomicCallback , NULL );	

			__bTestOn = FALSE;
		}

		// ����.
		return pstApdObject;
	}
	else
	{
		MD_SetErrorMessage( "g_pcsApmObject->UpdateInit Ŭ���� ���� ����" );
		return NULL;
	}

	/*
	//memcpy( &pstApdObject->m_stBlockInfo , &pstApdObjectTemplate->m_stBlockInfo, sizeof(pstApdObject->m_stBlockInfo) );
iiii
	pstAgcdObject->m_pClump = RpClumpClone( pClump );
	frame = RpClumpGetFrame(pstAgcdObject->m_pClump);

	ASSERT( NULL != frame );

	AcuObject::SetClumpType(pstAgcdObject->m_pClump, ACUOBJECT_TYPE_OBJECT, pstApdObject->m_lID);
	/*
	nIndex = RwFrameAddUserDataArray(frame, "OID", rpINTUSERDATA, 1);
	if( nIndex != -1 )
	{
		RpUserDataArray *uda = RwFrameGetUserDataArray( RpClumpGetFrame(pstAgcdObject->m_pClump), nIndex );

		ASSERT( NULL != uda );

		RpUserDataArraySetInt( uda, 0, pstApdObject->m_lID );
	}
	* /

	g_pcsAgcmObject->SetRpWorld(g_pEngine->m_pWorld);

	RpWorldAddClump( g_pEngine->m_pWorld, pstAgcdObject->m_pClump );

	RwFrameTranslate( RpClumpGetFrame( pstAgcdObject->m_pClump ), pos, rwCOMBINEREPLACE );
	*/	
}

BOOL CUITileList_ObjectTabWnd::TransformObject(RpClump *pClump, RsMouseStatus *pstCurrMouse , FLOAT fDeltaX , FLOAT fDeltaY , RwV3d *end, int flag)
{
	ASSERT( NULL != pClump			);
	//ASSERT( NULL != pstCurrMouse	);
	//ASSERT( NULL != end				);

	float	dx	= ( fDeltaX ) * 0.005f;

	int oid;
	//int i;

	//RwV3d				scale			;
	//RwV3d				position		;
	RwV3d				pos				;

	RwFrame			*	pFrame			= RpClumpGetFrame( pClump );
	RwMatrix		*	pMatrix			= RwFrameGetLTM( pFrame );
	//RpUserDataArray	*	pUserDataArray	;
	RwV3d			*	camPos			= RwMatrixGetPos( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );
	RwV3d			*	camLookAt		= RwMatrixGetAt	( RwFrameGetLTM( RwCameraGetFrame( g_pEngine->m_pCamera ) ) );

	ApdObject		*	pstApdObject	;
	AgcdObject		*	pstAgcdObject	;
	AuPOS				posNext			;

	oid = 0;
	// Object ID�� ��.
	INT32	nType = AcuObject::GetClumpType( pClump, &oid );

	switch( AcuObject::GetType( nType ) )
	{
	case	ACUOBJECT_TYPE_OBJECT			:
		break;
	default:
		oid = 0;
		break;
	}
	
	// �ش� ������Ʈ�� �����͸� ��.
	pstApdObject	= g_pcsApmObject->	GetObject		( oid			);
	ASSERT( NULL != pstApdObject	);
	pstAgcdObject	= g_pcsAgcmObject->GetObjectData	( pstApdObject	);
	ASSERT( NULL != pstAgcdObject	);

	// pos �� ���� Object�� Y��ǥ(��, ����)�� Mouse Cursor�� ������ World ��ǥ�� ��Ÿ����.
	if( end )
	{
		pos.x = end->x;
		pos.z = end->z;
		pos.y = pstApdObject->m_stPosition.y;
	}
	else
	{
		pos.x = pstApdObject->m_stPosition.x;
		pos.z = pstApdObject->m_stPosition.z;
		pos.y = pstApdObject->m_stPosition.y;
	}

	posNext = pstApdObject->m_stPosition;

	if( pstCurrMouse && pstCurrMouse->control && pstCurrMouse->shift )
	// Ctrl + Shift �� Y�� Rotate�̴�.
	{
		pstApdObject->m_fDegreeY		+= fDeltaX;

		if( m_bRotateXAxis )
			pstApdObject->m_fDegreeX	+= fDeltaY;
	}
	else if( pstCurrMouse && pstCurrMouse->control )
	// Ctrl �� Scale�̴�.
	{
		pstApdObject->m_stScale.x += dx;
		pstApdObject->m_stScale.y += dx;
		pstApdObject->m_stScale.z += dx;
	}
	else if( pstCurrMouse && pstCurrMouse->shift )
	{
		// y�����θ� ������..
		// �ش� ��.. pstApdObject->m_stPosition
		
		RwV3d normalvector	;
		RwV3d CollisionPoint;

		// ���� ��ֺ���..
		normalvector.x	= - camLookAt->x;
		normalvector.y	= 0.0f			;
		normalvector.z	= - camLookAt->z;

		RwLine	CameraPixelRay		;

		RwCameraCalcPixelRay( g_pEngine->m_pCamera , &CameraPixelRay	, &pstCurrMouse->pos );
		
		_GetIntersectPos( 
			&pstApdObject->m_stPosition	,
			&normalvector				,
			&CameraPixelRay.start		,
			&CameraPixelRay.end			,
			&CollisionPoint				);

		pstApdObject->m_stPosition.y = CollisionPoint.y;
	}
	else
	{
		if( camPos->y != pos.y )
		{
			if( m_bMoveXAxis ) posNext.x = pos.x + (camPos->x - pos.x) * (pstApdObject->m_stPosition.y - pos.y) / (camPos->y - pos.y);
			if( m_bMoveZAxis ) posNext.z = pos.z + (camPos->z - pos.z) * (pstApdObject->m_stPosition.y - pos.y) / (camPos->y - pos.y);
		}
		else
		{
			if( m_bMoveXAxis ) posNext.x = pos.x;
			if( m_bMoveZAxis ) posNext.z = pos.z;
		}

		INT32	nDivisionIndex , nDivisionIndexNext;
		nDivisionIndex		= GetDivisionIndexF( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z );
		nDivisionIndexNext	= GetDivisionIndexF( posNext.x , posNext.z );
		//TRACE( "������Ʈ �̵� : %04d , %04d\n" , nDivisionIndex , nDivisionIndexNext );
		if( nDivisionIndex != nDivisionIndexNext )
		{
			TRACE( "������Ʈ�� ����� �ʸӷ� �ű�������\n" );
			return FALSE;
		}
		else
		{
			pstApdObject->m_stPosition	= posNext;
		}

		// �׸��忡 ����..
		if( m_bSnapToGrid )
		{
			ApWorldSector * pSector;
			pSector = g_pcsApmMap->GetSector( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z );

			if( pSector )
			{
				FLOAT	applyX , applyZ;
				applyX = ( pSector->GetStepSizeX() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pstApdObject->m_stPosition.x - pSector->GetXStart() ) / ( pSector->GetStepSizeX() / 2 ) ) );
				applyZ = ( pSector->GetStepSizeZ() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pstApdObject->m_stPosition.z - pSector->GetZStart() ) / ( pSector->GetStepSizeZ() / 2 ) ) );
				
				pstApdObject->m_stPosition.x = pSector->GetXStart() + applyX;
				pstApdObject->m_stPosition.z = pSector->GetZStart() + applyZ;
			}
		}

		// ������ ���̸� ����..
		if( m_bSnapToHeight )
		{
			pstApdObject->m_stPosition.y = AGCMMAP_THIS->GetHeight_Lowlevel_HeightOnly( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z );
		}

	}

	g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

	

	return TRUE;
}

ApdObjectTemplate *CUITileList_ObjectTabWnd::AddTemplate(const char *name, const char *file, RwRGBA *pstColor, int nBlockingType, HTREEITEM item)
{
	ASSERT( !"����ϸ� �ȉ�!" );
/*
	ASSERT( NULL != name	);
	ASSERT( NULL != file	);

	ApdObjectTemplate	*	pstApdObjectTemplate	= NULL;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	= NULL;
	stObjectEntry		*	pEntry					= NULL;

	if( item )
		pEntry = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );
	else
	{
		// ����!
		
	}

	pstApdObjectTemplate = g_pcsApmObject->AddObjectTemplate(++m_nMaxTID);

	ASSERT( NULL != pstApdObjectTemplate	);

	if (!pstApdObjectTemplate)
		return NULL;

	pstAgcdObjectTemplate = g_pcsAgcmObject->GetTemplateData(pstApdObjectTemplate);

	ASSERT( NULL != pstAgcdObjectTemplate	);

	strcpy(pstApdObjectTemplate->m_szName, name);

	//pstApdObjectTemplate->m_stBlockInfo.type = ( AuBlockingType ) nBlockingType;
	strcpy(pstAgcdObjectTemplate->m_szDFFName, file);
	pstAgcdObjectTemplate->m_pstClump = g_pcsAgcmObject->LoadClump(pstAgcdObjectTemplate->m_szDFFName);

	ASSERT( NULL != pstAgcdObjectTemplate->m_pstClump && "Ŭ������ ������ ����." );

	if (!pstAgcdObjectTemplate->m_pstClump)
		return NULL;

	memset(&pEntry->stPreLitLum, 0, sizeof(RwRGBA));
	if (pstColor)
	{
		AcuObject::SetClumpPreLitLim(pstAgcdObjectTemplate->m_pstClump, pstColor);
	}

	ASSERT( NULL != pstAgcdObjectTemplate->m_pstClump && "������Ʈ Ŭ���� �ε� ����"	);

	//pEntry->pClump = pstAgcdObjectTemplate->m_pstClump;
	strcpy(pstAgcdObjectTemplate->m_szCategory, pEntry->name);

	if (pstAgcdObjectTemplate->m_pstClump)
		VERIFY( NULL != g_pcsAgcmObject->SaveClump(pstAgcdObjectTemplate->m_pstClump, pstAgcdObjectTemplate->m_szDFFName) && "������Ʈ Ŭ���� ���̺� ����");

	return pstApdObjectTemplate;
*/

	return NULL;
}

ApdObjectTemplate * CUITileList_ObjectTabWnd::EditTemplate(int nTID, const char *name, const char *file, RwRGBA *pstColor, int nBlockingType)
{
	ASSERT( !"����ϸ� �ȉ�!" );
/*
	ASSERT( NULL != name	);

	ApdObjectTemplate	*	pstApdObjectTemplate	;
	AgcdObjectTemplate	*	pstAgcdObjectTemplate	;

	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate( nTID );
	ASSERT( NULL != pstApdObjectTemplate	);
	if( !pstApdObjectTemplate )
		return FALSE;

	pstAgcdObjectTemplate = g_pcsAgcmObject->GetTemplateData( pstApdObjectTemplate );
	ASSERT( NULL != pstAgcdObjectTemplate	);

	strcpy( pstApdObjectTemplate	->m_szName		, name );

	if (file)
	{
		strcpy( pstAgcdObjectTemplate	->m_szDFFName	, file );

		if( pstAgcdObjectTemplate->m_pstClump )
			RpClumpDestroy( pstAgcdObjectTemplate->m_pstClump );

		pstAgcdObjectTemplate->m_pstClump = g_pcsAgcmObject->LoadClump( (char *) file );
	}

	if (pstColor)
	{
		AcuObject::SetClumpPreLitLim(pstAgcdObjectTemplate->m_pstClump, pstColor);

		g_pcsAgcmObject->SaveClump(pstAgcdObjectTemplate->m_pstClump, pstAgcdObjectTemplate->m_szDFFName);
	}

	ASSERT( NULL != pstAgcdObjectTemplate->m_pstClump	);

	//if( nBlockingType >= 0 )
	//	pstApdObjectTemplate->m_stBlockInfo.type = ( AuBlockingType ) nBlockingType;

	return pstApdObjectTemplate;
*/
	return NULL;
}

BOOL CUITileList_ObjectTabWnd::DeleteTemplate(int nTID)
{
	return g_pcsApmObject->DeleteObjectTemplate( nTID );
//	// ������ (2003-06-23 ���� 6:06:16) : ApmObject���� �������� ��Ʈ������..
//	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::DeleteObject(RpClump *pClump)
{
	ASSERT( NULL != pClump );
	if( NULL == pClump )
		return FALSE;

	return g_pcsAgcmObject->ClumpDelete( pClump , TRUE );

//	RwFrame			*	pFrame			= RpClumpGetFrame( pClump );
//	//RpUserDataArray	*	pUserDataArray	;
//	int oid = 0;
//	//int i;
//
//	ASSERT( NULL != pFrame );
//
//	INT32	nType = AcuObject::GetClumpType( pClump, &oid );
//
//	switch( AcuObject::GetType( nType ) )
//	{
//	case	ACUOBJECT_TYPE_OBJECT			:
//		break;
//	default:
//		oid = 0;
//		break;
//	}
//
//	/*
//	for( i = 0; i < RwFrameGetUserDataArrayCount( pFrame ); i++ )
//	{
//		pUserDataArray = RwFrameGetUserDataArray( pFrame, i );
//		if( pUserDataArray )
//		{
//			if( !strcmp( RpUserDataArrayGetName( pUserDataArray ), "OID" ) )
//			{
//				oid = RpUserDataArrayGetInt( pUserDataArray, 0 );
//			}
//		}
//	}
//	*/
//
//	if( oid )
//	{
//		RpWorldRemoveClump( g_pEngine->m_pWorld, pClump );
//
//		g_pcsApmObject->DeleteObject( oid );
//	}
//
//	return oid;
}



LRESULT CUITileList_ObjectTabWnd::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	switch( message )
	{
	case WM_COMMAND:
		{

			DWORD	wNotifyCode	= HIWORD(wParam); // notification code 
			DWORD	wID			= LOWORD(wParam); // item, control, or accelerator identifier 
			HWND	hwndCtl		= (HWND) lParam	; // handle of control 

			BOOL	*pBoolValue	= NULL			;

			if( wNotifyCode == BN_CLICKED || wNotifyCode == BN_DOUBLECLICKED )
			switch( wID )
			{
			case	ID_SNAPTOGRID	:	pBoolValue = &m_bSnapToGrid		;	break;
			case	ID_SNAPTOHEIGHT	:	pBoolValue = &m_bSnapToHeight	;	break;
			case	ID_MOVEXAXIS	:	pBoolValue = &m_bMoveXAxis		;	break;
			case	ID_MOVEZAXIS	:	pBoolValue = &m_bMoveZAxis		;	break;
			case	ID_ROTATEXAXIS	:	pBoolValue = &m_bRotateXAxis	;	break;
			case	ID_USE_BRUSHDRAWING	:pBoolValue = &m_bUseBrush		;	break;
			default:
				break;
			}

			if( pBoolValue )
			{
				if( *pBoolValue == FALSE )
				{
					// True�� ��۵ɶ� �ϴ� �۾�..
					switch( wID )
					{
					case	ID_USE_BRUSHDRAWING	:
						{
							// 
							CObjectBrushSettingDlg	dlg;

							dlg.m_fDensity		= m_fDensity	;	// Ÿ�ϴ� ����Ƽ..
							dlg.m_fRotateMin	= m_fRotateMin	;
							dlg.m_fRotateMax	= m_fRotateMax	;
							dlg.m_fRotateYMin	= m_fRotateYMin	;
							dlg.m_fRotateYMax	= m_fRotateYMax	;
							dlg.m_fScaleMin		= m_fScaleMin	;
							dlg.m_fScaleMax		= m_fScaleMax	;

							if( IDOK == dlg.DoModal() )
							{
								// ���� ����..
								*pBoolValue = ! *pBoolValue;
								m_pctlButton[ wID ].SetCheck( *pBoolValue );

								m_fDensity		= dlg.m_fDensity	;	// Ÿ�ϴ� ����Ƽ..
								m_fRotateMin	= dlg.m_fRotateMin	;
								m_fRotateMax	= dlg.m_fRotateMax	;
								m_fRotateYMin	= dlg.m_fRotateYMin	;
								m_fRotateYMax	= dlg.m_fRotateYMax	;
								m_fScaleMin		= dlg.m_fScaleMin	;
								m_fScaleMax		= dlg.m_fScaleMax	;
							}
							else
							{

							}
						}
						break;
					default:
						// Toggle Value
						*pBoolValue = ! *pBoolValue;
						m_pctlButton[ wID ].SetCheck( *pBoolValue );
						break;
					}
				}
				else
				{
					// Toggle Value
					*pBoolValue = ! *pBoolValue;

					m_pctlButton[ wID ].SetCheck( *pBoolValue );
				}
			}
		}
		break;
	default:
		break;
	}
	
	return CWnd::WindowProc(message, wParam, lParam);
}

void _GetIntersectPos( AuPOS *pPos , RwV3d *pNormal , RwV3d *pStart , RwV3d *pEnd , RwV3d *pCollisionPoint )
{
	FLOAT	t;

	t = ( pNormal->x * ( pPos->x - pStart->x ) + pNormal->z * ( pPos->z - pStart->z ) ) /
		( pNormal->x * ( pEnd->x - pStart->x ) + pNormal->z * ( pEnd->z - pStart->z ) )	;

	pCollisionPoint->x = ( pEnd->x - pStart->x ) * t + pStart->x;
	pCollisionPoint->y = ( pEnd->y - pStart->y ) * t + pStart->y;
	pCollisionPoint->z = ( pEnd->y - pStart->z ) * t + pStart->z;
}

BOOL	CUITileList_ObjectTabWnd::IsObjectSelected	()
{
	HTREEITEM	item	= m_wndTreeCtrl.GetSelectedItem();

	if( !item )
		return FALSE;
	else
		return TRUE;
}

INT32	CUITileList_ObjectTabWnd::GetObjectTID		()
{
	HTREEITEM				item					= m_wndTreeCtrl.GetSelectedItem();
	stObjectEntry		*	pObject					;
	
	ASSERT( NULL != item && "���É� �������� �����ϴ�." );

	if( !item )
		return -1;

	pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

	ASSERT( pObject != NULL );
	if( NULL == pObject )
	{
		return -1;
	}
	else
		return pObject->tid;
}

BOOL		CUITileList_ObjectTabWnd::LoadCategory		()
{
	char	filename[ 1024 ];
	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , OBJECT_FILE );
	FILE	*pFile = fopen( filename , "rt" );

	ASSERT( NULL != pFile && "������Ʈ ��ũ��Ʈ ���� ���� ����" );

	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return FALSE;
	}

	CGetArg2		arg;

	char			strBuffer[	1024	];
	//unsigned int	nRead = 0;

	int				count = 0;
	int				level;
	HTREEITEM		current[10];
	CCategory		stCategory;

	current[0] = NULL;

	while( fgets( strBuffer , 1024 , pFile ) )
	{
		// �о�����..

		arg.SetParam( strBuffer , "|\n" );

		if( arg.GetArgCount() < 2 )
		{
			// ���� �̻�
			continue;
		}

		level				= atoi( arg.GetParam( 0 ) )	;
		if( level >= 10 )
		{
			continue;
		}

		current[ level ] = AddEntry( 0, arg.GetParam( 1 ) , arg.GetArgCount() > 2 ? arg.GetParam( 2 ):NULL , NULL , level ? current[level - 1]:TVI_ROOT , NULL );

		// ������ (2005-04-11 ���� 4:31:36) : ī�װ� �����ص�..
		stCategory.str	= arg.GetParam( 1 );
		stCategory.pos	= current[ level ];
		m_listCategory.push_back( stCategory );

		count ++;
	}

	fclose( pFile );

	return TRUE;
}

BOOL CUITileList_ObjectTabWnd::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN	||
		pMsg->message == WM_KEYUP	||
		pMsg->message == WM_CHAR	)
	{
		g_pMainFrame->PostMessage( pMsg->message , pMsg->wParam , pMsg->lParam );
		return TRUE;
	}
	
	return CWnd::PreTranslateMessage(pMsg);
}

void CUITileList_ObjectTabWnd::OnObjectpopupTaketemplateinfo()
{
	ApdObjectTemplate *		pcsApdTemplate;
	AgcdObjectTemplate *	pcsAgcdTemplate;

	// ������ (2005-04-04 ���� 12:05:16) : 
	// ����Ÿ ����..
	{
		HTREEITEM		item;
		stObjectEntry	*pObject;

		item = m_wndTreeCtrl.GetSelectedItem();
		if( !item )
			return; // ���� �Ȱ� ����.

		pObject = (stObjectEntry *) m_wndTreeCtrl.GetItemData( item );

		if( !pObject )
			return; // ������Ʈ�� �ƴϴ�.

		pcsApdTemplate = g_pcsApmObject->GetObjectTemplate(pObject->tid);
		if (!pcsApdTemplate)
			return; // ���ø��� ����.

		pcsAgcdTemplate = g_pcsAgcmObject->GetTemplateData(pcsApdTemplate);
		if( !pcsAgcdTemplate )
			return; // ��?..
	}

	// ���۷��� ī��Ʈ ����..������Ʈ ����Ÿ �ε�.
	//++(pcsAgcdTemplate->m_lRefCount);
	//g_pcsAgcmObject->LoadTemplateData( pcsApdTemplate );
	RpClump * pClump = NULL;
	{
		pClump = g_pcsAgcmObject->LoadClump( pcsAgcdTemplate->m_stGroup.m_pstList->m_csData.m_pszDFFName , pcsAgcdTemplate->m_szCategory );

		ASSERT( NULL != pClump );
		if( !pClump ) return;
	}

	RwRaster *		pRaster	;
	RwRaster *		pZRaster;
	{
		// ������ �غ�
		RwRect			rectMap	;

		rectMap.x	= 0;
		rectMap.y	= 0;
		rectMap.w	= 512;
		rectMap.h	= 512;

		pRaster		= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPECAMERA		);
		if( NULL == pRaster )
		{
			MessageBox( "�̴ϸ� ī�޶� �޸� ���� ��������" );
			return;
		}
		pZRaster	= RwRasterCreate(rectMap.w , rectMap.h , 0 , rwRASTERTYPEZBUFFER	);
		if( NULL == pZRaster )
		{
			RwRasterDestroy	( pRaster	);
			MessageBox( "�̴ϸ� �޸� ���� ��������" );
			return;
		}

		RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , pRaster	);
		RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , pZRaster	);
	}

	// ī�޶� ��ġ ����
	INT32	nPrevType	= g_MyEngine.GetSubCameraType();
	g_MyEngine.SetSubCameraType( g_MyEngine.SC_OBJECTCAPTURE );
	g_MyEngine.MoveSubCameraToCenter( NULL );

	// ������Ʈ ��ġ�ϱ�.
	{
		RwFrame * pFrame = RpClumpGetFrame( pClump );
		RwV3d vPos;
		RwFrame * pCameraFrame = RwCameraGetFrame( g_MyEngine.GetParallelCamera() );
		vPos.x	= pCameraFrame->ltm.pos.x;
		vPos.y	= 0.0f;
		vPos.z	= pCameraFrame->ltm.pos.z;
		RwFrameTranslate( pFrame , &vPos , rwCOMBINEREPLACE );
	}

	// �����ϱ�
	{
		RwRGBA		rgbBackground;

		rgbBackground.red   = 255;
		rgbBackground.green	= 255;
		rgbBackground.blue	= 255;
		rgbBackground.alpha	= 255;
		
		RwCameraClear( g_MyEngine.GetParallelCamera(), &rgbBackground, rwCAMERACLEARZ|rwCAMERACLEARIMAGE);
	
		if( RwCameraBeginUpdate( g_MyEngine.GetParallelCamera() ) )
		{
			RpClumpRender( pClump );
	        RwCameraEndUpdate( g_MyEngine.GetParallelCamera() );
		}

		// RsCameraShowRaster( g_MyEngine.GetParallelCamera() );	// Flip the Raster! - -+
	}

	// ī�޶� ����ġ
	g_MyEngine.SetSubCameraType( nPrevType );

	// ���۷��� ī��Ʈ down & ���ø� ������.
	//--(pcsAgcdTemplate->m_lRefCount);
	//if (pcsAgcdTemplate->m_lRefCount < 1)
	//	g_pcsAgcmObject->ReleaseObjectTemplateData(pcsAgcdTemplate);

	if( pClump )
	{
		RpClumpDestroy( pClump );
	}

	// �̹��� ���� �м�.
	{
		RwRect			rectMap	;

		rectMap.x	= 0;
		rectMap.y	= 0;
		rectMap.w	= 512;
		rectMap.h	= 512;

		RwImage	*	pImage;
		VERIFY( pImage = RwImageCreate( rectMap.w , rectMap.h , 32 ) );
		RwImageAllocatePixels( pImage );

		char	filename[ 256 ];
		wsprintf( filename , "%d.bmp" , pcsApdTemplate->m_lID );

		VERIFY( RwImageSetFromRaster( pImage , RwCameraGetRaster( g_MyEngine.GetParallelCamera() ) ) );

		RwImageWrite( pImage , filename );

		RwImageDestroy	( pImage	);
	}

	// ������ ����..
	{
		RwCameraSetRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubRaster	);
		RwCameraSetZRaster	( g_MyEngine.GetParallelCamera() , g_MyEngine.m_pSubCameraMainCameraSubZRaster	);
		RwRasterDestroy	( pRaster	);
		RwRasterDestroy	( pZRaster	);
	}
}
