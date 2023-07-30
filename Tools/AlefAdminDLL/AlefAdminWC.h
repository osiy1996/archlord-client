#pragma once
#include "afxwin.h"


// 2006.11.30. steeple
// �ش� ĳ������ ������� ������ ����


// AlefAdminWC �� ���Դϴ�.

class AlefAdminWC : public CFormView
{
	DECLARE_DYNCREATE(AlefAdminWC)

protected:
	AlefAdminWC();           // ���� ����⿡ ���Ǵ� protected �������Դϴ�.
	virtual ~AlefAdminWC();

public:
	enum { IDD = IDD_WC };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnInitialUpdate();

	void ClearContent();
	BOOL OnReceiveWCInfo(stAgpdAdminCharDataMoney* pstWantedCriminal);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnOK() {;}
	virtual void OnCancel() {;}

	DECLARE_MESSAGE_MAP()

public:
	CXTButton m_csRefreshBtn;
	CXTButton m_csDeleteBtn;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
public:
	afx_msg void OnBnClickedBWcRefresh();
public:
	afx_msg void OnBnClickedBWcDelete();
};


