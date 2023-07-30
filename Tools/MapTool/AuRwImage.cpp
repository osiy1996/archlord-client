// AuRwImage.cpp: implementation of the AuRwImage class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "rwcore.h"
#include "rpworld.h"

#include <skeleton.h>
#include <menu.h>

#include <MagDebug.h>
//#include "src/MyEngine.h"
//#include "src/MainWindow.h"
#include <win.h>

#include "AuRwImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

BOOL	AuRwImage::Create( INT32 nWidth , INT32 nHeight , INT32 nDepth , BOOL bAllocPixel )
{
	Destroy();

	m_pImage = RwImageCreate( nWidth , nHeight , nDepth );

	if( NULL == m_pImage )
	{
		TRACE( "��� �̹��� ����\n" );
		return FALSE;
	}

	if( bAllocPixel )
	{
		VERIFY( RwImageAllocatePixels( m_pImage ) );
	}

	return TRUE;
}

RwRaster *	AuRwImage::CheckOutRaster( INT32 nFlag )
{
	ASSERT( NULL != m_pImage );
	if( NULL == m_pImage ) return NULL;
	if( NULL != m_pRaster )
	{
		++m_nCount;	// CheckOut Count
		return m_pRaster;
	}

	RwRaster	* pRaster;
	pRaster	= RwRasterCreate(
		RwImageGetWidth( m_pImage ) ,
		RwImageGetWidth( m_pImage ) ,
		RwImageGetDepth( m_pImage ) ,
		nFlag						);

	ASSERT( NULL != pRaster );
	if( NULL == pRaster ) return NULL;

	if( RwRasterSetFromImage( pRaster , m_pImage ) )
	{
		// �����͸���.
		// ����ϰ� ��Ʈ���� �� �Ұ�!.
		++m_nCount;	// CheckOut Count
		m_pRaster = pRaster;
		return m_pRaster;
	}
	else
	{
		TRACE("����ġ ���� ���� �ٻ�\n" );
		VERIFY( RwRasterDestroy( pRaster ) );
		return NULL;
	}
}

void		AuRwImage::CheckInRaster	()
{
	if( NULL == m_pRaster ) return;

	ASSERT( m_nCount > 0 );

	--m_nCount;
	if( m_nCount )
	{
		// ī��Ʈ�� ����������..
		return;
	}
	else
	{
		VERIFY( RwRasterDestroy( m_pRaster ) );
		m_pRaster = NULL;
		return;
	}
}


RwRaster *	AuRwImage::GetSubRaster( int x , int y , int cx , int cy , INT32 nFlag )
{
	ASSERT( NULL != m_pImage );
	if( NULL == m_pImage ) return NULL;

	ASSERT( x		<= RwImageGetWidth( m_pImage ) );
	ASSERT( y		<= RwImageGetWidth( m_pImage ) );
	ASSERT( x + cx	<= RwImageGetWidth( m_pImage ) );
	ASSERT( y + cy	<= RwImageGetWidth( m_pImage ) );

	RwRaster * pImageRaster = CheckOutRaster();
	ASSERT( NULL != pImageRaster );
	if( NULL == pImageRaster )
	{
		return NULL;
	}

	RwRect		rect;

	rect.x	= x	;
	rect.y	= y	;
	rect.w	= cx;
	rect.h	= cy;

	// ���극���� ����..
	RwRaster	* pSubRaster;
	VERIFY( pSubRaster = RwRasterCreate( 0 , 0 , 8 , nFlag ) );

	VERIFY( RwRasterSubRaster( pSubRaster , pImageRaster , & rect ) );

	CheckInRaster();

	return pSubRaster;
	// ���극���͸� Create �Ͽ� �����Ѵ�.
	// �� �����ʹ� Ref Count �����Ͽ� , ���극���ʹ� ����
	// �� �����ʹ� �ڻ쳯 ���ɼ��� ����.
	// �̰� ȣ���ϱ����� CheckOutRaster�� ��! ȣ���ϰ�
	// ���극���� ����Ŀ� ���극���Ϳ� �Բ� CheckInRaster�� ���ֵ���.
}

BOOL	AuRwImage::Load( char * pFileName )
{
	if( GetImagePointer() )
	{
		// �ε��ž��ִ�..
		TRACE( "�޸� ����� �� �Ŀ� �ε����ּ���.\n" );
		VERIFY( Destroy() );
	}

	RwImage * pImage = RwImageRead( pFileName );

	if( pImage )
	{
		// ����;;
		m_pImage = pImage;
		return TRUE;
	}
	else
	{
		// ��������!..
		return FALSE;
	}
}

BOOL	AuRwImage::CreateSubImage	( AuRwImage * pDestImage , int x , int y , int cx , int cy )
{
	ASSERT( NULL != pDestImage	);
	ASSERT( NULL != m_pImage	);
	ASSERT( NULL != m_pImage	);
	if( pDestImage->GetImagePointer() )
	{
		TRACE( "�� ����� �ϼ� -_-\n" );
		pDestImage->Destroy();
	}

	ASSERT( x		<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( y		<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( x + cx	<= RwImageGetWidth( GetImagePointer() ) );
	ASSERT( y + cy	<= RwImageGetWidth( GetImagePointer() ) );

	CheckOutRaster();

	RwRaster	*	pSubRaster = GetSubRaster( x , y , cx , cy );
	ASSERT( NULL != pSubRaster );
	if( NULL == pSubRaster ) return FALSE;

	RwRaster	*	pDummyRaster = RwRasterCreate( cx , cy , GetDepth() , rwRASTERTYPENORMAL );
	ASSERT( NULL != pDummyRaster );

	RwRasterPushContext( pDummyRaster );
	RwRasterRender( pSubRaster , 0 , 0 );
	RwRasterPopContext();

	VERIFY( pDestImage->Create( cx , cy , GetDepth() ) );
	VERIFY( pDestImage->SetFromRaster( pDummyRaster ) );

	RwRasterDestroy( pSubRaster		);
	RwRasterDestroy( pDummyRaster	);
	CheckInRaster();

	return TRUE;
}
