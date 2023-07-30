// Plugin_BossSpawn.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "MapTool.h"
#include "MainFrm.h"
#include "MainWindow.h"
#include "Plugin_BossSpawn.h"
#include "ApModuleStream.h"
#include "PositionDlg.h"

void __DrawWireFrame( RpClump * pClump );
void _GetIntersectPos( AuPOS *pPos , RwV3d *pNormal , RwV3d *pStart , RwV3d *pEnd , RwV3d *pCollisionPoint );

CPlugin_BossSpawn * CPlugin_BossSpawn::m_spThis = NULL;
extern MainWindow			g_MainWindow	;
static const char g_strINIObjectBossInfo	[]	= "BossCID"		;


ApdObject * CPlugin_BossSpawn::Spawn::GetObject		()
{
	return g_pcsApmObject->GetObject( this->nOID );
}

AgpdCharacter * CPlugin_BossSpawn::Spawn::GetCharacter	()
{
	return g_pcsAgpmCharacter->GetCharacter( this->nCID );
}

// CPlugin_BossSpawn

IMPLEMENT_DYNAMIC(CPlugin_BossSpawn, CWnd)
CPlugin_BossSpawn::CPlugin_BossSpawn():
	m_bLButtonDown( FALSE )
{
	SetModuleName("Plugin_BossSpawn");

	m_strShortName = "B.Spawn";
	m_nCharacterIDCounter	= BOSSMOB_OFFSET;

	m_pSelectedClump		= NULL	;

	m_PrevMouseStatus.pos.x	= 0;
	m_PrevMouseStatus.pos.y	= 0;

	m_nLastReadSpawnCharacterID	= 0;

	m_stOption.bExcludeWhenExport = true;

	m_pBossSpawnDlg			=	NULL;

	CPlugin_BossSpawn::m_spThis = this;
}

CPlugin_BossSpawn::~CPlugin_BossSpawn()
{
}


BEGIN_MESSAGE_MAP(CPlugin_BossSpawn, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
END_MESSAGE_MAP()



// CPlugin_BossSpawn �޽��� ó�����Դϴ�.


void CPlugin_BossSpawn::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 255 ) );


}

BOOL CPlugin_BossSpawn::OnSelectedPlugin		()
{
	m_pCurrentCharacter	= NULL;
	m_pBossSpawnDlg->ShowWindow( SW_SHOW );
	m_pBossSpawnDlg->OnInitNumber();
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnDeSelectedPlugin		()
{
	m_pCurrentCharacter	= NULL;
	m_pBossSpawnDlg->ShowWindow( SW_HIDE );
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnSelectObject			( RpClump * pClump )
{
	TRACE( "Select Object\n" );
	INT32	nID;
	switch( ACUOBJECT_TYPE_TYPEFILEDMASK & AcuObject::GetClumpType( pClump , &nID ) )
	{
	case ACUOBJECT_TYPE_CHARACTER:
		{
			// ID�� ������ Ȯ��..
			if( ISBOSSID( nID ) )
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
BOOL CPlugin_BossSpawn::OnLButtonDownGeometry	( RwV3d * pPos )
{
	AuPOS	vPos;

	vPos.x	=	pPos->x;
	vPos.y	=	pPos->y;
	vPos.z	=	pPos->z;

	if(	m_pBossSpawnDlg->CheckPathSet() && m_pCurrentCharacter )
	{
		STMonsterPathInfoPtr	pMonsterPath	=	m_pBossSpawnDlg->CreateBossSpawnPath( CString(m_pCurrentCharacter->m_szID) );

		if( m_pBossSpawnDlg->AddBossSpawnPath( pMonsterPath , vPos ) )
		{
			m_pBossSpawnDlg->UpdatePathListBox();
		}

		return TRUE;
	}

	m_pCurrentCharacter	= NULL;

	AgpdCharacter * pCharacter;

	INT	nSelect = m_pBossSpawnDlg->GetCurSel();
	if( nSelect == CB_ERR )
	{
		MD_SetErrorMessage( "No item is selected-_-\n" );
		return TRUE;
	}

	CString	str;
	m_pBossSpawnDlg->GetText( nSelect , str );

	int	nTemplateID = 1;	// ���ø��� �� 

	nTemplateID = atoi( ( LPCTSTR ) str );

	// ���ø� ������üũ..
	if( nTemplateID < 0 )
	{
		MD_SetErrorMessage( "�ɸ��� ���ø� ���̵� �̻�..\n" );
		return FALSE;
	}

	// �̷��� �ϸ� �ȵ���... -_-;
	// m_nCharacterIDCounter NPC������ ������ �������� ��� �������... -_-; By Parn
	char	strNPCName[ 256 ];

	m_nCharacterIDCounter = rand() % 10000 + BOSSMOB_OFFSET;
	wsprintf( strNPCName , "BOSS_%04d" , m_nCharacterIDCounter % BOSSMOB_OFFSET );

	g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ) );		// modify by gemani
	pCharacter = g_pcsAgpmCharacter->AddCharacter( m_nCharacterIDCounter++ , nTemplateID , strNPCName );

	if( NULL == pCharacter )
	{
		DisplayMessage( RGB( 232 , 121 , 25 ) , "�ش� �ɸ��Ϳ� ���� ���ø��� �����~" );
		return TRUE;
	}

	// �� �۾������� ���Ƿ� ����..
	AgpdCharacterTemplate * pTemplate = pCharacter->m_pcsCharacterTemplate;
	g_pcsAgpmCharacter->SetTypeTrap( pTemplate );

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

		// ���忡 Add �ž����Ƿ�..
		// ���� ������Ʈ�� ���� ��ġ�� ����.
		// �׸��� �̺�Ʈ ���.
		{
			AddSpawnObject( pCharacter );
		}
	}
	else
	{
		// ���������Ƿ� ����..

		// ���? -_-;;;
	}

	SetSaveData();

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnLButtonUpGeometry	( RwV3d * pPos )
{
	return TRUE;
}

BOOL CPlugin_BossSpawn::OnMouseMoveGeometry	( RwV3d * pPos )
{
	if( m_pCurrentCharacter && pPos && m_bLButtonDown )
	{
		// �ɸ��͸� pPos ��ġ�� �̵���Ŵ..
		AuPOS	pos;
		pos.x	= pPos->x;
		pos.y	= AGCMMAP_THIS->GetHeight_Lowlevel( pPos->x , pPos->z , SECTOR_MAX_HEIGHT);
		pos.z	= pPos->z;

		g_pcsAgpmCharacter->UpdatePosition( m_pCurrentCharacter , &pos , FALSE , FALSE );
		UpdateCharacter( m_pCurrentCharacter );

		SetSaveData();
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "Boss ���� ����" );

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnSaveData				()
{
	// ���� -_-;;
	// ���ھ�~
	VERIFY( AGCMMAP_THIS->EnumLoadedDivision( CPlugin_BossSpawn::CBSave , ( PVOID ) this ) );

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBCheckSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData );
	if( NULL == pData ) return FALSE;

	ApdObject *			pcsApdObject = ( ApdObject * ) pData;

	// ���� �����͸� ������.
	if( g_pcsApmEventManager->GetEvent( pcsApdObject , APDEVENT_FUNCTION_SPAWN ) )
		return TRUE;
	else
		return FALSE;
}

BOOL	CPlugin_BossSpawn::CBSave ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	if(!pDivisionInfo)
		return FALSE;

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

	wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,
		ALEF_CURRENT_DIRECTORY , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

	if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_BossSpawn::CBCheckSpawn )	)
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

	{
		CUITileList_ObjectTabWnd * pWnd = ( CUITileList_ObjectTabWnd * ) pData;

		// ���丮 �غ�..
		GetSubDataDirectory( pWnd->m_strSubDataMainDir );

		_CreateDirectory( pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini"							, pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini\\object"					, pWnd->m_strSubDataMainDir );
		_CreateDirectory( "%s\\ini\\object\\design"			, pWnd->m_strSubDataMainDir );

		//////////////////////////////////////////////////////////////////////////
		// ������ (2004-11-29 ���� 12:03:07) : 
		// SubData ����..
		wsprintf( strFilename , "%s\\" APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,
			pWnd->m_strSubDataMainDir , GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ ) );

		if( g_pcsApmObject->StreamWrite( bbox , 0 , strFilename, CPlugin_BossSpawn::CBCheckSpawn )	)
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
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnLoadData				()
{
	DivisionCallbackResult	result;
	VERIFY( AGCMMAP_THIS->EnumLoadedDivision( CPlugin_BossSpawn::CBLoad , static_cast< PVOID >( &result ) ) );

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBLoad ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	DivisionCallbackResult	* pResult = ( DivisionCallbackResult * ) pData;

	ASSERT( NULL != pResult );

	pResult->nDivisionCount++;

	AcuFrameMemory::Clear();

	char	strFilename[ 256 ];

	CFileFind	ff;

	wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_BOSSSPAWN ,  
		GetDivisionIndex( pDivisionInfo->nX , pDivisionInfo->nZ )		);

	if( ff.FindFile( strFilename ) )
	{
		VERIFY( g_pcsApmObject->StreamRead( strFilename , FALSE , CPlugin_BossSpawn::CBReadSpawn , TRUE )	);
	}
	else
	{
		// ��� �׳� �����Ѵ�.
		return TRUE;
	}

	pResult->nSuccessCount++;

	return TRUE;
}

BOOL CPlugin_BossSpawn::CBReadSpawn(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ASSERT( NULL != pData	);
	ASSERT( NULL != pClass	);
	
	if( NULL == pData ) return FALSE;

	CPlugin_BossSpawn	* pThis = CPlugin_BossSpawn::m_spThis;
	ApdObject	*		pcsApdObject = ( ApdObject * ) pData;

	// �� �۾������� ���Ƿ� ����..
	AgpdCharacterTemplate * pTemplate;
	pTemplate = g_pcsAgpmCharacter->GetCharacterTemplate(pThis->m_nLastReadSpawnCharacterID);
	// Ʈ�� ���� .. ���۾� ���ǿ�.
	g_pcsAgpmCharacter->SetTypeTrap( pTemplate );

	// �ɸ��� �߰�..
	pThis->AddCharacter( pcsApdObject , pThis->m_nLastReadSpawnCharacterID );

	return TRUE;
}

BOOL CPlugin_BossSpawn::OnCleanUpData			()
{
	// �ɸ��� ���鼭 Ư�� �ɼ� �̻��� �ɸ��� ���� &
	// ����� ���� ������Ʈ ����..
	return TRUE;
}

BOOL CPlugin_BossSpawn::Window_OnKeyDown		( RsKeyStatus *ks	)
{
	if( NULL == m_pCurrentCharacter ) return TRUE;

    switch( ks->keyCharCode )
    {
	case rsDEL:
		{
			// ���� ���� ����
			DeleteCharacter( m_pCurrentCharacter );

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

BOOL CPlugin_BossSpawn::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	SetFocus();

	// �н� üũ ��ư�� ���� ������ �н� �߰��� �Ѵ�
	// �ٸ� �۾��� ��ŵ~

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

				m_pBossSpawnDlg->SetSelectCharacter( m_pCurrentCharacter );
				m_pBossSpawnDlg->SetSelectBoss( m_pCurrentCharacter->m_szID , m_pCurrentCharacter->m_pcsCharacterTemplate->m_szTName , m_pCurrentCharacter->m_stPos );
				Invalidate( FALSE );
			}
			break;
		default:
			return FALSE;
		}

		m_pSelectedClump	= pClump;
		m_bLButtonDown		= TRUE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_BossSpawn::Window_OnLButtonDblClk	( RsMouseStatus *ms )
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

				// ���� ���� ����..

				Spawn * pSpawn = GetSpawn( m_pCurrentCharacter->m_lID );

				if( pSpawn )
				{
					ApdObject * pObject;
					pObject = pSpawn->GetObject();

					ASSERT( NULL != pObject );

					// ���� �̺�Ʈ ����.

					ApdEvent *	pstEvent;

					// EventNature Event Struct ��..
					pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

					if( pstEvent )
					{
						// �̺�Ʈ ����.
						g_pcsAgcmEventSpawnDlg->Open( pstEvent );
					}

					SetSaveData();

					return TRUE;
				}

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

BOOL CPlugin_BossSpawn::Window_OnLButtonUp		( RsMouseStatus *ms )
{
	m_pSelectedClump	= NULL;
	m_bLButtonDown		= FALSE;

	return FALSE;
}

BOOL CPlugin_BossSpawn::Window_OnMouseMove		( RsMouseStatus *ms )
{
	if( m_pCurrentCharacter )
	{
		if (ms->control && ms->shift )
		{
			FLOAT	fDeltaX	= ms->pos.x - m_PrevMouseStatus.pos.x;	
			FLOAT	fDeltaY	= ms->pos.y - m_PrevMouseStatus.pos.y;
			
			g_pcsAgpmCharacter->TurnCharacter( m_pCurrentCharacter , 0 , fDeltaY );
			UpdateCharacter( m_pCurrentCharacter );
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
			UpdateCharacter( m_pCurrentCharacter );
			return TRUE;
		}
	}

	m_PrevMouseStatus = * ms;
	return FALSE;
}

BOOL CPlugin_BossSpawn::OnWindowRender			()
{
	__DrawWireFrame( m_pSelectedClump );

	static BOOL _bShowSphere = FALSE;

	if( _bShowSphere )
	{
		INT32			lIndex			;
		ApdObject	*	pcsObject		;

		lIndex = 0;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{

			if( CPlugin_BossSpawn::CBCheckSpawn( ( void * ) pcsObject , NULL , NULL ) )
			{
				//AgcdObject * pstAgcdObject = g_pcsAgcmObject->GetObjectData(pcsObject);
				// AcuObjecWire::bGetInst().bRenderClump( pstAgcdObject->m_stGroup.m_pstList[ 0 ].m_csData.m_pstClump );
				g_MainWindow.DrawAreaSphere( pcsObject->m_stPosition.x , pcsObject->m_stPosition.y , pcsObject->m_stPosition.z ,
					g_pMainFrame->m_Document.m_fBrushRadius );
			}
		}
	}

	return TRUE;
}

int CPlugin_BossSpawn::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CUITileList_PluginBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pBossSpawnDlg	=	new	CBossSpawnDlg( this );
	m_pBossSpawnDlg->Create( IDD_BOSSSPAWNDLG , this );
	

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
			m_pBossSpawnDlg->AddSpawnName( str );
		}
	}

	return 0;
}

// ���� ���� ������Ʈ�� �����Ҷ� ����Ʈ ������ ������Ʈ �߰��� �̺�Ʈ ���.
BOOL	CPlugin_BossSpawn::AddSpawnObject( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	ApdObjectTemplate	*	pstApdObjectTemplate	;
	ApdObject			*	pstApdObject			;
	FLOAT	fRadius = 0.0f;

	pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(467);
	if( !pstApdObjectTemplate )
	{
		TRACE( "���ø� ID���� ���ø� ���� ��� ����..\n" );
		return FALSE;
	}

	RwV3d	pos;
	pos.x	= pCharacter->m_stPos.x;
	pos.y	= pCharacter->m_stPos.y;
	pos.z	= pCharacter->m_stPos.z;

	pstApdObject = g_pMainFrame->m_pTileList->m_pObjectWnd->AddObject(
		pstApdObjectTemplate ,
		&pos , 
		ACUOBJECT_TYPE_DUNGEON_STRUCTURE	// ���� ��Ʈ���ķ� �����..
		);

	ASSERT( NULL != pstApdObject );
	if( !pstApdObject )
	{
		MD_SetErrorMessage( "g_pcsApmObject->AddObject ������Ʈ �߰�����.." );
		return NULL;
	}

	// ��ġ����..
	pstApdObject->m_stPosition	= pCharacter->m_stPos;

	pstApdObject->m_stScale.x	= 0.000001f; // ������ ���ƶ� >_<;;
	pstApdObject->m_stScale.y	= 0.000001f;
	pstApdObject->m_stScale.z	= 0.000001f;

	g_pcsAgcmObject->SetupObjectClump_TransformOnly( pstApdObject );

	ApdObject * pObject = pstApdObject;
	ApdEvent *	pstEvent;

	// EventNature Event Struct ��..
	pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

	if( NULL == pstEvent )
	{
		// �̺�Ʈ ����..
		ApdEventAttachData *pstEventAD = g_pcsApmEventManager->GetEventData( pObject );

		pstEvent = g_pcsApmEventManager->AddEvent(pstEventAD, APDEVENT_FUNCTION_SPAWN , pObject , FALSE);
	}

	if( pstEvent )
	{
		// ��������~
		g_pcsApmEventManager->SetCondition( pstEvent , APDEVENT_COND_AREA );
		pstEvent->m_pstCondition->m_pstArea->m_eType = APDEVENT_AREA_SPHERE;
		pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius = fRadius;

		// ���� ���� ���� ���⼭..
		//g_pcsAgcmEventSpawnDlg->Open( pstEvent );
	}
	
	// ��̿� �߰��ص�
	Spawn	stSpawn;
	stSpawn.nCID	= pCharacter->m_lID	;
	stSpawn.nOID	= pObject->m_lID	;
	stSpawn.nTID	= pCharacter->m_pcsCharacterTemplate->m_lID;
	m_vecSpawn.push_back( stSpawn );

	return TRUE;
}

// ����������Ʈ�� �ε���� , �� ������ �ɸ��͸� �߰��ϱ� ���Ѱ�.
BOOL	CPlugin_BossSpawn::AddCharacter( ApdObject * pObject , INT32 nTemplateID )
{
	AgpdCharacter * pCharacter;


	// �̷��� �ϸ� �ȵ���... -_-;
	// m_nCharacterIDCounter NPC������ ������ �������� ��� �������... -_-; By Parn
	char	strNPCName[ 256 ];

	m_nCharacterIDCounter = rand() % 10000 + BOSSMOB_OFFSET;
	wsprintf( strNPCName , "BOSS_%04d" , m_nCharacterIDCounter % BOSSMOB_OFFSET );

	g_pcsAgcmRender->SetMainFrame( RwCameraGetFrame( g_MyEngine.m_pCamera ) );		// modify by gemani
	pCharacter = g_pcsAgpmCharacter->AddCharacter( m_nCharacterIDCounter++ , nTemplateID , strNPCName );

	if( NULL == pCharacter )
	{
		DisplayMessage( RGB( 232 , 121 , 25 ) , "�ش� �ɸ��Ϳ� ���� ���ø��� �����~" );
		return FALSE;
	}

	// �ش� ��ġ�� ���ø� ����..

	AuPOS	pos;
	pos	= pObject->m_stPosition;

	ApWorldSector * pWorldSector;
	pWorldSector = g_pcsApmMap->GetSector( pos.x , pos.z );

	if( pWorldSector )
	{
		pCharacter->m_unCurrentStatus	= AGPDCHAR_STATUS_IN_GAME_WORLD	;
		pCharacter->m_stPos				= pos							;
		pCharacter->m_fTurnX			= pObject->m_fDegreeX			;
		pCharacter->m_fTurnY			= pObject->m_fDegreeY			;

		g_pcsAgpmCharacter->UpdateInit( pCharacter );

		g_pcsAgpmCharacter->UpdatePosition	( pCharacter , &pos					); 

		// ��̿� �߰��ص�
		Spawn	stSpawn;
		stSpawn.nCID	= pCharacter->m_lID	;
		stSpawn.nOID	= pObject->m_lID	;
		stSpawn.nTID	= nTemplateID		;
		m_vecSpawn.push_back( stSpawn );
	}
	else
	{
		// ���������Ƿ� ����..

		// ���? -_-;;;
	}

	SetSaveData();

	return FALSE;
}

// ��ġ,������ ���� ������ ������ ���������� ��ũ�� ����.
BOOL	CPlugin_BossSpawn::UpdateCharacter( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	Spawn * pSpawn = GetSpawn( pCharacter->m_lID );

	if( pSpawn )
	{
		ApdObject * pObject;
		pObject = pSpawn->GetObject();

		// ���� ����...
		// ����Ÿ�� �����ϰ� ���� Init�� ��Ű�� ����.
		pObject->m_stPosition	= pCharacter->m_stPos	;
		pObject->m_fDegreeX		= pCharacter->m_fTurnX	;
		pObject->m_fDegreeY		= pCharacter->m_fTurnY	;

		return TRUE;
	}
	else
	{
		ASSERT( !"�̰� ������ �����µ� -_-.." );
	}

	return FALSE;
}

// �ɸ��͸� �����ϸ鼭 , ���� ��ϵ� ���� ������Ʈ�� ������.
// m_vecSpawn���� �� �ε����� 0���� �����صּ� ���߿� UpdateList ���� �����Ű���.
BOOL	CPlugin_BossSpawn::DeleteCharacter( AgpdCharacter * pCharacter )
{
	ASSERT( NULL != pCharacter );
	if( NULL == pCharacter ) return FALSE;

	Spawn * pSpawn = GetSpawn( pCharacter->m_lID );

	if( pSpawn )
	{
		ApdObject * pObject;
		pObject = pSpawn->GetObject();

		// ������Ʈ ����..
		if( pObject )
		{
			g_pcsApmObject->DeleteObject( pObject );
		}

		pSpawn->Delete();
		UpdateList();

		return TRUE;
	}
	else
	{
		ASSERT( !"�̰� ������ �����µ� -_-.." );
	}

	return FALSE;
}

// m_vecSpawn���� ����� ������Ʈ���� �����Ѵ�.
void	CPlugin_BossSpawn::UpdateList()
{
	vector< Spawn >::iterator iter;
	
	BOOL bFound = FALSE;
	do
	{
		bFound = FALSE;
		for( iter = m_vecSpawn.begin();
			iter != m_vecSpawn.end();
			iter ++ )
		{
			if( (*iter).nCID == 0	&&
				(*iter).nOID == 0	) 
			{
				// ���� ���.
				m_vecSpawn.erase( iter );
				bFound = TRUE;
				break;
			}
		}
	}
	while( bFound );
}

BOOL	CPlugin_BossSpawn::BossInfoStreamWriteCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CPlugin_BossSpawn	*pThis			= (CPlugin_BossSpawn *) pClass;
	ApdObject			*pstApdObject	= (ApdObject *) pData;

	Spawn * pSpawn = pThis->GetSpawnByObject( pstApdObject->m_lID );

	if( pSpawn )
	{
		if( !pcsStream->WriteValue( ( char * ) g_strINIObjectBossInfo , pSpawn->nTID ) )
			return FALSE;
	}

	return TRUE;
}

BOOL	CPlugin_BossSpawn::BossInfoStreamReadCB	(PVOID pData, ApModule *pClass, ApModuleStream *pcsStream)
{
	CPlugin_BossSpawn	*pThis = (CPlugin_BossSpawn *) pClass;
	ApdObject			*pstApdObject	= (ApdObject *) pData;

	const CHAR *szValueName;
	//char	strData		[ 256 ];

	while(pcsStream->ReadNextValue())
	{
		szValueName = pcsStream->GetValueName();

		if (!strcmp(szValueName, ( char * ) g_strINIObjectBossInfo))
		{
			// ������ (2005-06-20 ���� 3:52:14) : 
			// ���Ŀ� ���� CID�� �����ص�..
			pcsStream->GetValue ( (INT32 *) &pThis->m_nLastReadSpawnCharacterID );
		}
	}

	return TRUE;
}

BOOL CPlugin_BossSpawn::Window_OnRButtonDown	( RsMouseStatus *ms )
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

			CPositionDlg	dlg;

			dlg.m_fX		= m_pCurrentCharacter->m_stPos.x;
			dlg.m_fY		= m_pCurrentCharacter->m_stPos.y;
			dlg.m_fZ		= m_pCurrentCharacter->m_stPos.z;
			dlg.m_fDegreeX	= m_pCurrentCharacter->m_fTurnX;
			dlg.m_fDegreeY	= m_pCurrentCharacter->m_fTurnY;

			if( IDOK == dlg.DoModal() )
			{
				AuPOS	pos;
				pos.x	= dlg.m_fX;
				pos.y	= dlg.m_fY;
				pos.z	= dlg.m_fZ;

				g_pcsAgpmCharacter->UpdatePosition( m_pCurrentCharacter , &pos , FALSE , FALSE );
				g_pcsAgpmCharacter->TurnCharacter( m_pCurrentCharacter , 0 , dlg.m_fDegreeY );
				UpdateCharacter( m_pCurrentCharacter );

				SetSaveData();
			}

			m_pSelectedClump	= NULL;
			m_pCurrentCharacter	= NULL;

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


VOID	CPlugin_BossSpawn::PushCharacterPath( VOID )
{
	
}

VOID	CPlugin_BossSpawn::DelCharacterPath( VOID )
{

}