
#include <AgcmUIControl/AcUIModalImage.h>
#include <AgcModule/AgcEngine.h>

AgcmSound* AcUIModalImage::m_pAgcmSound	= NULL;
AgcmFont* AcUIModalImage::m_pAgcmFont = NULL;

AcUIModalImage::AcUIModalImage( void )
{
	m_bUseDrag = FALSE;
	m_bPassPostMessage = FALSE;
}

AcUIModalImage::~AcUIModalImage( void )
{
}

INT32 AcUIModalImage::AddImage( char * filename	)
// ���ϰ��� ��ϵ� �̹����� ������ �ε���.
{
	ASSERT( NULL != filename );

	RwTexture* pTexture = RwTextureRead( filename, NULL );
	ASSERT( NULL != pTexture && "���Ϸε�üũ" );
	if( !pTexture )	return 0;

	INT32 index = m_listTexture.GetCount();
	m_listTexture.AddTail( pTexture );

	if( 0 == index && NULL != pTexture->raster )
	{
		// ������ ũ�� ����
		w = RwRasterGetWidth( pTexture->raster );
		h = RwRasterGetHeight( pTexture->raster );
	}

	OnAddImage( pTexture );
	return index;
}

INT32 AcUIModalImage::AddOnlyThisImage( char * filename )
{
	DestroyTextureList();
	ASSERT( NULL != filename );

	RwTexture* pTexture = RwTextureRead( filename, NULL );
	ASSERT( NULL != pTexture && "���Ϸε�üũ" );

	INT32 index = m_listTexture.GetCount();
	m_listTexture.AddTail( pTexture );

	if ( 0 == index && NULL != pTexture->raster )
	{
		// ������ ũ�� ����
		w = RwRasterGetWidth( pTexture->raster );
		h = RwRasterGetHeight( pTexture->raster );
	}

	OnAddImage( pTexture );
	return index;
}

RwTexture *	AcUIModalImage::GetImage( INT32 index		)
// �ش� �ε����� �̹��� �����͸� ������..
{
	ASSERT( m_listTexture.GetCount() > index );
	if( m_listTexture.GetCount() <= index ) return NULL;
	
	if( m_listTexture.GetCount() ) return m_listTexture[ index ];
	else return NULL;
}

void AcUIModalImage::OnPostRender( RwRaster *raster	)	// ���� ������ ��.
{
}

void AcUIModalImage::OnWindowRender( void )
{
	PROFILE("AcUIModalImage::OnWindowRender");
	if ( m_listTexture.GetCount() <= 0 ) return;

	int nAbsolute_x = 0;
	int nAbsolute_y = 0;
	ClientToScreen( &nAbsolute_x, &nAbsolute_y );

	RwTexture* pTexture = m_listTexture.GetHead();
	ASSERT( NULL != pTexture );
	if( NULL != pTexture && NULL != pTexture->raster )
	{
		g_pEngine->DrawIm2D( pTexture, ( float )nAbsolute_x, ( float )nAbsolute_y, 
			( float )( RwRasterGetWidth( pTexture->raster ) ),
			( float )( RwRasterGetHeight( pTexture->raster ) ) );
	}
}

void AcUIModalImage::OnClose( void )
{
	// ��ϵ� �̹��� ��Ʈ����..
	DestroyTextureList();	
}

BOOL AcUIModalImage::OnLButtonDown( RsMouseStatus *ms )
{
	return FALSE;
}

BOOL AcUIModalImage::OnMouseMove( RsMouseStatus *ms	)
{
	return FALSE;
}

void AcUIModalImage::DestroyTextureList( void )
{
	while( AuNode< RwTexture* >* pNode = m_listTexture.GetHeadNode() )
	{
		RwTexture* pTexture = pNode->GetData();
		if( pTexture )
		{
			RwTextureDestroy( pTexture );
			pTexture = NULL;
		}

		m_listTexture.RemoveHead();
	}	
}

void AcUIModalImage::OnAddImage( RwTexture*	pTexture )
{
	return;
}