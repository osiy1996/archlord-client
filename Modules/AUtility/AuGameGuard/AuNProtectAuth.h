#pragma once

//////////////////////////////////////////////////////////////////////////
//
// nProtect GameGuard CS Authenticaltion
//     - GameGuard server module
//

#include <ApBase/ApDefine.h>

//-----------------------------------------------------------------------
//

#ifdef _WIN64 // 서버에서만 사용함

	#if defined( _AREA_KOREA_ ) || defined( _AREA_GLOBAL_ ) || defined( _AREA_TAIWAN_ )

		#include "../Lib/NProtect/ver_1_0_60/ggsrv25.h"
		#pragma comment(lib, "ggsrvlib25.lib")
		#define _USE_NPROTECT_GAMEGUARD_

	#elif defined( _AREA_JAPAN_ )

		#include "../Lib/NProtect/ver_1_0_50/ggsrv25.h"
		#pragma comment(lib, "ggsrvlib25_win64_MT.lib")
		#define _USE_NPROTECT_GAMEGUARD_

	#endif

#endif

#ifdef _USE_NPROTECT_GAMEGUARD_

//-----------------------------------------------------------------------
//

class NProtectAuth
{
public:
	NProtectAuth();
	~NProtectAuth();

	static bool Init();
	static bool ServerToClient(CCSAuth2 & auth2, GG_AUTH_DATA & ggData);
};

//-----------------------------------------------------------------------

#endif