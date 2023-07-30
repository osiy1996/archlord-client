#pragma once
#include "UIToolBaseObject.h"
#include "MyEdit.h"

class CUIToolEdit :	public CUIToolBaseObject
{
public:
	CUIToolEdit(void);
	~CUIToolEdit(void);

	virtual void InitProperty(void);
	virtual void RemoveProperties(void);
	virtual void FindPropertyValueChange(void);
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);
	virtual void Render(CBufferDC* pDC);

	virtual void SetFontColor(COLORREF p_color);
	virtual void SetImage(CString p_strimgpath);


public:
	CMyEdit*	m_Edit;
	
	LOGFONT		m_logfont;
	COLORREF	m_FontColor;

	int			m_iMaxLength;
	bool		m_bPassword;
	bool		m_bMultiLine;
	bool		m_bAutoLineFeed;
	bool		m_bCharFiltering;
	bool		m_bEndRelease;

	DWORD		m_style;

};
