#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4819 )

#include <AgcModule/AgcEngine.h>
#include "rtimport.h"


//#include <DShowMng.h>
class SplashWindow;
enum ePipeLine;
struct Technique;

class CTestPostEffectFX
{
public:
	CTestPostEffectFX() : m_tmStart(0), m_bCircleFX(false), m_pTechnique(NULL)	{		}
	virtual ~CTestPostEffectFX()							{		}

	void	Update();
	void	PlayEffectFX( const char* szText );
	void	PlayEffectFX( ePipeLine	eType );

protected:
	float	m_tmStart;
	bool	m_bCircleFX;
	char	m_pipeBU[1024];
	BOOL	m_bPostFXOn;
	Technique*	m_pTechnique;
};

class CScreenModeManager
{
public:
	CScreenModeManager();
	virtual ~CScreenModeManager();

	BOOL	IsFullScreen()		{	return m_bFullScreen;	}
	void	SetFullWindow();
	BOOL	SetFullScreen( int nWidth , int nHeight , int nDepth );
	void	ReturnScreenMode();

public:
	WINDOWPLACEMENT	m_WPPrev;
	DEVMODE			m_PrevDevMode;
	BOOL			m_bFullScreen;
	DWORD			m_dwPrevWindowStyle;
};

class CLogoViewer
{
private :
	__int64								m_nStartTime;
	__int64								m_nDuration;

	char								m_strImageFileName[ 256 ];
	RwTexture*							m_pTexture;
	float								m_fTextureWidth;
	float								m_fTextureHeight;

public :
	CLogoViewer( void );
	virtual ~CLogoViewer( void );

public :
	BOOL			OnCreate			( void );
	BOOL			OnUpdate			( __int64 nCurrTime );
	BOOL			OnRender			( void );
	BOOL			OnDestroy			( void );
};

class MyEngine;
class MainWindow : public AgcWindow, public CScreenModeManager, public CTestPostEffectFX
{
public:
	MainWindow( MyEngine& cMyEngine );
	virtual ~MainWindow();

	BOOL	OnInit();	// �ʱ�ȭ , ����Ÿ ���� & �׷��� ����Ÿ �ε�.	WindowUI �ǰ�� Open�Ŵ� ����, Full UI�� ��� Setting�Ŵ� ����.
	void	OnClose();	// ������ , �׷��� ����Ÿ ������. WindowUI �� ��� Close�Ŵ� ����,  FullUI�� ��� �ٸ� UI�� Setting�ű� ��.

	BOOL	OnIdle( UINT32 ulClockCount );

	void	OnPreRender( RwRaster* raster	);	// ���� �����ϱ���.
	void	OnRender( RwRaster* raster	);	// ������ ���⼭;
	void	OnPostRender( RwRaster* raster	);	// ������ ���⼭;
	void	OnWindowRender();
	
	BOOL	OnKeyDown( RsKeyStatus *ks	);
	BOOL	OnKeyUp( RsKeyStatus *ks	);

	BOOL	OnChar( char* pChar, UINT lParam )							{	return TRUE;	}
	BOOL	OnIMEComposing( char* composing, UINT lParam )				{	return TRUE;	}
	
	BOOL	PreTranslateInputMessage( RsEvent event , PVOID param	)	{	return FALSE;	}

	void	AutoHuntIdle();
	BOOL	LoadTemplates();

	void	ToggleRidableInfo()			{	m_bShowRidableInfo = !m_bShowRidableInfo;	}
	void	EnableTheaterMode();
	void	IntensiveCrash();
	void	ToggleFullScreenTest();

	void	ToggleMainThreadSleep();
	void	ToggleBackThreadSleep();

	void	ToggleCameraAngle();
	void	AutoHunt();

	void	ReportApMemory();

	void	VideoMemoryCheck();
	void	DrawGeometryGrid();		// ���� �׸��� ��¿�. ����� ����.

	static	BOOL CBObjectLoadError(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBAdminClientLogin(PVOID pData, PVOID pClass, PVOID pCustData);

private:
	void	UpdateLoadState();
	int		GetWindowSizeIndex();

	void	PlayDShow( char* szName );
	void	StopDShow( char* szName );
	BOOL	IsCompleteDShow( char* szName );

public:
	MyEngine&	m_cMyEngine;

	BOOL		m_bLCtrl;
	BOOL		m_bRCtrl;
	BOOL		m_bLAlt;
	BOOL		m_bLShift;
	
	BOOL		m_bDecryption;

	INT32		m_eTemplatesLoaded;
	RwTexture*	m_texLoadingSnda[3];
	RwTexture*	m_texLoadingNHN[3];

	int			m_lShowHeightGrid;
	int			m_lShowGeometryBlocking;
	int			m_lShowSkyBlocking;
	int			m_lShowObjectBlocking;
	int			m_lShowCharacterBlocking;
	int			m_lShowLineBlocking;
	int			m_lShowRidableObjectAvailable;

	BOOL		m_bShowRidableInfo;
	BOOL		m_bEnableTheaterMode;

	RwIm3DVertex*		m_pLineList;
	RwImVertexIndex*	m_pIndex;

	//CDShowMng			m_cDShowMng;

	CLogoViewer			m_LogoGameGrade;

	SplashWindow *m_pSplashWindow;
};





#pragma warning ( pop )
