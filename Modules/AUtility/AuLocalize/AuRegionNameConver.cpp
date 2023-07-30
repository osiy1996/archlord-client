#include <AuLocalize/AuRegionNameConvert.h>
#include <AuEtcUtil/MemMappedFile.h>
#include <AuEtcUtil/StrUtil.h>

#ifndef _AREA_CHINA_
#include <AuStringConv/AuStringConv.h>
#include <AuZpackLoader/AuZpackLoader.h>
#include <AuStaticPool/static_pool.h>
#include <sstream>

// -========================================================================-

AuRegionNameConvert& RegionLocalName()
{
	static AuRegionNameConvert rnc;
	return rnc;
}

// -========================================================================-

AuRegionNameConvert::AuRegionNameConvert()
{
	Load( "ini/RegionGlossary.txt",
#ifdef _BIN_EXEC_
		FALSE
#else
		TRUE
#endif
	);
}

AuRegionNameConvert::~AuRegionNameConvert()
{
}

void AuRegionNameConvert::parse_data( char * ptr )
{
	unsigned offset = 0;
	char line[4096] = {0,};

	while ( offset = StrUtil::GetOneLine( line, ptr ) )
	{
		StrUtil::RTrim( line );

		size_t pos = strcspn(line, "\t");
		umtl::static_pool::string key(line, line + pos);
		umtl::static_pool::string value(line + pos + 1);

		m_map[key.c_str()] = value.c_str();

		ptr += offset;
	}
}

void AuRegionNameConvert::parse_data( wchar_t * ptr )
{
	std::wstringstream stream( ptr );

	wchar_t line_buffer[4096] = {0,};
	char key[4096] = {0,};
	char value[4096] = {0,};
	int length = 0;

	while( stream.getline( line_buffer, 4096 ) )
	{
		StrUtil::RTrim( line_buffer );

		size_t pos = wcscspn( line_buffer, L"\t" );

		umtl::static_pool::wstring wKey( line_buffer, line_buffer + pos );
		umtl::static_pool::wstring wValue( line_buffer+pos+1 );

		if( !wKey.empty() && !wValue.empty() )
		{
			//ToMBCS( (wchar_t*)wKey.c_str(), key, sizeof(key) );
			length = WideCharToMultiByte( 949, 0, wKey.c_str(), (int)wKey.length(), key, sizeof(key), 0, 0 );
			key[ length ] = 0;
			length = WideCharToMultiByte( GetLangID(), 0, wValue.c_str(), (int)wValue.length(), value, sizeof(value), 0, 0 );
			value[ length ] = 0;
			//ToMBCS( (wchar_t*)wValue.c_str(), value, sizeof(value) );

			m_map[key] = value;
		}
	}
}

static bool IsUnicode( char const * ptr )
{
	bool result = false;

	if( strlen(ptr) > 2 )
	{
		unsigned short int checkByte = *(unsigned short int*)ptr;

		if( checkByte == 0xfeff )
			result = true;
	}

	return result;
}

bool AuRegionNameConvert::Load( char* filename, bool encrypt )
{
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
	if( encrypt )
	{
		ptr = GetDecryptStr( (char*)start, (DWORD)size);

		if( isZpackStream )
		{
			ReleaseZpackStream( packName, (char*)start );
			isZpackStream = false;
		}

		if(!ptr)
			return false;
	}

	if( IsUnicode(ptr) )
		parse_data( reinterpret_cast<wchar_t*>(ptr) );
	else
		parse_data( ptr );
	

	if( isZpackStream )
		ReleaseZpackStream( packName, ptr );
	else if ( encrypt )
		delete [] ptr;

	return true;
}

std::string AuRegionNameConvert::GetStr( char* key )
{
	static std::string dummy = "";

	StrMap::iterator iter = m_map.find( key );

	return (iter != m_map.end()) ? iter->second : dummy;
}

char* AuRegionNameConvert::GetDecryptStr( char* str, unsigned size )
{
	char* temp = new char [ size + 1];
	memcpy( temp, str, size );
	temp[ size ] = 0;

	StrUtil::Decrypt( temp, size );

	return temp;
}

#else
// -========================================================================-

AuRegionNameConvert& RegionLocalName()
{
	static AuRegionNameConvert rnc;
	return rnc;
}

// -========================================================================-

AuRegionNameConvert::AuRegionNameConvert()
{
	Load( "ini/RegionGlossary.txt", TRUE );
}

AuRegionNameConvert::~AuRegionNameConvert()
{
}

bool AuRegionNameConvert::Load( char* filename, bool encrypt )
{
	// open file
	MemMappedFile mmf;
	if ( !mmf.OpenForReadOnly( filename ) )
		return false;

	//
	const char* start = encrypt
					  ? GetDecryptStr( (char*)mmf.GetStartPtr(), mmf.GetFileSize())
					  : (char*)mmf.GetStartPtr();

	const char* ptr = start;

	char*	 line = new char [4096];
	unsigned offset = 0;
	bool	 result = true;

	while ( offset = StrUtil::GetOneLine( line, const_cast<char*>(ptr) ) )
	{
		StrUtil::RTrim( line );

		size_t pos = strcspn(line, "\t");
		std::string key(line, line + pos);
		std::string value(line + pos + 1);

		m_map[key] = value;

		ptr += offset;
	}

	if ( encrypt )
		delete [] start;
	
	delete [] line;

	return result;
}

std::string AuRegionNameConvert::GetStr( char* key )
{
	static std::string dummy = "";

	StrMap::iterator iter = m_map.find( key );

	return (iter != m_map.end()) ? iter->second : dummy;
}

char* AuRegionNameConvert::GetDecryptStr( char* str, unsigned size )
{
	char* temp = new char [ size + 1];
	memcpy( temp, str, size );
	temp[ size ] = 0;

	StrUtil::Decrypt( temp, size );

	return temp;
}

#endif
