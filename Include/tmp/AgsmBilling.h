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
#include "AgsmSystemMessage.h"//JK_�ɾ߼��ٿ�
#ifdef _WEBZEN_BILLING_
#include "WebzenBilling_kor.h"
#endif
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
class AgsmBillingGlobal;//JK_����
class AgsmBillingWebzen;//JK_��������
class CCallBackFunc;
class AgpmBillInfo;//JK_����
class AgsmBillInfo;
class AgpmFactors;
class AgsmSkill;

// �ѱ��� �Ϲ̴� ���� ����� ������ ����.

class AgsmBilling : public AgsModule
	{
	friend CCallBackFunc;

	private :
		static AgsmBilling* m_pInstance;//JK_����

		// Related modules
		AgsmCharacter		*m_pAgsmCharacter;
		AgpmCharacter*		m_pagpmCharacter;//JK_�ɾ߼��ٿ�
		AgsmServerManager2	*m_pAgsmServerManager2;
		AgpmCashMall		*m_pAgpmCashMall;
		AgsmCashMall		*m_pAgsmCashMall;
		AgsmItemManager		*m_pAgsmItemManager;
		AgpmItem			*m_pAgpmItem;
		AgsmItem			*m_pAgsmItem;
		AgsmSkill			*m_pagsmSkill;//JK_����
		AgsmSystemMessage	*m_pagsmSystemMessage;//JK_�ɾ߼��ٿ�



		// Billing server
		AgsmBillingChina	*m_pAgsmBillingChina;
		AgsmBillingJapan	*m_pAgsmBillingJapan;
		AgsdServer			*m_pAgsdServerBilling;
		AgsmBillingGlobal	*m_pAgsmBillingGlobal;//JK_����
		AgsmBillingWebzen	*m_pAgsmBillingWebzen;//JK_��������
		
		ApMemoryPool		m_PacketMemoryPool;
		AuGenerateID		m_GenerateID;
		AgsaBilling			m_Admin;

		AgpmBillInfo*		m_pagpmBillInfo;//JK_����
		AgsmBillInfo*		m_pagsmBillInfo;
		AgpmFactors*		m_pagpmFactors;
		AgpmConfig*			m_pAgpmConfig;	//JK_�ɾ߼��ٿ�


		UINT32				m_ulLastCheckClockClock;
		UINT32				m_ulLastCheckClockForHanGame;
		UINT32			    m_ulLastCheckClockForWebzen;
		UINT32				m_ulLastCheckClockForMidNightShutDown; //JK_�ɾ߼��ٿ�


		//JK_��������
		int					m_nYear;			// ���� ���� �������� ����ϴ� ShopScript ����
		int					m_nYearIndentity;	// ���� ���� �������� ����ϴ� ShopScript ����
		
	public :
		AgsmBilling();
		virtual ~AgsmBilling();

		static AgsmBilling* GetInstance()//JK����
		{
			return m_pInstance;
		};

		//	ApModule inherited
		BOOL	OnAddModule() { return (this->*OnAddModulePtr)(); }
		BOOL	OnAddModuleCn( void );
		BOOL	OnAddModuleJp( void );
		BOOL	OnAddModuleGlobal();//JK_����

		BOOL	OnInit( void ) { return (this->*OnInitPtr)(); }
		BOOL	OnInitCn( void );
		BOOL	OnInitJp( void );
#ifdef _AREA_CHINA_
		BOOL	OnIdle2(UINT32 ulClockCount);
#endif
#ifdef _WEBZEN_BILLING_
		BOOL	OnInitGlobal();
		BOOL	OnIdle2(UINT32 ulClockCount);
#endif
		//	Admin
		AgsdBilling*	Get(INT32 lID);
		AgsdBilling*	Get(CHAR* szAccountID);//JK_����
		BOOL			Add(AgsdBilling *pAgsdBilling);
		BOOL			Add(AgsdBilling* pAgsdBilling, CHAR* AccountID);//JK_����
		BOOL			Remove(INT32 lID);
		BOOL			Remove(CHAR* AccountID);//JK_����

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
		eAGSMBILLING_CONNECT_RESULT ConnectBillingServerGlobal();//JK_����

		BOOL		WaitForConnectServerGlobal();//JK_����

		static BOOL	CheckConnectBillingServer(INT32 lCID, PVOID pClass, UINT32 uDelay, PVOID pvData);

		BOOL		CheckBillingServer();				// operation check
		
		BOOL		OnDisconnect() { return (this->*OnDisconnectPtr)(); }
		BOOL		OnDisconnectKr();
		BOOL		OnDisconnectJp();
		BOOL		OnDisconnectCn();
		
		//	Billing methods
		BOOL	SendGetCashMoney(AgpdCharacter *pAgpdCharacter){ return (this->*SendGetCashMoneyPtr)(pAgpdCharacter); }
		BOOL	SendGetCashMoneyCn(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyJp(AgpdCharacter* pAgpdCharacter);
		BOOL	SendGetCashMoneyGlobal(AgpdCharacter* pAgpdCharacter);//JK_����


		BOOL	SendBuyCashItem(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemKr(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemCn(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemJp(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, stCashItemBuyList &sList);
		BOOL	SendBuyCashItemGlobal(AgpdCharacter *pAgpdCharacter, INT32 lProductID, CHAR *pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID, INT32 lType);//JK_����
		//JK_�ɾ߼��ٿ�
		void	KickOffUnderAge();
		//JK_�ɾ߼��ٿ�
		void	SendMidNightShutDownMessageAll(INT32 nLeftMin);




#ifdef _WEBZEN_BILLING_//JK_����
		BOOL	OnInquireCash(long ReturnCode, CHAR* AccountID, double WCoin, double PCoin );
		//JK_��������
		BOOL	OnInquireCash(long ReturnCode, DWORD AccountGUID, double WCoin );
		BOOL	OnBuyProduct(long ResultCode, CHAR* AccounID, DWORD DeductCashSeq);
		BOOL	OnInquirePersonDeduct(DWORD AccountGUID, DWORD ResultCode);
		BOOL	OnUserStatus(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType);
		//JK_�������� : �������պ���
		BOOL	OnUserStatusWebzen(DWORD AccountGUID, DWORD BillingGUID, DWORD RealEndDate, DOUBLE RestTime, INT32 DeductType);
		//JK_��������
		BOOL	OnBuyProductWebzen(DWORD dwAccountGUID, DWORD dwResultCode, DWORD wdLeftProductCount);
		//JK_��������
		BOOL	OnUseStorage(DWORD AccountGUID, DWORD ResultCode, CHAR* InGameProductID, BYTE PropertyCount, DWORD ProductSeq, DWORD StorageSeq, DWORD StorageItemSeq, STItemProperty* pPropertyList);
		//JK_��������
		BOOL	OnInquireStorageListPageGiftMessage(DWORD AccountGUID, DWORD ResultCode, CHAR StorageType, int NowPage, int TotalPage, int TotalCount, int ListCount, STStorageNoGiftMessage* StorageList);



		void	CheckIn(AgpdCharacter *pAgpdCharacter);
		void	CheckOut(AgpdCharacter *pAgpdCharacter);
		//JK_PC����ݻ���Ȯ��
		void	ReloginWebzen(DWORD dwAccountGUID);


		void	SetSalesZoneScriptVersion(int Year, int YearIndentity);
		//JK_��������
		void	UpdateScriptVersionAllUser(int Year, int YearIdentity);
		BOOL    SendToClientSalesZoneScriptVersion(AgpdCharacter *pAgpdCharacter);
		//JK_��������
		BOOL	SendUseStorageToBilling(AgpdCharacter *pAgpdCharacter, INT32 StorageSeq, INT32 StorageItemSeq);
		//JK_��������
		BOOL	SendBuyCashItemWebzen(AgpdCharacter *pAgpdCharacter, INT32 PackageSeq, INT32 DisplaySeq, INT32 PriceSeq);
		//JK_��������
		BOOL	SendInquireStorageListToBilling(AgpdCharacter *pAgpdCharacter, INT32 NowPage);



		//JK_��������
		static BOOL     CBSendToClientSalesZoneScriptVersion(PVOID pData, PVOID pClass, PVOID pCustData);

#endif
		//	Callback setting method(for result processing)		
		BOOL	SetCallbackGetCashMoney(ApModuleDefaultCallBack pfCallback, PVOID pClass);
		BOOL	SetCallbackBuyCashItem(ApModuleDefaultCallBack pfCallback, PVOID pClass);

		// 2008.01.11. steeple
		// �Ѱ��� ���� �ٲ���. �� �� �� ¥��
#ifdef _AREA_KOREA_
		static BOOL	CBAddServer(PVOID pData, PVOID pClass, PVOID pCustData);// { return (*CBAddServerPtr)(pData, pClass, pCustData); }

		BOOL	OnInitKr( void );
		BOOL	OnAddModuleKr( void );
		eAGSMBILLING_CONNECT_RESULT	ConnectBillingServerKr();
		BOOL	SendGetCashMoneyKr(AgpdCharacter* pAgpdCharacter);
#ifdef _HANGAME_
		BOOL	ConnectHanGameBilling();
		BOOL	GetHanCoin(AgpdCharacter* pcsCharacter);
#endif
		BOOL	BuyCashItem(AgpdCharacter* pcsCharacter, INT32 lProductID, CHAR* pszDesc, INT64 llMoney, UINT64 ullListSeq, INT32 lItemTID);
#endif
	};


#endif