#ifndef _AGSP_CASHITEMTIMEEXTEND_H
#define _AGSP_CASHITEMTIMEEXTEND_H

#include "AgsdRelay2.h"

struct PACKET_AGSMCASHITEMTIMEEXTEND_RELAY : public PACKET_HEADER
{
	CHAR	FlagLow;

	INT16	nParam;
	INT16 nOperation;
	
	PACKET_AGSMCASHITEMTIMEEXTEND_RELAY()
		: nParam(0), FlagLow(1), nOperation(0)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSMCASHITEMTIMEEXTEND_RELAY);
	}
};

enum eCashItemTimeExtendRelayType
{
	AGSMRELAY_PARAM_EXTEND_REQUEST = 1,
	AGSMRELAY_PARAM_EXTEND_RESULT,
};

struct PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY : public PACKET_AGSMCASHITEMTIMEEXTEND_RELAY
{
	UINT32 ulExtendTime;

	PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY(UINT32 ExtendTime)
	{
		nParam = AGSMRELAY_PARAM_CASHITEMTIMEEXTEND;
		nOperation = AGSMRELAY_PARAM_EXTEND_REQUEST;

		ulExtendTime = ExtendTime;

		unPacketLength	= (UINT16)sizeof(PACKET_AGSMCASHITEMTIMEEXTEND_REQUEST_RELAY);
	}
};

struct PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY : public PACKET_AGSMCASHITEMTIMEEXTEND_RELAY
{
	UINT32 ulExtendTime;
	INT32 nResult;

	PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY()
	{
		nParam = AGSMRELAY_PARAM_CASHITEMTIMEEXTEND;
		nOperation = AGSMRELAY_PARAM_EXTEND_RESULT;

		unPacketLength	= (UINT16)sizeof(PACKET_AGSMCASHITEMTIMEEXTEND_RESULT_RELAY);
	}
};

#endif //_AGSP_CASHITEMTIMEEXTEND_H