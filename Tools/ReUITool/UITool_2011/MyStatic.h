#pragma once


// CMyStatic

class CMyStatic : public CStatic
{
	DECLARE_DYNAMIC(CMyStatic)

public:
	CMyStatic();
	virtual ~CMyStatic();

	void SetbgColor_static(COLORREF p_color){ m_bgColor = p_color; }
	void SetTextColor_static(COLORREF p_color){ m_TextColor = p_color; }
	void SetImagePath(CString p_path){ m_bgImagePath = p_path; }
	void SetLogFont(LOGFONT* p_logfont){ m_LogFont = p_logfont; }

protected:
	DECLARE_MESSAGE_MAP()

	COLORREF	m_TextColor;
	COLORREF	m_bgColor;
	CString		m_bgImagePath;
	LOGFONT*    m_LogFont;
	
public:
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
};


