// ShopClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "ShopClient.h"
#include "ShopClientDlg.h"
#include "atlbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ���� ���α׷� ������ ���Ǵ� CAboutDlg ��ȭ �����Դϴ�.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

// �����Դϴ�.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CShopClientDlg ��ȭ ����




CShopClientDlg::CShopClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CShopClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShopClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CShopClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
#ifdef VERSION_VS60
	ON_BN_CLICKED(IDC_BUT_CONNECT, OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_SHOPINQUIRE, OnBnClickedButShopInquire)
	ON_BN_CLICKED(IDC_BUT_SHOPBUY, OnBnClickedButShopbuy)
	ON_BN_CLICKED(IDC_BUT_GIFT, OnBnClickedButGift)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)

	// AR_TW Start
	ON_BN_CLICKED(IDC_BUT_TW_SHOPINQUIRE, OnBnClickedButShopInquireTW)
	ON_BN_CLICKED(IDC_BUT_TW_SHOPBUY, OnBnClickedButShopbuyTW)
	// AR_TW End
#else
	ON_BN_CLICKED(IDC_BUT_CONNECT, &CShopClientDlg::OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_SHOPINQUIRE, &CShopClientDlg::OnBnClickedButShopInquire)
	ON_BN_CLICKED(IDC_BUT_SHOPBUY, &CShopClientDlg::OnBnClickedButShopbuy)
	ON_BN_CLICKED(IDC_BUT_GIFT, &CShopClientDlg::OnBnClickedButGift)
	ON_BN_CLICKED(IDCANCEL, &CShopClientDlg::OnBnClickedCancel)

	// AR_TW Start
	ON_BN_CLICKED(IDC_BUT_TW_SHOPINQUIRE, &CShopClientDlg::OnBnClickedButShopInquireTW)
	ON_BN_CLICKED(IDC_BUT_TW_SHOPBUY, &CShopClientDlg::OnBnClickedButShopbuyTW)
	
	// AR_TW End
#endif

END_MESSAGE_MAP()


// CShopClientDlg �޽��� ó����

BOOL CShopClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// �ý��� �޴��� "����..." �޴� �׸��� �߰��մϴ�.

	// IDM_ABOUTBOX�� �ý��� ��� ������ �־�� �մϴ�.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// �� ��ȭ ������ �������� �����մϴ�. ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	SetDlgItemText(IDC_EDIT_IP, _T("127.0.0.1"));
	SetDlgItemText(IDC_EDIT_PORT, _T("45611"));			


	SetDlgItemText(IDC_EDIT_BUY_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_GIFT_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_TW_IC_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_TW_BP_GAMECODE,	_T("417"));

	m_Client.CreateSession(CShopClientDlg::ErrorHandler); //Ŭ���̾�Ʈ �ʱ�ȭ	

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CShopClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�. ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CShopClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CShopClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CShopClientDlg::OnBnClickedButConnect()
{
	char aa[10] = {0};
	strcpy(aa, "aa");

	CString strIP, strPort;
	
	GetDlgItemText(IDC_EDIT_IP, strIP);
	GetDlgItemText(IDC_EDIT_PORT, strPort);
	
	m_Client.Connect(strIP.GetBuffer(0), _tstoi(strPort.GetBuffer(0)));
}

void CShopClientDlg::OnBnClickedButShopInquire()
{
	
	bool bSum = false;

	if(BST_CHECKED == ((CButton*)GetDlgItem(IDC_CHK_SUM))->GetCheck()) bSum = true;
	
	CString strType, strAccount, strGamecode, strID;

	GetDlgItemText(IDC_EDIT_INQUIRE_TYPE, strType);
	GetDlgItemText(IDC_EDIT_INQUIRE_ACCOUNT, strAccount);
	GetDlgItemText(IDC_EDIT_INQUIRE_GAMECODE, strGamecode);	
	GetDlgItemText(IDC_EDIT_INQUIRE_ACCOUNTID, strID);	

	m_Client.InquireCash(_tstoi(strType), 
						 _tstoi(strAccount), 
						 (LPSTR)(LPCTSTR)strID, 
						 _tstoi(strGamecode), 
						 bSum);
	
}

void CShopClientDlg::OnBnClickedButShopbuy()
{
	
	CString strAccount, strGameCode, strProductCode, strProductName, strZone, 
			strPrice, strType, strpu, strClass, strLevel, 
			strCharName, strMothod, strServerIndex, strID;

	GetDlgItemText(IDC_EDIT_BUY_ACCOUNT, strAccount);
	GetDlgItemText(IDC_EDIT_BUY_GAMECODE, strGameCode);
	GetDlgItemText(IDC_EDIT_BUY_PRODUCTCODE, strProductCode);
	GetDlgItemText(IDC_EDIT_BUY_NAME, strProductName);
	GetDlgItemText(IDC_EDIT_BUY_ZONE, strZone);
	GetDlgItemText(IDC_EDIT_BUY_PRICE, strPrice);
	GetDlgItemText(IDC_EDIT_BUY_TYPE, strType);
	GetDlgItemText(IDC_EDIT_BUY_PURCHASE, strpu);
	GetDlgItemText(IDC_EDIT_BUY_CLASS, strClass);
	GetDlgItemText(IDC_EDIT_BUY_LEVEL, strLevel);
	GetDlgItemText(IDC_EDIT_BUY_CHARNAME, strCharName);
	GetDlgItemText(IDC_EDIT_BUY_METHOD, strMothod);
	GetDlgItemText(IDC_EDIT_BUY_SERVERINDEX, strServerIndex);
	GetDlgItemText(IDC_EDIT_BUY_ACCOUNTID, strID);	

	USES_CONVERSION;

	m_Client.BuyProduct(_tstoi(strAccount), 
						(LPSTR)(LPCTSTR)strID, 
						_tstoi(strGameCode), 
						_tstoi(strProductCode), 
						A2W((LPSTR)(LPCTSTR)strProductName), 
						_tstoi(strpu), 
						_tstoi(strClass), 
						_tstoi(strLevel), 
						A2W((LPSTR)(LPCTSTR)strCharName), 
						_tstoi(strServerIndex), 
						_tstoi(strZone), 
						_tstof(strPrice), 
						strType[0], 
						_tstoi(strMothod));		
}

void CShopClientDlg::OnBnClickedButGift()
{
	
	CString strSender, strReceiver, strMessage, strGameCode, strProductCode, 
			strProductName, strZone, strPrice, strType, strpu, 
			strSClass, strSLevel, strSCharName, strRClass, strRLevel, 
			strRCharName, strMothod, strSServerIndex, strRServerIndex, strSenderID, 
			strRevID;
	
	GetDlgItemText(IDC_EDIT_GIFT_SENDER, strSender);
	GetDlgItemText(IDC_EDIT_GIFT_RECEIVER, strReceiver);
	GetDlgItemText(IDC_EDIT_GIFT_MESSAGE, strMessage);
	GetDlgItemText(IDC_EDIT_GIFT_GAMECODE, strGameCode);
	GetDlgItemText(IDC_EDIT_GIFT_PRODUCTCODE, strProductCode);
	GetDlgItemText(IDC_EDIT_GIFT_PRODUCTNAME, strProductName);
	GetDlgItemText(IDC_EDIT_GIFT_ZONE, strZone);
	GetDlgItemText(IDC_EDIT_GIFT_PRICE, strPrice);
	GetDlgItemText(IDC_EDIT_GIFT_TYPE, strType);
	GetDlgItemText(IDC_EDIT_GIFT_PURCHASE, strpu);
	GetDlgItemText(IDC_EDIT_GIFT_SCLASS, strSClass);
	GetDlgItemText(IDC_EDIT_GIFT_SLEVEL, strSLevel);
	GetDlgItemText(IDC_EDIT_GIFT_SCHARNAME, strSCharName);
	GetDlgItemText(IDC_EDIT_GIFT_RCLASS, strRClass);
	GetDlgItemText(IDC_EDIT_GIFT_RLEVEL, strRLevel);
	GetDlgItemText(IDC_EDIT_GIFT_RCHARNAME, strRCharName);
	GetDlgItemText(IDC_EDIT_GIFT_METHOD, strMothod);
	GetDlgItemText(IDC_EDIT_GIFT_SSERVERINDEX, strSServerIndex);
	GetDlgItemText(IDC_EDIT_GIFT_RSERVERINDEX, strRServerIndex);
	GetDlgItemText(IDC_EDIT_GIFT_SENDERID, strSenderID);
	GetDlgItemText(IDC_EDIT_GIFT_RECEIVERID, strRevID);


	USES_CONVERSION;

	m_Client.GiftProduct(_tstoi(strSender), 
						 (LPSTR)(LPCTSTR)strSenderID, 
						 _tstoi(strReceiver), 
						 (LPSTR)(LPCTSTR)strRevID, 
						 A2W((LPSTR)(LPCTSTR)strMessage), 
						 _tstoi(strGameCode), 
						 _tstoi(strProductCode), 
						 A2W((LPSTR)(LPCTSTR)strProductName), 
						 _tstoi(strpu), 
						 _tstoi(strSClass), 
						 _tstoi(strSLevel), 
						 A2W((LPSTR)(LPCTSTR)strSCharName), 
						 _tstoi(strSServerIndex), 
						 _tstoi(strRClass), 
						 _tstoi(strRLevel), 
						 A2W((LPSTR)(LPCTSTR)strRCharName), 
						 _tstoi(strRServerIndex), 
						 _tstoi(strZone), 
						 _tstof(strPrice), 
						 strType[0], 
						 _tstoi(strMothod));
}

void CShopClientDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Client.Close();
	OnCancel();
}

void CShopClientDlg::ErrorHandler( DWORD lastError, TCHAR* desc )
{

	CTime t = CTime::GetCurrentTime();   	
	
	CString str;
	str.Format(_T("[%s] %s (%d)\r\n"), t.Format(_T("%Y-%m-%d %H:%M:%S")), desc, lastError);

	TCHAR szModule[MAX_PATH] = {0};
	TCHAR szFilePath[MAX_PATH] = {0};

	::GetModuleFileName( NULL, szModule, MAX_PATH );

	*(_tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, MAX_PATH, _T("%s\\ShopNetWork.log"), szModule );

	HANDLE	hFile;	
	DWORD		dwBytesWritten, dwPos;  

	hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //File ���� 
	if (hFile == INVALID_HANDLE_VALUE)
	{ 
		return;
	}  	
		
	dwPos = SetFilePointer(hFile, 0, NULL, FILE_END); 

	LockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0); 
	WriteFile(hFile, str.GetBuffer(0), str.GetLength(), &dwBytesWritten, NULL); 
	str.ReleaseBuffer();
	UnlockFile(hFile, dwPos, 0, dwPos + str.GetLength(), 0);     
	CloseHandle(hFile); 
}

// AR_TW Start : ĳ�� ��ȸ
void CShopClientDlg::OnBnClickedButShopInquireTW()
{
	
	CString 
		strGameCode,		// ���� �ڵ�
		strServerIndex,		// ���� ��ȣ
		strAccountID;		// ID

	GetDlgItemText(IDC_EDIT_TW_IC_GAMECODE,		strGameCode);
	GetDlgItemText(IDC_EDIT_TW_IC_SERVERINDEX,	strServerIndex);
	GetDlgItemText(IDC_EDIT_TW_IC_ACCOUNTID,	strAccountID);


	m_Client.InquireCash(
		_tstoi(strGameCode), 
		 _tstoi(strServerIndex), 
		 (LPSTR)(LPCTSTR)strAccountID
	);
	
}
// AR_TW End

// AR_TW Start : ��ǰ ����
void CShopClientDlg::OnBnClickedButShopbuyTW()
{
	CString 
		strGameCode,		// ���� �ڵ�
		strServerIndex,		// ���� ��ȣ
		strAccountID,		// ID
		strCharName,		// ĳ���� �̸�
		strDeductPrice,		// ���� ĳ��
		strItemNo,			// ������ ��ȣ
		strOrderNo,			// �ֹ� ��ȣ
		strOrderQuantity,	// �ֹ� ����
		strAccessIP;		// IP ��巹��

	GetDlgItemText(IDC_EDIT_TW_BP_GAMECODE,			strGameCode);
	GetDlgItemText(IDC_EDIT_TW_BP_SERVERINDEX,		strServerIndex);
	GetDlgItemText(IDC_EDIT_TW_BP_ACCOUNTID,		strAccountID);
	GetDlgItemText(IDC_EDIT_TW_BP_CHARNAME,			strCharName);
	GetDlgItemText(IDC_EDIT_TW_BP_DEDUCTPRICE,		strDeductPrice);
	GetDlgItemText(IDC_EDIT_TW_BP_ITEMNO,			strItemNo);
	GetDlgItemText(IDC_EDIT_TW_BP_ORDERNO,			strOrderNo);
	GetDlgItemText(IDC_EDIT_TW_BP_ORDERQUANTITY,	strOrderQuantity);
	GetDlgItemText(IDC_EDIT_TW_BP_ACCESSIP,			strAccessIP);


	USES_CONVERSION;

	m_Client.BuyProduct(
		_tstoi(strGameCode),
		_tstoi(strServerIndex),
		(LPSTR)(LPCTSTR)strAccountID,
		A2W((LPSTR)(LPCTSTR)strCharName),
		_tstoi(strDeductPrice),
		_tstoi64(strItemNo),
		_tstoi64(strOrderNo),
		_tstoi(strOrderQuantity),
		inet_addr(strAccessIP)
	);
}
// AR_TW End
