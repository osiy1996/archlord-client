#pragma once


// CRoughDisplayDlg ��ȭ �����Դϴ�.

class CRoughDisplayDlg : public CDialog
{
	DECLARE_DYNAMIC(CRoughDisplayDlg)

public:
	CRoughDisplayDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CRoughDisplayDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_ROUGHLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedShow();
	afx_msg void OnBnClickedHide();
	float m_fSrcX;
	float m_fSrcZ;
	float m_fSrcHeight;
	UINT m_uDiv;
	int m_nRadius;
	float m_fCloudHeight;
	afx_msg void OnBnClickedSetDefault();
	float m_fDstX;
	float m_fDstZ;
};
