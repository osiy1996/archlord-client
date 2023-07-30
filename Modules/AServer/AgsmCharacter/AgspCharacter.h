#ifndef _AGSP_CHARACTER_H
#define _AGSP_CHARACTER_H

#include "AgsdRelay2.h"

struct PACKET_AGSP_CHARACTER : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;
	CHAR	Flag3;
	CHAR	Flag4;

	INT8	pcOperation;
	INT16	nParam;

	PACKET_AGSP_CHARACTER()
		: pcOperation(0), Flag1(1), Flag2(0), Flag3(0), Flag4(0)
	{
		cType			= AGSMRELAY_PACKET_TYPE;
		nParam			= AGSMRELAY_PARAM_COUPON;
		unPacketLength	= (UINT16)sizeof(PACKET_AGSP_CHARACTER);
	}
};

/************************************************************************/
/* Coupon Packet                                                        */
/************************************************************************/

const INT32	COUPON_ITEM_MAX				= 10;

enum eAgspCouponPacketOperation
{
	AGSP_CHARACTER_REQUEST_COUPON_USE	=	0,
	AGSP_CHARACTER_ANSWER_COUPON_USE,
};

struct CouponItem
{
	INT32		m_lItemTID;
	INT32		m_lCount;
};

// GameServer -> RelayServer
struct PACKET_AGSP_CHARACTER_REQUEST_COUPON_USE : public PACKET_AGSP_CHARACTER
{
	CHAR			m_szCharName[32 + 1];		
	stCouponInfo	stRequestCoupon;

	PACKET_AGSP_CHARACTER_REQUEST_COUPON_USE()
	{
		pcOperation = AGSP_CHARACTER_REQUEST_COUPON_USE;
		ZeroMemory(m_szCharName, sizeof(m_szCharName));
		ZeroMemory(&stRequestCoupon, sizeof(stRequestCoupon));
	}
};

// RelayServer -> GameServer
struct PACKET_AGSP_CHARACTER_ANSWER_COUPON_USE : public PACKET_AGSP_CHARACTER
{
	INT32			m_lResultCode;
	CouponItem		m_lItemInfo[COUPON_ITEM_MAX];
	CHAR			m_szCharName[32 + 1];

	PACKET_AGSP_CHARACTER_ANSWER_COUPON_USE()
		: m_lResultCode(0)
	{
		pcOperation	= AGSP_CHARACTER_ANSWER_COUPON_USE;
		ZeroMemory(m_lItemInfo, sizeof(m_lItemInfo));
		ZeroMemory(m_szCharName, sizeof(m_szCharName));
	}
};

#endif