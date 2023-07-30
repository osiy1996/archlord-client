#pragma once

#include "XTPGI_EffBase.h"
#include "AgcdEffCamera.h"



class CXTPGI_Camera 
	:	public CXTPGI_EffBase
{
public:
	CXTPGI_Camera( CString strCaption , AgcdEffCamera*	pEffCamera );
	virtual ~CXTPGI_Camera( VOID );


protected:
	VOID						OnAddChildItem( VOID );

private:
	AgcdEffCamera*					m_pEffCamera;


	CXTPGI_Enum<eEffCameraType>*	m_pItemType;

	CXTPGI_Float*					m_pItemPosX;
	CXTPGI_Float*					m_pItemPosY;
	CXTPGI_Float*					m_pItemPosZ;

	CXTPGI_Float*					m_pItemDirX;
	CXTPGI_Float*					m_pItemDirY;
	CXTPGI_Float*					m_pItemDirZ;


};
