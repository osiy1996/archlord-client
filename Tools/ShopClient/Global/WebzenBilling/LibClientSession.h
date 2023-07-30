#pragma once

/**************************************************************************************************

작성일: 2008-07-10
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 라이브러리의 세션관련 작업을 하는 객체

**************************************************************************************************/

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_64_MB_d.lib")
	#else
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_64_MB.lib")
	#endif
#else
	#ifdef _DEBUG
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_MB_d.lib")
	#else
		#pragma  comment(lib, "../_lib/ClientStub_VS2005_NOSP_MB.lib")
	#endif
#endif

#include <Winsock2.h>
#include <Windows.h>
#include <WBANetwork.h>
#include "../ClientStub/BillEventHandler.h"
#include "../ClientStub/ShopEventHandler_AR.h"

#include "WebzenBilling.h"

//////////////////////////////////////////////////////////////////////////
// Shop
class CLibClientSessionShop : public CShopEventHandler_AR
{
public:
	CLibClientSessionShop(void);
	virtual  ~CLibClientSessionShop(void);

	CFunction_OnInquireCash	WZSHOP_OnInquireCash;
	CFunction_OnBuyProduct	WZSHOP_OnBuyProduct;
	CFunction_OnLog			WZSHOP_OnLog;

	eWZConnect	m_bConnect;
	DWORD		m_nLastError;

protected:
	void WriteLog(char* szMsg);
	void OnNetConnect(bool success, DWORD error);
	void OnNetSend( int size );
	void OnNetClose( DWORD error );	
	
	void OnGiftProduct(DWORD SenderSeq, DWORD ReceiverSeq, DWORD DeductCashSeq, long ResultCode);
	void OnInquireCash(DWORD AccountSeq, CHAR* AccountID, double CashSum, double PointSum, ShopProtocol::STCashDetail_GB Detail[], long nCashInfoCount ,long ResultCode);	
	void OnBuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD DeductCashSeq, DWORD InGamePurchaseSeq, long ResultCode);
};


// Billing
class CLibClientSessionBilling : public CBillEventHandler
{
public:
	CLibClientSessionBilling(void);
	virtual  ~CLibClientSessionBilling(void);

	eWZConnect	m_bConnect;
	DWORD		m_nLastError;

	CFunction_OnLog					WZBILLING_OnLog;
	CFunction_OnInquirePCRoomPoint	WZBILLING_OnInquirePCRoomPoint;
	CFunction_OnInquireMultiUser	WZBILLING_OnInquireMultiUser;
	CFunction_OnUserStatus			WZBILLING_OnUserStatus;
	CFunction_OnInquirePersonDeduct	WZBILLING_OnInquirePersonDeduct;

protected:		
	void WriteLog(char* szMsg);
	void OnNetConnect(bool success, DWORD error);
	void OnNetSend( int size );
	void OnNetClose( DWORD error );	

	void OnUserStatus(DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode);
	void OnInquireMultiUser(long AccountID, long RoomGUID, long Result);
	void OnInquirePCRoomPoint(long AccountID, long RoomGUID, long GameCode, long ResultCode);
	void OnInquirePersonDeduct(long AccountGUID, long GameCode, long ResultCode);

private:
	CRITICAL_SECTION m_cs;

};


