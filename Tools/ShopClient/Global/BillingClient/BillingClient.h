// BillingClient.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CBillingClientApp:
// �� Ŭ������ ������ ���ؼ��� BillingClient.cpp�� �����Ͻʽÿ�.
//

class CBillingClientApp : public CWinApp
{
public:
	CBillingClientApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CBillingClientApp theApp;