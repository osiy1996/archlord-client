
#pragma once

#include "ViewTree.h"
#include "Utility.h"
#include "UIToolBaseObject.h"
#include "FileSearch.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
	
};

class CFileView : public CDockablePane
{
// 생성입니다.
public:
	CFileView();

	void AdjustLayout();
	void OnChangeVisualStyle();
	CViewTree m_wndFileView;
// 특성입니다.
protected:
	
	CImageList m_FileViewImages;
	CFileViewToolBar m_wndToolBar;
	CFileSearch m_wndSearchDlg;

protected:
	void FillFileView();

// 구현입니다.
public:
	virtual ~CFileView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	DECLARE_MESSAGE_MAP()

public:
	BOOL AddFile(CUIToolBaseObject* pAddObj);
	void SetSelectItemDelete();
	void SetSelectItemChange(CString pStr);
	void SetSelectObj_Item(CUIToolBaseObject* pAddObj);
	HTREEITEM NextItem(HTREEITEM hItem);

	HTREEITEM GetTreeItemFromItemText(CString& str, HTREEITEM hItem, BOOL bSearchDown, BOOL bOneCharSame = FALSE);
	void SetSelectItemText(CString NewID);
};

