
#include "stdafx.h"
#include "ViewTree.h"
#include "UITool_2011Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
:m_pTreeDragImage(NULL), m_hDragItem(NULL)
{
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
	ON_NOTIFY_REFLECT(TVN_BEGINDRAG, &CViewTree::OnTvnBegindrag)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree 메시지 처리기

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}

void CViewTree::OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	HTREEITEM hDragItem = pNMTreeView->itemNew.hItem;

	if(hDragItem == GetRootItem()) return;
	// 루트 아이템은 드랙 못하도록,,,
	// 드랙 안할 아템들은 여기서 필터링을 하심 되겠네엽


	//드래그 이미지 생성
	if(m_pTreeDragImage)
		m_pTreeDragImage->DeleteImageList();
	m_pTreeDragImage = CreateDragImage(pNMTreeView->itemNew.hItem);

	// 드래그시 사용할 이미지 크기 계산
	RECT  rc;
	GetItemRect(pNMTreeView->itemNew.hItem, &rc, TRUE); // 아이콘을 포함하는 크기

	// 드래그를 시작
	m_pTreeDragImage->BeginDrag(0, CPoint(pNMTreeView->ptDrag.x-rc.left+16, 

		pNMTreeView->ptDrag.y-rc.top));
	// 드래그 이미지 표시
	m_pTreeDragImage->DragEnter(this, pNMTreeView->ptDrag);

	// 마우스 메시지를 잡아두고
	SetCapture();

	// 현재 선택된 아이템 핸들을 기억
	m_hDragItem = pNMTreeView->itemNew.hItem;

	*pResult = 0;
}

void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 드래그 중이라면
	if(m_pTreeDragImage)
	{
		// 트리컨트롤 기준으로 마우스 좌표 계산
		CPoint  p = point;
		ClientToScreen(&p);
		::ScreenToClient(m_hWnd, &p);

		// 마우스가 위치한 아이템을 검사한다.항목이 트리 뷰 항목위에 있는지 확인하고 그렇다면 항목이 밝게 표시되도록한다.
		HTREEITEM hItem = HitTest(p);

		// 밝게 표시된 부분과 현재 선택된 아이템이 틀리다면
		if(hItem != GetDropHilightItem())
		{
			// 드래그 이미지 그리기 중지
			m_pTreeDragImage->DragLeave(this);

			// 새로운 항목을 밝게 표시한다.
			SelectDropTarget(hItem);

			// 드래그 이미지를 다시 보여준다.
			m_pTreeDragImage->DragEnter(this, p);
		}
		else
		{
			m_pTreeDragImage->DragMove(p);
		}
	}

	CTreeCtrl::OnMouseMove(nFlags, point);
}

void CViewTree::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	// 드래그 중이 었다면

	if(m_pTreeDragImage)
	{
		// 마우스 메시지 캡쳐 기능을 제거한다.
		ReleaseCapture();

		// 드래그 과정을 중단한다.
		m_pTreeDragImage->DragLeave(this);
		m_pTreeDragImage->EndDrag();
		m_pTreeDragImage->DeleteImageList();
		m_pTreeDragImage = NULL;

		// 일단 마지막으로 밝게 표시되었던 항목을 찾는다.
		HTREEITEM hTargetItem = GetDropHilightItem();

		// 밝게 표시된 드롭 항목의 선택을 취소한다.
		SelectDropTarget(NULL);

		// 선택된 항목(아이템)이 있다면
		if(hTargetItem)
		{
			// 선택된 아이템과 이동될 곳의 아이템이 같다면 이동할 필요가 없다.
			if(m_hDragItem != hTargetItem)
			{
				// 현재 자식의 부모 아이템 핸들을 구한다.
				HTREEITEM hParentItem = GetNextItem(m_hDragItem, TVGN_PARENT);

				// 이동하려는 곳이 자신이 직접속한 항목 이라면 이동할 필요가 없다.
				if(hParentItem != hTargetItem && m_hDragItem != GetNextItem(hTargetItem, TVGN_PARENT))
				{
					// 트리의 내용을 이동하자.
					MoveTreeItem(this, m_hDragItem, hTargetItem, FALSE);
					
					// 이동된 곳의 트리를 확장하자.
					Expand(hTargetItem, TVE_EXPAND);

					// 이미지도 확장한걸로 바꾸자
					//SetItemImage(hTargetItem, 1, 1);

					// 원본 트리의 모든 아이템이 사라졌다면 이미지 그림을 기본으로 바꾸자.
					HTREEITEM hItem = GetChildItem(hParentItem);
					if(!hItem)
					{
						SetItemImage(hParentItem, 0, 0);
					}
				}
			}
		}

		m_hDragItem = NULL;
	}


	CTreeCtrl::OnLButtonUp(nFlags, point);
	SelectItemUpdate();
}

BOOL CViewTree::MoveTreeItem(CTreeCtrl *pTree, HTREEITEM hSrcItem, HTREEITEM hDestItem, BOOL bAutoMoveChild)
{
	CString DestText = pTree->GetItemText(hDestItem);

	// 이동할 아이템의 정보를 알아내자.
	TVITEM    TV;
	char    str[256];
	ZeroMemory(str, sizeof(str));
	TV.hItem = hSrcItem;
	TV.mask  = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText = (LPWSTR)str;
	TV.cchTextMax = sizeof(str);
	GetItem(&TV);
	DWORD dwData = pTree->GetItemData(hSrcItem);


	// 아이템을 추가 하자.
	TVINSERTSTRUCT  TI;
	TI.hParent        = hDestItem;
	TI.hInsertAfter   = TVI_LAST;
	TI.item.mask     = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.iImage   = TV.iImage;
	TI.item.iSelectedImage = TV.iSelectedImage;
	TI.item.pszText   = TV.pszText;
	HTREEITEM hItem  = pTree->InsertItem(&TI);
	pTree->SetItemData(hItem, dwData);


	// 확장 여부를 알아서 똑같이 하자.
	TVITEM  item;
	item.mask = TVIF_HANDLE;
	item.hItem = hSrcItem;
	pTree->GetItem(&item);
	// 현재 아이템에 자식 아이템이 있다면
	HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);

	if(bAutoMoveChild)
	{
		if(hChildItem)
			MoveChildTreeItem(pTree, hChildItem, hItem);
	}
	else
	{
		if(item.state & TVIS_EXPANDED)
		{
			while(hChildItem  != NULL && hChildItem != hDestItem)
			{
				//부모랑 자식을 연결함.
				HTREEITEM hParentItem = GetNextItem(hSrcItem, TVGN_PARENT);
				MoveTreeItem(pTree, hChildItem, hParentItem, TRUE);
				hChildItem = GetNextItem(hSrcItem, TVGN_CHILD);
			}
		}
		else
		{
			// 자식 아이템이 있다면 같이 이동하자.
			if(hChildItem)
				MoveChildTreeItem(pTree, hChildItem, hItem);
		}
	}

	
	if(item.state & TVIS_EXPANDED)
	{
		pTree->Expand(hItem, TVE_EXPAND);
	}

	CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
	CUIToolBaseObject* pItemObj = pDoc->GetItemInTheList(TI.item.pszText);
	pItemObj->SetParentID(DestText);

	// 기존 아이템을 제거한다.
	pTree->DeleteItem(hSrcItem);
	// 아이템을 선택하자.
	pTree->SelectItem(hItem);
	return TRUE;
}

// 현재 트리의 모든 아이템 데이터 이동
BOOL CViewTree::MoveChildTreeItem(CTreeCtrl *pTree, HTREEITEM hChildItem, HTREEITEM hDestItem)
{
	HTREEITEM hSrcItem = hChildItem;

	while(hSrcItem)
	{
		// 이동할 아이템의 정보를 알아내자.
		TVITEM    TV;
		char    str[256];
		ZeroMemory(str, sizeof(str));
		TV.hItem     = hSrcItem;
		TV.mask     = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TV.pszText = (LPWSTR)str;
		TV.cchTextMax = sizeof(str);
		GetItem(&TV);
		DWORD dwData = pTree->GetItemData(hSrcItem);

		// 아이템을 추가 하자.
		TVINSERTSTRUCT  TI;
		TI.hParent       = hDestItem;
		TI.hInsertAfter  = TVI_LAST;
		TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.iImage   = TV.iImage;
		TI.item.iSelectedImage = TV.iSelectedImage;
		TI.item.pszText   = TV.pszText;
		HTREEITEM hItem  = pTree->InsertItem(&TI);
		pTree->SetItemData(hItem, dwData);

		// 현재 아이템에 자식 아이템이 있다면
		HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);

		 pTree->GetNextItem(hSrcItem, TVGN_CHILD);
		if(hChildItem)
		{
			MoveChildTreeItem(pTree, hChildItem, hItem);
		}

		// 확장 여부를 알아서 똑같이 하자.
		TVITEM  item;
		item.mask = TVIF_HANDLE;
		item.hItem = hSrcItem;
		pTree->GetItem(&item);
		if(item.state & TVIS_EXPANDED)
		{
			pTree->Expand(hItem, TVE_EXPAND);
		}

		// 다음 아이템을 알아보자.
		hSrcItem = pTree->GetNextItem(hSrcItem, TVGN_NEXT);
	}

	// 기존 아이템을 제거한다.
	pTree->DeleteItem(hChildItem);

	return TRUE;
}

void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CTreeCtrl::OnLButtonDown(nFlags, point);
	
	SelectItemUpdate();	
}

void CViewTree::SelectItemUpdate()
{
	HTREEITEM hSelectedItem = GetSelectedItem();
	if(hSelectedItem)
	{
		CString strData = GetItemText(hSelectedItem);
		CUITool_2011Doc* pDoc = (CUITool_2011Doc*)((CMainFrame*)AfxGetMainWnd())->GetActiveDocument();
		
		CUIToolBaseObject* tmp = pDoc->GetItemInTheList(strData);
		if(tmp != NULL)
			pDoc->CurrentPickUpObjUpdate(tmp, FALSE);
	}
}
