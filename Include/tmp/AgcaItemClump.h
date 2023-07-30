/*****************************************************************************
* File : AgcaItemClump.h
*
* Desc : ������ clump�� �����Ѵ�. (�ߺ��� ���� ����.)
*
* 2002. 9. 12. Bob Jung.
*****************************************************************************/

#ifndef __AGCAITEMCLUMP_H__
#define __AGCAITEMCLUMP_H__

#include "ApBase.h"

#include "ApAdmin.h"

#include "rwcore.h"
#include "rpworld.h"

#include "AgcmResourceLoader.h"

class AgcdItemClump
{
public:
	ApCriticalSection	m_csCSection;
	RpClump *			m_pstClump;
	INT32				m_lRefCount;

	AgcdItemClump()
	{
		m_pstClump	= NULL;
		m_lRefCount	= 0;
	}
};

class AgcaItemClump : public ApAdmin
{
public:
	AgcmResourceLoader				*m_pcsAgcmResourceLoader;
	static RwTextureCallBackRead	m_fnDefaultTextureReadCB;

	AgcaItemClump();
	virtual ~AgcaItemClump();

	BOOL		Initialize(INT32 lMaxDataNum);
	RpClump		*AddItemClump(CHAR* szFile);
	RpClump		*GetItemClump(CHAR* szFile);
	BOOL		RemoveItemClump(CHAR* szFile);
	BOOL		RemoveAllItemClump();

protected:	
	static RwTexture		*CBTextureRead(const RwChar *name, const RwChar *maskName);
};

#endif  // __AGCAITEMCLUMP_H__

/******************************************************************************
******************************************************************************/