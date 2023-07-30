#define _WINSOCKAPI_
#include "SplashWindow.h"
#include <AuPackingManager/AuPackingManager.h>
#include <process.h>

static HWND shWndCurWnd = 0;

//-----------------------------------------------------------------------
//

struct MemoryBuffer
{
	BYTE * buf_;
	unsigned int size_;

	MemoryBuffer( BYTE * buf = 0, unsigned int size = 0 ) : buf_( buf ), size_( size ) {}
	MemoryBuffer( MemoryBuffer & other ) : buf_( other.buf_ ), size_( other.size_ ) { other.buf_ = 0; other.size_ = 0; }
	~MemoryBuffer() { if( buf_ ) delete[] buf_; }

private:
	void operator=( MemoryBuffer const & other );
};

typedef std::auto_ptr< MemoryBuffer > MemoryBufferPtr;

//-----------------------------------------------------------------------
//

MemoryBufferPtr LoadPngResource( const char * filename )
{
	MemoryBufferPtr result;

	if( !filename )
		return result;

	BOOL	bPackingFile		=	TRUE;

	AuPackingManager* cPackingManager = AuPackingManager::GetSingletonPtr();
	cPackingManager->LoadReferenceFile(NULL);

	ApdFile		csApdFile;

	if( !cPackingManager->OpenFile( (char*)filename , &csApdFile) )
	{	
		return result;
	}

	UINT32		uLength = cPackingManager->GetFileSize( &csApdFile );
	BYTE		*pBuffer = new BYTE[uLength];
	ZeroMemory( pBuffer , uLength );


	if( cPackingManager->ReadFile( pBuffer, uLength, &csApdFile ) != uLength )
	{		
		cPackingManager->CloseFile(&csApdFile);
		return result;
	}

	cPackingManager->CloseFile(&csApdFile);

	result.reset( new MemoryBuffer( pBuffer, uLength ) );

	return result;
}

//-----------------------------------------------------------------------
//

BOOL CALLBACK cbfEnumWindowsProc(  HWND hWnd, LPARAM lParam )
{
	shWndCurWnd = 0;
	
	DWORD PID=0, TID=0;
	TID = ::GetWindowThreadProcessId (hWnd, &PID);

	if( (DWORD)lParam == PID){
		shWndCurWnd=hWnd;
		return FALSE;
	}

	return TRUE;
}

HWND GetWindowHandleFromProcessId( DWORD dwProcId )
{
	shWndCurWnd = 0;

	BOOL b = EnumWindows( cbfEnumWindowsProc,  dwProcId );

	if( (b==FALSE) &&(shWndCurWnd!=0)){
		return shWndCurWnd;
	}

	return 0;
}

HINSTANCE GetCurrentWindowInstance()
{
	HWND hwnd = GetWindowHandleFromProcessId( GetCurrentProcessId() );
	return (HINSTANCE)GetWindowLong( hwnd, GWL_HINSTANCE ); 
}

//////////////////////////////////////////////////////////////////////////
//

SplashWindow::SplashWindow( char const * imageFilename, HINSTANCE hinstance ) 
	: hinstance_(hinstance == 0 ? GetCurrentWindowInstance() : hinstance)
	, hwnd_(0)
	, stopEvent_( CreateEvent(0,0,0,0) )
	, name_(TEXT("Please Wait"))
	, thread_(0)
	, imageFilename_( imageFilename ? imageFilename : "" )
{
	thread_ = (HANDLE)_beginthreadex( 0, 0, thread_routine, this, 0, 0 );
}

//////////////////////////////////////////////////////////////////////////
//

SplashWindow::~SplashWindow()
{
	stop();

	if( stopEvent_ )
		CloseHandle(stopEvent_);

	if( thread_ )
		CloseHandle( thread_ );
}

//////////////////////////////////////////////////////////////////////////
//

void SplashWindow::stop()
{
	if( stopEvent_ )
	{
		SetEvent( stopEvent_ );
	}

	if( thread_ )
		WaitForSingleObject( thread_, INFINITE );
}

//-----------------------------------------------------------------------
//

void SplashWindow::init_image()
{
	MemoryBufferPtr memoryBuffer = LoadPngResource( imageFilename_.c_str() );

	if( !memoryBuffer.get() || splashImage_.LoadPNG( memoryBuffer->buf_, memoryBuffer->size_ ) == FALSE )
		return;
}

//////////////////////////////////////////////////////////////////////////
//

void SplashWindow::run()
{
	WNDCLASS wndClass;
	ZeroMemory( &wndClass, sizeof(wndClass) );

	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hinstance_;
	wndClass.lpfnWndProc = (WNDPROC)msgproc;
	wndClass.lpszClassName = name_;
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass( &wndClass );

	init_image();

	int h = splashImage_.Height();
	int w = splashImage_.Width();

	int x = GetSystemMetrics(SM_CXSCREEN)/2 - w/2;
	int y = GetSystemMetrics(SM_CYSCREEN)/2 - h/2;

	hwnd_ = CreateWindowEx(  WS_EX_TOOLWINDOW | WS_EX_LAYERED | WS_EX_TOPMOST, name_, TEXT(""), WS_POPUP | WS_OVERLAPPED, x, y, w, h, 0, (HMENU)0, hinstance_, 0 );


	splashImage_.ApplyAlpha2RGB4LayerdWindow();

	SetWindowPos( hwnd_, HWND_TOPMOST, x, y, w, h, 0 );

	HDC dcDst = NULL;
	HDC dcImg = splashImage_.GetDC();

	POINT ptDest = {0,0};
	POINT ptSrc = {0,0};

	SIZE szDest = {w, h};

	BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };

	BOOL ret = ::UpdateLayeredWindow(hwnd_, dcDst, NULL, &szDest, dcImg, &ptSrc, RGB(255,255,255), &bf, ULW_ALPHA);

	//if(ret==FALSE)
	//	return;

	ShowWindow( hwnd_, SW_SHOW );
	UpdateWindow( hwnd_ );

	MSG message;

	while( GetMessage( &message, 0,0,0 ) )
	{
		if( WaitForSingleObject( stopEvent_, 0 ) == WAIT_OBJECT_0 )
			break;

		TranslateMessage( &message );
		DispatchMessage( &message );
	}
}

//////////////////////////////////////////////////////////////////////////
//

unsigned int __stdcall SplashWindow::thread_routine( void * inst )
{
	SplashWindow * pthis = (SplashWindow*)inst;
	pthis->run();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
//

LRESULT CALLBACK SplashWindow::msgproc( HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam )
{
	switch( iMessage )
	{
	case WM_CREATE:
		SetTimer( hwnd, 0, 10, 0 );
		break;
	case WM_TIMER:
		InvalidateRect(hwnd, NULL, FALSE);
		break;

	case WM_PAINT:
		break;

	case WM_DESTROY :
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc( hwnd, iMessage, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////////