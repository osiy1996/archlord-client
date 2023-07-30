#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "afxwin.h"


// AlefAdminOption ��ȭ �����Դϴ�.

class AlefAdminOption : public CDialog
{
	DECLARE_DYNAMIC(AlefAdminOption)

public:
	AlefAdminOption(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~AlefAdminOption();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_OPTION };

public:
	BOOL m_bUseAlpha, m_bUseAlpha2;
	INT32 m_lAlpha, m_lAlpha2;

	BOOL m_bSaveWhisper;

public:
	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CSliderCtrl m_csAlphaSlider;
	CButton m_csAlphaBtn;

	afx_msg void OnBnClickedChUseLayered();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	CButton m_csSaveWhisperCheckBox;
};
