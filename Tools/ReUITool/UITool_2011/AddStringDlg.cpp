// AddString.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "AddStringDlg.h"
#include "UITool_2011Doc.h"

// CAddString ��ȭ �����Դϴ�.

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


// CAddString �޽��� ó�����Դϴ�.

void CAddStringDlg::OnBnClickedButtonAdd()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	UpdateData(TRUE);
	
	m_AddStringEdit.GetWindowText(m_strAddSting);

	m_AddList.InsertItem(0, m_strAddSting);

	m_AddStringEdit.Clear();
}

void CAddStringDlg::OnBnClickedButtonDelete()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	POSITION pos = m_AddList.GetFirstSelectedItemPosition();
	if(pos != NULL)
	{
		int nItem = m_AddList.GetNextSelectedItem(pos);
		m_AddList.DeleteItem(nItem);
	}
}

void CAddStringDlg::OnCancel()
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	
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
