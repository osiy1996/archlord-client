#pragma once

// 클라이언트에서만 사용하는 클래스입니다.
#ifndef _WIN64

//////////////////////////////////////////////////////////////////////////
//
// nProtect GameGuard Lib
//     - GameGuard client module
//

#include <ApDefine.h>

//-----------------------------------------------------------------------
//

#ifndef USE_MFC

	#if defined( _AREA_KOREA_ ) || defined( _AREA_GLOBAL_ ) || defined( _AREA_TAIWAN_ )

		#include "../Lib/NProtect/ver_1_0_60/NPGameLib.h"
		#ifndef _DEBUG
			#pragma comment(lib, "NPGameLib.lib")
			#define _USE_NPROTECT_GAMEGUARD_
		#endif

	#elif defined( _AREA_JAPAN_ )

		#include "../Lib/NProtect/ver_1_0_50/NPGameLib.h"
		#ifndef _DEBUG
			#pragma comment(lib, "NPGameLib_MDDLL.lib")
			#define _USE_NPROTECT_GAMEGUARD_
		#endif

	#endif

#endif

//-----------------------------------------------------------------------
//

class AuNPBase
{
public:
	AuNPBase()													{};

	virtual ~AuNPBase()											{};

	virtual BOOL	Init()										{ return TRUE; };
	virtual void	SetWndHandle( HWND hWnd )					{};
	virtual void	Auth( PVOID pggAuthData )					{};
	virtual BOOL	Check()										{ return TRUE; };
	virtual DWORD	Send( LPCTSTR lpszUserId )					{ return 0;	};
	virtual LPCSTR	GetInfo()									{ return NULL; };
	virtual CHAR*	GetMessage( DWORD dwResult )				{ return NULL; };
	virtual BOOL	NPGameMonCallback(DWORD dwMsg, DWORD dwArg)	{ return TRUE; };
};

//-----------------------------------------------------------------------
//

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "winmm.lib")

class CNPGameLib;
class AuNPGameLib : public AuNPBase
{
	CNPGameLib* m_pNPGameLib;

public:
	AuNPGameLib();
	virtual ~AuNPGameLib();

	BOOL	Init();
	void	SetWndHandle( HWND hWnd );
	void	Auth( PVOID pggAuthData );
	BOOL	Check();
	DWORD	Send( LPCTSTR lpszUserId );
	CHAR*	GetMessage(DWORD dwResult);
	BOOL	NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
};

BOOL CALLBACK NPGameMonCallback(DWORD dwMsg, DWORD dwArg);
BOOL CALLBACK NPGameMonSend(DWORD dwMsg);

//-----------------------------------------------------------------------
//

//////////////////////////////////////////////////////////////////////////
//

class AuNPWrapper
{
	AuNPBase* m_pNPGameLib;
	bool inited_;

public:
	virtual ~AuNPWrapper();

	bool	Init();
	void	SetWndHandle( HWND hWnd );
	void	Auth(PVOID pggAuthData);
	DWORD	Send(LPCTSTR lpszUserId);
	BOOL	Check();
	CHAR*	GetMessage(DWORD dwResult);
	BOOL	NPGameMonCallback(DWORD dwMsg, DWORD dwArg);

	friend AuNPWrapper & GetNProtect();

private:
	AuNPWrapper();
};

//-----------------------------------------------------------------------

#endif // !_Win64