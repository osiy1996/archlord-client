#pragma once


// CWorldPosExportDlg 대화 상자입니다.

class CWorldPosExportDlg : public CDialog
{
	DECLARE_DYNAMIC(CWorldPosExportDlg)

public:
	CWorldPosExportDlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CWorldPosExportDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_WORLDCOODCALCEX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedExportbtn();

private:
	int m_nDivision1;
	int m_nDivision2;
	int m_nX1;
	int m_nY1;
	int m_nX2;
	int m_nY2;

	FLOAT m_nDisX1;
	FLOAT m_nDisY1;
	FLOAT m_nDisX2;
	FLOAT m_nDisY2;
};
