#if !defined(AFX_PROGRESSDLG_H__ACE45C15_CA39_44DD_BE52_F5D9157FB798__INCLUDED_)
#define AFX_PROGRESSDLG_H__ACE45C15_CA39_44DD_BE52_F5D9157FB798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ProgressDlg.h : header file
//

#include <afxtempl.h>
#include "AuRK_API.h"

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog
// ����/���� ���縸�� �����ϴ� ���̾�α�
class CProgressDlg;

struct CProgressParam
{
	CProgressDlg	*pProgressDlg;
	INT32			*pCurrent;
	BOOL			*pCancel;
};

enum EnumCopyDirection
{
	CD_LOCAL_TO_SERVER	= 0,
	CD_SERVER_TO_LOCAL,
};

enum EnumCopyMode
{
	CM_CREATE		=0,
	CM_MODIFY,
};

enum EnumCopyOperation
{
	CO_INVALID_OPERATION			= 0,
	CO_CHECK_IN,							// ������ üũ��(������ �ø�)
	CO_TO_REMOTE_NEW_FILE,					// ���ο� ���� ���
	CO_TO_REMOTE_NEW_FOLDER,				// ���ο� ���� ���
	CO_TO_REMOTE_MODIFIED_FILE,				// ������ ���� ���(Check In)
	CO_TO_LOCAL_FILE_LIST,					// ���÷� ���� ����Ʈ �ٿ�ε�
	CO_TO_LOCAL_FILE,						// ���÷� ���� �ٿ�ε�
	CO_TO_LOCAL_FOLDER,						// ���÷� ���� �ٿ�ε�
	CO_TO_LOCAL_LATEST_FILE,				// ���÷� �ֽ� ���� �ٿ�ε�
	CO_TO_LOCAL_LATEST_FOLDER,				// ���÷� �ֽ� ���� �ٿ�ε�
	CO_TO_DELETE_FOLDER,					// ������ ������
};

class CProgressDlg : public CDialog
{
private:
	BOOL	m_bCancel;
	INT32	m_lCurrentPos;

	CString m_strExistFilePath;
	CString m_strNewFilePath;

	EnumCopyOperation	m_eCopyOperation;
	EnumCopyMode		m_eCopyMode;
	EnumCopyDirection	m_eCopyDirection;	// TRUE  : Local -> Server�� ���� ����
											// FALSE : Server -> Local�� ���� ����
	CList<FileInfo, FileInfo> *m_pList;

public:
	INT32	m_lTotalSize;

private:
	BOOL ProcessOperation();
	BOOL ProcessAddFile(CProgressParam *pParam);
	BOOL ProcessAddFolder(CProgressParam *pParam);
	BOOL ProcessCheckIn(CProgressParam *pParam);
	BOOL ProcessGetLatestFile(CProgressParam *pParam);
	BOOL ProcessGetLatestFolder(CProgressParam *pParam);
	BOOL ProcessGetFileList(CProgressParam *pParam);
	BOOL ProcessDeleteFolder(CProgressParam *pParam);
	BOOL ProcessDeleteFile(CProgressParam *pParam);

	INT32 GetFolderSize(LPCTSTR lpExistPath);
	INT32 GetFileListSize();
	INT32 GetFileCount(LPCTSTR lpExistPath);

	BOOL DirectoryCopy(CString strExistFilePath, CString strNewFilePath, CProgressParam *pParam, BOOL bCopyMode = TRUE);
	BOOL FileCopy(CString strExistFilePath, CString strNewFilePath, CProgressParam *pParam, INT32 lFileSize);

	BOOL GetStringInfo(CString &rLocalPathName, CString &rLocalFileName, CString &rRemotePathName, 
						CString &rRemoteFileName, CString &rEntriesFileName, CString &rRemoteFilePath);

	BOOL FileGetLatest(CString strRemotePathName, CString strLocalPathName, CProgressParam *pParam);
	BOOL DirectoryGetLatest(CString strRemotePathName, CString strLocalPathName, CProgressParam *pParam);

	BOOL RemoveFolder(CString strExistPath, CProgressParam *pParam);

// Construction
public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor

	void AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath = NULL);	// OK
	void AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath = NULL);			// OK
	void UploadFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath = NULL);
	void CheckIn(LPCTSTR lpFilePath);

	void GetLatestFile(LPCTSTR lpFilePath);			// OK
	void GetLatestFolder(LPCTSTR lpFolderPath);	// OK
	void GetFileList(CList<FileInfo, FileInfo> *pList);	// OK

	void DeleteFolder(LPCTSTR lpExistPath);			// OK
	void DeleteFile(LPCTSTR lpExistFilePath);

// Dialog Data
	//{{AFX_DATA(CProgressDlg)
	enum { IDD = IDD_PROGRESS };
	CProgressCtrl	m_ctrlProgress;
	CString	m_strFileName;
	CString	m_strSize;
	CString	m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProgressDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROGRESSDLG_H__ACE45C15_CA39_44DD_BE52_F5D9157FB798__INCLUDED_)
