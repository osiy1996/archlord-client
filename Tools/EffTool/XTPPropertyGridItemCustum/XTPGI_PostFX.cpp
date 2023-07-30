#include "stdafx.h"
#include "XTPGI_PostFX.h"

const CHAR*		FXDATA[]	=
{
	"Wave"			,
	"Ripple"		,
	"ShockWave"		,
	"EdgeDetect"	,
	"Sepia"			,
	"GrayScale"		,
	"Negative"		,
	"ClampingCircle",
	"Darken"		,
	"Brighten"		,
	"Twist"			,
};



CXTPGI_PostFX::CXTPGI_PostFX( CString strCaption , AgcdEffPostFX*	pEffPostFX )
: CXTPGI_EffBase(strCaption, pEffPostFX)
,	m_pItemType( NULL )
,	m_pEffPostFX( pEffPostFX )
{

}

CXTPGI_PostFX::~CXTPGI_PostFX( VOID )
{

}

VOID CXTPGI_PostFX::OnAddChildItem( VOID )
{
	CXTPGI_EffBase::OnAddChildItem();

	CXTPPropertyGridItem*	pCategory = AddChildItem( new CXTPPropertyGridItem( _T("-- Post FX --"), _T("category") ) );
	pCategory->SetReadOnly(TRUE);

	// Post FX Type
	m_pItemType =	(CXTPGI_Enum<eFXType>*)pCategory->AddChildItem( new CXTPGI_Enum<eFXType>( _T("FX Type")
					, e_fx_typenum
					, (eFXType)m_pEffPostFX->m_eFXType
					, (eFXType*)&m_pEffPostFX->m_eFXType
					, FXDATA ) );

	// Post FX Unseen Other
	m_pItemUnseenOther	= (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool((UINT*)&m_pEffPostFX->m_dwBitFlags, FLAG_EFFBASEPOSTFX_UNSEENOTHER, _T("Unseen Other")));
	m_pItemCenterLock	= (CXTPGI_FlagBool*)pCategory->AddChildItem(new CXTPGI_FlagBool((UINT*)&m_pEffPostFX->m_dwBitFlags, FLAG_EFFBASEPOSTFX_CENTERLOCK, _T("Center Lock")));
	
}
