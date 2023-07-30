/*****************************************************************
*   Comment  : 
*   Date&Time : 2002-12-05, ���� 7:53
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

#include <ApBase/ApBase.h>
#include <AuIniManager/AuIniManager.h>
#include <AuMD5EnCrypt/AuMD5Encrypt.h>
#include <ApMemoryTracker/ApMemoryTracker.h>
#ifndef _AREA_CHINA_
#include <AuZpackLoader/AuZpackLoader.h>
#include <AuExportedFileDic/AuExportedFileDic.h>
#include <AuStaticPool/static_pool.h>
#include <direct.h>
#endif // _AREA_CHINA_

#define CRLF						"\r\n"

#ifndef _AREA_CHINA_
#define HASH_KEY_STRING				"1111"
#endif // _AREA_CHINA_

//@{ 2006/07/28 burumal
int g_nCRLELength = 0;
//@}

int	print_compact_float( char * pStr , float fValue )
{
	sprintf( pStr , "%f" , fValue );

	int nLength = ( int ) strlen( pStr );

	for( int i = nLength - 1 ; i > 0 ; i -- )
	{
		if( pStr[ i ] == '0' )
		{
			pStr [ i ] = '\0';
		}
		else
		if( pStr[ i ] == '.' )
		{
			pStr [ i ] = '\0';
			break;
		}
		else break;
	}
	nLength = ( int ) strlen( pStr );
	return nLength;
}

int print_compact_format( char * pStr , const char * pFmt , ... )
{
	va_list	ap;

	int nSum = 0;

	const char *	pCurrentFmt = pFmt;
	char *			pCurrentStr = pStr;

	bool bArgument = false;

	va_start( ap , pFmt );
	/*
	int nValue = va_arg( ap , int );
	nValue = va_arg( ap , int );
	nValue = va_arg( ap , int );
	double fValue = va_arg( ap , double );
	fValue = va_arg( ap , double );
	fValue = va_arg( ap , double );
	*/

	while( *pCurrentFmt != '\0' )
	{
		if( bArgument )
		{
			switch( *pCurrentFmt )
			{
			case 'f':
			case 'F':
				{
					float fValue = ( float ) ( va_arg( ap , double ) );
					pCurrentStr += print_compact_float( pCurrentStr , fValue );
					break;
				}
			case 'd':
			case 'D':
				{
					int nValue = va_arg( ap , int );
					pCurrentStr += sprintf( pCurrentStr , "%d" , nValue );
					break;
				}
			case 's':
			case 'S':
				{
					const char * pValue = va_arg( ap , const char * );
					pCurrentStr += sprintf( pCurrentStr , "%s" , pValue );
					break;
				}
			case 'x':
			case 'X':
				{
					int nValue = va_arg( ap , int );
					pCurrentStr += sprintf( pCurrentStr , "%x" , nValue );
					break;
				}
			}
			bArgument = false;
		}
		else
		if( *pCurrentFmt == '%' )
		{
			bArgument = true;
		}
		else
		{
			bArgument = false;
			*pCurrentStr = *pCurrentFmt;
			pCurrentStr++;
			*pCurrentStr = '\0';
		}

		pCurrentFmt ++;
	}

	va_end( ap );

	int nLength = ( int ) strlen( pStr );
	return nLength;
}

const char * AuIniManagerA::GetKeyNameTable( int nIndex )
{
	ASSERT( nIndex < ( int ) m_vecKeyTable.size() );

	return m_vecKeyTable.at( nIndex ).c_str();
}

int		AuIniManagerA::GetKeyIndex	( const char * pStr )
{
	int nOffset = 0;
	for( vector< string >::iterator iTer = m_vecKeyTable.begin();
		iTer != m_vecKeyTable.end();
		iTer ++ , nOffset ++ )
	{
		string *pKey = &*iTer;

		if( !strncmp( pStr , pKey->c_str() , AUINIMANAGER_MAX_NAME ) )
		{
			// ����.
			return nOffset;
		}
	}

	// ����� ���� ���°�..
	// Ű�ε��� �߰���

	m_vecKeyTable.push_back( pStr );
	return nOffset;
}

AuIniManagerA::AuIniManagerA():
	m_eMode			( AUINIMANAGER_MODE_NORMAL	),
	m_eType			( AUINIMANAGER_TYPE_NORMAL	),
	m_stSection		( NULL						),
	lSectionNum		( 0							),
	lHalfSectionNum	( 0							),
	bIsSectionNumOdd( false						),
	m_eProcessMode	( AuIniManagerA::TXT		)
{
	m_lPartNum = 256;	
	m_plPartIndices = new int[m_lPartNum];	

	//@{ 2006/07/28 burumal
	g_nCRLELength = (int) strlen(CRLF);
	//@}

	Initialize();
}

AuIniManagerA::~AuIniManagerA()
{
	if ( m_stSection != NULL )
	{
		for ( int i = 0 ; i < lSectionNum ; ++i )
		{
			if ( m_stSection[i].stKey != NULL ) 
			{
				delete [] m_stSection[i].stKey;
				m_stSection[i].stKey = NULL;
			}
		}

		delete [] m_stSection;
		m_stSection = NULL;
	}

	if ( m_plPartIndices != NULL )
	{
		delete [] m_plPartIndices;
		m_plPartIndices = NULL;
	}
}

/*****************************************************************
*   Function : Initialize
*   Comment  : 
*   Date&Time : 2002-12-05, ���� 8:02
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::Initialize( )
{
	ClearAllSectionKey();		
}

/*****************************************************************
*   Function : SetPath
*   Comment  : 
*   Date&Time : 2002-12-05, ���� 7:54
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::SetPath(	const char* pszPath	)
{
	m_strPathName = pszPath;
	//	Initialize();		-			2003_01_09 ���� 
}

/*****************************************************************
*   Function : 
*   Comment  : 
*   Date&Time : 2002-12-05, ���� 8:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AuIniManagerA::ReadFile	( int lIndex, BOOL bDecryption )
{
	//�������� �ʱ�ȭ
	if ( m_strPathName.size() == 0 )
		return FALSE;

	ClearAllSectionKey();

	//-----------------------------------------------------------------------
	// ���ο� ��ŷ ���� ������� �ε����� ���� - kdi 2011.01.27

	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		BOOL result = FALSE;

		result = FromFile( lIndex, bDecryption );

		/*
		std::string filename = m_strPathName; // ���� �����̸� ����

		//{@ �����̸��� ��ȣȭ ���� �������·� ����
		const char * originalName = FileNameDic::FindOriginalName( filename );

		if( originalName )
		{
			m_strPathName = originalName;
		}
		//}@
		result = FromFile(lIndex, bDecryption);

		if( !result )
		{
			//{@ �����̸��� ��ȣȭ�� ���·� ����
			m_strPathName = filename;

			const char * exportedName = FileNameDic::FindExportedName( filename );

			if( exportedName )
			{
				m_strPathName = exportedName;
			}
			//}@

			result = FromFile(lIndex, bDecryption); // ��ȣȭ�� �̸����� �ε�
		}

		m_strPathName = filename; // ���� �����̸����� �ǵ���
		*/

		return result;
	}
	else
	{
		BOOL result = false;

		result = FromMemory( bDecryption );

		/*
		std::string filename = m_strPathName; // ���� �����̸� ����

		//{@ �����̸��� ��ȣȭ ���� �������·� ����
		const char * originalName = FileNameDic::FindOriginalName( filename );

		if( originalName )
		{
			m_strPathName = originalName;
		}
		//}@

		result = FromMemory(bDecryption); // ��ȣȭ ���� ���� �̸����� �ε�

		if( !result )
		{
			//{@ �����̸��� ��ȣȭ�� ���·� ����
			m_strPathName = filename;

			const char * exportedName = FileNameDic::FindExportedName( filename );

			if( exportedName )
			{
				m_strPathName = exportedName;
			}
			//}@

			result = FromMemory( bDecryption ); // ��ȣȭ�� �̸����� �ε�
		}

		m_strPathName = filename; // ���� �����̸����� �ǵ���
		*/

		return result;
	}

	//-----------------------------------------------------------------------
}	

#ifndef _AREA_CHINA_
void throw_exception( LPCSTR szFmt , ... )
{
	static char szOutBuff [ 1024 ] ;

	va_list  args ;

	va_start ( args , szFmt ) ;

	wvsprintfA ( szOutBuff , szFmt , args ) ;

	va_end ( args ) ;

	throw szOutBuff ;
}

umtl::static_pool::wstring AuIniManagerA::GetPackName()
{
	size_t zpackNameIdx1 = m_strPathName.find( "\\" );
	size_t zpackNameIdx2 = m_strPathName.find( "/" );

	zpackNameIdx1 = zpackNameIdx1 < zpackNameIdx2 ? zpackNameIdx1 : zpackNameIdx2;

	umtl::static_pool::string packName = m_strPathName.substr( 0, zpackNameIdx1 ).c_str();

	static wchar_t wpackName[1024] = {0,};

	if( !packName.empty() )
	{
		MultiByteToWideChar( CP_ACP, 0, packName.c_str(), (int)packName.length(), wpackName, 1024 );
		wpackName[packName.length()]=0;
		wcscat_s( wpackName, 1024, L".zp" );
		wpackName[packName.length()+3]=0;
	}
	return wpackName;
}
#endif

//@{ 2006/09/05 burumal
// �� �Լ��� AUINIMANAGER_TYPE_PART_INDEX Ÿ������ ������ �����Ƿ� ȥ���� ���ϱ� ���� ������
//BOOL	AuIniManagerA::FromMemory( int lIndex, BOOL bDecryption)
BOOL	AuIniManagerA::FromMemory( BOOL bDecryption)
//@}
{
	int	 lMaxCharacter = AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1;

	char szDummy[AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1];

	int	lFileSectionNum								= 0		;		//Section�� ��ΰ�. 

	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM]	= {0,}	;		//�� Section�� Key�� ����ΰ� 

	int		i = 0;
	int		lStartPos = 0;
	int		lEndPos	= 0;
	__int64	lCurPos = 0;

	size_t iOffsetBytes = 0;
	size_t iOffset		= 0;

	//File ����
	FILE*		file;
	char*		pBuffer;
	size_t		iFileSize;
	bool		bFileReadResult;

	bFileReadResult = true;
	pBuffer = NULL;
	iFileSize = 0;
#ifndef _AREA_CHINA_
	bool isZpackStream = true;

	umtl::static_pool::wstring packName = GetPackName();

	if( !packName.empty() )
	{
		pBuffer = (char*)GetZpackStream( (wchar_t*)packName.c_str(), m_strPathName.c_str(), iFileSize );
	}

	if( !pBuffer )
	{
		isZpackStream = false;

		file = fopen( m_strPathName.c_str(), "rb" );

		if( file )
		{
			fseek( file, 0, SEEK_END );
			iFileSize = ftell( file );
			fseek( file, 0, SEEK_SET );

			if( iFileSize )
			{
				pBuffer = new char[iFileSize + 1];

				//@{ 2006/11/22 burumal
				//memset(pBuffer, 0, iFileSize + 1);
				//@}

				if( fread( pBuffer, 1, iFileSize, file ) != iFileSize )
				{
					bFileReadResult = false;
				}

				pBuffer[iFileSize] = 0;
			}
			else
			{
				bFileReadResult = false;
			}

			fclose( file );
		}
		else
		{
			return FALSE;
		}
	}
#else
	file = fopen( m_strPathName.c_str(), "rb" );
	
	if( file )
	{
		fseek( file, 0, SEEK_END );
		iFileSize = ftell( file );
		fseek( file, 0, SEEK_SET );

		if( iFileSize )
		{
			pBuffer = new char[iFileSize + 1];

			//@{ 2006/11/22 burumal
			//memset(pBuffer, 0, iFileSize + 1);
			//@}

			if( fread( pBuffer, 1, iFileSize, file ) != iFileSize )
			{
				bFileReadResult = false;
			}
		}
		else
		{
			bFileReadResult = false;
		}

		fclose( file );
	}
	else
	{
		return FALSE;
	}
#endif

	

	if ( bFileReadResult == false )
		return FALSE;

	if ( bDecryption )
	{
		AuMD5Encrypt MD5;

#ifdef _AREA_CHINA_
		if (!MD5.DecryptString(MD5_HASH_KEY_STRING, pBuffer, (unsigned long)iFileSize))
#else
		if (!MD5.DecryptString(HASH_KEY_STRING, pBuffer, (unsigned long)iFileSize))
#endif
		{
			delete [] pBuffer;
			return FALSE;
		}
	}
	
	//@{ 2006/09/05 burumal
	/*
	// Parn�� �۾� Part Index type�� ���� �տ��� index �� �о �� �κ����� �Ѿ�� �д´�.
	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		if ( !(iOffset = ReadPartIndices( pBuffer + iOffsetBytes )) || lIndex >= m_lPartNum )
		{
			delete [] pBuffer;
			return FALSE;
		}

		iOffsetBytes += iOffset;

		lStartPos = m_plPartIndices[lIndex];

		if ( lIndex + 1 < m_lPartNum )
		{
			lEndPos = m_plPartIndices[lIndex + 1];
		}		
	}
	// Parn�� �۾�
	*/
	//@}

	int nDummyLen;

	BOOL bIniReadFlag = TRUE;
	int	nKeyIndexCount = 0;

	if ( bIniReadFlag == TRUE )
	{
		// lSectionNum�� lKeyNum ���ϱ�
		while( iOffsetBytes < iFileSize )
		{
			iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
			
			//@{ 2006/07/28 burumal
			//iOffset += strlen(CRLF);
			
#ifndef _AREA_CHINA_
			if( iOffsetBytes + iOffset + g_nCRLELength <= iFileSize )
				iOffset += g_nCRLELength;
			//@}
			
			//@{ 2006/08/31 burumal
			//memset(szDummy, 0, sizeof(szDummy));
			//@}

			if( iOffsetBytes + iOffset > iFileSize )
			{
				throw_exception( "������ �������� ���� : %s", m_strPathName.c_str() );
			}

			memcpy_s(szDummy, sizeof(szDummy), pBuffer + iOffsetBytes, iOffset);

			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
			{
				szDummy[iOffset] = '\0';
				if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
				{
					szDummy[ iOffset - 1 ] = 0;
				}
			}
			else
			{
				szDummy[sizeof(szDummy)-1] = '\0';
				throw_exception( "���������� ���� ����, �ʹ���\n %s : %s", m_strPathName.c_str(), szDummy );
			}
			//@}
#else
			iOffset += g_nCRLELength;
			//@}
			
			//@{ 2006/08/31 burumal
			//memset(szDummy, 0, sizeof(szDummy));
			//@}

			memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
				szDummy[iOffset] = '\0';
			//@}
#endif

			iOffsetBytes += iOffset;

			//@{ 2006/08/31 burumal
			nDummyLen = (int)strlen(szDummy);
			//@}

			//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
			if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
			{
				szDummy[strcspn(szDummy, ";")] = '\0';
				nDummyLen = (int)strlen(szDummy);
			}
			
			//Section�߰� 
			//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
			if ( (int)strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
			{
				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
					++lFileSectionNum;
#ifndef _AREA_CHINA_
				else
					throw_exception( "���� ���� �ִ밹�� �ʰ� : %s", m_strPathName.c_str() );
#endif
			}		
			//Key�߰�
			else
			//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
			if ( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
				++lFileKeyNum[lFileSectionNum - 1];
			else if( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 )
				nKeyIndexCount ++;

			if (lEndPos)
			{
				if ( (INT32) iOffsetBytes >= lEndPos)
					break;
			}
		}

		//�޸� �Ҵ� 
		lSectionNum = lFileSectionNum;
		if(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum)
			m_stSection = new stIniSection[ lSectionNum ];
		else
		{
			ASSERT(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum);
			return FALSE;
		}

		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		#ifdef _DEBUG
			/*
			sprintf(pDbgMsg, "Ini File[M] ===> (%s)\n", m_pszPathName);
			OutputDebugStr(pDbgMsg);

			sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
			OutputDebugStr(pDbgMsg);
			*/
		#endif

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			m_stSection[i].lKeyNum = 0;

			if ( lFileKeyNum[i] )
				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
			else
				m_stSection[i].stKey = NULL;

			#ifdef _DEBUG
				/*
				sprintf(pDbgMsg, "\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
				OutputDebugStr(pDbgMsg);
				*/
			#endif
		}
	}

	// Ű�ε����� �ִ°��� , �ε����� ����ϴ� ��� �ۿ� �����ϱ� ���⼭ �÷��� Ȯ��..
	bool bUseKeyIndex = false;
	if( nKeyIndexCount ) bUseKeyIndex = true;

	// ������ ��ġ�� ó������ �ٽ� �ʱ�ȭ
	iOffsetBytes = 0;
		
	//���Ϸ� ���� �Է� 
	lFileSectionNum = 0;
	
	//@{ 2006/07/27 burumal
	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
	//@}

	while( iOffsetBytes < iFileSize )
	{
		// CRLF�� �ִ� ���� ã�´�.
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
		
		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
#ifndef _AREA_CHINA_
		if( iOffsetBytes + iOffset + g_nCRLELength <= iFileSize )
			iOffset += g_nCRLELength;
		//@}

		// CRLF�� �߰ߵ� ������ ������ szDummy�� �����Ѵ�.
		
		//@{ 2006/08/31 burumal
		//memset(szDummy, 0, sizeof(szDummy));
		//@}

		memcpy_s(szDummy, sizeof(szDummy), pBuffer + iOffsetBytes, iOffset);

		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
		{
			szDummy[iOffset] = '\0';
			if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
			{
				szDummy[ iOffset - 1 ] = 0;
			}
		}
		//@}
#else
		iOffset += g_nCRLELength;
		//@}

		// CRLF�� �߰ߵ� ������ ������ szDummy�� �����Ѵ�.
		
		//@{ 2006/08/31 burumal
		//memset(szDummy, 0, sizeof(szDummy));
		//@}

		memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
			szDummy[iOffset] = '\0';
		//@}
#endif // _AREA_CHINA_
		
		iOffsetBytes += iOffset;

		//@{ 2006/08/31 burumal
		nDummyLen = (int) strlen(szDummy);
		//@}
		if ( nDummyLen == 0 )
			continue;
		
		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
		{
			szDummy[strcspn(szDummy, ";")] = '\0';			
			nDummyLen = (int) strlen(szDummy);
		}

		//i = strlen(szDummy) - 1;
		i = nDummyLen - 1;

#ifndef _AREA_CHINA_
		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
#else
		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ') )
#endif
		{
			szDummy[i] = '\0';
			--i;

			//@{ 2006/08/31 burumal
			--nDummyLen;
			//@}
		}	
		
		//Section �߰� 
		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
		if ( (int) strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
		{
			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
						szDummy, 
						nDummyLen,
						1, 
						(int) strcspn( szDummy, "]" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
			}

			++lFileSectionNum;
		}
		//Key�߰� 
		else 
		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
		{
			//Key, Value�Է��ϱ�
			char	strKeyName[ AUINIMANAGER_MAX_NAME ];

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}
			
			// ���⼭ Ű �ε����� ����ϴ°�� 
			// �׳� atoi�� �ϴ� ������ ������.
			if( bUseKeyIndex )
			{
				int	nKeyIndex = atoi( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}
			else
			{
				int	nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}

			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
			}

			++m_stSection[lFileSectionNum-1].lKeyNum;
			++lFileKeyNum[lFileSectionNum-1];
		}
		else if( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 && bUseKeyIndex )
		{
			// Ű ������ ���⼭ �Է¹޾� vector�� ������.
			//Key, Value�Է��ϱ�
			char	strKeyName	[ AUINIMANAGER_MAX_NAME		] = "";
			char	strKeyValue	[ AUINIMANAGER_MAX_KEYVALUE	] = "";

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}

			if ( !StringMid( strKeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy( strKeyValue , "" );
			}

			if( strlen( strKeyName ) && strlen( strKeyValue ) )
			{
				int	nKeyIndex = atoi( strKeyName );
				int nKeyIndexReturned;

				nKeyIndexReturned = GetKeyIndex( strKeyValue );

				ASSERT( nKeyIndex == nKeyIndexReturned );
			}
		}

		if ( lEndPos )
		{
			if ( (INT32) iOffsetBytes >= lEndPos)
				break;
		}
	}


#ifndef _AREA_CHINA_
	if( isZpackStream )
		ReleaseZpackStream( (wchar_t*)packName.c_str(), pBuffer );
	else
#endif
		delete [] pBuffer;

	if( lSectionNum == 0 )
		return FALSE;

	return TRUE;
}

BOOL AuIniManagerA::ParseMemory( char* pBuffer, unsigned long nBufferSize, BOOL bDecryption )
{
	if( !pBuffer ) return FALSE;

	if ( bDecryption )
	{
		AuMD5Encrypt MD5;
#ifndef _AREA_CHINA_
		if (!MD5.DecryptString(HASH_KEY_STRING, pBuffer, nBufferSize))
#else
		if (!MD5.DecryptString(MD5_HASH_KEY_STRING, pBuffer, nBufferSize))
#endif
		{
			return FALSE;
		}
	}
	
	int nDummyLen;

	BOOL bIniReadFlag = TRUE;
	int	nKeyIndexCount = 0;
	size_t iOffsetBytes = 0;
	size_t iOffset		= 0;

	int	lFileSectionNum								= 0		;		//Section�� ��ΰ�. 

	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM]	= {0,}	;		//�� Section�� Key�� ����ΰ� 

	int		i = 0;
	int		lStartPos = 0;
	int		lEndPos	= 0;
	__int64	lCurPos = 0;

	char szDummy[AUINIMANAGER_MAX_NAME + AUINIMANAGER_MAX_KEYVALUE + 1] = { 0, };


	if ( bIniReadFlag == TRUE )
	{
		// lSectionNum�� lKeyNum ���ϱ�
		while( iOffsetBytes < nBufferSize )
		{
			iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
			
			//@{ 2006/07/28 burumal
			//iOffset += strlen(CRLF);
			iOffset += g_nCRLELength;
			//@}
			
			//@{ 2006/08/31 burumal
			//memset(szDummy, 0, sizeof(szDummy));
			//@}

			memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);
#ifndef _AREA_CHINA_
			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
			{
				szDummy[iOffset] = '\0';
				if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
				{
					szDummy[ iOffset - 1 ] = 0;
				}
			}
			//@}
#else
			//@{ 2006/08/31 burumal
			if ( iOffset < sizeof(szDummy) )
				szDummy[iOffset] = '\0';
			//@}
#endif

			iOffsetBytes += iOffset;

			//@{ 2006/08/31 burumal
			nDummyLen = (int)strlen(szDummy);
			//@}

			//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
			if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
			{
				szDummy[strcspn(szDummy, ";")] = '\0';
				nDummyLen = (int)strlen(szDummy);
			}
			
			//Section�߰� 
			//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
			if ( (int)strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
			{
				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
					++lFileSectionNum;
			}		
			//Key�߰�
			else
			//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
			if ( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
				++lFileKeyNum[lFileSectionNum - 1];
			else if( (int)strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 )
				nKeyIndexCount ++;

			if (lEndPos)
			{
				if ( (INT32) iOffsetBytes >= lEndPos)
					break;
			}
		}

		//�޸� �Ҵ� 
		lSectionNum = lFileSectionNum;
		if(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum)
			m_stSection = new stIniSection[ lSectionNum ];
		else
		{
			ASSERT(AUINIMANAGER_MAX_SECTIONNUM > lFileSectionNum);
			return FALSE;
		}

		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		#ifdef _DEBUG
			/*
			sprintf(pDbgMsg, "Ini File[M] ===> (%s)\n", m_pszPathName);
			OutputDebugStr(pDbgMsg);

			sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
			OutputDebugStr(pDbgMsg);
			*/
		#endif

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			m_stSection[i].lKeyNum = 0;

			if ( lFileKeyNum[i] )
				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
			else
				m_stSection[i].stKey = NULL;

			#ifdef _DEBUG
				/*
				sprintf(pDbgMsg, "\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
				OutputDebugStr(pDbgMsg);
				*/
			#endif
		}
	}

	// Ű�ε����� �ִ°��� , �ε����� ����ϴ� ��� �ۿ� �����ϱ� ���⼭ �÷��� Ȯ��..
	bool bUseKeyIndex = false;
	if( nKeyIndexCount ) bUseKeyIndex = true;

	// ������ ��ġ�� ó������ �ٽ� �ʱ�ȭ
	iOffsetBytes = 0;
		
	//���Ϸ� ���� �Է� 
	lFileSectionNum = 0;
	
	//@{ 2006/07/27 burumal
	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
	//@}

	while( iOffsetBytes < nBufferSize )
	{
		// CRLF�� �ִ� ���� ã�´�.
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
		
		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
		iOffset += g_nCRLELength;
		//@}

		// CRLF�� �߰ߵ� ������ ������ szDummy�� �����Ѵ�.
		
		//@{ 2006/08/31 burumal
		//memset(szDummy, 0, sizeof(szDummy));
		//@}

		memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

#ifndef _AREA_CHINA_
		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
		{
			szDummy[iOffset] = '\0';
			if( iOffset > 0 && szDummy[ iOffset - 1 ] == 13 )
			{
				szDummy[ iOffset - 1 ] = 0;
			}
		}
		//@}
#else
		//@{ 2006/08/31 burumal
		if ( iOffset < sizeof(szDummy) )
			szDummy[iOffset] = '\0';
		//@}
#endif
		
		iOffsetBytes += iOffset;

		//@{ 2006/08/31 burumal
		nDummyLen = (int) strlen(szDummy);
		//@}
		if ( nDummyLen == 0 )
			continue;
		
		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
		{
			szDummy[strcspn(szDummy, ";")] = '\0';			
			nDummyLen = (int) strlen(szDummy);
		}

		//i = strlen(szDummy) - 1;
		i = nDummyLen - 1;
#ifndef _AREA_CHINA_
		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
#else
		while ( i >= 0 && (szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ') )
#endif
		{
			szDummy[i] = '\0';
			--i;

			//@{ 2006/08/31 burumal
			--nDummyLen;
			//@}
		}	
		
		//Section �߰� 
		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
		if ( (int) strcspn(szDummy, "[" ) < min(1,strlen(szDummy)) )
		{
			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
						szDummy, 
						nDummyLen,
						1, 
						(int) strcspn( szDummy, "]" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
			}

			++lFileSectionNum;
		}
		//Key�߰� 
		else 
		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
		{
			//Key, Value�Է��ϱ�
			char	strKeyName[ AUINIMANAGER_MAX_NAME ];

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}
			
			// ���⼭ Ű �ε����� ����ϴ°�� 
			// �׳� atoi�� �ϴ� ������ ������.
			if( bUseKeyIndex )
			{
				int	nKeyIndex = atoi( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}
			else
			{
				int	nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;
			}

			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
			}

			++m_stSection[lFileSectionNum-1].lKeyNum;
			++lFileKeyNum[lFileSectionNum-1];
		}
		else if( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum == 0 && bUseKeyIndex )
		{
			// Ű ������ ���⼭ �Է¹޾� vector�� ������.
			//Key, Value�Է��ϱ�
			char	strKeyName	[ AUINIMANAGER_MAX_NAME		] = "";
			char	strKeyValue	[ AUINIMANAGER_MAX_KEYVALUE	] = "";

			if ( !StringMid( strKeyName , 
						szDummy, 
						nDummyLen,
						0, 
						(int) strcspn( szDummy, "=" ) - 1, 
						AUINIMANAGER_MAX_NAME ) )
			{
				strcpy( strKeyName, "" );
			}

			if ( !StringMid( strKeyValue, 
						szDummy, 
						nDummyLen,
						(int) strcspn( szDummy, "=" ) + 1, 
						//(int) strlen( szDummy ) - 1 , 
						nDummyLen - 1,
						AUINIMANAGER_MAX_KEYVALUE ) )
			{
				strcpy( strKeyValue , "" );
			}

			if( strlen( strKeyName ) && strlen( strKeyValue ) )
			{
				int	nKeyIndex = atoi( strKeyName );
				int nKeyIndexReturned;

				nKeyIndexReturned = GetKeyIndex( strKeyValue );

				ASSERT( nKeyIndex == nKeyIndexReturned );
			}
		}

		if ( lEndPos )
		{
			if ( (INT32) iOffsetBytes >= lEndPos)
				break;
		}
	}

	delete [] pBuffer;

	if( lSectionNum == 0 )
		return FALSE;

	return TRUE;
}

// BOOL	AuIniManagerA::FromFile( int lIndex, BOOL bDecryption)
// {
// 	FILE* pFile;
// 	int lMaxCharacter = AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1;
// 	char szDummy[AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1];
// 	int i = 0;
// 	bool bSuccessKey = false, bSuccessValue = false;
// 	int	lFileSectionNum		= 0					;		//Section�� ��ΰ�. 
// 	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM];		//�� Section�� Key�� ����ΰ� 
// 	int lStartPos = 0;
// 	int lEndPos = 0;
// 	__int64 lCurPos = 0;
// 
// 	//@{ 2006/07/27 burumal
// 	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
// 	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
// 	//@}
// 	
// 	//File ���� 
// 	if ( (pFile = fopen( m_pszPathName , "r" )) == NULL )	//File Open
// 		return FALSE;
// 
// 	// Parn�� �۾� Part Index type�� ���� �տ��� index�� �о �� �κ����� �Ѿ�� �д´�.
// 	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
// 	{
// 		if ( !ReadPartIndices( pFile ) || lIndex >= m_lPartNum )
// 		{
// 			fclose( pFile );
// 			return FALSE;
// 		}
// 
// 		lStartPos = m_plPartIndices[lIndex];
// 
// 		if ( fseek( pFile, lStartPos, SEEK_SET) )
// 		{
// 			fclose( pFile );
// 			return FALSE;
// 		}
// 
// 		if ( lIndex + 1 < m_lPartNum)
// 			lEndPos = m_plPartIndices[lIndex + 1];
// 	}
// 	// Parn�� �۾�
// 
// 	int nDummyLen;
// 
// 	BOOL bIniReadFlag = TRUE;
// 
// 	if ( bIniReadFlag )
// 	{
// 		// lSectionNum�� lKeyNum ���ϱ� 
// 		while ( !feof(pFile) )
// 		{
// 			//@{ 2006/08/31 burumal
// 			/*
// 			fgets( szDummy, lMaxCharacter, pFile );
// 
// 			if ( strcspn(szDummy, ";" ) < strlen(szDummy) )
// 				szDummy[strcspn(szDummy, ";")] = '\0';
// 
// 			//Section�߰� 
// 			if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
// 			{
// 				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
// 					++lFileSectionNum;
// 			}		
// 			//Key�߰� 
// 			else 
// 			if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
// 				++lFileKeyNum[lFileSectionNum-1];
// 
// 			if ( lEndPos )
// 			{
// 				if ( fgetpos( pFile, &lCurPos ) )
// 					break;
// 
// 				if ( lCurPos >= lEndPos )
// 					break;
// 			}
// 			*/
// 			fgets(szDummy, lMaxCharacter, pFile);
// 			
// 			nDummyLen = (int) strlen(szDummy);
// 
// 			if ( (int) strcspn(szDummy, ";") < nDummyLen )
// 			{
// 				szDummy[strcspn(szDummy, ";")] = '\0';
// 				nDummyLen = (int) strlen(szDummy);
// 			}
// 
// 			//Section�߰�
// 			if ( (int) strcspn(szDummy, "[" ) < nDummyLen )
// 			{
// 				if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
// 					++lFileSectionNum;
// 			}
// 			//Key�߰�
// 			else
// 			if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
// 				++lFileKeyNum[lFileSectionNum - 1];
// 
// 			if ( lEndPos )
// 			{
// 				if ( fgetpos( pFile, &lCurPos ) )
// 					break;
// 
// 				if ( lCurPos >= lEndPos )
// 					break;
// 			}
// 			//@}
// 		}
// 
// 		//�޸� �Ҵ�
// 		lSectionNum = lFileSectionNum;
// 		m_stSection = new stIniSection[lSectionNum];
// 
// 		//@{ 2006/11/22 burumal
// 		lHalfSectionNum = lSectionNum / 2;
// 		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
// 		//@}
// 
// 	#ifdef _DEBUG
// 		/*
// 		sprintf(pDbgMsg, "Ini File[F] ===> (%s) [%d]\n", m_pszPathName, lIndex);
// 		OutputDebugStr(pDbgMsg);
// 
// 		sprintf(pDbgMsg, "\t\t SectionNum: %d\n", lSectionNum);
// 		OutputDebugStr(pDbgMsg);
// 		*/
// 	#endif
// 
// 		for ( i = 0 ; i < lSectionNum ; ++i )
// 		{
// 			m_stSection[i].lKeyNum = 0;
// 
// 			if ( lFileKeyNum[i] )
// 				m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
// 			else
// 				m_stSection[i].stKey = NULL;
// 
// 		#ifdef _DEBUG		
// 			/*
// 			TRACE("\t\t KeyNum[%d]: %d\n", i, lFileKeyNum[i]);
// 			*/
// 		#endif
// 		}
// 	}
// 
// 	fseek(pFile, lStartPos, SEEK_SET);
// 
// 	//���Ϸ� ���� �Է� 
// 	lFileSectionNum = 0;
// 	
// 	//@{ 2006/07/28 burumal
// 	//for ( i = 0 ; i < AUINIMANAGER_MAX_SECTIONNUM ; ++i ) lFileKeyNum[i] = 0;
// 	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));
// 	//@}
// 
// 	while( !feof(pFile) )
// 	{
// 		fgets( szDummy, lMaxCharacter, pFile );
// 
// 		//@{ 2006/08/31 burumal
// 		nDummyLen = (int) strlen(szDummy);
// 		//@}
// 
// 		//if ( strcspn(szDummy, ";" ) < strlen(szDummy) )	
// 		if ( (int) strcspn(szDummy, ";" ) < nDummyLen )
// 		{
// 			szDummy[strcspn(szDummy, ";")] = '\0';
// 			nDummyLen = (int) strlen(szDummy);
// 		}
// 
// 		//i = (int)strlen(szDummy) - 1;
// 		i = nDummyLen - 1;
// 
// 		while(szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ')
// 		{
// 			szDummy[i] = '\0';
// 			--i;
// 
// 			//@{ 2006/08/31 burumal
// 			--nDummyLen;
// 			//@}
// 		}
// 
// 		//Section �߰� 
// 		//if ( strcspn(szDummy, "[" ) < strlen(szDummy) )
// 		if ( (int) strcspn(szDummy, "[" ) < nDummyLen )
// 		{
// 			if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
// 						szDummy, 
// 						nDummyLen,
// 						1, 
// 						(int)strcspn( szDummy, "]" ) - 1, 
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum].SectionName, "" );
// 			}
// 			
// 			++lFileSectionNum;
// 		}
// 		//Key�߰� 
// 		else
// 		//if ( strcspn(szDummy, "=" ) < strlen(szDummy) && lFileSectionNum > 0 )
// 		if ( (int) strcspn(szDummy, "=" ) < nDummyLen && lFileSectionNum > 0 )
// 		{
// 			//Key, Value�Է��ϱ�
// 			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyName, 
// 						szDummy, 
// 						nDummyLen,
// 						0, 
// 						(int)strcspn( szDummy, "=" ) - 1, 
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyName, "" );
// 			}
// 			
// 			if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
// 						szDummy, 
// 						nDummyLen,
// 						(int)strcspn( szDummy, "=" ) + 1, 
// 						//(int)strlen( szDummy ) - 1 , 
// 						nDummyLen - 1,
// 						AUINIMANAGER_MAX_NAME ) )
// 			{
// 				strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
// 			}
// 			
// 			++m_stSection[lFileSectionNum-1].lKeyNum;
// 			++lFileKeyNum[lFileSectionNum-1];
// 		}
// 
// 		if ( lEndPos )
// 		{
// 			if ( fgetpos( pFile, &lCurPos ) )
// 				break;
// 
// 			if ( lCurPos >= lEndPos )
// 				break;
// 		}
// 	}
// 	
// 	fclose(pFile);											//File Close
// 
// 	if( lSectionNum == 0 )	return FALSE;
// 
// 	return FALSE;	
// }

BOOL	AuIniManagerA::FromFile( int lIndex, BOOL bDecryption)
{
	FILE* pFile;
	int lMaxCharacter = AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1;	
	char* szDummyPtr;
	int i = 0;
	bool bSuccessKey = false, bSuccessValue = false;
	int	lFileSectionNum		= 0					;		//Section�� ��ΰ�. 
	int	lFileKeyNum[AUINIMANAGER_MAX_SECTIONNUM];		//�� Section�� Key�� ����ΰ� 
	int lStartPos = 0;
	int lEndPos = 0;
	__int64 lCurPos = 0;

	ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));

#ifndef _AREA_CHINA_
	bool isZpackStream = true;

	char* pSrcReadData = 0;
	size_t fileSize = 0;
	int nDummyLen = 0;	
	int nTotalDataSize = 0;

	//if( m_strPathName == "Ini\\Obj01817.ini" )
	//	ASSERT(FALSE);

	umtl::static_pool::wstring packName = GetPackName();

	if( !packName.empty() )
	{
		pSrcReadData = (char*)GetZpackStream( (wchar_t*)packName.c_str(), m_strPathName.c_str(), fileSize );
	}

	char * orgStream = pSrcReadData;

	if( pSrcReadData )
	{
		if( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
		{
			if( !ReadPartIndices( pSrcReadData ) || lIndex > m_lPartNum )
			{
				ReleaseZpackStream( (wchar_t*)packName.c_str(), orgStream );
				pSrcReadData = 0;
				return FALSE;
			}

			lStartPos = m_plPartIndices[lIndex];

			pSrcReadData += lStartPos;

			if( lIndex + 1 < m_lPartNum )
				lEndPos = m_plPartIndices[lIndex + 1];
		}

		if( lEndPos == 0 )
			lEndPos = (int)(fileSize);

		if( lEndPos <= lStartPos )
		{
			ReleaseZpackStream( (wchar_t*)packName.c_str(), orgStream );
			pSrcReadData = 0;
			return FALSE;
		}

		nTotalDataSize = (lEndPos - lStartPos);

		if( nTotalDataSize>0 && nTotalDataSize <= (int)(fileSize-lStartPos) )
			pSrcReadData[nTotalDataSize]=0;
	}
	else
	{
		isZpackStream = false;
#endif

		//File ���� 
		if ( (pFile = fopen( m_strPathName.c_str() , "r" )) == NULL )	//File Open
			return FALSE;

		// Parn�� �۾� Part Index type�� ���� �տ��� index�� �о �� �κ����� �Ѿ�� �д´�.
		if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
		{
			if ( !ReadPartIndices( pFile ) || lIndex >= m_lPartNum )
			{
				fclose( pFile );
				return FALSE;
			}

			lStartPos = m_plPartIndices[lIndex];

			if ( fseek( pFile, lStartPos, SEEK_SET) )
			{
				fclose( pFile );
				return FALSE;
			}

			if ( lIndex + 1 < m_lPartNum)
				lEndPos = m_plPartIndices[lIndex + 1];
		}
		// Parn�� �۾�
#ifdef _AREA_CHINA_
		int nDummyLen;	
		char* pSrcReadData;
#endif

		if ( lEndPos == 0 )
		{
			fseek(pFile, 0, SEEK_END);
			lEndPos = ftell(pFile);
			fseek(pFile, lStartPos, SEEK_SET);
		}

		if ( lEndPos <= lStartPos )
		{
			fclose(pFile);
			return FALSE;
		}
#ifndef _AREA_CHINA_
		nTotalDataSize = (lEndPos - lStartPos);
#else
		int nTotalDataSize = (lEndPos - lStartPos);
#endif
		pSrcReadData = new char[nTotalDataSize];
		if ( !pSrcReadData )
		{
			fclose(pFile);
			return FALSE;
		}

		fread(pSrcReadData, nTotalDataSize, 1, pFile);
		fclose(pFile);
#ifndef _AREA_CHINA_
	}
#endif

	static DebugValueString	_stValue( "no file" , "Ini Access File = %s" );
	_stValue	= m_strPathName.c_str();

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
		char nBackupChar;
		char* pBackupCharPtr;
		int nBackupDummyPos;

		size_t iOffsetBytes = 0;
		size_t iOffset		= 0;

		BOOL bIniReadFlag = TRUE;

		if ( bIniReadFlag )
		{		
			while ( true )
			{
				iOffset = strcspn(pSrcReadData + iOffsetBytes, CRLF);

#ifndef _AREA_CHINA_
				//if ( pSrcReadData[iOffset] == '\r' && pSrcReadData[iOffset + 1] == '\n' )
				if ( pSrcReadData[iOffsetBytes+iOffset] == '\r' && pSrcReadData[iOffsetBytes+iOffset + 1] == '\n' )
#else
				if ( pSrcReadData[iOffset] == '\r' && pSrcReadData[iOffset + 1] == '\n' )
#endif
				{
					iOffset += g_nCRLELength;
				}
				else
				{
					iOffset++;
					nTotalDataSize--;
				}

				nBackupChar = *(pSrcReadData + iOffsetBytes + iOffset);
				*(pSrcReadData + iOffsetBytes + iOffset) = NULL;	

				szDummyPtr = pSrcReadData + iOffsetBytes;

				iOffsetBytes += iOffset;

				nDummyLen = (int) strlen(szDummyPtr);

#ifndef _AREA_CHINA_
				if( nDummyLen > 0 && szDummyPtr[nDummyLen-1] == 13 )
				{
					--nDummyLen;
					szDummyPtr[nDummyLen] = 0;
				}
#endif

				if ( (nBackupDummyPos = (int)strcspn(szDummyPtr, ";" )) < nDummyLen )
				{
					szDummyPtr[nBackupDummyPos] = '\0';
					nDummyLen = (int) strlen(szDummyPtr);
				}
				else
				{
					nBackupDummyPos = -1;
				}

				//Section�߰�
				if ( (int) strcspn(szDummyPtr, "[" ) < nDummyLen )
				{
					if ( lFileSectionNum < AUINIMANAGER_MAX_SECTIONNUM )
						++lFileSectionNum;
				}		
				//Key�߰�
				else
					if ( (int) strcspn(szDummyPtr, "=" ) < nDummyLen && lFileSectionNum > 0 )
						++lFileKeyNum[lFileSectionNum - 1];

				if ( nBackupDummyPos != -1 )
					szDummyPtr[nBackupDummyPos] = ';';

				*(pSrcReadData + iOffsetBytes) = nBackupChar;

				if ( (INT32) iOffsetBytes >=  nTotalDataSize )
					break;
			}

			//�޸� �Ҵ�
			lSectionNum = lFileSectionNum;
			m_stSection = new stIniSection[lSectionNum];

			lHalfSectionNum = lSectionNum / 2;
			bIsSectionNumOdd = (lSectionNum % 2) ? true : false;

			for ( i = 0 ; i < lSectionNum ; ++i )
			{
				m_stSection[i].lKeyNum = 0;

				if ( lFileKeyNum[i] )
					m_stSection[i].stKey = new stIniKey[ lFileKeyNum[i] ];
				else
					m_stSection[i].stKey = NULL;
			}
		}

		iOffsetBytes = 0;

		//���Ϸ� ���� �Է� 
		lFileSectionNum = 0;

		ZeroMemory(lFileKeyNum, sizeof(lFileKeyNum));

		while ( true )
		{
			iOffset = strcspn(pSrcReadData + iOffsetBytes, CRLF);
#ifndef _AREA_CHINA_
			if ( pSrcReadData[iOffsetBytes+iOffset] == '\r' && pSrcReadData[iOffsetBytes+iOffset + 1] == '\n' )
#else
			if ( pSrcReadData[iOffset] == '\r' && pSrcReadData[iOffset + 1] == '\n' )
#endif
				iOffset += g_nCRLELength;
			else
				iOffset++;

			pBackupCharPtr = pSrcReadData + iOffsetBytes + iOffset - 1;
#ifndef _AREA_CHINA_
			while ( pBackupCharPtr && (*pBackupCharPtr == '\n' || *pBackupCharPtr == '\t' || *pBackupCharPtr == '\r' || *pBackupCharPtr == ' ' || *pBackupCharPtr == 13 ) )
#else
			while ( pBackupCharPtr && (*pBackupCharPtr == '\n' || *pBackupCharPtr == '\t' || *pBackupCharPtr == '\r' || *pBackupCharPtr == ' ') )
#endif
				pBackupCharPtr--;

			pBackupCharPtr++;
			nBackupChar = *pBackupCharPtr;
			*pBackupCharPtr = NULL;

			szDummyPtr = pSrcReadData + iOffsetBytes;

			iOffsetBytes += iOffset;

			nDummyLen = (int) strlen(szDummyPtr);
#ifndef _AREA_CHINA_
			if( nDummyLen > 0 && szDummyPtr[nDummyLen-1] == 13 )
			{
				--nDummyLen;
				szDummyPtr[nDummyLen] = 0;
			}
#endif

			if ( (nBackupDummyPos = (int)strcspn(szDummyPtr, ";" )) < nDummyLen )
			{
				szDummyPtr[nBackupDummyPos] = '\0';
				nDummyLen = (int) strlen(szDummyPtr);
			}
			else
			{
				nBackupDummyPos = -1;
			}

			//Section�߰�
			if ( (int) strcspn(szDummyPtr, "[" ) < nDummyLen )
			{
				if ( !StringMid(m_stSection[lFileSectionNum].SectionName, 
					szDummyPtr, 
					nDummyLen,
					1, 
					(int)strcspn( szDummyPtr, "]" ) - 1, 
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy(m_stSection[lFileSectionNum].SectionName, "" );
				}

				++lFileSectionNum;
			}		
			//Key�߰�
			else if ( (int) strcspn(szDummyPtr, "=" ) < nDummyLen && lFileSectionNum > 0 )
			{
				char	strKeyName[ AUINIMANAGER_MAX_NAME ];
				//Key, Value�Է��ϱ�
				if ( !StringMid( strKeyName , 
					szDummyPtr, 
					nDummyLen,
					0, 
					(int)strcspn( szDummyPtr, "=" ) - 1, 
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy( strKeyName, "" );
				}

				int nKeyIndex = GetKeyIndex( strKeyName );
				m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].nKeyIndex = nKeyIndex;

				if ( !StringMid(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, 
					szDummyPtr, 
					nDummyLen,
					(int)strcspn( szDummyPtr, "=" ) + 1, 				
					nDummyLen - 1,
					AUINIMANAGER_MAX_NAME ) )
				{
					strcpy(m_stSection[lFileSectionNum-1].stKey[ lFileKeyNum[lFileSectionNum-1] ].KeyValue, "" );
				}

				++m_stSection[lFileSectionNum-1].lKeyNum;
				++lFileKeyNum[lFileSectionNum-1];
			}

			if ( nBackupDummyPos != -1 )
				szDummyPtr[nBackupDummyPos] = ';';

			*pBackupCharPtr = nBackupChar;

			if ( (INT32) iOffsetBytes >=  nTotalDataSize )
				break;
		}	
	}
	else if( IsProcessMode( AuIniManagerA::BIN ) )
	{
		// Binary ����Ÿ �б�....
		// ��� ¥���ϳ� orz..
	}

	ASSERT(pSrcReadData);
#ifndef _AREA_CHINA_
	if( isZpackStream )
		ReleaseZpackStream( (wchar_t*)packName.c_str(), orgStream );
	else
#endif
		delete [] pSrcReadData;

	if( lSectionNum == 0 )	return FALSE;

	return FALSE;	
}

/*****************************************************************
*   Function : StringMid
*   Date&Time : 2002-12-05, ���� 9:52
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::StringMid( char* pDest, char* pSource, 
		//@{ 2006/09/01 burumal
		int nSrcLen,
		//@}
		int lStart, int lEnd, int lDestLength )		
{
	if ( lStart > lEnd ) {
		return false;
	}
	
	//@{ 2006/09/01 burumal
	//if ( lStart < 0 || (int)strlen(pSource) <= lEnd ){
	if ( lStart < 0 || nSrcLen <= lEnd ){
	//@}
		ASSERT(false);
		return false;
	}

#ifndef _AREA_CHINA_
	int lCopyNum = lEnd - lStart + 1;

	if( lDestLength < lCopyNum )
	{
		ASSERT(false);
		return false;
	}
#else
	if ( lDestLength < lEnd ) {
		ASSERT(false);
		return false;
	}

	int lCopyNum = lEnd - lStart + 1;
#endif

	//for ( int i = 0 ; i < lCopyNum ; ++i )
		//pDest[i] = pSource[ lStart + i ];

	memcpy((void*)pDest,(void*)(pSource+lStart),lCopyNum);
	
	pDest[lCopyNum] = '\0';

	return true;
}

/*****************************************************************
*   Function : GetSectionName
*   Comment  : ��ȿ���� ���� ID�� ������ NULL�� ���� 
*   Date&Time : 2002-12-06, ���� 6:04
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetSectionName			( int lSectionID ) 
{ 
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return NULL;

	return m_stSection[lSectionID].SectionName;	
}	

/*****************************************************************
*   Function : GetNumKeys
*   Comment  : ��ȿ���� ���� ID�� ������ -1 ����
*   Date&Time : 2002-12-06, ���� 6:06
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int		AuIniManagerA::GetNumKeys			( int lSectionID )
{
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
   		// txt mode process
		return m_stSection[lSectionID].lKeyNum;		
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
   		// bin mode process
		return ( int ) m_vecSection[ lSectionID ].vecKey.size();
	}

	return -1;
}	

/*****************************************************************
*   Function : GetKeyName
*   Comment  : ��ȿ���� ���� ID�� ������ NULL�� ���� 
*   Date&Time : 2002-12-06, ���� 6:07
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
const char*	AuIniManagerA::GetKeyName  ( int lSectionID, int lkeyID )
{ 
	if ( lSectionID >= lSectionNum || lSectionID < 0 ) return NULL;
	if ( lkeyID >= GetNumKeys( lSectionID ) || lkeyID < 0 ) return NULL;

	return GetKeyNameTable( m_stSection[lSectionID].stKey[lkeyID].nKeyIndex );
}

/*****************************************************************
*   Function : GetValue
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 10:20
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetValue( int lSectionID, int lKeyID, char* defValue )
{
	static char strDummy[] = "";
	
	//default���� �����ش�. 
	if ( lSectionID >= lSectionNum  || lKeyID >= GetNumKeys( lSectionID ) || lSectionID < 0 || lKeyID < 0 )
	{
		if ( defValue == NULL )
			return strDummy;
		else 
			return defValue;
	}

	return m_stSection[lSectionID].stKey[lKeyID].KeyValue;
}
	
/*****************************************************************
*   Function : GetValue
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 10:20
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
char*	AuIniManagerA::GetValue( const char* pszSectionName, char* pszKeyName, char* defValue )
{
	static char strDummy[] = "";

	//@{ 2006/09/04 burumal
	if ( pszSectionName == NULL || pszKeyName == NULL )
		return NULL;
	//@}

	int SectionID	= 0;
	int KeyID		= 0;
	bool	bSearchSection = false, bSearchKey = false;
	
	for ( int i = 0 ; i < lSectionNum ; ++i )
	{
		if ( !strcmp( pszSectionName, m_stSection[i].SectionName ) )
		{
			SectionID = i;		bSearchSection = true;

			//@{ 2006/09/04 burumal
			break;
			//@}
		}
	}

	int	nKeyIndex = GetKeyIndex( pszKeyName );

	if ( bSearchSection == true )
	{
		for ( int j = 0 ; j < GetNumKeys( SectionID ) ; ++j )
		{
			if ( nKeyIndex == m_stSection[SectionID].stKey[j].nKeyIndex )
			{
				KeyID = j;		bSearchKey = true;

				//@{ 2006/09/04 burumal
				break;
				//@}
			}
		}
	}

	if ( bSearchSection == true && bSearchKey == true )
	{
		return m_stSection[SectionID].stKey[KeyID].KeyValue;
	}
	else 
	{
		if ( defValue == NULL )
			return strDummy;
		else 
			return defValue;
	}

	return strDummy;
}

/*****************************************************************
*   Function : GetStringValue
*   Comment  : Get Value �� String�� pszDest�� �־��ش�. 
*   Date&Time : 2002-12-13, ���� 3:29
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::GetStringValue( const char* pszSectionName, char* pszKeyName, char* pszDest, int lszDestLength, char* defValue )
{
	char* pszGetString;
	pszGetString = GetValue( pszSectionName, pszKeyName, defValue );
	strncpy( pszDest, pszGetString, lszDestLength );

	return true;
}

/*****************************************************************
*   Function : GetStringValue
*   Comment  : Get Value �� String�� pszDest�� �־��ش�.
*   Date&Time : 2003-02-12, ���� 4:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool	AuIniManagerA::GetStringValue( int lSectionID, int lKeyID, char*	pszDest,  int lszDestLength, char*	defValue )
{
	char* pszGetString;
	pszGetString = GetValue( lSectionID, lKeyID, defValue );
	strncpy( pszDest, pszGetString, lszDestLength );

	return true;
}

/*****************************************************************
*   Function : GetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 10:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int		AuIniManagerA::GetValueI( const char* pszSectionName, char* pszKeyName, int defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return atoi( strValue );
} 

/*****************************************************************
*   Function : GetValueI64
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 10:45
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
__int64		AuIniManagerA::GetValueI64( const char* pszSectionName, char* pszKeyName, __int64 defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return _atoi64( strValue );
} 

/*****************************************************************
*   Function : GetValueF
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 10:52
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
double	AuIniManagerA::GetValueF( const char* pszSectionName, char* pszKeyName, double defValue )
{
	char* strValue = GetValue( pszSectionName, pszKeyName );

	//@{ 2006/09/04 burumal
	//if ( !strcmp( strValue, "" ) || strValue == NULL )
	if ( strValue == NULL || strValue[0] == NULL )
	//@}
	{
		return defValue;
	}
	else
		return atof( strValue );
}

/*****************************************************************
*   Function : FindSection
*   Comment  : ã�� ���ϸ� -1�� �����ش�. 
*   Date&Time : 2002-12-06, ���� 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindSection( const char* pszSection )
{
	//@{ 2006/09/04 burumal
	if ( pszSection == NULL )
		return -1; 
	//@}

	//@{ 2006/11/22 burumal
	/*
	for ( int i = 0 ; i < lSectionNum ; ++i )
	{
		if ( !strcmp( pszSection, m_stSection[i].SectionName ) )
				return i;
	}
	*/
	if ( lSectionNum <= 0 )
		return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		for ( int i = 0; i < lHalfSectionNum; ++i )
		{
			if ( !strcmp( pszSection, m_stSection[i].SectionName ) )
				return i;

			if ( !strcmp( pszSection, m_stSection[lSectionNum - i - 1].SectionName ) )
				return (lSectionNum - i - 1);
		}

		if ( bIsSectionNumOdd )
		{
			if ( !strcmp( pszSection, m_stSection[lHalfSectionNum].SectionName ) )
				return lHalfSectionNum;
		}
		//@}	
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		for ( int i = 0; i < lHalfSectionNum; ++i )
		{
			if ( !strcmp( pszSection, m_vecSection[i].strSection.c_str() ) )
				return i;

			if ( !strcmp( pszSection, m_vecSection[lSectionNum - i - 1].strSection.c_str() ) )
				return (lSectionNum - i - 1);
		}

		if ( bIsSectionNumOdd )
		{
			if ( !strcmp( pszSection, m_vecSection[lHalfSectionNum].strSection.c_str() ) )
				return lHalfSectionNum;
		}
		//@}
	}
	return -1;
}

/*****************************************************************
*   Function : FindKey
*   Comment  : ã�� ���ϸ� -1�� �����ش�. 
*   Date&Time : 2002-12-06, ���� 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindKey( const char* pszSection, const char* pszKey )
{
	//@{ 2006/09/04 burumal
	if ( pszSection == NULL || pszKey == NULL )
		return -1;
	//@}

	int SectionID = FindSection( pszSection );
	if ( SectionID < 0 ) return -1;

	return FindKey( SectionID , pszKey );
}

/*****************************************************************
*   Function : FindKey
*   Comment  : ã�� ���ϸ� -1 �����ش�. 
*   Date&Time : 2002-12-06, ���� 11:12
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::FindKey( int SectionID, const char* pszKey )
{
	//@{ 2006/09/04 burumal
	if ( pszKey == NULL )
		return -1;
	//@}

	if ( SectionID >= lSectionNum || SectionID < 0 ) return -1;

	int nKeyIndex	= GetKeyIndex( pszKey );

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniSection	* pSection = &m_stSection[SectionID];

		if ( pSection->lKeyNum <= 0 )
			return -1;

		for( int j = 0 ; j < pSection->lKeyNum ; j ++ )
		{
			if( pSection->stKey[ j ].nKeyIndex == nKeyIndex )
				return j;
		}

		return -1;
	}
	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniSectionBinary * pSection = &m_vecSection[SectionID];

		if ( pSection->vecKey.size() <= 0 )
			return -1;

		for( int j = 0 ; j < ( int ) pSection->vecKey.size() ; j ++ )
		{
			if( pSection->vecKey[ j ].nKeyIndex == nKeyIndex )
				return j;
		}

		return -1;
	}

	return -1;
}

/*****************************************************************
*   Function : SetValue
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 11:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/	
bool AuIniManagerA::SetValue( int SectionID, int KeyID, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	if ( SectionID >= lSectionNum || SectionID < 0 ) return false;
	if ( KeyID >= GetNumKeys( SectionID ) || KeyID < 0 ) return false;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
		stIniKey	* pKey = &m_stSection[SectionID].stKey[KeyID];

   		// txt mode process
		pKey->eType	= eType;

		switch( eType )
		{
		case KT_I		:	sprintf( pKey->KeyValue , "%d" , data.i );						break;
		case KT_I64		:	sprintf( pKey->KeyValue , "%d" , data.i64 );					break;
		case KT_F		:	
							{
								// ���⼭ �뷮�� �� ���δ�.

								//sprintf( pKey->KeyValue , "%f" , data.f );
								print_compact_float( pKey->KeyValue , data.f );
								break;
							}
		case KT_STR		:	strncpy( pKey->KeyValue, data.str, AUINIMANAGER_MAX_KEYVALUE );	break;
		}
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
   		// bin mode process

		stIniKeyBinary	*pKey	= &m_vecSection[ SectionID ].vecKey[ KeyID ];

		pKey->eType	= eType	;
		pKey->data	= data	;
	}

	return true;
}

/*****************************************************************
*   Function : SetValue
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 11:17
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValue( const char* pszSectionName, const char* pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	//@{ 2006/09/04 burumal
	if ( pszSectionName == NULL || pszKeyName == NULL )
		return false;
	//@}

	int SectionID	= 0;
	int KeyID		= 0;

	SectionID = FindSection( pszSectionName );

	if ( SectionID >= lSectionNum ) return false;
	if ( SectionID < 0 ) 
	{
		SectionID = AddSection( pszSectionName );		
		
		if ( SectionID < 0 ) return false;
	}
	
	if ( m_eMode != AUINIMANAGER_MODE_NAME_OVERWRITE)
	{
		KeyID = FindKey( SectionID, pszKeyName );
		
		// �Ʒ� �ּ��� ������찡 �߻��ؼ� �ȵȴ�.
		//if ( KeyID >= m_stSection[SectionID].lKeyNum ) return false;
	}
	else
		KeyID = -1;

	if ( KeyID < 0 )				//�ش��ϴ� KeyName�� ���� ��� 
	{
		KeyID = AddKey( SectionID, pszKeyName, eType , data );

		if ( KeyID < 0  ) return false;
		else return true;
	}
	
	SetValue( SectionID , KeyID , eType , data );
	return true;
}

/*****************************************************************
*   Function : SetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 11:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueI( const char* pszSectionName, const char* pszKeyName, int value )
{
	stIniKeyBinary::DATA data;
	data.i	= value;
	return SetValue( pszSectionName, pszKeyName, KT_I , data );
}

bool AuIniManagerA::SetValue( const char*	pszSectionName, const char*	pszKeyName, const char* pszvalue)
{	
	stIniKeyBinary::DATA data;
	strncpy( data.str , pszvalue , AUINIMANAGER_MAX_KEYVALUE );
	return SetValue( pszSectionName, pszKeyName, KT_STR , data );
}

/*****************************************************************
*   Function : SetValueI
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 11:48
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueI64( const char* pszSectionName, const char* pszKeyName, __int64 value )
{	
	stIniKeyBinary::DATA data;
	data.i64	= value;
	return SetValue( pszSectionName, pszKeyName, KT_I64 , data );
}

/*****************************************************************
*   Function : SetValueF
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 11:51
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
bool AuIniManagerA::SetValueF( const char* pszSectionName, const char* pszKeyName, double value )
{	
	stIniKeyBinary::DATA data;

	// ���⼭ ������ �����Դ°� �� �Ű澲��.
	data.f	= ( float ) value;
	return SetValue( pszSectionName, pszKeyName, KT_F , data );
}

/*****************************************************************
*   Function : AddSection
*   Comment  : �����ϸ� ID �����ϸ� -1�� �����ش�. 
*   Date&Time : 2002-12-06, ���� 11:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int AuIniManagerA::AddSection( const char* pszSectionName )
{	
	int FindIndex = FindSection( pszSectionName );
	if ( FindIndex >= 0 ) return -1;

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniSection *pszDummy = NULL;
		pszDummy = (stIniSection*) realloc( m_stSection, sizeof(stIniSection) * ( lSectionNum + 1 ) );
		
		if ( pszDummy == NULL )  return -1;

		m_stSection = pszDummy;
		
		++lSectionNum;
				
		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		m_stSection[lSectionNum-1].lKeyNum = 0;
		m_stSection[lSectionNum-1].stKey = NULL;
		strncpy( m_stSection[lSectionNum-1].SectionName, pszSectionName, AUINIMANAGER_MAX_NAME );

		return ( lSectionNum - 1 );	
	}
	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniSectionBinary	stSection;
		stSection.strSection	= pszSectionName;
		
		m_vecSection.push_back( stSection );
			
		lSectionNum = ( int ) m_vecSection.size();
				
		//@{ 2006/11/22 burumal
		lHalfSectionNum = lSectionNum / 2;
		bIsSectionNumOdd = (lSectionNum % 2) ? true : false;
		//@}

		return ( lSectionNum - 1 );	
	}

	return -1;
}

/*****************************************************************
*   Function : AddKey
*   Comment  : �����ϸ� ID �����ϸ� -1�� �����ش� .
*   Date&Time : 2002-12-06, ���� 11:32
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int AuIniManagerA::AddKey( int SectionID, const char* pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	//@{ 2006/09/04 burumal
	if ( pszKeyName == NULL )
		return -1;
	//@}

	if ( SectionID >= lSectionNum || SectionID < 0 ) return -1;
	
	int FindIndex;
	
	if( m_eMode != AUINIMANAGER_MODE_NAME_OVERWRITE)
	{
		FindIndex = FindKey( SectionID, pszKeyName );
		if( FindIndex >= 0 ) return -1;
	}

	if( IsProcessMode( AuIniManagerA::TXT ) )
	{
	   // txt mode process
		stIniKey *pszDummy = NULL;

		pszDummy = (stIniKey*) realloc( m_stSection[SectionID].stKey,  sizeof(stIniKey) * ( m_stSection[SectionID].lKeyNum + 1 ) );

		if ( pszDummy == NULL ) return -1;
		m_stSection[SectionID].stKey = pszDummy;

		++m_stSection[SectionID].lKeyNum;

		int KeyID	= m_stSection[SectionID].lKeyNum - 1;

		int	nKeyIndex = GetKeyIndex( pszKeyName );
		
		m_stSection[SectionID].stKey[ KeyID ].nKeyIndex	= nKeyIndex;
		SetValue( SectionID , KeyID , eType , data );

		return KeyID;
	}

	else if ( IsProcessMode( AuIniManagerA::BIN ) )
	{
	   // bin mode process
		stIniKeyBinary	stKey;

		int	nKeyIndex = GetKeyIndex( pszKeyName );
		stKey.nKeyIndex	= nKeyIndex;

		stIniSectionBinary *pSection = &m_vecSection[ SectionID ];

		pSection->vecKey.push_back( stKey );
		int KeyID = ( int ) pSection->vecKey.size() - 1;

		SetValue( SectionID , KeyID , eType , data );
		return KeyID;
	}

	// ����� ���� �ȵſ�
	ASSERT( !"�ӽö�!" );
	return -1;
}

/*****************************************************************
*   Function : AddKey
*   Comment  : �����ϸ� ID �����ϸ� -1 �����ش� .
*   Date&Time : 2002-12-06, ���� 11:40
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
int	AuIniManagerA::AddKey( const char* pszSection, const char*pszKeyName, KEYTYPE eType , stIniKeyBinary::DATA data )
{
	int SectionID = FindSection( pszSection );
	if ( SectionID < 0 ) return -1;

	return AddKey( SectionID, pszKeyName, eType , data );
}

/*****************************************************************
*   Function : WriteFile
*   Comment  : 
*   Date&Time : 2002-12-06, ���� 12:11
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
BOOL	AuIniManagerA::WriteFile	(int lIndex,  BOOL bEncryption)
{
	if ( m_strPathName.size() == 0 ) return FALSE;

	FILE *pFile;
	char szDummy[AUINIMANAGER_MAX_NAME+AUINIMANAGER_MAX_KEYVALUE+1];
	int i = 0 , j = 0;
	char *pPreBuffer = NULL;
	char *pPostBuffer = NULL;
	int lPreBuffer = 0;
	int lPostBuffer = 0;
	__int64 lCurPos;

	if ( GetType() & AUINIMANAGER_TYPE_PART_INDEX )
	{
		if ( lIndex && (pFile = fopen( m_strPathName.c_str() , "rt" )) )
		{
			if ( !ReadPartIndices(pFile) )
			{
				fclose(pFile);
				return FALSE;
			}

			fclose(pFile);
		}
		else
		{
			pFile = fopen( m_strPathName.c_str() , "wt" );
			if( pFile ) fclose( pFile );
			else
			{
				TRACE( "AuIniManagerA::WriteFile Write Stream Open Fail\n" );
			}
			
			memset(m_plPartIndices, 0, sizeof(int) * m_lPartNum);
		}

		pFile = fopen( m_strPathName.c_str() , "r+t" );
		if ( !pFile )
		{
			TRACE( "AuIniManagerA::WriteFile Read + Stream Open Fail\n" );
			return FALSE;
		}

		WritePartIndices(pFile);

		fseek(pFile, 0, SEEK_END);

		fgetpos(pFile, &lCurPos);

		for ( i = 0 ; i < lSectionNum ; ++i )
		{
			sprintf( szDummy, "[%s]\n", m_stSection[i].SectionName );
			fputs( szDummy, pFile );
			
			for ( j = 0 ; j < GetNumKeys( i ) ; ++j )
			{
				const char * pKeyName = GetKeyNameTable( m_stSection[i].stKey[j].nKeyIndex );
				sprintf( szDummy, "%s=%s\n", pKeyName , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
		}

		m_plPartIndices[lIndex] = (int) lCurPos;

		WritePartIndices(pFile);
		fclose(pFile);

		return FALSE;
	}

	if ( (pFile = fopen( m_strPathName.c_str() , "wt" )) == NULL )	//File Open
	{
		//File�� ReadOnly��� �Ӽ��� �ٲٰ� �ٽ��ѹ� �о� �д�. 
		DWORD lFileAttribute = GetFileAttributes( m_strPathName.c_str() );
		
		if ( FILE_ATTRIBUTE_READONLY == ( lFileAttribute & FILE_ATTRIBUTE_READONLY ) )
		{
			SetFileAttributes( 	m_strPathName.c_str(), lFileAttribute & ~FILE_ATTRIBUTE_READONLY );
			if ( (pFile = fopen( m_strPathName.c_str() , "wt" )) == NULL ) return FALSE;
		}
		else
			return FALSE;
	}

	bool	bUseKeyIndex = GetType() & AUINIMANAGER_TYPE_KEYINDEX ? true : false;
	if( bUseKeyIndex )
	{
		// Key String Write
		vector< string >::iterator	iter = m_vecKeyTable.begin();
		for( int	nCount = 0;
			iter != m_vecKeyTable.end();
			iter++ , nCount++ )
		{
			const char * pKeyName = GetKeyNameTable( nCount );
			sprintf( szDummy, "%d=%s\n", nCount , pKeyName );
			fputs( szDummy, pFile );
		}
	}

	for ( i = 0 ; i < lSectionNum ; ++i )
	{
		sprintf( szDummy, "[%s]\n", m_stSection[i].SectionName );
		fputs( szDummy, pFile );
		
		for ( j = 0 ; j < GetNumKeys( i ) ; ++j )
		{
			if( bUseKeyIndex )
			{
				sprintf( szDummy, "%d=%s\n", m_stSection[i].stKey[j].nKeyIndex , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
			else
			{
				const char * pKeyName = GetKeyNameTable( m_stSection[i].stKey[j].nKeyIndex );
				sprintf( szDummy, "%s=%s\n", pKeyName , m_stSection[i].stKey[j].KeyValue );
				fputs( szDummy, pFile );
			}
		}
	}

	fclose(pFile);

	if (bEncryption)
		EncryptSave();

	// ������ (2004-05-31 ���� 6:02:03) : �̰� �� ���� ������? ;;
	return TRUE;
	//return FALSE;
}

/*****************************************************************
*   Function : ClearAllSectionKey
*   Comment  : All Section, All Key Data Clear
*   Date&Time : 2003-01-29, ���� 3:00
*   Code By : Seong Yon-jun@NHN Studio 
*****************************************************************/
void	AuIniManagerA::ClearAllSectionKey()
{
	if ( m_stSection != NULL )
	{
		for ( int i = 0 ; i < lSectionNum ; ++i )
		{
			if ( m_stSection[i].stKey != NULL ) 
			{
				delete [] m_stSection[i].stKey;
				m_stSection[i].stKey = NULL;
			}
		}

		delete [] m_stSection;
		m_stSection = NULL;
	}

	lSectionNum = 0;

	m_vecSection.clear();

	//@{ 2006/11/22 burumal
	lHalfSectionNum = 0;
	bIsSectionNumOdd = false;
	//@}
}

bool	AuIniManagerA::SetMode(AuIniManagerMode eMode)
{
	m_eMode = eMode;

	return true;
}

bool	AuIniManagerA::SetType(DWORD eType)
{
	m_eType = eType;

	return true;
}

//@{ 2006/09/05 burumal
// ������ �ʴ� �Լ��� �Ǵ� ������
// bool	AuIniManagerA::AddPartIndices(int lStartIndex, int lEndIndex, int lOffset)
// {
// 	int lOffsetAll = 0;
// 	int lIndex;
// 	int * plNewAlloc = NULL;
// 
// 	if ( lEndIndex >= m_lPartNum )
// 	{
// 		/*
// 		plNewAlloc = new int[lEndIndex + 1];
// 
// 		if ( !plNewAlloc )
// 		{
// 			return false;
// 		}
// 
// 		memset(plNewAlloc, 0, sizeof(int) * (lEndIndex + 1));
// 
// 		if ( m_plPartIndices )
// 		{
// 			memcpy(plNewAlloc, m_plPartIndices, sizeof(int) * m_lPartNum);
// 			delete [] m_plPartIndices;
// 		}
// 
// 		m_plPartIndices = plNewAlloc;
// 
// 		m_lPartNum = lEndIndex + 1;
// 
// 		lOffsetAll = GetStartIndex() - m_plPartIndices[0];
// 
// 		for ( lIndex = 0; lIndex < m_lPartNum; lIndex++ )
// 		{
// 			m_plPartIndices[lIndex] += lOffsetAll;
// 		}
// 		*/
// 		return false;
// 	}
// 	else
// 	{		
// 		for ( lIndex = lStartIndex ; lIndex <= lEndIndex ; ++lIndex )
// 		{
// 			m_plPartIndices[lIndex] += lOffsetAll + lOffset;
// 		}
// 	}
// 
// 	return true;
// }

int	AuIniManagerA::ReadPartIndices(char* pBuffer)
{
	size_t iOffsetBytes = 0;
	size_t iOffset = 0;
	int i;

	//@{ 2006/08/31 burumal
	//char szDummy[400] = {0,};
	char szDummy[400];
	//@}

	iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
	
	//@{ 2006/07/28 burumal
	//iOffset += strlen(CRLF);
	iOffset += g_nCRLELength;
	//@}

	memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

	//@{ 2006/08/31 burumal
	if ( iOffset < 400 )
	{
		szDummy[iOffset] = '\0';
		if( iOffset > 0 && szDummy[iOffset - 1] == 13 )
			szDummy[iOffset - 1] = 0;
			
	}
	//@}

	iOffsetBytes += iOffset;

	i = (int)strlen(szDummy) - 1;
	while( i > -1 && ( szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' || szDummy[i] == 13 ) )
	{
		szDummy[i] = '\0';
		--i;
	}

	m_lPartNum = atoi(szDummy);
	
	if ( m_plPartIndices )
		delete [] m_plPartIndices;
	
	m_plPartIndices = new int[m_lPartNum];
	if ( !m_plPartIndices )
		return -1;

	for ( int iLoop = 0 ; iLoop < m_lPartNum ; ++iLoop )
	{
		iOffset = strcspn(pBuffer + iOffsetBytes, CRLF);
		
		//@{ 2006/07/28 burumal
		//iOffset += strlen(CRLF);
		iOffset += g_nCRLELength;
		//@}

		memcpy(szDummy, pBuffer + iOffsetBytes, iOffset);

		i = (int)strlen(szDummy) - 1;

		szDummy[i+1] = 0;

		while( i > -1 && ( szDummy[i] == '\n' || szDummy[i] == '\t' || szDummy[i] == '\r' || szDummy[i] == ' ' ||szDummy[i] == 13 ) )
		{
			szDummy[i] = '\0';
			--i;
		}

		*(m_plPartIndices + iLoop) = atoi(szDummy);
//		if ( !(iOffset = sscanf(pBuffer	+ iOffsetBytes, "%10d\n", m_plPartIndices + i)) )
//			return -1;

		iOffsetBytes += iOffset;
	}

	return (int)iOffsetBytes;
}

bool	AuIniManagerA::ReadPartIndices(FILE *pFile)
{
	if ( !fscanf(pFile, "%10d\n", &m_lPartNum) )
		return false;
	
	if ( m_plPartIndices )
		delete [] m_plPartIndices;	

	m_plPartIndices = new int[m_lPartNum];
	if ( !m_plPartIndices )
		return false;

	for ( int i = 0 ; i < m_lPartNum ; ++i )
	{
		if ( !fscanf(pFile, "%10d\n", m_plPartIndices + i ) )
			return false;
	}

	return true;
}

bool	AuIniManagerA::WritePartIndices(FILE *pFile)
{
	fseek(pFile, 0, SEEK_SET);

	fprintf(pFile, "%10d\n", m_lPartNum);

	for ( int i = 0 ; i < m_lPartNum ; ++i )
	{
		fprintf(pFile, "%10d\n", m_plPartIndices[i]);
	}

	return true;
}

int		AuIniManagerA::GetStartIndex()
{
	return (12 * (m_lPartNum + 1));
}

bool	AuIniManagerA::EncryptSave()
{
	FILE *pReadFile;

	pReadFile = fopen(m_strPathName.c_str(), "rb");

	if (NULL == pReadFile) return false;

	// ���� ������ �˾ƿ���
	fseek(pReadFile, 0, SEEK_END);
	
    int lFileSize = ftell(pReadFile);
	fseek(pReadFile, 0, SEEK_SET);

	// ������ �о����
	char* pBuffer = new char[lFileSize];
	memset(pBuffer, 0, lFileSize);
	int lReadBytes = (int)fread(pBuffer, sizeof(BYTE), lFileSize, pReadFile);
	fclose(pReadFile);

	AuMD5Encrypt MD5;
#ifdef _AREA_CHINA_
	MD5.EncryptString(MD5_HASH_KEY_STRING, pBuffer, lFileSize);
#else
	MD5.EncryptString(HASH_KEY_STRING, pBuffer, lFileSize);
#endif
	
	FILE *pWriteFile = fopen(m_strPathName.c_str(), "wb");
	if (NULL == pWriteFile) return false;

	int WriteBytes = (int)fwrite(pBuffer, sizeof(char), lFileSize, pWriteFile);

	fclose(pWriteFile);

	if (WriteBytes == lFileSize) return true;
	else return false;
}