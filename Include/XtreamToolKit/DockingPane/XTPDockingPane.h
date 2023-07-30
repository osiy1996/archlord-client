// XTPDockingPane.h : interface for the CXTPDockingPane class.
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
#if !defined(__XTPDOCKINGPANE_H__)
#define __XTPDOCKINGPANE_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "XTPDockingPaneBase.h"
#include "Common/XTPSystemHelpers.h"

class CXTPImageManagerIcon;

//===========================================================================
// Summary:
//     CXTPDockingPane is a CXTPDockingPaneBase derived class.
//     It is used as a container for a user window.
//===========================================================================
class _XTP_EXT_CLASS CXTPDockingPane : public CXTPCmdTarget, public CXTPDockingPaneBase, public CXTPAccessible
{

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPDockingPane object
	//     Protected constructor. Use CXTPDockingPaneManager::CreatePane() to create it.
	// Parameters:
	//     pLayout - Points to a CXTPDockingPaneLayout object
	//-----------------------------------------------------------------------
	CXTPDockingPane(CXTPDockingPaneLayout* pLayout);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPDockingPane object, handles cleanup and deallocation.
	//-----------------------------------------------------------------------
	virtual ~CXTPDockingPane();

public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set a title for this object.
	// Parameters:
	//     lpszTitle - Title to be set.
	//-----------------------------------------------------------------------
	void SetTitle(LPCTSTR lpszTitle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the focus of the pane.
	//-----------------------------------------------------------------------
	virtual void SetFocus();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get pane's identifier.
	// Returns:
	//     The identifier of the pane.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the title of the pane.
	// Returns:
	//     The title of the pane.
	//-----------------------------------------------------------------------
	virtual CString GetTitle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the short title of the pane.
	// Returns:
	//     The short title of the pane.
	//-----------------------------------------------------------------------
	virtual CString GetShortTitle() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to close the pane.
	//-----------------------------------------------------------------------
	void Close();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to hide the pane.
	//-----------------------------------------------------------------------
	void Hide();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is associated with a user window.
	// Returns:
	//     TRUE if the pane is associated with a user window.
	//-----------------------------------------------------------------------
	BOOL IsValid() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is closed.
	// Returns:
	//     TRUE if the pane is closed.
	//-----------------------------------------------------------------------
	BOOL IsClosed() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is hidden.
	// Returns:
	//     TRUE if the pane is hidden.
	//-----------------------------------------------------------------------
	BOOL IsHidden() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is selected.
	// Returns:
	//     TRUE if the pane is selected.
	//-----------------------------------------------------------------------
	BOOL IsSelected() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to check if the pane is floating.
	// Returns:
	//     TRUE if the pane is floating.
	//-----------------------------------------------------------------------
	BOOL IsFloating() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to associate the pane with a user window.
	// Parameters:
	//     pWnd - Pointer to a user window.
	// Remarks:
	//     The best place to create a user window and associate it with a
	//     pane is after the pane becomes visible. To catch this event use
	//     the XTPWM_DOCKINGPANE_NOTIFY Handler.
	//-----------------------------------------------------------------------
	void Attach(CWnd* pWnd);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function dynamically creates a view based on a CRuntimeClass
	//     object and attaches it to the pane.
	// Parameters:
	//     pParentWnd - Points to the parent of the view to be created. The parent must be
	//                  valid.
	//     pViewClass - CView runtime class.
	//     pDocument  - CDocument associated with the view. It can be NULL.
	//     pContext   - Create context for the view. It can be NULL.
	// Remarks:
	//     The best place to create a user window and associate it with a
	//     pane is after the pane becomes visible. To catch this event, use
	//     the XTPWM_DOCKINGPANE_NOTIFY Handler.
	// Returns:
	//     A CWnd* pointer to the newly created view if successful,
	//     otherwise returns NULL.
	//-----------------------------------------------------------------------
	CWnd* AttachView(CWnd* pParentWnd, CRuntimeClass *pViewClass, CDocument *pDocument = NULL, CCreateContext* pContext = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to disassociate the pane from a user window.
	//-----------------------------------------------------------------------
	void Detach();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to select the pane.
	//-----------------------------------------------------------------------
	void Select();

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns identifier of icon if set, otherwise returns pane's identifier
	// Returns:
	//     The identifier of the icon if set, otherwise returns the identifier
	//     of the pane
	//-----------------------------------------------------------------------
	int GetIconID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves icon associated with the pane.
	// Parameters:
	//     nWidth - Width of the icon to retrieve.
	// Returns:
	//     Handle of the icon.
	//-----------------------------------------------------------------------
	virtual CXTPImageManagerIcon* GetIcon(int nWidth = 16) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the identifier of the icon.
	// Parameters:
	//     nID - Identifier of the icon
	//-----------------------------------------------------------------------
	void SetIconID(UINT nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the associated user window.
	// Returns:
	//     Pointer to the associated user child window.
	//-----------------------------------------------------------------------
	CWnd* GetChild() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set options of the pane.
	// Parameters:
	//     dwOptions - Options to be set. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     dwOptions parameter can be one or more of the following:
	//     * <b>xtpPaneNoCloseable</b> Indicates the pane can't be closed.
	//     * <b>xtpPaneNoHideable</b> Indicates the pane can't be hidden.
	//     * <b>xtpPaneNoFloatable</b> Indicates the pane can't be floated.
	//     * <b>xtpPaneNoCaption</b> Indicates the pane has no caption.
	//     * <b>xtpPaneHasMenuButton</b> Indicates the pane has menu button.
	//     * <b>xtpPaneNoDockable</b> Indicates the pane can't be docked.
	// See Also XTPDockingPaneOptions
	//-----------------------------------------------------------------------
	void SetOptions(DWORD dwOptions);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get the options of the pane.
	// Returns:
	//     The pane's data
	// See Also: XTPDockingPaneOptions
	//-----------------------------------------------------------------------
	DWORD GetOptions() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the 32-bit value associated with the pane
	// Parameters:
	//     dwData - Contains the new value to associate with the pane
	// See Also: GetPaneData
	//-----------------------------------------------------------------------
	void SetPaneData(DWORD_PTR dwData);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method retrieves the application-supplied 32-bit value
	//     associated with the tab item.
	// Returns:
	//     The 32-bit value associated with the pane
	// See Also: SetPaneData
	//-----------------------------------------------------------------------
	DWORD_PTR GetPaneData() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function returns the RGB value for the referenced
	//     item tab color or (COLORREF)-1, if the color was not set.
	// Returns:
	//     The RGB value for the referenced item tab color, or (COLORREF)-1,
	//     if the color was not set.
	//-----------------------------------------------------------------------
	virtual COLORREF GetItemColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the RGB value for the ite tab.
	// Parameters:
	//     clr - Color of the item tab.
	//-----------------------------------------------------------------------
	void SetItemColor(COLORREF clr);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to fill the pMinMaxInfo structure with the
	//     minimum and maximum width and height of the container.
	//     This will look at the MinMaxInfo for each pane in the
	//     container and set the appropriate minimum and maximum width
	//     and height for the container.
	// Parameters:
	//     pMinMaxInfo - Pointer to MINMAXINFO structure
	//-----------------------------------------------------------------------
	virtual void GetMinMaxInfo(LPMINMAXINFO pMinMaxInfo) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the state of the child window.
	// Returns:
	//     TRUE if child window is focused.
	//-----------------------------------------------------------------------
	BOOL IsFocus() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set minimum size of pane.
	// Parameters:
	//     sz - Minimum size of pane to set
	//-----------------------------------------------------------------------
	CSize SetMinTrackSize(CSize sz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set maximum size of pane.
	// Parameters:
	//     sz - Maximum size of pane to set
	//-----------------------------------------------------------------------
	CSize SetMaxTrackSize(CSize sz);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sets a context-sensitive help ID for the child pane
	// Parameters:
	//     nIDR - Specifies the context-sensitive help ID.
	//-----------------------------------------------------------------------
	void SetHelpID(int nIDR);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the parent frame window
	// Remarks:
	//     The member function searches up the parent chain until a CFrameWnd
	//     (or derived class) object is found.
	// Returns:
	//     A pointer to a frame window if successful; otherwise NULL
	//-----------------------------------------------------------------------
	CFrameWnd* GetParentFrame() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to retrieve the parent window for the pane
	// Returns:
	//     A pointer to a parent window if successful; otherwise NULL
	//-----------------------------------------------------------------------
	CWnd* GetDockingSite() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if pane is enabled
	// Returns:
	//     TRUE if pane is enabled
	//-----------------------------------------------------------------------
	XTPXTPDockingPaneEnableOptions GetEnabled() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to disable/enable the pane
	// Parameters:
	//     bEnabled - TRUE to enable pane; FALSE to disable
	//-----------------------------------------------------------------------
	void SetEnabled(XTPXTPDockingPaneEnableOptions bEnabled);

protected:
	//-----------------------------------------------------------------------
	// Summary:
	//     Copies self data from another pane
	// Parameters:
	//     pClone  - Pane which members need to copy.
	// See Also: Clone
	//-----------------------------------------------------------------------
	virtual void Copy(CXTPDockingPane* pClone);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to set the position of the pane
	// Parameters:
	//     pParent - Points to a CWnd object.
	//     rect    - Rectangle of the pane to be set.
	//     lParam  - Pointer to AFX_SIZEPARENTPARAMS structure.
	// ----------------------------------------------------------------------
	virtual void OnSizeParent(CWnd* pParent, CRect rect, LPVOID lParam);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set the parent container of the pane.
	// Parameters:
	//     pContainer - Points to a CXTPDockingPaneBase object.
	//-----------------------------------------------------------------------
	virtual void SetParentContainer(CXTPDockingPaneBase* pContainer);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to clone the pane.
	// Parameters:
	//     pLayout          - Points to a CXTPDockingPaneLayout object.
	//     pMap             - Points to a CXTPPaneToPaneMap object.
	//     dwIgnoredOptions - TRUE to ignore options of the pane.
	// Returns:
	//     A pointer to a CXTPDockingPaneBase object.
	// See Also: Copy, CXTPPaneToPaneMap
	//-----------------------------------------------------------------------
	CXTPDockingPaneBase* Clone(CXTPDockingPaneLayout* pLayout, CXTPPaneToPaneMap* pMap, DWORD dwIgnoredOptions = 0);

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
	//     Sets the visibility state of the window.
	// Parameters:
	//     bShow - TRUE to show the window, or FALSE to hide it.
	//-----------------------------------------------------------------------
	void ShowWindow(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the identifier of the item.
	// Parameters:
	//     nID - Identifier of the item.
	//-----------------------------------------------------------------------
	void SetID(int nID);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the bounding rectangle
	// Parameters:
	//     rc - Bounding rectangle to draw.
	//-----------------------------------------------------------------------
	void SetWindowRect(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called to destroy pane
	//-----------------------------------------------------------------------
	virtual void DeletePane();

protected:
//{{AFX_CODEJOCK_PRIVATE
	virtual HRESULT GetAccessibleParent(IDispatch** ppdispParent);
	virtual HRESULT GetAccessibleDescription(VARIANT varChild, BSTR* pszDescription);
	virtual HRESULT GetAccessibleChildCount(long* pcountChildren);
	virtual HRESULT GetAccessibleChild(VARIANT varChild, IDispatch** ppdispChild);
	virtual HRESULT GetAccessibleName(VARIANT varChild, BSTR* pszName);
	virtual HRESULT GetAccessibleRole(VARIANT varChild, VARIANT* pvarRole);
	virtual HRESULT AccessibleLocation(long *pxLeft, long *pyTop, long *pcxWidth, long* pcyHeight, VARIANT varChild);
	virtual HRESULT AccessibleHitTest(long xLeft, long yTop, VARIANT* pvarChild);
	virtual HRESULT GetAccessibleState(VARIANT varChild, VARIANT* pvarState);
	virtual CCmdTarget* GetAccessible();
	virtual HRESULT GetAccessibleDefaultAction(VARIANT varChild, BSTR* pszDefaultAction);
	virtual HRESULT AccessibleDoDefaultAction(VARIANT varChild);
	virtual HRESULT AccessibleSelect(long flagsSelect, VARIANT varChild);

	DECLARE_INTERFACE_MAP()
//}}AFX_CODEJOCK_PRIVATE

protected:
	HWND m_hwndChild;             // Child window handle
	int m_nID;                   // Pane identifier
	int m_nIconID;                // Icon identifier
	CString m_strTitle;           // Caption of the pane
	DWORD m_dwOptions;            // Options of the pane. See XTPDockingPaneOptions
	DWORD_PTR m_dwData;           // User item data
	CPoint m_ptMinTrackSize;      // Minimum pane size.
	CPoint m_ptMaxTrackSize;      // Maximum pane size.
	COLORREF m_clrItemTab;        // Color used to fill the docking pane tab buttons.
	int m_nIDHelp;                // Context-sensitive help ID for a child pane
	XTPXTPDockingPaneEnableOptions m_bEnabled;          // Pane enabled options



private:
	friend class CXTPDockingPaneMiniWnd;
	friend class CXTPDockingPaneManager;
	friend class CXTPDockingPaneTabbedContainer;
	friend class CXTPDockingPaneLayout;
	friend class CDockingPaneCtrl;

};


AFX_INLINE int  CXTPDockingPane::GetID() const {
	return m_nID;
}
AFX_INLINE int CXTPDockingPane::GetIconID() const {
	return m_nIconID == -1 ? m_nID : m_nIconID;
}
AFX_INLINE void CXTPDockingPane::SetIconID(UINT nID) {
	m_nIconID = nID;
}
AFX_INLINE CWnd* CXTPDockingPane::GetChild() const {
	return m_hwndChild ? CWnd ::FromHandle(m_hwndChild): NULL;
}
AFX_INLINE void CXTPDockingPane::SetOptions(DWORD dwOptions) {
	m_dwOptions = dwOptions;
}

AFX_INLINE DWORD_PTR CXTPDockingPane::GetPaneData() const {
	return m_dwData;
}
AFX_INLINE void CXTPDockingPane::SetPaneData(DWORD_PTR dwData) {
	m_dwData = dwData;
}
AFX_INLINE CSize CXTPDockingPane::SetMinTrackSize(CSize sz) {
	CSize szMinTrackSize(m_ptMinTrackSize);
	m_ptMinTrackSize = CPoint(sz);
	return szMinTrackSize;
}
AFX_INLINE CSize CXTPDockingPane::SetMaxTrackSize(CSize sz) {
	CSize szMaxTrackSize(m_ptMaxTrackSize);
	m_ptMaxTrackSize = CPoint(sz);
	return szMaxTrackSize;
}
AFX_INLINE void CXTPDockingPane::SetHelpID(int nIDR) {
	m_nIDHelp = nIDR;
}


#endif // #if !defined(__XTPDOCKINGPANE_H__)
