// ItemSelectDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "RegionToolDlg.h"
#include "ItemSelectDlg.h"
#include ".\itemselectdlg.h"


// CItemSelectDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CItemSelectDlg, CDialog)
CItemSelectDlg::CItemSelectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CItemSelectDlg::IDD, pParent)
{
	m_nItemID	= -1;
}

CItemSelectDlg::~CItemSelectDlg()
{
}

void CItemSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
}


BEGIN_MESSAGE_MAP(CItemSelectDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST, OnLbnDblclkList)
	ON_BN_CLICKED(IDC_NONEEDITEM, OnBnClickedNoneeditem)
END_MESSAGE_MAP()


// CItemSelectDlg �޽��� ó�����Դϴ�.

void CItemSelectDlg::OnLbnDblclkList()
{
	int	nIndex = m_ctlList.GetCurSel();
	CString	str;
	m_ctlList.GetText( nIndex , str );

	m_nItemID	= atoi( ( LPCTSTR ) str );

	OnOK();
}

BOOL CItemSelectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AgpdItemTemplate* pcsItemTemplate;
	INT32					lIndex = 0;
	CString	str;

	AgpaItemTemplate::iterator	Iter	=	g_pcsAgpmItem->csTemplateAdmin.begin();
	for( ; Iter != g_pcsAgpmItem->csTemplateAdmin.end() ; ++Iter )
	{
		pcsItemTemplate		=	Iter->second;
		str.Format( "%04d , %s" , pcsItemTemplate->m_lID , pcsItemTemplate->m_szName );
		m_ctlList.AddString( str );
	}

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CItemSelectDlg::OnBnClickedNoneeditem()
{
	m_nItemID	= -1;
	UpdateData( FALSE );

	OnOK();
}
