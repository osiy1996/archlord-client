#define _WINSOCKAPI_
#include "AgcGPAuth.h"
#include <AgcmLogin/AgcmLogin.h>
#include <AgcmUILogin/AgcmUILogin.h>
#include <stdio.h>
#include <iostream>
#include "Iphlpapi.h"
#pragma comment( lib, "Iphlpapi.lib" )

//-----------------------------------------------------------------------
//

AgcGPAuth::~AgcGPAuth()
{

}

//-----------------------------------------------------------------------
//

bool AgcGPAuth::init( std::string & cmdline )
{
	if( isTest() )
	{
		setProperty( "AutoLogin", false );
		return true;
	}

	if( cmdline.empty() )
		return false;

	size_t idx = cmdline.find("2:");

	//런쳐로부터 넘어온 파라메터만 남기고 짜른다.
	// gameid + accountid + authkey
	//account id 추출

	cmdline = cmdline.substr( idx+2 );

	idx = cmdline.find('|');

	setProperty( "member_id", cmdline.substr( 0, idx ) );  

	std::string gamestring = cmdline.substr( idx+1 );


	//게임 런쳐로부터 넘어온 파라메터에 mac address를 붙이기.

	char aMacAddress[128] ={0,};

	IP_ADAPTER_INFO	IpAdapterInfo[16];

	ZeroMemory( IpAdapterInfo, sizeof( IpAdapterInfo ) );

	DWORD dwBufferLength = sizeof( IpAdapterInfo );
	DWORD dwStatus = ::GetAdaptersInfo( IpAdapterInfo, &dwBufferLength );
	if( dwStatus != NO_ERROR )
	{
		MessageBoxA( 0, "Cannot read local MAC Address\n", "Archlord", 0 );
		return false;
	}
	// Add MAC Address after GameString..
	for( int nDeviceCount = 0 ; nDeviceCount < 16 ; nDeviceCount++ )
	{
		if( IpAdapterInfo[ nDeviceCount ].Type == MIB_IF_TYPE_ETHERNET )
		{
			sprintf_s( aMacAddress, sizeof(aMacAddress), "%02x-%02x-%02x-%02x-%02x-%02x",
				IpAdapterInfo[ nDeviceCount ].Address[ 0 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 1 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 2 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 3 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 4 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 5 ] );

			break;
		}				
	}

	gamestring += '|';
	gamestring += aMacAddress;

	setProperty( "gamestring", gamestring );
	setProperty( "autologin", true );

	return true;
}

//-----------------------------------------------------------------------
//

void AgcGPAuth::error_handling()
{
	char* szString = "The game client is loaded only through the official website.\n closing the application please try again";
	if( MessageBox( NULL, szString, "Archlord", MB_OK ) )
		::ShellExecute( NULL, "open", "http://archlord.webzen.com", NULL, NULL, SW_SHOW );
}

//-----------------------------------------------------------------------
//

void AgcGPAuth::final_process()
{

}

//-----------------------------------------------------------------------
//

bool AgcGPAuth::login( AgcmLogin * cmLogin, AgcmUILogin * uiLogin )
{
	if( !cmLogin || !uiLogin )
		return false;

	cmLogin->SetIDPassword( AuthManager().getProperty<char*>("member_id"), "autopwd" );
	uiLogin->ConnectLoginServer();

	return true;
}

//-----------------------------------------------------------------------