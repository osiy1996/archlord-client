// RKEntryManager.cpp
// 2003.07.30 steeple

#include "stdafx.h"
#include "RKInclude.h"
//#include "imagehlp.h"
// imagehlp.h -> dbghelp.h
#include "dbghelp.h"

BOOL MyCopyFile(LPCTSTR szLocal, LPCTSTR szTarget, BOOL bOverwrite);

CRKEntryManager::CRKEntryManager()
{
	m_szRemoteDrive = "";

	Init();
}

CRKEntryManager::~CRKEntryManager()
{
}

void CRKEntryManager::Init()
{
	ClearEntryInfo();
	ClearHistoryInfo();
}

void CRKEntryManager::ClearEntryInfo()
{
	m_stRKEntryInfo.szType = "";
	m_stRKEntryInfo.szFileName = "";
	m_stRKEntryInfo.szSize = "";
	m_stRKEntryInfo.szDate = "";
	m_stRKEntryInfo.szVer = "";
	m_stRKEntryInfo.szOwner = "";
}

void CRKEntryManager::ClearHistoryInfo()
{
	m_stRKHistoryInfo.szOwner = "";
	m_stRKHistoryInfo.szDate = "";
	m_stRKHistoryInfo.szVer = "";
}

void CRKEntryManager::SetEntryParserFileName(CString& szFileName)
{
	m_csRKEntryParser.SetFileName(szFileName);
}

void CRKEntryManager::SetHistoryParserFileName(CString& szFileName)
{
	m_csRKHistoryParser.SetFileName(szFileName);
}

void CRKEntryManager::SetEntryWriterFileName(CString& szFileName)
{
	m_csRKEntryWriter.SetFileName(szFileName);
}

void CRKEntryManager::SetHistoryWriterFileName(CString& szFileName)
{
	m_csRKHistoryWriter.SetFileName(szFileName);
}

void CRKEntryManager::GetFileNameByFilePath(CString& szFilePath, CString& szFileName)
{
	/* 2003.08.13 �̰� ���� �̻��ϰ� �ǹ����� ���� �ּ�ó��.. -0-
	CFileFind csFileFind;
	if(csFileFind.FindFile((LPCTSTR)szFilePath))
	{
		csFileFind.FindNextFile();
		if(csFileFind.IsDirectory())	// ���丮��� �׳� �������ְ� ������.
		{
			szFileName = szFilePath;
			return;
		}
	}
	*/

	int iIndex = szFilePath.ReverseFind(TCHAR('\\'));
	if(iIndex >= 0 && iIndex < szFilePath.GetLength())
		szFileName = szFilePath.Mid(iIndex+1, szFilePath.GetLength() - iIndex);
	else
		szFileName = szFilePath;
}

BOOL CRKEntryManager::GetEntryFileInfo(CString& szFileName, stRKEntryInfo* pstRKEntryInfo)
{
	if(!m_csRKEntryParser.Open())
		return FALSE;

	BOOL bResult = FALSE;

	// �ش� ������ ��Ʈ�� ������ ã�´�.
	if(m_csRKEntryParser.FindFileInfo(szFileName))
	{
		ClearEntryInfo();
		m_csRKEntryParser.GetEntryTokenStruct(m_stRKEntryInfo);

		bResult = TRUE;

		// ���ڷ� �Ѿ�� �� ������ ���� �������ش�.
		if(pstRKEntryInfo)
		{
			pstRKEntryInfo->szType = m_stRKEntryInfo.szType;
			pstRKEntryInfo->szFileName = m_stRKEntryInfo.szFileName;
			pstRKEntryInfo->szSize = m_stRKEntryInfo.szSize;
			pstRKEntryInfo->szDate = m_stRKEntryInfo.szDate;
			pstRKEntryInfo->szVer = m_stRKEntryInfo.szVer;
			pstRKEntryInfo->szOwner = m_stRKEntryInfo.szOwner;
		}
	}

	m_csRKEntryParser.Close();

	return bResult;
}

// �� �Լ��� ȣ���ϱ� ����, m_csRKEntryWriter �� SetFileName �� ���־�� �Ѵ�. �׷����� Open �� ��.
// ���⼭ szFileName �� szOnlyFileName �̴�.
BOOL CRKEntryManager::AddEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer)
{
	RemoveDirectoryFinalSlash(szFileName);

	if(!m_csRKEntryWriter.Open())
		return FALSE;

	BOOL bResult = m_csRKEntryWriter.WriteEntry(szType, szFileName, lFileSize, szDate, lVer);
	m_csRKEntryWriter.Close();

	return bResult;
}


// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::AddEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		CString szType;
		CFileFind csFileFind;
		if(csFileFind.FindFile((LPCTSTR)szFileName))
		{
			csFileFind.FindNextFile();
			szType = csFileFind.IsDirectory() ? "D" : "F";
		}
		else
			szType = "_";

		CString szDate;
		szDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csStatus.m_mtime.GetYear(), csStatus.m_mtime.GetMonth(),
																		csStatus.m_mtime.GetDay(), csStatus.m_mtime.GetHour(),
																		csStatus.m_mtime.GetMinute(), csStatus.m_mtime.GetSecond());
		INT32 lSize = (INT32)csStatus.m_size;
		INT32 lVer = 1;		// ����� ���� ���� 1 �̴�.

		if(m_csRKEntryWriter.Open())
		{
			bResult = m_csRKEntryWriter.WriteEntry(szType, szOnlyFileName, lSize, szDate, lVer);
			m_csRKEntryWriter.Close();
		}
	}

	return bResult;
}

// szLocalFileName, szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::CheckLatestVersion(CString& szLocalFileName, CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szLocalFileName);
	RemoveDirectoryFinalSlash(szFileName);

	CFileStatus csLocalStatus, csRemoteStatus;
	if(CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus) && CFile::GetStatus((LPCTSTR)szFileName, csRemoteStatus))
	{
		// ���丮��� �׳� return TRUE ���ش�. 2004.11.23. steeple
		if(csRemoteStatus.m_attribute & CFile::directory)
		{
			bResult = TRUE;
		}
		else
		{
			// �����̶�� �˻�����.
			if(csLocalStatus.m_size == csRemoteStatus.m_size)	// ������ ����..
			{
				CTimeSpan csSpan = csRemoteStatus.m_mtime - csLocalStatus.m_mtime;
				if(csSpan.GetTotalSeconds() == 0L)	// �����ð��� ����.
				{
					bResult = TRUE;
				}
			}
		}
	}
	
	// 2003.10.21 ����� CheckLatestVersion ����� �ٲ�.
	// M$ ��¥ ����� �й����� ���ؼ�, ��ȣ���� �������� ���ο� ������� �Žñ���.
	/*
	DWORD dwLocalHeadSum, dwOriginHeadSum;
	DWORD dwLocalCheckSum, dwOriginCheckSum;

	char szLocalFileNameTmp[512];
	char szOriginFileNameTmp[512];
	strcpy(szLocalFileNameTmp, (LPCTSTR)szLocalFileName);
	strcpy(szOriginFileNameTmp, (LPCTSTR)szFileName);

	// ���ϵ��� CheckSum �� �о�´�.
	if(MapFileAndCheckSum(szLocalFileNameTmp, &dwLocalHeadSum, &dwLocalCheckSum) == CHECKSUM_SUCCESS &&
		MapFileAndCheckSum(szOriginFileNameTmp, &dwOriginHeadSum, &dwOriginCheckSum) == CHECKSUM_SUCCESS)
	{
		if(dwLocalCheckSum == dwOriginCheckSum)
			bResult = TRUE;
	}
	*/

	return bResult;
}

// szLocalFileName, szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::CheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	// ���� �ֽ� �������� Ȯ���Ѵ�.
	if(CheckLatestVersion(szLocalFileName, szFileName) == FALSE)
		return FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// ������ ���õ� ���� �д´�.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// ���� �̸��� �°�, ������ �����ڰ� ���ٸ�..
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			m_stRKEntryInfo.szOwner.GetLength() == 0)
		{
			// üũ�ƿ� ����!!
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, 0, CString(""), szOwner);	// Size ��, ��¥�� ���⿡�� �ʿ�����Ƿ�.
				m_csRKEntryWriter.Close();
			}

			if(bResult)	// CheckOut ���������� LocalFile �� ReadOnly Ǯ���ش�. 2003.10.08 ������
				SetReadOnly(szLocalFileName, FALSE);
		}
	}

	return bResult;
}

// szLocalFileName, szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::CheckIn(CString& szLocalFileName, CString& szFileName, CString& szOwner, BOOL bRealCopy)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// ������ ���õ� ���� �д´�.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// ���� �̸��� �°�, ������ �����ڿ� ���ٸ�!!!
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
//			m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0)
			(m_stRKEntryInfo.szOwner.IsEmpty() || m_stRKEntryInfo.szOwner.CompareNoCase(LPCTSTR(szOwner)) == 0))	// Lock�� �ɾ���� ����ڰ� ����� ��
		{
			INT32 lVer = atoi((LPCTSTR)m_stRKEntryInfo.szVer);

			CString szNewDate;
			INT32 lSize = 0;

			CFileStatus csLocalStatus;
			if(CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus))
			{
				szNewDate.Format("%04d-%02d-%02d %02d:%02d:%02d", csLocalStatus.m_mtime.GetYear(), csLocalStatus.m_mtime.GetMonth(),
																				csLocalStatus.m_mtime.GetDay(), csLocalStatus.m_mtime.GetHour(),
																				csLocalStatus.m_mtime.GetMinute(), csLocalStatus.m_mtime.GetSecond());
				lSize = (INT32)csLocalStatus.m_size;
			}
			else
			{
				// ���� ���� �� ������ ���δ�.. -0-
				OutputDebugString("CRKEntryManager::CheckIn(...) => CFile::GetStatus((LPCTSTR)szLocalFileName, csLocalStatus) Failed!!!!\n");
				return FALSE;
			}

			// ������ ������ �ø���. (�ڿ� '_' �� �ٿ��� �ø���.)
			CString szTmpFileName = szFileName + "_";
			if(bRealCopy)
				CopyFile((LPCTSTR)szLocalFileName, (LPCTSTR)szTmpFileName, FALSE);	// Overwrite

			// üũ�� ����!!
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, CString(""), lVer+1);	// �������� ��¥�� ���

				if(bResult)
				{
					// BackUp ���ش�.
					BackupFile(szFileName, lVer);

					// History ���ش�.
					HistoryFile(szFileName, szNewDate, szOwner, lVer+1);	// ���ο� ���Ϸ� History ���ش�. 2004.11.25.
					
					m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, CString(""), lVer+1);	// �������� ��¥�� ���

					// ���� Lock ������ ������ - 2004.1.5. ������
					m_csRKEntryWriter.WriteOwner(szOnlyFileName, lSize, szNewDate, m_stRKEntryInfo.szOwner, lVer+1);	
				}

				m_csRKEntryWriter.Close();
			}

			if(bRealCopy)
			{
				// ���� ������ �����. �Ƹ� ReadOnly �� ���̴�.
				RemoveFile(szFileName);

				// �ø� ������ �̸��� �ٲ��ش�.
				MoveFileEx((LPCTSTR)szTmpFileName, (LPCTSTR)szFileName, MOVEFILE_REPLACE_EXISTING);
			}

			// ReadOnly �� ���ش�.
//			SetReadOnly(szLocalFileName);
//			SetReadOnly(szFileName);
		}
	}

	return bResult;
}

// szLocalFileName, szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::UndoCheckOut(CString& szLocalFileName, CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// ������ ���õ� ���� �д´�.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// ���� �̸��� �°�, szOwner �� Check-Out ���̾��ٸ�!!!
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0)
		{
			if(m_csRKEntryWriter.Open())
			{
				// �Ʒ� �Լ� ȣ���� �� ���ڿ� ���� �Ѵ�.
				bResult = m_csRKEntryWriter.WriteOwner(szOnlyFileName, atoi((LPCTSTR)m_stRKEntryInfo.szSize), m_stRKEntryInfo.szDate,
																				CString(""), atoi((LPCTSTR)m_stRKEntryInfo.szVer));
				m_csRKEntryWriter.Close();
			}
/*
			if(bResult)
				if(::AfxMessageBox(CONFIRM_REPLACE_FILE, MB_YESNO) == IDYES)
					CopyFile((LPCTSTR)szFileName, (LPCTSTR)szLocalFileName, FALSE);	// Overwrite
*/
		}
	}

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::RemoveEntry(CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	CString szType;
	CFileFind csFileFind;
	if(csFileFind.FindFile((LPCTSTR)szFileName))
	{
		csFileFind.FindNextFile();
		szType = csFileFind.IsDirectory() ? "D" : "F";
	}

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// ������ ���õ� ���� �д´�.
	if(GetEntryFileInfo(szOnlyFileName))
	{
		// ���� �̸� �°�, ���� �����ְ� ���� �� �����̴�.
		if(m_stRKEntryInfo.szFileName.CompareNoCase((LPCTSTR)szOnlyFileName) == 0 &&
			// ���� üũ�ƿ� ���̰ų�, �����ڰ� ���� �� �����ϴ�.
			(m_stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) == 0 || m_stRKEntryInfo.szOwner.GetLength() == 0))
		{
			if(m_csRKEntryWriter.Open())
			{
				bResult = m_csRKEntryWriter.RemoveEntry(szType, szOnlyFileName);
				m_csRKEntryWriter.Close();
			}

			if(bResult)
			{
				// ��� ���� ���� �����..
				RemoveBackupEntry(szFileName);

				// ������ �����.
				if(!DeleteFile((LPCTSTR)szFileName))
					RemoveFile(szFileName);
			}
		}
	}

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::RemoveDirectoryEntry(CString& szFileName, CString& szOwner)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	// ���� ���丮�� ���鼭 �ӽ� Check-Out �� �Ǵ�.
	bResult = TemporaryCheckOut(szFileName, szOwner);

	// �����ϸ� ���丮�� ������~
	if(bResult)
	{
		// ������~
		RemoveDirectoryNAllFiles(szFileName);

		int iIndex = szFileName.ReverseFind('\\');
		CString szUpDirectoryEntryFileName = szFileName.Left(iIndex + 1) + ENTRY_FILENAME;
		CString szOnlyName = szFileName.Right(szFileName.GetLength() - iIndex - 1);
		
		CRKWriter csRKWriter;
		csRKWriter.SetFileName(szUpDirectoryEntryFileName);
		if(csRKWriter.Open())
		{
			bResult = csRKWriter.RemoveEntry(CString("D"), szOnlyName);
			csRKWriter.Close();
		}

		if(bResult)
		{
			// ��� ���� ���� ���� ��
			RemoveBackupDirectoryEntry(szFileName);

			// ������ �����.
			if(!RemoveDirectory((LPCTSTR)szFileName))
				RemoveFile(szFileName);
		}
	}
	// �����ϸ� �ӽ� Check-Out �� �Ÿ� Ǯ���ش�.
	else
	{
		UndoTemporaryCheckOut(szFileName, szOwner);
	}

	return bResult;
}

// ���� ���丮�� ���鼭 ��� ���Ͽ� �ӽ� Check-Out �� �Ǵ�.
// �ϳ��� �����ϸ� �ٷ� FALSE �� �����Ѵ�.
BOOL CRKEntryManager::TemporaryCheckOut(CString& szFileName, CString& szOwner)
{
	BOOL bResult = TRUE;

	CString szFindFileName = szFileName + "\\*.*";

	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFindFileName);
	if(!bWorking)
		return FALSE;

	CString szOnlyFileName;
	CString szEntryFileName;
	CRKParser csRKParser;
	CRKWriter csRKWriter;
	CList<stRKEntryInfo, stRKEntryInfo> csEntryList;

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())	// . .. �� continue
			continue;

		if(csFileFind.IsDirectory())	// ���丮���, ��� ����.
		{
			bResult = TemporaryCheckOut(csFileFind.GetFilePath(), szOwner);
			if(!bResult)	// �̰� �����ϸ� �ٷ� �����̴�.
				return FALSE;

			continue;
		}

		szOnlyFileName = csFileFind.GetFileName();	// �ش� ������ �̸��� ���..

		// Entries.rk ������ �д´�.
		if(szOnlyFileName.CompareNoCase(ENTRY_FILENAME) == 0)
		{
			szEntryFileName = szFileName + "\\" + ENTRY_FILENAME;
			csRKParser.SetFileName(szEntryFileName);
			csRKWriter.SetFileName(szEntryFileName);

			// ������ Entry ����Ʈ�� ����.
			csEntryList.RemoveAll();
			if(csRKParser.Open())
			{
				// ��Ʈ�� ������ List �� �ִ´�.
				while(csRKParser.NewLine())
				{
					stRKEntryInfo stRKEntryInfo;
					csRKParser.GetEntryTokenStruct(stRKEntryInfo);
					csEntryList.AddTail(stRKEntryInfo);
				}
				csRKParser.Close();
			}

			int iCount = csEntryList.GetCount();
			POSITION p = csEntryList.GetHeadPosition();
			for(int i = 0; i < iCount; i++)
			{
				stRKEntryInfo stRKEntryInfo = csEntryList.GetNext(p);

				// �ش� ������ ������ �м��Ѵ�.
				if(stRKEntryInfo.szOwner.GetLength() != 0 &&
					stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) != 0)	// ������ üũ�ƿ� �ϰ� �ִ� �����̶�� �ߴ�!!!
					return FALSE;	// �ٷ� ����

				if(csRKWriter.Open())
				{
					bResult = csRKWriter.WriteOwner(stRKEntryInfo.szFileName, atoi((LPCTSTR)stRKEntryInfo.szSize),
														stRKEntryInfo.szDate, szOwner, atoi((LPCTSTR)stRKEntryInfo.szVer));
					csRKWriter.Close();
				}

				if(!bResult)	// ���� �����ϸ� �ٷ� ������.
					return FALSE;
			}
		}	// Entries.rk ���� ����
	}	// while(bWorking)

	return bResult;
}

// ���� ���丮�� ���鼭 �ӽ÷� Check-Out �ɷ��ִ� ��� ������ Ǭ��.
// �߰��� �����ϴ��� ������ ����.
BOOL CRKEntryManager::UndoTemporaryCheckOut(CString& szFileName, CString& szOwner)
{
	BOOL bResult = TRUE;

	CString szFindFileName = szFileName + "\\*.*";

	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFindFileName);
	if(!bWorking)
		return FALSE;

	CString szOnlyFileName;
	CString szEntryFileName;
	CRKParser csRKParser;
	CRKWriter csRKWriter;
	CList<stRKEntryInfo, stRKEntryInfo> csEntryList;

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())	// . .. �� continue
			continue;

		if(csFileFind.IsDirectory())	// ���丮���, ��� ����.
		{
			UndoTemporaryCheckOut(csFileFind.GetFilePath(), szOwner);
			continue;
		}

		szOnlyFileName = csFileFind.GetFileName();	// �ش� ������ �̸��� ���..

		// Entries.rk ������ �д´�.
		if(szOnlyFileName.CompareNoCase(ENTRY_FILENAME) == 0)
		{
			szEntryFileName = szFileName + "\\" + ENTRY_FILENAME;
			csRKParser.SetFileName(szEntryFileName);
			csRKWriter.SetFileName(szEntryFileName);

			// ������ Entry ����Ʈ�� ����.
			csEntryList.RemoveAll();
			if(csRKParser.Open())
			{
				// ��Ʈ�� ������ List �� �ִ´�.
				while(csRKParser.NewLine())
				{
					stRKEntryInfo stRKEntryInfo;
					csRKParser.GetEntryTokenStruct(stRKEntryInfo);
					csEntryList.AddTail(stRKEntryInfo);
				}
				csRKParser.Close();
			}

			int iCount = csEntryList.GetCount();
			POSITION p = csEntryList.GetHeadPosition();
			for(int i = 0; i < iCount; i++)
			{
				stRKEntryInfo stRKEntryInfo = csEntryList.GetNext(p);

				// �ش� ������ ������ �м��Ѵ�.
				if(stRKEntryInfo.szOwner.GetLength() != 0 &&
					stRKEntryInfo.szOwner.CompareNoCase((LPCTSTR)szOwner) != 0)	// �ٸ� ����� Check-Out �ϰ� ������ Skip
					continue;

				if(csRKWriter.Open())
				{
					bResult = csRKWriter.WriteOwner(stRKEntryInfo.szFileName, atoi((LPCTSTR)stRKEntryInfo.szSize),
														stRKEntryInfo.szDate, CString(""), atoi((LPCTSTR)stRKEntryInfo.szVer));
					csRKWriter.Close();
				}
			}
		}	// Entries.rk ���� ����
	}	// while(bWorking)

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
// Entries.rk �� ��ü ��ΰ� ���� ���̴�.
BOOL CRKEntryManager::GetEntryList(CString& szFileName, CList<stRKEntryInfo, stRKEntryInfo>& csList)
{
	BOOL bResult = FALSE;

	m_csRKEntryParser.SetFileName(szFileName);
	if(m_csRKEntryParser.Open())
	{
		while(m_csRKEntryParser.NewLine())
		{
			stRKEntryInfo stRKEntryInfo;
			m_csRKEntryParser.GetEntryTokenStruct(stRKEntryInfo);
			csList.AddTail(stRKEntryInfo);
		}

		m_csRKEntryParser.Close();
		bResult = TRUE;
	}

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::GetHistoryList(CString& szFileName, CList<stRKHistoryInfo, stRKHistoryInfo>& csList)
{
	BOOL bResult = FALSE;

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	// ���� ������ �ִ´�.
	/*
	CString szEntryFileName;
	GetDirectoryPath(szFileName, szEntryFileName);
	szEntryFileName += "\\"ENTRY_FILENAME;
	m_csRKEntryParser.SetFileName(szEntryFileName);

	stRKEntryInfo stRKEntryInfo;
	if(GetEntryFileInfo(szOnlyFileName, &stRKEntryInfo))
	{
		stRKHistoryInfo stHistoryInfo;
		stHistoryInfo.szDate = stRKEntryInfo.szDate;
		stHistoryInfo.szOwner = stRKEntryInfo.szOwner;
		stHistoryInfo.szVer = stRKEntryInfo.szVer;

		csList.AddTail(stHistoryInfo);

		bResult = TRUE;
	}
	else
	{
		// ���� ������ �� ������� ������.
		return FALSE;
	}
	*/

	// �����丮���� ã�Ƽ� �ִ´�.
	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CString szHistoryFileName;
	szHistoryFileName.Format("%s_%s", szOnlyFileName, HISTORY_SUBNAME);
	szTargetPath += "\\" + szHistoryFileName;

	m_csRKHistoryParser.SetFileName(szTargetPath);
	if(m_csRKHistoryParser.Open())
	{
		while(m_csRKHistoryParser.NewLine())
		{
			stRKHistoryInfo stRKHistoryInfo;
			m_csRKHistoryParser.GetHistoryTokenStruct(stRKHistoryInfo);
			csList.AddTail(stRKHistoryInfo);
		}
		m_csRKHistoryParser.Close();
		bResult = TRUE;
	}

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
CString CRKEntryManager::GetBackupFilePath(CString& szFileName, INT32 lVer)
{
	CString szNewPath;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	GetBackupFilePathByFileName(szFileName, szNewPath);

	CString szVer;
	szVer.Format("%d", lVer);
	szNewPath += CString("\\") + szOnlyFileName + CString("_") + szVer + CString("_") + CString(BACKUP_SUBNAME);

	return szNewPath;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::BackupFile(CString& szFileName, INT32 lVer)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CreateSubDirectory(szTargetPath);

	CString szBackupFileName;
	szBackupFileName.Format("%s_%d_%s", szOnlyFileName, lVer, BACKUP_SUBNAME);
	szTargetPath += "\\" + szBackupFileName;

	bResult = CopyFile((LPCTSTR)szFileName, (LPCTSTR)szTargetPath, FALSE);	// Overwrite

	return bResult;
}

// szFileName ���� ������ PATH �� �Ѿ�;� �ȴ�.
BOOL CRKEntryManager::HistoryFile(CString szFileName, CString szDate, CString szOwner, INT32 lVer)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	CreateSubDirectory(szTargetPath);

	CString szHistoryFileName;
	szHistoryFileName.Format("%s_%s", szOnlyFileName, HISTORY_SUBNAME);
	szTargetPath += "\\" + szHistoryFileName;

	m_csRKHistoryWriter.SetFileName(szTargetPath);
	if(m_csRKHistoryWriter.Open())
	{
		m_csRKHistoryWriter.WriteHistory(szOnlyFileName, szDate, szOwner, lVer);
		m_csRKHistoryWriter.Close();

		bResult = TRUE;
	}

	return bResult;
}

BOOL CRKEntryManager::RemoveBackupEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szOnlyFileName;
	GetFileNameByFilePath(szFileName, szOnlyFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	szTargetPath += "\\*.*";
	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile((LPCTSTR)szTargetPath);

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())
			continue;

		if(csFileFind.IsDirectory())
			continue;

		// �ش� ������ ��� �����̶��...
		if(csFileFind.GetFileName().Left(szOnlyFileName.GetLength()).CompareNoCase((LPCTSTR)szOnlyFileName) == 0)
		{
			// ������~
			if(!DeleteFile((LPCTSTR)csFileFind.GetFilePath()))
				RemoveFile(csFileFind.GetFilePath());

			bResult = TRUE;
		}
	}

	return bResult;
}

BOOL CRKEntryManager::RemoveBackupDirectoryEntry(CString& szFileName)
{
	BOOL bResult = FALSE;

	RemoveDirectoryFinalSlash(szFileName);

	CString szTargetPath;
	GetBackupFilePathByFileName(szFileName, szTargetPath);

	RemoveDirectoryNAllFiles(szTargetPath);

	bResult = RemoveDirectory(szTargetPath);
	if(!bResult)
	{
		RemoveFile(szTargetPath);
		bResult = TRUE;
	}

	return bResult;
}

// szFileName ���� ���� PATH �� �´�. - ���丮�� �� ���� �ְ�, ���� �̸� Ǯ ��ΰ� �� ���� �ִ�.
// szNewPath ���� PATH �� �Ѿ��. 
void CRKEntryManager::GetBackupFilePathByFileName(CString& szFileName, CString& szNewPath)
{
	szNewPath = szFileName;
	int iIndex = 0;

	CFileFind csFileFind;
	if(csFileFind.FindFile(szNewPath))
	{
		csFileFind.FindNextFile();
		if(!csFileFind.IsDirectory())	// �����̶��...
		{
			iIndex = szNewPath.ReverseFind(TCHAR('\\'));
			if(iIndex > 0)
				szNewPath.Delete(iIndex, szNewPath.GetLength() - iIndex);
		}
	}

	// Root ���丮������ Find �� �ȵǴ� ��찡 �߻��ؼ� ��� ����!!
	if(szNewPath.GetLength() == 0) return;
	if(szNewPath.GetAt(szNewPath.GetLength() - 1) != '\\')
		szNewPath += "\\";

	CString szReplace1 = "\\" + CString(REMOTE_ROOT_PATH) + "\\";
	CString szReplace2 = "\\" + CString(REMOTE_BACKUP_PATH) + "\\";

	iIndex = szNewPath.Find(szReplace1);
	szNewPath.Delete(iIndex, szReplace1.GetLength());
	szNewPath.Insert(iIndex, (LPCTSTR)szReplace2);

	// �ѹ� �� �����ش�.
	RemoveDirectoryFinalSlash(szNewPath);
}

// �б� ���� �����̶��, Ǯ�� �����.
void CRKEntryManager::RemoveFile(CString& szFileName)
{
	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		if(csStatus.m_attribute & CFile::readOnly)
		{
			OutputDebugString("RemoveFile ���� �б� ���� Ǯ�� ����\n");

			csStatus.m_mtime = 0;	// ���� �ð����� ��ȭ�� ���� �ʴ´�.
			csStatus.m_attribute &= ~CFile::readOnly;

			if(csStatus.m_attribute & CFile::directory)
			{
				Sleep(0);
				CFile::SetStatus((LPCTSTR)szFileName, csStatus);
				RemoveDirectory((LPCTSTR)szFileName);
			}
			else
			{
				CFile::SetStatus((LPCTSTR)szFileName, csStatus);
				DeleteFile((LPCTSTR)szFileName);
			}
		}
	}
}

void CRKEntryManager::RemoveDirectoryNAllFiles(CString& szDirectoryName)
{
	RemoveDirectoryFinalSlash(szDirectoryName);

	CString szFind = szDirectoryName + "\\*.*";
	CFileFind csFileFind;
	BOOL bWorking = csFileFind.FindFile(szFind);

	while(bWorking)
	{
		bWorking = csFileFind.FindNextFile();
		if(csFileFind.IsDots())
			continue;

		if(csFileFind.IsDirectory())
		{
			RemoveDirectoryNAllFiles(csFileFind.GetFilePath());

			Sleep(0);	// �̰� ���ָ�.. ���丮�� ���������� ��찡 �ִ�.

			BOOL bResult = RemoveDirectory((LPCTSTR)csFileFind.GetFilePath());
			if(!bResult)
			{
				RemoveFile(csFileFind.GetFilePath());
				bResult = TRUE;
			}

			continue;
		}

		if(!DeleteFile((LPCTSTR)csFileFind.GetFilePath()))
			RemoveFile(csFileFind.GetFilePath());
	}
}

void CRKEntryManager::RemoveDirectoryFinalSlash(CString& szFileName)
{
	if(szFileName.GetLength() == 0) return;

	if(szFileName.GetAt(szFileName.GetLength() - 1) == '\\')
		szFileName = szFileName.Left(szFileName.GetLength() - 1);
}

void CRKEntryManager::CreateSubDirectory(CString& szPath)
{
	int iIndex = 0;
	if(szPath.GetAt(0) == '\\')
		iIndex = szPath.Find("\\", 2);
	else
		iIndex = szPath.Find("\\", 0);

	while((iIndex = szPath.Find("\\", iIndex + 1)) >= 0)
	{
		if(_access((LPCTSTR)szPath.Left(iIndex), 0) == -1)
		{
			CreateDirectory(szPath.Left(iIndex), NULL);
		}
	}

	// Full Path �� �˻�
	if(_access((LPCTSTR)szPath, 0) == -1)
		CreateDirectory(szPath, NULL);
}

void CRKEntryManager::GetDirectoryPath(CString& szFileName, CString& szPath)
{
	if(szFileName.IsEmpty())
		return;

	RemoveDirectoryFinalSlash(szFileName);

	INT32 iPos = szFileName.ReverseFind('\\');
	szPath = szFileName.Left(iPos);
}

void CRKEntryManager::SetReadOnly(CString& szFileName, BOOL bFlag)
{
	CFileStatus csStatus;
	if(CFile::GetStatus((LPCTSTR)szFileName, csStatus))
	{
		csStatus.m_mtime = 0;	// ���� �ð����� ��ȭ�� ���� �ʴ´�.

		if(bFlag)
			csStatus.m_attribute |= CFile::readOnly;	// ReadOnly �ش�.
		else
			csStatus.m_attribute &= ~CFile::readOnly;	// ReadOnly Ǭ��.			

		if(csStatus.m_attribute & CFile::directory)
			Sleep(0);

		CFile::SetStatus((LPCTSTR)szFileName, csStatus);
	}
}

BOOL CRKEntryManager::SetModifyTime(CString& szOriginFileName, CString& szTargetFileName)
{
	BOOL bResult = FALSE;
	CFileStatus csOriginStatus, csTargetStatus;
	if(CFile::GetStatus((LPCTSTR)szOriginFileName, csOriginStatus) && CFile::GetStatus((LPCTSTR)szTargetFileName, csTargetStatus))
	{
		csTargetStatus.m_mtime = csOriginStatus.m_mtime;
		CFile::SetStatus((LPCTSTR)szTargetFileName, csTargetStatus);

		bResult = TRUE;
	}

	return bResult;
}