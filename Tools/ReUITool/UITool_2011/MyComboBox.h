#pragma once


// CMyComboBox

class CMyComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CMyComboBox)

public:
	CMyComboBox();
	virtual ~CMyComboBox();

	COLORREF	m_EditfontColor;
	CBrush		m_BkBrush;
	CString		m_ButtonImagePath;
	CString		m_BGImagePath;

	void SetImagePath(CString p_path){ m_ButtonImagePath = p_path; }
	void SetBGImagePath(CString p_path){ m_BGImagePath = p_path; }
	void SetEditTextColor(COLORREF p_color){ m_EditfontColor = p_color; }
	
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};


