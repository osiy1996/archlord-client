//
// File: AuExcelTxtLib.cpp
//
//

#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include <AuMD5EnCrypt/AuMD5Encrypt.h>
#include <ApMemoryTracker/ApMemoryTracker.h>

#ifndef _AREA_CHINA_
#include <AuZpackLoader/AuZpackLoader.h>

#define HASH_KEY_STRING "1111"
#endif

AuExcelTxtLib::AuExcelTxtLib()
{
	m_pppiData	= NULL	;
//	m_pFile		= NULL	;
	m_iColumn	= 0		;
	m_iRow		= 0		;
	m_pBuffer	= NULL	;
	m_iBufferSize = 0	;
	m_iBufferOffset = 0 ;
#ifndef _AREA_CHINA_
	isZpackStream_ = true;
#endif
}

void AuExcelTxtLib::Reset()
{
	if( m_pppiData != NULL )
	{
		int i = 0, j = 0;
		for(j=0; j<m_iRow; ++j )
		{
			for(i=0; i<m_iColumn; ++i )
			{
				delete [] m_pppiData[j][i];
			}
		}

		for( j=0; j<m_iRow; ++j )
		{
			delete [] m_pppiData[j];
		}

		delete [] m_pppiData;
	}

	if (m_pBuffer)
	{
#ifndef _AREA_CHINA_
		if( isZpackStream_ && !packName_.empty() )
			ReleaseZpackStream( (wchar_t*)packName_.c_str(), m_pBuffer );
		else
#endif
			delete [] m_pBuffer;
		m_pBuffer = NULL;
	}

	m_pppiData	= NULL	;
	m_iColumn	= 0		;
	m_iRow		= 0		;
	m_pBuffer	= NULL	;
	m_iBufferSize = 0	;
	m_iBufferOffset = 0 ;
#ifndef _AREA_CHINA_
	isZpackStream_ = true;
	packName_ = L"";
#endif
}

AuExcelTxtLib::~AuExcelTxtLib()
{
	Reset();
}

BOOL AuExcelTxtLib::OpenExcelFile( const char *pstrFileName, BOOL bLoadAll, BOOL bDecryption )
{
	BOOL			bResult;

	m_iBufferOffset = 0 ;
	
	Reset();

	if( pstrFileName == NULL )
		return FALSE;
#ifndef _AREA_CHINA_
	std::string pathName = pstrFileName;

	size_t zpackNameIdx1 = pathName.find( "\\" );
	size_t zpackNameIdx2 = pathName.find( "/" );

	zpackNameIdx1 = zpackNameIdx1 < zpackNameIdx2 ? zpackNameIdx1 : zpackNameIdx2;

	std::string packName = pathName.substr( 0, zpackNameIdx1 );

	wchar_t wpackName[1024] = {0,};

	if( !packName.empty() )
	{
		MultiByteToWideChar( CP_ACP, MB_COMPOSITE, packName.c_str(), (int)packName.length(), wpackName, 1024 );
		wcscat_s( wpackName, 1024, L".zp" );
		size_t buffsize = 0;
		m_pBuffer = (char*)GetZpackStream( wpackName, pathName.c_str(), buffsize );
		m_iBufferSize = buffsize;
		packName_ = wpackName;
	}

	if( !m_pBuffer )
	{
		isZpackStream_ = false;

		FILE* pFile = fopen(pstrFileName, "rb");

		if (NULL == pFile) return FALSE;

		// ���� ������ �˾ƿ���
		fseek(pFile, 0, SEEK_END);

		m_iBufferSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);

		// ������ �о����
		m_pBuffer = new char[m_iBufferSize+1];
		memset(m_pBuffer, 0, m_iBufferSize+1);
		size_t lReadBytes = fread(m_pBuffer, sizeof(BYTE), m_iBufferSize, pFile);
		fclose(pFile);
	}
#else
	FILE* pFile = fopen(pstrFileName, "rb");

	if (NULL == pFile) return FALSE;

	// ���� ������ �˾ƿ���
	fseek(pFile, 0, SEEK_END);
	
    m_iBufferSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	// ������ �о����
	m_pBuffer = new char[m_iBufferSize+1];
	memset(m_pBuffer, 0, m_iBufferSize+1);
	size_t lReadBytes = fread(m_pBuffer, sizeof(BYTE), m_iBufferSize, pFile);
	fclose(pFile);
#endif

//	m_pFile = fopen( pstrFileName, "rb" );

	if (bDecryption)
	{
		AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
		MD5.DecryptString(MD5_HASH_KEY_STRING, m_pBuffer, m_iBufferSize);
#else
		MD5.DecryptString(HASH_KEY_STRING, m_pBuffer, m_iBufferSize);
#endif
	}

//	if( m_pFile != NULL )
	if( NULL != m_pBuffer )
	{
		//��ü�� �� �о������.
		if( bLoadAll == TRUE )
		{
			//@{ 2006/07/31 burumal		

		#ifdef __USE_EXCEL_TXT_LIB_HELPER__
			
			EXCEL_TXT_FILE_INFO* pExcelTxt = AuExcelTxtLibHelper::GetSingletonPtr()->Find(pstrFileName);
			if ( pExcelTxt )
			{
				if ( m_iBufferSize == pExcelTxt->lFileSize )
				{
					m_iRow		= pExcelTxt->lRow;
					m_iColumn	= pExcelTxt->lColumn;
				}
				else
				{
					AuExcelTxtLibHelper::GetSingletonPtr()->SetModified(TRUE);
					AuExcelTxtLibHelper::GetSingletonPtr()->Delete(pstrFileName);
					pExcelTxt = NULL;
				}
			}

			if ( pExcelTxt == NULL )
			{
				GetRowFromFile();
				GetColumnFromFile();

				pExcelTxt = AuExcelTxtLibHelper::GetSingletonPtr()->NewElement(pstrFileName);
				if ( pExcelTxt )
				{
					pExcelTxt->lColumn		= m_iColumn;
					pExcelTxt->lRow			= m_iRow;
					pExcelTxt->lFileSize	= m_iBufferSize;

					AuExcelTxtLibHelper::GetSingletonPtr()->Insert(pExcelTxt);
				}
			}
		#else // __USE_EXCEL_TXT_LIB_HELPER__

			GetRowFromFile();
			GetColumnFromFile();

		#endif // __USE_EXCEL_TXT_LIB_HELPER__

			//@}

			//���� ���ΰ� ���� 0�� �ƴϸ�~
			if( (m_iRow != 0) && (m_iColumn != 0) )
			{
//				char			**ppiData;

				m_pppiData = new char **[m_iRow];
				//ppiData = new char *[m_iColumn];

				for( int i=0; i<m_iRow; ++i )
				{
					m_pppiData[i] = new char *[m_iColumn];

					ReadRow( m_pppiData[i] );
				}
			}
		}
		//���μ��θ� ����.
		else
		{
			//@{ 2006/09/25 burumal

		#ifdef __USE_EXCEL_TXT_LIB_HELPER__
			
			EXCEL_TXT_FILE_INFO* pExcelTxt = AuExcelTxtLibHelper::GetSingletonPtr()->Find(pstrFileName);
			if ( pExcelTxt )
			{
				if ( m_iBufferSize == pExcelTxt->lFileSize )
				{
					m_iRow		= pExcelTxt->lRow;
					m_iColumn	= pExcelTxt->lColumn;
				}
				else
				{
					AuExcelTxtLibHelper::GetSingletonPtr()->SetModified(TRUE);
					AuExcelTxtLibHelper::GetSingletonPtr()->Delete(pstrFileName);
					pExcelTxt = NULL;
				}
			}

			if ( pExcelTxt == NULL )
			{
				GetRowFromFile();
				GetColumnFromFile();

				pExcelTxt = AuExcelTxtLibHelper::GetSingletonPtr()->NewElement(pstrFileName);
				if ( pExcelTxt )
				{
					pExcelTxt->lColumn		= m_iColumn;
					pExcelTxt->lRow			= m_iRow;
					pExcelTxt->lFileSize	= m_iBufferSize;						

					AuExcelTxtLibHelper::GetSingletonPtr()->Insert(pExcelTxt);
				}
			}

		#else // __USE_EXCEL_TXT_LIB_HELPER__

			GetRowFromFile();
			GetColumnFromFile();

		#endif // __USE_EXCEL_TXT_LIB_HELPER__
		}

		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

BOOL AuExcelTxtLib::CloseFile()
{
	BOOL			bResult;

	if( m_pBuffer != NULL )
	{
#ifndef _AREA_CHINA_
		if( isZpackStream_ && !packName_.empty() )
			ReleaseZpackStream( (wchar_t*)(packName_.c_str()), m_pBuffer );
		else
#endif
			delete [] m_pBuffer;

		m_pBuffer = NULL;
		bResult = TRUE;
	}
	else
	{
		bResult = FALSE;
	}

	return bResult;
}

int AuExcelTxtLib::GetColumnFromFile()
{
	int			iColumn;
	int			iOffsetBytes = 0;

	iColumn = 0;

	if( NULL != m_pBuffer )
	{
		char		cLastData;
		char		cData;
		char		strBuffer[9999];
		int			iLocalIndex = 0;

		memset( &strBuffer, 0, sizeof(strBuffer) );

		while( 1 )
		{
			if( iOffsetBytes > m_iBufferSize )
			{
				break;
			}

			//cData = fgetc( m_pFile );
			cData = *(m_pBuffer + iOffsetBytes);

			if( cData == EOF )
				break;

			++iOffsetBytes;
			
			//Tab�̸� ������ ��ĭ�̴�.
			if( cData == 0x09 )
			{
				memset( &strBuffer, 0, sizeof(strBuffer) );
				iLocalIndex = 0;

				iColumn++;
			}
			//���������ΰ�?
			else
			{
				if( cData == 0x0A )
				{
					if( cLastData == 0x0D )
					{
						//���� �д��� ������ �ϳ� �� �߰�.
						iColumn++;

						break;
					}
				}

				strBuffer[iLocalIndex] = cData;
				iLocalIndex++;
			}

			cLastData = cData;
		}

		m_iColumn = iColumn;

//		fseek( m_pFile, 0, SEEK_SET );
	}

	return iColumn;
}

int AuExcelTxtLib::GetRowFromFile()
{
	int			iRow;
	int			iOffsetBytes = 0;

	iRow = 0;

	if( NULL != m_pBuffer )
	{
		char		cLastData;
		char		cData;
		int			iLocalIndex = 0;

		while( 1 )
		{
			if( iOffsetBytes >= m_iBufferSize )
			{
				break;
			}

			cData = *(m_pBuffer + iOffsetBytes);

			if ( cData == EOF ) break;

			if( cData == 0x0A )
			{
				if( cLastData == 0x0D )
				{
					iRow++;
				}
			}

			cLastData = cData;
			++iOffsetBytes;
		}

		m_iRow = iRow;

//		fseek( m_pFile, 0, SEEK_SET );
	}

	return iRow;
}

int AuExcelTxtLib::GetColumn()
{
	return m_iColumn;
}

int AuExcelTxtLib::GetRow()
{
	return m_iRow;
}

char *AuExcelTxtLib::GetData( char **ppiData, int iIndex )
{
	if( (0 <= iIndex) && (iIndex < m_iColumn ) )
	{
		return ppiData[iIndex];
	}
	else
	{
		return NULL;
	}
}

char *AuExcelTxtLib::GetData( int iColumn, int iRow )
{
	if( (0 <= iColumn) && (iColumn < m_iColumn ) && (0 <= iRow) && (iRow < m_iRow ) )
	{
		return m_pppiData[iRow][iColumn];
	}
	else
	{
		return NULL;
	}
}

INT32 AuExcelTxtLib::GetDataToInt( int iColumn, int iRow )
{
	CHAR* pRet = GetData(iColumn, iRow);
	if (NULL == pRet) return 0;

	return atoi(pRet);
}

float AuExcelTxtLib::GetDataToFloat( int iColumn, int iRow )
{
	CHAR* pRet = GetData(iColumn, iRow);
	if (NULL == pRet) return 0;

	return ( float ) atof(pRet);
}

bool AuExcelTxtLib::IsEmpty( int iColumn, int iRow )
{
	CHAR* pRet = GetData(iColumn, iRow);
	if ( pRet ) return false;
	else	return true;
}


void AuExcelTxtLib::ResetRowData( char **ppiData )
{
	for( int i=0; i<m_iColumn; ++i )
	{
		delete [] ppiData[i];
	}
}

BOOL AuExcelTxtLib::ReadRow( char **ppiData )
{
	int			iColumn;
	BOOL		bResult;
	int			iOffsetBytes = 0;

	iColumn = 0;
	bResult = TRUE;

//	if( m_pFile != NULL )
	if ( NULL != m_pBuffer )
	{
		char		cLastData;
		char		cData;
		char		strBuffer[9999];
		int			iLocalIndex = 0;

		for( int i=0; i<m_iColumn; ++i )
			ppiData[i] = NULL;

		//@{ 2006/11/22 burumal
		/*
		memset( &strBuffer, 0, sizeof(strBuffer) );
		*/
		strBuffer[0] = NULL;
		//@}

		while( 1 )
		{
			if (m_iBufferOffset + iOffsetBytes > m_iBufferSize)
			{
				bResult = FALSE;
				break;
			}

			cData = *(m_pBuffer + m_iBufferOffset + iOffsetBytes);
			//cData = fgetc( m_pFile );

			if( cData == EOF )
			{
				bResult = FALSE;
				break;
			}

			++iOffsetBytes;
/*
			if( cData == EOF )
			{
				bResult = FALSE;
				break;
			}
*/
			//Tab�̸� ������ ��ĭ�̴�.
			if( cData == 0x09 )
			{
				//������ �־���.
				if( iLocalIndex != 0 )
				{
					if( iColumn < m_iColumn )
					{
						char		*pstrData;
						size_t		iLength;

						iLength = strlen(strBuffer)+1;

						pstrData = new char[iLength];

						memcpy( pstrData, strBuffer, iLength );

						ppiData[iColumn] = pstrData;
					}
				}

				//@{ 2006/11/22 burumal
				/*
				memset( &strBuffer, 0, sizeof(strBuffer) );
				*/
				strBuffer[0] = NULL;
				//@}

				iLocalIndex = 0;

				iColumn++;
			}
			//���������ΰ�?
			else
			{
				if( cData == 0x0A )
				{
					if( cLastData == 0x0D )
					{
						iLocalIndex--; //�̹� �о�� 0x0A�� �����.

						//�д��� ������ �������ش�.
						if( iLocalIndex != 0 )
						{
							if( iColumn < m_iColumn )
							{
								char		*pstrData;
								size_t		iLength;

								//0x0D(������ �� ����Ʈ)�� ���� �����Ѵ�.
								iLength = strlen(strBuffer);

								pstrData = new char[iLength];

								//memset( pstrData, 0, iLength );
								pstrData[iLength - 1] = NULL;
								memcpy( pstrData, strBuffer, iLength-1 );

								ppiData[iColumn] = pstrData;
							}
						}

						break;
					}
				}

				//@{ 2006/11/22 burumal
				/*
				strBuffer[iLocalIndex] = cData;
				iLocalIndex++;
				*/
				strBuffer[iLocalIndex++] = cData;
				strBuffer[iLocalIndex] = NULL;
				//@}
			}

			cLastData = cData;
		}
	}
	else
	{
		bResult = FALSE;
	}

	m_iBufferOffset += iOffsetBytes;

	return bResult;
}

bool	AuExcelTxtLib::EncryptSave(LPCTSTR pszPathName)
{
	FILE *pReadFile;

	pReadFile = fopen(pszPathName, "rb");

	if (NULL == pReadFile) return false;

	// ���� ������ �˾ƿ���
	fseek(pReadFile, 0 , SEEK_END);
	
    size_t lFileSize = ftell(pReadFile);
	fseek(pReadFile, 0 , SEEK_SET);

	// ������ �о����
	char* pBuffer = new char[lFileSize];
	memset(pBuffer, 0, lFileSize);
	size_t lReadBytes = fread(pBuffer, sizeof(BYTE), lFileSize, pReadFile);
	fclose(pReadFile);

	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	MD5.EncryptString(MD5_HASH_KEY_STRING, pBuffer, (unsigned long)lFileSize);
#else
	MD5.EncryptString(HASH_KEY_STRING, pBuffer, (unsigned long)lFileSize);
#endif
	
	FILE *pWriteFile = fopen(pszPathName, "wb");
	if (NULL == pWriteFile) return false;

	size_t WriteBytes = fwrite(pBuffer, sizeof(char), lFileSize, pWriteFile);

	fclose(pWriteFile);

	if (WriteBytes == lFileSize) return true;
	else return false;
}

//@{ 2006/07/31 burumal
AuExcelTxtLibHelper* AuExcelTxtLibHelper::m_pThis = NULL;

AuExcelTxtLibHelper::AuExcelTxtLibHelper()
{	
	m_bModified		= FALSE;
	m_nLoadedFileInfoCount = 0;	
	ZeroMemory(m_pIniFileName, sizeof(m_pIniFileName));
}

AuExcelTxtLibHelper::~AuExcelTxtLibHelper()
{
	DeleteAll();
}

BOOL AuExcelTxtLibHelper::Insert(EXCEL_TXT_FILE_INFO* pExcelTxt)
{
	if ( pExcelTxt == NULL )
		return FALSE;

	m_hmapFiles.insert(hmapExcelTxtFileInfo::value_type(pExcelTxt->pFileName, pExcelTxt));

	return TRUE;
}

BOOL AuExcelTxtLibHelper::Delete(const char* pFullPathFileName)
{
	if ( pFullPathFileName == NULL )
		return FALSE;

	hmapExcelTxtFileInfoItr itDel = m_hmapFiles.find(string(pFullPathFileName));
	if ( itDel != m_hmapFiles.end() )
	{
		if ( itDel->second )
		{
			if ( itDel->second->pFileName )
				delete [] itDel->second->pFileName;

			delete itDel->second;
		}

		m_hmapFiles.erase(itDel);
	}

	return TRUE;
}

EXCEL_TXT_FILE_INFO* AuExcelTxtLibHelper::Find(const char* pFullPathFileName)
{	
	if ( m_hmapFiles.empty() || pFullPathFileName == NULL )
		return NULL;	
	
	hmapExcelTxtFileInfoItr itFound = m_hmapFiles.find(pFullPathFileName);
	if ( itFound != m_hmapFiles.end() )
		return itFound->second;

	return NULL;
}

VOID AuExcelTxtLibHelper::DeleteAll()
{
	hmapExcelTxtFileInfoItr itDel;
	for ( itDel = m_hmapFiles.begin(); itDel != m_hmapFiles.end(); itDel++ )
	{
		if ( itDel->second )
		{
			if ( itDel->second->pFileName )
				delete itDel->second->pFileName;

			delete itDel->second;
		}
	}
		
	m_hmapFiles.erase(m_hmapFiles.begin(), m_hmapFiles.end());
}

EXCEL_TXT_FILE_INFO* AuExcelTxtLibHelper::NewElement(const char* pFullPathFileName)
{	
	char drive[256], dir[256], fname[256], ext[256];

	if ( !pFullPathFileName )
		return NULL;

	fname[0] = NULL;
	_splitpath( pFullPathFileName, drive, dir, fname, ext );

	if ( fname[0] == NULL )
		return NULL;

	EXCEL_TXT_FILE_INFO* pNew = new EXCEL_TXT_FILE_INFO;
	if ( pNew )
	{
		pNew->Clear();

		pNew->pFileName = new char[strlen(pFullPathFileName) + 1];
		ASSERT(pNew->pFileName);

		strcpy(pNew->pFileName, pFullPathFileName);
	}

	return pNew;
}

EXCEL_TXT_FILE_INFO* AuExcelTxtLibHelper::NewElement(const char* pPath, const char* pFileName)
{
	if ( pFileName == NULL )
		return NULL;
	
	string strPath;
	strPath = pPath;
	if ( strPath[strPath.length() - 1] != '\\' && strPath[strPath.length() - 1] != '/' )
		strPath += "\\";
	strPath += pFileName;

	return NewElement(strPath.c_str());
}

int	AuExcelTxtLibHelper::GetRow(const char* pFullPathFileName)
{
	EXCEL_TXT_FILE_INFO* pExcelTxt = Find(pFullPathFileName);
	if ( pExcelTxt )
		return pExcelTxt->lRow;

	return 0;
}

int	AuExcelTxtLibHelper::GetColumn(const char* pFullPathFileName)
{
	EXCEL_TXT_FILE_INFO* pExcelTxt = Find(pFullPathFileName);
	if ( pExcelTxt )
		return pExcelTxt->lColumn;

	return 0;
}

VOID AuExcelTxtLibHelper::GetRowColumn(const char* pFullPathFileName, int* pRow, int* pColumn)
{
	if ( !pRow || !pColumn )
		return;

	EXCEL_TXT_FILE_INFO* pExcelTxt = Find(pFullPathFileName);
	if ( pExcelTxt )
	{
		*pRow = pExcelTxt->lRow;
		*pColumn = pExcelTxt->lColumn;
	}
}

AuExcelTxtLibHelper* AuExcelTxtLibHelper::MakeInstance()
{
	if ( m_pThis == NULL )
		return new AuExcelTxtLibHelper;

	return m_pThis;
}

AuExcelTxtLibHelper* AuExcelTxtLibHelper::GetSingletonPtr()
{
	if ( m_pThis == NULL )
		m_pThis = MakeInstance();

	return m_pThis;
}

void AuExcelTxtLibHelper::DestroyInstance()
{
	if ( m_pThis )
	{
		delete m_pThis;
		m_pThis = NULL;
	}
}

VOID AuExcelTxtLibHelper::SetIniFile(const char* pIniFileName)
{
	if ( pIniFileName == NULL )
	{
		m_pIniFileName[0] = NULL;
		return;
	}

	strcpy(m_pIniFileName, pIniFileName);
}

BOOL AuExcelTxtLibHelper::LoadIniFile()
{
	if ( IsEnable() )
		DeleteAll();

	m_nLoadedFileInfoCount = 0;

	FILE* pFile = fopen(m_pIniFileName, "rb");
	if ( !pFile )
		return FALSE;

	fseek(pFile, 0, SEEK_END);
	long lSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pDataBuff = new char[lSize];
	if ( !pDataBuff )
	{
		fclose(pFile);
		return FALSE;
	}

	fread(pDataBuff, lSize, 1, pFile);
	fclose(pFile);
	
	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	MD5.DecryptString(MD5_HASH_KEY_STRING, pDataBuff, lSize);
#else
	MD5.DecryptString(HASH_KEY_STRING, pDataBuff, lSize);
#endif

	char* pStartPtr = pDataBuff;

	bool bIsValid = FALSE;
	if ( CheckHeaderTag(pDataBuff, lSize) )
	{
		UINT16 uVersion = *((UINT16*) (pDataBuff + strlen(AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG) + 1));
		if ( uVersion == AU_EXCEL_TXT_LIB_HELPER_VERSION )
		{
			bIsValid = TRUE;
			pDataBuff += ((INT32) strlen(AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG) + sizeof(UINT16));			
			while ( *(pDataBuff++) != '\n' )
				;
		}
	}

	if ( bIsValid == FALSE )
	{
		DeleteIniFile();
		return FALSE;
	}
	
	BOOL bExit = FALSE;

	char pTempFileName[MAX_PATH];
	int lTempFileSize;
	int lTempRow;
	int lTempColumn;

	if ( pDataBuff < pStartPtr + lSize )
	{	
		do 
		{
			if ( (sscanf(pDataBuff, "%s ,%d,%d,%d\r\n", pTempFileName, &lTempFileSize, &lTempRow, &lTempColumn)) > 0 )
			{
				if ( pTempFileName[0] == NULL )
					break;

				EXCEL_TXT_FILE_INFO* pNew = NewElement(pTempFileName);
				if ( pNew )
				{				
					pNew->lFileSize = lTempFileSize;
					pNew->lRow = lTempRow;
					pNew->lColumn = lTempColumn;

					Insert(pNew);
				}

				pDataBuff = strstr(pDataBuff, "\r\n");
				pDataBuff += 2;

				if ( pDataBuff == NULL || ((pStartPtr + lSize) <= pDataBuff) )
					bExit = TRUE;
			}
			else
			{
				bExit = TRUE;
			}

		} while (bExit != TRUE);
	}

	delete [] pStartPtr;

	m_nLoadedFileInfoCount = (INT32) m_hmapFiles.size();

	return TRUE;
}

BOOL AuExcelTxtLibHelper::SaveIniFile()
{
	if ( m_pIniFileName[0] == NULL )
		return FALSE;

	if ( ( (INT32) m_hmapFiles.size() <= m_nLoadedFileInfoCount) && (m_bModified == FALSE) )
		return TRUE;

	FILE* pFile = fopen(m_pIniFileName, "wb");
	if ( !pFile )
		return FALSE;

	if ( WriteHeaderTag(pFile) != TRUE )
		return FALSE;

	char pTempData[1024];
	hmapExcelTxtFileInfoItr itFile;
	for ( itFile = m_hmapFiles.begin(); itFile != m_hmapFiles.end(); itFile++ )
	{
		EXCEL_TXT_FILE_INFO* pExcelFile = itFile->second;
		ASSERT(pExcelFile);

		sprintf(pTempData, "%s ,%d,%d,%d\r\n", 
			pExcelFile->pFileName, pExcelFile->lFileSize, pExcelFile->lRow, pExcelFile->lColumn);

		fwrite(pTempData, strlen(pTempData), 1, pFile);
	}

	fclose(pFile);
	
	pFile = fopen(m_pIniFileName, "rb");
	ASSERT(pFile != NULL);

	fseek(pFile, 0, SEEK_END);
	int lSize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	char* pDataBuff = new char[lSize];
	ASSERT(pDataBuff != NULL);

	fread(pDataBuff, lSize, 1, pFile);
	fclose(pFile);
	
	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	MD5.EncryptString(MD5_HASH_KEY_STRING, pDataBuff, lSize);
#else
	MD5.EncryptString(HASH_KEY_STRING, pDataBuff, lSize);
#endif

	pFile = fopen(m_pIniFileName, "wb");
	ASSERT(pFile != NULL);
	fwrite(pDataBuff, lSize, 1, pFile);
	fclose(pFile);	
	
	delete [] pDataBuff;

	return TRUE;
}
//@}

//@{ 2006/09/25 burumal
BOOL AuExcelTxtLibHelper::DeleteIniFile()
{
	if ( m_pIniFileName[0] == NULL )
		return FALSE;
	
	FILE* pFile = fopen(m_pIniFileName, "rb");
	fclose(pFile);

	if ( pFile != NULL )
	{
		::DeleteFile(m_pIniFileName);
	}

	return TRUE;
}

BOOL AuExcelTxtLibHelper::WriteHeaderTag(FILE* fpIni)
{
	if ( fpIni == NULL )
		return FALSE;
	
	UINT16 uVer = AU_EXCEL_TXT_LIB_HELPER_VERSION;	

	char pTemp[256];
	sprintf(pTemp, "%s", AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG);

	fseek(fpIni, 0, SEEK_SET);
	fwrite(pTemp, strlen(pTemp) + 1, 1, fpIni);
	fwrite(&uVer, sizeof(UINT16), 1, fpIni);	
	fputs("\r\n", fpIni);

	return TRUE;
}

BOOL AuExcelTxtLibHelper::CheckHeaderTag(const char* pDataBuffer, INT32 nSize)
{
	if ( pDataBuffer == NULL || nSize <= 0 )
		return FALSE;

	if ( nSize < (INT32) strlen(AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG) )
		return FALSE;

	int nHeaderLength = ( INT32 )strlen( AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG );

	char* pBuffer = new char[ nHeaderLength + 1 ];
	memset( pBuffer, 0, sizeof( char ) * ( nHeaderLength + 1 ) );
	memcpy_s( pBuffer, nHeaderLength, pDataBuffer, nHeaderLength );

	BOOL bIsVaild = strncmp(AU_EXCEL_TXT_LIB_HELPER_HEADER_TAG, pBuffer, nHeaderLength + 1) == 0 ? TRUE : FALSE;

	delete[] pBuffer;
	pBuffer = NULL;

	return bIsVaild;
}
//@}