// ProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "auresourcekeeper.h"
#include "ProgressDlg.h"

#include "AuRKSingleton.h"

#include "RKDefine.h"
#include "RKInclude.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog
BOOL MessagePeekAndPump()
{
	static MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) 
	{
		if (!AfxGetApp()->PumpMessage()) 
		{
			::PostQuitMessage(0);
			return FALSE;
		}	
	}

	return TRUE;
}

DWORD CALLBACK CopyProgressCallBack(LARGE_INTEGER TotalFileSize, LARGE_INTEGER TotalBytesTransferred,
                        LARGE_INTEGER StreamSize, LARGE_INTEGER StreamBytesTransferred,
						DWORD dwStreamNumber, DWORD dwCallbackReason, HANDLE hSourceFile,
						HANDLE hDestinationFile, LPVOID lpData)
{
	CProgressParam *pParam = (CProgressParam*)lpData;
	pParam->pProgressDlg->m_ctrlProgress.SetPos(*pParam->pCurrent + TotalBytesTransferred.LowPart);
	pParam->pProgressDlg->m_strSize.Format("%d/%d", *pParam->pCurrent + TotalBytesTransferred.LowPart, 
											pParam->pProgressDlg->m_lTotalSize);
	pParam->pProgressDlg->UpdateData(FALSE);

	MessagePeekAndPump();
	return PROGRESS_CONTINUE;
}


CProgressDlg::CProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProgressDlg)
	m_strFileName = _T("");
	m_strSize = _T("");
	m_strMessage = _T("");
	//}}AFX_DATA_INIT
	m_bCancel		= FALSE;
	m_lCurrentPos	= 0;
	m_eCopyOperation = CO_INVALID_OPERATION;
}


void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
	DDX_Text(pDX, IDC_FILENAME, m_strFileName);
	DDX_Text(pDX, IDC_FILESIZE, m_strSize);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here

	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CProgressDlg::ProcessOperation()
{
	CProgressParam Param;
	INT32 lCurrent = 0;

	Param.pProgressDlg = this;
	Param.pCancel	= &m_bCancel;
	Param.pCurrent	= &lCurrent;

	switch(m_eCopyOperation)
	{
	case CO_CHECK_IN:				return ProcessCheckIn(&Param);		break;
	case CO_TO_REMOTE_NEW_FILE:		return ProcessAddFile(&Param);		break;
	case CO_TO_REMOTE_NEW_FOLDER:	return ProcessAddFolder(&Param);	break;
	case CO_TO_REMOTE_MODIFIED_FILE:
	case CO_TO_LOCAL_FILE_LIST:		return ProcessGetFileList(&Param);	break;
	case CO_TO_LOCAL_FILE:
	case CO_TO_LOCAL_FOLDER:
	case CO_TO_LOCAL_LATEST_FILE:	return ProcessGetLatestFile(&Param);	break;
	case CO_TO_LOCAL_LATEST_FOLDER:	return ProcessGetLatestFolder(&Param);	break;
	case CO_TO_DELETE_FOLDER:		return ProcessDeleteFolder(&Param);		break;
	default: return FALSE;
	}
}

BOOL CProgressDlg::ProcessAddFile(CProgressParam *pParam)
{
	m_strMessage = "���� ����";
	UpdateData(FALSE);

	CString strLocalPathName, strLocalFileName, strRemotePathName, strRemoteFileName, strEntriesFileName, strRemoteFilePath;
	if (!GetStringInfo(strLocalPathName, strLocalFileName, strRemotePathName, strRemoteFileName, strEntriesFileName, strRemoteFilePath))
		return FALSE;

	CRKEntryManager cAddFile;
	cAddFile.SetEntryWriterFileName(strEntriesFileName);
	cAddFile.SetEntryParserFileName(strEntriesFileName);
	if (!cAddFile.GetEntryFileInfo(strRemoteFileName))
	{
		// ���� ������ �������� �������� ���
		CFileStatus csStatus;
		m_strFileName = strLocalPathName + PATH(strLocalFileName);
		if (CFile::GetStatus(m_strFileName, csStatus))
		{
			m_lTotalSize = (INT32)csStatus.m_size;
			m_ctrlProgress.SetRange32(0, (INT32)csStatus.m_size);

			UpdateData(FALSE);

			if (FileCopy(m_strFileName, strRemoteFilePath, pParam, (INT32)csStatus.m_size))
			{
				cAddFile.AddEntry(m_strFileName);
				CString szNewDate;
				szNewDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csStatus.m_mtime.GetYear(), csStatus.m_mtime.GetMonth(),
																				csStatus.m_mtime.GetDay(), csStatus.m_mtime.GetHour(),
																				csStatus.m_mtime.GetMinute(), csStatus.m_mtime.GetSecond());
				cAddFile.HistoryFile(strRemoteFilePath, szNewDate, AuRKSingleton::Instance()->m_strUserID, 1);
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CProgressDlg::GetStringInfo(CString &rLocalPathName, CString &rLocalFileName, CString &rRemotePathName, 
									CString &rRemoteFileName, CString &rEntriesFileName, CString &rRemoteFilePath)
{
	CString strRemote, strLocal;
	strLocal = m_strExistFilePath;

	// ��� ���
	rLocalPathName = AuRKSingleton::Instance()->GetFolderPath(m_strExistFilePath);
	AuRKSingleton::Instance()->ConvertLocalPath(rLocalPathName);
	
	// ���� �̸� ���
	rLocalFileName = AuRKSingleton::Instance()->GetFileName(m_strExistFilePath);

	// Repository ��� ���
	if (m_strNewFilePath.IsEmpty())
	{
		// �Ķ���Ͱ� �ϳ��϶� 
		rRemotePathName = rLocalPathName;
		rRemotePathName.MakeUpper();
		rRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder + PATH("RK"), RKROOT);
		AuRKSingleton::Instance()->ConvertRemotePath(rRemotePathName);

		rRemoteFileName = rLocalFileName;
	}
	else
	{
		// �Ķ���Ͱ� �ΰ��϶�
		rRemotePathName = AuRKSingleton::Instance()->GetFolderPath(m_strNewFilePath);
		rRemotePathName.MakeUpper();
		rRemotePathName.Replace(AuRKSingleton::Instance()->m_strRepoRootPath + PATH("RK"), RKROOT);
		AuRKSingleton::Instance()->ConvertRemotePath(rRemotePathName);

		rRemoteFileName = AuRKSingleton::Instance()->GetFileName(m_strNewFilePath);
	}

	rEntriesFileName.Format("%s\\%s", rRemotePathName, ENTRIES);
	rRemoteFilePath.Format("%s\\%s", rRemotePathName, rRemoteFileName);

	return TRUE;
}

void CProgressDlg::UploadFile(LPCTSTR lpExistFilePath, LPCTSTR lpENewFilePath)
{
/*
	CString strLocalPathName, strLocalFileName, strRemotePathName, strRemoteFileName, strEntriesFileName, strRemoteFilePath;
	if (!GetStringInfo(strLocalPathName, strLocalFileName, strRemotePathName, strRemoteFileName, strEntriesFileName, strRemoteFilePath))
		return FALSE;
*/
}

BOOL CProgressDlg::FileCopy(CString strExistFilePath, CString strNewFilePath, CProgressParam *pParam, INT32 lFileSize)
{
	// Entries ���Ͽ� ���� �������� �۾� ������ �����ϴٰ� �ǴܵǸ� ���� ����
	if (!::CopyFileEx(strExistFilePath, strNewFilePath, CopyProgressCallBack, (LPVOID)pParam, pParam->pCancel, 0))
	{
		DWORD dwCode = ::GetLastError();
		return FALSE;
	}

	*pParam->pCurrent += lFileSize;

	return TRUE;
}

INT32 CProgressDlg::GetFolderSize(LPCTSTR lpExistPath)
{
	INT32 nSize = 0;
	CFileFind FileFind;
	CString strExistFilePath;
	strExistFilePath.Format("%s\\*.*", lpExistPath);
	BOOL bWorking = FileFind.FindFile(strExistFilePath);
	if (!bWorking) return FALSE;

	// JNY TODO : �ߺ��� ������ ������� ������ ��꿡�� ����

	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
		if (FileFind.IsDots()) continue;	// ., .. �� ��󿡼� ����

		strExistFilePath.Format("%s\\%s", lpExistPath, FileFind.GetFileName());

		if (FileFind.IsDirectory())
		{
			nSize += GetFolderSize(strExistFilePath);
		}
		else
		{
			CFileStatus csStatus;
			if(CFile::GetStatus(strExistFilePath, csStatus))
			{
				nSize += (INT32)csStatus.m_size;
			}
		}
	}

	return nSize;
}

BOOL CProgressDlg::DirectoryCopy(CString strExistFilePath, CString strNewFilePath, CProgressParam *pParam, BOOL bCopyMode)
{
	CFileFind FileFind;
	::CreateDirectory(strNewFilePath, NULL);
	BOOL bWorking = FileFind.FindFile(strExistFilePath + PATH("*.*"));
	if (!bWorking) return FALSE;

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryParserFileName(strNewFilePath + PATH(ENTRIES));
	cRKEntryManager.SetEntryWriterFileName(strNewFilePath + PATH(ENTRIES));

	CFileStatus csStatus;
	::ZeroMemory(&csStatus, sizeof(CFileStatus));
	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
		if (FileFind.GetFileName().Find(ENTRIES) != -1) continue;
		if (FileFind.IsDots()) continue;	// ., .. �� ��󿡼� ����

		if (FileFind.IsDirectory())
		{
			if (DirectoryCopy(strExistFilePath + PATH(FileFind.GetFileName()),
						strNewFilePath + PATH(FileFind.GetFileName()), pParam, bCopyMode))
			{
				if (!cRKEntryManager.GetEntryFileInfo(FileFind.GetFileName()))
					cRKEntryManager.AddEntry(strExistFilePath + PATH(FileFind.GetFileName()));
			}
		}
		else
		{
			if(CFile::GetStatus(strExistFilePath + PATH(FileFind.GetFileName()), csStatus))
			{
				m_strFileName = FileFind.GetFileName();
				UpdateData(FALSE);

				BOOL bNewFile = FALSE;
				if (bCopyMode)
				{
					bNewFile = TRUE;
					if (FileCopy(strExistFilePath + PATH(FileFind.GetFileName()), 
									strNewFilePath + PATH(FileFind.GetFileName()), pParam, (INT32)csStatus.m_size))
					{
					}
				}
				else
				{
					if (!AuRKSingleton::Instance()->IsFileExist(strNewFilePath + PATH(FileFind.GetFileName())))
					{
						bNewFile = TRUE;
						if (FileCopy(strExistFilePath + PATH(FileFind.GetFileName()), 
										strNewFilePath + PATH(FileFind.GetFileName()), pParam, (INT32)csStatus.m_size))
						{
						}
					}
					else
					{
						// ������ ���� ���� ������Ʈ �ؾ��ϴ��� �˻��Ѵ�.
						if(!cRKEntryManager.CheckLatestVersion(strExistFilePath + PATH(FileFind.GetFileName()),
															strNewFilePath + PATH(FileFind.GetFileName())))
						{
							// 2004.11.17. steeple
							cRKEntryManager.CheckIn(strExistFilePath + PATH(FileFind.GetFileName()),
													strNewFilePath + PATH(FileFind.GetFileName()),
													AuRKSingleton::Instance()->m_strUserID,
													FALSE);	// CheckIn �ȿ����� Copy ���� �ʴ´�.

							if (FileCopy(strExistFilePath + PATH(FileFind.GetFileName()), 
											strNewFilePath + PATH(FileFind.GetFileName()), pParam, (INT32)csStatus.m_size))
							{
							}
						}
					}
				}

				m_lCurrentPos += (INT32)csStatus.m_size;

				if (bNewFile)
				{
					cRKEntryManager.AddEntry(strExistFilePath + PATH(FileFind.GetFileName()));

					if (csStatus.m_szFullName[0])
					{
						CString szNewDate;
						szNewDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csStatus.m_mtime.GetYear(), csStatus.m_mtime.GetMonth(),
																						csStatus.m_mtime.GetDay(), csStatus.m_mtime.GetHour(),
																						csStatus.m_mtime.GetMinute(), csStatus.m_mtime.GetSecond());
						cRKEntryManager.HistoryFile(strNewFilePath + PATH(FileFind.GetFileName()), szNewDate, AuRKSingleton::Instance()->m_strUserID, 1);
					}
					else
					{
						CString strMsg;
						OutputDebugString("\n���� �̸��� ã���� ����");
					}
				}
			}
		}
	}

	return TRUE;
}


void CProgressDlg::AddFile(LPCTSTR lpExistFilePath, LPCTSTR lpNewFilePath)
{
	m_eCopyOperation = CO_TO_REMOTE_NEW_FILE;
	m_strExistFilePath = lpExistFilePath;
	m_strNewFilePath = lpNewFilePath;
	if (!lpNewFilePath) m_strNewFilePath.Empty();
}

void CProgressDlg::AddFolder(LPCTSTR lpExistPath, LPCTSTR lpNewPath)
{
	m_eCopyOperation = CO_TO_REMOTE_NEW_FOLDER;
	m_strExistFilePath = lpExistPath;
	m_strNewFilePath = lpNewPath;
	if (!lpNewPath) m_strNewFilePath.Empty();
}

BOOL CProgressDlg::ProcessAddFolder(CProgressParam *pParam)
{
	m_strMessage = "���� ����";
	UpdateData(FALSE);

	CString strRemotePathName;
	if (m_strNewFilePath.IsEmpty())
	{
		// �Ķ���Ͱ� �ϳ��϶�
		strRemotePathName = m_strExistFilePath;
		strRemotePathName.MakeUpper();
		//if (!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder + PATH("RK"), RKROOT)) return FALSE;
		// 2004.11.16. steeple ����. �߰��� PATH("RK") ��
		if (!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) return FALSE;
		AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	}
	else
	{
		// �Ķ���Ͱ� �ΰ��϶�
		strRemotePathName = m_strNewFilePath;
		strRemotePathName.MakeUpper();
		if (!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strRepoRootPath + PATH("RK"), RKROOT)) return FALSE;
		AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	}

	CString strFolderName = strRemotePathName;
	INT32 lPos = strFolderName.ReverseFind('\\');
	strFolderName.Delete(0, lPos + 1);

	CString strEntryFolder = strRemotePathName;
	INT32 lPos2 = strEntryFolder.ReverseFind('\\');
	strEntryFolder.Delete(lPos2, strEntryFolder.GetLength() - lPos2);

	// 2004.11.16. steeple
	// ���������� �������丮�� ������ FALSE �� �����Ѵ�.
	if(_access((LPCTSTR)strEntryFolder, 00) == -1)
		return FALSE;

	AuRKSingleton::Instance()->CreateFolder(strRemotePathName);
	::CreateDirectory(strRemotePathName, NULL);

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryWriterFileName(strEntryFolder + PATH(ENTRIES));
	cRKEntryManager.SetEntryParserFileName(strEntryFolder + PATH(ENTRIES));

	BOOL bCopyMode = FALSE;
	if (!cRKEntryManager.GetEntryFileInfo(strFolderName))
	{
		bCopyMode = TRUE;
		cRKEntryManager.AddEntry(strRemotePathName);
	}

	m_lTotalSize = GetFolderSize(m_strExistFilePath);
	m_ctrlProgress.SetRange32(0, m_lTotalSize);

	return DirectoryCopy(m_strExistFilePath, strRemotePathName, pParam, bCopyMode);
}

void CProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == 1)
	{
		KillTimer(1);
		if (ProcessOperation()) CDialog::OnOK();
		else CDialog::OnCancel();
	}

	CDialog::OnTimer(nIDEvent);
}

void CProgressDlg::CheckIn(LPCTSTR lpFilePath)
{
	m_eCopyOperation = CO_CHECK_IN;
	m_strExistFilePath = lpFilePath;
	m_strNewFilePath.Empty();
}

BOOL CProgressDlg::ProcessCheckIn(CProgressParam *pParam)
{
	CString strRemotePathName, strLocalPathName, strFileName;

	// �Ķ���Ͱ� �ϳ�
	strRemotePathName = m_strExistFilePath;
	INT32 lPos = strRemotePathName.ReverseFind('\\');
	strRemotePathName.Delete(lPos, strRemotePathName.GetLength() - lPos);

	strFileName = m_strExistFilePath;
	INT32 lPos2 = strFileName.ReverseFind('\\');
	strFileName.Delete(0, lPos2);

	strRemotePathName.MakeUpper();
	//if (!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder + PATH("RK"), RKROOT)) return FALSE;
	strLocalPathName = strRemotePathName;
	AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	AuRKSingleton::Instance()->ConvertLocalPath(strLocalPathName);

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryParserFileName(strRemotePathName + PATH(ENTRIES));
	cRKEntryManager.SetEntryWriterFileName(strRemotePathName + PATH(ENTRIES));

	return cRKEntryManager.UndoCheckOut(strLocalPathName + PATH(strFileName),
		strRemotePathName + PATH(strFileName), AuRKSingleton::Instance()->m_strUserID);
}

void CProgressDlg::GetLatestFile(LPCTSTR lpFilePath)
{
	m_eCopyOperation = CO_TO_LOCAL_FILE;
	m_strExistFilePath = lpFilePath;
	m_strNewFilePath.Empty();
}

BOOL CProgressDlg::ProcessGetLatestFile(CProgressParam *pParam)
{
	m_strMessage = "�ֽ� ����";
	UpdateData(FALSE);

	CString strRemotePathName, strLocalPathName, strFileName;

	strRemotePathName = m_strExistFilePath;

	strRemotePathName.MakeUpper();
	//if ((!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) &&
	//	(!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strRepoRootPath + "\\RK", RKROOT ))) return FALSE;
	strLocalPathName = strRemotePathName;
	AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	
	// Ȥ�� �̷��� ���������� �𸣴�... 2004.11.23. steeple
	strLocalPathName.Replace(AuRKSingleton::Instance()->m_strRepoRootPath + "\\RK\\", RKROOT);
	AuRKSingleton::Instance()->ConvertLocalPath(strLocalPathName);

	return FileGetLatest(strRemotePathName, strLocalPathName, pParam);
}

void CProgressDlg::GetLatestFolder(LPCTSTR lpFolderPath)
{
	m_eCopyOperation = CO_TO_LOCAL_LATEST_FOLDER;
	m_strExistFilePath = lpFolderPath;
	m_strNewFilePath.Empty();	
}

BOOL CProgressDlg::ProcessGetLatestFolder(CProgressParam *pParam)
{
	m_strMessage = "�ֽ� ����";
	UpdateData(FALSE);

	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	CString strRemotePathName, strLocalPathName, strFileName;

	strRemotePathName = m_strExistFilePath;

	strRemotePathName.MakeUpper();
	//if (!strRemotePathName.Replace(AuRKSingleton::Instance()->m_strWorkingFolder, RKROOT)) return FALSE;
	strLocalPathName = strRemotePathName;
	AuRKSingleton::Instance()->ConvertRemotePath(strRemotePathName);
	AuRKSingleton::Instance()->ConvertLocalPath(strLocalPathName);

	cRKEntryManager.GetEntryList(strRemotePathName + PATH(ENTRIES), cList);

	m_lTotalSize = GetFolderSize(strRemotePathName);
	m_ctrlProgress.SetRange32(0, m_lTotalSize);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);
		CString strExist = strRemotePathName + PATH(stRKEntryInfo.szFileName);
		CString strNew = strLocalPathName + PATH(stRKEntryInfo.szFileName);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			DirectoryGetLatest(strExist, strNew, pParam);
		}
		else
		{
			FileGetLatest(strExist, strNew, pParam);
		}
	}

	return TRUE;
}

BOOL CProgressDlg::FileGetLatest(CString strRemotePathName, CString strLocalPathName, CProgressParam *pParam)
{
	CRKEntryManager cFileVersion;
	if (!cFileVersion.CheckLatestVersion(strRemotePathName, strLocalPathName))
	{
		// �ֽ� ������ �ƴ϶��
		AuRKSingleton::Instance()->CreateFolder(strLocalPathName);	// �ش� ������ ���� ��� ���� ��θ� �ϼ�������

		CFileStatus csStatus;
		if(CFile::GetStatus(strRemotePathName, csStatus))
		{
			CString strFileName = AuRKSingleton::Instance()->GetFileName(strRemotePathName);
			AuRKSingleton::Instance()->EnableReadOnly(strLocalPathName, FALSE);

			char szOut[255];
			BOOL bDeleteResult = ::DeleteFile((LPCTSTR)strLocalPathName);
			if(!bDeleteResult)
			{
				wsprintf(szOut, "DeleteFile GetLastError : %d\n", GetLastError());
				//::AfxMessageBox(szOut, MB_OK);
			}

			if (!FileCopy(strRemotePathName, strLocalPathName, pParam, (INT32)csStatus.m_size))
			{
				wsprintf(szOut, "CopyFileEx Get LastError : %d\n", GetLastError());
				::AfxMessageBox(szOut, MB_OK);
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CProgressDlg::DirectoryGetLatest(CString strRemotePathName, CString strLocalPathName, CProgressParam *pParam)
{
	CRKEntryManager cRKEntryManager;
	CList<stRKEntryInfo, stRKEntryInfo> cList;

	cRKEntryManager.GetEntryList(strRemotePathName + PATH(ENTRIES), cList);

	INT32 iSize = cList.GetCount();
	POSITION p = cList.GetHeadPosition();
	
	for(INT32 i = 0; i < iSize; i++)
	{
		stRKEntryInfo stRKEntryInfo = cList.GetNext(p);
		CString strExist = strRemotePathName + PATH(stRKEntryInfo.szFileName);
		CString strNew = strLocalPathName + PATH(stRKEntryInfo.szFileName);

		if (RK_FOLDER == stRKEntryInfo.szType)
		{
			DirectoryGetLatest(strExist, strNew, pParam);
		}
		else
		{
			FileGetLatest(strExist, strNew, pParam);
		}
	}

	return TRUE;
}

void CProgressDlg::GetFileList(CList<FileInfo, FileInfo> *pList)
{
	m_eCopyOperation = CO_TO_LOCAL_FILE_LIST;
	m_pList = pList;
}

INT32 CProgressDlg::GetFileListSize()
{
	INT32 lResult = 0;
	POSITION p = m_pList->GetHeadPosition();
	while (p)
	{
		FileInfo Info = m_pList->GetNext(p);

		CFileStatus csStatus;
		Info.m_strFileName.MakeUpper();
		if (!Info.m_strFileName.Replace(RKROOT, AuRKSingleton::Instance()->m_strRepoRootPath + "\\RK"))
			continue;
		
		if (CFile::GetStatus(Info.m_strFileName, csStatus))
		{
			lResult += (INT32)csStatus.m_size;
		}
	}

	return lResult;
}

BOOL CProgressDlg::ProcessGetFileList(CProgressParam *pParam)
{
	m_strMessage = "���ϸ���Ʈ ����";
	UpdateData(FALSE);

	this->m_lTotalSize = GetFileListSize();
	m_ctrlProgress.SetRange32(0, m_lTotalSize);

	INT32 lResult = 0;
	POSITION p = m_pList->GetHeadPosition();
	while (p)
	{
		FileInfo Info = m_pList->GetNext(p);

		CFileStatus csStatus;
		CString strRemoteFilePath = Info.m_strFileName;
		AuRKSingleton::Instance()->ConvertRemotePath(strRemoteFilePath);
		if (CFile::GetStatus(strRemoteFilePath, csStatus))
		{
			m_strExistFilePath = Info.m_strFileName;
			AuRKSingleton::Instance()->ConvertRemotePath(m_strExistFilePath);
			ProcessGetLatestFile(pParam);
		}
	}

	return TRUE;
}

INT32 CProgressDlg::GetFileCount(LPCTSTR lpExistPath)
{
	INT32 nCount = 0;
	CFileFind FileFind;
	CString strExistFilePath;
	strExistFilePath.Format("%s\\*.*", lpExistPath);
	BOOL bWorking = FileFind.FindFile(strExistFilePath);
	if (!bWorking) return FALSE;

	// JNY TODO : �ߺ��� ������ ������� ������ ��꿡�� ����

	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
		if (FileFind.IsDots()) continue;	// ., .. �� ��󿡼� ����

		strExistFilePath.Format("%s\\%s", lpExistPath, FileFind.GetFileName());

		if (FileFind.IsDirectory())
		{
			nCount += GetFileCount(strExistFilePath);
		}
		else
		{
			CFileStatus csStatus;
			if(CFile::GetStatus(strExistFilePath, csStatus))
			{
				nCount++;
			}
		}
	}

	return nCount;	
}

void CProgressDlg::DeleteFolder(LPCTSTR lpExistPath)
{
	m_eCopyOperation = CO_TO_DELETE_FOLDER;
	m_strExistFilePath = lpExistPath;
}

BOOL CProgressDlg::ProcessDeleteFolder(CProgressParam *pParam)
{
	m_strMessage = "���� ����";
	UpdateData(FALSE);

	m_lTotalSize = GetFileCount(m_strExistFilePath);
	m_ctrlProgress.SetRange32(0, m_lTotalSize);

//	BOOL bResult = TRUE;
	BOOL bResult = RemoveFolder(m_strExistFilePath, pParam);

	// ���� ������ ENTRIES���Ͽ��� ������ ���丮�� ���ش�.
	CString strEntries = m_strExistFilePath;
	INT32 lPos = strEntries.ReverseFind('\\');
	strEntries.Delete(lPos, strEntries.GetLength() - lPos);

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryWriterFileName(strEntries + PATH(ENTRIES));
	cRKEntryManager.SetEntryParserFileName(strEntries + PATH(ENTRIES));

	cRKEntryManager.RemoveDirectoryEntry(m_strExistFilePath, AuRKSingleton::Instance()->m_strUserID);
	::RemoveDirectory(m_strExistFilePath);

	return bResult;
}

BOOL CProgressDlg::RemoveFolder(CString strExistPath, CProgressParam *pParam)
{
	CFileFind FileFind;
//	::CreateDirectory(strNewFilePath, NULL);
	BOOL bWorking = FileFind.FindFile(strExistPath + PATH("*.*"));
	if (!bWorking) return FALSE;

	CRKEntryManager cRKEntryManager;
	cRKEntryManager.SetEntryWriterFileName(strExistPath + PATH(ENTRIES));

	CFileStatus csStatus;
	while (bWorking)
	{
		bWorking = FileFind.FindNextFile();
//		if (FileFind.GetFileName().Find(ENTRIES) != -1) continue;
		if (FileFind.IsDots()) continue;	// ., .. �� ��󿡼� ����

		if (FileFind.IsDirectory())
		{
			RemoveFolder(strExistPath + PATH(FileFind.GetFileName()), pParam);
			::RemoveDirectory(strExistPath + PATH(FileFind.GetFileName()));
		}
		else
		{
			m_strFileName = strExistPath + PATH(FileFind.GetFileName());
			UpdateData(FALSE);

			AuRKSingleton::Instance()->EnableReadOnly(strExistPath + PATH(FileFind.GetFileName()), FALSE);
			::DeleteFile(strExistPath + PATH(FileFind.GetFileName()));

			m_lCurrentPos++;
			m_ctrlProgress.SetPos(m_lCurrentPos);
			m_strSize.Format("%d/%d", m_lCurrentPos, m_lTotalSize);
			UpdateData(FALSE);
		}

		cRKEntryManager.RemoveEntry(strExistPath + PATH(FileFind.GetFileName()), 
									AuRKSingleton::Instance()->m_strUserID);
	}

	return TRUE;	
}

void CProgressDlg::DeleteFile(LPCTSTR lpExistFilePath)
{
/*
	// ���� ���� ����
	CRKEntryManager cRKEntryManager;
	CString strPath = GetPathFromNode(m_hRClickItem);
	CString strRepoPath = strPath;
	AuRKSingleton::Instance()->ConvertRemotePath(strRepoPath);

	CString strFileName = m_ctrlFile.GetItemText(m_iRClickIndex, 0);

	CString strMsg;
	cRKEntryManager.SetEntryParserFileName(strRepoPath + PATH(ENTRIES));
	cRKEntryManager.SetEntryWriterFileName(strRepoPath + PATH(ENTRIES));
	if (cRKEntryManager.RemoveEntry(strRepoPath + PATH(m_ctrlFile.GetItemText(m_iRClickIndex, 0)), 
				AuRKSingleton::Instance()->m_strUserID))
*/
}