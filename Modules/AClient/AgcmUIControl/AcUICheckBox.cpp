// AcUICheckBox.cpp: implementation of the AcUICheckBox class.
//
//////////////////////////////////////////////////////////////////////

#include <rwcore.h>
#include <rpworld.h>
#include <AgcSkeleton/skeleton.h>
#include <AgcSkeleton/menu.h>
#include <AgcSkeleton/camera.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUIControl/AcUICheckBox.h>

extern AgcEngine *	g_pEngine;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AcUICheckBox::AcUICheckBox( void )
{
	m_nType					= TYPE_CHECKBOX;
	
	m_bChecked				= FALSE;
	m_lCheckedBoxIndex[0]	= 0;	
	m_lCheckedBoxIndex[1]	= 0;						
	m_lTextPos_x			= 0;									
	m_lTextPos_y			= 0;									
	m_bTextWrite			= FALSE;
	m_cCheckedBoxMode		= 0;

	m_fTextureWidth			= 0.0f;
	m_fTextureHeight		= 0.0f;
}

AcUICheckBox::~AcUICheckBox( void )
{
}

/*****************************************************************
*   Function : OnAddImage
*   Comment  : OnAddImage function Overriding.
*   Date&Time : 2002-11-12, ���� 3:56
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUICheckBox::OnAddImage( RwTexture* pTexture )
{
	if( pTexture != NULL && pTexture->raster != NULL )
	{
		m_fTextureWidth = ( float )RwRasterGetWidth( pTexture->raster );
		m_fTextureHeight = ( float )RwRasterGetHeight( pTexture->raster );
	}
		
	return;
}

/*****************************************************************
*   Function : OnPostRender
*   Comment  : OnPostRender function Overriding
*   Date&Time : 2002-11-12, ���� 4:10
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUICheckBox::OnPostRender( RwRaster *raster )
{
}

/*****************************************************************
*   Function : OnWindowRender
*   Comment  : OnWindowRender
*   Date&Time : 2003-06-24, ���� 2:19
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUICheckBox::OnWindowRender( void )
{
	PROFILE("AcUICheckBox::OnWindowRender");

	if( m_csTextureList.GetCount() <= 0 ) 
	{
		AcUIBase::OnWindowRender();
		return;
	}

	int nAbsolute_x = 0;
	int nAbsolute_y = 0;
	ClientToScreen( &nAbsolute_x, &nAbsolute_y );
	
	stTextureListInfo* pTextureListInfo = m_csTextureList.GetHeadTextureListInfo();
	ASSERT( NULL != pTextureListInfo );

	if( pTextureListInfo->pTexture )
	{
		g_pEngine->DrawIm2D( pTextureListInfo->pTexture, ( float )nAbsolute_x, ( float )nAbsolute_y, w, h,
			m_stRenderInfo.fStartU, m_stRenderInfo.fStartV, ( float )( w / m_fTextureWidth ), ( float )( h / m_fTextureHeight ),
			0xffffffff, ( UINT8 )( 255.0f * ( m_pfAlpha ? *m_pfAlpha : 1 ) ) );
	}
}


/*****************************************************************
*   Function : OnLButtonDown
*   Comment  : OnLButtonDown function overriding
*   Date&Time : 2002-11-12, ���� 4:18
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL AcUICheckBox::OnLButtonDown( RsMouseStatus *ms	)
{
	//��Ȱ�� �϶��� �׳� ������. 
	if( m_cCheckedBoxMode == ACUICHECKBOX_MODE_UNCHECK_DISABLE || m_cCheckedBoxMode == ACUICHECKBOX_MODE_CHECK_DISABLE ) return TRUE;
	if( m_bChecked )
	{
		m_bChecked = FALSE;	
		m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_ACTIVE;						//���콺�� �ö�� �ֱ� ������ 
		
		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_CHECKBOX_ON_UNCHECK, ( PVOID )&m_lControlID );
		}
	}
	else
	{
		m_bChecked = TRUE;	
		m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_ACTIVE;						//���콺�� �ö�� �ֱ� ������ 
												
		if( pParent )
		{
			pParent->SendMessage( MESSAGE_COMMAND, ( PVOID )UICM_CHECKBOX_ON_CHECK, ( PVOID )&m_lControlID );
		}
	}
	
	return TRUE;
}

/*****************************************************************
*   Function  : OnSetFocus
*   Comment   : OnSetFocus
*   Date&Time : 2003-04-25 ���� 4:15:52
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUICheckBox::OnSetFocus( void )
{
	// AcUIBase ���� ó�� 
	AcUIBase::OnSetFocus();

	// To Do ..
	switch( m_cCheckedBoxMode )
	{
	case ACUICHECKBOX_MODE_UNCHECK_DISABLE :
	case ACUICHECKBOX_MODE_CHECK_DISABLE :
		{
			return;
		}
		break;

	case ACUICHECKBOX_MODE_UNCHECK_NORMAL :
		{
			m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_ACTIVE;
		}
		break;

	case ACUICHECKBOX_MODE_CHECK_NORMAL :
		{
			m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_ACTIVE;
		}
		break;
	}
}

/*****************************************************************
*   Function  : OnKillFocus
*   Comment   : OnKillFocus
*   Date&Time : 2003-04-25 ���� 4:15:54
*   Code By   : Seong Yon-jun@NHN Studio 
*****************************************************************/
void AcUICheckBox::OnKillFocus( void )
{
	// AcUIBase ���� ó��
	AcUIBase::OnKillFocus();

	// To Do ..
	switch( m_cCheckedBoxMode )
	{
	case ACUICHECKBOX_MODE_UNCHECK_DISABLE :
	case ACUICHECKBOX_MODE_CHECK_DISABLE :
		{
			return;
		}
		break;

	case ACUICHECKBOX_MODE_UNCHECK_ACTIVE :
		{
			m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_NORMAL;
		}
		break;

	case ACUICHECKBOX_MODE_CHECK_ACTIVE :
		{
			m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_NORMAL;
		}
		break;
	}
}

BOOL AcUICheckBox::OnMouseMove( RsMouseStatus *ms )
{
	return TRUE;
}



///*****************************************************************
//*   Function : SetCheckedValue
//*   Comment  : CheckedValue�� ������ Set�Ѵ�. 
//*   Date&Time : 2002-11-12, ���� 4:09
//*   Code By : Seong Yon-jun@NHN Studio 
//*****************************************************************/
//void AcUICheckBox::SetCheckedValue( BOOL bChecked )
//{
//	m_bChecked = bChecked;
//
//	switch( m_cCheckedBoxMode )
//	{
//	// Disable ��ų��
//	case ACUICHECKBOX_MODE_UNCHECK_DISABLE :
//	case ACUICHECKBOX_MODE_CHECK_DISABLE :
//		{
//			if( bChecked )
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_DISABLE;
//			}
//			else
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_DISABLE;
//			}
//		}
//		break;
//
//	// Active ��ų��
//	case ACUICHECKBOX_MODE_UNCHECK_ACTIVE :
//	case ACUICHECKBOX_MODE_CHECK_ACTIVE :
//		{
//			if( bChecked )
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_ACTIVE;
//			}
//			else
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_ACTIVE;
//			}
//		}
//		break;
//
//	default :
//		{
//			if( bChecked )
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_NORMAL;
//			}
//			else
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_NORMAL;
//			}
//		}
//		break;
//	}
//}
//
///*****************************************************************
//*   Function : SetTextUse
//*   Comment  : ����� Text�� Set�Ѵ�. 
//*   Date&Time : 2002-11-12, ���� 4:12
//*   Code By : Seong Yon-jun@NHN Studio 
//*****************************************************************/
//void AcUICheckBox::SetTextUse( BOOL bTextUse, char* szText, INT32 lTextPosX, INT32 lTextPosY )
//{
//	if( bTextUse == FALSE ) 
//	{
//		m_bTextWrite = FALSE;
//		return;
//	}
//
//	strncpy( m_szCheckedBoxText, szText, ACUICHECKEDBOX_H_MAX_STRING );
//	m_bTextWrite = TRUE;
//	m_lTextPos_x = lTextPosX;
//	m_lTextPos_y = lTextPosY;
//}
//
///*****************************************************************
//*   Function : SetCheckedBoxEnable
//*   Comment  : Checked�� ���θ� Set�Ѵ�. 
//*   Date&Time : 2002-11-12, ���� 5:13
//*   Code By : Seong Yon-jun@NHN Studio 
//*****************************************************************/
//void AcUICheckBox::SetCheckedBoxEnable( BOOL bEnable )
//{
//	if( bEnable )
//	{
//		if( ACUICHECKBOX_MODE_UNCHECK_ACTIVE == m_cCheckedBoxMode || ACUICHECKBOX_MODE_CHECK_ACTIVE == m_cCheckedBoxMode )
//		{
//			if( m_bChecked )
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_ACTIVE;
//			}
//			else 
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_ACTIVE;
//			}
//		}
//		else
//		{
//			if( m_bChecked )
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_NORMAL;
//			}
//			else 
//			{
//				m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_NORMAL;
//			}
//		}
//	}
//	else
//	{
//		if( m_bChecked )
//		{
//			m_cCheckedBoxMode = ACUICHECKBOX_MODE_CHECK_DISABLE;
//		}
//		else
//		{
//			m_cCheckedBoxMode = ACUICHECKBOX_MODE_UNCHECK_DISABLE;
//		}
//	}
//}
