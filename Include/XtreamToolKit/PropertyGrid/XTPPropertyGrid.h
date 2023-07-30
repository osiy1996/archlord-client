// XTPPropertyGrid.h interface for the CXTPPropertyGrid class.
//
// This file is a part of the XTREME PROPERTYGRID MFC class library.
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
#if !defined(__XTPPROPERTYGRID_H__)
#define __XTPPROPERTYGRID_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "XTPPropertyGridPaintManager.h"

class CXTPToolTipContext;
class CXTPImageManager;

//-----------------------------------------------------------------------
// Summary:
//     XTPPropertyGridSortOrder is an enumeration used by CXTPPropertyGrid to
//     determine the grouping style.  This will specify how the items
//     in the grid are displayed, I.e. Alphabetical, not sort, and
//     grouped by category.
// See Also: CXTPPropertyGrid::SetPropertySort, CXTPPropertyGrid::GetPropertySort
// Example:
//     <code>XTPPropertyGridSortOrder sort = m_wndPropertyGrid.GetPropertySort();</code>
//
// <KEYWORDS xtpGridSortCategorized, xtpGridSortAlphabetical, xtpGridSortNoSort>
//-----------------------------------------------------------------------
enum XTPPropertyGridSortOrder
{
	xtpGridSortCategorized,     // Group items by category.
	xtpGridSortAlphabetical,    // Sort items alphabetically.
	xtpGridSortNoSort           // Disable sorting and grouping, all items are displayed in
	                            // the order that they are added to the grid.
};

#include "XTPPropertyGridView.h"

class CXTPPropertyGridView;
class CXTPPropertyGridPaintManager;
class CXTPPropertyGrid;


//===========================================================================
// Summary:
//     CXTPPropertyGridUpdateContext is a CCmdTarget derived class used
//     to save the state of the property grid such as the currently
//     selected item, XTPPropertyGridSortOrder preference, and the expanding state
//     of each item in the grid.  The property grid state information is
//     stored in the CXTPPropertyGridUpdateContext object when
//     CXTPPropertyGrid::BeginUpdate is called and after CXTPPropertyGrid::EndUpdate
//     the expanding state of each item, selected item, and sort preference
//     will be restored.
// Remarks:
//     Each time you add a new item or category to the grid, The state
//     of the grid is updated the grid is redrawn.
//
//     If you want to delete all items in the grid and then add allot of
//     new items it will be quite slow. To do this, you would call
//     BeginUpdate, then delete all the items.  The property grid will be
//     locked until you call EndUpdate.
//
//     It might be useful to call BeginWaitCursor to indicate that the
//     grid is "busy" updating.
// See Also: CXTPPropertyGrid::BeginUpdate, CXTPPropertyGrid::EndUpdate
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridUpdateContext : public CXTPCmdTarget
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridUpdateContext object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridUpdateContext();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if any items in the grid were
	//     expanded before CXTPPropertyGrid::BeginUpdate was called.
	//     Only items with children can be expanded.
	// Returns:
	//     TRUE if no items were expanded before BeginUpdate, FALSE if
	//     at least one item was expanded.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

protected:
	CMap<UINT, UINT, BOOL, BOOL&> m_mapState;   // Stores the expanded state of each item in the grid before CXTPPropertyGrid::BeginUpdate.
	UINT m_nSelected;                           // Stores the Id of the currently selected item before CXTPPropertyGrid::BeginUpdate.
	UINT m_nTopIndex;                           // Stores the Index of the top-most visible item in the property grid.
	XTPPropertyGridSortOrder m_propertySort;    // Stores the currently used sort method of the grid before CXTPPropertyGrid::BeginUpdate.


	friend class CXTPPropertyGrid;

};

//===========================================================================
// Summary:
//     CXTPPropertyGridToolBar is a CToolBar derived class.
//     It is an internal class used by Property Grid control
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridToolBar : public CToolBar
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridToolBar object
	//-----------------------------------------------------------------------
	CXTPPropertyGridToolBar();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridToolBar object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	~CXTPPropertyGridToolBar();

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPPropertyGridToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPPropertyGridToolBar)
	afx_msg BOOL OnEraseBkgnd(CDC*);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg void OnPaint();
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

private:
	CXTPToolTipContext* m_pToolTipContext;

};

class CXTPPropertyGridVerbs;


//===========================================================================
// Summary:
//     CXTPPropertyGridVerb is used to create a verb link similar to what is seen
//     in Visual Studio .NET.  For example, when the DataGrid control is selected at
//     design time in VS .NET, an AutoFormat link button appears in the Properties window
//     just below the property grid and above the help panel.
//     Verbs will be displayed in a separate panel just below the property grid.  Verbs
//     are links that can be used to perform actions in your application.  When a verb is
//     clicked, CXTPPropertyGrid::OnVerbClick is called by the framework.
//     If both the verb panel and help panel are visible at the same time, the verb panel
//     is displayed above the help panel.
// See Also: CXTPPropertyGrid::GetVerbs, CXTPPropertyGridVerbs
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridVerb : public CXTPCmdTarget
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridVerb object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridVerb();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the text caption of this verb link.
	// Returns:
	//     Text caption of this verb link.
	//-----------------------------------------------------------------------
	CString GetCaption() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the Id assigned to this verb link.
	// Returns:
	//     The Id of this verb link.
	//-----------------------------------------------------------------------
	int GetID() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the Index of this verb link within the
	//     collection of verbs.
	// Returns:
	//     Index of this verb within the collection of verb links.
	//-----------------------------------------------------------------------
	int GetIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if this verb link is currently focused.
	//     If focused, a focus rectangle is placed around the text caption of the verb.
	// Returns:
	//     TRUE if this verb is currently focused, FALSE if this verb is not focused.
	//-----------------------------------------------------------------------
	BOOL IsFocused() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Used internally by PaintManager and PropertyGrid to set the
	//     bounding rectangle for verbs.  You do not need to call
	//     this member.
	// Parameters:
	//     rc - Bounding rectangle containing the verb text.
	//-----------------------------------------------------------------------
	void SetPart(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     Used internally by PaintManager and PropertyGrid to get the
	//     bounding rectangle for verbs.  You do not need to call
	//     this member.
	// Returns:
	//     Bounding rectangle of verb.
	//-----------------------------------------------------------------------
	CRect GetPart() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the last position that the user
	//     clicked.
	// Returns:
	//     Retrieves the last position the user clicked. If it was "clicked"
	//     by pressing the Enter key while a verb has focus, then the
	//     BottomLeft position of the "clicked" Verb is returned.
	//-----------------------------------------------------------------------
	CPoint GetClickPoint() const;

protected:
	CString m_strCaption;               // Text caption of this verb link.
	int m_nID;                          // Id assigned this verb link.
	int m_nIndex;                       // Index of this verb link in the collection of verbs.
	CRect m_rcPart;                     // Bounding rectangle of verb.
	CPoint m_ptClick;                   // The last position the user clicked.
	CXTPPropertyGridVerbs* m_pVerbs;    // Collection of verb links.

	friend class CXTPPropertyGridVerbs;
	friend class CXTPPropertyGrid;

};



//===========================================================================
// Summary:
//     A collection of CXTPPropertyGridVerb objects that are displayed in the
//     verb panel.  See CXTPPropertyGridVerb for a description of verbs.
// See Also: CXTPPropertyGridVerb
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGridVerbs : public CXTPCmdTarget
{
public:

	//-------------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGridVerbs object.
	//-------------------------------------------------------------------------
	CXTPPropertyGridVerbs();

	//-------------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGridVerbs object, handles cleanup and deallocation.
	//-------------------------------------------------------------------------
	~CXTPPropertyGridVerbs();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if any verbs have been added to the
	//     collection of verbs.
	// Returns:
	//     TRUE if there are no verbs in the collection, FALSE if there is at
	//     least one verb in the collection.
	//-----------------------------------------------------------------------
	BOOL IsEmpty() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to remove all verbs from the collection of verbs.
	//-----------------------------------------------------------------------
	void RemoveAll();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to get a pointer to a specific verb in the collection
	//     of verbs.
	// Parameters:
	//     nIndex - Index of the verb withing the collection of verbs.
	// Returns:
	//     Verb at the specified location in the collection of verbs.
	//-----------------------------------------------------------------------
	CXTPPropertyGridVerb* GetAt(int nIndex) const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to retrieve the total number of verb links in the
	//     collection of verbs.
	// Returns:
	//     Total number of verbs in the collection of verbs.
	//-----------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to add a verb link to the verb panel.
	// Parameters:
	//     lpszCaption - Text caption of the verb link.
	//     nID        - Id to assign to the new verb link.
	// Remarks:
	//     This will add a CXTPPropertyGridVerb object to the collection of
	//     verbs.  Verbs are displayed in the verb panel that is located just
	//     below the property grid and above the help panel.
	//-----------------------------------------------------------------------
	void Add(LPCTSTR lpszCaption, UINT nID);

protected:
	CArray<CXTPPropertyGridVerb*, CXTPPropertyGridVerb*> m_arrVerbs;    // Collection of verb links.
	CXTPPropertyGrid* m_pGrid;                                          // Pointer to the property grid that the verbs are drawn on.

	friend class CXTPPropertyGrid;
	friend class CXTPPropertyGridVerb;

};

//-----------------------------------------------------------------------
// Summary:
//     XTPPropertyGridUI is an enumeration used by CXTPPropertyGrid
//     during navigation of the Property Grid, items, and verbs
//     that specifies which object should receive focus.
// Remarks:
//     After the Tab key or Shift+Tab key combinations are pressed,
//     focus will be given to the object specified in the XTPPropertyGridUI
//     enumeration.  I.e. If xtpGridUIViewNext is passed into
//     OnNaviagte when an item in the grid has focus, then the next
//     item will receive focus.
// See Also: CXTPPropertyGrid::OnNavigate, CXTPPropertyGrid::NavigateItems
//
// <KEYWORDS xtpGridUIParentPrev, xtpGridUIViewPrev, xtpGridUIView, xtpGridUIInplaceEdit, xtpGridUIInplaceButton, xtpGridUIViewNext, xtpGridUIVerb, xtpGridUIParentNext>
//-----------------------------------------------------------------------
enum XTPPropertyGridUI
{
	xtpGridUIParentPrev      = -2,  // Used to indicate that focus should be given to the previous object in the tab order.
	xtpGridUIViewPrev        = -1,  // If m_bTabItems is TRUE, Used to indicate that focus should be given to the previous item in the grid.
	                                // This occurs when an item has focus and Shift+Tab is pressed.
	xtpGridUIView            =  0,  // Used to indicate that focus should be given to the Property Grid.
	xtpGridUIInplaceEdit     =  1,  // Used to indicate that the Tab key is pressed while in an in-place edit box has focus.
	xtpGridUIInplaceControl  =  2,  // Used to indicate that the Tab key is pressed while in an in-place edit box has focus.
	xtpGridUIInplaceButton   =  3,  // Used to indicate that the Tab key is pressed while an in-place button has focus.
	xtpGridUIViewNext        =  4,  // If m_bTabItems is TRUE, Used to indicate that focus should be given to the next item in the grid.
	                                // This occurs when an item has focus and the Tab key is pressed.
	xtpGridUIVerb            =  5,  // Used to indicate that the Tab key is pressed while in Verb has focus.
	xtpGridUIParentNext      =  6   // Used to indicate that focus should be given to the next object in the tab order.
};

//-----------------------------------------------------------------------
// Summary:
//     XTPPropertyGridHitCode is an enumeration used by CXTPPropertyGrid
//     to determine which part of the property grid is located at a
//     specific point. CXTPPropertyGrid::HitTest is used to test
//     a point when the user clicks on the property grid.
// See Also:
//     CXTPPropertyGrid::HitTest
// Example:
//     See CXTPPropertyGrid::HitTest.
//
// <KEYWORDS xtpGridHitError, xtpGridHitHelpSplitter, xtpGridHitVerbsSplitter, xtpGridHitFirstVerb>
//-----------------------------------------------------------------------
enum XTPPropertyGridHitCode
{
	xtpGridHitError         = -1,   // Indicates that the help splitter, verb splitter
	                                // and verbs were not clicked.
	xtpGridHitHelpSplitter  =  1,   // Indicates that the help splitter was clicked.
	xtpGridHitVerbsSplitter =  2,   // Indicates that the Verb splitter was clicked.
	xtpGridHitFirstVerb     =  3    // Determines if the item clicked upon is a verb.
};

//===========================================================================
// Summary:
//     CXTPPropertyGrid is a CWnd derived class. It is used to create a property grid
//     control similar to the property window in VS.NET.
//     See the "PropertyGrid" demo for an example of its usage.
//===========================================================================
class _XTP_EXT_CLASS CXTPPropertyGrid : public CWnd
{
	DECLARE_DYNAMIC(CXTPPropertyGrid)

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPPropertyGrid object
	//-----------------------------------------------------------------------
	CXTPPropertyGrid();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPPropertyGrid object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPPropertyGrid();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to create the Property Grid control.
	// Parameters:
	//     rect        - Size of the control.
	//     pParentWnd  - Parent of the control.
	//     nID         - Identifier of the Property Grid control.
	//     dwListStyle - List style of the items in the Property Grid.
	// Remarks:
	//     Available styles are listed below:
	//
	//         <p><b>LBS_OWNERDRAWFIXED</b> - The owner of the grid is
	//             responsible for drawing its contents; the items in the
	//             list box are the same height, should be used with
	//             LBS_NOINTEGRALHEIGHT.
	//         <p><b>LBS_OWNERDRAWVARIABLE</b> - The owner of the grid is
	//             responsible for drawing its contents; the items
	//             in the grid are variable in height, should be used with
	//             LBS_NOINTEGRALHEIGHT
	//         <p><b>XTP_PGS_OWNERDRAW</b> - Apply this style to the property
	//             grid to use the owner draw feature. Catch XTP_PGN_DRAWITEM
	//             to draw item. Recommended to use custom Paint Manager
	//             instead. (SetCustomTheme)
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	// See Also: SetCustomTheme, SetTheme
	//-----------------------------------------------------------------------
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID, DWORD dwListStyle = 0);

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to add a new category to the Property Grid control.
	// Parameters:
	//     lpszCaption - Name of the category to add.
	// Parameters:
	//     nID - Identifier of new category.
	// Returns:
	//     The pointer to the item object of the newly inserted category.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* AddCategory(LPCTSTR lpszCaption);
	CXTPPropertyGridItem* AddCategory(int nID);//<COMBINE CXTPPropertyGrid::AddCategory@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to insert a new category to the Property Grid control.
	// Parameters:
	//     lpszCaption - Name of the category to add.
	//     nID    - Identifier of new category.
	//     nIndex - Index of category to be inserted
	// Returns:
	//     The pointer to the item object of the newly inserted category.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* InsertCategory(int nIndex, LPCTSTR lpszCaption);
	CXTPPropertyGridItem* InsertCategory(int nIndex, int nID);//<COMBINE CXTPPropertyGrid::InsertCategory@int@LPCTSTR>

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to modify the way items are grouped.
	// Parameters:
	//     sort - style of grouping. Can be any of the values listed in the Remarks section.
	// Remarks:
	//     sort parameter can be one of the following:
	//     * <b>xtpGridSortCategorized</b> Group items by category.
	//     * <b>xtpGridSortAlphabetical</b> Sort items alphabetically.
	//     * <b>xtpGridSortNoSort</b> Disable sorting.
	// See Also: GetPropertySort
	//-----------------------------------------------------------------------
	void SetPropertySort (XTPPropertyGridSortOrder sort);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the current grouping style for
	//     the Property Grid control.
	// Returns:
	//     An XTPPropertyGridSortOrder value representing the current grouping style.
	// See Also: SetPropertySort
	//-----------------------------------------------------------------------
	XTPPropertyGridSortOrder GetPropertySort() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide the toolbar control
	//     in the Property Grid window.
	// Parameters:
	//     bShow - TRUE to show the toolbar control, or FALSE to hide it.
	//-----------------------------------------------------------------------
	void ShowToolBar(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to show or hide the description pane
	//     in the Property Grid window.
	// Parameters:
	//     bShow - TRUE to show the description pane, or FALSE to hide it.
	//-----------------------------------------------------------------------
	void ShowHelp(BOOL bShow = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the custom colors of the Property
	//     Grid control.
	// Parameters:
	//     clrHelpBack - An RGB value that represents the background color
	//                   the of description pane.
	//     clrHelpFore - An RGB value that represents the text color of
	//                   the description pane.
	//     clrViewLine - An RGB value that represents the color of separating
	//                   lines in control.
	//     clrViewBack - An RGB value that represents the background color
	//                   of the Property Grid control.
	//     clrViewFore - An RGB value that represents the text color of
	//                   the Property Grid control.
	//     clrCategoryFore - An RGB value that represents the text color of
	//                   the Category item text.
	//-----------------------------------------------------------------------
	void SetCustomColors(COLORREF clrHelpBack, COLORREF clrHelpFore, COLORREF clrViewLine, COLORREF clrViewBack, COLORREF clrViewFore, COLORREF clrCategoryFore = ::GetSysColor(COLOR_GRAYTEXT));

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the default colors of the Property
	//     Grid control.
	//-----------------------------------------------------------------------
	void SetStandardColors();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to find an item.
	// Parameters:
	//     strCaption - Caption for the item to find.
	//     nID        - Identifier for the item to find.
	// Returns:
	//     The pointer to the CXTPPropertyGridItem object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* FindItem(LPCTSTR strCaption) const;
	CXTPPropertyGridItem* FindItem(UINT nID) const; // <COMBINE CXTPPropertyGrid::FindItem@LPCTSTR@const>

	//-----------------------------------------------------------------------
	// Summary:
	//     Removes all items from the list box of a property grid control.
	//-----------------------------------------------------------------------
	void ResetContent();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the help pane is visible.
	// Returns:
	//     TRUE if the help pane is visible, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsHelpVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to determine if the toolbar is visible.
	// Returns:
	//     TRUE if the toolbar is visible, otherwise returns FALSE.
	//-----------------------------------------------------------------------
	BOOL IsBarVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the height of the help pane
	//     when it is visible.
	// Returns:
	//     An integer value that represents the height in pixels.
	//-----------------------------------------------------------------------
	int GetHelpHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the background color for
	//     the property grid.
	// Returns:
	//     An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetHelpBackColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the foreground color for
	//     the property grid.
	// Returns:
	//     An RGB value.
	//-----------------------------------------------------------------------
	COLORREF GetHelpForeColor() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the categories of the property
	//     grid.
	// Returns:
	//     The pointer to the CXTPPropertyGridItems object that represents
	//     collection of the categories.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItems* GetCategories() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to return a pointer to the collection of verbs that
	//     are displayed in the verb panel.
	// Returns:
	//     The pointer to the CXTPPropertyGridVerbs object that represents
	//     the collection of verbs.
	//-----------------------------------------------------------------------
	CXTPPropertyGridVerbs* GetVerbs() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve reference to the view object.
	// Returns:
	//     A CXTPPropertyGridView object
	//-----------------------------------------------------------------------
	CXTPPropertyGridView& GetGridView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to retrieve reference to the toolbar for the
	//     property grid.
	// Returns:
	//     A reference to a CToolBar object
	//-----------------------------------------------------------------------
	CToolBar& GetToolBar();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to get selected item.
	// Returns:
	//     The pointer to the CXTPPropertyGridItem object if the specified
	//     item is selected; otherwise, it is NULL.
	//-----------------------------------------------------------------------
	CXTPPropertyGridItem* GetSelectedItem() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the height of the help pane
	//     when visible.
	// Parameters:
	//     nHeight - The integer value represents the height in pixels.
	//-----------------------------------------------------------------------
	void SetHelpHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the divider position of the grid view.
	// Parameters:
	//     dDivider - A value between 0 and 1 that represents the location of the divider as
	//                a percentage from the left side of the view.  For example, if you wanted
	//                the divider to divide at 25% you would pass in .25.
	//-----------------------------------------------------------------------
	void SetViewDivider(double dDivider);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the divider position of the grid view.
	// Parameters:
	//     nDivider     - A value in pixels that represents the location of the divider.
	//     bLockDivider - TRUE to not scale divider if user resize property grid.
	//-----------------------------------------------------------------------
	void SetViewDividerPos(int nDivider, BOOL bLockDivider = FALSE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the divider position of the grid view.
	// Returns:
	//     A value between 0 and 1 that represents the location of the divider as
	//     a percentage from the left side of the view.  For example, if you wanted
	//     the divider to divide at 25% you would pass in .25.
	//-----------------------------------------------------------------------
	double GetViewDivider() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This method sets the grid's current font to the specified font.
	// Parameters:
	//     pFont - Pointer to a CFont object.
	//-----------------------------------------------------------------------
	void SetFont(CFont* pFont);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set the grid theme.
	// Parameters:
	//     paintTheme - theme of property grid. Can be one of the values
	//                  listed in the Remarks section.
	// Remarks:
	//     paintTheme can be one of the following values:
	//      * <b>xtpGridThemeDefault</b> Enables default theme.
	//      * <b>xtpGridThemeNativeWinXP</b> Enables WinXP style theme.
	//      * <b>xtpGridThemeOffice2003</b> Enables Office 2003 style theme.
	//      * <b>xtpGridThemeCool</b> Enables Cool theme.
	//      * <b>xtpGridThemeSimple</b> Enables Visual Basic style theme.
	//      * <b>xtpGridThemeDelphi</b> Enables Delphi style theme.
	//      * <b>xtpGridThemeWhidbey</b> Enables Visual Studio 2005 "Whidbey" style theme.
	//-----------------------------------------------------------------------
	void SetTheme(XTPPropertyGridPaintTheme paintTheme);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom theme of the grid.
	// Parameters:
	//     pPaintManager - Points to a CXTPPropertyGridPaintManager object
	//-----------------------------------------------------------------------
	void SetCustomTheme(CXTPPropertyGridPaintManager* pPaintManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves current theme of the grid.
	//-----------------------------------------------------------------------
	XTPPropertyGridPaintTheme GetCurrentTheme() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to refresh items in the list.
	//-----------------------------------------------------------------------
	void Refresh();

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to "lock" the property grid.  This
	//     should be called when adding or deleting a large number of item.
	// Parameters:
	//     context - Pointer to a CXTPPropertyGridUpdateContext object.
	// Remarks:
	//     Each time you add a new item or category to the grid, The state
	//     of the grid is updated the grid is redrawn.
	//
	//           If you want to delete all items in the grid and then add allot of
	//           new items it will be quite slow. To do this, you would call
	//           BeginUpdate, then delete all the items.  The property grid will be
	//           locked until you call EndUpdate.
	//
	//           It might be useful to call BeginWaitCursor to indicate that the
	//           grid is "busy" updating.
	// See Also: EndUpdate, CXTPPropertyGridUpdateContext
	//-----------------------------------------------------------------------
	void BeginUpdate(CXTPPropertyGridUpdateContext& context);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member should be called after BeginUpdate.  You need to call
	//     EndUpdate to "unlock" the grid after you have finished adding
	//     and deleting a large amount of items.
	// Parameters:
	//     context - Pointer to a CXTPPropertyGridUpdateContext object.
	// See Also: BeginUpdate, CXTPPropertyGridUpdateContext
	//-----------------------------------------------------------------------
	void EndUpdate(CXTPPropertyGridUpdateContext& context);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if the verb panel is currently visible.
	// Returns:
	//     TRUE if the verb panel is visible, FALSE otherwise.
	// See Also: CXTPPropertyGridVerb, CXTPPropertyGridVerbs
	//-----------------------------------------------------------------------
	BOOL IsVerbsVisible() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Returns the currently used control's Paint Manager.
	// Remarks:
	//     Call this member function to get the paint manager object used
	//     for drawing a property grid control window.
	// Returns:
	//     Pointer to the paint manager object.
	//-----------------------------------------------------------------------
	CXTPPropertyGridPaintManager* GetPaintManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Creates View of Property grid. Override this member to use custom view.
	//-----------------------------------------------------------------------
	virtual CXTPPropertyGridView* CreateView() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when a user clicks on a verb or when a
	//     verb has focus and the enter key is pressed.  Override this
	//     member to perform custom actions such as displaying a dialog.
	// Parameters:
	//     nIndex - Index of the verb within the collection of verbs.
	//     pt     - Point on the screen that the user clicked.  This
	//              will the position of the mouse cursor over a verb.  This
	//              point is stored in the CXTPPropertyGridVerb::m_ptClick
	//              member which is a CPoint object.  Coordinates will be
	//              relative to the screen and not the client.
	//-----------------------------------------------------------------------
	virtual void OnVerbClick(int nIndex, CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when items in the property grid are navigated
	//     using key on the keyboard.  This includes navigating the built in
	//     toolbar, verbs, and items in the property grid.
	// Parameters:
	//     nUIElement - Indicates which object should receive focus.
	//                  Members of XTPPropertyGridUI are used to indicate
	//                  which object receives focus.
	//     bForward   - TRUE if the Tab key is pressed and items are being
	//                  navigated in the forward direction, FALSE if Shift+Tab
	//                  is pressed and items are navigated backwards.
	//                  TRUE if the Right or Down arrow is used to navigate the items.
	//                  FALSE if the Up or Left arrow is used to navigate the items.
	//     pItem      - Pointer to the currently selected item.
	// See Also: XTPPropertyGridUI
	//-----------------------------------------------------------------------
	virtual void OnNavigate(XTPPropertyGridUI nUIElement, BOOL bForward, CXTPPropertyGridItem* pItem);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to enable navigate items using Tab key.
	// Parameters:
	//     bTabItems - TRUE to navigate items in the grid using the tab key.
	//         When TRUE, focus will move in this order:
	//         Caption, Item Value, Item InPlaceButton (if used), Next item Caption
	//         FALSE not to navigate the items in the grid with the tab key.
	//-----------------------------------------------------------------------
	void NavigateItems(BOOL bTabItems);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to show all in-place buttons of all items
	// Parameters:
	//     bShow - TRUE to always show in-place buttons; FALSE to show buttons only for selected item.
	// See Also: GetShowInplaceButtonsAlways
	//-----------------------------------------------------------------------
	void SetShowInplaceButtonsAlways(BOOL bShow);

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if in-place buttons are visible for all items.
	//-----------------------------------------------------------------------
	BOOL GetShowInplaceButtonsAlways() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to specify the height of the verb box when visible.
	// Parameters:
	//     nHeight - Height of the verb box.
	// See Also: CXTPPropertyGridVerb, CXTPPropertyGridVerbs, IsVerbsVisible
	//-----------------------------------------------------------------------
	void SetVerbsHeight(int nHeight);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to determine if changed items highlighter
	// See Also: HighlightChangedItems, CXTPPropertyGridItem::IsValueChanged
	//-----------------------------------------------------------------------
	BOOL IsHighlightChangedItems() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to highlight all item with changed values
	// Parameters:
	//     bHighlightChanged - TRUE to set bold font for all changed items
	// See Also: IsHighlightChangedItems, CXTPPropertyGridItem::IsValueChanged
	//-----------------------------------------------------------------------
	void HighlightChangedItems(BOOL bHighlightChanged);

	//-------------------------------------------------------------------------
	// Summary:
	//     Call this member to redraw control;
	//-------------------------------------------------------------------------
	void RedrawControl();

	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves Image manager associated with property grid
	// Returns:
	//     Pointer to the image manager object that stores the images in
	//     the property grid.
	//-----------------------------------------------------------------------
	CXTPImageManager* GetImageManager() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to assign new image manager.
	// Parameters:
	//     pImageManager - Pointer to the image manager object that stores the
	//     images in the property grid.
	//-----------------------------------------------------------------------
	void SetImageManager(CXTPImageManager* pImageManager);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to allow variable height items
	// Parameters:
	//     bVariable - TRUE to allow variable height items
	// See Also: CXTPPropertyGridItem::SetHeight, CXTPPropertyGridItem::SetMultiLinesCount
	//-----------------------------------------------------------------------
	void SetVariableItemsHeight(BOOL bVariable = TRUE);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom in-place edit control.
	// Parameters:
	//     pInplaceEdit - New custom edit control to be set
	// Example:
	//     <code>m_wndPropertyGrid.SetInplaceEdit(new CMyInplaceEdit());</code>
	// See Also: SetInplaceList, CXTPPropertyGridItem::GetInplaceEdit
	//-----------------------------------------------------------------------
	void SetInplaceEdit(CXTPPropertyGridInplaceEdit*  pInplaceEdit);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set custom in-place list box control.
	// Parameters:
	//     pInplaceList - New custom list box control to be set
	// Example:
	//     <code>m_wndPropertyGrid.SetInplaceList(new CMyInplaceList());</code>
	// See Also: SetInplaceEdit, CXTPPropertyGridItem::GetInplaceList
	//-----------------------------------------------------------------------
	void SetInplaceList(CXTPPropertyGridInplaceList*  pInplaceList);

	//-----------------------------------------------------------------------
	// Summary:
	//     Sends the specified message to the owner window.
	// Parameters:
	//     wParam - Specifies additional message-dependent information.
	//     lParam - Specifies additional message-dependent information
	// Returns:
	//     The result of the message processing; its value depends on the message sent.
	//-----------------------------------------------------------------------
	virtual LRESULT SendNotifyMessage(WPARAM wParam = 0, LPARAM lParam = 0);


	//-----------------------------------------------------------------------
	// Summary:
	//     Retrieves the zero-based index of the first visible item in a control.
	//     Initially, item 0 is at the top of the property grid, but if the list box is scrolled, another item may be at the top.
	// Returns:
	//     The zero-based index of the first visible item in a control if successful, -1 otherwise.
	//-----------------------------------------------------------------------
	int GetTopIndex() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Ensures that a particular list-box item is visible.
	//     Scrolls the property grid until either the item specified by nIndex appears at the top
	//     of the control or the maximum scroll range has been reached.
	// Parameters:
	//     nIndex - Specifies the zero-based index of the item.
	//-----------------------------------------------------------------------
	void SetTopIndex(int nIndex);

	//-------------------------------------------------------------------------
	// Summary:
	//     Retrieves the number of items in a property grid.
	// Returns:
	//     The number of items in the property grid, or LB_ERR if an error occurs.
	// Remarks:
	//     The returned count is one greater than the index value of the last item
	//     (the index is zero-based).
	//-------------------------------------------------------------------------
	int GetCount() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Register the window class if it has not already been registered.
	// Parameters:
	//     hInstance - Instance of resource where control is located
	// Returns:
	//     TRUE if the window class was successfully registered.
	//-----------------------------------------------------------------------
	BOOL RegisterWindowClass(HINSTANCE hInstance = NULL);

	//-----------------------------------------------------------------------
	// Summary: Specifies whether the user can move the property grid splitter.
	// Parameters:
	//               bVariable - TRUE to allow the user to resize the property grid splitter.
	// Remarks:
	//      SetVariableSplitterPos allows the user to resize the splitter by
	//      moving the mouse cursor over the splitter and dragging the mouse.
	//      A resize mouse cursor will appear when the mouse if positioned
	//      over the splitter.
	// See Also: SetViewDividerPos
	//-----------------------------------------------------------------------
	void SetVariableSplitterPos(BOOL bVariable);

	//-----------------------------------------------------------------------
	// Summary: Determines whether the user can move the property grid splitter.
	// Remarks:
	//      SetVariableSplitterPos allows the user to resize the splitter by
	//      moving the mouse cursor over the splitter and dragging the mouse.
	//      A resize mouse cursor will appear when the mouse if positioned
	//      over the splitter.
	// Returns:
	//      TRUE if the user is allowed to resize the property grid splitter.
	// See Also: SetVariableSplitterPos, SetViewDividerPos
	//-----------------------------------------------------------------------
	BOOL GetVariableSplitterPos() const;

	//-----------------------------------------------------------------------
	// Summary: Specifies whether the user can resize the help box with the mouse.
	// Parameters:
	//               bVariable - TRUE to allow the user to resize the help box with the mouse.
	// Remarks:
	//      SetVariableHelpHeight allows the user to resize the help box by moving
	//      the mouse cursor over the help splitter that appears between the
	//      bottom of the property grid and the top of the help box.  A resize
	//      mouse cursor will appear when the mouse if positioned over the splitter.
	// See Also: SetHelpHeight, GetVariableHelpHeight
	//-----------------------------------------------------------------------
	void SetVariableHelpHeight(BOOL bVariable);

	//-----------------------------------------------------------------------
	// Summary: Determines whether the user can resize the help box with the mouse.
	// Remarks:
	//      SetVariableHelpHeight allows the user to resize the help box by moving
	//      the mouse cursor over the help splitter that appears between the
	//      bottom of the property grid and the top of the help box.  A resize
	//      mouse cursor will appear when the mouse if positioned over the splitter.
	// Returns:
	//      TRUE if the user is allow to resize the help box with the mouse.
	// See Also: SetHelpHeight, SetVariableHelpHeight
	//-----------------------------------------------------------------------
	BOOL GetVariableHelpHeight() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to set border style for Property Grid
	// Parameters:
	//     borderStyle - borders style to be set
	// See Also: GetBorderStyle, XTPPropertyGridBorderStyle
	//-----------------------------------------------------------------------
	void SetBorderStyle(XTPPropertyGridBorderStyle borderStyle);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this method to get current border style for Property Grid
	// See Also: SetBorderStyle, XTPPropertyGridBorderStyle
	//-----------------------------------------------------------------------
	XTPPropertyGridBorderStyle GetBorderStyle() const;

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Determines if right-to-left mode was set.
	// Returns:
	//     TRUE if text is displayed using right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	BOOL IsLayoutRTL() const;

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member to set right-to-left mode.
	// Parameters:
	//     bRightToLeft - TRUE to set right-to-left reading-order properties.
	//-----------------------------------------------------------------------
	void SetLayoutRTL(BOOL bRightToLeft);

	//-------------------------------------------------------------------------
	// Summary:
	//     This method is called to reposition view and help pane inside grid
	//-------------------------------------------------------------------------
	void Reposition();

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This method is called by the framework during resizing of splitters.
	// Parameters:
	//     rc - Specifies the reference to a CRect object specifying
	//          the tracking rectangle.
	//-----------------------------------------------------------------------
	void OnInvertTracker(CRect rc);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to determine if the help splitter,
	//     verb splitter, or a verb was clicked upon.
	// Parameters:
	//     pt - A point to test.
	// Returns:
	//     The HitTest method returns the following values which specify which
	//     part of the property grid located under the coordinates specified
	//     by <i>pt</i>.
	//
	//     * <b>xtpGridHitError</b> Indicates something else was clicked.
	//     * <b>xtpGridHitHelpSplitter</b> Indicates the help splitter was clicked.
	//     * <b>xtpGridHitVerbsSplitter</b> Indicates the verb splitter was clicked.
	//     * <b>xtpGridHitFirstVerb</b> If the return value is greater than or
	//           equal to xtpGridHitFirstVerb then the item clicked upon is
	//           a verb. You can retrieve the verb index by subtracting
	//           xtpGridHitFirstVerb from the return value, for example:
	// <code>
	// int iHit = m_wndPropertyGrid.HitTest(pt);
	// if (iHit >= xtpGridHitFirstVerb)
	// {
	//     // Get the index of the verb.
	//     m_iVerbIndex = (iHit-xtpGridHitFirstVerb);
	// }
	// </code>
	// See Also:
	//     XTPPropertyGridHitCode
	//-----------------------------------------------------------------------
	virtual int HitTest(CPoint pt);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called when a new item is selected in the
	//     grid.  Or focus switched from one item to a different item.
	// Parameters:
	//     pItem - Item that the selection has changed too.  This is the
	//             item that just received focus.
	// See Also: GetSelectedItem
	//-----------------------------------------------------------------------
	virtual void OnSelectionChanged(CXTPPropertyGridItem* pItem);

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called when the sort order has changed.
	// See Also: SetPropertySort
	//-------------------------------------------------------------------------
	virtual void OnSortChanged();

	//-----------------------------------------------------------------------
	// Summary:
	//     This member is called to recalculate the position and size of the
	//     property grid, then it resizes the grid to the specified size.
	//     New size is 0, 0, cx, cy.
	//     This is the area that hold the toolbar and grid, this does not
	//     include the area for the verb and help panel.
	// Parameters:
	//     cx - New bottom-right x coordinate of the grid's bounding rectangle.
	//     cy - New bottom-right y coordinate of the grid's bounding rectangle.
	//-----------------------------------------------------------------------
	virtual void Reposition(int cx, int cy);

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called when the "built-in" toolbar is created.
	//-------------------------------------------------------------------------
	void CreateToolbar();

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called when the toolbar buttons need to be redrawn.
	// Remarks:
	//     This occurs when the sort order has changed, and when the toolbar
	//     is shown.  Sort order can be changed by code or by clicking on
	//     a button in the toolbar.
	//-------------------------------------------------------------------------
	void RefreshToolBarButtons();

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called when a Verb receives focus, and when focus
	//     switches from one verb to a different verb.
	//-------------------------------------------------------------------------
	void OnVerbsChanged();

	//-------------------------------------------------------------------------
	// Summary:
	//     This member is called to create grid view window
	// Parameters:
	//     dwListStyle - Default List style of the view
	// Returns:
	//     TRUE if successful, otherwise returns FALSE.
	//-------------------------------------------------------------------------
	BOOL CreateGridView(DWORD dwListStyle);

private:
	CWnd* GetNextGridTabItem(BOOL bForward);
	void RestoreExpandingState(CXTPPropertyGridItems* pItems, CXTPPropertyGridUpdateContext& context);
	void SaveExpandingState(CXTPPropertyGridItems* pItems, CXTPPropertyGridUpdateContext& context);



public:
	int                         m_nHelpHeight;              // Height of the description area (help panel).
	int                         m_nVerbsHeight;             // Height of the verbs panel.

protected:
	BOOL                        m_bHelpVisible;             // TRUE if the help panel is visible.
	BOOL m_bVariableHelpHeight;
	BOOL                        m_bToolBarVisible;          // TRUE if the built in toolbar is visible.
	CXTPPropertyGridToolBar     m_wndToolbar;               // Pointer to the "built-in" PropertyGrid toolbar.
	HCURSOR                     m_hCursorSplit;             // Handle to the cursor when positioned over a splitter.
	HCURSOR                     m_hCursorHand;              // Handle of the cursor.
	BOOL                        m_bTabItems;                // TRUE to navigate items with the tab key.

	CXTPPropertyGridVerbs*      m_pVerbs;                   // Collection of verbs (links) displayed in the verb panel when the panel is visible.
	BOOL                        m_bVerbsVisible;            // TRUE if the verb panel is visible.

	XTPPropertyGridPaintTheme   m_themeCurrent;             // Currently set theme.
	CXTPPropertyGridPaintManager* m_pPaintManager;          // Current paint manager.
	int                         m_nFocusedVerb;             // Index of the currently focused verb within the collection of verbs.
	mutable CXTPPropertyGridView* m_pView;                    // View pointer
	BOOL                        m_bHighlightChanged;        // TRUE to highlight changed values
	CXTPImageManager*           m_pImageManager;            // Image manager of property grid
	BOOL                        m_bVariableItemsHeight;     // TRUE to allow variable items height
	BOOL                        m_bPreSubclassWindow;       // 'true' when initialized from PreSubclassWindow.

	CXTPPropertyGridInplaceEdit*   m_pInplaceEdit;          // In-place edit control.
	CXTPPropertyGridInplaceList*   m_pInplaceListBox;       // In-place list control.
	BOOL                       m_bShowInplaceButtonsAlways;         // TRUE to show all inplace buttons

private:
	CRect                       m_rectTracker;
	BOOL                        m_bVerbActivate;
	XTPPropertyGridBorderStyle m_borderStyle;

protected:

//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_VIRTUAL(CXTPPropertyGrid)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPPropertyGrid)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSortAlphabetic();
	afx_msg void OnSortCategorized();
	afx_msg void OnSysColorChange();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
	afx_msg BOOL OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()

//}}AFX_CODEJOCK_PRIVATE

private:
	BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	void SetLayoutRTL(CWnd* pWnd, BOOL bRTLLayout);

	friend class CXTPPropertyGridView;
	friend class CPropertyGridCtrl;
	friend class CXTPPropertyGridVerb;
	friend class CXTPPropertyGridVerbs;
	friend class CXTPPropertyGridItem;
};

//////////////////////////////////////////////////////////////////////
AFX_INLINE BOOL CXTPPropertyGridUpdateContext::IsEmpty() const {
	return m_mapState.IsEmpty();
}

AFX_INLINE CString CXTPPropertyGridVerb::GetCaption() const {
	return m_strCaption;
}
AFX_INLINE int CXTPPropertyGridVerb::GetID() const {
	return m_nID;
}
AFX_INLINE int CXTPPropertyGridVerb::GetIndex() const {
	return m_nIndex;
}
AFX_INLINE void CXTPPropertyGridVerb::SetPart(CRect rc) {
	m_rcPart = rc;
}
AFX_INLINE CRect CXTPPropertyGridVerb::GetPart() const {
	return m_rcPart;
}
AFX_INLINE CPoint CXTPPropertyGridVerb::GetClickPoint() const {
	return m_ptClick;
}
AFX_INLINE BOOL CXTPPropertyGridVerb::IsFocused() const {
	return (m_nIndex == m_pVerbs->m_pGrid->m_nFocusedVerb) && (::GetFocus() == m_pVerbs->m_pGrid->GetSafeHwnd());
}
AFX_INLINE CXTPPropertyGridVerb* CXTPPropertyGridVerbs::GetAt(int nIndex) const {
	return m_arrVerbs.GetAt(nIndex);
}
AFX_INLINE BOOL CXTPPropertyGridVerbs::IsEmpty() const {
	return m_arrVerbs.GetSize() == 0;
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::FindItem(LPCTSTR strCaption) const {
	return GetGridView().m_pCategories->FindItem(strCaption);
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::FindItem(UINT nID) const {
	return GetGridView().m_pCategories->FindItem(nID);
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::AddCategory(LPCTSTR strCaption) {
	return GetGridView().AddCategory(strCaption);
}
AFX_INLINE CXTPPropertyGridItem* CXTPPropertyGrid::InsertCategory(int nIndex, LPCTSTR strCaption) {
	return GetGridView().InsertCategory(nIndex, strCaption);
}
AFX_INLINE void CXTPPropertyGrid::SetPropertySort (XTPPropertyGridSortOrder sort) {
	GetGridView().SetPropertySort(sort);
}
AFX_INLINE XTPPropertyGridSortOrder CXTPPropertyGrid::GetPropertySort () const {
	return GetGridView().m_properetySort;
}
AFX_INLINE BOOL CXTPPropertyGrid::IsHelpVisible() const {
	return m_bHelpVisible;
}
AFX_INLINE BOOL CXTPPropertyGrid::IsBarVisible() const {
	return m_bToolBarVisible;
}
AFX_INLINE int CXTPPropertyGrid::GetHelpHeight() const {
	return m_nHelpHeight;
}
AFX_INLINE COLORREF CXTPPropertyGrid::GetHelpBackColor() const {
	return m_pPaintManager->GetItemMetrics()->m_clrHelpBack;
}
AFX_INLINE COLORREF CXTPPropertyGrid::GetHelpForeColor() const {
	return m_pPaintManager->GetItemMetrics()->m_clrHelpFore;
}
AFX_INLINE CToolBar& CXTPPropertyGrid::GetToolBar() {
	return m_wndToolbar;
}
AFX_INLINE XTPPropertyGridPaintTheme CXTPPropertyGrid::GetCurrentTheme() const {
	return m_themeCurrent;
}
AFX_INLINE CXTPPropertyGridVerbs* CXTPPropertyGrid::GetVerbs() const {
	return m_pVerbs;
}
AFX_INLINE BOOL CXTPPropertyGrid::IsVerbsVisible() const {
	return !m_pVerbs->IsEmpty();
}
AFX_INLINE CXTPPropertyGridPaintManager* CXTPPropertyGrid::GetPaintManager() const {
	return m_pPaintManager;
}
AFX_INLINE BOOL CXTPPropertyGrid::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) {
	return CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
AFX_INLINE void CXTPPropertyGrid::NavigateItems(BOOL bTabItems) {
	m_bTabItems = bTabItems;
}
AFX_INLINE BOOL CXTPPropertyGrid::IsHighlightChangedItems() const {
	return m_bHighlightChanged;
}
AFX_INLINE void CXTPPropertyGrid::HighlightChangedItems(BOOL bHighlightChanged) {
	m_bHighlightChanged = bHighlightChanged;
	RedrawControl();
}
AFX_INLINE int CXTPPropertyGrid::GetCount() const {
	return GetGridView().GetCount();
}

AFX_INLINE void CXTPPropertyGrid::SetVariableSplitterPos(BOOL bVariable) {
	GetGridView().m_bVariableSplitterPos = bVariable;
}
AFX_INLINE BOOL CXTPPropertyGrid::GetVariableSplitterPos() const {
	return GetGridView().m_bVariableSplitterPos;
}
AFX_INLINE void CXTPPropertyGrid::SetVariableHelpHeight(BOOL bVariable) {
	m_bVariableHelpHeight = bVariable;
}
AFX_INLINE BOOL CXTPPropertyGrid::GetVariableHelpHeight() const {
	return m_bVariableHelpHeight;
}
AFX_INLINE BOOL CXTPPropertyGrid::GetShowInplaceButtonsAlways() const {
	return m_bShowInplaceButtonsAlways;
}
AFX_INLINE void CXTPPropertyGrid::SetShowInplaceButtonsAlways(BOOL bShow) {
	m_bShowInplaceButtonsAlways = bShow;
	Reposition();
}


#endif // #if !defined(__XTPPROPERTYGRID_H__)
