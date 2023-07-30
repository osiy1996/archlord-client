#include "AgsmBillingGlobal.h"
#ifdef _WEBZEN_BILLING_TAIWAN_
#include "WebzenBilling.h"
#endif
#include "WebzenBilling.h"

#ifdef _AREA_GLOBAL_
CWebzenShop* m_pWZShop = NULL;
CWebzenBilling* m_pWZBilling = NULL;

#ifdef _WIN64
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD_x64.lib") 
	#else
		#pragma comment(lib, "WebzenBilling_x64.lib") 
	#endif
#else
	#ifdef _DEBUG
		#pragma comment(lib, "WebzenBillingD.lib") 
	#else
		#pragma comment(lib, "WebzenBilling.lib") 
	#endif
#endif

#endif

#if defined(_AREA_GLOBAL_)

const DWORD WZBILLING_GAMECODE = 417;
const DWORD WZBILLING_SALESZONE = 428;
const DWORD WZBILLING_SERVERTYPE = 686; // 아크로드 글로벌 // Taiwan도 글로벌과 동일한 코드 사용

AgsmBillingGlobal::AgsmBillingGlobal()
{
}

AgsmBillingGlobal::~AgsmBillingGlobal()
{
	if( m_pWZShop )
		delete m_pWZShop;

	if( m_pWZBilling )
		delete m_pWZBilling;
}

BOOL AgsmBillingGlobal::Initialize()
{
	m_AdminGUID.SetCount(10000);
	if (!m_AdminGUID.InitializeObject(sizeof(AgpdBillingGlobal*), m_AdminGUID.GetCount()))
		return FALSE;

	//////////////////////////////////////////////////////////////////////////
	//
	m_pWZShop = new CWebzenShop;

	if(!m_pWZShop)
		return FALSE;

#ifdef _AREA_GLOBAL_
	//m_pWZShop->SetCallbackOnLog(AgsmBillingGlobal::OnLog);
	m_pWZShop->SetCallbackOnInquireCash(AgsmBillingGlobal::OnInquireCash);
	m_pWZShop->SetCallbackOnBuyProduct(AgsmBillingGlobal::OnBuyProduct);
	m_pWZShop->Initialize();
#endif

	//////////////////////////////////////////////////////////////////////////
	//
	m_pWZBilling = new CWebzenBilling;
	if(!m_pWZBilling)
		return FALSE;

	//m_pWZBilling->SetCallbackOnLog(AgsmBillingGlobal::OnLog);
	m_pWZBilling->SetCallbackOnUserStatus(AgsmBillingGlobal::OnUserStatus);
	m_pWZBilling->SetCallbackOnInquirePersonDeduct(AgsmBillingGlobal::OnInquirePersonDeduct);
	m_pWZBilling->Initialize();

	return TRUE;
}

BOOL AgsmBillingGlobal::OnIdle()
{
	eWZConnect eStatusShop = m_pWZShop->GetStatus();
	if(eStatusShop == eNotConnect)
		ConnectShop();

	eWZConnect eStatusBilling = m_pWZBilling->GetStatus();
	if(eStatusBilling == eNotConnect)
		ConnectBilling();

	return TRUE;
}

BOOL AgsmBillingGlobal::ConnectShop()
{
	AuXmlDocument pDoc;
	if(!pDoc.LoadFile("WebzenBilling.xml"))
		return FALSE;

	AuXmlNode* pFirstNode = pDoc.FirstChild("WebzenBilling");
	if(!pFirstNode)
		return FALSE;

	AuXmlElement* pShop = pFirstNode->FirstChildElement("Shop");
	if(!pShop)
		return FALSE;

	AuXmlElement* pNodeIP	= pShop->FirstChildElement("IPAddress");
	AuXmlElement* pNodePort	= pShop->FirstChildElement("Port");

	if(!pNodeIP || !pNodePort)
		return FALSE;

	CHAR* pIPAddress	= (CHAR*)pNodeIP->GetText();
	CHAR* pPort			= (CHAR*)pNodePort->GetText();

	if(!pIPAddress || !pPort)
		return FALSE;

	m_pWZShop->Connect(pIPAddress, atoi(pPort));

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ConnectShop : pIPAddress = %s, pPort = %s", pIPAddress, pPort);
		OnLog(strLog);
	}

	return TRUE;
}

BOOL AgsmBillingGlobal::ConnectBilling()
{
	AuXmlDocument pDoc;
	if(!pDoc.LoadFile("WebzenBilling.xml"))
		return FALSE;

	AuXmlNode* pFirstNode = pDoc.FirstChild("WebzenBilling");
	if(!pFirstNode)
		return FALSE;

	AuXmlElement* pBilling = pFirstNode->FirstChildElement("Billing");
	if(!pBilling)
		return FALSE;

	AuXmlElement* pNodeIP	= pBilling->FirstChildElement("IPAddress");
	AuXmlElement* pNodePort	= pBilling->FirstChildElement("Port");

	if(!pNodeIP || !pNodePort)
		return FALSE;

	CHAR* pIPAddress	= (CHAR*)pNodeIP->GetText();
	CHAR* pPort			= (CHAR*)pNodePort->GetText();

	if(!pIPAddress || !pPort)
		return FALSE;

	m_pWZBilling->Connect(pIPAddress, atoi(pPort));

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "ConnectBilling : pIPAddress = %s, pPort = %s", pIPAddress, pPort);
		OnLog(strLog);
	}

	return TRUE;
}

void AgsmBillingGlobal::OnLog( char* strLog )
{
	printf("%s\n", strLog);
	AuLogFile_s("LOG\\WZBilling.txt", strLog);
}

BOOL AgsmBillingGlobal::InquireCash( CHAR* szAccountID )
{
	return m_pWZShop->InquireCash(1, 0, szAccountID, WZBILLING_GAMECODE, false); // archlord gamecode = 417
}

/*
BOOL AgsmBillingGlobal::InquireCashTW( CHAR* szAccountID, INT32 lServerIndex )
{
	return m_pWZShop->InquireCash(WZBILLING_GAMECODE, lServerIndex, szAccountID);
}
*/

void AgsmBillingGlobal::OnInquireCash( PVOID pInquire )
{
	stInquire* pInquireResult = (stInquire*)pInquire;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnInquire : Result = %d, AccountSeq = %d, AccountID = %s, WCoinSum = %d, PCoinSum = %d", 
			pInquireResult->ResultCode, pInquireResult->AccountSeq, pInquireResult->AccountID, (int)pInquireResult->WCoinSum, (int)pInquireResult->PCoinSum);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnInquireCash(pInquireResult->ResultCode, pInquireResult->AccountID, pInquireResult->WCoinSum, pInquireResult->PCoinSum);
}

/*
void AgsmBillingGlobal::OnInquireCastTW( PVOID pInquire )
{
	stTWInquire *pInquireResult = (stTWInquire *) pInquire;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnInquire : Result = %d, , AccountID = %s, CoinSum = %d", 
			pInquireResult->lResultCode, pInquireResult->AccountID, pInquireResult->AccountID, (int)pInquireResult->lCoinSum);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnInquireCash(pInquireResult->lResultCode, pInquireResult->AccountID, pInquireResult->lCoinSum);
}
*/

BOOL AgsmBillingGlobal::BuyProduct(CHAR* AccountID, INT32 ProductID, CHAR* ProductName, UINT64 ListSeq, INT32 Class, INT32 Level, CHAR* szCharName, INT32 ServerIndex, double Price, INT32 lType )
{
	return m_pWZShop->BuyProduct(0, AccountID, WZBILLING_GAMECODE, ProductID, ProductName, ListSeq, Class, Level, szCharName, ServerIndex, WZBILLING_SALESZONE, Price, 'C', lType);
}

/*
BOOL AgsmBillingGlobal::BuyProductTW(INT32 ServerIndex, CHAR* AccountID, CHAR* CharName, double DefuctPrice, INT64 ItemNo, INT64 OrderNo, INT32 OrderQuantity, DWORD AccessIP)
{
	return m_pWZShop->BuyProduct(WZBILLING_GAMECODE, ServerIndex, AccountID, CharName, DefuctPrice, ItemNo, OrderNo, OrderQuantity, AccessIP);
}
*/

void AgsmBillingGlobal::OnBuyProduct(PVOID pBuyProduct)
{
	stBuyProduct* pstBuyProduct = (stBuyProduct*)pBuyProduct;

	{
		CHAR strLog[256] = { 0, };
		sprintf_s(strLog, sizeof(strLog), "OnBuyProduct : ResultCode = %d, AccountSeq = %d, AccountID = %s, DeductCashSeq = %I64d, InGamePurchaseSeq = %I64d", 
			pstBuyProduct->ResultCode, pstBuyProduct->AccountSeq, pstBuyProduct->AccountID, pstBuyProduct->DeductCashSeq, pstBuyProduct->InGamePurchaseSeq);
		OnLog(strLog);
	}

	AgsmBilling::GetInstance()->OnBuyProduct(pstBuyProduct->ResultCode, pstBuyProduct->AccountID, pstBuyProduct->DeductCashSeq);
}

/*
void AgsmBillingGlobal::OnBuyProductTW( PVOID pBuyProduct )
{

}
*/

void AgsmBillingGlobal::OnUserStatus( PVOID UserStatus )
{
	stUserStatus* pUserStatus = (stUserStatus*)UserStatus;

	/*pUserStatus->dwAccountGUID;
	pUserStatus->dwBillingGUID;
	pUserStatus->RealEndDate;
	pUserStatus->EndDate;
	pUserStatus->dRestPoint;
	pUserStatus->dRestTime;
	pUserStatus->nDeductType;
	pUserStatus->nAccessCheck;
	pUserStatus->nResultCode;*/

	if(pUserStatus->nResultCode == 0 && pUserStatus->nAccessCheck == 0 && pUserStatus->nDeductType != 0)
	{
		{
			CTime tEnd = CTime(pUserStatus->EndDate);
			std::string str = tEnd.Format("%Y-%m-%d %H:%M:%S");

			CHAR strLog[256] = { 0, };
			sprintf_s(strLog, sizeof(strLog), "OnUserStatus : nResultCode = %d, nDeductType = %d, dwAccountGUID = %d, dwBillingGUID = %d, nDeductType = %d, EndDate = %s", 
				pUserStatus->nResultCode, pUserStatus->nDeductType,
				pUserStatus->dwAccountGUID, pUserStatus->dwBillingGUID, pUserStatus->nDeductType, str.c_str());
			OnLog(strLog);
		}

		AgsmBilling::GetInstance()->OnUserStatus(pUserStatus->dwAccountGUID, pUserStatus->dwBillingGUID, pUserStatus->EndDate, pUserStatus->dRestTime, pUserStatus->nDeductType);
	}
}

void AgsmBillingGlobal::OnInquirePersonDeduct( PVOID InquirePersonDeduct )
{
	stInquirePersonDeduct* pInquirePersonDeduct = (stInquirePersonDeduct*)InquirePersonDeduct;

	AgsmBilling::GetInstance()->OnInquirePersonDeduct(pInquirePersonDeduct->AccountGUID, pInquirePersonDeduct->ResultCode);
}

BOOL AgsmBillingGlobal::InquirePersonDeduct( DWORD AccountGUID )
{
	if( m_pWZBilling )
		m_pWZBilling->InquirePersonDeduct(AccountGUID, WZBILLING_GAMECODE);

	return TRUE;
}

BOOL AgsmBillingGlobal::AddUser(AgpdCharacter* pcsCharacter, DWORD AccountGUID)
{
	AgpdBillingGlobal* pBillingUser = new AgpdBillingGlobal;
	if(!pBillingUser)
		return FALSE;

	pBillingUser->pcsCharacter = pcsCharacter;
	pBillingUser->AccountGUID = AccountGUID;

	m_AdminGUID.AddObject(&pBillingUser, AccountGUID);

	return TRUE;
}

BOOL AgsmBillingGlobal::RemoveUser(DWORD AccountGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	m_AdminGUID.RemoveObject(AccountGUID);

	delete (*pBillingUser);

	return TRUE;
}

AgpdCharacter* AgsmBillingGlobal::GetCharacter( DWORD AccountGUID )
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return NULL;

	return (*pBillingUser)->pcsCharacter;
}

BOOL AgsmBillingGlobal::SetBillingGUID(DWORD AccountGUID, DWORD BillingGUID)
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return FALSE;

	(*pBillingUser)->dwBillingGUID = BillingGUID;

	return TRUE;
}

BOOL AgsmBillingGlobal::Login( DWORD AccountGUID, CHAR* szIP )
{
	if( m_pWZBilling )
		m_pWZBilling->UserLogin(AccountGUID, inet_addr(szIP), 0, WZBILLING_GAMECODE, WZBILLING_SERVERTYPE);

	return TRUE;
}

void AgsmBillingGlobal::LogOut( DWORD AccountGUID )
{
	AgpdBillingGlobal** pBillingUser = (AgpdBillingGlobal**)m_AdminGUID.GetObject(AccountGUID);
	if(!pBillingUser || !*pBillingUser)
		return;

	DWORD dwBillingGUID = (*pBillingUser)->dwBillingGUID;

	if( m_pWZBilling && dwBillingGUID > 0)
		m_pWZBilling->UserLogout(dwBillingGUID);
}
#endif //_AREA_GLOBAL_