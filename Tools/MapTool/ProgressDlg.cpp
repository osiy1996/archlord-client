// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "maptool.h"
#include "ProgressDlg.h"
#include <vector>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	RGB_PROGRESSBACKGROUND	RGB( 72 , 53, 102 )
#define	RGB_PROGRESSBAR			RGB( 255 , 21, 43 )

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

int	CProgressDlg::m_nRefCount = 0;

CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_strPercent = _T("");
	m_strMessage = _T("");
	//}}AFX_DATA_INIT

	srand( (unsigned)time( NULL ) );

	vector< COLORREF >	colorref;
	
	/*
	colorref.push_back( RGB( 255 , 0 , 0	)  );
	colorref.push_back( RGB( 255 , 0 , 222	)  );
	colorref.push_back( RGB( 114 , 0 , 255	)  );
	colorref.push_back( RGB( 0 , 42 , 255	)  );
	colorref.push_back( RGB( 0 , 252 , 255	)  );
	colorref.push_back( RGB( 0 , 255 , 48	)  );
	colorref.push_back( RGB( 138 , 255 , 0	)  );
	colorref.push_back( RGB( 255 , 255 , 0	)  );
	colorref.push_back( RGB( 255 , 144 , 0	)  );
	colorref.push_back( RGB( 53 , 101 , 49	)  );
	colorref.push_back( RGB( 95 , 129 , 205	)  );
	colorref.push_back( RGB( 194 , 119 , 195 ) );
	colorref.push_back( RGB( 195 , 119 , 142 ) );
	colorref.push_back( RGB( 195 , 194 , 119 ) );
	colorref.push_back( RGB( 51 , 51 , 46	 ) );
	colorref.push_back( RGB( 185 , 185 , 175 ) );

	m_rgbBackground = colorref[ rand() % colorref.size() ];
	*/

	m_rgbBackground = RGB( ( rand() % 240 ) , ( rand() % 240 ) , ( rand() % 240 ) );

	CStringList	strList;

	/*
	// �ݿ�
	strList.AddTail( "��! �� Ȥ���ϴ� ����" );
	strList.AddTail( "�𽺰� �Ѱ��̸� ���ְ� �κ��̴�." );
	strList.AddTail( "���� �ݿ� � ������ ������" );

	// ������Ʈ ����
	strList.AddTail( "2005/04/04 ������Ʈ �̵� ����" );
	strList.AddTail( "2005/04/04 ������Ʈ �����ϰ� T��������!" );
	strList.AddTail( "2005/04/06 ���̺� üũ�� �۾������κ���" );
	strList.AddTail( "2005/04/06 ������ ������� ����" );
	strList.AddTail( "2005/04/06 ��������,���콺������ ����Ŭ��!" );
	strList.AddTail( "2005/04/07 ������Ʈ ���� C����ä �巡��." );
	strList.AddTail( "2005/04/07 ������Ʈ ������ t������ 45��ȸ��" );
	strList.AddTail( "2005/04/11 ������Ʈ �׷�ȸ���̵������ϸ�" );
	strList.AddTail( "2005/04/11 ������Ʈ �׷� ����&��� ����߰�" );
	strList.AddTail( "2005/04/11 ���� Į�� ���� �پ�������!" );
	strList.AddTail( "2005/04/12 ������Ʈ�̵��� ���� �뷮������" );
	strList.AddTail( "2005/04/12 ������Ʈ���ø� �ε��ӵ�����" );
	strList.AddTail( "2005/04/13 ������Ʈ ���� �� ���� ���� -,.-" );
	strList.AddTail( "2005/04/15 ���̺� ����Ÿ ����" );
	strList.AddTail( "2005/05/16 ��Ʈ�� �ͽ���Ʈ ����" );
	strList.AddTail( "2005/05/16 ������Ʈ ����ǥ�� �ɼ��߰�" );

	// ��
	strList.AddTail( "�̰��� ���ο� ���� �� �ý���" );
	strList.AddTail( "F5 , F6 ���� ī�޶� ��ġ����/�ε�" );
	strList.AddTail( "F7�� �̺�Ʈ ������Ʈã��,Ctrl F7�μ���" );
	strList.AddTail( "Alt+S �� ���̺�" );
	strList.AddTail( "���̺�� ������ SubData�� ���ÿ��ſ�" );
	strList.AddTail( "�̴ϸ��� ������ Minimap ������ ����" );
	strList.AddTail( "Ŀ�ǵ� cms�� ī�޶� �̵� �ӵ�����" );
	strList.AddTail( "Ŀ�ǵ� rs�� �ɸ��� �޸��� �ӵ� ����" );
	strList.AddTail( "Ÿ���ǿ��� I�� Ÿ�� ��ŷ" );
	strList.AddTail( "����� ��Ȱȭ!" );
	strList.AddTail( "������Ʈ+��Ʈ������Ÿ�� �׻� �������" );
	strList.AddTail( "������Ʈ �׷� ���� ���콺�����ʴ���Ŭ��" );
	strList.AddTail( "������Ʈ Shift Drag �� �����̵�" );
	strList.AddTail( "������Ʈ Control Drag �� �����ϸ�" );
	strList.AddTail( "������Ʈ Shift+Control Drag �� ȸ��" );
	strList.AddTail( "������Ʈ�� ����� ��踦 �Ѿ�� ���ؿ�" );
	strList.AddTail( "������Ʈ �׷켱�ý� ctrl�� ��ڿ��͵� ����" );
	strList.AddTail( "1��ư���� �����޴�" );
	strList.AddTail( "2��ư���� Ÿ�ϸ޴�" );
	strList.AddTail( "3��ư���� ������Ʈ�޴�" );
	strList.AddTail( "���������쿡�� ������ư �ι����� ī�޶��̵�" );
	strList.AddTail( "������Ʈ â���� CTRL + A �� ��ü����" );
	strList.AddTail( "ALT + Drag�� ������Ʈ �׷켱��" );
	strList.AddTail( "ALT + Shift Drag�� ������Ʈ �׷켱���߰�" );
	strList.AddTail( "ALT + Ctrl Drag�� ������Ʈ �׷켱�û���" );
	strList.AddTail( "�ɼǸ޴��� ������Ʈ ���� ǥ�� �߰�" );

	// ���
	strList.AddTail( "����� ������~" );
	strList.AddTail( "������ ������" );
	strList.AddTail( "������ ���� ������." );
	strList.AddTail( "��ũ�ε� ������" );
	strList.AddTail( "������ ������" );
	strList.AddTail( "����� ����" );
	strList.AddTail( "Maptool May Cry" );
	strList.AddTail( "���� �η��� �� , ���" );
	strList.AddTail( "������ �̹� �׾� �ִ�." );
	strList.AddTail( "������ ����" );
	strList.AddTail( "���� ��� �ָ���" );
	strList.AddTail( "�Ϳ����� ���� ������" );
	strList.AddTail( "�� �������� ���� ��� �ִ°�?" );
	strList.AddTail( "��������" );
	strList.AddTail( "�Ļ�� ���� ���� ���� ����" );
	strList.AddTail( "���� ������ �������� ���� �˰��ִ�." );
	strList.AddTail( "������ �츮��" );
	strList.AddTail( "�״� �׸��� ����" );
	strList.AddTail( "���� �ʸ� ������ ��" );
	strList.AddTail( "The Legend of Maptool" );
	strList.AddTail( "�����ڴ��� ���� ���" );
	strList.AddTail( "�״��� ����� �װ��� �����δ�." );
	strList.AddTail( "�����ִ� ���� ���̴�." );
	strList.AddTail( "������ �Ⱑ ����" );
	strList.AddTail( "�� �׿� �����ڴ�!" );
	strList.AddTail( "������ �׶� �׶� �޶��." );
	strList.AddTail( "ȥ�ڰ� �ƴ� ����" );
	strList.AddTail( "���ض�. �����ֱ׵����̰�" );
	strList.AddTail( "������ �� �� �̷�� ��" );
	strList.AddTail( "The Next Maptool" );
	*/

	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".............................." );
	strList.AddTail( ".....................!........" );
	strList.AddTail( ".............?................" );
	strList.AddTail( "........!!...................." );
	strList.AddTail( ".........................$$..." );
	strList.AddTail( ".......................(--  )." );
	strList.AddTail( ".................2%..........." );
	strList.AddTail( "..(+- -)......................" );
	strList.AddTail( "...�⸧�� ���� �ְ�ġ ������.." );
	strList.AddTail( ".........................#...." );

	int		nRand = rand() % strList.GetCount();
	POSITION	pos = strList.GetHeadPosition();
	while( nRand-- ) strList.GetNext( pos );
	m_strMaptoolMessage	= strList.GetAt( pos );

	m_nPopupLevel = m_nRefCount ++;
}

CProgressDlg::~CProgressDlg()
{
	m_nRefCount --;
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Text(pDX, IDC_PERCENT, m_strPercent);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_PAINT()
	ON_WM_ACTIVATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL	CProgressDlg::StartProgress( CString message , int target , CWnd * pParent )
{
	m_strMessage	= message	;
	m_nTarget		= target	;
	m_nCurrent		= 0			;

	return Create( IDD , pParent );
}

int		CProgressDlg::SetProgress( int progress )
{
	if( ( int ) ( 100.0f * ( float ) progress / ( float ) m_nTarget ) == ( int ) ( 100.0f * ( float ) m_nCurrent / ( float ) m_nTarget ) )
	{
		// ȭ�� ������Ʈ ����;;
		// m_nCurrent = progress;
		return m_nCurrent;
	}

	// ������ (2004-06-28 ���� 8:28:39) : Į�������
	Render();

	m_nCurrent = progress;

	m_strPercent.Format( "%d%%" , ( int ) ( 100.0f * ( float ) progress / ( float ) m_nTarget ) );

	return m_nCurrent;
}

void		CProgressDlg::EndProgress()
{
	DestroyWindow();
}

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// ������ (2004-06-28 ���� 8:42:17) : ��ũ�� ��Ʈ ���..
	GetDlgItem( IDC_PROGRESS )->GetWindowRect( m_rectProgress );
	ScreenToClient( m_rectProgress );
	GetDlgItem( IDC_PROGRESS )->DestroyWindow();

	CRect	rect;
	GetClientRect( rect );

	CDC * pDC = GetDC();
	m_bmpDC.DeleteObject();
	m_memDC.DeleteDC();
	m_bmpDC.CreateCompatibleBitmap( pDC , rect.Width() , rect.Height() );
	m_memDC.CreateCompatibleDC( pDC );
	m_memDC.SelectObject( m_bmpDC );
	ReleaseDC( pDC );

	SetWindowText( m_strMessage );
	
	GetWindowRect( rect );
	CRect	rectNew = rect;

	rectNew.SetRect(
		0,
		0,
		rect.Width(),
		rect.Height() );

	rectNew.left	+= 100 * ( m_nRefCount );
	rectNew.top		+= rect.Height() * ( m_nRefCount );
	rectNew.right	+= 100 * ( m_nRefCount );
	rectNew.bottom	+= rect.Height() * ( m_nRefCount );
	
	MoveWindow( rectNew );
	//SetWindowPos( &wndTopMost , rect.left , rect.top + m_nRefCount * rect.Height(), rect.Width() , rect.Height() , SWP_DRAWFRAME | SWP_NOMOVE );

	ShowWindow( SW_SHOW );
	Render();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL	CProgressDlg::SetMessage		( CString str )
{
	SetWindowText( str );
	return TRUE;
}

int		CProgressDlg::SetTarget		( int progress )
{
	m_nTarget	= progress;
	return 1;
}

void CProgressDlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	Render();
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CProgressDlg::Render( CDC * pDC )
{
	BOOL	bGetDC = FALSE;
	if( NULL == pDC )
	{
		pDC = GetDC();
		bGetDC	= TRUE;
	}

	// ����������~..

	CRect	rect;
	GetClientRect( rect );
	m_memDC.FillRect( rect , &ALEF_BRUSH_BACKGROUND );

	CString	str;
	str.Format( "(%d/%d) ������" , m_nCurrent + 1 , m_nTarget );
	m_memDC.SetBkMode( TRANSPARENT );
	m_memDC.SetTextColor( RGB( 255 , 255 , 168 ) );
	m_memDC.TextOut( 5 , 5 , str );

	INT32	nWidth;
	nWidth = ( INT32 ) ( ( FLOAT ) m_rectProgress.Width() * ( FLOAT ) m_nCurrent / ( FLOAT ) m_nTarget );
	m_memDC.FillSolidRect( m_rectProgress.left - 1 , m_rectProgress.top - 1 , m_rectProgress.Width() + 2 , m_rectProgress.Height() + 2 , RGB( 255 , 255 , 255 ) );
	
	m_memDC.FillSolidRect( m_rectProgress.left , m_rectProgress.top , nWidth , m_rectProgress.Height() , m_rgbBackground );

	CRect rectBox = m_rectProgress;
	m_memDC.SetBkMode( TRANSPARENT );
	m_memDC.SetTextColor( RGB( 0 , 0 , 0 ) );
	m_memDC.DrawText( m_strMaptoolMessage , &rectBox , DT_SINGLELINE | DT_CENTER | DT_VCENTER );
	rectBox.MoveToX( m_rectProgress.left - 1 );
	rectBox.MoveToY( m_rectProgress.top - 1 );
	m_memDC.SetTextColor( RGB( 255 , 255 , 255 ) );
	m_memDC.DrawText( m_strMaptoolMessage , &rectBox , DT_SINGLELINE | DT_CENTER | DT_VCENTER );

	m_memDC.FillSolidRect( m_rectProgress.left + nWidth , m_rectProgress.top , m_rectProgress.Width() - nWidth , m_rectProgress.Height() , RGB_PROGRESSBACKGROUND );

	pDC->BitBlt( 0 , 0 , rect.Width() , rect.Height() , &m_memDC , 0 , 0 , SRCCOPY );

	if( bGetDC )
	{
		ReleaseDC( pDC );
	}
}

void CProgressDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);
	
	Render();
}
