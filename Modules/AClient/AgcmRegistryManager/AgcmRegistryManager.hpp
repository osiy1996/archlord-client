#pragma once
#include <string>

//-----------------------------------------------------------------------
//

namespace AgcmRegistryManagerUtill
{
	template< typename Value >
	void set_value( Value & v, char * tmp, DWORD len )
	{
		memcpy_s( &v, sizeof(v), tmp, len );
	}

	void set_value( std::string & v, char * tmp, DWORD len );
	void set_value( std::wstring & v, char * tmp, DWORD len );
}

//-----------------------------------------------------------------------
//

template< typename Value >
Value AgcmRegistryManager::GetProperty( char const * key )
{
	AgcmRegistryManager & manager = inst();

	Value result;

	if( manager.handle_ && key )
	{
		char tmp[1024] = {0,};
		DWORD length = sizeof(tmp);
		DWORD type;

		if( ERROR_SUCCESS != RegQueryValueEx(manager.handle_, key, 0, &type, (LPBYTE)tmp, &length ) )
		{
			std::string message = "FAILED - AgcmRegistryManager::GetProperty, key = ";
			message += key;
			OutputDebugString( message.c_str() );
		}
		else
		{
			AgcmRegistryManagerUtill::set_value( result, tmp, length );
		}
	}

	return result;
}

//-----------------------------------------------------------------------