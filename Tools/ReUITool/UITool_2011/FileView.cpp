
#include "stdafx.h"
#include "mainfrm.h"
#include "FileView.h"
#include "Resource.h"
#include "UITool_2011.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

inline void MakeLower( CString& str ) 
{ 
	for( int i = 0; i < str.GetLength(); i++ ) 
	{ 
		char c = str[i];

		if( c >= 'A' && c <= 'Z' ) 
		{ 
			str.SetAt( i, c + 'a' - 'A' ); 
		} 
	} 
} 


/////////////////////////////////////////////////////////////////////////////
// CFileView

CFileView::CFileView()
{
}

CFileView::~CFileView()
{
}

BEGIN_MESSAGE_MAP(CFileView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 메시지 처리기

int CFileView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 뷰를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_EDITLABELS ;

	if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("파일 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	if(!m_wndSearchDlg.Create(IDD_SEARCHDLG, this))
	{
		TRACE0("검색 뷰를 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndSearchDlg.ShowWindow(SW_SHOW);

	// 뷰 이미지를 로드합니다.
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.ShowWindow(SW_HIDE);
	//m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);
	OnChangeVisualStyle();

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	//m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	// 정적 트리 뷰 데이터를 더미 코드로 채웁니다.
	//FillFileView();
	AdjustLayout();

	return 0;
}

void CFileView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	AdjustLayout();
}

void CFileView::FillFileView()
{
	HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp 파일"), 0, 0);
	m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

	HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp 소스 파일"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
	m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

	HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp 헤더 파일"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
	m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

	HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp 리소스 파일"), 0, 0, hRoot);

	m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
	m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

	m_wndFileView.Expand(hRoot, TVE_EXPAND);
	m_wndFileView.Expand(hSrc, TVE_EXPAND);
	m_wndFileView.Expand(hInc, TVE_EXPAND);
}

void CFileView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndFileView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 클릭한 항목을 선택합니다.
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CFileView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy +10;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndSearchDlg.SetWindowPos(NULL, rectClient.left + 2, rectClient.top, rectClient.Width() - 4, cyTlb, SWP_NOZORDER);
	m_wndFileView.SetWindowPos(NULL, rectClient.left + 2, rectClient.top + cyTlb + 2, rectClient.Width() - 4, rectClient.Height() - cyTlb - 4, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CFileView::OnProperties()
{
	AfxMessageBox(_T("속성...."));

}

void CFileView::OnFileOpen()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	AfxMessageBox(_T("열기...."));
}

void CFileView::OnFileOpenWith()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnDummyCompile()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditCut()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditCopy()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnEditClear()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
}

void CFileView::OnPaint()
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

	CRect rectTree;
	m_wndFileView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CFileView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndFileView.SetFocus();
}

void CFileView::OnChangeVisualStyle()
{
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 잠금 */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("비트맵을 로드할 수 없습니다. %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
}


BOOL CFileView::AddFile(CUIToolBaseObject* pObj)
{
	if(pObj->m_parent == NULL)
	{
		HTREEITEM hRoot = m_wndFileView.InsertItem(pObj->m_ID, 0, 0);
		m_wndFileView.Expand(hRoot, TVE_EXPAND);
		m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	}
	else
	{
		HTREEITEM hItem = GetTreeItemFromItemText(pObj->m_parent->m_ID, NULL, TRUE);
		if(hItem != NULL)
		{
			m_wndFileView.InsertItem(pObj->m_ID, 0, 0, hItem);
			m_wndFileView.Expand(hItem, TVE_EXPAND);
		}
		else
		{
			AfxMessageBox(pObj->m_parent->m_ID + _T("가 존재 하지 않습니다."));
			return FALSE;
		}
	}	

	return TRUE;
}

HTREEITEM CFileView::GetTreeItemFromItemText(CString& str, HTREEITEM hItem, BOOL bSearchDown, BOOL bOneCharSame)
{
	// 모든 Tree노드에서 찾는다.
	HTREEITEM hFind = NULL, hChild = NULL, hSibling = NULL;

	if( hItem == NULL )
		hItem = m_wndFileView.GetRootItem();

	CString strItem = m_wndFileView.GetItemText( hItem );
	if(bOneCharSame)
	{
		MakeLower(str);
		MakeLower(strItem);
		
		if(strItem.Find( str ) != -1)
			return hItem;
	}
	else
	{
		if( strItem.Find( str ) == 0 )
			return hItem;
	}
	
	// 자식 노드 검색
	if( bSearchDown )
		hChild = m_wndFileView.GetChildItem( hItem );
	else
	{
		HTREEITEM hLastSibling = m_wndFileView.GetChildItem( hItem );
		while( (hLastSibling = m_wndFileView.GetNextSiblingItem( hLastSibling )) != NULL )
		{
			hChild = hLastSibling;
		}
	}

	if( hChild )
	{
		hFind = GetTreeItemFromItemText( str, hChild, bSearchDown);
	}

	// 형제 노드 검색
	if( bSearchDown )
		hSibling = m_wndFileView.GetNextSiblingItem( hItem );
	else
		hSibling = m_wndFileView.GetPrevSiblingItem( hItem );
	if( hFind == NULL && hSibling )
	{
		hFind = GetTreeItemFromItemText( str, hSibling, bSearchDown);
	}

	return hFind;
}

void CFileView::SetSelectItemDelete()
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();

	if(hItem != NULL)
		m_wndFileView.DeleteItem(hItem);
}

void CFileView::SetSelectItemChange(CString pStr)
{
	TVITEM tvItem;
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	tvItem.hItem = hItem;
	tvItem.pszText = (LPWSTR)(LPCWSTR)pStr;
	m_wndFileView.SetItem(&tvItem);
}
void CFileView::SetSelectObj_Item(CUIToolBaseObject* pAddObj)
{
	HTREEITEM hItem = GetTreeItemFromItemText(pAddObj->m_ID, NULL, TRUE);
	
	if(hItem != NULL)
		m_wndFileView.SelectItem(hItem);
}
HTREEITEM CFileView::NextItem(HTREEITEM hItem)
{ 
	HTREEITEM hFind = NULL;
	if(hItem == NULL)
		hItem = m_wndFileView.GetRootItem();

	hFind = m_wndFileView.GetChildItem(hItem);

	if(hFind == NULL)
	{
		hFind = m_wndFileView.GetNextSiblingItem(hItem);
	}

	if (hFind == NULL)
	{
		hFind = m_wndFileView.GetNextVisibleItem(hItem);
	}

	return hFind;
}

void CFileView::SetSelectItemText(CString NewID)
{
	HTREEITEM hItem = m_wndFileView.GetSelectedItem();
	if(hItem != NULL)
	{
		m_wndFileView.SetItemText(hItem, NewID);
	}
}