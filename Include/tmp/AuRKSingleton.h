// AuRKSingleton.h: interface for the AuRKSingleton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AURKSINGLETON_H__B3E3A892_D588_4877_8DBE_907792821CC7__INCLUDED_)
#define AFX_AURKSINGLETON_H__B3E3A892_D588_4877_8DBE_907792821CC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include "AuRK_API.h"
#include "RKDefine.h"


enum EnumErrorCode
{
	EC_EXECUTABLE	= 0,
	EC_INIFILE,
};

class AuRKSingleton  
{
private:
	static AuRKSingleton* _Instance;	// Singleton ��ü 2004.11.24. steeple ����
	
	AuRKSingleton();
	void InitCurrentPath();
	void InitUserIDPassword();
	BOOL InitRepositoryRootPath();
	BOOL RecursiveUpdateFileList(LPCTSTR lpPath, CList<FileInfo, FileInfo> *pList, BOOL bRecursive = TRUE);
	BOOL RecursiveNewFileList(LPCTSTR lpPath, CList<FileInfo, FileInfo> *pList, BOOL bRecursive = TRUE);

public:
	INT32	m_iErrorCode;				// ���� �ڵ带 �����Ѵ�. 0�̸� ����, 1�̸� INI ������ ����. 
	BOOL	m_bAdmin;
	BOOL	m_bLogin;
	CString m_strRepoRootPath;		// Repository Root Path
	CString m_strCurrentPath;		// ���� DLL�� Load�� Path
	CString m_strUserID;			// User ID
	CString m_strWorkingFolder;		// RK�� ������ Local ���
	CString m_strRepoName;			// Repository Name

public:
	virtual ~AuRKSingleton();
	static AuRKSingleton* Instance();
	static void			  DeleteInstance();

	BOOL Initialize();
	BOOL GetFolderNameDlg(LPCTSTR lpTitle, CHAR* pszPath, INT32 iLength);
	BOOL ViewHistory(LPCTSTR lpFilePath);
	BOOL GetLatestFolder(LPCTSTR lpExistPath);
	BOOL GetLatestFile(LPCTSTR lpExistFile);
	INT32 GetRKErrorCode() {return m_iErrorCode;}
	BOOL GetRepoPath(CString &rRepoName, CString &rPathName);
	void CreateRepository();
	void SetWorkingFolder();
	BOOL AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath);
	BOOL AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath);
	void CreateFolder(LPCTSTR lpPath);
	
	// 2004.11.16. steeple
	BOOL Login(LPCTSTR szUserID, LPCTSTR szPassword, LPCTSTR szWorkingFolderPath, LPCTSTR szReporitoryPath);
	BOOL Logout(LPCTSTR szUserID = NULL);
	
	BOOL FileDownLoad(LPCTSTR lpFileName);

	BOOL IsFileExist(LPCSTR sFilePath);
	BOOL IsAvailableRepository(LPCTSTR lpRepositoryPath);
	BOOL IsDirectoryInRepository(LPCTSTR szPath);

	// ���丮 ������
	void ConvertRemotePath(CString &rPath);
	void ConvertLocalPath(CString &rPath);

	// ���� ���� �޼ҵ�
	CString GetFolderPath(CString &rFilePath);
	CString GetFileName(CString &rFilePath);

	BOOL GetUpdatedFileList(CList<FileInfo, FileInfo> *pList);
	BOOL GetNewFileList(CList<FileInfo, FileInfo> *pList);
	
	// 2004.11.16. steeple
	BOOL GetUpdatedFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// ������ �������� �˻�
	BOOL GetNewFileList2(CList<FileInfo, FileInfo>* pList, LPCTSTR lpFolderPath, BOOL bRecursive = TRUE);	// ������ �������� �˻�
	
	// 2004.11.25. steeple
	BOOL GetHistoryList(LPCTSTR lpFileName, CList<FileInfo, FileInfo>* pList);
	BOOL GetHistoryFile(LPCTSTR lpFileName, INT32 lVersion);
	INT32 GetCurrentVersion(LPCTSTR lpFileName);

	void EnableReadOnly(LPCTSTR lpExistingFile, BOOL bEnable);
};

#endif // !defined(AFX_AURKSINGLETON_H__B3E3A892_D588_4877_8DBE_907792821CC7__INCLUDED_)
