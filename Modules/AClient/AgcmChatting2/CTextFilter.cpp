#include <AuExcelTxtLib/AuExcelTxtLib.h>
#include <ApModule/ApModuleStream.h>
#include "CTextFilter.h"




CTextFilter::CTextFilter( void )
{
	m_strBlindText[ 0 ] = '*';
	m_strBlindText[ 1 ] = '\0';
}

CTextFilter::~CTextFilter( void )
{
}

BOOL CTextFilter::OnLoadFilterFile( char* pFileName, BOOL bIsEncrypt )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

#ifndef _BIN_EXEC_
	if( !bIsEncrypt ) return _LoadCustomFilterFile( pFileName );
#endif

	AuExcelTxtLib ExcelFile;
	if( !ExcelFile.OpenExcelFile( pFileName, TRUE, bIsEncrypt ) )
	{
		ExcelFile.CloseFile();

#ifdef _DEBUG
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "-- Error! -- : Failed to load Text Filter File, FileName = %s\n", pFileName );
		OutputDebugString( strDebug );
#endif
		return FALSE;
	}

	int nRowCount = ExcelFile.GetRow();
	for( int nCount = 0 ; nCount < nRowCount ; nCount++ )
	{
		char* pFilterText = ExcelFile.GetData( 0, nCount );
		if( pFilterText && strlen( pFilterText ) > 0 )
		{
			_strlwr( pFilterText );
			OnAddFilter( pFilterText, FALSE );
		}
	}

	ExcelFile.CloseFile();
	return TRUE;
}

BOOL CTextFilter::OnAddFilter( char* pFilterText, BOOL bIsCustom, BOOL bIsSave )
{
	if( !pFilterText || strlen( pFilterText ) <= 0 ) return FALSE;

	stTextFilterEntry* pEntry = bIsCustom ? m_mapFilterCustom.Get( pFilterText ) : m_mapFilter.Get( pFilterText );
	if( !pEntry )
	{
		stTextFilterEntry NewFilter;

		strcpy_s( NewFilter.m_strFilter, sizeof( char ) * 256, pFilterText );
		NewFilter.m_nTextLegnth = strlen( NewFilter.m_strFilter );

		bIsCustom ? m_mapFilterCustom.Add( pFilterText, NewFilter ) : m_mapFilter.Add( pFilterText, NewFilter );
		if( bIsCustom && bIsSave )
		{
			OnSaveFilterFileCustom( "INI\\CustomTextFilter.txt", pFilterText );
		}
	}

	return TRUE;
}

BOOL CTextFilter::OnRemoveFilter( char* pFilterText, BOOL bIsCustom )
{
	if( !pFilterText || strlen( pFilterText ) <= 0 ) return FALSE;

	bIsCustom ? m_mapFilterCustom.Delete( pFilterText ) : m_mapFilter.Delete( pFilterText );
	return TRUE;
}

BOOL CTextFilter::OnClearFilter( BOOL bIsCustom )
{
	bIsCustom ? m_mapFilterCustom.Clear() : m_mapFilter.Clear();
	return TRUE;
}

BOOL CTextFilter::OnSaveFilterFileCustom( char* pFileName, char* pFilter )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	if( !pFilter || strlen( pFilter ) <= 0 ) return FALSE;

	FILE* pFile = fopen( pFileName, "a+" );
	if( !pFile )
	{
		// ������ ������ ���� �����.
		pFile = fopen( pFileName, "wt" );
		if( !pFile ) return FALSE;

		fclose( pFile );
		pFile = fopen( pFileName, "a+" );
		if( !pFile ) return FALSE;
	}

	fseek( pFile, 0, SEEK_END );
	fwrite( pFilter, strlen( pFilter ), 1, pFile );
	fwrite( "\n", 1, 1, pFile );
	fclose( pFile );

	return TRUE;
}

BOOL CTextFilter::OnFiltering( char* pString )
{
	if( !pString || strlen( pString ) <= 0 ) return FALSE;

	// ù���ڰ� '/' �� �����ϴ� ��ɹ��� ��� �� �ޱ��ں��� �˻��Ѵ�.
	char strBuffer[ 256 ] = { 0, };
	bool bIsInstruction = false;
	int i = 0;

	if(pString[ 0 ] == '/')
		bIsInstruction = true;

	strcpy_s( strBuffer, sizeof( char ) * 256, bIsInstruction ? pString + 1 : pString );

	// �ӼӸ��̸� �ɸ��� �� �� ��Ģ��� ����Ѵ�.
	if(strBuffer[0] == 'w' || strBuffer[0] == 'W')
	{
		char strTemp[ 256 ] = { 0, };
		int	nChecker(0);
		strcpy_s( strTemp, sizeof( char ) * 256, strBuffer );

		while(nChecker<2)
		{
			if(strTemp[i++] == ' ')
				++nChecker;
		}

		memset(strBuffer, '\0', sizeof( char ) * 256);
		strcpy_s(strBuffer, sizeof( char ) * 256, strTemp + i);
	}

	_strlwr( strBuffer );
	int nBufferLength = strlen( strBuffer );

	int nFilterCount = m_mapFilter.GetSize();
	for( int nCount = 0 ; nCount < nFilterCount ; nCount++ )
	{
		stTextFilterEntry* pFilter = m_mapFilter.GetByIndex( nCount );
		if( pFilter && strlen( pFilter->m_strFilter ) > 0 )
		{
			for( int nBufferCount = 0 ; nBufferCount < nBufferLength ; nBufferCount++ )
			{
				if( !strncmp( pFilter->m_strFilter, &strBuffer[ nBufferCount ], pFilter->m_nTextLegnth ) )
				{
					for( int nBlindCount = 0 ; nBlindCount < pFilter->m_nTextLegnth ; nBlindCount++ )
					{
						pString[ nBufferCount + nBlindCount + i + (bIsInstruction?1:0) ] = m_strBlindText[ 0 ];
					}

					nBufferCount += pFilter->m_nTextLegnth;
				}
				else if( strBuffer[nBufferCount] < 0 ) // 2����Ʈ ���ڴ� 2ĭ�� �̵�
					++nBufferCount;
			}
		}
	}

	int nFilterCustomCount = m_mapFilterCustom.GetSize();
	for( int nCount = 0 ; nCount < nFilterCustomCount ; nCount++ )
	{
		stTextFilterEntry* pFilter = m_mapFilterCustom.GetByIndex( nCount );
		if( pFilter && strlen( pFilter->m_strFilter ) > 0 )
		{
			for( int nBufferCount = 0 ; nBufferCount < nBufferLength ; nBufferCount++ )
			{
				if( !strncmp( pFilter->m_strFilter, &strBuffer[ nBufferCount ], pFilter->m_nTextLegnth ) )
				{
					for( int nBlindCount = 0 ; nBlindCount < pFilter->m_nTextLegnth ; nBlindCount++ )
					{
						pString[ nBufferCount + nBlindCount + i + (bIsInstruction?1:0) ] = m_strBlindText[ 0 ];
					}

					nBufferCount += pFilter->m_nTextLegnth;
				}
				else if( strBuffer[nBufferCount] < 0 ) // 2����Ʈ ���ڴ� 2ĭ�� �̵�
					++nBufferCount;
			}
		}
	}

	return TRUE;
}

BOOL CTextFilter::_LoadCustomFilterFile( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;

	FILE* pFile = fopen( pFileName, "rt" );
	if( !pFile ) return FALSE;

	fseek( pFile, 0, SEEK_END );
	int nFileSize = ftell( pFile );
	if( nFileSize <= 0 )
	{
		fclose( pFile );
		return FALSE;
	}

	char* pBuffer = new char[ nFileSize ];
	memset( pBuffer, 0, sizeof( char ) * nFileSize );

	fseek( pFile, 0, SEEK_SET );
	fread( pBuffer, nFileSize, 1, pFile );
	fclose( pFile );

	int nIndex = 0;
	while( pBuffer[ nIndex ] != '\0' )
	{
		char strBuffer[ 256 ] = { 0, };
		int nCopyCount = 0;

		while( pBuffer[ nIndex ] != '\n' && pBuffer[ nIndex ] != '\0' )
		{
			strBuffer[ nCopyCount ] = pBuffer[ nIndex ];

			nCopyCount++;
			nIndex++;
		}

		OnAddFilter( strBuffer, TRUE );
		if( pBuffer[ nIndex ] != '\0' )
		{
			nIndex++;
		}
	}

	delete[] pBuffer;
	pBuffer = NULL;

	return TRUE;
}
