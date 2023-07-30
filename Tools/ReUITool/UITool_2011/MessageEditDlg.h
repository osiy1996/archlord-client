#pragma once
#include "afxwin.h"

#include "resource.h"
#include "afxcmn.h"

// CMessageEditDlg ��ȭ �����Դϴ�.

class CMessageEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CMessageEditDlg)

public:
	CMessageEditDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMessageEditDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MESSAGEEDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

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
