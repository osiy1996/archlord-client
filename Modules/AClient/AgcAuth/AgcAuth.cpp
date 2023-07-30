
#define _WINSOCKAPI_
#include <AgcAuth/AgcAuth.h>
#include <algorithm>
#include <AgcmLogin/AgcmLogin.h>
#include <AgcmUILogin/AgcmUILogin.h>
#include <AuIgnoringHelper/AuIgnoringHelper.h>
#include <AgcModule/AgcEngine.h>
#include <AgcmUIManager2/AgcmUIManager2.h>

//-----------------------------------------------------------------------
//

bool AgcAuth::init( char const * cmdline ) 
{
	if( isTest() )
		return true;

	return init( cmdline ? std::string( cmdline ) : "" );
};

//-----------------------------------------------------------------------
//

bool AgcAuth::init( std::string & cmdline )
{
	setProperty( "gamestring", cmdline.substr( cmdline.rfind( " " ) + 1, std::string::npos ) );
	//setProperty( "AutoLogin", true );
	//setProperty( "member_id", cmdline.find(":"),  )

	return true;
}

//-----------------------------------------------------------------------
//

void AgcAuth::error_handling()
{
	char* szString = "The game client is loaded only through the official website.\n closing the application please try again";
	if( MessageBox( NULL, szString, "Archlord", MB_OK ) )
		::ShellExecute( NULL, "open", "http://archlord.webzen.com", NULL, NULL, SW_SHOW );
}

//-----------------------------------------------------------------------
//

void AgcAuth::final_process()
{

}

//-----------------------------------------------------------------------
//

bool AgcAuth::isTest()
{
	/*
	DWORD dwAttr = GetFileAttributes( "NoExcuteAuth.arc" );
	return dwAttr != INVALID_FILE_ATTRIBUTES && ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0;
	*/

	return AuIgnoringHelper::IgnoreToFile( "NoExcuteAuth.arc" );
}

//-----------------------------------------------------------------------
//

void AgcAuth::setProperty_impl( std::string key, int value )
{
	char buf[256];
	_itoa_s( value, buf, sizeof(buf), 10 );
	propertyMap_[key] = buf;
}

//-----------------------------------------------------------------------
//

void AgcAuth::setProperty_impl( std::string key, bool value )
{
	propertyMap_[key] = value ? "true" : "false";
}

//-----------------------------------------------------------------------
//

void AgcAuth::setProperty_impl( std::string key, std::string value )
{
	propertyMap_[key] = value;
}

//-----------------------------------------------------------------------
//

bool AgcAuth::login( AgcmLogin * cmLogin, AgcmUILogin * uiLogin )
{
	if( !cmLogin || !uiLogin )
		return false;

	cmLogin->SetIDPassword( getProperty<char*>("member_id"), "autologin" );
	cmLogin->ConnectLoginServer();

	AgcmUIManager2* pcmUIManager = ( AgcmUIManager2* )uiLogin->GetModule( "AgcmUIManager2" );

	if( pcmUIManager )
		g_pEngine->WaitingDialog(NULL, pcmUIManager->GetUIMessage("LOGIN_WAITING"));

	return true;
}

//-----------------------------------------------------------------------
//

bool AgcAuth::encrypt( char * code, char * account, size_t account_size, char * password, size_t password_size )
{
	if( !code || !account || !password )
		return false;

	if( !md5Encrypt_.EncryptString( code, account, strlen(account) ) )
		return false;

	if( !md5Encrypt_.EncryptString( code, password, strlen(password) ) )
		return false;

	return true;
}

//-----------------------------------------------------------------------