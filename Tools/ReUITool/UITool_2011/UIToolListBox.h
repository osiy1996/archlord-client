#pragma once
#include "uitoolbaseobject.h"
#include "MyListBox.h"

class CUIToolListBox : public CUIToolBaseObject
{
public:
	CUIToolListBox(void);
	~CUIToolListBox(void);


	virtual void Render(CBufferDC* pDC);
	virtual void InitProperty(void);
	virtual	void RemoveProperties(void);
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);

	virtual void SetFontColor(COLORREF p_color);
	virtual void SetImage(CString p_strimgpath);


	CMyListBox*		m_ListBox;
	UINT			m_iRow;
	UINT			m_iColumn;


};
