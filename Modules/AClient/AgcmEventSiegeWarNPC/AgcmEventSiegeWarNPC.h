#pragma once

#include <AgcModule/AgcModule.h>

class AgcmEventSiegeWarNPC : public AgcModule {
public:
	AgcmEventSiegeWarNPC();
	virtual ~AgcmEventSiegeWarNPC();

	BOOL	OnAddModule();

public:
	class AgpmCharacter			*m_pcsAgpmCharacter;
	class AgpmEventSiegeWarNPC	*m_pcsAgpmEventSiegeWarNPC;
	class AgcmEventManager		*m_pcsAgcmEventManager;
	class AgcmCharacter			*m_pcsAgcmCharacter;

	static BOOL	CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);
};