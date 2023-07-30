#pragma once
#include "UIToolBaseObject.h"
#include "ColorStatic.h"
#include "MyStatic.h"

class CUIToolStatic: public CUIToolBaseObject
{
public:
	CUIToolStatic(void);
	~CUIToolStatic(void);

	virtual void InitProperty(void);
	virtual void RemoveProperties(void);
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);
	virtual void Render(CBufferDC* pDC);

	virtual void SetFontColor(COLORREF p_color);
	virtual void SetImage(CString p_strimgpath);

protected:

	CMyStatic*		m_Static;

	CString			m_strRowArray;
	CString			m_strColumnArray;

	bool			m_bNumberComma;
	bool			m_bShadow;
	COLORREF		m_BKFontColor;
	bool			m_bImgNumber;
	CString			m_ImgFontTexture;
	
};
