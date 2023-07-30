#ifndef	__AGPDBILLINFO_H__
#define	__AGPDBILLINFO_H__

#include <ApBase/ApBase.h>

typedef enum	_AgpdPaymentType {
	AGPD_PAYMENT_NONE					= 0,
	AGPD_PAYMENT_SUBSCRIPTION,
	AGPD_PAYMENT_TIME_COUPON,
	AGPD_PAYMENT_PC_ROOM,
	AGPD_PAYMENT_MAX,
} AgpdPaymentType;

class CashInfoGlobal 
{
public:
	DWORD m_dwAccountGUID;
	double m_WCoin;
	double m_PCoin;

	CashInfoGlobal()
	{
		m_dwAccountGUID = -1;
		m_WCoin = 0;
		m_PCoin = 0;
	}
};

enum AgpdPCRoomType
{
	AGPDPCROOMTYPE_NONE				= 0x00000000,
	AGPDPCROOMTYPE_HANGAME_S		= 0x00000001,
	AGPDPCROOMTYPE_HANGAME_TPACK	= 0x00000002,
	AGPDPCROOMTYPE_WEBZEN_GPREMIUM	= 0x00000004,
};

class AgpdBillInfo {
public:
	AgpdPaymentType		m_ePaymentType;
	UINT32				m_ulRemainCouponPlayTime;

	BOOL				m_bIsPCRoom;
	UINT32				m_ulPCRoomType;

	CashInfoGlobal		m_CashInfoGlobal;
};

#endif	//__AGPDBILLINFO_H__