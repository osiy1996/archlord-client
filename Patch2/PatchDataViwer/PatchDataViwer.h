// PatchDataViwer.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error PCH���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����Ͻʽÿ�.
#endif

#include "resource.h"		// �� ��ȣ


// CPatchDataViwerApp:
// �� Ŭ������ ������ ���ؼ��� PatchDataViwer.cpp�� �����Ͻʽÿ�.
//

class CPatchDataViwerApp : public CWinApp
{
public:
	CPatchDataViwerApp();

// ������
	public:
	virtual BOOL InitInstance();

// ����

	DECLARE_MESSAGE_MAP()
};

extern CPatchDataViwerApp theApp;
