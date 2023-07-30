
#include <AgcmUIControl/AcuTextureList.h>
#include <AgcmResourceLoader/AgcmResourceLoader.h>
#include <rtpng.h>



AcuTextureList::AcuTextureList( void )
{
	m_lAddImageIndex = 1;
}

AcuTextureList::~AcuTextureList( void )
{
	DestroyTextureList();
}

INT32 AcuTextureList::AddImage( char * filename, BOOL bRead	)
{
	ASSERT( NULL != filename );

	stTextureListInfo TextureListInfo;

	strcpy( TextureListInfo.szName, filename );
	TextureListInfo.pTexture = NULL;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	if( bRead )
	{
		RwTexture* pTexture = LoadTexture( filename );

		ASSERT( NULL != pTexture && "���Ϸε�üũ" );
		ASSERT( NULL != pTexture->raster && "���Ϸε�üũ" );

		INT32 index = m_listTexture.GetCount();
		TextureListInfo.pTexture = pTexture;

		if( pTexture && pTexture->raster )
		{
			if( pTexture->raster->width >= 1024 || pTexture->raster->height >= 768 )
			{
				RwTextureSetFilterMode( pTexture, rwFILTERLINEAR );
			}
		}
	}

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

RwTexture* AcuTextureList::GetImage_ID( INT32 lID )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );

		if( TextureListInfo.lTextureListID == lID )
		{
			return TextureListInfo.pTexture;
		}
		else if( TextureListInfo.lTextureListID > lID )
		{
			return NULL;
		}
	}

	return NULL;
}

CHAR* AcuTextureList::GetImageName_ID( INT32 lID )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.lTextureListID == lID )
		{
			return TextureListInfo.szName;
		}
		else if( TextureListInfo.lTextureListID > lID )
		{
			return NULL;
		}
	}

	return NULL;
}

RwTexture* AcuTextureList::GetImage_Index( INT32 lIndex, INT32 *plID )
{
	if( m_listTexture.GetCount() <= lIndex ) return NULL;
	if( 0 == m_listTexture.GetCount() ) return NULL;

	// plID �Է� 
	if( plID )
	{
		*plID = m_listTexture[ lIndex ].lTextureListID;
	}

	return m_listTexture[ lIndex ].pTexture;		
}

BOOL AcuTextureList::SetImage_Index( INT32 lIndex, char *filename,	BOOL bRead )
{
	if( m_listTexture.GetCount() <= lIndex ) return FALSE;

	// bRead �� TRUE �� ��� �̹����� �̹� �ε�Ǿ� �־ �ٽ� �ε��Ѵ�.
	if( bRead )
	{
		// �̹� �ε�Ǿ� ������ �ϴ� �����ϰ�..
		if( m_listTexture[ lIndex ].pTexture )
		{
			_SafeReleaseTexture( m_listTexture[ lIndex ].pTexture );
			m_listTexture[ lIndex ].pTexture = NULL;
		}

		// �̹��� �ε�..
		m_listTexture[ lIndex ].pTexture = LoadTexture( filename );
	}

	// �̹����� �ε��ϵ� ���ϵ� �̸��� �������ش�.
	strcpy( m_listTexture[ lIndex ].szName, filename );
	return TRUE;
}

BOOL AcuTextureList::SetImageName_Index( INT32 lIndex, CHAR *szName	)
{
	if( m_listTexture.GetCount() <= lIndex ) return FALSE;
	strcpy( m_listTexture[ lIndex ].szName, szName );
	return TRUE;
}

BOOL AcuTextureList::SetImage_ID( INT32 lID, char *filename, BOOL bRead )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if ( lID == TextureListInfo.lTextureListID ) 
		{
			// bRead �� TRUE �� �̹����� �����Ѵ�.
			if( TextureListInfo.pTexture )
			{
				_SafeReleaseTexture( TextureListInfo.pTexture );
				TextureListInfo.pTexture = NULL;
			}

			// �̹��� �ε�
			TextureListInfo.pTexture = LoadTexture( filename );

			// ���ϸ� ����
			strcpy( TextureListInfo.szName, filename );
			return TRUE;

		}
		else if( lID < TextureListInfo.lTextureListID )
		{
			return FALSE;
		}
	}

	return FALSE;		
}

BOOL AcuTextureList::SetImageID_Name( INT32 lID, char *filename, BOOL bRead	)
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo &	TextureListInfo = m_listTexture.GetNext( pListNode );
		if( !strcmp( filename, TextureListInfo.szName ) )
		{
			TextureListInfo.lTextureListID = lID;

			// �̸��� ���� Texture �� ��������� �̹� �ε�� ���ε�..
			// bRead �� TRUE �� �����ϰ� �ٽ� �ε�, �ƴϸ� �� �Ѿ��.
			if( bRead )
			{
				// �̹����� �̹� �ε��Ǿ� ������ ����
				if( TextureListInfo.pTexture )
				{
					_SafeReleaseTexture( TextureListInfo.pTexture );
					TextureListInfo.pTexture = NULL;
				}

				// �̹��� �ε�
				TextureListInfo.pTexture = LoadTexture( filename );

				// ���ϸ� ����
				strcpy( TextureListInfo.szName, filename );
			}

			return TRUE;
		}
	}

	return FALSE;
}

INT32 AcuTextureList::AddOnlyThisImage( char* filename, BOOL bLoad )
{
	DestroyTextureList();
	ASSERT( NULL != filename );

	stTextureListInfo TextureListInfo;

	TextureListInfo.pTexture = NULL;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	if( bLoad )
	{
		RwTexture* pTexture = LoadTexture( filename );

		ASSERT( NULL != pTexture && "���Ϸε�üũ" );
		ASSERT( NULL != pTexture->raster && "���Ϸε�üũ" );

		TextureListInfo.pTexture = pTexture;
	}

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

void AcuTextureList::DestroyTextureList( void )
{
	AuNode< stTextureListInfo >* pNode = NULL	;
	stTextureListInfo TextureListInfo;

	while( pNode = m_listTexture.GetHeadNode() )
	{
		TextureListInfo = pNode->GetData();
		if( TextureListInfo.pTexture )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}

		m_listTexture.RemoveHead();
	}		
}

INT32 AcuTextureList::GetCount( void )
{
	return m_listTexture.GetCount();
}

stTextureListInfo* AcuTextureList::GetHeadTextureListInfo( void )
{
	return &m_listTexture.GetHead();
}

BOOL AcuTextureList::DeleteImage_ID( INT32 lID, BOOL bDestroyTexture )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();
	stTextureListInfo TextureListInfo;

	while( pListNode )
	{
		TextureListInfo = m_listTexture.GetNext( pListNode );
		if( lID == TextureListInfo.lTextureListID ) 
		{
			// ���� ã������ ������!
			if ( NULL != TextureListInfo.pTexture && bDestroyTexture ) 
			{
				_SafeReleaseTexture( TextureListInfo.pTexture );
				TextureListInfo.pTexture = NULL;
			}

			m_listTexture.RemoveData( TextureListInfo );
			return TRUE;
		}
		else if( lID < TextureListInfo.lTextureListID )
		{
			return FALSE;
		}
	}

	return FALSE;		
}

BOOL AcuTextureList::DeleteImage_Index( INT32 lIndex )
{
	if ( m_listTexture.GetCount() <= lIndex ) return FALSE;

	stTextureListInfo TextureListInfo = m_listTexture[ lIndex ];

	if( TextureListInfo.pTexture )
	{
		_SafeReleaseTexture( TextureListInfo.pTexture );
		TextureListInfo.pTexture = NULL;
	}
	
	m_listTexture.RemoveData( m_listTexture[ lIndex ] );
	return TRUE;
}

AcuTextureList& AcuTextureList::operator=( const AcuTextureList& csTextureList ) 
{
	AuList< stTextureListInfo >	listTemp;

	listTemp = m_listTexture;
	m_listTexture = csTextureList.m_listTexture;
	m_lAddImageIndex = csTextureList.m_lAddImageIndex;
	
	// Add Reference Count
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	stTextureListInfo TextureListInfo;
	while( pListNode )
	{
		TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture )
		{
			// Add Reference Count 
			RwTextureAddRef( TextureListInfo.pTexture );
		}
	}

	pListNode = listTemp.GetHeadNode();
	while( pListNode )
	{
		TextureListInfo = listTemp.GetNext( pListNode );
		if( TextureListInfo.pTexture )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}
	}

	listTemp.RemoveAll();
	return *this;
}

CHAR* AcuTextureList::GetImageName_Index( INT32 lIndex, INT32 *plID	)
{
	if ( m_listTexture.GetCount() <= lIndex ) return NULL;
	if ( !m_listTexture.GetCount() ) return NULL;

	// plID �Է� 
	if( NULL != plID )
	{
		*plID = m_listTexture[ lIndex ].lTextureListID;
	}

	return m_listTexture[ lIndex ].szName;		
}

void AcuTextureList::LoadTextures( void	)
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture == NULL )
		{
			if ( TextureListInfo.szName == NULL || TextureListInfo.szName[ 0 ] == NULL ) continue;

			TextureListInfo.pTexture = LoadTexture( TextureListInfo.szName );
			if( !TextureListInfo.pTexture )
			{
				// �б���н� �ѹ��� �õ�.. RwTextureRead �� ���� �����Ҽ��� �ִ��.
				TextureListInfo.pTexture = LoadTexture( TextureListInfo.szName );
			}
		}
	}
}

RwTexture* AcuTextureList::LoadTexture( char* pFileName )
{
	if( pFileName == NULL || strlen( pFileName ) == 0 ) return NULL;

	// Texture ReadCallBack �� AgcmResourceLoader::CBTextureRead() �� ����Ǿ� �ִ�.
	// ���� �ݹ��� ���ؼ� �ε��� �õ��غ���.

	RwTexture* pTexture = RwTextureRead( pFileName, NULL );
	if( !pTexture )
	{
		// �ε��� �����ߴٸ� Renderware �� DefaultReadCallBack �� ���� ���� �ε��õ��Ѵ�.
		RwTextureCallBackRead fnReadCallBack = AgcmResourceLoader::GetDefaultCBReadTexture();
		if( !fnReadCallBack )
		{
#ifdef _DEBUG
			CHAR strDebug[ 256 ] = { 0, };
			sprintf( strDebug, "Texture Load Fail : %s ( Cannot Find Default ReadCallBack )\n", pFileName );
			OutputDebugString( strDebug );
			return NULL;
#endif
		}

		// ���ϸ� Ȯ���ڰ� �����ϸ� Ȯ���ڸ� �����Ѵ�.
		CHAR strFileName[ 256 ] = { 0, };
		CHAR strExtension[ 16 ] = { 0, };
		_ParseTextureFileName( pFileName, strFileName, strExtension );

		pTexture = fnReadCallBack( strFileName, NULL );
		if( !pTexture )
		{
#ifdef _DEBUG
			CHAR strDebug[ 256 ] = { 0, };
			sprintf( strDebug, "Texture Load Fail : %s ( Use Renderware DefaultCallBack )\n", pFileName );
			OutputDebugString( strDebug );
#endif
			return NULL;
		}
	}

	RwTextureSetFilterMode( pTexture, rwFILTERNEAREST );
	RwTextureSetAddressing( pTexture, rwTEXTUREADDRESSCLAMP );
	return pTexture;
}

void AcuTextureList::UnloadTextures( void )
{
	AuNode< stTextureListInfo >* pListNode = NULL;
	pListNode = m_listTexture.GetHeadNode();

	while( pListNode )
	{		
		stTextureListInfo& TextureListInfo = m_listTexture.GetNext( pListNode );
		if( TextureListInfo.pTexture != NULL )
		{
			_SafeReleaseTexture( TextureListInfo.pTexture );
			TextureListInfo.pTexture = NULL;
		}
	}
}

INT32 AcuTextureList::AddTexture( RwTexture* pstTexture )
{
	stTextureListInfo TextureListInfo;

	TextureListInfo.szName[ 0 ] = 0;
	TextureListInfo.pTexture = pstTexture;
	TextureListInfo.lTextureListID = m_lAddImageIndex;

	m_listTexture.AddTail( TextureListInfo );
	m_lAddImageIndex++;

	return ( m_lAddImageIndex - 1 );
}

BOOL AcuTextureList::_SafeReleaseTexture( RwTexture* pTexture )
{
	try
	{
		RwTextureDestroy( pTexture );
		pTexture = NULL;
	}
	catch( ... )
	{
		return FALSE;
	}

	return TRUE;
}

void AcuTextureList::_ParseTextureFileName( char* pFullFileName, char* pFileName, char* pExtension )
{
	if( !pFullFileName ) return;
	if( !pFileName ) return;
	if( !pExtension ) return;	

	INT32 nIndex = 0;
	INT32 nExtCount = 0;
	BOOL bIsExtension = FALSE;

	while( 1 )
	{
		if( pFullFileName[ nIndex ] == '\0' ) return;
		if( pFullFileName[ nIndex ] == '.' )
		{
			bIsExtension = TRUE;
		}
		else
		{
			if( !bIsExtension )
			{
				pFileName[ nIndex ] = pFullFileName[ nIndex ];
			}
			else
			{
				pExtension[ nExtCount ] = pFullFileName[ nIndex ];
				++nExtCount;
			}
		}

		++nIndex;
	}
}