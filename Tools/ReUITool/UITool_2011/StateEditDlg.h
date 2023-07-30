#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "UIToolBaseObject.h"

// CStateEditDlg 대화 상자입니다.

class CStateEditDlg : public CDialog
{
	DECLARE_DYNAMIC(CStateEditDlg)

public:
	CStateEditDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CStateEditDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_STATEEDITDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAdd();
	afx_msg void OnBnClickedDelete();
	
	afx_msg void OnBnClickedButtonTexture();
	afx_msg void OnBnClickedColorload();
	afx_msg void OnBnClickedCheckVisible();

public:
	CString m_strID;
	CString m_ColorR;
	CString m_ColorG;
	CString m_ColorB;
	CString m_strTexturePath;
	CString m_strPosX;
	CString m_strPosY;	
	CString m_strSizeX;
	CString m_strSizeY;

	CEdit m_Edit_ID;	
	CEdit m_Edit_PosX;
	CEdit m_EditColorB;
	CEdit m_EditColorG;
	CEdit m_EditColorR;
	CEdit m_EditTexturePath;
	CEdit m_Edit_PosY;
	CEdit m_Edit_SizeX;	
	CEdit m_Edit_SizeY;
	
	CListCtrl m_ListCtrl;

	CButton m_CheckBut_visivle;

	ObjectStatus* m_JustCopyObj;
	
protected:
	virtual void OnCancel();
};
