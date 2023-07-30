#pragma once
#include "UIToolBaseObject.h"
#include "UIToolButton.h"
#include "UIToolStatic.h"
#include "MyComboBox.h"

class CUIToolComboBox :	public CUIToolBaseObject
{
public:
	CUIToolComboBox(void);
	~CUIToolComboBox(void);

	virtual void Render(CBufferDC* pDC);
	virtual void InitProperty(void);
	virtual	void RemoveProperties(void);
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);

	virtual void SetFontColor(COLORREF p_color);
	virtual void SetImage(CString p_strimgpath);

	

public:
	CMyComboBox*				m_ComboBox;

	long						m_RowHeigth;
	UINT						m_Row;

	CString						m_BGImagePath;
};
