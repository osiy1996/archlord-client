// AddString.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "AddStringDlg.h"
#include "UITool_2011Doc.h"

// CAddString 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAddStringDlg, CDialog)

CAddStringDlg::CAddStringDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAddStringDlg::IDD, pParent)
	, m_strAddSting(_T(""))
{

}

CAddStringDlg::~CAddStringDlg()
{
}

void CAddStringDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ADDSTRING, m_AddStringEdit);
	DDX_Text(pDX, IDC_EDIT_ADDSTRING, m_strAddSting);
	DDX_Control(pDX, IDC_LIST_ADD, m_AddList);
}


BEGIN_MESSAGE_MAP(CAddStringDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CAddStringDlg::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CAddStringDlg::OnBnClickedButtonDelete)
END_MESSAGE_MAP()


// CAddString 메시지 처리기입니다.

void CAddStringDlg::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	m_AddStringEdit.GetWindowText(m_strAddSting);

	m_AddList.InsertItem(0, m_strAddSting);

	m_AddStringEdit.Clear();
}

void CAddStringDlg::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	POSITION pos = m_AddList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_AddList.GetNextSelectedItem(pos);
		m_AddList.DeleteItem(nItem);
	}
}

void CAddStringDlg::OnCancel()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	
	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	if(pDoc != NULL && pDoc->GetPickUpObject() != NULL)
	{
		CUIToolComboBox* pCombo = (CUIToolComboBox*)pDoc->GetPickUpObject();
		if(pCombo)
		{
			for(int i = 0; i < m_AddList.GetItemCount(); i++)
			{
				CString str = m_AddList.GetItemText(i, 0);
				pCombo->m_ComboBox->AddString(str);
			}
		}

		CUIToolListBox* pList = (CUIToolListBox*)pDoc->GetPickUpObject();
		if(pList)
		{
			for(int i = 0; i < m_AddList.GetItemCount(); i++)
			{
				CString str = m_AddList.GetItemText(i, 0);
				pList->m_ListBox->AddString(str);
			}
		}
	}

	CDialog::OnCancel();
}
