#if defined( _AREA_GLOBAL_ ) || defined( _AREA_TAIWAN_ )

#include <windows.h>
#include <stdio.h>
#include <iostream>
#include "Iphlpapi.h"
#include "AgcmGlobalAuth.h"

#define	MACADDRESS_LEN 32

#ifdef _SHARE_MEM_
	#include "ISharemem.h"
#endif

#pragma comment( lib, "Iphlpapi.lib" )

AgcmGlobalAuth g_gbAuth;

inline BOOL IsFileExist( const char* path )
{
	DWORD dwAttr = GetFileAttributes( path );
	return dwAttr != INVALID_FILE_ATTRIBUTES && ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) == 0;
}

AgcmGlobalAuth::AgcmGlobalAuth():m_autoLogin(TRUE)
{
}

AgcmGlobalAuth::~AgcmGlobalAuth()
{
}

//런쳐로부터 넘어온 파라메터를 파싱하고 로컬pc의 랜카드 mac address 를 수집한다.
bool AgcmGlobalAuth::Initialize(char* szCmdLine)
{
	if( IsFileExist( "NoExcuteAuth.arc" ) )
	{
		m_autoLogin	= FALSE;
		return	TRUE;
	}

	if( NULL == szCmdLine )
		return FALSE;

	string tmp = szCmdLine;
	string::size_type idx = tmp.find("2:");
	
	//런쳐로부터 넘어온 파라메터만 남기고 짜른다.
	// gameid + accountid + authkey
	//account id 추출
	tmp = tmp.substr( idx+2 );
	
	idx = tmp.find('|');

	// Find gameid
	m_szAccountId = tmp.substr( 0, idx );
	m_szGameString = tmp.substr( idx+1 );

	if( idx == string::npos )
		return FALSE;

	//게임 런쳐로부터 넘어온 파라메터에 mac address를 붙이기.
	char aMacAddress[MACADDRESS_LEN] ={0,};
	IP_ADAPTER_INFO		IpAdapterInfo[16] = {0,};

	DWORD dwBufferLength = sizeof( IpAdapterInfo );
	DWORD dwStatus = ::GetAdaptersInfo( IpAdapterInfo, &dwBufferLength );
	if( dwStatus != NO_ERROR )
	{
		OutputDebugString( "Cannot read local MAC Address\n" );
		return FALSE;
	}
	// Add MAC Address after GameString..
	for( int nDeviceCount = 0 ; nDeviceCount < 16 ; nDeviceCount++ )
	{
		if( IpAdapterInfo[ nDeviceCount ].Type == MIB_IF_TYPE_ETHERNET )
		{
			sprintf_s( aMacAddress, sizeof( char ) * MACADDRESS_LEN, "%02x-%02x-%02x-%02x-%02x-%02x",
				IpAdapterInfo[ nDeviceCount ].Address[ 0 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 1 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 2 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 3 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 4 ],
				IpAdapterInfo[ nDeviceCount ].Address[ 5 ] );
		}				
	}
	
	m_szGameString += '|';
	m_szGameString += aMacAddress;
#ifdef _DEBUG
	::MessageBox( NULL, m_szGameString.c_str(), "Archlord Authentication", MB_OK );
#endif

	return TRUE;
}

#ifdef _SHARE_MEM_
bool AgcmGlobalAuth::ProcessBySharedMemory()
{
	// 공유메모리를 관리하는 RMSM 클래스 객체를 생성하면서 동시에 공유메모리 공간을 생성시킴
	RMSM sm(CreateShareMem());

	// GAME_NAME은 정보를 담고 있는 맵파일의 NAME을 가리킴.
	std::string strGameName(GAMENAME);

	// 공유메모리에 저장되어 있는 정보를 읽어올 구조체를 할당.
	// (strUserID는 해당 유저 ID, strAuthCode는 인증정보임.)
	WLAUTH_INFO AuthInfo = {0};

	StringCbPrintf(AuthInfo.id, sizeof(AuthInfo.id), "%s", m_szGameId);
	RtlCopyMemory(&AuthInfo.key, static_cast<void*>(m_szGameString), sizeof(AuthInfo.key));

	/////////////////////////////////////////////////////////////////
	// WLAUTH_INFO 구조체에 공유메모리에 있는 정보를 읽어온다.
	if (0 != (sm.get())->ReadFromShareMemory(strGameName.c_str(), &AuthInfo) )
	{
		/////////////////////////////////////////////////////
		// 맵파일에서 정보를 읽는 작업이 실패할 경우.
		return FALSE;
	}

	//얻어온 아이디와 인증 키값 저장해야함...tj@090526
	return TRUE;
}
#endif

#endif _AREA_GLOBAL_