#pragma once

#include <ApBase/ApDefine.h>

#define	AGPMCASHMALL_MIN_INTERVAL_REFRESH_CASH		1000

typedef struct _AgpdCashMall
{
	UINT32		m_ulLastRefreshCashTimeMsec;
} AgpdCashMall;