#if !defined(__AGCMEVENTCHARCUSTOMIZE_H__)
#define	__AGCMEVENTCHARCUSTOMIZE_H__

#include <AgcModule/AgcModule.h>

typedef enum	_AgcmEventCharCustomizeCBID {
	AGCMEVENTCHARCUSTOMIZE_CB_GRANT_EVENT		= 0,
} AgcmEventCharCustomizeCBID;

class AgcmEventCharCustomize : public AgcModule {
private:
	class AgpmCharacter				*m_pcsAgpmCharacter;
	class AgpmEventCharCustomize	*m_pcsAgpmEventCharCustomize;
	class AgcmCharacter				*m_pcsAgcmCharacter;
	class AgcmEventManager			*m_pcsAgcmEventManager;

public:
	AgcmEventCharCustomize();
	virtual ~AgcmEventCharCustomize();

	BOOL	OnAddModule();

	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBEventCharCustomize(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL	SetCallbackGrantEvent(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTCHARCUSTOMIZE_H__