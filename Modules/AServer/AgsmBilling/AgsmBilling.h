/*===================================================================

	AgsmBilling.h

===================================================================*/

#ifndef	_AGSM_BILLING_H_
	#define	_AGSM_BILLING_H_

#include "AgsEngine.h"
#include "AgsmServerManager2.h"
#include "AgsmCharacter.h"
#include "AgsdBilling.h"
#include "AgsaBilling.h"
#include "AgsdItem.h"

/****************************************/
/*		The Definition of Constants		*/
/****************************************/
//
const enum eAGSMBILLING_CB
	{
	AGSMBILLING_CB_RESULT_GETMONEY = 0,
	AGSMBILLING_CB_RESULT_BUYITEM,
	};


const enum eAGSMBILLING_CONNECT_RESULT
	{
	AGSMBILLING_CONNECT_RESULT_SUCCESS = 0,
	AGSMBILLING_CONNECT_RESULT_NOT_EXIST_SERVER,
	AGSMBILLING_CONNECT_RESULT_FAIL,
	AGSMBILLING_CONNECT_RESULT_NEED_WAIT,
	};




/************************************************/
/*		The Definition of AgsmBilling class		*/
/************************************************/
//
class AgpmItem;
class AgsmItem;
class AgsmItemManager;
class AgpmCashMall;
class AgsmCashMall;
class AgsmBillingJapan;
class AgsmBillingChina;
class AgsmBillingGlobal;
class CCallBackFunc;
class AgpmBillInfo;
class AgsmBillInfo;
class AgpmFactors;
class AgsmSkill;

// 한국과 북미는 빌링 모듈이 완전히 같다.

class AgsmBilling : public AgsModule
	{
	friend CCallBackFunc;

	private :
		static AgsmBilling* m_pInstance;

		// Related modules
		AgsmCharacter		*m_pAgsmCharacter;
		AgsmServerManager2	*m_pAgsmServerManager2;
		AgpmCashMall		*m_pAgpmCashMall;
		AgsmCashMall		*m_pAgsmCashMall;
		AgsmItemManager		*m_pAgsmItemManager;
		AgpmItem			*m_pAgpmItem;
		AgsmItem			*m_pAgsmItem;
		AgsmSkill			*m_pagsmSkill;

		// Billing server
		AgsmBillingChina	*m_pAgsmBillingChina;
		AgsmBillingJapan	*m_pAgsmBillingJapan;
		AgsmBillingGlobal	*m_pAgsmBillingGlobal;
		AgsdServer			*m_pAgsdServerBilling;
		
		ApMemoryPool		m_PacketMemoryPool;
		AuGenerateID		m_GenerateID;
		AgsaBilling			m_Admin;
		
		AgpmBillInfo*		m_pagpmBillInfo;
		AgsmBillInfo*		m_pagsmBillInfo;
		AgpmFactors*		m_pagpmFactors;

		UINT32				m_ulLastCheckClockClock;
		UINT32				m_ulLastCheckClockForHanGame;
		
	public :
		AgsmBilling();
		virtual ~AgsmBilling();

		static AgsmBilling* GetInstance()
		{
			return m_pInstance;
		};
		#ifdef _AREA_CHINA_
		AuGenerateID&		GetAuGenerateID() { return m_GenerateID; }
		#endif

		//	ApModule inherited
		BOOL	OnAddModule() { return (this->*OnAddModulePtr)(); }
		BOOL	OnAddModuleCn( void );
		BOOL	OnAddModuleJp( void );
		BOOL	OnAddModuleGlobal();

		BOOL	OnInit( void ) { return (this->*OnInitPtr)(); }
		BOOL	OnInitCn( void );
		BOOL	OnInitJp( void );
		BOOL	OnInitGlobal();
		BOOL	OnIdle2(UINT32 ulClockCount);
		
		//	Admin
		AgsdBilling*	Get(INT32 lID);
		AgsdBilling*	Get(CHAR* szAccountID);
		BOOL			Add(AgsdBilling *pAgsdBilling);
		BOOL			Add(AgsdBilling* pAgsdBilling, CHAR* AccountID);
		BOOL			Remove(INT32 lID);
		BOOL			Remove(CHAR* AccountID);

		void	SetOrderSeed();
		
		//	Socket callback point
		static BOOL	DispatchBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
		static BOOL DisconnectBilling(PVOID pvPacket, PVOID pvParam, PVOID pvSocket);
	
	private :
		//	Packet processing
		BOOL	OnReceive(PVOID pvPacket, UINT32 ulNID);
		void	InitFuncPtr( void );

		// Function Pointers
		BOOL	(AgsmBilling::*OnAddModulePtr)( void );
		BOOL	(AgsmBilling::*OnInitPtr)( void );
		BOOL	(AgsmBilling::*WaitForConnectServerPtr)( void );
		void	(AgsmBilling::*BillingSvrDisconnectPtr)( void );
		BOOL	(AgsmBilling::*OnDisconnectPtr)( void );
		BOOL	(AgsmBilling::*SendGetCashMoneyPtr)(AgpdCharacter*);
		BOOL	(AgsmBilling::*SendBuyCashItemPtr)(AgpdCharacter*, INT32, CHAR*, INT64, stCashItemBuyList&);
		eAGSMBILLING_CONNECT_RESULT	(AgsmBilling::*ConnectBillingServerPtr)(void);
		
	public :	
		//	Connection
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServer();

		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerCn();
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerJp();

		static BOOL	CheckConnectBillingServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

		BOOL		CheckBillingServer();				// operation check
		
		BOOL		OnDisconnect() { return (this->*OnDisconnectPtr)(); }
		BOOL		OnDisconnectKr();
		BOOL		OnDisconnectJp();
		BOOL		OnDisconnectCn();
		BOOL		OnDisconnectGlobal();

		//	Billing methods
		BOOL	SendGetCashMoney(AgpdCharacter *pAgpdCharacter){ return (this->*SendGetCashMoneyPtr)(pAgpdCharacter); }
		BOOL	SendGetCashMoneyCn(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyJp(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyGlobal(AgpdCharacter* pAgpdCharacter);

		BOOL	SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemKr(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemCn(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemJp(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemGlobal(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);

		//	Callback setting method(for result processing)		
		BOOL	SetCallbackGetCashMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuyCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// 2008.01.11. steeple
		// 한게임 빌링 바꼈다. 아 초 왕 짜증
#ifdef _AREA_KOREA_
		static BOOL	CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData);// { return (*CBAddServerPtr)(pData, pClass, pCustData); }

		BOOL	OnInitKr( void );
		BOOL	OnAddModuleKr( void );
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerKr();
		BOOL	SendGetCashMoneyKr(AgpdCharacter* pAgpdCharacter);
		BOOL	ConnectHanGameBilling();
		BOOL	GetHanCoin(AgpdCharacter* pcsCharacter);
		BOOL	BuyCashItem(AgpdCharacter* pcsCharacter, INT32 lProductID, CHAR* pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID);
#endif
#ifdef _AREA_GLOBAL_
		BOOL	OnInquireCash(long ReturnCode, CHAR* AccountID, double WCoin, double PCoin );
		BOOL	OnBuyProduct(long ResultCode, CHAR* AccounID, DWORD DeductCashSeq);
		BOOL	OnInquirePersonDeduct(DWORD AccountGUID, DWORD ResultCode);
		BOOL	OnUserStatus(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType);
		void	CheckIn(AgpdCharacter *pAgpdCharacter);
		void	CheckOut(AgpdCharacter *pAgpdCharacter);
#endif //_AREA_GLOBAL_

	};


#endif