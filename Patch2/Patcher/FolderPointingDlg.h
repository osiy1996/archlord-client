#pragma once


// CFolderPointingDlg ��ȭ �����Դϴ�.

class CFolderPointingDlg : public CDialog
{
	DECLARE_DYNAMIC(CFolderPointingDlg)

public:
	CFolderPointingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFolderPointingDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FOLDER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()

public:
	CString m_strRealTimeFolder;

	afx_msg void OnBnClickedSearchrealtime();
};
