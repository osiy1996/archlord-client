// ShopClientDlg.h : ��� ����
//

#pragma once

#include "ShopEH_AR.h"

// CShopClientDlg ��ȭ ����
class CShopClientDlg : public CDialog
{
// �����Դϴ�.
public:
	CShopClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_SHOPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


	//��Ʈ�� ���̺귯�� �ݹ��Լ�
	static void ErrorHandler( DWORD lastError, TCHAR* desc );	

public: 
	CShopEH_AR m_Client;
// �����Դϴ�.
protected:
	HICON m_hIcon;
	
	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButConnect();
	afx_msg void OnBnClickedButShopInquire();
	afx_msg void OnBnClickedButShopbuy();
	afx_msg void OnBnClickedButGift();
	afx_msg void OnBnClickedCancel();

// AR_TW Start
	afx_msg void OnBnClickedButShopInquireTW();
	afx_msg void OnBnClickedButShopbuyTW();
// AR_TW End
};
