#ifndef	__AGCMEVENTSKILLMASTER_H__
#define	__AGCMEVENTSKILLMASTER_H__

#include <AgcModule/AgcModule.h>

#if _MSC_VER < 1300
#ifdef	_DEBUG
#pragma comment ( lib , "AgcmEventSkillMasterD" )
#else
#pragma comment ( lib , "AgcmEventSkillMaster" )
#endif
#endif

typedef enum	_AgcmEventSkillCB {
	AGCMEVENT_SKILL_CB_RESULT_SKILL_UPGRADE		= 0,
	AGCMEVENT_SKILL_CB_RESPONSE_GRANT,
} AgcmEventSkillCB;

struct ApdEvent;

class AgcmEventSkillMaster : public AgcModule {
private:
	class AgpmCharacter			*m_pcsAgpmCharacter;
	class AgpmEventSkillMaster	*m_pcsAgpmEventSkillMaster;
	class AgcmCharacter			*m_pcsAgcmCharacter;
	class AgcmItem				*m_pcsAgcmItem;
	class AgcmSkill				*m_pcsAgcmSkill;
	class AgcmEventManager		*m_pcsAgcmEventManager;

public:
	AgcmEventSkillMaster();
	virtual ~AgcmEventSkillMaster();

	BOOL		OnAddModule();

	BOOL		SendRequestEvent(ApdEvent *pcsEvent);
	BOOL		SendBuySkillBook(ApdEvent *pcsEvent, INT32 lSkillTID);
	BOOL		SendLearnSkill(ApdEvent *pcsEvent, INT32 lSkillTID);
	BOOL		SendRequestSkillUpgrade(ApdEvent *pcsEvent, INT32 lSkillID, INT32 lUpgradePoint);
	BOOL		SendRequestSkillInitialize(ApdEvent *pcsEvent);

	static BOOL	CBExecuteEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	static BOOL	CBResponseSkillUpgrade(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL	CBResponseEvent(PVOID pData, PVOID pClass, PVOID pCustData);

	BOOL		SetCallbackResultSkillUpgrade(ApModuleDefaultCallBack pfCallback, PVOID pClass);
	BOOL		SetCallbackResponseGrant(ApModuleDefaultCallBack pfCallback, PVOID pClass);
};

#endif	//__AGCMEVENTSKILLMASTER_H__