//	Item Repair Event Module (client-side)
//		- ������ ������ ���� ó��(Ŭ���̾�Ʈ��)�� ����ϴ� ����̴�.
/////////////////////////////////////////////////////////////

#ifndef	__AGCMEVENTITEMREPAIR_H__
#define	__AGCMEVENTITEMREPAIR_H__

#include "AgpmEventItemRepair.h"

#include "AgcmEventManager.h"
#include "AgcmCharacter.h"

enum EnumAgcmEventItemRepairCB
{
	AGCMEVENT_ITEMREPAIR_CB_GRANT = 0,
};

class AgcmEventItemRepair : public AgcModule {
private:
	AgpmEventItemRepair		*m_pcsAgpmEventItemRepair;
	AgcmEventManager		*m_pcsAgcmEventManager;
	AgcmCharacter			*m_pcsAgcmCharacter;
	AgpmCharacter			*m_pcsAgpmCharacter;

public:
	ApdEvent			*m_pcsLastGrantEvent;

public:
	AgcmEventItemRepair();
	virtual ~AgcmEventItemRepair();

	virtual BOOL OnAddModule();

	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
};

#endif	//__AGCMEVENTITEMREPAIR_H__