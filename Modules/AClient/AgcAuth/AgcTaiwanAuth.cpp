#include "AgcTaiwanAuth.h"
#include <AuIgnoringHelper/AuIgnoringHelper.h>
#include <AgcmRegistryManager/AgcmRegistryManager.h>
#include <stdio.h>
#include <iostream>

//-----------------------------------------------------------------------
//

AgcTaiwanAuth::~AgcTaiwanAuth()
{

}

//-----------------------------------------------------------------------
//

bool AgcTaiwanAuth::init( std::string & cmdline )
{
	setProperty( "AutoLogin", false );

	if( !AuIgnoringHelper::IgnoreToFile( "archlordTest.ini" ) )
	{
		size_t start = cmdline.find( " " ) + 1;

		std::string authString = cmdline.substr( start, std::string::npos );

		char patchCode[128] = {0,};

		DWORD code = AgcmRegistryManager::GetProperty<DWORD>( "Code" );

		sprintf_s( patchCode, sizeof(patchCode), "%u", code );

		return authString == patchCode;
	}

	return true;
}

//-----------------------------------------------------------------------
//

void AgcTaiwanAuth::error_handling()
{
	char* szString = "很抱歉,只?透過遊戲主?式??啟動《帝王戰記》,請關閉應用?式後再試?次。";
	if( MessageBox( NULL, szString, "Archlord", MB_OK ) )
		::ShellExecute( NULL, "open", "http://al.wpark.com.tw", NULL, NULL, SW_SHOW );
}

//-----------------------------------------------------------------------
//

void AgcTaiwanAuth::final_process()
{

}

//-----------------------------------------------------------------------
//

bool AgcTaiwanAuth::encrypt( char * code, char * account, size_t account_size, char * password, size_t password_size )
{
	if( !code || !account || !password || password_size < 32 )
		return false;

	std::string hashStr = md5Encrypt_.GetMD5Hash( password );

	strcpy_s( password, password_size, hashStr.c_str() );

	if( hashStr.empty() )
		return false;

	if( !md5Encrypt_.EncryptString( code, account, strlen(account) ) )
		return false;

	if( !md5Encrypt_.EncryptString( code, password, strlen(password) ) )
		return false;

	return true;
}

//-----------------------------------------------------------------------