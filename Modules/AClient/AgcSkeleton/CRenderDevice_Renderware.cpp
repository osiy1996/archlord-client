#include "CRenderDevice_Renderware.h"

#include <rwcore.h>

#include <rpworld.h>
#include <rplodatm.h>
#include <rppvs.h>
#include "rpmorph.h"
#include "rpmatfx.h"
#include "rpuvanim.h"
#include <rpcollis.h>
#include "rpspline.h"
#include <rprandom.h>
#include "rptoon.h"
#include "rtfsyst.h"
#include <rtpng.h>
#include <rtbmp.h>
#include <rttiff.h>
#include <AcuRpMatD3DFx/AcuRpMatD3DFx.h>
#include <AcuRpMatFxEx/AcuRpMatFxEx.h>
#include <AcuRpMTexture/AcuRpMTexture.h>
#include <AcuRpUVAnimData/AcuRpUVAnimData.h>
#include <AcuRtAmbOcclMap/AcuRtAmbOcclMap.h>
#include <AcuRWUtil/AgcdGeoTriAreaTbl.h>

#define rwRESOURCESDEFAULTARENASIZE			( 16 << 20 )



CRenderDevice_Renderware::CRenderDevice_Renderware( void )
{
	m_nCurrentSystemIndex = 0;
	m_nCurrentVideoMode = 0;
}

CRenderDevice_Renderware::~CRenderDevice_Renderware( void )
{
}

BOOL CRenderDevice_Renderware::OnCreate( stRenderDeivceSettings* pDeviceSettings )
{
	// DirectX �ʱ�ȭ
	::CoInitialize( NULL );

	// ����̽� �������� ����
	if( !CRenderDevice::OnCreate( pDeviceSettings ) ) return FALSE;

	// ���� �ʱ�ȭ
	if( !RwEngineInit( NULL, NULL, 0, rwRESOURCESDEFAULTARENASIZE ) ) return FALSE;

	// �÷����� Attach
	if( !_AttachPlugins() ) return FALSE;

	// ���� ���� �Ķ���� ����
	RwEngineOpenParams Param;
	if( !_MakeEngineOpenParams( &Param ) ) return FALSE;

	// ���� ����
	if( !RwEngineOpen( &Param ) )
	{
		RwEngineTerm();
		return FALSE;
	}

	// ���� ����̽� ����
	if( !_SelectVideoDevice() ) return FALSE;

	// ��Ƽ�����弼�����ϰ�.. 
	RwD3D9EngineSetMultiThreadSafe( TRUE );

	// ��Ƽ���ø� ���� ����
	RwD3D9EngineSetMultiSamplingLevels( m_DefaultSettings.m_nMultiSamplingLevel );

	// ���� �õ�
	if( !RwEngineStart() )
	{
        RwEngineClose();
        RwEngineTerm();
		return FALSE;
	}

	// �̹��� �δ� ���
	if( !_RegisterImageLoader() ) return FALSE;

	// ���Ͻý��� ����
	if( !_OpenFileSystem() ) return FALSE;

	return TRUE;
}

BOOL CRenderDevice_Renderware::OnDestroy( void )
{
	// ���Ͻý��� �ݱ�
	RtFSManagerClose();

	// ���� ����
	RwEngineStop();

	// ���� �ݱ�
	RwEngineClose();

	// ���� ����
	RwEngineTerm();

	// DirectX ����
	::CoUninitialize();
	return TRUE;
}

BOOL CRenderDevice_Renderware::OnSceneStart( void )
{
	return TRUE;
}

BOOL CRenderDevice_Renderware::OnSceneEnd( void )
{
	return TRUE;
}

BOOL CRenderDevice_Renderware::OnSetDefaultBlendMode( void )
{
	return TRUE;
}

BOOL CRenderDevice_Renderware::_SetMemoryAllocators( void )
{
	return TRUE;
}

BOOL CRenderDevice_Renderware::_AttachPlugins( void )
{
	// ���� �÷�����
	if( !RpWorldPluginAttach() ) return FALSE;
	if( !RpLODAtomicPluginAttach() ) return FALSE;
	if( !RpAmbOcclMapPluginAttach() ) return FALSE;

	// PVS�÷�����
	if( !RpPVSPluginAttach() ) return FALSE;
	if( !RpGeoTriAreaTblPluginAttach() ) return FALSE;

	// �ִϸ��̼� ���� �÷�����
	if( !RtAnimInitialize() ) return FALSE;
	if( !RpHAnimPluginAttach() ) return FALSE;
	if( !RpSkinPluginAttach() ) return FALSE;
	if( !RpMorphPluginAttach() ) return FALSE;

	// ���׸���(material) ȿ�� �÷�����
	if( !RpMatFXPluginAttach() ) return FALSE;
	if( !RpMatFxExPluginAttach() ) return FALSE;
	if( !RpMTexturePluginAttach() ) return FALSE;
	if( !RpMaterialD3DFxPluginAttach() ) return FALSE;

	// UV Animation �÷����� ����
	if( !RpUVAnimPluginAttach() ) return FALSE;
	if( !RpUVAnimDataPluginAttach() ) return FALSE;

	// �浹�÷�����
	if( !RpCollisionPluginAttach() ) return FALSE;

	// ���������� �÷�����
	if( !RpUserDataPluginAttach() ) return FALSE;

	// ���ö��� �÷�����
	if( !RpSplinePluginAttach() ) return FALSE;

	// ��ƼŬ���� �÷�����
	//if( !RpPTankPluginAttach() ) return FALSE;
	//if( !RpPrtStdPluginAttach() ) return FALSE;
	//if( !RpPrtAdvPluginAttach() ) return FALSE;

	// ��Ÿ
	if( !RpRandomPluginAttach() ) return FALSE;
	if( !RpToonPluginAttach() ) return FALSE;	

	return TRUE;
}

BOOL CRenderDevice_Renderware::_MakeEngineOpenParams( void* pParam )
{
	RwEngineOpenParams* pParams = ( RwEngineOpenParams* )pParam;
	if( !pParams ) return FALSE;

	pParams->displayID = m_DefaultSettings.m_hWnd;
	if( !pParams->displayID ) return FALSE;

	return TRUE;
}

BOOL CRenderDevice_Renderware::_SelectVideoDevice( void )
{
	int nSubSystemCount = RwEngineGetNumSubSystems();
	if( nSubSystemCount <= 0 ) return FALSE;
	if( nSubSystemCount > RW_MAX_SUBSYSTEM_COUNT ) return FALSE;

	RwSubSystemInfo	m_SubSystemInfo[ RW_MAX_SUBSYSTEM_COUNT ];
	memset( m_SubSystemInfo, 0, sizeof( RwSubSystemInfo ) * RW_MAX_SUBSYSTEM_COUNT );

	for( int nCount = 0 ; nCount < nSubSystemCount ; nCount++ )
	{
		RwEngineGetSubSystemInfo( &m_SubSystemInfo[ nCount ], nCount );
	}

	m_nCurrentSystemIndex = RwEngineGetCurrentSubSystem();
	m_nCurrentVideoMode = RwEngineGetCurrentVideoMode();

	// ȭ���ֻ����� �̸� �����س���..
	RwD3D9EngineSetRefreshRate( m_DefaultSettings.m_nRefreshRate );

	// Ǯ��ũ�� ���� ������ ���..
	if( m_DefaultSettings.m_bIsFullScreen )
	{
		int nVideMode = _GetSuitableVideoMode();
		if( nVideMode < 1 ) return FALSE;

		m_nCurrentVideoMode = nVideMode;
	}

	if( !RwEngineSetSubSystem( m_nCurrentSystemIndex ) ) return FALSE;
	if( !RwEngineSetVideoMode( m_nCurrentVideoMode ) ) return FALSE;

	RECT Rect;
	::GetClientRect( m_DefaultSettings.m_hWnd, &Rect );

	m_DefaultSettings.m_nScreenWidth = Rect.right;
	m_DefaultSettings.m_nScreenHeight = Rect.bottom;
	return TRUE;
}

int CRenderDevice_Renderware::_GetSuitableVideoMode( void )
{
	int nVideoModeCount = RwEngineGetNumVideoModes();
	int nDetectedVideoMode = -1;

	for( int nCount = 0 ; nCount < nVideoModeCount ; nCount++ )
	{
		RwVideoMode rwVideoMode;
		if( RwEngineGetVideoModeInfo( &rwVideoMode,	nCount ) )
		{
			if( rwVideoMode.width == m_DefaultSettings.m_nScreenWidth && rwVideoMode.height == m_DefaultSettings.m_nScreenHeight &&
				rwVideoMode.depth == m_DefaultSettings.m_nScreenDepth && ( rwVideoMode.flags & rwVIDEOMODEEXCLUSIVE ) )
			{
				nDetectedVideoMode = nCount;
			}
		}
	}

	return nDetectedVideoMode;
}

BOOL CRenderDevice_Renderware::_RegisterImageLoader( void )
{
    if( !RwImageRegisterImageFormat( RWSTRING( "bmp" ), RtBMPImageRead, 0 ) ) return FALSE;
    if( !RwImageRegisterImageFormat( RWSTRING( "png" ), RtPNGImageRead, 0 ) ) return FALSE;
    if( !RwImageRegisterImageFormat( RWSTRING( "tif" ), RtTIFFImageRead, 0 ) ) return FALSE;
	return TRUE;
}

BOOL CRenderDevice_Renderware::_OpenFileSystem( void )
{
	if( !RtFSManagerOpen( RTFSMAN_UNLIMITED_NUM_FS ) ) return FALSE;
	if( !_InstallFileSystem() ) return FALSE;

	return TRUE;
}

BOOL CRenderDevice_Renderware::_InstallFileSystem( void )
{
	RtFileSystem* sfstWfs = NULL;
	RtFileSystem* sfstUnc = NULL;
	
	char strCurrentPath[ 256 ] = { 0, };
	memset( strCurrentPath, 0, sizeof( char ) * 256 );
	DWORD dwPathNameLength = ::GetCurrentDirectory( 256, strCurrentPath );
	if( dwPathNameLength <= 0 || dwPathNameLength > 256 ) return FALSE;

	strCurrentPath[ dwPathNameLength ] = '\\';
	sfstUnc = RtWinFSystemInit( m_DefaultSettings.m_nMaxOpenFiles, strCurrentPath, "unc" );
	if( !sfstUnc ) return FALSE;
	if( !RtFSManagerRegister( sfstUnc ) ) return FALSE;

	CharUpper( strCurrentPath );
	int nDriveCount = ::GetLogicalDrives();

	char strFileSystemName[ 2 ] = { 0, };
	int nCount = 0;
	for( nCount = 2, nDriveCount >>= 1 ; nDriveCount != 0 ; nCount++, nDriveCount >>= 1 )
	{
		if( nDriveCount & 0x01 )
		{
			char strDriveName[ 4 ] = { 0, };

			strDriveName[ 0 ] = nCount + 'A' - 1;
			strDriveName[ 1 ] = ':';
			strDriveName[ 2 ] = '\\';   // Needed for Win98
			strDriveName[ 3 ] = '\0';

			int nDriveType = ::GetDriveType( strDriveName );
			if( nDriveType != DRIVE_UNKNOWN && nDriveType != DRIVE_NO_ROOT_DIR )
			{
				strDriveName[ 2 ]  = '\0';

				strFileSystemName[ 0 ] = strDriveName[ 0 ];
				strFileSystemName[ 1 ] = '\0';

				sfstWfs = RtWinFSystemInit( m_DefaultSettings.m_nMaxOpenFiles, strDriveName, strFileSystemName );
				if( !sfstWfs ) return FALSE;
				if( !RtFSManagerRegister( sfstWfs ) ) return FALSE;

				if( strCurrentPath[ 1 ] != ':' )
				{
					RtFSManagerSetDefaultFileSystem( sfstUnc );
				}
				else if( strDriveName[ 0 ] == strCurrentPath[ 0 ] )
				{
					RtFSManagerSetDefaultFileSystem( sfstWfs );
				}
			}
		}
	}

	return TRUE;
}
