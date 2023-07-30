// AlefTextureList.cpp: implementation of the AcTextureList class.
//
//////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <ApmMap/ApmMap.h>
#include <AgcmMap/AgcmMap.h>
#include <AgcmMap/AcTextureList.h>
#include <rwcore.h>
#include <rpworld.h>
#include <rtpitexd.h>
#include <ApBase/ApUtil.h>
#include <AgcmMap/bmp.h>
#include <ApBase/MagDebug.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcTextureList::AcTextureList()
{
	m_strTextureDirectory = NULL;

	m_strTextureDictionary[0] = 0;
	m_pTexDict = NULL;

	m_bEdited		= FALSE;
}

AcTextureList::~AcTextureList()
{

}

int		AcTextureList::LoadScript			( char *filename	)	// ��ũ��Ʈ�� �ε��Ѵ�.
	// ���� �� �������� �ؽ��ĸ� �ε��ϰ� 
	// Material�� �����Ͽ� ������ �ִ´�.
{
	// File Format
	// Category|Index|Type|Comment|FileName

	// 0x00XXYY00 XXŸ�� , YY�ε���
	// �� �޸𸮿� �ִ´�.
	// ���� �༮�̶�� 

	// �ؽ��Ĵ� �ٷ� �ε����� �ʰ� , �ִ����� ������ �ִ�.
	// Material Index�� ������ �־���Ѵ�.

	FILE	*pFile = fopen( filename , "rt" );
	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return FALSE;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];
	unsigned int	nRead = 0;
	char			drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

	TextureInfo		ti;

	int count = 0;
	int	dimension , category , index , type;
	while( fgets( strbuffer , 1024 , pFile ) )
	{
		// �о�����..

		arg.SetParam( strbuffer , "|\n" );

		if( arg.GetArgCount() < 5 )
		{
			// ���� �̻�
			continue;
		}

		category	= atoi( arg.GetParam( 0 ) );
		index		= atoi( arg.GetParam( 1 ) );
		type		= atoi( arg.GetParam( 2 ) );

		strncpy		( ti.strComment		, arg.GetParam( 3 ) , 256 );

		// �����̸� ����..
		strncpy		( ti.strFilename	, arg.GetParam( 4 ) , 256 );
		_splitpath	( ti.strFilename	, drive, dir, fname, ext );
		wsprintf	( ti.strFilename	, "Map\\Tile\\%s%s" , fname , ext );

		if( arg.GetArgCount() >= 6 )
			dimension	= atoi( arg.GetParam( 5 ) )	;
		else
		{
			switch( type )
			{
			case	TT_FLOORTILE	:	dimension	=	TT_FLOOR_DEPTH	;	break;
			case	TT_UPPERTILE	:	dimension	=	TT_UPPER_DEPTH	;	break;
			case	TT_ONE			:	dimension	=	TT_ONE_DEPTH	;	break;
			case	TT_ALPHATILE	:	dimension	=	TT_ALPHA_DEPTH	;	break;
			default:
				ASSERT( !"-__-!" );
				dimension	= TEXTURE_DIMENTION_DEFAULT	;					
				break;
			}
		}

		// ������ (2004-08-24 ���� 3:50:12) : Ÿ�� �ٴ� �Ӽ� �߰�..
		if( arg.GetArgCount() >= 7 )
			ti.nProperty	= atoi( arg.GetParam( 6 ) )	;
		else
		{
			ti.nProperty	= APMMAP_MATERIAL_SOIL;
		}

		ti.nIndex	= MAKE_TEXTURE_INDEX( dimension , category , index , type , 0x00 );

		TextureList.AddTail( ti );
		++count;
	}

	fclose( pFile );

	// InitialTempFiles();
	// �ؽ��� ����Ҷ� �ϰ� �ؾ���..

	//DisplayMessage( "�ؽ��� ����Ʈ ������ �ε��� �Ͽ����ϴ�." );

	ClearEdited();
	return count;
}

RwTexture *	AcTextureList::GetTexture			( int textureindex	)	// �ý��������͸� ���� �Լ�.
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );
	
	if( pTi )
	{
		// Ref Count �� ������!..
		return LoadTexture( pTi );
	}

	// ������ ���´�� -_-;;
	return NULL;
}

void		AcTextureList::DeleteAll			( void				)
{
	// ��� ��带 ����.

	AuNode< TextureInfo > * pNode;
	pNode = TextureList.GetHeadNode();

	while( pNode )
	{
		Delete( pNode );
		pNode = TextureList.GetHeadNode();
	}
}

void		AcTextureList::Delete			( AuNode< TextureInfo > * pNode , BOOL bDeleteGraphicFile)
{
	// ��� �ϳ��� ����.

	TextureInfo *	pTi = & pNode->GetData();

	if( bDeleteGraphicFile )
	{
		// ���� ����...
		char	fname[ 256 ];
		wsprintf( fname , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );
		DeleteFile( fname );
	}
	
	TextureList.RemoveNode( pNode );

	SetEdited();
}

RwTexture *		AcTextureList::LoadTexture			( TextureInfo * textureinfo , BOOL bForceUpdate )
{
	// �ؽ��� ������ ���� �ؽ��ĸ� �ε��Ѵ�.
    RwTexture *				texture		= NULL;
	char					drive[ 256 ] , dir [ 256 ] , f_name [ 256 ] , ext[ 256 ];

	char	strExportName[ 256 ];
	_splitpath( textureinfo->strFilename	, drive,	dir,	f_name,	ext		);

	//wsprintf( strExportName , ALEF_TILE_TEXTURENAME_NAME , textureinfo->nIndex );
	GetExportFileName( strExportName , textureinfo->nIndex );

	char	*pAlphaName = NULL;
	switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
	{
	case TT_ALPHATILE:
		{
			pAlphaName = NULL;
			// ������ (2004-04-22 ���� 6:59:17) : �Ӹʲ���.
			RwTextureSetMipmapping		( FALSE );
			RwTextureSetAutoMipmapping	( FALSE );
		}
		break;
	case TT_UPPERTILE:
		{
			int		textlen = ( int ) strlen( ext );
			for( int i = 0 ; i < textlen ; ++i )
				ext[ i ] = toupper( ext[ i ] );

			if( !strncmp( ext , ".PNG" , 4 ) )
				pAlphaName = NULL;
			else
				pAlphaName = strExportName;
		}
		break;
	default:
		pAlphaName = NULL;
		break;
	}

	AGCMMAP_THIS->m_pcsAgcmResourceLoader->EnableEncryption( FALSE );

	// ������ (2004-05-30 ���� 7:09:25) : ��ųʸ� ����..
	AGCMMAP_THIS->m_pcsAgcmResourceLoader->SetDefaultTexDict();
	if( bForceUpdate )
	{
		// do nothing..
		texture	= NULL;
	}
	else
	{
		// ������ (2005-04-22 ���� 2:33:06) : 
		// �ͽ���Ʈ�� Ÿ���� ���� ���� �ʴ´�.
		texture = RwTextureRead( strExportName , pAlphaName );
		//texture = NULL;
	}

	if( texture )
	{
		// ����
		RwTextureSetFilterMode( texture, rwFILTERLINEARMIPLINEAR );
	}
	else
	{
		// ����.
		// �������� ����.

		ASSERT( NULL != m_strTextureDirectory );
		char	fname1[ 256 ];
		wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , textureinfo->strFilename );

		switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
		{
		case TT_ALPHATILE:
			{
				pAlphaName = NULL;
				// ������ (2004-04-22 ���� 6:59:17) : �Ӹʲ���.
				RwTextureSetMipmapping		( FALSE );
				RwTextureSetAutoMipmapping	( FALSE );
			}
			break;
		case TT_UPPERTILE:
			{
				int textlen = ( int ) strlen( ext );
				for( int i = 0 ; i < textlen ; ++i )
					ext[ i ] = toupper( ext[ i ] );

				if( !strncmp( ext , ".PNG" , 4 ) )
					pAlphaName = NULL;
				else
					pAlphaName = fname1;
			}
		default:
			pAlphaName = NULL;
			break;
		}

		texture = RwTextureRead( RWSTRING ( fname1 ) , RWSTRING ( pAlphaName ) );
	}
	
	switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
	{
	case TT_ALPHATILE:
		{
			RwTextureSetMipmapping		( TRUE );
			RwTextureSetAutoMipmapping	( TRUE );
		}
		break;
	default:
		// do nothing
		break;
	}

	if( texture )
	{
		// ����
		switch( GET_TEXTURE_TYPE( textureinfo->nIndex ) )
		{
		case TT_ALPHATILE:
			{
				RwTextureSetFilterMode	( texture, rwFILTERNEAREST 	);
			}
			break;
		default:
			RwTextureSetFilterMode	( texture, rwFILTERLINEARMIPLINEAR	);
			break;
		}

		if( bForceUpdate )
		{
			RwTexture * pExist;
			if( pAlphaName )
			{
				pExist = RwTextureRead( strExportName , strExportName );
			}
			else
			{
				pExist = RwTextureRead( strExportName , NULL );
			}

			if( pExist )
			{
				// �̹� �����Ѵٸ�... ������ �ٲ�ġ�� ����..
				RwRaster * pRaster = pExist->raster	;
				pExist->raster	= texture->raster;
				texture->raster	= pRaster;

				// �ε��� �ؽ��� ��Ʈ����.
				RwTextureDestroy( texture );
				texture = NULL;
				texture = pExist;

				// ���۷��� ī��Ʈ ����.
				pExist->refCount--;
			}
		}
		else
		{
			RwTextureSetName		( texture , strExportName			);

			if( pAlphaName )
				RwTextureSetMaskName( texture , strExportName			);
		}
	}
	else
	{
		TRACE( "�ؽ��� �б� ���� (%s)" , textureinfo->strFilename );
	}

	AGCMMAP_THIS->m_pcsAgcmResourceLoader->EnableEncryption( TRUE );

	return texture;
}

AcTextureList::TextureInfo * AcTextureList::GetTextureInfo	( int textureindex	)	// �ؽ��� �˻�.... �˻� �¸�.
{
	// ������ �������� �ִ������� �˻���. 
	// textureindex = GET_TEXTURE_OFFSET_MASKING_OUT( textureindex );
	int	category , index;

	category	= GET_TEXTURE_CATEGORY	( textureindex );
	index		= GET_TEXTURE_INDEX		( textureindex );

	AcTextureList::TextureInfo 				*pTi;
	AuNode< TextureInfo >	*pNode = TextureList.GetHeadNode();
	while( pNode )
	{
		pTi = & pNode->GetData();

		if( category	== GET_TEXTURE_CATEGORY	( pTi->nIndex )	&&
			index		== GET_TEXTURE_INDEX	( pTi->nIndex )	)
		{
			return pTi;
		}
		pNode = pNode->GetNextNode();
	}

	return NULL;
}

BOOL		AcTextureList::DeleteTexture		( int textureindex , BOOL bDeleteGraphicFile	)	// �ؽ����� ������ ����Ʈ���� �����Ѵ�.
{
	// ���ϱ��� ����..
	TextureInfo * textureInfo = GetTextureInfo( textureindex );

	AuNode< TextureInfo > * pNode;
	pNode = TextureList.GetHeadNode();

	while( pNode )
	{
		if( &pNode->GetData() == textureInfo )
		{
			Delete( pNode , bDeleteGraphicFile );
			SetEdited();
			return TRUE;
		}
		TextureList.GetNext( pNode );
	}	

	// ��ã��!
	return FALSE;
}

AcTextureList::TextureInfo * AcTextureList::AddTexture		( int dimension , int category , int index , int type , char *filename , char *comment )
{
	TextureInfo		ti;

	if( dimension == TEXTURE_DIMENTION_DEFAULT )
	{
		switch( type )
		{
		case	TT_FLOORTILE	:	dimension	=	TT_FLOOR_DEPTH	;	break;
		case	TT_UPPERTILE	:	dimension	=	TT_UPPER_DEPTH	;	break;
		case	TT_ONE			:	dimension	=	TT_ONE_DEPTH	;	break;
		case	TT_ALPHATILE	:	dimension	=	TT_ALPHA_DEPTH	;	break;
		default:
			ASSERT( !"-__-!" );
			dimension	= TEXTURE_DIMENTION_DEFAULT	;					
			break;
		}
	}

	ti.nIndex	= MAKE_TEXTURE_INDEX( dimension , category , index , type , 0x00 );
	strncpy( ti.strFilename, filename , 256 );
	if( comment == NULL )
		strncpy( ti.strComment , filename , 256 );	// �����̸����� ä�� ������.
	else
		strncpy( ti.strComment , comment , 256 );

	// �������� ��Ʈ�� ����.
	// ������ �ִ����� �˻���..
	if( MakeTempBitmap( &ti ) )
	{
		TextureList.AddTail( ti );
		TextureInfo * pTi = &TextureList.GetTailNode()->GetData();
		SetEdited();
		return pTi;
	}
	else
	{
		return NULL;
	}
}

int			AcTextureList::SaveScript			( char *filename	)
{
	// File Format
	// Type|Index|Comment|FileName

	// 0x00XXYY00 XXŸ�� , YY�ε���
	// �� �޸𸮿� �ִ´�.
	// ���� �༮�̶�� 

	// �ؽ��Ĵ� �ٷ� �ε����� �ʰ� , �ִ����� ������ �ִ�.
	// Material Index�� ������ �־���Ѵ�.

	FILE	*pFile = fopen( filename , "wt" );
	if( pFile == NULL )
	{
		// ���Ͽ��� ����.
		return FALSE;
	}

	CGetArg2		arg;
	char			strbuffer[	1024	];

	TextureInfo		*pTi , *pTiLowest;

	int count = 0;
	int dimension , category , index , type ;

	AuNode< TextureInfo > * pNode		;
	AuNode< TextureInfo > * pNodeTmp	;
	
	////////////////////////////////////////////////////////////////////
	// ��Ʈ�Ѵ�..
	////////////////////////////////////////////////////////////////////
	AuList< TextureInfo > listTmp	= TextureList;
	AuList< TextureInfo > listSorted;

	while( listTmp.GetHeadNode() )
	{
		pNode		= listTmp.GetHeadNode()	;
		pNodeTmp	= pNode					;
		pTiLowest	= &pNode->GetData()		;

		while( pNode )
		{
			// ���� ���� ���� ã�Ƽ� �μ�Ʈ/������
			pTi		= &pNode->GetData();

			if( ( pTi->nIndex & 0x00ffffff ) < ( pTiLowest->nIndex & 0x00ffffff ) )
			{
				// ���� ������ �߰�.;;
				pNodeTmp	= pNode	;
				pTiLowest	= pTi	;
			}

			pNode = pNode->GetNextNode();
		}

		listSorted.AddTail( *pTiLowest	);
		listTmp.RemoveNode( pNodeTmp	);
	}
	////////////////////////////////////////////////////////////////////
	
	pNode = listSorted.GetHeadNode();
	while( pNode )
	{
		pTi = &pNode->GetData();

		dimension	= GET_TEXTURE_DIMENSION	( pTi->nIndex	);
		category	= GET_TEXTURE_CATEGORY	( pTi->nIndex	);
		index		= GET_TEXTURE_INDEX		( pTi->nIndex	);
		type		= GET_TEXTURE_TYPE		( pTi->nIndex	);

		// Type|Index|Comment|FileName

		wsprintf( strbuffer , "%d|%d|%d|%s|%s|%d|%d\n" ,
			category , index , type ,  pTi->strComment , pTi->strFilename , dimension , pTi->nProperty );

		fputs( strbuffer , pFile );

		count ++;
		listSorted.GetNext( pNode );
	}

	fclose( pFile );

	//DisplayMessage( "�ؽ��� ����Ʈ ������ �����Ͽ����ϴ�." );
	ClearEdited();
	return count;
}

BOOL		AcTextureList::MakeTempBitmap		( TextureInfo * pTi				)	// �ӽ� ��Ʈ�� ������.
{
	RwImage	* pImage;

	char	str[1024];
	ASSERT( NULL != m_strTextureDirectory );
	wsprintf( str , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );
	pImage = RwImageRead ( str );

	if( pImage == NULL )
	{
		// ���丮 ���濡 ����.. ȣ���ڵ�
		// ������ (2002-08-30 ���� 6:43:36) : 
		wsprintf( str , "%s\\Map\\%s" , m_strTextureDirectory , pTi->strFilename );
		pImage = RwImageRead ( str );
	}

	// ��¿�� ����.. ���� �̹��� ���
	if( NULL == pImage )
	{
		TRACE( "������ ��� ���Ƿ� ����(%s)\n" , pTi->strFilename );
		CBmp	bmp;
		HBRUSH	hBrush;

		bmp.Create( 50 , 50 );
		VERIFY( hBrush = CreateSolidBrush( RGB( 255 , 0 , 0 ) ) );

		RECT	rect;
		SetRect( &rect , 0 , 0 , 50 , 50 );
		::FillRect( bmp.GetDC() , &rect , hBrush );
		::SetTextColor( bmp.GetDC() , RGB( 0 , 0 , 0 ) );
		::SetBkMode( bmp.GetDC() , TRANSPARENT );
		::TextOut( bmp.GetDC() , 0 , 0 , "ERROR" , 5 );

		char 	strFilename[ 1024 ];
		wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

		VERIFY( bmp.Save( strFilename ) );

		DeleteObject( hBrush );
		return TRUE;
	}

	if( pImage == NULL )
	{
		// -_-;;;
		// ������ ��¼��� - -;;
//		CString	str;
//		str.Format( "�ؽ��� ����Ʈ�� ��ϵ� �̹���( \"%s\" )�� �������� �ʽ��ϴ�." , pTi->strComment );
//		DisplayMessage( AEM_ERROR , str );
//		ASSERT( !"Bitmap ���� ����!\n" );
		return FALSE;
	}
	else
	{
		switch( GET_TEXTURE_TYPE( pTi->nIndex ) )
		{
		case	TT_FLOORTILE	:
			{
				// �������� �Ѱ��� ����.
				// �ε�����.
				char strFilenameOrigin[ 1024 ];
				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 100 , 100 );

				ASSERT( NULL != pReSample );

				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				
				
				// ��ǥ Ÿ�� �Ѱ��� ����.
				// �ε�����.

				CBmp	bmp,bmpPicked;
				bmp.Load( strFilenameOrigin , NULL );

				bmpPicked.Create( 50 , 50 );
				bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 );

				char 	strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				VERIFY( bmpPicked.Save( strFilename ) );

				DeleteFile		( strFilenameOrigin );

				return TRUE;
			}
			break;
		case	TT_ALPHATILE	:
			{
				// �������� �Ѱ��� ����.
				// �ε�����.
				char strFilenameOrigin[ 1024 ];
				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 200 , 200 );

				ASSERT( NULL != pReSample );
				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				
				
				// ��ǥ Ÿ�� �Ѱ��� ����.
				// �ε�����.

				CBmp	bmp,bmpPicked;
				bmp.Load( strFilenameOrigin , NULL );

				bmpPicked.Create( 50 , 50 );
				bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 );

				char strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				VERIFY( bmpPicked.Save( (LPSTR)(LPCTSTR) strFilename ) );

				DeleteFile		( strFilenameOrigin );

				return TRUE;
			}
			break;
		case	TT_UPPERTILE	:
			{
//				// �������� �Ѱ��� ����.
//				// �ε�����.
//				char strFilenameOrigin[ 1024 ];
//				wsprintf( strFilenameOrigin , "%s\\Map\\Temp\\Origin_%x.bmp" , m_strTextureDirectory , pTi->nIndex );
//
//				RwImage	* pReSample;
//
//				pReSample = RwImageCreateResample( pImage , 200 , 200 );
//
//				ASSERT( NULL != pReSample );
//
//				VERIFY( RwImageWrite( pReSample , strFilenameOrigin ) );
//
//				RwImageDestroy( pImage		);
//				RwImageDestroy( pReSample	);
//				
//				
//				// ��ǥ Ÿ�� �Ѱ��� ����.
//				// �ε�����.
//
//				CBmp	bmp,bmpPicked;
//				bmp.Load( strFilenameOrigin );
//
//				bmpPicked.Create( 50 , 50 );
//
//				char strFilename[ 1024 ];
//
//				for( int y = 0 ; y < 4 ; ++y )
//				{
//					for( int x = 0 ; x < 4 ; ++x )
//					{
//						wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );
//						wsprintf( strFilename , "%s\\Map\\Temp\\%x.bmp" , m_strTextureDirectory , pTi->nIndex + y * 4 + x );
//
//						bmp.Draw( bmpPicked.GetDC() , 0 , 0 , 50 , 50 , x * 50 , y * 50 );
//							
//						VERIFY( bmpPicked.Save( strFilename ) );
//					}
//				}
//
//				DeleteFile		( strFilenameOrigin );
//
				return TRUE;
			}
			break;
		case	TT_ONE			:
			{
				// �������� �Ѱ��� ����.
				// �ε�����.
				char strFilename[ 1024 ];
				wsprintf( strFilename , "%s\\Map\\Temp\\%d,%d.bmp" , m_strTextureDirectory , GET_TEXTURE_CATEGORY( pTi->nIndex ) , GET_TEXTURE_INDEX( pTi->nIndex ) );

				RwImage	* pReSample;
				pReSample = RwImageCreateResample( pImage , 50 , 50 );

				ASSERT( NULL != pReSample );

				VERIFY( RwImageWrite( pReSample , (LPCTSTR) strFilename ) );

				RwImageDestroy( pImage		);
				RwImageDestroy( pReSample	);
				return TRUE;
			}
			break;
		default					:
			{
				// �Ѱ� ����.

			}
			break;
		}

//		// ������ �̹��� �����κ�.
//
//		// �ε�����.
//		CString	filename;
//		filename.Format( "%s\\Map\\Temp\\%d.bmp" , m_strTextureDirectory , pTi->nIndex );
//
//		RwImage	* pReSample;
//		pReSample = RwImageCreateResample( pImage , 50 , 50 );
//
//		if( RwImageWrite( pReSample , (LPCTSTR) filename ) )
//		{
//			// �ƽ�
//		}
//		else
//		{
//			CString	str;
//			str.Format( "�ؽ��� ����Ʈ�� ��ϵ� �̹���( \"%s\" ) �ӽ� ���� ���� ����" , pTi->strComment );
//			DisplayMessage( AEM_ERROR , str );
//		}
//
//		RwImageDestroy( pImage		);
//		RwImageDestroy( pReSample	);

		return FALSE;
	}
}

BOOL		AcTextureList::InitialTempFiles	( void				)	// Ÿ�� ���� �����쿡�� ����� ��Ʈ�� ���� ����.
{
	// ����Ʈ�� �ִ� �̹��� ���ϵ��� ��� bmp���Ϸ� ������.
	// 
	AcTextureList::TextureInfo 			*	pTi		;
	int										count	= 0;
	AuNode< TextureInfo >	*pNode = TextureList.GetHeadNode();
	while( pNode )
	{
		pTi = & pNode->GetData();
		if( MakeTempBitmap( pTi ) )
		{
			// ����
		}
		else
		{
			// ����.
		}
		pNode = pNode->GetNextNode();
	}

	return TRUE;
}


BOOL	AcTextureList::ExportTile	( char *destination , void ( *callback ) ( void * pData ) )
{
	// ��ϵ� Ÿ���� �� �ͽ���Ʈ�Ѵ�..
	ASSERT( NULL != m_strTextureDirectory );

	AuNode< TextureInfo >	* pNode = TextureList.GetHeadNode();
	TextureInfo	*	pTi			;

	//RwImage	*		pReSample	;

	char		str[1024]			;
	char		dstfullname[ 1024 ]	;
	char		dstShortFilename[ 1024 ];
	char		drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];

//	CProgressDlg	dlg;
//	dlg.StartProgress( "���� �ؽ��� �ͽ���Ʈ��..." , TextureList.GetCount() , g_pMainFrame );
	int	count = 0;

	while( pNode )
	{
		pTi = & pNode->GetData();

		// Texture Dictionary ������ ���ؼ� �� �ε��Ѵ�.
		if( m_strTextureDictionary[0] )
			LoadTexture(pTi);

		// �Ӹʵ���Ÿ ����...

		// �켱 �̹��� �ε�.
		wsprintf( str , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

		// 3�ܰ�� �Ӹ� ������ ������.
		// full 1/2 1/4 �� ����.

		// �����̸� ����
		// ����Ƽ���̼� ���丮�� ���� ����..

		_splitpath( pTi->strFilename , drive, dir, fname, ext );

		// Full Size..

		//wsprintf( dstShortFilename , ALEF_TILE_EXPORT_FILE_NAME , pTi->nIndex , ext );
		GetExportFileName( dstShortFilename , pTi->nIndex , ext );
		wsprintf( dstfullname , "%s\\Texture\\World\\%s" ,	m_strTextureDirectory , dstShortFilename);


		if( CopyFile( str , dstfullname , FALSE ) )
		{
			// ����

		}
		else
		{
			TRACE( "����!\n" );
		}


		TextureList.GetNext( pNode );

		++count;
//		dlg.SetProgress( count );
	}

	// Texture Dictionary�� ������, StreamWrite�Ѵ�. Platform Independent Data��
	if( m_pTexDict && m_strTextureDictionary[0] )
	{
		RwStream *		stream;

		stream = RwStreamOpen( rwSTREAMFILENAME, rwSTREAMWRITE, m_strTextureDictionary );
		if( stream )
		{
			RtPITexDictionaryStreamWrite( m_pTexDict , stream );
   
			RwStreamClose(stream, NULL);
		}
	}

//	dlg.EndProgress();
	
	return TRUE;
}

BOOL		AcTextureList::GetExportFileName	( char *filename , int nIndex , char * ext )
{
	int	nIndexExport;

	nIndexExport	=	GET_TEXTURE_DIMENTION_MASKING_OUT( GET_TEXTURE_OFFSET_MASKING_OUT( nIndex ) );

	wsprintf( filename , ALEF_TILE_EXPORT_FILE_NAME , nIndexExport , ext );
	return TRUE;
}

RwRaster *	AcTextureList::LoadRaster			( int			textureindex	)
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );

	ASSERT( NULL != m_strTextureDirectory );
	char	fname1[ 256 ];
	wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

	return RwRasterRead( fname1 );
}

RwImage *	AcTextureList::LoadImage			( int			textureindex	)
{
	TextureInfo 		* pTi;

	pTi = GetTextureInfo( textureindex );

	if( pTi->pImage )
	{
		// �̹� �ε�ž�����..
		return pTi->pImage;
	}
	else
	{
		ASSERT( NULL != m_strTextureDirectory );
		char	fname1[ 256 ];
		wsprintf( fname1 , "%s\\%s" , m_strTextureDirectory , pTi->strFilename );

		return pTi->pImage = RwImageRead( fname1 );
	}
}

#define TOLOWERSTRING( str )					\
{												\
	int tmp;									\
	tmp = strlen( str ) - 1;					\
	while( tmp > 0 )							\
	{											\
		str[ tmp ] = tolower( str[ tmp ] );		\
		tmp--;									\
	}											\
}

int		AcTextureList::CleanUpUnusedTile	()
{
	char	strPath[ 1024 ];
	wsprintf( strPath , "%s\\Map\\Tile\\*.*" ,	m_strTextureDirectory );

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path �� ������ ã�ƺ���.
	hFind = FindFirstFile( strPath , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( strPath , drive, dir, fname, ext );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "DeleteFiles : FindFile Handle Invalid\n" );
		return count;
	} 
	else 
	{
		char	drive2[ 256 ] , dir2 [ 256 ] , fname2 [ 256 ] , ext2[ 256 ];
		char	drive3[ 256 ] , dir3 [ 256 ] , fname3 [ 256 ] , ext3[ 256 ];

		AcTextureList::TextureInfo 	*pTi	;
		AuNode< TextureInfo >		*pNode	;
		_splitpath( FindFileData.cFileName	, drive2 , dir2 , fname2 , ext2 );
		wsprintf( strFile , "%s%s%s%s" , drive , dir , fname2 , ext2 );

		TOLOWERSTRING( fname2 );
		TOLOWERSTRING( ext2 );

		// ����Ʈ�� �ִ��� �˻�..
		// 

		pNode = TextureList.GetHeadNode();
		while( pNode )
		{
			pTi = & pNode->GetData();

			_splitpath( pTi->strFilename	, drive3 , dir3 , fname3 , ext3 );
			
			TOLOWERSTRING( fname3 );
			TOLOWERSTRING( ext3 );

			if( !strcmp( fname2 , fname3 ) && !strcmp( ext2 , ext3 ) )
			{
				break;
			}
			pNode = pNode->GetNextNode();
		}

		if( NULL == pNode && strcmp( ext2 , ".txt" ) )
		{
			if( DeleteFile( strFile ) )
				count ++;
			else
			{
				// ����Ʈ ����,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}


		while( FindNextFile( hFind , & FindFileData ) )
		{
			_splitpath( FindFileData.cFileName	, drive2 , dir2 , fname2 , ext2 );
			wsprintf( strFile , "%s%s%s%s" , drive , dir , fname2 , ext2 );

			TOLOWERSTRING( fname2 );
			TOLOWERSTRING( ext2 );

			pNode = TextureList.GetHeadNode();
			while( pNode )
			{
				pTi = & pNode->GetData();

				_splitpath( pTi->strFilename	, drive3 , dir3 , fname3 , ext3 );
				
				TOLOWERSTRING( fname3 );
				TOLOWERSTRING( ext3 );

				if( !strcmp( fname2 , fname3 ) && !strcmp( ext2 , ext3 ) )
				{
					break;
				}
				pNode = pNode->GetNextNode();
			}

			if( NULL == pNode && strcmp( ext2 , ".txt" ) )
			{
				if( DeleteFile( strFile ) )
					count ++;
				else
				{
					// ����Ʈ ����,.
					char strDebug[ 256 ];
					wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
					OutputDebugString( strDebug );
				}
			}
		}

		// ó�� ��
	}
	

	return count;
}