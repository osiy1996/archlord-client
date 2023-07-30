// ControlMenuDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "ControlMenuDlg.h"
#include "UITool_2011Doc.h"
#include "UITool_2011View.h"

// CControlMenuDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CControlMenuDlg, CDialog)
	
CControlMenuDlg::CControlMenuDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CControlMenuDlg::IDD, pParent)
{
}

CControlMenuDlg::~CControlMenuDlg()
{
}

void CControlMenuDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CControlMenuDlg, CDialog)
	ON_BN_CLICKED(IDC_UI, &CControlMenuDlg::OnBnClickedUi)
	ON_BN_CLICKED(IDC_CTRLDELETE, &CControlMenuDlg::OnBnClickedCtrldelete)
	ON_BN_CLICKED(IDC_EDIT, &CControlMenuDlg::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_BUTTON, &CControlMenuDlg::OnBnClickedButton)
	ON_BN_CLICKED(IDC_STATICEDIT, &CControlMenuDlg::OnBnClickedStaticedit)
	ON_BN_CLICKED(IDC_COMBOBOX, &CControlMenuDlg::OnBnClickedCombobox)
	ON_BN_CLICKED(IDC_LISTBOX, &CControlMenuDlg::OnBnClickedListbox)
END_MESSAGE_MAP()

// CControlMenuDlg 메시지 처리기입니다.

void CControlMenuDlg::SetCreate(CPoint pStart, CPoint pEnd)
{
	UINT ObjNum = 0;
	CString strMsg, str, str_class, count;
	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* NewObj = NULL;

	if(m_CurSelectObj != E_UI)
	{
		if(!pDoc || !pDoc->GetPickUpObject())
		{
			AfxMessageBox(_T("기본 UI를 먼저 생성한 후에 사용하십시요."));
			return;
		}

		strMsg = pDoc->GetPickUpObject()->m_ID;
		strMsg += _T("를 부모로 지정하겠습니까?");
 
		str = pDoc->GetPickUpObject()->m_ID;
	}
	
	switch(m_CurSelectObj)
	{
	case E_UI:
		{
			CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();

			ObjNum = pDoc->m_ObjectList.size()+1;
			str.Format( _T("%d"), ObjNum);   
			str = _T("UI_") + str;
			str_class = _T("UI");

			NewObj = (CUIToolBaseObject*)new CUIToolUI();
			break;
		}
	case E_BUTTON:
		{
			NewObj = (CUIToolBaseObject*)new CUIToolButton();
			str_class = _T("Button");
			str += _T("_Button_");
			break;
		}
	case E_STATIC:
		{
			NewObj = (CUIToolBaseObject*)new CUIToolStatic();
			str_class = _T("Static");
			str += _T("_Static_");
			break;
		}
	case E_EDIT:
		{
			NewObj = (CUIToolBaseObject*)new CUIToolEdit();
			str_class = _T("Edit");
			str += _T("_Edit_");
			break;		
		}
	case E_SCROLLBAR:
		break;
	case E_COMBOBOX:
		{
			NewObj = (CUIToolBaseObject*)new CUIToolComboBox();
			str_class = _T("ComboBox");
			str += _T("_ComboBox_");
			break;		
		}
	case E_LISTBOX:
			NewObj = (CUIToolBaseObject*)new CUIToolListBox();
			str_class = _T("ListBox");
			str += _T("_ListBox_");
		break;
	case E_TREE:
		break;
	case E_GRID:
		break;

	}
	
	if(m_CurSelectObj != E_UI)
	{
		ObjNum = pDoc->GetPickUpObject()->m_ChildList.size()+1;
		count.Format( _T("%d"), ObjNum);  
		str += count;

		if(AfxMessageBox(strMsg, MB_OKCANCEL, MB_ICONEXCLAMATION) == TDCBF_OK_BUTTON)
		{
			NewObj->m_parent = pDoc->GetPickUpObject();
		}
		else
		{			
			std::list<CUIToolBaseObject*>::iterator iter = pDoc->m_ObjectList.begin();
			NewObj->m_parent = (*iter);
		}
	}
	
	NewObj->Create(str, pStart, pEnd);

}

void CControlMenuDlg::OnBnClickedUi()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_UI;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}

void CControlMenuDlg::OnBnClickedCtrldelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame* Mfm = (CMainFrame*)AfxGetMainWnd();
	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)Mfm->GetActiveDocument();

	CUIToolBaseObject* tmp = pDoc->GetItemInTheList(pDoc->GetPickUpObject()->m_ID);
	if(tmp != NULL)
	{
		Mfm->m_wndFileView.SetSelectItemDelete();
	
		pDoc->CurrentPickUpObjUpdate(NULL);
		
		if(tmp->m_parent != NULL)
		{
			for(std::list<CUIToolBaseObject*>::iterator iter = tmp->m_parent->m_ChildList.begin(); iter != tmp->m_parent->m_ChildList.end();)
			{
				if((*iter) == tmp)
				{
					(*iter)->RemoveProperties();
					tmp->m_parent->m_ChildList.erase(iter++);
				}
				else
					iter++;
			}
		}
		else
		{
			for(std::list<CUIToolBaseObject*>::iterator iter = pDoc->m_ObjectList.begin(); iter != pDoc->m_ObjectList.end();)
			{
				if((*iter) == tmp)
				{
					(*iter)->RemoveProperties();
					pDoc->m_ObjectList.erase(iter++);
				}
				else
					iter++;
			}
		}
		
	}


	Mfm->Invalidate();		
}


void CControlMenuDlg::OnBnClickedEdit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_EDIT;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}

void CControlMenuDlg::OnBnClickedButton()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_BUTTON;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}

void CControlMenuDlg::OnBnClickedStaticedit()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_STATIC;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}

void CControlMenuDlg::OnBnClickedCombobox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_COMBOBOX;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}

void CControlMenuDlg::OnBnClickedListbox()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_CurSelectObj = E_LISTBOX;

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	pDoc->m_bTrack = true;	
}
