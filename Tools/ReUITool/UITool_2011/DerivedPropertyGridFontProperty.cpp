#include "stdafx.h"
#include "DerivedPropertyGridFontProperty.h"

CDerivedPropertyGridFontProperty::CDerivedPropertyGridFontProperty(	const CString& strName, LOGFONT& lf, DWORD dwFontDialogFlags = CF_EFFECTS | CF_SCREENFONTS, LPCTSTR lpszDescr = NULL, DWORD_PTR dwData = 0, COLORREF color = (COLORREF)-1)
:CMFCPropertyGridFontProperty(strName, lf, dwFontDialogFlags, lpszDescr, dwData, color)														
{

}
CDerivedPropertyGridFontProperty::~CDerivedPropertyGridFontProperty(void)
{

}