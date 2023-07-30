#pragma once

#include <AgcModule/AgcModule.h>

class AgpmFactors;
class AgpdItem;
class AgpmItem;
class AgpmCashMall;
class AgcmCharacter;
class AgcmItem;

class AgcmCashMall : public AgcModule {
private:
	AgpmFactors		*m_pcsAgpmFactors;
	AgpmItem		*m_pcsAgpmItem;
	AgpmCashMall	*m_pcsAgpmCashMall;
	AgcmCharacter	*m_pcsAgcmCharacter;
	AgcmItem		*m_pcsAgcmItem;

public:
	AgcmCashMall();

	virtual BOOL	OnAddModule();
	virtual BOOL	OnDestroy();

	AgpdItem*	CreateDummyItem(INT32 lTID, INT32 lCount);

private:
	BOOL		DestroyDummyItem(AgpdItem *pcsItem);
	BOOL		DestroyAllDummyItem();

public:
	BOOL	SendPacketRequestMallList(INT32 lTab);
#ifdef _AREA_GLOBAL_
	BOOL	SendPacketBuyProduct(INT32 lProductID, INT32 lType);
#else
	BOOL	SendPacketBuyProduct(INT32 lProductID);
#endif
	BOOL	SendPacketRefreshCash();
	BOOL	SendPacketCheckListVersion();

	static BOOL	CBUpdateMallList(PVOID pData, PVOID pClass, PVOID pCustData);
};