#pragma once

#include "AgcModule.h"
#include "AgcmRender.h"
#include "AgcmDynamicLightmap.h"
#include "AgpmEventPointLight.h"

#include <vector>

enum AgcdPLFlag
{
	AGCPLF_DISABLE			= 0x00000000,

	AGCPLF_ENABLE_ALLTIME	= 0x00ffffff,	// �ð���.

	AGCPLF_AFFECT_OBJECT	= 0x01000000,	// ������Ʈ ���� ( AgcmRender �� �߰� )
	AGCPLF_AFFECT_GEOMETRY	= 0x02000000,	// ������ ���� ( g_pcsAgcmDynamicLightmap �� �߰� )

	AGCPLF_DEFAULT			= 0x03ffffff
};

// ��ī�� ������ ���� ��Ʈ����..
class AgcdPointLight : public ApBase
{
public:
	UINT8	uRed		;
	UINT8	uGreen		;
	UINT8	uBlue		;
	FLOAT	fRadius		;
	INT32	nEnableFlag	;

	// 0��° ��Ʈ�� 0��
	// 1��° ��Ʈ�� 1��
	// ...
	// 23��° ��Ʈ�� 23��

	RpLight	*pLight		;
};

class AgcmEventPointLight : public AgcModule
{
public:
	ApmEventManager		*	m_pcsApmEventManager	;
	AgpmEventPointLight	*	m_pcsAgpmEventPointLight;

	AgcmRender			*	m_pcsAgcmRender			;
	AgcmDynamicLightmap	*	m_pcsAgcmDynamicLightmap;	
public:
	AgcdPointLight *	GetPointLightClientData( AgpdPointLight	* pstPLight );
	INT16				m_nPointLightAttachIndex;

public:
	struct	LightObjectInfo
	{
		ApdObject		*pcsApdObject	;
		AgcdPointLight	*pstAgcdPLight	;

		LightObjectInfo():pcsApdObject(NULL),pstAgcdPLight(NULL){}
	};

	vector< LightObjectInfo >	m_vectorPointLight;
	BOOL		AddLightToArray		( AgcdPointLight * pcsAgcdPLight , ApdObject * pcsApdObject );
	BOOL		RemoveLightToArray	( AgcdPointLight * pcsAgcdPLight );

	BOOL		AddLight	( AgcdPointLight * pcsAgcdPLight , AuPOS * pPos );
	BOOL		RemoveLight	( AgcdPointLight * pcsAgcdPLight );

	// Add & Remove
	static BOOL	CBOnPointLightAddObject		( PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBOnPointLightRemoveObject	( PVOID pData, PVOID pClass, PVOID pCustData);

public:
	AgcmEventPointLight(void);
	~AgcmEventPointLight(void);

	BOOL	m_bEnablePointLight;
	void	EnablePointLight( BOOL bEnable = TRUE );
	void	TogglePointLight();

	//. 2006. 3. 7. Nonstopdj
	//. Return to Login Process�� ��� CBSkyChange()�� EnablePointLight()�� TRUE�� �����Ѵ�.
	//. Login Lobby�� �̵��� ��� CBSkyChange()�� ���Ͽ� Sector�� Light���� Remove�ȴ�.
	BOOL	m_bReturnLoginLobby;

	virtual BOOL	OnAddModule	();
	virtual BOOL	OnInit		();

	// SkySet Data ������, �ı���
	static BOOL	OnPointLightCreate	(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	OnPointLightDestroy	(PVOID pData, PVOID pClass, PVOID pCustData);

	// SkySet Streaming Callback
	static BOOL	PointLightStreamReadCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);
	static BOOL	PointLightStreamWriteCB	( PVOID pData, ApModule *pClass, ApModuleStream *pcsStream);

	static BOOL	CBSkyChange ( PVOID	pData, PVOID pClass, PVOID pCustData );
};
