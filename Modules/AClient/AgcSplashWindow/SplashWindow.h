#pragma once

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif


#include <WinSock2.h>
#include <windows.h>
#include <string>
#include "Splash_XBitmap.h"

//-----------------------------------------------------------------------
//

class SplashWindow
{
public:
	SplashWindow( char const * imageFileName, HINSTANCE hinstance = 0 );

	~SplashWindow();

	void stop();

	HWND GetHWnd() { return hwnd_; }

private:
	HINSTANCE hinstance_;
	HWND hwnd_;
	HANDLE stopEvent_;
	HANDLE thread_;
	TCHAR const * name_;
	HFONT font_;
	std::string imageFilename_;
	XBitmap	splashImage_;

	void init_image();

	static unsigned int __stdcall thread_routine( void * inst );

	void run();

	static LRESULT CALLBACK msgproc( HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam );
};

//-----------------------------------------------------------------------