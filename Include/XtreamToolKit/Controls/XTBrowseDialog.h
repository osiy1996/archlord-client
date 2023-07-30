// XTBrowseDialog.h: interface for the CXTBrowseDialog class.
//
// This file is a part of the XTREME CONTROLS MFC class library.
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
#if !defined(__XTBROWSEDIALOG_H__)
#define __XTBROWSEDIALOG_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ----------------------------------------------------------------------
// Summary:
//     CXTBrowseDialog is derived from the BROWSEINFO structure.
//     It is used to display a directory chooser dialog using shell
//     extensions.
// Remarks:
//     The CXTBrowseDialog class allows you to configure
//     various options, including setting the dialog's title or assigning
//     a callback function for defining the folder display and file
//     filtering styles.
// ----------------------------------------------------------------------
class _XTP_EXT_CLASS CXTBrowseDialog : protected BROWSEINFO
{
public:

	// ----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTBrowseDialog object
	// Parameters:
	//     pParent -  [in] Points to a CWnd object that represents the parent
	//                window for the browse dialog.
	// ----------------------------------------------------------------------
	CXTBrowseDialog(CWnd* pParent = NULL);

	// -----------------------------------------------------------------
	// Summary:
	//     Destroys a CXTBrowseDialog object, handles cleanup and
	//     deallocation
	// -----------------------------------------------------------------
	virtual ~CXTBrowseDialog();

public:

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to invoke the browse dialog box and to
	//     return the dialog box result when done.
	// Returns:
	//     IDOK if the OK button was pressed, otherwise returns IDCANCEL.
	// --------------------------------------------------------------------
	INT_PTR DoModal();

	// -------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the owner window for the dialog box.
	// Parameters:
	//     hWnd -  [in] Handle to the owner window for the dialog box.
	// -------------------------------------------------------------------------
	void SetOwner(HWND hWnd);

	// ----------------------------------------------------------------
	// Summary:
	//     Call this member function to get an HWND handle to the owner
	//     window for the dialog box.
	// Returns:
	//     An HWND handle.
	// ----------------------------------------------------------------
	HWND GetOwner();

	// ----------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the address of an ITEMIDLIST
	//     structure which specifies the location of the root folder to
	//     browse from.
	// Parameters:
	//     pidl -  [in] Address of an ITEMIDLIST structure specifying the location
	//             of the root folder to browse from. Only the specified folder and
	//             its subfolders appear in the dialog box. This member can be
	//             NULL, in which case, the namespace root (the desktop folder) is
	//             used.
	// ----------------------------------------------------------------------------
	void SetPidlRoot(LPCITEMIDLIST pidl);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the address of the ITEMIDLIST
	//     structure that was specified for the location of the root folder.
	// Returns:
	//     The address of the ITEMIDLIST structure that was specified for the
	//     location of the root folder.
	//-----------------------------------------------------------------------
	LPCITEMIDLIST GetPidlRoot();

	// --------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the address for the display name
	//     the dialog box will use.
	// Parameters:
	//     szDisplayName -  [in] Address of a buffer to receive the display name of the
	//                      folder selected by the user. The size of this buffer is
	//                      assumed to be MAX_PATH bytes.
	// --------------------------------------------------------------------------------
	void SetDisplayName(LPCTSTR szDisplayName);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the display name that is used
	//     by the browse dialog box.
	// Returns:
	//     The display name that is used by the browse dialog box.
	//-----------------------------------------------------------------------
	LPCTSTR GetDisplayName();

	// --------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the title for the browse dialog
	//     box.
	// Parameters:
	//     szTitle -  [in] Address of a null\-terminated string that is displayed above
	//                the tree view control in the dialog box. This string can be
	//                used to specify instructions to the user.
	// --------------------------------------------------------------------------------
	void SetTitle(LPCTSTR szTitle);

	// ---------------------------------------------------------------------
	// Summary:
	//     Call this member function to return a NULL terminated string that
	//     represents the title that was set for the dialog box.
	// Returns:
	//     A NULL terminated string that represents the title that was set
	//     for the dialog box.
	// ---------------------------------------------------------------------
	LPCTSTR GetTitle();

	// ---------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the flags for specifying the
	//     \options for the browse dialog box.
	// Parameters:
	//     uf -  [in] Flags specifying the options for the dialog box. See the
	//           \Remarks section below for a list of available styles.
	// Remarks:
	//     Styles to be added or removed can be combined by using the bitwise
	//     OR (|) operator. It can be one or more of the following:
	//     * <b>BIF_BROWSEFORCOMPUTER</b> Only return computers. If the user
	//          selects anything other than a computer, the OK button is grayed.
	//     * <b>BIF_BROWSEFORPRINTER</b> Only allow the selection of printers.
	//          If the user selects anything other than a printer, the OK button
	//          is grayed. In Microsoft Windows XP, the best practice is to
	//          use an XP-style dialog, setting the root of the dialog to the
	//          Printers and Faxes folder (CSIDL_PRINTERS).
	//     * <b>BIF_BROWSEINCLUDEFILES</b> Version 4.71. The browse dialog
	//          box will display files as well as folders.
	//     * <b>BIF_BROWSEINCLUDEURLS</b> Version 5.0. The browse dialog box
	//          can display URLs. The BIF_USENEWUI and BIF_BROWSEINCLUDEFILES
	//          flags must also be set. If these three flags are not set, the
	//          browser dialog box will reject URLs. Even when these flags
	//          are set, the browse dialog box will only display URLs if the
	//          folder that contains the selected item supports them. When
	//          the folder's IShellFolder::GetAttributesOf method is called
	//          to request the selected item's attributes, the folder must
	//          set the SFGAO_FOLDER attribute flag. Otherwise, the browse
	//          dialog box will not display the URL.
	//     * <b>BIF_DONTGOBELOWDOMAIN</b> Do not include network folders below
	//          the domain level in the dialog box's tree view control.
	//     * <b>BIF_EDITBOX</b> Version 4.71. Include an edit control in the
	//          browse dialog box that allows the user to type the name of an item.
	//     * <b>BIF_NEWDIALOGSTYLE</b> Version 5.0. Use the new user interface.
	//          Setting this flag provides the user with a larger dialog box
	//          that can be resized. The dialog box has several new capabilities
	//          including: drag-and-drop capability within the dialog box,
	//          reordering, shortcut menus, new folders, delete, and other
	//          shortcut menu commands. To use this flag, you must call OleInitialize
	//          or CoInitialize before calling SHBrowseForFolder.
	//     * <b>BIF_NONEWFOLDERBUTTON</b> Version 6.0. Do not include the New
	//          Folder button in the browse dialog box.
	//     * <b>BIF_NOTRANSLATETARGETS</b> Version 6.0. When the selected item
	//          is a shortcut, return the PIDL of the shortcut itself rather
	//          than its target.
	//     * <b>BIF_RETURNFSANCESTORS</b> Only return file system ancestors.
	//          An ancestor is a subfolder that is beneath the root folder
	//          in the namespace hierarchy. If the user selects an ancestor
	//          of the root folder that is not part of the file system, the
	//          OK button is grayed.
	//     * <b>BIF_RETURNONLYFSDIRS</b> Only return file system directories.
	//          If the user selects folders that are not part of the file system,
	//          the OK button is grayed.
	//     * <b>BIF_SHAREABLE</b> Version 5.0. The browse dialog box can display
	//          shareable resources on remote systems. It is intended for applications
	//          that want to expose remote shares on a local system. The BIF_NEWDIALOGSTYLE
	//          flag must also be set.
	//     * <b>BIF_STATUSTEXT</b> Include a status area in the dialog box.
	//          The callback function can set the status text by sending messages
	//          to the dialog box. This flag is not supported when BIF_NEWDIALOGSTYLE
	//          is specified.
	//     * <b>BIF_UAHINT</b> Version 6.0. When combined with BIF_NEWDIALOGSTYLE,
	//          adds a usage hint to the dialog box in place of the edit box.
	//          BIF_EDITBOX overrides this flag.
	//     * <b>BIF_USENEWUI</b> Version 5.0. Use the new user interface, including
	//          an edit box. This flag is equivalent to BIF_EDITBOX | BIF_NEWDIALOGSTYLE.
	//          To use BIF_USENEWUI, you must call OleInitialize or CoInitialize
	//          before calling SHBrowseForFolder.
	//     * <b>BIF_VALIDATE</b> Version 4.71. If the user types an invalid
	//          name into the edit box, the browse dialog box will call the
	//          application's BrowseCallbackProc with the BFFM_VALIDATEFAILED
	//          message. This flag is ignored if BIF_EDITBOX is not specified.
	// ---------------------------------------------------------------------------
	void SetOptions(UINT uf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the flags specifying the options
	//     that have been set for the dialog box.
	// Returns:
	//     The flags specifying the options that have been set for the dialog box.
	//-----------------------------------------------------------------------
	UINT GetOptions();

	// --------------------------------------------------------------------------
	// Summary:
	//     Call this member function to define the address for the
	//     BrowseCallbackProc function that is to be called when an event
	//     \occurs.
	// Parameters:
	//     pf -  [in] Address of an application\-defined function that the dialog
	//           box calls when an event occurs. For more information, see the
	//           BrowseCallbackProc function. This member can be NULL.
	// --------------------------------------------------------------------------
	void SetCallback(BFFCALLBACK pf);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to return the address for the BrowseCallbackProc
	//     function that is called when an event occurs.
	// Returns:
	//     The address for the BrowseCallbackProc function that is called
	//     when an event occurs.
	//-----------------------------------------------------------------------
	BFFCALLBACK GetCallback();

	// -----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the application data that is
	//     passed to the callback function.
	// Parameters:
	//     lp -  [in] Application\-defined value that the dialog box passes to
	//           the callback function, if one is specified.
	// -----------------------------------------------------------------------
	void SetData(LPARAM lp);

	// ----------------------------------------------------------------------------
	// Summary:
	//     Returns application data that was set to be passed to the
	//     callback function.
	// Returns:
	//     The application data that was set to be passed to the callback function,
	//     if one is specified.
	// ----------------------------------------------------------------------------
	LPARAM GetData();

	// ---------------------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the initial path to select when
	//     the browse dialog is first opened.
	// Parameters:
	//     szSelPath -  [in] A NULL terminated string that represents the directory that
	//                  is selected when the dialog is initially opened. If not set,
	//                  GetCurrentDirectory is called to set the directory.
	// ---------------------------------------------------------------------------------
	void SetSelPath(LPCTSTR szSelPath);

	// --------------------------------------------------------------------
	// Summary:
	//     Call this member function to get a NULL terminated string that
	//     represents the currently selected directory.
	// Returns:
	//     A NULL terminated string that represents the selected directory.
	// --------------------------------------------------------------------
	LPCTSTR GetSelPath();

	// ------------------------------------------------------------------
	// Summary:
	//     Call this member function to get the index to the system image
	//     list of the image associated with the selected folder.
	// Returns:
	//     The index to the system image list.
	// ------------------------------------------------------------------
	int GetImage();


	//-----------------------------------------------------------------------
	// Summary:
	//     Application defined callback.
	// Parameters:
	//     hwnd   - A handle to a window.
	//     uMsg   - The message that is sent to the window.
	//     lParam - Specifies the application-defined data passed by the
	//              BrowseCtrlCallback function.
	//     lpData - Data that is passed into the function.
	// Remarks:
	//     Application defined callback function used with the SHBrowseForFolder
	//     function. The browse dialog box calls this function to notify
	//     it about events. You can define your own callback function by
	//     using the SetCallback method.
	// Returns:
	//     An integer value.
	//-----------------------------------------------------------------------
	static int CALLBACK BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);


protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     NULL terminated string that represents the selected directory
	//-----------------------------------------------------------------------
	TCHAR m_szSelPath[MAX_PATH];

private:

	CString m_strTitle; // default dialog title.

};

/////////////////////////////////////////////////////////////////////////////

AFX_INLINE void CXTBrowseDialog::SetOwner(HWND hWnd) {
	hwndOwner = hWnd;
}
AFX_INLINE HWND CXTBrowseDialog::GetOwner() {
	return hwndOwner;
}
AFX_INLINE void CXTBrowseDialog::SetPidlRoot(LPCITEMIDLIST pidl) {
	pidlRoot = pidl;
}
AFX_INLINE LPCITEMIDLIST CXTBrowseDialog::GetPidlRoot() {
	return pidlRoot;
}
AFX_INLINE void CXTBrowseDialog::SetDisplayName(LPCTSTR szDisplayName) {
	pszDisplayName = (LPTSTR)szDisplayName;
}
AFX_INLINE LPCTSTR CXTBrowseDialog::GetDisplayName() {
	return pszDisplayName;
}
AFX_INLINE void CXTBrowseDialog::SetTitle(LPCTSTR szTitle) {
	lpszTitle = szTitle;
}
AFX_INLINE LPCTSTR CXTBrowseDialog::GetTitle() {
	return lpszTitle;
}
AFX_INLINE void CXTBrowseDialog::SetOptions(UINT uf) {
	ulFlags = uf;
}
AFX_INLINE UINT CXTBrowseDialog::GetOptions() {
	return ulFlags;
}
AFX_INLINE void CXTBrowseDialog::SetCallback(BFFCALLBACK pf) {
	lpfn = pf;
}
AFX_INLINE BFFCALLBACK CXTBrowseDialog::GetCallback() {
	return lpfn;
}
AFX_INLINE void CXTBrowseDialog::SetData(LPARAM lp) {
	lParam = lp;
}
AFX_INLINE LPARAM CXTBrowseDialog::GetData() {
	return lParam;
}
AFX_INLINE LPCTSTR CXTBrowseDialog::GetSelPath() {
	return m_szSelPath;
}
AFX_INLINE int CXTBrowseDialog::GetImage() {
	return iImage;
}

//{{AFX_CODEJOCK_PRIVATE
#ifndef BIF_BROWSEINCLUDEURLS
#define BIF_BROWSEINCLUDEURLS  0x0080   // Allow URLs to be displayed or entered. (Requires BIF_USENEWUI)
#endif

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE     0x0040   // Use the new dialog layout with the ability to resize
#endif

#ifndef BIF_NONEWFOLDERBUTTON
#define BIF_NONEWFOLDERBUTTON  0x0200   // Do not add the "New Folder" button to the dialog.  Only applicable with BIF_NEWDIALOGSTYLE.
#endif

#ifndef BIF_NOTRANSLATETARGETS
#define BIF_NOTRANSLATETARGETS 0x0400   // don't traverse target as shortcut
#endif

#ifndef BIF_SHAREABLE
#define BIF_SHAREABLE          0x8000  // sharable resources displayed (remote shares, requires BIF_USENEWUI)
#endif

#ifndef BIF_UAHINT
#define BIF_UAHINT             0x0100   // Add a UA hint to the dialog, in place of the edit box. May not be combined with BIF_EDITBOX
#endif

#ifndef BIF_USENEWUI
#define BIF_USENEWUI           (BIF_NEWDIALOGSTYLE | BIF_EDITBOX)
#endif
//}}AFX_CODEJOCK_PRIVATE

#endif // !defined(__XTBROWSEDIALOG_H__)
