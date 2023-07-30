#pragma once
#include "UIToolBaseObject.h"

enum E_UITYPE
{
	EU_Normal = 0,
	EU_Popup_v,
	EU_Popup_H,
};
class CUIToolUI: public CUIToolBaseObject
{
public:
	CUIToolUI(void);
	~CUIToolUI(void);

	virtual void InitProperty(void);
	virtual	void RemoveProperties(void);
	virtual void FindPropertyValueChange();
	virtual void PropertyChange(CString pStr, CMFCPropertyGridProperty* pProp);

public:
	CString		m_strType;
	bool		m_bLogIn;
	bool		m_bMainUI;
	bool		m_bAutoClose;
	bool		m_bTransperent;
	bool		m_bResoultion_size;
	bool		m_bResoultion_Move;
	bool		m_bInitStatus;
};
