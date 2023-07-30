#pragma once
#include "afxwin.h"
#include "afxdtctl.h"


// CFullScanOptionDlg ��ȭ �����Դϴ�.

class CFullScanOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CFullScanOptionDlg)

public:
	CFullScanOptionDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFullScanOptionDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FULLSCAN_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg	VOID	OnBnClickedOk();

	INT				GetType			( VOID )		{	return m_nSelType;	}

	CStatic				m_staticLastPatch;
	CMonthCalCtrl		m_CalendarMin;
	CMonthCalCtrl		m_CalendarMax;

	INT					m_nSelType;

	INT					m_nMaxYear;
	INT					m_nMaxMonth;
	INT					m_nMaxDay;

	INT					m_nMinYear;
	INT					m_nMinMonth;
	INT					m_nMinDay;

public:
	afx_msg void OnBnClickedRadioFullscan1();
public:
	afx_msg void OnBnClickedRadioFullscan2();
public:
	afx_msg void OnBnClickedRadioFullscan3();
	CString m_strLastPatch;
};
