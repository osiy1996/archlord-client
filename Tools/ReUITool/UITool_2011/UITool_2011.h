
// UITool_2011.h : UITool_2011 ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CUITool_2011App:
// �� Ŭ������ ������ ���ؼ��� UITool_2011.cpp�� �����Ͻʽÿ�.
//

class CUITool_2011App : public CWinAppEx
{
public:
	CUITool_2011App();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CUITool_2011App theApp;
