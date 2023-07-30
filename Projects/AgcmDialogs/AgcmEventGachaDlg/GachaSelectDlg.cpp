// GachaSelectDlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "../resource.h"		// main symbols
#include "AgcmEventGachaDlg.h"

#include "GachaSelectDlg.h"


const char g_strFormat[] = "%d,%s";

// CGachaSelectDlg ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CGachaSelectDlg, CDialog)

CGachaSelectDlg::CGachaSelectDlg(AgcmEventGachaDlg * pParent , INT32 nIndex )
	: CDialog(CGachaSelectDlg::IDD, NULL) , m_pParentModule( pParent ) , m_nIndex( nIndex ),
	m_pMap ( NULL )
	, m_strSelectedGacha(_T(""))
{

}

CGachaSelectDlg::~CGachaSelectDlg()
{
}

void CGachaSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GACHA, m_listGacha);
	DDX_Text(pDX, IDC_SELECTED_GACHA, m_strSelectedGacha);
}


BEGIN_MESSAGE_MAP(CGachaSelectDlg, CDialog)
	ON_LBN_SELCHANGE(IDC_LIST_GACHA, &CGachaSelectDlg::OnLbnSelchangeListGacha)
END_MESSAGE_MAP()


// CGachaSelectDlg �޽��� ó�����Դϴ�.

BOOL CGachaSelectDlg::OnInitDialog()
{
	UpdateData( FALSE );

	// ����Ʈ�ڽ��� ����Ÿ �߰�
	m_pMap = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeMap();
	map< INT32	, AgpdGachaType	>::iterator iter;

	for( iter = m_pMap->begin();
		iter != m_pMap->end();
		iter ++ )
	{
		INT32				nGachaIndex	= iter->first	;
		AgpdGachaType *		pGachaType	= &iter->second	;

		CString	str;

		str.Format( g_strFormat , nGachaIndex , pGachaType->strName.c_str() );

		m_listGacha.AddString( str );
	}

	if( m_nIndex )
	{
		AgpdGachaType * pGachaType = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeInfo( m_nIndex );

		if( pGachaType )
		{
			m_strSelectedGacha.Format( g_strFormat , m_nIndex , pGachaType->strName.c_str() );
		}
		else
		{
			m_strSelectedGacha.Format( g_strFormat , m_nIndex , "�ش��ε����� ��í Ÿ���� �����" );
			m_nIndex = 0;
		}
	}
	else
	{
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , "������ ������ �����" );
	}

	UpdateData( FALSE );

	return TRUE;
}

// ����Ʈ�ڽ� ���� �̺�Ʈ �������̵�
// On OK �������̵�
void CGachaSelectDlg::OnLbnSelchangeListGacha()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	UpdateData( TRUE );

	int	nIndex = m_listGacha.GetCurSel();
	CString	str;
	m_listGacha.GetText( nIndex , str );

	INT32	nGachaIndex;
	nGachaIndex = atoi( (LPCTSTR) str );

	m_nIndex = nGachaIndex;

	AgpdGachaType * pGachaType = m_pParentModule->m_pcsAgpmEventGacha->GetGachaTypeInfo( nGachaIndex );

	if( pGachaType )
	{
		CString	str;
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , pGachaType->strName.c_str() );
	}
	else
	{
		m_strSelectedGacha.Format( g_strFormat , m_nIndex , "�ش��ε����� ��í Ÿ���� �����" );
		m_nIndex = 0;
	}

	UpdateData( FALSE );
}
