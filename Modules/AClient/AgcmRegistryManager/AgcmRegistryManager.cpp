#include <AgcmRegistryManager/AgcmRegistryManager.h>

//-----------------------------------------------------------------------
//

AgcmRegistryManager::AgcmRegistryManager()
	: handle_(0)
{
	// key setting
#ifdef _AREA_TAIWAN_
	HKEY key = HKEY_CURRENT_USER;
#else
	HKEY key = HKEY_LOCAL_MACHINE;
#endif

	// sub key setting
#ifdef _AREA_GLOBAL_
	const char * subKey = "SOFTWARE\\Webzen\\ArchLord";
#elif _AREA_TAIWAN_
	const char * subKey = "SOFTWARE\\Webzen\\ArchLord_TW";
#else
	const char * subKey = "SOFTWARE\\ArchLord";
#endif

	// open
	if( ERROR_SUCCESS != RegOpenKeyEx( key, subKey, 0, KEY_READ, &handle_ ) )
	{
		std::string message = "FAILED - AgcmRegistryManager::RegOpenKey, ";
		message += (char const*)subKey;
		OutputDebugString( message.c_str() );

		handle_ = 0;
	}
}

//-----------------------------------------------------------------------
//

AgcmRegistryManager::~AgcmRegistryManager()
{
	if( handle_ )
		RegCloseKey(handle_);
}

//-----------------------------------------------------------------------
//

namespace AgcmRegistryManagerUtill
{
	void set_value( std::string & v, char * tmp, DWORD len )
	{
		v = tmp;
	}

	void set_value( std::wstring & v, char * tmp, DWORD len )
	{
		wchar_t buf[1024];
		size_t convertedSize = 0;

		mbstowcs_s( &convertedSize, buf, sizeof(buf), tmp, len );

		v = buf;
	}
}

//-----------------------------------------------------------------------