#pragma once
#include "sdkconfig.h"

class PTSDK_STUFF CConfigInterface
{
public:
	virtual int GetConfigInt(
		const char * lpAppName,
		const char * lpKeyName,
		int nDefault )=0;

	virtual int GetConfigString(
		const char * lpAppName,
		const char * lpKeyName,
		const char * lpDefault,
		char *		 lpReturnedString,
		int			 nSize )=0;
};