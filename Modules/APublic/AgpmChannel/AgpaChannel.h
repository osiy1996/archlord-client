#pragma once

#include <ApAdmin/ApAdmin.h>
#include "AgpdChannel.h"

class AgpaChannel : public ApAdmin
{
public:
	AgpaChannel();
	virtual ~AgpaChannel();

    BOOL Add(TCHAR *szChannelName, AgpdChannel* pChannel);
	AgpdChannel* Get(TCHAR *pszChannelName);
	BOOL Remove(TCHAR *pszChannelName);
};