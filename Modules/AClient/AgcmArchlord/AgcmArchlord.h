#pragma once

#include <AgcModule/AgcModule.h>

struct ApdEvent;
class AgpdCharacter;

class AgcmArchlord : public AgcModule 
{
private:
	class AgpmArchlord *m_pcsAgpmArchlord;
	class AgpmCharacter *m_pcsAgpmCharacter;
	class AgcmCharacter *m_pcsAgcmCharacter;
	class AgcmEventManager *m_pcsAgcmEventManager;

public:
	AgcmArchlord();
	virtual ~AgcmArchlord();

//	virtual BOOL OnInit();
	virtual BOOL OnAddModule();
//	virtual BOOL OnIdle(UINT32 ulClockCount);	
//	virtual BOOL OnDestroy();

	static BOOL CBOperationSetArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBOperationCancelArchlord(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBRequestEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL OperationSetArchlord(char* szID);
	BOOL SendRequestEvent(AgpdCharacter *pcsCharacter, ApdEvent *pApdEvent);
};