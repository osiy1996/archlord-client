#include "AgcuUIControlPostFX.h"
#include <AgcmUILogin/AgcmUILogin.h>
#include <AgcmPostFX/AgcmPostFX.h>
#include <AgcModule/AgcEngine.h>

AgcuUIControlPostFX::AgcuUIControlPostFX( void )
: m_pPostFX( NULL ), m_bIsTranslate( FALSE ), m_bIsPostFXOn( FALSE ),
	m_fTranslateElapsedTime( 0.0f ), m_fTranslateTimeScale( 1.0f )
{
	memset( m_strPostFXPipeLine, 0, sizeof( CHAR ) * 256 );
}

BOOL AgcuUIControlPostFX::OnInitialzie( void* pUILogin )
{
	AgcmUILogin* pcmUILogin = ( AgcmUILogin* )pUILogin;
	if( !pcmUILogin ) 
		return FALSE;

	m_pPostFX = ( AgcmPostFX* )pcmUILogin->GetModule( "AgcmPostFX" );
	if( !m_pPostFX ) 
		return FALSE;

	m_bIsTranslate			= FALSE;
	m_bIsPostFXOn			= FALSE;

	m_fTranslateElapsedTime = 0.0f;
	m_fTranslateTimeScale	= 1.0f;

	return TRUE;
}

BOOL AgcuUIControlPostFX::OnUpdate( float fElapsedTime )
{
	if( !m_pPostFX ) 
		return FALSE;

	if( !m_bIsTranslate ) 
		return FALSE;

	// calculate passed time
	float fScaleTime		=	m_fTranslateTimeScale * fElapsedTime;
	m_fTranslateElapsedTime +=	fElapsedTime;

	if( m_fTranslateElapsedTime < 1.0f )
	{
		// passed time under 1 second, save time radius
		m_pPostFX->SetRadius( "AgcuUIControlPostFX" , m_fTranslateElapsedTime );
	}
	else
	{
		// if PostFX is enable..
		if( m_bIsPostFXOn )
		{
			// set rendering pipeline to PostFX, and On it
			m_pTechnique	=	m_pPostFX->CreateFX( "AgcuUIControlPostFX" , E_PIPELINE_TRANSCROSSFADE );
			m_pPostFX->PostFX_ON();
		}
		else
			m_pPostFX->PostFX_OFF( TRUE );
		
		m_bIsTranslate = FALSE;
	}

	return TRUE;
}

BOOL AgcuUIControlPostFX::OnStartPostFX( void )
{
	// it must enable PostFX, and not running current
	if( !m_pPostFX ) 
		return FALSE;

	if( m_bIsTranslate ) 
		return FALSE;

	if( !m_pPostFX->isReady() )
	{
		m_pPostFX->createRenderTargets();
	}

	m_fTranslateTimeScale = 2.0f;
	if( g_pEngine )
		g_pEngine->OnRender();
	
	m_pPostFX->saveImmediateFrame();

	m_pTechnique	=	m_pPostFX->CreateFX( "AgcuUIControlPostFX" , E_PIPELINE_TRANSCROSSFADE );
	m_pPostFX->PostFX_ON();
	m_pPostFX->SetRadius( m_pTechnique , 0.0f );

	m_fTranslateElapsedTime = 0.0f;
	m_bIsTranslate			= TRUE;

	return TRUE;
}

BOOL AgcuUIControlPostFX::OnStopPostFX( void )
{
	// it must enable PostFX, and not running current
	if( !m_pPostFX ) 
		return FALSE;

	m_pPostFX->PostFX_OFF();
	return TRUE;
}
