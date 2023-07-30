#pragma once

#include "resource.h"
#include "afxwin.h"

#include "UIToolUI.h"
#include "UIToolStatic.h"
#include "UIToolEdit.h"
#include "UIToolButton.h"
#include "UIToolComboBox.h"
#include "UIToolListBox.h"

#ifdef _WIN32_WCE
#error "Windows CE에서는 CDHtmlDialog가 지원되지 않습니다."
#endif 

enum E_ObjClass
{
	E_UI,
	E_BUTTON,
	E_STATIC,
	E_EDIT,
	E_SCROLLBAR,
	E_COMBOBOX,
	E_LISTBOX,
	E_TREE,
	E_GRID,
};

// CControlMenuDlg 대화 상자입니다.

class CControlMenuDlg : public CDialog
{
	DECLARE_DYNAMIC(CControlMenuDlg)

public:
	CControlMenuDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CControlMenuDlg();

	void SetCreate(CPoint pStart, CPoint pEnd);

// 대화 상자 데이터입니다.
	enum { IDD = IDD_CONTROLMENUDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

	

public:
	afx_msg void OnBnClickedUi();
	afx_msg void OnBnClickedCtrldelete();
	afx_msg void OnBnClickedEdit();
	afx_msg void OnBnClickedButton();
	afx_msg void OnBnClickedStatic();
	afx_msg void OnBnClickedStaticedit();
	afx_msg void OnBnClickedCombobox();
	
	
	E_ObjClass m_CurSelectObj;
	afx_msg void OnBnClickedListbox();
};
