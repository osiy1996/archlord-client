// ShopClientDlg.h : 헤더 파일
//

#pragma once

#include "ShopEH_AR.h"

// CShopClientDlg 대화 상자
class CShopClientDlg : public CDialog
{
// 생성입니다.
public:
	CShopClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
	enum { IDD = IDD_SHOPCLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	//네트웍 라이브러리 콜백함수
	static void ErrorHandler( DWORD lastError, TCHAR* desc );	

public: 
	CShopEH_AR m_Client;
// 구현입니다.
protected:
	HICON m_hIcon;
	
	// 생성된 메시지 맵 함수
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
