#pragma once
#include "afxwin.h"


// CItemSelectDlg ��ȭ �����Դϴ�.

class CItemSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CItemSelectDlg)

public:
	CItemSelectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CItemSelectDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ITEMSELECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnDblclkList();
	afx_msg void OnBnClickedNoneeditem();

public:
	CListBox	m_ctlList;
	int			m_nItemID;
	
	
};
