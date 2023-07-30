// RKWriter.cpp
// 2003.07.29 steeple

#include "stdafx.h"
#include "RKInclude.h"

CRKWriter::CRKWriter()
{
	Init();
}

CRKWriter::~CRKWriter()
{
	Close();
}

void CRKWriter::Init()
{
	m_bOpened = FALSE;

	m_szFileName = "";
}

BOOL CRKWriter::Open()
{
	if(m_szFileName.GetLength() == 0)
		return FALSE;

	CFileFind cFileFind;
	if(cFileFind.FindFile(m_szFileName))
	{
		if(m_csFile.Open((LPCTSTR)m_szFileName, CFile::modeReadWrite | CFile::shareDenyRead | CFile::shareDenyWrite))
		{
			m_bOpened = TRUE;
		}
	}
	else
	{
		if(m_csFile.Open((LPCTSTR)m_szFileName, CFile::modeCreate | CFile::modeReadWrite)) // | CFile::typeText))// |
																		//CFile::shareDenyRead | CFile::shareDenyWrite))
		{
			m_bOpened = TRUE;

			/* 2003.08.13. ���� ������ �� ������.. -0-  �׷��� �ּ� ó��
			// Hidden �ش�.
			CFileStatus csStatus;
			if(m_csFile.GetStatus(csStatus))
			{
				csStatus.m_attribute |= CFile::hidden;
				m_csFile.SetStatus((LPCTSTR)m_szFileName, csStatus);
			}
			*/
		}
	}

	return m_bOpened;
}

BOOL CRKWriter::Close()
{
	if(m_bOpened)
	{
		m_csFile.Close();
		m_bOpened = FALSE;
	}

	return TRUE;
}

BOOL CRKWriter::WriteEntry(CString& szType, CString& szFileName, INT32 lFileSize, CString& szDate, INT32 lVer)
{
	if(!m_bOpened)
		return FALSE;

	CString szLine;
	CString szFileSize;
	CString szVer;

	szFileSize.Format("%d", lFileSize);
	szVer.Format("%d", lVer);

	szLine = szType + "/" + szFileName + "/" + szFileSize + "/" + szDate + "/" + szVer + "/" + "/";
	szLine += "\r\n";

	m_csFile.SeekToEnd();
	m_csFile.WriteString(szLine);
	m_csFile.Flush();

	return TRUE;
}

BOOL CRKWriter::WriteOwner(CString& szFileName, INT32 lSize, CString& szDate, CString& szOwner, INT32 lVer)
{
	if(!m_bOpened)
		return FALSE;

	CString szTotal;
	CString szLine;

	int iStart = 0;	// �ι�° ������ ���� ��ġ
	int iEnd = 0;
	BOOL bFind = FALSE;

	m_csFile.SeekToBegin();

	while(m_csFile.ReadString(szLine))
	{
		iStart = szLine.Find(BRANCH_KEY, 0) + 1;
		for(int i = iStart; i < szLine.GetLength(); i++)
		{
			if(szLine.GetAt(i) == BRANCH_KEY)
			{
				// ���� �̸��� ������..
				if(szFileName.CompareNoCase((LPCTSTR)szLine.Mid(iStart, i - iStart)) == 0)
				{
					bFind = TRUE;
					break;
				}
			}
		}

		if(bFind)
			break;

		szTotal += szLine + "\n";
	}

	if(!bFind)	// �� ã������ ������.
		return FALSE;

	int iLength = szLine.GetLength();

	// Check-Out
	if(szOwner.GetLength() != 0)
	{
		// �����ڸ� �ٲ��ش�.
		iStart = iEnd = 0;
		for(int i = iLength - 1; i >= 0; i--)
		{
			if(szLine.GetAt(i) == BRANCH_KEY && iEnd == 0)
			{
				iEnd = i;	// ������ '/' ��ġ
				continue;
			}

			if(szLine.GetAt(i) == BRANCH_KEY && iEnd != 0)
			{
				iStart = i;	// ���������� �ι�° '/' ��ġ
				break;
			}
		}

		if(iStart + 1 != iEnd)	// ������ �ƴ϶��, �����ش�.
			szLine.Delete(iStart + 1, iEnd - iStart - 1);

		szLine.Insert(iStart + 1, szOwner);
	}
	// Check-In, Undo Check-Out
	else
	{
		char cType = szLine.GetAt(0);
		szLine.Empty();
		szLine.Format("%C/%s/%d/%s/%d//", cType, szFileName, lSize, szDate, lVer);
		szLine += "\r";
	}

	// ��ü ��Ʈ���� ���� ��
	szTotal += szLine + "\n";

	while(m_csFile.ReadString(szLine))
	{
		// ������ ��Ʈ����, ��ü ��Ʈ���� �ٿ��ش�.
		szTotal += szLine + "\n";
	}

	// �� �� �ѹ濡 ����.
	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.Write(szTotal, szTotal.GetLength());
	m_csFile.Flush();

	return TRUE;
}

BOOL CRKWriter::WriteHistory(CString szFileName, CString szDate, CString szOwner, INT32 lVer)
{
	if(!m_bOpened)
		return FALSE;

	CString szTotal;
	CString szLine;
	CString szTmp;
	CString szVer;

	szVer.Format("%d", lVer);

	szLine = szOwner + "/" + szDate + "/" + szVer + "/";
	szLine += "\r\n";

	if(m_csFile.GetLength() > 0)
	{
		while(m_csFile.ReadString(szTmp))
		{
			szTotal += szTmp + "\n";
		}
	}
	szTotal.Insert(0, (LPCTSTR)szLine);

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return TRUE;
}

BOOL CRKWriter::RemoveEntry(CString& szType, CString& szFileName)
{
	if(!m_bOpened)
		return FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 2);	// ���� szType �� �ǳ� �ڴ�.
		if(szLine.Mid(2, iIndex - 2).CompareNoCase((LPCTSTR)szFileName) == 0 &&
			szLine.Left(1).CompareNoCase((LPCTSTR)szType) == 0)
		{
			continue;
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return TRUE;
}

BOOL CRKWriter::WriteRepository(CString& szName, CString& szPath, CString& szWorkingFolder, INT8 cDefault)
{
	if(!m_bOpened)
		return FALSE;

	CString szLine;
	CString szDefault;

	szDefault.Format("%d", cDefault);

	szLine = szName + "/" + szPath + "/" + szWorkingFolder + "/" + szDefault + "/";
	szLine += "\r\n";

	m_csFile.SeekToEnd();
	m_csFile.WriteString(szLine);
	m_csFile.Flush();

	return TRUE;
}

BOOL CRKWriter::RemoveRepository(CString& szName)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 0);
		if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)
		{
			bResult = TRUE;
			continue;
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}

BOOL CRKWriter::WriteRemotePath(CString& szName, CString& szPath)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0, iSecondIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 0);
		if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)	// �߰�!!!
		{
			iSecondIndex = szLine.Find("/", iIndex + 1);

			szLine.Delete(iIndex+1, iSecondIndex - iIndex - 1);
			szLine.Insert(iIndex+1, szPath);
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}

BOOL CRKWriter::WriteWorkingFolder(CString& szName, CString& szWorkingFolder)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0, iSecondIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 0);
		if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)	// �߰�!!!
		{
			iIndex = szLine.Find("/", iIndex + 1);
			iSecondIndex = szLine.Find("/", iIndex + 1);

			szLine.Delete(iIndex+1, iSecondIndex - iIndex - 1);
			szLine.Insert(iIndex+1, szWorkingFolder);
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}

BOOL CRKWriter::WriteRepositoryDefault(CString& szName, BOOL bDefault)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 0);
		if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)
		{
			bResult = TRUE;

			if(bDefault)
				szLine.Replace("/0/", "/1/");
			else
				szLine.Replace("/1/", "/0/");
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}

BOOL CRKWriter::WriteUser(CString& szName, CString& szPassword, BOOL bAdmin, BOOL bAdd)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0;
	if(bAdd)
	{
		int iSecondIndex = 0;
		while(m_csFile.ReadString(szLine))
		{
			iIndex = szLine.Find("/", 0);
			if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)
			{
				// ��ȣ�����̳� Admin ������.
				bResult = TRUE;

				// ���� �ۼ����ش�.
				szLine.Format("%s/%d/%s/", szName, bAdmin, szPassword);
				szLine += "\r";
			}

			szTotal += szLine + "\n";
		}

		// ���ٸ� ���� �߰�~
		if(!bResult)
		{
			szLine.Format("%s/%d/%s/", szName, bAdmin, szPassword);
			szTotal += szLine + "\r\n";

			bResult = TRUE;
		}
	}
	else
	{
		while(m_csFile.ReadString(szLine))
		{
			iIndex = szLine.Find("/", 0);
			if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)
			{
				bResult = TRUE;
				continue;	// �߰����� �ʴ´�.
			}

			szTotal += szLine + "\n";
		}
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}

BOOL CRKWriter::WriteAdmin(CString& szName, BOOL bAdmin)
{
	if(!m_bOpened)
		return FALSE;

	BOOL bResult = FALSE;

	CString szTotal;
	CString szLine;

	int iIndex = 0;
	int iSecondIndex = 0;
	while(m_csFile.ReadString(szLine))
	{
		iIndex = szLine.Find("/", 0);
		if(szName.CompareNoCase(szLine.Left(iIndex)) == 0)
		{
			iSecondIndex = szLine.Find("/", iIndex+1);

			CString szPassword = szLine.Mid(iSecondIndex+1);
			szLine.Format("%s/%d/", szName, bAdmin);
			szLine += szPassword;

			bResult = TRUE;
		}

		szTotal += szLine + "\n";
	}

	m_csFile.SetLength(szTotal.GetLength());
	m_csFile.SeekToBegin();
	m_csFile.WriteString(szTotal);
	m_csFile.Flush();

	return bResult;
}