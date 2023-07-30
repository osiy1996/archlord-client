// DlgAgreement.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "PatchClient.h"
#include "DlgAgreement.h"
#include "PatchClientDlg.h"


// CDlgAgreement ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CDlgAgreement, CDialog)

CDlgAgreement::CDlgAgreement(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAgreement::IDD, pParent)
{
	m_bIsChecked = TRUE;
}

CDlgAgreement::~CDlgAgreement()
{
}

void CDlgAgreement::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgAgreement, CDialog)
	ON_BN_CLICKED(IDB_AGREE, &CDlgAgreement::OnBnClickedAgree)
	ON_BN_CLICKED(IDC_CHECK_AGREE, &CDlgAgreement::OnBnClickedCheckAgree)
END_MESSAGE_MAP()


// CDlgAgreement �޽��� ó�����Դϴ�.
BOOL CDlgAgreement::OnInitDialog( void )
{
	// ���ڿ� ����
	CWnd* pTextMsg = GetDlgItem( IDC_EDIT_AGREEMSG );
	if( pTextMsg )
	{
		CString strText;
		strText.LoadString( IDS_PVPAGREE_MSG );

		pTextMsg->SetWindowText( strText.GetBuffer() );
	}

	pTextMsg = GetDlgItem( IDC_CHECK_AGREE );
	if( pTextMsg )
	{
		CString strText;
		strText.LoadString( IDS_PVPAGREE_CHECK );

		pTextMsg->SetWindowText( strText.GetBuffer() );
	}

	// �⺻������ üũ���·� �ֵ���..
	CButton* pBtn = ( CButton* )GetDlgItem( IDC_CHECK_AGREE );
	if( pBtn )
	{
		pBtn->SetCheck( BST_CHECKED );
	}

	return CDialog::OnInitDialog();
}

void CDlgAgreement::OnBnClickedAgree()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	// üũ��ư�� ����üũ�� �������� ���� �ƴϸ� �� �Ѿ��.
	if( m_bIsChecked )
	{
		EndDialog( 0 );
	}
	else
	{
		EndDialog( 1 );
		_exit( 0 );
	}
}

void CDlgAgreement::OnBnClickedCheckAgree()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CButton* pButton = ( CButton* )GetDlgItem( IDB_AGREE );
	if( !pButton ) return;

	m_bIsChecked = IsDlgButtonChecked( IDC_CHECK_AGREE ) ? TRUE : FALSE;
	int nID = m_bIsChecked ? IDS_PVPAGREE_OK : IDS_PVPAGREE_NO;

	CString strText;
	strText.LoadString( nID );

	pButton->SetWindowText( strText.GetBuffer() );
}
