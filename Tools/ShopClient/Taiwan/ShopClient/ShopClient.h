// ShopClient.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CShopClientApp:
// �� Ŭ������ ������ ���ؼ��� ShopClient.cpp�� �����Ͻʽÿ�.
//

class CShopClientApp : public CWinApp
{
public:
	CShopClientApp();

// �������Դϴ�.
	public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CShopClientApp theApp;