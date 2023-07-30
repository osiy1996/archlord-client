#pragma once


// CColorStatic

class CColorStatic : public CStatic
{
	DECLARE_DYNAMIC(CColorStatic)

public:
	CColorStatic();  

	virtual ~CColorStatic();

	COLORREF    m_RGB;
	COLORREF    m_bkRGB;

	LOGFONT*    m_LogFont;

protected:
	DECLARE_MESSAGE_MAP()
public:

	void SetColor( COLORREF p_rgb , COLORREF p_bkrgb = RGB(255,255,255));
	//void SetFont(LOGFONT* pFont);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
};


