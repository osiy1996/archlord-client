#pragma once

/**************************************************************************************************

�ۼ���: 2008-07-08
�ۼ���: ������ (youngmoon@webzen.co.kr)

����: �� ������ ����ϴ� Ŭ���̾�Ʈ���� ���ŵ� ��Ŷ�� ������ �ֱ� ���� �̺�Ʈ ��ü 
      �̺�Ʈ�� �޴���(��Ŭ���̾�Ʈ)���� ��ӹ޾� ����ؾ� �Ѵ�.

**************************************************************************************************/

#include "ClientSession.h"
#include "protocol_shop.h"

class CShopEventHandler_AR : public CClientSession
{
public:
	CShopEventHandler_AR(void);
	virtual  ~CShopEventHandler_AR(void);

	// �ʱ�ȭ �Լ� (���α׷� �����ϰ� �ѹ��� ����ؾ� �Ѵ�.)
	bool CreateSession(WBANetwork::LPFN_ErrorHandler errhandler = NULL);

	// ĳ�� �� ����Ʈ�� ��ȸ�Ѵ�.
	bool InquireCash(BYTE ViewType, 
					 DWORD AccountSeq, 
					 char AccountID[MAX_ACCOUNT_LENGTH], 
					 DWORD GameCode, 
					 bool SumOnly);

	// ��ǰ�� �����Ѵ�.
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

	// ��ǰ�� �����Ѵ�.
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

	// ���������� ���� ���� ���� (���� ����Ǹ� ���� ������ �ʿ��� ��� ������ ������ �ٽ� �α����ؾ��Ѵ�.)
	//		success: ���� ���� ����
	//		error: �����ڵ�
	void OnConnect(bool success, DWORD error);

	// ���������� ���� ����Ÿ ���� ����
	//		size: ���۵� ��Ŷ ������
	void OnSend( int size );

	//���������� ������ �������� (���������� ���������� ó�� �ؾ��Ѵ�.)	
	//		error: �����ڵ�
	void OnClose( DWORD error );	

	// ĳ�� ��ȸ�� ���
	virtual void OnInquireCash(DWORD AccountSeq, 
							   char AccountID[MAX_ACCOUNT_LENGTH], 
							   double CashSum, 
							   double PointSum, 
							   ShopProtocol::STCashDetail_GB Detail[], 
							   long nCashInfoCount, 
							   long ResultCode) = 0;

	// ��ǰ ������ ���
	virtual void OnBuyProduct(DWORD AccountSeq, 
							  char AccountID[MAX_ACCOUNT_LENGTH], 
							  DWORD DeductCashSeq, 
							  DWORD InGamePurchaseSeq, 
							  long ResultCode) = 0;

	// ��ǰ ������ ���
	virtual void OnGiftProduct(DWORD AccountID, 
							   DWORD ReceiverSeq, 
							   DWORD DeductCashSeq, 
							   long ResultCode) = 0;

	
	// ������ ������ ����� �˷��ݴϴ�.
	virtual void OnNetConnect(bool success, DWORD error) = 0;

	// ������ ����Ÿ�� ������ ����� �˷��ݴϴ�.
	virtual void OnNetSend( int size ) = 0;

	// ������ ������ ����Ǿ����� �߻��մϴ�.
	virtual void OnNetClose( DWORD error ) = 0;	

	// �α׸� ����Ҷ� ������ �̺�Ʈ �޼ҵ�
	virtual void WriteLog(char* szMsg) = 0;

private:
	// �α׸� ����Ѵ�.
	void WriteLog(const char* szFormat, ...);	
};
