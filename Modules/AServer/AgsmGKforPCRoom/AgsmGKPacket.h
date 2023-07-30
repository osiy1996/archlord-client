#ifndef	__AGSMGKPACKET_H__
#define	__AGSMGKPACKET_H__

#include "ApDefine.h"
#include "ApPacket.h"

const int	GK_PACKET_COMMAND_AUTH					= 4000;
const int	GK_PACKET_COMMAND_AUTH_RESULT			= 4005;
const int	GK_PACKET_COMMAND_LOGOUT				= 4001;
const int	GK_PACKET_COMMAND_LOGOUT_RESULT			= 4008;
const int	GK_PACKET_COMMAND_BILL_AUTH				= 4004;
const int	GK_PACKET_COMMAND_BILL_AUTH_RESULT		= 4006;
const int	GK_PACKET_COMMAND_WARN_NOTICE			= 4003;
const int	GK_PACKET_COMMAND_CUT_IP				= 4002;
const int	GK_PACKET_COMMAND_PING_AUTH				= 4007;

typedef struct	_GK_PACKET_AUTH {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
} GK_PACKET_AUTH, *PGK_PACKET_AUTH;

typedef struct	_GK_PACKET_AUTH_RESULT {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
	UINT16				unResult;
} GK_PACKET_AUTH_RESULT, *PGK_PACKET_AUTH_RESULT;

typedef struct	_GK_PACKET_LOGOUT {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
	CHAR				szOuttime[18];
} GK_PACKET_LOGOUT, *PGK_PACKET_LOGOUT;

typedef struct	_GK_PACKET_LOGOUT_RESULT {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
	UINT16				unResult;
} GK_PACKET_LOGOUT_RESULT, *PGK_PACKET_LOGOUT_RESULT;

typedef struct	_GK_PACKET_BILL_AUTH {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
} GK_PACKET_BILL_AUTH, *PGK_PACKET_BILL_AUTH;

typedef struct	_GK_PACKET_BILL_AUTH_RESULT {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
	UINT16				unResult;
} GK_PACKET_BILL_AUTH_RESULT, *PGK_PACKET_BILL_AUTH_RESULT;

typedef struct	_GK_PACKET_WARN_NOTICE {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
	CHAR				szMessage[70];
} GK_PACKET_WARN_NOTICE, *PGK_PACKET_WARN_NOTICE;

typedef struct	_GK_PACKET_CUT_IP {
	LK_PACKET_HEADER	stHeader;

	CHAR				szIP[17];
	CHAR				szID[35];
} GK_PACKET_CUT_IP, *PGK_PACKET_CUT_IP;

typedef struct	_GK_PACKET_KEEP_ALIVE {
	LK_PACKET_HEADER	stHeader;
} GK_PACKET_KEEP_ALIVE, *PGK_PACKET_KEEP_ALIVE;

#endif	//__AGSMGKPACKET_H__