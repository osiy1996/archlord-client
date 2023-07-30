// AgcdEffPostFX.cpp
// -----------------------------------------------------------------------------
//                          _ ______  __  __ _____            _   ________   __                     
//     /\                  | |  ____|/ _|/ _|  __ \          | | |  ____\ \ / /                     
//    /  \    __ _  ___  __| | |__  | |_| |_| |__) | ___  ___| |_| |__   \ V /      ___ _ __  _ __  
//   / /\ \  / _` |/ __|/ _` |  __| |  _|  _|  ___/ / _ \/ __| __|  __|   > <      / __| '_ \| '_ \ 
//  / ____ \| (_| | (__| (_| | |____| | | | | |    | (_) \__ \ |_| |     / . \  _ | (__| |_) | |_) |
// /_/    \_\\__, |\___|\__,_|______|_| |_| |_|     \___/|___/\__|_|    /_/ \_\(_) \___| .__/| .__/ 
//            __/ |                                                                    | |   | |    
//           |___/                                                                     |_|   |_|    
//
// post fx effect
//
// -----------------------------------------------------------------------------
// Originally created on 04/21/2005 by Kyeongsam Moon
//
// Copyright 2005, NHN Games, Inc., all rights reserved.
// -----------------------------------------------------------------------------

#include <AgcmEff2/AgcdEffPostFX.h>
#include <ApMemoryTracker/ApMemoryTracker.h>

#include <AcuMath/AcuMathFunc.h>
USING_ACUMATH;

EFFMEMORYLOG(AgcdEffPostFX);

LPCSTR AgcdEffPostFX::tech[e_fx_typenum] = {
	"Wave",				//e_fx_Wave			
	"Ripple",			//e_fx_Ripple
	"ShockWave",		//e_fx_Shockwave		
	"EdgeDetect",		//e_fx_EdgeDetect
	"Sepia",			//e_fx_Sepia		
	"GrayScale",		//e_fx_GrayScale
	"Negative",			//e_fx_Negative	
	"ClampingCircle",	//e_fx_ClampingCircle	
	"Darken",			//e_fx_Darken
	"Brighten",			//e_fx_Brighten		
	"Twist",			//e_fx_Twist
};

// -----------------------------------------------------------------------------
AgcdEffPostFX::AgcdEffPostFX( VOID )
: AgcdEffBase( AgcdEffBase::E_EFFBASE_POSTFX )
,	m_eFXType(e_fx_Wave)
,	m_dwBitFlags(0)
{
	EFFMEMORYLOG_CON;

	ZeroMemory( &m_v2dCenter , sizeof(m_v2dCenter) );
}

// -----------------------------------------------------------------------------
AgcdEffPostFX::~AgcdEffPostFX( VOID )
{
	EFFMEMORYLOG_DES;
}


// file in out
// -----------------------------------------------------------------------------
RwInt32 AgcdEffPostFX::bToFile(FILE* fp)
{
	RwInt32	ir = AgcdEffBase::tToFile( fp );
	ir += AgcdEffBase::tToFile( fp );

	ir	+=	fwrite( &m_eFXType , 1 , sizeof(m_eFXType) , fp );
	ir	+=	fwrite( &m_v2dCenter , 1 , sizeof(m_v2dCenter) , fp );
	ir	+=	fwrite( &m_dwBitFlags , 1 , sizeof(m_dwBitFlags) , fp );

	ir += AgcdEffBase::tToFileVariableData( fp );
	return ir;
};

// -----------------------------------------------------------------------------
RwInt32 AgcdEffPostFX::bFromFile(FILE* fp)
{
	RwInt32 ir = AgcdEffBase::tFromFile(fp);

	ir	+=	fread( &m_eFXType , 1 , sizeof(m_eFXType) , fp );
	ir	+=	fread( &m_v2dCenter , 1 , sizeof(m_v2dCenter) , fp );
	ir	+=	fread( &m_dwBitFlags , 1 , sizeof(m_dwBitFlags) , fp );

	RwInt32 ir2 = AgcdEffBase::tFromFileVariableData(fp);
	if( T_ISMINUS4( ir2 ) )
		return -1;

	return ir + ir2;
};

#ifdef USE_MFC
// -----------------------------------------------------------------------------
INT32 AgcdEffPostFX::bForTool_Clone(AgcdEffBase* pEffBase)
{
	ASSERT( "kday" && !"PostFX는 한이펙트에 2개를 넣지 못합니다." );
	return -1;
}
#endif//USE_MFC

// -----------------------------------------------------------------------------
// AgcdEffPostFX.cpp - End of file
// -----------------------------------------------------------------------------
