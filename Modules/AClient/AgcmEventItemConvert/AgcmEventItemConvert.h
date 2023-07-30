#ifndef	__AGCMEVENTITEMCONVERT_H__
#define	__AGCMEVENTITEMCONVERT_H__

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventItemConvertD" )
#else
#pragma comment ( lib , "AgcmEventItemConvert" )
#endif
#endif

#include <AgcModule/AgcModule.h>

typedef enum	_eAgcmEventItemConvertCB {
	AGCMEVENT_ITEMCONVERT_CB_RECEIVE_GRANT			= 0,
} AgcmEventItemConvertCB;

class AgcmEventItemConvert : public AgcModule {
private:
	class AgpmCharacter			*m_pcsAgpmCharacter;
	class AgpmItemConvert		*m_pcsAgpmItemConvert;
	class AgpmEventItemConvert	*m_pcsAgpmEventItemConvert;
	class AgcmCharacter			*m_pcsAgcmCharacter;
	class AgcmEventManager		*m_pcsAgcmEventManager;

public:
	AgcmEventItemConvert();
	virtual ~AgcmEventItemConvert();

	BOOL			OnAddModule();

	static BOOL		CBEventItemConvert(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL		CBReceiveGrant(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBReceiveReject(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL			SetCallbackEventGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTITEMCONVERT_H__