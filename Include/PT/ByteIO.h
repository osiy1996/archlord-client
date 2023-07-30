#pragma once
#include "sdkconfig.h"

class PTSDK_STUFF CByteIO
{
public:
	virtual bool IsValuable()	= 0;
	virtual bool CanSend(int time)	= 0;
	virtual bool CanRecv(int time)	= 0;
	virtual int  Send(const char * buf, int bufLen) = 0;
	virtual int  Recv(char * buf, int bufLen) = 0;
};
