#pragma once
#include "afxwin.h"


// CLightOptionDlg ��ȭ �����Դϴ�.

class CLightOptionDlg : public CDialog
{
	DECLARE_DYNAMIC(CLightOptionDlg)

public:
	CLightOptionDlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLightOptionDlg();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_LIGHT_OPTION };
	CString		m_strType;
	float		m_fRadius;
	float		m_fAngle;
	CComboBox	m_cbLightList;

	BYTE		m_byRed;
	BYTE		m_byGreen;
	BYTE		m_byBlue;
	COLORREF	m_cColor;

	void		UpdateControl();
	void		DrawLight();

protected:
	CRpLight*	GetCurrLight();
	void		SetControl( CRpLight* pLight );
	void		GetControl( CRpLight* pLight );

	void		DrawDirect( CRpLight* pLight );
	void		DrawCone( CRpLight* pLight, RwReal fAngle, RwReal fSize, RwReal fRatio );
	void		DrawPoint( CRpLight* pLight );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedLightColorOpen();
	afx_msg void OnCbnSelchangeLightList();

	DECLARE_MESSAGE_MAP()
};
