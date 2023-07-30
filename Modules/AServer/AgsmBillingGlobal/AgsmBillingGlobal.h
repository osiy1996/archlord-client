#ifndef _AGSMBILLINGGLOBAL_H
#define _AGSMBILLINGGLOBAL_H

#include "ApDefine.h"
#include "AgsmBilling.h"

struct AgpdBillingGlobal : public ApMemory<AgpdBillingGlobal, 3000>
{
	DWORD AccountGUID;
	AgpdCharacter* pcsCharacter;
	DWORD dwBillingGUID;

	AgpdBillingGlobal()
	{
		AccountGUID = 0;
		pcsCharacter = NULL;
		dwBillingGUID = 0;
	};
};

class AgsmBillingGlobal
{
public:
	AgsmBillingGlobal();
	virtual ~AgsmBillingGlobal();

	ApAdmin m_AdminGUID;

	BOOL	Initialize();
	BOOL	OnIdle();
	
	BOOL ConnectShop();
	BOOL ConnectBilling();

	static void OnLog(char* strLog);
	static void OnInquireCash(PVOID pInquire);
	static void	OnBuyProduct(PVOID pBuyProduct);

	static void OnUserStatus(PVOID UserStatus);
	static void OnInquirePersonDeduct(PVOID InquirePersonDeduct);

	BOOL		InquireCash(CHAR* szAccountID);
	BOOL		BuyProduct(CHAR* AccountID, INT32 ProductID, CHAR* ProductName, UINT64 ListSeq, INT32 Class, INT32 Level, CHAR* szCharName, INT32 ServerIndex, double Price, INT32 lType );

	//AR_TW Start : ´ë¸¸

	BOOL		InquireCashTW(CHAR* szAccountID, INT32 lServerIndex);
	BOOL		BuyProductTW(INT32 ServerIndex, CHAR* AccountID, CHAR* CharName,double DefuctPrice,
						 INT64 ItemNo, INT64 OrderNo, INT32 OrderQuantity, DWORD AccessIP);

	static void OnInquireCastTW(PVOID pInquire);
	static void OnBuyProductTW(PVOID pBuyProduct);

	//AR_TW End

	BOOL	InquirePersonDeduct(DWORD AccountGUID);

	BOOL	AddUser(AgpdCharacter* pcsCharacter, DWORD AccountGUID);
	BOOL	RemoveUser(DWORD AccountGUID);
	AgpdCharacter* GetCharacter(DWORD AccountGUID);
	BOOL	SetBillingGUID(DWORD AccountGUID, DWORD BillingGUID);

	BOOL	Login(DWORD AccountGUID, CHAR* szIP);
	void	LogOut(DWORD AccountGUID);
};

#endif //_AGSMBILLINGGLOBAL_H