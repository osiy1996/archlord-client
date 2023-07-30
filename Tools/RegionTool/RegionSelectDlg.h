#pragma once
#include "afxcmn.h"
#include "RTMenuRegion.h"


// CRegionSelectDlg ��ȭ �����Դϴ�.

class CRegionSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CRegionSelectDlg)

public:
	CRegionSelectDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRegionSelectDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_REGIONSELECT_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

protected:
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_ctrRegionSelectList;
	int		  m_nSelectedItem;
	CString	  m_strSelectParentIndex;

public:
	BOOL UpdateList();

public:
	afx_msg void OnBnClickedOk();

public:
	afx_msg void OnLvnItemchangedRegionselectList(NMHDR *pNMHDR, LRESULT *pResult);
};
