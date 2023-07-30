// RKParser.cpp
// 2003.07.29 steeple

#include "stdafx.h"
#include "RKInclude.h"

CRKParser::CRKParser()
{
	Init();
}

CRKParser::~CRKParser()
{
	Close();
}

void CRKParser::Init()
{
	m_bOpened = FALSE;

	m_szFileName = "";
	m_szNowLine = "";
	for(int i = 0; i < MAX_TOKEN; i++)
		m_szToken[i] = "";

	m_cNumToken = m_cNowToken = 0;
	m_lNowLine = m_lTotalLine = 0;
}

void CRKParser::ClearParseData()
{
	m_cNowToken = m_cNumToken = 0;
	for(int i = 0; i < MAX_TOKEN; i++)
		m_szToken[i] = "";

	m_szNowLine = "";
}

BOOL CRKParser::Open()
{
	if(m_szFileName.GetLength() == 0)
		return FALSE;

	if(m_csFile.Open((LPCTSTR)m_szFileName, CFile::modeRead | CFile::shareDenyRead | CFile::shareDenyWrite))
	{
		m_bOpened = TRUE;
		return TRUE;
	}
	else
		return FALSE;
}

BOOL CRKParser::Close()
{
	// Close �� ���ϰ��� ������ TRUE ��..
	if(m_bOpened)
	{
		m_csFile.Close();
		m_bOpened = FALSE;
	}

	return TRUE;
}

BOOL CRKParser::NewLine()
{
	if(m_bOpened == FALSE)
		return FALSE;

	if(m_csFile.GetLength() <= 0)
		return FALSE;

	ClearParseData();

	if(!m_csFile.ReadString(m_szNowLine))
		return FALSE;

	ReadToken();

	return TRUE;
}

BOOL CRKParser::FindFileInfo(CString& szFileName)
{
	if(m_bOpened == FALSE)
		return FALSE;

	if(szFileName.GetLength() <= 0 || szFileName.GetLength() >= LINE_BUFFER)
		return FALSE;

	BOOL bResult = FALSE;
	CString* pcStringTmp = NULL;
	while(NewLine())
	{
		pcStringTmp = GetToken(1);
		if(pcStringTmp)
		{
			int i = pcStringTmp->GetLength();
			int j = szFileName.GetLength();

			// ������..
			if(pcStringTmp->CompareNoCase((LPCTSTR)szFileName) == 0)
			{
				bResult = TRUE;
				break;
			}
		}
	}

	return bResult;
}

BOOL CRKParser::ReadToken()
{
	if(m_szNowLine.GetLength() <= 0)
		return FALSE;

	int iStart, iEnd, iIndex;
	iStart = iEnd = iIndex = 0;
	int iLength = m_szNowLine.GetLength();
	for(int i = 0; i < m_szNowLine.GetLength(); i++)
	{
		if(m_szNowLine.GetAt(i) == BRANCH_KEY)
		{
			if(i != iStart)	// �Ϲ����� ���
				m_szToken[iIndex++] = m_szNowLine.Mid(iStart, i - iStart);
			else				// �ƹ��͵� ��ģ��� ///// �̷�������..
				m_szToken[iIndex++] = "";

			iStart = i + 1;

			// ������ ���� ó��
			if(iStart == iLength - 1)
			{
				m_szToken[iIndex++] = "";
				break;
			}
		}
	}

	m_cNumToken = iIndex;

	return TRUE;
}

// cIndex �� zero-base �̴�.
CString* CRKParser::GetToken(INT8 cIndex)
{
	if(m_bOpened == FALSE)
		return (CString*)NULL;

	if(cIndex >= 0 && cIndex < m_cNumToken)
		return &m_szToken[cIndex];
	else
		return (CString*)NULL;
}

CString* CRKParser::GetPassword()
{
	if(m_bOpened == FALSE)
		return (CString*)NULL;

	if(m_cNumToken < 3)
		return (CString*)NULL;

	int iIndex = m_szNowLine.Find("/", 0);
	iIndex = m_szNowLine.Find("/", iIndex+1);
	m_szToken[2] = m_szNowLine.Mid(iIndex+1, m_szNowLine.GetLength() - iIndex - 3);	// �ڿ� \n ���� �پ��ִٺ��� ���� ����.
	return &m_szToken[2];
}

// Entry ������ ���� ��, ������ ���� �� ����.
void CRKParser::GetEntryTokenStruct(struct stRKEntryInfo& stRKEntryInfo)
{
	if(m_bOpened == FALSE)
		return;

	if(m_cNumToken <= 0)
		return;

	stRKEntryInfo.szType = m_szToken[0];
	stRKEntryInfo.szFileName = m_szToken[1];
	stRKEntryInfo.szSize = m_szToken[2];
	stRKEntryInfo.szDate = m_szToken[3];
	stRKEntryInfo.szVer = m_szToken[4];
	stRKEntryInfo.szOwner = m_szToken[5];
}

// History ������ ���� ��, ������ ���� �� ����.
void CRKParser::GetHistoryTokenStruct(struct stRKHistoryInfo& stRKHistoryInfo)
{
	if(m_bOpened == FALSE)
		return;

	if(m_cNumToken <= 0)
		return;

	stRKHistoryInfo.szOwner = m_szToken[0];
	stRKHistoryInfo.szDate = m_szToken[1];
	stRKHistoryInfo.szVer = m_szToken[2];
}