// Plugin_TextureInfo.cpp: implementation of the CPlugin_TextureInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "maptool.h"
#include "myengine.h"
#include "mainwindow.h"
#include "MainFrm.h"
#include "Plugin_TextureInfo.h"
#include "AuRwImage.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlugin_TextureInfo, CWnd)
	//{{AFX_MSG_MAP(CPlugin_TextureInfo)
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

extern MainWindow			g_MainWindow	;

CPlugin_TextureInfo::CPlugin_TextureInfo()
{
	m_pCurrentSector	= NULL	;
	m_nCount			= 0		;

	m_strShortName = "TexInfo";
}

CPlugin_TextureInfo::~CPlugin_TextureInfo()
{

}

BOOL CPlugin_TextureInfo::OnMouseMoveGeometry	( RwV3d * pPos )
{
	if( g_MainWindow.m_pCurrentGeometry != m_pCurrentSector )
	{
		m_pCurrentSector	= g_MainWindow.m_pCurrentGeometry;

		if( NULL == m_pCurrentSector ) return TRUE;
		// ���� ���� ������..

		AuList< IndexPattern >	list;
		AuNode< IndexPattern >	* pNode;
		IndexPattern			pattern;
		IndexPattern			* pPattern;

		int x , z;
		int	i;

		ApDetailSegment	*pSegment;
		for( z = 0 ; z < m_pCurrentSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; z ++ )
		{
			for( x = 0 ; x < m_pCurrentSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; x ++ )
			{
				pSegment	= m_pCurrentSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z );
				if( NULL != pSegment )
				{
					// ������;;;

					// ���ϰ˻�..
					

					// �̹� �ִ� �������� �˻�..
					pNode = list.GetHeadNode();
					while( pNode )
					{
						pPattern	= &pNode->GetData();

						for( i = 0 ; i < TD_DEPTH ; i ++ )
						{
							if( pPattern->nIndex[ i ] != pSegment->pIndex[ i ] )
							{
								break;
							}
						}

						if( i == TD_DEPTH )
						{
							// ����!..
							break;
						}

						list.GetNext( pNode );
					}

					// �������� �ִٸ�?...
					if( pNode )
					{
						// do no op..
					}
					else
					{
						// ������ �߰���..
						
						for( i = 0 ; i < TD_DEPTH ; i ++ )
						{
							pattern.nIndex[ i ] = pSegment->pIndex[ i ];
						}

						list.AddTail( pattern );
					}
					
				}
				

			}

		}

		// ��곡..
		m_nCount = list.GetCount();

		Invalidate( FALSE );

		if( m_pCurrentSector )
		{
			CGeometryPatternTexture	gpt;
			gpt.Create( AGCMMAP_THIS , m_pCurrentSector );
		}
	}
	
	return TRUE;
}

void CPlugin_TextureInfo::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect	rect;
	CString	str;

	GetClientRect( rect );
	dc.FillSolidRect( rect , RGB( 0 , 0 , 0 ) );

	dc.SetBkMode( TRANSPARENT );
	
	// TODO: Add your message handler code here
	dc.SetTextColor( RGB( 255 , 255 , 255 ) );
	dc.TextOut( 0 , 0 , "-_-" );

	if( m_pCurrentSector )
	{
		// ������..;;
		str.Format( "���� ���Ϳ� %d���� ������ ����" , m_nCount );
		dc.TextOut( 0 , 20 , str );
	}

//	str.Format( "%d ���� ������ �����" , m_nPolygonCount );
//	dc.SetTextColor( RGB( 255 , 192 , 192 ) );
//	dc.TextOut( 0 , 40 , str );
	
	// Do not call CWnd::OnPaint() for painting messages
}

BOOL	CPlugin_TextureInfo::OnSelectedPlugin		()
{
	return TRUE;
}
BOOL	CPlugin_TextureInfo::OnDeSelectedPlugin		()
{
	return TRUE;
}

void CPlugin_TextureInfo::OnTimer(UINT nIDEvent) 
{
	//RecalcTriangleInfo();

	//Invalidate( FALSE );

	CWnd::OnTimer(nIDEvent);
}


///////////////////////////////////////////////////////////////
// CGeometryPatternTexture
///////////////////////////////////////////////////////////////

BOOL	CGeometryPatternTexture::Create( AgcmMap * pCmMap , ApWorldSector * pSector )
{
	Release();

	if( NULL == pSector ) return TRUE;

	TRACE( "CGeometryPatternTexture %d,%d ���� ó����\n" , pSector->GetIndexX() , pSector->GetIndexZ() );
	
	AuList< IndexPattern >	list;
	AuNode< IndexPattern >	* pNode;
	IndexPattern			pattern;
	IndexPattern			* pPattern;

	int x , z;
	int	i;

	ApDetailSegment	*pSegment;
	for( z = 0 ; z < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; z ++ )
	{
		for( x = 0 ; x < pSector->D_GetDepth( SECTOR_HIGHDETAIL ) ; x ++ )
		{
			pSegment	= pSector->D_GetSegment( SECTOR_HIGHDETAIL , x , z );
			if( NULL != pSegment )
			{
				// ������;;;

				// ���ϰ˻�..
				

				// �̹� �ִ� �������� �˻�..
				pNode = list.GetHeadNode();
				while( pNode )
				{
					pPattern	= &pNode->GetData();

					for( i = 0 ; i < TD_DEPTH ; i ++ )
					{
						if( pPattern->nIndex[ i ] != pSegment->pIndex[ i ] )
						{
							break;
						}
					}

					if( i == TD_DEPTH )
					{
						// ����!..
						break;
					}

					list.GetNext( pNode );
				}

				// �������� �ִٸ�?...
				if( pNode )
				{
					// do no op..
				}
				else
				{
					// ������ �߰���..
					
					for( i = 0 ; i < TD_DEPTH ; i ++ )
					{
						pattern.nIndex[ i ] = pSegment->pIndex[ i ];
					}

					list.AddTail( pattern );
				}
				
			}
		}
	}
	return TRUE;

	/*
	// ��� ����..

	VERIFY( m_pArrayPattern	= new IndexPattern[ m_nCount = list.GetCount() ] );

	int	count = 0;
	pNode = list.GetHeadNode();
	while( pNode )
	{
		pPattern	= &pNode->GetData();
		m_pArrayPattern[ count ++ ]	= * pPattern;
		list.GetNext( pNode );

		TRACE("CGeometryPatternTexture:: %d,%d,%d,%d\n"			, 
			pPattern->nIndex[ 0 ] , pPattern->nIndex[ 1 ]	,
			pPattern->nIndex[ 2 ] , pPattern->nIndex[ 3 ]	);
	}
	
	// ũ�� ����..
	if( m_nCount <= 256	)	m_nType	=	TEXTURE_1024	;
	if( m_nCount <= 64	)	m_nType	=	TEXTURE_512		;
	if( m_nCount <= 16	)	m_nType	=	TEXTURE_256		;
	if( m_nCount <= 4	)	m_nType	=	TEXTURE_128		;
	if( m_nCount <= 1	)	m_nType	=	TEXTURE_64		;
	if( m_nCount == 0	)	m_nType	=	TEXTURE_NONE	;
	
	ASSERT( m_nCount <= 256 );

	// �̹��� ����...
	RwRaster * pRaster_Image	= RwRasterCreate( TEXTURE_UNIT_SIZE * m_nType , TEXTURE_UNIT_SIZE * m_nType , 32 , rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888 );
	RwRaster * pRasterTile		= RwRasterCreate( TEXTURE_UNIT_SIZE , TEXTURE_UNIT_SIZE , 32 , rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888 );

	if( NULL == pRaster_Image || NULL == pRasterTile )
	{
		TRACE( "%d Ÿ��!\n" , m_nType );
		return FALSE;
	}

	AuRwImage	Layer1	;
	AuRwImage	Layer2	;
	AuRwImage	Layer3	;
	AuRwImage	Alpha	;
	RwRect		rect	;

	RwRasterPushContext( pRaster_Image );

	RwRaster	* pSubRaster;

	for( i = 0 ; i < m_nCount ; ++ i )
	{
		//RwRasterSubRaster( pSubRaster_RenderTarget , pRaster_Image , &rect );
		TRACE("CGeometryPatternTexture:: ����(%d/%d)!\n" , i , m_nCount );
	
		// �� ���Ϻ���..
		Layer1.Set( pCmMap->m_TextureList.LoadImage( m_pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );

		rect.x	= ( INT32 ) ( Layer1.GetWidth()		* GET_TEXTURE_U_START	( m_pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );
		rect.y	= ( INT32 ) ( Layer1.GetHeight()	* GET_TEXTURE_V_START	( m_pArrayPattern[ i ].nIndex[ TD_FIRST ] ) );
		rect.w	= ( INT32 ) ( Layer1.GetWidth()		* GET_TEXTURE_U_END		( m_pArrayPattern[ i ].nIndex[ TD_FIRST ] ) ) - rect.x	;
		rect.h	= ( INT32 ) ( Layer1.GetHeight()	* GET_TEXTURE_V_END		( m_pArrayPattern[ i ].nIndex[ TD_FIRST ] ) ) - rect.y	;

		if( NULL == Layer1.CheckOutRaster() ) continue;

		RwRasterPushContext( pRasterTile );

		pSubRaster	= Layer1.GetSubRaster( rect.x , rect.y , rect.w , rect.h );

		VERIFY( RwRasterRender( pSubRaster , 0 , 0 ) );

		// ���� �̹��� ���

		if( m_pArrayPattern[ i ].nIndex[ TD_SECOND	] != ALEF_TEXTURE_NO_TEXTURE	&&
			m_pArrayPattern[ i ].nIndex[ TD_THIRD	] != ALEF_TEXTURE_NO_TEXTURE	)
		{
			AuRwImage	subImageAlpha , subImageLayer2;

			// �����̹��� ����..
			BOOL	bRet ;
			bRet = Alpha.Set( pCmMap->m_TextureList.LoadImage( m_pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );

			rect.x	= ( INT32 ) ( Alpha.GetWidth()	* GET_TEXTURE_U_START	( m_pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );
			rect.y	= ( INT32 ) ( Alpha.GetHeight()	* GET_TEXTURE_V_START	( m_pArrayPattern[ i ].nIndex[ TD_SECOND ] ) );
			rect.w	= ( INT32 ) ( Alpha.GetWidth()	* GET_TEXTURE_U_END		( m_pArrayPattern[ i ].nIndex[ TD_SECOND ] ) ) - rect.x	;
			rect.h	= ( INT32 ) ( Alpha.GetHeight()	* GET_TEXTURE_V_END		( m_pArrayPattern[ i ].nIndex[ TD_SECOND ] ) ) - rect.y	;

			Alpha.CreateSubImage( & subImageAlpha , rect.x , rect.y , rect.w , rect.h );

			bRet = Layer2.Set( pCmMap->m_TextureList.LoadImage( m_pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );

			rect.x	= ( INT32 ) ( Layer2.GetWidth()		* GET_TEXTURE_U_START	( m_pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );
			rect.y	= ( INT32 ) ( Layer2.GetHeight()	* GET_TEXTURE_V_START	( m_pArrayPattern[ i ].nIndex[ TD_THIRD ] ) );
			rect.w	= ( INT32 ) ( Layer2.GetWidth()		* GET_TEXTURE_U_END		( m_pArrayPattern[ i ].nIndex[ TD_THIRD ] ) ) - rect.x	;
			rect.h	= ( INT32 ) ( Layer2.GetHeight()	* GET_TEXTURE_V_END		( m_pArrayPattern[ i ].nIndex[ TD_THIRD ] ) ) - rect.y	;

			Layer2.CreateSubImage( &subImageLayer2 , rect.x , rect.y , rect.w , rect.h );
			
			subImageAlpha.MakeMask();
			subImageLayer2.ApplyMask( subImageAlpha.GetImagePointer() );

			subImageLayer2.CheckOutRaster();
			RwRasterRender( subImageLayer2.GetRaster() , 0 , 0 );
			subImageLayer2.CheckInRaster();

		}

		if( m_pArrayPattern[ i ].nIndex[ TD_FOURTH	] != ALEF_TEXTURE_NO_TEXTURE	)
		{
			Layer3.Set( pCmMap->m_TextureList.LoadImage( m_pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );

			rect.x	= ( INT32 ) ( Layer3.GetWidth()		* GET_TEXTURE_U_START	( m_pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );
			rect.y	= ( INT32 ) ( Layer3.GetHeight()	* GET_TEXTURE_V_START	( m_pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) );
			rect.w	= ( INT32 ) ( Layer3.GetWidth()		* GET_TEXTURE_U_END		( m_pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) ) - rect.x	;
			rect.h	= ( INT32 ) ( Layer3.GetHeight()	* GET_TEXTURE_V_END		( m_pArrayPattern[ i ].nIndex[ TD_FOURTH ] ) ) - rect.y	;

			Layer3.CheckOutRaster();
			RwRasterRender( Layer3.GetRaster() , 0 , 0 );
			Layer3.CheckInRaster();

		}

		RwRasterPopContext();
		RwRasterRender( pRasterTile , GetLeft( i ) , GetTop( i ) );

		RwRasterDestroy( pSubRaster );
		Layer1.CheckInRaster();
	}

	RwRasterPopContext();

	// �̹����� ī��..
	m_pImage	= RwImageCreate(
		RwRasterGetWidth	( pRaster_Image )	,
		RwRasterGetHeight	( pRaster_Image )	,
		RwRasterGetDepth	( pRaster_Image )	);
	RwImageAllocatePixels( m_pImage );

	// ���̹��� ����..
	ASSERT( NULL != m_pImage );
	if( m_pImage )
	{
		RwImageSetFromRaster( m_pImage , pRaster_Image );

		{
			static	int	count = 0;
			char	strFilename[ 256 ];
			wsprintf( strFilename , "%02d.png" , count ++ );
			VERIFY( RwImageWrite( m_pImage , strFilename ) );

			RwError	error;
			RwErrorGet( &error );
		}
	}

	if( pRaster_Image	) RwRasterDestroy( pRaster_Image	);
	if( pRasterTile		) RwRasterDestroy( pRasterTile		);


	return TRUE;*/
}

void	CGeometryPatternTexture::Release()
{
	m_nType			= TEXTURE_NONE;
	if( m_pImage ) RwImageDestroy( m_pImage );
	m_pImage		= NULL	;
	if( m_pArrayPattern ) delete [] m_pArrayPattern;
	m_pArrayPattern	= NULL	;
	m_nCount		= 0		;
}

INT32	CGeometryPatternTexture::GetLeft	( INT32 nIndex )
{
	ASSERT( TEXTURE_NONE != m_nType );

	return ( nIndex % m_nType ) * TEXTURE_UNIT_SIZE;
}

INT32	CGeometryPatternTexture::GetTop		( INT32 nIndex )
{
	ASSERT( TEXTURE_NONE != m_nType );

	return ( nIndex / m_nType ) * TEXTURE_UNIT_SIZE;
}
