#pragma once
#include "afxwin.h"


// CSelectSkySetDlg ��ȭ �����Դϴ�.

class CSelectSkySetDlg : public CDialog
{
	DECLARE_DYNCREATE(CSelectSkySetDlg)

public:
	CSelectSkySetDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSelectSkySetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SELECTSKYSET };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnLbnDblclkList();

public:
	int			m_nItemID;
	CListBox	m_ctlList;
};
