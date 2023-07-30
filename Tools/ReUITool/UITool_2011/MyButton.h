#pragma once
//#include <atlctrlx.h>

// CMyButton

class CMyButton : public CMFCButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	CMyButton();
	virtual ~CMyButton();

	void SetbgColor_static(COLORREF p_color){ m_bgColor = p_color; }
	//void SetTextColor_static(COLORREF p_color){ m_TextColor = p_color; }
	//void SetImagePath(CString p_path){ m_bgImagePath = p_path; }
	void SetLogFont(LOGFONT* p_logfont){ m_LogFont = p_logfont; }

	void SetBitmaps(CString pImagePath, CString pstr, CPoint pSize);

protected:
	DECLARE_MESSAGE_MAP()
	
	//COLORREF	m_TextColor;
	COLORREF	m_bgColor;
	//CString		m_bgImagePath;
	LOGFONT*    m_LogFont;

	CRect rt;
	CDC*		m_dc;
 	CImage m_bitmapDefault;
	CImage m_bitmapMouseOn;
 	CImage m_bitmapClick;
 	CImage m_bitmapDisable;

public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);	
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT /*nCtlColor*/);
};


