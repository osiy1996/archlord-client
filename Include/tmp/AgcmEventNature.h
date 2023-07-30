// AgcmEventNature.h: interface for the AgcmEventNature class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_)
#define AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "rtimport.h"
#include "ApModule.h"
#include "AgcModule.h"
#include "AgpmEventNature.h"
#include "AgcmRender.h"
#include "AgcmEff2.h"
#include "AuList.h"
#include "MagImAtomic.h"
#include "AgcmResourceLoader.h"
#include "AgcmMap.h"
#include "AgcmSound.h"
#include "AgcmNatureEffect.h"
//#include "AgpmEventTeleport.h"

#include <vector>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventNatureD" )
#else
#pragma comment ( lib , "AgcmEventNature" )
#endif
#endif

#define	AGCMEVENTNATURE_SCRIPT_FILE_NAME	"ini\\alefskysetting.ini"
// ��ũ��Ʈ���Ͽ��� ������ ���� ������ ����ȴ�.
// 1,���Ŵ� �ؽ��� ����Ʈ
// 2,�ð��뺰 ��� �ؽ��Ŀ� ����Ʈ ����.
// 3,���ĺ� ����. ( ������ �� , Ư��ȿ���� )
#define	AGCMEVENTNATURE_DEFAULT_BGM_DIR		"sound\\bgm"

// ��� �ɰ�����..
#define	AGCMEVENTNATURE_SKY_DETAIL			32
#define	AGCMEVENTNATURE_MAX_EVENT			5
#define SECTOR_MAX_NATURE_OBJECT_COUNT	6

extern FLOAT	AGCEVENTNATURE_DEFAULT_SKY_RADIUS;
const FLOAT	AGCEVENTNATURE_DEFAULT_CLOUD_DIRECTION	= 0.0f		;	// �������� �귯��..

const FLOAT	AGCEVENTNATURE_DEFAULT_SUN_RADIUS		= 100.0f	;
const FLOAT	AGCEVENTNATURE_DEFAULT_MOON_RADIUS		= 100.0f	;

const INT32	AGCMEVENTNATURE_EMPTY_SKY				= -1		;

enum AGCMEVENTNATURE_CALLBACK
{
	AGCMEVENTNATURE_CB_ID_SKYCHANGE		,
	AGCMEVENTNATURE_CB_ID_LIGHTUPDATE	,
		// pData		: Directional Light RwRGBAReal
		// pCustData	: Ambient Light RwRGBAReal
	AGCMEVENTNATURE_CB_ID_BGMUPDATE
};

enum CLOUD_TYPE
{
	CLOUDTYPE_NONE		,
	CLOUDTYPE_DEFAULT	,
	CLOUDTYPE_1			,
	CLOUDTYPE_2			,
	CLOUDTYPE_3			,
	CLOUDTYPE_4			,
	CLOUDTYPE_COUNT
};

inline void RwRGBARealSet( RwRGBAReal * pRGB , FLOAT r = 1.0f , FLOAT g = 1.0f , FLOAT b = 1.0f , FLOAT a = 1.0f )
{
	pRGB->red	=r;
	pRGB->green	=g;
	pRGB->blue	=b;
	pRGB->alpha	=a;
}

inline void RwRGBARealSetByte( RwRGBAReal * pRGB , UINT8 r = 255 , UINT8 g = 255 , UINT8 b = 255 , UINT8 a = 255 )
{
	pRGB->red	= ( FLOAT ) r / 255.0f;
	pRGB->green	= ( FLOAT ) g / 255.0f;
	pRGB->blue	= ( FLOAT ) b / 255.0f;
	pRGB->alpha	= ( FLOAT ) a / 255.0f;
}

inline void RwRGBASet( RwRGBA * pRGB , UINT8 r = 255 , UINT8 g = 255 , UINT8 b = 255 , UINT8 a = 255 )
{
	pRGB->red	=r;
	pRGB->green	=g;
	pRGB->blue	=b;
	pRGB->alpha	=a;
}

struct	ASkySetting
{
	// Ÿ�Ӽ� ���̱� �۾�.
	enum	TimeSet
	{
		TS_NIGHT2	= 0,	// 00 ~ 05 ����
		TS_DAWN		= 1,	// 05 ~ 07 ����
		TS_DAYTIME1	= 2,	// 07 ~ 13 ����
		TS_DAYTIME2	= 3,	// 13 ~ 18 ����
		TS_EVENING	= 4,	// 19 ~ 21 ����
		TS_NIGHT1	= 5,	// 21 ~ 24 ����
		
		TS_MAX		= 6
	};

	static TimeSet	GetTimeSet( int nHour )
	{
		if		( nHour < 5		)	return TS_NIGHT2	;
		else if	( nHour < 7		)	return TS_DAWN		;
		else if	( nHour < 13	)	return TS_DAYTIME1	;
		else if	( nHour < 18	)	return TS_DAYTIME2	;
		else if	( nHour < 21	)	return TS_EVENING	;
		else						return TS_NIGHT1	;
	}
	static INT32 GetRealTime( TimeSet eTime )
	{
		switch( eTime )
		{
		case ASkySetting::TS_NIGHT2		: return 0	;
		case ASkySetting::TS_DAWN		: return 5	;
		case ASkySetting::TS_DAYTIME1	: return 7	;
		case ASkySetting::TS_DAYTIME2	: return 13	;
		case ASkySetting::TS_EVENING	: return 19	;
		case ASkySetting::TS_NIGHT1		: return 21	;
		}

		return 0;
	}
	static INT32 GetRealTime( INT32 nTime ) { return GetRealTime( ( TimeSet ) nTime ); }

	enum	DataAvailable
	{
		DA_NONE		= 0x00,
		DA_LIGHT	= 0x01,
		DA_FOG		= 0x02,
		DA_CLOUD	= 0x04,
		DA_EFFECT	= 0x08,
		DA_MUSIC	= 0x10,
		DA_WEATHER	= 0x20,
		DA_ALL		= 0x3f	
	};
	
	struct CloudSetting
	{
		INT32		nTexture;	// -1�� no texture
		RwRGBA		rgbColor;	// ���� Į�� ����..
		FLOAT		fSpeed	;	// �ʼ�..
	};

	enum	CloudLayer
	{
		CLOUD_FIRST		,	// ���� ���̴� �༮.
		CLOUD_SECOND	,	// �ָ� ���̴� �༮
		CLOUD_LAYER_MAX	
	};

	// �����Ÿ�� ������ �ִ��� �÷��׷� ����..
	// ��Ʈ�����ڷ� ������.
	SHORT			nDataAvailableFlag	;

	// Light Setting..
	// DA_LIGHT
	RwRGBAReal		rgbSkyTop			;
	RwRGBAReal		rgbSkyBottom		;
	RwRGBAReal		rgbAmbient			;
	RwRGBAReal		rgbDirectional		;

	// Fog Setting..
	// DA_FOG
	RwRGBAReal		rgbFog				;
	FLOAT			fFogDistance		;
	FLOAT			fFogFarClip			;

	// DA_CLOUD
	CloudSetting	aCloud[ CLOUD_LAYER_MAX ]	;
	INT32			nCircumstanceTexture;	// ���ؽ��� �ε���.
	RwRGBA			rgbCircumstance		;
	INT32			nSunTexture			;	// �¾�.
	RwRGBA			rgbSun				;

	// DA_EFFECT
	INT32			nEffectInterval		;
	INT32			aEffectArray	[ AGCMEVENTNATURE_MAX_EVENT ];
	INT32			aEffectRateArray[ AGCMEVENTNATURE_MAX_EVENT ];	// 0~100 
								// ����Ʈ�� 0 �̸� �Ⱦ��� �༮�ӵ�..
								// -1 �̸� Permanet
								// �������� ���ļ� 100�� �׸��� Ȯ������..

	char			strBackMusic[ EFF2_FILE_NAME_MAX ];

	// DA_WEATHER
	// ���ȿ�� 1 ,2 , �Ⱦ��°� -1

	enum WEATHER_PARAMETER_1
	{
		USE_LENS_FLARE		= 0x00000001,	// ���� �÷���
		SHOW_STAR			= 0x00000002,	// �� ���̱�
		SHOW_BREATH			= 0x00000004,	// �ӱ� ǥ��.
		SHOW_MOON			= 0x00000008,	// �޺��̱�..
		DISABLE_POINT_LIGHT	= 0x00000010,	// ����Ʈ ����Ʈ�� ������.
		DISABLE_WEATHER		= 0x00000020,	// ���� ��ȭ�� ���ġ �ʴ´�. �ǳ�,���� .. ���.

		RESERVED_1			= 0x00000040,	// Reserved..
		RESERVED_2			= 0x00000080,	// Reserved.. 
		RESERVED_3			= 0x00000100,	//  Reserved..

		WEA_DEFAULT			= 0x0000000f	
	};

	INT32			nWeatherEffect1;
	INT32			nWeatherEffect2;

	ASkySetting()
	{
		Reset();
	}

	void	Reset()
	{
		// ��� ����Ÿ ����..
		nDataAvailableFlag	= DA_NONE;	// �ϴ� �ƹ��͵� ���°����� ������.

		RwRGBARealSetByte( &rgbSkyTop		, 66 , 131 , 195	, 255 );
		RwRGBARealSetByte( &rgbSkyBottom	, 195 , 223 , 232	, 255 );
		RwRGBARealSetByte( &rgbDirectional	, 119 , 112 , 93	, 255 );
		RwRGBARealSetByte( &rgbAmbient		, 80 , 80 , 80		, 255 );
		RwRGBARealSetByte( &rgbFog			, 193 , 221 , 228	, 255 );

		fFogDistance	= 0.0f	;
		fFogFarClip		= 100000.0f	;

		aCloud[ CLOUD_FIRST		].nTexture	= 3		;
		aCloud[ CLOUD_FIRST		].fSpeed	= 1.0f	;
		RwRGBASet( &aCloud[ CLOUD_FIRST		].rgbColor  , 255 , 255 , 255 , 0 );

		aCloud[ CLOUD_SECOND	].nTexture	= -1	;
		aCloud[ CLOUD_SECOND	].fSpeed	= 1.0f	;
		RwRGBASet( &aCloud[ CLOUD_SECOND	].rgbColor  , 255 , 255 , 255 , 0 );

		nCircumstanceTexture	= -1;
		RwRGBASet( &rgbCircumstance  , 255 , 255 , 255 , 0 );

		nSunTexture	= -1;
		RwRGBASet( &rgbSun  , 255 , 255 , 255 , 0 );
			
		nEffectInterval	= 1000					;
		for( int i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			aEffectArray		[ i ] = 0	;	// 0�̸� �Ⱦ��°�..
			aEffectRateArray	[ i ] = 0	;	// 0�̸� �Ⱦ��°�..
		}

		strncpy( strBackMusic , "" , EFF2_FILE_NAME_MAX );

		nWeatherEffect1 = WEA_DEFAULT;
		nWeatherEffect2 = 0;
	}

	INT32	GetTemporaryEffect()
	{
		INT32	nRandom = rand() % 100;
		INT32	nRate	= 0;
		INT32	i;

		for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			if( aEffectRateArray[ i ] > 0 )
			{
				nRate += aEffectRateArray[ i ];
				if( nRandom < nRate )
				{
					return aEffectArray[ i ];
				}
			}
		}

		// ��ã����
		// ó����..
		for( i = 0 ; i < AGCMEVENTNATURE_MAX_EVENT ; ++ i )
		{
			if( aEffectRateArray[ i ] > 0 )
				return aEffectRateArray[ i ];
		}
		
		// ERROR
		return 0;
	}
};

inline INT32 __MakeSkyTemplateIndexForMapData( int nTemplate , FLOAT fDistance )
{
	// ������ (2004-01-02 ���� 5:05:00) : nTemplate �� ���� ����.
	// �׸��� Distance�� int ��ȯ�ؼ� 2����Ʈ�� ���.
	// ��. ���� 2����Ʈ�� template , ���� 2����Ʈ�� ��Ʈȭ�� �Ÿ�.
	// Distance�� / Ÿ�� �Ÿ� �� �ؼ� ������.

	fDistance /= MAP_STEPSIZE;

	return	( ( nTemplate % 0xffff ) << 16 ) |
			( ( ( INT32 ) fDistance ) % 0xffff );
}

inline INT32 __GetSkyTemplateIndexFromMapData( INT32 nIndex )
{
	return nIndex >> 16;
}

inline FLOAT __GetSkyTemplateDistanceFromMapData( INT32 nIndex )
{
	// ���ǻ���� �ٽ� ����..
	return ( ( FLOAT ) ( nIndex & 0xffff ) * MAP_STEPSIZE );
}

struct	ASkyQueueData : public ASkySetting
{
	ASkyQueueData() : bAvailable( FALSE ) {}

	BOOL	bAvailable;
	// �෹�̼� �߰�..
	INT32	nDuration;
};

// Ŭ���̾�Ʈ ����Ÿ �߰�..
struct AgcdSkySet
{
	ASkySetting		m_aSkySetting[ ASkySetting::TS_MAX ]	; // 24�ð�..
};

struct	ASkyTexture
{
	INT32		nIndex			;
	char		strName[ 256 ]	;
	//RwTexture *	pTexture		;	�������� �ʰ� ��..
};


// ������ (2004-06-09 ���� 10:39:09) : 
// ����� ���ؽ� ����.
struct D3DStarVertex {
	FLOAT	x,y,z;
	DWORD	color;
};
#define D3DFVF_STARVERTEX	( D3DFVF_XYZ | D3DFVF_DIFFUSE )

class AgcmEventNature : public AgcModule  
{
#ifdef _DEBUG
public:
	static BOOL		m_bInstanced			;
#endif
public:
	enum CLOUD_LAYER
	{
		FIRST_CLOUD			,
		FIRST_CLOUD_FADING	,
		SECOND_CLOUD		,
		SECOND_CLOUD_FADING	,
		CLOUD_ATOMIC_LAYER_MAX
	};
	// Fog�� ���õ� Setting��
	enum
	{
		CURRENT_SKY	,	// ���� ��ī��.
		NEXT_SKY	,	// �ٲ�� ���� ��ī��.
		WAIT_SKY	,	// ������ �ٲ����� ��ī��.
		MAX_SKY		
	};


	// Circumstance �ؽ��� ���̾�
	enum
	{
		FORE_TEXTURE	,
		BACK_TEXTURE	,
		MAX_CIRCUMSTANCE_TEXTURE		
	};

	// ������ (2004-06-07 ���� 3:24:15) : ���� ����� �÷���
	enum
	{
		SKY_DONOTCHANGELIGHTCOLOR	= 0x0001,	// Directional , Ambient Į�� ������ ����.
		SKY_DONOTCHANGELIGHTPOSITION= 0x0002,	// �𷺼ų� ����Ʈ�� ��ġ ������ ���߰� ��.
		SKY_DONOTCHANGESKYCOLOR		= 0x0004,	// �ϴ� �ܺ��� Į�� ������ ����
		SKY_DONOTCHANGEFOG			= 0x0008,	// ���� ������ ����.
		SKY_DONOTCHANGECLOUD		= 0x0010,	// �ϴ� �ؽ��� ������ ����
		SKY_DONOTCHANGECIRCUMSTANCE	= 0x0020,	// ��ý���� ������ ����
		SKY_DONOTCHANGEEFFECT		= 0x0040,	// ����Ʈ �÷��̸� ����.
		SKY_DONOTCHANGEBGM			= 0x0080,	// ������� �÷��̸� ����.

		SKY_DONOTMOVE				= 0x0100,	// ��ī�� �ڽ� �̵��� ���ϰ� ��..
		SKY_DONOTTIMECHANGE			= 0x0200,	// �ð��� ���� �ϴ��� ��ȭ�� ���´�.
												// ��.. �ð��� ���߰� �ϴ��� �ٲ����� �𸥴�.

		SKY_DONOTCHANGE				= 0x00ff,	// ��ī�� ��ü�� ��ȭ��Ű�� ���� ������.
	};
	static	FLOAT	__fsSunAngle;

protected:
	BOOL			m_bInit					;	// Init �ž���?

	ApmMap			*	m_pcsApmMap			;
	AgcmMap			*	m_pcsAgcmMap		;
	AgpmEventNature	*	m_pcsAgpmEventNature;
	AgcmRender		*	m_pcsAgcmRender		;
	AgpmTimer		*	m_pcsAgpmTimer		;
	ApmEventManager	*	m_pcsApmEventManager;
	RwTexDictionary	*	m_prwTextureDictionary;	// �ؽ��� ��ųʸ� ����.
	AgcmEff2		*	m_pcsAgcmEff2		;
	AgcmResourceLoader *m_pcsAgcmResourceLoader	;
	ApmObject		*	m_pcsApmObject			;
	AgcmSound		*	m_pcsAgcmSound			;

	AgcmNatureEffect *	m_pcsAgcmNatureEffect	;
//	AgpmEventTeleport*	m_pcsAgpmEventTeleport	;

	// BGM�� �ִ� ���丮..
	char			m_strBGMDirectory[ 256 ];

	struct	EffectInfo
	{
		AgcdEffCtrl_Set		*	pEffect		;
		UINT32					lEID		;
		UINT32					uGap		;
		UINT32					uStartTime	;

		EffectInfo()
		{
			pEffect		= NULL	;
			uGap		= 0		;
			uStartTime	= 0		;
			lEID		= 0		;
		}
	};

	EffectInfo m_paEffectPtrAttay [ AGCMEVENTNATURE_MAX_EVENT ];
					// ����Ʈ ������ ����..

	// �־����� �͵�..
	RpLight		*	m_pLightAmbient		;
	RpLight		*	m_pLightDirectional	;

	// �ϴ��� �����ϴµ� ���Ŵ� �ؽ��ĵ��� ����Ʈ..
	AuList< ASkyTexture		>	m_listTexture		;

	FLOAT			m_fSkyRadius		;
	BOOL			m_bFogOn			;
	BOOL			m_bShowSky			;	// ���� �ϴ��� ǥ�õŰ� �ִ��� ����..
	UINT64			m_nCurrentTime		;	// ����ð�.. �̰��� �������� �������� ������..
	BOOL			m_bNatureEffectPlaying		;	// ����ȿ�� ���ΰ�?..
	ASkyQueueData	m_skyCurrentSetting[ MAX_SKY ];	// ��ī���� ��ȭ�� ���� ������ ������..

	INT32			m_skyMixedUpIndex	[ SECTOR_MAX_NATURE_OBJECT_COUNT ];
	ASkySetting		m_skyMixedUp		[ 24	];	// ��ī�� ������ �����Ͽ� �޸𸮿� �����صд�.

	// �ֱ� �÷����� ���� ����Ʈ ����
	INT32			m_nLastPlayedWeatherEffect1;
	INT32			m_nLastPlayedWeatherEffect2;

	// ���������� ������ �ɸ����� ������..
	RwV3d			m_posLastPosition	;

	////////////////////////////////////////////////////////
	// ���� ȭ���� ǥ���ϴµ� �ʿ��� ����Ÿ��..
	////////////////////////////////////////////////////////
	// �ϴ��� ǥ���ϴ� ������ ����.
	MagImAtomic		m_AtomicSky			;	// ���� ���̴°�
	MagImAtomic		m_aAtomicCloud		[ CLOUD_ATOMIC_LAYER_MAX	]	;	// ���� ����� ó��..
	MagImAtomic		m_aAtomicCircustance[ MAX_CIRCUMSTANCE_TEXTURE	]	;	// ������.
	MagImAtomic		m_aAtomicSun		[ MAX_CIRCUMSTANCE_TEXTURE	]	;	// �¾�.. ��..

	// ��... Nature ���� Object�� Static���� ���� Load�Ǿ��������� ���� �� ������... -_-; �� �κ��� ���� ó�� �ȵ��ֳ�...-_-;
	// �ϴ� �ӽ÷� �ǰԸ� �س�����.... ���ĳ�����..
	AuList< INT32 >	m_listNatureObject;
	
	// Move Sky ���� �����ϴ� ��ǵ�..
	BOOL			m_bSkyIsChanging		;	// ���� �ϴ��� ���ϰ� �ִ��� 
	UINT64			m_nSkyChangeStartTime	;	// �ϴ� ��ȭ ���۽ð�.. ����� ���� �ð� ����

	BOOL			IsSkyChanging()			{ return m_bSkyIsChanging; }

	// Fog Update�� ���� �༮.
	RwCamera	*	m_pCamera				;
	RwBool			m_bFogUpdate			;	// ���׸� ������Ʈ�ؾ��Ѵ�..

	void			SetFogUpdate()			{ m_bFogUpdate = TRUE;}
	RwBool			GetFogUpdate()			{ return m_bFogUpdate;}

	void			CloudUVShift( ASkySetting * pSkySet );
	void			SkyChange	( ASkySetting * pSkySet );
					// ���̾ ������ �ٲ����� UV�� ��ȯ ó��.

	// ������ (2004-06-07 ���� 3:19:21) : 
	UINT32			m_uMaptoolDebugInfoFlag	;	// �������� ��ī�� ��Ʈ�� �÷��� ����.. (������� ��..)

	// ������ (2004-06-08 ���� 4:13:24) : 
	// �����~
	enum	STARTYPE
	{
		SMALL_STAR	,
		MEDIUM_STAT	,
		BLINK_STAR	,
		BIG_STAR	,
		MAX_STAR	
	};

	int				m_nStarShowLevel			;
	int				m_anStarCount[ MAX_STAR ]	;

	// ������ (2004-06-09 ���� 10:50:15) : ���� �޸� ����.
	void *					m_pD3DStarVertex	;
	RwUInt32				m_uStarVertexStride	;
	RwUInt32				m_uStarVertexSize	;
	RwUInt32				m_uStarVertexOffset	;
	RwFrame	*				m_pStarFrame		;

	void			RenderStar	();
	void			BlinkStar	();
	void			DestroyStar	();
	
public:
	int				SetStarShowLevel	( int nLevel )
	{
		int prev = m_nStarShowLevel;
		m_nStarShowLevel = nLevel;
		return prev;
	}
	BOOL			GenerateStarVertex	( int nSmall , int nMedium , int nBlink , int nBig );
	BOOL			LoadStarData		( char * pFilename );

	// ������ (2004-06-07 ���� 3:40:37) : ��Ʋ�� ����� ���.. Ŭ���̾�Ʈ���� ������ ����..
	static BOOL		IsMaptoolBuild			();
	UINT32			GetMaptoolDebugInfoFlag	(){ return m_uMaptoolDebugInfoFlag; }
	UINT32			SetMaptoolDebugInfoFlag	( UINT32 uFlag ){ return m_uMaptoolDebugInfoFlag = uFlag; }

	// ��ī�̿�����Ʈ ����Ʈ ������ .. 
	BOOL			AddSkyObject	( INT32 nID );
	BOOL			RemoveSkyObject	( INT32 nID );

	char *			GetBGMDirectory	() { return m_strBGMDirectory; }
	char *			SetBGMDirectory	( char * pDir ) { strcpy( m_strBGMDirectory , pDir ); return m_strBGMDirectory; }

	BOOL			ModifySky		( MagImAtomic * pAtomic		, RwRGBA * pRgbTop			, RwRGBA * pRgbBottom	);
	BOOL			ModifyAtomic	( MagImAtomic * pAtomic		, INT32 nTexture			, INT32 nAlpha = 255	)
	{
		RwRGBA	rgb = { 255 , 255 , 255 , nAlpha };
		return		ModifyAtomic( pAtomic , nTexture , & rgb );
	}
	BOOL			ModifyAtomic	( MagImAtomic * pAtomic		, INT32 nTexture			, RwRGBA * pRgbColor	);
	BOOL			SetCloud		( INT32 nLayer , ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo , FLOAT fAlpha			);
					// NULL �� ������ �� �ؽ��� ó��..
	BOOL			SetCircumstance	( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo 	, FLOAT fAlpha			);
	BOOL			SetSun			( ASkySetting * pFirstInfo	, ASkySetting * pSecondInfo 	, FLOAT fAlpha			);
	BOOL			FlowCloud		( INT32 nLayer , FLOAT fAmount			);	// ���� �귯����..

	BOOL			ChangeSunSize	( FLOAT fRadius = 0.2f );
	
	// ���� �����ϴ� �Լ�..
	void			UpdateProjectionMatrix(RwCamera*	camera);		// beginupdate�� endupdate ���ϰ� far���� �ǵ���..gemani(04.08.25)
	void			RenderSky		();
	void			SetSkyHeight	( FLOAT height				);

	// �ϴ����� ��.
	ASkySetting *			GetCurrentSkySetting	()
	{
		return &m_skyCurrentSetting[ CURRENT_SKY ];
	}

	AuList< ASkyTexture	> *	GetTextureList	()
	{
		return & m_listTexture;
	}

	// ��ī�� ���ø� ���� ���..

	// ��ī�̸� õõ�� ��ȭ��Ű�� �Լ�..
	//BOOL	SetNextWeather( ASkySetting * pSkySet , INT32 nDuration = 10000 , BOOL bForce = FALSE );
	BOOL	SetNextWeather( ASkySetting::TimeSet eTimeSet , INT32 nDuration = 10000 , BOOL bForce = FALSE );
	// ��ī�̸� ���� ���ø�����Ÿ�� ��Ī �������� �ð��� �����ϴ� ������� ����.

	// �� ��ī�� ������ �ð���� ������ ��ȭ��Ų��. Ȯ ���ϰԵ�.
	BOOL	ApplySkySetting		();
	
	// ���ø� �����Լ�..
	// �̰ɷ� �����ϸ� ���� �ð��� ������ ���� �´�.
	INT32	SetSkyTemplateID( INT32 nID					, INT32 nFlag );
	INT32	SetSkyTemplateID( ASkySetting	* pSkySet	, INT32 nFlag );
		// nFlag �� ���ö��� �ε���..

	// ������ (2006-03-10 ���� 11:18:13) : 
	// ������ ����Ʈ ��ī�� ����.
	INT32	SetNatureEffectSkyTemplateID( INT32 nID , BOOL bForce );
	void	EndNatureEffectSky( BOOL bForce ) ;

	BOOL	IsNatureEffectPlaying() { return m_bNatureEffectPlaying; }

	// ��������� ���⿡ ������ �´�..
	INT32				m_nCurrentBGMSetIndex;
	vector< char * >	m_arrayBGM			;
	UINT32				m_uLastBGMStarted	;

	void	PlayBGM();

public:
	AgcmEventNature();
	virtual ~AgcmEventNature();

	// �ʱ�ȭ ���..
	// �ϴõ���Ÿ�� ����Ʈ �������� ������..
	RwCamera *	SetCamera			( RwCamera * pNewCamera )
	{
		RwCamera * pOldCamera = m_pCamera;
		m_pCamera = pNewCamera;
		return m_pCamera;
	}
	BOOL		Init				( RpLight * pAmbient , RpLight * pDirectional , RwCamera * pCamera );
	BOOL		Init				( RpWorld * pWorld , RpLight * pAmbient , RpLight * pDirectional , RwCamera * pCamera )
	{
		// �������� ȣ�Ͽ�..
		return Init( pAmbient , pDirectional , pCamera );
	}

	// ���� ���..
	BOOL		SetTime				( UINT64	time				);
	UINT64		GetTime				() { return m_nCurrentTime; }
	BOOL		SetTimeForce		( UINT64	time				);	// ������ �ش�ð����ϴ÷� �ٲ������.

	BOOL		SetFogSetting		();									// ���� ������ ��������Ŭ �ȿ� ��ġ��Ų��. ���� ������ �����Ͽ� �ڵ� �����Ŵ..
		// ���� �ð��� ������. �̰����� �¾�Ǵ� ���� ��ġ�� �ٲ�..
		// ���� ����Ʈ ������ ������..

	BOOL		SetCharacterPosition( AuPOS pos	, BOOL bForce = FALSE	);
		// �ɸ��Ͱ� ���� ��ġ�� �κ��� ǥ����.
		// �� �Լ��� ȣ��� , �ϴùݱ��� ��ġ�� �ɸ����� ��ġ�� �ű�.

	BOOL		ShowSky				( BOOL	bShow		);
	BOOL		IsShowSky			() { return m_bShowSky; }
		// ���忡�� �ϴ� �ݱ��� ������..

	// �ؽ��� ����Ʈ ����
	INT32		AddTexture			( char *	pFilename	, INT32	nIndex = 0	);

	RwTexture *	GetTexture			( INT32		nIndex		);
		// �ؽ��ĸ� ���������� , ��! RwTextureDestroy�� ���־���Ѵ�.
		// ��ü���� �ؽ��ĸ� ������ ���� �������̴�..

	BOOL		RemoveTexture		( INT32		nIndex		);
	INT32		GetTextureIndex		( char *	pFilename	);
	void		RemoveAllTexture	(						);

	// Fog ����..

	FLOAT			m_fFogDistance			;
	FLOAT			m_fFogFarClip			;
	RwRGBA			m_stFogColor			;

	FLOAT		GetFogDistance	()	{ return m_fFogDistance	;	}
	FLOAT		GetFogFarClip	()	{ return m_fFogFarClip	;	}
	RwRGBA *	GetFogColor		()	{ return &m_stFogColor	;	}

	void		SetSkyRadius		( FLOAT		fRadius		);
	void		SetSkyRadius		( INT32		nRadius		);	// ���� ����.
	void		SetFog				(
		BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogFarClip	,
		RwRGBA *	pstFogColor		);

	// ����..
	BOOL		LoadTextureScriptFile	( CHAR	* szPath	= AGCMEVENTNATURE_SCRIPT_FILE_NAME, BOOL bDecryption = FALSE	);
	BOOL		SaveTextureScriptFile	( CHAR	* szPath	= AGCMEVENTNATURE_SCRIPT_FILE_NAME, BOOL bEncryption = FALSE	);
	void		CleanAllData			();	// �о�鿴�� ����Ÿ �ʱ�ȭ..

	
	// ��ī�� ������Ʈ
	void		MoveSky				();

	// ������ ������� , �ϴ��� ���� �������� �����Ͽ� ������� �� �����Ѵ�.
	void		SetCloudZoomRate	( FLOAT fCloudZoomRate );

public:
	// ��ī�� �� ����..
	// AgpdSkySet�κ��� AgcdSkySet�� �����´�.
	AgcdSkySet *	GetSkySetClientData( AgpdSkySet	* pstSkySet );
	INT16			m_nSkySetAttachIndex;


//protected:
public:
		// �ϴ� �ݱ� ����µ� �ʿ��� �ؽ��Ŀ� ��ũ��Ʈ�� �о����..

	// �¿��� ���� ����.. fAngle�� Radian ���� ó����.
	// ���ʿ��� �㶧�� 0 ���� ���� , �׸��� PI���� �Ÿ� ������ ���� ����..
	// �� ������� �̻󳻷����� ǥ������ ����..
	BOOL		SetSunAngle			( FLOAT	fAngle		);
	BOOL		SetMoonAngle		( FLOAT	fAngle		);

	// �ݱ��� �����.
	BOOL		CreateSkyAtomic				( MagImAtomic * pAtomic );
	BOOL		CreateCloudAtomic			( MagImAtomic * pAtomic , FLOAT fCloudZoomRate = 4.0f );
	BOOL		CreateCircumstanceAtomic	( MagImAtomic * pAtomic );
	BOOL		CreateSunAtomic				( MagImAtomic * pAtomic	);

	// �������� ���°�.
	static RpAtomic		*	CreateSkyAtomic		();
	static RtWorldImport *	_CreateHalfSphere	();	// �ݱ� �����..
	static BOOL				ModifyAtomic		( RpAtomic * pAtomic , INT32 nAlpha	);

public:

	// Virtual ��ǵ�
	BOOL	OnAddModule		();

	BOOL	OnInit			();
	BOOL	OnDestroy		();
	void		OnLuaInitialize( AuLua * pLua );

	BOOL	OnIdle			(UINT32 ulClockCount);


	BOOL	SetCallbackSkyChange		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_SKYCHANGE		, pfCallback, pClass);}
	BOOL	SetCallbackLightUpdate		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_LIGHTUPDATE	, pfCallback, pClass);}
		// pData		: Directional Light RwRGBAReal
		// pCustData	: Ambient Light RwRGBAReal
	BOOL	SetCallbackBGMUpdate		(ApModuleDefaultCallBack pfCallback, PVOID pClass){	return SetCallback( AGCMEVENTNATURE_CB_ID_BGMUPDATE	, pfCallback, pClass);}
		// pData		: current played BGM file name ( char * )
	
	// Callback
	static BOOL	OnMyCharacterPositionChange	( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnSetSelfCharacter		( PVOID pData, PVOID pClass, PVOID pCustData );
//	static BOOL	CBOnTeleportStart			( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnSetAutoLoadChange		( PVOID pData, PVOID pClass, PVOID pCustData );

	// SkySet Data ������, �ı���
	static BOOL	OnSkySetCreate	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	OnSkySetDestroy	(PVOID pData, PVOID pClass, PVOID pCustData);

	// SkySet Streaming Callback
	static BOOL	SkySetStreamReadCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	SkySetStreamWriteCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	static BOOL	CBOnAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBOnNatureEffectStart	( PVOID pData, PVOID pClass, PVOID pCustData );
	static BOOL	CBOnNatureEffectEnd		( PVOID pData, PVOID pClass, PVOID pCustData );

	INT32	GetDefaultSky	( FLOAT fX , FLOAT fZ );
	BOOL	AddEventNatureObject( ApWorldSector * pSector , ApdObject * pcsApdObject , INT32 nTemplate , FLOAT fRadius , UINT32 nSkyFlag , INT32 nDepth = 1 );

	static BOOL	CBLoadSector		(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	UpdateSectorNatureObject( ApWorldSector * pSector , INT32 uID );

	static BOOL CBReceiveTime		(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBNature			(PVOID pData, PVOID pClass, PVOID pCustData);

	// ���� ����..
	AuList< ApWorldSector * >	m_listUpdateSectorNatureInfo		;
	ApCriticalSection			m_MutexlistUpdateSectorNatureInfo	;
	void	UpdateSectorNatureInfo	( ApWorldSector * pSector );

	struct SectorNatureUpdateInfo
	{
		INT32	uSectorX	;
		INT32	uSectorZ	;
		INT32	uObjectID	;
	};

	AuList< SectorNatureUpdateInfo >	m_listUpdateSectorNature	;

public:
	// ������ ����ϴ� ��ǵ�..
	static BOOL	ResampleTexture		( char * pSource , char * pDestination , INT32 nWidth , INT32 nHeight );
	static BOOL	__SetTime			( AgcmEventNature * pEventNature , INT32 hour , INT32 minutes );
	static void	__SetFog			( AgcmEventNature * pEventNature , 
		BOOL		bFogOn			, FLOAT		fFogDistance	, FLOAT		fFogFarClip	,
		RwRGBA *	pstFogColor		);

	static void		__MoveSky		( AgcmEventNature * pEventNature );
	static void		__SetSkyRadius	( AgcmEventNature * pEventNature ,  FLOAT		fRadius		);
	static INT32	__SetSkyTemplateID	( AgcmEventNature * pEventNature , ASkySetting	* pSkySet	, INT32 nFlag );

	static BOOL		__PreRenderCallback(PVOID pData, PVOID pClass, PVOID pCustData);
	
	// ȯ�溯��.
	static FLOAT	AgcmEventNature::SKY_RADIUS				;
	static FLOAT	AgcmEventNature::SKY_HEIGHT				;
	static FLOAT	AgcmEventNature::CIRCUMSTANCE_RADIUS	;
	static FLOAT	AgcmEventNature::CIRCUMSTANCE_HEIGHT	;

	// �¾� ��ġ 
	FLOAT			m_fSkyCircleRadius;
	void			SetSkyCircleRadius();				//ȯ�溯���� sky���� �������� ���Ѵ�

	RwV3d			m_posSun;
	void			UpdateSunPosition();				//light direction ���� ����ÿ� sun position�� �ٽ� ���Ѵ�.
	BOOL			GetSunPosition	( RwV3d * pV3d);
				// return TRUE	: Sun
				// return FALSE	: Moon

	BOOL			EnableFog	( BOOL bEnable );
	BOOL			IsFogOn		()	{ return m_bFogOn		;	}

	// ������ (2005-03-31 ���� 6:20:06) : �ε�Ÿ������� �� ��ȭ��Ŵ.
	FLOAT			m_fFogAdjustFarClip		;

	FLOAT			m_fNearValue			;
	FLOAT			m_fNormalValue			;

	BOOL			m_bSkyFogDisable			;

	RwV3d			GetLastPosition() { return m_posLastPosition; }

	//add by dobal
public:
	bool			m_bLostDevice;
	static AgcmEventNature*	m_pThis;
	static rwD3D9DeviceRestoreCallBack OldD3D9RestoreDeviceCB;
	static rwD3D9DeviceReleaseCallBack OldD3D9ReleaseDeviceCB;

	static void		NewD3D9ReleaseDeviceCB();
	static void		NewD3D9RestoreDeviceCB();

	void			OnLostDevice();
	void			OnResetDevice();

private :
	void			_ChangeEnvironmentSound( void );
};

extern FLOAT	AGCEVENTNATURE_DEFAULT_SKY_HEIGHT;
extern UINT32	__uSkyChangeGap;

#endif // !defined(AFX_AGCMEVENTNATURE_H__03F92BCE_D3D8_4B8B_88FA_F351422C3D5E__INCLUDED_)
