#pragma once


// CWorldMapCalcDlg ��ȭ �����Դϴ�.

class CWorldMapCalcDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldMapCalcDlg)

public:
	CWorldMapCalcDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CWorldMapCalcDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_WORLDCOODCALC };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
	
public:
	afx_msg void OnBnClickedOk();

public:
	int		nX1;
	int		nY1;
	int		nX2;
	int		nY2;
	int		nDivision1;
	int		nDivision2;

	FLOAT	m_fStartX;
	FLOAT	m_fStartZ;
	FLOAT	m_fEndX;
	FLOAT	m_fEndZ;
};
