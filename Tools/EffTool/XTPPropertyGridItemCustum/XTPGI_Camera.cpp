#include "stdafx.h"
#include "XTPGI_Camera.h"



const CHAR*		CAMERADATA[]	=
{
	"Zoom"		,
	"Rotate"	,
	"Origin"	,
};


CXTPGI_Camera::CXTPGI_Camera( CString strCaption , AgcdEffCamera*	pEffCamera )
: CXTPGI_EffBase(strCaption, pEffCamera)
,	m_pItemType( NULL )
,	m_pItemPosX( NULL )
,	m_pItemPosY( NULL )
,	m_pItemPosZ( NULL )
,	m_pItemDirX( NULL )
,	m_pItemDirY( NULL )
,	m_pItemDirZ( NULL )
,	m_pEffCamera( pEffCamera )
{

}

CXTPGI_Camera::~CXTPGI_Camera( VOID )
{

}

VOID CXTPGI_Camera::OnAddChildItem( VOID )
{
	CXTPGI_EffBase::OnAddChildItem();

	m_pItemType = 
		(CXTPGI_Enum<eEffCameraType>*)AddChildItem( new CXTPGI_Enum<eEffCameraType>( _T("Type")
		, EFF_CAMERA_COUNT
		, (eEffCameraType)m_pEffCamera->m_unCameraType
		, (eEffCameraType*)&m_pEffCamera->m_unCameraType
		, CAMERADATA ) );
	
	m_pItemPosX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Pos X") , &m_pEffCamera->m_vCameraPos.x , m_pEffCamera->m_vCameraPos.x ));
	m_pItemPosX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Pos Y") , &m_pEffCamera->m_vCameraPos.y , m_pEffCamera->m_vCameraPos.y ));
	m_pItemPosX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Pos Z") , &m_pEffCamera->m_vCameraPos.z , m_pEffCamera->m_vCameraPos.z ));

	m_pItemDirX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Dir X") , &m_pEffCamera->m_vCameraDir.x , m_pEffCamera->m_vCameraDir.x ));
	m_pItemDirX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Dir Y") , &m_pEffCamera->m_vCameraDir.y , m_pEffCamera->m_vCameraDir.y ));
	m_pItemDirX		= (CXTPGI_Float*)AddChildItem( new CXTPGI_Float( _T("Dir Z") , &m_pEffCamera->m_vCameraDir.z , m_pEffCamera->m_vCameraDir.z ));
}