#ifndef __AGSMBILLINGCHINA_H__
#define __AGSMBILLINGCHINA_H__

#include "AgsmCharacter.h"
#include "bsipmsg.h"

#include <map>

#define SNDA_BILLING_UPGRADE_V1_6_5
// **************************************************************************
class AgsdBilling;
class AgsmItemManager;
class AgsmCashMall;
class AgpmCashMall;
class AgpmItem;
class AgsmItem;
class AgsmBilling;
class AgsdBillingItem;

typedef enum BSIP_ERROR {
	BSIP_ERR_NONE					= 0,
	BSIP_ERR_LOAD_LIBRARY			= 500,
	BSIP_ERR_GET_FUNC_ADDR,
	BSIP_ERR_SET_CALLBACK_FUNC,
	BSIP_ERR_GS_INITIALIZE,
	BSIP_ERR_SUCCESS,
};

class CBillingBridge
{
private:
	class Lock
	{
	public:
		Lock(CRITICAL_SECTION& lock) : m_lock(lock) { EnterCriticalSection(&m_lock); }
		~Lock() { LeaveCriticalSection(&m_lock); }

	private:
		CRITICAL_SECTION& m_lock;
	};

public:
	CBillingBridge();
	~CBillingBridge();

	void SetCharManager(AgpmCharacter* pmChar, AgsmCharacter* smChar);
	void SetItemManager(AgpmItem* pmItem, AgsmItem* smItem, AgsmItemManager* itemManager);
	void SetCashMallManager(AgpmCashMall* pmMall, AgsmCashMall* smMall);
	void SetBilling(AgsmBilling* pBilling);

	void Add(char* uid, AgsdBilling* billing);
	void Remove(char* uid);
	void AddItem(char* uid, AgsdBillingItem* pBillingItem);
	void RemoveItem(char* uid);

	AgsdBilling* Find(char* uid);
	AgsdBillingItem* FindItem(char* uid);
	
	AgsmCharacter* GetAgsmChar()	 { return m_pAgsmCharacter; }
	AgpmCharacter* GetAgpmChar()	 { return m_pAgpmCharacter; }
	AgsmCashMall*  GetAgsmCashMall() { return m_pAgsmCashMall; }
	AgpmCashMall*  GetAgpmCashMall() { return m_pAgpmCashMall; }
	AgsmBilling*   GetAgsmBilling()	 { return m_pAgsmBilling; }
	AgpmItem*	   GetAgpmItem()	 { return m_pAgpmItem; }
	AgsmItem*	   GetAgsmItem()	 { return m_pAgsmItem; }

	AgsmItemManager* GetAgsmItemManager() { return m_pAgsmItemManager; }

private:
	AgsmItemManager* m_pAgsmItemManager;

	AgsmBilling*	m_pAgsmBilling;
	AgsmCharacter*	m_pAgsmCharacter;
	AgpmCharacter*	m_pAgpmCharacter;

	AgsmCashMall*	m_pAgsmCashMall;
	AgpmCashMall*	m_pAgpmCashMall;
	
	AgpmItem*		m_pAgpmItem;
	AgsmItem*		m_pAgsmItem;

	typedef std::map<string, AgsdBilling*> StrBillingMap;
	typedef std::map<string, AgsdBillingItem*> StrBillingItemMap;
	StrBillingMap		m_map;
	StrBillingItemMap	m_BillItemMap;

	CRITICAL_SECTION	m_cs;
};

extern CBillingBridge g_billingBridge;

// **************************************************************************

#define  AGSM_SNDA_BILLING_CONFIG_FILE		_T("../billingdata/bsipconfig.ini")

#define  AGSM_SNDA_BILLING_SERVER_ID_ITEM		 "server_id"
#define  AGSM_SNDA_BILLING_AREA_ID_ITEM			 "area_id"
#define  AGSM_SNDA_BILLING_GROUP_ID_ITEM		 "group_id"
#define  AGSM_SNDA_BILLING_SECTION_VALUE		 "applicationinfo"

class CBillAPI;
class AgsmCharManager;
class AgpmReturnToLogin;

class AgsmBillingChina
{
public:
	AgsmBillingChina();
	//AgsmBillingChina(CBillAPI& billAPI);
	~AgsmBillingChina();

	bool Init(ApModule *pModule = NULL);

	BSIP_ERROR	m_eBsipErr;
	int			m_err;
	UINT32		m_ulInitializedClock;

public:
	// 처음 로그인할때, 계정의 금액 정보를 가져온다.
	int SendAccountAuthen(char* account, char* ip, AgsdBilling *billing);
	// 아이템 구매, AccountUnlock과 쌍으로 불려야한다.
	int SendAccountLock(char* account, int itemID, int itemPrice, AgsdBillingItem* pBillingItem);
	// 아이템 구매 완료
	int SendAccountUnlock(int itemID, int itemPrice, GSBsipAccountLockResDef* pRes);

	void WriteErrorLog();
	void CheckConnection();

	// log
	void SomethingWrong(char* log);
	void AccountAuthenLog(char* account, char* ip, char* sessid, char* orderid);
	void AccountAuthenResLog(char* account, int balance);
	void AccountLockLog(char* account, int itemID, int itemPrice, char* sessid, char* orderid, char* contextid);
	void AccountLockResLog(char* account, char* ip, int itemID, int itemPrice, char* orderid);
	void AccountUnlockLog(char* account, int itemID, int itemPrice, char* sessid, char* orderid, char* contextid);
public:
	//BOOL SendGetCashMoney(AgpdCharacter *pAgpdCharacter);
	//BOOL SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR* /*pszDesc*/, INT64 llMoney, UINT64 /*ullListSeq*/);

	BOOL SettingSNDAParam(const CHAR* szFileName);

private:
	CBillAPI&		m_Sndabilling;
	//AgsmCharacter	*m_pAgsmCharacter;

	AgsmCharManager		*m_pAgsmCharManager;
	AgpmReturnToLogin	*m_pAgpmReturnToLogin;

	// no assign/copy
	AgsmBillingChina(const AgsmBillingChina& rhs);
	const AgsmBillingChina& operator=(const AgsmBillingChina& rhs);

private:
	ApModule* m_pApModule;
	bool IsConnectedBSIP();

	/************************************************************************/
	/* SNDA와 맞춰줘야하는 수치                                             */
	/************************************************************************/
	INT32		m_lServerIDItem;
	INT32		m_lAreaIDItem;
	INT32		m_lGroupIDItem;

private:
	char	m_logFileName[MAX_PATH];
};

#endif