#pragma once
#include "afxpropertygridctrl.h"

class CDerivedPropertyGridFontProperty : public CMFCPropertyGridFontProperty
{
public:
	CDerivedPropertyGridFontProperty(const CString& strName, LOGFONT& lf, DWORD dwFontDialogFlags, LPCTSTR lpszDescr, DWORD_PTR dwData, COLORREF color);
	~CDerivedPropertyGridFontProperty(void);

	void SetColor(COLORREF pColor)
	{
		m_Color = pColor;
	}
	void SetLogFont(LOGFONT pFont)
	{
		m_lf = pFont;
	}
};

