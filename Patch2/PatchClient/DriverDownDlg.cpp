// DriverDownDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "DriverDownDlg.h"

#include "PatchClientDlg.h"
#include "AutoDetectMemoryLeak.h"

#ifdef _DEBUG

#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif



// DriverDownDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(DriverDownDlg, CDialog)
DriverDownDlg::DriverDownDlg(CWnd* pParent /*=NULL*/)
	: CDialog(DriverDownDlg::IDD, pParent)
{
	m_bSelectResult		= FALSE;
	m_strFileName		= "";
	m_strDriverURL		= "";
	m_bDownloadSuccess	= FALSE;
}

DriverDownDlg::~DriverDownDlg()
{
}

void DriverDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPatchClientOptionDlg)
	DDX_Control(pDX, IDC_BUTTON_DRIVER_OK, m_cKbcOKButton);
	DDX_Control(pDX, IDC_BUTTON_DRIVER_NO, m_cKbcNOButton);
	//}}AFX_DATA_MAP
	DDX_Control(pDX, IDC_PROGRESS1, m_ctrlProgress);
}


BEGIN_MESSAGE_MAP(DriverDownDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER_OK, OnBnClickedButtonDriverOk)
	ON_BN_CLICKED(IDC_BUTTON_DRIVER_NO, OnBnClickedButtonDriverNo)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CHECK_DONOTUSECHECKDRIVER, OnBnClickedCheckDonotusecheckdriver)
END_MESSAGE_MAP()


// DriverDownDlg �޽��� ó�����Դϴ�.

BOOL DriverDownDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.
	m_cDrawFont.CreateFont( 14, 0, 0, 0, 500, 0, 0, 0, HANGUL_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "����" );

	m_cBMPBackground.LoadBitmap( IDB_DRIVERBACK );

	m_cKbcOKButton.SetBmpButtonImage( IDB_Button_Option_Save_Set, RGB(0,0,255) );
	m_cKbcOKButton.SetButtonText( "��" );
	m_cKbcNOButton.SetBmpButtonImage( IDB_Button_Option_Cancel_Set, RGB(0,0,255) );
	m_cKbcNOButton.SetButtonText( "�ƴϿ�" );

	GetDlgItem( IDC_BUTTON_DRIVER_OK )->SetWindowPos( NULL, 75, 150, 87, 22, SWP_SHOWWINDOW );
	GetDlgItem( IDC_BUTTON_DRIVER_NO )->SetWindowPos( NULL, 228, 150, 87, 22, SWP_SHOWWINDOW );

	m_ctrlProgress.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL	DriverDownDlg::AutoDownloadAndExecDriver()
{
	CInternetSession	session;
	CInternetFile*		file = NULL;
	try
    {
        file = (CInternetFile *)session.OpenURL( m_strDriverURL );
    }
    catch( CInternetException * pEx )
    {
        TCHAR    lpszError[256];
        pEx->GetErrorMessage( lpszError, 256, NULL );
        return false;
    }

	if( !file )			return false;

	bool			bResult = true;
	FILE			*fp;

	int		iBufferSize = 4096;
	int		iReadByte;
	int		iTotalRecvByte = 0;

	
	int iFileSize = (int) file->GetLength();		//������ ����� �˾Ƴ���.
	CString cFileName = file->GetFileName();		//���� �̸��� ����.
    file->SetReadBufferSize(iBufferSize);

	fp = fopen( cFileName.GetBuffer(0), "wb" );
	if( !fp )
	{
		file->Close();
		delete file;
		session.Close();
	}

	m_strFileName = cFileName.GetBuffer( 0 );
	m_ctrlProgress.SetRange32( 0, iFileSize );

	char* pstrData = new char[iBufferSize];

	while(1)
	{ 
		iReadByte		= file->Read( pstrData, iBufferSize );
		iTotalRecvByte	+= iReadByte;
		if( iReadByte )
		{
			RECT	rect;
			m_ctrlProgress.GetWindowRect( &rect );
			m_ctrlProgress.SetPos( iTotalRecvByte );
			InvalidateRect( &rect );

			int iWriteBytes = (int)fwrite( pstrData, 1, iReadByte, fp );
			if( iWriteBytes != iReadByte )
			{
//				MessageBox( "�ϵ� ��ũ�� ����?" );
				bResult = false;
				break;
			}
		}
		else
			break;
	}

	delete [] pstrData;
	fclose( fp );

	cFileName.ReleaseBuffer();
    file->Close();
	delete file;
	session.Close();

	return bResult;
}

void DriverDownDlg::OnBnClickedButtonDriverOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bSelectResult = TRUE;

	m_ctrlProgress.ShowWindow(SW_SHOW);

	if( AutoDownloadAndExecDriver() )
		m_bDownloadSuccess = TRUE;
		
	OnOK();
}

void DriverDownDlg::OnBnClickedButtonDriverNo()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_bSelectResult = FALSE;
	OnCancel();
}

void DriverDownDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	// �׸��� �޽����� ���ؼ��� CDialog::OnPaint()��(��) ȣ������ ���ʽÿ�.

	CDC		memdc;
	memdc.CreateCompatibleDC( &dc );
	memdc.SelectObject( &m_cBMPBackground );

	BITMAP	cBitmap;
	m_cBMPBackground.GetBitmap( &cBitmap );

	dc.BitBlt( 0, 0, cBitmap.bmWidth, cBitmap.bmHeight, &memdc, 0, 0, SRCCOPY );
	
	//��Ʈó��
	//Text ���� ����.
	dc.SetBkMode(TRANSPARENT);              
	CFont* pOldFont = (CFont*)dc.SelectObject(&m_cDrawFont);
	dc.SetTextColor( RGB(255,255,255) );

	CString	str;
	dc.SetTextColor( RGB(255,0,0) );

	str = "��ũ�ε带 ������ ���� ���ؼ��� �ݵ�� �ֽ�";
	dc.TextOut( 32, 54, str, str.GetLength());
	
	str = "�׷���ī�� ����̹��� �ٿ�ε� �޾ƾ� �մϴ�. ";
	dc.TextOut( 32, 74, str, str.GetLength());

	dc.SetTextColor( RGB(255,255,255) );
	str = "���� �����ø� �ֽ� ����̹��� �ڵ����� ��ġ���ݴϴ�";
	dc.TextOut( 32, 94, str, str.GetLength());

	str = "�ֽ� �׷���ī�� ����̹��� ��ġ�Ͻðڽ��ϱ�?";
	dc.TextOut( 32, 114, str, str.GetLength());

	str = "�������� ����̹� �ڵ�������Ʈ ������� ����";
	dc.TextOut( 55, 190, str, str.GetLength());

	dc.SelectObject(pOldFont);
}

void DriverDownDlg::OnBnClickedCheckDonotusecheckdriver()
{
	BOOL bUse = ! ( (CButton*)GetDlgItem( IDC_CHECK_DONOTUSECHECKDRIVER ) )->GetCheck();
	CPatchClientRegistry cPathchReg;
	cPathchReg.SetUseDriverUpdate( bUse );
}
