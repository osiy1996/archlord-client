#pragma once

#include "ControlMenuDlg.h"
// CControlMenuView

class CControlMenuView : public CDockablePane
{
	DECLARE_DYNAMIC(CControlMenuView)

public:
	CControlMenuView();
	virtual ~CControlMenuView();

	void AdjustLayout();

protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);

public:
	CControlMenuDlg m_CtrlMenu;
};


