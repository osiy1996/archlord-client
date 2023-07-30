#pragma once

#include "resource.h"
#include "afxwin.h"

// CFileSearch 대화 상자입니다.

class CFileSearch : public CDialog
{
	DECLARE_DYNAMIC(CFileSearch)

public:
	CFileSearch(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CFileSearch();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SEARCHDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
