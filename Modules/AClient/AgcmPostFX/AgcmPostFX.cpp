//@{ Jaewon 20040730
// created
// Image-space post-processing effects module.
//@} Jaewon

#include <AgcmPostFX/AgcmPostFX.h>
#include <AgcmUIConsole/AgcmUIConsole.h>
#include <AgcmRender/AgcmRender.h>
#include <ctime>
#include <stdlib.h>
#include <rtfsyst.h>

#include <AuMD5EnCrypt/AuMD5EnCrypt.h>

#include <ApMemoryTracker/ApMemoryTracker.h>
#include <AgcModule/AgcEngine.h>

#ifdef _AREA_CHINA_
#define HASH_KEY_STRING "Protect_No_666_HevensDoor"
#else
#define HASH_KEY_STRING "1111"
#endif

// StretchRect can be used to support AA in post fx.
#define POSTFX_USE_STRETCH_RECT

#define max(a,b)    (((a) > (b)) ? (a) : (b))
#define min(a,b)    (((a) < (b)) ? (a) : (b))

// d3d device lost & reset
static AgcmPostFX*					_pThis					= NULL;
static rwD3D9DeviceRestoreCallBack	_oldD3D9RestoreDeviceCB = NULL;
static rwD3D9DeviceReleaseCallBack	_oldD3D9ReleaseDeviceCB = NULL;

static void newD3D9ReleaseDeviceCB( VOID )
{
	_pThis->OnLostDevice();

	if(_oldD3D9ReleaseDeviceCB)
		_oldD3D9ReleaseDeviceCB();
}
static void newD3D9RestoreDeviceCB( VOID )
{
	if(_oldD3D9RestoreDeviceCB)
		_oldD3D9RestoreDeviceCB();

	_pThis->OnResetDevice();
}

static CONST float _fMaxExposure = 3.5f;
static CONST float _fMinExposure = 1.5f;

// renderware hacks
static RwInt32 _RwD3D9RasterExtOffset = 0;

#define RASTEREXTFROMRASTER(raster) \
	((_rwD3D9RasterExt *)(((RwUInt8 *)(raster)) + _RwD3D9RasterExtOffset))

struct _rwD3D9Palette
{
	PALETTEENTRY    entries[256];
	RwInt32			globalindex;
};

struct _rwD3D9RasterExt
{
	LPDIRECT3DTEXTURE9      texture;
	_rwD3D9Palette          *palette;
	RwUInt8                 alpha;              /* This texture has alpha */
	RwUInt8                 cube : 4;           /* This texture is a cube texture */
	RwUInt8                 face : 4;           /* The active face of a cube texture */
	RwUInt8                 automipmapgen : 4;  /* This texture uses automipmap generation */
	RwUInt8                 compressed : 4;     /* This texture is compressed */
	RwUInt8                 lockedMipLevel;
	LPDIRECT3DSURFACE9      lockedSurface;
	D3DLOCKED_RECT          lockedRect;
	D3DFORMAT               d3dFormat;          /* D3D format */
	LPDIRECT3DSWAPCHAIN9    swapChain;
	HWND                    window;
};

extern "C"
{
	extern LPDIRECT3DSURFACE9           _RwD3D9RenderSurface;

	extern RwBool
		_rwD3D9SetRenderTarget(RwUInt32 index,
		LPDIRECT3DSURFACE9 rendertarget);
}

// vertex format for rendering a quad during post-process
struct PFXVERTEX
{
	float x, y, z;
	float u, v;

	CONST static D3DVERTEXELEMENT9 Decl[3];
};

// vertex declaration for post-processing
CONST D3DVERTEXELEMENT9 PFXVERTEX::Decl[] =
{
	{ 0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
	{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
	D3DDECL_END()
};

Technique::Technique()
:	m_hTechnique(NULL),
m_bDepedentRead(NULL)

{
	ZeroMemory( m_name			, sizeof( m_name		)	);
	ZeroMemory( m_szParamName	, sizeof( m_szParamName )	);
	ZeroMemory( m_hParam		, sizeof( m_hParam		)	);
	ZeroMemory( m_nParamSize	, sizeof( m_nParamSize	)	);
	ZeroMemory( m_vParamDef		, sizeof( m_vParamDef	)	);
	ZeroMemory( m_vParam		, sizeof( m_vParam		)	);

	ZeroMemory( &m_vCenter , sizeof(m_vCenter) );

	SetStartTime	( 0	);
	SetPlayTime		( 0	);
	SetSpeedScale	( 0.0f	);
	SetRadius		( 0.0f	);
	SetUpdate		( FALSE );

}

Technique::~Technique()
{
}

BOOL Technique::Initialize(LPD3DXEFFECT pFX, D3DXHANDLE hTechnique)
{
	// get the post fx technique handle.
	m_hTechnique = hTechnique;

	// get the technique description.
	D3DXTECHNIQUE_DESC pDesc;
	if( FAILED(pFX->GetTechniqueDesc(m_hTechnique, &pDesc)) )
		return FALSE;

	strcpy(m_name, pDesc.Name);

	CHAR buf[MAX_PATH];

	// obtain the handles to the changeable parameters, if any.
	for(int i=0; i<NUM_PARAMS; ++i)
	{
		sprintf(buf, "Parameter%d", i);
		D3DXHANDLE hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		LPCSTR szParamName;
		if(hAnno && SUCCEEDED(pFX->GetString(hAnno, &szParamName)))
		{
			m_hParam[i] = pFX->GetParameterByName(NULL, szParamName);
			strcpy(m_szParamName[i], szParamName);
		}

		// get the parameter size.
		sprintf(buf, "Parameter%dSize", i);
		hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		if(hAnno)
			pFX->GetInt(hAnno, &m_nParamSize[i]);

		// get the parameter default.
		sprintf(buf, "Parameter%dDef", i);
		hAnno = pFX->GetAnnotationByName(m_hTechnique, buf);
		if(hAnno)
			pFX->GetVector(hAnno, &m_vParamDef[i]);
	}

	// check the dependent read.
	D3DXHANDLE hAnno = pFX->GetAnnotationByName(m_hTechnique, "bDependentRead");
	BOOL bDependentRead;
	if(hAnno && SUCCEEDED(pFX->GetBool(hAnno, &bDependentRead)))
	{
		if(bDependentRead)
		{
			m_bDepedentRead = pFX->GetTechniqueByName("Copy");
			ASSERT(m_bDepedentRead);
		}
		else
			m_bDepedentRead = NULL;
	}

	return TRUE;

}

AgcmPostFX::RenderTargetChain::RenderTargetChain()
: m_nNext(0)
{
	memset(m_pRenderTarget, 0, sizeof(m_pRenderTarget));
}

AgcmPostFX::RenderTargetChain::~RenderTargetChain()
{
	for( INT i = 0 ; i < 2 ; ++i )
	{
		if( m_pRenderTarget[i] )
		{
			RwRasterDestroy( m_pRenderTarget[i] );
			m_pRenderTarget[i] = NULL;
		}
	}
}

bool AgcmPostFX::RenderTargetChain::Initialize(int w, int h)
{

	for(int i=0; i<2; ++i)
	{
		m_pRenderTarget[i] = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE | rwRASTERFORMAT8888);
		if(m_pRenderTarget[i] == NULL)
			return false;

		UINT8 *pPixel = RwRasterLock( m_pRenderTarget[i] , 0 , rwRASTERLOCKWRITE | rwRASTERLOCKNOFETCH );
		if (!pPixel)
			continue;
		// 0.4f
		*(pPixel+0) = UINT8(255.0f*0.4f);
		*(pPixel+1) = *(pPixel+0);
		*(pPixel+2) = *(pPixel+0);
		*(pPixel+3) = *(pPixel+0);

		RwRasterUnlock( m_pRenderTarget[i] );

	}

	return true;
}

void AgcmPostFX::RenderTargetChain::Flip()
{
	m_nNext = 1 - m_nNext;
}

RwRaster* AgcmPostFX::RenderTargetChain::TargetRaster() CONST
{
	return m_pRenderTarget[m_nNext];
}

RwRaster* AgcmPostFX::RenderTargetChain::SourceRaster() CONST
{
	return m_pRenderTarget[1-m_nNext];
}

AgcmPostFX::AgcmPostFX( VOID )
: m_pSceneRenderTarget(NULL), m_pOriginalRenderTarget(NULL),
m_bOn(FALSE), m_pd3dEffect(NULL), m_hTexScene(NULL),
m_pVertexDecl(NULL), m_pVertexBuffer(NULL), m_bValid(FALSE),
m_hWindowSize(NULL), m_hElapsedTime(NULL),
m_pAveragingTechnic(NULL), m_pExposureControlTechnic(NULL),
m_hTexCurrentExposure(NULL), m_hTexPrev(NULL),
m_hRadius(NULL), m_hCenter(NULL),
m_pSquareRenderTarget(NULL), m_hTexSquare(NULL),
m_hSquareRatio(NULL),
m_pZBuffer(NULL), m_pOriginalZBuffer(NULL),
m_bDoNotCreate(true),
m_bSaveNextFrame(false), m_hTexTransSource(NULL),
m_pTransitionSource(NULL),
m_pAgcmRender(NULL),
m_nCreateCount(0)
{
	memset(m_pAverageRenderTarget, 0, sizeof(m_pAverageRenderTarget));

	SetModuleName("AgcmPostFX");
	EnableIdle( TRUE );

	// for static callbacks
	_pThis = this;
}

AgcmPostFX::~AgcmPostFX( VOID )
{
	OnDestroy();
}


BOOL AgcmPostFX::OnAddModule( VOID )
{
	_RwD3D9RasterExtOffset = RwRasterGetPluginOffset(rwID_DEVICEMODULE);

	// save original device lost & reset callbacks.
	_oldD3D9ReleaseDeviceCB = _rwD3D9DeviceGetReleaseCallback();
	_oldD3D9RestoreDeviceCB = _rwD3D9DeviceGetRestoreCallback();

	// set new callbacks.
	_rwD3D9DeviceSetReleaseCallback(newD3D9ReleaseDeviceCB);
	_rwD3D9DeviceSetRestoreCallback(newD3D9RestoreDeviceCB);

	return TRUE;
}

void AgcmPostFX::PostFX_ON( VOID ) 
{ 
	if(m_bValid && !m_bOn)
	{
		// if not already created, create offscreen buffers.
		if(isReady() == false)
			createRenderTargets();

		// createRenderTargets() might fail, so...
		if(!m_bValid)
		{
			ASSERT(0);
			return;
		}

		// set the raster of the main camera to m_pSceneRenderTarget.
		RwCamera* pMainCam = GetCamera();
		if(pMainCam == NULL)
		{
			ASSERT(0);
			return;
		}
		m_pOriginalRenderTarget = RwCameraGetRaster		( pMainCam );
		m_pOriginalZBuffer		= RwCameraGetZRaster	( pMainCam );
		if(m_pSceneRenderTarget == NULL)
		{
			ASSERT(0);
			return;
		}
#ifndef POSTFX_USE_STRETCH_RECT
		if(m_pZBuffer == NULL)
		{
			ASSERT(0);
			return;
		}
		RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);
		RwCameraSetZRaster(pMainCam, m_pZBuffer);
#endif

		m_bOn = TRUE; 

	}
}

void AgcmPostFX::PostFX_OFF( BOOL doNotDestroyTargets )
{ 
	if(m_bValid && m_bOn)
	{
		// restore the raster of the main camera.
		RwCamera* pMainCam = GetCamera();
		if(pMainCam == NULL)
			return;

		RwRaster* pRaster = RwCameraGetRaster(pMainCam);
		RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);
		RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);

		m_pOriginalRenderTarget = NULL;
		m_pOriginalZBuffer		= NULL;
		m_bOn					= FALSE; 

		if( !doNotDestroyTargets && isReady() == true )
			destroyRenderTargets();
	}
}

BOOL AgcmPostFX::OnInit()
{
	m_pAgcmRender = (AgcmRender*)GetModule("AgcmRender");
	if(!m_pAgcmRender)
		return FALSE;

	AS_REGISTER_TYPE_BEGIN(AgcmPostFX, postFX);
	AS_REGISTER_METHOD0(void, PostFX_ON);
	AS_REGISTER_METHOD1(void, PostFX_OFF, bool);
	AS_REGISTER_METHOD0(void, saveNextFrame);
	AS_REGISTER_VARIABLE(int, m_bOn);
	AS_REGISTER_TYPE_END;

	LPD3DXBUFFER	pErrorMsgs	= NULL;
	HRESULT			hr;

	// macro definition added.
	D3DXMACRO		macro[] =	{	{ "ARCHLORD", "1" },
	{ NULL, NULL }			};

	if(RwFexist("effect/post.txt"))
	{
		FILE *pFile = fopen("effect/post.txt", "rb");
		ASSERT(pFile);

		fseek(pFile, 0, SEEK_END);	
		long fileSize = ftell(pFile);		
		fseek(pFile, 0, SEEK_SET);

		CHAR *buffer = new CHAR[fileSize];
		memset(buffer, 0, fileSize);
		size_t readBytes = fread(buffer, sizeof(CHAR), fileSize, pFile);
		ASSERT(readBytes == fileSize);
		fclose(pFile);

		// decrypt the file.
		AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
		bool bOK = MD5.DecryptString(MD5_HASH_KEY_STRING, buffer, fileSize);
#else
		bool bOK = MD5.DecryptString(HASH_KEY_STRING, buffer, fileSize);
#endif
		if(bOK == false)
		{
			DEF_SAFEDELETEARRAY( buffer );
		}

		else
		{
			// create a effect from the decrypted buffer.
			hr = D3DXCreateEffect(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()),
				buffer, fileSize,
				macro, NULL,
				0,//D3DXSHADER_SKIPVALIDATION,
				NULL,
				&m_pd3dEffect,
				&pErrorMsgs	);

			DEF_SAFEDELETEARRAY( buffer );
		}
	}

	else if(RwFexist("effect/post.fx"))
	{
		hr = D3DXCreateEffectFromFile(((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice()),
			"effect/post.fx",
			macro, NULL,
			0,//D3DXSHADER_SKIPVALIDATION,
			NULL,
			&m_pd3dEffect,
			&pErrorMsgs);
	}
	else
	{
		m_pd3dEffect = NULL;
		return TRUE;
	}

	if(FAILED(hr))
	{
		if(pErrorMsgs)
		{
			CONST CHAR* pLog = (CONST CHAR*)pErrorMsgs->GetBufferPointer();
			OutputDebugString(pLog);
			CHAR buf[1024];
			sprintf(buf, "FX compile error : \"%s\", so post fx will be disabled.", pLog);

			pErrorMsgs->Release();
		}

		m_pd3dEffect = NULL;
		return TRUE;
	}

	else if(pErrorMsgs)
		pErrorMsgs->Release();

	// obtain the handle to the saved scene texture.
	m_hTexScene			= m_pd3dEffect->GetParameterByName(NULL, "g_txScene");
	m_hTexSquare		= m_pd3dEffect->GetParameterByName(NULL, "g_txSquare");
	m_hSquareRatio		= m_pd3dEffect->GetParameterByName(NULL, "g_squareRatio");
	m_hTexPrev			= m_pd3dEffect->GetParameterByName(NULL, "g_txPrev");
	m_hCenter			= m_pd3dEffect->GetParameterByName(NULL, "g_center");
	m_hRadius			= m_pd3dEffect->GetParameterByName(NULL, "g_radius");
	m_hTexTransSource	= m_pd3dEffect->GetParameterByName(NULL, "g_txTransSource");
	m_hWindowSize		= m_pd3dEffect->GetParameterByName(NULL, "g_windowSize");
	m_hElapsedTime		= m_pd3dEffect->GetParameterByName(NULL, "g_fElapsedTime");

	m_pd3dEffect->SetFloat(m_hElapsedTime, 0.0f);

	// get all techniques in the fx.
	D3DXHANDLE hTechnic;
	for( INT i = 0 ; hTechnic = m_pd3dEffect->GetTechnique(i) ; ++i )
	{
		m_mapTechHandle.insert( make_pair( (ePipeLine)i , hTechnic) );
	}

	// Average , ControlExposure
	m_pAveragingTechnic			=	CreateFX( POSTFX_NAME_GENERATE , E_PIPELINE_AVERAGE );
	m_pExposureControlTechnic	=	CreateFX( POSTFX_NAME_GENERATE , E_PIPELINE_CONTROLEXPOSURE );
	if (!m_pAveragingTechnic || !m_pExposureControlTechnic) {
		ASSERT( 0 || "Average , ControlExposure Error" );
	}

	m_listTechnic.remove( m_pAveragingTechnic );
	m_listTechnic.remove( m_pExposureControlTechnic );

	// create a vertex declaration and a vertex buffer.
	if(RwD3D9CreateVertexDeclaration(PFXVERTEX::Decl, (void**)&m_pVertexDecl) == FALSE)
	{
		m_pVertexDecl = NULL;
		return TRUE;
	}
	UINT32 offset = 0;
	if(RwD3D9CreateVertexBuffer(sizeof(PFXVERTEX), sizeof(PFXVERTEX)*4, (void**)&m_pVertexBuffer, &offset) == FALSE)
	{
		m_pVertexBuffer = NULL;
		return TRUE;
	}

	// fill in the vertex buffer.
	PFXVERTEX Quad[4] =
	{
		{ -1.0f,  1.0f, 0.5f, 0.0f, 0.0f },
		{  1.0f,  1.0f, 0.5f, 1.0f, 0.0f },
		{ -1.0f, -1.0f, 0.5f, 0.0f, 1.0f },
		{  1.0f, -1.0f, 0.5f, 1.0f, 1.0f }
	};

	LPVOID pVBData	=	NULL;
	if(SUCCEEDED(m_pVertexBuffer->Lock(0, 0, &pVBData, 0)))
	{
		memcpy(pVBData, Quad, sizeof(Quad));
		m_pVertexBuffer->Unlock();
	}

	else
		return TRUE;

	m_hTexCurrentExposure = m_pd3dEffect->GetParameterByName(NULL, "g_txCurrentExposure");

	D3DXEFFECT_DESC EffectDesc;
	m_pd3dEffect->GetDesc(&EffectDesc);

	for( INT i = 0 ; i != EffectDesc.Parameters ; ++i )
	{
		D3DXPARAMETER_DESC desc;
		D3DXHANDLE handle;
		D3DXHANDLE annot;

		handle = m_pd3dEffect->GetParameter(NULL, i);
		m_pd3dEffect->GetParameterDesc(handle, &desc);

		// if it is a scene texture, just skip it.
		if(handle == m_hTexScene)
			continue;

		if(desc.Class == D3DXPC_OBJECT
			&& desc.Type == D3DXPT_TEXTURE)
			// if it is a texture parameter,
		{
			// get the filename for the texture.
			annot = m_pd3dEffect->GetAnnotationByName(handle, "ResourceName");
			if(NULL == annot)
				continue;

			CONST CHAR *file;
			hr = m_pd3dEffect->GetString(annot, &file);
			ASSERT(SUCCEEDED(hr));

			CHAR texName[MAX_PATH];
			// Etc -> NotPacked
			strcpy(texName, "Texture\\NotPacked\\");
			strcat(texName, file);

			// get the texture type.
			int		ttype	= 0; // 0 - 1D,2D / 1 - Cube / 2 - Volume
			annot			= m_pd3dEffect->GetAnnotationByName( handle, "TextureType" );
			if(annot)
			{
				CONST CHAR *textureType;
				hr = m_pd3dEffect->GetString(annot, &textureType);
				ASSERT(SUCCEEDED(hr));

				if ( 0 == stricmp(textureType, "volume") )
					ttype = 2;
				else if( 0 == stricmp(textureType, "cube") )
					ttype = 1;
				else
					ttype = 0;
			}

			LPDIRECT3DDEVICE9			pd3dDevice = ( (LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice() );
			LPDIRECT3DBASETEXTURE9		texture;
			HRESULT						hr;

			if		( ttype == 2 )
				hr = D3DXCreateVolumeTextureFromFile(pd3dDevice, texName, (LPDIRECT3DVOLUMETEXTURE9 *)&texture);

			else if	( ttype == 1 )
				hr = D3DXCreateCubeTextureFromFile	(pd3dDevice, texName, (LPDIRECT3DCUBETEXTURE9 *)&texture);

			else
				hr = D3DXCreateTextureFromFile		(pd3dDevice, texName, (LPDIRECT3DTEXTURE9 *)&texture);

			if(SUCCEEDED(hr))
			{
				m_pd3dEffect->SetTexture(handle, texture);
				m_textureLoaded.push_back(texture);
			}
		}
	}

	return TRUE;
}

BOOL	AgcmPostFX::OnIdle( UINT32 ulClockCount )
{
	FLOAT				fElapsed	=	0;
	ListTechniqueIter	Iter		=	m_listTechnic.begin();

	while( Iter != m_listTechnic.end() )
	{
		Technique*	pTech	=	(*Iter);

		// 객체가 없다.. 유령.. 그냥 지운다
		if( !pTech )
		{
			m_listTechnic.erase( Iter++ );
			continue;
		}

		// Update가 필요없는 FX거나Update 자체적으로 함.
		// 지우는것도 자체적으로 함.
		if( !pTech->m_bUpdate )
		{
			++Iter;
			continue;
		}

		fElapsed	=	(ulClockCount - pTech->m_dwStartTime) / 1000.0f; 

		// Play할 시간이 남았으면 계속 업데이트
		if( ulClockCount < pTech->m_dwStartTime + pTech->m_dwPlayTime )
		{
			pTech->SetRadius( fElapsed * pTech->m_fSpeedScale );
		}

		// Play할 시간이 끝났다. 지운다
		else
		{
			m_listTechnic.erase( Iter++ );
			continue;
		}

		++Iter;	
	}	

	return TRUE;
}

BOOL AgcmPostFX::OnDestroy()
{
	// in order to be safe
	PostFX_OFF();

	m_hTexScene				= NULL;
	m_hWindowSize			= NULL;
	m_hElapsedTime			= NULL;
	m_hTexCurrentExposure	= NULL;
	m_hTexPrev				= NULL;
	m_hRadius				= NULL;
	m_hCenter				= NULL;
	m_hTexSquare			= NULL;
	m_hSquareRatio			= NULL;
	m_hTexTransSource		= NULL;

	if(m_pSceneRenderTarget)
		destroyRenderTargets();

	// release loaded textures.
	UINT32 i;
	for(i=0; i<m_textureLoaded.size(); ++i)
	{
		m_textureLoaded[i]->Release();
	}
	m_textureLoaded.clear();

	// destroy the vertex declaration and the vertex buffer.
	if(m_pVertexDecl)
	{
		RwD3D9DeleteVertexDeclaration(m_pVertexDecl);
		m_pVertexDecl = NULL;
	}
	if(m_pVertexBuffer)
	{
		RwD3D9DestroyVertexBuffer(sizeof(PFXVERTEX), sizeof(PFXVERTEX)*4, m_pVertexBuffer, 0);
		m_pVertexBuffer = NULL;
	}

	// clear all techniques.

	ListTechniqueIter	Iter		=	m_listTechnic.begin();
	for(  ; Iter != m_listTechnic.end() ;  )
	{
		DEF_SAFEDELETE( (*Iter) );
	}

	m_listTechnic.clear();
	m_pAveragingTechnic		= NULL;

	// destroy the fx.
	if(m_pd3dEffect)
	{
		m_pd3dEffect->Release();
		m_pd3dEffect = NULL;
	}

	return TRUE;
}

void AgcmPostFX::OnLostDevice()
{	
	if(m_pd3dEffect==NULL) 
		return;

	m_pd3dEffect->OnLostDevice();

	if(m_pSceneRenderTarget)
	{
		destroyRenderTargets();
		m_bDoNotCreate = false;
	}

	else
		m_bDoNotCreate = true;
}

void AgcmPostFX::OnResetDevice()
{
	if(m_pd3dEffect==NULL) 
		return;

	m_pd3dEffect->OnResetDevice();

	if(m_bDoNotCreate == false)
		createRenderTargets();
}

Technique*	AgcmPostFX::GetInstance( CONST CHAR* pInstanceName )
{
	if( pInstanceName )
	{
		ListTechniqueIter	Iter	=	m_listTechnic.begin();
		while( Iter != m_listTechnic.end() )
		{
			if( strcmp( (*Iter)->m_InstanceName , pInstanceName ) == 0 )
			{
				return (*Iter);
			}

			++Iter;
		}
	}

	return NULL;
}

Technique*	AgcmPostFX::GetInstance( INT nIndex )
{
	ListTechniqueIter	Iter	=	m_listTechnic.begin();
	for( INT i = 0 ; Iter != m_listTechnic.end() ; ++i )
	{
		if( i == nIndex )
			return  (*Iter);
	}

	return NULL;
}

Technique* AgcmPostFX::CreateFX( const CHAR* pInstanceName , ePipeLine ePipelineType , BOOL bTop )
{
	Technique*	pTechnique	=	GetInstance( pInstanceName );

	if( pTechnique )
	{
		return pTechnique;
	}

	m_bValid		=	TRUE;
	pTechnique		=	new Technique;
	++m_nCreateCount;

	// 지정된 이름 사용
	if( pInstanceName && strcmp( pInstanceName , POSTFX_NAME_GENERATE ) != 0 )
		sprintf_s( pTechnique->m_InstanceName ,  TECHNIQUENAME_MAXLENGTH , "%s" , pInstanceName );

	// 이름 자동 생성
	else
		_AutoNameGenerate( pTechnique->m_InstanceName );

	mapTechHandleIter	Iter	=	m_mapTechHandle.find( ePipelineType );
	if( Iter == m_mapTechHandle.end())
	{
		DEF_SAFEDELETE( pTechnique );
		return NULL;
	}

	pTechnique->Initialize( m_pd3dEffect , Iter->second );

	if( bTop )
		m_listTechnic.push_front( pTechnique );

	else
		m_listTechnic.push_back( pTechnique );

	pTechnique->SetStartTime( GetClockCount() );

	return pTechnique;
}

BOOL	AgcmPostFX::DeleteFX( const CHAR* pInstanceName )
{
	Technique*	pTechnique	=	GetInstance( pInstanceName );
	return DeleteFX( pTechnique );
}

BOOL	AgcmPostFX::DeleteFX( Technique*& pTechnique )
{
	if( !pTechnique )
		return FALSE;

	m_listTechnic.remove( pTechnique );
	DEF_SAFEDELETE( pTechnique );

	if( !m_listTechnic.size() )
		m_bValid	=	FALSE;

	return TRUE;
}

VOID	AgcmPostFX::AllDeleteFX( VOID )
{
	ListTechniqueIter	Iter	=	m_listTechnic.begin();
	for( ; Iter != m_listTechnic.end() ; ++Iter )
	{
		DEF_SAFEDELETE( (*Iter) );
	}
	m_listTechnic.clear();
	m_bValid	=	FALSE;
}

void AgcmPostFX::RenderPostFX()
{
	LPDIRECT3DDEVICE9	pd3dDevice		= ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	RwCamera*			pMainCam		= GetCamera();

	if( !pd3dDevice || !pMainCam )
		return;

	if(!m_bOn && m_bSaveNextFrame && m_pTransitionSource )
	{
		RwCameraEndUpdate(pMainCam);
		pd3dDevice->EndScene();

		LPDIRECT3DSURFACE9	pSourceSurface;
		_rwD3D9RasterExt*	rasExt			= RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
		HRESULT				hr				= rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);

		hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
		pSourceSurface->Release();
		pd3dDevice->BeginScene();

		LockFrame();
		RwCameraBeginUpdate( pMainCam );
		RwD3D9SetRenderState( D3DRS_FOGEND,  *((DWORD*)&(m_pAgcmRender->m_fFogEnd)) ); 
		UnlockFrame();

		m_bSaveNextFrame = false;
		return;
	}

	static clock_t		prev	= clock();
	clock_t				now		= clock();
	float				dt		= float(now - prev) / CLOCKS_PER_SEC;
	prev = now;

	if( !m_bOn )
		return;

	if( m_hElapsedTime )
		m_pd3dEffect->SetFloat(m_hElapsedTime, dt);

	RwCameraEndUpdate(pMainCam);
	pd3dDevice->EndScene();

#ifdef POSTFX_USE_STRETCH_RECT
	{
		// copy the back buffer to m_pSceneRenderTarget.
		LPDIRECT3DSURFACE9	pSceneSurface;
		_rwD3D9RasterExt*	rasExt			= RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
		HRESULT				hr				= rasExt->texture->GetSurfaceLevel(0, &pSceneSurface);
		ASSERT(hr==D3D_OK);

		pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSceneSurface, NULL, D3DTEXF_NONE);
		pSceneSurface->Release();
	}
#endif

	// automatic exposure control
	if(m_autoExposure.m_bAutoExposureControl)
		_ControlExposure();

	// if the specified time has passed, update the target exposure level.
	static clock_t	prevAve = clock();
	clock_t			nowAve	= clock();
	float			dtAve	= float(nowAve - prevAve) / CLOCKS_PER_SEC;
	if(dtAve > m_autoExposure.m_fTargetUpdateInterval)
	{
		if(m_autoExposure.m_bAutoExposureControl)
		{
			_AverageScene();
		}
		prevAve += ( clock_t ) m_autoExposure.m_fTargetUpdateInterval;

	}

	ListTechniqueIter	Iter	=	m_listTechnic.begin();
	BOOL				bFirst	=	TRUE;

	while( Iter != m_listTechnic.end() )
		// for each technique instance
	{
		Technique*			pT	= (*Iter);

		if( !pT )
		{
			m_listTechnic.erase( Iter++ );
			continue;
		}

		// if there are any parameters, initialize them here.
		for(int j=0; j<NUM_PARAMS; ++j)
		{
			if(pT->m_hParam[j])
				m_pd3dEffect->SetVector(pT->m_hParam[j], &(pT->m_vParam[j]));
		}

		m_pd3dEffect->SetFloat	( m_hRadius	,	pT->m_fRadius );
		m_pd3dEffect->SetVector	( m_hCenter	,	&pT->m_vCenter );

		// render the quad.
		if(SUCCEEDED(pd3dDevice->BeginScene()))
		{
			if(pT->m_bDepedentRead)
			{
				if(!m_pSquareRenderTarget)
				{
					pd3dDevice->EndScene();
					continue;
				}

				m_pd3dEffect->SetTechnique(pT->m_bDepedentRead);
				RwD3D9SetVertexDeclaration(m_pVertexDecl);
				UINT32 cPasses;
				m_pd3dEffect->Begin(&cPasses, 0);

				if( bFirst )
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}
				else
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_RTChain.SourceRaster());
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}

				RwD3D9SetRenderTarget(0, m_pSquareRenderTarget);
				D3DVIEWPORT9 vp;
				pd3dDevice->GetViewport(&vp);
				vp.Width = RwRasterGetWidth(m_pSceneRenderTarget); 
				vp.Height = RwRasterGetHeight(m_pSceneRenderTarget);
				pd3dDevice->SetViewport(&vp);
				m_pd3dEffect->BeginPass(0);
				pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				m_pd3dEffect->EndPass();
				m_pd3dEffect->End();
			}

			m_pd3dEffect->SetTechnique(pT->m_hTechnique);

			// set the vertex declaration.
			RwD3D9SetVertexDeclaration(m_pVertexDecl);

			// draw the quad.
			UINT32 cPasses;
			m_pd3dEffect->Begin(&cPasses, 0);
			for(UINT32 p=0; p<cPasses; ++p)
			{

				if( bFirst )
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}
				else
				{
					_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_RTChain.SourceRaster());
					m_pd3dEffect->SetTexture(m_hTexPrev, rasExt->texture);
				}

				// 마지막이라면
				if( (*Iter) ==	(*m_listTechnic.rbegin()) )
					_rwD3D9SetRenderTarget(0, _RwD3D9RenderSurface);

				// 계속 남았다면 렌더타겟을 바꾼다
				else
					RwD3D9SetRenderTarget(0, m_RTChain.TargetRaster());

				// render.
				m_pd3dEffect->BeginPass(p);
				pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
				pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
				m_pd3dEffect->EndPass();

				m_RTChain.Flip();
			}

			m_pd3dEffect->End();
			pd3dDevice->EndScene();

		}

		bFirst	=	FALSE;
		++Iter;
	}

	// If m_bSaveNextFrame is true, save the back buffer 
	// to the transition source and return.
	if(m_bSaveNextFrame)
	{
		LPDIRECT3DSURFACE9	pSourceSurface;
		_rwD3D9RasterExt*	rasExt			= RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
		HRESULT				hr				= rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);

		if( SUCCEEDED( hr) )
		{
			hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
			pSourceSurface->Release();
			m_bSaveNextFrame = false;
		}
	}

	pd3dDevice->BeginScene();

#ifndef POSTFX_USE_STRETCH_RECT
	// now, restore the original render target.
	RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);

	RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);
#endif

	LockFrame();
	RwCameraBeginUpdate(pMainCam);
	RwD3D9SetRenderState(D3DRS_FOGEND,  *((DWORD*)&(m_pAgcmRender->m_fFogEnd))); 
	UnlockFrame();
}

void AgcmPostFX::changeRenderTarget()
{
	if( !m_bOn )
		return;

#ifndef POSTFX_USE_STRETCH_RECT
	RwCamera* pMainCam = GetCamera();
	RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);	
	RwCameraSetZRaster(pMainCam, m_pZBuffer);
#endif
}

void AgcmPostFX::OnCameraStateChange(CAMERASTATECHANGETYPE ctype)
{

	if(ctype == CSC_RESIZE && m_pSceneRenderTarget == NULL)
	{
		OnLostDevice();
		OnResetDevice();
	}

}

void AgcmPostFX::_AverageScene()
{
	if( !m_pAverageRenderTarget[3]  || !m_pAveragingTechnic )
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());

	// do scene averaging.
	if(SUCCEEDED(pd3dDevice->BeginScene()))
	{
		m_pd3dEffect->SetTechnique(m_pAveragingTechnic->m_hTechnique);

		// set the vertex declaration.
		RwD3D9SetVertexDeclaration(m_pVertexDecl);

		// draw the quad.
		UINT32 cPasses;
		m_pd3dEffect->Begin(&cPasses, 0);
		ASSERT(cPasses == 4);
		for(UINT32 p=0; p<cPasses; ++p)
		{
			RwD3D9SetRenderTarget(0, m_pAverageRenderTarget[p]);

			// render.
			m_pd3dEffect->BeginPass(p);
			pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
			pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
			m_pd3dEffect->EndPass();
		}
		m_pd3dEffect->End();

		pd3dDevice->EndScene();
	}
}

void AgcmPostFX::_ControlExposure( VOID )
{

	if(m_lumFeedbackRTChain.m_pRenderTarget[0] == NULL || m_pExposureControlTechnic == NULL)
		return;

	LPDIRECT3DDEVICE9 pd3dDevice = static_cast< LPDIRECT3DDEVICE9 >(RwD3D9GetCurrentD3DDevice());

	if(SUCCEEDED(pd3dDevice->BeginScene()))
	{
		m_pd3dEffect->SetTechnique(m_pExposureControlTechnic->m_hTechnique);

		// set the vertex declaration.
		RwD3D9SetVertexDeclaration(m_pVertexDecl);

		// draw the quad.
		UINT32 cPasses;
		m_pd3dEffect->Begin(&cPasses, 0);
		ASSERT(cPasses==1);
		RwD3D9SetRenderTarget(0, m_lumFeedbackRTChain.TargetRaster());

		// render.
		m_pd3dEffect->BeginPass(0);

		pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(PFXVERTEX));
		pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		m_pd3dEffect->EndPass();

		m_lumFeedbackRTChain.Flip();

		m_pd3dEffect->End();

		pd3dDevice->EndScene();
	}

	_rwD3D9RasterExt *rasExt = RASTEREXTFROMRASTER(m_lumFeedbackRTChain.SourceRaster());
	m_pd3dEffect->SetTexture(m_hTexCurrentExposure, rasExt->texture);

}

BOOL AgcmPostFX::SetRadius( Technique*	pTechnique		, FLOAT fRadius )
{
	if( !pTechnique )
		return FALSE;

	pTechnique->m_fRadius	=	fRadius;

	return TRUE;
}

BOOL AgcmPostFX::SetRadius( const CHAR*	pInstanceName	, FLOAT fRadius )
{
	Technique*	pTechnique	=	GetInstance( pInstanceName );
	return SetRadius( pTechnique , fRadius );
}


BOOL AgcmPostFX::SetCenter( Technique*	pTechnique , FLOAT fX , FLOAT fY )
{
	if( !pTechnique )
		return FALSE;

	pTechnique->m_vCenter		=	D3DXVECTOR4( fX , fY , 0 , 0 );
	return TRUE;
}

BOOL AgcmPostFX::SetCenter( const CHAR* pInstanceName , FLOAT fX , FLOAT fY )
{
	Technique*	pTechnique	=	GetInstance( pInstanceName );

	return SetCenter( pTechnique , fX , fY );
}

void AgcmPostFX::AutoExposureControlOff() 
{ 
	if(m_autoExposure.m_bAutoExposureControl)
	{
		if(m_lumFeedbackRTChain.SourceRaster())
		{
			// set the exposure level to a default.
			UINT8 *pPixel = RwRasterLock( m_lumFeedbackRTChain.SourceRaster() , 0 , rwRASTERLOCKWRITE | rwRASTERLOCKNOFETCH );

			*(pPixel+0) = UINT8(255.0f*0.4f);
			*(pPixel+1) = *(pPixel+0);
			*(pPixel+2) = *(pPixel+0);
			*(pPixel+3) = *(pPixel+0);

			RwRasterUnlock(m_lumFeedbackRTChain.SourceRaster());		
		}
		m_autoExposure.m_bAutoExposureControl = FALSE; 
	}
}

// in order to save video memory when post fx is off
void AgcmPostFX::createRenderTargets()
{
	if( !m_pd3dEffect )
	{
		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

	// all render target should be recreated.
	// if the post fx was enabled,
	// turn it on again with new targets.
	RwCamera* pMainCam	= GetCamera();
	RwRaster* pRaster	= RwCameraGetRaster(pMainCam);

	//. RwRaster* validation check
	if(!pRaster)
		return;

	INT		w	=	RwRasterGetWidth(pRaster);
	INT		h	=	RwRasterGetHeight(pRaster);

	// init the render target chain.
	if( m_RTChain.Initialize(w, h) == false )
	{
		m_RTChain.m_pRenderTarget[0] = NULL;
		m_RTChain.m_pRenderTarget[1] = NULL;

		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

	// create a render target for the main scene.
	m_pSceneRenderTarget = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888 );
	if(m_pSceneRenderTarget == NULL)
	{
		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

	UINT8 * pPixel = RwRasterLock( m_pSceneRenderTarget , 0 , rwRASTERLOCKWRITE || rwRASTERLOCKNOFETCH );
	if (pPixel) {
		*(pPixel+0) = UINT8(255.0f*0.4f);
		*(pPixel+1) = *(pPixel+0);
		*(pPixel+2) = *(pPixel+0);
		*(pPixel+3) = *(pPixel+0);
	}
	RwRasterUnlock( m_pSceneRenderTarget );

#ifndef POSTFX_USE_STRETCH_RECT
	// create a z buffer for the main scene.
	m_pZBuffer = RwRasterCreate(w, h, 0, rwRASTERTYPEZBUFFER);
	if(m_pZBuffer == NULL)
	{
		ASSERT(0);

		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

#endif
	// create render targets for scene averaging.
	INT	qdwSize	=	64;
	for( INT i = 0 ; i < 4 ; ++i , qdwSize/=4 ) {
		m_pAverageRenderTarget[i] = RwRasterCreate(qdwSize, qdwSize, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
		pPixel = RwRasterLock( m_pAverageRenderTarget[i] , 0 , rwRASTERLOCKWRITE || rwRASTERLOCKNOFETCH );
		if (pPixel) {
			*(pPixel+0) = UINT8(255.0f*0.4f);
			*(pPixel+1) = *(pPixel+0);
			*(pPixel+2) = *(pPixel+0);
			*(pPixel+3) = *(pPixel+0);
		}
		RwRasterUnlock( m_pAverageRenderTarget[i] );
	}

	// create render targets for the luminance feedback control.
	if(m_lumFeedbackRTChain.Initialize(1, 1) == false)
	{
		m_lumFeedbackRTChain.m_pRenderTarget[0] = NULL;
		m_lumFeedbackRTChain.m_pRenderTarget[1] = NULL;
	}
	// set the default luminance.
	pPixel = RwRasterLock(m_lumFeedbackRTChain.SourceRaster(), 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
	if (pPixel) {
		// 0.4f
		*(pPixel+0) = UINT8(255.0f*0.4f);
		*(pPixel+1) = *(pPixel+0);
		*(pPixel+2) = *(pPixel+0);
		*(pPixel+3) = *(pPixel+0);
	}
	RwRasterUnlock(m_lumFeedbackRTChain.SourceRaster());
	// create a square render target.
	INT		wPOW2=1, hPOW2=1;
	// 2 -> 1
	while(wPOW2 < w) wPOW2 <<= 1;
	while(hPOW2 < h) hPOW2 <<= 1;

	m_pSquareRenderTarget = RwRasterCreate(wPOW2, hPOW2, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	if(m_pSquareRenderTarget == NULL)
	{
		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

	pPixel = RwRasterLock(m_pSquareRenderTarget, 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
	if( pPixel ) {
		// 0.4f
		*(pPixel+0) = UINT8(255.0f*0.4f);
		*(pPixel+1) = *(pPixel+0);
		*(pPixel+2) = *(pPixel+0);
		*(pPixel+3) = *(pPixel+0);
	}
	RwRasterUnlock(m_pSquareRenderTarget);

	_rwD3D9RasterExt	*rasExt		=	RASTEREXTFROMRASTER(RwRasterGetParent(m_pSquareRenderTarget));
	m_pd3dEffect->SetTexture(m_hTexSquare, rasExt->texture);	

	// set the 'g_squareRatio' parameter.
	D3DXVECTOR4		squareRatio(float(w)/float(wPOW2), float(h)/float(hPOW2), 0, 0);
	HRESULT			hr;
	hr	=	 m_pd3dEffect->SetVector(m_hSquareRatio, &squareRatio);

	m_pTransitionSource = RwRasterCreate(w, h, 32, rwRASTERTYPECAMERATEXTURE|rwRASTERFORMAT8888);
	if( m_pTransitionSource == NULL )
	{
		PostFX_OFF();
		m_bValid = FALSE;
		return;
	}

	pPixel = RwRasterLock(m_pTransitionSource, 0, rwRASTERLOCKWRITE|rwRASTERLOCKNOFETCH);
	if (pPixel) {
		// 0.4f
		*(pPixel+0) = UINT8(255.0f*0.4f);
		*(pPixel+1) = *(pPixel+0);
		*(pPixel+2) = *(pPixel+0);
		*(pPixel+3) = *(pPixel+0);
	}
	RwRasterUnlock(m_pTransitionSource);

	// update global parameters.
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pSceneRenderTarget));
	m_pd3dEffect->SetTexture(m_hTexScene, rasExt->texture);

	D3DXVECTOR4 windowSize(float(w), float(h), 0, 0);
	m_pd3dEffect->SetVector(m_hWindowSize, &windowSize);

	rasExt = RASTEREXTFROMRASTER(m_lumFeedbackRTChain.SourceRaster());
	m_pd3dEffect->SetTexture(m_hTexCurrentExposure, rasExt->texture);

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[0]));
	hr = m_pd3dEffect->SetTexture("g_tx64x64", rasExt->texture);

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[1]));	
	hr = m_pd3dEffect->SetTexture("g_tx16x16", rasExt->texture);

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[2]));
	hr = m_pd3dEffect->SetTexture("g_tx4x4", rasExt->texture);

	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pAverageRenderTarget[3]));
	hr = m_pd3dEffect->SetTexture("g_txExposure", rasExt->texture);

	// We use m_pTransitionSource as a transition source texture.
	rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
	m_pd3dEffect->SetTexture(m_hTexTransSource, rasExt->texture);

	// restore 'on' settings.
	if( IsFxOn() )
	{
		m_pOriginalRenderTarget = pRaster;
#ifndef POSTFX_USE_STRETCH_RECT
		RwCameraSetRaster(pMainCam, m_pSceneRenderTarget);
#endif
		m_pOriginalZBuffer = RwCameraGetZRaster(pMainCam);
#ifndef POSTFX_USE_STRETCH_RECT
		RwCameraSetZRaster(pMainCam, m_pZBuffer);
#endif
	}

}

void AgcmPostFX::destroyRenderTargets()
{
	// all render target should be released.
	RwCamera* pMainCam = GetCamera();

	if( IsFxOn() && pMainCam )
	{
		// restore the original render target.
		if( m_pOriginalRenderTarget )
		{
			RwCameraSetRaster(pMainCam, m_pOriginalRenderTarget);
			m_pOriginalRenderTarget = NULL;
		}

		if( m_pOriginalZBuffer )
		{
			RwCameraSetZRaster(pMainCam, m_pOriginalZBuffer);
			m_pOriginalZBuffer = NULL;
		}
	}

	// destroy the main scene render target.
	if( m_pSceneRenderTarget )
	{
		RwRasterDestroy(m_pSceneRenderTarget);
		m_pSceneRenderTarget = NULL;
	}

	// destroy the z buffer.
	if( m_pZBuffer )
	{
		RwRasterDestroy(m_pZBuffer);
		m_pZBuffer = NULL;
	}

	// destroy the render target chain.
	m_RTChain.~RenderTargetChain();

	for( INT i = 0 ; i < 4 ; ++i )
	{
		if( m_pAverageRenderTarget[i] )
		{
			RwRasterDestroy( m_pAverageRenderTarget[i] );
			m_pAverageRenderTarget[i] = NULL;
		}
	}

	m_lumFeedbackRTChain.~RenderTargetChain();

	// destroy the square render target.
	if( m_pSquareRenderTarget )
	{
		RwRasterDestroy(m_pSquareRenderTarget);
		m_pSquareRenderTarget = NULL;
	}

	if( m_pTransitionSource )
	{
		RwRasterDestroy(m_pTransitionSource);
		m_pTransitionSource = NULL;
	}
}

// Save the current content of the back buffer.
void AgcmPostFX::saveImmediateFrame()
{
	if( !m_pTransitionSource )
		return;

	LPDIRECT3DSURFACE9		pSourceSurface;
	LPDIRECT3DDEVICE9		pd3dDevice		= ((LPDIRECT3DDEVICE9)RwD3D9GetCurrentD3DDevice());
	_rwD3D9RasterExt*		rasExt			= RASTEREXTFROMRASTER(RwRasterGetParent(m_pTransitionSource));
	HRESULT					hr				= rasExt->texture->GetSurfaceLevel(0, &pSourceSurface);
	ASSERT(hr==D3D_OK);
	hr = pd3dDevice->StretchRect(_RwD3D9RenderSurface, NULL, pSourceSurface, NULL, D3DTEXF_NONE);
	ASSERT(hr==D3D_OK);
	pSourceSurface->Release();
}


VOID			AgcmPostFX::_AutoNameGenerate( CHAR* pOutName )
{
	sprintf_s( pOutName , TECHNIQUENAME_MAXLENGTH , "PostFX_%d" , m_nCreateCount );
}


INT				AgcmPostFX::GetCount( VOID )
{
	return (INT)m_listTechnic.size();
}
