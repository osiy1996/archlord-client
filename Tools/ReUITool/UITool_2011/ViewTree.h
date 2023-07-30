
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree â�Դϴ�.

class CViewTree : public CTreeCtrl
{
// �����Դϴ�.
public:
	CViewTree();

// �������Դϴ�.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// �����Դϴ�.
public:
	virtual ~CViewTree();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnTvnBegindrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

public:
	CImageList* m_pTreeDragImage;
	HTREEITEM m_hDragItem;
	
	CString m_strParentItem;

	BOOL MoveTreeItem(CTreeCtrl *pTree, HTREEITEM hSrcItem, HTREEITEM hDestItem, BOOL bAutoMoveChild = TRUE);
	BOOL MoveChildTreeItem(CTreeCtrl *pTree, HTREEITEM hChildItem, HTREEITEM hDestItem);
	void SelectItemUpdate();
	
};
