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

//���ķκ��� �Ѿ�� �Ķ���͸� �Ľ��ϰ� ����pc�� ��ī�� mac address �� �����Ѵ�.
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
	
	//���ķκ��� �Ѿ�� �Ķ���͸� ����� ¥����.
	// gameid + accountid + authkey
	//account id ����
	tmp = tmp.substr( idx+2 );
	
	idx = tmp.find('|');

	// Find gameid
	m_szAccountId = tmp.substr( 0, idx );
	m_szGameString = tmp.substr( idx+1 );

	if( idx == string::npos )
		return FALSE;

	//���� ���ķκ��� �Ѿ�� �Ķ���Ϳ� mac address�� ���̱�.
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
	// �����޸𸮸� �����ϴ� RMSM Ŭ���� ��ü�� �����ϸ鼭 ���ÿ� �����޸� ������ ������Ŵ
	RMSM sm(CreateShareMem());

	// GAME_NAME�� ������ ��� �ִ� �������� NAME�� ����Ŵ.
	std::string strGameName(GAMENAME);

	// �����޸𸮿� ����Ǿ� �ִ� ������ �о�� ����ü�� �Ҵ�.
	// (strUserID�� �ش� ���� ID, strAuthCode�� ����������.)
	WLAUTH_INFO AuthInfo = {0};

	StringCbPrintf(AuthInfo.id, sizeof(AuthInfo.id), "%s", m_szGameId);
	RtlCopyMemory(&AuthInfo.key, static_cast<void*>(m_szGameString), sizeof(AuthInfo.key));

	/////////////////////////////////////////////////////////////////
	// WLAUTH_INFO ����ü�� �����޸𸮿� �ִ� ������ �о�´�.
	if (0 != (sm.get())->ReadFromShareMemory(strGameName.c_str(), &AuthInfo) )
	{
		/////////////////////////////////////////////////////
		// �����Ͽ��� ������ �д� �۾��� ������ ���.
		return FALSE;
	}

	//���� ���̵�� ���� Ű�� �����ؾ���...tj@090526
	return TRUE;
}
#endif

#endif _AREA_GLOBAL_