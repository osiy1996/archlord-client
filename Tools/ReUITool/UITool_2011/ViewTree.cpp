
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
// CViewTree �޽��� ó����

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	HTREEITEM hDragItem = pNMTreeView->itemNew.hItem;

	if(hDragItem == GetRootItem()) return;
	// ��Ʈ �������� �巢 ���ϵ���,,,
	// �巢 ���� ���۵��� ���⼭ ���͸��� �Ͻ� �ǰڳ׿�


	//�巡�� �̹��� ����
	if(m_pTreeDragImage)
		m_pTreeDragImage->DeleteImageList();
	m_pTreeDragImage = CreateDragImage(pNMTreeView->itemNew.hItem);

	// �巡�׽� ����� �̹��� ũ�� ���
	RECT  rc;
	GetItemRect(pNMTreeView->itemNew.hItem, &rc, TRUE); // �������� �����ϴ� ũ��

	// �巡�׸� ����
	m_pTreeDragImage->BeginDrag(0, CPoint(pNMTreeView->ptDrag.x-rc.left+16, 

		pNMTreeView->ptDrag.y-rc.top));
	// �巡�� �̹��� ǥ��
	m_pTreeDragImage->DragEnter(this, pNMTreeView->ptDrag);

	// ���콺 �޽����� ��Ƶΰ�
	SetCapture();

	// ���� ���õ� ������ �ڵ��� ���
	m_hDragItem = pNMTreeView->itemNew.hItem;

	*pResult = 0;
}

void CViewTree::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// �巡�� ���̶��
	if(m_pTreeDragImage)
	{
		// Ʈ����Ʈ�� �������� ���콺 ��ǥ ���
		CPoint  p = point;
		ClientToScreen(&p);
		::ScreenToClient(m_hWnd, &p);

		// ���콺�� ��ġ�� �������� �˻��Ѵ�.�׸��� Ʈ�� �� �׸����� �ִ��� Ȯ���ϰ� �׷��ٸ� �׸��� ��� ǥ�õǵ����Ѵ�.
		HTREEITEM hItem = HitTest(p);

		// ��� ǥ�õ� �κа� ���� ���õ� �������� Ʋ���ٸ�
		if(hItem != GetDropHilightItem())
		{
			// �巡�� �̹��� �׸��� ����
			m_pTreeDragImage->DragLeave(this);

			// ���ο� �׸��� ��� ǥ���Ѵ�.
			SelectDropTarget(hItem);

			// �巡�� �̹����� �ٽ� �����ش�.
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
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	// �巡�� ���� ���ٸ�

	if(m_pTreeDragImage)
	{
		// ���콺 �޽��� ĸ�� ����� �����Ѵ�.
		ReleaseCapture();

		// �巡�� ������ �ߴ��Ѵ�.
		m_pTreeDragImage->DragLeave(this);
		m_pTreeDragImage->EndDrag();
		m_pTreeDragImage->DeleteImageList();
		m_pTreeDragImage = NULL;

		// �ϴ� ���������� ��� ǥ�õǾ��� �׸��� ã�´�.
		HTREEITEM hTargetItem = GetDropHilightItem();

		// ��� ǥ�õ� ��� �׸��� ������ ����Ѵ�.
		SelectDropTarget(NULL);

		// ���õ� �׸�(������)�� �ִٸ�
		if(hTargetItem)
		{
			// ���õ� �����۰� �̵��� ���� �������� ���ٸ� �̵��� �ʿ䰡 ����.
			if(m_hDragItem != hTargetItem)
			{
				// ���� �ڽ��� �θ� ������ �ڵ��� ���Ѵ�.
				HTREEITEM hParentItem = GetNextItem(m_hDragItem, TVGN_PARENT);

				// �̵��Ϸ��� ���� �ڽ��� �������� �׸� �̶�� �̵��� �ʿ䰡 ����.
				if(hParentItem != hTargetItem && m_hDragItem != GetNextItem(hTargetItem, TVGN_PARENT))
				{
					// Ʈ���� ������ �̵�����.
					MoveTreeItem(this, m_hDragItem, hTargetItem, FALSE);
					
					// �̵��� ���� Ʈ���� Ȯ������.
					Expand(hTargetItem, TVE_EXPAND);

					// �̹����� Ȯ���Ѱɷ� �ٲ���
					//SetItemImage(hTargetItem, 1, 1);

					// ���� Ʈ���� ��� �������� ������ٸ� �̹��� �׸��� �⺻���� �ٲ���.
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

	// �̵��� �������� ������ �˾Ƴ���.
	TVITEM    TV;
	char    str[256];
	ZeroMemory(str, sizeof(str));
	TV.hItem = hSrcItem;
	TV.mask  = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TV.pszText = (LPWSTR)str;
	TV.cchTextMax = sizeof(str);
	GetItem(&TV);
	DWORD dwData = pTree->GetItemData(hSrcItem);


	// �������� �߰� ����.
	TVINSERTSTRUCT  TI;
	TI.hParent        = hDestItem;
	TI.hInsertAfter   = TVI_LAST;
	TI.item.mask     = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	TI.item.iImage   = TV.iImage;
	TI.item.iSelectedImage = TV.iSelectedImage;
	TI.item.pszText   = TV.pszText;
	HTREEITEM hItem  = pTree->InsertItem(&TI);
	pTree->SetItemData(hItem, dwData);


	// Ȯ�� ���θ� �˾Ƽ� �Ȱ��� ����.
	TVITEM  item;
	item.mask = TVIF_HANDLE;
	item.hItem = hSrcItem;
	pTree->GetItem(&item);
	// ���� �����ۿ� �ڽ� �������� �ִٸ�
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
				//�θ�� �ڽ��� ������.
				HTREEITEM hParentItem = GetNextItem(hSrcItem, TVGN_PARENT);
				MoveTreeItem(pTree, hChildItem, hParentItem, TRUE);
				hChildItem = GetNextItem(hSrcItem, TVGN_CHILD);
			}
		}
		else
		{
			// �ڽ� �������� �ִٸ� ���� �̵�����.
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

	// ���� �������� �����Ѵ�.
	pTree->DeleteItem(hSrcItem);
	// �������� ��������.
	pTree->SelectItem(hItem);
	return TRUE;
}

// ���� Ʈ���� ��� ������ ������ �̵�
BOOL CViewTree::MoveChildTreeItem(CTreeCtrl *pTree, HTREEITEM hChildItem, HTREEITEM hDestItem)
{
	HTREEITEM hSrcItem = hChildItem;

	while(hSrcItem)
	{
		// �̵��� �������� ������ �˾Ƴ���.
		TVITEM    TV;
		char    str[256];
		ZeroMemory(str, sizeof(str));
		TV.hItem     = hSrcItem;
		TV.mask     = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TV.pszText = (LPWSTR)str;
		TV.cchTextMax = sizeof(str);
		GetItem(&TV);
		DWORD dwData = pTree->GetItemData(hSrcItem);

		// �������� �߰� ����.
		TVINSERTSTRUCT  TI;
		TI.hParent       = hDestItem;
		TI.hInsertAfter  = TVI_LAST;
		TI.item.mask    = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
		TI.item.iImage   = TV.iImage;
		TI.item.iSelectedImage = TV.iSelectedImage;
		TI.item.pszText   = TV.pszText;
		HTREEITEM hItem  = pTree->InsertItem(&TI);
		pTree->SetItemData(hItem, dwData);

		// ���� �����ۿ� �ڽ� �������� �ִٸ�
		HTREEITEM hChildItem = pTree->GetChildItem(hSrcItem);

		 pTree->GetNextItem(hSrcItem, TVGN_CHILD);
		if(hChildItem)
		{
			MoveChildTreeItem(pTree, hChildItem, hItem);
		}

		// Ȯ�� ���θ� �˾Ƽ� �Ȱ��� ����.
		TVITEM  item;
		item.mask = TVIF_HANDLE;
		item.hItem = hSrcItem;
		pTree->GetItem(&item);
		if(item.state & TVIS_EXPANDED)
		{
			pTree->Expand(hItem, TVE_EXPAND);
		}

		// ���� �������� �˾ƺ���.
		hSrcItem = pTree->GetNextItem(hSrcItem, TVGN_NEXT);
	}

	// ���� �������� �����Ѵ�.
	pTree->DeleteItem(hChildItem);

	return TRUE;
}

void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
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
