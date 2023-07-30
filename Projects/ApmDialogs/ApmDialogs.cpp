// ApmDialogs.cpp : �ش� DLL�� �ʱ�ȭ ��ƾ�� �����մϴ�.
//

#include "stdafx.h"
#include <afxdllx.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE ApmDialogsDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// lpReserved�� ����ϴ� ��� ������ �����Ͻʽÿ�.
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("ApmDialogs.DLL �ʱ�ȭ!\n");
		
		// Ȯ�� DLL�� �� ���� �ʱ�ȭ�մϴ�.
		if (!AfxInitExtensionModule(ApmDialogsDLL, hInstance))
			return 0;

		// �� DLL�� ���ҽ� ü�ο� �����մϴ�.
		// ����: �� Ȯ�� DLL�� MFC ���� ���α׷���
		// �ƴ� ActiveX ��Ʈ�� ���� MFC �⺻ DLL��
		// ���� ��������� ��ũ�Ǿ� �ִ� ��쿡��
		// DllMain���� �� ���� �����ϰ�, ������ ���� �� Ȯ�� DLL����
		// ������ ������ �Լ��� �߰��մϴ�.
		// �׷� ���� �� Ȯ�� DLL�� ����ϴ� �⺻ DLL��
		// �ش� �Լ��� ��������� ȣ���Ͽ� �� Ȯ�� DLL��
		// �ʱ�ȭ�ؾ� �մϴ�. �׷��� ������, CDynLinkLibrary ��ü��
		// Ȯ�� DLL�� ���ҽ� ü�ο� �߰����� �����Ƿ�
		// �ɰ��� ������ �߻��մϴ�.

		new CDynLinkLibrary(ApmDialogsDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("ApmDialogs.DLL ����!\n");

		// �Ҹ��ڰ� ȣ��Ǳ� ���� ���̺귯���� �����մϴ�.
		AfxTermExtensionModule(ApmDialogsDLL);
	}
	return 1;   // Ȯ��
}
