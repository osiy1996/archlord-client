#define _WINSOCKAPI_
#include "CGameFramework.h"
#include "CGameFramework_ArchlordKorea.h"
#include "CGameFramework_ArchlordGlobal.h"
#include "CGameFramework_ArchlordJapan.h"
#include "CGameFramework_ArchlordChina.h"

#include "CGameFramework_Utility.h"
#include "stdio.h"

#include <AgcmRegistryManager/AgcmRegistryManager.h>

static CGameFramework* g_pGameFramework = NULL;
CGameFramework*	CGameFramework::GetGameFramework( char* pNationName )
{
	if( !pNationName || strlen( pNationName ) <= 0 ) return g_pGameFramework;

	if( !g_pGameFramework )
	{
		if( strcmp( pNationName, "Korea" ) == 0 )
		{
			g_pGameFramework = new CGameFramework_ArchlordKorea;
		}
		else if( strcmp( pNationName, "Global" ) == 0 )
		{
			g_pGameFramework = new CGameFramework_ArchlordGlobal;
		}
		else if( strcmp( pNationName, "Japan" ) == 0 )
		{
			g_pGameFramework = new CGameFramework_ArchlordJapan;
		}
		else if( strcmp( pNationName, "China" ) == 0 )
		{
			g_pGameFramework = new CGameFramework_ArchlordChina;
		}
	}

	return g_pGameFramework;
}

CGameFramework::CGameFramework( void )
{
	m_pRenderDevice = NULL;
	m_hInstance = NULL;
	m_fnWndProc = NULL;
}

CGameFramework::~CGameFramework( void )
{
}

BOOL CGameFramework::OnGameStart( HINSTANCE hInstacne, WNDPROC fnWndProc )
{
	m_hInstance = hInstacne;
	m_fnWndProc = fnWndProc;

	if( !CreateRenderWindow( "INI\\COption.ini" ) ) return FALSE;
	if( !CreateRenderDevice( "Renderware" ) ) return FALSE;
	if( !LoadGameResource() ) return FALSE;

	return TRUE;
}

BOOL CGameFramework::OnGameLoop( void )
{
	return TRUE;
}

BOOL CGameFramework::OnGameEnd( void )
{
	if( !DestroyRenderDevice() ) return FALSE;
	if( !DestroyRenderWindow() ) return FALSE;

	return TRUE;
}

BOOL CGameFramework::ParseCommandLine( void )
{
	return TRUE;
}

BOOL CGameFramework::CheckPatchCode( char* pPatchCode )
{
	// 아래의 파일이 존재하면 패치코드검사를 건너뛴다.
	FILE* pFile = NULL;
	fopen_s( &pFile, "NotCheckPatchCode.arc", "rb" );

	if( pFile )
	{
		fclose( pFile );
		return TRUE;
	}

	if( !pPatchCode )
	{
#ifdef _DEBUG
		OutputDebugString( "Receive Invalid PatchCodeString\n" );
#endif
		return FALSE;
	}

	int nPatchCodeLength = ( int )strlen( pPatchCode );
	if( nPatchCodeLength <= 0 || nPatchCodeLength >= 32 )
	{
#ifdef _DEBUG
		OutputDebugString( "Receive Invalid PatchCodeString\n" );
#endif
		return FALSE;
	}

	for( int nCount = 0 ; nCount < nPatchCodeLength ; nCount++ )
	{
		if( nCount != 0 && nCount % 2 == 0 )
		{
			char cTemp = pPatchCode[ nCount - 1 ];
			pPatchCode[ nCount - 1 ] = pPatchCode[ nCount ];
			pPatchCode[ nCount ] = cTemp;
		}
	}

	char strTimeLength[ 3 ] = { 0, };
	strTimeLength[ 0 ] = pPatchCode[ 0 ];
	strTimeLength[ 1 ] = pPatchCode[ 1 ];

	int nTimeLength = atoi( strTimeLength );
	char* pPatchCodePos = &pPatchCode[ nTimeLength + 2 ];
	__int64 nPatchCode = _atoi64( pPatchCodePos );
	DWORD dwPatchCode = ( DWORD )nPatchCode;

	/*
	HKEY hKey = NULL;
	LONG nResult = ::RegOpenKeyEx( HKEY_LOCAL_MACHINE, "SOFTWARE\\ArchLord", 0, KEY_READ, &hKey );
	if( nResult != ERROR_SUCCESS ) return FALSE;

	DWORD dwValueType = 0;
	DWORD dwValue = 0;
	DWORD dwValueLength = sizeof( DWORD );

	nResult = ::RegQueryValueEx( hKey, "Code", 0, &dwValueType, ( LPBYTE )( &dwValue ), &dwValueLength );
	if( nResult != ERROR_SUCCESS ) return FALSE;

	RegCloseKey( hKey );
	*/

	DWORD dwValue = AgcmRegistryManager::GetProperty<DWORD>( REGIKEY_PATHCHCODE );
	
	BOOL bIsValidPatchCode = dwPatchCode == dwValue ? TRUE : FALSE;

#ifdef _DEBUG
	if( !bIsValidPatchCode )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "Invalid Patch Code : 0x%d ( VaildCode = 0x%d )\n", dwPatchCode, dwValue );
		OutputDebugString( strDebug );
	}
#endif

	return bIsValidPatchCode;
}

BOOL CGameFramework::CreateRenderWindow( char* pSettingFileName )
{
	if( !pSettingFileName || strlen( pSettingFileName ) <= 0 ) return FALSE;
	if( !LoadDeviceSettings( pSettingFileName ) ) return FALSE;

	if( !m_RenderWindow.OnCreate( m_hInstance, m_DeviceSettings.m_nScreenWidth, m_DeviceSettings.m_nScreenHeight, 
		m_DeviceSettings.m_nScreenDepth, m_DeviceSettings.m_bIsFullScreen, m_fnWndProc ) ) return FALSE;

	return TRUE;
}

BOOL CGameFramework::CreateRenderDevice( char* pRendererName )
{
	if( !pRendererName || strlen( pRendererName ) <= 0 ) return FALSE;

	m_pRenderDevice = CRenderDevice::GetRenderDevice( pRendererName );
	if( !m_pRenderDevice ) return FALSE;

	if( !m_pRenderDevice->OnCreate( &m_DeviceSettings ) ) return FALSE;
	return TRUE;
}

BOOL CGameFramework::DestroyRenderWindow( void )
{
	return m_RenderWindow.OnDestroy();
}

BOOL CGameFramework::DestroyRenderDevice( void )
{
	return m_pRenderDevice ? m_pRenderDevice->OnDestroy() : TRUE;
}

BOOL CGameFramework::LoadDeviceSettings( char* pFileName )
{
	if( !pFileName || strlen( pFileName ) <= 0 ) return FALSE;
	return TRUE;
}

BOOL CGameFramework::LoadGameResource( void )
{
	return TRUE;
}
