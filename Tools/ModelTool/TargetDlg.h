#pragma once

class CTargetDlg : public CDialog
{
	DECLARE_DYNAMIC(CTargetDlg)

public:
	CTargetDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CTargetDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_TARGET_OPTION };
	long	m_lTargetTID;
	long	m_lTargetNum;
	float	m_fOffsetX;
	float	m_fOffsetY;
	float	m_fOffsetZ;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	afx_msg void OnBnClickedOk();
	DECLARE_MESSAGE_MAP()
};
