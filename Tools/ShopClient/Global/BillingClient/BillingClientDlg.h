// BillingClientDlg.h : ��� ����
//

#pragma once

#include "LibClientSession.h"

// CBillingClientDlg ��ȭ ����
class CBillingClientDlg : public CDialog
{
// �����Դϴ�.
public:
	CBillingClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_BILLINGCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.

	//��Ʈ�� ���̺귯�� �ݹ��Լ�
	static void ErrorHandler( DWORD lastError, TCHAR* desc );	
private:
	CLibClientSession m_Client;

// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButConnect();
	afx_msg void OnBnClickedButLogin();
	afx_msg void OnBnClickedButLogout();
	afx_msg void OnBnClickedButInquire();	
	afx_msg void OnBnClickedButInquireMulti();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButInquirePcpoint();
	//afx_msg void OnBnClickedButInquireDeducttype();
public:
	afx_msg void OnBnClickedButton2();
};
