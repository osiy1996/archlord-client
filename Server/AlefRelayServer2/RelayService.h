/*=============================================================

	RelayService.h

=============================================================*/

#ifndef _RELAY_SERVICE_H_
	#define _RELAY_SERVICE_H_

#include "AuService.h"
#include "AuRegistry.h"
#include "ServerEngine.h"

/****************************************************/
/*		The Definition of Relay Service class		*/
/****************************************************/
//
class RelayService : public AuService
	{
	protected:
		ServerEngine		m_csEngine;
		HANDLE				m_hEventStop;
		HANDLE				m_hEventStopConfirm;
		AuRegistry			m_Registry;

	public:
		RelayService(LPTSTR lpszName, LPTSTR lpszDisplay);
		virtual ~RelayService();

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
extern LPTSTR g_szRelayServiceName;
extern LPTSTR g_szRelayServiceDisplayName;

#endif	//	_RELAY_SERVICE_H_
