
#include <AgcmUIControl/AcUIBar.h>
#include <AgcModule/AgcEngine.h>


extern AgcEngine* g_pEngine;


#define	ACUIBAR_AFTERIMAGE_MOVE_SPEED					(0.00008f)
#define	ACUIBAR_AFTERIMAGE_MOVE_ACCELERATION			(0.0005f)	



AcUIBar::AcUIBar( void )
{
	m_lTotalPoint			= 1;
	m_lCurrentPoint			= 0;
	m_fCurrRate				= 0.0f;
	m_lEdgeImageID			= 0;
	m_lBodyImageID			= 0;
	
	m_nType					= TYPE_BAR;

	m_bVertical				= FALSE;
	m_eBarType				= 0;

	m_eAnim					= ACUIBAR_ANIM_NONE;
	m_fAfterImageRate		= 0.0f;
	m_fAfterImageMoveSpeed	= 0.0f;

	m_ulLastTick			= 0;
}

AcUIBar::~AcUIBar()
{	
}

/*****************************************************************
*   Function : SetPointInfo
*   Comment  : Point Info �� Setting �Ѵ� - ���⼭ �� Control�� Width�� �����ȴ�
*   Date&Time : 2003-07-18, ���� 3:10
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBar::SetPointInfo( INT32 lTotalPoint, INT32 lCurrentPoint )
{
	// �ִ�ġ�� 0 ���ϸ� ������ ��ġ
	if( lTotalPoint <= 0 )
	{
		m_lTotalPoint = 1;
		m_lCurrentPoint = 0;
		m_fCurrRate = 0.0f;
		return;
	}

	_CalculateNextRate( m_eBarType, m_eAnim, lTotalPoint, lCurrentPoint );

	m_lTotalPoint = lTotalPoint;	
	m_lCurrentPoint = lCurrentPoint;
}

void AcUIBar::_CalculateNextRate( INT32 eBarType, AcUIBarAnimType eAnimType, INT32 lTotalPoint, INT32 lCurrentPoint )
{
	// ���ο� ����� ���
	FLOAT fNextRate = ( FLOAT )lCurrentPoint / ( FLOAT )lTotalPoint ;
	if( fNextRate > 1.0f )
	{
		// ������� ������ ������ ���� ��ġ�� �߸� ���Ե�.. �ִ�ġ�� ���� �ʰ� ����
		fNextRate = 1.0f;
	}

	// ���� ���� ������� ������ ������� �ٸ��ٸ� ������ ���ҵ� �ִϸ��̼��� ����� ���̸� �ӵ��� �����ϴ�.
	if( fNextRate != m_fCurrRate )
	{
		// �ִϸ��̼� ������ �ε巴�� �ϵ��� �Ǿ� �ִٸ�..
		if( eBarType & ACUIBAR_TYPE_SMOOTH )
		{
			// ���ο� ��ǥġ�� ����ġ���� ũ�ٸ� ���� �ִϸ��̼�
			if( fNextRate > m_fCurrRate )
			{
				m_fAfterImageRate = fNextRate;
				m_eAnim = ACUIBAR_ANIM_INCREASE;
			}
			// �ƴ϶�� ���� �ִϸ��̼�
			else
			{
				m_fAfterImageRate = m_fCurrRate;
				m_eAnim = ACUIBAR_ANIM_DECREASE;
			}
		}

		// ���� ��ġ�� ��ǥ ��ġ�� ����
		m_fCurrRate = fNextRate;

		// ���� �� ���� �ִϸ��̼� �ӵ��� �����ӵ��� �����Ѵ�.
		m_fAfterImageMoveSpeed = ACUIBAR_AFTERIMAGE_MOVE_SPEED;
	}
	// ���� ������� ���� ������� ���ٸ�..
	else
	{
		// �������·� ����
		m_eAnim = ACUIBAR_ANIM_NONE;
		m_fAfterImageMoveSpeed = 0.0f;
	}
}

RwTexture* AcUIBar::_GetBodyTexture( void )
{
	return m_csTextureList.GetImage_ID( m_lBodyImageID );
}

RwTexture* AcUIBar::_GetEdgeTexture( void )
{
	return m_csTextureList.GetImage_ID( m_lEdgeImageID );
}

BOOL AcUIBar::_IsValidTexture( RwTexture* pTexture )
{
	// �����Ͱ� �����ΰ�
	if( !pTexture ) return FALSE;
	
	// �����Ͱ� �����ΰ�
	RwRaster* pRaster = RwTextureGetRaster( pTexture );
	if( !pRaster ) return FALSE;

	// �̹��� ����� �����ΰ�
	INT32 lImageWidth = RwRasterGetWidth( pRaster );
	INT32 lImageHeight = RwRasterGetHeight( pRaster );

	if( !lImageWidth || !lImageHeight )	return FALSE;

	// �����ΰ���;;
	return TRUE;
}

BOOL AcUIBar::_DrawTexture( RwTexture* pTexture, INT32 lPosX, INT32 lPosY, INT32 lWidth, INT32 lHeight, INT32 lUStart, INT32 lVStart, INT32 lUEnd, INT32 lVEnd, FLOAT fAlphaValue )
{
	if( !pTexture ) return FALSE;

	DWORD uColor = 0xffffffff;
	FLOAT fAlphaRate = m_pfAlpha ? *m_pfAlpha : 1.0f;

	if( g_pEngine )
	{
		return g_pEngine->DrawIm2DPixel( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, uColor, ( UINT8 )( fAlphaValue * fAlphaRate ) );
	}

	return FALSE;
}

BOOL AcUIBar::_DrawTexture( RwTexture* pTexture, FLOAT fPosX, FLOAT fPosY, FLOAT fWidth, FLOAT fHeight, FLOAT fUStart, FLOAT fVStart, FLOAT fUEnd, FLOAT fVEnd, FLOAT fAlphaValue )
{
	if( !pTexture ) return FALSE;

	DWORD uColor = 0xffffffff;
	FLOAT fAlphaRate = m_pfAlpha ? *m_pfAlpha : 1.0f;

	if( g_pEngine )
	{
		return g_pEngine->DrawIm2D( pTexture, fPosX, fPosY, fWidth, fHeight, fUStart, fVStart, fUEnd, fVEnd, uColor, ( UINT8 )( fAlphaValue * fAlphaRate ) );
	}

	return FALSE;
}

BOOL AcUIBar::_DrawBody( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	switch( m_eAnim )
	{
	case ACUIBAR_ANIM_NONE :
		{
			return _DrawBodyNoAnim( pTexture, lAbsX, lAbsY );
		}
		break;

	default :
		{
			return _DrawBodySmooth( pTexture, lAbsX, lAbsY );
		}
		break;
	}
}

BOOL AcUIBar::_DrawBodyNoAnim( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lOffSet = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	// ���� Bar �� ���..
	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lOffSet = ( int )( ( 1.0f - m_fCurrRate ) * h );
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	// ���� Bar �� ���..
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lOffSet = ( int )( m_fCurrRate * w );
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd );
}

BOOL AcUIBar::_DrawBodySmooth( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	INT32 lOffSetShadow = 0;
	INT32 lOffSetBody = 0;

	// �ִϸ��̼� ����ð��� ���..
	UINT32	curtick = g_pEngine->GetClockCount();
	UINT32	tickdiff = curtick - m_ulLastTick;

	m_ulLastTick = curtick;
	m_fAfterImageMoveSpeed *= ( 1.0f + ACUIBAR_AFTERIMAGE_MOVE_ACCELERATION * tickdiff );

	FLOAT fAlphaValue = 0.0f;

	// �ִϸ��̼� �����Ȳ üũ..
	switch( m_eAnim )
	{
	case ACUIBAR_ANIM_INCREASE :
		{
			// ���� �ִϸ��̼��� ����..
			m_fCurrRate += m_fAfterImageMoveSpeed * tickdiff;

			// ��ǥ�������� ����Ǿ�����..
			if( m_fCurrRate >= m_fAfterImageRate)
			{
				// �ִϸ��̼� ����
				m_eAnim = ACUIBAR_ANIM_NONE;
				m_fCurrRate = m_fAfterImageRate;
			}

			fAlphaValue = 192.0f;
		}
		break;

	case ACUIBAR_ANIM_DECREASE :
		{
			// ���� �ִϸ��̼��� ����..
			m_fAfterImageRate -= m_fAfterImageMoveSpeed * tickdiff;

			// ��ǥ�������� ����Ǿ�����..
			if( m_fAfterImageRate <= m_fCurrRate )
			{
				// �ִϸ��̼� ����
				m_eAnim = ACUIBAR_ANIM_NONE;
			}

			fAlphaValue = 128.0f;
		}
		break;

	default :
		{
			// �� �Լ��� �����ߴٴ� ���� �ִϸ��̼��� �ִٴ� ���ε� ������ ���ҵ� �ƴ϶�� ����!
			return FALSE;
		}
		break;
	}

	// ���� Ȥ�� ���� Bar�� ���� �ش��ϴ� �ܻ�� Body �� OffSet �� ���Ѵ�.
	if( m_bVertical )
	{
		lOffSetShadow = ( INT32 )( ( 1.0f - m_fAfterImageRate ) * h );
		lOffSetBody = ( INT32 )( ( 1.0f - m_fCurrRate ) * h );
	}
	else
	{
		lOffSetShadow = ( INT32 )( m_fAfterImageRate * w );
		lOffSetBody = ( int ) ( m_fCurrRate * w );
	}

	// �ܻ��� ���� �׷��ְ�..
	if( !_DrawBodySmoothShadow( pTexture, lAbsX, lAbsY, lOffSetShadow, fAlphaValue ) )
	{
		return FALSE;
	}

	// ���� Body�� �׷��ش�.
	if( !_DrawBodySmoothBody( pTexture, lAbsX, lAbsY, lOffSetBody, 255.0f ) )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL AcUIBar::_DrawBodySmoothShadow( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDONE );
	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, fAlphaValue );
}

BOOL AcUIBar::_DrawBodySmoothBody( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY, INT32 lOffSet, FLOAT fAlphaValue )
{
	INT32 lPosX = 0;
	INT32 lPosY = 0;
	INT32 lWidth = 0;
	INT32 lHeight = 0;
	INT32 lUStart = 0;
	INT32 lVStart = 0;
	INT32 lUEnd = 0;
	INT32 lVEnd = 0;

	if( m_bVertical )
	{
		lPosX = lAbsX;
		lPosY = lAbsY + lOffSet;
		lWidth = w;
		lHeight = h - lOffSet;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h - lOffSet;
		}
		else
		{
			lUStart = 0;
			lVStart = lOffSet;
			lUEnd = w;
			lVEnd = h;
		}
	}
	else
	{
		lPosX = lAbsX;
		lPosY = lAbsY;
		lWidth = lOffSet;
		lHeight = h;

		if( m_eBarType & ACUIBAR_TYPE_CUT_HEAD )
		{
			lUStart = 0;
			lVStart = 0;
			lUEnd = lOffSet;
			lVEnd = h;
		}
		else
		{
			lUStart = w - lOffSet;
			lVStart = 0;
			lUEnd = w;
			lVEnd = h;
		}
	}

	RwRenderStateSet( rwRENDERSTATEDESTBLEND, ( void* )rwBLENDINVSRCALPHA );
	return _DrawTexture( pTexture, lPosX, lPosY, lWidth, lHeight, lUStart, lVStart, lUEnd, lVEnd, fAlphaValue );
}

BOOL AcUIBar::_DrawEdge( RwTexture* pTexture, INT32 lAbsX, INT32 lAbsY )
{
	FLOAT fPosX = 0.0f;
	FLOAT fPosY = 0.0f;
	FLOAT fWidth = 0.0f;
	FLOAT fHeight = 0.0f;
	FLOAT fUStart = 0.0f;
	FLOAT fVStart = 0.0f;
	FLOAT fUEnd = 0.0f;
	FLOAT fVEnd = 0.0f;
	FLOAT fAlphaValue = 255.0f;

	if( m_bVertical )
	{
		fPosX = ( FLOAT )lAbsX;
		fPosY = ( FLOAT )( lAbsY + h - 1 );
		fWidth = w;
		fHeight = 1.0f;
		fUStart = 0.0f;
		fVStart = 1.0f - 1.0f / ( FLOAT )pTexture->raster->height;
		fUEnd = 1.0f;
		fVEnd = 1.0f;
	}
	else
	{
		fPosX = ( FLOAT )lAbsX;
		fPosY = ( FLOAT )lAbsY;
		fWidth = 1.0f;
		fHeight = h;
		fUStart = 0.0f;
		fVStart = 0.0f;
		fUEnd = 1.0f / ( FLOAT )pTexture->raster->width;
		fVEnd = 1.0f;
	}

	return _DrawTexture( pTexture, fPosX, fPosY, fWidth, fHeight, fUStart, fVStart, fUEnd, fVEnd, fAlphaValue );
}



/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-09-29, ���� 4:02
*   Code By : Seong Yon-jun : NHN Online RPG
*****************************************************************/
void AcUIBar::OnWindowRender()
{
	PROFILE( "AcUIBar::OnWindowRender" );

	// �⺻������ Edge, Body Image�� ���� �־�� �Ѵ� 
	if( !_IsTextureLoaded() )
	{
		// �ؽ�ó�� �ε���� ���� ��� ���� �Լ��� ���ϰ� �Ѿ��.
		AcUIBase::OnWindowRender();
		return;
	}

	RwTexture* pBodyTexture = _GetBodyTexture();
	if( !_IsValidTexture( pBodyTexture ) ) return;

	RwTexture* pEdgeTexture = _GetEdgeTexture();
	if( !_IsValidTexture( pEdgeTexture ) ) return;

	// ���� ȭ���� �ִ� X, Y ���� ���Ѵ�.
	INT32 lAbsoluteX = 0;
	INT32 lAbsoluteY = 0;
	ClientToScreen( &lAbsoluteX, &lAbsoluteY );

	// Body �׷��ְ�..
	_DrawBody( pBodyTexture, lAbsoluteX, lAbsoluteY );

	// Edge �׸���..
	_DrawEdge( pEdgeTexture, lAbsoluteX, lAbsoluteY );
}
