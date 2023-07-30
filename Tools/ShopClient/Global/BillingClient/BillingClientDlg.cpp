// BillingClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "BillingClient.h"
#include "BillingClientDlg.h"

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


// CBillingClientDlg ��ȭ ����


CBillingClientDlg::CBillingClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBillingClientDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBillingClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBillingClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
#ifdef VERSION_VS60
	ON_BN_CLICKED(IDC_BUT_CONNECT, OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_LOGIN, OnBnClickedButLogin)
	ON_BN_CLICKED(IDC_BUT_LOGOUT, OnBnClickedButLogout)
	ON_BN_CLICKED(IDC_BUT_INQUIRE, OnBnClickedButInquire)	
	ON_BN_CLICKED(IDC_BUT_INQUIRE_MULTI, OnBnClickedButInquireMulti)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_PCPOINT, OnBnClickedButInquirePcpoint)
	//ON_BN_CLICKED(IDC_BUT_INQUIRE_DEDUCTTYPE, OnBnClickedButInquireDeducttype)
#else
	ON_BN_CLICKED(IDC_BUT_CONNECT, &CBillingClientDlg::OnBnClickedButConnect)
	ON_BN_CLICKED(IDC_BUT_LOGIN, &CBillingClientDlg::OnBnClickedButLogin)
	ON_BN_CLICKED(IDC_BUT_LOGOUT, &CBillingClientDlg::OnBnClickedButLogout)
	ON_BN_CLICKED(IDC_BUT_INQUIRE, &CBillingClientDlg::OnBnClickedButInquire)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_MULTI, &CBillingClientDlg::OnBnClickedButInquireMulti)
	ON_BN_CLICKED(IDCANCEL, &CBillingClientDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUT_INQUIRE_PCPOINT, &CBillingClientDlg::OnBnClickedButInquirePcpoint)
	//ON_BN_CLICKED(IDC_BUT_INQUIRE_DEDUCTTYPE, &CBillingClientDlg::OnBnClickedButInquireDeducttype)
#endif	
	ON_BN_CLICKED(IDC_BUTTON2, &CBillingClientDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CBillingClientDlg �޽��� ó����

BOOL CBillingClientDlg::OnInitDialog()
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

	
	//SetDlgItemText(IDC_EDIT_IP, "10.101.203.202");
	SetDlgItemText(IDC_EDIT_IP, _T("218.234.76.11"));
	SetDlgItemText(IDC_EDIT_PORT, _T("45610"));
	SetDlgItemText(IDC_EDIT_LOGIN_COUNT, _T("1"));
	SetDlgItemText(IDC_EDIT_LOGIN_PC, _T("0"));
	SetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, _T("417"));

	m_Client.CreateSession(CBillingClientDlg::ErrorHandler); //Ŭ���̾�Ʈ �ʱ�ȭ

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

void CBillingClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBillingClientDlg::OnPaint()
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
HCURSOR CBillingClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CBillingClientDlg::OnBnClickedButConnect()
{	
	CString strIP, strPort;
	
	GetDlgItemText(IDC_EDIT_IP, strIP);
	GetDlgItemText(IDC_EDIT_PORT, strPort);
	
	m_Client.Connect(strIP.GetBuffer(0), _tstoi(strPort.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButLogin()
{
	//char* strAccountID, DWORD dwUserGUID, DWORD dwIPAddress, DWORD dwRoomGUID, DWORD dwGameCode, DWORD dwServerType
	//������ �������� �α��� �ؾ��Ѵ�. �α��� ���δ� OnConnected �޼ҵ带 ���� Ȯ���Ѵ�.
	CString str, strCount, strPC, strGameCode;
	GetDlgItemText(IDC_EDIT_LOGIN, str);	

	GetDlgItemText(IDC_EDIT_LOGIN_COUNT, strCount);
	GetDlgItemText(IDC_EDIT_LOGIN_PC, strPC);
	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, strGameCode);	

	int nID = _tstoi(str.GetBuffer(0));
	int nCount = _tstoi(strCount.GetBuffer(0));
	int nPC = _tstoi(strPC.GetBuffer(0));
	int nGameCode = _tstoi(strGameCode.GetBuffer(0));

	for(int i = 0 ; i < nCount ; i++)
	{
		m_Client.UserLogin(nID + i, inet_addr("127.0.0.1"), nPC, nGameCode, 1);
	}	
}

void CBillingClientDlg::OnBnClickedButLogout()
{	
	CString str;
	GetDlgItemText(IDC_EDIT_BILLINGID, str);
	
	m_Client.UserLogout( _tstoi(str.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButInquire()
{
	CString str;
	GetDlgItemText(IDC_EDIT_INQUIRE, str);

	m_Client.InquireUser( _tstoi(str.GetBuffer(0)));
	/*

	for(int i = 0 ; i < 100000 ; i++)	
	{
		Sleep(1000 * 60);
		m_Client.InquireUser( _tstoi(str.GetBuffer(0)));
	}
	*/
}

void CBillingClientDlg::OnBnClickedButInquireMulti()
{
	CString str, str1;
	GetDlgItemText(IDC_EDIT_MULTI, str);
	GetDlgItemText(IDC_EDIT_MULTIACCOUNT, str1);
	
	m_Client.InquireMultiUser( _tstoi(str1.GetBuffer(0)), _tstoi(str.GetBuffer(0)));
}

void CBillingClientDlg::OnBnClickedButInquirePcpoint()
{
	CString str1, str2, str3;
	GetDlgItemText(IDC_EDIT_PCPOINT_ACCOUNT, str1);
	GetDlgItemText(IDC_EDIT_PCPOINT_PCACCOUNT, str2);
	GetDlgItemText(IDC_EDIT_PCPOINT_GAMECODE, str3);

	m_Client.InquirePCRoomPoint(_tstoi(str1.GetBuffer(0)), _tstoi(str2.GetBuffer(0)), _tstoi(str3.GetBuffer(0)));
}
//
//void CBillingClientDlg::OnBnClickedButInquireDeducttype()
//{
//	CString str, str1;
//
//	GetDlgItemText(IDC_EDIT_ACCOUNT, str);
//	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE2, str1);	
//
//	m_Client.InquireDeductType(_tstoi(str.GetBuffer(0)), _tstoi(str1.GetBuffer(0)));
//}


void CBillingClientDlg::OnBnClickedCancel()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_Client.Close();
	OnCancel();
}

void CBillingClientDlg::ErrorHandler( DWORD lastError, TCHAR* desc )
{
	CTime t = CTime::GetCurrentTime();   	 
	
	CString str;
	str.Format(_T("[%s] %s (%d)\r\n"), t.Format(_T("%Y-%m-%d %H:%M:%S")), desc, lastError);

	TCHAR szModule[MAX_PATH] = {0};
	TCHAR szFilePath[MAX_PATH] = {0};

	::GetModuleFileName( NULL, szModule, MAX_PATH );

	*(_tcsrchr( szModule, '\\' ) ) = 0;

	StringCchPrintf( szFilePath, STRSAFE_MAX_CCH, _T("%s\\BillingNetWork.log"), szModule );

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

void CBillingClientDlg::OnBnClickedButton2()
{
	//m_Client.Close();
	//m_Client.SetKill();

	CString str, strCount, strPC, strGameCode;
	GetDlgItemText(IDC_EDIT_LOGIN, str);	

	GetDlgItemText(IDC_EDIT_LOGIN_COUNT, strCount);
	GetDlgItemText(IDC_EDIT_LOGIN_PC, strPC);
	GetDlgItemText(IDC_EDIT_LOGIN_GAMECODE, strGameCode);	

	int nID = _tstoi(str.GetBuffer(0));
	int nCount = _tstoi(strCount.GetBuffer(0));
	int nPC = _tstoi(strPC.GetBuffer(0));
	int nGameCode = _tstoi(strGameCode.GetBuffer(0));

	m_Client.InquirePersonDeduct(nID, nGameCode);
}
