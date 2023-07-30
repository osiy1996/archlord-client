// RTMenuSpawn.cpp : implementation file
//

#include "stdafx.h"
#include "regiontool.h"
#include "RegionToolDlg.h"
#include "RegionMenuDlg.h"
#include "RTMenuSpawn.h"
#include "ProgressDlg.h"
#include "RTMenuRegion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTMenuSpawn

BOOL _IsFileExist( char * pFilename )
{
	FILE	* pFile;
	BOOL	bAvailable = FALSE;

	pFile = fopen( pFilename , "r" );
	if( pFile )
	{
		bAvailable = TRUE;
		fclose( pFile );
	}

	return bAvailable;
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

char *	CRTMenuSpawn::GetMenuName()
{
	static char _strName[] = "Spawn";
	return ( char * ) _strName;
}

CRTMenuSpawn::CRTMenuSpawn()
{
	m_nRangeDragMode	= MODE_NONE	;
	m_pcsSelectedObject	= NULL		;
	m_bUnsavedData		= FALSE		;
	m_bFirstTimeNewObjectFileCreation	= FALSE;
	m_pcsOnMouseObject	= NULL		;
}

CRTMenuSpawn::~CRTMenuSpawn()
{
}

BOOL CRTMenuSpawn::OnLoadData		()
{
	// ������ (2005-06-07 ���� 5:19:53) : 
	// �ڷ���Ʈ �����̸� ����Ÿ �ε� ����.
	if( g_bTeleportOnlyMode ) return TRUE;

	char strFilename[ FILENAME_MAX ];

	TRACE( "ĳ���� ���ø� �ε�\n" );
	if( g_pcsAgpmCharacter &&!g_pcsAgpmCharacter->StreamReadTemplate("Ini\\CharacterTemplatePublic.ini") )
	{
		::MessageBox( NULL , "ĳ���� ���ø� �б⿡ �����߽��ϴ�. ���ø��� ����� �ε���� �ʾ��� �� �ֽ��ϴ�." , "����" , MB_ICONERROR | MB_OK );
	}

	wsprintf( strFilename , "Ini\\%s" , "ObjectTemplate.ini" );
	if( g_pcsApmObject && g_pcsApmObject->StreamReadTemplate( strFilename , NULL , NULL ) == FALSE )
	{
		//MessageBox( "ObjectTemplate.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	// ���� �׷� ����..
	wsprintf( strFilename , "Ini\\%s" , "SpawnGroup.ini" );
	if( g_pcsAgpmEventSpawn && g_pcsAgpmEventSpawn->StreamReadGroup( strFilename ) == FALSE )
	{
		MessageBox( "SpawnGroup.ini ���� �ε� ����.." , "����" , MB_OK | MB_ICONERROR );
	}

	FILE	* pFile;
	BOOL	bAvailable = FALSE;

	pFile = fopen( SPAWN_FILE_NAME , "r" );
	if( pFile )
	{
		bAvailable = TRUE;
		fclose( pFile );
	}

	if( bAvailable )
	{
		g_pcsApmObject->StreamRead( SPAWN_FILE_NAME , FALSE , NULL );
		m_bUnsavedData = TRUE;
		m_bFirstTimeNewObjectFileCreation = TRUE;
		this->m_pParent->m_pMainDlg->m_ctlRegion.SetAllUnsavedDataCheck( DUF_SPAWN );

		MessageBox( "�������������� ����� ���� �и��Ͽ����Ƿ� , ���� ���� �ϽŴ����� ������ SpawnServerInfo.ini�� �������ּ��� ( �����ڿ��� ���� )" , "������" , MB_OK | MB_ICONERROR );
	}
	else
	{
		{
			CProgressDlg	dlg;
			dlg.StartProgress( "���� ����Ÿ �ε���..." , ( MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ) * ( MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ), this );
			int	nCount = 0 ;

			int nDivisionX , nDivisionZ;
			for( nDivisionZ = 0 ; nDivisionZ < MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ; nDivisionZ ++ )
			{
				for( nDivisionX = 0 ; nDivisionX < MAP_WORLD_INDEX_WIDTH / MAP_DEFAULT_DEPTH ; nDivisionX ++ )
				{
					dlg.SetProgress( ++nCount );

					wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN ,  
						MakeDivisionIndex( nDivisionX , nDivisionZ )		);

					if( _IsFileExist( strFilename ) )
					{
						bAvailable |= TRUE;
						g_pcsApmObject->StreamRead( strFilename , FALSE , __ObjectLoadCallback_AdjustPosition , TRUE );
					}
					else
					{
						wsprintf( strFilename , "Map\\Data\\Object\\Design\\OBDP%04d.ini" ,  
							MakeDivisionIndex( nDivisionX , nDivisionZ )		);
					}
				}
			}
			dlg.EndProgress();
		}

		if( bAvailable )
		{
			// �� ������Ʈ���� ���� �Ϸ�..
			m_bUnsavedData = FALSE;
			this->m_pParent->m_pMainDlg->m_ctlRegion.ClearUnsavedDataCheck( DUF_SPAWN );
		}
	}

	INT32			lIndex			;
	ApdObject	*	pcsObject		;

	lIndex = 0;
	for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject												;
			pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pcsObject->m_bStatic	= FALSE;
	}

	// Unsaved Flag ����.

	g_pcsAgpmEventSpawn->LoadSpawnInfoFromExcel( "ini\\spawnExcelData.txt" );

	RefreshDivisionData();

	return TRUE;
}

static AuBOX	* g_pDivisionBox = NULL;

BOOL	__SpawnSaveCheckCallBack(PVOID pData, PVOID pClass, PVOID pCustData)
{
	ApdObject	* pcsApdObject = ( ApdObject * ) pData;
	ApdEvent	* pEvent			;

	if( NULL == pcsApdObject ) return FALSE;

	pEvent	= g_pcsApmEventManager->GetEvent( pcsApdObject , ( ApdEventFunction ) APDEVENT_FUNCTION_SPAWN );

	if( pEvent && g_pDivisionBox )
	{
		// ���� ������ �������� ������
		if( g_pDivisionBox->inf.x			<	pcsApdObject->m_stPosition.x	&&
			pcsApdObject->m_stPosition.x	<=	g_pDivisionBox->sup.x			&&
			g_pDivisionBox->inf.z			<	pcsApdObject->m_stPosition.z	&&
			pcsApdObject->m_stPosition.z	<=	g_pDivisionBox->sup.z			)
		{
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

BOOL CRTMenuSpawn::OnSaveData		()
{
	if( m_bUnsavedData )
	{
		_CreateDirectory( "ini" );
		_CreateDirectory( "ini\\Object" );
		_CreateDirectory( "ini\\Object\\Design" );

		char strFilename[ FILENAME_MAX ] , filename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN "Ini\\SpawnGroup.ini" );

		/*
		if( !IsLatestFile( strFilename ) )
		{
			if( IDYES == MessageBox( "�����׷��� �ֽ� ������ �ƴմϴ�. ������ �۾��� ���� �� ������ .. ���� �۾��Ѱ� �����Ҽ� �����.�����ϰ� �������?" , "������" , MB_YESNOCANCEL ) )
			{
				// do nothing..
			}
			else
			{
				return FALSE;
			}
		}
		*/

		// �����׷��� ������Ʈ �Ѵ�..
		wsprintf( filename , "Ini\\%s" , "SpawnGroup.ini" );
		if( !g_pcsAgpmEventSpawn->StreamWriteGroup( filename ) )
		{
			MessageBox( "�����׷� �������!" );
		}
		else
		{
			/*
			if( FileUpload( strFilename ) )
			{
			}
			else
			{
				MessageBox( "�����׷����� ���ε� ����." );
			}
			*/
		}

		{
			AuBOX bbox;
			char strFilename[ FILENAME_MAX ];
			int nDivisionX , nDivisionZ;
			int nDivisionIndex;
			for( nDivisionZ = 0 ; nDivisionZ < MAP_WORLD_INDEX_HEIGHT / MAP_DEFAULT_DEPTH ; nDivisionZ ++ )
			{
				for( nDivisionX = 0 ; nDivisionX < MAP_WORLD_INDEX_WIDTH / MAP_DEFAULT_DEPTH ; nDivisionX ++ )
				{
					if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag &
						DUF_SPAWN )
					{
						TRACE( "CRTMenuSky::OnSaveData Division %04d �༮�� �������\n", MakeDivisionIndex( nDivisionX , nDivisionZ ) );
					}
					else
					{
						continue;
					}

					nDivisionIndex = MakeDivisionIndex( nDivisionX , nDivisionZ );
					
					bbox.inf.x	=	GetSectorStartX( ArrayIndexToSectorIndexX( GetFirstSectorXInDivision( nDivisionIndex ) ) );
					bbox.inf.z	=	GetSectorStartZ( ArrayIndexToSectorIndexZ( GetFirstSectorZInDivision( nDivisionIndex ) ) );
					bbox.inf.y	=	0.0f;
					bbox.sup.x	=	bbox.inf.x + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH;
					bbox.sup.z	=	bbox.inf.z + MAP_SECTOR_WIDTH * MAP_DEFAULT_DEPTH;
					bbox.sup.y	=	0.0f;

					// ������ (2004-11-23 ���� 5:13:38) : ���� üũ ����..
					if( ( m_bFirstTimeNewObjectFileCreation && GetObjectCountInBBox( &bbox ) ) ||
						!m_bFirstTimeNewObjectFileCreation )
					{
						// ������ ����� üũ..
						g_pDivisionBox = &bbox;
						
						wsprintf( strFilename , APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN ,  
							MakeDivisionIndex( nDivisionX , nDivisionZ )		);
						g_pcsApmObject->StreamWrite( bbox , 0 , strFilename , __SpawnSaveCheckCallBack );

						// ���̺� ����Ÿ �÷��� ����..
						this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag &= ~DUF_SPAWN;
						UnlockDivision( MakeDivisionIndex( nDivisionX , nDivisionZ ) );
					}

				}
			}
			
			m_bUnsavedData = FALSE;
			this->m_pParent->m_pMainDlg->m_ctlRegion.ClearUnsavedDataCheck( DUF_SPAWN );
		}
	}

	return TRUE;
}

BOOL CRTMenuSpawn::OnQuerySaveData()
{
	return m_bUnsavedData;
}

BEGIN_MESSAGE_MAP(CRTMenuSpawn, CWnd)
	//{{AFX_MSG_MAP(CRTMenuSpawn)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRTMenuSpawn message handlers

void CRTMenuSpawn::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	if( m_pcsOnMouseObject )
	{
		ApdEvent	*	pEvent			;
		AgpdSpawn	*	pSpawn			;
		AgpdCharacterTemplate *	pcsTemplate;

		INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

		pEvent	= g_pcsApmEventManager->GetEvent( m_pcsOnMouseObject , ( ApdEventFunction ) nEventType );

		if( pEvent )
		{
			CString	str;
			pSpawn	=	static_cast< AgpdSpawn * >( pEvent->m_pvData );

			dc.SetTextColor( RGB( 255 , 255 , 255 ) );

			int nLine = 0;
			str.Format( "Name:%s" , pSpawn->m_szName );
			dc.TextOut( 0 , nLine++ * 20 , str );
			str.Format( "Max: %d" , pSpawn->m_stConfig.m_lMaxChar );
			dc.TextOut( 0 , nLine++ * 20 , str );
			str.Format( "Pos: %.0f,%.0f" ,  m_pcsOnMouseObject->m_stPosition.x , m_pcsOnMouseObject->m_stPosition.z );
			dc.TextOut( 0 , nLine++ * 20 , str );

			for ( int lIndex = 0; lIndex < pSpawn->m_stConfig.m_lSpawnChar; ++lIndex)
			{
				pcsTemplate = g_pcsAgpmCharacter->GetCharacterTemplate(pSpawn->m_stConfig.m_astChar[lIndex].m_lCTID);
				if (!pcsTemplate)
					continue;

				str.Format( "%d) %s" , lIndex + 1 , pcsTemplate->m_szTName );
				dc.TextOut( 0 , nLine++ * 20 , str );
			}

			str.Format( "SpawnTime :%d" , pSpawn->m_stInfo.m_ulPrevSpawnTime );
			dc.TextOut( 0 , nLine++ * 20 , str );
		}

	}
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL	CRTMenuSpawn::Create( CRect * pRect , CWnd * pParent )
{
	return CWnd::Create( NULL , NULL , WS_CHILD | WS_VISIBLE , *pRect , pParent , 1348 );
}

void CRTMenuSpawn::OnPaintOnMap		( CDC * pDC )
{
	static BOOL _sbUseNew = TRUE;
	if( _sbUseNew )
		OnPaintOnMap_New( pDC );
	else
		OnPaintOnMap_Origianal( pDC );
}

void CRTMenuSpawn::OnPaintOnMap_Origianal		( CDC * pDC )
{
	// ������Ʈ ���� ���...
	CPen	penRegion , penSelected;
	penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
	penSelected.CreatePen( PS_SOLID , 2 , RGB( 255 , 75 , 75 )	);
	pDC->SelectObject( penRegion );
	pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
	pDC->SetBkMode( TRANSPARENT );

	{
		
		INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		AgpdSpawn	*	pSpawn			;
		//AgpdCharacterTemplate *	pcsTemplate;

		CString		strMobName;

		lIndex = 0;

//		INT32		nEventType = APDEVENT_FUNCTION_NATURE	;
		INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

			if( pEvent )
			{
				pSpawn = (AgpdSpawn *) pEvent->m_pvData;
				strMobName.Empty();

				/*
				for ( int lIndex = 0; lIndex < pSpawn->m_stConfig.m_lSpawnChar; ++lIndex)
				{
					pcsTemplate = g_pcsAgpmCharacter->GetCharacterTemplate(pSpawn->m_stConfig.m_astChar[lIndex].m_lCTID);
					if (!pcsTemplate)
						continue;

					strMobName = pcsTemplate->m_szTName;
					break;
				}
				*/

				strMobName.Format( "%s(%d)" , pSpawn->m_szName , pSpawn->m_stConfig.m_lMaxChar );

				if( pEvent->m_pstCondition )
				{
					ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
					if( pArea )
					{
						switch( pArea->m_eType )
						{
						case	APDEVENT_AREA_SPHERE	:
							{
								pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );

								if( m_pcsSelectedObject == pcsObject )
								{
									pDC->SelectObject( penSelected );
								}
								else
								{
									pDC->SelectObject( penRegion );
								}

								DrawAreaSphere( pDC , pPos->x , pPos->y , pPos->z , pArea->m_uoData.m_fSphereRadius , 0 , &strMobName );
							}
							break;
						case	APDEVENT_AREA_FAN		:
						case	APDEVENT_AREA_BOX		:
							{
								TRACE( "�������� �ʴ� Ÿ�� -_-" );
							}
							break;
						}
					}
				}
				else
				{
					// = =?
				}
				

			}
			else
			{
				// do nothing
			}
		}
	}
	
	// Selection ����..
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		CPen	penSelection;
		penSelection.CreatePen( PS_SOLID , 2 , RGB( 255, 128 , 128 )	);
		pDC->SelectObject( penSelection );
		pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

		int nRadius1 = abs( m_SelectedPosX2 - m_SelectedPosX1 );
		int nRadius2 = abs( m_SelectedPosZ2 - m_SelectedPosZ1 );

		int nRadius = nRadius1 < nRadius2 ? nRadius2 : nRadius1;

		CRect	rectEllipse;
		rectEllipse.SetRect(
			m_SelectedPosX1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ,
			m_SelectedPosX1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ);

		pDC->Ellipse( rectEllipse );

		if( rectEllipse.Width() < 100 )
		{
			rectEllipse.left	-= 50;
			rectEllipse.right	+= 50;
		}
		if( rectEllipse.Height() < 100 )
		{
			rectEllipse.top		-= 10;
			rectEllipse.bottom	+= 10;
		}

		pDC->SetBkMode( TRANSPARENT );
		
		CString	str;
		str.Format( "%.1fm" , m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_LENGTH( nRadius ) / 100.0f);
		pDC->SetTextColor( RGB( 255 , 255 , 255 ) );
		pDC->DrawText( str , rectEllipse , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}

}

BOOL CRTMenuSpawn::OnLButtonDblDownMap( CPoint &point , int x , int z )
{
	if( NULL != m_pcsSelectedObject )
	{
		// ���õȰ��� �ִ�
		// �̺�Ʈ Dialog ���..

		try
		{
			if( LockDivision( GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ) )
			{
				

				ApdObject * pObject		= m_pcsSelectedObject;
				ApdEvent *	pstEvent	=	NULL;

				// EventNature Event Struct ��..
				pstEvent	= g_pcsApmEventManager->GetEvent( pObject , APDEVENT_FUNCTION_SPAWN );

				if( pstEvent )
				{
					// ��������~
					m_pParent->ReleaseCaptureRegionView();
					g_pcsAgcmEventSpawnDlg->Open( pstEvent );
					m_pParent->ReleaseCaptureRegionView();
				}

				if( m_pcsSelectedObject == NULL )
				{
					MessageBox( "Spawn Dialog�� ���µ� ������ �߻��߾��" );
					return TRUE;
				}

				m_bUnsavedData = TRUE;

				// �ش� ����� ���̺� üũ..
				this->m_pParent->m_pMainDlg->m_ctlRegion.
					UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
										DUF_SPAWN	);
			}
			else
			{
				CString	str;
				str.LoadString( IDS_ALREADY_LOCKED );
				str.Format( "�ش� Division(%04d) �� '%s'�������� ���� �־��" , 
					GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
					g_strLastLockOwnerName );
				MessageBox( str );
			}
		}
		catch(...)
		{
			MessageBox( "Spawn Dialog�� ���µ� ������ �߻��߾��" );
		}
	}
	return TRUE;
}

ApdObject	*	CRTMenuSpawn::GetSpawnObject( int x , int z )
{
	INT32			lIndex			;
	ApdObject	*	pcsObject		;
	ApdEvent	*	pEvent			;
	AuPOS		*	pPos			;
	AuPOS			posClick		;

	posClick.x	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
	posClick.z	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
	posClick.y	= 0.0f;

	lIndex = 0;

	INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

	for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject														;
			pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

		if( pEvent )
		{
			if( pEvent->m_pstCondition )
			{
				ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
				if( pArea )
				{
					switch( pArea->m_eType )
					{
					case	APDEVENT_AREA_SPHERE	:
						{
							pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );
							
							if( AUPOS_DISTANCE_XZ( * pPos , posClick ) < pArea->m_uoData.m_fSphereRadius )
							{
								return pcsObject;
							}
						}
						break;
					case	APDEVENT_AREA_FAN		:
					case	APDEVENT_AREA_BOX		:
						{
							TRACE( "�������� �ʴ� Ÿ�� -_-" );
						}
						break;
					}
				}
			}
			else
			{
				// = =?
			}
		}
		else
		{
			// do nothing
		}
	}

	return NULL;
}

BOOL CRTMenuSpawn::OnLButtonDownMap	( CPoint &pointOriginal , int x , int z )
{
	m_pcsSelectedObject = NULL;
	m_pointLastPress	= pointOriginal ;

	if( GetAsyncKeyState( VK_CONTROL ) >= 0 )
	{
		INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		AuPOS			posClick		;

		posClick.x	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
		posClick.z	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );
		posClick.y	= 0.0f;

		lIndex = 0;

		INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

			if( pEvent )
			{
				if( pEvent->m_pstCondition )
				{
					ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
					if( pArea )
					{
						switch( pArea->m_eType )
						{
						case	APDEVENT_AREA_SPHERE	:
							{
								pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );
								
								if( AUPOS_DISTANCE_XZ( * pPos , posClick ) < pArea->m_uoData.m_fSphereRadius )
								{
									if( LockDivision( GetDivisionIndexF( pcsObject->m_stPosition.x , pcsObject->m_stPosition.z ) ) )
									{
										// ����..
										m_pcsSelectedObject = pcsObject;
										m_pParent->InvalidateRegionView();
										m_nRangeDragMode = MODE_REGIONMOVE;
									}
									else
									{
										m_nRangeDragMode = MODE_NONE;
										m_pParent->InvalidateRegionView();
										
										CString	str;
										str.LoadString( IDS_ALREADY_LOCKED );
										str.Format( "�ش� Division(%04d) �� '%s'�������� ���� �־��" , 
											GetDivisionIndexF( pcsObject->m_stPosition.x , pcsObject->m_stPosition.z ) ,
											g_strLastLockOwnerName );
										MessageBox( str );

										return FALSE;
									}

									break;
								}
							}
							break;
						case	APDEVENT_AREA_FAN		:
						case	APDEVENT_AREA_BOX		:
							{
								TRACE( "�������� �ʴ� Ÿ�� -_-" );
							}
							break;
						}
					}
				}
				else
				{
					// = =?
				}
				

			}
			else
			{
				// do nothing
			}
		}
	}

	if( NULL == m_pcsSelectedObject )
	{
		m_pParent->SetCaptureRegionView();
	
		int nOffsetX , nOffsetZ;

		nOffsetX = x;
		nOffsetZ = z;

		// �巡�׸��..
		m_nRangeDragMode = MODE_RANGESELECTED;
		m_SelectedPosX1	 = m_SelectedPosX2 = nOffsetX;
		m_SelectedPosZ1	 = m_SelectedPosZ2 = nOffsetZ;
		m_pParent->InvalidateRegionView();
	}
	return TRUE;
}

BOOL CRTMenuSpawn::OnLButtonUpMap		( CPoint &pointOriginal , int x , int z )
{
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// ������ (2004-09-10 ���� 6:04:32) : 
		// ��ư�� ���� ó���ϴ� �༮��..
		int nX , nZ ;
		nX = m_SelectedPosX1;
		nZ = m_SelectedPosZ1;

		int nRadius1 = abs( m_SelectedPosX2 - m_SelectedPosX1 );
		int nRadius2 = abs( m_SelectedPosZ2 - m_SelectedPosZ1 );

		int nRadius = nRadius1 < nRadius2 ? nRadius2 : nRadius1;

		FLOAT	fX , fZ , fRadius;

		fX		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( nX );
		fZ		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( nZ );
		fRadius	= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_LENGTH( nRadius );

		char	str[ 256 ];
		sprintf( str , "(%.0f,%.0f) Radius = %.0f\n" , fX , fZ , fRadius );
		TRACE( str );

		m_nRangeDragMode = MODE_NONE;


		if( nRadius > 1 )
		{
			// ������ (2004-11-12 ���� 5:36:46) : 
			// ������Ʈ �߰�.

			if( LockDivision( GetDivisionIndexF( fX , fZ ) ) )
			{
				ApdObjectTemplate	*	pstApdObjectTemplate	;
				ApdObject			*	pstApdObject			;

				pstApdObjectTemplate = g_pcsApmObject->GetObjectTemplate(467);
				if( !pstApdObjectTemplate )
				{
					TRACE( "���ø� ID���� ���ø� ���� ��� ����..\n" );
					return NULL;
				}

				pstApdObject = g_pcsApmObject->AddObject( 
					g_pcsApmObject->GetEmptyIndex( GetDivisionIndexF( fX , fZ ) ) , pstApdObjectTemplate->m_lID );
				ASSERT( NULL != pstApdObject );
				if( !pstApdObject )
				{
					MD_SetErrorMessage( "g_pcsApmObject->AddObject ������Ʈ �߰�����.." );
					return NULL;
				}

				// ��ġ����..
				pstApdObject->m_stPosition.x	= fX;
				pstApdObject->m_stPosition.z	= fZ;

				try
				{
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

						m_pParent->ReleaseCaptureRegionView();
						g_pcsAgcmEventSpawnDlg->Open( pstEvent );
						m_pParent->ReleaseCaptureRegionView();
					}

					m_bUnsavedData = TRUE;
					
					// �ش� ����� ���̺� üũ..
					this->m_pParent->m_pMainDlg->m_ctlRegion.
						UnsavedDataCheck(	GetDivisionIndexF( pstApdObject->m_stPosition.x , pstApdObject->m_stPosition.z ) ,
											DUF_SPAWN	);

					RefreshDivisionData();
				}
				catch(...)
				{
				}
			}
			else
			{
				m_pParent->ReleaseCaptureRegionView();
				
				CString	str;
				str.LoadString( IDS_ALREADY_LOCKED );
				MessageBox( str );
			}

		}
		else
		{
			m_pParent->InvalidateRegionView();
		}
	}

	if( m_nRangeDragMode == MODE_REGIONMOVE)
	{
		m_nRangeDragMode = MODE_NONE;		
	}

	m_pParent->ReleaseCaptureRegionView();

	return TRUE;
}

BOOL CRTMenuSpawn::OnMouseMoveMap		( CPoint &pointOriginal , int x , int z )
{
	ApdObject * pObject = GetSpawnObject( x , z );
	if( pObject != m_pcsOnMouseObject )
	{
		m_pcsOnMouseObject = pObject;
		Invalidate(FALSE );
	}

	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		// ���� ������..
		m_SelectedPosX2 = x;
		m_SelectedPosZ2 = z;

		m_pParent->InvalidateRegionView();
	}
	
	if( m_pcsSelectedObject && m_nRangeDragMode == MODE_REGIONMOVE )
	{
		FLOAT	fX , fZ ;

		fX		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( x );
		fZ		= m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_POSITION( z );

		if( LockDivision( GetDivisionIndexF( fX , fZ ) ) )
		{
			// �̵��� ����� üũ , �̵��� ����� üũ..
			// �ش� ����� ���̺� üũ..
			this->m_pParent->m_pMainDlg->m_ctlRegion.
				UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
									DUF_SPAWN	);

			m_pcsSelectedObject->m_stPosition.x	= fX;
			m_pcsSelectedObject->m_stPosition.z	= fZ;

			// �ش� ����� ���̺� üũ..
			this->m_pParent->m_pMainDlg->m_ctlRegion.
				UnsavedDataCheck(	GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x , m_pcsSelectedObject->m_stPosition.z ) ,
									DUF_SPAWN	);

			m_pParent->InvalidateRegionView();

			m_bUnsavedData = TRUE;
		}
	}

	return TRUE;
}

void	CRTMenuSpawn::DrawAreaSphere( CDC * pDC , FLOAT fX , FLOAT fY , FLOAT fZ , FLOAT fRadius , INT32 nType , CString *pStr )
{
	fX -= GetSectorStartX( ArrayIndexToSectorIndexX( 0 ) );
	fZ -= GetSectorStartZ( ArrayIndexToSectorIndexZ( 0 ) );

	INT32	nRegionX	=	( INT32 ) ( fX / ( 200.0f / m_fScale ) );
	INT32	nRegionZ	=	( INT32 ) ( fZ / ( 200.0f / m_fScale ) );
	INT32	nRadius		=	( INT32 ) ( fRadius / ( 200.0f / m_fScale ) );
	

	CRect	rectMap;

	rectMap.SetRect(
		nRegionX + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX - nRadius	,
		nRegionZ + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ - nRadius	,
		nRegionX + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX + nRadius ,
		nRegionZ + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ + nRadius	);

	CRect	rect;
	this->m_pParent->m_pMainDlg->m_ctlRegion.GetClientRect( rect );

	if ( rect.right > rectMap.left && rectMap.right > rect.left)
	{
		if (rect.bottom > rectMap.top && rectMap.bottom > rect.top )
		{
			// ȭ�鿡 ���Եȴ�..
			// 
			pDC->Ellipse( rectMap );			

			CRect	rectText;
			rectText.left	= nRegionX - 100	+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX;
			rectText.top	= nRegionZ - 10		+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ;
			rectText.right	= nRegionX + 100	+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX;
			rectText.bottom	= nRegionZ + 10		+ this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ;
			pDC->SetTextColor( RGB( 0 , 255 , 0 ) );
			pDC->DrawText( *pStr , rectText , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		}
	}
}

BOOL CRTMenuSpawn::OnDeleteKeyDown()
{
	if( m_pcsSelectedObject )
	{
		m_bUnsavedData = TRUE;

		// �ش� ����� ���̺� üũ..
		this->m_pParent->m_pMainDlg->m_ctlRegion.UnsavedDataCheck( GetDivisionIndexF( m_pcsSelectedObject->m_stPosition.x, m_pcsSelectedObject->m_stPosition.z ), DUF_SPAWN );

		g_pcsApmEventManager->RemoveEvent( g_pcsApmEventManager->GetEvent( m_pcsSelectedObject, APDEVENT_FUNCTION_SPAWN ) );
		g_pcsApmObject->DeleteObject( m_pcsSelectedObject );
		RefreshDivisionData();
		m_pcsSelectedObject = NULL;
		m_pParent->InvalidateRegionView();
	}

	return TRUE;
}

int	CRTMenuSpawn::GetObjectCountInBBox( AuBOX * pBox )
{
	if( NULL == pBox ) return 0;

	int nCount = 0;
	INT32			lIndex;
	ApdObject	*	pcsObject;
	ApdEvent	*	pEvent;

	lIndex = 0;

	INT32		nEventType = APDEVENT_FUNCTION_SPAWN;

	for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject														;
			pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

		if( pEvent )
		{
			if( pBox->inf.x					<	pcsObject->m_stPosition.x	&&
				pcsObject->m_stPosition.x	<=	pBox->sup.x					&&
				pBox->inf.z					<	pcsObject->m_stPosition.z	&&
				pcsObject->m_stPosition.z	<=	pBox->sup.z					)
			{
				nCount ++;
			}
		}
		else
		{
			// do nothing
		}
	}

	return nCount;
}

BOOL	CRTMenuSpawn::LockDivision		( UINT32 uDivisionIndex )
{
	return TRUE;
	/*
	if( g_bTeleportOnlyMode ) return TRUE;

	UINT32	nDivisionX , nDivisionZ ;

	nDivisionX	= GetDivisionXIndex( uDivisionIndex );
	nDivisionZ	= GetDivisionZIndex( uDivisionIndex );

	if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock & DUF_SPAWN )
	{
		// �̹� �� �Ǿ��ִ�.
		return TRUE;
	}
	else
	{
		char strFilename[ FILENAME_MAX ];
		wsprintf( strFilename , RM_RK_DIRECTORY_BIN APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN , uDivisionIndex );

		// �ش� ���� �� �� �������� �˻�..
		// �̹� ���� �Ǿ� �ִ��� �˻�..
		// �ȵǾ������� ���� �ϰ�..
		// �ȵŴ°�� ���� FALSE.

		if( IsLock( strFilename ) )
		{
			CString	strOwner;
			GetLockOwner( strFilename , strOwner );

			if( strOwner == this->m_pParent->m_pMainDlg->m_strUserName )
			{
				// �̹� ���ϰ� �ִ°�.

				// �� �÷��� ����..
				this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SPAWN;
				return TRUE;
			}
			else
			{
				// ���� ���� �־�.
				g_strLastLockOwnerName = strOwner;
				return FALSE;
			}
		}

		// ������ ������ ���ε� ��Ų��..
		if( IsExistFileToRemote( strFilename ) )
		{
			// �ֽ��������� Ȯ��..
			if( !IsLatestFile( strFilename ) )
			{
				if( FileDownLoad( strFilename ) )
				{
					// ���� �ޱ� ����..
				}
				else
				{
					MessageBox( "�ֽ����� �ޱ� ����!" );
					return FALSE;
				}
			}

			if( Lock( strFilename ) )
			{
				// �� �÷��� ����..
				this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SPAWN;
				return TRUE;
			}
			else
			{
				// �� ����
				return FALSE;
			}
		}
		else
		{
			// ������ü�� �����ϱ�..
			// �׳� �� ����.
			// �� �÷��� ����..
			this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock |= DUF_SPAWN;
			return TRUE;
		}

	}
	*/
}

BOOL	CRTMenuSpawn::UnlockDivision	( UINT32 uDivisionIndex )
{
	return TRUE;
	/*
	if( g_bTeleportOnlyMode ) return TRUE;

	UINT32	nDivisionX , nDivisionZ ;

	nDivisionX	= GetDivisionXIndex( uDivisionIndex );
	nDivisionZ	= GetDivisionZIndex( uDivisionIndex );

	if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock & DUF_SPAWN )
	{
		// ���� �Ǿ� �ִ°͸� ó��..
		if( this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uFlag & DUF_SPAWN )
		{
			// ���̺� ���� ���� ����Ÿ�� �ִ� ���...
			// �� ���� ����� �� �� ����..
			return FALSE;
		}
		else
		{
			// ���� ���...
			// ����Ÿ�� ������ �ø��� ���� Ǭ��.
			char strFilename[ FILENAME_MAX ];
			wsprintf( strFilename , RM_RK_DIRECTORY_BIN APMOBJECT_LOCAL_INI_FILE_NAME_2_SPAWN , uDivisionIndex );

			if( FileUpload( strFilename ) )
			{
				if( UnLock( strFilename ) )
				{
					this->m_pParent->m_pMainDlg->m_ctlRegion.m_bmpMap[ nDivisionX ][ nDivisionZ ].uLock &= ~DUF_SPAWN;
				}
//				else
//				{
//					MessageBox( "���� Ǫ�°� ����!" );
//				}
				return TRUE;
			}
			else
			{
				TRACE( "���� ���ε� ����..\n" );
				return FALSE;
			}
		}
	}
	else
	{
		// ���� ���� ���� �༮�̹Ƿ�... ó������ ����..
		return FALSE;
	}
	*/

}

void	CRTMenuSpawn::RefreshDivisionData()
{
	this->m_pParent->m_pMainDlg->m_ctlRegion.ClearAllIndex();

	INT32			lIndex			;
	ApdObject	*	pcsObject		;
	ApdEvent	*	pEvent			;
	AuPOS		*	pPos			;
	AgpdSpawn	*	pSpawn			;

	CString		strMobName;

	lIndex = 0;

	INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

	for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
			pcsObject														;
			pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
	{
		pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

		if( pEvent )
		{
			pSpawn = (AgpdSpawn *) pEvent->m_pvData;
			if( pEvent->m_pstCondition )
			{
				ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
				if( pArea )
				{
					switch( pArea->m_eType )
					{
					case	APDEVENT_AREA_SPHERE	:
						{
							pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );
							INT32	nDivision = GetDivisionIndexF( pPos->x , pPos->z );

							CRegionViewStatic::DivisionImageData *	pDivisionStruct = 
								this->m_pParent->m_pMainDlg->m_ctlRegion.GetMap( 
								GetDivisionXIndex( nDivision ),
								GetDivisionZIndex( nDivision ) );

							if( pDivisionStruct )
							{
								pDivisionStruct->m_vecIndex.push_back( pcsObject );
							}
						}
						break;
					case	APDEVENT_AREA_FAN		:
					case	APDEVENT_AREA_BOX		:
						{
							TRACE( "�������� �ʴ� Ÿ�� -_-" );
						}
						break;
					}
				}
			}
			else
			{
				// = =?
			}
			

		}
		else
		{
			// do nothing
		}
	}
}

void	CRTMenuSpawn::OnPaintOnMap_New( CDC * pDC )
{
	// ������Ʈ ���� ���...
	CPen	penRegion , penSelected;
	penRegion.CreatePen( PS_SOLID , 2 , RGB( 87 , 25 , 255 )	);
	penSelected.CreatePen( PS_SOLID , 2 , RGB( 255 , 75 , 75 )	);
	pDC->SelectObject( penRegion );
	pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );
	pDC->SetBkMode( TRANSPARENT );

	{
		
		//INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		AgpdSpawn	*	pSpawn			;
		//AgpdCharacterTemplate *	pcsTemplate;

		CString		strMobName;

		vector <CRegionViewStatic::DivisionImageData *>::iterator IterDivision;
		CRegionViewStatic::DivisionImageData *	pDivision;
		INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

		for (
			IterDivision = this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.begin( ) ;
			IterDivision != this->m_pParent->m_pMainDlg->m_ctlRegion.m_vecDisplayedDivision.end( ) ;
			IterDivision++ )
		{
			pDivision = (*IterDivision);
			switch( pDivision->eViewState )
			{
			case DI_OUTSIDE:
				break;
			case DI_OVERLAP:
			case DI_INSIDE:
				{
					// ���� �˻�
					vector < ApdObject * >::iterator Iter;
					for (
						Iter = pDivision->m_vecIndex.begin( ) ;
						Iter != pDivision->m_vecIndex.end( ) ;
						Iter++ )
					{
						pcsObject = * Iter;// g_pcsApmObject->GetObject( *Iter );
						if( pcsObject )
						{
							pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );
							if( pEvent && pEvent->m_pstCondition )
							{
								pSpawn = (AgpdSpawn *) pEvent->m_pvData;
								strMobName.Empty();
								strMobName.Format( "%s(%d)" , pSpawn->m_szName , pSpawn->m_stConfig.m_lMaxChar );

								ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
								if( pArea )
								{
									switch( pArea->m_eType )
									{
									case	APDEVENT_AREA_SPHERE	:
										{
											pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );

											if( m_pcsSelectedObject == pcsObject )
											{
												pDC->SelectObject( penSelected );
											}
											else
											{
												pDC->SelectObject( penRegion );
											}

											DrawAreaSphere( pDC , pPos->x , pPos->y , pPos->z , pArea->m_uoData.m_fSphereRadius , 0 , &strMobName );
										}
										break;
									case	APDEVENT_AREA_FAN		:
									case	APDEVENT_AREA_BOX		:
										{
											TRACE( "�������� �ʴ� Ÿ�� -_-" );
										}
										break;
									}
								}
							}

						}
					}


				}
				break;
			}
		}
	}
	
	// Selection ����..
	if( m_nRangeDragMode == MODE_RANGESELECTED )
	{
		CPen	penSelection;
		penSelection.CreatePen( PS_SOLID , 2 , RGB( 255, 128 , 128 )	);
		pDC->SelectObject( penSelection );
		pDC->SelectObject( GetStockObject( HOLLOW_BRUSH ) );

		int nRadius1 = abs( m_SelectedPosX2 - m_SelectedPosX1 );
		int nRadius2 = abs( m_SelectedPosZ2 - m_SelectedPosZ1 );

		int nRadius = nRadius1 < nRadius2 ? nRadius2 : nRadius1;

		CRect	rectEllipse;
		rectEllipse.SetRect(
			m_SelectedPosX1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 - nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ,
			m_SelectedPosX1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetX,
			m_SelectedPosZ1 + nRadius + this->m_pParent->m_pMainDlg->m_ctlRegion.m_nOffsetZ);

		pDC->Ellipse( rectEllipse );

		if( rectEllipse.Width() < 100 )
		{
			rectEllipse.left	-= 50;
			rectEllipse.right	+= 50;
		}
		if( rectEllipse.Height() < 100 )
		{
			rectEllipse.top		-= 10;
			rectEllipse.bottom	+= 10;
		}

		pDC->SetBkMode( TRANSPARENT );
		
		CString	str;
		str.Format( "%.1fm" , m_pParent->m_pMainDlg->m_ctlRegion.REGION_OFFSET_TO_LENGTH( nRadius ) / 100.0f);
		pDC->SetTextColor( RGB( 255 , 255 , 255 ) );
		pDC->DrawText( str , rectEllipse , DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
}

AuNode< ApmMap::RegionElement > *	CRTMenuSpawn::GetRegion( INT32 nOffsetX , INT32 nOffsetZ )
{
	CRegionMenuDlg::MenuInfo * pInfo = &this->m_pParent->m_vectorMenuWnd[ 0 ];
	CRTMenuRegion * pMenuRegion = (CRTMenuRegion * )pInfo->pMenu;
	// ������Ʈ ����Ʈ �ڿ��� ���� �˻��� �ִ´�.
	// ���������� �������� ���� ���� ������Ʈ�� ���� �Ǵ� �ý���..
	// ���������� Ctrl �� ������ � ������ ���� ���� Ȯ���� �����ϴ�.

	AuNode< ApmMap::RegionElement > * pNode = pMenuRegion->m_listElement.GetTailNode();
	ApmMap::RegionElement * pElement;

	while( pNode )
	{
		pElement	= & pNode->GetData();

		if( pElement->nStartX <= nOffsetX && nOffsetX <= pElement->nEndX	&&
			pElement->nStartZ <= nOffsetZ && nOffsetZ <= pElement->nEndZ	)
		{
			return pNode;
		}

		pNode = pNode->GetPrevNode();
	}

	return NULL;
	// �׷��� ����..
}

void CRTMenuSpawn::OnRButtonDown(UINT nFlags, CPoint point)
{
	
	if( IDOK == MessageBox( "���� ������ �������Ϸ� �����ϽǷ���?" , "����" , MB_OKCANCEL ) )
	{
		FILE	* pFile = fopen( "Spawn.txt" , "wt" );
		if( NULL == pFile )
		{
			MessageBox( "���� ���½���" );
			return;
		}
		// ����
		// ���� , Division , Region , Position
		fprintf( pFile , "�����׷�\t����\tDivision\tRegion\tPosX\tPosZ\n" );

		INT32			lIndex			;
		ApdObject	*	pcsObject		;
		ApdEvent	*	pEvent			;
		AuPOS		*	pPos			;
		AgpdSpawn	*	pSpawn			;
		AgpdCharacterTemplate *	pcsTemplate;

		lIndex = 0;

//		INT32		nEventType = APDEVENT_FUNCTION_NATURE	;
		INT32		nEventType = APDEVENT_FUNCTION_SPAWN	;

		for (	pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	;
				pcsObject														;
				pcsObject = g_pcsApmObject->GetObjectSequence(&lIndex)	)
		{
			pEvent	= g_pcsApmEventManager->GetEvent( pcsObject , ( ApdEventFunction ) nEventType );

			if( pEvent )
			{
				pSpawn = (AgpdSpawn *) pEvent->m_pvData;
				CString		strSpawnGroupName;

				strSpawnGroupName.Format( "%s" , pSpawn->m_szName );//, pSpawn->m_stConfig.m_lMaxChar );

				for ( int lIndex = 0; lIndex < pSpawn->m_stConfig.m_lSpawnChar; ++lIndex)
				{
					pcsTemplate = g_pcsAgpmCharacter->GetCharacterTemplate(pSpawn->m_stConfig.m_astChar[lIndex].m_lCTID);
					if (!pcsTemplate)
						continue;

					char * pStrMobName = pcsTemplate->m_szTName;

					if( pEvent->m_pstCondition )
					{
						ApdEventConditionArea *	pArea = pEvent->m_pstCondition->m_pstArea;
						if( pArea )
						{
							switch( pArea->m_eType )
							{
							case	APDEVENT_AREA_SPHERE	:
								{
									pPos = g_pcsApmEventManager->GetBasePos( pcsObject , NULL );

									INT32	nSectorX ,nSectorZ;
									ApWorldSector  * pSector = g_pcsApmMap->GetSector( *pPos , &nSectorX , NULL , &nSectorZ );

									INT32 x , z;
									x = ( INT32 ) ( ( pPos->x - GetSectorStartX	( nSectorX )	) / MAP_STEPSIZE );
									z = ( INT32 ) ( ( pPos->z - GetSectorStartZ	( nSectorZ )	) / MAP_STEPSIZE );

									INT32	nOffsetX , nOffsetZ;
									AuNode< ApmMap::RegionElement > * pNode;
									nOffsetX = ( SectorIndexToArrayIndexX( nSectorX ) * MAP_DEFAULT_DEPTH + x ) * 2;
									nOffsetZ = ( SectorIndexToArrayIndexZ( nSectorZ ) * MAP_DEFAULT_DEPTH + z ) * 2;

									CString	strRegion;
									if( pNode = GetRegion( nOffsetX , nOffsetZ ) )
									{
										ApmMap::RegionTemplate * pTemplate = g_pcsApmMap->GetTemplate( pNode->GetData().nIndex );

										strRegion.Format( "(%d)%s" , pNode->GetData().nIndex , pTemplate->pStrName );
									}
									else
									{
										strRegion = "Unknown";
									}
																	
									fprintf( pFile , "%s\t%s\t%d\t%s\t%f\t%f\n" ,
										strSpawnGroupName,
										pStrMobName,
										GetDivisionIndexF( pPos->x , pPos->z ) ,
										strRegion ,
										pPos->x ,
										pPos->z );
								}
								break;
							case	APDEVENT_AREA_FAN		:
							case	APDEVENT_AREA_BOX		:
								{
									TRACE( "�������� �ʴ� Ÿ�� -_-" );
								}
								break;
							}
						}
					}
					else
					{
						// = =?
					}
					break;
				}
			}
			else
			{
				// do nothing
			}
		}

		fclose( pFile );
		MessageBox( "Ok" );
	}

	CRTMenuBase::OnRButtonDown(nFlags, point);
}
