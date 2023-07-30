// MessageEditDlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "UITool_2011.h"
#include "MessageEditDlg.h"
#include "UITool_2011Doc.h"


// CMessageEditDlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMessageEditDlg, CDialog)

CMessageEditDlg::CMessageEditDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMessageEditDlg::IDD, pParent)
	, m_strAction(_T(""))
	, m_strMessage(_T(""))
{
}

CMessageEditDlg::~CMessageEditDlg()
{
}

void CMessageEditDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MESSAGE, m_Combo_Message);
	DDX_Control(pDX, IDC_COMBO_ACTION, m_Combo_Action);
	DDX_CBString(pDX, IDC_COMBO_ACTION, m_strAction);
	DDX_CBString(pDX, IDC_COMBO_MESSAGE, m_strMessage);
	DDX_Control(pDX, IDC_TREE_MESSAGE, m_Tree_Message);
}


BEGIN_MESSAGE_MAP(CMessageEditDlg, CDialog)
	ON_BN_CLICKED(IDADD, &CMessageEditDlg::OnBnClickedAdd)
	ON_BN_CLICKED(IDDELETE, &CMessageEditDlg::OnBnClickedDelete)
END_MESSAGE_MAP()


// CMessageEditDlg 메시지 처리기입니다.


void CMessageEditDlg::OnBnClickedAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	
	HTREEITEM hItem = AllSearchItem(m_strMessage, NULL, FALSE);
	HTREEITEM hRoot;
	if(hItem == NULL)
		hRoot = m_Tree_Message.InsertItem(m_strMessage, 0, 0, TVI_ROOT);
	else
		hRoot = hItem;

	HTREEITEM hSrc = m_Tree_Message.InsertItem(m_strAction, 0, 0, hRoot);
	m_Tree_Message.EnsureVisible(hSrc);

	//입력 문자열 초기화
	m_Combo_Message.SetEditSel(0,-1);
	m_Combo_Message.Clear();

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* pObj = pDoc->GetPickUpObject();
	if(pObj != NULL)
	{
		for(std::list<MessageACt*>::iterator iter = pObj->m_MessageAction.begin(); iter != pObj->m_MessageAction.end(); iter++)
		{
			if(m_strMessage.Compare((*iter)->MessageState) == 0)
			{
				(*iter)->ActionType.push_back(m_strAction);
				break;
			}
		}

		MessageACt* msg = new MessageACt(m_strMessage, m_strAction);
		pDoc->GetPickUpObject()->m_MessageAction.push_back(msg);
	}
}

void CMessageEditDlg::OnBnClickedDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hItem = m_Tree_Message.GetSelectedItem();
	CString str = m_Tree_Message.GetItemText(hItem);

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* pObj = pDoc->GetPickUpObject();

	if(pObj != NULL)
	{
		for(std::list<MessageACt*>::iterator iter = pObj->m_MessageAction.begin(); iter != pObj->m_MessageAction.end(); )
		{
			if(str.Compare((*iter)->MessageState) == 0)
			{
				pObj->m_MessageAction.erase(iter++);
			}
			else
			{
				if((*iter)->SearchItemDelete(str))
					break;
				else
					iter++;
			}
		}
	}

	if(hItem != NULL)
		m_Tree_Message.DeleteItem(hItem);
}

BOOL CMessageEditDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	
	m_Combo_Message.AddString(_T("Set Focus"));
	m_Combo_Message.AddString(_T("Set D"));
	m_Combo_Message.AddString(_T("Set Focus"));
	m_Combo_Message.AddString(_T("Set Focus"));

	m_Combo_Action.AddString(_T("UI"));
	m_Combo_Action.AddString(_T("Static"));
	m_Combo_Action.AddString(_T("Edit"));
	m_Combo_Action.AddString(_T("Button"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

HTREEITEM CMessageEditDlg::AllSearchItem(const CString& str, HTREEITEM hItem, BOOL bSearchDown)
{
		// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_Tree_Message.GetRootItem();

	CString strItem = m_Tree_Message.GetItemText( hItem );

	if( strItem.Find( str ) == 0 )
		return hItem;

	// 자식 노드 검색
	if( bSearchDown )
		hChild = m_Tree_Message.GetChildItem( hItem );
	else
	{
		HTREEITEM hLastSibling = m_Tree_Message.GetChildItem( hItem );
		while( (hLastSibling = m_Tree_Message.GetNextSiblingItem( hLastSibling )) != NULL )
		{
			hChild = hLastSibling;
		}
	}

	if( hChild )
	{
		hFind = AllSearchItem( str, hChild, bSearchDown);
	}

	// 형제 노드 검색
	if( bSearchDown )
		hSibling = m_Tree_Message.GetNextSiblingItem( hItem );
	else
		hSibling = m_Tree_Message.GetPrevSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = AllSearchItem( str, hSibling, bSearchDown);
	}

	return hFind;
}
