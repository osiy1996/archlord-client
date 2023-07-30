// AgcmEventTeleport.h: interface for the AgcmEventTeleport class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTTELEPORT_H__E05B7CDF_25AA_4E94_BD67_D29CA0898494__INCLUDED_)
#define AFX_AGCMEVENTTELEPORT_H__E05B7CDF_25AA_4E94_BD67_D29CA0898494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventTeleportD" )
#else
#pragma comment ( lib , "AgcmEventTeleport" )
#endif
#endif

class AgpmArchlord;


typedef enum	_AgcmEventTeleportCB {
	AGCMTELEPORT_CB_SELECT_TELEPORT_POINT		= 0,
	AGCMTELEPORT_CB_CANNOT_USE_FOR_TRANSFORMING,
	AGCMTELEPORT_CB_CANNOT_USE_FOR_COMBATMODE,
} AgcmEventTeleportCB;

struct ApdEvent;

class AgcmEventTeleport : public AgcModule  
{
private:
	class AgpmCharacter *		m_pcsAgpmCharacter;
	class AgcmCharacter *		m_pcsAgcmCharacter;
	class ApmEventManager *		m_pcsApmEventManager;
	class AgcmEventManager *	m_pcsAgcmEventManager;
	class AgpmEventTeleport *	m_pcsAgpmEventTeleport;
	class AgpmArchlord*			m_pcsAgpmArchlord;

	ApdEvent*			m_pcsEvent;
	ApBase*				m_pcsGenerateBase;

public:
	AgcmEventTeleport();
	virtual ~AgcmEventTeleport();

	BOOL			OnAddModule();
	BOOL			OnInit();
	BOOL			OnDestroy();

	static BOOL		CBExecuteEvent(PVOID pvData, PVOID pvClass, PVOID pvCustData);
	static BOOL		CBReceivedRequestResult(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	static BOOL		CBReceiveTeleportLoading(PVOID pvData, PVOID pvClass, PVOID pvCustData);

	BOOL			RequestTeleport(CHAR *szPointName);
	BOOL			RequestTeleportReturnOnly();

	BOOL			SetCallbackSelectTeleportPoint(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCannotUseForTransforming(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL			SetCallbackCannotUseForCombatMode(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif // !defined(AFX_AGCMEVENTTELEPORT_H__E05B7CDF_25AA_4E94_BD67_D29CA0898494__INCLUDED_)
