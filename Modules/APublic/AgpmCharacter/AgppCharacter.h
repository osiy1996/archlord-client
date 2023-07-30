#ifndef AGPPCHARACTER_H
#define AGPPCHARACTER_H

//////////////////////////////////////////////////////////////////////////////
//

#define MAX_PINCH_WANTED_CHARACTER		5

struct PACKET_CHARACTER : public PACKET_HEADER
{
	CHAR	Flag1;
	CHAR	Flag2;
	CHAR	Flag3;
	CHAR	Flag4;

	INT8	pcOperation;

	PACKET_CHARACTER()
		: pcOperation(0), Flag1(1), Flag2(0), Flag3(0), Flag4(0)
	{
		cType			= AGPMCHARACTER_PACKET_TYPE;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER);
	}
};

struct PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST : public PACKET_CHARACTER
{
	INT32	lUserID;
	INT32   lRequestCharacterID;

	PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST()
		: lUserID(0), lRequestCharacterID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_PINCHWANTED_CHARACTER;
		unPacketLength  = (UINT16)sizeof(PACKET_CHARACTER_PINCHWANTED_CHAR_REQUEST);
	}
};

struct PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER : public PACKET_CHARACTER
{
	INT32   lPinchCharacterID;
	INT32	lPinchWantedCharacter[MAX_PINCH_WANTED_CHARACTER];

	PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER()
		: lPinchCharacterID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATUIN_ANSWER_PINCHWANTED_CHARACTER;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER_PINCHWANTED_CHAR_ANSWER);
		ZeroMemory(lPinchWantedCharacter, sizeof(lPinchWantedCharacter));
	}
};

// Coupon System

const int AGPPACCOUNT_MAX_COUPONNO				= 64;
const int AGPPACCOUNT_MAX_COUPON_PACKET			= 10;
const int AGPPACCOUNT_MAX_COUPON_TITLE			= 128;

struct stCouponInfo
{
	INT32		m_lCouponType;
	CHAR		m_szCouponNo[AGPPACCOUNT_MAX_COUPONNO + 1];
	CHAR		m_szCouponTitle[AGPPACCOUNT_MAX_COUPON_TITLE + 1];
};

/************************************************************************/
/* Inquire Coupon Info                                                  */
/************************************************************************/

// Client -> Server
struct PACKET_CHARACTER_COUPON_REQUEST : public PACKET_CHARACTER
{
	INT32		m_lCID;

	PACKET_CHARACTER_COUPON_REQUEST()
		: m_lCID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER_COUPON_REQUEST);
	}
};

// Server -> Client
struct PACKET_CHARACTER_COUPON_INFO : public PACKET_CHARACTER
{
	INT32			m_lTotalCount;
	stCouponInfo	m_stCouponInfo[AGPPACCOUNT_MAX_COUPON_PACKET];

	PACKET_CHARACTER_COUPON_INFO()
		: m_lTotalCount(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_COUPON_INFO;
		unPacketLength  = (UINT16)sizeof(PACKET_CHARACTER_COUPON_INFO);
		ZeroMemory(m_stCouponInfo, sizeof(m_stCouponInfo));
	}
};

/************************************************************************/
/* Request Use Coupon                                                   */
/************************************************************************/

// Client -> Server
struct PACKET_CHARACTER_REQUEST_COUPON_USE : public PACKET_CHARACTER
{
	INT32				m_lCID;
	stCouponInfo		m_stRequestCoupon;

	PACKET_CHARACTER_REQUEST_COUPON_USE()
		: m_lCID(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_REQUEST_COUPON_USE;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER_REQUEST_COUPON_USE);
		ZeroMemory(&m_stRequestCoupon, sizeof(m_stRequestCoupon));
	}
};

// Server -> Client
struct PACKET_CHARACTER_ANSWER_COUPON_USE : public PACKET_CHARACTER
{
	INT32				m_lResultCode;
	
	PACKET_CHARACTER_ANSWER_COUPON_USE()
		: m_lResultCode(0)
	{
		pcOperation		= AGPMCHAR_PACKET_OPERATION_ANSWER_COUPON_USE;
		unPacketLength	= (UINT16)sizeof(PACKET_CHARACTER_ANSWER_COUPON_USE);
	}
};

#endif