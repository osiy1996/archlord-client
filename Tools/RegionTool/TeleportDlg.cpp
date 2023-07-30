// TeleportDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "TeleportDlg.h"
#include ".\teleportdlg.h"


// CTeleportDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CTeleportDlg, CDialog)
CTeleportDlg::CTeleportDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTeleportDlg::IDD, pParent)
	, m_strMessage(_T(""))
	, m_bShow(TRUE)
{
}

CTeleportDlg::~CTeleportDlg()
{
}

void CTeleportDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_DISPLAY, m_strMessage);
	DDX_Check(pDX, IDC_SHOWTHISBOXAGAIN, m_bShow);
}


BEGIN_MESSAGE_MAP(CTeleportDlg, CDialog)
	ON_BN_CLICKED(IDC_SHOWTHISBOXAGAIN, OnBnClickedShowthisboxagain)
END_MESSAGE_MAP()


// CTeleportDlg �޽��� ó�����Դϴ�.

void CTeleportDlg::OnBnClickedShowthisboxagain()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	m_bShow = FALSE;
	OnOK();
}
