// SelectSkySetDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "RegionTool.h"
#include "SelectSkySetDlg.h"
#include "RegionToolDlg.h"

// CSelectSkySetDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNCREATE(CSelectSkySetDlg, CDHtmlDialog)

CSelectSkySetDlg::CSelectSkySetDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectSkySetDlg::IDD, pParent)
{
	m_nItemID	= 0;
}

CSelectSkySetDlg::~CSelectSkySetDlg()
{
}

void CSelectSkySetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST, m_ctlList);
}

BOOL CSelectSkySetDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	AuList< AgpdSkySet * >	* pList = g_pcsAgpmEventNature->GetSkySetList();
	AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
	AgpdSkySet				* pSkySet	;

	CString					str;
	while( pNode )
	{
		pSkySet	= pNode->GetData();

		str.Format( SKY_TEMPLATE_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
		m_ctlList.AddString( str );

		pList->GetNext( pNode );
	}


	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

BEGIN_MESSAGE_MAP(CSelectSkySetDlg, CDialog)
	ON_LBN_DBLCLK(IDC_LIST, OnLbnDblclkList)
END_MESSAGE_MAP()

void CSelectSkySetDlg::OnLbnDblclkList()
{
	int	nIndex = m_ctlList.GetCurSel();
	CString	str;
	m_ctlList.GetText( nIndex , str );

	m_nItemID	= atoi( ( LPCTSTR ) str );
	OnOK();
}
