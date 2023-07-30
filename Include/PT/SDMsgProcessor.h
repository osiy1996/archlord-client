#pragma once
#include "sdkconfig.h"
#include "sdtalk\SDMessage.h"

class PTSDK_STUFF CSDMsgProcessor
{
public:
	virtual int ProcessMsg(CSDMessage * p_msg, int flag = 0) = 0;
};
