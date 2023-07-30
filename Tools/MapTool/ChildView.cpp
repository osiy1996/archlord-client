// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"

#pragma warning(disable:4786) //disable linker key > 255 chars long (for stl map)
#include <map>
#include "RenderWare.h"

#include "mapTool.h"

#include "rwcore.h"
#include "rpworld.h"
//#include "AcuRpDWSector.h"
#include "MainWindow.h"

#include "ChildView.h"
#include "MainFrm.h"

#include <AgcEngine.h>
#include "MainWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChildView
/////////////////////////////////////////////////////////////////////////////

int	DeleteFiles( char * path )
{
	// ���丮�� ����� ���ؼ�..
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path �� ������ ã�ƺ���.
	hFind = FindFirstFile( path , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( path , drive, dir, fname, ext );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "DeleteFiles : FindFile Handle Invalid\n" );
		return 0;
	} 
	else 
	{
		wsprintf( strFile , "%s%s%s" , drive , dir , FindFileData.cFileName );

		if( DeleteFile( strFile ) )
			count ++;
		else
		{
			// ����Ʈ ����,.
			char strDebug[ 256 ];
			wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
			OutputDebugString( strDebug );
		}

		while( FindNextFile( hFind , & FindFileData ) )
		{
			wsprintf( strFile , "%s%s%s" , drive , dir , FindFileData.cFileName );

			if( DeleteFile( strFile ) )
				count ++;
			else
			{
				// ����Ʈ ����,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Delete '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}

		// ó�� ��

		return count;
	}
}

int	CopyFiles( char * path , char * pathTarget )
{
	// ���丮�� ����� ���ؼ�..
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	int count = 0;

	// path �� ������ ã�ƺ���.
	hFind = FindFirstFile( path , & FindFileData );

	char	strFile[ 1024 ];
	char	drive[ 256 ] , dir [ 256 ] , fname [ 256 ] , ext[ 256 ];
	_splitpath( path , drive, dir, fname, ext );

	char	strFileTarget[ 1024 ];
	char	drive2[ 256 ] , dir2 [ 256 ] , fname2 [ 256 ] , ext2[ 256 ];
	_splitpath( pathTarget , drive2 , dir2 , fname2 , ext2 );

	if (hFind == INVALID_HANDLE_VALUE) 
	{
		OutputDebugString( "CopyFiles : FindFile Handle Invalid\n" );
		return 0;
	} 
	else 
	{
		wsprintf( strFile		, "%s%s%s" , drive , dir , FindFileData.cFileName );
		wsprintf( strFileTarget	, "%s%s%s" , drive2 , dir2 , FindFileData.cFileName );

		if( CopyFile( strFile , strFileTarget , FALSE ) )
			count ++;
		else
		{
			// ����Ʈ ����,.
			char strDebug[ 256 ];
			wsprintf( strDebug , "Cannot Copy '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
			OutputDebugString( strDebug );
		}

		while( FindNextFile( hFind , & FindFileData ) )
		{
			wsprintf( strFile		, "%s%s%s" , drive , dir , FindFileData.cFileName );
			wsprintf( strFileTarget	, "%s%s%s" , drive2 , dir2 , FindFileData.cFileName );

			if( CopyFile( strFile , strFileTarget , FALSE ) )
				count ++;
			else
			{
				// ����Ʈ ����,.
				char strDebug[ 256 ];
				wsprintf( strDebug , "Cannot Copy '%s' File ( Error code = %d )\n" ,  FindFileData.cFileName , GetLastError() );
				OutputDebugString( strDebug );
			}
		}

		// ó�� ��

		return count;
	}
}

extern MainWindow			g_MainWindow	;

using namespace ChildView;

CChildView::CChildView()
{
	// ������ (2002-06-11 ���� 11:53:36) : ���丮 ����.
	MakeDirectory		();
}

//---------------------------------------------------------------------------

CChildView::~CChildView()
{
}

//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_SIZE()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers
/////////////////////////////////////////////////////////////////////////////

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;
	
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);
	
	return TRUE;
}

//---------------------------------------------------------------------------
int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	this->SetTimer (0, 500,0x0);
	
	CRect rcClient;
	GetClientRect(&rcClient);
	
	//Get the largest client dimension so the renderware client has a ratio of 1:1
	int iMaxDim;
	iMaxDim = ( rcClient.Width() > rcClient.Height() )? rcClient.Width() : rcClient.Height();
	
	//RenderWare initialisation===================================================
	if( !theApp.GetRenderWare().Initialize( this->GetSafeHwnd() ) )
	{
		MessageBox( "RenderWare ���� �ʱ�ȭ ���� : Direct X�� ������ Ȯ���ϼ��� ( 9.0c �̻��̾����" );
		//ExitProcess( -1 );
		PostQuitMessage(-1);
		return -1;
	}

	// ������ (2002-06-20 ���� 3:36:26) : �ؽ��� ����Ʈ �ۼ�.
	LoadTextureList		();
	// �ؽ��Ĵ� ����Ʈ�� �ۼ��صΰ� , ���� �����Ҷ� �ʿ��� �ؽ��ĸ� �޸𸮿� �о���̴� ����� ����Ѵ�.!!!
	
	g_Const.Update();

	return 0;
}


//---------------------------------------------------------------------------
// Tell RenderWare to close down, BEFORE the HWND is destroyed
void CChildView::OnDestroy( void ) 
{
	CWnd::OnDestroy();
	
	return;
}


//---------------------------------------------------------------------------
//Tell RenderWare to paint
void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	//Get a pointer to renderware and the window dimensions
	CRect rcClient;
	this->GetParentFrame()->GetClientRect(rcClient);
}

//---------------------------------------------------------------------------
//Change the navigation view for the next tick frame
void CChildView::OnTimer(UINT nIDEvent) 
{
	// ������ (2002-06-20 ���� 3:15:00) : 
	// OnFirstTime() ó���� �ڵ� 
	{
		static	bool	bFirst = true;
		if( bFirst )
		{
			bFirst = false;
			OnFirstTime();
		}
	}
	CWnd::OnTimer(nIDEvent);
}

//----------------------------------------------------------------------------------
//Tell RenderWare to resize itself accordingly
void CChildView::OnSize(UINT nType, int cx, int cy) 
{
	CWnd ::OnSize(nType, cx, cy);
	
	//Get the maximum dimension of the child window
	CRect rcClient;
	GetClientRect(&rcClient);
	int iMaxDim;
	iMaxDim = ( rcClient.Width() > rcClient.Height() )? rcClient.Width() : rcClient.Height();
	//Assing the viewport as a square to renderware
	//theApp.GetRenderWare().SetCameraViewSize(iMaxDim, iMaxDim);
}



BOOL CChildView::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_MOUSEWHEEL )
	{
		theApp.GetRenderWare().MessageProc( pMsg );
		return TRUE;
	}
	else
	if( pMsg->message == WM_KEYDOWN		||
		pMsg->message == WM_KEYUP		)
	{
		// ���� Ű�� �Է��� ���� �ʾƼ� ( ��򰡿��� �Ծ������ �ִµ��ϴ� )
		// �ε����ϰ� ���⼭ ������ ���ѹ���
		switch( pMsg->wParam )
		{
		case 'C'		:
		case 'W'		:
		case 'A'		:
		case 'S'		:
		case 'D'		:
		case 'Q'		:
		case 'E'		:
		case 'T'		:
		case VK_TAB		:
				/********************* Parn���� ����ٰ� VK_DEL �־���. **********/
		case VK_DELETE	:
		case VK_LEFT	:
		case VK_UP		:
		case VK_RIGHT	:
		case VK_DOWN	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return TRUE;
		case 'Z'		:
			{
				if( ISBUTTONDOWN( VK_CONTROL ) ) 
					break;
				else 
					theApp.GetRenderWare().MessageProc( pMsg );
				return TRUE;
			}
		case VK_SPACE	:
			{
				theApp.GetRenderWare().MessageProc( pMsg );
			}
			return TRUE;
		default:
			// do no op
			break;
		}
		
	}
	return CWnd ::PreTranslateMessage(pMsg);
}

void CChildView::MakeDirectory()
{
	// ���� ���丮����.

	// ����������..
	CreateDirectory( "map"					, NULL );
	CreateDirectory( "map\\temp"			, NULL );
	CreateDirectory( "map\\temp\\rough"		, NULL );
	CreateDirectory( "map\\data"			, NULL );
	CreateDirectory( "map\\data\\blocking"	, NULL );
	CreateDirectory( "map\\data\\geometry"	, NULL );
	CreateDirectory( "map\\data\\moonee"	, NULL );
	CreateDirectory( "map\\data\\compact"	, NULL );
	CreateDirectory( "map\\data\\grass"		, NULL );
	CreateDirectory( "map\\data\\water"		, NULL );
	CreateDirectory( "map\\tile"			, NULL );
	CreateDirectory( "map\\sky"				, NULL );
	CreateDirectory( "world"				, NULL );
	CreateDirectory( "texture\\world"		, NULL );

	// ���� ���� �����..
	CreateDirectory( "ini"					, NULL );
	CreateDirectory( "ini\\object"			, NULL );
	CreateDirectory( "ini\\object\\design"	, NULL );

	// ������ (2004-07-05 ���� 3:27:53) : �߰�~
	CreateDirectory( "map\\data\\object"			, NULL );
	CreateDirectory( "map\\data\\object\\artist"	, NULL );
	CreateDirectory( "map\\data\\object\\design"	, NULL );
	CreateDirectory( "map\\data\\object\\pointlight", NULL );
	
	CreateDirectory( "map\\subdata"			, NULL );
}

void CChildView::OnFirstTime( void )
{
	// �ʼ���â ���
	g_MainWindow.LoadPolygonData();

	CRect rcClient;
	GetClientRect(&rcClient);
	int iMaxDim;
	iMaxDim = (rcClient.Width() > rcClient.Height())? rcClient.Width() : rcClient.Height() ;
	theApp.GetRenderWare().SetCameraViewSize( rcClient.Width() , rcClient.Height() );
}

void CChildView::LoadTextureList		( void )	// ���丮 ������ �о ������ã�Ƴ�..
{
	// ��������Ʈ ����.
	// �̹� ���Ű� �ִ� �ؽ��ĵ��� ������ ������ ���� ���ɼ��� ����� ����.
//	g_MainWindow.m_TextureList.DeleteAll();
//
//	// ����Ʈ �ۼ�.
//	char filename [ 1024 ];
//	wsprintf( filename , "%s\\%s" , ALEF_CURRENT_DIRECTORY , ALEF_TILE_LIST_FILE_NAME );
//	if( g_MainWindow.m_TextureList.LoadScript( filename ) )
//	{
//		// ����
//		// ���� ���λ���?..
//	}
//	else
//	{
//		// ����.
//		MessageBox( "Ÿ�� ��ũ��Ʈ ���Ͽ� ������ �ֽ��ϴ�." );
//	}
}

LRESULT CChildView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	MSG messagestruct;
	messagestruct.hwnd		= GetSafeHwnd();
	messagestruct.message	= message;
	messagestruct.wParam	= wParam;
	messagestruct.lParam	= lParam;
	
	if( theApp.GetRenderWare().MessageProc( & messagestruct ) ) return TRUE;

	switch( message )
	{
	case WM_COMMAND:
		{
			int	menuid = LOWORD( wParam );

//			if( menuid >= IDM_OBJECT_POPUP_OFFSET && menuid < IDM_OBJECT_POPUP_OFFSET + g_MainWindow.m_listClumpSorted.GetCount() )
//			{
//				g_MainWindow.PopupMenuSelect( menuid - IDM_OBJECT_POPUP_OFFSET );
//			}
//			else
			if( menuid == IDM_OBJECT_PROPERTY		||
				menuid == IDM_OBJECT_EVENT			||
				menuid == IDM_OBJECTPOP_GROUPSAVE	||
				menuid == IDM_OBJECTPOP_OCTREEROOTDISPLAY )
			{
				g_MainWindow.PopupMenuObject(menuid);
			}
		}
		break;	
	default:
		break;
	}
	return CWnd ::WindowProc(message, wParam, lParam);
}
