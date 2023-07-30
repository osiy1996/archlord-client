// MyEngine.h: interface for the MyEngine class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_)
#define AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcEngine.h>
#include "ApmMap.h"
#include "AgcmMap.h"
#include "AgcmEffect.h"
#include "AgpmFactors.h"
#include "ApmObject.h"
#include "AgcmObject.h"
#include "AgpmCharacter.h"
#include "AgcmCharacter.h"
#include "AgpmItem.h"
#include "AgcmItem.h"


#define	MYENGINE			((MyEngine *) g_pEngine)

#define	AGCMAP_MAX_SECTOR_DIMENSION		10

typedef	enum AgcMapSectorType
{
	AGCMAP_SECTOR_TYPE_DUMMY = 0, 
	AGCMAP_SECTOR_TYPE_ROUGH, 
	AGCMAP_SECTOR_TYPE_DETAIL
} AgcMapSectorType;

typedef struct AgcMapSector
{
	RpWorldSector		*m_pSector;
	AgcMapSectorType	m_nType;
} AgcMapSector;

class MyEngine : public AgcEngine  
{
public:
	ApmMap			m_csApmMap			;
	AgcmMap			m_csAgcmMap			;

	AgcmEffect		m_csAgcmEffect		;

	AgpmFactors		m_csAgpmFactors		;

	ApmObject		m_csApmObject		;
	AgcmObject		m_csAgcmObject		;

	AgpmCharacter	m_csAgpmCharacter	;
	AgcmCharacter	m_csAgcmCharacter	;

	AgpmItem		m_csAgpmItem		;
	AgcmItem		m_csAgcmItem		;

	INT16			m_nDimensionX		;
	INT16			m_nDimensionZ		;
	FLOAT			m_fSectorSize		;
	RwBBox			m_stBBox			;
	AgcMapSector	m_astSectors		[AGCMAP_MAX_SECTOR_DIMENSION][AGCMAP_MAX_SECTOR_DIMENSION];

public:
	BOOL			OnRegisterModule	();
	MyEngine();
	virtual ~MyEngine();

	// Message
	virtual	BOOL		OnAttachPlugins			();	// �÷����� ���� �� ���⼭
	virtual	RpWorld *	OnCreateWorld			(); // ���带 �����ؼ� �������ش�.
	virtual	RwCamera *	OnCreateCamera			( RpWorld * pWorld );	// ���� ī�޶� Create���ش�.
//	virtual	BOOL		OnCreateLight			();	// ������ �����ؼ�. �� ������ �������ش�. m_pLightAmbient , m_pLightDirect
	virtual	AgcWindow *	OnSetMainWindow			();	// ����Ʈ �����츦 �������ִ� �޽���.. �����츦 �ʱ�ȭ�ؼ� ������ �ѱ�� �´�.
//	void				OnTerminate				();
//	void				OnRender				();
};

#endif // !defined(AFX_MYENGINE_H__771281B5_C1D8_49D7_B37D_3DDD8F62B6B3__INCLUDED_)
