#pragma once
//#include "ApBase.h"
#include <windows.h>


//-----------------------------------------------------------------------
//

#define REGIKEY_LOGINSERVERIP		"LoginServerIP"
#define REGIKEY_LOGINSERVERPORT		"LoginServerPort"
#define REGIKEY_LANGUAGE			"Language"
#define REGIKEY_PATHCHCODE			"Code"
#define REGIKEY_VERSION				"Version"

//-----------------------------------------------------------------------
// ������ ������Ʈ�� ������ �����ϴ� Ŭ����
// 2011.02.08 kdi

class AgcmRegistryManager
{
public:
	template< typename Value >
	static Value GetProperty( char const * key );

	~AgcmRegistryManager();

private:

	static AgcmRegistryManager & inst() {
		static AgcmRegistryManager instance;
		return instance;
	}

	AgcmRegistryManager();

	HKEY handle_;
};

//-----------------------------------------------------------------------

#include "../Modules/AClient/AgcmRegistryManager/AgcmRegistryManager.hpp"