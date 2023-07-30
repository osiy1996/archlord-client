// ShopClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "ShopClient.h"
#include "ShopClientDlg.h"
#include "atlbase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
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


// CShopClientDlg 대화 상자




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


// CShopClientDlg 메시지 처리기

BOOL CShopClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
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

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	SetDlgItemText(IDC_EDIT_IP, _T("127.0.0.1"));
	SetDlgItemText(IDC_EDIT_PORT, _T("45611"));			


	SetDlgItemText(IDC_EDIT_BUY_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_GIFT_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_TW_IC_GAMECODE,	_T("417"));
	SetDlgItemText(IDC_EDIT_TW_BP_GAMECODE,	_T("417"));

	m_Client.CreateSession(CShopClientDlg::ErrorHandler); //클라이언트 초기화	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
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

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CShopClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
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
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
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

	hFile = CreateFile(szFilePath, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  //File 생성 
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

// AR_TW Start : 캐시 조회
void CShopClientDlg::OnBnClickedButShopInquireTW()
{
	
	CString 
		strGameCode,		// 게임 코드
		strServerIndex,		// 서버 번호
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

// AR_TW Start : 상품 구매
void CShopClientDlg::OnBnClickedButShopbuyTW()
{
	CString 
		strGameCode,		// 게임 코드
		strServerIndex,		// 서버 번호
		strAccountID,		// ID
		strCharName,		// 캐릭터 이름
		strDeductPrice,		// 차감 캐시
		strItemNo,			// 아이템 번호
		strOrderNo,			// 주문 번호
		strOrderQuantity,	// 주문 수량
		strAccessIP;		// IP 어드레스

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
