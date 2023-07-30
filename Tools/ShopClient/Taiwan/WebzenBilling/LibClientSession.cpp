#include "LibClientSession.h"
#include "stdio.h"

CLibClientSessionShop::CLibClientSessionShop(void)
{
	WZSHOP_OnInquireCash	= NULL;
	WZSHOP_OnBuyProduct		= NULL;
	WZSHOP_OnLog			= NULL;

	m_bConnect = eNotConnect;
	m_nLastError = 0;
}

CLibClientSessionShop::~CLibClientSessionShop(void)
{
}

void CLibClientSessionShop::OnNetConnect(bool success, DWORD error)
{
	m_nLastError = error;
	m_bConnect = ( success ) ? eConnected : eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d, %d\n", __FUNCTION__, success, error);
	WriteLog(szLog);
}

void CLibClientSessionShop::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CLibClientSessionShop::OnNetClose( DWORD error )
{
	m_nLastError = error;
	m_bConnect = eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d\n", __FUNCTION__, error);
	WriteLog(szLog);
}

void CLibClientSessionShop::OnGiftProduct(DWORD SenderSeq, DWORD ReceiverSeq, DWORD DeductCashSeq, long ResultCode) 
{
	/*CString strMsg;

	strMsg.Format("상품 선물 결과\r\n보낸사람: %d, 받는사람: %d, 결과: %d, 웹 상품 차감에 대한 차감 번호: %d\r\n", 
		SenderSeq, ReceiverSeq, ResultCode, DeductCashSeq);

	WriteLog(strMsg.GetBuffer(0));*/
}

void CLibClientSessionShop::OnInquireCash(DWORD AccountSeq, CHAR* AccountID, double CashSum, double PointSum, ShopProtocol::STCashDetail_GB Detail[], long nCashInfoCount ,long ResultCode)
{
	stInquire pInquire;
	memset( &pInquire, 0, sizeof(stInquire));

	pInquire.ResultCode	= ResultCode;
	pInquire.AccountSeq = AccountSeq;
	strcpy_s(pInquire.AccountID, sizeof(pInquire.AccountID), AccountID);
	
	for(int i = 0 ; i < nCashInfoCount ;i++)
	{
		if(Detail[i].Type == 'C')
		{
			switch(Detail[i].CashTypeCode)
			{
				case 508: // WCash
					{
						pInquire.WCoinSum += Detail[i].Value;
					} break;
				case 509: // PP Card Only
					{
						pInquire.PCoinSum += Detail[i].Value;
					} break;
			}
		}
		else if(Detail[i].Type == 'P')
		{
			// nop
		}
	}

	//pInquire.WCoinSum -= pInquire.PCoinSum;

	if( WZSHOP_OnInquireCash )
		WZSHOP_OnInquireCash(&pInquire);
}

void CLibClientSessionShop::OnBuyProduct(DWORD AccountSeq, CHAR* AccountID, DWORD DeductCashSeq, DWORD InGamePurchaseSeq, long ResultCode) 
{	
	stBuyProduct pBuyProduct;
	pBuyProduct.ResultCode		= ResultCode;
	pBuyProduct.AccountSeq		= AccountSeq;
	strcpy_s(pBuyProduct.AccountID, sizeof(pBuyProduct.AccountID), AccountID);
	pBuyProduct.DeductCashSeq	= DeductCashSeq;
	pBuyProduct.InGamePurchaseSeq = InGamePurchaseSeq;

	if( WZSHOP_OnBuyProduct )
		WZSHOP_OnBuyProduct(&pBuyProduct);
}

void CLibClientSessionShop::WriteLog(char* szMsg)
{
	if( WZSHOP_OnLog )
		WZSHOP_OnLog(szMsg);
}

void CLibClientSessionShop::OnInquireCash(char *AccountID, double Cash, long ResultCode)
{
	stTWInquire stInquireInfo;

	stInquireInfo.lResultCode	= ResultCode;
	stInquireInfo.lCoinSum		= Cash;
	strncpy_s(stInquireInfo.AccountID, MAX_ACCOUNT_LENGTH, AccountID, MAX_ACCOUNT_LENGTH);

	if( WZSHOP_OnInquireCash )
		WZSHOP_OnInquireCash(&stInquireInfo);
}

void CLibClientSessionShop::OnBuyProduct(char *AccountID, INT64 ItemNo, INT64 OrderNo, int OrderNumber, WCHAR *RetMessage, long ResultCode)
{

}


//////////////////////////////////////////////////////////////////////////
// BILLING
CLibClientSessionBilling::CLibClientSessionBilling(void)
{
	WZBILLING_OnLog					= NULL;
	WZBILLING_OnInquirePCRoomPoint	= NULL;
	WZBILLING_OnInquireMultiUser	= NULL;
	WZBILLING_OnUserStatus			= NULL;
	WZBILLING_OnInquirePersonDeduct	= NULL;

	m_bConnect = eNotConnect;
	m_nLastError = 0;
}

CLibClientSessionBilling::~CLibClientSessionBilling(void)
{
}

void CLibClientSessionBilling::OnNetConnect(bool success, DWORD error)
{
	m_nLastError = error;
	m_bConnect = ( success ) ? eConnected : eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d, %d\n", __FUNCTION__, success, error);
	WriteLog(szLog);
}

void CLibClientSessionBilling::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CLibClientSessionBilling::OnNetClose( DWORD error )
{
	m_nLastError = error;
	m_bConnect = eNotConnect;

	CHAR szLog[256] = { 0, };
	sprintf(szLog, "[%s] %d\n", __FUNCTION__, error);
	WriteLog(szLog);
}

void CLibClientSessionBilling::WriteLog(char* szMsg)
{
	if( WZBILLING_OnLog )
		WZBILLING_OnLog(szMsg);
}

void CLibClientSessionBilling::OnInquirePCRoomPoint( long AccountID, long RoomGUID, long GameCode, long ResultCode )
{
	stInquirePCRoomPoint pInquirePCRoomPoint;
	pInquirePCRoomPoint.AccountGUID = AccountID;
	pInquirePCRoomPoint.RoomGUID = RoomGUID;
	pInquirePCRoomPoint.GameCode = GameCode;
	pInquirePCRoomPoint.ResultCode = ResultCode;

	if( WZBILLING_OnInquirePCRoomPoint )
		WZBILLING_OnInquirePCRoomPoint(&pInquirePCRoomPoint);
}

void CLibClientSessionBilling::OnInquireMultiUser( long AccountID, long RoomGUID, long Result )
{
	stInquireMultiUser pInquireMultiUser;
	pInquireMultiUser.AccountID = AccountID;
	pInquireMultiUser.RoomGUID = RoomGUID;
	pInquireMultiUser.Result = Result;
	
	if( WZBILLING_OnInquireMultiUser )
		WZBILLING_OnInquireMultiUser(&pInquireMultiUser);
}

void CLibClientSessionBilling::OnUserStatus( DWORD dwAccountGUID, long dwBillingGUID, DWORD RealEndDate, DWORD EndDate, double dRestPoint, double dRestTime, short nDeductType, short nAccessCheck, short nResultCode )
{
	stUserStatus pstUserStatus;
	pstUserStatus.dwAccountGUID = dwAccountGUID;
	pstUserStatus.dwBillingGUID = dwBillingGUID;
	pstUserStatus.RealEndDate = RealEndDate;
	pstUserStatus.EndDate = EndDate;
	pstUserStatus.dRestPoint = dRestPoint;
	pstUserStatus.dRestTime = dRestTime;
	pstUserStatus.nDeductType = nDeductType;
	pstUserStatus.nAccessCheck = nAccessCheck;
	pstUserStatus.nResultCode = nResultCode;

	if( WZBILLING_OnUserStatus )
		WZBILLING_OnUserStatus(&pstUserStatus);
}

void CLibClientSessionBilling::OnInquirePersonDeduct( long AccountGUID, long GameCode, long ResultCode )
{
	stInquirePersonDeduct pstInquirePersonDeduct;
	pstInquirePersonDeduct.AccountGUID = AccountGUID;
	pstInquirePersonDeduct.GameCode = GameCode;
	pstInquirePersonDeduct.ResultCode = ResultCode;

	if( WZBILLING_OnInquirePersonDeduct )
		WZBILLING_OnInquirePersonDeduct(&pstInquirePersonDeduct);
}
