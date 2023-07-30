#pragma once

#include "resource.h"
#include "afxwin.h"

// CFileSearch ��ȭ �����Դϴ�.

class CFileSearch : public CDialog
{
	DECLARE_DYNAMIC(CFileSearch)

public:
	CFileSearch(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFileSearch();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SEARCHDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSearch();
	CString m_strSearch;

	CEdit m_EditSearch;
	HTREEITEM m_SearchPosItem;
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnEnSetfocusEditSearch();
};
