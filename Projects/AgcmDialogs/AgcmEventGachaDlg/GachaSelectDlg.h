#pragma once

#include <map>
#include "afxwin.h"

using namespace std;

class	AgcmEventGachaDlg;

// CGachaSelectDlg ��ȭ �����Դϴ�.

class CGachaSelectDlg : public CDialog
{
	DECLARE_DYNAMIC(CGachaSelectDlg)

public:
	AgcmEventGachaDlg	* m_pParentModule;
	CGachaSelectDlg( AgcmEventGachaDlg * pParent , INT32 nIndex = 0 );   // ǥ�� �������Դϴ�.
	virtual ~CGachaSelectDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GACHA };

	INT32	m_nIndex;
	map< INT32	, AgpdGachaType	>	* m_pMap;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_listGacha;
	CString m_strSelectedGacha;
public:
	afx_msg void OnLbnSelchangeListGacha();
};
