#pragma once
#include "afxwin.h"
#include "UIToolBaseObject.h"
#include "MyButton.h"

// CUIToolButton

class CUIToolButton : public CUIToolBaseObject
{
public:
	CUIToolButton();
	virtual ~CUIToolButton();

	virtual void InitProperty(void);
	virtual void RemoveProperties(void);
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);
	virtual void Render(CBufferDC* pDC);

	virtual void SetFontColor(COLORREF p_color);
	


public:
	CMyButton*	m_Button;
	bool	m_bStartClicked;
	bool	m_bPushButton;

};


