#pragma once


// CTeleportDlg ��ȭ �����Դϴ�.

class CTeleportDlg : public CDialog
{
	DECLARE_DYNAMIC(CTeleportDlg)

public:
	CTeleportDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTeleportDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_GOTO_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strMessage;
	BOOL m_bShow;
	afx_msg void OnBnClickedShowthisboxagain();
};
