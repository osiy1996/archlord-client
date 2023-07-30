// XTPDockingPaneMiniWnd.h : interface for the CXTPDockingPaneMiniWnd class.
//
// This file is a part of the XTREME DOCKINGPANE MFC class library.
// (c)1998-2007 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPDOCKINGPANEMINIWND_H__)
#define __XTPDOCKINGPANEMINIWND_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneBase.h"

//===========================================================================
// Summary:
//     CXTPDockingPaneMiniWnd is a multiple inheritance class derived from
//     CMiniFrameWnd and CXTPDockingPaneBase. It is used internally as a
//     MiniWnd container for CXTPDockingPaneBase derived classes.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPaneMiniWnd : public CMiniFrameWnd, public CXTPDockingPaneBase
{
protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPaneMiniWnd object.
	// Parameters:
	//     pLayout - Parent layout pointer.
	//-----------------------------------------------------------------------
	CXTPDockingPaneMiniWnd(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPaneMiniWnd object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	~CXTPDockingPaneMiniWnd();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if the frame is active
	// Returns:
	//     TRUE if the frame is active
	//-----------------------------------------------------------------------
	BOOL IsActive() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to expand collapsed frame.
	//-----------------------------------------------------------------------
	void Expand();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves top level container of floating frame
	// Returns:
	//     Pointer to top container of frame
	//-----------------------------------------------------------------------
	CXTPDockingPaneSplitterContainer* GetTopPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if frame is collapsed
	//-----------------------------------------------------------------------
	BOOL IsCollapsed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to collapse frame.
	//-----------------------------------------------------------------------
	void Collapse();


protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Copies self data from another pane
	// Parameters:
	//     pClone  - Pane which members need to copy.
	//     pMap - Internal parameter needs to map pane to copied pane.
	// See Also:
	//     CXTPPaneToPaneMap
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPDockingPaneMiniWnd* pClone, CXTPPaneToPaneMap* pMap);

	//-----------------------------------------------------------------------
	// Summary:
	//     Initialize the frame window
	// Parameters:
	//     pPane - Child pane will be placed to frame
	//     rc    - Rectangle of floating frame.
	//-----------------------------------------------------------------------
	void Init(CXTPDockingPaneBase* pPane, CRect rc);
protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove a pane from collection.
	// Parameters:
	//     pPane - Points to a CXTPDockingPaneBase need to remove
	//-----------------------------------------------------------------------
	virtual void RemovePane(CXTPDockingPaneBase* pPane);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when a child pane collection is changed.
	// Parameters:
	//     pContainer - Points to a CXTPDockingPaneBase object that is changed.
	//-----------------------------------------------------------------------
	virtual void OnChildContainerChanged(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to clone the pane.
	// Parameters:
	//     pLayout - Points to a CXTPDockingPaneLayout object.
	//     pMap - Points to a CXTPPaneToPaneMap object.
	//     dwIgnoredOptions - Options that must be skipped.
	// Returns:
	//     A pointer to a CXTPDockingPaneBase object.
	// See Also:
	//     CXTPPaneToPaneMap
	//-----------------------------------------------------------------------
	virtual CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to save/restore the settings of the pane.
	// Parameters:
	//     pPX - Points to a CXTPPropExchange object.
	// Returns:
	//     TRUE if successful; otherwise returns FALSE.
	//-----------------------------------------------------------------------
	virtual BOOL DoPropExchange(CXTPPropExchange* pPX);


	//-----------------------------------------------------------------------
	// Summary:
	//     Creates frame window
	// Parameters:
	//     rc - Specifies the size and position of the window
	// Returns:
	//     Nonzero if initialization is successful; otherwise 0.
	//-----------------------------------------------------------------------
	virtual BOOL Create(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called by the framework when a caption button
	//     is clicked.
	// Parameters:
	//     pButton - Caption button that was clicked.
	//-----------------------------------------------------------------------
	virtual void OnCaptionButtonClick(CXTPDockingPaneCaptionButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called by the framework when the mouse button is pressed
	//      on a caption button.
	// Parameters:
	//     pButton - Caption button that is being clicked.
	//-----------------------------------------------------------------------
	virtual BOOL OnCaptionButtonDown(CXTPDockingPaneCaptionButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine the currently selected pane
	// Returns:
	//     Pointer to a CXTPDockingPane object that is the currently selected pane.
	//-----------------------------------------------------------------------
	CXTPDockingPane* GetSelectedPane() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if a specific caption button
	//     is visible.
	// Parameters:
	//     pButton - Button to check to see if it is visible.
	// Returns:
	//     TRUE if the caption button specified is visible, FALSE otherwise.
	//-----------------------------------------------------------------------
	virtual BOOL IsCaptionButtonVisible(CXTPDockingPaneCaptionButton* pButton);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if themed floating panes are used.
	// Returns:
	//     TRUE is themed floating panes are used, FALSE otherwise.
	//-----------------------------------------------------------------------
	BOOL IsThemed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the theme is changed.
	//-----------------------------------------------------------------------
	void OnThemedChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to redraw frame
	//-----------------------------------------------------------------------
	void RedrawPane();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves safe window handle.
	//-----------------------------------------------------------------------
	virtual HWND GetPaneHwnd() const;

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to animate collapsed frame
	// Parameters:
	//     bActivate - TRUE to activate frame.
	//-----------------------------------------------------------------------
	void DoSlideStep(BOOL bActivate = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when the "pin" caption button is clicked.
	//-----------------------------------------------------------------------
	void OnPinButtonClick();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when a need to redraw pane.
	// Parameters:
	//     bSelectionChanged - TRUE if selected pane was changed
	//-----------------------------------------------------------------------
	virtual void InvalidatePane(BOOL bSelectionChanged);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to update Opacity of floating frame
	// See Also: CXTPDockingPaneManager::SetFloatingFramesOpacity
	//-----------------------------------------------------------------------
	virtual void UpdateWindowOpacity();

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called when focus was changed inside application
	//-------------------------------------------------------------------------
	void OnFocusChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve a collection of the panes with
	//     specified type.
	// Parameters:
	//     type  - Type of the panes to find.
	//     pList - Points to a CXTPDockingPaneBaseList object that will
	//             contain found panes.
	// See Also:
	//     CXTPDockingPaneBaseList
	//-----------------------------------------------------------------------
	virtual void FindPane(XTPDockingPaneType type, CXTPDockingPaneBaseList* pList) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called if some action occur
	// Parameters:
	//     action - Docking Pane action.
	// Returns: TRUE if action was canceled
	//-----------------------------------------------------------------------
	BOOL OnAction(XTPDockingPaneAction action);

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPDockingPaneMiniWnd)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	virtual void PostNcDestroy();
	BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	void DeletePane();
	void OnFinalRelease();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPDockingPaneMiniWnd)
	afx_msg void OnClose();
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnIdleUpdateCmdUI();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnSizing(UINT nSide, LPRECT lpRect);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE



private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CWnd* pParentWnd = NULL, LPCTSTR lpszMenuName = NULL, DWORD dwExStyle = 0, CCreateContext* pContext = NULL);
	BOOL Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd = NULL, UINT nID = 0);

public:
	static BOOL m_bShowPinButton;                       // TRUE to show pin button of the frame.

protected:
	CXTPDockingPaneSplitterContainer* m_pTopContainer;  // Top level container.
	BOOL m_bCloseEnabled;       // Close button of frame is enabled.

	int m_nExpandedHeight;      // Expanded height of the frame.

	int m_nStepsCount;          // Total steps of animation process
	int m_nSlideStep;           // Current step of animation process
	int m_nDeactivationCount;   // Deactivation counter.

	BOOL m_bCollapsed;          // Pane is collapsed
	BOOL m_bExpanded;           // Pane is collapsed but expanded now
	BOOL m_bSlideOut;           // Pane is currently expanding.

	BOOL m_bDelayInvalidate;    // True to redraw frame after small delay


	DECLARE_DYNAMIC(CXTPDockingPaneMiniWnd)

	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPaneLayout;
};

AFX_INLINE CXTPDockingPaneSplitterContainer* CXTPDockingPaneMiniWnd::GetTopPane() const {
	return m_pTopContainer;
}
AFX_INLINE BOOL CXTPDockingPaneMiniWnd::IsActive() const {
	return m_bActive;
}
AFX_INLINE BOOL CXTPDockingPaneMiniWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE BOOL CXTPDockingPaneMiniWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, LPCTSTR lpszMenuName, DWORD dwExStyle, CCreateContext* pContext) {
	return CFrameWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, lpszMenuName, dwExStyle, pContext);
}
AFX_INLINE BOOL CXTPDockingPaneMiniWnd::Create(LPCTSTR lpClassName, LPCTSTR lpWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) {
	return CMiniFrameWnd::Create(lpClassName, lpWindowName, dwStyle, rect, pParentWnd, nID);
}
AFX_INLINE BOOL CXTPDockingPaneMiniWnd::IsCollapsed() const {
	return m_bCollapsed;
}
AFX_INLINE HWND CXTPDockingPaneMiniWnd::GetPaneHwnd() const {
	return CWnd::GetSafeHwnd();
}


#endif // #if !defined(__XTPDOCKINGPANEMINIWND_H__)
