// MainWindow.cpp: implementation of the MainWindow class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"

#include "AgcuEffUtil.h"

#include "rwcore.h"
#include "rpworld.h"

#ifdef RWLOGO
#include "rplogo.h"
#endif

#include "rpcollis.h"
#include "rtpick.h"
#include "rtcharse.h"
#include "rpmatfx.h"

#include <skeleton.h>
#include <menu.h>
//#include "events.h"
#include <camera.h>
#include "MainWindow.h"
#include "mmsystem.h"
#include "MyEngine.h"

#include "D3D9.h"

#include "../EffTool.h"
#include "../EffToolDoc.h"
#include "../EffToolView.h"

#include "Camera.h"
#include "PickUser.h"

#include "../ToolOption.h"



//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
MainWindow::MainWindow()
: m_bLBtnCamRot( TRUE )
{
}

MainWindow::~MainWindow()
{
}

BOOL MainWindow::OnInit			(					)	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
{
	// ī�޶� ��ġ ����..
	RwFrame		*pFrame = RwCameraGetFrame	( g_MyEngine.m_pCamera	);
	RwV3d		pos = { 0.0f, 0.0f,  -250.0f };
	RwFrameTranslate( pFrame, &pos, rwCOMBINEREPLACE );

	CCamera::bGetInst().bSetCam( g_MyEngine.m_pCamera );
	
	RwCameraSetFarClipPlane( g_MyEngine.m_pCamera, 50000.f );
	
	CCamera::bGetInst().bRotCX(45.f);
	CCamera::bGetInst().bZoom(-500.f);
	
	return TRUE;				
}

BOOL MainWindow::OnLButtonDown	( RsMouseStatus *ms	)
{	
	return TRUE;
}
BOOL MainWindow::OnLButtonUp	( RsMouseStatus *ms	)
{
	return TRUE;
}
BOOL MainWindow::OnRButtonDown	( RsMouseStatus *ms	)
{
	return TRUE;
}
BOOL MainWindow::OnRButtonUp	( RsMouseStatus *ms	)
{
	return TRUE;
}
BOOL MainWindow::OnMButtonUp( RsMouseStatus *ms	)
{
	return TRUE;
};
BOOL MainWindow::OnMButtonDown( RsMouseStatus *ms	)
{
	return TRUE;
};

BOOL MainWindow::OnMouseWheel	( INT32	lDelta		)
{
	return TRUE;
};

BOOL MainWindow::OnMouseMove	( RsMouseStatus *ms	)
{	
	return TRUE;
}


#include "../MainFrm.h"
void MainWindow::OnPreRender( RwRaster *raster	)	// ������ ���⼭;
{
	CMainFrame* pFrm	= (CMainFrame*) ::AfxGetMainWnd();

	RwRGBA BackgroundColor	= { 0, 0, 0, 255};
	RwCameraClear( g_MyEngine.m_pCamera, &BackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);

	
	if( CMainFrame::bGetThis() )
	{
		//if( CMainFrame::bGetThis()->m_wndStaticTexView.GetSafeHwnd() )
		//	g_MyEngine.bRenderTexPreview( CMainFrame::bGetThis()->m_wndStaticTexView.bGetHwndStatic() );

	}
	//g_MyEngine.bRenderTexPreview( pFrm->bGetDlgTexPreview()->bGetHWnd() );
	
	BackgroundColor.red		= 128;
	BackgroundColor.green	= 128;
	BackgroundColor.blue	= 128;
	BackgroundColor.alpha	= 255;

	RwCameraClear( g_MyEngine.m_pCamera, &BackgroundColor, rwCAMERACLEARZ | rwCAMERACLEARIMAGE);
}

//@{ kday 20050413
// for particle profiling.
#include "AgcuEff2ApMemoryLog.h"
float g_line	= 0;
void	printProfile(const NS_EFF2PARTICLEPROFILE::stProfile& prof)
{
	char	szText[MAX_PATH];
	sprintf( szText, "fname : %s, call : %d, time : %d" , prof.strName.c_str(), prof.callcnt, prof.accum );
	g_pAgcmFont->DrawTextIM2D( 10.f, g_line, szText, 0 );
	g_line += 20.f;
};
//@} kday

void MainWindow::OnRender( RwRaster *raster	)	// ������ ���⼭;
{

	RwD3D9SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
	RwD3D9SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
	RwD3D9SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_POINT );

	//renderstate ����	
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	
	RwRenderStateSet( rwRENDERSTATECULLMODE, (void*)rwCULLMODECULLNONE );

	g_MyEngine.m_FPSOn = TRUE;

	g_pAgcmFont->FontDrawStart(0);
	// FPSǥ�� 
	if( g_MyEngine.GetFpsMode() )
	{
		char	str[ 256 ];
		wsprintf( str , "FPS: %03d", g_MyEngine.GetFps() );
		g_pAgcmFont->DrawTextIM2D( 10.0f, 5.0f, str, 0 );

		LPSTAPMEMORYLOG	particleLog	= 
			AgcuEff2ApMemoryLog::bGetInst().bFind("tagStParticle");
		if( particleLog )
		{
			wsprintf( str, "particle -- max : %d, cnt : %d", particleLog->m_ulMax, particleLog->m_ulCnt );
			g_pAgcmFont->DrawTextIM2D( 10.0f, 25.0f, str, 0 );
		}

		g_line = 45.f;
		NS_EFF2PARTICLEPROFILE::stProfiler::forAll( printProfile );
		NS_EFF2PARTICLEPROFILE::stProfiler::reset();



		//extern STPARTICLEPROFILE	g_particleProFile;		
		//sprintf( str, "particle profiler -- update : %3d, render : %3d, draw : %3d, ur : %5.2f, dr : %5.2f, pur : %5.2f, ptrans : %5.2f"
		//	, g_particleProFile.st.update
		//	, g_particleProFile.st.render
		//	, g_particleProFile.st.draw
		//	, (float)(g_particleProFile.st.update)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.draw)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.render-g_particleProFile.st.draw)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.render-g_particleProFile.st.ptrans)/(float)(g_particleProFile.st.render)*100.f );
		//g_pAgcmFont->DrawTextIM2D( 10.0f, 45.0f, "particle profiler --", 0 );

		//sprintf( str, "update : %3d, render : %3d, draw : %3d, ptrans : %3d"
		//	, g_particleProFile.st.update
		//	, g_particleProFile.st.render
		//	, g_particleProFile.st.draw
		//	, g_particleProFile.st.ptrans
		//	);
		//g_pAgcmFont->DrawTextIM2D( 10.0f, 65.0f, str, 0 );
		//sprintf( str, "u/r : %5.2f, dr : %5.2f, pu/r : %5.2f, ptrans/r : %5.2f"
		//	, (float)(g_particleProFile.st.update)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.draw)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.render-g_particleProFile.st.draw-g_particleProFile.st.ptrans)/(float)(g_particleProFile.st.render)*100.f
		//	, (float)(g_particleProFile.st.ptrans)/(float)(g_particleProFile.st.render)*100.f );
		//g_pAgcmFont->DrawTextIM2D( 10.0f, 85.0f, str, 0 );



		//g_particleProFile.reset();
	}

	g_pAgcmFont->FontDrawEnd();
	
	RwD3D9SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
	RwD3D9SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	RwD3D9SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );
}

/*
 *****************************************************************************
 */
static RpClump *
_destroyClumpCB(RpClump *c, void *d)
{
    RpWorldRemoveClump( g_MyEngine.m_pWorld , c );
    RpClumpDestroy(c);

    return c;
}

void MainWindow::OnClose		(					)	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.
{
	
	g_MyEngine.bDestroyAcessory();
	// Ŭ���� ��Ʈ����. 
	RpWorldForAllClumps( g_MyEngine.m_pWorld , _destroyClumpCB , NULL );
}


BOOL MainWindow::OnKeyDown		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {

	case rsF1:
		break;
	
    case rsLCTRL:
		break;
	
    case rsRCTRL:
		break;
	
	default:
		break;
	}
	return TRUE;
}

BOOL MainWindow::OnKeyUp		( RsKeyStatus *ks	)
{
    switch( ks->keyCharCode )
    {
    case rsLCTRL:
		break;
	
    case rsRCTRL:
		break;
	default:
		break;
	}

	return TRUE;
}

BOOL MainWindow::OnIdle(UINT32 ulClockCount)
{
	/*
	// ���� Ŀ�� ��ġ�� �ִ� �������� �˾Ƴ���.
	RpIntersection	intersection;
	RwLine			pixelRay;
	RwV2d			pos;

	pos				= m_PrevMouseStatus.pos;
	RwCameraCalcPixelRay( g_MyEngine.m_pCamera , &pixelRay , &pos );
	intersection.type	=	rpINTERSECTLINE	;
	intersection.t.line	=	pixelRay		;
	/**/

//	static BOOL bV	= FALSE;
//	if( KEYDOWN_ONECE( 'V', bV ) ){
//		m_bLBtnCamRot	= !m_bLBtnCamRot;// CAMERA ROTATION WITH LBUTTON
//	};

	static BOOL b1	= FALSE;
	if( KEYDOWN_ONECE( '1', b1 ) ){//init camera lookat
		static RwV3d v3dOrigin	= {0.f, 0.f, 0.f};
		CCamera::bGetInst().bSetLookatPt( v3dOrigin );
	}
	
	static BOOL b2	= FALSE;
	if( KEYDOWN_ONECE( '2', b2 ) ){
		g_MyEngine.bBackToOrgShowFrm();
	}


	CCamera::bGetInst().bCameraWorkWithKey();

	AgcdEffGlobal::bGetInst().bSetCurrTime( GetTickCount() );

	return FALSE;	

}