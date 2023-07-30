#pragma once

#include "AgpmCashMall.h"
#include "AgsmItem.h"
#include "AgsmItemManager.h"
#include "AgsmCharManager.h"
#include "AgsmBilling.h"

class AgsmSystemMessage;
class CCallBackFunc;
class AgpmReturnToLogin;

class AgsmCashMall : public AgsModule {
	friend CCallBackFunc;

private:
	AgpmCharacter	*m_pcsAgpmCharacter;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;
	AgpmBillInfo	*m_pcsAgpmBillInfo;

	AgsmCharacter	*m_pcsAgsmCharacter;
	AgsmCharManager	*m_pcsAgsmCharManager;
	AgsmItem		*m_pcsAgsmItem;
	AgsmItemManager	*m_pcsAgsmItemManager;
	AgsmBilling		*m_pcsAgsmBilling;
	AgpmReturnToLogin *m_pcsAgpmReturnToLogin;

	AgsmSystemMessage *m_pcsAgsmSystemMessage;
	
public:
	AgsmCashMall();
	virtual ~AgsmCashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnInit();
	virtual BOOL	OnDestroy();

	BOOL	SendBuyResult(AgpdCharacter *pcsCharacter, INT32 lResult);

	static BOOL		CBEnterGame(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestMallProductList(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRequestBuyItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBRefreshCash(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBCheckListVersion(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBResultBuyItem(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBResultCashMoney(PVOID pData, PVOID pClass, PVOID pCustData);
	static BOOL		CBDisableForaWhile(PVOID pData, PVOID pClass, PVOID pCustData);
#ifdef _AREA_CHINA_
	static BOOL		CBDisconnect(PVOID pData, PVOID pClass, PVOID pCustData);
#endif

private:
	BOOL	CheckBuyRequirement(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion, INT32 lType);
	BOOL	ProcessBuyItem(AgpdCharacter *pcsCharacter, INT32 lProductID, UINT8 ucMallListVersion, INT32 lType);
	BOOL	ProcessBuyItemResult(AgpdCharacter *pcsCharacter, AgsdBillingItem *pcsBilling);
	BOOL	SendMallProductList(AgpdCharacter *pcsCharacter, INT32 lTab);
};