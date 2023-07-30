// MapTool.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#pragma warning(disable:4786) //disable linker key > 255 chars long (for stl map)
#include <map>
#include "MapTool.h"
#include "SplashDlg.h"
#include <afxadv.h>
#include "MainFrm.h"
#include "ProgressDlg.h"
#include "ApUtil.h"

#include "AgcmEventNature.h"
#include "AgcmRender.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;

#endif

#include "ConstDlg.h"
#include "MapSelectDlg.h"
#include "AuZpackLoader.h"

#include "crtdbg.h"

#define	DEFAULT_EXCEPTION_FILENAME	"Crash%02d%02d-%02d%02d.txt"

typedef struct _EXCEPTION_POINTERS EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

//@{ Jaewon 20041125
// pfnLogAfterMiniDumpCallback
typedef const char *(*PFNLOGAFTERMINIDUMPCALLBACK)(EXCEPTION_POINTERS* pExPtrs);
int __cdecl RecordExceptionInfo(PEXCEPTION_POINTERS data, const char *Message, const char *lpszAdditionalLogs,
								PFNLOGAFTERMINIDUMPCALLBACK pfnLogAfterMiniDumpCallback);

DWORD dwOpts = 
	//GSTSO_PARAMS |
	GSTSO_MODULE |
	GSTSO_SYMBOL |
	GSTSO_SRCLINE;

LONG __stdcall ExcepCallBack ( EXCEPTION_POINTERS * pExPtrs )
{

	try
	{
		char	strOutputFile[ 1024 ];
		//DWORD	dwCurrentTime = ::time(NULL);

		time_t long_time;
		time( &long_time );                /* Get time as long integer. */

		tm * pLocalTime;
		pLocalTime = localtime(  &long_time );

		wsprintf( strOutputFile , DEFAULT_EXCEPTION_FILENAME ,
			pLocalTime->tm_mon	, pLocalTime->tm_mday	,
			pLocalTime->tm_hour	, pLocalTime->tm_min	);

		FILE	* pFile = fopen( strOutputFile , "wt" );
		if( pFile )
		{
			fprintf ( pFile ,  "%s\n" , GetFaultReason ( pExPtrs ) ) ;
			fprintf ( pFile , "%s\n" , GetRegisterString ( pExPtrs ) ) ;
			const char * szBuff = GetFirstStackTraceString ( dwOpts  ,
				pExPtrs  ) ;
			do
			{
				fprintf ( pFile , "%s\n" , szBuff ) ;
				fflush ( pFile ) ;
				szBuff = GetNextStackTraceString ( dwOpts , pExPtrs ) ;
			}
			while ( NULL != szBuff ) ;

			fclose( pFile );
		}

		g_pMainFrame->MessageBox( "���α׷� �׾���� , �α� ������ϴ�~" , "����" , MB_OK | MB_ICONERROR );

		RecordExceptionInfo(pExPtrs, "AlefClient", "Maptool" , NULL );
	}
	catch( ... )
	{
		// do nothing..
	}
	
	//ExitProcess( 1 );
	PostQuitMessage(1);
	return ( EXCEPTION_CONTINUE_SEARCH ) ;
}

//global path to pass to browse call back so that it can set the initial directory
namespace
{
   CString szPath;
   const float TRAVERSE_SPEED = 500;   //number of steps to traverse average world span
}

/////////////////////////////////////////////////////////////////////////////
// CMapToolApp

BEGIN_MESSAGE_MAP(CMapToolApp, CWinApp)
	//{{AFX_MSG_MAP(CMapToolApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMapToolApp construction

CMapToolApp::CMapToolApp()
{
	m_bFailSelecting = FALSE;

	if( ::CreateMutex( NULL , TRUE , "AlefMaptool" ) )
	{
		if( GetLastError() == ERROR_ALREADY_EXISTS )
		{
			MessageBox( NULL , "������ ���� �����߿� �ֽ��ϴ�. �����ϰ� �����ϼ���" , "Maptool Launch" , MB_ICONERROR | MB_OK );
			//ExitProcess( 1 );
			PostQuitMessage(1);
		}
	}
	else
	{
		// ���� �̻�~
		MessageBox( NULL , "������ ���� �����߿� �ֽ��ϴ�. �����ϰ� �����ϼ���" , "Maptool Launch" , MB_ICONERROR | MB_OK );
		//ExitProcess( 1 );
		PostQuitMessage(1);
	}
	
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
#ifdef _DEBUG	
	int	nErrorCode = 0;
	if( nErrorCode )
		_CrtSetBreakAlloc( nErrorCode );
#endif

	m_pProgressDlg	= NULL;
}


CMapToolApp::~CMapToolApp()
{

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMapToolApp object

CMapToolApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMapToolApp initialization

BOOL CMapToolApp::InitInstance()
{
	if ( ( int ) TRUE != SetCrashHandlerFilter ( ExcepCallBack ) )
	{
		TRACE( "ũ���� �ڵ鷯 ���� ���� -_-;;;\n" );
	}

	// Ŀ�ǵ���� �м�..
	TRACE( GetCommandLine() );
	TRACE( "\n" );

	g_Const.LoadBmpResource();

	CGetArg2	arg;
	arg.SetParam( GetCommandLine() );
	if( arg.GetArgCount() >= 2 )
	{
		INT32	nLoadMode		= LOAD_NORMAL;

		BOOL	bCompactData	= FALSE;
		BOOL	bMapDetail		= FALSE;
		BOOL	bObject			= FALSE;
		BOOL	bMapRough		= FALSE;
		BOOL	bTile			= FALSE;
		BOOL	bServer			= FALSE;

		CString	strFilename	;
		INT32	nDivision	= 0;

		// �ɼ�üũ.
		if( !strcmp( arg.GetParam( 1 ) , "ed" ) || 
			!strcmp( arg.GetParam( 1 ) , "ED" )	|| 
			!strcmp( arg.GetParam( 1 ) , "eD" )	|| 
			!strcmp( arg.GetParam( 1 ) , "Ed" )	)
		{
			nLoadMode = LOAD_EXPORT_DIVISION;

			nDivision = atoi( arg.GetParam( 2 ) );

			TRACE( "�ͽ���Ʈ ����� ���\n" );
			TRACE( "(%d) ������� ����\n" , nDivision );
		}
		else
		if( !strcmp( arg.GetParam( 1 ) , "el" ) || 
			!strcmp( arg.GetParam( 1 ) , "EL" )	|| 
			!strcmp( arg.GetParam( 1 ) , "eL" )	|| 
			!strcmp( arg.GetParam( 1 ) , "El" )	)
		{
			nLoadMode = LOAD_EXPORT_LIST;

			strFilename = arg.GetParam( 2 );

			TRACE( "�ͽ���Ʈ ����� ���\n" );
			TRACE( "(%s) ������ ������ ����\n" , strFilename );

			CMapSelectDlg	dlg;
			if( dlg.LoadSelection( strFilename ) )
			{
				if( dlg.m_MapSelectStatic.m_vectorDivision.size() > 1 )
				{
					// ù ���� ����.
					nDivision = dlg.m_MapSelectStatic.m_vectorDivision[ 0 ];
				}
				else
				{
					MessageBox( NULL , "���� �������� ��ġ ����" , "���� �ͽ���Ʈ �ڵ�ȭ" , MB_OK | MB_ICONERROR );
					return FALSE;
				}
			}
			else
			{
				MessageBox( NULL , "���� �������� ã�� ���� ����" , "���� �ͽ���Ʈ �ڵ�ȭ" , MB_OK | MB_ICONERROR );
				return FALSE;
			}

		}
		else
		if( !strcmp( arg.GetParam( 1 ) , "ms" ) || 
			!strcmp( arg.GetParam( 1 ) , "MS" )	|| 
			!strcmp( arg.GetParam( 1 ) , "mS" )	|| 
			!strcmp( arg.GetParam( 1 ) , "Ms" )	)
		{
			// ���� ���� Dialog �� ���.
			// strFilename = arg.GetParam( 2 );

			// ������ ���� �ϵ��ڵ���.
			strFilename = "range.ini";
			ALEF_LOADING_MODE	= LOAD_EXPORT_DIVISION; // ������ ������ ����..

			CMapSelectDlg msdlg;

			if( !msdlg.LoadSelection( strFilename ) )
			{
				::MessageBox( NULL , "����" , "�⺻���������� ��� �׳� ���ϴ�." , MB_OK );
			}

			if( IDOK == msdlg.DoModal( EXPORT_RANGE_SELECT ) )
			{
				if( msdlg.SaveSelection( "currange.ini" ) )
				{
					return TRUE;
				}
				else
				{
					m_bFailSelecting = TRUE;
					return FALSE;
				}
			}
			else
			{
				m_bFailSelecting = TRUE;
				return FALSE;
			}
		}

		if( nLoadMode != LOAD_NORMAL )
		{
			// CDROT
			char	* pParam = arg.GetParam( 3 );
			for( int i = 0 ; i < ( int ) strlen( pParam ) ; i ++ )
			{
				if( pParam[ i ] == 'c' || pParam[ i ] == 'C' ) bCompactData	= TRUE;
				if( pParam[ i ] == 'd' || pParam[ i ] == 'D' ) bMapDetail	= TRUE;
				if( pParam[ i ] == 'r' || pParam[ i ] == 'R' ) bMapRough	= TRUE;
				if( pParam[ i ] == 'o' || pParam[ i ] == 'O' ) bObject		= TRUE;
				if( pParam[ i ] == 't' || pParam[ i ] == 'T' ) bTile		= TRUE;
				if( pParam[ i ] == 's' || pParam[ i ] == 'S' ) bServer		= TRUE;
			}

			TRACE( "�ͽ���Ʈ �ɼ�\n" );
			if( bCompactData	) { TRACE( "bCompactData\n"	); }
			if( bMapDetail		) { TRACE( "bMapDetail\n"	); }
			if( bMapRough		) { TRACE( "bMapRough\n"	); }
			if( bObject			) { TRACE( "bObject\n"		); }
			if( bTile			) { TRACE( "bTile\n"		); }
			if( bServer			) { TRACE( "bServer\n"		); }
			TRACE( "�����\n" );

			ALEF_LOADING_MODE	= nLoadMode;

			// ���� �����ص�..
			g_Const.m_stExportAutomateInfo.bCompactData		= bCompactData	;
			g_Const.m_stExportAutomateInfo.bMapDetail		= bMapDetail	;
			g_Const.m_stExportAutomateInfo.bObject			= bObject		;
			g_Const.m_stExportAutomateInfo.bMapRough		= bMapRough		;
			g_Const.m_stExportAutomateInfo.bTile			= bTile			;
			g_Const.m_stExportAutomateInfo.bServer			= bServer		;
			g_Const.m_stExportAutomateInfo.strFilename		= strFilename	;	
			g_Const.m_stExportAutomateInfo.nDivision		= nDivision		;
		}
	}

	// Constance Loading
	if( g_Const.Load() )
	{
		// Do no op;
	}
	else
	{
		//!! �ӽö�!

		CConstDlg dlg;

		dlg.m_fSectorWidth	= ALEF_SECTOR_WIDTH			;	
		dlg.m_nHoriCount	= ALEF_MAX_HORI_SECTOR		;	
		dlg.m_nVertCount	= ALEF_MAX_VERT_SECTOR		;	
		dlg.m_nDepth		= ALEF_SECTOR_DEFAULT_DEPTH	;

		if( dlg.DoModal() == IDOK )
		{
			ALEF_SECTOR_WIDTH			= dlg.m_fSectorWidth;	
			ALEF_MAX_HORI_SECTOR		= dlg.m_nHoriCount	;	
			ALEF_MAX_VERT_SECTOR		= dlg.m_nVertCount	;	
			ALEF_SECTOR_DEFAULT_DEPTH	= dlg.m_nDepth		;

			if( g_Const.Save() )
			{
				// ����

			}
			else
			{
				// ����1

				MessageBox( NULL , "����� ������ �Ҽ� �����ϴ� ���α׷��� �����մϴ�" , "����" , MB_OK | MB_ICONERROR );
				return FALSE;
			}
			
		}
		else
			return FALSE;
	
	}

	// ������ ���ϰ� �ϱ� ���ؼ�. 
	// Dialog�� ����� �Է��� �޵��� �Ѵ�.
	// ������ ������ �� dialog�� �̿��Ѵ�.
	CMapSelectDlg	msdlg;
	
	msdlg.m_nX_array	= ALEF_LOAD_RANGE_X1 / MAP_DEFAULT_DEPTH;
	msdlg.m_nY_array	= ALEF_LOAD_RANGE_Y1 / MAP_DEFAULT_DEPTH;
	msdlg.m_nX2_array	= ALEF_LOAD_RANGE_X2 / MAP_DEFAULT_DEPTH;
	msdlg.m_nY2_array	= ALEF_LOAD_RANGE_Y2 / MAP_DEFAULT_DEPTH;
	msdlg.m_bUseEffect	= ALEF_USE_EFFECT	;

	if( ALEF_LOADING_MODE != LOAD_NORMAL )
	{
		switch( ALEF_LOADING_MODE )
		{
		case 	LOAD_EXPORT_DIVISION	:
		case	LOAD_EXPORT_LIST		:
			{
				ALEF_LOAD_RANGE_X1	= ( GetDivisionXIndex( g_Const.m_stExportAutomateInfo.nDivision ) + 0 )	* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_Y1	= ( GetDivisionZIndex( g_Const.m_stExportAutomateInfo.nDivision ) + 0 )	* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_X2	= ( GetDivisionXIndex( g_Const.m_stExportAutomateInfo.nDivision ) + 1 )	* MAP_DEFAULT_DEPTH;
				ALEF_LOAD_RANGE_Y2	= ( GetDivisionZIndex( g_Const.m_stExportAutomateInfo.nDivision ) + 1 )	* MAP_DEFAULT_DEPTH;
			}
			break;
		default:
			break;
		}
	}
	else
	if ( msdlg.DoModal( NORMAL_LOADING_SELECT	) == IDOK )
	{
		// ������ ������.. �ʷε����� �̰� ������ϰ� ��.
		ALEF_LOAD_RANGE_X1	= msdlg.m_nX_array		* MAP_DEFAULT_DEPTH;
		ALEF_LOAD_RANGE_Y1	= msdlg.m_nY_array		* MAP_DEFAULT_DEPTH;
		ALEF_LOAD_RANGE_X2	= msdlg.m_nX2_array		* MAP_DEFAULT_DEPTH;
		ALEF_LOAD_RANGE_Y2	= msdlg.m_nY2_array		* MAP_DEFAULT_DEPTH;
		
		ALEF_USE_EFFECT		= msdlg.m_bUseEffect	;

		// ������Ʈ���� ����..
		g_Const.StoreRegistry();
	}
	else
	{
		// ���! -_-+

		return FALSE;
	}
	
	CProgressDlg	progressdlg;
	m_pProgressDlg = & progressdlg;
	progressdlg.StartProgress( "���� �ʱ�ȭ�� .." , 100 , NULL );

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	CString szRegKey;
	szRegKey.LoadString( IDS_REGKEY );
	SetRegistryKey(_T(szRegKey));
	LoadStdProfileSettings();  
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object.
	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	progressdlg.SetProgress( 20 );

	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	progressdlg.SetProgress( 80 );

	pFrame->MoveWindow( 0 , 0 , 1280 , 970 , FALSE );
	
	m_strTitle = "Maptool";
	#ifdef _DEBUG
	m_strTitle += " Debug Build";
	#else
	m_strTitle += " Release Build";
	#endif

	// �� ��� ���� �ɼ� Ȯ��.
	if( AgcmEventNature::IsMaptoolBuild() )
		m_strTitle += " , Nature Enable";
	else
		m_strTitle += " , Nature Disabled";

	// �� ��� ���� �ɼ� Ȯ��.
	if( AgcmRender::IsMaptoolBuild() )
		m_strTitle += " , Render Enable";
	else
		m_strTitle += " , Render Disabled";

	// ���� ����..
	char strFilename[ 1024 ];
	GetModuleFileName( NULL , strFilename , 1024 );
	CFileStatus	fs;
	if( CFile::GetStatus( strFilename , fs ) )
	{
		CString strDate;
		strDate.Format( " %04d/%02d/%02d" , fs.m_mtime.GetYear() , fs.m_mtime.GetMonth() , fs.m_mtime.GetDay() );
		m_strTitle += strDate;
	}

	pFrame->SetWindowText( m_strTitle );

	// The one and only window has been initialized, so show and update it.
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	progressdlg.SetProgress( 100 );
	progressdlg.EndProgress();
	m_pProgressDlg = NULL;

//	CSplashDlg pSplashDlg;
//	pSplashDlg.DoModal();

	// �ε� ��~��.
	ClearZpackCache( L"ini.zp" );
	ReleaseZpackPool();
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMapToolApp message handlers



/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CMapToolApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// Member functions
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//Get the file title from the full path name
void CMapToolApp::ExtractFileTitle(const CString& szFileName, CString& szFileTitle)
{
   //Find the last backslash
   int iPathLength = szFileName.ReverseFind( '\\' );
   szFileTitle = szFileName;
   
   //If the file path has a trailing backslash, then remove it and find the next backslash
   if ( iPathLength >= szFileTitle.GetLength() - 1 )
   {
      szFileTitle = szFileTitle.Mid( 0, szFileTitle.GetLength() - 1 );
      iPathLength = szFileTitle.ReverseFind( '\\' );
   }

   //Extract the filename and return it
   szFileTitle = szFileTitle.Mid( iPathLength + 1, MAX_PATH );
}


//---------------------------------------------------------------------------
//Append a backslash to the end of the path name if there's not one there already
void CMapToolApp::AppendBackslash(CString *szDirName)
{
   szDirName->ReleaseBuffer();
   if ( ! (szDirName->Right(1) == "\\") )
   *szDirName += '\\';
}

//---------------------------------------------------------------------------
CRecentFileList* CMapToolApp::GetRecentFileList()
{
   return m_pRecentFileList;
}


//---------------------------------------------------------------------------
//Callback procedure for browse folder
int CALLBACK CMapToolApp::BrowseProc(HWND hWnd, UINT nMsg, LPARAM, LPARAM lpData)
{
   if (nMsg == BFFM_INITIALIZED)
   {
      //remove trailing slash for SHBrowseForFolder
      int len = szPath.GetLength() - 1;
      if (len != 2 && szPath[len] == _T('\\'))
	      szPath.Delete(len); 

      //intialise the path position to the initial directory
      ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)szPath);
   }
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMapToolApp message handlers

CRenderWare&      CMapToolApp::GetRenderWare()
{
	return m_RenderWare;
}

BOOL CMapToolApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	return m_RenderWare.Idle(); // Idle Time�� ��� �������ֱ� ���ؼ�.
}

void	CMapToolApp::SetStartUpProcess( int nPercent )
{
	ASSERT( NULL != m_pProgressDlg );
	if( NULL == m_pProgressDlg ) return;

	m_pProgressDlg->SetProgress( nPercent );
}

int CMapToolApp::ExitInstance()
{
	if( m_bFailSelecting )
	{
		// ���и� ��Ÿ��.
		return 1;
	}
	else
	{
		return CWinApp::ExitInstance();
	}
}
