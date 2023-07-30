// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

// �Ʒ� ������ �÷����� �켱�ϴ� �÷����� ������� �ؾ� �Ѵٸ� ���� ���Ǹ� �����Ͻʽÿ�.
// �ٸ� �÷����� ���Ǵ� �ش� ���� �ֽ� ������ MSDN�� �����Ͻʽÿ�.
#ifndef WINVER				// Windows 95 �� Windows NT 4 �̻󿡼��� ����� ����� �� �ֽ��ϴ�.
#define WINVER 0x0400		// �� ���� Windows 98 �� Windows 2000 �̻��� ������� �ϴ� �� ������ ������ �����Ͻʽÿ�.
#endif

#ifndef _WIN32_WINNT		// Windows NT 4 �̻󿡼��� ����� ����� �� �ֽ��ϴ�.
#define _WIN32_WINNT 0x0400	// �� ���� Windows 2000 �̻��� ������� �ϴ� �� ������ ������ �����Ͻʽÿ�.
#endif						

#ifndef _WIN32_WINDOWS		// Windows 98 �̻󿡼��� ����� ����� �� �ֽ��ϴ�.
#define _WIN32_WINDOWS 0x0410 // �� ���� Windows Me �̻��� ������� �ϴ� �� ������ ������ �����Ͻʽÿ�.
#endif

#ifndef _WIN32_IE			// IE 4.0 �̻󿡼��� ����� ����� �� �ֽ��ϴ�.
#define _WIN32_IE 0x0400	// �� ���� IE 5.0 �̻��� ������� �ϴ� �� ������ ������ �����Ͻʽÿ�.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxole.h>         // MFC OLE Ŭ�����Դϴ�.
#include <afxodlgs.h>       // MFC OLE ��ȭ ���� Ŭ�����Դϴ�.
#include <afxdisp.h>        // MFC �ڵ�ȭ Ŭ�����Դϴ�.
#endif // _AFX_NO_OLE_SUPPORT

#ifndef _AFX_NO_DB_SUPPORT
#include <afxdb.h>			// MFC ODBC �����ͺ��̽� Ŭ�����Դϴ�.
#endif // _AFX_NO_DB_SUPPORT

#ifndef _AFX_NO_DAO_SUPPORT
#include <afxdao.h>			// MFC DAO �����ͺ��̽� Ŭ�����Դϴ�.
#endif // _AFX_NO_DAO_SUPPORT

#include <afxdtctl.h>		// Internet Explorer 4�� ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

