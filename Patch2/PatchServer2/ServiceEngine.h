/*=============================================================

	ServiceEngine.h

=============================================================*/

#ifndef _SERVICE_ENGINE_H_
	#define _SERVICE_ENGINE_H_

#include "AuService.h"
#include "AuRegistry.h"
#include "ServerEngine.h"

extern const GUID	g_guidApp;

/****************************************************/
/*		The Definition of Service Engine class		*/
/****************************************************/
//
class ServiceEngine : public AuService
	{
	protected:
		ServerEngine		m_csEngine;
		HANDLE				m_hEventStop;
		HANDLE				m_hEventStopConfirm;
		AuRegistry			m_Registry;

	public:
		ServiceEngine(LPTSTR lpszName, LPTSTR lpszDisplay);
		virtual ~ServiceEngine();

		virtual BOOL OnInit(DWORD *pdwStatus);
		virtual void OnStop();
		virtual void OnPause();
		virtual void OnContinue();

		virtual void Run();
		virtual void OnInstall();
		virtual void OnUninstall();
	};

/****************************************/
/*		The Definition of Global		*/
/****************************************/
//
extern LPTSTR g_szServiceEngineName;
extern LPTSTR g_szServiceEngineDisplayName;

#endif	//	_RELAY_SERVICE_H_
