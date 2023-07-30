#include <AgcAuth/AgcAuth.h>
#include "AgcGPAuth.h"
#include "AgcHanGameAuth.h"
#include "AgcJapanAuth.h"
#include "AgcTaiwanAuth.h"

//-----------------------------------------------------------------------
//

AgcAuth & AuthManager()
{
#if defined( _AREA_KOREA_ )
	static AgcHanGameAuth inst;
#elif defined( _AREA_JAPAN_ )
	static AgcJapanAuth inst;
#elif defined( _AREA_GLOBAL_ )
	static AgcGPAuth inst;
#elif defined( _AREA_TAIWAN_ )
	static AgcTaiwanAuth inst;
#else
	static AgcAuth inst;
#endif

	return inst;
}

//-----------------------------------------------------------------------