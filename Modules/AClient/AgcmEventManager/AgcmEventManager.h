/******************************************************************************
Module:  AgcmEventManager.h
Notices: Copyright (c) NHN Studio 2002 netong
Purpose: 
Last Update: 2003. 04. 23
******************************************************************************/

#if !defined(__AGCMEVENTMANAGER_H__)
#define __AGCMEVENTMANAGER_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventManagerD" )
#else
#pragma comment ( lib , "AgcmEventManager" )
#endif
#endif


#include <ApBase/ApBase.h>
#include <AgcModule/AgcModule.h>

class AgcmCharacter;
enum ApdEventFunction;

class AgcmEventManager : public AgcModule {
private:
	class AgcmObject *		m_pcsAgcmObject;
	class AgcmCharacter *	m_pcsAgcmCharacter;
	class ApmEventManager *	m_pcsApmEventManager;

	BOOL				m_bSetupEventObject;

public:
	AgcmEventManager();
	~AgcmEventManager();

	// �ʼ� �Լ� (virtual �Լ���)
	BOOL			OnAddModule		(						);
	BOOL			OnInit			(						);
	BOOL			OnDestroy		(						);

	BOOL			RegisterEventCallback	( ApdEventFunction eFunction, ApModuleDefaultCallBack fnCallback, PVOID pvClass );

	BOOL			CheckEvent		( ApBase *pcsBase,	ApBase *pcsGenerator	);
	BOOL			CheckEvent		( ApBase *pcsBase,	ApBase *pcsGenerator, ApdEventFunction eFunction );

	VOID			SetAddEventObject	( BOOL	bActive		)	{ m_bSetupEventObject = bActive; }

	static BOOL		CBInitEventObject(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif //__AGCMEVENTMANAGER_H__
