// AlefAdminDLL.h : AlefAdminDLL DLL�� �⺻ ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CAlefAdminDLLApp
// �� Ŭ������ ������ ������ AlefAdminDLL.cpp�� �����Ͻʽÿ�.
//

class CAlefAdminDLLApp : public CWinApp
{
public:
	CAlefAdminDLLApp();

// ������
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
