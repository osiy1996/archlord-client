#pragma once


// CFindObjectSettingDlg ��ȭ �����Դϴ�.

class CFindObjectSettingDlg : public CDialog
{
	DECLARE_DYNAMIC(CFindObjectSettingDlg)

public:
	CFindObjectSettingDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CFindObjectSettingDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_FINDOBJECT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_nEventID;
};
