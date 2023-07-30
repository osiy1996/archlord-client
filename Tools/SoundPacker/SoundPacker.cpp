// SoundPacker.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SoundPacker.h"
#include "SoundPackerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSoundPackerApp

BEGIN_MESSAGE_MAP(CSoundPackerApp, CWinApp)
	//{{AFX_MSG_MAP(CSoundPackerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoundPackerApp construction

CSoundPackerApp::CSoundPackerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSoundPackerApp object

CSoundPackerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSoundPackerApp initialization

BOOL CSoundPackerApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	if( m_lpCmdLine && strlen( m_lpCmdLine ) > 0 )
	{
		if( !DoExport() ) return FALSE;

		char strSrcPath[ 256 ] = { 0, };
		char strDstPath[ 256 ] = { 0, };

		sscanf( m_lpCmdLine, "%s %s", strSrcPath, strDstPath );

		if( !DoMoveExportedFiles( strSrcPath, strDstPath ) )
		{
			OutputDebugString( "Failed to Export Sound Data\n" );
		}

		return FALSE;
	}
	else
	{
		CSoundPackerDlg dlg;
		m_pMainWnd = &dlg;

		int nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

BOOL CSoundPackerApp::DoExport( void )
{
	CSoundPackerDlg dlg;
	m_pMainWnd = &dlg;

	dlg.DoModal();
	return TRUE;
}

BOOL CSoundPackerApp::DoMoveExportedFiles( char* pSrcPath, char* pDstPath )
{
	if( !pSrcPath || !pDstPath ) return FALSE;

	DeleteAllSubFiles( pDstPath );
	CreateSubFolder( pDstPath, "Sound" );

	char strSrcPath[ 256 ] = { 0, };
	sprintf_s( strSrcPath, sizeof( char ) * 256, "%s\\%s", pSrcPath, "Sound" );

	char strDstPath[ 256 ] = { 0, };
	sprintf_s( strDstPath, sizeof( char ) * 256, "%s\\%s", pDstPath, "Sound" );

	CreateSubFolder( strDstPath, "BGM" );
	CreateSubFolder( strDstPath, "Effect" );
	CreateSubFolder( strDstPath, "Stream" );
	CreateSubFolder( strDstPath, "UI" );

	char strFileSoundInfoSrc[ 256 ] = { 0, };
	sprintf_s( strFileSoundInfoSrc, sizeof( char ) * 256, "%s\\%s", strSrcPath, "SoundPak.info" );

	char strFileSoundInfoDst[ 256 ] = { 0, };
	sprintf_s( strFileSoundInfoDst, sizeof( char ) * 256, "%s\\%s", strDstPath, "SoundPak.info" );

	if( !DoMoveFile( strFileSoundInfoSrc, strFileSoundInfoDst, TRUE ) ) return FALSE;

	if( !DoMoveFolder( strSrcPath, strDstPath, "BGM", "*.mp3", FALSE ) ) return FALSE;
	if( !DoMoveFolder( strSrcPath, strDstPath, "Effect", "*.pk", TRUE ) ) return FALSE;
	if( !DoMoveFolder( strSrcPath, strDstPath, "Stream", "*.mp3", FALSE ) ) return FALSE;
	if( !DoMoveFolder( strSrcPath, strDstPath, "UI", "*.wav", FALSE ) ) return FALSE;

	return TRUE;
}

BOOL CSoundPackerApp::DoMoveFolder( char* pSrcPath, char* pDstPath, char* pSubPath, char* pWildCard, BOOL bIsDeleteFile )
{
	if( !pSrcPath || !pDstPath || !pSubPath || !pWildCard ) return FALSE;

	char strSrcPath[ 256 ] = { 0, };
	sprintf_s( strSrcPath, sizeof( char ) * 256, "%s\\%s", pSrcPath, pSubPath );

	char strDstPath[ 256 ] = { 0, };
	sprintf_s( strDstPath, sizeof( char ) * 256, "%s\\%s", pDstPath, pSubPath );

	if( !DoMoveFiles( strSrcPath, pWildCard, strDstPath, bIsDeleteFile ) ) return FALSE;
	return TRUE;
}

BOOL CSoundPackerApp::DoMoveFiles( char* pSrcPath, char* pSrcWildCard, char* pDstPath, BOOL bIsDeleteFile )
{
	if( !pSrcPath || !pSrcWildCard || !pDstPath ) return FALSE;

	WIN32_FIND_DATA Find;

	char strSearch[ 256 ] = { 0, };
	sprintf_s( strSearch, sizeof( char ) * 256, "%s\\%s", pSrcPath, pSrcWildCard );
	
	HANDLE hFind = ::FindFirstFile( strSearch, &Find );
	if( hFind == INVALID_HANDLE_VALUE ) return FALSE;

	char strSrcFileName[ 256 ] = { 0, };
	char strDstFileName[ 256 ] = { 0, };

	if( !( Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		sprintf_s( strSrcFileName, sizeof( char ) * 256, "%s\\%s", pSrcPath, Find.cFileName );
		sprintf_s( strDstFileName, sizeof( char ) * 256, "%s\\%s", pDstPath, Find.cFileName );

		if( !DoMoveFile( strSrcFileName, strDstFileName, bIsDeleteFile ) ) return FALSE;
	}

	while( ::FindNextFile( hFind, &Find ) )
	{
		if( !( Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
		{
			memset( strSrcFileName, 0, sizeof( char ) * 256 );
			sprintf_s( strSrcFileName, sizeof( char ) * 256, "%s\\%s", pSrcPath, Find.cFileName );

			memset( strDstFileName, 0, sizeof( char ) * 256 );
			sprintf_s( strDstFileName, sizeof( char ) * 256, "%s\\%s", pDstPath, Find.cFileName );

			if( !DoMoveFile( strSrcFileName, strDstFileName, bIsDeleteFile ) ) return FALSE;
		}
	}

	::FindClose( hFind );
	return TRUE;
}

BOOL CSoundPackerApp::DoMoveFile( char* pSrcFileName, char* pDstFileName, BOOL bIsDeleteFile )
{
	if( !pSrcFileName || !pDstFileName ) return FALSE;

	char strMessage[ 256 ] = { 0, };
	sprintf( strMessage, "Copy File %s --> %s ...", pSrcFileName, pDstFileName );
	OutputDebugString( strMessage );

	if( !::CopyFile( pSrcFileName, pDstFileName, FALSE ) )
	{
		char strDebug[ 256 ] = { 0, };
		sprintf_s( strDebug, sizeof( char ) * 256, "Failed to copy file, %s --> %s\n", pSrcFileName, pDstFileName );
		OutputDebugString( strDebug );
		return FALSE;
	}

	OutputDebugString( "Success!\n" );

	if( bIsDeleteFile )
	{
		memset( strMessage, 0, sizeof( char ) * 256 );
		sprintf( strMessage, "Delete File %s ...", pSrcFileName );
		OutputDebugString( strMessage );

		if( !::DeleteFile( pSrcFileName ) )
		{
			char strDebug[ 256 ] = { 0, };
			sprintf_s( strDebug, sizeof( char ) * 256, "Failed to delete file, %s\n", pSrcFileName );
			OutputDebugString( strDebug );
		}

		OutputDebugString( "Success!\n" );
	}

	return TRUE;
}

BOOL CSoundPackerApp::DeleteAllSubFiles( char* pDstPath )
{
	if( !pDstPath ) return FALSE;

	WIN32_FIND_DATA Find;

	char strSearch[ 256 ] = { 0, };
	sprintf_s( strSearch, sizeof( char ) * 256, "%s\\%s", pDstPath, "*" );
	
	HANDLE hFind = ::FindFirstFile( strSearch, &Find );
	if( hFind == INVALID_HANDLE_VALUE ) return FALSE;

	char strDeleteFileName[ 256 ] = { 0, };
	if( strcmp( Find.cFileName, "." ) && strcmp( Find.cFileName, ".." ) )
	{
		sprintf_s( strDeleteFileName, sizeof( char ) * 256, "%s\\%s", pDstPath, Find.cFileName );

		if( Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( !DeleteAllSubFiles( strDeleteFileName ) ) return FALSE;
			
			if( !::RemoveDirectory( strDeleteFileName ) )
			{
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, sizeof( char ) * 256, "Failed to delete Folder, %s\n", strDeleteFileName );
				OutputDebugString( strDebug );
			}
		}
		else
		{
			if( !::DeleteFile( strDeleteFileName ) )
			{
				char strDebug[ 256 ] = { 0, };
				sprintf_s( strDebug, sizeof( char ) * 256, "Failed to delete file, %s\n", strDeleteFileName );
				OutputDebugString( strDebug );
			}
		}
	}

	while( ::FindNextFile( hFind, &Find ) )
	{
		if( strcmp( Find.cFileName, "." ) && strcmp( Find.cFileName, ".." ) )
		{
			memset( strDeleteFileName, 0, sizeof( char ) * 256 );
			sprintf_s( strDeleteFileName, sizeof( char ) * 256, "%s\\%s", pDstPath, Find.cFileName );

			if( Find.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			{
				if( !DeleteAllSubFiles( strDeleteFileName ) ) return FALSE;

				if( !::RemoveDirectory( strDeleteFileName ) )
				{
					char strDebug[ 256 ] = { 0, };
					sprintf_s( strDebug, sizeof( char ) * 256, "Failed to delete Folder, %s\n", strDeleteFileName );
					OutputDebugString( strDebug );
				}
			}
			else
			{
				if( !::DeleteFile( strDeleteFileName ) )
				{
					char strDebug[ 256 ] = { 0, };
					sprintf_s( strDebug, sizeof( char ) * 256, "Failed to delete file, %s\n", strDeleteFileName );
					OutputDebugString( strDebug );
				}
			}
		}
	}

	::FindClose( hFind );
	return TRUE;
}

BOOL CSoundPackerApp::CreateSubFolder( char* pSrcPath, char* pFolderName )
{
	if( !pSrcPath || !pFolderName ) return FALSE;
	if( strlen( pSrcPath ) <= 0 || strlen( pFolderName ) <= 0 ) return FALSE;

	char strCreatePathName[ 256 ] = { 0, };
	sprintf_s( strCreatePathName, sizeof( char ) * 256, "%s\\%s", pSrcPath, pFolderName );
	return ::CreateDirectory( strCreatePathName, NULL );	
}
