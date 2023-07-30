// EventNatureSettingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "../resource.h"
#include "EventNatureSettingDlg.h"
#include "TemplateSelectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEventNatureSettingDlg dialog


CEventNatureSettingDlg::CEventNatureSettingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventNatureSettingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEventNatureSettingDlg)
	m_fRange = 0.0f;
	//}}AFX_DATA_INIT
}


void CEventNatureSettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEventNatureSettingDlg)
	DDX_Control(pDX, IDC_WEATHER_TEMPLATE_COMBO, m_ctlWeatherCombo);
	DDX_Text(pDX, IDC_RANGE, m_fRange);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEventNatureSettingDlg, CDialog)
	//{{AFX_MSG_MAP(CEventNatureSettingDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEventNatureSettingDlg message handlers

int CEventNatureSettingDlg::DoModal() 
{
	ASSERT( !"�̰� ȣ���ϸ�ȵſ�!" );
	
	return IDCANCEL;
}

UINT CEventNatureSettingDlg::DoModal( AgcmEventNatureDlg * pParent , ApdEvent * pstEvent) 
{
	ASSERT( NULL != pParent			);

	m_csAgcmEventNatureDlg	= pParent	;
	m_pstEvent				= pstEvent	;
	
	return CDialog::DoModal();
}

BOOL CEventNatureSettingDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	AuList< AgpdSkySet * >	* pList = m_csAgcmEventNatureDlg->m_pcsAgpmEventNature->GetSkySetList();
	AuNode< AgpdSkySet * >	* pNode	= pList->GetHeadNode();
	AgpdSkySet				* pSkySet	;

	int						count = 0;
	CString					str;
	int						nIndex;
	while( pNode )
	{
		pSkySet	= pNode->GetData();

		str.Format( TEMPLATE_LIST_FORMAT , pSkySet->m_nIndex , pSkySet->m_strName );
		nIndex = m_ctlWeatherCombo.AddString( str );

		if( ( INT32 ) m_pstEvent->m_pvData == pSkySet->m_nIndex )
		{
			// �̳��̷ȴ�!..
			m_ctlWeatherCombo.SetCurSel( nIndex );
		}

		count ++;

		pList->GetNext( pNode );
	}

	TRACE( "CEventNatureSettingDlg::OnInitDialog %d���� ���ø� �߰�.." , count );

	UpdateData( TRUE );
	// ����Ÿ ����..
	ASSERT( NULL != m_pstEvent );
	if( m_pstEvent && m_pstEvent->m_pstCondition && m_pstEvent->m_pstCondition->m_pstArea )
	{
		// ����.. �� ����..
		m_fRange = m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius / 100.0f; // ���ʹ��� ȯ����..
	}
	else
	{
		// ������� �����ž� ���� �ʽ�..
	}

	UpdateData( FALSE );
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CEventNatureSettingDlg::OnOK() 
{
	UpdateData( TRUE );

	// ����Ÿ ���� ����..
	VERIFY( m_csAgcmEventNatureDlg->m_pcsApmEventManager->SetCondition( m_pstEvent, APDEVENT_COND_AREA ) );

	m_pstEvent->m_pstCondition->m_pstArea->m_eType					= APDEVENT_AREA_SPHERE	;
	m_pstEvent->m_pstCondition->m_pstArea->m_uoData.m_fSphereRadius	= m_fRange * 100.0f		;// ���ʹ�����..


	CString	str;
	m_ctlWeatherCombo .GetWindowText( str );

	int	nTemplateID;
	nTemplateID	= atoi( (LPCTSTR) str );

	m_pstEvent->m_pvData		= ( PVOID ) nTemplateID;

	TRACE( "%d ���ø� ���� .. (%s)\n" , nTemplateID , str );

	CDialog::OnOK();
}
