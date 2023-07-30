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
#define _WIN32_WINNT 0x0500	// �� ���� Windows 2000 �̻��� ������� �ϴ� �� ������ ������ �����Ͻʽÿ�.
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

#include <winuser.h>
#include <windows.h>

// ���⼭ ���� ���� ��Ŭ���
#include <XTToolkitPro.h>      // Xtreme Toolkit MFC Extensions

// Define (Resource Editor �� ��� �ȵǴ� ��)
#define IDC_PROPERTY_GRID_CHAR		900001
#define IDC_LIST_SKILL				900002
#define IDC_EDIT_MESSAGE_CHAT		900003
#define IDC_EDIT_MESSAGE_SYSTEM		900004
#define IDC_EDIT_MESSAGE_ADMIN		900005
#define IDC_EDIT_MESSAGE_ACTION		900006
#define IDC_PROPERTY_GRID_GUILD		900010

#include "ConfigManager.h"

//#include "AlefAdminXTExtension.h"

#include "AgpdAdmin.h"
#include "AgcmAdmin.h"

//#include "AlefAdminAPI.h"

//#include "AlefAdminStringManager.h"
//#include "AlefAdminOption.h"
//
//#include "AlefAdminSkill.h"
//#include "AlefAdminCharMacro.h"
//#include "AlefAdminCharacter.h"
//#include "AlefAdminItem.h"
//#include "AlefAdminNotice.h"
//#include "AlefAdminGuild.h"
//#include "AlefAdminMain.h"
//
//#include "AlefAdminManager.h"
