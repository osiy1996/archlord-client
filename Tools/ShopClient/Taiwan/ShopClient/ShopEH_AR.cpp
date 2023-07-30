#include "stdafx.h"
#include "ShopEH_AR.h"
#include "atlbase.h"

CShopEH_AR::CShopEH_AR(void)
{
	::InitializeCriticalSection(&m_cs);
}

CShopEH_AR::~CShopEH_AR(void)
{
	::DeleteCriticalSection(&m_cs);
}

void CShopEH_AR::OnNetConnect(bool success, DWORD error)
{
	if(success) AfxMessageBox(_T("OnConnect succeed"));
	else 
	{
		CString strMsg;
		strMsg.Format(_T("OnConnect fali (ErrorCode: %d)"), error);
		AfxMessageBox(strMsg);
	}
}

void CShopEH_AR::OnNetSend( int size )
{
	//AfxMessageBox("OnSend");
}

void CShopEH_AR::OnNetClose( DWORD error )
{
	AfxMessageBox(_T("OnClose"));
}

void CShopEH_AR::OnGiftProduct(DWORD AccountSeq, 
							   DWORD ReceiverSeq, 
							   DWORD DeductCashSeq, 
							   long ResultCode) 
{
	CString strMsg;

	strMsg.Format(_T("��ǰ ���� ���\r\n�������: %d, �޴»��: %d, ���: %d, �� ��ǰ ������ ���� ���� ��ȣ: %d\r\n"), 
		AccountSeq, ReceiverSeq, ResultCode, DeductCashSeq);

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

void CShopEH_AR::OnInquireCash(DWORD AccountSeq, 
							   char AccountID[MAX_ACCOUNT_LENGTH], 
							   double CashSum, 
							   double PointSum, 
							   ShopProtocol::STCashDetail_GB Detail[], 
							   long nCashInfoCount, 
							   long ResultCode)
{
	CString strMsg, strDetail;

	strMsg.Format(_T("ĳ�� ��ȸ ���\r\n��ȸ�� ���: %s(%d), ĳ���հ�: %f, ����Ʈ�հ�: %f, ���: %d, \r\n���� ����: \r\n"), 
		AccountID, AccountSeq, CashSum, PointSum, ResultCode);

	USES_CONVERSION;
	
	for(int i = 0 ; i < nCashInfoCount ;i++)
	{
		if(0 != Detail[i].Type)
		{
			strDetail.Format(_T("[ĳ��Ÿ��: %d, �̸�: %s, ����: %c, ��:%f] \r\n"), 
				Detail[i].CashTypeCode, W2A(Detail[i].Name), Detail[i].Type, Detail[i].Value);
			strMsg = strMsg + strDetail;
		}
	}

	strMsg += _T("\r\n");

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

void CShopEH_AR::OnBuyProduct(DWORD AccountSeq, 
							  char AccountID[MAX_ACCOUNT_LENGTH], 
							  DWORD DeductCashSeq, 
							  DWORD InGamePurchaseSeq, 
							  long ResultCode) 
{	
	CString strMsg;

	strMsg.Format(_T("��ǰ ���� ���\r\n������ ���: %s(%d), ���: %d, �� ��ǰ ������ ���� ���� ��ȣ: %d, ���� �� ��ǰ ���Ź�ȣ: %d\r\n"), 
		AccountID, AccountSeq, ResultCode, DeductCashSeq, InGamePurchaseSeq);

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}



// AR_TW Start
void CShopEH_AR::OnInquireCash(char AccountID[MAX_ACCOUNT_LENGTH], 
							   double Cash,
							   long ResultCode)
{
	CString strMsg, strDetail;

	strMsg.Format(_T("AR_TW ĳ�� ��ȸ ���\r\n��ȸ�� ���: %s, ĳ��: %f, ���: %d"), 
		AccountID, Cash, ResultCode);

	strMsg += _T("\r\n");

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}
// AR_TW End

// AR_TW Start
void CShopEH_AR::OnBuyProduct(char	AccountID[MAX_ACCOUNT_LENGTH], 
							  INT64 ItemNo,
							  INT64 OrderNo,
							  int	OrderNumber,
							  WCHAR	RetMessage[MAX_RETMESSAGE_LENGTH],
							  long	ResultCode)
{	
	CString strMsg;

	USES_CONVERSION;
	strMsg.Format(_T("AR_TW ��ǰ ���� ���\r\n������ ���:%s, ������ ��ȣ:%I64d, �ֹ�(����)��ȣ:%I64d, �ֹ������ȣ:%d, �޽���:%s, ���:%d\r\n"), 
		AccountID, ItemNo, OrderNo, OrderNumber, W2A(RetMessage), ResultCode);

	
	

	WriteLog((LPSTR)(LPCTSTR)strMsg);//WriteLog(strMsg.GetBuffer(0));
}

// AR_TW End

void CShopEH_AR::WriteLog(char* szMsg)
{
	::EnterCriticalSection(&m_cs);

	TCHAR szModule[MAX_PATH] = {0};
	TCHAR szFilePath[MAX_PATH] = {0};

	::GetModuleFileName( NULL, szModule, MAX_PATH );

	*(_tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, MAX_PATH, _T("%s\\Shop.log"), szModule );

	CTime t = CTime::GetCurrentTime();   	
	
	CString str;
	str.Format(_T("[%s] %s"), t.Format(_T("%Y-%m-%d %H:%M:%S")), szMsg);


	HANDLE	hFile;	
	DWORD		dwBytesWritten, dwPos;  

	hFile = CreateFile(szFilePath, FILE_SHARE_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //File ���� 

	if (hFile != INVALID_HANDLE_VALUE)
	{ 
		dwPos = SetFilePointer(hFile, 0, NULL, FILE_END); 

		//LockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0); 
		WriteFile(hFile, str.GetBuffer(0), str.GetLength(), &dwBytesWritten, NULL); 
		str.ReleaseBuffer();
		//UnlockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0);     
		CloseHandle(hFile); 
	}
	
	::LeaveCriticalSection(&m_cs);
}