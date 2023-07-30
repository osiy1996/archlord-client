#ifndef	_H_AGCDEFFPOSTFX_20050421
#define _H_AGCDEFFPOSTFX_20050421

#include "AgcuEff2ApMemoryLog.h"
#include "AgcdEffBase.h"

enum	eFXType
{
	e_fx_Wave	= 0,
	e_fx_Ripple			,
	e_fx_ShockWave		,
	e_fx_EdgeDetect		,
	e_fx_Sepia			,
	e_fx_GrayScale		,
	e_fx_Negative		,
	e_fx_ClampingCircle	,
	e_fx_Darken			,
	e_fx_Brighten		,
	e_fx_Twist			,

	e_fx_typenum		,
};

class AgcdEffPostFX : public AgcdEffBase
{
	EFFMEMORYLOG_SMV;

	static LPCSTR	tech[e_fx_typenum];

public:
	AgcdEffPostFX				( VOID );
	AgcdEffPostFX				( const AgcdEffPostFX& cpy ) : AgcdEffBase(cpy) {cpy;}
	AgcdEffPostFX& operator =	( const AgcdEffPostFX& cpy ) { cpy; return *this; }
	virtual	~AgcdEffPostFX		( VOID );

	VOID						SetTech				( RwV2d*	pV2dCenter , FLOAT fRadius , CONST TCHAR*	pszTexture = NULL );
	LPCSTR						GetTech				( VOID )	CONST		{	return tech[m_eFXType];	}

	RwInt32						bToFile				( FILE* fp );
	RwInt32						bFromFile			( FILE* fp );
	
#ifdef USE_MFC
	virtual	INT32				bForTool_Clone		( AgcdEffBase* pEffBase );
#endif
	
	eFXType						m_eFXType;				//	FX Type
	RwV2d						m_v2dCenter;			//	FX Start Position
	DWORD						m_dwBitFlags;			//	FX Flags

};

#endif