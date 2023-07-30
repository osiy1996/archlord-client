// FileSearch.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "FileSearch.h"
#include "MainFrm.h"
#include "UITool_2011Doc.h"

// CFileSearch ��ȭ �����Դϴ�.

IMPLEMENT_DYNAMIC(CFileSearch, CDialog)

CFileSearch::CFileSearch(CWnd* pParent /*=NULL*/)
	: CDialog(CFileSearch::IDD, pParent)
	, m_strSearch(_T("")), m_SearchPosItem(NULL)
{

}

CFileSearch::~CFileSearch()
{
}

void CFileSearch::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SEARCH, m_strSearch);
	DDX_Control(pDX, IDC_EDIT_SEARCH, m_EditSearch);
}


BEGIN_MESSAGE_MAP(CFileSearch, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH, &CFileSearch::OnBnClickedButtonSearch)
//	ON_WM_SETFOCUS()
//ON_WM_ACTIVATE()
ON_EN_SETFOCUS(IDC_EDIT_SEARCH, &CFileSearch::OnEnSetfocusEditSearch)
END_MESSAGE_MAP()


// CFileSearch �޽��� ó�����Դϴ�.

void CFileSearch::OnBnClickedButtonSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	m_EditSearch.GetWindowText(m_strSearch);
	if(m_strSearch.Compare(_T("")) != 0)
	{
		CMainFrame* mFrm = (CMainFrame*)AfxGetMainWnd();
		
		HTREEITEM hItem = NULL;
		
		while(hItem != mFrm->m_wndFileView.m_wndFileView.GetRootItem())
		{
			hItem = mFrm->m_wndFileView.GetTreeItemFromItemText(m_strSearch, m_SearchPosItem, TRUE, TRUE);
			m_SearchPosItem = mFrm->m_wndFileView.NextItem(m_SearchPosItem);
			if(hItem != NULL)
				break;
		}

		mFrm->m_wndFileView.m_wndFileView.SelectItem(hItem);	
		CString strID = mFrm->m_wndFileView.m_wndFileView.GetItemText(hItem);

		CUITool_2011Doc* pDoc = (CUITool_2011Doc*)mFrm->GetActiveDocument();
		pDoc->CurrentPickUpObjUpdate(pDoc->GetItemInTheList(strID));
		mFrm->GetActiveWindow()->Invalidate();
	}
	else
	{
		AfxMessageBox(_T("�˻�� �Է��� �ּ���."));
	}
}

void CFileSearch::OnEnSetfocusEditSearch()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	/*m_EditSearch.GetWindowText(m_strSearch);
	CMainFrame* mFrm = (CMainFrame*)AfxGetMainWnd();
	m_SearchPosItem = mFrm->m_wndFileView.m_wndFileView.GetSelectedItem();*/
}
