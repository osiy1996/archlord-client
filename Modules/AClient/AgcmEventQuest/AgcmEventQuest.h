// AgcmEventQuest.h: interface for the AgcmEventQuest class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_)
#define AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
#ifdef _DEBUG
	#pragma comment(lib, "AgcmEventQuestD.lib")
#else
	#pragma comment(lib, "AgcmEventQuest.lib")
#endif
#endif

typedef enum _eAgcmEventQuestCallback
{
	AGCMEVENTQUEST_CB_GRANT = 0,
} eAgcmEventQuestCallBack;

struct ApdEvent;

class AgcmEventQuest : public AgcModule
{
private:
	class AgpmCharacter		*m_pcsAgpmCharacter;
	class AgpmEventQuest	*m_pcsAgpmEventQuest;
	class AgcmCharacter		*m_pcsAgcmCharacter;
	class AgcmEventManager	*m_pcsAgcmEventManager;
	class AgcmObject		*m_pcsAgcmObject;

public:
	ApdEvent			*m_pcsLastGrantEvent;
	
public:
	AgcmEventQuest();
	virtual ~AgcmEventQuest();

	BOOL OnAddModule();
	
	BOOL SetCallbackGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);

	static BOOL CBEventRequest(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL CBEventGrant(PVOID pData, PVOID pClass, PVOID pCustData);	
};

#endif // !defined(AFX_AGCMEVENTQUEST_H__E0176399_92A7_4D96_9620_E1130248356D__INCLUDED_)
