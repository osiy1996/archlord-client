
#pragma once

/////////////////////////////////////////////////////////////////////////////
// CViewTree 창입니다.

class CViewTree : public CTreeCtrl
{
// 생성입니다.
public:
	CViewTree();

// 재정의입니다.
protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);

// 구현입니다.
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
