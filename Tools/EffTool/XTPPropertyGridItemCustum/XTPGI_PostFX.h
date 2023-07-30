#pragma once

#include "XTPGI_EffBase.h"
#include "AgcdEffPostFX.h"

class CXTPGI_PostFX 
	:	public CXTPGI_EffBase
{
public:
	CXTPGI_PostFX( CString strCaption , AgcdEffPostFX*	pEffPostFX );
	virtual ~CXTPGI_PostFX( VOID );

protected:
	VOID						OnAddChildItem( VOID );

private:
	AgcdEffPostFX*				m_pEffPostFX;
	CXTPGI_Enum<eFXType>*		m_pItemType;

	CXTPGI_FlagBool*			m_pItemUnseenOther;
	CXTPGI_FlagBool*			m_pItemCenterLock;

};
