#pragma once

/**************************************************************************************************

작성일: 2008-07-08
작성자: 문상현 (youngmoon@webzen.co.kr)

설명: 샵 서버를 사용하는 클라이언트에게 수신된 패킷을 전달해 주기 위한 이벤트 객체 
      이벤트를 받는쪽(샵클라이언트)에서 상속받아 사용해야 한다.

**************************************************************************************************/

#include "ClientSession.h"
#include "protocol_shop.h"

class CShopEventHandler_AR : public CClientSession
{
public:
	CShopEventHandler_AR(void);
	virtual  ~CShopEventHandler_AR(void);

	// 초기화 함수 (프로그램 실행하고 한번만 사용해야 한다.)
	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);

	// 캐쉬 및 포인트를 조회한다.
	bool InquireCash(BYTE ViewType, 
					 DWORD AccountSeq, 
					 char AccountID[MAX_ACCOUNT_LENGTH], 
					 DWORD GameCode, 
					 bool SumOnly);

	// 물품을 구매한다.
	bool BuyProduct(DWORD AccountSeq, 
					char AccountID[MAX_ACCOUNT_LENGTH], 
					DWORD GameCode, 
					DWORD ProductSeq, 
					WCHAR ProductName[MAX_PRODUCTNAME_LENGTH], 
					DWORD InGamePurchaseSeq, 
					int Class, 
					int Level, 
					WCHAR CharName[MAX_CHARNAME_LENGTH], 
					int ServerIndex, 
					DWORD SalesZone, 
					double DeductPrice, 
					char DeductType, 
					int CashTypeCode);

	// 물품을 선물한다.
	bool GiftProduct(DWORD SenderSeq, 
					 char SenderID[MAX_ACCOUNT_LENGTH], 
					 DWORD ReceiverSeq, 
					 char ReceiverID[MAX_ACCOUNT_LENGTH], 
					 WCHAR Message[MAX_MESSAGE_LENGTH], 
					 DWORD GameCode, 
					 DWORD ProductSeq, 
					 WCHAR ProductName[MAX_PRODUCTNAME_LENGTH], 
					 DWORD InGamePurchaseSeq, 
					 int SendClass, 
					 int SendLevel, 
					 WCHAR SendCharName[MAX_CHARNAME_LENGTH], 
					 int SendServerIndex, 
					 int RevClass, 
					 int RevLevel, 
					 WCHAR RevCharName[MAX_CHARNAME_LENGTH], 
					 int RevServerIndex, 
					 DWORD SalesZone, 
					 double DeductPrice, 
					 char DeductType, 
					 int CashTypeCode);

protected:
	void OnReceive( PBYTE buffer, int size );	

	// 빌링서버의 연결 성공 여부 (새로 연결되면 현재 차감이 필요한 모든 유저의 정보를 다시 로그인해야한다.)
	//		success: 연결 성공 여부
	//		error: 에러코드
	void OnConnect(bool success, DWORD error);

	// 빌링서버로 보낸 데이타 전송 여부
	//		size: 전송된 패킷 사이즈
	void OnSend( int size );

	//빌링서버와 세션이 끊겼을때 (빌링서버가 죽은것으로 처리 해야한다.)	
	//		error: 에러코드
	void OnClose( DWORD error );	

	// 캐쉬 조회의 결과
	virtual void OnInquireCash(DWORD AccountSeq, 
							   char AccountID[MAX_ACCOUNT_LENGTH], 
							   double CashSum, 
							   double PointSum, 
							   ShopProtocol::STCashDetail_GB Detail[], 
							   long nCashInfoCount, 
							   long ResultCode) = 0;

	// 상품 구매의 결과
	virtual void OnBuyProduct(DWORD AccountSeq, 
							  char AccountID[MAX_ACCOUNT_LENGTH], 
							  DWORD DeductCashSeq, 
							  DWORD InGamePurchaseSeq, 
							  long ResultCode) = 0;

	// 상품 선물의 결과
	virtual void OnGiftProduct(DWORD AccountID, 
							   DWORD ReceiverSeq, 
							   DWORD DeductCashSeq, 
							   long ResultCode) = 0;

	
	// 서버에 연결한 결과를 알려줍니다.
	virtual void OnNetConnect(bool success, DWORD error) = 0;

	// 서버에 데이타를 전송한 결과를 알려줍니다.
	virtual void OnNetSend( int size ) = 0;

	// 서버와 접속이 종료되었을때 발생합니다.
	virtual void OnNetClose( DWORD error ) = 0;	

	// 로그를 써야할때 보내는 이벤트 메소드
	virtual void WriteLog(char* szMsg) = 0;

private:
	// 로그를 사용한다.
	void WriteLog(const char* szFormat, ...);	
};
