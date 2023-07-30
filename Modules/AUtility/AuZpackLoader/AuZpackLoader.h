#pragma once
#include <ApBase/ApBase.h>
#include <string>
#include <AuStaticPool/static_pool.h>

//////////////////////////////////////////////////////////////////////////////
//

class AuZpackLoader
{
public:
	AuZpackLoader();
	~AuZpackLoader();

	static AuZpackLoader & Instance();

	void SetPassword( wchar_t const * password );
	umtl::static_pool::wstring GetPassword();

private:
	HMODULE hDll_;
	umtl::static_pool::wstring password_;

	typedef unsigned char*	(__cdecl* Find)				( wchar_t * pack_file, wchar_t * file_name, unsigned int & outSizeProcessed, wchar_t * password );
	typedef void			(__cdecl* Release)			( wchar_t * pack_file, wchar_t * buffer );
	typedef void			(__cdecl* SetAllocator)		( wchar_t * pack_file, void*(*allocFunc)(void*,unsigned int), void(*freeFunc)(void*,void*) );
	typedef void			(__cdecl* SetTmpAllocator)	( wchar_t * pack_file, void*(*allocFunc)(void*,unsigned int), void(*freeFunc)(void*,void*) );
	typedef wchar_t const * (__cdecl* FolderInfo)		( wchar_t * pack_file, wchar_t * path, wchar_t const * password );
	typedef void			(__cdecl* ClearCache)		( wchar_t * pack_file );

	typedef bool			(__cdecl* Del)				( wchar_t * pack_file, wchar_t * file_name, wchar_t * password );
	typedef bool			(__cdecl* Add)				( wchar_t * pack_file, wchar_t * file_name, int level, wchar_t * password );
	typedef bool			(__cdecl* Extract)			( wchar_t * pack_file, wchar_t * file_name, wchar_t * outDir, bool fullPath, wchar_t * password );

public:
	Find					find_;
	Release					release_;
	SetAllocator			setAllocator_;
	SetTmpAllocator			setTmpAllocator_;
	FolderInfo				folderInfo_;
	ClearCache				clearCache_;
	Del						del_;
	Add						add_;
	Extract					extract_;
};

//////////////////////////////////////////////////////////////////////////////
//

BYTE *			GetZpackStream		( wchar_t * zpackFilename, const wchar_t * file, size_t & size );
BYTE *			GetZpackStream		( wchar_t * zpackFileName, const char * file, size_t & size );
void			ReleaseZpackStream	( wchar_t * zpackFileName, void * stream );
void			ClearZpackCache		( std::wstring zpackFileNane );
void			ReleaseZpackPool	();
wchar_t const * GetZpackFolderInfo	( wchar_t * zpackFileName, const char * folderPath );
void			ClearZpackCacheAll	();

//////////////////////////////////////////////////////////////////////////////