#include "AuXmlParser.h"
#include <AuMD5EnCrypt/AuMD5Encrypt.h>
#include <fstream>
#include <AuPackingManager/AuPackingManager.h>

#define XML_MAX_FILESIZE		1024*1024*5

#define MD5_HASHKEY				"1111"


ApMemoryPool	AuXmlParser::m_MemoryPool;

AuXmlParser::AuXmlParser( void )
{

}

AuXmlParser::AuXmlParser( const string& strFileName )
{
	LoadXMLFile( strFileName );
}

AuXmlParser::AuXmlParser( char* pBuffer , int nSize )
{
	LoadXMLMemory( pBuffer , nSize );
}

AuXmlParser::~AuXmlParser( void )
{
	Clear();
}

bool	AuXmlParser::LoadXMLFile( CONST string& strFileName )
{
	static bool	bMemoryInit	=	FALSE;
	if( !bMemoryInit )
	{
		m_MemoryPool.Initialize( XML_MAX_FILESIZE , 1 );
		bMemoryInit	=	TRUE;
	}

	if( !strFileName.length() )
		return false;

	ifstream		InFileStream;
	int				nSize		=	0;
	char*			pFileBuffer	=	NULL;

	// 1 - ������ �о �޸𸮷� �ҷ��´�
	InFileStream.open( strFileName.c_str() , ios_base::in | ios_base::binary );
	if( !InFileStream )
	{
		CHAR	szPath[ MAX_PATH ];
		sprintf_s( szPath, MAX_PATH, "%s", strFileName.c_str());

		AuPackingManager* pPackingManager = AuPackingManager::GetSingletonPtr();
		if( !pPackingManager ) return 0;

		ApdFile pdFile;
		if( !pPackingManager->OpenFile( szPath, &pdFile ) )
		{
			return 0;
		}

		nSize = pPackingManager->GetFileSize( &pdFile );
		if( nSize <= 0 )
		{
			pPackingManager->CloseFile( &pdFile );
			return 0;
		}

		pFileBuffer = new char[ nSize ];
		int nReadSize = pPackingManager->ReadFile( pFileBuffer, nSize, &pdFile );
		pPackingManager->CloseFile( &pdFile );

		if( nReadSize != nSize )
		{
			delete []pFileBuffer;
			pFileBuffer = NULL;
			return 0;
		}
	}
	else
	{
		InFileStream.seekg( 0 , ios_base::end );
		nSize		=	(int)InFileStream.tellg();
		pFileBuffer	=	(char*)m_MemoryPool.Alloc();
		ZeroMemory( pFileBuffer , nSize );

		InFileStream.seekg( 0 , ios_base::beg );
		InFileStream.read( pFileBuffer , nSize );
		InFileStream.close();
	}

	bool	bResult	=	LoadXMLMemory( pFileBuffer , nSize );

	m_MemoryPool.Free( pFileBuffer );

	return bResult;
}

bool	AuXmlParser::LoadXMLMemory( char* pBuffer , int nSize )
{
	AuMD5Encrypt	MD5;

	if( !LoadMemory( pBuffer , nSize , TIXML_ENCODING_LEGACY ) )
	{
		// 2 - �޸��� ������ ��ȣȭ �Ѵ�
#ifndef _AREA_CHINA_
		if( !MD5.DecryptString( MD5_HASHKEY , pBuffer, nSize ) )
#else
		if( !MD5.DecryptString( MD5_HASH_KEY_STRING , pBuffer, nSize ) )
#endif
		{
			return false;
		}

		// ��ȣȭ ��Ų ���¿��� �ٽ� �д´�
		if( !LoadMemory( pBuffer , nSize , TIXML_ENCODING_LEGACY ) )
		{
			return false;
		}
	}

	return true;
}