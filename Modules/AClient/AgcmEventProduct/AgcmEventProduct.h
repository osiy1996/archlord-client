/*===================================================================

	AgcmEventProduct.h
	
===================================================================*/

#ifndef _AGCMEVENT_PRODUCT_H_
	#define _AGCMEVENT_PRODUCT_H_

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventProductD.lib")
#else
	#pragma comment(lib, "AgcmEventProduct.lib")
#endif
#endif

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
enum eAGCMEVENT_PRODUCT_CB
	{
	AGCMEVENT_PRODUCT_CB_GRANT = 0,
	};

struct ApdEvent;

/************************************************/
/*		The Definition of AgcmEventProduct		*/
/************************************************/
//
class AgcmEventProduct : public AgcModule
	{
	private:
		class AgpmCharacter		*m_pAgpmCharacter;
		class AgpmEventProduct	*m_pAgpmEventProduct;
		class AgcmCharacter		*m_pAgcmCharacter;
		class AgcmEventManager	*m_pAgcmEventManager;

	public:
		AgcmEventProduct();
		virtual ~AgcmEventProduct();

		// ApModule inherited
		BOOL OnAddModule();
		
		// Callback setting
		BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// Callbacks
		static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
		static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
		
		//
		BOOL	SendLearnSkill(ApdEvent *pApdEvent, INT32 lSkillTID);
		BOOL	SendBuyCompose(ApdEvent *pApdEvent, INT32 lComposeID);
	};

#endif
