
#include <AuStrTable/AuStrTable.h>
#include <ApBase/MagDebug.h>
#include <AuEtcUtil/MemMappedFile.h>
#include <AuEtcUtil/StrUtil.h>

#ifndef _AREA_CHINA_
#include <AuZpackLoader/AuZpackLoader.h>

//-------------------------------- Global -------------------------------------
#ifdef _BIN_EXEC_
bool	g_ClientStrEncrypt = false;
#else
bool	g_ClientStrEncrypt = true;
#endif
#else
bool	g_ClientStrEncrypt = true;
#endif // #ifndef _AREA_CHINA_
char*	g_INIFileName = "ini\\sysstr.txt";

AuStrTable& ClientStr()
{
	static AuStrTable st(g_ClientStrEncrypt);
	return st;
}

AuStrTable& ServerStr()
{
#ifdef _AREA_CHINA_
	static AuStrTable st(true);
#else
	static AuStrTable st(false);
#endif
	return st;
}

//-------------------------------- AuStrTable -------------------------------------
AuStrTable::AuStrTable(bool encrypt) :
 m_szStringTable(NULL),
 m_tableSize(0),
 m_maxIndex( 0 ),
 m_isLoad( false ),
 m_encrypt( encrypt )
{
	Load( g_INIFileName );
}

AuStrTable::~AuStrTable()
{
	for ( unsigned i = 0; i < m_tableSize; ++i )
	{
		if( m_szStringTable[i] )
		{
			delete [] m_szStringTable[i];
			m_szStringTable[i] = NULL;
		}
	}

	if( m_szStringTable )
	{
		delete [] m_szStringTable;
		m_szStringTable = NULL;
	}
}

bool AuStrTable::Load( char* filename, unsigned tableSize )
{
#ifndef _AREA_CHINA_

	bool isZpackStream = true;
	MemMappedFile mmf;
	size_t size;

	wchar_t * packName = L"ini.zp";
	char * start = (char *)GetZpackStream( packName, filename, size );
	char * ptr = 0;

	if( !start )
	{
		isZpackStream = false;

		// open file
		if ( !mmf.OpenForReadOnly( filename ) )
			return false;

		start = (char *)mmf.GetStartPtr();
		size = (size_t)mmf.GetFileSize();
	}

	ptr = start;

	//
	if( m_encrypt )
	{
		ptr = GetDecryptStr( (char*)start, (DWORD)size);

		if( isZpackStream )
		{
			ReleaseZpackStream( packName, (char*)start );
			isZpackStream = false;
		}

		if(!ptr)
			return false;

		start = ptr;
	}


	// create string table
	m_tableSize		= tableSize;
	m_szStringTable = new char* [m_tableSize];
	ZeroMemory( m_szStringTable , sizeof(char*)*tableSize );

	INT		AccuOffset	= 0;
		
	bool	 result = true;

	char szLine[4096];
	ZeroMemory( szLine , 4096 );

	unsigned offset;
	while( ( offset = StrUtil::GetOneLine( szLine , ptr ) ) && AccuOffset < (INT)size )
	{
		StrUtil::RTrim( (char*)szLine );

		INT nIndex = ReadIndex( szLine );
		if( nIndex	<	0 )	break;

		ASSERT( (UINT)nIndex < m_tableSize && "SysStr.txt MaxTable»çÀÌÁî¸¦ ´Ã·ÁÁà¾ßÇÑ´Ù" );

		m_maxIndex = max( (UINT)nIndex, m_maxIndex );

		m_szStringTable[nIndex] = ReadStr( szLine );

		ptr += offset;
		ZeroMemory( szLine , 4096 );

		AccuOffset		+=	offset;
	}

	if( isZpackStream )
		ReleaseZpackStream( packName, start );
	else if ( m_encrypt )
		delete [] start;

	m_isLoad	= true;	

	return result;
#else
	// open file
	MemMappedFile mmf;
	if ( !mmf.OpenForReadOnly( filename ) )		return false;

	// create string table
	m_tableSize		= tableSize;
	m_szStringTable = new char* [m_tableSize];
	ZeroMemory( m_szStringTable , sizeof(char*)*tableSize );

	char*	start		= m_encrypt ? GetDecryptStr( (char*)mmf.GetStartPtr(), mmf.GetFileSize() ) : (char*)mmf.GetStartPtr();
	char*	ptr			= start;
	INT		TotalSize	= mmf.GetFileSize();
	INT		AccuOffset	= 0;
		
	bool	 result = true;

	char szLine[4096];
	ZeroMemory( szLine , 4096 );

	unsigned offset;
	while( ( offset = StrUtil::GetOneLine( szLine , ptr ) ) && AccuOffset < TotalSize )
	{
		StrUtil::RTrim( (char*)szLine );

		INT nIndex = ReadIndex( szLine );
		if( nIndex	<	0 )	break;

		ASSERT( nIndex < m_tableSize && "SysStr.txt MaxTable»çÀÌÁî¸¦ ´Ã·ÁÁà¾ßÇÑ´Ù" );

		m_maxIndex = max( nIndex, m_maxIndex );

		m_szStringTable[nIndex] = ReadStr( szLine );

		ptr += offset;
		ZeroMemory( szLine , 4096 );

		AccuOffset		+=	offset;
	}

	if ( m_encrypt )		delete [] start;
	m_isLoad	= true;

	

	return result;
#endif
}

char* AuStrTable::GetStr( unsigned index )
{
	static char* szNull = "";
	return index > m_maxIndex ? szNull : m_szStringTable[index];
}

int AuStrTable::ReadIndex( char* line )
{
	char pNumber[8];
	size_t pos = strcspn( line, "=" );
	
	if ( !pos || pos >= strlen( line ) )
		return -1;

	// index
	memcpy( pNumber, line, pos );
	pNumber[pos] = 0;

	return atoi( pNumber );
}

char* AuStrTable::ReadStr( char* line )
{
	if(	!line )					return NULL;
	if( strlen( line ) < 3 )	return NULL;

	size_t	pos		= strcspn( line, "=" ) + 1;
	size_t	strLen	= strlen( line ) - pos;
	char*	str		= new char [ strLen+1 ];

	memcpy( str, line + pos, strLen );
	str[strLen] = 0;

	return str;
}

char* AuStrTable::GetDecryptStr( char* str, unsigned size )
{
	char* temp = new char [ size+1 ];
	memcpy( temp, str, size );
	temp[ size ] = 0;

	StrUtil::Decrypt( temp, size );

	return temp;
}
