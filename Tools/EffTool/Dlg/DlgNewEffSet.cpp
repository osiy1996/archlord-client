#include "stdafx.h"
#include "..\EffTool.h"
#include "DlgNewEffSet.h"

#include "AcuMathFunc.h"
USING_ACUMATH;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CDlgNewEffSet::CDlgNewEffSet(CWnd* pParent /*=NULL*/) : CDialog(CDlgNewEffSet::IDD, pParent)
{
	m_dwEffSetID = 0;
	m_strTitle = _T("");
}

void CDlgNewEffSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_ID, m_dwEffSetID);
	DDX_Text(pDX, IDC_EDIT_TITLE, m_strTitle);
	DDV_MaxChars(pDX, m_strTitle, 31);
}

BEGIN_MESSAGE_MAP(CDlgNewEffSet, CDialog)
	//{{AFX_MSG_MAP(CDlgNewEffSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CDlgNewEffSet::OnOK() 
{
	UpdateData(TRUE);

	// TODO: Add extra validation here
	if( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->ForTool_FindEffSet(m_dwEffSetID) )
	{
		ErrToWnd("�ش� ���̵� ���� �մϴ�!");
		return;
	}

	if( EFF2_BASE_TITLE_MAX_NUM <= m_strTitle.GetLength() )
	{
		ErrToWnd("����Ʈ�� �̸��� �ʹ� ��ϴ�!");
		return;
	}

	LPEFFSET pNewEffSet	= new AgcdEffSet(m_dwEffSetID);
	if( !pNewEffSet )
	{
		ErrToWnd("�޸� �Ҵ� ����!");
		CDialog::OnOK();
		return;
	}

	pNewEffSet->bSetTitle( m_strTitle );
	pNewEffSet->bSetLoopOpt( e_TblDir_infinity );

	if( T_ISMINUS4( AgcdEffGlobal::bGetInst().bGetPtrAgcmEff2()->ForTool_InsEffSet(pNewEffSet) ) )
	{
		ErrToWnd("�߰��� ����..!");
		DEF_SAFEDELETE( pNewEffSet );
	}
	
	CDialog::OnOK();
}
