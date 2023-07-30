#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CAddStringDlg 대화 상자입니다.

class CAddStringDlg : public CDialog
{
	DECLARE_DYNAMIC(CAddStringDlg)

public:
	CAddStringDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CAddStringDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ADDSTRING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

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
