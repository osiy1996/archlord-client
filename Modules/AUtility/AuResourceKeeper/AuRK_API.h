#pragma once

#ifdef RK_IMPORT
#define RK_DLL extern "C" __declspec(dllimport)
#else
#define RK_DLL extern "C" __declspec(dllexport)
#endif

#include "RKDefine.h"

//////////////////////////////////////////////////////////////////////////
//
// 2004.11.16. steeple
//
// ��� ���� �̶�� �ּ��� �޸� ����
//
// $RK\��ò���\��ò���.ini
//
// �̷������� ��θ� �Ѱ��ָ� �ȴ�.
//
//


typedef struct _FileInfo
{
	CString m_strFileName;
	CString m_strDate;
	CString m_strOwner;
	INT32	m_iVersion;
} FileInfo;

//////////////////////////////////////////////////////////////////////////
// 2004.11.24. steeple
// 
// ���Ӱ� �߰��ǰų� ������ API. (������ �ַ� ���� ���)
// �̳�� ���ַ� ���� �ҽ��鵵 �����Ͽ��� ������,
// ��~~ �ؿ� �ִ� ������ API �� Ȥ ���� ���� ���� ����. -_-;;
// �� �׷����ϴ�. ļ����!
//

// ex : ConnectRemoteDrive("\\\\alefserver\\ResourceKeeper", "R:", "alef", "archlord");
RK_DLL VOID ConnectRemoteDrive(LPCTSTR lpRemotePCName, LPCTSTR lpLocalDriveName, LPCTSTR lpUserName, LPCTSTR lpPassword);

// 2004.11.17. steeple. ��δ� ����̺� ���� ��ΰ� �;���.
RK_DLL BOOL Login(LPCTSTR szUserID, LPCTSTR szPassword, LPCTSTR szWorkingFolderPath, LPCTSTR szRepositoryPath);

RK_DLL BOOL Logout(LPCTSTR szUserID = NULL);

RK_DLL BOOL IsLogin();

RK_DLL BOOL IsAdmin();

RK_DLL LPCTSTR GetUserID();

RK_DLL BOOL AddUser(LPCTSTR lpUserID, LPCTSTR lpPassword, BOOL bAdmin = FALSE, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL DeleteUser(LPCTSTR lpUserID, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL ChangePassword(LPCTSTR lpUserID, LPCTSTR lpNewPassword, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL SetAdmin(LPCTSTR lpUserID, BOOL bAdmin, LPCTSTR lpRepositoryPath = NULL);

RK_DLL BOOL Lock(LPCTSTR lpFileName);

RK_DLL BOOL UnLock(LPCTSTR lpFileName);

RK_DLL BOOL IsLock(LPCTSTR lpFileName);

RK_DLL BOOL GetLockOwner(LPCTSTR lpFileName, CString &rOwnerName);

RK_DLL BOOL LoadInfoFile(LPTSTR lpBuffer);	// Local �� �ִ� info.rk �� �д´�. lpBuffer �� ���� WorkingFolder �� �־��ش�.

RK_DLL BOOL SaveInfoFile(LPCTSTR lpWorkingFolder);	// Local �� info.rk �� �����. WorkingFolder �����.

RK_DLL BOOL GetUpdatedFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// �Ѱ��� �������� �˻�. ������ ��� ����

RK_DLL BOOL GetNewFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// �Ѱ��� �������� �˻�. ������ ��� ����

RK_DLL BOOL OpenSelectDlg(CList<FileInfo, FileInfo> *pUpdatedFileList, CList<FileInfo, FileInfo> *pSelectedFileList);

RK_DLL BOOL FileDownLoad(LPCTSTR lpFileName);	// ��� ����. 2004.11.18. steeple 

RK_DLL BOOL FileUpload(LPCTSTR lpFileName);		// ��� ����

RK_DLL BOOL FolderUpload(LPCTSTR lpFolderName);	// ��� ����. 2004.11.16. steeple

RK_DLL BOOL DeleteFileInRepository(LPCTSTR lpFileName);	// ��� ����. ������ �ȴ�. 2004.11.16. steeple

RK_DLL BOOL IsLatestFile(LPCTSTR lpFileName);

RK_DLL BOOL GetLatestFile(LPCTSTR lpExistFilePath);	// ��� ����

RK_DLL BOOL GetFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL IsExistFileToRemote(LPCTSTR lpFileName);

RK_DLL LPCTSTR GetWorkingFolderString();

RK_DLL BOOL GetHistoryList(LPCTSTR lpFileName, CList<FileInfo, FileInfo>* pList);

RK_DLL BOOL GetHistoryFile(LPCTSTR lpFileName, INT32 lVersion);

RK_DLL INT32 GetCurrentVersion(LPCTSTR lpFileName);

RK_DLL BOOL OpenHistoryDlg(CList<FileInfo, FileInfo>* pList = NULL);

RK_DLL BOOL DirectoryRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate);

RK_DLL BOOL FileRollBack(CString strRemotePathName, CString strLocalPathName, CString szUser, CString szDate);

RK_DLL BOOL DeleteInstance();





//////////////////////////////////////////////////////////////////////////
// ���������� ���̴� API
// ������ �ܺο����� �����Ͽ�����, ������ �����鼭 ���������� ���̱� ������
// �ܺο��� �θ��� ���а� �� ���� ����.
RK_DLL BOOL CheckIn(LPCTSTR lpFilePath);		// ��� ����

RK_DLL BOOL CheckOut(LPCTSTR lpFileName);		// ��� ����







//////////////////////////////////////////////////////////////////////////
// ������ API
// ���� �۵���������... �۵����� ���� ������ ���� ���!!!
RK_DLL BOOL OpenMainDlg();

RK_DLL BOOL OpenLoginDlg();

RK_DLL BOOL RKLogout();

RK_DLL BOOL OpenFolderDlg();

RK_DLL LPCTSTR GetCurrentPath();

RK_DLL BOOL GetWorkingFolder();

RK_DLL BOOL SetRepoRootPath();

RK_DLL LPCTSTR GetRepoRootPath();

RK_DLL BOOL CreateRepository();

RK_DLL BOOL SetWorkingFolder();

RK_DLL BOOL GetLatestFolder(LPCTSTR lpExistPath);

RK_DLL BOOL AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath = NULL);		// ��� ����

RK_DLL BOOL AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath = NULL);	// ��� ����

RK_DLL BOOL GetUpdatedFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL GetNewFileList(CList<FileInfo, FileInfo> *pList);

RK_DLL BOOL GetLocalName(CString &rLocalName);

