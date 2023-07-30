// Plugin_LineBlock.cpp : implementation file
//

#include "stdafx.h"
#include "MapTool.h"
#include "Plugin_LineBlock.h"
#include "MainWindow.h"
#include "MainFrm.h"

extern MainWindow			g_MainWindow	;
// CPlugin_LineBlock

IMPLEMENT_DYNAMIC(CPlugin_LineBlock, CWnd)

CPlugin_LineBlock::CPlugin_LineBlock() : 
	m_bDrawingNow			( FALSE		)	,
	m_fBlockingVisibleHeight( 2000.0f	)	,
	m_fBlockingAlphaValue	( 0.7f		)	,
	m_bSelectMode			( FALSE		)	,
	m_bGridSnap				( TRUE		)	,
	m_bBackfaceCull			( FALSE		)	,
	m_bShowServer			( TRUE		)	,
	m_bShowClient			( TRUE		)	,
	m_bShowGeometryBlock	( TRUE		)	,
	m_bExportServer			( FALSE		)	,
	m_pPluginBlocking		( NULL		)	
{
	m_strShortName = "Line.B";

	m_rectBackfaceCull	.SetRect( 50 , 30 , 250 , 60 )		;
	m_rectGridSnap		.SetRect( 50 , 70 , 250 , 100 )		;
	m_rectServerData	.SetRect( 50 , 110 , 250 , 140 )	;
	m_rectClientOnly	.SetRect( 50 , 150 , 250 , 180 )	;
	m_rectGeomtryBlock	.SetRect( 50 , 190 , 250 , 220 )	;
}

CPlugin_LineBlock::~CPlugin_LineBlock()
{
	CleanUp();
}

BOOL CPlugin_LineBlock::OnCleanUpData			()
{
	CleanUp();
	return TRUE;
}


BEGIN_MESSAGE_MAP(CPlugin_LineBlock, CWnd)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CPlugin_LineBlock message handlers

BOOL CPlugin_LineBlock::OnQuerySaveData		( char * pStr )
{
	strcpy( pStr , "Line Blocking ��������" );
	return TRUE;
}
BOOL CPlugin_LineBlock::OnSaveData				()
{
	char	strSubDataPath[ MAX_PATH ];
	GetSubDataDirectory( strSubDataPath );

	//char	strBlocking		[ MAX_PATH ];
	//wsprintf( strBlocking		, "%s\\" ALEF_GEOMETRY_BLOCKING_SAVE_FILE_PATH , strSubDataPath );

	_CreateDirectory( strSubDataPath );
	_CreateDirectory( "%s\\map" , strSubDataPath );
	_CreateDirectory( "%s\\map\\data" , strSubDataPath );
	_CreateDirectory( "%s\\map\\data\\blocking" , strSubDataPath );

	return 	AGCMMAP_THIS->EnumLoadedDivision( CBDivisionSaveCallback , this );
}

BOOL CPlugin_LineBlock::OnLoadData				()
{
	return 	AGCMMAP_THIS->EnumLoadedDivision( CBDivisionLoadCallback , this );
}

#define ALEF_LINE_BLOCKING_SAVE_FILE_PATH		"map\\data\\blocking"
#define ALEF_LINE_BLOCKING_SAVE_FILE_NAME		"LB%04d.dat"

BOOL	CPlugin_LineBlock::CBDivisionSaveCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_LineBlock * pThis = ( CPlugin_LineBlock * ) pData;
	char	strSubDataPath[ MAX_PATH ];
	GetSubDataDirectory( strSubDataPath );

	char	strFilename[ MAX_PATH ];
	wsprintf( strFilename , "%s\\" ALEF_LINE_BLOCKING_SAVE_FILE_PATH "\\" ALEF_LINE_BLOCKING_SAVE_FILE_NAME , 
		ALEF_CURRENT_DIRECTORY , pDivisionInfo->nIndex );

	if( !pThis->Save( pDivisionInfo->nIndex , strFilename ) )
	{
		TRACE( "��ŷ ���� �������!" );
	}

	wsprintf( strFilename , "%s\\" ALEF_LINE_BLOCKING_SAVE_FILE_PATH "\\" ALEF_LINE_BLOCKING_SAVE_FILE_NAME , 
		strSubDataPath , pDivisionInfo->nIndex );

	if( !pThis->Save( pDivisionInfo->nIndex , strFilename ) )
	{
		TRACE( "��ŷ ���� ����� �������!" );
	}

	return TRUE;
}

BOOL	CPlugin_LineBlock::CBDivisionLoadCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_LineBlock * pThis = ( CPlugin_LineBlock * ) pData;

	if( !pThis->Load( pDivisionInfo->nIndex ) )
	{
		TRACE( "����Ÿ �б� ����!" );
	}

	return TRUE;
}

BOOL	CPlugin_LineBlock::Division::Save( char * pFileName )
{
	INT32	nVersion = LINE_BLOCK_FILE_VERSION;
	INT32	nServerCount = 0 ;
	INT32	nClientCount = 0 ;

	vector< AuLineMaptool >::iterator iter;
	for( iter = vecLine.begin() ;
		iter != vecLine.end() ;
		iter ++ )
	{
		AuLineMaptool	line = *iter;
		if( line.eType == AuLineMaptool::SERVER		) nServerCount++;
		if( line.eType == AuLineMaptool::CLIENTONLY	) nClientCount++;
	}

	if( nServerCount == 0 && nClientCount == 0) return FALSE; 

	FILE	* pFile;
	pFile = fopen( pFileName , "wb" );

	if( !pFile )
	{
		//MessageBox( "���� ���� ����!" );
		return FALSE;
	}

	fwrite( ( void * ) &nVersion , sizeof INT32 , 1 , pFile );
	fwrite( ( void * ) &nServerCount , sizeof INT32 , 1 , pFile );

	// ���� ����
	// ��������Ÿ ���� ����.

	FLOAT	fDivisionStartX = GetSectorStartX( GetFirstSectorXInDivision( nIndex ) );
	FLOAT	fDivisionStartZ = GetSectorStartZ( GetFirstSectorZInDivision( nIndex ) );

	for( iter = vecLine.begin() ;
		iter != vecLine.end() ;
		iter ++ )
	{
		AuLineMaptool	line = *iter;
		if( line.eType != AuLineMaptool::SERVER ) continue;

		line.start.x	-= fDivisionStartX;
		line.start.z	-= fDivisionStartZ;
		line.end.x		-= fDivisionStartX;
		line.end.z		-= fDivisionStartZ;

		// Write

		fwrite( ( void * ) &line , sizeof AuLine , 1 , pFile );
	}

	fwrite( ( void * ) &nClientCount , sizeof INT32 , 1 , pFile );

	for( iter = vecLine.begin() ;
		iter != vecLine.end() ;
		iter ++ )
	{
		AuLineMaptool	line = *iter;
		if( line.eType != AuLineMaptool::CLIENTONLY ) continue;

		line.start.x	-= fDivisionStartX;
		line.start.z	-= fDivisionStartZ;
		line.end.x		-= fDivisionStartX;
		line.end.z		-= fDivisionStartZ;

		// Write

		fwrite( ( void * ) &line , sizeof AuLine , 1 , pFile );
	}
	fclose( pFile );

	return TRUE;
}

BOOL	CPlugin_LineBlock::Division::Load()
{
	char	strFilename[ MAX_PATH ];
	wsprintf( strFilename , "%s\\" ALEF_LINE_BLOCKING_SAVE_FILE_PATH "\\" ALEF_LINE_BLOCKING_SAVE_FILE_NAME , 
		ALEF_CURRENT_DIRECTORY , nIndex );

	FILE	* pFile;
	pFile = fopen( strFilename , "rb" );

	if( !pFile )
	{
		// ������ ����������
		return FALSE;
	}

	INT32	nVersion;
	fread( ( void * ) &nVersion , sizeof INT32 , 1 , pFile );

	INT32	nCountServer;
	fread( ( void * ) &nCountServer , sizeof INT32 , 1 , pFile );

	if( nVersion != LINE_BLOCK_FILE_VERSION )
	{
		//MessageBox( "������ �ٸ��ϴ�" );
		fclose( pFile );
		return FALSE;
	}

	// �ѹ� ����..
	vecLine.clear();

	// ���� ����

	FLOAT	fDivisionStartX = GetSectorStartX( GetFirstSectorXInDivision( nIndex ) );
	FLOAT	fDivisionStartZ = GetSectorStartZ( GetFirstSectorZInDivision( nIndex ) );

	for( int i = 0 ; i < nCountServer ; i ++ )
	{
		AuLineMaptool	line;
		line.eType = AuLineMaptool::SERVER;
		fread( ( void * ) &line , sizeof AuLine , 1 , pFile );

		line.start.x	+= fDivisionStartX;
		line.start.z	+= fDivisionStartZ;
		line.end.x		+= fDivisionStartX;
		line.end.z		+= fDivisionStartZ;

		vecLine.push_back( line );
	}

	INT32	nCountClient;
	fread( ( void * ) &nCountClient , sizeof INT32 , 1 , pFile );
	for( int i = 0 ; i < nCountClient ; i ++ )
	{
		AuLineMaptool	line;
		line.eType = AuLineMaptool::CLIENTONLY;
		fread( ( void * ) &line , sizeof AuLine , 1 , pFile );

		line.start.x	+= fDivisionStartX;
		line.start.z	+= fDivisionStartZ;
		line.end.x		+= fDivisionStartX;
		line.end.z		+= fDivisionStartZ;

		vecLine.push_back( line );
	}
	fclose( pFile );

	return TRUE;
}

BOOL	CPlugin_LineBlock::Save( INT32 nDivision , char * pFileName )
{
	vector< Division * >::iterator iter;
	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		if( pDivision->nIndex == nDivision )
		{
			return pDivision->Save( pFileName );
		}
	}

	// ã�� �� ����
	return FALSE;
}

BOOL	CPlugin_LineBlock::Load( INT32 nDivision )
{
	vector< Division * >::iterator iter;
	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		if( pDivision->nIndex == nDivision )
		{
			// �̹� �ε� �Ǿ�����.
			return TRUE;
		}
	}

	// ã�� �� ����
	Division * pDivision = new Division;
	pDivision->nIndex = nDivision;
	BOOL bRet = pDivision->Load();

	if( bRet )
	{
		m_vecDivision.push_back( pDivision );
		return TRUE;
	}
	else
	{
		// �հ� ������ �ִ�.
		delete pDivision;
		return FALSE;
	}
}

void	CPlugin_LineBlock::CleanUp()
{
	// �� ������.

	vector< Division * >::iterator iter;
	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		delete pDivision;
	}

	m_vecDivision.clear();
}

CPlugin_LineBlock::Division *		CPlugin_LineBlock::GetDivision( INT32 nDivision )
{
	vector< Division * >::iterator iter;
	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		if( pDivision->nIndex == nDivision )
		{
			return pDivision;
		}
	}

	return NULL;
}

CPlugin_LineBlock::Division *		CPlugin_LineBlock::AddDivision( INT32 nDivision )
{
	Division * pDivision = GetDivision( nDivision );
	if( pDivision ) return pDivision;

	pDivision = new Division;
	pDivision->nIndex = nDivision;
	
	m_vecDivision.push_back( pDivision );

	return pDivision;
}

void	CPlugin_LineBlock::DrawBlocking( AuLineMaptool * pLine , TYPELINE eType )
{
	RwIm3DVertex 		pLineList[ 8 ]	;
	RwImVertexIndex		pIndex[ 12 ]	;

	float fsin = sin( 3.1415927f * ( float ) ( GetTickCount() ) / 300.0f );
	INT32 nAlpha = 96 + 32 + ( int ) ( 32 * fsin );
	INT32 nLowAlpha = 0;

	if( pLine->eType == AuLineMaptool::SERVER && !m_bShowServer )
		
		return;
	if( pLine->eType == AuLineMaptool::CLIENTONLY && !m_bShowClient ) return;

	if( pLine->eType == AuLineMaptool::SERVER )
	{
		// ��������Ÿ�� ���ϰ� ǥ����.
		nAlpha = 196;
		nLowAlpha = nAlpha;
	}

	RwUInt32    colorFrontLow , colorBackLow;
	RwUInt32    colorFrontHigh , colorBackHigh;

	switch( eType )
	{
	case NORMAL_BLOCKING:
		{
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , nAlpha  );
			colorFrontLow = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , nAlpha  );
			colorBackLow = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , nLowAlpha  );
			colorFrontHigh = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , nLowAlpha  );
			colorBackHigh = pLineList[ 0 ].color;
		}

		break;
	case SELECTING_NOW:
		{
			float fsin = sin( 3.1415927f * ( float ) ( GetTickCount() ) / 150.0f );

			nAlpha = 127 + 64 + ( int ) ( 64 * fsin );

			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , nAlpha  );
			colorFrontLow = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , nAlpha  );
			colorBackLow = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 255 , 0 , 0 , nLowAlpha  );
			colorFrontHigh = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 0 , 0 , 255 , nLowAlpha  );
			colorBackHigh = pLineList[ 0 ].color;
		}
		break;
	case SELECTED:
		{
			float fsin = sin( 3.1415927f * ( float ) ( GetTickCount() ) / 500.0f );
			nAlpha = 127 + 64 + ( int ) ( 64 * fsin );

			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 227 , 232 , 53 , nAlpha  );
			colorFrontLow = pLineList[ 0 ].color;
			colorBackLow = pLineList[ 0 ].color;
			RwIm3DVertexSetRGBA	( &pLineList[ 0 ] , 227 , 232 , 53 , nLowAlpha  );
			colorFrontHigh = pLineList[ 0 ].color;
			colorBackHigh = pLineList[ 0 ].color;
		}
	}

	RwIm3DVertexSetPos	( &pLineList[ 0 ] , pLine->start.x , pLine->start.y + m_fBlockingVisibleHeight , pLine->start.z	);
	RwIm3DVertexSetU	( &pLineList[ 0 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 0 ] , 1.0f				);
	pLineList[ 0 ].color = colorFrontHigh;

	RwIm3DVertexSetPos	( &pLineList[ 1 ] , pLine->end.x , pLine->end.y + m_fBlockingVisibleHeight , pLine->end.z	);
	RwIm3DVertexSetU	( &pLineList[ 1 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 1 ] , 1.0f				);
	pLineList[ 1 ].color = colorFrontHigh;
	
	RwIm3DVertexSetPos	( &pLineList[ 2 ] , pLine->start.x , pLine->start.y , pLine->start.z	);
	RwIm3DVertexSetU	( &pLineList[ 2 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 2 ] , 1.0f				);
	pLineList[ 2 ].color = colorFrontLow;
	
	RwIm3DVertexSetPos	( &pLineList[ 3 ] , pLine->end.x , pLine->end.y , pLine->end.z	);
	RwIm3DVertexSetU	( &pLineList[ 3 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 3 ] , 1.0f				);
	pLineList[ 3 ].color = colorFrontLow;

	RwIm3DVertexSetPos	( &pLineList[ 4 ] , pLine->end.x , pLine->end.y + m_fBlockingVisibleHeight , pLine->end.z	);
	RwIm3DVertexSetU	( &pLineList[ 4 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 4 ] , 1.0f				);
	pLineList[ 4 ].color = colorBackHigh;

	RwIm3DVertexSetPos	( &pLineList[ 5 ] , pLine->start.x , pLine->start.y + m_fBlockingVisibleHeight , pLine->start.z	);
	RwIm3DVertexSetU	( &pLineList[ 5 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 5 ] , 1.0f				);
	pLineList[ 5 ].color = colorBackHigh;

	RwIm3DVertexSetPos	( &pLineList[ 6 ] , pLine->end.x , pLine->end.y , pLine->end.z	);
	RwIm3DVertexSetU	( &pLineList[ 6 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 6 ] , 1.0f				);
	pLineList[ 6 ].color = colorBackLow;
	
	RwIm3DVertexSetPos	( &pLineList[ 7 ] , pLine->start.x , pLine->start.y , pLine->start.z	);
	RwIm3DVertexSetU	( &pLineList[ 7 ] , 1.0f				);    
	RwIm3DVertexSetV	( &pLineList[ 7 ] , 1.0f				);
	pLineList[ 7 ].color = colorBackLow;

	pIndex[ 0 ]	= 1;
	pIndex[ 1 ]	= 2;
	pIndex[ 2 ]	= 0;
	pIndex[ 3 ]	= 1;
	pIndex[ 4 ]	= 3;
	pIndex[ 5 ]	= 2;

	pIndex[ 6 ]	= 5;
	pIndex[ 7 ]	= 6;
	pIndex[ 8 ]	= 4;
	pIndex[ 9 ]	= 5;
	pIndex[ 10]	= 7;
	pIndex[ 11]	= 6;

	if( RwIm3DTransform( pLineList , 8 , NULL, rwIM3D_VERTEXRGBA ) )
	{
		if( m_bBackfaceCull )
		{
			RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST,
				pIndex , 6 );
		}
		else
		{
			RwIm3DRenderIndexedPrimitive( rwPRIMTYPETRILIST,
				pIndex , 12 );
		}

		RwIm3DEnd();
	}
}

BOOL CPlugin_LineBlock::OnLButtonDownGeometry	( RwV3d * pPos )
{
	if( m_bDrawingNow )
	{
		m_lineDrawing.end.x = pPos->x; 
		m_lineDrawing.end.y = pPos->y; 
		m_lineDrawing.end.z = pPos->z; 

		INT32 nDivision = GetDivisionIndexF( m_lineDrawing.start.x , m_lineDrawing.start.z );
		Division * pDivision = GetDivision( nDivision );

		if( NULL == pDivision )
		{
			VERIFY( pDivision = AddDivision( nDivision ) );
		}

		pDivision->vecLine.push_back( m_lineDrawing );
		m_lineDrawing.start = m_lineDrawing.end;

		SetSaveData();
	}
	else
	{
		// �ƽ�
		m_bDrawingNow = TRUE;

		// Ŭ���Ҷ� Ctrl�� ��Ȳ�� Ȯ���Ѵ�.
		if( GetAsyncKeyState( VK_CONTROL ) < 0 )
		{
			m_lineDrawing.eType	= AuLineMaptool::SERVER;
			if( !m_bShowServer )
			{
				m_bShowServer = TRUE;	// ������ ������ �ڵ� �� ��Ŵ
				Invalidate( FALSE );
			}
		}
		else
		{
			m_lineDrawing.eType	= AuLineMaptool::CLIENTONLY;
			if( !m_bShowClient )
			{
				m_bShowClient = TRUE;	// ������ ������ �ڵ� �� ��Ŵ
				Invalidate( FALSE );
			}
		}

		if( m_bGridSnap )
		{
			ApWorldSector * pSector;
			pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );

			if( pSector )
			{
				FLOAT	applyX , applyZ;
				applyX = ( pSector->GetStepSizeX() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pPos->x - pSector->GetXStart() ) / ( pSector->GetStepSizeX() / 2 ) ) );
				applyZ = ( pSector->GetStepSizeZ() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pPos->z - pSector->GetZStart() ) / ( pSector->GetStepSizeZ() / 2 ) ) );
				
				pPos->x = pSector->GetXStart() + applyX;
				pPos->z = pSector->GetZStart() + applyZ;
			}
		}

		m_lineDrawing.start.x = pPos->x; 
		m_lineDrawing.start.y = pPos->y; 
		m_lineDrawing.start.z = pPos->z; 

		m_lineDrawing.end = m_lineDrawing.start;
	}
	return TRUE;
}

BOOL CPlugin_LineBlock::Window_OnLButtonUp		( RsMouseStatus *ms )
{
	if( m_bSelectMode == 2)
	{
		 m_bSelectMode = TRUE;
	}

	return TRUE;
}

BOOL CPlugin_LineBlock::Window_OnLButtonDown	( RsMouseStatus *ms )
{
	// Line Collision �� ��..
	if( m_bDrawingNow ) return FALSE;
	
	vector< Division * >::iterator iter;
	Division *					pDivisionFound = NULL;
	vector< AuLineMaptool >::iterator	pLineIterFound = NULL;
	BOOL						bFound			= FALSE;

	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		FLOAT fDistance = 99999999999999999999999.0f;

		for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
			iterLine != pDivision->vecLine.end();
			iterLine ++ )
		{
			AuLineMaptool * pLine = &( *iterLine );

			// �ɼ��� ������ Ŭ���� �ȵŰ� �ؾ���.
			if( pLine->eType == AuLineMaptool::SERVER && !m_bShowServer ) continue;
			if( pLine->eType == AuLineMaptool::CLIENTONLY && !m_bShowClient ) continue;

			if( CollisionCheck( pLine , ms , &fDistance ) )
			{
				// iterator ����.
				pDivisionFound = pDivision;
				pLineIterFound = iterLine;
				bFound = TRUE;
			}
			// 
		}
	}

	if( !bFound )
	{
		m_bSelectMode = FALSE;
		ClearSelection();
		return FALSE;
	}

	/*
	// ����Ʈ ���� ��.
	// ������ ���ý��� ��ġ�°��� ã��.
	m_iterVecLineBegin	= pLineIterFound;
	m_iterVecLineEnd	= ++pLineIterFound;

	// ������ �ڷ� Ȯ��..
	// �ϴ� �ڷ� Ȯ��

	AuLineMaptool * pLinePrev = &( * m_iterVecLineBegin );
	AuLineMaptool * pLineCurrent;

	while( m_iterVecLineEnd != pDivisionFound->vecLine.end() )
	{
		pLineCurrent = &( * m_iterVecLineEnd );

		if( pLinePrev->end.x == pLineCurrent->start.x &&
			pLinePrev->end.z == pLineCurrent->start.z )
		{
			pLinePrev = pLineCurrent;
			m_iterVecLineEnd++;
		}
		else
		{
			break;
		}
	}

	// ������ Ȯ��

	while( m_iterVecLineBegin != pDivisionFound->vecLine.begin() )
	{
		vector< AuLineMaptool >::iterator	pIterCurrent;

		pLinePrev		= &( * m_iterVecLineBegin );

		// ������ ���س�.
		pIterCurrent	= m_iterVecLineBegin;
		pIterCurrent--;

		pLineCurrent	= &( * pIterCurrent );


		if( pLineCurrent->end.x == pLinePrev->start.x &&
			pLineCurrent->end.z == pLinePrev->start.z )
		{
			m_iterVecLineBegin--;
		}
		else
		{
			break;
		}
	}


	*/
	m_bSelectMode = 2;
	
	(*pLineIterFound).bSelected = TRUE;

	return TRUE;
}

BOOL	CPlugin_LineBlock::CollisionCheck( AuLineMaptool * pLine , RsMouseStatus *ms , FLOAT * pfDistance )
{
	// pfDistance���� ���� ũ�� �浹���� �����Ѵ�.
	RwLine			pixelRay		;
	RwCameraCalcPixelRay( g_pEngine->m_pCamera , &pixelRay , &ms->pos );
	RwV3d	v3dLineDelta;
	RwV3dSub( &v3dLineDelta , &pixelRay.end, &pixelRay.start );

	RwV3d	vPoint[ 4 ];
	#define SET_V3D( v , fx , fy , fz ) { v.x = fx; v.y = fy ; v.z = fz; }
	SET_V3D	( vPoint[ 0 ] , pLine->start.x , pLine->start.y + m_fBlockingVisibleHeight , pLine->start.z	);
	SET_V3D	( vPoint[ 1 ] , pLine->end.x , pLine->end.y + m_fBlockingVisibleHeight , pLine->end.z	);
	SET_V3D	( vPoint[ 2 ] , pLine->start.x , pLine->start.y , pLine->start.z	);
	SET_V3D	( vPoint[ 3 ] , pLine->end.x , pLine->end.y , pLine->end.z	);

	BOOL bFound = FALSE;

	// 4�� ����.
	FLOAT fDistance , fDistanceNearest;
	if( RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
		&vPoint[ 1 ],
		&vPoint[ 2 ],
		&vPoint[ 0 ],
		&fDistance
		)
	)
	{
		bFound = TRUE;
		fDistanceNearest = fDistance;
	}
	if( RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
		&vPoint[ 1 ],
		&vPoint[ 3 ],
		&vPoint[ 2 ],
		&fDistance
		)
	)
	{
		if( bFound )
		{
			if( fDistance < fDistanceNearest )
				fDistanceNearest = fDistance;
		}
		else
		{
			bFound = TRUE;
			fDistanceNearest = fDistance;
		}
	}
	if( RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
		&vPoint[ 0 ],
		&vPoint[ 3 ],
		&vPoint[ 1 ],
		&fDistance
		)
	)
	{
		if( bFound )
		{
			if( fDistance < fDistanceNearest )
				fDistanceNearest = fDistance;
		}
		else
		{
			bFound = TRUE;
			fDistanceNearest = fDistance;
		}
	}
	if( RtIntersectionLineTriangle( &pixelRay.start , &v3dLineDelta , 
		&vPoint[ 0 ],
		&vPoint[ 2 ],
		&vPoint[ 3 ],
		&fDistance
		)
	)
	{
		if( bFound )
		{
			if( fDistance < fDistanceNearest )
				fDistanceNearest = fDistance;
		}
		else
		{
			bFound = TRUE;
			fDistanceNearest = fDistance;
		}
	}

	if( bFound )
	{
		if( fDistanceNearest > *pfDistance )
		{
			return FALSE;
		}
		else
		{
			*pfDistance = fDistanceNearest;
			return TRUE;
		}
	}
	else
	{
		return FALSE;
	}
}

BOOL CPlugin_LineBlock::OnMouseMoveGeometry	( RwV3d * pPos )
{
	if( m_bDrawingNow && pPos )
	{
		if( m_bGridSnap )
		{
			ApWorldSector * pSector;
			pSector = g_pcsApmMap->GetSector( pPos->x , pPos->z );

			if( pSector )
			{
				FLOAT	applyX , applyZ;
				applyX = ( pSector->GetStepSizeX() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pPos->x - pSector->GetXStart() ) / ( pSector->GetStepSizeX() / 2 ) ) );
				applyZ = ( pSector->GetStepSizeZ() / 2 ) * ( FLOAT ) ( ( INT32 ) ( ( pPos->z - pSector->GetZStart() ) / ( pSector->GetStepSizeZ() / 2 ) ) );
				
				pPos->x = pSector->GetXStart() + applyX;
				pPos->z = pSector->GetZStart() + applyZ;
			}
		}

		m_lineDrawing.end.x = pPos->x; 
		m_lineDrawing.end.y = pPos->y; 
		m_lineDrawing.end.z = pPos->z; 
	}

	if( m_pPluginBlocking ) m_pPluginBlocking->OnMouseMoveGeometry( pPos );

	return TRUE;
}

BOOL CPlugin_LineBlock::Window_OnKeyDown		( RsKeyStatus *ks )
{
	switch( ks->keyCharCode )
	{
	case rsESC:
		{
			// ���� ���
			m_bDrawingNow = FALSE;
			m_bSelectMode = FALSE;
			ClearSelection();
		}
		break;

	case rsDEL:
		{
			if( m_bSelectMode )
			{
				vector< Division * >::iterator iter;
				Division *					pDivisionFound = NULL;
				vector< AuLineMaptool >::iterator	pLineIterFound = NULL;

				for( iter = m_vecDivision.begin() ;
					iter != m_vecDivision.end() ;
					iter ++ )
				{
					Division * pDivision = ( Division * ) *iter;

					BOOL bStillSelected = TRUE;

					while( bStillSelected )
					{
						bStillSelected = FALSE;
						for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
							iterLine != pDivision->vecLine.end();
							iterLine ++ )
						{
							AuLineMaptool * pLine = &( *iterLine );
							
							if( pLine->bSelected )
							{
								pDivision->vecLine.erase( iterLine );
								bStillSelected = TRUE;
								break;
							}
						}
					}
				}

				/*
				m_iterVecLineBegin = NULL;
				m_iterVecLineEnd = NULL;
				*/
				ClearSelection();
				m_bSelectMode = FALSE;

				SetSaveData();
			}
		}
		break;
	}
	return TRUE;
}

BOOL CPlugin_LineBlock::Window_OnMouseMove		( RsMouseStatus *ms )
{
	if( m_bSelectMode != 2 ) return FALSE;
	
	vector< Division * >::iterator iter;
	Division *					pDivisionFound = NULL;
	vector< AuLineMaptool >::iterator	pLineIterFound = NULL;
	BOOL						bFound			= FALSE;

	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		FLOAT fDistance = 99999999999999999999999.0f;

		for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
			iterLine != pDivision->vecLine.end();
			iterLine ++ )
		{
			AuLineMaptool * pLine = &( *iterLine );

			if( CollisionCheck( pLine , ms , &fDistance ) )
			{
				// iterator ����.
				pDivisionFound = pDivision;
				pLineIterFound = iterLine;
				bFound = TRUE;
			}
			// 
		}
	}

	if( !bFound )
	{
		return FALSE;
	}

	(*pLineIterFound).bSelected = TRUE;

	return TRUE;
}

BOOL CPlugin_LineBlock::OnWindowRender			()
{
	if( m_pPluginBlocking && m_bShowGeometryBlock ) m_pPluginBlocking->OnWindowRender();

	RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLBACK	);

	vector< Division * >::iterator iter;
	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		if( m_bSelectMode )
		{
			for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
				iterLine != pDivision->vecLine.end();
				iterLine ++ )
			{
				AuLineMaptool * pLine = &( *iterLine );

				if( pLine->bSelected )
				{
					DrawBlocking( pLine , SELECTED );
				}
				else
				{
					DrawBlocking( pLine , NORMAL_BLOCKING );
				}
			}
		}
		else
		{
			for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
				iterLine != pDivision->vecLine.end();
				iterLine ++ )
			{
				AuLineMaptool * pLine = &( *iterLine );
				DrawBlocking( pLine , NORMAL_BLOCKING );
			}
		}
	}

	if( m_bDrawingNow )
	{
		DrawBlocking( &m_lineDrawing , SELECTING_NOW );
	}
	else
	{
		RwV3d pos = g_MainWindow.m_Position;
		if( m_bGridSnap )
		{
			ApWorldSector * pSector;
			pSector = g_pcsApmMap->GetSector( pos.x , pos.z );

			if( pSector )
			{
				FLOAT	applyX , applyZ;
				applyX = ( pSector->GetStepSizeX() / 2 ) * ( FLOAT ) ( ( INT32 ) ( (pos.x - pSector->GetXStart() ) / ( pSector->GetStepSizeX() / 2 ) ) );
				applyZ = ( pSector->GetStepSizeZ() / 2 ) * ( FLOAT ) ( ( INT32 ) ( (pos.z - pSector->GetZStart() ) / ( pSector->GetStepSizeZ() / 2 ) ) );
				
				pos.x = pSector->GetXStart() + applyX;
				pos.z = pSector->GetZStart() + applyZ;
			}
		}

		g_MainWindow.DrawPillar( pos.x , pos.z );
	}

	RwRenderStateSet(rwRENDERSTATECULLMODE  , (void *) rwCULLMODECULLNONE	);

	return TRUE;
}

void CPlugin_LineBlock::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect	rect;
	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	{
		rect.top = rect.bottom - ( INT32 )( m_fBlockingVisibleHeight / 100 );
		dc.FillSolidRect( rect , RGB( 0 , 100 , 0 ) );
		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB( 255 ,255 , 255 ) );

		CString	str;
		str.Format( "��ŷǥ�� ���� %.0f" , m_fBlockingVisibleHeight );
		dc.DrawText( str , rect , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	}

	COLORREF	rgbOn , rgbOff;
	rgbOn	= RGB( 46 , 103 , 227 );
	rgbOff	= RGB( 24 , 24 , 24 );

	dc.SetBkMode( TRANSPARENT );
	dc.SetTextColor( RGB( 255 ,255 , 255 ) );

	if( m_bBackfaceCull )
		dc.FillSolidRect( m_rectBackfaceCull , rgbOn );
	else
		dc.FillSolidRect( m_rectBackfaceCull , rgbOff );

	dc.DrawText( "Back Face Culling" , m_rectBackfaceCull , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	if( m_bGridSnap )
		dc.FillSolidRect( m_rectGridSnap , rgbOn );
	else
		dc.FillSolidRect( m_rectGridSnap , rgbOff );

	dc.DrawText( "Grid Snap" , m_rectGridSnap , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	if( m_bShowServer )
		dc.FillSolidRect( m_rectServerData , rgbOn );
	else
		dc.FillSolidRect( m_rectServerData , rgbOff );

	dc.DrawText( "Server Data" , m_rectServerData , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	if( m_bShowClient )
		dc.FillSolidRect( m_rectClientOnly , rgbOn );
	else
		dc.FillSolidRect( m_rectClientOnly , rgbOff );

	dc.DrawText( "Client Only" , m_rectClientOnly , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	if( m_bShowGeometryBlock )
		dc.FillSolidRect( m_rectGeomtryBlock , rgbOn );
	else
		dc.FillSolidRect( m_rectGeomtryBlock , rgbOff );

	dc.DrawText( "Show Geomtry Block" , m_rectGeomtryBlock , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	int	nLine = 250;
	#define DRAWTEXT( str ) { dc.TextOut( 20 , nLine , str ); nLine += 20; }
	DRAWTEXT( "ESC ��ư���� ����/����� ���" );
	DRAWTEXT( "��ŷ�� ���⼺�� Ȯ���ϼ���" );
	DRAWTEXT( "�������� ���Ŵ� ����" );
	DRAWTEXT( "���� �༮�� ���� ��ŷ" );
	DRAWTEXT( "Control������ �׸��� ������ŷ" );
	DRAWTEXT( "�� ȭ�� ��ư �� Ŭ���ϸ� ���̺���" );
}

void CPlugin_LineBlock::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_rectBackfaceCull.PtInRect( point ) )
	{
		m_bBackfaceCull = ! m_bBackfaceCull;
		Invalidate( FALSE );
	}
	else
	if( m_rectGridSnap.PtInRect( point ) )
	{
		m_bGridSnap = ! m_bGridSnap;
		Invalidate( FALSE );
	}
	else
	if( m_rectServerData.PtInRect( point ) )
	{
		m_bShowServer = ! m_bShowServer;
		Invalidate( FALSE );
	}
	else
	if( m_rectClientOnly.PtInRect( point ) )
	{
		m_bShowClient = ! m_bShowClient;
		Invalidate( FALSE );
	}
	else
	if( m_rectGeomtryBlock.PtInRect( point ) )
	{
		m_bShowGeometryBlock = ! m_bShowGeometryBlock;
		Invalidate( FALSE );
	}
	else
	{
		CRect	rect;
		GetClientRect( rect );
		INT32	nHeight = rect.Height() - point.y;
		m_fBlockingVisibleHeight	= ( FLOAT ) nHeight * 100;
		Invalidate( FALSE );
	}
}

BOOL CPlugin_LineBlock::OnExport( BOOL bServer )
{
	m_bExportServer = bServer;
	// ��������� ���͸� ���鼭 ���� ����..
	AGCMMAP_THIS->EnumLoadedDivision( CBExportCallback , this );
	return TRUE;
}


bool ValidPointInLineZ( CPlugin_LineBlock::AuLineMaptool * pLine , FLOAT fZ )
{
	if( pLine->start.z < pLine->end.z )
		return ( pLine->start.z <= fZ && fZ <= pLine->end.z ) ? true : false;
	else
		return ( pLine->end.z <= fZ && fZ <= pLine->start.z ) ? true : false;
}

bool ValidPointInLineX( CPlugin_LineBlock::AuLineMaptool * pLine , FLOAT fX )
{
	if( pLine->start.x < pLine->end.x )
		return ( pLine->start.x <= fX && fX <= pLine->end.x ) ? true : false;
	else
		return ( pLine->end.x <= fX && fX <= pLine->start.x ) ? true : false;
}
BOOL	CPlugin_LineBlock::CBExportCallback ( DivisionInfo * pDivisionInfo , PVOID pData )
{
	CPlugin_LineBlock * pThis = ( CPlugin_LineBlock * ) pData;

	Division * pDivision = pThis->GetDivision( pDivisionInfo->nIndex );

	if( NULL == pDivision ) return TRUE;

	ApWorldSector * pSector;
	int x , z;

	TRACE( "�귯ŷ ����......\n" );

	// ������ (2004-07-09 ���� 4:58:29) : �ִ� �ּ� ���� ����.
	for( z = pDivisionInfo->nZ	; z < pDivisionInfo->nZ + pDivisionInfo->nDepth ; ++ z )
	{
		for( x = pDivisionInfo->nX	; x < pDivisionInfo->nX + pDivisionInfo->nDepth ; ++ x )
		{
			pSector	= g_pcsApmMap->GetSectorByArray( x , 0 , z );

			if( NULL == pSector ) continue;

			// Line �� ��ġ�°� ������ �߶� ���� ����.
			pSector->m_vecBlockLine.clear();

			UINT32 uSequence = 0;
			for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
				iterLine != pDivision->vecLine.end();
				iterLine ++ , uSequence ++ )
			{
				// 
				AuLineMaptool lineCopy = *iterLine;

				if( pThis->m_bExportServer )
				{
					// ���� �ͽ���Ʈ
					if( lineCopy.eType != AuLineMaptool::SERVER ) continue;
				}
				else
				{
					// Ŭ���̾�Ʈ���� ������ �� ���� ����.
					// if( lineCopy.eType != AuLineMaptool::CLIENTONLY ) continue;
				}

				// �� ������ �� ���͸� ���������� ���� Ȯ��.
				// �������� �׳� ����..

				// 4���� ������ �ִµ�.. line to line �ݸ����� ��������
				// �ɸ��°��� �׳� ADD ��Ŵ.

				// pLine->start , pLine->end �� �մ� ������ �� ���͸� ����ϴ� ���..
				// ���Խ�Ŵ

				// start ����
				
				AuPOS	vDelta = lineCopy.end - lineCopy.start;

				BOOL	bColl = FALSE;

				if( pSector->IsInSector( lineCopy.start )	||
					pSector->IsInSector( lineCopy.end )	)
				{
					bColl = TRUE;
				}

				// ������.. ������ ������.
				//		+------------(4)------------+
				//		|							|
				//		|							|
				//	   (1)						   (3)
				//		|							|
				//		|							|
				//		+-----------(2)-------------+



				// (1)
				if( !bColl )
				{
					AuPOS vContactPoint;
					vContactPoint.z = vDelta.z / vDelta.x * ( pSector->GetXStart() - lineCopy.start.x ) + lineCopy.start.z;
					if( pSector->GetZStart()	<= vContactPoint.z	&&
						pSector->GetZEnd()		>= vContactPoint.z	&&
						ValidPointInLineX( &lineCopy , pSector->GetXStart() ) )
					{
						// ����..
						bColl = TRUE;
					}
				}
				// (2)
				if( !bColl )
				{
					AuPOS vContactPoint;
					vContactPoint.x = vDelta.x / vDelta.z * ( pSector->GetZEnd() - lineCopy.start.z ) + lineCopy.start.x;
					if( pSector->GetXStart()	<= vContactPoint.x	&&
						pSector->GetXEnd()		>= vContactPoint.x	&&
						ValidPointInLineZ( &lineCopy , pSector->GetZEnd() ) )
					{
						// ����..
						bColl = TRUE;
					}
				}
				// (3)
				if( !bColl )
				{
					AuPOS vContactPoint;
					vContactPoint.z = vDelta.z / vDelta.x * ( pSector->GetXEnd() - lineCopy.start.x ) + lineCopy.start.z;
					if( pSector->GetZStart()	<= vContactPoint.z	&&
						pSector->GetZEnd()		>= vContactPoint.z	&&
						ValidPointInLineX( &lineCopy , pSector->GetXEnd() ) )
					{
						// ����..
						bColl = TRUE;
					}
				}
				// (4)
				if( !bColl )
				{
					AuPOS vContactPoint;
					vContactPoint.x = vDelta.x / vDelta.z * ( pSector->GetZStart() - lineCopy.start.z ) + lineCopy.start.x;
					if( pSector->GetXStart()	<= vContactPoint.x	&&
						pSector->GetXEnd()		>= vContactPoint.x	&&
						ValidPointInLineZ( &lineCopy , pSector->GetZStart() ) )
					{
						// ����..
						bColl = TRUE;
					}
				}

				// Ȯ�� ������ ������
				if( bColl )
				{
					ApWorldSector::AuLineBlock stBlock;
					stBlock.FromAuLine( lineCopy );

					// �ø��� �ε����� 7�ڸ�..
					// xxxxyyy , xxxx�� ����� �ε��� , yyy�� ��������� �ø��� �ε���.
					// �ߺ��˻��.

					stBlock.uSerial = pDivision->nIndex * 1000 + uSequence;
					
					VERIFY( pSector->AddLineBlock( stBlock ) );
				}	
			}

			TRACE( "%02d " , pSector->GetLineBlockCount() );
		}
		TRACE( "\n" );
	}
		
	return TRUE;
}

void	CPlugin_LineBlock::ClearSelection()
{
	vector< Division * >::iterator iter;
	Division *					pDivisionFound = NULL;
	vector< AuLineMaptool >::iterator	pLineIterFound = NULL;

	for( iter = m_vecDivision.begin() ;
		iter != m_vecDivision.end() ;
		iter ++ )
	{
		Division * pDivision = ( Division * ) *iter;

		for( vector< AuLineMaptool >::iterator iterLine = pDivision->vecLine.begin();
			iterLine != pDivision->vecLine.end();
			iterLine ++ )
		{
			AuLineMaptool * pLine = &( *iterLine );
			pLine->bSelected = FALSE;
		}
	}
}