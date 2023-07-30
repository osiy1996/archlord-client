/*====================================================

	AgcmEventRefinery.h

====================================================*/

#ifndef _AGCM_EVENT_REFINERY_H_
	#define _AGCM_EVENT_REFINERY_H_

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventRefineryD.lib")
#else
	#pragma comment(lib, "AgcmEventRefinery.lib")
#endif
#endif

#include <AgcModule/AgcModule.h>

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGCMEVENTREFINERY_CB
	{
	AGCMEVENTREFINERY_CB_GRANT = 0,
	};

struct ApdEvent;

/********************************************************/
/*		The Definition of AgcmEventRefinery class		*/
/********************************************************/
//
class AgcmEventRefinery : public AgcModule
	{
	private:
		class AgpmCharacter		*m_pAgpmCharacter;
		class AgpmEventRefinery	*m_pAgpmEventRefinery;
		class AgcmCharacter		*m_pAgcmCharacter;
		class AgcmEventManager	*m_pAgcmEventManager;

	public:
		ApdEvent			*m_pLastestEvent;

	public:
		AgcmEventRefinery();
		virtual ~AgcmEventRefinery();

		// ... ApModule inherited
		BOOL OnAddModule();
		
		// ... Callback setting
		BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// ... Callbacks
		static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
	};

#endif
