#pragma once


// CDlgAgreement ��ȭ �����Դϴ�.

class CDlgAgreement : public CDialog
{
	DECLARE_DYNAMIC(CDlgAgreement)

private :
	BOOL									m_bIsChecked;

public:
	CDlgAgreement(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDlgAgreement();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_DLG_PVP_AGREEMENT };

public :
	virtual BOOL OnInitDialog( void );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAgree();
public:
	afx_msg void OnBnClickedCheckAgree();

public :
	BOOL IsChecked( void ) { return m_bIsChecked; }
};
