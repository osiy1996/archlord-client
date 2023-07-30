#ifndef __AGCMPOSTFX_H__
#define __AGCMPOSTFX_H__

#include "AgcModule.h"
#include <vector>
#include <map>
#include <d3dx9effect.h>

#define		TECHNIQUENAME_MAXLENGTH		64
#define		NUM_PARAMS					2
#define		POSTFX_NAME_GENERATE		"AUTO"

class AgcmRender;

enum	ePipeLine
{
		E_PIPELINE_CONTROLEXPOSURE		,
		E_PIPELINE_AVERAGE				,
		E_PIPELINE_COPY					,
		E_PIPELINE_DOWNSAMPLE			,
		E_PIPELINE_BLURH				,
		E_PIPELINE_BLURV				,
		E_PIPELINE_BLURHLC				,
		E_PIPELINE_BLURVLC				,
		E_PIPELINE_BLOOMCOMP			,
		E_PIPELINE_LIGHTADAPT			,
		E_PIPELINE_GRAYSCALE			,
		E_PIPELINE_SEPIA				,
		E_PIPELINE_EDGEDETECT			,
		E_PIPELINE_X2					,
		E_PIPELINE_BOXBLUR1				,
		E_PIPELINE_BOXBLOOMCOMP			,
		E_PIPELINE_NEGATIVE				,
		E_PIPELINE_CLAMPINGCIRCLE		,
		E_PIPELINE_WAVE					,
		E_PIPELINE_SHOCKWAVE			,
		E_PIPELINE_DARKEN				,
		E_PIPELINE_BRIGHTEN				,
		E_PIPELINE_RIPPLE				,
		E_PIPELINE_TWIST				,
		E_PIPELINE_TRANSSATURATE		,
		E_PIPELINE_TRANSMESHUGGAH		,
		E_PIPELINE_TRANSCROSSFADE		,

		E_PIPELINE_COUNT				,
};

struct Technique
{
	CHAR			m_InstanceName	[ TECHNIQUENAME_MAXLENGTH ];	// Instance Name
	CHAR			m_name			[ TECHNIQUENAME_MAXLENGTH ];	// technique name
	INT				m_nParamSize	[ NUM_PARAMS ]; 				// size of the parameter. indicates how many components of float4 are used.
	D3DXVECTOR4		m_vParamDef		[ NUM_PARAMS ];					// parameter default
	CHAR			m_szParamName	[ NUM_PARAMS ][MAX_PATH];		// names of changeable parameters
	D3DXHANDLE		m_hParam		[ NUM_PARAMS ];					// handles to the changeable parameters
	D3DXHANDLE		m_hTechnique;									// post fx technique handle
	D3DXHANDLE		m_bDepedentRead;								// this technique does a dependent texture read?

	DWORD			m_dwStartTime;									// Start Time
	DWORD			m_dwPlayTime;									// Play Time			

	FLOAT			m_fSpeedScale;									// Speed Scale

	FLOAT			m_fRadius;										// Radius
	D3DXVECTOR4		m_vCenter;										// Center

	BOOL			m_bUpdate;										// Update Enable

	D3DXVECTOR4		m_vParam		[NUM_PARAMS];

	Technique						( VOID );
	~Technique						( VOID );

	BOOL			Initialize		( LPD3DXEFFECT pFX, D3DXHANDLE hTechnique );

	VOID			SetStartTime	( DWORD	dwStartTime	)	{	m_dwStartTime	=	dwStartTime;	}
	VOID			SetPlayTime		( DWORD dwPlayTime	)	{	m_dwPlayTime	=	dwPlayTime;		}
	VOID			SetSpeedScale	( FLOAT	fSpeedScale )	{	m_fSpeedScale	=	fSpeedScale;	}
	VOID			SetRadius		( FLOAT fRadius		)	{	m_fRadius		=	fRadius;		}
	VOID			SetCenter		( D3DXVECTOR4& vCenter ){	m_vCenter		=	vCenter;		}
	VOID			SetCenter		( FLOAT fX , FLOAT fY )	{	m_vCenter.x = fX; m_vCenter.y = fY;	}
	VOID			SetUpdate		( BOOL bUpdate )		{	m_bUpdate		=	bUpdate;		}
	
};

class AgcmPostFX : public AgcModule
{
protected:

	typedef	std::vector<LPDIRECT3DBASETEXTURE9>				vectorDXTexture;
	typedef std::vector<LPDIRECT3DBASETEXTURE9>::iterator	vectorDXTextureIter;

	typedef std::map< ePipeLine , D3DXHANDLE >					mapTechHandle;
	typedef std::map< ePipeLine , D3DXHANDLE >::iterator		mapTechHandleIter;

	typedef std::list<Technique*>							ListTechnique;
	typedef std::list<Technique*>::iterator					ListTechniqueIter;

	struct RenderTargetChain
	{
		INT					m_nNext;
		RwRaster*			m_pRenderTarget[ NUM_PARAMS ];

		RenderTargetChain					( VOID );
		~RenderTargetChain					( VOID );

		bool				Initialize		( INT width , INT height );
		VOID				Flip			( VOID );
		
		RwRaster*			TargetRaster	( VOID ) CONST;
		RwRaster*			SourceRaster	( VOID ) CONST;
	};

	struct AutoExposureControl
	{
		
		BOOL	m_bAutoExposureControl;				// automatic exposure control on/off
		float	m_fCurrentExposureLevel;			// current & target exposure level
		float	m_fTargetExposureLevel;
		float	m_fTargetUpdateInterval;			// interval between successive target exposure updates in seconds
		float	m_fFeedbackK;						// a feedback coefficient for exposure control

		AutoExposureControl( VOID )
			: m_bAutoExposureControl(TRUE), m_fCurrentExposureLevel(2.0f),
			m_fTargetExposureLevel(2.0f), m_fTargetUpdateInterval(0.1f),
			m_fFeedbackK(3.0f)
		{}
	};

public:
	AgcmPostFX							( VOID );
	virtual ~AgcmPostFX					( VOID );

	BOOL		OnAddModule				( VOID );
	BOOL		OnInit					( VOID );
	BOOL		OnIdle					( UINT32 ulClockCount );

	BOOL		OnDestroy				( VOID );

	VOID		OnCameraStateChange		( CAMERASTATECHANGETYPE ctype );

	// ON / OFF
	VOID		PostFX_ON				( VOID );
	VOID		PostFX_OFF				( BOOL doNotDestroyTargets = false );
	BOOL		IsFxOn					( VOID ) CONST						{ return m_bOn; }

	BOOL		isAutoExposureControlOn	( VOID ) { return m_autoExposure.m_bAutoExposureControl; }
	VOID		AutoExposureControlOn	( VOID ) { m_autoExposure.m_bAutoExposureControl = TRUE; }
	VOID		AutoExposureControlOff	( VOID );

	Technique*	CreateFX				( const CHAR* pInstanceName , ePipeLine ePipelineType , BOOL bTop = FALSE );	// Pipeline is set

	BOOL		DeleteFX				( Technique*&	pTechnique );
	BOOL		DeleteFX				( const CHAR* pInstanceName );

	VOID		AllDeleteFX				( VOID );

	BOOL		GetInstance				( CHAR* pipeline, int nMax ) const;					// Pipeline gets the order. Returns FALSE if the pipeline is active
	Technique*	GetInstance				( CONST CHAR*	pInstanceName );
	Technique*	GetInstance				( INT nIndex );

	INT			GetCount				( VOID );
	
	BOOL		SetSpeedScale			( FLOAT	fSpeedScale );
	BOOL		SetStart				( FLOAT fStart		);

	// This applies to the post FX chain.
	VOID		RenderPostFX			( VOID );
	
	// change the main camera's render target to the offscreen buffer.
	VOID		changeRenderTarget		( VOID );

	// d3d stuff
	VOID		OnLostDevice			( VOID );
	VOID		OnResetDevice			( VOID );

	// Set the radius and determined the effect that gives out the location
	BOOL		SetRadius				( Technique*	pTechnique		, FLOAT fRadius );
	BOOL		SetRadius				( const CHAR*	pInstanceName	, FLOAT fRadius );

	BOOL		SetCenter				( Technique*	pTechnique		, FLOAT fX , FLOAT fY );
	BOOL		SetCenter				( const CHAR*	pInstanceName	, FLOAT	fX , FLOAT fY );

	BOOL		SetCenter				( Technique*	pTechnique		, RpClump*		pClump	);
	BOOL		SetCenter				( Technique*	pTechnique		, RpAtomic*		pAtomic );

	// in order to save video memory when post fx is off
	VOID		createRenderTargets		( VOID );
	VOID		destroyRenderTargets	( VOID );
	bool		isReady					( VOID ) const { return m_pSceneRenderTarget != NULL; }

	// Target chain, the source re-render the frame buffer to store
	VOID		saveNextFrame			( VOID )		{ m_bSaveNextFrame = true; }
	// Back Buffer to save the current contents.
	VOID		saveImmediateFrame		( VOID );

	RwRaster*	getSavedFrame			( VOID ) const	{ return m_pTransitionSource; }

	// technique objects for post fx
	ListTechnique					m_listTechnic;

	// effects
	LPD3DXEFFECT					m_pd3dEffect;

protected:

	VOID							_ControlExposure		( VOID );	// apply the feedback loop between current & target luminances.
	VOID							_AverageScene			( VOID );	// get the 1x1 average image of the original scene.
	VOID							_AutoNameGenerate		( CHAR*	pOutName );
	
	// camera raster for post-processing
	RwRaster*						m_pSceneRenderTarget;
	RwRaster*						m_pOriginalRenderTarget;
	
	RenderTargetChain				m_RTChain;					// render target chain
	
	BOOL							m_bOn;						// PostFX module on/off
	
	BOOL							m_bValid;					// all hardware requirements for the fx are satisfied?
	
	D3DXHANDLE						m_hTexScene;				// handle to the saved scene texture
	D3DXHANDLE						m_hTexPrev;					// handle to the previous result
	
	vectorDXTexture					m_textureLoaded;		// list of resource loaded textures

	mapTechHandle					m_mapTechHandle;
	
	D3DXHANDLE						m_hWindowSize;				// handle to the window size parameter	
	D3DXHANDLE						m_hElapsedTime;				// handle to the elapsed time in seconds

	
	LPDIRECT3DVERTEXDECLARATION9	m_pVertexDecl;				// vertex declaration for post fx
	LPDIRECT3DVERTEXBUFFER9			m_pVertexBuffer;			// vertex buffer for post fx

	AutoExposureControl				m_autoExposure;
	
	RwRaster*						m_pAverageRenderTarget[4];	// 64x64, 16x16, 4x4, 1x1 render targets for scene averaging
	Technique*						m_pAveragingTechnic;		// pointer to the averaging technique
	Technique*						m_pExposureControlTechnic;	// pointer to the exposure control technique
	D3DXHANDLE						m_hTexCurrentExposure;		// handle to the current exposure texture

	RenderTargetChain				m_lumFeedbackRTChain;		// render targets for the luminance feedback control

	// radius & center for circle based effects
	D3DXHANDLE						m_hRadius;
	D3DXHANDLE						m_hCenter;

	// for dependent reads on ps 1.1 ~ 1.3
	RwRaster*						m_pSquareRenderTarget;
	D3DXHANDLE						m_hTexSquare;
	D3DXHANDLE						m_hSquareRatio;

	// to prevent z-related bugs of some graphics card in low-res window mode.
	RwRaster*						m_pZBuffer;
	RwRaster*						m_pOriginalZBuffer;
	
	bool							m_bDoNotCreate;				// in order to save video memory when post fx is off
	bool							m_bSaveNextFrame;			// Save the back buffer of the next frame to the render target chain source.
	
	D3DXHANDLE						m_hTexTransSource;			// Handle to the transition source texture
	RwRaster*						m_pTransitionSource;		// Source raster for transition fx

	AgcmRender*						m_pAgcmRender;

	UINT							m_nCreateCount;
};

#endif