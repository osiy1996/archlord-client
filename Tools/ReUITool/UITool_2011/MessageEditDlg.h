#pragma once
#include "afxwin.h"

#include "resource.h"
#include "afxcmn.h"

// CMessageEditDlg 대화 상자입니다.

class CMessageEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageEditDlg)

public:
	CMessageEditDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CMessageEditDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_MESSAGEEDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:

	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	virtual BOOL OnInitDialog();

	CComboBox m_Combo_Message;
	CComboBox m_Combo_Action;
	CString m_strAction;
	CString m_strMessage;
	CTreeCtrl m_Tree_Message;
	
	HTREEITEM AllSearchItem(const CString& str, HTREEITEM hItem, BOOL bSearchDown);
};
