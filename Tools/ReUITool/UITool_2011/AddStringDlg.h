#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAddStringDlg ��ȭ �����Դϴ�.

class CAddStringDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddStringDlg)

public:
	CAddStringDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAddStringDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ADDSTRING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_AddStringEdit;
	CString m_strAddSting;
	CListCtrl m_AddList;
	
	afx_msg void OnBnClickedButtonAdd();
	afx_msg void OnBnClickedButtonDelete();
protected:
	virtual void OnCancel();
};
