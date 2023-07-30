// AlefAdminManager.h
// (C) NHN - ArchLord Development Team
// steeple, 2004. 02. 14.

// ���� ��Ʈ

// 2004.02.16.
// AlefAdminDLL �� ����鼭 �������ٴ� ���� ���� �����ϱ� ���ؼ�
// AgcmAdmin Module �� �����ٰ� ����Ϸ��� �ߴ�.
// �������� �������� �� �ǰ�, ���� ��⵵ �� ������ �� �־���.
// �׷��� ����ġ ���ߴ� ������ �߻��ߴ�. ������ �� �ϳ�.

// g_AuCircularBuffer �� extern ���� AuPacket.h �� �����ִµ�,
// DLL �� InitInstance �Ǵ� �������� �ϳ��� ������,
// AlefClient �� ����Ǵ� �������� ���ϳ��� ������. -_-;;
// ��, global extern ������ ������ �ȵȴٴ� ���̴�.
// ������ �̷� ������ �� ���� ���� �ֱ� ������,
// �׳� ���������� �����ϱ�� �ٽ� �����ߴ�.
// �׷��� �׸��ϱ⿣ �ʹ� �ð��� �����ɸ���....
// �̸� ��°�� �ϳ�...

// 2004.02.24.
// �׳� �������� global extern ������ ���� �ʱ�ȭ���ִϱ� �ߵȴ�.............
// ������ ���ȴ�.............................
// ��ġ�ڴ�..................................

#pragma once

#include "AlefAdminMain.h"
#include "AlefAdminStringManager.h"
#include "CDataManager.h"
#include "AgcmAdmin.h"

class AlefAdminManager
{
private:
	static AlefAdminManager* _Instance;

protected:
	AlefAdminManager();

public:
	virtual ~AlefAdminManager();

	static AlefAdminManager* Instance();

	void DeleteInstance();

public:
	stAgpdAdminInfo m_stSelfAdminInfo;
	PVOID m_pAdminModule;

	CDataManager m_DataManager;

	AlefAdminMain* m_pMainDlg;

	AlefAdminStringManager m_csStringManager;

public:
	// Common
	BOOL SetAdminModule(PVOID pModule);
	BOOL SetAdminModule(AgcmAdmin* pcsAgcmAdmin);
	AgcmAdmin* GetAdminModule( void );
	CDataManager* GetAdminData( void );

	BOOL IsDialogMessage(LPMSG lpMsg);
	BOOL SetSelfAdminInfo(stAgpdAdminInfo* pstInfo);
	INT16 GetAdminLevel();
	BOOL OpenNotPrivilegeDlg();

	void OnInit();
	BOOL IsInitialized();

	UINT32 GetCurrentTimeStamp();
	BOOL ConvertTimeStampToString(UINT32 ulTimeStamp, CHAR* szTmp);
	BOOL ConvertTimeStampToDateString(UINT32 ulTimeStamp, CHAR* szTmp);

	// Main
	BOOL OpenMainDlg();
	BOOL OnIdle() { return m_DataManager.OnIdle(); }

	// Item
	BOOL AddItemTemplate(stAgpdAdminItemTemplate* pstItemTemplate);
	BOOL ClearItemTemplateData();

	// String Manager
	AlefAdminStringManager& GetStringManager();
};