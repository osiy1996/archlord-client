// ShopClient.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <Windows.h>
#include <WinSock2.h>
#include "ShopClient.h"

#pragma comment(lib, "Ws2_32.lib")

bool bInquire = false;
bool bBuyProduct = false;

long BILLING_dwInquireResult = -1;
bool BILLING_bLogin = false;
long BILLING_dwBillingGUID = -1;

//////////////////////////////////////////////////////////////////////////
//
const DWORD GameCode = 417; // 아크로드 글로벌
const DWORD SalesZone = 428; // 아크로드 글로벌 - 게임
const DWORD ServerType = 686; // 아크로드 글로벌

CWebzenBilling* m_pWZBilling = NULL;

void OnInquire(void* Inquire)
{
	stInquire* pInquire = (stInquire*)Inquire;
	printf("OnInquire\n");
	printf("Result = %d\n", pInquire->ResultCode);
	printf("AccountSeq = %d\n", pInquire->AccountSeq);
	printf("AccountID = %s\n", pInquire->AccountID);
	printf("WCoinSum = %d, PCoinSum = %d \n", (int)pInquire->WCoinSum, (int)pInquire->PCoinSum);

	bInquire = true;
}

void OnLog(char* strLog)
{
	printf("%s\n", strLog);
}

void OnBuyProduct(void* BuyProduct)
{
	stBuyProduct* pBuyProduct = (stBuyProduct*)BuyProduct;
	printf("ResultCode = %d\n", pBuyProduct->ResultCode);
	printf("AccountSeq = %d\n", pBuyProduct->AccountSeq);
	printf("AccountID = %s\n", pBuyProduct->AccountID);
	printf("DeductCashSeq = %d\n", pBuyProduct->DeductCashSeq);
	printf("InGamePurchaseSeq = %d\n", pBuyProduct->InGamePurchaseSeq);	

	bBuyProduct = true;
}

/*void OnInquirePCRoomPoint(PVOID InquirePCRoomPoint)
{
	stInquirePCRoomPoint* pInquirePCRoomPoint = (stInquirePCRoomPoint*)InquirePCRoomPoint;

}*/

void OnUserStatus(PVOID UserStatus)
{
	stUserStatus* pUserStatus = (stUserStatus*)UserStatus;

	pUserStatus->dwAccountGUID;
	pUserStatus->dwBillingGUID;
	pUserStatus->RealEndDate;
	pUserStatus->EndDate;
	pUserStatus->dRestPoint;
	pUserStatus->dRestTime;
	pUserStatus->nDeductType;
	pUserStatus->nAccessCheck;
	pUserStatus->nResultCode;

	if(pUserStatus->nResultCode == 0 && pUserStatus->nAccessCheck == 0 && pUserStatus->nDeductType != 0)
	{
		printf("Success\n");

		BILLING_dwBillingGUID = pUserStatus->dwBillingGUID;
		BILLING_bLogin = true;
	}
}

/*void OnInquireMultiUser(PVOID InquireMultiUser)
{
	stInquireMultiUser* pInquireMultiUser = (stInquireMultiUser*)InquireMultiUser;

}*/

void OnInquirePersonDeduct(PVOID InquirePersonDeduct)
{
	stInquirePersonDeduct* pInquirePersonDeduct = (stInquirePersonDeduct*)InquirePersonDeduct;

	//pInquirePersonDeduct->AccountGUID;
	//pInquirePersonDeduct->GameCode;
	
	BILLING_dwInquireResult = pInquirePersonDeduct->ResultCode;
	

	if(BILLING_dwInquireResult == 1 && BILLING_bLogin == false)
	{
		m_pWZBilling->UserLogin(71618, inet_addr("127.0.0.1"), 0, GameCode, ServerType);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//////////////////////////////////////////////////////////////////////////////////////
	//
	m_pWZBilling = new CWebzenBilling;
	m_pWZBilling->SetCallbackOnLog(OnLog);
	//m_pWZBilling->SetCallbackOnInquirePCRoomPoint(OnInquirePCRoomPoint);
	m_pWZBilling->SetCallbackOnUserStatus(OnUserStatus);
	//m_pWZBilling->SetCallbackOnInquireMultiUser(OnInquireMultiUser);
	m_pWZBilling->SetCallbackOnInquirePersonDeduct(OnInquirePersonDeduct);
	m_pWZBilling->Initialize();

	while(true)
	{
		Sleep(2000);

		if(m_pWZBilling->GetStatus() == eConnecting)
			continue;

		if(m_pWZBilling->GetStatus() == eNotConnect)
		{
			m_pWZBilling->Connect("218.234.76.11", 45610); // 테스트 Billing 서버
			continue;
		}

		if(BILLING_bLogin == false)
		{
			m_pWZBilling->InquirePersonDeduct(71618, GameCode);
		}
		else
		{

		}
	}

	//////////////////////////////////////////////////////////////////////////////////////
	//
	CWebzenShop m_pWZShop;
	m_pWZShop.SetCallbackOnLog(OnLog);
	m_pWZShop.SetCallbackOnInquireCash(OnInquire);
	m_pWZShop.SetCallbackOnBuyProduct(OnBuyProduct);

	m_pWZShop.Initialize();

	while(TRUE)
	{
		Sleep(100);

		if(m_pWZShop.GetStatus() == eConnecting)
			continue;
		else if(m_pWZShop.GetStatus() == eNotConnect)
		{
			m_pWZShop.Connect("218.234.76.11", 45611); // 테스트 샾서버
			//m_pWZShop.Connect("172.18.1.131", 45611); // 리얼 샾서버
		}
		else
			break;		
	}

	m_pWZShop.InquireCash(1, 0, "ehtest01", GameCode, false);

	while(!bInquire)
	{
		Sleep(100);
	}

	DWORD InGamePurchaseSeq = 12340000;
	BOOL IsPPCardCash = TRUE;
	double DeductPrice = 10;
	
	m_pWZShop.BuyProduct(0, "123456789012345678901", GameCode, 1111, "Test", InGamePurchaseSeq, 1, 2, "Bill", 1, SalesZone, DeductPrice, 'C', IsPPCardCash );

	while(!bBuyProduct)
	{
		Sleep(100);
	}

	bInquire = FALSE;
	m_pWZShop.InquireCash(0, 0, "123456789012345678901", GameCode, false);
	
	while(!bInquire)
	{
		Sleep(100);
	}

	return 0;
}

