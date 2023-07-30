// ServerEditDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "Patcher2.h"
#include "ServerEditDlg.h"


// CServerEditDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CServerEditDlg, CDialog)

CServerEditDlg::CServerEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CServerEditDlg::IDD, pParent)
	, m_NameData(_T(""))
	, m_IPData(_T(""))
{

}

CServerEditDlg::~CServerEditDlg()
{
}

void CServerEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_NAME_EDIT	, m_NameData);
	DDX_Text(pDX, IDC_IP_EDIT	, m_IPData	);
}


BEGIN_MESSAGE_MAP(CServerEditDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CServerEditDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CServerEditDlg �޽��� ó�����Դϴ�.



void CServerEditDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData( TRUE );
	UpdateData( FALSE );


	OnOK();
}
